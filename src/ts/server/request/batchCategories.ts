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
  fetch(
    `https://news.kagi.com/api/batches/${batchId}/categories${getServerLang() && `?lang=${getServerLang()}`}`,
  ).then(async (res) => {
    return res.ok ? await res.json() as BatchCategoriesResponse : Promise.reject(res);
  });
