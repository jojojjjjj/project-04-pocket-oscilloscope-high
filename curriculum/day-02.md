# Day 02: Vivado 工程与 ZYNQ 硬件配置 | Vivado Project & ZYNQ Hardware Configuration

> **今日目标 Today's Objectives:**
> - 掌握 Vivado 工程创建流程
> - 学会使用 ZYNQ Block Design 配置 PS（ARM 处理器系统）
> - 理解 AXI GPIO 模块的作用并添加到设计中
> - 完成第一个 FPGA 设计的完整流程：综合 → 实现 → 生成比特流 → 下载到 ZYNQ
>
> **产出 Deliverable:** ZYNQ 上 LED 成功点亮/闪烁，完成 PS + PL 联合设计

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:15 | 晨会 Morning Meeting | 昨日回顾、今日目标 |
| 09:15-10:30 | 知识讲解 Lecture | Vivado 工程创建 + ZYNQ Block Design |
| 10:30-10:45 | 休息 Break | Break |
| 10:45-12:00 | 动手实验 Hands-on | PS 配置 + PL 配置 |
| 12:00-13:30 | 午餐 Lunch | Lunch + Break |
| 13:30-15:00 | 动手实验 Hands-on | AXI GPIO + 约束文件 |
| 15:00-15:15 | 休息 Break | Break |
| 15:15-16:30 | 动手实验 Hands-on | 综合、实现、生成比特流、下载到 ZYNQ |
| 16:30-17:00 | 回顾 Review | 今日总结 + 答疑 |

---

## 上午: Vivado 工程创建 | Morning: Vivado Project Creation

### 为什么要学这个? | Why Learn This?

Vivado 是 FPGA 开发的核心工具，就像写 C 代码需要 IDE 一样。ZYNQ 的特殊之处在于它同时包含 ARM（PS）和 FPGA（PL），所以我们需要用 Block Design（图形化设计）的方式把两部分的配置"画"出来。今天你将完成从零到一的整个流程——这是后续所有 FPGA 开发的基础。

Vivado is the core tool for FPGA development, just as you need an IDE for C coding. ZYNQ is special because it contains both an ARM processor (PS) and FPGA (PL), so we use Block Design (graphical design) to "draw" the configuration for both sides. Today you will complete the entire flow from zero to one — this is the foundation for all subsequent FPGA development.

### 任务 2.1: 创建 Vivado 工程 (30 分钟)

**步骤 Steps:**

1. **启动 Vivado：**
   - 双击桌面图标或从开始菜单启动
   - 等待启动完成（可能需要 1-2 分钟）

2. **创建新工程：**
   - 点击 `Create Project`
   - 选择工程路径（建议：`D:/TinyAWG/day02_led/`，路径中不要有中文和空格）
   - 工程名称：`tinyawg_led`
   - 勾选 `Create project subdirectory`
   - 点击 Next

3. **选择工程类型：**
   - 选择 `RTL Project`
   - 勾选 `Do not specify sources at this time`（我们稍后添加）
   - 点击 Next

4. **选择目标器件：**
   - 在 `Parts` 选项卡中：
     - Family: `Zynq-7000`
     - Package: `clg400`
     - Speed: `-1`
   - 或者直接搜索：`xc7z010clg400-1`
   - 选中后点击 Next，然后点击 Finish

5. **确认工程创建成功：**
   - Vivado 主界面打开后，左侧 `Flow Navigator` 面板可见
   - 顶部显示工程名称和目标器件

**预期结果 Expected Result:**
- Vivado 主界面正常显示
- 工程名称和目标器件 `xc7z010clg400-1` 正确

**常见问题 Common Issues:**
- "工程路径包含中文导致错误" -- 将工程移动到纯英文路径下重新创建
- "找不到 xc7z010clg400-1" -- 确认 Vivado 安装时包含了 Zynq-7000 器件支持

### 任务 2.2: 创建 ZYNQ Block Design (45 分钟)

**什么是 Block Design？**

Block Design 是 Vivado 提供的图形化设计工具，让你像搭积木一样搭建 FPGA 系统。对于 ZYNQ 来说，Block Design 是配置 PS（ARM）和连接 PL（FPGA）模块的主要方式。

Block Design is Vivado's graphical design tool that lets you build FPGA systems like assembling blocks. For ZYNQ, Block Design is the primary way to configure the PS (ARM) and connect PL (FPGA) modules.

**步骤 Steps:**

1. **创建 Block Design：**
   - 在 Flow Navigator 中，点击 `IP INTEGRATOR` → `Create Block Design`
   - Design name: `system`
   - 点击 OK

2. **添加 ZYNQ Processing System：**
   - 在空白画布上点击 `+` 号（或右键 → Add IP）
   - 搜索 `ZYNQ7 Processing System`
   - 双击添加到设计中
   - 你会看到 ZYNQ PS 模块出现在画布上

3. **运行 Block Automation：**
   - 添加 ZYNQ PS 后，顶部会出现绿色提示条 `Run Block Automation`
   - 点击它，使用默认设置
   - 这会自动配置 DDR 和 FIXED_IO 端口

4. **配置 ZYNQ PS 参数：**
   - 双击 ZYNQ PS 模块打开配置界面 `Re-customize IP`
   - 你将看到 ZYNQ 的配置页面，包含多个选项卡

**PS 配置详解——Page 1: PS-PL Configuration：**

```
ZYNQ7 Processing System 配置页面：

┌──────────────────────────────────────────────┐
│ PS-PL Configuration │ Peripherals │ Clock │ ..│
├──────────────────────────────────────────────┤
│                                               │
│  ▼ AXI Non Secure Enablement                  │
│    ├─ GP Master 0:  ✓ (PS 读 PL 寄存器)       │
│    └─ GP Master 1:  ☐                         │
│                                               │
│  ▼ Slave Interfaces                            │
│    └─ GP Slave 0:   ☐                         │
│                                               │
│  (这些是 PS 和 PL 之间的 AXI 通道)              │
│                                               │
└──────────────────────────────────────────────┘
```

   - 确认 `M AXI GP0 interface` 已启用（默认启用）
   - 这是 ARM 访问 FPGA 逻辑的"大门"

**PS 配置详解——Page 2: Peripherals：**

   - 展开 `I/O Peripherals`
   - 确认 UART 1 已启用（用于调试打印）
   - 确认 GPIO 已启用（MIO 部分，用于 PS 侧的 GPIO）
   - 其他保持默认

**PS 配置详解——Page 3: Clock Configuration：**

   - 展开 `PL Fabric Clocks`
   - 启用 `FCLK_CLK0`，设置为 `100 MHz`（PL 侧的主时钟）
   - 这个 100MHz 时钟将驱动 PL 侧的所有模块
   - 启用 `FCLK_CLK1`，设置为 `200 MHz`（DDS 所需的高速时钟，为后续准备）

   - 点击 OK 保存配置

**预期结果 Expected Result:**
- ZYNQ PS 模块出现在 Block Design 中
- DDR 和 FIXED_IO 端口已通过 Block Automation 连接
- FCLK_CLK0 (100MHz) 和 FCLK_CLK1 (200MHz) 已启用

**常见问题 Common Issues:**
- "Run Block Automation 没有出现" -- 点击画布空白处，再看顶部提示条
- "找不到 ZYNQ7 Processing System" -- 确认工程的目标器件是 ZYNQ (xc7z010)，不是纯 FPGA
- "配置页面选项太多，不知道该改哪些" -- 暂时只改上面提到的内容，其余保持默认

### 任务 2.3: PL 侧配置与 AXI GPIO (30 分钟)

**为什么要用 AXI GPIO？**

在 TinyAWG 中，ARM (PS) 需要控制 FPGA (PL) 上的各种模块。AXI GPIO 就是一个简单的"控制通道"——ARM 写一个 32 位数值，FPGA 就能在对应的引脚上输出高/低电平。我们先用它控制 LED，后续会用 AXI Lite 寄存器来控制 DDS 的频率、幅度等参数。

In TinyAWG, the ARM (PS) needs to control modules on the FPGA (PL). AXI GPIO is a simple "control channel" — the ARM writes a 32-bit value, and the FPGA outputs high/low levels on corresponding pins. We start with LED control, later we'll use AXI Lite registers to control DDS frequency, amplitude, etc.

**步骤 Steps:**

1. **添加 AXI GPIO 模块：**
   - 在 Block Design 中点击 `+` 号
   - 搜索 `AXI GPIO`
   - 双击添加

2. **配置 AXI GPIO：**
   - 双击 AXI GPIO 模块
   - 将 `GPIO Width` 设为 `4`（4 位，控制 4 个 LED）
   - `GPIO is bidirectional` 保持不勾选（我们只需要输出）
   - 点击 OK

3. **运行 Connection Automation：**
   - 顶部出现绿色提示条 `Run Connection Automation`
   - 勾选所有选项，点击 OK
   - Vivado 会自动连接：
     - AXI GPIO 的 S_AXI → ZYNQ PS 的 M_AXI_GP0（数据通道）
     - AXI GPIO 的 s_axi_aclk → FCLK_CLK0（时钟）
     - AXI GPIO 的 s_axi_aresetn → 复位信号

4. **观察连接结果：**
   - 你应该看到类似如下的连接拓扑：

```
┌──────────────┐    AXI 总线     ┌──────────────┐
│  ZYNQ PS     │───────────────►│  AXI GPIO    │
│              │  M_AXI_GP0     │              │
│  FCLK_CLK0   │───►(时钟)──────│  s_axi_aclk  │
│  FCLK_CLK1   │                │              │
│              │                │  gpio_rtl    │──► LED[3:0]
└──────────────┘                └──────────────┘
     DDR3                           │
      │                        Processor System
      ▼                        Reset Module
   DDR 端口                    (自动添加)
```

**预期结果 Expected Result:**
- AXI GPIO 模块已添加并自动连接到 ZYNQ PS
- 可以看到 `gpio_rtl` 端口引出在外（待连接到引脚）

**常见问题 Common Issues:**
- "Connection Automation 只显示部分选项" -- 确保先完成 ZYNQ PS 的 Block Automation，再添加 AXI GPIO
- "AXI 总线连接太乱看不清" -- 点击工具栏的"放大"按钮，或按 `Ctrl+F` 搜索模块名称

---

## 下午: 约束文件与比特流生成 | Afternoon: Constraints & Bitstream Generation

### 任务 2.4: 创建约束文件 (XDC) (30 分钟)

**什么是约束文件？**

约束文件（XDC, Xilinx Design Constraints）告诉 Vivado："这个模块的信号对应到芯片的哪个引脚"。FPGA 芯片上有数百个引脚，你必须明确指定每个信号连接到哪个引脚，否则 Vivado 不知道怎么连线。

Constraint files (XDC) tell Vivado: "This signal maps to which pin on the chip." An FPGA has hundreds of pins, and you must explicitly specify which signal connects to which pin, otherwise Vivado doesn't know how to route the wires.

**步骤 Steps:**

1. **确定 LED 引脚编号：**
   - 查看核心板原理图，找到 LED 连接的引脚
   - 常见的 ZYNQ7010 核心板 LED 引脚：`H15`, `G15`, `M14`, `L14`（具体以你的核心板为准）
   - 向助教确认你使用的核心板的 LED 引脚编号

2. **添加约束文件：**
   - 在 `Sources` 面板，右键 `Constraints` → `Add Sources`
   - 选择 `Create File`
   - 文件名：`tinyawg_led.xdc`
   - 点击 OK

3. **编写约束：**

```tcl
## TinyAWG LED 约束文件
## LED 引脚定义（根据实际核心板原理图修改）

# LED 0 - 绿色
set_property PACKAGE_PIN H15 [get_ports {gpio_rtl_tri_o[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_rtl_tri_o[0]}]

# LED 1 - 绿色
set_property PACKAGE_PIN G15 [get_ports {gpio_rtl_tri_o[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_rtl_tri_o[1]}]

# LED 2 - 蓝色
set_property PACKAGE_PIN M14 [get_ports {gpio_rtl_tri_o[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_rtl_tri_o[2]}]

# LED 3 - 蓝色
set_property PACKAGE_PIN L14 [get_ports {gpio_rtl_tri_o[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_rtl_tri_o[3]}]

## 配置引脚电平标准
## LVCMOS33 = 3.3V LVCMOS 标准（ZYNQ PL 侧 Bank 电压）
```

4. **保存文件**（Ctrl+S）

**引脚约束语法解释：**

```tcl
# set_property 属性 值 [get_ports 端口名]
#
# PACKAGE_PIN: 物理引脚编号（芯片封装上的位置）
# IOSTANDARD: 电平标准（LVCMOS33 = 3.3V CMOS）
#
# gpio_rtl_tri_o[0] 是 AXI GPIO 自动生成的端口名
# "tri" 表示三态 (tri-state)，"o" 表示输出 (output)
```

**预期结果 Expected Result:**
- XDC 文件已添加到工程中
- 引脚编号与核心板原理图一致

**常见问题 Common Issues:**
- "引脚名 `gpio_rtl_tri_o` 从哪里来的？" -- 这是 Block Design 中 AXI GPIO 的端口自动生成的名称。可以在 Block Design 中点击端口查看
- "IOSTANDARD 报错" -- 确认 ZYNQ PL 侧 Bank 供电电压。大部分核心板是 3.3V，对应 LVCMOS33
- "找不到对应引脚" -- 确认你查的是 PL 侧引脚，不是 PS 侧的 MIO 引脚

### 任务 2.5: 综合、实现与比特流生成 (60 分钟)

**FPGA 开发流程：**

```
源代码/Block Design → [综合 Synthesis] → [实现 Implementation] → [比特流 Bitstream]
    RTL 代码/图形         逻辑优化            布局布线             .bit 文件
                         生成网表            物理映射              可下载
```

1. **综合 (Synthesis):** 把你的设计翻译成 FPGA 能理解的基本逻辑单元（LUT、FF、BRAM 等）
2. **实现 (Implementation):** 把逻辑单元放置到 FPGA 芯片的具体位置，并连接布线
3. **比特流生成 (Bitstream Generation):** 生成最终的 .bit 文件，可以下载到 FPGA

**步骤 Steps:**

1. **生成 HDL Wrapper：**
   - 在 Sources 面板，右键 Block Design (`system`)
   - 选择 `Create HDL Wrapper`
   - 选择 `Let Vivado manage wrapper and auto-update`
   - 点击 OK
   - 这会生成顶层 Verilog 文件，把 Block Design 包装成标准 RTL 模块

2. **运行综合 (Synthesis)：**
   - 在 Flow Navigator 中，点击 `Run Synthesis`
   - 等待完成（首次可能需要 5-15 分钟）
   - 综合完成后弹出对话框，选择 `Run Implementation`

3. **运行实现 (Implementation)：**
   - 等待完成（可能需要 5-10 分钟）
   - 如果出现错误，检查约束文件中的引脚编号是否正确
   - 实现完成后弹出对话框，选择 `Generate Bitstream`

4. **生成比特流 (Bitstream)：**
   - 等待完成（约 3-5 分钟）
   - 完成后弹出对话框，选择 `Open Hardware Manager`

5. **查看资源利用率：**
   - 综合完成后可以查看 `Open Synthesized Design` → `Report Utilization`
   - 你应该看到 AXI GPIO 和 ZYNQ PS 使用了极少的 PL 资源

**预期结果 Expected Result:**
- 综合、实现、比特流生成全部成功，无错误
- 在工程目录下生成了 `tinyawg_led.bit` 文件

**常见问题 Common Issues:**
- "综合报错：cannot find port gpio_rtl_tri_o" -- 端口名可能不同，打开 Block Design 确认端口名，或查看 HDL Wrapper 文件
- "实现报错：Placer could not place IO" -- 约束文件中的引脚编号与实际核心板不匹配，检查原理图
- "比特流生成失败" -- 检查是否完成了 Generate Bitstream 流程，查看错误日志 (Messages 面板)
- "等待时间太长" -- 首次综合需要较长时间，后续增量编译会快很多。可以利用等待时间预习后续内容

### 任务 2.6: 首次下载到 ZYNQ (45 分钟)

**步骤 Steps:**

1. **连接硬件：**
   - 确保 JTAG 下载器已连接到核心板（参考 Day 01 任务 1.6）
   - 核心板上电

2. **打开 Hardware Manager：**
   - 如果比特流生成后选择了 `Open Hardware Manager`，它已自动打开
   - 否则：Flow Navigator → `PROGRAM AND DEBUG` → `Open Hardware Manager`

3. **连接目标器件：**
   - 点击 `Open target` → `Auto Connect`
   - 等待识别到 `xc7z010` 器件
   - 展开器件树，确认看到：
     - `xc7z010_1` (ZYNQ PL)
     - `ARM DAP` (ZYNQ PS 调试口)

4. **编程器件：**
   - 右键 `xc7z010_1` → `Program Device`
   - 确认 Bitstream file 路径正确（指向 `tinyawg_led.bit`）
   - 点击 `Program`
   - 等待编程完成（约 10-30 秒）

5. **验证 LED 状态：**
   - 编程完成后，观察核心板上的 LED
   - 由于我们还没有编写 ARM 端的 C 代码，LED 状态取决于默认值
   - LED 应该全部处于低电平（灭）或高电平（灭），取决于电路设计

6. **使用 XSCT 测试 LED（可选）：**
   - 打开 Vivado 的 XSCT 控制台（Tools → XSCT Console）
   - 连接到 ARM 并运行简单命令：

```tcl
# 连接到目标
connect
targets -set -filter {name =~ "Cortex-A9 #0"}
rst -system

# 加载 FPGA 比特流
fpga D:/TinyAWG/day02_led/tinyawg_led.runs/impl_1/system_wrapper.bit

# 暂停处理器
stop

# 写 AXI GPIO 寄存器（点亮 LED）
# AXI GPIO 基地址可以在 Address Editor 中查看，通常是 0x41200000
mwr 0x41200000 0x0F    # 写入 0x0F = 0b1111，4个 LED 全部拉高

# 熄灭所有 LED
mwr 0x41200000 0x00    # 写入 0x00 = 0b0000

# 点亮单个 LED
mwr 0x41200000 0x01    # 写入 0x01 = 0b0001，只有 LED0 亮
mwr 0x41200000 0x05    # 写入 0x05 = 0b0101，LED0 和 LED2 亮
```

**预期结果 Expected Result:**
- FPGA 编程成功，无报错
- 通过 XSCT 写 AXI GPIO 寄存器可以控制 LED 亮灭
- 理解了"ARM 通过 AXI 总线控制 FPGA 逻辑"这一核心概念

**常见问题 Common Issues:**
- "Auto Connect 找不到器件" -- 参考 Day 01 任务 1.6 的排查步骤
- "Program 失败" -- 检查 .bit 文件路径是否正确，确认核心板已上电
- "mwr 写了但 LED 没反应" -- 确认 AXI GPIO 基地址（在 Address Editor 中查看），确认 LED 引脚约束是否正确，确认 LED 是高电平点亮还是低电平点亮

---

## 今日作业 | Homework

1. **画图题：** 画出你的 Block Design 连接图，标注 ZYNQ PS、AXI GPIO、时钟、复位之间的连线关系
2. **填空题：**
   - (a) ZYNQ7010 中 PS 代表 ________，运行 ________ 代码；PL 代表 ________，运行 ________ 代码
   - (b) PS 和 PL 之间通过 ________ 总线通信
   - (c) 约束文件 (XDC) 的作用是 ________
   - (d) FPGA 开发三步流程是：______ → ______ → ______
3. **操作题：** 尝试修改 AXI GPIO 的输出值，实现 LED 跑马灯效果（用 XSCT 手动逐个写入）
4. **思考题：** 在 TinyAWG 中，DDS 的频率控制字需要由 ARM 写入 FPGA。你会用什么方式实现这个"写"操作？（提示：参考今天的 AXI GPIO）
5. **写实验日志：** 记录今天的完整操作步骤，截图保存 Vivado Block Design 和 Hardware Manager 界面

---

## 明日预告 | Tomorrow's Preview

明天我们将学习 FPGA 的"母语"——Verilog 硬件描述语言。你将编写第一个 Verilog 模块（PWM 调光），学习组合逻辑和时序逻辑的区别，并在仿真器中观察信号波形。这是从"图形化积木"到"代码化设计"的飞跃！

Tomorrow we will learn FPGA's "native language" — Verilog Hardware Description Language. You will write your first Verilog module (PWM dimmer), learn the difference between combinational and sequential logic, and observe signal waveforms in the simulator. This is the leap from "graphical blocks" to "code-based design"!

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| ZYNQ Block Design 入门教程 | B站视频 | 搜索"ZYNQ Block Design 入门" |
| AXI GPIO 使用指南 | Xilinx 文档 | PG144: AXI GPIO v2.0 |
| Vivado 约束文件教程 | Xilinx 文档 | UG912: Vivado Design Suite User Guide |
| ZYNQ7 PS 配置详解 | Xilinx 文档 | UG585: Zynq-7000 Technical Reference Manual |
| TinyAWG 开源工程约束参考 | GitHub | 参考 TinyAWG 项目中的 XDC 文件 |
| XSCT 命令参考 | Xilinx 文档 | UG1208: Embedded Design Tutorial |

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
