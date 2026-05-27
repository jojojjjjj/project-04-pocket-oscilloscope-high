# Day 06: TFT 波形显示 | TFT Waveform Display

> **今日目标 Today's Objectives:**
> - 理解 SPI 通信协议和 TFT 显示屏驱动原理
> - 使用 STM32 SPI 驱动 ST7789 TFT 显示屏
> - 在 TFT 屏幕上绘制网格、坐标轴和波形曲线
> - 实现基本的示波器 UI 界面
>
> **产出 Deliverable:** TFT 屏幕实时显示带网格和刻度的示波器波形界面

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 | 回顾触发逻辑 + 今日目标 |
| 09:30-10:30 | 知识讲解 | SPI 协议 + TFT 显示原理 |
| 10:30-10:45 | 休息 | Break |
| 10:45-12:00 | 动手实验 | TFT 屏初始化 + 基本绘图 |
| 12:00-13:30 | 午餐 | Lunch + Break |
| 13:30-15:00 | 动手实验 | 波形绘制函数 |
| 15:00-15:15 | 休息 | Break |
| 15:15-16:30 | 动手实验 | 示波器 UI 界面设计 |
| 16:30-17:00 | 回顾 | 今日总结 + 答疑 |

---

## 上午: SPI 与 TFT 驱动 | Morning: SPI & TFT Driving

### 为什么要学这个? | Why Learn This?

到目前为止，我们的波形只能通过串口在电脑上显示。但一个真正的示波器需要有自己的屏幕！TFT (Thin Film Transistor) 彩色液晶屏是嵌入式设备最常用的显示方案。理解 SPI 通信和 TFT 驱动，你就能让任何嵌入式设备拥有自己的显示界面。

So far, our waveforms can only be displayed on a PC via serial port. But a real oscilloscope needs its own screen! TFT (Thin Film Transistor) color LCD is the most common display solution for embedded devices. Understanding SPI communication and TFT driving allows you to give any embedded device its own display interface.

**SPI 是什么？**

SPI (Serial Peripheral Interface) 是一种高速同步串行通信协议。它用 4 根线：

```
STM32 (Master)              TFT (Slave)
   MOSI  ────────────────────  SDA  (数据：主→从)
   SCK   ────────────────────  SCL  (时钟)
   CS    ────────────────────  CS   (片选)
   DC    ────────────────────  DC   (数据/命令选择)
   RST   ────────────────────  RST  (复位)
   3.3V  ────────────────────  VCC  (电源)
   GND   ────────────────────  GND  (地)
```

### 任务 6.1: TFT 屏幕接线与初始化 (45 分钟)

**ST7789 TFT 引脚定义 (1.8寸 160x128)：**

| TFT 引脚 | 连接到 STM32 | 说明 |
|---------|-------------|------|
| VCC | 3.3V | 电源 |
| GND | GND | 地 |
| SCL | PA5 (SPI1_SCK) | SPI 时钟 |
| SDA | PA7 (SPI1_MOSI) | SPI 数据 |
| CS | PA4 | 片选 (低电平有效) |
| DC | PA3 | 数据/命令选择 |
| RST | PA2 | 复位 (低电平有效) |
| BLK | PA1 | 背光控制 (高电平点亮) |

**CubeMX SPI 配置：**
- SPI1: Full-Duplex Master
- Prescaler: 2 (36MHz，SPI 最高速)
- Data Size: 8-bit
- CPOL: Low, CPHA: 1 Edge
- NSS: Software (软件片选)

**步骤 Steps:**
1. **接线：** 按照上表连接 TFT 屏幕到 STM32
2. **CubeMX 配置 SPI1 和 GPIO：**
   - SPI1: 如上配置
   - PA1, PA2, PA3, PA4: GPIO_Output
3. **编写 TFT 驱动初始化代码：**

```c
// display.c - TFT 显示驱动
#include "display.h"
#include "spi.h"

// ST7789 命令定义
#define ST7789_NOP       0x00
#define ST7789_SWRESET   0x01
#define ST7789_SLPOUT    0x11
#define ST7789_NORON     0x13
#define ST7789_INVON     0x21
#define ST7789_DISPON    0x29
#define ST7789_CASET     0x2A
#define ST7789_RASET     0x2B
#define ST7789_RAMWR     0x2C
#define ST7789_MADCTL    0x36
#define ST7789_COLMOD    0x3A

// 屏幕尺寸
#define SCREEN_WIDTH     160
#define SCREEN_HEIGHT    128

// GPIO 引脚定义
#define TFT_CS_PIN   GPIO_PIN_4
#define TFT_CS_PORT  GPIOA
#define TFT_DC_PIN   GPIO_PIN_3
#define TFT_DC_PORT  GPIOA
#define TFT_RST_PIN  GPIO_PIN_2
#define TFT_RST_PORT GPIOA
#define TFT_BLK_PIN  GPIO_PIN_1
#define TFT_BLK_PORT GPIOA

/**
 * 发送命令到 TFT
 * Send command to TFT
 */
void tft_send_cmd(uint8_t cmd)
{
    HAL_GPIO_WritePin(TFT_DC_PORT, TFT_DC_PIN, GPIO_PIN_RESET);  // DC = 0 (命令)
    HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_RESET);  // CS = 0 (选中)
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_SET);    // CS = 1 (释放)
}

/**
 * 发送数据到 TFT
 * Send data to TFT
 */
void tft_send_data(uint8_t *data, uint16_t size)
{
    HAL_GPIO_WritePin(TFT_DC_PORT, TFT_DC_PIN, GPIO_PIN_SET);    // DC = 1 (数据)
    HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_RESET);  // CS = 0
    HAL_SPI_Transmit(&hspi1, data, size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_SET);    // CS = 1
}

/**
 * TFT 初始化
 * Initialize TFT display
 */
void tft_init(void)
{
    // 硬件复位
    HAL_GPIO_WritePin(TFT_RST_PORT, TFT_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(TFT_RST_PORT, TFT_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(100);

    // 打开背光
    HAL_GPIO_WritePin(TFT_BLK_PORT, TFT_BLK_PIN, GPIO_PIN_SET);

    // 初始化序列
    tft_send_cmd(ST7789_SWRESET);
    HAL_Delay(150);

    tft_send_cmd(ST7789_SLPOUT);
    HAL_Delay(50);

    tft_send_cmd(ST7789_COLMOD);
    uint8_t colmod = 0x05;  // 16-bit color (RGB565)
    tft_send_data(&colmod, 1);

    tft_send_cmd(ST7789_MADCTL);
    uint8_t madctl = 0x00;  // 正常方向
    tft_send_data(&madctl, 1);

    tft_send_cmd(ST7789_INVON);   // 颜色反转（ST7789 需要）
    HAL_Delay(10);

    tft_send_cmd(ST7789_NORON);
    HAL_Delay(10);

    tft_send_cmd(ST7789_DISPON);
    HAL_Delay(10);
}
```

4. **测试：** 编译下载，TFT 屏幕应点亮并显示白色/黑色背景

**预期结果 Expected Result:**
- TFT 屏幕背光亮起
- 屏幕显示内容（可能是随机颜色，说明初始化成功）

**常见问题 Common Issues:**
- "屏幕不亮" -- 检查 BLK 引脚是否接高电平，VCC 是否供电
- "屏幕全白" -- SPI 接线可能有误，检查 MOSI/SCK/CS 连接
- "颜色不对" -- 可能需要调整 MADCTL 寄存器值

### 任务 6.2: 基本绘图函数 (45 分钟)

**RGB565 颜色格式：**

TFT 使用 16-bit 颜色，格式为 RGB565：
```
Bit:   15 14 13 12 11 | 10 9 8 7 6 5 | 4 3 2 1 0
Color:  R  R  R  R  R  |  G  G G G G G |  B  B B B B
```

常用颜色定义：
```c
// display.h 中定义常用颜色
#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xFFFF
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_BLUE      0x001F
#define COLOR_YELLOW    0xFFE0
#define COLOR_CYAN      0x07FF
#define COLOR_GRAY      0x8410
#define COLOR_DARK_GRAY 0x4208
#define COLOR_BG        0x0000    // 背景色：黑色
#define COLOR_GRID      0x4208    // 网格色：深灰
#define COLOR_WAVE      0x07E0    // 波形色：绿色
#define COLOR_TEXT       0xFFFF    // 文字色：白色
```

**绘制像素点：**

```c
/**
 * 设置绘图窗口
 * Set drawing window
 */
void tft_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t data[4];

    tft_send_cmd(ST7789_CASET);    // 列地址设置
    data[0] = x0 >> 8; data[1] = x0 & 0xFF;
    data[2] = x1 >> 8; data[3] = x1 & 0xFF;
    tft_send_data(data, 4);

    tft_send_cmd(ST7789_RASET);    // 行地址设置
    data[0] = y0 >> 8; data[1] = y0 & 0xFF;
    data[2] = y1 >> 8; data[3] = y1 & 0xFF;
    tft_send_data(data, 4);

    tft_send_cmd(ST7789_RAMWR);    // 开始写入显示数据
}

/**
 * 绘制一个像素点
 * Draw a single pixel
 */
void tft_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) return;

    tft_set_window(x, y, x, y);
    uint8_t data[2] = {color >> 8, color & 0xFF};
    tft_send_data(data, 2);
}

/**
 * 绘制直线 (Bresenham 算法)
 * Draw a line using Bresenham's algorithm
 */
void tft_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1)
    {
        tft_draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx)  { err += dx; y0 += sy; }
    }
}

/**
 * 填充矩形
 * Fill a rectangle
 */
void tft_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    tft_set_window(x, y, x + w - 1, y + h - 1);
    uint8_t buf[2] = {color >> 8, color & 0xFF};

    for (uint32_t i = 0; i < w * h; i++)
    {
        tft_send_data(buf, 2);
    }
}
```

**步骤 Steps:**
1. 创建 `display.c` 和 `display.h`
2. 实现上述绘图函数
3. 测试：在屏幕上画一个红色矩形和绿色对角线

**预期结果 Expected Result:**
- 屏幕显示红色矩形
- 绿色对角线从左上角到右下角

---

## 下午: 波形绘制与 UI | Afternoon: Waveform Drawing & UI

### 任务 6.3: 网格绘制函数 (30 分钟)

```c
/**
 * 绘制示波器网格
 * Draw oscilloscope grid
 */
void draw_grid(void)
{
    // 竖线 (8 格，每格 20 像素)
    for (int i = 0; i <= 8; i++)
    {
        uint16_t x = i * 20;
        tft_draw_line(x, 0, x, SCREEN_HEIGHT - 1, COLOR_GRID);
    }

    // 横线 (6 格，每格约 21 像素)
    for (int i = 0; i <= 6; i++)
    {
        uint16_t y = i * 21;
        tft_draw_line(0, y, SCREEN_WIDTH - 1, y, COLOR_GRID);
    }

    // 中心十字线 (加粗)
    tft_draw_line(80, 0, 80, SCREEN_HEIGHT - 1, COLOR_DARK_GRAY);
    tft_draw_line(0, 64, SCREEN_WIDTH - 1, 64, COLOR_DARK_GRAY);
}
```

### 任务 6.4: 波形绘制函数 (45 分钟)

**核心挑战：** ADC 值是 0~4095，屏幕 Y 轴只有 0~127 像素。需要做缩放。

```
屏幕 Y 像素 = 屏幕高度 - 1 - (ADC值 × 屏幕高度 / 4096)
            = 127 - (ADC值 × 128 / 4096)
            = 127 - (ADC值 >> 5)    // 近似除以 32
```

```c
#define WAVEFORM_AREA_X      0
#define WAVEFORM_AREA_Y      0
#define WAVEFORM_AREA_W      160
#define WAVEFORM_AREA_H      128

/**
 * 绘制波形
 * Draw waveform from ADC buffer
 *
 * @param buffer  ADC 数据缓冲区
 * @param count   数据点数
 * @param color   波形颜色
 */
void draw_waveform(uint16_t *buffer, uint16_t count, uint16_t color)
{
    // 限制显示点数不超过屏幕宽度
    if (count > WAVEFORM_AREA_W) count = WAVEFORM_AREA_W;

    uint16_t prev_y = 0;

    for (uint16_t i = 0; i < count; i++)
    {
        // ADC 值 (0~4095) 映射到屏幕 Y 坐标 (0~127)
        uint16_t y = WAVEFORM_AREA_H - 1 - (buffer[i] * WAVEFORM_AREA_H / 4096);
        uint16_t x = i;

        if (i > 0)
        {
            // 画线连接相邻两点
            tft_draw_line(x - 1, prev_y, x, y, color);
        }
        prev_y = y;
    }
}

/**
 * 清除波形区域（重绘背景和网格）
 * Clear waveform area
 */
void clear_waveform_area(void)
{
    tft_fill_rect(WAVEFORM_AREA_X, WAVEFORM_AREA_Y,
                  WAVEFORM_AREA_W, WAVEFORM_AREA_H, COLOR_BG);
    draw_grid();
}
```

**步骤 Steps:**
1. 实现网格绘制和波形绘制函数
2. 在 main.c 中集成：

```c
// 初始化
tft_init();
clear_waveform_area();

// 主循环
while (1)
{
    if (buffer_full)
    {
        buffer_full = 0;

        // 寻找触发点
        uint16_t trig_level = calculate_center_level(adc_buffer, SAMPLE_COUNT);
        int trig_point = find_rising_edge(adc_buffer, SAMPLE_COUNT, trig_level);

        // 清除旧波形
        clear_waveform_area();

        // 绘制新波形
        if (trig_point >= 0)
        {
            draw_waveform(&adc_buffer[trig_point],
                         SAMPLE_COUNT - trig_point, COLOR_WAVE);
        }
        else
        {
            draw_waveform(adc_buffer, SAMPLE_COUNT, COLOR_WAVE);
        }

        // 重新启动采集
        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, SAMPLE_COUNT);
    }
}
```

3. 编译下载，观察 TFT 屏幕上的波形

**预期结果 Expected Result:**
- TFT 屏幕显示绿色网格背景
- 正弦波形以绿色显示在网格上
- 波形稳定不跑动（触发功能生效）

**常见问题 Common Issues:**
- "波形反了" -- ADC 值越大电压越高，但屏幕 Y 轴向下增大，需要翻转
- "波形太小/太大" -- 调整 Y 轴缩放比例
- "刷新太慢" -- SPI 速度不够或绘图函数效率低，可优化 DMA SPI

### 任务 6.5: 示波器 UI 信息显示 (30 分钟)

```c
// 简单的数字/字符显示 (不使用字库，用像素画)
// 或者使用简易 5x7 字库

/**
 * 在屏幕顶部绘制参数信息
 * Draw parameter info at top of screen
 */
void draw_info_bar(uint16_t freq_khz, uint16_t vpp_mv, const char *timebase)
{
    // 顶部信息栏背景
    tft_fill_rect(0, 0, SCREEN_WIDTH, 10, COLOR_BLUE);

    // 这里可以添加简易字库显示文字
    // 简化版：用不同颜色的小方块代表不同参数
}
```

---

## 今日作业 | Homework

1. **优化题：** 波形绘制使用逐点 `tft_draw_pixel` 效率很低，尝试优化为整行 DMA 传输
2. **实验题：** 调整波形的颜色、网格密度，让界面更好看
3. **思考题：** 为什么商用示波器的波形刷新率能达到每秒几千次？我们的方案有什么限制？
4. **写实验日志：** 记录 TFT 驱动调试过程中遇到的问题和解决方法

---

## 明日预告 | Tomorrow's Preview

明天我们将实现时基控制和采样率切换——就像转动示波器的"TIME/DIV"旋钮，你可以放大看细节（高采样率/短时基），也可以缩小看全貌（低采样率/长时基）。

Tomorrow we will implement timebase control and sampling rate switching -- like turning the "TIME/DIV" knob on an oscilloscope. You can zoom in for detail (high sample rate/short timebase) or zoom out for overview (low sample rate/long timebase).

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| 三代示波器开发-波形显示 | B站视频 | https://www.bilibili.com/video/BV1pT4y1o7ZK/ |
| STM32 TFT SPI 屏示波器 | GitHub | github.com/offpic/OSCILLOSCOPE-STM32-ILI9341-SPI-TFT |
| ST7789 数据手册 | PDF | 搜索"ST7789 datasheet" |
| Bresenham 画线算法 | 搜索 | 搜索"Bresenham 直线算法" |

---

*最后更新：2026-05-26*
*Last updated: 2026-05-26*
