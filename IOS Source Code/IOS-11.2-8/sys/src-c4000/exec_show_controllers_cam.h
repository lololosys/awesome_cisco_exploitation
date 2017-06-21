/* $Id: exec_show_controllers_cam.h,v 3.3.20.1 1996/08/12 16:07:29 widmer Exp $
 * $Source: /release/112/cvs/Xsys/src-c4000/exec_show_controllers_cam.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ C O T R O L L E R S _ C A M. H
 *
 * June 1994, Greg Stovall
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_controllers_cam.h,v $
 * Revision 3.3.20.1  1996/08/12  16:07:29  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:41  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:45:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:57:34  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/21  18:17:52  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:23:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show controllers cam [<unit number>][address]
 *
 */
EOLNS	(show_control_cam_eol, show_control_cam);

ENADDR  (show_control_cam_address, show_control_cam_eol, show_control_cam_eol,
	 OBJ(hwaddr,1), NULL /*"MAC address"*/);

UNIT_NUMBER (show_control_cam_num, show_control_cam_address,
	     show_control_cam_address,
	     OBJ(idb,1), IFTYPE_FDDI);

KEYWORD_ID (show_control_cam_fddi,  show_control_cam_num,
	    show_control_cam_address,
	    OBJ(int,1), IDB_FDDI,
	    "fddi", NULL /*"FDDI interface internal state"*/,
	    PRIV_USER | PRIV_HIDDEN);

KEYWORD (show_control_cam, show_control_cam_fddi, ALTERNATE,
	 "cam", NULL /*"Show contents of CAM"*/, PRIV_USER | PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_control_cam
