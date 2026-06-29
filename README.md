# IoT-LVGL 传感器测试工装

基于 ESP32-P4 + LVGL v9 的工业传感器测试界面，支持 800x480 触摸屏显示。

![ESP32-P4](https://img.shields.io/badge/ESP32-P4-blue)
![LVGL](https://img.shields.io/badge/LVGL-v9-green)
![Screen](https://img.shields.io/badge/Screen-800x480-orange)

---

## 📋 项目功能

### 传感器测试界面

- ✅ **GS 标志** - 左上角橙色渐变 Logo
- ✅ **标题显示** - "传感器测试工装" 加粗中文字体
- ✅ **电压选择** - 5V / 12V / 24V 光电传感器切换
- ✅ **测试控制** - 开始/停止测试按钮
- ✅ **实时显示** - 高电平、低电平电压值
- ✅ **切换计数** - 高低电平切换次数统计
- ✅ **波形图表** - 传感器电压实时波形 (100 点滚动)
- ✅ **状态指示** - 待机/测试中状态灯

### 界面布局

```
┌─────────────────────────────────────────────┐
│  GS  传感器测试工装                          │  Header (55px)
├─────────────────────────────────────────────┤
│  [5V]      [12V]      [24V]                 │  选择区 (75px)
│  光电传感器  光电传感器  光电传感器           │
├──────────────┬──────────────────────────────┤
│  左侧 (280px) │  右侧 (自适应)                │
│  ┌─────────┐ │  ┌──────────────────────┐    │  主内容区
│  │开始测试 │ │  │  传感器电压波形       │    │
│  └─────────┘ │  │  ┌──────────────────┐ │    │
│  ┌─────────┐ │  │  │  波形图表        │ │    │
│  │高电平   │ │  │  │  ~~~~~~~~~~~~~~  │ │    │
│  │低电平   │ │  │  └──────────────────┘ │    │
│  │切换次数 │ │  └──────────────────────┘    │
├──────────────┴──────────────────────────────┤
│  ● 待机中        GS Vending | v1.0          │  Footer (30px)
└─────────────────────────────────────────────┘
```

---

## 🛠️ 硬件要求

| 组件 | 规格 |
|------|------|
| **主控** | ESP32-P4 (双核 RISC-V, 400MHz) |
| **Flash** | 16MB QIO |
| **PSRAM** | 32MB (200MHz) |
| **显示屏** | 800x480 RGB LCD (4.3 寸) |
| **触摸屏** | 电容触摸 (I2C) |
| **电源** | 5V/2A USB-C |

### 推荐开发板

- ESP32-P4-Function-Evaluation-Board
- 或兼容的 4.3 寸 800x480 HMI 板

---

## 📦 项目结构

```
iot-lgvl/
├── main/
│   ├── main.c                  # 主入口
│   ├── CMakeLists.txt          # 组件配置
│   └── ui/
│       ├── lv_sensor_test.c    # 传感器测试 UI (496 行)
│       ├── lv_sensor_test.h    # UI 头文件
│       ├── lv_login_page.c/h   # 登录页面
│       ├── lv_font_chinese_12.c
│       ├── lv_font_chinese_16.c
│       ├── lv_font_chinese_18.c
│       └── lv_font_chinese_24.c
├── components/
│   ├── bsp_extra/              # BSP 扩展
│   └── esp32_p4_wifi6_touch_lcd_4_3/  # 板级支持包
├── CMakeLists.txt              # 项目配置
├── sdkconfig.defaults          # SDK 配置
├── partitions.csv              # 分区表
└── README.md                   # 本文档
```

---

## 🔨 编译步骤

### 1. 环境准备

```bash
# 安装 ESP-IDF v5.4+
git clone -b v5.4 https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32p4
source export.sh
```

### 2. 配置项目

```bash
cd iot-lgvl
idf.py set-target esp32p4
```

### 3. 编译固件

```bash
#  Release 编译
idf.py build

# Debug 编译 (带日志)
idf.py -D CMAKE_BUILD_TYPE=Debug build
```

### 4. 烧录固件

```bash
# 替换实际串口端口
idf.py -p /dev/ttyUSB0 flash

# 烧录并监控
idf.py -p /dev/ttyUSB0 flash monitor
```

### 5. 查看日志

```bash
# 单独监控
idf.py -p /dev/ttyUSB0 monitor

# 日志级别
idf.py menuconfig  # Component config -> Log output
```

---

## ⚙️ 配置说明

### sdkconfig.defaults 关键配置

```ini
# 目标芯片
CONFIG_IDF_TARGET="esp32p4"

# Flash/PSRAM
CONFIG_ESPTOOLPY_FLASHSIZE_32MB=y
CONFIG_SPIRAM=y
CONFIG_SPIRAM_SPEED_200M=y

# LVGL 配置
CONFIG_LV_OS_FREERTOS=y
CONFIG_LV_DEF_REFR_PERIOD=15      # 15ms 刷新率 (66Hz)
CONFIG_LV_OBJ_STYLE_CACHE=y       # 样式缓存
CONFIG_LV_DRAW_SW_DRAW_UNIT_CNT=2 # 2 个绘制单元

# 中文字体
CONFIG_LV_FONT_MONTSERRAT_12=y
CONFIG_LV_FONT_MONTSERRAT_16=y
CONFIG_LV_FONT_MONTSERRAT_18=y
CONFIG_LV_FONT_MONTSERRAT_24=y
CONFIG_LV_USE_IMGFONT=y           # 图片字体支持

# LVGL 演示
CONFIG_LV_USE_DEMO_WIDGETS=y
CONFIG_LV_USE_DEMO_MUSIC=y
```

---

## 💻 API 使用

### 创建传感器测试界面

```c
#include "lv_sensor_test.h"

void app_main(void)
{
    // 初始化显示
    bsp_display_start_with_config(&cfg);
    bsp_display_backlight_on();
    
    // 创建 UI
    bsp_display_lock(-1);
    lv_sensor_test_create();
    bsp_display_unlock();
}
```

### 更新传感器数据

```c
// 在定时器中更新显示
void sensor_timer_cb(lv_timer_t *timer)
{
    sensor_test_t *ui = (sensor_test_t *)timer->user_data;
    
    // 读取实际传感器电压
    float voltage = sensor_adc_read();
    int is_high = (voltage > 2.5f);
    
    // 更新 UI
    lv_sensor_test_update_voltage(ui, voltage, is_high);
    
    // 添加波形点
    int16_t y_value = 75 - (int16_t)((voltage / 24.0f) * 70);
    lv_sensor_test_add_waveform_point(ui, y_value);
}
```

### UI 接口函数

```c
// 创建界面
void lv_sensor_test_create(void);

// 获取当前电压设置
voltage_type_t lv_sensor_test_get_voltage(void);

// 设置电压
void lv_sensor_test_set_voltage(voltage_type_t v);

// 获取测试状态
bool lv_sensor_test_is_testing(void);

// 更新电压显示
void lv_sensor_test_update_voltage(float voltage, int is_high);

// 更新切换次数
void lv_sensor_test_update_toggle_count(uint32_t count);

// 添加波形点
void lv_sensor_test_add_waveform_point(int16_t value);
```

---

## 🎨 UI 定制

### 修改颜色方案

编辑 `main/ui/lv_sensor_test.c`:

```c
#define COLOR_BG_PRIMARY      lv_color_hex(0x1a1a2e)
#define COLOR_BG_SECONDARY    lv_color_hex(0x16213e)
#define COLOR_ACCENT_BLUE     lv_color_hex(0x00adb5)
#define COLOR_ACCENT_GREEN    lv_color_hex(0x00ff88)
#define COLOR_ACCENT_RED      lv_color_hex(0xff6b6b)
#define COLOR_ACCENT_YELLOW   lv_color_hex(0xffd93d)
```

### 修改屏幕尺寸

```c
// 在 lv_sensor_test.c 中调整布局宏
#define HEADER_H          55
#define SEL_H             75
#define BTN_H             90
#define STATUS_H          160
#define WAVE_H            280
```

---

## 📊 内存占用

| 项目 | 占用 |
|------|------|
| 代码段 (Flash) | ~2.5MB |
| 中文字体 | ~200KB |
| LVGL 堆内存 | ~5MB |
| 显示缓冲 (3 重) | ~2.3MB |
| **总 PSRAM** | **~8MB** |

---

## 🐛 故障排查

### 屏幕无显示

```bash
# 检查背光
bsp_display_backlight_on();

# 检查屏幕初始化日志
I (1234) bsp_display: Display initialized
I (1235) lvgl: LVGL v9.x initialized
```

### 触摸不响应

```c
// 检查触摸配置
bsp_display_cfg_t cfg = {
    .touch_flags = {
        .swap_xy = 1,
        .mirror_x = 1,
        .mirror_y = 0
    }
};
```

### 编译错误

```bash
# 清理构建
idf.py fullclean

# 重新配置
idf.py set-target esp32p4

# 重新编译
idf.py build
```

### 中文字体乱码

确保：
1. `sdkconfig.defaults` 中启用对应字体
2. `lv_sensor_test.c` 中声明字体：`LV_FONT_DECLARE(lv_font_chinese_24);`
3. CMakeLists.txt 包含字体源文件

---

## 📝 版本历史

| 版本 | 日期 | 更新内容 |
|------|------|----------|
| v1.0 | 2026-06-29 | 初始版本 - 传感器测试界面 |
| v1.0 | 2026-06-29 | 添加登录页面 |
| v1.0 | 2026-06-29 | 完整中文字体支持 |

---

## 📄 许可证

MIT License

---

## 👨‍💻 作者

- **Darren Host** - [GitHub](https://github.com/DarrenHost)

---

## 🔗 相关资源

- [ESP-IDF 编程指南](https://docs.espressif.com/projects/esp-idf/en/latest/esp32p4/)
- [LVGL v9 文档](https://docs.lvgl.io/master/)
- [ESP32-P4 数据手册](https://www.espressif.com/sites/default/files/documentation/esp32-p4_datasheet_en.pdf)

---

## 📧 技术支持

如有问题请提交 Issue 或联系：
- GitHub Issues: https://github.com/DarrenHost/iot-lgvl/issues
- Email: dev@coffeeji.com
