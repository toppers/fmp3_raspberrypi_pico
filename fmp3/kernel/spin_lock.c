/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2007-2020 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: spin_lock.c 263 2021-01-08 06:08:59Z ertl-honda $
 */

/*
 *		スピンロック機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "spin_lock.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_LOC_SPN_ENTER
#define LOG_LOC_SPN_ENTER(spnid)
#endif  /* LOG_LOC_SPN_ENTER */

#ifndef LOG_LOC_SPN_LEAVE
#define LOG_LOC_SPN_LEAVE(ercd)
#endif  /* LOG_LOC_SPN_LEAVE */

#ifndef LOG_TRY_SPN_ENTER
#define LOG_TRY_SPN_ENTER(spnid)
#endif  /* LOG_TRY_SPN_ENTER */

#ifndef LOG_TRY_SPN_LEAVE
#define LOG_TRY_SPN_LEAVE(ercd)
#endif  /* LOG_TRY_SPN_LEAVE */

#ifndef LOG_UNL_SPN_ENTER
#define LOG_UNL_SPN_ENTER(spnid)
#endif  /* LOG_UNL_SPN_ENTER */

#ifndef LOG_UNL_SPN_LEAVE
#define LOG_UNL_SPN_LEAVE(ercd)
#endif  /* LOG_UNL_SPN_LEAVE */

#ifndef LOG_REF_SPN_ENTER
#define LOG_REF_SPN_ENTER(spnid, pk_rspn)
#endif  /* LOG_REF_SPN_ENTER */

#ifndef LOG_REF_SPN_LEAVE
#define LOG_REF_SPN_LEAVE(ercd, pk_rspn)
#endif  /* LOG_REF_SPN_LEAVE */

/*
 *  スピンロックの数
 */
#define tnum_spn			((uint_t)(tmax_spnid - TMIN_SPNID + 1))
#define INDEX_SPN(spnid)	((uint_t)((spnid) - TMIN_SPNID))
#define get_spninib(spnid)	(&(spninib_table[INDEX_SPN(spnid)]))

/*
 *  スピンロックの実現方式を判定するマクロの定義
 *
 *  このマクロ（IS_NATIVE）は，コンパイラの最適化により不要なコードが
 *  削除されるようにするために用意している．
 */
#ifndef TMAX_NATIVE_SPN

/*
 *  TMAX_NATIVE_SPNが定義されていない場合は，ネイティブスピンロックの
 *  みを使用する．
 */
#define IS_NATIVE(spnatr)	true

#else /* TMAX_NATIVE_SPN */
#if TMAX_NATIVE_SPN == 0

/*
 *  TMAX_NATIVE_SPNが0の場合は，エミュレートされたスピンロックのみを使
 *  用する．
 */
#define IS_NATIVE(spnatr)	false

#else /* TMAX_NATIVE_SPN == 0 */

/*
 *  TMAX_NATIVE_SPNが0より大きい場合は，両方のスピンロックを使用する．
 */
#define IS_NATIVE(spnatr)	(((spnatr) & TA_NATIVE) != 0U)

#endif /* TMAX_NATIVE_SPN == 0 */
#endif /* TMAX_NATIVE_SPN */

/*
 *  エミュレートされたスピンロックにおけるロック状態を表す変数
 */
#define LOCKFLAG(p_spninib)		(*((volatile bool_t *)((p_spninib)->lock)))

/*
 *  スピンロック機能の初期化
 */
#ifdef TOPPERS_spnini

void
initialize_spin_lock(PCB *p_my_pcb)
{
	uint_t			i;
	const SPNINIB	*p_spninib;

	if (p_my_pcb->prcid == TOPPERS_MASTER_PRCID) {
		for (i = 0; i < tnum_spn;  i++) {
			p_spninib = &(spninib_table[i]);
			if (IS_NATIVE(p_spninib->spnatr)) {
				initialize_native_spn(p_spninib);
			}
			else {
				LOCKFLAG(p_spninib) = false;
			}
		}
	}
}

#endif /* TOPPERS_spnini */

/*
 *  スピンロックの強制解放
 */
#ifdef TOPPERS_funlspn

void
force_unlock_spin(PCB *p_my_pcb)
{
	const SPNINIB	*p_spninib = p_my_pcb->p_locspn;

	if (p_spninib != NULL) {
		/* ここではCPUロック状態になっている */
		p_my_pcb->p_locspn = NULL;
		if (IS_NATIVE(p_spninib->spnatr)) {
			unlock_native_spn(p_spninib);
		}
		else {
			LOCKFLAG(p_spninib) = false;
		}
	}
}

#endif /* TOPPERS_funlspn */

/*
 *  スピンロックの取得
 */
#ifdef TOPPERS_loc_spn

ER
loc_spn(ID spnid)
{
	ER				ercd;
	const SPNINIB	*p_spninib;
	TCB				*p_selftsk;
	bool_t			context;

	LOG_LOC_SPN_ENTER(spnid);
	CHECK_UNL_MYSTATE(&p_selftsk, &context);
	CHECK_ID(VALID_SPNID(spnid));
	p_spninib = get_spninib(spnid);

	/*
	 *  ロックを取得している場合にはCHECK_UNL_MYSTATEでエラーとなるの
	 *  で，ここに来る時にはロックは取得していない．
	 */
	lock_cpu();

	if (IS_NATIVE(p_spninib->spnatr)) {
		lock_native_spn(p_spninib);
	}
	else {
		acquire_glock();
		while (LOCKFLAG(p_spninib)) {
			/* 取得されていた場合 */
			release_glock();
			unlock_cpu();
			while (LOCKFLAG(p_spninib)) {
				delay_for_emulate_spn();
				delay_for_interrupt();
			}
			lock_cpu();
			acquire_glock();
		}
			/* 取得されていなかった場合 */
			LOCKFLAG(p_spninib) = true;
			release_glock();
		}
	get_my_pcb()->p_locspn = p_spninib;
	ercd = E_OK;

  error_exit:
	LOG_LOC_SPN_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_loc_spn */

/*
 *  スピンロックの取得（ポーリング）
 */
#ifdef TOPPERS_try_spn

ER
try_spn(ID spnid)
{
	ER				ercd;
	const SPNINIB	*p_spninib;
	TCB				*p_selftsk;
	bool_t			context;

	LOG_TRY_SPN_ENTER(spnid);
	CHECK_UNL_MYSTATE(&p_selftsk, &context);
	CHECK_ID(VALID_SPNID(spnid));
	p_spninib = get_spninib(spnid);

	/*
	 *  ロックを取得している場合にはCHECK_UNL_MYSTATEでエラーとなるの
	 *  で，ここに来る時にはロックは取得していない．
	 */
	lock_cpu();

	if (IS_NATIVE(p_spninib->spnatr)) {
		if (try_native_spn(p_spninib)) {
			/* 取得されていた場合 */
			unlock_cpu();
			ercd = E_OBJ;
		}
		else {
			/* 取得されていなかった場合 */
			get_my_pcb()->p_locspn = p_spninib;
			ercd = E_OK;
		}
	}
	else {
		acquire_glock();
		if (LOCKFLAG(p_spninib)) {
			/* 取得されていた場合 */
			release_glock();
			unlock_cpu();
			delay_for_interrupt();
			ercd = E_OBJ;
		}
		else {
			/* 取得されていなかった場合 */
			LOCKFLAG(p_spninib) = true;
			release_glock();
			get_my_pcb()->p_locspn = p_spninib;
			ercd = E_OK;
		}
	}

  error_exit:
	LOG_TRY_SPN_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_try_spn */

/*
 *  スピンロックの返却
 */
#ifdef TOPPERS_unl_spn

ER
unl_spn(ID spnid)
{
	ER				ercd;
	const SPNINIB	*p_spninib;
	PCB				*p_my_pcb;
	bool_t			locked;

	LOG_UNL_SPN_ENTER(spnid);
	CHECK_ID(VALID_SPNID(spnid));
	p_spninib = get_spninib(spnid);

	locked = sense_lock();
	if (!locked) {
		lock_cpu();
	}
	p_my_pcb = get_my_pcb();
	if (p_my_pcb->p_locspn != p_spninib) {
		if (!locked) {
			unlock_cpu();
		}
		ercd = E_ILUSE;
	}
	else {
		/*
		 *  取得との競合を避けるため，返却の前にp_my_pcb->p_locspnを設
		 *  定する．
		 */
		p_my_pcb->p_locspn = NULL;
		if (IS_NATIVE(p_spninib->spnatr)) {
			unlock_native_spn(p_spninib);
		}
		else {
			/*
			 *  1つの変数を設定するだけなので，プロセッサ間の排他制御
			 *  は必要ない．
			 */
			LOCKFLAG(p_spninib) = false;
		}
		unlock_cpu();
		ercd = E_OK;
	}

  error_exit:
	LOG_UNL_SPN_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_unl_spn */

/*
 *  スピンロックの状態参照
 */
#ifdef TOPPERS_ref_spn

ER
ref_spn(ID spnid, T_RSPN *pk_rspn)
{
	ER				ercd;
	const SPNINIB	*p_spninib;
	TCB				*p_selftsk;
	bool_t			spnlocked;

	LOG_REF_SPN_ENTER(spnid, pk_rspn);
	CHECK_TSKCTX_UNL_MYSTATE(&p_selftsk);
												/*［NGKI2207］［NGKI2208］*/
	CHECK_ID(VALID_SPNID(spnid));							/*［NGKI2209］*/
	p_spninib = get_spninib(spnid);

	if (IS_NATIVE(p_spninib->spnatr)) {
		spnlocked = refer_native_spn(p_spninib);
	}
	else {
		spnlocked = LOCKFLAG(p_spninib);
	}
	pk_rspn->spnstat = spnlocked ? TSPN_LOC : TSPN_UNL;
	ercd = E_OK;								/*［NGKI2213］［NGKI2214］*/

  error_exit:
	LOG_REF_SPN_LEAVE(ercd, pk_rspn);
	return(ercd);
}

#endif /* TOPPERS_ref_spn */
