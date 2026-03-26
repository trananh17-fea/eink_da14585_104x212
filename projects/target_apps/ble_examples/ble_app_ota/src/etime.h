#ifndef _ETIME_H_
#define _ETIME_H_

#include "stdint.h"
#include "string.h"

#define YEAR0 (2000)       /* The first year */
#define EPOCH_YR (2026)    /* EPOCH = Jan 1 2026 00:00:00 */
#define SEC_PER_DY (86400) // seconds per day
#define SEC_PER_HR (3600)  // seconds per hour

typedef struct devtm {
  uint16_t tm_year;
  uint8_t tm_mon;
  uint8_t tm_mday;
  uint8_t tm_hour;
  uint8_t tm_min;
  uint8_t tm_sec;
  uint8_t tm_wday;
} tm_t;

int is_leap(int yr);
unsigned char day_of_week_get(unsigned char month, unsigned char day,
                              unsigned short year);
void transformTime(uint32_t unix_time, struct devtm *result);

#endif