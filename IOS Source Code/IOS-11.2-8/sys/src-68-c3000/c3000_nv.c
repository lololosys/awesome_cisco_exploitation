/* $Id: c3000_nv.c,v 3.3.46.2 1996/05/21 10:03:52 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c3000/c3000_nv.c,v $
 *------------------------------------------------------------------
 * c3000_nv.c - Routines for IGS/2500/3000 NVRAM manipulation
 *
 * February 1995, Scott Mackie (moved from init_pan.c)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: c3000_nv.c,v $
 * Revision 3.3.46.2  1996/05/21  10:03:52  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.46.1  1996/03/18  21:59:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.3  1996/03/09  05:12:15  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.14.2  1996/03/07  10:43:22  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/01/24  23:01:05  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/12/17  18:37:52  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:43:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:25:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/07  03:18:26  dcoli
 * CSCdi35405:  C1000 NVRAM : simultaneous config accesses can cause crash
 * In nv_done, only call nv_eeprom_done, nv_writedisable and free nvram
 * lock if the calling process is the one that opened nvram for write.
 *
 * Revision 2.1  1995/06/07  22:54:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "../../boot/cpu.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "exception.h"
#include "packet.h"
#include "stacks.h"
#include "logger.h"
#include "config.h"
#include "../os/free.h"
#include "../dev/monitor1.h"
#include "../les/if_les.h"
#include "../os/nv.h"
#include "../les/init_les.h"
#include "../les/nv_config_eeprom.h"
#include "../../boot/src-68-c3000/confreg.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

#define ALTERNATE       NONE
#include "../parser/cfg_configreg.h"
LINK_POINT(configreg_command, ALTERNATE);
#undef  ALTERNATE

/*
 * Local Storage:
 */

static int nv_write_pid; /* Process ID of process opening nvram for write */


/*
 * get_sys_nvsize: Get the NVRAM size usable by the system image
 */
long
get_sys_nvsize(void)
{
    return((long)(mon_get_nvram_size() - (ADRSPC_NVRAM_START - ADRSPC_NVRAM)));
}

/*
 * get_nvbase: Get the pointer to the base of NVRAM usable by the system image
 */
uchar *
get_nvbase(void)
{
    return((uchar *) NVBASE);
}

/*
 * get_nvmagic: Get the pointer to the NVRAM magic number
 */
ushort *
get_nvmagic(void)
{
    return((ushort *) ADRSPC_NVRAM_MAGIC);
}

/*
 * nv_getptr
 * Return pointer to base of non-volatile memory
 */

nvtype *nv_getptr (void)
{
    nvtype *p;

    if (nvptr) {  /* Check lock */
	printf("\nNon-Volatile memory is in use");
	return(NULL);
    }
    if (cpu_type == CPU_CANCUN) {
	p = nv_eeprom_getptr();
    } else {
	p = (nvtype *)get_nvbase();
    }
    if (nv_check_getptr(p)) {
	return(p);
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
    ushort number;

    if (cpu_type == CPU_CANCUN) {

        /*
         * read into 'number', big endian byte at a time.
         */
        number = (eeprom_nvreadbyte((uchar *)config_ptr,0) << 8);
        number &= 0xff00;
        number |= (eeprom_nvreadbyte((uchar *)config_ptr,1) & 0xff);
    } else {
        number = *(ushort *)config_ptr;
    }
    return (number);
}


/*
 * nv_setconfig
 * write a number to the NVRAM configuration register:
 */
void nv_setconfig (
    ushort number)
{
    if (cpu_type == CPU_CANCUN) {

	/* 
	 * store 'number' big endian byte at a time
	 */
	eeprom_nvwritebyte(number >> 8, (uchar *)config_ptr,0);
	eeprom_nvwritebyte(number & 0xff, (uchar *)config_ptr,1);
	/* 
	 * store 'magic number' big endian byte at a time
	 */
	eeprom_nvwritebyte(CONFREG_MAGIC >> 8, (uchar *)config_magic_ptr, 0);
	eeprom_nvwritebyte(CONFREG_MAGIC & 0xff, (uchar *)config_magic_ptr, 1);
    } else if (mon_set_confreg(number) == -1) {
	/*
	 * Set the conf reg here if ROM monitor is old
	 */
	*(ushort *)config_ptr = number;
 	*config_magic_ptr = CONFREG_MAGIC;
    }
}

/*
 * nv_badptr
 * Given a correctly checksummed portion of non-volatile memory, examine
 * the data structure components more closely for sanity.
 * If the configuration magic number is set, return zero.
 */
boolean nv_badptr (nvtype *ptr)
{
    ushort *nv_magic_ptr = get_nvmagic();

    if (cpu_type == CPU_CANCUN) {
	return(nv_eeprom_badptr());
    } else {
	if (*nv_magic_ptr == CONFIG_MAGIC)
	    return(FALSE);
	else
	    return(TRUE);
    }
}

/*
 * nv_done
 * Handles writing configuration data to memory.
 */

void nv_done (nvtype *ptr)
{
    pid_t current_pid;
    ushort *nv_magic_ptr;
  
    nv_check_done(ptr);
    process_get_pid(&current_pid);
    if (nv_writeflag  && (current_pid == nv_write_pid)) {
        if (cpu_type == CPU_CANCUN) {
	    nv_eeprom_done(ptr);
        } else {
    	    nv_magic_ptr = (ushort *)get_nvmagic();
            *nv_magic_ptr = CONFIG_MAGIC;
        }
	nv_writedisable(ptr);

	/* Assume that nvram only locked for writes in nv_write_open. */
	if (ptr == nvptr)           /* Free lock */
	    nvptr = NULL;
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
    malloc_config_eeprom_memory();
    reg_add_config_register_read(0, nv_readconfig, "nv_readconfig");

    parser_add_commands(PARSE_ADD_CFG_TOP_CMD, &pname(configreg_command),
                        "configreg");
}

