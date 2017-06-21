/* $Id: lpbkdiag.c,v 1.1.4.1 1996/05/09 15:01:10 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/lpbkdiag.c,v $
 *------------------------------------------------------------------
 * lpbkdiag.c -- ATM Line Card Loopback Diagnostics Test Suites
 *
 * July 1995, rbau
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lpbkdiag.c,v $
 * Revision 1.1.4.1  1996/05/09  15:01:10  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:09  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:32  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:19  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:54:04  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:37  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.2.4.1  1995/11/16  00:05:57  rbau
 * merge mainline code changes in 1.4 branch
 *
 * Revision 1.3  1995/11/08  00:20:43  rbau
 * added support for DVT loopback test. complete the cvs header
 *
 * Revision 1.2  1995/08/24  23:25:56  rbau
 * added wrapper functions of loopback tests.
 *
 * Revision 1.1  1995/07/21  01:40:16  rbau
 * loopback test suite for off-line diag
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include	"types.h"
#include	"constant.h"
#include	"struct.h"
#include	"globdef.h"
#include	"localdef.h"
#include	"hw.h"
#include	"errcode.h"

#include	"diag.h"

extern	uInt32	sys_fail_address, sys_data_expected, sys_data_read;
extern	Int32		sys_error_code;
extern	char		test_in_progress_msg[], cli_print_buf[], sys_specific_info[];
extern	char		*diag_test_id;
extern	uInt32	diag_current_runcount, diag_verbosity, diag_runcount;
extern	char		diag_packetsizevariant[], diag_datapatternvariant[];

/*--- Loopback Diag Section Description ---*/
char		loopback_description[] = 
		"ALC Loopback Diagnostics Section";

extern	char	loop1_description[];
extern	char	loop2_description[];
extern	char	loop3_description[];
extern	char	loop4_description[];

char     lpbk_setup_description[]="loopback test setup";
char     dvt_description[]="DVT Loopback test";

/* 
 * Loopback Diag Section result variables 
 */
uInt32	loopback_runcount = 1;
uInt32	loopback_maxerrorcount = 0;
uInt32	loopback_timedelaycount = 0;
char     loopback_resetflag[6] = "False";

/*
 * loop1 results variables
 */
extern	char     loop1_time_of_run[];
extern	Int32    loop1_error_code;
extern	uInt32   loop1_run_count;
extern	uInt32   loop1_test_fail_count;
extern	uInt32   loop1_error_count;
uInt32   loop1_fail_address;
uInt32   loop1_data_expected;
uInt32   loop1_data_read;


/*
 * loop2 results variables
 */
extern	char     loop2_time_of_run[];
extern	Int32    loop2_error_code;
extern	uInt32   loop2_run_count;
extern	uInt32   loop2_test_fail_count;
extern	uInt32   loop2_error_count;
uInt32   loop2_fail_address;
uInt32   loop2_data_expected;
uInt32   loop2_data_read;

/*
 * loop3 results variables
 */
extern	char     loop3_time_of_run[];
extern	Int32    loop3_error_code;
extern	uInt32   loop3_run_count;
extern	uInt32   loop3_test_fail_count;
extern	uInt32   loop3_error_count;
uInt32   loop3_fail_address;
uInt32   loop3_data_expected;
uInt32   loop3_data_read;

/*
 * loop4 results variables
 */
extern	char     loop4_time_of_run[];
extern	Int32    loop4_error_code;
extern	uInt32	loop4_run_count;
extern	uInt32   loop4_test_fail_count;
extern	uInt32   loop4_error_count;
uInt32   loop4_fail_address;
uInt32   loop4_data_expected;
uInt32   loop4_data_read;

/*
 * DVT Loopback test results variables
 */
char     dvt_time_of_run[18];
Int32    dvt_error_code;
uInt32   dvt_run_count;
uInt32   dvt_test_fail_count;
uInt32   dvt_error_count;
uInt32   dvt_fail_address;
uInt32   dvt_data_expected;
uInt32   dvt_data_read;


/*--- Loopback Diag Parameters ---*/

/*
 * loopback diag section parameter
 */
struct PARAMETER_TABLE  loopback_parameter[] =
{
	{ (char *) &loopback_runcount, parm_runcount, fmt_uInt32,
		1, 1, 99999999, NULL, 0 },
	{ (char *) &loopback_maxerrorcount, parm_maxerrorcount, fmt_uInt32,
		0, 0, 99999999, NULL, 0 },
	{ (char *) &loopback_timedelaycount, parm_timedelaycount, fmt_uInt32,
		0, 0, 99999999, NULL, 0 },
	{ (char *)  loopback_resetflag, parm_resetflag, fmt_string,
		(uInt32) value_flag_false, 0, 0, value_flag, 0 },
	{ NULL, NULL, NULL, 0, 0, 0, NULL, 0 }
};

/*
 * loop1_parameter
 */
struct PARAMETER_TABLE	loop1_parameter[] =
{
	{ NULL, NULL, NULL, 0, 0, 0, NULL, 0 }
};

/*
 * loop2_parameter
 */
struct PARAMETER_TABLE	loop2_parameter[] =
{
	{ NULL, NULL, NULL, 0, 0, 0, NULL, 0 }
};

/*
 * loop3_parameter
 */
struct PARAMETER_TABLE	loop3_parameter[] =
{
	{ NULL, NULL, NULL, 0, 0, 0, NULL, 0 }
};

/*
 * loop4_parameter
 */
struct PARAMETER_TABLE	loop4_parameter[] =
{
	{ NULL, NULL, NULL, 0, 0, 0, NULL, 0 }
};

/*
 * dvt_parameter
 */
struct PARAMETER_TABLE  dvt_parameter[] =
{
    { NULL, NULL, NULL, 0, 0, 0, NULL, 0 }
};


/*--- Loopback Test Results ---*/

/*
 * loop1_result{}
 */
struct RESULTS_TABLE	loop1_results[] =
{
   { (char *)loop1_description, result_description, fmt_string, 
    OFF },
	{ (char *)loop1_time_of_run, result_time_of_run, fmt_string, 
	    OFF },
	{ (char *)&loop1_error_code, result_error_code, res_fmt_Int32, 
	    OFF },
	{ (char *) &loop1_run_count, result_run_count, res_fmt_uInt32, 
	    OFF },
	{ (char *) &loop1_test_fail_count, result_test_fail_count, 
	    res_fmt_uInt32, OFF },
	{ (char *) &loop1_error_count, result_error_count, res_fmt_uInt32, 
	    OFF },
	{ (char *) &loop1_fail_address, result_fail_address, res_fmt_xInt32,
       OFF },
	{ (char *) &loop1_data_expected, result_data_expected,
       res_fmt_xInt32, OFF },
	{ (char *) &loop1_data_read, result_data_read, res_fmt_xInt32,
       OFF },
	{ NULL, NULL, NULL, OFF }
};

/*
 * loop2_result{}
 */
struct RESULTS_TABLE	loop2_results[] =
{
   { (char *)loop2_description, result_description, fmt_string, 
    OFF },
	{ (char *)loop2_time_of_run, result_time_of_run, fmt_string, 
	    OFF },
	{ (char *) &loop2_error_code, result_error_code, res_fmt_Int32, 
	    OFF },
	{ (char *) &loop2_run_count, result_run_count, res_fmt_uInt32, 
	    OFF },
	{ (char *) &loop2_test_fail_count, result_test_fail_count, 
	    res_fmt_uInt32, OFF },
	{ (char *) &loop2_error_count, result_error_count, res_fmt_uInt32, 
	    OFF },
	{ (char *) &loop2_fail_address, result_fail_address, res_fmt_xInt32,
       OFF },
	{ (char *) &loop2_data_expected, result_data_expected,
       res_fmt_xInt32, OFF },
	{ (char *) &loop2_data_read, result_data_read, res_fmt_xInt32,
       OFF },
	{ NULL, NULL, NULL, OFF }
};

/*
 * loop3_result{}
 */
struct RESULTS_TABLE loop3_results[] =
{
   { (char *)loop3_description, result_description, fmt_string,
    OFF },
   { (char *)loop3_time_of_run, result_time_of_run, fmt_string,
       OFF },
   { (char *) &loop3_error_code, result_error_code, res_fmt_Int32,
       OFF },
   { (char *) &loop3_run_count, result_run_count, res_fmt_uInt32,
       OFF },
   { (char *) &loop3_test_fail_count, result_test_fail_count,
       res_fmt_uInt32, OFF },
   { (char *) &loop3_error_count, result_error_count, res_fmt_uInt32,
       OFF },
	{ (char *) &loop3_fail_address, result_fail_address, res_fmt_xInt32,
       OFF },
	{ (char *) &loop3_data_expected, result_data_expected,
       res_fmt_xInt32, OFF },
	{ (char *) &loop3_data_read, result_data_read, res_fmt_xInt32,
       OFF },
   { NULL, NULL, NULL, OFF }
};

/*
 * loop4_result{}
 */
struct RESULTS_TABLE loop4_results[] =
{
   { (char *)loop4_description, result_description, fmt_string,
    OFF },
   { (char *)loop4_time_of_run, result_time_of_run, fmt_string,
       OFF },
   { (char *) &loop4_error_code, result_error_code, res_fmt_Int32,
       OFF },
   { (char *) &loop4_run_count, result_run_count, res_fmt_uInt32,
       OFF },
   { (char *) &loop4_test_fail_count, result_test_fail_count,
       res_fmt_uInt32, OFF },
   { (char *) &loop4_error_count, result_error_count, res_fmt_uInt32,
       OFF },
	{ (char *) &loop4_fail_address, result_fail_address, res_fmt_xInt32,
       OFF },
	{ (char *) &loop4_data_expected, result_data_expected,
       res_fmt_xInt32, OFF },
	{ (char *) &loop4_data_read, result_data_read, res_fmt_xInt32,
       OFF },
   { NULL, NULL, NULL, OFF }
};

/*
 * dvt_result{}
 */
struct RESULTS_TABLE dvt_results[] =
{
   { (char *)dvt_description, result_description, fmt_string,
    OFF },
   { (char *)dvt_time_of_run, result_time_of_run, fmt_string,
       OFF },
   { (char *) &dvt_error_code, result_error_code, res_fmt_Int32,
       OFF },
   { (char *) &dvt_run_count, result_run_count, res_fmt_uInt32,
       OFF },
   { (char *) &dvt_test_fail_count, result_test_fail_count,
       res_fmt_uInt32, OFF },
   { (char *) &dvt_error_count, result_error_count, res_fmt_uInt32,
       OFF },
    { (char *) &dvt_fail_address, result_fail_address, res_fmt_xInt32,
       OFF },
    { (char *) &dvt_data_expected, result_data_expected,
       res_fmt_xInt32, OFF },
    { (char *) &dvt_data_read, result_data_read, res_fmt_xInt32,
       OFF },
   { NULL, NULL, NULL, OFF }
};


/*---< Prototypes >---*/
int	LOOP1_Test();
int	LOOP2_Test();
int	LOOP3_Test();
int	LOOP4_Test();
int dvt_test();
int lpbk_setup();

extern 	int	loop1_test();
extern 	int	loop2_test();
extern 	int	loop3_test();
extern 	int	loop4_test();
extern  int DVT_Test();
extern 	int LpbkSetup();

/*--------------------------------------------------------------------*
 * Test Token structure for LOOPBACK
 *--------------------------------------------------------------------*/
struct TEST_TOC		loopback_toc[] =
{
	{ loop1_description, "LOOP_1", "L1", LOOP1_Test,
     loop1_parameter, loop1_results, LOOP1_RUN_COUNT, 0 },
	{ loop2_description, "LOOP_2", "L2", LOOP2_Test,
     loop2_parameter, loop2_results, LOOP2_RUN_COUNT, 0 },
	{ loop3_description, "LOOP_3", "L3", LOOP3_Test,
     loop3_parameter, loop3_results, LOOP3_RUN_COUNT, 0 },
	{ loop4_description, "LOOP_4", "L4", LOOP4_Test,
     loop4_parameter, loop4_results, LOOP4_RUN_COUNT, 0 },
    { dvt_description, "DVT_Test", "L5", dvt_test,
     dvt_parameter, dvt_results, LOOP4_RUN_COUNT, 0 },

	{ NULL, NULL, NULL }
};  /* loopback_toc */


/*--------------------------------------------------------------------*
 * LOOP1_Test()
 *--------------------------------------------------------------------*/
int LOOP1_Test(
   int      ex_flag)
{
   register int   rc;
   struct image_desc *id;
   register uChar checksum = 0;

	rc = loop1_test();

   return(rc);

}  /* LOOP1_Test() */


/*--------------------------------------------------------------------*
 * LOOP2_Test()
 *--------------------------------------------------------------------*/
int LOOP2_Test(
   int      ex_flag)
{
	int	rc;

	rc = loop2_test();

	return rc;

}  /* LOOP2_Test() */


/*--------------------------------------------------------------------*
 * LOOP3_Test()
 *--------------------------------------------------------------------*/
int LOOP3_Test(
   int      ex_flag)
{
	int	rc;

	rc = loop3_test();

	return rc;

}  /* LOOP3_Test() */


/*--------------------------------------------------------------------*
 * LOOP4_Test()
 *--------------------------------------------------------------------*/
int LOOP4_Test(
   int      ex_flag)
{
	int	rc;

	rc = loop4_test();

	return rc;

}  /* LOOP4_Test() */


/*--------------------------------------------------------------------*
 * dvt_test()
 *    DVT loopback test. depend on W&G to pump ethernet traffic 
 *--------------------------------------------------------------------*/
int
dvt_test(int  ex_flag)
{
    int rc;

    rc = DVT_Test();

    return rc;

}  /* dvt_Test() */

/*--------------------------------------------------------------------*
 * lpbk_setup()
 *     turn on/off the PMC internal loopback mode
 *--------------------------------------------------------------------*/
int
lpbk_setup(int  ex_flag)
{
    int rc;

    rc = LpbkSetup();

    return rc;

}  /* lpbk_setup() */


/*---------------------------< END >---------------------------------*/
