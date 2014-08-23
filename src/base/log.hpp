#include "sg/attribute.h"
namespace Base {

struct Log {
    /// Log an debug message.
    SG_ATTR_FORMAT(printf, 1, 2)
    static void debug(const char *msg, ...);

    /// Log an info message.
    SG_ATTR_FORMAT(printf, 1, 2)
    static void info(const char *msg, ...);

    /// Log a warning message.
    SG_ATTR_FORMAT(printf, 1, 2)
    static void warn(const char *msg, ...);

    /// Log an error message.
    SG_ATTR_FORMAT(printf, 1, 2)
    static void error(const char *msg, ...);

    /// Abort with the given message.
    SG_ATTR_FORMAT(printf, 1, 2)
    static void abort(const char *msg, ...);

    /// Initialize the logger.
    static void init();
};

}
