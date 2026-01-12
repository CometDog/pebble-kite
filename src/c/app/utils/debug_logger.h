#pragma once

#include <pebble.h>

/**
 * Debug logger that sends messages to the phone to display as notifications
 *
 * Flow: C code -> AppMessage -> JS -> Pebble.showSimpleNotificationOnPebble()
 */

typedef enum
{
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_ERROR = 3
} DebugLogLevel;

/**
 * Initialize the debug logger
 */
void debug_logger_init(void);

/**
 * Deinitialize the debug logger
 */
void debug_logger_deinit(void);

/**
 * Enable or disable debug mode
 * When enabled, warn and error messages are sent to phone for display
 * @param enabled: true to enable debug mode, false to disable
 */
void debug_logger_set_enabled(bool enabled);

/**
 * Check if debug mode is enabled
 */
bool debug_logger_is_enabled(void);

/**
 * USE DEBUG_LOG INSTEAD
 * Log a debug message
 * @param tag: Tag string (usually file/module name)
 * @param message: Message string
 */
void debug_log(const char *tag, const char *message);

/**
 * USE INFO_LOG INSTEAD
 * Log an info message
 * @param tag: Tag string (usually file/module name)
 * @param message: Message string
 */
void info_log(const char *tag, const char *message);

/**
 * USE WARN_LOG INSTEAD
 * Log a warning message
 * @param tag: Tag string (usually file/module name)
 * @param message: Message string
 */
void warn_log(const char *tag, const char *message);

/**
 * USE ERROR_LOG INSTEAD
 * Log an error message
 * @param tag: Tag string (usually file/module name)
 * @param message: Message string
 */
void error_log(const char *tag, const char *message);

/*
 * Formatted logging macros (uses snprintf internally)
 * Example: DEBUG_LOG("MyTag", "Value is %d", 42);
 * @param tag: Tag string (usually file/module name)
 * @param fmt: Format string (printf-style)
 * @param ...: Format arguments
 */

#ifdef PBL_BW
static char _shared_log_buf[64];
#else
static char _shared_log_buf[96];
#endif

#define DEBUG_LOG(tag, fmt, ...)                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        snprintf(_shared_log_buf, sizeof(_shared_log_buf), fmt, ##__VA_ARGS__);                                        \
        debug_log(tag, _shared_log_buf);                                                                               \
    } while (0)

#define INFO_LOG(tag, fmt, ...)                                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        snprintf(_shared_log_buf, sizeof(_shared_log_buf), fmt, ##__VA_ARGS__);                                        \
        info_log(tag, _shared_log_buf);                                                                                \
    } while (0)

#define WARN_LOG(tag, fmt, ...)                                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        snprintf(_shared_log_buf, sizeof(_shared_log_buf), fmt, ##__VA_ARGS__);                                        \
        warn_log(tag, _shared_log_buf);                                                                                \
    } while (0)

#define ERROR_LOG(tag, fmt, ...)                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        snprintf(_shared_log_buf, sizeof(_shared_log_buf), fmt, ##__VA_ARGS__);                                        \
        error_log(tag, _shared_log_buf);                                                                               \
    } while (0)

/* In release builds we don't want debug/info logging code included
 */
#ifdef RELEASE
#undef DEBUG_LOG
#undef INFO_LOG
#define DEBUG_LOG(tag, fmt, ...)                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define INFO_LOG(tag, fmt, ...)                                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)

#ifdef PBL_PLATFORM_APLITE
#undef WARN_LOG
#undef ERROR_LOG
#define WARN_LOG(tag, fmt, ...)                                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define ERROR_LOG(tag, fmt, ...)                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#endif
#endif
