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
 *  $Id: test_spinlock1.c 265 2021-01-08 06:17:00Z ertl-honda $
 */

/* 
 *		スピンロック機能のテスト(1)
 *
 * 【テストの目的】
 *
 *	スピンロック機能の基本的な振る舞いをテストする．
 *
 * 【テスト項目】
 *
 *	ネイティブスピンロックとエミュレーションされたスピンロックの両方を
 *	使う場合をテストするために，2つのスピンロックに対して，同じテスト
 *	を行う．
 *
 *	(A) loc_spnの振る舞い
 *		(A-1) ロックが取得できるまでループで待つこと［NGKI2178］
 *			  ［NGKI2179］［NGKI2180］
 *		(A-2) ロックの取得を待つ間は，CPUロック解除状態であること
 *			  ［NGKI2120］
 *		(A-3) ロックの取得に成功した場合には，スピンロックは取得されて
 *			  いる状態になること［NGKI2181］
 *		(A-4) ロックの取得に成功した場合には，CPUロック状態へ遷移する
 *			  こと［NGKI2182］
 *	(B) try_spnの振る舞い
 *		(B-1) ロックの取得を試みること［NGKI2192］
 *		(B-2) ロックの取得に成功した場合には，スピンロックは取得されて
 *			  いる状態になること［NGKI2193］
 *		(B-3) ロックの取得に成功した場合には，CPUロック状態へ遷移する
 *			  こと［NGKI2194］
 *		(B-4) ロックの取得に失敗した場合には，E_OBJエラーとなること
 *			  ［NGKI2195］
 *		(B-5) ロックの取得に失敗した時は，CPUロック状態にならないこと
 *		(B-6) ロックが取得できるまで繰り返し呼ぶ間に，割込みが受け付け
 *			  られること
 *	(C) unl_spnの振る舞い
 *		(C-1) ロックを返却し，スピンロックを取得されていない状態とする
 *			  こと［NGKI2203］
 *		(C-2) CPUロック解除状態へ遷移すること［NGKI2204］
 *		(C-3) 対象スピンロックが取得されていない状態である場合には，
 *			  E_ILUSEエラーとなること［NGKI2205］
 *		(C-4) 対象スピンロックが他のプロセッサによって取得されている状
 *			  態である場合には，E_ILUSEエラーとなること［NGKI2205］
 *	(D) ref_spnの振る舞い
 *		(D-1) 対象スピンロックが取得されていない状態である場合には，
 *			  spnstatにTSPN_UNLが返されること［NGKI2214］
 *		(D-2) 対象スピンロックが取得されている状態である場合には，
 *			  spnstatにTSPN_LOCが返されること［NGKI2214］
 *
 * 【使用リソース】
 *
 *	CLS_PRC1: プロセッサ1のみで実行
 *	CLS_PRC2: プロセッサ2のみで実行
 *
 *	TASK1: CLS_PRC1，中優先度タスク，メインタスク，最初から起動
 *	TASK2: CLS_PRC2，中優先度タスク
 *	TASK3: CLS_PRC2，低優先度タスク
 *	SPN1: CLS_PRC1，スピンロック
 *	SPN2: CLS_PRC1，スピンロック，TA_NATIVE属性
 *	ALM1: CLS_PRC1，アラームハンドラ
 *	ALM2: CLS_PRC2，アラームハンドラ
 *
 * 【テストシーケンス】
 *
 *	== START_PRC1 ==					｜	== START_PRC2 ==
 *	== TASK1（中）==					｜
 *	1:	ref_spn(SPN1, &rspn) ... (D-1)	｜
 *		assert(rspn.spnstat == TSPN_UNL)｜
 *		act_tsk(TASK2)					｜	== TASK2（中）==
 *		act_tsk(TASK3)					｜
 *		sta_alm(ALM1, TEST_TIME_CP)		｜
 *		loc_spn(SPN1)					｜		// SPN1が取得されるのを待つ
 *	2:	SET(task2_flag)					｜	1:	WAIT_RESET(task2_flag)
 *		assert(sns_loc() == true)		｜	2:	ref_spn(SPN1, &rspn)
 *							... (A-4)	｜		assert(rspn.spnstat == TSPN_LOC)
 *										｜						... (A-3)(D-2)
 *										｜		sta_alm(ALM2, 2 * TEST_TIME_CP)
 *										｜		loc_spn(SPN1)		... (A-1)
 *										｜	== ALM2-1 ==			... (A-2)
 *		WAIT_RESET(task1_flag)			｜	3:	SET(task1_flag)
 *	3:	unl_spn(SPN1)					｜		RETURN
 *	== ALM1-1 ==						｜	== TASK2（続き）==
 *	4:	RETURN							｜	4:	assert(sns_loc() == true)
 *	== TASK1（続き）==					｜
 *	5:	assert(sns_loc() == false)		｜
 *							... (C-2)	｜
 *		unl_spn(SPN1) -> E_ILUSE		｜
 *							... (C-4)	｜
 *		SET(task2_flag)					｜	5:	WAIT_RESET(task2_flag)
 *										｜	6:	unl_spn(SPN1)
 *										｜		ref_spn(SPN1, &rspn) ... (C-1)
 *		// SPN1が解放されるのを待つ		｜		assert(rspn.spnstat == TSPN_UNL)
 *		WAIT_RESET(task1_flag)			｜		SET(task1_flag)
 *	6:	unl_spn(SPN1) -> E_ILUSE		｜
 *							... (C-3)	｜
 *		sta_alm(ALM1, TEST_TIME_CP)		｜
 *		try_spn(SPN1)		... (B-1)	｜		// SPN1が取得されるのを待つ
 *		SET(task2_flag)					｜		WAIT_RESET(task2_flag)
 *		assert(sns_loc() == true)		｜	7:	ref_spn(SPN1, &rspn)
 *							... (B-3)	｜		assert(rspn.spnstat == TSPN_LOC)
 *										｜							... (B-2)
 *										｜		try_spn(SPN1) -> E_OBJ ... (B-4)
 *										｜		assert(sns_loc() == false)
 *										｜							... (B-5)
 *										｜		sta_alm(ALM2, 2 * TEST_TIME_CP)
 *										｜		DO(while((ercd=try_spn(SPN1)) \
 *										｜							== E_OBJ);)
 *										｜	== ALM2-2 ==			... (B-6)
 *		WAIT_RESET(task1_flag)			｜	8:	SET(task1_flag)
 *	7:	unl_spn(SPN1)					｜		RETURN
 *	== ALM1-2 ==						｜	== TASK2（続き）==
 *	8:	RETURN							｜	9:	assert(ercd == E_OK)
 *	== TASK1（続き）==					｜		assert(sns_loc() == true)
 *										｜		unl_spn(SPN1)
 *	// ここから，SPN2に対するテスト
 *	9:	ref_spn(SPN2, &rspn) ... (D-1)	｜
 *		assert(rspn.spnstat == TSPN_UNL)｜
 *		sta_alm(ALM1, TEST_TIME_CP)		｜
 *		loc_spn(SPN2)					｜		// SPN2が取得されるのを待つ
 *	10:	SET(task2_flag)					｜	10:	WAIT_RESET(task2_flag)
 *		assert(sns_loc() == true)		｜	11:	ref_spn(SPN2, &rspn)
 *							... (A-4)	｜		assert(rspn.spnstat == TSPN_LOC)
 *										｜						... (A-3)(D-2)
 *										｜		sta_alm(ALM2, 2 * TEST_TIME_CP)
 *										｜		loc_spn(SPN2)		... (A-1)
 *										｜	== ALM2-3 ==			... (A-2)
 *		WAIT_RESET(task1_flag)			｜	12:	SET(task1_flag)
 *	11:	unl_spn(SPN2)					｜		RETURN
 *	== ALM1-3 ==						｜	== TASK2（続き）==
 *	12:	RETURN							｜	13:	assert(sns_loc() == true)
 *	== TASK1（続き）==					｜
 *	13:	assert(sns_loc() == false)		｜
 *							... (C-2)	｜
 *		unl_spn(SPN2) -> E_ILUSE		｜
 *							... (C-4)	｜
 *		SET(task2_flag)					｜	14:	WAIT_RESET(task2_flag)
 *										｜	15:	unl_spn(SPN2)
 *										｜		ref_spn(SPN2, &rspn) ... (C-1)
 *		// SPN2が解放されるのを待つ		｜		assert(rspn.spnstat == TSPN_UNL)
 *		WAIT_RESET(task1_flag)			｜		SET(task1_flag)
 *	14:	unl_spn(SPN2) -> E_ILUSE		｜
 *							... (C-3)	｜
 *		sta_alm(ALM1, TEST_TIME_CP)		｜
 *		try_spn(SPN2)		... (B-1)	｜		// SPN2が取得されるのを待つ
 *		SET(task2_flag)					｜		WAIT_RESET(task2_flag)
 *		assert(sns_loc() == true)		｜	16:	ref_spn(SPN2, &rspn)
 *							... (B-3)	｜		assert(rspn.spnstat == TSPN_LOC)
 *										｜							... (B-2)
 *										｜		try_spn(SPN2) -> E_OBJ ... (B-4)
 *										｜		assert(sns_loc() == false)
 *										｜							... (B-5)
 *										｜		sta_alm(ALM2, 2 * TEST_TIME_CP)
 *										｜		DO(while((ercd=try_spn(SPN2)) \
 *										｜							== E_OBJ);)
 *										｜	== ALM2-4 ==			... (B-6)
 *		WAIT_RESET(task1_flag)			｜	17:	SET(task1_flag)
 *	15:	unl_spn(SPN2)					｜		RETURN
 *	== ALM1-4 ==						｜	== TASK2（続き）==
 *	16:	RETURN							｜	18:	assert(ercd == E_OK)
 *	== TASK1（続き）==					｜		assert(sns_loc() == true)
 *										｜		unl_spn(SPN2)
 *										｜		ext_tsk()
 *										｜	== TASK3（低）==
 *		WAIT_RESET(task1_flag)			｜	19:	SET(task1_flag)
 *	17:	END								｜		ext_tsk()
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"

volatile bool_t	task1_flag = false;
volatile bool_t	task2_flag = false;

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

static uint_t	alarm1_count = 0;

void
alarm1_handler(EXINF exinf)
{

	switch (++alarm1_count) {
	case 1:
		check_point_PRC1(4);
		return;

		check_assert(false);

	case 2:
		check_point_PRC1(8);
		return;

		check_assert(false);

	case 3:
		check_point_PRC1(12);
		return;

		check_assert(false);

	case 4:
		check_point_PRC1(16);
		return;

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	alarm2_count = 0;

void
alarm2_handler(EXINF exinf)
{

	switch (++alarm2_count) {
	case 1:
		check_point_PRC2(3);
		SET(task1_flag);

		return;

		check_assert(false);

	case 2:
		check_point_PRC2(8);
		SET(task1_flag);

		return;

		check_assert(false);

	case 3:
		check_point_PRC2(12);
		SET(task1_flag);

		return;

		check_assert(false);

	case 4:
		check_point_PRC2(17);
		SET(task1_flag);

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
	T_RSPN	rspn;

	test_start(__FILE__);

	check_point_PRC1(1);
	ercd = ref_spn(SPN1, &rspn);
	check_ercd(ercd, E_OK);

	check_assert(rspn.spnstat == TSPN_UNL);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = sta_alm(ALM1, TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	ercd = loc_spn(SPN1);
	check_ercd(ercd, E_OK);

	check_point_PRC1(2);
	SET(task2_flag);

	check_assert(sns_loc() == true);

	WAIT_RESET(task1_flag);

	check_point_PRC1(3);
	ercd = unl_spn(SPN1);
	check_ercd(ercd, E_OK);

	check_point_PRC1(5);
	check_assert(sns_loc() == false);

	ercd = unl_spn(SPN1);
	check_ercd(ercd, E_ILUSE);

	SET(task2_flag);

	WAIT_RESET(task1_flag);

	check_point_PRC1(6);
	ercd = unl_spn(SPN1);
	check_ercd(ercd, E_ILUSE);

	ercd = sta_alm(ALM1, TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	ercd = try_spn(SPN1);
	check_ercd(ercd, E_OK);

	SET(task2_flag);

	check_assert(sns_loc() == true);

	WAIT_RESET(task1_flag);

	check_point_PRC1(7);
	ercd = unl_spn(SPN1);
	check_ercd(ercd, E_OK);

	check_point_PRC1(9);
	ercd = ref_spn(SPN2, &rspn);
	check_ercd(ercd, E_OK);

	check_assert(rspn.spnstat == TSPN_UNL);

	ercd = sta_alm(ALM1, TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	ercd = loc_spn(SPN2);
	check_ercd(ercd, E_OK);

	check_point_PRC1(10);
	SET(task2_flag);

	check_assert(sns_loc() == true);

	WAIT_RESET(task1_flag);

	check_point_PRC1(11);
	ercd = unl_spn(SPN2);
	check_ercd(ercd, E_OK);

	check_point_PRC1(13);
	check_assert(sns_loc() == false);

	ercd = unl_spn(SPN2);
	check_ercd(ercd, E_ILUSE);

	SET(task2_flag);

	WAIT_RESET(task1_flag);

	check_point_PRC1(14);
	ercd = unl_spn(SPN2);
	check_ercd(ercd, E_ILUSE);

	ercd = sta_alm(ALM1, TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	ercd = try_spn(SPN2);
	check_ercd(ercd, E_OK);

	SET(task2_flag);

	check_assert(sns_loc() == true);

	WAIT_RESET(task1_flag);

	check_point_PRC1(15);
	ercd = unl_spn(SPN2);
	check_ercd(ercd, E_OK);

	WAIT_RESET(task1_flag);

	check_finish_PRC1(17);
	check_assert(false);
}

void
task2(EXINF exinf)
{
	ER_UINT	ercd;
	T_RSPN	rspn;

	check_point_PRC2(1);
	WAIT_RESET(task2_flag);

	check_point_PRC2(2);
	ercd = ref_spn(SPN1, &rspn);
	check_ercd(ercd, E_OK);

	check_assert(rspn.spnstat == TSPN_LOC);

	ercd = sta_alm(ALM2, 2 * TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	ercd = loc_spn(SPN1);
	check_ercd(ercd, E_OK);

	check_point_PRC2(4);
	check_assert(sns_loc() == true);

	check_point_PRC2(5);
	WAIT_RESET(task2_flag);

	check_point_PRC2(6);
	ercd = unl_spn(SPN1);
	check_ercd(ercd, E_OK);

	ercd = ref_spn(SPN1, &rspn);
	check_ercd(ercd, E_OK);

	check_assert(rspn.spnstat == TSPN_UNL);

	SET(task1_flag);

	WAIT_RESET(task2_flag);

	check_point_PRC2(7);
	ercd = ref_spn(SPN1, &rspn);
	check_ercd(ercd, E_OK);

	check_assert(rspn.spnstat == TSPN_LOC);

	ercd = try_spn(SPN1);
	check_ercd(ercd, E_OBJ);

	check_assert(sns_loc() == false);

	ercd = sta_alm(ALM2, 2 * TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	while((ercd=try_spn(SPN1)) == E_OBJ);;

	check_point_PRC2(9);
	check_assert(ercd == E_OK);

	check_assert(sns_loc() == true);

	ercd = unl_spn(SPN1);
	check_ercd(ercd, E_OK);

	check_point_PRC2(10);
	WAIT_RESET(task2_flag);

	check_point_PRC2(11);
	ercd = ref_spn(SPN2, &rspn);
	check_ercd(ercd, E_OK);

	check_assert(rspn.spnstat == TSPN_LOC);

	ercd = sta_alm(ALM2, 2 * TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	ercd = loc_spn(SPN2);
	check_ercd(ercd, E_OK);

	check_point_PRC2(13);
	check_assert(sns_loc() == true);

	check_point_PRC2(14);
	WAIT_RESET(task2_flag);

	check_point_PRC2(15);
	ercd = unl_spn(SPN2);
	check_ercd(ercd, E_OK);

	ercd = ref_spn(SPN2, &rspn);
	check_ercd(ercd, E_OK);

	check_assert(rspn.spnstat == TSPN_UNL);

	SET(task1_flag);

	WAIT_RESET(task2_flag);

	check_point_PRC2(16);
	ercd = ref_spn(SPN2, &rspn);
	check_ercd(ercd, E_OK);

	check_assert(rspn.spnstat == TSPN_LOC);

	ercd = try_spn(SPN2);
	check_ercd(ercd, E_OBJ);

	check_assert(sns_loc() == false);

	ercd = sta_alm(ALM2, 2 * TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	while((ercd=try_spn(SPN2)) == E_OBJ);;

	check_point_PRC2(18);
	check_assert(ercd == E_OK);

	check_assert(sns_loc() == true);

	ercd = unl_spn(SPN2);
	check_ercd(ercd, E_OK);

	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task3(EXINF exinf)
{
	ER_UINT	ercd;

	check_point_PRC2(19);
	SET(task1_flag);

	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}
