"""
基础测试模块
Basic Test Module

功能：测试波形分析和触发逻辑的正确性
Features: Test waveform analysis and trigger logic correctness
"""

import sys
import os
import unittest
import numpy as np

# 添加源代码目录到路径
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

from waveform import WaveformAnalyzer
from trigger import TriggerEngine
from utils import format_frequency, format_voltage


class TestWaveformAnalyzer(unittest.TestCase):
    """
    波形分析器测试
    Waveform Analyzer Tests
    """

    def setUp(self):
        """测试前准备"""
        self.analyzer = WaveformAnalyzer(vref=3.3)
        self.sample_rate = 1000000    # 1 Msps

    def _generate_sine(self, freq, sample_rate, num_points, vpp=1.0, offset=1.65):
        """
        生成测试用正弦波 ADC 数据
        Generate test sine wave ADC data

        Args:
            freq: 信号频率 (Hz)
            sample_rate: 采样率 (Hz)
            num_points: 采样点数
            vpp: 峰峰值 (V)
            offset: 直流偏置 (V)

        Returns:
            np.ndarray: ADC 值数组
        """
        t = np.arange(num_points) / sample_rate
        voltage = offset + (vpp / 2) * np.sin(2 * np.pi * freq * t)
        adc_data = np.clip(voltage * 4095 / 3.3, 0, 4095).astype(int)
        return adc_data

    def test_frequency_measurement_1khz(self):
        """测试 1kHz 频率测量精度"""
        data = self._generate_sine(1000, self.sample_rate, 2048)
        freq = self.analyzer.measure_frequency(data, self.sample_rate)
        # 允许 5% 误差
        self.assertAlmostEqual(freq, 1000, delta=50)

    def test_frequency_measurement_10khz(self):
        """测试 10kHz 频率测量精度"""
        data = self._generate_sine(10000, self.sample_rate, 2048)
        freq = self.analyzer.measure_frequency(data, self.sample_rate)
        self.assertAlmostEqual(freq, 10000, delta=500)

    def test_frequency_measurement_100hz(self):
        """测试 100Hz 频率测量精度"""
        data = self._generate_sine(100, self.sample_rate, 10240)
        freq = self.analyzer.measure_frequency(data, self.sample_rate)
        self.assertAlmostEqual(freq, 100, delta=5)

    def test_vpp_measurement(self):
        """测试峰峰值测量"""
        # 创建 1Vpp 信号，偏置 1.65V
        data = self._generate_sine(1000, self.sample_rate, 2048, vpp=1.0, offset=1.65)
        vpp = self.analyzer.measure_vpp(data)
        # 允许 10% 误差 (量化误差)
        self.assertAlmostEqual(vpp, 1.0, delta=0.1)

    def test_dc_offset_measurement(self):
        """测试直流偏置测量"""
        data = self._generate_sine(1000, self.sample_rate, 2048, offset=1.65)
        offset = self.analyzer.measure_dc_offset(data)
        self.assertAlmostEqual(offset, 1.65, delta=0.05)

    def test_empty_data(self):
        """测试空数据处理"""
        data = np.array([], dtype=int)
        self.assertEqual(self.analyzer.measure_frequency(data, self.sample_rate), 0.0)
        self.assertEqual(self.analyzer.measure_vpp(data), 0.0)
        self.assertEqual(self.analyzer.measure_dc_offset(data), 0.0)

    def test_adc_voltage_conversion(self):
        """测试 ADC/电压转换"""
        self.assertAlmostEqual(self.analyzer.adc_to_voltage(0), 0.0)
        self.assertAlmostEqual(self.analyzer.adc_to_voltage(4095), 3.3, places=2)
        self.assertAlmostEqual(self.analyzer.adc_to_voltage(2048), 1.65, places=2)

    def test_calculate_center(self):
        """测试中心值计算"""
        data = self._generate_sine(1000, self.sample_rate, 2048, offset=1.65)
        center = self.analyzer.calculate_center(data)
        expected_adc = int(1.65 * 4095 / 3.3)
        self.assertAlmostEqual(center, expected_adc, delta=50)


class TestTriggerEngine(unittest.TestCase):
    """
    触发引擎测试
    Trigger Engine Tests
    """

    def setUp(self):
        """测试前准备"""
        self.trigger = TriggerEngine(mode='auto', slope='rising')

    def _generate_sine(self, freq=1000, sample_rate=1000000, num_points=1024):
        """生成测试正弦波"""
        t = np.arange(num_points) / sample_rate
        voltage = 1.65 + 0.5 * np.sin(2 * np.pi * freq * t)
        return np.clip(voltage * 4095 / 3.3, 0, 4095).astype(int)

    def test_rising_edge_detection(self):
        """测试上升沿检测"""
        data = np.array([100, 200, 3000, 3500, 100, 200])
        level = 2048    # 触发电平

        trigger_point = self.trigger.find_rising_edge(data, level)
        # 在索引 1->2 处 (200->3000)，信号越过 2048
        self.assertEqual(trigger_point, 1)

    def test_falling_edge_detection(self):
        """测试下降沿检测"""
        data = np.array([3000, 3500, 100, 200, 3000, 3500])
        level = 2048

        trigger_point = self.trigger.find_falling_edge(data, level)
        # 在索引 1->2 处 (3500->100)，信号跌破 2048
        self.assertEqual(trigger_point, 1)

    def test_auto_trigger(self):
        """测试自动模式触发"""
        data = self._generate_sine()
        level = int(np.mean(data))

        result = self.trigger.find_trigger(data, level)
        # 自动模式始终返回数据
        self.assertIsNotNone(result)

    def test_trigger_returns_data(self):
        """测试触发后返回数据"""
        data = self._generate_sine()
        level = int(np.mean(data))

        result = self.trigger.find_trigger(data, level, slope='rising')
        self.assertIsNotNone(result)
        self.assertGreater(len(result), 0)

    def test_auto_level_calculation(self):
        """测试自动电平计算"""
        data = self._generate_sine()
        level = self.trigger.auto_level(data)

        # 自动电平应在信号中间
        self.assertGreater(level, 1500)
        self.assertLess(level, 2600)

    def test_set_mode(self):
        """测试模式切换"""
        self.trigger.set_mode('normal')
        self.assertEqual(self.trigger.mode, 'normal')

        self.trigger.set_mode('single')
        self.assertEqual(self.trigger.mode, 'single')
        self.assertTrue(self.trigger.armed)

    def test_set_slope(self):
        """测试边沿切换"""
        self.trigger.set_slope('falling')
        self.assertEqual(self.trigger.slope, 'falling')


class TestUtils(unittest.TestCase):
    """
    工具函数测试
    Utility Function Tests
    """

    def test_format_frequency(self):
        """测试频率格式化"""
        self.assertEqual(format_frequency(1000000), '1.00 MHz')
        self.assertEqual(format_frequency(1500), '1.50 kHz')
        self.assertIn('Hz', format_frequency(500))

    def test_format_voltage(self):
        """测试电压格式化"""
        self.assertIn('V', format_voltage(3.3))
        self.assertIn('mV', format_voltage(0.05))


if __name__ == '__main__':
    unittest.main()
