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
 *  $Id: test_mtskman1.c 265 2021-01-08 06:17:00Z ertl-honda $
 */

/* 
 *		マルチプロセッサ対応のタスク管理機能のテスト(1)
 *
 * 【テストの目的】
 *
 *	マルチプロセッサにおけるact_tskの振る舞いをテストする．あわせて，
 *	ref_tskについてもテストする．
 *
 * 【テスト項目】
 *
 *	(A) 他プロセッサ上の休止状態のタスクに対するact_tsk
 *		(A-1) タスクコンテキストから呼出しで，タスク切換えがある場合
 *		(A-2) タスクコンテキストから呼出しで，タスク切換えがない場合
 *		(A-3) 非タスクコンテキストから呼出しで，タスク切換えがない場合
 *		(A-4) 非タスクコンテキストから呼出しで，タスク切換えがある場合
 *		(A-5) 割付けプロセッサが変わらないこと
 *	(B) 他プロセッサ上の休止状態でないタスクに対するact_tskで，起動要
 *		求がキューイングされる場合
 *		(B-1) 割付けプロセッサが変わらないこと
 *
 * 【使用リソース】
 *
 *	CLS_PRC1: プロセッサ1のみで実行
 *	CLS_PRC2: プロセッサ2のみで実行
 *	CLS_ALL_PRC2: 両プロセッサで実行，初期割付けはプロセッサ2
 *
 *	TASK1: CLS_PRC1，中優先度タスク，メインタスク，最初から起動
 *	TASK2: CLS_PRC2，低優先度タスク
 *	TASK3: CLS_ALL_PRC2: 高優先度タスク
 *	ALM1:  CLS_PRC1，アラームハンドラ
 *
 * 【テストシーケンス】
 *
 *	== START_PRC1 ==					｜	== START_PRC2 ==
 *	== TASK1（中）==					｜
 *	1:	ref_tsk(TASK2, &rtsk)			｜
 *		assert(rtsk.tskstat == TTS_DMT)	｜
 *		assert(rtsk.prcid == PRC2)		｜
 *		assert(rtsk.actprc == TSK_NONE)	｜
 *		ref_tsk(TASK3, &rtsk)			｜
 *		assert(rtsk.tskstat == TTS_DMT)	｜
 *		assert(rtsk.prcid == PRC2)		｜
 *		assert(rtsk.actprc == TSK_NONE)	｜
 *		act_tsk(TASK3)		... (A-1)	｜	== TASK3-1（高，1回目）==
 *	2:	BARRIER(1)						｜	1:	BARRIER(1)
 *		ref_tsk(TASK3, &rtsk)			｜		ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskstat == TTS_RUN)	｜		assert(rtsk.tskstat == TTS_RUN)
 *		assert(rtsk.prcid == PRC2)		｜		assert(rtsk.prcid == PRC1)
 *		assert(rtsk.actprc == TSK_NONE)	｜		assert(rtsk.actprc == TSK_NONE)
 *	3:	act_tsk(TASK2)		... (A-2)	｜
 *		BARRIER(2)						｜	2:	BARRIER(2)
 *		// 実行を継続					｜		ext_tsk()
 *										｜	== TASK2-1（低，1回目）==
 *	4:	BARRIER(3)						｜	3:	BARRIER(3)
 *		sta_alm(ALM1, TEST_TIME_CP)		｜		ext_tsk()
 *		// 実行を継続					｜
 *	== ALM1-1（1回目）==				｜
 *	5:	act_tsk(TASK3)		... (A-3)	｜	== TASK3-2（高，2回目）==
 *		BARRIER(4)						｜	4:	BARRIER(4)
 *	6:	act_tsk(TASK2)		... (A-4)	｜
 *		RETURN							｜
 *	== TASK1（続き）==					｜
 *		BARRIER(5)						｜	5:	BARRIER(5)
 *										｜	6:	ext_tsk()
 *										｜	== TASK2-2（低，2回目）==
 *	7:	BARRIER(6)						｜	7:	BARRIER(6)
 *		mact_tsk(TASK3, PRC1)			｜		ext_tsk()
 *	== TASK3-3（高，3回目）==			｜
 *	8:	ext_tsk()						｜
 *	== TASK1（続き）==					｜
 *	9:	act_tsk(TASK3)		... (A-5)	｜
 *	== TASK3-4（高，4回目）==			｜
 *	10:	ext_tsk()						｜
 *	== TASK1（続き）==					｜
 *	11:	mact_tsk(TASK3, TPRC_INI)		｜	== TASK3-5（高，5回目）==
 *		BARRIER(7)						｜	8:	BARRIER(7)
 *		act_tsk(TASK3)		... (B-1)	｜
 *	12:	BARRIER(8)						｜	9:	BARRIER(8)
 *										｜		ext_tsk()
 *										｜	== TASK3-6（高，6回目）==
 *	13:	BARRIER(9)						｜	10:	BARRIER(9)
 *	14:	END								｜		ext_tsk()
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

static uint_t	alarm1_count = 0;

void
alarm1_handler(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++alarm1_count) {
	case 1:
		check_point_PRC1(5);
		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		test_barrier(4);

		check_point_PRC1(6);
		ercd = act_tsk(TASK2);
		check_ercd(ercd, E_OK);

		return;

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

void
task1(EXINF exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;

	test_start(__FILE__);

	check_point_PRC1(1);
	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskstat == TTS_DMT);

	check_assert(rtsk.prcid == PRC2);

	check_assert(rtsk.actprc == TSK_NONE);

	ercd = ref_tsk(TASK3, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskstat == TTS_DMT);

	check_assert(rtsk.prcid == PRC2);

	check_assert(rtsk.actprc == TSK_NONE);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point_PRC1(2);
	test_barrier(1);

	ercd = ref_tsk(TASK3, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskstat == TTS_RUN);

	check_assert(rtsk.prcid == PRC2);

	check_assert(rtsk.actprc == TSK_NONE);

	check_point_PRC1(3);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	test_barrier(2);

	check_point_PRC1(4);
	test_barrier(3);

	ercd = sta_alm(ALM1, TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	test_barrier(5);

	check_point_PRC1(7);
	test_barrier(6);

	ercd = mact_tsk(TASK3, PRC1);
	check_ercd(ercd, E_OK);

	check_point_PRC1(9);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point_PRC1(11);
	ercd = mact_tsk(TASK3, TPRC_INI);
	check_ercd(ercd, E_OK);

	test_barrier(7);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point_PRC1(12);
	test_barrier(8);

	check_point_PRC1(13);
	test_barrier(9);

	check_finish_PRC1(14);
	check_assert(false);
}

static uint_t	task2_count = 0;

void
task2(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task2_count) {
	case 1:
		check_point_PRC2(3);
		test_barrier(3);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point_PRC2(7);
		test_barrier(6);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	task3_count = 0;

void
task3(EXINF exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;

	switch (++task3_count) {
	case 1:
		check_point_PRC2(1);
		test_barrier(1);

		ercd = ref_tsk(TASK1, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskstat == TTS_RUN);

		check_assert(rtsk.prcid == PRC1);

		check_assert(rtsk.actprc == TSK_NONE);

		check_point_PRC2(2);
		test_barrier(2);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point_PRC2(4);
		test_barrier(4);

		check_point_PRC2(5);
		test_barrier(5);

		check_point_PRC2(6);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point_PRC1(8);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point_PRC1(10);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point_PRC2(8);
		test_barrier(7);

		check_point_PRC2(9);
		test_barrier(8);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 6:
		check_point_PRC2(10);
		test_barrier(9);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}
