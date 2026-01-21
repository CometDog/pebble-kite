import { fetchJSON } from "../fetchUtils";
import { getServerLang } from "../localeManager";

type BatchesResponse = { id: string; createdAt: string };

export const batchesRequest = () =>
  fetchJSON<BatchesResponse>(
    `https://news.kagi.com/api/batches/latest${getServerLang() && `?lang=${getServerLang()}`}`,
  );
