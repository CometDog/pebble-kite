import { InterfaceStrings } from "../type";

export const strings: InterfaceStrings = {
  general: {
    loading: "Загрузка...",
    tensionIndex: "Мировая напряженность",
    tensionIndexShort: "Напряженность",
    tensionIndexExplanationTitle: "Что такое мировая напряженность?",
    tensionIndexLevel1:
      "0-20° Прохладно - Период спокойствия, рутинная деятельность",
    tensionIndexLevel2: "21-40° Умеренно - Нормальная мировая напряженность",
    tensionIndexLevel3: "41-60° Тепло - Повышенные опасения",
    tensionIndexLevel4: "61-80° Горячо - Серьезные ситуации",
    tensionIndexLevel5: "81-100° Критично - Экстремальный кризис",
    tensionIndexExplanation:
      "Этот индекс мировой напряженности генерируется с помощью ИИ-аналитики текущих мировых новостных заголовков. Он не предназначен для создания паники, а для предоставления общего представления о глобальной напряженности, которую следует учитывать при просмотре новостных историй.",
  },
  title: {
    feeds: "Ленты",
    news: "Новости",
    category: "Категории",
    stories: "Новости",
    availableDetails: "Доступные детали",
  },
  section: {
    historicalBackground: "Историческая справка",
    humanitarianImpact: "Гуманитарное воздействие",
    didYouKnow: "Знаете ли вы?",
    quote: "Цитаты",
    highlights: "Основные моменты",
    perspectives: "Перспективы",
    quickQuestions: "Быстрые вопросы",
    technicalDetails: "Технические детали",
    industryImpact: "Воздействие на отрасль",
    timeline: "Хроника событий",
    travelAdvisory: "Советы для путешествующих",
    internationalReactions: "Международная реакция",
    actionItems: "Пункты действий",
    scientificSignificance: "Научное значение",
    performanceStatistics: "Статистика эффективности",
    sources: "Источники",
  },
  category: {
    World: "Мир",
    USA: "США",
    Business: "Бизнес",
    Technology: "Технологии",
    Science: "Наука",
    Sports: "Спорт",
    Gaming: "Игры",
  },
  feature: {
    small_web_feed: "Малый веб",
  },
} as const;
