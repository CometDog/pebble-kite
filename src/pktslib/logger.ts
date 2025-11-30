type LogLevel = "debug" | "info" | "warn" | "error";

let debugMode = false;

/**
 * Initialize debug mode from stored settings
 */
export const initDebugMode = (): void => {
  const stored = localStorage.getItem("debugMode");
  debugMode = stored === "true";
};

/**
 * Set debug mode on/off
 * @param enabled - true to enable debug mode, false to disable
 */
export const setDebugMode = (enabled: boolean): void => {
  debugMode = enabled;
  localStorage.setItem("debugMode", String(enabled));
};

/**
 * Check if debug mode is enabled
 * @returns true if debug mode is enabled, false otherwise
 */
export const isDebugMode = (): boolean => {
  return debugMode;
};

const log = (
  level: LogLevel,
  tag: string,
  message: string,
  data?: unknown,
): void => {
  const prefix = `[${tag}]`;
  const fullMessage =
    data !== undefined ? `${message} ${JSON.stringify(data)}` : message;

  switch (level) {
    case "debug":
      console.log(`${prefix} ${fullMessage}`);
      break;
    case "info":
      console.log(`${prefix} ${fullMessage}`);
      break;
    case "warn":
      console.warn(`${prefix} ${fullMessage}`);
      break;
    case "error":
      console.error(`${prefix} ${fullMessage}`);
      break;
  }

  if (debugMode && (level === "warn" || level === "error")) {
    const title = level === "error" ? `X ${tag}` : `! ${tag}`;
    // Truncate message to fit on watch screen
    const truncated =
      fullMessage.length > 100
        ? fullMessage.substring(0, 97) + "..."
        : fullMessage;
    try {
      Pebble.showSimpleNotificationOnPebble(title, truncated);
    } catch {
      // Ignore if notification fails
    }
  }
};

/**
 * Create a logger instance with a specific tag
 * @param tag - The tag to use for log messages
 * @returns An object with logging methods
 */
export const createLogger = (tag: string) => {
  return {
    debug: (message: string, data?: unknown) =>
      log("debug", tag, message, data),
    info: (message: string, data?: unknown) => log("info", tag, message, data),
    warn: (message: string, data?: unknown) => log("warn", tag, message, data),
    error: (message: string, data?: unknown) =>
      log("error", tag, message, data),

    /**
     * Force show a notification regardless of debug mode
     * Useful for critical errors
     */
    notify: (title: string, message: string) => {
      try {
        Pebble.showSimpleNotificationOnPebble(title, message.substring(0, 100));
      } catch {
        // Ignore if notification fails
      }
    },
  };
};
