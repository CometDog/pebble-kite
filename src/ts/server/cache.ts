import { createLogger } from "../../lib";
import { fetchJSON, fetchText } from "./fetchUtils";
import * as packageJson from "../../../package-lock.json";

export interface CacheEntry<T> {
  appVersion: string;
  data: T;
  timestamp: number;
  url: string;
  batchId?: string;
}

const log = createLogger("KNJSCache");

const CACHE_PREFIX = "kagi_news_cache_";
const getCacheKey = (url: string): string => `${CACHE_PREFIX}${url}`;

const isCacheInvalid = <T>(entry: CacheEntry<T>): boolean => {
  if (entry.appVersion !== packageJson.version) {
    log.info(
      `Cache invalidated due to app version mismatch (cache: ${entry.appVersion}, current: ${packageJson.version})`,
    );
    return true;
  }
  return false;
};

const getCachedBatchInfo = (): { id: string; createdAtTimestamp: number } => {
  try {
    const cached = localStorage.getItem(getCacheKey("batchInfo"));
    if (cached) {
      return JSON.parse(cached);
    }
  } catch (error) {
    log.error("Failed to retrieve cached batch info:", error);
  }
  return { id: "", createdAtTimestamp: 0 };
};

export const updateCachedBatchInfo = ({
  id,
  createdAt,
}: {
  id: string;
  createdAt: string;
}) => {
  const cachedBatchInfo = getCachedBatchInfo();
  const createdAtTimestamp = new Date(createdAt).getTime();
  if (
    cachedBatchInfo.id !== id &&
    cachedBatchInfo.createdAtTimestamp !== createdAtTimestamp
  ) {
    clearCache();
    localStorage.setItem(
      getCacheKey("batchInfo"),
      JSON.stringify({ id, createdAtTimestamp }),
    );
  }
};

export const setCacheData = <T>(url: string, data: T) => {
  try {
    const cachedBatchInfo = getCachedBatchInfo();
    const entry: CacheEntry<T> = {
      appVersion: packageJson.version,
      data,
      timestamp: cachedBatchInfo.createdAtTimestamp,
      url,
      batchId: cachedBatchInfo.id,
    };
    localStorage.setItem(getCacheKey(url), JSON.stringify(entry));
  } catch (error) {
    log.error("Failed to cache data:", error);
  }
};

export const getCacheData = <T>(url: string): CacheEntry<T> | null => {
  try {
    const cached = localStorage.getItem(getCacheKey(url));
    if (!cached) {
      return null;
    }
    const entry: CacheEntry<T> = JSON.parse(cached);
    if (isCacheInvalid(entry)) {
      return null;
    }
    return entry;
  } catch (error) {
    log.error(`Failed to retrieve cache: `, error);
    return null;
  }
};

export const clearCache = () => {
  try {
    const keysToDelete: string[] = [];
    for (let i = 0; i < localStorage.length; i++) {
      const key = localStorage.key(i);
      if (key && key.startsWith(CACHE_PREFIX)) {
        keysToDelete.push(key);
      }
    }
    keysToDelete.forEach((key) => {
      localStorage.removeItem(key);
    });
  } catch (error) {
    log.error("Failed to clear cache:", error);
  }
};

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

export const cacheFirstFetchJSON = <T>(url: string): Promise<T> =>
  cacheFirstFetch({ url, fetcher: () => fetchJSON<T>(url) });

export const cacheFirstFetchText = (url: string): Promise<string> =>
  cacheFirstFetch({ url, fetcher: () => fetchText(url) });
