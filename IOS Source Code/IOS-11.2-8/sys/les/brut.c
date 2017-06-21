/* $Id: brut.c,v 3.4.12.1 1996/03/18 20:40:16 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/les/brut.c,v $
 *------------------------------------------------------------------
 * brut2681.c  --  Signetics 2681 DUART driver for brut
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: brut.c,v $
 * Revision 3.4.12.1  1996/03/18  20:40:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.3  1996/03/07  10:04:56  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.2  1996/02/20  01:07:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/13  20:42:24  hampton
 * Remove platform specific dependencies from common tty header files.
 * [CSCdi48844]
 *
 * Revision 3.3  1996/01/25  11:19:43  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.2.24.1  1996/01/24  22:21:57  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  17:36:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/23  17:42:55  jquan
 * CSCdi40522:  Modularity: sub_c3000 should not require sub_brut
 *
 * Revision 2.4  1995/06/23  18:48:11  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.3  1995/06/19  06:46:52  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  23:39:50  achopra
 * CSCdi04836:  Provide ip-on-break functionality on TS
 *
 * Revision 2.1  1995/06/07  21:30:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 * BRUT interrupt service function for the Aux port.
 */
#include "master.h"
#include <ciscolib.h>
#include "les_console.h"
#include "sys_registry.h"
#include "ttysrv.h"
#include "subsys.h"
#include "brut_debug.h"
#include "../ts/s2681.h"
#include "../os/async.h"
#include "if_les.h"
#include "brut.h"
#include "platform_registry.h"
#include "../les/platform_registry.regc"


/*
 * External declarations
 */
extern char pan_uartinterrupt;
extern vectortype console_vector;

static int trace_head = 0;
static int trace_tail = 0;
static int trace_table_full = FALSE;
static RXMSGENTRY      brut_as_slave_resp;
static BRUTGLOBENTRY   brutglobentry;

static trace_table *trace_tbl;

static RESPMSGSTRUCTW2 slave_resp_msg =
{
    RESP_START_MSG,
    '0',			/* to be filled in at init */
    CMD_CD_WHO,
    BRUT_ID,			/* 'T' */
    {'0', '0', '0', '0', '0', '0',
     '0', '0', '0', '0', '0', '0'},	/* to be filled in at init */
    END_MSG1,
    END_MSG2
};

static void transmit_hub_message(tt_soc *, TXMSGENTRY *);
static void brut_init(void);
static void get_brut_params(unsigned char *, unsigned char *);
static void brut_print_msg(void *);
static void brut_subsys_init(subsystype *);

/*
 * Brut subsystem header
 */

#define BRUT_MAJVERSION   1
#define BRUT_MINVERSION   0
#define BRUT_EDITVERSION  1


SUBSYS_HEADER(brut, BRUT_MAJVERSION, BRUT_MINVERSION, BRUT_EDITVERSION,
	      brut_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);

static void trace_brut (uchar code, uchar isr, ushort status, int c)
{
    if (!trace_table_full) {
	trace_tbl[trace_tail].code = code;
	trace_tbl[trace_tail].isr = isr;
	trace_tbl[trace_tail].status = status;
	trace_tbl[trace_tail].c = c;
	trace_tail++;
	if (trace_tail == 2000) 
	    trace_tail = 0;
	if (trace_tail == trace_head)
	    trace_table_full = TRUE;
    }
}


static void brut2681_common (tt_soc *tty)
{
    register ushort status;
    register int    c;
    uartaddress    *uart;
    unsigned char   isr;
    volatile uchar *data_ptr, *status_ptr;

    data_ptr = tty->uartdata;
    status_ptr = tty->uartstatus;
    status = *status_ptr;
    uart = WHICHUART (tty->ttynum);

    /* brut break interrupt handling - s.p. */
    /* only worry about this on the aux port */
    isr = uart->interrupt;

    if (status & (OVRRNERROR+FRMERROR)) {
	tty->overrun++;
	uart->commandB = RESETERROR;
	status = *status_ptr;
	if (brut_trace_debug)
	    trace_brut (3,isr,status,48);
    }
    /*
     * Disable RX and TX interrupt to avoid nested interrupts
     */
    pan_uartinterrupt &= ~(RXRDYB | TXRDYB);
    uart->interrupt = pan_uartinterrupt;
    while (status & RXREADY) {
	c = *data_ptr;
	if (brut_trace_debug)
	    trace_brut (4,isr,status,c);
	tty->totalin++;

	if (tty->intail >= tty->inend)
	    tty->intail = tty->instart;	/* Handle wraps */
	*tty->intail++ = c;
	++tty->incount;
	/* look for {u?<cr>, where u=my_slot */
	if (brut_driver_state != BRUT_DRIVER_STATE_RESPONDING) {
	    if ((uchar) c == REQ_START_MSG)
		brut_driver_state = BRUT_DRIVER_STATE_RX_START;
	    else if (((uchar) c == (brut_slot + '0')) &&
		     (brut_driver_state == BRUT_DRIVER_STATE_RX_START))
		brut_driver_state = BRUT_DRIVER_STATE_RX_MY_SLOT;
	    else if (((uchar) c == CMD_CD_WHO) &&
		     (brut_driver_state == BRUT_DRIVER_STATE_RX_MY_SLOT))
		brut_driver_state = BRUT_DRIVER_STATE_RX_WHO_ARE_YOU;
	    else if (((uchar) c == END_MSG1) &&
		     (brut_driver_state == BRUT_DRIVER_STATE_RX_WHO_ARE_YOU)) {
		brut_driver_state = BRUT_DRIVER_STATE_RESPONDING;
		transmit_hub_message(tty, (TXMSGENTRY *) & brut_as_slave_resp);
		/*startoutput (tty);*/
	    } else
		brut_driver_state = BRUT_DRIVER_STATE_NORMAL;
	}
	status = *status_ptr;
    }

    while (status & TXREADY) {

	if (tty->outcount) {
	    register uchar *cp = ++tty->outhead;
	    if (cp == tty->outend) {
		cp = tty->outstart;
		tty->outhead = cp;
	    }
	    if (brut_trace_debug)
		trace_brut (5,isr,status,*cp);
	    tty->outcount--;
	    *data_ptr = *cp;
	    tty->totalout++;
	} else {
#ifdef HUB_ARB_DEBUG	  
	    trace_brut (6,isr,status,48);
#endif HUB_ARB_DEBUG
	    (*(tty->txfinish)) (tty);
	    /*
	     * resync head and tail to avoid mixing up characters
	     */
	    tty->outtail = tty->outhead;
	    if (brut_driver_state == BRUT_DRIVER_STATE_RESPONDING)
		brut_driver_state = BRUT_DRIVER_STATE_NORMAL;
	    break;
	}
	status = *status_ptr;
    }
    /*
     * Re-enable interrupts for next time
     */
    pan_uartinterrupt |= RXRDYB;
    if (tty->outcount)
	pan_uartinterrupt |= TXRDYB;	
    uart->interrupt = pan_uartinterrupt;
}


/*
 * brut_select_hub ()
 * This function sets the hub to which the brut box will be talking to.
 * Currently, the brut box can only be slave.
 * A zero (0) return code is returned if there was no error. If an
 * error occured, a -1 is returned.
 */
static int brut_select_hub (uchar hub_number)
{
    int             sts = 0;
/* the hub select bit is active low */
    if (hub_number == 0)
	/* set the bit to talk to hub 1, the uart register */
	/* bit is set by writing the bit to the aux3 address */
	uartaddr->setport = BRUT_HUB_SELECT_MASK;
    else if (hub_number == 1)
	/* clear the bit to talk to hub 2, the uart register */
	/* bit is cleared by writing the bit to the aux4 address */
	uartaddr->resetport = BRUT_HUB_SELECT_MASK;
    else			/* illegal hub number passed in */
	sts = -1;

    return (sts);
}


static void transmit_hub_message (tt_soc *tty,
				  TXMSGENTRY *message)
{
    int             i;
    uchar          *p;
    if ( brut_debug_transmit ) {
	ttyprintf(CONTTY,"\n Tx msg : ");
      brut_print_msg ((void *) message);
    }
    p = (uchar *) & message->RQ_start;
    /* 
     * Brut is always in listen mode
     */
    brut_select_hub (1);
    for (i = 0; i < sizeof (BRUTMSGSTRUCT); ++i, p++) {
	quick_putc (tty, (int) *p);
	if (*p == END_MSG1)
	    break;
    }
}


/*
 * bruttty_init
 * Initialize the terminal data structure
 */

static tt_soc *bruttty_init (int n, ulong resetstatus, int ttytype, 
			     vectortype *vector)
{
    register tt_soc *tty;

    tty = malloc(sizeof(tt_soc));
    MODEMS[n] = tty;
    if (tty == NULL) {
	system_running = FALSE;
	crashdump(0);
	return((tt_soc *)NULL);
    }
    tty->ttynum = n;
    tty->type = ttytype;
    tty->ivector = tty->ovector = tty->nvector = vector;
    tty->statbits = resetstatus;
    tty->def_capabilities = tty->capabilities = 0L;
    tty->def_capabilities2 = tty->capabilities2 = 0L;
    brut_init();
    return(tty);
}

/*
 * brutauxline_init
 * Initialize port B of console UART to work on DEC Hub 90.
 */

static void brutauxline_init (void)
{
    register tt_soc *tty;
    leveltype SR;

    SR = raise_interrupt_level(TTY_DISABLE);

    bruttty = (tt_soc *) bruttty_init(AuxBase,READY|NOBANNER,AUX_LINE,
				      &console_vector);
    tty = bruttty;
    if (!tty)
      return;

    reg_add_brut2681_common(brut2681_common, "brut2681_common");

    serial_setbuffers(tty,MAXINCHARS,MAXOUTCHARS); /* set up buffering */ 

    tty->uartdata = &uartaddr->dataB;
    tty->uartstatus = &uartaddr->statusB;
    tty->txfinish = pan_tx_disable;

    tty->tty_databits = 8;
    tty->tty_parity = PARITY_NO;
    tty->tty_stopbits = STOPBITS2;
    tty->modem_type = MODEMTYPE_LOCAL; /* No modem control initially */

    uartaddr->commandB = RESETMRPTR;
    uartaddr->modeB = (RXRDYINTR | CHARERROR | PRTYOFF | DBITS8);
    uartaddr->modeB = MODENORMAL | STOP16X16;

    tty->tty_rxspeed = tty->tty_txspeed = 0;
    setspeed(tty, ASYNC_DEFSPEED, ASYNC_DEFSPEED);

    /*
     * Enable interrupts:
     */
    uartaddr->commandB = TXDISABLE + STP_RXENABLE;
    pan_uartinterrupt |= (RXRDYB | TXRDYB);
    uartaddr->interrupt = pan_uartinterrupt;
    /* Turn on "Port 0 OK" LED now because that's what 9.14 did */
    uartaddr->setport = BRUT_PORT_0_OK;

    reset_interrupt_level(SR);
}

/*
 * brut_init
 * Initialize world
 */

static void brut_init (void)
{
    brut_debug_transmit = 0;
    brut_debug_receive = 0;
    brut_trace_debug = FALSE;

    brut_debug_init();
    /*
     * brut_slot - physical hub slot number to which BRUT is plugged into
     * brut_mac - 6 byte mac address of BRUT
     */
    get_brut_params((uchar *) &brut_slot, (uchar *) brut_mac);
    /*
     * Initially set to listen mode
     */
    brut_select_hub (1);

    /* construct who are you response */
    slave_resp_msg.SlotNumber = brut_slot + '0';
    sprintf ((char *) slave_resp_msg.stationAdd,
	     "%02x%02x%02x%02x%02x%02x", brut_mac[0], brut_mac[1],
	     brut_mac[2], brut_mac[3], brut_mac[4], brut_mac[5]);
    slave_resp_msg.endmsg1 = END_MSG1;
    bcopy(&slave_resp_msg, &(brut_as_slave_resp.resp_msg.RespMsgStructW2),
	  sizeof (RESPMSGSTRUCTW2));

    brut_driver_state = BRUT_DRIVER_STATE_NORMAL;
    /*
     * Initialize BRUT parser support
     */
    brut_parser_init();
}

/*
 * little function to return the hub slot number, the hardware revision
 * number, and the Ethernet address for the BRUT box.
 * Returns 0 if no error encountered.
 * Returns -1 if any error encountered, such as not a BRUT box.
 */
static void get_brut_params (unsigned char *m_slot, unsigned char *m_mac)
{
    int             i;
    unsigned char  *tmp_ptr;
    
    tmp_ptr = (unsigned char *) ADRSPC_BRUT_ENET;
    *m_slot = ((*(short *) ADRSPC_CONTROL3) & 0xF000) >> 12;
    for (i = 0; i < IEEEBYTES; i++)
	*m_mac++ = *tmp_ptr++;
}

static void brut_print_msg (void *msg)
{
    uchar          *p;
    register int    i;
    RXMSGENTRY     *msg_rx;
    msg_rx = (RXMSGENTRY *) msg;
    p = (uchar *) & msg_rx->RS_start;
    ttyprintf(CONTTY," hub = %d ; ", msg_rx->hubid);
    for (i = 0; i < 20; i++) {
	if (*p == END_MSG2) {
	    ttyprintf(CONTTY," 0x%x", *p++);
	    break;
	} else if (*p == END_MSG1) {
	    ttyprintf(CONTTY," 0x%x", *p++);
	    break;
	} else
	    ttyprintf(CONTTY," %c", *p++);
    }
}

/*
 * Brut subsystem initialization entry point
 */

static void brut_subsys_init (subsystype *subsys)
{
    /*
     * Initialize the aux port properly if box is a BRUT
     */
    if (IS_BRUT(cookie)) {
        platform_regcode = registry_create(REG_UNKNOWN,
					   SERVICE_PLATFORM_MAX,
					   "Platform");
	if (platform_regcode != REG_ILLEGAL)
           create_registry_platform();

        reg_add_line_init(brutauxline_init, "brutauxline_init");
        trace_tbl = malloc(sizeof(trace_table) * TRACE_TABLE_MAX);
        if (!trace_tbl) {
           crashdump(0);
        }
    }
}
