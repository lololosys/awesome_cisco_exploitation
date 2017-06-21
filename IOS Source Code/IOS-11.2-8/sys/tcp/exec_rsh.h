/* $Id: exec_rsh.h,v 3.1.2.2 1996/08/12 16:08:04 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tcp/exec_rsh.h,v $
 *------------------------------------------------------------------
 * Rsh exec command.
 *
 * January 1996, Bruce Cole
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_rsh.h,v $
 * Revision 3.1.2.2  1996/08/12  16:08:04  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/04/15  21:23:19  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  01:40:27  bcole
 * Placeholders for 11.2 features.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * rsh
 *
 * rsh [<host> [<command>]]
 *
 * OBJ(int, 1) = TRUE if 1st token after rsh matches "/user"
 * OBJ(string, 1) = ip address or hostname of remote rsh server
 * OBJ(string, 2) = remote user name
 * OBJ(string, 3) = remote command
 */

EOLNS    (exec_rsh_eol, rsh_connect_command);
TEXT    (exec_rsh_rcommand_set, exec_rsh_eol, no_alt,
         OBJ(string,3), "Command to be executed remotely");

TEST_MULTIPLE_FUNCS(exec_rsh_test, exec_rsh_rcommand_set,
		    no_alt, NONE);

STRING  (exec_rsh_remote_username, exec_rsh_rcommand_set, no_alt,
         OBJ(string,2), "Remote user name");

KEYWORD_ID (exec_rsh_login_name, exec_rsh_remote_username, exec_rsh_test,
         OBJ(int, 1), TRUE,
         "/user", "Specify remote username", PRIV_ROOT);

STRING  (exec_rsh_host, exec_rsh_login_name, no_alt,
         OBJ(string,1), "IP address or hostname of rsh server");

KEYWORD (exec_rsh, exec_rsh_host, ALTERNATE,
        "rsh", "Execute a remote command", PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	exec_rsh
