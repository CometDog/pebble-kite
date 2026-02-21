import { ClayConstructor } from "../lib/Clay";
// @ts-ignore
const Clay: ClayConstructor = require("@rebble/clay");
// @ts-ignore
const clayConfig = require("../ts-build/config");
const clay = new Clay(clayConfig, null, { autoHandleEvents: false });

import {
  getConfig,
  setConfig,
  booleanToCategories,
  booleanToSections,
  generateClaySettings,
  booleanToFeeds,
} from "./config";
import { healthRequest } from "./server/request/health";
import * as handlers from "./handlers";
import {
  additionalFeeds,
  availableCategories,
  availableSections,
  defaultCategories,
  defaultSections,
  StoryDetailEnum,
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
import { updateCachedBatchInfo } from "./server/cache/cacheManager";
import { batchesRequest } from "./server/request/batches";
import * as timeline from "./timeline";

const log = createLogger("KNJS");

Pebble.addEventListener("ready", async () => {
  initDebugMode();
  log.info("PebbleKit JS ready");

  PebbleTS.sendAppMessage({
    type: "set_debug_mode",
    isDebugMode: isDebugMode() ? 1 : 0,
  });

  try {
    const response = await healthRequest();
    if (response.health === true) {
      log.info("Kagi News API is healthy");
      const batchInfo = await batchesRequest();
      log.info(
        `Current batch ID: ${batchInfo.id}, created at ${batchInfo.createdAt}`,
      );
      updateCachedBatchInfo(batchInfo);

      const newsRefreshPinEnabled = getConfig("newsRefreshPinSetting");
      if (newsRefreshPinEnabled) {
        timeline.pushNewsRefreshPinToTimeline();
      }

      const textSize = getConfig("selectedTextSize");
      PebbleTS.sendAppMessage({
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

      const interfaceLanguage = getConfig("selectedInterfaceLanguage");
      const interfaceStrings = flattenInterfaceStrings(
        filterInterfaceStringOptionals({
          strings: getInterfaceStrings(interfaceLanguage),
          sectionKeys: selectedSectionNames ?? defaultSections,
          categoryKeys: selectedCategoryNames ?? defaultCategories,
          featureKeys: selectedAdditionalFeeds ?? defaultCategories,
        }),
      );
      log.debug("Sending interface strings:", JSON.stringify(interfaceStrings));
      PebbleTS.sendAppMessage({
        type: "send_interface_strings",
        interfaceStrings: JSON.stringify(interfaceStrings),
      });

      await handlers.handleUpdateCategories();
      log.info("Notifying watch that data is ready");
      PebbleTS.sendAppMessage({ type: "update_categories", state: "success" });
    } else {
      log.warn("Kagi News API is unhealthy");
    }
  } catch (err) {
    log.error("Something went wrong with initial setup", err);
  }
});

Pebble.addEventListener("showConfiguration", () => {
  try {
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
    PebbleTS.sendAppMessage({ type: "restart_app" });

    const settings = clay.getSettings(event.response);
    log.debug("Clay settings received:", JSON.stringify(settings));
    // DebugMode is at key 10405 (after all category and section checkbox keys)
    const debugModeKey = "10405";
    const rawDebugValue = settings[debugModeKey];
    const debugModeValue = rawDebugValue === true || rawDebugValue === 1;

    setDebugMode(debugModeValue);
    PebbleTS.sendAppMessage({
      type: "set_debug_mode",
      isDebugMode: debugModeValue ? 1 : 0,
    });

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
    const interfaceLanguage = getConfig("selectedInterfaceLanguage");
    const interfaceStrings = flattenInterfaceStrings(
      filterInterfaceStringOptionals({
        strings: getInterfaceStrings(interfaceLanguage),
        sectionKeys: selectedSectionNames ?? defaultSections,
        categoryKeys: selectedNames ?? defaultCategories,
        featureKeys: selectedAdditionalFeeds ?? defaultCategories,
      }),
    );
    PebbleTS.sendAppMessage({
      type: "send_interface_strings",
      interfaceStrings: JSON.stringify(interfaceStrings),
    });

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
    PebbleTS.sendAppMessage({
      type: "set_text_size",
      textSize: newTextSize,
    });

    // News refresh timeline pin setting is key 10404
    const newsRefreshPinSetting = settings["10404"] as boolean;
    setConfig("newsRefreshPinSetting", newsRefreshPinSetting);
    if (newsRefreshPinSetting) {
      timeline.pushNewsRefreshPinToTimeline();
    }

    // Clear full cache on save is key 10406
    const clearFullCacheOnSave = settings["10406"] as boolean;
    if (clearFullCacheOnSave) {
      log.info("Clearing full cache as per user request");
      await handlers.clearFullCache();
    } else {
      // Clear timeline pin memory on save is key 10407
      const clearPinCacheOnSave = settings["10407"] as boolean;
      if (clearPinCacheOnSave) {
        log.info("Clearing timeline pin cache as per user request");
        handlers.clearPinCache();
      }
      // Clear read stories memory on save is key 10408
      const clearReadStoriesOnSave = settings["10408"] as boolean;
      if (clearReadStoriesOnSave) {
        log.info("Clearing read stories cache as per user request");
        handlers.clearReadStoriesCache();
      }
      // Clear network cache on save is key 10409
      // This is redundant because we always clear the network cache on Clay closing, but the certainty of it is useful to have
      const clearNetworkCacheOnSave = settings["10409"] as boolean;
      if (clearNetworkCacheOnSave) {
        log.info("Clearing network cache as per user request");
        await handlers.clearNetworkCache({ repopulateBatchId: true });
      }
    }
  } catch (err) {
    PebbleTS.sendAppMessage({ type: "update_categories", state: "success" });
    log.error(
      "Error in webviewclosed handler",
      err instanceof Error ? err.message : String(err),
    );
  } finally {
    await handlers.handleUpdateCategories();
    PebbleTS.sendAppMessage({ type: "update_categories", state: "success" });
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
        PebbleTS.sendAppMessage({
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
        PebbleTS.sendAppMessage({
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
        PebbleTS.sendAppMessage({
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
        PebbleTS.sendAppMessage({
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
        PebbleTS.sendAppMessage({
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
        PebbleTS.sendAppMessage({
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
        PebbleTS.sendAppMessage({
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
        PebbleTS.sendAppMessage({
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
    default:
    // Do nothing
  }
});
