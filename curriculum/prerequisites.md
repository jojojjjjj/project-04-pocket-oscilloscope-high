# 前置知识与准备 | Prerequisites & Preparation
<!-- 注：价格信息已注释隐藏（查价不准确，仅作参考不严谨）/ Note: price info commented out (inaccurate, not rigorous as reference) -->

## 概述 | Overview

本课程为零基础友好设计，但提前做好准备能让你的学习更加顺畅。本文档列出你需要了解的前置知识、需要准备的软硬件环境，以及推荐的预习资源。

This course is designed to be beginner-friendly, but preparation in advance will make your learning smoother. This document lists the prerequisite knowledge, required software/hardware setup, and recommended pre-study resources.

---

## 知识要求 | Knowledge Requirements

### 必须掌握 (入门级) | Required (Beginner Level)

| 知识点 Knowledge | 说明 Description | 预习时间 Est. Time |
|-----------------|-----------------|---------|
| 基本电路概念 | 电压(V)、电流(I)、电阻(R)、欧姆定律 V=IR | 1 小时 |
| 串联与并联电路 | 理解串并联的基本特性 | 30 分钟 |
| 电容基础 | 知道电容能储存电荷，理解充放电 | 30 分钟 |
| 二进制与十六进制 | 理解 0/1 表示、十六进制(0x)前缀、位运算基础 | 1 小时 |
| 基本电脑操作 | 文件管理、软件安装、USB 设备使用 | -- |

### 了解即可 (课程中会教) | Nice to Know (Taught in Course)

| 知识点 Knowledge | 说明 Description | 课程覆盖 |
|-----------------|-----------------|---------|
| C 语言基础 | 变量、循环、函数、数组、指针概念 | Day 5-6 逐步讲解 |
| 数字逻辑基础 | 与/或/非门、真值表、组合/时序逻辑 | Day 2 从头教起 |
| 信号基础 | 什么是正弦波、频率、幅度、相位 | Day 1 |
| 运算放大器 | 反相/同相放大、电压跟随器 | Day 9 |
| PCB 基础 | 电路板的基本组成 | Day 10 从头教起 |

### 不需要掌握 | Not Required

- FPGA 开发经验 -- 课程从逻辑门教起
- Verilog / VHDL 语言 -- Day 2 从零开始
- 嵌入式 Linux 经验 -- 使用 standalone bare-metal 模式
- Xilinx 工具链使用经验 -- Day 1 手把手安装和配置
- 高频模拟电路设计经验 -- Day 9 从基础教起

---

## 硬件准备 | Hardware Preparation

### 课程提供 (学校实验室) | Provided by School Lab

以下设备由学校实验室统一提供：

- 万用表 (用于调试电压和通断)
- 烙铁与焊接工具 (Day 10 使用，含贴片焊接头)
- 示波器 (用于验证波形输出，至少双通道 50MHz)
- JTAG 调试器 (用于 ZYNQ 下载 Bitstream 和调试 PS 程序)
- 逻辑分析仪 (可选，用于验证 FPGA 数字信号)

### 个人购买清单 | Personal Purchase List

以下模块为个人套装，课程开始前请准备好：

| 物品 Item | 数量 | 参考价格 | 备注 Notes |
|-----------|------|---------|-----------|
| TinyAWG ZYNQ7010 核心板套件 | 1 套 | <!-- ~220 元 --> | 含 ZYNQ7010 核心板 + 底板 + 屏幕，淘宝/立创开源硬件平台搜索 "TinyAWG" |
| Micro-USB 或 Type-C 数据线 | 1 | <!-- 5 元 --> | 确保是数据线而非充电线 |
| 杜邦线套装 | 1 套 | <!-- 5 元 --> | 公对公+公对母，调试用 |
| 面包板 830孔 (可选) | 1 | <!-- 8 元 --> | 前期实验用 |

**<!-- 总计约 220 元 -->** (核心板套件已含大部分元器件)

> 提示：TinyAWG 是开源项目，PCB 可直接从嘉立创下单打样。建议在淘宝搜索 "ZYNQ7010 开发板" 或 "TinyAWG" 购买预焊接好的完整套件，省去前期贴片焊接的麻烦。
> Tip: TinyAWG is an open-source project. PCBs can be ordered from JLC (嘉立创). Search "ZYNQ7010 dev board" or "TinyAWG" on Taobao for pre-assembled kits, saving you from SMD soldering initially.

---

## 软件环境准备 | Software Environment Setup

请在课程开始前完成以下软件安装。Vivado 安装包非常大，请务必提前下载。如遇到问题，可参考各软件官网的安装指南。

Please complete the following software installations before the course begins. The Vivado installer is very large, so be sure to download it in advance. If you encounter issues, refer to each software's official installation guide.

### 必装软件 | Required Software

#### 1. Vivado Design Suite 2023.2+ (FPGA 开发)

- **下载地址：** https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/2023-2.html
- **版本要求：** 2023.2 或更新版本 (选择 Vivado MLSD -- 免费版，足够本项目)
- **安装步骤：**
  1. 注册 AMD/Xilinx 官网账号（免费）
  2. 下载 Vivado MLSD (Web Installer 或 Full Image)
  3. 安装时选择 "Vivado" + "Vitis" (Vitis 统一 IDE 包含在内)
  4. 安装完成后，添加 ZYNQ-7000 器件支持
- **预计时间：** 2~3 小时 (下载 + 安装)
- **磁盘空间：** ~60 GB (含 Vitis)
- **注意：** 安装路径不要含中文或空格；建议安装在 SSD 上以提升编译速度

#### 2. Vitis Unified IDE (ARM 开发)

- **说明：** Vitis 已包含在 Vivado 安装包中，安装 Vivado 时勾选即可
- **用途：** 编译 PS 端 ARM Cortex-A9 C/C++ 程序
- **依赖：** 需要 Vivado 先安装完成

#### 3. CMake 3.16+

- **下载地址：** https://cmake.org/download/
- **用途：** LVGL 和 ARM 软件项目的构建系统
- **安装步骤：** 下载安装包，安装时勾选 "Add CMake to system PATH"

#### 4. Git

- **下载地址：** https://git-scm.com/download/win
- **用途：** 版本管理、代码提交
- **安装步骤：** 按默认选项安装即可

#### 5. 串口终端 | Serial Terminal

- **推荐工具：** MobaXterm (推荐，功能全面) / PuTTY / Tera Term
- **下载地址：** https://mobaxterm.mobatek.net/download.html (免费版 Home Edition)
- **用途：** 查看 PS 端 UART 调试输出

### 选装软件 | Optional Software

#### 6. Python 3.8+ (PC 端工具)

- **下载地址：** https://www.python.org/downloads/
- **安装步骤：**
  1. 下载 Python 3.10 或更新版本
  2. 安装时勾选 "Add Python to PATH"
  3. 打开命令行，运行 `pip install numpy matplotlib`
- **用途：** 数据分析、波形可视化脚本

#### 7. 嘉立创 EDA (PCB 查看)

- **下载地址：** https://lceda.cn/
- **版本要求：** 嘉立创 EDA 专业版 (免费)
- **用途：** 查看 TinyAWG 原理图和 PCB 布局
- **注意：** 可在线使用，也可下载客户端

---

## 硬件环境检查清单 | Hardware Environment Checklist

课程开始前一天，请逐项检查以下内容：

The day before the course begins, please check each item:

- [ ] Vivado 能正常打开，已安装 ZYNQ-7000 器件支持 (Help > About 确认版本)
- [ ] Vitis 能正常启动 (开始菜单搜索 "Vitis")
- [ ] CMake 已安装，命令行运行 `cmake --version` 能显示版本号
- [ ] Git 已安装，命令行运行 `git --version` 能显示版本号
- [ ] 串口终端软件已安装并能正常运行
- [ ] ZYNQ7010 核心板收到，外观完好无损
- [ ] USB 数据线能连接核心板并被电脑识别
- [ ] 电脑有至少 100 GB 可用磁盘空间 (Vivado 工程 + 编译产物)
- [ ] 网络连接正常 (下载代码、查阅 Xilinx 文档)

---

## 预习任务 | Pre-Study Tasks

如果课程开始前有 2~3 天时间，强烈建议完成以下预习：

If you have 2-3 days before the course starts, it is strongly recommended to complete the following:

### 任务 1：理解欧姆定律与基本电路 (30 分钟)

在 B 站搜索 "欧姆定律 入门"，观看一个基础教学视频。理解以下公式：
- V = I * R (电压 = 电流 x 电阻)
- P = V * I (功率 = 电压 x 电流)

### 任务 2：了解信号发生器是什么 (30 分钟)

B 站搜索 "信号发生器 原理" 或 "AWG 任意波形发生器"，理解：
- 信号发生器与示波器的区别
- 什么是任意波形（相比固定正弦/方波）
- 为什么需要信号发生器（测试和校准的用途）

### 任务 3：安装 Vivado (2~3 小时)

按照上面 "软件环境准备" 部分，下载并安装 Vivado MLSD 2023.2+。安装过程较长，请预留充足时间。

### 任务 4：了解二进制与十六进制 (30 分钟)

B 站搜索 "二进制 十六进制 入门"，理解：
- 二进制如何表示数字 (如 1010 = 10)
- 十六进制前缀 0x (如 0xFF = 255)
- 为什么计算机使用二进制
- 基本位运算：与(&)、或(|)、非(~)、异或(^)

### 任务 5：了解 FPGA 是什么 (30 分钟)

B 站搜索 "FPGA 入门 介绍"，观看一个介绍视频，了解：
- 什么是 FPGA (Field Programmable Gate Array)
- FPGA 与 CPU 的本质区别（硬件 vs 软件）
- FPGA 的典型应用场景
- ZYNQ 是什么（ARM + FPGA 二合一芯片）

---

## 常见问题 | FAQ

**Q1: 我是文科生 / 没有任何技术基础，可以参加吗？**
A: 本课程建议有一定理工科兴趣基础。由于涉及 FPGA 和嵌入式开发，完全零基础的文科生可能会感到吃力。但只要你对电子制作有强烈兴趣，愿意投入额外时间，仍然可以参加。最重要的是保持好奇心和学习毅力。

**Q2: 电脑配置有要求吗？**
A: Vivado 是资源密集型软件，建议配置如下：
- **最低：** Windows 10/11, 8 GB 内存, 100 GB 可用 SSD 空间
- **推荐：** Windows 10/11, 16 GB 内存, 200 GB 可用 SSD 空间
- FPGA 综合/实现过程较耗时，SSD 和充足内存能显著缩短等待时间

**Q3: 需要提前学习 Verilog 吗？**
A: 不需要。Day 2 会从最基本的逻辑门和 Verilog 语法开始教起。如果你有额外时间，可以提前浏览一些 Verilog 基础教程，但不做强制要求。

**Q4: 之前学过 STM32 / Arduino，对学 ZYNQ 有帮助吗？**
A: 有帮助。STM32/Arduino 的嵌入式编程经验（C 语言、外设配置思路）会帮助你更快理解 PS 端的 ARM 开发。但 FPGA 部分是完全不同的编程范式（硬件描述 vs 软件指令），需要转换思维方式。

**Q5: 淘宝买的套件到了发现有问题怎么办？**
A: 课程第一天会检查所有人的硬件，有问题的可以当天重新下单或联系卖家售后。建议尽早购买，留出到货时间。TinyAWG 是开源项目，也可以自己从嘉立创下单 PCB 并采购元器件。

**Q6: 可以用 macOS 或 Linux 吗？**
A: Vivado 支持 Linux (RHEL/Ubuntu)，但本课程教程基于 Windows 编写。Linux 用户可以参加，但可能需要自行解决部分环境差异。macOS 不被 Vivado 官方支持，建议使用虚拟机或双系统。

---

## 推荐预习资源 | Recommended Pre-Study Resources

| 资源 Resource | 类型 Type | 链接 Link | 说明 Description |
|--------------|----------|----------|-----------------|
| 《FPGA 入门教程》 | B站搜索 | 搜索 "FPGA 入门 教程" | 大量免费 FPGA 入门视频 |
| 《Verilog 基础语法》 | B站搜索 | 搜索 "Verilog 入门" | Verilog HDL 语法教学 |
| 《ZYNQ 开发入门》 | B站搜索 | 搜索 "ZYNQ 入门 教程" | ZYNQ PS/PL 开发流程 |
| 《DDS 原理讲解》 | B站搜索 | 搜索 "DDS 直接数字频率合成" | DDS 工作原理动画讲解 |
| 《信号发生器使用入门》 | B站视频 | 搜索 "信号发生器 使用教程" | 信号发生器基础知识 |
| Xilinx Vivado 教程 | 官方文档 | https://docs.xilinx.com/ | Vivado 官方使用指南 |
| TinyAWG 开源项目 | 立创开源 | https://oshwhub.com/greentor/tinyawg-signal-source | 项目原始设计文件 |

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
