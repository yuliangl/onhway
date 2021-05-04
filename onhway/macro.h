#ifndef _ONHWAY_MACRO_H_
#define _ONHWAY_MACRO_H_

#include <assert.h>
#include "util.h"
#include "log.h"


#define onhway_assert(x) \
    if(!(x)) { \
        ONHWAY_LOG_ERROR(ONHWAY_LOG_ROOT) << "ASSERTION: " #x \
                    << "\nbacktrace:\n" \
                    << onhway::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#define onhway_assert1(x, w) \
    if(!(x)) { \
        ONHWAY_LOG_ERROR(ONHWAY_LOG_ROOT) << "ASSERTION: " #x \
                    << "\nonhway_note: " << (w) \
                    << "\nbacktrace:\n" \
                    << onhway::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }



#endif
