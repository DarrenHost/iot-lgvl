set(LV_DEMO_DIR ../managed_components/lvgl__lvgl/demos)
file(GLOB_RECURSE LV_DEMOS_SOURCES ${LV_DEMO_DIR}/*.c)

set(MY_UI_SOURCES
    ui/lv_login_page.c
    ui/lv_sensor_test.c 
    ui/lv_font_chinese_12.c
    ui/lv_font_chinese_16.c
    ui/lv_font_chinese_24.c
)

idf_component_register(
    SRCS main.c ${LV_DEMOS_SOURCES} ${MY_UI_SOURCES}
    INCLUDE_DIRS . ${LV_DEMO_DIR} ui)    

idf_component_get_property(LVGL_LIB lvgl__lvgl COMPONENT_LIB)
target_compile_options(
    ${LVGL_LIB} 
    PRIVATE
        -DLV_LVGL_H_INCLUDE_SIMPLE
        -DLV_USE_DEMO_MUSIC
)
