import { fetchJSON } from "../fetchUtils";
import { cacheFirstFetchJSON } from "../cache";
import { getServerLang } from "../localeManager";
import { BatchCategory } from "../type/BatchCategory";
import { Story } from "../type/Story";
import { StoryDomain } from "../type/StoryDomain";

type BatchesResponse = { id: string; createdAt: string };

export const batchesRequest = ({
  batchId = "latest",
}: {
  batchId?: string;
} = {}) =>
  fetchJSON<BatchesResponse>(
    `https://news.kagi.com/api/batches/${batchId}${getServerLang() && `?lang=${getServerLang()}`}`,
  );

type BatchCategoriesResponse = {
  batchId: string;
  createdAt: string;
  hasOnthisDay: boolean;
  categories: BatchCategory[];
};

export const batchCategoriesRequest = ({
  batchId = "latest",
}: {
  batchId?: string;
} = {}) =>
  cacheFirstFetchJSON<BatchCategoriesResponse>(
    `https://news.kagi.com/api/batches/${batchId}/categories${getServerLang() && `?lang=${getServerLang()}`}`,
  );

export type BatchCategoryStoriesResponse = {
  batchId: string;
  categoryId: string;
  categoryName: string;
  timestamp: number;
  stories: Story[];
  totalStories: string;
  domains: StoryDomain[];
  readCount: number;
};

export const batchCategoryStoriesRequest = ({
  batchId = "latest",
  categoryId,
  limit = 50,
}: {
  batchId?: string;
  categoryId: string;
  limit?: number;
}) =>
  cacheFirstFetchJSON<BatchCategoryStoriesResponse>(
    `https://news.kagi.com/api/batches/${batchId}/categories/${categoryId}/stories?limit=${limit}${getServerLang() && `&lang=${getServerLang()}`}`,
  );
