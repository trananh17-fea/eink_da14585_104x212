#include "etime.h"

// Số giây trong một năm: [0] Năm thường, [1] Năm nhuận
uint32_t SEC_PER_YR[2] = {31536000, 31622400}; 

// Số giây trong từng tháng (Năm thường và Năm nhuận)
uint32_t SEC_PER_MT[2][12] = {
    {2678400, 2419200, 2678400, 2592000, 2678400, 2592000,
     2678400, 2678400, 2592000, 2678400, 2592000, 2678400},
    {2678400, 2505600, 2678400, 2592000, 2678400, 2592000,
     2678400, 2678400, 2592000, 2678400, 2592000, 2678400},
};

/**
 * Kiểm tra năm nhuận
 */
int is_leap(int yr)
{
    if (0 == (yr % 100))
        return (yr % 400 == 0) ? 1 : 0;
    else
        return (yr % 4 == 0) ? 1 : 0;
}

/**
 * Tính thứ trong tuần (Sakamoto's algorithm)
 */
unsigned char day_of_week_get(unsigned char month, unsigned char day, unsigned short year)
{
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    year -= (uint8_t)(month < 3);
    return (year + year / 4 - year / 100 + year / 400 + t[month - 1] + day) % 7;
}

/**
 * Chuyển đổi Unix Timestamp sang giờ Việt Nam (GMT+7)
 */
void transformTime(uint32_t unix_time, struct devtm *result)
{
    int leapyr = 0;
    
    uint32_t ltime = unix_time; 

    memset(result, 0, sizeof(struct devtm));
    result->tm_year = EPOCH_YR; // Mặc định là 1970

    // Tính năm
    while (1)
    {
        uint32_t sec_in_this_yr = SEC_PER_YR[is_leap(result->tm_year)];
        if (ltime < sec_in_this_yr) break;
        
        ltime -= sec_in_this_yr;
        ++(result->tm_year);
    }

    // Tính tháng
    leapyr = is_leap(result->tm_year);
    while (1)
    {
        if (ltime < SEC_PER_MT[leapyr][result->tm_mon]) break;
        
        ltime -= SEC_PER_MT[leapyr][result->tm_mon];
        ++(result->tm_mon);
    }

    // Tính ngày (1 ngày = 86400 giây)
    result->tm_mday = ltime / 86400; 
    ++(result->tm_mday);
    ltime = ltime % 86400;

    // Tính giờ
    result->tm_hour = ltime / 3600;
    ltime = ltime % 3600;

    // Tính phút và giây
    result->tm_min = ltime / 60;
    result->tm_sec = ltime % 60;

    // Tính thứ (0: Chủ Nhật, 1: Thứ 2...)
    result->tm_wday = day_of_week_get(result->tm_mon + 1, result->tm_mday, result->tm_year);

    // Trừ đi mốc YEAR0 (thường là 1900) nếu cấu trúc yêu cầu
    result->tm_year -= YEAR0;
}