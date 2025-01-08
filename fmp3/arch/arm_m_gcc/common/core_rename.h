/* This file is generated from core_rename.def by genrename. */

#ifndef TOPPERS_CORE_RENAME_H
#define TOPPERS_CORE_RENAME_H

/*
 *  core_kernel_impl.c
 */
#define p_exc_tbl					_kernel_p_exc_tbl
#define p_vector_table				_kernel_p_vector_table
#define default_exc_handler			_kernel_default_exc_handler
#define default_int_handler			_kernel_default_int_handler
#define config_int					_kernel_config_int
#define core_initialize				_kernel_core_initialize
#define core_terminate				_kernel_core_terminate
#define idstkpt_table				_kernel_idstkpt_table
#define p_bitpat_cfgint				_kernel_p_bitpat_cfgint
#define set_exc_int_priority		_kernel_set_exc_int_priority
#define enable_exc					_kernel_enable_exc
#define disable_exc					_kernel_disable_exc
#define p_iipm_enable_irq_tbl		_kernel_p_iipm_enable_irq_tbl

/*
 *  core_support.S
 */
#define core_int_entry				_kernel_core_int_entry
#define core_exc_entry				_kernel_core_exc_entry
#define svc_handler					_kernel_svc_handler
#define pendsv_handler				_kernel_pendsv_handler
#define dispatch					_kernel_dispatch
#define do_dispatch					_kernel_do_dispatch
#define dispatcher_1				_kernel_dispatcher_1
#define start_r						_kernel_start_r
#define start_dispatch				_kernel_start_dispatch
#define exit_and_dispatch			_kernel_exit_and_dispatch
#define call_exit_kernel			_kernel_call_exit_kernel


#endif /* TOPPERS_CORE_RENAME_H */
