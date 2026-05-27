# 课程大纲总览 | Curriculum Overview

## 项目名称 | Project Name

口袋信号发生器 "TinyAWG" | Pocket Signal Generator "TinyAWG"

> 开源参考：https://oshwhub.com/greentor/tinyawg-signal-source

## 课程定位 | Course Positioning

本课程是面向高中生的全日制暑期实践项目，为期 12 天（每天 6~8 小时），通过亲手打造一台口袋任意波形发生器（AWG），系统学习 ZYNQ SoC 架构、FPGA 数字设计、DDS 信号合成、嵌入式软件开发和 LVGL GUI 五大核心领域。

This is a full-time summer practicum course for high school students, spanning 12 days (6-8 hours per day). By building a pocket Arbitrary Waveform Generator (AWG) from scratch, students will systematically learn five core areas: ZYNQ SoC architecture, FPGA digital design, DDS signal synthesis, embedded software development, and LVGL GUI.

### TinyAWG 核心参数 | TinyAWG Key Specs

| 参数 Parameter | 规格 Specification |
|--------------|-------------------|
| 主控 SoC | Xilinx ZYNQ-7010 (ARM Cortex-A9 PS + Artix-7 FPGA PL) |
| DAC | AD9744, 14-bit, 200 MSa/s |
| 输出带宽 | 35 MHz |
| 输出幅度 | 10 Vpp |
| 输出运放 | OPA2673 双通道高速运放 |
| 辅助 DAC | DAC8562 (16-bit, 双通道, 用于偏置/增益控制) |
| 显示 | 2.8" TFT 触摸屏, FT6336 触控 IC |
| GUI 框架 | LVGL 8.3.1 |
| 供电 | 内置锂电池 |
| 参考成本 | ~220 元 |

## 学习目标 | Learning Objectives

完成本课程后，学生将能够：

Upon completing this course, students will be able to:

1. **理解 AWG 工作原理** -- 掌握 DDS（直接数字频率合成）原理、相位累加器、查找表、DAC 重建滤波等核心概念
2. **理解 ZYNQ SoC 架构** -- 了解 PS（Processing System, ARM）与 PL（Programmable Logic, FPGA）的分工协作模型
3. **掌握 FPGA 设计基础** -- 能使用 Verilog 编写组合逻辑与时序逻辑模块，完成综合与实现流程
4. **实现 DDS 信号合成** -- 在 FPGA 中构建相位累加器与波形查找表，输出正弦波、方波、三角波、锯齿波
5. **编写 ARM 嵌入式软件** -- 使用 Vitis IDE 开发 PS 端 C 程序，通过 AXI 总线控制 PL 端 IP 核
6. **开发 LVGL 图形界面** -- 使用 LVGL 8.3.1 构建触摸交互界面，实现参数调节与波形预览
7. **完成 PCB 组装与调试** -- 焊接贴片元件，调试模拟输出通路（DAC + 运放 + 滤波）
8. **撰写技术文档** -- 编写清晰的 README、系统架构图、寄存器说明和项目报告

## 三阶段课程结构 | Three-Phase Structure

### 阶段一 Phase 1：数字系统基础与 FPGA | Digital System & FPGA Fundamentals (Day 1-4)

| 天数 | 主题 | 核心知识点 | 关键产出 |
|------|------|-----------|---------|
| Day 1 | 项目启动与 ZYNQ 架构 | AWG 原理、ZYNQ PS/PL 架构、Vivado 开发环境搭建 | Vivado 环境就绪，理解 PS/PL 关系 |
| Day 2 | 数字逻辑与 Verilog 基础 | 布尔代数、组合逻辑、时序逻辑、Verilog 语法 | LED 闪烁、按键消抖、多路选择器 |
| Day 3 | FPGA 设计流程 | Vivado 工程、约束文件 (XDC)、综合、实现、Bitstream 生成 | 在 PL 端运行第一个自定义模块 |
| Day 4 | DDS 原理与 FPGA 实现 | 相位累加器、波形查找表 (LUT)、频率控制字 (FTW)、DAC 接口 | FPGA 输出正弦波数据，逻辑分析仪验证 |

**阶段目标：** 学生理解 ZYNQ 架构，能用 Verilog 在 PL 端实现 DDS 核心逻辑，并通过仿真验证波形数据。
**Phase Goal:** Students understand the ZYNQ architecture, implement DDS core logic in the PL side using Verilog, and verify waveform data through simulation.

### 阶段二 Phase 2：嵌入式软件与 GUI | Embedded Software & GUI (Day 5-8)

| 天数 | 主题 | 核心知识点 | 关键产出 |
|------|------|-----------|---------|
| Day 5 | ARM PS 开发入门 | Vitis IDE、 standalone BSP、UART 调试、GPIO 控制 | PS 端程序运行，串口输出调试信息 |
| Day 6 | AXI 总线与 PS-PL 通信 | AXI4-Lite IP 核封装、地址映射、寄存器读写 | PS 通过 AXI 控制 PL 端 DDS 参数 |
| Day 7 | LVGL 图形框架入门 | LVGL 8.3.1 移植、显示驱动、触摸驱动 FT6336、基础控件 | 屏幕显示文字与按钮，触摸响应 |
| Day 8 | 完整 GUI 开发 | 波形参数界面、频率/幅度滑块、波形类型选择、实时预览 | 完整交互界面，可设置波形参数 |

**阶段目标：** 学生能在 PS 端运行嵌入式程序，通过 AXI 总线控制 PL 端 FPGA 逻辑，并在触摸屏上构建完整的用户界面。
**Phase Goal:** Students can run embedded programs on the PS side, control PL-side FPGA logic through the AXI bus, and build a complete user interface on the touchscreen.

### 阶段三 Phase 3：系统集成与展示 | System Integration & Presentation (Day 9-12)

| 天数 | 主题 | 核心知识点 | 关键产出 |
|------|------|-----------|---------|
| Day 9 | 模拟输出通路与 DAC 调试 | AD9744 SPI 驱动、OPA2673 运放电路、重建滤波器、DAC8562 偏置控制 | 示波器验证模拟波形输出 |
| Day 10 | PCB 焊接与系统联调 | 贴片焊接技巧、信号完整性检查、电源纹波测量、全链路调试 | 完整硬件组装，系统可运行 |
| Day 11 | 性能优化与功能完善 | 输出校准（频率精度、幅度精度）、附加波形（ARB）、故障排除 | 校准完成，功能完善 |
| Day 12 | 项目展示与总结 | 演示技巧、技术复盘、项目文档完善、Demo 网站发布 | 最终展示，项目报告 |

**阶段目标：** 学生完成完整硬件组装与系统联调，实现校准后的波形输出，并进行项目展示。
**Phase Goal:** Students complete full hardware assembly and system integration, achieve calibrated waveform output, and present the project.

## 课程时间表 | Course Timeline

```
Day  1  2  3  4  5  6  7  8  9  10 11 12
     |----- Phase 1 -----|----- Phase 2 -----|----- Phase 3 -----|
     数字基础+FPGA          嵌入式软件+GUI        系统集成+展示
     Digital+FPGA           Embedded+GUI          Integration+Demo
         |                     |                     |
      Day 4: DDS           Day 6: AXI通信       Day 9: 模拟输出
      FPGA验证成功          PS控制PL成功           示波器验证波形
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
- **不需要嵌入式 Linux 经验** -- 使用 bare-metal standalone 模式
- **需要兴趣和耐心** -- 这是最重要的！

See `prerequisites.md` for details. In brief:

- **Basic circuit knowledge required** -- voltage, current, resistance, Ohm's law
- **Familiarity with binary and hexadecimal** -- digital system basics
- **No FPGA experience required** -- we start from logic gates
- **No Verilog experience required** -- Day 2 starts from scratch
- **No embedded Linux experience required** -- we use bare-metal standalone mode
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
