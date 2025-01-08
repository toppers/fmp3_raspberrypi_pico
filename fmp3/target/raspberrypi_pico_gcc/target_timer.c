/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2016 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: target_timer.c 292 2021-10-11 12:27:17Z ertl-komori $
 */

/*
 * タイマドライバ（RaspberryPi Pico用）
 */

#include "kernel_impl.h"
#include "time_event.h"
#include "target_timer.h"
#include <sil.h>

/*
 * タイマの起動処理
 */
void target_hrt_initialize(intptr_t exinf)
{
    if (ID_PRC(get_my_prcidx()) == PRC1) {
        /* 12MHz XTAL / 12 -> 1MHz */
        sil_wrw_mem(RP2040_WATCHDOG_TICK, RP2040_WATCHDOG_ENABLE | 12); /* According to pico-sdk, not 12 - 1 */
        while ((sil_rew_mem(RP2040_WATCHDOG_TICK) & RP2040_WATCHDOG_RUNNING) == 0) ;
        /* Reset timer */
        sil_orw(RP2040_RESETS_RESET, RP2040_RESETS_RESET_TIMER);
        sil_clrw(RP2040_RESETS_RESET, RP2040_RESETS_RESET_TIMER);
        while ((sil_rew_mem(RP2040_RESETS_RESET_DONE) & RP2040_RESETS_RESET_TIMER) == 0) ;
        sil_orw(RP2040_TIMER_INTE, RP2040_TIMER_INT_ALARM_0); /* Enable interrupt */
    } else {
        sil_orw(RP2040_TIMER_INTE, RP2040_TIMER_INT_ALARM_1); /* Enable interrupt */
    }
    while (target_hrt_get_current() < 1) ; /* Timer counter may not start immediately */
}

/*
 * タイマの停止処理
 */
void target_hrt_terminate(intptr_t exinf)
{
    sil_orw(RP2040_RESETS_RESET, RP2040_RESETS_RESET_TIMER); /* Set reset */
}

/*
 *  タイマ割込みハンドラ
 */
void target_hrt_handler_prc1(void)
{
    sil_wrw_mem(RP2040_TIMER_INTR, RP2040_TIMER_INT_ALARM_0); /* Clear int flag */
    signal_time();
}

void target_hrt_handler_prc2(void)
{
    sil_wrw_mem(RP2040_TIMER_INTR, RP2040_TIMER_INT_ALARM_1); /* Clear int flag */
    signal_time();
}
