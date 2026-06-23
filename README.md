# 口袋信号发生器 "TinyAWG" | Pocket Signal Generator "TinyAWG"

## 项目简介 | Project Overview

你是否好奇电子工程师如何产生精确的测试信号？信号发生器（AWG，任意波形发生器）就是电子实验室的"声带"——它能产生正弦波、方波、三角波，甚至你自己手绘的任意波形。在这个项目中，你将从零开始，亲手打造一台口袋信号发生器 "WavePocket"！

Have you ever wondered how electronic engineers generate precise test signals? An Arbitrary Waveform Generator (AWG) is the "voice box" of the electronics lab -- it produces sine waves, square waves, triangle waves, and even hand-drawn arbitrary waveforms. In this project, you will build a pocket signal generator "WavePocket" from scratch!

本项目参考开源项目 [TinyAWG](https://oshwhub.com/greentor/tinyawg-signal-source)（演示视频 [BV1a42QBUECC](https://www.bilibili.com/video/BV1a42QBUECC/)），主线方案改用更适合夏令营的 **Lattice iCE40 FPGA 开发板 + 现成模块**：iCE40 FPGA 负责跑 DDS（直接数字合成）逻辑，14 位高速 DAC 模块（DAC904）把数字波形变成模拟电压，1.8" TFT 彩屏显示当前波形和参数，旋转编码器（EC11）选波形、调频率。整个过程跟着教程跑通现成模块，不需要自己画 4 层板、不需要焊 0402 贴片，把精力放在"DDS 怎么合成出波形"这件最有意思的事上。

This project references the open-source [TinyAWG](https://oshwhub.com/greentor/tinyawg-signal-source) (demo video [BV1a42QBUECC](https://www.bilibili.com/video/BV1a42QBUECC/)). The main-line plan uses a summer-camp-friendly **Lattice iCE40 FPGA dev board + off-the-shelf modules**: the iCE40 FPGA runs DDS (Direct Digital Synthesis) logic, a 14-bit high-speed DAC module (DAC904) turns the digital waveform into an analog voltage, a 1.8" TFT display shows the current waveform and parameters, and a rotary encoder (EC11) selects waveforms and tunes frequency. The whole build follows tutorials with ready-made modules -- no 4-layer PCB design, no 0402 SMD soldering -- so energy goes into the most interesting question: "how does DDS synthesize a waveform?"

## 最终效果 | Final Result

完成本项目后，你将拥有一台能够：

Upon completing this project, you will own a device that can:

- **产生多种标准波形** | Generate standard waveforms: sine, square, triangle, ramp, pulse, noise, etc.
- **输出任意自定义波形** | Output arbitrary custom waveforms via PC upload or hand-drawing
- **调节幅度和偏置** | Adjust output amplitude and offset
- **扫频输出** | Perform frequency sweep from start to stop frequency
- **旋转编码器交互** | Pick waveform and tune frequency with a rotary encoder (EC11)
- **TFT 彩屏显示** | 1.8" SPI TFT shows current waveform type and frequency
- **电池供电便携** | Battery-powered portable operation

预期参数 | Expected Specifications：

| 参数 Parameter | 指标 Specification |
|------|------|
| 采样率 Sampling Rate | ~30MSa/s (iCE40 FPGA DDS) |
| 分辨率 Resolution | 14-bit (DAC904 DAC) |
| 带宽 Bandwidth | ~10MHz (-3dB) |
| 波形点数 Waveform Points | 1024 (BRAM LUT) |
| 输出幅度 Output Amplitude | 可调 (运放模块调理) |
| 频率范围 Frequency Range | 1Hz ~ 10MHz |
| 显示 Display | 1.8" SPI TFT (ST7735) |
| 交互 Input | EC11 旋转编码器 (选波形/调频) |
| 供电 Power | USB 5V / 移动电源 |
| 主控 Controller | Lattice iCE40 (HX8K / UP5K) FPGA |

## 核心技术 | Core Technologies

| 技术 Technology | 用途 Purpose | 说明 Description |
|----------------|-------------|-----------------|
| iCE40 FPGA | 主控逻辑 | Lattice iCE40 (HX8K/UP5K)，纯 FPGA 单板，跟开源工具链跑通 |
| FPGA DDS | 波形合成 | 相位累加器 + BRAM 查找表，开源综合工具 |
| DAC904 | 数模转换 | 14-bit 高速 DAC 模块，并行数据接口 |
| 运放模块 | 输出调理 | 现成运放模块，把 DAC 输出调理到合适幅度 |
| ST7735 TFT | 显示 | 1.8" SPI 彩屏，显示波形类型和频率 |
| EC11 编码器 | 交互输入 | 旋转选波形/调频，按压切换菜单 |
| Yosys + nextpnr | FPGA 开发 | 开源综合 + 布局布线工具链，免费、轻量 |
| IceStorm 工具链 | 比特流下载 | iceprog 下载比特流到 iCE40 |

## 硬件清单 | Hardware List

> 主线方案：**iCE40 FPGA 开发板 + 现成模块**，跟着教程接线即可，不用画板、不用焊贴片。成本远低于 500 元上限。

| 部件 Component | 规格 Specification | 数量 | 参考价格 Price | 购买建议 Source |
|---------------|-------------------|------|--------------|----------------|
| iCE40 FPGA 开发板 | iCE40-HX8K 或 iCE40-UP5K 开发板 | 1 | 80 元 | 淘宝搜索"iCE40 开发板" |
| DAC904 模块 | 14-bit 高速 DAC 模块 (或兼容) | 1 | 15 元 | 淘宝/立创商城 |
| 运放/信号调理模块 | 现成运放模块 | 1 | 8 元 | 淘宝 |
| 1.8" TFT 彩屏 | ST7735 SPI, 128×160 | 1 | 12 元 | 淘宝搜索"1.8寸 ST7735" |
| EC11 旋转编码器 | 含按压开关 | 1 | 5 元 | 淘宝 |
| 面包板 + 杜邦线 | 用于接线 | 1 套 | 10 元 | 淘宝 |
| 电阻电容套件 | 直插/0803, 各值 | 1 套 | 10 元 | 淘宝 |
| USB 供电线 | Type-A/Micro/Type-C (按板子) | 1 | 3 元 | 淘宝 |
| BNC/SMA 输出接口 | 信号输出接线柱 | 1 | 5 元 | 淘宝 |
| 3D 打印外壳 (可选) | 树脂/PLA | 1 | 30 元 | 嘉立创 3D 打印 (可选) |
| **合计 Total** | | | **约 178 元** | **远低于 500 元预算** |

> 注：以上为参考价格，实际价格可能因市场波动略有变化。主线方案全部用现成开发板和模块，不用打板、不用焊贴片芯片。
> Note: Prices are approximate and may vary. The main-line plan uses off-the-shelf dev boards and modules -- no PCB fabrication, no SMD chip soldering.

## 软件环境 | Software Environment

| 工具 Tool | 版本 Version | 用途 Purpose | 获取方式 |
|-----------|-------------|-------------|---------|
| Yosys | 最新版 | Verilog 综合 (开源) | yosyshq.net 免费 |
| nextpnr-ice40 | 最新版 | 布局布线 (开源) | github.com/YosysHQ/nextpnr 免费 |
| IceStorm 工具链 | 最新版 | 比特流打包/下载 | github.com/YosysHQ/icestorm 免费 |
| iceprog / 下载器 | FT2232 或板载 | 烧录 iCE40 | 随开发板 |
| GTKWave | 最新版 | 仿真波形查看 | gtkwave.sourceforge.net 免费 |
| Icarus Verilog | 最新版 | 行为仿真 | iverilog (开源) |
| Git | 最新版 | 版本控制 | git-scm.com |
| Python 3.8+ | 3.8~3.12 | 生成查找表/PC 工具 | python.org |

> iCE40 全套工具链都是开源免费的，安装包小、对电脑配置要求低（普通笔记本即可），不用装几十 GB 的商业软件。

## 快速开始 | Quick Start

### 硬件准备 Hardware Setup

1. 按 `hardware/BOM.md` 采购开发板和模块（全部现成件，下单即用）
2. 在面包板上按 `hardware/wiring-guide.md` 接线：iCE40 ↔ DAC904 ↔ 运放模块 ↔ TFT ↔ 编码器
3. （可选）有余力的同学可参考开源 PCB 自己打板，列为 stretch 拓展

### 软件准备 Software Setup

1. 安装 iCE40 开源工具链（Yosys + nextpnr + IceStorm，一键安装包即可）
2. 下载本项目的 Verilog 工程和 PC 工具代码
3. 综合并生成比特流：`yosys` 综合 → `nextpnr` 布局布线 → `icepack` 打包
4. 用 `iceprog`（或 FT2232 下载器）烧录到 iCE40 开发板

### 第一次运行 First Run

1. 接好线，给开发板和模块供电（USB 5V）
2. 烧录比特流到 iCE40
3. TFT 屏幕应显示当前波形类型和频率
4. 旋转编码器切换波形（正弦/方波/三角/锯齿），按压调频
5. 在 DAC 输出接线柱接示波器，验证波形正确

## 课程安排 | Course Schedule

20天全日制课程，分为四个阶段（与网站时间线一致）：

20-day full-time course, divided into four phases (aligned with the website timeline):

### 第一阶段 Phase 1：FPGA 基础 | FPGA Basics (Day 1-5)

| 天数 Day | 主题 Topic | 核心内容 Key Content |
|----------|----------|---------------------|
| Day 1 | 项目启动与信号发生器原理 | AWG 工作原理、DDS 概念、iCE40 开发板认识 |
| Day 2 | 开发环境与 Verilog 入门 | iCE40 工具链 (Yosys/nextpnr/IceStorm) 安装、Verilog 语法 |
| Day 3 | 组合逻辑与时序逻辑 | LED 闪烁、按键消抖、多路选择器 |
| Day 4 | FPGA 设计流程 | 约束文件 (.pcf)、综合、布局布线、烧录 |
| Day 5 | DDS 原理与相位累加器 | 相位累加器、频率控制字、BRAM 查找表 |

### 第二阶段 Phase 2：DDS 核心 | DDS Core (Day 6-10)

| 天数 Day | 主题 Topic | 核心内容 Key Content |
|----------|----------|---------------------|
| Day 6 | 波形查找表 | 用 Python 生成正弦/方波/三角/锯齿 LUT，BRAM 存储 |
| Day 7 | DAC904 驱动 | 并行 DAC 接口时序、数据格式、示波器验证 |
| Day 8 | 完整 DDS 顶层模块 | dds_top 整合，多波形切换，仿真验证 |
| Day 9 | 输出幅度调理 | 运放模块接线、幅度/偏置调节 |
| Day 10 | 频率精度与扫频 | FCW 计算、扫频实现、频率测量对比 |

### 第三阶段 Phase 3：交互显示 | Display & Control (Day 11-15)

| 天数 Day | 主题 Topic | 核心内容 Key Content |
|----------|----------|---------------------|
| Day 11 | ST7735 TFT 驱动 | SPI 时序、显示文字/简单图形 |
| Day 12 | TFT 显示波形信息 | 显示当前波形类型和频率值 |
| Day 13 | EC11 旋转编码器 | 编码器正交解码、消抖 |
| Day 14 | 编码器调参交互 | 旋转选波形、调频率，按压切菜单 |
| Day 15 | 显示与 DDS 联动 | 屏幕显示与输出波形同步 |

### 第四阶段 Phase 4：系统集成与展示 | Integration & Presentation (Day 16-20)

| 天数 Day | 主题 Topic | 核心内容 Key Content |
|----------|----------|---------------------|
| Day 16 | 整机接线与联调 | 开发板+模块整机连线、电源检查 |
| Day 17 | 系统测试与校准 | 频率/幅度校准、故障排除 |
| Day 18 | （可选）外壳与装配 | 3D 打印外壳、整机装配（stretch） |
| Day 19 | 文档与 Demo 网站 | README、架构图、Demo 网站 |
| Day 20 | 项目展示与答辩 | 5-8 分钟演示、技术答辩、技术复盘 |

详细每日课程内容请查看 `curriculum/day-01.md` 至 `curriculum/day-12.md`（核心 12 天详细讲义，Day 13-20 为集成与展示延伸，按 phase 节奏推进）。

For detailed daily course content, see `curriculum/day-01.md` through `curriculum/day-12.md` (the core 12-day lecture notes; Days 13-20 are integration and presentation extensions paced by phase).

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
│   ├── src/                           # 源代码 (DDS/波形/DAC 驱动参考实现)
│   │   ├── main.c                     # 主程序入口 (参考)
│   │   ├── dds_controller.c           # DDS 控制器
│   │   ├── waveform.c                 # 波形数据处理
│   │   ├── output_ctrl.c              # 输出控制
│   │   ├── dac_driver.c               # DAC 驱动
│   │   ├── gui_helper.c               # 显示辅助函数
│   │   └── utils.c                    # 工具函数
│   └── tests/
│       └── test_basic.py              # 基础测试 (PC 端)
├── assignments/                       # 作业与评分
│   ├── week-1-checkin.md              # 第一周进度报告
│   ├── week-2-checkin.md              # 第二周进度报告
│   ├── final-presentation.md          # 最终展示要求
│   └── rubric.md                      # 详细评分表
└── resources/                         # 资源 (开源工具链/教程链接)
```

## 学习资源 | Learning Resources

### 开源参考项目 | Open Source Reference

| 项目 Project | 链接 Link | 说明 Description |
|-------------|----------|-----------------|
| TinyAWG 信号源 (本项目参考) | [OSHWhub](https://oshwhub.com/greentor/tinyawg-signal-source) | 开源任意波发生器，本项目借鉴其 DDS 思路 |
| TinyAWG 演示视频 | [B站 BV1a42QBUECC](https://www.bilibili.com/video/BV1a42QBUECC/) | 实物展示和功能演示 |
| iCE40 开源工具链 | [projectf.io 教程](https://projectf.io/posts/fpga-verilog-intro/) | iCE40 + Yosys/nextpnr 入门系列 |
| AWG_DHO8-900 | [GitHub](https://github.com/MatthiasElectronic/AWG_DHO8-900) | 输出级电路参考设计 |

### 推荐 B 站视频 | Recommended Bilibili Videos

1. **TinyAWG 信号源演示视频**
   - https://www.bilibili.com/video/BV1a42QBUECC/
   - 本项目实物展示和功能演示

2. **iCE40 / FPGA 入门** -- 搜索"iCE40 教程"或"FPGA Verilog 入门"
   - 用开源工具链在 iCE40 上跑第一个 Verilog 工程

3. **DDS 直接数字合成原理** -- 搜索"DDS 原理 讲解"
   - 理解 DDS 波形生成核心原理

4. **ST7735 TFT 显示** -- 搜索"ST7735 SPI 教程"
   - SPI 显示屏驱动入门

### 参考书籍与文档 | Reference Books & Docs

- iCE40 Family Datasheet -- Lattice 官网
- Yosys / nextpnr / IceStorm 文档 -- github.com/YosysHQ
- DAC904 Datasheet -- Texas Instruments
- ST7735 Datasheet -- Sitronix
- projectf.io FPGA 教程系列 -- iCE40 开发入门参考

## 常见问题 | FAQ

**Q: 这个项目难度适合高中生吗？**
A: 本项目属于高难度（High）级别，需要一定的学习热情和耐心。课程会从 FPGA 基础教起，提供模板代码和详细指导，主线跟着教程跑通现成模块，不要求从零设计。关键是有持续学习的动力。

**Q: 需要会 FPGA/Verilog 吗？**
A: 不需要预先掌握。课程 Day 2-5 会从 FPGA 基础开始教起，重点在于理解 DDS 原理（数字怎么变成波形），而非深入 FPGA 设计。

**Q: iCE40 工具链对电脑配置要求高吗？**
A: 不高。iCE40 全套工具链（Yosys + nextpnr + IceStorm）都是开源免费的，安装包小，普通笔记本就能跑，不像商业 FPGA 软件那样要几十 GB 空间。

**Q: iCE40 开发板怎么获取？**
A: 淘宝搜索"iCE40 开发板"（iCE40-HX8K 或 iCE40-UP5K），直接购买现成开发板即可，不用自己打样。

**Q: 需要焊贴片元件吗？**
A: 主线方案不用。全部用现成开发板和模块 + 面包板接线，不用焊 0402 贴片、不用画 4 层板。有余力的同学可以把"自己打 PCB"作为 stretch 拓展。

**Q: 可以用更简单的 MCU 代替 FPGA 吗？**
A: 普通 MCU 做不了高速 DDS 波形合成（需要并行数据 + 精确时序）。iCE40 FPGA 开发板约 80 元，专门干这件事最合适。

## 许可证 | License

本项目课程文档采用 [CC BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/) 协议发布。

硬件设计基于 [LGPL 3.0](https://www.gnu.org/licenses/lgpl-3.0.html) 协议（遵循 TinyAWG 原项目协议）。

This project course materials are released under CC BY-NC-SA 4.0. Hardware design follows LGPL 3.0 per the original TinyAWG project.

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
