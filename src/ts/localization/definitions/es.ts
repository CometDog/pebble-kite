import { InterfaceStrings } from "../type";

export const strings: InterfaceStrings = {
  general: {
    loading: "Cargando...",
    tensionIndex: "Tensión Mundial",
    tensionIndexShort: "Tensión",
    tensionIndexExplanationTitle: "¿Qué es la Tensión Mundial?",
    tensionIndexLevel1: "0-20° Fresco - Período de calma, actividad rutinaria",
    tensionIndexLevel2: "21-40° Suave - Tensiones globales normales",
    tensionIndexLevel3: "41-60° Cálido - Preocupaciones elevadas",
    tensionIndexLevel4: "61-80° Caliente - Situaciones serias",
    tensionIndexLevel5: "81-100° Crítico - Crisis extrema",
    tensionIndexExplanation:
      "Este índice de Tensión Mundial se genera mediante análisis de IA de los titulares de noticias mundiales actuales. No está destinado a generar alarma, sino a proporcionar una instantánea general de las tensiones globales para considerar al ver las historias de noticias.",
  },
  title: {
    feeds: "Fuentes",
    news: "Noticias",
    category: "Categorías",
    stories: "Historias",
    availableDetails: "Detalles disponibles",
  },
  section: {
    historicalBackground: "Antecedentes Históricos",
    humanitarianImpact: "Impacto Humanitario",
    didYouKnow: "¿Sabías Que?",
    quote: "Cita",
    highlights: "Aspectos Destacados",
    perspectives: "Perspectivas",
    quickQuestions: "Preguntas Rápidas",
    technicalDetails: "Detalles Técnicos",
    industryImpact: "Impacto en la Industria",
    timeline: "Cronología",
    travelAdvisory: "Aviso de Viaje",
    internationalReactions: "Reacciones Internacionales",
    actionItems: "Puntos de Acción",
    scientificSignificance: "Relevancia Científica",
    performanceStatistics: "Estadísticas de Rendimiento",
    sources: "Fuentes",
  },
  category: {
    World: "Mundo",
    USA: "EE. UU.",
    Business: "Negocios",
    Technology: "Tecnología",
    Science: "Ciencia",
    Sports: "Deportes",
    Gaming: "Videojuegos",
  },
  feature: {
    small_web_feed: "Pequeña Fuente",
  },
} as const;
