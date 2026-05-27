# Day 04: STM32 ADC 配置与数据采集 | STM32 ADC Configuration & Data Acquisition

> **今日目标 Today's Objectives:**
> - 优化 ADC+DMA+TIM 配置，达到 1Msps 采样率
> - 理解采样时间、转换时间与采样率的关系
> - 实现定时器精确触发 ADC 采样
> - 通过串口将波形数据发送到 PC 端并用 Python 绘图验证
>
> **产出 Deliverable:** STM32 以 1Msps 采样率采集正弦波信号，PC 端 Python 绘制出清晰波形

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 | 回顾 Day 3 + 今日目标 |
| 09:30-10:30 | 知识讲解 | 采样率计算、定时器触发原理 |
| 10:30-10:45 | 休息 | Break |
| 10:45-12:00 | 动手实验 | 优化 ADC+DMA 配置 |
| 12:00-13:30 | 午餐 | Lunch + Break |
| 13:30-15:00 | 动手实验 | Python PC 端波形绘制 |
| 15:00-15:15 | 休息 | Break |
| 15:15-16:30 | 动手实验 | 多种信号测试与性能验证 |
| 16:30-17:00 | 回顾 | Phase 1 总结 + Phase 2 预告 |

---

## 上午: 优化 ADC 采样性能 | Morning: Optimizing ADC Sampling Performance

### 为什么要学这个? | Why Learn This?

昨天我们实现了 ADC+DMA 的基本采样，但采样率可能还不够高。在真正的示波器中，采样率直接决定了能测量的最高信号频率（带宽）。今天的任务是把采样率推到 STM32 内置 ADC 的极限。

Yesterday we achieved basic ADC+DMA sampling, but the sampling rate may not be high enough. In a real oscilloscope, the sampling rate directly determines the maximum signal frequency (bandwidth) that can be measured. Today's task is to push the sampling rate to the STM32 internal ADC's limit.

**STM32F103 ADC 采样率计算：**

```
采样率 = ADC时钟 / (采样时间 + 转换时间)
       = 12MHz / (1.5 + 12.5) cycles
       = 12MHz / 14
       ≈ 857ksps
```

要达到 1Msps，需要：
- ADC 时钟 = 14MHz (PCLK2/6 = 72MHz/6 = 12MHz，但可调)
- 采样时间 = 1.5 cycles (最快)
- 注意：ADC 时钟不能超过 14MHz

### 任务 4.1: 理解采样时间与采样率 (30 分钟)

**STM32 ADC 采样时间选项：**

| 采样周期 Sample Cycles | 采样时间 (12MHz) | 说明 |
|----------------------|-----------------|------|
| 1.5 cycles | 0.125us | 最快，精度略低 |
| 7.5 cycles | 0.625us | 较快 |
| 28.5 cycles | 2.375us | 中等 |
| 55.5 cycles | 4.625us | 较慢，精度更高 |
| 239.5 cycles | 19.958us | 最慢，精度最高 |

**选择策略：**
- 示波器模式：选 1.5 cycles（追求最高速度）
- 万用表模式：选 239.5 cycles（追求最高精度）

**步骤 Steps:**
1. 打开 CubeMX，配置 ADC 采样时间为 1.5 cycles
2. 确认 ADC 时钟配置为 12MHz (PCLK2 = 72MHz, ADC Prescaler = /6)
3. 计算实际采样率并记录

### 任务 4.2: 定时器精确触发 ADC (45 分钟)

**为什么用定时器触发？**

不使用定时器时，ADC 采样间隔取决于代码执行速度，不够精确。使用定时器触发可以：
- 精确控制采样率（误差 < 0.01%）
- CPU 完全不参与触发过程
- 可以随时调整采样率（修改定时器参数）

**Timer 3 配置计算：**

```
定时器时钟 = 72MHz
想要采样率 = 1Msps
计数器周期 = 72MHz / 1Msps - 1 = 71
```

所以：Prescaler = 0, Counter Period = 71 → 采样率 = 72MHz / 72 = 1MHz = 1Msps

**不同采样率对应的定时器配置：**

| 采样率 | Prescaler | Counter Period | 说明 |
|--------|-----------|---------------|------|
| 1 Msps | 0 | 71 | 最高采样率 |
| 500 ksps | 0 | 143 | |
| 200 ksps | 0 | 359 | |
| 100 ksps | 0 | 719 | |
| 50 ksps | 0 | 1439 | |
| 10 ksps | 71 | 999 | 较低采样率 |

**步骤 Steps:**

1. **CubeMX 配置：**
   - Timer 3 → Prescaler: 0, Counter Period: 71
   - Timer 3 → Trigger Event Selection: Update Event
   - ADC1 → External Trigger: Timer 3 TRGO Event
   - ADC1 → Continuous Conversion Mode: **Disabled** (由定时器触发，不需要连续模式)
   - DMA: Circular 模式，Half Word

2. **完整 CubeMX 配置清单：**
   ```
   RCC: HSE = Crystal
   Clock: 72MHz
   ADC1:
     - IN0 (PA0) 启用
     - Resolution: 12 bits
     - Sampling Time: 1.5 Cycles
     - External Trigger: Timer 3 TRGO
     - Continuous: Disabled
     - DMA Circular: Enabled
   TIM3:
     - Prescaler: 0
     - Counter Period: 71
     - Trigger Event: Update Event
   USART1:
     - 115200, 8N1
   ```

3. **生成代码，编写采样输出程序**

```c
/* USER CODE BEGIN 0 */
#define SAMPLE_COUNT 1024
uint16_t adc_buffer[SAMPLE_COUNT];
volatile uint8_t buffer_full = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    buffer_full = 1;
}
/* USER CODE END 0 */

// main 函数中:
/* USER CODE BEGIN 2 */
HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, SAMPLE_COUNT);
HAL_TIM_Base_Start(&htim3);  // 启动定时器触发
/* USER CODE END 2 */

/* USER CODE BEGIN WHILE */
while (1)
{
    if (buffer_full)
    {
        buffer_full = 0;

        // 输出数据
        printf("START\n");
        for (int i = 0; i < SAMPLE_COUNT; i++)
        {
            printf("%d\n", adc_buffer[i]);
        }
        printf("END\n");

        // 重新启动下一轮采集
        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, SAMPLE_COUNT);
    }
}
/* USER CODE END WHILE */
```

4. 连接信号发生器：1kHz 正弦波, 1Vpp, 偏置 1.65V
5. 编译下载，打开串口验证数据输出

**预期结果 Expected Result:**
- 串口输出 1024 个 ADC 采样值
- 数据呈现规则的正弦波形状
- 1kHz 信号在 1Msps 下每周期有 1000 个采样点

**常见问题 Common Issues:**
- "DMA 传输一次就停止" -- 确认 DMA 模式为 Circular，或在回调中重新启动
- "数据全是噪声" -- 检查信号调理电路，确保输入信号在 0~3.3V 范围内
- "波形不完整/截断" -- 可能是信号频率和采样深度不匹配，调整信号频率或采样点数

---

## 下午: Python 波形绘制与验证 | Afternoon: Python Waveform Visualization

### 任务 4.3: Python PC 端波形绘制 (60 分钟)

**步骤 Steps:**

1. **安装 Python 依赖：**

```bash
pip install pyserial matplotlib numpy
```

2. **编写 Python 程序：** 使用 `software/src/main.py` 中的模板代码

```python
# software/src/main.py
"""
口袋示波器 PC 端波形绘制工具
Pocket Oscilloscope PC Waveform Visualization Tool

功能：通过串口接收 STM32 的 ADC 采样数据，绘制波形图
"""

import serial
import matplotlib.pyplot as plt
import numpy as np
import sys

# === 配置参数 ===
SERIAL_PORT = 'COM3'          # 串口号，根据实际情况修改
BAUD_RATE = 115200            # 波特率，与 STM32 保持一致
SAMPLE_COUNT = 1024           # 采样点数
VREF = 3.3                    # ADC 参考电压

def read_waveform(ser):
    """从串口读取一组波形数据"""
    data = []
    reading = False

    while True:
        line = ser.readline().decode('utf-8').strip()
        if not line:
            continue

        if line == "START":
            data = []
            reading = True
            continue

        if line == "END":
            reading = False
            break

        if reading:
            try:
                data.append(int(line))
            except ValueError:
                continue

    return np.array(data)

def adc_to_voltage(adc_values):
    """将 ADC 数值转换为电压值"""
    return adc_values * VREF / 4095.0

def plot_waveform(adc_data, title="Oscilloscope Waveform"):
    """绘制波形图"""
    voltage = adc_to_voltage(adc_data)
    time_us = np.arange(len(adc_data))  # 采样点索引（1us间隔 @1Msps）

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 6))

    # ADC 原始值
    ax1.plot(time_us, adc_data, 'b-', linewidth=0.5)
    ax1.set_ylabel('ADC Value (0-4095)')
    ax1.set_title(title)
    ax1.grid(True, alpha=0.3)
    ax1.set_ylim(0, 4095)

    # 电压值
    ax2.plot(time_us, voltage, 'r-', linewidth=0.5)
    ax2.set_xlabel('Sample Index (1us each @1Msps)')
    ax2.set_ylabel('Voltage (V)')
    ax2.grid(True, alpha=0.3)
    ax2.set_ylim(0, VREF)

    plt.tight_layout()
    plt.show()

def measure_frequency(adc_data, sample_rate=1000000):
    """测量信号频率（过零法）"""
    # 找到中点值
    mid = np.mean(adc_data)

    # 找到所有上升过零点
    crossings = []
    for i in range(1, len(adc_data)):
        if adc_data[i-1] < mid and adc_data[i] >= mid:
            crossings.append(i)

    if len(crossings) < 2:
        return 0

    # 计算平均周期
    periods = np.diff(crossings)
    avg_period_samples = np.mean(periods)
    frequency = sample_rate / avg_period_samples

    return frequency

# === 主程序 ===
if __name__ == '__main__':
    port = sys.argv[1] if len(sys.argv) > 1 else SERIAL_PORT

    print(f"连接串口 {port}...")
    try:
        ser = serial.Serial(port, BAUD_RATE, timeout=2)
    except serial.SerialException:
        print(f"无法打开串口 {port}，请检查连接")
        sys.exit(1)

    print("等待波形数据...")
    data = read_waveform(ser)
    ser.close()

    print(f"接收到 {len(data)} 个采样点")
    print(f"ADC 范围: {np.min(data)} ~ {np.max(data)}")
    print(f"电压范围: {adc_to_voltage(np.min(data)):.3f}V ~ {adc_to_voltage(np.max(data)):.3f}V")

    freq = measure_frequency(data)
    if freq > 0:
        print(f"测量频率: {freq:.1f} Hz")

    plot_waveform(data)
```

3. **运行测试：**
   - 确保 STM32 正在发送数据
   - 运行：`python main.py COM3` (替换为实际串口号)

**预期结果 Expected Result:**
- Python 窗口显示双子图：上面是 ADC 原始值，下面是电压值
- 波形为清晰的正弦波
- 频率测量值与信号发生器设置一致 (误差 < 5%)

### 任务 4.4: 多种信号测试 (45 分钟)

**测试矩阵：**

| 信号类型 | 频率 | 幅度 | 预期 ADC 波形 |
|---------|------|------|-------------|
| 正弦波 | 1kHz | 1Vpp | 平滑正弦 |
| 正弦波 | 10kHz | 1Vpp | 较稀疏正弦 (每周期100点) |
| 正弦波 | 100kHz | 1Vpp | 非常稀疏 (每周期10点) |
| 方波 | 1kHz | 1Vpp | 陡峭的方波 |
| 三角波 | 1kHz | 1Vpp | 线性上升下降 |

**步骤 Steps:**
1. 依次用信号发生器产生上述信号
2. 用 Python 记录每种信号的波形截图
3. 分析不同频率下的波形质量
4. 记录最高能清晰采样的信号频率

**预期结果 Expected Result:**
- 1kHz 和 10kHz 信号能清晰显示
- 100kHz 信号开始出现混叠或不完整
- 这就是采样率限制带来的"带宽"概念

**关键观察：** 当信号频率接近采样率的一半时（奈奎斯特频率 = 500kHz），波形开始失真。实际可用带宽约为采样率的 1/5 ~ 1/10，即 100~200kHz。

---

## Phase 1 总结 | Phase 1 Summary

恭喜！经过 4 天的学习，你已经掌握了示波器的三大基础：

Congratulations! After 4 days, you have mastered the three foundations of the oscilloscope:

1. **模拟信号调理 (Day 2):** 运放电路将信号调整到 ADC 可接受的范围
2. **ADC 采样 (Day 3-4):** 将模拟电压转换为数字值
3. **DMA 数据搬运 (Day 3-4):** 高速、自动化的数据采集

**你已经能：**
- 配置 STM32 的 ADC+DMA+TIM 实现精确采样
- 用 Python 在 PC 端显示波形
- 理解采样率、带宽、分辨率等核心概念

**接下来 (Phase 2)，你将：**
- 学习波形触发逻辑（让波形稳定显示）
- 在 TFT 屏幕上绘制波形（真正的示波器界面）
- 实现时基和量程控制
- 测量信号频率

---

## 今日作业 | Homework

1. **实验题：** 用 Python 测量不同频率信号的测量误差，制作误差表
2. **计算题：** 如果将定时器 Period 从 71 改为 143，采样率变为多少？10kHz 信号每周期多少点？
3. **思考题：** 为什么实际可用带宽只有采样率的 1/5 而不是 1/2 (奈奎斯特极限)？
4. **Phase 1 实验报告：** 完成第一周进度报告 `assignments/week-1-checkin.md`

---

## 明日预告 | Tomorrow's Preview

Phase 2 开始！明天我们将学习波形触发逻辑——没有触发，波形在屏幕上会左右"跑动"，根本看不清。触发就像给波形"拍照"按快门的时机，选对了时机，波形就稳住了！

Phase 2 begins! Tomorrow we learn waveform triggering -- without triggering, the waveform "runs" left and right on screen and you can't see it clearly. Triggering is like choosing the right moment to press the shutter on a camera -- get it right, and the waveform stands still!

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| ADC+DMA+TIM CubeMX 完整教程 | B站视频 | https://www.bilibili.com/video/BV1GC4y1T7BN/ |
| 双ADC交替触发+DMA高速采样 | B站视频 | https://www.bilibili.com/video/BV1XH4y1G7TK/ |
| STM32F103 ADC 完整配置 | B站搜索 | 搜索"STM32F103 ADC DMA TIM 配置" |
| DIY 示波器原理讲解 | B站视频 | https://www.bilibili.com/video/BV15i4y1U78x/ |
| 奈奎斯特采样定理 | 搜索 | 搜索"奈奎斯特采样定理 详解" |

---

*最后更新：2026-05-26*
*Last updated: 2026-05-26*
