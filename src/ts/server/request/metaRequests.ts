import { fetchJSON } from "../fetchUtils";
import { cacheFirstFetchJSON } from "../cache";
import { getServerLang } from "../localeManager";
import { CategoryMetadata } from "../type/CategoryMetadata";

type HealthResponse = {
  health: boolean;
  hasBatch: boolean;
  hasCategories: boolean;
  hasTranslations: boolean;
  isRecent: boolean;
  batchId: string;
  createdAt: string;
};

export const healthRequest = () =>
  fetchJSON<HealthResponse>("https://news.kagi.com/api/health");

// Normalize encoding and remove zero-width characters
const normalizeCategoryName = (name: string): string =>
  name.normalize("NFC").replace(/[\u200B-\u200D\uFEFF]/g, "");

type CategoryMetadataResponse = {
  categories: {
    categoryId: string;
    displayName: string;
    displayNames?: Record<string, string>;
    sourceLanguage: string;
  }[];
};

export type PossibleCategoriesResult = {
  names: string[];
  displayNamesMap: Map<string, Record<string, string>>;
};

export const possibleCategoriesRequest =
  async (): Promise<PossibleCategoriesResult> =>
    cacheFirstFetchJSON<CategoryMetadataResponse>(
      `https://news.kagi.com/api/categories/metadata`,
    ).then((response) => {
      const displayNamesMap = new Map<string, Record<string, string>>();
      const names: string[] = [];

      for (const category of response.categories) {
        // Normalize encoding and remove zero-width characters to prevent issues with Clay checkboxes
        const name = normalizeCategoryName(category.displayName);

        names.push(name);

        if (category.displayNames) {
          // Normalize all display name values
          const normalized: Record<string, string> = {};
          for (const [lang, value] of Object.entries(category.displayNames)) {
            normalized[lang] = normalizeCategoryName(value);
          }
          displayNamesMap.set(name, normalized);
        }
      }

      names.sort();
      return { names, displayNamesMap };
    });

type CategoriesMetadataResponse = {
  categories: CategoryMetadata[];
};

export const categoriesMetadataRequest = () =>
  cacheFirstFetchJSON<CategoriesMetadataResponse>(
    `https://news.kagi.com/api/categories/metadata${getServerLang() && `?lang=${getServerLang()}`}`,
  );
