#ifndef _NK_DATETIME_H
#define _NK_DATETIME_H

#include <stdint.h>

namespace nk
{

    struct DateTime
    {
        int32_t year;
        int16_t month;
        int16_t day;

        int16_t hour;
        int16_t minute;
        int16_t second;
    };

}; // namespace nk

#endif