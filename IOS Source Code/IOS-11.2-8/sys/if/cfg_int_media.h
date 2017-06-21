/* $Id: cfg_int_media.h,v 3.4.10.2 1996/08/12 16:02:13 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_media.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ M E D I A . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_media.h,v $
 * Revision 3.4.10.2  1996/08/12  16:02:13  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.10.1  1996/03/21  22:48:35  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/03/18  23:23:09  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.1  1996/01/29  19:03:42  wfried
 * Branch: ELC_branch
 * Use HW address filtering, disable rx side for better input throttling,
 * add loopback code, correct ignored frames counting, allow only MII media
 * on the Predator I/O card FE interface.
 *
 * Revision 3.4  1996/02/23  10:20:05  feisal
 * CSCdi48332:  CLI changes needed for FastEther Port Adapter Variants
 *
 * Revision 3.3  1996/02/13  21:25:43  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.2  1995/11/17  09:26:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/09  00:20:08  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.1  1995/07/02  10:52:31  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:06:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * media-type { AUI | 10BaseT | 100BaseT | MII }
 *
 */
EOLNS	(ci_media_eol, media_type_command);

KEYWORD_ID(ci_media_10baseT, ci_media_eol, no_alt,
	   OBJ(int,1), PARSER_MEDIATYPE_10BASET,
	   "10BaseT", "Use RJ45 connector", PRIV_CONF);

KEYWORD_ID(ci_media_aui, ci_media_eol, ci_media_10baseT,
	   OBJ(int,1), PARSER_MEDIATYPE_AUI,
	   "AUI", "Use AUI connector", PRIV_CONF);

KEYWORD_ID(ci_media_100baseT, ci_media_eol, no_alt,
	   OBJ(int,1), PARSER_MEDIATYPE_100BASET,
	   "100BaseX", "Use RJ45 for -TX; SC FO for -FX", PRIV_CONF);

IFELSE  (ci_media_pred, no_alt, ci_media_100baseT,
         (cpu_type == CPU_C7100) && (csb->interface->hwptr->slot == 0));

KEYWORD_ID(ci_media_mii, ci_media_eol, ci_media_pred,
	   OBJ(int,1), PARSER_MEDIATYPE_MII,
	   "MII", "Use MII connector", PRIV_CONF);

IFELSE	(ci_media_lance, ci_media_aui, no_alt,
	 csb->interface->hwptr->type == IDBTYPE_LANCE);

IFELSE	(ci_media_feip, ci_media_mii, ci_media_lance,
	 csb->interface->hwptr->type == IDBTYPE_FEIP);

KEYWORD	(ci_media, ci_media_feip, NONE,
	 "media-type", "Interface media type", PRIV_CONF);

ASSERT	(ci_media_assert, ci_media, ALTERNATE,
         ((csb->interface->hwptr->type == IDBTYPE_LANCE) ||
          (csb->interface->hwptr->type == IDBTYPE_FEIP)) );

#undef  ALTERNATE
#define ALTERNATE ci_media_assert
