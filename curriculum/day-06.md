# Day 06: DAC 驱动与输出控制 | DAC Drivers & Output Control

> **今日目标 Today's Objectives:**
> - 理解 AD9744 高速 DAC 的并行接口、时序要求和数据格式 (14-bit, 2's complement)
> - 掌握 DAC8562 精密 DAC 的 SPI 驱动 (16-bit 双通道，幅度/偏置控制)
> - 实现波形数据生成 (正弦波、方波、三角波、锯齿波、噪声)
> - 完成输出控制模块 (继电器切换、输出使能)，用示波器验证模拟波形输出
>
> **产出 Deliverable:** TinyAWG 输出可调频率的正弦波/方波/三角波，示波器显示清晰波形

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 Morning Meeting | 回顾 Day 5 + 今日目标 |
| 09:30-10:30 | 知识讲解 Lecture | AD9744 与 DAC8562 工作原理 |
| 10:30-10:45 | 休息 Break | Break |
| 10:45-12:00 | 动手实验 Hands-on | DAC8562 SPI 驱动 + AD9744 数据格式 |
| 12:00-13:30 | 午餐 Lunch | Lunch + Break |
| 13:30-15:00 | 动手实验 Hands-on | 波形数据生成 + 输出控制模块 |
| 15:00-15:15 | 休息 Break | Break |
| 15:15-16:30 | 动手实验 Hands-on | 示波器验证波形输出 |
| 16:30-17:00 | 回顾 Review | 今日总结 + 答疑 |

---

## 上午: DAC 芯片原理与驱动 | Morning: DAC Chip Principles & Drivers

### 为什么要学这个? | Why Learn This?

昨天你学会了让 ARM 处理器控制 LED、读电池、发 SPI/I2C。但 TinyAWG 的核心使命是**产生模拟信号**——把数字世界的 "0 和 1" 变成真实世界的电压波形。DAC (Digital-to-Analog Converter，数模转换器) 就是连接数字和模拟的桥梁。

Yesterday you learned to control LEDs, read battery, and send SPI/I2C. But TinyAWG's core mission is **generating analog signals** — turning digital "0s and 1s" into real-world voltage waveforms. The DAC (Digital-to-Analog Converter) is the bridge between digital and analog.

**TinyAWG 的双 DAC 架构：**

```
                    数字侧 (PS/FPGA)                  模拟侧 (输出)
                    ──────────────                    ───────────

  波形数据 ──→  ┌─────────────┐    模拟信号    ┌──────────┐
  (14-bit)     │   AD9744     │────────────→ │ OPA2673  │──→ BNC 输出
  200MSa/s     │   高速 DAC   │    电流输出   │ 高速运放  │
               └─────────────┘              └──────────┘

  幅度/偏置 ──→┌─────────────┐   控制电压    ┌──────────┐
  (16-bit)     │   DAC8562    │────────────→ │ 增益/偏置 │
  SPI          │   精密 DAC   │              │ 调节电路  │
               └─────────────┘              └──────────┘
```

- **AD9744** 负责"快速画波形"——200MSa/s 高速输出信号
- **DAC8562** 负责"调节音量"——控制输出波形的幅度和直流偏置

### 任务 6.1: AD9744 高速 DAC — 原理与数据格式 (30 分钟)

**AD9744 关键参数：**

| 参数 Parameter | 规格 Specification |
|---------------|-------------------|
| 分辨率 Resolution | 14-bit |
| 采样率 Max Sample Rate | 210 MSa/s |
| 数据接口 Data Interface | 并行 14-bit |
| 输出类型 Output Type | 电流输出 (需外接运放) |
| 数据格式 Data Format | 二进制补码 (2's complement) |
| TinyAWG 实际使用 | 200 MSa/s, 200MHz FPGA 时钟 |

**为什么使用 14-bit 二进制补码？**

AD9744 的 14-bit 数据用二进制补码表示正负电压：
- `0x1FFF` (+8191) → 最大正电压
- `0x0000` (0) → 零点 (中点)
- `0x2000` (-8192) → 最大负电压

```
14-bit 补码数据范围:

  0x1FFF = 011 1111 1111 1111 = +8191  (正峰值)
  0x0001 = 000 0000 0000 0001 = +1
  0x0000 = 000 0000 0000 0000 = 0      (零交叉点)
  0x3FFF = 111 1111 1111 1111 = -1
  0x2000 = 010 0000 0000 0000 = -8192  (负峰值)
```

**AD9744 并行接口时序 (简化)：**

```
         ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐
CLK  ────┘   └───┘   └───┘   └───┘   └───┘   └───
         ┌───────────┐ ┌───────────┐ ┌───────────┐
D[13:0]──┤  Sample 0 ├─┤  Sample 1 ├─┤  Sample 2 ├─
         └───────────┘ └───────────┘ └───────────┘

→ 每个 CLK 上升沿锁存 14-bit 数据
→ 200MHz CLK = 每秒输出 200M 个采样点
```

**在 TinyAWG 中，PS 端通过 AXI 总线将波形参数写入 PL 端 BRAM，PL 端 DDS 引擎自动从 BRAM 读取波形数据送给 AD9744。PS 端不需要直接操作 AD9744 的并行接口。**

**步骤 Steps:**
1. 阅读 AD9744 数据手册第 5-7 页 (数据格式和时序)
2. 理解 14-bit 补码如何表示正弦波数据
3. 用计算器验证：正弦波峰值 0x1FFF 对应的十进制值

**预期结果 Expected Result:**
- 理解 AD9744 的数据格式和时序要求
- 能手算正弦波采样点的 14-bit 补码值

**常见问题 Common Issues:**
- "为什么不用无符号数？" -- 补码表示可以让 0 对应输出中点，正负对称，方便后续运放电路处理
- "14-bit 数据怎么对齐到 16-bit 总线？" -- 高位对齐：`data[15:2] = dac_value[13:0]`，低 2 位填 0

### 任务 6.2: DAC8562 精密 DAC 驱动 (45 分钟)

**DAC8562 的作用：** 它是一个 16-bit 双通道 DAC，通过 SPI 接受 PS 端控制。通道 A 控制输出波形的**幅度** (增益)，通道 B 控制输出波形的**直流偏置**。

**DAC8562 SPI 时序：**

```
SPI 模式: CPOL=0, CPHA=1 (Mode 1)
帧格式: 24-bit (8-bit 命令 + 16-bit 数据)

  CS ──┐                                                         ┌──
       └─────────────────────────────────────────────────────────┘
  SCLK ──┐  ┐  ┐  ┐  ┐  ┐  ┐  ┐  ┐  ┐  ┐  ┐  ┐  ┐  ┐  ┐  ┐  ┐  ┐  ┐  ┐  ┐  ┐
       ──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └─ ... ──┘  └──┘  └──┘
  DIN ──┤ CMD[7:0] │   DB[15:0]                             │
```

```c
// DAC8562/dac8562.c — DAC8562 双通道精密 DAC 驱动
// DAC8562 dual-channel precision DAC driver

#include "xspips.h"
#include "xil_printf.h"

// DAC8562 SPI 命令定义
#define DAC8562_CMD_WRITE_BUF_A         0x00    // 写入缓冲区 A (幅度)
#define DAC8562_CMD_WRITE_BUF_B         0x01    // 写入缓冲区 B (偏置)
#define DAC8562_CMD_LOAD_BUF_A          0x02    // 加载缓冲区 A 到 DAC A
#define DAC8562_CMD_LOAD_BUF_B          0x03    // 加载缓冲区 B 到 DAC B
#define DAC8562_CMD_WRITE_LOAD_A        0x06    // 写入并加载 DAC A
#define DAC8562_CMD_WRITE_LOAD_B        0x07    // 写入并加载 DAC B
#define DAC8562_CMD_WRITE_LOAD_ALL      0x0F    // 写入并加载双通道
#define DAC8562_CMD_POWER_UP_RESET      0x05    // 上电复位
#define DAC8562_CMD_INTERNAL_REF_EN     0x0D    // 使能内部参考电压 (2.5V)

// DAC8562 使用 PS SPI1 (与 WS2812 共享 SPI 总线，不同的 CS 片选)
extern XSpiPs spi;    // 复用 Day 5 的 SPI 实例

#define DAC8562_CS_PIN      55    // EMIO 1 — DAC8562 片选

/**
 * 发送 24-bit 命令到 DAC8562
 * Send 24-bit command to DAC8562
 *
 * @param cmd    8-bit 命令字节
 * @param data   16-bit 数据
 */
void dac8562_write_cmd(uint8_t cmd, uint16_t data)
{
    uint8_t tx_buf[3];

    tx_buf[0] = cmd;
    tx_buf[1] = (data >> 8) & 0xFF;    // 数据高字节
    tx_buf[2] = data & 0xFF;            // 数据低字节

    // 拉低 CS (片选 DAC8562)
    // 注意：使用 GPIO 控制 CS 而非 SPI 硬件 CS
    XGpioPs_WritePin(&gpio, DAC8562_CS_PIN, 0);

    // 发送 3 字节
    XSpiPs_PolledTransfer(&spi, tx_buf, NULL, 3);

    // 拉高 CS (释放)
    XGpioPs_WritePin(&gpio, DAC8562_CS_PIN, 1);
}

/**
 * 初始化 DAC8562
 * Initialize DAC8562: power-up and enable internal reference
 */
void dac8562_init(void)
{
    // 上电复位
    dac8562_write_cmd(DAC8562_CMD_POWER_UP_RESET, 0x0000);
    usleep(1000);    // 等待复位完成

    // 使能内部 2.5V 参考电压
    dac8562_write_cmd(DAC8562_CMD_INTERNAL_REF_EN, 0x0000);
    usleep(50000);   // 等待参考电压稳定 (~50ms)

    // 初始化输出为中间值 (零偏置, 中等幅度)
    dac8562_set_amplitude(0x8000);    // 50% 幅度
    dac8562_set_offset(0x8000);       // 零偏置

    xil_printf("DAC8562 initialized OK\r\n");
}

/**
 * 设置输出幅度 (通道 A)
 * Set output amplitude (Channel A)
 *
 * @param value  16-bit 值 (0x0000=最小, 0xFFFF=最大)
 */
void dac8562_set_amplitude(uint16_t value)
{
    dac8562_write_cmd(DAC8562_CMD_WRITE_LOAD_A, value);
}

/**
 * 设置输出直流偏置 (通道 B)
 * Set output DC offset (Channel B)
 *
 * @param value  16-bit 值 (0x0000=最大负偏置, 0x8000=零偏置, 0xFFFF=最大正偏置)
 */
void dac8562_set_offset(uint16_t value)
{
    dac8562_write_cmd(DAC8562_CMD_WRITE_LOAD_B, value);
}
```

**步骤 Steps:**
1. 创建 `DAC8562/dac8562.c` 和 `DAC8562/dac8562.h`
2. 实现 DAC8562 初始化和通道控制函数
3. 在 main() 中测试：

```c
dac8562_init();

// 测试：逐步增大幅度
xil_printf("DAC8562 amplitude sweep test\r\n");
for (uint16_t amp = 0x0000; amp <= 0xFFFF; amp += 0x1000)
{
    dac8562_set_amplitude(amp);
    xil_printf("Amplitude: 0x%04X\r\n", amp);
    usleep(500000);
}
```

4. 用万用表测量 DAC8562 输出引脚电压，确认电压随设定值线性变化

**预期结果 Expected Result:**
- DAC8562 初始化成功，串口无报错
- 通道 A 输出电压随设定值从 0V 增加到约 2.5V (内部参考电压)

**常见问题 Common Issues:**
- "输出电压始终为 0" -- 检查 SPI 模式 (DAC8562 需要 CPOL=0, CPHA=1)；CS 引脚是否正确
- "电压只到一半" -- 内部参考电压未使能，确认 `DAC8562_CMD_INTERNAL_REF_EN` 已发送
- "两个通道互相影响" -- 确认使用 `WRITE_LOAD_A/B` 命令而非 `WRITE_BUF` 命令

---

## 下午: 波形生成与输出控制 | Afternoon: Waveform Generation & Output Control

### 为什么要学这个? | Why Learn This?

DAC 芯片负责把数字变成模拟信号，但"变成什么波形"由软件决定。你需要编写函数来生成各种标准波形的采样数据：正弦波、方波、三角波、锯齿波，以及噪声。这些数据将被写入 PL 端 BRAM，DDS 引擎按 200MSa/s 速率送到 AD9744 输出。

The DAC chip converts digital to analog, but "what waveform to generate" is decided by software. You need to write functions that generate sample data for various standard waveforms: sine, square, triangle, ramp, and noise. This data will be written to PL-side BRAM, and the DDS engine sends it to the AD9744 at 200MSa/s.

### 任务 6.3: 波形数据生成函数 (45 分钟)

**波形数据存储在 BRAM 中，每个波形一个周期包含 1024 个采样点，每个点 14-bit。**

```c
// output_ctrl/waveform_gen.c — 波形数据生成
// Waveform data generation module

#include <math.h>
#include <stdlib.h>
#include "xil_printf.h"

// 波形查找表大小 (一个周期的采样点数)
#define WAVEFORM_LUT_SIZE   1024

// 14-bit 补码的范围
#define DAC_14BIT_MAX       8191    // 0x1FFF
#define DAC_14BIT_MIN       (-8192) // 0x2000
#define DAC_14BIT_MID       0

// 波形类型枚举
typedef enum {
    WAVE_SINE     = 0,    // 正弦波
    WAVE_SQUARE   = 1,    // 方波
    WAVE_TRIANGLE = 2,    // 三角波
    WAVE_RAMP     = 3,    // 锯齿波
    WAVE_NOISE    = 4,    // 白噪声
    WAVE_DC       = 5,    // 直流
} WaveType;

/**
 * 生成正弦波查找表
 * Generate sine wave lookup table
 *
 * @param table  输出缓冲区 (int16_t, 14-bit 补码)
 * @param size   表大小 (通常 1024)
 */
void gen_sine(int16_t *table, int size)
{
    for (int i = 0; i < size; i++)
    {
        // 角度 0 ~ 2*PI
        double angle = 2.0 * M_PI * i / size;

        // sin() 结果 -1.0 ~ +1.0，映射到 14-bit 补码范围
        double sample = sin(angle);
        table[i] = (int16_t)(sample * DAC_14BIT_MAX);

        // 限制范围 (防止浮点误差导致溢出)
        if (table[i] > DAC_14BIT_MAX) table[i] = DAC_14BIT_MAX;
        if (table[i] < DAC_14BIT_MIN) table[i] = DAC_14BIT_MIN;
    }
}

/**
 * 生成方波查找表
 * Generate square wave lookup table
 */
void gen_square(int16_t *table, int size)
{
    for (int i = 0; i < size; i++)
    {
        // 前半周期正，后半周期负
        table[i] = (i < size / 2) ? DAC_14BIT_MAX : DAC_14BIT_MIN;
    }
}

/**
 * 生成三角波查找表
 * Generate triangle wave lookup table
 */
void gen_triangle(int16_t *table, int size)
{
    for (int i = 0; i < size; i++)
    {
        // 前半周期线性上升，后半周期线性下降
        if (i < size / 2)
        {
            // -MAX → +MAX
            table[i] = (int16_t)((2.0 * DAC_14BIT_MAX * i / (size / 2)) - DAC_14BIT_MAX);
        }
        else
        {
            // +MAX → -MAX
            table[i] = (int16_t)((2.0 * DAC_14BIT_MAX * (size - i) / (size / 2)) - DAC_14BIT_MAX);
        }
    }
}

/**
 * 生成锯齿波查找表
 * Generate sawtooth (ramp) wave lookup table
 */
void gen_ramp(int16_t *table, int size)
{
    for (int i = 0; i < size; i++)
    {
        // 从 -MAX 线性上升到 +MAX
        table[i] = (int16_t)(DAC_14BIT_MIN +
                   (int32_t)(DAC_14BIT_MAX - DAC_14BIT_MIN) * i / (size - 1));
    }
}

/**
 * 生成白噪声查找表 (伪随机)
 * Generate white noise lookup table (pseudo-random)
 */
void gen_noise(int16_t *table, int size)
{
    // 简单的线性反馈移位寄存器 (LFSR) 伪随机数生成
    uint32_t lfsr = 0xACE1u;    // 种子值

    for (int i = 0; i < size; i++)
    {
        // LFSR: 16-bit, taps at bits 0, 2, 3, 5
        uint32_t bit = ((lfsr >> 0) ^ (lfsr >> 2) ^
                        (lfsr >> 3) ^ (lfsr >> 5)) & 1u;
        lfsr = (lfsr >> 1) | (bit << 15);

        // 映射到 14-bit 范围
        table[i] = (int16_t)((int32_t)lfsr * (DAC_14BIT_MAX - DAC_14BIT_MIN) / 65536
                              + DAC_14BIT_MIN);
    }
}

/**
 * 根据波形类型生成查找表
 * Generate LUT based on waveform type
 */
void gen_waveform(WaveType type, int16_t *table, int size)
{
    switch (type)
    {
        case WAVE_SINE:     gen_sine(table, size);     break;
        case WAVE_SQUARE:   gen_square(table, size);   break;
        case WAVE_TRIANGLE: gen_triangle(table, size); break;
        case WAVE_RAMP:     gen_ramp(table, size);     break;
        case WAVE_NOISE:    gen_noise(table, size);    break;
        case WAVE_DC:
        default:
            for (int i = 0; i < size; i++) table[i] = 0;
            break;
    }
}
```

**步骤 Steps:**
1. 创建 `output_ctrl/waveform_gen.c` 和 `output_ctrl/waveform_gen.h`
2. 实现所有波形生成函数
3. 在 main() 中生成正弦波并打印前 16 个采样点验证：

```c
int16_t waveform_lut[WAVEFORM_LUT_SIZE];

xil_printf("--- Waveform Generation Test ---\r\n");

// 生成正弦波
gen_sine(waveform_lut, WAVEFORM_LUT_SIZE);
xil_printf("Sine wave (first 16 samples):\r\n");
for (int i = 0; i < 16; i++)
{
    xil_printf("  [%3d] = %6d (0x%04X)\r\n",
               i, waveform_lut[i],
               (uint16_t)waveform_lut[i] & 0x3FFF);
}
```

**预期结果 Expected Result:**
- 正弦波第一个点接近 0 (零交叉)
- 第 256 个点接近 +8191 (正峰值, 1/4 周期)
- 第 512 个点接近 0 (零交叉, 1/2 周期)
- 第 768 个点接近 -8192 (负峰值, 3/4 周期)

**常见问题 Common Issues:**
- "正弦波第一个点不是 0" -- `sin(0) = 0`，但由于浮点精度，可能是一个很小的值，这是正常的
- "三角波/锯齿波在过零点有跳变" -- 检查整数除法的取整方向，可能差 1 个计数
- "噪声每次结果相同" -- LFSR 种子相同，可改为用定时器计数器做种子

### 任务 6.4: 输出控制模块 (45 分钟)

**输出控制模块负责：**
1. 将波形 LUT 写入 PL 端 BRAM (通过 AXI 总线)
2. 设置 DDS 频率控制字 (FTW, Frequency Tuning Word)
3. 控制输出继电器使能/禁用
4. 通过 DAC8562 设置幅度和偏置

```c
// output_ctrl/output_ctrl.c — 输出控制模块
// Output control module

#include "xil_io.h"        // AXI 寄存器读写
#include "xil_printf.h"
#include "gpio.h"
#include "dac8562.h"
#include "waveform_gen.h"

// PL 端 AXI Lite 寄存器地址 (从 Vivado 地址编辑器获取)
// PL-side AXI Lite register addresses (from Vivado Address Editor)
#define DDS_BASE_ADDR          0x40000000

#define DDS_REG_CTRL           (DDS_BASE_ADDR + 0x00)  // 控制寄存器
#define DDS_REG_FTW            (DDS_BASE_ADDR + 0x04)  // 频率控制字
#define DDS_REG_AMPLITUDE      (DDS_BASE_ADDR + 0x08)  // 幅度缩放
#define DDS_REG_OFFSET         (DDS_BASE_ADDR + 0x0C)  // 偏置
#define DDS_REG_WAVE_TYPE      (DDS_BASE_ADDR + 0x10)  // 波形类型

// BRAM 基地址 (存储波形查找表)
#define BRAM_BASE_ADDR         0x40010000
#define BRAM_SIZE              WAVEFORM_LUT_SIZE        // 1024 个 16-bit 字

/**
 * 将波形查找表写入 PL 端 BRAM
 * Write waveform LUT to PL-side BRAM
 */
void write_waveform_to_bram(int16_t *table, int size)
{
    for (int i = 0; i < size && i < BRAM_SIZE; i++)
    {
        // 每个地址存储一个 16-bit 采样值
        // BRAM 宽度 32-bit，低 16-bit 为有效数据
        Xil_Out32(BRAM_BASE_ADDR + i * 4,
                  (uint32_t)(table[i] & 0x3FFF));
    }
}

/**
 * 计算频率控制字 (FTW)
 * Calculate Frequency Tuning Word
 *
 * FTW = (目标频率 / 系统时钟频率) × 2^N
 * N = 相位累加器位数 (32-bit)
 * 系统时钟 = 200 MHz (PL 端 DDS 时钟)
 *
 * @param freq_hz  目标频率 (Hz)
 * @return         32-bit 频率控制字
 */
uint32_t calculate_ftw(double freq_hz)
{
    double fclk = 200000000.0;    // 200 MHz
    uint32_t ftw = (uint32_t)((freq_hz / fclk) * 4294967296.0);

    // 限制范围
    if (ftw > 0x7FFFFFFF) ftw = 0x7FFFFFFF;
    if (ftw == 0 && freq_hz > 0) ftw = 1;    // 最小非零频率

    return ftw;
}

/**
 * 设置输出频率
 * Set output frequency
 *
 * @param freq_hz  频率 (Hz), 范围 1 ~ 20000000 (20MHz)
 */
void set_frequency(double freq_hz)
{
    uint32_t ftw = calculate_ftw(freq_hz);
    Xil_Out32(DDS_REG_FTW, ftw);
    xil_printf("Set freq: %.1f Hz, FTW=0x%08X\r\n", freq_hz, ftw);
}

/**
 * 启动输出
 * Start waveform output
 *
 * @param type     波形类型
 * @param freq_hz  频率 (Hz)
 * @param enable   1=使能输出, 0=禁用输出
 */
void output_start(WaveType type, double freq_hz, int enable)
{
    // 1. 生成波形数据
    int16_t table[WAVEFORM_LUT_SIZE];
    gen_waveform(type, table, WAVEFORM_LUT_SIZE);

    // 2. 写入 BRAM
    write_waveform_to_bram(table, WAVEFORM_LUT_SIZE);

    // 3. 设置波形类型
    Xil_Out32(DDS_REG_WAVE_TYPE, (uint32_t)type);

    // 4. 设置频率
    set_frequency(freq_hz);

    // 5. 设置默认幅度和偏置
    dac8562_set_amplitude(0x8000);    // 50% 幅度
    dac8562_set_offset(0x8000);       // 零偏置

    // 6. 控制继电器
    gpio_set_relay(enable);

    // 7. 启动 DDS
    Xil_Out32(DDS_REG_CTRL, enable ? 1 : 0);

    xil_printf("Output: wave=%d freq=%.1fHz enable=%d\r\n",
               type, freq_hz, enable);
}
```

**步骤 Steps:**
1. 创建 `output_ctrl/output_ctrl.c` 和 `output_ctrl/output_ctrl.h`
2. 实现 BRAM 写入、FTW 计算、输出控制函数
3. 在 main() 中测试输出 1kHz 正弦波：

```c
// 初始化所有模块
gpio_init();
spi_init();
i2c_init();
dac8562_init();

xil_printf("=== TinyAWG Output Test ===\r\n");

// 输出 1kHz 正弦波
output_start(WAVE_SINE, 1000.0, 1);

xil_printf("Outputting 1kHz sine wave...\r\n");
xil_printf("Connect oscilloscope to BNC output!\r\n");

while (1)
{
    // 保持输出
    usleep(1000000);
}
```

**预期结果 Expected Result:**
- BRAM 写入成功，无总线错误
- FTW 计算正确：1kHz / 200MHz × 2^32 = 0x0000147B
- 继电器闭合 (输出使能)

**常见问题 Common Issues:**
- "BRAM 写入导致 Data Abort" -- AXI 地址映射不正确，确认 Vivado 地址编辑器中的 BRAM 基地址
- "FTW 计算结果为 0" -- `freq_hz / fclk` 结果太小被截断为 0，确认使用 `double` 类型计算
- "继电器不动作" -- 检查 EMIO 是否在 Vivado 中正确连接到物理引脚

### 任务 6.5: 示波器验证 (30 分钟)

**测试矩阵：**

| 测试项 Test | 波形 Waveform | 频率 Frequency | 预期示波器显示 Expected |
|------------|-------------|---------------|----------------------|
| 1 | 正弦波 Sine | 1 kHz | 平滑正弦，1ms 周期 |
| 2 | 正弦波 Sine | 10 kHz | 正弦波，100us 周期 |
| 3 | 方波 Square | 1 kHz | 陡峭方波，占空比 50% |
| 4 | 三角波 Triangle | 1 kHz | 线性上升下降 |
| 5 | 锯齿波 Ramp | 5 kHz | 线性上升后跳变 |
| 6 | 正弦波 Sine | 100 kHz | 正弦波，10us 周期 |

**步骤 Steps:**
1. 将示波器探头连接到 TinyAWG 的 BNC 输出端
2. 依次输出上表中的波形，用示波器观察
3. 记录每种波形的频率误差和波形质量
4. 调节 DAC8562 幅度和偏置，观察输出变化

```c
// 自动测试序列
xil_printf("=== Automated Waveform Test ===\r\n");

struct {
    WaveType type;
    double freq;
    const char *name;
} tests[] = {
    { WAVE_SINE,     1000.0,   "1kHz Sine"     },
    { WAVE_SINE,     10000.0,  "10kHz Sine"    },
    { WAVE_SQUARE,   1000.0,   "1kHz Square"   },
    { WAVE_TRIANGLE, 1000.0,   "1kHz Triangle" },
    { WAVE_RAMP,     5000.0,   "5kHz Ramp"     },
    { WAVE_SINE,     100000.0, "100kHz Sine"   },
};

for (int i = 0; i < 6; i++)
{
    xil_printf("\r\nTest %d: %s\r\n", i + 1, tests[i].name);
    output_start(tests[i].type, tests[i].freq, 1);
    xil_printf("Press key to continue...\r\n");
    usleep(5000000);    // 等待 5 秒观察
}
```

**预期结果 Expected Result:**
- 示波器显示清晰的波形，与预期形状一致
- 频率误差 < 1% (DDS 频率精度取决于 FTW 量化误差)
- 幅度可通过 DAC8562 调节

**常见问题 Common Issues:**
- "波形有毛刺/抖动" -- PL 端 DDS 时钟抖动，或 BRAM 数据未完全写入就开始输出
- "高频波形失真" -- AD9744 输出后的模拟滤波器截止频率不够高
- "方波边沿变圆" -- 运放带宽限制，OPA2673 的压摆率不足，这是正常现象

---

## 今日作业 | Homework

1. **计算题：** 计算 FTW 的量化误差。当 FTW = 0x0000147B 时，实际输出频率是多少？与目标 1kHz 的误差是多少 ppm？
2. **代码题：** 为波形生成添加"任意波形" (ARB) 功能——从串口接收 1024 个采样点数据，写入 BRAM 输出
3. **实验题：** 用示波器测试不同频率下的波形失真度，记录从哪个频率开始波形明显失真
4. **写实验日志：** 记录 DAC 驱动调试过程，特别是 DAC8562 SPI 时序问题和 AD9744 数据对齐问题

---

## 明日预告 | Tomorrow's Preview

今天你让 TinyAWG 真正"发出声音"了！但操作全靠代码和串口，用户体验不好。明天我们将搭建 LVGL 图形界面和触摸驱动——给 TinyAWG 装上"眼睛" (2.8" 显示屏) 和"手指" (FT6336 触控)，让用户通过触摸屏来选择波形、调节频率和幅度！

Today TinyAWG is finally "making sounds"! But everything is done through code and serial port — poor user experience. Tomorrow we'll build the LVGL GUI and touch driver — giving TinyAWG "eyes" (2.8" display) and "fingers" (FT6336 touch), so users can select waveforms and adjust frequency and amplitude through the touchscreen!

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| AD9744 数据手册 | PDF | 搜索 "AD9744 datasheet" |
| DAC8562 数据手册 | PDF | 搜索 "DAC8562 datasheet" |
| OPA2673 数据手册 | PDF | 搜索 "OPA2673 datasheet" |
| DDS 原理与频率控制字计算 | 文章 | 搜索 "DDS 频率控制字 计算" |
| ZYNQ AXI BRAM 读写 | Xilinx Doc | Xilinx PG078 (AXI BRAM Controller) |
| Xil_Out32 / Xil_In32 API | Xilinx Doc | Vitis 帮助文档 > xil_io |

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
