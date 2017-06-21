/* $Id: extern.c,v 3.4.20.2 1996/05/10 23:16:58 gstovall Exp $
 * $Source: /release/112/cvs/Xboot/extern.c,v $
 *------------------------------------------------------------------
 * extern.c -- definitions for C routines
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: extern.c,v $
 * Revision 3.4.20.2  1996/05/10  23:16:58  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * The beast still exists, finish killing off the CS500, and while I am in
 * there, nuke the even crustier CSC1.
 *
 * Revision 3.4.20.1  1996/05/09  13:59:52  rbadri
 * Branch: California_branch
 * Synergy features
 *
 * Revision 3.4.48.1  1996/04/27  06:12:54  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.4.38.1  1996/04/08  01:34:15  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.4.26.1  1996/03/22  09:06:48  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4.10.1  1996/03/03  21:18:53  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.4.8.1  1996/03/02  01:06:32  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.4  1996/01/30  21:11:00  vandys
 * CSCdi46080:  Deep six CS500 in source base
 *
 * Revision 3.3.26.1  1996/02/29  07:41:51  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.3  1995/12/03  17:54:52  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.2  1995/11/17  07:41:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:11:33  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/10/03  07:04:32  rlowe
 * Re-port IOS Boot for Synergy ATM line card from 10.3 to 11.0.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1  1995/06/07  19:11:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../defs.h"
#include "stdio.h"
#include "../reason.h"
#include "../caps.h"
#include "extern.h"

char cpu_type;				/* CPU type code */
char ciscopro_flag;			/* CiscoPro platform */

#ifdef MC68000
    struct buserrinfo error;		/* bus errors are put here */
#else
    struct buserrinfo *error;		/* bus errors are put here */
#endif

struct confreg config;			/* copy of the config register */

long memsize;				/* memory size in bytes */
#ifdef SYNALC
u_long true_memsize ;          /* memory size + iomem + reserved mem */
#endif


#if defined(PAN) || defined(XX) || defined(SYNALC)
uint	iomem_base;			/* Base address of IO RAM */
uint    iomem_size;			/* size of IO RAM */
uint	revision;			/* Hardware revision number */
uint	nvram_size;			/* Size of NVRAM */
#endif

#ifdef XX
long shmemsize;
#endif

u_short *prom2;				/* system image existance flag */
char brk;				/* break flag */
char abort;				/* abort flag */
char abortstate;			/* what to do with abort */
void (*buserrhandler)();		/* routine to bus error abort */

char breakf;				/* break point in progress flag */
short *breakaddr;			/* break point address */
short breakvalue;			/* instr. replaced by break trap */

u_long clock;				/* ms clock */
u_long hclock;				/* upper ms clock */
#ifdef  SYNALC
long   clk_cor_cnt ;                    /* cur countdown for corrections */
long   clk_cor_adj ;                    /* nbr ticks to correct clock by */
u_long resetrsns ;                      /* reset reasons at boot time */
#endif

char echoing;				/* are we echoing characters */
short linelength;			/* input line length */

long lastloc;				/* last location seen for dep/ex. */
char lastsize;				/* last size seen for dep/ex. */
char lastswap;				/* TRUE if last display byte-swap */
short instrsize;			/* last size of instruction */
u_long instraddr;			/* base address for instr output */

#if defined(MC68020) || defined(MC68040)
    char cache;				/* cache toggle state */
    char i_cache;                       /* instr. cache for 68040 */
    char d_cache;                       /* data cache for 68040 */
#endif

long *baseaddr;				/* where to load net boot progs */
char bootfilename[FNAME_LEN];		/* name of file to bootload */
long bootaddr;				/* IP of host that booted us */
long bootstuff[BOOTDATA+1];		/* contains data about last reboot */

char boot_information[BOOTINFOSIZE];	/* Contains data about netbooted
					 * images */
#ifdef RP1
powerfail_info_t pf_info;		/* Power-fail info */
char boot_satisfied;                        
char sys_dirty;
char monlib_initialized;
#endif

union buffer_union bufu;

struct capabilities caps;		/* Capability list */

char system_state;		/* State of current system image */

uchar vsr_set;			/* has vstring_sys_rom[] been set? */
char vstring_sys_rom[VSTRING_BUFF_SIZE]; /* ROM system version string */
char vstring_sys[VSTRING_BUFF_SIZE];	/* remember previous image version */

#ifdef  SYNALC
unsigned long ALC_Zero = 0 ;    /* Guaranteed to be zero ! */
#endif  SYNALC

/*
 * Messages
 */
char noprom2[] = "No system proms present\n";
char illarg[] = "Illegal argument\n";
