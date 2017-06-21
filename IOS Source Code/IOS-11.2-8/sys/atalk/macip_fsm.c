/* $Id: macip_fsm.c,v 3.2.60.3 1996/07/10 22:19:28 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/macip_fsm.c,v $
 *------------------------------------------------------------------
 * macip_fsm.c  - macip servers state machine implementation.
 *
 * September 1991, Russ Tremain, Tremain Laboratories, under contract
 *  for cisco Systems, Inc.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *
 *===================================================================
 * NOTES:
 *  - "internally-generated" events have priority over "external" events
 *    i.e., internal events are jammed on the head of the state event queue.
 *    this is to allow for externally-generated event "sequences"
 *    without fouling up the machine.
 *
 *  - a single time-out delay event is allowed for each state.
 *    the delay interval can be inherited from a previous state,
 *    deactivated (set to zero), or set to some arbitrary value.
 *    multi-level timeouts can be implemented by adding new states.
 *
 *------------------------------------------------------------------
 * $Log: macip_fsm.c,v $
 * Revision 3.2.60.3  1996/07/10  22:19:28  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/03/23  01:28:28  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2.60.1  1996/03/18  18:53:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/02/20  13:34:30  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:44:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:51:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/13  06:50:25  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.1  1995/06/07 20:10:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * macip_fsm.c  - macip servers state machine implementation.
 *
 * NOTES:
 *  - "internally-generated" events have priority over "external" events
 *    i.e., internal events are jammed on the head of the state event queue.
 *    this is to allow for externally-generated event "sequences"
 *    without fouling up the machine.
 *
 *  - a single time-out delay event is allowed for each state.
 *    the delay interval can be inherited from a previous state,
 *    deactivated (set to zero), or set to some arbitrary value.
 *    multi-level timeouts can be implemented by adding new states.
 *
 */

#include "master.h"
#include <interface.h>
#include "packet.h"
#include "string.h"
#include "atalk_private.h"
#include "macip_util.h"
#include "atalk_nbp.h"
#include "macip_db.h"
#include "macip_fsm.h"
#include "macip_server.h"
#include "../atalk/atip_debug.h"

/* definitions for at_err(): */
#define DEBUGSWITCH (at_debug(appletalk_debug, ATALK_NULL_MASK))

#define STATE_TRACE 22

/* definition of FSM table entry: */
typedef struct {
    PFV curr;
    macipFsmEvent event;
    PFV next;
} fsm_table;


/*************************  PRIVATE FUNCTIONS  ************************/

/*
 * initial -
 *	
 *	initial   | MACIP_FSM_ADD_SERVER  | resource_wait | config "server" statement
 *	
 */
static void
initial (server_rec *ss)
{
    /* can only be called from "*" states */
}

/*
 * resource_wait -
 *	
 *	resource_wait | MACIP_FSM_TIMEOUT | resource_wait | warning after timeout
 *	resource_wait | MACIP_FSM_ADD_RESOURCE    | zone_wait | wait for zone seeding
 *	
 */
static void
resource_wait (server_rec *ss)
{
   
    if (at_debug(atalkmacip_debug,0))
        buginf("\nAT-MacIP: state resource_wait %lx", ss);

    /* set timer delay on first invocation: */
    if (!TIMER_RUNNING(ss->timer))
        MACIP_SET_DELAY(ss, RESOURCE_WAIT_TIME);
}

/*
 * zone_wait -
 *	
 *	zone_wait | MACIP_FSM_ZONE_SEEDED | server_start  | register server
 *	zone_wait | MACIP_FSM_TIMEOUT | zone_wait | wait until seeded
 *	zone_wait | MACIP_FSM_ADD_RESOURCE    | zone_wait | ignore
 *	
 */
static void
zone_wait (server_rec *ss)
{
    if (at_debug(atalkmacip_debug,0))
        buginf("\nAT-MacIP: state zone_wait %lx", ss);

    /* set timer delay on first invocation: */
    if (!TIMER_RUNNING(ss->timer))
        MACIP_SET_DELAY(ss, ZONE_WAIT_TIME);

    if (macip_zone_seeded(ss))
        macip_jam_event(ss,MACIP_FSM_ZONE_SEEDED,0);
}

/*
 * server_start -
 *	
 *	server_start  | MACIP_FSM_START_OK    | reg_wait  | wait for server reg
 *	server_start  | MACIP_FSM_START_FAIL  | config_wait| couldn't start (config err)
 *	
 */
static void
server_start (server_rec *ss)
{
    if (at_debug(atalkmacip_debug,0))
        buginf("\nAT-MacIP: state server_start %lx", ss);

    /* set timer delay on first invocation: */
    if (!TIMER_RUNNING(ss->timer))
        MACIP_NO_TIMEOUT(ss);

    /* start server registration, open client listen socket: */
    if (macip_start_server(ss))
        macip_jam_event(ss,MACIP_FSM_START_OK,0);
    else {
	/* log message: */
	errmsg(AT_ERR_MACIPBADCONF, at_hex_display(SY_STRING(ss->zone)),
		ss->ipaddr);

        macip_jam_event(ss,MACIP_FSM_START_FAIL,0);
    }
}

/*
 * reg_wait -
 *	
 *	reg_wait  | MACIP_FSM_REG_OK  | server_up | registration successful
 *	reg_wait  | MZCIP_FSM_ZONE_DOWN| zone_wait| interface down, try later
 *	reg_wait  | MACIP_FSM_REG_FAIL | config_wait| reg. failed (dup IP?)
 *	reg_wait  | MACIP_FSM_TIMEOUT | reg_wait  | wait until register
 *	
 */
static void
reg_wait (server_rec *ss)
{
    nbpEntity nve;
    nbp_result astatus, gstatus;

    if (at_debug(atalkmacip_debug,0))
        buginf("\nAT-MacIP: state reg_wait %lx", ss);

    /* set timer on first invocation: */
    if (!TIMER_RUNNING(ss->timer))
        MACIP_SET_DELAY(ss, SERVER_REG_WAIT);

    atnbp_mkname(&nve, ipaddrStr(ss->ipaddr), "IPGATEWAY",
		 SY_STRING(ss->zone));
    gstatus = atnbp_isregistered(&nve);

    atnbp_mkname(&nve,
		 ipaddrStr(ss->ipaddr), "IPADDRESS", SY_STRING(ss->zone));
    astatus = atnbp_isregistered(&nve);

    if (gstatus == NBP_NOT_REGISTERED || astatus == NBP_NOT_REGISTERED) {
	/* if interface is still running... */
        if (atalkif_operational(ss->ddpidb)) {
	    /* ...then registration error, probably duplicate */
	    macip_jam_event(ss,MACIP_FSM_REG_FAIL,0);
	}
        else {
	    /* interface down, try again later: */
	    macip_jam_event(ss,MACIP_FSM_ZONE_DOWN,0);
        }

        return;
    }

    if (gstatus == NBP_REG_PENDING || astatus == NBP_REG_PENDING)
        return;     /* reg. still pending */

    if (gstatus != NBP_OK || astatus != NBP_OK) {
        /* programmer error! */
        at_err(MODID,"macip_fsm: reg_wait:  assertion failed");
        macip_jam_event(ss,MACIP_FSM_REG_FAIL,0);
        return;
    }

    /* start client search: */
    if (!macip_client_search(ss)) {
        macip_jam_event(ss,MACIP_FSM_REG_FAIL,0);
        return;
    }

    /* success! */
    macip_jam_event(ss,MACIP_FSM_REG_OK,0);
}

static void
config_wait(server_rec *ss)
/*
 *	| config_wait	| MACIP_FSM_TIMEOUT | server_start | config corrected?
 */
{

    if (at_debug(atalkmacip_debug,0))
        buginf("\nAT-MacIP: state config_wait %lx", ss);

    /* set timer on first invocation: */
    if (!TIMER_RUNNING(ss->timer))
        MACIP_SET_DELAY(ss, CONFIG_WAIT_TIME);
}

/*
 * server_up -
 *	
 *	server_up | MACIP_FSM_TIMEOUT | send_confirms | nbp_confirm all clients
 *	
 */
static void
server_up (server_rec *ss)
{

    if (at_debug(atalkmacip_debug,0))
        buginf("\nAT-MacIP: state server_up %lx", ss);

    /* set timer delay on first invocation: */
    if (!TIMER_RUNNING(ss->timer))
        MACIP_SET_DELAY(ss, NBP_CONFIRM_TIME);
    
    /* this is a running server.  process client requests,
     * responses from confirms:
     */
    macip_client_service(ss);
}

/*
 * send_confirms -
 *	
 * send_confirms | MACIP_FSM_CONFIRM_OK | server_up | nbp_confirm all clients
 * send_confirms | ZONE_DOWN		| zone_wait | server zone down, restart
 *	
 */
static void
send_confirms (server_rec *ss)
{
    
    if (at_debug(atalkmacip_debug,0))
        buginf("\nAT-MacIP: state send_confirms %lx", ss);

    /* set timer delay on first invocation: */
    if (!TIMER_RUNNING(ss->timer))
        MACIP_NO_TIMEOUT(ss);

    /* if IP interface has gone away... */
    if (macip_ip_down(ss)) {
        macip_stop_server(ss);
        macip_jam_event(ss,MACIP_FSM_ZONE_DOWN,0);
    }
    else /* check our DDP interface, zone configuration: */
    if (macip_ddp_down(ss)) {
	/* our ddp port or zone is down, re-register when zone comes back: */
        macip_stop_server(ss);
        macip_jam_event(ss,MACIP_FSM_ZONE_DOWN,0);
    }
    else {
	/* send out confirm requests: */
	macip_confirm_clients(ss);
	macip_jam_event(ss,MACIP_FSM_CONFIRM_OK,0);
    }
}

/*
 * del_server -
 *	
 *	this is the FINAL state transition.  once server
 *	is deleted from database, state machine for this server
 *	no longer operates.
 *	
 */
static void
del_server (server_rec *ss)
{
    if (at_debug(atalkmacip_debug,0))
        buginf("\nAT-MacIP: state del_server %lx", ss);

    /* set timer delay on first invocation: */
    if (!TIMER_RUNNING(ss->timer))
        MACIP_NO_TIMEOUT(ss);

    /* halt operation: */
    macip_stop_server(ss);

    /* flush any pending events: */
    macip_clear_stateQ(ss);

    /* delete from database: */
    mdb_purge_server(ss);
}

/*
 * ck_resource -
 *	
 *	ck_resource   | MACIP_FSM_YES_RESOURCES   | *     | return to previous state
 *	ck_resource   | MACIP_FSM_NO_RESOURCES    | resource_wait | shutdown, wait for resources
 *	
 */
static void
ck_resource (server_rec *ss)
{
    if (at_debug(atalkmacip_debug,0))
        buginf("\nAT-MacIP: state ck_resource %lx", ss);

    /* set timer on first invocation: */
    if (!TIMER_RUNNING(ss->timer))
        MACIP_NO_TIMEOUT(ss);

    if (mdb_no_resources(ss)) {
        macip_stop_server(ss);
        macip_jam_event(ss,MACIP_FSM_NO_RESOURCES,0);
    }
    else
        macip_jam_event(ss,MACIP_FSM_YES_RESOURCES,0);
}

static fsm_table const stab[] = {
    { server_up, MACIP_FSM_TIMEOUT, send_confirms },
    { send_confirms, MACIP_FSM_CONFIRM_OK, server_up },
    { send_confirms, MACIP_FSM_ZONE_DOWN, zone_wait },
    { initial, MACIP_FSM_ADD_SERVER, resource_wait },
    { resource_wait, MACIP_FSM_TIMEOUT, resource_wait },
    { resource_wait, MACIP_FSM_ADD_RESOURCE, zone_wait },
    { zone_wait, MACIP_FSM_ZONE_SEEDED, server_start },
    { zone_wait, MACIP_FSM_TIMEOUT, zone_wait },
    { config_wait, MACIP_FSM_TIMEOUT, server_start },
    { server_start, MACIP_FSM_START_OK, reg_wait },
    { server_start, MACIP_FSM_START_FAIL, config_wait },
    { reg_wait, MACIP_FSM_REG_OK, server_up },
    { reg_wait, MACIP_FSM_REG_FAIL, config_wait },
    { reg_wait, MACIP_FSM_ZONE_DOWN, zone_wait },
    { reg_wait, MACIP_FSM_TIMEOUT, reg_wait },
    { NILPFV, MACIP_FSM_ADD_RESOURCE, NILPFV },
    { NILPFV, MACIP_FSM_DEL_SERVER, del_server },
    { NILPFV, MACIP_FSM_DEL_RESOURCE, ck_resource },
    { ck_resource, MACIP_FSM_YES_RESOURCES, NILPFV},
    { ck_resource, MACIP_FSM_NO_RESOURCES, resource_wait },
    { NILPFV, MACIP_FSM_NILEVENT, NILPFV },
};


/*************************  EXPORTED FUNCTIONS  ***********************/

/*
 * macip_halt_state -
 *	true if this is the halt state
 */
boolean
macip_halt_state (PFV sfunc)
{
    return(sfunc == del_server);
}

/*
 * macip_jam_event -
 *	
 *	add a state transition event to the FRONT of the event queue.
 *	
 */
void
macip_jam_event (server_rec *ss, macipFsmEvent ee, ulong arg)
{
    macipServerEvent *tm;

    /* allocate and enqueue an event record: */
    tm = macip_get_server_event();
    if (tm != NULL) {
        tm->event = ee;
        tm->arg = arg;
        requeue(&ss->stateQ, tm);
    }
}

/*
 * macip_add_event -
 *	
 *	add a state transition event to the END of the event queue,
 *	execute event queue.
 *	
 */
void
macip_add_event (server_rec *ss, macipFsmEvent ee, ulong arg)
{
    macipServerEvent *tm;

    /* allocate and enqueue an event record: */
    tm = macip_get_server_event();
    if (tm != NULL) {
        tm->event = ee;
        tm->arg = arg;
        enqueue(&ss->stateQ, tm);
    }
}

/*
 * macip_next_state -
 *	
 *	lookup next state in <stab> based on current state <ss> and event <ee>
 *	
 */
PFV
macip_next_state (server_rec *ss, macipFsmEvent ee)
{
    register int ii;

    /* first, look for "fully qualified" state transition: */
    for (ii=0; stab[ii].event != MACIP_FSM_NILEVENT; ii++) {
        if (stab[ii].curr == ss->stateFunc && stab[ii].event == ee) {
            /* if "to previous" transition... */
            if (stab[ii].next == NILPFV) {
                PFV tmpfunc = ss->prevStateFunc;

                ss->prevStateFunc = initial;
                /* ... then return previous state: */
                return(tmpfunc);
            }
            else    /* "normal" state change: */
                return(stab[ii].next);
        }
    }

    /* not found, look for "from any state" transition: */
    for (ii=0; stab[ii].event != MACIP_FSM_NILEVENT; ii++) {
        if (stab[ii].curr == NILPFV && stab[ii].event == ee) {
            if (stab[ii].next == NILPFV)
                return(ss->stateFunc);  /* ignore */
            else {
                /* save current state to return to: */
                ss->prevStateFunc = ss->stateFunc;
                return(stab[ii].next);
            }
        }
    }

    at_err(MODID,"illegal state transition event, %s(%s)",
        macip_stateStr(ss->stateFunc),
        macip_eventStr(ee));

    /* return current state: */
    return(ss->stateFunc);
}

/*
 * macip_init_state -
 *	initialize state machine and related variables.
 */
void
macip_init_state (server_rec *ss)
{
    ss->reqQ = create_watched_queue("MacIP Server Request packets", 0, 0);
    ss->confirmQ = create_watched_queue("MacIP Server Confirm packets", 0, 0);
    queue_init(&ss->stateQ, 0); /* state event queue */
    ss->stateFunc = initial;    /* current state routine */
    ss->prevStateFunc = initial;    /* for "any state" events */
    ss->timeout = FALSE;        /* TRUE if MACIP_FSM_TIMEOUT event */
    TIMER_STOP(ss->timer);      /* timer */
    ss->delay = 0;          /* delay before MACIP_FSM_TIMEOUT */
}

/*
 * macip_eventStr -
 *	state machine and related variables.
 */
char *
macip_eventStr (macipFsmEvent ee)
{
    switch(ee) {
    case MACIP_FSM_ADD_SERVER:      return("MACIP_FSM_ADD_SERVER");
    case MACIP_FSM_TIMEOUT:     return("MACIP_FSM_TIMEOUT");
    case MACIP_FSM_ADD_RESOURCE:        return("MACIP_FSM_ADD_RESOURCE");
    case MACIP_FSM_ZONE_SEEDED:     return("MACIP_FSM_ZONE_SEEDED");
    case MACIP_FSM_ZONE_DOWN:     return("MACIP_FSM_ZONE_DOWN");
    case MACIP_FSM_START_OK:        return("MACIP_FSM_START_OK");
    case MACIP_FSM_START_FAIL:      return("MACIP_FSM_START_FAIL");
    case MACIP_FSM_REG_OK:      return("MACIP_FSM_REG_OK");
    case MACIP_FSM_REG_FAIL:        return("MACIP_FSM_REG_FAIL");
    case MACIP_FSM_CONFIRM_OK:      return("MACIP_FSM_CONFIRM_OK");
    case MACIP_FSM_DEL_SERVER:      return("MACIP_FSM_DEL_SERVER");
    case MACIP_FSM_DEL_RESOURCE:        return("MACIP_FSM_DEL_RESOURCE");
    case MACIP_FSM_YES_RESOURCES:       return("MACIP_FSM_YES_RESOURCES");
    case MACIP_FSM_NO_RESOURCES:        return("MACIP_FSM_NO_RESOURCES");
    default:        return("macip_eventStr:  bad arg");
    }
}

/*
 * macip_stateStr -
 *	machine and related variables.
 */
char *
macip_stateStr (PFV func)
{
    if (func == initial)
        return("initial");
    else if (func == resource_wait)
        return("resource_wait");
    else if (func == zone_wait)
        return("zone_wait");
    else if (func == server_start)
        return("server_start");
    else if (func == config_wait)
	return("config_wait");
    else if (func == reg_wait)
        return("reg_wait");
    else if (func == del_server)
        return("del_server");
    else if (func == server_up)
        return("server_up");
    else if (func == send_confirms)
        return("send_confirms");
    else if (func == ck_resource)
        return("ck_resource");
    else if (func == NILPFV)
        return("*");
    else    
        return("macip_stateStr:  bad arg");
}
