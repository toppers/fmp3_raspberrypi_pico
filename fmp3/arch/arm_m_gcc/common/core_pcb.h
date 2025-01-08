/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2009-2020 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 *
 *  上記著作権者は，以下の(1)～(4)の条件を満たす場合に限り，本ソフトウェ
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
 *  @(#) $Id: core_pcb.h 289 2021-08-05 14:44:10Z ertl-komori $
 */

/*
 *		pcb.hのコア依存部（ARM-M用）
 */

#ifndef TOPPERS_CORE_PCB_H
#define TOPPERS_CORE_PCB_H

#include "arm_m.h"

#if __TARGET_ARCH_THUMB == 3 && !defined(TBITW_IPRI)
/* FIXME: core_kernel_v6m.h が本来の定義箇所 */
#define TBITW_IPRI 2
#endif

#ifndef TOPPERS_MACRO_ONLY

/*
 *  ARM依存プロセッサコントロールブロック
 */
typedef struct target_processor_control_block {
	/*
	 *  CPUロックフラグ実現のための変数
	 *
	 *  これらの変数は，CPUロック状態の時のみ書き換えてもよいとする．
	 *  インライン関数中で，アクセスの順序が変化しないよう，volatile を指定．
	 */
#if __TARGET_ARCH_THUMB >= 4
	volatile bool_t   lock_flag;  /* CPUロックフラグの値を保持する変数 */
	volatile uint32_t saved_iipm; /* 割込み優先度をマスクする変数 */
#else
	volatile bool_t lock_flag;
	volatile uint32_t *current_iipm_enable_mask;
	volatile uint32_t iipm_enable_masks[(1 << TBITW_IPRI) + 1];
#endif

	/*
	 *  アイドル処理用のスタックの初期値
	 */
	STK_T* idstkpt;
	STK_T* idstktop;
} TPCB;

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_CORE_PCB_H */
