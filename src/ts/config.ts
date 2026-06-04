import {
  AvailableCategory,
  AvailableSection,
  AdditionalFeature,
  availableCategories,
  availableSections,
  defaultCategories,
  defaultMaxStories,
  defaultSections,
  additionalFeeds,
} from "./types";
import { createLogger } from "../lib/logger";

const log = createLogger("KNJSConfig");

/**
 * Application configuration interface
 * @property selectedAdditionalFeeds The selected additional feeds
 * @property newsRefreshPinSetting Whether the news refresh Timeline pin is enabled
 * @property selectedTextSize The selected text size
 * @property selectedInterfaceLanguage The selected interface language
 * @property selectedContentLanguage The selected content language
 * @property selectedMaxStoryCount The selected maximum story count
 * @property selectedSectionNames The enabled section names
 * @property selectedCategoryNames The enabled category names
 * @property isTensionIndexEnabled Whether the tension index is enabled
 */
export interface AppConfig {
  selectedAdditionalFeeds: AdditionalFeature[];
  newsRefreshPinSetting: boolean;
  selectedTextSize: number;
  selectedInterfaceLanguage: string;
  selectedContentLanguage: string;
  selectedMaxStoryCount: number;
  selectedSectionNames: AvailableSection[];
  selectedCategoryNames: AvailableCategory[];
  isTensionIndexEnabled: boolean;
}

const CONFIG_DEFAULTS: AppConfig = {
  selectedAdditionalFeeds: [],
  newsRefreshPinSetting: false,
  selectedTextSize: 0,
  selectedInterfaceLanguage: "",
  selectedContentLanguage: "",
  selectedMaxStoryCount: defaultMaxStories,
  selectedSectionNames: defaultSections,
  selectedCategoryNames: defaultCategories,
  isTensionIndexEnabled: true,
};

const CONFIG_KEY = "kagi_news_config";

// Legacy per-key storage used before the config was unified into a single blob.
// Property names match the old CONFIG_KEYS entries, which were identical to the property names.
const LEGACY_KEYS: (keyof AppConfig)[] = [
  "selectedAdditionalFeeds",
  "newsRefreshPinSetting",
  "selectedTextSize",
  "selectedInterfaceLanguage",
  "selectedContentLanguage",
  "selectedMaxStoryCount",
  "selectedSectionNames",
  "selectedCategoryNames",
  "isTensionIndexEnabled",
];

const migrateFromLegacyStorage = (): AppConfig => {
  const migrated: Partial<AppConfig> = {};
  let didMigrate = false;
  for (const key of LEGACY_KEYS) {
    const raw = localStorage.getItem(key);
    if (raw) {
      try {
        (migrated as any)[key] = JSON.parse(raw);
        didMigrate = true;
      } catch {}
    }
  }
  const config = { ...CONFIG_DEFAULTS, ...migrated };
  if (didMigrate) {
    localStorage.setItem(CONFIG_KEY, JSON.stringify(config));
    LEGACY_KEYS.forEach((key) => localStorage.removeItem(key));
    log.info("Migrated config from per-key storage to single blob");
  }
  return config;
};

export const getAppConfig = (): AppConfig => {
  try {
    const stored = localStorage.getItem(CONFIG_KEY);
    if (stored) {
      // Merge with defaults to handle new keys added in future versions
      return { ...CONFIG_DEFAULTS, ...JSON.parse(stored) };
    }
    return migrateFromLegacyStorage();
  } catch (err) {
    log.warn(
      "Failed to read config, using defaults",
      err instanceof Error ? err.message : String(err),
    );
    return { ...CONFIG_DEFAULTS };
  }
};

/**
 * Save a configuration to localStorage
 * @param config The configuration to save
 */
export const saveAppConfig = (config: AppConfig): void => {
  try {
    localStorage.setItem(CONFIG_KEY, JSON.stringify(config));
    log.debug("Config saved");
  } catch (err) {
    log.error(
      "Failed to save config",
      err instanceof Error ? err.message : String(err),
    );
  }
};

/**
 * Convert category names to boolean array for Clay
 * @param categoryNames The category names to convert (if not provided, uses stored config)
 * @returns Boolean array representing selected categories
 */
export const categoriesToBoolean = (
  categoryNames?: AvailableCategory[],
): boolean[] => {
  const names = categoryNames ?? getAppConfig().selectedCategoryNames;
  return availableCategories.map((cat) => names.includes(cat));
};

/**
 * Convert section names to boolean array for Clay
 * @param sectionNames The section names to convert (if not provided, uses stored config)
 * @returns Boolean array representing selected sections
 */
export const sectionsToBoolean = (
  sectionNames?: AvailableSection[],
): boolean[] => {
  const names = sectionNames ?? getAppConfig().selectedSectionNames;
  return availableSections.map((sec) => names.includes(sec));
};

/**
 * Convert additional feature names to boolean array for Clay
 * @param featureNames The feature names to convert (if not provided, uses stored config)
 * @returns Boolean array representing selected additional feeds
 */
export const feedsToBoolean = (
  featureNames?: AdditionalFeature[],
): boolean[] => {
  const names = featureNames ?? getAppConfig().selectedAdditionalFeeds;
  return additionalFeeds.map((feat) => names.includes(feat));
};

/**
 * Convert boolean array to category names
 * @param booleans The boolean array to convert
 * @returns The selected category names
 */
export const booleanToCategories = (
  booleans: boolean[],
): AvailableCategory[] => {
  return availableCategories.filter((_, i) => booleans[i]);
};

/**
 * Convert boolean array to section names
 * @param booleans The boolean array to convert
 * @returns The selected section names
 */
export const booleanToSections = (booleans: boolean[]): AvailableSection[] => {
  return availableSections.filter((_, i) => booleans[i]);
};

/**
 * Convert boolean array to additional feature names
 * @param booleans The boolean array to convert
 * @returns The selected additional feature names
 */
export const booleanToFeeds = (booleans: boolean[]): AdditionalFeature[] => {
  return additionalFeeds.filter((_, i) => booleans[i]);
};

/**
 * Generate Clay settings object from stored configuration
 * @param debugMode Whether debug mode is enabled
 * @returns Clay settings object
 */
export const generateClaySettings = (
  debugMode: boolean,
): Record<string, any> => {
  const config = getAppConfig();

  return {
    UserCategories: categoriesToBoolean(config.selectedCategoryNames),
    UserSections: sectionsToBoolean(config.selectedSectionNames),
    AdditionalFeeds: feedsToBoolean(config.selectedAdditionalFeeds),
    UserInterfaceLanguage: config.selectedInterfaceLanguage,
    UserContentLanguage: config.selectedContentLanguage,
    UserMaxStories: config.selectedMaxStoryCount,
    UserTextSize: config.selectedTextSize,
    UserTimelinePins: [config.newsRefreshPinSetting],
    ClearCacheOnSave: [false, false, false, false], // Always default to false on load
    DebugMode: debugMode,
    ShowTensionIndex: config.isTensionIndexEnabled,
  };
};
