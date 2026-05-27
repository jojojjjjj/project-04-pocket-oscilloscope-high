# 硬件故障排除 | Hardware Troubleshooting

## 概述 | Overview

本文档列出口袋示波器项目中常见的硬件问题和解决方案。遇到问题时，先不要慌张，按照本文档的步骤逐一排查。

This document lists common hardware issues and solutions for the Pocket Oscilloscope project. When encountering a problem, don't panic -- follow the steps in this document to troubleshoot systematically.

---

## 通用排查步骤 | General Troubleshooting Steps

遇到任何硬件问题时，按以下顺序排查：

1. **检查电源：** 万用表测量各电源点电压是否正常
2. **检查接线：** 逐根检查杜邦线是否松动或错位
3. **检查元件方向：** IC、电解电容、LED 是否方向正确
4. **检查短路：** 万用表测量相邻引脚是否短路
5. **最小系统法：** 去掉所有外围电路，只保留最小系统，逐步添加

---

## 问题列表 | Problem List

### 1. STM32 蓝pill板无法上电 | STM32 Board Won't Power On

| 现象 | 可能原因 | 解决方案 |
|------|---------|---------|
| PWR LED 不亮 | USB 线损坏 | 更换 USB 数据线（确认是数据线而非充电线） |
| PWR LED 不亮 | USB 口损坏 | 尝试其他 USB 口或电脑 |
| PWR LED 不亮 | 板子损坏 | 检查板子上是否有明显烧毁痕迹 |
| PWR LED 闪烁 | 电源不稳 | 更换 USB 口，检查是否有短路 |
| 板子发烫 | 短路 | 立即断电！检查 5V/GND 之间是否短路 |

---

### 2. ST-Link 无法连接 | ST-Link Connection Failed

| 现象 | 可能原因 | 解决方案 |
|------|---------|---------|
| "No ST-Link detected" | USB 线问题 | 换一根 USB 线，确认是数据线 |
| "No ST-Link detected" | 驱动未安装 | 安装 ST-Link 驱动 (st.com 下载) |
| "No target found" | 接线错误 | 检查 SWDIO/SWCLK/GND 接线 |
| "No target found" | SWD 引脚被占用 | 确认代码中没有禁用 SWD 引脚 |
| "No target found" | 芯片锁死 | 使用 ST-Link Utility 进行 "Full Chip Erase" |
| 连接后立即断开 | 供电不足 | 同时连接 USB 供电和 ST-Link |

---

### 3. ADC 采样值异常 | ADC Sampling Issues

| 现象 | 可能原因 | 解决方案 |
|------|---------|---------|
| ADC 值始终为 0 | PA0 未接信号 | 检查 PA0 连接 |
| ADC 值始终为 4095 | PA0 接到 3.3V | 检查输入电路 |
| ADC 值剧烈跳动 | 电源噪声 | 在 PA0 和 GND 之间加 100nF 去耦电容 |
| ADC 值剧烈跳动 | 输入信号不稳定 | 用示波器检查输入信号质量 |
| ADC 值只有 0 和 4095 | 输入信号幅度太大 | 使用衰减电路降低信号幅度 |
| ADC 值偏移 | 偏置电路问题 | 测量偏置电压是否为 1.65V |
| DMA 不工作 | 配置错误 | 检查 CubeMX 中 DMA 配置，确认 Circular 模式 |
| DMA 只采集一次 | 未设为 Circular 模式 | CubeMX DMA Settings → Mode: Circular |

---

### 4. TFT 屏幕不显示 | TFT Display Not Working

| 现象 | 可能原因 | 解决方案 |
|------|---------|---------|
| 屏幕全黑 | 背光未开 | BLK 引脚接高电平 (PA1 → TFT BLK) |
| 屏幕全黑 | 电源未接 | VCC 接 3.3V，GND 接地 |
| 屏幕全白 | SPI 接线错误 | 检查 SCL→PA5, SDA→PA7 |
| 屏幕全白 | CS 未接 | CS 引脚必须接 PA4 (低电平有效) |
| 屏幕全白 | 初始化失败 | 检查 ST7789 初始化命令序列 |
| 颜色显示异常 | MADCTL 设置错误 | 尝试修改 MADCTL 寄存器值 |
| 画面撕裂 | 刷新速度不够 | 优化 SPI 速度，使用 DMA SPI |
| 部分区域有坏点 | 屏幕损坏 | 联系卖家更换 |
| 显示位置偏移 | 行列地址偏移 | 检查 CASET/RASET 设置 |

---

### 5. 波形显示问题 | Waveform Display Issues

| 现象 | 可能原因 | 解决方案 |
|------|---------|---------|
| 没有波形 | 信号未输入 | 检查信号源到 PA0 的完整链路 |
| 波形一直在跑 | 触发未生效 | 检查触发电平是否在信号范围内 |
| 波形反了 | Y 轴方向错误 | 修改 ADC 值到屏幕坐标的映射公式 |
| 波形幅度太小 | Y 轴缩放太小 | 增大 Y 轴缩放因子 |
| 波形削顶 | 输入超出 ADC 范围 | 调整运放增益或衰减网络 |
| 波形不连续 | 采样率太低 | 提高定时器触发频率 |
| 波形有锯齿 | 采样点不足 | 增加采样深度或降低信号频率 |

---

### 6. 频率测量问题 | Frequency Measurement Issues

| 现象 | 可能原因 | 解决方案 |
|------|---------|---------|
| 频率显示 0 | 信号未输入或幅度太小 | 检查信号输入 |
| 频率跳动很大 | 信号噪声大 | 增加滤波，或使用均值法 |
| 频率偏差大 | 采样率不准 | 检查定时器配置 |
| 高频测量不准 | 采样率不足 | 降低被测信号频率或提高采样率 |

---

### 7. AD9833 信号发生器问题 | Signal Generator Issues

| 现象 | 可能原因 | 解决方案 |
|------|---------|---------|
| 无输出 | SPI 接线错误 | 检查 DAT→PB15, CLK→PB13, FSYNC→PB12 |
| 无输出 | 电源未接 | VCC 接 3.3V 或 5V |
| 频率不对 | 参考时钟不对 | 确认晶振为 25MHz |
| 波形失真 | 输出负载过重 | 输出接高阻负载 (>10kΩ) |
| 波形有毛刺 | 电源噪声 | 在 VCC/GND 之间加 100nF 电容 |

---

### 8. 面包板连接问题 | Breadboard Connection Issues

| 现象 | 可能原因 | 解决方案 |
|------|---------|---------|
| 间歇性故障 | 杜邦线接触不良 | 更换杜邦线或重新插紧 |
| 间歇性故障 | 面包板老化 | 使用新的面包板 |
| 信号噪声大 | 面包板寄生电容 | 缩短连线，信号线远离电源线 |
| 不知哪根线坏了 | 线太多看不清 | 用万用表逐根检查通断 |
| 相邻孔短路 | 元件引脚太近 | 检查元件引脚是否互相碰触 |

---

## 快速诊断流程图 | Quick Diagnosis Flowchart

```
问题发生
   │
   ├── 上电后无反应？
   │   ├── 检查 USB 线和电源
   │   └── 检查是否有短路
   │
   ├── 能上电但无法烧录？
   │   ├── 检查 ST-Link 接线
   │   ├── 检查 ST-Link 驱动
   │   └── 尝试 Full Chip Erase
   │
   ├── 烧录成功但功能异常？
   │   ├── 检查 ADC 输入是否有信号
   │   ├── 检查 TFT 接线
   │   └── 检查各模块供电
   │
   └── 功能基本正常但波形不好？
       ├── 调整触发参数
       ├── 调整时基和量程
       └── 检查信号调理电路
```

---

## 调试工具推荐 | Recommended Debug Tools

| 工具 | 用途 | 价格 |
|------|------|------|
| 万用表 | 测量电压、电流、电阻、通断 | 30~50 元 |
| ST-Link V2 | 程序烧录和调试 | 10 元 |
| 逻辑分析仪 (可选) | 分析 SPI/UART 波形 | 20~50 元 |
| USB 电流表 (可选) | 测量功耗 | 15 元 |
| 信号发生器 (实验室) | 提供标准测试信号 | 实验室提供 |

---

## 何时求助 | When to Ask for Help

如果按照本文档排查后仍无法解决问题：

1. **记录问题现象：** 具体描述发生了什么，你期望发生什么
2. **记录排查步骤：** 你已经尝试了哪些解决方案
3. **拍照/录屏：** 拍下接线图、错误画面、万用表读数
4. **求助导师或同学：** 带着上述信息一起讨论

> 记住：调试是工程中最常见的工作。遇到问题不代表你做错了，而是你在学习的过程中。
> Remember: Debugging is the most common task in engineering. Encountering problems doesn't mean you did something wrong -- it means you're learning.

---

*最后更新：2026-05-26*
*Last updated: 2026-05-26*
