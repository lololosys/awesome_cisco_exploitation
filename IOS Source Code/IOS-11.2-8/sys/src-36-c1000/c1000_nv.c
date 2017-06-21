/* $Id: c1000_nv.c,v 3.3.46.2 1996/05/21 10:02:58 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-36-c1000/c1000_nv.c,v $
 *------------------------------------------------------------------
 * c1000_nv.c - Routines for cisco 1000 NVRAM manipulation
 *
 * February 1995, Scott Mackie (moved from init_c1000.c)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: c1000_nv.c,v $
 * Revision 3.3.46.2  1996/05/21  10:02:58  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.46.1  1996/03/18  21:57:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.1  1996/01/24  22:50:04  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/12/17  18:37:31  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:39:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:19:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/07  03:18:15  dcoli
 * CSCdi35405:  C1000 NVRAM : simultaneous config accesses can cause crash
 * In nv_done, only call nv_eeprom_done, nv_writedisable and free nvram
 * lock if
 * the calling process is the one that opened nvram for write.
 *
 * Revision 2.2  1995/07/18  06:48:34  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.1  1995/06/07 22:52:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "config.h"
#include "../dev/monitor1.h"
#include "../les/if_les.h"
#include "../os/nv.h"
#include "../les/nv_config_eeprom.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

#define ALTERNATE       NONE
#include "../parser/cfg_configreg.h"
LINK_POINT(configreg_command, ALTERNATE);
#undef  ALTERNATE

/*
 * Local storage
 */

static pid_t nv_write_pid; /* Process ID of process opening nvram for write */

/*
 * get_sys_nvsize: Get the NVRAM size usable by the system image
 */
long get_sys_nvsize (void)
{
    return((long) (mon_get_nvram_size() - mon_get_mon_nvram_bytes() -
                   NVRAM_START_OFFSET));
}

/*
 * get_nvbase: Get the pointer to the base of NVRAM usable by the system image
 */
uchar *get_nvbase (void)
{
    return((uchar *) (ADRSPC_NVRAM + mon_get_mon_nvram_bytes() +
                      NVRAM_START_OFFSET));
}

/*
 * get_nvmagic: Get the pointer to the NVRAM magic number
 */
ushort *get_nvmagic (void)
{
    return((ushort *) (ADRSPC_NVRAM + mon_get_mon_nvram_bytes() +
                       NVRAM_MAGIC_OFFSET));
}

/*
 * nv_getptr
 * Return pointer to base of non-volatile memory
 */

nvtype *nv_getptr (void)
{
    nvtype *nv_info;

    if (nvptr) {  /* Check lock */
	printf("\nNon-Volatile memory is in use");
	return(NULL);
    }
    nv_info = nv_eeprom_getptr();
    if (nv_info == NULL)
	return(NULL);

    if (nv_check_getptr(nv_info)) {
	return(nv_info);
    } else {
	return(NULL);
    }
}


/*
 * nv_readconfig
 * read NVRAM configuration register
 */
static ushort nv_readconfig (void)
{
    return mon_getconfig();
}


/*
 * nv_setconfig
 * Write a number to the NVRAM configuration register.
 * The config-reg is owned by the Rommon, so let it do it.
 */
void nv_setconfig (ushort number)
{
    leveltype sts;
    sys_timestamp time_now, time_prev;
    sts = raise_interrupt_level(ALL_DISABLE); /* Disable intr except NMI */
    
    /*
     * Wait out till an NMI has occurred. Then go in and do our
     * writes. Each consecutive write should be within 100 usecs,
     * else, the chip will exit from write mode and go into read
     * mode. So we don't want any interrupts within our EMT CALL
     */
    
    GET_TIMESTAMP(time_prev);
    GET_TIMESTAMP(time_now);
    while (TIMERS_EQUAL(time_now, time_prev)) {
	GET_TIMESTAMP(time_now);
    }
    mon_set_confreg(number);
    reset_interrupt_level(sts);  /* Restore interrupt level */
}


/*
 * nv_badptr
 * Given a correctly checksummed portion of non-volatile memory, examine
 * the data structure components more closely for sanity.
 * If the configuration magic number is set, return FALSE.
 */

boolean nv_badptr (nvtype *ptr)
{
    return(nv_eeprom_badptr());
}

/*
 * nv_done
 * Handles writing configuration data to memory.
 */

void nv_done (nvtype *ptr)
{
    pid_t current_pid;

    nv_check_done(ptr);

    process_get_pid(&current_pid);

    if (nv_writeflag  && (current_pid == nv_write_pid)) {
	nv_eeprom_done(ptr);
	nv_writedisable(ptr);

	/* Assume nvram only locked for writes in nv_write_open. */
	
	if (ptr == nvptr)		/* Free lock */
	    nvptr = NULL;
    }

}

/*
 * nv_writeenable - Enable writing to NVRAM
 */

void nv_writeenable (nvtype *ptr)
{
    process_get_pid(&nv_write_pid);
    nv_writeflag = TRUE;
    quicc_write_enable_nvram();
}

/*
 * nv_writedisable - Disable writing to NVRAM; Can still be read
 */

void nv_writedisable (nvtype *ptr)
{
    nv_writeflag = FALSE;
    quicc_write_protect_nvram();
}

/*
 * nv_init
 * Discover the size of the NVRAM
 */

void nv_init (void)
{
    nvptr = NULL;               /* make sure unlocked */

    nvsize = get_sys_nvsize();
    malloc_config_eeprom_memory();
    reg_add_config_register_read(0, nv_readconfig, "nv_readconfig");

    parser_add_commands(PARSE_ADD_CFG_TOP_CMD, &pname(configreg_command),
                        "configreg");
}

