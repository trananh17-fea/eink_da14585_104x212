#include "custom_drawing.h"
#include "gui_paint.h" // 依赖项目中的图形库
#include <time.h>      // 确保包含 time.h

// ======================================================================
// 1. 底层绘制函数 (内部使用，保持不变)

/**
 * @brief 适配器函数，使用 Paint_DrawRectangle 来实现绘制实心矩形的功能。
 */
static void draw_rectangle_filled(int x, int y, int width, int height, int color) {
    int x_end = x + width - 1;
    int y_end = y + height - 1;
    Paint_DrawRectangle(x, y, x_end, y_end, color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
}

/**
 * @brief 绘制一个任意大小和线宽的七段码风格数字。
 */
static void draw_seven_segment_digit(int x, int y, int digit, int width, int height, int thickness, int color) {
    const uint8_t segment_map[10][7] = {
        {1,1,1,1,1,1,0}, {0,1,1,0,0,0,0}, {1,1,0,1,1,0,1}, {1,1,1,1,0,0,1}, {0,1,1,0,0,1,1},
        {1,0,1,1,0,1,1}, {1,0,1,1,1,1,1}, {1,1,1,0,0,0,0}, {1,1,1,1,1,1,1}, {1,1,1,1,0,1,1}
    };
    if (digit < 0 || digit > 9) return;
    if (width <= 2 * thickness || height <= 2 * thickness) return;
    const int h_seg_len = width - 2 * thickness;
    const int v_seg_len = (height - 3 * thickness) / 2;
    if (h_seg_len <= 0 || v_seg_len <= 0) return;
    if (segment_map[digit][0]) draw_rectangle_filled(x + thickness, y, h_seg_len, thickness, color);
    if (segment_map[digit][1]) draw_rectangle_filled(x + width - thickness, y + thickness, thickness, v_seg_len, color);
    if (segment_map[digit][2]) draw_rectangle_filled(x + width - thickness, y + 2 * thickness + v_seg_len, thickness, v_seg_len, color);
    if (segment_map[digit][3]) draw_rectangle_filled(x + thickness, y + height - thickness, h_seg_len, thickness, color);
    if (segment_map[digit][4]) draw_rectangle_filled(x, y + 2 * thickness + v_seg_len, thickness, v_seg_len, color);
    if (segment_map[digit][5]) draw_rectangle_filled(x, y + thickness, thickness, v_seg_len, color);
    if (segment_map[digit][6]) draw_rectangle_filled(x + thickness, y + thickness + v_seg_len, h_seg_len, thickness, color);
}

/**
 * @brief 绘制一个任意大小的冒号 (:)。
 */
static void draw_colon(int x, int y, int width, int height, int color) {
    int dot_size = width * 6 / 10;
    if (dot_size == 0) dot_size = 1;
    int dot_x = x + (width - dot_size) / 2;
    int top_dot_y = y + (height / 4) - (dot_size / 2);
    int bottom_dot_y = y + (height * 3 / 4) - (dot_size / 2);
    draw_rectangle_filled(dot_x, top_dot_y, dot_size, dot_size, color);
    draw_rectangle_filled(dot_x, bottom_dot_y, dot_size, dot_size, color);
}


// ======================================================================
// 2. 公开的高级绘制函数 (对接 struct tm)
// ======================================================================

void Draw_Time_String(int x, int y, const struct tm* time_ptr, int digit_width, int digit_height, int color) {
    if (!time_ptr) return;

    // --- 动态计算布局参数 ---
    int colon_width = digit_width / 2;
    int spacing = digit_width / 5;
    int thickness = digit_width / 6;
    if (thickness == 0) thickness = 1;

    int current_x = x;

    // --- 绘制小时 (HH) ---
    // 从 struct tm 中获取小时
    uint8_t hour = time_ptr->tm_hour;
    draw_seven_segment_digit(current_x, y, hour / 10, digit_width, digit_height, thickness, color);
    current_x += digit_width + spacing;
    draw_seven_segment_digit(current_x, y, hour % 10, digit_width, digit_height, thickness, color);
    current_x += digit_width + spacing;

    // --- 绘制冒号 (:) ---
    draw_colon(current_x, y, colon_width, digit_height, color);
    current_x += colon_width + spacing;

    // --- 绘制分钟 (MM) ---
    // 从 struct tm 中获取分钟
    uint8_t minute = time_ptr->tm_min;
    draw_seven_segment_digit(current_x, y, minute / 10, digit_width, digit_height, thickness, color);
    current_x += digit_width + spacing;
    draw_seven_segment_digit(current_x, y, minute % 10, digit_width, digit_height, thickness, color);
}

