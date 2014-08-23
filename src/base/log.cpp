#include "log.hpp"
#include "sg/log.h"
#include "sg/entry.h"
#include <stdarg.h>
namespace Base {

static sg_logger *logger;

void Log::init() {
    logger = sg_logger_get("game");
}

void Log::debug(const char *msg, ...) {
    va_list ap;
    va_start(ap, msg);
    sg_logv(logger, SG_LOG_DEBUG, msg, ap);
    va_end(ap);
}

void Log::info(const char *msg, ...) {
    va_list ap;
    va_start(ap, msg);
    sg_logv(logger, SG_LOG_INFO, msg, ap);
    va_end(ap);
}

void Log::warn(const char *msg, ...) {
    va_list ap;
    va_start(ap, msg);
    sg_logv(logger, SG_LOG_WARN, msg, ap);
    va_end(ap);
}

void Log::error(const char *msg, ...) {
    va_list ap;
    va_start(ap, msg);
    sg_logv(logger, SG_LOG_ERROR, msg, ap);
    va_end(ap);
}

void Log::abort(const char *msg, ...) {
    va_list ap;
    va_start(ap, msg);
    sg_sys_abortv(msg, ap);
    va_end(ap);
}

}
