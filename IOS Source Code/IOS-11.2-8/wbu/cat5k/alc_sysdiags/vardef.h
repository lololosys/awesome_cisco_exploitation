/* $Id: vardef.h,v 1.1.4.1 1996/05/09 15:01:20 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/vardef.h,v $
 *------------------------------------------------------------------
 * vardef.h -- "Variable Definitions" included as part of Diagnostics Driver.
 *
 * 1995, Randy Bau
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vardef.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:20  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:19  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:44  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:29  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:54:15  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:48  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.2.6.1  1996/01/18  05:02:17  rlowe
 * Manufacturability changes to bootrom and flash boot code.
 *
 * Branch: ATM21_branch
 *
 * Revision 1.2  1995/10/27  02:51:56  rbau
 * removed the EXIT command. completed the header with cvs keywords.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

	char		cli_invalid_command[] = 
			"INVALID COMMAND, USE HELP FACILITY";
	char		cli_command_not_in_history[] = 
			"COMMAND NOT IN HISTORY, USE HISTORY FACILITY";
        char            Syntax_unexpected_argument_error[] = 
                        "UNEXPECTED ARGUMENT ERROR";
        char            Syntax_missing_arguments_error[] = 
                        "MISSING ARGUMENTS";
        char            Syntax_too_few_arguments_error[] = 
                        "TOO FEW ARGUMENTS";
        char            Syntax_too_many_arguments_error[] = 
                        "TOO MANY ARGUMENTS";
        char            Syntax_invalid_argument_delimiter_error[] = 
                        "INVALID ARGUMENT DELIMITER";
        char            Syntax_invalid_argument_option_error[] = 
                        "INVALID ARGUMENT OPTION";
#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
        char            Syntax_start_address_not_specified_error[] = 
                        "START ADDRESS NOT SPECIFIED";
        char            Syntax_invalid_start_address_error[] = 
                        "INVALID START ADDRESS VALUE";
        char            Syntax_more_than_one_start_address_specified_error[] = 
                        "MORE THAN ONE START ADDRESS OPTION SPECIFIED";
        char            Syntax_end_address_not_specified_error[] = 
                        "END ADDRESS OPTION USED, BUT NOT SPECIFIED";
        char            Syntax_invalid_end_address_error[] = 
                        "INVALID END ADDRESS VALUE";
        char            Syntax_more_than_one_end_address_specified_error[] = 
                        "MORE THAN ONE END ADDRESS OPTION SPECIFIED";
        char            Syntax_length_not_specified_error[] = 
                        "LENGTH OPTION USED, BUT NOT SPECIFIED";
        char            Syntax_invalid_length_error[] = 
                        "INVALID LENGTH VALUE";
        char            Syntax_more_than_one_length_specified_error[] = 
                        "MORE THAN ONE LENGTH OPTION SPECIFIED";
        char            Syntax_both_end_address_and_length_specified_error[] = 
                        "BOTH END ADDRESS AND LENGTH SPECIFIED";
        char            Syntax_data_not_specified_error[] = 
                        "DATA NOT SPECIFIED";
        char            Syntax_invalid_data_error[] = 
                        "INVALID DATA VALUE";
        char            Syntax_more_than_one_data_specified_error[] = 
                        "MORE THAN ONE DATA OPTION SPECIFIED";
        char            Syntax_access_not_specified_error[] = 
                        "ACCESS OPTION USED, BUT NOT SPECIFIED";
        char            Syntax_invalid_access_error[] = 
                        "INVALID ACCESS VALUE";
        char            Syntax_more_than_one_access_specified_error[] = 
                        "MORE THAN ONE ACCESS OPTION SPECIFIED";
        char            Syntax_mode_not_specified_error[] = 
                        "MODE OPTION USED, BUT NOT SPECIFIED";
        char            Syntax_invalid_mode_error[] = 
                        "INVALID MODE VALUE";
        char            Syntax_more_than_one_mode_specified_error[] = 
                        "MORE THAN ONE MODE OPTION SPECIFIED";
        char            Syntax_increment_not_specified_error[] = 
                        "INCREMENT OPTION USED, BUT NOT SPECIFIED";
        char            Syntax_invalid_increment_error[] = 
                        "INVALID INCREMENT VALUE";
        char            Syntax_more_than_one_increment_specified_error[] = 
                        "MORE THAN ONE INCREMENT OPTION SPECIFIED";
#ifdef DEBUGFUNC
        char            Syntax_instruction_not_specified_error[] = 
                        "INSTRUCTION NOT SPECIFIED";
        char            Syntax_unexpected_operand_error[] = 
                        "UNEXPECTED INSTRUCTION OPERAND";
        char            Syntax_missing_operand_error[] = 
                        "MISSING INSTRUCTION OPERAND";
        char            Syntax_invalid_operand_error[] = 
                        "INVALID INSTRUCTION OPERAND";
        char            Syntax_operand_out_of_range_error[] = 
                        "INSTRUCTION OPERAND VALUE OUT OF RANGE";
        char            Syntax_invalid_instruction_error[] = 
                        "INVALID INSTRUCTION";
        char            Syntax_invalid_instruction_format_error[] = 
                        "INVALID INSTRUCTION FORMAT";
        char            Syntax_instruction_not_implemented_error[] = 
                        "INSTRUCTION NOT YET IMPLEMENTED";
        char            Syntax_more_than_one_instruction_specified_error[] = 
                        "MORE THAN ONE INSTRUCTION OPTION SPECIFIED";
#endif /* DEBUGFUNC */
        char            Syntax_number_of_times_not_specified_error[] = 
                        "NUMBER_OF_TIMES OPTION USED, BUT NOT SPECIFIED";
        char            Syntax_invalid_number_of_times_error[] = 
                        "INVALID NUMBER_OF_TIMES VALUE";
        char            Syntax_more_than_one_number_of_times_specified_error[] =
                        "MORE THAN ONE NUMBER_OF_TIMES OPTION SPECIFIED";
        char            Syntax_time_delay_not_specified_error[] = 
                        "TIME DELAY OPTION USED, BUT NOT SPECIFIED";
        char            Syntax_invalid_time_delay_error[] = 
                        "INVALID TIME DELAY VALUE";
        char            Syntax_more_than_one_time_delay_specified_error[] =
                        "MORE THAN ONE TIME DELAY OPTION SPECIFIED";
        char            Syntax_verbosity_not_specified_error[] = 
                        "VERBOSITY OPTION USED, BUT NOT SPECIFIED";
        char            Syntax_invalid_verbosity_error[] = 
                        "INVALID VERBOSITY VALUE";
        char            Syntax_more_than_one_verbosity_specified_error[] =
                        "MORE THAN ONE VERBOSITY OPTION SPECIFIED";
        char            Syntax_verbosity_out_of_range_error[] =
                        "VERBOSITY VALUE OUT OF RANGE";
        char            Syntax_write_address_not_specified_error[] = 
                        "WRITE ADDRESS NOT SPECIFIED";
        char            Syntax_invalid_write_address_error[] = 
                        "INVALID WRITE ADDRESS VALUE";
        char            Syntax_more_than_one_write_address_specified_error[] =
                        "MORE THAN ONE WRITE ADDRESS OPTION SPECIFIED";
        char            Syntax_read_address_not_specified_error[] = 
                        "READ ADDRESS OPTION USED, BUT NOT SPECIFIED";
        char            Syntax_invalid_read_address_error[] = 
                        "INVALID READ ADDRESS VALUE";
        char            Syntax_more_than_one_read_address_specified_error[] =
                        "MORE THAN ONE READ ADDRESS OPTION SPECIFIED";
        char            Syntax_compare_data_not_specified_error[] = 
                        "COMPARE DATA OPTION USED, BUT NOT SPECIFIED";
        char            Syntax_invalid_compare_data_error[] = 
                        "INVALID COMPARE DATA VALUE";
        char            Syntax_more_than_one_compare_data_specified_error[] =
                        "MORE THAN ONE COMPARE DATA OPTION SPECIFIED";
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */
        char            Syntax_command_not_specified_error[] = 
                        "COMMAND OPTION USED, BUT NOT SPECIFIED";
        char            Syntax_command_not_found_error[] = 
                        "COMMAND NOT FOUND";
        char            Syntax_no_of_commands_not_specified_error[] = 
                        "NO_OF_COMMANDS OPTION USED, BUT NOT SPECIFIED";
        char            Syntax_invalid_number_of_commands_error[] = 
                        "INVALID NUMBER_OF_COMMANDS VALUE";
        char            Syntax_number_of_commands_out_of_range_error[] = 
                        "NUMBER_OF_COMMANDS VALUE OUT OF RANGE";
        char            Syntax_section_id_not_specified_error[] = 
                        "SECTION_ID OPTION USED, BUT NOT SPECIFIED";
        char            Syntax_section_id_not_found_error[] = 
                        "SECTION_ID NOT FOUND";
        char            Syntax_more_than_one_section_id_specified_error[] = 
                        "MORE THAN ONE SECTION_ID OPTION SPECIFIED";
        char            Syntax_test_id_not_specified_error[] = 
                        "TEST_ID OPTION USED, BUT NOT SPECIFIED";
        char            Syntax_test_id_not_found_error[] = 
                        "TEST_ID NOT FOUND";
        char            Syntax_more_than_one_test_id_specified_error[] = 
                        "MORE THAN ONE TEST_ID OPTION SPECIFIED";
        char            Syntax_both_section_id_and_test_id_specified_error[] = 
                        "BOTH SECTION_ID AND TEST_ID OPTIONS SPECIFIED";
        char            Syntax_parameter_not_specified_error[] = 
                        "PARAMETER NOT SPECIFIED";
        char            Syntax_parameter_not_found_error[] = 
                        "PARAMETER NOT FOUND";
        char            Syntax_parameter_value_not_specified_error[] = 
                        "PARAMETER VALUE NOT SPECIFIED";
        char            Syntax_invalid_parameter_value_error[] = 
                        "INVALID PARAMETER VALUE";
        char            Syntax_invalid_parameter_value_delimiter_error[] = 
                        "INVALID PARAMETER VALUE DELIMITER";
        char            Syntax_parameter_value_out_of_range_error[] = 
                        "PARAMETER VALUE OUT OF RANGE";
        char            Syntax_parameter_length_out_of_range_error[] = 
                        "PARAMETER LENGTH OUT OF RANGE";
#ifndef RISC
        char            Syntax_invalid_date_specified_error[] = 
                        "INVALID DATE VALUE SPECIFIED";
        char            Syntax_invalid_time_specified_error[] = 
                        "INVALID TIME VALUE SPECIFIED";
#endif
#ifdef CALYPSO
        char            Syntax_prompt_string_too_large_error[] = 
                        "PROMPT STRING TOO LARGE";
        char            Syntax_error_code_not_specified_error[] = 
                        "ERROR CODE NOT SPECIFIED";
        char            Syntax_invalid_error_code_error[] = 
                        "INVALID ERROR CODE";
        char            Syntax_error_code_out_of_range_error[] = 
                        "ERROR CODE OUT OF RANGE";
#ifndef BOOT
	char		Syntax_invalid_source_mac_specified_error[] =
			"INVALID SOURCE MAC SPECIFIED ERROR";
	char		Syntax_invalid_destination_mac_specified_error[] =
			"INVALID DESTINATION MAC SPECIFIED ERROR";
#endif /* BOOT */
#endif /* CALYPSO */

	char		*keys_info[] =
	{ "CTL_A - Stop ALL test execution after a Test Break Interrupt",
#ifdef CALYPSO
#ifndef RISC
	  "CTL_B - Invoke DMP Diagnostics with Debug Console Interface",
#endif /* RISC */
#endif /* CALYPSO */
	  "CTL_C - Test Break Interrupt",
#ifdef CALYPSO
#ifndef RISC
	  "CTL_D - Invoke DMP Diagnostics with Admin Console Interface",
#endif /* RISC */
#endif /* CALYPSO */
	  "CTL_H - Toggle Help Facility between Functional/Alphabetical Format",
#ifdef CALYPSO
#ifdef RISC
	  "CTL_N - Invoke ACP Diagnostics with Admin Console Interface",
#endif /* RISC */
#endif /* CALYPSO */
	  "CTL_P - Toggle MORE facility On/Off",
	  "CTL_R - Refresh and Clear Screen in VT100 Terminal mode",
	  "CTL_T - Toggle Clock Display On/Off in VT100 Terminal mode",
#ifdef CALYPSO
#ifndef RISC
	  "CTL_U - Stop Heart-beat Update in the NMP<->DMP Message Area",
#endif /* RISC */
#endif /* CALYPSO */
	  "CTL_V - Toggle between VT100/non-VT100 Terminal modes",
	  "" };

	char		help_description[] = 
"Help       - Provide Command Help Information";
	char		history_description[] =
"History    - Provide Command History Information";

#ifdef DIAGTEST
	char		contents_description[] =
"Contents   - Provide Diagnostic Test Contents Information";
#ifndef RISC
	char		set_description[] =
"Set        - Set Diagnostic Section/Test Parameter Values or Date/Time/Prompt";
#else /* RISC */
	char		set_description[] =
"Set        - Set Diagnostic Section/Test Parameter Values or Prompt";
#endif /* RISC */
	char		drivers_description[] =
"Drivers    - Initialize Device Drivers Used for Diagnostics";
	char		reset_description[] =
"Reset      - Reset Diagnostic Section/Test Parameters to Default Values";
	char		show_description[] =
"Show       - Show Diagnostic Section(s)/Test(s) Parameter Values";
	char		run_description[] =
"Run        - Run Diagnostic Section(s)/Test(s)";
	char		skip_description[] =
"Skip       - Skip Diagnostic Section(s)/Test(s) in RUN ALL Command";
	char		results_description[] =
"Results    - Display Diagnostic Section(s)/Test(s) Execution Results";
#endif /* DIAGTEST */

#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
	char		write_description[] =
"Write      - Write Data in one or more Address locations";
	char		read_description[] =
"Read       - Read Data from one or more Address locations";
	char		compare_description[] =
"Compare    - Compare Data with Contents of one or more Address locations";
	char		search_description[] =
"Search     - Search Data in one or more Address locations";
#ifdef DEBUGFUNC
#ifndef RISC
	char		asm_description[] =
"Asm        - Assemble M68000 instructions in one or more Memory locations";
	char		dasm_description[] =
"Dasm       - Disassemble M68000 machine codes from one or more Memory locations";
	char		execute_description[] =
"Execute    - Execute M68000 machine codes from one or more Memory locations";
#else /* RISC */
	char		asm_description[] =
"Asm        - Assemble R3081 instructions in one or more Memory locations";
	char		dasm_description[] =
"Dasm       - Disassemble R3081 machine codes from one or more Memory locations";
	char		execute_description[] =
"Execute    - Execute R3081 machine codes from one or more Memory locations";
#endif /* RISC */
#endif /* DEBUGFUNC */
	char		test_description[] =
"Test       - Test one Address location performing R/W Operation";
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */

#ifdef LOADGEN
	char		transmit_description[] =
"Transmit   - Transmit Packets along the specified Path(s)";
	char		receive_description[] =
"Receive    - Receive Packets along the specified Path(s)";
	char		loopback_description[] =
"Loopback   - Loopback Packets along the specified Path(s)";
	char		txrx_description[] =
"Transceive - Transceive Packets along the specified Path(s)";
#endif /* LOADGEN */

	char		version_description[] =
"Version    - Display Diagnostics Version information";
#ifdef CALYPSO
	char		errorinfo_description[] =
"Errorinfo  - Display Diagnostics Error Code information";
#endif /* CALYPSO */
	char		keys_description[] =
"Keys       - Display Control Keys Usage during Diagnostics Session";
	char		clear_description[] =
"Clear      - Clear Screen";

	char		*help_syntax[] =
{ "Usage: HE[LP] ALL | [-c<command>] [-c<command>] ...",
  "      <command> - One of the Commands queried thru HELP",
  "                  e.g. help -cshow         -> To get Help on SHOW",
  "                       help -cwrite -cread -> To get Help on WRITE/READ", 
  "      ALL       - Provide detailed help on all Commands",
  NULL };
	char		*history_syntax[] =
{ "Usage: HIS[TORY] [-n<no_of_commands>]",
  "      <no_of_commands> - Number of Commands desired in the range 1 thru 100",
  NULL };

#ifdef	DIAGTEST
	char		*contents_syntax[] =
{ "Usage: CON[TENTS] [-s<section_id>] [-s<section_id>] ...",
  "      <section_id> - One of Diagnostic Sections queried thru CONTENTS",
  NULL };
	char		*drivers_syntax[] =
{ "Usage: DRIV[ERS]",
  NULL };
	char		*set_syntax[] =
{ "Usage: SET ? | [-s<section_id> | -t<test_id>] -p<parameter>=<value> ...",
#ifndef RISC
  "       SET DATE YY/MM/DD",
  "       SET TIME HH:MM:SS",
#endif /* RISC */
#ifdef CALYPSO
  "       SET PROMPT <prompt_str>",
#ifndef BOOT
  "       SET PAUSE [ON | OFF]",
#ifndef RISC
  "       SET PASSWORD",
#endif /* RISC */
  "       SET SMAC <source_mac_address>",
  "       SET DMAC <destination_mac_address>",
#endif /* BOOT */
#endif /* CALYPSO */
  "      <section_id> - One of Diagnostic Section ids queried thru SET ?",
  "      <test_id>    - One of Diagnostic Test ids queried thru SET ?",
  "      <parameter>  - One of Diagnostic Section/Test Parameters",
  "      <value>      - Diagnostic Section/Test Parameter Value,",
  "                     * -> Maximum permissible Value,",
  "                     % -> Default Value",
#ifndef RISC
  "      YY/MM/DD     - Date in the range 93/01/01 thru 99/12/31",
  "      HH:MM:SS     - Time in the range 00:00:00 thru 23:59:59",
#endif /* RISC */
#ifdef CALYPSO
  "      <prompt_str> - Prompt String of 10 characters maximum, should end with",
  "                     a '$' for command number to be displayed",
#endif /* CALYPSO */
  NULL };
	char		*reset_syntax[] =
{ "Usage: RESE[T] ? | ALL | [-s<section_id> | -t<test_id>] [-p<parameter>] ...",
#ifdef CALYPSO
  "       RESE[T] PROMPT",
#ifndef RISC
  "       RESE[T] NMP | DMP",
#endif /* RISC */
#ifndef BOOT
  "       RESE[T] SMAC",
  "       RESE[T] DMAC",
#endif /* BOOT */
#endif /* CALYPSO */
  "      <section_id> - One of Diagnostic Section ids queried thru RESET ?",
  "      <test_id>    - One of Diagnostic Test ids queried thru RESET ?",
  "      <parameter>  - One of Diagnostic Section/Test Parameters",
  "      ALL          - Reset all Diagnostic Section/Test parameters to default",
  NULL };
	char		*show_syntax[] =
{ "Usage: SH[OW] ? | ALL | [-s<section_id> | -t<test_id>] ...",
  "       SH[OW] DATE",
  "       SH[OW] TIME",
#ifdef CALYPSO
#ifndef BOOT
  "       SH[OW] SMAC",
  "       SH[OW] DMAC",
#endif /* BOOT */
#endif /* CALYPSO */
  "      <section_id> - One of Diagnostic Section ids queried thru SHOW ?",
  "      <test_id>    - One of Diagnostic Test ids queried thru SHOW ?",
  "      ALL          - Show all Diagnostic Section/Test parameter values",
  NULL };
	char		*run_syntax[] =
#ifdef CALYPSO
#ifndef BOOT
{ "Usage: RUN ? | ! | ALL | EXHAUSTIVE | POWERUP | -s<section_id> | -t<test_id> ..",
#else /* BOOT */
{ "Usage: RUN ? | ! | ALL | EXHAUSTIVE | -s<section_id> | -t<test_id> ...",
#endif /* BOOT */
#else /* CALYPSO */
{ "Usage: RUN ? | ! | ALL | EXHAUSTIVE | -s<section_id> | -t<test_id> ...",
#endif /* CALYPSO */
  "      <section_id> - One of Diagnostic Section ids queried thru RUN ?",
  "      <test_id>    - One of Diagnostic Test ids queried thru RUN ?",
  "      ALL          - Run all Diagnostic Section(s)/Test(s)",
  "      EXHAUSTIVE   - Run all Diagnostic Section(s)/Test(s) exhaustively",
#ifdef CALYPSO
  "      POWERUP      - Run Power-up Diagnostic Section(s)/Test(s)",
#endif /* CALYPSO */
  "      !            - Run the last executed run-list",
  NULL };
	char		*skip_syntax[] =
{ "Usage: SK[IP] ? | NONE | -s<section_id> | -t<test_id> ...",
  "      <section_id> - One of Diagnostic Section ids queried thru SKIP ?",
  "      <test_id>    - One of Diagnostic Test ids queried thru SKIP ?",
  "      NONE         - Skip none of the Diagnostic Section/Test in RUN ALL",
  NULL };
	char		*results_syntax[] =
#ifdef CALYPSO
{ "Usage: RESU[LTS] ? | ALL | ERRORS | ERRCODE | [-s<section_id> | -t<test_id>] ..",
#else /* CALYPSO */
{ "Usage: RESU[LTS] ? | ALL | ERRORS | [-s<section_id> | -t<test_id>] ..",
#endif /* CALYPSO */
  "      <section_id> - One of Diagnostic Section ids queried thru RESULTS ?",
  "      <test_id>    - One of Diagnostic Test ids queried thru RESULTS ?",
  "      ALL          - Display all Diagnostic Section/Test Execution results",
  "      ERRORS       - Display Diagnostic Section/Test Error results",
#ifdef CALYPSO
  "      ERRCODE      - Display Diagnostic Section/Test Error Code results",
#endif /* CALYPSO */
  NULL };
#endif /* DIAGTEST */

#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
	char		*write_syntax[] =
{ "Usage: W[RITE] -s<start_address> [-e<end_address> | -l<length>] -d<data>",
  "               [-a<access>] [-m<mode>] [-i<increment>]",
  "               [-n<no_of_times>] [-t<time_delay>] [-v<verbosity>]",
  "      <start_address> - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <end_address>   - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <length>        - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <data>          - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <access>        - String Value, Byte/Word/Long",
  "      <mode>          - String Value, Normal/Increment/Complement/Shift/Random",
  "      <increment>     - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <no_of_times>   - Decimal Value in the range 1 thru 99999999",
  "      <time_delay>    - Decimal Value in the range 0 thru 99999999",
  "      <verbosity>     - Decimal Value in the range 0 thru 3",
  NULL };
	char		*read_syntax[] =
{ "Usage: REA[D] -s<start_address> [-e<end_address> | -l<length>] [-a<access>]",
  "              [-n<no_of_times>] [-t<time_delay>] [-v<verbosity>]",
  "      <start_address> - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <end_address>   - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <length>        - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <access>        - String Value, Byte/Word/Long",
  "      <no_of_times>   - Decimal Value in the range 1 thru 99999999",
  "      <time_delay>    - Decimal Value in the range 0 thru 99999999",
  "      <verbosity>     - Decimal Value in the range 0 thru 3",
  NULL };
	char		*compare_syntax[] =
{ "Usage: COM[PARE] -s<start_address> [-e<end_address> | -l<length>] -d<data>",
  "                 [-a<access>] [-m<mode>] [-i<increment>]",
  "                 [-n<no_of_times>] [-t<time_delay>] [-v<verbosity>]",
  "      <start_address> - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <end_address>   - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <length>        - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <data>          - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <access>        - String Value, Byte/Word/Long",
  "      <mode>          - String Value, Normal/Increment/Complement/Shift/Random",
  "      <increment>     - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <no_of_times>   - Decimal Value in the range 1 thru 99999999",
  "      <time_delay>    - Decimal Value in the range 0 thru 99999999",
  "      <verbosity>     - Decimal Value in the range 0 thru 3",
  NULL };
	char		*search_syntax[] =
{ "Usage: SEA[RCH] -s<start_address> [-e<end_address> | -l<length>] -d<data>",
  "                [-a<access>] [-n<no_of_times>] [-t<time_delay>] [-v<verbosity>]",
  "      <start_address> - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <end_address>   - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <length>        - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <data>          - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <access>        - String Value, Byte/Word/Long",
  "      <no_of_times>   - Decimal Value in the range 1 thru 99999999",
  "      <time_delay>    - Decimal Value in the range 0 thru 99999999",
  "      <verbosity>     - Decimal Value in the range 0 thru 3",
  NULL };
#ifdef DEBUGFUNC
	char		*asm_syntax[] =
{ "Usage: A[SM] -s<start_address> [-e<end_address> | -l<length>] -i<instruction>",
  "      <start_address> - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <end_address>   - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <length>        - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
#ifndef RISC
  "      <instruction>   - M68000 family instruction",
#else /* RISC */
  "      <instruction>   - R3081 family instruction",
#endif /* RISC */
  NULL };
	char		*dasm_syntax[] =
{ "Usage: DA[SM] -s<start_address> [-e<end_address> | -l<length>]",
  "      <start_address> - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <end_address>   - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <length>        - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  NULL };
	char		*execute_syntax[] =
{ "Usage: EXE[CUTE] -s<start_address> [-e<end_address> | -l<length>]",
  "                 [-n<no_of_times>] [-t<time_delay>] [-v<verbosity>]",
#ifdef CALYPSO
#ifdef RISC
  "       EXE[CUTE] NMP",
#else /* RISC */
  "       EXE[CUTE] DMP [VCONSOLE | DCONSOLE]",
#endif /* RISC */
#endif /* CALYPSO */
  "      <start_address> - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <end_address>   - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <length>        - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <no_of_times>   - Decimal Value in the range 1 thru 99999999",
  "      <time_delay>    - Decimal Value in the range 0 thru 99999999",
  "      <verbosity>     - Decimal Value in the range 0 thru 3",
  NULL };
#endif /* DEBUGFUNC */
	char		*test_syntax[] =
{ "Usage: TE[ST] -w<write_address> [-r<read_address>] -d<data> [-c<compare_data>]",
  "              [-a<access>] [-n<no_of_times>] [-t<time_delay>] [-v<verbosity>]",
  "      <write_address> - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <read_address>  - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <data>          - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <compare_data>  - Hex Value in the range 0x00000000 thru 0xFFFFFFFF",
  "      <access>        - String Value, Byte/Word/Long",
  "      <no_of_times>   - Decimal Value in the range 1 thru 99999999",
  "      <time_delay>    - Decimal Value in the range 0 thru 99999999",
  "      <verbosity>     - Decimal Value in the range 0 thru 3",
  NULL };
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */

#ifdef LOADGEN
	char		*transmit_syntax[] =
{ "Usage: TRANSM[IT] ???",
  NULL };
	char		*receive_syntax[] =
{ "Usage: REC[EIVE] ???",
  NULL };
	char		*loopback_syntax[] =
{ "Usage: L[OOPBACK] ???",
  NULL };
	char		*txrx_syntax[] =
{ "Usage: TRANSC[EIVE] ???",
  NULL };
#endif /* LOADGEN */

	char		*version_syntax[] =
{ "Usage: VER[SION]",
  NULL };
#ifdef CALYPSO
	char		*errorinfo_syntax[] =
{ "Usage: ERR[ORINFO] [-c<error_code>] [-c<error_code>] ...",
  "      <error_code> - Diagnostic Error Code in the range 1 thru 106",
  NULL };
#endif /* CALYPSO */
	char		*keys_syntax[] =
{ "Usage: KEYS",
  NULL };
	char		*clear_syntax[] =
{ "Usage: CLEAR",
  NULL };
	char		*exit_syntax[] =
{ "Usage: EXIT",
  NULL };

#ifdef CALYPSO
extern	int		Errorinfo();
#endif /* CALYPSO */
extern	int		Help();
extern	int		History();
extern	int		Keys();
extern	int		Version();
	struct	COMMAND_TABLE	info_command_table[] =
#ifdef CALYPSO
	{ { "ERRORINFO", errorinfo_description, errorinfo_syntax, 3, Errorinfo,
	    ON },
	  { "HELP",    help_description,    help_syntax,    2, Help,    ON },
#else /* CALYPSO */
	{ { "HELP",    help_description,    help_syntax,    2, Help,    ON },
#endif /* CALYPSO */
	  { "HISTORY", history_description, history_syntax, 3, History, ON },
	  { "KEYS",      keys_description, keys_syntax, 4, Keys, ON },
	  { "VERSION",   version_description, version_syntax, 3, Version, ON },
	  { NULL, NULL, (char **) 0, 0 } };

#ifdef DIAGTEST
extern	int		Contents();
extern	int		Drivers();
extern	int		Reset();
extern	int		Results();
extern	int		Run();
extern	int		Set();
extern	int		Show();
extern	int		Skip();
	struct	COMMAND_TABLE	diag_command_table[] =
	{ { "CONTENTS", contents_description, contents_syntax, 3, Contents,ON },
	  { "DRIVERS",  drivers_description,  drivers_syntax,  4, Drivers, ON },
	  { "RESET",    reset_description,    reset_syntax,    4, Reset,   ON },
	  { "RESULTS",  results_description,  results_syntax,  4, Results, ON },
	  { "RUN",      run_description,      run_syntax,      3, Run,     ON },
	  { "SET",      set_description,      set_syntax,      3, Set,     ON },
	  { "SHOW",     show_description,     show_syntax,     2, Show,    ON },
	  { "SKIP",     skip_description,     skip_syntax,     2, Skip,    ON },
	  { NULL, NULL, (char **) 0, 0 } };
#endif /* DIAGTEST */

#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
extern	int		Write();
extern	int		Read();
extern	int		Compare();
extern	int		Search();
#ifdef DEBUGFUNC
extern	int		Asm();
extern	int		Dasm();
extern	int		Execute();
#endif /* DEBUGFUNC */
extern	int		Test();

/*------------------------------------------------------------------
 */
	struct	COMMAND_TABLE	debug_command_table[] =
#ifdef DEBUGFUNC
	{ { "ASM",     asm_description,     asm_syntax,     1, Asm,     ON },
	  { "COMPARE", compare_description, compare_syntax, 3, Compare, ON },
	  { "DASM",    dasm_description,    dasm_syntax,    2, Dasm,    ON },
	  { "EXECUTE", execute_description, execute_syntax, 3, Execute, ON },
#else /* DEBUGFUNC */
	{ { "COMPARE", compare_description, compare_syntax, 3, Compare, ON },
#endif /* DEBUGFUNC */
	  { "READ",    read_description,    read_syntax,    3, Read,    ON },
	  { "SEARCH",  search_description,  search_syntax,  3, Search,  ON },
	  { "TEST",    test_description,    test_syntax,    2, Test,    ON },
	  { "WRITE",   write_description,   write_syntax,   1, Write,   ON },
	  { NULL, NULL, (char **) 0, 0 } };
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */

#ifdef LOADGEN
extern	int		Transmit();
extern	int		Receive();
extern	int		Loopback();
extern	int		Txrx();
	struct	COMMAND_TABLE	loadgen_command_table[] =
	{ { "TRANSMIT",   transmit_description, transmit_syntax, 6, Transmit, ON },
	  { "RECEIVE",    receive_description,  receive_syntax,  3, Receive,  ON },
	  { "LOOPBACK",   loopback_description, loopback_syntax, 1, Loopback, ON },
	  { "TRANSCEIVE", txrx_description,     txrx_syntax,     6, Txrx,     ON },
	  { NULL, NULL, (char **) 0, 0 } };
#endif /* LOADGEN */

extern	int		Clear();
extern	int		Exit();
	struct	COMMAND_TABLE	misc_command_table[] =
	{ { "CLEAR",     clear_description, clear_syntax, 5, Clear, ON },
	  { NULL, NULL, (char **) 0, 0, } };

