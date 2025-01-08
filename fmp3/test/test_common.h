/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2006-2021 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: test_common.h 265 2021-01-08 06:17:00Z ertl-honda $
 */

/*
 *		テストプログラムで共通に使用する定義と宣言
 */

#include <kernel.h>
#include <sil.h>

/*
 *  ターゲット依存の定義
 */
#include "target_test.h"

/*
 *  プロセッサ固定のチェックポイント関数
 */
#define	check_point_PRC1(count)		check_point_prc(count, PRC1)
#define	check_point_PRC2(count)		check_point_prc(count, PRC2)
#define	check_finish_PRC1(count)	check_finish_prc(count, PRC1)
#define	check_finish_PRC2(count)	check_finish_prc(count, PRC2)

/*
 *  タスクの優先度の定義
 */
#define HIGH_PRIORITY	9		/* 高優先度 */
#define MID_PRIORITY	10		/* 中優先度 */
#define LOW_PRIORITY	11		/* 低優先度 */

/*
 *  拡張SVC番号の定義
 */
#define TFN_EXTSVC0		50
#define TFN_EXTSVC1		51
#define TFN_EXTSVC2		52
#define TFN_EXTSVC3		53

/*
 *  ターゲットに依存する可能性のある定数の定義
 */
#ifndef STACK_SIZE
#define	STACK_SIZE		4096		/* タスクのスタックサイズ */
#endif /* STACK_SIZE */

#ifndef TEST_TIME_CP
#define TEST_TIME_CP	50000U		/* チェックポイント到達情報の出力時間 */
#endif /* TEST_TIME_CP */

#ifndef TEST_TIME_PROC
#define TEST_TIME_PROC	1000U		/* チェックポイントを通らない場合の時間 */
#endif /* TEST_TIME_PROC */

/*
 *  範囲外のパラメータ値の定義
 */
#define TSKID_LARGE				((TNUM_TSKID) + 1)
#define TSKID_SMALL_TASK		(TSK_SELF - 1)
#define TSKID_SMALL_INT			(0)

#define TPRI_LARGE				(TMAX_TPRI + 1)
#define TPRI_SMALL_TASK			(TPRI_SELF - 1)
#define TPRI_SMALL_INT			(TMIN_TPRI - 1)

#define CYCID_LARGE				((TNUM_CYCID) + 1)
#define CYCID_SMALL				(0)

#define ALMID_LARGE				((TNUM_ALMID) + 1)
#define ALMID_SMALL				(0)

#define DOMID_LARGE				((TNUM_DOMID) + 1)
#define DOMID_SMALL				(TDOM_NONE)

#define PRCID_LARGE				((TNUM_PRCID) + 1)
#define PRCID_SMALL				(TPRC_INI - 1)

#define RELTIM_LARGE			(TMAX_RELTIM + 1)

/*
 *  ポーリング同期用フラグ変数の操作
 */
#define SET(flag) do {							\
	sil_swrb_mem((uint8_t *) &flag, true);		\
} while (false)

#define RESET(flag) do {						\
	sil_swrb_mem((uint8_t *) &flag, false);		\
} while (false)

#define WAIT(flag) do {							\
	sil_swrb_mem((uint8_t *) &flag, false);		\
	while (!sil_reb_mem((uint8_t *) &flag)) ;	\
} while (false)

#define WAIT_WO_RESET(flag) do {				\
	while (!sil_reb_mem((uint8_t *) &flag)) ;	\
} while (false)

#define WAIT_RESET(flag) do {					\
	while (!sil_reb_mem((uint8_t *) &flag)) ;	\
	sil_swrb_mem((uint8_t *) &flag, false);		\
} while (false)

/*
 *  関数のプロトタイプ宣言
 */
#ifndef TOPPERS_MACRO_ONLY

extern void	task1(EXINF exinf);
extern void	task2(EXINF exinf);
extern void	task3(EXINF exinf);
extern void	task4(EXINF exinf);
extern void	task5(EXINF exinf);
extern void	task6(EXINF exinf);
extern void	task7(EXINF exinf);
extern void	task8(EXINF exinf);
extern void	task9(EXINF exinf);
extern void	task10(EXINF exinf);
extern void	task11(EXINF exinf);
extern void	task12(EXINF exinf);
extern void	task13(EXINF exinf);
extern void	task51(EXINF exinf);
extern void	task52(EXINF exinf);

extern void	cyclic1_handler(EXINF exinf);

extern void	alarm1_handler(EXINF exinf);
extern void	alarm2_handler(EXINF exinf);

extern void	isr1(EXINF exinf);
extern void	isr2(EXINF exinf);

extern void cpuexc1_handler(void *p_excinf);
extern void	cpuexc2_handler(void *p_excinf);
extern void	cpuexc3_handler(void *p_excinf);
extern void	cpuexc4_handler(void *p_excinf);
extern void	cpuexc5_handler(void *p_excinf);

extern ER_UINT extsvc1_routine(intptr_t par1, intptr_t par2, intptr_t par3,
									intptr_t par4, intptr_t par5, ID cdmid);
extern ER_UINT extsvc2_routine(intptr_t par1, intptr_t par2, intptr_t par3,
									intptr_t par4, intptr_t par5, ID cdmid);
extern ER_UINT extsvc3_routine(intptr_t par1, intptr_t par2, intptr_t par3,
									intptr_t par4, intptr_t par5, ID cdmid);

#endif /* TOPPERS_MACRO_ONLY */
