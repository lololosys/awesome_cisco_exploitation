/* $Id: x25_facil.c,v 3.3.10.2 1996/08/07 09:04:48 snyder Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_facil.c,v $
 *------------------------------------------------------------------
 * x25_facility.c -- X25 Facility processing
 *
 * February 1987, Greg Satz
 *
 * Copyright (c) 1987-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_facil.c,v $
 * Revision 3.3.10.2  1996/08/07  09:04:48  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3.10.1  1996/03/18  22:50:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  07:57:38  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/03/07  11:19:38  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:56:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/23  23:21:34  carvalho
 * CSCdi41580:  Calling Address Ext. facility modified while X25 switching
 * Save digit count/authority field for called and calling address
 * extension facilities for re-use when call is forwarded.
 *
 * Revision 3.2  1995/11/17  18:07:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/04  22:02:30  gglick
 * CSCdi37951:  X.25/BFE X.121 address generation incorrect
 *
 * Revision 2.3  1995/06/27  19:13:09  gglick
 * CSCdi36424:  X.25 processing local facilities as standard facilities
 *         Incorporate code review comments
 *
 * Revision 2.2  1995/06/27  00:14:14  gglick
 * CSCdi36424:  X.25 processing local facilities as standard facilities
 *
 * Revision 2.1  1995/06/07  23:22:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "address.h"
#include "packet.h"
#include "logger.h"
#include "x25.h"
#include "x25_proto.h"
#include "x25_bfe.h"
#include "lapb.h"
#include "../os/buffers.h"
#include "../clns/clns.h"		/* Gag.  We really shouldn't need to */
#include "../clns/clns_route.h"		/* include all of these, but that's  */
#include "../clns/iso_igrp.h"		/* only way to get the prototype file */
#include "config.h"
#include "../clns/clns_externs.h"



/*
 *  define a constant to distinguish the standard facilities from
 *  local/remote/CCITT facilities
 */
#define NO_MARKER	0xffff	/* this works because markers are 1 byte */

/*
 *  define the constants and structures used for determining the legailty
 *  of a facility for a given packet type
 */

#define facil_data	struct _facil_data

facil_data {
    uchar	code;		/* facility code */
    uchar	legal_in;	/* packet types in which the code is allowed */
    uchar	allowed_by;	/* allowed by which CCITT Recommendations? */
    uchar	found;		/* # of times found */
};

#define X25_FACILITY_ENDOFARRAY	0xff

#define X25FAC_DDN	0x01		/* allowed by DDN X.25 */
#define X25FAC_BFE	0x02		/* allowed by BFE X.25 */
#define X25FAC_1980	0x04		/* allowed by 1980 CCITT X.25 */
#define X25FAC_1984	0x08		/* allowed by 1984 CCITT X.25 */
#define X25FAC_1988	0x10		/* allowed by 1988 CCITT X.25 */

#define X25FAC_CALL_DTE_M	0x01	/* Call Request, from DTE */
#define X25FAC_CALL_DCE_M	0x02	/* Incoming Call, from DCE */
#define X25FAC_CALL_C_DTE_M	0x04	/* Call Accepted, from DTE */
#define X25FAC_CALL_C_DCE_M	0x08	/* Call Connected, from DCE */
#define X25FAC_CLEAR_DTE_M	0x10	/* Clear Request, from DTE */
#define X25FAC_CLEAR_DCE_M	0x20	/* Clear Indication, from DCE */
#define X25FAC_CLEAR_C_DTE_M	0x40	/* Clear Confirm from DTE */
#define X25FAC_CLEAR_C_DCE_M	0x80	/* Clear Confirm from DCE */

#define X25FAC_CALLS_M		(X25FAC_CALL_DTE_M | X25FAC_CALL_DCE_M)
#define X25FAC_CALL_CS_M	(X25FAC_CALL_C_DTE_M | X25FAC_CALL_C_DCE_M)
#define X25FAC_CLEARS_M		(X25FAC_CLEAR_DTE_M | X25FAC_CLEAR_DCE_M)

/*
 *  reference 1984 CCITT Table 27/X.25 and 1988 CCITT Table 29/X.25
 */
static facil_data	facil_x25[] = {
    {X25_FACILITY_FLOW_PACKET,
	(X25FAC_CALLS_M | X25FAC_CALL_CS_M),
	(X25FAC_DDN | X25FAC_BFE | X25FAC_1980 | X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_FLOW_WINDOW,
	(X25FAC_CALLS_M | X25FAC_CALL_CS_M),
	(X25FAC_DDN | X25FAC_BFE | X25FAC_1980 | X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_THROUGHPUT,
	(X25FAC_CALLS_M | X25FAC_CALL_CS_M),
	(X25FAC_DDN | X25FAC_1980 | X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_CUG_BASIC,
	X25FAC_CALLS_M,
	(X25FAC_DDN | X25FAC_1980 | X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_CUG_EXTENDED,
	X25FAC_CALLS_M,
	(X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_CUG_OUT_BASIC,
	X25FAC_CALLS_M,
	(X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_CUG_OUT_EXTENDED,
	X25FAC_CALLS_M,
	(X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_CUG_BILATERAL,
	X25FAC_CALLS_M,
	(X25FAC_1980 | X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_REVERSE_CHARGING,	/* == X25_FACILITY_FAST_SELECT */
	X25FAC_CALLS_M,
	(X25FAC_DDN | X25FAC_1980 | X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_NETWORK_USER_ID,
	(X25FAC_CALL_DTE_M | X25FAC_CALL_C_DTE_M),
	(X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_CHARGE_REQUEST,
	(X25FAC_CALL_DTE_M | X25FAC_CALL_C_DTE_M),
	(X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_CHARGE_REC_MONEY,
	(X25FAC_CLEAR_DCE_M | X25FAC_CLEAR_C_DCE_M),
	(X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_CHARGE_REC_CNT,
	(X25FAC_CLEAR_DCE_M | X25FAC_CLEAR_C_DCE_M),
	(X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_CHARGE_REC_TIME,
	(X25FAC_CLEAR_DCE_M | X25FAC_CLEAR_C_DCE_M),
	(X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_RPOA_BASIC,
	X25FAC_CALL_DTE_M,
	(X25FAC_1980 | X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_RPOA_EXTENDED,
	X25FAC_CALL_DTE_M,
	(X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_CALL_DEFLECT_SEL,
	X25FAC_CLEAR_DTE_M,
	X25FAC_1988, 0},
    {X25_FACILITY_CALL_REDIRECT_NOTE,
	X25FAC_CALL_DCE_M,
	(X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_LINE_ADDR_MODIFY,
     (X25FAC_CALL_CS_M | X25FAC_CLEARS_M),
	(X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_TDELAY,
	(X25FAC_CALLS_M | X25FAC_CALL_C_DCE_M),
	(X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_ENDOFARRAY, 0, 0, 0}		/* end-of-array marker */
};


/*
 *  reference 1984 & 1988 CCITT Table G-1/X.25
 *
 *  Note that some facilities have two entries; they represent the CLEAR
 *  REQUEST packet type which, in 1988, became an allowed packet type for
 *  these facilities.
 */
static facil_data	facil_ccitt[] = {
    {X25_FACILITY_CALLING_EXT,
	X25FAC_CALLS_M,
	(X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_CALLING_EXT,
	X25FAC_CLEAR_DTE_M,
	X25FAC_1988, 0},
    {X25_FACILITY_CALLED_EXT,
	(X25FAC_CALLS_M | X25FAC_CALL_CS_M | X25FAC_CLEARS_M),
	(X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_QOS_THROUGHPUT,
	X25FAC_CALLS_M,
	(X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_QOS_THROUGHPUT,
	X25FAC_CLEAR_DTE_M,
	X25FAC_1988, 0},
    {X25_FACILITY_QOS_TDELAY,
	(X25FAC_CALLS_M | X25FAC_CALL_CS_M),
	(X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_QOS_TDELAY,
	X25FAC_CLEAR_DTE_M,
	X25FAC_1988, 0},
    {X25_FACILITY_QOS_PRIORITY,
	(X25FAC_CALLS_M | X25FAC_CALL_CS_M | X25FAC_CLEAR_DTE_M),
	X25FAC_1988, 0},
    {X25_FACILITY_QOS_PROTECTION,
	(X25FAC_CALLS_M | X25FAC_CALL_CS_M | X25FAC_CLEAR_DTE_M),
	X25FAC_1988, 0},
    {X25_FACILITY_EXP_DATA,
	(X25FAC_CALLS_M | X25FAC_CALL_CS_M),
	(X25FAC_1984 | X25FAC_1988), 0},
    {X25_FACILITY_EXP_DATA,
	X25FAC_CLEAR_DTE_M,
	X25FAC_1988, 0},
    {X25_FACILITY_ENDOFARRAY, 0, 0, 0}		/* end-of-array marker */
};

/*
 *  reference DDN and BFE X.25 specifications--
 *  note that DDN and BFE are the only 1980-compliant X.25 services we offer
 */
static facil_data	facil_ddn_bfe[] = {
    {X25_FACILITY_DDN_STANDARD,
	(X25FAC_CALLS_M | X25FAC_CALL_CS_M),
	(X25FAC_DDN | X25FAC_BFE), 0},
    {X25_FACILITY_DDN_CALL_PREC,
	(X25FAC_CALLS_M | X25FAC_CALL_CS_M),
	(X25FAC_DDN | X25FAC_BFE), 0},
    {X25_FACILITY_DDN_BFE_ATIF,
	X25FAC_CALLS_M,
	X25FAC_BFE, 0},
    {X25_FACILITY_ENDOFARRAY, 0, 0}		/* end-of-array marker */
};

/*
 * x25_parsefacility
 * Parse incoming facilities
 */

boolean x25_parsefacility (
    register hwidbtype *idb,
    register lcitype *lci,
    register uchar *c,
    uchar type,
    uchar *cause,
    uchar *diag,
    int role)
{
    register considbtype *considb;
    register ushort len;
    ushort marker;
    register facil_data *facil_entry;
    facil_data *facil_start;
    register ushort fac_len;
    uchar allowed_m;
    uchar version_m;
    uchar out, in;
    boolean cug_defined;		/* multiple codes can define a CUG */
    boolean rpoa_defined;		/* multiple codes can define a RPOA */
    boolean local_marker_defined;	/* local facility marker found */
    boolean remote_marker_defined;	/* remote facility marker found */
    boolean ccitt_marker_defined;	/* CCITT facility marker found */
    uchar *marker_head;			/* start of unknown facilities */
    uchar *marker_cur;			/* temp. unknown facility pointer */

    considb = get_lci_considb(lci);

    /*
     *  determine the packet type mask
     */
    switch (type) {
      case X25_CALL_REQUEST:
	allowed_m = X25FAC_CALL_DTE_M | X25FAC_CALL_DCE_M;
	break;
      case X25_CALL_CONNECTED:
	allowed_m = X25FAC_CALL_C_DTE_M | X25FAC_CALL_C_DCE_M;
	break;
      case X25_CLEAR:
	allowed_m = X25FAC_CLEAR_DTE_M | X25FAC_CLEAR_DCE_M;
	break;
      case X25_CLEAR_CONFIRMATION:
	allowed_m = X25FAC_CLEAR_C_DTE_M | X25FAC_CLEAR_C_DCE_M;
	break;
      default:
	allowed_m = 0;	/* nothing's legal */
	break;
    }

    /*
     *  determine the interface version mask
     */
    if (IS_1984(idb))
	version_m = X25FAC_1984;
    else if (IS_1988(idb))
	version_m = X25FAC_1988;
    else if (ISBFE(idb))
	version_m = X25FAC_BFE;
    else if (ISDDN(idb))
	version_m = X25FAC_DDN;
    else
	version_m = X25FAC_1980;

    /*
     *  if we have strict CCITT facility checking, modify the packet type mask
     *  to disallow receipt of facilities from the wrong type of interface
     */
    if (idb->x25_strict_ccitt_facil) {
	if (ISDTE(idb, considb))
	    /* only allow facilities that can come from a DCE */
	    allowed_m &= X25FAC_CALL_DCE_M | X25FAC_CALL_C_DCE_M |
		      X25FAC_CLEAR_DCE_M | X25FAC_CLEAR_C_DCE_M;
	else
	    /* only allow facilities that can come from a DTE */
	    allowed_m &= X25FAC_CALL_DTE_M | X25FAC_CALL_C_DTE_M |
		      X25FAC_CLEAR_DTE_M | X25FAC_CLEAR_C_DTE_M;
    }

    /*
     *  initialize the validation fields
     */
    for (facil_entry = facil_x25;
	 facil_entry->code != X25_FACILITY_ENDOFARRAY; facil_entry++)
	facil_entry->found = 0;
    cug_defined = FALSE;
    rpoa_defined = FALSE;
    for (facil_entry = facil_ddn_bfe;
	 facil_entry->code != X25_FACILITY_ENDOFARRAY; facil_entry++)
	facil_entry->found = 0;
    local_marker_defined = FALSE;
    remote_marker_defined = FALSE;
    marker_head = NULL;
    for (facil_entry = facil_ccitt;
	 facil_entry->code != X25_FACILITY_ENDOFARRAY; facil_entry++)
	facil_entry->found = 0;
    ccitt_marker_defined = FALSE;

    len = *c++;

    if (len >
	(IS_1980(idb) ? X25_FACILITY_DATA_1980 : X25_FACILITY_DATA_1984)) {
	*cause = X25_CLEAR_LOCAL_PROC_ERR;
	*diag = X25_DIAG_INVALID_LENGTH;
	return(FALSE);
    }

    /*
     *  start with the standard facilities
     */
    marker = NO_MARKER;
    facil_start = facil_x25;

    /*
     *  parse the facilities
     */
    while (len) {
	/*
	 *  determine the size, Figure 23/X.25
	 */
	switch (*c & X25_FACILITY_CLASS_MASK) {
	  case X25_FACILITY_CLASS_A_CODE:
	    fac_len = X25_FACILITY_CLASS_A_LEN;
	    break;
	  case X25_FACILITY_CLASS_B_CODE:
	    fac_len = X25_FACILITY_CLASS_B_LEN;
	    break;
	  case X25_FACILITY_CLASS_C_CODE:
	    fac_len = X25_FACILITY_CLASS_C_LEN;
	    break;
	  case X25_FACILITY_CLASS_D_CODE:
	  default:		/* keep compiler happy */
	    fac_len = c[1] + 2;
	    break;
	}

	if (*c == X25_FACILITY_MARKER && len >= fac_len) {
	    /*
	     *  make sure only one marker of a given type is present
	     *  and that nothing follows a CCITT marker
	     */
	    if ((c[1] == X25_FACILITY_MARKER_LOCAL &&
		 !ccitt_marker_defined && !local_marker_defined) ||
		(c[1] == X25_FACILITY_MARKER_REMOTE &&
		 !ccitt_marker_defined && !remote_marker_defined) ||
		(c[1] == X25_FACILITY_MARKER_CCITT &&
		 !ccitt_marker_defined)) {
		; /* EMPTY */
	    } else {
		*cause = X25_CLEAR_LOCAL_PROC_ERR;
		*diag = X25_DIAG_CODE_ILLEGAL;
		return(FALSE);
	    }

	    /*
	     *  this marks the start of a new facility block--
	     *
	     *  if the facilities encoded in the block are known, facil_start
	     *  will be set to the structure that describes the codes
	     *
	     *  if the facilities aren't known, marker_head will be set to
	     *  the address of the first facility in preparation for validation
	     */
	    facil_start = NULL;
	    marker_head = NULL;

	    switch (marker = c[1]) {
	      case X25_FACILITY_MARKER_LOCAL:
		local_marker_defined = TRUE;
		if (ISDDN(idb))
		    facil_start = facil_ddn_bfe;
		else
		    marker_head = &c[fac_len];
		break;
	      case X25_FACILITY_MARKER_REMOTE:
		remote_marker_defined = TRUE;
		marker_head = &c[fac_len];
		break;
	      case X25_FACILITY_MARKER_CCITT:
		ccitt_marker_defined = TRUE;
		facil_start = facil_ccitt;
		break;
	    }

	    c += fac_len;
	    len -= fac_len;
	    continue;
	} else if (facil_start) {
	    /*
	     *  this is a standard, DDN or CCITT facility--
	     *  verify the facility code is recognized and
	     *  that it isn't duplicated
	     */
	    for (facil_entry = facil_start;
		 (facil_entry->code != *c) &&
		 (facil_entry->code != X25_FACILITY_ENDOFARRAY);
		 facil_entry++)
	    ;	/* EMPTY */

	    /*
	     *  if this entry doesn't allow the facility, check any
	     *  subsequent entries for an allowable entry--note that the
	     *  count of occurances will still be valid because a given
	     *  packet type will always land at the same table entry
	     */
	    while (((!(facil_entry->allowed_by & version_m)) ||
		    (!(facil_entry->legal_in & allowed_m))) &&
		   (facil_entry[1].code == facil_entry->code))
		facil_entry++;

	    /*
	     *  fail on a facility that isn't legal for this version
	     */
	    if (!(facil_entry->allowed_by & version_m)) {
		if (BFE_BROKEN(idb) && (*c == X25_FACILITY_FAST_SELECT)) {
		    /*
		     *  despite the BFE specification explicit statement
		     *  that fast select is not supported, our certification
		     *  requires that we allow it
		     */
		    ;	/* EMPTY */
		} else {
		    *cause = X25_CLEAR_INVALID_FACILITY;
		    *diag = X25_DIAG_CODE_ILLEGAL;
		    return(FALSE);
		}
	    }

	    /*
	     *  make sure the facility is legal for this packet type
	     */
	    if (!(facil_entry->legal_in & allowed_m)) {
		*cause = X25_CLEAR_INVALID_FACILITY;
		*diag = X25_DIAG_PACK_INCOMPAT_FACILITY;
		return(FALSE);
	    }

	    /*
	     *  validate the number of times it has occurred--
	     *  DDN/BFE specifications allow multiple occurances of a
	     *  facility code
	     */
	    if (ISDDN(idb)) {
		if (*c == X25_FACILITY_CUG_BASIC)
		    cug_defined = FALSE;
	    } else if (facil_entry->found) {
		*cause = X25_CLEAR_LOCAL_PROC_ERR;
		*diag = X25_DIAG_DUP_FACILITY;
		return(FALSE);
	    }
	    facil_entry->found++;
	} else {
	    /*
	     *  this is a block of unknown facilities (either remote network
	     *  facilities or local facilities on a non-DDN interface)--
	     *  just check that this facility code only occurs once
	     */
	    marker_cur = marker_head;
	    for ( ; ; ) {
		if (marker_cur != c && *marker_cur == *c) {
		    *cause = X25_CLEAR_LOCAL_PROC_ERR;
		    *diag = X25_DIAG_DUP_FACILITY;
		    return(FALSE);
		}

		/*
		 *  determine facility length, Figure 23/X.25
		 */
		switch (*marker_cur & X25_FACILITY_CLASS_MASK) {
		  case X25_FACILITY_CLASS_A_CODE:
		    fac_len = X25_FACILITY_CLASS_A_LEN;
		    break;
		  case X25_FACILITY_CLASS_B_CODE:
		    fac_len = X25_FACILITY_CLASS_B_LEN;
		    break;
		  case X25_FACILITY_CLASS_C_CODE:
		    fac_len = X25_FACILITY_CLASS_C_LEN;
		    break;
		  case X25_FACILITY_CLASS_D_CODE:
		  default:		/* keep compiler happy */
		    fac_len = c[1] + 2;
		    break;
		}

		/*
		 * We used to just check == c, but this is safer...
		 */
		if (marker_cur >= c)
		    break;

		marker_cur += fac_len;
	    }
	}

	/*
	 *  validate the length of the facility parameter
	 */
	if (len < fac_len) {
	    *cause = X25_CLEAR_LOCAL_PROC_ERR;
	    *diag = X25_DIAG_INVALID_LENGTH;
	    return(FALSE);
	}

	if (marker == NO_MARKER) {
	    switch (*c) {
	      case X25_FACILITY_FLOW_PACKET:
		/*
		 *  the packet size in/packet size out values depend
		 *  on our role in the call--values are ordered as
		 *  "from called DTE" then "from calling DTE"
		 */ 
		if (role == X25_ROLE_CALLED_DTE) {
		    out = c[1];
		    in = c[2];
		} else {
		    in = c[1];
		    out = c[2];
		}

		if (in < 4 || in > 12 || out < 4 || out > 12) {
		    *cause = X25_CLEAR_INVALID_FACILITY;
		    *diag = X25_DIAG_FACILITY_PARAM_ILLEGAL;
		    return(FALSE);
		}

		/*
		 *  1980 CCITT limits packet size to 1024
		 */
		if (IS_1980(idb) && (in > 10 || out > 10)) {
		    *cause = X25_CLEAR_INVALID_FACILITY;
		    *diag = X25_DIAG_FACILITY_PARAM_ILLEGAL;
		    return(FALSE);
		}

		if (type == X25_CALL_CONNECTED) {
		    /*
		     *  we have a CALL CONFIRM--
		     *  validate the negotiation process
		     *  per 1988 CCITT Table 24/X.25 and Table 25/X.25
		     */
		    if ((lci->lci_pout != out) &&
			((lci->lci_pout == X25_DEFAULT_OPS) ||
			 (lci->lci_pout > X25_DEFAULT_OPS &&
			  out > lci->lci_pout) ||
			 (lci->lci_pout < X25_DEFAULT_OPS &&
			  out < lci->lci_pout))) {
			*cause = X25_CLEAR_INVALID_FACILITY;
			*diag = X25_DIAG_FACILITY_PARAM_ILLEGAL;
			return(FALSE);
		    }
		    if ((lci->lci_pin != in) &&
			((lci->lci_pin == X25_DEFAULT_IPS) ||
			 (lci->lci_pin > X25_DEFAULT_IPS &&
			  in > lci->lci_pin) ||
			 (lci->lci_pin < X25_DEFAULT_IPS &&
			  in < lci->lci_pin))) {
			*cause = X25_CLEAR_INVALID_FACILITY;
			*diag = X25_DIAG_FACILITY_PARAM_ILLEGAL;
			return(FALSE);
		    }

		    /*
		     *  if the CONFIRM changes the packet sizes, flag the fact
		     */
		    lci->lci_force_pksize |=
			(lci->lci_pin != in) || (lci->lci_pout != out);
		}

		/*
		 *  Note that the values in a CALL may exceed our capacity,
		 *  as even the default values may exceed our capacity!
		 *  Other code must check for this and lower the packet
		 *  size(s) for the CALL CONFIRM
		 */
		lci->lci_pin = in;
		lci->lci_pout = out;
		break;
	      case X25_FACILITY_FLOW_WINDOW:
		/*
		 *  the window size in/window size out values depend
		 *  on our role in the call--values are ordered as
		 *  "from called DTE" then "from calling DTE"
		 */ 
		if (role == X25_ROLE_CALLED_DTE) {
		    out = c[1];
		    in = c[2];
		} else {
		    in = c[1];
		    out = c[2];
		}

		if (in == 0 || in >= idb->x25_modulo ||
		    out == 0 || out >= idb->x25_modulo) {
		    /*
		     *  window size(s) are invalid for this interface, but
		     *  negotiate lower window sizes for incoming
		     *  tunneled CALLs
		     */
		    if ((type == X25_CALL_REQUEST) &&
			(role == X25_ROLE_CALLING_DTE)) {
			if (in >= idb->x25_modulo)
			    in = idb->x25_modulo - 1;
			if (out >= idb->x25_modulo)
			    out = idb->x25_modulo - 1;
			lci->lci_force_winsize = TRUE;
		    } else {
			*cause = X25_CLEAR_INVALID_FACILITY;
			*diag = X25_DIAG_FACILITY_PARAM_ILLEGAL;
			return(FALSE);
		    }
		}

		if (type == X25_CALL_CONNECTED) {
		    /*
		     *  we have a CALL CONFIRM--
		     *  validate the negotiation process
		     *  per 1988 CCITT Tables 24/X.25 and 25/X.25
		     */
		    if ((lci->lci_wout != out) &&
			((lci->lci_wout == X25_DEFAULT_WOUT) ||
			 (lci->lci_wout > X25_DEFAULT_WOUT &&
			  out > lci->lci_wout) ||
			 (lci->lci_wout < X25_DEFAULT_WOUT &&
			  out < lci->lci_wout))) {
			*cause = X25_CLEAR_INVALID_FACILITY;
			*diag = X25_DIAG_FACILITY_PARAM_ILLEGAL;
			return(FALSE);
		    }
		    if ((lci->lci_win != in) &&
			((lci->lci_win == X25_DEFAULT_WIN) ||
			 (lci->lci_win > X25_DEFAULT_WIN &&
			  in > lci->lci_win) ||
			 (lci->lci_win < X25_DEFAULT_WIN &&
			  in < lci->lci_win))) {
			*cause = X25_CLEAR_INVALID_FACILITY;
			*diag = X25_DIAG_FACILITY_PARAM_ILLEGAL;
			return(FALSE);
		    }

		    /*
		     *  if the CONFIRM changes the window sizes, flag the fact
		     */
		    lci->lci_force_winsize |=
			(lci->lci_win != in) || (lci->lci_wout != out);
		}

		lci->lci_win = in;
		lci->lci_wout = out;
		break;
	      case X25_FACILITY_THROUGHPUT:
		/*
		 *  the throughput in/out values depend the VC's role--
		 *  high nibble "from called", low nibble "from calling"
		 */ 
		if (role == X25_ROLE_CALLED_DTE) {
		    out = (c[1] >> 4) & 0x0f;
		    in = c[1] & 0x0f;
		} else {
		    in = (c[1] >> 4) & 0x0f;
		    out = c[1] & 0x0f;
		}
		if (in < 3 || out < 3 ||
		    in > (IS_1988(idb) ? 13 : 12) ||
		    out > (IS_1988(idb) ? 13 : 12)) {
		    *cause = X25_CLEAR_INVALID_FACILITY;
		    *diag = X25_DIAG_FACILITY_PARAM_ILLEGAL;
		    return(FALSE);
		}
		lci->lci_tput_out = out;
		lci->lci_tput_in = in;
		break;
	      case X25_FACILITY_CUG_BASIC:
		if (cug_defined) {
		    *cause = X25_CLEAR_LOCAL_PROC_ERR;
		    *diag = X25_DIAG_DUP_FACILITY;
		    return(FALSE);
		}
		cug_defined = TRUE;
		lci->lci_cug = c[1];
		break;
	      case X25_FACILITY_CUG_EXTENDED:
	      case X25_FACILITY_CUG_OUT_BASIC:
	      case X25_FACILITY_CUG_OUT_EXTENDED:
	      case X25_FACILITY_CUG_BILATERAL:
		/*
		 *  we don't encode these CUG formats; simply validate
		 */
		if (cug_defined) {
		    *cause = X25_CLEAR_LOCAL_PROC_ERR;
		    *diag = X25_DIAG_DUP_FACILITY;
		    return(FALSE);
		}
		cug_defined = TRUE;
		break;
	      case X25_FACILITY_REVERSE_CHARGING:
	   /* case X25_FACILITY_FAST_SELECT: is equivalent */
		lci->lci_reverse = (c[1] & X25_FACILITY_REVERSE_CHARGE_ON) ?
		    TRUE : FALSE;
		lci->lci_fast = c[1] & X25_FACILITY_FAST_SELECT_MASK;
		if (lci->lci_fast) {
		    lci->lci_confirm_addr = TRUE;	/* ignore reverse */
		}
		break;
	      case X25_FACILITY_NETWORK_USER_ID:
		lci->lci_nuidlen = c[1];
		lci->lci_nuid = &c[2];
		break;
	      case X25_FACILITY_CHARGE_REQUEST:
	      case X25_FACILITY_CHARGE_REC_MONEY:
	      case X25_FACILITY_CHARGE_REC_CNT:
	      case X25_FACILITY_CHARGE_REC_TIME:
		/*
		 *  unused
		 */
		break;
	      case X25_FACILITY_RPOA_BASIC:
	      case X25_FACILITY_RPOA_EXTENDED:
		/*
		 *  unused
		 */
		if (rpoa_defined) {
		    *cause = X25_CLEAR_LOCAL_PROC_ERR;
		    *diag = X25_DIAG_DUP_FACILITY;
		    return(FALSE);
		}
		rpoa_defined = TRUE;
		break;
	      case X25_FACILITY_CALL_DEFLECT_SEL:
		/*
		 *  unused
		 */
		break;
	      case X25_FACILITY_CALL_REDIRECT_NOTE:
		if (c[1] == 0 ) {
		    *cause = X25_CLEAR_INVALID_FACILITY;
		    *diag = X25_DIAG_FACILITY_PARAM_ILLEGAL;
		    return(FALSE);
		}
		/* 
		 *  no need to save it, since it is only a notification on an
		 *  incoming call. We don't pass through or do any action.
		 */
		break;
	      case X25_FACILITY_LINE_ADDR_MODIFY:
		lci->lci_lam = c[1];
		lci->lci_confirm_addr = TRUE;
		break;
	      case X25_FACILITY_TDELAY:
		lci->lci_tdelay = (c[1] << 8) | c[2];
 		break;
	      default:
		/*
		 * ? we shouldn't get here
		 */
		*cause = X25_CLEAR_INVALID_FACILITY;
		*diag = X25_DIAG_CODE_ILLEGAL;
		return(FALSE);
	    } /* standard facility code case */
	} else if (ISDDN(idb) && (marker == X25_FACILITY_MARKER_LOCAL)) {
	    /*
	     *  parse the DDN local facilities
	     */
	    switch (*c) {
	      case X25_FACILITY_DDN_STANDARD:
		if (c[1] != X25_FACILITY_DDN_STANDARD_ON) {	
		    *cause = X25_CLEAR_INVALID_FACILITY;
		    *diag = X25_DIAG_CALL_ERROR;
		    return(FALSE);
		}
		break;

	      case X25_FACILITY_DDN_CALL_PREC:
		if (c[1] > 3) {
		    *cause = X25_CLEAR_INVALID_FACILITY;
		    *diag = X25_DIAG_FACILITY_PARAM_ILLEGAL;
		    return(FALSE);
		}
		if (type == X25_CALL_CONNECTED)
		    lci->lci_prec_connected = c[1];
                        else
		    lci->lci_prec = c[1];
		break;

	      case X25_FACILITY_DDN_BFE_ATIF:
		if (c[1] != BFE_FACIL_LEN) {
		    *cause = X25_CLEAR_INVALID_FACILITY;
		    *diag = X25_DIAG_FACILITY_PARAM_ILLEGAL;
		    return(FALSE);
		}
		/* 
		 * no need to keep it since it's for outgoing calls only
		 */
		break;
	      default:
		/*
		 * ? we shouldn't get here
		 */
		*cause = X25_CLEAR_INVALID_FACILITY;
		*diag = X25_DIAG_CODE_ILLEGAL;
		return(FALSE);
	    } /* local (DDN) facility code case */
	} else if ((marker == X25_FACILITY_MARKER_LOCAL) ||
		   (marker == X25_FACILITY_MARKER_REMOTE)) {
	    /*
	     *  non-DDN local facilities and remote facilities don't get
	     *  any processing
	     */
	    ;
	} else {	/* X25_FACILITY_MARKER_CCITT */
	    switch (*c) {
	      case X25_FACILITY_CALLING_EXT:
		/*
		 *  verify that the length is valid and that the encoded
		 *  number of octets doesn't exceed the permitted or the
		 *  encoded length
		 */
		if ((c[1] == 0) ||
		    (c[1] > ADDRLEN_CLNS) ||
		    ((c[2] & 0x3f) > ((c[1] - 1) * 2))) {
		    *cause = X25_CLEAR_INVALID_FACILITY;
		    *diag = X25_DIAG_FACILITY_PARAM_ILLEGAL;
		    return FALSE;
		}

		lci->lci_src_nsap.type = ADDR_CMNS;
		lci->lci_src_nsap.length = c[1] - 1;
		/* per CLNS usage, the length is also placed in the 1st byte */
		lci->lci_src_nsap.cmns_addr[0] = lci->lci_src_nsap.length;
		bcopy(&c[3], &lci->lci_src_nsap.cmns_addr[1],
			lci->lci_src_nsap.length);
		/* also save the digit count/authority field so we can
		 * regenerate the facility when forwarding a call
		 */
		lci->lci_src_nsap_digits = c[2];
		break;

	      case X25_FACILITY_CALLED_EXT:
		if ((c[1] == 0) ||
		    (c[1] > ADDRLEN_CLNS) ||
		    ((c[2] & 0x3f) > ((c[1] - 1) * 2))) {
		   *cause = X25_CLEAR_INVALID_FACILITY;
		   *diag = X25_DIAG_FACILITY_PARAM_ILLEGAL;
	           return FALSE;
		}

		lci->lci_dst_nsap.type = ADDR_CMNS;
		lci->lci_dst_nsap.length = c[1] - 1;
		/* per CLNS usage, the length is also placed in the 1st byte */
		lci->lci_dst_nsap.cmns_addr[0] = lci->lci_dst_nsap.length;
	        bcopy(&c[3], &lci->lci_dst_nsap.cmns_addr[1],
			lci->lci_dst_nsap.length);
		/* also save the digit count/authority field so we can
		 * regenerate the facility when forwarding a call
		 */
		lci->lci_dst_nsap_digits = c[2];
		break;

	      case X25_FACILITY_QOS_THROUGHPUT:
	      case X25_FACILITY_QOS_TDELAY:
	      case X25_FACILITY_QOS_PRIORITY:
	      case X25_FACILITY_QOS_PROTECTION:
	      case X25_FACILITY_EXP_DATA:
		break;

	      default:
		/*
		 * ? we shouldn't get here
		 */
		*cause = X25_CLEAR_INVALID_FACILITY;
		*diag = X25_DIAG_CODE_ILLEGAL;
		return(FALSE);
	    } /* ccitt facility code case */
	}

	/*
	 *  adjust the facility encoding pointer and length
	 */
	c += fac_len;
	len -= fac_len;
    }

    /*
     *  we shouldn't have any unparsed facility data
     */
    if (len) {
	*cause = X25_CLEAR_LOCAL_PROC_ERR;
	*diag = X25_DIAG_INVALID_LENGTH;
	return(FALSE);
    }

    return(TRUE);
}

/*
 *  x25_insert_fc_facil(pak, called_w, calling_w, called_p, calling_p)
 *  Under some circumstances, a flow control facility must be forced
 *  into a CALL or a CALL CONFIRM packet; this routine does that.
 *  The passed window and packet sizes are inserted into the passed
 *  packet address unless the values given are zero.
 */
paktype *
x25_insert_flow_facil (
    register paktype *pak,
    uchar called_w,
    uchar calling_w,
    uchar called_p,
    uchar calling_p)
{
    uchar *c;
    uchar preamble_len;
    uchar *fac_block;
    uchar fac_len;
    int count;
    register paktype *newpak;

    /*
     *	determine where the existing facilities start, allowing for
     *	a short CALL CONFIRM packet
     */

    c = pak->datagramstart + X25HEADERBYTES;

    if (pak->datagramsize == X25HEADERBYTES) {
	/*
	 *  this is a CALL CONFIRM with no address lengths byte or
	 *  facility length byte
	 */
	preamble_len = 2;
	fac_block = c;
	fac_len = 0;
    } else {
	count = bcdlen_inline(*c) + 1;

	if (pak->datagramsize == X25HEADERBYTES + count) {
	    preamble_len = 1;		/* no facility length byte	*/
	    fac_block = c + count;
	    fac_len = 0;
	} else {
	    preamble_len = 0;
	    fac_block = c + count;
	    fac_len = *fac_block;
	}
    }

    c = fac_block + 1;

    /*
     *  we need to overwrite any facilities that are already encoded--
     *	scan the existing facilities
     */
    while(fac_len) {
	if (*c == X25_FACILITY_MARKER)
	    break;

	switch (*c & X25_FACILITY_CLASS_MASK) {
	  case X25_FACILITY_CLASS_A_CODE:
	    c += X25_FACILITY_CLASS_A_LEN;
	    fac_len -= X25_FACILITY_CLASS_A_LEN;
	    break;
	  case X25_FACILITY_CLASS_B_CODE:
	    /*
	     *	over-write an existing set of flow-control values
	     *	and clear the passed values to skip insertion
	     */
	    if (*c == X25_FACILITY_FLOW_WINDOW) {
		c++;
		if (called_w) {
		    *c++ = called_w;
		    *c++ = calling_w;
		    called_w = calling_w = 0;
		} else {
		    c += 2;
		}
	    } else if (*c == X25_FACILITY_FLOW_PACKET) {
		c++;
		if (called_p) {
		    *c++ = called_p;
		    *c++ = calling_p;
		    called_p = calling_p = 0;
		} else {
		    c += 2;
		}
	    } else {
		c += X25_FACILITY_CLASS_B_LEN;
	    }
	    fac_len -= X25_FACILITY_CLASS_B_LEN;
	    break;
	  case X25_FACILITY_CLASS_C_CODE:
	    c += X25_FACILITY_CLASS_C_LEN;
	    fac_len -= X25_FACILITY_CLASS_C_LEN;
	    break;
	  case X25_FACILITY_CLASS_D_CODE:
	  default:		/* keep compiler happy */
	    /*
	     *	skip the facility code, length and value
	     */
	    c++;
	    fac_len -= 2 + *c;
	    c += *c + 1;
	    break;
	}
    }

    /*
     *	determine how many bytes of facilities we need to insert
     */

    fac_len = ((called_w) ? X25_FACILITY_CLASS_B_LEN : 0) +
	      ((called_p) ? X25_FACILITY_CLASS_B_LEN : 0);

    if (fac_len == 0)
	return(pak);

    /*
     *	determine the number of bytes up to and including the facility
     *	length field (if it exists)
     */
    count = ((ulong) fac_block - (ulong) pak->datagramstart) +
	((preamble_len) ? 0 : 1);

    /*
     *	create a new packet if the facilities need to be jammed into the
     *	middle of the existing packet -OR- the packet is too small
     */
    if ((count < pak->datagramsize) ||
	((pak->data_area + pak_get_pool_size(pak)) <
	 (pak->datagramstart + pak->datagramsize + preamble_len + fac_len))) {
	/*
	 *  create a new CALL packet with the added flow control facilities
	 */

	if ((newpak = (paktype *)
		getbuffer(pak->datagramsize + preamble_len +
			  fac_len + LAPB_OVERHEAD_128)) == NULL) {
	    datagram_done(pak);
	    return(NULL);
	}

	/* alternate routing requires re-queueing of newpak          */
	/* for x25_timer() via p_enqueue(&x25iQ, pak);               */
        /* so packet header info must be copied from pak to newpak   */

	pak_copy(pak, newpak, pak->datagramsize);

	newpak->datagramstart = newpak->network_start - X25HEADERBYTES;

	/*
	 *  copy the leading data into the new packet
	 */
	bcopy(pak->datagramstart, newpak->datagramstart, count);
	newpak->datagramsize = count;

	/*
	 *  handle the special case where we're simply extending the packet
	 */
	if (count == pak->datagramsize) {
	    datagram_done(pak);
	    pak = newpak;
	}
    } else {
	/*
	 *  just tack the added facilities on to the end of the existing packet
	 */
	newpak = pak;
    }

    /*
     *	stuff any preamble bytes
     */

    c = newpak->datagramstart + newpak->datagramsize;

    while (preamble_len--) {
	*c++ = 0;
	newpak->datagramsize++;
    }

    fac_block = c - 1;

    /*
     *	put the flow control parameters in
     */

    if (called_w) {
	*c++ = X25_FACILITY_FLOW_WINDOW;
	*c++ = called_w;
	*c++ = calling_w;
	*fac_block += X25_FACILITY_CLASS_B_LEN;
    }

    if (called_p) {
	*c++ = X25_FACILITY_FLOW_PACKET;
	*c++ = called_p;
	*c++ = calling_p;
	*fac_block += X25_FACILITY_CLASS_B_LEN;
    }

    newpak->datagramsize += fac_len;

    /*
     *	fix up the packet size
     */
    if (newpak != pak) {
	/*
	 *  finish off the new CALL packet
	 */
	bcopy(pak->datagramstart + count, fac_block + 1 + fac_len,
		pak->datagramsize - count);
	newpak->datagramsize += pak->datagramsize - count;

	datagram_done(pak);
    }

    return(newpak);
}

/*
 * format a NSAP as found in the X.25 called/calling address extenstion
 * facilities.  These NSAPs may have an odd number of digits, so we can't
 * use the CLNS formatting routine.
 */
void x25_format_nsap (char *buf, char *fac, int digits)
{
    char *ls;
    int inx;

    if (digits > ((ADDRLEN_CLNS - 1) * 2))
	digits = (ADDRLEN_CLNS - 1) * 2;

    ls = buf;
    for (inx = 0; inx < digits; inx++) {
	/*
	 *  a dot separator occurs after the 2nd and
	 *  every subsequent 4th digit
	 */
	if ((inx % 4) == 2)
	    *ls++ = '.';
	if ((inx & 1) == 0)
	    *ls = '0' + ((*fac >> 4) & 0xf);
	else
	    *ls = '0' + (*fac++ & 0xf);

	if (*ls > '9')
	    *ls = (*ls + 'A') - '0' - 10;
	ls++;
    }
    *ls = '\0';
}

/*
 *  descriptions of the address extension address spaces
 */
static char *const addr_ext_descr[4] = {
	"",				/* standard/expected encoding */
	"Reserved address 40, ",
	"non-ISO address, ",
	"Reserved address C0, "};

/*
 * x25_printfacility
 * Print out X25 facilities
 */

uchar *x25_printfacility (register uchar *c)
{
    register ushort len;
    register ushort fac_len;
    ushort inx;
    static char local_str[80];
    char *ls;
    ushort marker = NO_MARKER;

    len = *c++;
    buginf("\n  Facilities: (%d)", len);

    if (len > X25_FACILITY_DATA_1984) {
	buginf(", too large");
	c += len;
	return(c);
    }

    /*
     *  step through the facilities
     */
    while (len) {
	/*
	 *  determine the size, Figure 23/X.25
	 */
	switch (*c & X25_FACILITY_CLASS_MASK) {
	  case X25_FACILITY_CLASS_A_CODE:
	    fac_len = X25_FACILITY_CLASS_A_LEN;
	    break;
	  case X25_FACILITY_CLASS_B_CODE:
	    fac_len = X25_FACILITY_CLASS_B_LEN;
	    break;
	  case X25_FACILITY_CLASS_C_CODE:
	    fac_len = X25_FACILITY_CLASS_C_LEN;
	    break;
	  case X25_FACILITY_CLASS_D_CODE:
	  default:		/* keep compiler happy */
	    fac_len = ((len > 1) ? c[1] : 0) + 2;
	    break;
	}

	if (len < fac_len) {
	    /*
	     *  don't do much if we don't have the data
	     */
	    buginf("\n    Not enough data (%d) for facility code %#x (%d)",
		   len, *c, fac_len);
	    fac_len = len;
	} else if (*c == X25_FACILITY_MARKER) {
	    /*
	     *  handle a facility marker
	     */
	    switch (c[1]) {
	      case X25_FACILITY_MARKER_LOCAL:
		buginf("\n    Local facility marker");
		break;
	      case X25_FACILITY_MARKER_REMOTE:
		buginf("\n    Remote facility marker");
		break;
	      case X25_FACILITY_MARKER_CCITT:
		buginf("\n    CCITT facility marker");
		break;
	      default:
		buginf("\n    Unknown facility marker %#x", c[1]);
		break;
	    }
	    marker = c[1];
	} else if (marker == NO_MARKER) {
	    switch (*c) {
	      case X25_FACILITY_FLOW_PACKET:
		buginf("\n    Packet size: %d %d", 1 << c[1], 1 << c[2]);
		break;
	      case X25_FACILITY_FLOW_WINDOW:
		buginf("\n    Window size: %d %d", c[1], c[2]);
		break;
	      case X25_FACILITY_THROUGHPUT:
		buginf("\n    Throughput: %d, %d",
		    x25_facil_thru((c[1] >> 4) & 0xf),
		    x25_facil_thru(c[1] & 0xf));
		break;
	      case X25_FACILITY_CUG_BASIC:
		buginf("\n    CUG (basic): %d",
		    (((c[1] >> 4) & 0xf) * 10) + (c[1] & 0xf));
		break;
	      case X25_FACILITY_CUG_EXTENDED:
		buginf("\n    CUG (extended): %d",
		    (((c[1] >> 4) & 0xf) * 1000) + ((c[1] & 0xf) * 100) +
		    (((c[2] >> 4) & 0xf) * 10) + (c[2] & 0xf));
		break;
	      case X25_FACILITY_CUG_OUT_BASIC:
		buginf("\n    CUG with outgoing access (basic): %d",
		    (((c[1] >> 4) & 0xf) * 10) + (c[1] & 0xf));
		break;
	      case X25_FACILITY_CUG_OUT_EXTENDED:
		buginf("\n    CUG with outgoing access (extended): %d",
		    (((c[1] >> 4) & 0xf) * 1000) + ((c[1] & 0xf) * 100) +
		    (((c[2] >> 4) & 0xf) * 10) + (c[2] & 0xf));
		break;
	      case X25_FACILITY_CUG_BILATERAL:
		buginf("\n    Bilateral CUG: %d",
		    (((c[1] >> 4) & 0xf) * 1000) + ((c[1] & 0xf) * 100) +
		    (((c[2] >> 4) & 0xf) * 10) + (c[2] & 0xf));
		break;
	      case X25_FACILITY_REVERSE_CHARGING:
	   /* case X25_FACILITY_FAST_SELECT:  is equivalent */
		buginf("\n    Reverse charging %srequested",
		    c[1] & X25_FACILITY_REVERSE_CHARGE_ON ? "" : "not ");
		switch (c[1] & X25_FACILITY_FAST_SELECT_MASK) {
		  case X25_FACILITY_FAST_SELECT_UNRES:
		    buginf("\n    Fast select requested, unrestricted resp.");
		    break;
		  case X25_FACILITY_FAST_SELECT_RES:
		    buginf("\n    Fast select requested, restricted resp.");
		    break;
		  default:
		    buginf("\n    Fast select not requested");
		    break;
		}
		break;
	      case X25_FACILITY_NETWORK_USER_ID:
		buginf("\n    Network User ID (%d)", c[1]);
		break;
	      case X25_FACILITY_CHARGE_REQUEST:
		buginf("\n    Charging information %srequested",
			c[1] & X25_FACILITY_CHARGE_REQ ? "" : "not ");
		break;
	      case X25_FACILITY_CHARGE_REC_MONEY:
		buginf("\n    Charging information, monetary (%d)", c[1]);
		break;
	      case X25_FACILITY_CHARGE_REC_CNT:
		buginf("\n    Charging information, segments (%d)", c[1]);
		break;
	      case X25_FACILITY_CHARGE_REC_TIME:
		buginf("\n    Charging information, time (%d)", c[1]);
		break;
	      case X25_FACILITY_RPOA_BASIC:
		buginf("\n    RPOA selection (basic) %d",
		    (((c[1] >> 4) & 0xf) * 1000) + ((c[1] & 0xf) * 100) +
		    (((c[2] >> 4) & 0xf) * 10) + (c[2] & 0xf));
		break;
	      case X25_FACILITY_RPOA_EXTENDED:
		if ((c[1] % 2) == 1) {
		    strcpy(local_str, "bad length");
		} else {
		    ls = local_str;
		    for (inx = 2;
			 (inx < fac_len) && ((inx - 1) * 5 < sizeof(local_str));
			 inx += 2) {
			*ls++ = ' ';
			*ls++ = '0' + ((c[inx] >> 4) & 0xf);
			*ls++ = '0' + (c[inx] & 0xf);
			*ls++ = '0' + ((c[inx + 1] >> 4) & 0xf);
			*ls++ = '0' + (c[inx + 1] & 0xf);
		    }
		    *ls = '\0';
		}
		buginf("\n    RPOA selection (extended) %s", local_str);
		break;
	      case X25_FACILITY_CALL_DEFLECT_SEL:
		buginf("\n    Call deflection select (%d)", c[1]);
		break;
	      case X25_FACILITY_CALL_REDIRECT_NOTE:
		buginf("\n    Call restriction or deflection notification (%d)",
			c[1]);
		break;
	      case X25_FACILITY_LINE_ADDR_MODIFY:
		buginf("\n    Called line address modified notification (%d)",
			c[1]);
		break;
	      case X25_FACILITY_TDELAY:
		buginf("\n    Transit delay %d msec", (c[1] << 8) + c[2]);
		break;
	      default:
		buginf("\n    Unknown facility code %#x (%d)", *c, fac_len);
		break;
	    }
	} else if (marker == X25_FACILITY_MARKER_LOCAL) {
	    switch (*c) {
	      case X25_FACILITY_DDN_STANDARD:
		buginf("\n    DDN %s service",
			c[1] == X25_FACILITY_DDN_STANDARD_ON ?
			"standard" : "basic");
		break;
	      case X25_FACILITY_DDN_CALL_PREC:
		buginf("\n    DDN precedence %d", c[1]);
		break;
	      case X25_FACILITY_DDN_BFE_ATIF:
		{
		    ipaddrtype ip;
		    rvntype rvn;
		    addrtype x121;

		    if (c[1] != BFE_FACIL_LEN) {
			buginf("\n    BFE addr len error (%d)", c[1]);
		    } else {
			bcopy(&c[2], &ip, BFE_FACIL_LEN);
			if (! bfe_ip_parse(ip, &rvn)) {
			    buginf("\n    BFE addr (%i) error", &c[2]);
			} else if (rvn.net == 0 && rvn.port == 0 &&
				   rvn.domain == 0 && rvn.host == 0) {
			    buginf("\n    BFE Enter Emergency Mode");
			} else {
			    bfe_x121_form(&rvn, &x121);
			    buginf("\n    Remote BFE address %s",
				   x121.x121_addr);
			}
		    }
		}
		break;
	      default:
		buginf("\n    Unknown facility code %#x (%d)", *c, fac_len);
 		break;
	    } /* local facility code case */
	} else if (marker == X25_FACILITY_MARKER_CCITT) {
	    switch (*c) {
	      case X25_FACILITY_CALLING_EXT:
	      case X25_FACILITY_CALLED_EXT:
		{
		    uchar count;

		    /*
		     *  determine the number of digits to print, limiting the
		     *  length in case of an encoding error
		     */
		    count = c[2] & 0x3f;
		    if (count > ((c[1] - 1) * 2))
			count = (c[1] - 1) * 2;
		    x25_format_nsap(local_str, &c[3], count);
		}

		/*
		 *  print the information
		 */
		buginf("\n    %s Address Ext. (%s%d) %s",
		    (*c == X25_FACILITY_CALLED_EXT) ? "Called" : "Calling",
		    addr_ext_descr[(c[2] >> 6) & 0x03], c[2] & 0x3f,
		    local_str);

		break;
	      case X25_FACILITY_QOS_THROUGHPUT:
		buginf("\n    Throughput QOS: %d, %d",
		    x25_facil_thru((c[1] >> 4) & 0xf),
		    x25_facil_thru(c[1] & 0xf));
		break;
	      case X25_FACILITY_QOS_TDELAY:
		switch (c[1]) {
		  case 2:
		    buginf("\n    End-to-end transit delay QOS:"
			   " cumul. %d",
			   (c[2] << 8) + c[3]);
		    break;
		  case 4:
		    buginf("\n    End-to-end transit delay QOS:"
			   " cumul. %d, req. %d",
			   (c[2] << 8) + c[3], (c[4] << 8) + c[5]);
		    break;
		  case 6:
		    buginf("\n    End-to-end transit delay QOS:"
			   " cumul. %d, req. %d, max. %d",
			   (c[2] << 8) + c[3], (c[4] << 8) + c[5],
			   (c[6] << 8) + c[7]);
		    break;
		  default:
		    buginf("\n    End-to-end transit delay QOS:"
			   " bad length (%d)", fac_len);
		    break;
		}
		break;
	      case X25_FACILITY_QOS_PRIORITY:
		switch (c[1]) {
		  case 1:
		    buginf("\n    Priority QOS: "
			   "data %d",
			    c[2]);
		    break;
		  case 2:
		    buginf("\n    Priority QOS: "
			   "data %d, add con %d",
			    c[2], c[3]);
		    break;
		  case 3:
		    buginf("\n    Priority QOS: "
			   "data %d, add con %d, keep con %d",
			    c[2], c[3], c[4]);
		    break;
		  case 4:
		    buginf("\n    Priority QOS: "
			   "data %d, add con %d, keep con %d; "
			   "min. data %d",
			    c[2], c[3], c[4], c[5]);
		    break;
		  case 5:
		    buginf("\n    Priority QOS: "
			   "data %d, add con %d, keep con %d; "
			   "min. data %d, add con %d",
			    c[2], c[3], c[4], c[5], c[6]);
		    break;
		  case 6:
		    buginf("\n    Priority QOS: "
			   "data %d, add con %d, keep con %d; "
			   "min. data %d, add con %d, keep con %d",
			    c[2], c[3], c[4], c[5], c[6], c[7]);
		    break;
		  default:
		    buginf("\n    Priority QOS:"
			   " bad length (%d)", fac_len);
		    break;
		}
		break;
	      case X25_FACILITY_QOS_PROTECTION:
		switch (c[2]) {
		  case 0x00:
		    buginf("\n    Protection QOS (%d), reserved format",
			fac_len);
		    break;
		  case 0x40:
		    buginf("\n    Protection QOS (%d), source address specific",
			fac_len);
		    break;
		  case 0x80:
		    buginf("\n    Protection QOS (%d), dest. address specific",
			fac_len);
		    break;
		  case 0xc0:
		    buginf("\n    Protection QOS (%d), globally unique",
			fac_len);
		    break;
		  default:
		    buginf("\n    Protection QOS (%d), bad format %#x",
			fac_len, c[2]);
		    break;
		}
		break;
	      case X25_FACILITY_EXP_DATA:
		    buginf("\n    Expedited data negotiation for %suse",
			(c[1] & 1) ? "" : "no ");
		break;
	      default:
		buginf("\n    Unknown facility code %#x (%d)", *c, fac_len);
 		break;
	    } /* ccitt facility case */
	} else if (marker == X25_FACILITY_MARKER_REMOTE) {
	    buginf("\n    Remote facility code %#x (%d)", *c, fac_len);
	} else {
	    buginf("\n    Unknown facility code %#x (%d)", *c, fac_len);
	}

	/*
	 *  adjust the facility encoding pointer and length
	 */
	c += fac_len;
	len -= fac_len;
    }

    return(c);
}

/*
 * x25_facil_thru
 * Translate X.25 facility throughput codes into real numbers
 */

int x25_facil_thru (uint code)
{
    switch (code) {
    case X25_FACILITY_THROUGHPUT_75:
	return(75);
    case X25_FACILITY_THROUGHPUT_150:
	return(150);
    case X25_FACILITY_THROUGHPUT_300:
	return(300);
    case X25_FACILITY_THROUGHPUT_600:
	return(600);
    case X25_FACILITY_THROUGHPUT_1200:
	return(1200);
    case X25_FACILITY_THROUGHPUT_2400:
	return(2400);
    case X25_FACILITY_THROUGHPUT_4800:
	return(4800);
    case X25_FACILITY_THROUGHPUT_9600:
	return(9600);
    case X25_FACILITY_THROUGHPUT_19200:
	return(19200);
    case X25_FACILITY_THROUGHPUT_48000:
	return(48000);
    case X25_FACILITY_THROUGHPUT_64000:
	return(64000);
    }
    return(0);
}

/*
 * x25_facil_speed
 * Translate real number into X.25 facility throughput codes
 */

int x25_facil_speed (hwidbtype *idb, uint speed)
{
    switch (speed) {
    case 75:
	return(X25_FACILITY_THROUGHPUT_75);
    case 150:
	return(X25_FACILITY_THROUGHPUT_150);
    case 300:
	return(X25_FACILITY_THROUGHPUT_300);
    case 600:
	return(X25_FACILITY_THROUGHPUT_600);
    case 1200:
	return(X25_FACILITY_THROUGHPUT_1200);
    case 2400:
	return(X25_FACILITY_THROUGHPUT_2400);
    case 4800:
	return(X25_FACILITY_THROUGHPUT_4800);
    case 9600:
	return(X25_FACILITY_THROUGHPUT_9600);
    case 19200:
	return(X25_FACILITY_THROUGHPUT_19200);
    case 48000:
	return(X25_FACILITY_THROUGHPUT_48000);
    case 64000:
	if (IS_1988(idb))
	    return(X25_FACILITY_THROUGHPUT_64000);
	break;
    }
    return(0);
}
