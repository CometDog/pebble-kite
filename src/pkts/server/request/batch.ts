import { fetchJSON } from "../../../pktslib/fetch";
import { getServerLang } from "../localeManager";
import { Batch } from "../type/Batch";

export const batchRequest = ({ batchId }: { batchId: string }) =>
  fetchJSON<Batch>(
    `https://news.kagi.com/api/batches/${batchId}${getServerLang() && `?lang=${getServerLang()}`}`,
  );
