/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2005-2019 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: test_svc.c 209 2020-01-30 09:59:08Z ertl-honda $
 */

/* 
 *		テストプログラム用サービス（非TECS版専用）
 */

#include <kernel.h>
#include <sil.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/syslog.h"
#include "target_syssvc.h"
#include "test_svc.h"

/*
 *	チェックポイント
 */
static uint_t	check_count[TNUM_PRCID];

/*
 *  バリア同期用の変数
 */ 
static volatile uint_t	prc_phase[TNUM_PRCID];
static volatile uint_t	sys_phase;

/*
 *  テストプログラムの開始
 */
void
test_start(const char *progname)
{
	uint_t	i;

	syslog_1(LOG_NOTICE, "Test program: %s", progname);
	for (i = 0; i < TNUM_PRCID; i++) {
		check_count[i] = 0U;
		prc_phase[i] = 0U;
	}
	sys_phase = 0U;
}

/*
 *	テストプログラムの終了
 */
static void
test_finish(void)
{
	SIL_PRE_LOC;

	SIL_LOC_INT();
	(void) syslog_fls_log();
	(void) ext_ker();

	/* ここへ来ることはないはず */
	SIL_UNL_INT();
}

/*
 *	チェックポイント
 */
void
check_point_prc(uint_t count, ID prcid)
{
	ID			my_prcid;
	bool_t		errorflag = false;
#ifdef CHECK_BIT_FUNC
	ER			rercd;
	extern ER	CHECK_BIT_FUNC(void);
#endif /* CHECK_BIT_FUNC */
	SIL_PRE_LOC;

	/*
	 *  割込みロック状態に
	 */
	SIL_LOC_INT();

	/*
	 *  シーケンスチェック
	 */
	if (0 < prcid && prcid <= TNUM_PRCID) {
		sil_get_pid(&my_prcid);
		if (my_prcid != prcid) {
			syslog_3(LOG_ERROR, "## Unexpected check point %d-%d "
							"by processor %d.\007", prcid, count, my_prcid);
			errorflag = true;
		}
		else if (++check_count[prcid - 1] == count) {
			syslog_2(LOG_NOTICE, "Check point %d-%d passed.", prcid, count);
	}
	else {
			syslog_2(LOG_ERROR, "## Unexpected check point %d-%d.\007",
															prcid, count);
		errorflag = true;
	}
	}
	else {
		if (++check_count[0] == count) {
			syslog_1(LOG_NOTICE, "Check point %d passed.", count);
		}
		else {
			syslog_1(LOG_ERROR, "## Unexpected check point %d.\007", count);
			errorflag = true;
		}
	}

	/*
	 *  カーネルの内部状態の検査
	 */
#ifdef CHECK_BIT_FUNC
	rercd = CHECK_BIT_FUNC();
	if (rercd < 0) {
		syslog_2(LOG_ERROR, "## Internal inconsistency detected (%s, %d).\007",
										itron_strerror(rercd), SERCD(rercd));
		errorflag = true;
	}
#endif /* CHECK_BIT_FUNC */

	/*
	 *  エラーが検出された場合は，テストプログラムを終了する．
	 */
	if (errorflag) {
		test_finish();
	}

	/*
	 *  割込みロック状態を解除
	 */
	SIL_UNL_INT();
}

/*
 *	完了チェックポイント（シングルコアテスト用）
 */
void
check_finish(uint_t count)
{
	if (count > 0U) {
		check_point_prc(count, 0);
		syslog_0(LOG_NOTICE, "All check points passed.");
	}
	test_finish();
}

/*
 *	完了チェックポイント（マルチコアテスト用）
 */
void
check_finish_prc(uint_t count, ID prcid)
{
	if (count > 0U) {
		check_point_prc(count, prcid);
		syslog_0(LOG_NOTICE, "All check points passed.");
	}
	test_finish();
}

/*
 *	条件チェックのエラー処理
 */
void
check_assert_error(const char *expr, const char *file, int_t line)
{
	syslog_3(LOG_ERROR, "## Assertion `%s' failed at %s:%u.\007",
												expr, file, line);
	test_finish();
}

/*
 *	エラーコードチェックのエラー処理
 */
void
check_ercd_error(ER ercd, const char *file, int_t line)
{
	syslog_3(LOG_ERROR, "## Unexpected error %s detected at %s:%u.\007",
									itron_strerror(ercd), file, line);
	test_finish();
}

/*
 *	割込み優先度マスクのチェック
 */
ER
get_interrupt_priority_mask(PRI *p_intpri)
{
	return(get_ipm(p_intpri));
}

/*
 *  テストプログラム用のバリア同期
 *
 *	バリア同期途中に，割込みやプリエンプションが発生する可能性がある．
 *	バリア同期途中のタスクマイグレーションには対応していない．
 */
void
test_barrier(uint_t phase)
{
	ID     prcid;
	bool_t	flag;
	uint_t	i;

	get_pid(&prcid);
	prc_phase[prcid - 1] = phase;

	if (prcid == TOPPERS_MASTER_PRCID) {
		while (true) {
			flag = false;
			for (i = 0; i < TNUM_PRCID; i++){
				if (prc_phase[i] != phase) {
					flag = true;
					break;
				}
			}
			if (!flag) {
				break;
			}
			sil_dly_nse(100);

			/* 割込みでバリアを使った場合のために書き直す */
			prc_phase[prcid - 1] = phase;
		}
		sys_phase = phase;
	}
	else {
		while (sys_phase != phase) {
			sil_dly_nse(100);

			/* 割込みでバリアを使った場合のために書き直す */
			prc_phase[prcid - 1] = phase;
		}
	}
}

