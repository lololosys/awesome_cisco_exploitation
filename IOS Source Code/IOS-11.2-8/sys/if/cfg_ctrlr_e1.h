/* $Id: cfg_ctrlr_e1.h,v 3.4.20.2 1996/06/16 21:12:41 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_ctrlr_e1.h,v $
 *------------------------------------------------------------------
 * C F G _ C T R L R _ E 1 . H
 * 
 * 20-June-1994, Bill May (from cfg_ctrlr_t1.h)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_ctrlr_e1.h,v $
 * Revision 3.4.20.2  1996/06/16  21:12:41  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.4.20.1  1996/06/03  23:08:52  wmay
 * CSCdi59341:  Can only define 30 channel-groups for E1 - allow that 31st
 * channel-group
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/06  02:24:10  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 * Put back some Keyword_Id macros
 *
 * Revision 3.3  1996/02/01  23:33:27  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  09:25:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:05  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/19  21:51:06  wmay
 * CSCdi36002:  CE1 layer 1 fails Australia Homologation - add Australia
 * option to framing controller E1 command.
 *
 * Revision 2.1  1995/06/07  22:02:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/* forward declarations */
pdecl(ci_e1_ts_num1);
pdecl(ci_e1_ts_num1w);
pdecl(ci_e1_pri_num1);
pdecl(ci_e1_pri_num1w);

/******************************************************************
 * pri-group [timeslots <range 1-31>] [speed <64,56,48>]
 *  OBJ(int,1) = timeslots (bit field)
 *  OBJ(int,2) = speed	(this is hidden from the user)
 *  OBJ(int,3) - (used as a temp variable)
 *  OBJ(int,4) - (used as a temp variable)
 */
EOLS	(ci_e1_pri_eol, e1ctrlr_command, E1_CTRLR_PRI_CHANNEL);
KEYWORD_ID(ci_e1_pri_speed_val3, ci_e1_pri_eol, no_alt,
	   OBJ(int,2), DSX1_SPEED_48,
	   "48", "48 Kb/s", PRIV_CONF);
KEYWORD_ID(ci_e1_pri_speed_val2, ci_e1_pri_eol, ci_e1_pri_speed_val3,
	   OBJ(int,2), DSX1_SPEED_56,
	   "56", "56 Kb/s", PRIV_CONF);
KEYWORD_ID(ci_e1_pri_speed_val, ci_e1_pri_eol, ci_e1_pri_speed_val2,
	   OBJ(int,2), DSX1_SPEED_64,
	   "64", "64 Kb/s", PRIV_CONF);
KEYWORD (ci_e1_pri_speed, ci_e1_pri_speed_val, ci_e1_pri_eol, "speed",
     "Specify the speed of the D-channel", PRIV_CONF|PRIV_HIDDEN);
CHARACTER(ci_e1_pri_num4, ci_e1_pri_num1w, ci_e1_pri_speed, ',');
EVAL    (ci_e1_pri_num3a, ci_e1_pri_num4, SETOBJ(int,1) =
     GETOBJ(int,1) | make_bit_field(GETOBJ(int,3), GETOBJ(int,4)));
GENERAL_NUMBER(ci_e1_pri_num3, ci_e1_pri_num3a, no_alt, OBJ(int,4),
           1, 31, "List of B-channel timeslots D is set for 16",
           NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
WHITESPACE(ci_e1_pri_num1w, ci_e1_pri_num1, ci_e1_pri_num1);
WHITESPACE(ci_e1_pri_num3w, ci_e1_pri_num3, ci_e1_pri_num3);
CHARACTER(ci_e1_pri_num2a, ci_e1_pri_num1w, ci_e1_pri_speed, ',');
CHARACTER(ci_e1_pri_num2, ci_e1_pri_num3w, ci_e1_pri_num2a, '-');
EVAL    (ci_e1_pri_num1a, ci_e1_pri_num2,
     SETOBJ(int,1) =
           GETOBJ(int,1) | make_bit_field(GETOBJ(int,3), GETOBJ(int,3)));
GENERAL_NUMBER(ci_e1_pri_num1, ci_e1_pri_num1a, no_alt, OBJ(int,3),
           1, 31, "List of timeslots which comprise the pri-group",
           NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
KEYWORD_ID(ci_e1_pri_timeslot, ci_e1_pri_num1, ci_e1_pri_speed,
	   OBJ(int,1), 0,
	   "timeslots", "List of timeslots in the pri-group", PRIV_CONF);
SET (ci_e1_pri_num0a, ci_e1_pri_timeslot, OBJ(int,2), DSX1_SPEED_64);
SET (ci_e1_pri_timeset, ci_e1_pri_num0a, OBJ(int,1), 0xFFFFFFFE);
NVGENS  (ci_e1_pri_chan_nvgen, ci_e1_pri_timeset, e1ctrlr_command,
     E1_CTRLR_PRI_CHANNEL);
KEYWORD (ci_e1_pri, ci_e1_pri_chan_nvgen, NONE, "pri-group", 
		"Configure the specified timeslots for PRI ", PRIV_CONF);

ASSERT    (ci_e1_pri_check, ci_e1_pri, NONE, !nbrinets);

/****************************************************************************
 * [no] channel-group <number> timeslots <range> [ speed { 64 | 56 | 48 } ]
 *
 * <range>	:: <range entry> | <range> ',' <range_entry>
 * <range_entry>:: <number> | <number> '-' <number>
 *
 *	OBJ(int,1) = channel number
 *	OBJ(int,2) = timeslots (bit field)
 *	OBJ(int,3) = speed
 *	OBJ(int,4) - (used as a temp variable)
 *	OBJ(int,5) - (used as a temp variable)
 */
EOLS	(ci_e1_ts_eol, e1ctrlr_command, E1_CTRLR_CHANNEL);
KEYWORD_ID(ci_e1_ts_speed_val8, ci_e1_ts_eol, no_alt,
	   OBJ(int,3), DSX1_SPEED_8,
	   "8", "8 Kb/s", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_e1_ts_speed_val7, ci_e1_ts_eol, ci_e1_ts_speed_val8,
	   OBJ(int,3), DSX1_SPEED_16,
	   "16", "16 Kb/s", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_e1_ts_speed_val6, ci_e1_ts_eol, ci_e1_ts_speed_val7,
	   OBJ(int,3), DSX1_SPEED_24,
	   "24", "24 Kb/s", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_e1_ts_speed_val5, ci_e1_ts_eol, ci_e1_ts_speed_val6,
	   OBJ(int,3), DSX1_SPEED_32,
	   "32", "32 Kb/s", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_e1_ts_speed_val4, ci_e1_ts_eol, ci_e1_ts_speed_val5,
	   OBJ(int,3), DSX1_SPEED_40,
	   "40", "40 Kb/s", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_e1_ts_speed_val3a, ci_e1_ts_eol, ci_e1_ts_speed_val4,
	   OBJ(int,3), DSX1_SPEED_48_I460,
	   "i460-48", "48 Kb/s for I.460", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_e1_ts_speed_val3, ci_e1_ts_eol, ci_e1_ts_speed_val3a,
	   OBJ(int,3), DSX1_SPEED_48,
	   "48", "48 Kb/s", PRIV_CONF);
KEYWORD_ID(ci_e1_ts_speed_val2, ci_e1_ts_eol, ci_e1_ts_speed_val3,
	   OBJ(int,3), DSX1_SPEED_56,
	   "56", "56 Kb/s", PRIV_CONF);
KEYWORD_ID(ci_e1_ts_speed_val, ci_e1_ts_eol, ci_e1_ts_speed_val2,
	   OBJ(int,3), DSX1_SPEED_64,
	   "64", "64 Kb/s", PRIV_CONF);
KEYWORD	(ci_e1_ts_speed, ci_e1_ts_speed_val, ci_e1_ts_eol,
	 "speed", "Specify the speed of the underlying DS0s", PRIV_CONF);
	 
CHARACTER(ci_e1_ts_num4, ci_e1_ts_num1w, ci_e1_ts_speed, ',');
EVAL	(ci_e1_ts_num3a, ci_e1_ts_num4, SETOBJ(int,2) =
	 GETOBJ(int,2) | make_bit_field(GETOBJ(int,4), GETOBJ(int,5)));
GENERAL_NUMBER(ci_e1_ts_num3, ci_e1_ts_num3a, no_alt, OBJ(int,5),
	       1, 31, "List of timeslots which comprise the channel",
	       NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
WHITESPACE(ci_e1_ts_num1w, ci_e1_ts_num1, ci_e1_ts_num1);
WHITESPACE(ci_e1_ts_num3w, ci_e1_ts_num3, ci_e1_ts_num3);
CHARACTER(ci_e1_ts_num2a, ci_e1_ts_num1w, ci_e1_ts_speed, ',');
CHARACTER(ci_e1_ts_num2, ci_e1_ts_num3w, ci_e1_ts_num2a, '-');
EVAL	(ci_e1_ts_num1a, ci_e1_ts_num2,
	 SETOBJ(int,2) =
	       GETOBJ(int,2) | make_bit_field(GETOBJ(int,4), GETOBJ(int,4)));
GENERAL_NUMBER(ci_e1_ts_num1, ci_e1_ts_num1a, no_alt, OBJ(int,4),
	       1, 31, "List of timeslots which comprise the channel",
	       NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
KEYWORD_ID(ci_e1_timeslot, ci_e1_ts_num1, no_alt,
	   OBJ(int,3), DSX1_SPEED_64,
	   "timeslots", "List of timeslots in the channel group", PRIV_CONF);
NUMBER	(ci_e1_channel_num, ci_e1_timeslot, no_alt, OBJ(int,1), 0,
	 E1_MAXCHAN, "Channel number");
NOPREFIX(ci_e1_channel_no, ci_e1_channel_num, ci_e1_ts_eol);
NVGENS	(ci_e1_channel_nvgen, ci_e1_channel_num, e1ctrlr_command,
	 E1_CTRLR_CHANNEL);
KEYWORD	(ci_e1_channel, ci_e1_channel_nvgen, ci_e1_pri_check, "channel-group",
	 "Specify the timeslots to channel-group mapping for an interface",
	 PRIV_CONF);


/*****************************************************************
 * [no] loop
 *
 */
EOLS	(ci_e1_loop_eol, e1ctrlr_command, E1_CTRLR_LOOP);
KEYWORD	(ci_e1_loop, ci_e1_loop_eol, ci_e1_channel, "loopback",
	 "Put the entire E1 line into loopback", PRIV_CONF);

/******************************************************************
 * linecode { ami | hdb3 }
 *	OBJ(int,1) = DSX1_LC_AMI | DSX1_LC_HDB3
 */
EOLS	(ci_e1_linecode_eol, e1ctrlr_command, E1_CTRLR_LINECODE);
KEYWORD_ID(ci_e1_linecode_hdb3, ci_e1_linecode_eol, no_alt, OBJ(int,1),
	   DSX1_LC_HDB3, "hdb3", "HDB3 encoding", PRIV_CONF);
KEYWORD_ID(ci_e1_linecode_ami, ci_e1_linecode_eol, ci_e1_linecode_hdb3,
	   OBJ(int,1), DSX1_LC_AMI, "ami", "AMI encoding", PRIV_CONF);
NOPREFIX(ci_e1_linecode_no, ci_e1_linecode_ami, ci_e1_linecode_eol);
NVGENS	(ci_e1_linecode_nvgen, ci_e1_linecode_no, e1ctrlr_command,
	 E1_CTRLR_LINECODE);
KEYWORD	(ci_e1_linecode, ci_e1_linecode_nvgen, ci_e1_loop, "linecode",
	 "Specify the line encoding method for a E1 link", PRIV_CONF);

/******************************************************************
 * clock source [line {primary | secondary | ..... | eighth} | internal]
 *      OBJ(int,1) = DSX1_CLK_INTERNAL | DSX1_CLK_EXTERNAL
 *      OBJ(int,2) = PRIMARY | SECONDARY | ... | EIGHTH
 */
EOLS    (ci_e1_clocking_eol, e1ctrlr_command, E1_CTRLR_CLOCKING);
KEYWORD_ID(ci_e1_clock_eighth, ci_e1_clocking_eol, ci_e1_clocking_eol, OBJ(int,2),
           DSX1_EIGHTH, "eighth", "Eighth Source", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_e1_clock_seventh, ci_e1_clocking_eol, ci_e1_clock_eighth,
            OBJ(int,2), DSX1_SEVENTH, "seventh", "Seventh Source",
            PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_e1_clock_sixth, ci_e1_clocking_eol, ci_e1_clock_seventh,
            OBJ(int,2), DSX1_SIXTH, "sixth", "Sixth Source",
            PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_e1_clock_fifth, ci_e1_clocking_eol, ci_e1_clock_sixth,
            OBJ(int,2), DSX1_FIFTH, "fifth", "Fifth Source",
            PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_e1_clock_fourth, ci_e1_clocking_eol, ci_e1_clock_fifth,
            OBJ(int,2), DSX1_FOURTH, "fourth", "Fourth Source",
            PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_e1_clock_third, ci_e1_clocking_eol, ci_e1_clock_fourth,
            OBJ(int,2), DSX1_THIRD, "third", "Third Source",
            PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_e1_clock_secondary, ci_e1_clocking_eol, ci_e1_clock_third,
            OBJ(int,2), DSX1_SECONDARY, "secondary", "Secondary Source",
            PRIV_CONF);
KEYWORD_ID(ci_e1_clock_primary, ci_e1_clocking_eol, ci_e1_clock_secondary,
            OBJ(int,2), DSX1_PRIMARY, "primary", "Primary Source", PRIV_CONF);
IFELSE (ci_e1_clock_primary_chk, ci_e1_clock_primary, ci_e1_clocking_eol,
                                              IS_FAMILY(FAMILY_BRASIL));
KEYWORD_ID(ci_e1_clocking_line, ci_e1_clock_primary_chk, no_alt,
           OBJ(int,1), DSX1_CLK_LINE, "line", "Recovered Clock",
           PRIV_CONF);
KEYWORD_ID(ci_e1_clocking_free, ci_e1_clocking_eol, ci_e1_clocking_line,
           OBJ(int,1), DSX1_CLK_FREE, "free-running", "Free Running Clock",
           PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_e1_clocking_int, ci_e1_clocking_eol, ci_e1_clocking_free,
           OBJ(int,1), DSX1_CLK_INTERNAL, "internal",
           "Internal Clock", PRIV_CONF);
NOPREFIX(ci_e1_clocking_no, ci_e1_clocking_int, ci_e1_clocking_eol);
NVGENS  (ci_e1_clocking_nvgen, ci_e1_clocking_no, e1ctrlr_command,
         E1_CTRLR_CLOCKING);
KEYWORD (ci_e1_clock_source, ci_e1_clocking_nvgen, no_alt,
         "source", "Specify the clock source for a DS1 link", PRIV_CONF);
KEYWORD (ci_e1_clocking, ci_e1_clock_source, ci_e1_linecode,
         "clock", "Specify the clock source for a DS1 link", PRIV_CONF);

/******************************************************************
 * framing { no-crc4 | crc4 } {australia}
 *	OBJ(int,1) = DSX1_FRM_E1 | DSX1_FRM_E1_CRC
 */
EOLS	(ci_e1_framing_eol, e1ctrlr_command, E1_CTRLR_FRAMING);
KEYWORD_ID(ci_e1_framing_aus, ci_e1_framing_eol, ci_e1_framing_eol,
	   OBJ(int,2), DSX1_HOMOL_AUSTRALIA, "australia",
	   "Australian Layer 1 Homologation", PRIV_CONF);
SET	(ci_e1_framing_check_homol, ci_e1_framing_aus,
	 OBJ(int,2), DSX1_HOMOL_NORMAL);
KEYWORD_ID(ci_e1_framing_e1crc, ci_e1_framing_check_homol, no_alt,
	   OBJ(int,1), DSX1_FRM_E1_CRC, "crc4", "E1 with CRC",
	   PRIV_CONF);
KEYWORD_ID(ci_e1_framing_e1, ci_e1_framing_check_homol, ci_e1_framing_e1crc,
	   OBJ(int,1), DSX1_FRM_E1, "no-crc4", "E1", PRIV_CONF);
NOPREFIX(ci_e1_framing_no, ci_e1_framing_e1, ci_e1_framing_eol);
NVGENS	(ci_e1_framing_nvgen, ci_e1_framing_no, e1ctrlr_command,
	 E1_CTRLR_FRAMING);
KEYWORD	(ci_e1_framing, ci_e1_framing_nvgen, ci_e1_clocking,
	   "framing", "Specify the type of Framing on a E1 link",
	   PRIV_CONF);

/******************************************************************
 * [no] shut
 */
EOLS	(ci_e1_shut_eol, e1ctrlr_command, E1_CTRLR_SHUT);
KEYWORD	(ci_e1_shut, ci_e1_shut_eol, ci_e1_framing, "shutdown",
	 "Shut down a E1 link (send Blue Alarm)", PRIV_CONF);

ASSERT (ci_e1_shut_test, ci_e1_shut, ALTERNATE,
	(csb->controller->pif_type == PIFT_E1));
#undef ALTERNATE
#define ALTERNATE ci_e1_shut_test

