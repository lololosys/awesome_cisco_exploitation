/* $Id: exec_telnet.h,v 3.5.44.1 1996/04/15 21:15:31 bcole Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_telnet.h,v $
 *------------------------------------------------------------------
 * E X E C _ T E L N E T . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_telnet.h,v $
 * Revision 3.5.44.1  1996/04/15  21:15:31  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.5  1995/12/15  22:27:43  widmer
 * CSCdi45685:  Need priv flag for interactive commands
 * Add Priv_Interactive to flag interactive commands
 *
 * Revision 3.4  1995/11/27  21:24:43  widmer
 * CSCdi43607:  Add keyword_option macro to parse multiple keywords
 * Also create permit_deny macro using keyword_option macro to parse
 * permit/deny keywords.
 * Change "ip security multilevel" command to use keyword_option macro.
 *
 * Revision 3.3  1995/11/20  23:21:48  ahh
 * CSCdi41919:  PT: cannot specify source interface for TCP connections
 * Add source-interface option to outgoing TCP translation and telnet.
 * Also convert some parser defines to enums.
 *
 * Revision 3.2  1995/11/17  09:32:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/13  21:38:59  ahh
 * CSCdi36962:  UDP parsing of tacacs-ds port name broken
 * Restore "tacacs-ds" as a keyword, but hide it and map it to TACACS_PORT
 * for backwards compatibility.
 *
 * Revision 2.1  1995/07/01  12:55:46  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.2  1995/06/15  15:19:24  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  22:26:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Moved
 */
