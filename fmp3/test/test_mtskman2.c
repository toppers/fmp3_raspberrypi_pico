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
 *  $Id: test_mtskman2.c 265 2021-01-08 06:17:00Z ertl-honda $
 */

/* 
 *		マルチプロセッサ対応のタスク管理機能のテスト(2)
 *
 * 【テストの目的】
 *
 *	タスクの割付けプロセッサと次回起動時の割付けプロセッサに関する仕様
 *	（mig_tskに関するものは除く）と，mact_tskの振る舞いをテストする．
 *	あわせて，ref_tskについてもテストする．
 *
 * 【テスト項目】
 *
 *	(A) タスクの生成時の処理
 *		(A-1) 割付けプロセッサが，タスクが属するクラスの初期割付けプロ
 *			  セッサに初期化されること［NGKI1006］
 *		(A-2) 次回起動時の割付けプロセッサが，未設定の状態に初期化され
 *			  ること［NGKI1007］
 *	(B) act_tskの対象タスクが休止状態でなく，起動要求がキューイングさ
 *		れる場合に，次回起動時の割付けプロセッサが設定されないこと
 *	(C) mact_tskの対象タスクが休止状態である場合の処理［NGKI1132］
 *		(C-1) 割付けプロセッサが変わらない場合
 *		(C-2) 割付けプロセッサが変わる場合
 *	(D) mact_tskの対象タスクが休止状態でなく，起動要求がキューイングさ
 *		れる場合の処理［NGKI3557］
 *		(D-1) 次回起動時の割付けプロセッサが，割付けプロセッサと同じに
 *			  なる場合
 *		(D-2) 次回起動時の割付けプロセッサが，割付けプロセッサと別にな
 *			  る場合
 *	(E) mact_tskの対象タスクが休止状態でなく，E_QOVRになる場合［NGKI3558］
 *	(F) mact_tskで，tskidにTSK_SELFを指定した場合［NGKI1135］
 *		(F-1) 次回起動時の割付けプロセッサが，割付けプロセッサと同じに
 *			  なる場合
 *		(F-2) 次回起動時の割付けプロセッサが，割付けプロセッサと別にな
 *			  る場合
 *	(G) mact_tskで，対象タスクを割付け可能でないプロセッサをprcidに指
 *		定した場合［NGKI1136］
 *	(H) mact_tskで，prcidにTPRC_INIを指定した場合［NGKI1137］
 *	(I) can_actで，対象タスクの次回起動時の割付けプロセッサが未設定の
 *		状態に戻ること［NGKI1145］
 *	(J) タスク終了時に，起動要求がキューイングされており，タスクを再起
 *		動する場合の処理［NGKI1008］
 *		(J-1) 次回起動時の割付けプロセッサが設定されていれば，割付けプ
 *			  ロセッサがそのプロセッサに変更されること
 *		(J-2) 上の時に，次回起動時の割付けプロセッサが未設定の状態に戻
 *			  ること
 *		(J-3) 次回起動時の割付けプロセッサが設定されていなければ，割付
 *			  けプロセッサは変更されないこと
 *	(K) ref_tskで，prcidに，対象タスクの割付けプロセッサのID番号が返さ
 *		れること［NGKI1245］
 *	(L) ref_tskで，actprcに，対象タスクの次回起動時の割付けプロセッサ
 *		のID番号が返されること［NGKI1246］
 *	(M) ref_tskで，次回起動時の割付けプロセッサが未設定の場合には，
 *		actprcにTPRC_NONEが返されること［NGKI1247］
 *
 * 【使用リソース】
 *
 *	CLS_PRC1: プロセッサ1のみで実行
 *	CLS_ALL_PRC2: 両プロセッサで実行，初期割付けはプロセッサ2
 *
 *	TASK1: CLS_PRC1，中優先度タスク，メインタスク，最初から起動
 *	TASK2: CLS_ALL_PRC2: 高優先度タスク
 *
 * 【テストシーケンス】
 *
 *	== START_PRC1 ==					｜	== START_PRC2 ==
 *	== TASK1（中）==					｜
 *	1:	ref_tsk(TASK2, &rtsk)			｜
 *		assert(rtsk.tskstat == TTS_DMT)	｜
 *		assert(rtsk.prcid == PRC2)		｜	... (A-1)(K)
 *		assert(rtsk.actprc == TPRC_NONE)｜	... (A-2)(M)
 *		mact_tsk(TASK2, PRC2) ... (C-1)	｜	== TASK2-1（高，1回目）==
 *	2:	BARRIER(1)						｜	1:	BARRIER(1)
 *		ref_tsk(TASK2, &rtsk)			｜
 *		assert(rtsk.tskstat == TTS_RUN)	｜
 *		assert(rtsk.prcid == PRC2)		｜	... (K)
 *		assert(rtsk.actprc == TPRC_NONE)｜	... (M)
 *	3:	BARRIER(2)						｜	2:	BARRIER(2)
 *		dly_tsk(TEST_TIME_CP)			｜		ext_tsk()
 *	4:	mact_tsk(TASK2, PRC1) ... (C-2)	｜
 *	== TASK2-2（高，2回目）==			｜
 *	5:	ref_tsk(TASK2, &rtsk)			｜
 *		assert(rtsk.tskstat == TTS_RUN)	｜
 *		assert(rtsk.prcid == PRC1)		｜	... (K)
 *		assert(rtsk.actprc == TPRC_NONE)｜	... (M)
 *		ext_tsk()						｜
 *	== TASK1（続き）==					｜
 *	6:	mact_tsk(TASK2, PRC2)			｜	== TASK2-3（高，3回目）==
 *		BARRIER(3)						｜	3:	BARRIER(3)
 *	7:	mact_tsk(TASK2, PRC2) ... (D-1)	｜
 *		ref_tsk(TASK2, &rtsk)			｜
 *		assert(rtsk.tskstat == TTS_RUN)	｜
 *		assert(rtsk.prcid == PRC2)		｜	... (K)
 *		assert(rtsk.actprc == PRC2)		｜	... (L)
 *		BARRIER(4)						｜	4:	BARRIER(4)
 *										｜		ext_tsk()
 *										｜	== TASK2-4（高，4回目）==
 *	8:	BARRIER(5)						｜	5:	BARRIER(5)
 *		mact_tsk(TASK2, PRC1) ... (D-2)	｜
 *		ref_tsk(TASK2, &rtsk)			｜
 *		assert(rtsk.tskstat == TTS_RUN)	｜
 *		assert(rtsk.prcid == PRC2)		｜	... (K)
 *		assert(rtsk.actprc == PRC1)		｜	... (L)
 *		mact_tsk(TASK2, PRC2) -> E_QOVR	｜
 *								... (E)	｜
 *		mact_tsk(TASK1, PRC2) -> E_PAR	｜
 *								... (G)	｜
 *	9:	BARRIER(6)						｜	6:	BARRIER(6)
 *										｜		ext_tsk()
 *	== TASK2-5（高，5回目）==			｜
 *	10:	ref_tsk(TASK2, &rtsk)			｜
 *		assert(rtsk.tskstat == TTS_RUN)	｜
 *		assert(rtsk.prcid == PRC1)		｜	... (J-1)(K)
 *		assert(rtsk.actprc == TPRC_NONE)｜	... (J-2)(M)
 *		mact_tsk(TSK_SELF, PRC1)		｜
 *							... (F-1)	｜
 *	11:	ref_tsk(TASK2, &rtsk)			｜
 *		assert(rtsk.tskstat == TTS_RUN)	｜
 *		assert(rtsk.prcid == PRC1)		｜	... (K)
 *		assert(rtsk.actprc == PRC1)		｜	... (L)
 *		ext_tsk()						｜
 *	== TASK2-6（高，6回目）==			｜
 *	12:	mact_tsk(TSK_SELF, TPRC_INI)	｜
 *							... (F-2)(H)｜
 *		ext_tsk()						｜	== TASK2-7（高，7回目）==
 *	== TASK1（続き）==					｜
 *		BARRIER(7)						｜	7:	BARRIER(7)
 *	13:	act_tsk(TASK2)		... (B)		｜
 *		ref_tsk(TASK2, &rtsk)			｜
 *		assert(rtsk.tskstat == TTS_RUN)	｜
 *		assert(rtsk.prcid == PRC2)		｜	... (J-1)(K)
 *		assert(rtsk.actprc == TPRC_NONE)｜	... (J-2)(M)
 *	14: BARRIER(8)						｜	8:	BARRIER(8)
 *										｜		ext_tsk()
 *										｜	== TASK2-8（高，8回目）==
 *	15: BARRIER(9)						｜	9:	BARRIER(9)
 *		ref_tsk(TASK2, &rtsk)			｜
 *		assert(rtsk.tskstat == TTS_RUN)	｜
 *		assert(rtsk.prcid == PRC2)		｜	... (J-3)(K)
 *		assert(rtsk.actprc == TPRC_NONE)｜	... (M)
 *	16:	mact_tsk(TASK2, PRC1)			｜
 *		ref_tsk(TASK2, &rtsk)			｜
 *		assert(rtsk.tskstat == TTS_RUN)	｜
 *		assert(rtsk.prcid == PRC2)		｜	... (K)
 *		assert(rtsk.actprc == PRC1)		｜	... (L)
 *	17:	can_act(TASK2) -> 1				｜
 *		ref_tsk(TASK2, &rtsk)			｜
 *		assert(rtsk.tskstat == TTS_RUN)	｜
 *		assert(rtsk.prcid == PRC2)		｜	... (K)
 *		assert(rtsk.actprc == TPRC_NONE)｜	... (I)(M)
 *	18: BARRIER(10)						｜	10:	BARRIER(10)
 *	19: END								｜	ext_tsk()
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

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

	check_assert(rtsk.actprc == TPRC_NONE);

	ercd = mact_tsk(TASK2, PRC2);
	check_ercd(ercd, E_OK);

	check_point_PRC1(2);
	test_barrier(1);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskstat == TTS_RUN);

	check_assert(rtsk.prcid == PRC2);

	check_assert(rtsk.actprc == TPRC_NONE);

	check_point_PRC1(3);
	test_barrier(2);

	ercd = dly_tsk(TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	check_point_PRC1(4);
	ercd = mact_tsk(TASK2, PRC1);
	check_ercd(ercd, E_OK);

	check_point_PRC1(6);
	ercd = mact_tsk(TASK2, PRC2);
	check_ercd(ercd, E_OK);

	test_barrier(3);

	check_point_PRC1(7);
	ercd = mact_tsk(TASK2, PRC2);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskstat == TTS_RUN);

	check_assert(rtsk.prcid == PRC2);

	check_assert(rtsk.actprc == PRC2);

	test_barrier(4);

	check_point_PRC1(8);
	test_barrier(5);

	ercd = mact_tsk(TASK2, PRC1);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskstat == TTS_RUN);

	check_assert(rtsk.prcid == PRC2);

	check_assert(rtsk.actprc == PRC1);

	ercd = mact_tsk(TASK2, PRC2);
	check_ercd(ercd, E_QOVR);

	ercd = mact_tsk(TASK1, PRC2);
	check_ercd(ercd, E_PAR);

	check_point_PRC1(9);
	test_barrier(6);

	test_barrier(7);

	check_point_PRC1(13);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskstat == TTS_RUN);

	check_assert(rtsk.prcid == PRC2);

	check_assert(rtsk.actprc == TPRC_NONE);

	check_point_PRC1(14);
	test_barrier(8);

	check_point_PRC1(15);
	test_barrier(9);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskstat == TTS_RUN);

	check_assert(rtsk.prcid == PRC2);

	check_assert(rtsk.actprc == TPRC_NONE);

	check_point_PRC1(16);
	ercd = mact_tsk(TASK2, PRC1);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskstat == TTS_RUN);

	check_assert(rtsk.prcid == PRC2);

	check_assert(rtsk.actprc == PRC1);

	check_point_PRC1(17);
	ercd = can_act(TASK2);
	check_ercd(ercd, 1);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskstat == TTS_RUN);

	check_assert(rtsk.prcid == PRC2);

	check_assert(rtsk.actprc == TPRC_NONE);

	check_point_PRC1(18);
	test_barrier(10);

	check_finish_PRC1(19);
	check_assert(false);
}

static uint_t	task2_count = 0;

void
task2(EXINF exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;

	switch (++task2_count) {
	case 1:
		check_point_PRC2(1);
		test_barrier(1);

		check_point_PRC2(2);
		test_barrier(2);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point_PRC1(5);
		ercd = ref_tsk(TASK2, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskstat == TTS_RUN);

		check_assert(rtsk.prcid == PRC1);

		check_assert(rtsk.actprc == TPRC_NONE);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point_PRC2(3);
		test_barrier(3);

		check_point_PRC2(4);
		test_barrier(4);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point_PRC2(5);
		test_barrier(5);

		check_point_PRC2(6);
		test_barrier(6);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point_PRC1(10);
		ercd = ref_tsk(TASK2, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskstat == TTS_RUN);

		check_assert(rtsk.prcid == PRC1);

		check_assert(rtsk.actprc == TPRC_NONE);

		ercd = mact_tsk(TSK_SELF, PRC1);
		check_ercd(ercd, E_OK);

		check_point_PRC1(11);
		ercd = ref_tsk(TASK2, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskstat == TTS_RUN);

		check_assert(rtsk.prcid == PRC1);

		check_assert(rtsk.actprc == PRC1);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 6:
		check_point_PRC1(12);
		ercd = mact_tsk(TSK_SELF, TPRC_INI);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 7:
		check_point_PRC2(7);
		test_barrier(7);

		check_point_PRC2(8);
		test_barrier(8);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 8:
		check_point_PRC2(9);
		test_barrier(9);

		check_point_PRC2(10);
		test_barrier(10);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}
