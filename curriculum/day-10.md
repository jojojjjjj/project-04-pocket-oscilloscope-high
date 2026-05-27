# Day 10: PCB 设计与焊接 | PCB Design & Soldering

> **今日目标 Today's Objectives:**
> - 了解 PCB 设计的基本流程
> - 使用嘉立创 EDA 绘制示波器原理图
> - 完成 PCB 布局和布线
> - 下单打板，学习基本焊接技能
>
> **产出 Deliverable:** 嘉立创 EDA 原理图和 PCB 设计文件完成，打板订单提交

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 | 回顾 Day 9 + 今日目标 |
| 09:30-10:30 | 知识讲解 | PCB 设计流程、嘉立创 EDA 介绍 |
| 10:30-10:45 | 休息 | Break |
| 10:45-12:00 | 动手实验 | 原理图绘制 (Schematic) |
| 12:00-13:30 | 午餐 | Lunch + Break |
| 13:30-15:00 | 动手实验 | PCB 布局与布线 |
| 15:00-15:15 | 休息 | Break |
| 15:15-16:30 | 动手实验 | 焊接技能训练 |
| 16:30-17:00 | 回顾 | 今日总结 + 答疑 |

---

## 上午: 原理图设计 | Morning: Schematic Design

### 为什么要学这个? | Why Learn This?

到目前为止，我们的电路都在面包板上。面包板适合原型验证，但有几个问题：
- 连接不可靠（接触不良导致噪声大）
- 不能携带（一碰就散）
- 高频性能差（面包板的寄生电容影响信号质量）

PCB (Printed Circuit Board，印刷电路板) 是电子产品中电路的"骨架"。学会 PCB 设计，你就掌握了将电路从"草稿"变成"产品"的关键技能。嘉立创打板 5 片只需 5 元，成本极低！

So far, our circuit has been on a breadboard. Breadboards are great for prototyping but have issues: unreliable connections, can't be carried around, and poor high-frequency performance. PCB (Printed Circuit Board) is the "skeleton" of circuits in electronics. Learning PCB design gives you the key skill to turn circuits from "drafts" to "products." JLCPCB offers 5 PCBs for just 5 yuan!

### 任务 10.1: 嘉立创 EDA 入门 (30 分钟)

**嘉立创 EDA 两种版本：**

| 版本 | 说明 | 推荐 |
|------|------|------|
| 嘉立创 EDA 在线版 | 网页直接使用，无需安装 | 推荐新手使用 |
| 嘉立创 EDA 专业版 | 客户端安装，功能更全 | 进阶使用 |

**使用步骤：**
1. 访问 https://lceda.cn/ 并注册账号
2. 新建工程，命名为 "PocketOscilloscope"
3. 选择"新建原理图"

**基本操作：**
- 放置元件：点击左侧库，搜索元件，拖入画布
- 连线：点击引脚端点，拖动到目标引脚
- 标注：给元件添加标号 (R1, R2, C1, U1 等)
- 网络标签：相同名称的标签表示电气连接

### 任务 10.2: 绘制原理图 (90 分钟)

**示波器原理图模块划分：**

```
1. 电源模块 (Power)
   USB 5V → AMS1117-3.3 → 3.3V 系统电源
   3.3V → STM32, TFT, AD9833
   5V → LM358 运放

2. 主控模块 (MCU)
   STM32F103C8T6
   - PA0: ADC 输入
   - PA1: TFT 背光
   - PA2: TFT RST
   - PA3: TFT DC
   - PA4: TFT CS
   - PA5: TFT SCK (SPI1)
   - PA7: TFT SDA (SPI1)
   - PB12: AD9833 CS
   - PB13: AD9833 CLK (SPI2)
   - PB15: AD9833 DATA (SPI2)
   - PC13: LED
   - SWD: 调试接口

3. 信号调理模块 (Signal Conditioning)
   输入 → 1M电阻 → LM358 电压跟随器 → 偏置电路 → PA0

4. TFT 显示模块
   ST7789 1.8寸 TFT, SPI 接口

5. 信号发生器模块
   AD9833 DDS, SPI 接口, 运放放大输出
```

**步骤 Steps:**
1. 在嘉立创 EDA 中创建原理图
2. 按模块逐个绘制：
   - 先画电源模块（最重要，画错了芯片会烧）
   - 再画主控模块
   - 最后画信号调理和显示模块
3. 检查每个元件的封装 (Package) 是否正确
4. 运行 DRC (设计规则检查) 排查错误

**预期结果 Expected Result:**
- 原理图完整，包含所有 5 个模块
- DRC 无错误
- 每个元件都有正确的标号和封装

**常见问题 Common Issues:**
- "找不到某个元件" -- 嘉立创 EDA 的元件库可能没有，可以用立创商城的元件
- "封装选择错误" -- STM32F103C8T6 用 LQFP-48 封装，LM358 用 DIP-8 或 SOP-8
- "引脚连接错误" -- 仔细对照数据手册确认引脚定义

### 任务 10.3: PCB 布局与布线 (60 分钟)

**PCB 布局原则：**

1. **模拟与数字分离：** 信号调理电路远离数字信号线
2. **电源走粗线：** VCC 和 GND 走线宽度 ≥ 0.5mm，大电流路径更宽
3. **短走线：** 高速信号 (SPI) 走线尽量短
4. **去耦电容：** 每个 IC 的 VCC 引脚旁放 100nF 去耦电容
5. **GND 铺铜：** 底层大面积铺地铜，提高抗干扰能力

**PCB 尺寸建议：**
- 目标尺寸：50mm x 80mm（名片大小，口袋可放）
- 安装孔：4 个角各一个 M3 安装孔

**步骤 Steps:**
1. 从原理图导入网络表到 PCB
2. 先摆放核心元件位置：
   - STM32 居中
   - TFT 连接器在一侧
   - 输入接口在另一侧
   - AD9833 在角落
3. 自动布线（嘉立创 EDA 支持自动布线）
4. 检查关键信号走线，手动优化
5. 运行 DRC 检查
6. 铺铜（GND 铺铜）

**预期结果 Expected Result:**
- PCB 布局合理，元件紧凑
- DRC 无错误
- 所有网络已正确连接

**常见问题 Common Issues:**
- "布线不通" -- 调整元件位置，或在背面添加走线
- "DRC 错误" -- 通常是间距不够或线宽太细，按提示修复
- "不知道怎么铺铜" -- 嘉立创 EDA 有"铺铜"工具，选择 GND 网络即可

---

## 下午: 焊接训练 | Afternoon: Soldering Practice

### 任务 10.4: 焊接基础技能 (60 分钟)

**焊接安全须知：**
- 烙铁头温度约 300~400°C，不要触摸
- 焊接时保持通风，避免吸入焊锡烟雾
- 不用时将烙铁放回烙铁架
- 焊接完成后洗手

**焊接工具：**
- 电烙铁 (30W~60W)
- 焊锡丝 (0.8mm 含松香芯)
- 助焊剂 (松香或免清洗助焊剂)
- 吸锡器或吸锡线 (用于修正)
- 镊子 (夹持小元件)

**基本焊接步骤：**
1. 加热焊盘和引脚 (烙铁接触 2~3 秒)
2. 送入焊锡丝 (从另一侧)
3. 焊锡融化并流动后，先撤焊锡丝
4. 再撤烙铁
5. 理想焊点：光滑、圆润、呈锥形

**练习项目：**
1. 在洞洞板上焊接电阻 (DIP 元件)
2. 焊接 LM358 DIP-8 IC 座
3. 焊接 LED 和按键

**预期结果 Expected Result:**
- 焊点光滑、无虚焊
- 无焊锡桥接 (短路)
- 元件牢固

### 任务 10.5: 下单打板 (30 分钟)

**嘉立创 PCB 打板流程：**
1. 在嘉立创 EDA 中导出 Gerber 文件
2. 访问 https://www.jlcpcb.com/
3. 上传 Gerber 文件
4. 选择参数：
   - 层数：2 层
   - 尺寸：50mm x 80mm
   - 数量：5 片
   - 表面处理：HASL (有铅喷锡，容易焊接)
   - 颜色：绿色 (最便宜)
5. 确认价格 (约 5 元)
6. 下单支付

**预期结果 Expected Result:**
- 打板订单成功提交
- 预计 3~5 天到货

---

## 今日作业 | Homework

1. **完善设计：** 检查原理图和 PCB 设计，确保没有遗漏的连线
2. **练习题：** 在洞洞板上焊接一个完整的运放电路 (Day 2 的同相放大器)
3. **学习题：** 了解 SMD (表面贴装) 焊接的基本方法，为后续焊接 PCB 做准备
4. **思考题：** 为什么 PCB 要分"模拟地"和"数字地"？不分离会有什么问题？

---

## 明日预告 | Tomorrow's Preview

明天我们将设计 3D 打印外壳，并将所有硬件模块组装在一起，完成整机集成！你的示波器将从一堆散落的元件变成一台可以拿在手中的完整设备。

Tomorrow we design a 3D printed case and assemble all hardware modules together! Your oscilloscope will transform from scattered components into a complete handheld device.

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| 嘉立创 EDA 官方教程 | 官网 | https://lceda.cn/help |
| 嘉立创 PCB 打板教程 | B站视频 | https://www.bilibili.com/video/BV1jD421j7u6/ |
| PCB 设计 入门教程 | B站搜索 | 搜索"嘉立创EDA PCB设计 入门" |
| 焊接入门教程 | B站搜索 | 搜索"焊接入门 教程 电子" |

---

*最后更新：2026-05-26*
*Last updated: 2026-05-26*
