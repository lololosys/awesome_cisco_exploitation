/* $Id: bigadiag.c,v 1.1.4.1 1996/05/09 15:00:47 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/bigadiag.c,v $
 *-----------------------------------------------------------------
 * bigadiag.c -- Bus Interface Gate Array (BIGA) Diagnostics
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bigadiag.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:47  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:45  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:06  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:57  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:40  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:16  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************/
/*                                                                            */
/* CALYPSO DIAGNOSTICS TEST SUITES                                            */
/*                                                                            */
/* This file contains procedures related to "Bus Interface Gate Array (BIGA)  */
/* Diagnostics Section" that are included as part of CALYPSO Diagnostics Test */
/* Suites.                                                                    */
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

#include	"types.h"
#include	"constant.h"
#include	"struct.h"
#include	"globdef.h"
#include	"localdef.h"
#include	"hw.h"
#include	"errcode.h"

#include	"biga_hw.h"

#if  1  /* RLB DBG */
#include	"mac_hw.h"
#endif  /* RLB DBG */ 

#include	"eti_hw.h"
#include	"diag.h"


#define	PHY_PORT_A	1
#define	PHY_PORT_B	2

extern	uChar	sys_source_fddi_mac[6];
extern	uChar	sys_dest_fddi_mac[6];
extern	uInt32	sys_fail_address, sys_data_expected, sys_data_read;
extern	Int32	sys_error_code;
extern	char	test_in_progress_msg[], cli_print_buf[], sys_specific_info[];
extern	char	*diag_test_id;
extern	uInt32	diag_current_runcount, diag_verbosity, diag_runcount;
extern	char	diag_packetsizevariant[], diag_datapatternvariant[];
extern	char	diag_npbusarbitration[], diag_fbbusarbitration[];
/*extern	char	ROMAddress[];*/

	char		bigadiag_description[] = 
	"BIGA Gate Array Diagnostics Section";
	uInt32		bigadiag_runcount = 1;
	uInt32		bigadiag_maxerrorcount = 0;
	uInt32		bigadiag_timedelaycount = 0;
	char		bigadiag_bigainterface[3] = "16";
	char		bigadiag_nmparbitration[6] = "False";
	char		bigadiag_ethernetarbitration[6] = "False";
	char		bigadiag_resetflag[6] = "False";
extern	int		etidiag_loadgen_initonly;

	char		bigaacc_description[] = 
	"BIGA Gate Array Accessibility Test";
	char		bigaacc_time_of_run[18];
	Int32		bigaacc_error_code;
	uInt32		bigaacc_run_count;
	uInt32		bigaacc_test_fail_count;
	uInt32		bigaacc_error_count;
	uInt32		bigaacc_fail_address;
	uInt32		bigaacc_data_expected;
	uInt32		bigaacc_data_read;

	char		bigaecho_description[] = 
	"BIGA Packet Echo Test";
	char		bigaecho_time_of_run[18];
	Int32		bigaecho_error_code;
	uInt32		bigaecho_run_count;
	uInt32		bigaecho_test_fail_count;
	uInt32		bigaecho_error_count;
	uInt32		bigaecho_fail_address;
	uInt32		bigaecho_data_expected;
	uInt32		bigaecho_data_read;

#if  0  /* RLB: remove tests do not apply to ATM  */
	char		bigabufrw_description[] = 
	"BIGA->MAC Buffer Memory Read/Write Test";
	uInt32		bigabufrw_start_address = MACBUF_START;
	uInt32		bigabufrw_end_address = MACBUF_END;
	uInt32		bigabufrw_data = 0x12345678;
	char		bigabufrw_access[5] = "Long";
	char		bigabufrw_mode[11] = "Random";
	uInt32		bigabufrw_increment = 0x00000001;
	char		bigabufrw_time_of_run[18];
	Int32		bigabufrw_error_code;
	uInt32		result_bigabufrw_start_address;
	uInt32		result_bigabufrw_end_address;
	uInt32		result_bigabufrw_data;
	char		result_bigabufrw_access[5];
	char		result_bigabufrw_mode[11];
	uInt32		result_bigabufrw_increment;
	uInt32		bigabufrw_run_count;
	uInt32		bigabufrw_test_fail_count;
	uInt32		bigabufrw_error_count;
	uInt32		bigabufrw_fail_address;
	uInt32		bigabufrw_data_expected;
	uInt32		bigabufrw_data_read;

	char		bigatxdma_description[] = 
	"BIGA Frame Buffer->MAC Transmit DMA Test";
	char		bigatxdma_qlevel[4] = "0";
	uInt32		bigatxdma_packetsize = FDDI_DEFAULT_PACKETSIZE;
	uInt32		bigatxdma_data = 0x12;
	char		bigatxdma_access[5] = "Byte";
	char		bigatxdma_mode[11] = "Random";
	uInt32		bigatxdma_increment = 0x01;
	char		bigatxdma_burstsize[4] = "8";
	char		bigatxdma_time_of_run[18];
	Int32		bigatxdma_error_code;
	char		result_bigatxdma_qlevel[4];
	uInt32		result_bigatxdma_packetsize;
	uInt32		result_bigatxdma_data;
	char		result_bigatxdma_access[5];
	char		result_bigatxdma_mode[11];
	uInt32		result_bigatxdma_increment;
	char		result_bigatxdma_burstsize[4];
	uInt32		bigatxdma_run_count;
	uInt32		bigatxdma_test_fail_count;
	uInt32		bigatxdma_error_count;
	uInt32		bigatxdma_fail_address;
	uInt32		bigatxdma_data_expected;
	uInt32		bigatxdma_data_read;

	char		bigarxdma_description[] = 
	"BIGA Frame Buffer<-MAC Receive DMA Test";
	uInt32		bigarxdma_packetsize = FDDI_DEFAULT_PACKETSIZE;
	uInt32		bigarxdma_data = 0x12;
	char		bigarxdma_access[5] = "Byte";
	char		bigarxdma_mode[11] = "Random";
	uInt32		bigarxdma_increment = 0x01;
	char		bigarxdma_burstsize[4] = "8";
	char		bigarxdma_time_of_run[18];
	Int32		bigarxdma_error_code;
	uInt32		result_bigarxdma_packetsize;
	uInt32		result_bigarxdma_data;
	char		result_bigarxdma_access[5];
	char		result_bigarxdma_mode[11];
	uInt32		result_bigarxdma_increment;
	char		result_bigarxdma_burstsize[4];
	uInt32		bigarxdma_run_count;
	uInt32		bigarxdma_test_fail_count;
	uInt32		bigarxdma_error_count;
	uInt32		bigarxdma_fail_address;
	uInt32		bigarxdma_data_expected;
	uInt32		bigarxdma_data_read;

	char		bigaintlb_description[] = 
	"BIGA<->MAC Layer Internal Loopback Test";
	char		bigaintlb_qlevel[4] = "0";
	uInt32		bigaintlb_packetsize = FDDI_DEFAULT_PACKETSIZE;
	uInt32		bigaintlb_data = 0x12;
	char		bigaintlb_access[5] = "Byte";
	char		bigaintlb_mode[11] = "Random";
	uInt32		bigaintlb_increment = 0x01;
	char		bigaintlb_burstsize[4] = "8";
	char		bigaintlb_time_of_run[18];
	Int32		bigaintlb_error_code;
	char		result_bigaintlb_qlevel[4];
	uInt32		result_bigaintlb_packetsize;
	uInt32		result_bigaintlb_data;
	char		result_bigaintlb_access[5];
	char		result_bigaintlb_mode[11];
	uInt32		result_bigaintlb_increment;
	char		result_bigaintlb_burstsize[4];
	uInt32		bigaintlb_run_count;
	uInt32		bigaintlb_test_fail_count;
	uInt32		bigaintlb_error_count;
	uInt32		bigaintlb_fail_address;
	uInt32		bigaintlb_data_expected;
	uInt32		bigaintlb_data_read;

	char		bigaextlb_description[] = 
	"BIGA<->MAC Layer External Loopback Test";
	char		bigaextlb_portnum[4] = "All";
	char		bigaextlb_qlevel[4] = "0";
	uInt32		bigaextlb_packetsize = FDDI_DEFAULT_PACKETSIZE;
	uInt32		bigaextlb_data = 0x12;
	char		bigaextlb_access[5] = "Byte";
	char		bigaextlb_mode[11] = "Random";
	uInt32		bigaextlb_increment = 0x01;
	char		bigaextlb_burstsize[4] = "8";
	char		bigaextlb_lblevel[9] = "XT_Loop";
	char		bigaextlb_time_of_run[18];
	Int32		bigaextlb_error_code;
	char		result_bigaextlb_portnum[4];
	char		result_bigaextlb_qlevel[4];
	uInt32		result_bigaextlb_packetsize;
	uInt32		result_bigaextlb_data;
	char		result_bigaextlb_access[5];
	char		result_bigaextlb_mode[11];
	uInt32		result_bigaextlb_increment;
	char		result_bigaextlb_burstsize[4];
	char		result_bigaextlb_lblevel[9];
	uInt32		bigaextlb_run_count;
	uInt32		bigaextlb_test_fail_count;
	uInt32		bigaextlb_error_count;
	uInt32		bigaextlb_fail_address;
	uInt32		bigaextlb_data_expected;
	uInt32		bigaextlb_data_read;

 	char            bigaloadgen_description[] =
        "BIGA<->MAC Layer Load Generation Test";
        uInt32          bigaloadgen_no_of_packets = 64;
        uInt32          bigaloadgen_packetsize = FDDI_DEFAULT_PACKETSIZE;
        uInt32          bigaloadgen_data = 0x12;
        char            bigaloadgen_access[5] = "Byte";
        char            bigaloadgen_mode[11] = "Random";
        uInt32          bigaloadgen_increment = 0x01;
	char		bigaloadgen_burstsize[4] = "8";
	char		bigaloadgen_validation[6] = "True";
	char		bigaloadgen_loadgenmode[10] = "Xceive";
	uInt32		bigaloadgen_frequency = 0;
	uInt32		bigaloadgen_runcount = 1;
        char            bigaloadgen_time_of_run[18];
        Int32           bigaloadgen_error_code;
        uInt32          result_bigaloadgen_no_of_packets;
        uInt32          result_bigaloadgen_packetsize;
        uInt32          result_bigaloadgen_data;
        char            result_bigaloadgen_access[5];
        char            result_bigaloadgen_mode[11];
        uInt32          result_bigaloadgen_increment;
	char		result_bigaloadgen_burstsize[4];
	char		result_bigaloadgen_validation[6];
	char		result_bigaloadgen_loadgenmode[10];
	uInt32		result_bigaloadgen_frequency;
        uInt32          bigaloadgen_run_count;
        uInt32          bigaloadgen_test_fail_count;
        uInt32          bigaloadgen_error_count;
        uInt32          bigaloadgen_fail_address;
        uInt32          bigaloadgen_data_expected;
        uInt32          bigaloadgen_data_read;
#endif  /* RLB DBG */

	struct PARAMETER_TABLE	bigadiag_parameter[] =
	{ { (char *) &bigadiag_runcount, parm_runcount, fmt_uInt32,
	    1, 1, 99999999, NULL, 0 },
	  { (char *) &bigadiag_maxerrorcount, parm_maxerrorcount, fmt_uInt32,
	    0, 0, 99999999, NULL, 0 },
	  { (char *) &bigadiag_timedelaycount, parm_timedelaycount, fmt_uInt32,
	    0, 0, 99999999, NULL, 0 },
	  { (char *)  bigadiag_bigainterface, parm_bigainterface, fmt_string,
	    (uInt32) value_bigainterface_16, 0, 0, value_bigainterface, 0 },
	  { (char *)  bigadiag_nmparbitration, parm_nmparbitration, fmt_string,
	    (uInt32) value_flag_false, 0, 0, value_flag, 0 },
	  { (char *)  bigadiag_ethernetarbitration, parm_ethernetarbitration, 
	    fmt_string,
	    (uInt32) value_flag_false, 0, 0, value_flag, 0 },
	  { (char *)  bigadiag_resetflag, parm_resetflag, fmt_string,
	    (uInt32) value_flag_false, 0, 0, value_flag, 0 },
	  { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };

	struct PARAMETER_TABLE	bigaacc_parameter[] =
	{ { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };

	struct PARAMETER_TABLE	bigaecho_parameter[] =
	{ { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };

#if  0  /* RLB: N/A for ATM */
	struct PARAMETER_TABLE	bigabufrw_parameter[] =
	{ { (char *) &bigabufrw_start_address, parm_start_address, fmt_xInt32,
	    MACBUF_START, MACBUF_START, MACBUF_END, NULL, 0 },
	  { (char *) &bigabufrw_end_address, parm_end_address, fmt_xInt32,
	    MACBUF_END, MACBUF_START, MACBUF_END, NULL, 0 },
	  { (char *) &bigabufrw_data, parm_data, fmt_xInt32,
	    0x12345678, 0x00000000, 0xFFFFFFFF, NULL, 0 },
	  { (char *)  bigabufrw_access, parm_access, fmt_string,
	    (uInt32) value_access_long, 0, 0, value_access, 0 },
	  { (char *)  bigabufrw_mode, parm_mode, fmt_string,
	    (uInt32) value_mode_random, 0, 0, value_mode, 0 },
	  { (char *) &bigabufrw_increment, parm_increment, fmt_xInt32,
	    0x00000001, 0x00000001, 0xFFFFFFFF, NULL, 0 },
	  { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };

	struct PARAMETER_TABLE	bigatxdma_parameter[] =
	{ { (char *)  bigatxdma_qlevel, parm_qlevel, fmt_string,
	    (uInt32) value_qlevel_0, 0, 0, value_qlevel, 0 },
	  { (char *) &bigatxdma_packetsize, parm_packetsize, fmt_uInt32,
	    FDDI_DEFAULT_PACKETSIZE, FDDI_MIN_PACKETSIZE, FDDI_MAX_PACKETSIZE, 
	    NULL, 0 },
	  { (char *) &bigatxdma_data, parm_data, fmt_xInt08,
	    0x12, 0x00, 0xFF, NULL, 0 },
	  { (char *)  bigatxdma_access, parm_access, fmt_string,
	    (uInt32) value_access_byte, 0, 0, value_access_byte_only, 0 },
	  { (char *)  bigatxdma_mode, parm_mode, fmt_string,
	    (uInt32) value_mode_random, 0, 0, value_mode, 0 },
	  { (char *) &bigatxdma_increment, parm_increment, fmt_xInt08,
	    0x01, 0x01, 0xFF, NULL, 0 },
	  { (char *)  bigatxdma_burstsize, parm_burstsize, fmt_string,
	    (uInt32) value_burstsize_8, 0, 0, value_burstsize, 0 },
	  { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };

	struct PARAMETER_TABLE	bigarxdma_parameter[] =
	{ { (char *) &bigarxdma_packetsize, parm_packetsize, fmt_uInt32,
	    FDDI_DEFAULT_PACKETSIZE, FDDI_MIN_PACKETSIZE, FDDI_MAX_PACKETSIZE, 
	    NULL, 0 },
	  { (char *) &bigarxdma_data, parm_data, fmt_xInt08,
	    0x12, 0x00, 0xFF, NULL, 0 },
	  { (char *)  bigarxdma_access, parm_access, fmt_string,
	    (uInt32) value_access_byte, 0, 0, value_access_byte_only, 0 },
	  { (char *)  bigarxdma_mode, parm_mode, fmt_string,
	    (uInt32) value_mode_random, 0, 0, value_mode, 0 },
	  { (char *) &bigarxdma_increment, parm_increment, fmt_xInt08,
	    0x01, 0x01, 0xFF, NULL, 0 },
	  { (char *)  bigarxdma_burstsize, parm_burstsize, fmt_string,
	    (uInt32) value_burstsize_8, 0, 0, value_burstsize, 0 },
	  { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };

	struct PARAMETER_TABLE	bigaintlb_parameter[] =
	{ { (char *)  bigaintlb_qlevel, parm_qlevel, fmt_string,
	    (uInt32) value_qlevel_0, 0, 0, value_qlevel, 0 },
	  { (char *) &bigaintlb_packetsize, parm_packetsize, fmt_uInt32,
	    FDDI_DEFAULT_PACKETSIZE, FDDI_MIN_PACKETSIZE, FDDI_MAX_PACKETSIZE, 
	    NULL, 0 },
	  { (char *) &bigaintlb_data, parm_data, fmt_xInt08,
	    0x12, 0x00, 0xFF, NULL, 0 },
	  { (char *)  bigaintlb_access, parm_access, fmt_string,
	    (uInt32) value_access_byte, 0, 0, value_access_byte_only, 0 },
	  { (char *)  bigaintlb_mode, parm_mode, fmt_string,
	    (uInt32) value_mode_random, 0, 0, value_mode, 0 },
	  { (char *) &bigaintlb_increment, parm_increment, fmt_xInt08,
	    0x01, 0x01, 0xFF, NULL, 0 },
	  { (char *)  bigaintlb_burstsize, parm_burstsize, fmt_string,
	    (uInt32) value_burstsize_8, 0, 0, value_burstsize, 0 },
	  { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };

	struct PARAMETER_TABLE	bigaextlb_parameter[] =
	{ { (char *)  bigaextlb_portnum, parm_portnum, fmt_string,
	    (uInt32) value_portnum_all, 0, 0, value_portnum_AB, 0 },
	  { (char *)  bigaextlb_qlevel, parm_qlevel, fmt_string,
	    (uInt32) value_qlevel_0, 0, 0, value_qlevel, 0 },
	  { (char *) &bigaextlb_packetsize, parm_packetsize, fmt_uInt32,
	    FDDI_DEFAULT_PACKETSIZE, FDDI_MIN_PACKETSIZE, FDDI_MAX_PACKETSIZE, 
	    NULL, 0 },
	  { (char *) &bigaextlb_data, parm_data, fmt_xInt08,
	    0x12, 0x00, 0xFF, NULL, 0 },
	  { (char *)  bigaextlb_access, parm_access, fmt_string,
	    (uInt32) value_access_byte, 0, 0, value_access_byte_only, 0 },
	  { (char *)  bigaextlb_mode, parm_mode, fmt_string,
	    (uInt32) value_mode_random, 0, 0, value_mode, 0 },
	  { (char *) &bigaextlb_increment, parm_increment, fmt_xInt08,
	    0x01, 0x01, 0xFF, NULL, 0 },
	  { (char *)  bigaextlb_burstsize, parm_burstsize, fmt_string,
	    (uInt32) value_burstsize_8, 0, 0, value_burstsize, 0 },
	  { (char *)  bigaextlb_lblevel, parm_lblevel, fmt_string,
	    (uInt32) value_lblevel_XT, 0, 0, value_lblevel, 0 },
	  { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };

 	struct PARAMETER_TABLE  bigaloadgen_parameter[] =
        { { (char *) &bigaloadgen_no_of_packets, parm_no_of_packets, fmt_uInt32,
            64, 1, 64, NULL, 0 },
          { (char *) &bigaloadgen_packetsize, parm_packetsize, fmt_uInt32,
            FDDI_DEFAULT_PACKETSIZE,
            FDDI_MIN_PACKETSIZE, FDDI_MAX_PACKETSIZE,
            NULL, 0 },
          { (char *) &bigaloadgen_data, parm_data, fmt_xInt08,
            0x12, 0x00, 0xFF, NULL, 0 },
          { (char *)  bigaloadgen_access, parm_access, fmt_string,
            (uInt32) value_access_byte, 0, 0, value_access_byte_only, 0 },
          { (char *)  bigaloadgen_mode, parm_mode, fmt_string,
            (uInt32) value_mode_random, 0, 0, value_mode, 0 },
          { (char *) &bigaloadgen_increment, parm_increment, fmt_xInt08,
            0x01, 0x01, 0xFF, NULL, 0 },
	  { (char *)  bigaloadgen_burstsize, parm_burstsize, fmt_string,
	    (uInt32) value_burstsize_8, 0, 0, "/1/2/4/8/16/", 0 },
	  { (char *)  bigaloadgen_validation, parm_validation, fmt_string,
	    (uInt32) value_flag_true, 0, 0, value_flag, 0 },
	  { (char *)  bigaloadgen_loadgenmode, parm_loadgenmode, fmt_string,
	    (uInt32) value_loadgenmode_xceive, 0, 0, value_loadgenmode, 0 },
          { (char *) &bigaloadgen_frequency, parm_frequency, fmt_uInt32,
            0, 0, 99999999, NULL, 0 },
          { (char *) &bigaloadgen_runcount, parm_runcount, fmt_uInt32,
            1, 1, 99999999, NULL, 0 },
          { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };

#endif   /* RLB DBG */

	struct RESULTS_TABLE	bigaacc_results[] =
	{ { (char *)  bigaacc_description, result_description, fmt_string, 
	    OFF },
	  { (char *)  bigaacc_time_of_run, result_time_of_run, fmt_string, 
	    OFF },
	  { (char *) &bigaacc_error_code, result_error_code, res_fmt_Int32, 
	    OFF },
	  { (char *) &bigaacc_run_count, result_run_count, res_fmt_uInt32, 
	    OFF },
	  { (char *) &bigaacc_test_fail_count, result_test_fail_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &bigaacc_error_count, result_error_count, res_fmt_uInt32, 
	    OFF },
	  { (char *) &bigaacc_fail_address, result_fail_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &bigaacc_data_expected, result_data_expected, 
	    res_fmt_xInt32, OFF },
	  { (char *) &bigaacc_data_read, result_data_read, res_fmt_xInt32, 
	    OFF },
	  { NULL, NULL, NULL, OFF } };

	struct RESULTS_TABLE	bigaecho_results[] =
	{ { (char *)bigaecho_description, result_description, fmt_string, 
	    OFF },
	  { (char *)bigaecho_time_of_run, result_time_of_run, fmt_string, 
	    OFF },
	  { (char *)&bigaecho_error_code, result_error_code, res_fmt_Int32, 
	    OFF },
	  { (char *)&bigaecho_run_count, result_run_count, res_fmt_uInt32, 
	    OFF },
	  { (char *)&bigaecho_test_fail_count, result_test_fail_count, 
	    res_fmt_uInt32, OFF },
	  { (char *)&bigaecho_error_count, result_error_count, res_fmt_uInt32, 
	    OFF },
	  { (char *)&bigaecho_fail_address, result_fail_address, 
	    res_fmt_xInt32, OFF },
	  { (char *)&bigaecho_data_expected, result_data_expected, 
	    res_fmt_xInt32, OFF },
	  { (char *)&bigaecho_data_read, result_data_read, res_fmt_xInt32, 
	    OFF },
	  { NULL, NULL, NULL, OFF } };

#if  0  /* RLB: N/A for ATM */
	struct RESULTS_TABLE	bigabufrw_results[] =
	{ { (char *)  bigabufrw_description, result_description, fmt_string,
	    OFF },
	  { (char *)  bigabufrw_time_of_run, result_time_of_run, fmt_string,
	    OFF },
	  { (char *) &bigabufrw_error_code, result_error_code, res_fmt_Int32,
	    OFF },
	  { (char *) &result_bigabufrw_start_address, result_start_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &result_bigabufrw_end_address, result_end_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &result_bigabufrw_data, result_data, res_fmt_xInt32, 
	    OFF },
	  { (char *)  result_bigabufrw_access, result_access, res_fmt_string,
	    OFF },
	  { (char *)  result_bigabufrw_mode, result_mode, res_fmt_string, 
	    OFF },
	  { (char *) &result_bigabufrw_increment, result_increment,
	    res_fmt_xInt32, OFF },
	  { (char *) &bigabufrw_run_count, result_run_count, res_fmt_uInt32,
	    OFF },
	  { (char *) &bigabufrw_test_fail_count, result_test_fail_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &bigabufrw_error_count, result_error_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &bigabufrw_fail_address, result_fail_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &bigabufrw_data_expected, result_data_expected,
	    res_fmt_xInt32, OFF },
	  { (char *) &bigabufrw_data_read, result_data_read, res_fmt_xInt32,
	    OFF },
	  { NULL, NULL, NULL, OFF } };

	struct RESULTS_TABLE	bigatxdma_results[] =
	{ { (char *)  bigatxdma_description, result_description, fmt_string,
	    OFF },
	  { (char *)  bigatxdma_time_of_run, result_time_of_run, fmt_string,
	    OFF },
	  { (char *) &bigatxdma_error_code, result_error_code, res_fmt_Int32,
	    OFF },
	  { (char *)  result_bigatxdma_qlevel, result_qlevel, res_fmt_string,
	    OFF },
	  { (char *) &result_bigatxdma_packetsize, result_packetsize, 
	    res_fmt_uInt32, OFF },
	  { (char *) &result_bigatxdma_data, result_data, res_fmt_xInt08, 
	    OFF },
	  { (char *)  result_bigatxdma_access, result_access, res_fmt_string,
	    OFF },
	  { (char *)  result_bigatxdma_mode, result_mode, res_fmt_string, 
	    OFF },
	  { (char *) &result_bigatxdma_increment, result_increment,
	    res_fmt_xInt08, OFF },
	  { (char *)  result_bigatxdma_burstsize, result_burstsize, 
	    res_fmt_string, OFF },
	  { (char *) &bigatxdma_run_count, result_run_count, res_fmt_uInt32,
	    OFF },
	  { (char *) &bigatxdma_test_fail_count, result_test_fail_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &bigatxdma_error_count, result_error_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &bigatxdma_fail_address, result_fail_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &bigatxdma_data_expected, result_data_expected,
	    res_fmt_xInt32, OFF },
	  { (char *) &bigatxdma_data_read, result_data_read, res_fmt_xInt32,
	    OFF },
	  { NULL, NULL, NULL, OFF } };

	struct RESULTS_TABLE	bigarxdma_results[] =
	{ { (char *)  bigarxdma_description, result_description, fmt_string,
	    OFF },
	  { (char *)  bigarxdma_time_of_run, result_time_of_run, fmt_string,
	    OFF },
	  { (char *) &bigarxdma_error_code, result_error_code, res_fmt_Int32,
	    OFF },
	  { (char *) &result_bigarxdma_packetsize, result_packetsize, 
	    res_fmt_uInt32, OFF },
	  { (char *) &result_bigarxdma_data, result_data, res_fmt_xInt08, 
	    OFF },
	  { (char *)  result_bigarxdma_access, result_access, res_fmt_string,
	    OFF },
	  { (char *)  result_bigarxdma_mode, result_mode, res_fmt_string, 
	    OFF },
	  { (char *) &result_bigarxdma_increment, result_increment,
	    res_fmt_xInt08, OFF },
	  { (char *)  result_bigarxdma_burstsize, result_burstsize, 
	    res_fmt_string, OFF },
	  { (char *) &bigarxdma_run_count, result_run_count, res_fmt_uInt32,
	    OFF },
	  { (char *) &bigarxdma_test_fail_count, result_test_fail_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &bigarxdma_error_count, result_error_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &bigarxdma_fail_address, result_fail_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &bigarxdma_data_expected, result_data_expected,
	    res_fmt_xInt32, OFF },
	  { (char *) &bigarxdma_data_read, result_data_read, res_fmt_xInt32,
	    OFF },
	  { NULL, NULL, NULL, OFF } };

	struct RESULTS_TABLE	bigaintlb_results[] =
	{ { (char *)  bigaintlb_description, result_description, fmt_string,
	    OFF },
	  { (char *)  bigaintlb_time_of_run, result_time_of_run, fmt_string,
	    OFF },
	  { (char *) &bigaintlb_error_code, result_error_code, res_fmt_Int32,
	    OFF },
	  { (char *)  result_bigaintlb_qlevel, result_qlevel, res_fmt_string,
	    OFF },
	  { (char *) &result_bigaintlb_packetsize, result_packetsize, 
	    res_fmt_uInt32, OFF },
	  { (char *) &result_bigaintlb_data, result_data, res_fmt_xInt08, 
	    OFF },
	  { (char *)  result_bigaintlb_access, result_access, res_fmt_string,
	    OFF },
	  { (char *)  result_bigaintlb_mode, result_mode, res_fmt_string, 
	    OFF },
	  { (char *) &result_bigaintlb_increment, result_increment,
	    res_fmt_xInt08, OFF },
	  { (char *)  result_bigaintlb_burstsize, result_burstsize, 
	    res_fmt_string, OFF },
	  { (char *) &bigaintlb_run_count, result_run_count, res_fmt_uInt32,
	    OFF },
	  { (char *) &bigaintlb_test_fail_count, result_test_fail_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &bigaintlb_error_count, result_error_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &bigaintlb_fail_address, result_fail_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &bigaintlb_data_expected, result_data_expected, 
	    res_fmt_xInt32, OFF },
	  { (char *) &bigaintlb_data_read, result_data_read, res_fmt_xInt32,
	    OFF },
	  { NULL, NULL, NULL, OFF } };

	struct RESULTS_TABLE	bigaextlb_results[] =
	{ { (char *)  bigaextlb_description, result_description, fmt_string,
	    OFF },
	  { (char *)  bigaextlb_time_of_run, result_time_of_run, fmt_string,
	    OFF },
	  { (char *) &bigaextlb_error_code, result_error_code, res_fmt_Int32,
	    OFF },
	  { (char *)  result_bigaextlb_portnum, result_portnum, res_fmt_string,
	    OFF },
	  { (char *)  result_bigaextlb_qlevel, result_qlevel, res_fmt_string,
	    OFF },
	  { (char *) &result_bigaextlb_packetsize, result_packetsize, 
	    res_fmt_uInt32, OFF },
	  { (char *) &result_bigaextlb_data, result_data, res_fmt_xInt08, 
	    OFF },
	  { (char *)  result_bigaextlb_access, result_access, res_fmt_string,
	    OFF },
	  { (char *)  result_bigaextlb_mode, result_mode, res_fmt_string, 
	    OFF },
	  { (char *) &result_bigaextlb_increment, result_increment,
	    res_fmt_xInt08, OFF },
	  { (char *)  result_bigaextlb_burstsize, result_burstsize, 
	    res_fmt_string, OFF },
	  { (char *)  result_bigaextlb_lblevel, result_lblevel, 
	    res_fmt_string, OFF },
	  { (char *) &bigaextlb_run_count, result_run_count, res_fmt_uInt32,
	    OFF },
	  { (char *) &bigaextlb_test_fail_count, result_test_fail_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &bigaextlb_error_count, result_error_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &bigaextlb_fail_address, result_fail_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &bigaextlb_data_expected, result_data_expected, 
	    res_fmt_xInt32, OFF },
	  { (char *) &bigaextlb_data_read, result_data_read, res_fmt_xInt32,
	    OFF },
	  { NULL, NULL, NULL, OFF } };

        struct RESULTS_TABLE    bigaloadgen_results[] =
        { { (char *)  bigaloadgen_description, result_description, fmt_string,
            OFF },
          { (char *)  bigaloadgen_time_of_run, result_time_of_run, fmt_string,
            OFF },
          { (char *) &bigaloadgen_error_code, result_error_code, res_fmt_Int32,
            OFF },
          { (char *) &result_bigaloadgen_no_of_packets, result_no_of_packets,
            res_fmt_uInt32, OFF },
          { (char *) &result_bigaloadgen_packetsize, result_packetsize,
            res_fmt_uInt32, OFF },
          { (char *) &result_bigaloadgen_data, result_data, res_fmt_xInt08,
            OFF },
          { (char *)  result_bigaloadgen_access, result_access, res_fmt_string,
            OFF },
          { (char *)  result_bigaloadgen_mode, result_mode, res_fmt_string,
            OFF },
          { (char *) &result_bigaloadgen_increment, result_increment,
            res_fmt_xInt08, OFF },
	  { (char *)  result_bigaloadgen_burstsize, result_burstsize, 
	    res_fmt_string, OFF },
	  { (char *)  result_bigaloadgen_validation, result_validation, 
	    res_fmt_string, OFF },
	  { (char *)  result_bigaloadgen_loadgenmode, result_loadgenmode, 
	    res_fmt_string, OFF },
          { (char *) &result_bigaloadgen_frequency, result_frequency, 
	    res_fmt_uInt32, OFF },
          { (char *) &bigaloadgen_runcount, result_run_count, res_fmt_uInt32,
            OFF },
          { (char *) &bigaloadgen_run_count, result_run_count, res_fmt_uInt32,
            OFF },
          { (char *) &bigaloadgen_test_fail_count, result_test_fail_count,
            res_fmt_uInt32, OFF },
          { (char *) &bigaloadgen_error_count, result_error_count,
            res_fmt_uInt32, OFF },
          { (char *) &bigaloadgen_fail_address, result_fail_address,
            res_fmt_xInt32, OFF },
          { (char *) &bigaloadgen_data_expected, result_data_expected,
            res_fmt_xInt32, OFF },
          { (char *) &bigaloadgen_data_read, result_data_read, res_fmt_xInt32,
            OFF },
          { NULL, NULL, NULL, OFF } };

	int		BIGABUFRW_Test();
	int		BIGATXDMA_Test();
	int		BIGARXDMA_Test();
	int		BIGAINTLB_Test();
	int		BIGAEXTLB_Test();
	int		BIGALOADGEN_Test();
#endif  /* RLB DBG */

	int		BIGAACC_Test();
int	BIGALB1_Test();   /* RLB DBG */
extern  void	bigatest_main(void);   /* RLB DBG */

        /*--------------------------------------------------------------------*
         * Test Token structure for BIGADIAG
         *--------------------------------------------------------------------*/
	struct TEST_TOC		bigadiag_toc[] =
	{ { bigaacc_description, "BIGAACC", "B1", BIGAACC_Test,
	    bigaacc_parameter, bigaacc_results, BIGAACC_RUN_COUNT, 0 },
#if  0   /* RLB DBG */
	  { bigaecho_description, "BIGALB1", "B2", BIGALB1_Test,
	    bigaecho_parameter, bigaecho_results, 
	    BIGAECHO_RUN_COUNT, 0 },
#endif  /* RLB DBG */

#if  0  /* RLB DBG */
	  { bigabufrw_description, "BIGABUFRW", "B2", BIGABUFRW_Test,
	    bigabufrw_parameter, bigabufrw_results, 
	    BIGAMACBUFRW_RUN_COUNT, 0 },
	  { bigatxdma_description, "BIGATXDMA", "B3", BIGATXDMA_Test,
	    bigatxdma_parameter, bigatxdma_results, 
	    BIGATXDMA_RUN_COUNT, 0 },
	  { bigarxdma_description, "BIGARXDMA", "B4", BIGARXDMA_Test,
	    bigarxdma_parameter, bigarxdma_results, 
	    BIGARXDMA_RUN_COUNT, 0 },
	  { bigaintlb_description, "BIGAINTLB", "B5", BIGAINTLB_Test,
	    bigaintlb_parameter, bigaintlb_results, 
	    BIGAINTLB_RUN_COUNT, 0 },
	  { bigaextlb_description, "BIGAEXTLB", "B6", BIGAEXTLB_Test,
	    bigaextlb_parameter, bigaextlb_results, 
	    BIGAEXTLB_RUN_COUNT, 0 },
	  { bigaloadgen_description, "BIGALOADGEN", "B7", BIGALOADGEN_Test,
	    bigaloadgen_parameter, bigaloadgen_results, 
	    BIGALOADGEN_RUN_COUNT, 0 },
#endif  /* RLB DBG */
	  { NULL, NULL, NULL } };

	void	soft_reset_biga();
	void	configure_biga();


/*-------------------------------------------------------------------*
 * BIGA_ACC_Test()
 *-------------------------------------------------------------------*/
int BIGAACC_Test(
	int		ex_flag)
{
extern	uInt16	biga_status;

	register int	i;

	struct BIGA_REGS *biga_ptr = (struct BIGA_REGS *) BIGA_START;
	struct BIGA_REGS *lbiga_ptr = (struct BIGA_REGS *) LOCAL_BIGA_START;
	uInt32		data_read;
	uInt32		*head, *tail, *tmph, *ptr;

	if (diag_current_runcount == 1 || bigadiag_resetflag[0] == 'T') {
	   soft_reset_biga();
	}

	if (diag_verbosity >= VERBOSITY_LEVEL3) {
	   strcpy(sys_specific_info, "BIGA"); 
	   sprintf(cli_print_buf, test_in_progress_msg, (uInt32) biga_ptr,
	           diag_test_id, diag_current_runcount, sys_specific_info);
	   put_line(cli_print_buf);
	}

	for (i = 0; i < 2; i++) {

	   if (i == 0) {
	      head = (uInt32 *)&biga_ptr->r28.thead;
	      tail = (uInt32 *)&biga_ptr->r29.ttail;
	      tmph = (uInt32 *)&biga_ptr->r30.ttmph;
	      ptr  = (uInt32 *)&biga_ptr->r31.tptr;
	   }
	   else {
	      head = (uInt32 *) &biga_ptr->r37.rhead;
	      tail = (uInt32 *) &biga_ptr->r38.rtail;
	      tmph = (uInt32 *) &biga_ptr->r39.rtmph;
	      ptr  = (uInt32 *) &biga_ptr->r42.rptr;
	   }

  	   *head = 0x55555554; *tail = 0xAAAAAAA8;
	   if ((data_read = *head) != 0x55555554) {
	      sys_fail_address = (uInt32) head;
	      sys_data_expected = 0x55555554;
	      sys_data_read = data_read;
	      sys_error_code = BIGADIAG_BIGA_ACCESSIBILITY_ERROR;
	      biga_status |= BIGA_ACC_FAIL;
	      if (check_for_test_stop())
	         return(sys_error_code);
	   }
	   if ((data_read = *tail) != 0xAAAAAAA8) {
	      sys_fail_address = (uInt32) tail;
	      sys_data_expected = 0xAAAAAAA8;
	      sys_data_read = data_read;
	      sys_error_code = BIGADIAG_BIGA_ACCESSIBILITY_ERROR;
	      biga_status |= BIGA_ACC_FAIL;
	      if (check_for_test_stop())
	         return(sys_error_code);
	   }

  	   *tmph = 0x33333330; *ptr = 0xCCCCCCCC;
	   if ((data_read = *tmph) != 0x33333330) {
	      sys_fail_address = (uInt32) tmph;
	      sys_data_expected = 0x33333330;
	      sys_data_read = data_read;
	      sys_error_code = BIGADIAG_BIGA_ACCESSIBILITY_ERROR;
	      biga_status |= BIGA_ACC_FAIL;
	      if (check_for_test_stop())
	         return(sys_error_code);
	   }
	   if ((data_read = *ptr) != 0xCCCCCCCC) {
	      sys_fail_address = (uInt32) ptr;
	      sys_data_expected = 0xCCCCCCCC;
	      sys_data_read = data_read;
	      sys_error_code = BIGADIAG_BIGA_ACCESSIBILITY_ERROR;
	      biga_status |= BIGA_ACC_FAIL;
	      if (check_for_test_stop())
	         return(sys_error_code);
	   }
	}

	if (diag_verbosity >= VERBOSITY_LEVEL3) {
	   strcpy(sys_specific_info, "BIGA"); 
	   sprintf(cli_print_buf, test_in_progress_msg, (uInt32) lbiga_ptr,
	           diag_test_id, diag_current_runcount, sys_specific_info);
	   put_line(cli_print_buf);
	}

	for (i = 0; i < 2; i++) {

	   if (i == 0) {
	      head = (uInt32 *) &lbiga_ptr->r28.thead;
	      tail = (uInt32 *) &lbiga_ptr->r29.ttail;
	      tmph = (uInt32 *) &lbiga_ptr->r30.ttmph;
	      ptr  = (uInt32 *) &lbiga_ptr->r31.tptr;
	   }
	   else {
	      head = (uInt32 *) &lbiga_ptr->r37.rhead;
	      tail = (uInt32 *) &lbiga_ptr->r38.rtail;
	      tmph = (uInt32 *) &lbiga_ptr->r39.rtmph;
	      ptr  = (uInt32 *) &lbiga_ptr->r42.rptr;
	   }

  	   *head = 0x55540000; *tail = 0xAAA80000;
	   if ((data_read = *((uInt16 *) head)) != 0x00005554) {
	      sys_fail_address = (uInt32) head;
	      sys_data_expected = 0x00005555;
	      sys_data_read = data_read;
	      sys_error_code = BIGADIAG_BIGA_ACCESSIBILITY_ERROR;
	      biga_status |= BIGA_ACC_FAIL;
	      if (check_for_test_stop())
	         return(sys_error_code);
	   }
	   if ((data_read = *((uInt16 *) tail)) != 0x0000AAA8) {
	      sys_fail_address = (uInt32) tail;
	      sys_data_expected = 0x0000AAA8;
	      sys_data_read = data_read;
	      sys_error_code = BIGADIAG_BIGA_ACCESSIBILITY_ERROR;
	      biga_status |= BIGA_ACC_FAIL;
	      if (check_for_test_stop())
	         return(sys_error_code);
	   }

  	   *tmph = 0x33300000; *ptr = 0xCCCC0000;
	   if ((data_read = *((uInt16 *) tmph)) != 0x00003330) {
	      sys_fail_address = (uInt32) tmph;
	      sys_data_expected = 0x00003330;
	      sys_data_read = data_read;
	      sys_error_code = BIGADIAG_BIGA_ACCESSIBILITY_ERROR;
	      biga_status |= BIGA_ACC_FAIL;
	      if (check_for_test_stop())
	         return(sys_error_code);
	   }
	   if ((data_read = *((uInt16 *) ptr)) != 0x0000CCCC) {
	      sys_fail_address = (uInt32) ptr;
	      sys_data_expected = 0x0000CCCC;
	      sys_data_read = data_read;
	      sys_error_code = BIGADIAG_BIGA_ACCESSIBILITY_ERROR;
	      biga_status |= BIGA_ACC_FAIL;
	      if (check_for_test_stop())
	         return(sys_error_code);
	   }
	}

	return(BIGADIAG_OK);
}

#if  0  /* RLB  N/A for ATM */

int BIGABUFRW_Test(
	int		ex_flag)
{
extern	uInt16	biga_status;

	register int	rc;
	struct BIGA_REGS *biga_ptr = (struct BIGA_REGS *) BIGA_START;
	struct BIGA_REGS *lbiga_ptr = (struct BIGA_REGS *) LOCAL_BIGA_START;

	*((uInt32 *) COMM_STATUS) |= COMM_STATUS_STOP_QSRAM_ACCESS;

	if (diag_verbosity >= VERBOSITY_LEVEL3)
	   strcpy(sys_specific_info, "BIGA_BUF");

	if (diag_current_runcount == 1 || bigadiag_resetflag[0] == 'T') {
	   soft_reset_biga();
	}

	if (rc = MEMORY_Test(bigabufrw_parameter, 
	              (bigadiag_bigainterface[0] == '1') ? lbiga_ptr : biga_ptr,
	                 BIGABUFRW_CALL,
	                 ex_flag))
	   biga_status |= BIGA_MAC_BUF_FAIL;

	*((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_STOP_QSRAM_ACCESS;

	return(rc);
}

int BIGATXDMA_Test(
	int		ex_flag)
{
extern	uInt16	biga_status;

	int		rc;

	if (rc = BIGADMA_Test(bigatxdma_parameter, bigatxdma_results, 
	                      BIGATXDMA_CALL, ex_flag))
	   biga_status |= BIGA_TXDMA_FAIL;

	return(rc);
}

int BIGARXDMA_Test(
	int		ex_flag)
{
extern	uInt16	biga_status;

	int		rc;

	/*return(BIGADMA_Test(bigarxdma_parameter, bigarxdma_results, 
	                    BIGARXDMA_CALL, ex_flag));*/
	return(rc = 0);
}

int BIGAINTLB_Test(
	int		ex_flag)
{
extern	uInt16	biga_status;

	int		rc;

	if (rc = BIGALB_Test(bigaintlb_parameter, bigaintlb_results, 
	                     BIGAINTLB_CALL, ex_flag))
	   biga_status |= BIGA_INTLB_FAIL;

	return(rc);
}

int BIGAEXTLB_Test(
	int		ex_flag)
{
extern	uInt16	biga_status;

	int		rc;

	if (rc = BIGALB_Test(bigaextlb_parameter, bigaextlb_results, 
	                     BIGAEXTLB_CALL, ex_flag))
	   biga_status |= BIGA_EXTLB_FAIL;

	return(rc);
}

#define	MAX_LB_WAIT	100000
int BIGALOADGEN_Test(
	int		ex_flag)
{
extern	uInt16	biga_status;

	register int	m;
	register int	i, num;
	register uInt32	s_addr, e_addr, tx_buf, rx_buf, addr;
	register uInt32	p, packetsize, data, data_read;
	register uChar	cksum;
	register int	rc, runcount;
	register char	*c_mode;
	register struct MAC_REGS *mac_ptr = (struct MAC_REGS *) MAC_START;
	register struct BIGA_REGS *biga_ptr = (struct BIGA_REGS *) BIGA_START;
	register struct BIGA_REGS *lbiga_ptr = (struct BIGA_REGS *) 
	                                                       LOCAL_BIGA_START;
	register uInt16	*sist,*cmdreg1,*rpr,*wpr,*swpr,*rpxa0,*wpxa0,*swpxa0;
	register volatile uInt16 *thead_l, *thead_h, *rhead_l, *rhead_h;
	register struct BIGA_TBD *tbd;
	register struct BIGA_RBD *rbd;
	register uInt32	ttail, rtail;
	register int	biga_int32;
	register enum CALL_TYPE	call_type;
	register int	tx, rx, xcv;
	register uInt32	last_tx_time;
extern	volatile uInt32	system_tick_time;
	register int	f_flag = FALSE,
	                fbbusarb = FALSE,
	                npbusarb = FALSE,
	                nmparb = FALSE,
	                etherarb = FALSE;
	register struct	SONIC_REGS *sonic_ptr;
	register struct	TDA	   *tda_ptr;

	bigaloadgen_results[BIGALOADGEN_RUNCOUNT].display_control = OFF;

#if  0    /* RLB DBG */

	if (bigaloadgen_frequency)
	   f_flag = TRUE;
	if (diag_npbusarbitration[0] == 'T')
	   npbusarb = TRUE;
	if (diag_fbbusarbitration[0] == 'T')
	   fbbusarb = TRUE;
	if (bigadiag_nmparbitration[0] == 'T')
	   nmparb = TRUE;
	if (bigadiag_ethernetarbitration[0] == 'T')
	   etherarb = TRUE;

	if (bigadiag_bigainterface[0] == '3')
	   biga_int32 = TRUE;
	else
	   biga_int32 = FALSE;

	packetsize = bigaloadgen_packetsize;
	data = bigaloadgen_data;

	if (rc = configure_phy_in_PMD_Loopback_mode(PHY_PORT_A)) {
	   biga_status |= BIGA_LOADGEN_FAIL;
	   return(rc);
	}
	if (rc = configure_phy_in_PMD_Loopback_mode(PHY_PORT_B)) {
	   biga_status |= BIGA_LOADGEN_FAIL;
	   return(rc);
	}

	soft_reset_biga();
	reset_mac();

	configure_biga(biga_ptr, lbiga_ptr);

	tx = rx = xcv = FALSE;
	switch (bigaloadgen_loadgenmode[0]) {
	   case 'T': 
	      call_type = BIGALOADGEN_TX_CALL; tx = TRUE; break;
	   case 'R': 
	      call_type = BIGALOADGEN_RX_CALL; rx = TRUE; break;
	   default:  
	      call_type = BIGALOADGEN_CALL; xcv = TRUE; break;
	}

	if (rc = init_mac((bigadiag_bigainterface[0]=='1') ? lbiga_ptr:biga_ptr,
	                   call_type, 0)) {
	   biga_status |= BIGA_LOADGEN_FAIL;
	   return(rc);
	}

	tx_buf = TX_BUF_START;
	tbd = (struct BIGA_TBD *) TX_DESC_START;
	*((uInt32 *) TX_DESC_P_START) = (uInt32) tbd;
	rx_buf = RX_BUF_START;
	rbd = (struct BIGA_RBD *) RX_DESC_START;
	*((uInt32 *) RX_DESC_P_START) = (uInt32) rbd;
	for (num = 0; num < bigaloadgen_no_of_packets; num++,
	     tbd++, rbd++, tx_buf += TX_BUF_SIZE, rx_buf += RX_BUF_SIZE) {

	   if (!tx) {
	      rbd->rbuf_ptr = (uChar *) rx_buf;
	      rbd->rbuf_usable_len = RX_BUF_SIZE;
	      rbd->rcv_info.info_u = 0;
	      rbd->rcv_desc = 0;
	      if (num == (bigaloadgen_no_of_packets-1)) {
	         rbd->rbd_ptr = (struct BIGA_RBD *) 0;
	      }
	      else {
	         rbd->rbd_ptr = rbd+1;
	      }
	   }

	   if (!rx) {
	      if (diag_packetsizevariant[0] == 'T') {
	         packetsize = (packetsize * (num+1)) %
	                      (FDDI_MAX_PACKETSIZE+1);
	         if (packetsize < FDDI_MIN_PACKETSIZE)
	            packetsize = FDDI_MIN_PACKETSIZE;
	      }
	      if (diag_datapatternvariant[0] == 'T') {
	         data = (data * (num+1)) & 0xFF;
	      }

	      m = num%5;
	      c_mode = ((bigaloadgen_mode[0] == 'A') ?
	               (char *) get_mode(m) : bigaloadgen_mode);

	      tbd->tbuf_ptr = (uChar *) tx_buf;
	      tbd->tbd_control.control_u = 0;
	      tbd->tbd_control.control.qcode = QC_ASYNC0;
	      tbd->tbd_control.control.tbuf_len = packetsize;
	      if (num == (bigaloadgen_no_of_packets-1))
	         tbd->tbd_ptr = (struct BIGA_TBD *) 0;
	      else {
	         tbd->tbd_ptr = tbd+1;
	      }

	      *((uInt32 *) tx_buf) = (LLC_FC << 24) |
                                     (sys_dest_fddi_mac[0] << 16) |
                                     (sys_dest_fddi_mac[1] << 8) | 
	                              sys_dest_fddi_mac[2];
              *((uInt32 *) (tx_buf+4)) = (sys_dest_fddi_mac[3] << 24) |
                                         (sys_dest_fddi_mac[4] << 16) |
                                         (sys_dest_fddi_mac[5] << 8)  |
                                          sys_source_fddi_mac[0];
              *((uInt32 *) (tx_buf+8)) = (sys_source_fddi_mac[1] << 24) |
                                         (sys_source_fddi_mac[2] << 16) |
                                         (sys_source_fddi_mac[3] << 8)  |
                                          sys_source_fddi_mac[4];
              *((uChar *) (tx_buf+12)) = sys_source_fddi_mac[5];

	      sprintf(sys_specific_info, "BIGA,%u", packetsize);
	      if (packetsize > FDDI_MIN_PACKETSIZE) {
	         s_addr = tx_buf + FDDI_MIN_PACKETSIZE; 
	         e_addr = s_addr + (packetsize - FDDI_MIN_PACKETSIZE - 1);
	         if (rc = perform_write(s_addr, e_addr, data, 
	                                bigaloadgen_access, c_mode, 
	                                bigaloadgen_increment,
	                                (uInt32) 1, (uInt32) 0, (uInt32) 0, 
	                                (uInt32) 0, BIGALOADGEN_CALL)) {
	            biga_status |= BIGA_LOADGEN_FAIL;
	            return(rc);
	         }
	      }
	      for (p = cksum = 0; p < packetsize; p++) {
	         cksum += *((uChar *) (tx_buf+p));
	      }
	      *((uChar *) (tx_buf+p)) = cksum;
	   }
	}
	if (biga_int32) {
	   if (!rx) {
              *((uInt32 *) &biga_ptr->r28.thead) = TX_DESC_P_START;
              *((uInt32 *) &biga_ptr->r30.ttmph) = (uInt32) &(tbd-1)->tbd_ptr;
              *((uInt32 *) &biga_ptr->r29.ttail) = ttail = 
	                                             (uInt32) &(tbd-1)->tbd_ptr;
	   }
	   if (!tx) {
              *((uInt32 *) &biga_ptr->r37.rhead) = RX_DESC_P_START;
              *((uInt32 *) &biga_ptr->r39.rtmph) = (uInt32) &(rbd-1)->rbd_ptr;
              *((uInt32 *) &biga_ptr->r38.rtail) = rtail = 
	                                             (uInt32) &(rbd-1)->rbd_ptr;
	   }
	}
	else {
	   if (!rx) {
           *((uInt32 *) &lbiga_ptr->r28.thead) = TX_DESC_P_START << 16;
           *((uInt32 *) &lbiga_ptr->r28.thead1) = TX_DESC_P_START & 0xFFFF0000;
           *((uInt32 *) &lbiga_ptr->r30.ttmph) = 
	                                     ((uInt32) &(tbd-1)->tbd_ptr) << 16;
           *((uInt32 *) &lbiga_ptr->r30.ttmph1) = 
	                              ((uInt32) &(tbd-1)->tbd_ptr) & 0xFFFF0000;
           *((uInt32 *) &lbiga_ptr->r29.ttail) = 
	                                     ((uInt32) &(tbd-1)->tbd_ptr) << 16;
           *((uInt32 *) &lbiga_ptr->r29.ttail1) = 
	                              ((uInt32) &(tbd-1)->tbd_ptr) & 0xFFFF0000;
           ttail = (uInt32) &(tbd-1)->tbd_ptr;
	   }
	   if (!tx) {
           *((uInt32 *) &lbiga_ptr->r37.rhead) = RX_DESC_P_START << 16;
           *((uInt32 *) &lbiga_ptr->r37.rhead1) = RX_DESC_P_START & 0xFFFF0000;
           *((uInt32 *) &lbiga_ptr->r39.rtmph) = 
	                                     ((uInt32) &(rbd-1)->rbd_ptr) << 16;
           *((uInt32 *) &lbiga_ptr->r39.rtmph1) = 
	                              ((uInt32) &(rbd-1)->rbd_ptr) & 0xFFFF0000;
           *((uInt32 *) &lbiga_ptr->r38.rtail) = 
	                                     ((uInt32) &(rbd-1)->rbd_ptr) << 16;
           *((uInt32 *) &lbiga_ptr->r38.rtail1) = 
	                              ((uInt32) &(rbd-1)->rbd_ptr) & 0xFFFF0000;
           rtail = (uInt32) &(rbd-1)->rbd_ptr;
	   }
	}

	if (xcv)
  	   mac_ptr->r8.lw.mdreg1 = (MAC_ONLINE | SELRA |
	                            FULL_HALF | ADDR_DET_MODE0);
	else
  	   mac_ptr->r8.lw.mdreg1 = (MAC_ONLINE | SELRA |
	                            ADDR_DET_MODE0);

	sist = (uInt16 *) &lbiga_ptr->r19.sist;
	thead_l = (uInt16 *) &lbiga_ptr->r28.thead.low;
	thead_h = (uInt16 *) &lbiga_ptr->r28.thead1.high;
	rhead_l = (uInt16 *) &lbiga_ptr->r37.rhead.low;
	rhead_h = (uInt16 *) &lbiga_ptr->r37.rhead1.high;
	cmdreg1 = (uInt16 *) &mac_ptr->r0.lw.addr0.cmdreg1;
	rpr = (uInt16 *) &mac_ptr->r22.lw.rpr;
	wpr = (uInt16 *) &mac_ptr->r23.lw.wpr;
	swpr = (uInt16 *) &mac_ptr->r23.lw.swpr;
	rpxa0 = (uInt16 *) &mac_ptr->r28.lw.rpxa0;
	wpxa0 = (uInt16 *) &mac_ptr->r24.lw.wpxa0;
	swpxa0 = (uInt16 *) &mac_ptr->r26.lw.swpxa0;

	switch (bigaloadgen_burstsize[0]) {
	   case '2':
	      switch (bigaloadgen_loadgenmode[0]) {
	         case 'T':
                    *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP |
                                                           DMA_TXA | DMA_TXEN |
                                                           DMA_BSIZ2) << 16;
	            break;
	         case 'R':
                    *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP |
                                                           DMA_RXA | DMA_RXEN |
                                                           DMA_BSIZ2) << 16;
	            break;
	         default:
                    *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP |
                                                           DMA_TXA | DMA_TXEN |
                                                           DMA_RXA | DMA_RXEN |
                                                           DMA_BSIZ2) << 16;
	            break;
	      }
              break;
	   case '4':
	      switch (bigaloadgen_loadgenmode[0]) {
	         case 'T':
                    *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP |
                                                           DMA_TXA | DMA_TXEN |
                                                           DMA_BSIZ4) << 16;
	            break;
	         case 'R':
                    *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP |
                                                           DMA_RXA | DMA_RXEN |
                                                           DMA_BSIZ4) << 16;
	            break;
	         default:
                    *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP |
                                                           DMA_TXA | DMA_TXEN |
                                                           DMA_RXA | DMA_RXEN |
                                                           DMA_BSIZ4) << 16;
	            break;
	      }
              break;
	   case '8':
	      switch (bigaloadgen_loadgenmode[0]) {
	         case 'T':
                    *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP |
                                                           DMA_TXA | DMA_TXEN |
                                                           DMA_BSIZ8) << 16;
	            break;
	         case 'R':
                    *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP |
                                                           DMA_RXA | DMA_RXEN |
                                                           DMA_BSIZ8) << 16;
	            break;
	         default:
                    *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP |
                                                           DMA_TXA | DMA_TXEN |
                                                           DMA_RXA | DMA_RXEN |
                                                           DMA_BSIZ8) << 16;
	            break;
	      }
              break;
	   default:
	      if (bigaloadgen_burstsize[1] == '6') {
	         switch (bigaloadgen_loadgenmode[0]) {
	            case 'T':
                       *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP |
                                                              DMA_TXA|DMA_TXEN |
                                                              DMA_BSIZ16) << 16;
	               break;
	            case 'R':
                       *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP |
                                                              DMA_RXA|DMA_RXEN |
                                                              DMA_BSIZ16) << 16;
	               break;
	            default:
                       *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP |
                                                              DMA_TXA|DMA_TXEN |
                                                              DMA_RXA|DMA_RXEN |
                                                              DMA_BSIZ16) << 16;
	               break;
	         }
	      }
	      else {
	         switch (bigaloadgen_loadgenmode[0]) {
	            case 'T':
                       *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP |
                                                              DMA_TXA|DMA_TXEN |
                                                              DMA_BSIZ1) << 16;
	               break;
	            case 'R':
                       *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP |
                                                              DMA_RXA|DMA_RXEN |
                                                              DMA_BSIZ1) << 16;
	               break;
	            default:
                       *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP |
                                                              DMA_TXA|DMA_TXEN |
                                                              DMA_RXA|DMA_RXEN |
                                                              DMA_BSIZ1) << 16;
	               break;
	         }
	      }
              break;
	}

	if (etherarb) {
	   etidiag_loadgen_initonly = TRUE;
	   ETILOADGEN_Test(0);
	   etidiag_loadgen_initonly = FALSE;
	}

	if (f_flag) {
	   if (!rx) {
#ifdef RISC
	      get_systime();
#endif /* RISC */
	      last_tx_time = system_tick_time;
	   }
	}
	if (nmparb) {
	   *((volatile uInt32 *) COMM_STATUS) |= COMM_STATUS_NMP_ARBITRATION;
	}
	for (runcount = 1; runcount <= bigaloadgen_runcount; runcount++,
	     diag_current_runcount++) {

	   bigaloadgen_run_count = diag_current_runcount;

	   if (diag_verbosity > VERBOSITY_LEVEL0)
	      putchar_p('.');

	   *((uInt32 *) sist) = 0xffff << 16;
	   if (!rx) {
 	      if (data_read = (lbiga_ptr->r19.sist & SIST_XMTD)) {
                 sys_fail_address = (uInt32) &lbiga_ptr->r19.sist;
                 sys_data_expected = 0;
                 sys_data_read = data_read;
                 sys_error_code = BIGADIAG_STATUS_BIT_XMTD_STUCK_ERROR;
	         biga_status |= BIGA_LOADGEN_FAIL;
                 if (check_for_test_stop())
                    return(sys_error_code);
              }

	      /**rpxa0 = EAS_VAL_PTR_LGEN+1;
	      *wpxa0 = EAS_VAL_PTR_LGEN+1;
	      *swpxa0 = EAS_VAL_PTR_LGEN+1;*/
	   }
	   if (!tx) {
 	      if (data_read = (lbiga_ptr->r19.sist & SIST_RDSB)) {
                 sys_fail_address = (uInt32) &lbiga_ptr->r19.sist;
                 sys_data_expected = 0;
                 sys_data_read = data_read;
                 sys_error_code = BIGADIAG_STATUS_BIT_RDSB_STUCK_ERROR;
	         biga_status |= BIGA_LOADGEN_FAIL;
                 if (check_for_test_stop())
                    return(sys_error_code);
              }

	      /**rpr = EACB_VAL_PTR_LGEN+1;
	      *wpr = EACB_VAL_PTR_LGEN+1;
	      *swpr = EACB_VAL_PTR_LGEN+1;*/
	   }
	
	   if (f_flag) {
	      if (!rx) {
	         while ((last_tx_time+bigaloadgen_frequency) >
	                 system_tick_time) {
#ifdef RISC
	            get_systime();
#endif /* RISC */
	         }
	         last_tx_time = system_tick_time;
	      }
	   }

	   /**cmdreg1 = CLR_ALL_LOCKS;*/

	   if (biga_int32) {
	      if (!tx) {
                 *((uInt32 *) &biga_ptr->r37.rhead) = RX_DESC_P_START;
	         *((uInt32 *) &biga_ptr->r39.rtmph) = RX_DESC_P_START;
	      }
	      if (!rx) {
                 *((uInt32 *) &biga_ptr->r28.thead) = TX_DESC_P_START;
	         *((uInt32 *) &biga_ptr->r30.ttmph) = TX_DESC_P_START;
	      }
	   }
	   else {
	      if (!tx) {
                 *((uInt32 *) &lbiga_ptr->r37.rhead) = RX_DESC_P_START << 16;
                 *((uInt32 *) &lbiga_ptr->r37.rhead1) = 
	                                           RX_DESC_P_START & 0xFFFF0000;
	         *((uInt32 *) &lbiga_ptr->r39.rtmph) = RX_DESC_P_START << 16;
	         *((uInt32 *) &lbiga_ptr->r39.rtmph1) = 
	                                           RX_DESC_P_START & 0xFFFF0000;
	      }
	      if (!rx) {
                 *((uInt32 *) &lbiga_ptr->r28.thead) = TX_DESC_P_START << 16;
                 *((uInt32 *) &lbiga_ptr->r28.thead1) = 
	                                           TX_DESC_P_START & 0xFFFF0000;
	         *((uInt32 *) &lbiga_ptr->r30.ttmph) = TX_DESC_P_START << 16;
	         *((uInt32 *) &lbiga_ptr->r30.ttmph1) = 
	                                           TX_DESC_P_START & 0xFFFF0000;
	      }
	   }

	   if (!rx) {
 	      for (i = 0; i < MAX_LB_WAIT; i++) {
	         if (biga_int32) {
	            if ((data_read = *((volatile uInt32 *)&biga_ptr->r28.thead))
	                                                               == ttail)
                       break;
	         }
	         else {
	            if ((data_read = ((*thead_h << 16) | *thead_l)) == ttail)
	               break;
	         }
                 if (npbusarb) {
                    for (addr = QSRAM_START_ARBITRATE;
                         addr < QSRAM_END_ARBITRATE; addr += 4) {
                       *((uInt32 *) addr) = addr;
                       if ((data_read = *((uInt32 *) addr)) != addr) {
                          sys_fail_address = addr;
                          sys_data_expected = addr;
                          sys_data_read = data_read;
                          sys_error_code = BIGADIAG_QSRAM_ARBITRATION_ERROR;
                          if (check_for_test_stop())
                             return(rc = sys_error_code);
                       }
                    }   
                 }   
                 if (fbbusarb) {
                    for (addr = FRAMEBUF_START_ARBITRATE;
                         addr < FRAMEBUF_END_ARBITRATE; addr += 4) {
                       *((uInt32 *) addr) = addr;
                       if ((data_read = *((uInt32 *) addr)) != addr) {
                          sys_fail_address = addr;
                          sys_data_expected = addr;
                          sys_data_read = data_read;
                          sys_error_code = BIGADIAG_FRAMEBUF_ARBITRATION_ERROR;
                          if (check_for_test_stop())
                             return(rc = sys_error_code);
                       }
                    }   
                 }   
	         if (etherarb) {
	            for (p = 1, 
	                 sonic_ptr = (struct SONIC_REGS *) SONIC_T_1_START,
	                 tda_ptr = (struct TDA *) TDA_START_ARB; p <= 8; p++,
	                 sonic_ptr = (struct SONIC_REGS *) 
	                                      ((uInt32) sonic_ptr - 0x00800000),
	                 tda_ptr = (struct TDA *) ((uInt32) tda_ptr +
	                                        SONIC_PORT_QSRAM_REGION_SIZE)) {
	               if (!(sonic_ptr->cr & CR_TXP)) {
	                  sonic_ptr->ctda = ((uInt32) tda_ptr) & 0xFFFF;
	                  sonic_ptr->isr = 0xFFFF;
	                  sonic_ptr->cr = CR_TXP;
	               }
	            }
	         }
              }
              if (i == MAX_LB_WAIT) {
	         if (biga_int32)
                    sys_fail_address = (uInt32) &biga_ptr->r28.thead;
	         else
                    sys_fail_address = (uInt32) &lbiga_ptr->r28.thead;
                 sys_data_expected = ttail;
                 sys_data_read = data_read;
                 sys_error_code = BIGADIAG_PACKET_TRANSMIT_TIMEOUT_ERROR;
	         biga_status |= BIGA_LOADGEN_FAIL;
                 if (check_for_test_stop()) {
                    return(sys_error_code);
	         }
              }
              if (!(lbiga_ptr->r19.sist & SIST_XMTD)) {
                 sys_fail_address = (uInt32) &lbiga_ptr->r19.sist;
                 sys_data_expected = SIST_XMTD;
                 sys_data_read = 0x0000;
                 sys_error_code = BIGADIAG_STATUS_BIT_XMTD_NOT_SET_ERROR;
	         biga_status |= BIGA_LOADGEN_FAIL;
                 if (check_for_test_stop()) {
                    return(sys_error_code);
	         }
              }
	   }

	   if (!tx) {
 	      for (i = 0; i < (100*MAX_LB_WAIT); i++) {
	         if (biga_int32) {
	            if ((data_read = *((volatile uInt32 *)&biga_ptr->r37.rhead)) ==
	                                                                  rtail)
                       break;
	         }
	         else {
	            if ((data_read = ((*rhead_h << 16) | *rhead_l)) == rtail)
	               break;
	         }
	         if (rc = check_for_break())
	            return(rc);
                 if (npbusarb) {
                    for (addr = QSRAM_START_ARBITRATE;
                         addr < QSRAM_END_ARBITRATE; addr += 4) {
                       *((uInt32 *) addr) = addr;
                       if ((data_read = *((uInt32 *) addr)) != addr) {
                          sys_fail_address = addr;
                          sys_data_expected = addr;
                          sys_data_read = data_read;
                          sys_error_code = BIGADIAG_QSRAM_ARBITRATION_ERROR;
                          if (check_for_test_stop())
                             return(rc = sys_error_code);
                       }
                    }   
                 }   
                 if (fbbusarb) {
                    for (addr = FRAMEBUF_START_ARBITRATE;
                         addr < FRAMEBUF_END_ARBITRATE; addr += 4) {
                       *((uInt32 *) addr) = addr;
                       if ((data_read = *((uInt32 *) addr)) != addr) {
                          sys_fail_address = addr;
                          sys_data_expected = addr;
                          sys_data_read = data_read;
                          sys_error_code = BIGADIAG_FRAMEBUF_ARBITRATION_ERROR;
                          if (check_for_test_stop())
                             return(rc = sys_error_code);
                       }
                    }   
                 }   
	         if (etherarb) {
	            for (p = 1, 
	                 sonic_ptr = (struct SONIC_REGS *) SONIC_T_1_START,
	                 tda_ptr = (struct TDA *) TDA_START_ARB; p <= 8; p++,
	                 sonic_ptr = (struct SONIC_REGS *) 
	                                      ((uInt32) sonic_ptr - 0x00800000),
	                 tda_ptr = (struct TDA *) ((uInt32) tda_ptr +
	                                        SONIC_PORT_QSRAM_REGION_SIZE)) {
	               if (!(sonic_ptr->cr & CR_TXP)) {
	                  sonic_ptr->ctda = ((uInt32) tda_ptr) & 0xFFFF;
	                  sonic_ptr->isr = 0xFFFF;
	                  sonic_ptr->cr = CR_TXP;
	               }
	            }
	         }
              }
              if (i == (100*MAX_LB_WAIT)) {
	         if (biga_int32)
                    sys_fail_address = (uInt32) &biga_ptr->r37.rhead;
	         else
                    sys_fail_address = (uInt32) &lbiga_ptr->r37.rhead;
                 sys_data_expected = rtail;
                 sys_data_read = data_read;
                 sys_error_code = BIGADIAG_PACKET_RECEIVE_TIMEOUT_ERROR;
	         biga_status |= BIGA_LOADGEN_FAIL;
                 if (check_for_test_stop())
                    return(sys_error_code);
              }
              if (!(lbiga_ptr->r19.sist & SIST_RDSB)) {
                 sys_fail_address = (uInt32) &lbiga_ptr->r19.sist;
                 sys_data_expected = SIST_RDSB;
                 sys_data_read = 0x0000;
                 sys_error_code = BIGADIAG_STATUS_BIT_RDSB_NOT_SET_ERROR;
	         biga_status |= BIGA_LOADGEN_FAIL;
                 if (check_for_test_stop()) {
                    return(sys_error_code);
	         }
              }

	      if (xcv && bigaloadgen_validation[0] == 'T') {
	         tx_buf = TX_BUF_START;
	         rx_buf = RX_BUF_START;
	         packetsize = bigaloadgen_packetsize;
	         for (num = 0; num < bigaloadgen_no_of_packets; num++,
	              tx_buf += TX_BUF_SIZE, rx_buf += RX_BUF_SIZE) {
	            if (diag_packetsizevariant[0] == 'T') {
	               packetsize = (packetsize * (num+1)) %
	                            (FDDI_MAX_PACKETSIZE+1);
	               if (packetsize < FDDI_MIN_PACKETSIZE)
	                  packetsize = FDDI_MIN_PACKETSIZE;
	            }
	            for (p = cksum = 0; p < packetsize; p++) {
	               cksum += *((uChar *) (rx_buf+p+3));
	            }
	            *((uChar *) (rx_buf+p+3)) = cksum;
                    if (cksum != *((uChar *) (tx_buf+p))) {
                       sys_fail_address = (uInt32) (tx_buf+p);
                       sys_data_expected = cksum;
                       sys_data_read = *((uChar *) (tx_buf+p));
                       sys_error_code = BIGADIAG_PACKET_DATA_ERROR;
	               biga_status |= BIGA_LOADGEN_FAIL;
                       if (check_for_test_stop()) {
                          return(sys_error_code);
	               }
                    }
	         }
	      }
	   }

	   if (rc = check_for_break())
	      break;
	}
	if (diag_verbosity > VERBOSITY_LEVEL1)
	   putchar_p('\n');

	if (nmparb) {
	   *((volatile uInt32 *) COMM_STATUS) &= ~COMM_STATUS_NMP_ARBITRATION;
	}
	*((uInt32 *) &lbiga_ptr->r24.dctrl) = 0x0000 << 16;

	if (rc == 0)
	   diag_current_runcount--;

#endif  /* RLB DBG */

	return(rc);
}

int BIGADMA_Test(
	struct PARAMETER_TABLE	*parm_table,
	struct RESULTS_TABLE	*r_table,
	enum CALL_TYPE		call_type,
	int			ex_flag)
{
	int		queue, q, m, qlevel_all, mode_all;
	int		i, b, burst, burst_all, index;
	uInt32		addr, s_addr, e_addr, tx_buf, rx_buf;
	uInt32		packetsize, data, increment;
	register int	rc;
	char		*c_mode, *c_qlevel;
	struct MAC_REGS *mac_ptr = (struct MAC_REGS *) MAC_START;
	struct BIGA_REGS *biga_ptr = (struct BIGA_REGS *) BIGA_START;
	struct BIGA_REGS *lbiga_ptr = (struct BIGA_REGS *) LOCAL_BIGA_START;
	char		*qlevel, *access, *mode, *burstsize;
	uInt32		data_read;
	struct BIGA_TBD	*tbd;
	struct BIGA_RBD	*rbd;
	int		biga_int32;

#if  0   /* RLB DBG */

	if (bigadiag_bigainterface[0] == '3')
	   biga_int32 = TRUE;
	else
	   biga_int32 = FALSE;

	if (call_type == BIGATXDMA_CALL) {
	   qlevel = (char *) parm_table->parameter;
	   index = 1;
	}
	else {
	   index = 0;
	}

	packetsize = *((uInt32 *) (parm_table+0+index)->parameter);
	data       = *((uInt32 *) (parm_table+1+index)->parameter);
	access     = (char *)     (parm_table+2+index)->parameter;
	mode       = (char *)     (parm_table+3+index)->parameter;
	increment  = *((uInt32 *) (parm_table+4+index)->parameter);
	burstsize  = (char *)     (parm_table+5+index)->parameter;

	if (diag_packetsizevariant[0] == 'T') {
	   packetsize = (packetsize * diag_current_runcount) %
	                (FDDI_MAX_PACKETSIZE+1);
	   if (packetsize < FDDI_MIN_PACKETSIZE)
	      packetsize = FDDI_MIN_PACKETSIZE;
	   *((uInt32 *) (r_table+BIGARXDMA_PACKETSIZE+index)->result) = 
	                                                             packetsize;
	}
	if (diag_datapatternvariant[0] == 'T') {
	   data = (data * diag_current_runcount) & 0xFF;
	   *((uInt32 *) (r_table+BIGARXDMA_DATA+index)->result) = data;
	}

	if (call_type == BIGATXDMA_CALL) {
	   if (ex_flag || *qlevel == 'A')
	      qlevel_all = TRUE;
	   else
	      qlevel_all = FALSE;
	}
	else
	   qlevel_all = FALSE;

	if (ex_flag || *burstsize == 'A')
	   burst_all = TRUE;
	else
	   burst_all = FALSE;

	if (ex_flag || *mode == 'A')
	   mode_all = TRUE;
	else
	   mode_all = FALSE;

	for (queue = 0; queue < 3; queue++) {
	   if (call_type == BIGATXDMA_CALL) {
	      q = queue;
	      if (qlevel_all) {
	         (r_table+BIGATXDMA_QLEVEL)->result[0] = q + '0';
	         (r_table+BIGATXDMA_QLEVEL)->result[1] = '\0';
	      }
	      else {
	         q = *qlevel - '0';
	      }
	   }

	   for (burst = 0; burst < 5; burst++) {
	      b = burst;
	      if (burst_all) {
	         switch (b) {
	            case 0: strcpy((r_table+BIGARXDMA_BURSTSIZE+index)->result,
	                           value_burstsize_1); break;
	            case 1: strcpy((r_table+BIGARXDMA_BURSTSIZE+index)->result,
	                           value_burstsize_2); break;
	            case 2: strcpy((r_table+BIGARXDMA_BURSTSIZE+index)->result,
	                           value_burstsize_4); break;
	            case 3: strcpy((r_table+BIGARXDMA_BURSTSIZE+index)->result,
	                           value_burstsize_8); break;
	            default: strcpy((r_table+BIGARXDMA_BURSTSIZE+index)->result,
	                            value_burstsize_16); break;
	         }
	      }
	      else {
	         switch (*burstsize) {
	            case '2': b = 1; break;
	            case '4': b = 2; break;
	            case '8': b = 3; break;
	            default: if (*(burstsize+1) == '6')
	                        b = 4; 
	                     break;
	         }
	      }
	      for (m = 0; m < 5; m++) {
	         c_mode = ((mode_all) ? (char *) get_mode(m) : mode);
	         if (mode_all)
	            strcpy((r_table+BIGARXDMA_MODE+index)->result, c_mode);

	         if (diag_verbosity >= VERBOSITY_LEVEL3) {
	            strcpy(sys_specific_info, "BIGA");
	            if (call_type == BIGATXDMA_CALL) {
	               sprintf(cli_print_buf, ",A%u", q);
	               strcat(sys_specific_info, cli_print_buf);
	            }
	            sprintf(cli_print_buf, ",%u,%s", packetsize, 
	                   (r_table+BIGARXDMA_BURSTSIZE+index)->result);
	            strcat(sys_specific_info, cli_print_buf);
	         }

	         if ((diag_current_runcount == 1 && queue == 0 && 
	              burst == 0 && m == 0) ||
	              bigadiag_resetflag[0] == 'T') {
	            soft_reset_biga();
	            reset_mac();
	         }

	         configure_biga(biga_ptr, lbiga_ptr);

	         if (rc = init_mac((bigadiag_bigainterface[0] == '1') ? 
	                            lbiga_ptr : biga_ptr, call_type, 0))
	            break;

	         if (call_type == BIGATXDMA_CALL) {
	            tx_buf = TX_BUF_START;  
	            tbd = (struct BIGA_TBD *) TX_DESC_START;
	            *((uInt32 *) TX_DESC_P_START) = (uInt32) tbd;
	            tbd->tbuf_ptr = (uChar *) tx_buf;
	            tbd->tbd_control.control_u = 0;
	            tbd->tbd_control.control.qcode = q + QC_ASYNC0;
	            tbd->tbd_control.control.tbuf_len = packetsize;
	            tbd->tbd_ptr = (struct BIGA_TBD *) 0;
	            if (biga_int32) {
	               *((uInt32 *) &biga_ptr->r28.thead) = TX_DESC_P_START;
	               *((uInt32 *) &biga_ptr->r30.ttmph) = TX_DESC_P_START;
	               *((uInt32 *)&biga_ptr->r29.ttail)=(uInt32) &tbd->tbd_ptr;
	            }
	            else {
	               *((uInt32 *) &lbiga_ptr->r28.thead) = 
	                                                  TX_DESC_P_START << 16;
	               *((uInt32 *) &lbiga_ptr->r28.thead1) = 
	                                           TX_DESC_P_START & 0xFFFF0000;
	               *((uInt32 *) &lbiga_ptr->r30.ttmph) = 
	                                                  TX_DESC_P_START << 16;
	               *((uInt32 *) &lbiga_ptr->r30.ttmph1) = 
	                                           TX_DESC_P_START & 0xFFFF0000;
	               *((uInt32 *) &lbiga_ptr->r29.ttail) = 
	                                         ((uInt32) &tbd->tbd_ptr) << 16;
	               *((uInt32 *) &lbiga_ptr->r29.ttail1) = 
	                                  ((uInt32) &tbd->tbd_ptr) & 0xFFFF0000;
	            }
	            switch (q) {
	               case 0: rx_buf = (EAS_VAL_PTR + 1) << 2; break;
	               case 1: rx_buf = (EAA0_VAL_PTR + 1) << 2; break;
	               case 2: rx_buf = (EAA1_VAL_PTR + 1) << 2; break;
	            }
	         }
	         else {
	            tx_buf = (EACB_VAL_PTR + 1) << 2;
	            rx_buf = RX_BUF_START;
	            rbd = (struct BIGA_RBD *) RX_DESC_START;
	            *((uInt32 *) RX_DESC_P_START) = (uInt32) rbd;
	            rbd->rbuf_ptr = (uChar *) rx_buf;
	            rbd->rbuf_usable_len = RX_BUF_SIZE;
	            rbd->rcv_info.info_u = 0;
	            rbd->rcv_desc = 0;
	            rbd->rbd_ptr = (struct BIGA_RBD *) 0;
	            if (biga_int32) {
	               *((uInt32 *) &biga_ptr->r37.rhead) = RX_DESC_P_START;
	               *((uInt32 *) &biga_ptr->r39.rtmph) = RX_DESC_P_START;
	               *((uInt32 *)&biga_ptr->r38.rtail)=(uInt32) &rbd->rbd_ptr;
	            }
	            else {
	               *((uInt32 *) &lbiga_ptr->r37.rhead) = 
	                                                  RX_DESC_P_START << 16;
	               *((uInt32 *) &lbiga_ptr->r37.rhead1) = 
	                                           RX_DESC_P_START & 0xFFFF0000;
	               *((uInt32 *) &lbiga_ptr->r39.rtmph) = 
	                                                  RX_DESC_P_START << 16;
	               *((uInt32 *) &lbiga_ptr->r39.rtmph1) = 
	                                           RX_DESC_P_START & 0xFFFF0000;
	               *((uInt32 *) &lbiga_ptr->r38.rtail) = 
	                                         ((uInt32) &rbd->rbd_ptr) << 16;
	               *((uInt32 *) &lbiga_ptr->r38.rtail1) = 
	                                  ((uInt32) &rbd->rbd_ptr) & 0xFFFF0000;
	            }
	         }

	         if (call_type == BIGATXDMA_CALL) {
	            *((uInt32 *) tx_buf) = (LLC_FC << 24) | 
	                                   (sys_dest_fddi_mac[0] << 16) | 
	                                   (sys_dest_fddi_mac[1] << 8) | 
	                                    sys_dest_fddi_mac[2];
	            *((uInt32 *) (tx_buf+4)) = (sys_dest_fddi_mac[3] << 24) |
	                                       (sys_dest_fddi_mac[4] << 16) |
	                                       (sys_dest_fddi_mac[5] << 8)  | 
	                                        sys_source_fddi_mac[0];
	            *((uInt32 *) (tx_buf+8)) = (sys_source_fddi_mac[1] << 24) |
	                                       (sys_source_fddi_mac[2] << 16) |
	                                       (sys_source_fddi_mac[3] << 8)  | 
	                                        sys_source_fddi_mac[4];
	            *((uChar *) (tx_buf+12)) = sys_source_fddi_mac[5];
	         }
	         else {
	            mac_ptr->r30.lw.marw = (EACB_VAL_PTR + 1);
	            mac_ptr->data_reg.wd.mdru = (LLC_FC << 8) | 
	                                         sys_dest_fddi_mac[0];
	            mac_ptr->data_reg.wd.mdrl = (sys_dest_fddi_mac[1] << 8) | 
	                                         sys_dest_fddi_mac[2];
	            mac_ptr->data_reg.wd.mdru = (sys_dest_fddi_mac[3] << 8) | 
	                                         sys_dest_fddi_mac[4];
	            mac_ptr->data_reg.wd.mdrl = (sys_dest_fddi_mac[5] << 8) | 
	                                         sys_source_fddi_mac[0];
	            mac_ptr->data_reg.wd.mdru = (sys_source_fddi_mac[1] << 8) | 
	                                         sys_source_fddi_mac[2];
	            mac_ptr->data_reg.wd.mdrl = (sys_source_fddi_mac[3] << 8) | 
	                                         sys_source_fddi_mac[4];
	            mac_ptr->data_reg.wd.mdru = (sys_source_fddi_mac[5] << 8); 
	         }

	         if (packetsize > FDDI_MIN_PACKETSIZE) {
	            s_addr = tx_buf + FDDI_MIN_PACKETSIZE;
	            e_addr = s_addr + (packetsize - FDDI_MIN_PACKETSIZE - 1);
	            if (rc = perform_write(s_addr, e_addr, data, 
	                                   access, c_mode, increment,
	                                   (uInt32) 1, (uInt32) 0, (uInt32) 0, 
	                  (call_type == BIGATXDMA_CALL) ? (uInt32) 0 : mac_ptr,
	                                   call_type))
	               break;
	         }

  	         /*mac_ptr->r8.lw.mdreg1 = (INTERNAL_LOOPBACK | SELRA |
	                                  FULL_HALF | ADDR_DET_MODE0);*/
  	         mac_ptr->r8.lw.mdreg1 = (SELRA | ADDR_DET_MODE0);

	         if (call_type == BIGATXDMA_CALL) {
 	            if (data_read = (lbiga_ptr->r19.sist & SIST_XMTD)) {
              	       sys_fail_address = (uInt32) &lbiga_ptr->r19.sist;
                       sys_data_expected = 0;
                       sys_data_read = data_read;
                       sys_error_code = BIGADIAG_STATUS_BIT_XMTD_STUCK_ERROR;
                       if (check_for_test_stop())
                          return(sys_error_code);
                    }
	         }
	         else {
 	            if (data_read = (lbiga_ptr->r19.sist & SIST_RDSB)) {
              	       sys_fail_address = (uInt32) &lbiga_ptr->r19.sist;
                       sys_data_expected = 0;
                       sys_data_read = data_read;
                       sys_error_code = BIGADIAG_STATUS_BIT_RDSB_STUCK_ERROR;
                       if (check_for_test_stop())
                          return(sys_error_code);
                    }
	         }
	
	         switch (b) {
	            case 0:
	               if (call_type == BIGATXDMA_CALL)
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_TXA | DMA_TXEN |
	                                                    DMA_BSIZ1) << 16;
	               else
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_RXA | DMA_RXEN |
	                                                    DMA_BSIZ1) << 16;
	               break;
	            case 1:
	               if (call_type == BIGATXDMA_CALL)
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_TXA | DMA_TXEN |
	                                                    DMA_BSIZ2) << 16;
	               else
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_RXA | DMA_RXEN |
	                                                    DMA_BSIZ2) << 16;
	               break;
	            case 2:
	               if (call_type == BIGATXDMA_CALL)
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_TXA | DMA_TXEN |
	                                                    DMA_BSIZ4) << 16;
	               else
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_RXA | DMA_RXEN |
	                                                    DMA_BSIZ4) << 16;
	               break;
	            case 3:
	               if (call_type == BIGATXDMA_CALL)
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_TXA | DMA_TXEN |
	                                                    DMA_BSIZ8) << 16;
	               else
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_RXA | DMA_RXEN |
	                                                    DMA_BSIZ8) << 16;
	               break;
	            default:
	               if (call_type == BIGATXDMA_CALL)
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_TXA | DMA_TXEN |
	                                                    DMA_BSIZ16) << 16;
	               else
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_RXA | DMA_RXEN |
	                                                    DMA_BSIZ16) << 16;
	               break;
	         }

	         if (call_type == BIGATXDMA_CALL) {
 	            for (i = 0; i < MAX_LB_WAIT; i++) {
 	               if (diag_npbusarbitration[0] == 'T') {
                          for (addr = QSRAM_START_ARBITRATE; 
	                       addr < QSRAM_END_ARBITRATE; addr += 4) {
                             *((uInt32 *) addr) = addr;
                             if ((data_read = *((uInt32 *) addr)) != addr) {
                                sys_fail_address = addr;
                                sys_data_expected = addr;
                                sys_data_read = data_read;
                                sys_error_code = 
	                                       BIGADIAG_QSRAM_ARBITRATION_ERROR;
                                if (check_for_test_stop())
                                   return(rc = sys_error_code);
                             }
                          }               
                       }   
                       if (diag_fbbusarbitration[0] == 'T') {
                          for (addr = FRAMEBUF_START_ARBITRATE;
                               addr < FRAMEBUF_END_ARBITRATE; addr += 4) {
                             *((uInt32 *) addr) = addr;
                             if ((data_read = *((uInt32 *) addr)) != addr) {
                                sys_fail_address = addr;
                                sys_data_expected = addr;
                                sys_data_read = data_read;
                                sys_error_code = 
	                                    BIGADIAG_FRAMEBUF_ARBITRATION_ERROR;
                                if (check_for_test_stop())
                                   return(rc = sys_error_code);
                             }
                          }   
                       }   
                       if (data_read = (lbiga_ptr->r19.sist & SIST_XMTD))
                          break;
                    }
                    if (i == MAX_LB_WAIT) {
                       sys_fail_address = (uInt32) &lbiga_ptr->r19.sist;
                       sys_data_expected = SIST_XMTD;
                       sys_data_read = data_read;
                       sys_error_code = BIGADIAG_PACKET_TRANSMIT_TIMEOUT_ERROR;
                       if (check_for_test_stop()) {
                          return(sys_error_code);
	               }
                    }
	         }
	         else {
	            /* write status_word/frame_length in receive buffer */
	            mac_ptr->r30.lw.marw = (EACB_VAL_PTR + (packetsize+7)/4);
	            mac_ptr->r0.lw.addr1.cmdreg2 = SET_TAG_BIT;
	            mac_ptr->data_reg.wd.mdru = 0x8690 | ((packetsize+3)%4);
	            mac_ptr->data_reg.wd.mdrl = packetsize;

	            /* simulate frame received condition */
	            mac_ptr->r23.lw.swpr = mac_ptr->r30.lw.marw;
	            mac_ptr->r23.lw.wpr  = mac_ptr->r23.lw.swpr + 1;

 	            for (i = 0; i < MAX_LB_WAIT; i++) {
 	               if (diag_npbusarbitration[0] == 'T') {
                          for (addr = QSRAM_START_ARBITRATE; 
	                       addr < QSRAM_END_ARBITRATE; addr += 4) {
                             *((uInt32 *) addr) = addr;
                             if ((data_read = *((uInt32 *) addr)) != addr) {
                                sys_fail_address = addr;
                                sys_data_expected = addr;
                                sys_data_read = data_read;
                                sys_error_code = 
	                                       BIGADIAG_QSRAM_ARBITRATION_ERROR;
                                if (check_for_test_stop())
                                   return(rc = sys_error_code);
                             }
                          }               
                       }   
                       if (diag_fbbusarbitration[0] == 'T') {
                          for (addr = FRAMEBUF_START_ARBITRATE;
                               addr < FRAMEBUF_END_ARBITRATE; addr += 4) {
                             *((uInt32 *) addr) = addr;
                             if ((data_read = *((uInt32 *) addr)) != addr) {
                                sys_fail_address = addr;
                                sys_data_expected = addr;
                                sys_data_read = data_read;
                                sys_error_code = 
	                                    BIGADIAG_FRAMEBUF_ARBITRATION_ERROR;
                                if (check_for_test_stop())
                                   return(rc = sys_error_code);
                             }
                          }   
                       }   
                       if (data_read = (lbiga_ptr->r19.sist & SIST_RDSB))
                          break;
                    }
                    if (i == MAX_LB_WAIT) {
                       sys_fail_address = (uInt32) &lbiga_ptr->r19.sist;
                       sys_data_expected = SIST_RDSB;
                       sys_data_read = data_read;
                       sys_error_code = BIGADIAG_PACKET_RECEIVE_TIMEOUT_ERROR;
                       if (check_for_test_stop()) {
                          return(sys_error_code);
	               }
                    }
	         }

	         *((uInt32 *) &lbiga_ptr->r24.dctrl) = 0x0000 << 16;

	         if (packetsize > FDDI_MIN_PACKETSIZE) {
	            if (call_type == BIGATXDMA_CALL)
	               s_addr = rx_buf + FDDI_MIN_PACKETSIZE;
	            else
	               s_addr = rx_buf + FDDI_MIN_PACKETSIZE + 3;
	            e_addr = s_addr + (packetsize - FDDI_MIN_PACKETSIZE - 1);
	            if (rc = perform_compare(s_addr, e_addr, data, 
	                                     access, c_mode, increment,
	                                     (uInt32) 1, (uInt32) 0, (uInt32) 0,
	                  (call_type == BIGATXDMA_CALL) ? mac_ptr : (uInt32) 0,
	                                     call_type))
	               break;
	         }

	         if (!mode_all)
	            break;
	      }
	      if (!burst_all || rc)
	         break;
	      }
	   if (!qlevel_all || rc)
	      break;
	}

	if (rc == 0) {
	   if (qlevel_all) {
	      strcpy((r_table+BIGATXDMA_QLEVEL)->result, 
	             value_qlevel_all);
	   }
	   if (burst_all) {
	      strcpy((r_table+BIGARXDMA_BURSTSIZE+index)->result, 
	             value_burstsize_all);
	   }
	   if (mode_all) {
	      strcpy((r_table+BIGARXDMA_MODE+index)->result, value_mode_all);
	   }
	}
	else {
	   if (rc == DIAG_READ_WRITE_FAIL_ERROR)
	      rc = BIGADIAG_PACKET_DATA_ERROR;
	}

#endif  /* RLB DBG */

	return(rc);
}

int BIGALB_Test(
	struct PARAMETER_TABLE	*parm_table,
	struct RESULTS_TABLE	*r_table,
	enum CALL_TYPE		call_type,
	int			ex_flag)
{
	int		queue, q, m, l, qlevel_all, mode_all, lblevel_all;
	int		p, port_all, i, b, burst, burst_all, index;
	uInt32		addr, s_addr, e_addr, tx_buf, rx_buf;
	uInt32		packetsize, data, increment;
	register int	rc;
	char		*c_mode, *c_qlevel;
	struct MAC_REGS *mac_ptr = (struct MAC_REGS *) MAC_START;
	struct BIGA_REGS *biga_ptr = (struct BIGA_REGS *) BIGA_START;
	struct BIGA_REGS *lbiga_ptr = (struct BIGA_REGS *) LOCAL_BIGA_START;
	char		*qlevel, *access, *mode, *lblevel, *portnum, *burstsize;
	uInt32		data_read;
	struct BIGA_TBD	*tbd;
	struct BIGA_RBD	*rbd;
	int		biga_int32;

#if  0  /* RLB DBG */

	if (bigadiag_bigainterface[0] == '3')
	   biga_int32 = TRUE;
	else
	   biga_int32 = FALSE;


	if (call_type == BIGAINTLB_CALL)
	   index = 0;
	else
	   index = 1;

	if (call_type == BIGAEXTLB_CALL)
	   portnum = (char *)     (parm_table+0)->parameter;
	qlevel     = (char *)     (parm_table+0+index)->parameter;
	packetsize = *((uInt32 *) (parm_table+1+index)->parameter);
	data       = *((uInt32 *) (parm_table+2+index)->parameter);
	access     = (char *)     (parm_table+3+index)->parameter;
	mode       = (char *)     (parm_table+4+index)->parameter;
	increment  = *((uInt32 *) (parm_table+5+index)->parameter);
	burstsize  = (char *)     (parm_table+6+index)->parameter;
	if (call_type == BIGAEXTLB_CALL)
	   lblevel = (char *) (parm_table+8)->parameter;

	if (diag_packetsizevariant[0] == 'T') {
	   packetsize = (packetsize * diag_current_runcount) %
	                (FDDI_MAX_PACKETSIZE+1);
	   if (packetsize < FDDI_MIN_PACKETSIZE)
	      packetsize = FDDI_MIN_PACKETSIZE;
	   *((uInt32 *) (r_table+BIGAINTLB_PACKETSIZE+index)->result) = 
	                                                             packetsize;
	}
	if (diag_datapatternvariant[0] == 'T') {
	   data = (data * diag_current_runcount) & 0xFF;
	   *((uInt32 *) (r_table+BIGAINTLB_DATA+index)->result) = data;
	}

	if (call_type == BIGAEXTLB_CALL) {
	   if (ex_flag)
	      port_all = TRUE;
	   else
	      port_all = FALSE;
	   if (ex_flag || *lblevel == 'A')
	      lblevel_all = TRUE;
	   else
	      lblevel_all = FALSE;
	}
	else {
	   port_all = FALSE;
	   lblevel_all = FALSE;
	}

	if (ex_flag || *qlevel == 'A')
	   qlevel_all = TRUE;
	else
	   qlevel_all = FALSE;

	if (ex_flag || *burstsize == 'A')
	   burst_all = TRUE;
	else
	   burst_all = FALSE;

	if (ex_flag || *mode == 'A')
	   mode_all = TRUE;
	else
	   mode_all = FALSE;

	for (p = 1; p < 4; p++) {

	   if (call_type == BIGAEXTLB_CALL) {
	      if (port_all) {
	         switch(p) {
	            case 1: 
	               strcpy((r_table+BIGAEXTLB_PORTNUM)->result, 
	                      value_portnum_A);
	               break;
	            case 2: 
	               strcpy((r_table+BIGAEXTLB_PORTNUM)->result, 
	                      value_portnum_B);
	               break;
	            default: 
	               strcpy((r_table+BIGAEXTLB_PORTNUM)->result, "All");
	               break;
	         }
	      }
	      else {
	         if (*portnum == 'B')
	            p++;
	         if (!strcmp_nocase(bigaextlb_portnum, "all"))
	            p += 2;
	      }
	   }

	   for (l = 0; l < 4; l++) {

	      if (call_type == BIGAEXTLB_CALL) {
	         if (lblevel_all) {
	            switch (l) {
	               case 0: 
	                  strcpy((r_table+BIGAEXTLB_LBLEVEL)->result, 
	                         value_lblevel_LM);
	                  break;
	               case 1: 
	                  strcpy((r_table+BIGAEXTLB_LBLEVEL)->result, 
	                         value_lblevel_EB);
	                  break;
	               case 3: 
	                  strcpy((r_table+BIGAEXTLB_LBLEVEL)->result, 
	                         value_lblevel_PMD);
	                  break;
	               default: 
	                  strcpy((r_table+BIGAEXTLB_LBLEVEL)->result, 
	                         value_lblevel_XT);
	                  break;
	            }
	         }
	         else {
	            if (*lblevel == 'E')
	               l++;
	            if (*lblevel == 'X')
	               l += 2;
	            if (*lblevel == 'P')
	               l += 3;
	         }
	         if (p != 3 && l != 3) {
	            if (p == 1) {
	               if (rc = configure_phy_in_Bypass_mode(PHY_PORT_B))
	                  return(rc);
	            }
	            else {
	               if (rc = configure_phy_in_Bypass_mode(PHY_PORT_A))
	                  return(rc);
	            }
	         }
	         switch (l) {
	            case 0: 
	               if (p != 3) {
	                  if (rc = configure_phy_in_LM_Loopback_mode(p))
	                     return(rc);
	               }
	               else {
	                  if (rc=configure_phy_in_LM_Loopback_mode(PHY_PORT_A))
	                     return(rc);
	                  if (rc=configure_phy_in_LM_Loopback_mode(PHY_PORT_B))
	                     return(rc);
	               }
	               break;
	            case 1: 
	               if (p != 3) {
	                  if (rc = configure_phy_in_EB_Loopback_mode(p))
	                     return(rc);
	               }
	               else {
	                  if (rc=configure_phy_in_EB_Loopback_mode(PHY_PORT_A))
	                     return(rc);
	                  if (rc=configure_phy_in_EB_Loopback_mode(PHY_PORT_B))
	                     return(rc);
	               }
	               break;
	            default: 
	               if (l == 2) {
	                  if (p != 3) {
	                     if (rc = configure_phy_in_XT_Loopback_mode(p))
	                        return(rc);
	                  }
	                  else {
	                     if (rc = configure_phy_in_XT_Loopback_mode(
                                                                    PHY_PORT_A))
	                        return(rc);
	                     if (rc = configure_phy_in_XT_Loopback_mode(
                                                                    PHY_PORT_B))
	                        return(rc);
	                  }
	               }
	               else {
	                  if (rc = configure_phy_in_PMD_Loopback_mode(
	                                                            PHY_PORT_A))
	                     return(rc);
	                  if (rc = configure_phy_in_PMD_Loopback_mode(
	                                                            PHY_PORT_B))
	                     return(rc);
	               }
	         }
	      }

	      for (queue = 0; queue < 3; queue++) {
	         q = queue;
	         if (qlevel_all) {
	            (r_table+BIGAINTLB_QLEVEL+index)->result[0] = q + '0';
	            (r_table+BIGAINTLB_QLEVEL+index)->result[1] = '\0';
	         }
	         else {
	            q = *qlevel - '0';
	         }

	         for (burst = 0; burst < 5; burst++) {
	         b = burst;
	         if (burst_all) {
	            switch (b) {
	               case 0: strcpy((r_table+BIGAINTLB_BURSTSIZE+index)->
	                               result, 
	                              value_burstsize_1); break;
	               case 1: strcpy((r_table+BIGAINTLB_BURSTSIZE+index)->
	                               result, 
	                              value_burstsize_2); break;
	               case 2: strcpy((r_table+BIGAINTLB_BURSTSIZE+index)->
	                               result, 
	                              value_burstsize_4); break;
	               case 3: strcpy((r_table+BIGAINTLB_BURSTSIZE+index)->
	                               result, 
	                              value_burstsize_8); break;
	               default: strcpy((r_table+BIGAINTLB_BURSTSIZE+index)->
	                               result, 
	                               value_burstsize_16); break;
	            }
	         }
	         else {
	            switch (*burstsize) {
	               case '2': b = 1; break;
	               case '4': b = 2; break;
	               case '8': b = 3; break;
	               default: if (*(burstsize+1) == '6')
	                           b = 4; 
	                        break;
	            }
	         }
	         for (m = 0; m < 5; m++) {
	            c_mode = ((mode_all) ? (char *) get_mode(m) : mode);
	            if (mode_all)
	               strcpy((r_table+BIGAINTLB_MODE+index)->result, c_mode);

	            if (diag_verbosity >= VERBOSITY_LEVEL3) {
	               sprintf(sys_specific_info, "BIGA,A%u,%u,%s", q, 
	                       packetsize, 
	                      (r_table+BIGAINTLB_BURSTSIZE+index)->result);
	               if (call_type == BIGAEXTLB_CALL) {
	                  switch (p) {
	                     case 1: strcat(sys_specific_info, ",PHY_A"); break;
	                     case 2: strcat(sys_specific_info, ",PHY_B"); break;
	                     default: 
	                           strcat(sys_specific_info, ",All"); break;
	                  }
	                  switch (l) {
	                     case 0: strcat(sys_specific_info, ",LM"); break;
	                     case 1: strcat(sys_specific_info, ",EB"); break;
	                     case 3: strcat(sys_specific_info, ",PM"); break;
	                     default: strcat(sys_specific_info, ",XT"); break;
	                  }
	               }
	            }

	            if ((diag_current_runcount == 1 && queue == 0 && 
	                 burst == 0 && m == 0) ||
	                 bigadiag_resetflag[0] == 'T') {
	               soft_reset_biga();
	               reset_mac();
	            }

	            configure_biga(biga_ptr, lbiga_ptr);

	            if (rc = init_mac((bigadiag_bigainterface[0] == '1') ? 
	                               lbiga_ptr : biga_ptr, call_type, l))
	               break;

	            tx_buf = TX_BUF_START;  
	            /* initialize Tx list */
	            tbd = (struct BIGA_TBD *) TX_DESC_START;
	            *((uInt32 *) TX_DESC_P_START) = (uInt32) tbd;
	            tbd->tbuf_ptr = (uChar *) tx_buf;
	            tbd->tbd_control.control_u = 0;
	            tbd->tbd_control.control.qcode = q + QC_ASYNC0;
	            tbd->tbd_control.control.tbuf_len = packetsize;
	            tbd->tbd_ptr = (struct BIGA_TBD *) 0;
	            if (biga_int32) {
	               *((uInt32 *) &biga_ptr->r28.thead) = TX_DESC_P_START;
	               *((uInt32 *) &biga_ptr->r30.ttmph) = TX_DESC_P_START;
	               *((uInt32 *)&biga_ptr->r29.ttail)=(uInt32) &tbd->tbd_ptr;
	            }
	            else {
	               *((uInt32 *) &lbiga_ptr->r28.thead) = 
	                                                  TX_DESC_P_START << 16;
	               *((uInt32 *) &lbiga_ptr->r28.thead1) = 
	                                           TX_DESC_P_START & 0xFFFF0000;
	               *((uInt32 *) &lbiga_ptr->r30.ttmph) = 
	                                                  TX_DESC_P_START << 16;
	               *((uInt32 *) &lbiga_ptr->r30.ttmph1) = 
	                                           TX_DESC_P_START & 0xFFFF0000;
	               *((uInt32 *) &lbiga_ptr->r29.ttail) = 
	                                         ((uInt32) &tbd->tbd_ptr) << 16;
	               *((uInt32 *) &lbiga_ptr->r29.ttail1) = 
	                                  ((uInt32) &tbd->tbd_ptr) & 0xFFFF0000;
	            }

	            rx_buf = RX_BUF_START;
	            /* initialize Rx list */
	            rbd = (struct BIGA_RBD *) RX_DESC_START;
	            *((uInt32 *) RX_DESC_P_START) = (uInt32) rbd;
	            rbd->rbuf_ptr = (uChar *) rx_buf;
	            rbd->rbuf_usable_len = RX_BUF_SIZE;
	            rbd->rcv_info.info_u = 0;
	            rbd->rcv_desc = 0;
	            rbd->rbd_ptr = (struct BIGA_RBD *) 0;
	            if (biga_int32) {
	               *((uInt32 *) &biga_ptr->r37.rhead) = RX_DESC_P_START;
	               *((uInt32 *) &biga_ptr->r39.rtmph) = RX_DESC_P_START;
	               *((uInt32 *)&biga_ptr->r38.rtail)=(uInt32) &rbd->rbd_ptr;
	            }
	            else {
	               *((uInt32 *) &lbiga_ptr->r37.rhead) = 
	                                                  RX_DESC_P_START << 16;
	               *((uInt32 *) &lbiga_ptr->r37.rhead1) = 
	                                           RX_DESC_P_START & 0xFFFF0000;
	               *((uInt32 *) &lbiga_ptr->r39.rtmph) = 
	                                                  RX_DESC_P_START << 16;
	               *((uInt32 *) &lbiga_ptr->r39.rtmph1) = 
	                                           RX_DESC_P_START & 0xFFFF0000;
	               *((uInt32 *) &lbiga_ptr->r38.rtail) = 
	                                         ((uInt32) &rbd->rbd_ptr) << 16;
	               *((uInt32 *) &lbiga_ptr->r38.rtail1) = 
	                                  ((uInt32) &rbd->rbd_ptr) & 0xFFFF0000;
	            }

	            *((uInt32 *) tx_buf) = (LLC_FC << 24) | 
	                                   (sys_dest_fddi_mac[0] << 16) | 
	                                   (sys_dest_fddi_mac[1] << 8) | 
	                                    sys_dest_fddi_mac[2];
	            *((uInt32 *) (tx_buf+4)) = (sys_dest_fddi_mac[3] << 24) |
	                                       (sys_dest_fddi_mac[4] << 16) |
	                                       (sys_dest_fddi_mac[5] << 8)  | 
	                                        sys_source_fddi_mac[0];
	            *((uInt32 *) (tx_buf+8)) = (sys_source_fddi_mac[1] << 24) |
	                                       (sys_source_fddi_mac[2] << 16) |
	                                       (sys_source_fddi_mac[3] << 8)  | 
	                                        sys_source_fddi_mac[4];
	            *((uChar *) (tx_buf+12)) = sys_source_fddi_mac[5];

	            if (packetsize > FDDI_MIN_PACKETSIZE) {
	               s_addr = tx_buf + FDDI_MIN_PACKETSIZE;
	               e_addr = s_addr + (packetsize - FDDI_MIN_PACKETSIZE - 1);
	               if (rc = perform_write(s_addr, e_addr, data, 
	                        access, c_mode, increment,
	                        (uInt32) 1, (uInt32) 0, (uInt32) 0, 
	                        (uInt32) 0, call_type))
	                  break;
	            }

	            if (call_type == BIGAINTLB_CALL)
  	               mac_ptr->r8.lw.mdreg1 = (INTERNAL_LOOPBACK | SELRA |
	                                        FULL_HALF | ADDR_DET_MODE0);
	            else {
	               if (l != 3)
  	                  mac_ptr->r8.lw.mdreg1 = (EXTERNAL_LOOPBACK | SELRA |
	                                          FULL_HALF | ADDR_DET_MODE0);
	               else
  	                  mac_ptr->r8.lw.mdreg1 = (MAC_ONLINE | SELRA |
	                                          FULL_HALF | ADDR_DET_MODE0);
	            }

 	            if (data_read = (lbiga_ptr->r19.sist & SIST_XMTD)) {
              	       sys_fail_address = (uInt32) &lbiga_ptr->r19.sist;
                       sys_data_expected = 0;
                       sys_data_read = data_read;
                       sys_error_code = BIGADIAG_STATUS_BIT_XMTD_STUCK_ERROR;
                       if (check_for_test_stop())
                          return(sys_error_code);
                    }
	
 	            if (data_read = (lbiga_ptr->r19.sist & SIST_RDSB)) {
              	       sys_fail_address = (uInt32) &lbiga_ptr->r19.sist;
                       sys_data_expected = 0;
                       sys_data_read = data_read;
                       sys_error_code = BIGADIAG_STATUS_BIT_RDSB_STUCK_ERROR;
                       if (check_for_test_stop())
                          return(sys_error_code);
                    }
	
	            switch (b) {
	               case 0:
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_TXA | DMA_TXEN |
	                                                    DMA_RXA | DMA_RXEN |
	                                                    DMA_BSIZ1) << 16;
	                  break;
	               case 1:
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_TXA | DMA_TXEN |
	                                                    DMA_RXA | DMA_RXEN |
	                                                    DMA_BSIZ2) << 16;
	                  break;
	               case 2:
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_TXA | DMA_TXEN |
	                                                    DMA_RXA | DMA_RXEN |
	                                                    DMA_BSIZ4) << 16;
	                  break;
	               case 3:
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_TXA | DMA_TXEN |
	                                                    DMA_RXA | DMA_RXEN |
	                                                    DMA_BSIZ8) << 16;
	                  break;
	               default:
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_TXA | DMA_TXEN |
	                                                    DMA_RXA | DMA_RXEN |
	                                                    DMA_BSIZ16) << 16;
	                  break;
	            }

 	            for (i = 0; i < MAX_LB_WAIT; i++) {
 	               if (diag_npbusarbitration[0] == 'T') {
                          for (addr = QSRAM_START_ARBITRATE; 
	                       addr < QSRAM_END_ARBITRATE; addr += 4) {
                             *((uInt32 *) addr) = addr;
                             if ((data_read = *((uInt32 *) addr)) != addr) {
                                sys_fail_address = addr;
                                sys_data_expected = addr;
                                sys_data_read = data_read;
                                sys_error_code = 
	                                       BIGADIAG_QSRAM_ARBITRATION_ERROR;
                                if (check_for_test_stop())
                                   return(rc = sys_error_code);
                             }
                          }               
                       }   
                       if (diag_fbbusarbitration[0] == 'T') {
                          for (addr = FRAMEBUF_START_ARBITRATE;
                               addr < FRAMEBUF_END_ARBITRATE; addr += 4) {
                             *((uInt32 *) addr) = addr;
                             if ((data_read = *((uInt32 *) addr)) != addr) {
                                sys_fail_address = addr;
                                sys_data_expected = addr;
                                sys_data_read = data_read;
                                sys_error_code = 
	                                    BIGADIAG_FRAMEBUF_ARBITRATION_ERROR;
                                if (check_for_test_stop())
                                   return(rc = sys_error_code);
                             }
                          }   
                       }   
                       if (data_read = (lbiga_ptr->r19.sist & SIST_XMTD))
                          break;
                    }
                    if (i == MAX_LB_WAIT) {
                       sys_fail_address = (uInt32) &lbiga_ptr->r19.sist;
                       sys_data_expected = SIST_XMTD;
                       sys_data_read = data_read;
                       sys_error_code = BIGADIAG_PACKET_TRANSMIT_TIMEOUT_ERROR;
                       if (check_for_test_stop()) {
                          return(sys_error_code);
	               }
                    }

 	            for (i = 0; i < MAX_LB_WAIT; i++) {
 	               if (diag_npbusarbitration[0] == 'T') {
                          for (addr = QSRAM_START_ARBITRATE; 
	                       addr < QSRAM_END_ARBITRATE; addr += 4) {
                             *((uInt32 *) addr) = addr;
                             if ((data_read = *((uInt32 *) addr)) != addr) {
                                sys_fail_address = addr;
                                sys_data_expected = addr;
                                sys_data_read = data_read;
                                sys_error_code = 
	                                       BIGADIAG_QSRAM_ARBITRATION_ERROR;
                                if (check_for_test_stop())
                                   return(rc = sys_error_code);
                             }
                          }               
                       }   
                       if (diag_fbbusarbitration[0] == 'T') {
                          for (addr = FRAMEBUF_START_ARBITRATE;
                               addr < FRAMEBUF_END_ARBITRATE; addr += 4) {
                             *((uInt32 *) addr) = addr;
                             if ((data_read = *((uInt32 *) addr)) != addr) {
                                sys_fail_address = addr;
                                sys_data_expected = addr;
                                sys_data_read = data_read;
                                sys_error_code = 
	                                    BIGADIAG_FRAMEBUF_ARBITRATION_ERROR;
                                if (check_for_test_stop())
                                   return(rc = sys_error_code);
                             }
                          }   
                       }   
                       if (data_read = (lbiga_ptr->r19.sist & SIST_RDSB))
                          break;
                    }
                    if (i == MAX_LB_WAIT) {
                       sys_fail_address = (uInt32) &lbiga_ptr->r19.sist;
                       sys_data_expected = SIST_RDSB;
                       sys_data_read = data_read;
                       sys_error_code = BIGADIAG_PACKET_RECEIVE_TIMEOUT_ERROR;
                       if (check_for_test_stop()) {
                          return(sys_error_code);
	               }
                    }

	            *((uInt32 *) &lbiga_ptr->r24.dctrl) = 0x0000 << 16;

	            if (packetsize > FDDI_MIN_PACKETSIZE) {
	               s_addr = rx_buf + FDDI_MIN_PACKETSIZE + 3;
	               e_addr = s_addr + (packetsize - FDDI_MIN_PACKETSIZE - 1);
	               if (rc = perform_compare(s_addr, e_addr, data, 
	                        access, c_mode, increment,
	                        (uInt32) 1, (uInt32) 0, (uInt32) 0, 
	                        (uInt32) 0, call_type))
	                  break;
	            }

	            if (!mode_all)
	               break;
	         }
	         if (!burst_all || rc)
	            break;
	         }

	         if (!qlevel_all || rc)
	            break;
	      }

	      if (!lblevel_all || rc)
	         break;
	   }

	   if (!port_all || rc)
	      break;
	}

	if (rc == 0) {
	   if (port_all) {
	      strcpy((r_table+BIGAEXTLB_PORTNUM)->result, value_portnum_all);
	   }
	   if (lblevel_all) {
	      strcpy((r_table+BIGAEXTLB_LBLEVEL)->result, value_lblevel_all);
	   }
	   if (qlevel_all) {
	      strcpy((r_table+BIGAINTLB_QLEVEL+index)->result, 
	             value_qlevel_all);
	   }
	   if (burst_all) {
	      strcpy((r_table+BIGAINTLB_BURSTSIZE+index)->result, 
	             value_burstsize_all);
	   }
	   if (mode_all) {
	      strcpy((r_table+BIGAINTLB_MODE+index)->result, value_mode_all);
	   }
	}
	else {
	   if (rc == DIAG_READ_WRITE_FAIL_ERROR)
	      rc = BIGADIAG_PACKET_DATA_ERROR;
	}

#endif  /* RLB DBG */
	return(rc);
}

#endif   /* RLB DBG */

/*----------------------------------------------------------------------------*
 * BIGALB1_Test()
 *    Loop back test from BIGA through SAGE, Synergy Bus and back to BIGA
 *----------------------------------------------------------------------------*/
int BIGALB1_Test(
	struct PARAMETER_TABLE	*parm_table,
	struct RESULTS_TABLE	*r_table,
	enum CALL_TYPE		call_type,
	int			ex_flag)
{
	register int	rc;
	struct BIGA_REGS *biga_ptr = (struct BIGA_REGS *) BIGA_START;
	struct BIGA_REGS *lbiga_ptr = (struct BIGA_REGS *) LOCAL_BIGA_START;
	uInt32		data_read;
	struct BIGA_TBD	*tbd;
	struct BIGA_RBD	*rbd;
	int		biga_int32;

#if  0  /* take out bigatest_main */ 
        bigatest_main();    /* call up Ratinder's test routine */
#endif  /* RLB */

#if  0  /* RLB DBG */

	if (bigadiag_bigainterface[0] == '3')
	   biga_int32 = TRUE;
	else
	   biga_int32 = FALSE;


	if (call_type == BIGAINTLB_CALL)
	   index = 0;
	else
	   index = 1;

	if (call_type == BIGAEXTLB_CALL)
	   portnum = (char *)     (parm_table+0)->parameter;
	qlevel     = (char *)     (parm_table+0+index)->parameter;
	packetsize = *((uInt32 *) (parm_table+1+index)->parameter);
	data       = *((uInt32 *) (parm_table+2+index)->parameter);
	access     = (char *)     (parm_table+3+index)->parameter;
	mode       = (char *)     (parm_table+4+index)->parameter;
	increment  = *((uInt32 *) (parm_table+5+index)->parameter);
	burstsize  = (char *)     (parm_table+6+index)->parameter;
	if (call_type == BIGAEXTLB_CALL)
	   lblevel = (char *) (parm_table+8)->parameter;

	if (diag_packetsizevariant[0] == 'T') {
	   packetsize = (packetsize * diag_current_runcount) %
	                (FDDI_MAX_PACKETSIZE+1);
	   if (packetsize < FDDI_MIN_PACKETSIZE)
	      packetsize = FDDI_MIN_PACKETSIZE;
	   *((uInt32 *) (r_table+BIGAINTLB_PACKETSIZE+index)->result) = 
	                                                             packetsize;
	}
	if (diag_datapatternvariant[0] == 'T') {
	   data = (data * diag_current_runcount) & 0xFF;
	   *((uInt32 *) (r_table+BIGAINTLB_DATA+index)->result) = data;
	}

	if (call_type == BIGAEXTLB_CALL) {
	   if (ex_flag)
	      port_all = TRUE;
	   else
	      port_all = FALSE;
	   if (ex_flag || *lblevel == 'A')
	      lblevel_all = TRUE;
	   else
	      lblevel_all = FALSE;
	}
	else {
	   port_all = FALSE;
	   lblevel_all = FALSE;
	}

	if (ex_flag || *qlevel == 'A')
	   qlevel_all = TRUE;
	else
	   qlevel_all = FALSE;

	if (ex_flag || *burstsize == 'A')
	   burst_all = TRUE;
	else
	   burst_all = FALSE;

	if (ex_flag || *mode == 'A')
	   mode_all = TRUE;
	else
	   mode_all = FALSE;

	for (p = 1; p < 4; p++) {

	   if (call_type == BIGAEXTLB_CALL) {
	      if (port_all) {
	         switch(p) {
	            case 1: 
	               strcpy((r_table+BIGAEXTLB_PORTNUM)->result, 
	                      value_portnum_A);
	               break;
	            case 2: 
	               strcpy((r_table+BIGAEXTLB_PORTNUM)->result, 
	                      value_portnum_B);
	               break;
	            default: 
	               strcpy((r_table+BIGAEXTLB_PORTNUM)->result, "All");
	               break;
	         }
	      }
	      else {
	         if (*portnum == 'B')
	            p++;
	         if (!strcmp_nocase(bigaextlb_portnum, "all"))
	            p += 2;
	      }
	   }

	   for (l = 0; l < 4; l++) {

	      if (call_type == BIGAEXTLB_CALL) {
	         if (lblevel_all) {
	            switch (l) {
	               case 0: 
	                  strcpy((r_table+BIGAEXTLB_LBLEVEL)->result, 
	                         value_lblevel_LM);
	                  break;
	               case 1: 
	                  strcpy((r_table+BIGAEXTLB_LBLEVEL)->result, 
	                         value_lblevel_EB);
	                  break;
	               case 3: 
	                  strcpy((r_table+BIGAEXTLB_LBLEVEL)->result, 
	                         value_lblevel_PMD);
	                  break;
	               default: 
	                  strcpy((r_table+BIGAEXTLB_LBLEVEL)->result, 
	                         value_lblevel_XT);
	                  break;
	            }
	         }
	         else {
	            if (*lblevel == 'E')
	               l++;
	            if (*lblevel == 'X')
	               l += 2;
	            if (*lblevel == 'P')
	               l += 3;
	         }
	         if (p != 3 && l != 3) {
	            if (p == 1) {
	               if (rc = configure_phy_in_Bypass_mode(PHY_PORT_B))
	                  return(rc);
	            }
	            else {
	               if (rc = configure_phy_in_Bypass_mode(PHY_PORT_A))
	                  return(rc);
	            }
	         }
	         switch (l) {
	            case 0: 
	               if (p != 3) {
	                  if (rc = configure_phy_in_LM_Loopback_mode(p))
	                     return(rc);
	               }
	               else {
	                  if (rc=configure_phy_in_LM_Loopback_mode(PHY_PORT_A))
	                     return(rc);
	                  if (rc=configure_phy_in_LM_Loopback_mode(PHY_PORT_B))
	                     return(rc);
	               }
	               break;
	            case 1: 
	               if (p != 3) {
	                  if (rc = configure_phy_in_EB_Loopback_mode(p))
	                     return(rc);
	               }
	               else {
	                  if (rc=configure_phy_in_EB_Loopback_mode(PHY_PORT_A))
	                     return(rc);
	                  if (rc=configure_phy_in_EB_Loopback_mode(PHY_PORT_B))
	                     return(rc);
	               }
	               break;
	            default: 
	               if (l == 2) {
	                  if (p != 3) {
	                     if (rc = configure_phy_in_XT_Loopback_mode(p))
	                        return(rc);
	                  }
	                  else {
	                     if (rc = configure_phy_in_XT_Loopback_mode(
                                                                    PHY_PORT_A))
	                        return(rc);
	                     if (rc = configure_phy_in_XT_Loopback_mode(
                                                                    PHY_PORT_B))
	                        return(rc);
	                  }
	               }
	               else {
	                  if (rc = configure_phy_in_PMD_Loopback_mode(
	                                                            PHY_PORT_A))
	                     return(rc);
	                  if (rc = configure_phy_in_PMD_Loopback_mode(
	                                                            PHY_PORT_B))
	                     return(rc);
	               }
	         }
	      }

	      for (queue = 0; queue < 3; queue++) {
	         q = queue;
	         if (qlevel_all) {
	            (r_table+BIGAINTLB_QLEVEL+index)->result[0] = q + '0';
	            (r_table+BIGAINTLB_QLEVEL+index)->result[1] = '\0';
	         }
	         else {
	            q = *qlevel - '0';
	         }

	         for (burst = 0; burst < 5; burst++) {
	         b = burst;
	         if (burst_all) {
	            switch (b) {
	               case 0: strcpy((r_table+BIGAINTLB_BURSTSIZE+index)->
	                               result, 
	                              value_burstsize_1); break;
	               case 1: strcpy((r_table+BIGAINTLB_BURSTSIZE+index)->
	                               result, 
	                              value_burstsize_2); break;
	               case 2: strcpy((r_table+BIGAINTLB_BURSTSIZE+index)->
	                               result, 
	                              value_burstsize_4); break;
	               case 3: strcpy((r_table+BIGAINTLB_BURSTSIZE+index)->
	                               result, 
	                              value_burstsize_8); break;
	               default: strcpy((r_table+BIGAINTLB_BURSTSIZE+index)->
	                               result, 
	                               value_burstsize_16); break;
	            }
	         }
	         else {
	            switch (*burstsize) {
	               case '2': b = 1; break;
	               case '4': b = 2; break;
	               case '8': b = 3; break;
	               default: if (*(burstsize+1) == '6')
	                           b = 4; 
	                        break;
	            }
	         }
	         for (m = 0; m < 5; m++) {
	            c_mode = ((mode_all) ? (char *) get_mode(m) : mode);
	            if (mode_all)
	               strcpy((r_table+BIGAINTLB_MODE+index)->result, c_mode);

	            if (diag_verbosity >= VERBOSITY_LEVEL3) {
	               sprintf(sys_specific_info, "BIGA,A%u,%u,%s", q, 
	                       packetsize, 
	                      (r_table+BIGAINTLB_BURSTSIZE+index)->result);
	               if (call_type == BIGAEXTLB_CALL) {
	                  switch (p) {
	                     case 1: strcat(sys_specific_info, ",PHY_A"); break;
	                     case 2: strcat(sys_specific_info, ",PHY_B"); break;
	                     default: 
	                           strcat(sys_specific_info, ",All"); break;
	                  }
	                  switch (l) {
	                     case 0: strcat(sys_specific_info, ",LM"); break;
	                     case 1: strcat(sys_specific_info, ",EB"); break;
	                     case 3: strcat(sys_specific_info, ",PM"); break;
	                     default: strcat(sys_specific_info, ",XT"); break;
	                  }
	               }
	            }

	            if ((diag_current_runcount == 1 && queue == 0 && 
	                 burst == 0 && m == 0) ||
	                 bigadiag_resetflag[0] == 'T') {
	               soft_reset_biga();
	               reset_mac();
	            }

	            configure_biga(biga_ptr, lbiga_ptr);

	            if (rc = init_mac((bigadiag_bigainterface[0] == '1') ? 
	                               lbiga_ptr : biga_ptr, call_type, l))
	               break;

	            tx_buf = TX_BUF_START;  
	            /* initialize Tx list */
	            tbd = (struct BIGA_TBD *) TX_DESC_START;
	            *((uInt32 *) TX_DESC_P_START) = (uInt32) tbd;
	            tbd->tbuf_ptr = (uChar *) tx_buf;
	            tbd->tbd_control.control_u = 0;
	            tbd->tbd_control.control.qcode = q + QC_ASYNC0;
	            tbd->tbd_control.control.tbuf_len = packetsize;
	            tbd->tbd_ptr = (struct BIGA_TBD *) 0;
	            if (biga_int32) {
	               *((uInt32 *) &biga_ptr->r28.thead) = TX_DESC_P_START;
	               *((uInt32 *) &biga_ptr->r30.ttmph) = TX_DESC_P_START;
	               *((uInt32 *)&biga_ptr->r29.ttail)=(uInt32) &tbd->tbd_ptr;
	            }
	            else {
	               *((uInt32 *) &lbiga_ptr->r28.thead) = 
	                                                  TX_DESC_P_START << 16;
	               *((uInt32 *) &lbiga_ptr->r28.thead1) = 
	                                           TX_DESC_P_START & 0xFFFF0000;
	               *((uInt32 *) &lbiga_ptr->r30.ttmph) = 
	                                                  TX_DESC_P_START << 16;
	               *((uInt32 *) &lbiga_ptr->r30.ttmph1) = 
	                                           TX_DESC_P_START & 0xFFFF0000;
	               *((uInt32 *) &lbiga_ptr->r29.ttail) = 
	                                         ((uInt32) &tbd->tbd_ptr) << 16;
	               *((uInt32 *) &lbiga_ptr->r29.ttail1) = 
	                                  ((uInt32) &tbd->tbd_ptr) & 0xFFFF0000;
	            }

	            rx_buf = RX_BUF_START;
	            /* initialize Rx list */
	            rbd = (struct BIGA_RBD *) RX_DESC_START;
	            *((uInt32 *) RX_DESC_P_START) = (uInt32) rbd;
	            rbd->rbuf_ptr = (uChar *) rx_buf;
	            rbd->rbuf_usable_len = RX_BUF_SIZE;
	            rbd->rcv_info.info_u = 0;
	            rbd->rcv_desc = 0;
	            rbd->rbd_ptr = (struct BIGA_RBD *) 0;
	            if (biga_int32) {
	               *((uInt32 *) &biga_ptr->r37.rhead) = RX_DESC_P_START;
	               *((uInt32 *) &biga_ptr->r39.rtmph) = RX_DESC_P_START;
	               *((uInt32 *)&biga_ptr->r38.rtail)=(uInt32) &rbd->rbd_ptr;
	            }
	            else {
	               *((uInt32 *) &lbiga_ptr->r37.rhead) = 
	                                                  RX_DESC_P_START << 16;
	               *((uInt32 *) &lbiga_ptr->r37.rhead1) = 
	                                           RX_DESC_P_START & 0xFFFF0000;
	               *((uInt32 *) &lbiga_ptr->r39.rtmph) = 
	                                                  RX_DESC_P_START << 16;
	               *((uInt32 *) &lbiga_ptr->r39.rtmph1) = 
	                                           RX_DESC_P_START & 0xFFFF0000;
	               *((uInt32 *) &lbiga_ptr->r38.rtail) = 
	                                         ((uInt32) &rbd->rbd_ptr) << 16;
	               *((uInt32 *) &lbiga_ptr->r38.rtail1) = 
	                                  ((uInt32) &rbd->rbd_ptr) & 0xFFFF0000;
	            }

	            *((uInt32 *) tx_buf) = (LLC_FC << 24) | 
	                                   (sys_dest_fddi_mac[0] << 16) | 
	                                   (sys_dest_fddi_mac[1] << 8) | 
	                                    sys_dest_fddi_mac[2];
	            *((uInt32 *) (tx_buf+4)) = (sys_dest_fddi_mac[3] << 24) |
	                                       (sys_dest_fddi_mac[4] << 16) |
	                                       (sys_dest_fddi_mac[5] << 8)  | 
	                                        sys_source_fddi_mac[0];
	            *((uInt32 *) (tx_buf+8)) = (sys_source_fddi_mac[1] << 24) |
	                                       (sys_source_fddi_mac[2] << 16) |
	                                       (sys_source_fddi_mac[3] << 8)  | 
	                                        sys_source_fddi_mac[4];
	            *((uChar *) (tx_buf+12)) = sys_source_fddi_mac[5];

	            if (packetsize > FDDI_MIN_PACKETSIZE) {
	               s_addr = tx_buf + FDDI_MIN_PACKETSIZE;
	               e_addr = s_addr + (packetsize - FDDI_MIN_PACKETSIZE - 1);
	               if (rc = perform_write(s_addr, e_addr, data, 
	                        access, c_mode, increment,
	                        (uInt32) 1, (uInt32) 0, (uInt32) 0, 
	                        (uInt32) 0, call_type))
	                  break;
	            }

	            if (call_type == BIGAINTLB_CALL)
  	               mac_ptr->r8.lw.mdreg1 = (INTERNAL_LOOPBACK | SELRA |
	                                        FULL_HALF | ADDR_DET_MODE0);
	            else {
	               if (l != 3)
  	                  mac_ptr->r8.lw.mdreg1 = (EXTERNAL_LOOPBACK | SELRA |
	                                          FULL_HALF | ADDR_DET_MODE0);
	               else
  	                  mac_ptr->r8.lw.mdreg1 = (MAC_ONLINE | SELRA |
	                                          FULL_HALF | ADDR_DET_MODE0);
	            }

 	            if (data_read = (lbiga_ptr->r19.sist & SIST_XMTD)) {
              	       sys_fail_address = (uInt32) &lbiga_ptr->r19.sist;
                       sys_data_expected = 0;
                       sys_data_read = data_read;
                       sys_error_code = BIGADIAG_STATUS_BIT_XMTD_STUCK_ERROR;
                       if (check_for_test_stop())
                          return(sys_error_code);
                    }
	
 	            if (data_read = (lbiga_ptr->r19.sist & SIST_RDSB)) {
              	       sys_fail_address = (uInt32) &lbiga_ptr->r19.sist;
                       sys_data_expected = 0;
                       sys_data_read = data_read;
                       sys_error_code = BIGADIAG_STATUS_BIT_RDSB_STUCK_ERROR;
                       if (check_for_test_stop())
                          return(sys_error_code);
                    }
	
	            switch (b) {
	               case 0:
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_TXA | DMA_TXEN |
	                                                    DMA_RXA | DMA_RXEN |
	                                                    DMA_BSIZ1) << 16;
	                  break;
	               case 1:
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_TXA | DMA_TXEN |
	                                                    DMA_RXA | DMA_RXEN |
	                                                    DMA_BSIZ2) << 16;
	                  break;
	               case 2:
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_TXA | DMA_TXEN |
	                                                    DMA_RXA | DMA_RXEN |
	                                                    DMA_BSIZ4) << 16;
	                  break;
	               case 3:
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_TXA | DMA_TXEN |
	                                                    DMA_RXA | DMA_RXEN |
	                                                    DMA_BSIZ8) << 16;
	                  break;
	               default:
	                  *((uInt32 *) &lbiga_ptr->r24.dctrl) = (DMA_DUMP | 
	                                                    DMA_TXA | DMA_TXEN |
	                                                    DMA_RXA | DMA_RXEN |
	                                                    DMA_BSIZ16) << 16;
	                  break;
	            }

 	            for (i = 0; i < MAX_LB_WAIT; i++) {
 	               if (diag_npbusarbitration[0] == 'T') {
                          for (addr = QSRAM_START_ARBITRATE; 
	                       addr < QSRAM_END_ARBITRATE; addr += 4) {
                             *((uInt32 *) addr) = addr;
                             if ((data_read = *((uInt32 *) addr)) != addr) {
                                sys_fail_address = addr;
                                sys_data_expected = addr;
                                sys_data_read = data_read;
                                sys_error_code = 
	                                       BIGADIAG_QSRAM_ARBITRATION_ERROR;
                                if (check_for_test_stop())
                                   return(rc = sys_error_code);
                             }
                          }               
                       }   
                       if (diag_fbbusarbitration[0] == 'T') {
                          for (addr = FRAMEBUF_START_ARBITRATE;
                               addr < FRAMEBUF_END_ARBITRATE; addr += 4) {
                             *((uInt32 *) addr) = addr;
                             if ((data_read = *((uInt32 *) addr)) != addr) {
                                sys_fail_address = addr;
                                sys_data_expected = addr;
                                sys_data_read = data_read;
                                sys_error_code = 
	                                    BIGADIAG_FRAMEBUF_ARBITRATION_ERROR;
                                if (check_for_test_stop())
                                   return(rc = sys_error_code);
                             }
                          }   
                       }   
                       if (data_read = (lbiga_ptr->r19.sist & SIST_XMTD))
                          break;
                    }
                    if (i == MAX_LB_WAIT) {
                       sys_fail_address = (uInt32) &lbiga_ptr->r19.sist;
                       sys_data_expected = SIST_XMTD;
                       sys_data_read = data_read;
                       sys_error_code = BIGADIAG_PACKET_TRANSMIT_TIMEOUT_ERROR;
                       if (check_for_test_stop()) {
                          return(sys_error_code);
	               }
                    }

 	            for (i = 0; i < MAX_LB_WAIT; i++) {
 	               if (diag_npbusarbitration[0] == 'T') {
                          for (addr = QSRAM_START_ARBITRATE; 
	                       addr < QSRAM_END_ARBITRATE; addr += 4) {
                             *((uInt32 *) addr) = addr;
                             if ((data_read = *((uInt32 *) addr)) != addr) {
                                sys_fail_address = addr;
                                sys_data_expected = addr;
                                sys_data_read = data_read;
                                sys_error_code = 
	                                       BIGADIAG_QSRAM_ARBITRATION_ERROR;
                                if (check_for_test_stop())
                                   return(rc = sys_error_code);
                             }
                          }               
                       }   
                       if (diag_fbbusarbitration[0] == 'T') {
                          for (addr = FRAMEBUF_START_ARBITRATE;
                               addr < FRAMEBUF_END_ARBITRATE; addr += 4) {
                             *((uInt32 *) addr) = addr;
                             if ((data_read = *((uInt32 *) addr)) != addr) {
                                sys_fail_address = addr;
                                sys_data_expected = addr;
                                sys_data_read = data_read;
                                sys_error_code = 
	                                    BIGADIAG_FRAMEBUF_ARBITRATION_ERROR;
                                if (check_for_test_stop())
                                   return(rc = sys_error_code);
                             }
                          }   
                       }   
                       if (data_read = (lbiga_ptr->r19.sist & SIST_RDSB))
                          break;
                    }
                    if (i == MAX_LB_WAIT) {
                       sys_fail_address = (uInt32) &lbiga_ptr->r19.sist;
                       sys_data_expected = SIST_RDSB;
                       sys_data_read = data_read;
                       sys_error_code = BIGADIAG_PACKET_RECEIVE_TIMEOUT_ERROR;
                       if (check_for_test_stop()) {
                          return(sys_error_code);
	               }
                    }

	            *((uInt32 *) &lbiga_ptr->r24.dctrl) = 0x0000 << 16;

	            if (packetsize > FDDI_MIN_PACKETSIZE) {
	               s_addr = rx_buf + FDDI_MIN_PACKETSIZE + 3;
	               e_addr = s_addr + (packetsize - FDDI_MIN_PACKETSIZE - 1);
	               if (rc = perform_compare(s_addr, e_addr, data, 
	                        access, c_mode, increment,
	                        (uInt32) 1, (uInt32) 0, (uInt32) 0, 
	                        (uInt32) 0, call_type))
	                  break;
	            }

	            if (!mode_all)
	               break;
	         }
	         if (!burst_all || rc)
	            break;
	         }

	         if (!qlevel_all || rc)
	            break;
	      }

	      if (!lblevel_all || rc)
	         break;
	   }

	   if (!port_all || rc)
	      break;
	}

	if (rc == 0) {
	   if (port_all) {
	      strcpy((r_table+BIGAEXTLB_PORTNUM)->result, value_portnum_all);
	   }
	   if (lblevel_all) {
	      strcpy((r_table+BIGAEXTLB_LBLEVEL)->result, value_lblevel_all);
	   }
	   if (qlevel_all) {
	      strcpy((r_table+BIGAINTLB_QLEVEL+index)->result, 
	             value_qlevel_all);
	   }
	   if (burst_all) {
	      strcpy((r_table+BIGAINTLB_BURSTSIZE+index)->result, 
	             value_burstsize_all);
	   }
	   if (mode_all) {
	      strcpy((r_table+BIGAINTLB_MODE+index)->result, value_mode_all);
	   }
	}
	else {
	   if (rc == DIAG_READ_WRITE_FAIL_ERROR)
	      rc = BIGADIAG_PACKET_DATA_ERROR;
	}

#endif  /* RLB DBG */
	return(rc);
}

/*----------------------------------------------------------------------------*
 * soft_reset_biga()
 *----------------------------------------------------------------------------*/
void soft_reset_biga()
{
	register int	i;
	struct BIGA_REGS *lbiga_ptr = (struct BIGA_REGS *) LOCAL_BIGA_START;

        *(char *)0x38000003 = 1;     /* release biga reset */
	*(char *)0x38000005 = 1;     /* release NTC reset */

	*((uInt16 *) &lbiga_ptr->r0.npctl16) = 0x200 ;
	for (i = 0; i < 4000; i++);

	*((uInt16 *) &lbiga_ptr->r0.npctl16)=(NPCTL_RESET | 0x200); 

	*((uInt16 *)&lbiga_ptr->r0.npctl16) = NPCTL_BYPN ;  
	for (i = 0; i < 10; i++);   

	*((uInt16 *)&lbiga_ptr->r0.npctl16) = (NPCTL_RESET | NPCTL_BYPN | 
	                          NPCTL_THHLC | NPCTL_RHHLC | NPCTL_IMOD) ;

	for (i = 0; i < 4000; i++);
}

/*----------------------------------------------------------------------------*
 * configure_biga()
 *----------------------------------------------------------------------------*/
void configure_biga(
	struct BIGA_REGS *biga_ptr,
	struct BIGA_REGS *lbiga_ptr)
{
	uChar		dummy;

	if (bigadiag_bigainterface[0] == '3')
	   biga_ptr->r60.cfg = 0x02020202;
	else
	   *((uInt32 *) &lbiga_ptr->r60.cfg) = 0x0202 << 16;
	
	*((uInt32 *) &lbiga_ptr->r19.sist) = 0xffff << 16;
	*((uInt32 *) &lbiga_ptr->r23.upad) = 0xffff << 16;
	*((uInt32 *) &lbiga_ptr->r20.hica) = 0x0000 << 16;
	*((uInt32 *) &lbiga_ptr->r21.hicb) = 0x0000 << 16;
	*((uInt32 *) &lbiga_ptr->r22.hicc) = 0x00 << 16;
	*((uInt32 *) &lbiga_ptr->r44.fltr) = 0xff << 16;
	*((uInt32 *) &lbiga_ptr->r24.dctrl) = 0x0000 << 16;
	dummy = lbiga_ptr->r25.dstat;
	*((uInt32 *) &lbiga_ptr->r26.dctrl2) = 0x00 << 16;
}

void set_mac_buf_ptrs(
	uInt32		start_address,
	uInt32		end_address)
{
	struct MAC_REGS *mac_ptr = (struct MAC_REGS *) MAC_START;
	register uInt32	startp, endp;

	startp = start_address >> 2;
	endp = end_address >> 2;

	mac_ptr->r8.lw.mdreg1 = INITIALIZE_MAC;
        mac_ptr->r17.lw.eacb = endp;
        mac_ptr->r17.lw.earv = endp;
        mac_ptr->r18.lw.eas = endp;
        mac_ptr->r18.lw.eaa0 = endp;
        mac_ptr->r19.lw.eaa1 = endp;
        mac_ptr->r19.lw.eaa2 = endp;   
        mac_ptr->r21.lw.wpxsf = startp;
        mac_ptr->r21.lw.rpxsf = startp;

	mac_ptr->r8.lw.mdreg1 = MEMORY_ACTIVE;
	mac_ptr->r16.lw.mdreg2 = BMMODE | ENHSRQ;
}

