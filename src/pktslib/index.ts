import { PromisePolyfill, PromiseShim } from "./promise-polyfill";
import { sendAppMessage } from "./appMessage";
import { fetchString, fetchJSON, fetchBinary } from "./fetch";

export {
  PromisePolyfill,
  PromiseShim,
  sendAppMessage,
  fetchString,
  fetchJSON,
  fetchBinary,
};

export interface InstallOptions {
  force?: boolean; // replace existing global Promise when true
}

/**
 * Install the polyfill onto globalThis.Promise.
 * By default it will not overwrite an existing Promise implementation.
 */
export function installGlobal(options: InstallOptions = {}): void {
  const globalObj: any =
    typeof globalThis !== "undefined"
      ? globalThis
      : typeof window !== "undefined"
        ? window
        : ({} as any);
  const shouldForce = !!options.force;
  if (!globalObj.Promise || shouldForce) {
    globalObj.Promise = PromiseShim;
  }
}

export default {
  PromisePolyfill,
  installGlobal,
  sendAppMessage,
  fetchString,
  fetchJSON,
  fetchBinary,
};
