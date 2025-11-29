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

Pebble.addEventListener("ready", async () => {
  console.log("[KNJS]: PebbleKit JS ready");
  try {
    const response = await healthRequest();
    if (response.health === true) {
      console.log("[KNJS]: Kagi News API is healthy");

      const storedCategoryNames = localStorage.getItem("selectedCategoryNames");
      if (storedCategoryNames) {
        const names: string[] = JSON.parse(storedCategoryNames);
        console.log(
          `[KNJS]: Found saved selected categories: ${names.length} items`,
        );

        handlers.setSelectedCategories(names);
      } else {
        handlers.setSelectedCategories(defaultCategories);
      }

      await handlers.handleUpdateCategories();
      console.log("[KNJS]: Notifying watch that data is ready");
      PebbleTS.sendAppMessage({ type: "update_categories", state: "success" });
    } else {
      console.log("[KNJS]: Kagi News API is unhealthy");
    }
  } catch (err) {
    console.log("[KNJS]: Something went wrong with initial setup", err);
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
    };
    localStorage.setItem("clay-settings", JSON.stringify(claySettings));
    console.log(
      "[KNJS]: Set clay-settings before opening config:",
      JSON.stringify(claySettings).substring(0, 200),
    );

    Pebble.openURL(clay.generateUrl());
  } catch (e) {
    console.error("Failed to open Clay configuration", e);
  }
});

Pebble.addEventListener("webviewclosed", async (event) => {
  try {
    if (!event || !event.response) return;
    const settings = clay.getSettings(event.response);
    const newSelected: boolean[] = availableCategories.map((_, idx) => {
      const key = (10000 + idx).toString();
      const val = settings[key];
      if (typeof val === "boolean") return val;
      if (typeof val === "string") return val.toLowerCase() === "true";
      return false;
    });
    handlers.setSelectedCategoriesFromBoolean(newSelected);

    // Persist as category names (not booleans) for future-proofing
    const selectedNames = availableCategories.filter((_, i) => newSelected[i]);
    localStorage.setItem(
      "selectedCategoryNames",
      JSON.stringify(selectedNames),
    );
    console.log(`[KNJS]: Saved ${selectedNames.length} selected categories`);

    await handlers.handleUpdateCategories();
    PebbleTS.sendAppMessage({ type: "update_categories", state: "success" });
  } catch (err) {
    console.error("Error parsing Clay settings", err);
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
      console.log(
        "Received get_qr_code_bitmap request",
        JSON.stringify(payload),
      );
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
    default:
    // Do nothing
  }
});
