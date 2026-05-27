# Day 11: 外壳设计与整机装配 | Enclosure Design & Final Assembly

> **今日目标 Today's Objectives:**
> - 了解 TinyAWG 两件式 3D 打印树脂外壳的结构设计
> - 完成电池安装和电源开关接线
> - 将 PCB 装入外壳，安装 LCD 屏幕、MCX 输出接口
> - 完成幅度校准、偏置校准和频率验证
>
> **产出 Deliverable:** TinyAWG 整机组装完成，校准后输出波形参数准确

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 | Day 10 回顾 + 今日装配目标 |
| 09:30-10:15 | 知识讲解 | 外壳设计回顾 + 装配流程讲解 |
| 10:15-10:30 | 休息 | Break |
| 10:30-12:00 | 动手实验 | 电池安装 + PCB 装入外壳 + 屏幕安装 |
| 12:00-13:30 | 午餐 | Lunch + Break |
| 13:30-14:30 | 动手实验 | 完成整机组装 + 接线整理 |
| 14:30-15:00 | 休息+过渡 | Break + 准备校准设备 |
| 15:00-16:15 | 动手实验 | 幅度校准 + 偏置校准 + 频率验证 |
| 16:15-17:00 | 回顾 | 今日总结 + 明日展示准备 |

---

## 上午: 外壳结构与硬件装配 | Morning: Enclosure Structure & Hardware Assembly

### 为什么要学这个? | Why Learn This?

一个好的外壳不仅保护电路，还决定了产品的使用体验。TinyAWG 的外壳采用树脂 3D 打印 (嘉立创3D打印)，两件式设计兼顾了美观和可制造性。今天把所有零件装进外壳，你的 TinyAWG 就从一块裸露的 PCB 变成一台手持仪器了！

A good enclosure not only protects the circuit but also defines the user experience. TinyAWG's case uses resin 3D printing (JLCPCB 3D printing), with a two-piece design balancing aesthetics and manufacturability. Today you turn a bare PCB into a handheld instrument!

### 任务 11.1: 外壳设计回顾 (20 分钟)

**TinyAWG 两件式外壳结构：**

```
+----------------------------------+     +----------------------------------+
|         外壳顶盖 (Top Cover)       |     |         外壳底座 (Base)            |
|                                  |     |                                  |
|   +------------------------+     |     |   +---电池仓---+                  |
|   |  2.8" IPS LCD 窗口      |     |     |   |  锂电池     |    PCB 安装区    |
|   |  (触摸屏可见)            |     |     |   |  3.7V      |                  |
|   +------------------------+     |     |   +-----------+                  |
|                                  |     |                                  |
|   [MCX 输出孔]    [Type-C 口]    |     |        [电源开关]                  |
+----------------------------------+     +----------------------------------+
```

**外壳设计要点：**

| 特征 Feature | 说明 Description |
|-------------|-----------------|
| 两件式设计 | 顶盖 + 底座，通过卡扣或螺丝连接 |
| 电池仓 | 独立隔间，3.7V 锂电池，带固定胶垫 |
| LCD 窗口 | 精确开窗，露出 2.8" IPS 触摸屏 |
| MCX 接口开口 | 侧面开孔，MCX 连接器伸出 |
| Type-C 开口 | 侧面开孔，USB Type-C 充电/通信 |
| 通风孔 | 功放区域散热孔 (OPA2673 可能发热) |
| 电源开关 | 侧面安装拨动开关 |

**3D 打印参数参考 (嘉立创3D打印)：**

| 参数 Parameter | 值 Value |
|--------------|---------|
| 材料 | 树脂 (Resin) |
| 精度 | 0.05mm 层高 |
| 表面处理 | 打磨 + 喷涂 (可选) |
| 颜色 | 黑色/白色/灰色 |

> **关于 ZYNQ7020 兼容性:**
> TinyAWG 兼容 ZYNQ7020 芯片。如果你手上的板子是 7020 版本，只需要在 Vivado 中更改芯片选型 (Part Selection) 即可，PCB 和外壳完全通用。
> TinyAWG is compatible with ZYNQ7020. If your board uses the 7020, just change the Vivado part selection. The PCB and enclosure are fully compatible.

### 任务 11.2: 电池安装与电源开关接线 (30 分钟)

> **WARNING / 锂电池安全警告:**
> - 锂电池短路会导致起火爆炸！安装前用绝缘胶带包裹电池端子
> - Lithium battery short circuit can cause fire/explosion! Tape terminals before installation.
> - 不要用尖锐物品刺穿电池
> - Do not puncture the battery with sharp objects.
> - 电池膨胀、变形或有异味时立即停止使用
> - Stop using immediately if battery is swollen, deformed, or smells unusual.
> - 电池极性必须正确！红线接正极 (+)，黑线接负极 (-)
> - Battery polarity MUST be correct! Red wire to positive (+), black wire to negative (-).

**安装步骤：**

1. **检查电池电压：** 万用表测量电池电压，应在 3.5~4.2V 范围
   - < 3.5V：电池过放，需要先充电
   - > 4.2V：过充，不正常，停止使用

2. **安装电池到电池仓：**
   - 将电池放入底座电池仓
   - 用双面胶或海绵垫固定电池，防止晃动
   - 确认电池不会压到 PCB 上任何元件

3. **焊接电源开关：**
   - 电源开关安装在底座侧面开口处
   - 开关串联在电池正极和 PCB 电源输入之间
   - 焊接后用热缩管绝缘

4. **连接电池到 PCB：**
   - 红线 (+) 接 PCB 的 BAT+ 焊盘
   - 黑线 (-) 接 PCB 的 BAT- 焊盘
   - **再次确认极性无误！**

5. **初步上电测试：**
   - 按下电源开关
   - 确认 PCB 上电源指示灯亮起
   - 用万用表快速测量 3.3V 轨电压

**预期结果 Expected Result:**
- 电池固定牢固，不晃动
- 电源开关操作顺畅
- 开机后指示灯亮，3.3V 轨正常

**常见问题 Common Issues:**
- "开机无反应" -- 检查电源开关接线是否正确，电池是否有电
- "电池装不进电池仓" -- 检查电池尺寸，可能需要更换更薄的电池或调整海绵垫厚度
- "开关拨动很紧" -- 开孔偏小，用砂纸稍微打磨

### 任务 11.3: PCB 装入外壳 + 屏幕安装 (60 分钟)

**装配步骤：**

1. **PCB 定位：**
   - 将 PCB 放入底座的 PCB 安装区
   - 对齐 PCB 安装孔与外壳定位柱
   - 用 M2 螺丝或卡扣固定 PCB
   - 确保 MCX 连接器对准侧面开口

2. **LCD 屏幕安装：**
   - 将 2.8" IPS LCD 屏幕放入顶盖的屏幕窗口
   - 屏幕排线 (FPC) 从 PCB 的 FPC 连接器引出
   - 用双面胶固定屏幕边框
   - 确认触摸面板 (FT6336) 与窗口对齐

3. **接线整理：**
   - 将所有排线整理到 PCB 下方空间
   - 确保没有线缆被 PCB 或外壳夹住
   - 电池线沿边缘走线，不经过 PCB 元件上方

4. **合盖测试 (不锁紧)：**
   - 将顶盖轻轻放在底座上
   - 检查是否有元件顶住顶盖
   - 检查屏幕是否贴合窗口
   - 如有干涉，标记位置并打磨外壳

**装配检查清单：**

- [ ] PCB 安装牢固，MCX 接口对准开口
- [ ] LCD 屏幕显示完整，无遮挡
- [ ] 触摸面板灵敏，操作正常
- [ ] Type-C 口可顺利插入充电器
- [ ] 电源开关操作顺畅
- [ ] 内部无异物，接线整齐
- [ ] 顶盖和底座能完全闭合

**预期结果 Expected Result:**
- 所有部件安装到位，外壳能闭合
- 屏幕显示清晰，触摸响应正常
- MCX 输出接口可正常连接 BNC 线缆

**常见问题 Common Issues:**
- "MCX 接口与开口不对齐" -- 松开 PCB 螺丝微调位置后重新固定
- "屏幕歪了" -- 重新粘贴双面胶，用直尺辅助对齐
- "顶盖盖不上" -- 有元件过高顶住，检查并确认元件高度，必要时打磨外壳内壁
- "触摸不灵敏" -- FPC 排线连接不牢，重新插拔 FPC 连接器

---

## 下午: 完成组装与校准 | Afternoon: Assembly Completion & Calibration

### 任务 11.4: 完成整机组装 (30 分钟)

**最终装配步骤：**

1. **锁紧外壳：**
   - 确认所有内部部件安装正确
   - 用螺丝锁紧顶盖和底座 (如有螺丝孔)
   - 或压紧卡扣 (如为卡扣设计)

2. **外观检查：**
   - 外壳无明显缝隙
   - 所有开口对齐
   - 屏幕无划痕 (撕掉屏幕保护膜)

3. **功能复检：**
   - 开机，确认 LVGL GUI 正常启动
   - 触摸操作正常
   - 串口通信正常 (连接 PC)
   - 波形输出正常 (连接示波器)

4. **贴标签：**
   - 在外壳底部贴上设备信息标签
   - 标注：型号 TinyAWG、输出范围、电源要求

### 任务 11.5: 幅度校准 (30 分钟)

**为什么要校准?**

DAC 和运放的实际输出与理论值存在偏差。通过校准，我们记录一组"修正系数"，让 GUI 上显示的幅度值与实际输出一致。

The actual output of DACs and op-amps differs from theoretical values. By calibrating, we record a set of "correction factors" so the amplitude displayed on the GUI matches the actual output.

**校准设备：**
- 示波器 (用于测量实际输出幅度)
- 万用表 (用于测量直流偏置)
- TinyAWG 本身 (LVGL GUI 设置参数)

**幅度校准步骤：**

1. 设置 TinyAWG 输出 1kHz 正弦波
2. 设置 DAC8562 幅度到最小值，用示波器测量 Vpp
3. 逐步增大 DAC8562 幅度值，记录每个设置点对应的实际 Vpp：

| DAC8562 设置值 | 示波器实测 Vpp | 偏差 Error |
|--------------|---------------|-----------|
| 0 (最小) | _____ mV | _____ |
| 8192 (25%) | _____ mV | _____ |
| 16384 (50%) | _____ mV | _____ |
| 24576 (75%) | _____ mV | _____ |
| 32767 (100%) | _____ mV | _____ |
| 65535 (最大) | _____ mV | _____ |

4. 绘制 DAC 设置值 vs 实际 Vpp 曲线
5. 计算线性校准系数：`校准后 Vpp = 实测 Vpp * (理论值 / 实测值)`
6. 将校准系数写入 PS 端配置文件

**预期结果 Expected Result:**
- 幅度线性度偏差 < 5%
- 最大输出 Vpp 接近 +/-6.5V 电源轨限制
- 校准后 GUI 显示值与实际输出误差 < 2%

**常见问题 Common Issues:**
- "最大输出达不到 6.5Vpp" -- 正常，受运放输出摆幅限制，接近电源轨时会有压降
- "小幅度时线性度差" -- DAC 低段分辨率不足，可分段校准
- "波形在大幅度时削顶" -- 超出 +/-6.5V 电源轨范围，降低幅度设置

### 任务 11.6: 偏置校准 (15 分钟)

1. 设置 TinyAWG 输出 1kHz 正弦波，幅度 2Vpp
2. 调节 DAC8562 偏置通道，使波形中心 (直流分量) 从 -3V 变化到 +3V
3. 用示波器测量各偏置设置下的实际直流偏移：

| DAC8562 偏置设置值 | 示波器实测直流偏移 | 偏差 |
|------------------|------------------|------|
| 0 (最小) | _____ mV | _____ |
| 32767 (中间) | _____ mV | _____ |
| 65535 (最大) | _____ mV | _____ |

4. 确认偏置范围为 +/-3V 左右 (受 +/-6.5V 电源轨限制)

### 任务 11.7: 频率验证 (15 分钟)

1. 用 TinyAWG 输出以下频率，示波器测量实际频率：

| 设置频率 | 示波器实测频率 | 误差 |
|---------|--------------|------|
| 100 Hz | _____ Hz | _____% |
| 1 kHz | _____ Hz | _____% |
| 10 kHz | _____ Hz | _____% |
| 100 kHz | _____ Hz | _____% |
| 1 MHz | _____ Hz | _____% |
| 5 MHz | _____ Hz | _____% |

2. 验证频率精度应 < 0.01% (DDS 基于晶体振荡器，精度很高)

**预期结果 Expected Result:**
- 全频率范围误差 < 0.01% (DDS 特性，由参考时钟精度决定)
- 低频 (100Hz) 和高频 (5MHz) 输出均正常

> **电源使用提示:**
> - TinyAWG 由内置电池供电，无需外部电源即可工作
> - 充电时支持 5~12V 输入。12V 支持 Type-C CH221K PD 触发
> - 接入外部电源后电池 WILL 自动充电
> - 如果只有 5V 输入，建议只充电不开机 (5V 可能不足以同时供电和充电)
> - 输出电压不会超过 +/-6.5V 电源轨范围

---

## 今日作业 | Homework

1. **校准报告：** 完成上面的幅度、偏置、频率校准表格，提交校准数据
2. **装配照片：** 拍摄组装过程和成品照片 (正面、背面、侧面、屏幕显示)，用于明天展示
3. **问题记录：** 记录装配过程中遇到的所有问题和解决方法
4. **准备展示：** 开始准备明天 5 分钟 PPT 的内容大纲

---

## 明日预告 | Tomorrow's Preview

最后一天！明天我们将进行全面的性能测试（频率精度、幅度平坦度、波形质量 THD、扫频功能、AM 调制），然后是最终项目展示。你将在全班面前演示 TinyAWG 的波形生成能力，分享 12 天的学习心得！

The final day! Tomorrow we conduct comprehensive performance testing (frequency accuracy, amplitude flatness, waveform quality THD, sweep function, AM modulation), followed by the final project presentation. You will demonstrate TinyAWG's waveform generation capabilities to the class and share your 12-day learning journey!

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| TinyAWG 开源项目 (OSHWhub) | 开源硬件 | https://oshwhub.com/greentor/tinyawg-signal-source |
| TinyAWG 搭建文档 (飞书) | 技术文档 | https://ai.feishu.cn/docx/BGycd4URIot8Aoxs3vLcBdq2nFc |
| 嘉立创 3D 打印服务 | 在线服务 | https://www.jlcpcb.com/3d-printing |
| MAX17048 电池监控 | PDF | 搜索"MAX17048 datasheet" |
| LVGL 8.3 GUI 文档 | 在线文档 | https://docs.lvgl.io/8.3/ |
| FT6336 触摸 IC | PDF | 搜索"FT6336 touch controller datasheet" |

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
