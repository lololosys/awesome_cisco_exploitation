/* $Id: rsp_nv.c,v 3.6.20.3 1996/09/10 02:16:03 ahh Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_nv.c,v $
 *------------------------------------------------------------------
 * rsp_nv.c - Routines for RSP NVRAM manipulation
 *
 * February 1995, Scott Mackie (moved from rsp_init.c)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: rsp_nv.c,v $
 * Revision 3.6.20.3  1996/09/10  02:16:03  ahh
 * CSCdi68092:  NVRAM in use errors with service compress-config
 * NVRAM access code fails to clean itself up properly when another user
 * tries to look at the config simultaneously.
 * Branch: California_branch
 *
 * Revision 3.6.20.2  1996/05/21  10:04:39  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.20.1  1996/03/18  22:02:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.2  1996/02/20  18:01:31  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/04  00:49:20  foster
 * CSCdi47925:  HSA write mem cannot update slaves config file.
 *
 * Revision 3.5  1996/01/30  19:16:00  foster
 * CSCdi47228:  HSA slave image needs config parsing disabled
 *
 * Revision 3.4.14.1  1996/01/24  23:04:13  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/12/17  18:38:24  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/05  07:06:32  dbath
 * CSCdi43178:  rommon boot info not kept in sync on slave
 * Ship the three boot related environment variables, and the config
 * register setting, across to the slave whenever autosync is enabled,
 * or whenever the user manually syncs the slave config.
 *
 * Revision 3.2  1995/11/17  18:49:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:28:38  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:58:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "exception.h"
#include "packet.h"
#include "stacks.h"
#include "logger.h"
#include "config.h"
#include "../os/free.h"
#include "../dev/monitor1.h"
#include "../os/nv.h"
#include "../filesys/filesys.h"
#include "../filesys/flh_config.h"
#include "rsp_pcmap.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../les/if_les.h" /* Needed for get_sys_nvsize prototype */
#include "ms_xchng.h"
#include "../h/sched.h"


#define ALTERNATE       NONE
#include "../parser/cfg_configreg.h"
LINK_POINT(configreg_command, ALTERNATE);
#undef  ALTERNATE

#define LONG_ALIGN(x) (((x) + sizeof(long) - 1) & ~(sizeof(long) - 1))

/* 
 * Global Storage
 */
pid_t nv_write_pid; /* Process ID of process opening nvram for write */

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
 * nv_readconfig
 *
 * read NVRAM configuration register
 */

static ushort nv_readconfig (void)
{
    return mon_getconfig();
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

static  boolean firsttime = TRUE;
/*
 * nv_getptr
 *
 * Return pointer to base of non-volatile memory
 */

nvtype *nv_getptr (void)
{
    nvtype *nv_avail;

    if( firsttime ){
	firsttime = FALSE;
	/*
	 *  If its the first time and we are a slave disable config
	 */
	if( reg_invoke_slave_test() ) {
	    return(NULL);
	}
    }

    if (nvptr) {  /* Check lock */
	printf("\nNon-Volatile memory is in use");
	return(NULL);
    }
    /*
     * Open flash config for subsequent processing.
     * If it returns TRUE then this is for reading
     * or for writing with error. nvp will be set
     * to NULL in error (for both read/write). In
     * this case we close flash config. It returns
     * FALSE only when write can be performed. Then
     * We continue to open nvram.
     */
    if (open_flash_config(&nv_avail) == TRUE) {
	if (nv_avail == NULL)
	    close_flash_config();
	return nv_avail;
    }

    nv_avail = (nvtype *)get_nvbase();
    if (nv_check_getptr(nv_avail)) {
	nvptr = nv_avail;
	return(nv_avail);
    } else {
	return(NULL);
    }
}

nvtype *nv_getptr_raw (void)
{
    nvtype *nv_avail;

    flash_config_on = FALSE;
    nv_avail = nv_getptr();
    flash_config_on = TRUE;
    return nv_avail;
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
    auto_set_slave_confreg(number, TRUE);
}

#define CONFIG_MAGIC		0xf0a5

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
    ushort *nv_magic_ptr = (ushort *)get_nvmagic();

    /*
     * To indicate we are done with flash config.
     */
    if (close_flash_config() == TRUE) {
	if (ptr == nvptr) {		/* Free lock */
	    nv_restore_nvptr(ptr);
	    nvptr = NULL;
	}
	return;
    }

    /*
     * Write magic number in NVRAM
     */
    *nv_magic_ptr = CONFIG_MAGIC;
    nv_check_done(ptr);
    if (ptr == nvptr)		/* Free lock */
	nvptr = NULL;
    nv_writedisable(ptr);
}

void nv_done_raw (nvtype *ptr)
{
    flash_config_on = FALSE;
    nv_done(ptr);
    flash_config_on = TRUE;
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
    if ((var == ROMMON_BOOT_VAR) ||
        (var == ROMMON_CONFIG_VAR) ||
        (var == ROMMON_BOOTLDR_VAR))
         return (TRUE);
    else
         return (FALSE);
}

