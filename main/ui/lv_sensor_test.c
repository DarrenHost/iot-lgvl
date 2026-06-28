/**
 * @file lv_sensor_test.c
 * @brief 传感器测试工装 - LVGL v9 实现
 * @design 参考用户上传的设计稿
 */

#include <string.h>
#include <stdlib.h>
#include "lv_sensor_test.h"
#if LV_USE_SENSOR_TEST

/*********************
 *      DEFINES
 *********************/
#define PAD               8
#define PAD_SMALL         4
#define HEADER_H          55
#define SEL_H             75
#define BTN_H             90
#define STATUS_H          160
#define WAVE_H            280  /* 波形区域高度 */
#define FOOTER_H          30

#define WAVE_POINT_COUNT  100

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    VOLTAGE_5V,
    VOLTAGE_12V,
    VOLTAGE_24V,
    VOLTAGE_MAX
} voltage_type_t;

typedef struct {
    lv_obj_t * screen;
    lv_obj_t * header;
    lv_obj_t * gs_logo;
    lv_obj_t * title;
    lv_obj_t * sel_container;
    lv_obj_t * btn_options[VOLTAGE_MAX];
    lv_obj_t * btn_test;
    lv_obj_t * btn_label;
    lv_obj_t * status_container;
    lv_obj_t * status_items[3];
    lv_obj_t * status_vals[3];
    lv_obj_t * wave_container;
    lv_obj_t * wave_chart;
    lv_obj_t * footer;
    lv_obj_t * footer_dot;
    lv_obj_t * footer_text;

    lv_chart_series_t * wave_ser;

    voltage_type_t selected_voltage;
    bool is_testing;
    uint32_t toggle_count;
    float current_voltage;
    bool current_state_high;
    float target_voltage;
} sensor_test_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void voltage_btn_event_cb(lv_event_t * e);
static void test_btn_event_cb(lv_event_t * e);
static void wave_timer_cb(lv_timer_t * timer);
static void update_status_display(sensor_test_t * st);
static void set_testing_state(sensor_test_t * st, bool testing);
static float get_high_threshold(voltage_type_t v);
static float get_low_threshold(voltage_type_t v);

/**********************
 *  STATIC VARIABLES
 **********************/
static sensor_test_t g_st;
static lv_timer_t * g_wave_timer = NULL;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_sensor_test_create(void)
{
    
    LV_FONT_DECLARE(lv_font_chinese_24);
    LV_FONT_DECLARE(lv_font_chinese_16);
    LV_FONT_DECLARE(lv_font_chinese_12);

    sensor_test_t * st = &g_st;
    memset(st, 0, sizeof(sensor_test_t));
    st->selected_voltage = VOLTAGE_24V;  /* 默认选中 24V */
    st->is_testing = false;
    st->toggle_count = 0;
    st->current_voltage = 0.0f;
    st->current_state_high = false;
    st->target_voltage = 0.0f;

    lv_obj_t * screen = lv_screen_active();
    st->screen = screen;

    /* 深色背景 #1a2a4a */
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x1a2a4a), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    /* ========== 顶部标题栏 ========== */
    st->header = lv_obj_create(screen);
    lv_obj_set_size(st->header, LV_HOR_RES - PAD * 2, HEADER_H);
    lv_obj_align(st->header, LV_ALIGN_TOP_MID, 0, PAD);
    lv_obj_set_style_bg_color(st->header, lv_color_hex(0x152238), 0);
    lv_obj_set_style_radius(st->header, 8, 0);
    lv_obj_set_style_border_width(st->header, 0, 0);
    lv_obj_set_style_pad_all(st->header, 0, 0);
    lv_obj_remove_flag(st->header, LV_OBJ_FLAG_SCROLLABLE);

    /* GS Logo - 橙色圆角方块 */
    st->gs_logo = lv_obj_create(st->header);
    lv_obj_set_size(st->gs_logo, 40, 40);
    lv_obj_align(st->gs_logo, LV_ALIGN_LEFT_MID, 12, 0);
    lv_obj_set_style_bg_color(st->gs_logo, lv_color_hex(0xff8c00), 0);
    lv_obj_set_style_radius(st->gs_logo, 6, 0);
    lv_obj_set_style_border_width(st->gs_logo, 0, 0);
    lv_obj_remove_flag(st->gs_logo, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * gs_label = lv_label_create(st->gs_logo);
    lv_label_set_text(gs_label, "GS");
    lv_obj_set_style_text_font(gs_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(gs_label, lv_color_white(), 0);
    lv_obj_center(gs_label);

    /* 标题 */
    st->title = lv_label_create(st->header);
    lv_label_set_text(st->title, "传感器测试工装");
    lv_obj_set_style_text_font(st->title, &lv_font_chinese_24, 0);
    lv_obj_set_style_text_color(st->title, lv_color_white(), 0);
    lv_obj_align(st->title, LV_ALIGN_LEFT_MID, 65, 0);

    /* ========== 传感器选择区域 ========== */
    st->sel_container = lv_obj_create(screen);
    lv_obj_set_size(st->sel_container, LV_HOR_RES - PAD * 2, SEL_H);
    lv_obj_align(st->sel_container, LV_ALIGN_TOP_MID, 0, HEADER_H + PAD * 2);
    lv_obj_set_style_bg_color(st->sel_container, lv_color_hex(0x152238), 0);
    lv_obj_set_style_radius(st->sel_container, 8, 0);
    lv_obj_set_style_border_width(st->sel_container, 0, 0);
    lv_obj_set_style_pad_all(st->sel_container, 10, 0);
    lv_obj_set_flex_flow(st->sel_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(st->sel_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(st->sel_container, 10, 0);
    lv_obj_remove_flag(st->sel_container, LV_OBJ_FLAG_SCROLLABLE);

    const char * volt_names[VOLTAGE_MAX] = {"5V", "12V", "24V"};

    for (int i = 0; i < VOLTAGE_MAX; i++) {
        st->btn_options[i] = lv_button_create(st->sel_container);
        lv_obj_set_size(st->btn_options[i], 160, 55);
        lv_obj_set_style_radius(st->btn_options[i], 8, 0);
        lv_obj_set_style_border_width(st->btn_options[i], 0, 0);
        //lv_obj_add_event_cb(st->btn_options[i], voltage_btn_event_cb, LV_EVENT_CLICKED, (void*)(intptr_t)i);

       

        lv_obj_t * volt_label = lv_label_create(st->btn_options[i]);
        lv_label_set_text_fmt(volt_label, "%s", volt_names[i]);
        lv_obj_set_style_text_font(volt_label, &lv_font_montserrat_18, 0);
        lv_obj_set_style_text_color(volt_label, lv_color_white(), 0);
        lv_obj_center(volt_label);
        lv_obj_set_y(volt_label, -6);
        lv_obj_align(volt_label, LV_ALIGN_BOTTOM_MID, 0, -6);

    }
    /* 默认选中 24V - 青色高亮 */
    lv_obj_set_style_bg_color(st->btn_options[VOLTAGE_24V], lv_color_hex(0x00adb5), 0);
    lv_obj_set_style_bg_color(st->btn_options[VOLTAGE_5V], lv_color_hex(0x1a2a4a), 0);
    lv_obj_set_style_bg_color(st->btn_options[VOLTAGE_12V], lv_color_hex(0x1a2a4a), 0);

    /* ========== 主内容区域：左侧按钮+状态，右侧波形 ========== */
    /* 左侧区域 */
    int left_w = 220;
    int right_x = left_w + PAD * 2;
    int right_w = LV_HOR_RES - right_x - PAD * 2;

    /* 测试按钮 - 绿色大按钮 */
    st->btn_test = lv_button_create(screen);
    lv_obj_set_size(st->btn_test, left_w, BTN_H);
    lv_obj_align(st->btn_test, LV_ALIGN_TOP_LEFT, PAD, HEADER_H + SEL_H + PAD * 3);
    lv_obj_set_style_bg_color(st->btn_test, lv_color_hex(0x00e676), 0);
    lv_obj_set_style_bg_color(st->btn_test, lv_color_hex(0x00c853), LV_STATE_PRESSED);
    lv_obj_set_style_radius(st->btn_test, 12, 0);
    lv_obj_set_style_border_width(st->btn_test, 0, 0);
    lv_obj_set_style_shadow_width(st->btn_test, 8, 0);
    lv_obj_set_style_shadow_color(st->btn_test, lv_color_hex(0x00e676), 0);
    lv_obj_set_style_shadow_opa(st->btn_test, LV_OPA_30, 0);
    lv_obj_add_event_cb(st->btn_test, test_btn_event_cb, LV_EVENT_CLICKED, NULL);

    st->btn_label = lv_label_create(st->btn_test);
    lv_label_set_text(st->btn_label, "开始测试");
    lv_obj_set_style_text_font(st->btn_label, &lv_font_chinese_24, 0);
    lv_obj_set_style_text_color(st->btn_label, lv_color_white(), 0);
    lv_obj_center(st->btn_label);

    /* 状态面板 */
    st->status_container = lv_obj_create(screen);
    lv_obj_set_size(st->status_container, left_w, STATUS_H);
    lv_obj_align(st->status_container, LV_ALIGN_TOP_LEFT, PAD, 
                 HEADER_H + SEL_H + BTN_H + PAD * 4);
    lv_obj_set_style_bg_color(st->status_container, lv_color_hex(0x152238), 0);
    lv_obj_set_style_radius(st->status_container, 8, 0);
    lv_obj_set_style_border_width(st->status_container, 0, 0);
    lv_obj_set_style_pad_all(st->status_container, 10, 0);
    lv_obj_set_flex_flow(st->status_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(st->status_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(st->status_container, 8, 0);
    lv_obj_remove_flag(st->status_container, LV_OBJ_FLAG_SCROLLABLE);

    const char * status_labels[3] = {"HIGHT", "LOW", "COUNT"};
    lv_color_t status_colors[3] = {lv_color_hex(0x00e676), lv_color_hex(0xff5252), lv_color_hex(0xffd740)};
    lv_color_t status_val_colors[3] = {lv_color_hex(0x00e676), lv_color_hex(0xff5252), lv_color_hex(0xffd740)};

    for (int i = 0; i < 3; i++) {
        lv_obj_t * item = lv_obj_create(st->status_container);
        lv_obj_set_size(item, LV_PCT(100), 40);
        lv_obj_set_style_bg_color(item, lv_color_hex(0x1a2a4a), 0);
        lv_obj_set_style_radius(item, 6, 0);
        lv_obj_set_style_border_width(item, 0, 0);
        lv_obj_set_style_pad_all(item, 0, 0);
        lv_obj_remove_flag(item, LV_OBJ_FLAG_SCROLLABLE);

        /* 左边彩色条 */
        lv_obj_t * bar = lv_obj_create(item);
        lv_obj_set_size(bar, 3, 30);
        lv_obj_align(bar, LV_ALIGN_LEFT_MID, 8, 0);
        lv_obj_set_style_bg_color(bar, status_colors[i], 0);
        lv_obj_set_style_radius(bar, 2, 0);
        lv_obj_set_style_border_width(bar, 0, 0);
        lv_obj_remove_flag(bar, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t * label = lv_label_create(item);
        lv_label_set_text(label, status_labels[i]);
        lv_obj_set_style_text_color(label, lv_color_hex(0xaaaaaa), 0);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
        lv_obj_align(label, LV_ALIGN_LEFT_MID, 20, 0);

        st->status_vals[i] = lv_label_create(item);
        lv_label_set_text(st->status_vals[i], i == 2 ? "0" : "0.00 V");
        lv_obj_set_style_text_color(st->status_vals[i], status_val_colors[i], 0);
        lv_obj_set_style_text_font(st->status_vals[i], &lv_font_montserrat_18, 0);
        lv_obj_align(st->status_vals[i], LV_ALIGN_RIGHT_MID, -12, 0);
    }

    /* ========== 右侧波形区域 ========== */
    st->wave_container = lv_obj_create(screen);
    lv_obj_set_size(st->wave_container, right_w, BTN_H + STATUS_H + PAD);
    lv_obj_align(st->wave_container, LV_ALIGN_TOP_LEFT, right_x, 
                 HEADER_H + SEL_H + PAD * 3);
    lv_obj_set_style_bg_color(st->wave_container, lv_color_hex(0x152238), 0);
    lv_obj_set_style_radius(st->wave_container, 8, 0);
    lv_obj_set_style_border_width(st->wave_container, 0, 0);
    lv_obj_set_style_pad_all(st->wave_container, 8, 0);
    lv_obj_remove_flag(st->wave_container, LV_OBJ_FLAG_SCROLLABLE);

    /* 波形标题 */
    lv_obj_t * wave_title = lv_label_create(st->wave_container);
    lv_label_set_text(wave_title, "波形");
    lv_obj_set_style_text_color(wave_title, lv_color_hex(0xaaaaaa), 0);
    lv_obj_set_style_text_font(wave_title, &lv_font_chinese_16, 0);
    lv_obj_align(wave_title, LV_ALIGN_TOP_LEFT, 4, 0);

    /* 波形图表 */
    st->wave_chart = lv_chart_create(st->wave_container);
    lv_obj_set_size(st->wave_chart, right_w - 16, BTN_H + STATUS_H - 30);
    lv_obj_align(st->wave_chart, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(st->wave_chart, lv_color_hex(0x0a0a1a), 0);
    lv_obj_set_style_radius(st->wave_chart, 6, 0);
    lv_chart_set_type(st->wave_chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(st->wave_chart, WAVE_POINT_COUNT);
    lv_chart_set_range(st->wave_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 300);
    lv_chart_set_update_mode(st->wave_chart, LV_CHART_UPDATE_MODE_SHIFT);
    lv_obj_set_style_size(st->wave_chart, 0, 0, LV_PART_INDICATOR);
    lv_obj_set_style_line_width(st->wave_chart, 2, LV_PART_ITEMS);
    lv_obj_set_style_line_color(st->wave_chart, lv_color_hex(0x00adb5), LV_PART_ITEMS);
    lv_obj_set_style_border_width(st->wave_chart, 1, 0);
    lv_obj_set_style_border_color(st->wave_chart, lv_color_hex(0x2d4a6f), 0);
    lv_obj_set_style_line_width(st->wave_chart, 0, LV_PART_MAIN);
    lv_obj_set_style_text_opa(st->wave_chart, LV_OPA_TRANSP, LV_PART_MAIN);

    st->wave_ser = lv_chart_add_series(st->wave_chart, lv_color_hex(0x00adb5), LV_CHART_AXIS_PRIMARY_Y);
    for (int i = 0; i < WAVE_POINT_COUNT; i++) {
        lv_chart_set_value_by_id(st->wave_chart, st->wave_ser, i, 0);
    }

    /* 电压标记 */
    lv_obj_t * v_max = lv_label_create(st->wave_container);
    lv_label_set_text(v_max, "5.0V");
    lv_obj_set_style_text_color(v_max, lv_color_hex(0x666666), 0);
    lv_obj_set_style_text_font(v_max, &lv_font_montserrat_12, 0);
    lv_obj_align(v_max, LV_ALIGN_TOP_LEFT, 8, 18);

    lv_obj_t * v_mid = lv_label_create(st->wave_container);
    lv_label_set_text(v_mid, "2.5V");
    lv_obj_set_style_text_color(v_mid, lv_color_hex(0x666666), 0);
    lv_obj_set_style_text_font(v_mid, &lv_font_montserrat_12, 0);
    lv_obj_align(v_mid, LV_ALIGN_LEFT_MID, 8, -10);

    lv_obj_t * v_min = lv_label_create(st->wave_container);
    lv_label_set_text(v_min, "0V");
    lv_obj_set_style_text_color(v_min, lv_color_hex(0x666666), 0);
    lv_obj_set_style_text_font(v_min, &lv_font_montserrat_12, 0);
    lv_obj_align(v_min, LV_ALIGN_BOTTOM_LEFT, 8, -4);

    /* ========== 底部状态栏 ========== */
    st->footer = lv_obj_create(screen);
    lv_obj_set_size(st->footer, LV_HOR_RES - PAD * 2, FOOTER_H);
    lv_obj_align(st->footer, LV_ALIGN_BOTTOM_MID, 0, -PAD);
    lv_obj_set_style_bg_color(st->footer, lv_color_hex(0x152238), 0);
    lv_obj_set_style_radius(st->footer, 6, 0);
    lv_obj_set_style_border_width(st->footer, 0, 0);
    lv_obj_set_style_pad_all(st->footer, 0, 0);
    lv_obj_remove_flag(st->footer, LV_OBJ_FLAG_SCROLLABLE);

    st->footer_dot = lv_obj_create(st->footer);
    lv_obj_set_size(st->footer_dot, 8, 8);
    lv_obj_align(st->footer_dot, LV_ALIGN_LEFT_MID, 12, 0);
    lv_obj_set_style_bg_color(st->footer_dot, lv_color_hex(0x888888), 0);
    lv_obj_set_style_radius(st->footer_dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(st->footer_dot, 0, 0);
    lv_obj_remove_flag(st->footer_dot, LV_OBJ_FLAG_SCROLLABLE);

    st->footer_text = lv_label_create(st->footer);
    lv_label_set_text(st->footer_text, "待机中");
    lv_obj_set_style_text_color(st->footer_text, lv_color_hex(0x888888), 0);
    lv_obj_set_style_text_font(st->footer_text, &lv_font_chinese_16, 0);
    lv_obj_align(st->footer_text, LV_ALIGN_LEFT_MID, 28, 0);

    lv_obj_t * footer_ver = lv_label_create(st->footer);
    lv_label_set_text(footer_ver, "GS Vending | Sensor Test Fixture v1.0");
    lv_obj_set_style_text_color(footer_ver, lv_color_hex(0x888888), 0);
    lv_obj_set_style_text_font(footer_ver, &lv_font_montserrat_12, 0);
    lv_obj_align(footer_ver, LV_ALIGN_RIGHT_MID, -12, 0);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void voltage_btn_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    // sensor_test_t * st = &g_st;
    // voltage_type_t selected = (voltage_type_t)(intptr_t)lv_event_get_user_data(e);

    // if (st->is_testing) return;

    // st->selected_voltage = selected;

    // for (int i = 0; i < VOLTAGE_MAX; i++) {
    //     if (i == selected) {
    //         lv_obj_set_style_bg_color(st->btn_options[i], lv_color_hex(0x00adb5), 0);
    //         //lv_obj_set_style_text_color(lv_obj_get_child(st->btn_options[i], 0), lv_color_white(), 0);
    //     } else {
    //         lv_obj_set_style_bg_color(st->btn_options[i], lv_color_hex(0x1a2a4a), 0);
    //         //lv_obj_set_style_text_color(lv_obj_get_child(st->btn_options[i], 0), lv_color_white(), 0);
    //     }
    // }

    //int max_val = (selected == VOLTAGE_5V) ? 60 : (selected == VOLTAGE_12V ? 150 : 300);
    //lv_chart_set_range(st->wave_chart, LV_CHART_AXIS_PRIMARY_Y, 0, max_val);
}

static void test_btn_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    sensor_test_t * st = &g_st;

    if (!st->is_testing) {
        set_testing_state(st, true);
        g_wave_timer = lv_timer_create(wave_timer_cb, 100, st);
    } else {
        set_testing_state(st, false);
        if (g_wave_timer) {
            lv_timer_delete(g_wave_timer);
            g_wave_timer = NULL;
        }
    }
}

static void wave_timer_cb(lv_timer_t * timer)
{
    sensor_test_t * st = (sensor_test_t *)lv_timer_get_user_data(timer);

    if ((rand() % 100) < 5) {
        st->current_state_high = !st->current_state_high;
        st->target_voltage = st->current_state_high ? 
            get_high_threshold(st->selected_voltage) : 
            get_low_threshold(st->selected_voltage);

        if (st->current_state_high) {
            st->toggle_count++;
        }
    }

    st->current_voltage += (st->target_voltage - st->current_voltage) * 0.1f;

    update_status_display(st);

    int max_val = (st->selected_voltage == VOLTAGE_5V) ? 60 : 
                  (st->selected_voltage == VOLTAGE_12V ? 150 : 300);
    int32_t y_val = (int32_t)((st->current_voltage / get_high_threshold(st->selected_voltage)) * max_val);
    if (y_val < 0) y_val = 0;
    if (y_val > max_val) y_val = max_val;

    lv_chart_set_next_value(st->wave_chart, st->wave_ser, y_val);
}

static void update_status_display(sensor_test_t * st)
{
    char buf[32];

    if (st->current_state_high) {
        lv_snprintf(buf, sizeof(buf), "%.2f V", st->current_voltage);
        lv_label_set_text(st->status_vals[0], buf);
    } else {
        lv_snprintf(buf, sizeof(buf), "%.2f V", st->current_voltage);
        lv_label_set_text(st->status_vals[1], buf);
    }

    lv_snprintf(buf, sizeof(buf), "%lu", st->toggle_count);
    lv_label_set_text(st->status_vals[2], buf);
}

static void set_testing_state(sensor_test_t * st, bool testing)
{
    st->is_testing = testing;

    if (testing) {
        lv_label_set_text(st->btn_label, "停止测试");
        lv_obj_set_style_bg_color(st->btn_test, lv_color_hex(0xffd740), 0);
        lv_obj_set_style_bg_color(st->btn_test, lv_color_hex(0xe6c200), LV_STATE_PRESSED);
        lv_obj_set_style_shadow_color(st->btn_test, lv_color_hex(0xffd740), 0);

        lv_obj_set_style_bg_color(st->footer_dot, lv_color_hex(0xffd740), 0);
        lv_label_set_text(st->footer_text, "测试进行中...");
        lv_obj_set_style_text_color(st->footer_text, lv_color_hex(0xffd740), 0);

        st->current_voltage = get_low_threshold(st->selected_voltage);
        st->target_voltage = st->current_voltage;
        st->current_state_high = false;

    } else {
        lv_label_set_text(st->btn_label, "开始测试");
        lv_obj_set_style_bg_color(st->btn_test, lv_color_hex(0x00e676), 0);
        lv_obj_set_style_bg_color(st->btn_test, lv_color_hex(0x00c853), LV_STATE_PRESSED);
        lv_obj_set_style_shadow_color(st->btn_test, lv_color_hex(0x00e676), 0);

        lv_obj_set_style_bg_color(st->footer_dot, lv_color_hex(0x888888), 0);
        lv_label_set_text(st->footer_text, "待机中");
        lv_obj_set_style_text_color(st->footer_text, lv_color_hex(0x888888), 0);

        st->toggle_count = 0;
        st->current_voltage = 0;
        st->target_voltage = 0;
        st->current_state_high = false;

        lv_label_set_text(st->status_vals[0], "0.00 V");
        lv_label_set_text(st->status_vals[1], "0.00 V");
        lv_label_set_text(st->status_vals[2], "0");

        for (int i = 0; i < WAVE_POINT_COUNT; i++) {
            lv_chart_set_value_by_id(st->wave_chart, st->wave_ser, i, 0);
        }
        lv_chart_refresh(st->wave_chart);
    }
}

static float get_high_threshold(voltage_type_t v)
{
    switch (v) {
        case VOLTAGE_5V:  return 4.5f;
        case VOLTAGE_12V: return 10.8f;
        case VOLTAGE_24V: return 21.6f;
        default: return 0;
    }
}

static float get_low_threshold(voltage_type_t v)
{
    switch (v) {
        case VOLTAGE_5V:  return 0.5f;
        case VOLTAGE_12V: return 1.2f;
        case VOLTAGE_24V: return 2.4f;
        default: return 0;
    }
}

#endif /* LV_USE_SENSOR_TEST */