import { getServerLang } from "../localeManager";

export const storiesRequest = ({ categoryId }: { categoryId: string }) =>
  fetch(
    `https://news.kagi.com/api/batches/latest/categories/${categoryId}/stories${getServerLang() && `?lang=${getServerLang()}`}`,
  ).then(async (res) => {
    return res.ok ? await res.text() : Promise.reject(res);
  });
