/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2007,2011,2015 by Embedded and Real-Time Systems Laboratory
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
 */

/*
 * sil.hのターゲット依存部（RP2350用）
 *
 * このインクルードファイルは，sil.hの先頭でインクルードされる．他のファ
 * イルからは直接インクルードすることはない．このファイルをインクルー
 * ドする前に，t_stddef.hがインクルードされるので，それらに依存しても
 * よい．
 */

#ifndef TOPPERS_CHIP_SIL_H
#define TOPPERS_CHIP_SIL_H

/*
 * プロセッサのインディアン定義
 * LPCはリトルエンディアン
 */
#define SIL_ENDIAN_LITTLE

/*
 * プロセッサで共通な定義 
 */
#include <core_sil.h>

#include <core_insn.h>

/*
 * 一般共通レジスタ操作関数
 */
#define sil_mskb( mem, val, msk ) sil_wrb_mem((uint8_t *)((uintptr_t)(mem) + 0x1000), ((sil_reb_mem(mem) ^ (val)) & (msk)))
#define sil_orb( mem, val )  sil_wrb_mem((uint8_t *)((uintptr_t)(mem) + 0x2000), val)
#define sil_andb( mem, val ) sil_wrb_mem(mem, sil_reb_mem(mem) & val)
#define sil_clrb( mem, val ) sil_wrb_mem((uint8_t *)((uintptr_t)(mem) + 0x3000), val)
#define sil_mskh( mem, val, msk ) sil_wrh_mem((uint16_t *)((uintptr_t)(mem) + 0x1000), ((sil_reh_mem(mem) ^ (val)) & (msk)))
#define sil_orh( mem, val )  sil_wrh_mem((uint16_t *)((uintptr_t)(mem) + 0x2000), val)
#define sil_andh( mem, val ) sil_wrh_mem(mem, sil_reh_mem(mem) & val)
#define sil_clrh( mem, val ) sil_wrh_mem((uint16_t *)((uintptr_t)(mem) + 0x3000), val)
#define sil_mskw( mem, val, msk ) sil_wrw_mem((uint32_t *)((uintptr_t)(mem) + 0x1000), ((sil_rew_mem(mem) ^ (val)) & (msk)))
#define sil_orw( mem, val )  sil_wrw_mem((uint32_t *)((uintptr_t)(mem) + 0x2000), val)
#define sil_andw( mem, val ) sil_wrw_mem(mem, sil_rew_mem(mem) & val)
#define sil_clrw( mem, val ) sil_wrw_mem((uint32_t *)((uintptr_t)(mem) + 0x3000), val)

#include "RP2350.h"

#ifndef TOPPERS_MACRO_ONLY

/*
 * プロセッサIDの取得
 */
Inline void sil_get_pid(ID *p_prcid)
{
    *p_prcid = (*RP2350_SIO_CPUID) + 1;
}

#define TOPPERS_sil_spn_hw_reg ((volatile uint32_t *)RP2350_SIO_SPINLOCKn(SPINLOCK_NO_FOR_SIL_LOCK))

/*
 * スピンロック変数（chip_kernel_impl.c）
 */
extern uint32_t TOPPERS_sil_spn_var;

/*
 * スピンロックの取得
 */
uint32_t TOPPERS_sil_loc_spn(void);
// Inline uint32_t TOPPERS_sil_loc_spn(void)
// {
//     uint32_t primask = get_primask();
//     set_primask();

//     ID prcid;
//     sil_get_pid(&prcid);

//     if (TOPPERS_sil_spn_var == prcid) {
//         /* すでに取得済み */
//         return (1 << 1) | primask;
//     }
//     while (1) {
//         if (*(TOPPERS_sil_spn_hw_reg)) {
//             TOPPERS_sil_spn_var = prcid;
//             ARM_MEMORY_CHANGED;
//             return primask;
//         }
//         assign_primask(primask);
//         /* insert WFE here */
//         set_primask();
//     }
// }

/*
 * スピンロックの返却
 */
Inline void TOPPERS_sil_unl_spn(uint32_t primask)
{
    if (primask & (1 << 1)) {
        /* スピンロックを取得していた場合 */
        primask &= ~(1 << 1);
    } else {
        TOPPERS_sil_spn_var = 0;
        ARM_MEMORY_CHANGED;
        *TOPPERS_sil_spn_hw_reg = 0;
        /* insert SEV here */
    }
    assign_primask(primask);
}

/*
 *  SILスピンロック
 */
#define SIL_LOC_SPN() ((void)(TOPPERS_locked = TOPPERS_sil_loc_spn()))
#define SIL_UNL_SPN() (TOPPERS_sil_unl_spn(TOPPERS_locked))

/*
 * スピンロックの強制解放
 *
 * 自プロセッサがスピンロックを取得している場合に解放する．
 */
Inline void TOPPERS_sil_force_unl_spn(void)
{
    ID prcid;
    sil_get_pid(&prcid);
    if (TOPPERS_sil_spn_var == prcid) {
        *TOPPERS_sil_spn_hw_reg = 0;
        TOPPERS_sil_spn_var = 0;
        ARM_MEMORY_CHANGED;
        /* Insert SEV here */
    }
}

#endif /* TOPPERS_MACRO_ONLY */

#endif /* TOPPERS_CHIP_SIL_H */
