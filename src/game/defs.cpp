#include "defs.hpp"
#include "sg/log.h"
#include <stdarg.h>
namespace Game {

static sg_logger *logger;

void Defs::init() {
    logger = sg_logger_get("game");
}

void Defs::debug(const char *msg, ...) {
    va_list ap;
    va_start(ap, msg);
    sg_logv(logger, SG_LOG_DEBUG, msg, ap);
    va_end(ap);
}

void Defs::info(const char *msg, ...) {
    va_list ap;
    va_start(ap, msg);
    sg_logv(logger, SG_LOG_INFO, msg, ap);
    va_end(ap);
}

void Defs::warn(const char *msg, ...) {
    va_list ap;
    va_start(ap, msg);
    sg_logv(logger, SG_LOG_WARN, msg, ap);
    va_end(ap);
}

void Defs::error(const char *msg, ...) {
    va_list ap;
    va_start(ap, msg);
    sg_logv(logger, SG_LOG_ERROR, msg, ap);
    va_end(ap);
}

}
