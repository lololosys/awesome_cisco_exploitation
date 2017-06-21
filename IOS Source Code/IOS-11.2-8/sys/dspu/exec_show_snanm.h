/* $Id: exec_show_snanm.h,v 3.2 1995/11/17 09:05:32 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/exec_show_snanm.h,v $
 *------------------------------------------------------------------
 * SNA Network Management Show parser
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_snanm.h,v $
 * Revision 3.2  1995/11/17  09:05:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:53:08  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/21  18:18:06  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:25:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show sna [pu <name>] [all]
 * 
 * OBJ(int,2) = TRUE if "all"; "show all the LU details"
 * OBJ(string,1) = <pu_name>
 */

EOLS	(show_sna_eol, snanm_show_command, SHOW_DSPU);

KEYWORD_ID  (show_sna_all, show_sna_eol, show_sna_eol, 
	OBJ(int, 2), TRUE, "all", "Show SNA All Command",PRIV_USER);
STRING	(show_sna_pu_name, show_sna_all, no_alt,
	 OBJ(string, 1), "Show SNA PU Name");
KEYWORD_ID (show_sna_pu, show_sna_pu_name, show_sna_all,
	 OBJ(int, 1), TRUE,"pu","Show host PU Name command",PRIV_USER);

/******************************************************************/
KEYWORD (show_sna, show_sna_pu, ALTERNATE,
	 "sna", "Display SNA host information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_sna

