/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_ANALYTICS_ANALYTICS_HPP
#define LD_ANALYTICS_ANALYTICS_HPP
namespace Analytics {

/// Level status.
static const int STATUS_COUNT = 5;
enum class Status {
    IN_PROGRESS, SKIP_NEXT, SKIP_PREV, RESTART, SUCCESS
};

/// Level analytics data.
struct Level {
    int index;
    int level;
    int time_start;
    int time_wake; // or -1 for null
    int time_end;
    int action_count;
    bool talked_to_shadow;
    Status status;

    void submit() const;
};

struct Analytics {
    /// Initialize the analytics system.
    static void init();

    /// Finish all pending requests.
    static void finish();
};

}
#endif
