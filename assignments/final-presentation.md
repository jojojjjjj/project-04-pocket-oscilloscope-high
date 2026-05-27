# 最终展示要求 | Final Presentation Requirements

## 展示概述 | Presentation Overview

| 项目 Item | 说明 Description |
|-----------|----------------|
| 时间 Timing | Day 12, 每组 8 分钟 (5 分钟演讲 + 2 分钟演示 + 1 分钟提问) |
| 形式 Format | PPT 演讲 + 现场功能演示 |
| 评委 Evaluators | 导师 + 外部评委 (如有) |
| 语言 Language | 中文或英文均可 |

---

## 一、PPT 结构 (5 分钟) | PPT Structure

| 页码 Slide | 内容 Content | 建议时间 Time |
|-----------|-------------|-------------|
| 1 | 封面：项目名称 TinyAWG、姓名、日期 | 15 秒 |
| 2 | 项目简介：什么是 AWG？为什么做？挑战在哪？ | 30 秒 |
| 3 | 技术架构：ZYNQ PS+PL 双核架构图、DDS 原理框图 | 45 秒 |
| 4 | FPGA 设计：Block Design 截图、DDS 模块、BRAM 波形表、DAC 接口 | 45 秒 |
| 5 | ARM 软件：驱动架构、波形数据处理流程、PS-PL 通信 | 45 秒 |
| 6 | LVGL GUI：界面截图、触摸交互、参数控制流程 | 30 秒 |
| 7 | 成果展示：示波器波形截图、不同模式输出、性能指标 | 45 秒 |
| 8 | 挑战与反思：遇到的困难、解决方法、学到了什么 | 30 秒 |

### PPT 制作要求 | PPT Design Requirements

- 每页不超过 5 行文字，以图片和图表为主
- 使用清晰的配色方案，字体至少 24pt
- FPGA 部分必须有 Vivado Block Design 截图或 RTL 电路图
- 软件部分必须有核心代码片段或数据流图
- 成果部分必须有示波器波形显示截图
- 准备一个备用方案（录屏视频），以防现场演示失败

### PPT 模板建议 | PPT Template Suggestion

```
配色方案：
  背景色：深蓝色 (#0a1628)
  强调色：信号绿 (#00ff88) (模拟信号波形色)
  波形色：亮黄色 (#ffdd00) (模拟示波器荧光)
  文字色：白色 (#ffffff)
  标题色：亮蓝色 (#00aaff)
```

---

## 二、现场演示要求 | Live Demo Requirements

### 必做演示 (Must Demo)

1. **波形输出：** 在示波器上展示 TinyAWG 输出的正弦波，波形清晰稳定
2. **多种波形类型：** 演示正弦波、方波、三角波切换输出
3. **频率调节：** 通过触摸屏调节输出频率，示波器实时验证
4. **幅度/偏置调节：** 演示 Vpp 和偏置的实时调节
5. **LVGL GUI 交互：** 展示触摸屏操作界面，响应流畅

### 加分演示 (Bonus Demo)

6. **扫频模式 (Sweep)：** 演示从低频到高频的自动扫频输出
7. **AM 调制模式：** 演示幅度调制信号输出
8. **任意波形：** 演示自定义波形的加载和输出
9. **PCB 实物：** 展示自行焊接的 PCB 板
10. **3D 外壳：** 展示 3D 打印的外壳
11. **上位机通信：** 通过 PC 端软件控制 TinyAWG
12. **电池供电运行：** 展示脱离外部电源的便携运行

### 演示注意事项 | Demo Tips

- 提前 30 分钟到场调试设备
- 准备好示波器探头、MCX-BNC 转接线和电源线
- 带一根备用 USB 线和备用电池
- 示波器提前校准好
- 如果某项功能失败，不要慌张，讲讲你的排查思路

---

## 三、技术 Q&A 准备 | Technical Q&A Preparation

评委可能会问以下问题，请提前准备：
The evaluators may ask the following questions. Please prepare in advance:

### 基础问题 | Foundational Questions

- "请简要解释 TinyAWG 是如何产生波形的？" -- 请从 DDS 原理讲起
- "ZYNQ 的 PS 和 PL 是如何分工协作的？在这个项目中各自负责什么？"
- "AD9744 DAC 的数据接口是如何工作的？FPGA 是如何将波形数据送到 DAC 的？"
- "频率控制字 (FCW) 是怎么计算的？为什么改变 FCW 就能改变输出频率？"

### 进阶问题 | Advanced Questions

- "如果要输出 50MHz 的正弦波，你的系统能做到吗？有什么限制？"
- "DAC8562 在系统中起什么作用？为什么要用两片 DAC？"
- "你的 DDS 波形表有多少个点？为什么选择这个点数？更多点会有什么影响？"
- "AM 调制模式是如何实现的？载波和调制信号分别在哪里产生？"
- "扫频模式的频率是如何变化的？是线性扫频还是对数扫频？"

### 拓展问题 | Extension Questions

- "如果给你更多时间和预算，你会增加什么功能？"
- "这个项目和商业信号发生器（如 Rigol DG800）相比有什么差距？"
- "这个项目对你未来的学习方向有什么影响？"
- "你会推荐其他同学做这个项目吗？为什么？"

---

## 四、项目展示网站 | Project Demo Website

**这是最终评分的重要组成部分。** 你必须创建一个在线可访问的项目展示网站。

**This is a critical part of your final grade.** You must create an online-accessible project demo website.

### 要求 | Requirements

1. **部署平台** -- 使用以下任一免费平台部署：
   - GitHub Pages（推荐）
   - Vercel -- 连接 GitHub 仓库自动部署
   - Netlify -- 拖拽文件夹即可部署
   - 其他可公网访问的平台均可

2. **网站内容必须包含：**
   - [ ] 项目介绍（TinyAWG 名称、目标、核心功能）
   - [ ] 硬件展示（BOM 清单、实物照片、ZYNQ 核心板、DAC 模块、电路原理图）
   - [ ] 技术架构（ZYNQ PS+PL 架构图、DDS 原理图、数据流图、模块说明）
   - [ ] FPGA 设计（Vivado Block Design 截图、RTL 电路、资源使用报告）
   - [ ] 软件架构（驱动层、应用层、GUI 层的分层说明、关键代码片段）
   - [ ] 演示视频或 GIF（展示 TinyAWG 输出波形在示波器上的效果）
   - [ ] 性能指标（实测带宽、频率精度、输出幅度范围、波形截图）
   - [ ] 12 天学习心得与总结
   - [ ] B 站参考视频链接列表

3. **技术建议：**
   - 最简方案：一个 `index.html` 文件 + CSS 样式即可
   - 可使用 GitHub Pages 的 Jekyll 主题快速生成
   - 推荐工具：Markdown + 静态网站生成器（如 VitePress、Hugo）

### 提交方式 | Submission

- 在最终展示前，将网站 URL 提交给老师
- URL 格式示例：`https://your-username.github.io/tinyawg-project/`
- 老师将通过访问该 URL 进行评分

### 评分标准 | Scoring

| 维度 Dimension | 分值 Points | 说明 Description |
|---------------|-----------|-----------------|
| 网站可访问性 Accessibility | 5 | URL 可正常打开，无 404 错误 |
| 内容完整性 Completeness | 5 | 包含所有要求的内容模块 |
| 视觉呈现 Visual Quality | 3 | 页面整洁、排版合理、图文并茂 |
| 技术展示深度 Technical Depth | 2 | 对核心技术的解释清晰准确 |
| **小计 Subtotal** | **15** | |

---

## 五、文档提交要求 | Documentation Submission Requirements

Day 12 展示前，请在 GitHub 仓库中提交以下材料：
Before the Day 12 presentation, submit the following in your GitHub repository:

### 必须提交 | Required Submissions

- [ ] `README.md` -- 项目说明文档（含项目介绍、架构说明、使用方法）
- [ ] `software/src/` -- 所有源代码（ARM C 代码 + Verilog/RTL 代码）
- [ ] `hardware/` -- 接线图、Vivado 工程截图、硬件照片
- [ ] `assignments/week-1-checkin.md` -- 第一周进度报告（已填写）
- [ ] `assignments/week-2-checkin.md` -- 第二周进度报告（已填写）
- [ ] `assignments/final-presentation.md` -- 本文件（已填写）
- [ ] PPT 文件 (可放在 `docs/` 目录)
- [ ] 12 篇实验日志 (`journal/day-01.md` ~ `day-12.md`)

### 加分提交 | Bonus Submissions

- [ ] 演示视频 (放在 `docs/` 目录)
- [ ] Vivado 工程文件 (`.xsa` 导出文件)
- [ ] PCB 设计文件 (嘉立创 EDA 源文件)
- [ ] 3D 外壳设计文件 (STL 或 Fusion 360 文件)

---

## 六、自我评估 | Self-Assessment

在展示前，请对自己的项目进行自我评估：
Before presenting, please self-assess your project:

| 维度 Dimension | 自评分 (1-10) Self-Score | 说明 Notes |
|---------------|-------------------------|-----------|
| FPGA/DDS 设计 | | DDS 模块设计、Block Design 完成度 |
| ARM 软件开发 | | 驱动实现、波形控制逻辑 |
| 波形输出质量 | | 输出波形是否干净、频率是否准确 |
| GUI 交互体验 | | LVGL 界面美观度、触摸响应 |
| 系统集成 | | PS-PL 通信、整机工作稳定性 |
| 文档质量 | | README、注释、报告完整性 |
| 展示准备 | | PPT 和演示准备程度 |

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
