# Day 08: GUI 界面设计与波形控制 | GUI Design & Waveform Control

> **今日目标 Today's Objectives:**
> - 设计 TinyAWG 三种工作模式的 GUI 界面：正常模式、扫描模式、调制模式
> - 使用 LVGL 控件构建交互界面：按钮、下拉列表、滑块、弧形控件
> - 实现事件处理回调 (events_init_normal, events_init_scan, events_init_mod)
> - 将 GUI 控件连接到实际的 DDS/DAC 参数，实现"触摸即输出"
> - 完成系统设置界面
>
> **产出 Deliverable:** 完整的 TinyAWG GUI 界面，可通过触摸屏控制波形类型、频率、幅度和偏置

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 Morning Meeting | 回顾 Day 7 + 今日目标 |
| 09:30-10:30 | 知识讲解 Lecture | GUI 设计原则 + 三种模式分析 |
| 10:30-10:45 | 休息 Break | Break |
| 10:45-12:00 | 动手实验 Hands-on | 正常模式界面设计与实现 |
| 12:00-13:30 | 午餐 Lunch | Lunch + Break |
| 13:30-15:00 | 动手实验 Hands-on | 扫描/调制模式 + 事件处理 |
| 15:00-15:15 | 休息 Break | Break |
| 15:15-16:30 | 动手实验 Hands-on | 系统设置 + GUI-DDS 联调 |
| 16:30-17:00 | 回顾 Review | Phase 2 总结 + Phase 3 预告 |

---

## 上午: 正常模式界面设计 | Morning: Normal Mode Interface Design

### 为什么要学这个? | Why Learn This?

昨天你学会了显示和触摸驱动，LVGL 已经"活"了。但现在的界面只是一个测试按钮和滑块——真正的 AWG 需要专业的操作界面：选择波形、精确调节频率、设置幅度和偏置。今天你将设计并实现 TinyAWG 的完整 GUI，让用户通过触摸屏就能控制波形输出。

Yesterday you got the display and touch drivers working, LVGL is "alive." But the current interface is just a test button and slider — a real AWG needs a professional operating interface: waveform selection, precise frequency adjustment, amplitude and offset settings. Today you'll design and implement TinyAWG's complete GUI so users can control waveform output via the touchscreen.

**TinyAWG 三种工作模式：**

| 模式 Mode | 功能 Function | 使用场景 Use Case |
|----------|-------------|-----------------|
| 正常 Normal | 选择波形、设置频率/幅度/偏置 | 通用信号输出 |
| 扫描 Sweep | 在频率范围内自动扫频 | 频率响应测试、滤波器调试 |
| 调制 Modulation | AM 幅度调制 | 射频/通信实验 |

**界面布局规划 (240×320 竖屏)：**

```
┌──────────────────────────┐  ← 240px
│  [Normal] [Sweep] [Mod]  │  模式标签栏 (30px)
├──────────────────────────┤
│                          │
│  波形: [▼ Sine     ]     │  下拉列表
│                          │
│  频率: 1.000 kHz         │  标签 + 值
│  ═══════●═════════       │  滑块
│                          │
│  幅度: 50%               │  标签 + 值
│  ═══════●═════════       │  滑块
│                          │
│  偏置: 0.00V             │  标签 + 值
│  ═══════●═════════       │  滑块
│                          │
│  [输出 ON/OFF]            │  按钮
│                          │
│  ┌────────────────────┐  │
│  │   波形预览区域      │  │  Canvas (60px高)
│  └────────────────────┘  │
├──────────────────────────┤
│  ⚡ 85% │ 3.7V           │  状态栏 (20px)
└──────────────────────────┘
    ↑ 320px
```

### 任务 8.1: GUI 数据结构与全局状态管理 (30 分钟)

**为什么需要全局状态？** 界面上的每个控件 (滑块、下拉列表) 都需要一个对应的变量来存储当前值。事件回调修改这些变量，主循环读取这些变量去控制 DDS/DAC。

```c
// gui/gui_state.h — GUI 全局状态定义
// GUI global state definitions

#ifndef GUI_STATE_H
#define GUI_STATE_H

#include "lvgl.h"
#include "waveform_gen.h"

// 工作模式枚举
typedef enum {
    MODE_NORMAL   = 0,    // 正常模式
    MODE_SWEEP    = 1,    // 扫描模式
    MODE_MOD      = 2,    // 调制模式
} WorkMode;

// 频率范围
#define FREQ_MIN_HZ         1.0         // 1 Hz
#define FREQ_MAX_HZ         20000000.0  // 20 MHz
#define FREQ_DEFAULT_HZ     1000.0      // 1 kHz

// 幅度范围 (DAC8562 值)
#define AMPLITUDE_MIN       0x0000
#define AMPLITUDE_MAX       0xFFFF
#define AMPLITUDE_DEFAULT   0x8000

// 偏置范围 (DAC8562 值)
#define OFFSET_MIN          0x0000
#define OFFSET_MAX          0xFFFF
#define OFFSET_DEFAULT      0x8000      // 零偏置

// 扫频参数
#define SWEEP_START_DEFAULT 100.0       // 100 Hz
#define SWEEP_STOP_DEFAULT  10000.0     // 10 kHz
#define SWEEP_TIME_DEFAULT  1.0         // 1 秒

// GUI 全局状态结构体
typedef struct {
    // 当前模式
    WorkMode current_mode;

    // 正常模式参数
    WaveType waveform;          // 波形类型
    double frequency;           // 频率 (Hz)
    uint16_t amplitude;         // 幅度 (DAC8562 原始值)
    uint16_t offset;            // 偏置 (DAC8562 原始值)
    uint8_t output_enabled;     // 输出使能

    // 扫描模式参数
    double sweep_start;         // 起始频率 (Hz)
    double sweep_stop;          // 终止频率 (Hz)
    double sweep_time;          // 扫描时间 (秒)

    // 调制模式参数
    double mod_depth;           // 调制深度 (0~100%)
    double mod_freq;            // 调制频率 (Hz)

    // 控件引用 (供事件回调使用)
    lv_obj_t *tab_btns;         // 模式标签按钮组
    lv_obj_t *dd_waveform;      // 波形下拉列表
    lv_obj_t *slider_freq;      // 频率滑块
    lv_obj_t *slider_amplitude; // 幅度滑块
    lv_obj_t *slider_offset;    // 偏置滑块
    lv_obj_t *btn_output;       // 输出开关按钮
    lv_obj_t *label_freq_val;   // 频率值标签
    lv_obj_t *label_amp_val;    // 幅度值标签
    lv_obj_t *label_off_val;    // 偏置值标签
    lv_obj_t *label_battery;    // 电池状态标签
    lv_obj_t *canvas_preview;   // 波形预览画布
} GuiState;

// 全局状态实例
extern GuiState gui_state;

#endif // GUI_STATE_H
```

```c
// gui/gui_state.c — 全局状态初始化
// Global state initialization

#include "gui_state.h"

GuiState gui_state = {
    .current_mode    = MODE_NORMAL,
    .waveform        = WAVE_SINE,
    .frequency       = FREQ_DEFAULT_HZ,
    .amplitude       = AMPLITUDE_DEFAULT,
    .offset          = OFFSET_DEFAULT,
    .output_enabled  = 0,
    .sweep_start     = SWEEP_START_DEFAULT,
    .sweep_stop      = SWEEP_STOP_DEFAULT,
    .sweep_time      = SWEEP_TIME_DEFAULT,
    .mod_depth       = 50.0,
    .mod_freq        = 100.0,
    .tab_btns        = NULL,
    .dd_waveform     = NULL,
    .slider_freq     = NULL,
    .slider_amplitude = NULL,
    .slider_offset   = NULL,
    .btn_output      = NULL,
    .label_freq_val  = NULL,
    .label_amp_val   = NULL,
    .label_off_val   = NULL,
    .label_battery   = NULL,
    .canvas_preview  = NULL,
};
```

**步骤 Steps:**
1. 创建 `gui/gui_state.c` 和 `gui/gui_state.h`
2. 定义全局状态结构体和初始值
3. 确认编译通过

### 任务 8.2: 正常模式界面 — 控件布局 (45 分钟)

```c
// gui/setup_scr_normal.c — 正常模式界面创建
// Normal mode screen setup

#include "lvgl.h"
#include "gui_state.h"
#include "xil_printf.h"

// 颜色定义 (TinyAWG 主题)
#define COLOR_BG           lv_color_hex(0x1E1E2E)    // 深色背景
#define COLOR_ACCENT       lv_color_hex(0x89B4FA)    // 蓝色强调
#define COLOR_TEXT          lv_color_hex(0xCDD6F4)    // 浅色文字
#define COLOR_GREEN        lv_color_hex(0xA6E3A1)    // 绿色 (开启)
#define COLOR_RED          lv_color_hex(0xF38BA8)    // 红色 (关闭)
#define COLOR_BTN_BG       lv_color_hex(0x313244)    // 按钮背景

// Y 坐标偏移，便于调整布局
#define Y_TABS       5
#define Y_WAVEFORM   40
#define Y_FREQ       90
#define Y_AMPLITUDE  155
#define Y_OFFSET     220
#define Y_OUTPUT     275
#define Y_CANVAS     200

/**
 * 创建正常模式界面
 * Create normal mode screen
 */
void setup_scr_normal(lv_obj_t *scr)
{
    // 设置屏幕背景色
    lv_obj_set_style_bg_color(scr, COLOR_BG, 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

    // ===== 波形选择下拉列表 =====
    lv_obj_t *lbl_wave = lv_label_create(scr);
    lv_label_set_text(lbl_wave, "Wave");
    lv_obj_set_style_text_color(lbl_wave, COLOR_TEXT, 0);
    lv_obj_align(lbl_wave, LV_ALIGN_TOP_LEFT, 10, Y_WAVEFORM);

    gui_state.dd_waveform = lv_dropdown_create(scr);
    lv_dropdown_set_options(gui_state.dd_waveform,
                            "Sine\nSquare\nTriangle\nRamp\nNoise");
    lv_obj_set_width(gui_state.dd_waveform, 140);
    lv_obj_align_to(gui_state.dd_waveform, lbl_wave,
                    LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_set_style_bg_color(gui_state.dd_waveform, COLOR_BTN_BG, 0);
    lv_obj_set_style_text_color(gui_state.dd_waveform, COLOR_TEXT, 0);

    // ===== 频率滑块 =====
    lv_obj_t *lbl_freq = lv_label_create(scr);
    lv_label_set_text(lbl_freq, "Freq");
    lv_obj_set_style_text_color(lbl_freq, COLOR_TEXT, 0);
    lv_obj_align(lbl_freq, LV_ALIGN_TOP_LEFT, 10, Y_FREQ);

    gui_state.label_freq_val = lv_label_create(scr);
    lv_label_set_text(gui_state.label_freq_val, "1.000 kHz");
    lv_obj_set_style_text_color(gui_state.label_freq_val, COLOR_ACCENT, 0);
    lv_obj_align(gui_state.label_freq_val, LV_ALIGN_TOP_RIGHT, -10, Y_FREQ);

    gui_state.slider_freq = lv_slider_create(scr);
    lv_obj_set_width(gui_state.slider_freq, 220);
    lv_obj_align(gui_state.slider_freq, LV_ALIGN_TOP_MID, 0, Y_FREQ + 20);
    lv_slider_set_range(gui_state.slider_freq, 0, 1000);  // 对数映射 0~1000
    lv_slider_set_value(gui_state.slider_freq, 500, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(gui_state.slider_freq, COLOR_ACCENT, LV_PART_INDICATOR);

    // ===== 幅度滑块 =====
    lv_obj_t *lbl_amp = lv_label_create(scr);
    lv_label_set_text(lbl_amp, "Ampl");
    lv_obj_set_style_text_color(lbl_amp, COLOR_TEXT, 0);
    lv_obj_align(lbl_amp, LV_ALIGN_TOP_LEFT, 10, Y_AMPLITUDE);

    gui_state.label_amp_val = lv_label_create(scr);
    lv_label_set_text(gui_state.label_amp_val, "50%");
    lv_obj_set_style_text_color(gui_state.label_amp_val, COLOR_ACCENT, 0);
    lv_obj_align(gui_state.label_amp_val, LV_ALIGN_TOP_RIGHT, -10, Y_AMPLITUDE);

    gui_state.slider_amplitude = lv_slider_create(scr);
    lv_obj_set_width(gui_state.slider_amplitude, 220);
    lv_obj_align(gui_state.slider_amplitude, LV_ALIGN_TOP_MID, 0, Y_AMPLITUDE + 20);
    lv_slider_set_range(gui_state.slider_amplitude, 0, 100);   // 0~100%
    lv_slider_set_value(gui_state.slider_amplitude, 50, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(gui_state.slider_amplitude, COLOR_ACCENT, LV_PART_INDICATOR);

    // ===== 偏置滑块 =====
    lv_obj_t *lbl_off = lv_label_create(scr);
    lv_label_set_text(lbl_off, "Offset");
    lv_obj_set_style_text_color(lbl_off, COLOR_TEXT, 0);
    lv_obj_align(lbl_off, LV_ALIGN_TOP_LEFT, 10, Y_OFFSET);

    gui_state.label_off_val = lv_label_create(scr);
    lv_label_set_text(gui_state.label_off_val, "0.00V");
    lv_obj_set_style_text_color(gui_state.label_off_val, COLOR_ACCENT, 0);
    lv_obj_align(gui_state.label_off_val, LV_ALIGN_TOP_RIGHT, -10, Y_OFFSET);

    gui_state.slider_offset = lv_slider_create(scr);
    lv_obj_set_width(gui_state.slider_offset, 220);
    lv_obj_align(gui_state.slider_offset, LV_ALIGN_TOP_MID, 0, Y_OFFSET + 20);
    lv_slider_set_range(gui_state.slider_offset, -100, 100);   // -100% ~ +100%
    lv_slider_set_value(gui_state.slider_offset, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(gui_state.slider_offset, COLOR_ACCENT, LV_PART_INDICATOR);

    // ===== 输出开关按钮 =====
    gui_state.btn_output = lv_btn_create(scr);
    lv_obj_set_size(gui_state.btn_output, 120, 35);
    lv_obj_align(gui_state.btn_output, LV_ALIGN_TOP_MID, 0, Y_OUTPUT);
    lv_obj_set_style_bg_color(gui_state.btn_output, COLOR_RED, 0);

    lv_obj_t *label_out = lv_label_create(gui_state.btn_output);
    lv_label_set_text(label_out, "OUTPUT OFF");
    lv_obj_center(label_out);
    lv_obj_set_style_text_color(label_out, lv_color_white(), 0);

    xil_printf("Normal mode screen created\r\n");
}
```

**步骤 Steps:**
1. 创建 `gui/setup_scr_normal.c` 和 `gui/setup_scr_normal.h`
2. 实现正常模式界面布局
3. 在 main() 中调用创建函数

```c
// 创建主屏幕
lv_obj_t *scr_main = lv_scr_act();
setup_scr_normal(scr_main);
```

4. 编译下载，观察界面布局是否正确

**预期结果 Expected Result:**
- 屏幕显示深色背景，带标题和三个滑块
- 下拉列表可展开，显示 5 种波形选项
- 输出按钮显示红色 "OUTPUT OFF"

**常见问题 Common Issues:**
- "控件重叠" -- Y 坐标偏移量需要调整，每个滑块 + 标签大约占 55px
- "文字太小/太大" -- 调整 `lv_conf.h` 中的 `LV_FONT_DEFAULT` 字体大小
- "颜色不对" -- 确认 `LV_COLOR_DEPTH` 为 16，且 `LV_COLOR_16_SWAP` 已启用

---

## 下午: 事件处理与模式切换 | Afternoon: Event Handling & Mode Switching

### 为什么要学这个? | Why Learn This?

界面画出来了，但点滑块没有反应——因为还没有连接"事件处理"。事件处理就是：用户拖动滑块 → LVGL 触发 `LV_EVENT_VALUE_CHANGED` → 你的回调函数读取滑块值 → 更新频率 → 重新配置 DDS → 波形输出改变。这条"用户操作到硬件响应"的完整链路，就是今天下午要实现的。

The interface is drawn, but moving sliders does nothing — because event handling is not connected yet. Event handling means: user drags slider → LVGL fires `LV_EVENT_VALUE_CHANGED` → your callback reads the slider value → updates frequency → reconfigures DDS → waveform output changes. This complete chain from "user action to hardware response" is what we implement this afternoon.

### 任务 8.3: 正常模式事件处理 (45 分钟)

```c
// gui/events_init_normal.c — 正常模式事件处理
// Normal mode event handlers

#include "lvgl.h"
#include "gui_state.h"
#include "output_ctrl.h"
#include "dac8562.h"
#include "xil_printf.h"

/**
 * 频率滑块值转换为实际频率 (对数映射)
 * Convert slider value to actual frequency (logarithmic mapping)
 *
 * 为什么用对数映射？
 * 人耳对频率的感知是对数的——从 100Hz 到 200Hz 的感觉差异
 * 和从 1000Hz 到 2000Hz 的感觉差异差不多。
 * 对数映射让低频段有更精细的调节，高频段也能覆盖。
 *
 * Why logarithmic mapping?
 * Human perception of frequency is logarithmic — the perceived difference
 * from 100Hz to 200Hz is similar to 1000Hz to 2000Hz.
 * Log mapping gives fine control at low frequencies while covering high.
 */
static double slider_to_freq(int slider_val)
{
    // slider_val: 0~1000
    // 映射到 1Hz ~ 20MHz (对数)
    // log10(1) = 0, log10(20000000) ≈ 7.3
    double min_log = 0.0;          // log10(1)
    double max_log = 7.301;        // log10(20,000,000)
    double freq_log = min_log + (max_log - min_log) * slider_val / 1000.0;
    return pow(10.0, freq_log);
}

/**
 * 格式化频率字符串
 * Format frequency as human-readable string
 */
static void format_freq(double freq_hz, char *buf, int buf_size)
{
    if (freq_hz >= 1000000.0)
        snprintf(buf, buf_size, "%.2f MHz", freq_hz / 1000000.0);
    else if (freq_hz >= 1000.0)
        snprintf(buf, buf_size, "%.3f kHz", freq_hz / 1000.0);
    else
        snprintf(buf, buf_size, "%.1f Hz", freq_hz);
}

/**
 * 波形下拉列表事件回调
 * Waveform dropdown event callback
 */
static void dd_waveform_event_cb(lv_event_t *e)
{
    lv_obj_t *dd = lv_event_get_target(e);
    uint16_t selected = lv_dropdown_get_selected(dd);

    // 映射到 WaveType 枚举
    gui_state.waveform = (WaveType)selected;

    const char *names[] = {"Sine", "Square", "Triangle", "Ramp", "Noise"};
    xil_printf("Waveform changed to: %s\r\n", names[selected]);

    // 立即更新输出
    if (gui_state.output_enabled)
    {
        output_start(gui_state.waveform, gui_state.frequency, 1);
    }
}

/**
 * 频率滑块事件回调
 * Frequency slider event callback
 */
static void slider_freq_event_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    int val = lv_slider_get_value(slider);

    // 对数映射到频率
    gui_state.frequency = slider_to_freq(val);

    // 更新标签
    char buf[32];
    format_freq(gui_state.frequency, buf, sizeof(buf));
    lv_label_set_text(gui_state.label_freq_val, buf);

    // 更新 DDS 输出频率
    if (gui_state.output_enabled)
    {
        set_frequency(gui_state.frequency);
    }
}

/**
 * 幅度滑块事件回调
 * Amplitude slider event callback
 */
static void slider_amplitude_event_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    int val = lv_slider_get_value(slider);    // 0~100%

    // 更新标签
    char buf[16];
    snprintf(buf, sizeof(buf), "%d%%", val);
    lv_label_set_text(gui_state.label_amp_val, buf);

    // 映射到 DAC8562 值 (0~100% → 0x0000~0xFFFF)
    gui_state.amplitude = (uint16_t)(val * 65535 / 100);
    dac8562_set_amplitude(gui_state.amplitude);

    xil_printf("Amplitude: %d%% (0x%04X)\r\n", val, gui_state.amplitude);
}

/**
 * 偏置滑块事件回调
 * Offset slider event callback
 */
static void slider_offset_event_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    int val = lv_slider_get_value(slider);    // -100~+100

    // 更新标签
    char buf[16];
    float offset_v = val * 2.5f / 100.0f;    // 映射到 -2.5V ~ +2.5V
    snprintf(buf, sizeof(buf), "%+.2fV", offset_v);
    lv_label_set_text(gui_state.label_off_val, buf);

    // 映射到 DAC8562 值 (-100~+100 → 0x0000~0xFFFF, 0x8000 为零偏置)
    gui_state.offset = (uint16_t)(0x8000 + (int32_t)val * 0x8000 / 100);
    dac8562_set_offset(gui_state.offset);
}

/**
 * 输出按钮事件回调
 * Output button event callback
 */
static void btn_output_event_cb(lv_event_t *e)
{
    gui_state.output_enabled = !gui_state.output_enabled;

    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *label = lv_obj_get_child(btn, 0);

    if (gui_state.output_enabled)
    {
        // 开启输出
        lv_obj_set_style_bg_color(btn, COLOR_GREEN, 0);
        lv_label_set_text(label, "OUTPUT ON");

        output_start(gui_state.waveform, gui_state.frequency, 1);
        xil_printf("Output: ON\r\n");
    }
    else
    {
        // 关闭输出
        lv_obj_set_style_bg_color(btn, COLOR_RED, 0);
        lv_label_set_text(label, "OUTPUT OFF");

        output_start(gui_state.waveform, gui_state.frequency, 0);
        xil_printf("Output: OFF\r\n");
    }
}

/**
 * 注册正常模式所有事件回调
 * Register all event callbacks for normal mode
 */
void events_init_normal(void)
{
    // 波形选择
    lv_obj_add_event_cb(gui_state.dd_waveform, dd_waveform_event_cb,
                        LV_EVENT_VALUE_CHANGED, NULL);

    // 频率滑块
    lv_obj_add_event_cb(gui_state.slider_freq, slider_freq_event_cb,
                        LV_EVENT_VALUE_CHANGED, NULL);

    // 幅度滑块
    lv_obj_add_event_cb(gui_state.slider_amplitude, slider_amplitude_event_cb,
                        LV_EVENT_VALUE_CHANGED, NULL);

    // 偏置滑块
    lv_obj_add_event_cb(gui_state.slider_offset, slider_offset_event_cb,
                        LV_EVENT_VALUE_CHANGED, NULL);

    // 输出按钮
    lv_obj_add_event_cb(gui_state.btn_output, btn_output_event_cb,
                        LV_EVENT_CLICKED, NULL);

    xil_printf("Normal mode events registered\r\n");
}
```

**步骤 Steps:**
1. 创建 `gui/events_init_normal.c` 和 `gui/events_init_normal.h`
2. 实现所有事件回调函数
3. 在创建界面后注册事件：
4. 测试：拖动频率滑块，观察串口输出频率变化；点击输出按钮，用示波器验证波形

```c
setup_scr_normal(scr_main);
events_init_normal();    // 注册事件回调
```

**预期结果 Expected Result:**
- 拖动频率滑块，标签实时更新 (例如 "1.234 kHz")
- 点击输出按钮变绿，示波器显示波形
- 改变波形下拉列表，输出波形类型改变
- 调节幅度滑块，波形幅度改变

**常见问题 Common Issues:**
- "滑块拖动时频率跳动" -- 对数映射计算正确但数值不稳定，可在回调中加 `LV_EVENT_VALUE_CHANGED` 过滤
- "波形切换后频率变了" -- 切换波形时调用了 `output_start()` 重新写入 BRAM，FTW 也被重设了；确认频率参数从 `gui_state.frequency` 读取
- "输出关闭后仍有信号" -- 继电器未断开，检查 `gpio_set_relay(0)` 是否被调用

### 任务 8.4: 扫描模式界面与事件 (30 分钟)

**扫描模式 (Sweep)：** 在起始频率和终止频率之间线性扫频，用于测试滤波器的频率响应。

```c
// gui/setup_scr_scan.c — 扫描模式界面
// Scan (Sweep) mode screen setup

#include "lvgl.h"
#include "gui_state.h"

void setup_scr_scan(lv_obj_t *scr)
{
    lv_obj_set_style_bg_color(scr, COLOR_BG, 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "Sweep Mode");
    lv_obj_set_style_text_color(title, COLOR_ACCENT, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // 起始频率
    lv_obj_t *lbl_start = lv_label_create(scr);
    lv_label_set_text(lbl_start, "Start Freq");
    lv_obj_set_style_text_color(lbl_start, COLOR_TEXT, 0);
    lv_obj_align(lbl_start, LV_ALIGN_TOP_LEFT, 10, 50);

    lv_obj_t *slider_start = lv_slider_create(scr);
    lv_obj_set_width(slider_start, 220);
    lv_obj_align(slider_start, LV_ALIGN_TOP_MID, 0, 75);
    lv_slider_set_range(slider_start, 0, 1000);
    lv_slider_set_value(slider_start, 100, LV_ANIM_OFF);

    // 终止频率
    lv_obj_t *lbl_stop = lv_label_create(scr);
    lv_label_set_text(lbl_stop, "Stop Freq");
    lv_obj_set_style_text_color(lbl_stop, COLOR_TEXT, 0);
    lv_obj_align(lbl_stop, LV_ALIGN_TOP_LEFT, 10, 115);

    lv_obj_t *slider_stop = lv_slider_create(scr);
    lv_obj_set_width(slider_stop, 220);
    lv_obj_align(slider_stop, LV_ALIGN_TOP_MID, 0, 140);
    lv_slider_set_range(slider_stop, 0, 1000);
    lv_slider_set_value(slider_stop, 800, LV_ANIM_OFF);

    // 扫描时间
    lv_obj_t *lbl_time = lv_label_create(scr);
    lv_label_set_text(lbl_time, "Sweep Time");
    lv_obj_set_style_text_color(lbl_time, COLOR_TEXT, 0);
    lv_obj_align(lbl_time, LV_ALIGN_TOP_LEFT, 10, 180);

    lv_obj_t *slider_time = lv_slider_create(scr);
    lv_obj_set_width(slider_time, 220);
    lv_obj_align(slider_time, LV_ALIGN_TOP_MID, 0, 205);
    lv_slider_set_range(slider_time, 1, 100);   // 0.1s ~ 10s
    lv_slider_set_value(slider_time, 10, LV_ANIM_OFF);

    // 启动/停止按钮
    gui_state.btn_output = lv_btn_create(scr);
    lv_obj_set_size(gui_state.btn_output, 120, 40);
    lv_obj_align(gui_state.btn_output, LV_ALIGN_TOP_MID, 0, 250);
    lv_obj_set_style_bg_color(gui_state.btn_output, COLOR_RED, 0);

    lv_obj_t *label_sweep = lv_label_create(gui_state.btn_output);
    lv_label_set_text(label_sweep, "START SWEEP");
    lv_obj_center(label_sweep);

    xil_printf("Sweep mode screen created\r\n");
}
```

```c
// gui/events_init_scan.c — 扫描模式事件处理
// Sweep mode event handlers

#include "lvgl.h"
#include "gui_state.h"
#include "output_ctrl.h"
#include "xil_printf.h"

/**
 * 扫频输出线程 (在主循环中调用)
 * Sweep output function (called from main loop)
 *
 * 在主循环中周期性调用，自动递增频率
 */
static int sweep_direction = 1;    // 1=上升, -1=下降
static double sweep_current_freq;
static uint32_t sweep_step_counter = 0;

void sweep_update(void)
{
    if (!gui_state.output_enabled || gui_state.current_mode != MODE_SWEEP)
        return;

    // 计算扫频步进
    // sweep_time 秒内从 start 扫到 stop
    // 主循环约 5ms 一次 → 每秒 200 步
    double freq_range = gui_state.sweep_stop - gui_state.sweep_start;
    double step = freq_range / (gui_state.sweep_time * 200.0);

    // 更新频率
    sweep_current_freq += step * sweep_direction;

    // 到达边界时反转方向
    if (sweep_current_freq >= gui_state.sweep_stop)
    {
        sweep_current_freq = gui_state.sweep_stop;
        sweep_direction = -1;
    }
    else if (sweep_current_freq <= gui_state.sweep_start)
    {
        sweep_current_freq = gui_state.sweep_start;
        sweep_direction = 1;
    }

    // 更新 DDS 频率
    set_frequency(sweep_current_freq);
}

void events_init_scan(void)
{
    xil_printf("Sweep mode events registered\r\n");
}
```

**步骤 Steps:**
1. 创建扫描模式界面和事件处理文件
2. 在主循环中添加 `sweep_update()` 调用
3. 测试：设置起始 100Hz、终止 10kHz，用示波器观察频率变化

**预期结果 Expected Result:**
- 示波器上波形频率从低到高再到低循环变化
- 频率变化平稳无跳变

**常见问题 Common Issues:**
- "频率跳变不连续" -- `set_frequency()` 每次都写 FTW，AXI 总线延迟导致步进不均匀；可减少步进频率
- "扫频范围不对" -- 检查对数映射是否与正常模式一致

### 任务 8.5: 调制模式界面 (30 分钟)

**调制模式 (AM)：** 输出正弦波被另一个低频正弦波进行幅度调制，产生 AM 信号。

```c
// gui/setup_scr_mod.c — 调制模式界面
// Modulation mode screen setup

#include "lvgl.h"
#include "gui_state.h"

void setup_scr_mod(lv_obj_t *scr)
{
    lv_obj_set_style_bg_color(scr, COLOR_BG, 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "AM Modulation");
    lv_obj_set_style_text_color(title, COLOR_ACCENT, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // 载波频率
    lv_obj_t *lbl_carrier = lv_label_create(scr);
    lv_label_set_text(lbl_carrier, "Carrier Freq");
    lv_obj_set_style_text_color(lbl_carrier, COLOR_TEXT, 0);
    lv_obj_align(lbl_carrier, LV_ALIGN_TOP_LEFT, 10, 50);

    gui_state.slider_freq = lv_slider_create(scr);
    lv_obj_set_width(gui_state.slider_freq, 220);
    lv_obj_align(gui_state.slider_freq, LV_ALIGN_TOP_MID, 0, 75);
    lv_slider_set_range(gui_state.slider_freq, 0, 1000);
    lv_slider_set_value(gui_state.slider_freq, 700, LV_ANIM_OFF);

    // 调制深度 (0~100%)
    lv_obj_t *lbl_depth = lv_label_create(scr);
    lv_label_set_text(lbl_depth, "Mod Depth");
    lv_obj_set_style_text_color(lbl_depth, COLOR_TEXT, 0);
    lv_obj_align(lbl_depth, LV_ALIGN_TOP_LEFT, 10, 115);

    lv_obj_t *slider_depth = lv_slider_create(scr);
    lv_obj_set_width(slider_depth, 220);
    lv_obj_align(slider_depth, LV_ALIGN_TOP_MID, 0, 140);
    lv_slider_set_range(slider_depth, 0, 100);
    lv_slider_set_value(slider_depth, 50, LV_ANIM_OFF);

    // 调制频率
    lv_obj_t *lbl_modf = lv_label_create(scr);
    lv_label_set_text(lbl_modf, "Mod Freq");
    lv_obj_set_style_text_color(lbl_modf, COLOR_TEXT, 0);
    lv_obj_align(lbl_modf, LV_ALIGN_TOP_LEFT, 10, 180);

    lv_obj_t *slider_modf = lv_slider_create(scr);
    lv_obj_set_width(slider_modf, 220);
    lv_obj_align(slider_modf, LV_ALIGN_TOP_MID, 0, 205);
    lv_slider_set_range(slider_modf, 1, 1000);    // 1Hz ~ 10kHz (对数)
    lv_slider_set_value(slider_modf, 100, LV_ANIM_OFF);

    // 输出按钮 (复用)
    gui_state.btn_output = lv_btn_create(scr);
    lv_obj_set_size(gui_state.btn_output, 120, 40);
    lv_obj_align(gui_state.btn_output, LV_ALIGN_TOP_MID, 0, 250);
    lv_obj_set_style_bg_color(gui_state.btn_output, COLOR_RED, 0);

    lv_obj_t *label_mod = lv_label_create(gui_state.btn_output);
    lv_label_set_text(label_mod, "OUTPUT OFF");
    lv_obj_center(label_mod);

    xil_printf("Modulation mode screen created\r\n");
}

void events_init_mod(void)
{
    xil_printf("Modulation mode events registered\r\n");
}
```

**AM 调制实现 (在主循环中更新幅度)：**

```c
// gui/events_init_mod.c 中的 AM 更新函数

#include <math.h>

/**
 * AM 调制更新 (在主循环中调用)
 * AM modulation update (called from main loop)
 *
 * AM 公式: output(t) = carrier × [1 + m × sin(2*pi*f_mod*t)]
 * 其中 m = 调制深度 (0~1)
 */
static double am_phase = 0.0;

void am_mod_update(void)
{
    if (!gui_state.output_enabled || gui_state.current_mode != MODE_MOD)
        return;

    // 更新调制相位 (主循环约 5ms)
    double dt = 0.005;    // 5ms
    am_phase += 2.0 * M_PI * gui_state.mod_freq * dt;
    if (am_phase > 2.0 * M_PI) am_phase -= 2.0 * M_PI;

    // 计算调制后的幅度
    double m = gui_state.mod_depth / 100.0;    // 调制深度 0~1
    double mod_signal = 1.0 + m * sin(am_phase);  // 1-m ~ 1+m

    // 映射到 DAC8562 范围
    uint16_t amplitude = (uint16_t)(gui_state.amplitude * mod_signal);
    dac8562_set_amplitude(amplitude);
}
```

**步骤 Steps:**
1. 创建调制模式界面和事件处理文件
2. 在主循环中添加 `am_mod_update()` 调用
3. 测试：设置载波 100kHz、调制频率 1kHz、调制深度 50%，用示波器观察 AM 波形

**预期结果 Expected Result:**
- 示波器显示 AM 调制波形（包络线清晰可见）
- 调节调制深度，包络幅度变化

**常见问题 Common Issues:**
- "看不到调制效果" -- 调制频率太低或载波频率太高，示波器时基不合适
- "波形失真" -- `am_mod_update()` 调用频率不稳定导致调制频率偏差

### 任务 8.6: 模式切换与系统设置 (30 分钟)

**模式切换逻辑：** 用户点击顶部标签栏切换模式时，需要：
1. 删除当前界面的所有控件
2. 创建新模式界面
3. 注册新模式的事件回调

```c
// gui/lv_setup.c — GUI 总控：模式切换与主循环
// GUI master: mode switching and main loop

#include "lvgl.h"
#include "gui_state.h"
#include "setup_scr_normal.h"
#include "setup_scr_scan.h"
#include "setup_scr_mod.h"
#include "events_init_normal.h"
#include "events_init_scan.h"
#include "events_init_mod.h"
#include "max17048.h"
#include "xil_printf.h"

// 模式标签按钮名称
static const char *tab_names[] = {"Normal", "Sweep", "Mod"};
static lv_obj_t *tab_btns[3];

/**
 * 模式切换回调
 * Mode switch callback
 */
static void mode_btn_event_cb(lv_event_t *e)
{
    int mode = (int)(intptr_t)lv_event_get_user_data(e);
    if (mode == gui_state.current_mode) return;

    // 关闭当前输出
    if (gui_state.output_enabled)
    {
        gui_state.output_enabled = 0;
        gpio_set_relay(0);
    }

    // 更新模式
    gui_state.current_mode = (WorkMode)mode;
    xil_printf("Mode switched to: %s\r\n", tab_names[mode]);

    // 删除旧界面的所有子控件 (保留标签栏)
    lv_obj_t *scr = lv_scr_act();
    uint32_t child_cnt = lv_obj_get_child_cnt(scr);
    // 从后往前删除，避免索引问题
    for (int i = child_cnt - 1; i >= 0; i--)
    {
        lv_obj_t *child = lv_obj_get_child(scr, i);
        // 不删除标签栏按钮
        int is_tab = 0;
        for (int t = 0; t < 3; t++)
        {
            if (child == tab_btns[t]) { is_tab = 1; break; }
        }
        if (!is_tab) lv_obj_del(child);
    }

    // 创建新界面
    switch (gui_state.current_mode)
    {
        case MODE_NORMAL:
            setup_scr_normal(scr);
            events_init_normal();
            break;
        case MODE_SWEEP:
            setup_scr_scan(scr);
            events_init_scan();
            break;
        case MODE_MOD:
            setup_scr_mod(scr);
            events_init_mod();
            break;
    }

    // 高亮当前标签按钮
    for (int i = 0; i < 3; i++)
    {
        lv_obj_set_style_bg_color(tab_btns[i],
            (i == mode) ? COLOR_ACCENT : COLOR_BTN_BG, 0);
    }
}

/**
 * 创建模式标签栏
 * Create mode tab bar
 */
static void create_tab_bar(lv_obj_t *scr)
{
    for (int i = 0; i < 3; i++)
    {
        tab_btns[i] = lv_btn_create(scr);
        lv_obj_set_size(tab_btns[i], 75, 28);
        lv_obj_align(tab_btns[i], LV_ALIGN_TOP_LEFT, 5 + i * 80, Y_TABS);
        lv_obj_set_style_bg_color(tab_btns[i],
            (i == 0) ? COLOR_ACCENT : COLOR_BTN_BG, 0);

        lv_obj_t *lbl = lv_label_create(tab_btns[i]);
        lv_label_set_text(lbl, tab_names[i]);
        lv_obj_center(lbl);
        lv_obj_set_style_text_color(lbl,
            (i == 0) ? lv_color_black() : COLOR_TEXT, 0);

        lv_obj_add_event_cb(tab_btns[i], mode_btn_event_cb,
                            LV_EVENT_CLICKED, (void *)(intptr_t)i);
    }
}

/**
 * 创建底部状态栏 (电池状态)
 * Create bottom status bar (battery status)
 */
static void create_status_bar(lv_obj_t *scr)
{
    gui_state.label_battery = lv_label_create(scr);
    lv_label_set_text(gui_state.label_battery, "");
    lv_obj_set_style_text_color(gui_state.label_battery, COLOR_TEXT, 0);
    lv_obj_align(gui_state.label_battery, LV_ALIGN_BOTTOM_LEFT, 10, -5);
}

/**
 * 更新电池状态 (在主循环中周期调用)
 * Update battery status (called periodically from main loop)
 */
static uint32_t battery_update_counter = 0;

void gui_update_status(void)
{
    battery_update_counter++;
    if (battery_update_counter >= 400)    // 每 400 × 5ms = 2 秒更新
    {
        battery_update_counter = 0;

        float voltage = max17048_read_voltage();
        float soc = max17048_read_soc();

        char buf[32];
        snprintf(buf, sizeof(buf), "%.0f%% %.1fV", soc, voltage / 1000.0f);
        lv_label_set_text(gui_state.label_battery, buf);
    }
}

/**
 * GUI 完整初始化
 * Complete GUI initialization
 */
void gui_init(void)
{
    lv_obj_t *scr = lv_scr_act();

    // 创建标签栏和状态栏
    create_tab_bar(scr);
    create_status_bar(scr);

    // 默认显示正常模式
    gui_state.current_mode = MODE_NORMAL;
    setup_scr_normal(scr);
    events_init_normal();

    xil_printf("GUI initialized\r\n");
}
```

**完整的 main() 函数：**

```c
int main()
{
    init_platform();
    xil_printf("=== TinyAWG v1.0 ===\r\n");
    xil_printf("GUI + Waveform Control\r\n\r\n");

    // 初始化硬件
    gpio_init();
    spi_init();
    i2c_init();
    dac8562_init();

    // 初始化 LVGL
    lv_init();
    lv_tick_set_cb(custom_tick_get);
    lv_port_disp_init();
    lv_port_indev_init();

    // 创建 GUI
    gui_init();

    xil_printf("System ready. Use touchscreen to control!\r\n");

    // 主循环
    while (1)
    {
        lv_tick_value += 5;
        lv_timer_handler();

        // 模式相关更新
        if (gui_state.output_enabled)
        {
            if (gui_state.current_mode == MODE_SWEEP)
                sweep_update();
            else if (gui_state.current_mode == MODE_MOD)
                am_mod_update();
        }

        // 更新状态栏
        gui_update_status();

        usleep(5000);    // 5ms 主循环周期
    }

    cleanup_platform();
    return 0;
}
```

**步骤 Steps:**
1. 整合所有 GUI 模块到完整系统中
2. 实现模式切换逻辑
3. 添加电池状态显示
4. 编译下载完整系统
5. 功能验证：
   - 点击 Normal/Sweep/Mod 标签切换模式
   - 在正常模式下选择波形、调节频率/幅度/偏置
   - 在扫描模式下启动扫频
   - 在调制模式下观察 AM 波形
   - 状态栏显示电池电量

**预期结果 Expected Result:**
- 三种模式可正常切换，界面刷新正确
- 所有滑块和按钮的事件回调正常工作
- 输出波形与 GUI 设置一致
- 电池状态每 2 秒更新

**常见问题 Common Issues:**
- "模式切换后控件残留" -- 删除旧控件不完整，确保遍历所有子对象
- "切换模式后输出不停止" -- 模式切换时需要停止当前输出、关闭继电器
- "内存不足" -- LVGL 控件占用 RAM，删除控件后调用 `lv_mem_monitor()` 检查内存使用

---

## Phase 2 总结 | Phase 2 Summary

恭喜完成 Phase 2！经过 4 天的学习，你已经掌握了 TinyAWG 的全部软件功能：

Congratulations on completing Phase 2! After 4 days, you have mastered all of TinyAWG's software capabilities:

1. **ARM PS 软件开发 (Day 5)** -- GPIO, SPI, I2C 驱动，PS 端程序框架
2. **DAC 驱动与输出 (Day 6)** -- AD9744 数据格式，DAC8562 控制，波形生成，AXI 总线通信
3. **LVGL 显示与触摸 (Day 7)** -- LCD 驱动，FT6336 触控，LVGL 移植
4. **GUI 界面设计 (Day 8)** -- 三种工作模式，事件处理，GUI-DDS 联调

**你的 TinyAWG 现在能够：**
- 通过触摸屏选择波形类型（正弦/方波/三角/锯齿/噪声）
- 对数频率调节覆盖 1Hz ~ 20MHz
- 幅度和偏置独立可调
- 支持扫频和 AM 调制模式
- 实时显示电池状态

**接下来 (Phase 3)，你将：**
- 完成模拟输出通路调试（AD9744 + OPA2673）
- PCB 焊接与系统联调
- 输出校准与性能优化
- 最终项目展示

---

## 今日作业 | Homework

1. **代码题：** 为正常模式添加波形预览 Canvas——在界面上实时绘制当前波形类型的小预览图 (提示：使用 `lv_canvas` 控件和 `lv_canvas_set_px_color`)
2. **实验题：** 完整测试三种模式，记录每种模式的操作流程和示波器截图。制作一个 "用户操作手册" 简表
3. **思考题：** 为什么频率滑块使用对数映射而不是线性映射？如果使用线性映射 (0~20MHz)，在低频段 (1~100Hz) 会出现什么问题？
4. **Phase 2 报告：** 完成第二周进度报告，包含：软件架构图、驱动模块列表、GUI 设计说明、测试结果

---

## 明日预告 | Tomorrow's Preview

Phase 3 开始！明天我们将深入模拟输出通路——调试 AD9744 高速 DAC 和 OPA2673 运放的配合，优化重建滤波器，让 TinyAWG 的输出波形更加干净和精确。你还将学习 PCB 焊接技巧和信号完整性基础知识。

Phase 3 begins! Tomorrow we dive into the analog output path — tuning the AD9744 high-speed DAC and OPA2673 op-amp cooperation, optimizing the reconstruction filter, and making TinyAWG's output cleaner and more precise. You'll also learn PCB soldering techniques and signal integrity basics.

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| LVGL 8.3 控件文档 | 文档 | https://docs.lvgl.io/8.3/widgets/index.html |
| LVGL 事件系统 | 文档 | https://docs.lvgl.io/8.3/overview/event.html |
| LVGL 样式系统 | 文档 | https://docs.lvgl.io/8.3/overview/style.html |
| LVGL Slider 控件 | 文档 | https://docs.lvgl.io/8.3/widgets/core/slider.html |
| LVGL Dropdown 控件 | 文档 | https://docs.lvgl.io/8.3/widgets/core/dropdown.html |
| AM 调制原理 | 文章 | 搜索 "AM 幅度调制 原理" |
| 对数频率映射 | 文章 | 搜索 "频率控制 对数映射 滑块" |
| Catppuccin 配色方案 | 参考 | 搜索 "Catppuccin color palette" (本文使用的配色) |

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
