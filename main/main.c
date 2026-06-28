#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_memory_utils.h"
#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#include "bsp_board_extra.h"
#include "lv_demos.h"
#include "lv_login_page.h"
#include "lv_sensor_test.h"




/* 100ms 定时器回调 */
void sensor_timer_cb(lv_timer_t *timer) {
    (void)timer;

    // if (!lv_sensor_test_is_testing(ui)) return;

    // /* 你的传感器数据采集逻辑 */
    // float voltage = sensor_adc_read();  // 实际 ADC 读取
    // int is_high = (voltage > 2.5f);

    // if (is_high) {
    //     toggle_count++;
    //     lv_sensor_test_update_toggle_count(ui, toggle_count);
    // }

    // lv_sensor_test_update_voltage(ui, voltage, is_high);

    // /* 添加波形点 (Y 轴 0-150) */
    // int16_t y_value = 75 - (int16_t)((voltage / 24.0f) * 70);
    // lv_sensor_test_add_waveform_point(ui, y_value);
}


void app_main(void)
{
    bsp_display_cfg_t cfg = {
        .lv_adapter_cfg = ESP_LV_ADAPTER_DEFAULT_CONFIG(),
        .rotation = ESP_LV_ADAPTER_ROTATE_90,
        .tear_avoid_mode = ESP_LV_ADAPTER_TEAR_AVOID_MODE_TRIPLE_PARTIAL,
        .touch_flags = {
            .swap_xy = 1,
            .mirror_x = 1,
            .mirror_y = 0
        }};
    bsp_display_start_with_config(&cfg);
    bsp_display_backlight_on();

    bsp_display_lock(-1);
    
    // lv_obj_t * label = lv_label_create(lv_screen_active());
    // lv_label_set_text(label, "Screen OK");
    // lv_obj_center(label);
    //lv_demo_music();
    //lv_demo_benchmark();
    //lv_demo_widgets();

    /* 创建 UI */
    lv_sensor_test_create();

    /* 创建定时器 */
    //lv_timer_create(sensor_timer_cb, 100, NULL);

    // while (1) {
    //     lv_timer_handler();
    //      sleep(5000);
    // }

    bsp_display_unlock();
}