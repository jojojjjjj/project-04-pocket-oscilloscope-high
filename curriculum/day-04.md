# Day 04: DDS 原理与 BRAM 波形存储 | DDS Principles & BRAM Waveform Storage

> **今日目标 Today's Objectives:**
> - 深入理解 DDS（直接数字频率合成）的工作原理：相位累加器、查找表、DAC
> - 掌握频率控制字 (FCW) 的计算方法
> - 学会在 Vivado 中配置 BRAM 存储正弦波查找表
> - 设计 AXI Lite 寄存器接口实现 PS-PL 通信
> - 仿真验证完整的 DDS 输出波形
>
> **产出 Deliverable:** BRAM 存储正弦波查找表，DDS 仿真输出正弦波形，AXI Lite 寄存器可读写

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:15 | 晨会 Morning Meeting | 昨日回顾、今日目标 |
| 09:15-10:15 | 知识讲解 Lecture | DDS 原理深入：相位累加器与查找表 |
| 10:15-10:30 | 休息 Break | Break |
| 10:30-12:00 | 动手实验 Hands-on | Verilog 实现 DDS 相位累加器 + BRAM 配置 |
| 12:00-13:30 | 午餐 Lunch | Lunch + Break |
| 13:30-15:00 | 动手实验 Hands-on | AXI Lite 寄存器设计与 PS-PL 通信 |
| 15:00-15:15 | 休息 Break | Break |
| 15:15-16:30 | 动手实验 Hands-on | 完整 DDS 仿真验证 |
| 16:30-17:00 | 回顾 Review | 今日总结 + 答疑 |

---

## 上午: DDS 原理与相位累加器 | Morning: DDS Principles & Phase Accumulator

### 为什么要学这个? | Why Learn This?

DDS 是 TinyAWG 的核心——它决定了信号发生器能产生什么频率、什么形状的波形。理解 DDS 就是理解了整个 TinyAWG 的工作原理。今天学完之后，你将能够用 FPGA 产生精确的正弦波、方波、三角波，甚至任意自定义波形。

DDS is TinyAWG's core — it determines what frequency and shape of waveform the signal generator can produce. Understanding DDS means understanding how the entire TinyAWG works. After today, you will be able to use an FPGA to generate precise sine waves, square waves, triangle waves, and even custom arbitrary waveforms.

昨天的 PWM 用了计数器 + 比较器，今天的 DDS 也用计数器 + 查找表。核心思想是一脉相承的——**用数字计数的方式精确控制模拟输出**。

Yesterday's PWM used a counter + comparator; today's DDS uses a counter + lookup table. The core idea is the same lineage — **using digital counting to precisely control analog output**.

### 任务 4.1: DDS 原理深入 (45 分钟)

**DDS 完整架构：**

```
                    TinyAWG DDS 核心架构

  频率控制字 FCW        相位累加器          波形查找表         DAC 输出
  (32-bit, 由ARM写入)   (32-bit)           (BRAM, 8~14bit)    (AD9744)

  ┌──────────┐     ┌──────────────┐    ┌──────────────┐    ┌─────────┐
  │  FCW     │     │  Phase Acc   │    │  Sine LUT    │    │ AD9744  │
  │ 0x0155   │────►│  +FCW        │───►│  phase→amp   │───►│ 14-bit  │
  │ (可调)    │     │  每时钟+FCW   │    │  存在BRAM中   │    │ DAC     │
  └──────────┘     │  溢出=新周期   │    │  1024×14bit  │    └─────────┘
                   └──────────────┘    └──────────────┘         │
                        │                                        ▼
                    截取高 N 位                              模拟信号输出
                    作为查找表地址                           (经过运放滤波)
```

**DDS 工作的五个步骤：**

1. **ARM 写入 FCW**：PS 端的 C 代码通过 AXI Lite 总线将频率控制字写入 PL 端寄存器
2. **相位累加**：每个时钟周期，累加器加上 FCW 值。累加器不断增长，溢出时自动从 0 继续
3. **相位截断**：32 位累加器太长，截取高 10 位作为 BRAM 地址（1024 个查找表项）
4. **波形查找**：用截断后的地址查找 BRAM 中预存的波形数据（正弦波的幅度值）
5. **DAC 输出**：将查找结果送给 AD9744 DAC，转换为模拟电压

**频率控制字 (FCW) 详解：**

```
核心公式：
f_out = (FCW / 2^N) × f_clk

其中：
  f_out = 输出频率 (Hz)
  FCW   = 频率控制字 (整数, 0 ~ 2^N-1)
  N     = 相位累加器位宽 (TinyAWG 用 32 位)
  f_clk = 系统时钟频率 (200MHz)

频率分辨率：
  Δf = f_clk / 2^N = 200,000,000 / 4,294,967,296 ≈ 0.0466 Hz

计算 FCW 的方法：
  FCW = f_out × 2^N / f_clk
      = f_out × 4,294,967,296 / 200,000,000
      = f_out × 21.47483648

常见频率对应的 FCW：

  f_out    | FCW (十进制)   | FCW (十六进制)
  ---------|---------------|---------------
  1 Hz     | 21            | 0x00000015
  100 Hz   | 2,147         | 0x00000863
  1 kHz    | 21,475        | 0x000053E3
  10 kHz   | 214,748       | 0x000346DC
  100 kHz  | 2,147,484     | 0x0020C49B
  1 MHz    | 21,474,836    | 0x0147AE14
  10 MHz   | 214,748,365   | 0x0CCCCCCC
```

**步骤 Steps:**
1. 手动验证公式：FCW = 21,474,836 时，f_out = 21,474,836 / 2^32 × 200,000,000 ≈ 1,000,000 Hz = 1MHz
2. 计算产生 440Hz (标准 A 音) 所需的 FCW 值
3. 计算产生 35MHz (TinyAWG 最大带宽) 所需的 FCW 值

**预期结果 Expected Result:**
能熟练使用 DDS 频率公式计算任意频率对应的 FCW 值。

**常见问题 Common Issues:**
- "FCW 为什么不能超过 2^N-1？" -- 相位累加器是 N 位的，FCW 是每步增加的量。如果 FCW >= 2^N，一个时钟周期就超过一个完整周期，输出频率会高于 f_clk/2（违反奈奎斯特定理）
- "截断高 10 位不会丢失信息吗？" -- 会引入微小的相位误差（截断噪声），但对 TinyAWG 的应用来说完全可接受

### 任务 4.2: Verilog 实现相位累加器 (30 分钟)

**步骤 Steps:**

1. **创建 DDS 相位累加器模块：**

```verilog
// 文件名：dds_phase_acc.v
// 功能：DDS 相位累加器
// 每个时钟周期将相位增加 FCW，产生连续增长的相位值

module dds_phase_acc #(
    parameter PHASE_WIDTH = 32  // 相位累加器位宽（默认 32 位）
)(
    input  wire                      clk,       // 系统时钟 (200MHz)
    input  wire                      rst_n,     // 异步复位
    input  wire [PHASE_WIDTH-1:0]    fcw,       // 频率控制字 (由 ARM 写入)
    output reg  [PHASE_WIDTH-1:0]    phase,     // 当前相位值 (32 位)
    output wire [9:0]                phase_trunc// 截断后的 10 位地址 (给 BRAM)
);

// 相位累加：每个时钟周期 phase += fcw
always @(posedge clk or negedge rst_n) begin
    if (!rst_n)
        phase <= {PHASE_WIDTH{1'b0}};  // 复位时相位归零
    else
        phase <= phase + fcw;          // 累加，自然溢出回绕
end

// 截取高 10 位作为 BRAM 地址
// 为什么取高位？因为高位变化最快，对应波形的快速扫描
// 10 位地址 = 1024 个查找表项
assign phase_trunc = phase[PHASE_WIDTH-1 -: 10];
// 语法说明：phase[31 -: 10] 表示从第 31 位开始，向下取 10 位
// 等价于 phase[31:22]

endmodule
```

**Verilog 语法新知识点：**

```verilog
// 参数化设计 (parameter)
// #() 中的 parameter 允许在实例化时修改
// 例如：可以实例化 16 位或 32 位的累加器

// 位选择语法
// phase[31:22]  = 从第 31 位到第 22 位（共 10 位）
// phase[31 -: 10] = 从第 31 位开始向下取 10 位（更灵活的写法）
// phase[0 +: 10]  = 从第 0 位开始向上取 10 位（等价于 phase[9:0]）

// 复位值
// {32{1'b0}} = 32 个 0 拼接，等于 32'h00000000
```

2. **分析相位累加器的行为（手动模拟）：**

```
假设 FCW = 0x0147AE14 (产生 1MHz)，phase_width = 32，clk = 200MHz

时钟 0: phase = 0x00000000  → phase_trunc = 0x000 (地址 0)
时钟 1: phase = 0x0147AE14  → phase_trunc = 0x005 (地址 5)
时钟 2: phase = 0x028F5C28  → phase_trunc = 0x00A (地址 10)
时钟 3: phase = 0x03D70A3C  → phase_trunc = 0x00F (地址 15)
...
每 200 个时钟周期，相位累加器溢出一次（完成一个完整正弦波周期）
输出频率 = 200MHz / 200 = 1MHz ✓
```

**预期结果 Expected Result:**
理解相位累加器的工作过程，能解释为什么截取高位作为 BRAM 地址。

**常见问题 Common Issues:**
- "为什么不截取低位？" -- 低位变化太频繁，相当于在波形中跳来跳去，不会产生平滑的波形扫描
- "phase 溢出后值是多少？" -- 自然溢出。32 位无符号加法，超过 2^32-1 自动从 0 开始，Verilog 中不需要特殊处理

### 任务 4.3: BRAM 配置与正弦波查找表 (45 分钟)

**什么是 BRAM？**

BRAM (Block RAM) 是 FPGA 内部专用的存储资源。ZYNQ7010 有约 2.1Mb (约 256KB) 的 BRAM。在 DDS 中，BRAM 用于存储波形查找表（Lookup Table, LUT）——预计算好一个完整周期的正弦波采样值，运行时根据地址直接读取。

BRAM (Block RAM) is dedicated storage resources inside the FPGA. ZYNQ7010 has approximately 2.1Mb (~256KB) of BRAM. In DDS, BRAM stores the waveform Lookup Table (LUT) — pre-computed samples of one complete sine wave cycle, read directly by address at runtime.

**为什么要预计算查找表？**

计算 sin(x) 需要浮点运算，FPGA 不擅长。但查表只需要一个时钟周期！这就是"空间换时间"的经典策略。

Computing sin(x) requires floating-point operations, which FPGAs aren't good at. But looking up a table takes only one clock cycle! This is the classic "space for time" trade-off.

**步骤 Steps:**

1. **生成正弦波查找表数据：**

可以用 Python 生成查找表的 Verilog 代码：

```python
# 文件名：gen_sine_lut.py
# 运行：python gen_sine_lut.py > sine_lut.mem

import math

# 参数
TABLE_SIZE = 1024    # 查找表大小（10 位地址）
DATA_WIDTH = 14      # 数据位宽（匹配 AD9744 的 14 位 DAC）

print(f"// 正弦波查找表")
print(f"// {TABLE_SIZE} 项, {DATA_WIDTH} 位无符号")
print(f"// 一个完整正弦波周期 (0 ~ 2π)")

for i in range(TABLE_SIZE):
    angle = 2 * math.pi * i / TABLE_SIZE
    # 正弦值范围 -1 ~ +1，映射到 0 ~ 2^DATA_WIDTH-1
    sine_val = (math.sin(angle) + 1) / 2  # 0 ~ 1
    digital_val = int(sine_val * (2**DATA_WIDTH - 1) + 0.5)
    # 输出为 Verilog 可读的格式
    if DATA_WIDTH <= 8:
        print(f"{digital_val:03d}")        # 8位：十进制
    else:
        print(f"{digital_val:05d}")        # 14位：十进制
```

2. **方法一：用 Vivado Block RAM IP 配置 BRAM：**
   - 在 Block Design 中添加 `Block Memory Generator` IP
   - 配置：
     - Memory Type: `Single Port ROM`（只读存储器）
     - Port A Width: `14`（14 位数据，匹配 DAC）
     - Port A Depth: `1024`（1024 个地址，10 位地址线）
     - Operating Mode: `No Change`（减少功耗）
   - 在 `Other Options` 选项卡中：
     - 勾选 `Load Init File`
     - 选择上面生成的 `.mem` 或 `.coe` 文件
     - 这会在 FPGA 配置时自动加载查找表数据到 BRAM

3. **方法二：用 Verilog 行为级描述 BRAM（推荐学习用）：**

```verilog
// 文件名：sine_lut.v
// 功能：正弦波查找表（用 Verilog 行为级描述）
// Vivado 综合器会自动推断为 BRAM

module sine_lut #(
    parameter ADDR_WIDTH = 10,          // 地址位宽（10位 = 1024项）
    parameter DATA_WIDTH = 14           // 数据位宽（14位 = 匹配AD9744）
)(
    input  wire                      clk,
    input  wire [ADDR_WIDTH-1:0]     addr,    // 查找表地址
    output reg  [DATA_WIDTH-1:0]     data_out // 查找表输出
);

// 查找表存储器
// reg [DATA_WIDTH-1:0] memory [0:2^ADDR_WIDTH-1]
// 这是一个 1024×14 位的存储器阵列
reg [DATA_WIDTH-1:0] rom [0:1023];

// 初始化：加载正弦波数据
// 注意：实际项目中用 $readmemh 或 .coe 文件加载
// 这里用公式计算初始化（仅用于仿真）
integer i;
real angle;
real sine_val;
initial begin
    for (i = 0; i < 1024; i = i + 1) begin
        angle = 2.0 * 3.14159265 * i / 1024.0;
        sine_val = ($sin(angle) + 1.0) / 2.0;  // 映射到 0~1
        rom[i] = integer'(sine_val * (2**14 - 1));
    end
end

// 同步读取（一个时钟周期延迟）
// 这是 BRAM 的标准行为：地址在时钟沿被采样，数据下一个时钟沿输出
always @(posedge clk) begin
    data_out <= rom[addr];
end

endmodule
```

4. **理解 BRAM 的关键特性：**

```
BRAM 读取时序（同步读取）：

时钟周期:    |  T1  |  T2  |  T3  |  T4  |
地址 addr:   |  0   |  5   | 10   | 15   |
             ┌──────┬──────┬──────┬──────┐
clk          │  ↑   │  ↑   │  ↑   │  ↑   │
             └──────┴──────┴──────┴──────┘
                    ┌──────┬──────┬──────┐
data_out:    XXXXXX │sin(0)│sin(5)│sin(10)│
                    └──────┴──────┴──────┘

注意：数据延迟一个时钟周期！
在 T1 给出地址 0，在 T2 才能读到 sin(0) 的值
```

**预期结果 Expected Result:**
理解 BRAM 同步读取的延迟特性。能解释为什么查找表存储的是正弦波的幅度值。

**常见问题 Common Issues:**
- "initial 块中的 $sin 在综合时能用吗？" -- 不能。`$sin` 是仿真专用系统函数。实际项目中需要用 Python 生成 .coe 或 .mem 文件加载
- "为什么读取有一个时钟周期延迟？" -- 这是 BRAM 的物理特性，由硬件存储器的结构决定。在流水线设计中可以通过寄存器对齐来补偿

---

## 下午: AXI Lite 寄存器与 DDS 仿真 | Afternoon: AXI Lite Registers & DDS Simulation

### 任务 4.4: AXI Lite 寄存器设计 (60 分钟)

**为什么需要 AXI Lite？**

在 TinyAWG 中，ARM (PS) 需要实时控制 FPGA (PL) 的 DDS 参数：
- 设置频率控制字 (FCW)
- 选择波形类型（正弦/方波/三角/任意）
- 设置输出幅度
- 控制开关

这些参数需要从 ARM 的 C 代码"传递"到 FPGA 的 Verilog 逻辑中。AXI Lite 寄存器就是这座"桥梁"——ARM 写一个地址，FPGA 就收到一个数值。

In TinyAWG, the ARM (PS) needs to control FPGA (PL) DDS parameters in real time. AXI Lite registers serve as the "bridge" — the ARM writes to an address, and the FPGA receives the value.

**AXI Lite 寄存器映射表（TinyAWG）：**

```
AXI Lite 基地址：0x43C00000（在 Vivado Address Editor 中分配）

偏移地址    | 寄存器名        | 位宽 | 功能
-----------|----------------|------|------
0x00       | DDS_CTRL       | 32   | 控制寄存器
           |  [0]           |  1   | DDS 使能 (1=运行, 0=停止)
           |  [3:1]         |  3   | 波形选择 (000=正弦, 001=方波, 010=三角, 011=锯齿)
0x04       | DDS_FCW        | 32   | 频率控制字 (32-bit)
0x08       | DDS_PHASE_OFF  | 32   | 相位偏移 (可选，用于调频)
0x0C       | DDS_AMPLITUDE  | 16   | 幅度控制 (0~16383, 14-bit DAC 范围)
```

**简化版 AXI Lite 寄存器 Verilog 实现：**

```verilog
// 文件名：axi_lite_regs.v
// 功能：简化的 AXI Lite 寄存器接口
// ARM 通过 AXI Lite 总线读写这些寄存器来控制 DDS

module axi_lite_regs #(
    parameter C_S_AXI_DATA_WIDTH = 32,
    parameter C_S_AXI_ADDR_WIDTH = 4   // 4 位地址 = 16 字节空间 = 4 个寄存器
)(
    // AXI Lite 时钟和复位
    input  wire                          S_AXI_ACLK,
    input  wire                          S_AXI_ARESETN,

    // AXI Lite 写地址通道
    input  wire [C_S_AXI_ADDR_WIDTH-1:0] S_AXI_AWADDR,
    input  wire                          S_AXI_AWVALID,
    output wire                          S_AXI_AWREADY,

    // AXI Lite 写数据通道
    input  wire [C_S_AXI_DATA_WIDTH-1:0] S_AXI_WDATA,
    input  wire                          S_AXI_WVALID,
    output wire                          S_AXI_WREADY,

    // AXI Lite 写响应通道
    output wire [1:0]                    S_AXI_BRESP,
    output wire                          S_AXI_BVALID,
    input  wire                          S_AXI_BREADY,

    // AXI Lite 读地址通道
    input  wire [C_S_AXI_ADDR_WIDTH-1:0] S_AXI_ARADDR,
    input  wire                          S_AXI_ARVALID,
    output wire                          S_AXI_ARREADY,

    // AXI Lite 读数据通道
    output wire [C_S_AXI_DATA_WIDTH-1:0] S_AXI_RDATA,
    output wire [1:0]                    S_AXI_RRESP,
    output wire                          S_AXI_RVALID,
    input  wire                          S_AXI_RREADY,

    // 寄存器输出（连接到 DDS 模块）
    output wire [31:0]                   dds_fcw,
    output wire [31:0]                   dds_phase_offset,
    output wire [13:0]                   dds_amplitude,
    output wire                          dds_enable,
    output wire [2:0]                    dds_wave_select
);

// ============================================
// 寄存器定义
// ============================================
reg [31:0] reg_ctrl;          // 偏移 0x00: 控制寄存器
reg [31:0] reg_fcw;           // 偏移 0x04: 频率控制字
reg [31:0] reg_phase_offset;  // 偏移 0x08: 相位偏移
reg [31:0] reg_amplitude;     // 偏移 0x0C: 幅度

// ============================================
// AXI Lite 写逻辑（简化版）
// 实际 AXI Lite 握手较复杂，这里用简化逻辑演示原理
// Vivado 可自动生成完整的 AXI Lite 模板
// ============================================
wire aw_en = S_AXI_AWREADY && S_AXI_WREADY && S_AXI_BREADY;

reg [C_S_AXI_ADDR_WIDTH-1:0] wr_addr;

always @(posedge S_AXI_ACLK) begin
    if (!S_AXI_ARESETN) begin
        reg_ctrl          <= 32'h00000000;
        reg_fcw           <= 32'h00000015;  // 默认约 1Hz
        reg_phase_offset  <= 32'h00000000;
        reg_amplitude     <= 32'h00003FFF;  // 默认最大幅度 (14-bit 满幅)
    end
    else begin
        // 写寄存器（当地址和数据都有效时）
        if (S_AXI_AWVALID && S_AXI_WVALID) begin
            case (S_AXI_AWADDR)
                4'h0: reg_ctrl          <= S_AXI_WDATA;
                4'h4: reg_fcw           <= S_AXI_WDATA;
                4'h8: reg_phase_offset  <= S_AXI_WDATA;
                4'hC: reg_amplitude     <= S_AXI_WDATA;
                default: ;  // 不存在的地址，忽略
            endcase
        end
    end
end

// ============================================
// AXI Lite 读逻辑（简化版）
// ============================================
reg [C_S_AXI_DATA_WIDTH-1:0] reg_data_out;

always @(*) begin
    case (S_AXI_ARADDR)
        4'h0: reg_data_out = reg_ctrl;
        4'h4: reg_data_out = reg_fcw;
        4'h8: reg_data_out = reg_phase_offset;
        4'hC: reg_data_out = reg_amplitude;
        default: reg_data_out = 32'h00000000;
    endcase
end

// ============================================
// 寄存器到 DDS 的映射
// ============================================
assign dds_enable       = reg_ctrl[0];
assign dds_wave_select  = reg_ctrl[3:1];
assign dds_fcw          = reg_fcw;
assign dds_phase_offset = reg_phase_offset;
assign dds_amplitude    = reg_amplitude[13:0];

// ============================================
// AXI 握手信号（简化处理）
// 实际项目中建议使用 Vivado 的 "Create Peripheral" 向导
// 自动生成完整的 AXI Lite 从设备模板
// ============================================
assign S_AXI_AWREADY = 1'b1;  // 简化：总是准备好接收写地址
assign S_AXI_WREADY  = 1'b1;  // 简化：总是准备好接收写数据
assign S_AXI_BRESP   = 2'b00; // OKAY 响应
assign S_AXI_BVALID  = 1'b1;  // 简化：总是有响应
assign S_AXI_ARREADY = 1'b1;  // 简化：总是准备好接收读地址
assign S_AXI_RDATA   = reg_data_out;
assign S_AXI_RRESP   = 2'b00; // OKAY 响应
assign S_AXI_RVALID  = 1'b1;  // 简化：总是有数据

endmodule
```

**注意：上面的代码是教学简化版，实际的 AXI Lite 握手协议需要更严格的时序控制。在 Vivado 中可以使用 "Tools → Create and Package New IP" 向导自动生成完整的 AXI Lite 模板。**

**Vivado 自动生成 AXI Lite IP 的步骤（推荐）：**
1. `Tools` → `Create and Package New IP`
2. 选择 `Create a new AXI4 peripheral`
3. 设置名称：`dds_controller`
4. 接口类型：`Lite`
5. 数据宽度：`32`
6. 寄存器数量：`4`
7. Vivado 会生成包含完整 AXI 握手逻辑的模板
8. 在生成的模板中添加你的寄存器读写逻辑

**ARM 端 C 代码示例（读写 AXI Lite 寄存器）：**

```c
// 文件名：dds_ctrl.h
// ARM 端 C 代码，运行在 ZYNQ PS 上

#include <stdint.h>

// AXI Lite 基地址（与 Vivado Address Editor 中一致）
#define DDS_CTRL_BASE   0x43C00000

// 寄存器偏移
#define DDS_CTRL_REG    (*(volatile uint32_t *)(DDS_CTRL_BASE + 0x00))
#define DDS_FCW_REG     (*(volatile uint32_t *)(DDS_CTRL_BASE + 0x04))
#define DDS_PHASE_REG   (*(volatile uint32_t *)(DDS_CTRL_BASE + 0x08))
#define DDS_AMP_REG     (*(volatile uint32_t *)(DDS_CTRL_BASE + 0x0C))

// 控制位定义
#define DDS_ENABLE      (1 << 0)
#define WAVE_SINE       (0 << 1)
#define WAVE_SQUARE     (1 << 1)
#define WAVE_TRIANGLE   (2 << 1)
#define WAVE_SAWTOOTH   (3 << 1)

// 设置输出频率
void dds_set_frequency(double freq_hz) {
    // FCW = freq × 2^32 / 200MHz
    uint32_t fcw = (uint32_t)(freq_hz * 4294967296.0 / 200000000.0);
    DDS_FCW_REG = fcw;
}

// 设置波形类型
void dds_set_waveform(uint32_t wave_type) {
    uint32_t ctrl = DDS_CTRL_REG;
    ctrl &= ~0x0E;            // 清除波形选择位 [3:1]
    ctrl |= wave_type;        // 设置新的波形类型
    DDS_CTRL_REG = ctrl;
}

// 启动 DDS
void dds_enable(void) {
    DDS_CTRL_REG |= DDS_ENABLE;
}

// 停止 DDS
void dds_disable(void) {
    DDS_CTRL_REG &= ~DDS_ENABLE;
}

// 设置幅度 (0~16383)
void dds_set_amplitude(uint16_t amp) {
    DDS_AMP_REG = (uint32_t)amp;
}

// 使用示例
void example_usage(void) {
    dds_set_frequency(1000000.0);  // 1MHz
    dds_set_waveform(WAVE_SINE);
    dds_set_amplitude(16383);      // 最大幅度
    dds_enable();
}
```

**预期结果 Expected Result:**
- 理解 AXI Lite 寄存器的工作原理
- 知道 ARM 端 C 代码如何通过读写寄存器控制 FPGA 逻辑
- 了解 Vivado 自动生成 AXI Lite IP 的方法

**常见问题 Common Issues:**
- "为什么用 volatile 关键字？" -- 告诉编译器这个地址的内容可能被硬件改变，每次都要真正读写，不要优化掉
- "地址 0x43C00000 从哪里来？" -- 在 Vivado 的 Address Editor 中，AXI Lite IP 会被分配一个基地址。这个地址必须和 C 代码中的宏定义一致

### 任务 4.5: 完整 DDS 仿真 (60 分钟)

**组装完整 DDS 系统：**

现在我们将相位累加器、波形查找表和幅度控制组装成一个完整的 DDS 模块。

**步骤 Steps:**

1. **创建完整 DDS 顶层模块：**

```verilog
// 文件名：dds_top.v
// 功能：完整 DDS 模块（相位累加器 + 查找表 + 幅度控制）

module dds_top #(
    parameter PHASE_WIDTH = 32,
    parameter ADDR_WIDTH  = 10,
    parameter DATA_WIDTH  = 14
)(
    input  wire                          clk,          // 200MHz 时钟
    input  wire                          rst_n,        // 复位
    input  wire [PHASE_WIDTH-1:0]        fcw,          // 频率控制字
    input  wire [PHASE_WIDTH-1:0]        phase_offset, // 相位偏移
    input  wire [DATA_WIDTH-1:0]         amplitude,    // 幅度控制
    input  wire                          enable,       // DDS 使能
    output wire [DATA_WIDTH-1:0]         dac_data      // DAC 输出数据
);

// 内部信号
wire [PHASE_WIDTH-1:0] current_phase;
wire [ADDR_WIDTH-1:0]  lut_addr;
wire [DATA_WIDTH-1:0]  lut_data;

// ============================================
// 1. 相位累加器
// ============================================
reg [PHASE_WIDTH-1:0] phase_acc;

always @(posedge clk or negedge rst_n) begin
    if (!rst_n)
        phase_acc <= {PHASE_WIDTH{1'b0}};
    else if (enable)
        phase_acc <= phase_acc + fcw;
    // else: enable=0 时保持当前相位（暂停输出）
end

// 加入相位偏移
assign current_phase = phase_acc + phase_offset;

// 截取高 10 位作为查找表地址
assign lut_addr = current_phase[PHASE_WIDTH-1 -: ADDR_WIDTH];

// ============================================
// 2. 波形查找表（正弦波 ROM）
// ============================================
reg [DATA_WIDTH-1:0] sine_rom [0:1023];

// 初始化正弦波数据
integer i;
real angle, sine_val;
initial begin
    for (i = 0; i < 1024; i = i + 1) begin
        angle = 2.0 * 3.14159265 * i / 1024.0;
        sine_val = ($sin(angle) + 1.0) / 2.0;
        sine_rom[i] = integer'(sine_val * (2**DATA_WIDTH - 1));
    end
end

// 同步读取
reg [DATA_WIDTH-1:0] lut_data_reg;
always @(posedge clk) begin
    if (enable)
        lut_data_reg <= sine_rom[lut_addr];
end

assign lut_data = lut_data_reg;

// ============================================
// 3. 幅度控制
// ============================================
// dac_data = lut_data × amplitude / (2^DATA_WIDTH - 1)
// 简化实现：取高 DATA_WIDTH 位

reg [DATA_WIDTH*2-1:0] mult_result;
reg [DATA_WIDTH-1:0]   amplitude_reg;

always @(posedge clk) begin
    mult_result   <= lut_data * amplitude;
    amplitude_reg <= mult_result[DATA_WIDTH*2-1 -: DATA_WIDTH];
end

// DAC 输出（延迟 2 个时钟周期：BRAM 1 周期 + 乘法 1 周期）
assign dac_data = enable ? amplitude_reg : {DATA_WIDTH{1'b0}};

endmodule
```

2. **创建完整 DDS 仿真激励：**

```verilog
// 文件名：tb_dds_top.v
`timescale 1ns / 1ps

module tb_dds_top();

// 参数
parameter CLK_PERIOD = 5;  // 5ns = 200MHz
parameter PHASE_WIDTH = 32;

// 信号
reg                          clk;
reg                          rst_n;
reg  [PHASE_WIDTH-1:0]       fcw;
reg  [PHASE_WIDTH-1:0]       phase_offset;
reg  [13:0]                  amplitude;
reg                          enable;
wire [13:0]                  dac_data;

// 实例化 DDS
dds_top #(
    .PHASE_WIDTH(PHASE_WIDTH),
    .ADDR_WIDTH(10),
    .DATA_WIDTH(14)
) uut (
    .clk(clk),
    .rst_n(rst_n),
    .fcw(fcw),
    .phase_offset(phase_offset),
    .amplitude(amplitude),
    .enable(enable),
    .dac_data(dac_data)
);

// 时钟生成
initial begin
    clk = 0;
    forever #(CLK_PERIOD/2) clk = ~clk;
end

// 测试流程
initial begin
    // 初始化
    rst_n = 0;
    fcw = 0;
    phase_offset = 0;
    amplitude = 14'd16383;  // 最大幅度
    enable = 0;

    // 复位
    #100;
    rst_n = 1;
    #50;

    // 测试 1: 产生约 1MHz 正弦波
    // FCW = 1MHz × 2^32 / 200MHz = 21,474,836
    $display("=== 测试 1: 1MHz 正弦波 ===");
    fcw = 32'd21474836;  // 约 1MHz（注意不是 21,474,836，这里故意改小以便仿真）
    enable = 1;
    #10000;  // 观察约 2000 个时钟周期

    // 测试 2: 改变频率到约 2MHz
    $display("=== 测试 2: 约 2MHz ===");
    fcw = 32'd42949673;
    #10000;

    // 测试 3: 降低幅度到 50%
    $display("=== 测试 3: 降低幅度到 50% ===");
    amplitude = 14'd8191;  // 50%
    #10000;

    // 测试 4: 暂停 DDS
    $display("=== 测试 4: 暂停 ===");
    enable = 0;
    #5000;

    // 测试 5: 恢复
    $display("=== 测试 5: 恢复 ===");
    enable = 1;
    amplitude = 14'd16383;
    fcw = 32'd21474836;
    #10000;

    $display("=== 仿真结束 ===");
    $finish;
end

// 监控 DAC 输出（每 100 个时钟周期打印一次）
integer count;
always @(posedge clk) begin
    count = count + 1;
    if (count % 100 == 0 && enable) begin
        $display("时间=%0t, DAC输出=%d", $time, dac_data);
    end
end

endmodule
```

3. **运行仿真：**
   - 在 Vivado 中设置 `tb_dds_top` 为顶层仿真模块
   - 运行 `Run Behavioral Simulation`
   - 观察波形：
     - `dac_data` 应该呈现正弦波形状
     - 频率改变时，正弦波周期变化
     - 幅度改变时，波形振幅变化
     - enable=0 时输出恒为 0

4. **分析仿真结果：**
   - 放大波形，确认正弦波的周期是否正确
   - 验证频率改变时波形是否正确响应
   - 验证幅度控制是否生效

**预期结果 Expected Result:**
- `dac_data` 在仿真波形中呈现清晰的正弦波
- 改变 FCW 时波形频率变化
- 改变 amplitude 时波形振幅变化
- enable=0 时输出归零

**常见问题 Common Issues:**
- "dac_data 全是 0" -- 检查 enable 是否为 1，检查 BRAM 初始化是否成功（initial 块中的 $sin 需要 Verilog 仿真器支持）
- "波形不像正弦波" -- 可能是仿真时间不够长，或者 FCW 太大导致采样点太少。尝试用较小的 FCW 值使一个正弦波周期包含更多采样点
- "幅度改变后波形不变" -- 检查乘法器的流水线延迟，可能需要等待几个额外的时钟周期

---

## 今日作业 | Homework

1. **计算题：**
   - (a) TinyAWG 时钟 200MHz，32 位相位累加器，要输出 5MHz 正弦波，FCW = ?
   - (b) 同上，要输出 440Hz（标准 A 音），FCW = ?
   - (c) 如果 FCW = 0x05555555，输出频率是多少？
2. **设计题：** 思考如何在查找表中同时支持正弦波、方波、三角波。提示：可以用多个 BRAM 或一个 BRAM 的不同区域存储不同波形，用 wave_select 信号选择
3. **代码题：** 在 `axi_lite_regs.v` 中添加第 5 个寄存器（偏移 0x10），用于控制 DAC 输出使能。写出需要修改的代码
4. **系统思考题：** 画出完整的 TinyAWG DDS 数据通路：ARM C 代码 → AXI Lite 寄存器 → 相位累加器 → BRAM 查找表 → 幅度控制 → DAC。标注每个环节的位宽
5. **写实验日志：** 截图 DDS 仿真波形，标注频率改变和幅度改变的效果

---

## 明日预告 | Tomorrow's Preview

今天你掌握了 TinyAWG 的核心——DDS 原理和实现。接下来的课程将进入 ARM 端软件开发（LVGL GUI、触摸驱动、DAC 控制程序），以及硬件集成测试，最终让 TinyAWG 成为一个完整的口袋信号发生器！

Today you mastered TinyAWG's core — DDS principles and implementation. Upcoming sessions will move into ARM-side software development (LVGL GUI, touch driver, DAC control), and hardware integration testing, ultimately making TinyAWG a complete pocket signal generator!

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| DDS 原理详解（FPGA 实现） | B站视频 | 搜索"DDS FPGA 实现 原理" |
| BRAM 使用教程 | B站视频 | 搜索"Vivado BRAM Block RAM 教程" |
| AXI Lite 协议入门 | Xilinx 文档 | UG761: AXI Reference Guide |
| AD9744 数据手册 | PDF | 在 analog.com 搜索 AD9744 |
| TinyAWG 开源项目 Verilog 源码 | GitHub | 参考 TinyAWG 项目中的 RTL 代码 |
| Xilinx DDS Compiler IP 手册 | Xilinx 文档 | PG172: DDS Compiler v6.0 |

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
