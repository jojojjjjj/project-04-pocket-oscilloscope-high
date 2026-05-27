# Day 10: PCB 组装与焊接 | PCB Assembly & Soldering

> **今日目标 Today's Objectives:**
> - 掌握 SMD (表面贴装) 元件的焊接技巧：锡膏法、热风枪法、烙铁法
> - 完成 TinyAWG PCB 电源部分组装 (LMR544006 DCDC、充电芯片、升压芯片)
> - 完成信号链路组装 (AD9744 DAC、OPA2673 运放、DAC8562)
> - 完成数字部分组装 (连接器、WS2812 LED) 并进行上电测试
>
> **产出 Deliverable:** TinyAWG PCB 全部元件焊接完成，电源轨验证通过 (+/-6.5V, 3.3V, 1.8V)

---

## 时间安排 | Schedule

| 时间 Time | 活动类型 Type | 内容 Content |
|----------|-------------|-------------|
| 09:00-09:30 | 晨会 | Day 9 回顾 + 今日焊接目标 |
| 09:30-10:15 | 知识讲解 | SMD 焊接技术 + 安全规范 |
| 10:15-10:30 | 休息 | Break |
| 10:30-12:00 | 动手实验 | 电源部分组装 (DCDC + 充电 + 升压) |
| 12:00-13:30 | 午餐 | Lunch + Break |
| 13:30-15:00 | 动手实验 | 信号链路组装 (AD9744 + OPA2673 + DAC8562) |
| 15:00-15:15 | 休息 | Break |
| 15:15-16:15 | 动手实验 | 数字部分 + 上电测试 + 电源轨验证 |
| 16:15-17:00 | 回顾 | 今日总结 + 焊接质量互检 |

---

## 上午: PCB 检验与电源组装 | Morning: PCB Inspection & Power Section Assembly

### 为什么要学这个? | Why Learn This?

焊接是将设计变成实物的关键一步。TinyAWG 的 PCB 上有大量 0402/0603 封装的电阻电容，以及 QFN 封装的芯片 (AD9744、OPA2673)。SMD 焊接看起来很难，但只要掌握了正确的方法，高中生也能做得很好。今天焊好的板子，就是明天装进外壳的成品！

Soldering is the critical step that turns design into reality. TinyAWG's PCB has many 0402/0603 resistors/capacitors and QFN-packaged ICs (AD9744, OPA2673). SMD soldering looks intimidating, but with the right techniques, high school students can do it well. The board you solder today becomes the finished product in its enclosure tomorrow!

### 任务 10.1: PCB 来料检验 (15 分钟)

**检验清单：**

| 检查项 Item | 方法 Method | 通过标准 Pass |
|------------|------------|-------------|
| PCB 外观 | 目视检查 | 无划痕、无断线、过孔通透 |
| 丝印清晰度 | 目视检查 | 元件位号清晰可读 |
| 阻焊层 | 目视检查 | 覆盖完整，焊盘裸露 |
| 板厚板弯 | 手感+对比 | 板厚约 1.6mm，无明显翘曲 |
| BOM 对照 | 逐一清点 | 所有元件数量正确 |

**TinyAWG PCB 关键元件清单 (按焊接顺序)：**

| 区域 | 关键芯片 | 封装 | 焊接难度 |
|------|---------|------|---------|
| 电源 | LMR544006 (DCDC 3.3V) | SOT-23-6 | 中等 |
| 电源 | 充电芯片 | QFN | 较高 |
| 电源 | 升压芯片 (+/-6.5V) | QFN | 较高 |
| 信号 | AD9744 (14-bit DAC) | TSSOP-28 | 中等 |
| 信号 | OPA2673 (双运放) | VSSOP-10 | 中等 |
| 信号 | DAC8562 (16-bit DAC) | VSSOP-10 | 中等 |
| 信号 | 继电器 (输出切换) | MSOP | 中等 |
| 数字 | WS2812 (LED) | 3535 | 简单 |
| 连接器 | Type-C, MCX, FPC | 贴片/插装 | 简单 |

**步骤 Steps:**
1. 拿到 PCB 后对照 BOM 清点所有元件
2. 用万用表检查 PCB 电源和地线是否短路 (上电前必做)
3. 将元件按焊接顺序分区域摆放

**预期结果 Expected Result:**
- 所有元件到齐，包装标签与 BOM 一致
- PCB 电源对地无短路

### 任务 10.2: SMD 焊接技术讲解 (30 分钟)

**焊接安全须知 Safety Rules:**

> **WARNING / 安全警告:**
> - 烙铁头温度 300~380C，严禁用手触摸烙铁头和刚焊好的焊点
> - Hot iron tip at 300~380C. NEVER touch the iron tip or freshly soldered joints.
> - 焊接时保持通风，避免长时间吸入焊锡烟雾
> - Keep ventilation. Avoid prolonged inhalation of solder fumes.
> - 不用时将烙铁放回烙铁架，严禁将烙铁放在桌面
> - Always return iron to stand. NEVER place on desk.
> - 使用铅锡焊丝后要洗手 (铅有害健康)
> - Wash hands after using leaded solder (lead is hazardous).
> - 热风枪温度可达 400C，远离易燃物品和人体
> - Hot air gun can reach 400C. Keep away from flammable materials and people.

**三种 SMD 焊接方法：**

| 方法 Method | 适用场景 | 工具 Tools | 难度 |
|------------|---------|-----------|------|
| 烙铁法 | 0603/0805 元件、SOP 芯片 | 烙铁+焊锡丝+助焊剂+镊子 | 入门 |
| 锡膏法 | 0402 元件、QFN 芯片 | 锡膏+钢网+回流焊/热风枪 | 中等 |
| 热风枪法 | QFN/BGA 芯片拆焊、返修 | 热风枪+助焊剂 | 进阶 |

**烙铁法焊接 0603 元件步骤 (最常用)：**

```
1. 烙铁加热焊盘一侧 (1~2 秒)
2. 送少量焊锡，形成"锡珠"
3. 用镊子夹住元件，对准焊盘
4. 先焊一端固定
5. 再焊另一端
6. 检查焊点：光滑、不连锡
```

**QFN 芯片焊接步骤 (拖焊法)：**

```
1. 给 PCB 焊盘涂助焊剂
2. 芯片对准焊盘 (注意方向标记!)
3. 烙铁先固定对角两个引脚
4. 在引脚侧面上焊锡丝
5. 烙铁沿引脚方向"拖"过去，利用表面张力使焊锡均匀分布
6. 用放大镜检查有无连锡 (短路)
7. 如有连锡，加助焊剂重新拖一遍
```

**步骤 Steps:**
1. 在练习板上用 0603 电阻练习烙铁法
2. 在废板/练习板上练习 QFN 拖焊
3. 老师检查练习焊点质量后开始正式焊接

**预期结果 Expected Result:**
- 0603 元件焊点光滑、无虚焊、不连锡
- QFN 芯片引脚清晰可见、无桥接

**常见问题 Common Issues:**
- "焊点灰暗不亮" -- 温度不够或焊锡质量差，加助焊剂重焊
- "连锡/桥接" -- 焊锡太多，用吸锡线吸掉多余焊锡后重焊
- "元件移位" -- 先焊一端固定，确认位置后再焊另一端
- "烙铁不粘锡" -- 烙铁头氧化，用湿海绵/铜丝球清洁

### 任务 10.3: 电源部分组装 (60 分钟)

> **WARNING / 重要警告:**
> 电源部分是整个 PCB 最关键的区域。电源焊接错误可能导致芯片烧毁或锂电池危险。必须由老师检查后才能上电测试！
> The power section is the most critical area. Errors here can destroy ICs or create lithium battery hazards. Teacher MUST inspect before power-on testing!

**电源架构回顾：**

```
电池 (3.7V 锂电) / Type-C 输入 (9~12V)
    |
    +---> LMR544006 DCDC ---> 3.3V (PS + PL + LCD)
    |
    +---> 升压芯片 ---> +6.5V / -6.5V (运放供电)
    |
    +---> 充电芯片 ---> 锂电池充电管理

注意事项:
- 12V 支持 Type-C CH221K PD 触发输入
- 一旦接外部电源，电池 WILL 充电 -- 必须使用 9~12V
- 如果只有 5V 输入，仅用于充电，不要开机
```

**焊接顺序 (按依赖关系)：**

1. **DCDC 3.3V (LMR544006)：** 先焊电源芯片和周围电感、电容
2. **充电芯片：** 焊接充电管理 IC 及外围元件
3. **升压芯片 (+/-6.5V)：** 焊接双电源升压电路

**每步检查：**
- 焊完一个芯片后，用万用表蜂鸣档检查该区域电源对地是否短路
- 检查电容方向 (钽电容/电解电容有极性)
- 检查电感焊点是否牢固

**预期结果 Expected Result:**
- 电源部分所有元件焊接完成
- 万用表测各电源轨对地电阻 > 1kOhm (无短路)

**常见问题 Common Issues:**
- "电感焊不牢" -- 电感焊盘散热快，需要加大烙铁接触面积或多给焊锡
- "DCDC 输出不对" -- 检查反馈电阻分压是否正确
- "焊接时元件烫手" -- 正常，热传导。用镊子夹持，不要用手

---

## 下午: 信号链路与数字部分 | Afternoon: Signal Path & Digital Section

### 任务 10.4: 信号链路组装 (45 分钟)

**信号链路焊接顺序：**

1. **DAC8562 (幅度/偏置 DAC)：** SPI 接口，VSSOP-10 封装
2. **AD9744 (主 DAC)：** 14-bit 并行数据输入，TSSOP-28 封装
3. **OPA2673 (输出运放)：** 高速双运放，VSSOP-10 封装
4. **继电器 (输出切换)：** 负责输出通断和阻抗切换
5. **周围电阻电容：** 0402/0603 封装的耦合电容、反馈电阻等

**关键注意事项：**

> **AD9744 是 TinyAWG 最贵的芯片之一，焊接前仔细确认方向！**
> AD9744 is one of the most expensive chips on TinyAWG. Double-check orientation before soldering!

- AD9744 第 1 引脚 (Pin 1) 标记：芯片正面有一个小圆点，或芯片边缘有一个缺口
- OPA2673 VSSOP-10 封装很小 (3mm x 3mm)，需要用细尖烙铁头 (0.2mm)
- DAC8562 也是 VSSOP-10，同样需要注意方向
- 所有高速信号路径 (DAC 输出 -> 运放 -> MCX 输出) 的去耦电容必须紧贴芯片焊盘

**步骤 Steps:**
1. 先焊 DAC8562 及其外围 (SPI 通信验证)
2. 再焊 AD9744 及其外围 (并行数据接口)
3. 最后焊 OPA2673 和继电器 (模拟输出级)
4. 每焊完一个芯片，目视检查焊点质量

**预期结果 Expected Result:**
- 信号链路所有元件焊接完成
- 目视检查无连锡、无虚焊
- MCX 输出连接器牢固

**常见问题 Common Issues:**
- "VSSOP-10 引脚太密看不清" -- 用放大镜或手机微距模式检查
- "AD9744 引脚连锡" -- 加助焊剂，用拖焊法重新整理
- "运放输出有偏移" -- 可能是反馈电阻虚焊，重新加热焊点

### 任务 10.5: 数字部分与连接器 (30 分钟)

**数字部分元件：**

1. **WS2812 RGB LED：** 状态指示灯，3535 封装
2. **Type-C 连接器：** USB 供电 + 串口通信
3. **FPC 连接器：** LCD 屏幕排线接口
4. **FT6336 触摸 IC：** I2C 接口 (如未预焊)
5. **MCX 连接器：** 信号输出接口
6. **排针/排母：** JTAG 调试接口
7. **电源开关：** 拨动开关

**步骤 Steps:**
1. 焊接 WS2812 (注意方向，有标记的一面朝上)
2. 焊接 Type-C 连接器 (注意引脚间距很密，用锡膏+热风枪效果好)
3. 焊接 FPC 连接器 (翻盖式，先焊固定脚再焊信号脚)
4. 焊接 MCX 连接器 (外壳需大面积接地，多给焊锡)
5. 焊接排针 (JTAG) 和电源开关

**预期结果 Expected Result:**
- 所有连接器焊好且牢固
- Type-C 插头能顺利插入
- FPC 连接器翻盖能正常开合

**常见问题 Common Issues:**
- "Type-C 引脚连锡" -- 用吸锡线处理，或用锡膏+热风枪重新焊
- "FPC 连接器歪了" -- 先固定对角两个引脚，确认端正后再焊其余脚
- "MCX 外壳接地不良" -- 多给焊锡和助焊剂，确保外壳四个角都焊好

### 任务 10.6: 上电测试与电源轨验证 (45 分钟)

> **WARNING / 上电安全检查:**
> 上电前必须完成以下检查，否则可能烧毁芯片或电池！
> Complete these checks before applying power, or you may destroy ICs or the battery!
>
> 1. 万用表蜂鸣档测各电源轨对地 (GND) 是否短路 -- 必须 无蜂鸣
> 2. 检查电池正负极是否接反 -- 反接会起火爆炸！
> 3. 确认 Type-C 口无金属碎屑
> 4. 老师签字确认后方可上电

**上电测试顺序：**

```
第一步: 不插电池，只用 USB 5V 供电
  -> 测 3.3V 轨 (万用表测 C?? 两端)     预期: 3.25~3.35V
  -> 测 1.8V 轨 (ZYNQ 内核)              预期: 1.75~1.85V

第二步: 断电，插入锂电池 (注意极性!)
  -> 不接外部电源，测电池电压             预期: 3.5~4.2V
  -> 按下电源开关开机
  -> 测 +/-6.5V 轨                       预期: +6.3~6.7V, -6.3~-6.7V

第三步: 连接 Type-C 9~12V 电源
  -> 确认充电指示灯亮起
  -> 测充电电流 (如有电流表)             预期: 0.3~1A

第四步: 连接 JTAG，烧写 FPGA bitstream
  -> 确认 FPGA DONE 指示灯亮
```

**电源轨验证表：**

| 电源轨 Rail | 额定值 Nominal | 允许范围 Range | 测试点 Test Point | 实测值 Measured |
|------------|---------------|---------------|------------------|----------------|
| VCC_3V3 | 3.3V | 3.25~3.35V | C?? (3.3V 电容) | _____ V |
| VCC_1V8 | 1.8V | 1.75~1.85V | C?? (1.8V 电容) | _____ V |
| VCC_+6V5 | +6.5V | +6.3~+6.7V | 运放正电源 | _____ V |
| VCC_-6V5 | -6.5V | -6.3~-6.7V | 运放负电源 | _____ V |
| VBAT | 3.7V | 3.5~4.2V | 电池接口 | _____ V |

**预期结果 Expected Result:**
- 所有电源轨电压在允许范围内
- 无异常发热 (用手轻触芯片表面，温热正常，烫手不正常)
- 充电指示灯工作正常

**常见问题 Common Issues:**
- "3.3V 只有 2.5V" -- DCDC 反馈电阻焊错或负载短路
- "+6.5V 无输出" -- 升压芯片使能脚未连接或电感虚焊
- "上电瞬间电流很大" -- 有短路！立即断电排查
- "电池插反了" -- 立即拔出，检查电池保护板是否完好

> **关于 GUI 屏幕闪烁的说明:**
> LVGL GUI 有时会出现刷新撕裂现象，这可能与电源相关 -- 屏幕需要 5V 但设计只提供了 3.3V。如果遇到此问题，记录频率和条件，后续优化。
> The LVGL GUI may occasionally show refresh tearing, likely power-related (screen needs 5V but design provides 3.3V). Document occurrences for future optimization.

---

## 今日作业 | Homework

1. **焊接记录：** 在实验日志中记录每个芯片的焊接时间、遇到的问题和解决方法
2. **电源测试表：** 完成上面的电源轨验证表，填写实测值
3. **练习题：** 画出一个 SOT-23-6 封装的焊盘布局图，标注引脚编号
4. **思考题：** 为什么电源部分要先焊接并单独测试，而不是全部焊完再一起测？

---

## 明日预告 | Tomorrow's Preview

明天我们将进行 3D 打印外壳的组装和整机装配！TinyAWG 的外壳采用两件式树脂 3D 打印设计 (嘉立创3D打印)，包含电池仓、屏幕开口和 MCX 输出接口开口。我们还将进行幅度校准、偏置校准和频率验证。

Tomorrow we assemble the 3D printed enclosure and complete the final build! TinyAWG's case uses a two-piece resin 3D printed design (JLCPCB 3D printing), with battery compartment, screen opening, and MCX output connector access. We will also perform amplitude calibration, offset calibration, and frequency verification.

---

## 参考资源 | References

| 资源 Resource | 类型 Type | 链接 Link |
|--------------|----------|----------|
| TinyAWG 开源项目 (OSHWhub) | 开源硬件 | https://oshwhub.com/greentor/tinyawg-signal-source |
| TinyAWG 搭建文档 (飞书) | 技术文档 | https://ai.feishu.cn/docx/BGycd4URIot8Aoxs3vLcBdq2nFc |
| SMD 焊接入门教程 | B站搜索 | 搜索"SMD 焊接教程 0402" |
| QFN 芯片拖焊教程 | B站搜索 | 搜索"QFN 拖焊教程" |
| AD9744 数据手册 | PDF | 搜索"AD9744 datasheet" |
| LMR544006 DCDC | PDF | 搜索"LMR544006 datasheet" |
| OPA2673 运放 | PDF | 搜索"OPA2673IRGVT datasheet" |

---

*最后更新：2026-05-27*
*Last updated: 2026-05-27*
