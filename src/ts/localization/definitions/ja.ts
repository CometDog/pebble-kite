import { InterfaceStrings } from "../type";

export const strings: InterfaceStrings = {
  general: {
    loading: "読み込み中...",
    tensionIndex: "世界の緊張度",
    tensionIndexShort: "緊張度",
    tensionIndexExplanationTitle: "世界の緊張度とは？",
    tensionIndexLevel1: "0-20° クール - 落ち着いた期間、通常の活動",
    tensionIndexLevel2: "21-40° マイルド - 通常の世界的緊張",
    tensionIndexLevel3: "41-60° ウォーム - 懸念の高まり",
    tensionIndexLevel4: "61-80° ホット - 深刻な状況",
    tensionIndexLevel5: "81-100° クリティカル - 極端な危機",
    tensionIndexExplanation:
      "この世界の緊張度指数は、現在の世界のニュース見出しのAI分析によって生成されます。警戒を呼び起こすことを目的とするものではなく、ニュースストーリーを見る際に考慮すべき世界的な緊張の一般的なスナップショットを提供することを目的としています。",
  },
  title: {
    feeds: "フィード",
    news: "ニュース",
    category: "カテゴリー",
    stories: "ニュース",
    availableDetails: "利用可能な詳細",
  },
  section: {
    historicalBackground: "歴史的背景",
    humanitarianImpact: "人道的影響",
    didYouKnow: "知っていましたか？",
    quote: "引用",
    highlights: "ハイライト",
    perspectives: "視点",
    quickQuestions: "簡単な質問",
    technicalDetails: "技術的な詳細",
    industryImpact: "業界への影響",
    timeline: "出来事のタイムライン",
    travelAdvisory: "旅行アドバイザリー",
    internationalReactions: "国際的反応",
    actionItems: "アクション項目",
    scientificSignificance: "科学的意義",
    performanceStatistics: "パフォーマンス統計",
    sources: "出典",
  },
  category: {
    World: "世界",
    USA: "アメリカ",
    Business: "ビジネス",
    Technology: "テクノロジー",
    Science: "科学",
    Sports: "スポーツ",
    Gaming: "ゲーム",
  },
  feature: {
    small_web_feed: "小規模ウェブ",
  },
} as const;
