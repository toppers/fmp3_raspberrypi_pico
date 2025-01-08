/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2006-2020 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 * 
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
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
 *  $Id: chip_serial.c 289 2021-08-05 14:44:10Z ertl-komori $
 */

/*
 * シリアルインタフェースドライバのチップ依存部（RP2040用）
 * （非TECS版専用）
 */

#include <kernel.h>
#include <sil.h>
#include <t_syslog.h>
#include "target_syssvc.h"
#include "target_serial.h"

struct sio_port_control_block {
    intptr_t exinf;
    uint32_t base;
};

static SIOPCB siopcb[TNUM_PORT];
static const uint32_t bases[TNUM_PORT] = {
    RP2040_UART0_BASE, RP2040_UART1_BASE
};

/*
 * SIOドライバの初期化
 */
void sio_initialize(intptr_t exinf)
{
    /* Select clk_sys */
    sil_wrw_mem(RP2040_CLOCKS_CLK_PERI_CTRL, RP2040_CLOCKS_CLK_PERI_CTRL_ENABLE
                                           | RP2040_CLOCKS_CLK_PERI_CTRL_SRC_CLK_SYS);
}

/*
 * SIOドライバの終了処理
 */
void sio_terminate(intptr_t exinf)
{
}

/*
 * SIOポートのオープン
 */
SIOPCB *sio_opn_por(ID siopid, intptr_t exinf)
{
    const uint32_t index = siopid - 1;
    siopcb[index].exinf = exinf;
    const uint32_t base = bases[index];
    if (siopcb[index].base) {
        /* It's already opened */
        return &siopcb[index];
    }
    siopcb[index].base = base;

    /* Reset UART */
    switch (index) {
        case 0:
            sil_orw(RP2040_RESETS_RESET, RP2040_RESETS_RESET_UART0);
            sil_clrw(RP2040_RESETS_RESET, RP2040_RESETS_RESET_UART0);
            while ((sil_rew_mem(RP2040_RESETS_RESET_DONE) & RP2040_RESETS_RESET_UART0) == 0) ;
            break;

        case 1:
            sil_orw(RP2040_RESETS_RESET, RP2040_RESETS_RESET_UART1);
            sil_clrw(RP2040_RESETS_RESET, RP2040_RESETS_RESET_UART1);
            while ((sil_rew_mem(RP2040_RESETS_RESET_DONE) & RP2040_RESETS_RESET_UART1) == 0) ;
            break;

        default:
            break;
    }

    sil_wrw_mem(RP2040_UART_IMSC(base), 0); /* Disable all interrupts */
    sil_wrw_mem(RP2040_UART_ICR(base), 0x7FF); /* Clear all interrupts */
    const uint64_t divisor = ((((uint64_t)CPU_CLOCK_HZ) << 7) / (16 * BPS_SETTING) + 1) >> 1;
    sil_wrw_mem(RP2040_UART_IBRD(base), divisor >> 6);
    sil_wrw_mem(RP2040_UART_FBRD(base), divisor & 0x3F);
    sil_wrw_mem(RP2040_UART_LCR_H(base), RP2040_UART_LCR_H_WLEN_8BITS /* 8bit word */
                                            | RP2040_UART_LCR_H_FEN); /* Enable FIFO */
    sil_wrw_mem(RP2040_UART_IFLS(base), (0b000 << 3) | (0b100 << 0)); /* Set FIFO level */
    /* Enable UART */
    sil_wrw_mem(RP2040_UART_CR(base), RP2040_UART_CR_RXE | RP2040_UART_CR_TXE | RP2040_UART_CR_UARTEN);

    return &siopcb[index];
}

/*
 * SIOポートのクローズ
 */
void sio_cls_por(SIOPCB *p_siopcb)
{
    /*
     *  UARTをディスエーブル
     */
    sil_wrw_mem(RP2040_UART_CR(p_siopcb->base), 0);
}

/*
 * SIOポートへの文字送信
 */
bool_t sio_snd_chr(SIOPCB *p_siopcb, char c)
{
    if ((sil_rew_mem(RP2040_UART_FR(p_siopcb->base)) & RP2040_UART_FR_TXFF) == 0) {
        sil_wrw_mem(RP2040_UART_DR(p_siopcb->base), c);
        return true;
    }
    return false;
}

/*
 * SIOポートからの文字受信
 */
int_t sio_rcv_chr(SIOPCB *p_siopcb)
{
    if ((sil_rew_mem(RP2040_UART_FR(p_siopcb->base)) & RP2040_UART_FR_RXFE) == 0) {
        return sil_rew_mem(RP2040_UART_DR(p_siopcb->base));
    }
    return -1;
}

/*
 * SIOポートからのコールバックの許可
 */
void sio_ena_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
    switch (cbrtn) {
        case SIO_RDY_SND:
            sil_orw(RP2040_UART_IMSC(p_siopcb->base), RP2040_UART_IMSC_TXIM);
            break;
        case SIO_RDY_RCV:
            sil_orw(RP2040_UART_IMSC(p_siopcb->base), RP2040_UART_IMSC_RTIM | RP2040_UART_IMSC_RXIM);
            break;
    }
}

/*
 * SIOポートからのコールバックの禁止
 */
void sio_dis_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
    switch (cbrtn) {
        case SIO_RDY_SND:
            sil_clrw(RP2040_UART_IMSC(p_siopcb->base), RP2040_UART_IMSC_TXIM);
            break;
        case SIO_RDY_RCV:
            sil_clrw(RP2040_UART_IMSC(p_siopcb->base), RP2040_UART_IMSC_RTIM | RP2040_UART_IMSC_RXIM);
            break;
    }
}

/*
 * SIOポートへの文字出力
 */
void target_fput_log(char c)
{
    if (c == '\n') {
        while (! sio_snd_chr(&siopcb[SIOPID_FPUT - 1], '\r')) ;
    }
    while (! sio_snd_chr(&siopcb[SIOPID_FPUT - 1], c)) ;
}

/*
 * SIOの割込みサービスルーチン
 */
void sio_isr(intptr_t exinf)
{
    const uint32_t stat = sil_rew_mem(RP2040_UART_MIS(siopcb[exinf].base));
    if (stat & RP2040_UART_MIS_TXMIS) {
        /*
         *  送信可能コールバックルーチンを呼び出す．
         */
        sio_irdy_snd(siopcb[exinf].exinf);
    }
    if (stat & (RP2040_UART_MIS_RTIM | RP2040_UART_MIS_RXMIS)) {
        /*
         *  受信通知コールバックルーチンを呼び出す．
         */
        sio_irdy_rcv(siopcb[exinf].exinf);
    }
}
