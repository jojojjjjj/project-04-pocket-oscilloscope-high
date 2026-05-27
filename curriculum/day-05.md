# Day 05: 波形触发逻辑设计 | Waveform Trigger Logic Design

> **今日目标 Today's Objectives:**
> - 理解示波器触发的工作原理和三种模式
> - 实现上升沿和下降沿检测算法
> - 实现触发电平设置和触发点定位
> - 完成软件触发函数，使波形在串口输出时稳定显示
>
> **产出 Deliverable:** 软件触发函数工作正常，Python 显示的波形稳定不再"跑动"

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 | Phase 1 回顾 + Phase 2 目标 |
| 09:30-10:30 | 知识讲解 | 触发原理：边沿、电平、模式 |
| 10:30-10:45 | 休息 | Break |
| 10:45-12:00 | 动手实验 | 实现上升沿/下降沿检测 |
| 12:00-13:30 | 午餐 | Lunch + Break |
| 13:30-15:00 | 动手实验 | 实现触发电平和触发点定位 |
| 15:00-15:15 | 休息 | Break |
| 15:15-16:30 | 动手实验 | 三种触发模式实现 |
| 16:30-17:00 | 回顾 | 今日总结 + 答疑 |

---

## 上午: 触发原理 | Morning: Trigger Principles

### 为什么要学这个? | Why Learn This?

你有没有想过：为什么示波器上的波形看起来总是"静止"的？明明信号每时每刻都在变化，为什么屏幕上不乱跑？

Have you ever wondered: why does the waveform on an oscilloscope always look "frozen"? The signal is changing every moment, why doesn't it run around on screen?

答案就是**触发 (Trigger)**。触发就像"拍照时机"——示波器不是连续显示信号，而是等待一个特定条件出现（比如信号从下往上穿过某个电压），然后在那个时刻"拍一张照片"并显示。每次都在同样的条件下拍照，波形看起来就"稳住"了。

The answer is **triggering**. Triggering is like "when to take a photo" -- the oscilloscope doesn't display the signal continuously. Instead, it waits for a specific condition (like the signal crossing a voltage level from below), then "takes a photo" at that moment. Since it always takes photos under the same condition, the waveform looks "stable."

**没有触发 vs 有触发：**

```
无触发 (波形跑动):
  第1次采集: ───/──\───
  第2次采集: ─/──\───/──
  第3次采集: ──/──\───/─
  (起始点不同，叠加后看起来在移动)

有触发 (波形稳定):
  第1次采集: ──/──\───/──
  第2次采集: ──/──\───/──
  第3次采集: ──/──\───/──
  (都在上升过零点开始，叠加后完全重合)
```

### 任务 5.1: 触发概念讲解 (30 分钟)

**触发的三要素：**

| 要素 Element | 说明 Description | 示例 Example |
|-------------|-----------------|-------------|
| 触发源 Source | 用哪个信号来判断触发 | 本通道信号 (CH1) |
| 触发边沿 Slope | 信号在什么方向时触发 | 上升沿 / 下降沿 |
| 触发水平 Level | 信号到达什么电压时触发 | 1.65V (信号中心) |

**三种触发模式：**

| 模式 Mode | 行为 Behavior | 适用场景 |
|----------|-------------|---------|
| 自动 Auto | 不管有没有触发条件都显示波形 | 通用观察 |
| 正常 Normal | 只在满足触发条件时才更新波形 | 低频信号、脉冲信号 |
| 单次 Single | 第一次触发后冻结波形 | 单次事件捕获 |

**触发的工作流程：**

```
1. DMA 持续采集数据到缓冲区 A
2. 同时在缓冲区 B 中搜索触发点
3. 找到触发点后：
   - 触发点之前的数据 = 触发前 (Pre-trigger)
   - 触发点之后的数据 = 触发后 (Post-trigger)
4. 将触发点对齐到屏幕中间，显示波形
5. 回到步骤 2，等待下一次触发
```

**步骤 Steps:**
1. 观看 B 站视频：Expert电子实验室《如何用示波器的触发捕捉波形》
   - 链接：https://www.bilibili.com/video/BV1HRVRz3EyK/
2. 在纸上画出"无触发"和"有触发"时波形的区别
3. 讨论：为什么触发电平通常设在信号的中心值？

### 任务 5.2: 实现上升沿检测 (45 分钟)

**上升沿检测算法：**

上升沿就是信号从低到高的"转折点"。检测方法很简单：
- 当前点 < 触发电平 AND 下一个点 >= 触发电平 → 检测到上升沿！

```c
// trigger.c - 触发逻辑模块
#include "trigger.h"

/**
 * 检测上升沿触发点
 * Rising edge trigger detection
 *
 * @param buffer  ADC 数据缓冲区
 * @param size    缓冲区大小
 * @param level   触发电平 (ADC 值，0~4095)
 * @return        触发点索引，-1 表示未找到
 */
int find_rising_edge(uint16_t *buffer, int size, uint16_t level)
{
    for (int i = 0; i < size - 1; i++)
    {
        // 前一个点低于电平，后一个点高于或等于电平
        if (buffer[i] < level && buffer[i + 1] >= level)
        {
            return i;    // 返回触发点索引
        }
    }
    return -1;    // 未找到触发点
}

/**
 * 检测下降沿触发点
 * Falling edge trigger detection
 */
int find_falling_edge(uint16_t *buffer, int size, uint16_t level)
{
    for (int i = 0; i < size - 1; i++)
    {
        if (buffer[i] >= level && buffer[i + 1] < level)
        {
            return i;
        }
    }
    return -1;
}
```

**步骤 Steps:**
1. 创建 `trigger.c` 和 `trigger.h` 文件
2. 实现 `find_rising_edge()` 函数
3. 在 main.c 中调用触发函数：

```c
// 在 DMA 完成回调中
if (buffer_full)
{
    buffer_full = 0;

    // 默认触发电平设在信号中心
    uint16_t trigger_level = 2048;  // 1.65V

    // 寻找上升沿触发点
    int trigger_point = find_rising_edge(adc_buffer, SAMPLE_COUNT, trigger_level);

    if (trigger_point >= 0)
    {
        // 从触发点开始输出数据
        printf("TRIG:%d\n", trigger_point);
        for (int i = trigger_point; i < SAMPLE_COUNT; i++)
        {
            printf("%d\n", adc_buffer[i]);
        }
    }
    else
    {
        // 未找到触发点，输出全部数据
        printf("NO_TRIG\n");
        for (int i = 0; i < SAMPLE_COUNT; i++)
        {
            printf("%d\n", adc_buffer[i]);
        }
    }

    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, SAMPLE_COUNT);
}
```

4. 测试：输入 1kHz 正弦波，观察 Python 显示的波形是否稳定

**预期结果 Expected Result:**
- 每次输出的数据都从上升沿过零点开始
- Python 显示的波形不再"跑动"，每次都在相同位置

**常见问题 Common Issues:**
- "触发点总是 -1 (未找到)" -- 触发电平可能不在信号范围内，调整电平值
- "波形还是在跑" -- 可能是信号频率和采样率不匹配，触发点间隔不一致

---

## 下午: 完善触发功能 | Afternoon: Refining Trigger Functionality

### 任务 5.3: 触发电平自动设置 (30 分钟)

**自动触发电平算法：**

对于正弦波，触发电平应设在信号的中间值：

```c
/**
 * 计算信号的中心电压（用于自动触发电平）
 * Calculate signal center voltage for auto trigger level
 */
uint16_t calculate_center_level(uint16_t *buffer, int size)
{
    uint16_t min_val = 4095;
    uint16_t max_val = 0;

    for (int i = 0; i < size; i++)
    {
        if (buffer[i] < min_val) min_val = buffer[i];
        if (buffer[i] > max_val) max_val = buffer[i];
    }

    return (min_val + max_val) / 2;
}
```

**步骤 Steps:**
1. 在触发前先计算信号中心值
2. 用计算出的中心值作为触发电平
3. 测试不同幅度的信号，验证自动电平是否正确

**预期结果 Expected Result:**
- 输入不同幅度的信号，触发电平自动调整到信号中心
- 不需要手动设置电平值

### 任务 5.4: 三种触发模式实现 (45 分钟)

**触发模式状态机：**

```c
typedef enum {
    TRIG_AUTO,       // 自动模式：超时后强制显示
    TRIG_NORMAL,     // 正常模式：只在触发时显示
    TRIG_SINGLE,     // 单次模式：触发一次后停止
} TriggerMode;

typedef struct {
    TriggerMode mode;        // 触发模式
    uint16_t level;          // 触发电平
    uint8_t slope;           // 0=下降沿, 1=上升沿
    uint8_t armed;           // 单次模式：是否已触发
} TriggerConfig;

TriggerConfig trig = {
    .mode = TRIG_AUTO,
    .level = 2048,
    .slope = 1,        // 上升沿
    .armed = 1
};

/**
 * 带超时的触发等待
 * Trigger wait with timeout (for AUTO mode)
 */
int wait_for_trigger(uint16_t *buffer, int size, TriggerConfig *config)
{
    int trig_func(int (*edge_func)(uint16_t*, int, uint16_t));

    // 选择边沿函数
    int (*edge_detect)(uint16_t*, int, uint16_t) =
        config->slope ? find_rising_edge : find_falling_edge;

    // 自动设置触发电平
    if (config->level == 0)
    {
        config->level = calculate_center_level(buffer, size);
    }

    int trigger_point = edge_detect(buffer, size, config->level);

    if (trigger_point >= 0)
    {
        return trigger_point;    // 找到触发点
    }

    // 未找到触发点
    if (config->mode == TRIG_AUTO)
    {
        return 0;    // 自动模式：从头显示
    }
    else if (config->mode == TRIG_SINGLE)
    {
        return -1;    // 单次模式：不显示
    }

    return -1;    // 正常模式：等待下一次
}
```

**步骤 Steps:**
1. 实现 `TriggerConfig` 结构体和 `wait_for_trigger()` 函数
2. 通过串口命令切换触发模式：
   - 发送 "A" → 自动模式
   - 发送 "N" → 正常模式
   - 发送 "S" → 单次模式
   - 发送 "U" → 上升沿
   - 发送 "D" → 下降沿
3. 测试三种模式在不同信号下的表现

**预期结果 Expected Result:**
- 自动模式：始终有波形显示，即使信号断开
- 正常模式：信号断开时波形冻结在最后一次触发
- 单次模式：第一次触发后波形冻结，发送 "S" 重新等待

### 任务 5.5: 触发功能集成测试 (30 分钟)

**测试场景：**

| 测试项 | 输入信号 | 触发设置 | 预期结果 |
|--------|---------|---------|---------|
| 上升沿触发 | 1kHz 正弦波 | 上升沿, 1.65V | 波形稳定 |
| 下降沿触发 | 1kHz 方波 | 下降沿, 1.65V | 方波下降沿对齐 |
| 自动模式 | 断开输入 | 自动 | 显示随机噪声/上次波形 |
| 正常模式 | 断开输入 | 正常 | 波形冻结 |
| 低幅度信号 | 0.1Vpp 正弦 | 自动电平 | 能自动检测并触发 |

**步骤 Steps:**
1. 按测试矩阵逐项测试
2. 记录每项测试结果和波形截图
3. 记录遇到的 bug 和修复方法

---

## 今日作业 | Homework

1. **代码题：** 完善 trigger.c，添加 hysteresis（回差）功能，防止信号在触发电平附近时触发点抖动
2. **思考题：** 为什么商用示波器有"释抑时间 (Holdoff)"功能？在什么场景下需要它？
3. **实验题：** 用方波信号测试触发，观察上升沿和下降沿触发时波形显示的区别
4. **写实验日志：** 记录触发逻辑的设计过程和测试结果

---

## 明日预告 | Tomorrow's Preview

触发解决了波形"稳住"的问题，但波形还在电脑上显示呢！明天我们把它搬到 TFT 屏幕上——从串口绘图进化到真正的嵌入式示波器界面！

Triggering solved the "stable waveform" problem, but the waveform is still on the PC! Tomorrow we move it to the TFT screen -- evolving from serial port plotting to a real embedded oscilloscope interface!

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| 示波器触发原理详解 | B站视频 | https://www.bilibili.com/video/BV1HRVRz3EyK/ |
| DSO138 触发逻辑源码 | GitHub | https://github.com/essboyer/DSO138 |
| DLO-138 替代固件 | GitHub | https://github.com/ardyesp/DLO-138 |
| 示波器触发模式讲解 | B站搜索 | 搜索"示波器 触发模式 教程" |

---

*最后更新：2026-05-26*
*Last updated: 2026-05-26*
