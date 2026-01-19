import { cacheFirstFetchJSON } from "../cache/cacheFirstFetch";
import { getServerLang } from "../localeManager";
import { OnThisDayEvents } from "../type/OnThisDayEvents";

export const onThisDayRequest = ({
  batchId = "latest",
}: {
  batchId?: string;
} = {}) =>
  cacheFirstFetchJSON<OnThisDayEvents>(
    `https://news.kagi.com/api/batches/${batchId}/onthisday${getServerLang() && `?lang=${getServerLang()}`}`,
  );
