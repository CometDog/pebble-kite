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
  fetch("https://news.kagi.com/api/health").then(async (res) => {
    return res.ok
      ? ((await res.json()) as HealthResponse)
      : Promise.reject(res);
  });
