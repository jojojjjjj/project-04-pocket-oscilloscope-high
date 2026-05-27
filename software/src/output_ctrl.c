/**
 * ============================================================================
 * 输出控制模块 | Output Control Module
 * ============================================================================
 *
 * 管理 TinyAWG 的模拟输出链路，包括:
 * Manages TinyAWG's analog output chain, including:
 *
 *   1. 输出幅度控制 (0~10Vpp) - 通过 DAC8562 通道 A
 *      Output amplitude control (0~10Vpp) - via DAC8562 channel A
 *
 *   2. 输出偏置控制 (±5V) - 通过 DAC8562 通道 B
 *      Output offset control (+/-5V) - via DAC8562 channel B
 *
 *   3. 输出使能/禁用 - 通过继电器
 *      Output enable/disable - via relay
 *
 *   4. 扫频模式 - 频率从起始值线性步进到终止值
 *      Sweep mode - frequency steps linearly from start to stop
 *
 *   5. AM 调制模式 - 幅度调制
 *      AM modulation mode - amplitude modulation
 *
 * ============================================================
 * 输出信号链路 | Output Signal Chain
 * ============================================================
 *
 *   FPGA DDS  ---> AD9744 DAC ---> 可变增益放大器 ---> 继电器 ---> 50Ω 输出
 *   (数字)        (14-bit DAC)     (DAC8562 控制)      (开关)     (BNC/MCX)
 *
 *   DAC8562 ChA (幅度) ---> 控制可变增益放大器的增益
 *   DAC8562 ChB (偏置) ---> 控制输出直流偏置
 *
 *   FPGA DDS  --> AD9744 DAC --> Variable Gain Amp --> Relay --> 50-ohm Output
 *   (digital)    (14-bit DAC)   (DAC8562 control)    (switch)   (BNC/MCX)
 *
 *   DAC8562 ChA (amplitude) --> Controls VGA gain
 *   DAC8562 ChB (offset)    --> Controls output DC offset
 *
 * ============================================================
 * 输出阻抗说明 | Output Impedance Note
 * ============================================================
 *
 * 标准信号发生器输出阻抗为 50Ω。当负载也是 50Ω 时 (高阻抗示波器探头)，
 * 实际输出电压为开路电压的一半。
 *
 * Standard signal generator output impedance is 50-ohm. When the load
 * is also 50-ohm, the actual output voltage is half the open-circuit voltage.
 *
 *   V_load = V_open * R_load / (R_out + R_load)
 *          = V_open * 50 / (50 + 50)
 *          = V_open / 2
 *
 * 因此 DAC 输出需要放大到 2 倍目标电压，以补偿 50Ω 分压。
 * Therefore DAC output must be amplified to 2x target voltage to
 * compensate for the 50-ohm voltage division.
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include "xstatus.h"
#include "dac_driver.h"
#include "dds_controller.h"
#include "output_ctrl.h"
#include "utils.h"


/* ============================================================
 * 常量定义 | Constant Definitions
 * ============================================================ */

/* 输出范围 | Output ranges */
#define OUTPUT_MAX_AMPLITUDE_VPP   10.0    /* 最大峰峰值 10V | Max Vpp 10V */
#define OUTPUT_MAX_OFFSET_V        5.0     /* 最大偏置 ±5V | Max offset ±5V */
#define OUTPUT_IMPEDANCE_OHM       50.0    /* 输出阻抗 50Ω | Output impedance */

/* 扫频默认参数 | Sweep default parameters */
#define SWEEP_DEFAULT_DWELL_MS     100     /* 每步停留 100ms | Dwell 100ms per step */

/* AM 调制范围 | AM modulation range */
#define AM_MOD_DEPTH_MIN           0.0     /* 最小调制深度 | Min modulation depth */
#define AM_MOD_DEPTH_MAX           1.0     /* 最大调制深度 | Max modulation depth */


/* ============================================================
 * 模块状态 | Module State
 * ============================================================ */

typedef struct {
    double amplitude_vpp;       /* 当前幅度 Vpp | Current amplitude Vpp */
    double offset_v;            /* 当前偏置 V | Current offset V */
    bool output_enabled;        /* 输出使能状态 | Output enable state */

    /* 扫频参数 | Sweep parameters */
    bool sweep_active;          /* 扫频是否正在进行 | Is sweep running */
    double sweep_start;         /* 扫频起始频率 | Sweep start freq */
    double sweep_stop;          /* 扫频终止频率 | Sweep stop freq */
    double sweep_step;          /* 扫频步进 | Sweep step */
    double sweep_dwell_ms;      /* 每步停留时间 | Dwell time per step */
    double sweep_current;       /* 当前扫频频率 | Current sweep freq */

    /* AM 调制参数 | AM modulation parameters */
    bool am_active;             /* AM 调制是否激活 | Is AM modulation active */
    double am_carrier_freq;     /* AM 载波频率 | AM carrier frequency */
    double am_mod_freq;         /* AM 调制频率 | AM modulation frequency */
    double am_depth;            /* AM 调制深度 (0~1) | AM modulation depth */
} output_state_t;

static output_state_t g_output;


/* ============================================================
 * 公共函数实现 | Public Function Implementations
 * ============================================================ */

/**
 * 初始化输出控制模块
 * Initialize output control module
 *
 * 将 DAC8562 两个通道都设为安全默认值 (零输出)。
 * Sets both DAC8562 channels to safe defaults (zero output).
 */
void output_ctrl_init(void)
{
    memset(&g_output, 0, sizeof(g_output));
    g_output.amplitude_vpp = 0.0;
    g_output.offset_v = 0.0;
    g_output.output_enabled = false;
    g_output.sweep_active = false;
    g_output.am_active = false;

    /* DAC8562 已在 main.c 中初始化 | DAC8562 already init'd in main.c */
    /* 设置零输出 | Set zero output */
    dac8562_set_channel_a(0);
    dac8562_set_channel_b(DAC8562_MID_CODE);  /* 偏置中点 | Offset midpoint */

    printf("[OUTPUT] 输出控制初始化完成\r\n");
}

/**
 * 设置输出幅度
 * Set output amplitude
 *
 * 将幅度设置值转换为 DAC8562 通道 A 的数字码。
 * Converts amplitude setting to DAC8562 channel A digital code.
 *
 * 转换链路 Conversion chain:
 *   Vpp (设置值) --> 放大器增益电压 --> DAC8562 码值
 *
 * DAC8562 输出电压范围: 0 ~ Vref*2 = 0 ~ 5.0V (使用内部 2.5V 基准)
 * DAC8562 output voltage range: 0 ~ Vref*2 = 0 ~ 5.0V (internal 2.5V ref)
 *
 * DAC8562 是 16-bit DAC，所以:
 * DAC8562 is a 16-bit DAC, so:
 *   code = V_out / (Vref * 2) * 65535
 *        = V_out / 5.0 * 65535
 *
 * 假设放大器增益与 DAC8562 输出是线性映射关系:
 * Assuming amplifier gain maps linearly to DAC8562 output:
 *   Vpp_max = 10V  -> DAC8562 code = 65535 (满量程)
 *   Vpp = 0V       -> DAC8562 code = 0
 *
 * @param vpp  目标峰峰值电压 (0~10V)
 * @return XST_SUCCESS
 */
int output_ctrl_set_amplitude(double vpp)
{
    /* 限制范围 | Clamp range */
    if (vpp < 0.0) vpp = 0.0;
    if (vpp > OUTPUT_MAX_AMPLITUDE_VPP) vpp = OUTPUT_MAX_AMPLITUDE_VPP;

    g_output.amplitude_vpp = vpp;

    /*
     * 将 Vpp 转换为 DAC8562 码值
     * Convert Vpp to DAC8562 code value
     *
     * 线性映射: code = (vpp / 10.0) * 65535
     *
     * 注意: 实际硬件可能有非线性，需要校准。
     * Note: Actual hardware may be non-linear, requiring calibration.
     * 校准系数存储在 config.yaml 中的 calibration 段。
     * Calibration coefficients are stored in config.yaml's calibration section.
     */
    uint16_t dac_code = (uint16_t)((vpp / OUTPUT_MAX_AMPLITUDE_VPP) * DAC8562_MAX_CODE);
    dac8562_set_channel_a(dac_code);

    return XST_SUCCESS;
}

/**
 * 设置输出偏置
 * Set output offset
 *
 * 将偏置设置值转换为 DAC8562 通道 B 的数字码。
 * Converts offset setting to DAC8562 channel B digital code.
 *
 * 偏置范围: -5V ~ +5V
 * Offset range: -5V ~ +5V
 *
 * 映射关系 Mapping:
 *   -5V  -> DAC8562 code = 0      (最小输出)
 *    0V  -> DAC8562 code = 32767   (中点)
 *   +5V  -> DAC8562 code = 65535   (最大输出)
 *
 * 公式 Formula:
 *   code = (offset + 5.0) / 10.0 * 65535
 *
 * @param offset_v  目标偏置电压 (-5V ~ +5V)
 * @return XST_SUCCESS
 */
int output_ctrl_set_offset(double offset_v)
{
    /* 限制范围 | Clamp range */
    if (offset_v < -OUTPUT_MAX_OFFSET_V) offset_v = -OUTPUT_MAX_OFFSET_V;
    if (offset_v > OUTPUT_MAX_OFFSET_V)  offset_v = OUTPUT_MAX_OFFSET_V;

    g_output.offset_v = offset_v;

    /*
     * 双极性偏置映射到单极性 DAC:
     * Bipolar offset mapped to unipolar DAC:
     *
     *   code = ((offset + 5.0) / 10.0) * 65535
     *
     * 例如 For example:
     *   offset = -5.0V -> code = 0
     *   offset =  0.0V -> code = 32767
     *   offset = +5.0V -> code = 65535
     */
    uint16_t dac_code = (uint16_t)(((offset_v + OUTPUT_MAX_OFFSET_V)
                          / (2.0 * OUTPUT_MAX_OFFSET_V)) * DAC8562_MAX_CODE);
    dac8562_set_channel_b(dac_code);

    return XST_SUCCESS;
}

/**
 * 使能或禁用输出
 * Enable or disable output
 *
 * 通过控制继电器来连接或断开输出信号通路。
 * Controls the relay to connect or disconnect the output signal path.
 *
 * 继电器控制 Relay control:
 *   GPIO 高电平 -> 继电器吸合 -> 信号输出连通
 *   GPIO 低电平 -> 继电器释放 -> 信号输出断开
 *
 *   GPIO high -> relay closed -> signal path connected
 *   GPIO low  -> relay open   -> signal path disconnected
 *
 * @param enable  true=使能输出, false=禁用输出
 */
void output_ctrl_enable(bool enable)
{
    g_output.output_enabled = enable;

    if (enable) {
        /* 继电器吸合，接通信号输出 */
        /* Close relay, connect signal output */
        /* TODO: GPIO 写高电平 */
        /* TODO: Write GPIO high */
        /* XGpio_DiscreteWrite(&gpio, RELAY_CHANNEL, RELAY_ENABLE_MASK); */

        /* 同时使能 DDS 输出 */
        /* Also enable DDS output */
        dds_enable(true);

        printf("[OUTPUT] 输出已使能 | Output enabled\r\n");
    } else {
        /* 继电器释放，断开信号输出 */
        /* Open relay, disconnect signal output */
        /* XGpio_DiscreteWrite(&gpio, RELAY_CHANNEL, 0); */

        /* 同时禁用 DDS 输出 */
        /* Also disable DDS output */
        dds_enable(false);

        printf("[OUTPUT] 输出已禁用 | Output disabled\r\n");
    }
}

/**
 * 启动扫频模式
 * Start frequency sweep mode
 *
 * 扫频模式下，输出频率从 start_freq 线性步进到 stop_freq，
 * 每个频率点停留 dwell_ms 毫秒。
 *
 * In sweep mode, output frequency steps linearly from start_freq
 * to stop_freq, dwelling dwell_ms milliseconds at each frequency point.
 *
 * 扫频常用于测量电路的频率响应 (如滤波器的幅频特性)。
 * Sweep is commonly used to measure circuit frequency response
 * (e.g., filter amplitude-frequency characteristics).
 *
 * @param start_freq  起始频率 (Hz)
 * @param stop_freq   终止频率 (Hz)
 * @param step_freq   步进频率 (Hz)
 * @param dwell_ms    每步停留时间 (ms)
 * @return XST_SUCCESS 或 XST_FAILURE
 */
int output_ctrl_start_sweep(double start_freq, double stop_freq,
                             double step_freq, double dwell_ms)
{
    /* 参数检查 | Parameter validation */
    if (start_freq <= 0 || stop_freq <= 0 || step_freq <= 0) {
        return XST_FAILURE;
    }
    if (start_freq >= stop_freq) {
        return XST_FAILURE;
    }

    g_output.sweep_start = start_freq;
    g_output.sweep_stop = stop_freq;
    g_output.sweep_step = step_freq;
    g_output.sweep_dwell_ms = dwell_ms;
    g_output.sweep_current = start_freq;
    g_output.sweep_active = true;

    /* 设置起始频率 | Set start frequency */
    dds_set_frequency(start_freq);

    printf("[OUTPUT] 扫频启动: %.1f Hz -> %.1f Hz, 步进 %.1f Hz\r\n",
           start_freq, stop_freq, step_freq);

    return XST_SUCCESS;
}

/**
 * 停止扫频模式
 * Stop frequency sweep mode
 */
void output_ctrl_stop_sweep(void)
{
    g_output.sweep_active = false;
    printf("[OUTPUT] 扫频已停止 | Sweep stopped\r\n");
}

/**
 * 扫频模式周期更新函数
 * Sweep mode periodic update function
 *
 * 应在主循环中周期性调用 (例如每 10ms)。
 * Should be called periodically in the main loop (e.g., every 10ms).
 *
 * @param elapsed_ms  距上次调用经过的时间 (ms)
 */
void output_ctrl_sweep_update(uint32_t elapsed_ms)
{
    static uint32_t dwell_counter = 0;

    if (!g_output.sweep_active) return;

    dwell_counter += elapsed_ms;

    /* 停留时间到，步进到下一个频率 | Dwell time elapsed, step to next freq */
    if (dwell_counter >= (uint32_t)g_output.sweep_dwell_ms) {
        dwell_counter = 0;

        g_output.sweep_current += g_output.sweep_step;

        /* 检查是否到达终止频率 | Check if stop frequency reached */
        if (g_output.sweep_current >= g_output.sweep_stop) {
            /* 循环: 回到起始频率 | Loop: return to start frequency */
            g_output.sweep_current = g_output.sweep_start;
        }

        /* 更新 DDS 频率 | Update DDS frequency */
        dds_set_frequency(g_output.sweep_current);
    }
}

/**
 * 启动 AM 调制模式
 * Start AM modulation mode
 *
 * AM (Amplitude Modulation) 幅度调制:
 *   s(t) = A_c * [1 + m * sin(2*pi*f_m*t)] * sin(2*pi*f_c*t)
 *
 * 其中 Where:
 *   A_c  = 载波幅度 | Carrier amplitude
 *   f_c  = 载波频率 | Carrier frequency
 *   f_m  = 调制频率 | Modulation frequency
 *   m    = 调制深度 (0~1) | Modulation depth
 *
 * AM 调制在软件中通过周期性调整 DAC8562 通道 A (幅度控制) 来实现。
 * AM modulation is implemented in software by periodically adjusting
 * DAC8562 channel A (amplitude control).
 *
 * 调制频率需要远低于 DDS 更新速率以保证平滑。
 * Modulation frequency must be well below DDS update rate for smoothness.
 *
 * @param carrier_freq  载波频率 (Hz)
 * @param mod_freq      调制频率 (Hz)
 * @param depth         调制深度 (0.0~1.0)
 * @return XST_SUCCESS 或 XST_FAILURE
 */
int output_ctrl_start_am(double carrier_freq, double mod_freq, double depth)
{
    /* 参数检查 | Parameter validation */
    if (carrier_freq <= 0 || mod_freq <= 0) {
        return XST_FAILURE;
    }
    if (depth < AM_MOD_DEPTH_MIN) depth = AM_MOD_DEPTH_MIN;
    if (depth > AM_MOD_DEPTH_MAX) depth = AM_MOD_DEPTH_MAX;

    g_output.am_carrier_freq = carrier_freq;
    g_output.am_mod_freq = mod_freq;
    g_output.am_depth = depth;
    g_output.am_active = true;

    /* 设置载波频率 | Set carrier frequency */
    dds_set_frequency(carrier_freq);

    printf("[OUTPUT] AM 调制启动: 载波 %.1f Hz, 调制 %.1f Hz, 深度 %.0f%%\r\n",
           carrier_freq, mod_freq, depth * 100.0);

    return XST_SUCCESS;
}

/**
 * 停止 AM 调制
 * Stop AM modulation
 */
void output_ctrl_stop_am(void)
{
    g_output.am_active = false;

    /* 恢复到当前设定的固定幅度 | Restore to current fixed amplitude */
    output_ctrl_set_amplitude(g_output.amplitude_vpp);

    printf("[OUTPUT] AM 调制已停止 | AM modulation stopped\r\n");
}

/**
 * AM 调制周期更新函数
 * AM modulation periodic update function
 *
 * 应在主循环中周期性调用。
 * Should be called periodically in the main loop.
 *
 * @param elapsed_ms  距上次调用经过的时间 (ms)
 */
void output_ctrl_am_update(uint32_t elapsed_ms)
{
    static double am_phase = 0.0;

    if (!g_output.am_active) return;

    /* 更新调制相位 | Update modulation phase */
    am_phase += 2.0 * M_PI * g_output.am_mod_freq * (elapsed_ms / 1000.0);
    if (am_phase > 2.0 * M_PI) {
        am_phase -= 2.0 * M_PI;
    }

    /*
     * AM 调制包络 AM modulation envelope:
     *   envelope = 1 + m * sin(phase)
     *
     * 调制后的有效幅度 modulated amplitude:
     *   effective_vpp = base_vpp * envelope
     *
     * 其中 m 是调制深度, base_vpp 是用户设置的幅度
     */
    double envelope = 1.0 + g_output.am_depth * sin(am_phase);
    double effective_vpp = g_output.amplitude_vpp * envelope;

    /* 限幅 | Clamp */
    if (effective_vpp < 0.0) effective_vpp = 0.0;
    if (effective_vpp > OUTPUT_MAX_AMPLITUDE_VPP) effective_vpp = OUTPUT_MAX_AMPLITUDE_VPP;

    /* 更新 DAC8562 通道 A (幅度) | Update DAC8562 channel A (amplitude) */
    uint16_t dac_code = (uint16_t)((effective_vpp / OUTPUT_MAX_AMPLITUDE_VPP) * DAC8562_MAX_CODE);
    dac8562_set_channel_a(dac_code);
}

/**
 * 获取当前输出幅度
 * Get current output amplitude
 * @return 幅度 Vpp (V)
 */
double output_ctrl_get_amplitude(void)
{
    return g_output.amplitude_vpp;
}

/**
 * 获取当前输出偏置
 * Get current output offset
 * @return 偏置 (V)
 */
double output_ctrl_get_offset(void)
{
    return g_output.offset_v;
}

/**
 * 获取输出使能状态
 * Get output enable state
 * @return true=已使能, false=已禁用
 */
bool output_ctrl_is_enabled(void)
{
    return g_output.output_enabled;
}
