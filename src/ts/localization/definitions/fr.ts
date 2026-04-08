import { InterfaceStrings } from "../type";

export const strings: InterfaceStrings = {
  general: {
    loading: "Chargement...",
    tensionIndex: "Tension Mondiale",
    tensionIndexShort: "Tension",
    tensionIndexExplanationTitle: "Qu'est-ce que la Tension Mondiale ?",
    tensionIndexLevel1: "0-20° Frais - Période calme, activité routinière",
    tensionIndexLevel2: "21-40° Doux - Tensions mondiales normales",
    tensionIndexLevel3: "41-60° Chaud - Préoccupations élevées",
    tensionIndexLevel4: "61-80° Brûlant - Situations sérieuses",
    tensionIndexLevel5: "81-100° Critique - Crise extrême",
    tensionIndexExplanation:
      "Cet indice de Tension Mondiale est généré par des analyses d'IA des titres d'actualités mondiales actuels. Il n'est pas destiné à susciter l'alarme, mais à fournir une vue d'ensemble des tensions globales à considérer lors de la lecture des histoires d'actualités.",
  },
  title: {
    feeds: "Flux",
    news: "Actualités",
    category: "Catégories",
    stories: "Histoires",
    availableDetails: "Détails disponibles",
  },
  section: {
    historicalBackground: "Contexte Historique",
    humanitarianImpact: "Impact Humanitaire",
    didYouKnow: "Le Saviez-Vous ?",
    quote: "Citation",
    highlights: "Faits Saillants",
    perspectives: "Perspectives",
    quickQuestions: "Questions Rapides",
    technicalDetails: "Détails Techniques",
    industryImpact: "Impact sur l'Industrie",
    timeline: "Chronologie",
    travelAdvisory: "Avis aux Voyageurs",
    internationalReactions: "Réactions Internationales",
    actionItems: "Actions à Mener",
    scientificSignificance: "Importance Scientifique",
    performanceStatistics: "Statistiques de Performance",
    sources: "Sources",
  },
  category: {
    World: "Monde",
    USA: "États-Unis",
    Business: "Affaires",
    Technology: "Technologie",
    Science: "Science",
    Sports: "Sports",
    Gaming: "Jeux Vidéo",
  },
  feature: {
    small_web_feed: "Petit Flux",
  },
} as const;
