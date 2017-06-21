/* $Id: microcom_mbox_c3000.c,v 3.2.4.3 1996/08/28 13:05:25 thille Exp $
 * $Source: /release/112/cvs/Xsys/partner/microcom_mbox_c3000.c,v $
 *------------------------------------------------------------------
 * microcom_mbox_c3000.c -- Microcom-Terminator specific Mailbox Functions
 *
 * August 1995, Li Jiang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The following file contains Chipcom mailbox protocol implementations for 
 * Microcom/Cisco-Terminator (c2511).
 *------------------------------------------------------------------
 * $Log: microcom_mbox_c3000.c,v $
 * Revision 3.2.4.3  1996/08/28  13:05:25  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.4.2  1996/08/03  23:35:55  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.2.4.1  1996/03/18  21:34:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.6.1  1996/03/16  07:23:23  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2  1996/03/09  00:47:36  ljiang
 * CSCdi50986:  Add a new procedure to fix a mailbox timeout problem
 *
 * Revision 3.1  1996/03/09  00:12:18  ljiang
 * CSCdi50986:  Add mailbox support for  Microcom Terminator(c2511
 * derivative)
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "logger.h"
#include "../les/msg_mailbox.c"		/* Not a typo, see logger.h */
#include "interface_private.h"
#include "sched.h"
#include "version.h"
#include "ieee.h"
#include "../os/gdb.h"
#include "../ip/ip_registry.h"
#include "../ui/debug.h"
#include "../dev/monitor1.h"
#include "../les/if_les.h"
#include "../les/if_les_hd64570.h"
#include "../les/if_les_lance.h"
#include "../les/mailbox_debug.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "partner_registry.h"
#include "chipcom_mbox.h"
#include "microcom_mbox_c3000.h"

/* 
 * Terminator mailbox globals. Data register and status register.
 */

volatile uchar *terminator_mb_data_ptr = (uchar *) ADRSPC_TERMINATOR_MBOX_DATA;
volatile ushort *terminator_mb_stat_ptr = (ushort *) ADRSPC_TERMINATOR_MBOX_STATUS;

/* Equivlent to the Cortez interrupt mask */
ushort terminator_mb_mask = TERMINATOR_MBOX_DISABLE;      

/* Local storage */
static int mailbox_input_pid = 0;
static int mailbox_output_pid = 0;

/* External Variables */
extern int nttylines;


/****************************************************************************
 * microcom_subsys_init
 *
 * The following function is called to allow generic initialization
 * of components of the chipcom subsystem.
 *****************************************************************************/

void microcom_subsys_init (subsystype *subsys)
{
    /* Microcom specific initialization. */
    (void) cfork((forkproc *) microcom_mailbox_v2_init, 0L, 500, 
                  "Microcom Mailbox Init", startup_ttynum);
    return;
}

/****************************************************************************
 * 
 * microcom_mailbox_v2_init
 *
 * This function is called by the subsystem initialization function at
 * the process level to initialize the mailbox interface. It calls
 * two "common" functions, one early on to set up the process handlers,
 * etc., and the other to perform the bulk of the initialization. This
 * method was chosen to allow as much sharing of code as is possible 
 * between the Cortez (2500) and Margarita (4500) and Terminator (2511)
 * implementations of the mailbox.
 * Note that since this function runs as an independant process, it is
 * terminated with a process_kill(THIS_PROCESS).
 *****************************************************************************/

void microcom_mailbox_v2_init (void)
{
    volatile uchar c_in;

    /*
     * Hook into the parse chains
     */
    chipcom_mbox_parser_init();

    /* Register callback functions needed for building PDU */
    reg_add_mailbox_v2_start_output(microcom_mailbox_v2_start_output,
	  		           "microcom_mailbox_v2_start_output");

    reg_add_mailbox_v2_fill_personality_PDU(microcom_mailbox_v2_fill_personality_PDU,
					    "microcom_mailbox_v2_fill_personality_PDU");
    
    reg_add_mailbox_v2_is_complex_port(microcom_mailbox_v2_is_complex_port,
				       "microcom_mailbox_v2_is_complex_port");

    reg_add_mailbox_v2_fence_interface(microcom_mailbox_v2_fence_interface,
				       "microcom_mailbox_v2_fence_interface");

    /* Perform the common early-init functions. */
    mailbox_v2_early_init_common(); 
 
    /* 
     * Create the two processes that handlers mailbox reads and writes.
     * The major difference between Cortez and Margarita is that 
     * Margarita does not generate interrupts for mailbox operations
     * as in the Cortez. These two processes are replacing the two 
     * corresponding interrupt service routings used for Cortez to
     * minimize changes to the rest of the mailbox code.
     */
    mailbox_output_pid = cfork((forkproc *)mailbox_v2_output, 0L, 0, 
                               "Mailbox Output", CONSOLE_LINENUM);
    mailbox_input_pid = cfork((forkproc *)mailbox_v2_input, 0L, 0, 
                              "Mailbox Input", CONSOLE_LINENUM);

    /*
     * Read the InBox, and set its state flag to "ONLINE". 
     */
    c_in=*terminator_mb_data_ptr;
    incoming_mbox_state = MAILBOX_STATE__ONLINE;
    if (mailbox_debug) {
        errmsg(&msgsym(ONLINE,MAILBOX),"Incoming");
    }

    /*
     * Enable reads and writes so we detect when the carrier reads the OutBox. 
     * After this happens, both mailboxes will be online, thus signaling the
     * success of the init procedure.
     */
    terminator_mb_mask = TERMINATOR_MBOX_ENABLE;

    /* Perform the bulk of the initialization. */
    mailbox_v2_init_common(MAILBOX_CAPABILITY_SET); 

       /* Register the appropriate callbacks. */
    reg_add_ip_address_change(mailbox_v2_update_ip,
                              "mailbox_v2_update_ip");
    reg_add_mac_address_change(mailbox_v2_update_mac,
                               "mailbox_v2_update_mac");
    reg_add_ip_defaultgw_change(mailbox_v2_update_ip_defaultgw,
                                "mailbox_v2_update_ip_defaultgw");
 
    /* We're done. */
    process_kill(THIS_PROCESS);
}

/******************************************************************************
 * microcom_mailbox_v2_start_output
 *
 * This function is called to initiate the transfer of data on the
 * mailbox interface. It is platform specific, as Cortez is implemented
 * using interrupts, while Margarita (4500) and Terminator use polling.
 *****************************************************************************/

void microcom_mailbox_v2_start_output (ushort c)
{
    sys_timestamp timer;

    if (mailbox_debug) {
        errmsg(&msgsym(MBOXDEBUG,MAILBOX),"Enabling write interrupts.");
        errmsg(&msgsym(WRITE,MAILBOX),c);
    }

    if (outbox_full()) {
        TIMER_START(timer,MAILBOX_INTEROCTET_TIMEOUT * ONESEC);
        while (outbox_full()) {
            if (AWAKE(timer)) {
                errmsg(&msgsym(TIMEOUT,MAILBOX),"outgoing");
                TIMER_START(timer,MAILBOX_INTEROCTET_TIMEOUT * ONESEC);
            }
            process_sleep_periodic(MAILBOX_INTEROCTET_DELAY);
        }
    }

    *terminator_mb_data_ptr = (uchar)c;
    terminator_mb_mask &= ~TERMINATOR_MBOX_WRITE_DISABLE;
    return;
}



/*
 * mailbox_v2_fence_interface
 *
 * This function is called to disable the mailbox interface in the
 * event of an error.
 */

void microcom_mailbox_v2_fence_interface (void)
{
    terminator_mb_mask = TERMINATOR_MBOX_DISABLE;
    /* Kill the input and output processes. */
    if (mailbox_input_pid) {
        process_kill(mailbox_input_pid);
        mailbox_input_pid = 0;
    }
    if (mailbox_output_pid) {
        process_kill(mailbox_output_pid);
        mailbox_output_pid = 0;
    }
    return;
}


/*******************************************************************************
 * mailbox_v2_output
 * 
 * A process to write the PDU to the mailbox data register one byte
 * at a time from the outgoing mailbox queue.
 *******************************************************************************/

forktype mailbox_v2_output (void)
{
    uchar cout;

    process_set_priority(PRIO_LOW);

    while (TRUE) {
        edisms((blockproc *)outbox_disabled_or_full, 0);

        switch (outgoing_mbox_state) {
        case MAILBOX_STATE__OFFLINE:
            errmsg(&msgsym(OFFLINE,MAILBOX),"Outgoing");
            break;
        case MAILBOX_STATE__RESET:
            if (mailbox_debug) {
                errmsg(&msgsym(ONLINE,MAILBOX),"Outgoing");
            }
            outgoing_mbox_state = MAILBOX_STATE__ONLINE; 
            terminator_mb_mask |= TERMINATOR_MBOX_WRITE_DISABLE;
            break;
        case MAILBOX_STATE__ONLINE:
            cout = cdequeue(outgoing_mbox_queue);
            *terminator_mb_data_ptr = cout;
            if (mailbox_debug) {
                errmsg(&msgsym(WRITE,MAILBOX),cout);
            }
            if (!(--outgoing_mbox_count)) {
                terminator_mb_mask |= TERMINATOR_MBOX_WRITE_DISABLE;
                if (mailbox_debug) {
                    errmsg(&msgsym(MBOXDEBUG,MAILBOX),
                           "Disabling mailbox writes.");
                }
    	    }
            break;
        }
    }
}
 

/*******************************************************************************
 * mailbox_v2_input
 *
 * A process to read the mailbox data register (one byte at a time) and 
 * store it in the incoming mailbox queue.
 *******************************************************************************/

forktype mailbox_v2_input (void)
{
    uchar cin; 

    process_set_priority(PRIO_LOW);

    while (TRUE) {
        edisms((blockproc *)inbox_empty_or_disabled, 0);

        switch (incoming_mbox_state) {
        case MAILBOX_STATE__OFFLINE:
            errmsg(&msgsym(OFFLINE,MAILBOX),"Incoming");
            break;
        case MAILBOX_STATE__ONLINE:
            cin = *terminator_mb_data_ptr;
            cenqueue(incoming_mbox_queue,cin);
            if (mailbox_debug) {
                errmsg(&msgsym(READ,MAILBOX),cin);
            }
            break;
        }
    }
}


/*******************************************************************************
 * mailbox_v2_fill_personality_PDU 
 *
 * Fill the personality PDU
 *******************************************************************************/
uchar *microcom_mailbox_v2_fill_personality_PDU (struct mbox_personality_PDU *p, 
                                        ushort flags)
{
    struct mbox_complex_port_info *q;
    hwidbtype *hwidb;
    uchar *c;
    ushort boot_version,boot_minor_version;
    int asyncs=0;
    static boolean fill_personality_PDU_initialization = TRUE;

    /*
     * Fill in the appropriate fields in the PERSONALITY PDU.
     */


  /*
     * Determine the nature of the image and fill in the appropriate fields
     * in the PERSONALITY PDU. (Using the CiscoWorks strings is an extremely
     * kludgey way to do this, but as there are no generic 'identity' functions,
     * no other solution is possible at this time.)
     */
    strcpy(p->module_descr,  MAILBOX_MODULE_DESCR);
    strcpy(p->module_num,    MAILBOX_MODULE_NUM);
    strcpy(p->module_num_ext,MAILBOX_MODULE_NUM_EXT);
    p->module_vendor = (uchar) MAILBOX_MODULE_VENDOR;
    p->module_config = (uchar) MAILBOX_MODULE_CONFIG;

    /* Set up the version information. See the Chipcom spec. for details. */
    p->sw_version[0] = (uchar) SYS_MAJORVERSION;
    p->sw_version[1] = (uchar) ((SYS_MINORVERSION & 0xF)<< 4);
    p->sw_version[0] &= 0x3f;   /* Reset the most signif. 2 bits. */

    if (strstr(ciscoversion,"Experimental")) {
        ;
    } else if (strstr(ciscoversion,"BETA TEST")) {
        p->sw_version[0] |= 0x40;
    } else {
        /* "Released" version */
        p->sw_version[0] |= 0xc0;
    }

    /*
     * The following code will extract the major and minor version numbers
     * from the boot version and store them in the PERSONALITY PDU. Note
     * that for boot ROMs with versions of 9.14(8) or less, the boot version
     * reported by mon_version() is incorrect. This method of implementation
     * over trying something like parsing the version string because it
     * seems to be the more stable approach over the long run.
     */
    boot_version = mon_version();
    p->sw_version[2] = boot_version / 256;
    boot_minor_version = boot_version % 256;
    p->sw_version[3] = (boot_minor_version &0xF) << 4;

    /*
     * Note that the two most-significant bits of the boot rev.
     * field are set to always reflect a 'released' version.
     */
    p->sw_version[2] |= 0xc0;

    p->module_capabilities = flags;
    p->op_status = OP_STATUS__NORMAL;
    p->admin_state = ADMIN_STATE__NORMAL;
    p->num_complex_ports = 0;  /* Increment this below. */
    p->num_simple_ports = 0;
    p->ip_dflt_gateway = (long) reg_invoke_ip_defaultgw();

    q = (struct mbox_complex_port_info *) &p->port_info;

    /* 
     * Run through all idb's for the complex port information.
     * For Margarita, all ports are complex ports.
     */
    FOR_ALL_HWIDBS(hwidb) {
        if (microcom_mailbox_v2_is_complex_port(hwidb)) {
            /* Set up the common information first. */
            q->port_capabilities = 0;
            q->op_status = PORT_OP_STATUS__NORMAL;
            if (hwidb->state == IDBS_ADMINDOWN) {
                q->admin_state = PORT_ADMIN_STATE__DISABLE;
            } else {
                q->admin_state = PORT_ADMIN_STATE__ENABLE;
            }
            q->conn_type = PORT_CONN_TYPE__BACKPLANE;
            q->ip_address = hwidb->firstsw->ip_address;
            q->ip_subnet_mask = hwidb->firstsw->ip_nets_mask;

            /* Set up the interface-specific information. */
            switch (hwidb->type) {
            case IDBTYPE_LANCE:
                q->network_type = PORT_NETWORK_TYPE__ETHERNET;
                q->speed = PORT_SPEED__10Mbps;
                ieee_copy(hwidb->hardware,q->mac_addr);
                break;
            default:
                ;
            }

            p->num_complex_ports++;
            q++;
        }
    }

    c = (uchar *) q;

    /* Run through all idb's again for the simple port information. */
    FOR_ALL_HWIDBS(hwidb) {
        if (!microcom_mailbox_v2_is_complex_port(hwidb)) {
            switch (hwidb->type) {
            case IDBTYPE_HD64570:
                *c++ = PORT_OP_STATUS__NORMAL;
                if (hwidb->state == IDBS_UP) {
                    *c++  =  PORT_ADMIN_STATE__ENABLE;
                } else {
                    *c++  =  PORT_ADMIN_STATE__DISABLE;
                }
                *c++ = PORT_NETWORK_TYPE__WAN;
                *c++ = mailbox_v2_get_serial_conn(hwidb);
                *c++ = mailbox_v2_get_serial_clock(hwidb);
                p->num_simple_ports++;
                break;
	      default:
                ;
            }
        }
    }
    /* Build PDU for 16 Async ports. HWIDB's will not be created for Async ports
     * until the interfaces are initialized. Here we are basically faking the
     * presence of those ports since Chipcom mailbox does not support dynamic update
     * of PDU's
     */
    if(fill_personality_PDU_initialization == TRUE) {
      asyncs=nttylines;
      while(asyncs--) {
	*c++ = PORT_OP_STATUS__NORMAL;
	*c++ = PORT_ADMIN_STATE__DISABLE;
	*c++ = PORT_NETWORK_TYPE__WAN;
	*c++ = PORT_CONN_TYPE__UNSPEC;
	*c++ = PORT_SPEED__UNSPEC;
	p->num_simple_ports++;
      }
      fill_personality_PDU_initialization = FALSE;
    }
    return c;
}


/*******************************************************************************
 * microcom_mailbox_v2_is_complex_port
 *
 * Returns true if the hwidb represents a complex port
 *******************************************************************************/

inline boolean microcom_mailbox_v2_is_complex_port (hwidbtype *idb)
{

    return ((idb->type == IDBTYPE_LANCE) ||
            (idb->type == IDBTYPE_TMS380));
}


/*
 * outbox_full
 */

static boolean outbox_full (void)
{
    return (*terminator_mb_stat_ptr & TERMINATOR_OUTBOX_FULL);
}

/*******************************************************************************
 * outbox_disabled_or_full
 *******************************************************************************/

static boolean outbox_disabled_or_full (void)
{
    return ((terminator_mb_mask & TERMINATOR_MBOX_WRITE_DISABLE) ||
            (*terminator_mb_stat_ptr & TERMINATOR_OUTBOX_FULL));
}


/*******************************************************************************
 * inbox_empty_or_disabled
 *******************************************************************************/

static boolean inbox_empty_or_disabled (void)
{
    return ((*terminator_mb_stat_ptr & TERMINATOR_INBOX_EMPTY) ||
            (terminator_mb_mask & TERMINATOR_MBOX_READ_DISABLE));
}


/*******************************************************************************
 * mailbox_v2_get_serial_conn
 *
 * This function is called while building the PERSONALITY_PDU to 
 * provide the cable type connected to the serial interfaces. 
 ******************************************************************************/

static uchar mailbox_v2_get_serial_conn (hwidbtype *idb)
{
    int cable;

    switch (hd64570_get_cable_type(idb)) {
    case CANCUN_CABLE_232:
        cable = PORT_CONN_TYPE__DB25;
        break;
    case CANCUN_CABLE_449:
        cable = PORT_CONN_TYPE__DB37;
        break;
    case CANCUN_CABLE_V35:
        cable = PORT_CONN_TYPE__V35;
        break;
    case CANCUN_CABLE_X21:
        cable = PORT_CONN_TYPE__X21;
        break;
    case CANCUN_CABLE_RS530:
        cable = PORT_CONN_TYPE__DB25;
        break;
    default:
        cable = PORT_CONN_TYPE__UNSPEC;
    }

    return(cable);
}


/*******************************************************************************
 * mailbox_v2_get_serial_clock
 *
 * This kludge function is necessary because the hd64570 header files
 * weren't written correctly, and thus the generic file requires the
 * inclusion of a platform-specific file.
 *******************************************************************************/

static uchar mailbox_v2_get_serial_clock (hwidbtype *hwidb)
{
    uchar speed;
    serialsb *ssb;

    ssb=idb_get_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    switch (hd64570_clock[ssb->serial_clockindex].rate) {
    case 19200:
        speed = PORT_SPEED__19_2Kbps;
        break;
    case 56000:
        speed = PORT_SPEED__56Kbps;
        break;
    case 4000000:
        speed = PORT_SPEED__4Mbps;
        break;
    default:
        speed = PORT_SPEED__UNSPEC;
        break;
    }
    return speed;
}

