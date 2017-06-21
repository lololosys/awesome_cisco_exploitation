/* $Id: globdef.h,v 1.1.4.1 1996/05/09 15:01:06 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/globdef.h,v $
 *------------------------------------------------------------------
 * globdef.h -- Global Definitions for diagnostics
 *
 * ?
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: globdef.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:06  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:04  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:28  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:15  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:54:00  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:34  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************/
/*                                                                            */
/* DIAGNOSTICS DRIVER                                                         */
/*                                                                            */
/* This file contains "Global Definitions" that are included as part of       */
/* Diagnostics Driver.                                                        */
/*                                                                            */
/* NOTE TO THE REVIEWER:                                                      */
/* --------------------                                                       */
/* You may find that Comments & Remarks are kept to the minimum extent, which */
/* eventually would be updated in due course of time.  The author believes in */
/* "Self-documentation" by way of meaningful names for procedures, variables  */
/* etc. and "Hierarchical programming" style which would work out better in   */
/* terms of time & productivity.  In the meantime, if you encounter any       */
/* problems in going thru the code or have any comments/suggestions, do let   */
/* the author know about it.                                                  */
/*                                                                            */
/******************************************************************************/

#define	parm_runcount		"RUNCOUNT"
#define	parm_maxerrorcount	"MAXERRORCOUNT"
#define	parm_timedelaycount	"TIMEDELAYCOUNT"
#define	parm_verbosity		"VERBOSITY"
#define	parm_allprompt		"ALLPROMPT"
#define	parm_npbusarbitration	"NPBUSARBITRATION"
#define	parm_fbbusarbitration	"FBBUSARBITRATION"
#define	parm_packetsizevariant	"PACKETSIZEVARIANT"
#define	parm_datapatternvariant	"DATAPATTERNVARIANT"
#define	parm_summaryformat	"SUMMARYFORMAT"

#define	result_diag_description	"Description"
#define	result_start_time_msec	"Start Time (in msec)"
#define	result_end_time_msec	"End Time (in msec)"
#define	result_elapsed_time_msec "Elapsed Time (in msec)"
#define	result_start_time	"Start Time"
#define	result_end_time		"End Time"
#define	result_elapsed_time	"Elapsed Time"
#define	result_total_errorcount	"Total Errorcount"

#define	value_environment	"/Power-up/Off-line/On-line/"
#define	value_env_powerup	"Power-up"
#define	value_env_offline	"Off-line"
#define	value_env_online	"On-line"

#define	value_flag		"/True/False/"
#define	value_flag_true_only	"/True/"
#define	value_flag_false_only	"/False/"
#define	value_flag_true		"True"
#define	value_flag_false	"False"

#define	value_access		"/Byte/Word/Long/All/"
#define	value_access_byte_only	"/Byte/"
#define	value_access_word_only	"/Word/"
#define	value_access_long_only	"/Long/"
#define	value_access_byte	"Byte"
#define	value_access_word	"Word"
#define	value_access_long	"Long"
#define	value_access_all	"All"
#define	value_access_word_long_only	"/Word/Long/All/"

#define	value_mode		"/Normal/Increment/Complement/Shift/Random/All/"
#define	value_mode_normal	"Normal"
#define	value_mode_increment	"Increment"
#define	value_mode_complement	"Complement"
#define	value_mode_shift	"Shift"
#define	value_mode_random	"Random"
#define	value_mode_all		"All"

#define	value_summaryformat_LIFO	"LIFO"
#define	value_summaryformat		"/FIFO/LIFO/OFF/"

#define	fmt_string	"%s"
#define	fmt_char	"%c"
#define	fmt_xInt32	"%lX"
#define	fmt_xInt16	"%X"
#define	fmt_xInt08	"%hX"
#define	fmt_uInt32	"%lu"
#define	fmt_uInt16	"%u"
#define	fmt_uInt08	"%hu"
#define	fmt_Int32	"%ld"
#define	fmt_Int16	"%d"

#define	res_fmt_string	"%10s"
#define	res_fmt_xInt32	"0x%08lX"
#define	res_fmt_xInt16	"    0x%04lX"
#define	res_fmt_xInt08	"      0x%02lX"
#define	res_fmt_uInt32	"  %8lu"
#define	res_fmt_Int32	"  %8ld"

