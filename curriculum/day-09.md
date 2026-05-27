# Day 09: 信号发生器 DDS | Signal Generator (DDS AD9833)

> **今日目标 Today's Objectives:**
> - 理解 DDS (直接数字频率合成) 的工作原理
> - 使用 SPI 驱动 AD9833 DDS 模块
> - 实现正弦波、方波、三角波的频率可调输出
> - 用示波器测量自己产生的信号（自收自发）
>
> **产出 Deliverable:** AD9833 输出 1kHz/10kHz 正弦波，示波器成功测量显示

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 | Phase 2 回顾 + Phase 3 目标 |
| 09:30-10:30 | 知识讲解 | DDS 原理与 AD9833 芯片介绍 |
| 10:30-10:45 | 休息 | Break |
| 10:45-12:00 | 动手实验 | AD9833 接线 + SPI 驱动 |
| 12:00-13:30 | 午餐 | Lunch + Break |
| 13:30-15:00 | 动手实验 | 波形生成与频率控制 |
| 15:00-15:15 | 休息 | Break |
| 15:15-16:30 | 动手实验 | 自收自发测试集成 |
| 16:30-17:00 | 回顾 | 今日总结 + 答疑 |

---

## 上午: DDS 原理与驱动 | Morning: DDS Principles & Driving

### 为什么要学这个? | Why Learn This?

到目前为止，测试示波器需要外接信号发生器。如果示波器能自己产生测试信号，就实现了"自收自发"——产生一个已知频率和幅度的信号，然后自己测量它，既能验证示波器的测量精度，又非常有趣！

So far, testing the oscilloscope requires an external signal generator. If the oscilloscope can generate its own test signals, it achieves "self-testing" -- generating a signal with known frequency and amplitude, then measuring it with itself. This verifies measurement accuracy and is very cool!

**DDS (Direct Digital Synthesis) 是什么？**

DDS 是一种用数字方式产生模拟波形的技术。核心思想：

1. **相位累加器：** 每个时钟周期，相位增加一个"步进值"
2. **查表：** 根据相位值，在 ROM 中查找对应的波形幅度值
3. **DAC 输出：** 将幅度值转换为模拟电压

```
频率 = 步进值 × 参考时钟频率 / 2^相位位数

AD9833:
  参考时钟 = 25MHz
  相位位数 = 28-bit
  最小频率分辨率 = 25MHz / 2^28 = 0.1 Hz
  最大输出频率 = 12.5 MHz (奈奎斯特)
```

### 任务 9.1: AD9833 接线与 SPI 配置 (45 分钟)

**AD9833 模块引脚：**

| AD9833 引脚 | 连接到 STM32 | 说明 |
|------------|-------------|------|
| VCC | 3.3V 或 5V | 电源 |
| GND | GND | 地 |
| DATA (DAT) | PB15 (SPI2_MOSI) | 串行数据 |
| CLK (SCK) | PB13 (SPI2_SCK) | 时钟 |
| FSYNC (CS) | PB12 | 片选 (低电平有效) |
| OUT | 信号输出 | 连接到示波器输入 |

> 注：使用 SPI2 而非 SPI1，因为 SPI1 已被 TFT 显示屏占用。
> Note: Use SPI2 instead of SPI1, since SPI1 is occupied by the TFT display.

**CubeMX 配置 SPI2：**
- SPI2: Full-Duplex Master
- Prescaler: 16 (4.5MHz，AD9833 最高 40MHz)
- Data Size: 16-bit
- CPOL: High, CPHA: 2 Edge (AD9833 要求)
- NSS: Software

**步骤 Steps:**
1. 配置 SPI2 和片选引脚 (PB12)
2. 接线：AD9833 连接到 SPI2
3. 编写 AD9833 初始化代码：

```c
// signal_gen.c - 信号发生器模块
#include "signal_gen.h"
#include "spi.h"

// AD9833 控制位定义
#define AD9833_B28      (1 << 13)    // 28-bit 频率控制
#define AD9833_HLB      (1 << 12)    // 高/低字节选择
#define AD9833_FSELECT  (1 << 11)    // 频率寄存器选择
#define AD9833_PSELECT  (1 << 10)    // 相位寄存器选择
#define AD9833_RESET    (1 << 8)     // 复位
#define AD9833_SLEEP1   (1 << 7)     // DAC 睡眠
#define AD9833_SLEEP12  (1 << 6)     // 内部时钟禁用
#define AD9833_OPBITEN  (1 << 5)     // 输出使能
#define AD9833_DIV2     (1 << 3)     // 方波分频
#define AD9833_MODE     (1 << 1)     // 三角波模式

// 频率寄存器地址
#define AD9833_FREQ0_REG  0x4000
#define AD9833_FREQ1_REG  0x8000
#define AD9833_PHASE0_REG 0xC000

#define F_MCLK  25000000UL    // AD9833 参考时钟 25MHz

/**
 * 通过 SPI 发送 16-bit 数据到 AD9833
 * Send 16-bit data to AD9833 via SPI
 */
void ad9833_write(uint16_t data)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);  // CS = 0
    HAL_SPI_Transmit(&hspi2, (uint8_t *)&data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);    // CS = 1
}

/**
 * 设置 AD9833 输出频率
 * Set AD9833 output frequency
 *
 * @param freq_hz  目标频率 (Hz)，范围 0.1Hz ~ 12.5MHz
 */
void ad9833_set_frequency(float freq_hz)
{
    // 计算频率寄存器值：freq_reg = freq * 2^28 / MCLK
    uint32_t freq_reg = (uint32_t)((freq_hz * (float)(1UL << 28)) / F_MCLK);

    uint16_t freq_low  = (uint16_t)(freq_reg & 0x3FFF) | AD9833_FREQ0_REG;
    uint16_t freq_high = (uint16_t)((freq_reg >> 14) & 0x3FFF) | AD9833_FREQ0_REG;

    // B28 模式：分两次写入 28-bit 频率值
    ad9833_write(AD9833_B28 | AD9833_RESET);
    ad9833_write(freq_low);
    ad9833_write(freq_high);
    ad9833_write(0x0000);    // 相位 = 0
    ad9833_write(AD9833_B28);    // 取消复位，开始输出
}

/**
 * 设置 AD9833 输出波形类型
 * Set AD9833 output waveform type
 *
 * @param type  波形类型：0=正弦波, 1=三角波, 2=方波
 */
void ad9833_set_waveform(uint8_t type)
{
    switch (type)
    {
    case 0:    // 正弦波
        ad9833_write(AD9833_B28);
        break;
    case 1:    // 三角波
        ad9833_write(AD9833_B28 | AD9833_MODE);
        break;
    case 2:    // 方波
        ad9833_write(AD9833_B28 | AD9833_OPBITEN | AD9833_DIV2);
        break;
    }
}

/**
 * AD9833 初始化
 * Initialize AD9833
 */
void signal_gen_init(void)
{
    ad9833_write(AD9833_RESET);    // 复位
    HAL_Delay(10);
    ad9833_set_frequency(1000);    // 默认 1kHz
    ad9833_set_waveform(0);        // 默认正弦波
}
```

4. 测试：初始化后，用万用表或示波器测量 AD9833 输出引脚

**预期结果 Expected Result:**
- AD9833 输出引脚有正弦波信号
- 频率约为 1kHz
- 幅度约 0.6Vpp (AD9833 输出幅度)

**常见问题 Common Issues:**
- "没有输出" -- 检查 SPI 接线，确认 CS 引脚正确
- "频率不对" -- 检查参考时钟是否为 25MHz
- "波形有毛刺" -- 电源去耦不良，在 VCC 和 GND 之间加 100nF 电容

---

## 下午: 波形控制与自收自发 | Afternoon: Waveform Control & Self-Testing

### 任务 9.2: 频率和波形切换 (30 分钟)

```c
// 信号发生器状态
typedef struct {
    float frequency;       // 输出频率 (Hz)
    uint8_t waveform;      // 波形类型: 0=正弦, 1=三角, 2=方波
} SignalGenConfig;

SignalGenConfig sig_gen = {
    .frequency = 1000.0f,
    .waveform = 0
};

// 预设频率列表
float freq_list[] = {100, 500, 1000, 2000, 5000, 10000, 50000, 100000};
#define FREQ_LIST_SIZE  (sizeof(freq_list) / sizeof(freq_list[0]))
volatile uint8_t freq_index = 2;    // 默认 1kHz

/**
 * 切换信号发生器频率
 */
void sig_gen_next_freq(void)
{
    freq_index = (freq_index + 1) % FREQ_LIST_SIZE;
    sig_gen.frequency = freq_list[freq_index];
    ad9833_set_frequency(sig_gen.frequency);
}

/**
 * 切换波形类型
 */
void sig_gen_next_waveform(void)
{
    sig_gen.waveform = (sig_gen.waveform + 1) % 3;
    ad9833_set_waveform(sig_gen.waveform);
}
```

**步骤 Steps:**
1. 实现频率和波形切换函数
2. 通过串口命令测试：
   - 发送 "F" → 切换频率
   - 发送 "W" → 切换波形
3. 用示波器测量不同设置下的输出

### 任务 9.3: 自收自发集成测试 (45 分钟)

**信号发生器输出幅度问题：**

AD9833 输出幅度约 0.6Vpp，太小了示波器不容易观察。解决方案：

1. **软件增益：** 在 ADC 端通过提高 Y 轴缩放来放大显示
2. **运放放大：** 用 LM358 将 AD9833 输出放大到 2Vpp

```
AD9833 (0.6Vpp) → [LM358 同相放大 x3] → (1.8Vpp) → [偏置 1.65V] → PA0 (ADC)
```

**集成测试步骤：**

```c
// 自收自发测试
void self_test(void)
{
    printf("=== Self Test Start ===\n");

    // 测试频率列表
    float test_freqs[] = {100, 1000, 5000, 10000};
    for (int i = 0; i < 4; i++)
    {
        ad9833_set_frequency(test_freqs[i]);
        ad9833_set_waveform(0);    // 正弦波
        HAL_Delay(100);            // 等待稳定

        // 采集并测量
        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, SAMPLE_COUNT);
        while (!buffer_full);
        buffer_full = 0;

        float measured_freq = measure_frequency(adc_buffer, SAMPLE_COUNT, sample_rate);
        float error = fabsf(measured_freq - test_freqs[i]) / test_freqs[i] * 100.0f;

        printf("Set: %.0f Hz, Measured: %.1f Hz, Error: %.1f%%\n",
               test_freqs[i], measured_freq, error);
    }

    printf("=== Self Test End ===\n");
}
```

**步骤 Steps:**
1. 将 AD9833 输出通过运放放大后连接到示波器输入 (PA0)
2. 运行自收自发测试
3. 记录不同频率下的测量误差
4. 在 TFT 屏幕上同时显示信号发生器输出波形和测量参数

**预期结果 Expected Result:**
- 100Hz ~ 10kHz 范围内，频率测量误差 < 5%
- TFT 屏幕显示清晰的波形和正确的频率读数
- 切换信号发生器波形时，示波器波形实时更新

### 任务 9.4: 按钮控制信号发生器 (30 分钟)

```c
// 使用额外的按键控制信号发生器
// PA0: 时基减小 / 自动模式切换
// PB0: 时基增大
// PA8 (新增): 信号发生器频率切换
// PB1 (新增): 信号发生器波形切换

while (1)
{
    // 示波器功能 (已有)
    if (buffer_full) { /* ... 显示波形 ... */ }

    // 信号发生器控制
    if (scan_button(GPIOA, GPIO_PIN_8))
    {
        sig_gen_next_freq();
        printf("Signal Gen: %.0f Hz\n", sig_gen.frequency);
    }
    if (scan_button(GPIOB, GPIO_PIN_1))
    {
        sig_gen_next_waveform();
        const char *wave_names[] = {"Sine", "Triangle", "Square"};
        printf("Signal Gen: %s\n", wave_names[sig_gen.waveform]);
    }
}
```

---

## 今日作业 | Homework

1. **实验题：** 测量 AD9833 输出的三种波形在不同频率下的幅度变化，制作幅度-频率曲线
2. **计算题：** AD9833 参考时钟 25MHz，要输出 1kHz 信号，频率寄存器值是多少？
3. **思考题：** DDS 输出的方波为什么会随着频率升高而失真？（提示：考虑 DAC 的更新速率）
4. **写实验日志：** 记录自收自发测试的结果和误差分析

---

## 明日预告 | Tomorrow's Preview

明天我们将学习 PCB 设计——把面包板上的电路变成一块精美的印刷电路板！你将使用嘉立创 EDA 画原理图和 PCB 布局，然后下单打板。

Tomorrow we learn PCB design -- turning the breadboard circuit into a beautiful printed circuit board! You will use JLCEDA to draw schematics and PCB layout, then order the PCB fabrication.

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| AD9833 DDS 模块使用教程 | B站搜索 | 搜索"AD9833 STM32 教程" |
| (开源) 多功能仪器含信号发生器 | B站视频 | https://www.bilibili.com/video/BV1oPn2zZExh/ |
| AD9833 数据手册 | PDF | 搜索"AD9833 datasheet" |
| DDS 原理讲解 | B站搜索 | 搜索"DDS 直接数字频率合成" |

---

*最后更新：2026-05-26*
*Last updated: 2026-05-26*
