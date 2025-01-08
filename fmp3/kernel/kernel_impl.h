/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2020 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: kernel_impl.h 263 2021-01-08 06:08:59Z ertl-honda $
 */

/*
 *		TOPPERS/FMPカーネル内部向け標準ヘッダファイル
 *
 *  このヘッダファイルは，カーネルを構成するプログラムのソースファイル
 *  で必ずインクルードするべき標準ヘッダファイルである．
 *
 *  アセンブリ言語のソースファイルからこのファイルをインクルードする時
 *  は，TOPPERS_MACRO_ONLYを定義しておく．これにより，マクロ定義以外を
 *  除くようになっている．
 */

#ifndef TOPPERS_KERNEL_IMPL_H
#define TOPPERS_KERNEL_IMPL_H

/*
 *  カーネルの内部識別名のリネーム
 */
#include "kernel_rename.h"

/*
 *  アプリケーションと共通のヘッダファイル
 */
#include <kernel.h>

/*
 *  システムログ機能のための定義
 */
#include <t_syslog.h>

/*
 *  トレースログに関する設定
 */
#ifdef TOPPERS_ENABLE_TRACE
#include "arch/tracelog/trace_log.h"
#endif /* TOPPERS_ENABLE_TRACE */

/*
 *  プロセッサIDの最小値と最大値の定義
 */
#define TMIN_PRCID		1			/* プロセッサIDの最小値 */
#define TMAX_PRCID		TNUM_PRCID	/* プロセッサIDの最大値 */

/*
 *  割込み番号のマスク（下位16bitのみが有効）
 */
#define INTNO_MASK(intno)		(intno & 0xffffU)

/*
 *  割込み番号の接続プロセッサのプロセッサID（上位16bit）
 */
#define INTNO_PRCID(intno)		(intno >> 16U)

/*
 *  割込みハンドラ番号のマスク（下位16bitのみが有効）
 */
#define INHNO_MASK(intno)		(intno & 0xffffU)

/*
 *  CPU例外ハンドラ番号のマスク（下位16bitのみが有効）
 */
#define EXCNO_MASK(intno)		(intno & 0xffffU)

/*
 *  CPU例外ハンドラ番号の接続プロセッサのプロセッサID（上位16bit）
 */
#define EXCNO_PRCID(intno)		(intno >> 16U)

/*
 *  優先度の段階数の定義
 *   pcb.hで参照するためにincludeする前に定義．
 */
#define TNUM_TPRI		(TMAX_TPRI - TMIN_TPRI + 1)
#define TNUM_DPRI		(TMAX_DPRI - TMIN_DPRI + 1)
#define TNUM_INTPRI		(TMAX_INTPRI - TMIN_INTPRI + 1)

#ifndef TOPPERS_MACRO_ONLY
/*
 *  プロセッサ管理ブロックの前方参照（pcb.h）
 */
typedef struct processor_control_block PCB;

/*
 *  タスク管理ブロックの型の前方参照（task.h）
 */
typedef struct task_control_block TCB;

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  ターゲット依存情報の定義
 */
#include "target_kernel_impl.h"

/*
 *  タイムマスタプロセッサ
 */
#ifndef TOPPERS_TMASTER_PRCID
#define TOPPERS_TMASTER_PRCID	TOPPERS_MASTER_PRCID
#endif /* TOPPERS_TMASTER_PRCID */

#ifndef TOPPERS_MACRO_ONLY
/*
 *  プロセッサ管理ブロックの定義
 */
#include "pcb.h"

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  すべての関数をコンパイルするための定義
 */
#ifdef ALLFUNC
#include "allfunc.h"
#endif /* ALLFUNC */

/*
 *  ビットフィールドでの符号無し整数型
 *
 *  8ビット以下の幅のビットフィールドで，符号無し整数値を保持したい場合
 *  に用いるべきデータ型．ANSI Cでは，ビットフィールドのデータ型には
 *  intとunsigned intしか許されないため，デフォルトの定義はunsigned
 *  intとしているが，ターゲットおよびツール依存で，unsigned charまたは
 *  unsigned shortに定義した方が効率が良い場合がある．
 */
#ifndef BIT_FIELD_UINT
#define BIT_FIELD_UINT		unsigned int
#endif /* BIT_FIELD_UINT */

/*
 *  ビットフィールドでのブール型
 *
 *  1ビット幅のビットフィールドで，ブール値を保持したい場合に用いるべき
 *  データ型．デフォルトではBIT_FIELD_UINTと同一に定義しているが，ブー
 *  ル値を保持することを明示するために別の名称としている．
 */
#ifndef BIT_FIELD_BOOL
#define BIT_FIELD_BOOL		BIT_FIELD_UINT
#endif /* BIT_FIELD_BOOL */

/*
 *  オブジェクトIDの最小値の定義
 */
#define TMIN_TSKID		1		/* タスクIDの最小値 */
#define TMIN_SEMID		1		/* セマフォIDの最小値 */
#define TMIN_FLGID		1		/* フラグIDの最小値 */
#define TMIN_DTQID		1		/* データキューIDの最小値 */
#define TMIN_PDQID		1		/* 優先度データキューIDの最小値 */
#define TMIN_MTXID		1		/* ミューテックスIDの最小値 */
#define TMIN_MPFID		1		/* 固定長メモリプールIDの最小値 */
#define TMIN_CYCID		1		/* 周期通知IDの最小値 */
#define TMIN_ALMID		1		/* アラーム通知IDの最小値 */
#define TMIN_SPNID		1		/* スピンロックIDの最小値 */

/*
 *  カーネル内部で使用する属性の定義
 */
#define TA_NOEXS		((ATR)(-1))			/* 未登録状態 */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  ヘッダファイルを持たないモジュールの関数・変数の宣言
 */

/*
 *  TECSの初期化（init_tecs.c）
 */
extern void initialize_tecs(void);

/*
 *  各モジュールの初期化（kernel_cfg.c）
 */
extern void initialize_object(PCB *p_my_pcb);

/*
 *  初期化ルーチン関係の定義（kernel_cfg.c）
 */
typedef struct initialization_routine_block {
	INIRTN		inirtn;					/* 初期化ルーチンの先頭番地 */
	EXINF		exinf;					/* 初期化ルーチンの拡張情報 */
} INIRTNB;

typedef struct initialization_routine_block_table_block {
	uint_t			tnum_inirtn;		/* 初期化ルーチンの数 */
	const INIRTNB	*inirtnb_table;		/* 初期化ルーチンブロックテーブル */
} INIRTNBB;

extern const INIRTNBB inirtnbb_table[];

/*
 *  終了処理ルーチン関係の定義（kernel_cfg.c）
 */
typedef struct termination_routine_block {
	TERRTN		terrtn;					/* 終了処理ルーチンの先頭番地 */
	EXINF		exinf;					/* 終了処理ルーチンの拡張情報 */
} TERRTNB;

typedef struct termination_routine_block_table_block {
	uint_t			tnum_terrtn;		/* 終了処理ルーチンの数 */
	const TERRTNB	*terrtnb_table;		/* 終了処理ルーチンブロックテーブル */
} TERRTNBB;

extern const TERRTNBB terrtnbb_table[];

/*
 *  システム状態に対するアクセス許可ベクタ（kernel_cfg.c）
 */
extern const ACVCT	sysstat_acvct;

/*
 *  非タスクコンテキスト用のスタック領域（kernel_cfg.c）
 */
extern const size_t istksz_table[];		/* スタック領域のサイズ（丸めた値）*/
extern STK_T *const istk_table[];		/* スタック領域の先頭番地 */
#ifdef TOPPERS_ISTKPT
extern STK_T *const istkpt_table[];		/* スタックポインタの初期値 */
#endif /* TOPPERS_ISTKPT */

/*
 *  カーネル動作状態フラグ（kernel_cfg.c）
 */
extern bool_t	kerflg_table[];

/*
 *  カーネルの起動／終了に用いるバリア同期（startup.c）
 */
extern void barrier_sync(uint_t phase);

/*
 *  カーネルの起動（startup.c）
 */
extern void	sta_ker(void);

/*
 *  カーネルの終了処理（startup.c）
 */
extern void	exit_kernel(PCB *p_my_pcb);

/*
 *  ディスパッチハンドラ（startup.c）
 */
extern void dispatch_handler(void);

/*
 *  カーネル終了ハンドラ（startup.c）
 */
extern void ext_ker_handler(void);

/*
 *  通知ハンドラの型定義
 */
typedef void	(*NFYHDR)(EXINF exinf);

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_KERNEL_IMPL_H */
