# Day 09: 系统集成与联调 | System Integration & Joint Debugging

> **今日目标 Today's Objectives:**
> - 完成 PS-PL AXI 通信，实现 PS 端向 PL 寄存器写入频率/幅度/波形参数
> - 测试完整波形输出链路：波形数据 -> BRAM -> DDS -> AD9744 DAC -> OPA2673 运放 -> 输出
> - 实现 PC 上位机串口通信，支持波形导入和手绘波形功能
> - 用示波器验证输出波形质量
>
> **产出 Deliverable:** TinyAWG 能通过 PS 控制 PL 输出指定频率/幅度的波形，PC 端可发送自定义波形数据

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 | Day 8 回顾 + 今日系统集成目标 |
| 09:30-10:30 | 知识讲解 | AXI Lite 通信原理与 PS-PL 数据交互 |
| 10:30-10:45 | 休息 | Break |
| 10:45-12:00 | 动手实验 | PS-PL 寄存器通信 + 波形输出链路测试 |
| 12:00-13:30 | 午餐 | Lunch + Break |
| 13:30-15:00 | 动手实验 | PC 上位机串口通信与波形导入 |
| 15:00-15:15 | 休息 | Break |
| 15:15-16:30 | 动手实验 | 手绘波形功能 + 完整系统示波器验证 |
| 16:30-17:00 | 回顾 | 今日总结 + Phase 3 硬件组装预告 |

---

## 上午: PS-PL 通信与波形链路 | Morning: PS-PL Communication & Waveform Pipeline

### 为什么要学这个? | Why Learn This?

ZYNQ7010 最大的优势就是"双核"架构：ARM Cortex-A9 (PS 端) 负责运行 Linux 和 LVGL GUI，FPGA Artix-7 (PL 端) 负责高速信号处理。两个核心之间需要通过 AXI 总线交换数据。今天我们要打通这条"高速公路"，让 ARM 写入的波形参数能够实时控制 FPGA 的 DDS 输出。

The biggest advantage of ZYNQ7010 is its "dual-core" architecture: the ARM Cortex-A9 (PS) runs Linux and LVGL GUI, while the FPGA Artix-7 (PL) handles high-speed signal processing. The two cores exchange data via the AXI bus. Today we will build this "highway" so waveform parameters written by ARM can control the FPGA DDS output in real time.

### 任务 9.1: AXI Lite 寄存器通信 (45 分钟)

**PS-PL 通信架构：**

```
ARM Cortex-A9 (PS)                    FPGA Artix-7 (PL)
   +-----------+                       +------------------+
   | LVGL GUI  |                       |   DDS Engine     |
   | (波形参数) |                       |   (相位累加器)    |
   +-----+-----+                       +--------+---------+
         |                                      |
   +-----+-----+     AXI4-Lite     +-----------+-----------+
   | AXI Master | <===============> | AXI Slave (自定义IP)  |
   +-----------+    32-bit 寄存器    +-----------+-----------+
                                          |      |      |
                                     freq_reg amp_reg wave_reg
```

**关键寄存器定义：**

| 寄存器地址 Offset | 名称 | 位宽 | 功能 |
|------------------|------|------|------|
| 0x00 | FREQ_REG | 32-bit | 频率控制字 (DDS 步进值) |
| 0x04 | AMP_REG | 16-bit | 幅度控制 (DAC8562) |
| 0x08 | OFFSET_REG | 16-bit | 直流偏置 (DAC8562) |
| 0x0C | WAVE_SEL | 4-bit | 波形选择 (0=正弦, 1=方波, 2=三角, 3=自定义) |
| 0x10 | CONTROL | 8-bit | 控制位 (bit0=输出使能, bit1=扫频使能) |

**步骤 Steps:**

1. 在 Vivado Block Design 中添加 AXI Lite Slave IP（自定义或使用 Create IP Wizard）
2. 配置寄存器映射，连接到 PL 端的 DDS 模块
3. 在 PS 端 Linux 应用中编写寄存器读写函数：

```c
// axi_comm.h - PS-PL AXI 通信模块
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

// AXI Lite 基地址 (根据 Vivado 地址映射)
#define AXI_BASE_ADDR   0x43C00000
#define AXI_MAP_SIZE    0x1000     // 4KB 映射

// 寄存器偏移
#define REG_FREQ        0x00
#define REG_AMP         0x04
#define REG_OFFSET      0x08
#define REG_WAVE_SEL    0x0C
#define REG_CONTROL     0x10

static int mem_fd;
static volatile void *axi_map;

/**
 * 初始化 AXI 通信
 * Initialize AXI communication (map /dev/mem)
 */
int axi_init(void)
{
    mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        perror("Cannot open /dev/mem");
        return -1;
    }

    axi_map = mmap(NULL, AXI_MAP_SIZE,
                   PROT_READ | PROT_WRITE, MAP_SHARED,
                   mem_fd, AXI_BASE_ADDR);
    if (axi_map == MAP_FAILED) {
        perror("mmap failed");
        close(mem_fd);
        return -1;
    }
    return 0;
}

/**
 * 写入 AXI 寄存器
 * Write to AXI register
 */
void axi_write(uint32_t offset, uint32_t value)
{
    *((volatile uint32_t *)((char *)axi_map + offset)) = value;
}

/**
 * 读取 AXI 寄存器
 * Read from AXI register
 */
uint32_t axi_read(uint32_t offset)
{
    return *((volatile uint32_t *)((char *)axi_map + offset));
}

/**
 * 设置输出频率
 * Set output frequency
 *
 * @param freq_hz 目标频率 (Hz)
 *
 * DDS 频率公式: freq_reg = freq_hz * 2^32 / DAC_CLK
 * DAC_CLK = 200MHz (AD9744)
 */
void set_frequency(float freq_hz)
{
    // 200MSa/s DAC 时钟, 32-bit 相位累加器
    uint32_t freq_reg = (uint32_t)((freq_hz * (double)(1ULL << 32)) / 200000000.0);
    axi_write(REG_FREQ, freq_reg);
}

/**
 * 设置波形类型
 * Set waveform type
 *
 * @param type 0=正弦, 1=方波, 2=三角, 3=自定义(BRAM)
 */
void set_waveform(uint8_t type)
{
    axi_write(REG_WAVE_SEL, (uint32_t)type);
}

/**
 * 使能/禁用输出
 * Enable/disable output
 */
void output_enable(int enable)
{
    uint32_t ctrl = axi_read(REG_CONTROL);
    if (enable)
        ctrl |= 0x01;
    else
        ctrl &= ~0x01;
    axi_write(REG_CONTROL, ctrl);
}
```

4. 编译并运行测试

**预期结果 Expected Result:**
- PS 端程序能正确读写 AXI 寄存器
- 通过 `devmem` 工具可手动验证寄存器值
- 修改 WAVE_SEL 寄存器后 PL 端 DDS 模块有响应

**常见问题 Common Issues:**
- "mmap 失败 Permission denied" -- 需要以 root 权限运行，或配置 /dev/mem 权限
- "读写值不一致" -- 检查 Vivado 地址编辑器中的基地址是否与代码匹配
- "PS 端崩溃 (Bus Error)" -- AXI 地址未在设备树中正确配置

### 任务 9.2: 完整波形输出链路测试 (45 分钟)

**信号链路总览：**

```
PS端 (ARM)                    PL端 (FPGA)                     模拟
+---------+  AXI  +--------+  BRAM  +-----+  并行数据  +--------+  模拟信号  +--------+
| LVGL GUI | ---> | 寄存器  | --->  | DDS | --------> | AD9744 | --------> | OPA2673 | ---> 输出
| (参数设置)|      | (控制)  |       | 查表 |   14-bit  |  DAC   |   差分    | 运放   |    BNC
+---------+       +--------+       +-----+           +--------+           +--------+

辅助 DAC:
  DAC8562 CH0 -> 幅度控制 (VCA)
  DAC8562 CH1 -> 直流偏置
```

**测试步骤 Steps:**

1. **正弦波测试：** 设置 WAVE_SEL=0, FREQ=1kHz, 用示波器观察输出
2. **方波测试：** 设置 WAVE_SEL=1, FREQ=10kHz
3. **三角波测试：** 设置 WAVE_SEL=2, FREQ=5kHz
4. **频率扫描：** 从 100Hz 扫描到 5MHz，记录各频点输出幅度
5. **幅度测试：** 固定频率 1kHz，调节 AMP_REG 从最小到最大，记录 Vpp 变化

```bash
# 在 PS 端 Linux 命令行用 devmem 快速测试
# 写入频率寄存器 (1kHz = 0x0000A7C5 近似值)
devmem 0x43C00000 32 0x0000A7C5

# 选择正弦波
devmem 0x43C0000C 32 0x00000000

# 使能输出
devmem 0x43C00010 32 0x00000001
```

**预期结果 Expected Result:**
- 正弦波输出：1kHz 时波形清晰，THD < 3%
- 方波输出：上升/下降沿 < 50ns
- 频率范围：100Hz ~ 5MHz 可调
- 幅度范围：通过 DAC8562 控制，Vpp 可在 0~6.5V 范围调节
- 输出不超过 +/-6.5V 电源轨

**常见问题 Common Issues:**
- "输出有毛刺" -- 检查 AD9744 的参考电压是否稳定，添加去耦电容
- "高频波形失真" -- DAC 更新率 200MSa/s，接近奈奎斯特极限时正常
- "幅度不可调" -- 检查 DAC8562 SPI 通信是否正常
- "输出为零" -- 确认 CONTROL 寄存器 bit0 已置 1 (输出使能)

> **安全提示 Safety Warning:**
> TinyAWG 输出端没有内置保护电路 (PCB 面积限制)。不要将输出连接到外部电压源，不要短路输出端。输出电压范围限制在 +/-6.5V 电源轨内。
> TinyAWG has no built-in output protection due to PCB size constraints. Do not connect output to external voltage sources. Do not short the output. Output voltage is limited to +/-6.5V supply rails.

---

## 下午: PC 通信与自定义波形 | Afternoon: PC Communication & Custom Waveforms

### 任务 9.3: PC 上位机串口通信 (45 分钟)

TinyAWG 通过 USB Type-C 连接 PC，使用 USB 虚拟串口 (UART) 通信。注意：USB 串口和电源共用同一个 Type-C 口，如果需要同时供电和使用串口，需要一个 USB 分线器 (power-data splitter)。

TinyAWG connects to PC via USB Type-C using USB virtual serial port (UART). Note: USB serial and power share the same Type-C port. If you need both power and serial simultaneously, use a USB power-data splitter.

**串口通信协议：**

| 命令 Command | 格式 Format | 功能 Function |
|-------------|------------|--------------|
| SET_FREQ | `F:<freq_hz>\n` | 设置频率 |
| SET_AMP | `A:<mv>\n` | 设置幅度 (毫伏) |
| SET_OFFSET | `O:<mv>\n` | 设置偏置 (毫伏) |
| SET_WAVE | `W:<type>\n` | 设置波形类型 |
| LOAD_WAVE | `D:<len>,<v0>,<v1>,...\n` | 加载自定义波形数据 |
| SWEEP | `S:<start>,<end>,<step>,<time>\n` | 启动频率扫描 |
| STOP | `X:\n` | 停止输出 |
| GET_STATUS | `?\n` | 查询状态 |

**PS 端串口接收代码：**

```c
// uart_cmd.c - 串口命令解析模块
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

#define UART_DEVICE  "/dev/ttyUSB0"   // 或 /dev/ttyPS0 (ZYNQ 内部 UART)
#define BUF_SIZE     1024

/**
 * 解析并执行串口命令
 * Parse and execute serial command
 */
void parse_command(const char *cmd)
{
    char cmd_type = cmd[0];
    const char *param = cmd + 2;  // 跳过 "X:"

    switch (cmd_type)
    {
    case 'F':  // 设置频率
    {
        float freq = atof(param);
        set_frequency(freq);
        printf("OK FREQ=%.1fHz\n", freq);
        break;
    }
    case 'A':  // 设置幅度
    {
        int mv = atoi(param);
        axi_write(REG_AMP, (uint32_t)(mv * 65535 / 6500));  // 映射到 DAC8562
        printf("OK AMP=%dmV\n", mv);
        break;
    }
    case 'W':  // 波形选择
    {
        int type = atoi(param);
        set_waveform((uint8_t)type);
        printf("OK WAVE=%d\n", type);
        break;
    }
    case 'D':  // 自定义波形数据
    {
        // 解析逗号分隔的波形数据点
        // 写入 BRAM 供 DDS 查表使用
        int len;
        const char *p = param;
        sscanf(p, "%d,", &len);
        // 跳过长度和逗号，逐点写入 BRAM
        // ... (详见任务 9.4)
        printf("OK LOADED %d points\n", len);
        break;
    }
    case 'X':  // 停止
        output_enable(0);
        printf("OK STOPPED\n");
        break;
    case '?':  // 状态查询
        printf("STATUS: FREQ=%u AMP=%u WAVE=%u\n",
               axi_read(REG_FREQ), axi_read(REG_AMP), axi_read(REG_WAVE_SEL));
        break;
    }
}
```

**PC 端 Python 上位机 (简化版)：**

```python
# tinyawg_controller.py - PC 上位机
import serial
import time

class TinyAWG:
    def __init__(self, port='COM3', baud=115200):
        self.ser = serial.Serial(port, baud, timeout=1)

    def set_frequency(self, freq_hz):
        self.ser.write(f'F:{freq_hz}\n'.encode())
        return self.ser.readline().decode().strip()

    def set_amplitude(self, mv):
        self.ser.write(f'A:{mv}\n'.encode())
        return self.ser.readline().decode().strip()

    def set_waveform(self, wave_type):
        self.ser.write(f'W:{wave_type}\n'.encode())
        return self.ser.readline().decode().strip()

    def load_custom_wave(self, data_points):
        """加载自定义波形 - data_points 为 -32767~32767 的整数列表"""
        data_str = ','.join(map(str, data_points))
        self.ser.write(f'D:{len(data_points)},{data_str}\n'.encode())
        return self.ser.readline().decode().strip()

    def stop(self):
        self.ser.write(b'X:\n')
        return self.ser.readline().decode().strip()

# 使用示例
awg = TinyAWG('COM3')
awg.set_waveform(0)       # 正弦波
awg.set_frequency(1000)   # 1kHz
awg.set_amplitude(2000)   # 2000mVpp
```

**步骤 Steps:**
1. 在 PS 端 Linux 中运行串口命令解析程序
2. PC 端使用串口终端 (PuTTY/TeraTerm) 手动发送命令测试
3. 运行 Python 上位机程序，验证所有命令响应正常

**预期结果 Expected Result:**
- PC 发送命令后 TinyAWG 正确响应 "OK"
- 频率/幅度/波形切换实时生效
- 串口通信稳定，无乱码

**常见问题 Common Issues:**
- "串口无法打开" -- 检查 USB 驱动，Linux 下需要 `chmod 666 /dev/ttyUSB0`
- "命令无响应" -- 确认波特率一致，检查换行符是否正确 (\n)
- "同时插电源和串口有问题" -- USB 口共用电源和串口，需要 USB 分线器

### 任务 9.4: 自定义波形与手绘波形 (45 分钟)

TinyAWG 的一大特色是支持任意波形输出。原理是将自定义波形数据写入 FPGA BRAM，DDS 查表时使用 BRAM 中的数据而非内置正弦/方波/三角波查找表。

A key feature of TinyAWG is arbitrary waveform output. The principle is writing custom waveform data into FPGA BRAM, so the DDS lookup uses BRAM data instead of built-in sine/square/triangle tables.

**自定义波形加载流程：**

```
PC 上位机 -> USB 串口 -> PS (ARM) -> AXI DMA -> PL BRAM -> DDS 查表 -> DAC 输出
```

**手绘波形功能：**

```python
# PC 端手绘波形界面 (使用 matplotlib)
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Button

class HandDrawWave:
    def __init__(self, points=256):
        self.n_points = points
        self.data = np.zeros(points)

        self.fig, self.ax = plt.subplots(figsize=(10, 4))
        self.ax.set_title("Hand-Draw Waveform Editor / 手绘波形编辑器")
        self.ax.set_xlabel("Sample / 采样点")
        self.ax.set_ylabel("Amplitude / 幅度")
        self.ax.set_xlim(0, points)
        self.ax.set_ylim(-1.0, 1.0)

        self.line, = self.ax.plot(range(points), self.data, 'b-', linewidth=2)
        self.fig.canvas.mpl_connect('button_press_event', self.on_press)
        self.fig.canvas.mpl_connect('motion_notify_event', self.on_motion)
        self.fig.canvas.mpl_connect('button_release_event', self.on_release)

        # 添加发送按钮
        ax_send = plt.axes([0.8, 0.05, 0.15, 0.075])
        self.btn_send = Button(ax_send, 'Send to AWG / 发送到 AWG')
        self.btn_send.on_clicked(self.send_to_awg)

        self.drawing = False
        self.awg = TinyAWG('COM3')  # 连接设备

    def on_press(self, event):
        if event.inaxes == self.ax:
            self.drawing = True
            self._update_point(event)

    def on_motion(self, event):
        if self.drawing and event.inaxes == self.ax:
            self._update_point(event)

    def on_release(self, event):
        self.drawing = False

    def _update_point(self, event):
        idx = int(event.xdata)
        if 0 <= idx < self.n_points:
            self.data[idx] = max(-1.0, min(1.0, event.ydata))
            self.line.set_ydata(self.data)
            self.fig.canvas.draw()

    def send_to_awg(self, event):
        """将手绘波形数据量化为 16-bit 并发送到 TinyAWG"""
        quantized = (self.data * 32767).astype(int)
        self.awg.set_waveform(3)  # 选择自定义波形模式
        self.awg.load_custom_wave(quantized.tolist())
        print("Waveform sent! / 波形已发送!")

    def show(self):
        plt.show()

# 启动手绘编辑器
editor = HandDrawWave(256)
editor.show()
```

**步骤 Steps:**
1. 在 PC 端运行手绘波形编辑器
2. 用鼠标绘制任意波形形状
3. 点击 "Send to AWG" 按钮发送
4. 用示波器观察 TinyAWG 输出是否与手绘波形一致

**预期结果 Expected Result:**
- 手绘波形能正确加载到 BRAM
- 输出波形与手绘形状基本一致 (允许一定量化误差)
- 自定义波形频率可调

**常见问题 Common Issues:**
- "波形有台阶感" -- 采样点数太少 (256 点)，增加 BRAM 深度可改善
- "手绘数据发送后波形不对" -- 检查数据格式，确认有符号/无符号转换正确
- "PC 端连接 AWG 超时" -- 确认串口号和波特率正确

### 任务 9.5: 完整系统示波器验证 (30 分钟)

**系统验证清单：**

| 测试项 Test Item | 条件 Condition | 通过标准 Pass Criteria |
|-----------------|---------------|----------------------|
| 正弦波输出 | 1kHz, 1Vpp | 波形清晰，无明显失真 |
| 方波输出 | 10kHz, 2Vpp | 上升沿 < 100ns |
| 三角波输出 | 5kHz, 1Vpp | 线性度好 |
| 自定义波形 | 手绘心形波 | 形状与输入一致 |
| 频率范围 | 100Hz ~ 5MHz | 全范围可调 |
| 幅度控制 | DAC8562 调节 | 0~6.5Vpp 可控 |
| 偏置控制 | DAC8562 调节 | +/-3V 可调 |
| PC 通信 | 串口命令 | 响应正确，延迟 < 100ms |
| GUI 显示 | LVGL 触摸操作 | 参数实时更新 |

**步骤 Steps:**
1. 按上表逐项测试，用示波器截图记录
2. 记录每项测试的实际测量值
3. 整理测试报告

---

## 今日作业 | Homework

1. **实验报告：** 记录完整波形链路测试结果，包括各频率点的输出幅度和波形截图
2. **计算题：** AD9744 DAC 时钟 200MHz，32-bit 相位累加器，输出 1MHz 正弦波的频率控制字是多少？频率分辨率是多少？
3. **编程题：** 在 Python 上位机中添加一个"波形库"功能，预存 5 种常用波形 (正弦、方波、三角、锯齿、脉冲)，一键加载
4. **思考题：** TinyAWG 输出没有保护电路，如果有人不小心把输出接到 12V 电源上会发生什么？你会怎么设计一个简单的输出保护电路？

---

## 明日预告 | Tomorrow's Preview

明天开始 Phase 3 硬件实战！我们将拿到的 PCB 板进行组装焊接。先从电源部分开始 (LMR544006 DCDC、充电芯片、升压芯片)，再到信号链路 (AD9744、OPA2673、DAC8562)。焊接是硬件工程师的基本功，今天练好，明天用得上！

Tomorrow begins Phase 3 hardware practice! We will assemble and solder the PCB, starting with the power section (LMR544006 DCDC, charging IC, boost IC), then the signal path (AD9744, OPA2673, DAC8562). Soldering is a fundamental skill for hardware engineers!

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| TinyAWG 开源项目 (OSHWhub) | 开源硬件 | https://oshwhub.com/greentor/tinyawg-signal-source |
| TinyAWG 搭建文档 (飞书) | 技术文档 | https://ai.feishu.cn/docx/BGycd4URIot8Aoxs3vLcBdq2nFc |
| ZYNQ AXI 通信教程 | B站搜索 | 搜索"ZYNQ AXI Lite PS PL 通信" |
| AD9744 数据手册 | PDF | 搜索"AD9744 datasheet" |
| OPA2673 数据手册 | PDF | 搜索"OPA2673IRGVT datasheet" |
| DAC8562 SPI DAC | PDF | 搜索"DAC8562 datasheet" |

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
