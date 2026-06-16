import { batchCategoriesRequest } from "./server/request/batchRequests";
import {
  batchCategoryStoriesRequest,
  BatchCategoryStoriesResponse,
} from "./server/request/batchRequests";
import type {
  CurrentData,
  CategorizedStories,
  SimpleStory,
  ReadStoriesStorageObject,
} from "./types";
import type { BatchCategory } from "./server/type/BatchCategory";
import { CONCURRENT_BATCH_SIZE } from "./types";
import { getAdditionalFeeds } from "./handlers";
import {
  smallWebFeedRequest,
  SmallWebFeedResponse,
} from "./server/request/contentRequests";
import { tensionRequest } from "./server/request/contentRequests";

let categoryBatchMap: CurrentData = {
  batchId: "",
  categorizedStories: [],
  tensionIndex: { index: -1, reason: "" },
};

export const getCategoryBatchMap = () => categoryBatchMap;

export const setCategoryBatchMap = (data: CurrentData) => {
  cleanUpStaleReadStories(data);
  categoryBatchMap = updateDataWithReadStories(data);
};

const getReadStories = (): ReadStoriesStorageObject => {
  const existingReadStoriesString = localStorage.getItem("readStories");
  return existingReadStoriesString
    ? JSON.parse(existingReadStoriesString)
    : { batchId: "", readStoryIds: [] };
};

const setReadStories = (readStories: ReadStoriesStorageObject) => {
  localStorage.setItem("readStories", JSON.stringify(readStories));
  categoryBatchMap = updateDataWithReadStories(categoryBatchMap);
};

export const addReadStory = (storyId: string) => {
  const existingReadStories = getReadStories();
  if (existingReadStories.readStoryIds.includes(storyId)) return;

  existingReadStories.readStoryIds.push(storyId);
  setReadStories(existingReadStories);
};

export const updateDataWithReadStories = (data: CurrentData): CurrentData => {
  const existingReadStories = getReadStories();

  return {
    batchId: data.batchId,
    categorizedStories: data.categorizedStories.map((category) => ({
      category: category.category,
      stories: category.stories.map((story) => ({
        ...story,
        read: existingReadStories.readStoryIds.includes(story.id) ?? false,
      })),
    })),
    tensionIndex: data.tensionIndex,
  };
};

export const cleanUpStaleReadStories = (data: CurrentData) => {
  const existingReadStories = getReadStories();
  const updatedReadStories: ReadStoriesStorageObject = {
    batchId: "",
    readStoryIds: [],
  };

  if (data.batchId !== existingReadStories.batchId) {
    updatedReadStories.batchId = data.batchId;
    updatedReadStories.readStoryIds = [];
    setReadStories(updatedReadStories);
  }
};

// Normalize strings: convert dashes, remove emojis and broken surrogate pairs
const normalizeString = (value: string) =>
  value
    // Convert non-standard dashes to ASCII hyphen
    .replace(/[\u2014\u2013\u2012\u2011\u2010\u2212]/g, "-")
    // Convert various apostrophe-like characters to ASCII apostrophe
    .replace(/[\u2018\u2019\u201A\u201B\u2032\u02BC\uFF07]/g, "'")
    // Remove Private Use Area characters (icon fonts like Font Awesome)
    .replace(/[\uE000-\uF8FF]/g, "")
    // Remove orphaned/broken UTF-16 surrogate pairs
    .replace(
      /[\uD800-\uDBFF](?![\uDC00-\uDFFF])|(?<![\uD800-\uDBFF])[\uDC00-\uDFFF]/g,
      "",
    )
    // Remove properly encoded emojis
    .replace(
      /[\u{1F600}-\u{1F64F}\u{1F300}-\u{1F5FF}\u{1F680}-\u{1F6FF}\u{1F1E0}-\u{1F1FF}\u{2600}-\u{26FF}\u{2700}-\u{27BF}\u{1F900}-\u{1F9FF}\u{1FA00}-\u{1FA6F}\u{1FA70}-\u{1FAFF}\u{FE00}-\u{FE0F}\u{200D}]/gu,
      "",
    )
    // Remove replacement characters
    .replace(/\uFFFD/g, "")
    .trim();

const normalizeValue = (value: any): any => {
  if (typeof value === "string") return normalizeString(value);
  if (Array.isArray(value)) return value.map(normalizeValue);
  if (value && typeof value === "object") {
    const out: Record<string, any> = {};
    for (const key in value) {
      if (Object.prototype.hasOwnProperty.call(value, key)) {
        out[key] = normalizeValue(value[key]);
      }
    }
    return out;
  }
  return value;
};

const updateStories = async ({
  categoryId,
  maxStoryCount,
}: {
  categoryId: string;
  maxStoryCount: number;
}) => {
  let response:
    | { type: "smallWebFeed"; data: SmallWebFeedResponse }
    | { type: "news"; data: BatchCategoryStoriesResponse }
    | null = null;
  if (categoryId === "smallWebFeed") {
    response = { type: "smallWebFeed", data: await smallWebFeedRequest() };
  } else {
    response = {
      type: "news",
      data: await batchCategoryStoriesRequest({
        batchId: "latest",
        categoryId,
        limit: maxStoryCount,
      }),
    };
  }
  if (!response) return [];

  if (response.type === "smallWebFeed") {
    return response.data.entries.map(
      (entry) =>
        normalizeValue({
          id: entry.id,
          unique_domains: 1,
          read: false,
          category: "Small Web",
          title: entry.title,
          shortSummary: entry.short_summary,
          articles: entry.articles,
        }) as SimpleStory,
    );
  } else {
    return response.data.stories.map(
      (story) =>
        normalizeValue({
          id: story.id,
          unique_domains: story.unique_domains,
          read: false,
          category: story.category,
          title: story.title,
          shortSummary: story.short_summary,
          historical_background: story.historical_background,
          articles:
            story.articles?.map((article) => ({
              link: article.link,
              domain: article.domain,
            })) ?? [],
          perspectives:
            story.perspectives?.map((perspective) => ({
              text: perspective.text,
              sources:
                perspective.sources?.map((source) => ({
                  name: source.name,
                  url: source.url,
                })) ?? [],
            })) ?? [],
          talking_points:
            story.talking_points?.map((talkingPoint) => talkingPoint) ?? [],
          technical_details:
            story.technical_details?.map(
              (technicalDetail) => technicalDetail,
            ) ?? [],
          industry_impact: story.industry_impact?.map((impact) => impact) ?? [],
          suggested_qna:
            story.suggested_qna?.map((qna) => ({
              question: qna.question,
              answer: qna.answer,
            })) ?? [],
          timeline:
            story.timeline?.map((timeline) => ({
              date: timeline.date,
              content: timeline.content,
            })) ?? [],
          humanitarian_impact: story.humanitarian_impact,
          travel_advisory: story.travel_advisory ?? [],
          did_you_know: story.did_you_know ?? [],
          quote: story.quote
            ? {
                text: story.quote ?? "",
                author: story.quote_author ?? "",
                source: story.quote_attribution ?? "",
                url: story.quote_source_url ?? "",
              }
            : undefined,
          international_reactions: story.international_reactions ?? [],
          user_action_items: story.user_action_items ?? [],
          scientific_significance: story.scientific_significance ?? [],
          performance_statistics: story.performance_statistics ?? [],
        }) as SimpleStory,
    );
  }
};

const sortData = (data: CurrentData) => {
  const sortedCategories = [...data.categorizedStories].sort(
    (categorizedA, categorizedB) => {
      const aName = categorizedA.category.name;
      const bName = categorizedB.category.name;
      if (aName > bName) return 1;
      if (aName < bName) return -1;
      return 0;
    },
  );
  const processedCategories = sortedCategories.map((categorized) => ({
    ...categorized,
    stories: [...categorized.stories].sort(
      (a, b) => b.unique_domains - a.unique_domains,
    ),
  }));
  return { ...data, categorizedStories: processedCategories };
};

export const updateCategories = async ({
  selectedCategoryNames,
  maxStoryCount,
  onProgress,
}: {
  selectedCategoryNames: string[];
  maxStoryCount: number;
  onProgress?: (completedGroups: number, totalGroups: number) => void;
}) => {
  const categoriesResponse = await batchCategoriesRequest({
    batchId: "latest",
  });
  let filtered = categoriesResponse;
  if (selectedCategoryNames.length > 0) {
    filtered = {
      ...categoriesResponse,
      categories: categoriesResponse.categories.filter(
        (category: BatchCategory) =>
          selectedCategoryNames.includes(category.categoryName),
      ),
    };
  }
  if (getAdditionalFeeds().includes("Small Web")) {
    filtered.categories.push({
      id: "smallWebFeed",
      categoryName: "Small Web",
      categoryId: "smallWebFeed",
      sourceLanguage: "English",
      timestamp: 0,
      readCount: 0,
      clusterCount: "",
    });
  }

  const categorizedStories: CategorizedStories[] = [];
  const totalGroups = Math.max(
    1,
    Math.ceil(filtered.categories.length / CONCURRENT_BATCH_SIZE),
  );
  let completedGroups = 0;

  for (let j = 0; j < filtered.categories.length; j += CONCURRENT_BATCH_SIZE) {
    const catGroup = filtered.categories.slice(j, j + CONCURRENT_BATCH_SIZE);
    await Promise.all(
      catGroup.map(async (category: BatchCategory) => {
        const stories = await updateStories({
          categoryId: category.id,
          maxStoryCount,
        });
        categorizedStories.push({
          category: { id: category.id, name: category.categoryName },
          stories,
        });
      }),
    );

    completedGroups += 1;
    onProgress?.(completedGroups, totalGroups);
  }

  if (filtered.categories.length === 0) {
    onProgress?.(totalGroups, totalGroups);
  }

  const newMap = {
    batchId: categoriesResponse.batchId,
    categorizedStories,
    tensionIndex: categoryBatchMap.tensionIndex,
  };
  setCategoryBatchMap(sortData(newMap));
};

export const updateTension = async () => {
  const tensionData = await tensionRequest({
    batchId: "latest",
  });
  // Throw away the data if both values aren't defined or the description is empty
  if (
    tensionData.chaosIndex === undefined ||
    tensionData.chaosDescription === undefined ||
    tensionData.chaosDescription.length === 0
  ) {
    return;
  }

  setCategoryBatchMap({
    ...getCategoryBatchMap(),
    tensionIndex: {
      index: tensionData.chaosIndex,
      reason: normalizeValue(tensionData.chaosDescription),
    },
  });
};
