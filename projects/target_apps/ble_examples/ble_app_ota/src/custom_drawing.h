#ifndef _CUSTOM_DRAWING_H_
#define _CUSTOM_DRAWING_H_

#include <time.h> // 使用标准的 time.h 结构体

/**
 * @brief 在屏幕上绘制一个格式为 "HH:MM" 的时间字符串，使用七段码风格。
 *        此函数会自动处理数字和冒号的绘制，并管理布局。
 *
 * @param x           时间字符串左上角的 x 坐标。
 * @param y           时间字符串左上角的 y 坐标。
 * @param time_ptr    一个指向标准 tm 结构体的指针，包含要显示的时间。
 * @param digit_width 每个数字的宽度。
 * @param digit_height 每个数字的高度 (也是冒号的高度)。
 * @param color       绘制的颜色 (例如 BLACK)。
 */
void Draw_Time_String(int x, int y, const struct tm* time_ptr, int digit_width, int digit_height, int color);

#endif // _CUSTOM_DRAWING_H_

