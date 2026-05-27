# Day 02: 运放电路与信号调理 | Op-Amp Circuits & Signal Conditioning

> **今日目标 Today's Objectives:**
> - 理解运算放大器的基本原理和常见电路拓扑
> - 在面包板上搭建电压跟随器、同相放大器、反相放大器
> - 理解示波器前端信号调理电路的设计需求
> - 完成一个偏置电路，将双极性信号转换为单极性信号
>
> **产出 Deliverable:** 面包板上的运放电路能将 0.1V 信号放大到 1V 以上，并用万用表验证

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 | 回顾 Day 1 + 今日目标 |
| 09:30-10:30 | 知识讲解 | 运放基础与信号调理原理 |
| 10:30-10:45 | 休息 | Break |
| 10:45-12:00 | 动手实验 | 电压跟随器 + 同相放大器搭建 |
| 12:00-13:30 | 午餐 | Lunch + Break |
| 13:30-15:00 | 动手实验 | 反相放大器 + 偏置电路 |
| 15:00-15:15 | 休息 | Break |
| 15:15-16:30 | 动手实验 | 示波器前端信号调理电路设计 |
| 16:30-17:00 | 回顾 | 今日总结 + 答疑 |

---

## 上午: 运算放大器基础 | Morning: Op-Amp Fundamentals

### 为什么要学这个? | Why Learn This?

示波器要测量的信号可能非常微弱（几毫伏），也可能很大（几十伏）。但我们的 ADC 只能接受 0~3.3V 的输入。怎么办？

Signals we want to measure may be very weak (a few millivolts) or very large (tens of volts), but our ADC only accepts 0~3.3V input. What do we do?

**运放就是解决这个问题的关键。** 它能：
- 放大微弱信号，让 ADC 能"看到"它
- 衰减过大信号，保护 ADC 不被烧坏
- 提供高输入阻抗，不影响被测电路
- 添加直流偏置，把正负信号变成 ADC 能接受的范围

**Op-amps are the key to solving this.** They can:
- Amplify weak signals so the ADC can "see" them
- Attenuate large signals to protect the ADC
- Provide high input impedance without affecting the measured circuit
- Add DC bias to shift bipolar signals into the ADC's range

### 任务 2.1: 运放基础概念 (30 分钟)

**什么是运放？**

运算放大器 (Operational Amplifier, 简称 Op-Amp) 是一种高增益的差分放大器。它的理想模型有几个重要特性：

- **无限大开环增益 (A → ∞):** 输出 = A × (V+ - V-)
- **无限大输入阻抗:** 输入端不汲取电流 (I+ = I- = 0)
- **零输出阻抗:** 输出能驱动任何负载
- **虚短虚断:** 在负反馈电路中，V+ ≈ V- (虚短)，I+ = I- = 0 (虚断)

**LM358 引脚图 (DIP-8 封装)：**

```
        LM358
       ┌──────┐
  OUT1 │1    8│ VCC (+)
  IN1- │2    7│ OUT2
  IN1+ │3    6│ IN2-
  GND  │4    5│ IN2+
       └──────┘
```

- Pin 8: 电源正极 (VCC)，接 +5V
- Pin 4: 电源负极 (GND)，接 地
- Pin 1: 运放1输出
- Pin 2: 运放1反相输入 (-)
- Pin 3: 运放1同相输入 (+)

**步骤 Steps:**
1. 在面包板上找到 LM358 的位置，确认引脚方向
2. 连接电源：Pin 8 → +5V，Pin 4 → GND
3. 用万用表测量 Pin 8 和 Pin 4 之间的电压，确认为 5V

**预期结果 Expected Result:**
- LM358 供电正常，测量 VCC 和 GND 之间为 5V

### 任务 2.2: 电压跟随器 (30 分钟)

**原理：**

电压跟随器是最简单的运放电路。输出直接连接到反相输入端 (Vout = Vin)。看起来没什么用？但它有一个关键作用：**阻抗变换**——高阻抗输入、低阻抗输出。

```
      ┌─────────┐
Vin ──│+        │
      │  Op-Amp ├── Vout (= Vin)
Vout──│-        │
      └─────────┘
```

**步骤 Steps:**
1. 断开电源，在面包板上搭建电压跟随器：
   - LM358 Pin 3 (IN1+) → 输入信号 (先用电位器产生 0~3.3V 可变电压)
   - LM358 Pin 2 (IN1-) → LM358 Pin 1 (OUT1) (输出反馈到反相输入)
   - LM358 Pin 1 (OUT1) → 输出端 (连接万用表)
2. 电位器中间引脚接 Pin 3，两端分别接 5V 和 GND
3. 上电，用电位器调节输入电压，用万用表测量输出电压

**预期结果 Expected Result:**
- 输出电压始终等于输入电压 (误差 < 50mV)
- 调节电位器时，输入输出同步变化

**常见问题 Common Issues:**
- "输出不是跟着输入" -- 检查反馈连线是否正确 (Pin 1 连到 Pin 2)
- "输出饱和到 5V 或 0V" -- 检查运放供电和输入范围，LM358 输入不能超过 VCC-1.5V

### 任务 2.3: 同相放大器 (45 分钟)

**原理：**

同相放大器能在保持信号相位不变的情况下放大小信号。增益由两个电阻决定：

```
             Rf
      ┌────┤电阻├────┐
      │              │
      │   ┌──────┐   │
Vin───├───│+     │   │
      │   │Op-Amp├───┴── Vout = Vin × (1 + Rf/Rg)
      └───│-     │
          └──┬───┘
             │
            Rg
             │
            GND
```

**增益公式：** G = 1 + Rf / Rg

例如：Rf = 10kΩ, Rg = 1kΩ → G = 1 + 10 = 10 倍

**步骤 Steps:**
1. 选择电阻：Rf = 10kΩ, Rg = 1kΩ
2. 搭建电路：
   - Pin 3 (IN1+) ← 输入信号 (电位器输出)
   - Pin 2 (IN1-) ← Rg 接地，Rf 接输出 (Pin 1)
3. 输入 0.2V (电位器调到 0.2V)
4. 测量输出电压

**预期结果 Expected Result:**
- 输入 0.2V → 输出约 2.0V (增益 10 倍)
- 输入 0.3V → 输出约 3.0V
- 注意：输出不能超过供电电压 (约 3.5V 对于 LM358)

**常见问题 Common Issues:**
- "增益不是 10 倍" -- 检查 Rf 和 Rg 的值是否正确，用万用表测量电阻实际值
- "输出削顶 (Clipping)" -- 输出超过运放的供电范围，降低输入电压

---

## 下午: 信号调理电路 | Afternoon: Signal Conditioning Circuit

### 任务 2.4: 反相放大器 (30 分钟)

**原理：**

反相放大器的输出与输入相位相反 (180度)。在示波器中，我们可以通过软件翻转数据来补偿。

```
        R1          Rf
Vin ──┤电阻├──┬──┤电阻├── Vout
              │
         ┌────┤
         │Op-Amp│
GND ─────│-     │
         │+     │
         └──┬───┘
            │
           GND (或偏置电压)
```

**增益公式：** G = -Rf / R1 (负号表示反相)

例如：R1 = 10kΩ, Rf = 10kΩ → G = -1 (反相跟随器)

**步骤 Steps:**
1. 选择电阻：R1 = 10kΩ, Rf = 22kΩ → 增益 = -2.2
2. 搭建电路：Pin 2 (IN1-) ← R1 接输入，Rf 接输出 (Pin 1)
3. Pin 3 (IN1+) → GND
4. 输入 0.5V，测量输出

**预期结果 Expected Result:**
- 输入 0.5V → 输出约 -1.1V
- 注意：LM358 单电源供电下无法输出负电压！需要偏置电路

### 任务 2.5: 偏置电路设计 (45 分钟)

**为什么需要偏置？**

ADC 只能测量 0~3.3V 的电压。如果信号是正弦波（有正有负），负半周会被截断。解决方案：给信号加一个直流偏置，把信号"抬高"到 ADC 的中间范围 (约 1.65V)。

The ADC can only measure 0~3.3V. If the signal is a sine wave (positive and negative), the negative half will be clipped. Solution: add a DC bias to shift the signal up to the ADC's mid-range (about 1.65V).

**偏置电路设计：**

```
5V ── R1 (10k) ──┬── R2 (10k) ── GND
                 │
                 ├── Vbias = 2.5V
                 │
              C1 (10uF)
                 │
信号输入 ────────┘──→ 到 ADC 输入
```

- R1 和 R2 分压产生 2.5V 偏置 (5V × R2/(R1+R2) = 2.5V)
- C1 是隔直电容，只让交流信号通过
- 到达 ADC 的信号 = 2.5V + 交流信号

**步骤 Steps:**
1. 用两个 10kΩ 电阻搭建分压电路，测量中点电压 (应为 2.5V)
2. 加入 10uF 电解电容作为隔直电容
3. 用信号发生器输入 1kHz, 1Vpp 正弦波
4. 用万用表测量偏置后的信号中心电压

**预期结果 Expected Result:**
- 分压中点电压约 2.5V
- 经过偏置后，正弦波中心从 0V 偏移到约 2.5V
- 信号幅度保持不变 (1Vpp)

**常见问题 Common Issues:**
- "偏置电压不稳定" -- 在分压中点并联一个 100nF 去耦电容
- "电解电容接反了" -- 电解电容有极性，长脚接正极、短脚接负极

### 任务 2.6: 完整信号调理电路设计 (45 分钟)

**示波器前端信号调理电路的三个功能：**

1. **输入保护：** 防止过大电压损坏 ADC
2. **阻抗匹配：** 高输入阻抗（不影响被测电路）
3. **幅度调整 + 偏置：** 信号调整到 0~3.3V 范围

**完整电路（简化版）：**

```
输入信号 → [1M输入电阻] → [电压跟随器] → [偏置电路] → [1k限流电阻] → ADC (PA0)
                              ↑ LM358                    ↑ 1.65V 偏置
                          (高阻抗输入)                (信号抬升)
```

**步骤 Steps:**
1. 按照 `hardware/wiring-guide.md` 中的运放接线图搭建完整电路
2. 输入信号：用信号发生器输出 1kHz, 2Vpp 正弦波
3. 测量各节点电压：
   - 输入端信号
   - 电压跟随器输出
   - 偏置后信号
4. 将最终输出连接到 PA0 (STM32 ADC 输入)

**预期结果 Expected Result:**
- 信号从 ±1V 调整为 1.65V ± 1V (即 0.65V ~ 2.65V)
- 全部在 ADC 的 0~3.3V 范围内
- 信号不失真

---

## 今日作业 | Homework

1. **计算题：** 如果 Rf = 47kΩ, Rg = 10kΩ，同相放大器的增益是多少？
   (答案：G = 1 + 47/10 = 5.7)
2. **画图题：** 在实验日志中画出你搭建的完整信号调理电路原理图，标注所有元件值
3. **思考题：** 为什么示波器的输入阻抗要很高 (1MΩ 或更高)？如果输入阻抗只有 1kΩ 会怎样？
4. **实验记录：** 记录今天搭建的每个电路的输入输出测量值，制作一个表格

---

## 明日预告 | Tomorrow's Preview

明天我们将学习 ADC 采样原理和 DMA 数据搬运——这是示波器的核心"大脑"！你将理解 STM32 如何把模拟信号变成数字数据，以及为什么 DMA 能让采样速度提升数倍。

Tomorrow we will learn ADC sampling principles and DMA data transfer -- the core "brain" of the oscilloscope! You will understand how STM32 converts analog signals to digital data, and why DMA can boost sampling speed significantly.

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| 运放电路入门 | B站搜索 | 搜索"运放电路 入门 教程" |
| 《运放电路 信号调理》 | B站搜索 | 搜索"运放信号调理电路" |
| (开源) 电路讲解 | B站视频 | https://www.bilibili.com/video/BV1NtW9zTEoK/ |
| LM358 数据手册 | PDF | 搜索"LM358 datasheet" |
| 《Op Amps for Everyone》 | 书籍 | TI 官网免费下载 |

---

*最后更新：2026-05-26*
*Last updated: 2026-05-26*
