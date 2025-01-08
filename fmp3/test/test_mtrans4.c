/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2019 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: test_mtrans4.c 265 2021-01-08 06:17:00Z ertl-honda $
 */

/* 
 *		過渡的な状態のテスト(4)
 *
 * 【テストの目的】
 *
 *  CPUロック状態でext_tskを呼び出した場合に，CPUロック解除状態に遷移
 *  した後，自タスクが終了する前に，割込みを受け付ける場合があり，その
 *  結果，他のタスクに切り換わる場合もあることをテストする［NGKI5183］．
 *
 * 【テスト項目】
 *
 *	(A) CPUロック状態で強制待ち状態［実行継続中］のタスクが，ext_tskを
 *		呼び出した場合に，タスクが終了する（休止状態に遷移する）場合と，
 *		強制待ち状態に遷移する場合があること［NGKI0648］
 *	(B) タスクAがCPUロック状態に遷移した後，同じプロセッサ上で優先度の
 *		高いタスクBを起動した状態で，タスクAがext_tskを呼び出した場合
 *		に，タスクAが終了する（休止状態に遷移する）場合と，実行可能状
 *		態に遷移する場合があること
 *
 * 【使用リソース】
 *
 *	CLS_PRC1: プロセッサ1のみで実行
 *	CLS_PRC2: プロセッサ2のみで実行
 *
 *	TASK1: CLS_PRC1，中優先度タスク，メインタスク，最初から起動
 *	TASK2: CLS_PRC2，中優先度タスク
 *	TASK3: CLS_PRC2，低優先度タスク
 *	TASK4: CLS_PRC2，高優先度タスク
 */

#include <kernel.h>
#include <sil.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"
#include <stdlib.h>

#define NO_LOOP			1000U

uint_t	count_dmt;
uint_t	count_sus;
uint_t	count_rdy;

volatile bool_t	task1_flag;
volatile bool_t	task2_flag;

void
task1(EXINF exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;
	uint_t	i;

	test_start(__FILE__);

	/*
	 *  テスト(A)
	 */
	syslog_0(LOG_NOTICE, "Test A");
	count_dmt = 0U;
	count_sus = 0U;

	for (i = 0; i < NO_LOOP; i++) {
		/* TASK3を起動する */
		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		/* TASK2がloc_cpuを呼び出すのを待つ */
		WAIT_RESET(task1_flag);
		ercd = sus_tsk(TASK2);
		check_ercd(ercd, E_OK);

		/* TASK1がsus_tskを呼び出したことを知らせる */
		SET(task2_flag);

		/* TASK3が終了するまで待つ */
		do {
			ercd = ref_tsk(TASK3, &rtsk);
			check_ercd(ercd, E_OK);
		} while (rtsk.tskstat != TTS_DMT);
	}
	syslog_1(LOG_NOTICE, "TTS_DMT is referenced: %d", count_dmt);
	syslog_1(LOG_NOTICE, "TTS_SUS is referenced: %d", count_sus);

	/*
	 *  テスト(B)
	 */
	syslog_0(LOG_NOTICE, "Test B");
	count_dmt = 0U;
	count_rdy = 0U;

	for (i = 0; i < NO_LOOP; i++) {
		/* TASK2を起動する */
		ercd = act_tsk(TASK2);
		check_ercd(ercd, E_OK);

		/* TASK2がloc_cpuを呼び出すのを待つ */
		WAIT_RESET(task1_flag);
		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		/* TASK1がact_tskを呼び出したことを知らせる */
		SET(task2_flag);

		/* TASK4が終了するまで待つ */
		do {
			ercd = ref_tsk(TASK4, &rtsk);
			check_ercd(ercd, E_OK);
		} while (rtsk.tskstat != TTS_DMT);
	}
	syslog_1(LOG_NOTICE, "TTS_DMT is referenced: %d", count_dmt);
	syslog_1(LOG_NOTICE, "TTS_RDY is referenced: %d", count_rdy);

	syslog_0(LOG_NOTICE, "Test finished.");
	check_finish_PRC1(0);
	check_assert(false);
}

void
task2(EXINF exinf)
{
	ER_UINT	ercd;

	ercd = loc_cpu();
	check_ercd(ercd, E_OK);

	/* TASK2がloc_cpuを呼び出したことを知らせる */
	SET(task1_flag);

	/* テスト(A)：TASK1がsus_tskを呼び出すのを待つ */
	/* テスト(B)：TASK1がact_tskを呼び出すのを待つ */
	WAIT_RESET(task2_flag);

	/* CPUロック状態でext_tskを呼ぶ */
	ext_tsk();
}

void
task3(EXINF exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;

	/* TASK2を起動する */
	ercd = act_tsk(TASK2);

	/* TASK2が終了するか，強制待ち状態になると，ここへ戻ってくる */
	check_ercd(ercd, E_OK);

	/* TASK2の状態を参照し，集計する */
	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	switch (rtsk.tskstat) {
	case TTS_DMT:
		count_dmt++;
		break;
	case TTS_SUS:
		count_sus++;
		ercd = rsm_tsk(TASK2);
		check_ercd(ercd, E_OK);
		break;
	default:
		check_assert(false);
	}
	ext_tsk();
}

void
task4(EXINF exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;

	/* TASK2の状態を参照し，集計する */
	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	switch (rtsk.tskstat) {
	case TTS_DMT:
		count_dmt++;
		break;
	case TTS_RDY:
		count_rdy++;
		ercd = chg_pri(TSK_SELF, LOW_PRIORITY);
		check_ercd(ercd, E_OK);
		break;
	default:
		check_assert(false);
	}
	ext_tsk();
}
