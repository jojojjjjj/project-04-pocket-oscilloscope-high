# Day 07: LVGL 显示与触摸驱动 | LVGL Display & Touch Drivers

> **今日目标 Today's Objectives:**
> - 理解 LVGL 8.3.1 框架的分层架构与工作原理
> - 完成 LCD 显示驱动移植 (lcd.c)，实现帧缓冲区管理
> - 编写 LVGL 显示端口 (lv_port_disp.c) 适配层
> - 移植 FT6336 触控驱动 (touch.c, ft6336.c)，编写输入设备端口 (lv_port_indev.c)
> - 校准触摸并测试基础 LVGL 控件 (按钮、标签、滑块)
>
> **产出 Deliverable:** 2.8" 触摸屏正常显示 LVGL 界面，触摸按钮/滑块有响应

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 Morning Meeting | 回顾 Day 6 + 今日目标 |
| 09:30-10:30 | 知识讲解 Lecture | LVGL 框架原理与移植方法 |
| 10:30-10:45 | 休息 Break | Break |
| 10:45-12:00 | 动手实验 Hands-on | LCD 显示驱动 + LVGL 显示端口 |
| 12:00-13:30 | 午餐 Lunch | Lunch + Break |
| 13:30-15:00 | 动手实验 Hands-on | FT6336 触控驱动 + 输入设备端口 |
| 15:00-15:15 | 休息 Break | Break |
| 15:15-16:30 | 动手实验 Hands-on | 触摸校准 + 基础控件测试 |
| 16:30-17:00 | 回顾 Review | 今日总结 + 答疑 |

---

## 上午: LVGL 框架与显示驱动 | Morning: LVGL Framework & Display Driver

### 为什么要学这个? | Why Learn This?

前两天你用串口和示波器验证了 TinyAWG 的输出功能，但一个真正的手持仪器需要**触摸屏界面**——用户用手指点选波形、滑动调节频率。LVGL (Light and Versatile Graphics Library) 是专门为嵌入式设备设计的开源 GUI 库，被广泛用于智能手表、工业面板、医疗设备等。

In the past two days you verified TinyAWG's output using serial port and oscilloscope, but a real handheld instrument needs a **touchscreen interface** — users tap to select waveforms, slide to adjust frequency. LVGL is an open-source GUI library designed specifically for embedded devices, widely used in smartwatches, industrial panels, and medical devices.

**LVGL 在 TinyAWG 中的位置：**

```
┌────────────────────────────────────────────────┐
│            用户界面层 (GUI)                      │
│  ┌──────────┐ ┌──────────┐ ┌───────────────┐   │
│  │ 按钮控件  │ │ 滑块控件  │ │ 下拉列表控件   │   │
│  └──────────┘ └──────────┘ └───────────────┘   │
├────────────────────────────────────────────────┤
│            LVGL 核心层 (lvgl/)                   │
│  渲染引擎 │ 事件系统 │ 动画系统 │ 主题管理       │
├────────────────────────────────────────────────┤
│            硬件适配层 (Porting)                   │
│  ┌──────────────────┐ ┌──────────────────┐      │
│  │ lv_port_disp.c   │ │ lv_port_indev.c  │      │
│  │ (显示适配)        │ │ (输入适配)        │      │
│  └────────┬─────────┘ └────────┬─────────┘      │
│           │                     │                │
│  ┌────────▼─────────┐ ┌────────▼─────────┐      │
│  │ lcd.c (LCD驱动)   │ │ touch.c / ft6336 │      │
│  │ SPI/DMA 通信      │ │ I2C 通信          │      │
│  └──────────────────┘ └──────────────────┘      │
├────────────────────────────────────────────────┤
│            硬件层 (PS Peripherals)               │
│       SPI 控制器              I2C 控制器          │
└────────────────────────────────────────────────┘
```

你需要做的"移植工作"就是中间两层：让 LVGL 知道怎么往屏幕画像素、怎么读取触摸坐标。

### 任务 7.1: LVGL 框架理解与工程配置 (30 分钟)

**LVGL 8.3.1 核心概念：**

| 概念 Concept | 说明 Description | 类比 Analogy |
|-------------|-----------------|-------------|
| 对象 (Object) | 所有 UI 元素都是对象 | 积木块 |
| 屏幕 (Screen) | 最顶层的对象容器 | 画布 |
| 控件 (Widget) | 按钮、标签、滑块等预定义对象 | 不同形状的积木 |
| 事件 (Event) | 用户交互触发 (点击、滑动、值改变) | 按门铃 |
| 样式 (Style) | 控件的外观 (颜色、字体、边框) | 装修风格 |
| 定时器 (Timer) | LVGL 需要周期性调用 `lv_timer_handler()` | 心跳 |

**LVGL 工作流程：**

```c
// LVGL 的最小工作循环
while (1)
{
    lv_timer_handler();    // 处理所有 LVGL 任务 (渲染、动画、事件)
    usleep(5000);          // 约 5ms 间隔 (200Hz 刷新)
}
```

**TinyAWG 的 LVGL 目录结构：**

```
src/
├── lvgl/                    # LVGL 8.3.1 库源码
│   ├── src/                 # 核心源码
│   └── lv_conf.h           # LVGL 配置文件 (非常重要！)
├── display/
│   ├── lcd.c/h             # LCD 底层驱动 (SPI 通信)
│   └── lv_port_disp.c/h    # LVGL 显示适配层
├── touch/
│   ├── touch.c/h           # 触摸驱动接口
│   ├── ft6336.c/h          # FT6336 I2C 驱动
│   └── lv_port_indev.c/h   # LVGL 输入适配层
└── gui/
    ├── lv_setup.c/h         # GUI 初始化
    ├── events_init.c/h      # 事件处理注册
    └── setup_scr_*.c/h      # 各屏幕界面生成
```

**步骤 Steps:**
1. 将 LVGL 8.3.1 源码复制到 `src/lvgl/` 目录
2. 复制 `lv_conf_template.h` 为 `lv_conf.h`，放在 `src/` 下
3. 修改 `lv_conf.h` 中的关键配置：

```c
// lv_conf.h — 关键配置项
// Key configuration items

#define LV_COLOR_DEPTH          16      // RGB565 (16-bit 色深，匹配 LCD)
#define LV_COLOR_16_SWAP        1       // 字节交换 (SPI 传输需要)
#define LV_HOR_RES_MAX          240     // 屏幕宽度 (2.8" LCD)
#define LV_VER_RES_MAX          320     // 屏幕高度 (2.8" LCD)
#define LV_FONT_DEFAULT         &lv_font_montserrat_14  // 默认字体
#define LV_USE_LOG              1       // 启用日志 (调试用，发布时关闭)
#define LV_LOG_LEVEL            LV_LOG_LEVEL_TRACE
#define LV_USE_FONT_PLACEHOLDER 1
#define LV_TICK_CUSTOM          1       // 使用自定义时钟
```

4. 确认编译无错误（此时还没有显示驱动，LVGL 初始化会失败，这是正常的）

**预期结果 Expected Result:**
- `lv_conf.h` 配置完成
- 工程编译通过 (可能有 "display driver not registered" 警告)

**常见问题 Common Issues:**
- "找不到 lv_conf.h" -- 确认文件在 `src/` 目录下，且编译器 include path 包含 `src/`
- "颜色显示不正确" -- `LV_COLOR_16_SWAP` 设置错误，SPI LCD 通常需要字节交换
- "编译报 LV_HOR_RES_MAX 未定义" -- `lv_conf.h` 顶部 `#if 0` 需要改为 `#if 1` 启用配置

### 任务 7.2: LCD 显示驱动 (lcd.c) (45 分钟)

**2.8" TFT LCD 参数：**

| 参数 Parameter | 规格 Specification |
|---------------|-------------------|
| 分辨率 Resolution | 240 × 320 像素 |
| 颜色深度 Color Depth | 16-bit (RGB565) |
| 驱动 IC | 通常为 ILI9341 或 ST7789 |
| 接口 Interface | SPI (4 线: SCK, MOSI, CS, DC) |
| SPI 时钟 | 最高 30 MHz |

**LCD SPI 通信协议：**

```
发送命令 vs 发送数据 (DC 引脚区分):
  DC = 0 → 接下来发送的是命令字节 (Command)
  DC = 1 → 接下来发送的是数据字节 (Data)

写显存流程:
  1. 发命令 0x2A (CASET) + 4字节列地址范围
  2. 发命令 0x2B (RASET) + 4字节行地址范围
  3. 发命令 0x2C (RAMWR) → 之后全部是像素数据
```

```c
// display/lcd.c — LCD 底层驱动
// LCD low-level driver

#include "xspips.h"
#include "xgpiops.h"
#include "xil_printf.h"

// LCD 引脚定义
#define LCD_CS_PIN      56      // EMIO 2 — 片选
#define LCD_DC_PIN      57      // EMIO 3 — 数据/命令
#define LCD_RST_PIN     58      // EMIO 4 — 复位
#define LCD_BL_PIN      59      // EMIO 5 — 背光

extern XSpiPs spi;
extern XGpioPs gpio;

// LCD 尺寸
#define LCD_WIDTH       240
#define LCD_HEIGHT      320

// LCD 命令定义 (ILI9341 / ST7789 通用)
#define LCD_CMD_SWRESET     0x01
#define LCD_CMD_SLPOUT      0x11
#define LCD_CMD_DISPOFF     0x28
#define LCD_CMD_DISPON      0x29
#define LCD_CMD_CASET       0x2A
#define LCD_CMD_RASET       0x2B
#define LCD_CMD_RAMWR       0x2C
#define LCD_CMD_MADCTL      0x36
#define LCD_CMD_COLMOD      0x3A

/**
 * 发送命令到 LCD
 * Send command to LCD (DC = 0)
 */
static void lcd_send_cmd(uint8_t cmd)
{
    XGpioPs_WritePin(&gpio, LCD_DC_PIN, 0);    // DC = 0 (命令模式)
    XGpioPs_WritePin(&gpio, LCD_CS_PIN, 0);    // CS = 0 (选中)

    XSpiPs_PolledTransfer(&spi, &cmd, NULL, 1);

    XGpioPs_WritePin(&gpio, LCD_CS_PIN, 1);    // CS = 1 (释放)
}

/**
 * 发送数据到 LCD
 * Send data to LCD (DC = 1)
 */
static void lcd_send_data(uint8_t *data, int len)
{
    XGpioPs_WritePin(&gpio, LCD_DC_PIN, 1);    // DC = 1 (数据模式)
    XGpioPs_WritePin(&gpio, LCD_CS_PIN, 0);

    XSpiPs_PolledTransfer(&spi, data, NULL, len);

    XGpioPs_WritePin(&gpio, LCD_CS_PIN, 1);
}

/**
 * 初始化 LCD
 * Initialize LCD display
 */
void lcd_init(void)
{
    // 硬件复位
    XGpioPs_WritePin(&gpio, LCD_RST_PIN, 0);
    usleep(10000);    // 10ms
    XGpioPs_WritePin(&gpio, LCD_RST_PIN, 1);
    usleep(120000);   // 等待 120ms

    // 软件复位
    lcd_send_cmd(LCD_CMD_SWRESET);
    usleep(120000);

    // 退出睡眠
    lcd_send_cmd(LCD_CMD_SLPOUT);
    usleep(50000);

    // 设置颜色模式: RGB565 (16-bit)
    lcd_send_cmd(LCD_CMD_COLMOD);
    uint8_t colmod = 0x05;    // 0x05 = 16-bit RGB565
    lcd_send_data(&colmod, 1);

    // 设置显示方向 (竖屏, 从左到右, 从上到下)
    lcd_send_cmd(LCD_CMD_MADCTL);
    uint8_t madctl = 0x00;    // 正常方向
    lcd_send_data(&madctl, 1);

    // 开启显示
    lcd_send_cmd(LCD_CMD_DISPON);
    usleep(10000);

    // 开启背光
    XGpioPs_WritePin(&gpio, LCD_BL_PIN, 1);

    xil_printf("LCD initialized OK (%dx%d)\r\n", LCD_WIDTH, LCD_HEIGHT);
}

/**
 * 设置绘图窗口
 * Set drawing window
 *
 * @param x1  起始列
 * @param y1  起始行
 * @param x2  结束列
 * @param y2  结束行
 */
void lcd_set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint8_t data[4];

    // 设置列地址范围
    lcd_send_cmd(LCD_CMD_CASET);
    data[0] = (x1 >> 8) & 0xFF;
    data[1] = x1 & 0xFF;
    data[2] = (x2 >> 8) & 0xFF;
    data[3] = x2 & 0xFF;
    lcd_send_data(data, 4);

    // 设置行地址范围
    lcd_send_cmd(LCD_CMD_RASET);
    data[0] = (y1 >> 8) & 0xFF;
    data[1] = y1 & 0xFF;
    data[2] = (y2 >> 8) & 0xFF;
    data[3] = y2 & 0xFF;
    lcd_send_data(data, 4);

    // 开始写入像素数据
    lcd_send_cmd(LCD_CMD_RAMWR);
}

/**
 * 刷新帧缓冲区到 LCD
 * Flush framebuffer to LCD
 *
 * 这是 LVGL 显示驱动的核心函数！
 * This is the core function for LVGL display driver!
 *
 * @param x1      区域左上角 X
 * @param y1      区域左上角 Y
 * @param x2      区域右下角 X
 * @param y2      区域右下角 Y
 * @param color_p 像素数据数组 (RGB565 格式)
 */
void lcd_flush(int x1, int y1, int x2, int y2, uint16_t *color_p)
{
    // 设置刷新窗口
    lcd_set_window(x1, y1, x2, y2);

    // 计算像素总数
    uint32_t size = (x2 - x1 + 1) * (y2 - y1 + 1);

    // 发送像素数据 (注意: SPI 发送的是字节流，每像素 2 字节)
    lcd_send_data((uint8_t *)color_p, size * 2);
}
```

**步骤 Steps:**
1. 创建 `display/lcd.c` 和 `display/lcd.h`
2. 实现 LCD 初始化和帧刷新函数
3. 测试 LCD 初始化：

```c
// 在 main() 中
lcd_init();

// 简单测试：填充红色
uint16_t red_buf[LCD_WIDTH];    // 一行红色像素
for (int i = 0; i < LCD_WIDTH; i++) red_buf[i] = 0xF800;  // RGB565 红色

lcd_set_window(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
lcd_send_cmd(LCD_CMD_RAMWR);
for (int row = 0; row < LCD_HEIGHT; row++)
{
    lcd_send_data((uint8_t *)red_buf, LCD_WIDTH * 2);
}
xil_printf("LCD filled with RED\r\n");
```

**预期结果 Expected Result:**
- LCD 屏幕背光亮起
- 整个屏幕显示红色

**常见问题 Common Issues:**
- "屏幕白屏/黑屏无变化" -- SPI 时钟太高或 DC/CS 引脚配置错误
- "颜色不对 (红变蓝)" -- RGB565 字节序问题，需要字节交换或调整 MADCTL
- "只亮一半" -- LCD 复位时间不够，增加延时

### 任务 7.3: LVGL 显示端口适配 (lv_port_disp.c) (30 分钟)

**LVGL 显示端口的作用：** LVGL 不知道你用的是什么屏幕、怎么通信。显示端口就是一个"翻译官"——LVGL 说"把这个区域的像素画出来"，显示端口翻译成具体的 LCD SPI 操作。

```c
// display/lv_port_disp.c — LVGL 显示适配层
// LVGL display port adaptation layer

#include "lvgl.h"
#include "lcd.h"

// 帧缓冲区 (双缓冲可提高刷新率，这里先用单缓冲)
// Framebuffer (double buffering improves refresh rate, single for now)
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[LCD_WIDTH * 50];    // 行缓冲 (一次刷新 50 行)

static lv_disp_drv_t disp_drv;             // 显示驱动对象

/**
 * 显示驱动刷新回调函数
 * Display driver flush callback
 *
 * LVGL 调用此函数将渲染好的像素数据发送到屏幕
 * LVGL calls this function to send rendered pixel data to the screen
 *
 * @param disp_drv  显示驱动指针
 * @param area      需要刷新的区域
 * @param color_p   像素数据 (RGB565 数组)
 */
static void disp_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    // 调用 LCD 底层驱动刷新函数
    lcd_flush(area->x1, area->y1, area->x2, area->y2, (uint16_t *)color_p);

    // 告诉 LVGL 刷新完成 (必须调用!)
    lv_disp_flush_ready(disp_drv);
}

/**
 * 初始化 LVGL 显示驱动
 * Initialize LVGL display driver
 */
void lv_port_disp_init(void)
{
    // 1. 初始化 LCD 硬件
    lcd_init();

    // 2. 初始化 LVGL 绘图缓冲区
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, LCD_WIDTH * 50);

    // 3. 初始化显示驱动
    lv_disp_drv_init(&disp_drv);            // 设置默认值
    disp_drv.hor_res = LCD_WIDTH;           // 水平分辨率
    disp_drv.ver_res = LCD_HEIGHT;          // 垂直分辨率
    disp_drv.flush_cb = disp_flush_cb;      // 刷新回调函数
    disp_drv.draw_buf = &draw_buf;          // 绘图缓冲区

    // 4. 注册显示驱动
    lv_disp_drv_register(&disp_drv);

    xil_printf("LVGL display port initialized\r\n");
}
```

**步骤 Steps:**
1. 创建 `display/lv_port_disp.c` 和 `display/lv_port_disp.h`
2. 实现显示端口初始化和刷新回调
3. 在 main() 中测试 LVGL 渲染：

```c
#include "lvgl.h"
#include "lv_port_disp.h"

// LVGL 时钟节拍 (需要周期性调用)
static uint32_t lv_tick_value = 0;

// 自定义 tick 函数
uint32_t custom_tick_get(void)
{
    return lv_tick_value;
}

int main()
{
    init_platform();
    gpio_init();
    spi_init();

    xil_printf("=== TinyAWG LVGL Test ===\r\n");

    // 初始化 LVGL
    lv_init();
    lv_tick_set_cb(custom_tick_get);

    // 初始化显示端口
    lv_port_disp_init();

    // 创建一个简单的标签测试
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "TinyAWG v1.0\nHello LVGL!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    xil_printf("LVGL running, entering main loop...\r\n");

    // 主循环
    while (1)
    {
        lv_tick_value += 5;            // 5ms 递增
        lv_timer_handler();            // 处理 LVGL 任务
        usleep(5000);                  // 5ms 延时
    }

    cleanup_platform();
    return 0;
}
```

**预期结果 Expected Result:**
- LCD 屏幕显示 "TinyAWG v1.0" 和 "Hello LVGL!" 文字
- 文字位于屏幕中央

**常见问题 Common Issues:**
- "屏幕全黑无显示" -- `lv_disp_flush_ready()` 忘记调用，LVGL 会一直等"上一帧完成"
- "文字颜色看不清" -- LVGL 默认白色文字在黑色背景上，确认 LCD 颜色正确
- "只显示了一部分" -- 行缓冲区太小，增大 `buf1` 的行数

---

## 下午: 触摸驱动与控件测试 | Afternoon: Touch Driver & Widget Testing

### 为什么要学这个? | Why Learn This?

显示驱动让 TinyAWG 有了"眼睛"，但用户还需要"手指"来操作。FT6336 是一颗电容触摸控制器，通过 I2C 接口上报触摸坐标。将它接入 LVGL 的输入设备系统，用户就能用手指点选按钮、拖动滑块——这才是真正的交互体验。

The display driver gives TinyAWG "eyes," but users still need "fingers" to operate. FT6336 is a capacitive touch controller that reports touch coordinates via I2C. Integrating it into LVGL's input device system allows users to tap buttons and drag sliders — this is real interactive experience.

### 任务 7.4: FT6336 触控驱动 (45 分钟)

**FT6336 关键参数：**

| 参数 Parameter | 规格 Specification |
|---------------|-------------------|
| 触摸点数 Touch Points | 最多 5 点 (TinyAWG 只用 1 点) |
| 接口 Interface | I2C (地址 0x38) |
| 分辨率 Resolution | 最大支持 800 × 600 |
| 报告速率 Report Rate | 最高 100Hz |
| 中断引脚 Interrupt | 触摸时产生下降沿中断 |

**FT6336 I2C 寄存器映射：**

| 寄存器地址 Register | 名称 Name | 内容 Content |
|--------------------|-----------|-------------|
| 0x02 | TD_STATUS | 触摸点数 (0~5) |
| 0x03 | P1_XH | 触点 1 X 坐标高 4 位 + 事件标志 |
| 0x04 | P1_XL | 触点 1 X 坐标低 8 位 |
| 0x05 | P1_YH | 触点 1 Y 坐标高 4 位 |
| 0x06 | P1_YL | 触点 1 Y 坐标低 8 位 |

```c
// touch/ft6336.c — FT6336 电容触摸控制器驱动
// FT6336 capacitive touch controller driver

#include "xiicps.h"
#include "xgpiops.h"
#include "xil_printf.h"

#define FT6336_ADDR         0x38    // FT6336 I2C 7-bit 地址
#define FT6336_REG_TD_STATUS  0x02  // 触摸状态寄存器
#define FT6336_REG_P1_XH     0x03   // 触点 1 X 高字节
#define FT6336_REG_P1_XL     0x04   // 触点 1 X 低字节
#define FT6336_REG_P1_YH     0x05   // 触点 1 Y 高字节
#define FT6336_REG_P1_YL     0x06   // 触点 1 Y 低字节

extern XIicPs i2c;

/**
 * 从 FT6336 读取寄存器
 * Read register from FT6336
 */
static int ft6336_read_reg(uint8_t reg, uint8_t *buf, int len)
{
    int status;

    // 发送寄存器地址
    status = XIicPs_MasterSendPolled(&i2c, &reg, 1, FT6336_ADDR);
    if (status != XST_SUCCESS) return status;

    while (XIicPs_BusIsBusy(&i2c));

    // 读取数据
    status = XIicPs_MasterRecvPolled(&i2c, buf, len, FT6336_ADDR);
    if (status != XST_SUCCESS) return status;

    while (XIicPs_BusIsBusy(&i2c));

    return XST_SUCCESS;
}

/**
 * 读取触摸坐标
 * Read touch coordinates
 *
 * @param x       X 坐标输出 (0~239)
 * @param y       Y 坐标输出 (0~319)
 * @param pressed 是否按下 (1=按下, 0=松开)
 * @return        XST_SUCCESS or XST_FAILURE
 */
int ft6336_read_touch(uint16_t *x, uint16_t *y, uint8_t *pressed)
{
    uint8_t buf[5];

    // 读取触摸状态和坐标 (寄存器 0x02~0x06)
    int status = ft6336_read_reg(FT6336_REG_TD_STATUS, buf, 5);
    if (status != XST_SUCCESS)
    {
        *pressed = 0;
        return status;
    }

    // 触摸点数
    uint8_t touch_count = buf[0] & 0x0F;

    if (touch_count > 0 && touch_count <= 5)
    {
        // 解析坐标 (12-bit, 高 4 位在高字节, 低 8 位在低字节)
        uint16_t raw_x = ((buf[1] & 0x0F) << 8) | buf[2];
        uint16_t raw_y = ((buf[3] & 0x0F) << 8) | buf[4];

        // FT6336 返回的坐标可能需要旋转/翻转以匹配 LCD 方向
        // TinyAWG LCD 竖屏: X=0~239, Y=0~319
        *x = raw_x;
        *y = raw_y;

        // 限制范围
        if (*x >= 240) *x = 239;
        if (*y >= 320) *y = 319;

        *pressed = 1;
    }
    else
    {
        *pressed = 0;
        *x = 0;
        *y = 0;
    }

    return XST_SUCCESS;
}
```

**步骤 Steps:**
1. 创建 `touch/ft6336.c` 和 `touch/ft6336.h`
2. 实现 FT6336 I2C 通信和坐标读取函数
3. 测试：用串口打印触摸坐标

```c
i2c_init();

xil_printf("=== FT6336 Touch Test ===\r\n");
xil_printf("Touch the screen to see coordinates...\r\n");

while (1)
{
    uint16_t x, y;
    uint8_t pressed;

    ft6336_read_touch(&x, &y, &pressed);
    if (pressed)
    {
        xil_printf("Touch: X=%3d Y=%3d\r\n", x, y);
    }
    usleep(20000);    // 20ms 轮询
}
```

**预期结果 Expected Result:**
- 手指触摸屏幕时串口打印坐标
- 松开后不再打印
- 坐标值在 0~239 (X) 和 0~319 (Y) 范围内

**常见问题 Common Issues:**
- "I2C 通信失败" -- 确认 FT6336 地址是 0x38 (7-bit)，部分模块可能不同
- "坐标不变/固定值" -- FT6336 未正确初始化，检查 I2C 波特率 (建议 400kHz)
- "坐标是镜像的" -- 需要在软件中做 X/Y 交换或翻转，取决于触摸屏安装方向

### 任务 7.5: LVGL 输入设备端口 (lv_port_indev.c) (30 分钟)

```c
// touch/lv_port_indev.c — LVGL 输入设备适配层
// LVGL input device port adaptation layer

#include "lvgl.h"
#include "ft6336.h"

static lv_indev_drv_t indev_drv;        // 输入设备驱动
static lv_indev_t *indev_touchpad;      // 输入设备指针

/**
 * 触摸读取回调函数
 * Touchpad read callback
 *
 * LVGL 周期性调用此函数获取触摸状态
 * LVGL calls this periodically to get touch state
 *
 * @param drv    输入设备驱动指针
 * @param data   输出数据 (坐标和按下状态)
 */
static void touchpad_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    uint16_t x, y;
    uint8_t pressed;

    // 从 FT6336 读取触摸数据
    ft6336_read_touch(&x, &y, &pressed);

    if (pressed)
    {
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }

    // 返回最后一个已知坐标 (即使松开也需要有效坐标)
    // Return last known coordinates (needed even when released)
    data->point.x = x;
    data->point.y = y;
}

/**
 * 初始化 LVGL 输入设备
 * Initialize LVGL input device
 */
void lv_port_indev_init(void)
{
    // 初始化输入设备驱动
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;            // 指针类型 (触摸屏/鼠标)
    indev_drv.read_cb = touchpad_read_cb;              // 读取回调

    // 注册输入设备
    indev_touchpad = lv_indev_drv_register(&indev_drv);

    xil_printf("LVGL input device initialized\r\n");
}
```

**步骤 Steps:**
1. 创建 `touch/lv_port_indev.c` 和 `touch/lv_port_indev.h`
2. 实现 LVGL 输入设备适配
3. 在 main() 中同时初始化显示和输入：

```c
lv_init();
lv_tick_set_cb(custom_tick_get);
lv_port_disp_init();        // 显示驱动
lv_port_indev_init();       // 触摸驱动

xil_printf("LVGL fully initialized!\r\n");
```

**预期结果 Expected Result:**
- LVGL 初始化无错误
- 触摸事件可以被 LVGL 接收

**常见问题 Common Issues:**
- "触摸无反应" -- 确认 `read_cb` 返回 `LV_INDEV_STATE_PRESSED` 而不是 0/1
- "触摸位置偏移" -- FT6336 坐标系与 LCD 坐标系不一致，需要做坐标映射

### 任务 7.6: 触摸校准与基础控件测试 (45 分钟)

**创建测试界面：一个按钮和一个滑块**

```c
// gui/lv_setup.c — GUI 初始化与测试
// GUI initialization and test

#include "lvgl.h"

// 全局控件引用 (供事件回调使用)
static lv_obj_t *btn_test;
static lv_obj_t *label_btn;
static lv_obj_t *slider_test;
static lv_obj_t *label_slider;

/**
 * 按钮事件回调
 * Button event callback
 */
static void btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        static int count = 0;
        count++;
        char buf[32];
        sprintf(buf, "Clicked: %d", count);
        lv_label_set_text(label_btn, buf);
        xil_printf("Button clicked: %d\r\n", count);
    }
}

/**
 * 滑块值改变回调
 * Slider value change callback
 */
static void slider_event_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    int value = lv_slider_get_value(slider);

    char buf[16];
    sprintf(buf, "Value: %d", value);
    lv_label_set_text(label_slider, buf);
    xil_printf("Slider: %d\r\n", value);
}

/**
 * 创建测试界面
 * Create test screen
 */
void gui_create_test_screen(void)
{
    // 创建一个按钮
    btn_test = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn_test, 120, 50);
    lv_obj_align(btn_test, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_add_event_cb(btn_test, btn_event_cb, LV_EVENT_CLICKED, NULL);

    label_btn = lv_label_create(btn_test);
    lv_label_set_text(label_btn, "Tap me!");
    lv_obj_center(label_btn);

    // 创建一个滑块
    slider_test = lv_slider_create(lv_scr_act());
    lv_obj_set_width(slider_test, 200);
    lv_obj_align(slider_test, LV_ALIGN_CENTER, 0, 0);
    lv_slider_set_range(slider_test, 0, 100);
    lv_slider_set_value(slider_test, 50, LV_ANIM_OFF);
    lv_obj_add_event_cb(slider_test, slider_event_cb,
                        LV_EVENT_VALUE_CHANGED, NULL);

    label_slider = lv_label_create(lv_scr_act());
    lv_label_set_text(label_slider, "Value: 50");
    lv_obj_align_to(label_slider, slider_test, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    // 标题
    lv_obj_t *title = lv_label_create(lv_scr_act());
    lv_label_set_text(title, "TinyAWG Touch Test");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 80);

    xil_printf("Test GUI created\r\n");
}
```

**完整的 main() 函数：**

```c
int main()
{
    init_platform();
    xil_printf("=== TinyAWG LVGL GUI Test ===\r\n");

    // 初始化硬件
    gpio_init();
    spi_init();
    i2c_init();

    // 初始化 LVGL
    lv_init();
    lv_tick_set_cb(custom_tick_get);
    lv_port_disp_init();
    lv_port_indev_init();

    // 创建测试界面
    gui_create_test_screen();

    xil_printf("GUI running. Touch the screen!\r\n");

    // 主循环
    while (1)
    {
        lv_tick_value += 5;
        lv_timer_handler();
        usleep(5000);
    }

    cleanup_platform();
    return 0;
}
```

**步骤 Steps:**
1. 创建 `gui/lv_setup.c` 和 `gui/lv_setup.h`
2. 实现测试界面和事件回调
3. 编译下载运行
4. 用手指触摸按钮，观察计数变化
5. 拖动滑块，观察数值变化
6. 检查触摸位置是否准确（手指点哪里，按钮就在哪里响应）

**触摸校准（如果位置偏移）：**

```c
// 在 ft6336_read_touch() 中添加坐标映射
// TinyAWG 可能需要 X/Y 交换或翻转，取决于安装方向

// 方案 1: X/Y 交换 (横屏安装)
*x = raw_y;
*y = LCD_WIDTH - 1 - raw_x;

// 方案 2: X 翻转
*x = LCD_WIDTH - 1 - raw_x;
*y = raw_y;

// 方案 3: 不变换 (竖屏，默认)
*x = raw_x;
*y = raw_y;
```

**预期结果 Expected Result:**
- 屏幕显示按钮 "Tap me!" 和滑块
- 手指点击按钮，标签显示 "Clicked: 1", "Clicked: 2"...
- 拖动滑块，标签显示对应数值 (0~100)
- 触摸位置准确，无偏移

**常见问题 Common Issues:**
- "按钮点击无响应但滑块可以拖动" -- 按钮事件可能没有正确注册，检查 `lv_obj_add_event_cb`
- "触摸位置偏移 90 度" -- FT6336 坐标系与 LCD 方向差 90 度，需要交换 X/Y
- "滑块拖不动" -- `LV_INDEV_STATE_RELEASED` 时也需要提供正确的坐标，否则 LVGL 认为手指已经离开
- "屏幕刷新太慢/闪烁" -- 增大行缓冲区大小，或检查 SPI 时钟频率

---

## 今日作业 | Homework

1. **代码题：** 为 LVGL 添加一个下拉列表 (dropdown) 控件，选项为 "Sine", "Square", "Triangle", "Ramp"，选择后串口打印当前选项
2. **实验题：** 测试 LVGL 刷新率——在 `disp_flush_cb` 中计数每秒调用次数，计算实际 FPS；调整行缓冲区大小观察 FPS 变化
3. **思考题：** 为什么 LVGL 需要周期性调用 `lv_timer_handler()`？如果调用频率过低 (例如 10Hz) 会发生什么？
4. **写实验日志：** 记录 LCD 驱动和触摸驱动的调试过程，特别是 SPI 时序问题和触摸坐标映射

---

## 明日预告 | Tomorrow's Preview

今天你让 TinyAWG 有了"眼睛"和"手指"——触摸屏能显示界面、能响应操作。明天我们将设计 TinyAWG 的完整 GUI 界面：正常模式 (波形选择、频率/幅度/偏置调节)、扫描模式 (扫频)、调制模式 (AM)，并把 GUI 控件真正连接到 DAC/DDS 参数！

Today TinyAWG got "eyes" and "fingers" — the touchscreen can display UI and respond to touches. Tomorrow we design TinyAWG's complete GUI: normal mode (waveform selection, frequency/amplitude/offset control), scan mode (frequency sweep), modulation mode (AM), and connect GUI controls to actual DAC/DDS parameters!

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| LVGL 8.3 官方文档 | 文档 | https://docs.lvgl.io/8.3/ |
| LVGL 移植指南 | 文档 | https://docs.lvgl.io/8.3/porting/index.html |
| LVGL 控件示例 | 示例 | https://docs.lvgl.io/8.3/widgets/index.html |
| FT6336 数据手册 | PDF | 搜索 "FT6336 datasheet" |
| ILI9341 数据手册 | PDF | 搜索 "ILI9341 datasheet" |
| LVGL 入门教程 | B站视频 | 搜索 "LVGL 入门 移植 教程" |
| LVGL 中文社区 | 论坛 | https://lvgl.100ask.net/ |

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
