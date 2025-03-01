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
 *  $Id: wait.c 263 2021-01-08 06:08:59Z ertl-honda $
 */

/*
 *		待ち状態管理モジュール
 */

#include "kernel_impl.h"
#include "taskhook.h"
#include "wait.h"

/*
 *  待ち状態への遷移（タイムアウト指定）
 */
#ifdef TOPPERS_waimake

void
make_wait_tmout(PCB *p_my_pcb, uint_t tstat, TCB *p_selftsk, TMO tmout)
{
	make_wait(p_my_pcb, tstat, p_selftsk);
	if (tmout == TMO_FEVR) {
		p_selftsk->winfo.tmevtb.callback = NULL;
	}
	else {
		assert(tmout <= TMAX_RELTIM);
		p_selftsk->winfo.tmevtb.callback = (CBACK) wait_tmout;
		p_selftsk->winfo.tmevtb.arg = (void *) p_selftsk;
		tmevtb_enqueue_reltim(&(p_selftsk->winfo.tmevtb), (RELTIM) tmout,
						p_my_pcb);
	}
}

#endif /* TOPPERS_waimake */

/*
 *  オブジェクト待ちキューからの削除
 */
#ifdef TOPPERS_waiwobj

void
wait_dequeue_wobj(PCB *p_my_pcb, TCB *p_tcb)
{
	if (TSTAT_WAIT_WOBJ(p_tcb->tstat)) {
		queue_delete(&(p_tcb->task_queue));
	}
}

#endif /* TOPPERS_waiwobj */

/*
 *  待ち解除
 */
#ifdef TOPPERS_waicmp

void
wait_complete(PCB *p_my_pcb, TCB *p_tcb)
{
	wait_dequeue_tmevtb(p_tcb);
	p_tcb->winfo.wercd = E_OK;
	make_non_wait(p_my_pcb, p_tcb);
}

#endif /* TOPPERS_waicmp */

/*
 *  タイムアウトに伴う待ち解除
 */
#ifdef TOPPERS_waitmo

void
wait_tmout(PCB *p_my_pcb, TCB *p_tcb)
{
	wait_dequeue_wobj(p_my_pcb, p_tcb);
	p_tcb->winfo.wercd = E_TMOUT;
	make_non_wait(p_my_pcb, p_tcb);
	if (p_my_pcb->p_runtsk != p_my_pcb->p_schedtsk) {
		if (!sense_context(p_my_pcb)) {
			assert(!p_my_pcb->dspflg);
		}
		else { 
			request_dispatch_retint();
		}
	}

	/*
	 *  ここで優先度の高い割込みを受け付ける．
	 */
	release_glock();
	unlock_cpu();
	delay_for_interrupt();
	lock_cpu();
	acquire_glock();
}

#endif /* TOPPERS_waitmo */

#ifdef TOPPERS_waitmook

void
wait_tmout_ok(PCB *p_my_pcb, TCB *p_tcb)
{
	p_tcb->winfo.wercd = E_OK;
	make_non_wait(p_my_pcb, p_tcb);
	if (p_my_pcb->p_runtsk != p_my_pcb->p_schedtsk) {
		if (!sense_context(p_my_pcb)) {
			assert(!p_my_pcb->dspflg);
		}
		else { 
			request_dispatch_retint();
		}
	}

	/*
	 *  ここで優先度の高い割込みを受け付ける．
	 */
	release_glock();
	unlock_cpu();
	delay_for_interrupt();
	lock_cpu();
	acquire_glock();
}

#endif /* TOPPERS_waitmook */

/*
 *  実行中のタスクの同期・通信オブジェクトの待ちキューへの挿入
 *
 *  実行中のタスクを，同期・通信オブジェクトの待ちキューへ挿入する．オ
 *  ブジェクトの属性に応じて，FIFO順またはタスク優先度順で挿入する．
 */
Inline void
wobj_queue_insert(WOBJCB *p_wobjcb, TCB *p_selftsk)
{
	if ((p_wobjcb->p_wobjinib->wobjatr & TA_TPRI) != 0U) {
		queue_insert_tpri(&(p_wobjcb->wait_queue), p_selftsk);
	}
	else {
		queue_insert_prev(&(p_wobjcb->wait_queue), &(p_selftsk->task_queue));
	}
}

/*
 *  同期・通信オブジェクトに対する待ち状態への遷移
 */
#ifdef TOPPERS_wobjwai

void
wobj_make_wait(PCB *p_my_pcb, WOBJCB *p_wobjcb, uint_t tstat, TCB *p_selftsk)
{
	make_wait(p_my_pcb, tstat, p_selftsk);
	wobj_queue_insert(p_wobjcb, p_selftsk);
	p_selftsk->p_wobjcb = p_wobjcb;
	LOG_TSKSTAT(p_selftsk);
}

#endif /* TOPPERS_wobjwai */
#ifdef TOPPERS_wobjwaitmo

void
wobj_make_wait_tmout(PCB *p_my_pcb, WOBJCB *p_wobjcb,
									uint_t tstat, TCB *p_selftsk, TMO tmout)
{
	make_wait_tmout(p_my_pcb, tstat, p_selftsk, tmout);
	wobj_queue_insert(p_wobjcb, p_selftsk);
	p_selftsk->p_wobjcb = p_wobjcb;
	LOG_TSKSTAT(p_selftsk);
}

#endif /* TOPPERS_wobjwaitmo */

/*
 *  待ちキューの初期化
 */
#ifdef TOPPERS_iniwque

void
init_wait_queue(PCB *p_my_pcb, QUEUE *p_wait_queue)
{
	TCB		*p_tcb;

	while (!queue_empty(p_wait_queue)) {
		p_tcb = (TCB *) queue_delete_next(p_wait_queue);
		wait_dequeue_tmevtb(p_tcb);
		p_tcb->winfo.wercd = E_DLT;
		make_non_wait(p_my_pcb, p_tcb);
	}
}

#endif /* TOPPERS_iniwque */
