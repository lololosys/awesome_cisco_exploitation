/* $Id: dialer_callback.c,v 3.1.2.4 1996/08/28 13:21:11 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/dialer_callback.c,v $
 *------------------------------------------------------------------
 * dialer_callback.c - routines to handle callback
 *
 * June, 1996, Bill May (from work by Dana Blair)
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dialer_callback.c,v $
 * Revision 3.1.2.4  1996/08/28  13:21:11  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.3  1996/08/27  01:01:23  wmay
 * CSCdi67232:  dialer callback pending checks not efficient - remember
 * to sanity check strings before comparing them
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/08/27  00:39:44  wmay
 * CSCdi67232:  dialer callback pending checks not efficient
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/07/19  19:50:31  wmay
 * CSCdi60728:  ISDN dialer holdQ not handled correctly - do not allocate
 * holdq until needed, and pass the holdq correctly as calls and callbacks
 * are made.  Redo a little callback code to deal with the holdq better and
 * rename some of the structures.  Add a debug dialer holdq (hidden).
 * Branch: California_branch
 *
 * Revision 3.1  1996/07/19  19:37:17  wmay
 * Placeholder file
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
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
#include "msg_dialer.c"			/* Not a typo, see logger.h */
#include "../if/static_map.h"

/* dialer callback linked list structure */
dqueue_t callback_dqueue;

/*
 * Note - if you use this and you remove the callback you are on,
 * you need to set dq to dq->flink.  Note: we run this backward, so
 * we get the oldest first (when we insert, we insert "after" the
 * callback_dqueue element, so callback_dqueue.blink is the first
 * element put on the list.
 */
#define FOR_ALL_CALLBACKS(dq, callback)					\
        for ((dq) = callback_dqueue.blink, (callback) = (dq)->parent;	\
	     (callback) != NULL;					\
	     (dq) = (dq)->blink, (callback) = (dq)->parent)


/*
 * callback_init
 * Init the callback queue, with no parent (that's how we tell that
 * we've hit the end of the list).
 */
void callback_system_init (void)
{
    o_init(&callback_dqueue, NULL);
}

/*
 * create_callback
 * Create a callback structure, link it into the callback_dqueue, and
 * setup various fields.
 *
 * For clients, this should be called when the interface is disconnected.
 * You should start the callback_timer with a timeout value for the callback
 *
 * For servers, this should be called when the callback is decided on.
 * You need to start the callback_timer with the amount of time to
 * start the callback.
 */
callback_type *create_callback (dialer_holdq_type *holdq,
				enum CALLBACK_TYPE type,
				char *remote_name,
				hwidbtype *idb,
				char *dialstr,
				dialermaptype *pdialermap)
{
    callback_type *callback;
    
    callback = malloc_named(sizeof(callback_type), "callback");
    if (callback) {
	o_init(&callback->dqueue, callback);
	lw_insert(&callback->dqueue, &callback_dqueue);
	callback->holdQ = holdq;
	callback->callback_type = type;
	/* 
	 *  Fill in elements of the holdq list member
	 */
	if (remote_name)
	    strcpy(callback->remote_name, remote_name);
	if (dialstr)
	    strcpy(callback->number_called, dialstr);
	callback->initial_idb = idb;
	callback->pdialermap = pdialermap;
    
    }
    return(callback);
}

/*
 * find_callback_by_dial_string
 * See if there is a callback with the same dial string
 */
callback_type *find_callback_by_dial_string (char *dialstr)
{
    dqueue_t *dq;
    callback_type *call;

    if (null(dialstr))
	return(NULL);
  
    FOR_ALL_CALLBACKS(dq, call) {
	if (strcmp(call->number_called, dialstr) == 0)
	    return(call);
    }
    return(NULL);
}

/*
 * find_callback_by_remote_name
 * See if there is a callback with the same remote name
 */
static callback_type *find_callback_by_remote_name (char *name)
{
  dqueue_t *dq;
  callback_type *call;

  if (null(name))
      return(NULL);
  
  FOR_ALL_CALLBACKS(dq, call) {
      if (strcmp(call->remote_name, name) == 0)
	  return(call);
  }
  return(NULL);
}

/*
 * find_callback_by_call
 * See if the current call for this ddb is in the callback
 */
callback_type *find_callback_by_call (dialerdbtype *ddb)
{
    callback_type *call;

    call = find_callback_by_dial_string(ddb->number_called);

    if (call == NULL)
	call = find_callback_by_remote_name(ddb->remote_name);

    return(call);
}

/*
 * find_callback_by_idb
 * For the server, when the enable timer expires, this routine
 * is called to see if we have a match on the same dialer rotary.
 * we compare net_idbs
 */
callback_type *find_callback_by_idb (hwidbtype *hwidb)
{
    dqueue_t *dq;
    callback_type *call;
    hwidb = get_netidb(hwidb);

    if (hwidb == NULL)
	return(NULL);
  
    FOR_ALL_CALLBACKS(dq, call) {
	if (hwidb == get_netidb(call->initial_idb))
	    return(call);
    }
    return(NULL);
}
    
/*
 * remove_callback
 * Unlink the member, discard the holdQ (so if you transfer the
 * holdQ, don't forget to NULL the pointer), stop the timer and
 * free the callback memory
 */
void remove_callback (callback_type *call)
{
    if (call) {
	if (dialer_debug) 
	    buginf("\nFreeing callback to %s %s",
		   call->remote_name, call->number_called);
	lw_remove(&call->dqueue);
	if (call->holdQ)
	    dialer_holdq_discard_queue(&call->holdQ, "removing callback");
	stop_callback_timer(call);
	free(call);
    }
}

/*
 * callback_pending
 * Go through the dialer map for this call, and see if we have a
 * matching remote name or dial string
 * We're eventually going to want to just check out the dial strings,
 * because we may not have a map in a dpak structure
 */
callback_type  *callback_pending (dialerpaktype *dpak)
{
    dialermaptype *map;
    dialerentrytype *e;
    callback_type *call;
    dqueue_t *dq;

    map = dpak->map;
    if (map == NULL)
	return(NULL);

    FOR_ALL_CALLBACKS(dq, call) {
	if (map == call->pdialermap) {
	    return(call);
	}
	for (e = map->entry; e != NULL; e = e->next) {
	    if ((!(null(e->dialer_string))) &&
		(strcmp(e->dialer_string, call->number_called) == 0))
		return(call);
	    if ((!(null(e->hostname))) &&
		((strcmp(e->hostname, call->remote_name) == 0) ||
		 (strcmp(e->hostname, call->number_called) == 0)))
		return(call);
	}
    }
    return(NULL);
}



/*
 * callback_start
 * entry point for callback option that need dialer.
 * boolean = TRUE callback process start
 *           FALSE continue with no callback (ie. ncp negotiation)
 * idb -> hwidbtype
 * info -> username information
 * neg_result = boolean of callback negotiation result
 *
 * This is called through a registry when we're done negotiating callback.
 * It is called if callback is configured, and not only if negotiation is
 * successful.
 */
boolean callback_start (hwidbtype * idb,
			usernametype * info,
			boolean neg_result,
			char *dialstr)
{
    dialerdbtype *ddb;
    enum CALLBACK_TYPE type;
    dialerentrytype *pdialerentry;
    dialermaptype *pdialermap;
    mapclass_type * pmapclass;
    dialer_params_type * pparams;
    boolean server_user;
    boolean server_dial;
    boolean is_dial_str;
    boolean is_user_dial_str;
    dialer_holdq_type *new_holdq;
    callback_type *call;

    pdialerentry = NULL;
    pdialermap = NULL;
    pmapclass = NULL;
    pparams = NULL;
    server_user = FALSE;
    server_dial = FALSE;
    is_dial_str = FALSE;
    is_user_dial_str = FALSE;

    ddb = get_dialerdb(idb);
    if (ddb == NULL)
	return(FALSE);

    /*
     *  Router is operating as a callback client
     */
    
    if (!(reg_invoke_dialer_is_answer(ddb->dialer, ddb))) {
	type = CALLBACK_CLIENT;
	/* 
	 * client always proceeds as if callback was not negotiated.
	 * This allows the callback server the freedom to maintain the
	 * existing call or disconnect and make a return call.
	 */  
	if (!neg_result) {
	    if (dialer_debug) {
		buginf("\n%s: No callback negotiated", idb->hw_namestring);
	    }
	    
	    return(FALSE);
	}
	
	if (ddb->remote_name[0] == '\0') {
	    if (dialer_debug)
		buginf("\n: Authentication required for callback");
	    return(FALSE);
	    
	}
	
	/*
	 * Just indicate that we are expecting callback - don't
	 * create a structure yet
	 */
	ddb->callback_type = CALLBACK_CLIENT;
	
	if (dialer_debug) {
	    buginf("\n%s: Callback negotiated - waiting for server disconnect",
		   idb->hw_namestring);
	}

	return(FALSE);
    }
 
    /*
     *  Router is operating as a callback server
     */
    
    type = CALLBACK_SERVER;

    if (!neg_result) {
	if (dialer_debug) {
	    buginf("\n%s: No callback negotiated",
		   idb->hw_namestring);
	}
	if (ddb->callback_secure) {
	    dialer_disconnect(idb);
	    return(TRUE);
	} else {
	    return(FALSE);
	}
    }
    
    if (!strlen(ddb->remote_name)) {
	if (dialer_debug)
	    buginf("\n: Authentication required for callback");
	if (ddb->callback_secure) {
	    dialer_disconnect(idb); 
	    return(TRUE);
	} else {
	    return(FALSE);
	}
    }

    call = find_callback_by_remote_name(ddb->remote_name);
    
    if (call) {
	if (dialer_debug)
	    buginf("\n%s: Callback already started for %s",
		   idb->hw_namestring, ddb->remote_name);
	dialer_disconnect(idb); /* disconnect may clear remote_name */
	return(TRUE);
    }    
    
    pdialerentry = dialer_name_lookup(ddb, ddb->remote_name);
    
    if (pdialerentry) {
	is_user_dial_str = (boolean)strlen(pdialerentry->dialer_string);
	pdialermap = pdialerentry->head;
	if (pdialermap) {
	    pmapclass = mapclass_lookup(pdialerentry->class, ET_DIALER);
	    if (pmapclass) {
		if (pmapclass->params) {
		    pparams = (dialer_params_type *)pmapclass->params;
		    server_user = pparams->callback_server_user;
		    server_dial = pparams->callback_server_dial;
		}
	    }
	}
    }   
    
    if (info)
	is_dial_str = (boolean)strlen(dialstr);
    
    /*
     *  Determine if callback is configured for the remote user
     *
     *  IF not configured for remote user AND callback-secure
     *    disconnect and prevent call from proceeding as if without callback
     *  ELSE IF not configure for remote user
     *   return and allow call to proceed as if without callback
     */
    
    if ((!server_user && !server_dial) ||
	( server_user && !server_dial && !is_user_dial_str) ||
	(!server_user &&  server_dial && !is_dial_str) ||
	( server_user &&  server_dial && !is_dial_str && !is_user_dial_str)) {
	if (dialer_debug)
	    buginf("\n%s: No callback configured for user %s.",
		   idb->hw_namestring, ddb->remote_name);
	if (ddb->callback_secure) {
	    dialer_disconnect(idb); /* disconnect may clear remote_name */
	    return(TRUE);
	}
	else
	    return(FALSE);
    }
    
    /*
     * Create a holdq, if needed
     */
    new_holdq = create_dialer_holdq(ddb);

    /*
     * Create a callback structure - this is used to actually do the
     * callback.  It also acts to catch any packets to stop premature
     * dialing.
     */
    call = create_callback(new_holdq,
			   type,
			   ddb->remote_name,
			   idb,
			   /* Only use callback dialstring option
			    * if not secure security.
			    */
			   (server_dial && is_dial_str &&
			    !ddb->callback_secure) ? dialstr :
			   pdialerentry->dialer_string,
			   pdialermap);
			   
    if (!call) {
	if (ddb->callback_secure) {
	    dialer_holdq_free(&new_holdq);
	    dialer_disconnect(idb); /* disconnect may clear remote_name */
	    return(TRUE);
	} else {
	    return(FALSE);
	}
    }
    
    dialer_holdq_timer_start(new_holdq, ddb->dialer_holdq_ticks);
    if (dialer_debug)
	buginf("\n%s:Callback server starting to %s %s",
	       ddb->ownidb->hw_namestring, call->remote_name,
	       call->number_called);
    
    dialer_disconnect(idb); /* disconnect may clear remote_name */

    /*
     * Start the timer for callback.  If we are a single serial doing
     * callback, we need to wait for the enable timer, but we take care
     * of that down below
     */
    start_callback_timer(call, ddb->dialer_enable_ticks);
    
    return(TRUE);
}

/*
 * callback_dial
 * Perform the callback.  Either way, return the callback structure at the
 * end. 
 */
void callback_dial (callback_type *call) 
{	
    dialerdbtype *ddb;
    dialerpaktype dpak;
    
    ddb = get_dialerdb(call->initial_idb);

    if (ddb != NULL) {
	ddb = GET_NETDDB(ddb);
	if (dialer_debug) 
	    buginf("\n%s:beginning callback to %s %s",
		   ddb->ownidb->hw_namestring, call->remote_name,
		   call->number_called);
	ddb = reg_invoke_get_free_dialer(ddb->dialer, ddb);
	
	if (ddb) {  
	    bzero(&dpak, sizeof(dialerpaktype));
	    dpak.map = call->pdialermap;
	    dpak.header = NULL;
	    dpak.holdQ = call->holdQ;
	    dpak.ddb = GET_NETDDB(ddb);
	    call->holdQ = NULL;
	    /* we will take the holdq from the callback structure here */
	    setstring(&dpak.header, "Callback return call");
	    if (!(dial_if(ddb, &dpak))) {
		if (dialer_debug)
		    buginf("\n: unable to dial callback return call");
	    }
	} else {
	    if (dialer_debug)
		buginf("\n: No interface available for callback to %s %s",
		       call->remote_name, call->number_called);
	}
    }

    remove_callback(call);
}

/*
 * Callback_onesec
 * one second processing of callback timers
 */
void callback_onesec (void)
{
    callback_type *call;
    dqueue_t *dq;
    dialerdbtype *ddb;

    FOR_ALL_CALLBACKS(dq, call) {
	if ((call->holdQ) &&
	    dialer_holdq_timer_running_and_awake(call->holdQ)) {
	    dialer_holdq_discard_queue(&call->holdQ,
				       "callback holdq timeout");
	}
	
	if (TIMER_RUNNING_AND_AWAKE(call->callback_timer)) {
	    /*
	     *  Only use the callback timer if the enable timer is not started
	     *    The enable timer is only started for dialer in-band Async or
	     *    Sync that are not part of a dialer rotary-group
	     */
	    ddb = get_dialerdb(call->initial_idb);
	    if (!((HUNTGROUP_LEADER(ddb)) || (HUNTGROUP_MEMBER(ddb)))) {
		/*
		 * We are a single member of a dialer group, and our
		 * enable timer has not expired yet.  We don't want
		 * to start the callback until the enable timer has
		 * finished.  Other than that, fall through.
		 */
		if (TIMER_RUNNING(ddb->dialer_enable_timer)) {
		    TIMER_STOP(call->callback_timer);
		    continue;
		}
	    }
	    /*
	     * Timer will get stopped since either case removes the
	     * callback.  In both cases, since the callback gets removed,
	     * set the pointer to the linked list to the previous
	     * callback structure.
	     */
	    dq = dq->flink;
	    if (call->callback_type == CALLBACK_SERVER) {
		/*
		 * callback server case - Make the return call
		 */
		if (dialer_debug)
		    buginf("\n: Callback timer expired");

		callback_dial(call);
	    } else {
		/*
		 * Callback client case - they have not called back.  We
		 * will remove the callback structure, and let nature
		 * take its course
		 */
		if (dialer_debug)
		    buginf("\nNo callback received from %s %s",
			   call->remote_name, call->number_called);
		remove_callback(call);
	    }
	}
    }
}

