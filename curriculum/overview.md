# 课程大纲总览 | Curriculum Overview

## 项目名称 | Project Name
口袋示波器 "波形大师" | Pocket Oscilloscope "WaveMaster"

## 课程定位 | Course Positioning

本课程是面向高中生的全日制暑期实践项目，为期 12 天（每天 6~8 小时），通过亲手打造一台数字示波器，系统学习模拟电路、嵌入式编程和信号处理三大核心领域。

This is a full-time summer practicum course for high school students, spanning 12 days (6-8 hours per day). By building a digital oscilloscope from scratch, students will systematically learn three core areas: analog circuits, embedded programming, and signal processing.

## 学习目标 | Learning Objectives

完成本课程后，学生将能够：

Upon completing this course, students will be able to:

1. **理解示波器工作原理** -- 掌握 ADC 采样、波形触发、时基控制等核心概念
2. **设计基本运放电路** -- 能搭建同相/反相放大器、电压跟随器、偏置电路
3. **配置 STM32 外设** -- 使用 CubeMX 配置 ADC、DMA、定时器、SPI 等外设
4. **实现波形显示** -- 在 TFT 屏幕上实时绘制波形、网格和参数
5. **编写触发逻辑** -- 实现上升沿/下降沿检测和自动触发模式
6. **拓展信号发生器** -- 使用 AD9833 DDS 模块生成测试信号
7. **设计 PCB 与外壳** -- 使用嘉立创 EDA 设计 PCB，3D 打印外壳
8. **撰写技术文档** -- 编写清晰的 README、接线图和项目报告

## 三阶段课程结构 | Three-Phase Structure

### 阶段一 Phase 1：模拟电路与 ADC 基础 | Analog Circuit & ADC Fundamentals (Day 1-4)

| 天数 | 主题 | 核心知识点 | 关键产出 |
|------|------|-----------|---------|
| Day 1 | 项目启动与示波器原理 | 示波器架构、STM32 开发环境、GPIO 基础 | 开发环境就绪，LED 闪烁 |
| Day 2 | 运放电路与信号调理 | 同相/反相放大器、电压跟随器、偏置电路 | 面包板运放电路，信号放大验证 |
| Day 3 | ADC 采样原理与 DMA | 12-bit ADC 原理、DMA 搬运、采样率计算 | 串口输出 ADC 采样值 |
| Day 4 | STM32 ADC 配置与数据采集 | CubeMX 配置 ADC+DMA+TIM、连续采样 | 串口打印波形数据，Python 绘图验证 |

**阶段目标：** 学生能通过 STM32 采集模拟信号并输出原始数据。
**Phase Goal:** Students can acquire analog signals through STM32 and output raw data.

### 阶段二 Phase 2：波形处理与显示 | Waveform Processing & Display (Day 5-8)

| 天数 | 主题 | 核心知识点 | 关键产出 |
|------|------|-----------|---------|
| Day 5 | 波形触发逻辑设计 | 边沿检测算法、触发电平、触发模式 | 软件触发函数，波形稳定显示 |
| Day 6 | OLED/TFT 波形显示 | SPI 驱动、像素绘制、波形曲线、网格 | TFT 屏幕实时显示波形 |
| Day 7 | 时基与采样率控制 | 定时器配置、时基档位、采样深度 | 时基可调，波形缩放正常 |
| Day 8 | 频率测量与自动量程 | 周期法测频、过零检测、量程切换 | 频率读数显示，自动量程功能 |

**阶段目标：** 学生能在 TFT 屏幕上实时显示稳定波形，并测量信号频率。
**Phase Goal:** Students can display stable waveforms in real-time on a TFT screen and measure signal frequency.

### 阶段三 Phase 3：功能拓展与展示 | Extension & Presentation (Day 9-12)

| 天数 | 主题 | 核心知识点 | 关键产出 |
|------|------|-----------|---------|
| Day 9 | 信号发生器 DDS | AD9833 SPI 驱动、波形选择、频率设置 | 信号发生器模块输出正弦/方波 |
| Day 10 | PCB 设计与焊接 | 嘉立创 EDA、原理图绘制、PCB 布局 | PCB 设计文件，完成打板下单 |
| Day 11 | 外壳设计与系统集成 | 3D 建模基础、外壳设计、整机装配 | 外壳打印完成，整机可运行 |
| Day 12 | 项目展示与总结 | 演示技巧、技术复盘、项目文档完善 | 最终展示，项目报告 |

**阶段目标：** 学生能拓展信号发生器功能，完成 PCB/外壳设计，并进行项目展示。
**Phase Goal:** Students can extend signal generator functionality, complete PCB/case design, and present the project.

## 课程时间表 | Course Timeline

```
Day  1  2  3  4  5  6  7  8  9  10 11 12
     |----- Phase 1 -----|----- Phase 2 -----|----- Phase 3 -----|
     模拟电路+ADC基础       波形处理+显示         拓展+展示
     Analog+ADC            Waveform+Display     Extension+Present
```

## 教学方法 | Teaching Methods

| 方法 Method | 说明 Description |
|------------|-----------------|
| 项目驱动 Project-Based | 以最终作品为导向，每天的课程都为最终成果添砖加瓦 |
| 动手实验 Hands-On | 每天至少 60% 时间用于动手实验和编码 |
| 费曼教学 Feynman Method | 鼓励学生用自己的话解释技术原理 |
| 结对编程 Pair Programming | 关键代码模块采用两人协作方式完成 |
| 每日回顾 Daily Review | 每天结束前 30 分钟进行知识回顾和答疑 |

## 每日时间安排模板 | Daily Schedule Template

| 时间 Time | 活动 Activity |
|----------|-------------|
| 09:00-09:30 | 晨会：回顾昨日 + 今日目标 |
| 09:30-10:30 | 知识讲解：新概念与原理 |
| 10:30-10:45 | 休息 Break |
| 10:45-12:00 | 动手实验：编码与调试 |
| 12:00-13:30 | 午餐 + 休息 |
| 13:30-15:30 | 动手实验：继续开发 |
| 15:30-15:45 | 休息 Break |
| 15:45-17:00 | 自由探索 + 答疑 |
| 17:00-17:30 | 每日回顾 + 布置作业 |

## 前置要求 | Prerequisites

详见 `prerequisites.md`。简要来说：
- **无需编程基础** -- 课程会从头教起
- **无需电子基础** -- 从最基本的电路知识开始
- **需要兴趣和耐心** -- 这是最重要的！

See `prerequisites.md` for details. In brief:
- **No programming experience required** -- we start from scratch
- **No electronics background needed** -- we begin with basic circuit knowledge
- **Interest and patience required** -- this is what matters most!

## 评估方式 | Assessment

| 评估项 Assessment | 时间 Timing | 权重 Weight |
|------------------|------------|-----------|
| 每日实验成果 Daily Lab Results | 每天 | 贯穿全程 |
| 第一周进度报告 Week 1 Check-in | Day 5 | 17% (10分) |
| 第二周进度报告 Week 2 Check-in | Day 9 | 17% (10分) |
| 最终展示 Final Presentation | Day 12 | 17% (20分) |
| 项目文档 Project Documentation | Day 12 | 9% (10分) |
| 技术实现 Technical Implementation | Day 12 | 35% (40分) |
| Demo 网站 Project Demo Website | Day 12 | 13% (15分) |

详细评分标准见 `grading-rubric.md` 和 `assignments/rubric.md`。

*最后更新：2026-05-26*
*Last updated: 2026-05-26*
