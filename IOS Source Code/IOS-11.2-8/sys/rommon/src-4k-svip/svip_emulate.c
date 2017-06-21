/* $Id: svip_emulate.c,v 1.1.6.1 1996/07/08 22:43:06 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-svip/svip_emulate.c,v $
 *------------------------------------------------------------------
 * svip_emulate.c
 *
 * Mar, 96. Paul Greenfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_emulate.c,v $
 * Revision 1.1.6.1  1996/07/08  22:43:06  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 1.1  1996/03/23  01:23:00  pgreenfi
 * Placeholder files for src-4k-svip.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "emt.h"
#include "mon_defs.h"
#include "cpu.h"
#include "cpu_mips.h"
#include "signal_4k.h"
#include "confreg.h"
#include "mon_boot.h"
#include "extern.h"
#include "proto.h"

struct bootinfo bootinfo = {
    0,			/* protocol identifier 	*/
    "svip-dw-m.ucode",	/* boot filename	*/
    0,			/* ip address 		*/
    "mop",		/* mopaddr		*/
    "null",		/* mop interface 	*/
    "none"		/* mop host		*/
};

static long boot_data_stub[] = {
    BOOTDATA,		/* length of bootstuff 	*/
    EXC_RESET,		/* reason 		*/
    0,			/* PC 			*/
    0,			/* address		*/
    0,			/* stack trace		*/
    0			/* stack last		*/
};


/*
 * emulate:
 * DiagMon EMT call support
 */
unsigned long
emulate(r4k_context_t *frame, unsigned long a0, unsigned long a1,
	unsigned long a2, unsigned long a3)
{
    uchar *ucp;

    switch(a0) {
    
    case EMT_VERSION:
	return((ROM_MAJORVERSION*256)+ROM_MINORVERSION);

    case EMT_GETMEMSIZE:
	return(sizemainmem());
	

    case EMT_PROCTYPE:
	return(cpu_type);

    case EMT_RELOAD:
	setcp0_sreg(getcp0_sreg() | SREG_BEV);  /* use monitor vectors */
	reset(0, 0);
	break;

    case EMT_BOOTIPADDR:
	return(0);

    case EMT_VSTRING:  /* return the monitor version string */
	return((unsigned long)banner_string);

    case EMT_LASTERROR:
	return((long)&boot_data_stub);

    case EMT_GET_REV:
	/* want register 0x22 in eeprom */
	ucp = (uchar *)(PHY_TO_KSEG1(PHY_ADRSPC_DBUS) + 7 + 0x22 * 8);
	return((unsigned long)(*ucp & 0xff));

    case EMT_GET_NVRAM_SIZE:  /* the size of nvram */
	return(0L);

    case EMT_PROCNAME:
	return((unsigned long)platform_string);

    case EMT_GET_BOOTINFO:
	return((int)&bootinfo);

    case EMT_GET_IOSIZE:
	return(sizesram());

    case EMT_NVCOMPRESS_SUPPORT:
	return(0);

    case EMT_SWCONFREG_SUPPORT:
	return(0);

    case EMT_EXCEPTION_TABLE_ADDR:
	return((unsigned long)r4k_exception_table);

    case EMT_CPU_EXCEPTION:
	setcp0_sreg(getcp0_sreg() | SREG_BEV);  /* use monitor vectors */
	system_returned(frame, a1, a2, a3);	/* will not return */
	break;

    case EMT_MON_NVRAM_BYTES:  /* offset from beginning for system config */
	return(0);

    case EMT_SETCONFIG:
	return(0);


    case EMT_TICKS:
    case EMT_PUTCHAR:
    case EMT_GETCHAR:
    case EMT_SETECHO:
    case EMT_GETCONFIG:
    case EMT_CLOCKADDR:
    case EMT_TOUCHR:
    case EMT_HCLOCKADDR:
    case EMT_TOUCHW:
    case EMT_GETCHAR_RAW:
    case EMT_RESET_IO:
    case EMT_GET_IOBASE:
    case EMT_BOOT_CONFREG:
    case EMT_VSTRING_SYS:
    case EMT_VSTRING_SYS_SET:
    case EMT_VSTRING_SYS_ROM:
    case EMT_VSTRING_SYS_ROM_SET:
    case EMT_SETVAR:
    case EMT_GETVAR:
	return(-1);		/* not implemented */


    case EMT_ROMADDR:
    case EMT_CONSOLE:
    case EMT_GETCAP:
    case EMT_SETCAP:
    case EMT_DISABLE_DCACHE:
    case EMT_ENABLE_DCACHE:
    case EMT_SETCACR:
    case EMT_GET_POWERFAIL_ADDR:
    case EMT_GET_COOKIE:
    case EMT_WRITE_BAD_MAIN_MEMORY_PARITY:
    case EMT_SIMBRK:
    case EMT_GET_COOKIE_COPY:
    case EMT_FLASH_LOAD_HELPER:
    case EMT_RESET_REASON:
    default:
	return(-1);
    }
    return(0);
}

/* End of file */

