/* $Id: rsvp_debug.c,v 1.1.4.2 1996/08/09 00:18:06 asastry Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/rsvp_debug.c,v $
 *------------------------------------------------------------------
 * Resource Reservation Protocol
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsvp_debug.c,v $
 * Revision 1.1.4.2  1996/08/09  00:18:06  asastry
 * CSCdi65504:  Allow <debug ip rsvp detail> command to be RESV-only or
 * PATH-only
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/04/17  13:57:18  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "../rsvp/rsvp_debug.h"

/*
 * Declare the initialized text for the debugging array.
 */
#define __DECLARE_DEBUG_ARR__
#include "../rsvp/rsvp_debug_flags.h"

/*
 * Use this variable to restrict RSVP debug info via an access control list.
 */
int rsvp_acl = 0;

/*
 * Boolean to indicate if we need more detailed output (debug ip rsvp detail).
 */
int rsvp_detail_flag;
boolean rsvp_special_flag;

/*
 * rsvp_debug_commands
 */
void
rsvp_debug_commands (parseinfo * csb)
{
    rsvp_debug = csb->sense;
    rsvp_acl = GETOBJ(int, 4);
    rsvp_detail_flag = 0;
    rsvp_special_flag = 0;
    /*
     * Support for 'debug ip rsvp [detail [resv | path] [acl]]
     * 
     * See if user specified 'resv' or 'path' after the 'detail'
     * keyword. If neither was specified, only GETOBJ(int,1)
     * will be TRUE (i.e. display detailed debug info for both RESV
     * and PATH messages).
     */

    if (GETOBJ(int,2)) {
	rsvp_detail_flag = RSVP_RESV_DEBUG_FLAG;
    } else if (GETOBJ(int,3)) {
	rsvp_detail_flag = RSVP_PATH_DEBUG_FLAG;
    } else if (GETOBJ(int,1)) {
        rsvp_detail_flag = RSVP_DEBUG_ENABLE;
    }
    if (GETOBJ(int,5)) {
	/* 
	 * If hidden keyword 'special' was specified, turn off all RSVP
	 * debugging, but allow for special debugging to be enabled.
	 */
        rsvp_special_flag = 1;
	rsvp_debug = 0;
    }
    debug_show_flag(rsvp_debug, TRUE, "RSVP");
}
/*
 * rsvp_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the IP multicast routing debug flags at once. The
 * argument is TRUE for "debug all", FALSE for "undebug all".
 */
void
rsvp_debug_all (boolean flag)
{
    if(!flag) {
	rsvp_debug = FALSE;
	rsvp_detail_flag = 0;
        rsvp_special_flag = 0;
	rsvp_acl = 0;
    }
}
/*
 * rsvp_debug_show
 *
 * Called to display the values of all the IP multicast debugging variables.
 */
void
rsvp_debug_show (void)
{
    if(!rsvp_debug)
	return;

    printf("\nIP RSVP debugging");
    if (rsvp_detail_flag == RSVP_RESV_DEBUG_FLAG)
	printf(" (detailed, RESV messages only) is on");
    else if (rsvp_detail_flag == RSVP_PATH_DEBUG_FLAG)
	printf(" (detailed, PATH messages only) is on");
    else if (rsvp_detail_flag == RSVP_DEBUG_ENABLE)
	printf(" (detailed) is on");
    else
	printf(" is on");
    if (rsvp_acl)
	printf(" for access list %d", rsvp_acl); 
}
/*
 * rsvp_debug_init
 *
 * Initialize IP RSVP debugging variables.
 */
void
rsvp_debug_init (void)
{
    reg_add_debug_all(rsvp_debug_all, "rsvp_debug_all");
    reg_add_debug_show(rsvp_debug_show, "rsvp_debug_show");

    rsvp_debug_all(debug_all_p());
}

