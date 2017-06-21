/* $Id: vconsole.c,v 3.6.18.1 1996/03/21 23:33:09 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-rsp2/vconsole.c,v $
 *------------------------------------------------------------------
 * vconsole.c
 *
 * Oct, 95, Steve J. Zhang
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vconsole.c,v $
 * Revision 3.6.18.1  1996/03/21  23:33:09  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/19  01:01:50  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/08  08:59:53  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/01  04:24:43  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.6  1996/02/09  00:02:23  tkam
 * CSCdi47873:  HSA ROMMON needs to add flow control to dbus download
 * Add download flow control to HSA ROMMON.
 *
 * Revision 3.5  1996/02/01  02:27:40  mansonw
 * CSCdi47569:  HSA slave refuses to switch to master state
 * Fixed the HSA slave to gain master during hot insertion/removal
 *
 * Revision 3.4  1996/01/22  09:06:45  mdb
 * CSCdi47263:  change definition of NULL to ((void *) 0) from 0
 *
 * Revision 3.3  1996/01/17  23:32:54  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:48:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:09:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  23:14:13  szhang
 * placeholder for vconsole.c
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** Virtual Console routines.
*/
#include "monitor.h"
#include "dgbus.h"
#include "ci_dbus.h"

#include "queue.h"
#include "dev.h"
#include "cpu_mips.h"

#define DBUS_PORT_IO_AVAIL     0x08
#define NULL ((void *) 0)
#define DBUS_MEM_SIZE 64
#define TERM_CHAR 0
#define DBUS_IMAGE_FILENAME "dbus_image"

vc_t   *vcp;
boolean dbus_hooked;




uchar *compute_dbus_address(int slot_id)
{
    if ((slot_id > 0) && (slot_id < CHASSIS_INTERFACE_SLOT))
	return((uchar *)(KSEG1_ADRSPC_DIAGBUS + DBUS_MEM_SIZE * slot_id));
    else
	return((uchar *)NULL);
}


inline char read_dbus_data(uint slot_id, uint port_id)
{
    char *addr;

    addr = compute_dbus_address(slot_id);
    return(*(addr + port_id));
}


char read_dbus_status(uint slot_id)
{
    char *addr;
    static data;

    addr = compute_dbus_address(slot_id);
    addr += 2;
    return(*addr);
}


inline boolean read_io_available(uint slot_id)
{
    return(read_dbus_status(slot_id) & DBUS_PORT_IO_AVAIL);
}



static inline ushort write_dbus_string(uint slot_id, uint port_id, char *str_data)
{
    char *ptr;
    char *addr;

    ushort status;

    ptr = str_data;
    addr = compute_dbus_address(slot_id);
 
    while (*ptr) {
	*(addr + port_id) = *ptr;
	++ptr;
    	delay_10us(100); 
    }
    *(addr + port_id) = '\n';
}


boolean get_command(char *comstr)
{
    getline(comstr, MONLINESIZ);
}
    

inline boolean check_slave()
{
    return (REG_IRSP2DBUS_FPGA->MasterSlaveStatus & MS_STATUS_THIS_IRSP2_SLAVE)
	? TRUE : FALSE;
}

boolean starttest;
void slavemon(int argc, char *argv[])
{
    uint slot_id, port_id, master_slot;
    char com_string[MONLINESIZ];
    char dbus_data;
    char dbus_status;
    int timeout;

    master_slot = (uint)slot_num();
    if (argc < 2) {
	slot_id = get_slave_slot(master_slot);
	port_id = 0x9;
    } else if (argc < 3) {
	getnum(argv[1], 10, &slot_id);
	port_id = 0x9;
    } else {
	getnum(argv[1], 10, &slot_id);
	getnum(argv[2], 10, &port_id);
    }
    starttest = TRUE;

    while (get_command(com_string)) {
	write_dbus_string(slot_id, port_id, com_string);
    }
    starttest = FALSE;
}


inline void read_dbus(void)
{
    putchar(read_dbus_data(get_slave_slot(), 7));
}

extern void general_exception(r4k_context_t *frame);
extern void handle_interrupt(r4k_context_t *frame, char *str);
extern void rom_dcl_int(r4k_context_t *frame, uint intr_level);
extern void dcl_write_berr(r4k_context_t *frame);

extern char sys_dirty;
void load_dbus_handler(void)
{
    char buffer[128];

    if (check_slave()) {
        sprintf(buffer, "%s%s", DEV_EPROM_ID, DBUS_IMAGE_FILENAME);
        if((vcp = (vc_t *)loadprog(buffer, 0, 0, (char **)0)) == NULL) {  /* load and execute */
            puts("\nfail loading the dbus_image program\n");
        } else {
#ifdef DEBUG
            puts("\nloaded the dbus_image program\n");
#endif
            dbus_hooked = TRUE;
/* added to handle exeception passed from DBUS handler code */
	    vcp->rom_general_exception = general_exception;
	    vcp->rom_handle_interrupt = handle_interrupt;
	    vcp->rom_dcl_interrupt = rom_dcl_int;
	    vcp->rom_dbus_handler = dcl_write_berr;
            vcp->memsize = sizemainmem(); /* pass memory size to dbus */
        }
        sys_dirty = 0;
        fill_icache(); /* preload instruction into the instruction
                          cache to improve the first dbus command
                          handling latency. It may not be necessary.*/
	
        setcp0_sreg( (getcp0_sreg() & ~SREG_BEV) );
#ifdef DEBUG
        printf("vcp = %x\n", vcp);
#endif
    REG_IRSP2DBUS_FPGA->StatusReset |= (DBUS_SW_READY | DBUS_PORT_IO_AVAIL);
    }
}
