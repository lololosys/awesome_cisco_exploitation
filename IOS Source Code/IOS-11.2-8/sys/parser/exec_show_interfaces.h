/* $Id: exec_show_interfaces.h,v 3.4.20.5 1996/08/12 16:06:21 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_show_interfaces.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ I N T E R F A C E S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_interfaces.h,v $
 * Revision 3.4.20.5  1996/08/12  16:06:21  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.20.4  1996/05/07  23:33:05  wilber
 * CSCdi56986:  Hidden command show interface <int> switching is broken
 * New command overwrote its parser variable OBJ(int, x)
 * Branch: California_branch
 *
 * Revision 3.4.20.3  1996/05/04  01:36:49  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Bridge-group Virtual Interface parser/configuration support.
 *
 * Revision 3.4.20.2  1996/04/27  06:39:05  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.2.20.3  1996/04/26  01:22:00  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.2.20.2  1996/04/17  03:57:42  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.2.20.1  1996/03/01  22:26:33  perryl
 * Branch: Dial1_branch
 * Disallow UI configuration for virtual access interface.
 * Allow show and clear command for virtual access interface.
 * Disallow clear command for virtual template interface.
 *
 * Revision 3.4.20.1  1996/03/18  21:33:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.2  1996/03/13  01:51:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.2.1  1996/02/20  16:47:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/01/29  07:09:22  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.3  1996/01/22  06:57:52  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  18:47:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/25  08:36:26  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.2  1995/08/18  23:54:30  speakman
 * CSCdi39005:  CRB commands collide
 * Require minimum 3 letter match for 'show interface crb'.
 *
 * Revision 2.1  1995/06/07  22:24:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*************************************************************************
 * show interfaces [<interface>] [accounting] [stats] [switching] [<num>]
 * 
 * OBJ(idb,1) = <interface>
 */

EOLNS	(show_interfaces_eol, show_interface);

NUMBER	(show_interface_bri_num2, show_interfaces_eol, show_interfaces_eol,
	 OBJ(int,3), 1, 2, "Ending B-channel number" );
NUMBER	(show_interface_bri_num1, show_interface_bri_num2, NONE,
	 OBJ(int,2), 1, 2, "Starting B-channel number" );
TESTVAR	(show_interface_bri_test, show_interface_bri_num1, NONE,
	 NONE, NONE, show_interfaces_eol, OBJ(int,1), TRUE );
EVAL	(show_interface_bri_eval, show_interface_bri_test,
	 (GETOBJ(int,1) =
	  ((GETOBJ(idb,1) ?
	    (GETOBJ(idb,1)->hwptr->type == IDBTYPE_BRI) : FALSE) ? TRUE :
	   FALSE)) );


NUMBER	(show_interface_e1_num2, show_interfaces_eol, show_interfaces_eol,
	 OBJ(int,5), 1, 31, "Last B-channel to be displayed." );
NUMBER	(show_interface_bchan_e1, show_interface_e1_num2, NONE,
	 OBJ(int,4), 1, 31, "B-channel or first B-channel in a range "
	 "to be displayed." );
ASSERT    (show_interface_bchan_e1_test, show_interface_bchan_e1, 
	   show_interface_bri_eval,
	   (GETOBJ(idb,1)->hwptr->type == IDBTYPE_E1));

NUMBER	(show_interface_pri_num2, show_interfaces_eol, show_interfaces_eol,
	 OBJ(int,5), 1, 23, "Last B-channel to be displayed." );
NUMBER	(show_interface_bchan_t1, show_interface_pri_num2, NONE,
	 OBJ(int,4), 1, 23, "B-channel or first B-channel to be displayed." );

ASSERT    (show_interface_bchan_pri, show_interface_bchan_t1, 
	   show_interface_bchan_e1_test,
	   (GETOBJ(idb,1)->hwptr->type == IDBTYPE_T1));
KEYWORD(show_interface_bchan, show_interface_bchan_pri, no_alt,
	   "bchan", "Show bchan interfaces", PRIV_USER );
ASSERT    (show_interface_bchan_test, show_interface_bchan, 
	   show_interface_bri_eval,
	   (GETOBJ(idb,1) != NULL) && (GETOBJ(idb,1)->hwptr->isdn_info));

KEYWORD_ID_MM(show_interface_irb, show_interfaces_eol,
	      no_alt, OBJ(int,13), TRUE,
	      "irb", "Show interface routing/bridging info", PRIV_USER, 3);

KEYWORD_ID_MM(show_interface_crb, show_interfaces_eol,
	      show_interface_irb, OBJ(int,11), TRUE,
              "crb", "Show interface routing/bridging info", PRIV_USER, 3);

KEYWORD_ID(show_interface_accounting, show_interfaces_eol,
	   show_interface_crb, OBJ(int,10), TRUE,
	   "accounting", "Show interface accounting", PRIV_USER );

KEYWORD_ID(show_interface_switching, show_interfaces_eol,
	   show_interface_accounting, OBJ(int, 12), TRUE, "switching",
	   NULL, PRIV_USER | PRIV_HIDDEN);

KEYWORD_ID(show_interface_stats, show_interfaces_eol, 
	   show_interface_switching, OBJ(int,9), TRUE,
	   "stats", NULL, PRIV_USER | PRIV_HIDDEN); 

ASSERT	(show_interface_stats_test, show_interface_stats,
	 show_interface_bchan_test,
	 (GETOBJ(idb,1) == NULL) || !is_subinterface(GETOBJ(idb,1)));

INTERFACE (show_interface_get_interface, show_interface_stats_test,
	   show_interface_stats_test, OBJ(idb,1),
	   ((IFTYPE_ANYSUB | IFTYPE_VACCESS) & ~IFTYPE_LEX));

KEYWORD (show_interfaces, show_interface_get_interface, ALTERNATE,
	 "interfaces", "Interface status and configuration",
	 PRIV_USER|PRIV_USER_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_interfaces
