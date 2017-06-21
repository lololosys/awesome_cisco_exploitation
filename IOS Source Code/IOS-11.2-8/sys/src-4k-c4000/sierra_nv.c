/* $Id: sierra_nv.c,v 3.3.46.3 1996/05/21 10:03:18 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c4000/sierra_nv.c,v $
 *------------------------------------------------------------------
 * sierra_nv.c - Routines for c4500 NVRAM support
 *
 * March 1995, Scott Mackie (moved from init_sierra.c)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sierra_nv.c,v $
 * Revision 3.3.46.3  1996/05/21  10:03:18  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.46.2  1996/05/10  19:21:00  tangiral
 * CSCdi51059:  Doing write mem and sho config, same time crashes router.
 * Branch: California_branch
 * C4000 NVRAM: simultaneous config accesses can cause crash.  In nv_done,
 * only call nv_writedisable and free nvram lock if the calling process
 * is the one that opened nvram for write.
 *
 * Revision 3.3.46.1  1996/03/18  21:58:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.1  1996/01/24  22:50:42  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/12/17  18:37:42  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:41:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:23:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:53:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
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
#include "../les/if_les.h"
#include "../os/nv.h"
#include "../les/init_les.h"
#include "../if/tring_common.h"
#include "../les/if_les_madge.h"
#include "../../boot/src-68-c4000/confreg.h"
#include "../h/sched.h"


#define ALTERNATE       NONE
#include "../parser/cfg_configreg.h"
LINK_POINT(configreg_command, ALTERNATE);
#undef  ALTERNATE

/*
 * Local storage
 */

static pid_t nv_write_pid; /* Process ID of process opening nvram for write */


 

/*
 * get_sys_nvsize
 *
 * Get the NVRAM size usable by the system image
 */

long get_sys_nvsize(void)
{
    return((long) LONG_ALIGN(mon_get_nvram_size() - 
			     mon_get_mon_nvram_bytes() -
			     NVRAM_START_OFFSET));
}

/*
 * get_nvbase
 *
 * Get the pointer to the base of NVRAM usable by the system image
 */

uchar *get_nvbase(void)
{
    return((uchar *) LONG_ALIGN(ADRSPC_NVRAM +
				mon_get_mon_nvram_bytes() + 
				NVRAM_START_OFFSET));
}

/*
 * nv_getptr
 *
 * Return pointer to base of non-volatile memory
 */

nvtype *nv_getptr (void)
{
    nvtype *nv_avail;

    if (nvptr) {  /* Check lock */
	printf("\nNon-Volatile memory is in use");
	return(NULL);
    }
    nv_avail = (nvtype *)get_nvbase();
    if (nv_check_getptr(nv_avail)) {
	return(nv_avail);
    } else {
	return(NULL);
    }
}

/*
 * nv_readconfig
 *
 * read NVRAM configuration register
 */

static ushort nv_readconfig (void)
{
    return mon_getconfig();
}


/*
 * nv_setconfig
 *
 * write a number to the NVRAM configuration register:
 */

void nv_setconfig (ushort number)
{
    /*
     * TDB: Need to add a check that reads back the confreg
     * to sanity check the ROM monitor doing
     */
    mon_set_confreg(number);
}

/*
 * get_nvmagic
 *
 * Get the pointer to the NVRAM magic number
 */

ushort *get_nvmagic(void)
{
    return((ushort *) (ADRSPC_NVRAM + mon_get_mon_nvram_bytes() + 
		       NVRAM_MAGIC_OFFSET));
}


/*
 * nv_badptr
 *
 * Given a correctly checksummed portion of non-volatile memory, examine
 * the data structure components more closely for sanity.
 * If the configuration magic number is set, return zero.
 */

boolean nv_badptr (nvtype *ptr)
{
    ushort *nv_magic_ptr = get_nvmagic();
	
    if (*nv_magic_ptr == CONFIG_MAGIC)
	return(FALSE);
    else
	return(TRUE);
}


/*
 * nv_done
 *
 * Handles writing configuration data to memory.
 */

void nv_done (nvtype *ptr)
{
    pid_t current_pid;
    ushort *nv_magic_ptr = (ushort *)get_nvmagic();

    process_get_pid(&current_pid);

    /*
     * Write magic number in NVRAM
     */
    *nv_magic_ptr = CONFIG_MAGIC;
    nv_check_done(ptr);
    if (nv_writeflag  && (current_pid == nv_write_pid)) {

    	if (ptr == nvptr)		/* Free lock */
		nvptr = NULL;
    	nv_writedisable(ptr);
    }	
}

void nv_writeenable (nvtype *ptr)
{
    process_get_pid(&nv_write_pid);
    nv_writeflag = TRUE;
}

void nv_writedisable (nvtype *ptr)
{
    nv_writeflag = FALSE;
}

/*
 * nv_init
 * Discover the size of the NVRAM
 */

void nv_init (void)
{
    nvptr = NULL;               /* make sure unlocked */
    
    nvsize = get_sys_nvsize();
    reg_add_config_register_read(0, nv_readconfig, "nv_readconfig");
    
    parser_add_commands(PARSE_ADD_CFG_TOP_CMD, &pname(configreg_command),
                         "configreg");
}

/* 
 * platform_nvvar_support
 * NVRAM variables supported
 */
boolean platform_nvvar_support (int var)
{
    if (var == ROMMON_BOOT_VAR)
        return (TRUE);
    else
        return (FALSE);
}
