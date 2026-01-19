import { fetchJSON } from "../fetchUtils";
import { getServerLang } from "../localeManager";

type BatchesResponse = { batches: [{ id: string; createdAt: string }] };

export const batchesRequest = () =>
  fetchJSON<BatchesResponse>(
    `https://news.kagi.com/api/batches${getServerLang() && `?lang=${getServerLang()}`}`,
  );
