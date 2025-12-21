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
} from "./config";
import { healthRequest } from "./server/request/health";
import * as handlers from "./handlers";
import {
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
  filterInterfaceStringSectionsAndCategories,
  flattenInterfaceStrings,
  getInterfaceStrings,
} from "./localization";
import { setServerLang } from "./server/localeManager";

const log = createLogger("KNJS");

Pebble.addEventListener("ready", async () => {
  initDebugMode();
  log.info("PebbleKit JS ready");

  PebbleTS.sendAppMessage({
    type: "set_debug_mode",
    data: isDebugMode() ? 1 : 0,
  });

  try {
    const response = await healthRequest();
    if (response.health === true) {
      log.info("Kagi News API is healthy");

      const newsRefreshPinEnabled = getConfig("newsRefreshPinSetting");
      if (newsRefreshPinEnabled) {
        handlers.pushNewsRefreshPinToTimeline();
      }

      const textSize = getConfig("selectedTextSize");
      PebbleTS.sendAppMessage({
        type: "set_text_size",
        data: textSize,
      });

      const contentLanguage = getConfig("selectedContentLanguage");
      setServerLang(contentLanguage);

      const maxStoryCount = getConfig("selectedMaxStoryCount");
      handlers.setMaxStoryCount(maxStoryCount);

      const selectedSectionNames = getConfig("selectedSectionNames");
      handlers.setSelectedSections(selectedSectionNames);

      const selectedCategoryNames = getConfig("selectedCategoryNames");
      handlers.setSelectedCategories(selectedCategoryNames);

      const interfaceLanguage = getConfig("selectedInterfaceLanguage");
      const interfaceStrings = flattenInterfaceStrings(
        filterInterfaceStringSectionsAndCategories({
          strings: getInterfaceStrings(interfaceLanguage),
          sectionKeys: selectedSectionNames ?? defaultSections,
          categoryKeys: selectedCategoryNames ?? defaultCategories,
        }),
      );
      console.log(
        "Sending interface strings:",
        JSON.stringify(interfaceStrings),
      );
      PebbleTS.sendAppMessage({
        type: "send_interface_strings",
        data: JSON.stringify(interfaceStrings),
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
    localStorage.setItem("clay-settings", JSON.stringify(claySettings));
    Pebble.openURL(clay.generateUrl());
  } catch (e) {
    log.error("Failed to open Clay configuration", e);
  }
});

// @ts-ignore
Pebble.addEventListener("webviewclosed", async (event: any) => {
  try {
    if (!event || !event.response) return;
    PebbleTS.sendAppMessage({ type: "restart_app" });

    const settings = clay.getSettings(event.response);
    console.log("Clay settings received:", JSON.stringify(settings));
    // DebugMode is at key 10305 (after all category and section checkbox keys)
    const debugModeKey = "10305";
    const rawDebugValue = settings[debugModeKey];
    const debugModeValue = rawDebugValue === true || rawDebugValue === 1;

    setDebugMode(debugModeValue);
    PebbleTS.sendAppMessage({
      type: "set_debug_mode",
      data: debugModeValue ? 1 : 0,
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

    // Interface language is key 10300
    const newInterfaceLanguage = settings["10300"] as string;
    setConfig("selectedInterfaceLanguage", newInterfaceLanguage);
    const interfaceLanguage = getConfig("selectedInterfaceLanguage");
    const interfaceStrings = flattenInterfaceStrings(
      filterInterfaceStringSectionsAndCategories({
        strings: getInterfaceStrings(interfaceLanguage),
        sectionKeys: selectedSectionNames ?? defaultSections,
        categoryKeys: selectedNames ?? defaultCategories,
      }),
    );
    PebbleTS.sendAppMessage({
      type: "send_interface_strings",
      data: JSON.stringify(interfaceStrings),
    });

    // Content language is key 10301
    const newContentLanguage = settings["10301"] as string;
    setServerLang(newContentLanguage);
    setConfig("selectedContentLanguage", newContentLanguage);

    // Max story count is key 10302
    const newMaxStoryCount = settings["10302"] as number;
    handlers.setMaxStoryCount(newMaxStoryCount);
    setConfig("selectedMaxStoryCount", newMaxStoryCount);

    // Text size is key 10303
    const newTextSize = settings["10303"] as number;
    setConfig("selectedTextSize", newTextSize);
    PebbleTS.sendAppMessage({
      type: "set_text_size",
      data: newTextSize,
    });

    // News refresh timeline pin setting is key 10304
    const newsRefreshPinSetting = settings["10304"] as boolean;
    setConfig("newsRefreshPinSetting", newsRefreshPinSetting);
    if (newsRefreshPinSetting) {
      handlers.pushNewsRefreshPinToTimeline();
    }

    // Clear full cache on save is key 10306
    const clearFullCacheOnSave = settings["10306"] as boolean;
    if (clearFullCacheOnSave) {
      log.info("Clearing full cache as per user request");
      handlers.clearFullCache();
    } else {
      // Clear timeline pin memory on save is key 10307
      const clearPinCacheOnSave = settings["10307"] as boolean;
      if (clearPinCacheOnSave) {
        log.info("Clearing timeline pin cache as per user request");
        handlers.clearPinCache();
      }
      // Clear read stories memory on save is key 10308
      const clearReadStoriesOnSave = settings["10308"] as boolean;
      if (clearReadStoriesOnSave) {
        log.info("Clearing read stories cache as per user request");
        handlers.clearReadStoriesCache();
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
