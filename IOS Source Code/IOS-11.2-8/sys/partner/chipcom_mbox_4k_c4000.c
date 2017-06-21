/* $Id: chipcom_mbox_4k_c4000.c,v 3.6.4.3 1996/08/28 13:05:21 thille Exp $
 * $Source: /release/112/cvs/Xsys/partner/chipcom_mbox_4k_c4000.c,v $
 *------------------------------------------------------------------
 * chipcom_mbox_4k_c4000.c -- Margarita-specific Mailbox Functions
 *
 * February 1995, Cheng-Lee Nee
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The following file contains those functions specific to the
 * Margarita (4500) implementation of the Chipcom mailbox protocol.
 *------------------------------------------------------------------
 * $Log: chipcom_mbox_4k_c4000.c,v $
 * Revision 3.6.4.3  1996/08/28  13:05:21  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.4.2  1996/08/03  23:35:53  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.6.4.1  1996/03/18  21:33:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.4  1996/03/16  07:23:14  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.2.3  1996/03/09  05:11:09  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.2.2  1996/03/07  10:40:31  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/01/24  22:37:49  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1996/03/08  23:38:18  ljiang
 * CSCdi50986:  Add mailbox support for Microcom Terminator(c2511
 * derivative)
 *
 * Revision 3.5  1996/02/24  20:13:36  cnee
 * CSCdi49808:  Partner mailbox messages trashed under heavy mailbox
 * traffics
 *
 * Revision 3.4  1996/01/22  06:58:24  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/17  18:53:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:20:44  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:57:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/08/29  17:22:03  cnee
 * CSCdi39275:  Partners mailbox does not initialize correctly
 *
 * Revision 2.4  1995/07/12  22:27:26  gstovall
 * CSCdi36994:  Partner code needs to be pulled out of core subsystem
 * Move the parse chain init from the c3000 specific file to a partner
 * specific file.  Make "debug mailbox" work on the 4500 in the process.
 *
 * Revision 2.3  1995/06/18  06:19:04  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.2  1995/06/09 13:12:45  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:32:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../../boot/cpu.h"
#include "sys_registry.h"
#include "logger.h"
#include "../les/msg_mailbox.c"		/* Not a typo, see logger.h */
#include "interface_private.h"
#include "version.h"
#include "ieee.h"
#include "../os/region.h"
#include "../ip/ip_registry.h"
#include "../ui/debug.h"
#include "../dev/monitor1.h"
#include "../les/if_les.h"
#include "../src-c4000/c4000_bsi.h"
#include "../les/if_les_hd64570.h"
#include "../les/if_c4000_mcn.h"
#include "../les/if_les_lance.h"
#include "../les/mailbox_debug.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "partner_registry.h"
#include "chipcom_mbox.h"
#include "chipcom_mbox_4k_c4000.h"

/* 
 * Margarita mailbox globals.
 */

volatile uchar *margarita_mb_data_ptr = (uchar *) ADRSPC_MARG_MBOX_DATA;
volatile ushort *margarita_mb_stat_ptr = (ushort *) ADRSPC_MARG_MBOX_STAT;

/* Equivlent to the Cortez interrupt mask */
ushort margarita_mb_mask = MARGARITA_MBOX_DISABLE;	

/* Local storage */
static int mailbox_input_pid = 0;
static int mailbox_output_pid = 0;

/* External functions */
extern ulong get_processor_memory_size(void);

/*
 * chipcom_subsys_init
 *
 * The following function is called to allow generic initialization
 * of components of the chipcom subsystem.
 */

void chipcom_subsys_init (subsystype *subsys)
{

    /* Chipcom specific initialization. */
    (void) cfork((forkproc *) mailbox_v2_init, 0L, 500, 
                 "Mailbox Init", startup_ttynum);
    return;
}
 

/*
 * mailbox_v2_init
 *
 * This function is called by the subsystem initialization function at
 * the process level to initialize the mailbox interface. It calls
 * two "common" functions, one early on to set up the process handlers,
 * etc., and the other to perform the bulk of the initialization. This
 * method was chosen to allow as much sharing of code as is possible 
 * between the Cortez (2500) and Margarita (4500) implementations of
 * the mailbox.

 * Note that since this function runs as an independant process, it is
 * terminated with a process_kill(THIS_PROCESS).
 */

void mailbox_v2_init (void)
{
    volatile uchar  c_in;

    /*
     * Hook into the parse chains
     */
    chipcom_mbox_parser_init();

    /* Register callback functions needed for building PDU */
    reg_add_mailbox_v2_start_output(mailbox_v2_start_output,
	  		           "mailbox_v2_start_output");

    reg_add_mailbox_v2_fill_personality_PDU(mailbox_v2_fill_personality_PDU,
					    "mailbox_v2_fill_personality_PDU");
    
    reg_add_mailbox_v2_is_complex_port(mailbox_v2_is_complex_port,
				       "mailbox_v2_is_complex_port");

    reg_add_mailbox_v2_fence_interface(mailbox_v2_fence_interface,
				       "mailbox_v2_fence_interface");

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
    c_in = *margarita_mb_data_ptr;
    incoming_mbox_state = MAILBOX_STATE__ONLINE;
    if (mailbox_debug) {
        errmsg(&msgsym(ONLINE,MAILBOX),"Incoming");
    }

    /*
     * Enable reads and writes so we detect when the carrier reads the OutBox. 
     * After this happens, both mailboxes will be online, thus signaling the
     * success of the init procedure.
     */
    margarita_mb_mask = MARGARITA_MBOX_ENABLE;

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


/*
 * mailbox_v2_start_output
 *
 * This function is called to initiate the transfer of data on the
 * mailbox interface. It is platform specific, as Cortez is implemented
 * using interrupts, while Margarita (4500) uses polling.
 */

void mailbox_v2_start_output(ushort c)
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

    *margarita_mb_data_ptr = (uchar) c;
    margarita_mb_mask &= ~MARGARITA_MBOX_WRITE_DISABLE;
    return;
}


/*
 * mailbox_v2_fence_interface
 *
 * This function is called to disable the mailbox interface in the
 * event of an error.
 */

void mailbox_v2_fence_interface (void)
{
    margarita_mb_mask = MARGARITA_MBOX_DISABLE;
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


/*
 * mailbox_v2_output
 * 
 * A process to write the PDU to the mailbox data register one byte
 * at a time from the outgoing mailbox queue.
 */

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
            margarita_mb_mask |= MARGARITA_MBOX_WRITE_DISABLE;
            break;
        case MAILBOX_STATE__ONLINE:
            cout = cdequeue(outgoing_mbox_queue);
            *margarita_mb_data_ptr = cout;
            if (mailbox_debug) {
                errmsg(&msgsym(WRITE,MAILBOX),cout);
            }
            if (!(--outgoing_mbox_count)) {
                margarita_mb_mask |= MARGARITA_MBOX_WRITE_DISABLE;
                if (mailbox_debug) {
                    errmsg(&msgsym(MBOXDEBUG,MAILBOX),
                           "Disabling mailbox writes.");
                }
    	    }
            break;
        }
    }
}
 

/*
 * mailbox_v2_input
 *
 * A process to read the mailbox data register (one byte at a time) and 
 * store it in the incoming mailbox queue.
 */

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
            cin = *margarita_mb_data_ptr;
            cenqueue(incoming_mbox_queue,cin);
            if (mailbox_debug) {
                errmsg(&msgsym(READ,MAILBOX),cin);
            }
            break;
        }
    }
}


/*
 * mailbox_v2_fill_personality_PDU 
 *
 * Fill the personality PDU
 */
uchar *mailbox_v2_fill_personality_PDU (struct mbox_personality_PDU *p, 
                                        ushort flags)
{
    struct mbox_complex_port_info *q;
    hwidbtype *hwidb;
    uchar *c;
    ushort boot_version,boot_minor_version;
    ushort ext_nim_type = 0xFF;                /* default to none */
    uint type;
    ulong pmem_size, iomem_size;
    list_element *l_elt;

    /*
     * Fill in the appropriate fields in the PERSONALITY PDU.
     */
    strcpy(p->module_descr,  MAILBOX_MODULE_DESCR);

    if (nim_exists(EXT_NIM_SLOT)) {
        ext_nim_type = nim_get_type(EXT_NIM_SLOT);
    }
    switch (ext_nim_type) {
    case XX_FDDI_DBRIDGE:
    case XX_FDDI_DUAL:
        FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_FDDI, l_elt, hwidb) {
            if (hwidb->type == IDBTYPE_BSI) {
                if (mailbox_v2_get_fddi_conn(hwidb) == 
                    PORT_CONN_TYPE__SNGL_MD_FDDI)
                    strcpy(p->module_num, MAILBOX_MODULE_NUM_1FDS);
                else
                    strcpy(p->module_num, MAILBOX_MODULE_NUM_1FDM);
            }
        }
        break;
    case XX_FDDI_SBRIDGE:
    case XX_FDDI_SINGLE:
        FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_FDDI, l_elt, hwidb) {
            if (hwidb->type == IDBTYPE_BSI) {
                if (mailbox_v2_get_fddi_conn(hwidb) == 
                    PORT_CONN_TYPE__SNGL_MD_FDDI)
                    strcpy(p->module_num, MAILBOX_MODULE_NUM_1FSS);
                else
                    strcpy(p->module_num, MAILBOX_MODULE_NUM_1FSM);
            }
        }
        break;
    case XX_SERIAL_QUAD:
        strcpy(p->module_num, MAILBOX_MODULE_NUM_4SER);
        break;
    case XX_ETHER_DUAL:
    case XX_ETHER_SINGLE:
        strcpy(p->module_num, MAILBOX_MODULE_NUM_2E);
        break;
    case XX_MCN:
        type = mcn_get_adapter_type(EXT_NIM_SLOT);
        if (type == MCN_PA_BRI_SINGLE)
            strcpy(p->module_num, MAILBOX_MODULE_NUM_4BRI);
        else
            strcpy(p->module_num, MAILBOX_MODULE_NUM_8BRI);
        break;
    case XX_TOKEN_DUAL:
    case XX_TOKEN_SINGLE:
        strcpy(p->module_num, MAILBOX_MODULE_NUM_2TR);
        break;
    default:
        strcpy(p->module_num, MAILBOX_MODULE_NUM_UNKNOWN);
        break;
    }

    pmem_size = region_get_size_by_class(REGION_CLASS_LOCAL);
    iomem_size = region_get_size_by_class(REGION_CLASS_IOMEM);

    p->module_num_ext[0] = (char) (pmem_size >> 20);
    p->module_num_ext[1] = (char) (iomem_size >> 20);
    p->module_num_ext[2] = (char) 0;
    p->module_num_ext[3] = (char) 0;

    p->module_identifier = MAILBOX_MODULE_IDENTIFIER;
    p->module_vendor = (uchar) MAILBOX_MODULE_VENDOR;
    p->module_config = (uchar) MAILBOX_MODULE_CONFIG;

    /* Set up the version information. See the Chipcom spec. for details. */
    p->sw_version[0] = (uchar) SYS_MAJORVERSION;
    p->sw_version[1] = (uchar) ((SYS_MINORVERSION & 0xF) << 4);

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
    p->sw_version[3] = (boot_minor_version & 0xF) << 4;
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
        if (mailbox_v2_is_complex_port(hwidb)) {
            /* Set up the common information first. */
            q->port_capabilities = 0;
            q->op_status = PORT_OP_STATUS__NORMAL;
            q->admin_state = PORT_ADMIN_STATE__ENABLE;
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
            case IDBTYPE_TMS380:
                q->network_type = PORT_NETWORK_TYPE__TOKEN_RING;
                if (hwidb->tr_ring_speed == 4) {
                    q->speed = PORT_SPEED__4Mbps;
                } else if (hwidb->tr_ring_speed == 16) {
                    q->speed = PORT_SPEED__16Mbps;
                } else {
                    q->speed = PORT_SPEED__16Mbps_4Mbps;
                }
                ieee_copy(hwidb->hardware,q->mac_addr);
                break;
            case IDBTYPE_BSI:
                q->network_type = PORT_NETWORK_TYPE__FDDI;
                q->speed = PORT_SPEED__100Mbps;
                ieee_copy(hwidb->hardware,q->mac_addr);
                break;
            case IDBTYPE_HD64570:
                q->network_type = PORT_NETWORK_TYPE__WAN;
                q->speed = mailbox_v2_get_serial_clock(hwidb);
                break;
            case IDBTYPE_BRI:
                q->network_type = PORT_NETWORK_TYPE__WAN;
                q->speed = PORT_SPEED__56Kbps; /* Closest number to 
                                                  BRI B-channel*/
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
        if (!mailbox_v2_is_complex_port(hwidb)) {
            switch (hwidb->type) {
            case IDBTYPE_LANCE:
                *c++ = PORT_OP_STATUS__NORMAL;
                *c++  =  PORT_ADMIN_STATE__ENABLE;
                *c++ = PORT_NETWORK_TYPE__ETHERNET;
                *c++ = mailbox_v2_get_ether_conn(hwidb);
                *c++ = PORT_SPEED__10Mbps;
                p->num_simple_ports++;
                break;
            case IDBTYPE_TMS380:
                *c++ = PORT_OP_STATUS__NORMAL;
                *c++  =  PORT_ADMIN_STATE__ENABLE;
                *c++ = PORT_NETWORK_TYPE__TOKEN_RING;
                *c++ = PORT_CONN_TYPE__DB9;
                if (hwidb->tr_ring_speed == 4) {
                    *c++ = PORT_SPEED__4Mbps;
                } else if (hwidb->tr_ring_speed == 16) {
                    *c++ = PORT_SPEED__16Mbps;
                } else {
                    *c++ = PORT_SPEED__16Mbps_4Mbps;
                }
                p->num_simple_ports++;
                break;
            case IDBTYPE_BSI:
                *c++ = PORT_OP_STATUS__NORMAL;
                *c++  =  PORT_ADMIN_STATE__ENABLE;
                *c++ = PORT_NETWORK_TYPE__FDDI;
                *c++ = mailbox_v2_get_fddi_conn(hwidb);
                *c++ = PORT_SPEED__100Mbps;
                p->num_simple_ports++;
                break;
            case IDBTYPE_HD64570:
                *c++ = PORT_OP_STATUS__NORMAL;
                *c++  =  PORT_ADMIN_STATE__ENABLE;
                *c++ = PORT_NETWORK_TYPE__WAN;
                *c++ = mailbox_v2_get_serial_conn(hwidb);
                *c++ = mailbox_v2_get_serial_clock(hwidb);
                p->num_simple_ports++;
                break;
            case IDBTYPE_BRI:
                *c++ = PORT_OP_STATUS__NORMAL;
                *c++  =  PORT_ADMIN_STATE__ENABLE;
                *c++ = PORT_NETWORK_TYPE__WAN;
                *c++ = PORT_CONN_TYPE__RJ_UTP;
                *c++ = PORT_SPEED__56Kbps; /* Closest number to 
                                              BRI B-channel*/
                p->num_simple_ports++;
                break;
            default:
                ;
            }
        }
    }
    return c;
}


/*
 * mailbox_v2_is_complex_port
 *
 * Returns true if the hwidb represents a complex port
 */

inline boolean mailbox_v2_is_complex_port (hwidbtype *idb)
{
    return (idb->slot != EXT_NIM_SLOT);
}


/*
 * outbox_full
 */

static boolean outbox_full (void)
{
    return (*margarita_mb_stat_ptr & MARGARITA_OUTBOX_FULL);
}


/*
 * outbox_disabled_or_full
 */

static boolean outbox_disabled_or_full (void)
{
    return ((margarita_mb_mask & MARGARITA_MBOX_WRITE_DISABLE) ||
            (*margarita_mb_stat_ptr & MARGARITA_OUTBOX_FULL));
}


/*
 * inbox_empty_or_disabled
 */

static boolean inbox_empty_or_disabled (void)
{
    return ((*margarita_mb_stat_ptr & MARGARITA_INBOX_EMPTY) ||
            (margarita_mb_mask & MARGARITA_MBOX_READ_DISABLE));
}


/*
 * mailbox_v2_get_fddi_conn
 *
 * This function is called while building the PERSONALITY_PDU to 
 * provide the fddi connector mode.
 */

static uchar mailbox_v2_get_fddi_conn (hwidbtype *idb)
{
    bsi_instance_t      *ds;
    xx_fddi_regs_t      *nim;
    uchar type;

    ds = INSTANCE;
    nim = ds->module_regs;
    if (nim->player0.udr & SINGLEMODE)
	type = PORT_CONN_TYPE__SNGL_MD_FDDI;
    else
        type = PORT_CONN_TYPE__MULT_MD_FDDI;
    return type;
}


/*
 * mailbox_v2_get_ether_conn
 *
 * This function is called while building the PERSONALITY_PDU to 
 * provide the ethernet media type.
 */

static uchar mailbox_v2_get_ether_conn (hwidbtype *idb)
{
    uchar type;
    lance_instance_t *ds;

    ds = INSTANCE;
    if (ds->eim_regaddr->xx_csr0 & EIM_MEDIA_TYPE)
        type = PORT_CONN_TYPE__RJ_UTP;
    else
        type = PORT_CONN_TYPE__FEMALE_AUI;
    return type;
}


/*
 * mailbox_v2_get_serial_conn
 *
 * This function is called while building the PERSONALITY_PDU to 
 * provide the cable type connected to the serial interfaces. 
 */

static uchar mailbox_v2_get_serial_conn (hwidbtype *idb)
{
    uchar type;
    hd64570_instance_t *ds;

    ds = INSTANCE;
    switch (ds->port_id & ~1) {        /* convert to DTE for this test */
    case PA_NONE:
        type = PORT_CONN_TYPE__UNSPEC;
        break;
    case PA_RS232_DTE:
        type = PORT_CONN_TYPE__DB25;
        break;
    case PA_V35_DTE:
        type = PORT_CONN_TYPE__V35;
        break;
    case PA_RS449_DTE:
        type = PORT_CONN_TYPE__DB37;
        break;
    case PA_X21_DTE:
        type = PORT_CONN_TYPE__X21;
        break;
    case PA_RS232_NRZI_DTE:
        type = PORT_CONN_TYPE__DB25;
        break;
    case PA_G703_DTE:
        type = PORT_CONN_TYPE__UNSPEC;
        break;
    case PA_RS530_DTE:
        type = PORT_CONN_TYPE__DB25;
        break;
    default:
        type = PORT_CONN_TYPE__UNSPEC;
    }
    return type;
}


/*
 * mailbox_v2_get_serial_clock
 *
 * This kludge function is necessary because the hd64570 header files
 * weren't written correctly, and thus the generic file requires the
 * inclusion of a platform-specific file.
 */

static uchar mailbox_v2_get_serial_clock (hwidbtype *hwidb)
{
    uchar speed;
    serialsb *ssb;

    ssb = idb_get_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
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

