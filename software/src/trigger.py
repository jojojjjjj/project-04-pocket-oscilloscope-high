"""
波形触发模块
Waveform Trigger Module

功能：实现示波器的边沿触发逻辑
Features: Implement oscilloscope edge trigger logic
"""

import numpy as np


class TriggerEngine:
    """
    触发引擎
    Trigger Engine

    实现示波器的三种触发模式：
    - 自动 (Auto)：始终显示波形
    - 正常 (Normal)：仅在满足条件时显示
    - 单次 (Single)：触发一次后停止

    Implements three oscilloscope trigger modes:
    - Auto: Always display waveform
    - Normal: Display only when condition met
    - Single: Stop after one trigger
    """

    def __init__(self, mode='auto', slope='rising', holdoff_ms=100):
        """
        初始化触发引擎
        Initialize trigger engine

        Args:
            mode: 触发模式 (auto/normal/single)
            slope: 触发边沿 (rising/falling)
            holdoff_ms: 释抑时间 (ms)
        """
        self.mode = mode
        self.slope = slope
        self.holdoff_ms = holdoff_ms
        self.armed = True       # 单次模式：是否已触发

    def find_rising_edge(self, adc_data, level):
        """
        寻找上升沿触发点
        Find rising edge trigger point

        上升沿定义：前一个点 < 电平 且 后一个点 >= 电平

        Args:
            adc_data: ADC 数据数组
            level: 触发电平 (ADC 值)

        Returns:
            int: 触发点索引，-1 表示未找到
        """
        for i in range(len(adc_data) - 1):
            if adc_data[i] < level and adc_data[i + 1] >= level:
                return i
        return -1

    def find_falling_edge(self, adc_data, level):
        """
        寻找下降沿触发点
        Find falling edge trigger point

        下降沿定义：前一个点 >= 电平 且 后一个点 < 电平

        Args:
            adc_data: ADC 数据数组
            level: 触发电平 (ADC 值)

        Returns:
            int: 触发点索引，-1 表示未找到
        """
        for i in range(len(adc_data) - 1):
            if adc_data[i] >= level and adc_data[i + 1] < level:
                return i
        return -1

    def find_trigger(self, adc_data, level=None, slope=None):
        """
        寻找触发点并返回触发后的数据
        Find trigger point and return triggered data

        Args:
            adc_data: ADC 数据数组
            level: 触发电平 (None=自动计算中心)
            slope: 触发边沿 (None=使用默认)

        Returns:
            np.ndarray: 触发后的数据，None 表示未触发
        """
        if len(adc_data) == 0:
            return None

        # 自动触发电平
        if level is None:
            level = int(np.mean(adc_data))

        # 使用默认边沿
        if slope is None:
            slope = self.slope

        # 选择边沿检测函数
        if slope == 'rising':
            trigger_point = self.find_rising_edge(adc_data, level)
        else:
            trigger_point = self.find_falling_edge(adc_data, level)

        # 处理触发结果
        if trigger_point >= 0:
            # 找到触发点，返回从触发点开始的数据
            return adc_data[trigger_point:]
        elif self.mode == 'auto':
            # 自动模式：未找到触发点也显示
            return adc_data
        elif self.mode == 'single' and not self.armed:
            # 单次模式且已触发：不更新
            return None
        else:
            # 正常模式或单次模式未触发
            return None

    def set_mode(self, mode):
        """
        设置触发模式
        Set trigger mode

        Args:
            mode: auto / normal / single
        """
        self.mode = mode
        if mode == 'single':
            self.armed = True    # 重新武装

    def set_slope(self, slope):
        """
        设置触发边沿
        Set trigger slope

        Args:
            slope: rising / falling
        """
        self.slope = slope

    def auto_level(self, adc_data):
        """
        自动计算触发电平
        Auto-calculate trigger level

        Args:
            adc_data: ADC 数据数组

        Returns:
            int: 推荐的触发电平 (ADC 值)
        """
        if len(adc_data) == 0:
            return 2048    # 12-bit ADC 中间值

        adc_min = np.min(adc_data)
        adc_max = np.max(adc_data)

        return int((adc_min + adc_max) / 2)
