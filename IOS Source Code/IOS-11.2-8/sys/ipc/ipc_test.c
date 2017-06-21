/* $Id: ipc_test.c,v 3.7.20.5 1996/08/28 12:50:52 thille Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc_test.c,v $
 *------------------------------------------------------------------
 * ipc_test.c - Test routines for IPC system.
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_test.c,v $
 * Revision 3.7.20.5  1996/08/28  12:50:52  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.20.4  1996/06/28  23:18:20  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.7.20.3  1996/06/10  04:52:06  dlobete
 * CSCdi45148:  Error messages on VIP not reported on RP/RSP
 * Branch: California_branch
 *   - Use a more efficient scheme for slave-log port-name generation at
 *           both RP/RSP and VIP
 *   - Pull slave application details out of IPC CORE files
 *
 * Revision 3.7.20.2  1996/05/21  09:52:08  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.20.1  1996/03/18  20:18:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.2  1996/03/07  09:47:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  01:01:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/01  06:04:39  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/29  07:28:39  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1996/01/19  01:00:58  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/12/17  18:30:11  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  17:28:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:41:41  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:00:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:05:46  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/17  20:51:47  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.1  1995/06/07  21:02:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "logger.h"
#include "../ipc/msg_ipc.c"		/* Not a typo, see logger.h */
#include "registry.h"
#include "sys_registry.h"
#include "../os/free.h"
#include "../os/boot.h"
#include "parser.h"
#include "../hes/parser_defs_fci.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

#include "ttysrv.h"

#include "ipc_util.h"
#include "ipc.h"
#include "ipc_private.h"
#include "ipc_seats.h"
#include "ipc_cbus.h"
#include "ipc_test.h"
#include "ipc_slave_apps.h"

#include "../ui/debug.h"
#include "ipc_debug.h"

const char test_message[] =
	"When uncertain or in doubt: Run in circles! Scream and shout!";
static const unsigned int test_context = 0x12345678;

static ipc_seat slot_seat_array[] = {
SLOT_0_SEAT,
SLOT_1_SEAT,
SLOT_2_SEAT,
SLOT_3_SEAT,
SLOT_4_SEAT };


static void ipc_test_callback_routine(ipc_message *message,
			       void *context,
			       ipc_error_code error)
{

    printf("    Data arrived using callback from IPC system: %s.\n",
	   ipc_decode_error(error));
    
    if (strcmp(test_message, message->data) == 0)
	printf("    Message data is correct.\n");
    else 
	printf("    Message data DOES NOT MATCH what we sent.\n");

    if (test_context == (int)context)
	printf("    Callback context is correct.\n");
    else
	printf("    Callback context is set improperly.\n");

    reg_invoke_ipc_return_message(message);
    return;
}


static ipc_port_id ipc_get_test_port (void)
{
    ipc_error_code error, dup_port;
    ipc_port_id test_port;
    ipc_port_id ping_port;

    char test_port_name[] = "Test:Local Test Port";

    error = reg_invoke_ipc_create_named_port(test_port_name, &test_port, IPC_PORT_OOB);

    printf("Create named port... ");

    if (error == IPC_OK)
	printf("PASS.\n");
    else
	printf("FAIL. (response=%s)\n", ipc_decode_error(error));

    error = reg_invoke_ipc_create_named_port(test_port_name, &dup_port, IPC_PORT_OOB);

    printf("Create port again... ");

    if (error == IPC_OK)
	printf("FAIL.\n");
    else
	printf("PASS. (response=%s)\n", ipc_decode_error(error));

    ping_port = reg_invoke_ipc_locate_port(test_port_name);

    printf("Locate port... ");

    if (ping_port != 0) 
	printf("PASS.\n");
    else
	printf("FAIL. (response=%s)\n", ipc_decode_error(error));

    return ping_port;
}

static void ipc_close_test_port (ipc_port_id ping_port)
{
    ipc_error_code error;
    error = reg_invoke_ipc_close_port(ping_port);

    printf("Close message port... ");

    if (error == IPC_OK)
	printf("PASS.\n");
    else
	printf("FAIL. (response=%s)\n", ipc_decode_error(error));

    error = reg_invoke_ipc_close_port(ping_port);

    printf("Close message port again... ");

    if (error == IPC_OK)
	printf("FAIL.\n");
    else
	printf("PASS. (response=%s)\n", ipc_decode_error(error));
}

void ipc_test_api(parseinfo *csb)
{
    ipc_port_id  test_port;
    ipc_message *message;


    printf("\nTesting some IPC API calls:\n\n");

    test_port = ipc_get_test_port();

    /*
     * Allocate and return a message
     */

    message = reg_invoke_ipc_get_message(strlen(test_message)+1, test_port,
			     IPC_TYPE_SERVER_ECHO);

    printf("Create empty message... ");
    if (message != NULL)
	printf("PASS.\n");
    else
	printf("FAIL.\n");

    reg_invoke_ipc_return_message(message);

    ipc_close_test_port(test_port);
}

/*
 * Perform continuous rpc loopback test.
 */
static void ipc_test_rpc_forever (ipc_port_id test_port)
{
    ipc_message *message, *response;
    ipc_error_code error;
    int throw_away;
    int tcount = 0;
    int xcount = 0;
    int count = 0;

    /*
    ulong elapsed;
    usec_timestamp start;
    ulong max_elapsed = 0;
    */

    psion(stdio);

    while (psipending() == FALSE) {

	do
	    throw_away = getc(stdio);
	while (throw_away != -1);

	flush();
	process_may_suspend();

	tcount++;

	/** USEC_GET_TIMESTAMP(start); **/

	/*
	 * Allocate our test message
	 */
	message = reg_invoke_ipc_get_message(strlen(test_message)+1, test_port,
	    IPC_TYPE_SERVER_ECHO);
	if (message == NULL) {
	    printf("M");
	    continue;
	}

	strcpy(message->data, test_message);
	response = reg_invoke_ipc_send_rpc(message, &error);
	xcount++;

	/*
	elapsed = USEC_ELAPSED_TIME(start);
	if (max_elapsed < elapsed)
	    max_elapsed = elapsed;
	*/

	if (error != IPC_OK) {
	    printf("S");
	    reg_invoke_ipc_return_message(message);
	    continue;
	}

	if (response == NULL) {
	    printf("R");
	    continue;
	}

	if (strcmp(test_message, response->data) == 0) {
	    printf("!");
	    count++;
	} else 
	    printf("D");
	reg_invoke_ipc_return_message(response);

    }

    psioff(stdio);

    printf("\n");
    printf("Total loop count: %d\n", tcount);
    printf("Total send count: %d\n", xcount);
    printf("Total success count: %d\n", count);
    /** printf("Maximum RPC elaps time (in usec): %d\n", max_elapsed); **/
}

void ipc_test_rpc(parseinfo *csb)
{
    ipc_port_id test_port;
    ipc_message *message, *response;

    ipc_error_code error;

    test_port = reg_invoke_ipc_locate_port("IPC Master:Echo");

    if (GETOBJ(int,1)) {
	ipc_test_rpc_forever(test_port);
	return;
    }

    /*
     * Allocate our test message
     */

    message = reg_invoke_ipc_get_message(strlen(test_message)+1, test_port,
			     IPC_TYPE_SERVER_ECHO);
    printf("Create empty message... ");
    if (message != NULL)
	printf("PASS.\n");
    else
	printf("FAIL.\n");

    strcpy(message->data, test_message);

    printf("Sending message...\n");

    response = reg_invoke_ipc_send_rpc(message, &error);

    if (error != IPC_OK) {
	printf("FAIL (response=%s).\n", ipc_decode_error(error));
	reg_invoke_ipc_return_message(message);
    }

    if (response == NULL) {
	printf("    Reply did not come back!\n");
    } else {
	if (strcmp(test_message, response->data) == 0)
	    printf("    Message data is correct.\n");
	else 
	    printf("    Message data DOES NOT MATCH what we sent.\n");

	reg_invoke_ipc_return_message(response);
    }
}

void ipc_test_queue(parseinfo *csb)
{
    ipc_port_id test_port;
    ipc_message *message;
    ipc_error_code error;
    queuetype test_messageQ;

    queue_init(&test_messageQ, 0);
    test_port = ipc_get_test_port();

    /*
     * Set up the port queue options.
     */

    error = ipc_set_receive_queue(test_port, &test_messageQ);

    printf("Setting up receive queue... ");

    if (error == IPC_OK)
	printf("PASS\n");
    else
	printf("FAIL\n");

    /*
     * Allocate and return a message
     */

    message = reg_invoke_ipc_get_message(strlen(test_message)+1, test_port,
			     IPC_TYPE_SERVER_ECHO);

    printf("Create empty message... ");
    if (message != NULL)
	printf("PASS.\n");
    else
	printf("FAIL.\n");

    strcpy(message->data, test_message);

    printf("Sending message...\n");

    error = reg_invoke_ipc_send_message(message);

    if (error != IPC_OK)
	printf("FAIL (response=%s).\n", ipc_decode_error(error));

    ipc_close_test_port(test_port);

    /* Check the message from the queue. */

    message = dequeue(&test_messageQ);

    if (message == NULL) {
	printf("    Message did not appear on receive queue!\n");
    } else {
	if (strcmp(test_message, message->data) == 0)
	    printf("    Message data is correct.\n");
	else 
	    printf("    Message data DOES NOT MATCH what we sent.\n");
    }
    reg_invoke_ipc_return_message(message);
}

void ipc_test_callback(parseinfo *csb)
{
    ipc_port_id test_port;
    ipc_message *message;
    ipc_error_code error;


    printf("\nTesting message arrival with callbacks.\n\n");

    test_port = ipc_get_test_port();

    /*
     * Set up the port callback options.
     */

    error = reg_invoke_ipc_set_receive_callback(test_port, (void *)test_context,
				     ipc_test_callback_routine);

    printf("Set receive callback... ");

    if (error == IPC_OK)
	printf("PASS.\n");
    else
	printf("FAIL.\n");
	
    /*
     * Transmit a message
     */

    message = reg_invoke_ipc_get_message(strlen(test_message)+1, test_port,
			     IPC_TYPE_SERVER_ECHO);

    printf("Create empty message... ");
    if (message != NULL)
	printf("PASS.\n");
    else
	printf("FAIL.\n");

    printf("Sending message...\n");

    strcpy(message->data, test_message);
    
    error = reg_invoke_ipc_send_message(message);

    printf("    ");

    if (error == IPC_OK) 
	printf("PASS.\n");
    else
	printf("FAIL. (response=%s)\n", ipc_decode_error(error));

    ipc_close_test_port(test_port);

}

#if 0
void ipc_test_misc_receiver(ipc_message *message,
			    void *context,
			    ipc_error_code error)
{
    reg_invoke_ipc_return_message(message);
}

int ipc_test_misc_done;

void ipc_test_misc_callback(ipc_message *message,
			    void *context,
			    ipc_error_code error)
{
    reg_invoke_ipc_return_message(message);
    ipc_test_misc_done=1;
}

void ipc_test_misc(parseinfo *csb)
{
    int attempts;
    ipc_port_id test_port;
    ipc_message *message;

    sys_timestamp begin;
    sys_timestamp end;

    reg_invoke_ipc_create_named_port("Local:Perf Test", &test_port, IPC_PORT_OOB);
    reg_invoke_ipc_set_receive_callback(test_port, NULL, ipc_test_misc_receiver);

    GET_TIMESTAMP(begin);

    for (attempts = 0; attempts < 20000; attempts++) {
	message = reg_invoke_ipc_get_message(10, test_port, 1);
	ipc_test_misc_done = 0;
	reg_invoke_ipc_send_message_async(message, NULL, ipc_test_misc_callback);
	while (!ipc_test_misc_done);
    }

    printf("\n\n20000 messages in %d milliseconds.\n\n",
	   ELAPSED_TIME(begin));

    reg_invoke_ipc_close_port(test_port);
}
#endif

void ipc_test_slavecore(parseinfo *csb)
{

#if defined(IPC_SLAVE_COREDUMP_TEST)

    ipc_port_id slavecore_port=NULL;
    ipc_message *message;
    ipc_error_code error;
    uchar *mem_pointer;
    int j;


    printf("\nTesting slave coredumps via IPC.\n\n");

    /*
     * Transmit some messages as a VIP would ...
     */

    slavecore_port = reg_invoke_ipc_locate_port(IPC_SLAVECORE_PORT0_NAME);

    /* Send the startup message */
    message = reg_invoke_ipc_get_message(sizeof(IPC_STARTUP_MESSAGE) + 1,
                slavecore_port, IPC_TYPE_SERVER_ECHO);
    strcpy(message->data, IPC_STARTUP_MESSAGE);
    error = reg_invoke_ipc_send_message(message);
    if (error != IPC_OK) {
        printf("FAIL. (response=%s)\n", ipc_decode_error(error));
    }


    for (j = 0, mem_pointer = RAMBASE; 
		j < (pmem.memtop - (uchar *)RAMBASE)/TFTP_MAXDATA;
		j++, mem_pointer += TFTP_MAXDATA) {

    	message = reg_invoke_ipc_get_message(TFTP_MAXDATA, slavecore_port, 
				IPC_TYPE_SERVER_ECHO);
    	if (message == NULL) {
		printf("FAIL.\n");
		continue;
	}

	bcopy(mem_pointer, message->data, TFTP_MAXDATA); 

    	error = reg_invoke_ipc_send_message(message);
	if (error != IPC_OK) {
	    printf("FAIL. (response=%s)\n", ipc_decode_error(error));
	    printf(";");
	}
    }
    printf("\n");

    /* Send the terminator message */
    message = reg_invoke_ipc_get_message(sizeof(IPC_TERMINATOR_MESSAGE) + 1, 
		slavecore_port, IPC_TYPE_SERVER_ECHO);
    strcpy(message->data, IPC_TERMINATOR_MESSAGE);
    error = reg_invoke_ipc_send_message(message);
    if (error != IPC_OK) {
        printf("FAIL. (response=%s)\n", ipc_decode_error(error));       
    }

#endif /* IPC_SLAVE_COREDUMP_TEST */

}

void ipc_test_cbustx(parseinfo *csb)
{

    ushort slot=0;
    char test_message[]="abcdefghijklmnopqrstuvwxyz";
    ipc_message *message;
    ipc_error_code error;

    if (csb->sense) {
        slot = GETOBJ(int,1);
    }

    /* Get the Seat Id for this slot if available */
    if (ipc_get_seat(slot_seat_array[slot])) {

	/* Send a message to a cbus slot with a calculated portid */ 
	message = reg_invoke_ipc_get_message(strlen(test_message)+1, 
				slot_seat_array[slot] + 10,
                             	(ipc_message_type) 0);
    	if (message == NULL)
            buginf("ipc_get_message() FAIL.\n");
    	strcpy(message->data, test_message);
    	error = reg_invoke_ipc_send_message(message);

    	if (error != IPC_OK) {
       	    printf("ipc_test_cbustx: FAIL. (response=%s)\n", 
		ipc_decode_error(error));       
    	}
    } else {
	printf("No support for IPC in slot %d", slot);
    }
}


static void ipc_send_fake_log_msg(int slot)
{
    ipc_port_id test_port;
    ipc_message *message;
    ipc_error_code error;
    char test_message[] = "Log some message";
    char *port_name=NULL;
	
    switch(slot) {
	case 0:
	    port_name = IPC_SLAVELOG_PORT0_NAME;
	    break;
	case 1:
	    port_name = IPC_SLAVELOG_PORT1_NAME;
	    break;
	case 2:
	    port_name = IPC_SLAVELOG_PORT2_NAME;
	    break;
	case 3:
	    port_name = IPC_SLAVELOG_PORT3_NAME;
	    break;
	case 4:
	    port_name = IPC_SLAVELOG_PORT4_NAME;
	    break;
    }

    printf("Attempting to send log message from slot %d.\n", slot);

    test_port = reg_invoke_ipc_locate_port(port_name);
    if (test_port == (ipc_port_id) 0) {
	buginf("No IPC port open for slot %d!\n", slot);
	return;
    }
    message = reg_invoke_ipc_get_message(strlen(test_message)+1, test_port,
                             0);
    if (message == NULL)
        buginf("ipc_get_message() FAIL.\n");
    strcpy(message->data, test_message);
    error = reg_invoke_ipc_send_message(message);
    if (error != IPC_OK)
        buginf("ipc_send_message() FAIL. (response=%s)\n", 
			ipc_decode_error(error));

}
void ipc_test_slavelog(parseinfo *csb)
{

    printf("\nTesting slave logging via IPC.\n\n");
 
    /*
     * Transmit some fake log messages
     */

    ipc_send_fake_log_msg(0);
    ipc_send_fake_log_msg(1);
    ipc_send_fake_log_msg(2);
    ipc_send_fake_log_msg(3);
    ipc_send_fake_log_msg(4);
    ipc_send_fake_log_msg(4);
    ipc_send_fake_log_msg(3);
    ipc_send_fake_log_msg(2);
    ipc_send_fake_log_msg(1);
    ipc_send_fake_log_msg(0);
}

void ipc_test_misc(parseinfo *csb)
{
#if 0
    static ipc_port_id test_port=0;

    if (!test_port)
    	reg_invoke_ipc_create_named_port("Test:Register", &test_port, 
					 IPC_PORT_OOB);
    reg_invoke_ipc_register_port(test_port);
#endif

#if 0
    ipc_send_fake_log_msg(2);
#endif

    errmsg(&msgsym(NOMEM,IPC),".. something but I can't remember what.");
}
