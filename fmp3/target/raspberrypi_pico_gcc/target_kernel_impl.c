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

/*
 * エラー時の処理
 */
extern void Error_Handler(void);

extern volatile int boot2_image;

/*
 * 起動時のハードウェア初期化処理
 */
void hardware_init_hook(void)
{
    if (boot2_image) ; /* Enforce boot2_image to be linked */

    if (ID_PRC(get_my_prcidx()) == PRC2) {
        return;
    }

    sil_wrw_mem(RP2040_PSM_FRCE_OFF, RP2040_PSM_PROC1); /* Reset sub-processor */
    sil_wrw_mem(RP2040_PSM_FRCE_OFF, 0);
    /* Reset everything but the fundamental parts */
    sil_orw(RP2040_RESETS_RESET,
            ~(RP2040_RESETS_RESET_PLL_SYS | RP2040_RESETS_RESET_PADS_QSPI | RP2040_RESETS_RESET_IO_QSPI));
    sil_clrw(RP2040_RESETS_RESET, ~(RP2040_RESETS_RESET_PLL_SYS | RP2040_RESETS_RESET_PADS_QSPI | RP2040_RESETS_RESET_IO_QSPI));

    /*
     * +------------+    +-------+  1596MHz  +-----+  266MHz  +-----+
     * | XOSC 12MHz | -> | * 133 | --------> | / 6 | -------> | / 2 | -> 133MHz
     * +------------+    +-------+           +-----+          +-----+
     */
    if ((sil_rew_mem(RP2040_XOSC_STATUS) & RP2040_XOSC_STATUS_STABLE) == 0) {
        /* XOSC is inactive. Activate it first */
        sil_wrw_mem(RP2040_XOSC_CTRL, RP2040_XOSC_CTRL_FREQ_RANGE_MAGIC);
        sil_wrw_mem(RP2040_XOSC_STARTUP, 47); /* 1ms delay. refer to the datasheet */
        sil_orw(RP2040_XOSC_CTRL, RP2040_XOSC_CTRL_ENABLE_MAGIC); /* Enable OSC */
        while ((sil_rew_mem(RP2040_XOSC_STATUS) & RP2040_XOSC_STATUS_STABLE) == 0) ; /* Wait to be stable */
        /* 
         * Some chip crashes if we switch clocks to XOSC here (hardware issue ?).
         * We assume this is the first power-on reset because XOSC was not active.
         * All the clocks should be derived from ROSC, and we safely reset the PLL.
         */
    } else {
        /* XOSC is already active. Switch clocks */
        /* XTAL -> clk_ref (glitchless) */
        sil_wrw_mem(RP2040_CLOCKS_CLK_REF_CTRL, RP2040_CLOCKS_CLK_REF_CTRL_SRC_XOSC);
        sil_wrw_mem(RP2040_CLOCKS_CLK_REF_DIV, 1 << 8);
        while (sil_rew_mem(RP2040_CLOCKS_CLK_REF_SELECTED) != (1 << RP2040_CLOCKS_CLK_REF_CTRL_SRC_XOSC)) ;

        /* clk_ref -> clk_sys (glitchless) */
        sil_wrw_mem(RP2040_CLOCKS_CLK_SYS_CTRL, RP2040_CLOCKS_CLK_SYS_CTRL_SRC_REF);
        while (sil_rew_mem(RP2040_CLOCKS_CLK_SYS_SELECTED) != (1 << RP2040_CLOCKS_CLK_SYS_CTRL_SRC_REF)) ;
    }

    /* Reset PLL */
    sil_orw(RP2040_RESETS_RESET, RP2040_RESETS_RESET_PLL_SYS);
    sil_clrw(RP2040_RESETS_RESET, RP2040_RESETS_RESET_PLL_SYS);
    while ((sil_rew_mem(RP2040_RESETS_RESET_DONE) & RP2040_RESETS_RESET_PLL_SYS) == 0) ;

    sil_wrw_mem(RP2040_PLL_SYS_CS, RP2040_PLL_SYS_CS_REFDIV(1)); /* Set pre-divide 1 */
    sil_wrw_mem(RP2040_PLL_SYS_FBDIV_INT, 133); /* Set mult 133 */
    sil_clrw(RP2040_PLL_SYS_PWR, RP2040_PLL_SYS_PWR_PD|RP2040_PLL_SYS_PWR_VCOPD);
    while ((sil_rew_mem(RP2040_PLL_SYS_CS) & RP2040_PLL_SYS_CS_LOCK) == 0) ; /* Wait for locking */
    sil_wrw_mem(RP2040_PLL_SYS_PRIM, RP2040_PLL_SYS_PRIM_POSTDIV1(6)
                                   | RP2040_PLL_SYS_PRIM_POSTDIV2(2)); /* Set post-divide */
    sil_wrw_mem(RP2040_PLL_SYS_PWR, 0); /* Power up PLL */

    /* XOSC -> clk_ref (glitchless) */
    sil_wrw_mem(RP2040_CLOCKS_CLK_REF_CTRL, RP2040_CLOCKS_CLK_REF_CTRL_SRC_XOSC);
    sil_wrw_mem(RP2040_CLOCKS_CLK_REF_DIV, 1 << 8);
    while (sil_rew_mem(RP2040_CLOCKS_CLK_REF_SELECTED) != (1 << RP2040_CLOCKS_CLK_REF_CTRL_SRC_XOSC)) ;

    /* clk_ref -> clk_sys (glitchless) */
    sil_wrw_mem(RP2040_CLOCKS_CLK_SYS_CTRL, RP2040_CLOCKS_CLK_SYS_CTRL_SRC_REF);
    while (sil_rew_mem(RP2040_CLOCKS_CLK_SYS_SELECTED) != (1 << RP2040_CLOCKS_CLK_SYS_CTRL_SRC_REF)) ;

    /* Set clk_sys = pll_sys / 1 */
    sil_wrw_mem(RP2040_CLOCKS_CLK_SYS_DIV, 1 << 8);
    sil_wrw_mem(RP2040_CLOCKS_CLK_SYS_CTRL, RP2040_CLOCKS_CLK_SYS_CTRL_AUXSRC_PLL_SYS
                                          | RP2040_CLOCKS_CLK_SYS_CTRL_SRC_AUX);

    /* TX:GP0(F2) RX:GP1(F2) */
    /* Reset GPIO */
    sil_orw(RP2040_RESETS_RESET, RP2040_RESETS_RESET_IO_BANK0);
    sil_clrw(RP2040_RESETS_RESET, RP2040_RESETS_RESET_IO_BANK0);
    while ((sil_rew_mem(RP2040_RESETS_RESET_DONE) & RP2040_RESETS_RESET_IO_BANK0) == 0) ;
    sil_wrw_mem(RP2040_IO_BANK0_GPIO_CTRL(0), 2);
    sil_wrw_mem(RP2040_IO_BANK0_GPIO_CTRL(1), 2);

    /* Reset SIO (hardware spinlock) */
    for (int i = 0; i < 32; ++i) {
        sil_wrw_mem(RP2040_SIO_SPINLOCKn(i), 0);
    }
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

    sil_wrw_mem(RP2040_PSM_FRCE_ON, RP2040_PSM_PROC1);
    while ((sil_rew_mem(RP2040_PSM_FRCE_DONE) & RP2040_PSM_PROC1) == 0) ;

    const uint32_t cmd[] = {0, 0, 1, (uintptr_t)p_vector_table[1], 
                            (uintptr_t)istkpt_table[1], (uintptr_t)_kernel_start};
    int i = 0;
    while (i < sizeof(cmd) / sizeof(cmd[0])) {
        uint32_t c = cmd[i];
        if (c == 0) {
            /* flush garbage */
            while (sil_rew_mem(RP2040_SIO_FIFO_ST) & RP2040_SIO_FIFO_ST_VLD) {
                sil_rew_mem(RP2040_SIO_FIFO_RD);
            }
            Asm("SEV");
        }
        /* send a command */
        while ((sil_rew_mem(RP2040_SIO_FIFO_ST) & RP2040_SIO_FIFO_ST_RDY) == 0) ;
        sil_wrw_mem(RP2040_SIO_FIFO_WR, c);
        Asm("SEV");
        /* receive a response */
        while ((sil_rew_mem(RP2040_SIO_FIFO_ST) & RP2040_SIO_FIFO_ST_VLD) == 0) ;
        if (sil_rew_mem(RP2040_SIO_FIFO_RD) == c) {
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
