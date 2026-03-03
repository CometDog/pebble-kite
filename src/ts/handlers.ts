import { createLogger } from "../lib";
import {
  addReadStory,
  getCategoryBatchMap,
  updateCategories,
  updateTension,
} from "./api";
import { updateQrCodeBitmap, getQrCodeChunk } from "./qr";
import { clearCache, updateCachedBatchInfo } from "./server/cache/cacheManager";
import { batchesRequest } from "./server/request/batches";
import { sendAppMessageWithSession } from "./sessionBasedAppMessage";

import {
  availableCategories,
  availableSections,
  defaultCategories,
  defaultMaxStories,
  defaultSections,
  PAGE_SIZE,
  SHORT_TITLE_LENGTH,
  StoryDetailEnum,
} from "./types";
import type {
  AvailableCategory,
  AvailableSection,
  SimpleStory,
  StoryDetailType,
  CategorizedStories,
  AdditionalFeature,
} from "./types";

const log = createLogger("KNJSHandler");

let selectedCategoryNames: AvailableCategory[] = [...defaultCategories];
let selectedSectionNames: AvailableSection[] = [...defaultSections];
let selectedMaxStoryCount: number = defaultMaxStories;
let additionalFeeds: AdditionalFeature[] = [];

let currentStory: SimpleStory | null = null;

export const setSelectedCategories = (selected: AvailableCategory[]) => {
  selectedCategoryNames = selected;
};

export const setSelectedCategoriesFromBoolean = (selected: boolean[]) => {
  selectedCategoryNames = availableCategories.filter((_, i) => selected[i]);
};

export const getSelectedCategoryNames = ({
  includeAdditionalFeeds = false,
} = {}) => {
  if (includeAdditionalFeeds) {
    return [...additionalFeeds, ...selectedCategoryNames];
  }
  return selectedCategoryNames;
};

export const setSelectedSections = (selected: AvailableSection[]) => {
  selectedSectionNames = selected;
};

export const setSelectedSectionsFromBoolean = (selected: boolean[]) => {
  selectedSectionNames = availableSections.filter((_, i) => selected[i]);
};

export const getSelectedSectionNames = () => selectedSectionNames;

export const setMaxStoryCount = (count: number) => {
  selectedMaxStoryCount = count;
};

export const getMaxStoryCount = () => selectedMaxStoryCount;

const sendAppMessage = (payload: Record<string, any>) =>
  sendAppMessageWithSession(payload);

const paginate = <T>(items: T[], page = 1, pageSize = PAGE_SIZE) => {
  const start = (page - 1) * pageSize;
  const pageItems = items.slice(start, start + pageSize);
  const hasNext = start + pageSize < items.length;
  return { pageItems, nextPage: hasNext ? page + 1 : undefined };
};

const escapeRegExp = (s: string) => s.replace(/[.*+?^${}()|[\]\\]/g, "\\$&");

const getCategorizedList = (categoryName?: string): CategorizedStories[] => {
  const categorizedStories = getCategoryBatchMap().categorizedStories;
  if (categoryName) {
    const filtered = categorizedStories.filter(
      (story) => story.category.name === categoryName,
    );
    return filtered;
  }
  return categorizedStories;
};

// Generic handler for list-like details (perspectives, talking points, technical details, industry impact)
const handleDetailList = <T>(
  items: T[] | undefined,
  getKey: (it: T) => string | undefined,
  getContent: (it: T) => string | undefined,
  detailType: StoryDetailType,
  detailKey: string | undefined,
  keyFallback = "Item",
  data3ForKeys = "povs",
) => {
  if (detailKey) {
    const content = getDetailContent(detailType, detailKey);
    if (content) {
      sendAppMessage({
        type: "get_story_detail",
        state: "success",
        detailType: detailType,
        detailContent: content,
        detailContentType: "text",
        detailHasSources: contentContainsSources(content) ? 1 : 0,
      });
      return true;
    }
    return false;
  }

  const keys: string[] = [];
  items?.forEach((item) => {
    const raw = getKey(item);
    if (!raw) return;
    // If the raw key contains a colon, extract the text before the colon (trim spaces).
    const colonIndex = raw.indexOf(":");
    if (colonIndex !== -1) {
      const before = raw.slice(0, colonIndex).trim();
      keys.push(before.length ? before : keyFallback);
    } else {
      // Otherwise, use the raw key as is
      keys.push(raw);
    }
  });
  sendAppMessage({
    type: "get_story_detail",
    state: "success",
    detailType: detailType,
    detailContent: keys.join("||"),
    detailContentType: data3ForKeys,
  });
  return true;
};

const contentContainsSources = (content: string) => /\[.+?#\d\]/g.test(content);

const getDetailContent = (
  detailType: StoryDetailType,
  detailKey?: string,
): string => {
  if (!currentStory) return "";
  const prefixRegex = detailKey
    ? new RegExp(`^${escapeRegExp(detailKey)}\\s*:\\s*`)
    : null;

  const findAndStrip = (items: string[] | undefined) => {
    if (!prefixRegex) return "";
    const found = items?.find((item) => prefixRegex.test(item));
    return found ? found.replace(prefixRegex, "") : "";
  };

  const findAndStripPerspective = (
    items: Array<{ text: string }> | undefined,
  ) => {
    if (!prefixRegex) return "";
    const found = items?.find((item) => prefixRegex.test(item.text));
    return found ? found.text.replace(prefixRegex, "") : "";
  };

  const findQnAAnswer = (
    items: Array<{ question: string; answer: string }> | undefined,
  ) => {
    if (!detailKey) return "";
    const found = items?.find((item) => item.question === detailKey);
    return found ? found.answer : "";
  };

  const findTimelineContent = (
    items: Array<{ date: string; content: string }> | undefined,
  ) => {
    if (!detailKey) return "";
    const found = items?.find((item) => item.date === detailKey);
    return found ? found.content : "";
  };

  switch (detailType) {
    case StoryDetailEnum.Background:
      return currentStory.historical_background;
    case StoryDetailEnum.TalkingPoints:
      return findAndStrip(currentStory.talking_points);
    case StoryDetailEnum.Quote:
      return currentStory.quote?.text ?? "";
    case StoryDetailEnum.Perspectives:
      return findAndStripPerspective(currentStory.perspectives);
    case StoryDetailEnum.TechnicalDetails:
      return findAndStrip(currentStory.technical_details);
    case StoryDetailEnum.IndustryImpact:
      return findAndStrip(currentStory.industry_impact);
    case StoryDetailEnum.QnA:
      return findQnAAnswer(currentStory.suggested_qna);
    case StoryDetailEnum.Timeline:
      return findTimelineContent(currentStory.timeline);
    case StoryDetailEnum.HumanitarianImpact:
      return currentStory.humanitarian_impact ?? "";
    case StoryDetailEnum.TravelAdvisory:
      return findAndStrip(currentStory.travel_advisory);
    case StoryDetailEnum.DidYouKnow:
      return currentStory.did_you_know ?? "";
    case StoryDetailEnum.InternationalReactions:
      return findAndStrip(currentStory.international_reactions);
    case StoryDetailEnum.UserActionItems:
      return findAndStrip(currentStory.user_action_items);
    case StoryDetailEnum.PerformanceStatistics:
      return findAndStrip(currentStory.performance_statistics);
    case StoryDetailEnum.ScientificSignificance:
      return findAndStrip(currentStory.scientific_significance);
    default:
      return "";
  }
};

export const handleGetCategoryNames = ({ page = 1 }: { page?: number }) => {
  const batch = getCategoryBatchMap();
  if (!batch)
    return sendAppMessage({
      type: "get_category_names",
      state: "error",
      error: "No data",
    });
  const regularNames = batch.categorizedStories.map((cs) => cs.category.name);
  // Add enabled additional feeds as pseudo-categories
  // TODO: This is horrible. Why would Additional Feeds end up in Regular Names? I don't know why but it happens
  const allNames = [...new Set([...additionalFeeds, ...regularNames])];
  const { pageItems, nextPage } = paginate(allNames, page);
  sendAppMessage({
    type: "get_category_names",
    state: "success",
    categoryNamePage: pageItems.join("||"),
    additionalFeedsCount: additionalFeeds.length,
    ...(nextPage && { nextPage }),
  });
};

export const handleGetStoryTitles = ({
  page = 1,
  categoryName,
  shortData = false,
}: {
  page?: number;
  categoryName: string;
  shortData?: boolean;
}) => {
  const batch = getCategoryBatchMap();
  if (!batch)
    return sendAppMessage({
      type: "get_story_titles",
      state: "error",
      error: "No data",
    });
  const titles =
    batch.categorizedStories
      .find((categorized) => categorized.category.name === categoryName)
      ?.stories.map((story) => story.title) ?? [];
  const { pageItems, nextPage } = paginate(titles, page);
  const processed = shortData
    ? pageItems.map((title) =>
        title.length > SHORT_TITLE_LENGTH
          ? title.slice(0, SHORT_TITLE_LENGTH - 3) + "..."
          : title,
      )
    : pageItems;

  // This is redundant to the code above and should be DRY'd up
  const readStories =
    batch.categorizedStories
      .find((categorized) => categorized.category.name === categoryName)
      ?.stories.map((story) => story.read) ?? [];

  sendAppMessage({
    type: "get_story_titles",
    state: "success",
    categoryName,
    shortData: shortData ? 1 : 0,
    storyTitles: processed.join("||"),
    storyReadStatuses: readStories.map((read) => (read ? "1" : "0")).join("||"),
    ...(nextPage && { nextPage }),
  });
};

export const handleGetShortSummary = ({
  categoryName,
  storyTitle,
}: {
  categoryName?: string;
  storyTitle: string;
}) => {
  const categories = getCategorizedList(categoryName);

  let storyId = "";
  let fullTitle = "";
  let shortSummary = "";

  for (const category of categories) {
    for (const story of category.stories) {
      if (
        story.title.startsWith(
          storyTitle.slice(0, Math.max(0, storyTitle.length - 3)),
        ) ||
        story.title === storyTitle
      ) {
        storyId = story.id;
        fullTitle = story.title;
        shortSummary = story.shortSummary;
        break;
      }
    }
  }

  sendAppMessage({
    type: "get_short_summary",
    state: "success",
    storyTitle: fullTitle,
    storySummary: shortSummary,
    storyId: storyId,
  });
};

export const handleGetAvailableDetails = ({
  categoryName,
  storyId,
}: {
  categoryName?: string;
  storyId: string;
}) => {
  currentStory = null;
  const categories = getCategorizedList(categoryName);

  for (const category of categories) {
    for (const story of category.stories) {
      if (story.id === storyId) {
        currentStory = story;
        currentStory!.articles = currentStory!.articles?.map(
          (article, index) => ({
            link: article.link,
            domain: article.domain.startsWith("[")
              ? article.domain
              : `[${index + 1}] ${article.domain}`,
          }),
        );
        break;
      }
    }
  }

  if (!currentStory)
    return sendAppMessage({
      type: "get_available_details",
      state: "error",
      error: "Story not found",
    });
  const newStory = currentStory as SimpleStory;
  const available: string[] = [];
  if (newStory.articles && newStory.articles.length)
    available.push(StoryDetailEnum.Articles);
  if (
    selectedSectionNames.includes("Highlights") &&
    newStory.talking_points &&
    newStory.talking_points.length
  )
    available.push(StoryDetailEnum.TalkingPoints);
  if (
    selectedSectionNames.includes("Quote") &&
    newStory.quote &&
    newStory.quote.text.length
  )
    available.push(StoryDetailEnum.Quote);
  if (
    selectedSectionNames.includes("Perspectives") &&
    newStory.perspectives &&
    newStory.perspectives.length
  )
    available.push(StoryDetailEnum.Perspectives);
  if (
    selectedSectionNames.includes("Historical Background") &&
    newStory.historical_background &&
    newStory.historical_background.length
  )
    available.push(StoryDetailEnum.Background);
  if (
    selectedSectionNames.includes("Humanitarian Impact") &&
    newStory.humanitarian_impact &&
    newStory.humanitarian_impact.length
  )
    available.push(StoryDetailEnum.HumanitarianImpact);
  if (
    selectedSectionNames.includes("Travel Advisory") &&
    newStory.travel_advisory &&
    newStory.travel_advisory.length
  )
    available.push(StoryDetailEnum.TravelAdvisory);
  if (
    selectedSectionNames.includes("Industry Impact") &&
    newStory.industry_impact &&
    newStory.industry_impact.length
  )
    available.push(StoryDetailEnum.IndustryImpact);
  if (
    selectedSectionNames.includes("Technical Details") &&
    newStory.technical_details &&
    newStory.technical_details.length
  )
    available.push(StoryDetailEnum.TechnicalDetails);
  if (
    selectedSectionNames.includes("Performance Statistics") &&
    newStory.performance_statistics &&
    newStory.performance_statistics.length
  )
    available.push(StoryDetailEnum.PerformanceStatistics);
  if (
    selectedSectionNames.includes("Scientific Significance") &&
    newStory.scientific_significance &&
    newStory.scientific_significance.length
  )
    available.push(StoryDetailEnum.ScientificSignificance);
  if (
    selectedSectionNames.includes("Timeline") &&
    newStory.timeline &&
    newStory.timeline.length
  )
    available.push(StoryDetailEnum.Timeline);
  if (
    selectedSectionNames.includes("International Reactions") &&
    newStory.international_reactions &&
    newStory.international_reactions.length
  )
    available.push(StoryDetailEnum.InternationalReactions);
  if (
    selectedSectionNames.includes("Quick Questions") &&
    newStory.suggested_qna &&
    newStory.suggested_qna.length
  )
    available.push(StoryDetailEnum.QnA);
  if (
    selectedSectionNames.includes("Action Items") &&
    newStory.user_action_items &&
    newStory.user_action_items.length
  )
    available.push(StoryDetailEnum.UserActionItems);
  if (
    selectedSectionNames.includes("Did You Know?") &&
    newStory.did_you_know &&
    newStory.did_you_know.length
  )
    available.push(StoryDetailEnum.DidYouKnow);
  sendAppMessage({
    type: "get_available_details",
    state: "success",
    availableDetailTypes: available.join("||"),
  });
};

export const handleGetStoryDetail = ({
  detailType,
  detailKey = undefined,
}: {
  detailType: StoryDetailType;
  detailKey?: string;
}) => {
  if (!currentStory)
    return sendAppMessage({
      type: "get_story_detail",
      state: "error",
      error: "No current story selected",
    });
  switch (detailType) {
    case StoryDetailEnum.Background:
      sendAppMessage({
        type: "get_story_detail",
        state: "success",
        detailType: detailType,
        detailContent: currentStory.historical_background,
        detailHasSources: contentContainsSources(
          currentStory.historical_background,
        )
          ? 1
          : 0,
      });
      break;
    case StoryDetailEnum.Articles:
      sendAppMessage({
        type: "get_story_detail",
        state: "success",
        detailType: detailType,
        detailContent:
          currentStory.articles?.map((a) => a.domain).join("||") ?? "",
      });
      break;
    case StoryDetailEnum.Perspectives:
      handleDetailList(
        currentStory.perspectives,
        (perspective?: Record<string, any>) => perspective?.text,
        (perspective?: Record<string, any>) =>
          perspective?.text?.replace(`${detailKey}: `, ""),
        detailType,
        detailKey,
        "Perspective",
        "povs",
      );
      break;
    case StoryDetailEnum.TalkingPoints:
      handleDetailList(
        currentStory.talking_points,
        (talkingPoint?: string) => talkingPoint as string,
        (talkingPoint?: string) => talkingPoint?.replace(`${detailKey}: `, ""),
        detailType,
        detailKey,
        "Talking Point",
        "povs",
      );
      break;
    case StoryDetailEnum.TechnicalDetails:
      handleDetailList(
        currentStory.technical_details,
        (technicalDetail?: string) => technicalDetail as string,
        (technicalDetail?: string) =>
          technicalDetail?.replace(`${detailKey}: `, ""),
        detailType,
        detailKey,
        "Technical Detail",
        "povs",
      );
      break;
    case StoryDetailEnum.IndustryImpact:
      handleDetailList(
        currentStory.industry_impact,
        (industryImpact?: string) => industryImpact as string,
        (industryImpact?: string) =>
          industryImpact?.replace(`${detailKey}: `, ""),
        detailType,
        detailKey,
        "Industry Impact",
        "povs",
      );
      break;
    case StoryDetailEnum.QnA:
      handleDetailList(
        currentStory.suggested_qna,
        (qna?: Record<string, any>) => qna?.question,
        (qna?: Record<string, any>) => qna?.answer,
        detailType,
        detailKey,
        "Question",
        "qnas",
      );
      break;
    case StoryDetailEnum.Timeline:
      handleDetailList(
        currentStory.timeline,
        (time?: Record<string, any>) => time?.date,
        (time?: Record<string, any>) => time?.content,
        detailType,
        detailKey,
        "Date",
        "dates",
      );
      break;
    case StoryDetailEnum.HumanitarianImpact:
      sendAppMessage({
        type: "get_story_detail",
        state: "success",
        detailType: detailType,
        detailContent: currentStory.humanitarian_impact,
        detailHasSources: contentContainsSources(
          currentStory.humanitarian_impact ?? "",
        )
          ? 1
          : 0,
      });
      break;
    case StoryDetailEnum.TravelAdvisory:
      handleDetailList(
        currentStory.travel_advisory,
        (advisory?: string) => advisory as string,
        (advisory?: string) => advisory?.replace(`${detailKey}: `, ""),
        detailType,
        detailKey,
        "Advisory",
        "advisories",
      );
      break;
    case StoryDetailEnum.DidYouKnow:
      sendAppMessage({
        type: "get_story_detail",
        state: "success",
        detailType: detailType,
        detailContent: currentStory.did_you_know,
        detailHasSources: contentContainsSources(
          currentStory.did_you_know ?? "",
        )
          ? 1
          : 0,
      });
      break;
    case StoryDetailEnum.Quote:
      sendAppMessage({
        type: "get_story_detail",
        state: "success",
        detailType: detailType,
        detailContent: currentStory.quote?.text ?? "",
        detailContentType: currentStory.quote?.author ?? "",
        detailHasSources: currentStory.quote?.source ? 1 : 0,
      });
      break;
    case StoryDetailEnum.InternationalReactions:
      handleDetailList(
        currentStory.international_reactions,
        (reaction?: string) => reaction as string,
        (reaction?: string) => reaction?.replace(`${detailKey}: `, ""),
        detailType,
        detailKey,
        "Reaction",
        "reactions",
      );
      break;
    case StoryDetailEnum.UserActionItems:
      handleDetailList(
        currentStory.user_action_items,
        (item?: string) => item as string,
        (item?: string) => item?.replace(`${detailKey}: `, ""),
        detailType,
        detailKey,
        "Action Item",
        "action_items",
      );
      break;
    case StoryDetailEnum.ScientificSignificance:
      handleDetailList(
        currentStory.scientific_significance,
        (item?: string) => item as string,
        (item?: string) => item?.replace(`${detailKey}: `, ""),
        detailType,
        detailKey,
        "Scientific Significance",
        "significance",
      );
      break;
    case StoryDetailEnum.PerformanceStatistics:
      handleDetailList(
        currentStory.performance_statistics,
        (item?: string) => item as string,
        (item?: string) => item?.replace(`${detailKey}: `, ""),
        detailType,
        detailKey,
        "Statistic",
        "statistics",
      );
      break;
    default:
      sendAppMessage({
        type: "get_story_detail",
        state: "error",
        error: "Unknown detail type",
      });
  }
};

export const handleUpdateCategories = async () => {
  await updateCategories({
    selectedCategoryNames,
    maxStoryCount: selectedMaxStoryCount,
  });
};

export const handleGetArticleDomainsFromContent = ({
  detailType,
  detailKey,
}: {
  detailType: StoryDetailType;
  detailKey?: string;
}) => {
  const contentText = getDetailContent(detailType, detailKey);
  const sourceMatches = contentText.match(/\[[a-zA-Z0-9\-\.]+#\d+\]/g);
  const domainCounts = new Map<string, number>();

  const articleDomains = currentStory?.articles
    ?.map((article) => {
      const plainDomain = article.domain.replace(/^\[\d+\]\s*/, "");
      const instanceCount = (domainCounts.get(plainDomain) || 0) + 1;
      domainCounts.set(plainDomain, instanceCount);

      const articleTag = `[${plainDomain}#${instanceCount}]`;
      if (sourceMatches?.some((match) => match === articleTag)) {
        return article.domain;
      }
    })
    .filter(Boolean);
  return sendAppMessage({
    type: "get_story_detail_sources",
    state: "success",
    articleDomains: articleDomains?.join("||") || "",
  });
};

export const handleGetArticleDomainForQuote = () => {
  return sendAppMessage({
    type: "get_story_detail_sources",
    state: "success",
    articleDomains: currentStory?.quote?.source,
  });
};

export const handleUpdateQrCodeForQuote = async () => {
  const url = currentStory?.quote?.url;
  handleUpdateQrCodeFromUrl(url);
};

export const handleUpdateQrCode = async ({ domain }: { domain: string }) => {
  const url = currentStory?.articles?.find(
    (article) => article.domain === domain,
  )?.link;
  handleUpdateQrCodeFromUrl(url);
};

const handleUpdateQrCodeFromUrl = async (url?: string) => {
  if (!url) {
    sendAppMessage({
      type: "get_qr_code_bitmap",
      state: "error",
      error: "QR generation failed",
    });
    return;
  }
  try {
    const data = await updateQrCodeBitmap({ url });
    if (!data)
      return sendAppMessage({
        type: "get_qr_code_bitmap",
        state: "error",
        error: "QR generation failed",
      });
    const totalChunks = data.totalChunks;
    sendAppMessage({
      type: "get_qr_code_bitmap",
      state: "success",
      qrData: [],
      qrSize: data.size,
      ...(totalChunks > 0 && { nextChunk: 1 }),
    });
  } catch (err) {
    log.error("QR update failed", err);
    sendAppMessage({
      type: "get_qr_code_bitmap",
      state: "error",
      error: "QR generation failed",
    });
  }
};

export const handleGetQrCodeChunk = ({ chunk }: { chunk: number }) => {
  const res = getQrCodeChunk(chunk);
  if (!res) return;
  sendAppMessage({
    type: "get_qr_code_bitmap",
    state: "success",
    qrData: res.chunkData,
    ...(res.isNext && { nextChunk: res.nextChunk }),
  });
};

export const handleAddReadStory = (storyId: string) => {
  addReadStory(storyId);
  // For performance reasons, this doesn't send back to the watch the index of the
  // story that was marked as read. The watch app is expected to keep track of
  // which stories have been read already locally until the next full update.
};

export const setAdditionalFeeds = (...feeds: AdditionalFeature[]) => {
  additionalFeeds = feeds;
};

export const getAdditionalFeeds = () => {
  return additionalFeeds;
};

export const clearPinCache = () => {
  localStorage.removeItem("newsRefreshPin0PushTime");
  localStorage.removeItem("newsRefreshPin1PushTime");
  localStorage.removeItem("newsRefreshPin2PushTime");
};

export const clearReadStoriesCache = () => {
  localStorage.removeItem("readStories");
};

export const clearNetworkCache = async ({
  repopulateBatchId,
}: {
  repopulateBatchId: boolean;
}) => {
  clearCache();
  if (repopulateBatchId) {
    const batchInfo = await batchesRequest();
    log.info(
      `Current batch ID: ${batchInfo.id}, created at ${batchInfo.createdAt}`,
    );
    updateCachedBatchInfo(batchInfo);
  }
};

export const clearFullCache = async () => {
  clearPinCache();
  clearReadStoriesCache();
  await clearNetworkCache({ repopulateBatchId: true });
};

const minimumTensionIndexForNotification = 50;
export const updateTensionIndex = async (tensionIndexEnabled: boolean) => {
  if (tensionIndexEnabled) {
    await updateTension();
    if (
      getCategoryBatchMap()?.tensionIndex?.index >=
      minimumTensionIndexForNotification
    ) {
      // TODO: This should be moved to the start up tasks. Sending app messages here implies this is a user-initiated action, which this is not
      sendAppMessage({
        type: "tension_index_update",
        tensionIndex: getCategoryBatchMap().tensionIndex.index,
      });
    }
  } else {
    sendAppMessage({
      type: "tension_index_update",
      tensionIndex: 0,
    });
  }
};

export const sendFullTensionInfo = () => {
  const { index, reason } = getCategoryBatchMap().tensionIndex;
  if (index >= minimumTensionIndexForNotification) {
    sendAppMessage({
      type: "tension_index_update",
      tensionIndex: index,
      tensionReason: reason,
    });
  }
};
