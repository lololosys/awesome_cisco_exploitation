/* $Id: cfg_router_offset.h,v 3.3.20.4 1996/09/04 23:46:26 mikel Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_offset.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ O F F S E T . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_offset.h,v $
 * Revision 3.3.20.4  1996/09/04  23:46:26  mikel
 * CSCdi65889:  offset-list doesnt work for EIGRP
 * Branch: California_branch
 * Re-enable offset-list command as well as adding offset-list out support,
 * and fix offset-list to work with access-lists
 *
 * Revision 3.3.20.3  1996/08/12  16:03:48  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.20.2  1996/05/04  01:21:23  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.3.20.1  1996/04/09  05:54:35  mikel
 * CSCdi49628:  EIGRP ignores outgoing offset-list
 * Branch: California_branch
 * disable offset-list command from eigrp router config
 *
 * Revision 3.3  1996/02/01  23:33:59  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  17:32:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:58  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:12:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * offset-list {<list #>|<list name>} { in | out } <offset> <interface>
 * no offset-list {<list #>|<list name>} { in | out } [<offset>] <interface>
 *
 * OBJ(int,1) = <list #>
 * OBJ(string,1) = <list name>
 * OBJ(int,2) = IN or OUT
 * OBJ(int,3) = <offset>
 * OBJ(idb,1) = <interface>
 */
EOLNS	(rtr_offset_eol, offsetlist_command);

/* <interface-name><unit> */
INTERFACE (rtr_offset_if, rtr_offset_eol, rtr_offset_eol,
	   OBJ(idb,1), IFTYPE_ANYSUB);

TESTVAR	(rtr_offset_noprefix, rtr_offset_if,
	 no_alt, NONE, NONE, NONE, sense, FALSE);

/* <offset> */
NUMBER	(rtr_offset_igrp_offset, rtr_offset_if, rtr_offset_noprefix,
	 OBJ(int,3), 0, MAXINT, "Offset");

/* Magic number 16 is RIP maximum metric. */
NUMBER	(rtr_offset_rip_offset, rtr_offset_if, rtr_offset_noprefix,
	 OBJ(int,3), 0, 16, "Offset");

IFELSE (rtr_offset_rip, rtr_offset_rip_offset, rtr_offset_igrp_offset,
	(((pdbtype *)csb->protocol)->proctype & PROC_RIP));


/* { in | out } */
KEYWORD_ID(rtr_offset_out, rtr_offset_rip, no_alt,
	   OBJ(int,2), OUT,
	   "out", "Perform offset on outgoing updates", PRIV_CONF);

KEYWORD_ID(rtr_offset_in, rtr_offset_rip, rtr_offset_out,
	   OBJ(int,2), IN,
	   "in", "Perform offset on incoming updates", PRIV_CONF);

/* <list> */
GENERAL_STRING (rtr_offset_accname, rtr_offset_in, no_alt,
		OBJ(string,1), "Access-list name", STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS(rtr_offset_stest, rtr_offset_accname, NONE,
		    no_alt);
NUMBER	(rtr_offset_list, rtr_offset_in, rtr_offset_stest,
	 OBJ(int,1), 0, MAXFASTACC, 
	 "Access list of networks to apply offset (0 selects all networks)");

NVGENNS	(rtr_offset_nvgen, rtr_offset_list, offsetlist_command);

KEYWORD	(rtr_offset, rtr_offset_nvgen, NONE,
	"offset-list", 
	 "Add or subtract offset from IGRP or RIP metrics", PRIV_CONF);

ASSERT	(router_offset, rtr_offset, ALTERNATE,
	 ((pdbtype *) csb->protocol)->family == PDB_IP &&
	 (((pdbtype *) csb->protocol)->proctype == PROC_IGRP ||
	  ((pdbtype *) csb->protocol)->proctype == PROC_IGRP2 ||
	  ((pdbtype *) csb->protocol)->proctype == PROC_RIP));

#undef	ALTERNATE
#define	ALTERNATE	router_offset
