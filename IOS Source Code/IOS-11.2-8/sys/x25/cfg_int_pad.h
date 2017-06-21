/* $Id: cfg_int_pad.h,v 3.1.2.1 1996/04/11 23:47:38 gglick Exp $
 * $Source: /release/112/cvs/Xsys/x25/cfg_int_pad.h,v $
 *------------------------------------------------------------------
 * PAD client interface parsing support
 *
 * April 1996, Gilbert Glick
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_pad.h,v $
 * Revision 3.1.2.1  1996/04/11  23:47:38  gglick
 * CSCdi54514:  X.25 parser modularity files
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/11  23:40:00  gglick
 * Initial commit of X.25 parser modularization files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/************************************************************************
 *      ...[options]
 * (PAD map and PVC command option external linkage)
 */

LINK_EXIT(ci_x25pad_mappvc_exit, no_alt);
NOP     (x25pad_mappvc_opt, ci_x25pad_mappvc_exit, no_alt);

/************************************************************************
 *      (x25 pvc <#>) pad [<X.121-addr>]...
 * PAD PVC command addresses
 *
 * For a PVC command;
 *  csb->uiarray[0] is set to the permissible PVC options
 *  csb->uiarray[1] is set to LINK_PAD
 *  OBJ(paddr,1) is set to the remote host's X.121 address, if specified
 */

SET	(x25pad_pvc_opt_set, x25pad_mappvc_opt,
	 uiarray[0], X25OPT_PAD_OPTIONS & X25OPT_PVC_OPTIONS);
X121ADDR(x25pad_pvc_x121, x25pad_pvc_opt_set, x25pad_pvc_opt_set,
	 OBJ(paddr,2), "Destination host address");

PROTONAME(x25pad_pvc_proto, x25pad_pvc_x121, NONE, uiarray[1], PMATCH_PAD);

/************************************************************************
 *      (x25 map) pad <X.121-addr>...
 * PAD map command address
 *
 * For a map command:
 *  csb->uiarray[0] is set to the permissible map options
 *  csb->uiarray[1] is set to LINK_PAD
 *  OBJ(paddr,1) is set to the remote host's X.121 address
 */

SET	(x25pad_map_opt_set, x25pad_mappvc_opt,
	 uiarray[0], X25OPT_PAD_OPTIONS);
PROTOADDR(x25pad_map_proto, x25pad_map_opt_set, NONE,
	  uiarray[1], OBJ(paddr,1), PMATCH_PAD);

/************************************************************************/

/* OBJ(int,1) is 0 for a map, non-zero for a PVC */
SPLIT	(x25pad_mappvc_split, x25pad_map_proto, x25pad_pvc_proto,
	 OBJ(int,1), 0);

NOP	(x25pad_mappvc, x25pad_mappvc_split, ALTERNATE);

#undef	ALTERNATE
#define	ALTERNATE	x25pad_mappvc
LINK_POINT(x25pad_mappvc_enter, ALTERNATE);
#undef	ALTERNATE
#define	ALTERNATE	NONE

/************************************************************************
 *	x25 pad-access
 */

EOLS	(x25pad_access_eol, pad_configure, X25PAD_PADACCESS);
KEYWORD	(x25pad_access, x25pad_access_eol, NONE,
	 "pad-access",
	 "Accept only PAD connections from statically mapped X25 hosts",
	 PRIV_CONF);

/************************************************************************/

KEYWORD (x25pad_keyword, x25pad_access, NONE,
	 "x25", "", PRIV_CONF | PRIV_SUBIF | PRIV_DUPLICATE);

ASSERT (x25pad_keyword_test, x25pad_keyword, ALTERNATE,
	hwidb_is_x25_inline(csb->interface->hwptr));


#undef	ALTERNATE
#define	ALTERNATE	x25pad_keyword_test
