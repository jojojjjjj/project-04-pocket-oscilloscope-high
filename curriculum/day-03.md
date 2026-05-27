# Day 03: ADC 采样原理与 DMA | ADC Sampling Principles & DMA

> **今日目标 Today's Objectives:**
> - 理解 ADC (模数转换器) 的工作原理和关键参数
> - 理解 DMA (直接内存访问) 如何实现高速数据搬运
> - 使用 CubeMX 配置 STM32 的 ADC 单通道单次转换
> - 通过串口打印 ADC 采样值，建立对数字信号的直观认识
>
> **产出 Deliverable:** 串口输出 ADC 采样值（0~4095），输入不同电压时数值正确变化

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 | 回顾 Day 2 + 今日目标 |
| 09:30-10:30 | 知识讲解 | ADC 原理、采样率、奈奎斯特定理 |
| 10:30-10:45 | 休息 | Break |
| 10:45-12:00 | 动手实验 | ADC 单次转换 + 串口输出 |
| 12:00-13:30 | 午餐 | Lunch + Break |
| 13:30-15:00 | 知识讲解 + 实验 | DMA 原理 + CubeMX 配置 ADC+DMA |
| 15:00-15:15 | 休息 | Break |
| 15:15-16:30 | 动手实验 | DMA 连续采样 + 数据输出 |
| 16:30-17:00 | 回顾 | 今日总结 + 答疑 |

---

## 上午: ADC 基础 | Morning: ADC Fundamentals

### 为什么要学这个? | Why Learn This?

ADC (Analog-to-Digital Converter，模数转换器) 是示波器的核心部件。它的工作就是把"模拟的"电压信号变成"数字的"数值。没有 ADC，MCU 就无法"看到"外部世界的模拟信号。

The ADC (Analog-to-Digital Converter) is the core component of an oscilloscope. Its job is to convert "analog" voltage signals into "digital" values. Without the ADC, the MCU cannot "see" analog signals from the outside world.

生活类比：ADC 就像温度计上的刻度。温度是连续变化的（模拟量），但温度计上的刻度是离散的（数字量）。刻度越细，测量越精确。

Life analogy: The ADC is like markings on a thermometer. Temperature changes continuously (analog), but the markings are discrete (digital). The finer the markings, the more precise the measurement.

### 任务 3.1: ADC 原理讲解 (30 分钟)

**ADC 的工作过程：**

```
模拟电压 (0~3.3V) → [采样] → [保持] → [量化] → [编码] → 数字值 (0~4095)
```

1. **采样 (Sample):** 在某一时刻"抓取"电压值
2. **保持 (Hold):** 短暂保持这个值，为转换争取时间
3. **量化 (Quantize):** 把连续的电压值映射到离散的数字级
4. **编码 (Encode):** 输出二进制数字

**STM32F103 ADC 的关键参数：**

| 参数 Parameter | 数值 Value | 说明 Description |
|---------------|-----------|-----------------|
| 分辨率 Resolution | 12-bit | 2^12 = 4096 级 |
| 输入范围 Input Range | 0 ~ 3.3V (VREF) | 不能输入负电压 |
| 转换时间 Conversion Time | ~1us | 每次转换约 1us |
| 最大采样率 Max Sample Rate | ~1Msps | 每秒约 100 万个样本 |
| 通道数 Channels | 10 个外部通道 | PA0~PA7, PB0, PB1 |

**量化公式：**

```
数字值 = (模拟电压 / 参考电压) × (2^分辨率 - 1)
       = (V_in / 3.3) × 4095
```

例如：
- 输入 0V → 数字值 = 0
- 输入 1.65V → 数字值 ≈ 2048
- 输入 3.3V → 数字值 = 4095

**反向计算（从数字值算电压）：**

```
V_in = 数字值 × 3.3 / 4095
```

例如：
- 数字值 1024 → V_in ≈ 0.825V
- 数字值 3000 → V_in ≈ 2.417V

**步骤 Steps:**
1. 理解上述公式，手算以下值：
   - 输入 1.0V，数字值 = ?
   - 数字值 2500，输入电压 = ?
2. 讨论：12-bit 分辨率对 0~3.3V 范围的测量精度是多少毫伏？
   (答案：3300mV / 4096 ≈ 0.8mV)

**常见问题 Common Issues:**
- "为什么 ADC 不能测负电压？" -- ADC 参考电压从 0V 开始，没有负方向。需要偏置电路把信号抬高。
- "12-bit 够用吗？" -- 对于入门级示波器足够。专业设备会用更高分辨率的 ADC。

### 任务 3.2: CubeMX 配置 ADC 单次转换 (45 分钟)

**步骤 Steps:**

1. **新建 CubeMX 工程** (或修改 Day 1 的工程)
2. **配置 ADC1：**
   - 在 Pinout 页面，Analog → ADC1 → 勾选 IN0 (PA0)
   - 在 ADC1 Configuration 中：
     - Resolution: 12 Bits
     - Data Alignment: Right aligned
     - Scan Conversion Mode: Disabled
     - Continuous Conversion Mode: Disabled
     - Number of Conversion: 1
     - Rank 1: Channel 0, Sampling Time: 1.5 Cycles
3. **配置 USART1：**
   - Mode: Asynchronous
   - Baud Rate: 115200
4. **配置时钟：** 72MHz (同 Day 1)
5. **生成代码**

**在 main.c 中添加 ADC 读取代码：**

```c
/* USER CODE BEGIN Includes */
#include <stdio.h>    // 用于 printf 重定向
/* USER CODE END Includes */

/* USER CODE BEGIN 0 */
// 重定向 printf 到 USART1
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

uint16_t read_adc(void)
{
    HAL_ADC_Start(&hadc1);                        // 启动 ADC 转换
    HAL_ADC_PollForConversion(&hadc1, 100);        // 等待转换完成
    uint16_t value = HAL_ADC_GetValue(&hadc1);     // 读取结果
    HAL_ADC_Stop(&hadc1);                          // 停止 ADC
    return value;
}
/* USER CODE END 0 */

/* USER CODE BEGIN WHILE */
while (1)
{
    uint16_t adc_value = read_adc();
    float voltage = adc_value * 3.3f / 4095.0f;    // 转换为电压

    printf("ADC: %d, Voltage: %.3f V\r\n", adc_value, voltage);
    HAL_Delay(500);    // 每 500ms 采样一次
}
/* USER CODE END WHILE */
```

3. **接线：** PA0 接电位器中间引脚（电位器两端接 3.3V 和 GND）
4. **编译下载：** 连接 ST-Link，编译下载
5. **串口查看：** 打开串口调试助手 (115200, 8N1)，观察输出

**预期结果 Expected Result:**
- 串口每 500ms 输出一次 ADC 值和对应电压
- 旋转电位器时，数值从 0 变化到 4095
- 电压值从 0V 变化到约 3.3V

**常见问题 Common Issues:**
- "串口没有输出" -- 检查 USART1 的 TX/RX 接线，确认波特率设置正确
- "ADC 值不变" -- 检查 PA0 是否正确连接到电位器
- "ADC 值跳动很大" -- 在 PA0 和 GND 之间并联一个 100nF 电容（去耦）

---

## 下午: DMA 高速采样 | Afternoon: DMA High-Speed Sampling

### 为什么要学 DMA? | Why Learn DMA?

在上面的代码中，CPU 每次都要亲自去"读取"ADC 值。这就像你每次要喝水，都要亲自去接一杯。如果需要每秒采集 100 万个样本（1Msps），CPU 就要每秒执行 100 万次读取操作，根本忙不过来！

In the code above, the CPU has to personally "read" the ADC value each time. This is like getting up to fetch a glass of water every time you're thirsty. If you need to sample 1 million times per second (1Msps), the CPU would need to execute 1 million read operations per second -- it simply can't keep up!

**DMA (Direct Memory Access) 的解决方案：**

DMA 就像雇了一个"搬运工"。你只需要告诉它：
- 数据从哪来 (ADC 数据寄存器)
- 数据放到哪去 (内存数组)
- 搬多少 (采样深度)

然后 DMA 就会自动搬运数据，**完全不占用 CPU**！CPU 可以专心处理其他事情（比如波形显示、触发判断）。

DMA is like hiring a "mover." You just tell it:
- Where to get data (ADC data register)
- Where to put it (memory array)
- How much (sample depth)

Then DMA automatically moves data **without involving the CPU at all**! The CPU is free to do other things (like display waveforms, trigger detection).

```
没有 DMA:
  CPU: "读ADC → 存内存 → 读ADC → 存内存 → 读ADC → 存内存..."  (CPU 全程工作)

有了 DMA:
  CPU: "DMA，帮我把1024个ADC数据搬到这个数组里"
  DMA: "好的，我来搬"
  CPU: "太好了，我去做其他事" (CPU 去处理波形显示)
  DMA: "搬完了！" (触发中断通知 CPU)
```

### 任务 3.3: CubeMX 配置 ADC+DMA 连续采样 (45 分钟)

**步骤 Steps:**

1. **修改 CubeMX 配置：**
   - ADC1 → Continuous Conversion Mode: **Enabled**
   - ADC1 → DMA Settings → Add DMA Request:
     - Channel: DMA1 Channel 1
     - Direction: Peripheral to Memory
     - Data Width: Half Word (16-bit)
     - Mode: Circular (循环模式)
   - ADC1 → NVIC Settings → 勾选 DMA1 channel1 global interrupt

2. **配置定时器触发 (可选，更精确的采样率控制)：**
   - Timer 3 → Clock Source: Internal Clock
   - Timer 3 → Channel 1: Output Compare No Output
   - Prescaler: 0 (不分频，72MHz)
   - Counter Period: 71 (72MHz / 72 = 1MHz 触发频率)
   - Trigger Event Selection: Update Event
   - ADC1 → External Trigger Conversion Source: Timer 3 TRGO Event

3. **生成代码**

4. **在 main.c 中添加 DMA 采样代码：**

```c
/* USER CODE BEGIN 0 */
#define ADC_BUFFER_SIZE 1024    // 采样深度：1024 个点

uint16_t adc_buffer[ADC_BUFFER_SIZE];  // DMA 目标数组
volatile uint8_t adc_complete = 0;       // DMA 完成标志

// DMA 传输完成回调函数
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        adc_complete = 1;    // 标记 DMA 传输完成
    }
}
/* USER CODE END 0 */

/* USER CODE BEGIN 2 */
// 启动 ADC+DMA 连续采样
HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, ADC_BUFFER_SIZE);
/* USER CODE END 2 */

/* USER CODE BEGIN WHILE */
while (1)
{
    if (adc_complete)
    {
        adc_complete = 0;

        // 通过串口输出所有采样数据
        printf("=== Waveform Data Start ===\r\n");
        for (int i = 0; i < ADC_BUFFER_SIZE; i++)
        {
            printf("%d\r\n", adc_buffer[i]);
        }
        printf("=== Waveform Data End ===\r\n");

        HAL_Delay(2000);    // 等待 2 秒再采集下一组
    }
}
/* USER CODE END WHILE */
```

5. **接线：** PA0 接信号发生器 (1kHz 正弦波, 1Vpp, 偏置 1.65V)
6. **编译下载**

**预期结果 Expected Result:**
- 串口输出 1024 个 ADC 值
- 数据呈现正弦波形状 (数值在某个中心值上下波动)
- 信号发生器的 1kHz 正弦波被正确采样

**常见问题 Common Issues:**
- "DMA 数组全为 0 或 4095" -- 检查信号是否正确输入到 PA0
- "数据看起来不规律" -- 可能是采样率和信号频率不匹配，调整信号频率或采样深度
- "DMA 中断不触发" -- 检查 NVIC 配置中 DMA 中断是否使能

### 任务 3.4: Python 绘制波形 (30 分钟) [选做]

**步骤 Steps:**

1. 安装 Python 依赖：`pip install pyserial matplotlib numpy`
2. 运行 `software/src/main.py` 中的串口读取功能
3. 将串口数据绘制成波形图

```python
# software/src/main.py 的简化版本
import serial
import matplotlib.pyplot as plt

# 配置串口
ser = serial.Serial('COM3', 115200, timeout=1)  # 修改为你的串口号

# 读取数据
data = []
reading = False
for line in ser:
    line = line.decode('utf-8').strip()
    if line == "=== Waveform Data Start ===":
        data = []
        reading = True
    elif line == "=== Waveform Data End ===":
        reading = False
        break
    elif reading:
        data.append(int(line))

# 绘制波形
plt.figure(figsize=(10, 4))
plt.plot(data, 'b-', linewidth=0.5)
plt.title('ADC Waveform')
plt.xlabel('Sample Index')
plt.ylabel('ADC Value (0-4095)')
plt.grid(True)
plt.show()
```

**预期结果 Expected Result:**
- matplotlib 窗口显示正弦波形
- 波形清晰，幅度和频率与信号发生器设置一致

---

## 今日作业 | Homework

1. **计算题：** STM32 ADC 采样率为 1Msps，要采集 1024 个点，需要多长时间？
   (答案：1024 / 1,000,000 = 1.024ms)
2. **计算题：** 如果信号频率是 10kHz，采样率是 1Msps，一个周期内能采集多少个点？
   (答案：1,000,000 / 10,000 = 100 个点)
3. **思考题：** DMA 为什么能提高采样效率？它和 CPU 直接读取有什么本质区别？
4. **实验题：** 改变定时器的预分频值，观察采样率变化对正弦波采样效果的影响

---

## 明日预告 | Tomorrow's Preview

明天我们将把 ADC+DMA 配置完善到示波器级别的采样性能，并通过串口把波形数据发送到 PC 端用 Python 绘图验证。你将看到真正的"示波器采集到的波形"！

Tomorrow we will refine the ADC+DMA configuration to oscilloscope-level sampling performance, and send waveform data to PC via serial port for Python visualization. You will see a real "oscilloscope-captured waveform"!

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| ADC+DMA+TIM 实现示波器 (CubeMX) | B站视频 | https://www.bilibili.com/video/BV1GC4y1T7BN/ |
| 双ADC交替触发+DMA 高速采样 | B站视频 | https://www.bilibili.com/video/BV1XH4y1G7TK/ |
| STM32 ADC 原理讲解 | B站搜索 | 搜索"STM32 ADC 原理 教程" |
| STM32 DMA 原理讲解 | B站搜索 | 搜索"STM32 DMA 教程" |
| STM32F103 参考手册 ADC 章节 | PDF | RM0008 第 11 章 |

---

*最后更新：2026-05-26*
*Last updated: 2026-05-26*
