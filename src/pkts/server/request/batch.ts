import { fetchJSON } from "../../../pktslib/fetch";
import { Batch } from "../type/Batch";

export const batchRequest = ({ batchId }: { batchId: string }) =>
  fetchJSON<Batch>(`https://news.kagi.com/api/batches/${batchId}`);
