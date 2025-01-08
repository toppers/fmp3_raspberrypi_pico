/* This file is generated from core_rename.def by genrename. */

/* This file is included only when core_rename.h has been included. */
#ifdef TOPPERS_CORE_RENAME_H
#undef TOPPERS_CORE_RENAME_H

/*
 *  core_kernel_impl.c
 */
#undef p_exc_tbl
#undef p_vector_table
#undef default_exc_handler
#undef default_int_handler
#undef config_int
#undef core_initialize
#undef core_terminate
#undef idstkpt_table
#undef p_bitpat_cfgint
#undef set_exc_int_priority
#undef enable_exc
#undef disable_exc
#undef p_iipm_enable_irq_tbl

/*
 *  core_support.S
 */
#undef core_int_entry
#undef core_exc_entry
#undef svc_handler
#undef pendsv_handler
#undef dispatch
#undef do_dispatch
#undef dispatcher_1
#undef start_r
#undef start_dispatch
#undef exit_and_dispatch
#undef call_exit_kernel


#endif /* TOPPERS_CORE_RENAME_H */
