# 口袋信号发生器 "TinyAWG" | Pocket Signal Generator "TinyAWG"

## 项目简介 | Project Overview

你是否好奇电子工程师如何产生精确的测试信号？信号发生器（AWG，任意波形发生器）就是电子实验室的"声带"——它能产生正弦波、方波、三角波，甚至你自己手绘的任意波形。在这个项目中，你将从零开始，亲手打造一台专业级的口袋信号发生器！

Have you ever wondered how electronic engineers generate precise test signals? An Arbitrary Waveform Generator (AWG) is the "voice box" of the electronics lab -- it produces sine waves, square waves, triangle waves, and even hand-drawn arbitrary waveforms. In this project, you will build a professional-grade pocket signal generator from scratch!

本项目基于开源项目 [TinyAWG](https://oshwhub.com/greentor/tinyawg-signal-source)，使用 Xilinx ZYNQ7010 作为主控（集成了 ARM Cortex-A9 处理器 + FPGA 可编程逻辑），配合 14 位高速 DAC（AD9744）、运算放大器输出级、触摸屏 GUI（LVGL），构建一台 200MSa/s 采样率、35MHz 带宽、10Vpp 输出的专业级任意波形发生器。你将学习 FPGA 数字设计、嵌入式 C 编程、DDS（直接数字合成）原理、模拟电路设计，并最终完成整机组装和调试。

This project is based on the open-source [TinyAWG](https://oshwhub.com/greentor/tinyawg-signal-source), using Xilinx ZYNQ7010 as the main controller (integrating an ARM Cortex-A9 processor + FPGA programmable logic), combined with a 14-bit high-speed DAC (AD9744), op-amp output stage, touchscreen GUI (LVGL), to build a professional-grade arbitrary waveform generator with 200MSa/s sampling rate, 35MHz bandwidth, and 10Vpp output. You will learn FPGA digital design, embedded C programming, DDS (Direct Digital Synthesis) principles, analog circuit design, and complete final assembly and debugging.

## 最终效果 | Final Result

完成本项目后，你将拥有一台能够：

Upon completing this project, you will own a device that can:

- **产生多种标准波形** | Generate standard waveforms: sine, square, triangle, ramp, pulse, noise, sinc, exponential decay, ECG, etc.
- **输出任意自定义波形** | Output arbitrary custom waveforms via PC software upload or hand-drawing
- **调节幅度和偏置** | Adjust output amplitude (0~10Vpp) and offset (±5V)
- **扫频输出** | Perform frequency sweep from start to stop frequency
- **AM 信号调制** | Amplitude modulation with configurable carrier and modulation frequencies
- **触摸屏交互** | Interactive GUI with 2.8" touchscreen display
- **电池供电便携** | Battery-powered portable operation

预期参数 | Expected Specifications：

| 参数 Parameter | 指标 Specification |
|------|------|
| 采样率 Sampling Rate | 200MSa/s (FPGA DDS) |
| 分辨率 Resolution | 14-bit (AD9744 DAC) |
| 带宽 Bandwidth | 35MHz (-3dB), 单频可至 70MHz+ |
| 波形点数 Waveform Points | 65536 (64K) |
| 输出幅度 Output Amplitude | 0~10Vpp 可调 |
| 输出偏置 Output Offset | ±5V 可调 |
| 输出阻抗 Output Impedance | 50Ω |
| 频率范围 Frequency Range | 1Hz ~ 70MHz+ |
| 显示 Display | 2.8" IPS LCD, 触摸屏 |
| 供电 Power | 内置锂电池 / 外部 9-12V DC |
| 通信接口 Interface | USB 串口 (上位机通信) |

## 核心技术 | Core Technologies

| 技术 Technology | 用途 Purpose | 说明 Description |
|----------------|-------------|-----------------|
| ZYNQ7010 (XC7Z010) | 主控制器 | ARM Cortex-A9 (PS) + FPGA Artix-7 (PL) |
| FPGA DDS | 波形合成 | 基于 BRAM 的流水线 DDS, 200MHz 时钟 |
| AD9744 | 数模转换 | 14-bit, 210MSPS DAC |
| OPA2673IRGVT | 输出放大 | 高速双运放, 信号调理和放大 |
| DAC8562SDGSR | 辅助 DAC | 幅度和偏置控制 |
| MAX17048 | 电量监测 | I2C 电池 SOC 监测 |
| FT6336 | 触摸控制 | I2C 电容触摸控制器 |
| LVGL 8.3.1 | 图形界面 | 嵌入式 GUI 框架 |
| Vivado | FPGA 开发 | Xilinx 综合开发环境 |
| Vitis / SDK | ARM 开发 | 嵌入式软件编译 |

## 硬件清单 | Hardware List

| 部件 Component | 规格 Specification | 数量 | 参考价格 Price | 购买建议 Source |
|---------------|-------------------|------|--------------|----------------|
| ZYNQ7010 核心板 | 开源核心板, 含 PS+PL | 1 | 60 元 | 立创开源打样 |
| AD9744 DAC 芯片 | 14-bit, 210MSPS | 1 | 10 元 | 淘宝/立创商城 |
| OPA2673IRGVT 运放 | 高速双运放 | 1 | 6 元 | 淘宝/立创商城 |
| DAC8562SDGSR | 16-bit 双通道 DAC | 1 | 7 元 | 淘宝/立创商城 |
| 信号继电器 | 高频 RF 继电器 | 2 | 16 元 | 淘宝 |
| MCX 母座 | 射频连接器 | 4 | 8 元 | 淘宝 |
| 2.8" IPS LCD | 含触摸, FT6336 | 1 | 63 元 | 淘宝搜索"2.8寸 IPS 触摸屏" |
| LMR544006 DCDC | 正负电源转换 | 2 | 6 元 | 立创商城 |
| 充电芯片 | 宽电压充电 | 1 | 3 元 | 立创商城 |
| 升压芯片 | 5V→12V 升压 | 1 | 2 元 | 立创商城 |
| MAX17048 | 电量计芯片 | 1 | 3 元 | 立创商城 |
| 板对板连接器 | 核心板接口 | 1 对 | 9 元 | 淘宝 |
| WS2812 LED | 状态指示灯 | 1 | 1 元 | 淘宝 |
| 锂电池 | 3.7V, 1500mAh | 1 | 20 元 | 淘宝 |
| 电阻电容 | 各值阻容 | 1 套 | 15 元 | 淘宝/立创商城 |
| Type-C 接口 | USB 供电+串口 | 1 | 2 元 | 淘宝 |
| PCB 打板 | 嘉立创, 4层 | 5 片 | 30 元 | jlcpcb.com |
| 3D 打印外壳 | 树脂材料 | 1 | 50 元 | 嘉立创 3D 打印 |
| **合计 Total** | | | **约 220 元** | **远低于 500 元预算** |

> 注：以上为参考价格，实际价格可能因市场波动略有变化。ZYNQ 核心板可参考立创开源项目自行打样。
> Note: Prices are approximate and may vary. The ZYNQ core board can be fabricated via the open-source project on OSHWHub.

## 软件环境 | Software Environment

| 工具 Tool | 版本 Version | 用途 Purpose | 获取方式 |
|-----------|-------------|-------------|---------|
| Vivado Design Suite | 2023.2+ | FPGA/PL 设计、综合、实现 | Xilinx 官网免费下载 (WebPACK) |
| Vitis Unified IDE | 2023.2+ | ARM/PS 嵌入式软件开发 | 随 Vivado 安装 |
| CMake | 3.16+ | C 项目构建系统 | cmake.org |
| Git | 最新版 | 版本控制 | git-scm.com |
| 串口终端 | 任意 | 调试输出查看 | SSCOM / PuTTY / minicom |
| Python 3.8+ | 3.8~3.12 | PC 端波形工具 | python.org |
| 嘉立创 EDA | 最新版 | PCB 设计(查看/修改) | lceda.cn 免费 |
| 3D 建模软件 | 任意 | 外壳设计查看 | Fusion 360 / Blender |

## 快速开始 | Quick Start

### 硬件准备 Hardware Setup

1. 按照 `hardware/BOM.md` 采购所有元器件
2. 下载 [TinyAWG 开源项目](https://oshwhub.com/greentor/tinyawg-signal-source) 的 PCB 文件
3. 在嘉立创下单打板（4层板），收到后按 `hardware/assembly-steps.md` 焊接

### 软件准备 Software Setup

1. 安装 Vivado Design Suite（含 Vitis）
2. 下载 TinyAWG 开源项目的 Vivado 工程和 PS 软件代码
3. 编译 FPGA 比特流和 ARM 固件
4. 通过 JTAG 下载到 ZYNQ 核心板

### 第一次运行 First Run

1. 连接 JTAG 调试器（Xilinx Platform Cable 或兼容器）
2. 下载 FPGA 比特流和 ARM 固件
3. 上电后 LCD 屏幕应显示 TinyAWG 主界面
4. 触摸屏操作，选择波形类型并调节参数
5. 通过 MCX 输出端口连接示波器验证波形

## 课程安排 | Course Schedule

12天全日制课程，分为三个阶段：

12-day full-time course, divided into three phases:

### 第一阶段 Phase 1：数字系统基础与 FPGA | Digital System & FPGA Fundamentals (Day 1-4)

| 天数 Day | 主题 Topic | 核心内容 Key Content |
|----------|----------|---------------------|
| Day 1 | 项目启动与信号发生器原理 | AWG 工作原理、ZYNQ 架构概览、开发环境搭建 |
| Day 2 | Vivado 工程与 ZYNQ 硬件配置 | Vivado 工程创建、ZYNQ Block Design、时钟配置 |
| Day 3 | FPGA 基础与 Verilog 入门 | 组合逻辑、时序逻辑、LED 闪烁实验 |
| Day 4 | DDS 原理与 BRAM 波形存储 | DDS 直接数字合成原理、BRAM 配置、AXI Lite 接口 |

### 第二阶段 Phase 2：嵌入式软件与 GUI | Embedded Software & GUI (Day 5-8)

| 天数 Day | 主题 Topic | 核心内容 Key Content |
|----------|----------|---------------------|
| Day 5 | ARM 软件开发基础 | Vitis 工程、CMake 构建、GPIO/SPI/I2C 驱动 |
| Day 6 | DAC 驱动与输出控制 | AD9744 高速 DAC 驱动、DAC8562 幅度/偏置控制 |
| Day 7 | LVGL 显示与触摸驱动 | LCD 驱动、FT6336 触摸、LVGL 移植 |
| Day 8 | GUI 界面设计与波形控制 | 波形参数界面、事件处理、扫频/调制界面 |

### 第三阶段 Phase 3：系统集成与展示 | System Integration & Presentation (Day 9-12)

| 天数 Day | 主题 Topic | 核心内容 Key Content |
|----------|----------|---------------------|
| Day 9 | 系统集成与联调 | PS-PL 通信、波形输出测试、上位机通信 |
| Day 10 | PCB 组装与焊接 | PCB 焊接、电源测试、信号通路验证 |
| Day 11 | 外壳设计与整机装配 | 3D 打印外壳、电池安装、整机装配调试 |
| Day 12 | 测试校准与项目展示 | 性能测试、校准、最终展示、技术复盘 |

详细每日课程内容请查看 `curriculum/day-01.md` 至 `curriculum/day-12.md`。

For detailed daily course content, see `curriculum/day-01.md` through `curriculum/day-12.md`.

## 评分标准 | Grading Rubric

| 维度 Dimension | 权重 Weight | 说明 Description |
|----------------|-----------|-----------------|
| 技术实现 Technical Implementation | 35% | 功能完整度、波形输出质量、频率精度 |
| 文档质量 Documentation | 9% | README、注释、原理图、技术说明 |
| 演示展示 Presentation | 17% | 最终展示的清晰度、深度与表达能力 |
| 进度汇报 Check-ins | 17% | 每周进度报告的质量与及时性 |
| 团队协作 Collaboration | 9% | Git 记录、分工合理性、互助精神 |
| Demo 网站 Project Demo Website | 13% | 项目展示网站的内容完整性、视觉呈现、技术深度 |

## 项目结构 | Project Structure

```
project-04-pocket-oscilloscope/
├── README.md                          # 本文件，项目总览
├── curriculum/                        # 课程文档
│   ├── overview.md                    # 课程大纲总览
│   ├── prerequisites.md               # 前置知识与准备
│   ├── assignments.md                 # 作业说明
│   ├── grading-rubric.md              # 详细评分标准
│   └── day-01.md ~ day-12.md          # 每日课程内容
├── hardware/                          # 硬件相关
│   ├── BOM.md                         # 物料清单
│   ├── wiring-guide.md                # 接线/电路指南
│   ├── assembly-steps.md              # 组装步骤
│   └── troubleshooting.md             # 硬件故障排除
├── software/                          # 软件代码
│   ├── requirements.txt               # Python 依赖 (PC 工具)
│   ├── config.template.yaml           # 配置模板
│   ├── src/                           # 源代码
│   │   ├── main.c                     # 主程序入口 (ZYNQ PS)
│   │   ├── dds_controller.c           # DDS 控制器
│   │   ├── waveform.c                 # 波形数据处理
│   │   ├── output_ctrl.c              # 输出控制
│   │   ├── dac_driver.c               # DAC 驱动
│   │   ├── gui_helper.c               # GUI 辅助函数
│   │   └── utils.c                    # 工具函数
│   └── tests/
│       └── test_basic.py              # 基础测试 (PC 端)
├── assignments/                       # 作业与评分
│   ├── week-1-checkin.md              # 第一周进度报告
│   ├── week-2-checkin.md              # 第二周进度报告
│   ├── final-presentation.md          # 最终展示要求
│   └── rubric.md                      # 详细评分表
└── resources/
    └── search-results.md              # 资源搜索结果
```

## 学习资源 | Learning Resources

### 开源参考项目 | Open Source Reference

| 项目 Project | 链接 Link | 说明 Description |
|-------------|----------|-----------------|
| TinyAWG 信号源 (本项目核心参考) | [OSHWhub](https://oshwhub.com/greentor/tinyawg-signal-source) | 35MHz 带宽 200MSa/s 14位任意波发生器 |
| TinyAWG 制作文档 | [飞书文档](https://ai.feishu.cn/docx/BGycd4URIot8Aoxs3vLcBdq2nFc) | 详细制作教程和注意事项 |
| ZYNQ7020 核心板开源项目 | [OSHWhub](https://oshwhub.com/z_star/zynq7020-core-board-and-various-rf-modules) | ZYNQ 核心板设计文件 |
| AWG_DHO8-900 | [GitHub](https://github.com/MatthiasElectronic/AWG_DHO8-900) | 输出级电路参考设计 |

### 推荐 B 站视频 | Recommended Bilibili Videos

1. **TinyAWG 信号源演示视频**
   - https://www.bilibili.com/video/BV1a42QBUECC/
   - 本项目实物展示和功能演示

2. **ZYNQ 入门教程系列** -- 搜索"ZYNQ 入门 Vivado 教程"
   - 理解 PS (ARM) 和 PL (FPGA) 协同工作

3. **FPGA Verilog 入门** -- 搜索"Verilog 入门 教程"
   - FPGA 开发基础

4. **DDS 直接数字合成原理** -- 搜索"DDS 原理 讲解"
   - 理解 DDS 波形生成核心原理

5. **LVGL 嵌入式 GUI 开发** -- 搜索"LVGL 入门 教程"
   - GUI 开发框架入门

### 参考书籍与文档 | Reference Books & Docs

- ZYNQ-7000 SoC Technical Reference Manual (UG585) -- Xilinx 官网
- Vivado Design Suite User Guide -- Xilinx 官网
- AD9744 Datasheet -- Analog Devices
- LVGL 8.3 Documentation -- https://docs.lvgl.io/
- 《FPGA 权威指南》-- Vivado 开发参考

## 常见问题 | FAQ

**Q: 这个项目难度适合高中生吗？**
A: 本项目属于高难度（High）级别，需要一定的学习热情和耐心。课程会从 FPGA 和嵌入式开发基础教起，提供大量模板代码和详细指导。关键是有持续学习的动力。

**Q: 需要会 FPGA/Verilog 吗？**
A: 不需要预先掌握。课程 Day 2-4 会从 FPGA 基础开始教起，重点在于理解 DDS 原理和 PS-PL 通信，而非深入 FPGA 设计。

**Q: Vivado 软件对电脑配置要求高吗？**
A: Vivado 对电脑配置有一定要求：建议 8GB 以上内存、100GB 以上磁盘空间、Windows 10/11 或 Linux。建议提前安装，安装过程约需 1-2 小时。

**Q: ZYNQ 核心板怎么获取？**
A: 可通过立创开源平台下载核心板设计文件，在嘉立创下单打样。也可以参考开源项目直接购买兼容核心板。

**Q: PCB 焊接难度大吗？**
A: 本项目使用较多贴片元件（SMD），需要基本的焊接技能。Day 10 的 PCB 焊接课程会提供详细指导。建议先在练习板上熟悉贴片焊接。

**Q: 可以用更简单的 MCU 代替 ZYNQ 吗？**
A: 不建议。TinyAWG 的 200MSa/s 高速 DDS 输出依赖 FPGA 的并行处理能力，这是普通 MCU 无法实现的。ZYNQ7010 核心板约 60 元，性价比很高。

## 许可证 | License

本项目课程文档采用 [CC BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/) 协议发布。

硬件设计基于 [LGPL 3.0](https://www.gnu.org/licenses/lgpl-3.0.html) 协议（遵循 TinyAWG 原项目协议）。

This project course materials are released under CC BY-NC-SA 4.0. Hardware design follows LGPL 3.0 per the original TinyAWG project.

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
