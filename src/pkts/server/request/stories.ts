import { fetchString } from "../../../pktslib/fetch";
import { getServerLang } from "../localeManager";

export const storiesRequest = ({ categoryId }: { categoryId: string }) =>
  fetchString(
    `https://news.kagi.com/api/batches/latest/categories/${categoryId}/stories${getServerLang() && `?lang=${getServerLang()}`}`,
  );
