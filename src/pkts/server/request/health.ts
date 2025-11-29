import { fetchJSON } from "../../../pktslib/fetch";

type HealthResponse = {
  health: boolean;
  hasBatch: boolean;
  hasCategories: boolean;
  hasTranslations: boolean;
  isRecent: boolean;
  batchId: string;
  createdAt: string;
};

export const healthRequest = () =>
  fetchJSON<HealthResponse>("https://news.kagi.com/api/health");
