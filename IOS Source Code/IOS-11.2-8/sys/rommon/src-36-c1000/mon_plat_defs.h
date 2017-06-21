/* $Id: mon_plat_defs.h,v 3.2 1995/11/17 18:44:06 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-36-c1000/mon_plat_defs.h,v $
 *------------------------------------------------------------------
 * mon_defs.h
 *
 * December 1994, Rob Clevenger
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Miscellaneous platform specific monitor defines.
 *------------------------------------------------------------------
 * $Log: mon_plat_defs.h,v $
 * Revision 3.2  1995/11/17  18:44:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/23  05:58:03  enf
 * CSCdi39189:  100X need insurance against loading incorrect image
 *
 * Revision 2.1  1995/06/07  22:39:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __MON_PLAT_DEFS_H__
#define __MON_PLAT_DEFS_H__

/*
** Timer stuff.
*/
#define MON_TIMER_INTERVAL   4   /* 4ms timer in monitor */
#define MON_TIMER_VAL        (MON_TIMER_INTERVAL << 3)

/*
** The default value for the virtual config register
*/
#define BOOT_DEFAULT	0x0001
#define DEFAULT_CONFIG	(DISABLE_BREAK | BOOT_DEFAULT)
#define SCOPE_TRIGGER_ADDR ((long)0)

/*
** Macros for NVRAM write protection routines
*/
#define WR_ENA_NVRAM  wr_ena_nvram()
#define WR_PROT_NVRAM wr_prot_nvram()

/*
 * Macro for nop's surround accesses that may cause bus errors
 */
#define BUSERR_NOP_X4()                    	\
    asm volatile ("nop; nop; nop; nop");

#define FLUSH_BUFFERS	BUSERR_NOP_X4

/*
 * On M68k platforms, 3,5,6,7,8 byte accesses are impossible
 * so the macros expand out to nothing
 */

#define LD_ACCESS(a, t, tu)
#define LDR_ACCESS(a, t, tu)
#define LDL_ACCESS(a, t, tu)
#define LWR_ACCESS(a, t)
#define LWL_ACCESS(a, t)

#define SD_ACCESS(a, t, tu)
#define SDR_ACCESS(a, t, tu)
#define SDL_ACCESS(a, t, tu)
#define SWR_ACCESS(a, t)
#define SWL_ACCESS(a, t)

#if defined(FUTURE)
#define CHECK_INTERFACE check_interface
#endif /* defined(FUTURE) */
/*
** protos and externs
*/
extern char hw_rev;
extern void pet_the_dog(void);
extern void wr_ena_nvram(void);
extern void wr_prot_nvram(void);
extern void spi_open(void);
extern void spi_close(void);
extern int spi_cmd(unsigned char cmd);

#endif  /* __MON_PLAT_DEFS_H__ */

