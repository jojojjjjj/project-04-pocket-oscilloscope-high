# 第二周进度报告 | Week 2 Check-in Report

**学生姓名 | Student Name:** _______________
**日期 | Date:** _______________
**项目 | Project:** TinyAWG 口袋任意波形发生器 | Pocket Arbitrary Waveform Generator

---

## 一、ARM 软件开发总结 | ARM Software Development Summary

### 1.1 驱动开发状态 | Driver Development Status

请勾选已完成的软件模块：
Check off the software modules you have completed:

| 模块 Module | 完成状态 Status | 说明 Notes |
|------------|----------------|-----------|
| Vitis 工程创建 + CMake 配置 | [ ] 完成 [ ] 部分 [ ] 未开始 | |
| GPIO 驱动 (LED / 继电器控制) | [ ] 完成 [ ] 部分 [ ] 未开始 | |
| SPI 驱动 (AD9744 DAC 数据) | [ ] 完成 [ ] 部分 [ ] 未开始 | |
| I2C 驱动 (DAC8562 幅度/偏置) | [ ] 完成 [ ] 部分 [ ] 未开始 | |
| I2C 驱动 (MAX17048 电量计) | [ ] 完成 [ ] 部分 [ ] 未开始 | |
| I2C 驱动 (FT6336 触摸) | [ ] 完成 [ ] 部分 [ ] 未开始 | |
| AXI Lite 通信 (PS-PL 接口) | [ ] 完成 [ ] 部分 [ ] 未开始 | |
| UART 串口通信 (上位机) | [ ] 完成 [ ] 部分 [ ] 未开始 | |

### 1.2 波形生成算法 | Waveform Generation Algorithm

请描述你是如何通过 PS 向 PL 端 BRAM 写入波形数据的：
Describe how you write waveform data from PS to PL-side BRAM:

（在此填写 / Write here）

**频率控制字 (FCW) 的计算方法：**

（在此填写 / Write here）

### 1.3 最有挑战的技术点 | Most Challenging Technical Point

Phase 2 中你遇到的最具挑战性的技术点是什么？你是如何克服的？
What was the most challenging technical point in Phase 2? How did you overcome it?

（在此填写 / Write here）

---

## 二、DAC 输出测试结果 | DAC Output Testing Results

### 2.1 波形输出测试 | Waveform Output Test

请使用示波器测试 DAC 输出，记录以下结果：
Use an oscilloscope to test DAC output and record the results:

| 测试波形 Waveform | 频率 Frequency | 预期输出 Expected | 实际输出 Actual | 输出质量 Quality |
|-----------------|---------------|-----------------|---------------|----------------|
| 正弦波 Sine | 1 kHz | 1Vpp | | 优/良/中/差 |
| 正弦波 Sine | 1 MHz | 1Vpp | | 优/良/中/差 |
| 方波 Square | 100 kHz | 1Vpp | | 优/良/中/差 |
| 三角波 Triangle | 10 kHz | 1Vpp | | 优/良/中/差 |
| 锯齿波 Ramp | 50 kHz | 1Vpp | | 优/良/中/差 |

### 2.2 性能指标测试 | Performance Metrics

| 指标 Parameter | 测量值 Measured | 目标值 Target |
|--------------|----------------|-------------|
| 最高输出频率 (正弦) | ____ MHz | 35 MHz (-3dB) |
| 频率精度 (1kHz 对比) | ____ % | < 1% |
| 输出幅度范围 | ____ Vpp | 0~10 Vpp |
| 幅度精度 | ____ % | < 5% |
| 偏置范围 | ____ V | +/-5 V |
| 输出噪声底 | ____ mVrms | < 10 mVrms |
| SFDR (无杂散动态范围) | ____ dBc | > 50 dBc |

### 2.3 示波器截图 | Oscilloscope Screenshots

**截图 1：1kHz 正弦波输出 | 1kHz Sine Wave Output**

（在此粘贴示波器截图 / Paste oscilloscope screenshot here）

**截图 2：100kHz 方波输出 | 100kHz Square Wave Output**

（在此粘贴示波器截图 / Paste oscilloscope screenshot here）

**截图 3：扫频模式输出 | Sweep Mode Output**

（在此粘贴示波器截图 / Paste oscilloscope screenshot here）

---

## 三、LVGL GUI 开发进展 | LVGL GUI Development Progress

### 3.1 GUI 功能完成情况 | GUI Feature Completion

| 功能 Feature | 完成状态 Status | 质量评估 Quality |
|-------------|----------------|-----------------|
| LCD 显示驱动 (SPI/RGB) | [ ] 完成 [ ] 部分 [ ] 未开始 | 优/良/中/差 |
| FT6336 触摸驱动 | [ ] 完成 [ ] 部分 [ ] 未开始 | 优/良/中/差 |
| LVGL 移植到 ZYNQ PS | [ ] 完成 [ ] 部分 [ ] 未开始 | 优/良/中/差 |
| 波形类型选择界面 | [ ] 完成 [ ] 部分 [ ] 未开始 | 优/良/中/差 |
| 频率参数调节 | [ ] 完成 [ ] 部分 [ ] 未开始 | 优/良/中/差 |
| 幅度参数调节 | [ ] 完成 [ ] 部分 [ ] 未开始 | 优/良/中/差 |
| 偏置参数调节 | [ ] 完成 [ ] 部分 [ ] 未开始 | 优/良/中/差 |
| 扫频模式界面 | [ ] 完成 [ ] 部分 [ ] 未开始 | 优/良/中/差 |
| AM 调制界面 | [ ] 完成 [ ] 部分 [ ] 未开始 | 优/良/中/差 |
| 波形预览显示 | [ ] 完成 [ ] 部分 [ ] 未开始 | 优/良/中/差 |

### 3.2 GUI 截图 | GUI Screenshots

**截图 1：TinyAWG 主界面 | Main Interface**

（在此粘贴 LCD 显示照片 / Paste LCD display photo here）

**截图 2：参数调节界面 | Parameter Adjustment Interface**

（在此粘贴 LCD 显示照片 / Paste LCD display photo here）

---

## 四、系统集成状态 | System Integration Status

### 4.1 PS-PL 通信测试 | PS-PL Communication Test

| 测试项 Test Item | 结果 Result | 说明 Notes |
|----------------|-----------|-----------|
| PS 通过 AXI 写入 BRAM | [ ] 通过 [ ] 失败 | |
| PS 写入频率控制字 (FCW) | [ ] 通过 [ ] 失败 | |
| PL DDS 正确响应 FCW 变化 | [ ] 通过 [ ] 失败 | |
| DAC 数据通路完整 | [ ] 通过 [ ] 失败 | |
| 触摸事件传递到 PS | [ ] 通过 [ ] 失败 | |
| GUI 参数更新触发波形变化 | [ ] 通过 [ ] 失败 | |

### 4.2 数据流验证 | Data Flow Verification

请描述完整的数据流：从用户触摸屏幕选择频率 -> 到 DAC 输出新频率波形的完整路径：
Describe the complete data flow: from user touching the screen to select frequency -> to DAC outputting the new frequency waveform:

（在此填写 / Write here）

---

## 五、技术挑战与解决方案 | Technical Challenges & Solutions

### 5.1 主要技术挑战 | Major Technical Challenges

**挑战 1：**
- 描述 (Description):
- 解决方案 (Solution):

**挑战 2：**
- 描述 (Description):
- 解决方案 (Solution):

### 5.2 调试技巧总结 | Debugging Tips

在 Phase 2 中，你总结了哪些有用的调试技巧？
What useful debugging techniques did you develop during Phase 2?

（在此填写 / Write here）

---

## 六、工作照片/视频 | Photos & Video

### 6.1 波形输出视频/照片 | Waveform Output Photos/Video

请提供 TinyAWG 在示波器上显示波形输出的照片或视频：
Provide photos or video of TinyAWG waveform output displayed on an oscilloscope:

**照片/视频 1：正弦波输出 + LCD 界面 (推荐将 TinyAWG 和示波器同时入镜)**

（在此粘贴 / Paste here）

**照片/视频 2：不同波形类型切换**

（在此粘贴 / Paste here）

### 6.2 系统整体照片 | System Overview Photo

（在此粘贴整机组装照片 / Paste photo of assembled system here）

---

## 七、Phase 3 计划 | Phase 3 Plan

### 7.1 系统集成与完善计划 | Integration & Refinement Plan

你计划在 Phase 3 中完成哪些工作？
What do you plan to accomplish in Phase 3?

- [ ] 系统联调 (所有模块协同工作)
- [ ] PCB 组装与焊接
- [ ] 电源测试与信号通路验证
- [ ] 外壳设计与 3D 打印
- [ ] 整机装配与调试
- [ ] 性能测试与校准
- [ ] 上位机通信功能
- [ ] 其他: _____________

### 7.2 时间安排 | Schedule

| 天数 Day | 计划完成内容 Planned Tasks |
|----------|--------------------------|
| Day 9 | |
| Day 10 | |
| Day 11 | |
| Day 12 | |

### 7.3 最终展示准备 | Final Presentation Preparation

你打算如何在最终展示中呈现你的项目？重点讲什么内容？
How do you plan to present your project at the final demo? What will you focus on?

（在此填写 / Write here）

---

## 八、团队协作 | Collaboration

### 8.1 Git 提交记录 | Git Commits

本周你的 Git 提交次数：_______ 次

### 8.2 互助情况 | Peer Support

你帮助了哪位同学解决了什么问题？或者你从哪位同学那里获得了帮助？
Who did you help, and who helped you? With what?

（在此填写 / Write here）

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
