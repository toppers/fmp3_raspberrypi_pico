/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2007-2019 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: winfo.h 178 2019-10-08 13:55:00Z ertl-honda $
 */

/*
 *		待ち情報ブロックの定義
 *
 *  このインクルードファイルは，task.hのみでインクルードされる．
 */
#ifndef TOPPERS_WINFO_H
#define TOPPERS_WINFO_H

/*
 *  同期・通信オブジェクトの管理ブロックの共通部分
 *
 *  同期・通信オブジェクトの初期化ブロックと管理ブロックの先頭部分は共
 *  通になっている．以下は，その共通部分を扱うための型である．
 */

/*
 *  同期・通信オブジェクトの初期化ブロックの共通部分
 */
typedef struct wait_object_initialization_block {
	ATR			wobjatr;		/* オブジェクト属性 */
} WOBJINIB;

/*
 *  同期・通信オブジェクトの管理ブロックの共通部分
 */
typedef struct wait_object_control_block {
	QUEUE		wait_queue;		/* 待ちキュー */
	const WOBJINIB *p_wobjinib;	/* 初期化ブロックへのポインタ */
} WOBJCB;

/*
 *  イベントフラグ待ち情報ブロックの定義
 *
 *  waiptnには，待ち状態の間は待ちパターンを，待ち解除された後は待ち解
 *  除時のパターンを入れる（1つのフィールドを2つの目的に兼用している）．
 */
typedef struct eventflag_waiting_information {
	FLGPTN		waiptn;			/* 待ちパターン／待ち解除時のパターン */
	MODE		wfmode;			/* 待ちモード */
} WINFO_FLG;

/*
 *  データキュー待ち情報ブロックの定義
 */
typedef struct dataqueue_send_waiting_information {
	intptr_t	data;			/* 送信データ */
} WINFO_SDTQ;

typedef struct dataqueue_receive_waiting_information {
	intptr_t	data;			/* 受信データ */
} WINFO_RDTQ;

/*
 *  優先度データキュー待ち情報ブロックの定義
 */
typedef struct pridataq_send_waiting_information {
	intptr_t	data;			/* 送信データ */
	PRI			datapri;		/* データ優先度 */
} WINFO_SPDQ;

typedef struct pridataq_receive_waiting_information {
	intptr_t	data;			/* 受信データ */
	PRI			datapri;		/* データ優先度 */
} WINFO_RPDQ;

/*
 *  固定長メモリプール待ち情報ブロックの定義
 */
typedef struct fixed_memorypool_waiting_information {
	void		*blk;			/* 獲得したメモリブロック */
} WINFO_MPF;

/*
 *  TCBに含めるオブジェクト毎の待ち情報ブロック
 */
typedef union wait_object_waiting_information {
	WINFO_FLG	flg;		/* イベントフラグ待ち情報ブロック */
	WINFO_SDTQ	sdtq;		/* データキュー送信待ち情報ブロック */
	WINFO_RDTQ	rdtq;		/* データキュー受信待ち情報ブロック */
	WINFO_SPDQ	spdq;		/* 優先度データキュー送信待ち情報ブロック */
	WINFO_RPDQ	rpdq;		/* 優先度データキュー受信待ち情報ブロック */
	WINFO_MPF	mpf;		/* 固定長メモリプール待ち情報ブロック */
} WINFO_OBJ;

#endif  /* TOPPERS_WINFO_H */
