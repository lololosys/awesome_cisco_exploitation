/* $Id: mon_plat_defs.h,v 3.3 1996/01/17 23:31:43 mansonw Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/rommon/src-4k-rsp/mon_plat_defs.h,v $
 *------------------------------------------------------------------
 * mon_plat_defs.h
 *
 * Mar 1995, Steve J. Zhang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Miscellaneous platform specific monitor defines.
 *------------------------------------------------------------------
 * $Log: mon_plat_defs.h,v $
 * Revision 3.3  1996/01/17  23:31:43  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:46:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:41:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MON_PLAT_DEFS_H__
#define __MON_PLAT_DEFS_H__
/*
** General defines.
*/
#define ALARM_FREQUENCY (1000)       		/* 1ms */

#define TRUE 1
#define FALSE 0

/*
** The default value for the virtual config register
*/
#define BOOT_DEFAULT	0x0001
/* #define DEFAULT_CONFIG	(DISABLE_BREAK | BOOT_DEFAULT) */
#define DEFAULT_CONFIG	DISABLE_BREAK

/*
** Macros for NVRAM write protection routines
** NVRAM is not write protected on this platform so these
** are left empty
*/
#define WR_ENA_NVRAM
#define WR_PROT_NVRAM

/*
 * Macro for nop's surround accesses that may cause bus errors
 */
#ifdef __mips

#define BUSERR_NOP_X4()                  	\
    asm volatile (".set noreorder");         	\
    asm volatile ("nop; nop; nop; nop");        \
    asm volatile (".set reorder"); 

/*
 * Define a write buffer flush macro
 */
#define FLUSH_BUFFERS	flush_all_wb()

/*
 * Define macros for low level primitives
 */
#define LD_ACCESS(a, t, tu)	ld_access(a, t, tu)
#define LDR_ACCESS(a, t, tu)	ldr_access(a, t, tu)
#define LDL_ACCESS(a, t, tu)	ldl_access(a, t, tu)
#define LWR_ACCESS(a, t)	lwr_access(a, t)
#define LWL_ACCESS(a, t)	lwl_access(a, t)
#define LW_ACCESS(a, t)		lw_access(a, t)
#define LH_ACCESS(a, t)		lh_access(a, t)
#define LB_ACCESS(a, t)		lb_access(a, t)

#define SD_ACCESS(a, t, tu)	sd_access(a, t, tu)
#define SDR_ACCESS(a, t, tu)	sdr_access(a, t, tu)
#define SDL_ACCESS(a, t, tu)	sdl_access(a, t, tu)
#define SWR_ACCESS(a, t)	swr_access(a, t)
#define SWL_ACCESS(a, t)	swl_access(a, t)

#endif /* __mips */

#define SCOPE_TRIGGER_ADDR ((unsigned long)&REG_FPGA->led_reg)

/*
 * declare platform specific functions
 */

/* init.c */
extern void save_reset_reason();
extern unsigned char get_reset_reason();

/* rsp_ralib.c */
extern void monlib_vector_init(void);

#if defined(RSP2)
void handle_ms_int();
void enable_ms_int();
#endif


#endif  /* __MON_PLAT_DEFS_H__ */

