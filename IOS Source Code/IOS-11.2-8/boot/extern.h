/* $Id: extern.h,v 3.4.20.2 1996/05/10 23:17:00 gstovall Exp $
 * $Source: /release/112/cvs/Xboot/extern.h,v $
 *------------------------------------------------------------------
 * extern.h -- definitions for C routines
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: extern.h,v $
 * Revision 3.4.20.2  1996/05/10  23:17:00  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * The beast still exists, finish killing off the CS500, and while I am in
 * there, nuke the even crustier CSC1.
 *
 * Revision 3.4.20.1  1996/05/09  13:59:54  rbadri
 * Branch: California_branch
 * Synergy features
 *
 * Revision 3.4.48.1  1996/04/27  06:12:56  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.4.38.1  1996/04/08  01:34:16  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.4.26.1  1996/03/22  09:06:49  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4.10.1  1996/03/03  21:18:54  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.4.8.1  1996/03/02  01:06:33  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.4  1996/01/30  21:11:01  vandys
 * CSCdi46080:  Deep six CS500 in source base
 *
 * Revision 3.3.26.1  1996/02/29  07:41:52  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.3  1995/12/03  17:54:52  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.2  1995/11/17  07:41:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2.54.1  1996/02/03  06:11:34  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.2.36.1  1995/10/03  07:04:33  rlowe
 * Re-port IOS Boot for Synergy ATM line card from 10.3 to 11.0.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.2  1995/06/21  07:44:57  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 * Ensure 7000 boot roms compile.
 *
 * Revision 2.1  1995/06/07  19:11:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../buff.h"

extern char cpu_type;			/* CPU type code */
extern char ciscopro_flag;		/* CiscoPro platform */
extern char *cputypes[];                /* CPU character array names */

#if defined(STS10X)
# ifdef	lint
    extern short refreshit[];			/* for lint */
# else
    extern short refreshit[REFRESHSIZE];	/* refresh routine in BSS */
# endif
#endif

#ifdef MC68000
    extern struct buserrinfo error;	/* bus errors are put here */
#else
    extern struct buserrinfo *error;	/* bus errors are put here */
#endif

extern struct confreg config;		/* copy of the config register */

extern long memsize;			/* memory size in bytes */
#ifdef	SYNALC
extern u_long	true_memsize ;		/* memory size + iomem + reserved mem */
#endif

#if defined(PAN) || defined(XX) || defined (SYNALC)
extern uint iomem_base;			/* Base address of IO RAM */
extern uint iomem_size;			/* size of IO RAM */
extern uint revision;			/* Hardware revision number */
extern uint nvram_size;			/* Size of NVRAM */
#endif

#ifdef XX
extern long shmemsize;
#endif

extern u_short *prom2;			/* system image existance flag */
extern char brk;			/* break flag */
extern char abort;			/* abort flag */
extern char abortstate;			/* what to do with abort */
extern void (*buserrhandler)();		/* routine to bus error abort */

extern char breakf;			/* break point in progress flag */
extern short *breakaddr;		/* break point address */
extern short breakvalue;		/* instr. replaced by break trap */

extern u_long clock;			/* ms clock */
extern u_long hclock;			/* upper ms clock */
#ifdef  SYNALC
extern long   clk_cor_cnt ;             /* cur countdown for corrections */
extern long   clk_cor_adj ;             /* nbr ticks to correct clock by */
extern u_long resetrsns ;               /* reset reasons at boot time */
#endif


extern char echoing;			/* are we echoing characters */
extern short linelength;		/* input line length */

extern long lastloc;			/* last location seen for dep/ex. */
extern char lastsize;			/* last size seen for dep/ex. */
extern char lastswap;			/* last byte swap done for dep/ex. */
extern short instrsize;			/* last size of instruction */
extern u_long instraddr;		/* base address of instr output */

#if defined(MC68020) || defined(MC68040)
    extern char cache;			/* cache toggle state */
    extern char i_cache;                /* instr. cache for 68040 */
    extern char d_cache;                /* data cache for 68040 */
#endif

extern long *baseaddr;			/* where to load net boot progs */
extern char bootfilename[];		/* name of file to bootload */
extern long bootaddr;			/* IP of host that booted us */
extern long bootstuff[BOOTDATA+1];	/* data from last reboot */

extern char boot_information[BOOTINFOSIZE]; /* Boot information */

extern union buffer_union bufu;		/* Input buffers */

extern struct capabilities caps;	/* Capability list */

extern char system_state;		/* State of running system */

extern uchar vsr_set;			/* has vstring_sys_rom[] been set? */
extern char vstring_sys_rom[VSTRING_BUFF_SIZE]; /* rom version string */
extern char vstring_sys[VSTRING_BUFF_SIZE]; /* previous image version */

#ifdef RP1
#include "../sys/os/clock_format_string_lengths.h"
#include "../sys/hes/powerfail.h"
extern powerfail_info_t pf_info;
extern char boot_satisfied;                        
extern char sys_dirty;
extern char monlib_initialized;
#endif

#ifdef  SYNALC
extern unsigned long ALC_Zero ;         /* Guaranteed to be zero ! */
#endif  SYNALC


/*
 * Messages
 */
extern char noprom2[];
extern char illarg[];

long catchbe();				/* catch and ignore bus errors */
long ebuserror();                       /* catch and process bus errors */
