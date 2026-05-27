"""
波形显示模块
Waveform Display Module

功能：在 PC 端绘制示波器风格的波形图
Features: Plot oscilloscope-style waveform graphs on PC
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec


class WaveformPlotter:
    """
    波形绘制器
    Waveform Plotter

    绘制带网格的示波器风格波形图。
    Draws oscilloscope-style waveform with grid.
    """

    def __init__(self, sample_rate=1000000, vref=3.3, width=160, height=128):
        """
        初始化绘制器
        Initialize plotter

        Args:
            sample_rate: 采样率 (Hz) | Sample rate
            vref: ADC 参考电压 (V) | Reference voltage
            width: 屏幕宽度 (像素) | Screen width
            height: 屏幕高度 (像素) | Screen height
        """
        self.sample_rate = sample_rate
        self.vref = vref
        self.width = width
        self.height = height

        # 创建图形
        self.fig = plt.figure(figsize=(10, 6))
        self.fig.patch.set_facecolor('#1a1a2e')    # 深色背景

        # 使用 GridSpec 布局：上面波形，下面信息栏
        gs = gridspec.GridSpec(2, 1, height_ratios=[5, 1])
        self.ax_wave = self.fig.add_subplot(gs[0])
        self.ax_info = self.fig.add_subplot(gs[1])

        self._setup_axes()
        self.fig.tight_layout()

    def _setup_axes(self):
        """
        设置坐标轴样式（模拟示波器界面）
        Set axis style (oscilloscope-like interface)
        """
        # 波形区域
        self.ax_wave.set_facecolor('#0a0a1a')
        self.ax_wave.set_xlim(0, self.width)
        self.ax_wave.set_ylim(0, 4095)
        self.ax_wave.set_xlabel('Sample Index', color='#aaaaaa')
        self.ax_wave.set_ylabel('ADC Value', color='#aaaaaa')
        self.ax_wave.tick_params(colors='#aaaaaa')
        self.ax_wave.grid(True, color='#1a3a1a', alpha=0.5, linewidth=0.5)

        # 中心十字线
        self.ax_wave.axhline(y=2048, color='#2a5a2a', linewidth=0.8, alpha=0.7)
        self.ax_wave.axvline(x=self.width / 2, color='#2a5a2a', linewidth=0.8, alpha=0.7)

        # 信息栏
        self.ax_info.set_facecolor('#0a0a1a')
        self.ax_info.set_xlim(0, 10)
        self.ax_info.set_ylim(0, 1)
        self.ax_info.axis('off')    # 隐藏坐标轴

    def update(self, adc_data, freq=0, vpp=0, title='Pocket Oscilloscope'):
        """
        更新波形显示
        Update waveform display

        Args:
            adc_data: ADC 数据数组
            freq: 测量频率 (Hz)
            vpp: 测量峰峰值 (V)
            title: 图表标题
        """
        # 清除旧波形
        self.ax_wave.clear()
        self.ax_info.clear()
        self._setup_axes()

        if len(adc_data) == 0:
            self.ax_wave.set_title(title, color='#00ff00', fontsize=14)
            return

        # X 轴：采样点索引
        x = np.arange(len(adc_data))

        # 绘制波形 (绿色，模拟示波器荧光)
        self.ax_wave.plot(x, adc_data, color='#00ff00', linewidth=0.8, alpha=0.9)

        # 标题
        self.ax_wave.set_title(title, color='#00ff00', fontsize=14, fontweight='bold')

        # 信息栏
        self.ax_info.set_facecolor('#0a0a1a')

        # 格式化频率显示
        if freq > 1e6:
            freq_str = f'{freq/1e6:.2f} MHz'
        elif freq > 1e3:
            freq_str = f'{freq/1e3:.2f} kHz'
        else:
            freq_str = f'{freq:.1f} Hz'

        # 显示参数
        info_text = (f'Freq: {freq_str}   '
                     f'Vpp: {vpp:.3f} V   '
                     f'Samples: {len(adc_data)}   '
                     f'Rate: {self.sample_rate/1e3:.0f} ksps')

        self.ax_info.text(5, 0.5, info_text,
                         ha='center', va='center',
                         color='#00ff00', fontsize=11,
                         fontfamily='monospace')

    def save_screenshot(self, filename='waveform.png'):
        """
        保存波形截图
        Save waveform screenshot

        Args:
            filename: 文件名
        """
        self.fig.savefig(filename, dpi=150, facecolor=self.fig.get_facecolor())
        print(f"截图已保存: {filename}")

    def show(self):
        """显示图形窗口"""
        plt.show()
