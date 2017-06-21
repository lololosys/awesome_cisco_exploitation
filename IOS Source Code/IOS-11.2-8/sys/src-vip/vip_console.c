/* $Id: vip_console.c,v 3.3.28.3 1996/06/10 04:52:54 dlobete Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/vip_console.c,v $
 *------------------------------------------------------------------
 * vip_console.c : VIP and Checkers Console support
 *
 * January 1995, Michael Beesley 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: vip_console.c,v $
 * Revision 3.3.28.3  1996/06/10  04:52:54  dlobete
 * CSCdi45148:  Error messages on VIP not reported on RP/RSP
 * Branch: California_branch
 *
 * Revision 3.3.28.2  1996/03/21  23:52:20  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.3.28.1  1996/03/18  22:03:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.2  1996/02/10  00:22:28  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1996/02/08  09:31:15  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3  1996/01/22  07:16:06  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  18:51:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:37  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files 
 */
#include "master.h"
#include <ciscolib.h>
#include "packet.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "ttysrv.h"
#include "stacks.h"
#include "logger.h"

#include "../os/async.h"

#include "../pas/if_pas.h"
#include "../hes/dgbus.h"
#include "../src-vip/vip_dbus.h"
#include "../src-vip/vip_memd.h"

#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ipc/ipc_seats.h"
#include "../ipc/ipc_cbus_vip.h"
#include "../ipc/ipc_slave_apps.h"

/*
 * Local storage
 */
static vectortype 	console_vector;
static sys_timestamp 	con_time;

static ipc_port_id	vip_logport;
static char		*vip_logport_ptr;
static boolean          vip_con_ok;

/*
 * Literals
 */
#define VIP_CONSOLE_FLUSH_TIME	5000	/* 5 secs flush of Tx ring */
#define CONSOLE_TX_DELAY	100	/* 100ms delay between each polled
					   Tx character */
#define IPC_LOG_MAX_MSG_SIZE	1500

/*
 * Any logger message that has a level below this level 
 * will be logged to our IPC master for logging either to the 
 * master console or to a syslog server.
 */
#define IPC_LOG_MSG_LEVEL	LOG_ERR

/*
 * vip_tx_disable : 
 * Disable the console DBUS character ready state
 */
static void vip_tx_disable (tt_soc *tty)
{
    if (tty->ttynum != 0) {
	return;			/* only support the console */
    }
}

/*
 * vip_portdisable :
 * Disable the receive side of the console
 */
static void vip_portdisable (tt_soc *tty)
{
    if (tty->ttynum != 0) {
	return;			/* only support the console */
    }
    dbus_cntl.rx_state = FALSE;	/* disable reception of characters */
}

/*
 * vip_portenable :
 * Enable receive and transmit on the DBUS console
 */
static void vip_portenable (tt_soc *tty)
{
    if (tty->ttynum != 0) {
	return;			/* only support the console */
    }
    dbus_cntl.rx_state = TRUE;	/* enable reception of characters */
}

/*
 * vip_startoutput :
 * Start output of characters to the RP across the DBUS console
 */
static boolean vip_startoutput (tt_soc *tty)
{
    if (tty->ttynum != 0) {
	return(FALSE);	     	/* only support the console */
    }

    /*
     * Signal helper process.
     */
    if (tty_signal_helper(tty))
	return(TRUE);

    if (tty->statbits & (HOLDING | WANTXON))
	return(TRUE);		/* Do nothing if output stopped */
    
    vip_portenable(tty);
    return(TRUE);
}

/*
 * vip_startinput :
 * Reset any stopped input.
 */
static void vip_startinput (tt_soc *tty)
{
    if (tty->ttynum != 0)
	return;			/* only support console */
    dbus_cntl.rx_state = TRUE;	/* enable reception of characters */
}

/*
 * console_putc :
 * Write a byte to the console, using polled IO
 */
void console_putc (uchar ch) 
{
    int i;

    if (ch == '\n')
	console_putc('\r');

    if (kernel_debugging) {
	while (vip_dbus_txhalf_full())
	    ;
    } else if (systeminit_flag) {
	for (i=0; i<CONSOLE_TX_DELAY; i++) {
	    if (!vip_dbus_txhalf_full()) {
		break;
	    }
	    DELAY(1);
	}
    }
    vip_console_write_char(ch);
}

/*
 * console_getc : 
 * Get a character from the DBUS thru polling
 */
int console_getc (void)
{
    leveltype level;
    int c;

    /*
     * Wait until the Tx side has drained
     */
    while (!vip_dbus_txempty())
	;

    /*
     * Wait for a character from the dbus
     */
    while (TRUE) {
	GLOBAL_INTERRUPT_BLOCK(level);
	if (dbus_cntl.rx_h != dbus_cntl.rx_t) {
	    c = dbus_cntl.rx_r[dbus_cntl.rx_t];
	    dbus_cntl.rx_t = (dbus_cntl.rx_t + 1) & MAX_DBUS_CHARS_MASK;
	    GLOBAL_INTERRUPT_UNBLOCK(level);
	    return(c);
	}
	GLOBAL_INTERRUPT_UNBLOCK(level);
    }
}

/*
 * console_init :
 * Setup the system console.
 */
tt_soc *console_init (void)
{
    vectortype *vector;
    tt_soc *tty;
    leveltype status;

    /*
     * Initialize the vector pointers
     */
    status = raise_interrupt_level(LEVEL_CONSOLE);
    vector = &console_vector;
    vector_init(vector);

    /*
     * Initialize VIP specific vectors
     */
    vector->vgetc = serial_getc;
    vector->vclearinput = serial_clearinput;
    vector->vinputpending = serial_inputpending;
    vector->vputc = serial_putc;
    vector->vstartoutput = vip_startoutput;
    vector->vstartinput = vip_startinput;
    vector->vclearoutput = serial_clearoutput;
    vector->voutputpending = serial_outputpending;
    vector->voutputblock = serial_outputblock;
    vector->voutputxoffed = serial_outputxoffed;
    vector->vputpkt = generic_putpkt;
    vector->vstopoutput = vip_tx_disable;
    vector->vportdisable = vip_portdisable;
    vector->vportenable = vip_portenable;
    vector->vservicehook = helper_servicehook;
    vector->vnoservicehook = helper_noservicehook;

    /*
     * Set up the TTY data structure
     */
    tty = (tt_soc *) tty_init(0,READY,CTY_LINE,vector);
    if (!tty)
	return(NULL);
    serial_setbuffers(tty, MAXINCHARS, MAXOUTCHARS);
    tty->txfinish = vip_tx_disable;
    tty->modem_bits &= ~(255);		/* Clear saved modem state. */
    tty->modem_bits |= MODEMBIT_DTR;	/* Update saved modem state. */
    tty->tty_exectimeout = 0;		/* Never timeout */

    /*
     * Set dbus_cntl.tty to tell the DBUS handler that we are all set
     * and install required variables
     */
    dbus_cntl.tty = tty;

    /*
     * Restore interrupt level and return tty pointer
     */
    reset_interrupt_level(status);
    return(tty);
}

/*
 * vip_dbusBLOCK :
 * Wait for input on the dbus rx ring
 */
static int vip_dbusBLOCK (void)
{
    if (ELAPSED_TIME(con_time) > VIP_CONSOLE_FLUSH_TIME) {
	return(0);					/* run process */
    }
    if (dbus_cntl.rx_h != dbus_cntl.rx_t) {
	return(0);					/* run process */
    }
    if (dbus_cntl.tty->outcount != 0) {
	return(0);					/* run process */
    }
    return(1);						/* block */
}

/*
 * vip_console_process : 
 * VIPER Console process. Move bytes from dbus_cntl to/from tty
 */
forktype vip_console_process (tt_soc *tty)
{
    char ttybuffer[MAX_DBUS_CHARS], c;
    uchar *cp;
    int bytes, i;
    leveltype SR;
    volatile ulong old_txt;

    GET_TIMESTAMP(con_time);
    vip_con_ok = TRUE;

    while (TRUE) {

	/*
	 * Wait for some traffic
	 */
	edisms((blockproc *)vip_dbusBLOCK, 0);

	if (ELAPSED_TIME(con_time) > VIP_CONSOLE_FLUSH_TIME) {

	    GET_TIMESTAMP(con_time);
	    if (!vip_dbus_txempty() && old_txt == dbus_cntl.tx_t) {
		/*
		 * Tx ring is non-empty and the tail has not moved.
		 * Flush the Tx ring as the RP is not listening.
		 */
		vip_dbus_txflush();
	    }
	    old_txt = dbus_cntl.tx_t;
	}
	
	/*
	 * Handle moving characters from DBUS to tty structure
	 */
	bytes = vip_console_read_buffer(ttybuffer, 32);
	if (!dbus_cntl.rx_state) {
	    bytes = 0;
	}

	for (i=0; i<bytes; i++) {
	    c = ttybuffer[i];
	    if (special_chars(tty, c) >= 0) {
		
		/*
		 * Handle overflow
		 */
		SR = raise_interrupt_level(ALL_DISABLE);
		if (tty->incount > (tty->insize >> 1)) {
		    if ((tty->intail == tty->inhead)) {
			tty->overflow++;
			if (modem_debug)
			    ttybug(tty,"Input buffer overflow");
		    }
		    reset_interrupt_level(SR);
		    break;
		}

		/*
		 * Handle buffer wrap
		 */
		if (tty->intail >= tty->inend)
		    tty->intail = tty->instart;
		
		/*
		 * Add character to end of buffer
		 */
		*tty->intail++ = c;
		++tty->incount;
		reset_interrupt_level(SR);
	    }
	}
	
	/*
	 * Handle moving bytes from tty to DBUS
	 */
	while (!vip_dbus_txfull()) {
	    if (tty->outcount <= 0) {
		break;
	    }
	    SR = raise_interrupt_level(ALL_DISABLE);
	    cp = ++tty->outhead;
	    if (cp == tty->outend) {
		cp = tty->outstart;
		tty->outhead = cp;
	    }
	    vip_console_write_char(*cp);
	    tty->outcount--;
	    tty->totalout++;
	    reset_interrupt_level(SR);
	}
    }
}

/*
 * vip_logcallback :
 * Callback to clean up message resource
 */
static void vip_logcallback (ipc_message *msg, void *ctx, 
			     ipc_error_code err)
{
    if (msg)
	ipc_return_message(msg);
}

/*
 * vip_logsend :
 * Send a logger message across the IPC to our master
 */
static void vip_logsend (char *buffer, uint buflen, char *msgp, 
			 uint msglen, int msglvl)
{
    ipc_message *msg;

    /*
     * If the level of this message is above our logging
     * level, or the console is not up, then simply return
     */
    if ((msglvl >= IPC_LOG_MSG_LEVEL) || !vip_con_ok) {
	return;
    }

    if (msglen > IPC_LOG_MAX_MSG_SIZE)
	return;

    /*
     * If the IPC port is 0, then if has not been opened. Try and
     * open it.
     */
    if (!vip_logport) {
	if (!vip_logport_ptr) {
	    vip_logport_ptr = generate_slavelog_port_name(vip_logical_slot());
	    if (!vip_logport_ptr)
		return;
	}
	vip_logport = ipc_locate_port(vip_logport_ptr);
    }
    if (!vip_logport) {
	return;
    }

    /*
     * Get a message, fill it up and send it
     */
    msg = ipc_get_message(msglen+1, vip_logport, 0);
    if (!msg) {
	return;
    }
    bcopy(msgp, msg->data, msglen);
    ((char *)(msg->data))[msglen] = '\0';
    ipc_send_message_async(msg, NULL, vip_logcallback);
}

/*
 * vip_init_ipc_logging :
 * Initialize the logging machanisms across the cbus
 */
static void vip_init_ipc_logging (void)
{
    /*
     * Mark the IPC port as not opened and register the logger callback
     */
    vip_logport = 0;
    reg_add_log_send(vip_logsend, "vip_logsend");
}

/*
 * platform_line_init :
 * Initializes the terminal line data structures
 */
void platform_line_init (void)
{
    /*
     * Spawn the console process
     */
    cfork((forkproc *)vip_console_process, (long)dbus_cntl.tty, 0, 
	  "DBUS Console", CONSOLE_LINENUM);

    /*
     * We only support the console. Maybe later the Debug
     * port could be supported as an async line.
     */
    nconlines = 1;
    nauxlines = 0;

    /*
     * Assign base indexes into the MODEMS[] data structure.
     */
    AuxBase = freeLineBase;
    VTYBase = freeLineBase;
    ALLlines = nconlines;

    /*
     * Initialize VIP slave logging thru the IPC mechanism
     */
    vip_init_ipc_logging();
}

/*
 * console_check_break :
 * On VIP, we do not take breaks, so return 0 all the time
 */
int console_check_break (void)
{
    return(0);
}

/* end of file */
