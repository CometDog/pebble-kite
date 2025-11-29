import { fetchJSON } from "../../../pktslib/fetch";
import { Batch } from "../type/Batch";

type BatchesResponse = {
  batches: Batch[];
};

export const batchesRequest = () =>
  fetchJSON<BatchesResponse>("https://news.kagi.com/api/batches");
