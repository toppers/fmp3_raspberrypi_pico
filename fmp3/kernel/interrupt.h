/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 *  $Id: interrupt.h 263 2021-01-08 06:08:59Z ertl-honda $
 */

/*
 *		割込み管理機能
 */

#ifndef TOPPERS_INTERRUPT_H
#define TOPPERS_INTERRUPT_H

#include "kernel_impl.h"

#if !defined(OMIT_INITIALIZE_INTERRUPT) || defined(USE_INHINIB_TABLE)

/*
 *  割込みハンドラ初期化ブロック
 */
typedef struct interrupt_handler_initialization_block {
	INHNO		inhno;			/* 割込みハンドラ番号 */
	ATR			inhatr;			/* 割込みハンドラ属性 */
	FP			int_entry;		/* 割込みハンドラの出入口処理の番地 */
	ID			prcid;			/* 割込みハンドラの割付けプロセッサ */
} INHINIB;

/*
 *  定義する割込みハンドラ番号の数（kernel_cfg.c）
 */
extern const uint_t	tnum_def_inhno;

/*
 *  割込みハンドラ初期化ブロックのエリア（kernel_cfg.c）
 */
extern const INHINIB	inhinib_table[];

#endif /* !defined(OMIT_INITIALIZE_INTERRUPT) || defined(USE_INHINIB_TABLE) */

#if !defined(OMIT_INITIALIZE_INTERRUPT) || defined(USE_INTINIB_TABLE)

/*
 *  割込み要求ライン初期化ブロック
 */
typedef struct interrupt_request_initialization_block {
	INTNO		intno;			/* 割込み番号 */
	ATR			intatr;			/* 割込み属性 */
	PRI			intpri;			/* 割込み優先度 */
	ID			iprcid;			/* 割込みの初期割付けプロセッサ */
	uint_t		affinity;		/* 割込みの割付け可能プロセッサ */
} INTINIB;

/*
 *  設定する割込み要求ラインの数（kernel_cfg.c）
 */
extern const uint_t	tnum_cfg_intno;

/*
 *  割込み要求ライン初期化ブロックのエリア（kernel_cfg.c）
 */
extern const INTINIB	intinib_table[];

#endif /* !defined(OMIT_INITIALIZE_INTERRUPT) || defined(USE_INTINIB_TABLE) */

/*
 *  割込み管理機能の初期化
 */
extern void	initialize_interrupt(PCB *p_my_pcb);

#endif /* TOPPERS_INTERRUPT_H */
