/* $Id: mon_plat_defs.h,v 1.1.68.1 1996/06/16 21:18:53 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-68-as5200/mon_plat_defs.h,v $
 *------------------------------------------------------------------
 * mon_plat_defs.h -- Miscellaneous platform specific monitor defs.
 *
 * October 1995, Joe Turner
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mon_plat_defs.h,v $
 * Revision 1.1.68.1  1996/06/16  21:18:53  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/03  21:16:31  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MON_PLAT_DEFS_H__
#define __MON_PLAT_DEFS_H__

#define TRUE 1
#define FALSE 0

#define NO_COOKIE_PROM_CANCUN 0xFF      /* Cancun own version of no_cookie */
#define ADRSPC_BAD_ADDR                 0xFFFFFFFF /* Bad addr */

/*
** Timer stuff.
*/
#define MON_TIMER_INTERVAL   4
#define MON_TIMER_VAL        (MON_TIMER_INTERVAL * 1000)
#define CTDEVBASEADDR	0x2120000

/*
 * Serial NVRAM stuff
 */
#define X2444_PORT *(uchar *)0x02110060        /* Data register for X2444 */
#define X2444_CE 0x08           /* CE: Chip enable, also seen as CS */
#define X2444_SK 0x04           /* SK: Serial clock in */
#define X2444_DI 0x02           /* DI: Serial data in  */
#define X2444_DO 0x01           /* DO: Serial data out */

/*
 * Flash VPP control register bit definition.
 * N.B. This turns on VPP for both System and Boot flash!
 */
#define CONTROL_FLASH_VPP	0x07
#define BRASIL_FLASH_VPP_MODE	0x01

/*
 * The default value for the virtual config register
 */
#define DEFAULT_CONFIG	(DISABLE_BREAK | BOOT_DEFAULT)
#define BOOT_DEFAULT	0x0001
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

/*
 * protos and externs
 */
extern unsigned long clock, hclock;
extern char brk;
extern unsigned int hw_rev;
extern int wr_ena_nvram(void);
extern int wr_prot_nvram(void);
extern struct cookie *get_cookie(void);
 
#endif  /* __MON_PLAT_DEFS_H__ */

