# 详细评分表 | Detailed Grading Rubric

**学生姓名 | Student Name:** _______________
**评分人 | Evaluator:** _______________
**日期 | Date:** _______________

---

## 评分标准 | Grading Criteria

总分 115 分，分为六个维度。
Total: 115 points, divided into six dimensions.

---

### 1. 技术实现 (40 分) | Technical Implementation

#### 1.1 FPGA / DDS 设计与功能 (15 分) | FPGA / DDS Design & Functionality

| 等级 Level | 分数 Score | 标准 Criteria |
|-----------|-----------|--------------|
| 优秀 Excellent | 13-15 | Vivado Block Design 完整正确；DDS 模块（相位累加器 + 波形查找表）工作正常；支持多种波形类型；BRAM 波形表配置正确；综合和实现无错误；能通过 AXI 接口从 PS 动态控制频率 |
| 良好 Good | 10-12 | Block Design 基本完成；DDS 核心功能可用；支持至少 2 种波形；频率控制基本正常；偶有小问题 |
| 满意 Satisfactory | 7-9 | Block Design 部分完成；DDS 模块基本框架存在；至少有 1 种波形输出；需要较多人工干预 |
| 需改进 Needs Improvement | 0-6 | Block Design 未完成或有严重错误；DDS 模块无法工作；无有效波形输出 |

评分：____/15

备注：_______________________________________________

#### 1.2 ARM 软件与驱动实现 (10 分) | ARM Software & Driver Implementation

| 等级 Level | 分数 Score | 标准 Criteria |
|-----------|-----------|--------------|
| 优秀 Excellent | 9-10 | 所有驱动 (SPI, I2C, GPIO, AXI) 实现完整且稳定；CMake 构建系统配置正确；PS-PL 通信可靠；波形参数控制准确；串口通信可用 |
| 良好 Good | 7-8 | 主要驱动可用；基本 PS-PL 通信正常；波形参数控制基本准确；偶有不稳定 |
| 满意 Satisfactory | 5-6 | 部分驱动实现；PS-PL 通信基本可用；功能不完整但核心框架存在 |
| 需改进 Needs Improvement | 0-4 | 驱动严重缺失；PS-PL 通信不工作；软件框架不完整 |

评分：____/10

备注：_______________________________________________

#### 1.3 波形输出质量与精度 (10 分) | Waveform Output Quality & Accuracy

| 等级 Level | 分数 Score | 标准 Criteria |
|-----------|-----------|--------------|
| 优秀 Excellent | 9-10 | 输出波形干净、无明显失真；频率精度 < 1%；幅度可调范围达到 0~10Vpp；偏置可调达到 +/-5V；支持正弦/方波/三角波/锯齿波等至少 4 种波形；扫频/AM 调制模式工作正常 |
| 良好 Good | 7-8 | 波形输出基本正常；频率精度 < 3%；幅度可调；支持 2~3 种波形；扫频或调制至少一项可用 |
| 满意 Satisfactory | 5-6 | 能输出至少 1 种波形；频率基本正确；输出质量一般，有可见失真或噪声 |
| 需改进 Needs Improvement | 0-4 | 波形输出质量差或无法稳定输出；频率严重偏差 |

评分：____/10

备注：_______________________________________________

#### 1.4 系统集成完整度 (5 分) | System Integration Completeness

| 等级 Level | 分数 Score | 标准 Criteria |
|-----------|-----------|--------------|
| 优秀 Excellent | 5 | PS-PL 通信可靠；GUI 操作实时反映到波形输出；整机能稳定运行；电池供电正常 |
| 良好 Good | 4 | PS-PL 通信基本正常；GUI 到波形输出的联动基本工作；偶有不稳定 |
| 满意 Satisfactory | 3 | 系统集成部分完成；需要一些手动干预或 workaround |
| 需改进 Needs Improvement | 0-2 | 系统未集成；各模块独立但无法协同工作 |

评分：____/5

备注：_______________________________________________

**技术实现小计 Technical Subtotal：____/40**

---

### 2. 文档与展示 (30 分) | Documentation & Presentation

#### 2.1 README 质量 (8 分) | README Quality

| 等级 Level | 分数 Score | 标准 Criteria |
|-----------|-----------|--------------|
| 优秀 Excellent | 7-8 | README 完整包含项目介绍、ZYNQ 架构说明、DDS 原理、构建步骤、使用方法、接线图/框图；排版清晰美观 |
| 良好 Good | 5-6 | README 基本完整；有项目说明和使用方法；有架构图或框图 |
| 满意 Satisfactory | 3-4 | 有 README 但内容不够完整；缺少部分关键说明 |
| 需改进 Needs Improvement | 0-2 | README 严重缺失或几乎无内容 |

评分：____/8

备注：_______________________________________________

#### 2.2 代码文档 (7 分) | Code Documentation

| 等级 Level | 分数 Score | 标准 Criteria |
|-----------|-----------|--------------|
| 优秀 Excellent | 6-7 | 代码注释清晰完整（每个函数有功能说明）；模块结构清晰；有 Doxygen 或类似文档；Verilog 和 C 代码均有注释 |
| 良好 Good | 5 | 代码有较完整的注释；模块划分基本清晰；关键函数有说明 |
| 满意 Satisfactory | 3-4 | 代码有部分注释；模块划分存在但不清晰 |
| 需改进 Needs Improvement | 0-2 | 代码几乎没有注释；结构混乱 |

评分：____/7

备注：_______________________________________________

#### 2.3 最终展示 (10 分) | Final Presentation

| 等级 Level | 分数 Score | 标准 Criteria |
|-----------|-----------|--------------|
| 优秀 Excellent | 9-10 | PPT 内容充实，逻辑清晰；能清楚解释 ZYNQ 架构、DDS 原理、DAC 接口等核心技术；现场演示流畅，波形输出稳定；回答评委问题有理有据 |
| 良好 Good | 7-8 | PPT 基本完整；能解释主要技术原理；演示基本成功；回答问题基本正确 |
| 满意 Satisfactory | 5-6 | 有 PPT 但内容较浅；演示有部分成功；回答问题需要提示 |
| 需改进 Needs Improvement | 0-4 | PPT 不完整或无 PPT；演示失败；无法解释技术原理 |

评分：____/10

备注：_______________________________________________

#### 2.4 Demo 网站 (5 分) | Demo Website

| 等级 Level | 分数 Score | 标准 Criteria |
|-----------|-----------|--------------|
| 优秀 Excellent | 5 | 网站美观整洁，内容完整（含硬件展示、架构说明、FPGA 设计截图、演示视频/GIF、性能指标、学习心得）；技术解释清晰准确 |
| 良好 Good | 4 | 网站基本整洁，内容较完整；有图文配合；技术解释基本到位 |
| 满意 Satisfactory | 3 | 网站存在但内容不完整；排版一般；缺少部分模块 |
| 需改进 Needs Improvement | 0-2 | 网站无法访问或内容严重缺失 |

评分：____/5

备注：_______________________________________________

**文档与展示小计 Documentation & Presentation Subtotal：____/30**

---

### 3. 进度汇报 (20 分) | Check-in Reports

#### 3.1 第一周进度报告 (10 分) | Week 1 Check-in

| 等级 Level | 分数 Score | 标准 Criteria |
|-----------|-----------|--------------|
| 优秀 Excellent | 9-10 | 报告详实；ZYNQ/FPGA/DDS 知识点理解准确；Vivado 工程进度记录完整；实验截图清晰；技术挑战描述深入；Phase 2 计划合理可行 |
| 良好 Good | 7-8 | 报告完整；有知识总结和 Vivado 进度；有截图；有反思和计划 |
| 满意 Satisfactory | 5-6 | 有报告但内容较简略；截图不完整；反思不够深入 |
| 需改进 Needs Improvement | 3-4 | 报告不完整或过于简略 |
| 未提交 Not Submitted | 0-2 | 未提交或严重延迟 |

评分：____/10

备注：_______________________________________________

#### 3.2 第二周进度报告 (10 分) | Week 2 Check-in

| 等级 Level | 分数 Score | 标准 Criteria |
|-----------|-----------|--------------|
| 优秀 Excellent | 9-10 | 报告详实；有 DAC 输出测试数据和示波器截图；LVGL GUI 进度记录完整；系统集成测试结果清晰；技术挑战有深度分析 |
| 良好 Good | 7-8 | 报告完整；有波形输出截图；有 GUI 和系统集成进展说明 |
| 满意 Satisfactory | 5-6 | 有报告但内容较简略；测试数据不完整 |
| 需改进 Needs Improvement | 3-4 | 报告不完整或缺少关键测试数据 |
| 未提交 Not Submitted | 0-2 | 未提交或严重延迟 |

评分：____/10

备注：_______________________________________________

**进度汇报小计 Check-in Subtotal：____/20**

---

### 4. 团队协作 (10 分) | Collaboration

| 等级 Level | 分数 Score | 标准 Criteria |
|-----------|-----------|--------------|
| 优秀 Excellent | 9-10 | Git 提交规范 (commit message 清晰描述每次更改)；积极帮助同学解决问题；分工合理；代码审查参与积极 |
| 良好 Good | 7-8 | 有较规范的 Git 记录；偶尔帮助同学；基本分工合理 |
| 满意 Satisfactory | 5-6 | 有基本 Git 记录；参与度一般；分工不够清晰 |
| 需改进 Needs Improvement | 3-4 | Git 记录很少或不规范；参与度低 |
| 未参与 Not Participated | 0-2 | 无 Git 记录或完全不参与 |

评分：____/10

备注：_______________________________________________

---

### 5. Demo 网站 (15 分) | Project Demo Website

#### 5.1 网站可访问性 (5 分) | Website Accessibility

| 等级 Level | 分数 Score | 标准 Criteria |
|-----------|-----------|--------------|
| 优秀 Excellent | 5 | URL 可正常打开，页面加载完整，无 404 错误，响应速度快 |
| 良好 Good | 3-4 | URL 可访问但加载较慢或有部分内容缺失 |
| 满意 Satisfactory | 1-2 | URL 偶尔可访问，稳定性不足 |
| 需改进 Needs Improvement | 0 | 无法访问或未提交 URL |

评分：____/5

#### 5.2 内容完整性 (5 分) | Content Completeness

| 等级 Level | 分数 Score | 标准 Criteria |
|-----------|-----------|--------------|
| 优秀 Excellent | 5 | 包含全部 9 项要求内容（项目介绍、硬件展示、技术架构、FPGA 设计、软件架构、演示视频/GIF、性能指标、学习心得、B 站参考链接） |
| 良好 Good | 3-4 | 包含 6-8 项要求内容 |
| 满意 Satisfactory | 1-2 | 包含 3-5 项要求内容 |
| 需改进 Needs Improvement | 0 | 内容严重缺失（少于 3 项） |

评分：____/5

#### 5.3 视觉呈现与技术深度 (5 分) | Visual Presentation & Technical Depth

| 等级 Level | 分数 Score | 标准 Criteria |
|-----------|-----------|--------------|
| 优秀 Excellent | 5 | 页面整洁美观，排版合理，图文并茂；对 ZYNQ 架构、DDS 原理、DAC 接口等核心技术的解释清晰准确 |
| 良好 Good | 3-4 | 页面基本整洁，有图文配合，技术解释基本到位 |
| 满意 Satisfactory | 1-2 | 页面排版一般，图片较少，技术解释较浅 |
| 需改进 Needs Improvement | 0 | 页面混乱或无技术内容 |

评分：____/5

**Demo 网站小计 Demo Website Subtotal：____/15**

---

## 总分汇总 | Score Summary

| 维度 Dimension | 满分 Max | 得分 Score |
|---------------|---------|-----------|
| 技术实现 Technical Implementation | 40 | |
| 文档与展示 Documentation & Presentation | 30 | |
| 进度汇报 Check-in Reports | 20 | |
| 团队协作 Collaboration | 10 | |
| Demo 网站 Demo Website | 15 | |
| **总分 Total** | **115** | |

---

## 等级 | Grade

| 等级 Grade | 分数范围 Score Range | 本次评定 This Assessment |
|-----------|--------------------|-----------------------|
| A+ (卓越 Outstanding) | 109-115 | [ ] |
| A (优秀 Excellent) | 98-108 | [ ] |
| B+ (良好 Good) | 86-97 | [ ] |
| B (中等 Satisfactory) | 75-85 | [ ] |
| C+ (及格 Pass) | 63-74 | [ ] |
| C (及格 Marginal Pass) | 52-62 | [ ] |
| D (不及格 Fail) | 0-51 | [ ] |

---

## 评语 | Comments

**优点 Strengths:**

_______________________________________________

_______________________________________________

**改进建议 Suggestions for Improvement:**

_______________________________________________

_______________________________________________

**额外加分说明 Bonus Points (如有/if any):**

_______________________________________________

---

**评分人签名 | Evaluator Signature:** _______________

**日期 | Date:** _______________

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
