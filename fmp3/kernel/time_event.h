/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2020 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: time_event.h 263 2021-01-08 06:08:59Z ertl-honda $
 */

/*
 *		タイムイベント管理モジュール
 */

#ifndef TOPPERS_TIME_EVENT_H
#define TOPPERS_TIME_EVENT_H

#include "kernel_impl.h"
#include "target_timer.h"

/*
 *  境界イベント時刻［ASPD1008］
 */
extern EVTTIM	boundary_evttim;

/*
 *  現在のイベント時刻と境界イベント時刻の差［ASPD1010］
 */
#define BOUNDARY_MARGIN		(200000000U)

/*
 *  最後に現在時刻を算出した時点でのイベント時刻［ASPD1012］
 */
extern EVTTIM	current_evttim;

/*
 *  最後に現在時刻を算出した時点での高分解能タイマのカウント値［ASPD1012］
 */
extern HRTCNT	current_hrtcnt;

/*
 *  最も進んでいた時のイベント時刻［ASPD1041］
 */
extern EVTTIM	monotonic_evttim;

/*
 *  システム時刻のオフセット［ASPD1043］
 *
 *  get_timで参照するシステム時刻とmonotonic_evttimの差を保持する．
 */
extern SYSTIM	systim_offset;

/*
 *  タイムイベントヒープへのアクセステーブル（kernel_cfg.c）
 */
extern TMEVTN* const p_tmevt_heap_table[];

/*
 *  TEVTCB へのアクセステーブル（kernel_cfg.c）
 */
extern TEVTCB* const p_tevtcb_table[];

/*
 *  タイムイベント管理モジュールの初期化
 */
extern void		initialize_tmevt(PCB *p_my_pcb);

/*
 *  タイムイベントの挿入位置の探索
 */
extern TMEVTN	*tmevt_up(TMEVTN *p_tmevtn, EVTTIM evttim,
												TMEVTN *p_tmevt_heap);
extern TMEVTN	*tmevt_down(TMEVTN *p_tmevtn, EVTTIM evttim,
												TMEVTN *p_tmevt_heap);

/*
 *  現在のイベント時刻の更新
 *
 *  current_evttimとcurrent_hrtcntを，現在の値に更新する．
 */
extern void		update_current_evttim(void);

/*
 *  現在のイベント時刻を遅い方に丸めたイベント時刻の算出［ASPD1027］
 *
 *  現在のイベント時刻を更新した後に呼ぶことを想定している．
 */
Inline EVTTIM
calc_current_evttim_ub(void)
{
	return(current_evttim + ((EVTTIM) TSTEP_HRTCNT));
}

/*
 *  高分解能タイマ割込みの発生タイミングの設定
 *
 *  現在のイベント時刻を取得した後に呼び出すことを想定している．
 */
extern void		set_hrt_event(PCB *p_pcb);

/*
 *  高分解能タイマ割込みの発生タイミングの設定（割込みハンドラ版）
 *
 */
extern void		set_hrt_event_handler(void);

/*
 *  タイムイベントブロックのヒープへの挿入
 *
 *  p_tmevtbで指定したタイムイベントブロックを登録する．タイムイベント
 *  の発生時刻，コールバック関数，コールバック関数へ渡す引数は，
 *  p_tmevtbが指すタイムイベントブロック中に設定しておく．
 *
 *  高分解能タイマ割込みの発生タイミングの設定を行わないため，カーネル
 *  の初期化時か，高分解能タイマ割込みの処理中で，それが必要ない場合に
 *  のみ用いることができる．
 */
extern void		tmevtb_register(TMEVTB *p_tmevtb, PCB *p_pcb);

/*
 *  タイムイベントの登録
 *
 *  p_tmevtbで指定したタイムイベントブロックを登録する．タイムイベント
 *  の発生時刻，コールバック関数，コールバック関数へ渡す引数は，
 *  p_tmevtbが指すタイムイベントブロック中に設定しておく．
 */
extern void		tmevtb_enqueue(TMEVTB *p_tmevtb, PCB *p_pcb);

/*
 *  相対時間指定によるタイムイベントの登録
 *
 *  timeで指定した相対時間が経過した後にコールバック関数が呼び出される
 *  ように，p_tmevtbで指定したタイムイベントブロックを登録する．コール
 *  バック関数，コールバック関数へ渡す引数は，p_tmevtbが指すタイムイベ
 *  ントブロック中に設定しておく．
 */
extern void		tmevtb_enqueue_reltim(TMEVTB *p_tmevtb, RELTIM time,
											PCB *p_pcb);

/*
 *  タイムイベントの登録解除
 */
extern void		tmevtb_dequeue(TMEVTB *p_tmevtb, PCB *p_pcb);

/*
 *  システム時刻の調整時のエラーチェック
 *
 *  adjtimで指定された時間の分，システム時刻を調整してよいか判定する．
 *  調整してはならない場合にtrue，そうでない場合にfalseを返す．現在のイ
 *  ベント時刻を取得した後に呼び出すことを想定している．
 */
extern bool_t	check_adjtim(int32_t adjtim, PCB *p_pcb);

/*
 *  タイムイベントが発生するまでの時間の計算
 */
extern RELTIM	tmevt_lefttim(TMEVTB *p_tmevtb);

/*
 *  高分解能タイマ割込みの処理
 */
extern void		signal_time(void);

#endif /* TOPPERS_TIME_EVENT_H */
