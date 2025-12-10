export default [
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
        defaultValue: [
          false, // AI
          false, // Africa
          false, // Apple
          false, // Australia
          false, // Austria
          false, // Bay
          false, // Belgium
          false, // Bitcoin
          false, // Brazil
          false, // Business
          false, // Canada
          false, // Catholic
          false, // China
          false, // Coffee
          false, // Colombia
          false, // Costa Rica
          false, // Croatia
          false, // Cryptocurrency
          false, // Cybersecurity
          false, // Czech Republic
          false, // Denmark
          false, // Economy
          false, // Estonia
          false, // Europe
          false, // Finland
          false, // Formula 1
          false, // France
          false, // Gaming
          false, // Germany | Hesse
          false, // Germany
          false, // Greece
          false, // Hungary
          false, // India
          false, // Iran
          false, // Ireland
          false, // Israel
          false, // Italy
          false, // Japan
          false, // Linux & OSS
          false, // Lithuania
          false, // Mexico
          false, // Middle East
          false, // NFL
          false, // NHL
          false, // New Zealand
          false, // Nintendo
          false, // Norway
          false, // Pakistan
          false, // Palestine
          false, // Philippines
          false, // Podcasting
          false, // Poland
          false, // Portugal
          false, // Romania
          false, // Russia
          false, // Science
          false, // Serbia
          false, // Singapore
          false, // Slovakia
          false, // Slovenia
          false, // South Korea
          false, // Spain
          false, // Sports
          false, // Sweden
          false, // Switzerland (DE)
          false, // Switzerland (FR)
          false, // Taiwan
          false, // Technology
          false, // Thailand
          false, // The Netherlands
          false, // Turkey
          false, // UK
          false, // Ukraine
          false, // USA | Austin, TX
          false, // USA | Chicago
          false, // USA | Colorado
          false, // USA | Michigan
          false, // USA | Minnesota
          false, // USA | New York City
          false, // USA | Ohio
          false, // USA | Utah
          false, // USA | Virginia
          false, // USA
          false, // iRacing & Sim Racing
          false, // World
        ],
        options: [
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
        ],
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
          true, // Action items,
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
          "Action items",
          "Did You Know?",
        ],
      },
      {
        type: "select",
        messageKey: "UserInterfaceLanguage",
        label: "Interface Language",
        defaultValue: "",
        options: [
          { label: "Default (System Language)", value: "" },
          { label: "English (English)", value: "en_US" },
          { label: "Español (Spanish)", value: "es_ES" },
          { label: "Français (French)", value: "fr_FR" },
          { label: "Deutsch (German)", value: "de_DE" },
          { label: "Italiano (Italian)", value: "it_IT" },
          { label: "Português (Portuguese)", value: "pt_PT" },
        ],
      },
      {
        type: "select",
        messageKey: "UserContentLanguage",
        label: "Content Language",
        defaultValue: "",
        options: [
          { label: "Default (Original Language)", value: "" },
          { label: "English (English)", value: "en" },
          { label: "Español (Spanish)", value: "es" },
          { label: "Français (French)", value: "fr" },
          { label: "Deutsch (German)", value: "de" },
          { label: "Italiano (Italian)", value: "it" },
          { label: "Português (Portuguese)", value: "pt" },
          {
            label: "Português Brasileiro (Brazilian Portuguese)",
            value: "pt-BR",
          },
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
        defaultValue: [false, false, false],
        options: [
          "Clear full cache",
          "Clear timeline pin memory",
          "Clear read stories memory",
        ],
      },
    ],
  },
  {
    type: "submit",
    defaultValue: "Save",
  },
];
