# Day 12: 测试校准与项目展示 | Testing, Calibration & Project Presentation

> **今日目标 Today's Objectives:**
> - 完成全面性能测试：频率精度、幅度平坦度、波形质量 (THD)、扫频、AM 调制
> - 记录并文档化所有测试结果
> - 进行 5 分钟 PPT 项目展示与实时波形生成演示
> - 技术答辩与项目反思
>
> **产出 Deliverable:** 性能测试报告完成，5 分钟 PPT 展示 + 实时 Demo + 技术问答

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 | Day 11 回顾 + 今日测试+展示安排 |
| 09:30-10:30 | 动手实验 | 频率精度 + 幅度平坦度测试 |
| 10:30-10:45 | 休息 | Break |
| 10:45-12:00 | 动手实验 | 波形质量 THD + 扫频 + AM 调制测试 |
| 12:00-13:30 | 午餐 | Lunch + Break |
| 13:30-14:00 | 准备 | PPT 最终检查 + Demo 准备 |
| 14:00-15:30 | 展示 | 项目展示 (每人 8 分钟: 5 分钟 PPT + 3 分钟 Demo+QA) |
| 15:30-15:45 | 休息 | Break |
| 15:45-16:30 | 回顾 | 同学互评 + 集体讨论 |
| 16:30-17:00 | 总结 | 项目复盘 + 结营仪式 |

---

## 上午: 全面性能测试 | Morning: Comprehensive Performance Testing

### 为什么要学这个? | Why Learn This?

性能测试是产品交付前的最后一步，也是工程师最重视的环节。一份完整的测试报告不仅证明了你的作品达到了设计指标，更体现了严谨的工程态度。WavePocket 作为一台信号发生器，核心指标包括频率精度、幅度平坦度和波形质量 (THD)。今天我们要像真正的产品测试工程师一样，逐项验证每一个指标。

Performance testing is the final step before product delivery and the most valued by engineers. A complete test report not only proves your work meets design specifications but also reflects rigorous engineering attitude. As a signal generator, WavePocket's core metrics include frequency accuracy, amplitude flatness, and waveform quality (THD). Today we verify each metric like real product test engineers.

### 任务 12.1: 频率精度测试 (30 分钟)

**测试原理：** DDS 输出频率精度取决于系统时钟精度。WavePocket 使用 30MHz 时钟，理论上频率精度等于时钟源精度 (通常 < 50ppm)。

**测试设备：** 示波器 (频率测量功能) 或频率计数器

**测试步骤 Steps:**

1. WavePocket 输出正弦波，幅度设为 2Vpp
2. 依次设置以下频率，用示波器测量实际输出频率
3. 计算误差百分比

| 序号 | 设置频率 Set Freq | 示波器实测 Measured | 误差 Error (%) | 通过 Pass? |
|------|-----------------|-------------------|--------------|-----------|
| 1 | 10 Hz | _____ Hz | _____% | |
| 2 | 100 Hz | _____ Hz | _____% | |
| 3 | 500 Hz | _____ Hz | _____% | |
| 4 | 1 kHz | _____ kHz | _____% | |
| 5 | 5 kHz | _____ kHz | _____% | |
| 6 | 10 kHz | _____ kHz | _____% | |
| 7 | 50 kHz | _____ kHz | _____% | |
| 8 | 100 kHz | _____ kHz | _____% | |
| 9 | 500 kHz | _____ kHz | _____% | |
| 10 | 1 MHz | _____ MHz | _____% | |
| 11 | 2 MHz | _____ MHz | _____% | |
| 12 | 5 MHz | _____ MHz | _____% | |

**通过标准：** 全范围误差 < 0.01% (DDS 时钟精度)

**预期结果 Expected Result:**
- 所有频点误差 < 0.01%
- 5MHz 处波形已明显失真 (接近 30MSa/s 奈奎斯特极限，WavePocket 带宽约 10MHz)

**常见问题 Common Issues:**
- "示波器频率读数跳动" -- 增加示波器的测量门限时间
- "5MHz 波形失真严重" -- 正常现象，30MSa/s 下 5MHz 只有 6 个采样点/周期，失真正常
- "低频 (10Hz) 测量不准" -- 示波器时基设置太大，调小到 10ms/div

### 任务 12.2: 幅度平坦度测试 (30 分钟)

**测试原理：** 理想波形发生器的输出幅度不随频率变化。实际中，DAC 输出阻抗、运放带宽和 PCB 寄生参数都会导致高频幅度下降。幅度平坦度衡量的是：在固定幅度设置下，不同频率的输出幅度变化量。

**测试步骤 Steps:**

1. WavePocket 输出正弦波，幅度固定为 2Vpp (1kHz 时校准)
2. 改变频率，记录示波器测量的 Vpp

| 频率 Frequency | 实测 Vpp | 相对偏差 vs 1kHz | 通过? |
|--------------|---------|-----------------|------|
| 100 Hz | _____ V | _____ dB | |
| 500 Hz | _____ V | _____ dB | |
| 1 kHz (基准) | 2.00 V | 0 dB | |
| 5 kHz | _____ V | _____ dB | |
| 10 kHz | _____ V | _____ dB | |
| 50 kHz | _____ V | _____ dB | |
| 100 kHz | _____ V | _____ dB | |
| 500 kHz | _____ V | _____ dB | |
| 1 MHz | _____ V | _____ dB | |

3. 绘制幅度 vs 频率曲线 (频率对数坐标)
4. 计算 -3dB 带宽 (幅度下降到基准的 70.7%)

**计算公式：**

```
相对偏差 (dB) = 20 * log10(实测Vpp / 基准Vpp)
-3dB 点: 实测Vpp = 基准Vpp * 0.707
```

**通过标准：** 100Hz~1MHz 范围内幅度平坦度 < 1dB (10%)

**预期结果 Expected Result:**
- 100Hz~100kHz: < 0.5dB 平坦度
- -3dB 带宽 > 5MHz

**常见问题 Common Issues:**
- "高频幅度明显下降" -- 运放模块增益带宽积限制，属正常现象
- "某些频点幅度突然下降" -- 可能有寄生谐振，检查 PCB 布线

### 任务 12.3: 波形质量测试 - THD (30 分钟)

**THD (总谐波失真) 是什么?**

THD 衡量输出波形中谐波分量占总信号的比例。纯正弦波的 THD = 0%。实际输出总会有谐波，THD 越低越好。

THD measures the ratio of harmonic components to the fundamental signal. A pure sine wave has THD = 0%. Real outputs always have harmonics; lower THD is better.

```
THD = sqrt(V2^2 + V3^2 + V4^2 + ...) / V1 * 100%

其中:
  V1 = 基波幅度 (fundamental)
  V2, V3, V4... = 二次、三次、四次谐波幅度
```

**测试方法 (使用示波器 FFT 功能)：**

1. WavePocket 输出 1kHz 正弦波，幅度 2Vpp
2. 示波器打开 FFT 模式
3. 观察频谱图，找到基波 (1kHz) 和各次谐波 (2kHz, 3kHz, 4kHz...)
4. 读取各谐波幅度，计算 THD

| 谐波 | 频率 | 幅度 (dBm 或 mV) |
|------|------|-----------------|
| 基波 (1次) | 1 kHz | _____ |
| 2 次谐波 | 2 kHz | _____ |
| 3 次谐波 | 3 kHz | _____ |
| 4 次谐波 | 4 kHz | _____ |
| 5 次谐波 | 5 kHz | _____ |

**THD 估算：**

| 测试条件 | THD (估算) | 通过标准 | 通过? |
|---------|-----------|---------|------|
| 1kHz 正弦波, 2Vpp | _____% | < 3% | |
| 10kHz 正弦波, 2Vpp | _____% | < 5% | |
| 100kHz 正弦波, 2Vpp | _____% | < 10% | |

**如果没有示波器 FFT 功能：**
- 用 PC 声卡 + 软件频谱分析仪 (如 Audacity, SpectrumLab) 代替
- 注意：声卡输入阻抗和带宽有限，适合测 20Hz~20kHz 范围

**预期结果 Expected Result:**
- 1kHz 正弦波 THD < 3%
- THD 随频率升高而增大 (正常)
- 方波的"谐波"实际上不是失真，而是方波的固有频谱成分

**常见问题 Common Issues:**
- "THD 远高于预期" -- 检查电源去耦是否充分，运放模块接线是否正常、电源是否稳定
- "FFT 图上有大量杂散" -- 电源噪声，检查 LDO 和 DCDC 输出纹波
- "方波 THD 很高" -- 方波本身就包含大量谐波，这不是失真而是正常频谱

### 任务 12.4: 扫频与 AM 调制功能测试 (30 分钟)

**扫频功能测试：**

WavePocket 支持频率扫描功能，输出频率在一定范围内自动变化。

**测试步骤：**
1. 设置扫频参数：起始 100Hz，终止 1MHz，步进时间 100ms
2. 示波器设置为长时基 (100ms/div 或更长)
3. 观察波形频率是否逐渐升高
4. 用示波器的"瀑布图"或"频谱图"模式记录扫频过程

| 扫频设置 | 示波器观察 | 通过? |
|---------|----------|------|
| 100Hz -> 10kHz, 10s | 频率渐变可观察 | |
| 1kHz -> 1MHz, 5s | 频率渐变可观察 | |
| 循环扫频 (100Hz~1MHz) | 周期性重复 | |

**AM 调制功能测试：**

如果 WavePocket 软件支持 AM (幅度调制)：

1. 设置载波 1MHz，调制频率 1kHz，调制深度 50%
2. 示波器观察包络线

| AM 设置 | 示波器观察 | 通过? |
|--------|----------|------|
| 载波 100kHz, 调制 1kHz, 深度 80% | 包络线清晰 | |
| 载波 1MHz, 调制 10kHz, 深度 50% | 包络线可见 | |

### 任务 12.5: 性能文档化 (15 分钟)

**将所有测试结果汇总为性能报告：**

```
WavePocket 性能测试报告
====================
测试人: ___________
测试日期: 2026-__-__
设备序列号: ___________

1. 频率精度: 通过 / 未通过
   全范围误差: _____% (目标 < 0.01%)

2. 幅度平坦度: 通过 / 未通过
   100Hz~1MHz 偏差: _____ dB (目标 < 1dB)
   -3dB 带宽: _____ MHz

3. THD:
   1kHz 正弦波: _____% (目标 < 3%)
   10kHz 正弦波: _____% (目标 < 5%)

4. 最大输出幅度: _____ Vpp
5. 偏置范围: +/-_____ V
6. 扫频功能: 正常 / 异常
7. AM 调制: 正常 / 异常 / 未实现

总评: ___________
```

---

## 下午: 项目展示 | Afternoon: Project Presentation

### 任务 12.6: PPT 展示准备 (30 分钟)

**PPT 结构建议 (5 分钟, 8~10 页)：**

| 页码 | 内容 Content | 时间 | 提示 Tips |
|------|-------------|------|---------|
| 1 | 封面: WavePocket 项目名称、姓名、日期 | 15s | 放一张成品照片 |
| 2 | 项目简介: WavePocket 是什么? 能做什么? | 30s | 用一句话说清楚 |
| 3 | 技术架构: iCE40 FPGA + DDS + DAC904 系统框图 | 45s | 画系统框图 |
| 4 | 硬件设计: 开发板+模块、DAC904、运放模块 | 45s | 放整机连线照片 |
| 5 | 软件设计: Verilog DDS、TFT 显示、编码器交互 | 45s | 放屏幕照片 |
| 6 | 整机接线与调试: 面包板连线、分层调试 | 30s | 放过程照片 |
| 7 | 测试结果: 频率精度、THD、幅度平坦度 | 45s | 放测试数据表格 |
| 8 | Demo 视频: 实时波形输出 (备用) | 30s | 防止现场翻车 |
| 9 | 挑战与收获: 最大的困难是什么? 学到了什么? | 30s | 讲真心话 |
| 10 | 感谢 + Q&A | 15s | 开放提问 |

**PPT 制作要点：**
- 图片为主，文字为辅 (每页不超过 5 行文字)
- 数据用图表展示 (测试数据用表格或柱状图)
- 准备一个 30 秒的演示视频作为备用 (防止现场设备故障)
- 提前演练至少 2 遍，控制在 5 分钟内

### 任务 12.7: 现场展示 (每人 8 分钟)

**展示流程：**

1. **PPT 演示 (5 分钟)**
   - 讲解 WavePocket 的技术架构和实现方案
   - 展示设计过程中的关键决策
   - 展示测试数据和性能指标
   - 分享遇到的最大挑战和解决方案

2. **实时 Demo (2 分钟)**
   - 将 WavePocket 连接到示波器
   - 演示以下功能 (按顺序)：
     - 正弦波 1kHz 输出 -> 切换到方波 -> 切换到三角波
     - 频率调节: 100Hz -> 1kHz -> 100kHz -> 1MHz
     - 幅度调节: 从小到大
     - (加分项) PC 端手绘波形 -> 加载到 WavePocket -> 示波器验证

3. **技术问答 (1 分钟)**
   - 评委可能提问的方向：
     - "为什么用 FPGA (iCE40) 而不是 STM32 做 DDS?"
     - "DDS 是怎么合成出正弦波的? 相位累加器和查找表各干什么?"
     - "DAC904 的分辨率和更新率是多少? 对输出有什么影响?"
     - "为什么高速 DAC 用并行接口而不是串行 SPI?"
     - "TFT 显示和编码器是怎么和 DDS 联动的?"
     - "你的项目总成本是多少? 最贵的元件是什么?"
     - "如果让你改进，你会增加什么功能?"
     - "这个项目和你在高中学的哪些知识有关?"

**展示技巧：**
- 保持自信，语速适中，眼神交流
- 先 Demo 能工作的功能，再讲技术原理 (先让评委"哇"，再让他们"懂")
- 如果 Demo 翻车了，不要慌 -- 讲讲你排查问题的思路
- 诚实回答：不会就说不会，不要编造

### 任务 12.8: 同学互评 (15 分钟)

为 2~3 位同学的项目打分：

| 评价维度 Dimension | 分数 (1-10) | 备注 Notes |
|-------------------|-----------|-----------|
| 功能完整性 Functionality | | 波形输出是否正常 |
| 波形质量 Waveform Quality | | 用示波器看波形是否干净 |
| 显示与交互 Display & Interaction | | TFT 显示和编码器是否好用 |
| 整机接线 Wiring | | 接线是否整齐可靠 |
| 外壳装配 Enclosure Assembly | | 整机是否紧凑美观 (可选) |
| 展示表达 Presentation | | 讲解是否清晰有逻辑 |
| 创新亮点 Innovation | | 有没有超越基本要求的功能 |

### 任务 12.9: 项目复盘 (15 分钟)

**个人反思 (写在实验日志中)：**

1. **技术收获：** 这 12 天你学到的最重要的 3 个技术知识点是什么？
   - ___________
   - ___________
   - ___________

2. **最大挑战：** 你遇到的最大技术挑战是什么？你是怎么解决的？

3. **如果重来：** 如果再做一次 WavePocket，你会在哪个环节做不同的选择？

4. **继续学习：** 你接下来想学什么？
   - 更高级的 FPGA 设计 (Verilog/VHDL)
   - 嵌入式 Linux 开发
   - 高速 PCB 设计
   - 其他：___________

5. **与高中知识的联系：** 这个项目和你高中学的哪些知识有关？
   - 物理: ___________
   - 数学: ___________
   - 信息技术: ___________

---

## 课程回顾 | Course Recap

### 12 天学习之旅 | 12-Day Learning Journey

```
Phase 1 (Day 1-4): 基础搭建
  Day 1: 项目启动       -- 认识 iCE40 FPGA 与信号发生器，搭建开源工具链
  Day 2: 工具链与 Verilog -- Yosys/nextpnr/IceStorm 跑通，Verilog 入门
  Day 3: 数字逻辑       -- 组合逻辑、时序逻辑、LED 闪烁、按键消抖
  Day 4: DDS 原理       -- 相位累加器、查找表、频率控制字

Phase 2 (Day 5-8): 核心功能
  Day 5: DDS 模块       -- 相位累加器 + BRAM 查找表，仿真验证
  Day 6: 多波形查找表    -- 正弦/方波/三角/锯齿 LUT 生成与切换
  Day 7: DAC904 驱动    -- 并行 DAC 接口，示波器见真实波形
  Day 8: 输出幅度调理    -- 运放模块接线，幅度/偏置可调

Phase 3 (Day 9-12): 交互与集成
  Day 9: TFT 显示驱动   -- ST7735 SPI，显示波形类型和频率
  Day 10: 编码器交互     -- EC11 正交解码、消抖、选波形/调频
  Day 11: 系统联调      -- 整机接线，分层调试，全链路跑通
  Day 12: 测试与展示     -- 性能测试，项目展示，技术答辩
```

### 你掌握的核心技能 | Core Skills Acquired

| 技能 Skill | 具体能力 Ability |
|-----------|-----------------|
| FPGA 设计 | Verilog 基础，iCE40 开源工具链 (Yosys/nextpnr/IceStorm)，DDS 算法 |
| DDS 原理 | 相位累加器、查找表、频率控制字、频率分辨率 |
| 高速 DAC | DAC904 14-bit 并行驱动，运放模块输出调理 |
| 显示交互 | ST7735 SPI TFT 驱动，EC11 旋转编码器正交解码 |
| 模拟电路 | 运放模块调理，幅度/偏置控制，重建滤波 |
| 系统调试 | 电源轨验证，分层调试，示波器验证波形 |
| 项目管理 | Git 版本控制，技术文档，测试报告 |

### 进一步学习建议 | Next Steps

1. **增加输出保护：** 设计一个简单的输出保护电路 (保险丝 + 钳位二极管)
2. **提高 DAC 分辨率：** 升级到 16-bit DAC，改善波形质量
3. **增加频率范围：** 换更高速的 DAC 模块提高输出带宽
4. **USB 波形传输：** 开发 PC 端软件，支持从 PC 下载任意波形到 FPGA
5. **自制 PCB：** 有余力的同学参考开源 PCB 自己打板，把模块换成贴片元件
6. **添加逻辑分析仪功能：** 利用 FPGA 的并行特性，同时做 AWG + 逻辑分析仪

---

## 恭喜完成！ | Congratulations!

你已经完成了 WavePocket -- 口袋信号发生器的全部课程！

You have completed the entire WavePocket -- Pocket Signal Generator course!

12 天前，你可能连 FPGA 是什么都不知道。现在，你拥有了一台自己接线、调试、跑通的信号发生器。它基于 iCE40 FPGA，能输出正弦波、方波、三角波、锯齿波，带 TFT 屏幕显示和编码器调参，示波器上能看到干净的真实波形。这台设备的成本<!-- 只有约 178 元 -->很低，但包含了专业 AWG 的全部核心原理。

12 days ago, you might not have known what an FPGA was. Now you own a signal generator that you wired, debugged, and brought to life yourself. Based on an iCE40 FPGA, it outputs sine, square, triangle, and sawtooth waves, with a TFT display and encoder tuning, and you can see clean real waveforms on the oscilloscope. This device <!-- costs only about 178 yuan --> is very low cost but contains all core principles of professional AWGs.

**这就是工程的魅力所在。**

**This is the beauty of engineering.**

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| TinyAWG 开源项目 (OSHWhub) | 开源硬件 | https://oshwhub.com/greentor/tinyawg-signal-source |
| TinyAWG 演示视频 | B站 | https://www.bilibili.com/video/BV1a42QBUECC |
| iCE40 开源工具链 | 在线文档 | github.com/YosysHQ/icestorm |
| projectf.io FPGA 教程 | 在线教程 | projectf.io/posts/fpga-verilog-intro/ |
| DAC904 14-bit DAC | PDF | 搜索"DAC904 datasheet TI" |
| THD 测量方法 | B站搜索 | 搜索"THD 谐波失真 测量 示波器" |

---

*最后更新：2026-06-23*
*Last updated：2026-06-23*
