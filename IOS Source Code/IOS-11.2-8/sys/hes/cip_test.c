/* $Id: cip_test.c,v 3.3.60.3 1996/08/07 17:59:25 kmitchel Exp $
 * $Source: /release/112/cvs/Xsys/hes/cip_test.c,v $
 *------------------------------------------------------------------
 * CIP specific functions for 'test cbus' facility
 *
 * July 1994, Bill Palmer & Stig Thormodsrud
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cip_test.c,v $
 * Revision 3.3.60.3  1996/08/07  17:59:25  kmitchel
 * CSCdi49854:  DBUSINTERR after RSP dbus write; ineffective recvry of
 * disabled card
 * Branch: California_branch
 *
 * Revision 3.3.60.2  1996/07/15  02:15:31  bchan
 * CSCdi50608:  makefile for mib incorrect
 * Branch: California_branch
 *
 * Revision 3.3.60.1  1996/03/18  19:39:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  09:38:37  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  14:18:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  09:15:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:25:53  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:37:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:59:39  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/06/18  23:16:34  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:04:22  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:38:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "interface_private.h"
#include "ttysrv.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/if_hes_common.h"
#include "../hes/if_cip.h"
#include "../hes/cbus_test.h"
#include "dgbus.h"
#include "dbus.h"
#include "dbus_io.h"
#include "ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "slot.h"
#include "ctrlr_cmds.h"


/* We need to handle high addresses like 0xa4000000 and 0xbfc00000 */
#define MAX_CHANNEL_PROCESSOR_MEMORY (0xffffffff)

typedef enum dump_state_ {
  DUMP_STATE_NEVER, DUMP_STATE_STARTED, DUMP_STATE_DONE
  } dump_state;

/*
 * get_dbus_char
 * worker routine to grab a character and eliminate spurious 0xFF's
 * caused by kludgy dbus interface on channel card
 */ 
static char get_dbus_char (volatile uchar *ptr, boolean *fatal)
{
    int patience;
    uchar result;
    
    patience = 100;
    while (((result = *ptr) == 0xff) && --patience) 
	;	
    if (patience)
	*fatal = FALSE;
    else
	*fatal = TRUE;
    return(result);
}


/*
 * logger_drain
 * display any logger port messages that may be
 * pending
 */
void logger_drain (int slot)
{
    dgtype *dbus;
    uchar buffer[DBUS_IO_MAXLEN];
    uchar *p;
    int severity;
    uchar c;
    int count = 50;
    boolean fatal_err = FALSE;

    dbus = pslot_to_dbus(slot); 

    if (!touch_device(dbus))
	return;

    if (!slots[slot].io_regs) {
	return;
    }

    while ((dbus->io_status & DBUS_IO_MSG) && !fatal_err) {
	count = 10;
	severity = get_dbus_char(&dbus->io_msg, &fatal_err);
	p = buffer;
	memset(buffer, 0, DBUS_IO_MAXLEN);
	if (severity != 0) {
	    if ((c = get_dbus_char(&dbus->io_msg, &fatal_err)) !=
		DBUS_IO_BUGINF)
		*p++ = c;
	cont:
	    do {
		*p++ = c = get_dbus_char(&dbus->io_msg, &fatal_err);
		DELAY(1);
	    } while ((c != DBUS_IO_EMPTY) && (p < &buffer[DBUS_IO_MAXLEN]) &&
		     !fatal_err);
	    *(p - 1) = 0; 

	    if (!fatal_err)
		buginf("\nCIP: %s", buffer);
	    else
		buginf("\nCIP (fatal_err): %s", buffer);
	    if (c != DBUS_IO_EMPTY && !fatal_err) {	/* buffer overflow */
		p = buffer;
		memset(buffer, 0, DBUS_IO_MAXLEN);
		if (count-- > 0)
		    goto cont;
	    }
	} else {
	    buginf("\nCIP reported severity of 0, ignoring");
	    return;
	}
    }
    if (dbus->io_status & DBUS_IO_MSG)
	buginf("\n--More messages pending--");
}


void console_drain (int slot)
{
    dgtype *dbus;
    uchar buffer[DBUS_IO_MAXLEN];
    uchar *p;
    int severity;
    uchar c;
    int count = 50;
    boolean fatal_err = FALSE;

    dbus = pslot_to_dbus(slot); 
    if (!touch_device(dbus))
	return;

    if (!slots[slot].io_regs) {
	return;
    }

    while ((dbus->io_status & DBUS_IO_CONSOLE) && !fatal_err) {
	count = 10;
	severity = get_dbus_char(&dbus->io_console, &fatal_err);
	p = buffer;
	memset(buffer, 0, DBUS_IO_MAXLEN);
	if (severity != 0) {
	    if ((c = get_dbus_char(&dbus->io_console, &fatal_err)) !=
		DBUS_IO_BUGINF)
		*p++ = c;
	cont:
	    do {
		*p++ = c = get_dbus_char(&dbus->io_console, &fatal_err);
	    } while (c != DBUS_IO_EMPTY && p < &buffer[DBUS_IO_MAXLEN] &&
		     !fatal_err);
	    *(p - 1) = 0; 
	    if (c != DBUS_IO_EMPTY && !fatal_err) {	/* buffer overflow */
		buginf("\nCIP: %s", buffer);
		p = buffer;
		memset(buffer, 0, DBUS_IO_MAXLEN);
		if (count-- > 0)
		    goto cont;
	    }
	    buginf("\nCIP: %s", buffer);
	} else {
	    buginf("\nCIP reported severity of 0, ignoring");
	    return;
	}
    }
    if (dbus->io_status & DBUS_IO_CONSOLE)
	buginf("\n--More messages pending--");
}

#if 0

/*
 * save_dbus_control_reg
 * Get the value of the dbus control reg
 */
static boolean save_dbus_control_reg (int slot, uchar *control)
{
    dgtype* dbus;

    dbus = pslot_to_dbus(slot);
    if (!touch_device(dbus))
	return (FALSE);
    *control = dbus_prot_rd(&dbus->diag_control);
    return (TRUE);
}

/*
 * set_dbus_control_reg
 * Set the value of the dbus control reg
 */
static void reset_dbus_control_reg (int slot, uchar control)
{
    dgtype* dbus;

    dbus = pslot_to_dbus(slot); 
    dbus_prot_wr(slot, &dbus->diag_control, control);  /* this will reset the CIP */
}
 
/*
 * channel_dump_memory
 * Dump out a range of locations in processor RAM
 */

static void channel_dump_memory (int slot, ulong *lo, ulong *hi)
{
    dgtype* dbus;
    leveltype level;
    ulong linect, len, count;
    ulong addr, i, *l_ptr;
    uchar *buf, *buf_ptr;
    uchar *ch_ptr, c;
    ushort crc;
    ulong low = *lo;
    ulong high = *hi;
    

    low = (ulong)low & ~1;
    if (high < low) {
	printf ("\nHigh address is lower than low address");
	return;
    }
    len = high - low;
    if (len & 0x0f)
      len = (len & ~0x0f) + 0x10;
    buf = malloc(len+16); 
    if (!buf) {
	printf("\nInsufficient memory, try a smaller block");
	return;
    }

    dbus = pslot_to_dbus(slot); 
    if (!touch_device(dbus))
	return;
    /* "halt" microcode and put in cmd mode */
    dbus_prot_wr(slot, &dbus->diag_control, WCS_DBUS_CMD_ENABLE);

    level = raise_interrupt_level(NETS_DISABLE);
    dbus->ucode_command_and_data = WCS_CMD_REG;
    dbus->ucode_command_and_data = WCS_CMD_DOWNLOAD_IMAGE;
    dbus->ucode_command_and_data = (low >> 24) & 0xff;
    dbus->ucode_command_and_data = (low >> 16) & 0xff;
    dbus->ucode_command_and_data = (low >>  8) & 0xff;
    dbus->ucode_command_and_data = (low >>  0) & 0xff;
    
    dbus->ucode_command_and_data = (len >> 24) & 0xff;
    dbus->ucode_command_and_data = (len >> 16) & 0xff;
    dbus->ucode_command_and_data = (len >>  8) & 0xff;
    dbus->ucode_command_and_data = (len >>  0) & 0xff;

    dbus->ucode_command_and_data = 0;
    dbus->ucode_command_and_data = 0;
    dbus->ucode_command_and_data = 0;
    dbus->ucode_command_and_data = 0;
    
    dbus->ucode_command_and_data = WCS_DIRECTION_IP_TO_RP;
    dbus->ucode_command_and_data = WCS_IMAGE_COMPRESSION_NONE;
    dbus->ucode_command_and_data = 0;
    dbus->ucode_command_and_data = 0;
    dbus->ucode_command_and_data = 0;
    dbus->ucode_command_and_data = 0;

    /* Read status register to verify CIP is okay with this request */
    if (dbus_prot_rd(&dbus->diag_status) & DBUS_INVALID_REQUEST) {
	printf ("\nInvalid memory address");
	reset_interrupt_level(level);
	free(buf);
	return;
    }

    /* Update addresses for next request */
    *lo = low + len;
    *hi = *lo + len;
    
    buf_ptr = buf;
    for (i = 0; i < len; i++)  
      *buf_ptr++ = dbus->ucode_command_and_data;

    crc = dbus->ucode_command_and_data;
    crc <<= 8;
    crc |= dbus->ucode_command_and_data;
    reset_interrupt_level(level);
    process_sleep_for(ONESEC/10);

    /*
     * now print it
     */
   
    linect = 0;
    count = 0;
    addr = low;
    l_ptr = (ulong *)buf;
    while (count < len && !automore_quit()) {
	printf("\n%-06x: ", addr);	    
	ch_ptr = (uchar *)l_ptr;
	for (i = 0; i < 4; i++) 
	    printf("%-08x ", *l_ptr++);
	for (i = 0; i < 16; i++) {
	    c = *ch_ptr++ & 0x7f;
	    if ((c < ' ') || (c == 0x7f))
		c = '.';
	    printf("%c", c);
	}
	addr += 16;
	count += 16;
	if (linect++ > 100) {
	    process_sleep_for(ONESEC/10); 
	    linect = 0;
	}
    }
    free(buf);

}


/*
 * cip_test_ctrlr_cmd - send the CIP a CTRLR_CMD and arg
 *                      repeat times
 */
#define EXECUTE_ONCE 1
static void cip_test_ctrlr_cmd (
   mcitype *mci,
   int interface,
   int repeat,				
   ushort *response,
   ushort cmd,
   ulong larg)
{
    mciregtype *regptr;
    leveltype level;
    dcitype *dci;
    hwidbtype *hwidb;
    int i;
    int slot, slotunit;
    
    cbus_unit2slotunit(interface, &slot, &slotunit);
    dci = mci->dci[slot]; 
    hwidb = mci->interface_list[interface];
    regptr = mci->regptr;

    level = raise_interrupt_level(NETS_DISABLE);

    fci_select_if(hwidb, regptr);
    fci_select_ctrlr(hwidb, regptr);

    for (i = 0; i < repeat; i++)
	fci_ctrlr_qry_larg(dci, regptr, 
			  cmd, larg, response,
			  CTRLR_CMD_ATTEMPTS, TRUE);

    reset_interrupt_level(level);
}

#define BUFLEN 80
/*
 * parse_dci_command - Ask for cmd arg and count 
 */
static boolean parse_dci_command (
    ulong *argument,
    ushort *command,
    int *rcount)
{
    char buff[BUFLEN];
    boolean parsed;

    printf("\nrepeat count [%d]: ", *rcount);
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
        if (!null(buff))
 	    *rcount = (int) parse_unsigned(buff, &parsed);
    }
    printf("\ncommand [%d]: ", *command);
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
 	if (!null(buff))
 	    *command = (ushort) parse_unsigned(buff, &parsed);
    } else {
 	printf("\n%% bad command code");
 	return FALSE;
    }
    printf("\nargument [%d]: ", *argument);
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
 	if (!null(buff))
	    *argument = parse_unsigned(buff, &parsed);
    } else {
 	printf("\n%% bad argument");
 	return FALSE;
    }
    return TRUE;
}
/*
 * cip_dci_command - issue a CTRLR_CMD to the CIP
 */
static void cip_dci_command (
    mcitype *mci,
    int interface,
    ulong *argument,
    ushort *command,
    int *rcount)
{
   ushort response = 0;
  
   if (!parse_dci_command(argument, command, rcount))
       return;
   cip_test_ctrlr_cmd(mci, interface, 
		      *rcount, &response, 
		      *command, *argument);
   printf("\nvalue returned: 0x%-04x\n", response);
}

/*
 * channel_diag
 */
static void channel_diag (mcitype * mci, int interface)
{
    ulong diag_num;
    char buff[BUFLEN];
    boolean parsed;
    boolean okflag;
    ushort response;
    
    printf("\nEnter CIP diag #: ");
    parsed = rdtty(buff, BUFLEN);
    if (!null(buff) && parsed)
	diag_num = parse_unsigned(buff, &okflag);	
    else
	return;
    if (!okflag) {
	printf("\nbad value");
	return;
    }
    cip_test_ctrlr_cmd(mci, interface,
		       EXECUTE_ONCE, &response, 
		       FCI_CMD_CIP_DIAGNOSTIC, diag_num);
}

/*
 * channel_stat_interval
 */
static void channel_stats_interval (mcitype * mci, int interface)
{
    ulong seconds;
    char buff[BUFLEN];
    boolean parsed;
    boolean okflag;
    ushort response;

    printf("\nEnter new CIP stat interval in seconds: ");
    parsed = rdtty(buff, BUFLEN);
    if (!null(buff) && parsed)
	seconds = parse_unsigned(buff, &okflag);	
    else
	return;
    if (!okflag) {
	printf("\nbad value");
	return;
    }

    if (seconds >= CIP_MIN_STATS_INTERVAL)
	cip_stats_interval = (ushort) seconds;
    else {
	printf("\nMinimum value is %d", CIP_MIN_STATS_INTERVAL);
	return;
    }
    cip_test_ctrlr_cmd(mci, interface, 
		       EXECUTE_ONCE, &response,
		       FCI_CMD_CIP_STATS_INTERVAL, cip_stats_interval);
}


/*
 * channel_specific
 * Channel specific diagnostic commands
 *
 */
void channel_specific (mcitype *mci)
{
    char cmdchar;
    char buff[BUFLEN], *args;
    boolean parsed;
    ulong arglong;
    ushort command;
    int rcount;
    ulong mlow, mhigh;
    int slot;
    mcitype *mci2;
    uchar control;
    dump_state dumpstate = DUMP_STATE_NEVER;

    printf("\n\nChannel interface commands");
    arglong = command = mlow = mhigh = 0;
    cmdchar = '?';
    rcount = 1;   
    slot = default_interface / CBUS2_MAX_CTRLR_INTERFACES;
    mci2 = (mcitype*)cbusQ.qhead;

    while (TRUE) {
	printf("\nchannel (? for help) [%c]: ", cmdchar);
	parsed = rdtty(buff, BUFLEN);
	if (!null(buff) && parsed)
	    cmdchar = toupper(buff[0]);
	args = findarg(buff);
	if (stdio->statbits & (IDLETIMEOUT | HANGUP | CARDROP | IDLE))
	    return;
	automore_enable(NULL);
	if ((cmdchar != 'D') && (cmdchar != '?') &&
	    (dumpstate == DUMP_STATE_STARTED)) {
	  dumpstate = DUMP_STATE_DONE;
	  reset_dbus_control_reg (slot, control);
	}
	switch (cmdchar) {
	  case '?':
	    printf("\n");
	    printf("\nC                        - execute a controller command");
	    printf("\nD                        - dump main memory");
	    printf("\nL                        - logger drain");
	    printf("\nR                        - run CIP diagnostics");
	    printf("\nS                        - set stats interval");
	    printf("\nQ                        - quit");
	    break;

	  case 'C':
	    cip_dci_command(mci2, default_interface, 
			    &arglong, &command, &rcount);
	    break;

	  case 'D':
	    switch (dumpstate) {
	      case DUMP_STATE_DONE:
		printf ("\nWarning; CIP has been reset, memory has changed.");
	      case DUMP_STATE_NEVER: 
	        if (!save_dbus_control_reg (slot, &control))
		  continue;
		dumpstate = DUMP_STATE_STARTED;
		break;
	      case DUMP_STATE_STARTED:
		break;
	      }
	    cbus_address(&mlow, &mhigh, MAX_CHANNEL_PROCESSOR_MEMORY);
	    channel_dump_memory(slot, &mlow, &mhigh);
	    break;

	  case 'L':
	    logger_drain(slot);
	    break;

	  case 'R':
	    channel_diag(mci2, default_interface);
	    break;

	  case 'S':
	    channel_stats_interval(mci2, default_interface);
	    break;

	  case 'Q':
	    return;

	  default:
	    printf("\nunknown command \"%c\"", cmdchar);
	    cmdchar = '?';
	    break;
	}
	automore_disable();
    }
}
#endif
