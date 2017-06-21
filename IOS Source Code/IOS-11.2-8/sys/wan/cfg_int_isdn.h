/* $Id: cfg_int_isdn.h,v 3.4.20.2 1996/06/16 21:20:53 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_isdn.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ I S D N . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_isdn.h,v $
 * Revision 3.4.20.2  1996/06/16  21:20:53  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.4.20.1  1996/04/10  18:42:07  hrobison
 * CSCdi52503:  LIF_Fatal displays garbage with ISDN process exits
 * Branch: California_branch
 * ISDN configuration commands should only be asserted for
 * T1/E1 controllers configured for PRI.
 *
 * Revision 3.4  1996/01/26  01:31:03  hrobison
 * CSCdi43998:  BRI COMMANDS ARE NOT RECOGNIZED IN UNITS WITH MBRI &
 * CE1/CT1
 * Move increment of nprinets so that this only happens if
 * PRI service is configured.  Previously nprinets was
 * incremented if a 4XXX CT1/PRI or CE1/PRI, or 7XXX MIP
 * was installed even if no pri-group was configured.
 * This prevented channelized service from working in
 * the same router with BRI service.  Parser checks for
 * nprinets have been changed to check for either
 * nbrinets or (nt1ports, ne1ports, nst1ports or
 * nse1ports).
 *
 * Revision 3.3  1995/12/07  17:45:36  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  18:01:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:35:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/10/17  22:18:00  rbeach
 * CSCdi36915:  voice calls are not handled correctly over ISDN.
 * Allow configuration for data over voice on both outgoing and
 * incoming calls. For outgoing there is a new option for the
 * dialer map command called class. For incoming there is a new
 * interface option "isdn incoming-voice data". This will allow
 * the isdn code to answer incoming voice calls.
 *
 * Revision 2.1  1995/07/02  01:45:54  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:05:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(ci_isdn_extend_here, no_alt);

/***************************************************************
 * isdn sending-complete
 * no isdn sending-complete
 *
 * Force a Sending complete in the outgoing Setup message. There are no
 * required parameters for this command, it either sets the flag true or
 * use the no command to disable.
 */
EOLS    (ci_isdn_sc_eol, isdnif_command, ISDN_SENDING_COMPLETE);

NOPREFIX(ci_isdn_sc_noprefix, ci_isdn_sc_eol, ci_isdn_sc_eol );

NVGENS  (ci_isdn_sc_nvgens, ci_isdn_sc_noprefix, isdnif_command,
         ISDN_SENDING_COMPLETE);

KEYWORD (ci_isdn_sending_complete, ci_isdn_sc_nvgens, ci_isdn_extend_here,
         "sending-complete",
         "Specify if Sending Complete included in outgoing SETUP message",
         PRIV_CONF);

/***************************************************************
 * isdn busy-cause busy|not-available|<cause value>
 */

/* These two definitions are taken from ../isdn/ccie.h.  We have to redefine
 * them here because we can't include ccie.h without generating conflicts
 * with macros.h.
*/
#define REQ_CHANNEL_NOT_AVAIL 44
#define USER_BUSY 17

EOLS    (ci_isdn_modem_busy_cause_eol, isdnif_command, ISDN_MODEM_BUSY_CAUSE);

NUMBER  (ci_isdn_modem_busy_cause_value, ci_isdn_modem_busy_cause_eol, 
                no_alt, OBJ(int,1), 1, 127, 
                "If no modems are available, specified cause code will be sent to the switch" );

KEYWORD_ID (ci_isdn_modem_busy_cause_not_avail, ci_isdn_modem_busy_cause_eol, 
                ci_isdn_modem_busy_cause_value, OBJ(int,1), 
	        REQ_CHANNEL_NOT_AVAIL, "not-available", 
	        "If no modems are available, CHANNEL NOT AVAILABLE will be sent to the switch ", 
                PRIV_CONF );

KEYWORD_ID (ci_isdn_modem_busy_cause_busy, ci_isdn_modem_busy_cause_eol, 
                ci_isdn_modem_busy_cause_not_avail, OBJ(int,1), USER_BUSY, "busy", 
	        "If no modems are available, USER BUSY will be sent to the switch ", 
                PRIV_CONF );

NOPREFIX(ci_isdn_modem_busy_cause_noprefix, ci_isdn_modem_busy_cause_busy, 
                ci_isdn_modem_busy_cause_eol );

NVGENS  (ci_isdn_modem_busy_cause_nvgens, ci_isdn_modem_busy_cause_noprefix, 
                isdnif_command, ISDN_MODEM_BUSY_CAUSE);

KEYWORD (ci_isdn_modem_busy_cause, ci_isdn_modem_busy_cause_nvgens, 
	        ci_isdn_sending_complete, 
	        "modem-busy-cause", 
		"Specify cause code to return in call rejection when no modems are available", 
	        PRIV_CONF );

/***************************************************************
 * isdn T200 <msecs>  
 * no isdn T200 <msecs>
 */
EOLS    (ci_isdn_t200_eol, isdnif_command, ISDN_T200_TIMER);

NOPREFIX(ci_isdn_t200_noprefix, ci_isdn_t200_eol, ci_isdn_t200_eol );

NUMBER  (ci_isdn_t200_val, ci_isdn_t200_noprefix, no_alt, OBJ(int,1), 
		400, 400000, "Timer T200 value in milliseconds" );

NVGENS  (ci_isdn_t200_nvgens, ci_isdn_t200_val, isdnif_command, 
		ISDN_T200_TIMER);

KEYWORD (ci_isdn_t200, ci_isdn_t200_nvgens, ci_isdn_modem_busy_cause, "T200", 
		"Specify Timer T200 in milliseconds", PRIV_CONF | PRIV_HIDDEN);
/***************************************************************
 * isdn N200 <number of retransmissions>  
 * no isdn N200 <number>
 */
EOLS    (ci_isdn_n200_eol, isdnif_command, ISDN_N200_RETRIES);

NOPREFIX(ci_isdn_n200_noprefix, ci_isdn_n200_eol, ci_isdn_n200_eol );

NUMBER  (ci_isdn_n200_val, ci_isdn_n200_noprefix, no_alt, OBJ(int,1), 
		1, 100, "Number of Retransmissions" );

NVGENS  (ci_isdn_n200_nvgens, ci_isdn_n200_val, isdnif_command, 
		ISDN_N200_RETRIES);

KEYWORD (ci_isdn_n200, ci_isdn_n200_nvgens, ci_isdn_t200, "N200", 
		"Specify the Number of Retransmissions", PRIV_CONF | PRIV_HIDDEN);
/***************************************************************
 * isdn calling-number <string>  
 * no isdn calling-number <string>
 *  <string> = <calling party number>
 * This command is only allowed for Australia switches
 * (basic-ts013), that is why it is a hidden command.
 */
EOLS    (ci_isdn_calling_string_eol, isdnif_command,
	ISDN_CALLING_NUMBER_STRING );

NOPREFIX(ci_isdn_calling_noprefix, ci_isdn_calling_string_eol,
	ci_isdn_calling_string_eol );

STRING  (ci_isdn_calling_string, ci_isdn_calling_noprefix,
	ci_isdn_calling_string_eol, OBJ(string,1), "Calling Number" );

NVGENS  (ci_isdn_calling_nvgens, ci_isdn_calling_string,
	isdnif_command, ISDN_CALLING_NUMBER_STRING );

KEYWORD (ci_isdn_calling, ci_isdn_calling_nvgens, ci_isdn_n200,
	"calling-number", "Specify Calling Party number ",
	PRIV_CONF | PRIV_HIDDEN);

/***************************************************************
 * isdn answer2 <string>  
 * no isdn answer2 <string>
 *  <string> = <called party number:subaddress>
 */
EOLS    (ci_isdn_answer2_string_eol, isdnif_command, ISDN_ANSWER2_STRING );

NOPREFIX(ci_isdn_answer2_noprefix, ci_isdn_answer2_string_eol,
	ci_isdn_answer2_string_eol );

STRING  (ci_isdn_answer2_string, ci_isdn_answer2_noprefix,
	ci_isdn_answer2_string_eol, OBJ(string,1), "answer2 Number" );

NVGENS  (ci_isdn_answer2_nvgens, ci_isdn_answer2_string,
	isdnif_command, ISDN_ANSWER2_STRING );

KEYWORD (ci_isdn_answer2, ci_isdn_answer2_nvgens, ci_isdn_calling,
	"answer2", "Specify Called Party number and subaddress ", PRIV_CONF );
/***************************************************************
 * isdn answer1 <string>  
 * no isdn answer1 <string>
 *  <string> = <called party number:subaddress>
 */
EOLS    (ci_isdn_answer1_string_eol, isdnif_command, ISDN_ANSWER1_STRING );

NOPREFIX(ci_isdn_answer1_noprefix, ci_isdn_answer1_string_eol,
	ci_isdn_answer1_string_eol );

STRING  (ci_isdn_answer1_string, ci_isdn_answer1_noprefix,
	ci_isdn_answer1_string_eol, OBJ(string,1), "answer1 Number" );

NVGENS  (ci_isdn_answer1_nvgens, ci_isdn_answer1_string,
	isdnif_command, ISDN_ANSWER1_STRING );

KEYWORD (ci_isdn_answer1, ci_isdn_answer1_nvgens, ci_isdn_answer2,
	"answer1", "Specify Called Party number and subaddress ", PRIV_CONF );
/***************************************************************
 * isdn incoming-voice modem [<56|64>]
 * no isdn incoming-voice modem
 */
EOLS    (ci_isdn_modem_eol, isdnif_command, ISDN_VOICE_MODEM );

KEYWORD_ID(ci_isdn_incoming_modem_56, ci_isdn_modem_eol,
                        ci_isdn_modem_eol, OBJ(int,1), 56, "56",
                        "B Channel Bandwidth of 56Kb/s", PRIV_CONF);

KEYWORD_ID(ci_isdn_incoming_modem_64, ci_isdn_modem_eol,
                        ci_isdn_incoming_modem_56, OBJ(int,1), 64, "64",
                        "B Channel Bandwidth of 64Kb/s", PRIV_CONF);

NOPREFIX(ci_isdn_modem_noprefix, ci_isdn_incoming_modem_64,
        ci_isdn_modem_eol );

NVGENS  (ci_isdn_modem_nvgens, ci_isdn_modem_noprefix,
        isdnif_command, ISDN_VOICE_MODEM );

KEYWORD (ci_isdn_incoming_modem, ci_isdn_modem_nvgens, no_alt,
        "modem", "Incoming voice calls will be handled as modems. ", PRIV_CONF );
/***************************************************************
 * isdn incoming-voice data  [<56|64>]
 * no isdn incoming-voice data
 */
EOLS    (ci_isdn_incoming_eol, isdnif_command, ISDN_VOICE_DATA );

KEYWORD_ID(ci_isdn_incoming_data_56, ci_isdn_incoming_eol,
                        ci_isdn_incoming_eol, OBJ(int,1), 56, "56",
                        "B Channel Bandwidth of 56Kb/s", PRIV_CONF);

KEYWORD_ID(ci_isdn_incoming_data_64, ci_isdn_incoming_eol,
                        ci_isdn_incoming_data_56, OBJ(int,1), 64, "64",
                        "B Channel Bandwidth of 64Kb/s", PRIV_CONF);

NOPREFIX(ci_isdn_incoming_noprefix, ci_isdn_incoming_data_64,
        ci_isdn_incoming_eol );

NVGENS  (ci_isdn_incoming_nvgens, ci_isdn_incoming_noprefix,
        isdnif_command, ISDN_VOICE_DATA );

KEYWORD (ci_isdn_incoming_data, ci_isdn_incoming_nvgens, ci_isdn_incoming_modem,
        "data", "Incoming voice calls will be handled as data. ", PRIV_CONF );

KEYWORD (ci_isdn_incoming_voice, ci_isdn_incoming_data, ci_isdn_answer1,
        "incoming-voice", "Specify options for incoming calls. ", PRIV_CONF );
/***************************************************************
 * isdn fast-rollover-delay <num>
 * no isdn fast-rollover-delay
 */
PARAMS_KEYONLY(ci_isdn_fast_rollover, ci_isdn_incoming_voice,
	       "fast-rollover-delay",
	       OBJ(int,1), ISDN_FASTROLLOVER_DELAY_MIN,
	       ISDN_FASTROLLOVER_DELAY_MAX,
	       isdnif_command, ISDN_FASTROLLOVER_DELAY,
	       "Delay between fastrollover dials",
	       "Fast rollover delay in seconds", PRIV_CONF);

/***************************************************************
 * isdn caller <string>
 * no isdn caller
 */
EOLS    (ci_isdn_caller_string_eol, isdnif_command, ISDN_CALLER_STRING );

NOPREFIX(ci_isdn_caller_noprefix, ci_isdn_caller_string_eol,
     ci_isdn_caller_string_eol );

STRING  (ci_isdn_caller_string, ci_isdn_caller_noprefix,
    ci_isdn_caller_string_eol, OBJ(string,1), "Caller String" );

NVGENS  (ci_isdn_caller_nvgens, ci_isdn_caller_string,
     isdnif_command, ISDN_CALLER_STRING );

KEYWORD (ci_isdn_caller, ci_isdn_caller_nvgens, ci_isdn_fast_rollover,
     "caller", "Specify incoming telephone number to be verified",
     PRIV_CONF );

/***************************************************************
 * isdn not-end-to-end [<56|64>]
 * no isdn not-end-to-end
 */
EOLS (ci_isdn_not_end_to_end_eol, isdnif_command, ISDN_NOT_END_TO_END);

KEYWORD_ID(ci_isdn_not_end_to_end_56, ci_isdn_not_end_to_end_eol,
			ci_isdn_not_end_to_end_eol, OBJ(int,1), 56, "56",
			"B Channel Bandwidth of 56Kb/s", PRIV_CONF);

KEYWORD_ID(ci_isdn_not_end_to_end_64, ci_isdn_not_end_to_end_eol,
			ci_isdn_not_end_to_end_56, OBJ(int,1), 64, "64",
			"B Channel Bandwidth of 64Kb/s", PRIV_CONF);

NOPREFIX(ci_isdn_not_end_to_end_noprefix, ci_isdn_not_end_to_end_64,
		ci_isdn_not_end_to_end_eol);

NVGENS (ci_isdn_not_end_to_end_nvgen, ci_isdn_not_end_to_end_noprefix,
		isdnif_command, ISDN_NOT_END_TO_END);

KEYWORD(ci_isdn_not_end_to_end, ci_isdn_not_end_to_end_nvgen, ci_isdn_caller,
		"not-end-to-end",
		"Specify speed when calls received are not isdn end to end",
		PRIV_CONF);

/***************************************************************
 * isdn spid2 <string>
 * no isdn spid2
 */
EOLS	(ci_isdn_spid2_string_eol, isdnif_command, ISDN_SPID2_STRING);

STRING	(ci_isdn_spid2_ldn_string, ci_isdn_spid2_string_eol,
	 ci_isdn_spid2_string_eol, OBJ(string,2), "local directory number" );

STRING	(ci_isdn_spid2_string, ci_isdn_spid2_ldn_string, no_alt,
	 OBJ(string,1), "spid2 string");

NOPREFIX(ci_isdn_spid2_noprefix, ci_isdn_spid2_string,
	 ci_isdn_spid2_string_eol);

NVGENS	(ci_isdn_spid2_nvgens, ci_isdn_spid2_noprefix,
	 isdnif_command, ISDN_SPID2_STRING);

KEYWORD (ci_isdn_spid2, ci_isdn_spid2_nvgens, ci_isdn_not_end_to_end,
	 "spid2", "Specify Service Profile IDentifier",
	  PRIV_CONF);

/***************************************************************
 * isdn spid1 <string>
 * no isdn spid1
 */
EOLS	(ci_isdn_spid_string_eol, isdnif_command, ISDN_SPID_STRING);

STRING	(ci_isdn_spid_ldn_string, ci_isdn_spid_string_eol,
	 ci_isdn_spid_string_eol, OBJ(string,2), "local directory number" );

STRING	(ci_isdn_spid_string, ci_isdn_spid_ldn_string, no_alt,
	 OBJ(string,1), "spid1 string" );

NOPREFIX(ci_isdn_spid_noprefix, ci_isdn_spid_string,
	 ci_isdn_spid_string_eol);

NVGENS	(ci_isdn_spid_nvgens, ci_isdn_spid_noprefix,
	 isdnif_command, ISDN_SPID_STRING);

KEYWORD (ci_isdn_spid, ci_isdn_spid_nvgens, ci_isdn_spid2,
	 "spid1", "Specify Service Profile IDentifier",
	  PRIV_CONF);

/***************************************************************
 * isdn twait_timer
 * no isdn twait_timer 
 */
EOLS	(ci_isdn_twait_eol, isdnif_command, ISDN_TWAIT_DISABLED);

KEYWORD (ci_isdn_twait, ci_isdn_twait_eol, ci_isdn_spid, "twait-disable",
         "Delay National ISDN BRI switchtype from activating interface on powerup",
         PRIV_CONF);

TEST_INT(ci_isdn_check, ci_isdn_twait, ci_isdn_not_end_to_end, NONE,
	 nbrinets);

KEYWORD	(ci_isdn, ci_isdn_check, no_alt, "isdn",
	  "ISDN Interface configuration commands", PRIV_CONF);

ASSERT (ci_isdn_test, ci_isdn, ALTERNATE,
	csb->interface->hwptr->type == IDBTYPE_BRI ||
	(csb->interface->hwptr->type == IDBTYPE_T1 && is_isdn(csb->interface->hwptr)) ||
	(csb->interface->hwptr->type == IDBTYPE_E1 && is_isdn(csb->interface->hwptr)) );

#undef	ALTERNATE
#define	ALTERNATE	ci_isdn_test
