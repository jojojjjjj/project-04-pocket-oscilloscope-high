"""
口袋示波器 PC 端主程序
Pocket Oscilloscope PC Main Program

功能：
1. 通过串口接收 STM32 采集的 ADC 波形数据
2. 在 PC 端绘制波形图
3. 测量信号参数（频率、幅度）
4. 支持配置文件加载

Features:
1. Receive ADC waveform data from STM32 via serial port
2. Plot waveforms on PC
3. Measure signal parameters (frequency, amplitude)
4. Support configuration file loading
"""

import sys
import os
import time
import numpy as np
import matplotlib.pyplot as plt

# 添加当前目录到路径
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from adc_sampler import SerialADCSampler
from waveform import WaveformAnalyzer
from trigger import TriggerEngine
from display import WaveformPlotter
from utils import load_config, setup_logging


def main():
    """主程序入口"""
    # 加载配置
    config = load_config()

    # 设置日志
    logger = setup_logging(config.get('logging', {}))

    # 串口配置
    serial_cfg = config.get('serial', {})
    port = serial_cfg.get('port', 'COM3')
    baudrate = serial_cfg.get('baudrate', 115200)

    # ADC 配置
    adc_cfg = config.get('adc', {})
    vref = adc_cfg.get('vref', 3.3)
    sample_count = adc_cfg.get('sample_count', 1024)
    sample_rate = adc_cfg.get('sample_rate', 1000000)

    # 创建各模块实例
    sampler = SerialADCSampler(port, baudrate, sample_count)
    analyzer = WaveformAnalyzer(vref)
    trigger = TriggerEngine()
    plotter = WaveformPlotter(sample_rate, vref)

    logger.info(f"连接串口 {port}，波特率 {baudrate}...")

    # 连接串口
    if not sampler.connect():
        logger.error(f"无法打开串口 {port}")
        sys.exit(1)

    logger.info("等待波形数据...")
    print("等待波形数据... (按 Ctrl+C 退出)")

    try:
        while True:
            # 读取一组波形数据
            raw_data = sampler.read_waveform()
            if raw_data is None or len(raw_data) == 0:
                logger.warning("未接收到数据，重试...")
                continue

            # 转换为 numpy 数组
            adc_data = np.array(raw_data)

            # 分析波形参数
            frequency = analyzer.measure_frequency(adc_data, sample_rate)
            vpp = analyzer.measure_vpp(adc_data)
            dc_offset = analyzer.measure_dc_offset(adc_data)

            # 打印参数
            print(f"\r频率: {frequency:>10.1f} Hz | "
                  f"Vpp: {vpp:>6.3f} V | "
                  f"偏置: {dc_offset:>5.3f} V | "
                  f"点数: {len(adc_data)}", end='')

            # 触发处理
            trigger_level = analyzer.calculate_center(adc_data)
            triggered_data = trigger.find_trigger(
                adc_data, trigger_level, slope='rising'
            )

            # 绘制波形
            plotter.update(triggered_data if triggered_data is not None else adc_data,
                          freq=frequency, vpp=vpp)
            plt.pause(0.01)  # 短暂暂停让图形更新

    except KeyboardInterrupt:
        print("\n\n程序已退出。")
    finally:
        sampler.disconnect()


if __name__ == '__main__':
    main()
