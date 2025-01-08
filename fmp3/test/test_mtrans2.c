/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2018,2019 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: test_mtrans2.c 312 2022-12-07 14:27:02Z ertl-honda $
 */

/* 
 *		過渡的な状態のテスト(2)
 *
 * 【テストの目的】
 *
 *	自タスクが強制待ち状態に見える場合があることをテストする．
 *
 * 【テスト項目】
 *
 *	(A) get_tstで，対象タスクが自タスクの場合にも，TTS_SUSが返る場合
 *		があること［NGKI5209］
 *	(B) ref_tskで，対象タスクが自タスクの場合にも，tskstatがTTS_SUSと
 *		なる場合があること［NGKI1226］
 *	(C) sus_tskで，対象タスクが自タスクの場合にも，E_QOVRエラーとなる
 *		場合があること［NGKI1309］
 *	(D) loc_mtxにASP3カーネルと同様のassertを入れると，assert failが起
 *		こる場合があること（assert failになる条件を作り出す）
 *
 * 【使用リソース】
 *
 *	CLS_PRC1: プロセッサ1のみで実行
 *	CLS_PRC2: プロセッサ2のみで実行
 *
 *	TASK1: CLS_PRC1，中優先度タスク，メインタスク，最初から起動
 *	TASK2: CLS_PRC2，中優先度タスク
 *	TASK3: CLS_PRC2，中優先度タスク
 *	MTX1: CLS_PRC1，ミューテックス（TA_CEILING属性，上限は中優先度）
 */

#include <kernel.h>
#include <sil.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"
#include <stdlib.h>

#define NO_LOOP			1000U

#ifndef TEST_DELAY_TIME_NSE
#define TEST_DELAY_TIME_NSE		10U
#endif /* TEST_DELAY_TIME_NSE */

uint_t	count_run;
uint_t	count_sus;

uint_t	count_ok;
uint_t	count_qovr;

volatile bool_t	task2_flag;
volatile bool_t	task3_flag;

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
	sil_dly_nse(count + 1);
}

void
task1(EXINF exinf)
{
	ER_UINT	ercd;
	STAT	tskstat;
	T_RTSK	rtsk;
	uint_t	i;

	test_start(__FILE__);

	/*
	 *  テスト(A)
	 */
	count_run = 0U;
	count_sus = 0U;

	task2_flag = true;
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	for (i = 0; i < NO_LOOP; i++) {
		ercd = get_tst(TSK_SELF, &tskstat);
		check_ercd(ercd, E_OK);

		switch (tskstat) {
		case TTS_RUN:
			count_run++;
			break;
		case TTS_SUS:
			count_sus++;							/* テスト(A) */
			break;
		default:
			check_assert(false);
		}

		delay_count(get_rand(TEST_DELAY_TIME_NSE * 10));
	}
	task2_flag = false;

	syslog_1(LOG_NOTICE, "TTS_RUN is referenced: %d", count_run);
	syslog_1(LOG_NOTICE, "TTS_SUS is referenced: %d", count_sus);

	/*
	 *  テスト(B)
	 */
	count_run = 0U;
	count_sus = 0U;

	task2_flag = true;
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	for (i = 0; i < NO_LOOP; i++) {
		ercd = ref_tsk(TSK_SELF, &rtsk);
		check_ercd(ercd, E_OK);

		switch (rtsk.tskstat) {
		case TTS_RUN:
			count_run++;
			break;
		case TTS_SUS:
			count_sus++;							/* テスト(B) */
			break;
		default:
			check_assert(false);
		}

		delay_count(get_rand(TEST_DELAY_TIME_NSE * 10));
	}
	task2_flag = false;

	syslog_1(LOG_NOTICE, "TTS_RUN is referenced: %d", count_run);
	syslog_1(LOG_NOTICE, "TTS_SUS is referenced: %d", count_sus);

	/*
	 *  テスト(C)
	 */
	count_ok = 0U;
	count_qovr = 0U;

	task3_flag = true;
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	for (i = 0; i < NO_LOOP; i++) {
		ercd = sus_tsk(TSK_SELF);

		if (MERCD(ercd) == E_QOVR) {
			count_qovr++;							/* テスト(C) */
		}
		else {
			check_ercd(ercd, E_OK);
			count_ok++;
		}

		delay_count(get_rand(TEST_DELAY_TIME_NSE));
	}
	task3_flag = false;

	syslog_1(LOG_NOTICE, "E_OK is returned: %d", count_ok);
	syslog_1(LOG_NOTICE, "E_QOVR is returned: %d", count_qovr);

	/*
	 *  テスト(D)
	 */
	task2_flag = true;
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	for (i = 0; i < NO_LOOP; i++) {
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		delay_count(get_rand(TEST_DELAY_TIME_NSE * 10));
	}

	task2_flag = false;

	syslog_0(LOG_NOTICE, "Test finished.");
	check_finish_PRC1(0);
	check_assert(false);
}

void
task2(EXINF exinf)
{
	ER_UINT	ercd;

	while (task2_flag) {
		ercd = sus_tsk(TASK1);
		check_ercd(ercd, E_OK);

		ercd = rsm_tsk(TASK1);
		check_ercd(ercd, E_OK);

		delay_count(get_rand(TEST_DELAY_TIME_NSE));
	}
}

void
task3(EXINF exinf)
{
	ER_UINT	ercd;

	while (task3_flag) {
		ercd = sus_tsk(TASK1);
		if (MERCD(ercd) != E_QOVR) {
			check_ercd(ercd, E_OK);
		}

		ercd = rsm_tsk(TASK1);
		check_ercd(ercd, E_OK);

		delay_count(get_rand(TEST_DELAY_TIME_NSE));
	}
}
