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
 *  $Id: sample1.c 263 2021-01-08 06:08:59Z ertl-honda $
 */

/* 
 *  サンプルプログラム(1)の本体
 *
 *  FMPカーネルの基本的な動作を確認するためのサンプルプログラム．
 *
 *  プログラムの概要:
 *
 *  ユーザインタフェースを受け持つメインタスク（MAIN_TASK）と，プロセッ
 *  サ毎に，3つの並行実行されるタスク（TASK1_1〜TASK1_3，TASK2_1〜
 *  TASK2_3，TASK3_1〜TASK3_3，TASK4_1〜TASK4_3），例外処理タスク
 * （EXC_TASK1，EXC_TASK2，EXC_TASK3，EXC_TASK4），アラーム通知で
 *  起動されるタスク（ALM_TASK1，ALM_TASK2，ALM_TASK3，ALM_TASK4）の
 *  計21個のタスクを用いる．
 *  これらの他に，システムログタスクが動作する．また，プロセッサ毎に，
 *  周期ハンドラ，アラーム通知，割込みサービスルーチン，CPU例外ハンド
 *  ラをそれぞれ1つ用いる．
 *
 *  はカーネルドメインに属する．
 *
 *  システム周期を15ミリ秒とし，システム動作モードを3つ用意する．シス
 *  テム動作モード1（SOM1）では，両プロセッサで，DOM1とDOM2に各4ミリ秒
 *  のタイムウィンドウを割り当てる．システム動作モード2（SOM2）とシス
 *  テム動作モード3（SOM3）は，2つを巡回するようにし，いずれのシステム
 *  動作モードでも，プロセッサ1ではDOM1の方に長いタイムウィンドウを割
 *  り当て，プロセッサ2ではDOM1のみにタイムウィンドウを割り当てる．
 *
 *  並行実行されるタスクは，task_loop回のループを実行する度に，タスク
 *  が実行中であることをあらわすメッセージを表示する．ループを実行する
 *  のは，プログラムの動作を確認しやすくするためである．また，低速なシ
 *  リアルポートを用いてメッセージを出力する場合に，すべてのメッセージ
 *  が出力できるように，メッセージの量を制限するという理由もある．
 *
 *  周期ハンドラ，アラーム通知で起動されるタスク，割込みサービスルーチ
 *  ンは，3つの優先度（HIGH_PRIORITY，MID_PRIORITY，
 *  LOW_PRIORITY）のレディキューを回転させる．周期ハンドラは，プログラ
 *  ムの起動直後は停止状態になっている．
 *
 *  CPU例外ハンドラは，CPU例外からの復帰が可能な場合には，例外処理タス
 *  クを起動する．例外処理タスクは，CPU例外を起こしたタスクに対して，
 *  終了要求を行う．
 *
 *  メインタスクは，シリアルポートからの文字入力を行い（文字入力を待っ
 *  ている間は，並行実行されるタスクが実行されている），入力された文字
 *  に対応した処理を実行する．入力された文字と処理の関係は次の通り．
 *  Control-Cまたは'Q'が入力されると，プログラムを終了する．
 *
 *  '1' : 対象タスクをTASK1_1/TASK2_1/TASK3_1/TASK4_1に切り換える（初期設定はTASK1_1）．
 *  '2' : 対象タスクをTASK1_2/TASK2_2/TASK3_2/TASK4_2に切り換える．
 *  '3' : 対象タスクをTASK1_3/TASK2_3/TASK3_3/TASK4_3に切り換える．
 *  '4' : クラス1に所属するタスクを指定する．
 *        対象周期ハンドラをCYCHDR1に切り替える．
 *        対象アラームハンドラをALMHDR1に切り替える．
 *  '5' : クラス2に所属するタスクを指定する．
 *        対象周期ハンドラをCYCHDR2に切り替える．
 *        対象アラームハンドラをALMHDR2に切り替える．
 *  '6' : クラス3に所属するタスクを指定する．
 *        対象周期ハンドラをCYCHDR3に切り替える．
 *        対象アラームハンドラをALMHDR3に切り替える．
 *  '7' : クラス4に所属するタスクを指定する．
 *        対象周期ハンドラをCYCHDR4に切り替える．
 *        対象アラームハンドラをALMHDR4に切り替える．
 *  '8' : 対象プロセッサをPRC1とする（初期設定）
 *  '9' : 対象プロセッサをPRC2とする
 *  '0' : 対象プロセッサをPRC3とする
 *  '-' : 対象プロセッサをPRC4とする  
 *  'a' : 対象タスクをact_tskにより起動する．
 *  'A' : 対象タスクに対する起動要求をcan_actによりキャンセルする．
 *  'e' : 対象タスクにext_tskを呼び出させ，終了させる．
 *  't' : 対象タスクをter_tskにより強制終了する．
 *  '>' : 対象タスクの優先度をHIGH_PRIORITYにする．
 *  '=' : 対象タスクの優先度をMID_PRIORITYにする．
 *  '<' : 対象タスクの優先度をLOW_PRIORITYにする．
 *  'G' : 対象タスクの優先度をget_priで読み出す．
 *  's' : 対象タスクにslp_tskを呼び出させ，起床待ちにさせる．
 *  'S' : 対象タスクにtslp_tsk(10秒)を呼び出させ，起床待ちにさせる．
 *  'w' : 対象タスクをwup_tskにより起床する．
 *  'W' : 対象タスクに対する起床要求をcan_wupによりキャンセルする．
 *  'l' : 対象タスクをrel_waiにより強制的に待ち解除にする．
 *  'u' : 対象タスクをsus_tskにより強制待ち状態にする．
 *  'm' : 対象タスクの強制待ち状態をrsm_tskにより解除する．
 *  'd' : 対象タスクにdly_tsk(10秒)を呼び出させ，時間経過待ちにさせる．
 *  'x' : 対象タスクにras_terにより終了要求する．
 *  'y' : 対象タスクにdis_terを呼び出させ，タスク終了を禁止する．
 *  'Y' : 対象タスクにena_terを呼び出させ，タスク終了を許可する．
 *  'r' : 3つの優先度（HIGH_PRIORITY，MID_PRIORITY，LOW_PRIORITY）のレ
 *        ディキューを回転させる．
 *  'c' : 周期ハンドラを動作開始させる．
 *  'C' : 周期ハンドラを動作停止させる．
 *  'b' : アラーム通知を5秒後に起動するよう動作開始させる．
 *  'B' : アラーム通知を動作停止させる．
 *  'z' : 対象タスクにCPU例外を発生させる（ターゲットによっては復帰可能）．
 *  'Z' : 対象タスクにCPUロック状態でCPU例外を発生させる（復帰不可能）．
 *  'V' : 短いループを挟んで，fch_hrtで高分解能タイマを2回読む．
 *  'v' : 発行したシステムコールを表示する（デフォルト）．
 *  'q' : 発行したシステムコールを表示しない．
 */

#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "sample1.h"

/*
 *  サービスコールのエラーのログ出力
 */
Inline void
svc_perror(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define	SVC_PERROR(expr)	svc_perror(__FILE__, __LINE__, #expr, (expr))

/*
 *  プロセッサ時間の消費
 *
 *  ループによりプロセッサ時間を消費する．最適化ができないように，ルー
 *  プ内でvolatile変数を読み込む．
 */
static volatile long_t	volatile_var;

static void
consume_time(ulong_t ctime)
{
	ulong_t		i;

	for (i = 0; i < ctime; i++) {
		(void) volatile_var;
	}
}

#define SERVER_REQ_MIG_TSK  0x01
#define SERVER_REQ_TER_TSK  0x02

const ID server_dtqid[TNUM_PRCID] = {
    SERVER_DTQ1,
#if TNUM_PRCID >= 2
    SERVER_DTQ2,
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    SERVER_DTQ3,
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    SERVER_DTQ4,
#endif /* TNUM_PRCID >= 4 */
};

/*
 *  タスクIDのテーブル
 */
const uint_t task_id[TNUM_PRCID][3] = {
    {TASK1_1,TASK1_2,TASK1_3},
#if TNUM_PRCID >= 2
    {TASK2_1,TASK2_2,TASK2_3},
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    {TASK3_1,TASK3_2,TASK3_3},
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    {TASK4_1,TASK4_2,TASK4_3},
#endif /* TNUM_PRCID >= 4 */
};

/*
 *  タスク名(コンソール表示用)のテーブル
 */
const char *const task_name[TNUM_PRCID][3] = {
    {"TASK1_1", "TASK1_2", "TASK1_3"},
#if TNUM_PRCID >= 2
    {"TASK2_1", "TASK2_2", "TASK2_3"},
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    {"TASK3_1", "TASK3_2", "TASK3_3"},
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    {"TASK4_1", "TASK4_2", "TASK4_3"}
#endif /* TNUM_PRCID >= 4 */
};

#define TSKNO_TO_TSKID(tskno)  task_id[(tskno >> 4) - 1][(tskno & 0xf) -1]

/*
 *  例外タスクのタスクIDのテーブル
 */
const uint_t exctsk_tskid[TNUM_PRCID] = {
	EXC_TASK1,
#if TNUM_PRCID >= 2
	EXC_TASK2,
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
	EXC_TASK3,
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
	EXC_TASK4
#endif /* TNUM_PRCID >= 4 */
};

/*
 *  周期ハンドラIDのテーブル
 */
const uint_t cychd_id[TNUM_PRCID] = {
    CYCHDR1_1,
#if TNUM_PRCID >= 2
    CYCHDR2_1,
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    CYCHDR3_1,
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    CYCHDR4_1,
#endif /* TNUM_PRCID >= 4 */
};

/*
 *  アラームIDのテーブル
 */
const uint_t almhd_id[TNUM_PRCID] = {
    ALMHDR1_1,
#if TNUM_PRCID >= 2
    ALMHDR2_1,
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    ALMHDR3_1,
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    ALMHDR4_1,
#endif /* TNUM_PRCID >= 4 */
};

#ifdef TOPPERS_SUPPORT_RAS_INT
/*
 *  割込み番号のテーブル
 */
const INTNO inthd_no[TNUM_PRCID] = {
    INTNO1,
#if TNUM_PRCID >= 2
    INTNO2,
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    INTNO3,
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    INTNO4,
#endif /* TNUM_PRCID >= 4 */
};
#endif /* TOPPERS_SUPPORT_RAS_INT */

/*
 *  SERVERタスク
 *  他のタスクからの要求を受けて mig_tsk()/ter_tsk() を実行する． 
 */
void server_task(EXINF exinf)
{
	EXINF req;
	ID		tskid;
	ID		prcid;
	uint8_t syscall;
	ID dtqid = server_dtqid[exinf - 1];

	syslog(LOG_NOTICE, "Server task %d starts.", (int_t) exinf);

	while(1){
		rcv_dtq(dtqid, &req);
		tskid = (req >> 16) & 0xff;
		prcid = (req >>  8) & 0xff;
		syscall = req & 0xff;

		/*
		 *  mig_tsk/ter_tskは，対象のタスクがどのプロセッサに割り付けられて
		 *  いるか分からないため，メインタスクからの要求を全てのサーバータ
		 *  スクで実行するため，対象タスクとは異なるプロセッサに割り付けら
		 *  れているプロセッサではエラーとなる．
		 */
		if (syscall == SERVER_REQ_MIG_TSK) {
			syslog(LOG_INFO, "#Server task %d : mig_tsk(0x%x, %d)", (int_t) exinf, tskid, prcid);
			SVC_PERROR(mig_tsk(tskid, prcid));
		}
		else if (syscall == SERVER_REQ_TER_TSK) {
			syslog(LOG_INFO, "#Server task %d : ter_tsk(0x%x)", (int_t) exinf, tskid);
			SVC_PERROR(ter_tsk(tskid));
		}
	}
}

/*
 *  並行実行されるタスクへのメッセージ領域
 */
char	message[TNUM_PRCID][3];

/*
 *  タスクのマイグレーション先のプロセッサの指定
 */
ID  tsk_mig_prc;

/*
 *  ループ回数
 */
ulong_t	task_loop;		/* タスク内でのループ回数 */

/*
 *  並行実行されるタスク
 */
void
task(EXINF exinf)
{
	int_t		n = 0;
	int_t		tskno = (int_t) (exinf & 0xffff);
	int_t		classno = (int_t) (exinf >> 16);
	const char	*tskname = task_name[classno-1][tskno-1];
	const char	*graph[] = { "|", "  +", "    *" };
	char		c;
	ID		prcid;
	ER		ercd;

	while (true) {
		SVC_PERROR(get_pid(&prcid));
		syslog(LOG_NOTICE, "%s is running on prc%d (%03d) .   %s",
			   tskname, prcid, ++n, graph[tskno-1]);
		consume_time(task_loop);
		c = message[classno-1][tskno-1];
		message[classno-1][tskno-1] = 0;
		switch (c) {
		case 'e':
			syslog(LOG_INFO, "#%s#ext_tsk()", tskname);
			SVC_PERROR(ext_tsk());
			assert(0);
		case 's':
			syslog(LOG_INFO, "#%s#slp_tsk()", tskname);
			SVC_PERROR(slp_tsk());
			break;
		case 'S':
			syslog(LOG_INFO, "#%s#tslp_tsk(10000000)", tskname);
			SVC_PERROR(tslp_tsk(10000000));
			break;
		case 'd':
			syslog(LOG_INFO, "#%s#dly_tsk(10000000)", tskname);
			SVC_PERROR(dly_tsk(10000000));
			break;
		case 'g':
			syslog(LOG_INFO, "#%s#mig_tsk(0, %d)", tskname, tsk_mig_prc);
			SVC_PERROR(mig_tsk(0, tsk_mig_prc));
			break;
		case 'y':
			syslog(LOG_INFO, "#%s#dis_ter()", tskname);
			SVC_PERROR(dis_ter());
			break;
		case 'Y':
			syslog(LOG_INFO, "#%s#ena_ter()", tskname);
			SVC_PERROR(ena_ter());
			break;
		case 'p':
			syslog(LOG_INFO, "#%s#get_pid()", tskname);
			SVC_PERROR(ercd = get_pid(&prcid));
			if (ercd >= 0) {
				syslog(LOG_NOTICE, "Processor id is %d", prcid);
			}
			break;
#if defined(CPUEXC1_PRC1) || defined(CPUEXC1_PRC2) || defined(CPUEXC1_PRC3) || defined(CPUEXC1_PRC4)
		case 'z':
			syslog(LOG_NOTICE, "#%s#raise CPU exception", tskname);
			RAISE_CPU_EXCEPTION;
			break;
		case 'Z':
			SVC_PERROR(loc_cpu());
			syslog(LOG_NOTICE, "#%s#raise CPU exception", tskname);
			RAISE_CPU_EXCEPTION;
			SVC_PERROR(unl_cpu());
			break;
#endif /* defined(CPUEXC1_PRC1) || defined(CPUEXC1_PRC2) || defined(CPUEXC1_PRC3) || defined(CPUEXC1_PRC4) */
		default:
			break;
		}
	}
}

/*
 *  割込みサービスルーチン
 *
 *  HIGH_PRIORITY，MID_PRIORITY，LOW_PRIORITY の各優先度のレディキュー
 *  を回転させる．
 */
#ifdef INTNO1

void
intno1_isr(EXINF exinf)
{
	intno1_clear();
	syslog(LOG_NOTICE, "intno1_isr %d start!", exinf);
	SVC_PERROR(rot_rdq(HIGH_PRIORITY));
	SVC_PERROR(rot_rdq(MID_PRIORITY));
	SVC_PERROR(rot_rdq(LOW_PRIORITY));
}

#endif /* INTNO1 */

#ifdef INTNO2

void
intno2_isr(EXINF exinf)
{
	intno2_clear();
	syslog(LOG_NOTICE, "intno2_isr %d start!", exinf);
	SVC_PERROR(rot_rdq(HIGH_PRIORITY));
	SVC_PERROR(rot_rdq(MID_PRIORITY));
	SVC_PERROR(rot_rdq(LOW_PRIORITY));
}

#endif /* INTNO2 */

#ifdef INTNO3

void 
intno3_isr(EXINF exinf)
{
	intno3_clear();
	syslog(LOG_NOTICE, "intno3_isr %d start!", exinf);
	SVC_PERROR(rot_rdq(HIGH_PRIORITY));
	SVC_PERROR(rot_rdq(MID_PRIORITY));
	SVC_PERROR(rot_rdq(LOW_PRIORITY));
}

#endif /* INTNO3 */

#ifdef INTNO4

void 
intno4_isr(EXINF exinf)
{
	intno4_clear();
	syslog(LOG_NOTICE, "intno4_isr %d start!", exinf);
	SVC_PERROR(rot_rdq(HIGH_PRIORITY));
	SVC_PERROR(rot_rdq(MID_PRIORITY));
	SVC_PERROR(rot_rdq(LOW_PRIORITY));
}

#endif /* INTNO4 */


/*
 *  CPU例外ハンドラ
 */
ID	cpuexc_tskid_PRC1;		/* CPU例外を起こしたタスクのID */
ID	cpuexc_tskid_PRC2;
ID	cpuexc_tskid_PRC3;
ID	cpuexc_tskid_PRC4;

#if defined(CPUEXC1_PRC1) || defined(CPUEXC1_PRC2) || defined(CPUEXC1_PRC3) || defined(CPUEXC1_PRC4)

void
cpuexc_handler(void *p_excinf)
{
	ID	prcid;

	syslog(LOG_NOTICE, "CPU exception handler (p_excinf = %08p).", p_excinf);
	if (sns_ctx() != true) {
		syslog(LOG_WARNING,
					"sns_ctx() is not true in CPU exception handler.");
	}
	if (sns_dpn() != true) {
		syslog(LOG_WARNING,
					"sns_dpn() is not true in CPU exception handler.");
	}
	syslog(LOG_INFO, "sns_loc = %d, sns_dsp = %d, xsns_dpn = %d",
								sns_loc(), sns_dsp(), xsns_dpn(p_excinf));

	if (xsns_dpn(p_excinf)) {
		syslog(LOG_NOTICE, "Sample program ends with exception.");
		SVC_PERROR(ext_ker());
		assert(0);
	}

	SVC_PERROR(get_pid(&prcid));
	if (prcid == PRC1) {
		SVC_PERROR(get_tid(&cpuexc_tskid_PRC1));
		SVC_PERROR(act_tsk(EXC_TASK1));
	}
#if TNUM_PRCID >= 2
	else if (prcid == PRC2) {
		SVC_PERROR(get_tid(&cpuexc_tskid_PRC2));
		SVC_PERROR(act_tsk(EXC_TASK2));
	}
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
	else if (prcid == PRC3) {
		SVC_PERROR(get_tid(&cpuexc_tskid_PRC3));
		SVC_PERROR(act_tsk(EXC_TASK3));
	}
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
	else if (prcid == PRC4) {
		SVC_PERROR(get_tid(&cpuexc_tskid_PRC4));
		SVC_PERROR(act_tsk(EXC_TASK4));
	}
#endif /* TNUM_PRCID >= 4 */
}

#endif /* defined(CPUEXC1_PRC1) || defined(CPUEXC1_PRC2) || defined(CPUEXC1_PRC3) || defined(CPUEXC1_PRC4) */

/*
 *  周期ハンドラ
 *
 *  HIGH_PRIORITY，MID_PRIORITY，LOW_PRIORITY の各優先度のレディキュー
 *  を回転させる．
 */
void
cyclic_handler(EXINF exinf)
{
	ID	prcid = (ID) exinf;

	syslog(LOG_NOTICE, "cyclic_handler %d on prc%d start!", exinf, prcid);
	SVC_PERROR(mrot_rdq(prcid, HIGH_PRIORITY));
	SVC_PERROR(mrot_rdq(prcid, MID_PRIORITY));
	SVC_PERROR(mrot_rdq(prcid, LOW_PRIORITY));
}

/*
 *  アラームハンドラ
 *
 *  HIGH_PRIORITY，MID_PRIORITY，LOW_PRIORITY の各優先度のレディキュー
 *  を回転させる．
 */
void
alarm_handler(EXINF exinf)
{
	ID	prcid = (ID) exinf;

	syslog(LOG_NOTICE, "alarm_handler %d on prc%d start!", exinf, prcid);
	SVC_PERROR(mrot_rdq(prcid, HIGH_PRIORITY));
	SVC_PERROR(mrot_rdq(prcid, MID_PRIORITY));
	SVC_PERROR(mrot_rdq(prcid, LOW_PRIORITY));
}

/*
 *  例外処理タスク
 */
void
exc_task(EXINF exinf)
{
	ID	prcid;

	SVC_PERROR(get_pid(&prcid));
	if (prcid == PRC1) {
		SVC_PERROR(ras_ter(cpuexc_tskid_PRC1));
	}
#if TNUM_PRCID >= 2
	else if (prcid == PRC2) {
		SVC_PERROR(ras_ter(cpuexc_tskid_PRC2));
	}
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
	else if (prcid == PRC3) {
		SVC_PERROR(ras_ter(cpuexc_tskid_PRC3));
	}
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
	else if (prcid == PRC4) {
		SVC_PERROR(ras_ter(cpuexc_tskid_PRC4));
	}
#endif /* TNUM_PRCID >= 4 */
}


/*
 *  初期化ルーチンの確認のためのカウンタ
 */
static uint_t counter;

/*
 *  グローバル初期化ルーチン
 */
void
global_inirtn(EXINF exinf)
{
	counter = 1;
}

/*
 *  グローバル終了ルーチン
 */
void
global_terrtn(EXINF exinf)
{
	syslog(LOG_EMERG, "global_terrtn exinf = %d, counter = %d", exinf, counter++);
}

/*
 *  ローカル初期化ルーチン
 */
void
local_inirtn(EXINF exinf)
{
	syslog(LOG_NOTICE, "local_inirtn exinf = %d, counter = %d", exinf, counter++);
}

/*
 *  ローカル終了ルーチン
 */
void
local_terrtn(EXINF exinf)
{
	syslog(LOG_NOTICE, "local_terrtn exinf = %d, counter = %d", exinf, counter++);
}

/*
 *  メインタスク
 */
void 
main_task(EXINF exinf)
{
	ER_UINT		ercd;
	PRI			tskpri;
	ID			clsid = exinf;
	ID			prcid = exinf;
	ID			tskid = task_id[(int_t)exinf-1][0];
	const char	*tskname = task_name[(int_t)exinf-1][0];
	ID			cycid = cychd_id[(int_t)exinf-1];
	ID			almid = almhd_id[(int_t)exinf-1];
#ifdef TOPPERS_SUPPORT_RAS_INT
	INTNO		intno = inthd_no[(int_t)exinf-1];
#endif /* TOPPERS_SUPPORT_RAS_INT */
	bool_t		update_select = true;
	uint_t		tsk_select    = 1;
	uint_t		tme_select    = exinf;
	uint_t		class_select  = exinf;
	uint_t		prc_select    = exinf;
#ifdef TOPPERS_SUPPORT_RAS_INT
	uint_t		int_select    = exinf;
#endif /* TOPPERS_SUPPORT_RAS_INT */    
	uint32_t	server_req;
#ifndef TASK_LOOP
	SYSTIM		stime1, stime2;
#endif /* TASK_LOOP */
	HRTCNT		hrtcnt1, hrtcnt2;
	char		c;

	tsk_mig_prc = 1;

	SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_INFO), LOG_UPTO(LOG_EMERG)));
	syslog(LOG_NOTICE, "Sample program starts (exinf = %d).", (int_t) exinf);

	/*
	 *  シリアルポートの初期化
	 *
	 *  システムログタスクと同じシリアルポートを使う場合など，シリアル
	 *  ポートがオープン済みの場合にはここでE_OBJエラーになるが，支障は
	 *  ない．
	 */
	ercd = serial_opn_por(TASK_PORTID);
	if (ercd < 0 && MERCD(ercd) != E_OBJ) {
		syslog(LOG_ERROR, "%s (%d) reported by `serial_opn_por'.",
									itron_strerror(ercd), SERCD(ercd));
	}
	SVC_PERROR(serial_ctl_por(TASK_PORTID,
							(IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV)));

	/*
 	 *  ループ回数の設定
	 *
	 *  並行実行されるタスク内でのループの回数（task_loop）は，ループ
	 *  の実行時間が約0.4秒になるように設定する．この設定のために，
	 *  LOOP_REF回のループの実行時間を，その前後でget_timを呼ぶことで
	 *  測定し，その測定結果から空ループの実行時間が0.4秒になるループ
	 *  回数を求め，task_loopに設定する．
	 *
	 *  LOOP_REFは，デフォルトでは1,000,000に設定しているが，想定した
	 *  より遅いプロセッサでは，サンプルプログラムの実行開始に時間がか
	 *  かりすぎるという問題を生じる．逆に想定したより速いプロセッサで
	 *  は，LOOP_REF回のループの実行時間が短くなり，task_loopに設定す
	 *  る値の誤差が大きくなるという問題がある．そこで，そのようなター
	 *  ゲットでは，target_test.hで，LOOP_REFを適切な値に定義すること
	 *  とする．
	 *
	 *  また，task_loopの値を固定したい場合には，その値をTASK_LOOPにマ
	 *  クロ定義する．TASK_LOOPがマクロ定義されている場合，上記の測定
	 *  を行わずに，TASK_LOOPに定義された値をループの回数とする．
	 *
	 *  ターゲットによっては，ループの実行時間の1回目の測定で，本来より
	 *  も長めになるものがある．このようなターゲットでは，MEASURE_TWICE
	 *  をマクロ定義することで，1回目の測定結果を捨てて，2回目の測定結
	 *  果を使う．
	 */
#ifdef TASK_LOOP
	task_loop = TASK_LOOP;
#else /* TASK_LOOP */

#ifdef MEASURE_TWICE
	SVC_PERROR(get_tim(&stime1));
	consume_time(LOOP_REF);
	SVC_PERROR(get_tim(&stime2));
#endif /* MEASURE_TWICE */

	SVC_PERROR(get_tim(&stime1));
	consume_time(LOOP_REF);
	SVC_PERROR(get_tim(&stime2));
	task_loop = LOOP_REF * 400LU / (ulong_t)(stime2 - stime1) * 1000LU;

#endif /* TASK_LOOP */

	/*
 	 *  タスクの起動
	 */
	SVC_PERROR(act_tsk(TASK1_1));
	SVC_PERROR(act_tsk(TASK1_2));
	SVC_PERROR(act_tsk(TASK1_3));
#if TNUM_PRCID >= 2
	SVC_PERROR(act_tsk(TASK2_1));
	SVC_PERROR(act_tsk(TASK2_2));
	SVC_PERROR(act_tsk(TASK2_3));
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
	SVC_PERROR(act_tsk(TASK3_1));
	SVC_PERROR(act_tsk(TASK3_2));
	SVC_PERROR(act_tsk(TASK3_3));
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
	SVC_PERROR(act_tsk(TASK4_1));
	SVC_PERROR(act_tsk(TASK4_2));
	SVC_PERROR(act_tsk(TASK4_3));
#endif /* TNUM_PRCID >= 4 */
	/*
 	 *  メインループ
	 */
	do {
		if (update_select) {
			prcid = prc_select;
			clsid = class_select;
			tskid = task_id[clsid - 1][tsk_select-1];
			tskname = task_name[clsid - 1][tsk_select-1];
			cycid = cychd_id[tme_select-1];
			almid = almhd_id[tme_select-1];
#ifdef TOPPERS_SUPPORT_RAS_INT            
			intno = inthd_no[int_select-1];
#endif /* TOPPERS_SUPPORT_RAS_INT */
			tsk_mig_prc = prcid;
			update_select = false;
			syslog(LOG_INFO, "select %s", tskname);
			syslog(LOG_INFO, "select cycid %d", cycid);
			syslog(LOG_INFO, "select almid %d", almid);
			syslog(LOG_INFO, "select processor %d", prcid);
			syslog(LOG_INFO, "select class     %d", clsid);
		}

		SVC_PERROR(serial_rea_dat(TASK_PORTID, &c, 1));

		switch (c) {
		case 'p':
		case 'e':
		case 's':
		case 'S':
		case 'd':
		case 'y':
		case 'Y':
		case 'z':
		case 'Z':
		case 'g':
			message[clsid-1][tsk_select-1] = c;
			break;
		case '1':
			tsk_select = 1;
			update_select = true;
			break;
		case '2':
			tsk_select = 2;
			update_select = true;
			break;
		case '3':
			tsk_select = 3;
			update_select = true;
			break;
		case '4':
			tme_select = 1;
			class_select = 1;
#ifdef TOPPERS_SUPPORT_RAS_INT
			int_select = 1;
#endif /* TOPPERS_SUPPORT_RAS_INT */            
			update_select = true;
			break;
#if TNUM_PRCID >= 2
		case '5':
			tme_select = 2;
			class_select = 2;
#ifdef TOPPERS_SUPPORT_RAS_INT
			int_select = 2;
#endif /* TOPPERS_SUPPORT_RAS_INT */            
			update_select = true;
			break;
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
		case '6':
			tme_select = 3;
			class_select = 3;
#ifdef TOPPERS_SUPPORT_RAS_INT            
			int_select = 3;
#endif /* TOPPERS_SUPPORT_RAS_INT */            
			update_select = true;
			break;
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
		case '7':
			tme_select = 4;
			class_select = 4;
#ifdef TOPPERS_SUPPORT_RAS_INT            
			int_select = 4;
#endif /* TOPPERS_SUPPORT_RAS_INT */            
			update_select = true;
			break;
#endif /* TNUM_PRCID >= 4 */
		case '8':
			prc_select = 1;
			update_select = true;
			break;
#if TNUM_PRCID >= 2
		case '9':
			prc_select = 2;
			update_select = true;
			break;
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
		case '0':
			prc_select = 3;
			update_select = true;
			break;
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
		case '-':
			prc_select = 4;
			update_select = true;
			break;
#endif /* TNUM_PRCID >= 4 */
		case 'a':
			syslog(LOG_INFO, "#act_tsk(%s)", tskname);
			SVC_PERROR(act_tsk(tskid));
			break;
		case 'A':
			syslog(LOG_INFO, "#can_act(%s)", tskname);
			SVC_PERROR(ercd = can_act(tskid));
			if (ercd >= 0) {
				syslog(LOG_NOTICE, "can_act(%s) returns %d", tskname, ercd);
			}
			break;
		case 'f':
			syslog(LOG_INFO, "#mact_tsk(%s, 0x%x)", tskname, tsk_mig_prc);
			SVC_PERROR(mact_tsk(tskid, tsk_mig_prc));
			break;
		case 't':
			server_req = tskid << 16 | SERVER_REQ_TER_TSK;
			SVC_PERROR(snd_dtq(SERVER_DTQ1, server_req));
#if TNUM_PRCID >= 2
			SVC_PERROR(snd_dtq(SERVER_DTQ2, server_req));
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
			SVC_PERROR(snd_dtq(SERVER_DTQ3, server_req));
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
			SVC_PERROR(snd_dtq(SERVER_DTQ4, server_req));
#endif /* TNUM_PRCID >= 4 */
			break;
		case '>':
			syslog(LOG_INFO, "#chg_pri(%s, HIGH_PRIORITY)", tskname);
			SVC_PERROR(chg_pri(tskid, HIGH_PRIORITY));
			break;
		case '=':
			syslog(LOG_INFO, "#chg_pri(%s, MID_PRIORITY)", tskname);
			SVC_PERROR(chg_pri(tskid, MID_PRIORITY));
			break;
		case '<':
			syslog(LOG_INFO, "#chg_pri(%s, LOW_PRIORITY)", tskname);
			SVC_PERROR(chg_pri(tskid, LOW_PRIORITY));
			break;
		case 'G':
			syslog(LOG_INFO, "#get_pri(%s, &tskpri)", tskname);
			SVC_PERROR(ercd = get_pri(tskid, &tskpri));
			if (ercd >= 0) {
				syslog(LOG_NOTICE, "priority of %s is %d", tskname, tskpri);
			}
			break;
		case 'w':
			syslog(LOG_INFO, "#wup_tsk(%s)", tskname);
			SVC_PERROR(wup_tsk(tskid));
			break;
		case 'W':
			syslog(LOG_INFO, "#can_wup(%s)", tskname);
			SVC_PERROR(ercd = can_wup(tskid));
			if (ercd >= 0) {
				syslog(LOG_NOTICE, "can_wup(%s) returns %d", tskname, ercd);
			}
			break;
		case 'l':
			syslog(LOG_INFO, "#rel_wai(%s)", tskname);
			SVC_PERROR(rel_wai(tskid));
			break;
		case 'u':
			syslog(LOG_INFO, "#sus_tsk(%s)", tskname);
			SVC_PERROR(sus_tsk(tskid));
			break;
		case 'i':
			server_req = (tskid << 16) | (prcid << 8) | SERVER_REQ_MIG_TSK;
			SVC_PERROR(snd_dtq(SERVER_DTQ1, server_req));
#if TNUM_PRCID >= 2
			SVC_PERROR(snd_dtq(SERVER_DTQ2, server_req));
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
			SVC_PERROR(snd_dtq(SERVER_DTQ3, server_req));
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
			SVC_PERROR(snd_dtq(SERVER_DTQ4, server_req));
#endif /* TNUM_PRCID >= 4 */
			break;
		case 'm':
			syslog(LOG_INFO, "#rsm_tsk(%s)", tskname);
			SVC_PERROR(rsm_tsk(tskid));
			break;
		case 'x':
			syslog(LOG_INFO, "#ras_ter(%s)", tskname);
			SVC_PERROR(ras_ter(tskid));
			break;
		case 'r':
			syslog(LOG_INFO, "#mrot_rdq(three priorities, %d)", prcid);
			SVC_PERROR(mrot_rdq(prcid, HIGH_PRIORITY));
			SVC_PERROR(mrot_rdq(prcid, MID_PRIORITY));
			SVC_PERROR(mrot_rdq(prcid, LOW_PRIORITY));
			break;
		case 'c':
			syslog(LOG_INFO, "#sta_cyc(%d)", cycid);
			SVC_PERROR(sta_cyc(cycid));
			break;
		case 'C':
			syslog(LOG_INFO, "#stp_cyc(%d)", cycid);
			SVC_PERROR(stp_cyc(cycid));
			break;
		case 'I':
			syslog(LOG_INFO, "#msta_cyc(%d, %d)", cycid, prcid);
			SVC_PERROR(msta_cyc(cycid, prcid));
			break;
		case 'b':
			syslog(LOG_INFO, "#sta_alm(ALMHDR1, 5000000)");
			SVC_PERROR(sta_alm(almid, 5000000));
			break;
		case 'B':
			syslog(LOG_INFO, "#stp_alm(ALMHDR1)");
			SVC_PERROR(stp_alm(almid));
			break;
		case 'E':
			syslog(LOG_INFO, "#msta_alm(%d, 5000000, %d)", almid, prcid);
			SVC_PERROR(msta_alm(almid, 5000000, prcid));
			break;
#ifdef TOPPERS_SUPPORT_RAS_INT
		  case 'j':
			syslog(LOG_INFO, "#ras_int(0x%x)", intno);
			SVC_PERROR(ras_int(intno));
			break;
#endif /* TOPPERS_SUPPORT_RAS_INT */
		case 'V':
			hrtcnt1 = fch_hrt();
			consume_time(1000LU);
			hrtcnt2 = fch_hrt();
			syslog(LOG_NOTICE, "hrtcnt1 = %tu, hrtcnt2 = %tu",
								(uint32_t) hrtcnt1, (uint32_t) hrtcnt2);
			break;

		case 'v':
			SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_INFO),
										LOG_UPTO(LOG_EMERG)));
			break;
		case 'q':
			SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_NOTICE),
										LOG_UPTO(LOG_EMERG)));
			break;

		case '\003':
		case 'Q':
			break;

		default:
			syslog(LOG_INFO, "Unknown command: '%c'.", c);
			break;
		}
	} while (c != '\003' && c != 'Q');

	syslog(LOG_NOTICE, "Sample program ends.");
	SVC_PERROR(ext_ker());
	assert(0);
}
