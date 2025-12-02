#include "debug_logger.h"
#include "../communication.h"

static bool s_debug_enabled = false;

void debug_logger_init(void)
{
    s_debug_enabled = false;
#ifndef RELEASE
    APP_LOG(APP_LOG_LEVEL_INFO, "[DebugLogger] Initialized, waiting for phone to send debug mode setting");
#endif
}

void debug_logger_deinit(void)
{
    // Nothing to clean up
}

void debug_logger_set_enabled(bool enabled)
{
    s_debug_enabled = enabled;
#ifndef RELEASE
    APP_LOG(APP_LOG_LEVEL_INFO, "[DebugLogger] Debug mode %s", enabled ? "enabled" : "disabled");
#endif
}

bool debug_logger_is_enabled(void)
{
    return s_debug_enabled;
}

static void send_log_to_phone(DebugLogLevel level, const char *tag, const char *message)
{
    if (!send_debug_log((int32_t)level, tag, message))
    {
#ifndef RELEASE
        APP_LOG(APP_LOG_LEVEL_ERROR, "[DebugLogger] Failed to send debug log");
#endif
    }
}

void debug_log(const char *tag, const char *message)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "[%s] %s", tag, message);
}

void info_log(const char *tag, const char *message)
{
    APP_LOG(APP_LOG_LEVEL_INFO, "[%s] %s", tag, message);
}

void warn_log(const char *tag, const char *message)
{
#ifndef RELEASE
    APP_LOG(APP_LOG_LEVEL_WARNING, "[%s] %s", tag, message);
#endif

    if (s_debug_enabled)
    {
        send_log_to_phone(LOG_LEVEL_WARN, tag, message);
    }
}

void error_log(const char *tag, const char *message)
{
#ifndef RELEASE
    APP_LOG(APP_LOG_LEVEL_ERROR, "[%s] %s", tag, message);
#endif

    if (s_debug_enabled)
    {
        send_log_to_phone(LOG_LEVEL_ERROR, tag, message);
    }
}

void debug_logger_notify(const char *title, const char *message)
{
    // Always send, regardless of debug mode
#ifndef RELEASE
    APP_LOG(APP_LOG_LEVEL_INFO, "[DebugLogger] Notify: %s - %s", title, message);
#endif

    if (!send_debug_notify(title, message))
    {
#ifndef RELEASE
        APP_LOG(APP_LOG_LEVEL_ERROR, "[DebugLogger] Failed to send debug notify");
#endif
    }
}
