/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2019 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: pridataq.c 263 2021-01-08 06:08:59Z ertl-honda $
 */

/*
 *		優先度データキュー機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "wait.h"
#include "pridataq.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_SND_PDQ_ENTER
#define LOG_SND_PDQ_ENTER(pdqid, data, datapri)
#endif /* LOG_SND_PDQ_ENTER */

#ifndef LOG_SND_PDQ_LEAVE
#define LOG_SND_PDQ_LEAVE(ercd)
#endif /* LOG_SND_PDQ_LEAVE */

#ifndef LOG_PSND_PDQ_ENTER
#define LOG_PSND_PDQ_ENTER(pdqid, data, datapri)
#endif /* LOG_PSND_PDQ_ENTER */

#ifndef LOG_PSND_PDQ_LEAVE
#define LOG_PSND_PDQ_LEAVE(ercd)
#endif /* LOG_PSND_PDQ_LEAVE */

#ifndef LOG_TSND_PDQ_ENTER
#define LOG_TSND_PDQ_ENTER(pdqid, data, datapri, tmout)
#endif /* LOG_TSND_PDQ_ENTER */

#ifndef LOG_TSND_PDQ_LEAVE
#define LOG_TSND_PDQ_LEAVE(ercd)
#endif /* LOG_TSND_PDQ_LEAVE */

#ifndef LOG_RCV_PDQ_ENTER
#define LOG_RCV_PDQ_ENTER(pdqid, p_data, p_datapri)
#endif /* LOG_RCV_PDQ_ENTER */

#ifndef LOG_RCV_PDQ_LEAVE
#define LOG_RCV_PDQ_LEAVE(ercd, p_data, p_datapri)
#endif /* LOG_RCV_PDQ_LEAVE */

#ifndef LOG_PRCV_PDQ_ENTER
#define LOG_PRCV_PDQ_ENTER(pdqid, p_data, p_datapri)
#endif /* LOG_PRCV_PDQ_ENTER */

#ifndef LOG_PRCV_PDQ_LEAVE
#define LOG_PRCV_PDQ_LEAVE(ercd, p_data, p_datapri)
#endif /* LOG_PRCV_PDQ_LEAVE */

#ifndef LOG_TRCV_PDQ_ENTER
#define LOG_TRCV_PDQ_ENTER(pdqid, p_data, p_datapri, tmout)
#endif /* LOG_TRCV_PDQ_ENTER */

#ifndef LOG_TRCV_PDQ_LEAVE
#define LOG_TRCV_PDQ_LEAVE(ercd, p_data, p_datapri)
#endif /* LOG_TRCV_PDQ_LEAVE */

#ifndef LOG_INI_PDQ_ENTER
#define LOG_INI_PDQ_ENTER(pdqid)
#endif /* LOG_INI_PDQ_ENTER */

#ifndef LOG_INI_PDQ_LEAVE
#define LOG_INI_PDQ_LEAVE(ercd)
#endif /* LOG_INI_PDQ_LEAVE */

#ifndef LOG_REF_PDQ_ENTER
#define LOG_REF_PDQ_ENTER(pdqid, pk_rpdq)
#endif /* LOG_REF_PDQ_ENTER */

#ifndef LOG_REF_PDQ_LEAVE
#define LOG_REF_PDQ_LEAVE(ercd, pk_rpdq)
#endif /* LOG_REF_PDQ_LEAVE */

/*
 *  優先度データキューの数
 */
#define tnum_pdq	((uint_t)(tmax_pdqid - TMIN_PDQID + 1))

/*
 *  優先度データキューIDから優先度データキュー管理ブロックを取り出すた
 *  めのマクロ
 */
#define INDEX_PDQ(pdqid)	((uint_t)((pdqid) - TMIN_PDQID))
#define get_pdqcb(pdqid)	(p_pdqcb_table[INDEX_PDQ(pdqid)])

/*
 *  優先度データキュー機能の初期化
 */
#ifdef TOPPERS_pdqini

void
initialize_pridataq(PCB *p_my_pcb)
{
	uint_t	i;
	PDQCB	*p_pdqcb;

	if (p_my_pcb->prcid == TOPPERS_MASTER_PRCID) {
		for (i = 0; i < tnum_pdq; i++) {
			p_pdqcb = p_pdqcb_table[i];
			queue_initialize(&(p_pdqcb->swait_queue));
			p_pdqcb->p_pdqinib = &(pdqinib_table[i]);
			queue_initialize(&(p_pdqcb->rwait_queue));
			p_pdqcb->count = 0U;
			p_pdqcb->p_head = NULL;
			p_pdqcb->unused = 0U;
			p_pdqcb->p_freelist = NULL;
		}
	}
}

#endif /* TOPPERS_pdqini */

/*
 *  優先度データキュー管理領域へのデータの格納
 */
#ifdef TOPPERS_pdqenq

void
enqueue_pridata(PDQCB *p_pdqcb, intptr_t data, PRI datapri)
{
	PDQMB	*p_pdqmb;
	PDQMB	**pp_prev_next, *p_next;

	if (p_pdqcb->p_freelist != NULL) {
		p_pdqmb = p_pdqcb->p_freelist;
		p_pdqcb->p_freelist = p_pdqmb->p_next;
	}
	else {
		p_pdqmb = p_pdqcb->p_pdqinib->p_pdqmb + p_pdqcb->unused;
		p_pdqcb->unused++;
	}

	p_pdqmb->data = data;
	p_pdqmb->datapri = datapri;

	pp_prev_next = &(p_pdqcb->p_head);
	while ((p_next = *pp_prev_next) != NULL) {
		if (p_next->datapri > datapri) {
			break;
		}
		pp_prev_next = &(p_next->p_next);
	}
	p_pdqmb->p_next = p_next;
	*pp_prev_next = p_pdqmb;
	p_pdqcb->count++;
}

#endif /* TOPPERS_pdqenq */

/*
 *  優先度データキュー管理領域からのデータの取出し
 */
#ifdef TOPPERS_pdqdeq

void
dequeue_pridata(PDQCB *p_pdqcb, intptr_t *p_data, PRI *p_datapri)
{
	PDQMB	*p_pdqmb;

	p_pdqmb = p_pdqcb->p_head;
	p_pdqcb->p_head = p_pdqmb->p_next;
	p_pdqcb->count--;

	*p_data = p_pdqmb->data;
	*p_datapri = p_pdqmb->datapri;

	p_pdqmb->p_next = p_pdqcb->p_freelist;
	p_pdqcb->p_freelist = p_pdqmb;
}

#endif /* TOPPERS_pdqdeq */

/*
 *  優先度データキューへのデータ送信
 */
#ifdef TOPPERS_pdqsnd

bool_t
send_pridata(PCB *p_my_pcb, PDQCB *p_pdqcb, intptr_t data, PRI datapri)
{
	TCB		*p_tcb;

	if (!queue_empty(&(p_pdqcb->rwait_queue))) {
		p_tcb = (TCB *) queue_delete_next(&(p_pdqcb->rwait_queue));
		p_tcb->winfo_obj.rpdq.data = data;
		p_tcb->winfo_obj.rpdq.datapri = datapri;
		wait_complete(p_my_pcb, p_tcb);
		return(true);
	}
	else if (p_pdqcb->count < p_pdqcb->p_pdqinib->pdqcnt) {
		enqueue_pridata(p_pdqcb, data, datapri);
		return(true);
	}
	else {
		return(false);
	}
}

#endif /* TOPPERS_pdqsnd */

/*
 *  優先度データキューからのデータ受信
 */
#ifdef TOPPERS_pdqrcv

bool_t
receive_pridata(PCB *p_my_pcb, PDQCB *p_pdqcb, intptr_t *p_data, PRI *p_datapri)
{
	TCB		*p_tcb;
	intptr_t data;
	PRI		datapri;

	if (p_pdqcb->count > 0U) {
		dequeue_pridata(p_pdqcb, p_data, p_datapri);
		if (!queue_empty(&(p_pdqcb->swait_queue))) {
			p_tcb = (TCB *) queue_delete_next(&(p_pdqcb->swait_queue));
			data = p_tcb->winfo_obj.spdq.data;
			datapri = p_tcb->winfo_obj.spdq.datapri;
			enqueue_pridata(p_pdqcb, data, datapri);
			wait_complete(p_my_pcb, p_tcb);
		}
		return(true);
	}
	else if (!queue_empty(&(p_pdqcb->swait_queue))) {
		p_tcb = (TCB *) queue_delete_next(&(p_pdqcb->swait_queue));
		*p_data = p_tcb->winfo_obj.spdq.data;
		*p_datapri = p_tcb->winfo_obj.spdq.datapri;
		wait_complete(p_my_pcb, p_tcb);
		return(true);
	}
	else {
		return(false);
	}
}

#endif /* TOPPERS_pdqrcv */

/*
 *  優先度データキューへの送信
 */
#ifdef TOPPERS_snd_pdq

ER
snd_pdq(ID pdqid, intptr_t data, PRI datapri)
{
	PDQCB	*p_pdqcb;
	ER		ercd;
	TCB		*p_selftsk;
	PCB		*p_my_pcb;

	LOG_SND_PDQ_ENTER(pdqid, data, datapri);
	CHECK_DISPATCH_MYSTATE(&p_selftsk);
	CHECK_ID(VALID_PDQID(pdqid));
	p_pdqcb = get_pdqcb(pdqid);
	CHECK_PAR(TMIN_DPRI <= datapri && datapri <= p_pdqcb->p_pdqinib->maxdpri);

	lock_cpu_dsp();
	acquire_glock();
	p_my_pcb = get_my_pcb();
	if (p_selftsk->raster) {
		ercd = E_RASTER;
	}
	else if (send_pridata(p_my_pcb, p_pdqcb, data, datapri)) {
		if (p_selftsk != p_my_pcb->p_schedtsk) {
			release_glock();
			dispatch();
			ercd = E_OK;
			goto unlock_and_exit;
		}
		ercd = E_OK;
	}
	else {
		p_selftsk->winfo_obj.spdq.data = data;
		p_selftsk->winfo_obj.spdq.datapri = datapri;
		wobj_make_wait(p_my_pcb, (WOBJCB *) p_pdqcb, TS_WAITING_SPDQ,
															p_selftsk);
		release_glock();
		dispatch();
		ercd = p_selftsk->winfo.wercd;
		goto unlock_and_exit;
	}
	release_glock();
  unlock_and_exit:
	unlock_cpu_dsp();

  error_exit:
	LOG_SND_PDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_snd_pdq */

/*
 *  優先度データキューへの送信（ポーリング）
 */
#ifdef TOPPERS_psnd_pdq

ER
psnd_pdq(ID pdqid, intptr_t data, PRI datapri)
{
	PDQCB	*p_pdqcb;
	ER		ercd;
	TCB		*p_selftsk;
	bool_t	context;
	PCB		*p_my_pcb;

	LOG_PSND_PDQ_ENTER(pdqid, data, datapri);
	CHECK_UNL_MYSTATE(&p_selftsk, &context);
	CHECK_ID(VALID_PDQID(pdqid));
	p_pdqcb = get_pdqcb(pdqid);
	CHECK_PAR(TMIN_DPRI <= datapri && datapri <= p_pdqcb->p_pdqinib->maxdpri);

	lock_cpu();
	acquire_glock();
	p_my_pcb = get_my_pcb();
	if (send_pridata(p_my_pcb, p_pdqcb, data, datapri)) {
		if (p_selftsk != p_my_pcb->p_schedtsk) {
			if (!context) {
				release_glock();
				dispatch();
				ercd = E_OK;
				goto unlock_and_exit;
			}
			else {
				request_dispatch_retint();
			}
		}
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	release_glock();
  unlock_and_exit:
	unlock_cpu();

  error_exit:
	LOG_PSND_PDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_psnd_pdq */

/*
 *  優先度データキューへの送信（タイムアウトあり）
 */
#ifdef TOPPERS_tsnd_pdq

ER
tsnd_pdq(ID pdqid, intptr_t data, PRI datapri, TMO tmout)
{
	PDQCB	*p_pdqcb;
	ER		ercd;
	TCB		*p_selftsk;
	PCB		*p_my_pcb;

	LOG_TSND_PDQ_ENTER(pdqid, data, datapri, tmout);
	CHECK_DISPATCH_MYSTATE(&p_selftsk);
	CHECK_ID(VALID_PDQID(pdqid));
	CHECK_PAR(VALID_TMOUT(tmout));
	p_pdqcb = get_pdqcb(pdqid);
	CHECK_PAR(TMIN_DPRI <= datapri && datapri <= p_pdqcb->p_pdqinib->maxdpri);

	lock_cpu_dsp();
	acquire_glock();
	p_my_pcb = get_my_pcb();
	if (p_selftsk->raster) {
		ercd = E_RASTER;
	}
	else if (send_pridata(p_my_pcb, p_pdqcb, data, datapri)) {
		if (p_selftsk != p_my_pcb->p_schedtsk) {
			release_glock();
			dispatch();
			ercd = E_OK;
			goto unlock_and_exit;
		}
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		p_selftsk->winfo_obj.spdq.data = data;
		p_selftsk->winfo_obj.spdq.datapri = datapri;
		wobj_make_wait_tmout(p_my_pcb, (WOBJCB *) p_pdqcb, TS_WAITING_SPDQ,
															p_selftsk, tmout);
		release_glock();
		dispatch();
		ercd = p_selftsk->winfo.wercd;
		goto unlock_and_exit;
	}
	release_glock();
  unlock_and_exit:
	unlock_cpu_dsp();

  error_exit:
	LOG_TSND_PDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_tsnd_pdq */

/*
 *  優先度データキューからの受信
 */
#ifdef TOPPERS_rcv_pdq

ER
rcv_pdq(ID pdqid, intptr_t *p_data, PRI *p_datapri)
{
	PDQCB	*p_pdqcb;
	ER		ercd;
	TCB		*p_selftsk;
	PCB		*p_my_pcb;

	LOG_RCV_PDQ_ENTER(pdqid, p_data, p_datapri);
	CHECK_DISPATCH_MYSTATE(&p_selftsk);
	CHECK_ID(VALID_PDQID(pdqid));
	p_pdqcb = get_pdqcb(pdqid);

	lock_cpu_dsp();
	acquire_glock();
	p_my_pcb = get_my_pcb();
	if (p_selftsk->raster) {
		ercd = E_RASTER;
	}
	else if (receive_pridata(p_my_pcb, p_pdqcb, p_data, p_datapri)) {
		if (p_selftsk != p_my_pcb->p_schedtsk) {
			release_glock();
			dispatch();
			ercd = E_OK;
			goto unlock_and_exit;
		}
		ercd = E_OK;
	}
	else {
		make_wait(p_my_pcb, TS_WAITING_RPDQ, p_selftsk);
		queue_insert_prev(&(p_pdqcb->rwait_queue), &(p_selftsk->task_queue));
		p_selftsk->p_wobjcb = (WOBJCB *) p_pdqcb;
		LOG_TSKSTAT(p_selftsk);
		release_glock();
		dispatch();
		ercd = p_selftsk->winfo.wercd;
		if (ercd == E_OK) {
			*p_data = p_selftsk->winfo_obj.rpdq.data;
			*p_datapri = p_selftsk->winfo_obj.rpdq.datapri;
		}
		goto unlock_and_exit;
	}
	release_glock();
  unlock_and_exit:
	unlock_cpu_dsp();

  error_exit:
	LOG_RCV_PDQ_LEAVE(ercd, p_data, p_datapri);
	return(ercd);
}

#endif /* TOPPERS_rcv_pdq */

/*
 *  優先度データキューからの受信（ポーリング）
 */
#ifdef TOPPERS_prcv_pdq

ER
prcv_pdq(ID pdqid, intptr_t *p_data, PRI *p_datapri)
{
	PDQCB	*p_pdqcb;
	ER		ercd;
	TCB		*p_selftsk;
	PCB		*p_my_pcb;

	LOG_PRCV_PDQ_ENTER(pdqid, p_data, p_datapri);
	CHECK_TSKCTX_UNL_MYSTATE(&p_selftsk);
	CHECK_ID(VALID_PDQID(pdqid));
	p_pdqcb = get_pdqcb(pdqid);

	lock_cpu();
	acquire_glock();
	p_my_pcb = get_my_pcb();
	if (receive_pridata(p_my_pcb, p_pdqcb, p_data, p_datapri)) {
		if (p_selftsk != p_my_pcb->p_schedtsk) {
			release_glock();
			dispatch();
			ercd = E_OK;
			goto unlock_and_exit;
		}
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	release_glock();
  unlock_and_exit:
	unlock_cpu();

  error_exit:
	LOG_PRCV_PDQ_LEAVE(ercd, p_data, p_datapri);
	return(ercd);
}

#endif /* TOPPERS_prcv_pdq */

/*
 *  優先度データキューからの受信（タイムアウトあり）
 */
#ifdef TOPPERS_trcv_pdq

ER
trcv_pdq(ID pdqid, intptr_t *p_data, PRI *p_datapri, TMO tmout)
{
	PDQCB	*p_pdqcb;
	ER		ercd;
	TCB		*p_selftsk;
	PCB		*p_my_pcb;

	LOG_TRCV_PDQ_ENTER(pdqid, p_data, p_datapri, tmout);
	CHECK_DISPATCH_MYSTATE(&p_selftsk);
	CHECK_ID(VALID_PDQID(pdqid));
	CHECK_PAR(VALID_TMOUT(tmout));
	p_pdqcb = get_pdqcb(pdqid);

	lock_cpu_dsp();
	acquire_glock();
	p_my_pcb = get_my_pcb();
	if (p_selftsk->raster) {
		ercd = E_RASTER;
	}
	else if (receive_pridata(p_my_pcb, p_pdqcb, p_data, p_datapri)) {
		if (p_selftsk != p_my_pcb->p_schedtsk) {
			release_glock();
			dispatch();
			ercd = E_OK;
			goto unlock_and_exit;
		}
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		make_wait_tmout(p_my_pcb, TS_WAITING_RPDQ, p_selftsk, tmout);
		queue_insert_prev(&(p_pdqcb->rwait_queue), &(p_selftsk->task_queue));
		p_selftsk->p_wobjcb = (WOBJCB *) p_pdqcb;
		LOG_TSKSTAT(p_selftsk);
		release_glock();
		dispatch();
		ercd = p_selftsk->winfo.wercd;
		if (ercd == E_OK) {
			*p_data = p_selftsk->winfo_obj.rpdq.data;
			*p_datapri = p_selftsk->winfo_obj.rpdq.datapri;
		}
		goto unlock_and_exit;
	}
	release_glock();
  unlock_and_exit:
	unlock_cpu_dsp();

  error_exit:
	LOG_TRCV_PDQ_LEAVE(ercd, p_data, p_datapri);
	return(ercd);
}

#endif /* TOPPERS_trcv_pdq */

/*
 *  優先度データキューの再初期化
 */
#ifdef TOPPERS_ini_pdq

ER
ini_pdq(ID pdqid)
{
	PDQCB	*p_pdqcb;
	ER		ercd;
	TCB		*p_selftsk;
	PCB		*p_my_pcb;

	LOG_INI_PDQ_ENTER(pdqid);
	CHECK_TSKCTX_UNL_MYSTATE(&p_selftsk);
	CHECK_ID(VALID_PDQID(pdqid));
	p_pdqcb = get_pdqcb(pdqid);

	lock_cpu();
	acquire_glock();
	p_my_pcb = get_my_pcb();
	init_wait_queue(p_my_pcb, &(p_pdqcb->swait_queue));
	init_wait_queue(p_my_pcb, &(p_pdqcb->rwait_queue));
	p_pdqcb->count = 0U;
	p_pdqcb->p_head = NULL;
	p_pdqcb->unused = 0U;
	p_pdqcb->p_freelist = NULL;
	ercd = E_OK;
	if (p_selftsk != p_my_pcb->p_schedtsk) {
		release_glock();
		dispatch();
		goto unlock_and_exit;
	}
	release_glock();
  unlock_and_exit:
	unlock_cpu();

  error_exit:
	LOG_INI_PDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ini_pdq */

/*
 *  優先度データキューの状態参照
 */
#ifdef TOPPERS_ref_pdq

ER
ref_pdq(ID pdqid, T_RPDQ *pk_rpdq)
{
	PDQCB	*p_pdqcb;
	ER		ercd;

	LOG_REF_PDQ_ENTER(pdqid, pk_rpdq);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_PDQID(pdqid));
	p_pdqcb = get_pdqcb(pdqid);

	lock_cpu();
	acquire_glock();
	pk_rpdq->stskid = wait_tskid(&(p_pdqcb->swait_queue));
	pk_rpdq->rtskid = wait_tskid(&(p_pdqcb->rwait_queue));
	pk_rpdq->spdqcnt = p_pdqcb->count;
	ercd = E_OK;
	release_glock();
	unlock_cpu();

  error_exit:
	LOG_REF_PDQ_LEAVE(ercd, pk_rpdq);
	return(ercd);
}

#endif /* TOPPERS_ref_pdq */
