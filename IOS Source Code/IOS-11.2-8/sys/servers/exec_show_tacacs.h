/* $Id: exec_show_tacacs.h,v 3.1.14.2 1996/08/12 16:07:11 widmer Exp $
 * $Source: /release/112/cvs/Xsys/servers/exec_show_tacacs.h,v $
 *------------------------------------------------------------------
 * Show tacacs command for tacacs+
 *
 * Feb 1996, Lol Grant
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_tacacs.h,v $
 * Revision 3.1.14.2  1996/08/12  16:07:11  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.14.1  1996/04/19  15:29:20  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.1  1996/02/28  06:14:57  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 3.1  1996/02/28  04:44:08  lol
 * Placeholder for IosSec branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS	(show_tacacs_eol, show_tacacs);
KEYWORD (show_tacacs_kwd, show_tacacs_eol, ALTERNATE,
	 "tacacs", "Shows tacacs+ server statistics ", PRIV_USER);


#undef	ALTERNATE
#define	ALTERNATE	show_tacacs_kwd
