import { createLogger } from "../../../lib";
import * as packageJson from "../../../../package-lock.json";

export interface CacheEntry<T> {
  appVersion: string;
  data: T;
  timestamp: number;
  url: string;
  batchId?: string;
}

const log = createLogger("KNJSCacheManager");

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
    const key = getCacheKey("batchInfo");
    const cached = localStorage.getItem(key);
    if (cached) {
      return JSON.parse(cached);
    }
  } catch (error) {
    log.error("Failed to retrieve cached batch info:", error);
  }
  return { id: "", createdAtTimestamp: 0 };
};
const CACHE_PREFIX = "kagi_news_cache_";
const getCacheKey = (url: string): string => {
  return `${CACHE_PREFIX}${url}`;
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
    const key = getCacheKey("batchInfo");
    localStorage.setItem(key, JSON.stringify({ id, createdAtTimestamp }));
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
    const key = getCacheKey(url);
    localStorage.setItem(key, JSON.stringify(entry));
  } catch (error) {
    log.error("Failed to cache data:", error);
  }
};

export const getCacheData = <T>(url: string): CacheEntry<T> | null => {
  try {
    const key = getCacheKey(url);
    const cached = localStorage.getItem(key);

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
