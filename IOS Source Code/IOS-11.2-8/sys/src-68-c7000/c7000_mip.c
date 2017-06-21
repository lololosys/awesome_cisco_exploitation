/* $Id: c7000_mip.c,v 3.5.18.6 1996/08/28 13:12:14 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c7000/c7000_mip.c,v $
 *------------------------------------------------------------------
 * c7000_mip.c -- Support for the MIP card specific to c7000
 *
 * 29-June-1993, Ronnie B. Kon (originally from if_mip.c)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: c7000_mip.c,v $
 * Revision 3.5.18.6  1996/08/28  13:12:14  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.18.5  1996/08/27  07:15:11  mwu
 * CSCdi67143:  MIP interfaces dont come up on 7000 platform
 * Branch: California_branch
 *
 * Revision 3.5.18.4  1996/08/13  02:23:17  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.5.18.3  1996/07/23  13:28:23  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.5.18.2  1996/07/08  22:44:57  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.5.18.1  1996/03/18  21:59:59  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.16.2  1996/02/20  17:56:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/08  22:22:22  wmay
 * CSCdi48458:  mip uses old scheduler - use watched queues for love
 * note handler
 *
 * Revision 3.4.16.1  1996/01/24  23:02:08  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/12/07  22:32:44  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/17  18:44:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:31:43  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:25:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:55:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "../hes/if_hes_common.h"
#include "../hes/if_fci.h"
#include "../hes/if_mci.h"
#include "../if/if_controller.h"
#include "../hes/hes_controller.h"
#include "../if/dsx1.h"
#include "../hes/if_mip.h"
#include "logger.h"
#include "packet.h"
#include "../wan/smds.h"
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */
#include "../if/priority.h"

/*
 * mip_platform_configured_check
 * Check to see if the system has been configured during
 * the system initialization. In the 7000 platform,
 * there is no post configuration init call back. So we
 * need to pretend we have done the configuration.
 */
boolean mip_platform_configured_check (void)
{
    return (TRUE);
}
void mip_platform_init (subsystype *subsys)
{
}
boolean mip_platform_idb_create (hwidbtype *hwidb)
{
    /* 7000 RP doesn't support fancy queueing for MIP */
    delete_fair_queue(hwidb); 
    return (TRUE);
}
void mip_platform_idb_delete (hwidbtype *hwidb)
{
}
void mip_platform_map_channel (mipinfo *mip, hwidbtype *hwidb,
			       ulong bitfield)
{
}
void mip_platform_if_reset (mipinfo *mip, hwidbtype *hwidb)
{
}

int mip_platform_get_txqlength (hwidbtype *idb)
{
    mipinfo *mip;
    leveltype level;
    ushort result;

    mip = mip_find_by_idb(idb);
    level = raise_interrupt_level(NETS_DISABLE);

    mip_select_vc(mip, idb->vc);

    mip_simple_command_result(mip, FCI_CMD_TXQLENGTH, &result);

    reset_interrupt_level(level);
    return(result);
}

ushort mip_select_if (mipinfo *mip)
{
    platform_hware_handle_type *hware_handle;
    mciregtype *regptr;
    ushort     status;

    hware_handle = mip->cdb->platform_hware_handle;
    regptr = hware_handle->regptr;
    
    regptr->argreg = mip->unit;
    regptr->cmdreg = FCI_CMD_SELECT;
    status = regptr->cmdreg;
    if (status != FCI_RSP_OKAY) {
	return(status);
    }

    regptr->argreg = mip->cdb->slot;
    regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
    status = regptr->cmdreg;
    return(status);
}

ushort mip_controller_status (mipinfo *mip, ushort param)
{
    platform_hware_handle_type *hware_handle;
    mciregtype *regptr;

    hware_handle = mip->cdb->platform_hware_handle;
    regptr = hware_handle->regptr;

    regptr->argreg = param;
    regptr->cmdreg = FCI_CMD_STATUS;
    return(regptr->cmdreg);
}

/*
 * mip_simple_command
 * Send a command with no parameter down to the card
 */
ushort mip_simple_command (mipinfo *mip, int command)
{
    platform_hware_handle_type *hware_handle;
    mciregtype *regptr;

    hware_handle = mip->cdb->platform_hware_handle;
    regptr = hware_handle->regptr;
    
    regptr->argreg = command;
    regptr->cmdreg = FCI_CTRLR_CMD;
    return(regptr->cmdreg);
}

/*
 * mip_simple_command_result
 * Send a command with no parameter down to the card, and get
 * back a result
 */
ushort mip_simple_command_result (mipinfo *mip, int command, 
					 ushort *result)
{
    ushort status;
    platform_hware_handle_type *hware_handle;
    mciregtype *regptr;

    hware_handle = mip->cdb->platform_hware_handle;
    regptr = hware_handle->regptr;

    
    regptr->argreg = command;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    if (status == FCI_RSP_OKAY) {
	*result = regptr->argreg;
    }
    return(status);
}

/*
 * mip_command_arg
 * Send a basic command with a ushort parameter down to the card
 */
ushort mip_command_arg (mipinfo *mip, int command, ushort param)
{
    platform_hware_handle_type *hware_handle;
    mciregtype *regptr;

    hware_handle = mip->cdb->platform_hware_handle;
    regptr = hware_handle->regptr;
    
    regptr->argreg = param;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;

    regptr->argreg = command;
    regptr->cmdreg = FCI_CTRLR_CMD;

    return(regptr->cmdreg);
}

/*
 * mip_command_larg
 * Send a basic command with a ulong parameter down to the card
 */
ushort mip_command_larg (mipinfo *mip, int command, ulong param)
{
    platform_hware_handle_type *hware_handle;
    mciregtype *regptr;

    hware_handle = mip->cdb->platform_hware_handle;
    regptr = hware_handle->regptr;

    regptr->largreg = param;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;

    regptr->argreg = command;
    regptr->cmdreg = FCI_CTRLR_CMD;
    return(regptr->cmdreg);
}


/*
 * mip_get_max_loveletter_size
 * Find the largest love letter size to help calculate MTU
 * Note that this is only used to help calculate the MTU,
 * and this is because, on the c7000, love letters are
 * shipped up in MEMD buffers from the same pool
 */
static ulong mip_get_max_loveletter_size (mipinfo *mip)
{
    int result = mip->applique_type == APPL_T1 ? T1_MAXCHAN : E1_MAXCHAN;

    result *= sizeof(MIP_VCErrorsType);
    return(sizeof(LoveLtrHdrType) + result);
}
	   

/*
 * platform_mip_adjust_mtu_for_love
 * Tweak various fields relating to MTUs to take into account
 * whether or not we ship loveletters in these buffers.   On
 * platforms where loveletters have their own buffer pools (e.g. RSP)
 * this routine does nothing.
 */
void platform_mip_adjust_mtu_for_love (mipinfo *mip,
				       ushort *maxmtu,
				       ulong *buffersize,
				       ulong *maxdgram)
{
    ulong ix;
    int delta ;

    ix = mip_get_max_loveletter_size(mip);
    if (ix > *maxdgram) {
	delta = ix - *maxdgram;
	*maxmtu += delta;
	*buffersize += delta;
	*maxdgram = ix;
    }
}

/*
 * platform_chain_hwidb_in
 * Platform specific routine to chain a hwidb into the 'system'
 * On the c7000 we simply call two utility routines to do all
 * the work.
 */
void platform_chain_hwidb_in (cdbtype *cdb, hwidbtype *hwidb)
{
    /*
     *  After hot swap of another board, or a microcode reload, the
     *  dci structure is nuked.  We need to guarantee our entries are
     *  there.  The idb chain is still valid from before, we just need
     *  to set the pointer to the head of the chain.
     *
     *  If the dci entry is still there, then this must simply be a
     *  remap of an existing channel.  This is perfectly legal (and
     *  fairly common).  These routines are smart enough to deal with
     *  this.
     */
    mci_add_interface(cdb->platform_hware_handle, cdb->unit, hwidb);
    dci_add_interface(cdb->platform_hware_handle, cdb->unit, hwidb);
}

/*
 * platform_unchain_hwidb
 * Platform specific routine to unchain a hwidb from the 'system'
 * On the c7000, this involves traversing the hwidb chain 
 * associated with this mci_index, and removing it.
 */
void platform_unchain_hwidb (cdbtype *cdb, hwidbtype *hwidb)
{
    mcitype *cbus;
    hwidbtype *cur, *prev;

    cbus = cdb->platform_hware_handle;
    for (cur = cbus->interface_list[hwidb->mci_index], prev = NULL ;
	 cur != hwidb && cur != NULL ; 
	 prev = cur, cur = cur->next_idb_on_physint)
	;
    if (cur == NULL) {
	/* how very odd */
	buginf("\n%%unlink_idb(): Cannot find IDB %x", hwidb);
    } else {
	if (prev == NULL) {
	    /* first on list */
	    cbus->interface_list[hwidb->mci_index] = 
		hwidb->next_idb_on_physint;
	    cbus->dci[hwidb->slot]->interface_list[hwidb->slotunit] =
		hwidb->next_idb_on_physint;
	} else
	    prev->next_idb_on_physint = hwidb->next_idb_on_physint;
    }
}

/*
 * prepare for hot-swaps
 * used only the c7000.  RSP, does not preallocate.
 */
void mip_preallocate (boolean more)
{
    int ix;
    mipinfo *tmp;
    
    if (!more)
	queue_init(&mip_preQ, MIP_MAX_DAUGHTERS);

    for (ix = mip_preQ.count ; ix < MIP_MAX_DAUGHTERS ; ix++) {
	tmp = malloc_named(sizeof(mipinfo), "MIP Info");
	if (tmp != NULL)
	    enqueue(&mip_preQ, tmp);
    }

    dsx1_preallocate(more, MIP_MAX_DAUGHTERS);
}

/*
 * free_mip
 * Either enqueue a mipinfo on the preQ, or free it
 */
void free_mip (mipinfo *mip)
{
    /*
     * 	a great idea here might be to free the memory from the mipinfo.
     *
     *	However, on the c7000, it is called from the hotstall
     *  interrupt handler, and these things are not so big that it's worth
     *  figuring out how to be clever and signal process level to do the
     *  free for us.     -- ronnie
     */
    enqueue(&mip_preQ, mip);
}

/*
 * platform_mip_set_offsets
 * Set up the rxoffsets, span_ stuff, SMDS stuff
 */
void platform_mip_set_offsets (hwidbtype *hwidb)
{
    hwidb->rxoffset = MIP_RXOFFSET_WORDS;
    switch (hwidb->enctype) {
    case ET_PPP:
	hwidb->span_macoffset = hwidb->rxoffset + (hwidb->encsize >> 1) + 1;
	hwidb->span_encapsize = hwidb->encsize + 2;
	break;
    case ET_SMDS:
	hwidb->span_macoffset = hwidb->rxoffset + (hwidb->encsize >> 1) + 1;
	hwidb->span_encapsize = hwidb->encsize + 2;

	/*
	 * On the c7000, we carry the board encaps around with the
	 * incoming packet, hence we need to account for it in
	 * fast_input_modelen, as well as fast_output_modelen.
	 * (see also comment in smds.h)
	 */
	hwidb->smds_pdb->fast_input_modelen =
	hwidb->smds_pdb->fast_output_modelen = MIP_ENCAPSIZE_BYTES +
	    (hwidb->smds_pdb->dximode ? SMDS_DXIBYTES : SMDS_CISCOBYTES);

	break;
    default:
	hwidb->span_macoffset = hwidb->rxoffset + (hwidb->encsize >> 1);
	hwidb->span_encapsize = hwidb->encsize;
	break;
    }
}

/*
 * platform_mip_adjust_encsize
 * Adjust the encsize field in the hwidb, in a platform
 * specific way.
 *
 * On the c7000, we bump it by MIP_ENCAPSIZE_BYTES, since
 * datagramstart on input packets will point at the board
 * encapsulation (the two vc bytes). 
 */
void platform_mip_adjust_encsize (hwidbtype *hwidb)
{
    hwidb->encsize += (MIP_ENCAPSIZE_BYTES);
}

/*
 * platform_mip_restart_init 
 *
 * Take care of the platform specific parts of re-initing a MIP
 * plughole.
 */
void platform_mip_restart_init (mipinfo *mip)
{
    platform_hware_handle_type *hware_handle;
    mciregtype *regptr;
    cdbtype *cdb = mip->cdb;

    hware_handle = mip->cdb->platform_hware_handle;
    regptr = hware_handle->regptr;

    cbus_idb_assign_pool(regptr, cdb->buffer_pool);
    cbus_idb_rx_max_byte_size(regptr, cdb->max_buffer_size);
    cbus_idb_rx_offset(regptr, MCI_SERIAL_OFFSET - MIP_ENCAPSIZE_WORDS);
    cbus_idb_rx_error(regptr, FCI_RX_ERROR_POOL);
    cbus_rx_buffer_parking(regptr, cdb->slot, cdb->cbus_burst_count);

    regptr->argreg = cdb->unit;
    regptr->cmdreg = MCI_CMD_SELECT;
    regptr->argreg = cdb->mci_txcount;
    regptr->cmdreg = MCI_CMD_QUEUE;
    regptr->argreg = 2;
    regptr->cmdreg = MCI_CMD_SLIMIT;
}

/*
 * platform_mip_carve_in_progress
 *
 * Determine whether or not it's safe to send buffer pool info
 * down to the card.  If we're in the middle of recarving MEMD,
 * then it's not safe.  On the c7000, this information is held
 * in the 'configuring' field of the cbus structure.
 */
boolean platform_mip_carve_in_progress (cdbtype *cdb)
{
    platform_hware_handle_type *hware_handle;

    hware_handle = cdb->platform_hware_handle;

    return(hware_handle->configuring);
}

/*
 * platform_mip_reset_in_progress
 *
 * Determine whether or not we're in the middle of resetting
 * the cbus.  On the c7000, this is indicated by the state
 * of the resetting flag in the cbus structure.
 */
boolean platform_mip_reset_in_progress (cdbtype *cdb)
{
    platform_hware_handle_type *hware_handle;

    hware_handle = cdb->platform_hware_handle;

    return(hware_handle->resetting);
}

/*
 * platform_mip_request_bus_reset
 *
 * Request that the system reset the cbus at its earliest
 * convenience.
 */
void platform_mip_request_bus_reset (cdbtype *cdb)
{
    platform_hware_handle_type *hware_handle;

    hware_handle = cdb->platform_hware_handle;

    hware_handle->restartdeferred = TRUE;
}

/*
 * platform_mip_update_carve_dbase
 *
 * Gets called by mip_set_maxdgram when the user makes a
 * significant change to a vc's MTU,  where significant is
 * defined as making it larger or smaller than the current
 * largest or smallest for all vc's on that plughole.
 * This routine gets called after the fields in the cdb have
 * been adjusted to reflect the new settings, but before we
 * recarve.  Because the c7000 currently carves from 
 * information directly gathered from the cdb, this routine
 * is currently a no-op on this platform.
 */
void platform_mip_update_carve_dbase (cdbtype *cdb)
{
}

/*
 * platform_mip_dynamic_idb_init
 *
 * Do the 'normal'  hwidb init stuff that usually happens at
 * cbus analyze time, for a freshly created MIP idb.
 * Returns TRUE if there's an error, otherwise FALSE.
 * On the c7000 we  simply call the old trusty cbus_mci_idb_init()
 */
boolean platform_mip_dynamic_idb_init (cdbtype *cdb, mipinfo* mip,
				       hwidbtype * hwidb)
{
    if (oir_debug)
	buginf("\nDoing platform_mip_dynamic_idb_init on %s, cdb %s, hwidb %#x",
	       hwidb->hw_namestring, cdb->cdb_namestring, hwidb);

    return(cbus_mci_idb_init(cdb->platform_hware_handle, cdb->unit,
			     mip->interfacetype, NULL, hwidb));
}


/*
 * mip_love_letter_handler
 * Process a love letter from the MIP (interrupt routine)
 */
void mip_love_letter_handler (mcitype *cbus, ushort bytes, int index)
{
    paktype *pak;
    
    /*
     *	Because we don't free these until process level, on a system which
     *  is doing a lot of fastswitching we can end up chewing up all our
     *  buffers here.  So we set a limit of
     *  (FCI_MAX_DCI_CTRLRS * MIP_MAX_DAUGHTERS + 1) love letters we will keep
     *  at any time.  When we pass that, we will throttle all MIPs until we
     *  drain the queue.
     */
    if (process_queue_size(mip_loveletterQ) >=
	(FCI_MAX_DCI_CTRLRS * MIP_MAX_DAUGHTERS + 1))
	mip_throttle_love_letters(TRUE);

    if((pak = getbuffer(bytes)) == NULL) {
	return;
    }
    pak->datagramstart = &pak->data_area[4];
    
    mci2buffer(&cbus->regptr->readlong, pak->datagramstart, bytes);
    
    PUTLONG(&pak->datagramstart[-4], index);
    if (process_enqueue(mip_loveletterQ, pak) == FALSE)
	retbuffer(pak);
}
