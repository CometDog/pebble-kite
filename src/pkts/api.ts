import { batchesRequest } from "./server/request/batches";
import { batchCategoriesRequest } from "./server/request/batchCategories";
import { batchCategoryStoriesRequest } from "./server/request/batchCategoryStories";
import type { CurrentData, CategorizedStories, SimpleStory } from "./types";
import type { BatchCategory } from "./server/type/BatchCategory";
import { CONCURRENT_BATCH_SIZE } from "./types";

let categoryBatchMap: CurrentData[] = [];

export const getCategoryBatchMap = () => categoryBatchMap;

export const setCategoryBatchMap = (data: CurrentData[]) => {
  categoryBatchMap = data;
};

// Normalize strings: convert dashes, remove emojis and broken surrogate pairs
const normalizeString = (value: string) =>
  value
    // Convert non-standard dashes to ASCII hyphen
    .replace(/[\u2014\u2013\u2012\u2011\u2010\u2212]/g, "-")
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
  batchId,
  categoryId,
}: {
  batchId: string;
  categoryId: string;
}) => {
  const response = await batchCategoryStoriesRequest({ batchId, categoryId });
  return response.stories.map(
    (story) =>
      normalizeValue({
        id: story.id,
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
          story.technical_details?.map((technicalDetail) => technicalDetail) ??
          [],
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
        quote:
          story.quote && story.quote_attribution
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
};

const sortData = (data: CurrentData[]) => {
  return data.map((batch) => {
    const sortedCategories = [...batch.categorizedStories].sort(
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
      stories: [...categorized.stories].sort((a, b) => {
        if (a.id > b.id) return 1;
        if (a.id < b.id) return -1;
        return 0;
      }),
    }));
    return { ...batch, categorizedStories: processedCategories };
  });
};

export const updateCategories = async (selectedCategoryNames: string[]) => {
  const response = await batchesRequest();
  const batchIds = response.batches.map((b) => b.id);

  const newMap: CurrentData[] = [];

  for (let i = 0; i < batchIds.length; i += CONCURRENT_BATCH_SIZE) {
    const group = batchIds.slice(i, i + CONCURRENT_BATCH_SIZE);
    await Promise.all(
      group.map(async (batchId) => {
        const categoriesResponse = await batchCategoriesRequest({ batchId });
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

        const categorizedStories: CategorizedStories[] = [];

        for (
          let j = 0;
          j < filtered.categories.length;
          j += CONCURRENT_BATCH_SIZE
        ) {
          const catGroup = filtered.categories.slice(
            j,
            j + CONCURRENT_BATCH_SIZE,
          );
          await Promise.all(
            catGroup.map(async (category: BatchCategory) => {
              const stories = await updateStories({
                batchId,
                categoryId: category.id,
              });
              categorizedStories.push({
                category: { id: category.id, name: category.categoryName },
                stories,
              });
            }),
          );
        }

        newMap.push({ batchId, categorizedStories });
      }),
    );
  }

  setCategoryBatchMap(sortData(newMap));
};
