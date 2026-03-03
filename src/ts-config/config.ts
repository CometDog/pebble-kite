/**
 * Build the Clay configuration array.
 * @param categories - The list of category names to show as checkbox options
 */
export default function buildClayConfig(categories: string[] = []) {
  return [
    {
      type: "heading",
      defaultValue: "Settings",
    },
    {
      type: "section",
      items: [
        {
          type: "checkboxgroup",
          messageKey: "UserCategories",
          label: "Categories",
          description:
            "NOTE: Some categories may have limited content available. If there are no summaries for the day they will not show up in the app. Categories are automatically kept up to date from Kagi feeds. Selecting more than 20 categories is unsupported and will cause unexpected behavior.",
          defaultValue: categories.map(() => false),
          options: categories,
        },
        {
          type: "checkboxgroup",
          messageKey: "UserSections",
          label: "Article Sections",
          defaultValue: [
            true, // Highlights
            true, // Quote
            true, // Perspectives
            true, // Historical Background
            true, // Humanitarian Impact
            true, // Technical Details
            true, // Scientific Significance
            true, // Travel Advisory
            true, // Performance Statistics
            true, // Industry Impact
            true, // Timeline
            true, // International Reactions
            true, // Quick Questions
            true, // Action Items
            true, // Did You Know?
          ],
          options: [
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
          ],
        },
        {
          type: "toggle",
          messageKey: "ShowTensionIndex",
          label: "Show Tension Index",
          description:
            "Show the World Tension Index in the status bar when greater than 50. The World Tension Index is a number from 0 to 100 that represents the overall level of tension in the world based on the news. A higher number means more tension. Pressing up from the top of the Categories screen will show more details about the current tension level.",
          defaultValue: true,
        },
        {
          type: "checkboxgroup",
          messageKey: "AdditionalFeeds",
          label: "Additional Feeds",
          defaultValue: [false],
          description:
            "Feeds that are separate from News Feed within the Kagiverse. May not be localized in all languages. May be out of sync with the main news feed.",
          options: ["Small Web"],
        },
        {
          type: "select",
          messageKey: "UserInterfaceLanguage",
          label: "Interface Language",
          defaultValue: "",
          description:
            "Changes the language of the app interface. Some languages may require additional language packs to display correctly.",
          options: [
            { label: "Default (System Language)", value: "" },
            { label: "English (English)", value: "en_US" },
            { label: "Español (Spanish)", value: "es_ES" },
            { label: "Français (French)", value: "fr_FR" },
            { label: "Deutsch (German)", value: "de_DE" },
            { label: "Italiano (Italian)", value: "it_IT" },
            { label: "Português (Portuguese)", value: "pt_PT" },
            { label: "Русский (Russian)", value: "ru_RU" },
            { label: "日本語 (Japanese)", value: "ja_JP" },
          ],
        },
        {
          type: "select",
          messageKey: "UserContentLanguage",
          label: "Content Language",
          defaultValue: "",
          description:
            "Changes the language of the news content. Not all languages may have full content available. Some languages may require additional language packs to display correctly.",
          options: [
            { label: "Default (Original Language)", value: "" },
            { label: "English (English)", value: "en" },
            { label: "Italiano (Italian)", value: "it" },
            { label: "Português (Portuguese)", value: "pt" },
            {
              label: "Português Brasileiro (Brazilian Portuguese)",
              value: "pt-BR",
            },
            { label: "Français (French)", value: "fr" },
            { label: "Español (Spanish)", value: "es" },
            { label: "Deutsch (German)", value: "de" },
            { label: "日本語 (Japanese)", value: "ja" },
            { label: "हिन्दी (Hindi)", value: "hi" },
            { label: "Nederlands (Dutch)", value: "nl" },
            { label: "Українська (Ukrainian)", value: "uk" },
            { label: "Català (Catalan)", value: "ca" },
            { label: "Suomi (Finnish)", value: "fi" },
            { label: "한국어 (Korean)", value: "ko" },
            { label: "Lëtzebuergesch (Luxembourgish)", value: "lb" },
            { label: "Norsk (Norwegian)", value: "nb" },
            { label: "Polski (Polish)", value: "pl" },
            { label: "Русский (Russian)", value: "ru" },
            { label: "中文 (Simplified Chinese)", value: "zh-Hans" },
            { label: "中文 (Traditional Chinese)", value: "zh-Hant" },
            { label: "Svenska (Swedish)", value: "sv" },
            { label: "ภาษาไทย (Thai)", value: "th" },
            { label: "Türkçe (Turkish)", value: "tr" },
          ],
        },
        {
          type: "slider",
          messageKey: "UserMaxStories",
          label: "Number of stories shown",
          defaultValue: 12,
          min: 3,
          max: 12,
          step: 1,
        },
        {
          type: "select",
          messageKey: "UserTextSize",
          label: "Text Size",
          defaultValue: "",
          options: [
            { label: "Default (Follow Notification Size)", value: "0" },
            { label: "Small", value: "1" },
            { label: "Medium", value: "2" },
            { label: "Large", value: "3" },
          ],
        },
        {
          type: "checkboxgroup",
          messageKey: "UserTimelinePins",
          label: "Timeline Pins",
          defaultValue: [false],
          options: ["Show news refresh time pins"],
        },
      ],
    },
    {
      type: "section",
      items: [
        {
          type: "heading",
          defaultValue: "Developer",
        },
        {
          type: "toggle",
          messageKey: "DebugMode",
          label: "Debug Mode",
          description: "Show notifications for warnings and errors",
          defaultValue: false,
        },
        {
          type: "checkboxgroup",
          messageKey: "ClearCacheOnSave",
          label: "Clear Cache On Save",
          defaultValue: [false, false, false, false],
          options: [
            "Clear full cache",
            "Clear timeline pin memory",
            "Clear read stories memory",
            "Clear network cache",
          ],
        },
      ],
    },
    {
      type: "submit",
      defaultValue: "Save",
    },
  ];
}
