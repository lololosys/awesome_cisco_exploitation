/* $Id: extern.h,v 3.4.12.1 1996/03/21 23:27:10 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/extern.h,v $
 *------------------------------------------------------------------
 * extern.h
 *
 * February 1994, Rob Clevenger
 *
 * Copyright (c) 1994 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Miscellaneous monitor external declarations.
 *------------------------------------------------------------------
 * $Log: extern.h,v $
 * Revision 3.4.12.1  1996/03/21  23:27:10  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/19  01:01:03  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/28  01:00:17  szhang
 * CSCdi50105:  C7100 ROMMON code cleanup
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/12/11  19:14:40  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.4  1996/02/17  05:48:43  smaser
 * CSCdi46943:  break command in Rom Monitor causes image checksum
 * validation to fail
 *
 * Revision 3.3  1995/12/03  17:59:46  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.2  1995/11/17  18:41:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __EXTERN_H__
#define __EXTERN_H__

/*
** External declarations
*/

extern long memsize, shmemsize;
extern unsigned long *shmemaddr;
extern long r4k_exception_table[];
extern char *banner_string;
extern char vstring_sys[257];		/* remember previous image version */
extern char vstring_sys_rom[257];	/* ROM system version string */
extern unsigned char vsr_set;			/* has vstring_sys_rom[] been set? */
extern char *platform_string;
extern char *default_monlib_string;
extern int cpu_type;
extern char ciscopro_flag;              /* CiscoPro platform */
extern volatile short buserrcnt; /* Bus error count */
extern char *end, *_fbss;
extern int sys_state;
extern int sys_chksum_disable;
extern char *errlog_start;
extern struct nvram *nvram_ptr;
extern char *compile_info;
extern int netflashbooted;
extern volatile unsigned char envflag, hkeepflags;
extern int optind;
extern unsigned char doauxtest;


#ifdef __SETJMP_H__
extern jmp_buf monjmpbuf, *monjmpptr;
#endif __SETJMP_H__


/* 
 * External for function
 */

#endif /* __EXTERN_H__ */

