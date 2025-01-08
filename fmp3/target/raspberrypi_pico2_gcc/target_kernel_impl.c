/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2016 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 * 
 *  上記著作権者は，以下の(1)～(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 * 
 */

/*
 * ターゲット依存モジュール（RaspberryPi Pico用）
 */
#include "kernel_impl.h"
#include "target_syssvc.h"
#include <sil.h>
#ifdef TOPPERS_OMIT_TECS
#include "chip_serial.h"
#endif
#include <stdatomic.h>

/*
 * エラー時の処理
 */
extern void Error_Handler(void);

#define ROM_TABLE_CODE(c1, c2) ((c1) | ((c2) << 8))
#define ROM_FUNC_BOOTROM_STATE_RESET            ROM_TABLE_CODE('S', 'R')
#define RT_FLAG_FUNC_ARM_SEC    0x0004
#define RT_FLAG_FUNC_ARM_NONSEC 0x0010
#define BOOTROM_STATE_RESET_GLOBAL_STATE 0x04
#define BOOTROM_STATE_RESET_CURRENT_CORE 0x01

static int pico_processor_state_is_nonsecure()
{
    uint32_t tt;
    Asm (
        "movs %0, #0\n"
        "tt %0, %0\n"
        : "=r" (tt) : : "cc"
    );
    return !(tt & (1u << 22));
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
static void bootrom_state_reset(uint32_t flags)
{
    void *(*rom_table_lookup_fn)(uint32_t table, uint32_t code);
    rom_table_lookup_fn = (void *(*)(uint32_t table, uint32_t code))(uintptr_t)sil_reh_mem((uint16_t*)(0x14 + ((sil_reb_mem((uint8_t *)0x13) == 2) ? 2 : 4)));
    void (*rom_bootrom_state_reset_fn)(uint32_t flags);
    uint32_t code = (pico_processor_state_is_nonsecure()) ? RT_FLAG_FUNC_ARM_NONSEC : RT_FLAG_FUNC_ARM_SEC;
    uint32_t table = ROM_FUNC_BOOTROM_STATE_RESET;
    rom_bootrom_state_reset_fn = (void (*)(uint32_t flags))rom_table_lookup_fn(table, code);
    rom_bootrom_state_reset_fn(flags);
}
#pragma GCC diagnostic pop

/*
 * 起動時のハードウェア初期化処理
 */
void hardware_init_hook(void)
{
    if (ID_PRC(get_my_prcidx()) == PRC1) {
        bootrom_state_reset(BOOTROM_STATE_RESET_GLOBAL_STATE);

        sil_wrw_mem(RP2350_PSM_FRCE_OFF, RP2350_PSM_PROC1); /* Reset sub-processor */
        sil_wrw_mem(RP2350_PSM_FRCE_OFF, 0);

        /* Reset everything but the fundamental parts */
        uint32_t mask = ~(RP2350_RESETS_RESET_PLL_SYS | RP2350_RESETS_RESET_SYSCFG
                    | RP2350_RESETS_RESET_USBCTRL | RP2350_RESETS_RESET_PLL_USB
                    | RP2350_RESETS_RESET_PADS_QSPI | RP2350_RESETS_RESET_IO_QSPI);
        sil_orw(RP2350_RESETS_RESET, mask);
        mask = RP2350_RESETS_RESET_BITS & ~(RP2350_RESETS_RESET_USBCTRL
                    | RP2350_RESETS_RESET_UART1 | RP2350_RESETS_RESET_UART0
                    | RP2350_RESETS_RESET_SPI1 | RP2350_RESETS_RESET_SPI0
                    | RP2350_RESETS_RESET_ADC | RP2350_RESETS_RESET_HSTX);
        sil_clrw(RP2350_RESETS_RESET, mask);
        while (((~sil_rew_mem(RP2350_RESETS_RESET_DONE)) & mask) != 0) ;

        /*
        * +------------+    +-------+  1500MHz  +-----+  300MHz  +-----+
        * | XOSC 12MHz | -> | * 150 | --------> | / 5 | -------> | / 2 | -> 150MHz
        * +------------+    +-------+           +-----+          +-----+
        */
        sil_wrw_mem(RP2350_CLOCKS_CLK_SYS_RESUS_CTRL, 0);
        /* XOSC is inactive. Activate it first */
        sil_wrw_mem(RP2350_XOSC_CTRL, RP2350_XOSC_CTRL_FREQ_RANGE_MAGIC);
        sil_wrw_mem(RP2350_XOSC_STARTUP, 47); /* 1ms delay. refer to the datasheet */
        sil_orw(RP2350_XOSC_CTRL, RP2350_XOSC_CTRL_ENABLE_MAGIC); /* Enable OSC */
        while ((sil_rew_mem(RP2350_XOSC_STATUS) & RP2350_XOSC_STATUS_STABLE) == 0) ; /* Wait to be stable */

        /* clear clk_sys */
        sil_clrw(RP2350_CLOCKS_CLK_SYS_CTRL, RP2350_CLOCKS_CLK_SYS_CTRL_SRC_BIT);
        while (sil_rew_mem(RP2350_CLOCKS_CLK_SYS_SELECTED) != 0x1) ;

        /* clear clk_ref */
        sil_clrw(RP2350_CLOCKS_CLK_REF_CTRL, RP2350_CLOCKS_CLK_REF_CTRL_SRC_BITS);
        while (sil_rew_mem(RP2350_CLOCKS_CLK_REF_SELECTED) != 0x1) ;

        /* Reset PLL_SYS */
        sil_orw(RP2350_RESETS_RESET, RP2350_RESETS_RESET_PLL_SYS);
        sil_clrw(RP2350_RESETS_RESET, RP2350_RESETS_RESET_PLL_SYS);
        while ((sil_rew_mem(RP2350_RESETS_RESET_DONE) & RP2350_RESETS_RESET_PLL_SYS) == 0) ;

        sil_wrw_mem(RP2350_PLL_SYS_CS, RP2350_PLL_SYS_CS_REFDIV(1)); /* Set pre-divide 1 */
        sil_wrw_mem(RP2350_PLL_SYS_FBDIV_INT, 125); /* Set mult 125 */
        sil_clrw(RP2350_PLL_SYS_PWR, RP2350_PLL_SYS_PWR_PD|RP2350_PLL_SYS_PWR_VCOPD);
        while ((sil_rew_mem(RP2350_PLL_SYS_CS) & RP2350_PLL_SYS_CS_LOCK) == 0) ; /* Wait for locking */
        sil_wrw_mem(RP2350_PLL_SYS_PRIM, RP2350_PLL_SYS_PRIM_POSTDIV1(5)
                                    | RP2350_PLL_SYS_PRIM_POSTDIV2(2)); /* Set post-divide */
        sil_wrw_mem(RP2350_PLL_SYS_PWR, 0); /* Power up PLL */

        /* Reset PLL_USB */
        sil_orw(RP2350_RESETS_RESET, RP2350_RESETS_RESET_PLL_USB);
        sil_clrw(RP2350_RESETS_RESET, RP2350_RESETS_RESET_PLL_USB);
        while ((sil_rew_mem(RP2350_RESETS_RESET_DONE) & RP2350_RESETS_RESET_PLL_USB) == 0) ;

        sil_wrw_mem(RP2350_PLL_USB_CS, RP2350_PLL_USB_CS_REFDIV(1)); /* Set pre-divide 1 */
        sil_wrw_mem(RP2350_PLL_USB_FBDIV_INT, 100); /* Set mult 100 */
        sil_clrw(RP2350_PLL_USB_PWR, RP2350_PLL_USB_PWR_PD|RP2350_PLL_USB_PWR_VCOPD);
        while ((sil_rew_mem(RP2350_PLL_USB_CS) & RP2350_PLL_USB_CS_LOCK) == 0) ; /* Wait for locking */
        sil_wrw_mem(RP2350_PLL_USB_PRIM, RP2350_PLL_USB_PRIM_POSTDIV1(5)
                                    | RP2350_PLL_USB_PRIM_POSTDIV2(5)); /* Set post-divide */
        sil_wrw_mem(RP2350_PLL_USB_PWR, 0); /* Power up PLL */

        /* XOSC -> clk_ref (glitchless) */
        sil_wrw_mem(RP2350_CLOCKS_CLK_REF_CTRL, RP2350_CLOCKS_CLK_REF_CTRL_SRC_XOSC);
        sil_wrw_mem(RP2350_CLOCKS_CLK_REF_DIV, 1 << 16);
        while (sil_rew_mem(RP2350_CLOCKS_CLK_REF_SELECTED) != (1 << RP2350_CLOCKS_CLK_REF_CTRL_SRC_XOSC)) ;

        /* clk_ref -> clk_sys (glitchless) */
        sil_wrw_mem(RP2350_CLOCKS_CLK_SYS_CTRL, RP2350_CLOCKS_CLK_SYS_CTRL_SRC_REF);
        while (sil_rew_mem(RP2350_CLOCKS_CLK_SYS_SELECTED) != (1 << RP2350_CLOCKS_CLK_SYS_CTRL_SRC_REF)) ;

        /* Set clk_sys = pll_sys / 1 */
        sil_wrw_mem(RP2350_CLOCKS_CLK_SYS_DIV, 1 << 16);
        sil_wrw_mem(RP2350_CLOCKS_CLK_SYS_CTRL, RP2350_CLOCKS_CLK_SYS_CTRL_AUXSRC_PLL_SYS
                                            | RP2350_CLOCKS_CLK_SYS_CTRL_SRC_AUX);

        /* pll_usb -> clk_usb */
        sil_wrw_mem(RP2350_CLOCKS_CLK_USB_CTRL, RP2350_CLOCKS_CLK_USB_CTRL_CLKSRC_PLL_USB);
        sil_wrw_mem(RP2350_CLOCKS_CLK_USB_DIV, 1 << 16);
        while (sil_rew_mem(RP2350_CLOCKS_CLK_USB_SELECTED) != (1 << RP2350_CLOCKS_CLK_USB_CTRL_CLKSRC_PLL_USB)) ;

        /* clk_sys -> clk_peri */
        sil_wrw_mem(RP2350_CLOCKS_CLK_PERI_CTRL, RP2350_CLOCKS_CLK_PERI_CTRL_SRC_CLK_SYS);
        sil_wrw_mem(RP2350_CLOCKS_CLK_PERI_DIV, 1 << 16);
        while (sil_rew_mem(RP2350_CLOCKS_CLK_PERI_SELECTED) != (1 << RP2350_CLOCKS_CLK_PERI_CTRL_SRC_CLK_SYS)) ;

        /* clk_sys -> clk_hstx */
        sil_wrw_mem(RP2350_CLOCKS_CLK_HSTX_CTRL, RP2350_CLOCKS_CLK_HSTX_CTRL_SRC_CLK_SYS);
        sil_wrw_mem(RP2350_CLOCKS_CLK_HSTX_DIV, 1 << 16);
        while (sil_rew_mem(RP2350_CLOCKS_CLK_HSTX_SELECTED) != (1 << RP2350_CLOCKS_CLK_HSTX_CTRL_SRC_CLK_SYS)) ;

        /* Reset BUSCTRL */
        sil_orw(RP2350_RESETS_RESET, RP2350_RESETS_RESET_BUSCTRL);
        sil_clrw(RP2350_RESETS_RESET, RP2350_RESETS_RESET_BUSCTRL);
        while ((sil_rew_mem(RP2350_RESETS_RESET_DONE) & RP2350_RESETS_RESET_BUSCTRL) == 0) ;

        /* Reset GPIO */
        sil_orw(RP2350_RESETS_RESET, (RP2350_RESETS_RESET_IO_BANK0 | RP2350_RESETS_RESET_PADS_BANK0));
        sil_clrw(RP2350_RESETS_RESET, (RP2350_RESETS_RESET_IO_BANK0 | RP2350_RESETS_RESET_PADS_BANK0));
        while ((sil_rew_mem(RP2350_RESETS_RESET_DONE) & (RP2350_RESETS_RESET_IO_BANK0 | RP2350_RESETS_RESET_PADS_BANK0)) == 0) ;
        /* TX:GP0(F2) */
        sil_mskw(RP2350_PADS_BANK0_GPIO(0), RP2350_PADS_BANK0_GPIOX_IE,
                   RP2350_PADS_BANK0_GPIOX_IE | RP2350_PADS_BANK0_GPIOX_OD);
        sil_wrw_mem(RP2350_IO_BANK0_GPIO_CTRL(0), 2);
        sil_clrw(RP2350_PADS_BANK0_GPIO(0), RP2350_PADS_BANK0_GPIOX_ISO);
        /* RX:GP1(F2) */
        sil_mskw(RP2350_PADS_BANK0_GPIO(1), RP2350_PADS_BANK0_GPIOX_IE,
                   RP2350_PADS_BANK0_GPIOX_IE | RP2350_PADS_BANK0_GPIOX_OD);
        sil_wrw_mem(RP2350_IO_BANK0_GPIO_CTRL(1), 2);
        sil_clrw(RP2350_PADS_BANK0_GPIO(1), RP2350_PADS_BANK0_GPIOX_ISO);

        /* Reset SIO (hardware spinlock) */
        for (int i = 0; i < 32; ++i) {
            sil_wrw_mem(RP2350_SIO_SPINLOCKn(i), 0);
        }

        /* Reset boot blocks */
        for (int i = 0; i < 8; ++i) {
            atomic_thread_fence(memory_order_release);
            *(long int *)(RP2350_BOOTRAM_BASE + RP2350_BOOTRAM_BOOTLOCK0_OFFSET + i * 4) = 0;
        }

        /* bootrom locking enable */
        while(__builtin_expect(!*(long int *)(RP2350_BOOTRAM_BASE + RP2350_BOOTRAM_BOOTLOCK0_OFFSET + 7 * 4), 0)) ;
        atomic_thread_fence(memory_order_acquire);
    }
    // PRC1 & PRC2
    bootrom_state_reset(BOOTROM_STATE_RESET_CURRENT_CORE);

    // FPU & GPIO Enable
    sil_wrw_mem(RP2350_M33_CPACR, RP2350_M33_CPACR_CP0_BITS | RP2350_M33_CPACR_CP4_BITS | RP2350_M33_CPACR_CP10_BITS);
    Asm("mrc p4,#0,r0,c0,c0,#1" : : : "r0"); // clear engaged flag via RCMP
}

void software_init_hook(void)
{
    /* Initialize sio for fput */
#ifdef TOPPERS_OMIT_TECS
    sio_initialize(0);
    sio_opn_por(SIOPID_FPUT, 0);
#endif
}

extern const FP* const p_vector_table[];

/*
 * Master processor initialization before str_ker().
 * (E.g.: initialize the interrupt controller or the SCU.)
 */
void
target_mprc_initialize(void)
{
    /* Boot CPU1 */

    sil_wrw_mem(RP2350_PSM_FRCE_ON, RP2350_PSM_PROC1);
    while ((sil_rew_mem(RP2350_PSM_FRCE_DONE) & RP2350_PSM_PROC1) == 0) ;

    const uint32_t cmd[] = {0, 0, 1, (uintptr_t)p_vector_table[1], 
                            (uintptr_t)istkpt_table[1], (uintptr_t)_kernel_start};
    int i = 0;
    while (i < sizeof(cmd) / sizeof(cmd[0])) {
        uint32_t c = cmd[i];
        if (c == 0) {
            /* flush garbage */
            while (sil_rew_mem(RP2350_SIO_FIFO_ST) & RP2350_SIO_FIFO_ST_VLD) {
                sil_rew_mem(RP2350_SIO_FIFO_RD);
            }
            Asm("SEV");
        }
        /* send a command */
        while ((sil_rew_mem(RP2350_SIO_FIFO_ST) & RP2350_SIO_FIFO_ST_RDY) == 0) ;
        sil_wrw_mem(RP2350_SIO_FIFO_WR, c);
        Asm("SEV");
        /* receive a response */
        while ((sil_rew_mem(RP2350_SIO_FIFO_ST) & RP2350_SIO_FIFO_ST_VLD) == 0) Asm("WFE");
        if (sil_rew_mem(RP2350_SIO_FIFO_RD) == c) {
            i += 1;
        }
    }
}

#ifndef TOPPERS_OMIT_TECS
/*
 *  システムログの低レベル出力のための初期化
 *
 */
extern void tPutLogSIOPort_initialize(void);
#endif

/*
 * ターゲット依存部 初期化処理
 */
void target_initialize(PCB *p_my_pcb)
{
    /*
     * コア依存部の初期化
     */
    core_initialize(p_my_pcb);
    /*
     * SIOを初期化
     */
#ifndef TOPPERS_OMIT_TECS
    tPutLogSIOPort_initialize();
#endif /* TOPPERS_OMIT_TECS */
}

/*
 * ターゲット依存部 終了処理
 */
void target_exit(void)
{
    /* チップ依存部の終了処理 */
    core_terminate();
    while(1) ;
}

/*
 * エラー発生時の処理
 */
void Error_Handler(void)
{
    while (1) ;
}
