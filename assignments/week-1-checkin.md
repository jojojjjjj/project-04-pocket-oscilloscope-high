# 第一周进度报告 | Week 1 Check-in Report

**学生姓名 | Student Name:** _______________
**日期 | Date:** _______________
**项目 | Project:** TinyAWG 口袋任意波形发生器 | Pocket Arbitrary Waveform Generator

---

## 一、Phase 1 学习总结 | Phase 1 Learning Summary

### 1.1 知识点回顾 | Knowledge Review

请用自己的话（不是复制粘贴！）解释以下概念：
Please explain the following concepts in your own words (no copy-paste!):

**什么是 ZYNQ？它的 PS 和 PL 分别负责什么？**
What is ZYNQ? What do its PS and PL each do?

（在此填写 / Write here）

**什么是 DDS（直接数字合成）？它是如何产生不同频率波形的？**
What is DDS (Direct Digital Synthesis)? How does it generate waveforms at different frequencies?

（在此填写 / Write here）

**什么是 DAC？AD9744 的 14 位分辨率和 210MSPS 速率意味着什么？**
What is a DAC? What do AD9744's 14-bit resolution and 210MSPS rate mean?

（在此填写 / Write here）

**解释 FPGA 中 BRAM 的作用，以及为什么 DDS 波形表需要存储在 BRAM 中。**
Explain the role of BRAM in FPGA, and why the DDS waveform table needs to be stored in BRAM.

（在此填写 / Write here）

### 1.2 最有收获的一件事 | Most Valuable Takeaway

请描述本周你学到的最有价值的一个知识点或技能：
Describe the most valuable concept or skill you learned this week:

（在此填写 / Write here）

---

## 二、Vivado 工程进展 | Vivado Project Progress

### 2.1 开发环境状态 | Development Environment Status

| 项目 Item | 状态 Status |
|-----------|------------|
| Vivado 安装完成 | [ ] 是 [ ] 否 |
| Vitis 安装完成 | [ ] 是 [ ] 否 |
| JTAG 下载器可用 | [ ] 是 [ ] 否 |
| ZYNQ 核心板到手 | [ ] 是 [ ] 否 |

### 2.2 Vivado Block Design 完成情况 | Block Design Completion

请勾选已完成的项目：
Check off the items you have completed:

- [ ] 创建 Vivado 工程，选择 ZYNQ7010 器件
- [ ] 添加 ZYNQ7 Processing System IP 核
- [ ] 配置 PS 时钟 (DDR3, MIO, 时钟频率)
- [ ] 添加 AXI GPIO IP 核 (用于 LED 控制)
- [ ] 添加 AXI BRAM Controller IP 核 (用于波形存储)
- [ ] 完成 Block Design 连线和地址分配
- [ ] 生成 HDL Wrapper
- [ ] 完成 Synthesis (综合)
- [ ] 完成 Implementation (实现)
- [ ] 生成 Bitstream (比特流)
- [ ] 导出 Hardware (.xsa) 到 Vitis

### 2.3 Bitstream 信息 | Bitstream Information

| 参数 Parameter | 值 Value |
|---------------|---------|
| FPGA 器件 | XC7Z010-CLG400 |
| BRAM 使用量 | ____ / 60 (____%) |
| LUT 使用量 | ____ / ____ (____%) |
| FF 使用量 | ____ / ____ (____%) |
| 最大时钟频率 | ____ MHz |
| Bitstream 大小 | ____ KB |

---

## 三、Verilog / FPGA 实验完成情况 | Verilog / FPGA Experiments

### 3.1 已完成的实验 | Completed Experiments

| 实验 Experiment | 完成状态 Status | 截图/说明 Notes |
|----------------|----------------|----------------|
| LED 闪烁 (Blink) | [ ] 完成 [ ] 部分 [ ] 未开始 | |
| PWM 呼吸灯 | [ ] 完成 [ ] 部分 [ ] 未开始 | |
| 按键输入检测 | [ ] 完成 [ ] 部分 [ ] 未开始 | |
| BRAM 读写测试 | [ ] 完成 [ ] 部分 [ ] 未开始 | |
| DDS 仿真 (行为级) | [ ] 完成 [ ] 部分 [ ] 未开始 | |
| DDS 下板测试 (DAC 输出) | [ ] 完成 [ ] 部分 [ ] 未开始 | |

### 3.2 DDS 模块设计状态 | DDS Module Design Status

| 模块 Module | 完成状态 Status | 说明 Notes |
|------------|----------------|-----------|
| 相位累加器 (Phase Accumulator) | [ ] 完成 [ ] 部分 [ ] 未开始 | |
| 波形查找表 (Waveform LUT) | [ ] 完成 [ ] 部分 [ ] 未开始 | |
| 频率控制字接口 (FCW Interface) | [ ] 完成 [ ] 部分 [ ] 未开始 | |
| AD9744 DAC 数据接口 | [ ] 完成 [ ] 部分 [ ] 未开始 | |

### 3.3 实验截图 | Experiment Screenshots

**截图 1：Vivado Block Design**
（在此粘贴截图 / Paste screenshot here）

**截图 2：DDS 行为仿真波形**
（在此粘贴截图 / Paste screenshot here）

**截图 3：LED 闪烁 / PWM 实验照片（如果有下板）**
（在此粘贴照片 / Paste photo here）

---

## 四、技术挑战与解决方案 | Technical Challenges & Solutions

### 4.1 主要技术问题 | Major Technical Issues

请描述本周遇到的最大的 1~2 个技术问题：
Describe the 1~2 biggest technical problems you encountered this week:

**问题 1：**
- 现象 (Symptom):
- 原因分析 (Root Cause):
- 解决方法 (Solution):

**问题 2：**
- 现象 (Symptom):
- 原因分析 (Root Cause):
- 解决方法 (Solution):

### 4.2 仍未解决的问题 | Unresolved Issues

如果有还未解决的问题，请描述：
If there are still unresolved issues, describe them here:

（在此填写 / Write here）

---

## 五、工作照片/截图 | Photos & Screenshots

请粘贴本周的工作照片或截图（至少 3 张）：
Paste at least 3 photos or screenshots of your work this week:

**照片 1：** 开发环境 / 工作台 (Development environment / Workbench)

（在此粘贴 / Paste here）

**照片 2：** Vivado 界面截图 (Vivado IDE screenshot)

（在此粘贴 / Paste here）

**照片 3：** 硬件实物 / 连接线 (Hardware / Wiring)

（在此粘贴 / Paste here）

**照片 4：** 其他 (Other)

（在此粘贴 / Paste here）

---

## 六、Phase 2 计划与目标 | Phase 2 Plan & Goals

### 6.1 Phase 2 目标 | Phase 2 Goals

你对 Phase 2（嵌入式软件与 GUI）的期望和计划：
Your expectations and plans for Phase 2 (Embedded Software & GUI):

（在此填写 / Write here）

### 6.2 预期风险 | Anticipated Risks

你认为 Phase 2 可能遇到的困难：
Difficulties you anticipate in Phase 2:

（在此填写 / Write here）

### 6.3 Phase 2 时间安排 | Phase 2 Schedule

| 天数 Day | 计划完成内容 Planned Tasks |
|----------|--------------------------|
| Day 5 | |
| Day 6 | |
| Day 7 | |
| Day 8 | |

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
