/* $Id: ipc_slave_apps.c,v 3.3.8.3 1996/08/28 12:50:50 thille Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc_slave_apps.c,v $
 *------------------------------------------------------------------
 * ipc_slave_apps.c - IPC Slave applications
 * Slave core dump
 * Slave logger
 *
 * May 1995 Don Lobete
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_slave_apps.c,v $
 * Revision 3.3.8.3  1996/08/28  12:50:50  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.8.2  1996/06/10  04:52:03  dlobete
 * CSCdi45148:  Error messages on VIP not reported on RP/RSP
 * Branch: California_branch
 *   - Use a more efficient scheme for slave-log port-name generation at
 *           both RP/RSP and VIP
 *   - Pull slave application details out of IPC CORE files
 *
 * Revision 3.3.8.1  1996/03/18  20:18:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.4  1996/03/16  07:00:38  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.3  1996/03/09  05:06:50  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.2  1996/03/07  09:47:39  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  14:26:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/29  05:22:32  foster
 * CSCdi47230:  HSA slave should come up after crash
 *
 * Revision 3.2  1995/11/17  17:28:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:05:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/13  00:13:01  dlobete
 * CSCdi34151:  UDP transport non-operational
 *
 * Revision 2.1  1995/07/12  22:27:08  dlobete
 * CSCdi34149:  Cleanup slave applications
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "file.h"
#include "../ip/ip.h"
#include "../ip/tftp.h"
#include "logger.h"
#include "../ipc/msg_ipc.c"		/* Not a typo, see logger.h */
#include "exception.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ipc/ipc_private.h"
#include "../ipc/ipc_debug.h"
#include "../ipc/ipc_slave_apps.h"
#include "../ip/ip_registry.h"

#include "parser.h"
#include "../parser/parser_defs_misc.h"

extern file_access_protocol sl_exception_dumpproto;


static void ipc_slavelog_callback_routine (ipc_message *message,
                               void *context,
                               ipc_error_code error)
{
    uchar *string_ptr;
    int slot_number;
    int i;
    uchar *slavelog_port_name;

    if (!slavelog_enable) {
        ipc_return_message(message);
        return;
    }

    string_ptr = context;

    /* 
     * Which log port/slot did this message come in on ?
     */
    slot_number = -1;
    for(i = 0; i <  MAX_SLAVE_SLOTS; i++) {

	slavelog_port_name = generate_slavelog_port_name(i);

	if (strcmp(string_ptr,slavelog_port_name) == 0) {
	    slot_number = i;
	    break;
	}
    }

    /*
     * Something disturbing happened.
     */
    if (slot_number == -1) {
        if (ipc_debug_ports)
	    buginf("Slave log callback. (response=%s)\n", 
		   ipc_decode_error(error));
	ipc_return_message(message);
	return;
    }

    string_ptr = malloc(message->header->size + IPC_SLOT_STR_SIZE);
    sprintf(string_ptr,"%s%d:\n %s", IPC_SLOT_STR, slot_number, 
	                       (uchar *)message->data + 1); /* jump the % */
    errmsg(&msgsym(SLAVELOG,IPC), string_ptr);
    free(string_ptr);
 
    ipc_return_message(message);

}

void ipc_open_slave_log_ports (void)
{
    ipc_port_id slavelog_port;
    ipc_error_code error;
    int i;
    uchar *slavelog_port_name;
 
    /*
     * Open up each log port.
     */
    for(i = 0; i < MAX_SLAVE_SLOTS; i++) {
	slavelog_port_name = generate_slavelog_port_name(i);
	if (!slavelog_port_name)
	    continue;

	error = ipc_create_named_port(slavelog_port_name,
						 &slavelog_port, IPC_PORT_OOB);
	if (error != IPC_OK) {
	    if (ipc_debug_ports)
		buginf("FAIL. (response=%s)\n", ipc_decode_error(error));
	} else {  /* setup call back routine */
	    error = ipc_set_receive_callback(slavelog_port,
					     (void *)slavelog_port_name, 
					     ipc_slavelog_callback_routine);
	    if (error != IPC_OK) {
		if (ipc_debug_ports)
		    buginf("FAIL.\n");
	    }
	}
    }
}

void ipc_close_slave_log_ports (void)
{
    ipc_port_id slavelog_port;
    ipc_error_code error;
    int i;
    uchar *slavelog_port_name;
 
    /*
     * Open up each log port.
     */
    for(i = 0; i < MAX_SLAVE_SLOTS; i++) {
        slavelog_port_name = generate_slavelog_port_name(i);
	        if (!slavelog_port_name)
            continue;

	if ( (slavelog_port = 
	      ipc_locate_port(slavelog_port_name))) {
	    error = ipc_close_port(slavelog_port);
	    if (error != IPC_OK)
		if (ipc_debug_ports)
		    buginf("ipc_close_port() FAIL. (response=%s)\n",
			   ipc_decode_error(error));
	}
    }
}

/*
 * Only allow one core dump at a time.
 */
static void ipc_slavecore_callback_routine(ipc_message *message,
                               void *context,
                               ipc_error_code error)
{
    uchar *string_ptr;
    int slot_number;
    static int current_slot_doing_dump=-1;
    static tftp_transaction_failed=FALSE;

    if (!slavecore_enable) {
        ipc_return_message(message);
        return;
    }
    string_ptr = context;
 
    /*
     * Which log port/slot did this message come in on ?
     */
    if (strcmp(string_ptr, IPC_SLAVECORE_PORT0_NAME) == 0)
        slot_number = 0;
    else if (strcmp(string_ptr, IPC_SLAVECORE_PORT1_NAME) == 0)
        slot_number = 1;
    else if (strcmp(string_ptr, IPC_SLAVECORE_PORT2_NAME) == 0)
        slot_number = 2;
    else if (strcmp(string_ptr, IPC_SLAVECORE_PORT3_NAME) == 0)
        slot_number = 3;
    else                     /* IPC_SLAVECORE_PORT4_NAME */
        slot_number = 4;

    if (current_slot_doing_dump < 0) {
	current_slot_doing_dump = slot_number;
    	if (strcmp(IPC_STARTUP_MESSAGE, message->data) == 0) {
 
	printf("\nOpening TFTP socket for slot %d ...\n", slot_number);
 
            if (!reg_invoke_slave_core_dump(sl_exception_dumpproto, 
					message->data,
           		                message->header->size,
                       	    		sl_dumpfile, sl_dumphost.ip_addr,
                                	IPC_SLAVECORE_TFTP_OPEN)) {
                printf("Can't open slave core socket for slot %d\n", 
					slot_number);
		tftp_transaction_failed = TRUE;
	    }
        }
	ipc_return_message(message);
        return;
    } else {
	if (slot_number != current_slot_doing_dump) {
	    printf("Attempt by slot %d to dump core ignored; current slot %d\n",
			current_slot_doing_dump, slot_number);
	    ipc_return_message(message);
	    return;
	}
    }

    /*
     * Chop up data into TFTP-palatable sizes (512 bytes)
     * Check if the TFTP open or previous write operations failed -
     * if they did the just bounce the message back to IPC.
     */

    if (strcmp(IPC_TERMINATOR_MESSAGE, message->data) != 0) {

        if (tftp_transaction_failed) {
	    ipc_return_message(message);
            return;
        }
 
        if (!reg_invoke_slave_core_dump(sl_exception_dumpproto, message->data,
                                message->header->size,
                                sl_dumpfile, sl_dumphost.ip_addr,
                                IPC_SLAVECORE_TFTP_WRITE)) {
            printf("Can't write to slave core socket for slot %d\n", 
								slot_number);
	    tftp_transaction_failed = TRUE;
	}
    } else { /* terminate our network write */
 
         printf("\nClosing TFTP socket for slot %d ...\n", slot_number);
        if (!reg_invoke_slave_core_dump(sl_exception_dumpproto, message->data,
                                message->header->size,
                                sl_dumpfile, sl_dumphost.ip_addr,
                                IPC_SLAVECORE_TFTP_CLOSE)) {
            printf("Can't close slave core socket for slot %d\n", slot_number);
	}
	current_slot_doing_dump = -1; 
	tftp_transaction_failed = FALSE;
    }
    ipc_return_message(message);

}

void ipc_open_slave_core_ports(void)
{
    ipc_port_id slavecore_port0, slavecore_port1, slavecore_port2,
                slavecore_port3, slavecore_port4;
    ipc_error_code error;

    /*
     * Open up each log port. Do something smarter here;
     * key off the number of slots in the box and maybe initialize
     * an array of strings which is processed in this routine.
     */
    error = ipc_create_named_port(IPC_SLAVECORE_PORT0_NAME, 
					     &slavecore_port0,	IPC_PORT_OOB);
    if (error != IPC_OK) {
	if (ipc_debug_ports)
            buginf("FAIL. (response=%s)\n", ipc_decode_error(error));
    } else {  /* set callback routine */
	error = ipc_set_receive_callback(slavecore_port0, 
				(void *)IPC_SLAVECORE_PORT0_NAME, 
				ipc_slavecore_callback_routine);
	if (error != IPC_OK) {
	    if (ipc_debug_ports)
		buginf("FAIL.\n");
	}
    }

    error = ipc_create_named_port(IPC_SLAVECORE_PORT1_NAME,
					     &slavecore_port1, IPC_PORT_OOB);
    if (error != IPC_OK) {
	if (ipc_debug_ports)
            buginf("FAIL. (response=%s)\n", ipc_decode_error(error));
    } else {  /* set callback routine */
	error = ipc_set_receive_callback(slavecore_port1, 
		(void *)IPC_SLAVECORE_PORT1_NAME, 
                ipc_slavecore_callback_routine);
	if (error != IPC_OK) {
	    if (ipc_debug_ports)
		buginf("FAIL.\n");
	}
    }

    error = ipc_create_named_port(IPC_SLAVECORE_PORT2_NAME,
					     &slavecore_port2, IPC_PORT_OOB);
    if (error != IPC_OK) {
	if (ipc_debug_ports)
            buginf("FAIL. (response=%s)\n", ipc_decode_error(error));
    } else {  /* set callback routine */
	error = ipc_set_receive_callback(slavecore_port2, 
		(void *)IPC_SLAVECORE_PORT2_NAME, 
		 ipc_slavecore_callback_routine);
	if (error != IPC_OK) {
	    if (ipc_debug_ports)
		buginf("FAIL.\n");
	}
    }

    error = ipc_create_named_port(IPC_SLAVECORE_PORT3_NAME, 
					     &slavecore_port3, IPC_PORT_OOB);
    if (error != IPC_OK) {
	if (ipc_debug_ports)
            buginf("FAIL. (response=%s)\n", ipc_decode_error(error));
    } else {  /* set callback routine */
	error = ipc_set_receive_callback(slavecore_port3, 
		(void *)IPC_SLAVECORE_PORT3_NAME, 
				ipc_slavecore_callback_routine);
	if (error != IPC_OK) {
	    if (ipc_debug_ports)
		buginf("FAIL.\n");
	}
    }

    error = ipc_create_named_port(IPC_SLAVECORE_PORT4_NAME, 
					     &slavecore_port4, IPC_PORT_OOB);
    if (error != IPC_OK) {
	if (ipc_debug_ports)
            buginf("FAIL. (response=%s)\n", ipc_decode_error(error));
    } else {  /* set callback routine */
	error = ipc_set_receive_callback(slavecore_port4, 
		       (void *)IPC_SLAVECORE_PORT4_NAME, 
		      	ipc_slavecore_callback_routine);
	if (error != IPC_OK) {
	    if (ipc_debug_ports)
		buginf("FAIL.\n");
	}
    }
}

void ipc_close_slave_core_ports(void)
{
    ipc_port_id slavecore_port0, slavecore_port1, slavecore_port2,
                slavecore_port3, slavecore_port4;
    ipc_error_code error;
 
    if ( (slavecore_port0 = ipc_locate_port(IPC_SLAVECORE_PORT0_NAME))) {
    	error = ipc_close_port(slavecore_port0);
    	if (error != IPC_OK)
	    if (ipc_debug_ports)
        	buginf("ipc_close_port() FAIL. (response=%s)\n",
        	ipc_decode_error(error));
    }
 
    if ((slavecore_port1 = ipc_locate_port(IPC_SLAVECORE_PORT1_NAME))) {
    	error = ipc_close_port(slavecore_port1);
    	if (error != IPC_OK)
	    if (ipc_debug_ports)
        	buginf("ipc_close_port() FAIL. (response=%s)\n",
        	ipc_decode_error(error));
    }

    if ((slavecore_port2 = ipc_locate_port(IPC_SLAVECORE_PORT2_NAME))) {
    	error = ipc_close_port(slavecore_port2);
    	if (error != IPC_OK)
	    if (ipc_debug_ports)
        	buginf("ipc_close_port() FAIL. (response=%s)\n",
        	ipc_decode_error(error));
    }

    if ((slavecore_port3 = ipc_locate_port(IPC_SLAVECORE_PORT3_NAME))) {
    	error = ipc_close_port(slavecore_port3);
    	if (error != IPC_OK)
	    if (ipc_debug_ports)
        	buginf("ipc_close_port() FAIL. (response=%s)\n",
        	ipc_decode_error(error));
    }

    if ((slavecore_port4 = ipc_locate_port(IPC_SLAVECORE_PORT4_NAME))) {
    	error = ipc_close_port(slavecore_port4);
    	if (error != IPC_OK)
	    if (ipc_debug_ports)
        	buginf("ipc_close_port() FAIL. (response=%s)\n",
        	ipc_decode_error(error));
    }
}
