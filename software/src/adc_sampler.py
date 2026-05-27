"""
ADC 采样数据接收模块
ADC Sampling Data Receiver Module

功能：通过串口从 STM32 接收 ADC 采样数据
Features: Receive ADC sampling data from STM32 via serial port
"""

import serial
import time
import numpy as np


class SerialADCSampler:
    """
    串口 ADC 数据采样器
    Serial ADC Data Sampler

    通过串口从 STM32 接收 DMA 采集的 ADC 数据。
    数据格式：
      START         (开始标记)
      数值1          (每行一个 ADC 值，0~4095)
      数值2
      ...
      END            (结束标记)

    Receive DMA-collected ADC data from STM32 via serial port.
    Data format:
      START         (start marker)
      value1        (one ADC value per line, 0-4095)
      value2
      ...
      END           (end marker)
    """

    def __init__(self, port='COM3', baudrate=115200, expected_count=1024):
        """
        初始化串口采样器
        Initialize serial sampler

        Args:
            port: 串口号 | Serial port name
            baudrate: 波特率 | Baud rate
            expected_count: 期望的采样点数 | Expected sample count
        """
        self.port = port
        self.baudrate = baudrate
        self.expected_count = expected_count
        self.ser = None      # 串口对象 | Serial object

    def connect(self):
        """
        连接串口
        Connect to serial port

        Returns:
            bool: 连接是否成功
        """
        try:
            self.ser = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                timeout=2,         # 2秒超时
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE
            )
            time.sleep(0.1)        # 等待连接稳定
            return True
        except serial.SerialException as e:
            print(f"串口连接失败: {e}")
            return False

    def disconnect(self):
        """断开串口连接"""
        if self.ser and self.ser.is_open:
            self.ser.close()
            self.ser = None

    def read_waveform(self):
        """
        读取一组波形数据
        Read one waveform dataset

        Returns:
            list[int]: ADC 值列表，失败返回 None
        """
        if not self.ser or not self.ser.is_open:
            return None

        data = []
        reading = False
        start_time = time.time()

        while True:
            # 超时检查 (5秒)
            if time.time() - start_time > 5:
                return None

            try:
                line = self.ser.readline()
                if not line:
                    continue

                # 解码 (忽略错误字符)
                line = line.decode('utf-8', errors='ignore').strip()

                if not line:
                    continue

                if line in ('START', '=== Waveform Data Start ==='):
                    data = []
                    reading = True
                    continue

                if line in ('END', '=== Waveform Data End ==='):
                    reading = False
                    if len(data) > 0:
                        return data
                    continue

                if reading:
                    try:
                        value = int(line)
                        # 有效性检查 (12-bit ADC: 0~4095)
                        if 0 <= value <= 4095:
                            data.append(value)
                    except ValueError:
                        # 非数字行，忽略
                        continue

            except serial.SerialException:
                return None

    def read_waveform_numpy(self):
        """
        读取波形数据并返回 numpy 数组
        Read waveform data as numpy array

        Returns:
            np.ndarray: ADC 值数组，失败返回 None
        """
        data = self.read_waveform()
        if data:
            return np.array(data, dtype=np.uint16)
        return None

    def is_connected(self):
        """检查串口是否连接"""
        return self.ser is not None and self.ser.is_open
