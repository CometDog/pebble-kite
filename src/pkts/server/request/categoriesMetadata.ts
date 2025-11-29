import { fetchJSON } from "../../../pktslib/fetch";
import { CategoryMetadata } from "../type/CategoryMetadata";

type CategoriesMetadataResponse = {
  categories: CategoryMetadata[];
};

export const categoriesMetadataRequest = () =>
  fetchJSON<CategoriesMetadataResponse>(
    "https://news.kagi.com/api/categories/metadata",
  );
