import { fetchJSON } from "../../../pktslib/fetch";
import { getServerLang } from "../localeManager";
import { OnThisDayEvents } from "../type/OnThisDayEvents";

export const onThisDayRequest = ({
  batchId = "latest",
}: {
  batchId?: string;
}) =>
  fetchJSON<OnThisDayEvents>(
    `https://news.kagi.com/api/batches/${batchId}/onthisday${getServerLang() && `?lang=${getServerLang()}`}`,
  );
