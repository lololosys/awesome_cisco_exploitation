/* $Id: cfg_ctrlr_t1.h,v 3.4.20.2 1996/07/11 22:39:19 jliu Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_ctrlr_t1.h,v $
 *------------------------------------------------------------------
 * C F G _ C T R L R _ T 1 . H
 * 
 * 8-July-1993, Ronnie B. Kon
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_ctrlr_t1.h,v $
 * Revision 3.4.20.2  1996/07/11  22:39:19  jliu
 * CSCdi62357:  need to support AT&T and ANSI facility data link standard
 * Branch: California_branch
 *            o Added new fdl command for t1 controller in order to support
 *              CSU function
 *
 * Revision 3.4.20.1  1996/06/16  21:12:43  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.4  1996/02/06  02:24:12  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 * Put back some Keyword_Id macros
 *
 * Revision 3.3  1996/02/01  23:33:28  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  09:25:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:06  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:02:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* forward declarations */
pdecl(ci_t1_ts_num1);
pdecl(ci_t1_ts_num1w);
pdecl(ci_t1_pri_num1);
pdecl(ci_t1_pri_num1w);
pdecl(ci_t1_cas_num1);
pdecl(ci_t1_cas_num1w);

/******************************************************************
 * fdl { att | ansi }
 *      OBJ(int,1) = DSX1_FDL_ATT | DSX1_FDL_ANSI
 */
EOLS    (ci_t1_fdl_eol, t1ctrlr_command, T1_CTRLR_FDL);
KEYWORD_ID(ci_t1_fdl_ansi, ci_t1_fdl_eol, no_alt, OBJ(int,1),
           DSX1_FDL_ANSI, "ansi", "ANSI T1.403", PRIV_CONF);
KEYWORD_ID(ci_t1_fdl_att, ci_t1_fdl_eol, ci_t1_fdl_ansi,
           OBJ(int,1), DSX1_FDL_ATT, "att", "AT&T TR54016", PRIV_CONF);
NOPREFIX(ci_t1_fdl_no, ci_t1_fdl_att, ci_t1_fdl_eol);
NVGENS  (ci_t1_fdl_nvgen, ci_t1_fdl_no, t1ctrlr_command,
         T1_CTRLR_FDL);
KEYWORD (ci_t1_fdl, ci_t1_fdl_nvgen, NONE, "fdl",
         "Specify the FDL standard for a DS1 data link", PRIV_CONF);

IFELSE (ci_t1_fdl_chk, ci_t1_fdl, NONE, IS_FAMILY(FAMILY_BRASIL));

/******************************************************************
 * [no] cas-group <number> timeslots <range 1-24> [type <E&M,>]
 * <range>	:: <range entry> | <range> ',' <range_entry>
 * <range_entry>:: <number> | <number> '-' <number>
 *
 *	OBJ(int,1) = channel number
 *	OBJ(int,2) = timeslots (bit field)
 *	OBJ(int,3) = signal type
 *	OBJ(int,4) - (used as a temp variable)
 *	OBJ(int,5) - (used as a temp variable)
 */
EOLS	(ci_t1_cas_eol, t1ctrlr_command, T1_CTRLR_CAS_CHANNEL);
KEYWORD_ID(ci_t1_cas_type_val, ci_t1_cas_eol, no_alt,
	   OBJ(int,3), DSX1_SIGTYPE_EM,
	   "E&M", "E & M Signaling", PRIV_CONF);
KEYWORD (ci_t1_cas_type, ci_t1_cas_type_val, ci_t1_cas_eol, "type",
     "Specify the type of signaling", PRIV_CONF|PRIV_HIDDEN);
CHARACTER(ci_t1_cas_num4, ci_t1_cas_num1w, ci_t1_cas_type, ',');
EVAL    (ci_t1_cas_num3a, ci_t1_cas_num4, SETOBJ(int,2) =
     GETOBJ(int,2) | make_bit_field(GETOBJ(int,4), GETOBJ(int,5)));
GENERAL_NUMBER(ci_t1_cas_num3, ci_t1_cas_num3a, no_alt, OBJ(int,5),
           1, 24, "List of T1 timeslots",
           NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
WHITESPACE(ci_t1_cas_num1w, ci_t1_cas_num1, ci_t1_cas_num1);
WHITESPACE(ci_t1_cas_num3w, ci_t1_cas_num3, ci_t1_cas_num3);
CHARACTER(ci_t1_cas_num2a, ci_t1_cas_num1w, ci_t1_cas_type, ',');
CHARACTER(ci_t1_cas_num2, ci_t1_cas_num3w, ci_t1_cas_num2a, '-');
EVAL    (ci_t1_cas_num1a, ci_t1_cas_num2,
     SETOBJ(int,2) =
           GETOBJ(int,2) | make_bit_field(GETOBJ(int,4), GETOBJ(int,4)));
GENERAL_NUMBER(ci_t1_cas_num1, ci_t1_cas_num1a, no_alt, OBJ(int,4),
           1, 24, "List of timeslots which comprise the cas-group",
           NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
KEYWORD_ID(ci_t1_cas_timeslot, ci_t1_cas_num1, ci_t1_cas_type,
	   OBJ(int,2), 0,
	   "timeslots", "List of timeslots in the cas-group", PRIV_CONF);
SET (ci_t1_cas_num0b, ci_t1_cas_timeslot, OBJ(int,3), DSX1_SIGTYPE_EM);
SET (ci_t1_cas_num0a, ci_t1_cas_num0b, OBJ(int,2), 0xFFFFFF00);
NUMBER	(ci_t1_cas_channel_num, ci_t1_cas_num0a, no_alt, OBJ(int,1), 0, 23,
	 "Channel number");
NOPREFIX(ci_t1_cas_channel_no, ci_t1_cas_channel_num, ci_t1_cas_eol);
NVGENS  (ci_t1_cas_chan_nvgen, ci_t1_cas_channel_num, t1ctrlr_command,
     T1_CTRLR_CAS_CHANNEL);
KEYWORD (ci_t1_cas, ci_t1_cas_chan_nvgen, ci_t1_fdl_chk, "cas-group", 
	"Configure the specified timeslots for CAS(Channel Associate Signals)", 
	PRIV_CONF);

IFELSE (ci_t1_cas_ifelse, ci_t1_cas, ci_t1_fdl_chk, dsx1_cas_supp);

/******************************************************************
 * pri-group [timeslots <range 1-23>] [speed <64,56,48>]
 *  OBJ(int,1) = timeslots (bit field)
 *  OBJ(int,2) = speed	(this is hidden from the user)
 *  OBJ(int,3) - (used as a temp variable)
 *  OBJ(int,4) - (used as a temp variable)
 */
EOLS	(ci_t1_pri_eol, t1ctrlr_command, T1_CTRLR_PRI_CHANNEL);
KEYWORD_ID(ci_t1_pri_speed_val3, ci_t1_pri_eol, no_alt,
	   OBJ(int,2), DSX1_SPEED_48,
	   "48", "48 Kb/s", PRIV_CONF);
KEYWORD_ID(ci_t1_pri_speed_val2, ci_t1_pri_eol, ci_t1_pri_speed_val3,
	   OBJ(int,2), DSX1_SPEED_56,
	   "56", "56 Kb/s", PRIV_CONF);
KEYWORD_ID(ci_t1_pri_speed_val, ci_t1_pri_eol, ci_t1_pri_speed_val2,
	   OBJ(int,2), DSX1_SPEED_64,
	   "64", "64 Kb/s", PRIV_CONF);
KEYWORD (ci_t1_pri_speed, ci_t1_pri_speed_val, ci_t1_pri_eol, "speed",
     "Specify the speed of the D-channel", PRIV_CONF|PRIV_HIDDEN);
CHARACTER(ci_t1_pri_num4, ci_t1_pri_num1w, ci_t1_pri_speed, ',');
EVAL    (ci_t1_pri_num3a, ci_t1_pri_num4, SETOBJ(int,1) =
     GETOBJ(int,1) | make_bit_field(GETOBJ(int,3), GETOBJ(int,4)));
GENERAL_NUMBER(ci_t1_pri_num3, ci_t1_pri_num3a, no_alt, OBJ(int,4),
           1, 24, "List of B-channel timeslots D is set for 24",
           NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
WHITESPACE(ci_t1_pri_num1w, ci_t1_pri_num1, ci_t1_pri_num1);
WHITESPACE(ci_t1_pri_num3w, ci_t1_pri_num3, ci_t1_pri_num3);
CHARACTER(ci_t1_pri_num2a, ci_t1_pri_num1w, ci_t1_pri_speed, ',');
CHARACTER(ci_t1_pri_num2, ci_t1_pri_num3w, ci_t1_pri_num2a, '-');
EVAL    (ci_t1_pri_num1a, ci_t1_pri_num2,
     SETOBJ(int,1) =
           GETOBJ(int,1) | make_bit_field(GETOBJ(int,3), GETOBJ(int,3)));
GENERAL_NUMBER(ci_t1_pri_num1, ci_t1_pri_num1a, no_alt, OBJ(int,3),
           1, 24, "List of timeslots which comprise the pri-group",
           NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
KEYWORD_ID(ci_t1_pri_timeslot, ci_t1_pri_num1, ci_t1_pri_speed,
	   OBJ(int,1), 0,
	   "timeslots", "List of timeslots in the pri-group", PRIV_CONF);
SET (ci_t1_pri_num0a, ci_t1_pri_timeslot, OBJ(int,2), DSX1_SPEED_64);
SET (ci_t1_pri_timeset, ci_t1_pri_num0a, OBJ(int,1), 0xFFFFFF00);
NVGENS  (ci_t1_pri_chan_nvgen, ci_t1_pri_timeset, t1ctrlr_command,
     T1_CTRLR_PRI_CHANNEL);
KEYWORD (ci_t1_pri, ci_t1_pri_chan_nvgen, NONE, "pri-group", 
		"Configure the specified timeslots for PRI ", PRIV_CONF);

ASSERT    (ci_t1_pri_check, ci_t1_pri, ci_t1_cas_ifelse, !nbrinets);


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
EOLS	(ci_t1_ts_eol, t1ctrlr_command, T1_CTRLR_CHANNEL);
KEYWORD_ID(ci_t1_ts_speed_val8, ci_t1_ts_eol, no_alt,
	   OBJ(int,3), DSX1_SPEED_8,
	   "8", "8 Kb/s", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_t1_ts_speed_val7, ci_t1_ts_eol, ci_t1_ts_speed_val8,
	   OBJ(int,3), DSX1_SPEED_16,
	   "16", "16 Kb/s", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_t1_ts_speed_val6, ci_t1_ts_eol, ci_t1_ts_speed_val7,
	   OBJ(int,3), DSX1_SPEED_24,
	   "24", "24 Kb/s", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_t1_ts_speed_val5, ci_t1_ts_eol, ci_t1_ts_speed_val6,
	   OBJ(int,3), DSX1_SPEED_32,
	   "32", "32 Kb/s", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_t1_ts_speed_val4, ci_t1_ts_eol, ci_t1_ts_speed_val5,
	   OBJ(int,3), DSX1_SPEED_40,
	   "40", "40 Kb/s", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_t1_ts_speed_val3a, ci_t1_ts_eol, ci_t1_ts_speed_val4,
	   OBJ(int,3), DSX1_SPEED_48_I460,
	   "i460-48", "48 Kb/s for I.460", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_t1_ts_speed_val3, ci_t1_ts_eol, ci_t1_ts_speed_val3a,
	   OBJ(int,3), DSX1_SPEED_48,
	   "48", "48 Kb/s", PRIV_CONF);
KEYWORD_ID(ci_t1_ts_speed_val2, ci_t1_ts_eol, ci_t1_ts_speed_val3,
	   OBJ(int,3), DSX1_SPEED_56,
	   "56", "56 Kb/s", PRIV_CONF);
KEYWORD_ID(ci_t1_ts_speed_val, ci_t1_ts_eol, ci_t1_ts_speed_val2,
	   OBJ(int,3), DSX1_SPEED_64,
	   "64", "64 Kb/s", PRIV_CONF);
KEYWORD	(ci_t1_ts_speed, ci_t1_ts_speed_val, ci_t1_ts_eol, "speed",
	 "Specify the speed of the underlying DS0s", PRIV_CONF);
	 
CHARACTER(ci_t1_ts_num4, ci_t1_ts_num1w, ci_t1_ts_speed, ',');
EVAL	(ci_t1_ts_num3a, ci_t1_ts_num4, SETOBJ(int,2) =
	 GETOBJ(int,2) | make_bit_field(GETOBJ(int,4), GETOBJ(int,5)));
GENERAL_NUMBER(ci_t1_ts_num3, ci_t1_ts_num3a, no_alt, OBJ(int,5),
	       1, 24, "List of timeslots which comprise the channel",
	       NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
WHITESPACE(ci_t1_ts_num1w, ci_t1_ts_num1, ci_t1_ts_num1);
WHITESPACE(ci_t1_ts_num3w, ci_t1_ts_num3, ci_t1_ts_num3);
CHARACTER(ci_t1_ts_num2a, ci_t1_ts_num1w, ci_t1_ts_speed, ',');
CHARACTER(ci_t1_ts_num2, ci_t1_ts_num3w, ci_t1_ts_num2a, '-');
EVAL	(ci_t1_ts_num1a, ci_t1_ts_num2,
	 SETOBJ(int,2) =
	       GETOBJ(int,2) | make_bit_field(GETOBJ(int,4), GETOBJ(int,4)));
GENERAL_NUMBER(ci_t1_ts_num1, ci_t1_ts_num1a, no_alt, OBJ(int,4),
	       1, 24, "List of timeslots which comprise the channel",
	       NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
KEYWORD_ID(ci_t1_timeslot, ci_t1_ts_num1, no_alt,
	   OBJ(int,3), DSX1_SPEED_56,
	   "timeslots", "List of timeslots in the channel group", PRIV_CONF);
NUMBER	(ci_t1_channel_num, ci_t1_timeslot, no_alt, OBJ(int,1), 0, 23,
	 "Channel number");
NOPREFIX(ci_t1_channel_no, ci_t1_channel_num, ci_t1_ts_eol);
NVGENS	(ci_t1_channel_nvgen, ci_t1_channel_num, t1ctrlr_command,
	 T1_CTRLR_CHANNEL);
KEYWORD	(ci_t1_channel, ci_t1_channel_nvgen, ci_t1_pri_check, "channel-group",
	 "Specify the timeslots to channel-group mapping for an interface",
	 PRIV_CONF);

/*****************************************************************
 * [no] compand a-law
 *
 */
EOLS	(ci_t1_compand_eols, t1ctrlr_command, T1_CTRLR_TONE_A_LAW);
KEYWORD(ci_t1_compand_type, ci_t1_compand_eols, no_alt,
	   "a-law", "Compand a-law", PRIV_CONF | PRIV_HIDDEN);
NOPREFIX(ci_t1_compand_no, ci_t1_compand_type, ci_t1_compand_eols);
NVGENS	(ci_t1_compand_nvgen, ci_t1_compand_no, t1ctrlr_command, T1_CTRLR_TONE_A_LAW);
KEYWORD	(ci_t1_compand, ci_t1_compand_nvgen, ci_t1_channel, "compand",
	 "Forcing T1 digit collection to a-law", PRIV_CONF| PRIV_HIDDEN);
IFELSE (ci_t1_compand_chk, ci_t1_compand, ci_t1_channel,
                                     IS_FAMILY(FAMILY_BRASIL));

/*****************************************************************
 * [no] loop [ local | remote ]
 *
 *	OBJ(int,1) = DSX1_LOOP_NONE | DSX1_LOOP_LOCAL | DSX1_LOOP_REMOTE
 */
EOLS	(ci_t1_loop_eols, t1ctrlr_command, T1_CTRLR_LOOP);
SET	(ci_t1_loop_default, ci_t1_loop_eols, OBJ(int,1), DSX1_LOOP_LOCAL);
KEYWORD_ID(ci_t1_loop_remote, ci_t1_loop_eols, ci_t1_loop_default,
	   OBJ(int,1), DSX1_LOOP_REMOTE,
	   "remote", "Remote loopback", PRIV_CONF);
KEYWORD_ID(ci_t1_loop_type, ci_t1_loop_eols, ci_t1_loop_remote,
	   OBJ(int,1), DSX1_LOOP_LOCAL,
	   "local", "Local loopback", PRIV_CONF);
SET	(ci_t1_loop_noloop, ci_t1_loop_eols, OBJ(int,1), DSX1_LOOP_NONE);
NOPREFIX(ci_t1_loop_no, ci_t1_loop_type, ci_t1_loop_noloop);
NVGENS	(ci_t1_loop_nvgen, ci_t1_loop_no, t1ctrlr_command, T1_CTRLR_LOOP);
KEYWORD	(ci_t1_loop, ci_t1_loop_nvgen, ci_t1_compand_chk, "loopback",
	 "Put the entire T1 line into loopback", PRIV_CONF);

/******************************************************************
 * cablelength {long [gian36 | gain 26] [0db | -7.5db | -15db | -22.5db] | short}
 *      OBJ(int,1) = DSX1_LONG | DSX1_SHORT
 *      OBJ(int,2) = DSX1_GAIN36 | DSX1_GAIN26
 *      OBJ(int,3) = DSX1_0DB | DSX1_75DB | DSX1_15DB | DSX1_225DB
 */
EOLS    (ci_t1_cl_eols, t1ctrlr_command, T1_CTRLR_CABLELENGTH);
SET     (ci_t1_len_0db, ci_t1_cl_eols, OBJ(int,3), DSX1_0DB);
SET     (ci_t1_len_75db, ci_t1_cl_eols, OBJ(int,3), DSX1_75DB);
SET     (ci_t1_len_15db, ci_t1_cl_eols, OBJ(int,3), DSX1_15DB);
SET     (ci_t1_len_225db, ci_t1_cl_eols, OBJ(int,3), DSX1_225DB);
KEYWORD (ci_t1_cl_len_val225, ci_t1_len_225db, no_alt, "-22.5db",
         "-22.5 db pulse", PRIV_CONF);
KEYWORD (ci_t1_cl_len_val15, ci_t1_len_15db, ci_t1_cl_len_val225, "-15db",
         "-15 db pulse", PRIV_CONF);
KEYWORD (ci_t1_cl_len_val75, ci_t1_len_75db, ci_t1_cl_len_val15, "-7.5db",
         "-7.5 db pulse", PRIV_CONF);
KEYWORD (ci_t1_cl_len_val0, ci_t1_len_0db, ci_t1_cl_len_val75, "0db",
         "0 db pulse", PRIV_CONF);
KEYWORD_ID(ci_t1_cl_gain26, ci_t1_cl_len_val0, no_alt, OBJ(int,2),
           DSX1_GAIN26, "gain26", "26db Gain", PRIV_CONF);
KEYWORD_ID(ci_t1_cl_gain36, ci_t1_cl_len_val0, ci_t1_cl_gain26, OBJ(int,2),
           DSX1_GAIN36, "gain36", "36db Gain", PRIV_CONF);
KEYWORD_ID(ci_t1_cl_long, ci_t1_cl_gain36, no_alt, OBJ(int,1),
           DSX1_LONG, "long", "long cable", PRIV_CONF);
KEYWORD_ID(ci_t1_cl_short, ci_t1_cl_eols, ci_t1_cl_long, OBJ(int,1),
           DSX1_SHORT, "short", "short cable", PRIV_CONF);
NOPREFIX(ci_t1_cl_no, ci_t1_cl_long, ci_t1_cl_eols);
NVGENS  (ci_t1_cl_nvgen, ci_t1_cl_no, t1ctrlr_command, T1_CTRLR_CABLELENGTH);
KEYWORD (ci_t1_cablelength, ci_t1_cl_nvgen, ci_t1_loop, "cablelength",
         "Specify the cable length for a DS1 link", PRIV_CONF);

IFELSE (ci_t1_cablelength_chk, ci_t1_cablelength, ci_t1_loop,
                                     IS_FAMILY(FAMILY_BRASIL));

/******************************************************************
 * linecode { ami | b8zs }
 *	OBJ(int,1) = DSX1_LC_AMI | DSX1_LC_B8ZS
 */
EOLS	(ci_t1_linecode_eol, t1ctrlr_command, T1_CTRLR_LINECODE);
KEYWORD_ID(ci_t1_linecode_b8zs, ci_t1_linecode_eol, no_alt, OBJ(int,1),
	   DSX1_LC_B8ZS, "b8zs", "B8ZS encoding", PRIV_CONF);
KEYWORD_ID(ci_t1_linecode_ami, ci_t1_linecode_eol, ci_t1_linecode_b8zs,
	   OBJ(int,1), DSX1_LC_AMI, "ami", "AMI encoding", PRIV_CONF);
NOPREFIX(ci_t1_linecode_no, ci_t1_linecode_ami, ci_t1_linecode_eol);
NVGENS	(ci_t1_linecode_nvgen, ci_t1_linecode_no, t1ctrlr_command,
	 T1_CTRLR_LINECODE);
KEYWORD	(ci_t1_linecode, ci_t1_linecode_nvgen, ci_t1_cablelength_chk, "linecode",
	 "Specify the line encoding method for a DS1 link", PRIV_CONF);
	
/******************************************************************
 * clock source [line {primary | secondary | ..... | eighth} | internal]
 *      OBJ(int,1) = DSX1_CLK_INTERNAL | DSX1_CLK_EXTERNAL
 *      OBJ(int,2) = PRIMARY | SECONDARY | ... | EIGHTH
 */
EOLS    (ci_t1_clocking_eol, t1ctrlr_command, T1_CTRLR_CLOCKING);
KEYWORD_ID(ci_t1_clock_eighth, ci_t1_clocking_eol, ci_t1_clocking_eol, OBJ(int,2),
           DSX1_EIGHTH, "eighth", "Eighth Source", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_t1_clock_seventh, ci_t1_clocking_eol, ci_t1_clock_eighth,
            OBJ(int,2), DSX1_SEVENTH, "seventh", "Seventh Source",
            PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_t1_clock_sixth, ci_t1_clocking_eol, ci_t1_clock_seventh,
            OBJ(int,2), DSX1_SIXTH, "sixth", "Sixth Source",
            PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_t1_clock_fifth, ci_t1_clocking_eol, ci_t1_clock_sixth,
            OBJ(int,2), DSX1_FIFTH, "fifth", "Fifth Source",
            PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_t1_clock_fourth, ci_t1_clocking_eol, ci_t1_clock_fifth,
            OBJ(int,2), DSX1_FOURTH, "fourth", "Fourth Source",
            PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_t1_clock_third, ci_t1_clocking_eol, ci_t1_clock_fourth,
            OBJ(int,2), DSX1_THIRD, "third", "Third Source",
            PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_t1_clock_secondary, ci_t1_clocking_eol, ci_t1_clock_third,
            OBJ(int,2), DSX1_SECONDARY, "secondary", "Secondary Source",
            PRIV_CONF);
KEYWORD_ID(ci_t1_clock_primary, ci_t1_clocking_eol, ci_t1_clock_secondary,
            OBJ(int,2), DSX1_PRIMARY, "primary", "Primary Source", PRIV_CONF);
IFELSE (ci_t1_clock_primary_chk, ci_t1_clock_primary, ci_t1_clocking_eol,
                                              IS_FAMILY(FAMILY_BRASIL));
KEYWORD_ID(ci_t1_clocking_line, ci_t1_clock_primary_chk, no_alt,
           OBJ(int,1), DSX1_CLK_LINE, "line", "Recovered Clock",
           PRIV_CONF);
KEYWORD_ID(ci_t1_clocking_free, ci_t1_clocking_eol, ci_t1_clocking_line,
           OBJ(int,1), DSX1_CLK_FREE, "free-running", "Free Running Clock",
           PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_t1_clocking_int, ci_t1_clocking_eol, ci_t1_clocking_free,
           OBJ(int,1), DSX1_CLK_INTERNAL, "internal",
           "Internal Clock", PRIV_CONF);
NOPREFIX(ci_t1_clocking_no, ci_t1_clocking_int, ci_t1_clocking_eol);
NVGENS  (ci_t1_clocking_nvgen, ci_t1_clocking_no, t1ctrlr_command,
         T1_CTRLR_CLOCKING);
KEYWORD (ci_t1_clock_source, ci_t1_clocking_nvgen, no_alt,
         "source", "Specify the clock source for a DS1 link", PRIV_CONF);
KEYWORD (ci_t1_clocking, ci_t1_clock_source, ci_t1_linecode,
         "clock", "Specify the clock source for a DS1 link", PRIV_CONF);

/******************************************************************
 * framing { esf | sf }
 *	OBJ(int,1) = DSX1_FRM_ESF | DSX1_FRM_SF
 */
EOLS	(ci_t1_framing_eol, t1ctrlr_command, T1_CTRLR_FRAMING);
KEYWORD_ID(ci_t1_framing_sf, ci_t1_framing_eol, no_alt,
	   OBJ(int,1), DSX1_FRM_SF, "sf", "Superframe", PRIV_CONF);
KEYWORD_ID(ci_t1_framing_esf, ci_t1_framing_eol, ci_t1_framing_sf,
	   OBJ(int,1), DSX1_FRM_ESF, "esf", "Extended Superframe",
	   PRIV_CONF);
NOPREFIX(ci_t1_framing_no, ci_t1_framing_esf, ci_t1_framing_eol);
NVGENS	(ci_t1_framing_nvgen, ci_t1_framing_no, t1ctrlr_command,
	 T1_CTRLR_FRAMING);
KEYWORD	(ci_t1_framing, ci_t1_framing_nvgen, ci_t1_clocking,
	   "framing", "Specify the type of Framing on a DS1 link",
	   PRIV_CONF);

/******************************************************************
 * [no] shut
 */
EOLS	(ci_t1_shut_eol, t1ctrlr_command, T1_CTRLR_SHUT);
KEYWORD	(ci_t1_shut, ci_t1_shut_eol, ci_t1_framing, "shutdown",
	 "Shut down a DS1 link (send Blue Alarm)", PRIV_CONF);
ASSERT (ci_t1_shut_test,ci_t1_shut, ALTERNATE,
	(csb->controller->pif_type == PIFT_T1));

#undef ALTERNATE
#define ALTERNATE ci_t1_shut_test

