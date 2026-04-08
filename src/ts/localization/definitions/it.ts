import { InterfaceStrings } from "../type";

export const strings: InterfaceStrings = {
  general: {
    loading: "Caricamento...",
    tensionIndex: "Tensione Mondiale",
    tensionIndexShort: "Tensione",
    tensionIndexExplanationTitle: "Cos'è la Tensione Mondiale?",
    tensionIndexLevel1: "0-20° Fresco - Periodo di calma, attività di routine",
    tensionIndexLevel2: "21-40° Leggero - Tensioni globali normali",
    tensionIndexLevel3: "41-60° Caldo - Preoccupazioni elevate",
    tensionIndexLevel4: "61-80° Molto Caldo - Situazioni serie",
    tensionIndexLevel5: "81-100° Critico - Crisi estrema",
    tensionIndexExplanation:
      "Questo indice di Tensione Mondiale è generato da analisi AI dei titoli delle notizie mondiali attuali. Non è destinato a creare allarme, ma a fornire una panoramica generale delle tensioni globali da considerare quando si visualizzano le storie di notizie.",
  },
  title: {
    feeds: "Feed",
    news: "Notizie",
    category: "Categorie",
    stories: "Storie",
    availableDetails: "Dettagli disponibili",
  },
  section: {
    historicalBackground: "Contesto Storico",
    humanitarianImpact: "Impatto Umanitario",
    didYouKnow: "Lo Sapevi?",
    quote: "Citazione",
    highlights: "Punti Salienti",
    perspectives: "Prospettive",
    quickQuestions: "Domande Rapide",
    technicalDetails: "Dettagli Tecnici",
    industryImpact: "Impatto sul Settore",
    timeline: "Cronologia",
    travelAdvisory: "Avviso di Viaggio",
    internationalReactions: "Reazioni Internazionali",
    actionItems: "Punti d'Azione",
    scientificSignificance: "Importanza Scientifica",
    performanceStatistics: "Statistiche delle Prestazioni",
    sources: "Fonti",
  },
  category: {
    World: "Mondo",
    USA: "USA",
    Business: "Economia",
    Technology: "Tecnologia",
    Science: "Scienza",
    Sports: "Sport",
    Gaming: "Videogiochi",
  },
  feature: {
    small_web_feed: "Piccolo Web",
  },
} as const;
