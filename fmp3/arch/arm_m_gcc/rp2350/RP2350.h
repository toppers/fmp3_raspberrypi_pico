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

#ifndef TOPPERS_RP2350_H
#define TOPPERS_RP2350_H

#ifndef CAST
#define CAST(type, val) ((type)(val))
#endif

#define RP2350_CLOCKS_BASE                  0x40010000
#define RP2350_CLOCKS_CLK_REF_CTRL          CAST(uint32_t *, RP2350_CLOCKS_BASE + 0x30)
#define RP2350_CLOCKS_CLK_REF_DIV           CAST(uint32_t *, RP2350_CLOCKS_BASE + 0x34)
#define RP2350_CLOCKS_CLK_REF_SELECTED      CAST(uint32_t *, RP2350_CLOCKS_BASE + 0x38)
#define RP2350_CLOCKS_CLK_SYS_CTRL          CAST(uint32_t *, RP2350_CLOCKS_BASE + 0x3C)
#define RP2350_CLOCKS_CLK_SYS_DIV           CAST(uint32_t *, RP2350_CLOCKS_BASE + 0x40)
#define RP2350_CLOCKS_CLK_SYS_SELECTED      CAST(uint32_t *, RP2350_CLOCKS_BASE + 0x44)
#define RP2350_CLOCKS_CLK_PERI_CTRL         CAST(uint32_t *, RP2350_CLOCKS_BASE + 0x48)
#define RP2350_CLOCKS_CLK_PERI_DIV          CAST(uint32_t *, RP2350_CLOCKS_BASE + 0x4C)
#define RP2350_CLOCKS_CLK_PERI_SELECTED     CAST(uint32_t *, RP2350_CLOCKS_BASE + 0x50)
#define RP2350_CLOCKS_CLK_HSTX_CTRL         CAST(uint32_t *, RP2350_CLOCKS_BASE + 0x54)
#define RP2350_CLOCKS_CLK_HSTX_DIV          CAST(uint32_t *, RP2350_CLOCKS_BASE + 0x58)
#define RP2350_CLOCKS_CLK_HSTX_SELECTED     CAST(uint32_t *, RP2350_CLOCKS_BASE + 0x5C)
#define RP2350_CLOCKS_CLK_USB_CTRL          CAST(uint32_t *, RP2350_CLOCKS_BASE + 0x60)
#define RP2350_CLOCKS_CLK_USB_DIV           CAST(uint32_t *, RP2350_CLOCKS_BASE + 0x64)
#define RP2350_CLOCKS_CLK_USB_SELECTED      CAST(uint32_t *, RP2350_CLOCKS_BASE + 0x68)
#define RP2350_CLOCKS_CLK_SYS_RESUS_CTRL    CAST(uint32_t *, RP2350_CLOCKS_BASE + 0x84)

#define RP2350_CLOCKS_CLK_REF_CTRL_SRC_XOSC       (0x2 << 0)
#define RP2350_CLOCKS_CLK_REF_CTRL_SRC_BITS       (0x3 << 0)
#define RP2350_CLOCKS_CLK_SYS_CTRL_AUXSRC_PLL_SYS (0x0 << 5)
#define RP2350_CLOCKS_CLK_SYS_CTRL_SRC_REF        (0x0 << 0)
#define RP2350_CLOCKS_CLK_SYS_CTRL_SRC_AUX        (0x1 << 0)
#define RP2350_CLOCKS_CLK_SYS_CTRL_SRC_BIT        (0x1 << 0)
#define RP2350_CLOCKS_CLK_PERI_CTRL_ENABLE        (1 << 11)
#define RP2350_CLOCKS_CLK_PERI_CTRL_SRC_CLK_SYS   (0 << 0)
#define RP2350_CLOCKS_CLK_USB_CTRL_CLKSRC_PLL_USB (0 << 0)
#define RP2350_CLOCKS_CLK_HSTX_CTRL_SRC_CLK_SYS   (0 << 0)

#define RP2350_RESETS_BASE        0x40020000
#define RP2350_RESETS_RESET       CAST(uint32_t *, RP2350_RESETS_BASE + 0x0)
#define RP2350_RESETS_RESET_DONE  CAST(uint32_t *, RP2350_RESETS_BASE + 0x8)

#define RP2350_RESETS_RESET_BITS        (0x1fffffff)
#define RP2350_RESETS_RESET_USBCTRL     (1 << 28)
#define RP2350_RESETS_RESET_UART1       (1 << 27)
#define RP2350_RESETS_RESET_UART0       (1 << 26)
#define RP2350_RESETS_RESET_TIMER0      (1 << 23)
#define RP2350_RESETS_RESET_SYSCFG      (1 << 20)
#define RP2350_RESETS_RESET_SPI1        (1 << 19)
#define RP2350_RESETS_RESET_SPI0        (1 << 18)
#define RP2350_RESETS_RESET_PLL_USB     (1 << 15)
#define RP2350_RESETS_RESET_PLL_SYS     (1 << 14)
#define RP2350_RESETS_RESET_PADS_QSPI   (1 << 10)
#define RP2350_RESETS_RESET_PADS_BANK0  (1 << 9)
#define RP2350_RESETS_RESET_IO_QSPI     (1 << 7)
#define RP2350_RESETS_RESET_IO_BANK0    (1 << 6)
#define RP2350_RESETS_RESET_HSTX        (1 << 3)
#define RP2350_RESETS_RESET_BUSCTRL     (1 << 1)
#define RP2350_RESETS_RESET_ADC         (1 << 0)

#define RP2350_PSM_BASE      0x40018000
#define RP2350_PSM_FRCE_ON   CAST(uint32_t *, RP2350_PSM_BASE + 0x0)
#define RP2350_PSM_FRCE_OFF  CAST(uint32_t *, RP2350_PSM_BASE + 0x4)
#define RP2350_PSM_FRCE_DONE CAST(uint32_t *, RP2350_PSM_BASE + 0xC)

#define RP2350_PSM_PROC1        (1 << 24)

#define RP2350_IO_BANK0_BASE                0x40028000
#define RP2350_IO_BANK0_GPIO_STATUS(n)      CAST(uint32_t *, RP2350_IO_BANK0_BASE + (n) * 8)
#define RP2350_IO_BANK0_GPIO_CTRL(n)        CAST(uint32_t *, RP2350_IO_BANK0_BASE + (n) * 8 + 4)

#define RP2350_PADS_BANK0_BASE              0x40038000
#define RP2350_PADS_BANK0_GPIO(n)           CAST(uint32_t *, RP2350_PADS_BANK0_BASE + (n) * 4 + 4)

#define RP2350_PADS_BANK0_GPIOX_IE          0x00000040
#define RP2350_PADS_BANK0_GPIOX_OD          0x00000080
#define RP2350_PADS_BANK0_GPIOX_ISO         0x00000100

#define RP2350_XOSC_BASE    0x40048000
#define RP2350_XOSC_CTRL    CAST(uint32_t *, RP2350_XOSC_BASE + 0x00)
#define RP2350_XOSC_STATUS  CAST(uint32_t *, RP2350_XOSC_BASE + 0x04)
#define RP2350_XOSC_DORMANT CAST(uint32_t *, RP2350_XOSC_BASE + 0x08)
#define RP2350_XOSC_STARTUP CAST(uint32_t *, RP2350_XOSC_BASE + 0x0C)
#define RP2350_XOSC_COUNT   CAST(uint32_t *, RP2350_XOSC_BASE + 0x10)

#define RP2350_XOSC_CTRL_ENABLE_MAGIC     (0xfab << 12)
#define RP2350_XOSC_CTRL_FREQ_RANGE_MAGIC (0xaa0 << 0)
#define RP2350_XOSC_STATUS_STABLE         (1 << 31)

#define RP2350_PLL_SYS_BASE      0x40050000
#define RP2350_PLL_SYS_CS        CAST(uint32_t *, RP2350_PLL_SYS_BASE + 0x00)
#define RP2350_PLL_SYS_PWR       CAST(uint32_t *, RP2350_PLL_SYS_BASE + 0x04)
#define RP2350_PLL_SYS_FBDIV_INT CAST(uint32_t *, RP2350_PLL_SYS_BASE + 0x08)
#define RP2350_PLL_SYS_PRIM      CAST(uint32_t *, RP2350_PLL_SYS_BASE + 0x0C)

#define RP2350_PLL_SYS_CS_LOCK          (1 << 31)
#define RP2350_PLL_SYS_CS_REFDIV(n)     ((n) << 0)
#define RP2350_PLL_SYS_PRIM_POSTDIV1(n) ((n) << 16)
#define RP2350_PLL_SYS_PWR_PD           (1 << 0)
#define RP2350_PLL_SYS_PWR_VCOPD        (1 << 5)
#define RP2350_PLL_SYS_PRIM_POSTDIV2(n) ((n) << 12)

#define RP2350_PLL_USB_BASE      0x40058000
#define RP2350_PLL_USB_CS        CAST(uint32_t *, RP2350_PLL_USB_BASE + 0x00)
#define RP2350_PLL_USB_PWR       CAST(uint32_t *, RP2350_PLL_USB_BASE + 0x04)
#define RP2350_PLL_USB_FBDIV_INT CAST(uint32_t *, RP2350_PLL_USB_BASE + 0x08)
#define RP2350_PLL_USB_PRIM      CAST(uint32_t *, RP2350_PLL_USB_BASE + 0x0C)

#define RP2350_PLL_USB_CS_LOCK          (1 << 31)
#define RP2350_PLL_USB_CS_REFDIV(n)     ((n) << 0)
#define RP2350_PLL_USB_PRIM_POSTDIV1(n) ((n) << 16)
#define RP2350_PLL_USB_PWR_PD           (1 << 0)
#define RP2350_PLL_USB_PWR_VCOPD        (1 << 5)
#define RP2350_PLL_USB_PRIM_POSTDIV2(n) ((n) << 12)

#define RP2350_UART0_BASE       0x40070000
#define RP2350_UART1_BASE       0x40078000
#define RP2350_UART_DR(base)    CAST(uint32_t *, (base) + 0x000)
#define RP2350_UART_FR(base)    CAST(uint32_t *, (base) + 0x018)
#define RP2350_UART_IBRD(base)  CAST(uint32_t *, (base) + 0x024)
#define RP2350_UART_FBRD(base)  CAST(uint32_t *, (base) + 0x028)
#define RP2350_UART_LCR_H(base) CAST(uint32_t *, (base) + 0x02C)
#define RP2350_UART_CR(base)    CAST(uint32_t *, (base) + 0x030)
#define RP2350_UART_IFLS(base)  CAST(uint32_t *, (base) + 0x034)
#define RP2350_UART_IMSC(base)  CAST(uint32_t *, (base) + 0x038)
#define RP2350_UART_MIS(base)   CAST(uint32_t *, (base) + 0x040)
#define RP2350_UART_ICR(base)   CAST(uint32_t *, (base) + 0x044)

#define RP2350_UART_FR_TXFF             (1 << 5)
#define RP2350_UART_FR_RXFE             (1 << 4)
#define RP2350_UART_LCR_H_WLEN_8BITS    (0b11 << 5)
#define RP2350_UART_LCR_H_FEN           (1 << 4)
#define RP2350_UART_CR_RXE              (1 << 9)
#define RP2350_UART_CR_TXE              (1 << 8)
#define RP2350_UART_CR_UARTEN           (1 << 0)
#define RP2350_UART_IMSC_RTIM           (1 << 6)
#define RP2350_UART_IMSC_TXIM           (1 << 5)
#define RP2350_UART_IMSC_RXIM           (1 << 4)
#define RP2350_UART_MIS_RTIM            (1 << 6)
#define RP2350_UART_MIS_TXMIS           (1 << 5)
#define RP2350_UART_MIS_RXMIS           (1 << 4)
#define RP2350_UART_ICR_RTIM            (1 << 6)
#define RP2350_UART_ICR_TXIC            (1 << 5)
#define RP2350_UART_ICR_RXIC            (1 << 4)

#define RP2350_TICKS_BASE               0x40108000
#define RP2350_TICKS_TIMER0_CTRL        CAST(uint32_t *, RP2350_TICKS_BASE + 0x18)
#define RP2350_TICKS_TIMER0_CYCLES      CAST(uint32_t *, RP2350_TICKS_BASE + 0x1c)

#define RP2350_TICKS_TIMER0_CTRL_RUNNING (1 << 1)
#define RP2350_TICKS_TIMER0_CTRL_ENABLE  (1 << 0)

#define RP2350_TIMER0_BASE      0x400b0000
#define RP2350_TIMER0_ALARM0    CAST(uint32_t *, RP2350_TIMER0_BASE + 0x10)
#define RP2350_TIMER0_ALARM1    CAST(uint32_t *, RP2350_TIMER0_BASE + 0x14)
#define RP2350_TIMER0_ALARM2    CAST(uint32_t *, RP2350_TIMER0_BASE + 0x18)
#define RP2350_TIMER0_ALARM3    CAST(uint32_t *, RP2350_TIMER0_BASE + 0x1C)
#define RP2350_TIMER0_ARMED     CAST(uint32_t *, RP2350_TIMER0_BASE + 0x20)
#define RP2350_TIMER0_TIMERAWL  CAST(uint32_t *, RP2350_TIMER0_BASE + 0x28)
#define RP2350_TIMER0_SOURCE    CAST(uint32_t *, RP2350_TIMER0_BASE + 0x38)
#define RP2350_TIMER0_INTR      CAST(uint32_t *, RP2350_TIMER0_BASE + 0x3C)
#define RP2350_TIMER0_INTE      CAST(uint32_t *, RP2350_TIMER0_BASE + 0x40)
#define RP2350_TIMER0_INTF      CAST(uint32_t *, RP2350_TIMER0_BASE + 0x44)

#define RP2350_TIMER0_SOURCE_TICK       (0 << 0)
#define RP2350_TIMER0_SOURCE_CLK_SYS    (1 << 0)

#define RP2350_TIMER0_INT_ALARM_0       (1 << 0)
#define RP2350_TIMER0_INT_ALARM_1       (1 << 1)
#define RP2350_TIMER0_INT_ALARM_2       (1 << 2)
#define RP2350_TIMER0_INT_ALARM_3       (1 << 3)

#define RP2350_SIO_BASE         0xd0000000
#define RP2350_SIO_CPUID        CAST(uint32_t *, RP2350_SIO_BASE + 0x000)
#define RP2350_SIO_FIFO_ST      CAST(uint32_t *, RP2350_SIO_BASE + 0x050)
#define RP2350_SIO_FIFO_WR      CAST(uint32_t *, RP2350_SIO_BASE + 0x054)
#define RP2350_SIO_FIFO_RD      CAST(uint32_t *, RP2350_SIO_BASE + 0x058)
#define RP2350_SIO_SPINLOCK_ST  CAST(uint32_t *, RP2350_SIO_BASE + 0x05C)
#define RP2350_SIO_SPINLOCKn(n) CAST(uint32_t *, RP2350_SIO_BASE + 0x100 + 4 * (n))

#define RP2350_SIO_FIFO_ST_ROE (1 << 3)
#define RP2350_SIO_FIFO_ST_WOF (1 << 2)
#define RP2350_SIO_FIFO_ST_RDY (1 << 1)
#define RP2350_SIO_FIFO_ST_VLD (1 << 0)

#define RP2350_BOOTRAM_BASE                 0x400e0000
#define RP2350_BOOTRAM_BOOTLOCK0_OFFSET     0x0000080c

#define RP2350_PPB_BASE                     0xe0000000
#define RP2350_PPB_NONSEC_BASE              0xe0020000
#define RP2350_M33_CPUID_OFFSET             0x0000ed00
#define RP2350_M33_CPUID                    CAST(uint32_t *, RP2350_PPB_BASE + RP2350_M33_CPUID_OFFSET)
#define RP2350_M33_CPUID_NONSEC             CAST(uint32_t *, RP2350_PPB_NONSEC_BASE + RP2350_M33_CPUID_OFFSET)
#define RP2350_M33_VTOR_OFFSET              0x0000ed08
#define RP2350_M33_VTOR                     CAST(uint32_t *, RP2350_PPB_BASE + RP2350_M33_VTOR_OFFSET)
#define RP2350_M33_VTOR_NONSEC              CAST(uint32_t *, RP2350_PPB_NONSEC_BASE + RP2350_M33_VTOR_OFFSET)
#define RP2350_M33_SCR_OFFSET			    0x0000ed10
#define RP2350_M33_SCR                      CAST(uint32_t *, RP2350_PPB_BASE + RP2350_M33_SCR_OFFSET)
#define RP2350_M33_SCR_NONSEC               CAST(uint32_t *, RP2350_PPB_NONSEC_BASE + RP2350_M33_SCR_OFFSET)
#define RP2350_M33_CPACR_OFFSET             0x0000ed88
#define RP2350_M33_CPACR                    CAST(uint32_t *, RP2350_PPB_BASE + RP2350_M33_CPACR_OFFSET)
#define RP2350_M33_MPU_RLAR_OFFSET          0x0000eda0
#define RP2350_M33_MPU_RLAR                 CAST(uint32_t *, RP2350_PPB_BASE + RP2350_M33_MPU_RLAR_OFFSET)
#define RP2350_M33_MPU_RLAR_NONSEC          CAST(uint32_t *, RP2350_PPB_NONSEC_BASE + RP2350_M33_MPU_RLAR_OFFSET)

#define RP2350_M33_CPACR_CP0_BITS           0x00000003
#define RP2350_M33_CPACR_CP4_BITS           0x00000300
#define RP2350_M33_CPACR_CP10_BITS          0x00300000

#define RP2350_M33_SCR_SLEEPDEEP_BITS	    (1 << 2)
#define RP2350_M33_MPU_RLAR_EN_BITS         (1 << 0)

#define RP2350_BOOTRAM_BASE                 0x400e0000
#define RP2350_BOOTRAM_BOOTLOCK_OFFSET      0x0000080c
#define RP2350_BOOTRAM_BOOTLOCK(n)          CAST(uint32_t *, RP2350_BOOTRAM_BASE + RP2350_BOOTRAM_BOOTLOCK_OFFSET + (n) * 4)

#define RP2350_TIMER0_0_IRQn   0
#define RP2350_TIMER0_1_IRQn   1
#define RP2350_TIMER0_2_IRQn   2
#define RP2350_TIMER0_3_IRQn   3
#define RP2350_SIO_FIFO_IRQn  25
#define RP2350_UART0_IRQn     33
#define RP2350_UART1_IRQn     34

#endif /* TOPPERS_RP2350_H */
