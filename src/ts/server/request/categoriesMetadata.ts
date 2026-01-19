import { cacheFirstFetchJSON } from "../cache/cacheFirstFetch";
import { getServerLang } from "../localeManager";
import { CategoryMetadata } from "../type/CategoryMetadata";

type CategoriesMetadataResponse = {
  categories: CategoryMetadata[];
};

export const categoriesMetadataRequest = () =>
  cacheFirstFetchJSON<CategoriesMetadataResponse>(
    `https://news.kagi.com/api/categories/metadata${getServerLang() && `?lang=${getServerLang()}`}`,
  );
