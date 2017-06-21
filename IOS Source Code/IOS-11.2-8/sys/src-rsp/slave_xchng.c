/* $Id: slave_xchng.c,v 3.9.12.3 1996/08/29 22:57:57 ganesh Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/slave_xchng.c,v $
 *------------------------------------------------------------------
 * slave_xchng.c - slave half of m/s arbitration info exchange
 *
 * September 1995, David Bath
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: slave_xchng.c,v $
 * Revision 3.9.12.3  1996/08/29  22:57:57  ganesh
 * CSCdi62683:  HSA requires that both RSP2s have same amount of DRAM
 * Branch: California_branch
 * Fill in ms_memsize field with slave memory size that is now part
 * of slave signin structure.
 *
 * Revision 3.9.12.2  1996/08/28  13:14:34  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.12.1  1996/03/18  22:02:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.14.3  1996/03/16  07:36:56  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.14.2  1996/03/07  10:45:23  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.14.1  1996/02/20  18:02:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/15  00:53:10  dbath
 * CSCdi38455:  HSA needs SNMP support
 * Include major and minor SW versions in the slave signin message.
 * On the master, be sure to update the slave's SW version in the
 * chassis MIB, when the slave signs in.
 *
 * Revision 3.8  1996/01/23  01:18:00  dbath
 * CSCdi47277:  HSA slave leaks IPC buffers when version mismatch
 * Generate some form of response to all rpc requests, no matter how
 * little we might understand them.  Also, add a simple m/s exchange
 * version check on sign-in.
 *
 * Revision 3.7  1995/12/14  07:42:21  dbath
 * CSCdi45563:  Slave filesystem timestamps are wrong.
 * Ship the current time over to the slave every minute.
 *
 * Revision 3.6  1995/12/12  09:09:49  dbath
 * CSCdi45422:  HSA systems forget what time it is on failover
 * Implement client and server code for setting the calendar
 * on the slave.  Also, clean up some other IPC respone code
 * that was allocating unused data space.
 *
 * Revision 3.5  1995/12/11  04:09:25  dbath
 * CSCdi45341:  Cant get crash stack on HSA systems.
 * Add client and server code to gather slave crash stack details,
 * to the master/slave exchange mechanism.
 *
 * Revision 3.4  1995/12/05  07:06:37  dbath
 * CSCdi43178:  rommon boot info not kept in sync on slave
 * Ship the three boot related environment variables, and the config
 * register setting, across to the slave whenever autosync is enabled,
 * or whenever the user manually syncs the slave config.
 *
 * Revision 3.3  1995/11/22  03:43:56  dbath
 * CSCdi43201:  Unecessary errmsgs need to be collapsed into existing
 * errmsg
 * Rationalise error messages as promised in the doco review.
 *
 * Revision 3.2  1995/11/17  18:50:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:28:56  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/14  03:53:09  dbath
 * Placeholders for VIP_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "config.h"
#include "logger.h"
#include "../src-rsp/msg_rsp.c"		/* Not a typo, see logger.h */
#include "stacks.h"
#include "sys_registry.h"
#include "../os/clock.h"
#include "../dev/monitor1.h"
#include "../../boot/reason.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../hes/calendar_p.h"
#include "ms_xchng_private.h"
#include "ms_xchng.h"


/*
 * Handle a slave ROM service messages from the master.
 * These include:
 *   setms and getms
 *   get/set environment variables
 *   get/set configreg setting
 */
static void
slaverom_handler (ipc_message *req_msg, void *context, ipc_error_code ec)
{
    ipc_message *rsp_msg;
    ipc_message_header *req_hdr;
    slaverom_t *req;
    slaverom_t *rsp;
    whyreload_t *stack_rsp;
    slave_time_t *time_req;
    char *str;
    long *boot_data;

    req_hdr = req_msg->header;
    switch (req_hdr->type) {

    /*
     * We're being asked to provide our current ms setting.  We simply
     * read it directly from the ROM monitor into the response message,
     * and send it back.
     */
    case SLAVEROM_GETMS_REQ:
	rsp_msg = ipc_get_message(sizeof(slaverom_t), 0, SLAVEROM_GETMS_RSP);
	if (!rsp_msg) {
	    errmsg(&msgsym(IPC, RSP), 
		   "slave could not allocate GETMS response msg", "");
	    return;
	}
	rsp = rsp_msg->data;
	rsp->ms_setting = mon_get_ms();
	break;

    /*
     * We're being asked to set out current ms setting.  Simply grab
     * the value out of the request message and send it down to the
     * ROM monitor.  We send a content-free reply back to acknowledge
     * the call.
     */
    case SLAVEROM_SETMS_REQ:
	rsp_msg = ipc_get_message(0, 0, SLAVEROM_SETMS_RSP);
	if (!rsp_msg) {
	    errmsg(&msgsym(IPC, RSP), 
		   "slave could not allocate SETMS response msg", "");
	    return;
	}
	req = req_msg->data;
	mon_set_ms(req->ms_setting);
	break;

    /*
     * We're being asked to issue a reg_invoke_getvar.  Grab
     * the code out of the request message and send it down to the
     * ROM monitor.  Send the results back to acknowledge
     * the call.
     */
    case SLAVEROM_GETVAR_REQ:
	rsp_msg = ipc_get_message(sizeof(slaverom_t), 0, SLAVEROM_GETVAR_RSP);
	if (!rsp_msg) {
	    errmsg(&msgsym(IPC, RSP), 
		   "slave could not allocate GETVAR response msg", "");
	    return;
	}
	req = req_msg->data;
	rsp = rsp_msg->data;
	str = reg_invoke_monvar_get(req->monvar_code);
	if (str) {
	    rsp->monvar_exists = TRUE;
	    strncpy(rsp->monvar_value, str, MAX_BOOTVAR_LEN);
	} else {
	    rsp->monvar_exists = FALSE;
	}
	break;

    /*
     * We're being asked to issue a mon_getconfig().
     */
    case SLAVEROM_GETCONF_REQ:
	rsp_msg = ipc_get_message(sizeof(slaverom_t), 0, SLAVEROM_GETCONF_RSP);
	if (!rsp_msg) {
	    errmsg(&msgsym(IPC, RSP), 
		   "slave could not allocate GETCONF response msg", "");
	    return;
	}
	rsp = rsp_msg->data;
	rsp->confreg_setting = ~mon_getconfig();
	break;

    /*
     * We're being asked to return our crash stack details
     */
    case SLAVEROM_GETSTACK_REQ:
	rsp_msg = ipc_get_message(sizeof(whyreload_t), 0, 
				  SLAVEROM_GETSTACK_RSP);
	if (!rsp_msg) {
	    errmsg(&msgsym(IPC, RSP), 
		   "slave could not allocate GETSTACK response msg", "");
	    return;
	}
	stack_rsp = rsp_msg->data;

	/*
	 * Fetch the boot data from the ROM monitor, and see if it looks
	 * healthy.
	 */
	boot_data = mon_lasterror();
	if (boot_data && (boot_data != (long *)-1)) {
	    char *rom_string;

	    /*
	     * The ROM monitor gave us something back, so gather it up and
	     * send it back to the master for further analysis.
	     */
	    memcpy(stack_rsp->boot_data, boot_data, sizeof(stack_rsp->boot_data));

	    /*
	     * Next we need to see if BOOT_REASON is a simple code, or
	     * whether it requires us to go hunting for even more strings.
	     */
	    rom_string = (char *)stack_rsp->boot_data[BOOT_REASON];

	    /*
	     * If it's larger than any of the reason codes, and it's
	     * a legal address, and it's an ascii string, then we'll
	     * assume it's a ROM monitor string that it also needs to be
	     * passed back.  This algorithm is stolen from make_reason().
	     */
	    if (((ulong)rom_string > EXC_MAX)  && touch_device(rom_string) && 
		is_string(rom_string)) {
		strncpy(stack_rsp->rom_string, rom_string, MAX_ROM_REASON_LEN);

		/*
		 * Make sure there's a trailing NULL just in case we were
		 * forced to truncate a particularly verbose ROM monitor.
		 */
		stack_rsp->rom_string[MAX_ROM_REASON_LEN-1] = '\0';
	    } else {
		/*
		 * Doesn't look like it's a string pointer, so nullify what
		 * we send back, to let the master know nothing was found.
		 */
		*(stack_rsp->rom_string) = '\0';
	    }
	}
	else {
	    /*
	     * The boot_data[] wasn't stored, or has somehow been corrupted.
	     * Either way, nullify the array and the string so that the master
	     * will know we have nothing to offer.  A 0 in the BOOT_COUNT field
	     * indicates an invalid boot_data[] array, since the first long word
	     * should be equal to BOOTDATA.
	     */
	    stack_rsp->boot_data[BOOT_COUNT] = 0;
	    *(stack_rsp->rom_string) = '\0';
	}

	/*
	 * Give 'em the last version string
	 */
	if (last_sysversion)
	    strcpy(stack_rsp->prior_version_string, last_sysversion);
	else
	    *(stack_rsp->prior_version_string) = '\0';
	break;

    /*
     * We're being asked to send the provided environment variable
     * down to the ROM monitor.  Note the provided string is a complete
     * string comprising of the variable name, an "=", and it's value.
     * Send back an empty message to acknowledge the call.
     */
    case SLAVEROM_SETVAR_REQ:
	rsp_msg = ipc_get_message(0, 0, SLAVEROM_SETVAR_RSP);
	if (!rsp_msg) {
	    errmsg(&msgsym(IPC, RSP), 
		   "slave could not allocate SETVAR response msg", "");
	    return;
	}
	req = req_msg->data;
	mon_set_var(req->monvar_value);
	break;

    /*
     * We're being asked to send the provided config register 
     * down to the ROM monitor.
     * Send back an empty message to acknowledge the call.
     */
    case SLAVEROM_SETCONF_REQ:
	rsp_msg = ipc_get_message(0, 0, SLAVEROM_SETCONF_RSP);
	if (!rsp_msg) {
	    errmsg(&msgsym(IPC, RSP), 
		   "slave could not allocate SETCONF response msg", "");
	    return;
	}
	req = req_msg->data;
	mon_set_confreg(req->confreg_setting);
	break;

    /*
     * We're being asked to set the battery backed calendar hardware to
     * the value passed.
     * Send back an empty message to acknowledge the call.
     */
    case SLAVEROM_SETCALENDAR_REQ:
	rsp_msg = ipc_get_message(0, 0, SLAVEROM_SETCALENDAR_RSP);
	if (!rsp_msg) {
	    errmsg(&msgsym(IPC, RSP), 
		   "slave could not allocate SETCALENDAR response msg", "");
	    return;
	}
	write_calendar(req_msg->data);
	break;

    /*
     * We're being asked to set the current system time.
     * Send back an empty message to acknowledge the call.
     */
    case SLAVEROM_SETTIME_REQ:
	rsp_msg = ipc_get_message(0, 0, SLAVEROM_SETTIME_RSP);
	if (!rsp_msg) {
	    errmsg(&msgsym(IPC, RSP), 
		   "slave could not allocate SETTIME response msg", "");
	    return;
	}
	time_req = req_msg->data;
	clock_set(&time_req->current_time, time_req->source);
	if (time_req->valid)
	    clock_is_now_valid();
	    
	break;

    /*
     * Who knows what we're being asked to do here. We'll respond
     * with an UNKNOWN response code and log an error.
     */
    default:
	rsp_msg = ipc_get_message(0, 0, SLAVEROM_UNKNOWN_RSP);
	errmsg(&msgsym(IPC, RSP), "slave received bad msg", "");
	break;
    }
    ipc_send_rpc_reply(req_msg, rsp_msg);

}

/*
 * Initialise the slave's ROM services.  This service reads and sets
 * the ROM monitor "setms" status on the slave, as requested by
 * the master, plus reads environment variables and config registers,
 * clocks and calendars.
 */
static void
setup_slaverom_service (void)
{
    static ipc_port_id slaverom_port;
    ipc_error_code ec;
    
    /*
     * We need to create a named port, set up the receive callback, and
     * register the port.
     */
    ec = ipc_create_named_port(SLAVEROM_PORT_NAME, &slaverom_port, 0);
    if (ec != IPC_OK) {
	errmsg(&msgsym(IPC, RSP), "slave could not create named port",
	       ipc_decode_error(ec));
        return;
    }

    ec = ipc_set_receive_callback(slaverom_port, 0, slaverom_handler);
    if (ec != IPC_OK) {
	errmsg(&msgsym(IPC, RSP), "slave could not set callback",
	       ipc_decode_error(ec));
	return;
    }

    ec = ipc_register_port(slaverom_port);
    if (ec != IPC_OK) {
	errmsg(&msgsym(IPC, RSP), "slave could not register port",
	       ipc_decode_error(ec));
        return;
    }
}

/*
 * Generate a sign-in message to the master.
 */
static void
slave_signin (void)
{
    static ipc_port_id signin_port;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    slave_signin_t *slave_signin;

    /*
     * Firstly, advertise our own SLAVEROM service port, since the
     * master is going to want to be able to call us once we
     * sign in.
     */
    setup_slaverom_service();

    /*
     * Find the port.
     */
    signin_port = ipc_locate_port(SIGNIN_PORT_NAME);
    if (!signin_port) {
	errmsg(&msgsym(IPC, RSP), "slave could not find registration port",
	       "");
	return;
    }

    req_msg = ipc_get_message(sizeof(slave_signin_t), signin_port, 0);
    if (!req_msg) {
	errmsg(&msgsym(IPC, RSP), "slave could not allocate sign-in message",
	       "");
	return;
    }

    /*
     * Load up our various versions:
     *   ms_version - version of this rpc interface
     *   version_string - full bodied version string displayed in "show vers"
     *   minor/major_sysversion - used by master's chassis MIB for slave RSP
     */
    slave_signin = req_msg->data;
    slave_signin->ms_version = MS_XCHNG_VERSION;
    slave_signin->ms_memsize = mon_getmemsize();
    strncpy(slave_signin->version_string, version, CW_MAX_DESCR_STRLEN);
    slave_signin->sw_version = (SYS_MAJORVERSION << 8) | SYS_MINORVERSION;
	
    rsp_msg = ipc_send_rpc(req_msg, &ec);
    if (!rsp_msg) {
	errmsg(&msgsym(IPC, RSP), "slave sign-in failed", "");
	return;
    }

    /*
     * It's our job to free up the response message.
     */
    ipc_return_message(rsp_msg);

}

/*
 * Do the necessary IPC initialisation to allow both the master and
 * slave to handshake upon booting (known as the slave sign-in).  
 * The signin message is a simple
 * one-shot message from the slave to the master, saying ....
 * "Hi, I'm here, my MEMD is mapped, and here is my version string".
 * It is the receipt of this message that promotes the slave_state
 * to SLAVE_RUNNING.   What follows is the slave implementation of
 * this initialisation.  The master also provides a routine of the
 * same name to perform the master side of this initialisation.
 */
void
rsp_slave_init (void)
{
    /*
     * While running on the slave, we are the 'client'.  At the time
     * this code runs on the slave, IPC is not fully established.
     * In particular, it won't  have it's final 'seat' allocation
     * until the master has set things up.  Also, the slave IPC
     * rx mechanism is a polling process that is not running yet.
     * We therefore use a "deferral" technique similar to what
     * the filesystem uses.  This will cause IPC to call us back
     * later, when it's truly ready for operations.
     */
    ipc_register_delayed_func(IPC_LEVEL_OTHERS, slave_signin);
}

