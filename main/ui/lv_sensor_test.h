/**
 * @file lv_sensor_test.h
 * @brief 传感器测试工装头文件
 */

#ifndef LV_SENSOR_TEST_H
#define LV_SENSOR_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

#ifndef LV_USE_SENSOR_TEST
#define LV_USE_SENSOR_TEST 1
#endif

/**
 * @brief 创建传感器测试界面
 */
void lv_sensor_test_create(void);

#ifdef __cplusplus
}
#endif

#endif /* LV_SENSOR_TEST_H */