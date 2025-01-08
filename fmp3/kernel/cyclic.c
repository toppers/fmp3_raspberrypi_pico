/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2021 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: cyclic.c 263 2021-01-08 06:08:59Z ertl-honda $
 */

/*
 *		周期通知機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "cyclic.h"
#include "spin_lock.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_CYC_ENTER
#define LOG_CYC_ENTER(p_cyccb)
#endif /* LOG_CYC_ENTER */

#ifndef LOG_CYC_LEAVE
#define LOG_CYC_LEAVE(p_cyccb)
#endif /* LOG_CYC_LEAVE */

#ifndef LOG_STA_CYC_ENTER
#define LOG_STA_CYC_ENTER(cycid)
#endif /* LOG_STA_CYC_ENTER */

#ifndef LOG_STA_CYC_LEAVE
#define LOG_STA_CYC_LEAVE(ercd)
#endif /* LOG_STA_CYC_LEAVE */

#ifndef LOG_MSTA_CYC_ENTER
#define LOG_MSTA_CYC_ENTER(cycid, prcid)
#endif /* LOG_MSTA_CYC_ENTER */

#ifndef LOG_MSTA_CYC_LEAVE
#define LOG_MSTA_CYC_LEAVE(ercd)
#endif /* LOG_MSTA_CYC_LEAVE */

#ifndef LOG_CYCMIG
#define LOG_CYCMIG(p_cyccb, src_id, dest_id)
#endif /* LOG_CYCMIG */

#ifndef LOG_STP_CYC_ENTER
#define LOG_STP_CYC_ENTER(cycid)
#endif /* LOG_STP_CYC_ENTER */

#ifndef LOG_STP_CYC_LEAVE
#define LOG_STP_CYC_LEAVE(ercd)
#endif /* LOG_STP_CYC_LEAVE */

#ifndef LOG_REF_CYC_ENTER
#define LOG_REF_CYC_ENTER(cycid, pk_rcyc)
#endif /* LOG_REF_CYC_ENTER */

#ifndef LOG_REF_CYC_LEAVE
#define LOG_REF_CYC_LEAVE(ercd, pk_rcyc)
#endif /* LOG_REF_CYC_LEAVE */

/*
 *  周期通知の数
 */
#define tnum_cyc	((uint_t)(tmax_cycid - TMIN_CYCID + 1))

/*
 *  周期通知IDから周期通知管理ブロックを取り出すためのマクロ
 */
#define INDEX_CYC(cycid)	((uint_t)((cycid) - TMIN_CYCID))
#define get_cyccb(cycid)	(p_cyccb_table[INDEX_CYC(cycid)])

/*
 *  周期通知機能の初期化
 */
#ifdef TOPPERS_cycini

void
initialize_cyclic(PCB *p_my_pcb)
{
	uint_t	i;
	CYCCB	*p_cyccb;

	if (p_my_pcb->p_tevtcb == NULL){
		return;
	}

	for (i = 0; i < tnum_cyc; i++) {
		if(cycinib_table[i].iprcid == p_my_pcb->prcid) {
			p_cyccb = p_cyccb_table[i];
			p_cyccb->p_cycinib = &(cycinib_table[i]);
			p_cyccb->tmevtb.callback = (CBACK) call_cyclic;
			p_cyccb->tmevtb.arg = (void *) p_cyccb;
			p_cyccb->p_pcb = p_my_pcb;
			if ((p_cyccb->p_cycinib->cycatr & TA_STA) != 0U) {
				/*
				 *  初回の起動のためのタイムイベントを登録する［ASPD1035］
				 *  ［ASPD1062］．
				 */
				p_cyccb->cycsta = true;
				p_cyccb->tmevtb.evttim = (EVTTIM)(p_cyccb->p_cycinib->cycphs);
				tmevtb_register(&(p_cyccb->tmevtb), p_my_pcb);
			}
			else {
				p_cyccb->cycsta = false;
			}
		}
	}
}

#endif /* TOPPERS_cycini */

/*
 *  周期通知の動作開始
 */
#ifdef TOPPERS_sta_cyc

ER
sta_cyc(ID cycid)
{
	CYCCB	*p_cyccb;
	ER		ercd;

	LOG_STA_CYC_ENTER(cycid);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_CYCID(cycid));
	p_cyccb = get_cyccb(cycid);

	lock_cpu();
	acquire_glock();
	if (p_cyccb->cycsta) {
		tmevtb_dequeue(&(p_cyccb->tmevtb), p_cyccb->p_pcb);
	}
	else {
		p_cyccb->cycsta = true;
	}
	/*
	 *  初回の起動のためのタイムイベントを登録する［ASPD1036］．
	 */
	tmevtb_enqueue_reltim(&(p_cyccb->tmevtb), p_cyccb->p_cycinib->cycphs,
									p_cyccb->p_pcb);
	ercd = E_OK;
	release_glock();
	unlock_cpu();

  error_exit:
	LOG_STA_CYC_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_sta_cyc */

/*
 *  割付けプロセッサ指定での周期通知の動作開始
 */
#ifdef TOPPERS_msta_cyc

ER
msta_cyc(ID cycid, ID prcid)
{
	CYCCB	*p_cyccb;
	ER		ercd;

	LOG_MSTA_CYC_ENTER(cycid, prcid);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_CYCID(cycid));
	p_cyccb = get_cyccb(cycid);
	if (prcid == TPRC_INI) {
		prcid = p_cyccb->p_cycinib->iprcid;
	}
	else {
		CHECK_PRCID(prcid);
	}
	CHECK_MIG(p_cyccb->p_cycinib->affinity, prcid);

	lock_cpu();
	acquire_glock();
	if (p_cyccb->cycsta) {
		tmevtb_dequeue(&(p_cyccb->tmevtb), p_cyccb->p_pcb);
	}
	else {
		p_cyccb->cycsta = true;
	}
	LOG_CYCMIG(p_cyccb, p_cyccb->p_pcb->prcid, prcid);
	p_cyccb->p_pcb = get_pcb(prcid);
	/*
	 *  初回の起動のためのタイムイベントを登録する［ASPD1036］．
	 */
	tmevtb_enqueue_reltim(&(p_cyccb->tmevtb), p_cyccb->p_cycinib->cycphs,
												p_cyccb->p_pcb);
	ercd = E_OK;
	release_glock();
	unlock_cpu();

  error_exit:
	LOG_MSTA_CYC_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_msta_cyc */

/*
 *  周期通知の動作停止
 */
#ifdef TOPPERS_stp_cyc

ER
stp_cyc(ID cycid)
{
	CYCCB	*p_cyccb;
	ER		ercd;

	LOG_STP_CYC_ENTER(cycid);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_CYCID(cycid));
	p_cyccb = get_cyccb(cycid);

	lock_cpu();
	acquire_glock();
	if (p_cyccb->cycsta) {
		p_cyccb->cycsta = false;
		tmevtb_dequeue(&(p_cyccb->tmevtb), p_cyccb->p_pcb);
	}
	ercd = E_OK;
	release_glock();
	unlock_cpu();

  error_exit:
	LOG_STP_CYC_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_stp_cyc */

/*
 *  周期通知の状態参照
 */
#ifdef TOPPERS_ref_cyc

ER
ref_cyc(ID cycid, T_RCYC *pk_rcyc)
{
	CYCCB	*p_cyccb;
	ER		ercd;

	LOG_REF_CYC_ENTER(cycid, pk_rcyc);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_CYCID(cycid));
	p_cyccb = get_cyccb(cycid);

	lock_cpu();
	acquire_glock();
	if (p_cyccb->cycsta) {
		pk_rcyc->cycstat = TCYC_STA;
		pk_rcyc->lefttim = tmevt_lefttim(&(p_cyccb->tmevtb));
	}
	else {
		pk_rcyc->cycstat = TCYC_STP;
	}
	pk_rcyc->prcid = p_cyccb->p_pcb->prcid;
	ercd = E_OK;
	release_glock();
	unlock_cpu();

  error_exit:
	LOG_REF_CYC_LEAVE(ercd, pk_rcyc);
	return(ercd);
}

#endif /* TOPPERS_ref_cyc */

/*
 *  周期通知起動ルーチン
 */
#ifdef TOPPERS_cyccal

void
call_cyclic(PCB *p_my_pcb, CYCCB *p_cyccb)
{
	/*
	 *  次回の起動のためのタイムイベントを登録する［ASPD1037］．
	 *
	 *  tmevtb_enqueueを用いるのが素直であるが，この関数は高分解能タイ
	 *  マ割込みの処理中でのみ呼び出されるため，tmevtb_registerを用い
	 *  ている．
	 */
	p_cyccb->tmevtb.evttim += p_cyccb->p_cycinib->cyctim;	/*［ASPD1038］*/
	tmevtb_register(&(p_cyccb->tmevtb), p_cyccb->p_pcb);

	/*
	 *  通知ハンドラを，CPUロック解除状態で呼び出す．
	 */
	release_glock();
	unlock_cpu();

	LOG_CYC_ENTER(p_cyccb);
	(*(p_cyccb->p_cycinib->nfyhdr))(p_cyccb->p_cycinib->exinf);
	LOG_CYC_LEAVE(p_cyccb);

	if (sense_lock()) {
		force_unlock_spin(p_my_pcb);
	}
	else {
		lock_cpu();
	}
	acquire_glock();
}

#endif /* TOPPERS_cyccal */
