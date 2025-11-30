import { ClayConstructor } from "../pktslib/Clay";
// @ts-ignore
const Clay: ClayConstructor = require("pebble-clay");
// @ts-ignore
const clayConfig = require("../pkjs/config");
const clay = new Clay(clayConfig, null, { autoHandleEvents: false });

import { healthRequest } from "./server/request/health";
import * as handlers from "./handlers";
import {
  availableCategories,
  defaultCategories,
  StoryDetailEnum,
} from "./types";
import {
  createLogger,
  initDebugMode,
  setDebugMode,
  isDebugMode,
} from "../pktslib/logger";

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

      const storedCategoryNames = localStorage.getItem("selectedCategoryNames");
      if (storedCategoryNames) {
        const names: string[] = JSON.parse(storedCategoryNames);
        log.info(`Found saved selected categories: ${names.length} items`);

        handlers.setSelectedCategories(names);
      } else {
        handlers.setSelectedCategories(defaultCategories);
      }

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
    // Load saved category names and convert to boolean array for Clay
    const storedCategoryNames = localStorage.getItem("selectedCategoryNames");
    let booleanCategories: boolean[];

    if (storedCategoryNames) {
      const names: string[] = JSON.parse(storedCategoryNames);
      booleanCategories = availableCategories.map((cat) => names.includes(cat));
    } else {
      booleanCategories = availableCategories.map((cat) =>
        defaultCategories.includes(cat as any),
      );
    }

    const claySettings: Record<string, any> = {
      UserCategories: booleanCategories,
      DebugMode: isDebugMode(),
    };
    localStorage.setItem("clay-settings", JSON.stringify(claySettings));

    Pebble.openURL(clay.generateUrl());
  } catch (e) {
    log.error("Failed to open Clay configuration", e);
  }
});

Pebble.addEventListener("webviewclosed", async (event) => {
  try {
    if (!event || !event.response) return;
    const settings = clay.getSettings(event.response);
    // DebugMode is at key 10100 (after all category checkbox keys)
    const debugModeKey = "10100";
    const rawDebugValue = settings[debugModeKey];
    const debugModeValue = rawDebugValue === true || rawDebugValue === 1;

    setDebugMode(debugModeValue);
    PebbleTS.sendAppMessage({
      type: "set_debug_mode",
      data: debugModeValue ? 1 : 0,
    });

    // Categories are at keys 10000-10099
    const newSelected: boolean[] = availableCategories.map((_, idx) => {
      const key = (10000 + idx).toString();
      const val = settings[key];
      return val === true || val === 1;
    });
    handlers.setSelectedCategoriesFromBoolean(newSelected);

    // Persist as category names (not booleans) for future-proofing
    const selectedNames = availableCategories.filter((_, i) => newSelected[i]);
    localStorage.setItem(
      "selectedCategoryNames",
      JSON.stringify(selectedNames),
    );
    log.info(`Saved ${selectedNames.length} selected categories`);

    await handlers.handleUpdateCategories();
    PebbleTS.sendAppMessage({ type: "update_categories", state: "success" });
  } catch (err) {
    log.error(
      "Error in webviewclosed handler",
      err instanceof Error ? err.message : String(err),
    );
  }
});

Pebble.addEventListener("appmessage", async (event) => {
  if (!event.payload || !("type" in event.payload)) return;
  const payload: any = event.payload;

  switch (payload.type) {
    case "get_category_names":
      handlers.handleGetCategoryNames({
        page: payload.page,
        batchId: payload.batchId,
      });
      break;
    case "get_story_titles":
      if ("categoryName" in payload) {
        handlers.handleGetStoryTitles({
          page: payload.page,
          batchId: payload.batchId,
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
          batchId: payload.batchId,
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
          batchId: payload.batchId,
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
