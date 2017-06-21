/* $Id: atm_actions.h,v 3.2.60.1 1996/03/18 20:13:12 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/if/atm_actions.h,v $
 *------------------------------------------------------------------
 * Header file for ATM specific parsing functions.
 *
 * November 1993,  Lionel Bustini
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: atm_actions.h,v $
 * Revision 3.2.60.1  1996/03/18  20:13:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  22:17:53  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:25:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:47:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/26  15:14:27  schrupp
 * CSCdi37690:  Support ATM ESI addressing with ILMI prefix
 *
 * Revision 2.1  1995/06/07  20:50:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef atm_actions_h
#define atm_actions_h

uchar hexCharValue(char c);
char *atm_nsap_short_help(parseinfo*, uint, idbtype*);
boolean match_atm_nsap(parseinfo*, char*, int*, void*, int, uint, idbtype*);
char *atm_esi_short_help(parseinfo*, uint, idbtype*);
boolean match_atm_esi(parseinfo*, char*, int*, void*, int, uint, idbtype*);

#define NSAP_E164_FORMAT	0x45	/* E.164 ATM Format */
#define NSAP_START_RD_FIELD	8       /* Start index of the RD field */
#define NSAP_E164_FIELD_SIZE	8	/* Size of E.164 field */

#endif atm_actions_h

