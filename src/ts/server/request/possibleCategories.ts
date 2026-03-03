import { cacheFirstFetchJSON } from "../cache/cacheFirstFetch";
import { fetchJSON } from "../fetchUtils";

type PossibleCategoriesResponse = {
  categories: {
    name: string;
  }[];
};

export const possibleCategoriesRequest = async (): Promise<string[]> =>
  (
    fetchJSON<PossibleCategoriesResponse>(
      `https://news.kagi.com/kite.json`,
    )
  ).then((response) =>
    response.categories.map(
      // Encode then decode because some category names have incorrectly decoded names
      (category: { name: string }) =>
        new TextDecoder().decode(new TextEncoder().encode(category.name)),
    )
      .sort()
  );
