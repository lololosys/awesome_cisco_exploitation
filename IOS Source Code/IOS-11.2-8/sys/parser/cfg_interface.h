/* $Id: cfg_interface.h,v 3.2.62.4 1996/08/12 16:05:48 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_interface.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T E R F A C E . H
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_interface.h,v $
 * Revision 3.2.62.4  1996/08/12  16:05:48  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.62.3  1996/06/07  17:29:51  irfan
 * CSCdi58223:  Bus error crash while using chat script to dial out under
 * group asy
 * Branch: California_branch
 * Don't allow async group interfaces to be used for anything except
 * group definitions.
 *
 * Revision 3.2.62.2  1996/05/21  09:59:13  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.62.1  1996/03/18  21:33:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  01:51:33  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  16:43:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:43:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/06  00:14:13  achopra
 * CSCdi36651:  Frame relay subifs should require a type only when being
 * created
 *
 * Revision 2.2  1995/06/19  18:36:28  achopra
 * CSCdi32283:  Frame relay subinterfaces should have no default value
 *
 * Revision 2.1  1995/06/07  22:08:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * interface  <type><optional-whitespace><unit> [{point-to-point|multipoint}]
 *
 *	<type>	"ethernet"
 *		"serial"
 *		"tokenring"
 *		"fddi"
 *              "hssi"
 *              "ultra"
 *
 *	<unit>	number
 *
 *      OBJ(int,5) = subinterface type
 */
EOLNS	(act_interface, interface_command);

KEYWORD_ID(cfg_int_subif_multi, act_interface, NONE,
	   OBJ(int,5), SUBIF_TYPE_MULTI,
	   "multipoint", "Treat as a multipoint link", PRIV_CONF);

KEYWORD_ID(cfg_int_subif_p2p, act_interface, cfg_int_subif_multi,
	   OBJ(int,5), SUBIF_TYPE_P2P,
	   "point-to-point", "Treat as a point-to-point link", PRIV_CONF);
IFELSE  (check_subif_type, NONE, act_interface,
	 GETOBJ(idb,1) && is_subinterface(GETOBJ(idb,1)) &&
	 can_select_subif_type(GETOBJ(idb,1)) && 
	 (is_frame_relay((GETOBJ(idb,1))->hwptr)) && 
	 !((GETOBJ(idb,1))->subif_link_type));

ASSERT	(cfg_int_subif_test, cfg_int_subif_p2p, check_subif_type,
	 GETOBJ(idb,1) &&
	 is_subinterface(GETOBJ(idb,1)) &&
	 can_select_subif_type(GETOBJ(idb,1)));

NOPREFIX(cfg_int_no, cfg_int_subif_test, act_interface);

NVGENNS   (cfg_int_nvgens, cfg_int_no, interface_command);

/*
 * Do not show vty-async interface subcommand under interface command.
 * Vty-async interfaces are not nvgened and user should not be allowd 
 * to configure them.
 */
FORWARD_INTERFACE(config_interface_int, cfg_int_nvgens, no_alt, OBJ(idb,1),
		  (IFTYPE_ANYSUB | IFTYPE_ASYNC_GROUP) & ~IFTYPE_VTY_ASYNC);

KEYWORD_ID(configure_interface, config_interface_int, ALTERNATE, 
	   leave_subcommand, TRUE,
	   common_str_interface, "Select an interface to configure", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	configure_interface
