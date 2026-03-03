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

export const availableCategories = [
  "3D Printing",
  "Africa",
  "AI",
  "Apple",
  "Argentina",
  "Asia",
  "Australia",
  "Austria",
  "Bay Area",
  "Belgium",
  "Bitcoin",
  "Brazil",
  "Business",
  "Canada",
  "Catholic",
  "China",
  "Climate Change",
  "Colombia",
  "Costa Rica",
  "Croatia",
  "Cryptocurrency",
  "Cybersecurity",
  "Cycling",
  "Czech Republic",
  "Defense",
  "Denmark",
  "Economy",
  "Estonia",
  "Europe",
  "Finland | Helsinki",
  "Finland",
  "Formula 1",
  "France",
  "French Music",
  "Gaming",
  "Germany | Baden-Württemberg",
  "Germany | Hamburg",
  "Germany | Hesse",
  "Germany",
  "Google",
  "Greece",
  "Healthcare | USA",
  "Hong Kong",
  "Hungary",
  "India",
  "Iran",
  "Ireland",
  "Israel",
  "Italy",
  "Japan",
  "Linux & OSS",
  "Lithuania",
  "Metal Music",
  "Mexico",
  "Microsoft",
  "Middle East",
  "MLB",
  "MMA",
  "Morocco",
  "Movies",
  "Music",
  "New Zealand",
  "NFL",
  "NHL",
  "Nintendo",
  "North Macedonia",
  "Norway",
  "Pakistan",
  "Palestine",
  "Philippines",
  "Podcasting",
  "Poland",
  "Portugal",
  "Privacy",
  "Professional Wrestling",
  "Romania",
  "Russia",
  "Science",
  "Serbia",
  "Simulation Games",
  "Singapore",
  "Slovakia",
  "Slovenia",
  "South Korea",
  "Spain",
  "Sports",
  "Sweden",
  "Switzerland (DE)",
  "Switzerland (FR)",
  "Switzerland (IT)",
  "Taiwan",
  "Technology",
  "Thailand",
  "The Netherlands",
  "Turkey",
  "U.S. Soccer",
  "UK",
  "Ukraine",
  "USA | Boston",
  "USA | Chicago",
  "USA | Colorado",
  "USA | Florida",
  "USA | Michigan",
  "USA | Minnesota",
  "USA | Nevada",
  "USA | New Mexico",
  "USA | New York City",
  "USA | Ohio",
  "USA | Oregon",
  "USA | Utah",
  "USA | Vermont",
  "USA | Virginia",
  "USA | Washington",
  "USA",
  "Venezuela",
  "Watches & Horology",
  "World",
] as const;

export type AvailableCategory = (typeof availableCategories)[number];

export const defaultCategories: Partial<AvailableCategory>[] = [
  "World",
  "USA",
  "Business",
  "Technology",
  "Science",
  "Sports",
  "Gaming",
] as const;

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
