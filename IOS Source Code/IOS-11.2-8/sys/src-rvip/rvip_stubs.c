/* $Id: rvip_stubs.c,v 3.4.6.3 1996/04/03 22:49:21 jwelder Exp $
 * $Source: /release/112/cvs/Xsys/src-rvip/rvip_stubs.c,v $
 *------------------------------------------------------------------
 * rvip_stubs.c : Checkers specific stubs
 *
 * January 1995, Michael Beesley
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rvip_stubs.c,v $
 * Revision 3.4.6.3  1996/04/03  22:49:21  jwelder
 * CSCdi47348:  lingering references to rvip
 * Branch: California_branch
 *
 * Revision 3.4.6.2  1996/03/21  23:50:58  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.4.6.1  1996/03/18  22:03:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.3  1996/03/16  07:37:43  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.12.2  1996/03/09  05:13:19  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.12.1  1996/03/07  10:45:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4  1996/03/06  21:24:50  dlobete
 * CSCdi49840:  Support for 32 MB DRAM required for DFS on RVIP
 * Map the DRAM according to the size we dynamically sense on board.
 * - For 8 MB RVIPs we'll have 6 MB DRAM and 3 MB of PCI space.
 * - For 32 MB RVIPs we'll have 28 MB DRAM and 4 MB of PCI space.
 *
 * Revision 3.3  1996/01/06  03:23:27  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.2  1995/11/17  18:50:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include <stdarg.h>
#include "ttysrv.h"
#include "../../boot/cpu.h"
#include "interface_private.h"

#include "../os/nv.h"
#include "../os/gdb.h"
#include "../os/boot.h"
#include "../dev/monitor1.h"
#include "../os/timer_generic.h"

#include "../pas/if_pas.h"
#include "../hes/dgbus.h"
#include "../src-vip/vip_dbus.h"

#include "../../boot/emt.h"
#include "../../boot/reason.h"

/*
 * Local storage
 */
static long boot_data_stub[] = {
    BOOTDATA,		/* length of bootstuff 	*/
    EXC_RESET,		/* reason 		*/
    0,			/* PC 			*/
    0,			/* address		*/
    0,			/* stack trace		*/
    0			/* stack last		*/
};

static struct bootinfo_t_ boot_info_stub = {
    0,			/* protocol identifier 	*/
    "rvip-dw-m.ucode",	/* boot filename	*/
    0,			/* ip address 		*/
    "mop",		/* mopaddr		*/
    "null",		/* mop interface 	*/
    "none"		/* mop host		*/
};
    
/*
 * rvip_exception_table_addr :
 * Return the address of the exception table
 */
extern ulong r4k_exception_table[];

ulong rvip_exception_table_addr (void)
{
    return((ulong)r4k_exception_table);
}

/*
 * Routine which depending on how big DRAM is returns
 * DRAM and IO memory sizes.
 */
static ulong rvip_get_dram_or_io_size (boolean get_dramsize)
{
    switch (IO_ASIC_KSEG1->cfgb & CFGB_DRAM_CFG_MASK) {
      case CFGB_DRAM_VAL_MASK_128:
	if (get_dramsize)
	    return(RVIP_DRAM_MEMSIZE_128);
	else
	    return(RVIP_PCI_MEMSIZE_128);

      case CFGB_DRAM_VAL_MASK_32:
	if (get_dramsize)
	    return(RVIP_DRAM_MEMSIZE_32);
	else
	    return(RVIP_PCI_MEMSIZE_32);

      case CFGB_DRAM_VAL_MASK_8:
      default:
	if (get_dramsize)
	    return(RVIP_DRAM_MEMSIZE_8);
	else
	    return(RVIP_PCI_MEMSIZE_8);
    }
}

/*
 * On Checkers, the CIP rom monitor does not support EMT
 * calls. So we provide stubs here
 */
ulong emt_call (int traptype, ...)
{
    va_list ap;
    int ch, sig, code;
    void *context;

    /*
     * Handle EMT_GETMEMSIZE seperately as the compiler uses a 
     * jump table for the switch statement which does not work
     * when the virtual address space is yet to be defined
     */
    if (traptype == EMT_GETMEMSIZE) {
	return(rvip_get_dram_or_io_size(TRUE));
    }

    va_start(ap, traptype);

    switch (traptype) {
	
    case EMT_VERSION:
	return(SYS_MAJORVERSION*256+SYS_MINORVERSION);
	
    case EMT_GETCHAR:
	return(console_getc());

    case EMT_GETCHAR_RAW:
	return(console_getc());

    case EMT_GET_REV:
	return(vip_board_revision());

    case EMT_PUTCHAR:
	ch = va_arg(ap, uchar);
	console_putc(ch);
	return(0);

    case EMT_PROCTYPE:
	return(CPU_VIP);
	
    case EMT_RELOAD:
	rvip_reload(0, 0, NULL);
	break;

    case EMT_CPU_EXCEPTION:
	sig = va_arg(ap, int);
	code = va_arg(ap, int);
	context = va_arg(ap, void *);
	rvip_reload(sig, code, context);
	break;
	
    case EMT_BOOTIPADDR:
	return(0L);
	
    case EMT_VSTRING:
	return((long) "\r\nBootstrap is CIP Rom Monitor\n");

    case EMT_GET_NVRAM_SIZE:
	return(0L);
	
    case EMT_PROCNAME:
	return((long)"VIP");

    case EMT_NVCOMPRESS_SUPPORT:
	return(0);

    case EMT_GET_IOSIZE:
	return(rvip_get_dram_or_io_size(FALSE));

    case EMT_RESET_IO:
	return(rvip_reset_io());
    
    case EMT_SETCONFIG:
	return(0);

    case EMT_MON_NVRAM_BYTES:
	return(0);

    case EMT_EXCEPTION_TABLE_ADDR:
	return(rvip_exception_table_addr());

    case EMT_GET_BOOTINFO:
	return((ulong)&boot_info_stub);

    case EMT_LASTERROR:
	return((ulong)&boot_data_stub);

    case EMT_GET_IOBASE:
    case EMT_GETVAR:
    case EMT_SETVAR:
    case EMT_GETCONFIG:
    case EMT_TOUCHR:
    case EMT_TOUCHW:
    case EMT_TICKS:
    case EMT_ROMADDR:
    case EMT_HCLOCKADDR:
    case EMT_GETCAP:
    case EMT_SETCAP:
    case EMT_CLOCKADDR:
    case EMT_BOOT_CONFREG:
    case EMT_VSTRING_SYS:
    case EMT_VSTRING_SYS_SET:
    case EMT_VSTRING_SYS_ROM:
    case EMT_VSTRING_SYS_ROM_SET:
    case EMT_FAULT_HISTORY_ADDR:
    case EMT_FAULT_HISTORY_BYTES:
    case EMT_WRITE_FAULT_HISTORY:
    default:
	return(-1L);		/* the "not implemented" return */
    }
    va_end(ap);
    return(0L);
}

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

