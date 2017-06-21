/* $Id: dbus_main.c,v 3.4.18.2 1996/03/29 03:25:57 tkam Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/dbus_main.c,v $
 *------------------------------------------------------------------
 * dbus_main.c
 *
 * Oct, 95, Steve J. Zhang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dbus_main.c,v $
 * Revision 3.4.18.2  1996/03/29  03:25:57  tkam
 * CSCdi52192:  ROMs forget crash details.
 * Branch: California_branch
 * Changed dbus initialization and system_returned() .
 *
 * Revision 3.4.18.1  1996/03/21  23:26:57  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/19  01:01:00  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/08  08:57:57  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:06  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.4  1996/02/09  00:01:48  tkam
 * CSCdi47873:  HSA ROMMON needs to add flow control to dbus download
 * Add download flow control to HSA ROMMON.
 *
 * Revision 3.3  1996/01/17  23:31:13  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:41:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:23:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  23:11:19  szhang
 * placeholder for DBUS handlers in RSP2
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "cpu_mips.h"
#include "dbus.h"
#include "dbstatus.h"
#include "queue.h"

typedef struct vector_table_ {
    uchar vect_utlb[0x80];
    uchar vect_xtlb[0x80];
    uchar vect_cerr[0x80];
    uchar vect_gexp[0x80];
}vector_table;

dbuscore_t dbuscore;
dbuscore_t *dbuscore_p;
vc_t       vc_intf;

main() {
    /*
     ** hookExceptions -- point to exception vector in KSEG0 memory
     */
    extern uchar vec_start[];

    dbuscore_p = &dbuscore;
    dbus_early_init(); 
    /* turn software ready and download flow control bits on in DRAM copy */
    dbuscore_p->db.r.regs.status |= (DBUS_RSP_READY | DBUS_PORT_IO_AVAIL);
    dbuscore_p->db.r.regs.ctrl |= CNTL_WCS_LOADED;
#ifdef DEBUG 
    printf("starting the dbus image code.\n");
    printf("Initializing DBUS structure.\n");
#endif
    *(vector_table *)0x80000000 = *(vector_table *)vec_start;
    cache_flush(0, -1);
    setcp0_sreg( (getcp0_sreg() & ~SREG_BEV) );
#ifdef DEBUG
    printf("Vectors copied, BEV bit cleared, timer interrupt disabled.\n");
    printf("MASK = %x ", REG_IRSP2DBUS_FPGA->DBusFPGAIntMask);
#endif
    REG_IRSP2DBUS_FPGA->DBusFPGAIntMask &=
	~(IRSP2DBUS_FPGA_INT_SLAVE_REQ_MASK | IRSP2DBUS_FPGA_INT_CMD_FAIL_MASK);
#ifdef DEBUG
    printf("Unmasked CMD_FAIL/SLAVE_REQ bits = %x.\n", REG_IRSP2DBUS_FPGA->DBusFPGAIntMask);
#endif
    vc_intf.getc = console_getc;
    vc_intf.putc = console_putc;
    vc_intf.flush = dbus_init;
    exit(&vc_intf);
}








