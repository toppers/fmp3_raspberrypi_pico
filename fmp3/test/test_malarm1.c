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
 *  $Id: test_malarm1.c 265 2021-01-08 06:17:00Z ertl-honda $
 */

/*
 *		マルチプロセッサ対応のアラーム通知のテスト(1)
 *
 * 【テストの目的】
 *
 *	他のプロセッサに割り付けられたアラーム通知に対するsta_almの振る舞
 *	いと，msta_almの振る舞いをテストする．あわせて，ref_almについても
 *	テストする．
 *
 * 【テスト項目】
 *
 *	(A) 他のプロセッサに割り付けられたアラーム通知に対するsta_alm
 *		(A-1) アラーム通知が動作していない場合
 *		(A-2) アラーム通知が動作している場合
 *	(B) 自プロセッサに割り付けられたアラーム通知に対するmsta_alm
 *		(B-1) アラーム通知が動作しておらず，割付けプロセッサが変化しな
 *			  い場合
 *		(B-2) アラーム通知が動作しておらず，割付けプロセッサが変する場
 *			  合
 *		(B-3) アラーム通知が動作しており，割付けプロセッサが変化しない
 *			  場合
 *		(B-4) アラーム通知が動作しており，割付けプロセッサが変する場合
 *	(C) 他プロセッサに割り付けられたアラーム通知に対するmsta_alm
 *		(C-1) アラーム通知が動作しておらず，割付けプロセッサが変化しな
 *			  い場合
 *		(C-2) アラーム通知が動作しておらず，割付けプロセッサが変する場
 *			  合
 *		(C-3) アラーム通知が動作しており，割付けプロセッサが変化しない
 *			  場合
 *		(C-4) アラーム通知が動作しており，割付けプロセッサが変する場合
 *	(D) アラームハンドラから自アラーム通知に対するmsta_alm
 *		(D-1) 割付けプロセッサが変化しない場合
 *		(D-2) 割付けプロセッサが変する場合
 *
 *	以上の条件のテストにより，msta_almに関する以下を仕様が満たされてい
 *	ることを確認する．
 *
 *	(E) 割付けプロセッサが指定したプロセッサに変更された後，アラーム通
 *		知が動作している状態となること［NGKI2554］
 *	(F) 通知時刻は，almtimで指定した相対時間後に設定されること［NGKI2555］
 *	(G) アラーム通知が動作していれば，割付けプロセッサが指定したプロセッ
 *		サに変更された後，通知時刻の再設定が行われること［NGKI2556］
 *	(H) 対象アラーム通知が実行中である場合の振る舞い
 *		(H-1) 割付けプロセッサを変更しても，実行中のアラーム通知を実行
 *			  するプロセッサは変更されないこと［NGKI2557］
 *		(H-2) 次に通知される時には，変更後の割付けプロセッサで対象アラー
 *			  ム通知が実行されること［NGKI2558］
 *	(I) prcidにTPRC_INIを指定すると，割付けプロセッサを，初期割付けプ
 *		ロセッサに変更すること［NGKI2560］
 *	(J) タスクコンテキストからも非タスクコンテキストからも呼び出せるこ
 *		と［NGKI3544］
 *		(J-1) タスクコンテキストから呼び出せること
 *		(J-2) 非タスクコンテキストから呼び出せること
 *
 * 【使用リソース】
 *
 *	CLS_ALL_PRC1: 両プロセッサで実行，初期割付けはプロセッサ1
 *	CLS_PRC1: プロセッサ1のみで実行
 *	CLS_PRC2: プロセッサ2のみで実行
 *
 *	TASK1: CLS_PRC1，中優先度タスク，メインタスク，最初から起動
 *	ALM1: CLS_PRC2，アラームハンドラ
 *	ALM2: CLS_ALL_PRC1，アラームハンドラ
 *
 * 【テストシーケンス】
 *
 *	== START_PRC1 ==					｜	== START_PRC2 ==
 *	== TASK1（中）==					｜
 *	1:	sta_alm(ALM1, RELTIM1)			｜
 *							... (A-1)	｜
 *		ref_alm(ALM1, &ralm)			｜
 *		assert(ralm.almstat == TALM_STA)｜
 *		assert(ralm.lefttim <= RELTIM1)	｜
 *		assert(ralm.prcid == PRC2)		｜
 *		slp_tsk()						｜
 *										｜	== ALM1-1 ==
 *	== TASK1（続き）==					｜	1:	wup_tsk(TASK1)
 *	2:	sta_alm(ALM1, RELTIM2)			｜		RETURN
 *		ref_alm(ALM1, &ralm)			｜
 *		assert(ralm.almstat == TALM_STA)｜
 *		assert(ralm.lefttim > RELTIM1)	｜
 *		sta_alm(ALM1, RELTIM1)			｜
 *							... (A-2)	｜
 *		ref_alm(ALM1, &ralm)			｜
 *		assert(ralm.almstat == TALM_STA)｜
 *		assert(ralm.lefttim <= RELTIM1)	｜
 *		slp_tsk()						｜
 *										｜	== ALM1-2 ==
 *	== TASK1（続き）==					｜	2:	wup_tsk(TASK1)
 *	3:	msta_alm(ALM2, RELTIM1, PRC1)	｜		RETURN
 *						... (B-1)(J-1)	｜
 *		ref_alm(ALM2, &ralm)			｜
 *		assert(ralm.prcid == PRC1)		｜
 *		slp_tsk()						｜
 *	== ALM2-1 ==						｜
 *	4:	wup_tsk(TASK1)					｜
 *		RETURN							｜
 *	== TASK1（続き）==					｜
 *	5:	get_tim(&systim1)				｜
 *		msta_alm(ALM2, RELTIM1, PRC2)	｜
 *							... (B-2)	｜
 *		ref_alm(ALM2, &ralm)	... (E)	｜
 *		assert(ralm.almstat == TALM_STA)｜
 *		assert(ralm.prcid == PRC2)		｜
 *		slp_tsk()						｜
 *										｜	== ALM2-2 ==
 *	== TASK1（続き）==					｜	3:	wup_tsk(TASK1)
 *	6:	get_tim(&systim2)		... (F)	｜		RETURN
 *		assert(RELTIM1 <= (systim2 - systim1))
 *		assert((systim2 - systim1) <= RELTIM2)
 *		msta_alm(ALM2, RELTIM1, PRC2)	｜
 *							... (C-1)	｜
 *		slp_tsk()						｜
 *										｜	== ALM2-3 ==
 *	== TASK1（続き）==					｜	4:	wup_tsk(TASK1)
 *	7:	msta_alm(ALM2, RELTIM1, \		｜		RETURN
 *				TPRC_INI) ... (C-2)(I)	｜
 *		slp_tsk()						｜
 *	== ALM2-4 ==						｜
 *	8:	wup_tsk(TASK1)					｜
 *		RETURN							｜
 *	== TASK1（続き）==					｜
 *	9:	sta_alm(ALM2, RELTIM2)			｜
 *		msta_alm(ALM2, RELTIM1, PRC1)	｜
 *							... (B-3)	｜
 *		ref_alm(ALM2, &ralm)	... (G)	｜
 *		assert(ralm.lefttim <= RELTIM1)	｜
 *		slp_tsk()						｜
 *	== ALM2-5 ==						｜
 *	10:	wup_tsk(TASK1)					｜
 *		RETURN							｜
 *	== TASK1（続き）==					｜
 *	11:	sta_alm(ALM2, RELTIM2)			｜
 *		msta_alm(ALM2, RELTIM1, PRC2)	｜
 *							... (B-4)	｜
 *		ref_alm(ALM2, &ralm)			｜
 *		assert(ralm.lefttim <= RELTIM1)	｜
 *		slp_tsk()						｜
 *										｜	== ALM2-6 ==
 *	== TASK1（続き）==					｜	5:	wup_tsk(TASK1)
 *	12:	sta_alm(ALM2, RELTIM2)			｜		RETURN
 *		msta_alm(ALM2, RELTIM1, PRC2)	｜
 *							... (C-3)	｜
 *		ref_alm(ALM2, &ralm)			｜
 *		assert(ralm.lefttim <= RELTIM1)	｜
 *		slp_tsk()						｜
 *										｜	== ALM2-7 ==
 *	== TASK1（続き）==					｜	6:	wup_tsk(TASK1)
 *	13:	sta_alm(ALM2, RELTIM2)			｜		RETURN
 *		msta_alm(ALM2, RELTIM1, PRC1)	｜
 *							... (C-4)	｜
 *		ref_alm(ALM2, &ralm)			｜
 *		assert(ralm.lefttim <= RELTIM1)	｜
 *		slp_tsk()						｜
 *	== ALM2-8 ==						｜
 *	14:	msta_alm(ALM2, RELTIM1, PRC1)	｜
 *						... (D-1)(J-2)	｜
 *		RETURN							｜
 *	== ALM2-9 ==						｜
 *	15:	get_pid(&prcid)					｜
 *		assert(prcid == PRC1)			｜
 *		msta_alm(ALM2, RELTIM1, PRC2)	｜
 *							... (D-2)	｜
 *		get_pid(&prcid)					｜
 *		assert(prcid == PRC1) ... (H-1)	｜
 *		RETURN							｜
 *										｜	== ALM2-10 ==
 *										｜	7:	get_pid(&prcid)	
 *										｜		assert(prcid == PRC2) ... (H-2)
 *	== TASK1（続き）==					｜		wup_tsk(TASK1)
 *	16:	END								｜		RETURN
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"

#ifndef RELTIM1
#define RELTIM1		(2 * TEST_TIME_CP)
#endif /* RELTIM1 */

#ifndef RELTIM2
#define RELTIM2		(2 * RELTIM1)
#endif /* RELTIM2 */

void
task2(EXINF exinf)
{
	check_assert(false);
}

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

static uint_t	alarm1_count = 0;

void
alarm1_handler(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++alarm1_count) {
	case 1:
		check_point_PRC2(1);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		return;

		check_assert(false);

	case 2:
		check_point_PRC2(2);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

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
	ER_UINT	ercd;
	ID		prcid;

	switch (++alarm2_count) {
	case 1:
		check_point_PRC1(4);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		return;

		check_assert(false);

	case 2:
		check_point_PRC2(3);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		return;

		check_assert(false);

	case 3:
		check_point_PRC2(4);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		return;

		check_assert(false);

	case 4:
		check_point_PRC1(8);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		return;

		check_assert(false);

	case 5:
		check_point_PRC1(10);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		return;

		check_assert(false);

	case 6:
		check_point_PRC2(5);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		return;

		check_assert(false);

	case 7:
		check_point_PRC2(6);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		return;

		check_assert(false);

	case 8:
		check_point_PRC1(14);
		ercd = msta_alm(ALM2, RELTIM1, PRC1);
		check_ercd(ercd, E_OK);

		return;

		check_assert(false);

	case 9:
		check_point_PRC1(15);
		ercd = get_pid(&prcid);
		check_ercd(ercd, E_OK);

		check_assert(prcid == PRC1);

		ercd = msta_alm(ALM2, RELTIM1, PRC2);
		check_ercd(ercd, E_OK);

		ercd = get_pid(&prcid);
		check_ercd(ercd, E_OK);

		check_assert(prcid == PRC1);

		return;

		check_assert(false);

	case 10:
		check_point_PRC2(7);
		ercd = get_pid(&prcid);
		check_ercd(ercd, E_OK);

		check_assert(prcid == PRC2);

		ercd = wup_tsk(TASK1);
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
	T_RALM	ralm;
	SYSTIM	systim1;
	SYSTIM	systim2;

	test_start(__FILE__);

	check_point_PRC1(1);
	ercd = sta_alm(ALM1, RELTIM1);
	check_ercd(ercd, E_OK);

	ercd = ref_alm(ALM1, &ralm);
	check_ercd(ercd, E_OK);

	check_assert(ralm.almstat == TALM_STA);

	check_assert(ralm.lefttim <= RELTIM1);

	check_assert(ralm.prcid == PRC2);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point_PRC1(2);
	ercd = sta_alm(ALM1, RELTIM2);
	check_ercd(ercd, E_OK);

	ercd = ref_alm(ALM1, &ralm);
	check_ercd(ercd, E_OK);

	check_assert(ralm.almstat == TALM_STA);

	check_assert(ralm.lefttim > RELTIM1);

	ercd = sta_alm(ALM1, RELTIM1);
	check_ercd(ercd, E_OK);

	ercd = ref_alm(ALM1, &ralm);
	check_ercd(ercd, E_OK);

	check_assert(ralm.almstat == TALM_STA);

	check_assert(ralm.lefttim <= RELTIM1);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point_PRC1(3);
	ercd = msta_alm(ALM2, RELTIM1, PRC1);
	check_ercd(ercd, E_OK);

	ercd = ref_alm(ALM2, &ralm);
	check_ercd(ercd, E_OK);

	check_assert(ralm.prcid == PRC1);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point_PRC1(5);
	ercd = get_tim(&systim1);
	check_ercd(ercd, E_OK);

	ercd = msta_alm(ALM2, RELTIM1, PRC2);
	check_ercd(ercd, E_OK);

	ercd = ref_alm(ALM2, &ralm);
	check_ercd(ercd, E_OK);

	check_assert(ralm.almstat == TALM_STA);

	check_assert(ralm.prcid == PRC2);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point_PRC1(6);
	ercd = get_tim(&systim2);
	check_ercd(ercd, E_OK);

	check_assert(RELTIM1 <= (systim2 - systim1));

	check_assert((systim2 - systim1) <= RELTIM2);

	ercd = msta_alm(ALM2, RELTIM1, PRC2);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point_PRC1(7);
	ercd = msta_alm(ALM2, RELTIM1, TPRC_INI);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point_PRC1(9);
	ercd = sta_alm(ALM2, RELTIM2);
	check_ercd(ercd, E_OK);

	ercd = msta_alm(ALM2, RELTIM1, PRC1);
	check_ercd(ercd, E_OK);

	ercd = ref_alm(ALM2, &ralm);
	check_ercd(ercd, E_OK);

	check_assert(ralm.lefttim <= RELTIM1);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point_PRC1(11);
	ercd = sta_alm(ALM2, RELTIM2);
	check_ercd(ercd, E_OK);

	ercd = msta_alm(ALM2, RELTIM1, PRC2);
	check_ercd(ercd, E_OK);

	ercd = ref_alm(ALM2, &ralm);
	check_ercd(ercd, E_OK);

	check_assert(ralm.lefttim <= RELTIM1);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point_PRC1(12);
	ercd = sta_alm(ALM2, RELTIM2);
	check_ercd(ercd, E_OK);

	ercd = msta_alm(ALM2, RELTIM1, PRC2);
	check_ercd(ercd, E_OK);

	ercd = ref_alm(ALM2, &ralm);
	check_ercd(ercd, E_OK);

	check_assert(ralm.lefttim <= RELTIM1);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point_PRC1(13);
	ercd = sta_alm(ALM2, RELTIM2);
	check_ercd(ercd, E_OK);

	ercd = msta_alm(ALM2, RELTIM1, PRC1);
	check_ercd(ercd, E_OK);

	ercd = ref_alm(ALM2, &ralm);
	check_ercd(ercd, E_OK);

	check_assert(ralm.lefttim <= RELTIM1);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_finish_PRC1(16);
	check_assert(false);
}
