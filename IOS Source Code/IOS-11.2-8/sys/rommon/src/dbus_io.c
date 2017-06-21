/* $Id: dbus_io.c,v 3.2.62.1 1996/03/21 23:26:55 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/dbus_io.c,v $
 *------------------------------------------------------------------
 * dbus_io.c
 *
 * Oct, 95, Steve J. Zhang
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dbus_io.c,v $
 * Revision 3.2.62.1  1996/03/21  23:26:55  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:05  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:41:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:23:44  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  23:11:18  szhang
 * placeholder for DBUS handlers in RSP2
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "dbus.h"

#define CONSOLE_FLUSH_TIME	5000	/* 5 secs flush of Tx ring */
#define CONSOLE_TX_DELAY	100	/* 100ms delay between each polled

/*
 * portdisable :
 * Disable the recieve side of the console
 */
static void portdisable (uint port)
{
    dbuscore_p->queue[port].rx_state = FALSE;	/* disable reception of characters */
}


/*
 * portenable :
 * Enable receive and transmit on the DBUS console
 */
static void portenable (uint port)
{
    dbuscore_p->queue[port].rx_state = TRUE;	/* enable reception of characters */
}


/*
 * dbus_write_char:
 * Write one character to the DBUS tx ring
 */
static inline void dbus_write_char (uint port, uchar c)
{
    volatile ulong new_head;

    disable_dcl_int(6);
    if (dbuscore_p->queue[port].tx_head == dbuscore_p->queue[port].tx_tail) {
        dbuscore_p->db.r.regs.port_status.out_avail |= (1 << port);
        dbuscore_p->db.r.regs.status |= DBUS_PORT_IO_AVAIL;
        db_hw->status = dbuscore_p->db.r.regs.status; 
    }
    new_head = (dbuscore_p->queue[port].tx_head + 1) & MAX_DBUS_CHARS_MASK;
    if (new_head != dbuscore_p->queue[port].tx_tail) {
        dbuscore_p->queue[port].tx_q[dbuscore_p->queue[port].tx_head] = c;
        dbuscore_p->queue[port].tx_head = new_head;
    }
    enable_dcl_int(6);
}


/*
 * dbus_putc :
 * Write a byte to the DBUS thru polling
 */
static inline void dbus_putc (uint port, uchar ch) 
{
    if (ch == '\n')
	dbus_putc(port, '\r');

    while (dbus_txhalf_full(port))
	; 
    dbus_write_char(port, ch);
}


/*
 * dbus_getc : 
 * Get a character from the DBUS thru polling
 */
static inline int dbus_getc (uint port)
{
    int c;
    
    /*
     * Wait until the Tx side has drained
     */

    while (!dbus_txempty(port))
	;

    /*
     * Wait for a character from the dbus
     */
    while (TRUE) {
	disable_dcl_int(6); 
	if (dbuscore_p->queue[port].rx_head != dbuscore_p->queue[port].rx_tail) {
	    c = dbuscore_p->queue[port].rx_q[dbuscore_p->queue[port].rx_tail];
	    dbuscore_p->queue[port].rx_tail = (dbuscore_p->queue[port].rx_tail + 1)
		& MAX_DBUS_CHARS_MASK;
	    enable_dcl_int(6);
	    return(c);
	} else {
	    return(-1);
	}
	enable_dcl_int(6); 
    }
}


/*
 * console_putc :
 * Write a byte to the CONSOLE port
 */
void console_putc (uchar ch) 
{
    dbus_putc(CONSOLE_PORT, ch);
}


/*
 * console_getc : 
 * Get a character from the CONSOLE port
 */
int console_getc (void)
{
    return(dbus_getc(CONSOLE_PORT));
}


/*
 * debug_putc :
 * Write a byte to the DEBUG port
 */
void debug_putc (uchar ch) 
{
    dbus_putc(DEBUG_PORT, ch);
}


/*
 * debug_getc : 
 * Get a character from the DEBUG port
 */
int debug_getc ( )
{
    return(dbus_getc(DEBUG_PORT));
}


/* dump queue status */
void print_queue(uint port)
{
    int i;

    printf("tx_h : %d , tx_t : %d, rx_h : %d, rx_t : %d\n",
	   dbuscore.queue[port].tx_head, dbuscore.queue[port].tx_tail,
	   dbuscore.queue[port].rx_head, dbuscore.queue[port].rx_tail);

    printf("\ntx_queue : ");
    for (i = 0; i < MAX_DBUS_CHARS; ++i)
	if (dbuscore.queue[port].rx_q[i] != 0)
	    putchar(dbuscore.queue[port].tx_q[i]);
    printf("\nrx_queue : ");
    for (i = 0; i < MAX_DBUS_CHARS; ++i)
	if (dbuscore.queue[port].rx_q[i] != 0)
	    putchar(dbuscore.queue[port].rx_q[i]);
}























