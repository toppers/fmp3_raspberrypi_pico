/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2021 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: sample1.h 263 2021-01-08 06:08:59Z ertl-honda $
 */

/*
 *		サンプルプログラム(1)のヘッダファイル
 */

#include <kernel.h>

/*
 *  ターゲット依存の定義
 */
#include "target_test.h"

/*
 *  各タスクの優先度の定義
 */
#define EXC_PRIORITY	1		/* 例外処理タスクの優先度 */
#define MAIN_PRIORITY	5		/* メインタスクの優先度 */
								/* HIGH_PRIORITYより高くすること */

#define SERVER_PRIORITY	4		/* サーバタスクの優先度 */

#define HIGH_PRIORITY	9		/* 並行実行されるタスクの優先度 */
#define MID_PRIORITY	10
#define LOW_PRIORITY	11

/*
 *  SERVERタスクへの要求のためのDTQのバッファ数
 */
#define NUM_SERVER_DTQ		10

/*
 *  ターゲットに依存する可能性のある定数の定義
 */

#ifndef TASK_PORTID
#define	TASK_PORTID		1			/* 文字入力するシリアルポートID */
#endif /* TASK_PORTID */

#ifndef STACK_SIZE
#define	STACK_SIZE		4096		/* タスクのスタックサイズ */
#endif /* STACK_SIZE */

#ifndef LOOP_REF
#define LOOP_REF		ULONG_C(1000000)	/* 速度計測用のループ回数 */
#endif /* LOOP_REF */

/*
 *  関数のプロトタイプ宣言
 */
#ifndef TOPPERS_MACRO_ONLY

extern void	task(EXINF exinf);
extern void	main_task(EXINF exinf);
extern void	server_task(EXINF exinf);
extern void exc_task(EXINF exinf);
#ifdef INTNO1
extern void intno1_isr(EXINF exinf);
#endif /* INTNO1 */
#ifdef INTNO2
extern void intno2_isr(EXINF exinf);
#endif /* INTNO2 */
#ifdef INTNO3
extern void intno3_isr(EXINF exinf);
#endif /* INTNO3 */
#ifdef INTNO4
extern void intno4_isr(EXINF exinf);
#endif /* INTNO4 */
#if defined(CPUEXC1_PRC1) || defined(CPUEXC1_PRC2) || defined(CPUEXC1_PRC3) || defined(CPUEXC1_PRC4)
extern void	cpuexc_handler(void *p_excinf);
#endif /* defined(CPUEXC1_PRC1) || defined(CPUEXC1_PRC2) || defined(CPUEXC1_PRC3) || defined(CPUEXC1_PRC4) */
extern void	cyclic_handler(EXINF exinf);
extern void	alarm_handler(EXINF exinf);

extern void global_inirtn(EXINF exinf);
extern void global_terrtn(EXINF exinf);
extern void local_inirtn(EXINF exinf);
extern void local_terrtn(EXINF exinf);

#endif /* TOPPERS_MACRO_ONLY */
