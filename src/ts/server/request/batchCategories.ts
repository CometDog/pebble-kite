import { cacheFirstFetchJSON } from "../cache/cacheFirstFetch";
import { getServerLang } from "../localeManager";
import { BatchCategory } from "../type/BatchCategory";

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
