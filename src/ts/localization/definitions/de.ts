import { InterfaceStrings } from "../type";

export const strings: InterfaceStrings = {
  general: {
    loading: "Laden...",
    tensionIndex: "Weltweite Spannung",
    tensionIndexShort: "Spannung",
    tensionIndexExplanationTitle: "Was ist weltweite Spannung?",
    tensionIndexLevel1: "0-20° Kühl - Ruhige Phase, routinemäßige Aktivität",
    tensionIndexLevel2: "21-40° Mild - Normale globale Spannungen",
    tensionIndexLevel3: "41-60° Warm - Erhöhte Besorgnis",
    tensionIndexLevel4: "61-80° Heiß - Ernsthafte Situationen",
    tensionIndexLevel5: "81-100° Kritisch - Extreme Krise",
    tensionIndexExplanation:
      "Dieser Weltspannungsindex wird durch KI-Analyse der aktuellen weltweiten Nachrichtenüberschriften generiert. Er soll keine Alarmstimmung erzeugen, sondern einen allgemeinen Überblick über die globalen Spannungen bieten, die bei der Betrachtung von Nachrichten berücksichtigt werden sollten.",
  },
  title: {
    feeds: "Feeds",
    news: "Nachrichten",
    category: "Kategorien",
    stories: "Geschichten",
    availableDetails: "Verfügbare Details",
  },
  section: {
    historicalBackground: "Historischer Hintergrund",
    humanitarianImpact: "Humanitäre Auswirkungen",
    didYouKnow: "Wussten Sie Schon?",
    quote: "Zitat",
    highlights: "Höhepunkte",
    perspectives: "Perspektiven",
    quickQuestions: "Schnelle Fragen",
    technicalDetails: "Technische Details",
    industryImpact: "Auswirkungen auf die Branche",
    timeline: "Zeitleiste",
    travelAdvisory: "Reisehinweis",
    internationalReactions: "Internationale Reaktionen",
    actionItems: "Aufgaben",
    scientificSignificance: "Wissenschaftliche Bedeutung",
    performanceStatistics: "Leistungsstatistik",
    sources: "Quellen",
  },
  category: {
    World: "Welt",
    USA: "USA",
    Business: "Business",
    Technology: "Technologie",
    Science: "Wissenschaft",
    Sports: "Sport",
    Gaming: "Gaming",
  },
  feature: {
    small_web_feed: "Kleiner Web",
  },
} as const;
