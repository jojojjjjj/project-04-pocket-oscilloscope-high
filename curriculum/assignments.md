# 作业说明 | Assignments Overview

## 概述 | Overview

本课程共设置四类作业：每日实验日志、第一周进度报告、第二周进度报告、最终项目展示。所有作业均需提交至个人 GitHub 仓库。

This course has four types of assignments: daily lab journals, week 1 check-in report, week 2 check-in report, and final project presentation. All assignments should be submitted to your personal GitHub repository.

---

## 作业清单 | Assignment List

### 1. 每日实验日志 | Daily Lab Journal

**频率：** 每天
**提交方式：** 在 GitHub 仓库的 `journal/` 目录下新建 `day-XX.md`
**截止时间：** 当天课程结束前 30 分钟

**内容要求：**

- 今日学到了什么新知识？（用自己的话解释，不要复制粘贴）
- 完成了哪些实验？遇到了什么问题？如何解决的？
- 有什么还没有理解清楚的地方？
- 一句话总结今天最大的收获

**字数要求：** 200~500 字（中英文均可）

### 2. 第一周进度报告 | Week 1 Check-in Report

**提交时间：** Day 4 结束前
**提交方式：** 提交至 `assignments/week-1-checkin.md`
**详细要求：** 见 `assignments/week-1-checkin.md`

**核心内容：**

- Phase 1 学习总结（ZYNQ 架构理解、Verilog 基础、FPGA 设计流程、DDS 原理）
- FPGA 仿真与验证进展（截图或逻辑分析仪截图）
- 遇到的主要技术挑战及解决方案
- Phase 2 计划与预期目标

### 3. 第二周进度报告 | Week 2 Check-in Report

**提交时间：** Day 8 结束前
**提交方式：** 提交至 `assignments/week-2-checkin.md`
**详细要求：** 见 `assignments/week-2-checkin.md`

**核心内容：**

- Phase 2 学习总结（ARM PS 开发、AXI 通信、LVGL GUI）
- PS-PL 联合调试进展（截图或视频）
- GUI 界面功能演示
- Phase 3 系统集成计划

### 4. 最终项目展示 | Final Presentation

**提交时间：** Day 12
**提交方式：** 现场展示 + GitHub 提交全部项目文件
**详细要求：** 见 `assignments/final-presentation.md`

**核心内容：**

- 5 分钟 PPT 演示
- 现场功能演示（用示波器验证波形输出）
- 技术原理讲解（能回答评委提问）
- 项目复盘与反思

---

## 提交规范 | Submission Guidelines

### GitHub 仓库结构

```
tinyawg-project/
├── README.md              # 项目说明
├── journal/               # 每日实验日志
│   ├── day-01.md
│   ├── day-02.md
│   └── ...
├── assignments/           # 作业提交
│   ├── week-1-checkin.md
│   ├── week-2-checkin.md
│   └── final-presentation.md
├── fpga/                  # FPGA 设计文件
│   ├── vivado-project/    # Vivado 工程
│   ├── src/               # Verilog 源文件
│   └── sim/               # 仿真文件
├── software/              # ARM PS 软件代码
│   ├── src/               # C 源文件
│   ├── lvgl/              # LVGL 相关
│   └── CMakeLists.txt
├── hardware/              # 硬件相关
│   ├── schematics/        # 原理图
│   └── photos/            # 组装照片
└── docs/                  # 文档资料
    ├── architecture.md    # 系统架构说明
    └── register-map.md    # 寄存器映射表
```

### Git 提交规范

每次提交的 commit message 应遵循以下格式：

```
<类型>(<范围>): <简要描述>

类型包括：
- feat: 新功能
- fix: 修复
- docs: 文档
- fpga: FPGA/Verilog 相关
- sw: ARM 软件相关
- hw: 硬件相关
- gui: LVGL GUI 相关
- refactor: 重构
```

**示例：**
- `feat(fpga): 实现 DDS 相位累加器模块`
- `fix(axi): 修复 AXI Lite 寄存器读写时序`
- `sw(uart): 添加串口调试输出`
- `gui(wave): 添加波形参数调节滑块`
- `hw(solder): 完成核心板焊接`
- `docs(journal): 添加 Day 3 实验日志`

---

## 评分权重 | Grading Weight

| 作业 Assignment | 权重 Weight | 说明 Description |
|----------------|-----------|-----------------|
| 每日实验日志 Daily Journals | 贯穿 Ongoing | 12 篇日志的平均质量 |
| 第一周进度报告 Week 1 Check-in | 17% | Phase 1 (FPGA + DDS) 学习成效 |
| 第二周进度报告 Week 2 Check-in | 17% | Phase 2 (ARM + GUI) 学习成效 |
| 最终项目展示 Final Presentation | 17% | 综合表现 |
| 技术实现 Technical Implementation | 35% | 最终作品的功能完整度 |
| 文档质量 Documentation | 9% | README、注释、架构图的完整性 |
| Demo 网站 Project Demo Website | 13% | 项目展示网站的完整度 |

详细评分标准见 `grading-rubric.md`。

---

## 学术诚信 | Academic Integrity

- **鼓励参考开源项目**：TinyAWG 本身就是开源项目，但必须在代码注释和 README 中注明哪些是参考的、哪些是自己编写/修改的
- **鼓励互相讨论**：FPGA 和嵌入式开发中互相帮助是很重要的能力，但每位同学必须独立完成自己的项目
- **禁止直接复制粘贴**：用 Verilog 重写模块时要理解每一行代码的含义，能解释其工作原理
- **使用 AI 辅助学习可以**：但需要理解 AI 生成的内容，能解释代码的工作原理和设计意图

*Collaboration is encouraged, but each student must complete their own project. TinyAWG is itself open-source; you must attribute which parts are referenced vs. self-written. Using AI tools for learning is allowed, but you must understand the generated content and be able to explain how it works.*

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
