#include "lv_login_page.h"

#if LV_USE_LOGIN_PAGE

typedef struct {
    lv_obj_t * container;
    lv_obj_t * user_input;
    lv_obj_t * pwd_input;
    lv_obj_t * login_btn;
    lv_obj_t * btn_label;
    lv_obj_t * status_label;
} login_page_t;

static login_page_t g_login;

static void login_event_cb(lv_event_t * e);
static void pwd_toggle_event_cb(lv_event_t * e);

/**
 * @brief 运行时计算登录框尺寸（替代 #if LV_HOR_RES >= LV_VER_RES）
 */
static void calc_login_size(int32_t * w, int32_t * h, int32_t * input_h, int32_t * btn_h)
{
    int32_t hor = lv_display_get_horizontal_resolution(lv_display_get_default());
    int32_t ver = lv_display_get_vertical_resolution(lv_display_get_default());
    
    if(hor >= ver) {
        /* 横屏 800x480：宽度 340，高度 380 */
        *w = 340;
        *h = 380;
        *input_h = 40;
        *btn_h = 44;
    } else {
        /* 竖屏 480x800：宽度 360，高度 440 */
        *w = 360;
        *h = 440;
        *input_h = 50;
        *btn_h = 50;
    }
}

void lv_login_page_create(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_palette_main(LV_PALETTE_BLUE_GREY), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    /* 运行时计算尺寸 */
    int32_t login_w, login_h, input_h, btn_h;
    calc_login_size(&login_w, &login_h, &input_h, &btn_h);

    /* 白色卡片容器 */
    g_login.container = lv_obj_create(screen);
    lv_obj_set_size(g_login.container, login_w, login_h);
    lv_obj_center(g_login.container);
    lv_obj_set_style_bg_color(g_login.container, lv_color_white(), 0);
    lv_obj_set_style_radius(g_login.container, 12, 0);
    lv_obj_set_style_shadow_width(g_login.container, 16, 0);
    lv_obj_set_style_shadow_color(g_login.container, lv_color_black(), 0);
    lv_obj_set_style_shadow_opa(g_login.container, LV_OPA_20, 0);
    lv_obj_set_style_pad_all(g_login.container, 30, 0);
    lv_obj_set_style_border_width(g_login.container, 0, 0);

    /* Flex 纵向布局 */
    lv_obj_set_flex_flow(g_login.container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(g_login.container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(g_login.container, 16, 0);

    /* 标题 */
    lv_obj_t * title = lv_label_create(g_login.container);
    lv_label_set_text(title, "User Login");
    lv_obj_set_style_text_color(title, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_margin_bottom(title, 10, 0);

    /* 用户名输入框 */
    g_login.user_input = lv_textarea_create(g_login.container);
    lv_obj_set_width(g_login.user_input, LV_PCT(100));
    lv_obj_set_height(g_login.user_input, input_h);
    lv_textarea_set_placeholder_text(g_login.user_input, "Username");
    lv_textarea_set_one_line(g_login.user_input, true);
    lv_obj_set_style_bg_color(g_login.user_input, lv_palette_lighten(LV_PALETTE_GREY, 4), 0);
    lv_obj_set_style_radius(g_login.user_input, 8, 0);
    lv_obj_set_style_pad_left(g_login.user_input, 12, 0);

    /* 密码输入框 */
    g_login.pwd_input = lv_textarea_create(g_login.container);
    lv_obj_set_width(g_login.pwd_input, LV_PCT(100));
    lv_obj_set_height(g_login.pwd_input, input_h);
    lv_textarea_set_placeholder_text(g_login.pwd_input, "Password");
    lv_textarea_set_one_line(g_login.pwd_input, true);
    lv_textarea_set_password_mode(g_login.pwd_input, true);
    lv_obj_set_style_bg_color(g_login.pwd_input, lv_palette_lighten(LV_PALETTE_GREY, 4), 0);
    lv_obj_set_style_radius(g_login.pwd_input, 8, 0);
    lv_obj_set_style_pad_left(g_login.pwd_input, 12, 0);

    /* 密码显示/隐藏切换 */
    lv_obj_t * pwd_toggle = lv_button_create(g_login.pwd_input);
    lv_obj_set_size(pwd_toggle, 50, input_h - 8);
    lv_obj_align(pwd_toggle, LV_ALIGN_RIGHT_MID, -4, 0);
    lv_obj_set_style_bg_color(pwd_toggle, lv_color_white(), 0);
    lv_obj_set_style_shadow_width(pwd_toggle, 0, 0);
    lv_obj_set_style_radius(pwd_toggle, 6, 0);
    lv_obj_t * toggle_icon = lv_label_create(pwd_toggle);
    lv_label_set_text(toggle_icon, "Hide");
    lv_obj_center(toggle_icon);
    lv_obj_set_style_text_color(toggle_icon, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_add_event_cb(pwd_toggle, pwd_toggle_event_cb, LV_EVENT_CLICKED, g_login.pwd_input);

    /* 登录按钮 */
    g_login.login_btn = lv_button_create(g_login.container);
    lv_obj_set_width(g_login.login_btn, LV_PCT(100));
    lv_obj_set_height(g_login.login_btn, btn_h);
    lv_obj_set_style_bg_color(g_login.login_btn, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_bg_color(g_login.login_btn, lv_palette_darken(LV_PALETTE_BLUE, 2), LV_STATE_PRESSED);
    lv_obj_set_style_radius(g_login.login_btn, 10, 0);
    lv_obj_add_event_cb(g_login.login_btn, login_event_cb, LV_EVENT_CLICKED, NULL);

    g_login.btn_label = lv_label_create(g_login.login_btn);
    lv_label_set_text(g_login.btn_label, "Login");
    lv_obj_set_style_text_color(g_login.btn_label, lv_color_white(), 0);
    lv_obj_center(g_login.btn_label);

    /* 状态提示 */
    g_login.status_label = lv_label_create(g_login.container);
    lv_label_set_text(g_login.status_label, "");
    lv_obj_set_style_text_color(g_login.status_label, lv_palette_main(LV_PALETTE_RED), 0);

    /* 入场动画 */
    lv_obj_fade_in(g_login.container, 400, 100);
}

const char * lv_login_page_get_username(void)
{
    return lv_textarea_get_text(g_login.user_input);
}

const char * lv_login_page_get_password(void)
{
    return lv_textarea_get_text(g_login.pwd_input);
}

void lv_login_page_show_status(const char * text, lv_color_t color)
{
    lv_label_set_text(g_login.status_label, text);
    lv_obj_set_style_text_color(g_login.status_label, color, 0);
}

static void login_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    const char * user = lv_textarea_get_text(g_login.user_input);
    const char * pwd  = lv_textarea_get_text(g_login.pwd_input);

    if(user == NULL || user[0] == '\0') {
        lv_label_set_text(g_login.status_label, "Please enter username");
        lv_obj_set_style_text_color(g_login.status_label, lv_palette_main(LV_PALETTE_RED), 0);
        return;
    }
    if(pwd == NULL || pwd[0] == '\0') {
        lv_label_set_text(g_login.status_label, "Please enter password");
        lv_obj_set_style_text_color(g_login.status_label, lv_palette_main(LV_PALETTE_RED), 0);
        return;
    }

    lv_label_set_text(g_login.btn_label, "Logging in...");
    lv_obj_set_style_bg_color(g_login.login_btn, lv_palette_main(LV_PALETTE_GREY), 0);

    lv_label_set_text(g_login.status_label, "Login successful!");
    lv_obj_set_style_text_color(g_login.status_label, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_label_set_text(g_login.btn_label, "Login");
    lv_obj_set_style_bg_color(g_login.login_btn, lv_palette_main(LV_PALETTE_BLUE), 0);
}

static void pwd_toggle_event_cb(lv_event_t * e)
{
    lv_obj_t * pwd_input = lv_event_get_user_data(e);
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * icon = lv_obj_get_child(btn, 0);
    bool is_pwd = lv_textarea_get_password_mode(pwd_input);
    lv_textarea_set_password_mode(pwd_input, !is_pwd);
    lv_label_set_text(icon, is_pwd ? "Show" : "Hide");
}

#endif