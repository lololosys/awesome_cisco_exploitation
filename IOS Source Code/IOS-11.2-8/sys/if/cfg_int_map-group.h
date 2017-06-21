/* $Id: cfg_int_map-group.h,v 3.2.60.3 1996/08/12 16:02:12 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_map-group.h,v $
 *------------------------------------------------------------------
 * map-group interface command
 *
 * October 1993, Lionel Bustini
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_map-group.h,v $
 * Revision 3.2.60.3  1996/08/12  16:02:12  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.2  1996/04/25  23:13:08  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.2.60.1.2.1  1996/04/24  04:06:39  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.2.60.1  1996/03/18  20:13:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  01:35:01  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  14:22:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:26:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:10  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/21  18:16:51  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:06:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] map-group <name> 
 */

EOLNS	(ci_mapgroup_eol, mapgroup_command);

STRING	(ci_mapgroup_name, ci_mapgroup_eol, no_alt,
	 OBJ(string, 1), "Static map group name");

NVGENNS  (ci_mapgroup_nvgens, ci_mapgroup_name, mapgroup_command);

KEYWORD (ci_mapgroup, ci_mapgroup_nvgens, NONE,
         "map-group", "Configure static map group", PRIV_CONF|PRIV_SUBIF);

ASSERT (mapgroup_test, ci_mapgroup, ALTERNATE, (is_atm(csb->interface->hwptr) ||
		is_frame_relay(csb->interface->hwptr)));

#undef  ALTERNATE
#define ALTERNATE       mapgroup_test
