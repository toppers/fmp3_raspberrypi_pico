/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2023 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: task.h 263 2021-01-08 06:08:59Z ertl-honda $
 */

/*
 *		タスク管理モジュール
 */

#ifndef TOPPERS_TASK_H
#define TOPPERS_TASK_H

#include "kernel_impl.h"
#include <queue.h>
#include "time_event.h"
#include "winfo.h"
#include "target_ipi.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_TSKSTAT
#define LOG_TSKSTAT(p_tcb)
#endif /* LOG_TSKSTAT */

#ifndef LOG_TSKMIG
#define LOG_TSKMIG(p_tcb, src_id, dest_id)
#endif /* LOG_TSKMIG */

/*
 *  タスク優先度の内部表現・外部表現変換マクロ
 */
#define INT_PRIORITY(x)		((uint_t)((x) - TMIN_TPRI))
#define EXT_TSKPRI(x)		((PRI)(x) + TMIN_TPRI)

/*
 *  タスク状態の内部表現
 *
 *  TCB中のタスク状態のフィールドでは，タスクの状態と，タスクが待ち状態
 *  の時の待ち要因を表す．ただし，実行状態（RUNNING）と実行可能状態
 *  （READY）は区別せず，両状態をあわせて実行できる状態（RUNNABLE）とし
 *  て管理する．二重待ち状態は，(TS_WAITING_??? | TS_SUSPENDED)で表す．
 *
 *  タスクが待ち状態（二重待ち状態を含む）の時は，TS_WAITING_???で待ち
 *  要因を表す．待ち要因（5ビットで表現される）の上位2ビットで，同期・
 *  通信オブジェクトの待ちキューにつながっているかどうかを表す．同期・
 *  通信オブジェクトの待ちキューにつながらないものは上位2ビットを00，同
 *  期・通信オブジェクトの管理ブロックの共通部分（WOBJCB）の待ちキュー
 *  につながるものは10または11，それ以外の待ちキューにつながるものは01
 *  とする．
 */
#define TS_DORMANT		0x00U			/* 休止状態 */
#define TS_RUNNABLE		0x01U			/* 実行できる状態 */
#define TS_SUSPENDED	0x02U			/* 強制待ち状態 */

#define TS_WAITING_SLP	(0x01U << 2)	/* 起床待ち */
#define TS_WAITING_DLY	(0x02U << 2)	/* 時間経過待ち */
#define TS_WAITING_RDTQ	(0x08U << 2)	/* データキューからの受信待ち */
#define TS_WAITING_RPDQ	(0x09U << 2)	/* 優先度データキューからの受信待ち */
#define TS_WAITING_SEM	(0x10U << 2)	/* セマフォ資源の獲得待ち */
#define TS_WAITING_FLG	(0x11U << 2)	/* イベントフラグ待ち */
#define TS_WAITING_SDTQ	(0x12U << 2)	/* データキューへの送信待ち */
#define TS_WAITING_SPDQ	(0x13U << 2)	/* 優先度データキューへの送信待ち */
#define TS_WAITING_MTX	(0x14U << 2)	/* ミューテックスのロック待ち */
#define TS_WAITING_MPF	(0x15U << 2)	/* 固定長メモリブロックの獲得待ち */

#define TS_WAITING_MASK	(0x1fU << 2)	/* 待ち状態の判別用マスク */

/*
 *  タスク状態判別マクロ
 *
 *  TSTAT_DORMANTはタスクが休止状態であるかどうかを，TSTAT_RUNNABLEは
 *  タスクが実行できる状態であるかどうかを判別する．TSTAT_WAITINGは待
 *  ち状態と二重待ち状態のいずれかであるかどうかを，TSTAT_SUSPENDEDは
 *  強制待ち状態と二重待ち状態のいずれかであるかどうかを判別する．
 */
#define TSTAT_DORMANT(tstat)	((tstat) == TS_DORMANT)
#define TSTAT_RUNNABLE(tstat)	(((tstat) & TS_RUNNABLE) != 0U)
#define TSTAT_WAITING(tstat)	(((tstat) & TS_WAITING_MASK) != 0U)
#define TSTAT_SUSPENDED(tstat)	(((tstat) & TS_SUSPENDED) != 0U)

/*
 *  タスク待ち要因判別マクロ
 *
 *  TSTAT_WAIT_SLPはタスクが起床待ちであるかどうかを，TSTAT_WAIT_MTXは
 *  タスクがミューテックス待ちであるかどうかを判別する．
 *
 *  また，TSTAT_WAIT_WOBJ はタスクが同期・通信オブジェクトに対する待ち
 *  であるか（言い換えると，同期・通信オブジェクトの待ちキューにつなが
 *  れているか）どうかを，TSTAT_WAIT_WOBJCBはタスクが同期・通信オブジェ
 *  クトの管理ブロックの共通部分（WOBJCB）の待ちキューにつながれている
 *  かどうかを判別する．
 */
#define TSTAT_WAIT_SLP(tstat)	(((tstat) & ~TS_SUSPENDED) == TS_WAITING_SLP)
#define TSTAT_WAIT_MTX(tstat)	(((tstat) & ~TS_SUSPENDED) == TS_WAITING_MTX)

#define TSTAT_WAIT_WOBJ(tstat)		(((tstat) & (0x18U << 2)) != 0U)
#define TSTAT_WAIT_WOBJCB(tstat)	(((tstat) & (0x10U << 2)) != 0U)

/*
 *  ミューテックス管理ブロックの型の前方参照
 */
typedef struct mutex_control_block MTXCB;

/*
 *  タスク初期化ブロック
 *
 *  タスクに関する情報を，値が変わらないためにROMに置ける部分（タスク
 *  初期化ブロック）と，値が変化するためにRAMに置かなければならない部
 *  分（タスク管理ブロック，TCB）に分離し，TCB内に対応するタスク初期化
 *  ブロックを指すポインタを入れる．タスク初期化ブロック内に対応する
 *  TCBを指すポインタを入れる方法の方が，RAMの節約の観点からは望ましい
 *  が，実行効率が悪くなるために採用していない．他のオブジェクトについ
 *  ても同様に扱う．
 */
typedef struct task_initialization_block {
	ATR			tskatr;			/* タスク属性 */
	EXINF		exinf;			/* タスクの拡張情報 */
	TASK		task;			/* タスクの起動番地 */
	uint_t		ipriority;		/* タスクの起動時優先度（内部表現） */

#ifdef USE_TSKINICTXB
	TSKINICTXB	tskinictxb;		/* タスク初期化コンテキストブロック */
#else /* USE_TSKINICTXB */
	size_t		stksz;			/* スタック領域のサイズ（丸めた値） */
	void		*stk;			/* スタック領域の先頭番地 */
#endif /* USE_TSKINICTXB */
	ID			iprcid;			/* タスクの初期割付けプロセッサ */
	uint_t		affinity;		/* タスクの割付け可能プロセッサ */
} TINIB;

/*
 *  タスク管理ブロック（TCB）
 *
 *  FMP3カーネルでは，タスクの起動要求キューイング数の最大値（TMAX_ACTCNT）
 *  と起床要求キューイング数の最大値（TMAX_WUPCNT）は1に固定されている
 *  ため，キューイングされているかどうかの真偽値で表現することができる．
 *  また，強制待ち要求ネスト数の最大値（TMAX_SUSCNT）が1に固定されてい
 *  るので，強制待ち要求ネスト数（suscnt）は必要ない．
 *
 *  TCBのいくつかのフィールドは，特定のタスク状態でのみ有効な値を保持し，
 *  それ以外の場合は値が保証されない（よって，参照してはならない）．各
 *  フィールドが有効な値を保持する条件は次の通り．
 *
 *  ・初期化後は常に有効：
 *  		p_tinib，p_dominib，p_schedcb，tstat，actque，p_pcb，actprc
 *  ・休止状態以外で有効（休止状態では初期値になっている）：
 *  		svclevel，bpriority，priority，wupque，raster，enater，
 *  		boosted，p_lastmtx
 *  ・待ち状態（二重待ち状態を含む）で有効：
 *  		winfo，p_wobjcb，winfo_obj
 *  ・実行できる状態と同期・通信オブジェクトに対する待ち状態で有効：
 *  		task_queue
 *  ・実行可能状態，待ち状態，強制待ち状態，二重待ち状態で有効：
 *  		tskctxb
 */
typedef struct task_control_block {
	QUEUE			task_queue;		/* タスクキュー */
	const TINIB		*p_tinib;		/* 初期化ブロックへのポインタ */

#ifdef UINT8_MAX
	uint8_t			tstat;			/* タスク状態（内部表現）*/
	uint8_t			bpriority;		/* ベース優先度（内部表現）*/
	uint8_t			priority;		/* 現在の優先度（内部表現）*/
#else /* UINT8_MAX */
	BIT_FIELD_UINT	tstat : 8;		/* タスク状態（内部表現）*/
	BIT_FIELD_UINT	bpriority : 8;	/* ベース優先度（内部表現）*/
	BIT_FIELD_UINT	priority : 8;	/* 現在の優先度（内部表現）*/
#endif /* UINT8_MAX */
	BIT_FIELD_BOOL	actque : 1;		/* 起動要求キューイング */
	BIT_FIELD_BOOL	wupque : 1;		/* 起床要求キューイング */
	BIT_FIELD_BOOL	raster : 1;		/* タスク終了要求フラグ */
	BIT_FIELD_BOOL	enater : 1;		/* タスク終了許可状態 */
	BIT_FIELD_BOOL	boosted : 1;	/* 優先度上昇状態 */

	uint_t			subpri;			/* サブ優先度 */

	MTXCB			*p_lastmtx;		/* 最後にロックしたミューテックス */
	TSKCTXB			tskctxb;		/* タスクコンテキストブロック */

	PCB				*p_pcb;			/* 割付けプロセッサのPCB */
	ID				actprc;			/* 次回起動時割付けプロセッサ */

	/*
	 *  待ち状態の実現に必要な情報
	 *
	 *  HRMP3カーネルでは，他のタスクのスタックがアクセスできるとは限
	 *  らないことから，タスクの待ち状態の実現に必要な情報は，TCB中に
	 *  持つ．
	 *
	 *  タスクが待ち状態の間は，TCBを次のように設定しなければならない．
	 *
	 *  (a) TCBのtstatを待ち状態を表す値（TS_WAITING_???）にする．
	 *
	 *  (b) タイムアウトを監視するために，タイムイベントブロックを登録
	 *  する．登録するタイムイベントブロックは，TCBのwinfo.tmevtbを用
	 *  いる．タイムアウトの監視が必要ない場合（永久待ちの場合）には，
	 *  winfo.p_tmevtb.callbackをNULLにする．
	 *
	 *  同期・通信オブジェクトに対する待ち状態の場合には，以下の(c)〜
	 *  (e)の設定を行う必要がある．同期・通信オブジェクトに関係しない
	 *  待ち（起床待ち，時間経過待ち）の場合には，これらは必要ない．
	 *
	 *  (c) TCBを待ち対象の同期・通信オブジェクトの待ちキューにつなぐ．
	 *  待ちキューにつなぐために，task_queueを使う．
	 *
	 *  (d) 待ち対象の同期・通信オブジェクトの管理ブロックへのポインタ
	 *  を，TCBのp_wobjcbに記憶する．
	 *
	 *  (e) 待ち対象の同期・通信オブジェクトに依存して記憶することが必
	 *  要な情報がある場合には，WINFO_???内のフィールドに記憶する．
	 *
	 *  待ち状態を解除する際には，待ち解除したタスクに対する返値をTCB
	 *  のwinfo.wercdに設定する．winfo.wercdが必要なのは待ち解除以降で
	 *  あるのに対して，winfo.tmevtbは待ち解除後は必要ないため，メモリ
	 *  節約のために共用体を使っている．そのため，winfo.wercdへエラー
	 *  コードを設定するのは，タイムイベントブロックを登録解除した後に
	 *  しなければならない．
	 */
	union waiting_information {
		ER		wercd;				/* 待ち解除時のエラーコード */
		TMEVTB	tmevtb;				/* タイムイベントブロック */
	} winfo;
	WOBJCB		*p_wobjcb;			/* 待ちオブジェクトの管理ブロック */
	WINFO_OBJ	winfo_obj;			/* オブジェクト毎の待ち情報ブロック */
} TCB;

/*
 *  指定した優先度に対応するビットマップ中でのビット位置
 */
#ifndef PRIMAP_BIT
#define	PRIMAP_BIT(pri)		(1U << (pri))
#endif /* PRIMAP_BIT */

/*
 *  サブ優先度を用いてスケジュールする優先度（kernel_cfg.c）
 *
 *  優先度が16段階であることを仮定しているため，uint16_t型としている．
 */
extern const uint16_t	subprio_primap;

/*
 *  タスクIDの最大値（kernel_cfg.c）
 */
extern const ID	tmax_tskid;

/*
 *  タスク初期化ブロックのエリア（kernel_cfg.c）
 */
extern const TINIB	tinib_table[];

/*
 *  タスク生成順序テーブル（kernel_cfg.c）
 */
extern const ID	torder_table[];

/*
 *  TCBのエリアへのポインタ（kernel_cfg.c）
 */
extern TCB *const	p_tcb_table[];

/*
 *  タスクの数
 */
#define tnum_tsk	((uint_t)(tmax_tskid - TMIN_TSKID + 1))

/*
 *  タスクIDからTCBを取り出すためのマクロ
 */
#define INDEX_TSK(tskid)	((uint_t)((tskid) - TMIN_TSKID))
#define get_tcb(tskid)		(p_tcb_table[INDEX_TSK(tskid)])

/*
 *  TCBからタスクIDを取り出すためのマクロ
 */
#define	TSKID(p_tcb)	((ID)(((p_tcb)->p_tinib - tinib_table) + TMIN_TSKID))

/*
 *  タスク管理モジュールの初期化
 */
extern void	initialize_task(PCB *p_my_pcb);

/*
 *  最高優先順位タスクのサーチ
 *
 *  レディキュー中の最高優先順位のタスクをサーチし，そのTCBへのポインタ
 *  を返す．レディキューが空の場合には，この関数を呼び出してはならない．
 */
extern TCB	*search_schedtsk(PCB *p_pcb);

/*
 *  実行すべきタスクの更新とディスパッチの要求
 *
 *  実行すべきタスクを更新し，その結果，他プロセッサでのタスクディスパッ
 *  チが必要になる場合には，他プロセッサに割込みをかけてディスパッチを
 *  要求する．
 */
Inline void
update_schedtsk_dsp(PCB *p_my_pcb, PCB *p_pcb, TCB *p_new_schedtsk)
{
	TCB		*p_prev_schedtsk;

	if (p_pcb->dspflg) {
		p_prev_schedtsk = p_pcb->p_schedtsk;
		memory_barrier();
		p_pcb->p_schedtsk = p_new_schedtsk;
		if ((p_pcb != p_my_pcb) && (p_prev_schedtsk != p_pcb->p_schedtsk)) {
			request_dispatch_prc(p_pcb->prcid);
		}
	}
}

/*
 *  実行できる状態への遷移
 *
 *  p_tcbで指定されるタスクをレディキューに挿入する．また，必要な場合
 *  には，実行すべきタスクを更新する．
 */
extern void	make_runnable(PCB *p_my_pcb, TCB *p_tcb);

/*
 *  実行できる状態から他の状態への遷移
 *
 *  p_tcbで指定されるタスクをレディキューから削除する．p_tcbで指定した
 *  タスクが実行すべきタスクであった場合には，実行すべきタスクを更新す
 *  る．
 */
extern void	make_non_runnable(PCB *p_my_pcb, TCB *p_tcb);

/*
 *  タスクディスパッチ可能状態への遷移
 *
 *  タスクディスパッチ可能状態であることを示すフラグ（dspflg）をtrueに
 *  し，実行すべきタスクを更新する．
 */
Inline void
set_dspflg(PCB *p_my_pcb)
{
	/*
	 *  割込み優先度マスクをTIPM_ENAALLに戻す．
	 */
	t_set_ipm(TIPM_ENAALL);
	p_my_pcb->dspflg = true;
	p_my_pcb->p_schedtsk = search_schedtsk(p_my_pcb);
}

/*
 *  休止状態への遷移
 *
 *  p_tcbで指定されるタスクの状態を休止状態とする．また，タスクの起動
 *  時に初期化すべき変数の初期化と，タスク起動のためのコンテキストを設
 *  定する．
 */
extern void	make_dormant(TCB *p_tcb);

/*
 *  休止状態から実行できる状態への遷移
 *
 *  p_tcbで指定されるタスクの状態を休止状態から実行できる状態とする．
 */
extern void	make_active(PCB *p_my_pcb, TCB *p_tcb);

/*
 *  タスクの優先度の変更
 *
 *  p_tcbで指定されるタスクの優先度をnewpri（内部表現）に変更する．ま
 *  た，必要な場合には，実行すべきタスクを更新する．
 *
 *  p_tcbで指定されるタスクが実行できる状態である場合，その優先順位は，
 *  優先度が同じタスクの中で，mtxmodeがfalseの時は最低，mtxmodeがtrue
 *  の時は最高とする．
 */
extern void	change_priority(PCB *p_my_pcb, TCB *p_tcb,
									uint_t newpri, bool_t mtxmode);

/*
 *  タスクのサブ優先度の変更
 *
 *  p_tcbで指定されるタスク（対象タスク）のサブ優先度をsubpriに変更す
 *  る．また，必要な場合には，実行すべきタスクを更新する．
 *
 *  対象タスクの優先順位は，タスクが実行できる状態で，タスクの現在優先
 *  度がサブ優先度を使用すると設定されている場合には，同じ優先度で同じ
 *  サブ優先度のタスクの中で最低とする．
 */
extern void change_subprio(PCB *p_my_pcb, TCB *p_tcb, uint_t subpri, PCB *p_pcb);

/*
 *  レディキューの回転
 *
 *  レディキュー中の，優先度priのタスクキューを回転させる．また，必要
 *  な場合には，実行すべきタスクを更新する．
 */
extern void	rotate_ready_queue(PCB *p_my_pcb, uint_t pri, PCB *p_pcb);

/*
 *  タスクの終了処理
 *
 *  p_tcbで指定されるタスクを終了させる処理を行う．p_tcbで指定されるタ
 *  スクは，自プロセッサに割り付けられているタスクでなければならない．
 *  タスクの起動要求キューイング数が0でない場合には，再度起動するため
 *  の処理を行う．p_tcbで指定されるタスクが自タスクで，他のプロセッサ
 *  で再起動すべき場合にtrue，そうでない場合にfalseを返す．
 */
extern bool_t task_terminate(PCB *p_my_pcb, TCB *p_tcb);

/*
 *  自タスクのマイグレート
 */
extern void migrate_self(PCB *p_my_pcb, TCB *p_selftsk);

/*
 *  自タスクのマイグレートと起動
 */
extern void migrate_activate_self(PCB *p_my_pcb, TCB *p_selftsk);

#endif /* TOPPERS_TASK_H */
