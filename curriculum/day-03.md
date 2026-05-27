# Day 03: FPGA 基础与 Verilog 入门 | FPGA Basics & Verilog Introduction

> **今日目标 Today's Objectives:**
> - 理解 FPGA、ASIC 和 MCU 的区别与各自的应用场景
> - 掌握 Verilog HDL 的基本语法：模块、always 块、寄存器
> - 区分组合逻辑和时序逻辑
> - 设计一个 PWM 模块，在 Vivado 仿真器中验证，并下载到 ZYNQ 观察效果
>
> **产出 Deliverable:** Verilog PWM 模块仿真通过，下载到 PL 侧后 LED 亮度可控

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:15 | 晨会 Morning Meeting | 昨日回顾、今日目标 |
| 09:15-10:15 | 知识讲解 Lecture | FPGA vs ASIC vs MCU + Verilog 基础语法 |
| 10:15-10:30 | 休息 Break | Break |
| 10:30-12:00 | 动手实验 Hands-on | 组合逻辑与时序逻辑练习 |
| 12:00-13:30 | 午餐 Lunch | Lunch + Break |
| 13:30-15:00 | 动手实验 Hands-on | PWM 模块 Verilog 编写 |
| 15:00-15:15 | 休息 Break | Break |
| 15:15-16:30 | 动手实验 Hands-on | Vivado 仿真 + 下载到 ZYNQ 验证 |
| 16:30-17:00 | 回顾 Review | 今日总结 + 答疑 |

---

## 上午: FPGA 基础与 Verilog 语法 | Morning: FPGA Fundamentals & Verilog Syntax

### 为什么要学这个? | Why Learn This?

昨天我们用 Vivado 的 Block Design（图形化积木）搭建了 ZYNQ 系统，但这只是 FPGA 开发的冰山一角。真正的 FPGA 设计使用 **Verilog 硬件描述语言** 来编写。Verilog 不是软件编程语言——它描述的是**硬件电路**。一行 Verilog 代码可能对应一个门电路、一个触发器、甚至一个完整的计算单元。掌握 Verilog 是你从"使用者"变成"设计者"的关键一步。

Yesterday we built a ZYNQ system using Vivado Block Design (graphical blocks), but that's just the tip of the FPGA development iceberg. Real FPGA design uses **Verilog Hardware Description Language**. Verilog is not a software programming language — it describes **hardware circuits**. One line of Verilog code may correspond to a gate, a flip-flop, or even a complete computation unit. Mastering Verilog is the key step from "user" to "designer."

### 任务 3.1: FPGA vs ASIC vs MCU (30 分钟)

**三种芯片的对比：**

| 特性 Feature | MCU (单片机) | FPGA (现场可编程门阵列) | ASIC (专用集成电路) |
|-------------|-------------|----------------------|-------------------|
| 工作方式 | 逐条执行指令 | 并行执行硬件逻辑 | 固定硬件功能 |
| 灵活性 | 通过软件修改 | 通过重新编程修改 | 不可修改 |
| 速度 | 受时钟频率限制 | 可达纳秒级响应 | 最快 |
| 功耗 | 低 | 中等 | 最低（同等功能） |
| 开发难度 | ★★☆ | ★★★☆ | ★★★★★ |
| 成本 | 很低 | 中等 | 极高（需流片） |
| 典型应用 | Arduino, STM32 | TinyAWG, 通信设备 | 手机芯片, GPU |

**为什么 TinyAWG 选 FPGA？**

TinyAWG 需要 200MSa/s 的精确时序——每 5 纳秒就要输出一个新的 DAC 采样值。MCU 执行一条指令需要几十纳秒，根本来不及。而 FPGA 的硬件逻辑可以在每个时钟周期精确地完成所有操作，不受指令执行延迟的影响。这就是 FPGA 在高速信号处理中不可替代的原因。

TinyAWG needs 200MSa/s precise timing — a new DAC sample every 5 nanoseconds. An MCU takes tens of nanoseconds per instruction, which is too slow. FPGA hardware logic can precisely complete all operations every clock cycle, unaffected by instruction execution latency. This is why FPGAs are irreplaceable in high-speed signal processing.

**步骤 Steps:**
1. 阅读上面的对比表，思考每种芯片的优劣势
2. 回答问题：为什么不用 STM32 来做 TinyAWG 的 DDS？
3. 回答问题：手机处理器是 ASIC 还是 FPGA？为什么？

**预期结果 Expected Result:**
能解释三种芯片的本质区别，能说明 TinyAWG 为什么选择 FPGA (ZYNQ PL)。

### 任务 3.2: Verilog 基本语法 (45 分钟)

**Verilog 的核心概念：**

Verilog 与 C 语言有一些表面上的相似（比如 `if-else`, `case`），但本质完全不同：
- C 语言是**顺序执行**的：一行代码接着一行执行
- Verilog 是**并发执行**的：所有 `always` 块同时运行

```
C 语言思维：                Verilog 思维：
                            ┌──────────────┐
步骤1 → 步骤2 → 步骤3       │ always 块 1  │──→ 输出 A
  ↓                         │ (同时运行)    │
步骤4 → 步骤5               ├──────────────┤
                            │ always 块 2  │──→ 输出 B
                            │ (同时运行)    │
                            ├──────────────┤
                            │ always 块 3  │──→ 输出 C
                            │ (同时运行)    │
                            └──────────────┘
```

**基本模块结构：**

```verilog
// Verilog 模块 = 一个电路模块
// 就像 C 语言的函数，但描述的是硬件

module led_blink(          // module 模块名(
    input  wire clk,       //   输入：时钟信号（连线类型 wire）
    input  wire rst_n,     //   输入：复位信号（低电平有效，_n 表示负逻辑）
    output reg  led        //   输出：LED 控制信号（寄存器类型 reg）
);                         // );

// wire = 连线，只能被连续赋值（组合逻辑）
// reg  = 寄存器，可以在 always 块中赋值（时序/组合逻辑）

// 参数定义（类似 C 语言的 #define 或 const）
parameter COUNTER_MAX = 25_000_000;  // 100MHz 下 0.25 秒计数值

// 内部信号
reg [24:0] counter;  // 25 位计数器，可以数到 33,554,431

// always 块：时序逻辑（带时钟沿触发）
always @(posedge clk or negedge rst_n) begin
    // posedge clk  = 时钟上升沿触发
    // negedge rst_n = 复位信号下降沿触发
    if (!rst_n) begin
        // 复位状态：所有寄存器归零
        counter <= 0;      // <= 是非阻塞赋值（时序逻辑用）
        led     <= 0;
    end
    else begin
        if (counter >= COUNTER_MAX - 1) begin
            counter <= 0;
            led     <= ~led;   // ~ 是取反，LED 翻转
        end
        else begin
            counter <= counter + 1;
        end
    end
end

endmodule  // 模块结束
```

**关键语法对比：**

| 概念 | C 语言 | Verilog |
|------|--------|---------|
| 赋值 | `a = 5;` | 阻塞赋值 `a = 5;`（组合逻辑）或非阻塞赋值 `a <= 5;`（时序逻辑） |
| 条件 | `if (a > 0)` | 完全相同 `if (a > 0)` |
| 位宽 | 不需要声明 | `reg [7:0] data;` 表示 8 位寄存器 |
| 注释 | `//` 和 `/* */` | 完全相同 |
| 进制 | `255` | `8'd255` 或 `8'b11111111` 或 `8'hFF` |
| 取反 | `!a` (逻辑) | `~a` (按位) 或 `!a` (逻辑) |
| 拼接 | 不适用 | `{a, b}` 将 a 和 b 拼接 |

**数值表示法：**

```
8'd255  = 8位十进制 255
8'b11111111 = 8位二进制 11111111
8'hFF   = 8位十六进制 FF
8'o377  = 8位八进制 377

// 以上四个值相等！
// 格式：[位宽]'[进制][数值]
// d=decimal, b=binary, h=hex, o=octal
```

**步骤 Steps:**
1. 仔细阅读上面的代码和注释
2. 在纸上回答：
   - `reg [24:0] counter` 能表示的最大值是多少？（提示：2^25 - 1）
   - 如果时钟是 100MHz，`COUNTER_MAX = 25_000_000` 时 LED 闪烁频率是多少？
3. 尝试修改 `COUNTER_MAX` 使 LED 以 2Hz 频率闪烁

**预期结果 Expected Result:**
理解 module、wire、reg、always、posedge 的含义。能手动计算计数器值和闪烁频率的关系。

**常见问题 Common Issues:**
- "wire 和 reg 有什么区别？" -- wire 是连线（组合逻辑），reg 是寄存器（可以存状态）。在 always 块中赋值的信号必须声明为 reg
- "`<=` 和 `=` 有什么区别？" -- `<=` 是非阻塞赋值（时序逻辑推荐），`=` 是阻塞赋值（组合逻辑使用）。初学时在 `always @(posedge clk)` 中统一用 `<=`

### 任务 3.3: 组合逻辑与时序逻辑 (45 分钟)

**两种逻辑的区别：**

这是 Verilog 中最重要的概念之一：

| 特性 | 组合逻辑 Combinational | 时序逻辑 Sequential |
|------|----------------------|-------------------|
| 是否需要时钟 | 不需要 | 需要 |
| 是否有记忆 | 没有（输出只取决于当前输入） | 有（输出取决于输入和当前状态） |
| always 触发方式 | `always @(*)` 或 `always @(a or b)` | `always @(posedge clk)` |
| 赋值方式 | 阻塞赋值 `=` | 非阻塞赋值 `<=` |
| 典型电路 | 与或非门、多路选择器、加法器 | 计数器、移位寄存器、状态机 |
| Verilog 关键字 | `assign`, `always @(*)` | `always @(posedge clk)` |

**组合逻辑示例——多路选择器 (MUX)：**

```verilog
module mux_2to1(
    input  wire [7:0] data_a,   // 输入 A（8位）
    input  wire [7:0] data_b,   // 输入 B（8位）
    input  wire       sel,      // 选择信号：0 选 A，1 选 B
    output wire [7:0] data_out  // 输出
);

// 方法1：用 assign（连续赋值，最简洁）
assign data_out = sel ? data_b : data_a;
// sel=1 选 data_b，sel=0 选 data_a

// 方法2：用 always @(*)（组合逻辑 always 块）
// 注意：output 必须改成 reg 类型才能在 always 中赋值
// reg [7:0] data_out;
// always @(*) begin
//     if (sel)
//         data_out = data_b;    // 阻塞赋值 =
//     else
//         data_out = data_a;
// end

endmodule
```

**时序逻辑示例——计数器：**

```verilog
module counter(
    input  wire        clk,      // 时钟
    input  wire        rst_n,    // 异步复位（低有效）
    input  wire        enable,   // 计数使能
    output reg  [3:0]  count     // 4 位计数值（0~15）
);

always @(posedge clk or negedge rst_n) begin
    if (!rst_n)
        count <= 4'd0;           // 复位时清零
    else if (enable)
        count <= count + 4'd1;   // 使能时加1（自动溢出回绕）
    // else 保持不变
end

endmodule
```

**步骤 Steps:**

1. 在 Vivado 中创建新的 Verilog 源文件：
   - Flow Navigator → `PROJECT MANAGER` → `Add Sources` → `Add or create design sources`
   - 点击 `Create File`，文件名：`mux_2to1.v`
   - 将上面的多路选择器代码粘贴进去

2. 创建计数器源文件 `counter.v`，粘贴计数器代码

3. **编写测试激励 (Testbench)：**

```verilog
// 文件名：tb_counter.v
// 测试激励 = 给模块输入信号，观察输出是否正确

`timescale 1ns / 1ps  // 时间单位 1ns，精度 1ps

module tb_counter();

// 输入信号（用 reg 驱动）
reg        clk;
reg        rst_n;
reg        enable;

// 输出信号（用 wire 接收）
wire [3:0] count;

// 实例化被测模块（就像在面包板上插一个芯片）
counter uut (
    .clk(clk),
    .rst_n(rst_n),
    .enable(enable),
    .count(count)
);

// 生成时钟信号：10ns 周期 = 100MHz
initial begin
    clk = 0;
    forever #5 clk = ~clk;  // 每 5ns 翻转一次，周期 10ns
end

// 测试流程
initial begin
    // 初始化
    rst_n  = 0;
    enable = 0;

    // 等待 100ns 后释放复位
    #100;
    rst_n = 1;
    $display("复位释放，开始计数");

    // 使能计数
    #50;
    enable = 1;

    // 观察计数过程
    #500;

    // 停止计数
    enable = 0;
    $display("计数停止，当前值 = %d", count);

    // 继续观察一段时间
    #200;

    $display("测试结束");
    $finish;  // 结束仿真
end

endmodule
```

4. 在 Vivado 中添加测试激励文件：
   - `Add Sources` → `Add or create simulation sources` → `Create File` → `tb_counter.v`

**预期结果 Expected Result:**
理解组合逻辑和时序逻辑的本质区别。知道什么时候用 `assign`/`always @(*)`，什么时候用 `always @(posedge clk)`。

**常见问题 Common Issues:**
- "`always @(*)` 里的 `*` 是什么意思？" -- 表示"所有输入信号"，任何一个输入变化都会触发。等价于把所有输入都列出来
- "为什么时序逻辑用 `<=` 不用 `=`？" -- 非阻塞赋值保证所有寄存器同时更新，避免仿真和综合结果不一致

---

## 下午: PWM 模块设计与验证 | Afternoon: PWM Module Design & Verification

### 任务 3.4: 设计 PWM 模块 (60 分钟)

**什么是 PWM？**

PWM (Pulse Width Modulation，脉宽调制) 通过改变高电平在一个周期中的占比（占空比）来控制平均电压。在我们的项目中，PWM 的原理和 DDS 的原理非常相似——都是用计数器和比较器来实现精确的数字控制。

PWM (Pulse Width Modulation) controls average voltage by changing the duty cycle — the proportion of high-level time within one period. In our project, PWM principles are very similar to DDS — both use counters and comparators for precise digital control.

```
占空比 50%:          占空比 25%:          占空比 75%:
┌──┐  ┌──┐          ┌─┐    ┌─┐          ┌───┐ ┌───┐
│  │  │  │          │ │    │ │          │   │ │   │
│  │  │  │          │ │    │ │          │   │ │   │
┘  └──┘  └          ┘ └────┘ └          ┘   └─┘   └

LED 亮度：中等       LED 亮度：暗         LED 亮度：亮
平均电压：1.65V      平均电压：0.825V     平均电压：2.475V
```

**PWM 工作原理：**

```
计数器值:  0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15  0  1  2 ...
           ┌──────────────────┐                              ┌────
比较值=10: │██████████████████│  (计数器 < 10 → 高电平)        │
           │                  └──────────────────────────────┘
           ┌──────────────────────────────────────────────────┐
比较值=5:  │█████│            (计数器 < 5 → 高电平)             │
           │     └────────────────────────────────────────────┘
```

**Verilog PWM 模块代码：**

```verilog
// 文件名：pwm.v
// 功能：PWM 调光模块
// 输入 clk = 100MHz 时，PWM 频率 ≈ 100MHz/256/256 ≈ 1.5kHz

module pwm(
    input  wire        clk,         // 系统时钟 (100MHz)
    input  wire        rst_n,       // 异步复位，低有效
    input  wire [7:0]  duty_cycle,  // 占空比值 (0=0%, 255=100%)
    output reg         pwm_out      // PWM 输出
);

// PWM 分辨率 8 位 (256 级)
// 用两个计数器：一个慢计数器控制 PWM 频率，一个控制比较

reg [7:0] pwm_counter;   // PWM 周期计数器 (0~255)

// PWM 计数器：每个时钟周期加 1，到 255 后回 0
always @(posedge clk or negedge rst_n) begin
    if (!rst_n)
        pwm_counter <= 8'd0;
    else
        pwm_counter <= pwm_counter + 8'd1;  // 自动溢出回绕 (255→0)
end

// PWM 比较输出：计数器 < 占空比值时输出高电平
always @(posedge clk or negedge rst_n) begin
    if (!rst_n)
        pwm_out <= 1'b0;
    else if (pwm_counter < duty_cycle)
        pwm_out <= 1'b1;    // 高电平
    else
        pwm_out <= 1'b0;    // 低电平
end

endmodule
```

**PWM 频率计算：**

```
PWM 周期 = 256 个时钟周期
时钟频率 = 100MHz
PWM 频率 = 100MHz / 256 = 390.625 kHz

占空比 = duty_cycle / 256
  duty_cycle = 0   → 0%   → LED 灭
  duty_cycle = 64  → 25%  → LED 暗
  duty_cycle = 128 → 50%  → LED 中等亮度
  duty_cycle = 192 → 75%  → LED 亮
  duty_cycle = 255 → ~100% → LED 最亮
```

**步骤 Steps:**

1. 在 Vivado 工程中创建 `pwm.v` 文件，粘贴上面的代码
2. 仔细阅读代码中的注释，确保理解每一行

**预期结果 Expected Result:**
- PWM Verilog 文件创建完成
- 理解计数器和比较器如何配合产生 PWM 信号

**常见问题 Common Issues:**
- "为什么 255 不是 100%？" -- 因为计数器范围是 0~255（共 256 个值），当 duty_cycle=255 时，计数器 < 255 的时间是 255/256 ≈ 99.6%
- "PWM 频率太高人眼看不见闪烁吗？" -- 390kHz 远超人眼可感知频率，LED 看起来是稳定的亮度变化

### 任务 3.5: Vivado 仿真验证 (45 分钟)

**为什么要仿真？**

在实际下载到 FPGA 之前，先用仿真器验证逻辑是否正确，就像写完代码先在电脑上跑测试一样。仿真可以让你看到每一个时钟周期的信号变化，是 FPGA 调试的第一手段。

Before downloading to the FPGA, use the simulator to verify logic correctness, just like running tests on your computer after writing code. Simulation lets you see signal changes at every clock cycle — it's the primary debugging method for FPGA design.

**步骤 Steps:**

1. **创建仿真激励文件：**

```verilog
// 文件名：tb_pwm.v
`timescale 1ns / 1ps

module tb_pwm();

reg        clk;
reg        rst_n;
reg  [7:0] duty_cycle;
wire       pwm_out;

// 实例化 PWM 模块
pwm uut (
    .clk(clk),
    .rst_n(rst_n),
    .duty_cycle(duty_cycle),
    .pwm_out(pwm_out)
);

// 100MHz 时钟
initial begin
    clk = 0;
    forever #5 clk = ~clk;
end

// 测试流程
initial begin
    // 初始化
    rst_n = 0;
    duty_cycle = 8'd0;

    // 复位
    #100;
    rst_n = 1;
    $display("时间 %0t: 复位释放", $time);

    // 测试 25% 占空比
    duty_cycle = 8'd64;   // 25%
    #2000;
    $display("时间 %0t: 占空比 25%%", $time);

    // 测试 50% 占空比
    duty_cycle = 8'd128;  // 50%
    #2000;
    $display("时间 %0t: 占空比 50%%", $time);

    // 测试 75% 占空比
    duty_cycle = 8'd192;  // 75%
    #2000;
    $display("时间 %0t: 占空比 75%%", $time);

    // 测试 100% 占空比
    duty_cycle = 8'd255;  // ~100%
    #2000;
    $display("时间 %0t: 占空比 ~100%%", $time);

    $display("测试结束");
    $finish;
end

endmodule
```

2. **运行仿真：**
   - 在 Sources 面板，将 `tb_pwm` 设置为顶层仿真模块
     - 右键 `tb_pwm.v` → `Set as Top`
   - 在 Flow Navigator 中，点击 `SIMULATION` → `Run Simulation` → `Run Behavioral Simulation`
   - 等待仿真窗口打开（可能需要 1-2 分钟）

3. **观察波形：**
   - 在波形窗口中，你应该看到：
     - `clk` 信号：方波，周期 10ns
     - `pwm_counter`：0→1→2→...→255→0（锯齿波）
     - `pwm_out`：随 duty_cycle 变化而改变占空比
   - 使用放大镜工具观察不同占空比的 PWM 输出
   - 添加信号到波形窗口：右键信号 → `Add to Wave Window`

4. **验证正确性：**
   - 放大到能看清一个完整 PWM 周期（256 个时钟周期 = 2560ns）
   - 数一下高电平的时钟周期数，验证是否等于 duty_cycle 的值

**预期结果 Expected Result:**
- 仿真波形中能看到 PWM 信号随占空比变化而改变
- 25% 占空比时，高电平约占 1/4 周期
- 50% 占空比时，高电平约占 1/2 周期

**常见问题 Common Issues:**
- "仿真窗口打开但看不到波形" -- 点击"Zoom Fit"按钮（适配全部波形），或手动放大
- "波形是一条直线" -- 检查 rst_n 是否已释放（从 0 变为 1），检查 clk 是否在翻转
- "Run Simulation 选项是灰色的" -- 确认已添加仿真源文件并设为 Top

### 任务 3.6: 下载到 ZYNQ 观察 LED 亮度变化 (45 分钟)

**将 PWM 集成到 ZYNQ 工程：**

现在我们将 PWM 模块添加到昨天的 Block Design 工程中，让它驱动 LED。

**步骤 Steps:**

1. **修改 Block Design，添加 PWM 模块连接：**
   - 在 Block Design 中，我们需要在 PL 侧加入 PWM 模块
   - 方法：修改 HDL Wrapper，在顶层模块中实例化 PWM

2. **或者，创建独立的 PL-only 工程（推荐新手）：**
   - 创建新的 Vivado 工程（不用 Block Design）
   - 只使用 Verilog + 约束文件
   - 这样更直接地理解 PL 侧的工作方式

3. **创建顶层模块 `top.v`：**

```verilog
// 文件名：top.v
// TinyAWG PWM LED 控制顶层模块

module top(
    input  wire clk,       // 100MHz 时钟（来自 ZYNQ PS 的 FCLK_CLK0 或外部晶振）
    input  wire rst_n,     // 按键复位（低有效）
    output wire led0,      // PWM LED 0
    output wire led1,      // PWM LED 1
    output wire led2,      // PWM LED 2
    output wire led3       // PWM LED 3
);

// 慢速计数器：用于自动改变占空比，产生呼吸灯效果
reg [23:0] breath_counter;  // 24位计数器
reg [7:0]  duty;             // 当前占空比

always @(posedge clk or negedge rst_n) begin
    if (!rst_n)
        breath_counter <= 24'd0;
    else
        breath_counter <= breath_counter + 24'd1;
end

// 从慢速计数器中取高 8 位作为 PWM 占空比
// 这会产生一个 0→255→0→255 的三角波变化
wire [7:0] breath_value = breath_counter[23:16];

// 简单的呼吸灯：占空比线性增加再减少
// 使用计数器的特定位产生三角波
wire direction = breath_counter[23];  // 方向位

// 三角波发生器
reg [7:0] triangle;
always @(*) begin
    if (direction)
        triangle = 8'd255 - breath_value;  // 下降
    else
        triangle = breath_value;            // 上升
end

// 实例化 PWM 模块（4 个 LED 不同亮度）
pwm pwm_led0(
    .clk(clk),
    .rst_n(rst_n),
    .duty_cycle(triangle),     // 呼吸灯
    .pwm_out(led0)
);

pwm pwm_led1(
    .clk(clk),
    .rst_n(rst_n),
    .duty_cycle(triangle),     // 同步呼吸
    .pwm_out(led1)
);

pwm pwm_led2(
    .clk(clk),
    .rst_n(rst_n),
    .duty_cycle(8'd128),       // 固定 50% 亮度
    .pwm_out(led2)
);

pwm pwm_led3(
    .clk(clk),
    .rst_n(rst_n),
    .duty_cycle(8'd64),        // 固定 25% 亮度
    .pwm_out(led3)
);

endmodule
```

4. **创建约束文件（约束 PWM 输出到 LED 引脚）：**

```tcl
## TinyAWG PWM LED 约束

# 时钟引脚（根据核心板确定，可能是外部晶振或 PS 提供的时钟）
# 如果使用外部晶振：
# set_property PACKAGE_PIN H16 [get_ports clk]
# set_property IOSTANDARD LVCMOS33 [get_ports clk]

# 复位按键引脚
set_property PACKAGE_PIN G15 [get_ports rst_n]
set_property IOSTANDARD LVCMOS33 [get_ports rst_n]

# LED 引脚
set_property PACKAGE_PIN H15 [get_ports led0]
set_property IOSTANDARD LVCMOS33 [get_ports led0]

set_property PACKAGE_PIN G15 [get_ports led1]
set_property IOSTANDARD LVCMOS33 [get_ports led1]

set_property PACKAGE_PIN M14 [get_ports led2]
set_property IOSTANDARD LVCMOS33 [get_ports led2]

set_property PACKAGE_PIN L14 [get_ports led3]
set_property IOSTANDARD LVCMOS33 [get_ports led3]
```

5. **综合 → 实现 → 生成比特流（参考 Day 02 步骤）**

6. **下载到 ZYNQ，观察 LED：**
   - LED0 和 LED1 应该呈现"呼吸灯"效果（亮度缓慢变亮再变暗，循环往复）
   - LED2 保持中等亮度（50% 占空比）
   - LED3 保持较暗亮度（25% 占空比）

**预期结果 Expected Result:**
- LED0/LED1 呈呼吸灯效果
- LED2 稳定中等亮度
- LED3 稳定较暗亮度
- 理解 PWM 如何通过占空比控制 LED 亮度

**常见问题 Common Issues:**
- "LED 一直亮或一直灭，没有呼吸效果" -- 检查 breath_counter 的位宽是否足够。24 位在 100MHz 下约 167ms 一个周期，肉眼可见
- "LED 闪烁而不是渐变" -- PWM 频率太低。确认 clk 是 100MHz。如果 clk 更低，需要减小计数器位宽
- "综合报错：multi-driven net" -- 确认同一个 LED 引脚没有被多次驱动。检查约束文件中是否有重复引脚
- "所有 LED 亮度一样" -- 检查 PWM 实例化是否正确连接了不同的 duty_cycle 值

---

## 今日作业 | Homework

1. **代码阅读题：** 阅读 PWM 模块代码，回答：
   - (a) 如果要将 PWM 分辨率从 8 位改为 10 位，需要修改哪些地方？
   - (b) PWM 频率会变成多少？
2. **编程题：** 修改呼吸灯模块，让 4 个 LED 以不同的相位呼吸（LED0 最先亮，LED1 延迟 1/4 周期，LED2 延迟 1/2 周期，LED3 延迟 3/4 周期）
3. **思考题：** PWM 的占空比控制和 DDS 的频率控制有什么相似之处？（提示：都是用计数器 + 比较器/查找表）
4. **概念题：** 解释 wire 和 reg 的区别，以及阻塞赋值 `=` 和非阻塞赋值 `<=` 的区别
5. **写实验日志：** 截图保存仿真波形，标注占空比变化的位置

---

## 明日预告 | Tomorrow's Preview

明天进入 TinyAWG 的核心——DDS（直接数字频率合成）！你将理解相位累加器如何产生精确频率的正弦波，学习 BRAM 波形查找表的原理，并设计 PS 和 PL 之间的 AXI Lite 通信接口。这是 TinyAWG 项目最关键的一天！

Tomorrow we dive into TinyAWG's core — DDS (Direct Digital Synthesis)! You will understand how the phase accumulator produces precise-frequency sine waves, learn BRAM waveform lookup table principles, and design the AXI Lite communication interface between PS and PL. This is the most critical day of the TinyAWG project!

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| Verilog 入门教程 | B站视频 | 搜索"Verilog 入门 教程 FPGA" |
| FPGA 基础知识 | B站视频 | 搜索"FPGA 入门 基础" |
| Verilog 语法参考 | 网站 | verilog.renrence.com 或搜索"Verilog cheat sheet" |
| Vivado 仿真教程 | B站视频 | 搜索"Vivado 仿真 教程" |
| PWM 原理详解 | B站视频 | 搜索"PWM 原理 LED 调光" |
| FPGA 设计实战 | 书籍 | 《FPGA设计实战》相关章节 |

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
