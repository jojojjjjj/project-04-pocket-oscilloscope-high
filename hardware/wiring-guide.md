# 接线指南 | Wiring Guide

## 概述 | Overview

本文档描述口袋示波器所有模块的接线方式。请严格按照接线图操作，接线错误可能导致元件损坏。

This document describes the wiring for all modules of the Pocket Oscilloscope. Follow the wiring diagrams strictly -- incorrect wiring may damage components.

---

## 接线总览 | Wiring Overview

```
                    ┌──────────────────────────────────────┐
                    │         STM32F103C8T6 蓝pill板         │
                    │                                      │
   ┌────────────────│ PA0  (ADC_IN0)  ← 信号调理电路输出    │
   │                │ PA1  (GPIO)     → TFT 背光 (BLK)     │
   │   信号输入      │ PA2  (GPIO)     → TFT 复位 (RST)     │
   │   ↓            │ PA3  (GPIO)     → TFT DC             │
   │  信号调理电路    │ PA4  (GPIO)     → TFT 片选 (CS)      │
   │   ↓            │ PA5  (SPI1_SCK) → TFT 时钟 (SCL)     │
   └───────────────→│ PA7  (SPI1_MOSI)→ TFT 数据 (SDA)     │
                    │                                      │
                    │ PB12 (GPIO)     → AD9833 片选 (FSYNC) │
                    │ PB13 (SPI2_SCK) → AD9833 时钟 (CLK)  │
                    │ PB15 (SPI2_MOSI)→ AD9833 数据 (DAT)  │
                    │                                      │
                    │ PC13 (GPIO)     → 板载 LED            │
                    │                                      │
                    │ PA0  (GPIO_IN)  → 按键1 (时基-)       │
                    │ PB0  (GPIO_IN)  → 按键2 (时基+)       │
                    │ PA8  (GPIO_IN)  → 按键3 (频率切换)    │
                    │ PB1  (GPIO_IN)  → 按键4 (波形切换)    │
                    │                                      │
                    │ PA9  (USART1_TX)→ 串口调试 TX         │
                    │ PA10 (USART1_RX)→ 串口调试 RX         │
                    │                                      │
                    │ SWDIO          ← ST-Link SWDIO        │
                    │ SWCLK          ← ST-Link SWCLK        │
                    │ GND            ← ST-Link GND           │
                    │ 3.3V           ← ST-Link 3.3V          │
                    │ USB            ← USB 5V 供电           │
                    └──────────────────────────────────────┘
```

---

## 详细接线 | Detailed Wiring

### 1. ST-Link 调试接口 | ST-Link Debug Interface

| ST-Link V2 | STM32 蓝pill板 | 说明 |
|-----------|---------------|------|
| SWDIO | SWDIO | 数据线 |
| SWCLK | SWCLK | 时钟线 |
| GND | GND | 地线 |
| 3.3V | 3.3V | 电源 (可选，也可用 USB 供电) |

> 接线图（文字描述）：
> ST-Link 四根排针依次接蓝pill板右侧的 SWD 排针 (SWDIO, SWCLK, GND, 3.3V)

### 2. TFT 显示屏接线 | TFT Display Wiring

| TFT ST7789 引脚 | STM32 引脚 | 面包板连接说明 |
|----------------|-----------|-------------|
| VCC | 3.3V | 接 3.3V 电源轨 |
| GND | GND | 接 GND 电源轨 |
| SCL | PA5 | SPI 时钟 |
| SDA | PA7 | SPI 数据 (MOSI) |
| CS | PA4 | 片选 (低电平有效) |
| DC | PA3 | 数据/命令选择 |
| RST | PA2 | 复位 (低电平有效) |
| BLK | PA1 | 背光 (高电平点亮) |

> 注意：不同厂家的 TFT 模块引脚顺序可能不同，请以模块背面丝印为准。
> Note: Pin order may vary by manufacturer. Refer to the silkscreen on the back of your module.

### 3. 信号调理电路接线 | Signal Conditioning Circuit Wiring

#### 3.1 电压跟随器 (LM358 运放 1)

```
面包板布局：

LM358 (DIP-8, 凹槽朝左)
     ┌──────┐
Pin1 │1    8│ VCC (接 +5V 或 3.3V)
     │2    7│ (运放2输出，本电路不用)
     │3    6│ (运放2反相输入，不用)
     │4    5│ (运放2同相输入，不用)
     └──────┘
      GND

连接方式：
  Pin 3 (IN1+) ← 输入信号 (经 100kΩ 电阻)
  Pin 2 (IN1-) ─┐
  Pin 1 (OUT1) ─┤── 输出到偏置电路
  Pin 8 (VCC)   ← +5V (或 3.3V)
  Pin 4 (GND)   ← GND
```

#### 3.2 偏置电路

```
分压偏置：
  +3.3V ── R1 (10kΩ) ──┬── R2 (10kΩ) ── GND
                        │
                        ├── Vbias = 1.65V
                        │
                     C1 (10uF, 电解电容)
                        │
                        ├── 信号输入 (来自运放输出)
                        │
                     R3 (1kΩ)
                        │
                        └── PA0 (STM32 ADC 输入)

去耦电容：
  PA0 和 GND 之间并联 100nF 瓷片电容
```

### 4. AD9833 信号发生器接线 | AD9833 Signal Generator Wiring

| AD9833 模块引脚 | STM32 引脚 | 说明 |
|----------------|-----------|------|
| VCC | 3.3V 或 5V | 电源 |
| GND | GND | 地 |
| DAT | PB15 (SPI2_MOSI) | 串行数据 |
| CLK | PB13 (SPI2_SCK) | SPI 时钟 |
| FSYNC | PB12 (GPIO) | 片选 (软件控制) |
| OUT | 信号输出 | 经运放放大后接示波器输入 |

> AD9833 输出幅度约 0.6Vpp，可选经 LM358 放大后再输入示波器。

### 5. 按键接线 | Button Wiring

| 按键功能 | STM32 引脚 | 接线说明 |
|---------|-----------|---------|
| 时基减小 | PA0 | 按键一端接 PA0，另一端接 GND (内部上拉) |
| 时基增大 | PB0 | 按键一端接 PB0，另一端接 GND (内部上拉) |
| 信号频率切换 | PA8 | 按键一端接 PA8，另一端接 GND (内部上拉) |
| 波形类型切换 | PB1 | 按键一端接 PB1，另一端接 GND (内部上拉) |

```
按键接线示意：

STM32 PA0 ───┐
             │
           ┌─┴─┐
           │   │  轻触按键 (6x6mm)
           │ ● │
           └─┬─┘
             │
           GND

(使用 STM32 内部上拉电阻，无需外部电阻)
```

### 6. 串口调试接线 | Serial Debug Wiring

| 串口模块 | STM32 引脚 | 说明 |
|---------|-----------|------|
| USB-TTL TX | PA10 (USART1_RX) | 数据接收 |
| USB-TTL RX | PA9 (USART1_TX) | 数据发送 |
| USB-TTL GND | GND | 地线 |
| USB-TTL VCC | 不接 | 用 USB 独立供电 |

> 如果使用 ST-Link 的虚拟串口功能，可能无需额外接线。
> 波特率：115200, 8位数据, 无校验, 1位停止 (8N1)

---

## 电源接线 | Power Wiring

### 电源方案 (推荐使用 USB 供电)

```
USB 5V ──→ STM32 蓝pill板 (板载 3.3V 稳压)
          │
          ├──→ 5V 电源轨 (给 LM358 供电)
          │
          └──→ AMS1117-3.3V ──→ 3.3V 电源轨 (给 TFT、AD9833 供电)
```

> 蓝pill板自带 AMS1117-3.3V 稳压器，3.3V 引脚可直接输出 3.3V 给其他模块。
> 注意：3.3V 引脚最大输出约 300mA，足够驱动 TFT 和 AD9833。

---

## 面包板布局建议 | Breadboard Layout Suggestion

```
面包板俯视图 (830孔)：

     +  -                            +  -
     |  |                            |  |
 ┌───┴──┴────────────────────────────┴──┴───┐
 │  3.3V  GND                              │
 │                                          │
 │  [STM32F103 蓝pill板]                      │
 │  (放在中间，跨越中间凹槽)                   │
 │                                          │
 │                                          │
 │  [LM358 #1]  [10kΩ]  [10kΩ]  [10uF]    │
 │   电压跟随器   R1      R2      C1       │
 │              偏置电路                     │
 │                                          │
 │  [按键1] [按键2] [按键3] [按键4]          │
 │  时基-   时基+   频率    波形             │
 │                                          │
 │  [AD9833 模块]                            │
 │  (放在面包板一侧)                          │
 │                                          │
 │  [TFT 显示屏模块]                         │
 │  (放在面包板顶部，朝向操作者)              │
 │                                          │
 └──────────────────────────────────────────┘
     +  -                            +  -
     |  |                            |  |
   5V电源轨                          5V电源轨
```

---

## 接线检查清单 | Wiring Checklist

接线完成后，逐项检查：

- [ ] ST-Link 四根线正确连接 (SWDIO, SWCLK, GND, 3.3V)
- [ ] TFT 8 根线全部正确连接
- [ ] LM358 电源 (Pin8=VCC, Pin4=GND) 正确
- [ ] 信号调理电路输出连接到 PA0
- [ ] 偏置电压 (1.65V) 用万用表验证
- [ ] AD9833 SPI 三根线正确 (DAT→PB15, CLK→PB13, FSYNC→PB12)
- [ ] 按键连接正确 (到 GND)
- [ ] USB 数据线已连接 (供电)
- [ ] 无短路 (万用表检查 5V 与 GND 之间无短路)

---

*最后更新：2026-05-26*
*Last updated: 2026-05-26*
