/* $Id: if_ags_c7000_cip.c,v 3.4.8.7 1996/08/28 12:44:26 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_ags_c7000_cip.c,v $
 *------------------------------------------------------------------
 * if_ags_c7000_cip.c: C7000 specific code for the Channel Interface Processor
 *
 * February 1995, John Foster
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_ags_c7000_cip.c,v $
 * Revision 3.4.8.7  1996/08/28  12:44:26  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.8.6  1996/08/26  18:24:17  kmitchel
 * CSCdi66467:  CIP love letter stats not displayed after 49 days of uptime
 * Branch: California_branch
 *
 * Revision 3.4.8.5  1996/08/18  23:00:46  kmitchel
 * CSCdi64004:  CPU utilisation in show controller cxbus is not shown for
 *              CIP
 * Branch: California_branch
 *
 * Revision 3.4.8.4  1996/08/13  02:17:21  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.4.8.3  1996/08/08  23:55:11  whp4
 * CSCdi52884:  cip ignores mtu size on datagrams to the host
 * Branch: California_branch
 * SP needs to know CIP mtu is different from AIP mtu for shared path.
 *
 * Revision 3.4.8.2  1996/05/17  11:17:36  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.48.3  1996/04/03  14:29:05  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.48.2  1996/03/21  06:07:38  ppearce
 * IBU modularity: Channel subblock
 * Branch: IbuMod_Calif_branch
 *   Move Channel fields from hwidbtype/idbtype structs to Channel subblock
 *
 * Revision 3.3.48.1  1996/03/17  17:49:01  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.4.8.1  1996/03/18  19:41:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/16  06:50:12  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.1  1996/01/24  21:45:55  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/02/28  19:47:44  motto
 * CSCdi45011:  Get hung output on CIP virtual interface
 *
 * Revision 3.3  1995/11/17  09:17:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:26:58  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/11  10:23:41  kmitchel
 * CSCdi25909:  Erroneous CIP configuration commands are still added to
 *              config file
 *
 * Revision 2.2  1995/08/08  16:46:45  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.1  1995/06/07 20:40:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "if_mci.h"
#include "if_fci.h"
#include "if_cip.h"
#include "dbus.h"
#include "if_cmd.h"
#include "logger.h"
#include "msg_cbus.c"			/* Not a typo, see logger.h */
#include "channel_sb.h"

const char cip_show_util_indentation[] = "  ";


/*****************************************************************************
 * Name: cip_send_mtu
 *
 * Description: Send down MTU to SP
 *
 * Input:
 *  hwidb -- pointer to hardware IDB for interface
 *
 * Return Value: None
 *
 ****************************************************************************/ 
static void cip_send_mtu (hwidbtype *hwidb) 
{
    idbtype *idb;
    ushort mtu;

    mtu = CIP_DEFAULT_MTU;
    FOR_ALL_SWIDBS_ON_HW(hwidb, idb)
	mtu = min(mtu, idb->ip_mtu);

    send_set_mtu_cmd(hwidb, mtu, FCI_SET_IP_MTU);
}

/*****************************************************************************
 * Name: cip_restart_init
 *
 * Description: Post CIP restart reinitialization
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface.
 *
 * Return Value: None
 *
 *****************************************************************************/
void cip_restart_init (hwidbtype *hwidb)
{
    hwidb->rxoffset = MCI_OFFSETSIZE - (hwidb->encsize >> 1);
    /*
     * Re-initialize the specified interface.
     * Assign the interface to a buffer pool, set max datagramsize,
     * set receive offset and love note pool.
     */
    send_assign_poolindex_cmd(hwidb, hwidb->buffer_pool);
    send_rxbytes_cmd(hwidb, hwidb->max_buffer_size);
    send_rx_offset_cmd(hwidb, hwidb->rxoffset);
    send_rx_error_cmd(hwidb, FCI_RX_ERROR_POOL);

    /*
     * Fix up SP's idea of interface MTU - we share an interface 
     * type with AIP, but AIP has bigger default MTU. 
     */
    cip_send_mtu(hwidb);
}


/*****************************************************************************
 * Name: cip_platform_init
 *
 * Description: platform-specific CIP init.  Since there are no
 *              special requirements on the ags/rp1, this is a stub.
 *
 * Input: 
 *   subsys -- pointer to the sybsystype for this subsystem
 *
 * Return Value: None
 *
 *****************************************************************************/
void cip_platform_init (subsystype *subsys)
{
}

/*
 * channel_periodic
 * Called from periodic background task.
 */
void channel_periodic (hwidbtype *hwidb)
{
    mcitype *mci;
    leveltype status;
    
    if (hwidb->state == IDBS_ADMINDOWN)
	return;

    mci = hwidb->devctl;
    
    /* If a delayed restart has been scheduled, do it now */
    if (mci->restartdeferred) {
	status = raise_interrupt_level(NETS_DISABLE);
	cbus_init(mci, FALSE);
	reset_interrupt_level(status);
    }
}

/*
 * The 7000 requires at least one interface to not be in the ADMINDOWN 
 * state for IPC to function to that slot.
 */
boolean ipc_avail_when_interfaces_shut (void) 
{
    return (FALSE);
}

/* 
 * fill in router serial number
 */

void cip_chassis_serial_init (cip_nodeidtype *cip_nodeid) 
{

    sprintf(cip_nodeid->sequence, "%02x%02x%02x%02x%02x%02x",
	    system_mac_addr[0], system_mac_addr[1],
	    system_mac_addr[2], system_mac_addr[3],
	    system_mac_addr[4], system_mac_addr[5]);
}


/*****************************************************************************
 * Name: cip_encaps_deed
 *
 * Description: Process data coming from the CIP
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface.
 *   src   -- start of data (RSP), NULL otherwise
 *   bytes -- length of incoming data
 *
 * Return Value: None
 *
 *****************************************************************************/
void cip_encaps_deed (hwidbtype *hwidb, void *src, ushort bytes)
{
    mciregtype       *regptr;
    plevel_t         *plevel_element;
    cip_idb_infotype *cip;
    ulong             code, data;
    
    regptr = hwidb->mci_regptr;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
	return;
    }

    plevel_element = cip_plevel_element_get(cip, PLEVEL_PREQ);
    
    if (!plevel_element) {
	/* Gotta reset the CIP */
	code = regptr->readlong;
	data = regptr->readlong;
	
	errmsg(&msgsym(CIPENCAP, CBUS), 
	       hwidb->hw_namestring, code, data);
	
	(*hwidb->reset)(hwidb);
	return;
    }

     
    /* Validate length of buffer before copying it in */
    if (bytes > PLEVENT_DSIZE) {
	errmsg(&msgsym(BADENCAPSZ, CBUS), 
	       hwidb->hw_namestring, bytes, PLEVENT_DSIZE);
	bytes = PLEVENT_DSIZE;
    }
 
    mci2buffer(&regptr->readlong, plevel_element->pl_data, bytes);
    cip_plevel_enqueue(hwidb, cip, plevel_element, bytes);
}


/*****************************************************************************
 * Name: hwidb_lookup
 *
 * Description: return the hwidb for the interface <slot>/<port>
 *
 * Input: 
 *   slot, port
 *
 * Return Value: hwidb or NULL 
 *
 *****************************************************************************/
hwidbtype *
hwidb_lookup (ushort slot, ushort port) {

    hwidbtype *hwidb;
    mcitype *cbus;
    dcitype *dci;

    hwidb = NULL;
    cbus = (mcitype *) cbusQ.qhead;
    
    if (slot < FCI_MAX_DCI_CTRLRS &&
	port < MAX_CTRLR_INTERFACES) {
	dci = cbus->dci[slot];
	if (dci)
	    hwidb = dci->interface_list[port];
    }
    return hwidb;
}

/*****************************************************************************
 * Name: cip_version_info
 *
 * Description: return the hw and sw version numbers if a CIP is in designated
 *              slot
 * Input: 
 *   slot
 *
 * Return Value: TRUE if a CIP is in slot
 *
 *****************************************************************************/
boolean
cip_version_info (ushort slot, ushort *hw_version, ushort *sw_version) {

    mcitype *cbus;
    dcitype *dci;

    cbus = (mcitype *) cbusQ.qhead;
    
    if (slot < FCI_MAX_DCI_CTRLRS) {
	dci = cbus->dci[slot];
	if (dci && 
	    (dci->ctrlr_type == FCI_CIP_CONTROLLER)) {
	    *hw_version = dci->hw_version;
	    *sw_version = dci->sw_version;
	    return TRUE;
	}
    }
    return FALSE;
}

/*****************************************************************************
 * Name: cip_fs_setup
 *
 * Description: Setup CIP fast switching for ilan interfaces.
 *
 * Input: 
 *   hwidbtype * of a CIP ILAN
 *
 * Return Value: None
 *
 *****************************************************************************/
void cip_fs_setup (hwidbtype *ilan_hwidb) {
  /*
   * Nothing to do for C7000 platform
   */
}
