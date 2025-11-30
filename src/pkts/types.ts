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
};

export const availableCategories = [
  "AI",
  "Africa",
  "Apple",
  "Australia",
  "Austria",
  "Bay",
  "Belgium",
  "Bitcoin",
  "Brazil",
  "Business",
  "Canada",
  "Catholic",
  "China",
  "Coffee",
  "Colombia",
  "Costa Rica",
  "Croatia",
  "Cryptocurrency",
  "Cybersecurity",
  "Czech Republic",
  "Denmark",
  "Economy",
  "Estonia",
  "Europe",
  "Finland",
  "Formula 1",
  "France",
  "Gaming",
  "Germany | Hesse",
  "Germany",
  "Greece",
  "Hungary",
  "India",
  "Iran",
  "Ireland",
  "Israel",
  "Italy",
  "Japan",
  "Linux & OSS",
  "Lithuania",
  "Mexico",
  "Middle East",
  "NFL",
  "NHL",
  "New Zealand",
  "Nintendo",
  "Norway",
  "Pakistan",
  "Palestine",
  "Philippines",
  "Podcasting",
  "Poland",
  "Portugal",
  "Romania",
  "Russia",
  "Science",
  "Serbia",
  "Singapore",
  "Slovakia",
  "Slovenia",
  "South Korea",
  "Spain",
  "Sports",
  "Sweden",
  "Switzerland (DE)",
  "Switzerland (FR)",
  "Taiwan",
  "Technology",
  "Thailand",
  "The Netherlands",
  "Turkey",
  "UK",
  "Ukraine",
  "USA | Austin, TX",
  "USA | Chicago",
  "USA | Colorado",
  "USA | Michigan",
  "USA | Minnesota",
  "USA | New York City",
  "USA | Ohio",
  "USA | Utah",
  "USA | Virginia",
  "USA",
  "iRacing & Sim Racing",
  "World",
] as const;

export const defaultCategories = [
  "World",
  "USA",
  "Business",
  "Technology",
  "Science",
  "Sports",
  "Gaming",
];
