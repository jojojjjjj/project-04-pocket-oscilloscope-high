# Day 08: 频率测量与自动量程 | Frequency Measurement & Auto-Ranging

> **今日目标 Today's Objectives:**
> - 实现周期法信号频率测量
> - 理解频率测量精度与采样参数的关系
> - 实现信号幅度 (Vpp) 测量
> - 实现自动量程功能（根据信号自动调整时基和电压量程）
>
> **产出 Deliverable:** 示波器能显示信号频率和 Vpp，自动量程功能正常工作

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 | Phase 2 中期回顾 + 今日目标 |
| 09:30-10:30 | 知识讲解 | 频率测量方法与精度分析 |
| 10:30-10:45 | 休息 | Break |
| 10:45-12:00 | 动手实验 | 周期法测频实现 |
| 12:00-13:30 | 午餐 | Lunch + Break |
| 13:30-15:00 | 动手实验 | Vpp 测量 + 参数显示 |
| 15:00-15:15 | 休息 | Break |
| 15:15-16:30 | 动手实验 | 自动量程功能实现 |
| 16:30-17:00 | 回顾 | Phase 2 总结 + Phase 3 预告 |

---

## 上午: 频率测量 | Morning: Frequency Measurement

### 为什么要学这个? | Why Learn This?

示波器不只是"看"波形，更要"测量"波形。工程师使用示波器最常见的操作之一就是测量信号频率——从检查时钟晶振是否正常 (32.768kHz)，到验证 PWM 信号频率 (1kHz, 20kHz)，再到调试通信协议 (9600 baud, 115200 baud)。

An oscilloscope doesn't just "show" waveforms -- it "measures" them. One of the most common operations is measuring signal frequency -- from checking crystal oscillators (32.768kHz), to verifying PWM signal frequency (1kHz, 20kHz), to debugging communication protocols (9600 baud, 115200 baud).

### 任务 8.1: 周期法测频原理 (30 分钟)

**频率测量的两种基本方法：**

| 方法 Method | 原理 Principle | 适用场景 |
|-----------|---------------|---------|
| 频率计数法 | 在固定时间 (1s) 内计数脉冲个数 | 高频信号 (> 10kHz) |
| 周期测量法 | 测量一个周期的时间，计算频率 | 低频信号 (< 100kHz) |

对于示波器，我们使用**周期测量法**（因为我们已经采集了完整的波形数据）：

```
频率 = 1 / 周期
周期 = 两个上升沿之间的间隔时间
     = 两个上升沿之间的采样点数 × 采样周期
     = 点数 / 采样率
```

**精度分析：**

频率测量精度取决于：
- **采样率：** 越高越好，每个周期的点数越多
- **量化误差：** 最大 ±1 个采样点的误差
- **信号噪声：** 噪声会导致过零点抖动

```
相对精度 ≈ 1 / (每周期采样点数)
         = 信号频率 / 采样率

例如：1kHz 信号, 1Msps → 1000 点/周期 → 精度 0.1%
例如：100kHz 信号, 1Msps → 10 点/周期 → 精度 10% (较差)
```

### 任务 8.2: 实现周期法测频 (45 分钟)

```c
// waveform.c - 波形分析模块
#include "waveform.h"
#include <math.h>

/**
 * 测量信号频率 (周期法)
 * Measure signal frequency using period method
 *
 * @param buffer      ADC 数据缓冲区
 * @param size        缓冲区大小
 * @param sample_rate 采样率 (Hz)
 * @return            测量频率 (Hz)，0 表示测量失败
 */
float measure_frequency(uint16_t *buffer, uint16_t size, uint32_t sample_rate)
{
    // 1. 计算信号中心值
    uint16_t mid = 0;
    uint32_t sum = 0;
    for (int i = 0; i < size; i++) sum += buffer[i];
    mid = sum / size;

    // 2. 找到所有上升过零点
    uint16_t crossings[20];    // 最多记录 20 个过零点
    uint16_t num_crossings = 0;

    for (int i = 1; i < size && num_crossings < 20; i++)
    {
        if (buffer[i - 1] < mid && buffer[i] >= mid)
        {
            // 线性插值，提高精度
            float frac = (float)(mid - buffer[i - 1]) / (buffer[i] - buffer[i - 1]);
            crossings[num_crossings++] = i;  // 简化：使用整数索引
        }
    }

    // 3. 计算平均周期
    if (num_crossings < 2) return 0;    // 过零点不足，无法测量

    uint32_t total_samples = 0;
    for (int i = 1; i < num_crossings; i++)
    {
        total_samples += crossings[i] - crossings[i - 1];
    }

    float avg_period_samples = (float)total_samples / (num_crossings - 1);
    float frequency = (float)sample_rate / avg_period_samples;

    return frequency;
}

/**
 * 测量信号峰峰值 (Vpp)
 * Measure peak-to-peak voltage
 *
 * @param buffer  ADC 数据缓冲区
 * @param size    缓冲区大小
 * @return        峰峰值 (V)
 */
float measure_vpp(uint16_t *buffer, uint16_t size)
{
    uint16_t min_val = 4095;
    uint16_t max_val = 0;

    for (int i = 0; i < size; i++)
    {
        if (buffer[i] < min_val) min_val = buffer[i];
        if (buffer[i] > max_val) max_val = buffer[i];
    }

    float v_min = min_val * 3.3f / 4095.0f;
    float v_max = max_val * 3.3f / 4095.0f;

    return v_max - v_min;
}

/**
 * 测量信号平均值 (直流偏置)
 * Measure signal DC offset
 */
float measure_dc_offset(uint16_t *buffer, uint16_t size)
{
    uint32_t sum = 0;
    for (int i = 0; i < size; i++) sum += buffer[i];
    return (float)sum / size * 3.3f / 4095.0f;
}
```

**步骤 Steps:**
1. 创建 `waveform.c` 和 `waveform.h`
2. 实现 `measure_frequency()` 函数
3. 在 main.c 中调用并显示结果

```c
// 在波形显示后计算并打印参数
float freq = measure_frequency(adc_buffer, SAMPLE_COUNT, current_sample_rate);
float vpp = measure_vpp(adc_buffer, SAMPLE_COUNT);

printf("Freq: %.1f Hz, Vpp: %.3f V\n", freq, vpp);

// 在 TFT 上显示 (简化版)
draw_info_text(freq, vpp);
```

4. 测试：输入已知频率信号，对比测量值和实际值

**预期结果 Expected Result:**
- 1kHz 信号：测量值 998~1002 Hz (误差 < 0.2%)
- 10kHz 信号：测量值 9.9~10.1 kHz (误差 < 1%)
- 100kHz 信号：测量值 90~110 kHz (误差约 10%，受采样率限制)

**常见问题 Common Issues:**
- "频率测量为 0" -- 过零点不足，可能是信号幅度太小或信号断开
- "频率跳动很大" -- 信号噪声导致过零点抖动，增加均值滤波
- "高频精度差" -- 采样率限制，每个周期采样点太少

### 任务 8.3: TFT 参数显示 (30 分钟)

在 TFT 屏幕底部或顶部显示测量参数：

```c
/**
 * 在屏幕上绘制测量参数
 * Draw measurement parameters on screen
 */
void draw_parameters(float freq, float vpp, const char *timebase_label)
{
    // 底部信息栏背景
    tft_fill_rect(0, SCREEN_HEIGHT - 20, SCREEN_WIDTH, 20, COLOR_BLUE);

    // 频率显示 (用简易数字绘制)
    char buf[32];
    if (freq > 1000)
        sprintf(buf, "%.2fkHz", freq / 1000.0);
    else
        sprintf(buf, "%.1fHz", freq);

    // Vpp 显示
    char vbuf[32];
    sprintf(vbuf, "%.2fVpp", vpp);

    // 时基显示
    // ... 使用简易字库绘制 ...

    // 简化版：通过串口输出参数
    printf("[%s] Freq=%s Vpp=%s\n", timebase_label, buf, vbuf);
}
```

**步骤 Steps:**
1. 实现参数显示函数
2. 在 TFT 底部绘制蓝色信息栏
3. 显示频率、Vpp、当前时基

---

## 下午: 自动量程 | Afternoon: Auto-Ranging

### 任务 8.4: 自动时基 (30 分钟)

**自动时基算法：**

根据测量到的信号频率，自动选择合适的时基档位，使得屏幕上显示 2~5 个完整周期：

```c
/**
 * 根据信号频率自动选择时基档位
 * Auto-select timebase based on signal frequency
 *
 * 目标：屏幕显示 2~5 个完整周期
 * 满屏时间 = 160 / sample_rate
 * 信号周期 = 1 / frequency
 * 每屏周期数 = 满屏时间 / 信号周期 = 160 * frequency / sample_rate
 */
void auto_set_timebase(float frequency)
{
    if (frequency <= 0) return;    // 无效频率

    // 目标：屏幕显示 3 个周期
    float target_periods = 3.0f;
    float target_full_time = target_periods / frequency;  // 期望满屏时间 (秒)
    float target_full_time_us = target_full_time * 1e6f;  // 转换为微秒

    // 每格时间 = 满屏时间 / 8 格
    float target_us_per_div = target_full_time_us / 8.0f;

    // 找到最接近的时基档位
    int best_level = 0;
    float best_diff = 999999;

    for (int i = 0; i < 8; i++)
    {
        float diff = fabsf(timebase_table[i].us_per_div - target_us_per_div);
        if (diff < best_diff)
        {
            best_diff = diff;
            best_level = i;
        }
    }

    set_timebase(best_level);
}
```

**步骤 Steps:**
1. 实现 `auto_set_timebase()` 函数
2. 在每次测量频率后调用
3. 测试：改变信号频率，观察时基是否自动调整

**预期结果 Expected Result:**
- 输入 1kHz → 自动选择 200~500 us/div
- 输入 10kHz → 自动选择 20~50 us/div
- 输入 100kHz → 自动选择 10 us/div

### 任务 8.5: 自动电压量程 (30 分钟)

```c
/**
 * 根据信号幅度自动选择电压量程
 * Auto-select voltage range based on signal amplitude
 */
void auto_set_voltage_range(float vpp)
{
    if (vpp <= 0.001f) return;    // 无效信号

    // 目标：信号占屏幕 60%~80% 的高度
    float target_screen_vpp = 3.3f * 0.7f;    // 屏幕满量程的 70%
    float needed_gain = target_screen_vpp / vpp;

    // 限制增益范围
    if (needed_gain > 5.0f) needed_gain = 5.0f;
    if (needed_gain < 0.2f) needed_gain = 0.2f;

    // 更新 Y 轴缩放
    current_y_scale = needed_gain;

    // 或者切换运放增益 (如果有可编程增益)
    // set_amplifier_gain(needed_gain);
}
```

**步骤 Steps:**
1. 实现 `auto_set_voltage_range()` 函数
2. 根据 Vpp 测量值自动调整 Y 轴缩放
3. 测试不同幅度的信号

### 任务 8.6: 完整自动模式集成 (30 分钟)

```c
// 完整的自动模式主循环
uint8_t auto_mode = 1;    // 0=手动, 1=自动

while (1)
{
    if (buffer_full)
    {
        buffer_full = 0;

        // 1. 频率测量
        float freq = measure_frequency(adc_buffer, SAMPLE_COUNT, sample_rate);
        float vpp = measure_vpp(adc_buffer, SAMPLE_COUNT);

        // 2. 自动量程
        if (auto_mode && freq > 0)
        {
            auto_set_timebase(freq);
            auto_set_voltage_range(vpp);
        }

        // 3. 触发
        uint16_t trig_level = calculate_center_level(adc_buffer, SAMPLE_COUNT);
        int trig_point = find_rising_edge(adc_buffer, SAMPLE_COUNT, trig_level);

        // 4. 显示
        clear_waveform_area();
        if (trig_point >= 0)
            draw_waveform(&adc_buffer[trig_point], SAMPLE_COUNT - trig_point, COLOR_WAVE);
        else
            draw_waveform(adc_buffer, SAMPLE_COUNT, COLOR_WAVE);

        // 5. 参数显示
        draw_parameters(freq, vpp, timebase_table[current_timebase].label);

        // 6. 重新采集
        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, SAMPLE_COUNT);
    }

    // 按键处理
    if (scan_button(GPIOA, GPIO_PIN_0)) auto_mode = !auto_mode;  // 切换自动/手动
    if (scan_button(GPIOB, GPIO_PIN_0) && !auto_mode) timebase_increase();
}
```

---

## Phase 2 总结 | Phase 2 Summary

恭喜完成 Phase 2！你的示波器现在能够：

Congratulations on completing Phase 2! Your oscilloscope can now:

1. **稳定触发显示波形** -- 上升沿/下降沿/自动触发 (Day 5)
2. **TFT 屏幕实时显示** -- 带网格和坐标的波形界面 (Day 6)
3. **时基可调** -- 8 个档位，10us/div 到 10ms/div (Day 7)
4. **频率和幅度测量** -- 自动读取信号参数 (Day 8)
5. **自动量程** -- 根据信号自动调整显示 (Day 8)

**你已经拥有了一台基本可用的数字示波器！**

---

## 今日作业 | Homework

1. **精度测试：** 用已知频率的信号 (如 32.768kHz 晶振输出) 测量频率，计算误差百分比
2. **代码题：** 为频率测量添加滑动平均滤波，减少读数跳动
3. **思考题：** 自动量程为什么需要"迟滞"？没有迟滞会导致什么问题？
4. **Phase 2 报告：** 完成第二周进度报告 `assignments/week-2-checkin.md`

---

## 明日预告 | Tomorrow's Preview

Phase 3 开始！明天我们将添加信号发生器功能——用 AD9833 DDS 芯片产生正弦波、方波和三角波。有了信号发生器，你的示波器就能"自收自发"，自己产生测试信号然后自己测量！

Phase 3 begins! Tomorrow we add a signal generator -- using the AD9833 DDS chip to generate sine, square, and triangle waves. With the signal generator, your oscilloscope can "self-test" -- generate test signals and measure them!

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| (开源) 基于 STM32F103 的多功能仪器（含频率测量） | B站视频 | https://www.bilibili.com/video/BV1oPn2zZExh/ |
| 频率测量方法详解 | B站搜索 | 搜索"频率测量 周期法 MCU" |
| DSO138 频率测量实现 | GitHub | https://github.com/essboyer/DSO138 |
| 自动量程算法 | B站搜索 | 搜索"示波器 自动量程 算法" |

---

*最后更新：2026-05-26*
*Last updated: 2026-05-26*
