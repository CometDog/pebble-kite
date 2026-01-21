import { fetchJSON } from "../fetchUtils";
import { getServerLang } from "../localeManager";

type BatchesResponse = { id: string; createdAt: string };

export const batchesRequest = ({
  batchId = "latest",
}: {
  batchId?: string;
} = {}) =>
  fetchJSON<BatchesResponse>(
    `https://news.kagi.com/api/batches/${batchId}${getServerLang() && `?lang=${getServerLang()}`}`,
  );
