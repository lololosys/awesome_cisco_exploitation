/* $Id: monitor.h,v 3.2.58.3 1996/07/08 22:42:52 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/monitor.h,v $
 *------------------------------------------------------------------
 * monitor.h
 *
 * July 1993, Rob Clevenger
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines for the Monitor command driven user interface.
 *------------------------------------------------------------------
 * $Log: monitor.h,v $
 * Revision 3.2.58.3  1996/07/08  22:42:52  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.2.58.2  1996/06/16  21:18:25  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2.58.1  1996/03/21  23:27:37  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:41  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:42:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __MONITOR_H__
#define __MONITOR_H__

/*
 * Include type defintions
 */
#include "../../../boot/types.h"
#include "../../../boot/reason.h"
#include "setjmp.h"

/*
 * Include the platform support header files
 */

/*
 * Cisco 4500
 */
#if 	defined(SIERRA)
#include "../../src-4k-c4000/pcmap.h"
#include "../../src-4k/nevasic.h"
#include "../src-4k-c4000/mon_plat_defs.h"
#include "../src-4k-c4000/nvmonvars.h"
#endif	/* SIERRA */

/*
 * Cisco 5200, Brasil
 */
#if	defined(BRASIL)
#include "../../src-68-as5200/as5200_pcmap.h"
#include "../../../boot/src-68-c3000/pcmap.h"
#include "../src-68-as5200/mon_plat_defs.h"
#include "../src-68-as5200/nvmonvars.h"
#endif	/* BRASIL */

/*
 * Cisco 7500, RSP1 and RSP2
 */
#if 	defined(RSP) || defined(RSP2)
#include "../../src-rsp/rsp_pcmap.h"
#include "../src-4k-rsp/mon_plat_defs.h"
#include "../src-4k-rsp/nvmonvars.h"
#include "../src-4k-rsp/regfpga.h"
#endif	/* RSP1, RSP2 */

/*
 * Cisco 1000, Sapphire
 */
#if	defined(SAPPHIRE)
#include "../../src-36-c1000/pcmap.h"
#include "../src-36-c1000/mon_plat_defs.h"
#include "../src-36-c1000/nvmonvars.h"
#endif	/* SAPPHIRE */

/*
 * Cisco 7100
 */
#if 	defined(PREDATOR)
#include "../../src-4k-c7100/c7100_pcmap.h"
#include "../../src-4k-c7100/c7100_fpga.h"
#include "../src-4k-c7100/pred_plat_defs.h"
#include "../src-4k-c7100/pred_nvmonvars.h"
#endif 	/* PREDATOR */

/*
 * Cisco SVIP
 */
#if 	defined(SVIP)
#include "../../src-svip/svip_pcmap.h"
#include "../../src-svip/svip_nevada.h"
#include "../src-4k-svip/svip_vfpga.h"
#include "../src-4k-svip/svip_mon_plat_defs.h"
#include "../src-4k-svip/svip_nvmonvars.h"
#include "../src-4k-svip/stubs.h"
#endif 	/* SVIP */

/*
** defines for the monitor command table
** including monitor flags
*/
/* must be privileged to view or exec privileged commands */
#define MF_PRIVILEGED 1
/* hidden commands never get displayed but can be exec'd anytime */
#define MF_HIDDEN     2

struct monitem {
  char *command;
  int (*procedure)();
  char *description;
  int flags;
};

extern int moncmdsiz;
extern struct monitem moncmd[];

/* defines for hkeepflags (housekeeping flags) */
#define H_USRINT        0x01        /* the user interrupt flag */
#define H_INCFILL       0x02        /* incrementing fill flag */
#define H_MORE          0x04        /* pagination flag */
#define H_PRIVILEGED    0x08        /* privileged mode */
#define H_SCRIPT        0x10        /* processing a monsh script */
#define H_BUSERR        0x80        /* bus error flag */

/* defines for envflag */
#define INPONCT         0x01        /* in power-on confidence test */
#define INRESET         0x02        /* in reset test */
#define INDIAG          0x04        /* in the menu driven diagnostics */
#define INMON           0x08        /* in the monitor */
#define INWARMBOOT      0x100       /* in a warm boot */

/*
** Define the number of lines in monitor history buffer.
** This number must be 1 greater than the number of lines you wish
** displayed by the history command, the current monitor item takes
** one buffer.
*/
#define NUMMONHIST    17
#define MONLINESIZ   128  /* monitor line size */

struct cmdhist {          /* our command history data */
    unsigned short histnum;
    char cmdbuf[MONLINESIZ];
};

extern volatile unsigned char envflag, hkeepflags;
extern jmp_buf monjmpbuf, *monjmpptr;

extern void monitor(int reset);
extern int shift(int count, char *argv[]);
extern int help(void);
extern int rom_reload(void);
extern int priv(int argc, char *argv[]);

#endif  /* __MONITOR_H__ */
