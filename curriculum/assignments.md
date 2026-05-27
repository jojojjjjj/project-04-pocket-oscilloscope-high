# 作业说明 | Assignments Overview

## 概述 | Overview

本课程共设置三类作业：每日实验日志、每周进度报告、最终项目展示。所有作业均需提交至个人 GitHub 仓库。

This course has three types of assignments: daily lab journals, weekly check-in reports, and final project presentation. All assignments should be submitted to your personal GitHub repository.

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

**提交时间：** Day 5 结束前
**提交方式：** 提交至 `assignments/week-1-checkin.md`
**详细要求：** 见 `assignments/week-1-checkin.md`

**核心内容：**
- Phase 1 学习总结（模拟电路、ADC 采样）
- 硬件搭建进展与照片
- 遇到的主要技术挑战及解决方案
- Phase 2 计划与预期目标

### 3. 第二周进度报告 | Week 2 Check-in Report

**提交时间：** Day 9 结束前
**提交方式：** 提交至 `assignments/week-2-checkin.md`
**详细要求：** 见 `assignments/week-2-checkin.md`

**核心内容：**
- Phase 2 学习总结（触发逻辑、波形显示、时基控制）
- 示波器功能演示（截图或视频）
- 频率测量精度分析
- Phase 3 拓展计划

### 4. 最终项目展示 | Final Presentation

**提交时间：** Day 12
**提交方式：** 现场展示 + GitHub 提交全部项目文件
**详细要求：** 见 `assignments/final-presentation.md`

**核心内容：**
- 5 分钟 PPT 演示
- 现场功能演示（测量标准信号源）
- 技术原理讲解（能回答评委提问）
- 项目复盘与反思

---

## 提交规范 | Submission Guidelines

### GitHub 仓库结构

```
oscilloscope-project/
├── README.md              # 项目说明
├── journal/               # 每日实验日志
│   ├── day-01.md
│   ├── day-02.md
│   └── ...
├── assignments/           # 作业提交
│   ├── week-1-checkin.md
│   ├── week-2-checkin.md
│   └── final-presentation.md
├── hardware/              # 硬件相关
│   ├── wiring-diagram.png
│   └── photos/
├── software/              # 源代码
│   ├── src/
│   └── ...
└── docs/                  # 文档资料
    ├── schematics/
    └── datasheets/
```

### Git 提交规范

每次提交的 commit message 应遵循以下格式：

```
<类型>(<范围>): <简要描述>

类型包括：
- feat: 新功能
- fix: 修复
- docs: 文档
- hw: 硬件相关
- refactor: 重构
```

**示例：**
- `feat(adc): 完成 ADC+DMA 连续采样`
- `fix(trigger): 修复上升沿触发误判`
- `hw(wiring): 更新 TFT 接线图`
- `docs(journal): 添加 Day 3 实验日志`

---

## 评分权重 | Grading Weight

| 作业 Assignment | 权重 Weight | 说明 Description |
|----------------|-----------|-----------------|
| 每日实验日志 Daily Journals | 10% | 12 篇日志的平均质量 |
| 第一周进度报告 Week 1 Check-in | 10% | Phase 1 学习成效 |
| 第二周进度报告 Week 2 Check-in | 10% | Phase 2 学习成效 |
| 最终项目展示 Final Presentation | 20% | 综合表现 |
| 技术实现 Technical Implementation | 40% | 最终作品的功能完整度 |
| 文档质量 Documentation | 10% | README、注释、图表的完整性 |

---

## 学术诚信 | Academic Integrity

- **鼓励参考开源项目**：但必须在代码注释和 README 中注明来源
- **鼓励互相讨论**：但每位同学必须独立完成自己的项目
- **禁止直接复制粘贴**：用自己的话重写代码和文档
- **使用 AI 辅助学习可以**：但需要理解 AI 生成的内容，能解释其工作原理

*Collaboration is encouraged, but each student must complete their own project. Reference to open-source projects is welcome with proper attribution. Using AI tools for learning is allowed, but you must understand the generated content and be able to explain how it works.*

---

*最后更新：2026-05-26*
*Last updated: 2026-05-26*
