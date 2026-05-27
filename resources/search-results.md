# 口袋信号发生器 TinyAWG 项目资源搜索结果

搜索日期：2026-05-27

---

## 一、核心开源参考项目

### 1. TinyAWG 信号源（本项目核心参考）

1. **TinyAWG 35M带宽200M采样率14位任意波发生器** - greentor
   - 链接：https://oshwhub.com/greentor/tinyawg-signal-source
   - 简介：35MHz带宽、200MSa/s、14位任意波形发生器，ZYNQ7010主控
   - 相关性：**本项目直接参考** -- 完整开源硬件和软件

2. **TinyAWG 制作文档（必看）**
   - 链接：https://ai.feishu.cn/docx/BGycd4URIot8Aoxs3vLcBdq2nFc
   - 简介：详细制作教程、注意事项、已知问题修复
   - 相关性：**必读文档** -- 包含所有制作和调试要点

3. **TinyAWG B站演示视频**
   - 链接：https://www.bilibili.com/video/BV1a42QBUECC/
   - 简介：实物展示，波形输出演示，功能介绍
   - 相关性：直接项目演示

### 2. ZYNQ 核心板参考

4. **ZYNQ7020 核心板及各种射频模块** - z_star
   - 链接：https://oshwhub.com/z_star/zynq7020-core-board-and-various-rf-modules
   - 简介：立创开源的 ZYNQ 核心板设计，TinyAWG 项目使用此核心板
   - 相关性：核心板设计文件，可直接打样

### 3. 输出级电路参考

5. **AWG_DHO8-900** - MatthiasElectronic
   - 链接：https://github.com/MatthiasElectronic/AWG_DHO8-900
   - 简介：DHO8/900 示波器的 AWG 电路设计
   - 相关性：TinyAWG 输出级电路参考来源

---

## 二、B站视频资源

### 1. TinyAWG 相关

1. **TinyAWG 信号源演示视频**
   - 链接：https://www.bilibili.com/video/BV1a42QBUECC/
   - 简介：项目实物展示，波形输出测试，功能演示
   - 相关性：**必看** -- 本项目最终效果展示

### 2. ZYNQ / FPGA 入门教程

1. **ZYNQ 入门系列教程** -- 搜索"ZYNQ 入门 Vivado 教程"
   - 核心内容：PS/PL 架构、Block Design、AXI 接口
   - 推荐UP主：威视锐、正点原子、米联客

2. **Vivado 使用教程** -- 搜索"Vivado 入门教程"
   - 核心内容：工程创建、综合、实现、比特流生成
   - 相关性：Day 2 必备

3. **Verilog 入门教程** -- 搜索"Verilog HDL 入门"
   - 核心内容：模块、always块、时序逻辑、组合逻辑
   - 相关性：Day 3 FPGA 编程基础

4. **FPGA DDS 设计** -- 搜索"FPGA DDS 直接数字合成"
   - 核心内容：相位累加器、查找表、频率控制字
   - 相关性：Day 4 DDS 核心原理

### 3. 嵌入式开发教程

1. **LVGL 嵌入式 GUI 开发** -- 搜索"LVGL 入门 教程"
   - 核心内容：LVGL 框架、控件使用、事件处理
   - 相关性：Day 7-8 GUI 开发

2. **I2C / SPI 通信协议** -- 搜索"I2C SPI 通信原理"
   - 核心内容：总线协议、时序、驱动编写
   - 相关性：Day 5-6 驱动开发

3. **DAC 数模转换原理** -- 搜索"DAC 原理 数模转换"
   - 核心内容：R-2R、电流舵 DAC、分辨率与采样率
   - 相关性：Day 6 DAC 驱动

### 4. 信号与波形基础

1. **信号发生器原理** -- 搜索"信号发生器 原理 DDS"
   - 核心内容：DDS 原理、任意波形、频率合成
   - 相关性：Day 1 项目原理

2. **正弦波与频谱分析** -- 搜索"频谱分析 FFT 教学"
   - 核心内容：傅里叶变换、频域分析、THD
   - 相关性：Day 12 性能测试

### 5. PCB 焊接与组装

1. **SMD 贴片焊接教程** -- 搜索"SMD 焊接教程 热风枪"
   - 核心内容：贴片焊接技巧、焊锡膏使用、热风枪操作
   - 相关性：Day 10 PCB 组装

2. **嘉立创 PCB 打板教程** -- 搜索"嘉立创 PCB 打板"
   - 核心内容：EDA 设计、下单打板、焊接
   - 相关性：Day 10

3. **3D 打印外壳设计** -- 搜索"3D 打印 外壳 设计"
   - 核心内容：Fusion 360 建模、STL 导出、打印参数
   - 相关性：Day 11 外壳制作

---

## 三、GitHub 开源项目

### ZYNQ / FPGA 信号发生器

| 项目 Project | Stars | 说明 Description |
|-------------|-------|-----------------|
| [AWG_DHO8-900](https://github.com/MatthiasElectronic/AWG_DHO8-900) | -- | TinyAWG 输出级电路参考，DHO8/900 AWG 设计 |
| [TinyAWG](https://oshwhub.com/greentor/tinyawg-signal-source) | -- | 本项目核心参考（OSHWhub 平台） |

### FPGA 开发参考

| 项目 Project | Stars | 说明 Description |
|-------------|-------|-----------------|
| [FPGA-Oscilloscope](https://github.com/agural/FPGA-Oscilloscope) | 125 | FPGA 示波器，有完整设计文档和 Verilog 代码 |
| [fpga_dds](https://github.com/synthworks/fpga_dds) | -- | FPGA DDS 实现，可参考相位累加器设计 |

### LVGL GUI 开发

| 项目 Project | Stars | 说明 Description |
|-------------|-------|-----------------|
| [lvgl](https://github.com/lvgl/lvgl) | 16000+ | LVGL 嵌入式图形库，本项目使用的 GUI 框架 |
| [lv_port_pc_visual_studio](https://github.com/lvgl/lv_port_pc_visual_studio) | -- | LVGL PC 模拟器，无需硬件即可开发 GUI |

### DAC 驱动参考

| 项目 Project | 说明 Description |
|-------------|-----------------|
| [AD9744 Datasheet](https://www.analog.com/en/products/ad9744.html) | AD9744 官方数据手册，14-bit 210MSPS DAC |
| [DAC8562 Datasheet](https://www.ti.com/product/DAC8562) | DAC8562 官方数据手册，16-bit 双通道 DAC |

---

## 四、学习教程资源

### FPGA / ZYNQ 基础

1. **ZYNQ-7000 SoC Technical Reference Manual (UG585)**
   - 来源：Xilinx 官网
   - 核心内容：PS (ARM) 架构、PL (FPGA) 架构、AXI 接口

2. **Vivado Design Suite User Guide**
   - 来源：Xilinx 官网
   - 核心内容：工程创建、Block Design、约束文件、综合实现

3. **Verilog HDL 入门**
   - 推荐资源：B站搜索"Verilog 入门"
   - 核心内容：模块化设计、时序/组合逻辑、状态机

### DDS 与信号处理

1. **DDS（直接数字合成）原理**
   - 核心公式：f_out = (FCW / 2^N) * f_clk
   - 关键概念：相位累加器、查找表、DAC 重建滤波
   - B站搜索："DDS 原理 讲解"

2. **信号完整性基础**
   - 核心内容：阻抗匹配、反射、串扰、抖动
   - 相关性：输出级电路设计和 PCB 布局

### 模拟电路

1. **运算放大器应用**
   - 核心内容：高速运放、反馈网络、带宽增益积
   - 参考芯片：OPA2673 (TinyAWG 使用)
   - B站搜索："高速运放 信号调理"

2. **电源设计**
   - 核心内容：DCDC 转换器、正负电源、纹波抑制
   - 参考芯片：LMR544006 (TinyAWG 使用)
   - B站搜索："DCDC 电源设计 正负电源"

### 嵌入式软件开发

1. **Vitis / SDK 嵌入式开发**
   - 核心内容：ARM Cortex-A9 编程、外设驱动、CMake
   - 来源：Xilinx 官网 Vitis 文档

2. **LVGL 8.3 开发文档**
   - 链接：https://docs.lvgl.io/
   - 核心内容：控件 API、事件系统、主题、字体

3. **I2C / SPI 驱动开发**
   - 核心内容：ZYNQ PS I2C/SPI 外设配置、中断处理
   - 参考：ZYNQ SDK examples

---

## 五、采购参考

### 核心元器件

| 元器件 | 型号 | 参考价格 | 采购渠道 | 备注 |
|--------|------|---------|---------|------|
| 主控 | ZYNQ7010 核心板 | 60 元 | 立创开源打样 | 自制核心板 |
| DAC | AD9744 | 10 元 | 淘宝/立创商城 | 14-bit 210MSPS |
| 运放 | OPA2673IRGVT | 6 元 | 淘宝/立创商城 | 高速双运放 |
| 辅助DAC | DAC8562SDGSR | 7 元 | 淘宝/立创商城 | 16-bit 双通道 |
| 电源DCDC | LMR544006 | 3 元/片 | 立创商城 | 正负电源转换 |
| 触摸IC | FT6336 | 3 元 | 淘宝 | I2C 电容触摸 |
| 电量计 | MAX17048 | 3 元 | 立创商城 | I2C 电池监测 |
| 显示屏 | 2.8" IPS LCD+触摸 | 63 元 | 淘宝 | 含触摸面板 |
| 电池 | 3.7V 锂电池 1500mAh | 20 元 | 淘宝 | 聚合物锂电池 |

### PCB 与外壳

| 项目 | 参考价格 | 采购渠道 | 备注 |
|------|---------|---------|------|
| PCB 打板 (4层) | 30 元/5片 | 嘉立创 jlcpcb.com | 使用开源 PCB 文件 |
| 3D 打印外壳 | 50 元 | 嘉立创 3D 打印 | 树脂材料推荐 |

### 工具

| 工具 | 参考价格 | 备注 |
|------|---------|------|
| JTAG 调试器 | 20-50 元 | Xilinx Platform Cable 或兼容器 |
| 烙铁+热风枪 | 50-100 元 | SMD 焊接必备 |
| 万用表 | 30-50 元 | 调试必备 |
| 焊锡膏+助焊剂 | 15-20 元 | SMD 焊接辅助 |
| 电源数据分路器 | 10-15 元 | USB 串口调试必备 |

---

## 六、项目实施建议

### 推荐技术路线

**TinyAWG 方案（本项目）：**
- 主控：ZYNQ7010 (PS: ARM Cortex-A9 + PL: FPGA Artix-7)
- DDS：PL 端 BRAM 流水线结构，200MHz 时钟
- DAC：AD9744 (14-bit 210MSPS) + DAC8562 (幅度/偏置控制)
- GUI：LVGL 8.3.1 + 2.8" IPS 触摸屏
- 成本：约 220 元
- 带宽：35MHz (-3dB)

### 关键学习里程碑

1. **阶段1**：理解 AWG 和 DDS 原理（Day 1）
2. **阶段2**：ZYNQ 开发环境搭建和基础实验（Day 2-3）
3. **阶段3**：FPGA DDS 模块设计与仿真（Day 4）
4. **阶段4**：ARM 驱动开发（SPI/I2C/GPIO）（Day 5）
5. **阶段5**：DAC 输出控制与波形生成（Day 6）
6. **阶段6**：LVGL GUI 开发与触摸交互（Day 7-8）
7. **阶段7**：系统集成与联调（Day 9）
8. **阶段8**：PCB 组装与焊接（Day 10）
9. **阶段9**：外壳设计与整机装配（Day 11）
10. **阶段10**：测试校准与项目展示（Day 12）

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
