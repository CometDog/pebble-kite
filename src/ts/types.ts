export const PAGE_SIZE = 5;
export const CONCURRENT_BATCH_SIZE = 5;
export const SHORT_TITLE_LENGTH = 100;
export const QR_CODE_CHUNK_SIZE = 100;

export enum StoryDetailEnum {
  Background = "Background",
  Articles = "Articles",
  Perspectives = "Perspectives",
  TechnicalDetails = "TechnicalDetails",
  IndustryImpact = "IndustryImpact",
  TalkingPoints = "TalkingPoints",
  QnA = "QnA",
  Timeline = "Timeline",
  HumanitarianImpact = "HumanitarianImpact",
  TravelAdvisory = "TravelAdvisory",
  DidYouKnow = "DidYouKnow",
  Quote = "Quote",
  InternationalReactions = "InternationalReactions",
  UserActionItems = "UserActionItems",
  ScientificSignificance = "ScientificSignificance",
  DestinationHighlights = "DestinationHighlights",
  PerformanceStatistics = "PerformanceStatistics",
}
export type StoryDetailType = keyof typeof StoryDetailEnum;

export type SimpleStory = {
  id: string;
  unique_domains: number;
  read: boolean;
  category: string;
  title: string;
  shortSummary: string;
  historical_background: string;
  articles?: { link: string; domain: string }[];
  perspectives?: { text: string; sources: { name: string; url: string }[] }[];
  talking_points?: string[];
  technical_details?: string[];
  industry_impact?: string[];
  suggested_qna?: { question: string; answer: string }[];
  timeline?: { date: string; content: string }[];
  humanitarian_impact?: string;
  travel_advisory?: string[];
  did_you_know?: string;
  quote?: { text: string; author: string; source: string; url: string };
  geopolitical_context?: string;
  international_reactions?: string[];
  economic_implications?: string;
  future_outlook?: string;
  business_angle?: { text: string; points: string[] };
  user_action_items?: string[];
  scientific_significance?: string[];
  destination_highlights?: string;
  culinary_significance?: string;
  performance_statistics?: string[];
  league_standings?: string;
  diy_tips?: string;
  design_principles?: string;
  user_experience_impact?: string[];
  gameplay_mechanics?: string[];
};

export type CategorizedStories = {
  category: { id: string; name: string };
  stories: SimpleStory[];
};

export type CurrentData = {
  batchId: string;
  categorizedStories: CategorizedStories[];
  tensionIndex: { index: number; reason: string };
};

export type ReadStoriesStorageObject = {
  batchId: string;
  readStoryIds: string[];
};

export type AvailableCategory = string;

export const defaultCategories: AvailableCategory[] = [
  "World",
  "USA",
  "Business",
  "Technology",
  "Science",
  "Sports",
  "Gaming",
];

export const availableSections = [
  "Highlights",
  "Quote",
  "Perspectives",
  "Historical Background",
  "Humanitarian Impact",
  "Technical Details",
  "Scientific Significance",
  "Travel Advisory",
  "Performance Statistics",
  "Industry Impact",
  "Timeline",
  "International Reactions",
  "Quick Questions",
  "Action Items",
  "Did You Know?",
] as const;

export type AvailableSection = (typeof availableSections)[number];

export const defaultSections: Partial<AvailableSection>[] = [
  ...availableSections,
];

export const additionalFeeds = ["Small Web"] as const;

export type AdditionalFeature = (typeof additionalFeeds)[number];

export const defaultMaxStories = 12;

// Categories to exclude when setting available categories from Kagi News API
const EXCLUDED_CATEGORIES = ["OnThisDay"];

export const availableCategories: AvailableCategory[] = [...defaultCategories];

export const setAvailableCategories = (categories: AvailableCategory[]) => {
  const filtered = categories.filter(
    (category) => !EXCLUDED_CATEGORIES.includes(category),
  );
  availableCategories.length = 0;
  availableCategories.push(...filtered);
};
