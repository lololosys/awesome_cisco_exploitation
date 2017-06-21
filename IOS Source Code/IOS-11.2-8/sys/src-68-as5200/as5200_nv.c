/* $Id: as5200_nv.c,v 1.1.70.2 1996/07/02 23:55:34 jturner Exp $
 * $Source: /release/112/cvs/Xsys/src-68-as5200/as5200_nv.c,v $
 *------------------------------------------------------------------
 * as5200_nv.c - Platform NVRAM routines for AS5200
 *
 * October 1995, Joe Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: as5200_nv.c,v $
 * Revision 1.1.70.2  1996/07/02  23:55:34  jturner
 * CSCdi61176:  post-brasil-commit code cleanup in tdm, asm, src and
 * makefile
 * Branch: California_branch
 *
 * Revision 1.1.70.1  1996/06/16  21:19:37  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/03  21:25:13  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
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
#include "../../boot/cpu.h"
#include "../../boot/src-68-c3000/confreg.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "sched.h"

#define ALTERNATE       NONE
#include "../parser/cfg_configreg.h"
LINK_POINT(configreg_command, ALTERNATE);
#undef  ALTERNATE

/*
 * Local Storage:
 */

static int nv_write_pid; /* Process ID of process opening nvram for write */

/*
 * Defines
 */
#define NVRAM_MAGIC_OFFSET      0x06       /* Magic number NVRAM config offset*/
#define NVRAM_START_OFFSET      0x08       /* NVRAM start offset. */


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
    nvtype *p;

    if (nvptr) {  /* Check lock */
	printf("\nNon-Volatile memory is in use");
	return(NULL);
    }
    p = nv_eeprom_getptr();

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
ushort nv_readconfig (void)
{
    return mon_getconfig();
}


/*
 * nv_setconfig
 * write a number to the NVRAM configuration register:
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
 * If the configuration magic number is set, return zero.
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

