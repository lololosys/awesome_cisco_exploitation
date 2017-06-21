/* $Id: rsp_posip.c,v 3.1.2.4 1996/09/07 18:57:26 xtang Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_posip.c,v $
 *------------------------------------------------------------------
 * rsp_posip.c - RSP specific POSIP support code
 *
 * January 1996, Robert Broberg
 * adapted from rsp_serial.c
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_posip.c,v $
 * Revision 3.1.2.4  1996/09/07  18:57:26  xtang
 * CSCdi67401:  ALIGN-3-SPURIOUS: Spurious memory access made at
 * 0x60191478 reading
 * Branch: California_branch
 *         Turned off hwidb->serial_flags for HSSI and POSIP interfaces,
 *         therefore disallow clock rate (clockrate) command on these.
 *         Any serial interfaces that do not have clockrate command
 *         implemented should act similarly.
 *
 * Revision 3.1.2.3  1996/08/28  13:14:20  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.2  1996/08/07  18:02:27  kmitchel
 * CSCdi49854:  DBUSINTERR after RSP dbus write; ineffective recvry of
 * disabled card
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/07/08  22:52:52  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/06/07  05:32:36  rbroberg
 * Add placeholders for new project
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "plural.h"
#include "logger.h"
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */
#include "interface_private.h"
#include "rsp_if.h"
#include "rsp_pcmap.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "../hes/cbus_registry.h"
#include "../hes/if_hes_common.h"
#include "../hes/if_fci.h"
#include "../hes/if_mci.h"
#include "rsp_common_if.h"
#include "../hes/if_cmd.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "lovenote.h"
#include "../hes/if_hes_serial.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../hes/if_posip.h"
#include "../parser/parser_defs_parser.h"

extern int nposnets;

static uchar  nPOSIPs;

/*****************************************************************************
 * Name: posip_love_note
 *
 * Description: love note handler for serial IP's
 *  
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the IP interface that
 *     generated the event (love note)
 *   lovenote -- pointer to the love note 
 *
 * Return Value: None
 *
 *****************************************************************************/
static void posip_love_note (hwidbtype *hwidb, lovenotetype *lovenote)
{
    if (lovenote->love_code & LOVE_CODE_CARRIER_CHANGE) {
        cbus_mci_carrier(hwidb);
    }
    else if (lovenote->data_length) {
	posip_parse_love_letter(hwidb,
				MEMD_OFFSET_TO_ADDR(lovenote->buffer_ptr),
				lovenote->data_length);
	}
}

void rsp_posip_idb_init (hwidbtype *hwidb, boolean vip_style)
{
    serialsb	*ssb;

    /*
     * Protect ourselves in case anybody tries to reset us
     * before MEMD is carved:
     */
    hwidb->reset = (reset_t) return_nothing;
    hwidb->listen = (listen_t)return_nothing;

    nposnets++;

    if (hwidb->status & IDB_NOHARDWARE) {
	hwidb->status &= ~IDB_NOHARDWARE;
	return;
    }

    hes_idb_init_before(hwidb);

    hwidb->status = IDB_SERIAL | FCI_IDBFLAGS | 
	IDB_HSSI | IDB_SYNC | IDB_POS;
    hwidb->name = IFNAME_POSIP;

    /*
     * RSP-specific POSIP initialization.
     */
    hwidb->serial_flags |= SERIAL_FLAGS_NO_CLOCKRATE;
    
    serial_idb_init(hwidb, MAXDS3SIZE,
		    (MAXDS3SIZE + DS3_OVERHEAD_BYTES),
		    VISIBLE_FCI_SERIAL_BANDOC3,
		    POS_SERIAL_DELAY);

    reg_add_media_transmit_delay(IDBTYPE_HSSI,
				 cbus_set_transmit_delay,
				 "cbus_set_transmit_delay");
    
    hwidb->firstsw->ip_mtu = MAXDS3SIZE;
    
    /*
     * hwidb->set_maxdgram must be initialized after calling
     * serial_idb_init() so that hdlc_setup does not attempt to call
     * cbus_mci_set_maxdgram() while doing system init.
     */
    hwidb->set_maxdgram = cbus_mci_set_maxdgram;
    hwidb->buffersize = (MAXDS3SIZE + DS3_BAGGAGE);
    
    rsp_serial_set_rxoffset(hwidb);
    
    hwidb->cbus_burst_count = hwidb->cbus_burst_count_def =
	HSCI_BURST_COUNT_DEF;
    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    ssb->hssi_lc_inhibit = TRUE;	/* Inhibit by default */
    ssb->serial_clockindex = 0;
    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    hwidb->love_note = (void *)posip_love_note;

    /*
     * Complete set up of common fields.
     */
    hes_serial_idb_init(hwidb);
    init_new_hwidb(hwidb);
    hes_idb_init_after(hwidb);
}


/*****************************************************************************
 * Name: posip_ctrlr_analyze
 *
 * Description: analyze part of initialization for POSIP
 *   This function is called when an board is first discovered and initialized.
 *   It is called the first time the board is plugged in, or when the router
 *   is resstarted.  It is *NOT* called for boards that are installed and
 *   running when a new board is inserted.
 *
 *   This function is responsible for discovering the interfaces
 *   on the IP (via the CBUS -- select command), and creating
 *   and initializing the CDB and the IDB for those interfaces.
 *
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *
 * Input: 
 *   slot -- pointer to a slottype structure for the IP with the
 *     following fields filled in:
 *       slot_num -- slot number for this card
 *       ccb_ptr -- pointer to the CCB for the IP
 *       cmd_q_ptr -- pointer to the CCB for the IP
 *
 * Output:
 *   The following fields in the slottype (pointed to by slot) are changed
 *     interface_count -- number of interfaces on this IP
 *     interface_list -- list of pointers to IDB's for the interfaces 
 *
 * Return Value: None
 *
 *****************************************************************************/
static void posip_ctrlr_analyze (slottype *slot)
{
    int port_num, i;
    ushort status, *sptr;
    ushort interface_type;
    hwidbtype *hwidb=NULL;
    enum IDBTYPE idb_type;

    
    /*
     * We loop through the interfaces on the IP selecting them as we go.
     * We use the select command to tell if the interface is there.  If the
     * select command is successful, it returns the interface type (which
     * should be either FCI_TYPE_SERIAL or FCI_TYPE_HSSI). If the select
     * commands fails, or the response is incorect, we break out of this
     * loop and return.  If things go OK, we initialize the port.
     */

    port_num = 0;
    status = send_select_cmd(slot, port_num, &interface_type);
    if (status != FCI_RSP_OKAY) {
	char name[CARDNAMELEN + 5]; /* room for name + slot number */

	sprintf(name, "%s%u", slot->card_name, slot->slot_num);
	errmsg(&msgsym(CMD, CBUS), "select", status, name);
	return;
    }
    if (interface_type == FCI_TYPE_OUT_OF_RANGE)
	return;
    else if (interface_type == FCI_TYPE_POS) {
	interface_type = FCI_TYPE_HSSI;
	idb_type = IDBTYPE_HSSI;
    }
    else {
	errmsg(&msgsym(PORTTYPE,CBUS), slot->card_name, slot->slot_num,
	       port_num, interface_type);
	return;
    }

    /*
     * Get the IDB for the interface
     * If we find a matching one, we can just use it,
     * otherwise we init a new one.
     */
    hwidb = rsp_get_matching_idb(slot, port_num, interface_type,
				 idb_type);
    if (hwidb == NULL) {
	return;		/* rsp_get_matching_idb() complains */
    }			/* about malloc() failure */
    
    /*
     * Set up common fields.
     */
    slot->interface_list[port_num] = hwidb;
    slot->interface_count = 1 + port_num;
    rsp_posip_idb_init(hwidb, FALSE);
    
    ++nPOSIPs;
    slot->pa_info[0] = malloc_named(32, "POSIP PA info");
    sptr = (ushort *)slot->pa_info[0];
    if (sptr == NULL) return;
    send_specific_cmd_1arg(hwidb, FCI_CMD_VERSION, FCI_PA_READ_RST);
    for (i = 0;i < 16; i++, sptr++)
	send_specific_cmd_1arg_sret(hwidb, FCI_CMD_VERSION,
				    FCI_PA_EEPROM_READ, sptr);
    
}

/*****************************************************************************
 * Name: posip_show_cbus
 *
 * Description: Get POSIP specific info. for 'show controller cbus'
 *
 * Input: 
 *   indx -- slot index number for this controller
 *
 * Return Value: None
 *
 *****************************************************************************/
void posip_show_cbus (ushort slot_index)
{
    slottype *slotp;
    hwidbtype *hwidb;
    int ix;
    
    slotp = &slots[slot_index];
    /* 
     * If the card is disabled, we have no relevant
     * information to report.  Also, we will be unable to
     * communicate with it using the cbus.
     */
    if (slotp->flags & BOARD_DISABLED)
	return;

    /*
     * find the Packet over Sonet interface
     */
    hwidb = NULL;
    for (ix = 0; ix < slotp->interface_count; ix++) {
	hwidb = slotp->interface_list[ix];
	if (hwidb)
	    break;
    }

    if (hwidb) {
	mem_lock(hwidb);
	posip_show_util(hwidb);
	free(hwidb);
    }
}

/*
 * invoked when an POSIP has been removed from the system
 */
static void
posip_ctrlr_removed (slottype *sp)
{
    --nPOSIPs;
    if (sp->pa_info[0]) {
	free(sp->pa_info[0]);
	sp->pa_info[0] = NULL;
    }
    if (sp->interface_list[0]) {
	--nnets;
	--nposnets;
	}
    return;
}

/*
 * display count of POSIP interfaces in the system
 */
static void
posip_print_controllers (void)
{
    if (nPOSIPs) 
	printf("%u POSIP controller%s (%u POSI).\n",
	      ArgAndPlural(nPOSIPs,"","s"), nposnets);
    return;
}

/*****************************************************************************
 * Name: posip_platform_init
 *
 * Description: platform-specific serial IP init
 *
 * Input: 
 *   subsys -- pointer to the sybsystype for this subsystem
 *
 * Return Value: None
 *
 *****************************************************************************/
void posip_platform_init (subsystype *subsys)
{
    nPOSIPs = 0;

    reg_add_ctrlr_analyze(FCI_POSIP_CONTROLLER,
	posip_ctrlr_analyze, "posip_ctrlr_analyze");
    reg_add_ctrlr_init(FCI_POSIP_CONTROLLER,
	serial_ctrlr_init, "serial_ctrlr_init");
    reg_add_if_init(FCI_POSIP_CONTROLLER, serial_if_init, "serial_if_init");
    reg_add_ctrlr_removed(FCI_POSIP_CONTROLLER,
			  posip_ctrlr_removed, "posip_ctrlr_removed");

    reg_add_print_controllers(posip_print_controllers,
			      "posip_print_controllers");
}

