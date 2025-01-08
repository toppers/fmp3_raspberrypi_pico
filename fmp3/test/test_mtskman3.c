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
 *  $Id: test_mtskman3.c 265 2021-01-08 06:17:00Z ertl-honda $
 */

/* 
 *		マルチプロセッサ対応のタスク管理機能のテスト(3)
 *
 * 【テストの目的】
 *
 *	mig_tskの振る舞いをテストする．
 *
 * 【テスト項目】
 *
 *	(A) 対象タスクがprcidに割り付けられること［NGKI1155］
 *		(A-1) 自タスクが対象で，マイグレート先でタスク切り換えが発生す
 *			  る場合
 *		(A-2) 自タスクが対象で，マイグレート先でタスク切り換えが発生し
 *			  ない場合．この時，同じ優先度内で最低優先順位になること
 *			  ［NGKI1156］
 *		(A-3) 自タスクが対象で，割り付けられているプロセッサとマイグレー
 *			  ト先が同一の場合．この時，同じ優先度内で最低優先順位にな
 *			  ること［NGKI1156］
 *		(A-4) 対象タスクが実行可能状態で，マイグレート先でタスク切り換
 *			  えが発生する場合
 *		(A-5) 対象タスクが実行可能状態で，マイグレート先でタスク切り換
 *			  えが発生しない場合．この時，同じ優先度内で最低優先順位に
 *			  なること［NGKI1156］
 *		(A-6) 対象タスクが実行可能状態で，割り付けられているプロセッサ
 *			  とマイグレート先が同一の場合．この時，同じ優先度内で最低
 *			  優先順位になること［NGKI1156］
 *		(A-7) 対象タスクがタイムアウトなしの待ち状態の場合
 *		(A-8) 対象タスクがタイムアウトありの待ち状態の場合
 *		(A-9) 対象タスクが強制待ち状態の場合
 *		(A-10) 対象タスクがタイムアウトなしの二重待ち状態の場合
 *		(A-11) 対象タスクがタイムアウトありの二重待ち状態の場合
 *		(A-12) 対象タスクが休止状態の場合
 *	(B) 対象タスクが，他プロセッサに割り付けられている場合には，E_OBJ
 *		エラーとなること［NGKI1157］
 *	(C) tskidにTSK_SELFを指定すると，自タスクを対象とすること［NGKI1158］
 *	(D) ディスパッチ保留状態で，自タスクを対象とした場合，E_CTXエラー
 *		となること［NGKI1159］
 *	(E) マイグレート先が，対象タスクの属するクラスの割付け可能プロセッ
 *		サに含まれていない場合に，E_PARエラーとなること［NGKI1160］
 *	(F) prcidにTPRC_INIを指定すると，割付けプロセッサを，初期割付けプ
 *		ロセッサに変更すること［NGKI1161］
 *
 * 【使用リソース】
 *
 *	CLS_ALL_PRC1: 両プロセッサで実行，初期割付けはプロセッサ1
 *	CLS_ALL_PRC2: 両プロセッサで実行，初期割付けはプロセッサ2
 *
 *	TASK1: CLS_ALL_PRC1，中優先度タスク，最初から起動
 *	TASK2: CLS_ALL_PRC1: 低優先度タスク
 *	TASK3: CLS_PRC2: 中優先度タスク
 *	TASK4: CLS_ALL_PRC2: 低優先度タスク
 *	TASK5: CLS_PRC2: 低優先度タスク
 *
 * 【テストシーケンス】
 *
 *	== START_PRC1 ==					｜	== START_PRC2 ==
 *	== TASK1-1（中，1回目）==			｜
 *	1:	act_tsk(TASK2)					｜
 *		act_tsk(TASK3)					｜	== TASK3（中）==
 *		act_tsk(TASK4)					｜
 *		// TASK3の起動を待つ			｜
 *		WAIT_RESET(task1_flag)			｜	1:	SET(task1_flag)
 *	2:	mig_tsk(TSK_SELF, PRC2)			｜
 *							... (A-2)(C)｜
 *	== TASK2（低）==					｜
 *		// mig_tskの発行後にTASK2とTASK3が実行されていることを確認する
 *	3:	BARRIER(1)						｜		BARRIER(1)
 *		WAIT_RESET(task2_flag)			｜	2:	mig_tsk(TASK3, PRC2) ... (A-3)
 *										｜	== TASK1-1（続き）==
 *	== TASK1-1（続き）==				｜	3:	mig_tsk(TASK1, TPRC_INI)
 *										｜							... (A-1)(F)
 *										｜	== TASK3（続き）==
 *		// mig_tskの発行後にTASK1とTASK3が実行されていることを確認する
 *	4:	BARRIER(2)						｜	4:	BARRIER(2)
 *	5:	SET(task2_flag)					｜
 *		mig_tsk(TASK2, PRC2) ... (A-5)	｜
 *		// mig_tskの発行後もTASK1とTASK3が実行されていることを確認する
 *		BARRIER(3)						｜	5:	BARRIER(3)
 *	6:	ext_tsk()						｜	6:	slp_tsk()
 *										｜	== TASK4（低）==
 *										｜	7:	rot_rdq(TPRI_SELF)
 *										｜	== TASK2（続き）==
 *										｜	8:	wup_tsk(TASK3)
 *										｜	== TASK3（続き）==
 *										｜	9:	mig_tsk(TASK2, PRC2) ... (A-6)
 *										｜		slp_tsk()
 *										｜	== TASK4（続き）==
 *										｜	10:	slp_tsk()
 *										｜	== TASK2（続き）==
 *										｜	11:	wup_tsk(TASK3)
 *										｜	== TASK3（続き）==
 *	== TASK2（続き）==					｜	12:	mig_tsk(TASK2, PRC1) ... (A-4)
 *		// mig_tskの発行後にTASK2とTASK3が実行されていることを確認する
 *	7:	BARRIER(4)						｜		BARRIER(4)
 *	8:	slp_tsk()						｜	13:	mig_tsk(TASK4, PRC1) ... (A-7)
 *	== TASK4（続き）==					｜		wup_tsk(TASK4)
 *	9:	wup_tsk(TASK2)					｜		slp_tsk()
 *		tslp_tsk(5 * TEST_TIME_CP) \	｜
 *							-> E_TMOUT	｜
 *	== TASK2（続き）==					｜
 *	10:	dly_tsk(TEST_TIME_CP)			｜
 *		ref_tsk(TASK4, &rtsk)			｜
 *		DO(lefttmo1 = rtsk.lefttmo)		｜
 *		mig_tsk(TASK4, PRC2) ... (A-8)	｜
 *		ref_tsk(TASK4, &rtsk)			｜
 *		assert(rtsk.lefttmo <= lefttmo1)｜
 *		slp_tsk()						｜
 *										｜	== TASK4（続き）==
 *										｜	14:	wup_tsk(TASK3)
 *										｜	== TASK3（続き）==
 *										｜	15:	sus_tsk(TASK4)
 *										｜		mig_tsk(TASK4, PRC1) ... (A-9)
 *	== TASK4（続き）==					｜		rsm_tsk(TASK4)
 *	11:	wup_tsk(TASK2)					｜		slp_tsk()
 *		slp_tsk()						｜
 *	== TASK2（続き）==					｜
 *	12:	sus_tsk(TASK4)					｜
 *		mig_tsk(TASK4, PRC2) ... (A-10)	｜
 *		rsm_tsk(TASK4)					｜
 *		wup_tsk(TASK4)					｜	== TASK4（続き）==
 *		ext_tsk()						｜	16:	act_tsk(TASK5)
 *										｜		tslp_tsk(TEST_TIME_CP * 5) \
 *										｜							-> E_TMOUT
 *										｜	== TASK5（低）==
 *										｜	17:	sus_tsk(TASK4)
 *										｜		mig_tsk(TASK4, PRC1) ... (A-11)
 *										｜		rsm_tsk(TASK4)
 *										｜		ext_tsk()
 *	== TASK4（続き）==					｜
 *	13:	mig_tsk(TASK1, PRC2) ... (A-12)	｜
 *		act_tsk(TASK1)					｜	== TASK1-2（中，2回目）==
 *										｜	18:	mig_tsk(TASK4, PRC1) -> E_OBJ
 *										｜								... (B)	
 *										｜		mig_tsk(TASK3, PRC1) -> E_PAR
 *										｜								... (E)	
 *										｜		dis_dsp()
 *										｜		mig_tsk(TASK1, PRC1) -> E_CTX
 *		// TASK1の処理終了を待つ		｜								... (D)	
 *		WAIT_RESET(task4_flag)			｜	19:	SET(task4_flag)
 *	14: END								｜		ext_tsk()
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"

volatile bool_t	task1_flag = false;
volatile bool_t	task2_flag = false;
volatile bool_t	task4_flag = false;
volatile TMO	lefttmo1;

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

static uint_t	task1_count = 0;

void
task1(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task1_count) {
	case 1:
		test_start(__FILE__);

		check_point_PRC1(1);
		ercd = act_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		WAIT_RESET(task1_flag);

		check_point_PRC1(2);
		ercd = mig_tsk(TSK_SELF, PRC2);
		check_ercd(ercd, E_OK);

		check_point_PRC2(3);
		ercd = mig_tsk(TASK1, TPRC_INI);
		check_ercd(ercd, E_OK);

		check_point_PRC1(4);
		test_barrier(2);

		check_point_PRC1(5);
		SET(task2_flag);

		ercd = mig_tsk(TASK2, PRC2);
		check_ercd(ercd, E_OK);

		test_barrier(3);

		check_point_PRC1(6);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point_PRC2(18);
		ercd = mig_tsk(TASK4, PRC1);
		check_ercd(ercd, E_OBJ);

		ercd = mig_tsk(TASK3, PRC1);
		check_ercd(ercd, E_PAR);

		ercd = dis_dsp();
		check_ercd(ercd, E_OK);

		ercd = mig_tsk(TASK1, PRC1);
		check_ercd(ercd, E_CTX);

		check_point_PRC2(19);
		SET(task4_flag);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

void
task2(EXINF exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;

	check_point_PRC1(3);
	test_barrier(1);

	WAIT_RESET(task2_flag);

	check_point_PRC2(8);
	ercd = wup_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point_PRC2(11);
	ercd = wup_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point_PRC1(7);
	test_barrier(4);

	check_point_PRC1(8);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point_PRC1(10);
	ercd = dly_tsk(TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK4, &rtsk);
	check_ercd(ercd, E_OK);

	lefttmo1 = rtsk.lefttmo;

	ercd = mig_tsk(TASK4, PRC2);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK4, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.lefttmo <= lefttmo1);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point_PRC1(12);
	ercd = sus_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = mig_tsk(TASK4, PRC2);
	check_ercd(ercd, E_OK);

	ercd = rsm_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = wup_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task3(EXINF exinf)
{
	ER_UINT	ercd;

	check_point_PRC2(1);
	SET(task1_flag);

	test_barrier(1);

	check_point_PRC2(2);
	ercd = mig_tsk(TASK3, PRC2);
	check_ercd(ercd, E_OK);

	check_point_PRC2(4);
	test_barrier(2);

	check_point_PRC2(5);
	test_barrier(3);

	check_point_PRC2(6);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point_PRC2(9);
	ercd = mig_tsk(TASK2, PRC2);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point_PRC2(12);
	ercd = mig_tsk(TASK2, PRC1);
	check_ercd(ercd, E_OK);

	test_barrier(4);

	check_point_PRC2(13);
	ercd = mig_tsk(TASK4, PRC1);
	check_ercd(ercd, E_OK);

	ercd = wup_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point_PRC2(15);
	ercd = sus_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = mig_tsk(TASK4, PRC1);
	check_ercd(ercd, E_OK);

	ercd = rsm_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task4(EXINF exinf)
{
	ER_UINT	ercd;

	check_point_PRC2(7);
	ercd = rot_rdq(TPRI_SELF);
	check_ercd(ercd, E_OK);

	check_point_PRC2(10);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point_PRC1(9);
	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = tslp_tsk(5 * TEST_TIME_CP);
	check_ercd(ercd, E_TMOUT);

	check_point_PRC2(14);
	ercd = wup_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point_PRC1(11);
	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point_PRC2(16);
	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	ercd = tslp_tsk(TEST_TIME_CP * 5);
	check_ercd(ercd, E_TMOUT);

	check_point_PRC1(13);
	ercd = mig_tsk(TASK1, PRC2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK1);
	check_ercd(ercd, E_OK);

	WAIT_RESET(task4_flag);

	check_finish_PRC1(14);
	check_assert(false);
}

void
task5(EXINF exinf)
{
	ER_UINT	ercd;

	check_point_PRC2(17);
	ercd = sus_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = mig_tsk(TASK4, PRC1);
	check_ercd(ercd, E_OK);

	ercd = rsm_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}
