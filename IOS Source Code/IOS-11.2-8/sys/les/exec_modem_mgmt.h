/* $Id: exec_modem_mgmt.h,v 3.1.68.2 1996/09/09 20:56:36 jchlin Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_modem_mgmt.h,v $
 *------------------------------------------------------------------
 * A dummy file for brasil branch.
 * E X E C _ M O D E M _ M G M T . H
 *
 * November, 1995 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_modem_mgmt.h,v $
 * Revision 3.1.68.2  1996/09/09  20:56:36  jchlin
 * CSCdi66525:  ct1/rbs cannot busy back the CO trunk when theres no free
 * modem
 * Branch: California_branch
 * o       The bugfix will busy dsx0s back to CO thru RBS when there's
 *         no free modem to accept incoming calls.
 * o       The modem pool is changed to have multiple pools for different
 *         types of modems.
 * o       Add hidden debug commands.
 *
 * Revision 3.1.68.1  1996/06/16  21:14:20  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/06  19:12:24  jchlin
 * Placeholders for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(exec_modem_mgmt_extend_here, no_alt);

/* forward declarations */
pdecl(modem_mgmt_port_num1);
pdecl(modem_mgmt_port_num1w);
 
/****************************************************************************
 * slot-group <number> ports <range>
 *
 * <range>      :: <range entry> | <range> ',' <range_entry>
 * <range_entry>:: <number> | <number> '-' <number>
 *
 *      OBJ(int,1) = action id
 *      OBJ(int,2) = slot number
 *      OBJ(int,3) = ports (bit field)
 *      OBJ(int,4) - (used as a temp variable)
 *      OBJ(int,5) - (used as a temp variable)
 */
EOLS    (modem_mgmt_port_eol, modem_mgmt_exec_command, MODEM_MGMT_EXEC_CSM_MODE);
 
CHARACTER(modem_mgmt_port_num4, modem_mgmt_port_num1w, modem_mgmt_port_eol, ',');

EVAL    (modem_mgmt_port_num3a, modem_mgmt_port_num4, SETOBJ(int,3) =
         GETOBJ(int,3) | csm_make_bit_field(GETOBJ(int,4), GETOBJ(int,5)));

GENERAL_NUMBER(modem_mgmt_port_num3, modem_mgmt_port_num3a, no_alt, OBJ(int,5),
               0, 31, "List of ports which comprise the slot",
               NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);

WHITESPACE(modem_mgmt_port_num1w, modem_mgmt_port_num1, modem_mgmt_port_num1);
WHITESPACE(modem_mgmt_port_num3w, modem_mgmt_port_num3, modem_mgmt_port_num3);
CHARACTER(modem_mgmt_port_num2a, modem_mgmt_port_num1w, modem_mgmt_port_eol, ',');
CHARACTER(modem_mgmt_port_num2, modem_mgmt_port_num3w, modem_mgmt_port_num2a, '-');

EVAL    (modem_mgmt_port_num1a, modem_mgmt_port_num2,
         SETOBJ(int,3) =
               GETOBJ(int,3) | csm_make_bit_field(GETOBJ(int,4), GETOBJ(int,4)));

GENERAL_NUMBER(modem_mgmt_port_num1, modem_mgmt_port_num1a, no_alt, OBJ(int,4),
               0, 31, "List of ports which comprise the slot",
               NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);

SET     (modem_mgmt_port_num0, modem_mgmt_port_num1, OBJ(int,3), 0);

KEYWORD (modem_mgmt_port, modem_mgmt_port_num0, no_alt, "ports",
         "List of ports in the slot group",
         PRIV_USER);

NUMBER  (modem_mgmt_slot_num, modem_mgmt_port, no_alt, OBJ(int,2), 0,
         2, "Slot number");

KEYWORD (modem_mgmt_slot, modem_mgmt_slot_num, exec_modem_mgmt_extend_here, "slot-group",
         "Specify the ports to slot-group mapping for an interface",
         PRIV_USER);

STRING  (modem_mgmt_dial_numstr, modem_mgmt_slot, no_alt,
         OBJ(string,1), "the phone number for specified modem dialing out");
 
SET     (modem_mgmt_clr_dial_num, modem_mgmt_dial_numstr, OBJ(string, 1), 0);

KEYWORD (modem_mgmt_dial_num, modem_mgmt_clr_dial_num, modem_mgmt_slot, "dial-number",
         "Specify the phone number to dial out",
         PRIV_USER);
 
NUMBER  (modem_mgmt_ring_delay, modem_mgmt_port_eol, no_alt, OBJ(int,2), 0,
         600, "seconds");

/******************************************************************
 *  csm [ on-hook | outbound-call | inbound-call | isdn-idle | busy-out | ring-delay | ringing] 
 *      OBJ(int,1) = CSM_EXEC_ON_HOOK | CSM_EXEC_OUTBOUND_CALL | CSM_EXEC_INBOUND_CALL
 *                   | CSM_EXEC_ISDN_IDLE | CSM_EXEC_BUSYOUT | CSM_EXEC_NO_BUSYOUT
 *                   | CSM_EXEC_RING_DELAY | CSM_EXEC_RINGING | CSM_EXEC_DISABLE_ROUND_ROBIN
 *                   | CSM_EXEC_ENABLE_ROUND_ROBIN | CSM_EXEC_NO_DEBUG_RBS
 *                   | CSM_EXEC_DEBUG_RBS
 */

KEYWORD_ID(modem_mgmt_csm_exec11, modem_mgmt_port_eol, no_alt,
           OBJ(int,1), CSM_EXEC_DEBUG_RBS, "debug-rbs",
           "enable modem pool round robin scheme",
           PRIV_USER);
 
KEYWORD_ID(modem_mgmt_csm_exec10, modem_mgmt_port_eol, modem_mgmt_csm_exec11,
           OBJ(int,1), CSM_EXEC_NO_DEBUG_RBS, "no-debug-rbs",
           "disable modem pool round robin scheme",
           PRIV_USER);
 
KEYWORD_ID(modem_mgmt_csm_exec9, modem_mgmt_port_eol, modem_mgmt_csm_exec10,
           OBJ(int,1), CSM_EXEC_ENABLE_ROUND_ROBIN, "modem-roundrobin",
           "enable modem pool round robin scheme",
           PRIV_USER);

KEYWORD_ID(modem_mgmt_csm_exec8, modem_mgmt_port_eol, modem_mgmt_csm_exec9,
           OBJ(int,1), CSM_EXEC_DISABLE_ROUND_ROBIN, "no-modem-roundrobin",
           "disable modem pool round robin scheme",
           PRIV_USER);

KEYWORD_ID(modem_mgmt_csm_exec7, modem_mgmt_slot, modem_mgmt_csm_exec8,
           OBJ(int,1), CSM_EXEC_RINGING, "ringing", "apply ring indication",
           PRIV_USER);

KEYWORD_ID(modem_mgmt_csm_exec6, modem_mgmt_ring_delay, modem_mgmt_csm_exec7,
           OBJ(int,1), CSM_EXEC_RING_DELAY, "ring-delay", "ring delay in seconds",
           PRIV_USER);

KEYWORD_ID(modem_mgmt_csm_exec5, modem_mgmt_slot, modem_mgmt_csm_exec6,
           OBJ(int,1), CSM_EXEC_NO_BUSYOUT, "no-busy-out", "clear nice modem busyout",
           PRIV_USER);

KEYWORD_ID(modem_mgmt_csm_exec4, modem_mgmt_slot, modem_mgmt_csm_exec5,
           OBJ(int,1), CSM_EXEC_BUSYOUT, "busy-out", "nice modem busyout",
           PRIV_USER);

KEYWORD_ID(modem_mgmt_csm_exec3, modem_mgmt_slot, modem_mgmt_csm_exec4,
           OBJ(int,1), CSM_EXEC_ISDN_IDLE, "isdn-idle", "disconnect isdn call",
           PRIV_USER);

KEYWORD_ID(modem_mgmt_csm_exec2, modem_mgmt_slot, modem_mgmt_csm_exec3,
           OBJ(int,1), CSM_EXEC_INBOUND_CALL, "inbound-call", "make inbound calls",
           PRIV_USER);

KEYWORD_ID(modem_mgmt_csm_exec1, modem_mgmt_dial_num, modem_mgmt_csm_exec2,
           OBJ(int,1), CSM_EXEC_OUTBOUND_CALL, "outbound-call", "make outbound calls",
           PRIV_USER);

KEYWORD_ID(modem_mgmt_csm_exec, modem_mgmt_slot, modem_mgmt_csm_exec1,
           OBJ(int,1), CSM_EXEC_ON_HOOK, "on-hook", "put modem on-hook", PRIV_USER);

/******************************************************************
 * modem-mgmt csm
 *  OBJ(int,1) = modem to manage
 *
 */
KEYWORD (exec_modem_mgmt_csm_mode, modem_mgmt_csm_exec,
	 exec_modem_mgmt_extend_here, "csm",
	 "management CSM commands", PRIV_USER);

KEYWORD_MM (exec_modem_mgmt, exec_modem_mgmt_csm_mode, ALTERNATE,
	 MODEM_MGMT_KEYWORD, "Modem Management CSM commands", PRIV_INTERNAL, 6);

#undef	ALTERNATE
#define	ALTERNATE	exec_modem_mgmt

