"""
工具函数模块
Utility Functions Module

功能：配置加载、日志设置、数据格式转换等
Features: Configuration loading, logging setup, data format conversion
"""

import os
import logging
from datetime import datetime


def load_config(config_path=None):
    """
    加载配置文件
    Load configuration file

    按以下优先级查找配置文件：
    1. 指定的路径
    2. 当前目录的 config.yaml
    3. 当前目录的 config.template.yaml
    4. 使用默认值

    Args:
        config_path: 配置文件路径 (None=自动查找)

    Returns:
        dict: 配置字典
    """
    # 默认配置
    default_config = {
        'serial': {
            'port': 'COM3',
            'baudrate': 115200,
            'timeout': 2,
        },
        'adc': {
            'resolution': 12,
            'vref': 3.3,
            'sample_count': 1024,
            'sample_rate': 1000000,
        },
        'display': {
            'width': 160,
            'height': 128,
            'grid_div_x': 8,
            'grid_div_y': 6,
        },
        'trigger': {
            'mode': 'auto',
            'slope': 'rising',
            'level': 0,
            'holdoff_ms': 100,
        },
        'logging': {
            'level': 'INFO',
            'save_waveforms': False,
            'waveform_dir': 'waveforms',
        },
    }

    # 尝试加载 YAML 配置文件
    try:
        import yaml
    except ImportError:
        print("警告: PyYAML 未安装，使用默认配置")
        return default_config

    # 查找配置文件
    if config_path is None:
        candidates = [
            'config.yaml',
            'config.template.yaml',
            os.path.join(os.path.dirname(__file__), '..', 'config.yaml'),
            os.path.join(os.path.dirname(__file__), '..', 'config.template.yaml'),
        ]
        for candidate in candidates:
            if os.path.exists(candidate):
                config_path = candidate
                break

    if config_path and os.path.exists(config_path):
        try:
            with open(config_path, 'r', encoding='utf-8') as f:
                loaded = yaml.safe_load(f)
                if loaded:
                    # 合并：加载的配置覆盖默认值
                    _merge_config(default_config, loaded)
                    print(f"已加载配置文件: {config_path}")
        except Exception as e:
            print(f"配置文件加载失败: {e}，使用默认配置")

    return default_config


def _merge_config(base, override):
    """
    递归合并配置字典
    Recursively merge configuration dictionaries

    Args:
        base: 基础配置 (会被修改)
        override: 覆盖配置
    """
    for key, value in override.items():
        if key in base and isinstance(base[key], dict) and isinstance(value, dict):
            _merge_config(base[key], value)
        else:
            base[key] = value


def setup_logging(log_config=None):
    """
    设置日志系统
    Setup logging system

    Args:
        log_config: 日志配置字典

    Returns:
        logging.Logger: 日志器实例
    """
    if log_config is None:
        log_config = {}

    level_str = log_config.get('level', 'INFO').upper()
    level_map = {
        'DEBUG': logging.DEBUG,
        'INFO': logging.INFO,
        'WARNING': logging.WARNING,
        'ERROR': logging.ERROR,
    }
    level = level_map.get(level_str, logging.INFO)

    # 配置日志格式
    logging.basicConfig(
        level=level,
        format='%(asctime)s [%(levelname)s] %(message)s',
        datefmt='%H:%M:%S',
    )

    return logging.getLogger('pocket_oscilloscope')


def save_waveform_csv(data, filename=None):
    """
    保存波形数据为 CSV 文件
    Save waveform data to CSV file

    Args:
        data: ADC 数据数组
        filename: 文件名 (None=自动生成带时间戳的文件名)

    Returns:
        str: 保存的文件路径
    """
    if filename is None:
        timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
        filename = f'waveform_{timestamp}.csv'

    # 确保目录存在
    os.makedirs(os.path.dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)

    with open(filename, 'w', encoding='utf-8') as f:
        f.write('sample_index,adc_value,voltage\n')
        for i, value in enumerate(data):
            voltage = value * 3.3 / 4095
            f.write(f'{i},{value},{voltage:.6f}\n')

    return filename


def format_frequency(freq_hz):
    """
    格式化频率显示
    Format frequency for display

    Args:
        freq_hz: 频率 (Hz)

    Returns:
        str: 格式化后的频率字符串
    """
    if freq_hz >= 1e6:
        return f'{freq_hz/1e6:.2f} MHz'
    elif freq_hz >= 1e3:
        return f'{freq_hz/1e3:.2f} kHz'
    elif freq_hz >= 1:
        return f'{freq_hz:.1f} Hz'
    elif freq_hz > 0:
        return f'{freq_hz*1000:.1f} mHz'
    else:
        return '--- Hz'


def format_voltage(voltage):
    """
    格式化电压显示
    Format voltage for display

    Args:
        voltage: 电压 (V)

    Returns:
        str: 格式化后的电压字符串
    """
    if abs(voltage) >= 1:
        return f'{voltage:.3f} V'
    elif abs(voltage) >= 0.001:
        return f'{voltage*1000:.1f} mV'
    else:
        return f'{voltage*1e6:.1f} uV'
