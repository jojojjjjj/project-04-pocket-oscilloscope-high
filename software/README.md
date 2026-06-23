# Software | 软件说明

> Project 04 — WavePocket 口袋信号发生器（iCE40 FPGA 主线）

## 主线技术栈 | Main-line Stack

本项目主线是 **iCE40 FPGA + Verilog**，不是 MCU + C。FPGA 用硬件描述语言（Verilog）编程，工具链是开源的 **Yosys（综合）→ nextpnr（布局布线）→ iceprog（烧录）**，不装 Vivado。

- 目标板：Lattice iCE40（HX8K / UP5K）开发板
- 语言：**Verilog HDL**（在 FPGA 里写相位累加器、波形查找表、DAC 接口等组合/时序逻辑）
- 工具链：Yosys + nextpnr-ice40 + iceprog（开源，轻量，~几百 MB，远低于 Vivado 的 60GB）
- 上位机（可选）：用 PC 脚本通过串口/SPI 调参，不写 Android/Unity

主线跟着教程跑通：**先用 Verilog 点亮 LED → 按键消抖 → DDS 相位累加器+查找表 → DAC 输出波形 → TFT 显示+编码器调参**。详见 `curriculum/day-01.md` 起的每日教案。

## `src/*.c` 是什么 | What the `.c` files are

`src/` 下的 C 文件（`dds_controller.c`、`dac_driver.c`、`waveform.c`、`main.c`、`gui_helper.c`、`output_ctrl.c`、`utils.c`）是**早期 ZYNQ SoC（ARM 侧）版本的参考实现**，本项目已迁移到纯 iCE40 FPGA 主线后**不再直接使用**——iCE40 没有 ARM 核，不跑 C 程序。

**它们的价值**：DDS 算法（相位累加、频率控制字、波形表）的逻辑是通用的。读这些 C 代码能帮你**理解算法**（相位累加器怎么累加、查找表怎么索引、频率怎么换算），再**翻译成 Verilog** 写到 FPGA 里。也就是：**用 C 读懂算法 → 用 Verilog 实现硬件**。

- `waveform.c` — 正弦/方波/三角/锯齿波的查找表生成逻辑（翻译成 Verilog 的 `case` 或初始化的 reg 表）
- `dds_controller.c` — 相位累加器 + 频率控制字（FTW）逻辑（翻译成 Verilog 的累加寄存器）
- `dac_driver.c` — DAC 并行数据接口时序（翻译成 Verilog 的输出寄存器）
- `main.c` / `gui_helper.c` / `output_ctrl.c` — ZYNQ ARM 侧的控制逻辑，iCE40 主线不需要（调参改用 FPGA 上的编码器+TFT）

> ⚠️ **不要用 C 编译器编译 `src/*.c` 期望烧进 iCE40**——iCE40 用 Verilog 比特流。这些 C 是**算法参考**，不是可烧录固件。

## Verilog 在哪 | Where's the Verilog

主线的 Verilog 代码跟着 `curriculum/day-XX.md` 逐步写出来（day-01 LED、day-02 组合/时序逻辑、day-04 DDS 核心……）。教案里给关键模块的 Verilog 骨架，学员补全后用 Yosys+nextpnr 综合烧录。如需完整参考工程，见开源出处 [oshwhub greentor/tinyawg-signal-source](https://oshwhub.com/greentor/tinyawg-signal-source)（TinyAWG 原项目，iCE40/AWG 参考）。

## 配置 | Config

- `config.template.yaml` — 上位机/参数模板（只读对照，参考用）

---

*主线 = iCE40 FPGA + Verilog + 开源工具链；`src/*.c` 是 ZYNQ 版算法参考，帮理解 DDS 再翻译成 Verilog。*
