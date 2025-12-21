import {
  AvailableCategory,
  AvailableSection,
  availableCategories,
  availableSections,
  defaultCategories,
  defaultMaxStories,
  defaultSections,
} from "./types";
import { createLogger } from "../lib/logger";

const log = createLogger("KNJSConfig");

/**
 * Application configuration interface
 * @property newsRefreshPinSetting Whether the news refresh Timeline pin is enabled
 * @property selectedTextSize The selected text size
 * @property selectedInterfaceLanguage The selected interface language
 * @property selectedContentLanguage The selected content language
 * @property selectedMaxStoryCount The selected maximum story count
 * @property selectedSectionNames The enabled section names
 * @property selectedCategoryNames The enabled category names
 */
export interface AppConfig {
  newsRefreshPinSetting: boolean;
  selectedTextSize: number;
  selectedInterfaceLanguage: string;
  selectedContentLanguage: string;
  selectedMaxStoryCount: number;
  selectedSectionNames: AvailableSection[];
  selectedCategoryNames: AvailableCategory[];
}

const CONFIG_KEYS: Record<keyof AppConfig, string> = {
  newsRefreshPinSetting: "newsRefreshPinSetting",
  selectedTextSize: "selectedTextSize",
  selectedInterfaceLanguage: "selectedInterfaceLanguage",
  selectedContentLanguage: "selectedContentLanguage",
  selectedMaxStoryCount: "selectedMaxStoryCount",
  selectedSectionNames: "selectedSectionNames",
  selectedCategoryNames: "selectedCategoryNames",
};

const CONFIG_DEFAULTS: AppConfig = {
  newsRefreshPinSetting: false,
  selectedTextSize: 0,
  selectedInterfaceLanguage: "",
  selectedContentLanguage: "",
  selectedMaxStoryCount: defaultMaxStories,
  selectedSectionNames: defaultSections,
  selectedCategoryNames: defaultCategories,
};

/**
 * Get a configuration value from localStorage with proper typing and fallback to defaults
 * @param key The configuration key to retrieve
 * @returns The configuration value
 */
export const getConfig = <K extends keyof AppConfig>(key: K): AppConfig[K] => {
  try {
    const stored = localStorage.getItem(CONFIG_KEYS[key]);
    if (!stored) {
      log.debug(`No stored value for ${String(key)}, using default`);
      return CONFIG_DEFAULTS[key];
    }

    const parsed = JSON.parse(stored);
    log.debug(`Retrieved ${String(key)}: ${JSON.stringify(parsed)}`);
    return parsed;
  } catch (err) {
    log.warn(
      `Failed to retrieve ${String(key)}, using default`,
      err instanceof Error ? err.message : String(err),
    );
    return CONFIG_DEFAULTS[key];
  }
};

/**
 * Set a configuration value to localStorage
 * @param key The configuration key to set
 * @param value The configuration value to set
 */
export const setConfig = <K extends keyof AppConfig>(
  key: K,
  value: AppConfig[K],
): void => {
  try {
    localStorage.setItem(CONFIG_KEYS[key], JSON.stringify(value));
    log.info(`Saved ${String(key)}: ${JSON.stringify(value)}`);
  } catch (err) {
    log.error(
      `Failed to save ${String(key)}`,
      err instanceof Error ? err.message : String(err),
    );
  }
};

/**
 * Get all configuration values at once
 * @returns The complete application configuration
 */
export const getAllConfig = (): AppConfig => ({
  newsRefreshPinSetting: getConfig("newsRefreshPinSetting"),
  selectedTextSize: getConfig("selectedTextSize"),
  selectedInterfaceLanguage: getConfig("selectedInterfaceLanguage"),
  selectedContentLanguage: getConfig("selectedContentLanguage"),
  selectedMaxStoryCount: getConfig("selectedMaxStoryCount"),
  selectedSectionNames: getConfig("selectedSectionNames"),
  selectedCategoryNames: getConfig("selectedCategoryNames"),
});

/**
 * Convert category names to boolean array for Clay
 * @param categoryNames The category names to convert (if not provided, uses stored config)
 * @returns Boolean array representing selected categories
 */
export const categoriesToBoolean = (
  categoryNames?: AvailableCategory[],
): boolean[] => {
  const names = categoryNames ?? getConfig("selectedCategoryNames");
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
  const names = sectionNames ?? getConfig("selectedSectionNames");
  return availableSections.map((sec) => names.includes(sec));
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
 * Generate Clay settings object from stored configuration
 * @param debugMode Whether debug mode is enabled
 * @returns Clay settings object
 */
export const generateClaySettings = (
  debugMode: boolean,
): Record<string, any> => {
  const config = getAllConfig();

  return {
    UserCategories: categoriesToBoolean(config.selectedCategoryNames),
    UserSections: sectionsToBoolean(config.selectedSectionNames),
    UserInterfaceLanguage: config.selectedInterfaceLanguage,
    UserContentLanguage: config.selectedContentLanguage,
    UserMaxStories: config.selectedMaxStoryCount,
    UserTextSize: config.selectedTextSize,
    UserTimelinePins: [config.newsRefreshPinSetting],
    ClearCacheOnSave: [false, false], // Always default to false on load
    DebugMode: debugMode,
  };
};
