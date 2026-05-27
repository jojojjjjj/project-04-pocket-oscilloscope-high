# 组装步骤 | Assembly Steps

## 概述 | Overview

本文档提供口袋示波器的分步组装指南。请按照顺序完成每个步骤，不要跳步。

This document provides step-by-step assembly instructions for the Pocket Oscilloscope. Follow each step in order -- do not skip steps.

---

## 准备工作 | Preparation

### 工具与材料准备

- [ ] 所有 BOM 清单中的元件（参照 `hardware/BOM.md`）
- [ ] 面包板、杜邦线
- [ ] ST-Link V2 下载器
- [ ] USB 数据线
- [ ] 万用表（调试用）
- [ ] 信号发生器（测试用，可用手机 APP 替代）

### 软件环境准备

- [ ] STM32CubeMX 已安装
- [ ] Keil MDK / STM32CubeIDE 已安装
- [ ] ST-Link 驱动已安装
- [ ] 串口调试助手已准备

---

## 第一步：验证 STM32 最小系统板 | Step 1: Verify STM32 Board

**操作步骤：**

1. 将蓝pill板通过 USB 连接到电脑
2. 板载 PWR LED 应亮起（红色）
3. 在设备管理器中检查是否有新设备出现
4. 将 ST-Link V2 连接到蓝pill板：
   - SWDIO → SWDIO
   - SWCLK → SWCLK
   - GND → GND
   - 3.3V → 3.3V
5. 打开 STM32CubeMX，点击 "Connect"，应能识别到芯片

**验证标准：**
- [x] PWR LED 亮起
- [x] CubeMX 能识别 STM32F103C8Tx
- [x] ST-Link 连接正常

**预计时间：** 10 分钟

---

## 第二步：烧录测试固件 | Step 2: Flash Test Firmware

**操作步骤：**

1. 在 CubeMX 中创建新工程，选择 STM32F103C8Tx
2. 配置 PC13 为 GPIO_Output (LED)
3. 配置 USART1 (115200, 8N1)
4. 配置 ADC1 IN0 (PA0, 12-bit)
5. 配置 TIM3 (Prescaler=0, Period=71, 触发 ADC)
6. 配置 DMA (ADC1 → Circular)
7. 生成代码，编译下载
8. LED 应以 1Hz 频率闪烁

**验证标准：**
- [x] 编译成功
- [x] 烧录成功
- [x] LED 闪烁正常

**预计时间：** 20 分钟

---

## 第三步：搭建信号调理电路 | Step 3: Build Signal Conditioning Circuit

**操作步骤：**

1. **电源确认：** 用万用表确认面包板 5V 和 3.3V 电源轨正常
2. **安装 LM358：**
   - 将 LM358 DIP-8 IC 插入面包板
   - Pin 8 接 +5V，Pin 4 接 GND
   - 万用表测量 Pin 8 与 Pin 4 之间为 5V
3. **搭建电压跟随器：**
   - Pin 3 (IN1+) 接电位器中间引脚
   - Pin 2 (IN1-) 接 Pin 1 (OUT1)
   - 电位器两端接 3.3V 和 GND
   - 测量输出：旋转电位器，输出应跟随输入
4. **搭建偏置电路：**
   - 两个 10kΩ 电阻串联在 3.3V 和 GND 之间
   - 测量中点电压：应为 ~1.65V
   - 100nF 去耦电容并联在中点和 GND 之间
5. **连接输入到 PA0：**
   - 偏置输出 → 100nF 电容 → PA0
   - 万用表测量 PA0 电压：应为 ~1.65V

**验证标准：**
- [x] LM358 供电正常
- [x] 电压跟随器输出 = 输入
- [x] 偏置电压约 1.65V
- [x] PA0 电压约 1.65V

**预计时间：** 30 分钟

---

## 第四步：连接 TFT 显示屏 | Step 4: Connect TFT Display

**操作步骤：**

1. 断开电源
2. 按照 `hardware/wiring-guide.md` 连接 TFT 屏幕：
   - VCC → 3.3V
   - GND → GND
   - SCL → PA5
   - SDA → PA7
   - CS → PA4
   - DC → PA3
   - RST → PA2
   - BLK → PA1
3. 上电
4. 烧录 TFT 测试固件（含 ST7789 初始化代码）
5. TFT 背光应亮起

**验证标准：**
- [x] TFT 背光亮起
- [x] 屏幕能显示颜色（测试填充红色矩形）

**预计时间：** 20 分钟

---

## 第五步：集成波形显示 | Step 5: Integrate Waveform Display

**操作步骤：**

1. 将 ADC+DMA+TIM 采样代码与 TFT 显示代码合并
2. 烧录完整示波器固件
3. 信号发生器输出 1kHz, 1Vpp, 偏置 1.65V 的正弦波
4. 信号发生器输出 → 运放电路 → PA0
5. 观察 TFT 屏幕

**验证标准：**
- [x] TFT 屏幕显示波形
- [x] 波形稳定（触发功能正常）
- [x] 频率和幅度读数正确

**预计时间：** 30 分钟

---

## 第六步：连接信号发生器 (可选) | Step 6: Connect Signal Generator (Optional)

**操作步骤：**

1. 断开电源
2. 按照接线指南连接 AD9833 模块：
   - VCC → 3.3V
   - GND → GND
   - DAT → PB15
   - CLK → PB13
   - FSYNC → PB12
3. AD9833 输出连接到运放电路输入
4. 烧录包含信号发生器功能的固件
5. 用按键切换频率和波形
6. 示波器应能测量自己产生的信号

**验证标准：**
- [x] AD9833 输出信号（万用表/示波器验证）
- [x] 示波器能测量 AD9833 输出
- [x] 按键切换功能正常

**预计时间：** 30 分钟

---

## 第七步：添加控制按键 | Step 7: Add Control Buttons

**操作步骤：**

1. 安装 4 个轻触按键到面包板：
   - 按键1: PA0 ↔ GND（时基减小）
   - 按键2: PB0 ↔ GND（时基增大）
   - 按键3: PA8 ↔ GND（频率切换）
   - 按键4: PB1 ↔ GND（波形切换）
2. 配置 CubeMX 中对应引脚为 GPIO_Input, Pull-Up
3. 测试每个按键的功能

**验证标准：**
- [x] 4 个按键都能正确响应
- [x] 按键切换时 TFT 显示相应变化

**预计时间：** 15 分钟

---

## 第八步：外壳装配 (可选) | Step 8: Case Assembly (Optional)

**操作步骤：**

1. 检查 3D 打印外壳与硬件的配合
2. 将蓝pill板固定到底座（双面胶或铜柱）
3. 将 TFT 屏幕对准窗口安装
4. 将按键帽穿出按键孔
5. 引出 USB 线和输入接口
6. 安装顶盖

**验证标准：**
- [x] 所有开口对齐
- [x] 按键操作灵活
- [x] USB 线能正常插入

**预计时间：** 20 分钟

---

## 最终检查 | Final Check

| 检查项 | 状态 |
|--------|------|
| USB 供电正常，LED 指示灯亮 | [ ] |
| TFT 显示正常，无坏点 | [ ] |
| 波形显示稳定，触发功能正常 | [ ] |
| 频率测量正确 (误差 < 5%) | [ ] |
| 时基切换正常 | [ ] |
| 信号发生器输出正常 | [ ] |
| 按键操作灵活 | [ ] |
| 外壳完整 | [ ] |

---

*最后更新：2026-05-26*
*Last updated: 2026-05-26*
