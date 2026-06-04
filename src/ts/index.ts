import { ClayConstructor, Clay as ClayInstance } from "../lib/Clay";
import { buildClayConfig } from "./clayConfig";
// @ts-ignore
const Clay: ClayConstructor = require("@rebble/clay");

import {
  getConfig,
  setConfig,
  booleanToCategories,
  booleanToSections,
  generateClaySettings,
  booleanToFeeds,
} from "./config";
import {
  healthRequest,
  possibleCategoriesRequest,
} from "./server/request/metaRequests";
import * as handlers from "./handlers";
import {
  additionalFeeds,
  availableCategories,
  setAvailableCategories,
  availableSections,
  defaultCategories,
  defaultSections,
  StoryDetailEnum,
  AvailableCategory,
} from "./types";
import {
  createLogger,
  initDebugMode,
  setDebugMode,
  isDebugMode,
} from "../lib/logger";
import {
  filterInterfaceStringOptionals,
  flattenInterfaceStrings,
  getInterfaceStrings,
} from "./localization";
import { setServerLang } from "./server/localeManager";
import { updateCachedBatchInfo } from "./server/cache";
import { batchesRequest } from "./server/request/batchRequests";
import * as timeline from "./timeline";
import {
  sendAppMessageWithSession,
  setSessionId,
} from "./sessionBasedAppMessage";
import { getCategoryBatchMap } from "./api";

const log = createLogger("KNJS");

const LOADING_MILESTONES = {
  READY: 5,
  POSSIBLE_CATEGORIES: 15,
  DEBUG_MODE: 25,
  BATCH_INFO: 40,
  CONFIG: 55,
  INTERFACE_STRINGS: 65,
  TENSION: 95,
} as const;

// The percentage sent is arbitrary based on what seems right for how long each step takes, and is only meant to give a rough sense of progress rather than be exact.
const sendStartupLoadingState = (() => {
  let lastSent = 0;
  return async (current: number) => {
    if (current > lastSent) {
      lastSent = current;
      return sendAppMessageWithSession({
        type: "loading_state",
        loadingCurrent: current,
        loadingMax: 100,
      });
    }
  };
})();

// Mutable Clay instance that is updated based on runtime fetches of Kagi News data
let clay: ClayInstance = new Clay(buildClayConfig(defaultCategories), null, {
  autoHandleEvents: false,
});

const refreshPossibleCategories = async (): Promise<void> => {
  const result = await possibleCategoriesRequest();
  if (result.names.length > 0) {
    setAvailableCategories(result.names, result.displayNamesMap);
    log.info(
      `Fetched ${availableCategories.length} categories from metadata API`,
    );
  } else {
    setAvailableCategories(defaultCategories);
    log.error(
      "Failed to fetch categories from metadata API, using fallback list",
    );
  }
  clay = new Clay(buildClayConfig(availableCategories), null, {
    autoHandleEvents: false,
  });
};

Pebble.addEventListener("ready", async () => {
  setSessionId(Date.now().toString());
  initDebugMode();
  log.info("PebbleKit JS ready");

  await sendAppMessageWithSession({
    type: "provide_session_id",
  });

  await sendStartupLoadingState(LOADING_MILESTONES.READY);

  // Refresh possible categories from Kagi News API
  await refreshPossibleCategories();
  await sendStartupLoadingState(LOADING_MILESTONES.POSSIBLE_CATEGORIES);

  await sendAppMessageWithSession({
    type: "set_debug_mode",
    isDebugMode: isDebugMode() ? 1 : 0,
  });
  await sendStartupLoadingState(LOADING_MILESTONES.DEBUG_MODE);

  try {
    const response = await healthRequest();
    if (response.health === true) {
      log.info("Kagi News API is healthy");
      const batchInfo = await batchesRequest();
      log.info(
        `Current batch ID: ${batchInfo.id}, created at ${batchInfo.createdAt}`,
      );
      updateCachedBatchInfo(batchInfo);
      await sendStartupLoadingState(LOADING_MILESTONES.BATCH_INFO);

      const newsRefreshPinEnabled = getConfig("newsRefreshPinSetting");
      if (newsRefreshPinEnabled) {
        timeline.pushNewsRefreshPinToTimeline();
      }

      const textSize = getConfig("selectedTextSize");
      await sendAppMessageWithSession({
        type: "set_text_size",
        textSize: textSize,
      });

      const contentLanguage = getConfig("selectedContentLanguage");
      setServerLang(contentLanguage);

      const maxStoryCount = getConfig("selectedMaxStoryCount");
      handlers.setMaxStoryCount(maxStoryCount);

      const selectedSectionNames = getConfig("selectedSectionNames");
      handlers.setSelectedSections(selectedSectionNames);

      const selectedCategoryNames = getConfig("selectedCategoryNames");
      handlers.setSelectedCategories(selectedCategoryNames);

      const selectedAdditionalFeeds = getConfig("selectedAdditionalFeeds");
      handlers.setAdditionalFeeds(...selectedAdditionalFeeds);

      const showTensionIndex = getConfig("isTensionIndexEnabled");
      await sendStartupLoadingState(LOADING_MILESTONES.CONFIG);

      const interfaceLanguage = getConfig("selectedInterfaceLanguage");
      const sendInterfaceStrings = async (categoryKeys: string[]) => {
        const strings = flattenInterfaceStrings(
          filterInterfaceStringOptionals({
            strings: getInterfaceStrings(interfaceLanguage),
            sectionKeys: selectedSectionNames ?? defaultSections,
            categoryKeys,
            featureKeys: selectedAdditionalFeeds ?? defaultCategories,
            lang: interfaceLanguage,
          }),
        );
        log.debug("Sending interface strings:", JSON.stringify(strings));
        await sendAppMessageWithSession({
          type: "send_interface_strings",
          interfaceStrings: JSON.stringify(strings),
        });
      };

      await sendInterfaceStrings(selectedCategoryNames ?? defaultCategories);
      await sendStartupLoadingState(LOADING_MILESTONES.INTERFACE_STRINGS);

      // Categories must update first before tension. Both should be finished before notifying the watch
      await handlers.handleUpdateCategories((completedGroups, totalGroups) => {
        const categoryProgress =
          LOADING_MILESTONES.INTERFACE_STRINGS +
          Math.round(
            (completedGroups / totalGroups) *
              (LOADING_MILESTONES.TENSION -
                LOADING_MILESTONES.INTERFACE_STRINGS),
          );
        void sendStartupLoadingState(categoryProgress);
      });

      // TODO: This should be cleaned up a lot. I don't like that I have to repeat the additionalFeeds removal here.
      // Re-send with only categories that actually have stories today; empty categories
      // (e.g. "Bitcoin" on a slow news day) would otherwise occupy a localization slot
      // and shift every subsequent category's display name by one position on the watch.
      // Additional feeds should be removed here
      const actualCategoryNames = getCategoryBatchMap()
        .categorizedStories.map((cs) => cs.category.name)
        .filter(
          (name) => !(additionalFeeds as readonly string[]).includes(name),
        );
      await sendInterfaceStrings(actualCategoryNames);

      await handlers.updateTensionIndex(showTensionIndex);
      await sendStartupLoadingState(LOADING_MILESTONES.TENSION);

      log.info("Notifying watch that data is ready");
      await sendAppMessageWithSession({
        type: "update_categories",
        state: "success",
      });
    } else {
      log.warn("Kagi News API is unhealthy");
    }
  } catch (err) {
    log.error("Something went wrong with initial setup", err);
  }
});

Pebble.addEventListener("showConfiguration", async () => {
  try {
    // Refresh possible categories from Kagi News API
    await refreshPossibleCategories();

    const claySettings = generateClaySettings(isDebugMode());
    log.debug("Generated Clay settings:", claySettings);
    localStorage.setItem("clay-settings", JSON.stringify(claySettings));
    Pebble.openURL(clay.generateUrl());
  } catch (e) {
    log.error("Failed to open Clay configuration", e);
    log.error("Clay config error:", e);
  }
});

// @ts-ignore
Pebble.addEventListener("webviewclosed", async (event: any) => {
  // Dump the entire cache if Clay is closed to avoid problems
  await handlers.clearNetworkCache({ repopulateBatchId: true });

  try {
    if (!event || !event.response) return;

    // TODO: Handling settings changes should be consolidated so it can be the same whether the app first starts or when config changes
    const settings = clay.getSettings(event.response);
    log.debug("Clay settings received:", JSON.stringify(settings));
    // DebugMode is at key 10406 (after all category and section checkbox keys)
    const debugModeKey = "10406";
    const rawDebugValue = settings[debugModeKey];
    const debugModeValue = rawDebugValue === true || rawDebugValue === 1;

    setDebugMode(debugModeValue);

    // Categories are at keys 10000-10199
    const newSelected: boolean[] = availableCategories.map((_, idx) => {
      const key = (10000 + idx).toString();
      const val = settings[key];
      return val === true || val === 1;
    });
    handlers.setSelectedCategoriesFromBoolean(newSelected);
    const selectedNames = booleanToCategories(newSelected);
    setConfig("selectedCategoryNames", selectedNames);

    // Sections are at keys 10200-10299
    const newSelectedSections: boolean[] = availableSections.map((_, idx) => {
      const key = (10200 + idx).toString();
      const val = settings[key];
      return val === true || val === 1;
    });
    handlers.setSelectedSectionsFromBoolean(newSelectedSections);
    const selectedSectionNames = booleanToSections(newSelectedSections);
    setConfig("selectedSectionNames", selectedSectionNames);

    // Additional Feeds are at keys 10300-10399
    const newSelectedFeeds: boolean[] = additionalFeeds.map((_, idx) => {
      const key = (10300 + idx).toString();
      const val = settings[key];
      return val === true || val === 1;
    });
    const selectedAdditionalFeeds = booleanToFeeds(newSelectedFeeds);
    setConfig("selectedAdditionalFeeds", selectedAdditionalFeeds);
    handlers.setAdditionalFeeds(...selectedAdditionalFeeds);

    // Interface language is key 10400
    const newInterfaceLanguage = settings["10400"] as string;
    setConfig("selectedInterfaceLanguage", newInterfaceLanguage);

    // Content language is key 10401
    const newContentLanguage = settings["10401"] as string;
    setServerLang(newContentLanguage);
    setConfig("selectedContentLanguage", newContentLanguage);

    // Max story count is key 10402
    const newMaxStoryCount = settings["10402"] as number;
    handlers.setMaxStoryCount(newMaxStoryCount);
    setConfig("selectedMaxStoryCount", newMaxStoryCount);

    // Text size is key 10403
    const newTextSize = settings["10403"] as number;
    setConfig("selectedTextSize", newTextSize);

    // News refresh timeline pin setting is key 10404
    const newsRefreshPinSetting = settings["10404"] as boolean;
    setConfig("newsRefreshPinSetting", newsRefreshPinSetting);
    if (newsRefreshPinSetting) {
      timeline.pushNewsRefreshPinToTimeline();
    }

    // Show tension index setting is key 10405
    const showTensionIndex = settings["10405"] as boolean;
    setConfig("isTensionIndexEnabled", showTensionIndex);
    // Do not update here, do it in the finally after categories are updated

    // Clear full cache on save is key 10407
    const clearFullCacheOnSave = settings["10407"] as boolean;
    if (clearFullCacheOnSave) {
      log.info("Clearing full cache as per user request");
      await handlers.clearFullCache();
    } else {
      // Clear timeline pin memory on save is key 10408
      const clearPinCacheOnSave = settings["10408"] as boolean;
      if (clearPinCacheOnSave) {
        log.info("Clearing timeline pin cache as per user request");
        handlers.clearPinCache();
      }
      // Clear read stories memory on save is key 10409
      const clearReadStoriesOnSave = settings["10409"] as boolean;
      if (clearReadStoriesOnSave) {
        log.info("Clearing read stories cache as per user request");
        handlers.clearReadStoriesCache();
      }
      // Clear network cache on save is key 10410
      // This is redundant because we always clear the network cache on Clay closing, but the certainty of it is useful to have
      const clearNetworkCacheOnSave = settings["10410"] as boolean;
      if (clearNetworkCacheOnSave) {
        log.info("Clearing network cache as per user request");
        await handlers.clearNetworkCache({ repopulateBatchId: true });
      }
    }
  } catch (err) {
    log.error(
      "Error in webviewclosed handler",
      err instanceof Error ? err.message : String(err),
    );
  } finally {
    // TODO: The order of operations for restarting the app should be consolidated
    sendAppMessageWithSession({ type: "restart_app" }).then(async () => {
      const interfaceLanguage = getConfig("selectedInterfaceLanguage");
      const interfaceStrings = flattenInterfaceStrings(
        filterInterfaceStringOptionals({
          strings: getInterfaceStrings(interfaceLanguage),
          sectionKeys: handlers.getSelectedSectionNames?.() ?? defaultSections,
          categoryKeys:
            handlers.getSelectedCategoryNames?.() ?? defaultCategories,
          featureKeys: handlers.getAdditionalFeeds?.() ?? defaultCategories,
          lang: interfaceLanguage,
        }),
      );
      Promise.all([
        sendAppMessageWithSession({
          type: "send_interface_strings",
          interfaceStrings: JSON.stringify(interfaceStrings),
        }),
        sendAppMessageWithSession({
          type: "set_debug_mode",
          isDebugMode: isDebugMode() ? 1 : 0,
        }),
        sendAppMessageWithSession({
          type: "set_text_size",
          textSize: getConfig("selectedTextSize"),
        }),
      ]).then(async () => {
        await handlers.handleUpdateCategories();
        await handlers.updateTensionIndex(getConfig("isTensionIndexEnabled"));
        sendAppMessageWithSession({
          type: "update_categories",
          state: "success",
        });
      });
    });
  }
});

Pebble.addEventListener("appmessage", async (event) => {
  if (!event.payload || !("type" in event.payload)) return;
  const payload: any = event.payload;

  switch (payload.type) {
    case "get_category_names":
      handlers.handleGetCategoryNames({
        page: payload.page,
      });
      break;
    case "get_story_titles":
      if ("categoryName" in payload) {
        handlers.handleGetStoryTitles({
          page: payload.page,
          categoryName: payload.categoryName,
          shortData: payload.shortData === 1,
        });
      } else {
        sendAppMessageWithSession({
          type: "get_story_titles",
          state: "error",
          error: "categoryName required for get_story_titles but none provided",
        });
      }
      break;
    case "get_short_summary":
      if ("storyTitle" in payload) {
        handlers.handleGetShortSummary({
          categoryName: payload.categoryName,
          storyTitle: payload.storyTitle,
        });
      } else {
        sendAppMessageWithSession({
          type: "get_short_summary",
          state: "error",
          error: "storyTitle required for get_short_summary but none provided",
        });
      }
      break;
    case "get_available_details":
      if ("storyId" in payload) {
        handlers.handleGetAvailableDetails({
          categoryName: payload.categoryName,
          storyId: payload.storyId,
        });
      } else {
        sendAppMessageWithSession({
          type: "get_available_details",
          state: "error",
          error: "storyId required for get_available_details but none provided",
        });
      }
      break;
    case "get_story_detail":
      if ("detailType" in payload) {
        handlers.handleGetStoryDetail({
          detailType: payload.detailType,
          detailKey: payload.detailKey,
        });
      } else {
        sendAppMessageWithSession({
          type: "get_story_detail",
          state: "error",
          error: "detailType required for get_story_detail but none provided",
        });
      }
      break;
    case "get_story_detail_sources":
      if ("detailType" in payload) {
        if (payload.detailType === StoryDetailEnum.Quote) {
          handlers.handleGetArticleDomainForQuote();
        } else {
          handlers.handleGetArticleDomainsFromContent({
            detailType: payload.detailType,
            detailKey: payload.detailKey,
          });
        }
      } else {
        sendAppMessageWithSession({
          type: "get_story_detail_sources",
          state: "error",
          error:
            "detailKey required for get_story_detail_sources but none provided",
        });
      }
      break;
    case "get_qr_code_bitmap":
      log.debug("Received get_qr_code_bitmap request", payload);
      if ("articleDomain" in payload) {
        if (payload.articleDomain === "Type_Quote") {
          handlers.handleUpdateQrCodeForQuote();
        } else {
          handlers.handleUpdateQrCode({ domain: payload.articleDomain });
        }
      } else {
        sendAppMessageWithSession({
          type: "get_qr_code_bitmap",
          state: "error",
          error:
            "articleDomain required for get_qr_code_bitmap but none provided",
        });
      }
      break;
    case "get_next_qr_code_bitmap":
      if ("chunk" in payload) {
        handlers.handleGetQrCodeChunk({ chunk: payload.chunk });
      } else {
        sendAppMessageWithSession({
          type: "get_next_qr_code_bitmap",
          state: "error",
          error: "chunk required for get_next_qr_code_bitmap but none provided",
        });
      }
      break;
    case "mark_story_read":
      if ("storyId" in payload) {
        handlers.handleAddReadStory(payload.storyId);
      } else {
        sendAppMessageWithSession({
          type: "mark_story_read",
          state: "error",
          error: "storyId required for mark_story_read but not provided",
        });
      }
      break;
    case "debug_log":
      if (
        "logLevel" in payload &&
        "logTag" in payload &&
        "logMessage" in payload
      ) {
        const level = payload.logLevel as number;
        const tag = payload.logTag as string;
        const message = payload.logMessage as string;
        const cLog = createLogger(tag);

        if (level === 0) cLog.debug(message);
        else if (level === 1) cLog.info(message);
        else if (level === 2) cLog.warn(message);
        else if (level === 3) cLog.error(message);
      }
      break;
    case "debug_notify":
      if ("logTag" in payload && "logMessage" in payload) {
        log.notify(payload.logTag as string, payload.logMessage as string);
      }
      break;
    case "tension_index_update":
      handlers.sendFullTensionInfo();
      break;
    default:
    // Do nothing
  }
});
