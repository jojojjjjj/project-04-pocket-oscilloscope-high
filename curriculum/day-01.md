# Day 01: 项目启动与示波器原理 | Project Launch & Oscilloscope Principles

> **今日目标 Today's Objectives:**
> - 理解示波器的基本工作原理和核心参数
> - 搭建 STM32 开发环境 (STM32CubeMX + Keil/CubeIDE)
> - 完成第一个 STM32 程序：LED 闪烁
> - 理解 GPIO 的基本概念和配置方法
>
> **产出 Deliverable:** 开发环境就绪，STM32 蓝pill板 LED 成功闪烁

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 | 开营仪式、自我介绍、课程介绍 |
| 09:30-10:30 | 知识讲解 | 示波器原理与核心参数 |
| 10:30-10:45 | 休息 | Break |
| 10:45-12:00 | 动手实验 | 硬件环境搭建 + 软件安装检查 |
| 12:00-13:30 | 午餐 | Lunch + Break |
| 13:30-15:00 | 动手实验 | CubeMX 工程创建 + LED 闪烁 |
| 15:00-15:15 | 休息 | Break |
| 15:15-16:30 | 动手实验 | GPIO 深入 + 按键输入实验 |
| 16:30-17:00 | 回顾 | 今日总结 + 答疑 |

---

## 上午: 认识示波器 | Morning: Understanding the Oscilloscope

### 为什么要学这个? | Why Learn This?

示波器被称为"电子工程师的眼睛"。在调试任何电路时，你最常问的问题是："这个信号长什么样？" 是正弦波还是方波？频率是多少？有没有噪声？这些问题只有示波器能回答。

The oscilloscope is called the "eye of electronics engineers." When debugging any circuit, the most common question is: "What does this signal look like?" Is it a sine wave or square wave? What frequency? Is there noise? Only an oscilloscope can answer these questions.

你将亲手打造的这台示波器，虽然比不上几万元的商用示波器，但它包含了示波器的全部核心原理：**采样**、**量化**、**触发**、**显示**。理解了这些原理，将来使用任何示波器都会得心应手。

The oscilloscope you will build, while not matching commercial ones costing tens of thousands of yuan, contains all the core principles: **sampling**, **quantization**, **triggering**, and **display**. Understanding these principles will make you proficient with any oscilloscope in the future.

### 任务 1.1: 什么是示波器？ (30 分钟)

**什么是波形？**

在电子世界中，电压会随时间变化。把这种变化画成图，就是"波形"（Waveform）。

- **正弦波 (Sine Wave):** 最基本的波形，交流电就是正弦波
- **方波 (Square Wave):** 数字信号的基本波形，Arduino PWM 输出就是方波
- **三角波 (Triangle Wave):** 锯齿状上升和下降的波形
- **噪声 (Noise):** 不规则的随机波动

**示波器的工作原理 (简化版)：**

```
被测信号 → [前端电路] → [ADC 采样] → [MCU 处理] → [屏幕显示]
             衰减/放大     模拟→数字     触发/存储     波形绘制
```

1. **前端电路：** 调节信号幅度，使之适合 ADC 的输入范围（0~3.3V）
2. **ADC 采样：** 把连续的模拟电压信号转换成数字值（0~4095 对应 0~3.3V）
3. **MCU 处理：** 微控制器决定何时触发、如何存储波形数据
4. **屏幕显示：** 将数字数据还原为屏幕上的波形图

**示波器的核心参数：**

| 参数 Parameter | 含义 Meaning | 本项目目标 |
|--------------|-------------|----------|
| 带宽 Bandwidth | 能测量的最高频率信号 | ~200kHz |
| 采样率 Sampling Rate | 每秒采集多少个点 | 1Msps |
| 分辨率 Resolution | 电压测量精度 | 12-bit (4096级) |
| 通道数 Channels | 同时测量几路信号 | 1 通道 |
| 时基 Timebase | 横轴每格代表多少时间 | 1us/div ~ 100ms/div |

**关键公式：**

- 采样周期 = 1 / 采样率 = 1 / 1,000,000 = 1us
- 电压分辨率 = 3.3V / 4096 = 0.8mV
- 奈奎斯特定理：要准确测量某频率信号，采样率必须 > 2倍信号频率

**步骤 Steps:**
1. 观看 B 站视频：尤里卡学院《示波管原理演示视频》
   - 链接：https://www.bilibili.com/video/BV174411h7bn/
2. 观看 B 站视频：Expert电子实验室《如何用示波器的触发捕捉波形》
   - 链接：https://www.bilibili.com/video/BV1HRVRz3EyK/
3. 用纸笔画出你理解的示波器工作流程图
4. 回答问题：如果要测量一个 100kHz 的正弦波，采样率至少需要多少？

**预期结果 Expected Result:**
能画出信号从输入到显示的完整流程图，能正确计算最低采样率 (200kHz 以上)。

**常见问题 Common Issues:**
- "采样率越高越好吗？" -- 是的，但受限于 ADC 硬件。STM32 内置 ADC 最高约 1Msps。
- "12-bit 分辨率够用吗？" -- 对于入门级示波器足够。专业示波器通常 8-bit 高速 ADC。

### 任务 1.2: 硬件环境搭建 (45 分钟)

**步骤 Steps:**

1. **检查硬件清单：** 对照 `hardware/BOM.md` 检查所有元件是否到齐
2. **认识蓝pill板：**
   - 找到 USB 接口、电源指示灯 (PWR)、板载 LED (PC13)
   - 找到各引脚的位置：PA0-PA15, PB0-PB15, PC13-PC15
   - 找到 SWD 调试接口：SWDIO, SWCLK, GND, 3.3V
3. **连接 ST-Link V2：**
   - ST-Link SWDIO → 蓝pill SWDIO
   - ST-Link SWCLK → 蓝pill SWCLK
   - ST-Link GND → 蓝pill GND
   - ST-Link 3.3V → 蓝pill 3.3V (也可用 USB 供电)
4. **安装驱动：** 如果 ST-Link 未被识别，安装驱动程序

**预期结果 Expected Result:**
- 电脑设备管理器中能看到 STLink USB 设备
- STM32CubeMX 能识别到 STM32F103C8Tx 芯片

**常见问题 Common Issues:**
- "ST-Link 无法识别" -- 检查 USB 线是否为数据线（非充电线），重新安装驱动
- "设备管理器显示感叹号" -- 右键更新驱动，选择 ST-Link 驱动目录

---

## 下午: 第一个 STM32 程序 | Afternoon: First STM32 Program

### 任务 1.3: CubeMX 工程创建 (45 分钟)

**步骤 Steps:**

1. 打开 STM32CubeMX，点击 "ACCESS TO MCU SELECTOR"
2. 搜索并选择 "STM32F103C8Tx"
3. **配置时钟：**
   - 在 Pinout 页面，将 RCC -> HSE 设置为 "Crystal/Ceramic Resonator"
   - 在 Clock Configuration 页面：
     - HSE 输入 8MHz
     - PLL 倍频选择 x9
     - 系统时钟 = 72MHz
4. **配置 GPIO 输出 (LED):**
   - 点击 PC13 引脚，选择 "GPIO_Output"
   - 在 System Core -> GPIO 中：
     - PC13 的 User Label 设为 "LED"
     - GPIO output level 设为 "High" (LED 默认灭，低电平点亮)
5. **配置串口 (调试用)：**
   - 在 Connectivity -> USART1 中，Mode 设为 "Asynchronous"
   - 波特率设为 115200
6. **生成代码：**
   - Project Manager -> 填写工程名称和路径
   - Toolchain/IDE 选择 "MDK-ARM V5" 或 "STM32CubeIDE"
   - 点击 "GENERATE CODE"

**预期结果 Expected Result:**
- 成功生成 STM32 工程代码
- 在 Keil/CubeIDE 中能打开工程并编译通过

### 任务 1.4: LED 闪烁程序 (45 分钟)

**步骤 Steps:**

1. 打开生成的工程，在 `main.c` 的 `USER CODE BEGIN` 区域添加代码：

```c
/* USER CODE BEGIN WHILE */
while (1)
{
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);  // 翻转 PC13 (LED)
    HAL_Delay(500);                           // 延时 500ms
}
/* USER CODE END WHILE */
```

2. 编译工程 (Build)
3. 连接 ST-Link，点击下载 (Download/Flash)
4. 观察蓝pill板上的 LED 是否以 1Hz 频率闪烁 (亮 0.5s, 灭 0.5s)

**预期结果 Expected Result:**
- 编译成功，无错误
- 下载成功后，LED 以约 1Hz 频率闪烁

**常见问题 Common Issues:**
- "编译错误：找不到头文件" -- 检查 CubeMX 生成的 Include Path 是否正确
- "下载失败：No ST-Link detected" -- 检查接线和驱动，确认 ST-Link 指示灯亮
- "LED 不亮" -- PC13 是低电平点亮，检查 GPIO 输出电平设置

### 任务 1.5: GPIO 深入与按键输入 (45 分钟)

**为什么要学 GPIO？**

GPIO (通用输入输出) 是 STM32 最基础的外设。在我们的示波器项目中：
- ADC 采样引脚是 GPIO 的模拟输入模式
- SPI 显示屏使用 GPIO 的复用功能模式
- 按键控制使用 GPIO 的输入模式

GPIO is the most fundamental peripheral of STM32. In our oscilloscope project:
- ADC sampling pins use GPIO analog input mode
- SPI display uses GPIO alternate function mode
- Button control uses GPIO input mode

**步骤 Steps:**

1. 在 CubeMX 中新增配置：PA0 设为 GPIO_Input (上拉输入)
2. 修改代码，实现按键控制 LED 闪烁频率：

```c
/* USER CODE BEGIN WHILE */
uint32_t delay_ms = 500;  // 默认延时 500ms

while (1)
{
    // 读取按键状态 (PA0)，按下时为低电平
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
    {
        delay_ms = 100;  // 按下时加速闪烁
    }
    else
    {
        delay_ms = 500;  // 松开时恢复正常
    }

    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    HAL_Delay(delay_ms);
}
/* USER CODE END WHILE */
```

3. 面包板上连接一个按键到 PA0 (按键另一端接 GND)
4. 下载测试，按下按键时 LED 闪烁加快

**预期结果 Expected Result:**
- 按键按下时 LED 闪烁加速 (100ms)
- 松开时恢复正常 (500ms)

---

## 今日作业 | Homework

1. **画图题：** 在实验日志中画出示波器的工作流程图，标注每个环节的功能
2. **计算题：** 如果 ADC 采样率是 1Msps，采样深度是 1024 个点，满屏显示需要多少时间？
   (提示：时间 = 点数 / 采样率 = 1024 / 1000000 = 1.024ms)
3. **思考题：** 为什么 STM32 内置 ADC 只有 12-bit？如果想要更高精度怎么办？
4. **写实验日志：** 按照 `assignments/` 目录的要求，写第一篇实验日志

---

## 明日预告 | Tomorrow's Preview

明天我们将学习运算放大器电路——这是示波器的"耳朵"，负责把外界信号"听"清楚并放大到合适的大小。你将亲手在面包板上搭建放大电路！

Tomorrow we will learn about op-amp circuits -- the "ears" of the oscilloscope, responsible for listening to external signals clearly and amplifying them to the right level. You will build amplifier circuits on a breadboard!

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| 示波器原理演示 | B站视频 | https://www.bilibili.com/video/BV174411h7bn/ |
| 如何用示波器的触发捕捉波形 | B站视频 | https://www.bilibili.com/video/BV1HRVRz3EyK/ |
| STM32CubeMX 安装教程 | B站搜索 | 搜索"STM32CubeMX 安装 教程" |
| STM32F103 参考手册 | PDF | st.com 下载 RM0008 |
| 蓝pill板引脚图 | 图片 | 搜索"STM32 blue pill pinout" |

---

*最后更新：2026-05-26*
*Last updated: 2026-05-26*
