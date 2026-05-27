/**
 * ============================================================================
 * DAC 驱动模块 | DAC Driver Module
 * ============================================================================
 *
 * 驱动两颗 DAC 芯片，分别用于不同的功能:
 * Drives two DAC chips for different functions:
 *
 *   1. DAC8562 (16-bit 双通道, SPI)
 *      - 通道 A: 控制输出幅度 (0~10Vpp)
 *      - 通道 B: 控制输出偏置 (±5V)
 *      - SPI 接口, 24-bit 数据帧
 *
 *   2. AD9744 (14-bit 高速 DAC)
 *      - 接收 FPGA DDS 输出的 14-bit 并行数据
 *      - 210 MSPS 更新速率
 *      - 由 FPGA 直接驱动，PS 端仅需配置相关参数
 *
 * ============================================================
 * DAC8562 SPI 通信协议 | DAC8562 SPI Communication Protocol
 * ============================================================
 *
 * DAC8562 是 Texas Instruments 的 16-bit 双通道 DAC，特点:
 * DAC8562 is a Texas Instruments 16-bit dual-channel DAC, features:
 *   - 16-bit 分辨率 (65536 级)
 *   - 双通道独立输出 (Channel A + Channel B)
 *   - SPI 串行接口 (最高 50 MHz)
 *   - 内置 2.5V 基准电压源
 *   - 输出范围: 0 ~ 2*Vref = 0 ~ 5.0V
 *
 * SPI 数据帧格式 (24-bit) | SPI data frame format (24-bit):
 *
 *   Bit 23-20: 命令字节 Command byte
 *     0000 = 写入输入寄存器并更新输出 (Write to & update)
 *     0001 = 写入输入寄存器 (Write input register only)
 *     0011 = 写入并更新特定通道
 *
 *   Bit 17-16: 地址字节 Address byte
 *     00 = 通道 A (幅度)
 *     01 = 通道 B (偏置)
 *
 *   Bit 15-0: 16-bit DAC 数据值 (MSB first)
 *
 * SPI 时序要求 SPI timing requirements:
 *   CPOL = 0, CPHA = 1 (Mode 1)
 *   CS 低电平有效，帧间拉高
 *   MSB first
 *
 * ============================================================
 * AD9744 数据格式说明 | AD9744 Data Format Notes
 * ============================================================
 *
 * AD9744 是 Analog Devices 的 14-bit 210MSPS DAC:
 * AD9744 is an Analog Devices 14-bit 210MSPS DAC:
 *
 *   - 数据输入: 14-bit 并行接口 (D13~D0)
 *   - 时钟: 外部时钟输入，最高 210 MHz
 *   - 输出模式: 电流输出 (需外接电阻转电压)
 *   - 数据格式: 偏移二进制 (Offset Binary) 或二进制补码 (Two's Complement)
 *
 * 14-bit 数据编码 (偏移二进制) | 14-bit data encoding (offset binary):
 *   0x0000 (0)      -> 负满量程 (-FS)
 *   0x1FFF (8191)   -> 零点 (Mid-scale)
 *   0x3FFF (16383)  -> 正满量程 (+FS)
 *
 * 14-bit 数据编码 (二进制补码) | 14-bit data encoding (two's complement):
 *   0x2000 (-8192)  -> 负满量程 (-FS)
 *   0x0000 (0)      -> 零点 (Mid-scale)
 *   0x1FFF (8191)   -> 正满量程 (+FS)
 *
 * 在 TinyAWG 中，FPGA DDS 输出使用偏移二进制格式，
 * 波形数据范围 0~16383 直接映射到 AD9744 的输入。
 *
 * In TinyAWG, FPGA DDS output uses offset binary format,
 * waveform data range 0~16383 maps directly to AD9744 input.
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "xil_types.h"
#include "xstatus.h"
#include "dac_driver.h"


/* ============================================================
 * 常量定义 | Constant Definitions
 * ============================================================ */

/* DAC8562 SPI 命令定义 | DAC8562 SPI command definitions */
#define DAC8562_CMD_WRITE_UPDATE   0x00   /* 写入并更新 | Write and update */
#define DAC8562_CMD_WRITE_ONLY     0x01   /* 仅写入 | Write only */
#define DAC8562_CMD_WRITE_CH       0x03   /* 写入指定通道 | Write specific channel */

/* DAC8562 地址定义 | DAC8562 address definitions */
#define DAC8562_ADDR_CH_A          0x00   /* 通道 A (幅度) | Channel A (amplitude) */
#define DAC8562_ADDR_CH_B          0x01   /* 通道 B (偏置) | Channel B (offset) */

/* DAC8562 控制命令 | DAC8562 control commands */
#define DAC8562_CTRL_PWRUP_NONE    0x0000  /* 两个通道都上电 | Power up both channels */
#define DAC8562_CTRL_PWRUP_CH_A    0x0100  /* 仅上电 CH A | Power up CH A only */
#define DAC8562_CTRL_PWRUP_CH_B    0x0200  /* 仅上电 CH B | Power up CH B only */
#define DAC8562_CTRL_RESET         0x0300  /* 软复位 | Software reset */
#define DAC8562_CTRL_LDAC          0x0400  /* LDAC 控制 | LDAC control */
#define DAC8562_CTRL_ENABLE_REF    0x0500  /* 使能内部基准 | Enable internal reference */

/* AD9744 参数 | AD9744 parameters */
#define AD9744_RESOLUTION          14       /* 14-bit 分辨率 */
#define AD9744_FULL_SCALE          16383    /* 2^14 - 1 = 16383 */
#define AD9744_MID_SCALE           8191     /* 中点值 | Mid-scale value */


/* ============================================================
 * SPI 底层通信函数 | SPI Low-level Communication Functions
 * ============================================================ */

/**
 * SPI 发送 24-bit 数据帧到 DAC8562
 * Send 24-bit data frame to DAC8562 via SPI
 *
 * DAC8562 SPI 时序 DAC8562 SPI timing:
 *   1. 拉低 CS (片选)
 *   2. 发送 24-bit 数据 (MSB first)
 *   3. 拉高 CS
 *
 * @param data  24-bit 数据 (包含命令+地址+DAC值)
 */
static void spi_send_24bit(uint32_t data)
{
    /*
     * TODO: 使用 Xilinx SPI 驱动发送数据
     * TODO: Send data using Xilinx SPI driver
     *
     * 实现方式一 Implementation 1: 使用 XSpi 驱动
     *   uint8_t tx_buf[3];
     *   tx_buf[0] = (data >> 16) & 0xFF;  // 命令+地址
     *   tx_buf[1] = (data >> 8)  & 0xFF;  // 数据高字节
     *   tx_buf[2] =  data        & 0xFF;  // 数据低字节
     *   XSpi_Transfer(&spi_inst, tx_buf, NULL, 3);
     *
     * 实现方式二 Implementation 2: GPIO 模拟 SPI (bit-banging)
     *   用于调试或 SPI 硬件不可用时
     *   for (int bit = 23; bit >= 0; bit--) {
     *       gpio_write(SPI_MOSI_PIN, (data >> bit) & 1);
     *       gpio_write(SPI_CLK_PIN, 1);
     *       gpio_write(SPI_CLK_PIN, 0);
     *   }
     *
     * 这里使用占位实现 | Placeholder implementation here:
     */
    (void)data;

    /* 调试输出 | Debug output */
    /* printf("[SPI] TX: 0x%06X\r\n", data); */
}


/* ============================================================
 * DAC8562 公共函数 | DAC8562 Public Functions
 * ============================================================ */

/**
 * 初始化 DAC8562
 * Initialize DAC8562
 *
 * 初始化序列 Init sequence:
 *   1. 软复位 (清除所有寄存器)
 *   2. 使能内部 2.5V 基准电压
 *   3. 上电两个通道
 *   4. 设置零输出
 *
 * @return XST_SUCCESS 或 XST_FAILURE
 */
int dac8562_init(void)
{
    /*
     * Step 1: 软复位 | Software reset
     *
     * 24-bit 帧: 命令=RESET, 数据=0x0000
     * 24-bit frame: command=RESET, data=0x0000
     */
    spi_send_24bit((DAC8562_CTRL_RESET << 16) | 0x0000);

    /*
     * Step 2: 使能内部基准电压源
     * Enable internal voltage reference
     *
     * DAC8562 有内部 2.5V 基准。如果使能内部基准，
     * 输出范围 = 0 ~ 2 * 2.5V = 0 ~ 5.0V
     *
     * DAC8562 has internal 2.5V reference. If enabled,
     * output range = 0 ~ 2 * 2.5V = 0 ~ 5.0V
     */
    spi_send_24bit((DAC8562_CTRL_ENABLE_REF << 16) | 0x0001);

    /*
     * Step 3: 上电两个通道 | Power up both channels
     */
    spi_send_24bit((DAC8562_CTRL_PWRUP_NONE << 16) | 0x0000);

    /*
     * Step 4: 零输出 | Zero output
     */
    dac8562_set_channel_a(0);               /* 通道 A = 0V */
    dac8562_set_channel_b(DAC8562_MID_CODE); /* 通道 B = 中点 */

    printf("[DAC8562] 初始化完成\r\n");
    return XST_SUCCESS;
}

/**
 * 设置 DAC8562 通道 A 输出 (幅度控制)
 * Set DAC8562 channel A output (amplitude control)
 *
 * @param code  16-bit DAC 码值 (0~65535)
 *              0 = 0V, 65535 = 5.0V (满量程)
 */
void dac8562_set_channel_a(uint16_t code)
{
    /*
     * 构建 24-bit SPI 数据帧 | Build 24-bit SPI data frame
     *
     * [23:20] 命令 = 0011 (写入并更新指定通道)
     * [17:16] 地址 = 00   (通道 A)
     * [15:0]  数据 = 16-bit DAC 值
     *
     * [23:20] Command = 0011 (write to & update specific channel)
     * [17:16] Address = 00   (Channel A)
     * [15:0]  Data    = 16-bit DAC value
     */
    uint32_t frame = ((DAC8562_CMD_WRITE_CH & 0xF) << 20)
                   | ((DAC8562_ADDR_CH_A & 0x3) << 16)
                   | (code & 0xFFFF);

    spi_send_24bit(frame);
}

/**
 * 设置 DAC8562 通道 B 输出 (偏置控制)
 * Set DAC8562 channel B output (offset control)
 *
 * @param code  16-bit DAC 码值 (0~65535)
 *              0 = 0V, 32767 = 2.5V (中点), 65535 = 5.0V
 */
void dac8562_set_channel_b(uint16_t code)
{
    /*
     * 构建 24-bit SPI 数据帧 (通道 B)
     * Build 24-bit SPI data frame (Channel B)
     *
     * 与通道 A 相同，仅地址位不同:
     * Same as Channel A, only address bits differ:
     *   [17:16] = 01 (通道 B | Channel B)
     */
    uint32_t frame = ((DAC8562_CMD_WRITE_CH & 0xF) << 20)
                   | ((DAC8562_ADDR_CH_B & 0x3) << 16)
                   | (code & 0xFFFF);

    spi_send_24bit(frame);
}

/**
 * 同时更新两个通道 (使用 LDAC)
 * Update both channels simultaneously (using LDAC)
 *
 * 先分别写入两个通道的输入寄存器，然后通过 LDAC 脉冲
 * 同时更新输出，保证幅度和偏置同步变化。
 *
 * Write to both input registers separately, then issue LDAC
 * pulse to update outputs simultaneously, ensuring amplitude
 * and offset change synchronously.
 *
 * @param code_a  通道 A 码值
 * @param code_b  通道 B 码值
 */
void dac8562_set_both(uint16_t code_a, uint16_t code_b)
{
    /* 写入通道 A 输入寄存器 (不更新输出) */
    /* Write to Channel A input register (don't update output) */
    uint32_t frame_a = ((DAC8562_CMD_WRITE_ONLY & 0xF) << 20)
                     | ((DAC8562_ADDR_CH_A & 0x3) << 16)
                     | (code_a & 0xFFFF);
    spi_send_24bit(frame_a);

    /* 写入通道 B 输入寄存器 (不更新输出) */
    /* Write to Channel B input register (don't update output) */
    uint32_t frame_b = ((DAC8562_CMD_WRITE_ONLY & 0xF) << 20)
                     | ((DAC8562_ADDR_CH_B & 0x3) << 16)
                     | (code_b & 0xFFFF);
    spi_send_24bit(frame_b);

    /* LDAC 脉冲: 同时更新两个通道输出 */
    /* LDAC pulse: update both channel outputs simultaneously */
    spi_send_24bit((DAC8562_CTRL_LDAC << 16) | 0x0003);
}


/* ============================================================
 * AD9744 相关函数 | AD9744 Related Functions
 * ============================================================ */

/**
 * 将波形数据转换为 AD9744 偏移二进制格式
 * Convert waveform data to AD9744 offset binary format
 *
 * AD9744 使用偏移二进制编码 (Offset Binary):
 *   0x0000 (0)      -> 最负输出 | Most negative output
 *   0x1FFF (8191)   -> 零点 (中点) | Zero (mid-scale)
 *   0x3FFF (16383)  -> 最正输出 | Most positive output
 *
 * 如果波形数据已经是 0~16383 的无符号格式 (与 BRAM 中相同)，
 * 则无需额外转换。
 *
 * If waveform data is already in unsigned 0~16383 format (same as BRAM),
 * no additional conversion is needed.
 *
 * @param input_code  14-bit 无符号输入 (0~16383)
 * @return AD9744 偏移二进制码 (与输入相同，因为已对齐)
 */
uint16_t ad9744_format_data(uint16_t input_code)
{
    /* 限制在 14-bit 范围内 | Clamp to 14-bit range */
    if (input_code > AD9744_FULL_SCALE) {
        input_code = AD9744_FULL_SCALE;
    }

    /*
     * 在 TinyAWG 的实现中，BRAM 中的波形数据已经存储为
     * 0~16383 的偏移二进制格式，所以直接返回即可。
     *
     * 如果需要从二进制补码 (Two's Complement) 转换:
     *   offset_binary = twos_complement + 2^(N-1)
     *   offset_binary = twos_complement + 8192
     *
     * In TinyAWG's implementation, BRAM waveform data is already
     * stored in 0~16383 offset binary format, so return directly.
     *
     * If converting from two's complement:
     *   offset_binary = twos_complement + 2^(N-1)
     *   offset_binary = twos_complement + 8192
     */
    return input_code;
}

/**
 * 将有符号波形值转换为 AD9744 格式
 * Convert signed waveform value to AD9744 format
 *
 * 输入: -1.0 ~ +1.0 的归一化浮点值
 * 输出: 0 ~ 16383 的 14-bit 偏移二进制码
 *
 * Input: normalized floating-point value in -1.0 ~ +1.0
 * Output: 14-bit offset binary code in 0 ~ 16383
 *
 * @param normalized  归一化值 (-1.0 ~ +1.0)
 * @return AD9744 14-bit 码值
 */
uint16_t ad9744_from_normalized(double normalized)
{
    /* 限幅 | Clamp */
    if (normalized > 1.0) normalized = 1.0;
    if (normalized < -1.0) normalized = -1.0;

    /*
     * 偏移二进制映射 Offset binary mapping:
     *   -1.0 -> 0
     *    0.0 -> 8191 (中点 | mid-scale)
     *   +1.0 -> 16383
     *
     *   code = (normalized + 1.0) / 2.0 * 16383
     */
    uint16_t code = (uint16_t)(((normalized + 1.0) / 2.0) * AD9744_FULL_SCALE + 0.5);

    return code;
}
