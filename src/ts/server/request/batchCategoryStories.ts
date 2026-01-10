import { getServerLang } from "../localeManager";
import { Story } from "../type/Story";
import { StoryDomain } from "../type/StoryDomain";
import { fetchJSON } from "../fetchUtils";

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
  fetchJSON<BatchCategoryStoriesResponse>(
    `https://news.kagi.com/api/batches/${batchId}/categories/${categoryId}/stories?limit=${limit}${getServerLang() && `&lang=${getServerLang()}`}`,
  );
