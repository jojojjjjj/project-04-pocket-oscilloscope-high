# 课程大纲总览 | Curriculum Overview

## 项目名称 | Project Name

口袋信号发生器 "WavePocket" | Pocket Signal Generator "WavePocket"

> 开源参考：https://oshwhub.com/greentor/tinyawg-signal-source （演示视频 BV1a42QBUECC）

## 课程定位 | Course Positioning

本课程是面向高中生的全日制暑期实践项目，为期 20 天（每天 6~8 小时），通过亲手打造一台口袋任意波形发生器（AWG），系统学习 FPGA 数字设计、Verilog、DDS 信号合成、DAC 驱动和显示交互四大核心领域。主线方案用 **iCE40 FPGA 开发板 + 现成模块**（DAC904 模块 + ST7735 TFT + EC11 编码器），跟着教程跑通，不用画板、不用焊贴片。

This is a full-time summer practicum course for high school students, spanning 20 days (6-8 hours per day). By building a pocket Arbitrary Waveform Generator (AWG) from scratch, students will systematically learn four core areas: FPGA digital design, Verilog, DDS signal synthesis, DAC driving, and display/interaction. The main-line plan uses an **iCE40 FPGA dev board + off-the-shelf modules** (DAC904 module + ST7735 TFT + EC11 encoder), following tutorials to get it running -- no PCB design, no SMD soldering.

### WavePocket 核心参数 | WavePocket Key Specs

| 参数 Parameter | 规格 Specification |
|--------------|-------------------|
| 主控 FPGA | Lattice iCE40 (HX8K / UP5K) |
| DAC | DAC904, 14-bit, 高速并行 DAC 模块 |
| 输出带宽 | ~10 MHz |
| 输出幅度 | 可调 (运放模块调理) |
| 显示 | 1.8" SPI TFT (ST7735), 128×160 |
| 交互输入 | EC11 旋转编码器 (选波形/调频/切菜单) |
| 波形查找表 | 1024 点 BRAM LUT (正弦/方波/三角/锯齿) |
| 工具链 | Yosys + nextpnr + IceStorm (全开源免费) |
| 参考成本 | <!-- ~178 元 (全部现成开发板+模块) --> |

## 学习目标 | Learning Objectives

完成本课程后，学生将能够：

Upon completing this course, students will be able to:

1. **理解 AWG 工作原理** -- 掌握 DDS（直接数字频率合成）原理、相位累加器、查找表、DAC 重建滤波等核心概念
2. **理解 FPGA 与 Verilog 基础** -- 了解 FPGA 如何用硬件逻辑并行执行任务，能用 Verilog 写组合逻辑与时序逻辑模块
3. **掌握 FPGA 设计流程** -- 用开源工具链（Yosys 综合 → nextpnr 布局布线 → iceprog 烧录）把 Verilog 变成跑在 iCE40 上的比特流
4. **实现 DDS 信号合成** -- 在 FPGA 中构建相位累加器与波形查找表，输出正弦波、方波、三角波、锯齿波
5. **驱动 DAC 输出模拟波形** -- 理解 DAC904 并行接口时序，把数字波形变成示波器能看见的真实电压
6. **实现显示与交互** -- 用 ST7735 TFT 显示当前波形和频率，用 EC11 编码器选波形、调频率
7. **完成整机集成与调试** -- 开发板+模块整机接线、电源检查、分层调试，让示波器上出现干净的波形
8. **撰写技术文档** -- 编写清晰的 README、系统框图、调试笔记和项目报告

## 四阶段课程结构 | Four-Phase Structure

### 阶段一 Phase 1：FPGA 基础 | FPGA Basics (Day 1-5)

| 天数 | 主题 | 核心知识点 | 关键产出 |
|------|------|-----------|---------|
| Day 1 | 项目启动与 AWG 原理 | AWG 工作原理、DDS 概念、iCE40 开发板认识 | 理解 DDS 流程图，开发板点亮 |
| Day 2 | 开发环境与 Verilog 入门 | Yosys/nextpnr/IceStorm 安装、Verilog 语法 | LED 闪烁工程跑通 |
| Day 3 | 组合与时序逻辑 | 布尔代数、组合逻辑、时序逻辑、按键消抖 | 多路选择器、消抖模块 |
| Day 4 | FPGA 设计流程 | 约束文件 (.pcf)、综合、布局布线、烧录 | 在 iCE40 上跑第一个自定义模块 |
| Day 5 | DDS 原理与相位累加器 | 相位累加器、频率控制字 (FTW)、BRAM 查找表 | FPGA 输出数字波形，仿真验证 |

**阶段目标：** 学生理解 FPGA 与 DDS 原理，能用 Verilog 在 iCE40 上实现 DDS 核心逻辑，并通过仿真验证波形数据。
**Phase Goal:** Students understand FPGA and DDS principles, implement DDS core logic on iCE40 using Verilog, and verify waveform data through simulation.

### 阶段二 Phase 2：DDS 核心 | DDS Core (Day 6-10)

| 天数 | 主题 | 核心知识点 | 关键产出 |
|------|------|-----------|---------|
| Day 6 | 波形查找表 | 用 Python 生成正弦/方波/三角/锯齿 LUT，BRAM 存储 | 多波形 LUT 生成脚本 |
| Day 7 | DAC904 驱动 | 并行 DAC 接口时序、数据格式、示波器验证 | 示波器看到真实模拟波形 |
| Day 8 | 完整 DDS 顶层模块 | dds_top 整合、多波形切换、仿真验证 | dds_top 跑通，多波形切换 |
| Day 9 | 输出幅度调理 | 运放模块接线、幅度/偏置调节 | 输出幅度可调 |
| Day 10 | 频率精度与扫频 | FCW 计算、扫频实现、频率测量对比 | 频率精度达标，扫频可用 |

**阶段目标：** 学生能让 FPGA 通过 DAC 输出真实模拟波形，并理解频率控制与扫频的实现。
**Phase Goal:** Students can make the FPGA output real analog waveforms through the DAC, and understand frequency control and sweep implementation.

### 阶段三 Phase 3：交互显示 | Display & Control (Day 11-15)

| 天数 | 主题 | 核心知识点 | 关键产出 |
|------|------|-----------|---------|
| Day 11 | ST7735 TFT 驱动 | SPI 时序、显示文字/简单图形 | 屏幕显示文字 |
| Day 12 | TFT 显示波形信息 | 显示当前波形类型和频率值 | 屏幕同步显示波形状态 |
| Day 13 | EC11 旋转编码器 | 编码器正交解码、消抖 | 编码器读数稳定 |
| Day 14 | 编码器调参交互 | 旋转选波形、调频率，按压切菜单 | 编码器可改波形和频率 |
| Day 15 | 显示与 DDS 联动 | 屏幕显示与输出波形同步 | 屏幕与输出一致 |

**阶段目标：** 学生能驱动 TFT 显示和编码器交互，实现"选波形/调频率"的完整人机交互。
**Phase Goal:** Students can drive the TFT display and encoder interaction, achieving full "pick waveform / tune frequency" interaction.

### 阶段四 Phase 4：系统集成与展示 | Integration & Presentation (Day 16-20)

| 天数 | 主题 | 核心知识点 | 关键产出 |
|------|------|-----------|---------|
| Day 16 | 整机接线与联调 | 开发板+模块整机连线、电源检查、分层调试 | 整机可运行 |
| Day 17 | 系统测试与校准 | 频率/幅度校准、故障排除 | 校准完成，波形干净 |
| Day 18 | （可选）外壳与装配 | 3D 打印外壳、整机装配 (stretch) | 装壳完成（可选） |
| Day 19 | 文档与 Demo 网站 | README、架构图、Demo 网站 | 文档完整 |
| Day 20 | 项目展示与答辩 | 演示技巧、技术复盘、技术答辩 | 最终展示 |

**阶段目标：** 学生完成整机集成与校准，撰写文档，并进行项目展示。
**Phase Goal:** Students complete full system integration and calibration, write documentation, and present the project.

## 课程时间表 | Course Timeline

```
Day  1  2  3  4  5   6  7  8  9  10   11 12 13 14 15   16 17 18 19 20
     |-- Phase 1 --|-- Phase 2 --|-- Phase 3 --|-- Phase 4 --|
     FPGA 基础       DDS 核心       交互显示       集成与展示
     FPGA Basics     DDS Core       Display/Ctrl   Integration
        |               |              |               |
     Day 5: DDS      Day 7: DAC     Day 14: 编码器   Day 17: 校准
     FPGA 验证成功    示波器见波形    调参可用         波形干净
```

## 教学方法 | Teaching Methods

| 方法 Method | 说明 Description |
|------------|-----------------|
| 项目驱动 Project-Based | 以最终作品为导向，每天的课程都为最终成果添砖加瓦 |
| 动手实验 Hands-On | 每天至少 60% 时间用于动手实验、编码或硬件调试 |
| 费曼教学 Feynman Method | 鼓励学生用自己的话解释技术原理（如"DDS 是怎么合成出正弦波的？"） |
| 结对编程 Pair Programming | FPGA 和 ARM 开发采用两人协作方式完成 |
| 每日回顾 Daily Review | 每天结束前 30 分钟进行知识回顾和答疑 |

## 每日时间安排模板 | Daily Schedule Template

| 时间 Time | 活动 Activity |
|----------|-------------|
| 09:00-09:30 | 晨会：回顾昨日 + 今日目标 Morning standup: review + goals |
| 09:30-10:30 | 知识讲解：新概念与原理 Lecture: new concepts and principles |
| 10:30-10:45 | 休息 Break |
| 10:45-12:00 | 动手实验：编码/硬件调试 Hands-on: coding or hardware debugging |
| 12:00-13:30 | 午餐 + 休息 Lunch + rest |
| 13:30-15:30 | 动手实验：继续开发 Hands-on: continued development |
| 15:30-15:45 | 休息 Break |
| 15:45-17:00 | 自由探索 + 答疑 Open exploration + Q&A |
| 17:00-17:30 | 每日回顾 + 布置作业 Daily review + assign homework |

## 前置要求 | Prerequisites

详见 `prerequisites.md`。简要来说：

- **需要基本电路知识** -- 电压、电流、电阻、欧姆定律
- **了解二进制和十六进制** -- 数字系统基础
- **不需要 FPGA 经验** -- 课程从逻辑门教起
- **不需要 Verilog 经验** -- Day 2 从零开始
- **不需要嵌入式开发经验** -- 主线全在 FPGA 里跑，没有 ARM/PS 软件层
- **需要兴趣和耐心** -- 这是最重要的！

See `prerequisites.md` for details. In brief:

- **Basic circuit knowledge required** -- voltage, current, resistance, Ohm's law
- **Familiarity with binary and hexadecimal** -- digital system basics
- **No FPGA experience required** -- we start from logic gates
- **No Verilog experience required** -- Day 2 starts from scratch
- **No embedded development experience required** -- the main line runs entirely in the FPGA, no ARM/PS software layer
- **Interest and patience required** -- this is what matters most!

## 评估方式 | Assessment

| 评估项 Assessment | 时间 Timing | 权重 Weight |
|------------------|------------|-----------|
| 每日实验成果 Daily Lab Results | 每天 Daily | 贯穿全程 Ongoing |
| 第一周进度报告 Week 1 Check-in | Day 4 | 17% |
| 第二周进度报告 Week 2 Check-in | Day 8 | 17% |
| 最终展示 Final Presentation | Day 12 | 17% |
| 项目文档 Project Documentation | Day 12 | 9% |
| 技术实现 Technical Implementation | Day 12 | 35% |
| Demo 网站 Project Demo Website | Day 12 | 13% |

详细评分标准见 `grading-rubric.md` 和 `assignments/rubric.md`。

For detailed grading criteria, see `grading-rubric.md` and `assignments/rubric.md`.

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
