"""
============================================================================
TinyAWG 基础测试模块 | TinyAWG Basic Test Module
============================================================================

功能：测试波形生成算法、DDS 频率计算、DAC8562 数值计算的正确性
Features: Test waveform generation algorithms, DDS frequency calculation,
          and DAC8562 value calculation correctness

这些测试在 PC 端运行（不需要实际硬件），用于验证嵌入式软件的
核心算法在移植到 ZYNQ 之前是正确的。

These tests run on PC (no actual hardware needed), used to verify
core algorithms of the embedded software are correct before
porting to ZYNQ.

运行方法 | How to run:
    python test_basic.py
    python test_basic.py -v              # 详细输出 | Verbose output
    python test_basic.py TestClassName   # 运行指定测试类

依赖 Dependencies:
    pip install numpy matplotlib

============================================================================
"""

import sys
import os
import unittest
import math

import numpy as np

# 确保可以找到项目模块 | Ensure project modules are findable
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))


# ============================================================
# 常量定义（与 C 代码保持一致）| Constants (matching C code)
# ============================================================

DDS_CLK_FREQ = 200e6       # DDS 系统时钟 200 MHz | DDS clock 200 MHz
DDS_PHASE_WIDTH = 48        # 相位累加器位宽 | Phase accumulator width
DDS_PHASE_MODULO = 2 ** 48  # 2^48
WAVEFORM_POINTS = 65536     # 波形查找表点数 | LUT points
DAC8562_MAX_CODE = 65535    # DAC8562 满量程码值 | Full-scale code
AD9744_MAX_VALUE = 16383    # AD9744 满量程值 (14-bit) | Full-scale value (14-bit)
AD9744_MID_VALUE = 8191     # AD9744 中点值 | Mid-scale value


# ============================================================
# 波形生成算法（Python 版本，与 C 代码逻辑对应）
# Waveform Generation Algorithms (Python version, matching C logic)
# ============================================================

class WaveformGenerator:
    """
    波形生成器 (Python 版本)
    Waveform Generator (Python version)

    生成各种波形的采样数据，每个采样点为 14-bit 无符号整数 (0~16383)，
    对应 AD9744 DAC 的输入范围。

    Generates waveform sample data, each sample as a 14-bit unsigned
    integer (0~16383), matching the AD9744 DAC input range.
    """

    @staticmethod
    def sine(points=WAVEFORM_POINTS):
        """正弦波 | Sine wave"""
        t = np.arange(points)
        phase = 2.0 * np.pi * t / points
        val = np.sin(phase)
        return (AD9744_MID_VALUE + val * (AD9744_MAX_VALUE / 2.0)).astype(np.uint16)

    @staticmethod
    def square(points=WAVEFORM_POINTS):
        """方波 | Square wave"""
        t = np.arange(points)
        phase = 2.0 * np.pi * t / points
        val = np.where(np.sin(phase) >= 0, 1.0, -1.0)
        return (AD9744_MID_VALUE + val * (AD9744_MAX_VALUE / 2.0)).astype(np.uint16)

    @staticmethod
    def triangle(points=WAVEFORM_POINTS):
        """三角波 | Triangle wave"""
        t = np.arange(points) / points
        val = np.where(t < 0.25, 4.0 * t,
              np.where(t < 0.75, 2.0 - 4.0 * t, -4.0 + 4.0 * t))
        return (AD9744_MID_VALUE + val * (AD9744_MAX_VALUE / 2.0)).astype(np.uint16)

    @staticmethod
    def ramp(points=WAVEFORM_POINTS):
        """锯齿波 | Sawtooth (ramp) wave"""
        t = np.arange(points) / points
        val = 2.0 * t - 1.0
        return (AD9744_MID_VALUE + val * (AD9744_MAX_VALUE / 2.0)).astype(np.uint16)

    @staticmethod
    def pulse(points=WAVEFORM_POINTS, duty=0.10):
        """脉冲波 | Pulse wave"""
        t = np.arange(points)
        val = np.where(t < points * duty, 1.0, -1.0)
        return (AD9744_MID_VALUE + val * (AD9744_MAX_VALUE / 2.0)).astype(np.uint16)

    @staticmethod
    def noise(points=WAVEFORM_POINTS, seed=12345):
        """白噪声 | White noise"""
        rng = np.random.RandomState(seed)
        val = 2.0 * rng.random(points) - 1.0
        return (AD9744_MID_VALUE + val * (AD9744_MAX_VALUE / 2.0)).astype(np.uint16)

    @staticmethod
    def sinc(points=WAVEFORM_POINTS):
        """Sinc 函数 | Sinc function"""
        x_range = 6.0 * np.pi
        x = np.linspace(-x_range, x_range, points)
        val = np.where(np.abs(x) < 1e-10, 1.0, np.sin(x) / x)
        return (AD9744_MID_VALUE + val * (AD9744_MAX_VALUE / 2.0)).astype(np.uint16)

    @staticmethod
    def exp_decay(points=WAVEFORM_POINTS):
        """指数衰减 | Exponential decay"""
        t = np.arange(points) / points
        val = np.exp(-3.0 * t) * np.sin(2.0 * np.pi * 8.0 * t)
        return (AD9744_MID_VALUE + val * (AD9744_MAX_VALUE / 2.0)).astype(np.uint16)

    @staticmethod
    def ecg(points=WAVEFORM_POINTS):
        """心电图 | ECG waveform"""
        cycle = points // 4
        result = np.zeros(points)
        for i in range(points):
            t = (i % cycle) / cycle
            val = 0.0
            # P 波
            val += 0.15 * math.exp(-(t - 0.15)**2 / (2 * 0.02**2))
            # Q 波
            val -= 0.08 * math.exp(-(t - 0.28)**2 / (2 * 0.005**2))
            # R 波
            val += 1.0 * math.exp(-(t - 0.30)**2 / (2 * 0.008**2))
            # S 波
            val -= 0.15 * math.exp(-(t - 0.32)**2 / (2 * 0.008**2))
            # T 波
            val += 0.25 * math.exp(-(t - 0.55)**2 / (2 * 0.04**2))
            result[i] = val
        return (AD9744_MID_VALUE + result * (AD9744_MAX_VALUE / 2.0)).astype(np.uint16)

    @staticmethod
    def half_rect(points=WAVEFORM_POINTS):
        """半波整流 | Half-wave rectified"""
        phase = 2.0 * np.pi * np.arange(points) / points
        val = np.maximum(np.sin(phase), 0.0)
        return (val * AD9744_MAX_VALUE).astype(np.uint16)

    @staticmethod
    def full_rect(points=WAVEFORM_POINTS):
        """全波整流 | Full-wave rectified"""
        phase = 2.0 * np.pi * np.arange(points) / points
        val = np.abs(np.sin(phase))
        return (val * AD9744_MAX_VALUE).astype(np.uint16)

    @staticmethod
    def staircase(points=WAVEFORM_POINTS, num_steps=16):
        """阶梯波 | Staircase wave"""
        samples_per_step = points // num_steps
        step = np.arange(points) // samples_per_step
        step = np.minimum(step, num_steps - 1)
        val = -1.0 + 2.0 * step / (num_steps - 1)
        return (AD9744_MID_VALUE + val * (AD9744_MAX_VALUE / 2.0)).astype(np.uint16)


# ============================================================
# DDS 频率计算函数（Python 版本）
# DDS Frequency Calculation Functions (Python version)
# ============================================================

def dds_calc_fcw(freq_hz):
    """
    计算频率控制字 (FCW) | Calculate Frequency Control Word

    公式 Formula:
        FCW = f_out * 2^N / f_clk

    其中 Where:
        N = 48 (相位累加器位宽)
        f_clk = 200 MHz

    参数 Args:
        freq_hz: 目标输出频率 (Hz)

    返回 Returns:
        int: 48-bit 频率控制字
    """
    fcw = freq_hz * DDS_PHASE_MODULO / DDS_CLK_FREQ
    fcw = min(fcw, DDS_PHASE_MODULO / 2 - 1)  # 限制最大频率 | Cap max freq
    return int(round(fcw))


def dds_freq_from_fcw(fcw):
    """
    从 FCW 反算输出频率 | Calculate output frequency from FCW

    公式 Formula:
        f_out = FCW * f_clk / 2^N

    参数 Args:
        fcw: 频率控制字

    返回 Returns:
        float: 输出频率 (Hz)
    """
    return fcw * DDS_CLK_FREQ / DDS_PHASE_MODULO


# ============================================================
# DAC8562 数值计算函数
# DAC8562 Value Calculation Functions
# ============================================================

def dac8562_vpp_to_code(vpp, vpp_max=10.0):
    """
    将峰峰值电压转换为 DAC8562 通道 A 码值
    Convert Vpp voltage to DAC8562 Channel A code

    线性映射 Linear mapping:
        code = (vpp / vpp_max) * 65535

    参数 Args:
        vpp: 目标峰峰值 (0~10V)
        vpp_max: 最大峰峰值 (V)

    返回 Returns:
        int: 16-bit DAC 码值 (0~65535)
    """
    vpp = max(0.0, min(vpp, vpp_max))
    return int(round(vpp / vpp_max * DAC8562_MAX_CODE))


def dac8562_offset_to_code(offset_v, offset_max=5.0):
    """
    将偏置电压转换为 DAC8562 通道 B 码值
    Convert offset voltage to DAC8562 Channel B code

    双极性偏置映射到单极性 DAC Bipolar offset mapped to unipolar DAC:
        code = (offset + offset_max) / (2 * offset_max) * 65535

    例如 Example:
        -5V  -> 0
         0V  -> 32767
        +5V  -> 65535

    参数 Args:
        offset_v: 偏置电压 (-5V ~ +5V)
        offset_max: 最大偏置 (V)

    返回 Returns:
        int: 16-bit DAC 码值 (0~65535)
    """
    offset_v = max(-offset_max, min(offset_v, offset_max))
    return int(round((offset_v + offset_max) / (2.0 * offset_max) * DAC8562_MAX_CODE))


def ad9744_from_normalized(normalized):
    """
    将归一化值 (-1~+1) 转换为 AD9744 14-bit 偏移二进制码
    Convert normalized value (-1~+1) to AD9744 14-bit offset binary code

    参数 Args:
        normalized: 归一化值 (-1.0 ~ +1.0)

    返回 Returns:
        int: 14-bit 码值 (0~16383)
    """
    normalized = max(-1.0, min(normalized, 1.0))
    return int(round((normalized + 1.0) / 2.0 * AD9744_MAX_VALUE))


# ============================================================
# 测试用例 | Test Cases
# ============================================================

class TestWaveformGeneration(unittest.TestCase):
    """
    波形生成算法测试
    Waveform Generation Algorithm Tests
    """

    def test_sine_range(self):
        """正弦波值域测试 | Sine wave range test"""
        data = WaveformGenerator.sine()
        self.assertEqual(len(data), WAVEFORM_POINTS)
        # 所有值应在 14-bit 范围内 (0~16383)
        self.assertTrue(np.all(data >= 0))
        self.assertTrue(np.all(data <= AD9744_MAX_VALUE))
        # 正弦波应包含接近最大值和最小值的点
        self.assertTrue(np.max(data) > AD9744_MAX_VALUE * 0.95)
        self.assertTrue(np.min(data) < AD9744_MAX_VALUE * 0.05)

    def test_sine_symmetry(self):
        """正弦波对称性测试 | Sine wave symmetry test"""
        data = WaveformGenerator.sine()
        # 前半周期和后半周期应关于中点对称
        half = WAVEFORM_POINTS // 2
        # sin(x) 的前半和后半关于中点对称: data[i] + data[i+N/2] = 2*mid
        sums = data[:half].astype(int) + data[half:2*half].astype(int)
        expected = 2 * AD9744_MID_VALUE
        # 允许 +/-2 的量化误差
        np.testing.assert_allclose(sums, expected, atol=2)

    def test_square_values(self):
        """方波值测试 | Square wave value test"""
        data = WaveformGenerator.square()
        # 方波应该只有两个主要值: 接近最大和接近最小
        unique_vals = np.unique(data)
        self.assertEqual(len(unique_vals), 2)

    def test_triangle_linearity(self):
        """三角波线性测试 | Triangle wave linearity test"""
        data = WaveformGenerator.triangle()
        # 上升段应近似线性 (连续差值接近常数)
        # 前 1/4 周期是上升段
        quarter = WAVEFORM_POINTS // 4
        rising = data[:quarter].astype(int)
        diffs = np.diff(rising)
        # 差值应接近常数
        self.assertTrue(np.std(diffs) < 2.0, "三角波上升段应近似线性")

    def test_ramp_monotonic(self):
        """锯齿波单调性测试 | Ramp monotonicity test"""
        data = WaveformGenerator.ramp()
        # 锯齿波整体应单调递增 (除了跳变点)
        diffs = np.diff(data.astype(int))
        increasing = np.sum(diffs > 0)
        # 几乎所有差值都应为正 (除了回跳点)
        self.assertGreater(increasing, WAVEFORM_POINTS * 0.99)

    def test_pulse_duty_cycle(self):
        """脉冲波占空比测试 | Pulse duty cycle test"""
        duty = 0.10
        data = WaveformGenerator.pulse(duty=duty)
        # 高电平点数应约为 duty * total
        high_count = np.sum(data > AD9744_MID_VALUE)
        actual_duty = high_count / WAVEFORM_POINTS
        self.assertAlmostEqual(actual_duty, duty, delta=0.01)

    def test_noise_randomness(self):
        """噪声随机性测试 | Noise randomness test"""
        data = WaveformGenerator.noise(seed=42)
        # 噪声应覆盖大部分值域
        self.assertEqual(len(data), WAVEFORM_POINTS)
        # 值域覆盖率应大于 30%
        unique_count = len(np.unique(data))
        coverage = unique_count / WAVEFORM_POINTS
        self.assertGreater(coverage, 0.3)

    def test_sinc_peak_at_center(self):
        """Sinc 峰值在中点测试 | Sinc peak at center test"""
        data = WaveformGenerator.sinc()
        # sinc(0) 应在数组中心附近取得最大值
        center = WAVEFORM_POINTS // 2
        max_idx = np.argmax(data)
        self.assertAlmostEqual(max_idx, center, delta=5)

    def test_staircase_steps(self):
        """阶梯波步数测试 | Staircase step count test"""
        num_steps = 16
        data = WaveformGenerator.staircase(num_steps=num_steps)
        unique_vals = np.unique(data)
        self.assertEqual(len(unique_vals), num_steps)

    def test_full_rect_no_negative(self):
        """全波整流无负值测试 | Full-wave rect no negative test"""
        data = WaveformGenerator.full_rect()
        # 全波整流后所有值应为非负 (0~DAC_MAX)
        self.assertTrue(np.all(data >= 0))

    def test_half_rect_has_zeros(self):
        """半波整流有零值测试 | Half-wave rect has zeros test"""
        data = WaveformGenerator.half_rect()
        # 半波整流后负半周变为 0
        zero_count = np.sum(data == 0)
        # 大约 50% 的点应为零
        zero_ratio = zero_count / WAVEFORM_POINTS
        self.assertAlmostEqual(zero_ratio, 0.5, delta=0.02)

    def test_all_waveforms_correct_length(self):
        """所有波形长度正确测试 | All waveforms correct length test"""
        generators = [
            WaveformGenerator.sine,
            WaveformGenerator.square,
            WaveformGenerator.triangle,
            WaveformGenerator.ramp,
            WaveformGenerator.pulse,
            WaveformGenerator.noise,
            WaveformGenerator.sinc,
            WaveformGenerator.exp_decay,
            WaveformGenerator.half_rect,
            WaveformGenerator.full_rect,
            WaveformGenerator.staircase,
        ]
        for gen in generators:
            data = gen()
            self.assertEqual(len(data), WAVEFORM_POINTS,
                           f"{gen.__name__} length incorrect")

    def test_all_waveforms_in_valid_range(self):
        """所有波形值在有效范围内测试 | All waveforms valid range test"""
        generators = [
            WaveformGenerator.sine,
            WaveformGenerator.square,
            WaveformGenerator.triangle,
            WaveformGenerator.ramp,
            WaveformGenerator.pulse,
            WaveformGenerator.noise,
            WaveformGenerator.sinc,
            WaveformGenerator.exp_decay,
            WaveformGenerator.half_rect,
            WaveformGenerator.full_rect,
            WaveformGenerator.staircase,
        ]
        for gen in generators:
            data = gen()
            self.assertTrue(np.all(data >= 0),
                          f"{gen.__name__} has negative values")
            self.assertTrue(np.all(data <= AD9744_MAX_VALUE),
                          f"{gen.__name__} exceeds 14-bit range")


class TestDDSFrequency(unittest.TestCase):
    """
    DDS 频率控制字计算测试
    DDS Frequency Control Word Calculation Tests
    """

    def test_fcw_calculation_1khz(self):
        """1kHz FCW 计算测试 | 1kHz FCW calculation test"""
        fcw = dds_calc_fcw(1000.0)
        # 反算频率应接近 1kHz
        freq = dds_freq_from_fcw(fcw)
        self.assertAlmostEqual(freq, 1000.0, delta=0.01)

    def test_fcw_calculation_1mhz(self):
        """1MHz FCW 计算测试 | 1MHz FCW calculation test"""
        fcw = dds_calc_fcw(1e6)
        freq = dds_freq_from_fcw(fcw)
        self.assertAlmostEqual(freq, 1e6, delta=1.0)

    def test_fcw_calculation_70mhz(self):
        """70MHz FCW 计算测试 | 70MHz FCW calculation test"""
        fcw = dds_calc_fcw(70e6)
        freq = dds_freq_from_fcw(fcw)
        self.assertAlmostEqual(freq, 70e6, delta=10.0)

    def test_fcw_zero_frequency(self):
        """零频率 FCW 测试 | Zero frequency FCW test"""
        fcw = dds_calc_fcw(0.0)
        self.assertEqual(fcw, 0)

    def test_fcw_proportionality(self):
        """FCW 线性比例测试 | FCW linearity test"""
        # FCW 应与频率成正比 (DDS 的核心特性)
        # FCW should be proportional to frequency (core DDS property)
        fcw_1khz = dds_calc_fcw(1000.0)
        fcw_2khz = dds_calc_fcw(2000.0)
        self.assertAlmostEqual(fcw_2khz / fcw_1khz, 2.0, places=5)

    def test_frequency_resolution(self):
        """频率分辨率测试 | Frequency resolution test"""
        # 频率分辨率 = f_clk / 2^48 = 0.0007 Hz
        freq_res = DDS_CLK_FREQ / DDS_PHASE_MODULO
        self.assertAlmostEqual(freq_res, 0.0007, delta=0.0001)
        print(f"    DDS frequency resolution: {freq_res:.6f} Hz")

    def test_fcw_max_frequency_clamp(self):
        """FCW 最大频率限幅测试 | FCW max frequency clamp test"""
        # FCW 不应超过 2^47 (Nyquist 限制)
        # FCW should not exceed 2^47 (Nyquist limit)
        fcw = dds_calc_fcw(200e6)  # 超过 f_clk/2
        self.assertLess(fcw, 2**47)


class TestDAC8562Conversion(unittest.TestCase):
    """
    DAC8562 数值转换测试
    DAC8562 Value Conversion Tests
    """

    def test_vpp_zero(self):
        """0Vpp 码值测试 | 0Vpp code test"""
        code = dac8562_vpp_to_code(0.0)
        self.assertEqual(code, 0)

    def test_vpp_max(self):
        """10Vpp 码值测试 | 10Vpp code test"""
        code = dac8562_vpp_to_code(10.0)
        self.assertEqual(code, DAC8562_MAX_CODE)

    def test_vpp_mid(self):
        """5Vpp 码值测试 | 5Vpp code test"""
        code = dac8562_vpp_to_code(5.0)
        expected = int(round(5.0 / 10.0 * DAC8562_MAX_CODE))
        self.assertEqual(code, expected)

    def test_vpp_overrange(self):
        """超范围 Vpp 限幅测试 | Over-range Vpp clamp test"""
        code = dac8562_vpp_to_code(15.0)  # 超过最大值
        self.assertEqual(code, DAC8562_MAX_CODE)

    def test_vpp_negative(self):
        """负 Vpp 限幅测试 | Negative Vpp clamp test"""
        code = dac8562_vpp_to_code(-1.0)
        self.assertEqual(code, 0)

    def test_offset_negative_max(self):
        """-5V 偏置码值测试 | -5V offset code test"""
        code = dac8562_offset_to_code(-5.0)
        self.assertEqual(code, 0)

    def test_offset_positive_max(self):
        """+5V 偏置码值测试 | +5V offset code test"""
        code = dac8562_offset_to_code(5.0)
        self.assertEqual(code, DAC8562_MAX_CODE)

    def test_offset_zero(self):
        """0V 偏置码值测试 | 0V offset code test"""
        code = dac8562_offset_to_code(0.0)
        expected = int(round(0.5 * DAC8562_MAX_CODE))
        self.assertAlmostEqual(code, expected, delta=1)

    def test_offset_symmetry(self):
        """偏置对称性测试 | Offset symmetry test"""
        # +V 和 -V 的码值应关于中点对称
        code_pos = dac8562_offset_to_code(3.0)
        code_neg = dac8562_offset_to_code(-3.0)
        code_mid = dac8562_offset_to_code(0.0)
        self.assertEqual(code_pos + code_neg, 2 * code_mid)


class TestAD9744Conversion(unittest.TestCase):
    """
    AD9744 数值转换测试
    AD9744 Value Conversion Tests
    """

    def test_normalized_positive_max(self):
        """+1.0 归一化值测试 | +1.0 normalized value test"""
        code = ad9744_from_normalized(1.0)
        self.assertEqual(code, AD9744_MAX_VALUE)

    def test_normalized_negative_max(self):
        """-1.0 归一化值测试 | -1.0 normalized value test"""
        code = ad9744_from_normalized(-1.0)
        self.assertEqual(code, 0)

    def test_normalized_zero(self):
        """0.0 归一化值测试 | 0.0 normalized value test"""
        code = ad9744_from_normalized(0.0)
        self.assertAlmostEqual(code, AD9744_MID_VALUE, delta=1)

    def test_normalized_clamp(self):
        """超范围归一化值限幅测试 | Over-range normalized clamp test"""
        code_over = ad9744_from_normalized(2.0)
        code_under = ad9744_from_normalized(-2.0)
        self.assertEqual(code_over, AD9744_MAX_VALUE)
        self.assertEqual(code_under, 0)


class TestWaveformVisualization(unittest.TestCase):
    """
    波形可视化测试 (生成波形图片用于人工验证)
    Waveform Visualization Tests (generates images for manual verification)
    """

    @staticmethod
    def _save_waveform_plot(data, filename, title=""):
        """保存波形图到文件 | Save waveform plot to file"""
        try:
            import matplotlib
            matplotlib.use('Agg')
            import matplotlib.pyplot as plt

            fig, ax = plt.subplots(figsize=(10, 3))
            ax.plot(data[::64], linewidth=0.8)
            ax.set_title(title)
            ax.set_xlabel('Sample')
            ax.set_ylabel('DAC Value (14-bit)')
            ax.set_ylim(-500, 17000)
            ax.grid(True, alpha=0.3)
            plt.tight_layout()

            out_dir = os.path.join(os.path.dirname(__file__), 'output')
            os.makedirs(out_dir, exist_ok=True)
            filepath = os.path.join(out_dir, filename)
            fig.savefig(filepath, dpi=100)
            plt.close(fig)
            print(f"    Waveform saved: {filepath}")
            return filepath
        except ImportError:
            print("    matplotlib not installed, skipping visualization")
            return None

    def test_visualize_all_waveforms(self):
        """可视化所有波形 | Visualize all waveforms"""
        waveforms = {
            'sine': WaveformGenerator.sine,
            'square': WaveformGenerator.square,
            'triangle': WaveformGenerator.triangle,
            'ramp': WaveformGenerator.ramp,
            'pulse': WaveformGenerator.pulse,
            'noise': WaveformGenerator.noise,
            'sinc': WaveformGenerator.sinc,
            'exp_decay': WaveformGenerator.exp_decay,
            'staircase': WaveformGenerator.staircase,
            'full_rect': WaveformGenerator.full_rect,
            'half_rect': WaveformGenerator.half_rect,
            'ecg': WaveformGenerator.ecg,
        }

        for name, gen_func in waveforms.items():
            data = gen_func()
            self._save_waveform_plot(data, f'waveform_{name}.png',
                                    title=f'TinyAWG: {name}')

    def test_dds_frequency_sweep_simulation(self):
        """DDS 频率扫描模拟 | DDS frequency sweep simulation"""
        try:
            import matplotlib
            matplotlib.use('Agg')
            import matplotlib.pyplot as plt
        except ImportError:
            return

        fig, axes = plt.subplots(4, 1, figsize=(10, 8))
        sample_rate = 200e6
        num_display = 500

        for idx, freq in enumerate([1e3, 100e3, 1e6, 10e6]):
            t = np.arange(num_display) / sample_rate
            signal = np.sin(2 * np.pi * freq * t)
            axes[idx].plot(t * 1e6, signal, linewidth=0.8)
            axes[idx].set_title(f'{freq/1e3:.0f} kHz')
            axes[idx].set_ylabel('Amplitude')
            axes[idx].grid(True, alpha=0.3)
            axes[idx].set_xlabel('Time (us)')

        plt.suptitle('TinyAWG DDS Output Simulation (200 MSa/s)')
        plt.tight_layout()

        out_dir = os.path.join(os.path.dirname(__file__), 'output')
        os.makedirs(out_dir, exist_ok=True)
        filepath = os.path.join(out_dir, 'dds_frequency_sweep.png')
        fig.savefig(filepath, dpi=100)
        plt.close(fig)
        print(f"    DDS sweep image: {filepath}")


# ============================================================
# 运行入口 | Run Entry Point
# ============================================================

if __name__ == '__main__':
    print("=" * 60)
    print("TinyAWG Basic Tests")
    print("=" * 60)
    print(f"DDS Parameters:")
    print(f"  System Clock:          {DDS_CLK_FREQ/1e6:.0f} MHz")
    print(f"  Phase Width:           {DDS_PHASE_WIDTH} bit")
    print(f"  Freq Resolution:       {DDS_CLK_FREQ/DDS_PHASE_MODULO:.6f} Hz")
    print(f"  LUT Points:            {WAVEFORM_POINTS}")
    print(f"  AD9744 Resolution:     14-bit ({AD9744_MAX_VALUE})")
    print(f"  DAC8562 Resolution:    16-bit ({DAC8562_MAX_CODE})")
    print("=" * 60)
    print()

    unittest.main(verbosity=2)
