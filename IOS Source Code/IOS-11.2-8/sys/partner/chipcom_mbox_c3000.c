/* $Id: chipcom_mbox_c3000.c,v 3.5.4.2 1996/08/28 13:05:23 thille Exp $
 * $Source: /release/112/cvs/Xsys/partner/chipcom_mbox_c3000.c,v $
 *------------------------------------------------------------------
 * chipcom_mbox.c -- Cortez-specific Chipcom Mailbox Functions
 *
 * July 1994, John G. Kuzma
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The following file contains those functions specific to the Cortez
 * (2500) implementation of the Chipcom mailbox protocol.
 *------------------------------------------------------------------
 * $Log: chipcom_mbox_c3000.c,v $
 * Revision 3.5.4.2  1996/08/28  13:05:23  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.4.1  1996/03/18  21:34:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.4  1996/03/16  07:23:18  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.3  1996/03/09  05:11:11  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.26.2  1996/03/07  10:40:33  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/01/24  22:37:51  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1996/03/11  18:57:59  ljiang
 * CSCdi50986:  Add missing header file after port ready commit
 *
 * Revision 3.4  1996/03/08  23:38:20  ljiang
 * CSCdi50986:  Add mailbox support for Microcom Terminator(c2511
 * derivative)
 *
 * Revision 3.3  1995/11/17  18:53:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:20:46  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:57:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/14  00:18:38  jkuzma
 * CSCdi37060:  Change version PDU sent to partner box for IPIPX and D-IBM
 *              images.
 *
 * Revision 2.3  1995/07/12  22:27:28  gstovall
 * CSCdi36994:  Partner code needs to be pulled out of core subsystem
 * Move the parse chain init from the c3000 specific file to a partner
 * specific file.  Make "debug mailbox" work on the 4500 in the process.
 *
 * Revision 2.2  1995/06/09  13:12:46  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:32:32  hampton
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
#include "stacks.h"
#include "../ui/debug.h"
#include "../dev/monitor1.h"
#include "../ip/ip_registry.h"
#include "../../boot/src-68-c3000/pcmap.h"
#include "../les/if_les.h"
#include "../les/if_les_hd64570.h"
#include "../les/mailbox_debug.h"
#include "chipcom_mbox.h"
#include "chipcom_mbox_c3000.h"
#include "partner_registry.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"


/* Cortez mailbox globals. */
volatile ushort *cortez_mb_data_ptr = (ushort *) ADRSPC_CORTEZ_MBOX_DATA;
volatile uchar  *cortez_mb_mask_ptr = (uchar *)  ADRSPC_CORTEZ_MBOX_MASK;

/************************************************************************
 Function: void chipcom_subsys_init(subsystype *subsys)
 
 Descr   : The following function is called to allow generic initialization
           of components of the chipcom subsystem.
 ************************************************************************/
void chipcom_subsys_init (subsystype *subsys)
{
    /* Chipcom specific initialization. */
    (void) cfork((forkproc *) mailbox_v2_init, 0L, 500, 
                 "Mailbox Init", startup_ttynum);
}
 
 

/******************************************************************************
 Function: void mailbox_v2_init(void)
 
 Descr   : This function is called by the subsystem initialization function at
           the process level to initialize the mailbox interface. It calls
           two "common" functions, one early on to set up the process handlers,
           etc., and the other to perform the bulk of the initialization. This
           method was chosen to allow as much sharing of code as is possible 
           between the Cortez (2500) and Blue-hair (4500) implementations of
           the mailbox.

           Note that since this function runs as an independant process, it is
           terminated with a process_kill(THIS_PROCESS).
 ******************************************************************************/
void mailbox_v2_init (void)
{
    uchar  c_out,c_in;

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
 
    /* Install the two interrupt handlers. */
    createlevel(MAILBOX_CLEARED_INT_LVL, mailbox_v2_outbox_isr,
            "Chipcom outgoing mailbox-cleared interrupt.");
    createlevel(MAILBOX_FILLED_INT_LVL, mailbox_v2_inbox_isr,
            "Chipcom incoming mailbox-filled interrupt.");

    /*
     * Read the InBox, and set its state flag to "ONLINE". Note that this
     * functionality once existed in the interrupt service routine, but the
     * strategy used during a reload forced its relocation to this point.
     */
    c_in = *cortez_mb_data_ptr;
    incoming_mbox_state = MAILBOX_STATE__ONLINE;
    if (mailbox_debug) {
        errmsg(&msgsym(ONLINE,MAILBOX),"Incoming");
    }

    /*
     * Enable interrupts so we detect when the carrier reads the OutBox. 
     * After this happens, both mailboxes will be online, thus signaling the
     * success of the init procedure.
     */
    c_out = ENABLE_MBOX_INTERRUPTS;
    *cortez_mb_mask_ptr = c_out;

    /* Perform the bulk of the initialization. */
    mailbox_v2_init_common(MAILBOX_CAPABILITY_SET); 

       /* Register the appropriate callbacks. */
    reg_add_ip_address_change(mailbox_v2_update_ip,
                              "mailbox_v2_update_ip");
    reg_add_mac_address_change(mailbox_v2_update_mac,
                               "mailbox_v2_update_mac");
    /* We're done. */
    process_kill(THIS_PROCESS);
}



/******************************************************************************
 Function: void mailbox_v2_start_output(ushort c)
 
 Descr   : This function is called to initiate the transfer of data on the
           mailbox interface. It is platform specific, as Cortez is implemented
           using interrupts, while Margarita (4500) uses polling.
 ******************************************************************************/
void mailbox_v2_start_output(ushort c)
{
    if (mailbox_debug) {
        errmsg(&msgsym(MBOXDEBUG,MAILBOX),"Enabling write interrupts.");
        errmsg(&msgsym(WRITE,MAILBOX),c);
    }
    *cortez_mb_data_ptr = BUILD_MBOX_CHAR(c);
    *cortez_mb_mask_ptr = ENABLE_MBOX_INTERRUPTS;
    return;
}



/******************************************************************************
 Function: void mailbox_v2_fence_interface(void)
 
 Descr   : This function is called to disable the mailbox interface in the
           event of an error.
 ******************************************************************************/
void mailbox_v2_fence_interface (void)
{
    *cortez_mb_mask_ptr = DISABLE_MBOX_INTERRUPTS;
    return;
}



/******************************************************************************
 Function: void mailbox_v2_outbox_isr(void)
 
 Descr   : Interrupt-service-routine which is called after the carrier (Chipcom)
           has read the router's outgoing mailbox. In the middle of a PDU, this
           is a signal that the next byte may be written into the mailbox. For 
           the last byte of a PDU, the interrupt will be masked by writing the 
           mask register after writing the data.
 ******************************************************************************/
void mailbox_v2_outbox_isr (void)
{
    ushort cout;

    switch (outgoing_mbox_state) {
    case MAILBOX_STATE__OFFLINE:
        errmsg(&msgsym(OFFLINE,MAILBOX),"Outgoing");
        break;
    case MAILBOX_STATE__RESET:
        if (mailbox_debug) {
            errmsg(&msgsym(ONLINE,MAILBOX),"Outgoing");
        }
        outgoing_mbox_state = MAILBOX_STATE__ONLINE; 
        *cortez_mb_mask_ptr = DISABLE_MBOX_WRITE_INTR;
        break;
    case MAILBOX_STATE__ONLINE:
        cout = (short) cdequeue(outgoing_mbox_queue);
        *cortez_mb_data_ptr = BUILD_MBOX_CHAR(cout);
        if (mailbox_debug) {
            errmsg(&msgsym(WRITE,MAILBOX),cout);
        }
        if (!(--outgoing_mbox_count)) {
            *cortez_mb_mask_ptr = DISABLE_MBOX_WRITE_INTR;
            if (mailbox_debug) {
                errmsg(&msgsym(MBOXDEBUG,MAILBOX),"Disabling write interrupts.");
            }
	}
        break;
    }
    return;
}
 


/******************************************************************************
 Function: void mailbox_v2_inbox_isr(void)
 
 Descr   : Interrupt-service-routine which is called after the carrier (Chipcom)
           has placed a byte in the router's incoming mailbox, indicating it has
           some data to read. Read the byte and place it in the input queue. 
           Note that since the byte is read within the interrupt service 
           routine, the incoming mailbox may always be considered "empty".
 ******************************************************************************/
void mailbox_v2_inbox_isr (void)
{
    ushort cin; 

    switch (incoming_mbox_state) {
    case MAILBOX_STATE__OFFLINE:
        errmsg(&msgsym(OFFLINE,MAILBOX),"Incoming");
        break;
    case MAILBOX_STATE__ONLINE:
        cin = *cortez_mb_data_ptr;
        cenqueue(incoming_mbox_queue,GET_MBOX_CHAR(cin));
        if (mailbox_debug) {
            errmsg(&msgsym(READ,MAILBOX),GET_MBOX_CHAR(cin));
        }
        break;
    }
    return;
}



/******************************************************************************
 Function: mailbox_v2_fill_personality_PDU
 
 Descr   : Fill the personality PDU
 ******************************************************************************/

uchar *mailbox_v2_fill_personality_PDU (struct mbox_personality_PDU *p,
                                        ushort flags)
{
    struct mbox_complex_port_info *q;
    hwidbtype *hw_idb_ptr;
    uchar *c;
    ushort boot_version,boot_minor_version;

    /*
     * Determine the nature of the image and fill in the appropriate fields
     * in the PERSONALITY PDU. (Using the CiscoWorks strings is an extremely
     * kludgey way to do this, but as there are no generic 'identity'
     * functions, no other solution is possible at this time.)
     */
    if (strstr(cw_table[CW_TABLE__CW_FEATURE].ptr,"ENTERPRISE$")) {
        strcpy(p->module_descr,  MAILBOX_MODULE_DESCR_E);
        strcpy(p->module_num,    MAILBOX_MODULE_NUM_E);
    } else if (strstr(cw_table[CW_TABLE__CW_FEATURE].ptr,"DESKTOP$")) {
        strcpy(p->module_descr,  MAILBOX_MODULE_DESCR_D);
        strcpy(p->module_num,    MAILBOX_MODULE_NUM_D);
    } else if (strstr(cw_table[CW_TABLE__CW_FEATURE].ptr,"DESKTOP|RSRB$")) {
        strcpy(p->module_descr,  MAILBOX_MODULE_DESCR_DIBM);
        strcpy(p->module_num,    MAILBOX_MODULE_NUM_DIBM);
    } else if (strstr(cw_table[CW_TABLE__CW_FEATURE].ptr,"IP|IPX$")) {
        strcpy(p->module_descr,  MAILBOX_MODULE_DESCR_IPIPX);
        strcpy(p->module_num,    MAILBOX_MODULE_NUM_IPIPX);
    } else {
        strcpy(p->module_descr,  MAILBOX_MODULE_DESCR_UNKNOWN);
        strcpy(p->module_num,    MAILBOX_MODULE_NUM_UNKNOWN);
    }

    strcpy(p->module_num_ext,MAILBOX_MODULE_NUM_EXT);
    p->module_vendor = (uchar) MAILBOX_MODULE_VENDOR;
    p->module_config = (uchar) MAILBOX_MODULE_CONFIG;

    /* Set up the version information. See the Chipcom spec. for details. */
    p->sw_version[0] = (uchar) SYS_MAJORVERSION;
    p->sw_version[1] = (uchar) (SYS_MINORVERSION/10 << 4);
    p->sw_version[1] |= (uchar) (SYS_MINORVERSION % 10);

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
    p->sw_version[3] = (boot_minor_version / 10) << 4;
    p->sw_version[3] |= (boot_minor_version % 10);
    /*
     * Note that the two most-significant bits of the boot rev.
     * field are set to always reflect a 'released' version.
     */
    p->sw_version[2] |= 0xc0;

    p->module_capabilities = flags;
    p->op_status           = OP_STATUS__NORMAL;
    p->admin_state         = ADMIN_STATE__NORMAL;
    p->num_complex_ports   = 0;  /* Increment this below. */
    p->num_simple_ports    = 0;
    p->ip_dflt_gateway = 0L;

    q = (struct mbox_complex_port_info *) &p->port_info;

    /* Run through all idb's for the complex port information. */
    FOR_ALL_HWIDBS(hw_idb_ptr) {
        if (mailbox_v2_is_complex_port(hw_idb_ptr)) {
            /* Set up the common information first. */
            q->port_capabilities = 0;
            q->op_status         = PORT_OP_STATUS__NORMAL;
            if (hw_idb_ptr->state == IDBS_UP) {
                q->admin_state = PORT_ADMIN_STATE__ENABLE;
            } else {
                q->admin_state = PORT_ADMIN_STATE__DISABLE;
            }
            q->conn_type      = PORT_CONN_TYPE__BACKPLANE;
            q->ip_address     = hw_idb_ptr->firstsw->ip_address;
            q->ip_subnet_mask = hw_idb_ptr->firstsw->ip_nets_mask;
            ieee_copy(hw_idb_ptr->hardware,q->mac_addr); /* MAC address */

            /* Next, set up the interface-specific information. */
            switch (hw_idb_ptr->type) {
            case IDBTYPE_LANCE:
                p->module_identifier = (ushort) MAILBOX_MODULE_IDENTIFIER_E;
                q->network_type = PORT_NETWORK_TYPE__ETHERNET;
                q->speed        = PORT_SPEED__10Mbps;
                break;
            case IDBTYPE_TMS380:
                p->module_identifier = (ushort)MAILBOX_MODULE_IDENTIFIER_TR;
                q->network_type = PORT_NETWORK_TYPE__TOKEN_RING;
                if (hw_idb_ptr->tr_ring_speed == 4) {
                    q->speed = PORT_SPEED__4Mbps;
                } else if (hw_idb_ptr->tr_ring_speed == 16) {
                    q->speed = PORT_SPEED__16Mbps;
                } else {
                    q->speed = PORT_SPEED__16Mbps_4Mbps;
                }
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
    FOR_ALL_HWIDBS(hw_idb_ptr) {
        switch (hw_idb_ptr->type) {
        case IDBTYPE_HD64570:
            *c++ = PORT_OP_STATUS__NORMAL;
            if (hw_idb_ptr->state == IDBS_UP) {
                *c++  =  PORT_ADMIN_STATE__ENABLE;
            } else {
                *c++  =  PORT_ADMIN_STATE__DISABLE;
            }
            *c++ = PORT_NETWORK_TYPE__WAN;
            *c++ = mailbox_v2_get_serial_conn(hw_idb_ptr);
            switch (mailbox_v2_get_serial_clock(hw_idb_ptr)) {
            case 19200:
                *c++ = PORT_SPEED__19_2Kbps;
                break;
            case 56000:
                *c++ = PORT_SPEED__56Kbps;
                break;
            case 4000000:
                *c++ = PORT_SPEED__4Mbps;
                break;
            default:
                *c++ = PORT_SPEED__UNSPEC;
                break;
            }

            p->num_simple_ports++;
            break;
        case IDBTYPE_BRI:
            *c++ = PORT_OP_STATUS__NORMAL;
            if (hw_idb_ptr->state == IDBS_UP) {
                *c++  =  PORT_ADMIN_STATE__ENABLE;
            } else {
                *c++  =  PORT_ADMIN_STATE__DISABLE;
            }
            *c++ = PORT_NETWORK_TYPE__WAN;
            *c++ = PORT_CONN_TYPE__TELCO;
            *c++ = PORT_SPEED__56Kbps; /* Closest number to BRI B-channel.*/

            p->num_simple_ports++;
            break;
        default:
            ;
        }
    }
    return c;
}



/******************************************************************************
 Function: mailbox_v2_is_complex_port
 
 Descr   : Returns true if the hwidb represents a complex port
 ******************************************************************************/
inline boolean mailbox_v2_is_complex_port (hwidbtype *idb)
{
    return ((idb->type == IDBTYPE_LANCE) ||
            (idb->type == IDBTYPE_TMS380));
}



/******************************************************************************
 Function: int mailbox_v2_get_serial_conn(hwidbtype *idb)
 
 Descr   : This function is called while building the PERSONALITY_PDU to 
           provide the cable type connected to the serial interfaces. 
 ******************************************************************************/
int mailbox_v2_get_serial_conn (hwidbtype *idb)
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



/******************************************************************************
 Function: int mailbox_v2_get_serial_clock(hwidbtype *hwidb)
 
 Descr   : This kludge function is necessary because the hd64570 header files
           weren't written correctly, and thus the generic file requires the
           inclusion of a platform-specific file.
 ******************************************************************************/
int mailbox_v2_get_serial_clock (hwidbtype *hwidb)
{
    serialsb *ssb;

    ssb = idb_get_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    return(hd64570_clock[ssb->serial_clockindex].rate);
}
