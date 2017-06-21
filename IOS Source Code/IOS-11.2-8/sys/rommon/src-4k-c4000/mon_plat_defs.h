/* $Id: mon_plat_defs.h,v 3.2 1995/11/17 18:45:33 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-4k-c4000/mon_plat_defs.h,v $
 *------------------------------------------------------------------
 * mon_plat_defs.h
 *
 * December 1994, Rob Clevenger
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Miscellaneous platform specific monitor defines.
 *------------------------------------------------------------------
 * $Log: mon_plat_defs.h,v $
 * Revision 3.2  1995/11/17  18:45:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:23  hampton
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
#define DEFAULT_CONFIG	(DISABLE_BREAK | BOOT_DEFAULT)

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

#define SCOPE_TRIGGER_ADDR ((unsigned long)&KSEG1_IO_ASIC->sys_ctl)

#endif  /* __MON_PLAT_DEFS_H__ */

