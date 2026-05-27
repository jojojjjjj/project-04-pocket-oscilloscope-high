# Day 05: ARM 软件开发基础 | ARM Software Development Basics

> **今日目标 Today's Objectives:**
> - 搭建 Vitis IDE 开发环境，创建 PS 端 C 工程
> - 理解 CMake 构建系统和 PS 软件架构分层
> - 掌握 GPIO 驱动基础：LED 控制、继电器控制
> - 编写 SPI 驱动与 I2C 驱动，完成 WS2812 LED 和 MAX17048 电池读数测试
>
> **产出 Deliverable:** PS 端程序运行，串口输出调试信息，LED 闪烁，读取电池电量并通过串口打印

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 Morning Meeting | Phase 1 回顾 + Phase 2 目标 |
| 09:30-10:30 | 知识讲解 Lecture | ZYNQ PS 开发环境 + 软件架构 |
| 10:30-10:45 | 休息 Break | Break |
| 10:45-12:00 | 动手实验 Hands-on | Vitis 工程创建 + GPIO 驱动 |
| 12:00-13:30 | 午餐 Lunch | Lunch + Break |
| 13:30-15:00 | 动手实验 Hands-on | SPI 驱动 (WS2812) + I2C 驱动 (MAX17048) |
| 15:00-15:15 | 休息 Break | Break |
| 15:15-16:30 | 动手实验 Hands-on | 综合测试：LED + 电池 + 串口 |
| 16:30-17:00 | 回顾 Review | 今日总结 + 答疑 |

---

## 上午: PS 开发环境与 GPIO 驱动 | Morning: PS Development Environment & GPIO Driver

### 为什么要学这个? | Why Learn This?

在 Phase 1 中，你用 Verilog 在 FPGA (PL) 端实现了 DDS 核心逻辑。但 FPGA 本身不擅长做"决策"——它像一台不知疲倦的计算引擎，只做你告诉它的事。真正的"大脑"是 ARM Cortex-A9 处理器 (PS 端)：它负责响应用户触摸、计算频率参数、控制 DAC 输出、更新屏幕显示。

In Phase 1, you implemented DDS core logic in the FPGA (PL side) using Verilog. But the FPGA itself is not good at "making decisions" — it's like a tireless computing engine that only does what you tell it. The real "brain" is the ARM Cortex-A9 processor (PS side): it responds to user touches, computes frequency parameters, controls DAC output, and updates the screen display.

**PS 与 PL 的分工协作：**

```
┌─────────────────────────────────────────────────┐
│                  ZYNQ-7010                       │
│                                                  │
│  ┌──────────────────┐  ┌──────────────────────┐  │
│  │   PS (ARM A9)    │  │   PL (FPGA)           │  │
│  │                  │  │                        │  │
│  │  • C 程序        │  │  • DDS 相位累加器     │  │
│  │  • LVGL GUI     │  │  • 波形查找表 (BRAM)  │  │
│  │  • 触摸/显示驱动 │  │  • AXI Lite 寄存器    │  │
│  │  • SPI/I2C/GPIO │──│──• DAC 数据接口       │  │
│  │  • AXI 主机     │  │  • 200MHz 时钟域      │  │
│  └──────────────────┘  └──────────────────────┘  │
│                                                  │
└─────────────────────────────────────────────────┘
```

### 任务 5.1: Vitis 工程创建与软件架构理解 (45 分钟)

**TinyAWG PS 软件目录结构：**

```
src/
├── helloworld.c          # 主入口 main()
├── platform.c/h          # 平台抽象层 (init platform)
├── DAC8562/              # DAC8562 SPI 驱动 (幅度/偏置控制)
├── MAX17048/             # MAX17048 I2C 驱动 (电池监控)
├── display/              # LCD 显示驱动
├── gui/                  # LVGL GUI (事件处理、界面生成)
├── hardware/             # 底层硬件驱动 (GPIO, I2C, SPI, Timer)
├── lvgl/                 # LVGL 8.3.1 库
├── output_ctrl/          # 输出控制 + 用户 DDS
└── touch/                # FT6336 触控驱动
```

**为什么需要分层架构？** 把"硬件操作"和"业务逻辑"分开，就像开车不需要知道发动机怎么工作——你只需要踩油门。上层代码调用 `gpio_set(GPIO_LED, 1)` 点灯，不需要知道寄存器地址。

**步骤 Steps:**

1. 打开 Vitis IDE，导入 Day 4 导出的硬件平台 (XSA 文件)
2. 创建新的 Application Project：
   - OS Platform: **standalone** (裸机，无操作系统)
   - Template: **Hello World**
3. 在 `helloworld.c` 中找到 `main()` 函数

```c
// helloworld.c — TinyAWG 主入口
// TinyAWG main entry point

#include "platform.h"
#include "xil_printf.h"

int main()
{
    // 初始化平台 (配置缓存、UART 等)
    // Initialize platform (cache, UART, etc.)
    init_platform();

    xil_printf("\r\n--- TinyAWG Starting ---\r\n");
    xil_printf("ZYNQ-7010 PS Software v1.0\r\n");
    xil_printf("ARM Cortex-A9 @ 667 MHz\r\n\r\n");

    // 后续将在这里添加各模块初始化
    // Module initialization will be added here later

    while (1)
    {
        // 主循环：处理触摸、更新显示、控制输出
        // Main loop: handle touch, update display, control output
    }

    cleanup_platform();
    return 0;
}
```

4. 编译下载，确认串口输出 "TinyAWG Starting"

**预期结果 Expected Result:**
- Vitis 编译无错误
- 串口终端 (115200, 8N1) 显示启动信息
- `xil_printf` 输出正常

**常见问题 Common Issues:**
- "找不到 XSA 文件" -- 需要先在 Vivado 中 File > Export Hardware，勾选 Include Bitstream
- "编译报错 xil_printf 未定义" -- 确认 `#include "xil_printf.h"` 已添加
- "串口无输出" -- 检查波特率 (115200)、COM 口选择、USB 转串口驱动

### 任务 5.2: GPIO 驱动 — LED 与继电器控制 (45 分钟)

**为什么先学 GPIO？** GPIO (General Purpose I/O) 是最简单的外设——只有"高"和"低"两个状态。掌握 GPIO，你就能控制 LED、继电器、使能引脚等所有"开关型"硬件。

**TinyAWG 的 GPIO 使用：**

| GPIO 引脚 | 功能 Function | 有效电平 Active Level |
|-----------|-------------|---------------------|
| MIO 7 | 系统状态 LED | 低电平点亮 (Active Low) |
| EMIO 0 | 输出使能继电器 | 高电平有效 (Active High) |
| EMIO 1 | WS2812 数据线 | 数据协议 (PWM) |

**ZYNQ GPIO 分为两类：**

```
MIO  (Multi-use I/O):  固定引脚，共 54 个，直接连接 PS 外设
EMIO (Extended MIO):    通过 PL 引出到物理引脚，数量灵活

GPIO 操作步骤：
1. 查找 GPIO 配置 (XGpioPs_Config)
2. 初始化 GPIO (XGpioPs_CfgInitialize)
3. 设置引脚方向 (XGpioPs_SetDirectionPin)
4. 使能输出 (XGpioPs_SetOutputEnablePin)
5. 读写引脚 (XGpioPs_WritePin / XGpioPs_ReadPin)
```

```c
// hardware/gpio.c — GPIO 驱动
// GPIO driver for TinyAWG

#include "xgpiops.h"
#include "xil_printf.h"

#define GPIO_DEVICE_ID      XPAR_XGPIOPS_0_DEVICE_ID
#define GPIO_LED_PIN        7       // MIO 7 — 状态 LED
#define GPIO_RELAY_PIN      54      // EMIO 0 (MIO 编号从 54 开始) — 输出继电器

static XGpioPs gpio;                // GPIO 驱动实例

/**
 * 初始化 GPIO
 * Initialize GPIO subsystem
 */
int gpio_init(void)
{
    XGpioPs_Config *config;

    // 查找 GPIO 配置
    config = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
    if (config == NULL)
    {
        xil_printf("ERROR: GPIO config not found\r\n");
        return XST_FAILURE;
    }

    // 初始化 GPIO 驱动
    int status = XGpioPs_CfgInitialize(&gpio, config, config->BaseAddr);
    if (status != XST_SUCCESS)
    {
        xil_printf("ERROR: GPIO init failed\r\n");
        return XST_FAILURE;
    }

    // 配置 LED 引脚为输出
    XGpioPs_SetDirectionPin(&gpio, GPIO_LED_PIN, 1);     // 1 = 输出
    XGpioPs_SetOutputEnablePin(&gpio, GPIO_LED_PIN, 1);  // 1 = 使能输出

    // 配置继电器引脚为输出
    XGpioPs_SetDirectionPin(&gpio, GPIO_RELAY_PIN, 1);
    XGpioPs_SetOutputEnablePin(&gpio, GPIO_RELAY_PIN, 1);

    xil_printf("GPIO initialized OK\r\n");
    return XST_SUCCESS;
}

/**
 * 控制 LED 亮灭
 * Control LED on/off
 *
 * @param on  1=点亮, 0=熄灭
 */
void gpio_set_led(int on)
{
    // LED 低电平有效：写入 0 点亮，写入 1 熄灭
    XGpioPs_WritePin(&gpio, GPIO_LED_PIN, on ? 0 : 1);
}

/**
 * 控制输出继电器
 * Control output relay
 *
 * @param enable  1=使能输出, 0=断开输出
 */
void gpio_set_relay(int enable)
{
    XGpioPs_WritePin(&gpio, GPIO_RELAY_PIN, enable ? 1 : 0);
}
```

**在 main() 中测试 LED 闪烁：**

```c
#include "sleep.h"    // 提供 usleep() 微秒延时

int main()
{
    init_platform();
    xil_printf("--- TinyAWG Starting ---\r\n");

    gpio_init();

    xil_printf("LED blink test: 10 blinks\r\n");
    for (int i = 0; i < 10; i++)
    {
        gpio_set_led(1);         // 点亮
        usleep(200000);          // 延时 200ms
        gpio_set_led(0);         // 熄灭
        usleep(200000);
    }
    xil_printf("LED test done.\r\n");

    while (1)
    {
        // 主循环
    }

    cleanup_platform();
    return 0;
}
```

**步骤 Steps:**
1. 创建 `hardware/gpio.c` 和 `hardware/gpio.h`
2. 在 `helloworld.c` 中调用 `gpio_init()` 和 LED 闪烁测试
3. 编译下载，观察 LED 是否闪烁 10 次后停止

**预期结果 Expected Result:**
- LED 闪烁 10 次，每次间隔约 0.4 秒
- 串口打印 "LED test done."

**常见问题 Common Issues:**
- "LED 不亮" -- 确认 LED 引脚编号和有效电平（有的板子是高电平点亮）
- "编译找不到 xgpiops.h" -- 确认 standalone BSP 包含了 GPIO 驱动
- "EMIO 引脚无反应" -- 确认 Vivado 中已将 EMIO 连接到 PL 端物理引脚，且 Bitstream 已下载

---

## 下午: SPI 与 I2C 驱动开发 | Afternoon: SPI & I2C Driver Development

### 为什么要学这个? | Why Learn This?

GPIO 只能做"开/关"控制。但 TinyAWG 上的很多外设需要传输**数据**：WS2812 RGB LED 需要 SPI 发送颜色数据；MAX17048 电量计需要 I2C 读取电池电压；DAC8562 需要 SPI 设置幅度和偏置。掌握 SPI 和 I2C 两种最常用的通信总线，你就能驱动几乎所有的外设芯片。

GPIO only does "on/off" control. But many peripherals on TinyAWG need to transfer **data**: WS2812 RGB LED needs SPI for color data; MAX17048 fuel gauge needs I2C for battery voltage; DAC8562 needs SPI for amplitude and offset. Mastering SPI and I2C, the two most common communication buses, enables you to drive almost any peripheral chip.

**SPI vs I2C 对比：**

| 特性 Feature | SPI | I2C |
|-------------|-----|-----|
| 信号线 Signal Lines | 4 线 (SCK, MOSI, MISO, CS) | 2 线 (SCL, SDA) |
| 速度 Speed | 高速 (几十 MHz) | 较慢 (100k~400kHz) |
| 地址 Addressing | 硬件片选 (CS 引脚) | 软件地址 (7-bit 地址) |
| 全双工 Full Duplex | 是 | 否 |
| TinyAWG 用途 | DAC8562, WS2812 | MAX17048, FT6336 |

### 任务 5.3: SPI 驱动 — WS2812 RGB LED (45 分钟)

**WS2812 的工作原理：** 每个 WS2812 LED 接收 24-bit 颜色数据 (G8-R8-B8)。数据协议使用"高电平时间长短"来区分 0 和 1：

```
WS2812 时序 (数据编码):
  Bit '1':  高电平 0.7us + 低电平 0.6us ≈ 1.25us
  Bit '0':  高电平 0.35us + 低电平 0.8us ≈ 1.15us
  Reset:    低电平 > 50us
```

TinyAWG 使用 PS 端 SPI 外设，以 5MHz 时钟发送数据来模拟 WS2812 时序。每个 WS2812 bit 用 3 个 SPI bit 表示。

```c
// hardware/spi.c — SPI 驱动 (WS2812 控制)
// SPI driver for WS2812 RGB LED control

#include "xspips.h"
#include "xil_printf.h"

#define SPI_DEVICE_ID       XPAR_XSPIPS_0_DEVICE_ID
#define WS2812_SPI_FREQ_HZ  5000000    // 5 MHz SPI 时钟

static XSpiPs spi;

/**
 * 初始化 SPI
 * Initialize SPI peripheral
 */
int spi_init(void)
{
    XSpiPs_Config *config;

    config = XSpiPs_LookupConfig(SPI_DEVICE_ID);
    if (config == NULL)
    {
        xil_printf("ERROR: SPI config not found\r\n");
        return XST_FAILURE;
    }

    int status = XSpiPs_CfgInitialize(&spi, config, config->BaseAddress);
    if (status != XST_SUCCESS)
    {
        xil_printf("ERROR: SPI init failed\r\n");
        return XST_FAILURE;
    }

    // 设置 SPI 时钟频率
    XSpiPs_SetClkPrescaler(&spi, XSPIPS_CLK_PRESCALE_8);  // 5 MHz

    // 配置 SPI: 主模式, CPOL=0, CPHA=0
    XSpiPs_SetOptions(&spi, XSPIPS_MASTER_OPTION |
                            XSPIPS_CLK_ACTIVE_LOW_OPTION);
    XSpiPs_SetOptions(&spi, XSPIPS_MASTER_OPTION);  // CPOL=0, CPHA=0

    xil_printf("SPI initialized OK\r\n");
    return XST_SUCCESS;
}

// WS2812 颜色编码查找表
// 3 SPI bits per WS2812 bit: '1' → 0b110, '0' → 0b100
static const uint8_t ws2812_bit_encode[2] = { 0x04, 0x06 };  // 0b100, 0b110

/**
 * 设置 WS2812 LED 颜色
 * Set WS2812 LED color
 *
 * @param red    红色分量 (0~255)
 * @param green  绿色分量 (0~255)
 * @param blue   蓝色分量 (0~255)
 */
void ws2812_set_color(uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t tx_buf[24];    // 24-bit color = 24 bytes (1 byte per WS2812 bit)
    int idx = 0;

    // WS2812 数据顺序: Green → Red → Blue, 每字节高位先发
    // Data order: Green → Red → Blue, MSB first
    uint8_t colors[3] = { green, red, blue };

    for (int c = 0; c < 3; c++)
    {
        for (int bit = 7; bit >= 0; bit--)
        {
            int val = (colors[c] >> bit) & 0x01;
            tx_buf[idx++] = ws2812_bit_encode[val];
        }
    }

    // 发送数据
    XSpiPs_PolledTransfer(&spi, tx_buf, NULL, 24);

    // 发送 Reset 信号 (低电平 > 50us)
    // 通过延时实现
    usleep(60);

    xil_printf("WS2812: R=%d G=%d B=%d\r\n", red, green, blue);
}
```

**测试：让 WS2812 呼吸闪烁**

```c
// 在 main() 中
spi_init();

xil_printf("WS2812 breathing test\r\n");
for (int cycle = 0; cycle < 5; cycle++)
{
    // 渐亮 fade in
    for (int i = 0; i <= 255; i += 5)
    {
        ws2812_set_color(i, 0, 0);     // 红色渐亮
        usleep(20000);                   // 20ms
    }
    // 渐暗 fade out
    for (int i = 255; i >= 0; i -= 5)
    {
        ws2812_set_color(i, 0, 0);
        usleep(20000);
    }
}
```

**步骤 Steps:**
1. 创建 `hardware/spi.c` 和 `hardware/spi.h`
2. 实现 SPI 初始化和 WS2812 颜色设置函数
3. 在 main() 中调用呼吸闪烁测试
4. 编译下载，观察 WS2812 LED 呼吸效果

**预期结果 Expected Result:**
- WS2812 LED 红色呼吸闪烁 5 个周期
- 每次颜色变化时串口打印 RGB 值

**常见问题 Common Issues:**
- "LED 颜色不对/花色" -- WS2812 数据顺序是 G-R-B 而非 R-G-B，检查颜色字节顺序
- "LED 不亮" -- SPI 时钟频率不对；WS2812 需要 5MHz SPI 才能正确编码
- "LED 全白/全亮" -- 数据全被识别为 '1'，检查 SPI 极性配置 (CPOL/CPHA)

### 任务 5.4: I2C 驱动 — MAX17048 电池监控 (45 分钟)

**MAX17048 是什么？** 它是一个锂电池电量计芯片，通过 I2C 接口提供：
- 电池电压 (0~5V，精度 1.25mV)
- 电池剩余电量百分比 (State of Charge, 0~100%)
- 充放电速率

**I2C 通信基础：**

```
I2C 通信流程 (以读取电池电压为例):

主机 (PS)                     从机 (MAX17048)
  │                              │
  │── START ───────────────────→ │
  │── 地址 (0x36) + W ────────→ │    // 写模式，先设寄存器地址
  │── 寄存器地址 (0x02) ──────→ │    // VCELL 寄存器
  │── RE-START ───────────────→ │
  │── 地址 (0x36) + R ────────→ │    // 切换到读模式
  │←── 数据高字节 ────────────── │
  │←── 数据低字节 ────────────── │
  │── STOP ────────────────────→ │
```

```c
// MAX17048/max17048.c — MAX17048 电池监控驱动
// MAX17048 battery gauge driver

#include "xiicps.h"
#include "xil_printf.h"

#define I2C_DEVICE_ID       XPAR_XIICPS_0_DEVICE_ID
#define MAX17048_ADDR       0x36    // MAX17048 I2C 地址 (7-bit)

// MAX17048 寄存器地址
#define MAX17048_REG_VCELL  0x02    // 电池电压 (12-bit, 单位 1.25mV)
#define MAX17048_REG_SOC    0x04    // 剩余电量百分比 (16-bit)
#define MAX17048_REG_MODE   0x06    // 模式寄存器
#define MAX17048_REG_VERSION 0x08   // 芯片版本

static XIicPs i2c;

/**
 * 初始化 I2C
 * Initialize I2C peripheral
 */
int i2c_init(void)
{
    XIicPs_Config *config;

    config = XIicPs_LookupConfig(I2C_DEVICE_ID);
    if (config == NULL)
    {
        xil_printf("ERROR: I2C config not found\r\n");
        return XST_FAILURE;
    }

    int status = XIicPs_CfgInitialize(&i2c, config, config->BaseAddress);
    if (status != XST_SUCCESS)
    {
        xil_printf("ERROR: I2C init failed\r\n");
        return XST_FAILURE;
    }

    // 设置 I2C 时钟频率为 400kHz (快速模式)
    XIicPs_SetSClk(&i2c, 400000);

    xil_printf("I2C initialized OK\r\n");
    return XST_SUCCESS;
}

/**
 * 从 MAX17048 读取 16-bit 寄存器
 * Read 16-bit register from MAX17048
 *
 * @param reg  寄存器地址
 * @param val  读取值 (输出)
 * @return     XST_SUCCESS or XST_FAILURE
 */
int max17048_read_reg(uint8_t reg, uint16_t *val)
{
    uint8_t buf[2];
    int status;

    // 发送寄存器地址 (写操作)
    status = XIicPs_MasterSendPolled(&i2c, &reg, 1, MAX17048_ADDR);
    if (status != XST_SUCCESS) return status;

    // 等待发送完成
    while (XIicPs_BusIsBusy(&i2c));

    // 读取 2 字节数据
    status = XIicPs_MasterRecvPolled(&i2c, buf, 2, MAX17048_ADDR);
    if (status != XST_SUCCESS) return status;

    while (XIicPs_BusIsBusy(&i2c));

    // MAX17048 数据是大端序 (Big-Endian)
    *val = ((uint16_t)buf[0] << 8) | buf[1];
    return XST_SUCCESS;
}

/**
 * 读取电池电压 (单位: mV)
 * Read battery voltage in millivolts
 */
float max17048_read_voltage(void)
{
    uint16_t raw;
    if (max17048_read_reg(MAX17048_REG_VCELL, &raw) != XST_SUCCESS)
    {
        xil_printf("ERROR: Failed to read VCELL\r\n");
        return -1.0f;
    }

    // VCELL 寄存器: 12-bit, 分辨率 1.25mV
    float voltage_mv = (raw >> 4) * 1.25f;
    return voltage_mv;
}

/**
 * 读取电池剩余电量百分比
 * Read battery State of Charge percentage
 */
float max17048_read_soc(void)
{
    uint16_t raw;
    if (max17048_read_reg(MAX17048_REG_SOC, &raw) != XST_SUCCESS)
    {
        xil_printf("ERROR: Failed to read SOC\r\n");
        return -1.0f;
    }

    // SOC 寄存器: 高字节为整数百分比, 低字节为小数部分
    float soc = (raw >> 8) + (float)(raw & 0xFF) / 256.0f;
    return soc;
}
```

**测试：读取电池电压和电量**

```c
// 在 main() 中
i2c_init();

xil_printf("--- Battery Monitor Test ---\r\n");

// 读取芯片版本确认通信正常
uint16_t version;
if (max17048_read_reg(MAX17048_REG_VERSION, &version) == XST_SUCCESS)
{
    xil_printf("MAX17048 version: 0x%04X\r\n", version);
}
else
{
    xil_printf("WARNING: MAX17048 not detected, check wiring!\r\n");
}

// 持续读取电池状态
for (int i = 0; i < 20; i++)
{
    float voltage = max17048_read_voltage();
    float soc = max17048_read_soc();
    xil_printf("Battery: %.1f mV, SOC: %.1f%%\r\n", voltage, soc);
    usleep(2000000);    // 每 2 秒读一次
}
```

**步骤 Steps:**
1. 创建 `MAX17048/max17048.c` 和 `MAX17048/max17048.h`
2. 实现 I2C 初始化和 MAX17048 寄存器读取函数
3. 在 main() 中添加电池读取测试
4. 编译下载，观察串口输出的电池电压和电量

**预期结果 Expected Result:**
- 串口打印 MAX17048 芯片版本号 (通常为 0x0010~0x0012)
- 电池电压读数约 3500~4200mV (取决于电池电量)
- SOC 读数 0~100%

**常见问题 Common Issues:**
- "MAX17048 not detected" -- I2C 地址错误，确认是 0x36 而非 0x6C (8-bit 地址)
- "电压读数为 0" -- SDA/SCL 接反或上拉电阻缺失 (I2C 需要外部上拉)
- "读数跳动很大" -- MAX17048 上电后需要约 500ms 完成首次测量，加延时再读

### 任务 5.5: 综合测试 (30 分钟)

将 LED 闪烁、WS2812、MAX17048 三个模块整合到一个完整的 main 函数中：

```c
int main()
{
    init_platform();
    xil_printf("\r\n=== TinyAWG v1.0 ===\r\n");
    xil_printf("Phase 2 Day 5: PS Driver Test\r\n\r\n");

    // 初始化所有外设
    gpio_init();
    spi_init();
    i2c_init();

    xil_printf("All peripherals initialized.\r\n\r\n");

    // 主循环
    int loop_count = 0;
    while (1)
    {
        // LED 心跳指示 (每秒翻转)
        gpio_set_led(loop_count % 2);

        // 每 2 秒读取电池电量
        if (loop_count % 2 == 0)
        {
            float voltage = max17048_read_voltage();
            float soc = max17048_read_soc();
            xil_printf("[%d] Battery: %.0f mV (%.0f%%)\r\n",
                       loop_count, voltage, soc);
        }

        usleep(1000000);    // 1 秒
        loop_count++;
    }

    cleanup_platform();
    return 0;
}
```

**步骤 Steps:**
1. 整合所有模块到 main() 函数
2. 编译下载，运行综合测试
3. 记录串口输出和 LED 状态

**预期结果 Expected Result:**
- LED 每秒闪烁一次 (心跳)
- 串口每 2 秒输出一次电池信息
- WS2812 保持初始颜色

---

## 今日作业 | Homework

1. **代码题：** 为 GPIO 驱动添加按键输入检测功能（配置 MIO 为输入，读取按键状态并打印），注意上拉/下拉配置
2. **实验题：** 测试 WS2812 三种颜色 (R, G, B) 分别点亮，观察混色效果；记录 SPI 频率对 WS2812 显示的影响
3. **思考题：** ZYNQ 的 MIO 和 EMIO 有什么区别？为什么 WS2812 数据线通常使用 EMIO 而不是 MIO？
4. **写实验日志：** 记录 PS 端各驱动的调试过程，特别是遇到的问题和解决方法

---

## 明日预告 | Tomorrow's Preview

今天你让 ARM 处理器"活"了起来——能控制 LED、读电池、发 SPI。明天我们进入核心功能：驱动 AD9744 高速 DAC 和 DAC8562 精密 DAC，让 TinyAWG 真正输出模拟波形！你将亲手产生正弦波、方波、三角波，并用示波器验证输出。

Today you brought the ARM processor to life — controlling LEDs, reading battery, sending SPI. Tomorrow we enter the core functionality: driving the AD9744 high-speed DAC and DAC8562 precision DAC, making TinyAWG output real analog waveforms! You will generate sine, square, and triangle waves and verify them with an oscilloscope.

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| ZYNQ PS GPIO 编程指南 | Xilinx Doc | Xilinx Ug585 (ZYNQ TRM) Chapter 13 |
| XSpiPs 驱动 API 文档 | Xilinx Doc | Vitis 帮助文档 > SPI PS Driver |
| XIicPs 驱动 API 文档 | Xilinx Doc | Vitis 帮助文档 > IIC PS Driver |
| WS2812 数据手册 | PDF | 搜索 "WS2812B datasheet" |
| MAX17048 数据手册 | PDF | 搜索 "MAX17048 datasheet" |
| ZYNQ 嵌入式开发教程 | B站视频 | 搜索 "ZYNQ Vitis standalone 开发" |

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
