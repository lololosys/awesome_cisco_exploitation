/* $Id: chipcom_mbox.c,v 3.4.4.4 1996/08/28 13:05:17 thille Exp $
 * $Source: /release/112/cvs/Xsys/partner/chipcom_mbox.c,v $
 *------------------------------------------------------------------
 * chipcom_mbox.c -- Chipcom Mailbox Functions
 *
 * July 1994, John G. Kuzma
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The following file contains those functions used to implement the 
 * Chipcom Mailbox Protocol, as documented in Chipcom P/N 97-00033.
 *------------------------------------------------------------------
 * $Log: chipcom_mbox.c,v $
 * Revision 3.4.4.4  1996/08/28  13:05:17  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.4.3  1996/08/03  23:35:51  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.4.4.2  1996/05/23  07:38:06  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.4.4.1  1996/03/18  21:33:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/16  07:23:12  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.2  1996/03/07  10:40:29  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  16:49:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/03/08  23:38:17  ljiang
 * CSCdi50986:  Add mailbox support for Microcom Terminator(c2511
 * derivative)
 *
 * Revision 3.3  1995/11/17  18:53:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:20:42  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:56:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/19  23:13:48  cnee
 * CSCdi40675:  no ip address notifications not sent for partners mailbox
 *
 * Revision 2.2  1995/06/09  13:12:44  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:32:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ctype.h>
#include <ciscolib.h>
#include "subsys.h"
#include "logger.h"
#include "../les/msg_mailbox.c"		/* Not a typo, see logger.h */
#include "interface_private.h"
#include "ieee.h"
#include "version.h"
#include "../ui/debug.h"
#include "../if/ether.h"
#include "../ip/ip_registry.h"
#include "../os/gdb.h"
#include "../../boot/cpu.h"
#include "../les/mailbox_debug.h"
#include "partner_registry.h"
#include "chipcom_mbox.h"


/* Shared global variables. */
int outgoing_mbox_count    = 0;
int incoming_mbox_state = MAILBOX_STATE__RESET;
int outgoing_mbox_state = MAILBOX_STATE__RESET;
char_queue_t *incoming_mbox_queue = NULL;
char_queue_t *outgoing_mbox_queue = NULL;

/* "Local" global variables. */
static int mailbox_recv_hndlr_pid = 0;
static int mailbox_send_hndlr_pid = 0;
static boolean echo_resp_pending = FALSE;
static boolean echo_rqst_pending = FALSE;
static boolean echo_resp_valid   = FALSE;
static uchar incoming_msg_buffer[MAX_MAILBOX_PDU_SIZE];
static uchar outgoing_msg_buffer[MAX_MAILBOX_PDU_SIZE];
static uchar outgoing_echo_rqst_data[MAX_MAILBOX_PDU_SIZE];



/******************************************************************************
 Function: void mailbox_v2_early_init_common(void)
 
 Descr   : This function is called by the platform-specific initialization
           function to perform the common initialization functions, such as
           starting the process handlers, creating the queues, etc.
 ******************************************************************************/
void mailbox_v2_early_init_common (void)
{

    /* Enable the gdb breakpoint by uncommenting the next two lines. */
    /*
    gdb_kernel_enable();
    breakpoint();
    */


    /* Set up the debugging flags. */
    mailbox_debug_init();

    /* Create the input and output queues. */
    if ((incoming_mbox_queue = cqcreate(MAILBOX_QUEUE_SIZE)) == NULL ||
        (outgoing_mbox_queue = cqcreate(MAILBOX_QUEUE_SIZE)) == NULL) {
        errmsg(&msgsym(INITFAIL,MAILBOX),"Cannot create queues.");
        mailbox_v2_quiesce(MAILBOX_QUIESCE_FROM_ELSEWHERE);
        process_kill(THIS_PROCESS);
    }
    /* Start the send & recv handlers. */
    mailbox_recv_hndlr_pid = cfork(mailbox_v2_inbox_hndlr,0L,500,
                                   "Incoming Mailbox",CONSOLE_LINENUM);
    mailbox_send_hndlr_pid = cfork(mailbox_v2_outbox_hndlr,0L,500,
                                   "Outgoing Mailbox",CONSOLE_LINENUM);
    return;
}



/******************************************************************************
 Function: void mailbox_v2_init_common (ushort mbox_capabilities)
 
 Descr   : This function is called by the platform-specific initialization 
           function to perform the bulk of the mailbox initialization. 
 
           The steps performed for initialization may be summarized as follows:
                    1) Read InBox
                    2) Be sure Chipcom has read OutBox
                    3) Send ECHO_REQUEST
                    4) Get ECHO_RESPONSE and validate it.
                    5) Send PERSONALITY_PDU 
                    6) Install callbacks

           All exceptions will result in both mailbox 'state' flags being set
           to 'OFFLINE', mailbox interrupts will be disabled, and the send and
           receive handlers will be killed.

           Finally, note that the 'mbox_capabilities' field is used to provide
           the capabilities mask for building the PERSONALITY PDU. Providing
           this information via a parameter allows a generic function to be used
           for both Cortez and Margarita.
 ******************************************************************************/
void mailbox_v2_init_common (ushort mbox_capabilities)
{
    sys_timestamp timer;

    /* Wait for Chipcom to read their InBox. */
    TIMER_START(timer,MAILBOX_INIT_TIMEOUT * ONESEC);
    while (SLEEPING(timer) && (
            outgoing_mbox_state != MAILBOX_STATE__ONLINE || 
            incoming_mbox_state != MAILBOX_STATE__ONLINE)) {
        process_sleep_periodic(MAILBOX_INIT_DELAY);
    }

    /* Whoa! We timed-out on initialization. Turn the mailbox off. */
    if (outgoing_mbox_state != MAILBOX_STATE__ONLINE || 
        incoming_mbox_state != MAILBOX_STATE__ONLINE) {
        errmsg(&msgsym(INITFAIL,MAILBOX),
               "timed-out on mailbox initialization.");
        mailbox_v2_quiesce(MAILBOX_QUIESCE_FROM_ELSEWHERE);
        return;
    }


    /*
     * Enqueue the ECHO_REQUEST.
     */ 
    mailbox_v2_build_PDU(MAILBOX_PDU__ECHO_RQST,MAILBOX_INIT_ECHO_RQST,strlen(MAILBOX_INIT_ECHO_RQST),0);
    strcpy(outgoing_echo_rqst_data,MAILBOX_INIT_ECHO_RESP);
    cenqueue_PDU(outgoing_mbox_queue,outgoing_msg_buffer);
    echo_rqst_pending = TRUE;

    /* Wait for the ECHO_RQST to get sent. */
    TIMER_START(timer,MAILBOX_INIT_TIMEOUT * ONESEC * 5);
    while (SLEEPING(timer) && echo_rqst_pending) {
        process_sleep_periodic(MAILBOX_INIT_DELAY);
    }

    /* Whoa! The ECHO_RQST never made it! */
    if (echo_rqst_pending) {
        errmsg(&msgsym(INITFAIL,MAILBOX),"Echo response is not sent!!");
        mailbox_v2_quiesce(MAILBOX_QUIESCE_FROM_ELSEWHERE);
        return;
    }


    /*
     * Wait for the ECHO_RESPONSE. 
     */
    TIMER_START(timer,MAILBOX_ECHO_TIMEOUT * ONESEC * 5);
    while (SLEEPING(timer) && echo_resp_pending) {
        process_sleep_periodic(MAILBOX_INIT_DELAY);
    }

    /* Be sure we got a good echo-response. */
    if (echo_resp_pending || echo_resp_valid == FALSE) {
        if (echo_resp_pending) {
            errmsg(&msgsym(NOECHO,MAILBOX));
        }
        mailbox_v2_quiesce(MAILBOX_QUIESCE_FROM_ELSEWHERE);
        return;
    }
    

    /* Send the personality PDU. */
    mailbox_v2_build_PDU(MAILBOX_PDU__PERSONALITY_N,NULL,0,mbox_capabilities);
    cenqueue_PDU(outgoing_mbox_queue,outgoing_msg_buffer);

    /* Initialization has completed! */
    errmsg(&msgsym(INITOK,MAILBOX));
    return;
}



/******************************************************************************
 Function: void mailbox_v2_outbox_hndlr(void)
 
 Descr   : This function will run forever. It is designed to sleep until the 
           outgoing mailbox queue is non-empty, and then will come alive to 
           ensure the timing between successive bytes of the PDU being sent is 
           followed. This function also initiates the sending of the first byte 
           of the PDU.
 ******************************************************************************/
void mailbox_v2_outbox_hndlr (void)
{
    ushort cout;
    int old_mbox_count,sending_echo;
    sys_timestamp timer;

    process_set_priority(PRIO_LOW);

    if (mailbox_debug) {
        errmsg(&msgsym(MBOXDEBUG,MAILBOX),"Starting outgoing mailbox handler");
    }

    while (1) {
        while (cqempty(outgoing_mbox_queue)) {
            edisms((blockproc *) cqempty,(ulong) outgoing_mbox_queue);
        }
 
        cout = cdequeue(outgoing_mbox_queue);
        old_mbox_count = outgoing_mbox_count = cout + 2;

        /* See if we're sending an ECHO_REQUEST. */
        sending_echo = (*(outgoing_mbox_queue->toq) == MAILBOX_PDU__ECHO_RQST);
            
        if (sending_echo) {
            echo_resp_pending = TRUE;
        }

        /* Start things up by writing the first octet. */

	reg_invoke_mailbox_v2_start_output(cout);

        TIMER_START(timer,MAILBOX_INTEROCTET_TIMEOUT * ONESEC);
        while (outgoing_mbox_count) {
            if (outgoing_mbox_count == old_mbox_count) {
                if (AWAKE(timer)) {
                    errmsg(&msgsym(TIMEOUT,MAILBOX),"outgoing");
                    TIMER_START(timer,MAILBOX_INTEROCTET_TIMEOUT * ONESEC);
                }
                process_sleep_periodic(MAILBOX_INTEROCTET_DELAY);
            } else {
                TIMER_START(timer,MAILBOX_INTEROCTET_TIMEOUT * ONESEC);
                old_mbox_count = outgoing_mbox_count;
            }
        }
     
        if (sending_echo) {
            echo_rqst_pending = FALSE;
        }
        if (mailbox_debug) {
            errmsg(&msgsym(MBOXDEBUG,MAILBOX),"Finished sending PDU.");
        }

        /* Give up the processor. */
        process_suspend();

    }
    return;
}



/******************************************************************************
 Function: void mailbox_v2_inbox_hldlr(void)
 
 Descr   : This function will run forever, periodically checking the 
           incoming_mbox_queue to see if any data exists. When data is detected,
           the entire PDU is read into 'incoming_msg_buffer" and then processed 
           accordingly.
 ******************************************************************************/
void mailbox_v2_inbox_hndlr (void)
{
    uint i,count;
    sys_timestamp timer;

    process_set_priority(PRIO_LOW);

    if (mailbox_debug) {
        errmsg(&msgsym(MBOXDEBUG,MAILBOX),"Starting incoming mailbox handler");
    }

    while (1) {
        /* Wait until there's some work to do. */
        while (cqempty(incoming_mbox_queue)) {
            edisms((blockproc *) cqempty,(ulong) incoming_mbox_queue);
        }
  
        /* Get the length. */
        incoming_msg_buffer[0] = cdequeue(incoming_mbox_queue);
        count = (uint) incoming_msg_buffer[0] + MAILBOX_PDU_HEADER_BYTES;
        i = 1;

        while (i < count) {
            TIMER_START(timer,MAILBOX_INTEROCTET_TIMEOUT * ONESEC);
            while ((i < count) && cqempty(incoming_mbox_queue)) {
                /* Did we timeout? */
                if (AWAKE(timer)) {
                    errmsg(&msgsym(TIMEOUT,MAILBOX),"incoming");
                    TIMER_START(timer,MAILBOX_INTEROCTET_TIMEOUT * ONESEC);
                }
                process_sleep_periodic(MAILBOX_INTEROCTET_DELAY);
            }

            incoming_msg_buffer[i++] = cdequeue(incoming_mbox_queue);
        }

        if (mailbox_debug) {
            errmsg(&msgsym(MBOXDEBUG,MAILBOX),"Finished receiving PDU.");
        }
        /* Do something with this PDU. */
        mailbox_v2_process_PDU(incoming_msg_buffer);

        /* Give up the processor. */
        process_suspend();
    }
    return;
}



/******************************************************************************
 Function: void mailbox_v2_build_PDU (int pdu_type, uchar *data, int len,
                                      ushort flags)
 
 Descr   : Build the PDU whose type is specified by the 'pdu_type' parameter 
           into a statically allocated buffer. The 'data' parameter is used to 
           provide data for things like ECHO_REQUEST, ECHO_RESPONSE, etc. The 
           'len' parameter specifies the length of 'data'.
 ******************************************************************************/
void mailbox_v2_build_PDU (int pdu_type, uchar *data, int len, ushort flags)
{
    struct mbox_personality_PDU *p;
    hwidbtype *hw_idb_ptr;
    uchar *c;
    ipaddrtype gw;
    boolean port_type;    

    /* Clear the buffer to make debugging easier. */
    memset(outgoing_msg_buffer, 0,MAX_MAILBOX_PDU_SIZE);

    switch (pdu_type) {
    case MAILBOX_PDU__ECHO_RQST:
        outgoing_msg_buffer[0] = (uchar) len;
        outgoing_msg_buffer[1] = MAILBOX_PDU__ECHO_RQST;
        sstrncpy(&outgoing_msg_buffer[2],data,len+1);
        outgoing_msg_buffer[len+2] = 
          mailbox_v2_compute_checksum(outgoing_msg_buffer,len+2);
        break;
    case MAILBOX_PDU__ECHO_RESP:
        outgoing_msg_buffer[0] = (uchar) len;
        outgoing_msg_buffer[1] = MAILBOX_PDU__ECHO_RESP;
        sstrncpy(&outgoing_msg_buffer[2],data,len+1);
        outgoing_msg_buffer[len+2] = 
          mailbox_v2_compute_checksum(outgoing_msg_buffer,len+2);
        break;

    case MAILBOX_PDU__PERSONALITY_N:
        outgoing_msg_buffer[1] = MAILBOX_PDU__PERSONALITY_N;
        p = (struct mbox_personality_PDU *) outgoing_msg_buffer;
        p->mc = MAILBOX_PDU__PERSONALITY_N;

	c = reg_invoke_mailbox_v2_fill_personality_PDU(p, flags);

        len = outgoing_msg_buffer[0] = c - (outgoing_msg_buffer + 2);
        outgoing_msg_buffer[len+2] = 
          mailbox_v2_compute_checksum(outgoing_msg_buffer,len+2);
        break;

    case MAILBOX_PDU__IP_CONFIG_N:
        outgoing_msg_buffer[1] = MAILBOX_PDU__IP_CONFIG_N;
        c = &outgoing_msg_buffer[2];
        FOR_ALL_HWIDBS(hw_idb_ptr) {
	    port_type = reg_invoke_mailbox_v2_is_complex_port(hw_idb_ptr);

	    if (port_type) {
		memcpy(c, &(hw_idb_ptr->firstsw->ip_address),
		       sizeof(hw_idb_ptr->firstsw->ip_address));
		c+=4;
		memcpy(c, &(hw_idb_ptr->firstsw->ip_nets_mask),
		       sizeof(hw_idb_ptr->firstsw->ip_nets_mask));
		c+=4;
	    }
	}
        outgoing_msg_buffer[0] = len = c - &outgoing_msg_buffer[2];
        outgoing_msg_buffer[len+2] = 
          mailbox_v2_compute_checksum(outgoing_msg_buffer,len+2);
        break;

    case MAILBOX_PDU__MAC_CONFIG_N:
        outgoing_msg_buffer[1] = MAILBOX_PDU__MAC_CONFIG_N;
        c = &outgoing_msg_buffer[2];

        FOR_ALL_HWIDBS(hw_idb_ptr) {
	  port_type = reg_invoke_mailbox_v2_is_complex_port(hw_idb_ptr);

          if (port_type) {
            ieee_copy(hw_idb_ptr->hardware,c);
            c+=IEEEBYTES;
          }
	}
        outgoing_msg_buffer[0] = len = c - &outgoing_msg_buffer[2];
        outgoing_msg_buffer[len+2] = 
          mailbox_v2_compute_checksum(outgoing_msg_buffer,len+2);
        break;

    case MAILBOX_PDU__IP_GATEWAY_N:
        outgoing_msg_buffer[1] = MAILBOX_PDU__IP_GATEWAY_N;
        c = &outgoing_msg_buffer[2];
        gw = reg_invoke_ip_defaultgw();
        memcpy(c, &gw, sizeof(ipaddrtype));
        c += 4;
        outgoing_msg_buffer[0] = len = c - &outgoing_msg_buffer[2];
        outgoing_msg_buffer[len+2] = 
          mailbox_v2_compute_checksum(outgoing_msg_buffer,len+2);
        break;

    case MAILBOX_PDU__OP_STATUS_N:
    case MAILBOX_PDU__SPEED_CONFIG_N:
    case MAILBOX_PDU__DATE_TIME_N:
    case MAILBOX_PDU__NET_CONFIG_N:
    case MAILBOX_PDU__RMT_CMD_RQST:
    case MAILBOX_PDU__RMT_CMD_RESP:
    case MAILBOX_PDU__ADMIN_STATE_N:
        /* Unsupported PDUs. We'll never be called to build these. */
        break;
    }

    return;
}



/******************************************************************************
 Function: void mailbox_v2_process_PDU(uchar *buffer)
 
 Descr   : The following function will examine the PDU passed in the 'buffer' 
           parm and take whatever actions are appropriate. This might be 
           something like changing an interfaces IP address, changing an 
           interfaces state, responding to an echo request, etc. Note that 
           checksum checking is also performed in this function.
 ******************************************************************************/
void mailbox_v2_process_PDU (uchar *buffer)
{
    uchar chksum;
    uint  len;


    len = buffer[0] + 2;
    /* Check the checksum. */
    if ((chksum = mailbox_v2_compute_checksum(buffer,len)) != buffer[len]) {
        errmsg(&msgsym(BADCHKSUM,MAILBOX),buffer[len],chksum);
        return;
    }
    if (mailbox_debug) {
        errmsg(&msgsym(MBOXDEBUG,MAILBOX),"Checksum OK.");
    }
    


    /* Determine what to do with this PDU. */
    switch (buffer[1]) {
    case MAILBOX_PDU__ECHO_RQST:
        if (mailbox_debug) {
            errmsg(&msgsym(MBOXDEBUG,MAILBOX),"Received PDU; type = Echo_Request");
        }
        mailbox_v2_build_PDU(MAILBOX_PDU__ECHO_RESP,&buffer[2],buffer[0],0);
        cenqueue_PDU(outgoing_mbox_queue,outgoing_msg_buffer);
        break;

    case MAILBOX_PDU__ECHO_RESP:
        if (mailbox_debug) {
            errmsg(&msgsym(MBOXDEBUG,MAILBOX),"Received PDU; type = Echo_Response");
        }
        echo_resp_pending = FALSE;   /* "Cancel" the timeout. */
        len = buffer[0];
        if (mailbox_v2_validate_echo_resp(&buffer[2],len)) {
            errmsg(&msgsym(BADECHO,MAILBOX));
            echo_resp_valid = FALSE;
        } else {
            if (mailbox_debug) {
                errmsg(&msgsym(MBOXDEBUG,MAILBOX),"Echo response match.");
            }
            echo_resp_valid = TRUE;
        }
        break;

    case MAILBOX_PDU__ADMIN_STATE_N:
        errmsg(&msgsym(BADPDU,MAILBOX),"Admin_State_N");
        break;
    case MAILBOX_PDU__NET_CONFIG_N:
        errmsg(&msgsym(BADPDU,MAILBOX),"Net_Config_N");
        break;
    case MAILBOX_PDU__SPEED_CONFIG_N:
        errmsg(&msgsym(BADPDU,MAILBOX),"Speed_Config_N");
        break;
    case MAILBOX_PDU__IP_CONFIG_N:
        errmsg(&msgsym(BADPDU,MAILBOX),"IP_Config_N");
        break;
    case MAILBOX_PDU__MAC_CONFIG_N:
        errmsg(&msgsym(BADPDU,MAILBOX),"MAC_Config_N");
        break;
    case MAILBOX_PDU__RMT_CMD_RQST:
        errmsg(&msgsym(BADPDU,MAILBOX),"Rmt_Cmd_Rqst");
        break;
    case MAILBOX_PDU__DATE_TIME_N:
        errmsg(&msgsym(BADPDU,MAILBOX),"Date_Time_N");
        break;
    case MAILBOX_PDU__FAST_RESET_N:
        errmsg(&msgsym(BADPDU,MAILBOX),"Fast_Reset_N");
        break;
    case MAILBOX_PDU__IP_GATEWAY_N:
        errmsg(&msgsym(BADPDU,MAILBOX),"IP_Gateway_N");
        break;
    default:
        errmsg(&msgsym(BADPDU,MAILBOX),"Invalid");
    }

    return;
}



/******************************************************************************
 Function: void mailbox_v2_quiesce(int whence)
 
 Descr   : This function is called to set the mailbox into an idle state, 
           killing the inbox and outbox handler, resetting the state variables, 
           etc. It is called whenever something nasty (timeout, bad PDU) happens
           to the mailbox interface.  The single parameter is used to determine
           the order in which the handler processes are to be killed.
 ******************************************************************************/
void mailbox_v2_quiesce (int whence)
{
    /* Reset all variables. */
    incoming_mbox_state = MAILBOX_STATE__OFFLINE;
    outgoing_mbox_state = MAILBOX_STATE__OFFLINE;
    echo_resp_pending = FALSE;

    /* Disable interrupts. */
    reg_invoke_mailbox_v2_fence_interface();

    /* Kill the InBox and OutBox handlers. */
    switch (whence) {
    case MAILBOX_QUIESCE_FROM_ELSEWHERE:
        if (mailbox_recv_hndlr_pid) {
            process_kill(mailbox_recv_hndlr_pid);
            mailbox_recv_hndlr_pid = 0;
        }
        if (mailbox_send_hndlr_pid) {
            process_kill(mailbox_send_hndlr_pid);
            mailbox_send_hndlr_pid = 0;
        }
        break;
    case MAILBOX_QUIESCE_FROM_SEND_PROC:
        if (mailbox_recv_hndlr_pid) {
            process_kill(mailbox_recv_hndlr_pid);
            mailbox_recv_hndlr_pid = 0;
        }
        mailbox_send_hndlr_pid = 0;
        process_kill(THIS_PROCESS);
        break;
    case MAILBOX_QUIESCE_FROM_RECV_PROC:
        if (mailbox_send_hndlr_pid) {
            process_kill(mailbox_send_hndlr_pid);
            mailbox_send_hndlr_pid = 0;
        }
        mailbox_recv_hndlr_pid = 0;
        process_kill(THIS_PROCESS);
        break;
    }

    return;
}



/******************************************************************************
 Function: uchar mailbox_v2_compute_checksum(uchar *msg, int len)
 
 Descr   : Compute the checksum for the message pointed to by 'msg'. This is 
           nothing more than a running sum of all octets.
 ******************************************************************************/
uchar mailbox_v2_compute_checksum (uchar *msg, int len)
{
    uchar sum = 0;
    
    while (len--) {
        sum += *msg++;
    }
    return(sum);
}



/******************************************************************************
 Function: int mailbox_v2_validate_echo_resp(uchar *p,int len)
 
 Descr   : Compare the data held in the global 'echo-rqst' variable with the 
           data pointed to by 'p' and determine if they match.
 ******************************************************************************/
int mailbox_v2_validate_echo_resp (uchar *p,int len)
{
    int rc = 0;
    uchar *q;

    q = outgoing_echo_rqst_data;
    while (!rc && len--) { 
        if (*p++ != *q++) {
            rc = 1;
        }
    }

    return(rc);
}



/******************************************************************************
 Function: void mailbox_v2_update_ip(idbtype *idb, ipaddrtype addr, 
                                     ipaddrtype mask, boolean secondary,
				     boolean adding)
 
 Descr   : Called whenever the IP address has been modified in the hwidb. The
           test to see if the interface type is either ETHERNET or TOKEN_RING
           is done to satisfy the current capabilities of the Chipcom mailbox
           software.
 ******************************************************************************/
void mailbox_v2_update_ip (idbtype *idb, ipaddrtype addr, ipaddrtype mask,
                           boolean secondary, boolean adding)
{
    if ((idb->hwptr->type == IDBTYPE_LANCE || 
         idb->hwptr->type == IDBTYPE_TMS380) && 
        ((addr && adding == TRUE) || ((!addr) && adding == FALSE))) {
        if (outgoing_mbox_state == MAILBOX_STATE__ONLINE) {
            mailbox_v2_build_PDU(MAILBOX_PDU__IP_CONFIG_N,NULL,0,0);
            cenqueue_PDU(outgoing_mbox_queue,outgoing_msg_buffer);
        }
    }
    return;
}



/******************************************************************************
 Function: void mailbox_v2_update_mac(hwidbtype *hwidb)
 
 Descr   : Called whenever the MAC address has been modified in the hwidb. The
           test to see if the interface type is either ETHERNET or TOKEN_RING
           is done to satisfy the current capabilities of the Chipcom mailbox
           software.
 ******************************************************************************/
void mailbox_v2_update_mac (hwidbtype *hwidb)
{
    if (hwidb->type == IDBTYPE_LANCE || hwidb->type == IDBTYPE_TMS380) {
        if (outgoing_mbox_state == MAILBOX_STATE__ONLINE) {
            mailbox_v2_build_PDU(MAILBOX_PDU__MAC_CONFIG_N,NULL,0,0);
            cenqueue_PDU(outgoing_mbox_queue,outgoing_msg_buffer);
        }
    }
    return;
}
 


/******************************************************************************
 * mailbox_v2_update_ip_defaultgw
 *
 * Called whenever the IP default gateway has been modified.
 ******************************************************************************/

void mailbox_v2_update_ip_defaultgw (ipaddrtype gw)
{
    if (outgoing_mbox_state == MAILBOX_STATE__ONLINE) {
        mailbox_v2_build_PDU(MAILBOX_PDU__IP_GATEWAY_N,NULL,0,0);
        cenqueue_PDU(outgoing_mbox_queue,outgoing_msg_buffer);
    }
    return;
}



/******************************************************************************
 Function: char_queue_t *cqcreate(uint size)
 
 Descr   : This function is used to allocate space for the data and control
           variables of a character queue.
 ******************************************************************************/
char_queue_t *cqcreate (uint size)
{
    char_queue_t *q;
    uchar *p;

    if ((q = malloc(sizeof(char_queue_t))) == NULL) {
        return(NULL);
    } else if ((p = malloc(size)) == NULL) {
        free(q);
        return(NULL);
    } else {
        q->data = p;
        q->toq = q->boq = q->data;
        q->size = size;
        q->full = 0;
        return(q);
    }
}



/******************************************************************************
 Function: inline uchar cdequeue(char_queue_t *q)
 
 Descr   : The following function retrieves one character off the top of the
           queue if it is not empty. It is declared 'inline' to avoid the
           overhead associated with the function call. 
 ******************************************************************************/
inline uchar cdequeue (char_queue_t *q)
{
    uchar c;

    if (!q->full && q->toq == q->boq) {
        printf("cdequeue: Queue empty!\n");
        return('\0');
    } else {
        q->full = 0;
        c = *(q->toq++);
        if (q->toq == (q->data + q->size)) {
            q->toq = q->data;
        }
        return(c);
    }
}



/******************************************************************************
 Function: inline void cenqueue(char_queue_t *q, uchar c)
 
 Descr   : The following function places one character at the bottom of the 
           character queue if it is not full. It is declared 'inline' to avoid 
           the overhead associated with the function call.
 ******************************************************************************/
inline void cenqueue (char_queue_t *q, uchar c)
{
    if (!q->full) {
        *(q->boq++) = c;
        if (q->boq == (q->data + q->size)) {
            q->boq = q->data;
        }
 
        if (q->toq == q->boq) {
            q->full = 1;
        }
    }
    return;
}



/******************************************************************************
 Function: void cenqueue_PDU(char_queue_t *q, uchar *s)
 
 Descr   : Repeatedly call 'cenqueue' to enqueue an entire PDU. Note that this
           method of implementation does not pose significant overhead, since
           'cenqueue' is declared inline.  
 ******************************************************************************/
void cenqueue_PDU (char_queue_t *q, uchar *s)
{
    int i,len;
 
    len = *s + MAILBOX_PDU_HEADER_BYTES;
    for (i=0; i<len; i++) {
        cenqueue(q,s[i]);
    }
    return;
}



/******************************************************************************
 Function: inline boolean cqempty (char_queue_t *q)
 
 Descr   : Determine if the queue is empty and return the results.
 ******************************************************************************/
inline boolean cqempty (char_queue_t *q)
{
    return(!q->full && q->toq == q->boq);
}



/******************************************************************************
 Function: void show_mailbox_state (void)
 
 Descr   : Print all mailbox global variables to the console. This function is 
           useful for debugging the mailbox interface.
 ******************************************************************************/
void show_mailbox_state (void)
{
    PRINTINT(mailbox_recv_hndlr_pid);
    PRINTINT(mailbox_send_hndlr_pid);
    PRINTINT(incoming_mbox_state);
    PRINTINT(outgoing_mbox_state);

    printf("\n");
    PRINTINT(outgoing_mbox_count);

    printf("\n");
    PRINTBOOL(echo_resp_pending);
    PRINTBOOL(echo_resp_valid);
    return;
}
