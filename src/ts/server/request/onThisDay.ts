import { getServerLang } from "../localeManager";
import { OnThisDayEvents } from "../type/OnThisDayEvents";

export const onThisDayRequest = ({
  batchId = "latest",
}: {
  batchId?: string;
}) =>
  fetch(
    `https://news.kagi.com/api/batches/${batchId}/onthisday${getServerLang() && `?lang=${getServerLang()}`}`,
  ).then(async (res) => {
    return res.ok ? await res.json() as OnThisDayEvents : Promise.reject(res);
  });