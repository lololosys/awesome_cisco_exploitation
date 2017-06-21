/* $Id: constant.h,v 1.1.4.1 1996/05/09 15:00:52 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/constant.h,v $
 *-----------------------------------------------------------------
 * constant.h -- Constants & Enum Definitions for Diagnostics Driver
 *
 * ?
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: constant.h,v $
 * Revision 1.1.4.1  1996/05/09  15:00:52  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:49  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:11  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:01  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:44  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:20  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************/
/*                                                                            */
/* DIAGNOSTICS DRIVER                                                         */
/*                                                                            */
/* This file contains "Constants & Enum Definitions" that are included as     */
/* part of Diagnostics Driver.                                                */
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

#define	MAX_COMMAND_ARGUMENTS		20

#define	HISTORY_BUF_SIZE		32
#define	HISTORY_LINE_SIZE		144

#define	DISPLAY_BUF_SIZE		128
#define	DISPLAY_LINE_SIZE		144

#define	TEST_BUF_SIZE			32
#define	TEST_LINE_SIZE			80

#define	PRINT_LINE_SIZE			144

#define	COMMAND_LINE_SIZE  		128
#define	PROMPT_LINE_SIZE  		11

#define	CTL_A				1
#define	CTL_B				2
#define	CTL_C				3
#define	CTL_D				4
#define	CTL_H				8
#define	CTL_N				14
#define	CTL_P				16
#define	CTL_R				18
#define	CTL_T				20
#define	CTL_U				21
#define	CTL_V				22
#define	ESC				27
#define	DEL				127

	enum	TEST_STATUS {
	TEST_FAIL = -1,
	TEST_SUCCESSFUL = 0,
	TEST_WARNING = 1
	};

	enum	DISPLAY_CONTROL {
	OFF = 0,
	ON = 1
	};

	enum	FLAG_TYPE {
	FALSE = 0,
	TRUE = 1
	};

	enum    DIAG_ENVIRONMENT {
	POWER_UP = 0,
	OFF_LINE = 1,
	ON_LINE = 2
	};

	enum	USER_LEVEL {
	USER_LOGOUT = -1,
	USER_GENERAL = 0,
	USER_CRESCENDO = 1,
	USER_ROOT = 2
	};

	enum		DEBUG_BREAK_TYPE {
	RESUME_DEBUG = 0,
	BREAK_DEBUG = 1
	};

	enum		TEST_BREAK_TYPE {
	BREAK_ALL_TESTS = -99,
	BREAK_CURRENT_TEST = -9,
	RESUME_TEST = 0
	};

        enum            CLI_RETURN_CODE {
	CLI_INVALID_COMMAND = -2,
	CLI_COMMAND_NOT_IN_HISTORY = -1,
        CLI_OK = 0,
	SYNTAX_ERROR_START,
        SYNTAX_UNEXPECTED_ARGUMENT_ERROR,
        SYNTAX_MISSING_ARGUMENTS_ERROR,
        SYNTAX_TOO_FEW_ARGUMENTS_ERROR,
        SYNTAX_TOO_MANY_ARGUMENTS_ERROR,
        SYNTAX_INVALID_ARGUMENT_DELIMITER_ERROR,
        SYNTAX_INVALID_ARGUMENT_OPTION_ERROR,
        SYNTAX_START_ADDRESS_NOT_SPECIFIED_ERROR,
        SYNTAX_INVALID_START_ADDRESS_ERROR,
	SYNTAX_MORE_THAN_ONE_START_ADDRESS_SPECIFIED_ERROR,
        SYNTAX_END_ADDRESS_NOT_SPECIFIED_ERROR,
        SYNTAX_INVALID_END_ADDRESS_ERROR,
	SYNTAX_MORE_THAN_ONE_END_ADDRESS_SPECIFIED_ERROR,
        SYNTAX_LENGTH_NOT_SPECIFIED_ERROR,
        SYNTAX_INVALID_LENGTH_ERROR,
	SYNTAX_MORE_THAN_ONE_LENGTH_SPECIFIED_ERROR,
        SYNTAX_BOTH_END_ADDRESS_AND_LENGTH_SPECIFIED_ERROR,
        SYNTAX_DATA_NOT_SPECIFIED_ERROR,
        SYNTAX_INVALID_DATA_ERROR,
	SYNTAX_MORE_THAN_ONE_DATA_SPECIFIED_ERROR,
        SYNTAX_ACCESS_NOT_SPECIFIED_ERROR,
        SYNTAX_INVALID_ACCESS_ERROR,
	SYNTAX_MORE_THAN_ONE_ACCESS_SPECIFIED_ERROR,
        SYNTAX_MODE_NOT_SPECIFIED_ERROR,
        SYNTAX_INVALID_MODE_ERROR,
	SYNTAX_MORE_THAN_ONE_MODE_SPECIFIED_ERROR,
        SYNTAX_INCREMENT_NOT_SPECIFIED_ERROR,
        SYNTAX_INVALID_INCREMENT_ERROR,
	SYNTAX_MORE_THAN_ONE_INCREMENT_SPECIFIED_ERROR,
        SYNTAX_INSTRUCTION_NOT_SPECIFIED_ERROR,
        SYNTAX_UNEXPECTED_OPERAND_ERROR,
        SYNTAX_MISSING_OPERAND_ERROR,
        SYNTAX_INVALID_OPERAND_ERROR,
        SYNTAX_OPERAND_OUT_OF_RANGE_ERROR,
        SYNTAX_INVALID_INSTRUCTION_ERROR,
        SYNTAX_INVALID_INSTRUCTION_FORMAT_ERROR,
        SYNTAX_INSTRUCTION_NOT_IMPLEMENTED_ERROR,
	SYNTAX_MORE_THAN_ONE_INSTRUCTION_SPECIFIED_ERROR,
        SYNTAX_NUMBER_OF_TIMES_NOT_SPECIFIED_ERROR,
        SYNTAX_INVALID_NUMBER_OF_TIMES_ERROR,
	SYNTAX_MORE_THAN_ONE_NUMBER_OF_TIMES_SPECIFIED_ERROR,
        SYNTAX_TIME_DELAY_NOT_SPECIFIED_ERROR,
        SYNTAX_INVALID_TIME_DELAY_ERROR,
	SYNTAX_MORE_THAN_ONE_TIME_DELAY_SPECIFIED_ERROR,
        SYNTAX_VERBOSITY_NOT_SPECIFIED_ERROR,
        SYNTAX_INVALID_VERBOSITY_ERROR,
	SYNTAX_MORE_THAN_ONE_VERBOSITY_SPECIFIED_ERROR,
	SYNTAX_VERBOSITY_OUT_OF_RANGE_ERROR,
        SYNTAX_WRITE_ADDRESS_NOT_SPECIFIED_ERROR,
        SYNTAX_INVALID_WRITE_ADDRESS_ERROR,
	SYNTAX_MORE_THAN_ONE_WRITE_ADDRESS_SPECIFIED_ERROR,
        SYNTAX_READ_ADDRESS_NOT_SPECIFIED_ERROR,
        SYNTAX_INVALID_READ_ADDRESS_ERROR,
	SYNTAX_MORE_THAN_ONE_READ_ADDRESS_SPECIFIED_ERROR,
        SYNTAX_COMPARE_DATA_NOT_SPECIFIED_ERROR,
        SYNTAX_INVALID_COMPARE_DATA_ERROR,
	SYNTAX_MORE_THAN_ONE_COMPARE_DATA_SPECIFIED_ERROR,
	SYNTAX_COMMAND_NOT_SPECIFIED_ERROR,
	SYNTAX_COMMAND_NOT_FOUND_ERROR,
	SYNTAX_NO_OF_COMMANDS_NOT_SPECIFIED_ERROR,
	SYNTAX_INVALID_NUMBER_OF_COMMANDS_ERROR,
	SYNTAX_NUMBER_OF_COMMANDS_OUT_OF_RANGE_ERROR,
	SYNTAX_SECTION_ID_NOT_SPECIFIED_ERROR,
	SYNTAX_SECTION_ID_NOT_FOUND_ERROR,
	SYNTAX_MORE_THAN_ONE_SECTION_ID_SPECIFIED_ERROR,
	SYNTAX_TEST_ID_NOT_SPECIFIED_ERROR,
	SYNTAX_TEST_ID_NOT_FOUND_ERROR,
	SYNTAX_MORE_THAN_ONE_TEST_ID_SPECIFIED_ERROR,
	SYNTAX_BOTH_SECTION_ID_AND_TEST_ID_SPECIFIED_ERROR,
	SYNTAX_PARAMETER_NOT_SPECIFIED_ERROR,
	SYNTAX_PARAMETER_NOT_FOUND_ERROR,
	SYNTAX_PARAMETER_VALUE_NOT_SPECIFIED_ERROR,
	SYNTAX_INVALID_PARAMETER_VALUE_ERROR,
	SYNTAX_INVALID_PARAMETER_VALUE_DELIMITER_ERROR,
	SYNTAX_PARAMETER_VALUE_OUT_OF_RANGE_ERROR,
	SYNTAX_PARAMETER_LENGTH_OUT_OF_RANGE_ERROR,
	SYNTAX_INVALID_DATE_SPECIFIED_ERROR,
	SYNTAX_INVALID_TIME_SPECIFIED_ERROR,
	SYNTAX_PROMPT_STRING_TOO_LARGE_ERROR,
	SYNTAX_ERROR_CODE_NOT_SPECIFIED_ERROR,
	SYNTAX_INVALID_ERROR_CODE_ERROR,
	SYNTAX_ERROR_CODE_OUT_OF_RANGE_ERROR,
	SYNTAX_INVALID_SOURCE_MAC_SPECIFIED_ERROR,
	SYNTAX_INVALID_DESTINATION_MAC_SPECIFIED_ERROR,
	SYNTAX_ERROR_END,
	CLI_EXIT = 9999
        };

	enum	CALL_TYPE {
	DEBUG_CALL,
	BOOTROM_CALL,
	EEPROMRO_CALL,
	FLASHRO_CALL,
	NVRAMRO_CALL,
	NVRAMRW_CALL,
	SRAMRW_CALL,
	DRAMRW_CALL,
	QSRAMRW_CALL,
	FRAMEBUFRW_CALL,
	PHYCCE_CALL,
	MACACC_CALL,
	MACCCE_CALL,
	MACBUFRW_CALL,
	MACTOKEN_CALL,
	MACINTLB_CALL,
	MACEXTLB_CALL,
	PRING_CALL,
	SRING_CALL,
	LRING_CALL,
	RINGLB_CALL,
	BIGABUFRW_CALL,
	BIGAINTLB_CALL,
	BIGAEXTLB_CALL,
	ETIINTLB_CALL,
	ETIEXTLB_CALL,
	ETILOADGEN_CALL,
	ETILOADGEN_TX_CALL,
	ETILOADGEN_RX_CALL,
	MACLOADGEN_CALL,
	MACLOADGEN_TX_CALL,
	MACLOADGEN_RX_CALL,
	BIGATXDMA_CALL,
	BIGARXDMA_CALL,
	BIGALOADGEN_CALL,
	BIGALOADGEN_TX_CALL,
	BIGALOADGEN_RX_CALL,
	TXSARRAM_CALL,
	RXSARRAM_CALL,
	DPRAMRW_CALL
	};

	enum	VERBOSITY_LEVEL {
	VERBOSITY_LEVEL0,
	VERBOSITY_LEVEL1,
	VERBOSITY_LEVEL2,
	VERBOSITY_LEVEL3
	};

        enum            ACCESS_TYPE {
        ACCESS_TYPE_BYTE   = 'B',
        ACCESS_TYPE_WORD   = 'W',
        ACCESS_TYPE_LONG   = 'L',
	ACCESS_TYPE_ALL    = 'A'
        };

        enum            MODE_TYPE {
        MODE_TYPE_NORMAL     = 'N',
        MODE_TYPE_INCREMENT  = 'I',
        MODE_TYPE_COMPLEMENT = 'C',
        MODE_TYPE_SHIFT      = 'S',
        MODE_TYPE_RANDOM     = 'R',
	MODE_TYPE_ALL        = 'A'
        };

