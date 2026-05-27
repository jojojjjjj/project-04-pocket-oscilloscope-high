/**
 * ============================================================================
 * 工具函数模块 | Utility Functions Module
 * ============================================================================
 *
 * 提供各种通用辅助函数，供其他模块调用。
 * Provides various general-purpose helper functions for other modules.
 *
 * 功能分类 Function Categories:
 *   1. 字符串格式化 - 频率、电压等参数的显示格式化
 *      String formatting - display formatting for frequency, voltage, etc.
 *
 *   2. 数学辅助 - 线性插值、限幅、映射
 *      Math helpers - linear interpolation, clamp, mapping
 *
 *   3. UART 调试输出 - 串口打印调试信息
 *      UART debug output - serial port debug printing
 *
 *   4. 电池电量计算 - MAX17048 电量读取和计算
 *      Battery calculation - MAX17048 SOC reading and calculation
 *
 *   5. UART 命令处理 - 上位机命令解析
 *      UART command processing - host PC command parsing
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "xil_types.h"
#include "utils.h"


/* ============================================================
 * 字符串格式化函数 | String Formatting Functions
 * ============================================================ */

/**
 * 格式化频率显示
 * Format frequency for display
 *
 * 自动选择合适的单位 (Hz, kHz, MHz)。
 * Automatically selects appropriate unit (Hz, kHz, MHz).
 *
 * 示例 Examples:
 *   0.5      -> "0.5 Hz"
 *   1500     -> "1.50 kHz"
 *   12345678 -> "12.35 MHz"
 *
 * @param freq_hz  频率值 (Hz)
 * @param buf      输出字符串缓冲区
 * @param buf_size 缓冲区大小
 */
void utils_format_frequency(double freq_hz, char *buf, uint32_t buf_size)
{
    if (buf == NULL || buf_size == 0) return;

    if (freq_hz >= 1e6) {
        snprintf(buf, buf_size, "%.2f MHz", freq_hz / 1e6);
    } else if (freq_hz >= 1e3) {
        snprintf(buf, buf_size, "%.2f kHz", freq_hz / 1e3);
    } else if (freq_hz >= 1.0) {
        snprintf(buf, buf_size, "%.1f Hz", freq_hz);
    } else if (freq_hz > 0.0) {
        snprintf(buf, buf_size, "%.1f mHz", freq_hz * 1000.0);
    } else {
        snprintf(buf, buf_size, "--- Hz");
    }
}

/**
 * 格式化电压显示
 * Format voltage for display
 *
 * 自动选择合适的单位 (V, mV, uV)。
 * Automatically selects appropriate unit (V, mV, uV).
 *
 * @param voltage  电压值 (V)
 * @param buf      输出字符串缓冲区
 * @param buf_size 缓冲区大小
 */
void utils_format_voltage(double voltage, char *buf, uint32_t buf_size)
{
    if (buf == NULL || buf_size == 0) return;

    double abs_v = fabs(voltage);
    if (abs_v >= 1.0) {
        snprintf(buf, buf_size, "%.3f V", voltage);
    } else if (abs_v >= 0.001) {
        snprintf(buf, buf_size, "%.1f mV", voltage * 1000.0);
    } else {
        snprintf(buf, buf_size, "%.1f uV", voltage * 1e6);
    }
}

/**
 * 格式化时间显示
 * Format time for display
 *
 * @param seconds  时间值 (秒)
 * @param buf      输出字符串缓冲区
 * @param buf_size 缓冲区大小
 */
void utils_format_time(double seconds, char *buf, uint32_t buf_size)
{
    if (buf == NULL || buf_size == 0) return;

    if (seconds >= 1.0) {
        snprintf(buf, buf_size, "%.2f s", seconds);
    } else if (seconds >= 0.001) {
        snprintf(buf, buf_size, "%.1f ms", seconds * 1000.0);
    } else {
        snprintf(buf, buf_size, "%.1f us", seconds * 1e6);
    }
}


/* ============================================================
 * 数学辅助函数 | Math Helper Functions
 * ============================================================ */

/**
 * 限幅函数 (将值限制在指定范围内)
 * Clamp function (restrict value to specified range)
 *
 * 这是信号处理中最常用的函数之一，防止数值溢出。
 * This is one of the most commonly used functions in signal
 * processing to prevent numeric overflow.
 *
 * @param val  输入值
 * @param min  最小值
 * @param max  最大值
 * @return 限幅后的值
 */
double utils_clamp(double val, double min, double max)
{
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

/**
 * 线性插值
 * Linear interpolation
 *
 * 在两个已知点之间进行线性插值。
 * Performs linear interpolation between two known points.
 *
 * 公式 Formula:
 *   y = y0 + (x - x0) * (y1 - y0) / (x1 - x0)
 *
 * 应用场景 Application scenarios:
 *   - 波形数据的重采样
 *   - DAC 校准曲线的内插
 *   - 传感器数据的平滑
 *
 *   - Waveform data resampling
 *   - DAC calibration curve interpolation
 *   - Sensor data smoothing
 *
 * @param x   插值点的 x 坐标
 * @param x0  起始点 x
 * @param y0  起始点 y
 * @param x1  终止点 x
 * @param y1  终止点 y
 * @return 插值结果 y
 */
double utils_lerp(double x, double x0, double y0, double x1, double y1)
{
    if (x1 == x0) return y0;  /* 避免除零 | Avoid division by zero */
    return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
}

/**
 * 值映射 (将一个范围的值线性映射到另一个范围)
 * Value mapping (linearly maps values from one range to another)
 *
 * 公式 Formula:
 *   out = (in - in_min) / (in_max - in_min) * (out_max - out_min) + out_min
 *
 * 示例 Example:
 *   将 0~1000 的滑块值映射到 1Hz~70MHz (线性)
 *   Map slider value 0~1000 to 1Hz~70MHz (linear)
 *
 * @param in       输入值
 * @param in_min   输入范围最小值
 * @param in_max   输入范围最大值
 * @param out_min  输出范围最小值
 * @param out_max  输出范围最大值
 * @return 映射后的值
 */
double utils_map(double in, double in_min, double in_max,
                  double out_min, double out_max)
{
    if (in_max == in_min) return out_min;
    double ratio = (in - in_min) / (in_max - in_min);
    return out_min + ratio * (out_max - out_min);
}

/**
 * 对数映射 (将线性输入映射到对数输出)
 * Logarithmic mapping (maps linear input to logarithmic output)
 *
 * 用于频率滑块等需要宽范围对数调节的场景。
 * Used for frequency sliders and other scenarios requiring
 * wide-range logarithmic adjustment.
 *
 * 公式 Formula:
 *   out = out_min * (out_max/out_min)^(in/in_max)
 *
 * 或等效地 Or equivalently:
 *   out = 10^(log10(out_max/out_min) * in/in_max) * out_min
 *
 * @param in       输入值 (0~in_max)
 * @param in_max   输入范围最大值
 * @param out_min  输出范围最小值
 * @param out_max  输出范围最大值
 * @return 映射后的值
 */
double utils_log_map(double in, double in_max,
                      double out_min, double out_max)
{
    if (out_min <= 0 || out_max <= 0 || in_max <= 0) return out_min;
    double ratio = in / in_max;
    return out_min * pow(out_max / out_min, ratio);
}


/* ============================================================
 * UART 调试输出函数 | UART Debug Output Functions
 * ============================================================ */

/**
 * UART 调试打印
 * UART debug print
 *
 * 在 ZYNQ 上，UART0 通常用于调试输出 (printk)。
 * On ZYNQ, UART0 is typically used for debug output.
 *
 * Vitis SDK 会自动将 printf 重定向到 UART。
 * Vitis SDK automatically redirects printf to UART.
 *
 * DEBUG_PRINT 宏在 utils.h 中定义:
 * DEBUG_PRINT macro is defined in utils.h:
 *   #define DEBUG_PRINT(fmt, ...) printf(fmt "\r\n", ##__VA_ARGS__)
 *
 * @param fmt  格式化字符串
 * @param ...  可变参数
 */
/* 注意: 实际使用 DEBUG_PRINT 宏，不需要单独调用此函数 */
/* Note: Use DEBUG_PRINT macro, no need to call this function directly */


/* ============================================================
 * 电池电量函数 | Battery Functions
 * ============================================================ */

/**
 * 读取 MAX17048 电池电量百分比
 * Read MAX17048 battery state-of-charge percentage
 *
 * MAX17048 是 Maxim (现 Analog Devices) 的电量计芯片:
 * MAX17048 is a fuel gauge chip by Maxim (now Analog Devices):
 *
 *   - I2C 地址: 0x36 (7-bit)
 *   - SOC 寄存器: 0x04 (16-bit, 高字节=整数%, 低字节=小数)
 *   - 使用 ModelGauge 算法，无需电池建模
 *   - 精度: ±1%
 *
 * 读取流程 Read flow:
 *   1. I2C Start + 设备地址 (0x36, Write)
 *   2. 写入寄存器地址 (0x04)
 *   3. I2C Repeated Start + 设备地址 (0x36, Read)
 *   4. 读取 2 字节 (MSB first)
 *
 * @return 电量百分比 (0~100), -1=读取失败
 */
int utils_read_battery_percent(void)
{
    /*
     * TODO: 使用 XIicPs 驱动读取 MAX17048
     * TODO: Read MAX17048 using XIicPs driver
     *
     * uint8_t reg_addr = 0x04;  // SOC 寄存器
     * uint8_t data[2];
     *
     * // 写入寄存器地址
     * XIicPs_MasterSendPolled(&i2c_inst, &reg_addr, 1, 0x36);
     *
     * // 读取 2 字节数据
     * XIicPs_MasterRecvPolled(&i2c_inst, data, 2, 0x36);
     *
     * // SOC 值: 高字节为整数百分比
     * int percent = data[0];
     * return percent;
     *
     * 注: MAX17048 的 SOC 寄存器返回值格式:
     * Note: MAX17048 SOC register return format:
     *   data[0] = 整数百分比 (0~100)
     *   data[1] = 小数部分 (1/256%)
     */

    /* 模拟返回固定值 | Simulated fixed return value */
    return 85;
}

/**
 * 读取 MAX17048 电池电压
 * Read MAX17048 battery voltage
 *
 * VCELL 寄存器 (0x02) 返回 16-bit 值:
 * VCELL register (0x02) returns 16-bit value:
 *
 *   voltage = register_value * 1.25 / 1000 / 16  (单位: mV)
 *
 * 简化: voltage_mV = register_value * 78.125 / 1000
 *
 * 锂电池正常电压范围: 3.0V ~ 4.2V
 * Li-ion battery normal voltage range: 3.0V ~ 4.2V
 *
 * @return 电池电压 (mV), -1=读取失败
 */
int utils_read_battery_voltage_mv(void)
{
    /*
     * TODO: 读取 MAX17048 VCELL 寄存器 (0x02)
     * TODO: Read MAX17048 VCELL register (0x02)
     *
     * uint8_t reg_addr = 0x02;
     * uint8_t data[2];
     * XIicPs_MasterSendPolled(&i2c_inst, &reg_addr, 1, 0x36);
     * XIicPs_MasterRecvPolled(&i2c_inst, data, 2, 0x36);
     * uint16_t raw = (data[0] << 8) | data[1];
     * int voltage_mv = (int)(raw * 78.125 / 1000.0);
     * return voltage_mv;
     */

    /* 模拟返回 3.85V | Simulated return 3.85V */
    return 3850;
}


/* ============================================================
 * UART 命令处理 | UART Command Processing
 * ============================================================ */

/**
 * 处理上位机 UART 命令
 * Process host PC UART commands
 *
 * 在主循环中周期性调用，非阻塞式检查是否有新命令。
 * Called periodically in main loop, non-blocking check for new commands.
 *
 * 命令格式 Command format:
 *   "FREQ <value>\n"      - 设置频率 (Hz)
 *   "AMP <value>\n"       - 设置幅度 (Vpp)
 *   "OFFSET <value>\n"    - 设置偏置 (V)
 *   "WAVE <type>\n"       - 选择波形 (sine/square/triangle/...)
 *   "OUTPUT <0|1>\n"      - 开关输出
 *   "SWEEP <start> <stop> <step> <dwell>\n"  - 启动扫频
 *   "AM <carrier> <mod> <depth>\n"  - 启动 AM
 *   "UPLOAD\n"            - 进入波形上传模式
 *   "STATUS\n"            - 查询状态
 *
 * 响应格式 Response format:
 *   "OK\n"                - 命令执行成功
 *   "ERR <message>\n"     - 命令执行失败
 *   "STATUS freq=... amp=... offset=... wave=... output=...\n"
 */
void utils_process_uart_commands(void)
{
    /*
     * TODO: 实现 UART 非阻塞接收和命令解析
     * TODO: Implement non-blocking UART receive and command parsing
     *
     * 伪代码 Pseudocode:
     *
     *   static char cmd_buf[128];
     *   static int cmd_idx = 0;
     *
     *   // 非阻塞读取一个字节
     *   while (uart_rx_available()) {
     *       char c = uart_rx_byte();
     *       if (c == '\n') {
     *           cmd_buf[cmd_idx] = '\0';
     *           parse_and_execute_command(cmd_buf);
     *           cmd_idx = 0;
     *       } else if (cmd_idx < sizeof(cmd_buf) - 1) {
     *           cmd_buf[cmd_idx++] = c;
     *       }
     *   }
     *
     * void parse_and_execute_command(char *cmd) {
     *   if (strncmp(cmd, "FREQ ", 5) == 0) {
     *       double freq = atof(cmd + 5);
     *       dds_set_frequency(freq);
     *       uart_send("OK\n");
     *   } else if (strncmp(cmd, "AMP ", 4) == 0) {
     *       double amp = atof(cmd + 4);
     *       output_ctrl_set_amplitude(amp);
     *       uart_send("OK\n");
     *   }
     *   // ... 其他命令
     * }
     */
}
