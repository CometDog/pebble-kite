import { fetchJSON } from "../fetchUtils";
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
}) =>
  fetchJSON<BatchCategoriesResponse>(
    `https://news.kagi.com/api/batches/${batchId}/categories${getServerLang() && `?lang=${getServerLang()}`}`,
  );
