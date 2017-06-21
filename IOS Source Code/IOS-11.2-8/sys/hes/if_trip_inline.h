/* $Id: if_trip_inline.h,v 3.1.6.1 1996/03/18 19:42:31 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/hes/if_trip_inline.h,v $
 *------------------------------------------------------------------
 * if_trip_inline.h -- inline defines for the cBus Token Ring Interface Card
 *
 * January 1996, Greg Stovall
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved. 
 *
 *------------------------------------------------------------------
 * $Log: if_trip_inline.h,v $
 * Revision 3.1.6.1  1996/03/18  19:42:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/14  00:56:49  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.1  1996/03/08  17:53:58  gstovall
 * Placeholder for the King.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_TRIP_INLINE_H__
#define __IF_TRIP_INLINE_H__

static inline void issue_ctr_srb_command_inline (
    hwidbtype *hwidb)
{
    (void)send_trip_poke_cuetrc_cmd(hwidb, 0x0F);
}

static inline void ctr_memb_setup_inline (
    hwidbtype *idb,
    ushort address)
{
    (void)select_if(idb);
    (void)send_trip_memb_select_cmd(idb, address);
}

static inline void ctr_memb_write_inline (
    hwidbtype *idb,
    ushort  value)
{
    (void)send_trip_memb_cont_write_cmd(idb, value);
}


/*
 * Routines for interlocking access to the Spyglass SRB
 */

static inline void lock_ctr (hwidbtype* idb, ulong max_length)
{
    spyglass_data_t* ctr = idb->ctr_private;
    ctr->cmd_in_progress = TRUE;
    TIMER_START(ctr->cmd_timeout, max_length);
}

static inline void unlock_ctr (hwidbtype* idb)
{
    spyglass_data_t* ctr = idb->ctr_private;

    ctr->cmd_in_progress = FALSE;
    TIMER_STOP(ctr->cmd_timeout);
}

static inline boolean ctr_locked (hwidbtype* idb)
{
    spyglass_data_t* ctr = idb->ctr_private;
    tokensb *tsb;

    if (ctr->cmd_in_progress && TIMER_RUNNING(ctr->cmd_timeout) &&
        !AWAKE(ctr->cmd_timeout)) {
        return(TRUE);
    } else {
        if (ctr->cmd_in_progress) {
	    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);
            tsb->tr_polling_state = TR_POLLING_TMOUT;
            tsb->tr_polling_failure = 0;
	    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
            if (tokenevent_debug)
                buginf("\n%s: TRIP spyglass SRB command timed out", idb->hw_namestring);
        }
        unlock_ctr(idb);
        return(FALSE);
    }
}

#endif   /* __IF_TRIP_H__ */
