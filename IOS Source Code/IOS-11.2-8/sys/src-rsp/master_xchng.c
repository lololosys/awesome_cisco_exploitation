/* $Id: master_xchng.c,v 3.12.6.4 1996/08/29 22:59:08 ganesh Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/master_xchng.c,v $
 *------------------------------------------------------------------
 * master_xchng.c - master half of m/s arbitration info exchange
 *
 * September 1995, David Bath
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: master_xchng.c,v $
 * Revision 3.12.6.4  1996/08/29  22:59:08  ganesh
 * CSCdi62683:  HSA requires that both RSP2s have same amount of DRAM
 * Branch: California_branch
 * Print out a warning message if HSA requirements a) Both master and
 * slave should have same amount of memory. b) Minimum memory size
 * is 24mb. are not met.
 *
 * Revision 3.12.6.3  1996/08/28  13:12:57  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.12.6.2  1996/05/21  06:40:18  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.12.6.1  1996/03/18  22:01:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.2.3  1996/03/16  07:35:48  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.9.2.2  1996/03/07  10:44:19  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.9.2.1  1996/02/20  17:59:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.12  1996/03/05  01:25:12  dbath
 * CSCdi47229:  HSA slave reload and slave <image> commands need to work
 * Allow non-bundled slave images to be used in the slave.
 *
 * Revision 3.11  1996/02/15  00:53:06  dbath
 * CSCdi38455:  HSA needs SNMP support
 * Include major and minor SW versions in the slave signin message.
 * On the master, be sure to update the slave's SW version in the
 * chassis MIB, when the slave signs in.
 *
 * Revision 3.10  1996/01/23  01:17:53  dbath
 * CSCdi47277:  HSA slave leaks IPC buffers when version mismatch
 * Generate some form of response to all rpc requests, no matter how
 * little we might understand them.  Also, add a simple m/s exchange
 * version check on sign-in.
 *
 * Revision 3.9  1996/01/22  07:15:48  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.8  1995/12/14  07:42:17  dbath
 * CSCdi45563:  Slave filesystem timestamps are wrong.
 * Ship the current time over to the slave every minute.
 *
 * Revision 3.7  1995/12/12  09:09:44  dbath
 * CSCdi45422:  HSA systems forget what time it is on failover
 * Implement client and server code for setting the calendar
 * on the slave.  Also, clean up some other IPC respone code
 * that was allocating unused data space.
 *
 * Revision 3.6  1995/12/11  04:09:22  dbath
 * CSCdi45341:  Cant get crash stack on HSA systems.
 * Add client and server code to gather slave crash stack details,
 * to the master/slave exchange mechanism.
 *
 * Revision 3.5  1995/12/05  07:06:25  dbath
 * CSCdi43178:  rommon boot info not kept in sync on slave
 * Ship the three boot related environment variables, and the config
 * register setting, across to the slave whenever autosync is enabled,
 * or whenever the user manually syncs the slave config.
 *
 * Revision 3.4  1995/11/24  21:33:14  yanke
 * Change slave sync command to be privileged.
 * Raw nvram access shouldn't update rom environment variables
 *
 * CSCdi42679:  new exec command should be privileged
 *
 * Revision 3.3  1995/11/22  03:43:48  dbath
 * CSCdi43201:  Unecessary errmsgs need to be collapsed into existing
 * errmsg
 * Rationalise error messages as promised in the doco review.
 *
 * Revision 3.2  1995/11/17  18:46:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:27:44  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/14  03:53:08  dbath
 * Placeholders for VIP_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <string.h>
#include "config.h"
#include "logger.h"
#include "../src-rsp/msg_rsp.c"		/* Not a typo, see logger.h */
#include "interface_private.h"
#include "sys_registry.h"
#include "../snmp/chassismib_registry.h"
#include "../os/clock.h"
#include "../dev/monitor1.h"
#include "../../boot/reason.h"
#include "../../boot/save_stack.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../hes/calendar_p.h"
#include "../hes/if_fci.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "ms_xchng_private.h"
#include "ms_xchng.h"
#include "rsp_if.h"
#include "rsp_init.h"

static slave_signin_t  slave_signin_info;
static tinybool ms_version_mismatch;

#define NO_SLAVE_STACK   "\n\nCannot display slave stack due to IPC error"

/*
 * If there is a slave installed, show it's nvram crash stack, if it has
 * one.  Called as part of the "show stack" chain.
 */
static void
slave_stack_whyreload (void)
{
    slave_state_t slave_state;
    static ipc_port_id slaverom_port;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    whyreload_t *rsp;


    slave_state = rsp_slave_state();

    /*
     * No suprises for customers who haven't bothered to buy a second
     * RSP.
     */
    if (slave_state == SLAVE_UNPLUGGED)
	return;

    /*
     * If it's installed, but not running, let them know so they can
     * hopefully fix it.
     */
    if (slave_state != SLAVE_RUNNING) {
	printf("\n\nSlave in slot %d is halted.", get_rsp_peer_slot());
	return;
    }

    /*
     * It's installed and running, so launch an IPC to gather it's
     * boot_data[] details.  Start by locating the slave's message
     * port if we haven't already.
     */
    if (!slaverom_port)
	slaverom_port = ipc_locate_port(SLAVEROM_PORT_NAME);

    if (!slaverom_port) {
	printf(NO_SLAVE_STACK);
	return;
    }

    /*
     * Next, allocate a message of the correct type and size.
     */
    req_msg = ipc_get_message(0, slaverom_port, SLAVEROM_GETSTACK_REQ);
    if (!req_msg) {
	printf(NO_SLAVE_STACK);
	return;
    }

    /*
     * Send the request
     */
    rsp_msg = ipc_send_rpc(req_msg, &ec);
    if ((ec != IPC_OK) || (!rsp_msg)) {
	printf(NO_SLAVE_STACK);
	return;
    }
    rsp = rsp_msg->data;

    /*
     * Now analyse and display what came back.
     */
    if ((rsp->boot_data[BOOT_COUNT] == BOOTDATA) &&
	(rsp->boot_data[BOOT_REASON] != EXC_RESET) &&
	(rsp->boot_data[BOOT_REASON] != EXC_ABORT) &&
	(rsp->boot_data[BOOT_REASON] != EXC_RELOAD)) {

	uint pc, address;
	struct frame *frames;

	pc = rsp->boot_data[BOOT_PC];
	address = rsp->boot_data[BOOT_ADDRESS];

	printf("\n\nSlave in slot %d was restarted by ", get_rsp_peer_slot());
	switch (rsp->boot_data[BOOT_REASON]) {
	case EXC_BUSERR:
	    printf("bus error at PC %#lx, address %#lx", pc, address);
	    break;
	case EXC_ADRERR:
	    printf("address error at PC %#lx, address %#lx", pc, address);
	    break;
	case EXC_WATCHDOG:
	    printf("watchdog timer expired");
	    break;
	case EXC_PARITY:
	    printf("processor memory parity error at PC %#lx, address %#lx",
		   pc, address);
	    break;
	case EXC_SHMEM_PE:
	    printf("shared memory parity error at PC %#lx", pc);
	    break;
	case EXC_UNEXP_INT:
	    printf("unexpected interrupt at PC %#lx, address %#lx", pc, address);
	    break;
	default:
	    /*
	     * The "code" may have been a pointer to a string, in which
	     * case the slave will have loaded the string into rom_string[],
	     * if not, it'll be a null string, so print it either way
	     */
	    printf("error - %s, PC %#lx", rsp->rom_string, pc);
	    break;
	}

	frames = (struct frame *)&rsp->boot_data[BOOT_STACK_TRACE];
	if ((frames != (struct frame *)NULL) &&
	    ((frames[0].fp != 0) || (frames[0].pc != 0))) {
	    int i;

	    if (strlen(rsp->prior_version_string))
		printf("\n%s", rsp->prior_version_string);
	    else
		printf("\nVersion unknown");
	    printf("\nStack trace from system failure:\n");
	    for (i = 0; i < BOOT_STACK_FRAMES; i++) {
		if ((frames[i].fp == 0) && (frames[i].pc == 0))
		    break;
		printf("FP: %#x, RA: %#x\n", frames[i].fp, frames[i].pc);
	    }
	}
    }

    ipc_return_message(rsp_msg);
}


/*
 * If there is a slave installed and running, update it's calendar.
 * Called whenever the master's calendar is written to.
 */
static void
write_slave_calendar (calendar_t *cal)
{
    static ipc_port_id slaverom_port;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;


    /*
     * If the slave is not installed or running, there's no point proceeding,
     * so just silently return.
     */
    if (rsp_slave_state() != SLAVE_RUNNING)
	return;

    /*
     * It's installed and running, so launch an IPC to set it's
     * calendar.  Start by locating the slave's message
     * port if we haven't already.
     */
    if (!slaverom_port)
	slaverom_port = ipc_locate_port(SLAVEROM_PORT_NAME);

    if (!slaverom_port)
	return;

    /*
     * Next, allocate a message of the correct type and size.
     */
    req_msg = ipc_get_message(sizeof(calendar_t), slaverom_port, 
			      SLAVEROM_SETCALENDAR_REQ);
    if (!req_msg)
	return;

    /*
     * Send the request
     */
    memcpy(req_msg->data, cal, sizeof(calendar_t));
    rsp_msg = ipc_send_rpc(req_msg, &ec);
    if ((ec != IPC_OK) || (!rsp_msg))
	return;

    ipc_return_message(rsp_msg);
}


/*
 * Send the time over to the slave once per minute.
 */
process set_slave_time (void)
{
    static ipc_port_id slaverom_port;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    slave_time_t *req_time;

    while (TRUE) {
	process_sleep_for(ONEMIN);

	/*
	 * If the slave is not installed or running, there's no point 
	 * proceeding, so just silently try again later.
	 */
	if (rsp_slave_state() != SLAVE_RUNNING)
	    continue;

	/*
	 * It's installed and running, so launch an IPC to set it's
	 * calendar.  Start by locating the slave's message
	 * port if we haven't already.
	 */
	if (!slaverom_port)
	    slaverom_port = ipc_locate_port(SLAVEROM_PORT_NAME);

	if (!slaverom_port)
	    continue;

	/*
	 * Next, allocate a message of the correct type and size.
	 */
	req_msg = ipc_get_message(sizeof(slave_time_t), slaverom_port, 
				  SLAVEROM_SETTIME_REQ);
	if (!req_msg)
	    continue;

	/*
	 * Send the request
	 */
	req_time = req_msg->data;
	req_time->valid = clock_is_probably_valid();
	req_time->source = current_time_source();
	clock_get_time(&req_time->current_time);
	rsp_msg = ipc_send_rpc(req_msg, &ec);
	if ((ec != IPC_OK) || (!rsp_msg))
	    continue;

	ipc_return_message(rsp_msg);
    }

}


/*
 * Display the slave state, and if available, version info.  Called as part
 * of the "show version" chain.
 */
static void
print_slave_info (void)
{
    uchar peer_slot = get_rsp_peer_slot();

    switch (rsp_slave_state()) {

    case SLAVE_UNPLUGGED:
	printf("\nNo slave installed in slot %d.", peer_slot);
	break;
    case SLAVE_CRASHED:
    case SLAVE_NEEDS_BOOT:
	if (ms_version_mismatch) {
	    printf("\nSlave in slot %d is halted due to m/s version mismatch.",
		   peer_slot);
	    printf("\nMaster has m/s version %d, slave has m/s version %d.",
		   MS_XCHNG_VERSION, slave_signin_info.ms_version);
	} else {
	    printf("\nSlave in slot %d is halted.", peer_slot);
	}
	break;
    case SLAVE_RUNNING:
	printf("\n\nSlave in slot %d is running %s\nLoaded from %s %s\n", 
	       peer_slot, slave_signin_info.version_string,
	       get_ucode_src(peer_slot), get_ucode_src_file(peer_slot));
	printf("Slave RSP has %dK bytes of memory.\n",
	       slave_signin_info.ms_memsize/1024);
	break;
    }
}


/*
 * Call mon_getconfig() on the slave.  If successful, return TRUE, 
 * and the value associated with the slave's config register.
 */
static boolean
get_slave_config (ushort* value)
{
    static ipc_port_id slaverom_port;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    slaverom_t *rsp;

    /*
     * Firstly, do a quick sanity check to see if we even think the
     * slave is running.  If we know it's not, it's better to shortcut
     * this now, then to wait for rpc timeouts.
     */
    if (rsp_slave_state() != SLAVE_RUNNING) {
	return FALSE;
    }


    /*
     * Next,  find the slave's message port if we haven't already.
     */
    if (!slaverom_port)
	slaverom_port = ipc_locate_port(SLAVEROM_PORT_NAME);

    if (!slaverom_port) {
	return FALSE;
    }

    /*
     * Next, allocate a message of the correct type and size.
     */
    req_msg = ipc_get_message(sizeof(slaverom_t), slaverom_port, 
			      SLAVEROM_GETCONF_REQ);
    if (!req_msg) {
	return FALSE;
    }

    /*
     * Send the request
     */
    rsp_msg = ipc_send_rpc(req_msg, &ec);
    if ((ec != IPC_OK) || (!rsp_msg)) {
	return FALSE;
    }

    rsp = rsp_msg->data;
    *value = rsp->confreg_setting;

    ipc_return_message(rsp_msg);
    return TRUE;
}


/*
 * get_slave_monvar 
 * Invoke monvar_get on the slave.
 *
 * Inputs
 *   monvar_code - one of the ROMMON_xxx_VAR family
 *
 * Outputs
 *   function value - TRUE iff IPC was successful
 *   exists - TRUE if the environment variable exists (and IPC successful)
 *   value - contents of the variable (if exists), otherwise NULL
 */
boolean
get_slave_monvar (int monvar_code, boolean *exists, char *value)
{
    static ipc_port_id slaverom_port;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    slaverom_t *rsp;
    slaverom_t *req;

    /*
     * Null out the output string, just in case we fail, and the
     * the caller doesn't bother to check the various failure flags.
     */
    *value = '\0';

    /*
     * Firstly, do a quick sanity check to see if we even think the
     * slave is running.  If we know it's not, it's better to shortcut
     * this now, then to wait for rpc timeouts.
     */
    if (rsp_slave_state() != SLAVE_RUNNING) {
	return FALSE;
    }


    /*
     * Next,  find the slave's message port if we haven't already.
     */
    if (!slaverom_port)
	slaverom_port = ipc_locate_port(SLAVEROM_PORT_NAME);

    if (!slaverom_port) {
	return FALSE;
    }

    /*
     * Next, allocate a message of the correct type and size.
     */
    req_msg = ipc_get_message(sizeof(slaverom_t), slaverom_port, 
			      SLAVEROM_GETVAR_REQ);
    if (!req_msg) {
	return FALSE;
    }

    /*
     * Send the request
     */
    req = req_msg->data;
    req->monvar_code = monvar_code;
    rsp_msg = ipc_send_rpc(req_msg, &ec);
    if ((ec != IPC_OK) || (!rsp_msg)) {
	return FALSE;
    }

    rsp = rsp_msg->data;
    *exists = rsp->monvar_exists;

    if (*exists) {
	strncpy(value, rsp->monvar_value, MAX_BOOTVAR_LEN);
    }

    ipc_return_message(rsp_msg);
    return TRUE;
}



/*
 * Display the slave state, and if available, boot info.  Called as part
 * of the "show boot" chain.
 */
static void
show_slave_boot (void)
{
    char *str;
    ushort slave_configreg;
    boolean exists;

    printf("\nSlave auto-sync config mode is %s\n",
	get_slave_auto_sync_mode() ? "on" : "off");

    switch (rsp_slave_state()) {

    case SLAVE_UNPLUGGED:
	printf("\nNo slave installed in slot %d.", get_rsp_peer_slot());
	break;

    case SLAVE_CRASHED:
    case SLAVE_NEEDS_BOOT:
	printf("\nCurrent slave in slot %d is halted.", get_rsp_peer_slot());
	break;

    case SLAVE_RUNNING:
	printf("\nCurrent slave is in slot %d.", get_rsp_peer_slot());

	str = malloc(MAX_BOOTVAR_LEN);
	if (!str) return;

	printf("\nslave BOOT variable ");
	if (get_slave_monvar(ROMMON_BOOT_VAR, &exists, str)) {
	    if (exists)
		printf("= %s", str);
	    else
		printf("does not exist");
	} else {
	    printf("is unobtainable due to IPC error");
	}

	printf("\nslave CONFIG_FILE variable ");
	if (get_slave_monvar(ROMMON_CONFIG_VAR, &exists, str)) {
	    if (exists)
		printf("= %s", str);
	    else
		printf("does not exist");
	} else {
	    printf("is unobtainable due to IPC error");
	}

	printf("\nslave BOOTLDR variable ");
	if (get_slave_monvar(ROMMON_BOOTLDR_VAR, &exists, str)) {
	    if (exists)
		printf("= %s", str);
	    else
		printf("does not exist");
	} else {
	    printf("is unobtainable due to IPC error");
	}

	printf("\nslave Configuration register is ");
	if (get_slave_config(&slave_configreg)) {
	    printf("%#x\n", slave_configreg);
	} else {
	    printf("unobtainable due to IPC error");
	}

	free(str);
	break;
    }
}


/*
 * Handle a sign-in message from the slave.
 * We need to generate a response
 * as part of the RCP mechanism, even though we don't have any information
 * we need to give to the slave.
 */
process slave_signin_action (void)
{
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    slave_signin_t *slave_message;
    ulong master_memsize;

   if (!process_get_arg_ptr((void**)&req_msg))
	process_kill(THIS_PROCESS);

    /*
     * Extract and keep the information he's sent us.
     */
    slave_message = req_msg->data;
    slave_signin_info = *slave_message;

    ms_version_mismatch = (slave_signin_info.ms_version != MS_XCHNG_VERSION);
    if (ms_version_mismatch) {
	errmsg(&msgsym(MSVERS, RSP), MS_XCHNG_VERSION, 
	       slave_signin_info.ms_version);
    }

    master_memsize = mon_getmemsize();
    if (slave_signin_info.ms_memsize != master_memsize) {
        errmsg(&msgsym(HSA_MEM, RSP), master_memsize/(1024*1024),
               slave_signin_info.ms_memsize/(1024*1024));
    }
 
    if (slave_signin_info.ms_memsize < HSA_MIN_MEMSIZE ||
        master_memsize < HSA_MIN_MEMSIZE) {
        errmsg(&msgsym(HSA_MINMEM, RSP), HSA_MIN_MEMSIZE/(1024*1024));
    }

    rsp_msg = ipc_get_message(0, 0, 0);
    if (rsp_msg) {
	ipc_send_rpc_reply(req_msg, rsp_msg);
    } else {
	errmsg(&msgsym(IPC, RSP),
	       "master could not allocate response message", "");
    }

    /*
     * Now act on all the good stuff we've just received, assuming
     * there were no errors.
     */
    if (!ms_version_mismatch && rsp_msg) {

	char* cp;
	char* swver;
	char* hwver;
	char versions[48];
	int card_index;
	ushort slave_slot = get_rsp_peer_slot();
	slottype* slotp = &slots[slave_slot];
	

	/*
	 * Firstly, update the slave state machine being driven by L6
	 * interrupts in rsp_init.c
	 */
	promote_slave_state();

	/*
	 * Next, update the s/w version stored in the slot structure, and
	 * in the chassis MIB.
	 */
	slotp->sw_version = slave_signin_info.sw_version;
	cp = versions;
	cp += sprintf(hwver = cp, "%u.%u",
		      slotp->hw_version >> 8, slotp->hw_version & 0xFF);
	cp += sprintf(swver = ++cp, "%u.%u",
		      slotp->sw_version >> 8, slotp->sw_version & 0xFF);
	card_index = reg_invoke_chassismib_find_slot_card_index(slave_slot);
	reg_invoke_chassismib_update_sub_card_entry(card_index,
						    slotp->serial_number,
						    hwver, swver);

	/*
	 * Finally, check that the ROM monitor's master/slave settings
	 * aren't out of whack with what's specified in the system config
	 * file.
	 */
	sanitise_ms_settings();
    }

    process_sleep_for(ONESEC);
    process_kill(THIS_PROCESS);
}


/*
 * Handle a sign-in message from the slave.  This routine merely
 * forks a new process, which does all the real work.  By running
 * in the context of a new process, we can freely callback to the
 * slave from within the handler, without fear of locking up the
 * ipc deferred startup code on the slave.
 */
static void
slave_signin_handler (ipc_message *req_msg, void *context, ipc_error_code ec)
{
    int result;

    result = process_create(slave_signin_action, "Slave Server", LARGE_STACK, 
			    PRIO_NORMAL);
    if (result != NO_PROCESS) {
	process_set_arg_ptr(result, req_msg);
    }
}


/*
 * Send the provided environment variable over to the slave, regardless
 * of whether or not auto sync mode is enabled.  The variable passed to
 * us is a completed string, i.e includes both the variable name, "=", and
 * its contents.  Hence, we don't need to know which environment variable
 * we're setting.
 *
 * Returns TRUE iff successful.
 */
static boolean
set_slave_monvar (char *env_variable)
{
    static ipc_port_id slaverom_port;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    slaverom_t *req;

    /*
     * Firstly, do a quick sanity check to see if we even think the
     * slave is running.  If we know it's not, it's better to shortcut
     * this now, then to wait for rpc timeouts.
     */
    if (rsp_slave_state() != SLAVE_RUNNING) {
	return FALSE;
    }


    /*
     * Next,  find the slave's message port if we haven't already.
     */
    if (!slaverom_port)
	slaverom_port = ipc_locate_port(SLAVEROM_PORT_NAME);

    if (!slaverom_port) {
	return FALSE;
    }

    /*
     * Next, allocate a message of the correct type and size.
     */
    req_msg = ipc_get_message(sizeof(slaverom_t), slaverom_port, 
			      SLAVEROM_SETVAR_REQ);
    if (!req_msg) {
	return FALSE;
    }

    /*
     * Send the request
     */
    req = req_msg->data;
    strcpy(req->monvar_value, env_variable);
    rsp_msg = ipc_send_rpc(req_msg, &ec);
    if ((ec != IPC_OK) || (!rsp_msg)) {
	return FALSE;
    }
    ipc_return_message(rsp_msg);
    return TRUE;
}

/*
 * Send the provided environment variable over to the slave, if auto
 * sync mode is configured, and a slave is installed.  If it's not 
 * configured, or there is no slave installed, do nothing.
 */
static void
auto_set_slave_monvar (char *env_variable, boolean vocal)
{
    if (get_slave_auto_sync_mode() && (rsp_slave_state() != SLAVE_UNPLUGGED))
	if (!set_slave_monvar(env_variable) && vocal)
	    printf("\n%% Cannot communicate boot variable to slave");
}


/*
 * Send the provided confreg setting over to the slave, regardless
 * of whether or not auto sync mode is enabled.
 *
 * Returns TRUE iff successful.
 */
static boolean
set_slave_confreg (ushort number)
{
    static ipc_port_id slaverom_port;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    slaverom_t *req;

    /*
     * Firstly, do a quick sanity check to see if we even think the
     * slave is running.  If we know it's not, it's better to shortcut
     * this now, then to wait for rpc timeouts.
     */
    if (rsp_slave_state() != SLAVE_RUNNING) {
	return FALSE;
    }


    /*
     * Next,  find the slave's message port if we haven't already.
     */
    if (!slaverom_port)
	slaverom_port = ipc_locate_port(SLAVEROM_PORT_NAME);

    if (!slaverom_port) {
	return FALSE;
    }

    /*
     * Next, allocate a message of the correct type and size.
     */
    req_msg = ipc_get_message(sizeof(slaverom_t), slaverom_port, 
			      SLAVEROM_SETCONF_REQ);
    if (!req_msg) {
	return FALSE;
    }

    /*
     * Send the request
     */
    req = req_msg->data;
    req->confreg_setting = number;
    rsp_msg = ipc_send_rpc(req_msg, &ec);
    if ((ec != IPC_OK) || (!rsp_msg)) {
	return FALSE;
    }
    ipc_return_message(rsp_msg);
    return TRUE;
}

/*
 * Send the provided config register setting to the slave, if
 * sync mode is configured, and a slave is installed.  If it's not 
 * configured, or there is no slave installed, do nothing.
 */
void
auto_set_slave_confreg (ushort number, boolean vocal)
{
    if (get_slave_auto_sync_mode() && (rsp_slave_state() != SLAVE_UNPLUGGED))
	if (!set_slave_confreg(number) && vocal)
	    printf("\n%% Cannot communicate config register to slave");
}


/*
 * Called when the user manually issues a "slave sync config" command.
 * Grab the current ROM monitor settings of the BOOT, CONFIG_FILE,
 * and BOOTLDR variables, and send them across to the slave.
 */
void manual_set_slave_monvars (boolean vocal)
{
    char *var_contents;
    char *var_name;
    ushort master_config;

    if (rsp_slave_state() != SLAVE_RUNNING) {
	if (vocal)
	    printf("\n%% Cannot communicate boot variables to slave");
	return;
    }
    /*
     * No point attempting a printf() if we're out of memory.
     * The system will be alerting the user to low memory conditions.
     */
    var_contents = malloc(MAX_BOOTVAR_LEN);
    if (!var_contents)
	return;

    var_name = malloc(MAX_BOOTVAR_LEN);
    if (!var_name) {
	free(var_contents);
	return;
    }

    strcpy(var_name, BOOT_VAR_NAME_STRING);
    mon_get_var(var_name, var_contents, MAX_BOOTVAR_LEN);
    strcat(var_name, "=");
    strcat(var_name, var_contents);
    if (!set_slave_monvar(var_name) && vocal)
	printf("\n%% Cannot communicate boot variable to slave");

    strcpy(var_name, CONFIG_VAR_NAME_STRING);
    mon_get_var(var_name, var_contents, MAX_BOOTVAR_LEN);
    strcat(var_name, "=");
    strcat(var_name, var_contents);
    if (!set_slave_monvar(var_name) && vocal)
	printf("\n%% Cannot communicate boot variable to slave");

    strcpy(var_name, BOOTLDR_VAR_NAME_STRING);
    mon_get_var(var_name, var_contents, MAX_BOOTVAR_LEN);
    strcat(var_name, "=");
    strcat(var_name, var_contents);
    if (!set_slave_monvar(var_name) && vocal)
	printf("\n%% Cannot communicate boot variable to slave");

    free(var_contents);
    free(var_name);

    /*
     * Send our current config register setting over to the slave
     */
    master_config = ~mon_getconfig();
    if (!set_slave_confreg(master_config) && vocal)
	    printf("\n%% Cannot communicate config register to slave");
}
/*
 * Add registry functions used by common routines to copy
 * a master config to slave, whenever we copy config to
 * startup-config file.
 */
static void slave_cfg_init (void)
{
    reg_add_open_slave_config(open_slave_config, "open_slave_config");
    reg_add_close_slave_config(close_slave_config, "close_slave_config");
    reg_add_write_slave_config(write_slave_config, "write_slave_config");
    reg_add_sync_slave_config(sync_slave_config, "sync_slave_config");
    reg_add_sync_slave_private_config(sync_slave_private_config, 
			      "sync_slave_private_config");
    reg_add_set_slave_monvar(auto_set_slave_monvar, "auto_set_slave_monvar");
}


/*
 * Do the necessary IPC initialisation to allow both the master and
 * slave to handshake upon booting (known as the slave sign-in).
 * The signin message is a simple
 * one-shot message from the slave to the master, saying ....
 * "Hi, I'm here, and here is my version string".
 * It is the receipt of this message that promotes the slave_state
 * to SLAVE_RUNNING.  What follows is the master implementation of
 * this initialisation.  The slave also provides a routine of the
 * same name to perform the slave side of this initialisation.
 * Also set up some registries.
 */
void
rsp_slave_init (void)
{
    static ipc_port_id signin_port;
    ipc_error_code ec;
    

    reg_add_print_slave_info(print_slave_info, "print_slave_info");
    reg_add_show_slave_boot(show_slave_boot, "show_slave_boot");
    reg_add_slave_stack_whyreload(slave_stack_whyreload, 
				  "slave_stack_whyreload");
    reg_add_write_slave_calendar(write_slave_calendar, "write_slave_calendar");

    slave_parser_init();
    slave_cfg_init();

    /*
     * Spawn the process that will periodically send the current time
     * over to the slave.
     */
    process_create(set_slave_time, "Slave Time", SMALL_STACK, PRIO_LOW);

    /*
     * While running on the master, we are the 'server'.  We need
     * to create a named port, set up the receive callback, and
     * register the port.  In the master  environment, this can
     * all safely be done here, since IPC is already up and
     * running.
     */
    ec = ipc_create_named_port(SIGNIN_PORT_NAME, &signin_port, 0);
    if (ec != IPC_OK) {
	errmsg(&msgsym(IPC, RSP), "master could not create named port",
	       ipc_decode_error(ec));
        return;
    }

    ec = ipc_set_receive_callback(signin_port, 0, slave_signin_handler);
    if (ec != IPC_OK) {
	errmsg(&msgsym(IPC, RSP), "master could not set callback",
	       ipc_decode_error(ec));
	return;
    }

    ec = ipc_register_port(signin_port);
    if (ec != IPC_OK) {
	errmsg(&msgsym(IPC, RSP), "master could not register port",
	       ipc_decode_error(ec));
        return;
    }
}
