/* $Id: cfg_staticmap_fr.h,v 3.1.66.4 1996/08/12 16:02:26 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_staticmap_fr.h,v $
 *------------------------------------------------------------------
 * Frame Relay static map configuration commands
 *
 * February 1995, Shankar Natarajan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_staticmap_fr.h,v $
 * Revision 3.1.66.4  1996/08/12  16:02:26  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.66.3  1996/07/21  16:02:45  shankar
 * CSCdi63256:  Failed to parse map-list subcommands with no prefix
 * Allow keyword trigger to be visible when no prefix is applied
 * Branch: California_branch
 *
 * Revision 3.1.66.2  1996/06/25  14:10:31  shankar
 * CSCdi61103:  Broadcast triggering of SVCs needs a configuration hook
 * for users
 * Branch: California_branch
 *
 * Revision 3.1.66.1  1996/04/25  23:13:14  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.70.1  1996/04/24  04:06:43  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1  1995/12/04  20:56:26  shankar
 *  FR SVC development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS	(cmap_fr_eol, fr_map_command);

pdecl(cmap_fr_nop);

KEYWORD_ID(cmap_fr_ietf, cmap_fr_nop, no_alt,
	   OBJ(int,5), TRUE,
	   "IETF", "RFC 1490 encapsulation", PRIV_CONF);

KEYWORD_ID(cmap_fr_broad_trigger, cmap_fr_nop, NONE, 
		OBJ(int,6), TRUE, "trigger", 
		"allow protocol broadcasts to trigger SVC", PRIV_CONF);

ASSERT	(cmap_fr_broad_check, cmap_fr_broad_trigger, cmap_fr_ietf,
		(GETOBJ(int,4) == TRUE));

KEYWORD_ID(cmap_fr_broad, cmap_fr_nop, cmap_fr_broad_check,
	   OBJ(int,4), TRUE,
	   "broadcast", "broadcast", PRIV_CONF);

NOP     (cmap_fr_nop, cmap_fr_broad, cmap_fr_eol);

STRING	(cmap_fr_class_name, cmap_fr_nop, no_alt,
	 OBJ(string,2), "Frame Relay class name");

KEYWORD	(cmap_fr_class, cmap_fr_class_name, no_alt, 
	 "class", "Frame Relay static map class name", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE	cmap_fr_class
