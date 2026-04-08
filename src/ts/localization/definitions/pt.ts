import { InterfaceStrings } from "../type";

export const strings: InterfaceStrings = {
  general: {
    loading: "Carregando...",
    tensionIndex: "Tensão Mundial",
    tensionIndexShort: "Tensão",
    tensionIndexExplanationTitle: "O que é Tensão Mundial?",
    tensionIndexLevel1: "0-20° Fresco - Período de calma, atividade rotineira",
    tensionIndexLevel2: "21-40° Suave - Tensões globais normais",
    tensionIndexLevel3: "41-60° Quente - Preocupações elevadas",
    tensionIndexLevel4: "61-80° Muito Quente - Situações sérias",
    tensionIndexLevel5: "81-100° Crítico - Crise extrema",
    tensionIndexExplanation:
      "Este índice de Tensão Mundial é gerado por análises de IA dos títulos das notícias mundiais atuais. Ele não tem a intenção de causar alarme, mas sim fornecer uma visão geral das tensões globais a considerar ao visualizar as histórias de notícias.",
  },
  title: {
    feeds: "Feeds",
    news: "Notícias",
    category: "Categorias",
    stories: "Histórias",
    availableDetails: "Detalhes disponíveis",
  },
  section: {
    historicalBackground: "Contexto Histórico",
    humanitarianImpact: "Impacto Humanitário",
    didYouKnow: "Você Sabia?",
    quote: "Citação",
    highlights: "Destaques",
    perspectives: "Perspectivas",
    quickQuestions: "Perguntas Rápidas",
    technicalDetails: "Detalhes Técnicos",
    industryImpact: "Impacto na Indústria",
    timeline: "Linha do Tempo",
    travelAdvisory: "Aviso de Viagem",
    internationalReactions: "Reações Internacionais",
    actionItems: "Itens de Ação",
    scientificSignificance: "Significância Científica",
    performanceStatistics: "Estatísticas de Desempenho",
    sources: "Fontes",
  },
  category: {
    World: "Mundo",
    USA: "EUA",
    Business: "Negócios",
    Technology: "Tecnologia",
    Science: "Ciência",
    Sports: "Esportes",
    Gaming: "Jogos",
  },
  feature: {
    small_web_feed: "Pequena Web",
  },
} as const;
