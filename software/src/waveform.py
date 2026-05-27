"""
波形分析模块
Waveform Analysis Module

功能：频率测量、幅度测量、波形参数计算
Features: Frequency measurement, amplitude measurement, waveform parameter calculation
"""

import numpy as np


class WaveformAnalyzer:
    """
    波形分析器
    Waveform Analyzer

    提供波形信号的各种测量功能。
    Provides various measurement functions for waveform signals.
    """

    def __init__(self, vref=3.3):
        """
        初始化波形分析器
        Initialize waveform analyzer

        Args:
            vref: ADC 参考电压 (V) | ADC reference voltage
        """
        self.vref = vref
        self.max_adc = 4095    # 12-bit ADC 最大值

    def adc_to_voltage(self, adc_value):
        """
        ADC 值转换为电压
        Convert ADC value to voltage

        Args:
            adc_value: ADC 数值 (0~4095)

        Returns:
            float: 电压值 (V)
        """
        return adc_value * self.vref / self.max_adc

    def voltage_to_adc(self, voltage):
        """
        电压转换为 ADC 值
        Convert voltage to ADC value

        Args:
            voltage: 电压值 (V)

        Returns:
            int: ADC 数值
        """
        return int(voltage * self.max_adc / self.vref)

    def measure_frequency(self, adc_data, sample_rate):
        """
        测量信号频率 (过零法)
        Measure signal frequency (zero-crossing method)

        通过检测信号上升过零点来计算频率。
        通过检测上升过零点之间的间隔来计算周期。

        Args:
            adc_data: ADC 数据数组
            sample_rate: 采样率 (Hz)

        Returns:
            float: 测量频率 (Hz)，0 表示测量失败
        """
        if len(adc_data) < 10:
            return 0.0

        # 计算信号中心值 (均值)
        center = np.mean(adc_data)

        # 寻找上升过零点
        crossings = []
        for i in range(1, len(adc_data)):
            if adc_data[i - 1] < center and adc_data[i] >= center:
                # 线性插值提高精度
                if adc_data[i] != adc_data[i - 1]:
                    frac = (center - adc_data[i - 1]) / (adc_data[i] - adc_data[i - 1])
                    crossings.append(i - 1 + frac)
                else:
                    crossings.append(i)

        # 过零点不足，无法计算频率
        if len(crossings) < 2:
            return 0.0

        # 计算平均周期 (采样点数)
        periods = np.diff(crossings)
        avg_period = np.mean(periods)

        if avg_period <= 0:
            return 0.0

        # 频率 = 采样率 / 每周期采样点数
        frequency = sample_rate / avg_period

        return frequency

    def measure_vpp(self, adc_data):
        """
        测量信号峰峰值 (Vpp)
        Measure peak-to-peak voltage

        Args:
            adc_data: ADC 数据数组

        Returns:
            float: 峰峰值电压 (V)
        """
        if len(adc_data) == 0:
            return 0.0

        v_min = self.adc_to_voltage(np.min(adc_data))
        v_max = self.adc_to_voltage(np.max(adc_data))

        return v_max - v_min

    def measure_dc_offset(self, adc_data):
        """
        测量信号直流偏置
        Measure signal DC offset

        Args:
            adc_data: ADC 数据数组

        Returns:
            float: 直流偏置电压 (V)
        """
        if len(adc_data) == 0:
            return 0.0

        return self.adc_to_voltage(np.mean(adc_data))

    def measure_rms(self, adc_data):
        """
        测量信号有效值 (RMS)
        Measure signal RMS value

        Args:
            adc_data: ADC 数据数组

        Returns:
            float: RMS 电压 (V)
        """
        if len(adc_data) == 0:
            return 0.0

        # 先去除直流分量
        dc = np.mean(adc_data)
        ac_data = adc_data - dc

        # 计算 RMS
        rms_adc = np.sqrt(np.mean(ac_data ** 2))
        return self.adc_to_voltage(rms_adc)

    def calculate_center(self, adc_data):
        """
        计算信号中心值 (用于触发电平)
        Calculate signal center value (for trigger level)

        Args:
            adc_data: ADC 数据数组

        Returns:
            int: 中心 ADC 值
        """
        if len(adc_data) == 0:
            return 2048    # 默认中心值

        return int(np.mean(adc_data))

    def get_all_params(self, adc_data, sample_rate):
        """
        获取所有波形参数
        Get all waveform parameters

        Args:
            adc_data: ADC 数据数组
            sample_rate: 采样率 (Hz)

        Returns:
            dict: 包含所有参数的字典
        """
        return {
            'frequency': self.measure_frequency(adc_data, sample_rate),
            'vpp': self.measure_vpp(adc_data),
            'dc_offset': self.measure_dc_offset(adc_data),
            'rms': self.measure_rms(adc_data),
            'center': self.calculate_center(adc_data),
            'adc_min': int(np.min(adc_data)) if len(adc_data) > 0 else 0,
            'adc_max': int(np.max(adc_data)) if len(adc_data) > 0 else 0,
            'sample_count': len(adc_data),
        }
