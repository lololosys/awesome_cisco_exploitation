/* $Id: atm_actions.c,v 3.3.44.4 1996/08/26 23:40:42 cakyol Exp $
 * $Source: /release/112/cvs/Xsys/if/atm_actions.c,v $
 *------------------------------------------------------------------
 * ATM specific parsing functions.
 *
 * November 1993,  Lionel Bustini 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: atm_actions.c,v $
 * Revision 3.3.44.4  1996/08/26  23:40:42  cakyol
 * CSCdi67024:  E.164 atm addresses should NOT be expanded
 * Branch: California_branch
 *
 * Revision 3.3.44.3  1996/05/09  14:29:39  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.44.2.2.1  1996/04/27  07:03:30  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.44.2  1996/04/25  23:12:53  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.44.1.8.1  1996/04/16  02:43:37  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.44.1  1996/03/18  20:13:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.2  1996/03/07  09:43:39  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/01/24  22:17:50  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/12/17  18:28:41  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:25:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:47:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/26  15:14:26  schrupp
 * CSCdi37690:  Support ATM ESI addressing with ILMI prefix
 *
 * Revision 2.1  1995/06/07  20:50:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "config.h"
#include "interface_private.h"
#include "atm_actions.h"

#include "atm.h"

uchar hexCharValue (char c)
{
    if ((c<='9') && (c>='0')) return c-'0';
    if ((c<='F') && (c>='A')) return c-'A'+10;
    if ((c<='f') && (c>='a')) return c-'a'+10;
    return 0;
}

char *atm_nsap_short_help (parseinfo *csb, uint flag, idbtype *idb)
{ return (""); }

boolean match_atm_nsap  (parseinfo *csb, char *buf, int *pi,
			 void *addr_pointer, int in_help, uint flag,
			 idbtype *idb)
{
    uchar addr[STATIONLEN_ATMNSAP];
    char c;
    int i, j, k;
    hwaddrtype *hwaddrp = (hwaddrtype *)addr_pointer;


    /*
     * ATM NSAP is 40 octets of hex structured as described above.
     */

    /*
     * Read first two hex digits
     */
    k = 0;
    c = toupper(buf[(*pi)]);
    if (!isxdigit(c))
       return (FALSE);
    addr[k] = hexCharValue(c) << 4;
    c = toupper(buf[++(*pi)]);
    if (!isxdigit(c))
       return (FALSE);
    addr[k] |= (hexCharValue(c) & 0x0f);

/* do NOT treat the E.164 address specially */
#undef SPECIAL_E164
#ifdef SPECIAL_E164
	/*
	 * Determine whether the address is E.164 format based on the
	 * first byte (AFI field).  If it is we parse the first 9 bytes
	 * (18 hex digits) differently than the DCC and ICD ATM formats.
	 */ 
    if ( addr[k] == NSAP_E164_FORMAT ) {
	
	boolean is_e164_fmt = FALSE;
	boolean odd = FALSE;
	int bcnt, pad;
	uchar tmp[STATIONLEN_ATMNSAP];

	/* printf("Parsing for E.164 format since AFI = 0x%x\n\n", addr[k] ); */

	is_e164_fmt = TRUE;

	/* Read the next digit; it must be a '.' */
	c = toupper(buf[++(*pi)]);
	if (c == '.')
	    c = toupper(buf[++(*pi)]);
	else
	    return (FALSE);		/* skip non-optional '.' */

	if (!isxdigit(c))
	    return (FALSE);

	    /* move E.164 addr into tmp[] */
	i = 0;
	j = 0;
	while (j < STATIONLEN_ATMNSAP) tmp [j++] = 0;

	while (isxdigit (c)) 
	{
	    tmp [i++] = hexCharValue (c);
	    odd = TRUE;
	    c = toupper (buf [++(*pi)]);
	    if (c == '.') break;
	/* these can have at most 15 digits in the E.164 field */
	    if (i >= 15) break;
	    if (!isxdigit (c)) return FALSE;
	    tmp [i++] = hexCharValue (c);
	    odd = FALSE;
	    c = toupper (buf [++(*pi)]);
	/* these can have at most 15 digits in the E.164 field */
	    if (i >= 15) break;
	}

	/* set bcnt and append 0x0f to E.164 addr */
	if (c == '.') 
	{
	    tmp [i] = 0x0f;
	    bcnt = i;
	}
	else
	    return FALSE;	/* must have been another non-hex digit */

	/*
	 * If the number of digits in the E.164 phone # is odd, we parse
	 * differently than if it's even so that the address can be packed
	 * correctly.  Note that for an odd number of digits entered the 
	 * actual address will be even, since we append an 0xf to conform to
	 * standard E.164.
	 */
	if ( odd ) {
	    pad = 8 - ( (bcnt + 1) / 2 );   /* # bytes to pad with zeros */
	} 
	else {
	    pad = 7 - ( bcnt / 2 );
	}

	while( k < pad )
	    addr[++k] = 0;		    /* pad the array */

	i = 0;

	if ( !odd ) {
	    addr[++k] = 0;		    /* pack in the extra digit */
	    addr[k] |= ( tmp[i++] & 0x0f );
	} 

	   /* pack the rest of the E.164 address into the array */
	while( k < NSAP_START_RD_FIELD ) {
	    addr[++k] = tmp[i++] << 4;
	    addr[k] |= ( tmp[i++] & 0x0f );
	}

    } /* if( E.164 format */
    else 
#endif SPECIAL_E164
    {	/* DCC or ICD formatted NSAP */

        /*
         * Read four hex digits
         */
        for (j = 0; j < 2; ++j) {
            c = toupper(buf[++(*pi)]); 
            if (c == '.')
                c = toupper(buf[++(*pi)]);      /* skip optional '.' */
            if (!isxdigit(c))
                return (FALSE);
            addr[++k] = hexCharValue(c) << 4;
            c = toupper(buf[++(*pi)]);
            if (!isxdigit(c))
                return (FALSE);
            addr[k] |= (hexCharValue(c) & 0x0f);
        } 

        /*
         * Read two hex digits
         */
        c = toupper(buf[++(*pi)]);
        if (c == '.')
            c = toupper(buf[++(*pi)]);	   /* skip optional '.' */
        if (!isxdigit(c))
            return (FALSE);
        addr[++k] = hexCharValue(c) << 4;
        c = toupper(buf[++(*pi)]);
        if (!isxdigit(c))
            return (FALSE);
        addr[k] |= (hexCharValue(c) & 0x0f);

        /*
         * Read six hex digits
         */
        for (j = 0; j < 3; ++j) {
            c = toupper(buf[++(*pi)]);
            if (c == '.')
               c = toupper(buf[++(*pi)]);      /* skip optional '.' */
            if (!isxdigit(c))
               return (FALSE);
            addr[++k] = hexCharValue(c) << 4;
            c = toupper(buf[++(*pi)]);
            if (!isxdigit(c))
               return (FALSE);
            addr[k] |= (hexCharValue(c) & 0x0f);
        } 

        /*
         * Read four hex digits
         */
        for (j = 0; j < 2; ++j) {
            c = toupper(buf[++(*pi)]); 
            if (c == '.')
                c = toupper(buf[++(*pi)]);      /* skip optional '.' */
            if (!isxdigit(c))
                return (FALSE);
            addr[++k] = hexCharValue(c) << 4;
            c = toupper(buf[++(*pi)]);
            if (!isxdigit(c))
                return (FALSE);
            addr[k] |= (hexCharValue(c) & 0x0f);
        } 

    } /* DCC or ICD formatted NSAP */


    /*
     * Read five groups of 4 hex digits
     */
    for (i = 0; i < 5; ++i) { 
       for (j = 0; j < 2; ++j) {
          c = toupper(buf[++(*pi)]);
          if (c == '.')
             c = toupper(buf[++(*pi)]);      /* skip optional '.' */
          if (!isxdigit(c))
             return (FALSE);
          addr[++k] = hexCharValue(c) << 4;
          c = toupper(buf[++(*pi)]);
          if (!isxdigit(c))
             return (FALSE);
          addr[k] |= (hexCharValue(c) & 0x0f);
       }
    } 

    /*
     * Read last two hex digits
     */
    c = toupper(buf[++(*pi)]);
    if (c == '.')
        c = toupper(buf[++(*pi)]);     /* skip optional '.' */
    if (!isxdigit(c))
       return (FALSE);
    addr[++k] = hexCharValue(c) << 4;
    c = toupper(buf[++(*pi)]);
    if (!isxdigit(c))
       return (FALSE);
    addr[k] |= (hexCharValue(c) & 0x0f);
    c = toupper(buf[++(*pi)]);

    /*
     * Fill in the hwaddrtype fields before returning
     */
    hwaddrp->type = STATION_ATMNSAP;
    hwaddrp->length = STATIONLEN_ATMNSAP;
    hwaddrp->target_type = TARGET_UNICAST;
    bcopy(addr, hwaddrp->addr, STATIONLEN_ATMNSAP);

    /* printf( "Made it through the NSAP parser!\n\n" ); */

    return(TRUE);
}


char *atm_esi_short_help (parseinfo *csb, uint flag, idbtype *idb)
{
    return ("atm esi address\n");
}

boolean match_atm_esi (parseinfo *csb, char *buf, int *pi, void *addr_pointer,
                        int in_help, uint flag, idbtype *idb)
{

  uchar addr[STATIONLEN_ATMESI];
  char c;
  int j, k;
  hwaddrtype *hwaddrp = addr_pointer;


    /*
     * Read first two hex digits
     */
    k = 0;
    c = toupper(buf[(*pi)]);
    if (!isxdigit(c))
       return (FALSE);
    addr[k] = hexCharValue(c) << 4;
    c = toupper(buf[++(*pi)]);
    if (!isxdigit(c))
       return (FALSE);
    addr[k] |= (hexCharValue(c) & 0x0f);


  /*
   * Read twelve hex digits
   */
  for (j = 0; j < (ILMI_ESI_SIZE - 1); ++j) { 

    c = toupper(buf[++(*pi)]);
    if (c == '.')
      c = toupper(buf[++(*pi)]);      /* skip optional '.' */
    if (!isxdigit(c))
      return (FALSE);
    addr[++k] = hexCharValue(c) << 4;
    c = toupper(buf[++(*pi)]);
    if (!isxdigit(c))
      return (FALSE);
    addr[k] |= (hexCharValue(c) & 0x0f);
  } 

  /* avoid whitespace parsing problems */
  c = toupper(buf[++(*pi)]);

  /* set ATMESI here for parser, set AIPNSAP in atm.c */
  hwaddrp->type = STATION_ATMESI;
  hwaddrp->length = STATIONLEN_ATMESI;
  hwaddrp->target_type = TARGET_UNICAST;

  /* copy ESI into ESI portion of addr */
  bcopy(addr, &hwaddrp->addr[ILMI_ESI_OFFSET], STATIONLEN_ATMESI);

  return(TRUE);

}


