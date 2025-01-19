/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2007,2011,2015 by Embedded and Real-Time Systems Laboratory
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

#ifndef TOPPERS_RP2040_H
#define TOPPERS_RP2040_H

#ifndef CAST
#define CAST(type, val) ((type)(val))
#endif

#define RP2040_CLOCKS_BASE             0x40008000
#define RP2040_CLOCKS_CLK_REF_CTRL     CAST(uint32_t *, RP2040_CLOCKS_BASE + 0x30)
#define RP2040_CLOCKS_CLK_REF_DIV      CAST(uint32_t *, RP2040_CLOCKS_BASE + 0x34)
#define RP2040_CLOCKS_CLK_REF_SELECTED CAST(uint32_t *, RP2040_CLOCKS_BASE + 0x38)
#define RP2040_CLOCKS_CLK_SYS_CTRL     CAST(uint32_t *, RP2040_CLOCKS_BASE + 0x3C)
#define RP2040_CLOCKS_CLK_SYS_DIV      CAST(uint32_t *, RP2040_CLOCKS_BASE + 0x40)
#define RP2040_CLOCKS_CLK_SYS_SELECTED CAST(uint32_t *, RP2040_CLOCKS_BASE + 0x44)
#define RP2040_CLOCKS_CLK_PERI_CTRL    CAST(uint32_t *, RP2040_CLOCKS_BASE + 0x48)

#define RP2040_CLOCKS_CLK_REF_CTRL_SRC_XOSC       (0x2 << 0)
#define RP2040_CLOCKS_CLK_REF_CTRL_SRC_BITS       (0x3 << 0)
#define RP2040_CLOCKS_CLK_SYS_CTRL_AUXSRC_PLL_SYS (0x0 << 5)
#define RP2040_CLOCKS_CLK_SYS_CTRL_SRC_REF        (0x0 << 0)
#define RP2040_CLOCKS_CLK_SYS_CTRL_SRC_AUX        (0x1 << 0)
#define RP2040_CLOCKS_CLK_SYS_CTRL_SRC_BIT        (0x1 << 0)
#define RP2040_CLOCKS_CLK_PERI_CTRL_ENABLE        (1 << 11)
#define RP2040_CLOCKS_CLK_PERI_CTRL_SRC_CLK_SYS   (1 << 0)

#define RP2040_RESETS_BASE        0x4000c000
#define RP2040_RESETS_RESET       CAST(uint32_t *, 0x4000c000 + 0x0)
#define RP2040_RESETS_RESET_DONE  CAST(uint32_t *, 0x4000c000 + 0x8)

#define RP2040_RESETS_RESET_UART1     (1 << 23)
#define RP2040_RESETS_RESET_UART0     (1 << 22)
#define RP2040_RESETS_RESET_TIMER     (1 << 21)
#define RP2040_RESETS_RESET_PLL_SYS   (1 << 12)
#define RP2040_RESETS_RESET_PADS_QSPI (1 << 9)
#define RP2040_RESETS_RESET_IO_QSPI   (1 << 6)
#define RP2040_RESETS_RESET_IO_BANK0  (1 << 5)

#define RP2040_PSM_BASE      0x40010000
#define RP2040_PSM_FRCE_ON   CAST(uint32_t *, 0x40010000 + 0x0)
#define RP2040_PSM_FRCE_OFF  CAST(uint32_t *, 0x40010000 + 0x4)
#define RP2040_PSM_FRCE_DONE CAST(uint32_t *, 0x40010000 + 0xC)

#define RP2040_PSM_PROC1 (1 << 16)

#define RP2040_PPB_BASE                     0xe0000000
#define RP2040_PPB_NONSEC_BASE              0xe0020000
#define RP2040_M0PLUS_VTOR_OFFSET           0x0000ed08
#define RP2040_M0PLUS_VTOR                  CAST(uint32_t *, RP2040_PPB_BASE + RP2040_M0PLUS_VTOR_OFFSET)
#define RP2040_M0PLUS_VTOR_NONSEC           CAST(uint32_t *, RP2040_PPB_NONSEC_BASE + RP2040_M0PLUS_VTOR_OFFSET)

#define RP2040_IO_BANK0_BASE           0x40014000
#define RP2040_IO_BANK0_GPIO_STATUS(n) CAST(uint32_t *, RP2040_IO_BANK0_BASE + (n) * 8)
#define RP2040_IO_BANK0_GPIO_CTRL(n)   CAST(uint32_t *, RP2040_IO_BANK0_BASE + (n) * 8 + 4)

#define RP2040_XOSC_BASE    0x40024000
#define RP2040_XOSC_CTRL    CAST(uint32_t *, RP2040_XOSC_BASE + 0x00)
#define RP2040_XOSC_STATUS  CAST(uint32_t *, RP2040_XOSC_BASE + 0x04)
#define RP2040_XOSC_DORMANT CAST(uint32_t *, RP2040_XOSC_BASE + 0x08)
#define RP2040_XOSC_STARTUP CAST(uint32_t *, RP2040_XOSC_BASE + 0x0C)
#define RP2040_XOSC_COUNT   CAST(uint32_t *, RP2040_XOSC_BASE + 0x1C)

#define RP2040_XOSC_CTRL_ENABLE_MAGIC     (0xfab << 12)
#define RP2040_XOSC_CTRL_FREQ_RANGE_MAGIC (0xaa0 << 0)
#define RP2040_XOSC_STATUS_STABLE         (1 << 31)

#define RP2040_PLL_SYS_BASE      0x40028000
#define RP2040_PLL_SYS_CS        CAST(uint32_t *, RP2040_PLL_SYS_BASE + 0x00)
#define RP2040_PLL_SYS_PWR       CAST(uint32_t *, RP2040_PLL_SYS_BASE + 0x04)
#define RP2040_PLL_SYS_FBDIV_INT CAST(uint32_t *, RP2040_PLL_SYS_BASE + 0x08)
#define RP2040_PLL_SYS_PRIM      CAST(uint32_t *, RP2040_PLL_SYS_BASE + 0x0C)

#define RP2040_PLL_SYS_CS_LOCK          (1 << 31)
#define RP2040_PLL_SYS_CS_REFDIV(n)     ((n) << 0)
#define RP2040_PLL_SYS_PRIM_POSTDIV1(n) ((n) << 16)
#define RP2040_PLL_SYS_PWR_PD           (1 << 0)
#define RP2040_PLL_SYS_PWR_VCOPD        (1 << 5)
#define RP2040_PLL_SYS_PRIM_POSTDIV2(n) ((n) << 12)

#define RP2040_UART0_BASE       0x40034000
#define RP2040_UART1_BASE       0x40038000
#define RP2040_UART_DR(base)    CAST(uint32_t *, (base) + 0x000)
#define RP2040_UART_FR(base)    CAST(uint32_t *, (base) + 0x018)
#define RP2040_UART_IBRD(base)  CAST(uint32_t *, (base) + 0x024)
#define RP2040_UART_FBRD(base)  CAST(uint32_t *, (base) + 0x028)
#define RP2040_UART_LCR_H(base) CAST(uint32_t *, (base) + 0x02C)
#define RP2040_UART_CR(base)    CAST(uint32_t *, (base) + 0x030)
#define RP2040_UART_IFLS(base)  CAST(uint32_t *, (base) + 0x034)
#define RP2040_UART_IMSC(base)  CAST(uint32_t *, (base) + 0x038)
#define RP2040_UART_MIS(base)   CAST(uint32_t *, (base) + 0x040)
#define RP2040_UART_ICR(base)   CAST(uint32_t *, (base) + 0x044)

#define RP2040_UART_FR_TXFF          (1 << 5)
#define RP2040_UART_FR_RXFE          (1 << 4)
#define RP2040_UART_LCR_H_WLEN_8BITS (0b11 << 5)
#define RP2040_UART_LCR_H_FEN        (1 << 4)
#define RP2040_UART_CR_RXE           (1 << 9)
#define RP2040_UART_CR_TXE           (1 << 8)
#define RP2040_UART_CR_UARTEN        (1 << 0)
#define RP2040_UART_IMSC_RTIM        (1 << 6)
#define RP2040_UART_IMSC_TXIM        (1 << 5)
#define RP2040_UART_IMSC_RXIM        (1 << 4)
#define RP2040_UART_MIS_RTIM         (1 << 6)
#define RP2040_UART_MIS_TXMIS        (1 << 5)
#define RP2040_UART_MIS_RXMIS        (1 << 4)
#define RP2040_UART_ICR_RTIM         (1 << 6)
#define RP2040_UART_ICR_TXIC         (1 << 5)
#define RP2040_UART_ICR_RXIC         (1 << 4)

#define RP2040_WATCHDOG_BASE 0x40058000
#define RP2040_WATCHDOG_TICK CAST(uint32_t *, RP2040_WATCHDOG_BASE + 0x2c)

#define RP2040_WATCHDOG_RUNNING (1 << 10)
#define RP2040_WATCHDOG_ENABLE  (1 << 9)

#define RP2040_TIMER_BASE     0x40054000
#define RP2040_TIMER_ALARM0   CAST(uint32_t *, RP2040_TIMER_BASE + 0x10)
#define RP2040_TIMER_ALARM1   CAST(uint32_t *, RP2040_TIMER_BASE + 0x14)
#define RP2040_TIMER_ALARM2   CAST(uint32_t *, RP2040_TIMER_BASE + 0x18)
#define RP2040_TIMER_ALARM3   CAST(uint32_t *, RP2040_TIMER_BASE + 0x1C)
#define RP2040_TIMER_ARMED    CAST(uint32_t *, RP2040_TIMER_BASE + 0x20)
#define RP2040_TIMER_TIMERAWL CAST(uint32_t *, RP2040_TIMER_BASE + 0x28)
#define RP2040_TIMER_INTR     CAST(uint32_t *, RP2040_TIMER_BASE + 0x34)
#define RP2040_TIMER_INTE     CAST(uint32_t *, RP2040_TIMER_BASE + 0x38)
#define RP2040_TIMER_INTF     CAST(uint32_t *, RP2040_TIMER_BASE + 0x3C)

#define RP2040_TIMER_INT_ALARM_0 (1 << 0)
#define RP2040_TIMER_INT_ALARM_1 (1 << 1)
#define RP2040_TIMER_INT_ALARM_2 (1 << 2)
#define RP2040_TIMER_INT_ALARM_3 (1 << 3)

#define RP2040_SIO_BASE         0xd0000000
#define RP2040_SIO_CPUID        CAST(uint32_t *, RP2040_SIO_BASE + 0x000)
#define RP2040_SIO_FIFO_ST      CAST(uint32_t *, RP2040_SIO_BASE + 0x050)
#define RP2040_SIO_FIFO_WR      CAST(uint32_t *, RP2040_SIO_BASE + 0x054)
#define RP2040_SIO_FIFO_RD      CAST(uint32_t *, RP2040_SIO_BASE + 0x058)
#define RP2040_SIO_SPINLOCK_ST  CAST(uint32_t *, RP2040_SIO_BASE + 0x05C)
#define RP2040_SIO_SPINLOCKn(n) CAST(uint32_t *, RP2040_SIO_BASE + 0x100 + 4 * (n))

#define RP2040_SIO_FIFO_ST_ROE (1 << 3)
#define RP2040_SIO_FIFO_ST_WOF (1 << 2)
#define RP2040_SIO_FIFO_ST_RDY (1 << 1)
#define RP2040_SIO_FIFO_ST_VLD (1 << 0)

#define RP2040_TIMER0_IRQn    0
#define RP2040_TIMER1_IRQn    1
#define RP2040_TIMER2_IRQn    2
#define RP2040_TIMER3_IRQn    3
#define RP2040_SIO_PROC0_IRQn 15
#define RP2040_SIO_PROC1_IRQn 16
#define RP2040_UART0_IRQn     20
#define RP2040_UART1_IRQn     21

#endif /* TOPPERS_RP2040_H */
