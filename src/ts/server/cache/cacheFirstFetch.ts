import { createLogger } from "../../../lib";
import { fetchJSON, fetchText } from "../fetchUtils";
import { getCacheData, setCacheData } from "./cacheManager";

const log = createLogger("KNJSCachedFetch");

const cacheFirstFetch = async <T>({
  url,
  fetcher,
}: {
  url: string;
  fetcher: () => Promise<T>;
}): Promise<T> => {
  const cachedEntry = getCacheData<T>(url);

  if (cachedEntry !== null) {
    log.debug("Cache hit, returning cached data");
    return cachedEntry.data;
  }

  log.debug("Cache miss, fetching fresh data");
  try {
    const newData = await fetcher();
    setCacheData(url, newData);

    return newData;
  } catch (error) {
    log.error("Failed to fetch:", error);
    throw error;
  }
};

export const cacheFirstFetchJSON = <T>(url: string): Promise<T> => {
  return cacheFirstFetch({
    url,
    fetcher: () => fetchJSON<T>(url),
  });
};

export const cacheFirstFetchText = (url: string): Promise<string> => {
  return cacheFirstFetch({
    url,
    fetcher: () => fetchText(url),
  });
};
