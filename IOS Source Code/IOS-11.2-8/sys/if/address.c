/* $Id: address.c,v 3.3.60.2 1996/04/25 23:12:29 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/if/address.c,v $
 *------------------------------------------------------------------
 * Functions for manipulating addrtype hwaddrtype objects.
 *
 * July 1994, Steve Berl
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * hwaddrtype is defined in h/address.h 
 * This file contains some utility functions for initializing,
 * comparing and copying these objects. It gathered many of these
 * functions from other parts of the code where they were created for
 * some specific useage, and makes them generally available.
 * 
 * If other generally useful functions for dealing with hwaddrtype or
 * addrtype structures make themselves known, feel free to add them to
 * this file.
 *------------------------------------------------------------------
 * $Log: address.c,v $
 * Revision 3.3.60.2  1996/04/25  23:12:29  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.60.1.2.1  1996/03/24  03:55:20  rzagst
 * ATM modularity: Remove AIP from all files that are not referring to the
 * AIP i/f
 * Branch: ATM_Cal_branch
 *
 * Revision 3.3.60.1  1996/03/18  20:12:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  09:43:23  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/01/24  22:17:29  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  09:25:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:36:58  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:47:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/25  22:45:52  sberl
 * CSCdi39340:  Unprotected buginf() output in snpa_init processing
 * Get rid of offending buginf() statements. Convince myself that these
 * can never happen. Change bcopy/bcmp/bzero to use appropriate ANSI C
 * library functions instead.
 *
 * Revision 2.3  1995/08/23  22:50:08  ronnie
 * CSCdi38804:  InARP on ATM interfaces doesnt populate ARP table
 *         Many places were passing a complete addrtype to the hashing
 *         function, when only the first few bytes were set.  This made
 *         the hash function nondeterministic.
 *
 * Revision 2.2  1995/07/26  15:14:13  schrupp
 * CSCdi37690:  Support ATM ESI addressing with ILMI prefix
 *
 * Revision 2.1  1995/06/07  20:49:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "address.h"

static inline uchar snpa_type_to_length (uchar type)
{
    switch (type) {
    case STATION_ILLEGAL:
	return(0);
    case STATION_IEEE48:
	return(STATIONLEN_IEEE48);
    case STATION_IEEE16:
	return(STATIONLEN_IEEE16);
    case STATION_XEROX:
	return(STATIONLEN_XEROX);
    case STATION_X121:
	return(STATIONLEN_X121);
    case STATION_CISCO_HDLC:
	return(STATIONLEN_CISCO_HDLC);
    case STATION_CISCO_MLAPB:
	return(STATIONLEN_CISCO_MLAPB);
    case STATION_LAPB:
	return(STATIONLEN_LAPB);
    case STATION_SMDS48:
	return(STATIONLEN_SMDS48);
    case STATION_CISCO_MSDLC:
	return(STATIONLEN_CISCO_MSDLC);
    case STATION_FR10:
	return(STATIONLEN_FR10);
    case STATION_ULTRA:
	return(STATIONLEN_ULTRA);
    case STATION_CISCO_TUNNEL:
	return(STATIONLEN_CISCO_TUNNEL);
    case STATION_PPP:
	return(STATIONLEN_PPP);
    case STATION_SMDS64:
	return(STATIONLEN_SMDS64);
    case STATION_ATMVC:
	return(STATIONLEN_ATMVC);
    case STATION_ATMNSAP:
	return(STATIONLEN_ATMNSAP);
    case STATION_ATMESI:
	return(STATIONLEN_ATMESI);
    case STATION_SDLC:
	return(STATIONLEN_SDLC);
    case STATION_X25PVC:
	return(STATIONLEN_X25PVC);
    }
    return(0);
}

/*
 * snpa_init
 *
 * Initialize an snpa based on the input parameters.
 */
void snpa_init (hwaddrtype* snpa, 
		SNPAType_e type, 
		SNPATarget_e target_type, 
		uchar* addr)
{
    snpa->type = type;
    snpa->target_type = target_type;
    snpa->length = snpa_type_to_length(type);
    memset(snpa->addr, 0, STATIONLEN_MAX);
    memcpy(snpa->addr, addr, snpa->length);
}

/*
 * snpa_equ
 * Returns TRUE if two SNPAs are equal, FALSE if not.
 */
boolean snpa_equ (hwaddrtype *snpa1, hwaddrtype *snpa2)
{
    if (snpa1->type != snpa2->type)
	return(FALSE);
    if (snpa1->length != snpa2->length)
	return(FALSE);
    if (snpa1->target_type != snpa2->target_type)
	return(FALSE);
    return(!memcmp(snpa1->addr, snpa2->addr, snpa1->length));
}


/*
 * address_equ(addr1, addr2)
 *
 * Returns TRUE if the two addresses are equal, FALSE otherwise
 */
boolean address_equ (addrtype *addr1, addrtype *addr2)
{
    if (addr1 == NULL || addr2 == NULL)
	return FALSE;		/* this should never happen */
    
    if (addr1->type != addr2->type)
	return FALSE;
    if (addr1->length != addr2->length)
	return FALSE;
    return bcmp(&addr1->addr, &addr2->addr, addr1->length) == 0;
}

