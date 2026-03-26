#ifndef _LUNAR_H_
#define _LUNAR_H_

#include <stdint.h>
#include "DEV_Config.h"

struct Lunar_Date {
  uint16_t Year;
  uint8_t Month;
  uint8_t Date;
  uint8_t IsLeap;
};

// Khai báo các biến chuỗi giao diện (đồng bộ với file .c)
extern const char Lunar_MonthString[13][5];
extern const char Lunar_MonthLeapString[2][10];
extern const char Lunar_DateString[31][3];
extern const char Lunar_ZodiacString[12][6];
extern const char Lunar_StemString[10][6];
extern const char WEEK_VN[8][10];
extern const char JieQiStr[24][16];

// Khai báo các hàm xử lý
void LUNAR_SolarToLunar(struct Lunar_Date *lunar, uint16_t solar_year,
                        uint8_t solar_month, uint8_t solar_date);
uint8_t LUNAR_GetZodiac(const struct Lunar_Date *lunar);
uint8_t LUNAR_GetStem(const struct Lunar_Date *lunar);
uint8_t LUNAR_GetBranch(const struct Lunar_Date *lunar);

// Khai báo các hàm xử lý tiết khí
uint8_t GetJieQiStr(uint16_t myear, uint8_t mmonth, uint8_t mday, uint8_t *day);
uint8_t GetJieQi(uint16_t myear, uint8_t mmonth, uint8_t mday, uint8_t *JQdate);

#endif /* _LUNAR_H_ */