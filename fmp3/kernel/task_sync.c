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
 *  $Id: task_sync.c 263 2021-01-08 06:08:59Z ertl-honda $
 */

/*
 *		タスク付属同期機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "wait.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_SLP_TSK_ENTER
#define LOG_SLP_TSK_ENTER()
#endif /* LOG_SLP_TSK_ENTER */

#ifndef LOG_SLP_TSK_LEAVE
#define LOG_SLP_TSK_LEAVE(ercd)
#endif /* LOG_SLP_TSK_LEAVE */

#ifndef LOG_TSLP_TSK_ENTER
#define LOG_TSLP_TSK_ENTER(tmout)
#endif /* LOG_TSLP_TSK_ENTER */

#ifndef LOG_TSLP_TSK_LEAVE
#define LOG_TSLP_TSK_LEAVE(ercd)
#endif /* LOG_TSLP_TSK_LEAVE */

#ifndef LOG_WUP_TSK_ENTER
#define LOG_WUP_TSK_ENTER(tskid)
#endif /* LOG_WUP_TSK_ENTER */

#ifndef LOG_WUP_TSK_LEAVE
#define LOG_WUP_TSK_LEAVE(ercd)
#endif /* LOG_WUP_TSK_LEAVE */

#ifndef LOG_CAN_WUP_ENTER
#define LOG_CAN_WUP_ENTER(tskid)
#endif /* LOG_CAN_WUP_ENTER */

#ifndef LOG_CAN_WUP_LEAVE
#define LOG_CAN_WUP_LEAVE(ercd)
#endif /* LOG_CAN_WUP_LEAVE */

#ifndef LOG_REL_WAI_ENTER
#define LOG_REL_WAI_ENTER(tskid)
#endif /* LOG_REL_WAI_ENTER */

#ifndef LOG_REL_WAI_LEAVE
#define LOG_REL_WAI_LEAVE(ercd)
#endif /* LOG_REL_WAI_LEAVE */

#ifndef LOG_SUS_TSK_ENTER
#define LOG_SUS_TSK_ENTER(tskid)
#endif /* LOG_SUS_TSK_ENTER */

#ifndef LOG_SUS_TSK_LEAVE
#define LOG_SUS_TSK_LEAVE(ercd)
#endif /* LOG_SUS_TSK_LEAVE */

#ifndef LOG_RSM_TSK_ENTER
#define LOG_RSM_TSK_ENTER(tskid)
#endif /* LOG_RSM_TSK_ENTER */

#ifndef LOG_RSM_TSK_LEAVE
#define LOG_RSM_TSK_LEAVE(ercd)
#endif /* LOG_RSM_TSK_LEAVE */

#ifndef LOG_DLY_TSK_ENTER
#define LOG_DLY_TSK_ENTER(dlytim)
#endif /* LOG_DLY_TSK_ENTER */

#ifndef LOG_DLY_TSK_LEAVE
#define LOG_DLY_TSK_LEAVE(ercd)
#endif /* LOG_DLY_TSK_LEAVE */

/*
 *  起床待ち［NGKI1252］
 */
#ifdef TOPPERS_slp_tsk

ER
slp_tsk(void)
{
	ER		ercd;
	TCB		*p_selftsk;
	PCB		*p_my_pcb;

	LOG_SLP_TSK_ENTER();
	CHECK_DISPATCH();							/*［NGKI1254］*/

	lock_cpu_dsp();
	acquire_glock();
	p_my_pcb = get_my_pcb();
	p_selftsk = p_my_pcb->p_runtsk;
	if (p_selftsk->raster) {
		ercd = E_RASTER;						/*［NGKI3455］*/
	}
	else if (p_selftsk->wupque) {
		p_selftsk->wupque = false;				/*［NGKI1259］*/
		ercd = E_OK;
	}
	else {
		make_wait(p_my_pcb, TS_WAITING_SLP, p_selftsk);		/*［NGKI1260］*/
		LOG_TSKSTAT(p_selftsk);
		release_glock();
		dispatch();
		ercd = p_selftsk->winfo.wercd;
		goto unlock_and_exit;
	}
	release_glock();
  unlock_and_exit:
	unlock_cpu_dsp();

  error_exit:
	LOG_SLP_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_slp_tsk */

/*
 *  起床待ち（タイムアウトあり）［NGKI1253］
 */
#ifdef TOPPERS_tslp_tsk

ER
tslp_tsk(TMO tmout)
{
	ER		ercd;
	TCB		*p_selftsk;
	PCB		*p_my_pcb;

	LOG_TSLP_TSK_ENTER(tmout);
	CHECK_DISPATCH();							/*［NGKI1254］*/
	CHECK_PAR(VALID_TMOUT(tmout));				/*［NGKI1256］*/

	lock_cpu_dsp();
	acquire_glock();
	p_my_pcb = get_my_pcb();
	p_selftsk = p_my_pcb->p_runtsk;
	if (p_selftsk->raster) {
		ercd = E_RASTER;						/*［NGKI3455］*/
	}
	else if (p_selftsk->wupque) {
		p_selftsk->wupque = false;				/*［NGKI1259］*/
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;							/*［NGKI1257］*/
	}
	else {
		make_wait_tmout(p_my_pcb, TS_WAITING_SLP, p_selftsk, tmout);
		LOG_TSKSTAT(p_selftsk);					/*［NGKI1260］*/
		release_glock();
		dispatch();
		ercd = p_selftsk->winfo.wercd;
		goto unlock_and_exit;
	}
	release_glock();
  unlock_and_exit:
	unlock_cpu_dsp();

  error_exit:
	LOG_TSLP_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_tslp_tsk */

/*
 *  タスクの起床［NGKI3531］
 */
#ifdef TOPPERS_wup_tsk

ER
wup_tsk(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;
	TCB		*p_selftsk;
	bool_t	context;
	PCB		*p_my_pcb;

	LOG_WUP_TSK_ENTER(tskid);
	CHECK_UNL_MYSTATE(&p_selftsk, &context);		/*［NGKI1265］*/
	if (tskid == TSK_SELF && !context) {
		p_tcb = p_selftsk;							/*［NGKI1275］*/
	}
	else {
		CHECK_ID(VALID_TSKID(tskid));			/*［NGKI1267］*/
		p_tcb = get_tcb(tskid);
	}

	lock_cpu();
	acquire_glock();
	p_my_pcb = get_my_pcb();
	if (TSTAT_DORMANT(p_tcb->tstat)) {
		ercd = E_OBJ;							/*［NGKI1270］*/
	}
	else if (TSTAT_WAIT_SLP(p_tcb->tstat)) {
		wait_complete(p_my_pcb, p_tcb);			/*［NGKI1271］*/
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
	else if (!(p_tcb->wupque)) {
		p_tcb->wupque = true;					/*［NGKI1273］*/
		ercd = E_OK;
	}
	else {
		ercd = E_QOVR;							/*［NGKI1274］*/
	}
	release_glock();
  unlock_and_exit:
	unlock_cpu();

  error_exit:
	LOG_WUP_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_wup_tsk */

/*
 *  タスク起床要求のキャンセル［NGKI1276］
 */
#ifdef TOPPERS_can_wup

ER_UINT
can_wup(ID tskid)
{
	TCB		*p_tcb;
	ER_UINT	ercd;
	TCB		*p_selftsk;

	LOG_CAN_WUP_ENTER(tskid);
	CHECK_TSKCTX_UNL_MYSTATE(&p_selftsk);		/*［NGKI1277］［NGKI1278］*/
	if (tskid == TSK_SELF) {
		p_tcb = p_selftsk;						/*［NGKI1285］*/
	}
	else {
		CHECK_ID(VALID_TSKID(tskid));			/*［NGKI1280］*/
		p_tcb = get_tcb(tskid);
	}

	lock_cpu();
	acquire_glock();
	if (TSTAT_DORMANT(p_tcb->tstat)) {
		ercd = E_OBJ;							/*［NGKI1283］*/
	}
	else {
		ercd = p_tcb->wupque ? 1 : 0;			/*［NGKI1284］*/
		p_tcb->wupque = false;					/*［NGKI1284］*/
	}
	release_glock();
	unlock_cpu();

  error_exit:
	LOG_CAN_WUP_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_can_wup */

/*
 *  待ち状態の強制解除［NGKI3532］
 */
#ifdef TOPPERS_rel_wai

ER
rel_wai(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;
	TCB		*p_selftsk;
	bool_t	context;
	PCB		*p_my_pcb;

	LOG_REL_WAI_ENTER(tskid);
	CHECK_UNL_MYSTATE(&p_selftsk, &context);	/*［NGKI1290］*/
	CHECK_ID(VALID_TSKID(tskid));				/*［NGKI1292］*/
	p_tcb = get_tcb(tskid);

	lock_cpu();
	acquire_glock();
	p_my_pcb = get_my_pcb();
	if (!TSTAT_WAITING(p_tcb->tstat)) {
		ercd = E_OBJ;							/*［NGKI1295］*/
	}
	else {
		wait_dequeue_wobj(p_my_pcb, p_tcb);		/*［NGKI1296］*/
		wait_dequeue_tmevtb(p_tcb);
		p_tcb->winfo.wercd = E_RLWAI;			/*［NGKI1297］*/
		make_non_wait(p_my_pcb, p_tcb);
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
	release_glock();
  unlock_and_exit:
	unlock_cpu();

  error_exit:
	LOG_REL_WAI_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_rel_wai */

/*
 *  強制待ち状態への移行［NGKI1298］
 *
 *  自タスクが対象でない場合は相手のプロセッサがディスパッチ禁止中でも
 *  発行する．
 *  自タスクが対象の場合は，他のsus_tskとレーシングすると，E_QOVR が返るこ
 *  とがある．
 */
#ifdef TOPPERS_sus_tsk

ER
sus_tsk(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;
	TCB		*p_selftsk;
	PCB		*p_my_pcb;

	LOG_SUS_TSK_ENTER(tskid);
	CHECK_TSKCTX_UNL_MYSTATE(&p_selftsk);		/*［NGKI1299］［NGKI1300］*/
	if (tskid == TSK_SELF) {
		p_tcb = p_selftsk;						/*［NGKI1310］*/
	}
	else {
		CHECK_ID(VALID_TSKID(tskid));			/*［NGKI1302］*/
		p_tcb = get_tcb(tskid);
	}

	lock_cpu();
	acquire_glock();
	p_my_pcb = get_my_pcb();
	if (p_tcb == p_selftsk && !(p_my_pcb->dspflg)) {/*［NGKI1311］［NGKI3604］*/
		ercd = E_CTX;
	}
	else if (TSTAT_DORMANT(p_tcb->tstat)) {
		ercd = E_OBJ;							/*［NGKI1305］*/
	}
	else if (p_tcb->raster) {
		ercd = E_RASTER;						/*［NGKI3605］*/
	}
	else if (TSTAT_RUNNABLE(p_tcb->tstat)) {
		/*
		 *  実行できる状態から強制待ち状態への遷移［NGKI1307］
		 */
		p_tcb->tstat = TS_SUSPENDED;
		LOG_TSKSTAT(p_tcb);
		make_non_runnable(p_my_pcb, p_tcb);
		if (p_selftsk != p_my_pcb->p_schedtsk) {
			release_glock();
			dispatch();
			ercd = E_OK;
			goto unlock_and_exit;
		}
		ercd = E_OK;
	}
	else if (TSTAT_SUSPENDED(p_tcb->tstat)) {
		ercd = E_QOVR;							/*［NGKI1306］*/
	}
	else {
		/*
		 *  待ち状態から二重待ち状態への遷移［NGKI1308］
		 */
		p_tcb->tstat |= TS_SUSPENDED;
		LOG_TSKSTAT(p_tcb);
		ercd = E_OK;
	}
	release_glock();
  unlock_and_exit:
	unlock_cpu();

  error_exit:
	LOG_SUS_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_sus_tsk */

/*
 *  強制待ち状態からの再開［NGKI1312］
 */
#ifdef TOPPERS_rsm_tsk

ER
rsm_tsk(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;
	TCB		*p_selftsk;
	PCB		*p_my_pcb;

	LOG_RSM_TSK_ENTER(tskid);
	CHECK_TSKCTX_UNL_MYSTATE(&p_selftsk);	/*［NGKI1313］［NGKI1314］*/
	CHECK_ID(VALID_TSKID(tskid));				/*［NGKI1316］*/
	p_tcb = get_tcb(tskid);

	lock_cpu();
	acquire_glock();
	p_my_pcb = get_my_pcb();
	if (!TSTAT_SUSPENDED(p_tcb->tstat)) {
		ercd = E_OBJ;							/*［NGKI1319］*/
	}
	else {
		/*
		 *  強制待ちからの再開［NGKI1320］
		 */
		if (!TSTAT_WAITING(p_tcb->tstat)) {
			p_tcb->tstat = TS_RUNNABLE;
			LOG_TSKSTAT(p_tcb);
			make_runnable(p_my_pcb, p_tcb);
			if (p_selftsk != p_my_pcb->p_schedtsk) {
				release_glock();
				dispatch();
				ercd = E_OK;
				goto unlock_and_exit;
			}
		}
		else {
			p_tcb->tstat &= ~TS_SUSPENDED;
			LOG_TSKSTAT(p_tcb);
		}
		ercd = E_OK;
	}
	release_glock();
  unlock_and_exit:
	unlock_cpu();

  error_exit:
	LOG_RSM_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_rsm_tsk */

/*
 *  自タスクの遅延［NGKI1348］
 */
#ifdef TOPPERS_dly_tsk

ER
dly_tsk(RELTIM dlytim)
{
	ER		ercd;
	TCB		*p_selftsk;
	PCB		*p_my_pcb;

	LOG_DLY_TSK_ENTER(dlytim);
	CHECK_DISPATCH();							/*［NGKI1349］*/
	CHECK_PAR(VALID_RELTIM(dlytim));			/*［NGKI1351］*/

	lock_cpu_dsp();
	acquire_glock();
	p_my_pcb = get_my_pcb();
	p_selftsk = p_my_pcb->p_runtsk;
	if (p_selftsk->raster) {
		ercd = E_RASTER;						/*［NGKI3456］*/
		release_glock();
	}
	else {
		make_wait(p_my_pcb, TS_WAITING_DLY, p_selftsk);
		p_selftsk->winfo.tmevtb.callback = (CBACK) wait_tmout_ok;
		p_selftsk->winfo.tmevtb.arg = (void *) p_selftsk;
		tmevtb_enqueue_reltim(&(p_selftsk->winfo.tmevtb), dlytim,
							  p_my_pcb);
		LOG_TSKSTAT(p_selftsk);
		release_glock();
		dispatch();
		ercd = p_selftsk->winfo.wercd;
	}
	unlock_cpu_dsp();

  error_exit:
	LOG_DLY_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_dly_tsk */
