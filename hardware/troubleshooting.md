# 硬件故障排除 | Hardware Troubleshooting

## 概述 | Overview

本文档列出 TinyAWG 口袋信号发生器项目中常见的硬件问题和解决方案。遇到问题时，请按照本文档的步骤逐一排查。

This document lists common hardware issues and solutions for the TinyAWG pocket signal generator project. When you encounter problems, follow the troubleshooting steps systematically.

---

## 电源问题 | Power Issues

### 问题：上电后无反应 | No Response After Power-On

**症状：** 屏幕、LED 均无显示，核心板无响应。

**排查步骤：**

1. 检查电源输入
   - 外部电源是否为 9-12V？（5V 只能充电，不能开机）
   - Type-C 线是否支持 PD？（如果需要 12V PD 诱骗）
   - 电池是否有电？（电压 < 3.0V 需先充电）

2. 测量关键电压点

| 测试点 | 正常值 | 说明 |
|--------|-------|------|
| Type-C VBUS | 5V 或 12V | 电源输入 |
| 升压输出 | 12V | 检查升压芯片 |
| +6.5V 轨 | +6.5V | LMR544006 输出 |
| -6.5V 轨 | -6.5V | LMR544006 输出 |
| +3.3V | +3.3V | LDO 输出 |
| +1.8V | +1.8V | LDO 输出 |

3. 常见原因
   - 电源线接触不良 → 更换 USB 线
   - 电池电压过低 → 先充电
   - DCDC 芯片焊接问题 → 重新焊接
   - 负载短路 → 断开各模块逐一排查

### 问题：电池无法充电 | Battery Won't Charge

**排查步骤：**

1. 确认外部电源已连接（5V 或 9-12V）
2. 测量充电 IC 输出是否有充电电流
3. 检查电池连接器是否正确接入
4. 检查 MOSFET 路径管理是否正常
5. 确认电池保护板未触发保护（过放保护需激活）

### 问题：电源纹波大 | Large Power Ripple

**排查步骤：**

1. 用示波器 AC 耦合测量电源轨纹波
2. 检查 DCDC 输出滤波电容是否正确安装
3. 检查电感值是否匹配
4. 增加 LC 滤波器改善纹波
5. 确认模拟地和数字地是否正确分离

---

## FPGA / 比特流问题 | FPGA / Bitstream Issues

### 问题：FPGA 无法配置 | FPGA Won't Configure

**症状：** JTAG 无法连接到 PL，或比特流下载失败。

**排查步骤：**

1. 检查 JTAG 连接
   - 确认 TCK, TMS, TDI, TDO, GND 全部正确连接
   - 检查 JTAG 电缆驱动是否安装
   - 尝试降低 JTAG 时钟频率

2. 检查 ZYNQ 核心板
   - 确认核心板已正确插入底板连接器
   - 检查 3.3V 和 1.8V 供电正常
   - 确认核心板上配置 Flash 型号正确

3. Vivado 问题
   - 确认选择的目标芯片正确 (xc7z010clg400-1)
   - 检查比特流文件是否完整
   - 尝试使用 XSCT 命令行下载

### 问题：PL 逻辑不工作 | PL Logic Not Working

**排查步骤：**

1. 确认比特流已成功下载（Vivado 显示 "Device programmed successfully"）
2. 检查 PL 时钟是否正常（200MHz 差分时钟）
3. 检查 XDC 约束文件引脚分配是否正确
4. 用 Vivado 逻辑分析仪 (ILA) 调试内部信号

---

## DAC 输出问题 | DAC Output Issues

### 问题：无信号输出 | No Signal Output

**排查步骤：**

1. 确认 FPGA DDS 模块正常工作
   - 用 Vivado ILA 检查 DDS 输出数据
   - 确认相位累加器在运行

2. 检查 AD9744 DAC
   - 确认 DAC 时钟 (200MHz) 正常
   - 检查 DAC 电源 (+6.5V, 3.3V)
   - 确认 SLEEP 引脚为高电平（正常工作模式）

3. 检查输出放大器
   - 确认 OPA2673 电源 (±6.5V) 正常
   - 检查反馈电阻焊接
   - 测量运放输出端有无电压

4. 检查继电器
   - 确认继电器控制信号正常
   - 听继电器是否有切换声音
   - 用万用表测量继电器触点导通

### 问题：输出幅度不正确 | Incorrect Output Amplitude

**排查步骤：**

1. 检查 DAC8562 输出值
   - 用万用表测量 DAC8562 输出电压
   - 确认 SPI 通信正常（示波器抓取 SPI 波形）

2. 检查 OPA2673 增益
   - 确认反馈电阻值正确
   - 检查 DAC8562 CH-A (幅度控制) 电压

3. 校准步骤
   - 设置已知 DAC 值
   - 用示波器测量实际输出
   - 建立校准表

### 问题：输出波形失真 | Distorted Output Waveform

**排查步骤：**

1. 降低输出频率测试（如 1kHz 正弦波）
2. 检查电源纹波（可能耦合到输出）
3. 检查运放供电去耦是否充分
4. 确认 DAC 重建滤波器参数正确
5. 检查 PCB 布局（模拟/数字地是否分离）

---

## 显示问题 | Display Issues

### 问题：屏幕无显示 | Blank Screen

**排查步骤：**

1. 确认 LCD 供电
   - 测量 LCD VCC 引脚电压（应为 3.3V，可能需要 5V -- 见已知问题）
   - 检查背光控制信号

2. 检查 LCD 接口
   - 确认 FPC 排线连接牢固
   - 检查 RGB 数据线、时钟、同步信号
   - 确认 LCD 驱动初始化代码正确

3. 检查 LVGL 配置
   - 确认 lv_conf.h 中的分辨率和颜色深度设置
   - 检查显示端口 (lv_port_disp.c) 配置

### 问题：LVGL GUI 刷新撕裂/错位 | GUI Refresh Tearing

**已知问题：** 这是 TinyAWG 的已知 bug。

**原因分析：**
- 根据飞书制作文档 (2026/5/15)，很可能是屏幕供电问题
- LCD 需要 5V 供电，但设计提供的是 3.3V
- 使用电池供电时更明显，插电使用时较少出现

**临时解决方案：**
- 切换菜单强制刷新
- 使用外部电源供电
- 长期方案：修改 PCB 为屏幕提供 5V 供电

### 问题：触摸不工作 | Touch Not Working

**排查步骤：**

1. 确认 FT6336 I2C 通信
   - 用逻辑分析仪或示波器检查 I2C 总线
   - 确认 I2C 地址 (0x38)
   - 扫描 I2C 总线上的设备

2. 检查触摸中断
   - 确认 INT 引脚连接正常
   - 触摸屏幕时观察 INT 信号变化

3. 校准触摸
   - 运行触摸校准程序
   - 确认触摸坐标映射正确

---

## 通信问题 | Communication Issues

### 问题：USB 串口无法连接 | USB Serial Connection Failed

**已知限制：** USB Type-C 同时用于供电和串口通信。

**解决方案：**
- 使用**电源数据分路器** (Power-Data Splitter)
- 将 Type-C 的 VBUS 和 D+/D- 分开
- 用独立电源供电，USB 只用于数据通信

### 问题：上位机无法连接设备 | Host Software Can't Connect

**排查步骤：**

1. 确认串口驱动已安装
2. 检查波特率设置 (通常 115200 或 921600)
3. 确认 ARM 固件中的串口初始化正确
4. 用串口终端手动发送命令测试

---

## 电池问题 | Battery Issues

### 问题：电量显示不准确 | Inaccurate Battery Level

**排查步骤：**

1. 确认 MAX17048 I2C 通信正常
2. 确认 MAX17048 已完成初次学习周期
3. 检查 MAX17048 与电池之间的连接
4. 尝试重新初始化 MAX17048

### 问题：电池续航时间短 | Short Battery Life

**可能原因：**
- LCD 背光功耗过大
- FPGA 和 ARM 功耗较高
- 电池容量不足

**建议：**
- 降低 LCD 背光亮度
- 在不使用时关闭输出
- 考虑使用更大容量电池

---

## 已知问题 | Known Issues

| 问题 | 严重程度 | 状态 | 解决方案 |
|------|---------|------|---------|
| LCD 需要 5V 但设计提供 3.3V | 高 | 待修复 | 修改 PCB 设计，为 LCD 提供 5V 供电 |
| GUI 刷新撕裂（电池供电时） | 中 | 已知 | 使用外部电源，或切换菜单强制刷新 |
| USB 串口需要电源数据分路器 | 中 | 设计限制 | 使用分路器分离电源和数据 |
| 无输出保护电路 | 中 | 设计限制 | 使用时注意避免短路 |
| LVGL 点击控件偶尔触发错误 | 低 | 已修复 | 使用 LV_EVENT_CLICKED 替代 LV_EVENT_RELEASED |

---

## 调试技巧 | Debug Tips

### 1. 分步验证法

遇到问题时，按以下顺序验证：
1. 电源 → 2. 时钟 → 3. 复位 → 4. JTAG → 5. 外设

### 2. 最小系统法

断开所有外设，只保留核心板和电源。确认核心板能正常工作后，逐一连接外设。

### 3. 信号追踪法

用示波器从信号源到输出端追踪信号，找到信号丢失或异常的位置。

### 4. 对比法

对比正常工作的参考设备（如果有），找出差异。

---

## 获取帮助 | Getting Help

| 渠道 | 说明 |
|------|------|
| TinyAWG 飞书文档 | https://ai.feishu.cn/docx/BGycd4URIot8Aoxs3vLcBdq2nFc |
| OSHWhub 项目页 | https://oshwhub.com/greentor/tinyawg-signal-source |
| B站演示视频评论区 | https://www.bilibili.com/video/BV1a42QBUECC/ |
| Xilinx 官方论坛 | https://support.xilinx.com/ |
| LVGL 官方论坛 | https://forum.lvgl.io/ |

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
