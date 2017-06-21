/* $Id: dialer_profile.c,v 3.1.2.10 1996/09/10 07:32:42 dblair Exp $
 * $Source: /release/112/cvs/Xsys/wan/dialer_profile.c,v $
 *------------------------------------------------------------------
 * Dialer profiles.
 *
 * March 1996 - Lionel Bustini
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dialer_profile.c,v $
 * Revision 3.1.2.10  1996/09/10  07:32:42  dblair
 * CSCdi62263:  Dialer profile with Multilink not working
 * Branch: California_branch
 * Fix botched comment during previous commit of CSCdi62263
 *
 * Revision 3.1.2.9  1996/09/10  07:17:41  dblair
 * CSCdi62263:  Dialer profile with Multilink not working
 * Branch: California_branch
 * Fix multilink and dialer profiles
 *
 * Revision 3.1.2.8  1996/09/10  02:43:25  suresh
 * CSCdi68129:  dialer pool-member 1 min-link 1 doesnt reserve the link
 * Branch: California_branch
 * Completely rewritten code for sharing of ISDN interfaces
 *
 * Revision 3.1.2.7  1996/08/28  13:21:17  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.6  1996/08/15  05:17:03  fox
 * CSCdi65101:  Neighbor routes not removed when connection ends
 * Branch: California_branch
 * Clear the neighbor route before the interface is unbound from
 * the dialer interface since the neighbor routes will point ot the
 * dialer interface.
 *
 * Revision 3.1.2.5  1996/08/13  20:02:52  wmay
 * CSCdi65253:  crash in dialer_holdq_output: dpak structure is reused for
 * broadcasts - make sure that we clear out the holdq field of that
 * structure after each call to reg_invoke_dialer_pending
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/07/23  22:04:22  suresh
 * CSCdi62055:  Dialer profile error message misleading
 * Branch: California_branch
 * Code Cleanup for change from dialer user to dialer profile.
 *
 * Revision 3.1.2.3  1996/07/20  19:35:56  wmay
 * CSCdi60728:  ISDN dialer holdQ not handled correctly
 * Forgot in initial commit
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/07/19  01:17:44  suresh
 * CSCdi63165:  Code Cleanup: dialer pool
 * Branch: California_branch
 * Cleaned up the dialer pool code.
 *
 * Revision 3.1.2.1  1996/04/27  06:36:17  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1  1996/04/22  22:51:39  syiu
 * Placeholder for Dial1_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "logger.h"
#include "mgd_timers.h"
#include "parser_defs_dialer.h"
#include "../parser/parser_defs_parser.h"
#include "dialer.h"
#include "dialer_private.h"
#include "dialer_debug.h"
#include "dialer_registry.h"
#include "msg_dialer.c"		/* Not a typo, see logger.h */
#include "../if/network.h"
#include "../if/static_map.h"
#include "../wan/ppp.h"
#include "../wan/ppp_registry.h"


extern dialerpoolheadtype *dialer_pool_head;


/*
 * dialer_verify_min_link
 */

/*
 * get_dialer_pool_head
 * Return head of dialer pool list for given pool.
 */
static dialerpoolheadtype *get_dialer_pool_head (uchar number)
{
    dialerpoolheadtype *head;

    head = NULL;
    for (head = dialer_pool_head; head; head = head->next_head) {
	if (head->pool_number == number)
	    break;
    }
    return(head);
}

/*
 * get_dialer_pool
 * Return pointer to dialer pool.
 */
dialerpooltype *get_dialer_pool (dialerdbtype *ddb, uchar number)
{
    dialerpooltype *pool;

    pool = NULL;
    FOR_ALL_POOLS_IN_INTERFACE(ddb, pool) {
	if (pool->pool_number == number)
	    break;
    }
    return(pool);
}

/*
 * get_free_dialer_in_pool
 * Get a free dialer from the pool.
 */
dialerdbtype *get_free_dialer_in_pool (dialerdbtype *ddb)
{
    dialerpoolheadtype *head;
    dialerpooltype *pool, *bestpool;
    dialerdbtype *outddb;
    short minsum, absolute_max, max_link, max_available;
    short total_links, used;

    if (ddb->dialer_pool == NULL)
	return(NULL);

    head = get_dialer_pool_head(ddb->dialer_pool->pool_number);

    /*
     * Run through all dialers and test all interfaces.
     */
    if (head != NULL) {
	bestpool = NULL;
	FOR_ALL_MEMBERS_IN_POOL(head, pool) {
	    outddb = pool->ddb;

	    /*
	     *  Do not proceed with dialers in shutdown state.
	     */
	    if (DIALER_SHUT(outddb))
		continue;

	    if (reg_invoke_dialer_is_free(outddb->dialer, outddb)) {

		minsum = get_dialer_minsum(outddb);
		used = get_dialer_used(outddb);
		max_link = pool->max_link ? pool->max_link : 
		    outddb->dialer_num_link;
		max_available = outddb->dialer_num_link - minsum;
		total_links = pool->dial_out + pool->dial_in;

		/*
		 *
		 * Absolute Max is the maximum allowed calls for a
		 * particular pool on a per interface basis. Its
		 * calculated the following way:
		 *
		 * max_available = whats available after all "min-link"
		 *                 reservations on the interface. This 
		 *                 is the common counter for all Dial-Out
		 *                 and Dial-In calls.
		 *
		 * pool->max_link = the maximum allowed calls configured
		 *                  for a particular pool on particular 
		 *                  interface.
		 *
		 * dialer_num_link = maximum number of channels available
		 *                   on the interface. pool->max_link defaults
		 *                   to this.
		 *
		 */

		absolute_max = min(max_link, max_available) +
	    (max_link < outddb->dialer_num_link ? 0 : pool->min_link);

		/*
		 * Can this call be allowed or not ?
		 */

		if (total_links >= absolute_max) {
		    if (dialer_debug)
			buginf("\n%s: has %d ongoing call(s)," 
			       " maximum allowed call(s) %d on pool %d",
			       outddb->ownidb->hw_namestring, 
			       total_links, absolute_max,
			       pool->pool_number);
		    continue;
		}

		/*
		 * This call can go through, increment appropriate counters
		 */
		
		if (ISDN_DIALER(outddb) && /* This is a ISDN interface */
		    (!(pool->min_link && 
		       (total_links < pool->min_link)))) {
		    /*
		     * This is an ISDN interface which doesn't have
		     * "min_links" configured or has exceeded its
		     *  min_link count
		     */

		    
		    if (used >= max_available) {
			if (dialer_debug)
			    buginf("\n%s: has total %d channels," 
				   " reserved %d, ongoing %d, available %d"
				   "on pool %d",
				   outddb->ownidb->hw_namestring,
				   outddb->dialer_num_link,
				   minsum,
				   used,
				   max_available,
				   pool->pool_number);
			return NULL;
		    }

		}

		/*
		 * Pick higher priority interface.
		 */
		if ((bestpool != NULL) && 
		    (pool->pool_priority <= bestpool->pool_priority))
		    continue;
		/*
		 * We now have a best interface.
		 */
		bestpool = pool;
	    }
	}
	if (bestpool != NULL) {
	    bestpool->dial_out++;
	    return(bestpool->ddb);
	}
    }
    return(NULL);
}

/*
 * start_fast_idle_in_pool
 * Start fast idle timer for all interfaces in dialer pool.
 */
static void start_fast_idle_in_pool (uchar number)
{
    dialerpoolheadtype *head;
    dialerpooltype *pool;

    head = get_dialer_pool_head(number);

    /*
     * Run through all dialers and start fast idle timer on all interfaces.
     */
    if (head != NULL) {
	FOR_ALL_MEMBERS_IN_POOL(head, pool) {
	    start_fast_idle(pool->ddb);
	}
    }
}

/*
 * dialer_call_failed
 *
 */
void dialer_call_failed (dialerdbtype *ddb, dialerdbtype *profileddb)
{
    dialerpooltype *pool;
    short total_links;

    pool = get_dialer_pool(ddb, profileddb->dialer_pool->pool_number);
    total_links = pool->dial_out + pool->dial_in;

    if ((pool != NULL) && (total_links > 0)) {
	pool->dial_out--;
    }

    if (!CONNECTED_GROUP(profileddb)) {
	profileddb->dialer_fsm = dialer_fsm_idle;
	set_dialer_state(profileddb, DIALER_READY);
    }
}

/*
 * dialer_bind_encap
 * Change encpasulation on a physical interface being bound to a profile.
 */
void dialer_bind_encap (dialerdbtype *ddb, dialerdbtype *profileddb)
{
    hwidbtype *idb, *profileidb;

    idb = ddb->ownidb;
    profileidb = profileddb->ownidb;
    ddb->dialer_save_encap = idb->enctype;
    reg_invoke_dialer_save_config(idb->enctype, ddb);
    dialer_set_member_encap(NULL, idb, profileidb->enctype);
    reg_invoke_media_copy_encap_fields(idb->enctype, profileidb, idb);
}

/*
 * dialer_unbind_encap
 * Change encpasulation on a physical interface being unbound from a profile.
 */
void dialer_unbind_encap (dialerdbtype *ddb, dialerdbtype *profileddb)
{
    hwidbtype *idb, *profileidb;

    idb = ddb->ownidb;
    profileidb = profileddb->ownidb;
    dialer_set_member_encap(NULL, idb, ddb->dialer_save_encap);
    reg_invoke_dialer_restore_config(ddb->dialer_save_encap, ddb);
}


/*
 * dialer_bind_settings
 * Copy destination settings both on dial-out/dial-in
 */
void dialer_bind_settings (dialerdbtype *ddb, dialerdbtype *profileddb)
{

  ddb->dialer_idle_ticks = profileddb->dialer_idle_ticks;
  ddb->dialer_fast_idle_ticks = profileddb->dialer_fast_idle_ticks;
  ddb->dialer_carrier_ticks = profileddb->dialer_carrier_ticks;
  ddb->dialer_enable_ticks = profileddb->dialer_enable_ticks;
  ddb->dialer_holdq_ticks = profileddb->dialer_holdq_ticks;
  
}

/*
 * dialer_bind_profile
 * Bind physical interface to profile.
 */
void dialer_bind_profile (dialerdbtype *ddb, dialerdbtype *profileddb)
{
    dialerpooltype *pool;
    int dialing_pool_number;

    if (ddb->dialer_bound) {
	/*
	 *  Interface is already bound to a profile.  Don't
	 *    bind it again
	 */
	return;
    }

    ddb->dialer_binding = TRUE;
    ddb->save_netcfg_ddb = ddb->netcfg_ddb;
    ddb->netcfg_ddb = profileddb;
    ddb->netcfg_idb = profileddb->ownidb;
    ddb->dialing_ddb= NULL;
    ddb->dialer_bound = TRUE;
    dialing_pool_number = profileddb->dialer_pool->pool_number;

    if (ddb->ownidb->type != IDBTYPE_VACCESS) {
	/*
	 *  Multilink will bind a VACCESS interface for the bundle
	 *    to the same profile as the first link.  Don't count
	 *    it in the call counters, since the first link
	 *    is already counted, and the bundle interface did
	 *    did not answer or make a call.
	 */

	dialer_increment_call(profileddb);

	if (!ddb->dialer_placed_call) {
	    pool = get_dialer_pool(ddb->save_netcfg_ddb, dialing_pool_number);
	    if (pool != NULL) {
		pool->dial_in++;
	    }
	}
    }

    if (ddb->number_called[0] == '\0') {
	if (profileddb->dialer_string != NULL) {
	    /*
	     * If one or more dialer strings are configured, pick the first one.
	     */
	    dialer_set_number_called(ddb, 
			    profileddb->dialer_string->entry->dialer_string);
	} else {
	    strcpy(ddb->number_called, "<unknown phone number>");
	}
    }
    TIMER_START(ddb->dialer_idle_timer, ddb->dialer_idle_ticks);
/* This may be needed for dialer profile. 3/29/96
    dialer_bind_encap(ddb, profileddb);
*/
    ddb->dialer_binding = FALSE;
    if (LOG_INTF_IS_LOGGING_EVENTS_LINK_STATUS(profileddb->ownidb)) {
	errmsg(&msgsym(BIND, DIALER), ddb->ownidb->hw_namestring,
	       ddb->netcfg_idb->hw_namestring);
    }
}

/*
 * dialer_unbind_profile
 * Unbind physical interface from profile.
 */
void dialer_unbind_profile (dialerdbtype *ddb)
{
    dialerpooltype *pool;
    dialerdbtype *profileddb;
    int dialing_pool_number;

    if (!ddb->dialer_bound)
	return;
    
    ddb->dialer_bound = FALSE;

    /*
     *  Some processes below may suspend.  Stop the dialer_idle_timer
     *    here to prevent expiration during the suspension.
     */

    TIMER_STOP(ddb->dialer_idle_timer);
    TIMER_STOP(ddb->dialer_fast_idle_timer);

    /*
     * We need to do the IPCP cleanup NOW since the neighbor route
     * will be pointing at the dialer interface that we are about to
     * unbind from.  All sanity checking is done in the registry call.
     * This call is safe even if the interface is not PPP.
     */
    reg_invoke_ipcp_cleanup_ip_peer(ddb->ownidb->firstsw);

    /*
     *  Clean up connected groups, if any
     */

    if (CONNECTED_GROUP(ddb)) {
	dialer_unlink_member(ddb);
    }

    profileddb = ddb->netcfg_ddb;
    dialing_pool_number = profileddb->dialer_pool->pool_number;

    if (!CONNECTED_GROUP(profileddb)) {
	/*
	 * Undo any per-user authorisation settings now that all members
	 * have been removed.
	 */
	reg_invoke_author_remove_per_user(ddb->netcfg_idb);
	profileddb->dialer_fsm = dialer_fsm_idle;
	set_dialer_state(profileddb, DIALER_READY);
    }
/* This may be needed for dialer profile. 3/29/96
    dialer_unbind_encap(ddb, profileddb);
*/


    if (ddb->ownidb->type != IDBTYPE_VACCESS) {
	/*
	 *  Don't count VACCESS for the same
	 *    reasons as in dialer_bind_profile
	 */

	dialer_decrement_call(profileddb);
	pool = get_dialer_pool(ddb->save_netcfg_ddb, dialing_pool_number);
	if (pool != NULL) {
	    if (ddb->dialer_placed_call)
		pool->dial_out--;
	    else
		pool->dial_in--;
	    
	    if (dialer_debug)
		buginf("\n%s: has total %d call(s), dial_out %d, dial_in %d",
		       ddb->save_netcfg_ddb->ownidb->hw_namestring,
		       ddb->save_netcfg_ddb->dialer_num_link,
		       pool->dial_out,
		       pool->dial_in);
	    
	}
    }
    ddb->netcfg_ddb = ddb->save_netcfg_ddb;
    ddb->netcfg_idb = ddb->netcfg_ddb->ownidb;
    ddb->save_netcfg_ddb = NULL;
    ddb->dialer_idle_ticks = DIALER_IDLE_TIME;
    ddb->dialer_fast_idle_ticks = DIALER_FAST_IDLE_TIME;
    ddb->dialer_enable_ticks = DIALER_ENABLE_TIME;
    ddb->dialer_carrier_ticks = DIALER_CARRIER_TIME;
    if (LOG_INTF_IS_LOGGING_EVENTS_LINK_STATUS(profileddb->ownidb)) {
	errmsg(&msgsym(UNBIND, DIALER), ddb->ownidb->hw_namestring,
	       profileddb->ownidb->hw_namestring);
    }
}

/*
 * dialer_match_pool
 * Attempt to match and bind a physical interface to a profile using pool 
 * configuration.
 */
boolean dialer_match_pool (dialerdbtype *ddb)
{
    dialerpoolheadtype *head;
    dialerpooltype *pool;
    dialerdbtype *netddb, *profileddb;
    uchar number;

    /*
     *  Check if interface is already bound
     */

    if (ddb->dialer_bound)
	return(TRUE);

    netddb = ddb->netcfg_ddb;		/* Takes care of BRI/PRI */
    profileddb = NULL;
    number = 0;

    /*
     * Check all pools the interface is a member of.  In order to find a match
     * there has to be only one possible profile.
     */
    FOR_ALL_POOLS_IN_INTERFACE(netddb, pool) {
	head = pool->pool_head;
	if (head->number_of_profile_pools == 0)
	    continue;
	if ((head->number_of_profile_pools == 1) && (profileddb == NULL)) {
	    number = pool->pool_number;
	    profileddb = head->first_profile_pool->ddb;
	    continue;
	}
	return(FALSE);
    }

    if (profileddb == NULL) {
	return(FALSE);
    }

    dialer_bind_settings(ddb, profileddb);
    dialer_bind_profile(ddb, profileddb);
    return(TRUE);
}

/*
 * dialer_match_caller_id
 * Attempt to match and bind a physical interface to profile using CLI.
 */
static boolean dialer_match_caller_id (dialerdbtype *ddb, char *number)
{
  dialerpoolheadtype *head;
  dialerpooltype *pool, *profilepool;
  dialerdbtype *netddb, *profileddb;

  netddb = ddb->netcfg_ddb;		/* Takes care of BRI/PRI */
  profileddb = NULL;

  /*
   *  Check if interface is already bound
   */
  
  if (ddb->dialer_bound)
      return(TRUE);

  /*
   * Check all pools the interface is a member of.  Stop on the first
   * profile which has a "dialer caller" matching the caller id.
   * There should be only one if the box is properly configured; 
   * if not then you get what you deserve.
   */
  FOR_ALL_POOLS_IN_INTERFACE(netddb, pool) {
    head = pool->pool_head;
    FOR_ALL_PROFILES_IN_POOL(head, profilepool) {
      profileddb = profilepool->ddb;
      if (dialer_check_number(profileddb, number)) {
	dialer_bind_settings(ddb, profileddb);
	dialer_bind_profile(ddb, profileddb);
	return(TRUE);
      }
    }
  }

  return(FALSE);
}

/*
 * dialer_match_pool_or_caller_id
 * Attempt to match a physical interface to a profile using pool or CLI, then
 * bind them.
 */
boolean dialer_match_pool_or_caller_id (dialerdbtype *ddb, char *number)
{
    if (dialer_match_pool(ddb))
	return(TRUE);

    if (dialer_match_caller_id(ddb, number))
	return(TRUE);

    return(FALSE);
}

/*
 * dialer_match_name
 * Attempt to bind a physical interface to a profile using the authenticated
 * name, then bind them.
 */
boolean dialer_match_name (dialerdbtype *ddb, char *name)
{
    dialerpoolheadtype *head;
    dialerpooltype *pool, *profilepool;
    dialerdbtype *netddb, *profileddb;

    netddb = ddb->netcfg_ddb;		/* Takes care of BRI/PRI */
    profileddb = NULL;

    /*
     *  Check if interface is already bound
     */

    if (ddb->dialer_bound)
	return(TRUE);

    /*
     * Check all pools the interface is a member of.  Stop on the first
     * profile whose name matches the autheticated name.  There should
     * be only one if the box is properly configured; if not then you
     * get what you deserve.
     */
    FOR_ALL_POOLS_IN_INTERFACE(netddb, pool) {
	head = pool->pool_head;
	FOR_ALL_PROFILES_IN_POOL(head, profilepool) {
	    profileddb = profilepool->ddb;
	    if (strcmp(name, profileddb->dialer_profile) == 0) {
		dialer_bind_settings(ddb, profileddb);
		dialer_bind_profile(ddb, profileddb);
		return(TRUE);
	    }
	}
    }

    return(FALSE);
}
/*
 * dialer_match_bound_ddb
 * Attempt to bind an interface to the same profile as the
 *   given bound_ddb is bound to.
 */
boolean dialer_match_bound_ddb (dialerdbtype *ddb, dialerdbtype *bound_ddb)
{
    dialerdbtype *profileddb;

    /*
     *  bound_ddb must be non-NULL and already bound to a dialer profile
     *    interface. 
     */

    if (!ddb || !bound_ddb || !bound_ddb->dialer_bound)
	return(FALSE);

    /*
     *  Check if interface is already bound
     */

    if (ddb->dialer_bound)
	return(TRUE);

    /*
     *  The profile ddb is the netcfg_ddb of the interface that is already
     *    bound as done in dialer_bind_profile.
     */
    profileddb = bound_ddb->netcfg_ddb;

    dialer_bind_settings(ddb, profileddb);
    dialer_bind_profile(ddb, profileddb);

    return(TRUE);

}

/*
 *  dialer_find_profile_by_name
 *  given a name (or string), find a profileddb that matches it
 */

dialerdbtype * dialer_find_profile_by_name (char *name)
{

    dialerpoolheadtype *head;
    dialerpooltype *profilepool;
 
    /*
     *  Check the remote-names
     */
   
    FOR_ALL_HEAD_OF_POOLS(head) {
	FOR_ALL_PROFILES_IN_POOL(head, profilepool) {
	    if (strcmp(name, profilepool->ddb->dialer_profile) == 0) {
		return(profilepool->ddb);
	    }	
	}
    }
    return(NULL);
}


/*
 * dialer_may_match_name
 * Determine whether physical interface may be matched up to a profile
 * using the authenticated name.
 */
boolean dialer_may_match_name (hwidbtype *idb)
{
    return((is_ppp(idb) && IS_PPP_AUTH(idb)) || is_cpp(idb));
}

/*
 * dialer_fsm_idle
 * Dialer profile is not connected : attempt to place a call.
 */
dialerdbtype *dialer_fsm_idle (dialerdbtype *ddb, dialerpaktype *dpak)
{
    dialerdbtype *outddb;

    /*
     * We can't place a call if the packet is not interesting, or if
     * no dialer string or no dialer pool are configured.
     */
    if (!dpak->interesting) {
	return(NULL);
    }

    if (NULL == ddb->dialer_string) {
        if (dialer_debug) {
	    buginf("\n%s: Can't place call, no dialer string set",
		   ddb->ownidb->hw_namestring);
        }
        return(NULL);
    }

    if (NULL == ddb->dialer_pool) {
        if (dialer_debug) {
	    buginf("\n%s: Can't place call, no dialer pool set",
		   ddb->ownidb->hw_namestring);
        }
        return(NULL);
    }

    /*
     * Attempt to get a free dialer and place a call.
     */
    outddb = get_free_dialer_in_pool(ddb);
    if (outddb == NULL) {
	/*
	 * No free dialer, start fast idle timer.
	 */
	if (dialer_debug)
	    buginf("\n%s: No free dialer - starting fast idle timer",
		   ddb->ownidb->hw_namestring);
	start_fast_idle_in_pool(ddb->dialer_pool->pool_number);
	return(NULL);
    }
    dialer_bind_settings(outddb, ddb);
    dpak->map = ddb->dialer_string;
    dpak->holdQ = create_dialer_holdq(ddb);

    if (dialer_place_call(outddb, dpak)) {
	ddb->dialer_fsm = dialer_fsm_pending;
	set_dialer_state(ddb, DIALER_PENDING);
	ddb->profile_dialing_ddb = outddb;
    } else {
	ddb->profile_dialing_ddb = NULL;
    }

    return(NULL);
}

/*
 * dialer_fsm_pending
 * Dialer profile is connecting : attempt to queue packet.
 */
dialerdbtype *dialer_fsm_pending (dialerdbtype *ddb, dialerpaktype *dpak)
{
    paktype *pak;

    pak = dpak->pak;
    dpak->map = ddb->dialer_string;

    if (dpak->interesting) {
	if ((reg_invoke_dialer_pending(ddb->profile_dialing_ddb->dialer,
				       ddb->profile_dialing_ddb, dpak))) {
	    /*
	     * If there is a call pending to this destination, attempt to
	     * queue the packet in the dialer hold queue.  The pending routines
	     * need to set up dpak->holdQ.
	     */
	    dialer_holdq_enqueue(dpak);
	    pak->dialer_flags |= PAK_DDR_CALL_PENDING;
	}
    }
    pak->dialer_flags |= PAK_DDR_CALL_PENDING;

    return(NULL);
}

/*
 * dialer_fsm_up
 * Dialer profile is connected : attempt to transmit packet.
 */
dialerdbtype *dialer_fsm_up (dialerdbtype *ddb, dialerpaktype *dpak)
{
  dialerdbtype *outddb, *tmpddb;
  paktype *pak;
  ulong queuelen;
  sys_timestamp lastoutput;
  int i;

  outddb = NULL;
  queuelen = 0;
  GET_TIMESTAMP(lastoutput);
  if (ddb->conn_group == NULL) {
    if (dialer_debug)
        buginf("\n%s: No bundle in dialer_fsm_up", ddb->ownidb->hw_namestring);
    return(NULL);
  }
  FOR_ALL_MEMBERS_IN_GROUP(ddb->conn_group, i, tmpddb) {
    outddb = pick_best_dialer_idb(tmpddb, outddb, &queuelen, &lastoutput);
  }

  pak = dpak->pak;
  pak->if_output = outddb->ownidb->firstsw;

  if (dpak->interesting) {
    START_IDLE_TIMER(outddb);
  }

  return(outddb);
}

/*
 * is_profile
 * Return TRUE if the idb is configured with a dialer profile
 */
boolean is_profile (hwidbtype *idb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);
    if (ddb != NULL) {
	return(IS_PROFILE(ddb));
    }
    return(FALSE);
}

/*
 * dialpool__member
 * Return TRUE if the idb is a member of a dialer pool
 */
boolean dialpool_member (hwidbtype *idb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);
    if (ddb != NULL) {
	return(DIALPOOL_MEMBER(ddb));
    }
    return(FALSE);
}
