/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2020 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: test_mtrans5.c 265 2021-01-08 06:17:00Z ertl-honda $
 */

/* 
 *		過渡的な状態のテスト(5)
 *
 * 【テストの目的】
 *
 *	ディスパッチ保留状態でタスク切換えが起こる不具合が修正されているこ
 *	とをテストする．
 *
 * 【テスト項目】
 *
 *	(A) dis_dspによるディスパッチ保留状態で，タスク切換えが起こる場合
 *		がないこと．
 *	(B) chg_ipm(TMAX_INTPRI)によるディスパッチ保留状態で，タスク切換え
 *		が起こる場合がないこと．
 *	(C) chg_ipm(TMIN_INTPRI)によるディスパッチ保留状態で，タスク切換え
 *		が起こる場合がないこと．
 *
 * 【使用リソース】
 *
 *	CLS_PRC1: プロセッサ1のみで実行
 *	CLS_PRC2: プロセッサ2のみで実行
 *
 *	TASK1: CLS_PRC1，中優先度タスク，メインタスク，最初から起動
 *	TASK2: CLS_PRC1，高優先度タスク
 *	TASK3: CLS_PRC2，中優先度タスク
 *
 * 【テスト手順】
 *
 *	次の処理を繰り返し実行し，ディスパッチ保留状態で，タスク切換えが起
 *	こらないことを確認する．
 *
 *	(1) プロセッサ上のタスク1から，プロセッサ2上のタスク3を起動する．
 *	(2-1) タスク3が，プロセッサ1上の高優先度のタスク2を起動する．
 *	(2-2) タスク1が，ディスパッチ保留状態に遷移する．
 *	(3) タスク1で，ディスパッチ保留状態になっていることを確認する．
 *	(4) タスク1が，rot_rdqを発行する．
 *	(5) タスク1で，ディスパッチ保留状態になっていることを確認する．
 *	(6) タスク1が，ディスパッチ保留状態を解除する．これにより，タスク2
 *		への切換えが起こる．
 *	(7) タスク2は，すぐに終了する．
 *
 *	不具合があるコードでは，まず(2-1)が実行され，ディスパッチを要求す
 *	るプロセッサ間割込みが受け付けられる前に(2-2)を実行すると，ディス
 *	パッチ保留状態でp_runtskとp_schedtskが一致しない状態が生じる．
 *
 *	ケース1：ここでディスパッチ要求プロセッサ間割込みが受け付けられる
 *	場合には，(3)が実行される前に，タスク1からタスク2に切り換わる．タ
 *	スク2が実行され，終了すると，タスクの終了処理により，ディスパッチ
 *	保留状態が解除される．その結果，(3)のステップで検出される．
 *
 *	ケース2：ディスパッチ要求プロセッサ間割込みが受け付けられない場合
 *	には，(4)によってタスク1からタスク2に切り換わる．タスク2が実行され，
 *	終了すると，タスクの終了処理により，ディスパッチ保留状態が解除され
 *	る．その結果，(5)のステップで検出される．
 */

#include <kernel.h>
#include <sil.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"
#include <stdlib.h>

#define NO_LOOP			10000U

#ifndef TEST_DELAY_TIME_NSE
#define TEST_DELAY_TIME_NSE		10U
#endif /* TEST_DELAY_TIME_NSE */

volatile uint_t	task2_count = 0;

static uint_t
get_rand(uint_t scale)
{
	uint_t	x;

	loc_cpu();
	x = rand();
	unl_cpu();
	return(x % scale);
}

Inline void
delay_count(uint_t count)
{
	volatile uint_t	i;

	for (i = 0; i < count; i++);
}

void
task1(EXINF exinf)
{
	ER_UINT	ercd;
	uint_t	i;
	uint_t	count_wrong_a1, count_wrong_a2;
	uint_t	count_wrong_b1, count_wrong_b2;
	uint_t	count_wrong_c1, count_wrong_c2;

	test_start(__FILE__);

	/*
	 *  テスト(A)
	 */
	count_wrong_a1 = 0U;
	count_wrong_a2 = 0U;

	for (i = 0; i < NO_LOOP; i++) {
		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		delay_count(get_rand(TEST_DELAY_TIME_NSE));

		ercd = dis_dsp();
		check_ercd(ercd, E_OK);

		if (sns_dpn() == false) {
			count_wrong_a1++;
		}
		else {
			ercd = rot_rdq(HIGH_PRIORITY);
			check_ercd(ercd, E_OK);

			if (sns_dpn() == false) {
				count_wrong_a2++;
			}
		}

		ercd = ena_dsp();
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);
	}
	syslog_1(LOG_NOTICE, "Wrong1: %d", count_wrong_a1);
	syslog_1(LOG_NOTICE, "Wrong2: %d", count_wrong_a2);

	/*
	 *  テスト(B)
	 */
	count_wrong_b1 = 0U;
	count_wrong_b2 = 0U;

	for (i = 0; i < NO_LOOP; i++) {
		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		delay_count(get_rand(TEST_DELAY_TIME_NSE));

		ercd = chg_ipm(TMAX_INTPRI);
		check_ercd(ercd, E_OK);

		if (sns_dpn() == false) {
			count_wrong_b1++;
		}
		else {
			ercd = rot_rdq(HIGH_PRIORITY);
			check_ercd(ercd, E_OK);

			if (sns_dpn() == false) {
				count_wrong_b2++;
			}
		}

		ercd = chg_ipm(TIPM_ENAALL);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);
	}
	syslog_1(LOG_NOTICE, "Wrong1: %d", count_wrong_b1);
	syslog_1(LOG_NOTICE, "Wrong2: %d", count_wrong_b2);

	/*
	 *  テスト(C)
	 */
	count_wrong_c1 = 0U;
	count_wrong_c2 = 0U;

	for (i = 0; i < NO_LOOP; i++) {
		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		delay_count(get_rand(TEST_DELAY_TIME_NSE));

		ercd = chg_ipm(TMIN_INTPRI);
		check_ercd(ercd, E_OK);

		if (sns_dpn() == false) {
			count_wrong_c1++;
		}
		else {
			ercd = rot_rdq(HIGH_PRIORITY);
			check_ercd(ercd, E_OK);

			if (sns_dpn() == false) {
				count_wrong_c2++;
			}
		}

		ercd = chg_ipm(TIPM_ENAALL);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);
	}
	syslog_1(LOG_NOTICE, "Wrong1: %d", count_wrong_c1);
	syslog_1(LOG_NOTICE, "Wrong2: %d", count_wrong_c2);

	check_assert(count_wrong_a1 == 0 && count_wrong_a2 == 0
				&& count_wrong_b1 == 0 && count_wrong_b2 == 0
				&& count_wrong_c1 == 0 && count_wrong_c2 == 0);

	syslog_0(LOG_NOTICE, "Test finished.");
	check_finish_PRC1(0);
	check_assert(false);
}

void
task2(EXINF exinf)
{
	task2_count++;
	ext_tsk();
}

void
task3(EXINF exinf)
{
	ER_UINT	ercd;

	delay_count(get_rand(TEST_DELAY_TIME_NSE));

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	ext_tsk();
}
