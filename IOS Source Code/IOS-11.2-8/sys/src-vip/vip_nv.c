/* $Id: vip_nv.c,v 3.1.62.2 1996/05/21 10:05:00 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/vip_nv.c,v $
 *------------------------------------------------------------------
 * vip_nv.c - Routines for VIP NVRAM support
 *
 * May 1995, Michael Beesley
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vip_nv.c,v $
 * Revision 3.1.62.2  1996/05/21  10:05:00  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.62.1  1996/03/21  23:52:33  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.1  1996/02/10  00:22:38  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/27  18:34:49  getchell
 * Placeholder for VIPER development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface.h"
#include "sys_registry.h"
#include "packet.h"
#include "stacks.h"
#include "logger.h"
#include "exception.h"
#include "ttysrv.h"
#include "config.h"
#include "parser.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../os/gdb.h"
#include "../dev/monitor1.h"
#include "../../boot/caps.h"
#include "../os/nv.h"

/*
 * Local storage
 */
boolean nv_writeflag;

/*
 * nv_init :
 * Discover the size of the NVRAM
 */
void nv_init (void)
{
    nvptr = NULL;               /* make sure unlocked */
    nvsize = 0;
}

/*
 * nv_getptr :
 * Return pointer to base of non-volatile memory
 */
nvtype *nv_getptr (void)
{
    return(NULL);
}

/*
 * nv_setconfig :
 * write a number to the NVRAM configuration register:
 */
void nv_setconfig (ushort number)
{
}

/*
 * nv_badptr :
 * Given a correctly checksummed portion of non-volatile memory, examine
 * the data structure components more closely for sanity.
 * If the configuration magic number is set, return zero.
 */

boolean nv_badptr (nvtype *ptr)
{
    return(FALSE);
}

/*
 * nv_done :
 * Handles writing configuration data to memory.
 */
void nv_done (nvtype *ptr)
{
    nv_check_done(ptr);
    if (ptr == nvptr)		/* Free lock */
	nvptr = NULL;
    nv_writedisable(ptr);
}

/*
 * nv_writeenable :
 * Set the NVRAM write semaphore
 */
void nv_writeenable (nvtype *ptr)
{
    nv_writeflag = TRUE;
}

/*
 * nv_writedisable :
 * Clear the NVRAM write semaphore
 */
void nv_writedisable (nvtype *ptr)
{
    nv_writeflag = FALSE;
}

/*
 * nv_write_summary
 */
void nv_write_summary (boolean eraseflag) 
{
}

/* 
 * platform_nvvar_support
 * NVRAM variables supported
 */
boolean platform_nvvar_support (int var)
{
    if ((var == ROMMON_BOOT_VAR) ||
        (var == ROMMON_CONFIG_VAR) ||
        (var == ROMMON_BOOTLDR_VAR))
         return (TRUE);
    else
         return (FALSE);
}

/* end of file */
