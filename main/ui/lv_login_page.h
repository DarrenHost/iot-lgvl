/**
 * @file lv_login_page.h
 * @brief 登录页面头文件
 */

#ifndef LV_LOGIN_PAGE_H
#define LV_LOGIN_PAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/
#ifndef LV_USE_LOGIN_PAGE
#  define LV_USE_LOGIN_PAGE 1
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief 创建登录页面
 */
void lv_login_page_create(void);

/**
 * @brief 获取用户名
 * @return 用户名文本
 */
const char * lv_login_page_get_username(void);

/**
 * @brief 获取密码
 * @return 密码文本
 */
const char * lv_login_page_get_password(void);

/**
 * @brief 显示状态信息
 * @param text 提示文本
 * @param color 文本颜色
 */
void lv_login_page_show_status(const char * text, lv_color_t color);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LOGIN_PAGE_H*/