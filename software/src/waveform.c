/**
 * ============================================================================
 * 波形数据生成模块 | Waveform Data Generation Module
 * ============================================================================
 *
 * 生成各种标准波形的采样数据，用于加载到 FPGA BRAM 作为 DDS 查找表 (LUT)。
 * Generates sample data for various standard waveforms, loaded into FPGA
 * BRAM as the DDS lookup table (LUT).
 *
 * 每种波形生成 65536 个采样点 (16-bit 寻址)，每个采样点为 14-bit 无符号整数
 * (0~16383)，对应 AD9744 DAC 的输入范围。
 *
 * Each waveform generates 65536 sample points (16-bit addressing), each
 * sample is a 14-bit unsigned integer (0~16383), corresponding to the
 * AD9744 DAC input range.
 *
 * 支持的波形类型 Supported Waveform Types:
 *   1. Sine (正弦波)           - 最基本的周期信号
 *   2. Square (方波)           - 数字电路基础信号
 *   3. Triangle (三角波)       - 线性调制常用
 *   4. Ramp (锯齿波)           - 示波器时基、频率扫描
 *   5. Pulse (脉冲)            - 占空比可调
 *   6. Noise (噪声)            - 白噪声，用于系统测试
 *   7. Sinc (采样函数)         - 信号处理教学
 *   8. Exp Decay (指数衰减)    - 阻尼振荡
 *   9. ECG (心电信号)          - 生物医学信号模拟
 *  10. Half Rect (半波整流)    - 电力电子
 *  11. Full Rect (全波整流)    - 电源设计
 *  12. Staircase (阶梯波)      - DAC 测试
 *  13. Custom (自定义)         - 用户上传
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "waveform.h"


/* ============================================================
 * 内部辅助宏 | Internal Helper Macros
 * ============================================================ */

/* AD9744 是 14-bit DAC，满量程值 = 2^14 - 1 = 16383 */
/* AD9744 is a 14-bit DAC, full-scale value = 2^14 - 1 = 16383 */
#define DAC_MAX_VALUE   16383

/* 波形中点 (无偏置时的零电平) */
/* Waveform midpoint (zero level when no offset) */
#define DAC_MID_VALUE   (DAC_MAX_VALUE / 2)

/* 角度转弧度 | Degrees to radians */
#define DEG_TO_RAD(deg) ((deg) * M_PI / 180.0)


/* ============================================================
 * 波形生成函数 | Waveform Generation Functions
 * ============================================================ */

/**
 * 生成正弦波数据
 * Generate sine wave data
 *
 * 公式 Formula:
 *   sample[i] = MID + (MAX/2) * sin(2*pi*i/N)
 *
 * 其中 N = 波形总点数 (65536)
 * Where N = total waveform points (65536)
 *
 * 正弦波是 AWG 最核心的波形，也是所有周期信号的傅里叶基。
 * Sine wave is the most fundamental waveform of AWG and the
 * Fourier basis of all periodic signals.
 *
 * @param buf  输出缓冲区 (长度 >= points)
 * @param points  采样点数 (通常为 WAVEFORM_POINTS=65536)
 */
static void gen_sine(uint16_t *buf, uint32_t points)
{
    for (uint32_t i = 0; i < points; i++) {
        double phase = 2.0 * M_PI * (double)i / (double)points;
        double val = sin(phase);
        /* 映射到 14-bit DAC 范围: -1.0~+1.0 -> 0~16383 */
        /* Map to 14-bit DAC range: -1.0~+1.0 -> 0~16383 */
        buf[i] = (uint16_t)(DAC_MID_VALUE + val * (DAC_MAX_VALUE / 2.0));
    }
}

/**
 * 生成方波数据
 * Generate square wave data
 *
 * 方波在相位 < pi 时为高电平，>= pi 时为低电平。
 * Square wave is high when phase < pi, low when phase >= pi.
 *
 * 理想方波的频谱包含所有奇次谐波 (1, 3, 5, 7, ...)
 * Ideal square wave spectrum contains all odd harmonics.
 *
 * @param buf  输出缓冲区
 * @param points  采样点数
 */
static void gen_square(uint16_t *buf, uint32_t points)
{
    for (uint32_t i = 0; i < points; i++) {
        double phase = 2.0 * M_PI * (double)i / (double)points;
        double val = (sin(phase) >= 0.0) ? 1.0 : -1.0;
        buf[i] = (uint16_t)(DAC_MID_VALUE + val * (DAC_MAX_VALUE / 2.0));
    }
}

/**
 * 生成三角波数据
 * Generate triangle wave data
 *
 * 三角波的前半周期线性上升，后半周期线性下降。
 * Triangle wave rises linearly in the first half-period,
 * falls linearly in the second half.
 *
 * 频谱以二次谐波衰减 (比方波更快)，听起来更"柔和"。
 * Spectrum decays as 1/n^2 (faster than square), sounds "softer".
 *
 * @param buf  输出缓冲区
 * @param points  采样点数
 */
static void gen_triangle(uint16_t *buf, uint32_t points)
{
    for (uint32_t i = 0; i < points; i++) {
        double t = (double)i / (double)points;  /* 0.0 ~ 1.0 */
        double val;
        if (t < 0.25) {
            val = 4.0 * t;          /* 0 -> 1 */
        } else if (t < 0.75) {
            val = 2.0 - 4.0 * t;    /* 1 -> -1 */
        } else {
            val = -4.0 + 4.0 * t;   /* -1 -> 0 */
        }
        buf[i] = (uint16_t)(DAC_MID_VALUE + val * (DAC_MAX_VALUE / 2.0));
    }
}

/**
 * 生成锯齿波 (Ramp) 数据
 * Generate sawtooth (ramp) wave data
 *
 * 锯齿波从 -1 线性上升到 +1，然后瞬间跳回 -1。
 * Sawtooth rises linearly from -1 to +1, then jumps back to -1.
 *
 * 包含所有整数次谐波 (1, 2, 3, ...)，常用于频率扫描。
 * Contains all integer harmonics, commonly used for frequency sweeps.
 *
 * @param buf  输出缓冲区
 * @param points  采样点数
 */
static void gen_ramp(uint16_t *buf, uint32_t points)
{
    for (uint32_t i = 0; i < points; i++) {
        double t = (double)i / (double)points;  /* 0.0 ~ 1.0 */
        double val = 2.0 * t - 1.0;              /* -1.0 ~ +1.0 */
        buf[i] = (uint16_t)(DAC_MID_VALUE + val * (DAC_MAX_VALUE / 2.0));
    }
}

/**
 * 生成脉冲波数据
 * Generate pulse wave data
 *
 * 脉冲波类似方波，但占空比可以不是 50%。
 * Pulse wave is similar to square but with non-50% duty cycle.
 *
 * 默认占空比 10% (可通过参数调整)。
 * Default duty cycle 10% (adjustable via parameter).
 *
 * 占空比 Duty Cycle = 脉冲宽度 / 周期 * 100%
 *
 * @param buf  输出缓冲区
 * @param points  采样点数
 */
static void gen_pulse(uint16_t *buf, uint32_t points)
{
    /* 默认占空比 10% | Default duty cycle 10% */
    double duty = 0.10;
    uint32_t high_count = (uint32_t)(points * duty);

    for (uint32_t i = 0; i < points; i++) {
        double val = (i < high_count) ? 1.0 : -1.0;
        buf[i] = (uint16_t)(DAC_MID_VALUE + val * (DAC_MAX_VALUE / 2.0));
    }
}

/**
 * 生成白噪声数据
 * Generate white noise data
 *
 * 使用线性同余伪随机数生成器 (LCG)。
 * Uses Linear Congruential Generator (LCG) for pseudo-random numbers.
 *
 * LCG 公式 LCG formula:
 *   x[n+1] = (a * x[n] + c) mod m
 *
 * 其中 a=1103515245, c=12345, m=2^31 (glibc 参数)
 * Where a=1103515245, c=12345, m=2^31 (glibc parameters)
 *
 * 白噪声具有平坦的功率谱密度，常用于系统频率响应测试。
 * White noise has flat power spectral density, used for system
 * frequency response testing.
 *
 * @param buf  输出缓冲区
 * @param points  采样点数
 */
static void gen_noise(uint16_t *buf, uint32_t points)
{
    uint32_t seed = 12345;  /* 固定种子保证可重复 | Fixed seed for repeatability */

    for (uint32_t i = 0; i < points; i++) {
        /* LCG 伪随机数 | LCG pseudo-random number */
        seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
        /* 映射到 -1.0 ~ +1.0 | Map to -1.0 ~ +1.0 */
        double val = 2.0 * ((double)seed / 0x7FFFFFFF) - 1.0;
        buf[i] = (uint16_t)(DAC_MID_VALUE + val * (DAC_MAX_VALUE / 2.0));
    }
}

/**
 * 生成 Sinc 函数波形
 * Generate sinc function waveform
 *
 * Sinc 函数定义 Sinc function definition:
 *   sinc(x) = sin(pi*x) / (pi*x)    (x != 0)
 *   sinc(0) = 1
 *
 * Sinc 函数是理想低通滤波器的冲激响应，
 * 也是采样定理 (Nyquist) 的数学基础。
 *
 * Sinc function is the impulse response of an ideal low-pass filter,
 * and the mathematical foundation of the Nyquist sampling theorem.
 *
 * @param buf  输出缓冲区
 * @param points  采样点数
 */
static void gen_sinc(uint16_t *buf, uint32_t points)
{
    /* 在 -6*pi 到 +6*pi 范围内生成 | Generate in range [-6*pi, +6*pi] */
    double x_range = 6.0 * M_PI;

    for (uint32_t i = 0; i < points; i++) {
        double x = -x_range + 2.0 * x_range * (double)i / (double)points;

        double val;
        if (fabs(x) < 1e-10) {
            val = 1.0;   /* sinc(0) = 1 (避免除零) | sinc(0) = 1 (avoid div by zero) */
        } else {
            val = sin(x) / x;
        }

        /* 缩放以利用 DAC 满量程 | Scale to use full DAC range */
        buf[i] = (uint16_t)(DAC_MID_VALUE + val * (DAC_MAX_VALUE / 2.0));
    }
}

/**
 * 生成指数衰减波形
 * Generate exponential decay waveform
 *
 * 公式 Formula:
 *   y(t) = A * sin(2*pi*f*t) * exp(-alpha*t)
 *
 * 模拟阻尼振荡系统，如被敲击的铃铛、RLC 电路的暂态响应。
 * Simulates a damped oscillation system, like a struck bell or
 * the transient response of an RLC circuit.
 *
 * @param buf  输出缓冲区
 * @param points  采样点数
 */
static void gen_exp_decay(uint16_t *buf, uint32_t points)
{
    double alpha = 3.0;  /* 衰减系数 | Decay coefficient */

    for (uint32_t i = 0; i < points; i++) {
        double t = (double)i / (double)points;
        double envelope = exp(-alpha * t);
        double carrier = sin(2.0 * M_PI * 8.0 * t);  /* 8 个完整周期 */
        double val = envelope * carrier;
        buf[i] = (uint16_t)(DAC_MID_VALUE + val * (DAC_MAX_VALUE / 2.0));
    }
}

/**
 * 生成心电图 (ECG) 模拟波形
 * Generate ECG (Electrocardiogram) simulation waveform
 *
 * ECG 信号包含以下特征波 ECG signal contains these characteristic waves:
 *   P 波  - 心房去极化 | Atrial depolarization
 *   QRS 复合波 - 心室去极化 | Ventricular depolarization
 *   T 波  - 心室复极化 | Ventricular repolarization
 *
 * 这里使用简化的高斯函数组合来模拟 ECG 波形。
 * Uses simplified Gaussian function composition to simulate ECG waveform.
 *
 * @param buf  输出缓冲区
 * @param points  采样点数
 */
static void gen_ecg(uint16_t *buf, uint32_t points)
{
    /* 一个心跳周期的采样点数 | Samples per heartbeat cycle */
    uint32_t cycle = points / 4;  /* 4 次心跳在 65536 点中 */

    for (uint32_t i = 0; i < points; i++) {
        double t = (double)(i % cycle) / (double)cycle;  /* 0~1 一个周期 */
        double val = 0.0;

        /* P 波 (心房去极化) | P wave (atrial depolarization) */
        /* 高斯函数模拟: val += A * exp(-(t-center)^2 / (2*sigma^2)) */
        val += 0.15 * exp(-pow(t - 0.15, 2) / (2.0 * pow(0.02, 2)));

        /* Q 波 (小负偏转) | Q wave (small negative deflection) */
        val -= 0.08 * exp(-pow(t - 0.28, 2) / (2.0 * pow(0.005, 2)));

        /* R 波 (主峰) | R wave (main peak) */
        val += 1.0 * exp(-pow(t - 0.30, 2) / (2.0 * pow(0.008, 2)));

        /* S 波 (负偏转) | S wave (negative deflection) */
        val -= 0.15 * exp(-pow(t - 0.32, 2) / (2.0 * pow(0.008, 2)));

        /* T 波 (心室复极化) | T wave (ventricular repolarization) */
        val += 0.25 * exp(-pow(t - 0.55, 2) / (2.0 * pow(0.04, 2)));

        buf[i] = (uint16_t)(DAC_MID_VALUE + val * (DAC_MAX_VALUE / 2.0));
    }
}

/**
 * 生成半波整流正弦波
 * Generate half-wave rectified sine wave
 *
 * 半波整流: 只保留正半周，负半周置零。
 * Half-wave rectification: keep only positive half-cycle, zero negative.
 *
 * 常用于电源设计教学，展示二极管整流效果。
 * Commonly used in power supply design education, demonstrating
 * diode rectification effects.
 *
 * @param buf  输出缓冲区
 * @param points  采样点数
 */
static void gen_half_rect(uint16_t *buf, uint32_t points)
{
    for (uint32_t i = 0; i < points; i++) {
        double phase = 2.0 * M_PI * (double)i / (double)points;
        double val = sin(phase);
        /* 半波整流: 负值变为 0 | Half-wave rect: negative values become 0 */
        if (val < 0.0) val = 0.0;
        /* 映射: 0~1 -> 0~DAC_MAX (偏移到单极性) */
        /* Map: 0~1 -> 0~DAC_MAX (offset to unipolar) */
        buf[i] = (uint16_t)(val * DAC_MAX_VALUE);
    }
}

/**
 * 生成全波整流正弦波
 * Generate full-wave rectified sine wave
 *
 * 全波整流: |sin(x)|，将负半周翻转为正。
 * Full-wave rectification: |sin(x)|, flips negative half to positive.
 *
 * @param buf  输出缓冲区
 * @param points  采样点数
 */
static void gen_full_rect(uint16_t *buf, uint32_t points)
{
    for (uint32_t i = 0; i < points; i++) {
        double phase = 2.0 * M_PI * (double)i / (double)points;
        double val = fabs(sin(phase));  /* |sin(x)| */
        buf[i] = (uint16_t)(val * DAC_MAX_VALUE);
    }
}

/**
 * 生成阶梯波
 * Generate staircase wave
 *
 * 阶梯波在固定间隔跳变，常用于 DAC 线性度测试和 ADC 测试。
 * Staircase wave jumps at fixed intervals, commonly used for DAC
 * linearity testing and ADC testing.
 *
 * @param buf  输出缓冲区
 * @param points  采样点数
 */
static void gen_staircase(uint16_t *buf, uint32_t points)
{
    uint32_t num_steps = 16;  /* 阶梯数 | Number of steps */
    uint32_t samples_per_step = points / num_steps;

    for (uint32_t i = 0; i < points; i++) {
        uint32_t step = i / samples_per_step;
        if (step >= num_steps) step = num_steps - 1;

        double val = -1.0 + 2.0 * (double)step / (double)(num_steps - 1);
        buf[i] = (uint16_t)(DAC_MID_VALUE + val * (DAC_MAX_VALUE / 2.0));
    }
}


/* ============================================================
 * 公共接口函数 | Public Interface Functions
 * ============================================================ */

/**
 * 生成指定类型的波形数据
 * Generate waveform data of specified type
 *
 * @param type    波形类型
 * @param buf     输出缓冲区 (需预分配 WAVEFORM_POINTS 个 uint16_t)
 * @param points  采样点数 (通常 WAVEFORM_POINTS=65536)
 * @return 0=成功, -1=失败
 */
int waveform_generate(waveform_type_t type, uint16_t *buf, uint32_t points)
{
    if (buf == NULL || points == 0) return -1;

    switch (type) {
        case WAVEFORM_SINE:        gen_sine(buf, points);       break;
        case WAVEFORM_SQUARE:      gen_square(buf, points);     break;
        case WAVEFORM_TRIANGLE:    gen_triangle(buf, points);   break;
        case WAVEFORM_RAMP:        gen_ramp(buf, points);       break;
        case WAVEFORM_PULSE:       gen_pulse(buf, points);      break;
        case WAVEFORM_NOISE:       gen_noise(buf, points);      break;
        case WAVEFORM_SINC:        gen_sinc(buf, points);       break;
        case WAVEFORM_EXP_DECAY:   gen_exp_decay(buf, points);  break;
        case WAVEFORM_ECG:         gen_ecg(buf, points);        break;
        case WAVEFORM_HALF_RECT:   gen_half_rect(buf, points);  break;
        case WAVEFORM_FULL_RECT:   gen_full_rect(buf, points);  break;
        case WAVEFORM_STAIRCASE:   gen_staircase(buf, points);  break;
        default:
            /* 未知波形类型，生成静音 (中点值) */
            /* Unknown waveform type, generate silence (midpoint) */
            memset(buf, DAC_MID_VALUE & 0xFF, points * sizeof(uint16_t));
            return -1;
    }

    return 0;
}

/**
 * 波形数据缩放和归一化
 * Scale and normalize waveform data
 *
 * 将波形数据按指定的幅度比例进行缩放。
 * Scales waveform data by the specified amplitude ratio.
 *
 * @param buf       波形数据缓冲区
 * @param points    采样点数
 * @param scale     缩放因子 (0.0~1.0)
 * @param offset    直流偏移 (以 DAC 值为单位)
 */
void waveform_scale(uint16_t *buf, uint32_t points, double scale, int16_t offset)
{
    if (buf == NULL || points == 0) return;

    for (uint32_t i = 0; i < points; i++) {
        /* 从 DAC 值转换回归一化值 (-1.0~+1.0) */
        /* Convert DAC value back to normalized (-1.0~+1.0) */
        double normalized = ((double)buf[i] - DAC_MID_VALUE) / (DAC_MAX_VALUE / 2.0);

        /* 应用缩放 | Apply scaling */
        normalized *= scale;

        /* 应用偏移并限制范围 | Apply offset and clamp */
        double result = normalized * (DAC_MAX_VALUE / 2.0) + DAC_MID_VALUE + offset;

        /* 限幅 Clipping: 确保在 0~16383 范围内 */
        /* Clamp: ensure within 0~16383 range */
        if (result < 0) result = 0;
        if (result > DAC_MAX_VALUE) result = DAC_MAX_VALUE;

        buf[i] = (uint16_t)result;
    }
}

/**
 * 接收上位机上传的自定义波形数据
 * Receive custom waveform data uploaded from host PC
 *
 * 上位机通过 UART 以十六进制格式发送波形数据:
 * Host PC sends waveform data via UART in hex format:
 *
 * 协议格式 Protocol format:
 *   "WAVE_START\r\n"
 *   "XXXX\n"  (每行一个 16-bit 十六进制值, 如 "1FFF")
 *   ...
 *   "WAVE_END\r\n"
 *
 * @param buf     接收缓冲区
 * @param max_points  最大接收点数
 * @return 实际接收的点数, 0=失败
 */
uint32_t waveform_receive_custom(uint16_t *buf, uint32_t max_points)
{
    /*
     * TODO: 实现 UART 接收和十六进制解析
     * TODO: Implement UART receive and hex parsing
     *
     * 伪代码 Pseudocode:
     *   wait_for_line("WAVE_START");
     *   for (i = 0; i < max_points; i++) {
     *       line = read_uart_line();
     *       if (line == "WAVE_END") break;
     *       buf[i] = parse_hex(line);
     *   }
     *   return i;
     */
    printf("[WAVEFORM] 自定义波形上传功能待实现\r\n");
    (void)buf;
    (void)max_points;
    return 0;
}
