# 前置知识与准备 | Prerequisites & Preparation

## 概述 | Overview

本课程为零基础友好设计，但提前做好准备能让你的学习更加顺畅。本文档列出你需要了解的前置知识、需要准备的软硬件环境，以及推荐的预习资源。

This course is designed to be beginner-friendly, but preparation in advance will make your learning smoother. This document lists the prerequisite knowledge, required software/hardware setup, and recommended pre-study resources.

---

## 知识要求 | Knowledge Requirements

### 必须掌握 (入门级) | Required (Beginner Level)

| 知识点 Knowledge | 说明 Description | 预习时间 |
|-----------------|-----------------|---------|
| 基本电路概念 | 电压(V)、电流(I)、电阻(R)、欧姆定律 V=IR | 1 小时 |
| 串联与并联电路 | 理解串并联的基本特性 | 30 分钟 |
| 电容基础 | 知道电容能储存电荷，理解充放电 | 30 分钟 |
| 基本电脑操作 | 文件管理、软件安装、USB 设备使用 | -- |

### 了解即可 (课程中会教) | Nice to Know (Taught in Course)

| 知识点 Knowledge | 说明 Description | 课程覆盖 |
|-----------------|-----------------|---------|
| C 语言基础 | 变量、循环、函数、数组 | Day 1-4 逐步讲解 |
| 二进制与十六进制 | 理解数据的表示方式 | Day 3 ADC 部分 |
| 信号基础 | 什么是正弦波、频率、周期 | Day 1 |
| PCB 设计 | 基本的电路板设计 | Day 10 从头教起 |

### 不需要掌握 | Not Required

- 嵌入式编程经验 -- 课程从头教起
- 模拟电路设计经验 -- 课程从基础教起
- 3D 建模经验 -- Day 11 提供模板
- Linux 使用经验 -- 全部使用 Windows 工具

---

## 硬件准备 | Hardware Preparation

### 课程提供 (学校实验室) | Provided by School Lab

以下设备由学校实验室统一提供：

- 万用表 (用于调试)
- 烙铁与焊接工具 (Day 10-11 使用)
- USB 烧录器 ST-Link V2
- 信号发生器 (用于测试，Day 1-8 使用)
- 示波器 (用于对比测量，教学用)

### 个人购买清单 | Personal Purchase List

以下模块为个人套装，课程开始前请准备好：

| 物品 Item | 数量 | 参考价格 | 备注 Notes |
|-----------|------|---------|-----------|
| STM32F103C8T6 蓝pill板 | 1 | 8 元 | 淘宝搜索"蓝pill STM32" |
| ST7789 1.8寸 TFT 显示屏 | 1 | 25 元 | SPI 接口，160x128 |
| 面包板 830孔 | 1 | 8 元 | |
| 杜邦线套装 | 1 套 | 5 元 | 公对公+公对母 |
| Micro-USB 数据线 | 1 | 5 元 | 确保是数据线而非充电线 |
| LM358 运放 (DIP-8) | 2 | 2 元 | |
| 电阻套件 | 1 套 | 5 元 | |
| 电容套件 | 1 套 | 5 元 | |
| AD9833 DDS 模块 | 1 | 15 元 | Day 9 使用，可稍后购买 |

**总计约 80 元**

> 提示：建议在淘宝一次性搜索"STM32F103 学习套件"，很多店家会搭配面包板、杜邦线等一起出售，价格更优惠。
> Tip: Search "STM32F103 learning kit" on Taobao. Many sellers bundle breadboards, jumper wires, etc., at a better price.

---

## 软件环境准备 | Software Environment Setup

请在课程开始前完成以下软件安装。如遇到问题，可参考各软件官网的安装指南。

Please complete the following software installations before the course begins. If you encounter issues, refer to each software's official installation guide.

### 必装软件 | Required Software

#### 1. STM32CubeMX (配置工具)

- **下载地址：** https://www.st.com/en/development-tools/stm32cubemx.html
- **版本要求：** 最新版
- **安装步骤：**
  1. 注册 ST 官网账号（免费）
  2. 下载 STM32CubeMX 安装包
  3. 运行安装程序，按默认设置安装
  4. 安装完成后，打开软件，下载 STM32F1 系列固件包
- **预计时间：** 30 分钟
- **注意：** 需要 Java 运行环境，安装程序会自动提示安装

#### 2. Keil MDK 或 STM32CubeIDE (编译开发环境)

**方案 A：Keil MDK (推荐)**
- **下载地址：** https://www.keil.com/mdk5/
- **版本要求：** MDK-Lite (免费版，代码限制 32KB，足够本项目使用)
- **安装步骤：**
  1. 下载 MDK 安装包
  2. 安装时选择 STM32F1 系列器件支持包 (DFP)
  3. 安装完成后注册免费许可证
- **注意：** 免费版限制 32KB 代码，本项目代码量远小于此限制

**方案 B：STM32CubeIDE (免费，无限制)**
- **下载地址：** https://www.st.com/en/development-tools/stm32cubeide.html
- **优点：** 完全免费，无代码量限制，集成 CubeMX
- **缺点：** 软件较大 (~2GB)，对电脑配置要求较高

#### 3. ST-Link 驱动 (调试器驱动)

- **下载地址：** https://www.st.com/en/development-tools/stsw-link009.html
- **安装步骤：**
  1. 下载驱动包
  2. 解压后运行 dpinst_amd64.exe (64位系统) 或 dpinst_x86.exe (32位系统)
  3. 插入 ST-Link V2，系统应自动识别

#### 4. 串口调试助手

- **推荐工具：** SSCOM / 串口调试助手 / PuTTY
- **下载地址：** 搜索"SSCOM 串口调试助手"即可
- **用途：** 查看 STM32 输出的调试信息和波形数据

### 选装软件 | Optional Software

#### 5. Python 3.8+ (PC 端数据分析)

- **下载地址：** https://www.python.org/downloads/
- **安装步骤：**
  1. 下载 Python 3.10 或更新版本
  2. 安装时勾选 "Add Python to PATH"
  3. 打开命令行，运行 `pip install pyserial matplotlib numpy`
- **用途：** 接收串口波形数据并用 matplotlib 绘制图形

#### 6. 嘉立创 EDA (PCB 设计，Day 10 使用)

- **下载地址：** https://lceda.cn/
- **版本要求：** 嘉立创 EDA 专业版 (免费)
- **用途：** Day 10 的 PCB 设计课程
- **注意：** 可在线使用，也可下载客户端

---

## 硬件环境检查清单 | Hardware Environment Checklist

课程开始前一天，请逐项检查以下内容：

The day before the course begins, please check each item:

- [ ] STM32F103C8T6 蓝pill板能被电脑识别 (插上 USB，设备管理器有反应)
- [ ] ST-Link V2 驱动安装成功 (设备管理器显示 STLink)
- [ ] STM32CubeMX 能正常打开，已下载 F1 系列固件包
- [ ] Keil MDK / CubeIDE 能正常编译一个空工程
- [ ] 面包板和杜邦线到位
- [ ] 串口调试助手能正常运行
- [ ] 网络连接正常 (下载代码和查阅资料)

---

## 预习任务 | Pre-Study Tasks

如果课程开始前有 2~3 天时间，强烈建议完成以下预习：

If you have 2-3 days before the course starts, it is strongly recommended to complete the following:

### 任务 1：理解欧姆定律 (30 分钟)

在 B 站搜索"欧姆定律 入门"，观看一个基础教学视频。理解以下公式：
- V = I * R (电压 = 电流 x 电阻)
- P = V * I (功率 = 电压 x 电流)

### 任务 2：了解示波器是什么 (30 分钟)

观看 B 站视频：尤里卡学院《示波管原理演示视频》
- 链接：https://www.bilibili.com/video/BV174411h7bn/
- 理解示波器如何将电压变化显示为波形图

### 任务 3：搭建开发环境 (1 小时)

按照上面"软件环境准备"部分，安装 STM32CubeMX 和 Keil MDK/CubeIDE。

### 任务 4：了解 STM32 (30 分钟)

B 站搜索"STM32 入门 介绍"，观看一个介绍视频，了解：
- 什么是微控制器 (MCU)
- STM32 的基本外设 (GPIO, ADC, SPI, UART)
- 蓝pill板的基本引脚布局

### 任务 5：了解信号与波形 (30 分钟)

B 站搜索"正弦波 信号 教学"，理解：
- 正弦波的三要素：幅度、频率、相位
- 什么是频率 (Hz) 和周期 (s)
- 常见的波形：正弦波、方波、三角波

---

## 常见问题 | FAQ

**Q: 我是文科生 / 没有任何技术基础，可以参加吗？**
A: 可以！本课程专为零基础设计。只要你对电子制作有兴趣，愿意动手尝试，就能跟上进度。最重要的是保持好奇心。

**Q: 电脑配置有要求吗？**
A: 建议使用 Windows 10/11 系统，4GB 以上内存，20GB 可用磁盘空间。如果使用 STM32CubeIDE，建议 8GB 内存。

**Q: 需要买什么特别的工具吗？**
A: 除上述个人购买清单外，不需要额外工具。面包板实验无需焊接。烙铁等焊接工具由学校实验室提供。

**Q: 淘宝买的模块到了发现不对怎么办？**
A: 课程第一天会检查所有人的硬件，有问题的可以当天重新下单。建议尽早购买，留出到货时间。

**Q: 可以用 macOS 或 Linux 吗？**
A: STM32CubeMX 支持 macOS/Linux，但 Keil MDK 仅支持 Windows。如使用 macOS/Linux，请安装 STM32CubeIDE (跨平台)。本课程教程基于 Windows 编写。

---

## 推荐预习资源 | Recommended Pre-Study Resources

| 资源 Resource | 类型 Type | 链接 Link | 说明 Description |
|--------------|----------|----------|-----------------|
| 《示波器使用入门》 | B站视频 | https://www.bilibili.com/video/BV1t84y1A7dt/ | 零基础示波器使用教程 |
| 《示波管原理演示》 | B站视频 | https://www.bilibili.com/video/BV174411h7bn/ | 动画演示原理 |
| 《STM32入门》 | B站搜索 | 搜索"STM32 入门 教程" | 大量免费入门教程 |
| 《运放电路基础》 | B站搜索 | 搜索"运放电路 入门" | 运放原理教学 |
| STM32 参考手册 | PDF文档 | st.com 下载 RM0008 | 官方技术手册 |

---

*最后更新：2026-05-26*
*Last updated: 2026-05-26*
