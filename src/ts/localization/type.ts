export type InterfaceStrings = {
  general: {
    loading: string;
    tensionIndex: string;
    tensionIndexShort: string;
    tensionIndexExplanationTitle: string;
    tensionIndexLevel1: string;
    tensionIndexLevel2: string;
    tensionIndexLevel3: string;
    tensionIndexLevel4: string;
    tensionIndexLevel5: string;
    tensionIndexExplanation: string;
  };
  title: {
    feeds: string;
    news: string;
    category: string;
    stories: string;
    availableDetails: string;
  };
  section: {
    historicalBackground: string;
    humanitarianImpact: string;
    didYouKnow: string;
    quote: string;
    highlights: string;
    perspectives: string;
    quickQuestions: string;
    technicalDetails: string;
    industryImpact: string;
    timeline: string;
    travelAdvisory: string;
    internationalReactions: string;
    actionItems: string;
    scientificSignificance: string;
    performanceStatistics: string;
    sources: string;
  };
  category: Record<string, string> & {
    World: string;
    USA: string;
    Business: string;
    Technology: string;
    Science: string;
    Sports: string;
    Gaming: string;
  };
  feature: {
    small_web_feed: string;
  };
};
