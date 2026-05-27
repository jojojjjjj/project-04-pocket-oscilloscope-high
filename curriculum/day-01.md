# Day 01: 项目启动与信号发生器原理 | Project Launch & Signal Generator Principles

> **今日目标 Today's Objectives:**
> - 理解信号发生器的基本工作原理和核心参数
> - 掌握 DDS（直接数字频率合成）的基本概念
> - 了解 TinyAWG 系统架构（PS + PL 双核协同）
> - 完成硬件环境检查与 Vivado 安装验证
>
> **产出 Deliverable:** 完成硬件清单核对，Vivado 可正常启动，理解 TinyAWG 系统框图

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 Morning Meeting | 开营仪式、自我介绍、课程介绍 |
| 09:30-10:30 | 知识讲解 Lecture | 信号发生器原理与 DDS 概念 |
| 10:30-10:45 | 休息 Break | Break |
| 10:45-12:00 | 知识讲解 Lecture | TinyAWG 系统架构与核心参数 |
| 12:00-13:30 | 午餐 Lunch | Lunch + Break |
| 13:30-15:00 | 动手实验 Hands-on | Vivado 安装检查 + ZYNQ 核心板认识 |
| 15:00-15:15 | 休息 Break | Break |
| 15:15-16:30 | 动手实验 Hands-on | JTAG 连接 + 硬件套件完整检查 |
| 16:30-17:00 | 回顾 Review | 今日总结 + 答疑 |

---

## 上午: 认识信号发生器 | Morning: Understanding the Signal Generator

### 为什么要学这个? | Why Learn This?

信号发生器被称为"电子实验室的心脏"。如果说示波器是用来"看"信号的眼睛，那么信号发生器就是用来"产生"信号的心脏。无论是测试放大器增益、校准滤波器、还是驱动扬声器发声，你都需要一个可控的信号源。

A signal generator is called the "heart of the electronics lab." If an oscilloscope is the eye that "sees" signals, then a signal generator is the heart that "produces" signals. Whether testing amplifier gain, calibrating filters, or driving speakers, you need a controllable signal source.

你将亲手打造的 TinyAWG（Tiny Arbitrary Waveform Generator），虽然体积只有口袋大小，但性能令人惊叹：**200MSa/s 采样率、14-bit 分辨率、35MHz 带宽**。它的秘密武器是 Xilinx ZYNQ7010 芯片——一颗芯片里同时包含了 ARM 处理器和 FPGA 可编程逻辑，让软件和硬件完美协同。

The TinyAWG you will build is pocket-sized yet remarkably powerful: **200MSa/s, 14-bit resolution, 35MHz bandwidth**. Its secret weapon is the Xilinx ZYNQ7010 chip — a single chip containing both an ARM processor and FPGA programmable logic, enabling perfect software-hardware coordination.

### 任务 1.1: 什么是信号发生器？ (30 分钟)

**什么是波形？**

在电子世界中，电压会随时间变化。把这种变化画成图，就是"波形"（Waveform）。

- **正弦波 (Sine Wave):** 最基本的波形，交流电就是正弦波。一切复杂信号都可以分解成正弦波的叠加（傅里叶定理）
- **方波 (Square Wave):** 数字信号的基本波形，在高低电平之间快速切换
- **三角波 (Triangle Wave):** 线性上升和下降的波形，常用于扫频和 PWM 产生
- **锯齿波 (Sawtooth Wave):** 线性上升后瞬间归零，常用于 CRT 扫描和音频合成
- **任意波 (Arbitrary Waveform):** 用户自定义的任意形状波形，这就是 "AWG" 中的 "A"

**信号发生器的分类：**

| 类型 Type | 特点 Feature | 典型应用 Typical Use |
|----------|-------------|-------------------|
| 函数发生器 Function Generator | 输出标准波形（正弦、方波、三角波） | 通用电路测试 |
| 任意波形发生器 AWG | 可输出用户自定义的任意波形 | 通信仿真、复杂信号测试 |
| DDS 发生器 DDS Generator | 用数字方法精确控制频率和相位 | 高精度频率源、通信系统 |

**你将制作的 TinyAWG 属于 DDS 任意波形发生器——既能输出标准波形，也能自定义任意波形，而且频率精确度极高。**

**信号发生器的工作原理 (简化版)：**

```
频率控制字 → [相位累加器] → [波形查找表] → [DAC] → [滤波器] → 模拟输出
   FCW         Phase Accum     LUT/BRAM     AD9744   低通滤波    输出端口
```

1. **频率控制字 (FCW):** 一个数字值，决定输出信号的频率
2. **相位累加器 (Phase Accumulator):** 每个时钟周期将 FCW 累加，产生连续增长的相位值
3. **波形查找表 (LUT):** 将相位值映射为对应的波形幅值（存储在 FPGA 的 BRAM 中）
4. **DAC (数模转换器):** 将数字幅值转换为模拟电压信号
5. **滤波器 (Filter):** 平滑 DAC 输出的阶梯状波形

**步骤 Steps:**
1. 观看 B 站视频：了解信号发生器的基本用途
   - 搜索关键词："信号发生器 原理 教程"
2. 观看 B 站视频：DDS 直接数字频率合成原理
   - 搜索关键词："DDS 原理 直接数字频率合成"
3. 用纸笔画出你理解的信号发生器工作流程图
4. 回答问题：如果要产生一个 1kHz 的正弦波，每个周期至少需要多少个采样点才能看起来"像"正弦波？

**预期结果 Expected Result:**
能画出从频率控制字到模拟输出的完整 DDS 流程图，理解每个环节的作用。

**常见问题 Common Issues:**
- "DDS 和传统模拟振荡器有什么区别？" -- DDS 用纯数字方法产生信号，频率精度远高于模拟振荡器，而且可以瞬间切换频率
- "为什么需要 DAC？" -- FPGA 内部只能处理数字信号，DAC 负责把数字转换为真实的模拟电压

### 任务 1.2: DDS 原理入门 (30 分钟)

**为什么要学 DDS？**

DDS（Direct Digital Synthesis，直接数字频率合成）是现代信号发生器的核心技术。理解 DDS，你就理解了 TinyAWG 的核心工作原理。它是连接"数字世界"（FPGA）和"模拟世界"（真实信号）的桥梁。

DDS (Direct Digital Synthesis) is the core technology of modern signal generators. Understanding DDS means understanding how TinyAWG works at its core. It is the bridge between the "digital world" (FPGA) and the "analog world" (real signals).

**DDS 核心公式：**

```
f_out = (FCW / 2^N) × f_clk
```

其中：
- `f_out` = 输出信号频率（你想产生的频率）
- `FCW` = 频率控制字（Frequency Control Word），一个整数
- `N` = 相位累加器的位宽（TinyAWG 中通常为 32 位）
- `f_clk` = 系统时钟频率（TinyAWG 中为 200MHz）

**举个例子：**

假设我们想产生一个 1MHz 的正弦波：
- f_clk = 200,000,000 Hz (200MHz)
- N = 32 (32 位相位累加器)
- FCW = f_out × 2^N / f_clk = 1,000,000 × 4,294,967,296 / 200,000,000 = 21,474,836.48 ≈ 21,474,836

```
相位累加器工作过程（简化，4位示例）：

时钟周期 | 累加器值 | 对应角度 | 正弦值
---------|---------|---------|--------
   0     |  0000   |   0°    |  0.000
   1     |  0011   |  67.5°  |  0.924
   2     |  0110   | 135°    |  0.707
   3     |  1001   | 202.5°  | -0.383
   4     |  1100   | 270°    | -1.000
   5     |  1111   | 337.5°  | -0.383
   6     | 0010    |  45°    |  0.707   ← 溢出后从0继续
   ...
```

**步骤 Steps:**
1. 用计算器验证上面的例子：当 FCW = 21,474,836，f_clk = 200MHz，N = 32 时，f_out 是否约等于 1MHz？
2. 思考：如果要产生 10MHz 的信号，FCW 应该是多少？
3. 用纸笔模拟一个 4 位相位累加器的工作过程（FCW = 3），写出前 16 个时钟周期的累加器值

**预期结果 Expected Result:**
能手动计算给定频率的 FCW 值，能解释相位累加器溢出后如何自动产生周期性波形。

**常见问题 Common Issues:**
- "为什么相位累加器溢出不是错误？" -- 因为正弦波本身就是周期性的，相位超过 360° 就回到 0°，所以溢出恰好对应波形的下一个周期
- "FCW 必须是整数，那输出频率有误差吗？" -- 是的，有微小误差。当 N=32 时，频率分辨率为 200MHz/2^32 ≈ 0.047Hz，精度极高

### 任务 1.3: TinyAWG 系统架构 (45 分钟)

**TinyAWG 的"大脑"——ZYNQ7010**

ZYNQ7010 是一颗非常特殊的芯片，它把两个完全不同的"处理器"封装在一起：

```
┌─────────────────────────────────────────────────────────┐
│                    ZYNQ7010 芯片                          │
│                                                          │
│  ┌──────────────────┐    ┌──────────────────────────┐   │
│  │   PS (Processing  │    │    PL (Programmable       │   │
│  │    System)        │    │      Logic)               │   │
│  │                   │    │                           │   │
│  │  ARM Cortex-A9    │    │   FPGA (Artix-7)          │   │
│  │  双核 650MHz      │    │   可编程逻辑门阵列          │   │
│  │                   │    │                           │   │
│  │  运行 C 代码:     │    │  运行 Verilog:            │   │
│  │  - LVGL GUI      │◄──►│  - DDS 流水线             │   │
│  │  - 触摸驱动       │AXI │  - 波形查找表 BRAM         │   │
│  │  - DAC 控制       │Bus │  - AXI Lite 寄存器        │   │
│  │  - 电池管理       │    │  - DAC 数据接口            │   │
│  │                   │    │                           │   │
│  │  DDR3 内存控制    │    │   2560 LUTs, 5328 FFs    │   │
│  │  SD 卡启动        │    │   2.1Mb BRAM              │   │
│  └──────────────────┘    └──────────────────────────┘   │
└─────────────────────────────────────────────────────────┘
         │                                    │
         │                                    │
    ┌────▼────┐                         ┌────▼────┐
    │  DDR3   │                         │ AD9744  │
    │  内存    │                         │  DAC    │
    │ 256MB   │                         │ 14-bit  │
    └─────────┘                         └────┬────┘
                                            │
                                       ┌────▼────┐
                                       │OPA2673  │
                                       │ 运放     │
                                       │ 滤波+放大│
                                       └────┬────┘
                                            │
                                       ┌────▼────┐
                                       │ SMA输出  │
                                       │ BNC接口  │
                                       └─────────┘
```

**为什么要 PS + PL 双架构？**

在 TinyAWG 中，ARM 和 FPGA 各有所长，分工明确：

| 模块 Module | 负责人 Who | 任务 What | 为什么 Why |
|------------|-----------|----------|-----------|
| LVGL GUI 触摸界面 | ARM (PS) | 显示菜单、接收触摸输入 | GUI 需要灵活的软件逻辑，用 C 代码开发效率高 |
| DDS 相位累加 | FPGA (PL) | 高速相位累加、查表输出 | 需要 200MHz 精确时序，硬件逻辑无抖动 |
| 波形存储 (BRAM) | FPGA (PL) | 存储正弦/方波/任意波的采样点 | BRAM 是 FPGA 内部高速存储，一个时钟周期即可读取 |
| DAC 数据接口 | FPGA (PL) | 按 200MHz 节拍向 AD9744 送数据 | 需要精确的时序控制，软件做不到这么快 |
| 电池电压监测 | ARM (PS) | ADC 读取电池电压 | 低速任务，用 ARM 的软件 ADC 就够了 |
| PS ↔ PL 通信 | AXI 总线 | ARM 写寄存器控制 FPGA 参数 | AXI 是标准的总线协议，打通软硬件 |

**TinyAWG 的核心参数：**

| 参数 Parameter | 含义 Meaning | TinyAWG 规格 |
|--------------|-------------|-------------|
| 采样率 Sampling Rate | 每秒输出多少个采样点 | 200MSa/s (每秒2亿个点) |
| 分辨率 Resolution | 每个采样点的精度 | 14-bit (16384级) |
| 带宽 Bandwidth | 能输出的最高频率 | 35MHz |
| DAC | 数模转换芯片 | AD9744 (14-bit, 210MSa/s) |
| 输出运放 | 信号放大和滤波 | OPA2673IRGVT |
| 显示 | 人机界面 | 2.8" 触摸屏, LVGL 8.3.1 |
| 功耗 Power | 电池续航 | 锂电池供电 |
| 成本 Cost | 制作成本 | 约 220 元 |

**步骤 Steps:**
1. 对照系统框图，在纸上抄画一遍 TinyAWG 的系统架构
2. 标注每个模块对应芯片的型号
3. 回答问题：为什么 DDS 相位累加要用 FPGA 而不用 ARM 的 C 代码？

**预期结果 Expected Result:**
能完整画出 TinyAWG 系统框图，能解释 PS 和 PL 各自的职责分工。

**常见问题 Common Issues:**
- "ARM 不够快吗？为什么还需要 FPGA？" -- ARM 运行 C 代码，一条指令需要多个时钟周期，而且有中断、操作系统等不确定延迟。FPGA 是纯硬件逻辑，每个时钟周期都精确定位，200MHz 就是 200MHz，不多不少
- "什么是 AXI？" -- AXI (Advanced eXtensible Interface) 是 ARM 定义的一种高速总线协议，ZYNQ 中 PS 和 PL 通过 AXI 总线通信。你可以把它理解为 ARM 和 FPGA 之间的"高速公路"

---

## 下午: 硬件环境搭建 | Afternoon: Hardware Environment Setup

### 任务 1.4: Vivado 安装检查 (45 分钟)

**为什么要检查 Vivado？**

Vivado 是 Xilinx FPGA 的官方开发工具，是我们整个项目的"工作台"。所有的 FPGA 设计——包括 Block Design、Verilog 编写、综合、实现、下载——都在 Vivado 中完成。它是一个庞大的软件（安装包约 30GB），必须提前安装好。

Vivado is the official development tool for Xilinx FPGAs, serving as the "workbench" for our entire project. All FPGA design work — Block Design, Verilog coding, synthesis, implementation, and downloading — happens in Vivado. It is a large piece of software (~30GB installer) that must be installed in advance.

**步骤 Steps:**

1. **确认 Vivado 版本：**
   - 打开 Vivado（桌面快捷方式或开始菜单）
   - 查看版本号：Help → About Vivado
   - 推荐版本：**Vivado 2018.3** 或 **Vivado 2020.2**（与 TinyAWG 开源工程兼容）
   - 如果版本不对或未安装，向助教报告

2. **验证 License：**
   - 确认 Vivado 可以正常打开，无 License 报错
   - ZYNQ7010 属于 WebPACK 免费支持范围，无需付费 License

3. **测试创建工程：**
   - 打开 Vivado → Create Project → 选择一个测试路径
   - 在 Part 选择中搜索 "xc7z010"（ZYNQ7010 的型号）
   - 确认能找到 `xc7z010clg400-1` 这个器件

**预期结果 Expected Result:**
- Vivado 可正常启动，无 License 报错
- 能在器件列表中找到 `xc7z010clg400-1`

**常见问题 Common Issues:**
- "Vivado 启动很慢" -- 正常现象，Vivado 是大型 Java 应用，首次启动可能需要 1-2 分钟
- "找不到 xc7z010 器件" -- 安装时可能没有勾选 ZYNQ 支持。在 Vivado Installer 中确认 "Zynq-7000" 系列被选中
- "License 过期" -- 去 Xilinx 官网申请 WebPACK License（免费）

### 任务 1.5: 认识 ZYNQ 核心板 (30 分钟)

**步骤 Steps:**

1. **观察 ZYNQ 核心板：**
   - 找到主芯片（通常有 Xilinx 标志，型号 "XC7Z010"）
   - 找到 DDR3 内存芯片（通常在芯片背面）
   - 找到 JTAG 调试接口（通常是 6 针或 10 针排针）
   - 找到电源指示灯 (PWR LED)
   - 找到用户 LED（通常连接到 PL 侧引脚）
   - 找到用户按键（如果有的话）

2. **阅读核心板原理图（向助教索取）：**
   - 确认 JTAG 接口引脚定义：TCK, TMS, TDI, TDO, GND, VREF
   - 确认用户 LED 连接的 PL 引脚编号
   - 确认 MIO 引脚分配（PS 侧外设）
   - 记录到你的实验日志中

3. **认识扩展底板（如果有）：**
   - 找到 DAC (AD9744) 芯片位置
   - 找到 LCD 触摸屏接口
   - 找到 SMA/BNC 信号输出接口
   - 找到电池接口和充电电路
   - 找到电源开关

**预期结果 Expected Result:**
- 能在核心板上指出主芯片、DDR3、JTAG 接口的位置
- 记录了关键引脚编号（LED 引脚、JTAG 引脚）

**常见问题 Common Issues:**
- "核心板上芯片很小，看不到型号" -- 用手机放大镜功能或向助教确认
- "找不到原理图" -- 向助教索取电子版，或者查看 TinyAWG 开源项目页面的硬件资料

### 任务 1.6: JTAG 连接与硬件套件检查 (45 分钟)

**什么是 JTAG？**

JTAG (Joint Test Action Group) 是一种标准的调试接口，用于将比特流（bitstream）下载到 FPGA。你可以把它理解为电脑和 FPGA 之间的"数据线"——没有它，你就无法把设计好的电路"烧录"到 FPGA 中。

JTAG is a standard debug interface used to download bitstreams to the FPGA. Think of it as the "data cable" between your computer and the FPGA — without it, you cannot "burn" your designed circuits into the FPGA.

**步骤 Steps:**

1. **连接 JTAG 下载器：**
   - 将 JTAG 下载器（通常是 Xilinx Platform Cable 或兼容产品）连接到核心板的 JTAG 接口
   - 注意引脚对应关系：
     - VTREF → JTAG 接口的 VREF
     - TCK → TCK
     - TMS → TMS
     - TDI → TDI
     - TDO → TDO
     - GND → GND
   - **务必在断电状态下连接！**

2. **上电测试：**
   - 给核心板上电（USB 或外接电源）
   - 确认电源指示灯亮起
   - 连接 JTAG 下载器的 USB 端到电脑

3. **Vivado 中识别器件：**
   - 打开 Vivado → Open Hardware Manager
   - 点击 "Open target" → "Auto Connect"
   - 确认能看到 ZYNQ 器件（xc7z010）
   - 截图保存

4. **硬件清单最终检查：**

   | 物品 Item | 数量 Qty | 检查 Check |
   |----------|---------|-----------|
   | ZYNQ7010 核心板 | 1 | □ |
   | JTAG 下载器 | 1 | □ |
   | USB 数据线 | 2 | □ |
   | 2.8" 触摸屏模块 | 1 | □ |
   | SMA/BNC 输出接口 | 1 | □ |
   | 锂电池 | 1 | □ |
   | 杜邦线若干 | 1套 | □ |
   | 面包板 | 1 | □ |
   | 万用表 | 1 | □ |

**预期结果 Expected Result:**
- Vivado Hardware Manager 能识别到 ZYNQ 器件
- 硬件清单全部核对完毕

**常见问题 Common Issues:**
- "Hardware Manager 找不到器件" -- 检查 JTAG 接线是否正确，确认下载器驱动已安装，检查核心板是否已上电
- "下载器指示灯不亮" -- 更换 USB 线（确认是数据线不是充电线），更换 USB 口
- "识别到器件但显示未知" -- 可能需要安装 cable driver，在 Vivado 安装目录中找到 `install_drivers` 并运行

---

## 今日作业 | Homework

1. **画图题：** 在实验日志中画出 TinyAWG 的完整系统框图，标注 PS、PL、DAC、运放、显示等模块，用箭头标出数据流方向
2. **计算题：** TinyAWG 的 DDS 使用 32 位相位累加器，时钟频率 200MHz。请计算：
   - (a) 频率分辨率是多少？（提示：200MHz / 2^32）
   - (b) 要输出 5MHz 正弦波，FCW 应设为多少？
   - (c) 要输出 440Hz（标准 A 音），FCW 应设为多少？
3. **思考题：** 为什么 TinyAWG 的带宽（35MHz）小于采样率的一半（100MHz）？实际中还有哪些因素限制了带宽？
4. **写实验日志：** 按照 `assignments/` 目录的要求，写第一篇实验日志

---

## 明日预告 | Tomorrow's Preview

明天我们将正式进入 Vivado 工程！你将创建第一个 ZYNQ Block Design，配置 ARM 处理器系统（PS），并让 FPGA 上的 LED 亮起来。这是从"纸上谈兵"到"实战操作"的关键一步！

Tomorrow we will dive into Vivado! You will create your first ZYNQ Block Design, configure the ARM Processing System (PS), and light up an LED on the FPGA. This is the critical step from "theory" to "hands-on practice"!

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| TinyAWG 开源项目 | 立创开源 | https://oshwhub.com/greentor/tinyawg-signal-source |
| DDS 原理详解 | B站视频 | 搜索"DDS 直接数字频率合成 教程" |
| ZYNQ 架构简介 | B站视频 | 搜索"ZYNQ PS PL 架构 入门" |
| Vivado 安装教程 | B站视频 | 搜索"Vivado 安装教程 ZYNQ" |
| AD9744 数据手册 | PDF | 在 analog.com 搜索 AD9744 |
| ZYNQ7010 数据手册 | PDF | 在 xilinx.com 搜索 ds187 |

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
