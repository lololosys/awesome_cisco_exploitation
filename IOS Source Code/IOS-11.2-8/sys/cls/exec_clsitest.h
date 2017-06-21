/* $Id: exec_clsitest.h,v 3.3.20.1 1996/08/12 16:00:49 widmer Exp $
 * $Source: /release/112/cvs/Xsys/cls/exec_clsitest.h,v $
 *------------------------------------------------------------------
 * Parser Macros for CLSI Test program.
 *
 * 09/94, Kushal Patel
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This file contains parser macros for all user commands for the CLSI
 * Test program.
 *------------------------------------------------------------------
 * $Log: exec_clsitest.h,v $
 * Revision 3.3.20.1  1996/08/12  16:00:49  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:08:25  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  08:56:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:12:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:53:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/04  01:52:22  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:16:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

EOLNS	(exec_ct_eol, clsitest_command);
pdecl(enre_opts);
pdecl(dire_opts);
pdecl(actsap_opts);
pdecl(flowreq_opts);
pdecl(openstnreq_opts);
pdecl(closestnreq_opts);
pdecl(reqopnstn_opts);
pdecl(connreq_opts);
pdecl(sigstnreq_opts);
pdecl(datareq_opts);
pdecl(idreq_opts);
pdecl(deactsapreq_opts);
pdecl(idrsp_opts);
pdecl(disconnrsp_opts);
pdecl(connrsp_opts);



/* debug */

KEYWORD_ID (debug_off, exec_ct_eol, no_alt,
            OBJ(int,2), 0, "off", 
            "Turn Off Debug", PRIV_ROOT);
KEYWORD_ID (debug_on, exec_ct_eol, debug_off,
            OBJ(int,2), 1, "on", 
            "Turn On Debug", PRIV_ROOT);
KEYWORD_ID (ct_debug, debug_on, no_alt,
            OBJ(int,1), CLSIDEBUG, "debug", 
            "Debug Request", PRIV_ROOT);


/***************************************************************
 * clsitest connrsp [action <code>] [flags <string>] [correlator <num>]
 *                    [da <addr>] [sa <addr>] [dsap <num>] [ssap <num>]
 *                    [T1<msec>] [T2<msec>] [Ti<msec>] 
 *                    [maxout<count>] [maxretry<count>] [maxin<count>] 
 *                    [maxincr<count>] [maxifield<size>] [xidretry<num>]
 *                    [xidtimer<msec>] [dynwindow <size>] [exoption <type>]
 *                    [exretry <num>] [extimer <msec>] [riflength <size>]
 *                    [route<string>]
 *
 * OBJ(int,1) = CLS_CONNECT|CLS_RESPONSE
 *
 */

/* [route <string>] */
STRING (connrsp_rif_string, connrsp_opts, no_alt, OBJ(string,2),
        "RIF (default = 0)");
KEYWORD (connrsp_rif, connrsp_rif_string, no_alt,
         "route","RIF",PRIV_ROOT);

/* [riflength <size>] */
NUMBER (connrsp_riflength_num, connrsp_opts, no_alt, OBJ(int,20),
        0x00,0xff,"Routing Information Length(default = 0)");
KEYWORD (connrsp_riflength, connrsp_riflength_num, connrsp_rif,
         "riflength","Routing Information Length",PRIV_ROOT);

/* [extimer <msec>] */
NUMBER (connrsp_extimer_num, connrsp_opts, no_alt, OBJ(int,19),
        0x0,0xffffffff,"Retry time in msec (default = 60*1000)");
KEYWORD (connrsp_extimer, connrsp_extimer_num, connrsp_riflength,
         "extimer","Retry time in msec",PRIV_ROOT);

/* [exretry <type>] */
NUMBER (connrsp_exretry_num, connrsp_opts, no_alt, OBJ(int,18),
        0x0,0xffffffff,"Number of times to try (default = 10)");
KEYWORD (connrsp_exretry, connrsp_exretry_num, connrsp_extimer,
         "exretry","Number of times to try",PRIV_ROOT);

/* [exoption <type>] */
NUMBER (connrsp_exoption_num, connrsp_opts, no_alt, OBJ(int,17),
        0x0,0xffffffff,"Explorer option (default = EO_OUTBOUND_DEFAULT)");
KEYWORD (connrsp_exoption, connrsp_exoption_num, connrsp_exretry,
         "exoption","Explorer option",PRIV_ROOT);


/* [dynwindow <size>] */
NUMBER (connrsp_dynwindow_num, connrsp_opts, no_alt, OBJ(int,16),
        0x0,0xffffffff,"Dynamic window option (default = 0)");
KEYWORD (connrsp_dynwindow, connrsp_dynwindow_num, connrsp_exoption,
         "dynwindow","Dynamic window option",PRIV_ROOT);

/* [xidtimer <msec>] */
NUMBER (connrsp_xidtimer_num, connrsp_opts, no_alt, OBJ(int,15),
        0x0,0xffffffff,"Retry time in msec (default = 60*1000)");
KEYWORD (connrsp_xidtimer, connrsp_xidtimer_num, connrsp_dynwindow, 
         "xidtimer","Retry time in msec",PRIV_ROOT);
  
/* [xidretry <num>] */
NUMBER (connrsp_xidretry_num, connrsp_opts, no_alt, OBJ(int,14),
        0x0,0xffffffff,"Number of times to try (default = 10)");
KEYWORD (connrsp_xidretry, connrsp_xidretry_num, connrsp_xidtimer,
         "xidretry","Number of times to try",PRIV_ROOT);


/* [maxifield <count>] */
NUMBER (connrsp_maxifield_num, connrsp_opts, no_alt, OBJ(int,13),
        0x0,0xffffffff, "Maximum I-field length (default = 521)");
KEYWORD (connrsp_maxifield, connrsp_maxifield_num, connrsp_xidretry, 
         "maxifield","Maximum I-field length",PRIV_ROOT);

/* [maxincr <count>] */
NUMBER (connrsp_maxincr_num, connrsp_opts, no_alt, OBJ(int,12),
        0x0,0xffffffff,"Maxout increment (default = 0)");
KEYWORD (connrsp_maxincr, connrsp_maxincr_num, connrsp_maxifield, 
         "maxincr","Maxout increment",PRIV_ROOT);

/* [maxin <count>] */
NUMBER (connrsp_maxin_num, connrsp_opts, no_alt, OBJ(int,11),
        0x0,0xffffffff,"Maxin (default = 7)");
KEYWORD (connrsp_maxin, connrsp_maxin_num, connrsp_maxincr, "maxin",
           "Maxin",PRIV_ROOT);

/* [maxretry <count>] */
NUMBER (connrsp_maxretry_num, connrsp_opts, no_alt, OBJ(int,10),
        0x0,0xffffffff,"Max retry count (default = 0)");
KEYWORD (connrsp_maxretry, connrsp_maxretry_num, connrsp_maxin, 
         "maxretry","Max retry count",PRIV_ROOT);

/* [maxout <count>] */
NUMBER (connrsp_maxout_num, connrsp_opts, no_alt, OBJ(int,9),
        0x0,0xffffffff, "Maxout (default = 7)");
KEYWORD (connrsp_maxout, connrsp_maxout_num, connrsp_maxretry, "maxout",
           "Maxout",PRIV_ROOT);

/* [Ti <msec>] */
NUMBER (connrsp_Ti_num, connrsp_opts, no_alt, OBJ(int,8),0x0,0xffffffff,
        "Ti - Inactivity timer (default = 10000 msec)");
KEYWORD (connrsp_Ti, connrsp_Ti_num, connrsp_maxout, "Ti",
           "Ti - Inactivity Timer",PRIV_ROOT);

/* [T2 <msec>] */
NUMBER (connrsp_T2_num, connrsp_opts, no_alt, OBJ(int,7),0x0,0xffffffff,
        "T2 timer (default = 100 msec)");
KEYWORD (connrsp_T2, connrsp_T2_num, connrsp_Ti, "T2",
           "T2 - Acknowledgement Timer",PRIV_ROOT);

/* [T1 <msec>] */
NUMBER (connrsp_T1_num, connrsp_opts, no_alt, OBJ(int,6),0x0,0xffffffff,
        "T1 timer (default = 100 msec)");
KEYWORD (connrsp_T1, connrsp_T1_num, connrsp_T2, "T1",
           "T1 - Response Timer",PRIV_ROOT);


/* [ssap <num>] */
NUMBER (connrsp_ssap_num, connrsp_opts, no_alt, OBJ(int,5),0x00,0xff,
        "Remote SAP address (default = STATION_ILLEGAL)");
KEYWORD (connrsp_ssap, connrsp_ssap_num, connrsp_T1, "ssap",
           "Remote SAP address",PRIV_ROOT);

/* [dsap <num>] */
NUMBER (connrsp_dsap_num, connrsp_opts, no_alt, OBJ(int,4),0x00,0xff,
        "Remote SAP address (default = STATION_ILLEGAL)");
KEYWORD (connrsp_dsap, connrsp_dsap_num, connrsp_ssap, "dsap",
           "Remote SAP address",PRIV_ROOT);

/* [sa <addr>] */
ENADDR (connrsp_sa_num, connrsp_opts, no_alt, OBJ(hwaddr,2),
        "Source address (default = 0)");
KEYWORD (connrsp_sa, connrsp_sa_num, connrsp_dsap, "sa",
           "Source address",PRIV_ROOT);


/* [da <addr>] */
ENADDR (connrsp_da_num, connrsp_opts, no_alt, OBJ(hwaddr,1),
        "Destination address (default = 0000.3040.A6F0)");
KEYWORD (connrsp_da, connrsp_da_num, connrsp_sa, "da",
           "Destination address",PRIV_ROOT);


/* [correlator <num>] */
NUMBER (connrsp_correlator_num, connrsp_opts, no_alt, OBJ(int,3),
        0x0,0xffffffff, "Correlator Value (default = 0xdead)");
KEYWORD (connrsp_correlator, connrsp_correlator_num, connrsp_da, 
         "correlator","Correlator",PRIV_ROOT);

/* [flags <string>] */
STRING (connrsp_flags_string, connrsp_opts, no_alt, OBJ(string,1),
        "Modify Flags - 3 bytes (default = 000000)");
KEYWORD (connrsp_flags, connrsp_flags_string, connrsp_correlator, 
         "flags","Modify Flags",PRIV_ROOT);

/* [action <code>] */
NUMBER (connrsp_action_num, connrsp_opts, no_alt, OBJ(int,2),
        0x00,0x01,"Action Code (default = 0x00, Connect)");
KEYWORD (connrsp_action, connrsp_action_num, connrsp_flags, 
         "action","Action Code",PRIV_ROOT);


NOP (connrsp_opts, connrsp_action, exec_ct_eol); 

SET (connrsp_def_rif,connrsp_opts,OBJ(string,2),"");
SET (connrsp_def_riflength, connrsp_def_rif, OBJ(int,20), 0);
SET (connrsp_def_extimer,connrsp_def_riflength,OBJ(int,19),60*1000);
SET (connrsp_def_exretry, connrsp_def_extimer, OBJ(int,18), 10);
SET (connrsp_def_exoption,connrsp_def_exretry, OBJ(int,17),EO_OUTBOUND_DEFAULT);
SET (connrsp_def_dynwindow, connrsp_def_exoption, OBJ(int,16), 0);
SET (connrsp_def_xidtimer, connrsp_def_dynwindow, OBJ(int,15), 60*1000);
SET (connrsp_def_xidretry, connrsp_def_xidtimer, OBJ(int,14), 10);
SET (connrsp_def_maxifield, connrsp_def_xidretry, OBJ(int,13), 521);
SET (connrsp_def_maxincr,connrsp_def_maxifield,OBJ(int,12),0);
SET (connrsp_def_maxin, connrsp_def_maxincr, OBJ(int,11), 7);
SET (connrsp_def_maxretry,connrsp_def_maxin, OBJ(int,10),0);
SET (connrsp_def_maxout, connrsp_def_maxretry, OBJ(int,9), 7);
SET (connrsp_def_Ti, connrsp_def_maxout, OBJ(int,8), 10000);
SET (connrsp_def_T2, connrsp_def_Ti, OBJ(int,7), 100);
SET (connrsp_def_T1, connrsp_def_T2, OBJ(int,6), 100);
SET (connrsp_def_ssap, connrsp_def_T1, OBJ(int,5), 0);
SET (connrsp_def_dsap, connrsp_def_ssap, OBJ(int,4), 0x04);
SET (connrsp_def_sa, connrsp_def_dsap, OBJ(hwaddr,2), STATION_ILLEGAL);
SET (connrsp_def_da, connrsp_def_sa, OBJ(hwaddr,1),0);
SET (connrsp_def_correlator,connrsp_def_da, OBJ(int,3),0xdead);
SET (connrsp_def_flags, connrsp_def_correlator, OBJ(string,1),"000000");
SET (connrsp_def_action, connrsp_def_flags, OBJ(int,2),CLS_ACTION_CONNECT);

/* connrsp */
KEYWORD_ID (ct_connrsp, connrsp_def_action, ct_debug,
            OBJ(int,1), CLS_CONNECT|CLS_RESPONSE, "connrsp", 
            "Connect Response",
            PRIV_ROOT);



/***************************************************************
 * clsitest disconnrsp [action<code>] [correlator <value>
 *
 * OBJ(int,1) = CLS_DISCONNECT|CLS_RESPONSE
 * OBJ(int,2) = <code>
 * OBJ(int,3) = <value>
 *
 */

/* [correlator <value>] */
NUMBER (disconnrsp_correlator_num, disconnrsp_opts, no_alt, OBJ(int,3),
        0x0,0xffffffff,"Correlator Value (default = 0xdead)");
KEYWORD (disconnrsp_correlator, disconnrsp_correlator_num, no_alt,
         "correlator","Correlator",PRIV_ROOT);

/* [action <code>] */
NUMBER (disconnrsp_action_num, disconnrsp_opts, no_alt, OBJ(int,2),
        0x00,0x01,"Action Code (default = 0x00, Accept)");
KEYWORD (disconnrsp_action, disconnrsp_action_num, disconnrsp_correlator,
         "action","Action Code",PRIV_ROOT);

NOP (disconnrsp_opts, disconnrsp_action, exec_ct_eol);
SET (disconnrsp_def_correlator,disconnrsp_opts, OBJ(int,3),0xdead);
SET (disconnrsp_def_action,disconnrsp_def_correlator,
      OBJ(int,2), CLS_ACTION_ACCEPT);

/* disconnrsp */
KEYWORD_ID (ct_disconnrsp, disconnrsp_def_action, ct_connrsp,
            OBJ(int,1), CLS_DISCONNECT|CLS_RESPONSE, "disconnrsp", 
            "Disconnect Response",PRIV_ROOT);


/***************************************************************
 * clsitest idrsp [correlator <value>] [xid <data>]
 *
 * OBJ(int,1) = CLS_ID|CLS_RESPONSE
 * OBJ(int,2) = <value>
 * OBJ(string) = <data>
 *
 */

/* [xid <data>] */
STRING (idrsp_xid_string, idrsp_opts, no_alt, OBJ(string,1),
        "XID data (default = BEEFBABE)");
KEYWORD (idrsp_xid, idrsp_xid_string, no_alt,
         "xid","XID data",PRIV_ROOT);

/* [correlator <value>] */
NUMBER (idrsp_correlator_num, idrsp_opts, no_alt, OBJ(int,2),
        0x0,0xffffffff,"Correlator Value (default = 0xdead)");
KEYWORD (idrsp_correlator, idrsp_correlator_num, idrsp_xid,
         "correlator","Correlator",PRIV_ROOT);

NOP (idrsp_opts, idrsp_correlator, exec_ct_eol);
SET (idrsp_def_xid,idrsp_opts, OBJ(string,1),"BEEFBABE");
SET (idrsp_def_correlator,idrsp_def_xid,OBJ(int,2),0xdead);

/* idrsp */
KEYWORD_ID (ct_idrsp, idrsp_def_correlator, ct_disconnrsp,
            OBJ(int,1), CLS_ID|CLS_RESPONSE, "idrsp", 
            "Id Response",PRIV_ROOT);


/***************************************************************
 * clsitest reqopnstnreq [ucepid <id>] [pcepid <id>] [correlator <num>]
 *                    [da <addr>] [sa <addr>] [dsap <num>] [ssap <num>]
 *                    [T1<msec>] [T2<msec>] [Ti<msec>] 
 *                    [maxout<count>] [maxretry<count>] [maxin<count>] 
 *                    [maxincr<count>] [maxifield<size>] [xidretry<num>]
 *                    [xidtimer<msec>] [dynwindow <size>] [exoption <type>]
 *                    [exretry <num>] [extimer <msec>] [riflength <size>]
 *                    [route <string>]
 *
 * OBJ(int,1) = CLS_REQ_OPNSTN|CLS_REQUEST
 * OBJ(int,2) = <num>
 * OBJ(int,3) = <type>
 *
 */

/* [route <string>] */
STRING (reqopnstn_rif_string, reqopnstn_opts, no_alt, OBJ(string,1),
        "RIF (default = 0)");
KEYWORD (reqopnstn_rif, reqopnstn_rif_string, no_alt,
         "route","RIF",PRIV_ROOT);

/* [riflength <size>] */
NUMBER (reqopnstn_riflength_num, reqopnstn_opts, no_alt, OBJ(int,21),
        0x00,0xff,"Routing Information Length(default = 0)");
KEYWORD (reqopnstn_riflength, reqopnstn_riflength_num, reqopnstn_rif,
         "riflength","Routing Information Length",PRIV_ROOT);

/* [extimer <msec>] */
NUMBER (reqopnstn_extimer_num, reqopnstn_opts, no_alt, OBJ(int,20),
        0x0,0xffffffff,"Retry time in msec (default = 60*1000)");
KEYWORD (reqopnstn_extimer, reqopnstn_extimer_num, reqopnstn_riflength,
         "extimer","Retry time in msec",PRIV_ROOT);

/* [exretry <type>] */
NUMBER (reqopnstn_exretry_num, reqopnstn_opts, no_alt, OBJ(int,19),
        0x0,0xffffffff,"Number of times to try (default = 10)");
KEYWORD (reqopnstn_exretry, reqopnstn_exretry_num, reqopnstn_extimer,
         "exretry","Number of times to try",PRIV_ROOT);

/* [exoption <type>] */
NUMBER (reqopnstn_exoption_num, reqopnstn_opts, no_alt, OBJ(int,18),
        0x0,0xffffffff,"Explorer option (default = EO_OUTBOUND_DEFAULT)");
KEYWORD (reqopnstn_exoption, reqopnstn_exoption_num, reqopnstn_exretry,
         "exoption","Explorer option",PRIV_ROOT);


/* [dynwindow <size>] */
NUMBER (reqopnstn_dynwindow_num, reqopnstn_opts, no_alt, OBJ(int,17),
        0x0,0xffffffff,"Dynamic window option (default = 0)");
KEYWORD (reqopnstn_dynwindow, reqopnstn_dynwindow_num, reqopnstn_exoption,
         "dynwindow","Dynamic window option",PRIV_ROOT);

/* [xidtimer <msec>] */
NUMBER (reqopnstn_xidtimer_num, reqopnstn_opts, no_alt, OBJ(int,16),
        0x0,0xffffffff,"Retry time in msec (default = 60*1000)");
KEYWORD (reqopnstn_xidtimer, reqopnstn_xidtimer_num, reqopnstn_dynwindow, 
         "xidtimer","Retry time in msec",PRIV_ROOT);
  
/* [xidretry <num>] */
NUMBER (reqopnstn_xidretry_num, reqopnstn_opts, no_alt, OBJ(int,15),
        0x0,0xffffffff,"Number of times to try (default = 10)");
KEYWORD (reqopnstn_xidretry, reqopnstn_xidretry_num, reqopnstn_xidtimer,
         "xidretry","Number of times to try",PRIV_ROOT);


/* [maxifield <count>] */
NUMBER (reqopnstn_maxifield_num, reqopnstn_opts, no_alt, OBJ(int,14),
        0x0,0xffffffff, "Maximum I-field length (default = 521)");
KEYWORD (reqopnstn_maxifield, reqopnstn_maxifield_num, reqopnstn_xidretry, 
         "maxifield","Maximum I-field length",PRIV_ROOT);

/* [maxincr <count>] */
NUMBER (reqopnstn_maxincr_num, reqopnstn_opts, no_alt, OBJ(int,13),
        0x0,0xffffffff,"Maxout increment (default = 0)");
KEYWORD (reqopnstn_maxincr, reqopnstn_maxincr_num, reqopnstn_maxifield, 
         "maxincr","Maxout increment",PRIV_ROOT);

/* [maxin <count>] */
NUMBER (reqopnstn_maxin_num, reqopnstn_opts, no_alt, OBJ(int,12),
        0x0,0xffffffff,"Maxin (default = 7)");
KEYWORD (reqopnstn_maxin, reqopnstn_maxin_num, reqopnstn_maxincr, "maxin",
           "Maxin",PRIV_ROOT);

/* [maxretry <count>] */
NUMBER (reqopnstn_maxretry_num, reqopnstn_opts, no_alt, OBJ(int,11),
        0x0,0xffffffff,"Max retry count (default = 0)");
KEYWORD (reqopnstn_maxretry, reqopnstn_maxretry_num, reqopnstn_maxin, 
         "maxretry","Max retry count",PRIV_ROOT);

/* [maxout <count>] */
NUMBER (reqopnstn_maxout_num, reqopnstn_opts, no_alt, OBJ(int,10),
        0x0,0xffffffff, "Maxout (default = 7)");
KEYWORD (reqopnstn_maxout, reqopnstn_maxout_num, reqopnstn_maxretry, "maxout",
           "Maxout",PRIV_ROOT);

/* [Ti <msec>] */
NUMBER (reqopnstn_Ti_num, reqopnstn_opts, no_alt, OBJ(int,9),0x0,0xffffffff,
        "Ti - Inactivity timer (default = 10000 msec)");
KEYWORD (reqopnstn_Ti, reqopnstn_Ti_num, reqopnstn_maxout, "Ti",
           "Ti - Inactivity Timer",PRIV_ROOT);

/* [T2 <msec>] */
NUMBER (reqopnstn_T2_num, reqopnstn_opts, no_alt, OBJ(int,8),0x0,0xffffffff,
        "T2 timer (default = 100 msec)");
KEYWORD (reqopnstn_T2, reqopnstn_T2_num, reqopnstn_Ti, "T2",
           "T2 - Acknowledgement Timer",PRIV_ROOT);

/* [T1 <msec>] */
NUMBER (reqopnstn_T1_num, reqopnstn_opts, no_alt, OBJ(int,7),0x0,0xffffffff,
        "T1 timer (default = 100 msec)");
KEYWORD (reqopnstn_T1, reqopnstn_T1_num, reqopnstn_T2, "T1",
           "T1 - Response Timer",PRIV_ROOT);


/* [ssap <num>] */
NUMBER (reqopnstn_ssap_num, reqopnstn_opts, no_alt, OBJ(int,6),0x00,0xff,
        "Remote SAP address (default = STATION_ILLEGAL)");
KEYWORD (reqopnstn_ssap, reqopnstn_ssap_num, reqopnstn_T1, "ssap",
           "Remote SAP address",PRIV_ROOT);

/* [dsap <num>] */
NUMBER (reqopnstn_dsap_num, reqopnstn_opts, no_alt, OBJ(int,5),0x00,0xff,
        "Remote SAP address (default = STATION_ILLEGAL)");
KEYWORD (reqopnstn_dsap, reqopnstn_dsap_num, reqopnstn_ssap, "dsap",
           "Remote SAP address",PRIV_ROOT);

/* [sa <addr>] */
ENADDR (reqopnstn_sa_num, reqopnstn_opts, no_alt, OBJ(hwaddr,2),
        "Source address (default = 0)");
KEYWORD (reqopnstn_sa, reqopnstn_sa_num, reqopnstn_dsap, "sa",
           "Source address",PRIV_ROOT);


/* [da <addr>] */
ENADDR (reqopnstn_da_num, reqopnstn_opts, no_alt, OBJ(hwaddr,1),
        "Destination address (default = 0)");
KEYWORD (reqopnstn_da, reqopnstn_da_num, reqopnstn_sa, "da",
           "Destination address",PRIV_ROOT);


/* [correlator <num>] */
NUMBER (reqopnstn_correlator_num, reqopnstn_opts, no_alt, OBJ(int,4),
        0x0,0xffffffff, "Correlator Value (default = 0xdead)");
KEYWORD (reqopnstn_correlator, reqopnstn_correlator_num, reqopnstn_da, 
         "correlator","Correlator",PRIV_ROOT);

/* [pcepid <id>] */
NUMBER (reqopnstn_pcepid_num, reqopnstn_opts, no_alt, OBJ(int,3),0,255,
        "Pcepid Number (default = 0)");
KEYWORD (reqopnstn_pcepid, reqopnstn_pcepid_num, reqopnstn_correlator, 
         "pcepid","Provider CEP id",PRIV_ROOT);

/* [ucepid <id>] */
NUMBER (reqopnstn_ucepid_num, reqopnstn_opts, no_alt, OBJ(int,2),0,255,
        "Ucepid Number (default = 0)");
KEYWORD (reqopnstn_ucepid, reqopnstn_ucepid_num, reqopnstn_pcepid, "ucepid",
           "User CEP id",PRIV_ROOT);

NOP (reqopnstn_opts, reqopnstn_ucepid, exec_ct_eol); 

SET (reqopnstn_def_rif,reqopnstn_opts,OBJ(string,1),"hello");
SET (reqopnstn_def_riflength, reqopnstn_def_rif, OBJ(int,21), 0);
SET (reqopnstn_def_extimer,reqopnstn_def_riflength,OBJ(int,20),60*1000);
SET (reqopnstn_def_exretry, reqopnstn_def_extimer, OBJ(int,19), 10);
SET (reqopnstn_def_exoption,reqopnstn_def_exretry, OBJ(int,18),EO_OUTBOUND_DEFAULT);
SET (reqopnstn_def_dynwindow, reqopnstn_def_exoption, OBJ(int,17), 0);
SET (reqopnstn_def_xidtimer, reqopnstn_def_dynwindow, OBJ(int,16), 60*1000);
SET (reqopnstn_def_xidretry, reqopnstn_def_xidtimer, OBJ(int,15), 10);
SET (reqopnstn_def_maxifield, reqopnstn_def_xidretry, OBJ(int,14), 521);
SET (reqopnstn_def_maxincr,reqopnstn_def_maxifield,OBJ(int,13),0);
SET (reqopnstn_def_maxin, reqopnstn_def_maxincr, OBJ(int,12), 7);
SET (reqopnstn_def_maxretry,reqopnstn_def_maxin, OBJ(int,11),0);
SET (reqopnstn_def_maxout, reqopnstn_def_maxretry, OBJ(int,10), 7);
SET (reqopnstn_def_Ti, reqopnstn_def_maxout, OBJ(int,9), 10000);
SET (reqopnstn_def_T2, reqopnstn_def_Ti, OBJ(int,8), 100);
SET (reqopnstn_def_T1, reqopnstn_def_T2, OBJ(int,7), 100);
SET (reqopnstn_def_ssap, reqopnstn_def_T1, OBJ(int,6), 0);
SET (reqopnstn_def_dsap, reqopnstn_def_ssap, OBJ(int,5), 0x04);
SET (reqopnstn_def_sa, reqopnstn_def_dsap, OBJ(hwaddr,2), STATION_ILLEGAL);
SET (reqopnstn_def_da, reqopnstn_def_sa, OBJ(hwaddr,1), "0000.3040.A6F0");
SET (reqopnstn_def_correlator, reqopnstn_def_da, OBJ(int,4), 0xdead);
SET (reqopnstn_def_pcepid, reqopnstn_def_correlator, OBJ(int,3), CLS_NO_CEP);
SET (reqopnstn_def_ucepid, reqopnstn_def_pcepid, OBJ(int,2), 0x01);

/* reqopnstnreq */
KEYWORD_ID (ct_reqopnstnreq, reqopnstn_def_ucepid, ct_idrsp,
            OBJ(int,1), CLS_REQ_OPNSTN|CLS_REQUEST, "reqopnstnreq", "Request Open Station Request", PRIV_ROOT);



/***************************************************************
 * clsitest idreq [correlator <value>] [rlength <size>] [xid <data>]
 *
 * OBJ(int,1) = CLS_ID|CLS_REQUEST
 * OBJ(int,2) = <value>
 * OBJ(int,3) = <size>
 * OBJ(string,1) = <data>
 *
 */

/* [xid <data>] */
STRING (idreq_xid_string, idreq_opts, no_alt, OBJ(string,1),
        "Xid Data (Default = BEEFBABE)");
KEYWORD (idreq_xid, idreq_xid_string, no_alt, "xid",
           "XID data",PRIV_ROOT);

/* [rlength <size>] */
NUMBER (idreq_rlength_num, idreq_opts, no_alt, OBJ(int,3),
        0x0,0xffffffff,"Reserve Length (default = 0x02)");
KEYWORD (idreq_rlength, idreq_rlength_num, idreq_xid, "rlength",
           "Reserve length",PRIV_ROOT);

/* [correlator <value>] */
NUMBER (idreq_correlator_num, idreq_opts, no_alt, OBJ(int,2),
        0x0,0xffffffff,"Correlator Value (default = 0xdead)");
KEYWORD (idreq_correlator, idreq_correlator_num, idreq_rlength, "correlator",
           "Correlator",PRIV_ROOT);

NOP (idreq_opts, idreq_correlator, exec_ct_eol);
SET (idreq_def_xid, idreq_opts, OBJ(string,1), "BEEFBABE");
SET (idreq_def_rlength,idreq_def_xid, OBJ(int,3), 0x02);
SET (idreq_def_correlator,idreq_def_rlength, OBJ(int,2),0xdead);

/* idreq */
KEYWORD_ID (ct_idreq, idreq_def_correlator, ct_reqopnstnreq,
            OBJ(int,1), CLS_ID|CLS_REQUEST, "idreq", 
            "Id Request", PRIV_ROOT);


/***************************************************************
 * clsitest datareq [correlator <value>] [rlength <size>] [length<size>]
 *
 * OBJ(int,1) = CLS_DATA|CLS_REQUEST
 * OBJ(int,2) = <value>
 * OBJ(int,3) = <size>
 * OBJ(int,4) = <size>
 *
 */

/* [length <size>] */
NUMBER (datareq_length_num, datareq_opts, no_alt, OBJ(int,4),
        0x0,0xffffffff,"Data length (default = 256)");
KEYWORD (datareq_length, datareq_length_num, no_alt, "length",
           "Data length",PRIV_ROOT);

/* [rlength <size>] */
NUMBER (datareq_rlength_num, datareq_opts, no_alt, OBJ(int,3),
        0x0,0xffffffff,"Reserve Length (default = 0x02)");
KEYWORD (datareq_rlength, datareq_rlength_num, datareq_length, "rlength",
           "Reserve length",PRIV_ROOT);

/* [correlator <value>] */
NUMBER (datareq_correlator_num, datareq_opts, no_alt, OBJ(int,2),
        0x0,0xffffffff,"Correlator Value (default = 0xdead)");
KEYWORD (datareq_correlator, datareq_correlator_num, datareq_rlength, 
         "correlator","Correlator",PRIV_ROOT);

NOP (datareq_opts, datareq_correlator, exec_ct_eol);
SET (datareq_def_length,datareq_opts, OBJ(int,4), 256);
SET (datareq_def_rlength,datareq_def_length, OBJ(int,3), 0x02);
SET (datareq_def_correlator,datareq_def_rlength, OBJ(int,2),0xdead);

/* datareq */
KEYWORD_ID (ct_datareq, datareq_def_correlator, ct_idreq,
            OBJ(int,1), CLS_DATA|CLS_REQUEST, "datareq", 
            "Data Request", PRIV_ROOT);


/***************************************************************
 * clsitest sigstnreq [action <code>] [correlator <value>]
 *
 * OBJ(int,1) = CLS_SIGNAL_STN|CLS_REQUEST
 * OBJ(int,2) = <code>
 * OBJ(int,3) = <value>
 *
 */

/* [correlator <value>] */
NUMBER (sigstnreq_correlator_num, sigstnreq_opts, no_alt, OBJ(int,3),
        0x0,0xffffffff,"Correlator Value (default = 0xdead)");
KEYWORD (sigstnreq_correlator, sigstnreq_correlator_num, no_alt, "correlator",
           "Correlator",PRIV_ROOT);

/* [action <code>] */
ZEROXHEXNUM (sigstnreq_action_num, sigstnreq_opts, no_alt, OBJ(int,2),
        "Action code (default = 0x00, Start)");
KEYWORD (sigstnreq_action, sigstnreq_action_num, sigstnreq_correlator, 
                    "action", "Action code",PRIV_ROOT);

NOP (sigstnreq_opts, sigstnreq_action, exec_ct_eol);
SET (sigstnreq_def_correlator,sigstnreq_opts, OBJ(int,3),0xdead);
SET (sigstnreq_def_action, sigstnreq_def_correlator,
     OBJ(int,2),CLS_ACTION_START);

/* sigstnreq */
KEYWORD_ID (ct_sigstnreq, sigstnreq_def_action, ct_datareq,
            OBJ(int,1), CLS_SIGNAL_STN|CLS_REQUEST, "sigstnreq", 
            "Signal Station Request", PRIV_ROOT);


/***************************************************************
 * clsitest disconnreq [correlator <value>]
 *
 * OBJ(int,1) = CLS_DISCONNECT|CLS_REQUEST
 * OBJ(int,2) = <value>
 *
 */

/* [correlator <value>] */
NUMBER (disconnreq_correlator_num, exec_ct_eol, no_alt, OBJ(int,2),
        0x0,0xffffffff,"Correlator Value (default = 0xdead)");
KEYWORD (disconnreq_correlator, disconnreq_correlator_num, exec_ct_eol, 
         "correlator","Correlator",PRIV_ROOT);

SET (disconnreq_def_correlator,disconnreq_correlator, OBJ(int,2),0xdead);

/* disconnreq */
KEYWORD_ID (ct_disconnreq, disconnreq_def_correlator, ct_sigstnreq,
            OBJ(int,1), CLS_DISCONNECT|CLS_REQUEST, "disconnreq", 
            "Disconnect Request",PRIV_ROOT);



/***************************************************************
 * clsitest connreq [flags <string>] [correlator <num>]
 *                    [da <addr>] [sa <addr>] [dsap <num>] [ssap <num>]
 *                    [T1<msec>] [T2<msec>] [Ti<msec>] 
 *                    [maxout<count>] [maxretry<count>] [maxin<count>] 
 *                    [maxincr<count>] [maxifield<size>] [xidretry<num>]
 *                    [xidtimer<msec>] [dynwindow <size>] [exoption <type>]
 *                    [exretry <num>] [extimer <msec>] [riflength <size>]
 *                    [route<string>]
 *
 * OBJ(int,1) = CLS_CONNECT|CLS_REQUEST
 *
 */

/* [route <string>] */
STRING (connreq_rif_string, connreq_opts, no_alt, OBJ(string,2),
        "RIF (default = 0)");
KEYWORD (connreq_rif, connreq_rif_string, no_alt,
         "route","RIF",PRIV_ROOT);

/* [riflength <size>] */
NUMBER (connreq_riflength_num, connreq_opts, no_alt, OBJ(int,19),
        0x00,0xff,"Routing Information Length(default = 0)");
KEYWORD (connreq_riflength, connreq_riflength_num, connreq_rif,
         "riflength","Routing Information Length",PRIV_ROOT);

/* [extimer <msec>] */
NUMBER (connreq_extimer_num, connreq_opts, no_alt, OBJ(int,18),
        0x0,0xffffffff,"Retry time in msec (default = 60*1000)");
KEYWORD (connreq_extimer, connreq_extimer_num, connreq_riflength,
         "extimer","Retry time in msec",PRIV_ROOT);

/* [exretry <type>] */
NUMBER (connreq_exretry_num, connreq_opts, no_alt, OBJ(int,17),
        0x0,0xffffffff,"Number of times to try (default = 10)");
KEYWORD (connreq_exretry, connreq_exretry_num, connreq_extimer,
         "exretry","Number of times to try",PRIV_ROOT);

/* [exoption <type>] */
NUMBER (connreq_exoption_num, connreq_opts, no_alt, OBJ(int,16),
        0x0,0xffffffff,"Explorer option (default = EO_OUTBOUND_DEFAULT)");
KEYWORD (connreq_exoption, connreq_exoption_num, connreq_exretry,
         "exoption","Explorer option",PRIV_ROOT);


/* [dynwindow <size>] */
NUMBER (connreq_dynwindow_num, connreq_opts, no_alt, OBJ(int,15),
        0x0,0xffffffff,"Dynamic window option (default = 0)");
KEYWORD (connreq_dynwindow, connreq_dynwindow_num, connreq_exoption,
         "dynwindow","Dynamic window option",PRIV_ROOT);

/* [xidtimer <msec>] */
NUMBER (connreq_xidtimer_num, connreq_opts, no_alt, OBJ(int,14),
        0x0,0xffffffff,"Retry time in msec (default = 60*1000)");
KEYWORD (connreq_xidtimer, connreq_xidtimer_num, connreq_dynwindow, 
         "xidtimer","Retry time in msec",PRIV_ROOT);
  
/* [xidretry <num>] */
NUMBER (connreq_xidretry_num, connreq_opts, no_alt, OBJ(int,13),
        0x0,0xffffffff,"Number of times to try (default = 10)");
KEYWORD (connreq_xidretry, connreq_xidretry_num, connreq_xidtimer,
         "xidretry","Number of times to try",PRIV_ROOT);


/* [maxifield <count>] */
NUMBER (connreq_maxifield_num, connreq_opts, no_alt, OBJ(int,12),
        0x0,0xffffffff, "Maximum I-field length (default = 521)");
KEYWORD (connreq_maxifield, connreq_maxifield_num, connreq_xidretry, 
         "maxifield","Maximum I-field length",PRIV_ROOT);

/* [maxincr <count>] */
NUMBER (connreq_maxincr_num, connreq_opts, no_alt, OBJ(int,11),
        0x0,0xffffffff,"Maxout increment (default = 0)");
KEYWORD (connreq_maxincr, connreq_maxincr_num, connreq_maxifield, 
         "maxincr","Maxout increment",PRIV_ROOT);

/* [maxin <count>] */
NUMBER (connreq_maxin_num, connreq_opts, no_alt, OBJ(int,10),
        0x0,0xffffffff,"Maxin (default = 7)");
KEYWORD (connreq_maxin, connreq_maxin_num, connreq_maxincr, "maxin",
           "Maxin",PRIV_ROOT);

/* [maxretry <count>] */
NUMBER (connreq_maxretry_num, connreq_opts, no_alt, OBJ(int,9),
        0x0,0xffffffff,"Max retry count (default = 0)");
KEYWORD (connreq_maxretry, connreq_maxretry_num, connreq_maxin, 
         "maxretry","Max retry count",PRIV_ROOT);

/* [maxout <count>] */
NUMBER (connreq_maxout_num, connreq_opts, no_alt, OBJ(int,8),
        0x0,0xffffffff, "Maxout (default = 7)");
KEYWORD (connreq_maxout, connreq_maxout_num, connreq_maxretry, "maxout",
           "Maxout",PRIV_ROOT);

/* [Ti <msec>] */
NUMBER (connreq_Ti_num, connreq_opts, no_alt, OBJ(int,7),0x0,0xffffffff,
        "Ti - Inactivity timer (default = 10000 msec)");
KEYWORD (connreq_Ti, connreq_Ti_num, connreq_maxout, "Ti",
           "Ti - Inactivity Timer",PRIV_ROOT);

/* [T2 <msec>] */
NUMBER (connreq_T2_num, connreq_opts, no_alt, OBJ(int,6),0x0,0xffffffff,
        "T2 timer (default = 100 msec)");
KEYWORD (connreq_T2, connreq_T2_num, connreq_Ti, "T2",
           "T2 - Acknowledgement Timer",PRIV_ROOT);

/* [T1 <msec>] */
NUMBER (connreq_T1_num, connreq_opts, no_alt, OBJ(int,5),0x0,0xffffffff,
        "T1 timer (default = 100 msec)");
KEYWORD (connreq_T1, connreq_T1_num, connreq_T2, "T1",
           "T1 - Response Timer",PRIV_ROOT);


/* [ssap <num>] */
NUMBER (connreq_ssap_num, connreq_opts, no_alt, OBJ(int,4),0x00,0xff,
        "Remote SAP address (default = STATION_ILLEGAL)");
KEYWORD (connreq_ssap, connreq_ssap_num, connreq_T1, "ssap",
           "Remote SAP address",PRIV_ROOT);

/* [dsap <num>] */
NUMBER (connreq_dsap_num, connreq_opts, no_alt, OBJ(int,3),0x00,0xff,
        "Remote SAP address (default = STATION_ILLEGAL)");
KEYWORD (connreq_dsap, connreq_dsap_num, connreq_ssap, "dsap",
           "Remote SAP address",PRIV_ROOT);

/* [sa <addr>] */
ENADDR (connreq_sa_num, connreq_opts, no_alt, OBJ(hwaddr,2),
        "Source address (default = 0)");
KEYWORD (connreq_sa, connreq_sa_num, connreq_dsap, "sa",
           "Source address",PRIV_ROOT);


/* [da <addr>] */
ENADDR (connreq_da_num, connreq_opts, no_alt, OBJ(hwaddr,1),
        "Destination address (default = 0)");
KEYWORD (connreq_da, connreq_da_num, connreq_sa, "da",
           "Destination address",PRIV_ROOT);


/* [correlator <num>] */
NUMBER (connreq_correlator_num, connreq_opts, no_alt, OBJ(int,2),
        0x0,0xffffffff, "Correlator Value (default = 0xdead)");
KEYWORD (connreq_correlator, connreq_correlator_num, connreq_da, 
         "correlator","Correlator",PRIV_ROOT);

/* [flags <string>] */
STRING (connreq_flags_string, connreq_opts, no_alt, OBJ(string,1),
        "Modify Flags - 3 bytes (default = 000000)");
KEYWORD (connreq_flags, connreq_flags_string, connreq_correlator, 
         "flags","Modify Flagsr",PRIV_ROOT);


NOP (connreq_opts, connreq_flags, exec_ct_eol); 

SET (connreq_def_rif,connreq_opts,OBJ(string,2),"");
SET (connreq_def_riflength, connreq_def_rif, OBJ(int,19), 0);
SET (connreq_def_extimer,connreq_def_riflength,OBJ(int,18),60*1000);
SET (connreq_def_exretry, connreq_def_extimer, OBJ(int,17), 10);
SET (connreq_def_exoption,connreq_def_exretry, OBJ(int,16),EO_OUTBOUND_DEFAULT);
SET (connreq_def_dynwindow, connreq_def_exoption, OBJ(int,15), 0);
SET (connreq_def_xidtimer, connreq_def_dynwindow, OBJ(int,14),60*1000);
SET (connreq_def_xidretry, connreq_def_xidtimer, OBJ(int,13),10);
SET (connreq_def_maxifield, connreq_def_xidretry, OBJ(int,12), 521);
SET (connreq_def_maxincr,connreq_def_maxifield,OBJ(int,11),0);
SET (connreq_def_maxin, connreq_def_maxincr, OBJ(int,10), 7);
SET (connreq_def_maxretry,connreq_def_maxin, OBJ(int,9),0);
SET (connreq_def_maxout, connreq_def_maxretry, OBJ(int,8), 7);
SET (connreq_def_Ti, connreq_def_maxout, OBJ(int,7), 10000);
SET (connreq_def_T2, connreq_def_Ti, OBJ(int,6), 100);
SET (connreq_def_T1, connreq_def_T2, OBJ(int,5), 100);
SET (connreq_def_ssap, connreq_def_T1, OBJ(int,4), 0);
SET (connreq_def_dsap, connreq_def_ssap, OBJ(int,3), 0x04);
SET (connreq_def_sa, connreq_def_dsap, OBJ(hwaddr,2), STATION_ILLEGAL);
SET (connreq_def_da, connreq_def_sa, OBJ(hwaddr,1), 0);
SET (connreq_def_correlator,connreq_def_da, OBJ(int,2),0xdead);
SET (connreq_def_flags, connreq_def_correlator, OBJ(string,1),"000000");

/* connreq */
KEYWORD_ID (ct_connreq, connreq_def_flags, ct_disconnreq,
            OBJ(int,1), CLS_CONNECT|CLS_REQUEST, "connreq", "Connect Request",
            PRIV_ROOT);


/***************************************************************
 * clsitest closestnreq [ucepid <id>] [action <type>] [correlator <value>]
 *
 * OBJ(int,1) = CLS_CLOSE_STN|CLS_REQUEST
 * OBJ(int,2) = <id>
 * OBJ(int,3) = <type>
 * OBJ(int,4) = <value>
 *
 */

/* [correlator <value>] */
NUMBER (closestnreq_correlator_num, closestnreq_opts, no_alt, OBJ(int,4),0x0,0xffffffff,
        "Correlator Value (default = 0xdead)");
KEYWORD (closestnreq_correlator, closestnreq_correlator_num, no_alt, "correlator",
           "Correlator for closestn",PRIV_ROOT);


/* [action <type>] */
KEYWORD_ID (closestnreq_action_forced, closestnreq_opts, no_alt,
            OBJ(int,3),CLS_ACTION_FORCED,"forced",
            "Forced close station",PRIV_ROOT);
KEYWORD_ID (closestnreq_action_normal, closestnreq_opts, closestnreq_action_forced,
            OBJ(int,3),CLS_ACTION_NORMAL,"normal",
            "Normal close station",PRIV_ROOT);
KEYWORD (closestnreq_action, closestnreq_action_normal, closestnreq_correlator, "action",
           "Close station action",PRIV_ROOT);


/* [ucepid <id>] */
NUMBER (closestnreq_ucepid_num, closestnreq_opts, no_alt, OBJ(int,2),0x0,0xffffffff,
        "User CEP id (default = 0x01)");
KEYWORD (closestnreq_ucepid, closestnreq_ucepid_num, closestnreq_action, 
                    "ucepid", "User CEP id",PRIV_ROOT);

NOP (closestnreq_opts, closestnreq_ucepid, exec_ct_eol);
SET (closestnreq_def_correlator,closestnreq_opts, OBJ(int,4),0xdead);
SET (closestnreq_def_action, closestnreq_def_correlator, OBJ(int,3), 0x00);
SET (closestnreq_def_ucepid, closestnreq_def_action, OBJ(int,2),0x01);

/* closestnreq */
KEYWORD_ID (ct_closestnreq, closestnreq_def_ucepid, ct_connreq,
            OBJ(int,1), CLS_CLOSE_STN|CLS_REQUEST, "closestnreq", "Close Station Request", PRIV_ROOT);


/***************************************************************
 * clsitest openstnreq [ucepid <id>] [correlator <value>]
 *
 * OBJ(int,1) = CLS_OPEN_STN|CLS_REQUEST
 * OBJ(int,2) = <id>
 * OBJ(int,3) = <value>
 *
 */


/* [correlator <value>] */
NUMBER (openstnreq_correlator_num, openstnreq_opts, no_alt, OBJ(int,3),0x0,0xffffffff,
        "Correlator Value (default = 0xdead)");
KEYWORD (openstnreq_correlator, openstnreq_correlator_num, no_alt, "correlator",
           "Correlator for openstn",PRIV_ROOT);

/* [ucepid <id>] */
NUMBER (openstnreq_ucepid_num, openstnreq_opts, no_alt, OBJ(int,3),0x0,0xffffffff,
        "User CEP id (default = 0x01)");
KEYWORD (openstnreq_ucepid, openstnreq_ucepid_num, openstnreq_correlator, 
                    "ucepid", "User CEP id",PRIV_ROOT);

NOP (openstnreq_opts, openstnreq_ucepid, exec_ct_eol);
SET (openstnreq_def_correlator, openstnreq_opts, OBJ(int,3), 0xdead);
SET (openstnreq_def_ucepid, openstnreq_def_correlator, OBJ(int,2),0x01);

/* openstnreq */
KEYWORD_ID (ct_openstnreq, openstnreq_def_ucepid, ct_closestnreq,
            OBJ(int,1), CLS_OPEN_STN|CLS_REQUEST, "openstnreq", "Open Station Request", PRIV_ROOT);


/***************************************************************
 * clsitest flowreq [action <type>] [correlator <value>]
 *
 * OBJ(int,1) = CLS_FLOW|CLS_REQUEST
 * OBJ(int,2) = <type>
 * OBJ(int,3) = <value>
 *
 */

/* [correlator <value>] */
NUMBER (flowreq_correlator_num, flowreq_opts, no_alt, OBJ(int,3),0x0,0xffffffff,
        "Correlator Value (default = 0xdead)");
KEYWORD (flowreq_correlator, flowreq_correlator_num, no_alt, "correlator",
           "Correlator for flow",PRIV_ROOT);


/* [action <type>] */
KEYWORD_ID (flowreq_action_resume, flowreq_opts, no_alt,
            OBJ(int,2),CLS_ACTION_RESUME_FLOW,"resume",
            "Resume Flow",PRIV_ROOT);
KEYWORD_ID (flowreq_action_stop, flowreq_opts, flowreq_action_resume,
            OBJ(int,2),CLS_ACTION_STOP_FLOW,"stop",
            "Stop flow",PRIV_ROOT);
KEYWORD (flowreq_action, flowreq_action_stop, flowreq_correlator, "action",
           "Flow action",PRIV_ROOT);

NOP (flowreq_opts, flowreq_action, exec_ct_eol);
SET (flowreq_def_correlator, flowreq_opts, OBJ(int,3), 0xdead);
SET (flowreq_def_action, flowreq_def_correlator, OBJ(int,2),CLS_ACTION_STOP_FLOW);

/* flowreq */
KEYWORD_ID (ct_flowreq, flowreq_def_action, ct_openstnreq,
            OBJ(int,1), CLS_FLOW|CLS_REQUEST, "flowreq", "Flow Request", PRIV_ROOT);


/***************************************************************
 * clsitest deactsapreq [usapid <num>] [correlator <value>
 *
 * OBJ(int,1) = CLS_DEACTIVATE_SAP|CLS_REQUEST
 * OBJ(int,2) = <num>
 * OBJ(int,3) = <value>
 *
 */

/* [correlator <value>] */
NUMBER (deactsapreq_correlator_num, deactsapreq_opts, no_alt, 
        OBJ(int,3),0x0,0xffffffff,"Correlator Value (default = 0xdead)");
KEYWORD (deactsapreq_correlator, deactsapreq_correlator_num, no_alt, 
         "correlator", "Correlator Value",PRIV_ROOT);

/* [usapid <num>] */
NUMBER (deactsapreq_usapid_num, exec_ct_eol, no_alt, OBJ(int,2),0x0,0xff,
        "User SAP Id (default = 0)");
KEYWORD (deactsapreq_usapid, deactsapreq_usapid_num, deactsapreq_correlator,
         "usapid","User SAP Id",PRIV_ROOT);

NOP (deactsapreq_opts, deactsapreq_usapid, exec_ct_eol);
SET (deactsapreq_def_correlator, deactsapreq_opts, OBJ(int,3), 0xdead);
SET (deactsap_def_usapid, deactsapreq_def_correlator,OBJ(int,2),0);

/* deactsapreq */
KEYWORD_ID (ct_deactsapreq, deactsap_def_usapid, ct_flowreq,
            OBJ(int,1), CLS_DEACTIVATE_SAP|CLS_REQUEST, "deactsapreq", 
            "Deactivate SAP Request", PRIV_ROOT);


/***************************************************************
 * clsitest actsapreq [port <num>] [porttype <type>] [correlator <num>]
 *                    [sapid <num>] [T1<msec>] [T2<msec>] [Ti<msec>] 
 *                    [maxout<count>] [maxretry<count>] [maxin<count>] 
 *                    [maxincr<count>] [maxifield<size>] [xidretry<num>]
 *                    [xidtimer<msec>] [usapid <id>] [uclass <type>]
 *                    [sapflags <num>]
 *
 * OBJ(int,1) = CLS_ACTIVATE_SAP|CLS_REQUEST
 * OBJ(int,2) = <num>
 * OBJ(int,3) = <type>
 *
 */

/* [sapflags <num>] */
NUMBER (actsap_sapflags_num, actsap_opts, no_alt, OBJ(int,18),0x0,0xffffffff,
        "SAP flags (default = 0x01)");
KEYWORD (actsap_sapflags, actsap_sapflags_num, no_alt, "sapflags",
           "SAP flags",PRIV_ROOT);

/* [uclass <type>] */
NUMBER (actsap_uclass_num, actsap_opts, no_alt, OBJ(int,17),0x0,0xffffffff,
        "User Class (default = 0x02, LLC type 1 and 2)");
KEYWORD (actsap_uclass, actsap_uclass_num, actsap_sapflags, "uclass",
           "User Class",PRIV_ROOT);

/* [usapid <id>] */
NUMBER (actsap_usapid_num, actsap_opts, no_alt, OBJ(int,16),0x0,0xffffffff,
        "User SAP Id (default = 0)");
KEYWORD (actsap_usapid, actsap_usapid_num, actsap_uclass, "usapid",
           "User SAP Id",PRIV_ROOT);

/* [xidtimer <msec>] */
NUMBER (actsap_xidtimer_num, actsap_opts, no_alt, OBJ(int,15),0x0,0xffffffff,
        "Retry time in msec (default = 60*1000)");
KEYWORD (actsap_xidtimer, actsap_xidtimer_num, actsap_usapid, "xidtimer",
           "Retry time in msec",PRIV_ROOT);

/* [xidretry <num>] */
NUMBER (actsap_xidretry_num, actsap_opts, no_alt, OBJ(int,14),0x0,0xffffffff,
        "Number of times to try (default = 10)");
KEYWORD (actsap_xidretry, actsap_xidretry_num, actsap_xidtimer, "xidretry",
           "Number of times to try",PRIV_ROOT);


/* [maxifield <count>] */
NUMBER (actsap_maxifield_num, actsap_opts, no_alt, OBJ(int,13),0x0,0xffffffff,
        "Maximum I-field length (default = 521)");
KEYWORD (actsap_maxifield, actsap_maxifield_num, actsap_xidretry, "maxifield",
           "Maximum I-field length",PRIV_ROOT);

/* [maxincr <count>] */
NUMBER (actsap_maxincr_num, actsap_opts, no_alt, OBJ(int,12),0x0,0xffffffff,
        "Maxout increment (default = 0)");
KEYWORD (actsap_maxincr, actsap_maxincr_num, actsap_maxifield, "maxincr",
           "Maxout increment",PRIV_ROOT);

/* [maxin <count>] */
NUMBER (actsap_maxin_num, actsap_opts, no_alt, OBJ(int,11),0x0,0xffffffff,
        "Maxin (default = 7)");
KEYWORD (actsap_maxin, actsap_maxin_num, actsap_maxincr, "maxin",
           "Maxin",PRIV_ROOT);

/* [maxretry <count>] */
NUMBER (actsap_maxretry_num, actsap_opts, no_alt, OBJ(int,10),0x0,0xffffffff,
        "Max retry count (default = 0)");
KEYWORD (actsap_maxretry, actsap_maxretry_num, actsap_maxin, "maxretry",
           "Max retry count",PRIV_ROOT);

/* [maxout <count>] */
NUMBER (actsap_maxout_num, actsap_opts, no_alt, OBJ(int,9),0x0,0xffffffff,
        "Maxout (default = 7)");
KEYWORD (actsap_maxout, actsap_maxout_num, actsap_maxretry, "maxout",
           "Maxout",PRIV_ROOT);

/* [Ti <msec>] */
NUMBER (actsap_Ti_num, actsap_opts, no_alt, OBJ(int,8),0x0,0xffffffff,
        "Ti - Inactivity timer (default = 10000 msec)");
KEYWORD (actsap_Ti, actsap_Ti_num, actsap_maxout, "Ti",
           "Ti - Inactivity Timer",PRIV_ROOT);

/* [T2 <msec>] */
NUMBER (actsap_T2_num, actsap_opts, no_alt, OBJ(int,7),0x0,0xffffffff,
        "T2 timer (default = 100 msec)");
KEYWORD (actsap_T2, actsap_T2_num, actsap_Ti, "T2",
           "T2 - Acknowledgement Timer",PRIV_ROOT);

/* [T1 <msec>] */
NUMBER (actsap_T1_num, actsap_opts, no_alt, OBJ(int,6),0x0,0xffffffff,
        "T1 timer (default = 100 msec)");
KEYWORD (actsap_T1, actsap_T1_num, actsap_T2, "T1",
           "T1 - Response Timer",PRIV_ROOT);

/* [sapid <num>] */
NUMBER (actsap_sap_num, actsap_opts, no_alt, OBJ(int,5),0x00,0xff,
        "SAP value (default = 0x04)");
KEYWORD (actsap_sap, actsap_sap_num, actsap_T1, "sapid",
           "SAP value",PRIV_ROOT);


/* [correlator <value>] */
NUMBER (actsap_correlator_num, actsap_opts, no_alt, OBJ(int,4),0x0,0xffffffff,
        "Correlator Value (default = 0xdead)");
KEYWORD (actsap_correlator, actsap_correlator_num, actsap_sap, "correlator",
           "Correlator Value",PRIV_ROOT);


/* [porttype <type>] */
KEYWORD_ID (actsap_port_type_fddi, actsap_opts, no_alt,
            OBJ(int,3),CLS_LLC,"fddi",
            "Enable FDDI port",PRIV_ROOT);
KEYWORD_ID (actsap_port_type_frame_relay, actsap_opts, actsap_port_type_fddi,
            OBJ(int,3),CLS_FRAME_RELAY,"frame-relay",
            "Enable frame-relay port",PRIV_ROOT);
KEYWORD_ID (actsap_port_type_rsrb, actsap_opts, actsap_port_type_frame_relay,
            OBJ(int,3),CLS_RSRB,"rsrb",
            "Enable RSRB port",PRIV_ROOT);
KEYWORD_ID (actsap_port_type_local, actsap_opts, actsap_port_type_rsrb,
            OBJ(int,3),CLS_LOCAL,"local",
            "Enable local port",PRIV_ROOT);
KEYWORD_ID (actsap_port_type_token_ring, actsap_opts, actsap_port_type_local,
            OBJ(int,3),CLS_LLC,"token-ring",
            "Enable token-ring port",PRIV_ROOT);
KEYWORD_ID (actsap_port_type_ethernet, actsap_opts, actsap_port_type_token_ring,
            OBJ(int,3),CLS_LLC,"ethernet",
            "Enable ethernet port",PRIV_ROOT);
KEYWORD_ID (actsap_port_type_socket, actsap_opts, actsap_port_type_ethernet,
            OBJ(int,3),CLS_SOCKET,"socket",
            "Enable socket port",PRIV_ROOT);
KEYWORD_ID (actsap_port_type_sdlc, actsap_opts, actsap_port_type_socket,
            OBJ(int,3),CLS_SDLC,"sdlc",
            "Enable SDLC port",PRIV_ROOT);
KEYWORD (actsap_port_type, actsap_port_type_sdlc, actsap_correlator, "type",
           "Port Type",PRIV_ROOT);


/* [port <number>] */
NUMBER (actsap_port_num, actsap_opts, no_alt, OBJ(int,2),0,255,
        "Port Number (default = 2)");
KEYWORD (actsap_port, actsap_port_num, actsap_port_type, "port",
           "Port Number",PRIV_ROOT);

NOP (actsap_opts, actsap_port, exec_ct_eol); 

SET (actsap_def_sapflags, actsap_opts, OBJ(int,18), CLS_TEST_RESPONSE);
SET (actsap_def_uclass, actsap_def_sapflags, OBJ(int,17), 0x02);
SET (actsap_def_usapid, actsap_def_uclass, OBJ(int,16), 0);
SET (actsap_def_xidtimer, actsap_def_usapid, OBJ(int,15), 60*1000);
SET (actsap_def_xidretry, actsap_def_xidtimer, OBJ(int,14), 10);
SET (actsap_def_maxifield, actsap_def_xidretry, OBJ(int,13), 521);
SET (actsap_def_maxincr,actsap_def_maxifield,OBJ(int,12),0);
SET (actsap_def_maxin, actsap_def_maxincr, OBJ(int,11), 7);
SET (actsap_def_maxretry,actsap_def_maxin, OBJ(int,10),0);
SET (actsap_def_maxout, actsap_def_maxretry, OBJ(int,9), 7);
SET (actsap_def_Ti, actsap_def_maxout, OBJ(int,8), 10000);
SET (actsap_def_T2, actsap_def_Ti, OBJ(int,7), 100);
SET (actsap_def_T1, actsap_def_T2, OBJ(int,6), 100);
SET (actsap_def_sap, actsap_def_T1, OBJ(int,5), 0x04);
SET (actsap_def_correlator, actsap_def_sap, OBJ(int,4), 0xdead);
SET (actsap_def_port_type, actsap_def_correlator, OBJ(int,3), CLS_LLC);
SET (actsap_def_port, actsap_def_port_type, OBJ(int,2), 2);

/* actsapreq */
KEYWORD_ID (ct_actsapreq, actsap_def_port, ct_deactsapreq,
            OBJ(int,1), CLS_ACTIVATE_SAP|CLS_REQUEST, "actsapreq", "Activate SAP Request", PRIV_ROOT);


/***************************************************************
 * clsitest enablereq [port <num>] [porttype <type>] [correlator <value>]
 *
 * OBJ(int,1) = CLS_ENABLE|CLS_REQUEST
 * OBJ(int,2) = <num>
 * OBJ(int,3) = <type>
 * OBJ(int,4) = <value>
 *
 */

/* [correlator <value>] */
NUMBER (enre_correlator_num, enre_opts, no_alt, OBJ(int,4),0x0,0xffffffff,
        "Correlator Value (default = 0xdead)");
KEYWORD (enre_correlator, enre_correlator_num, no_alt, "correlator",
           "Set correlator for enable request",PRIV_ROOT);


/* [porttype <type>] */
KEYWORD_ID (enre_port_type_fddi, enre_opts, no_alt,
            OBJ(int,3),CLS_LLC,"fddi",
            "Enable FDDI port",PRIV_ROOT);
KEYWORD_ID (enre_port_type_frame_relay, enre_opts, enre_port_type_fddi,
            OBJ(int,3),CLS_FRAME_RELAY,"frame-relay",
            "Enable frame-relay port",PRIV_ROOT);
KEYWORD_ID (enre_port_type_rsrb, enre_opts, enre_port_type_frame_relay,
            OBJ(int,3),CLS_RSRB,"rsrb",
            "Enable RSRB port",PRIV_ROOT);
KEYWORD_ID (enre_port_type_local, enre_opts, enre_port_type_rsrb,
            OBJ(int,3),CLS_LOCAL,"local",
            "Enable local port",PRIV_ROOT);
KEYWORD_ID (enre_port_type_token_ring, enre_opts, enre_port_type_local,
            OBJ(int,3),CLS_LLC,"token-ring",
            "Enable token-ring port",PRIV_ROOT);
KEYWORD_ID (enre_port_type_ethernet, enre_opts, enre_port_type_token_ring,
            OBJ(int,3),CLS_LLC,"ethernet",
            "Enable ethernet port",PRIV_ROOT);
KEYWORD_ID (enre_port_type_socket, enre_opts, enre_port_type_ethernet,
            OBJ(int,3),CLS_SOCKET,"socket",
            "Enable socket port",PRIV_ROOT);
KEYWORD_ID (enre_port_type_sdlc, enre_opts, enre_port_type_socket,
            OBJ(int,3),CLS_SDLC,"sdlc",
            "Enable SDLC port",PRIV_ROOT);
KEYWORD (enre_port_type, enre_port_type_sdlc, enre_correlator, "type",
           "Set port type for enable request",PRIV_ROOT);


/* [port <number>] */
NUMBER (enre_port_num, enre_opts, no_alt, OBJ(int,2),0,255,
        "Port Number (default = 2)");
KEYWORD (enre_port, enre_port_num, enre_port_type, "port",
           "Set port number for enable request",PRIV_ROOT);

NOP (enre_opts, enre_port, exec_ct_eol);
SET (enre_def_correlator, enre_opts, OBJ(int,4), 0xdead);
SET (enre_def_port_type, enre_def_correlator, OBJ(int,3), CLS_LLC);
SET (enre_def_port, enre_def_port_type, OBJ(int,2), 2);

/* enablereq */
KEYWORD_ID (ct_enablereq, enre_def_port, ct_actsapreq,
            OBJ(int,1), CLS_ENABLE|CLS_REQUEST, "enablereq", "Enable Request", PRIV_ROOT);


/***************************************************************
 * clsitest disablereq [port <num>] [porttype <type>] [correlator <value>]
 *
 * OBJ(int,1) = CLS_DISABLE|CLS_REQUEST
 * OBJ(int,2) = <num>
 * OBJ(int,3) = <type>
 * OBJ(int,4) = <value>
 *
 */

/* [correlator <value>] */

NUMBER (dire_correlator_num, dire_opts, no_alt, OBJ(int,4),0x0,0xffffffff,
        "Correlator Value (default = 0xdead)");
KEYWORD (dire_correlator, dire_correlator_num, no_alt, "correlator",
           "Set correlator for disable request",PRIV_ROOT);


/* [porttype <type>] */
KEYWORD_ID (dire_port_type_fddi, dire_opts, no_alt,
            OBJ(int,3),CLS_LLC,"fddi",
            "Disable FDDI port",PRIV_ROOT);
KEYWORD_ID (dire_port_type_frame_relay, dire_opts, dire_port_type_fddi,
            OBJ(int,3),CLS_FRAME_RELAY,"frame-relay",
            "Disable frame-relay port",PRIV_ROOT);
KEYWORD_ID (dire_port_type_rsrb, dire_opts, dire_port_type_frame_relay,
            OBJ(int,3),CLS_RSRB,"rsrb",
            "Disable RSRB port",PRIV_ROOT);
KEYWORD_ID (dire_port_type_local, dire_opts, dire_port_type_rsrb,
            OBJ(int,3),CLS_LOCAL,"local",
            "Disable local port",PRIV_ROOT);
KEYWORD_ID (dire_port_type_token_ring, dire_opts, dire_port_type_local,
            OBJ(int,3),CLS_LLC,"token-ring",
            "Disable token-ring port",PRIV_ROOT);
KEYWORD_ID (dire_port_type_ethernet, dire_opts, dire_port_type_token_ring,
            OBJ(int,3),CLS_LLC,"ethernet",
            "Disable ethernet port",PRIV_ROOT);
KEYWORD_ID (dire_port_type_socket, dire_opts, dire_port_type_ethernet,
            OBJ(int,3),CLS_SOCKET,"socket",
            "Disable socket port",PRIV_ROOT);
KEYWORD_ID (dire_port_type_sdlc, dire_opts, dire_port_type_socket,
            OBJ(int,3),CLS_SDLC,"sdlc",
            "Disable SDLC port",PRIV_ROOT);
KEYWORD (dire_port_type, dire_port_type_sdlc, dire_correlator, "type",
           "Set port type for disable request",PRIV_ROOT);


/* [port <number>] */
NUMBER (dire_port_num, dire_opts, no_alt, OBJ(int,2),0,255,
        "Port Number (default = 2)");
KEYWORD (dire_port, dire_port_num, dire_port_type, "port",
           "Set port number for disable request",PRIV_ROOT);

NOP (dire_opts, dire_port, exec_ct_eol);
SET (dire_def_correlator, dire_opts, OBJ(int,4), 0xdead);
SET (dire_def_port_type, dire_def_correlator, OBJ(int,3), CLS_LLC);
SET (dire_def_port, dire_def_port_type, OBJ(int,2), 2);

/* disablereq */
KEYWORD_ID (ct_disablereq, dire_def_port, ct_enablereq,
            OBJ(int,1), CLS_DISABLE|CLS_REQUEST, "disablereq", "Disable Request", PRIV_ROOT);



/***************************************************************
 * clsitest stop
 *
 * OBJ(int,1) = CLS_STOP
 */
KEYWORD_ID (ct_stop, exec_ct_eol, ct_disablereq,
            OBJ(int,1), CLSISTOP, "stop", "Stop CLSI test program", PRIV_ROOT);


/***************************************************************
 * clsitest start
 *
 * OBJ(int,1) = CLS_START
 */

KEYWORD_ID (ct_start, exec_ct_eol, ct_stop,
            OBJ(int,1), CLSISTART, "start", "Start CLSI test program", PRIV_ROOT);

/****************************************************************
 * clsitest start
 *          stop
 *          enablereq 
 *
 *
 */
KEYWORD	(exec_clsitest, ct_start, ALTERNATE,
	 "clsitest", "CLSI test program execution command", PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	exec_clsitest



