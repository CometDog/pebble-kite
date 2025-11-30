interface FetchOptions {
  url: string;
  responseType?: "text" | "arraybuffer";
  timeout?: number;
}

function baseFetch({
  url,
  responseType,
  timeout = 30000,
}: FetchOptions): Promise<XMLHttpRequest> {
  return new Promise((resolve, reject) => {
    const req = new XMLHttpRequest();
    let settled = false;

    const settle = (fn: () => void) => {
      if (!settled) {
        settled = true;
        clearTimeout(timeoutId);
        fn();
      }
    };

    const timeoutId = setTimeout(() => {
      settle(() => {
        req.abort();
        reject(new Error("Request timeout (manual)"));
      });
    }, timeout);

    req.onreadystatechange = () => {
      if (req.readyState === 4) {
        if (req.status >= 200 && req.status < 300) {
          settle(() => resolve(req));
        } else if (req.status > 0) {
          settle(() =>
            reject(new Error(`Request failed with status ${req.status}`)),
          );
        } else {
          settle(() => reject(new Error("Network error (status 0)")));
        }
      }
    };

    req.onload = () => {
      if (req.status >= 200 && req.status < 300) {
        settle(() => resolve(req));
      } else {
        settle(() =>
          reject(new Error(`Request failed with status ${req.status}`)),
        );
      }
    };

    req.onerror = () => {
      settle(() => reject(new Error("Network error")));
    };

    req.ontimeout = () => {
      settle(() => reject(new Error("Request timeout")));
    };

    req.open("GET", url, true);
    if (responseType) {
      req.responseType = responseType;
    }
    req.send();
  });
}

/**
 * Fetch a string from a URL
 * @param url - The URL to fetch
 * @returns A promise that resolves with the string
 */
export async function fetchString(url: string): Promise<string> {
  const req = await baseFetch({ url });
  return req.responseText;
}

/**
 * Fetch a JSON object from a URL
 * @param url - The URL to fetch
 * @returns A promise that resolves with the JSON object
 */
export async function fetchJSON<T>(url: string): Promise<T> {
  const req = await baseFetch({ url });
  return JSON.parse(req.responseText);
}

/**
 * Fetch a binary file from a URL.
 * @param url - The URL to fetch
 * @returns A promise that resolves with the binary file as a Uint8Array
 */
export async function fetchBinary(url: string): Promise<Uint8Array> {
  const req = await baseFetch({ url, responseType: "arraybuffer" });
  return new Uint8Array(req.response);
}
