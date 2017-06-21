/* $Id: exec_show_rhosts.h,v 3.3.28.2 1996/08/12 16:08:06 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tcp/exec_show_rhosts.h,v $
 *------------------------------------------------------------------
 * E X E C _ R C M D _ H O S T S
 *
 * 6/94 Paul Traina/Andy Heffernan
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_rhosts.h,v $
 * Revision 3.3.28.2  1996/08/12  16:08:06  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.28.1  1996/04/15  21:23:21  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/22  07:29:11  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  18:47:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:56:32  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:25:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*************************************************************************
 * show rhosts
 */
EOLNS	(show_rhosts_eol, show_rhosts);

KEYWORD (show_rhosts_cmd, show_rhosts_eol, ALTERNATE,
	 "rhosts", "Remote-host+user equivalences", PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	show_rhosts_cmd
