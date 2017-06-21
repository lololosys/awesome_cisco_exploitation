/* $Id: ntp_access.c,v 3.2.62.3 1996/08/19 18:54:41 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_access.c,v $
 *------------------------------------------------------------------
 * NTP access control routines
 *
 * February 1993, Dave Katz
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ntp_access.c,v $
 * Revision 3.2.62.3  1996/08/19  18:54:41  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.2.62.2  1996/05/04  01:25:18  wilber
 * Branch: California_branch
 *
 * Name Access List
 *
 * Revision 3.2.62.1  1996/04/16  19:03:01  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:50:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:44:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CISCO

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "ntpd.h"

static ulong ntp_access_list[NTP_ACC_COUNT];	/* Set of access lists */
static boolean some_restrictions = FALSE; /* True if access lists exist */

static const ulong restrictions[NTP_ACC_COUNT] = {
    /* Corresponding restrictions */
    RES_NOMODIFY,					 /* peer */
    RES_DONTTRUST|RES_NOMODIFY|RES_NOPEER, 		 /* serve */
    RES_DONTTRUST|RES_NOQUERY|RES_NOMODIFY|RES_NOPEER,   /* serve-only */
    RES_DONTSERVE|RES_DONTTRUST|RES_NOMODIFY|RES_NOPEER  /* query-only */
};

/*
 * Update "some_restrictions"
 */
static void update_restrictions(void)
{
    int i;
    for (i=0; i<NTP_ACC_COUNT; i++) {
	if (ntp_access_list[i]) {
	    some_restrictions = TRUE;
	    return;
	}
    }
    some_restrictions = FALSE;
    return;
}

/*
 * ntp access-group {query-only | serve-only | serve | peer} <num>
 * no ntp access-group {query-only | serve-only | serve | peer}
 *
 * OBJ(int,1) = server type
 * OBJ(int,2) = access list number
 */
void ntp_access_command(parseinfo *csb)
{
    /* First nv generation, of course. */

    if (csb->nvgen) {
	if (ntp_running)
	    nv_write((ntp_access_list[GETOBJ(int,1)] != 0), "%s %d",
		     csb->nv_command, ntp_access_list[GETOBJ(int,1)]);
	return;
    }

    /* Fire up NTP. */

    init_ntp_process(NULL);

    /* Update the access list variable.  If "no", OBJ(int,2) will be zero. */

    ntp_access_list[GETOBJ(int,1)] = GETOBJ(int,2);
    update_restrictions();
}

/*
 * Return a set of restriction flags based on the source address of the
 * current packet.
 */
ulong ntp_restrictions(struct sockaddr_in *srcadr)
{
    int i;

    /* No restrictions if there are no access lists. */

    if (!some_restrictions)
	return(0);

    /*
     * Walk the access lists looking for a match.  Return the appropriate
     * restriction bits if we find a match.  If there is no match, return
     * RES_IGNORE.
     */
    for (i=0; i<NTP_ACC_COUNT; i++) {
	if (ntp_access_list[i]) {
	    if (fast_check(ntp_access_list[i], 
			   (ipaddrtype) srcadr->sin_addr.s_addr)) {
		return(restrictions[i]);
	    }
	}
    }
    return(RES_IGNORE);
}
