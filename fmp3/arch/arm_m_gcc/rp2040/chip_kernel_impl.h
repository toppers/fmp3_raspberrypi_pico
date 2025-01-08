/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2015 by Embedded and Real-Time Systems Laboratory
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

#ifndef TOPPERS_CHIP_KERNEL_IMPL_H
#define TOPPERS_CHIP_KERNEL_IMPL_H

/*
 * ターゲット依存部モジュール（RP2040用）
 *
 * カーネルのターゲット依存部のインクルードファイル．kernel_impl.hのター
 * ゲット依存部の位置付けとなす．
 */

#ifndef TOPPERS_MACRO_ONLY

/*
 * 自プロセッサのインデックス（0オリジン）の取得
 */
Inline uint_t get_my_prcidx(void)
{
    return *RP2040_SIO_CPUID;
}

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  コア依存モジュール（ARM-M用）
 *  core_kernel_impl.hがget_my_prcidx()に依存しているので、ここで読み込む．
 */
#include <core_kernel_impl.h>

#ifndef TOPPERS_MACRO_ONLY

/*
 * ロックに関する定義と操作
 */

/*
 * ロックの型
 */
typedef uint32_t LOCK;

/*
 *  ロックの初期化
 */
Inline void initialize_lock(LOCK *p_lock)
{
}

/*
 * ロックの取得
 *
 * ロック取得失敗時に，スピンを繰り返すのではなく，WFE命令によりイベ
 * ント待ちとすることが推奨されている．
 *
 * WFE命令によるイベント待ちの際，割込み応答性を低下させないため，割
 * 込みを許可した状態で，イベント待ちとするべきである．WFE命令自身は，
 * 割込みの許可／禁止を行わないため，割込みを明示的に許可／禁止する．
 *
 * その際，割込みが許可された直後（WFE命令を実行する前）に割込みが入
 * り，その後，割込みハンドラからのリターン後にWFE命令によりイベント
 * 待ちになり，イベントを逃す可能性が考えられる．
 *
 * しかしながら，割込みハンドラからのリターン（正確には例外一般からの
 * リターン）を行うと，イベントレジスタがセットされる．WFE命令実行時
 * にイベントレジスタがセットされていると，クリアしてWFE命令が即座に
 * 終了するため，問題ない．
 */
Inline void acquire_lock(LOCK *p_lock)
{
    while (1) {
        if (sil_rew_mem(p_lock)) {
            ARM_MEMORY_CHANGED;
            return;
        }
        unlock_cpu();
        /* insert WFE here */
        delay_for_interrupt();
        lock_cpu();
    }
}

/*
 * ロックの取得の試行
 */
Inline bool_t try_lock(LOCK *p_lock)
{
    if (sil_rew_mem(p_lock)) {
        ARM_MEMORY_CHANGED;
        return false;
    } else {
        return true;
    }
}

/*
 * ロックの解放
 */
Inline void release_lock(LOCK *p_lock)
{
    ARM_MEMORY_CHANGED;
    sil_wrw_mem(p_lock, 0);
}

/*
 * ロックの参照
 */
Inline bool_t refer_lock(LOCK *p_lock)
{
    const int index = (uintptr_t)p_lock - (uintptr_t)RP2040_SIO_SPINLOCKn(0);
    if (sil_rew_mem(RP2040_SIO_SPINLOCK_ST) & (1 << index)) {
        return true;
    } else {
        return false;
    }
}

/*
 * ジャイアントロックに関する定義と操作
 */

/*
 * ジャイアントロック用のロック変数
 */
extern LOCK *giant_lock;

/*
 *  ロックの初期化
 */
#define initialize_glock() giant_lock = (LOCK *)RP2040_SIO_SPINLOCKn(31)

/*
 * ロックの取得
 */
#define acquire_glock() acquire_lock(giant_lock)

/*
 * ロックの解放
 */
#define release_glock() release_lock(giant_lock)

/*
 * ネイティブスピンロックに関する定義と操作
 */

#define TMAX_NATIVE_SPN 30

/*
 * スピンロックの初期化
 */
#define initialize_native_spn(p_spninib)

/*
 * スピンロックの取得
 */
#define lock_native_spn(p_spninib) acquire_lock((LOCK *)p_spninib->lock)

/*
 * スピンロックの取得の試行
 */
#define try_native_spn(p_spninib) try_lock((LOCK *)p_spninib->lock)

/*
 * スピンロックの返却
 */
#define unlock_native_spn(p_spninib) release_lock((LOCK *)p_spninib->lock)

/*
 * スピンロック状態の参照
 */
#define refer_native_spn(p_spninib) refer_lock((LOCK *)p_spninib->lock)

/*
 * エミュレーションされたスピンロックに関する定義
 */
#define delay_for_emulate_spn()

#endif /* TOPPERS_MACRO_ONLY */

/*
 * トレースログに関する設定
 */
#ifdef TOPPERS_ENABLE_TRACE
#include "logtrace/trace_config.h"
#endif /* TOPPERS_ENABLE_TRACE */

/*
 * コア依存モジュール（ARM-M用）
 */
#include <core_kernel_impl.h>

#endif /* TOPPERS_CHIP_KERNEL_IMPL_H */
