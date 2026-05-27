/**
 * ============================================================================
 * DDS 控制器模块 | DDS Controller Module
 * ============================================================================
 *
 * DDS = Direct Digital Synthesis (直接数字合成)
 * DDS = Direct Digital Synthesis
 *
 * 这是 TinyAWG 波形合成的核心模块。它通过 AXI Lite 接口控制 FPGA PL 端
 * 的 DDS 引擎，实现精确的频率控制和波形选择。
 *
 * This is the core module for TinyAWG waveform synthesis. It controls the
 * FPGA PL-side DDS engine via AXI Lite interface for precise frequency
 * control and waveform selection.
 *
 * ============================================================
 * DDS 原理简述 | DDS Principle Overview
 * ============================================================
 *
 * DDS 的核心是一个 N-bit 相位累加器 (Phase Accumulator):
 * The core of DDS is an N-bit phase accumulator:
 *
 *   相位累加器 Phase Accumulator:
 *     phase[n+1] = phase[n] + FCW
 *
 *   其中 FCW = Frequency Control Word (频率控制字)
 *   Where FCW = Frequency Control Word
 *
 *   输出频率 Output frequency:
 *     f_out = FCW * f_clk / 2^N
 *
 *   频率分辨率 Frequency resolution:
 *     df = f_clk / 2^N
 *
 * 对于 TinyAWG 的参数 For TinyAWG parameters:
 *   f_clk = 200 MHz (FPGA 时钟)
 *   N = 48 bit (相位累加器位宽)
 *   频率分辨率 df = 200e6 / 2^48 ≈ 0.0007 Hz (极高精度!)
 *   Frequency resolution df = 200e6 / 2^48 ≈ 0.0007 Hz
 *
 * 相位累加器的输出作为 BRAM 波形查找表 (LUT) 的地址，
 * 查到的波形数据送入 AD9744 DAC 输出模拟信号。
 *
 * The phase accumulator output serves as the address for the BRAM
 * waveform LUT. The looked-up waveform data is sent to the AD9744
 * DAC to produce the analog output signal.
 *
 * ============================================================
 * AXI Lite 寄存器映射 | AXI Lite Register Map
 * ============================================================
 *
 * 偏移 Offset | 名称 Name        | 功能 Description
 * -----------+-------------------+--------------------------------
 * 0x00       | REG_CTRL          | 控制寄存器 (使能, 波形选择)
 * 0x04       | REG_FCW_LOW       | FCW 低 32 位
 * 0x08       | REG_FCW_HIGH      | FCW 高 16 位
 * 0x0C       | REG_PHASE_OFFSET  | 相位偏移
 * 0x10       | REG_BRAM_WR_ADDR  | BRAM 写入地址
 * 0x14       | REG_BRAM_WR_DATA  | BRAM 写入数据
 * 0x18       | REG_STATUS        | 状态寄存器 (只读)
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "xil_io.h"
#include "xstatus.h"
#include "dds_controller.h"


/* ============================================================
 * 常量定义 | Constant Definitions
 * ============================================================ */

/* DDS 系统时钟频率 | DDS system clock frequency */
#define DDS_CLK_FREQ_HZ     200000000.0  /* 200 MHz */

/* 相位累加器位宽 | Phase accumulator width (bits) */
#define DDS_PHASE_WIDTH      48

/* 2^48 = 281474976710656 */
#define DDS_PHASE_MODULO     ((uint64_t)1 << DDS_PHASE_WIDTH)

/* BRAM 波形查找表深度 | BRAM waveform LUT depth */
#define DDS_LUT_DEPTH        65536        /* 64K 采样点 */

/* AXI Lite 寄存器偏移 | AXI Lite register offsets */
#define REG_CTRL             0x00
#define REG_FCW_LOW          0x04
#define REG_FCW_HIGH         0x08
#define REG_PHASE_OFFSET     0x0C
#define REG_BRAM_WR_ADDR     0x10
#define REG_BRAM_WR_DATA     0x14
#define REG_STATUS           0x18

/* 控制寄存器位定义 | Control register bit definitions */
#define CTRL_ENABLE_BIT      (1 << 0)   /* bit 0: DDS 使能 */
#define CTRL_WAVEFORM_MASK   (0xF << 4) /* bits 7:4: 波形选择 */
#define CTRL_BRAM_WRITE_BIT  (1 << 8)   /* bit 8: BRAM 写使能 */


/* ============================================================
 * 模块内部状态 | Module Internal State
 * ============================================================ */

/**
 * DDS 控制器状态结构体 | DDS controller state structure
 */
typedef struct {
    uint32_t base_addr;       /* AXI Lite 基地址 | AXI Lite base address */
    uint64_t current_fcw;     /* 当前频率控制字 | Current FCW */
    double current_freq;      /* 当前频率 (Hz) | Current frequency */
    bool is_initialized;      /* 是否已初始化 | Whether initialized */
} dds_state_t;

static dds_state_t g_dds;


/* ============================================================
 * AXI Lite 寄存器读写函数 | AXI Lite Register R/W Functions
 * ============================================================ */

/**
 * 写 AXI Lite 寄存器
 * Write AXI Lite register
 *
 * 使用 Xilinx Xil_Out32 函数直接写寄存器。
 * Uses Xilinx Xil_Out32 function for direct register write.
 *
 * @param reg_offset  寄存器偏移地址 (0x00, 0x04, ...)
 * @param value       要写入的 32-bit 值
 */
static inline void axi_write(uint32_t base, uint32_t reg_offset, uint32_t value)
{
    /*
     * Xil_Out32 是 Xilinx 提供的寄存器写入函数:
     *   void Xil_Out32(uint32_t addr, uint32_t value);
     *
     * 它会执行一条 32-bit 写操作到指定的内存映射地址。
     * 在 ZYNQ 上，AXI Lite 外设被映射到处理器的地址空间中，
     * 所以可以用普通的内存写入操作来控制 FPGA 逻辑。
     *
     * Xil_Out32 is a register write function provided by Xilinx.
     * It performs a 32-bit write to the specified memory-mapped address.
     * On ZYNQ, AXI Lite peripherals are mapped into the processor's
     * address space, so normal memory writes can control FPGA logic.
     */
    Xil_Out32(base + reg_offset, value);
}

/**
 * 读 AXI Lite 寄存器
 * Read AXI Lite register
 *
 * @param reg_offset  寄存器偏移地址
 * @return 读取的 32-bit 值
 */
static inline uint32_t axi_read(uint32_t base, uint32_t reg_offset)
{
    return Xil_In32(base + reg_offset);
}


/* ============================================================
 * 公共函数实现 | Public Function Implementations
 * ============================================================ */

/**
 * 初始化 DDS 控制器
 * Initialize DDS controller
 *
 * @param base_addr  AXI Lite 寄存器基地址 (来自 xparameters.h)
 * @return XST_SUCCESS 或 XST_FAILURE
 */
int dds_init(uint32_t base_addr)
{
    g_dds.base_addr = base_addr;
    g_dds.current_fcw = 0;
    g_dds.current_freq = 0.0;
    g_dds.is_initialized = true;

    /* 复位 DDS 引擎: 先禁用，再使能 */
    /* Reset DDS engine: disable first, then enable */
    axi_write(g_dds.base_addr, REG_CTRL, 0x00000000);

    /* 清零相位 | Clear phase */
    axi_write(g_dds.base_addr, REG_FCW_LOW, 0);
    axi_write(g_dds.base_addr, REG_FCW_HIGH, 0);
    axi_write(g_dds.base_addr, REG_PHASE_OFFSET, 0);

    printf("[DDS] 初始化完成, 基地址: 0x%08X\r\n", base_addr);
    return XST_SUCCESS;
}

/**
 * 计算频率控制字 (FCW)
 * Calculate Frequency Control Word (FCW)
 *
 * 公式 Formula:
 *   FCW = f_out * 2^N / f_clk
 *
 * 其中 Where:
 *   f_out  = 目标输出频率 (Hz)
 *   N      = 相位累加器位宽 (48 bit)
 *   f_clk  = DDS 系统时钟 (200 MHz)
 *
 * 示例 Example:
 *   f_out = 1 MHz:
 *     FCW = 1e6 * 2^48 / 200e6 = 1e6 * 281474976710656 / 200e6
 *         = 1407374883553 ≈ 0x147AE147AE1
 *
 * @param freq_hz  目标频率 (Hz)
 * @return 48-bit 频率控制字
 */
uint64_t dds_calc_fcw(double freq_hz)
{
    /*
     * 注意: 48-bit FCW 分为高 16 位和低 32 位写入寄存器
     * Note: 48-bit FCW is split into high 16-bit and low 32-bit
     * for register writing.
     *
     *   FCW = round(f_out * 2^48 / f_clk)
     *       = round(f_out * 281474976710656 / 200000000)
     */
    double fcw_double = freq_hz * (double)DDS_PHASE_MODULO / DDS_CLK_FREQ_HZ;

    /* 防止溢出 (最大频率 = f_clk/2 = 100MHz) */
    /* Prevent overflow (max freq = f_clk/2 = 100MHz) */
    if (fcw_double >= (double)DDS_PHASE_MODULO / 2.0) {
        fcw_double = (double)DDS_PHASE_MODULO / 2.0 - 1.0;
    }
    if (fcw_double < 0) {
        fcw_double = 0;
    }

    return (uint64_t)(fcw_double + 0.5);  /* 四舍五入 | Round */
}

/**
 * 设置 DDS 输出频率
 * Set DDS output frequency
 *
 * 将频率转换为 FCW 并写入 AXI Lite 寄存器。
 * Converts frequency to FCW and writes to AXI Lite registers.
 *
 * @param freq_hz  目标频率 (Hz)
 * @return XST_SUCCESS
 */
int dds_set_frequency(double freq_hz)
{
    if (!g_dds.is_initialized) return XST_FAILURE;

    /* 计算频率控制字 | Calculate FCW */
    g_dds.current_fcw = dds_calc_fcw(freq_hz);
    g_dds.current_freq = freq_hz;

    /* 写入 FCW 寄存器 (低 32 位 + 高 16 位) */
    /* Write FCW registers (low 32-bit + high 16-bit) */
    uint32_t fcw_low  = (uint32_t)(g_dds.current_fcw & 0xFFFFFFFF);
    uint32_t fcw_high = (uint32_t)((g_dds.current_fcw >> 32) & 0x0000FFFF);

    axi_write(g_dds.base_addr, REG_FCW_LOW,  fcw_low);
    axi_write(g_dds.base_addr, REG_FCW_HIGH, fcw_high);

    return XST_SUCCESS;
}

/**
 * 选择波形类型
 * Select waveform type
 *
 * 通过控制寄存器的波形选择字段切换 BRAM 中不同的波形区域。
 * Switches to different waveform regions in BRAM via the waveform
 * select field in the control register.
 *
 * @param type  波形类型 (见 dds_controller.h 中的枚举)
 * @return XST_SUCCESS
 */
int dds_set_waveform(waveform_type_t type)
{
    if (!g_dds.is_initialized) return XST_FAILURE;

    /* 读取当前控制寄存器值 | Read current control register */
    uint32_t ctrl = axi_read(g_dds.base_addr, REG_CTRL);

    /* 清除波形选择位并设置新值 | Clear waveform bits and set new value */
    ctrl &= ~CTRL_WAVEFORM_MASK;
    ctrl |= ((uint32_t)type << 4) & CTRL_WAVEFORM_MASK;

    axi_write(g_dds.base_addr, REG_CTRL, ctrl);

    return XST_SUCCESS;
}

/**
 * 使能/禁用 DDS 输出
 * Enable/disable DDS output
 *
 * @param enable  true=使能输出, false=禁用输出
 */
void dds_enable(bool enable)
{
    if (!g_dds.is_initialized) return;

    uint32_t ctrl = axi_read(g_dds.base_addr, REG_CTRL);

    if (enable) {
        ctrl |= CTRL_ENABLE_BIT;
    } else {
        ctrl &= ~CTRL_ENABLE_BIT;
    }

    axi_write(g_dds.base_addr, REG_CTRL, ctrl);
}

/**
 * 加载波形数据到 FPGA BRAM
 * Load waveform data to FPGA BRAM
 *
 * 逐点通过 AXI Lite 接口写入 BRAM。
 * 每次写入:
 *   1. 设置 BRAM 写入地址 (REG_BRAM_WR_ADDR)
 *   2. 写入波形数据 (REG_BRAM_WR_DATA)
 *   3. FPGA 逻辑在检测到写操作后自动更新 BRAM
 *
 * Writes waveform data point-by-point via AXI Lite interface.
 * Each write:
 *   1. Set BRAM write address (REG_BRAM_WR_ADDR)
 *   2. Write waveform data (REG_BRAM_WR_DATA)
 *   3. FPGA logic auto-updates BRAM upon detecting write
 *
 * @param data   波形数据数组 (14-bit, 0~16383)
 * @param length 数据点数 (最大 65536)
 * @return XST_SUCCESS
 */
int dds_load_waveform(const uint16_t *data, uint32_t length)
{
    if (!g_dds.is_initialized || data == NULL) return XST_FAILURE;

    /* 限制长度不超过 BRAM 深度 */
    /* Clamp length to BRAM depth */
    if (length > DDS_LUT_DEPTH) {
        length = DDS_LUT_DEPTH;
    }

    /* 使能 BRAM 写模式 | Enable BRAM write mode */
    uint32_t ctrl = axi_read(g_dds.base_addr, REG_CTRL);
    ctrl |= CTRL_BRAM_WRITE_BIT;
    axi_write(g_dds.base_addr, REG_CTRL, ctrl);

    /* 逐点写入 BRAM | Write BRAM point by point */
    /*
     * 注意: 这是一个简化的实现。在高速设计中，可以考虑
     * 使用 DMA (AXI DMA) 来加速 BRAM 写入。
     *
     * Note: This is a simplified implementation. In high-speed
     * designs, consider using DMA (AXI DMA) to accelerate
     * BRAM writes.
     */
    for (uint32_t i = 0; i < length; i++) {
        axi_write(g_dds.base_addr, REG_BRAM_WR_ADDR, i);
        axi_write(g_dds.base_addr, REG_BRAM_WR_DATA, (uint32_t)data[i]);
    }

    /* 关闭 BRAM 写模式 | Disable BRAM write mode */
    ctrl = axi_read(g_dds.base_addr, REG_CTRL);
    ctrl &= ~CTRL_BRAM_WRITE_BIT;
    axi_write(g_dds.base_addr, REG_CTRL, ctrl);

    printf("[DDS] 已加载 %lu 点波形数据到 BRAM\r\n", length);
    return XST_SUCCESS;
}

/**
 * 设置相位偏移
 * Set phase offset
 *
 * @param phase_deg  相位偏移角度 (0~360 度)
 */
void dds_set_phase_offset(double phase_deg)
{
    if (!g_dds.is_initialized) return;

    /* 将角度转换为 16-bit 相位值 */
    /* Convert angle to 16-bit phase value */
    /*
     * phase_reg = phase_deg / 360 * 65536
     *
     * 例如: 90 度 -> 16384 (0x4000)
     * Example: 90 degrees -> 16384 (0x4000)
     */
    uint32_t phase_reg = (uint32_t)(phase_deg / 360.0 * 65536.0) & 0xFFFF;
    axi_write(g_dds.base_addr, REG_PHASE_OFFSET, phase_reg);
}

/**
 * 获取 DDS 当前频率
 * Get DDS current frequency
 *
 * @return 当前频率 (Hz)
 */
double dds_get_frequency(void)
{
    return g_dds.current_freq;
}

/**
 * 获取 DDS 状态寄存器
 * Get DDS status register
 *
 * @return 状态寄存器值
 */
uint32_t dds_get_status(void)
{
    if (!g_dds.is_initialized) return 0;
    return axi_read(g_dds.base_addr, REG_STATUS);
}
