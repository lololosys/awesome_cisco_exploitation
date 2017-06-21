/* $Id: memdiag.c,v 1.1.4.1 1996/05/09 15:01:12 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/memdiag.c,v $
 *------------------------------------------------------------------
 * memdiag.c -- Memory Diagnostics Section
 *
 * ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: memdiag.c,v $
 * Revision 1.1.4.1  1996/05/09  15:01:12  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:10  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:34  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:20  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:54:06  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:39  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************/
/*                                                                    */
/* CALYPSO DIAGNOSTICS TEST SUITES                                    */
/*                                                                    */
/* This file contains procedures related to "Memory Diagnostics       */
/* Section"  that are included as part of Calypso Diagnostics Test    */
/* Suites.                                                            */
/**********************************************************************/

#include	"types.h"
#include	"constant.h"
#include	"struct.h"
#include	"globdef.h"
#include	"localdef.h"
#include	"hw.h"
#include	"eeprom.h"
#include	"clock.h"
#include	"errcode.h"

#define  ATM

extern	char		cli_print_buf[], sys_specific_info[];
extern	char		test_in_progress_msg[];
extern	char		diag_datapatternvariant[], *diag_test_id;
extern	uInt32		diag_verbosity;
extern	uInt32		diag_current_runcount;
extern	uInt32		sys_fail_address;
extern	uInt32		sys_data_expected;
extern	uInt32		sys_data_read;
extern	Int32		sys_error_code;

#ifndef RISC
extern	int		cntl_reg_1_shadow, cntl_reg_2_shadow;
#endif /* RISC */

#ifndef  ATM
extern	uInt32		result_macbufrw_data;
extern	char		result_macbufrw_access[], result_macbufrw_mode[];
#endif   /* ATM */

#ifdef RISC
extern	uInt32		result_bigabufrw_data;
extern	char		result_bigabufrw_access[], result_bigabufrw_mode[];
#endif /* RISC */

/* shadow image for serial EEPROM */
char  ctrl_reg10_img=0;
char  ctrl_reg11_img=0;
char  ctrl_reg12_img=0;
char  ctrl_reg13_img=0;

#define	TOGGLE_CLOCK	ctrl_reg12_img |= SERIAL_EEPROM_SK_MASK; \
	                *(char *)CNTL_REG_12 = ctrl_reg12_img; \
	                ctrl_reg12_img &= ~SERIAL_EEPROM_SK_MASK; \
	                *(char *)CNTL_REG_12 = ctrl_reg12_img

#define	SELECT_EEPROM	ctrl_reg11_img |= SERIAL_EEPROM_CS_MASK; \
	   	        *(char *)CNTL_REG_11 = ctrl_reg11_img

#define	DESELECT_EEPROM	ctrl_reg11_img &= ~SERIAL_EEPROM_CS_MASK; \
	                *(char *)CNTL_REG_11 = ctrl_reg11_img

#define	RESET_DI	ctrl_reg10_img &= ~SERIAL_EEPROM_DI_MASK; \
	                *(char *)CNTL_REG_10 = ctrl_reg10_img

#define	SET_DI		ctrl_reg10_img |= SERIAL_EEPROM_DI_MASK; \
	                *(char *)CNTL_REG_10 = ctrl_reg10_img

	struct pmd_data eeprom_data_w, eeprom_data_r;

	char		databus_test_progress[] =
	" Data-bus Test @%08lX with %s access ... {%s} -> %lu [%s]\n";
	char		addrbus_test_progress[] =
	" Address-bus Test @%08lX with %s access ... {%s} -> %lu [%s]\n";
	char		faccess_test_progress[] =
	" Fast-access Test @%08lX with %s access ... {%s} -> %lu [%s]\n";

   char	memdiag_description[] = "Memory Diagnostics Section";
	uInt32		memdiag_runcount = 1;
	uInt32		memdiag_maxerrorcount = 0;
	uInt32		memdiag_timedelaycount = 0;
	char		memdiag_databus_test[6] = "True";
	char		memdiag_addrbus_test[6] = "True";
	char		memdiag_faccess_test[6] = "True";
	char		memdiag_cell_test[6] = "True";

#ifndef RISC
	char		bootrom_description[] = 
	"Boot ROM Checksum Test";
	uInt32		bootrom_start_address = BOOTROM_START;
	uInt32		bootrom_end_address = BOOTROM_END;
	char		bootrom_access[5] = "Byte";
	char		bootrom_time_of_run[18];
	Int32		bootrom_error_code;
	uInt32		result_bootrom_start_address;
	uInt32		result_bootrom_end_address;
	char		result_bootrom_access[5];
	uInt32		bootrom_run_count;
	uInt32		bootrom_test_fail_count;
	uInt32		bootrom_error_count;
	uInt32		bootrom_fail_address;
	uInt32		bootrom_data_expected;
	uInt32		bootrom_data_read;
	uInt32		bootrom_checksum_expected;
	uInt32		bootrom_checksum_read;

	char		eeprom_description[] = 
	"Serial EEPROM Read/Write Test";
	uInt32		eeprom_start_address = EEPROM_START_L;
	uInt32		eeprom_end_address = EEPROM_END_L;
	uInt32		eeprom_data = 0x1234;
	char		eeprom_access[5] = "Word";
	char		eeprom_mode[11] = "Random";
	uInt32		eeprom_increment = 0x0001;
	char		eeprom_typeofaccess[6] = "Read";
	char		eeprom_time_of_run[18];
	Int32		eeprom_error_code;
	uInt32		result_eeprom_start_address;
	uInt32		result_eeprom_end_address;
	uInt32		result_eeprom_data;
	char		result_eeprom_access[5];
	char		result_eeprom_mode[11];
	uInt32		result_eeprom_increment;
	char		result_eeprom_typeofaccess[6];
	uInt32		eeprom_run_count;
	uInt32		eeprom_test_fail_count;
	uInt32		eeprom_error_count;
	uInt32		eeprom_fail_address;
	uInt32		eeprom_data_expected;
	uInt32		eeprom_data_read;
	uInt32		eeprom_checksum_expected;
	uInt32		eeprom_checksum_read;

	char		nmpflash_description[] = 
	"NMP Flash Memory Read-Only Test";
	uInt32		nmpflash_start_address = FLASH_START;
	uInt32		nmpflash_end_address = FLASH_END;
	char		nmpflash_access[5] = "Byte";
	char		nmpflash_time_of_run[18];
	Int32		nmpflash_error_code;
	uInt32		result_nmpflash_start_address;
	uInt32		result_nmpflash_end_address;
	char		result_nmpflash_access[5];
	uInt32		nmpflash_run_count;
	uInt32		nmpflash_test_fail_count;
	uInt32		nmpflash_error_count;
	uInt32		nmpflash_fail_address;
	uInt32		nmpflash_data_expected;
	uInt32		nmpflash_data_read;
	uInt32		nmpflash_checksum_expected;
	uInt32		nmpflash_checksum_read;
#endif /* RISC */


        /* TX SAR */
	char		txsarram_description[] = 
	"TX SAR Shared memory Read/Write Test";
	uInt32		txsarram_start_address = TXSAR_RAM_START;
	uInt32		txsarram_end_address = TXSAR_RAM_END;
	uInt32		txsarram_data = 0x12345678;
	char		txsarram_access[5] = "Long";
	char		txsarram_mode[11] = "Random";
	uInt32		txsarram_increment = 0x00000001;
	char		txsarram_time_of_run[18];
	Int32		txsarram_error_code;
	uInt32		result_txsarram_start_address;
	uInt32		result_txsarram_end_address;
	uInt32		result_txsarram_data;
	char		result_txsarram_access[5];
	char		result_txsarram_mode[11];
	uInt32		result_txsarram_increment;
	uInt32		txsarram_run_count;
	uInt32		txsarram_test_fail_count;
	uInt32		txsarram_error_count;
	uInt32		txsarram_fail_address;
	uInt32		txsarram_data_expected;
	uInt32		txsarram_data_read;
	uInt32		txsarram_checksum_expected;
	uInt32		txsarram_checksum_read;

        /* RX SAR */
	char		rxsarram_description[] = 
	"RX SAR Shared memory Read/Write Test";
	uInt32		rxsarram_start_address = RXSAR_RAM_START;
	uInt32		rxsarram_end_address = RXSAR_RAM_END;
	uInt32		rxsarram_data = 0x12345678;
	char		rxsarram_access[5] = "Long";
	char		rxsarram_mode[11] = "Random";
	uInt32		rxsarram_increment = 0x00000001;
	char		rxsarram_time_of_run[18];
	Int32		rxsarram_error_code;
	uInt32		result_rxsarram_start_address;
	uInt32		result_rxsarram_end_address;
	uInt32		result_rxsarram_data;
	char		result_rxsarram_mode[11];
	char		result_rxsarram_access[5];
	uInt32		result_rxsarram_increment;
	uInt32		rxsarram_run_count;
	uInt32		rxsarram_test_fail_count;
	uInt32		rxsarram_error_count;
	uInt32		rxsarram_fail_address;
	uInt32		rxsarram_data_expected;
	uInt32		rxsarram_data_read;
	uInt32		rxsarram_checksum_expected;
	uInt32		rxsarram_checksum_read;


#ifndef RISC
	char		nvram_description[] = 
	"Non-Volatile Memory Read/Write Test";
	uInt32		nvram_start_address = NVRAM_START;
	uInt32		nvram_end_address = NVRAM_END;
	uInt32		nvram_data = 0x12345678;
	char		nvram_access[5] = "Long";
	char		nvram_mode[11] = "Random";
	uInt32		nvram_increment = 0x00000001;
	char		nvram_typeofaccess[6] = "Read";
	char		nvram_time_of_run[18];
	Int32		nvram_error_code;
	uInt32		result_nvram_start_address;
	uInt32		result_nvram_end_address;
	uInt32		result_nvram_data;
	char		result_nvram_access[5];
	char		result_nvram_mode[11];
	uInt32		result_nvram_increment;
	char		result_nvram_typeofaccess[6];
	uInt32		nvram_run_count;
	uInt32		nvram_test_fail_count;
	uInt32		nvram_error_count;
	uInt32		nvram_fail_address;
	uInt32		nvram_data_expected;
	uInt32		nvram_data_read;
	uInt32		nvram_checksum_expected;
	uInt32		nvram_checksum_read;
#endif /* RISC */

#ifdef RISC
	char		sramrw_description[] = 
	"Static Memory Read/Write Test";
	uInt32		sramrw_start_address = SRAM_START;
	uInt32		sramrw_end_address = SRAM_END_X;
	uInt32		sramrw_data = 0x12345678;
	char		sramrw_access[5] = "Long";
	char		sramrw_mode[11] = "Random";
	uInt32		sramrw_increment = 0x00000001;
	char		sramrw_time_of_run[18];
	Int32		sramrw_error_code;
	uInt32		result_sramrw_start_address;
	uInt32		result_sramrw_end_address;
	uInt32		result_sramrw_data;
	char		result_sramrw_access[5];
	char		result_sramrw_mode[11];
	uInt32		result_sramrw_increment;
	uInt32		sramrw_run_count;
	uInt32		sramrw_test_fail_count;
	uInt32		sramrw_error_count;
	uInt32		sramrw_fail_address;
	uInt32		sramrw_data_expected;
	uInt32		sramrw_data_read;
#endif /* RISC */

	char		dramrw_description[] = 
	"Dynamic Memory Read/Write Test";
	uInt32		dramrw_start_address = DRAM_START_X;
	uInt32		dramrw_end_address = DRAM_END_X;
	uInt32		dramrw_data = 0x12345678;
	char		dramrw_access[5] = "Long";
	char		dramrw_mode[11] = "Random";
	uInt32		dramrw_increment = 0x00000001;
	char		dramrw_time_of_run[18];
	Int32		dramrw_error_code;
	uInt32		result_dramrw_start_address;
	uInt32		result_dramrw_end_address;
	uInt32		result_dramrw_data;
	char		result_dramrw_access[5];
	char		result_dramrw_mode[11];
	uInt32		result_dramrw_increment;
	uInt32		dramrw_run_count;
	uInt32		dramrw_test_fail_count;
	uInt32		dramrw_error_count;
	uInt32		dramrw_fail_address;
	uInt32		dramrw_data_expected;
	uInt32		dramrw_data_read;

	char		dpramrw_description[] = 
	"Dual-Port Memory Read/Write Test";
	uInt32		dpramrw_start_address = DPRAM_START;
	uInt32		dpramrw_end_address = DPRAM_END;
	uInt32		dpramrw_data = 0x12;
	char		dpramrw_access[5] = "Byte";
	char		dpramrw_mode[11] = "Random";
	uInt32		dpramrw_increment = 0x00000001;
	char		dpramrw_time_of_run[18];
	Int32		dpramrw_error_code;
	uInt32		result_dpramrw_start_address;
	uInt32		result_dpramrw_end_address;
	uInt32		result_dpramrw_data;
	char		result_dpramrw_access[5];
	char		result_dpramrw_mode[11];
	uInt32		result_dpramrw_increment;
	uInt32		dpramrw_run_count;
	uInt32		dpramrw_test_fail_count;
	uInt32		dpramrw_error_count;
	uInt32		dpramrw_fail_address;
	uInt32		dpramrw_data_expected;
	uInt32		dpramrw_data_read;

	char		framebufrw_description[] = 
	"Frame Buffer Memory Read/Write Test";
	uInt32		framebufrw_start_address = FRAMEBUF_START;
	uInt32		framebufrw_end_address = FRAMEBUF_END;
	uInt32		framebufrw_data = 0x12345678;
	char		framebufrw_access[5] = "Long";
	char		framebufrw_mode[11] = "Random";
	uInt32		framebufrw_increment = 0x00000001;
	char		framebufrw_time_of_run[18];
	Int32		framebufrw_error_code;
	uInt32		result_framebufrw_start_address;
	uInt32		result_framebufrw_end_address;
	uInt32		result_framebufrw_data;
	char		result_framebufrw_access[5];
	char		result_framebufrw_mode[11];
	uInt32		result_framebufrw_increment;
	uInt32		framebufrw_run_count;
	uInt32		framebufrw_test_fail_count;
	uInt32		framebufrw_error_count;
	uInt32		framebufrw_fail_address;
	uInt32		framebufrw_data_expected;
	uInt32		framebufrw_data_read;

	struct PARAMETER_TABLE	memdiag_parameter[] =
	{ { (char *) &memdiag_runcount, parm_runcount, fmt_uInt32,
	    1, 1, 99999999, NULL, 0 },
	  { (char *) &memdiag_maxerrorcount, parm_maxerrorcount, fmt_uInt32,
	    0, 0, 99999999, NULL, 0 },
	  { (char *) &memdiag_timedelaycount, parm_timedelaycount, fmt_uInt32,
	    0, 0, 99999999, NULL, 0 },
	  { (char *)  memdiag_databus_test, parm_databus_test, fmt_string,
	    (uInt32) value_flag_true, 0, 0, value_flag, 0 },
	  { (char *)  memdiag_addrbus_test, parm_addrbus_test, fmt_string,
	    (uInt32) value_flag_true, 0, 0, value_flag, 0 },
	  { (char *)  memdiag_faccess_test, parm_faccess_test, fmt_string,
	    (uInt32) value_flag_true, 0, 0, value_flag, 0 },
	  { (char *)  memdiag_cell_test, parm_cell_test, fmt_string,
	    (uInt32) value_flag_true, 0, 0, value_flag, 0 },
	  { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };

#ifndef RISC
	struct PARAMETER_TABLE	bootrom_parameter[] =
	{ { (char *) &bootrom_start_address, parm_start_address, fmt_xInt32,
	    BOOTROM_START, BOOTROM_START, BOOTROM_START, NULL, 0 },
	  { (char *) &bootrom_end_address, parm_end_address, fmt_xInt32,
	    BOOTROM_END, BOOTROM_END, BOOTROM_END, NULL, 0 },
	  { (char *)  bootrom_access, parm_access, fmt_string,
	    (uInt32) value_access_byte, 0, 0, value_access_byte_only, 0 },
	  { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };

	struct PARAMETER_TABLE	eeprom_parameter[] =
	{ { (char *) &eeprom_start_address, parm_start_address, fmt_xInt08,
	    EEPROM_START_L, EEPROM_START_L, EEPROM_START_L, NULL, 0 },
	  { (char *) &eeprom_end_address, parm_end_address, fmt_xInt08,
	    EEPROM_END_L, EEPROM_END_L, EEPROM_END_L, NULL, 0 },
	  { (char *) &eeprom_data, parm_data, fmt_xInt16,
	    0x1234, 0x0000, 0xFFFF, NULL, 0 },
	  { (char *)  eeprom_access, parm_access, fmt_string,
	    (uInt32) value_access_word, 0, 0, value_access_word_only, 0 },
	  { (char *)  eeprom_mode, parm_mode, fmt_string,
	    (uInt32) value_mode_random, 0, 0, 
	    "/Normal/Increment/Complement/Shift/Random/", 0 },
	  { (char *) &eeprom_increment, parm_increment, fmt_xInt16,
	    0x0001, 0x0001, 0xFFFF, NULL, 0 },
	  { (char *)  eeprom_typeofaccess, parm_typeofaccess, fmt_string,
	    (uInt32) value_typeofaccess_read, 0, 0, value_typeofaccess, 0 },
	  { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };

	struct PARAMETER_TABLE	nmpflash_parameter[] =
	{ { (char *) &nmpflash_start_address, parm_start_address, fmt_xInt32,
	    FLASH_START, FLASH_START, FLASH_START, NULL, 0 },
	  { (char *) &nmpflash_end_address, parm_end_address, fmt_xInt32,
	    FLASH_END, FLASH_END, FLASH_END, NULL, 0 },
	  { (char *)  nmpflash_access, parm_access, fmt_string,
	    (uInt32) value_access_byte, 0, 0, value_access_byte_only, 0 },
	  { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };
#endif /* RISC */

   /*-----------------------------------------------------------*
    *  TX SAR Parameter Table 
    *-----------------------------------------------------------*/
	struct PARAMETER_TABLE	txsarram_parameter[] =
	{ { (char *) &txsarram_start_address, parm_start_address, fmt_xInt32,
	    TXSAR_RAM_START, TXSAR_RAM_START, TXSAR_RAM_END, NULL, 0 },
	  { (char *) &txsarram_end_address, parm_end_address, fmt_xInt32,
	    TXSAR_RAM_END, TXSAR_RAM_START, TXSAR_RAM_END, NULL, 0 },
	  { (char *) &txsarram_data, parm_data, fmt_xInt32,
	    0x12345678, 0x00000000, 0xFFFFFFFF, NULL, 0 },
	  { (char *)txsarram_access, parm_access, fmt_string,
	    (uInt32) value_access_long, 0, 0, value_access, 0 },
	  { (char *)txsarram_mode, parm_mode, fmt_string,
	    (uInt32) value_mode_random, 0, 0, value_mode, 0 },
	  { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };

	/*-----------------------------------------------------------*
  	 *  RX SAR Parameter Table 
	 *-----------------------------------------------------------*/
	struct PARAMETER_TABLE	rxsarram_parameter[] =
	{ { (char *) &rxsarram_start_address, parm_start_address, fmt_xInt32,
	    RXSAR_RAM_START, RXSAR_RAM_START, RXSAR_RAM_END, NULL, 0 },
	  { (char *) &rxsarram_end_address, parm_end_address, fmt_xInt32,
	    RXSAR_RAM_END, RXSAR_RAM_START, RXSAR_RAM_END, NULL, 0 },
	  { (char *) &rxsarram_data, parm_data, fmt_xInt32,
	    0x12345678, 0x00000000, 0xFFFFFFFF, NULL, 0 },
	  { (char *)rxsarram_access, parm_access, fmt_string,
	    (uInt32) value_access_long, 0, 0, value_access, 0 },
	  { (char *)rxsarram_mode, parm_mode, fmt_string,
	    (uInt32) value_mode_random, 0, 0, value_mode, 0 },
	  { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };


	struct PARAMETER_TABLE	nvram_parameter[] =
	{ { (char *) &nvram_start_address, parm_start_address, fmt_xInt32,
	    NVRAM_START, NVRAM_START, NVRAM_END, NULL, 0 },
	  { (char *) &nvram_end_address, parm_end_address, fmt_xInt32,
	    NVRAM_END, NVRAM_START, NVRAM_END, NULL, 0 },
	  { (char *) &nvram_data, parm_data, fmt_xInt32,
	    0x12345678, 0x00000000, 0xFFFFFFFF, NULL, 0 },
	  { (char *)  nvram_access, parm_access, fmt_string,
	    (uInt32) value_access_long, 0, 0, value_access, 0 },
	  { (char *)  nvram_mode, parm_mode, fmt_string,
	    (uInt32) value_mode_random, 0, 0, value_mode, 0 },
	  { (char *) &nvram_increment, parm_increment, fmt_xInt32,
	    0x00000001, 0x00000001, 0xFFFFFFFF, NULL, 0 },
	  { (char *)  nvram_typeofaccess, parm_typeofaccess, fmt_string,
	    (uInt32) value_typeofaccess_read, 0, 0, value_typeofaccess, 0 },
	  { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };

	struct PARAMETER_TABLE	dramrw_parameter[] =
	{ { (char *) &dramrw_start_address, parm_start_address, fmt_xInt32,
	    DRAM_START_X, DRAM_START_X, DRAM_END_X, NULL, 0 },
	  { (char *) &dramrw_end_address, parm_end_address, fmt_xInt32,
	    DRAM_END_X, DRAM_START_X, DRAM_END_X, NULL, 0 },
	  { (char *) &dramrw_data, parm_data, fmt_xInt32,
	    0x12345678, 0x00000000, 0xFFFFFFFF, NULL, 0 },
	  { (char *)  dramrw_access, parm_access, fmt_string,
	    (uInt32) value_access_long, 0, 0, value_access, 0 },
	  { (char *)  dramrw_mode, parm_mode, fmt_string,
	    (uInt32) value_mode_random, 0, 0, value_mode, 0 },
	  { (char *) &dramrw_increment, parm_increment, fmt_xInt32,
	    0x00000001, 0x00000001, 0xFFFFFFFF, NULL, 0 },
	  { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };

	struct PARAMETER_TABLE	dpramrw_parameter[] =
	{ { (char *) &dpramrw_start_address, parm_start_address, fmt_xInt32,
	    DPRAM_START, DPRAM_START, DPRAM_END, NULL, 0 },
	  { (char *) &dpramrw_end_address, parm_end_address, fmt_xInt32,
	    DPRAM_END, DPRAM_START, DPRAM_END, NULL, 0 },
	  { (char *) &dpramrw_data, parm_data, fmt_xInt32,
	    0x12345678, 0x00000000, 0xFFFFFFFF, NULL, 0 },
	  { (char *)  dpramrw_access, parm_access, fmt_string,
	    (uInt32) value_access_long, 0, 0, value_access, 0 },
	  { (char *)  dpramrw_mode, parm_mode, fmt_string,
	    (uInt32) value_mode_random, 0, 0, value_mode, 0 },
	  { (char *) &dpramrw_increment, parm_increment, fmt_xInt32,
	    0x00000001, 0x00000001, 0xFFFFFFFF, NULL, 0 },
	  { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };

	struct PARAMETER_TABLE	framebufrw_parameter[] =
	{ { (char *) &framebufrw_start_address, parm_start_address, fmt_xInt32,
	    FRAMEBUF_START, FRAMEBUF_START, FRAMEBUF_END, NULL, 0 },
	  { (char *) &framebufrw_end_address, parm_end_address, fmt_xInt32,
	    FRAMEBUF_END, FRAMEBUF_START, FRAMEBUF_END, NULL, 0 },
	  { (char *) &framebufrw_data, parm_data, fmt_xInt32,
	    0x12345678, 0x00000000, 0xFFFFFFFF, NULL, 0 },
	  { (char *)  framebufrw_access, parm_access, fmt_string,
	    (uInt32) value_access_long, 0, 0, value_access, 0 },
	  { (char *)  framebufrw_mode, parm_mode, fmt_string,
	    (uInt32) value_mode_random, 0, 0, value_mode, 0 },
	  { (char *) &framebufrw_increment, parm_increment, fmt_xInt32,
	    0x00000001, 0x00000001, 0xFFFFFFFF, NULL, 0 },
	  { NULL, NULL, NULL, 0, 0, 0, NULL, 0 } };

#ifndef RISC
	struct RESULTS_TABLE	bootrom_results[] =
	{ { (char *)  bootrom_description, result_description, fmt_string, 
	    OFF },
	  { (char *)  bootrom_time_of_run, result_time_of_run, fmt_string, 
	    OFF },
	  { (char *) &bootrom_error_code, result_error_code, res_fmt_Int32, 
	    OFF },
	  { (char *) &result_bootrom_start_address, result_start_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &result_bootrom_end_address, result_end_address, 
	    res_fmt_xInt32, OFF },
	  { (char *)  result_bootrom_access, result_access, res_fmt_string, 
	    OFF },
	  { (char *) &bootrom_run_count, result_run_count, res_fmt_uInt32, 
	    OFF },
	  { (char *) &bootrom_test_fail_count, result_test_fail_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &bootrom_error_count, result_error_count, res_fmt_uInt32, 
	    OFF },
	  { (char *) &bootrom_fail_address, result_fail_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &bootrom_data_expected, result_data_expected, 
	    res_fmt_xInt32, OFF },
	  { (char *) &bootrom_data_read, result_data_read, res_fmt_xInt32, 
	    OFF },
	  { (char *) &bootrom_checksum_expected, result_checksum_expected, 
	    res_fmt_xInt08, OFF },
	  { (char *) &bootrom_checksum_read, result_checksum_read, 
	    res_fmt_xInt08, OFF },
	  { NULL, NULL, NULL, OFF } };

	struct RESULTS_TABLE	eeprom_results[] =
	{ { (char *)  eeprom_description, result_description, fmt_string, 
	    OFF },
	  { (char *)  eeprom_time_of_run, result_time_of_run, fmt_string, 
	    OFF },
	  { (char *) &eeprom_error_code, result_error_code, res_fmt_Int32, 
	    OFF },
	  { (char *) &result_eeprom_start_address, result_start_address, 
	    res_fmt_xInt08, OFF },
	  { (char *) &result_eeprom_end_address, result_end_address, 
	    res_fmt_xInt08, OFF },
	  { (char *) &result_eeprom_data, result_data, res_fmt_xInt16, OFF },
	  { (char *)  result_eeprom_access, result_access, res_fmt_string, 
	    OFF },
	  { (char *)  result_eeprom_mode, result_mode, res_fmt_string, OFF },
	  { (char *) &result_eeprom_increment, result_increment, res_fmt_xInt16,
	    OFF },
	  { (char *)  result_eeprom_typeofaccess, result_typeofaccess, 
	    res_fmt_string, OFF },
	  { (char *) &eeprom_run_count, result_run_count, res_fmt_uInt32, 
	    OFF },
	  { (char *) &eeprom_test_fail_count, result_test_fail_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &eeprom_error_count, result_error_count, res_fmt_uInt32, 
	    OFF },
	  { (char *) &eeprom_fail_address, result_fail_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &eeprom_data_expected, result_data_expected, 
	    res_fmt_xInt32, OFF },
	  { (char *) &eeprom_data_read, result_data_read, res_fmt_xInt32, 
	    OFF },
	  { (char *) &eeprom_checksum_expected, result_checksum_expected, 
	    res_fmt_xInt16, OFF },
	  { (char *) &eeprom_checksum_read, result_checksum_read, 
	    res_fmt_xInt16, OFF },
	  { NULL, NULL, NULL, OFF } };

	struct RESULTS_TABLE	nmpflash_results[] =
	{ { (char *)  nmpflash_description, result_description, fmt_string, 
	    OFF },
	  { (char *)  nmpflash_time_of_run, result_time_of_run, fmt_string, 
	    OFF },
	  { (char *) &nmpflash_error_code, result_error_code, res_fmt_Int32, 
	    OFF },
	  { (char *) &result_nmpflash_start_address, result_start_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &result_nmpflash_end_address, result_end_address, 
	    res_fmt_xInt32, OFF },
	  { (char *)  result_nmpflash_access, result_access, res_fmt_string, 
	    OFF },
	  { (char *) &nmpflash_run_count, result_run_count, res_fmt_uInt32, 
	    OFF },
	  { (char *) &nmpflash_test_fail_count, result_test_fail_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &nmpflash_error_count, result_error_count, res_fmt_uInt32, 
	    OFF },
	  { (char *) &nmpflash_fail_address, result_fail_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &nmpflash_data_expected, result_data_expected, 
	    res_fmt_xInt32, OFF },
	  { (char *) &nmpflash_data_read, result_data_read, res_fmt_xInt32, 
	    OFF },
	  { (char *) &nmpflash_checksum_expected, result_checksum_expected, 
	    res_fmt_xInt08, OFF },
	  { (char *) &nmpflash_checksum_read, result_checksum_read, 
	    res_fmt_xInt08, OFF },
	  { NULL, NULL, NULL, OFF } };
#endif /* RISC */

        /*--- TX SAR DRAM R/W Test Results ---*/
	struct RESULTS_TABLE	txsarram_results[] =
	{ { (char *)txsarram_description, result_description, fmt_string, 
	    OFF },
	  { (char *)txsarram_time_of_run, result_time_of_run, fmt_string, 
	    OFF },
	  { (char *)&txsarram_error_code, result_error_code, res_fmt_Int32, 
	    OFF },
	  { (char *)&result_txsarram_start_address, result_start_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &result_txsarram_end_address, result_end_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &result_txsarram_data, result_data, res_fmt_xInt32, OFF },
	  { (char *)result_txsarram_access, result_access, res_fmt_string, 
	    OFF },
	  { (char *)result_txsarram_mode, result_mode, res_fmt_string, OFF },
	  { (char *)&result_txsarram_increment, result_increment, res_fmt_xInt32,
	    OFF },
	  { (char *) &txsarram_run_count, result_run_count, res_fmt_uInt32, OFF },
	  { (char *) &txsarram_test_fail_count, result_test_fail_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &txsarram_error_count, result_error_count, res_fmt_uInt32, 
	    OFF },
	  { (char *) &txsarram_fail_address, result_fail_address, res_fmt_xInt32, 
	    OFF },
	  { (char *) &txsarram_data_expected, result_data_expected, 
	    res_fmt_xInt32, OFF },
	  { (char *) &txsarram_data_read, result_data_read, res_fmt_xInt32, OFF },
	  { NULL, NULL, NULL, OFF } };

		/*--- RX SAR DRAM R/W Test Results ---*/
	struct RESULTS_TABLE	rxsarram_results[] =
	{ { (char *)rxsarram_description, result_description, fmt_string, 
	    OFF },
	  { (char *)rxsarram_time_of_run, result_time_of_run, fmt_string, 
	    OFF },
	  { (char *)&rxsarram_error_code, result_error_code, res_fmt_Int32, 
	    OFF },
	  { (char *)&result_rxsarram_start_address, result_start_address, 
	    res_fmt_xInt32, OFF },
	  { (char *)&result_rxsarram_end_address, result_end_address, 
	    res_fmt_xInt32, OFF },
	  { (char *)&result_rxsarram_data, result_data, res_fmt_xInt32, OFF },
	  { (char *)result_rxsarram_access, result_access, res_fmt_string, 
	    OFF },
	  { (char *)result_rxsarram_mode, result_mode, res_fmt_string, OFF },
	  { (char *)&result_rxsarram_increment, result_increment, res_fmt_xInt32,
	    OFF },
	  { (char *)&rxsarram_run_count, result_run_count, res_fmt_uInt32, OFF },
	  { (char *)&rxsarram_test_fail_count, result_test_fail_count, 
	    res_fmt_uInt32, OFF },
	  { (char *)&rxsarram_error_count, result_error_count, res_fmt_uInt32, 
	    OFF },
	  { (char *)&rxsarram_fail_address, result_fail_address, res_fmt_xInt32, 
	    OFF },
	  { (char *)&rxsarram_data_expected, result_data_expected, 
	    res_fmt_xInt32, OFF },
	  { (char *)&rxsarram_data_read, result_data_read, res_fmt_xInt32, OFF },
	  { NULL, NULL, NULL, OFF } };

#ifndef RISC
	struct RESULTS_TABLE	nvram_results[] =
	{ { (char *)  nvram_description, result_description, fmt_string, 
	    OFF },
	  { (char *)  nvram_time_of_run, result_time_of_run, fmt_string, 
	    OFF },
	  { (char *) &nvram_error_code, result_error_code, res_fmt_Int32, 
	    OFF },
	  { (char *) &result_nvram_start_address, result_start_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &result_nvram_end_address, result_end_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &result_nvram_data, result_data, res_fmt_xInt32, OFF },
	  { (char *)  result_nvram_access, result_access, res_fmt_string, 
	    OFF },
	  { (char *)  result_nvram_mode, result_mode, res_fmt_string, OFF },
	  { (char *) &result_nvram_increment, result_increment, 
	    res_fmt_xInt32, OFF },
	  { (char *)  result_nvram_typeofaccess, result_typeofaccess, 
	    res_fmt_string, OFF },
	  { (char *) &nvram_run_count, result_run_count, res_fmt_uInt32, 
	    OFF },
	  { (char *) &nvram_test_fail_count, result_test_fail_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &nvram_error_count, result_error_count, res_fmt_uInt32, 
	    OFF },
	  { (char *) &nvram_fail_address, result_fail_address, res_fmt_xInt32,
	    OFF },
	  { (char *) &nvram_data_expected, result_data_expected, 
	    res_fmt_xInt32, OFF },
	  { (char *) &nvram_data_read, result_data_read, res_fmt_xInt32, 
	    OFF },
	  { (char *) &nvram_checksum_expected, result_checksum_expected, 
	    res_fmt_xInt16, OFF },
	  { (char *) &nvram_checksum_read, result_checksum_read, 
	    res_fmt_xInt16, OFF },
	  { NULL, NULL, NULL, OFF } };
#endif /* RISC */


	struct RESULTS_TABLE	dramrw_results[] =
	{ { (char *)  dramrw_description, result_description, fmt_string, 
	    OFF },
	  { (char *)  dramrw_time_of_run, result_time_of_run, fmt_string, 
	    OFF },
	  { (char *) &dramrw_error_code, result_error_code, res_fmt_Int32, 
	    OFF },
	  { (char *) &result_dramrw_start_address, result_start_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &result_dramrw_end_address, result_end_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &result_dramrw_data, result_data, res_fmt_xInt32, OFF },
	  { (char *)  result_dramrw_access, result_access, res_fmt_string, 
	    OFF },
	  { (char *)  result_dramrw_mode, result_mode, res_fmt_string, OFF },
	  { (char *) &result_dramrw_increment, result_increment, res_fmt_xInt32,
	    OFF },
	  { (char *) &dramrw_run_count, result_run_count, res_fmt_uInt32, OFF },
	  { (char *) &dramrw_test_fail_count, result_test_fail_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &dramrw_error_count, result_error_count, res_fmt_uInt32, 
	    OFF },
	  { (char *) &dramrw_fail_address, result_fail_address, res_fmt_xInt32, 
	    OFF },
	  { (char *) &dramrw_data_expected, result_data_expected, 
	    res_fmt_xInt32, OFF },
	  { (char *) &dramrw_data_read, result_data_read, res_fmt_xInt32, OFF },
	  { NULL, NULL, NULL, OFF } };

	struct RESULTS_TABLE	dpramrw_results[] =
	{ { (char *)dpramrw_description, result_description, fmt_string, 
	    OFF },
	  { (char *)dpramrw_time_of_run, result_time_of_run, fmt_string, 
	    OFF },
	  { (char *)&dpramrw_error_code, result_error_code, res_fmt_Int32, 
	    OFF },
	  { (char *)&result_dpramrw_start_address, result_start_address, 
	    res_fmt_xInt32, OFF },
	  { (char *)&result_dpramrw_end_address, result_end_address, 
	    res_fmt_xInt32, OFF },
	  { (char *)&result_dpramrw_data, result_data, res_fmt_xInt32, OFF },
	  { (char *)result_dpramrw_access, result_access, res_fmt_string, 
	    OFF },
	  { (char *)result_dpramrw_mode, result_mode, res_fmt_string, OFF },
	  { (char *)&result_dpramrw_increment, result_increment, 
	    res_fmt_xInt32, OFF },
	  { (char *)&dpramrw_run_count, result_run_count, res_fmt_uInt32, 
	    OFF },
	  { (char *)&dpramrw_test_fail_count, result_test_fail_count, 
	    res_fmt_uInt32, OFF },
	  { (char *)&dpramrw_error_count, result_error_count, res_fmt_uInt32, 
	    OFF },
	  { (char *)&dpramrw_fail_address, result_fail_address, res_fmt_xInt32,
	    OFF },
	  { (char *)&dpramrw_data_expected, result_data_expected, 
	    res_fmt_xInt32, OFF },
	  { (char *)&dpramrw_data_read, result_data_read, res_fmt_xInt32, 
	    OFF },
	  { NULL, NULL, NULL, OFF } };

	struct RESULTS_TABLE	framebufrw_results[] =
	{ { (char *)  framebufrw_description, result_description, fmt_string, 
	    OFF },
	  { (char *)  framebufrw_time_of_run, result_time_of_run, fmt_string, 
	    OFF },
	  { (char *) &framebufrw_error_code, result_error_code, res_fmt_Int32, 
	    OFF },
	  { (char *) &result_framebufrw_start_address, result_start_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &result_framebufrw_end_address, result_end_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &result_framebufrw_data, result_data, res_fmt_xInt32, 
	    OFF },
	  { (char *)  result_framebufrw_access, result_access, res_fmt_string, 
	    OFF },
	  { (char *)  result_framebufrw_mode, result_mode, res_fmt_string, 
	    OFF },
	  { (char *) &result_framebufrw_increment, result_increment, 
	    res_fmt_xInt32, OFF },
	  { (char *) &framebufrw_run_count, result_run_count, res_fmt_uInt32, 
	    OFF },
	  { (char *) &framebufrw_test_fail_count, result_test_fail_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &framebufrw_error_count, result_error_count, 
	    res_fmt_uInt32, OFF },
	  { (char *) &framebufrw_fail_address, result_fail_address, 
	    res_fmt_xInt32, OFF },
	  { (char *) &framebufrw_data_expected, result_data_expected, 
	    res_fmt_xInt32, OFF },
	  { (char *) &framebufrw_data_read, result_data_read, res_fmt_xInt32, 
	    OFF },
	  { NULL, NULL, NULL, OFF } };

	int		BOOTROM_Test();
	int		EEPROM_Test();
	int		NMPFLASH_Test();

	int             TXSARRAM_Test();
	int             RXSARRAM_Test();

#ifndef RISC
	int		NVRAM_Test();
#endif /* RISC */
	int		DRAMRW_Test();
	int		QSRAMRW_Test();
	int		FRAMEBUFRW_Test();
	int		DPRAMRW_Test();

	struct TEST_TOC		memdiag_toc[] =
#ifndef RISC
	{ { bootrom_description, "BOOTROM", "M1", BOOTROM_Test,
	    bootrom_parameter, bootrom_results, MEMRO_RUN_COUNT, 0 },
	  { eeprom_description, "EEPROM", "M2", EEPROM_Test,
	    eeprom_parameter, eeprom_results, EEPROM_RUN_COUNT, 0 },
	  { nmpflash_description, "NMPFLASH", "M3", NMPFLASH_Test,
	    nmpflash_parameter, nmpflash_results, MEMRO_RUN_COUNT, 0 },
	  { txsarram_description, "TXSARRAM", "M4", TXSARRAM_Test,
	    txsarram_parameter, txsarram_results, MEMRW_RUN_COUNT, 0 },
	  { rxsarram_description, "RXSARRAM", "M5", RXSARRAM_Test,
	    rxsarram_parameter, rxsarram_results, MEMRW_RUN_COUNT, 0 },
	  { dramrw_description, "DRAMRW", "M6", DRAMRW_Test,
	    dramrw_parameter, dramrw_results, MEMRW_RUN_COUNT, 0 },
	  { nvram_description, "NVRAM", "M7", NVRAM_Test,
	    nvram_parameter, nvram_results, NVRAM_RUN_COUNT, 0 },
	  { framebufrw_description, "FRAMEBUFRW", "M8", FRAMEBUFRW_Test,
	    framebufrw_parameter, framebufrw_results, MEMRW_RUN_COUNT, 0 },
	  { dpramrw_description, "DPRAMRW", "M9", DPRAMRW_Test,
	    dpramrw_parameter, dpramrw_results, MEMRW_RUN_COUNT, 0 },
#else /* RISC */
	{ { dmpflash_description, "DMPFLASH", "M1", DMPFLASH_Test,
	    dmpflash_parameter, dmpflash_results, MEMRO_RUN_COUNT, 0 },
	  { sramrw_description, "SRAMRW", "M2", SRAMRW_Test,
	    sramrw_parameter, sramrw_results, MEMRW_RUN_COUNT, 0 },
	  { dramrw_description, "DRAMRW", "M3", DRAMRW_Test,
	    dramrw_parameter, dramrw_results, MEMRW_RUN_COUNT, 0 },
	  { framebufrw_description, "FRAMEBUFRW", "M5", FRAMEBUFRW_Test,
	    framebufrw_parameter, framebufrw_results, MEMRW_RUN_COUNT, 0 },
#endif /* RISC */
	  { NULL, NULL, NULL } };

	struct image_desc {
	   long	ilen;
	   unsigned short version;
	   unsigned char  icksum;
	   unsigned char  dcksum;
	   unsigned char  fill[8];
	};

uChar do_dcsum(
	uChar		*bp,
	Int32		len)
{
	uChar		cs = 0;

	while (len--)
	   cs += *bp++;

	return(cs);
}

#ifndef RISC
int BOOTROM_Test(
	int		ex_flag)
{
	register int	rc;
	struct image_desc *id;
	register uChar	checksum = 0;

	if (diag_verbosity >= VERBOSITY_LEVEL3) {
	   strcpy(sys_specific_info, "BOOTROM");
	   sprintf(cli_print_buf, test_in_progress_msg, bootrom_start_address,
	           diag_test_id, diag_current_runcount, sys_specific_info);
	   put_line(cli_print_buf);
	}

	id = (struct image_desc *) (bootrom_start_address + 0x3f0);
	checksum = do_dcsum((uChar *) id, 7);
	if (checksum != id->dcksum) {
	   sys_fail_address = (uInt32) &id->dcksum;
	   sys_data_expected = checksum;
	   sys_data_read = id->dcksum;
	   sys_error_code = MEMDIAG_BOOTROM_DESCRIPTOR_CHECKSUM_FAIL_ERROR;
	   if (check_for_test_stop())
	      return(rc = sys_error_code);
	}

	checksum = do_dcsum((uChar *) bootrom_start_address, id->ilen);
	if (checksum != id->icksum) {
	   sys_fail_address = (uInt32) &id->icksum;
	   sys_data_expected = checksum;
	   sys_data_read = id->icksum;
	   sys_error_code = MEMDIAG_BOOTROM_IMAGE_CHECKSUM_FAIL_ERROR;
	   if (check_for_test_stop())
	      return(rc = sys_error_code);
	}

	bootrom_checksum_expected = checksum;
	bootrom_checksum_read = id->icksum;
	bootrom_results[MEMRO_CHECKSUM_EXPECTED].display_control = ON;
	bootrom_results[MEMRO_CHECKSUM_READ].display_control = ON;

	return(MEMDIAG_OK);
}


/*-----------------------------------------------------------------*
 * EEPROM_Test()
 *-----------------------------------------------------------------*/
int EEPROM_Test(
	int		ex_flag)
{
	register int	rc, i, j;
	register uInt32	addr, eeprom_addr;
	register uInt16	*pw, *pr, cksum;
	uInt32		data_read, data;

	rc = 0;
	if (eeprom_typeofaccess[0] == 'W')
	{
	   pw = (uInt16 *) &eeprom_data_w.data_valid;
	   data = eeprom_data; cksum = 0;
	   if (eeprom_mode[0] == 'R')
	      srand(data);
	   for (i = 0, addr = eeprom_start_address; addr < eeprom_end_address;
	        i++, addr++, pw++) {
	      if (i == 16)
	         *pw = 0xAA55;
	      else
	         *pw = data;
	      if (i >= 16)
	         cksum += *pw;

	      switch (eeprom_mode[0]) {
	         case 'I':
	            data = (uInt16) (data + eeprom_increment);
	            break;
	         case 'C':
	            data = (uInt16) ~data;
	            break;
	         case 'S':
	            data = ((uInt16) (data << 1)) |
	                   ((uInt16) (data >> 15));
	            break;
	         case 'R':
	            data = (uInt16) rand();
	            break;
	         default:
	            break;
	      }
	   }
	   cksum = 0x5555 - cksum;
	   *pw = cksum;

	   /* write enable */
		*(uChar *)CNTL_REG_14 = 0x00;  /* disable write protection */
		*(uChar *)CNTL_REG_13 = 0x01;  /* enable write protection */
	   SELECT_EEPROM;
	   eeprom_addr = E_WEN_CODE;
	   for (i = 8; i >= 0; i--) {
	      if (eeprom_addr & (1 << i)) {
	         SET_DI;
	      }
	      else {
	         RESET_DI;
	      }
	      TOGGLE_CLOCK;
	   }
	   RESET_DI;
	   DESELECT_EEPROM;
	   TOGGLE_CLOCK;

	   pw = (uInt16 *) &eeprom_data_w.data_valid;

	   for (addr = eeprom_start_address; addr <= eeprom_end_address;
	        addr++, pw++) {

	      SELECT_EEPROM;

	      eeprom_addr = addr | E_WR_CODE;

	      /* send address */
	      for (i = 8; i >= 0; i--) {
	         if (eeprom_addr & (1 << i)) {
	            SET_DI;
	         }
	         else {
	            RESET_DI;
	         }
	         TOGGLE_CLOCK;
	      }
	      RESET_DI;
	      TOGGLE_CLOCK;

	      /* write word */
	      for (i = 0; i < 16; i++) {
	         if (*pw & (0x8000 >> i)) {
	            SET_DI;
	         }
	         else {
	            RESET_DI;
	         }
	         TOGGLE_CLOCK;
	      }
	      RESET_DI;
	      DESELECT_EEPROM;
	      TOGGLE_CLOCK;

	      SELECT_EEPROM;
	      for (j = 0; j < 6250; j++) { /* 6250 * 1.6 uS = 10 ms (approx) */
	         TOGGLE_CLOCK;

	         if (*((volatile uInt16 *) STATUS_REG) & SERIAL_EEPROM_DO_MASK)
	            break;
	      }
	      DESELECT_EEPROM;

	      if (j == 6250) {
	         sys_fail_address = STATUS_REG;
	         sys_data_expected = SERIAL_EEPROM_DO_MASK;
	         sys_data_read = 0;
	         sys_error_code = MEMDIAG_SERIAL_EEPROM_WRITE_TIMEOUT_ERROR;
	         if (check_for_test_stop())
	            return(sys_error_code);
	      }
	   }

	   /* write disable */
	   SELECT_EEPROM;
	   eeprom_addr = E_WDS_CODE;
	   for (i = 0; i >= 0; i--) {
	      if (eeprom_addr & (1 << i)) {
	         SET_DI;
	      }
	      else {
	         RESET_DI;
	      }
	      TOGGLE_CLOCK;
	   }
	   RESET_DI;
	   DESELECT_EEPROM;
	   TOGGLE_CLOCK;
	}
	else {
	   eeprom_results[EEPROM_DATA].display_control = OFF;
	   eeprom_results[EEPROM_MODE].display_control = OFF;
	   eeprom_results[EEPROM_INCREMENT].display_control = OFF;
	}

	pw = (uInt16 *) &eeprom_data_w.data_valid;
	pr = (uInt16 *) &eeprom_data_r.data_valid;

	for (addr = eeprom_start_address; addr <= eeprom_end_address;
	     addr++, pw++, pr++) {

	   SELECT_EEPROM;

	   eeprom_addr = addr | E_RD_CODE;

	   /* send address */
	   for (i = 8; i >= 0; i--) {
	      if (eeprom_addr & (1 << i)) {
	         SET_DI;
	      }
	      else {
	         RESET_DI;
	      }
	      TOGGLE_CLOCK;
	   }
	   RESET_DI;
	   TOGGLE_CLOCK;

	   /* read word */
	   for (*pr = i = 0; i < 16; i++) {
	      *pr |= (*((uInt16 *) STATUS_REG) & SERIAL_EEPROM_DO_MASK) ?
 		     (0x8000 >> i) : 0;
	      TOGGLE_CLOCK;
	   }
	   DESELECT_EEPROM;
	   TOGGLE_CLOCK;
	   if (eeprom_typeofaccess[0] == 'W') {
	      if (*pr != *pw) {
	         sys_fail_address = addr;
	         sys_data_expected = *pw;
	         sys_data_read = *pr;
#if 0
				/* RLB DBG */
				printf("data_read=%x, data_expected=%x\n",sys_data_read,sys_data_expected);
#endif /* ZERO */
	         sys_error_code = MEMDIAG_SERIAL_EEPROM_DATA_ERROR;
	         if (check_for_test_stop())
	            return(sys_error_code);
	      }
	   }
	}

	if (eeprom_typeofaccess[0] == 'R') {
	   if ((data_read = eeprom_data_r.data_valid) != 0x55AA &&
	        data_read != 0xAA55) {
	      sys_fail_address = 0x10;
	      sys_data_expected = 0x55AA;
	      sys_data_read = data_read;
			/* RLB DBG */
			printf("DATA_READ=%x, data_expected=%x\n",sys_data_read,sys_data_expected);
	      sys_error_code = MEMDIAG_SERIAL_EEPROM_DATA_ERROR;
	      if (check_for_test_stop())
	         return(sys_error_code);
	   }

	   if (diag_verbosity >= VERBOSITY_LEVEL3) {
	      strcpy(sys_specific_info, "EEPROM");
	   }
	   
	   if (rc = eeprom_checksum())
		{
	      sys_fail_address = 0x3F;
	      sys_data_expected = eeprom_checksum_expected;
	      sys_data_read = eeprom_checksum_read;
	      sys_error_code = MEMDIAG_SERIAL_EEPROM_CHECKSUM_FAIL_ERROR;
	      if (check_for_test_stop())
	         return(sys_error_code);
	   }
	}

	return(rc);
}

int NMPFLASH_Test(
	int		ex_flag)
{
	register int	rc;
	struct image_desc *id;
	register uChar	checksum = 0;
	register uChar	checksum1 = 0;

	if (diag_verbosity >= VERBOSITY_LEVEL3) {
	   strcpy(sys_specific_info, "NMPFLASH");
	   sprintf(cli_print_buf, test_in_progress_msg, nmpflash_start_address,
	           diag_test_id, diag_current_runcount, sys_specific_info);
	   put_line(cli_print_buf);
	}

	id = (struct image_desc *) (nmpflash_start_address + 0x3f0);
	checksum = do_dcsum((uChar *) id, 7);
	if (checksum != id->dcksum) {
	   sys_fail_address = (uInt32) &id->dcksum;
	   sys_data_expected = checksum;
	   sys_data_read = id->dcksum;
	   sys_error_code = MEMDIAG_FLASH_DESCRIPTOR_CHECKSUM_FAIL_ERROR;
	   if (check_for_test_stop())
	      return(rc = sys_error_code);
	}

	if (id->ilen <= FLASH_LENGTH)
	   checksum = do_dcsum((uChar *) nmpflash_start_address, id->ilen);
	else {
	   checksum = do_dcsum((uChar *) nmpflash_start_address, FLASH_LENGTH);
	   checksum1 = do_dcsum((uChar *) FLASH2_START,(id->ilen-FLASH_LENGTH));
	   checksum += checksum1;
	}
	if (checksum != id->icksum) {
	   sys_fail_address = (uInt32) &id->icksum;
	   sys_data_expected = checksum;
	   sys_data_read = id->icksum;
	   sys_error_code = MEMDIAG_FLASH_IMAGE_CHECKSUM_FAIL_ERROR;
	   if (check_for_test_stop())
	      return(rc = sys_error_code);
	}

	nmpflash_checksum_expected = checksum;
	nmpflash_checksum_read = id->icksum;
	nmpflash_results[MEMRO_CHECKSUM_EXPECTED].display_control = ON;
	nmpflash_results[MEMRO_CHECKSUM_READ].display_control = ON;

	return(MEMDIAG_OK);
}
#endif /* RISC */

/*---------------------------------------------------------------------
 * TXSARRAM_Test()
 *---------------------------------------------------------------------*/
#ifdef  ATM 
int TXSARRAM_Test(
    int   ex_flag)
{
   return(MEMORY_Test(txsarram_parameter, (uInt32) 0, TXSARRAM_CALL, ex_flag));
}

/*---------------------------------------------------------------------
 * RXSARRAM_Test()
 *---------------------------------------------------------------------*/
int RXSARRAM_Test(
    int   ex_flag)
{
   return(MEMORY_Test(rxsarram_parameter, (uInt32) 0, RXSARRAM_CALL, ex_flag));
}

/*---------------------------------------------------------------------
 * DPRAMRW_Test()
 *---------------------------------------------------------------------*/
int DPRAMRW_Test(
    int   ex_flag)
{
   return(MEMORY_Test(dpramrw_parameter, (uInt32) 0, DPRAMRW_CALL, ex_flag));
}

#else  /* !ATM */

int DMPFLASH_Test(
	int		ex_flag)
{
	register int	rc;
	register uChar	checksum = 0;
	register uChar	*bp;
	register uInt32	len;

	if (diag_verbosity >= VERBOSITY_LEVEL3) {
	   strcpy(sys_specific_info, "DMPFLASH");
	   sprintf(cli_print_buf, test_in_progress_msg,dmpflash_start_address,
	           diag_test_id, diag_current_runcount, sys_specific_info);
	   put_line(cli_print_buf);
	}

	bp = (uChar *) dmpflash_start_address;
	len = *((uInt32 *) (dmpflash_start_address + 0x24));

	if (len > DMP_FLASH_LENGTH) {
	   sys_fail_address = (uInt32) (dmpflash_start_address + 0x24);
	   sys_data_expected = DMP_FLASH_LENGTH;
	   sys_data_read = len;
	   sys_error_code = MEMDIAG_DMP_FLASH_INVALID_LENGTH_ERROR;
	   if (check_for_test_stop())
	      return(rc = sys_error_code);
	}

	while (len--) {
	   checksum += *bp++;
	}

	if (checksum != 0x00) {
	   sys_fail_address = (uInt32) (dmpflash_start_address + 0x20);
	   sys_data_read = *((uInt32 *) (dmpflash_start_address + 0x20))&0xFF;
	   sys_data_expected = (sys_data_read - checksum) & 0xFF;
	   sys_error_code = MEMDIAG_DMP_FLASH_CHECKSUM_FAIL_ERROR;
	   if (check_for_test_stop())
	      return(rc = sys_error_code);
	}

	dmpflash_checksum_read = *((uInt32 *) 
	                              (dmpflash_start_address + 0x20)) & 0xFF;
	dmpflash_checksum_expected = dmpflash_checksum_read;
	dmpflash_results[MEMRO_CHECKSUM_EXPECTED].display_control = ON;
	dmpflash_results[MEMRO_CHECKSUM_READ].display_control = ON;

	return(MEMDIAG_OK);
}
#endif   /* ATM */


#ifndef RISC
int NVRAM_Test(
	int		ex_flag)
{
	register int	rc, CresMibLength;
	register uInt16	*nvr;
	uInt32		data_read;

	if (nvram_typeofaccess[0] == 'R') {
	   strcpy(result_nvram_access, value_access_byte);

	   nvram_results[EEPROM_DATA].display_control = OFF;
	   nvram_results[EEPROM_MODE].display_control = OFF;
	   nvram_results[EEPROM_INCREMENT].display_control = OFF;

	   if (diag_verbosity >= VERBOSITY_LEVEL3) {
	      strcpy(sys_specific_info, "NVRAM");
	   }
	   
	   if (rc = verify_checksum((uInt16 *) BOOTNVRAM_START, 
	                            (int) BOOTNVRAM_LENGTH,
	                            nvram_results, NVRAMRO_CALL)) {
	      sys_fail_address = (uInt32) BOOTNVRAM_START +
	                         (uInt32) BOOTNVRAM_LENGTH - 2;
	      sys_data_expected = nvram_checksum_expected;
	      sys_data_read = nvram_checksum_read;
	      sys_error_code = MEMDIAG_BOOTNVRAM_CHECKSUM_FAIL_ERROR;
	      if (check_for_test_stop())
	         return(sys_error_code);
	   }

	   CresMibLength = CRESMIB_LENGTH;
	   if (rc = verify_checksum((uInt16 *) CRESMIB_START, 
	                            CresMibLength,
	                            nvram_results, NVRAMRO_CALL)) {
	      CresMibLength = CRESMIB_LENGTH/2;
	      if (rc = verify_checksum((uInt16 *) CRESMIB_START, 
	                            CresMibLength,
	                            nvram_results, NVRAMRO_CALL)) {
	         sys_fail_address = (uInt32) CRESMIB_START +
	                            CresMibLength - 2;
	         sys_data_expected = nvram_checksum_expected;
	         sys_data_read = nvram_checksum_read;
	         sys_error_code = MEMDIAG_NVRAM_CRESMIB_CHECKSUM_FAIL_ERROR;
	         if (check_for_test_stop())
	            return(sys_error_code);
	      }
	   }

	   if (rc = verify_checksum((uInt16 *) SMTMIB_START, 
	                            (int) SMTMIB_LENGTH,
	                            nvram_results, NVRAMRO_CALL)) {
	      sys_fail_address = (uInt32) SMTMIB_START +
	                         (uInt32) SMTMIB_LENGTH - 2;
	      sys_data_expected = nvram_checksum_expected;
	      sys_data_read = nvram_checksum_read;
	      sys_error_code = MEMDIAG_NVRAM_SMTMIB_CHECKSUM_FAIL_ERROR;
	      if (check_for_test_stop())
	         return(sys_error_code);
	   }

	   return(rc);
	}
	else {
	   return(MEMORY_Test(nvram_parameter, (uInt32) 0,
	                      NVRAMRW_CALL, ex_flag));
	}
}
#endif /* RISC */

#ifdef RISC
int SRAMRW_Test(
	int		ex_flag)
{
	return(MEMORY_Test(sramrw_parameter, (uInt32) 0, SRAMRW_CALL, ex_flag));
}
#endif /* RISC */

int DRAMRW_Test(
	int		ex_flag)
{
	return(MEMORY_Test(dramrw_parameter, (uInt32) 0, DRAMRW_CALL, ex_flag));
}


#if  0  /* ATM */
int QSRAMRW_Test(
	int		ex_flag)
{
	return(MEMORY_Test(qsramrw_parameter, (uInt32) 0, 
	                   QSRAMRW_CALL, ex_flag));
}
#endif  /* ATM */

int FRAMEBUFRW_Test(
	int		ex_flag)
{
	return(MEMORY_Test(framebufrw_parameter, (uInt32) 0, 
	                   FRAMEBUFRW_CALL, ex_flag));
}

/*------------------------------------------------------------
 * MEMORY_Test()
 *------------------------------------------------------------*/
int MEMORY_Test(
	struct PARAMETER_TABLE	*parm_table,
	uInt32			sys_info,
	enum CALL_TYPE		call_type,
	int			ex_flag)
{
	register uInt32	addr, r_addr;
	register uInt32	start_address, end_address, data, increment;
	register uInt32	c_start_address, c_end_address, c_data, c_increment;
	register char	*access, *mode;
	register char	*c_access, *c_mode;
	register int	rc, a, m, i, j;
	register int	access_all, mode_all;
	register uInt32	sys_info_c;
	register uInt32	data_expected, data_read, c_addr;

#if  1  /* ATM */
	if (call_type == BIGABUFRW_CALL)
	   sys_info_c = (uInt32) 0xAA800000;
	else
	   sys_info_c = sys_info;

	start_address = *((uInt32 *) (parm_table+0)->parameter);
	end_address   = *((uInt32 *) (parm_table+1)->parameter);
	data          = *((uInt32 *) (parm_table+2)->parameter);
	access        =  (char *)    (parm_table+3)->parameter;
	mode          =  (char *)    (parm_table+4)->parameter;
	increment     = *((uInt32 *) (parm_table+5)->parameter);

	if (start_address > end_address) {
	   addr = start_address;
	   start_address = end_address;
	   end_address = addr;
	}

	if (diag_datapatternvariant[0] == 'T') {
	   data *= diag_current_runcount;
	   switch (call_type) {
/* !ATM RLB DBG 
	      case MACBUFRW_CALL:   result_macbufrw_data = data;   break;
 */

#ifndef RISC
	      case NVRAMRW_CALL:    result_nvram_data = data;    break;
#else /* RISC */
	      case BIGABUFRW_CALL: result_bigabufrw_data = data; break;
	      case SRAMRW_CALL:     result_sramrw_data = data;    break;
#endif /* RISC */
	      case DRAMRW_CALL:     result_dramrw_data = data;     break;
	      case DPRAMRW_CALL:    result_dpramrw_data = data;    break;
#if  0  /* ATM */
	      case QSRAMRW_CALL:    result_qsramrw_data = data;    break;
#endif  /* ATM */
	      case FRAMEBUFRW_CALL: result_framebufrw_data = data; break;
	   }
	}

	if (ex_flag || *access == ACCESS_TYPE_ALL)
	   access_all = TRUE;
	else
	   access_all = FALSE;

	if (ex_flag || *mode == MODE_TYPE_ALL)
	   mode_all = TRUE;
	else
	   mode_all = FALSE;

	if (call_type == DRAMRW_CALL || 
#ifdef RISC
	    call_type == SRAMRW_CALL ||
#endif /* RISC */
	    call_type == DPRAMRW_CALL ||
#if  0  /* ATM */
	    call_type == QSRAMRW_CALL ||
#endif  /* ATM */
	    call_type == FRAMEBUFRW_CALL) {


	   switch (call_type) {
#ifdef RISC
	      case SRAMRW_CALL: strcpy(sys_specific_info, "SRAM"); break;
#endif /* RISC */
	      case DRAMRW_CALL: strcpy(sys_specific_info, "DRAM"); break;
#if  0 /* ATM */
	      case DPRAMRW_CALL: strcpy(sys_specific_info, "DPRAM"); break;
	      case QSRAMRW_CALL: strcpy(sys_specific_info, "QSRAM"); break;
#endif /* ATM */
	      default: strcpy(sys_specific_info, "FRAMEBUF"); break;
	   }

	   c_start_address = start_address & ~0x3;
      	   c_end_address   = end_address & ~0x3;

	   if (memdiag_databus_test[0] == 'T') {

	      if (diag_verbosity >= VERBOSITY_LEVEL3) {
	         sprintf(cli_print_buf, databus_test_progress, 
	                 c_start_address, value_access_long, diag_test_id, 
	                 diag_current_runcount,
	                 sys_specific_info);
	         put_line(cli_print_buf);
	      }


	      *((uInt32 *) c_start_address) = 0;
	      if ((data_read = *((uInt32 *) c_start_address)) != 0) {
	         sys_fail_address = c_start_address;
	         sys_data_expected = 0;
	         sys_data_read = data_read;
	         sys_error_code = MEMDIAG_DATA_BUS_STUCK_1_ERROR;
	         if (check_for_test_stop())
	            return(rc = sys_error_code);
	      }

	      *((uInt32 *) c_start_address) = 0xFFFFFFFF;
	      if ((data_read = *((uInt32 *) c_start_address)) != 0xFFFFFFFF) {
	         sys_fail_address = c_start_address;
	         sys_data_expected = 0xFFFFFFFF;
	         sys_data_read = data_read;
	         sys_error_code = MEMDIAG_DATA_BUS_STUCK_0_ERROR;
	         if (check_for_test_stop())
	            return(rc = sys_error_code);
	      }

	      for (i = 0; i < 16; i++) {
	         *((uInt32 *) c_start_address) = (1 << i);
	         if ((data_read = *((uInt32 *) c_start_address)) != (1 << i)) {
	            sys_fail_address = c_start_address;
	            sys_data_expected = (1 << i);
	            sys_data_read = data_read;
	            sys_error_code = MEMDIAG_DATA_BUS_SHORT_ERROR;
	            if (check_for_test_stop())
	               return(rc = sys_error_code);
	         }
	      }
	   }

	   if (memdiag_addrbus_test[0] == 'T') {

	      if (diag_verbosity >= VERBOSITY_LEVEL3) {
	         sprintf(cli_print_buf, addrbus_test_progress, 
	                 c_start_address, value_access_long, diag_test_id, 
	                 diag_current_runcount,
	                 sys_specific_info);
	         put_line(cli_print_buf);
	      }

	      for (c_addr = c_start_address, j = 2; c_addr <= c_end_address; 
	           j++) {
	         *((uInt32 *) c_addr) = 0x55555555;
	         for (addr = c_start_address, i = 2; addr <= c_end_address; 
	              i++) {
	            if (addr != c_addr)
	               *((uInt32 *) addr) = 0xAAAAAAAA;
	            if ((data_read = *((uInt32 *) c_addr)) != 0x55555555) {
	               sys_fail_address = addr;
	               sys_data_expected = 0x55555555;
	               sys_data_read = data_read;
	               sys_error_code = MEMDIAG_ADDRESS_BUS_STUCK_0_ERROR;
	               if (check_for_test_stop())
	                  return(rc = sys_error_code);
	            }
	            addr = c_start_address | (1 << i);
	         }
	         c_addr = c_start_address | (1 << j);
	      }

	      for (c_addr = c_end_address, j = 2; c_addr >= c_start_address; 
	           j++) {
	         *((uInt32 *) c_addr) = 0xAAAAAAAA;
	         for (addr = c_end_address, i = 2; addr >= c_start_address; 
	              i++) {
	            if (addr != c_addr)
	               *((uInt32 *) addr) = 0x55555555;
	            if ((data_read = *((uInt32 *) c_addr)) != 0xAAAAAAAA) {
	               sys_fail_address = addr;
	               sys_data_expected = 0xAAAAAAAA;
	               sys_data_read = data_read;
	               sys_error_code = MEMDIAG_ADDRESS_BUS_STUCK_1_ERROR;
	               if (check_for_test_stop())
	                  return(rc = sys_error_code);
	            }
	            addr = c_end_address & ~(1 << i);
	         }
	         c_addr = c_end_address & ~(1 << j);
	      }
	      if (rc = check_for_break())
	         return(rc);
	   }

	   if (memdiag_faccess_test[0] == 'T') {

	      if (diag_verbosity >= VERBOSITY_LEVEL3) {
	         sprintf(cli_print_buf, faccess_test_progress, 
	                 c_start_address, value_access_long, diag_test_id, 
	                 diag_current_runcount,
	                 sys_specific_info);
	         put_line(cli_print_buf);
	      }

	      for (i = 0; i < 1; i++) {
	         switch (i) {
	            case 1: c_data = 0x00000000; break;
	            case 2: c_data = 0xFFFFFFFF; break;
	            case 3: c_data = 0x55555555; break;
	            case 4: c_data = 0xAAAAAAAA; break;
	            default: c_data = 0xfeedbac; break;
	         }
	         if (rc = faccess_test(c_start_address, c_end_address,c_data)) {
	            switch (call_type) {
#ifdef RISC
	               case SRAMRW_CALL:
	                  sys_error_code = 
	                               MEMDIAG_SRAM_FAST_ACCESS_TEST_FAIL_ERROR;
	                  break;
#endif /* RISC */
	               case DRAMRW_CALL:
	                  sys_error_code = 
	                               MEMDIAG_DRAM_FAST_ACCESS_TEST_FAIL_ERROR;
	                  break;
	               case DPRAMRW_CALL:
	                  sys_error_code = 
	                              MEMDIAG_QSRAM_FAST_ACCESS_TEST_FAIL_ERROR;
	                  break;
#if  0 /* ATM */
	               case QSRAMRW_CALL:
	                  sys_error_code = 
	                              MEMDIAG_QSRAM_FAST_ACCESS_TEST_FAIL_ERROR;
	                  break;
#endif  /* ATM */
	               default:
	                  sys_error_code = 
	                           MEMDIAG_FRAMEBUF_FAST_ACCESS_TEST_FAIL_ERROR;
	                  break;
	            }
	            if (check_for_test_stop())
	               return(rc = sys_error_code);
	         }
	         if (rc = check_for_break())
	            return(rc);

#if  0  /* RLB DBG */
	         if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_POWER_UP_MODE)
	            break;
#endif  /* RLB DBG */

	      }
	   }
	}

	if (call_type != MACBUFRW_CALL && call_type != BIGABUFRW_CALL && 
	    memdiag_cell_test[0] == 'F')
	   return(rc = 0);

	for (a = 0; a < 3; a++) {
	   c_access = ((access_all) ? (char *) get_access(a) : access);
	   if (access_all) {
	      switch (call_type) {
#if  0  /* !ATM */
	         case MACBUFRW_CALL:
	            strcpy(result_macbufrw_access, c_access); break;
#endif  /* ATM */

#ifndef RISC
	         case NVRAMRW_CALL:    
	            strcpy(result_nvram_access, c_access); break;
#else /* RISC */
	         case BIGABUFRW_CALL:
	            strcpy(result_bigabufrw_access, c_access); break;
	         case SRAMRW_CALL:    
	            strcpy(result_sramrw_access, c_access); break;
#endif /* RISC */
	         case DRAMRW_CALL:
	            strcpy(result_dramrw_access, c_access); break;
	         case DPRAMRW_CALL:
	            strcpy(result_dpramrw_access, c_access); break;
#if  0  /* ATM */
	         case QSRAMRW_CALL:
	            strcpy(result_qsramrw_access, c_access); break;
#endif  /* ATM */
	         case FRAMEBUFRW_CALL:
	            strcpy(result_framebufrw_access, c_access); break;
	      }
	   }

	   for (m = 0; m < 5; m++) {
	      c_mode = ((mode_all) ? (char *) get_mode(m) : mode);
	      if (mode_all) {
	         switch (call_type) {
#if  0  /* !ATM */
	            case MACBUFRW_CALL:
	               strcpy(result_macbufrw_mode, c_mode); break;
#endif  /* ATM */

#ifndef RISC
	            case NVRAMRW_CALL:    
	               strcpy(result_nvram_mode, c_mode); break;
#else /* RISC */
	            case BIGABUFRW_CALL:
	               strcpy(result_bigabufrw_mode, c_mode); break;
	            case SRAMRW_CALL:    
	               strcpy(result_sramrw_mode, c_mode); break;
#endif /* RISC */
	            case DRAMRW_CALL:
	               strcpy(result_dramrw_mode, c_mode); break;
	            case DPRAMRW_CALL:
	               strcpy(result_dpramrw_mode, c_mode); break;
#if   0 /* ATM */
	            case QSRAMRW_CALL:
	               strcpy(result_qsramrw_mode, c_mode); break;
#endif  /* ATM */
	            case FRAMEBUFRW_CALL:
	               strcpy(result_framebufrw_mode, c_mode); break;
	         }
	      }

	      c_start_address = start_address;
      	      c_end_address   = end_address;
	      c_data          = data;
	      c_increment     = increment;

	      switch(*c_access) {
	         case ACCESS_TYPE_BYTE:
	            c_data &= 0xFF; c_increment &= 0xFF;
	            break;
	         case ACCESS_TYPE_WORD:
	            c_data &= 0xFFFF; c_increment &= 0xFFFF;
	            c_start_address &= ~0x1; c_end_address &= ~0x1; break;
	         default:
	            c_start_address &= ~0x3; c_end_address &= ~0x3; break;
	      }

#ifdef RISC
	      if (call_type == BIGABUFRW_CALL) {
	         reset_mac();
	         set_mac_buf_ptrs(c_start_address, c_end_address);
	      }
#endif /* RISC */

	      if (rc = perform_write(c_start_address, c_end_address, c_data,
	                             c_access, c_mode, c_increment, 
	                             (uInt32) 1, (uInt32) 0, (uInt32) 0,
	                             sys_info, call_type))
	         break;

	      if (rc = perform_compare(c_start_address, c_end_address, c_data,
	                               c_access, c_mode, c_increment, 
	                               (uInt32) 1, (uInt32) 0, (uInt32) 0,
	                               sys_info_c, call_type))
	         break;

	      if (!mode_all)
	         break;
	   }
	   if (!access_all || rc)
	      break;
	}

	if (rc == 0) {
	   if (access_all) {
	      switch (call_type) {
#if  0  /* !ATM */
	         case MACBUFRW_CALL:
	            strcpy(result_macbufrw_access, value_access_all); break;
#endif  /* ATM */

#ifndef RISC
	         case NVRAMRW_CALL:    
	            strcpy(result_nvram_access, value_access_all); break;
#else /* RISC */
	         case BIGABUFRW_CALL:
	            strcpy(result_bigabufrw_access, value_access_all); break;
	         case SRAMRW_CALL:    
	            strcpy(result_sramrw_access, value_access_all); break;
#endif /* RISC */
	         case DRAMRW_CALL:
	            strcpy(result_dramrw_access, value_access_all); break;
	         case DPRAMRW_CALL:
	            strcpy(result_dpramrw_access, value_access_all); break;
#if  0 /* ATM */
	         case QSRAMRW_CALL:
	            strcpy(result_qsramrw_access, value_access_all); break;
#endif  /* ATM */
	         case FRAMEBUFRW_CALL:
	            strcpy(result_framebufrw_access, value_access_all); break;
	      }
	   }
	   if (mode_all) {
	      switch (call_type) {
#if   0  /* !ATM */
	         case MACBUFRW_CALL:
	            strcpy(result_macbufrw_mode, value_mode_all); break;
#endif   /* ATM */

#ifndef RISC
	         case NVRAMRW_CALL:    
	            strcpy(result_nvram_mode, value_mode_all); break;
#else /* RISC */
	         case BIGABUFRW_CALL:
	            strcpy(result_bigabufrw_mode, value_mode_all); break;
	         case SRAMRW_CALL:    
	            strcpy(result_sramrw_mode, value_mode_all); break;
#endif /* RISC */
	         case DRAMRW_CALL:
	            strcpy(result_dramrw_mode, value_mode_all); break;
	         case DPRAMRW_CALL:
	            strcpy(result_dpramrw_mode, value_mode_all); break;
#if  0  /* ATM */
	         case QSRAMRW_CALL:
	            strcpy(result_qsramrw_mode, value_mode_all); break;
#endif  /* ATM */
	         case FRAMEBUFRW_CALL:
	            strcpy(result_framebufrw_mode, value_mode_all); break;
	      }
	   }
	}

	if (rc == DIAG_READ_WRITE_FAIL_ERROR) {
	   switch (call_type) {
	      case MACBUFRW_CALL:
	         rc = MACDIAG_BUFMEM_READ_WRITE_TEST_FAIL_ERROR; 
	         break;
	      case BIGABUFRW_CALL:
	         rc = BIGADIAG_BUFMEM_READ_WRITE_TEST_FAIL_ERROR; 
	         break;
#ifndef RISC
	      case NVRAMRW_CALL:
	         rc = MEMDIAG_NVRAM_READ_WRITE_TEST_FAIL_ERROR;
	         break;
#else /* RISC */
	      case SRAMRW_CALL:
	         rc = MEMDIAG_SRAM_READ_WRITE_TEST_FAIL_ERROR;
	         break;
#endif /* RISC */
	      case DRAMRW_CALL:
	         rc = MEMDIAG_DRAM_READ_WRITE_TEST_FAIL_ERROR;
	         break;
	      case DPRAMRW_CALL:
	         rc = MEMDIAG_QSRAM_READ_WRITE_TEST_FAIL_ERROR;
	         break;
#if  0  /* ATM */
	      case QSRAMRW_CALL:
	         rc = MEMDIAG_QSRAM_READ_WRITE_TEST_FAIL_ERROR;
	         break;
#endif  /* ATM */
	      case FRAMEBUFRW_CALL:
	         rc = MEMDIAG_FRAMEBUF_READ_WRITE_TEST_FAIL_ERROR;
	         break;
	   }
	}

#endif  /* ATM */

	return(rc);
}

#ifndef RISC
int verify_checksum(
	uInt16			*pw,
	int			len,
	struct RESULTS_TABLE	*r_table,
	enum CALL_TYPE		call_type)
{
	register int	i;
	register uInt16	checksum;
extern	int	diagExecution;

	if (diag_verbosity >= VERBOSITY_LEVEL3) {
	   sprintf(cli_print_buf, test_in_progress_msg, (uInt32) pw,
	           diag_test_id, diag_current_runcount, sys_specific_info);
	   put_line(cli_print_buf);
	}
	   
	for (checksum = 0, i = 0; i < (len - 2); i += 2, pw++) {
	   if (call_type == EEPROMRO_CALL)
	      checksum += *pw;
	   else {
	      checksum += *((uChar *) pw);
	      checksum += *((uChar *) pw+1);
	   }
	}
	if (call_type == EEPROMRO_CALL)
	   checksum = 0x5555 - checksum;
	else {
	   checksum = ((((0x55 - (checksum >> 8))) & 0xFF) << 8) |
	                ((0x55 - (checksum & 0xFF)) & 0xFF);
	}

	if (call_type == EEPROMRO_CALL) {
	   *((uInt32 *) (r_table+EEPROM_CHECKSUM_READ)->result) = *pw;
	   *((uInt32 *) (r_table+EEPROM_CHECKSUM_EXPECTED)->result) = checksum;
	   if (*pw == checksum && diagExecution == TRUE) {
	      (r_table+EEPROM_CHECKSUM_EXPECTED)->display_control = ON;
	      (r_table+EEPROM_CHECKSUM_READ)->display_control = ON;
	   }
	}
	else {
	   *((uInt32 *) (r_table+NVRAM_CHECKSUM_READ)->result) = *pw;
	   *((uInt32 *) (r_table+NVRAM_CHECKSUM_EXPECTED)->result) = checksum;
	   if (*pw == checksum) {
	      (r_table+NVRAM_CHECKSUM_EXPECTED)->display_control = ON;
	      (r_table+NVRAM_CHECKSUM_READ)->display_control = ON;
	   }
	}

	return(*pw != checksum);
}
#endif /* RISC */

