import { cacheFirstFetchJSON } from "../cache/cacheFirstFetch";
import { fetchJSON } from "../fetchUtils";
import { getServerLang } from "../localeManager";

type TensionResponse = {
  chaosIndex: number;
  chaosDescription: string;
  chaosLastUpdated: string;
};

export const tensionRequest = ({ batchId = "latest" }: { batchId?: string }) =>
  cacheFirstFetchJSON<TensionResponse>(
    `https://news.kagi.com/api/batches/${batchId}/chaos${getServerLang() && `?lang=${getServerLang()}`}`,
  );
