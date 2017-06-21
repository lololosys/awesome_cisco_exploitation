/* $Id: exec_debug_modem_csm.h,v 3.1.64.1 1996/06/16 21:14:13 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_debug_modem_csm.h,v $
 *------------------------------------------------------------------
 *  E X E C _ D E B U G _ M O D E M . H
 *
 * Nov 1995, Viet Nguyen
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_debug_modem_csm.h,v $
 * Revision 3.1.64.1  1996/06/16  21:14:13  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/21  23:39:19  vnguyen
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * debug modem {oob | csm | trace{abnormal | normal | all}} 
 *             [<slot/port #> | group [<group #>]] 
 * 
 * OBJ(int,1) = DEBUG_MODEM_OOB | DEBUG_MODEM_CSM | DEBUG_MODEM_TRACE
 * OBJ(int,2) = slot #
 * OBJ(int,3) = port #
 * OBJ(int,4) = group #
 * OBJ(int,5) = TRUE iff slot/port # is entered
 * OBJ(int,6) = TRUE iff group # is entered
 * OBJ(int,7) = DEBUG_TRACE_ALL | DEBUG_TRACE_ABNORM | DEBUG_TRACE_NORM
 *
 */
EOLS       (debug_modem_eol, debug_modem_command, DEBUG_MODEM);
EOLS       (debug_modem_group_eol, debug_modem_command, DEBUG_MODEM_GROUP);
SET        (debug_modem_group_num_set, debug_modem_group_eol,
            OBJ(int,6), TRUE);
NUMBER     (debug_modem_group_num, debug_modem_group_num_set,
            debug_modem_group_eol, OBJ(int,4), 1, PLATFORM_MAXLINES, 
            "Group number");
KEYWORD    (debug_modem_group, debug_modem_group_num,
            debug_modem_eol, "group", "Modem group information",
            PRIV_OPR);
SET        (debug_modem_slot_port_num_set, debug_modem_eol,
            OBJ(int,5), TRUE);
GENERAL_NUMBER (debug_modem_port_num, debug_modem_slot_port_num_set,
            NONE, OBJ(int,3), 0, 23, "Port number",
            NUMBER_DEC | NUMBER_WS_OK );
CHARACTER  (debug_modem_slot_separator, debug_modem_port_num,
            NONE, '/');
GENERAL_NUMBER (debug_modem_slot_num, debug_modem_slot_separator,
            debug_modem_group, OBJ(int,2), 0, 2, "Slot/Port number (i.e. 1/1)",
            NUMBER_DEC | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
SET        (debug_modem_slot_port_num_clear, debug_modem_slot_num,
            OBJ(int,5), FALSE);
SET        (debug_modem_set, debug_modem_slot_port_num_clear, 
            OBJ(pdb,1), modem_debug_arr);
KEYWORD_ID (debug_modem_csm, debug_modem_set, NONE,
            OBJ(int,1), DEBUG_MODEM_CSM,
            "csm", "CSM activity", PRIV_OPR);
KEYWORD_ID (debug_trace_norm, debug_modem_set, NONE,
            OBJ(int,7), DEBUG_TRACE_NORM,
            "normal", "Upload Trace on Normal Disconn", PRIV_OPR);
KEYWORD_ID (debug_trace_abnorm, debug_modem_set, debug_trace_norm,
            OBJ(int,7), DEBUG_TRACE_ABNORM,
            "abnormal", "Upload Trace on Abnormal Disconn", PRIV_OPR);
KEYWORD_ID (debug_trace_all, debug_modem_set, debug_trace_abnorm,
            OBJ(int,7), DEBUG_TRACE_ALL,
            "all", "Upload Trace on Any Disconn", PRIV_OPR);
KEYWORD_ID (debug_modem_trace, debug_trace_all, NONE,
            OBJ(int,1), DEBUG_MODEM_TRACE,
            "trace", "Call Trace Upload", PRIV_OPR);
KEYWORD_ID (debug_modem_oob, debug_modem_set, debug_modem_trace,
            OBJ(int,1), DEBUG_MODEM_OOB,
            "oob", "Modem out of band activity", PRIV_OPR);

/***************************************************************
 * check to see if it is manageable modem image, only certain
 * management commands are valid for manageable image
 */
ASSERT  (debug_modem_manageable, debug_modem_oob, debug_modem_csm, 
         reg_invoke_modem_mgmt_exists());

#undef	ALTERNATE
#define	ALTERNATE	debug_modem_manageable
