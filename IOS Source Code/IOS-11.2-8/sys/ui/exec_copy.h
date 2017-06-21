/* $Id: exec_copy.h,v 3.3.4.1 1996/04/15 21:25:02 bcole Exp $
 * $Source: /release/112/cvs/Xsys/ui/exec_copy.h,v $
 *------------------------------------------------------------------
 * E X E C _ C O P Y . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_copy.h,v $
 * Revision 3.3.4.1  1996/04/15  21:25:02  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.3  1996/03/09  22:58:04  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.2  1995/11/17  17:47:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/11  16:45:34  pst
 * CSCdi39650:  config overwrite is braindead
 *
 * Revision 2.1  1995/07/04  01:59:24  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:16:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This extension point is for the ambiguous "copy file ..." commands
 * using PARSE_ADD_COPYFILE_CMD.
 */
LINK_TRANS(exec_copyfile_extend_here, NONE);
/*
 * All non-ambiguous parse chains get extended here via PARSE_ADD_COPY_CMD.
 */
LINK_TRANS(exec_copy_extend_here, exec_copyfile_extend_here);

LINK_TRANS(exec_copy_startup_file_extend_here, NONE);
/* extend "copy startup-config ..." from here */
LINK_TRANS(exec_copy_startup_extend_here, exec_copy_startup_file_extend_here);

/* copy startup-config running-config */
EOLS    (exec_copy_start_conf_run_eol, manual_configure, PARSER_CONF_MEM);
KEYWORD (exec_copy_start_conf_run, exec_copy_start_conf_run_eol,
	 exec_copy_startup_extend_here,
	 "running-config", 
	 "Update (merge with) current system configuration", PRIV_ROOT);

KEYWORD_ID (exec_copy_startup_config, exec_copy_start_conf_run,
	    exec_copy_extend_here,
	    OBJ(int,2), COPY_STARTUP_CONFIG,
	    "startup-config", "Copy from startup configuration", PRIV_ROOT);

LINK_TRANS(exec_copy_running_file_extend_here, NONE);
/* extend "copy running-config ..." from here */
LINK_TRANS(exec_copy_running_extend_here, exec_copy_running_file_extend_here);

/* copy running-config startup-config */
EOLS    (exec_copy_run_conf_strt_eol, write_memory, WRITE_MEMORY);
KEYWORD (exec_copy_run_conf_strt, exec_copy_run_conf_strt_eol,
	 exec_copy_running_extend_here,
	 "startup-config", 
	 "Copy to startup configuration", PRIV_ROOT);

KEYWORD_ID (exec_copy_running_config, exec_copy_run_conf_strt,
	    exec_copy_startup_config,
	    OBJ(int,2), COPY_RUNNING_CONFIG,
	    "running-config", "Copy from current system configuration",
	    PRIV_ROOT);

KEYWORD	(exec_copy, exec_copy_running_config, ALTERNATE,
	 "copy", "Copy configuration or image data",
	 PRIV_ROOT | PRIV_INTERACTIVE);

#undef	ALTERNATE
#define	ALTERNATE	exec_copy
