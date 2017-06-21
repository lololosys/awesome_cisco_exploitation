/* $Id: vip_dbus.c,v 3.5.6.5 1996/07/16 17:27:40 getchell Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/vip_dbus.c,v $
 *------------------------------------------------------------------
 * vip_dbus.c : VIP and Checkers DBus support
 *
 * March 1995, Michael Beesley 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: vip_dbus.c,v $
 * Revision 3.5.6.5  1996/07/16  17:27:40  getchell
 * CSCdi63024:  vip console output blocks processor
 * Branch: California_branch
 *
 * Revision 3.5.6.4  1996/07/10  02:38:11  getchell
 * Added crash context mechanism which is shown via diag command and test
 * cbus (vc command)
 * CSCdi60415:  VIP difficult to debug in the field
 * Branch: California_branch
 *
 * Revision 3.5.6.3  1996/05/21  10:04:59  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.6.2  1996/03/21  23:52:22  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.5.6.1  1996/03/18  22:03:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.3  1996/03/18  23:36:36  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/10  00:22:30  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1996/01/16  02:36:17  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.5  1996/03/06  10:20:50  feisal
 * CSCdi47327:  Need means of getting RVIP stack trace
 *
 * Revision 3.4  1996/02/22  14:38:38  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1995/12/17  18:38:36  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:51:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:38  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files 
 */
#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "packet.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "config.h"
#include "parser.h"
#include "stacks.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"

#include "../pas/if_pas.h"
#include "../ts/nonvol_x2444.h"
#include "../hes/dgbus.h"
#include "../hes/if_vip.h"
#include "../os/gdb.h"

#include "vip_dbus.h"
#include "vip_dbus_chain.h"

/*
 * Global storage
 */
static uchar vip_crash_context[DBUS_CRASH_LEN];
static volatile char log_tx_r[MAX_DBUS_CHARS];	/* logger character Tx ring */
static volatile ulong log_tx_h=0, log_tx_t=0;	/* logger tx ring head, tail */
static volatile boolean log_stop_out=FALSE;	/* no more log output */
static volatile int crash_h=0;                  /* crash ring head */

/*
 * Parse chains
 */
static parser_extension_request vip_dbus_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(show_vip_dbus_command) },
    { PARSE_LIST_END, NULL }
};

/*
 * vip_crash_read:
 */
static inline uchar vip_crash_read (void)
{
    uchar data_out;

    data_out = vip_crash_context[crash_h++];
    if (crash_h == DBUS_CRASH_LEN) {
	dbus_cntl.r.regs.io_status &= ~DBUS_PORT_CRASH;
	if (!(dbus_cntl.r.regs.io_status & DBUS_PORT_MASK))
	    dbus_cntl.r.regs.diag_status.value &= ~DBUS_PORT_IO_AVAIL;
    }
    return(data_out);
}

static void vip_crash_setup (void *c)
{
    r4k_context_t *ptr = (r4k_context_t *)c;
    int i;
    ulong *buffer_ptr, *frame, *pc;

    *(volatile ulong *)(R4K_K1BASE + 0x24C) = 0x61616161;
    *(volatile ulong *)(R4K_K1BASE + 0x24C) = (ulong)vip_crash_context;

    /* 
     * Prepare crash context
     */
    bcopy(ptr, vip_crash_context, sizeof(r4k_context_t));
    crash_h = 0;

    /*
     * See if we can get a stack trace using the EPC register
     */
    pc = (ulong *)ptr->epc;
    frame = (ulong *)ptr->regs[(29*2)+1];
    buffer_ptr = (ulong *)&vip_crash_context[sizeof(r4k_context_t)];
    for (i=0; i<CRASH_TRACEBACK_COUNT; i++) {
	frame = trace_caller(frame, &pc);
	if (frame == NULL)
	    break;
	*buffer_ptr++ = (ulong)pc;
    }

    if (!i) {
	pc = (ulong *)r4k_get_cpu_reg(ptr, R4K_RA_REG);
	frame = (ulong *)r4k_get_cpu_reg(ptr, R4K_SP_REG);
	for (i=0; i<CRASH_TRACEBACK_COUNT; i++) {
	    frame = trace_caller(frame, &pc);
	    if (frame == NULL)
		break;
	    *buffer_ptr++ = (ulong)pc;
	}

    }

    /*
     * fill the remainder of the traceback portion of the buffer with nulls
     */
    while ( i++ < CRASH_TRACEBACK_COUNT) {
	*buffer_ptr++ = 0;
    }
    *(volatile ulong *)(R4K_K1BASE + 0x24C) = 0x61616122;
}

/*
 * vip_crash_dump
 */
void vip_crash_dump (void *c)
{
    leveltype level;
    int i;

    vip_crash_setup(c);

    GLOBAL_INTERRUPT_BLOCK(level);

    dbus_cntl.r.regs.io_status |= DBUS_PORT_CRASH;
    dbus_cntl.r.regs.diag_status.value |= DBUS_PORT_IO_AVAIL;
    dbus_cntl.dbus_reg->status = dbus_cntl.r.regs.diag_status.value;

    GLOBAL_INTERRUPT_UNBLOCK(level);

    /*
     * Wait for the crash output to drain. Do not wait more than
     * 1 minute.
     */
    for (i=0; i<600000; i++) {
	if (crash_h == DBUS_CRASH_LEN)
	    break;
	usecdelay(100);            /* wait 100 usec */
	*(volatile ulong *)(R4K_K1BASE + 0x24C) = (ulong)crash_h;
    }
}

/*
 * vip_logger_empty:
 * Return whether the logger tx ring is completely empty
 */
static inline boolean vip_logger_empty (void)
{
    leveltype level;
    boolean ret;

    GLOBAL_INTERRUPT_BLOCK(level);
    ret = (log_tx_h == log_tx_t);
    GLOBAL_INTERRUPT_UNBLOCK(level);
    return(ret);
}

/*
 * vip_logger_read : 
 * Reads the characters written on the logger port, one by one.
 */
static inline uchar vip_logger_read (void)
{
    uchar data_out;

    if (log_tx_h == log_tx_t) {
	data_out = DBUS_TERM_CHAR;
	if (log_stop_out) {
	    dbus_cntl.r.regs.io_status &= ~DBUS_PORT_LOGGER;
	    if (!(dbus_cntl.r.regs.io_status & DBUS_PORT_MASK))
		dbus_cntl.r.regs.diag_status.value &= ~DBUS_PORT_IO_AVAIL;
	} else {
	    log_stop_out = TRUE;
	}
    } else {
	data_out = log_tx_r[log_tx_t];
	log_tx_t = (log_tx_t + 1) & MAX_DBUS_CHARS_MASK;
	log_stop_out = FALSE;
    }
    return(data_out);
}

/*
 * vip_logger_write_str : 
 * Writes a string on the logger port (the previous string is overwritten). 
 * Note: This routine is blocking. It waits for atmost 10 mins. or until
 * the host has drained the logger port. 
 * Sometime in the future, the log messages should be buffered and this 
 * routine should be made non-blocking.
 */
void vip_logger_write_str (uchar msg_sev, uchar *str)
{
    leveltype level;
    int i;
    volatile ulong new_head;

    GLOBAL_INTERRUPT_BLOCK(level);

    /* The host decrements the first byte by one to get the message severity */
    log_tx_r[0] = msg_sev + 1; 
    log_tx_h = 1;
    log_tx_t = 0;

    while (*str != '\0') {
        new_head = (log_tx_h + 1) & MAX_DBUS_CHARS_MASK;
        if (new_head != log_tx_t) {
            log_tx_r[log_tx_h] = *str;
            log_tx_h = new_head;
        }
        str++;
    }

    dbus_cntl.r.regs.io_status |= DBUS_PORT_LOGGER;
    dbus_cntl.r.regs.diag_status.value |= DBUS_PORT_IO_AVAIL;
    dbus_cntl.dbus_reg->status = dbus_cntl.r.regs.diag_status.value;

    GLOBAL_INTERRUPT_UNBLOCK(level);

    /*
     * Wait for the logger output to drain. Do not wait more than
     * 10 minutes.
     */
    for (i=0; i<6000000; i++) {
	if (vip_logger_empty()) {
	    break;
	}
	usecdelay(100);            /* wait 100 usec */
    }
}

/*
 * vip_dbus_init : 
 * Initialize DBus support
 */
void vip_dbus_init (void)
{
}

/*
 * dbus_control_write :
 * Manage WCS state machine dance. Process dbus requests
 * to register 5, control.
 */
static inline void dbus_control_write (data_port_type *local_dp,
				       dbus_hardware_regs *hw)
{
    /* 
     * Grab the new control register from the dbus
     */
    dbus_cntl.r.regs.diag_control.value = local_dp->dbus_data; 

    /*
     * Set the Enable LED according to the value in the control register.
     */
    if (dbus_cntl.r.regs.diag_control.value & ENABLE_LED) {
	vip_set_enable_led(TRUE);
    } else {
	vip_set_enable_led(FALSE);
    }
	
    if (!(dbus_cntl.r.regs.diag_control.value & MASTER_ENABLE) ||
	!(dbus_cntl.r.regs.diag_control.value & WCS_LOADED )   ||
	(dbus_cntl.r.regs.diag_control.value & WCS_DBUS_CMD_ENABLE) ) {

	/* 
	 * If MASTER_ENABLE = 0 or WCS_LOADED = 0 or WCS_CMD_ENABLE = 1
	 * then the board should be stopped
	 */
	hw->status = dbus_cntl.r.regs.diag_status.value;
	vip_stop_microcode();
    }

    if (!(dbus_cntl.r.regs.diag_control.value & MASTER_ENABLE) &&
	!(dbus_cntl.r.regs.diag_control.value & WCS_DBUS_CMD_ENABLE) &&
	!(dbus_cntl.r.regs.diag_control.value & WCS_LOADED ) )  {

	/*
	 * If MASTER_ENABLE = 0 and WCS_CMD_ENABLE = 0 and 
	 * WCS_LOADED = 0, then load from ROM. Nop on VIP.
	 * Turn on LOADED and turn off LOADING bits
	 */
	dbus_cntl.r.regs.diag_control.value |= WCS_LOADED;
	dbus_cntl.r.regs.diag_control.value &= ~((uchar) WCS_LOADING);

    } else if ((dbus_cntl.r.regs.diag_control.value & MASTER_ENABLE) &&
	       (dbus_cntl.r.regs.diag_control.value & WCS_LOADED )   &&
	       !(dbus_cntl.r.regs.diag_control.value & WCS_DBUS_CMD_ENABLE) ){

	/*
	 * If MASTER_ENABLE = 1 and WCS_LOADED = 1 and WCS_CMD_ENABLE = 0
	 * then startup the operational code. It is running already.
	 */
	return;
    }
}

/*
 * vip_dbus_interrupt_handler :
 * Handle a DBus interrupt
 *
 * The following commands won't generate interrupts:
 *   0x00      RO -- Physical Slot
 *   0x01      RO -- Last command/Address
 *   0x02      RW -- Read, status reg, write, board reset
 *   0x04      RW -- Logical slot
 *   
 * All other commands are handled in software and result in an
 * interrupt to the processor.  The ones that we care about are:
 *   0x01,     WO -- Reload all VPLD's
 *   0x05      RW -- Control
 *   0x07      RW -- Microcode command/data
 *   0x20-0x3f RW -- EEPROM
 *
 * All other commands will be handled by software as an error 
 * condition.  0xff will be returned on the DBUS.
 */
void vip_dbus_interrupt_handler (void)
{
    dbus_hardware_regs *db_regs;
    data_port_type dp_copy;
    uchar addr;
    int c;

    db_regs = dbus_cntl.dbus_reg;
    *(ushort *)(&dp_copy) = *(ushort *)(&db_regs->data_port);

    if (dp_copy.ad.addrs & PORT_CMD_FAIL) {
	/*
	 * We failed to respond in the 20usec timeout.
	 * Register the error, clear the error in the hw.
	 * Turn on bits 0 and 6 in the dbus status register.
	 */
	dbus_cntl.lost_count += 1;
	db_regs->status = dbus_cntl.r.regs.diag_status.value;
	return;
    }

    addr = dp_copy.ad.addrs & PORT_DBUS_ADDRS;	/* get address field */

    if (dp_copy.ad.addrs & PORT_DBUS_READ) {

	switch(addr) {
	    
	case DB_CONSOLE:
	    if (dbus_cntl.tx_h == dbus_cntl.tx_t) {
		db_regs->data_port.ad.data_out = DBUS_TERM_CHAR;
		if (dbus_cntl.stop_out) {
		    dbus_cntl.r.regs.io_status &= ~DBUS_PORT_CONSOLE;
		    if (!(dbus_cntl.r.regs.io_status & DBUS_PORT_MASK))
			dbus_cntl.r.regs.diag_status.value &= ~DBUS_PORT_IO_AVAIL;
		    db_regs->status = dbus_cntl.r.regs.diag_status.value;
		} else {
		    dbus_cntl.stop_out = TRUE;
		}
	    } else {
		db_regs->data_port.ad.data_out = 
		    dbus_cntl.tx_r[dbus_cntl.tx_t];
		dbus_cntl.tx_t = (dbus_cntl.tx_t + 1) & MAX_DBUS_CHARS_MASK;
		dbus_cntl.charsout += 1;
		dbus_cntl.stop_out = FALSE;
	    }
	    return;

	case DB_LOGGER:
	    dbus_cntl.r.r_array[addr] = vip_logger_read();
	    break;

	case DB_MICROCODE:
	    dbus_cntl.r.r_array[addr] = DBUS_TERM_CHAR;	/* read_ucode(); */
	    break;

	case DB_DEBUG:
	    /*
	     * This release of software uses the crash mechanism to return
	     * context in response to the debus command 
	     */
	    dbus_cntl.r.r_array[addr] = vip_crash_read();
	    break;

        case DB_CRASH:
	    dbus_cntl.r.r_array[addr] = vip_crash_read();
	    break;

	default:
	    break;
        }
	*(volatile ulong *)(R4K_K1BASE + 0x24C) = 0x12121212;
	*(volatile ulong *)(R4K_K1BASE + 0x24C) = (ulong)dbus_cntl.r.r_array[addr];
	db_regs->data_port.ad.data_out = dbus_cntl.r.r_array[addr];
	db_regs->status = dbus_cntl.r.regs.diag_status.value;
	return;
    }

    /*
     * The RP/RSP did a write to our DBus registers
     */
    switch (addr) {

    case DB_CONTROL:
	dbus_control_write(&dp_copy, db_regs);
	break;

    case DB_MICROCODE:
	break;

    case DB_CONSOLE:
	c = dp_copy.dbus_data;
	dbus_cntl.rx_r[dbus_cntl.rx_h] = c;
	dbus_cntl.charsin += 1;
	dbus_cntl.rx_h = (dbus_cntl.rx_h + 1) & MAX_DBUS_CHARS_MASK;
	if (dbus_cntl.rx_h == dbus_cntl.rx_t) {
	    dbus_cntl.rx_t = (dbus_cntl.rx_t + 1) & MAX_DBUS_CHARS_MASK;
	}
	break;

    case DB_LOGGER:
    case DB_CRASH:
	/* 
	 * write cycles to these registers are unsupported
	 */
	db_regs->data_port.ad.data_out = DBUS_BAD_DATA;	
	break;

    case DB_PORT_STATUS:
	/*
	 * Update port status using bits specified by RSP
	 */
	dbus_cntl.r.regs.io_status =
	    (dbus_cntl.r.regs.io_status & ~DBUS_PORT_ACTIVE_MASK) |
		(dp_copy.dbus_data & DBUS_PORT_ACTIVE_MASK);

         /*
	  * If we just received a DB_PORT_STATUS command it is possible
          * that we were supressing, or need to suppress the fact that
          * there is output pending for the console or debug port
          * depending on the availability of data at these ports and
          * the status of these ports (connected/active or not).
          */
	if (console_io_pending() || debug_io_pending()) {
	    dbus_cntl.r.regs.diag_status.value |= DBUS_PORT_IO_AVAIL;
	} else {
	    dbus_cntl.r.regs.diag_status.value &= ~DBUS_PORT_IO_AVAIL;
	}
	break;

    case DB_DEBUG:
	/*
	 * This release of software uses a write to the DEBUG register as 
	 * a signal that the RSP wants context information on the interrupted
         * thread.  Use the crash mechanism to return the context.
	 */

	/*
	 * Turn SW_READY off so that the RSP knows we're cranking.
	 */
	dbus_cntl.r.regs.diag_status.value &= ~DBUS_SW_READY;
	dbus_cntl.dbus_reg->status = dbus_cntl.r.regs.diag_status.value;

	vip_crash_setup(vip_dbus_context);

	dbus_cntl.r.regs.diag_status.value |= DBUS_SW_READY;
	break;

    default:
	/*
	 * EEPROM write cycle
	 */
	break;
    }

    /*
     * Allow the DBus hw to ack the write cycle
     */
    db_regs->status = dbus_cntl.r.regs.diag_status.value;
}

/*
 * vip_physical_slot:
 * Return the physical slot into which the VIP is plugged in
 */
uint vip_physical_slot (void)
{
    dbus_hardware_regs *r;

    r = (dbus_hardware_regs *)(ADRSPC_DBUS);
    return(r->physical_slot & DBUS_PHYSICAL_SLOT_MASK);
}

/*
 * vip_logical_slot:
 * Return the logical slot into which the VIP is plugged in
 */
uint vip_logical_slot (void)
{
    dbus_hardware_regs *r;

    r = (dbus_hardware_regs *)(ADRSPC_DBUS);
    return(r->logical_slot & DBUS_LOGICAL_SLOT_MASK);
}

/*
 * vip_dbus_txfull:
 * Return whether the DBUS Tx ring is full or not
 */
boolean vip_dbus_txfull (void)
{
    leveltype level;
    boolean ret;
    volatile ulong new_head;

    GLOBAL_INTERRUPT_BLOCK(level);
    new_head = (dbus_cntl.tx_h + 1) & MAX_DBUS_CHARS_MASK;
    if (new_head == dbus_cntl.tx_t) {
	ret = TRUE;			/* it is full */
    } else {
	ret = FALSE;			/* there is more room */
    }
    GLOBAL_INTERRUPT_UNBLOCK(level);
    return(ret);
}

/*
 * vip_dbus_txhalf_full :
 * Return TRUE if the Tx ring is more than half full
 */
boolean vip_dbus_txhalf_full (void)
{
    leveltype level;
    int tot;

    GLOBAL_INTERRUPT_BLOCK(level);
    if (dbus_cntl.tx_h < dbus_cntl.tx_t) {
	tot = MAX_DBUS_CHARS - dbus_cntl.tx_t + dbus_cntl.tx_h;
    } else { 
	tot = dbus_cntl.tx_h - dbus_cntl.tx_t;
    }
    GLOBAL_INTERRUPT_UNBLOCK(level);
    return(tot > (MAX_DBUS_CHARS >> 1));
}

/*
 * vip_dbus_txflush:
 * Flush all pending IO from the DBUS Tx ring
 */
void vip_dbus_txflush (void)
{
    leveltype level;

    GLOBAL_INTERRUPT_BLOCK(level);
    dbus_cntl.tx_h = 0;
    dbus_cntl.tx_t = 0;
    dbus_cntl.flush_count += 1;
    GLOBAL_INTERRUPT_UNBLOCK(level);
}

/*
 * vip_console_read_buffer:
 * Transfer upto max characters from DBUS rx ring
 */
int vip_console_read_buffer (char *buf, int max)
{
    int i = 0;
    leveltype level;

    while (i<max) {
	GLOBAL_INTERRUPT_BLOCK(level);
	if (dbus_cntl.rx_t == dbus_cntl.rx_h) {
	    GLOBAL_INTERRUPT_UNBLOCK(level);
	    return(i);
	}
	buf[i] = dbus_cntl.rx_r[dbus_cntl.rx_t];
	dbus_cntl.rx_t = (dbus_cntl.rx_t + 1) & MAX_DBUS_CHARS_MASK;
	GLOBAL_INTERRUPT_UNBLOCK(level);
	i += 1;
    }
    return(i);
}

/*
 * show_vip_dbus :
 * Print out info for VIP dbus interface
 */
static void show_vip_dbus (parseinfo *csb)
{
    dbus_cntl_t *d;
    leveltype level;
    int i;
    tt_soc *tty;

    d = malloc(sizeof(dbus_cntl_t));
    if (!d) {
	return;
    }
    
    /*
     * Lock out interrupts and copy dbus_cntl
     */
    GLOBAL_INTERRUPT_BLOCK(level);
    bcopy(&dbus_cntl, d, sizeof(dbus_cntl_t));
    GLOBAL_INTERRUPT_UNBLOCK(level);

    /*
     * Print interesting fields
     */
    tty = d->tty;
    printf("\nDBUS Software Interface Information :\n");
    printf("\tdbus_cntl=%#x, dbus_regs=%#x, tty=%#x\n",
	   &dbus_cntl, d->dbus_reg, tty);
    printf("\tdiag_status=%#x, diag_control=%#x, io_status=%#x\n",
	   dbus_cntl.r.regs.diag_status.value, 
	   dbus_cntl.r.regs.diag_control.value,
	   dbus_cntl.r.regs.io_status);
    printf("\ttx_h=%d, tx_t=%d, rx_h=%d, rx_t=%d, lost_count=%d\n",
	   d->tx_h, d->tx_t, d->rx_h, d->rx_t, d->lost_count);
    printf("\trx_state=%d, charsin=%d, charsout=%d, flush_count=%d\n",
	   d->rx_state, d->charsin, d->charsout, d->flush_count);
    printf("\ttty->incount=%d, tty->outcount=%d, tty->statbits=%#x\n\n",
	   tty->incount, tty->outcount, tty->statbits);

    /*
     * Free the copy
     */
    free(d);

    /*
     * Print out the motherboard EEPROM contents
     */
    printf("\tEEPROM Contents (hex):\n");
    printf("\t  0x00:");
    for (i=0; i<0x10; i++) {
	printf(" %02x", dbus_cntl.r.r_array[0x20+i]);
    }
    printf("\n\t  0x10:");
    for (i=0; i<0x10; i++) {
	printf(" %02x", dbus_cntl.r.r_array[0x30+i]);
    }
    printf("\n");
}

/*
 * vip_init_dbus_parser :
 * Install the DBUS parser chains
 */
void vip_init_dbus_parser (void)
{
    /* 
     * Hook in parse chains
     */
    parser_add_command_list(vip_dbus_chain_init_table,
			    "software dbus commands");
}
    
/* end of file */
