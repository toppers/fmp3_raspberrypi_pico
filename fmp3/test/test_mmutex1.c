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
 *  $Id: test_mmutex1.c 265 2021-01-08 06:17:00Z ertl-honda $
 */

/*
 *		マルチプロセッサ対応のミューテックスのテスト(1)
 *
 * 【テストの目的】
 *
 *	ミューテックスのマルチプロセッサ特有の振舞いをテストする．
 *
 * 【テスト項目】
 *
 *	(A) unl_mtxによるミューテックスの解放で，他プロセッサ上のタスクが
 *		ミューテックスを獲得し，待ち解除される場合
 *		(A-1) 両プロセッサでタスク切り換えが起こらない場合
 *		(A-2) 自プロセッサでのみタスク切り換えが起こる場合
 *		(A-3) 他プロセッサでのみタスク切り換えが起こる場合
 *		(A-4) 両プロセッサでタスク切り換えが起こる場合
 *	(B) ini_mtxにより，優先度上限ミューテックスを獲得している他プロセッ
 *		サ上のタスクがミューテックスを解放させられ，優先度が下がる場合
 *		(B-1) 他プロセッサでタスク切り換えが起こらない場合
 *		(B-2) 他プロセッサでタスク切り換えが起こる場合
 *
 * 【使用リソース】
 *
 *	CLS_PRC1: プロセッサ1のみで実行
 *	CLS_PRC2: プロセッサ2のみで実行
 *
 *	TASK1: CLS_PRC1，低優先度タスク，メインタスク，最初から起動
 *	TASK2: CLS_PRC1，低優先度タスク
 *	TASK3: CLS_PRC1，中優先度タスク
 *	TASK4: CLS_PRC2，低優先度タスク
 *	TASK5: CLS_PRC2，低優先度タスク
 *	TASK6: CLS_PRC2，高優先度タスク
 *	MTX1: CLS_PRC1，ミューテックス（TA_CEILING属性，上限は中優先度）
 *
 * 【テストシーケンス】
 *
 *	== START_PRC1 ==					｜	== START_PRC2 ==
 *	== TASK1（低）==					｜
 *	1:	act_tsk(TASK2)					｜
 *		loc_mtx(MTX1)					｜
 *		act_tsk(TASK4)					｜	== TASK4（低）==
 *		act_tsk(TASK5)					｜	1:	loc_mtx(MTX1)
 *		// TASK5の起動を待つ			｜	== TASK5（低）==
 *		WAIT_RESET(task1_flag)			｜	2:	SET(task1_flag)
 *										｜		WAIT_RESET(task5_flag)
 *	2:	act_tsk(TASK6)					｜	== TASK6-1（高，1回目）==
 *		// TASK6の起動を待つ			｜	3:	SET(task5_flag)
 *		WAIT_RESET(task1_flag)			｜		SET(task1_flag)
 *	3:	unl_mtx(MTX1)		... (A-1)	｜
 *		// MTX1の解放後もTASK1とTASK6が実行されていることを確認する
 *		BARRIER(1)						｜		BARRIER(1)
 *										｜	4:	ext_tsk()
 *										｜	== TASK4（続き）==
 *		// MTX1の解放を待つ				｜	5:	unl_mtx(MTX1)
 *	4:	WAIT_RESET(task1_flag)			｜		SET(task1_flag)
 *	5:	loc_mtx(MTX1)					｜
 *		act_tsk(TASK3)					｜		// MTX1の獲得を待つ
 *		SET(task4_flag)					｜		WAIT_RESET(task4_flag)
 *										｜	6:	loc_mtx(MTX1)
 *		// TASK5の実行を待つ			｜	== TASK5（続き）==
 *		WAIT_RESET(task1_flag)			｜	7:	SET(task1_flag)
 *										｜		WAIT_RESET(task5_flag)
 *	6:	act_tsk(TASK6)					｜	== TASK6-2（高，2回目）==
 *		// TASK6の起動を待つ			｜	8:	SET(task5_flag)
 *		WAIT_RESET(task1_flag)			｜		SET(task1_flag)
 *	7:	unl_mtx(MTX1)		... (A-2)	｜
 *	== TASK3-1（中，1回目）==			｜
 *		// MTX1の解放後にTASK3とTASK6が実行されていることを確認する
 *	8:	BARRIER(2)						｜		BARRIER(2)
 *	9:	ext_tsk()						｜	9:	ext_tsk()
 *	== TASK1（続き）==					｜	== TASK4（続き）==
 *		// MTX1の解放を待つ				｜	10:	unl_mtx(MTX1)
 *	10:	WAIT_RESET(task1_flag)			｜		SET(task1_flag)
 *	11:	loc_mtx(MTX1)					｜		// MTX1の獲得を待つ
 *		SET(task4_flag)					｜		WAIT_RESET(task4_flag)
 *										｜	11:	loc_mtx(MTX1)
 *		// TASK5の実行を待つ			｜	== TASK5（続き）==
 *		WAIT_RESET(task1_flag)			｜	12:	SET(task1_flag)
 *										｜		WAIT_RESET(task5_flag)
 *	12:	unl_mtx(MTX1)		... (A-3)	｜	== TASK4（続き）==
 *		// MTX1の解放後にTASK1とTASK4が実行されていることを確認する
 *		BARRIER(3)						｜	13:	BARRIER(3)
 *										｜	14:	SET(task5_flag)
 *		// MTX1の解放を待つ				｜		unl_mtx(MTX1)
 *	13:	WAIT_RESET(task1_flag)			｜		SET(task1_flag)
 *	14:	loc_mtx(MTX1)					｜
 *		act_tsk(TASK3)					｜		// MTX1の獲得を待つ
 *		SET(task4_flag)					｜		WAIT_RESET(task4_flag)
 *										｜	15:	loc_mtx(MTX1)
 *		// TASK5の実行を待つ			｜	== TASK5（続き）==
 *		WAIT_RESET(task1_flag)			｜	16:	SET(task1_flag)
 *										｜		WAIT_RESET(task5_flag)
 *	15:	unl_mtx(MTX1)		... (A-4)	｜	== TASK4（続き）==
 *	== TASK3-2（中，2回目）==			｜
 *		// MTX1の解放後にTASK3とTASK4が実行されていることを確認する
 *	16:	BARRIER(4)						｜	17:	BARRIER(4)
 *										｜	18:	SET(task5_flag)
 *	17:	ext_tsk()						｜		unl_mtx(MTX1)
 *	== TASK1（続き）==					｜
 *		// MTX1の解放を待つ				｜
 *	18:	WAIT_RESET(task1_flag)			｜		SET(task1_flag)
 *	19:	loc_mtx(MTX1)					｜		// MTX1の獲得を待つ
 *		SET(task4_flag)					｜		WAIT_RESET(task4_flag)
 *										｜	19:	ini_mtx(MTX1)
 *		// MTX1の初期化後もTASK1とTASK4が実行されていることを確認する
 *		BARRIER(5)						｜		BARRIER(5)
 *	20:	unl_mtx(MTX1) -> E_OBJ			｜
 *		loc_mtx(MTX1)					｜
 *		act_tsk(TASK3)					｜		// MTX1の獲得を待つ
 *		SET(task4_flag)					｜		WAIT_RESET(task4_flag)
 *		WAIT_RESET(task1_flag)			｜
 *	== TASK3-3（中，3回目）==			｜	20:	ini_mtx(MTX1)
 *		// MTX1の初期化後にTASK3とTASK4が実行されていることを確認する
 *	21:	BARRIER(6)						｜		BARRIER(6)
 *		SET(task1_flag)					｜
 *		ext_tsk()						｜	21:	ext_tsk()
 *	== TASK1（続き）==					｜	== TASK5（続き）==
 *	22:	END								｜	22:	ext_tsk()
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"

volatile bool_t	task1_flag = false;
volatile bool_t	task4_flag = false;
volatile bool_t	task5_flag = false;

void
task2(EXINF exinf)
{
	check_assert(false);
}

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

void
task1(EXINF exinf)
{
	ER_UINT	ercd;

	test_start(__FILE__);

	check_point_PRC1(1);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	WAIT_RESET(task1_flag);

	check_point_PRC1(2);
	ercd = act_tsk(TASK6);
	check_ercd(ercd, E_OK);

	WAIT_RESET(task1_flag);

	check_point_PRC1(3);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	test_barrier(1);

	check_point_PRC1(4);
	WAIT_RESET(task1_flag);

	check_point_PRC1(5);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	SET(task4_flag);

	WAIT_RESET(task1_flag);

	check_point_PRC1(6);
	ercd = act_tsk(TASK6);
	check_ercd(ercd, E_OK);

	WAIT_RESET(task1_flag);

	check_point_PRC1(7);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point_PRC1(10);
	WAIT_RESET(task1_flag);

	check_point_PRC1(11);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	SET(task4_flag);

	WAIT_RESET(task1_flag);

	check_point_PRC1(12);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	test_barrier(3);

	check_point_PRC1(13);
	WAIT_RESET(task1_flag);

	check_point_PRC1(14);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	SET(task4_flag);

	WAIT_RESET(task1_flag);

	check_point_PRC1(15);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point_PRC1(18);
	WAIT_RESET(task1_flag);

	check_point_PRC1(19);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	SET(task4_flag);

	test_barrier(5);

	check_point_PRC1(20);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OBJ);

	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	SET(task4_flag);

	WAIT_RESET(task1_flag);

	check_finish_PRC1(22);
	check_assert(false);
}

static uint_t	task3_count = 0;

void
task3(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task3_count) {
	case 1:
		check_point_PRC1(8);
		test_barrier(2);

		check_point_PRC1(9);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point_PRC1(16);
		test_barrier(4);

		check_point_PRC1(17);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point_PRC1(21);
		test_barrier(6);

		SET(task1_flag);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

void
task4(EXINF exinf)
{
	ER_UINT	ercd;

	check_point_PRC2(1);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point_PRC2(5);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	SET(task1_flag);

	WAIT_RESET(task4_flag);

	check_point_PRC2(6);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point_PRC2(10);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	SET(task1_flag);

	WAIT_RESET(task4_flag);

	check_point_PRC2(11);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point_PRC2(13);
	test_barrier(3);

	check_point_PRC2(14);
	SET(task5_flag);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	SET(task1_flag);

	WAIT_RESET(task4_flag);

	check_point_PRC2(15);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point_PRC2(17);
	test_barrier(4);

	check_point_PRC2(18);
	SET(task5_flag);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	SET(task1_flag);

	WAIT_RESET(task4_flag);

	check_point_PRC2(19);
	ercd = ini_mtx(MTX1);
	check_ercd(ercd, E_OK);

	test_barrier(5);

	WAIT_RESET(task4_flag);

	check_point_PRC2(20);
	ercd = ini_mtx(MTX1);
	check_ercd(ercd, E_OK);

	test_barrier(6);

	check_point_PRC2(21);
	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task5(EXINF exinf)
{
	ER_UINT	ercd;

	check_point_PRC2(2);
	SET(task1_flag);

	WAIT_RESET(task5_flag);

	check_point_PRC2(7);
	SET(task1_flag);

	WAIT_RESET(task5_flag);

	check_point_PRC2(12);
	SET(task1_flag);

	WAIT_RESET(task5_flag);

	check_point_PRC2(16);
	SET(task1_flag);

	WAIT_RESET(task5_flag);

	check_point_PRC2(22);
	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

static uint_t	task6_count = 0;

void
task6(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task6_count) {
	case 1:
		check_point_PRC2(3);
		SET(task5_flag);

		SET(task1_flag);

		test_barrier(1);

		check_point_PRC2(4);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point_PRC2(8);
		SET(task5_flag);

		SET(task1_flag);

		test_barrier(2);

		check_point_PRC2(9);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}
