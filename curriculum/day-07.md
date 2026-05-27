# Day 07: 时基与采样率控制 | Timebase & Sampling Rate Control

> **今日目标 Today's Objectives:**
> - 理解时基 (Timebase) 的概念和档位设计
> - 实现定时器参数动态修改以切换采样率
> - 实现采样深度与时基的联动控制
> - 实现按键/旋钮切换时基档位
>
> **产出 Deliverable:** 示波器支持多个时基档位 (1us/div ~ 10ms/div)，按键切换正常

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 | 回顾 Day 6 + 今日目标 |
| 09:30-10:30 | 知识讲解 | 时基概念、采样率与显示的关系 |
| 10:30-10:45 | 休息 | Break |
| 10:45-12:00 | 动手实验 | 时基档位设计与定时器动态配置 |
| 12:00-13:30 | 午餐 | Lunch + Break |
| 13:30-15:00 | 动手实验 | 采样深度控制与缓冲区管理 |
| 15:00-15:15 | 休息 | Break |
| 15:15-16:30 | 动手实验 | 按键切换时基 + UI 响应 |
| 16:30-17:00 | 回顾 | 今日总结 + 答疑 |

---

## 上午: 时基原理 | Morning: Timebase Principles

### 为什么要学这个? | Why Learn This?

使用真实示波器时，你会转动一个 "TIME/DIV" (时基) 旋钮来调整波形的水平缩放。左转看到更宽的时间范围（适合看低频信号），右转看到更短的时间范围（适合看高频信号细节）。

When using a real oscilloscope, you turn a "TIME/DIV" (timebase) knob to adjust the horizontal zoom of the waveform. Turn left to see a wider time range (good for low-frequency signals), turn right for a shorter range (good for high-frequency details).

**时基的本质：** 它控制了屏幕上每个像素点代表多少时间。时基 = 屏幕上每格(div)代表的时间。

**时基与采样率的关系：**

```
时基 (s/div) = 屏幕每格像素数 × 采样周期
             = pixels_per_div × (1 / sample_rate)

例如：
  时基 10us/div, 20像素/div → 采样率 = 20 / 10us = 2Msps
  时基 1ms/div, 20像素/div  → 采样率 = 20 / 1ms = 20ksps
```

反过来，知道采样率和屏幕尺寸，可以算出满屏时间：

```
满屏时间 = 屏幕宽度 / 采样率
         = 160点 / 1Msps = 160us
         = 160点 / 100ksps = 1.6ms
```

### 任务 7.1: 时基档位设计 (30 分钟)

**设计 8 个时基档位：**

| 档位 | 时基 (us/div) | 采样率 | 满屏时间 | 适用信号频率 |
|------|--------------|--------|---------|------------|
| 0 | 10 us | 2 Msps | 160 us | > 100 kHz |
| 1 | 20 us | 1 Msps | 320 us | 50~100 kHz |
| 2 | 50 us | 400 ksps | 800 us | 20~50 kHz |
| 3 | 100 us | 200 ksps | 1.6 ms | 10~20 kHz |
| 4 | 200 us | 100 ksps | 3.2 ms | 5~10 kHz |
| 5 | 500 us | 40 ksps | 8 ms | 2~5 kHz |
| 6 | 1 ms | 20 ksps | 16 ms | 1~2 kHz |
| 7 | 10 ms | 2 ksps | 160 ms | < 1 kHz |

> 注：2 Msps 超过 STM32 内置 ADC 单通道极限 (~1Msps)，此档位可用双 ADC 交替或保持在 1Msps。
> Note: 2Msps exceeds single-channel ADC limit; use dual ADC interleaved mode or cap at 1Msps.

**定时器配置计算：**

```c
// timebase.h
typedef struct {
    uint16_t prescaler;      // 定时器预分频
    uint16_t period;         // 定时器周期
    uint16_t sample_rate_ksps; // 采样率 (ksps)
    const char *label;       // 显示标签
} TimebaseConfig;

// 8 个时基档位
TimebaseConfig timebase_table[8] = {
    {0,   71,   1000, "10us/div"},   // 72MHz / 72 = 1MHz
    {0,   71,   1000, "20us/div"},   // 复用 1MHz, 显示缩放
    {0,   179,  400,  "50us/div"},   // 72MHz / 180 = 400kHz
    {0,   359,  200,  "100us/div"},  // 72MHz / 360 = 200kHz
    {0,   719,  100,  "200us/div"},  // 72MHz / 720 = 100kHz
    {1,   899,  40,   "500us/div"},  // 36MHz / 900 = 40kHz
    {3,   599,  20,   "1ms/div"},    // 18MHz / 600 = 30kHz (近似)
    {35,  999,  2,    "10ms/div"},   // 2MHz / 1000 = 2kHz
};

volatile uint8_t current_timebase = 1;  // 默认档位
```

**步骤 Steps:**
1. 创建 `timebase.h` 和 `timebase.c`
2. 设计时基档位表
3. 计算每个档位对应的定时器预分频和周期值
4. 验证计算结果

### 任务 7.2: 动态切换定时器参数 (45 分钟)

**实现采样率切换：**

```c
/**
 * 切换时基档位
 * Switch timebase level
 *
 * @param level  时基档位索引 (0~7)
 */
void set_timebase(uint8_t level)
{
    if (level > 7) return;

    current_timebase = level;

    // 停止 DMA 和定时器
    HAL_ADC_Stop_DMA(&hadc1);
    HAL_TIM_Base_Stop(&htim3);

    // 修改定时器参数
    htim3.Instance->PSC = timebase_table[level].prescaler;
    htim3.Instance->ARR = timebase_table[level].period;
    htim3.Instance->EGR = TIM_EGR_UG;    // 生成更新事件，立即生效

    // 重新启动
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, SAMPLE_COUNT);
    HAL_TIM_Base_Start(&htim3);
}

/**
 * 时基递增
 * Increment timebase (slower)
 */
void timebase_increase(void)
{
    if (current_timebase < 7)
    {
        set_timebase(current_timebase + 1);
    }
}

/**
 * 时基递减
 * Decrement timebase (faster)
 */
void timebase_decrease(void)
{
    if (current_timebase > 0)
    {
        set_timebase(current_timebase - 1);
    }
}
```

**步骤 Steps:**
1. 实现 `set_timebase()` 函数
2. 通过串口命令测试：发送 "+" 增加时基，发送 "-" 减少时基
3. 观察不同档位下波形的变化

**预期结果 Expected Result:**
- 切换时基后，波形水平缩放发生变化
- 低时基 (10us/div)：能看到信号的细节
- 高时基 (10ms/div)：能看到更长时间范围的信号

### 任务 7.3: 按键切换时基 (45 分钟)

**硬件接线：**
- 按键 1 (时基减小): PA0 → GND (上拉输入)
- 按键 2 (时基增大): PB0 → GND (上拉输入)

```c
// 按键扫描 (带消抖)
uint8_t scan_button(GPIO_TypeDef *port, uint16_t pin)
{
    if (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET)  // 按下
    {
        HAL_Delay(20);    // 消抖延时
        if (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET)
        {
            while (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET)
                ;    // 等待松开
            return 1;    // 确认按键按下
        }
    }
    return 0;
}

// main 循环中
while (1)
{
    // 检测按键
    if (scan_button(GPIOA, GPIO_PIN_0))
    {
        timebase_decrease();    // 快速→精细
    }
    if (scan_button(GPIOB, GPIO_PIN_0))
    {
        timebase_increase();    // 慢速→全局
    }

    // 波形显示逻辑 (同 Day 6)
    if (buffer_full)
    {
        buffer_full = 0;
        clear_waveform_area();
        // ... 触发和绘制 ...
        draw_timebase_label(timebase_table[current_timebase].label);
        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, SAMPLE_COUNT);
    }
}
```

**步骤 Steps:**
1. 配置 PA0 和 PB0 为 GPIO_Input (上拉)
2. 面包板上连接两个按键
3. 实现按键扫描和时基切换
4. 测试：按不同按键观察时基切换

**预期结果 Expected Result:**
- 按键 1：时基减小 (看更细)
- 按键 2：时基增大 (看更宽)
- 屏幕上显示当前时基标签

---

## 下午: 采样深度控制 | Afternoon: Sample Depth Control

### 任务 7.4: 采样深度与显示的联动 (30 分钟)

**采样深度 (Sample Depth) 的概念：**

采样深度决定了每次采集多少个点。对于 160 像素宽的屏幕：
- 如果采集 160 个点：每个点对应一个像素 (1:1 映射)
- 如果采集 320 个点：需要抽取 160 个点显示 (2:1 抽取)
- 如果采集 80 个点：需要插值到 160 个点 (1:2 插值)

```c
/**
 * 波形数据抽取 (降采样)
 * Decimate waveform data for display
 *
 * @param src      源数据
 * @param src_len  源数据长度
 * @param dst      目标数据
 * @param dst_len  目标数据长度 (通常为屏幕宽度)
 */
void decimate_waveform(uint16_t *src, uint16_t src_len,
                       uint16_t *dst, uint16_t dst_len)
{
    for (uint16_t i = 0; i < dst_len; i++)
    {
        uint16_t src_idx = (uint32_t)i * src_len / dst_len;
        if (src_idx >= src_len) src_idx = src_len - 1;
        dst[i] = src[src_idx];
    }
}
```

**步骤 Steps:**
1. 实现 `decimate_waveform()` 抽取函数
2. 在显示前对采样数据进行抽取
3. 测试不同采样深度下的显示效果

### 任务 7.5: 电压量程切换 (30 分钟) [拓展]

**电压量程 (VOLTS/DIV) 的概念：**

时基控制水平缩放，电压量程控制垂直缩放。通过调整 Y 轴映射比例，可以放大或缩小信号的垂直方向。

```c
// 电压量程档位
typedef struct {
    uint16_t mv_per_div;   // 每格毫伏数
    float y_scale;         // Y轴缩放因子
    const char *label;
} VoltageRange;

VoltageRange voltage_table[] = {
    {100,  128.0 / 4095.0 * 5.0, "100mV"},   // 放大 5 倍
    {200,  128.0 / 4095.0 * 2.5, "200mV"},   // 放大 2.5 倍
    {500,  128.0 / 4095.0 * 1.0, "500mV"},   // 基准
    {1000, 128.0 / 4095.0 * 0.5, "1V/div"},  // 缩小 0.5 倍
    {2000, 128.0 / 4095.0 * 0.25, "2V/div"}, // 缩小 0.25 倍
};
```

---

## 今日作业 | Homework

1. **计算题：** 如果时基为 500us/div，屏幕每格 20 像素，满屏 160 像素，满屏时间是多少？采样率应设为多少？
2. **代码题：** 添加"自动时基"功能：根据信号频率自动选择合适的时基档位
3. **实验题：** 用同一信号测试所有 8 个时基档位，记录波形变化
4. **思考题：** 为什么高采样率时采样深度要大，低采样率时采样深度可以小？

---

## 明日预告 | Tomorrow's Preview

明天是 Phase 2 的最后一天！我们将实现频率测量和自动量程——示波器不再只是"看到"波形，还能"读懂"波形的参数：频率、周期、幅度、峰峰值。

The final day of Phase 2! We implement frequency measurement and auto-ranging -- the oscilloscope can now not only "see" waveforms but also "read" waveform parameters: frequency, period, amplitude, and peak-to-peak voltage.

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| ADC+DMA+TIM 实现简易示波器 (STM32CubeMX) | B站视频 | https://www.bilibili.com/video/BV1GC4y1T7BN/ |
| DSO138 时基控制逻辑 | GitHub | https://github.com/essboyer/DSO138 |
| STM32 定时器配置 | B站搜索 | 搜索"STM32 定时器 动态配置" |
| 示波器时基原理 | B站搜索 | 搜索"示波器 时基 TIME/DIV" |

---

*最后更新：2026-05-26*
*Last updated: 2026-05-26*
