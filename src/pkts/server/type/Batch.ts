export type Batch = {
  id: string;
  createdAt: string;
  totalCategories: number;
  totalClusters: number;
  totalArticles: number;
  totalReadCount: number;
  processingTime: number;
  languageCode: string;
  isComplete: boolean;
};
