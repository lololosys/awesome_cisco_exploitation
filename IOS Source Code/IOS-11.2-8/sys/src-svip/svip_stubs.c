/* $Id: svip_stubs.c,v 3.1.62.3 1996/05/22 14:34:55 getchell Exp $
 * $Source: /release/112/cvs/Xsys/src-svip/svip_stubs.c,v $
 *------------------------------------------------------------------
 * svip_stubs.c : Viper specific stubs
 *
 * January 1995, Michael Beesley
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_stubs.c,v $
 * Revision 3.1.62.3  1996/05/22  14:34:55  getchell
 * Enable emt call support
 * CSCdi48894:  show hardware in if-con will not display proper memory size
 * Branch: California_branch
 *
 * Revision 3.1.62.2  1996/04/03  22:49:26  jwelder
 * CSCdi47348:  lingering references to rvip
 * Branch: California_branch
 *
 * Revision 3.1.62.1  1996/03/21  23:51:39  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.1  1996/02/10  00:22:13  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/26  21:18:47  getchell
 * Placeholder files for Viper development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "../../boot/cpu.h"
#include <stdarg.h>
#include "ttysrv.h"

#include "../src-4k/signal_4k.h"

/*
 * restart_watchdog :
 */
void restart_watchdog (int t)
{
}

/*
 * restart_t2 :
 */
void restart_t2 (void)
{
}

/*
 * disable_watchdog :
 */
int disable_watchdog (void)
{
    return(0);
}

/*
 * nv_bcopy :
 */
void nv_bcopy (void const *src, void *dst, int length)
{
}

/*
 * write_berr_interrupt
 * Note: This routine does not ever get called. Instead,
 * the error interrupt handler gets called for everything, 
 * including write bus errors.
 */
void write_berr_interrupt (int code, r4k_context_t *p)
{
}
/* end of file */

