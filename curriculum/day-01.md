# Day 01: 项目启动与信号发生器原理 | Project Launch & Signal Generator Principles

> **今日目标 Today's Objectives:**
> - 理解信号发生器的基本工作原理和核心参数
> - 掌握 DDS（直接数字频率合成）的基本概念
> - 了解 WavePocket 系统架构（iCE40 FPGA + DAC904 模块 + TFT + 编码器）
> - 完成硬件环境检查与 iCE40 开源工具链安装验证
>
> **产出 Deliverable:** 完成硬件清单核对，工具链可正常启动，理解 WavePocket 系统框图

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 Morning Meeting | 开营仪式、自我介绍、课程介绍 |
| 09:30-10:30 | 知识讲解 Lecture | 信号发生器原理与 DDS 概念 |
| 10:30-10:45 | 休息 Break | Break |
| 10:45-12:00 | 知识讲解 Lecture | WavePocket 系统架构与核心参数 |
| 12:00-13:30 | 午餐 Lunch | Lunch + Break |
| 13:30-15:00 | 动手实验 Hands-on | iCE40 工具链检查 + 开发板认识 |
| 15:00-15:15 | 休息 Break | Break |
| 15:15-16:30 | 动手实验 Hands-on | JTAG 连接 + 硬件套件完整检查 |
| 16:30-17:00 | 回顾 Review | 今日总结 + 答疑 |

---

## 上午: 认识信号发生器 | Morning: Understanding the Signal Generator

### 为什么要学这个? | Why Learn This?

信号发生器被称为"电子实验室的心脏"。如果说示波器是用来"看"信号的眼睛，那么信号发生器就是用来"产生"信号的心脏。无论是测试放大器增益、校准滤波器、还是驱动扬声器发声，你都需要一个可控的信号源。

A signal generator is called the "heart of the electronics lab." If an oscilloscope is the eye that "sees" signals, then a signal generator is the heart that "produces" signals. Whether testing amplifier gain, calibrating filters, or driving speakers, you need a controllable signal source.

你将亲手打造的 WavePocket（口袋信号发生器），体积只有口袋大小，却能用 **iCE40 FPGA 跑 DDS**，输出正弦波、方波、三角波、锯齿波，频率 1Hz~10MHz。它的核心是一颗 Lattice iCE40 FPGA——纯硬件可编程逻辑，所有 DDS 波形合成都用 Verilog 写成电路在里面并行跑。

The WavePocket you will build is pocket-sized yet capable: an **iCE40 FPGA running DDS** to output sine, square, triangle, and sawtooth waves from 1Hz to 10MHz. Its core is a Lattice iCE40 FPGA — pure hardware programmable logic, where all DDS waveform synthesis runs as parallel Verilog circuits.

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

**你将制作的 WavePocket 属于 DDS 信号发生器——能输出标准波形，频率精确度极高。**

**信号发生器的工作原理 (简化版)：**

```
频率控制字 → [相位累加器] → [波形查找表] → [DAC] → [滤波器] → 模拟输出
   FCW         Phase Accum     LUT/BRAM     DAC904   低通滤波    输出端口
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

DDS（Direct Digital Synthesis，直接数字频率合成）是现代信号发生器的核心技术。理解 DDS，你就理解了 WavePocket 的核心工作原理。它是连接"数字世界"（FPGA）和"模拟世界"（真实信号）的桥梁。

DDS (Direct Digital Synthesis) is the core technology of modern signal generators. Understanding DDS means understanding how WavePocket works at its core. It is the bridge between the "digital world" (FPGA) and the "analog world" (real signals).

**DDS 核心公式：**

```
f_out = (FCW / 2^N) × f_clk
```

其中：
- `f_out` = 输出信号频率（你想产生的频率）
- `FCW` = 频率控制字（Frequency Control Word），一个整数
- `N` = 相位累加器的位宽（WavePocket 中通常为 32 位）
- `f_clk` = 系统时钟频率（WavePocket 中为 30MHz）

**举个例子：**

假设我们想产生一个 1MHz 的正弦波：
- f_clk = 30,000,000 Hz (30MHz)
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
1. 用计算器验证上面的例子：当 FCW = 1,607,077，f_clk = 30MHz，N = 32 时，f_out 是否约等于 1MHz？
2. 思考：如果要产生 10MHz 的信号，FCW 应该是多少？
3. 用纸笔模拟一个 4 位相位累加器的工作过程（FCW = 3），写出前 16 个时钟周期的累加器值

**预期结果 Expected Result:**
能手动计算给定频率的 FCW 值，能解释相位累加器溢出后如何自动产生周期性波形。

**常见问题 Common Issues:**
- "为什么相位累加器溢出不是错误？" -- 因为正弦波本身就是周期性的，相位超过 360° 就回到 0°，所以溢出恰好对应波形的下一个周期
- "FCW 必须是整数，那输出频率有误差吗？" -- 是的，有微小误差。当 N=32 时，频率分辨率为 30MHz/2^32 ≈ 0.00698Hz，精度极高

### 任务 1.3: WavePocket 系统架构 (45 分钟)

**WavePocket 的"大脑"——iCE40 FPGA**

WavePocket 用一颗 Lattice iCE40 FPGA 当大脑。它是一颗纯 FPGA（可编程逻辑门阵列），所有 DDS 逻辑都在它里面用 Verilog 写成硬件电路跑：

```
┌─────────────────────────────────────────────────────────┐
│                    iCE40 FPGA                            │
│                                                          │
│   运行 Verilog 硬件逻辑:                                  │
│   - DDS 相位累加器                                        │
│   - 波形查找表 (BRAM)                                     │
│   - DAC904 并行数据接口                                   │
│   - TFT SPI 显示驱动                                      │
│   - EC11 编码器读数                                       │
│                                                          │
│   资源: ~8K LUTs, BRAM 块, PLL                            │
└─────────────────────────────────────────────────────────┘
         │              │                │
    ┌────▼────┐   ┌─────▼─────┐   ┌─────▼─────┐
    │ DAC904  │   │ ST7735    │   │  EC11     │
    │ DAC模块 │   │ 1.8"TFT  │   │  编码器    │
    │ 14-bit  │   │ SPI 显示  │   │  选波/调频 │
    └────┬────┘   └───────────┘   └───────────┘
         │
    ┌────▼────┐
    │ 运放模块 │
    │ 滤波+放大│
    └────┬────┘
         │
    ┌────▼────┐
    │ 输出接线柱│
    │ (示波器) │
    └─────────┘
```

**为什么选 FPGA？**

| 模块 Module | 负责人 Who | 任务 What | 为什么 Why |
|------------|-----------|----------|-----------|
| DDS 相位累加 | FPGA | 高速相位累加、查表输出 | 需要精确时序，硬件逻辑并行无抖动 |
| 波形存储 (BRAM) | FPGA | 存正弦/方波/三角/锯齿采样点 | BRAM 是 FPGA 内部高速存储，一个时钟周期即可读取 |
| DAC 数据接口 | FPGA | 按节拍向 DAC904 送数据 | 需要精确时序控制，MCU 软件做不到这么快 |
| TFT 显示 | FPGA | SPI 送字符到屏幕 | 慢速任务，FPGA 顺手就做了 |
| 编码器读数 | FPGA | 读 A/B 相正交信号 | 简单 GPIO，FPGA 直接处理 |

**为什么不用 STM32 这种 MCU？** MCU 是串行执行指令的，每输出一个波形点都要走"读表→写 DAC"的指令流程，几十 MHz 的采样率下指令根本跑不过来。FPGA 是硬件并行——相位累加、查表、输出 DAC 数据在同一时钟周期内同时完成。类比：MCU 像一个人一张一张翻乐谱弹琴，FPGA 像一排琴键同时按下去。

**WavePocket 的核心参数：**

| 参数 Parameter | 含义 Meaning | WavePocket 规格 |
|--------------|-------------|-------------|
| 采样率 Sampling Rate | 每秒输出多少个采样点 | ~30MSa/s |
| 分辨率 Resolution | 每个采样点的精度 | 14-bit (16384级) |
| 带宽 Bandwidth | 能输出的最高频率 | ~10MHz |
| DAC | 数模转换模块 | DAC904 (14-bit) |
| 输出调理 | 信号放大和滤波 | 运放模块 |
| 显示 | 人机界面 | 1.8" ST7735 TFT |
| 交互 | 选波形/调频率 | EC11 旋转编码器 |
| 成本 Cost | 制作成本 | 约 178 元 (全部现成开发板+模块) |

**步骤 Steps:**
1. 对照系统框图，在纸上抄画一遍 WavePocket 的系统架构
2. 标注每个模块对应的型号
3. 回答问题：为什么 DDS 相位累加要用 FPGA 而不用 MCU 的 C 代码？

**预期结果 Expected Result:**
能完整画出 WavePocket 系统框图，能解释为什么用 FPGA 而不是 MCU。

**常见问题 Common Issues:**
- "MCU 不够快吗？为什么还需要 FPGA？" -- MCU 运行 C 代码，一条指令需要多个时钟周期，而且有中断等不确定延迟。FPGA 是纯硬件逻辑，每个时钟周期都精确定位，时序由电路保证
- "iCE40 工具链是什么？" -- iCE40 用全开源工具链：Yosys 综合 + nextpnr 布局布线 + IceStorm/iceprog 烧录，免费、轻量，普通笔记本就能跑

---

## 下午: 硬件环境搭建 | Afternoon: Hardware Environment Setup

### 任务 1.4: iCE40 工具链安装检查 (45 分钟)

**为什么要检查工具链？**

iCE40 的开源工具链（Yosys + nextpnr + IceStorm）是我们整个项目的"工作台"。所有的 FPGA 设计——Verilog 编写、综合、布局布线、烧录——都用它完成。和商业 FPGA 软件（几十 GB）不同，iCE40 工具链安装包小、对电脑配置要求低，普通笔记本就能跑。

The iCE40 open-source toolchain (Yosys + nextpnr + IceStorm) is the "workbench" for our entire project. All FPGA design work — Verilog coding, synthesis, place-and-route, and flashing — happens with it. Unlike commercial FPGA software (tens of GB), the iCE40 toolchain is small and runs on an ordinary laptop.

**步骤 Steps:**

1. **确认工具链已安装：**
   - 打开命令行（Windows: Git Bash / PowerShell；Linux/Mac: Terminal）
   - 运行 `yosys -V`、`nextpnr-ice40 -V`、`iceprog -V` 三个命令
   - 都能打印版本号就说明装好了
   - 如果没装，向助教报告，按教程一键安装包安装

2. **验证开发板连接：**
   - 用 USB 线连 iCE40 开发板
   - 确认电脑识别到下载器（FT2232 或板载下载器）
   - 运行 `iceprog -t` 测试能否读到 iCE40 芯片 ID
   - 能打印出芯片 ID（如 iCE40-HX8K）就说明开发板和下载器都通了

**预期结果 Expected Result:**
- `yosys -V` / `nextpnr-ice40 -V` / `iceprog -V` 都能打印版本号
- `iceprog -t` 能读到 iCE40 芯片 ID

**常见问题 Common Issues:**
- "命令找不到（not recognized）" -- 工具链没装好或没加进 PATH，让助教帮你按一键安装包重装
- "`iceprog -t` 报错找不到下载器" -- 换一根 USB 数据线（有的线只能充电不能传数据），或确认 FT2232 驱动已装
- "读到芯片 ID 但下载失败" -- 试试换 USB 口，或重新插拔下载器

### 任务 1.5: 认识 iCE40 开发板 (30 分钟)

**步骤 Steps:**

1. **观察 iCE40 开发板：**
   - 找到主芯片（Lattice iCE40，型号如 HX8K 或 UP5K）
   - 找到 USB 接口和板载下载器（FT2232 或类似）
   - 找到用户 LED（通常 8 个）
   - 找到用户按键（如果有的话）
   - 找到 PMOD 排针接口（接外设用）

2. **认识外设模块（按 BOM 清点）：**
   - DAC904 模块：14-bit 高速 DAC
   - 运放模块：信号调理
   - 1.8" ST7735 TFT 屏
   - EC11 旋转编码器
   - 面包板 + 杜邦线

3. **记录到实验日志：**
   - iCE40 开发板的型号和可用引脚
   - 各模块的供电电压（3.3V/5V）和接口（SPI/并行/GPIO）
   - 哪些引脚接 LED、哪些接 PMOD

**预期结果 Expected Result:**
- 能在开发板上指出主芯片、USB 接口、LED 的位置
- 记录了关键引脚编号（LED 引脚、PMOD 引脚）

**常见问题 Common Issues:**
- "开发板上芯片很小，看不到型号" -- 用手机放大镜功能或向助教确认
- "模块太多接不上" -- 先清点模块、确认每个模块的引脚定义，再按 wiring-guide 接

### 任务 1.6: 下载器连接与硬件套件检查 (45 分钟)

**什么是烧录？**

把比特流（bitstream）下载到 iCE40 FPGA，用 iceprog 工具通过 USB 完成。你可以把它理解为电脑和 FPGA 之间的"数据线"——没有它，你就无法把设计好的电路"烧录"到 FPGA 中。

Downloading a bitstream to the iCE40 FPGA is done with the iceprog tool over USB. Think of it as the "data cable" between your computer and the FPGA — without it, you cannot "burn" your designed circuits into the FPGA.

**步骤 Steps:**

1. **连接下载器：**
   - 用 USB 线把 iCE40 开发板连到电脑（板载下载器直接 USB；外置 FT2232 下载器按丝印接）
   - **务必在断电状态下接好线再上电！**

2. **上电测试：**
   - 给开发板上电（USB 5V）
   - 确认电源指示灯亮起

3. **用 iceprog 识别器件：**
   - 打开命令行，运行 `iceprog -t`（测试模式）
   - 确认能识别到 iCE40 芯片（打印出芯片 ID）
   - 截图保存

4. **硬件清单最终检查：**

   | 物品 Item | 数量 Qty | 检查 Check |
   |----------|---------|-----------|
   | iCE40 开发板 | 1 | □ |
   | USB 数据线 | 1 | □ |
   | DAC904 模块 | 1 | □ |
   | 运放模块 | 1 | □ |
   | 1.8" ST7735 TFT | 1 | □ |
   | EC11 旋转编码器 | 1 | □ |
   | 杜邦线若干 | 1套 | □ |
   | 面包板 | 1 | □ |
   | 万用表 | 1 | □ |

**预期结果 Expected Result:**
- iceprog 能识别到 iCE40 芯片 ID
- 硬件清单全部核对完毕

**常见问题 Common Issues:**
- "iceprog 找不到器件" -- 检查 USB 线是数据线不是充电线，确认下载器驱动已安装，检查开发板是否上电
- "下载器指示灯不亮" -- 更换 USB 线，更换 USB 口
- "识别到器件但烧录失败" -- 确认 iceprog 版本和开发板型号匹配

---

## 今日作业 | Homework

1. **画图题：** 在实验日志中画出 WavePocket 的完整系统框图，标注 iCE40、DAC904、运放、TFT、编码器等模块，用箭头标出数据流方向
2. **计算题：** WavePocket 的 DDS 使用 32 位相位累加器，时钟频率 30MHz。请计算：
   - (a) 频率分辨率是多少？（提示：30MHz / 2^32）
   - (b) 要输出 5MHz 正弦波，FCW 应设为多少？
   - (c) 要输出 440Hz（标准 A 音），FCW 应设为多少？
3. **思考题：** 为什么 WavePocket 的带宽（~10MHz）小于采样率的一半（~15MHz）？实际中还有哪些因素限制了带宽？
4. **写实验日志：** 按照 `assignments/` 目录的要求，写第一篇实验日志

---

## 明日预告 | Tomorrow's Preview

明天我们将正式开始写 Verilog！你将用 iCE40 开源工具链（Yosys + nextpnr + iceprog）跑通第一个 LED 闪烁工程，让 FPGA 上的 LED 亮起来。这是从"纸上谈兵"到"实战操作"的关键一步！

Tomorrow we start writing Verilog! You will use the iCE40 open-source toolchain (Yosys + nextpnr + iceprog) to get your first LED blink project running, lighting up an LED on the FPGA. This is the critical step from "theory" to "hands-on practice"!

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| TinyAWG 开源项目 (参考) | 立创开源 | https://oshwhub.com/greentor/tinyawg-signal-source |
| DDS 原理详解 | B站视频 | 搜索"DDS 直接数字频率合成 教程" |
| iCE40 工具链入门 | 在线教程 | projectf.io/posts/fpga-verilog-intro/ |
| iCE40 开源工具链 | GitHub | github.com/YosysHQ/icestorm |
| DAC904 数据手册 | PDF | 在 ti.com 搜索 DAC904 |
| iCE40 数据手册 | PDF | 在 lattice.com 搜索 iCE40 |

---

*最后更新：2026-06-23*
*Last updated：2026-06-23*
