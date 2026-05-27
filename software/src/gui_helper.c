/**
 * ============================================================================
 * GUI 辅助函数模块 | GUI Helper Functions Module
 * ============================================================================
 *
 * 基于 LVGL 8.3.1 框架的图形界面辅助函数。
 * GUI helper functions based on the LVGL 8.3.1 framework.
 *
 * ============================================================
 * 界面布局设计 | UI Layout Design
 * ============================================================
 *
 * TinyAWG 的 GUI 分为四个主要屏幕 (页面):
 * TinyAWG's GUI has four main screens (pages):
 *
 *   1. NORMAL (正常模式) - 波形类型、频率、幅度、偏置控制
 *      +------------------------------------------+
 *      | 波形名称        频率显示    电池图标      |
 *      |------------------------------------------|
 *      |                                          |
 *      |          波形预览区域                     |
 *      |          Waveform preview area           |
 *      |                                          |
 *      |------------------------------------------|
 *      | [波形]  [频率]  [幅度]  [偏置]  [更多]   |
 *      +------------------------------------------+
 *
 *   2. SWEEP (扫频模式) - 起始/终止频率、步进、速度
 *   3. MODULATION (调制模式) - AM 参数设置
 *   4. SYSTEM (系统设置) - 关于、校准、亮度、关机
 *
 * ============================================================
 * LVGL 基本概念 | LVGL Basic Concepts
 * ============================================================
 *
 * - lv_obj_t: LVGL 对象 (类似面向对象中的基类)
 *   lv_obj_t: LVGL object (like base class in OOP)
 *
 * - 屏幕 Screen: 最顶层的容器对象
 *   Screen: top-level container object
 *
 * - 控件 Widgets: 按钮、滑块、下拉框、标签等
 *   Widgets: buttons, sliders, dropdowns, labels, etc.
 *
 * - 样式 Styles: 控件的外观 (颜色、字体、边距)
 *   Styles: widget appearance (colors, fonts, margins)
 *
 * - 事件 Events: 用户交互触发 (点击、滑动、值变化)
 *   Events: triggered by user interaction (click, slide, value change)
 *
 * - 布局 Layouts: Flex (弹性盒) / Grid (网格)
 *   Layouts: Flex / Grid
 *
 * ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*
 * LVGL 头文件 | LVGL headers
 *
 * 在实际编译环境中需要正确配置 LVGL 路径。
 * In actual build environment, LVGL path needs proper configuration.
 *
 * #include "lvgl.h"
 * #include "lv_port_disp.h"
 * #include "lv_port_indev.h"
 */

#include "gui_helper.h"
#include "output_ctrl.h"
#include "dds_controller.h"
#include "waveform.h"
#include "utils.h"


/* ============================================================
 * 常量和颜色定义 | Constants and Color Definitions
 * ============================================================ */

/* 主题色 | Theme colors */
/* (在实机 LVGL 中使用 lv_color_hex() 定义) */
#define COLOR_BG           0x0A0A1A   /* 深色背景 | Dark background */
#define COLOR_PRIMARY      0x00AAFF   /* 主色调 (蓝色) | Primary (blue) */
#define COLOR_ACCENT       0x00FF88   /* 强调色 (绿色) | Accent (green) */
#define COLOR_TEXT          0xFFFFFF   /* 文字颜色 | Text color */
#define COLOR_DARK_PANEL   0x1A1A2E   /* 面板背景 | Panel background */
#define COLOR_WARNING      0xFFAA00   /* 警告色 | Warning color */

/* 屏幕分辨率 | Screen resolution */
#define SCREEN_WIDTH       320
#define SCREEN_HEIGHT      240

/* 波形名称数组 | Waveform name array */
static const char *waveform_names[] = {
    "Sine",           /* 正弦波 */
    "Square",         /* 方波 */
    "Triangle",       /* 三角波 */
    "Ramp",           /* 锯齿波 */
    "Pulse",          /* 脉冲波 */
    "Noise",          /* 噪声 */
    "Sinc",           /* 采样函数 */
    "Exp Decay",      /* 指数衰减 */
    "ECG",            /* 心电信号 */
    "Half Rect",      /* 半波整流 */
    "Full Rect",      /* 全波整流 */
    "Staircase",      /* 阶梯波 */
    "Custom"          /* 自定义 */
};


/* ============================================================
 * 屏幕对象引用 | Screen Object References
 * ============================================================ */

/*
 * 使用 void* 代替 lv_obj_t* 以便在不包含 LVGL 头文件时编译。
 * Using void* instead of lv_obj_t* so code compiles without LVGL headers.
 *
 * 在实际项目中，这些应该是 lv_obj_t* 类型。
 * In actual project, these should be lv_obj_t* type.
 */
static void *screen_normal = NULL;    /* 正常模式屏幕 */
static void *screen_sweep = NULL;     /* 扫频模式屏幕 */
static void *screen_mod = NULL;       /* 调制模式屏幕 */
static void *screen_system = NULL;    /* 系统设置屏幕 */
static void *screen_active = NULL;    /* 当前活动屏幕 */


/* ============================================================
 * 事件回调函数 | Event Callback Functions
 * ============================================================ */

/**
 * 波形选择下拉框事件回调
 * Waveform selection dropdown event callback
 *
 * 当用户从下拉框选择新的波形类型时触发。
 * Triggered when user selects a new waveform type from the dropdown.
 *
 * LVGL 事件回调函数签名 | LVGL event callback signature:
 *   void callback(lv_event_t *e)
 *
 * 通过 lv_event_get_target(e) 获取触发事件的控件。
 * Use lv_event_get_target(e) to get the triggering widget.
 *
 * @param e  LVGL 事件对象 (包含事件类型和目标控件)
 */
/*
 * 实际回调函数模板 (需取消注释 LVGL 头文件后使用):
 * Actual callback template (uncomment LVGL headers to use):
 *
 * static void waveform_dropdown_cb(lv_event_t *e)
 * {
 *     lv_obj_t *dd = lv_event_get_target(e);
 *     uint16_t sel = lv_dropdown_get_selected(dd);
 *
 *     // 选择对应的波形类型 | Select corresponding waveform type
 *     waveform_type_t type = (waveform_type_t)sel;
 *
 *     // 生成新波形数据 | Generate new waveform data
 *     uint16_t buf[WAVEFORM_POINTS];
 *     waveform_generate(type, buf, WAVEFORM_POINTS);
 *     dds_load_waveform(buf, WAVEFORM_POINTS);
 *     dds_set_waveform(type);
 *
 *     // 更新波形预览 | Update waveform preview
 *     update_waveform_preview(type);
 * }
 */
static void waveform_dropdown_cb_handler(int selected_index)
{
    waveform_type_t type = (waveform_type_t)selected_index;

    if (type >= 0 && type < WAVEFORM_CUSTOM) {
        /* 生成并加载新波形 | Generate and load new waveform */
        uint16_t buf[WAVEFORM_POINTS];
        waveform_generate(type, buf, WAVEFORM_POINTS);
        dds_load_waveform(buf, WAVEFORM_POINTS);
        dds_set_waveform(type);

        printf("[GUI] 波形切换: %s\r\n", waveform_names[type]);
    }
}

/**
 * 频率滑块事件回调
 * Frequency slider event callback
 *
 * 滑块值映射到频率范围:
 * Slider value maps to frequency range:
 *   滑块 0~1000 -> 频率 1Hz ~ 70MHz (对数映射)
 *   slider 0~1000 -> frequency 1Hz ~ 70MHz (log mapping)
 *
 * 对数映射原因 Reason for logarithmic mapping:
 *   人耳对频率的感知是对数的，同样 UI 操作在大频率范围时
 *   使用对数映射更直观。
 *
 *   Human perception of frequency is logarithmic, so logarithmic
 *   mapping is more intuitive for UI operations across wide ranges.
 *
 * @param slider_value  滑块当前值 (0~1000)
 */
static void frequency_slider_cb_handler(int slider_value)
{
    /*
     * 对数映射公式 Logarithmic mapping formula:
     *   freq = 10^(slider_val / slider_max * log10(fmax/fmin)) * fmin
     *
     * 设 fmin = 1 Hz, fmax = 70,000,000 Hz:
     *   log10(70e6/1) = 7.845
     *   freq = 10^(val/1000 * 7.845)
     */
    double fmin = 1.0;
    double fmax = 70000000.0;
    double log_range = 7.845;  /* log10(70e6) */
    double freq = pow(10.0, (double)slider_value / 1000.0 * log_range) * fmin;

    dds_set_frequency(freq);

    /* TODO: 更新频率标签显示 | Update frequency label display */
    char freq_str[32];
    utils_format_frequency(freq, freq_str, sizeof(freq_str));
    printf("[GUI] 频率: %s\r\n", freq_str);
}

/**
 * 幅度滑块事件回调
 * Amplitude slider event callback
 *
 * 滑块 0~100 映射到幅度 0~10Vpp
 *
 * @param slider_value  滑块值 (0~100)
 */
static void amplitude_slider_cb_handler(int slider_value)
{
    double vpp = (double)slider_value / 100.0 * 10.0;
    output_ctrl_set_amplitude(vpp);

    printf("[GUI] 幅度: %.2f Vpp\r\n", vpp);
}

/**
 * 偏置滑块事件回调
 * Offset slider event callback
 *
 * 滑块 0~100 映射到偏置 -5V~+5V
 *
 * @param slider_value  滑块值 (0~100, 50=0V)
 */
static void offset_slider_cb_handler(int slider_value)
{
    double offset = ((double)slider_value / 100.0 - 0.5) * 10.0;  /* -5V ~ +5V */
    output_ctrl_set_offset(offset);

    printf("[GUI] 偏置: %.2f V\r\n", offset);
}


/* ============================================================
 * 屏幕创建函数 | Screen Creation Functions
 * ============================================================ */

/**
 * 创建波形选择下拉框辅助函数
 * Create waveform selection dropdown helper function
 *
 * 创建一个 LVGL 下拉框控件，列出所有支持的波形类型。
 * Creates an LVGL dropdown widget listing all supported waveform types.
 *
 * @return 下拉框对象指针 (void* 代替 lv_obj_t*)
 */
void *gui_create_waveform_dropdown(void)
{
    /*
     * 实际 LVGL 实现模板 Actual LVGL implementation template:
     *
     *   lv_obj_t *dd = lv_dropdown_create(parent);
     *   lv_dropdown_set_options(dd,
     *       "Sine\n"
     *       "Square\n"
     *       "Triangle\n"
     *       "Ramp\n"
     *       "Pulse\n"
     *       "Noise\n"
     *       "Sinc\n"
     *       "Exp Decay\n"
     *       "ECG\n"
     *       "Half Rect\n"
     *       "Full Rect\n"
     *       "Staircase\n"
     *       "Custom"
     *   );
     *
     *   // 注册值变化事件回调 | Register value-change event callback
     *   lv_obj_add_event_cb(dd, waveform_dropdown_cb, LV_EVENT_VALUE_CHANGED, NULL);
     *
     *   return dd;
     */
    printf("[GUI] 波形下拉框创建 (模拟)\r\n");
    return NULL;
}

/**
 * 创建参数滑块辅助函数
 * Create parameter slider helper function
 *
 * 创建带标签和数值显示的滑块控件。
 * Creates a slider widget with label and value display.
 *
 * @param label_text  滑块标签文字 (如 "Frequency", "Amplitude")
 * @param min_val     最小值
 * @param max_val     最大值
 * @param init_val    初始值
 * @param callback    值变化回调函数
 * @return 滑块对象指针
 */
void *gui_create_slider(const char *label_text, int min_val, int max_val,
                         int init_val, void (*callback)(int))
{
    /*
     * 实际 LVGL 实现模板 Actual LVGL implementation template:
     *
     *   // 创建容器 | Create container
     *   lv_obj_t *cont = lv_obj_create(parent);
     *   lv_obj_set_size(cont, 300, 40);
     *
     *   // 创建标签 | Create label
     *   lv_obj_t *label = lv_label_create(cont);
     *   lv_label_set_text(label, label_text);
     *   lv_obj_align(label, LV_ALIGN_LEFT_MID, 5, 0);
     *
     *   // 创建滑块 | Create slider
     *   lv_obj_t *slider = lv_slider_create(cont);
     *   lv_slider_set_range(slider, min_val, max_val);
     *   lv_slider_set_value(slider, init_val, LV_ANIM_OFF);
     *   lv_obj_align(slider, LV_ALIGN_RIGHT_MID, -5, 0);
     *   lv_obj_set_width(slider, 180);
     *
     *   // 注册事件 | Register event
     *   lv_obj_add_event_cb(slider, callback, LV_EVENT_VALUE_CHANGED, NULL);
     *
     *   return slider;
     */
    (void)label_text; (void)min_val; (void)max_val;
    (void)init_val; (void)callback;
    printf("[GUI] 滑块创建: %s (模拟)\r\n", label_text);
    return NULL;
}

/**
 * 创建正常模式主屏幕
 * Create normal mode main screen
 *
 * 布局 Layout:
 *   - 顶部栏: 波形名称 + 频率 + 电池
 *   - 中间区: 波形预览图
 *   - 底部栏: 参数滑块 + 功能按钮
 */
void gui_create_screen_normal(void)
{
    /*
     * 实际 LVGL 实现 | Actual LVGL implementation:
     *
     *   screen_normal = lv_obj_create(NULL);  // 创建空屏幕
     *
     *   // --- 顶部信息栏 | Top info bar ---
     *   lv_obj_t *top_bar = lv_obj_create(screen_normal);
     *   lv_obj_set_size(top_bar, 320, 30);
     *   lv_obj_align(top_bar, LV_ALIGN_TOP_MID, 0, 0);
     *
     *   lv_obj_t *lbl_wave = lv_label_create(top_bar);
     *   lv_label_set_text(lbl_wave, "Sine");
     *   lv_obj_align(lbl_wave, LV_ALIGN_LEFT_MID, 5, 0);
     *
     *   lv_obj_t *lbl_freq = lv_label_create(top_bar);
     *   lv_label_set_text(lbl_freq, "1.000 kHz");
     *   lv_obj_align(lbl_freq, LV_ALIGN_CENTER, 0, 0);
     *
     *   lv_obj_t *lbl_batt = lv_label_create(top_bar);
     *   lv_label_set_text(lbl_batt, LV_SYMBOL_BATTERY_FULL);
     *   lv_obj_align(lbl_batt, LV_ALIGN_RIGHT_MID, -5, 0);
     *
     *   // --- 波形预览区域 | Waveform preview area ---
     *   lv_obj_t *preview = lv_obj_create(screen_normal);
     *   lv_obj_set_size(preview, 310, 120);
     *   lv_obj_align(preview, LV_ALIGN_CENTER, 0, -10);
     *   // 使用 lv_canvas 绘制波形预览
     *
     *   // --- 底部控制栏 | Bottom control bar ---
     *   lv_obj_t *bottom_bar = lv_obj_create(screen_normal);
     *   lv_obj_set_size(bottom_bar, 320, 60);
     *   lv_obj_align(bottom_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
     *
     *   // 波形选择下拉框 | Waveform dropdown
     *   lv_obj_t *dd_wave = gui_create_waveform_dropdown();
     *
     *   // 参数滑块 | Parameter sliders
     *   gui_create_slider("Freq", 0, 1000, 300, frequency_slider_cb);
     *   gui_create_slider("Amp",  0, 100,  10,  amplitude_slider_cb);
     *   gui_create_slider("Off",  0, 100,  50,  offset_slider_cb);
     *
     *   // 输出开关按钮 | Output toggle button
     *   lv_obj_t *btn_output = lv_btn_create(bottom_bar);
     *   lv_obj_t *lbl_output = lv_label_create(btn_output);
     *   lv_label_set_text(lbl_output, "OUTPUT");
     */
    printf("[GUI] 正常模式屏幕创建 (模拟)\r\n");
}

/**
 * 创建扫频模式屏幕
 * Create sweep mode screen
 */
void gui_create_screen_sweep(void)
{
    /*
     * 扫频界面包含:
     *   - 起始频率滑块
     *   - 终止频率滑块
     *   - 步进频率输入
     *   - 停留时间输入
     *   - 启动/停止按钮
     *
     * Sweep screen contains:
     *   - Start frequency slider
     *   - Stop frequency slider
     *   - Step frequency input
     *   - Dwell time input
     *   - Start/Stop button
     */
    printf("[GUI] 扫频模式屏幕创建 (模拟)\r\n");
}

/**
 * 创建调制模式屏幕
 * Create modulation mode screen
 */
void gui_create_screen_mod(void)
{
    printf("[GUI] 调制模式屏幕创建 (模拟)\r\n");
}

/**
 * 创建系统设置屏幕
 * Create system settings screen
 */
void gui_create_screen_system(void)
{
    printf("[GUI] 系统设置屏幕创建 (模拟)\r\n");
}


/* ============================================================
 * 主 GUI 创建入口 | Main GUI Creation Entry
 * ============================================================ */

/**
 * 创建主界面 (入口函数)
 * Create main UI (entry function)
 *
 * 在 main.c 的初始化阶段调用此函数来构建整个 GUI。
 * Called during main.c initialization to build the entire GUI.
 */
void gui_create_main_screen(void)
{
    /* 创建所有屏幕 | Create all screens */
    gui_create_screen_normal();
    gui_create_screen_sweep();
    gui_create_screen_mod();
    gui_create_screen_system();

    /* 加载正常模式屏幕为默认 | Load normal mode as default */
    /*
     * 实际 LVGL 调用 Actual LVGL call:
     *   lv_scr_load(screen_normal);
     */
    screen_active = screen_normal;

    printf("[GUI] 主界面创建完成\r\n");
}


/* ============================================================
 * 显示更新函数 | Display Update Functions
 * ============================================================ */

/**
 * 更新参数显示 (频率、幅度、偏置)
 * Update parameter display (frequency, amplitude, offset)
 *
 * @param freq  当前频率 (Hz)
 * @param vpp   当前幅度 (Vpp)
 * @param offset 当前偏置 (V)
 */
void gui_update_params(double freq, double vpp, double offset)
{
    char freq_str[32], amp_str[16], off_str[16];

    utils_format_frequency(freq, freq_str, sizeof(freq_str));
    snprintf(amp_str, sizeof(amp_str), "%.2f Vpp", vpp);
    snprintf(off_str, sizeof(off_str), "%+.2f V", offset);

    /*
     * TODO: 更新 LVGL 标签文字
     * TODO: Update LVGL label text
     *
     *   lv_label_set_text(lbl_freq, freq_str);
     *   lv_label_set_text(lbl_amp, amp_str);
     *   lv_label_set_text(lbl_offset, off_str);
     */
    (void)freq_str; (void)amp_str; (void)off_str;
}

/**
 * 更新电池图标
 * Update battery icon
 *
 * 根据电量百分比选择不同的电池图标。
 * Selects different battery icons based on charge percentage.
 *
 * LVGL 内置符号 LVGL built-in symbols:
 *   LV_SYMBOL_BATTERY_FULL   (满电)
 *   LV_SYMBOL_BATTERY_3      (75%)
 *   LV_SYMBOL_BATTERY_2      (50%)
 *   LV_SYMBOL_BATTERY_1      (25%)
 *   LV_SYMBOL_BATTERY_EMPTY  (空)
 *
 * @param percent  电池电量 (0~100)
 */
void gui_update_battery(int percent)
{
    const char *icon;
    if (percent >= 87)       icon = "BAT_FULL";
    else if (percent >= 62)  icon = "BAT_3";
    else if (percent >= 37)  icon = "BAT_2";
    else if (percent >= 12)  icon = "BAT_1";
    else                     icon = "BAT_EMPTY";

    (void)icon;
    /* TODO: lv_label_set_text(lbl_batt, icon); */
}

/**
 * 切换到指定屏幕
 * Switch to specified screen
 *
 * @param screen_id  屏幕编号 (0=Normal, 1=Sweep, 2=Mod, 3=System)
 */
void gui_switch_screen(int screen_id)
{
    switch (screen_id) {
        case 0:
            screen_active = screen_normal;
            /* lv_scr_load_anim(screen_normal, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false); */
            break;
        case 1:
            screen_active = screen_sweep;
            break;
        case 2:
            screen_active = screen_mod;
            break;
        case 3:
            screen_active = screen_system;
            break;
        default:
            return;
    }
    printf("[GUI] 屏幕切换: %d\r\n", screen_id);
}
