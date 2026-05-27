# 示波器项目复查报告

复查日期：2026-05-26

## 总体评价

项目整体质量很高，文件结构完整，中英双语覆盖全面，课程设计逻辑清晰、由浅入深，技术内容与高中生水平匹配。主要问题集中在**评分权重不一致**（部分文件含 Demo Website 13%，部分文件不含）和**一处引脚冲突**（PA9 同时被分配给 USART1_TX 和波形切换按键）。此外，多数 day-XX.md 文件缺少直接 B 站视频链接（仅有搜索建议），且 Day 09 文件末尾"最后更新"行格式不一致。

## 检查结果

### 1. 文件完整性

- [x] README.md
- [x] resources/search-results.md
- [x] curriculum/overview.md
- [x] curriculum/prerequisites.md
- [x] curriculum/assignments.md
- [x] curriculum/grading-rubric.md
- [x] curriculum/day-01.md
- [x] curriculum/day-02.md
- [x] curriculum/day-03.md
- [x] curriculum/day-04.md
- [x] curriculum/day-05.md
- [x] curriculum/day-06.md
- [x] curriculum/day-07.md
- [x] curriculum/day-08.md
- [x] curriculum/day-09.md
- [x] curriculum/day-10.md
- [x] curriculum/day-11.md
- [x] curriculum/day-12.md
- [x] hardware/BOM.md
- [x] hardware/wiring-guide.md
- [x] hardware/assembly-steps.md
- [x] hardware/troubleshooting.md
- [x] software/requirements.txt
- [x] software/config.template.yaml
- [x] software/src/main.py
- [x] software/src/adc_sampler.py
- [x] software/src/waveform.py
- [x] software/src/trigger.py
- [x] software/src/display.py
- [x] software/src/signal_gen.py
- [x] software/src/utils.py
- [x] software/tests/test_basic.py
- [x] assignments/week-1-checkin.md
- [x] assignments/week-2-checkin.md
- [x] assignments/final-presentation.md
- [x] assignments/rubric.md

**结论：** 全部 35 个必需文件均存在，无遗漏。

---

### 2. 内容质量

**README.md：**
- B 站视频链接：有（8 条视频，涵盖 DIY 教程和原理讲解）
- BOM 总预算：约 140 元，远低于 500 元上限
- 中英双语：完整覆盖

**每个 day-XX.md：**
- "为什么学这个"部分：全部 12 天均有此部分
- 时间安排：全部 12 天均有详细时间表
- 作业：全部 12 天均有"今日作业"部分
- 明日预告：全部 12 天均有（Day 12 为课程回顾）

**BOM.md：**
- 价格合理（核心模块约 50 元，总约 140 元）
- 有采购时间建议和替代方案
- 有购买建议（淘宝搜索关键词）

**代码文件中文注释：**
- 所有 7 个 Python 文件均有中文注释和 docstring，且同时提供英文翻译
- 注释风格统一（模块级 docstring + 函数级 docstring + 关键行注释）

**Demo 网站要求：**
- final-presentation.md：有完整的 Demo 网站要求（第 74-117 行），含评分标准（15 分）
- rubric.md：有 Demo 网站维度（第 156-191 行），15 分
- day-12.md：任务 12.2b 专门描述创建项目展示网站（第 80-116 行）
- README.md：Day 12 行提到"Demo 网站部署"，评分标准表包含 Demo 网站 13%

---

### 3. 一致性

**课程天数：** 全部一致，均为 12 天。

**评分标准不一致（严重问题）：**

以下两个文件使用 CLAUDE.md 模板权重（不含 Demo Website）：
- `curriculum/grading-rubric.md`：技术 40% / 文档 10% / 展示 20% / 汇报 20% / 协作 10%（总 100%，无 Demo Website）
- `curriculum/overview.md`：技术 40% / 文档 10% / 展示 20% / 汇报 20% / 协作 10%（总 100%，无 Demo Website）

以下四个文件使用新版权重（含 Demo Website）：
- `README.md`：技术 35% / 文档 9% / 展示 17% / 汇报 17% / 协作 9% / Demo 13%（总 100%）
- `assignments/rubric.md`：技术 40 / 文档 10 / 展示 20 / 汇报 20 / 协作 10 / Demo 15（总 115 分，对应百分比与 README 一致）
- `assignments/final-presentation.md`：Demo 网站 15 分
- `curriculum/day-12.md`：最终评分表含 Demo 网网站 /15，权重 13%

`curriculum/grading-rubric.md` 和 `curriculum/overview.md` 需要更新以包含 Demo Website 维度。

**硬件引脚冲突（严重问题）：**

`hardware/wiring-guide.md` 第 30-34 行的接线总览图中：
- PA9 同时被分配为 GPIO_IN（按键4 波形切换）和 USART1_TX（串口调试）
- 这两个功能不能共享同一引脚，需要选择其中一个功能重新分配到其他引脚

**硬件清单 vs 接线指南一致性：**

BOM.md 列出的元器件与 wiring-guide.md 中使用的元器件基本一致（STM32、ST7789 TFT、LM358、AD9833、电阻电容、按键、面包板、杜邦线）。BOM.md 中列出了 4 个轻触按键，wiring-guide.md 和 day 文件中也使用 4 个按键，一致。

**课程技术点与代码模块对应：**

| 课程内容 | 对应代码模块 |
|---------|------------|
| Day 3-4: ADC 采样 + DMA | adc_sampler.py |
| Day 5: 触发逻辑 | trigger.py |
| Day 6: TFT/波形显示 | display.py |
| Day 8: 频率测量 + 波形分析 | waveform.py |
| Day 9: 信号发生器 | signal_gen.py |
| 配置管理 + 工具 | utils.py |
| PC 端集成 | main.py |

对应关系完整。

---

### 4. B 站视频链接

**README.md：** 有 8 条直接 B 站视频链接（BV 号格式），涵盖 DIY 教程和原理讲解。

**search-results.md：** 有完整的视频列表（50+ 条），分类详细，包括 DIY 教程、原理讲解、DSO138、FPGA 方案、成品评测等。

**day-XX.md 中的视频链接：**

| 文件 | 直接链接 | 仅搜索建议 |
|------|---------|-----------|
| day-01.md | 2 条（示波器原理、触发原理） | 2 条 |
| day-02.md | 1 条（电路讲解） | 2 条 |
| day-03.md | 2 条（ADC+DMA、双 ADC） | 2 条 |
| day-04.md | 3 条（ADC+DMA、双 ADC、DIY 原理） | 2 条 |
| day-05.md | 1 条（触发原理） | 1 条 |
| day-06.md | 1 条（三代示波器波形显示） | 2 条 |
| day-07.md | 无 | 2 条 |
| day-08.md | 无 | 3 条 |
| day-09.md | 1 条（多功能仪器） | 3 条 |
| day-10.md | 1 条（嘉立创 PCB 教程） | 2 条 |
| day-11.md | 无 | 4 条 |
| day-12.md | 无 | 无 |

**问题：** Day 07、Day 08、Day 11、Day 12 没有直接 B 站视频链接，仅有搜索建议。考虑到 search-results.md 中有大量相关视频，建议为这些天补充直接链接。

---

### 5. 格式

**"最后更新"日期：**
- 35 个文件中 34 个格式统一为 `*最后更新：2026-05-26*`
- `curriculum/day-09.md`（第 353 行）格式不一致：`*Last updated:2026-05-26*`（缺少空格，应为 `*Last updated: 2026-05-26*`）

**中英双语：** 全部文件均为中英双语，覆盖完整。

**表格格式：** 所有表格的 Markdown 语法正确，列对齐正常，无渲染问题。

---

## 需要修复的问题

### 问题 1（严重）：评分权重不一致
- 文件：`curriculum/grading-rubric.md`（全文）
- 文件：`curriculum/overview.md`（第 112-121 行，评估方式表）
- 问题：这两个文件使用 CLAUDE.md 模板权重（技术 40% / 文档 10% / 展示 20% / 汇报 20% / 协作 10%），不含 Demo Website 维度。而 README.md、rubric.md、final-presentation.md、day-12.md 均已包含 Demo Website（13%/15分）。
- 建议：更新 grading-rubric.md 和 overview.md，添加 Demo Website 维度，调整权重使其与其他文件一致。

### 问题 2（严重）：PA9 引脚冲突
- 文件：`hardware/wiring-guide.md`（第 30-34 行）
- 问题：PA9 同时被分配为 GPIO_IN（按键4 波形切换）和 USART1_TX（串口调试），两个功能不能共用一个引脚。
- 建议：将按键4（波形切换）的引脚改为 PB1 或其他空闲 GPIO 引脚。同步更新所有涉及按键接线的文件（wiring-guide.md 第 139-143 行、assembly-steps.md 第 183-188 行、day-09.md 按键部分）。

### 问题 3（轻微）：Day 09 文件末尾格式不一致
- 文件：`curriculum/day-09.md`（第 353 行）
- 问题：`*Last updated:2026-05-26*` 缺少冒号后的空格
- 建议：改为 `*Last updated: 2026-05-26*`

### 问题 4（轻微）：部分 day-XX.md 缺少直接 B 站视频链接
- 文件：`curriculum/day-07.md`、`curriculum/day-08.md`、`curriculum/day-11.md`、`curriculum/day-12.md`
- 问题：这些文件的参考资源部分仅有搜索建议（如"搜索 XXX"），没有直接的 B 站视频链接。而 search-results.md 中有大量可直接引用的视频。
- 建议：从 search-results.md 中选取合适的视频链接，添加到这些 day 文件的参考资源表中。

---

## 修复建议

### 修复 1：统一评分权重

将 `curriculum/grading-rubric.md` 更新为与 `assignments/rubric.md` 一致的六维度评分体系：

| 维度 | 权重 | 满分 |
|------|------|------|
| 技术实现 | 35% | 40 |
| 文档质量 | 9% | 10 |
| 演示展示 | 17% | 20 |
| 进度汇报 | 17% | 20 |
| 团队协作 | 9% | 10 |
| Demo 网站 | 13% | 15 |
| **总计** | **100%** | **115** |

同时更新 `curriculum/overview.md` 第 112-121 行的评估方式表。

### 修复 2：解决 PA9 引脚冲突

推荐方案：
- 按键4（波形切换）改用 PB1（目前未使用）
- 更新文件列表：
  - `hardware/wiring-guide.md`：接线总览图（第 34 行）、按键接线表（第 143 行）
  - `hardware/assembly-steps.md`：Step 7 按键接线（第 188 行）
  - `curriculum/day-09.md`：按键接线说明（约第 299-300 行）

### 修复 3：Day 09 格式修正

将 `curriculum/day-09.md` 第 353 行：
```
*Last updated:2026-05-26*
```
改为：
```
*Last updated: 2026-05-26*
```

### 修复 4：补充 B 站视频链接

建议为 day-07、day-08、day-11、day-12 补充以下视频：

**day-07.md（时基控制）：**
- DSO138 时基相关：从 search-results.md 中 DSO138 系列选取

**day-08.md（频率测量）：**
- 搜索结果中的示波器使用教程视频

**day-11.md（外壳设计）：**
- TinkerCAD 或 3D 打印入门视频

**day-12.md（展示）：**
- 可不添加，因为最后一天以展示为主，无新知识输入
