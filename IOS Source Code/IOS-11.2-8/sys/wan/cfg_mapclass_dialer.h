/* $Id: cfg_mapclass_dialer.h,v 3.2.34.2 1996/07/26 18:49:15 suresh Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_mapclass_dialer.h,v $
 *------------------------------------------------------------------
 * Dialer map class configuration commands
 *
 * June 1995, Dana Blair
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_mapclass_dialer.h,v $
 * Revision 3.2.34.2  1996/07/26  18:49:15  suresh
 * CSCdi63924:  ISDN call setup with 1TR6 and new-style DDR fails
 * Branch: California_branch
 * ISDN SPC is false by default, changed the appropriate command.
 *
 * Revision 3.2.34.1  1996/04/27  06:35:44  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.2.2.1  1996/04/23  00:24:03  suresh
 * Branch: Dial1_branch
 * Add Dialer Profiles support to Dial1_branch
 *
 * Revision 3.2  1996/01/12  16:47:56  rbeach
 * CSCdi46101:  ISDN 4ESS Pri doesn't correctly handle outbound NSF.
 * The correct network specific facility values are now configurable.
 * We support outbound SDN, MEGACOMM and ACCUNET.
 *
 * Revision 3.1  1995/11/09  13:48:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/17  22:18:01  rbeach
 * CSCdi36915:  voice calls are not handled correctly over ISDN.
 * Allow configuration for data over voice on both outgoing and
 * incoming calls. For outgoing there is a new option for the
 * dialer map command called class. For incoming there is a new
 * interface option "isdn incoming-voice data". This will allow
 * the isdn code to answer incoming voice calls.
 *
 * Revision 2.1  1995/09/30  05:06:05  dblair
 * CSCdi41333:  DDR Callback initial commit
 * Adding Callback feature for DDR interfaces.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] dialer outgoing [ sdn, megacomm, accunet ]
 */
EOLS    (cmc_accu_eol, dialer_mapclass_command,
         DIALER_MAP_ACCU_CALL);

KEYWORD (cmc_dialer_plan_accu, cmc_accu_eol,
         no_alt, "accunet", "Dial using an NSF of AT&T ACCUNET.",
         PRIV_CONF);

EOLS    (cmc_mega_eol, dialer_mapclass_command,
         DIALER_MAP_MEGA_CALL);

KEYWORD (cmc_dialer_plan_mega, cmc_mega_eol,
         cmc_dialer_plan_accu, "megacomm", "Dial using an NSF of AT&T MEGACOMM.",
         PRIV_CONF);

EOLS    (cmc_sdn_eol, dialer_mapclass_command,
         DIALER_MAP_SDN_CALL);

KEYWORD (cmc_dialer_plan_sdn, cmc_sdn_eol,
         cmc_dialer_plan_mega, "sdn", "Dial using an NSF of AT&T SDN.",
         PRIV_CONF);

KEYWORD (cmc_dialer_plan, cmc_dialer_plan_sdn,
         no_alt, "outgoing", "Dial using the selected ISDN dialing plan.",
         PRIV_CONF);

ASSERT (cmc_dialer_plan_check, cmc_dialer_plan, NONE,
         (reg_invoke_isdn_pri_getswitch() == PRI_4ESS_STYPE));

/*
 * *********************************************************
 * [no] dialer isdn speed 56 | 64 spc
 */
EOLS	  (cmc_dialer_isdn_eol, dialer_mapclass_command, DIALER_MAP_ISDN);

/* spc :  semi permanent connecions */

KEYWORD_ID(cmc_dialer_isdn_spc, cmc_dialer_isdn_eol,
	   cmc_dialer_isdn_eol, OBJ(int,4), TRUE, "spc", 
	   "Semi Permanent Connections", PRIV_CONF);

/* speed { 56 | 64 } */

KEYWORD_ID(cmc_dialer_isdn_speed_56, cmc_dialer_isdn_spc,
	   no_alt, OBJ(int,3), 56, "56", "56K bps",
	   PRIV_CONF);

KEYWORD_ID(cmc_dialer_isdn_speed, cmc_dialer_isdn_speed_56, 
	   cmc_dialer_isdn_spc, OBJ(int,2), TRUE,
	   "speed", "Set ISDN speed", PRIV_CONF);

KEYWORD_ID(cmc_dialer_isdn, cmc_dialer_isdn_speed, 
	   cmc_dialer_plan_check, OBJ(int,1), TRUE,
	   "isdn", "ISDN Settings", PRIV_CONF);

/***************************************************************
 * dialer wait-for-carrier-time <num>
 * no dialer wait-for-carrier-time
 */

PARAMS_KEYONLY(cmc_dialer_wait, cmc_dialer_isdn,
	       "wait-for-carrier-time", OBJ(int,1), 1, DIALER_MAX_TIMER, 
	       dialer_mapclass_command, DIALER_MAP_CARRIER,
	       "How long the router will wait for carrier",
	       "Wait for carrier time in seconds", PRIV_CONF);

/***************************************************************
 * dialer enable-timeout <num>
 * no dialer enable-timeout
 */
PARAMS_KEYONLY(cmc_dialer_enable, cmc_dialer_wait, "enable-timeout",
	       OBJ(int,1), 1, DIALER_MAX_TIMER,
	       dialer_mapclass_command, DIALER_MAP_REENABLE,
	       "Set length of time an interface stays down"
	       " before it is available for dialing",
	       "Enable timeout in seconds", PRIV_CONF);

/***************************************************************
 * dialer fast-idle <num>
 * no dialer fast-idle
 */
PARAMS_KEYONLY(cmc_dialer_fast_idle, cmc_dialer_enable, "fast-idle",
	       OBJ(int,1), 1, DIALER_MAX_TIMER,
	       dialer_mapclass_command, DIALER_MAP_FAST_IDLE,
	       "Set idle time before disconnecting line with an"
	       " unusually high level of contention",
	       "Fast idle in seconds", PRIV_CONF);

/***************************************************************
 * dialer idle-timeout <num>
 * no dialer idle-timeout
 */
PARAMS_KEYONLY(cmc_idle_timeout,  cmc_dialer_fast_idle,
	       "idle-timeout", OBJ(int,1), 1, DIALER_MAX_TIMER,
	       dialer_mapclass_command, DIALER_MAP_IDLE,
	       "Set idle time before disconnecting line",
	       "Idle timeout in seconds", PRIV_CONF);

/***************************************************************
 * [no] dialer voice-call
 */
EOLS    (cmc_voice_call_eol, dialer_mapclass_command,
         DIALER_MAP_VOICE_CALL);

KEYWORD (cmc_voice_call, cmc_voice_call_eol, cmc_idle_timeout,
	 "voice-call", "Dial the configured number as a voice call",
	 PRIV_CONF);

/***************************************************************
 * [no] dialer callback-server [ username ] [ dial-string ] 
 * GETOBJ(int,1) = boolean username
 * GETOBJ(int,2) = boolean dial-string
 */
EOLS    (cmc_dialer_callback_server_eol, dialer_mapclass_command,
	 DIALER_MAP_CALLBACK_SERVER);

pdecl( int_dialer_class_opts);
    
KEYWORD_ID(cmc_dialer_callback_server_user, int_dialer_class_opts, 
           cmc_dialer_callback_server_eol,
	   OBJ(int,1), TRUE,
	   "username",
	   "Identify return call dial string using authenticated username",
	   PRIV_CONF);

KEYWORD_ID(cmc_dialer_callback_server_dial, int_dialer_class_opts, 
           cmc_dialer_callback_server_user,
	   OBJ(int,2), TRUE,
	   "dial-string",
	   "Get return call dial string from dial string option of callback negotiation",
	   PRIV_CONF);
NOP	(int_dialer_class_opts, cmc_dialer_callback_server_dial, NONE);

KEYWORD (cmc_dialer_callback_server, cmc_dialer_callback_server_dial,
	 cmc_voice_call, "callback-server", "Enable callback return call",
	 PRIV_CONF);

/***************************************************************/
KEYWORD	(cmc_dialer, cmc_dialer_callback_server, ALTERNATE,
           "dialer", "Configure dialer static map class", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE       cmc_dialer















