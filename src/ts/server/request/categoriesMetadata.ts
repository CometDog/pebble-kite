import { getServerLang } from "../localeManager";
import { CategoryMetadata } from "../type/CategoryMetadata";

type CategoriesMetadataResponse = {
  categories: CategoryMetadata[];
};

export const categoriesMetadataRequest = () =>
  fetch(
    `https://news.kagi.com/api/categories/metadata${getServerLang() && `?lang=${getServerLang()}`}`,
  ).then(async (res) => {
    return res.ok
      ? ((await res.json()) as CategoriesMetadataResponse)
      : Promise.reject(res);
  });
