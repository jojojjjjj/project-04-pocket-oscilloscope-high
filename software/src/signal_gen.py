"""
信号发生器控制模块 (PC 端)
Signal Generator Control Module (PC Side)

功能：通过串口向 STM32 发送信号发生器控制命令
Features: Send signal generator control commands to STM32 via serial port
"""


class SignalGenController:
    """
    信号发生器控制器 (PC 端)
    Signal Generator Controller (PC Side)

    通过串口发送命令给 STM32，控制 AD9833 信号发生器。
    Send commands to STM32 via serial to control AD9833 signal generator.
    """

    # 波形类型映射
    WAVEFORM_TYPES = {
        'sine': 0,        # 正弦波
        'triangle': 1,    # 三角波
        'square': 2,      # 方波
    }

    def __init__(self, serial_connection=None):
        """
        初始化信号发生器控制器
        Initialize signal generator controller

        Args:
            serial_connection: 串口连接对象 (pyserial)
        """
        self.ser = serial_connection
        self.current_freq = 1000       # 当前频率 (Hz)
        self.current_waveform = 0      # 当前波形 (0=正弦, 1=三角, 2=方波)

    def set_serial(self, serial_connection):
        """设置串口连接"""
        self.ser = serial_connection

    def send_command(self, cmd):
        """
        发送命令到 STM32
        Send command to STM32

        Args:
            cmd: 命令字符串 (单字符)

        Returns:
            bool: 是否发送成功
        """
        if self.ser and self.ser.is_open:
            try:
                self.ser.write(cmd.encode('utf-8'))
                return True
            except Exception:
                return False
        return False

    def set_frequency(self, freq_hz):
        """
        设置输出频率
        Set output frequency

        Args:
            freq_hz: 目标频率 (Hz)

        Returns:
            bool: 是否成功
        """
        self.current_freq = freq_hz
        # 发送 "F" 命令触发频率切换
        return self.send_command('F')

    def next_frequency(self):
        """
        切换到下一个预设频率
        Switch to next preset frequency

        Returns:
            bool: 是否成功
        """
        return self.send_command('F')

    def next_waveform(self):
        """
        切换到下一个波形类型
        Switch to next waveform type

        Returns:
            bool: 是否成功
        """
        self.current_waveform = (self.current_waveform + 1) % 3
        return self.send_command('W')

    def set_waveform(self, waveform_type):
        """
        设置波形类型
        Set waveform type

        Args:
            waveform_type: 'sine', 'triangle', 或 'square'

        Returns:
            bool: 是否成功
        """
        if waveform_type in self.WAVEFORM_TYPES:
            self.current_waveform = self.WAVEFORM_TYPES[waveform_type]
            return self.send_command('W')
        return False

    def get_status(self):
        """
        获取当前信号发生器状态
        Get current signal generator status

        Returns:
            dict: 包含频率和波形类型的状态字典
        """
        waveform_names = ['Sine', 'Triangle', 'Square']
        return {
            'frequency': self.current_freq,
            'waveform': waveform_names[self.current_waveform],
            'waveform_id': self.current_waveform,
        }
