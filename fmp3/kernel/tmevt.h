/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2020 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: tmevt.h 265 2021-01-08 06:17:00Z ertl-honda $
 */

/*
 *		タイムイベントブロックの定義
 *
 *  このインクルードファイルは，pcb.hのみでインクルードされる．
 */

#ifndef TOPPERS_TMEVT_H
#define TOPPERS_TMEVT_H

/*
 *  イベント時刻のデータ型の定義［ASPD1001］
 *
 *  タイムイベントヒープに登録するタイムイベントの発生時刻を表現するた
 *  めのデータ型．オーバヘッド低減のために，32ビットで扱う．
 */
typedef uint32_t	EVTTIM;

/*
 *  タイムイベントヒープ中のノードのデータ型の前方参照
 */
typedef union time_event_node TMEVTN;

/*
 *  タイムイベントブロックのデータ型の定義
 */
typedef void	(*CBACK)(PCB *, void *);	/* コールバック関数の型 */

typedef struct time_event_block {
	EVTTIM	evttim;			/* タイムイベントの発生時刻 */
	TMEVTN	*p_tmevtn;		/* タイムイベントヒープ中での位置 */
	CBACK	callback;		/* コールバック関数 */
	void	*arg;			/* コールバック関数へ渡す引数 */
} TMEVTB;

/*
 *  タイムイベントヒープ中のノードのデータ型の定義
 *
 *  タイムイベントヒープの先頭のノード（*p_tmevt_heap）に，最後の使用領
 *  域を指すポインタ（p_last）を格納し，それ以降をタイムイベントヒープ
 *  として使用する．(p_tmevt_heap->p_last - p_tmevt_heap) が，タイムイ
 *  ベントヒープに登録されているタイムイベントの数となる．
 */
typedef union time_event_node {
	TMEVTB	*p_tmevtb;		/* 対応するタイムイベントブロック */
	TMEVTN	*p_last;		/* タイムイベントヒープの最後の使用領域 */
} TMEVTN;

/*
 *  タイムイベントコントロールブロック
 */
typedef struct time_event_control_block{
	/*
	 *  高分解能タイマ割込みの処理中であることを示すフラグ［ASPD1032］
	 */
	bool_t	in_signal_time;

	/*
	 *  タイムイベントヒープへのポインタ
	 */
	TMEVTN  *p_tmevt_heap;
} TEVTCB;

#endif /* TOPPERS_TMEVT_H */
