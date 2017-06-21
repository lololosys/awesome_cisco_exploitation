/*
 * $Id: exec_startappn.h,v 3.2.40.1 1996/08/12 16:00:20 widmer Exp $
 * $Source: /release/112/cvs/Xsys/appn/exec_startappn.h,v $
 *------------------------------------------------------------------
 * exec_startappn.c - execute command to start APPN
 *
 * 8-Feb-1994, Leo Pereira, Meng Lyu
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 *
 * $Log: exec_startappn.h,v $
 * Revision 3.2.40.1  1996/08/12  16:00:20  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1996/02/01  23:32:50  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.1  1996/02/01  04:56:52  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/29  07:09:30  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:47:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/11  02:22:43  dmccowan
 * CSCdi30561:  need to support APING on our router
 *
 * Revision 2.2  1995/06/19  17:22:13  dmccowan
 * CSCdi29500:  appn stop and appn start should not be allowed in
 * non-enabled mode
 * CSCdi32349:  make internal debug tools hidden commands
 * fix permissions of appn commands.
 *
 * Revision 2.1  1995/06/07  22:26:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/******************************************************************
 * appn trace XXX
 */

EOLNS	(appn_trace_eol, appn_trace_cmd);
HEXNUM  (appn_trace_value, appn_trace_eol, no_alt,
	     OBJ(int,1), "trace setting");
KEYWORD (appn_trace, appn_trace_value, NONE,
	 "trace", "Set APPN traces", PRIV_ROOT|PRIV_HIDDEN);

/******************************************************************
 * appn tftp-dump [ip-addr <host addr>] [<filename>]
 */

EOLNS	(appn_tftp_eol, tftp_appn_shared_mem);

TEXT	(appn_tftp_file, appn_tftp_eol, no_alt,
	 OBJ(string,1), "");

IPADDR_NAME (appn_tftp_host, appn_tftp_file, appn_tftp_eol,
	     OBJ(paddr,1), "");

KEYWORD (tftp_dump_appn, appn_tftp_host, appn_trace,
	 "tftp-dump", "APPN shared memory dump", PRIV_ROOT|PRIV_HIDDEN);


/******************************************************************
 * appn dump debug
 *           buffer
 *           directory
 *           isr
 *           links
 *           memory
 *           node
 *           processes
 *           sessions
 *           topology
 *
 */

EOLS	(dump_appn_eol, dump_appn_commands, SHOW_APPN);

KEYWORD_ID(dump_appn_buffer, dump_appn_eol, no_alt,
	   OBJ(int,1), DUMP_APPN_BUFFER,
	   "buffer", "Show APPN buffer statistics", PRIV_ROOT|PRIV_HIDDEN);
KEYWORD_ID(dump_appn_trs, dump_appn_eol, dump_appn_buffer,
	   OBJ(int,1), DUMP_APPN_TRS,
	   "topology", "Show APPN topology database", PRIV_ROOT|PRIV_HIDDEN);
KEYWORD_ID(dump_appn_sess, dump_appn_eol, dump_appn_trs,
	   OBJ(int,1), DUMP_APPN_SESS,
	   "sessions", "Show APPN local session info", PRIV_ROOT|PRIV_HIDDEN);
KEYWORD_ID(dump_appn_proc, dump_appn_eol, dump_appn_sess,
	   OBJ(int,1), DUMP_APPN_PROC,
	   "processes", "Show APPN process info", PRIV_ROOT|PRIV_HIDDEN);
KEYWORD_ID(dump_appn_tree, dump_appn_eol, dump_appn_proc,
	   OBJ(int,1), DUMP_APPN_TREE,
	   "tree", "Show APPN process tree", PRIV_ROOT|PRIV_HIDDEN);
KEYWORD_ID(dump_appn_node, dump_appn_eol, dump_appn_tree,
	   OBJ(int,1), DUMP_APPN_NODE,
	   "node", "Show APPN control point info", PRIV_ROOT|PRIV_HIDDEN);
KEYWORD_ID(dump_appn_memory, dump_appn_eol, dump_appn_node,
	   OBJ(int,1), DUMP_APPN_MEMORY,
	   "memory", "Show APPN memory allocation", PRIV_ROOT|PRIV_HIDDEN);
KEYWORD_ID(dump_appn_links, dump_appn_eol, dump_appn_memory,
	   OBJ(int,1), DUMP_APPN_LINKS,
	   "links", "Show APPN link info", PRIV_ROOT|PRIV_HIDDEN);
KEYWORD_ID(dump_appn_isr, dump_appn_eol, dump_appn_links,
	   OBJ(int,1), DUMP_APPN_ISR,
	   "isr", "Show APPN ISR info", PRIV_ROOT|PRIV_HIDDEN);
KEYWORD_ID(dump_appn_ds, dump_appn_eol, dump_appn_isr,
	   OBJ(int,1), DUMP_APPN_DS,
	   "directory", "Show APPN directory database", PRIV_ROOT|PRIV_HIDDEN);
KEYWORD_ID(dump_appn_debug, dump_appn_eol, dump_appn_ds,
	   OBJ(int,1), DUMP_APPN_DEBUG,
	   "debug", "Show APPN debug print mask", PRIV_ROOT|PRIV_HIDDEN);

KEYWORD (dump_appn, dump_appn_debug, tftp_dump_appn,
	 "dump", "APPN dump commands", PRIV_ROOT|PRIV_HIDDEN);

/***************************************************************
 * appn aping [<flags>] <destination>
 *
 *  OBJ(string,1)
 *  destination
 *        may be either a symbolic destination name or a partner LU name
 *  OBJ(int,6) == TRUE --> blank mode
 *  OBJ(string,2)
 *  -m mode_name
 *        mode name (default: #INTER)
 *  OBJ(string,3)
 *  -t tp_name
 *        the TP to start on the server (default: APINGD)
 *  OBJ(int,1)
 *  -s N
 *        N is the size of the packet transmitted (default:
 *        100 bytes)
 *  OBJ(int,2)
 *  -i N
 *        N is the number of iterations (default: 2)
 *  OBJ(int,3)
 *  -c N
 *        N is the number of consecutive packets sent by each
 *        side (default: 1)
 *  OBJ(int,4)
 *  -1
 *        Only send data from client to server (No echo)
 *  OBJ(string,4)
 *  -u userid
 *  OBJ(string,5)
 *  -p password
 *        Security parameters.
 *  OBJ(int,5)
 *  -n
 *        Do not use any security (SECURITY=NONE).
 *  OBJ(int,7)
 *  -w
 *        Synchronous appn ping option
 *
 *  OBJ(int,7)
 *  -d
 *        Deactivate APING session when done.
 *
 *  OBJ(int,8)
 *  -r
 *        show route taken by APING.
 *
 */
EOLS	(exec_aping_eol, startappn_command, EXEC_APING);
pdecl   (exec_aping_flags);

APPN_FQNAME(exec_aping_dest, exec_aping_eol, no_alt,
         OBJ(string,1), "Name of destination LU");

/*
 * -m <mode_name>
 */
APPN_TYPE_A_NAME (exec_aping_mode_name, exec_aping_flags,
            no_alt, OBJ(string,2), "MODE (default: #INTER)");
SET     (exec_aping_mode_name1, exec_aping_mode_name,
         OBJ(int,6), FALSE);
SET     (exec_aping_mode_blank, exec_aping_flags,
         OBJ(int,6), TRUE);
NOP     (exec_aping_mode, exec_aping_mode_blank,
         exec_aping_mode_name1);
KEYWORD	(exec_aping_m, exec_aping_mode, no_alt,
	 "-m", "Specify APPN mode to use", PRIV_ROOT);
#undef  NEXT_FLAG
#define NEXT_FLAG exec_aping_m

/*
 * -t <tp_name>
 */
STRING  (exec_aping_tp, exec_aping_flags,
            no_alt, OBJ(string,3), "TPNAME (default: APINGD)");
KEYWORD	(exec_aping_t, exec_aping_tp, NEXT_FLAG,
	 "-t", "Specify TP to start on the server", PRIV_ROOT);
#undef  NEXT_FLAG
#define NEXT_FLAG exec_aping_t

/*
 * -s <size>
 */
NUMBER    (exec_aping_size_val, exec_aping_flags, no_alt,
	    OBJ(int,1), 100, 32767,
	    "Size of the packet (default: 100)");
KEYWORD	(exec_aping_size, exec_aping_size_val, NEXT_FLAG,
	 "-s", "Specify the size of packet transmitted", PRIV_ROOT);
#undef  NEXT_FLAG
#define NEXT_FLAG exec_aping_size

/*
 * -i <num_interations>
 */
NUMBER    (exec_aping_iters_val, exec_aping_flags, no_alt,
	    OBJ(int,2), 1, 20,
	    "Number of iterations (default: 2)");
KEYWORD	(exec_aping_iters, exec_aping_iters_val, NEXT_FLAG,
	 "-i", "Specify number of iterations", PRIV_ROOT);
#undef  NEXT_FLAG
#define NEXT_FLAG exec_aping_iters

/*
 * -c <num_packets>
 */
NUMBER    (exec_aping_consec_val, exec_aping_flags, no_alt,
	    OBJ(int,3), 1, 100,
	    "Number of consecutive packets (default: 1)");
KEYWORD	(exec_aping_consec, exec_aping_consec_val, NEXT_FLAG,
	 "-c", "Specify number of consecutive packets sent", PRIV_ROOT);
#undef  NEXT_FLAG
#define NEXT_FLAG exec_aping_consec

/*
 * -1
 */
KEYWORD_ID (exec_aping_noecho, exec_aping_flags,
            NEXT_FLAG, OBJ(int,4), TRUE, "-1",
            "Only send data from client to server (No echo)", PRIV_ROOT);
#undef  NEXT_FLAG
#define NEXT_FLAG exec_aping_noecho


/*
 * -u <userid> -p <password>
 */
STRING  (exec_aping_passwd, exec_aping_flags,
            no_alt, OBJ(string,4), "PASSWORD to use in transaction.");
KEYWORD	(exec_aping_p, exec_aping_passwd, no_alt,
	 "-p", "Specify PASSWORD", PRIV_ROOT);
STRING  (exec_aping_userid, exec_aping_p, no_alt,
        OBJ(string,5), "USERID to use in transaction");
KEYWORD	(exec_aping_u, exec_aping_userid, NEXT_FLAG,
	 "-u", "Specify USERID", PRIV_ROOT);
#undef  NEXT_FLAG
#define NEXT_FLAG exec_aping_u

/*
 * -n
 */
KEYWORD_ID (exec_aping_nosec, exec_aping_flags,
            NEXT_FLAG, OBJ(int,5), TRUE, "-n",
            "Do not use any security (SECURTIY=NONE)", PRIV_ROOT);
#undef  NEXT_FLAG
#define NEXT_FLAG exec_aping_nosec

/*
 * -d
 */
KEYWORD_ID (exec_aping_deactivate, exec_aping_flags,
            NEXT_FLAG, OBJ(int,7), TRUE, "-d",
            "Deactivate APING session when done", PRIV_ROOT);
#undef  NEXT_FLAG
#define NEXT_FLAG exec_aping_deactivate
/*
 * -r
 */
KEYWORD_ID (exec_aping_route, exec_aping_flags,
            NEXT_FLAG, OBJ(int,8), TRUE, "-r",
            "show route taken by APING", PRIV_ROOT);
#undef  NEXT_FLAG
#define NEXT_FLAG exec_aping_route

/*
 * -w
 */
KEYWORD_ID (exec_aping_wait, exec_aping_flags,
            NEXT_FLAG, OBJ(int,7), TRUE, "-w",
            "Wait for appn ping to complete before accepting next command", PRIV_ROOT);
#undef  NEXT_FLAG
#define NEXT_FLAG exec_aping_wait

/*
 * Allow flags in any order
 */
NOP (exec_aping_nop, NEXT_FLAG, exec_aping_dest);
HELP    (exec_aping_flags, exec_aping_nop,
         "appn ping [flags] <destination>\n");

KEYWORD	(exec_aping, exec_aping_flags, dump_appn,
	 "ping", "Execute the APING transaction program.", PRIV_ROOT);

/***************************************************************
 * appn stop [ port portname | link linkname ]
 *
 */
EOLS	(exec_appn_stop_eol, startappn_command, CMD_STOPAPPN);

APPN_TYPE_A_NAME (exec_appn_stop_name, exec_appn_stop_eol, no_alt,
       OBJ(string,1), "name of object to stop");
KEYWORD_ID (exec_appn_stop_port, exec_appn_stop_name,
            exec_appn_stop_eol, OBJ(int,1), EXEC_APPN_PORT, "port",
            "Deactivate a port", PRIV_ROOT);
KEYWORD_ID (exec_appn_stop_link, exec_appn_stop_name,
            exec_appn_stop_port, OBJ(int,1), EXEC_LINK, "link",
            "Deactivate a link", PRIV_ROOT);

KEYWORD_ID(exec_appn_stop, exec_appn_stop_link, exec_aping,
	   OBJ(int,1), EXEC_SYSTEM,
	   "stop", "Stop APPN subsystem or an APPN object.", PRIV_ROOT);


/***************************************************************
 * appn start [ port portname | link linkname ]
 *
 */

EOLS	(exec_appn_start_eol, startappn_command, CMD_STARTAPPN);

APPN_TYPE_A_NAME (exec_appn_start_name, exec_appn_start_eol, no_alt,
       OBJ(string,1), "name of object to start");

KEYWORD_ID (exec_appn_start_link, exec_appn_start_name,
            exec_appn_start_eol, OBJ(int,1), EXEC_LINK, "link",
            "Activate a link station", PRIV_ROOT);

KEYWORD_ID (exec_appn_start_port, exec_appn_start_name,
            exec_appn_start_link, OBJ(int,1), EXEC_APPN_PORT, "port",
            "Activate a port", PRIV_ROOT);

KEYWORD_ID(exec_appn_start, exec_appn_start_port, exec_appn_stop,
	   OBJ(int,1), EXEC_SYSTEM,
	   "start", "Start APPN subsystem or an APPN object.", PRIV_ROOT);

KEYWORD	(exec_appn, exec_appn_start, ALTERNATE,
	 "appn", "Send a command to the APPN subsystem.", PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	exec_appn
