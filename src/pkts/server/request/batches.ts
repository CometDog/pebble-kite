import { fetchJSON } from "../../../pktslib/fetch";
import { getServerLang } from "../localeManager";
import { Batch } from "../type/Batch";

type BatchesResponse = {
  batches: Batch[];
};

export const batchesRequest = () =>
  fetchJSON<BatchesResponse>(
    `https://news.kagi.com/api/batches${getServerLang() && `?lang=${getServerLang()}`}`,
  );
