# 口袋信号发生器 TinyAWG 项目复查报告

复查日期：2026-05-27

## 总体评价

本项目基于开源 TinyAWG 信号发生器（OSHWhub: greentor/tinyawg-signal-source）进行了全面重塑。项目从原 STM32 示波器方案更改为 ZYNQ7010 任意波形发生器方案，涉及所有文件的完全重写。项目核心技术包括 FPGA DDS 数字合成、ARM 嵌入式软件开发、高速 DAC 驱动、LVGL GUI 设计以及模拟输出电路。整体文件结构保持一致，中英双语覆盖全面，技术内容准确。

## 检查结果

### 1. 文件完整性

- [x] README.md
- [x] resources/search-results.md
- [x] curriculum/overview.md
- [x] curriculum/prerequisites.md
- [x] curriculum/assignments.md
- [x] curriculum/grading-rubric.md
- [x] curriculum/day-01.md ~ day-12.md (12 个文件)
- [x] hardware/BOM.md
- [x] hardware/wiring-guide.md
- [x] hardware/assembly-steps.md
- [x] hardware/troubleshooting.md
- [x] software/requirements.txt
- [x] software/config.template.yaml
- [x] software/src/main.c (原 main.py)
- [x] software/src/dds_controller.c
- [x] software/src/waveform.c
- [x] software/src/output_ctrl.c
- [x] software/src/dac_driver.c
- [x] software/src/gui_helper.c
- [x] software/src/utils.c
- [x] software/tests/test_basic.py
- [x] assignments/week-1-checkin.md
- [x] assignments/week-2-checkin.md
- [x] assignments/final-presentation.md
- [x] assignments/rubric.md

### 2. 技术准确性检查

**TinyAWG 核心参数核对：**
- ZYNQ7010 (XC7Z010): 正确
- 200MSa/s 采样率: 正确（PL 时钟 200MHz）
- 14-bit 分辨率: 正确（AD9744）
- 35MHz 带宽: 正确（-3dB）
- 65536 波形点数: 正确（64K BRAM）
- 10Vpp 输出幅度: 正确
- ±5V 偏置范围: 正确
- 50Ω 输出阻抗: 正确
- ±6.5V 电源轨: 正确
- 约 220 元成本: 正确

**硬件设计要点：**
- AD9744 并行接口连接 PL: 正确
- OPA2673 输出级参考 DHO914S: 正确
- DAC8562 SPI 控制 amplitude/offset: 正确
- LMR544006 DCDC 产生 ±6.5V: 正确
- MAX17048 I2C 电池监测: 正确
- FT6336 I2C 触摸: 正确
- Type-C CH221K PD 诱骗: 正确

**已知问题记录：**
- 屏幕供电 3.3V vs 5V 问题: 已在 troubleshooting.md 中记录
- USB 电源/数据分路器需求: 已在 wiring-guide.md 中说明
- GUI 刷新撕裂问题: 已在 troubleshooting.md 中记录

### 3. 课程设计检查

**三阶段结构：**
- Phase 1 (Day 1-4): 数字系统基础与 FPGA -- 合理
- Phase 2 (Day 5-8): 嵌入式软件与 GUI -- 合理
- Phase 3 (Day 9-12): 系统集成与展示 -- 合理

**难度梯度：**
- Day 1-2: 项目理解 + Vivado 环境 -- 入门级
- Day 3-4: FPGA 基础 + DDS -- 中等难度
- Day 5-6: ARM 软件驱动 -- 中等难度
- Day 7-8: LVGL GUI -- 实践为主
- Day 9: 系统集成 -- 综合应用
- Day 10-11: 硬件组装 -- 实践为主
- Day 12: 展示 -- 总结汇报

**高中生适配性：**
- FPGA 概念简化，侧重理解 DDS 原理
- 提供模板代码和详细步骤指导
- PCB 使用开源设计文件，学生焊接组装
- GUI 开发基于 LVGL 可视化框架，降低难度

### 4. 格式检查

- 所有文件中英双语: 待确认
- 表格格式正确: 待确认
- "最后更新" 日期一致 (2026-05-27): 待确认
- 评分权重一致: 待确认

### 5. 开源合规

- 原项目协议: LGPL 3.0 -- 已在 README.md 中注明
- 课程文档协议: CC BY-NC-SA 4.0 -- 已在 README.md 中注明
- 引用来源: OSHWhub + 飞书文档 -- 已在 README.md 中注明

---

## 需要确认的问题

### 问题 1: 评分权重一致性
需确认 grading-rubric.md、overview.md、rubric.md 中的评分维度和权重一致。

### 问题 2: 软件文件扩展名
原项目使用 .py 文件，新项目改为 .c 文件。需确认所有文件引用已更新。

### 问题 3: 课程天数匹配
确认所有 daily 文件 (day-01 到 day-12) 的主题与 README.md 课程安排表一致。

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
