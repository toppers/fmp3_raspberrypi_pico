/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2018 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: test_mtrans3.c 265 2021-01-08 06:17:00Z ertl-honda $
 */

/* 
 *		過渡的な状態のテスト(3)
 *
 * 【テストの目的】
 *
 *	タスクがマイグレートを繰り返す中で，各種のサービスコールが正常に動
 *	作することをテストする．
 *
 * 【テスト項目】
 *
 * 【使用リソース】
 *
 *	CLS_PRC1: プロセッサ1のみで実行
 *	CLS_PRC2: プロセッサ2のみで実行
 *	CLS_ALL_PRC1: 両プロセッサで実行，初期割付けはプロセッサ1
 *
 *	TASK1: CLS_ALL_PRC1，中優先度タスク，メインタスク，最初から起動
 *	TASK2: CLS_PRC1，高優先度タスク
 *	TASK3: CLS_PRC2，高優先度タスク
 */

#include <kernel.h>
#include <sil.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"

#define NO_LOOP			10000U

#ifndef TEST_DELAY_TIME
#define TEST_DELAY_TIME		100U
#endif /* TEST_DELAY_TIME */

uint_t run_on_prc1;
uint_t run_on_prc2;

volatile uint_t	count_mig1to2;
volatile uint_t	count_mig2to1;

volatile bool_t	task2_flag;
volatile bool_t	task3_flag;

void
task1(EXINF exinf)
{
	ER_UINT	ercd;
	ID		prcid;
	uint_t	i;

	test_start(__FILE__);

	run_on_prc1 = 0U;
	run_on_prc2 = 0U;
	count_mig1to2 = 0U;
	count_mig2to1 = 0U;

	task2_flag = true;
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	task3_flag = true;
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

    for (i = 0; i < NO_LOOP; i++) {
		ercd = get_pid(&prcid);
		check_ercd(ercd, E_OK);
		switch (prcid) {
		case PRC1:
			run_on_prc1++;
			break;
		case PRC2:
			run_on_prc2++;
			break;
		default:
			check_assert(false);
		}

		ercd = dis_dsp();
		check_ercd(ercd, E_OK);
		check_assert(sns_dsp() == true);

		ercd = ena_dsp();
		check_ercd(ercd, E_OK);
		check_assert(sns_dsp() == false);

		ercd = wup_tsk(TSK_SELF);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);
	}
	task2_flag = false;
	task3_flag = false;

	syslog_1(LOG_NOTICE, "running on PRC1: %d", run_on_prc1);
	syslog_1(LOG_NOTICE, "running on PRC2: %d", run_on_prc2);

	syslog_1(LOG_NOTICE, "migration from PRC1 to PRC2: %d", count_mig1to2);
	syslog_1(LOG_NOTICE, "migration from PRC2 to PRC1: %d", count_mig2to1);

	check_finish_PRC1(0);
	check_assert(false);
}

void
task2(EXINF exinf)
{
	T_RTSK	rtsk;
	ER_UINT	ercd;

	while (task2_flag) {
		ercd = ref_tsk(TASK1, &rtsk);
		check_ercd(ercd, E_OK);

		if (rtsk.prcid == PRC1) {
			mig_tsk(TASK1, PRC2);
			count_mig1to2++;
		}

		ercd = dly_tsk(TEST_DELAY_TIME);
		check_ercd(ercd, E_OK);
	}
}

void
task3(EXINF exinf)
{
	T_RTSK	rtsk;
	ER_UINT	ercd;

	while (task3_flag) {
		ercd = ref_tsk(TASK1, &rtsk);
		check_ercd(ercd, E_OK);

		if (rtsk.prcid == PRC2) {
			mig_tsk(TASK1, PRC1);
			count_mig2to1++;
		}

		ercd = dly_tsk(TEST_DELAY_TIME);
		check_ercd(ercd, E_OK);
	}
}
