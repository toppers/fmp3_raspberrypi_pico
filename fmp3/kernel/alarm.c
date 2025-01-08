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
 *  $Id: alarm.c 263 2021-01-08 06:08:59Z ertl-honda $
 */

/*
 *		アラーム通知機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "alarm.h"
#include "spin_lock.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_ALM_ENTER
#define LOG_ALM_ENTER(p_almcb)
#endif /* LOG_ALM_ENTER */

#ifndef LOG_ALM_LEAVE
#define LOG_ALM_LEAVE(p_almcb)
#endif /* LOG_ALM_LEAVE */

#ifndef LOG_STA_ALM_ENTER
#define LOG_STA_ALM_ENTER(almid, almtim)
#endif /* LOG_STA_ALM_ENTER */

#ifndef LOG_STA_ALM_LEAVE
#define LOG_STA_ALM_LEAVE(ercd)
#endif /* LOG_STA_ALM_LEAVE */

#ifndef LOG_MSTA_ALM_ENTER
#define LOG_MSTA_ALM_ENTER(almid, almtim, prcid)
#endif /* LOG_MSTA_ALM_ENTER */

#ifndef LOG_MSTA_ALM_LEAVE
#define LOG_MSTA_ALM_LEAVE(ercd)
#endif /* LOG_MSTA_ALM_LEAVE */

#ifndef LOG_ALMMIG
#define LOG_ALMMIG(p_almcb, src_id, dest_id)
#endif /* LOG_ALMMIG */

#ifndef LOG_STP_ALM_ENTER
#define LOG_STP_ALM_ENTER(almid)
#endif /* LOG_STP_ALM_ENTER */

#ifndef LOG_STP_ALM_LEAVE
#define LOG_STP_ALM_LEAVE(ercd)
#endif /* LOG_STP_ALM_LEAVE */

#ifndef LOG_REF_ALM_ENTER
#define LOG_REF_ALM_ENTER(almid, pk_ralm)
#endif /* LOG_REF_ALM_ENTER */

#ifndef LOG_REF_ALM_LEAVE
#define LOG_REF_ALM_LEAVE(ercd, pk_ralm)
#endif /* LOG_REF_ALM_LEAVE */

/*
 *  アラーム通知の数
 */
#define tnum_alm	((uint_t)(tmax_almid - TMIN_ALMID + 1))

/*
 *  アラーム通知IDからアラーム通知管理ブロックを取り出すためのマクロ
 */
#define INDEX_ALM(almid)	((uint_t)((almid) - TMIN_ALMID))
#define get_almcb(almid)	(p_almcb_table[INDEX_ALM(almid)])

/*
 *  アラーム通知機能の初期化
 */
#ifdef TOPPERS_almini

void
initialize_alarm(PCB *p_my_pcb)
{
	uint_t	i;
	ALMCB	*p_almcb;

	if (p_my_pcb->p_tevtcb == NULL) {
		return;
	}

	for (i = 0; i < tnum_alm; i++) {
		if (alminib_table[i].iprcid == p_my_pcb->prcid) {
			p_almcb = p_almcb_table[i];
			p_almcb->p_alminib = &(alminib_table[i]);
			p_almcb->almsta = false;
			p_almcb->p_pcb = p_my_pcb;
			p_almcb->tmevtb.callback = (CBACK) call_alarm;
			p_almcb->tmevtb.arg = (void *) p_almcb;
		}
	}
}


#endif /* TOPPERS_almini */

/*
 *  アラーム通知の動作開始
 */
#ifdef TOPPERS_sta_alm

ER
sta_alm(ID almid, RELTIM almtim)
{
	ALMCB	*p_almcb;
	ER		ercd;

	LOG_STA_ALM_ENTER(almid, almtim);
	CHECK_UNL();
	CHECK_ID(VALID_ALMID(almid));
	p_almcb = get_almcb(almid);
	CHECK_PAR(VALID_RELTIM(almtim));

	lock_cpu();
	acquire_glock();
	if (p_almcb->almsta) {
		tmevtb_dequeue(&(p_almcb->tmevtb), p_almcb->p_pcb);
	}
	else {
		p_almcb->almsta = true;
	}
	tmevtb_enqueue_reltim(&(p_almcb->tmevtb), almtim, p_almcb->p_pcb);
	ercd = E_OK;
	release_glock();
	unlock_cpu();

  error_exit:
	LOG_STA_ALM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_sta_alm */

/*
 *  割付けプロセッサ指定でのアラーム通知の動作開始
 */
#ifdef TOPPERS_msta_alm

ER
msta_alm(ID almid, RELTIM almtim, ID prcid)
{
	ALMCB	*p_almcb;
	ER		ercd;

	LOG_MSTA_ALM_ENTER(almid, almtim, prcid);
	CHECK_UNL();
	CHECK_ID(VALID_ALMID(almid));
	p_almcb = get_almcb(almid);
	CHECK_PAR(VALID_RELTIM(almtim));
	if (prcid == TPRC_INI) {
		prcid = p_almcb->p_alminib->iprcid;
	}
	else {
		CHECK_PRCID(prcid);
	}
	CHECK_MIG(p_almcb->p_alminib->affinity, prcid);

	lock_cpu();
	acquire_glock();
	if (p_almcb->almsta) {
		tmevtb_dequeue(&(p_almcb->tmevtb), p_almcb->p_pcb);
	}
	else {
		p_almcb->almsta = true;
	}
	LOG_ALMMIG(p_almcb, p_almcb->p_pcb->prcid, prcid);
	p_almcb->p_pcb = get_pcb(prcid);
	tmevtb_enqueue_reltim(&(p_almcb->tmevtb), almtim, p_almcb->p_pcb);
	ercd = E_OK;
	release_glock();
	unlock_cpu();

  error_exit:
	LOG_MSTA_ALM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_msta_alm */

/*
 *  アラーム通知の動作停止
 */
#ifdef TOPPERS_stp_alm

ER
stp_alm(ID almid)
{
	ALMCB	*p_almcb;
	ER		ercd;

	LOG_STP_ALM_ENTER(almid);
	CHECK_UNL();
	CHECK_ID(VALID_ALMID(almid));
	p_almcb = get_almcb(almid);

	lock_cpu();
	acquire_glock();
	if (p_almcb->almsta) {
		p_almcb->almsta = false;
		tmevtb_dequeue(&(p_almcb->tmevtb), p_almcb->p_pcb);
	}
	ercd = E_OK;
	release_glock();
	unlock_cpu();

  error_exit:
	LOG_STP_ALM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_stp_alm */

/*
 *  アラーム通知の状態参照
 */
#ifdef TOPPERS_ref_alm

ER
ref_alm(ID almid, T_RALM *pk_ralm)
{
	ALMCB	*p_almcb;
	ER		ercd;

	LOG_REF_ALM_ENTER(almid, pk_ralm);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_ALMID(almid));
	p_almcb = get_almcb(almid);

	lock_cpu();
	acquire_glock();
	if (p_almcb->almsta) {
		pk_ralm->almstat = TALM_STA;
		pk_ralm->lefttim = tmevt_lefttim(&(p_almcb->tmevtb));
	}
	else {
		pk_ralm->almstat = TALM_STP;
	}
	pk_ralm->prcid = p_almcb->p_pcb->prcid;
	ercd = E_OK;
	release_glock();
	unlock_cpu();

  error_exit:
	LOG_REF_ALM_LEAVE(ercd, pk_ralm);
	return(ercd);
}

#endif /* TOPPERS_ref_alm */

/*
 *  アラーム通知起動ルーチン
 */
#ifdef TOPPERS_almcal

void
call_alarm(PCB *p_my_pcb, ALMCB *p_almcb)
{
	/*
	 *  アラーム通知を停止状態にする．
	 */
	p_almcb->almsta = false;

	/*
	 *  通知ハンドラを，CPUロック解除状態で呼び出す．
	 */
	release_glock();
	unlock_cpu();

	LOG_ALM_ENTER(p_almcb);
	(*(p_almcb->p_alminib->nfyhdr))(p_almcb->p_alminib->exinf);
	LOG_ALM_LEAVE(p_almcb);

	if (sense_lock()) {
		force_unlock_spin(p_my_pcb);
	}
	else {
		lock_cpu();
	}
	acquire_glock();
}

#endif /* TOPPERS_almcal */
