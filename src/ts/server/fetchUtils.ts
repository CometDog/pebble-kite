import { createLogger } from "../../lib";

const log = createLogger("KNJSFetchUtils");

// Decode UTF-8 from ArrayBuffer
// PebbleKit.ts's fetch implementation doesn't properly handle UTF-8 in res.json(),
// so we need to manually decode UTF-8 bytes
// I just found this online so hopefully it's fine
export function decodeUTF8(buffer: ArrayBuffer): string {
  const bytes = new Uint8Array(buffer);
  let result = "";
  let i = 0;

  while (i < bytes.length) {
    const byte1 = bytes[i++];

    if (byte1 < 0x80) {
      // 1-byte character (0xxxxxxx)
      result += String.fromCharCode(byte1);
    } else if (byte1 < 0xe0) {
      // 2-byte character (110xxxxx 10xxxxxx)
      const byte2 = bytes[i++];
      result += String.fromCharCode(((byte1 & 0x1f) << 6) | (byte2 & 0x3f));
    } else if (byte1 < 0xf0) {
      // 3-byte character (1110xxxx 10xxxxxx 10xxxxxx)
      const byte2 = bytes[i++];
      const byte3 = bytes[i++];
      result += String.fromCharCode(
        ((byte1 & 0x0f) << 12) | ((byte2 & 0x3f) << 6) | (byte3 & 0x3f),
      );
    } else {
      // 4-byte character (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
      const byte2 = bytes[i++];
      const byte3 = bytes[i++];
      const byte4 = bytes[i++];
      let codePoint =
        ((byte1 & 0x07) << 18) |
        ((byte2 & 0x3f) << 12) |
        ((byte3 & 0x3f) << 6) |
        (byte4 & 0x3f);
      // Convert to UTF-16 surrogate pair
      codePoint -= 0x10000;
      result += String.fromCharCode(
        0xd800 + (codePoint >> 10),
        0xdc00 + (codePoint & 0x3ff),
      );
    }
  }

  return result;
}

// Helper to fetch and parse JSON with proper UTF-8 decoding
export async function fetchText(url: string): Promise<string> {
  const res = await fetch(url);
  if (!res.ok) {
    return Promise.reject(res);
  }
  const buffer = await res.arrayBuffer();
  log.debug(`Fetched ${url}, ${buffer.byteLength} bytes`);
  return decodeUTF8(buffer);
}

// Helper to fetch and parse JSON with proper UTF-8 decoding
export async function fetchJSON<T>(url: string): Promise<T> {
  const text = await fetchText(url);
  return JSON.parse(text) as T;
}
