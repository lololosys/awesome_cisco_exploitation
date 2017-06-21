/* $Id: cfg_staticmap_atm.h,v 3.3.6.1 1996/08/12 16:02:25 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_staticmap_atm.h,v $
 *------------------------------------------------------------------
 * ATM static maps configuration commands
 *
 * October 1993, Lionel Bustini
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_staticmap_atm.h,v $
 * Revision 3.3.6.1  1996/08/12  16:02:25  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/03/02  00:50:47  rzagst
 * CSCdi49815:  map-list statements permit inappropriate options for pvcs
 *
 * Revision 3.2  1995/11/17  09:26:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:15  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:14:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] <protocol> <protocol-address>
 *	{atm-vc <vc number> [broadcast] |
 *	 atm-nsap <nsap address> [class <name>] [aal5mux] [broadcast] |
 *	 smds <smds address> [broadcast]}
 *
 * (<protocol> and <protocol-address> are parsed in cfg_static-map.h)
 */
EOLNS	(cmap_atm_eol, atm_map_command);

KEYWORD_ID(cmap_atm_broad, cmap_atm_eol, cmap_atm_eol,
	   OBJ(int,4), TRUE,
	   "broadcast", "Pseudo-broadcast", PRIV_CONF);



/*
 * smds followed by a SMDS address, optionally broadcast at the end
 */
NOPREFIX(cmap_atm_vc_noprefix, cmap_atm_broad, cmap_atm_eol);

SMDSADDR(cmap_atm_smdsaddr, cmap_atm_vc_noprefix, no_alt,
	 OBJ(hwaddr,1), "SMDS E.164 address");

KEYWORD_ID(cmap_atm_smds, cmap_atm_smdsaddr, ALTERNATE,
           OBJ(int,2), ATM_MAP_SMDS,
           "smds", "ATM SMDS E.164", PRIV_CONF);


/*
 * atm-nsap followed by an nsap address, then optionally an ATM class and
 * then optionally the AAL5MUX keyword.  Then optionally a broadcast keyword
 * at the end.
 */
KEYWORD_ID(cmap_atm_aal5mux, cmap_atm_broad, cmap_atm_broad,
        OBJ(int,5), TRUE, "aal5mux",
        "AAL5+MUX Encapsulation", PRIV_CONF);

STRING	(cmap_atm_class_name, cmap_atm_aal5mux, no_alt,
	 OBJ(string,2), "ATM class name");

KEYWORD	(cmap_atm_class, cmap_atm_class_name, cmap_atm_aal5mux, 
	 "class", "ATM static map class name", PRIV_CONF);

NOPREFIX(cmap_atm_noprefix, cmap_atm_class, cmap_atm_eol);

ATMNSAPADDR(cmap_atm_nsapaddr, cmap_atm_noprefix, no_alt,
	    OBJ(hwaddr,1), "20-octet ATM NSAP");

KEYWORD_ID(cmap_atm_nsap, cmap_atm_nsapaddr, cmap_atm_smds,
           OBJ(int,2), ATM_MAP_NSAP,
           "atm-nsap", "ATM NSAP", PRIV_CONF);

/*
 * atm-vc followed by a vc number, optionally broadcast at the end
 */
static void cmap_atm_vc_range (parseinfo *csb, uint *lower, uint *upper)
{
    int i;
    
    *lower = 1;
	
    /*
     * check through all atm interface and find the largest
     * number of vcs any interface can support and use that
     * as the upper prompt
     */
    *upper = 1;
    for (i = 0;i < MAX_ATM_INTFC;i++) {
	if (&atm_db[i] != NULL) {
	    if (atm_db[i].config.max_intf_vcs > *upper)
		*upper = atm_db[i].config.max_intf_vcs;
	}
    }
    /* if no atm interface is installed */
    if (*upper == 1)
	*upper = ATM_MAX_VC_DEFAULT - 1;
    else
	(*upper)--;
    
}


NUMBER_FUNC(cmap_atm_vcnum, cmap_atm_vc_noprefix, no_alt,
         OBJ(int,3), cmap_atm_vc_range, "ATM PVC number");

KEYWORD_ID(cmap_atm_vc, cmap_atm_vcnum, cmap_atm_nsap,
	   OBJ(int,2), ATM_MAP_VC,
	   "atm-vc", "ATM VC", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE	cmap_atm_vc
