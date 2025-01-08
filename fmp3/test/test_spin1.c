/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2007-2018 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: test_spin1.c 263 2021-01-08 06:08:59Z ertl-honda $
 */

/* 
 *		スピンロック機能のテスト(1)
 *  【テストの目的】
 *    スピンロックサービスコールが動作しているかチェックする．
 *
 *  【テスト項目】
 *    APIの種類により場合分けを行う．
 *
 *    (1) loc_spnによるタスク間の排他制御
 *    (2) try_spnによるタスク間の排他制御
 *    (3) loc_spnによるアラームハンドラ間の排他制御
 *    (4) try_spnによるアラームハンドラ間の排他制御
 *    (5) loc_spnとloc_spnによるタスクとアラームハンドラ間の排他制御
 *    (6) try_spnとtry_spnによるタスクとアラームハンドラ間の排他制御  
 */


#include <kernel.h>
#include <t_syslog.h>
#include <sil.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_spin1.h"

#if TNUM_PRCID > 2
#error This test program only supports 2 processor configuration.
#endif /* TNUM_PRCID > 2 */

#define LOOP_REF 10000
#define BUSY_LOOP_REF 1000

volatile uint32_t global_data;
volatile uint32_t wait_data[TNUM_PRCID];

/*
 *  上位16bitのインクリメント
 */
int32_t
inc_data_upper(int32_t data)
{
	uint_t i;

	for (i = 0; i < BUSY_LOOP_REF; i++);
	return data + 0x10000;
}

/*
 *  下位6bitのインクリメント
 */
int32_t
inc_data_lower(int32_t data)
{
	uint_t i;

	for (i = 0; i < BUSY_LOOP_REF; i++);
	return data + 0x0001;
}

void
init_wait(void)
{
	int i;

	for (i = 0; i < TNUM_PRCID; i++) {
		wait_data[i] = 0;
	}
}

void
set_wait(void)
{
	ID prcid;

	sil_get_pid(&prcid);
	wait_data[prcid - 1] = 1;
}

void
wait_test(void) {
	int i;

	for (i = 0; i < TNUM_PRCID; i++) {
		while(wait_data[i] == 0);
	}
	init_wait();
}

/*
 *  PE1で動作するテストのメインタスク
 */
void
task1_1(EXINF exinf)
{
	ER	ercd;
	uint_t  i;

	init_wait();
	check_point(1);

	/*
	 * (1) loc_spnによるタスク間の排他制御
	 */
	global_data = 0;
	ercd = act_tsk(TASK2_1);
	check_ercd(ercd, E_OK);

	test_barrier(1);

	for(i = 0; i < LOOP_REF; i++) {
		ercd = loc_spn(SPN1);
		check_ercd(ercd, E_OK);
		global_data = inc_data_upper(global_data);
		ercd = unl_spn(SPN1);
		check_ercd(ercd, E_OK);
	}

	check_assert((global_data & 0xffff0000) == (LOOP_REF << 16));

	set_wait();
	wait_test();

	check_point(2);

	/*
	 * (2) try_spnによるタスク間の排他制御
	 */
	global_data = 0;
	ercd = act_tsk(TASK2_2);
	check_ercd(ercd, E_OK);

	test_barrier(2);

	for(i = 0; i < LOOP_REF; i++) {
		do {
			ercd = try_spn(SPN1);
		}while(ercd == E_OBJ);
		check_ercd(ercd, E_OK);
		global_data = inc_data_upper(global_data);
		ercd = unl_spn(SPN1);
		check_ercd(ercd, E_OK);
	}

	check_assert((global_data & 0xffff0000) == (LOOP_REF << 16));

	set_wait();
	wait_test();

	check_point(3);

	/*
	 * (3) loc_spnによるアラームハンドラ間の排他制御
	 */
	global_data = 0;
	ercd = sta_alm(ALMHDR2_1, 1);
	check_ercd(ercd, E_OK);

	check_point(4);
	ercd = sta_alm(ALMHDR1_1, 1);
	check_ercd(ercd, E_OK);

	wait_test();

	check_point(7);

	/*
	 * (4) try_spnによるアラームハンドラ間の排他制御
	 */
	global_data = 0;
	ercd = sta_alm(ALMHDR2_2, 1);
	check_ercd(ercd, E_OK);

	check_point(8);
	ercd = sta_alm(ALMHDR1_2, 1);
	check_ercd(ercd, E_OK);

	wait_test();

	check_point(11);

	/*
	 * (5) loc_spnとloc_spnによるタスクとアラームハンドラ間の排他制御
	 */
	global_data = 0;
	ercd = sta_alm(ALMHDR2_3, 1);
	check_ercd(ercd, E_OK);

	test_barrier(5);

	for(i = 0; i < LOOP_REF; i++) {
		ercd = loc_spn(SPN1);
		check_ercd(ercd, E_OK);
		global_data = inc_data_upper(global_data);
		ercd = unl_spn(SPN1);
		check_ercd(ercd, E_OK);
	}

	check_assert((global_data & 0xffff0000) == (LOOP_REF << 16));

	set_wait();
	wait_test();

	check_point(12);

	/*
	 * (6) try_spnとtry_spnによるタスクとアラームハンドラ間の排他制御
	 */
	global_data = 0;
	ercd = sta_alm(ALMHDR2_4, 11);
	check_ercd(ercd, E_OK);

	test_barrier(6);

	for(i = 0; i < LOOP_REF; i++) {
		do {
			ercd = try_spn(SPN2);
		} while(ercd == E_OBJ);
		check_ercd(ercd, E_OK);
		global_data = inc_data_upper(global_data);
		ercd = unl_spn(SPN2);
		check_ercd(ercd, E_OK);
	}

	check_assert((global_data & 0xffff0000) == (LOOP_REF << 16));

	set_wait();
	wait_test();

	check_finish_prc(13, PRC1);
}

/*
 * (1) loc_spnによるタスク間の排他制御用のタスク
 */
void
task2_1(EXINF exinf)
{
	ER	ercd;
	uint_t i;

	check_point_prc(1, PRC2);
	test_barrier(1);

	for(i = 0; i < LOOP_REF; i++) {
		ercd = loc_spn(SPN1);
		check_ercd(ercd, E_OK);
		global_data = inc_data_lower(global_data);
		ercd = unl_spn(SPN1);
		check_ercd(ercd, E_OK);
	}

	check_assert((global_data & 0xffff) == LOOP_REF);

	set_wait();

	check_point_prc(2, PRC2);
}

/*
 * (2) try_spnによるタスク間の排他制御用のタスク
 */
void
task2_2(EXINF exinf)
{
	ER	ercd;
	uint_t i;

	check_point_prc(3, PRC2);
	test_barrier(2);

	for(i = 0; i < LOOP_REF; i++) {
		do {
			ercd = try_spn(SPN1);
		} while(ercd == E_OBJ);
		check_ercd(ercd, E_OK);
		global_data = inc_data_lower(global_data);
		ercd = unl_spn(SPN1);
		check_ercd(ercd, E_OK);
	}

	check_assert((global_data & 0xffff) == LOOP_REF);

	set_wait();

	check_point_prc(4, PRC2);
}

/*
 * (3) loc_spnによるアラームハンドラ間の排他制御
 */
void
alarm_handler1_1(EXINF exinf)
{
	ER	ercd;
	uint_t i;

	check_point(5);
	test_barrier(3);

	for(i = 0; i < LOOP_REF; i++) {
		ercd = loc_spn(SPN2);
		check_ercd(ercd, E_OK);
		global_data = inc_data_upper(global_data);
		ercd = iunl_spn(SPN2);
		check_ercd(ercd, E_OK);
	}

	check_assert((global_data & 0xffff0000) == (LOOP_REF << 16));

	set_wait();

	check_point(6);
}

void
alarm_handler2_1(EXINF exinf)
{
	ER	ercd;
	uint_t i;

	check_point_prc(5, PRC2);

	test_barrier(3);

	for(i = 0; i < LOOP_REF; i++) {
		ercd = loc_spn(SPN2);
		check_ercd(ercd, E_OK);
		global_data = inc_data_lower(global_data);
		ercd = iunl_spn(SPN2);
		check_ercd(ercd, E_OK);
	}

	check_assert((global_data & 0xffff) == LOOP_REF);

	set_wait();

	check_point_prc(6, PRC2);
}

/*
 * (4) try_spnによるアラームハンドラ間の排他制御
 */
void
alarm_handler1_2(EXINF exinf)
{
	ER	ercd;
	uint_t i;

	check_point(9);
	test_barrier(4);

	for(i = 0; i < LOOP_REF; i++) {
		do {
			ercd = try_spn(SPN2);
		} while(ercd == E_OBJ);
		check_ercd(ercd, E_OK);
		global_data = inc_data_upper(global_data);
		ercd = iunl_spn(SPN2);
		check_ercd(ercd, E_OK);
	}

	check_assert((global_data & 0xffff0000) == (LOOP_REF << 16));

	set_wait();

	check_point(10);
}

void
alarm_handler2_2(EXINF exinf)
{
	ER	ercd;
	uint_t i;

	check_point_prc(7, PRC2);

	test_barrier(4);

	for(i = 0; i < LOOP_REF; i++) {
		do {
			ercd = try_spn(SPN2);
		} while(ercd == E_OBJ);
		check_ercd(ercd, E_OK);
		global_data = inc_data_lower(global_data);
		ercd = iunl_spn(SPN2);
		check_ercd(ercd, E_OK);
	}

	check_assert((global_data & 0xffff) == LOOP_REF);

	set_wait();

	check_point_prc(8, PRC2);
}

/*
 * (5) loc_spnとloc_spnによるタスクとアラームハンドラ間の排他制御
 */
void
alarm_handler2_3(EXINF exinf)
{
	ER	ercd;
	uint_t i;

	check_point_prc(9, PRC2);
	test_barrier(5);

	for(i = 0; i < LOOP_REF; i++) {
		ercd = loc_spn(SPN1);
		check_ercd(ercd, E_OK);
		global_data = inc_data_lower(global_data);
		ercd = iunl_spn(SPN1);
		check_ercd(ercd, E_OK);
	}

	check_assert((global_data & 0xffff) == LOOP_REF);

	set_wait();

	check_point_prc(10, PRC2);
}

/*
 * (6) try_spnとtry_spnによるタスクとアラームハンドラ間の排他制御
 */
void
alarm_handler2_4(EXINF exinf)
{
	ER	ercd;
	uint_t i;

	check_point_prc(11, PRC2);
	test_barrier(6);

	for(i = 0; i < LOOP_REF; i++) {
		do {
			ercd = try_spn(SPN2);
		} while(ercd == E_OBJ);
		check_ercd(ercd, E_OK);
		global_data = inc_data_lower(global_data);
		ercd = iunl_spn(SPN2);
		check_ercd(ercd, E_OK);
	}

	check_assert((global_data & 0xffff) == LOOP_REF);

	set_wait();

	check_finish_prc(12, PRC2);
}
