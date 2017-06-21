/* $Id: cfg_mapclass_fr.h,v 3.1.66.2 1996/05/15 05:40:20 fowler Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_mapclass_fr.h,v $
 *------------------------------------------------------------------
 * Frame Relay map class configuration commands
 *
 * February 1995, Shankar Natarajan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_mapclass_fr.h,v $
 * Revision 3.1.66.2  1996/05/15  05:40:20  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/01/02  02:16:13  shankar
 * Branch: FR_SVC_branch
 * Sync to mainline
 *
 * Revision 3.1.2.1  1995/12/07  22:36:44  shankar
 * Branch: FR_SVC_branch
 * Support for FR SVCs
 *
 * Revision 3.1  1995/12/04  20:56:06  shankar
 * FR SVC development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(cmc_fr_svcmap_extend_here, no_alt);
LINK_TRANS(cmc_fr_vcqmap_extend_here, cmc_fr_svcmap_extend_here);
/***************************************************************/
KEYWORD	(cmc_fr_mapclass, cmc_fr_vcqmap_extend_here, ALTERNATE,
           "frame-relay", "Configure Map parameters", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE       cmc_fr_mapclass
