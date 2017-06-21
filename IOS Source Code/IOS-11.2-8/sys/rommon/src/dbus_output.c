/* $Id: dbus_output.c,v 3.2.30.1 1996/03/21 23:26:59 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/dbus_output.c,v $
 *------------------------------------------------------------------
 * dbus_output.c
 *
 * Oct, 95, Steve J. Zhang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dbus_output.c,v $
 * Revision 3.2.30.1  1996/03/21  23:26:59  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.4.1  1996/02/08  08:57:58  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2  1996/01/17  23:31:14  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.1  1995/11/09  13:05:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/06  23:11:21  szhang
 * placeholder for DBUS handlers in RSP2
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "dbus.h"
#include "dbstatus.h"
#include "queue.h"

/*
 * Routine to output to debugger, console, or logger  port if idle
 *  This is called on the thread of a DBUS register read.
 *  Therefore, no exclusivity should be needed.
 *
 *  Note, ports is typedefed in printf.h with
 *      console = 0,
 *      debug   = 1,
 *      logger  = 2,
 *      MAX_PORTS = 3
 *  These values are used as indices into the arrays below.
 */
void read_port (uint port)
{
    if (dbuscore.queue[port].tx_head == dbuscore.queue[port].tx_tail) {
	write_data(TERM_CHAR);
	if (dbuscore.queue[port].stop_out) {
	    dbuscore.db.r.regs.port_status.out_avail &= 1 << port;
	    dbuscore.db.r.regs.status &= ~DBUS_PORT_IO_AVAIL;
	    write_status();
	} else {
	    dbuscore.queue[port].stop_out = TRUE;
	}
    } else {
	write_data(dbuscore.queue[port].tx_q[dbuscore.queue[port].tx_tail]);
	dbuscore.queue[port].tx_tail = (dbuscore.queue[port].tx_tail + 1) & MAX_DBUS_CHARS_MASK;
	dbuscore.queue[port].stop_out = FALSE;
    }
}

/* 
 * Data has arrived from the RP on the console or debug ports.
 * process this new data and make it available to console or debug
 * code.
 */
void write_port (uint port)
{
    dbuscore.queue[port].rx_q[dbuscore.queue[port].rx_head] = dbuscore.db.data_port.dbus_data;
    dbuscore.queue[port].rx_head = (dbuscore.queue[port].rx_head + 1) & MAX_DBUS_CHARS_MASK;
    if (dbuscore.queue[port].rx_head == dbuscore.queue[port].rx_tail)
	dbuscore.queue[port].rx_tail = (dbuscore.queue[port].rx_tail + 1) & MAX_DBUS_CHARS_MASK;
}


/*
 * Routine to start an output operation.  This is called by the
 * application which has queued a message.
 */
void port_ready_for_read (uint port)
{
    if (port <= LOGGER_PORT)
	if (dbuscore_p->queue[port].rx_head != dbuscore_p->queue[port].rx_tail) {
	    dbuscore_p->db.r.regs.port_status.out_avail |= 1 << port;
	    port_io_avail();   /* Set avail bit in status reg */
	}
}


/*
 * dbus_txempty:
 * Return whether the DBUS tx ring is completely empty
 */
boolean dbus_txempty (uint port)
{
    boolean ret;

    disable_dcl_int(6); 
    ret = (dbuscore_p->queue[port].tx_head == dbuscore_p->queue[port].tx_tail);
    enable_dcl_int(6); 
    return(ret);
}

/*
 * dbus_txfull:
 * Return whether the DBUS Tx ring is full or not
 */
boolean dbus_txfull (uint port)
{
    boolean ret;
    volatile ulong new_head;

    disable_dcl_int(6); 
    new_head = (dbuscore_p->queue[port].tx_head + 1) & MAX_DBUS_CHARS_MASK;
    ret = (new_head == dbuscore_p->queue[port].tx_tail) ? TRUE : FALSE;
    enable_dcl_int(6); 
    return(ret);
}

/*
 * dbus_txhalf_full :
 * Return TRUE if the Tx ring is more than half full
 */
boolean dbus_txhalf_full (uint port)
{
    int tot;

    disable_dcl_int(6); 
    if (dbuscore_p->queue[port].tx_head < dbuscore_p->queue[port].tx_tail) {
        tot = MAX_DBUS_CHARS - dbuscore_p->queue[port].tx_tail + 
	    dbuscore_p->queue[port].tx_head;
    } else {
        tot = dbuscore_p->queue[port].tx_head - dbuscore_p->queue[port].tx_tail;
    }
    enable_dcl_int(6);
    return(tot > (MAX_DBUS_CHARS >> 1));
}

/*
 * dbus_txflush:
 * Flush all pending IO from the DBUS Tx ring
 */
void dbus_txflush (uint port)
{
    disable_dcl_int(6);
    dbuscore_p->queue[port].tx_head = 0;
    dbuscore_p->queue[port].tx_tail = 0;
    enable_dcl_int(6);
}

/*
 * dbus_rxflush:
 * Flush all pending IO from the DBUS Tx ring
 */
void dbus_rxflush (uint port)
{
    disable_dcl_int(6);
    dbuscore_p->queue[port].rx_head = 0;
    dbuscore_p->queue[port].rx_tail = 0;
    enable_dcl_int(6);
}


/*
 * Routine to clear out old messages.  This should be
 * called after a reset, only.  It should be called
 * in conjuction with a clear_status() which will clear
 * the status register. 
 */
void dbus_init (void) 
{
    uint i;

    disable_dcl_int(6);
    for (i=0; i<MAX_PORTS; i++) {
	dbuscore_p->queue[i].tx_head = 0;
	dbuscore_p->queue[i].tx_tail = 0;
	dbuscore_p->queue[i].rx_head = 0;
	dbuscore_p->queue[i].rx_tail = 0;
	dbuscore_p->queue[i].tx_q[0] = 0;
	dbuscore_p->queue[i].rx_q[0] = 0;
    }
    dbuscore_p->db.r.regs.port_status.out_avail = 0;    
    dbuscore_p->db.r.regs.port_status.debug_act = 0;
    dbuscore_p->db.r.regs.port_status.console_act = 0;	
    enable_dcl_int(6);
}


/*
 * console_read_buffer:
 * Transfer upto max characters from DBUS rx ring
 */
int console_read_buffer (char *buf, int max, uint port)
{
    int i = 0;

    while (i<max) {
        disable_dcl_int(6);
        if (dbuscore_p->queue[port].rx_tail == dbuscore_p->queue[port].rx_head) {
            enable_dcl_int(6);
            return(i);
        }
        buf[i] = dbuscore_p->queue[port].rx_q[dbuscore_p->queue[port].rx_tail];
        dbuscore_p->queue[port].rx_tail = (dbuscore_p->queue[port].rx_tail + 1)
	    & MAX_DBUS_CHARS_MASK;
        enable_dcl_int(6);
        i += 1;
    }
    return(i);
}








