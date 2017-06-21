/* $Id: debug.c,v 1.1.4.1 1996/05/09 15:00:53 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/debug.c,v $
 *-----------------------------------------------------------------
 * debug.c -- Debug functions
 *
 * ?
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: debug.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:53  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:51  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:13  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:02  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:46  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:21  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************/
/*                                                                            */
/* DIAGNOSTICS DRIVER                                                         */
/*                                                                            */
/* This file contains procedures related to "Debug functions" that are        */
/* included as part of Diagnostics Driver.                                    */
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

#include	"ctype.h"
#include	"types.h"
#include	"constant.h"
#include	"globdef.h"
#include	"struct.h"
#include	"hw.h"

#define	DIAG_READ_WRITE_FAIL_ERROR	-1

#define  ATM

extern	enum USER_LEVEL	cli_user_level;
extern	char		*cli_display_buf_ptr[];
extern	char		cli_print_buf[];

extern	int		sys_clock_display;
extern	int		sys_term_vt100;

extern	volatile uInt32	system_tick_time;

#ifdef DIAGTEST
extern	char		*diag_test_name;
extern	char		*diag_test_id;
extern	uInt32		diag_verbosity;
extern	uInt32		diag_current_runcount;

extern	char		sys_specific_info[];
extern	uInt32		sys_fail_address;
extern	uInt32		sys_data_expected;
extern	uInt32		sys_data_read;
extern	uInt32		sys_error_code;
extern	int		diag_test_mode;

#define	IRMEMWI		0x2
#define	IRMEMWO		0x3
#endif /* DIAGTEST */

#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
#include	"debug.h"
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */

#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
	char		debug_break_msg[] =
	"--Break-- in DEBUG, Press <Y> to Confirm, <space> to Resume ...\n";
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */
#ifdef DIAGTEST
	char		*test_break_msg[] =
	{ "--Break-- in %s Test {%s} -> %u\n",
	  "   Press <CTL_A> to Quit All Tests\n",
	  "         <CTL_C> to Quit Current Test\n",
	  "         <CTL_D> to Perform Debug\n",
	  "         <CTL_R> to View Current Test Results\n",
	  "         <space> to Resume ...\n",
	  "" };
	char		all_quit_msg[] = "Quitting All Tests\n";
	char		test_quit_msg[] = "Quitting %s Test {%s} -> %u\n";
	char		test_resume_msg[] = "Resuming %s Test {%s} -> %u\n";
#endif /* DIAGTEST */

        char            *Access_type[] = 
	{ "Byte", "Word", "Long" };

        char            *Mode_type[] = 
	{ "Normal", "Increment", "Complement", "Shift", "Random" };

	char		write_progress_msg[] =
	" %s Write @%08lX in %s mode";

#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
	char		read_progress_msg[] =
	" %s Read @%08lX -> %u\n";
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */

	char		compare_progress_msg[] =
	" %s Compare @%08lX in %s mode";
#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
	char		compare_fail_msg[] =
	" Compare failed with the following discrepancies ...\n";

	char		search_progress_msg[] =
	" %s Search @%08lX -> %u\n";
	char		search_pass_msg[] =
" The Contents of the following Address locations match the specified data ...\n";
	char		execute_progress_msg[] =
	" Execute @%08lX -> %u\n";

#ifdef RISC
	int		switchtoNMP = FALSE;
#else /* RISC */
	int		switchtoDMP_DConsole = FALSE;
	int		switchtoDMP_VConsole = FALSE;
#endif /* RISC */

	char		*get_access();
	char		*get_mode();
	void		fill_ascii_chars();
	void		fill_opwords();
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */
	void		display_progress();
	uInt32		get_next_data();

#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
/*----------------------------------------------------------------
 * Write()
 *----------------------------------------------------------------*/
int Write(
	int		argc,
	char		*argv[])
{
   return(debug(argc, argv));
}


/*----------------------------------------------------------------
 * Read()
 *----------------------------------------------------------------*/
int Read(
	int		argc,
	char		*argv[])
{
	return(debug(argc, argv));
}

/*----------------------------------------------------------------
 * Compare()
 *----------------------------------------------------------------*/
int Compare(
	int		argc,
	char		*argv[])
{
	return(debug(argc, argv));
}

/*----------------------------------------------------------------
 * Search()
 *----------------------------------------------------------------*/
int Search(
	int		argc,
	char		*argv[])
{
	return(debug(argc, argv));
}

#ifdef DEBUGFUNC
/*----------------------------------------------------------------
 * Asm()
 *----------------------------------------------------------------*/
int Asm(
	int		argc,
	char		*argv[])
{
	return(debug(argc, argv));
}

/*----------------------------------------------------------------
 * Dasm()
 *----------------------------------------------------------------*/
int Dasm(
	int		argc,
	char		*argv[])
{
	return(debug(argc, argv));
}

/*----------------------------------------------------------------
 * Execute()
 *----------------------------------------------------------------*/
int Execute(
	int		argc,
	char		*argv[])
{
#ifdef RISC
	if (argc == 2 && !strcmp_nocase(argv[1], "nmp")) {
	   switchtoNMP = TRUE;
	   return(0);
	}
#else /* RISC */
	if ((argc == 2 || argc == 3) && !strcmp_nocase(argv[1], "dmp")) {
	   if (argc == 3 && !strcmp_nocase(argv[2], "dconsole"))
	      switchtoDMP_DConsole = TRUE;
	   else
	      switchtoDMP_VConsole = TRUE;
	   return(0);
	}
#endif /* RISC */
	return(debug(argc, argv));
}
#endif /* DEBUGFUNC */

/*----------------------------------------------------------------
 * Test()
 *----------------------------------------------------------------*/
int Test(
	int		argc,
	char		*argv[])
{
	return(debug(argc, argv));
}

/*----------------------------------------------------------------
 * debug()
 *----------------------------------------------------------------*/
int debug(
	int		argc,
	char		*argv[])
{
	register int	i;
	register char	debug_command_type, *argp, *access, *mode, *inst;
	register int	argc_min, argc_max;
	uInt32 		start_address, end_address, length, data, increment;
	uInt32 		num, time_delay, verbosity;
	uInt32 		write_address, read_address, compare_data, addr;
	enum FLAG_TYPE	start_address_specified,
			end_address_specified,
			length_specified,
			data_specified,
			access_specified,
			mode_specified,
			increment_specified,
			inst_specified,
			num_specified,
			time_delay_specified,
			verbosity_specified,
			write_address_specified,
			read_address_specified,
			compare_data_specified;

	if (argc == 1) {
	   return(SYNTAX_MISSING_ARGUMENTS_ERROR);
	}

	debug_command_type = *argv[0];
	if (isalpha(debug_command_type) && isupper(debug_command_type))
	   debug_command_type = tolower(debug_command_type);

	switch(debug_command_type) {
	   case DEBUG_COMMAND_TYPE_WRITE:
	      argc_min = MINIMUM_WRITE_ARGUMENTS;
	      argc_max = MAXIMUM_WRITE_ARGUMENTS;
	      break;
	   case DEBUG_COMMAND_TYPE_READ:
	      argc_min = MINIMUM_READ_ARGUMENTS;
	      argc_max = MAXIMUM_READ_ARGUMENTS;
	      break;
	   case DEBUG_COMMAND_TYPE_COMPARE:
	      argc_min = MINIMUM_COMPARE_ARGUMENTS;
	      argc_max = MAXIMUM_COMPARE_ARGUMENTS;
	      break;
	   case DEBUG_COMMAND_TYPE_SEARCH:
	      argc_min = MINIMUM_SEARCH_ARGUMENTS;
	      argc_max = MAXIMUM_SEARCH_ARGUMENTS;
	      break;
#ifdef DEBUGFUNC
	   case DEBUG_COMMAND_TYPE_ASM:
	      argc_min = MINIMUM_ASM_ARGUMENTS;
	      argc_max = MAXIMUM_ASM_ARGUMENTS;
	      break;
	   case DEBUG_COMMAND_TYPE_DASM:
	      argc_min = MINIMUM_DASM_ARGUMENTS;
	      argc_max = MAXIMUM_DASM_ARGUMENTS;
	      break;
	   case DEBUG_COMMAND_TYPE_EXECUTE:
	      argc_min = MINIMUM_EXECUTE_ARGUMENTS;
	      argc_max = MAXIMUM_EXECUTE_ARGUMENTS;
	      break;
#endif /* DEBUGFUNC */
	   case DEBUG_COMMAND_TYPE_TEST:
	      argc_min = MINIMUM_TEST_ARGUMENTS;
	      argc_max = MAXIMUM_TEST_ARGUMENTS;
	      break;
	}
	if (argc < argc_min) {
	   return(SYNTAX_TOO_FEW_ARGUMENTS_ERROR);
	}
	/*if (argc > argc_max) {
	   return(SYNTAX_TOO_MANY_ARGUMENTS_ERROR);
	}*/

	for (start_address_specified      =
	     end_address_specified        =
	     length_specified             =
	     data_specified               =
	     access_specified             =
	     mode_specified               =
	     increment_specified          =
	     inst_specified        	  =
	     num_specified                =
	     time_delay_specified         =
	     verbosity_specified         =
	     write_address_specified      =
	     read_address_specified       =
	     compare_data_specified       = FALSE, i = 1; 
	     i < argc && !inst_specified; i++) {
	   argp = argv[i];
	   if (*argp++ != '-') {
	      return(SYNTAX_INVALID_ARGUMENT_DELIMITER_ERROR);
	   }
	   switch(*argp++) {
	      case ARGUMENT_OPTION_START_ADDRESS:
		 if (debug_command_type != DEBUG_COMMAND_TYPE_TEST) {
	            if (start_address_specified)
	               return(SYNTAX_MORE_THAN_ONE_START_ADDRESS_SPECIFIED_ERROR);
	            if (*argp == '\0') {
	               if (i == (argc-1))
	                  return(SYNTAX_START_ADDRESS_NOT_SPECIFIED_ERROR);
	               argp = argv[++i];
	            }
		    if (!sscanf_x(argp, fmt_xInt32, &start_address))
		       return(SYNTAX_INVALID_START_ADDRESS_ERROR);
		    start_address_specified = TRUE;
		 }
		 else
		    return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
		 break;
	      case ARGUMENT_OPTION_END_ADDRESS:
		 if (debug_command_type != DEBUG_COMMAND_TYPE_TEST) {
	            if (end_address_specified)
	               return(SYNTAX_MORE_THAN_ONE_END_ADDRESS_SPECIFIED_ERROR);
	            if (*argp == '\0') {
	               if (i == (argc-1))
	                  return(SYNTAX_END_ADDRESS_NOT_SPECIFIED_ERROR);
	               argp = argv[++i];
	            }
		    if (!sscanf_x(argp, fmt_xInt32, &end_address))
		       return(SYNTAX_INVALID_END_ADDRESS_ERROR);
		    end_address_specified = TRUE;
		 }
		 else
		    return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
		 break;
	      case ARGUMENT_OPTION_LENGTH:
		 if (debug_command_type != DEBUG_COMMAND_TYPE_TEST) {
	            if (length_specified)
	               return(SYNTAX_MORE_THAN_ONE_LENGTH_SPECIFIED_ERROR);
	            if (*argp == '\0') {
	               if (i == (argc-1))
	                  return(SYNTAX_LENGTH_NOT_SPECIFIED_ERROR);
	               argp = argv[++i];
	            }
		    if (!sscanf_x(argp, fmt_xInt32, &length))
		       return(SYNTAX_INVALID_LENGTH_ERROR);
		    length_specified = TRUE;
		 }
		 else
		    return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
		 break;
	      case ARGUMENT_OPTION_DATA:
		 if (debug_command_type == DEBUG_COMMAND_TYPE_WRITE   ||
		     debug_command_type == DEBUG_COMMAND_TYPE_COMPARE ||
		     debug_command_type == DEBUG_COMMAND_TYPE_SEARCH  ||
		     debug_command_type == DEBUG_COMMAND_TYPE_TEST) {
	            if (data_specified)
	               return(SYNTAX_MORE_THAN_ONE_DATA_SPECIFIED_ERROR);
	            if (*argp == '\0') {
	               if (i == (argc-1))
	                  return(SYNTAX_DATA_NOT_SPECIFIED_ERROR);
	               argp = argv[++i];
	            }
		    if (!sscanf_x(argp, fmt_xInt32, &data))
		       return(SYNTAX_INVALID_DATA_ERROR);
		    data_specified = TRUE;
		 }
		 else
		    return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
		 break;
	      case ARGUMENT_OPTION_ACCESS:
		 if (debug_command_type == DEBUG_COMMAND_TYPE_WRITE   ||
		     debug_command_type == DEBUG_COMMAND_TYPE_READ    ||
		     debug_command_type == DEBUG_COMMAND_TYPE_COMPARE ||
		     debug_command_type == DEBUG_COMMAND_TYPE_SEARCH  ||
		     debug_command_type == DEBUG_COMMAND_TYPE_TEST) {
	            if (access_specified)
	               return(SYNTAX_MORE_THAN_ONE_ACCESS_SPECIFIED_ERROR);
	            if (*argp == '\0') {
	               if (i == (argc-1))
	                  return(SYNTAX_ACCESS_NOT_SPECIFIED_ERROR);
	               argp = argv[++i];
	            }
		    if ((access = get_access(argp)) == NULL)
		       return(SYNTAX_INVALID_ACCESS_ERROR);
		    access_specified = TRUE;
		 }
		 else
		    return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
		 break;
	      case ARGUMENT_OPTION_MODE:
		 if (debug_command_type == DEBUG_COMMAND_TYPE_WRITE   ||
		     debug_command_type == DEBUG_COMMAND_TYPE_COMPARE) {
	            if (mode_specified)
	               return(SYNTAX_MORE_THAN_ONE_MODE_SPECIFIED_ERROR);
	            if (*argp == '\0') {
	               if (i == (argc-1))
	                  return(SYNTAX_MODE_NOT_SPECIFIED_ERROR);
	               argp = argv[++i];
	            }
		    if ((mode = get_mode(argp)) == NULL)
		       return(SYNTAX_INVALID_MODE_ERROR);
		    mode_specified = TRUE;
		 }
		 else
		    return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
		 break;
	      case ARGUMENT_OPTION_INCREMENT_OR_INSTRUCTION:
		 if (debug_command_type == DEBUG_COMMAND_TYPE_WRITE   ||
		     debug_command_type == DEBUG_COMMAND_TYPE_COMPARE) {
	            if (increment_specified)
	               return(SYNTAX_MORE_THAN_ONE_INCREMENT_SPECIFIED_ERROR);
	            if (*argp == '\0') {
	               if (i == (argc-1))
	                  return(SYNTAX_INCREMENT_NOT_SPECIFIED_ERROR);
	               argp = argv[++i];
	            }
		    if (!sscanf_x(argp, fmt_xInt32, &increment))
		       return(SYNTAX_INVALID_INCREMENT_ERROR);
		    increment_specified = TRUE;
		 }
		 else {
#ifdef DEBUGFUNC
		    if (debug_command_type == DEBUG_COMMAND_TYPE_ASM) {
	               if (inst_specified)
	                  return(SYNTAX_MORE_THAN_ONE_INSTRUCTION_SPECIFIED_ERROR);
	               if (*argp == '\0') {
	                  if (i == (argc-1))
	                     return(SYNTAX_INSTRUCTION_NOT_SPECIFIED_ERROR);
	                  argp = argv[++i];
	               }
		       inst = argp;
		       inst_specified = TRUE;
		    }
		    else
#endif /* DEBUGFUNC */
		       return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
		 }
		 break;
	      case ARGUMENT_OPTION_NUMBER_OF_TIMES:
		 if (debug_command_type != DEBUG_COMMAND_TYPE_ASM &&
		     debug_command_type != DEBUG_COMMAND_TYPE_DASM) {
	            if (num_specified)
	               return(SYNTAX_MORE_THAN_ONE_NUMBER_OF_TIMES_SPECIFIED_ERROR);
	            if (*argp == '\0') {
	               if (i == (argc-1))
	                  return(SYNTAX_NUMBER_OF_TIMES_NOT_SPECIFIED_ERROR);
	               argp = argv[++i];
	            }
		    if (!sscanf_x(argp, fmt_uInt32, &num))
		       return(SYNTAX_INVALID_NUMBER_OF_TIMES_ERROR);
		    num_specified = TRUE;
		 }
		 else
		    return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
		 break;
	      case ARGUMENT_OPTION_TIME_DELAY:
		 if (debug_command_type != DEBUG_COMMAND_TYPE_ASM &&
		     debug_command_type != DEBUG_COMMAND_TYPE_DASM) {
	            if (time_delay_specified)
	               return(SYNTAX_MORE_THAN_ONE_TIME_DELAY_SPECIFIED_ERROR);
	            if (*argp == '\0') {
	               if (i == (argc-1))
	                  return(SYNTAX_TIME_DELAY_NOT_SPECIFIED_ERROR);
	               argp = argv[++i];
	            }
		    if (!sscanf_x(argp, fmt_uInt32, &time_delay))
		       return(SYNTAX_INVALID_TIME_DELAY_ERROR);
		    time_delay_specified = TRUE;
		 }
		 else
		    return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
		 break;
	      case ARGUMENT_OPTION_VERBOSITY:
		 if (debug_command_type != DEBUG_COMMAND_TYPE_ASM &&
		     debug_command_type != DEBUG_COMMAND_TYPE_DASM) {
	            if (verbosity_specified)
	               return(SYNTAX_MORE_THAN_ONE_VERBOSITY_SPECIFIED_ERROR);
	            if (*argp == '\0') {
	               if (i == (argc-1))
	                  return(SYNTAX_VERBOSITY_NOT_SPECIFIED_ERROR);
	               argp = argv[++i];
	            }
		    if (!sscanf_x(argp, fmt_uInt32, &verbosity))
		       return(SYNTAX_INVALID_VERBOSITY_ERROR);
	            if (verbosity > VERBOSITY_LEVEL3)
	               return(SYNTAX_VERBOSITY_OUT_OF_RANGE_ERROR);
		    verbosity_specified = TRUE;
		 }
		 else
		    return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
		 break;
	      case ARGUMENT_OPTION_WRITE_ADDRESS:
		 if (debug_command_type == DEBUG_COMMAND_TYPE_TEST) {
	            if (write_address_specified)
	               return(SYNTAX_MORE_THAN_ONE_WRITE_ADDRESS_SPECIFIED_ERROR);
	            if (*argp == '\0') {
	               if (i == (argc-1))
	                  return(SYNTAX_WRITE_ADDRESS_NOT_SPECIFIED_ERROR);
	               argp = argv[++i];
	            }
		    if (!sscanf_x(argp, fmt_xInt32, &write_address))
		       return(SYNTAX_INVALID_WRITE_ADDRESS_ERROR);
		    write_address_specified = TRUE;
		 }
		 else
		    return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
		 break;
	      case ARGUMENT_OPTION_READ_ADDRESS:
		 if (debug_command_type == DEBUG_COMMAND_TYPE_TEST) {
	            if (read_address_specified)
	               return(SYNTAX_MORE_THAN_ONE_READ_ADDRESS_SPECIFIED_ERROR);
	            if (*argp == '\0') {
	               if (i == (argc-1))
	                  return(SYNTAX_READ_ADDRESS_NOT_SPECIFIED_ERROR);
	               argp = argv[++i];
	            }
		    if (!sscanf_x(argp, fmt_xInt32, &read_address))
		       return(SYNTAX_INVALID_READ_ADDRESS_ERROR);
		    read_address_specified = TRUE;
		 }
		 else
		    return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
		 break;
	      case ARGUMENT_OPTION_COMPARE_DATA:
		 if (debug_command_type == DEBUG_COMMAND_TYPE_TEST) {
	            if (compare_data_specified)
	               return(SYNTAX_MORE_THAN_ONE_COMPARE_DATA_SPECIFIED_ERROR);
	            if (*argp == '\0') {
	               if (i == (argc-1))
	                  return(SYNTAX_COMPARE_DATA_NOT_SPECIFIED_ERROR);
	               argp = argv[++i];
	            }
		    if (!sscanf_x(argp, fmt_xInt32, &compare_data))
		       return(SYNTAX_INVALID_COMPARE_DATA_ERROR);
		    compare_data_specified = TRUE;
		 }
		 else
		    return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
		 break;
	      default:
		 return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
	   }
	}

	if (debug_command_type != DEBUG_COMMAND_TYPE_TEST) {
	   if (!start_address_specified)
	      return(SYNTAX_START_ADDRESS_NOT_SPECIFIED_ERROR);
	}

	if (length_specified) {
	   if (end_address_specified)
	      return(SYNTAX_BOTH_END_ADDRESS_AND_LENGTH_SPECIFIED_ERROR);
	   else
	      end_address = start_address + length - 1;
	}
	else
	   if (!end_address_specified)
	      end_address = start_address;

	if (debug_command_type == DEBUG_COMMAND_TYPE_WRITE   ||
	    debug_command_type == DEBUG_COMMAND_TYPE_COMPARE ||
	    debug_command_type == DEBUG_COMMAND_TYPE_SEARCH  ||
	    debug_command_type == DEBUG_COMMAND_TYPE_TEST) {
	   if (!data_specified)
	      return(SYNTAX_DATA_NOT_SPECIFIED_ERROR);
	}

	if (!access_specified)
#ifdef RISC
	   access = Access_type[2];
#else /* RISC */
	   access = Access_type[1];
#endif /* RISC */

	if (!mode_specified)
	   mode = Mode_type[0];

	if (!increment_specified)
	   increment = 1;

#ifdef DEBUGFUNC
	if (debug_command_type == DEBUG_COMMAND_TYPE_ASM) {
	   if (!inst_specified)
	      return(SYNTAX_INSTRUCTION_NOT_SPECIFIED_ERROR);
	}
#endif /* DEBUGFUNC */

	if (!num_specified)
	   num = 1;

	if (!time_delay_specified)
	   time_delay = 0;

	if (!verbosity_specified)
	   verbosity = VERBOSITY_LEVEL1;

	if (debug_command_type == DEBUG_COMMAND_TYPE_TEST) {
	   if (!write_address_specified)
	      return(SYNTAX_WRITE_ADDRESS_NOT_SPECIFIED_ERROR);
	   if (!read_address_specified)
	      read_address = write_address;
	   if (!compare_data_specified)
	      compare_data = data;
	}

	if (start_address > end_address) {
	   addr = start_address;
	   start_address = end_address;
	   end_address = addr;
	}

	switch(*access) {
	   case ACCESS_TYPE_BYTE:
	      data &= 0xFF; compare_data &= 0xFF; increment &= 0xFF;
	      break;
	   case ACCESS_TYPE_WORD:
	      data &= 0xFFFF; compare_data &= 0xFFFF; increment &= 0xFFFF;
	      start_address &= ~0x1; end_address &= ~0x1;
	      write_address &= ~0x1; read_address &= ~0x1; 
	      break;
	   case ACCESS_TYPE_LONG:
#if 0 /* RLB DBG */
		start_address &= ~0x3; end_address &= ~0x3;
	      write_address &= ~0x3; read_address &= ~0x3; 
   
#endif /* RLB DBG */
	      break;

	}

#ifdef RISC
	if (debug_command_type == DEBUG_COMMAND_TYPE_ASM ||
	    debug_command_type == DEBUG_COMMAND_TYPE_DASM ||
	    debug_command_type == DEBUG_COMMAND_TYPE_EXECUTE) {
	   start_address &= ~0x3; end_address &= ~0x3;
	}
#endif /* RISC */

	switch (debug_command_type) {
	   case DEBUG_COMMAND_TYPE_WRITE:
	      return(perform_write(start_address, end_address, data, access,
				   mode, increment, num, time_delay, verbosity,
	                           (uInt32) 0, DEBUG_CALL));
	   case DEBUG_COMMAND_TYPE_READ:
	      return(perform_read(start_address, end_address, access,
				  num, time_delay, verbosity));
	   case DEBUG_COMMAND_TYPE_COMPARE:
	      return(perform_compare(start_address, end_address, data, access,
				     mode, increment, num, time_delay,verbosity,
	                             (uInt32) 0, DEBUG_CALL));
	   case DEBUG_COMMAND_TYPE_SEARCH:
	      return(perform_search(start_address, end_address, data, access,
				    num, time_delay, verbosity));
#ifdef DEBUGFUNC
	   case DEBUG_COMMAND_TYPE_ASM:
	      return(perform_asm(start_address, end_address, inst, 
	                         argc, argv, i));
	   case DEBUG_COMMAND_TYPE_DASM:
	      return(perform_dasm(start_address, end_address));
	   case DEBUG_COMMAND_TYPE_EXECUTE:
	      return(perform_execute(start_address, end_address,
				     num, time_delay, verbosity));
#endif /* DEBUGFUNC */
	   case DEBUG_COMMAND_TYPE_TEST:
	      return(perform_test(write_address, read_address, data,
				  compare_data, access, num, time_delay, 
	                          verbosity));
	}
}
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */

int perform_write(
        uInt32          start_address,
        uInt32          end_address,
        uInt32          data,
        char            *access,
        char            *mode,
        uInt32          increment,
        uInt32          num,
        uInt32          time_delay,
	uInt32		verbosity,
	uInt32		sys_info,
	enum CALL_TYPE	call_type)
{
	uInt32		start_time, end_time;
	register uInt32	addr, n, write_data;
	register int	rc;
	register uInt16	*mac_marw, *mac_mdru, *mac_mdrl;
	register struct MAC_REGS *mac_ptr;
#ifdef RISC
	register struct BIGA_REGS *biga_ptr;
	register uInt32	*biga_sfq;
	register int	biga_int32;
	/*register uInt32	sram_ptr;*/
#endif /* RISC */

	if (*mode == MODE_TYPE_RANDOM)
	   srand(data);

#ifdef DIAGTEST
	if (call_type == MACBUFRW_CALL || 
	    call_type == MACINTLB_CALL ||
	    call_type == MACEXTLB_CALL ||
#ifdef RISC
	    call_type == BIGARXDMA_CALL ||
#endif /* RISC */
	    call_type == RINGLB_CALL) {
	   mac_ptr = (struct MAC_REGS *) sys_info;
	   mac_marw = (uInt16 *) &mac_ptr->r30.lw.marw;
	   mac_mdru = (uInt16 *) &mac_ptr->data_reg.wd.mdru;
	   mac_mdrl = (uInt16 *) &mac_ptr->data_reg.wd.mdrl;
	   *mac_marw = start_address >> 2;
#ifdef RISC
	   if (call_type == BIGARXDMA_CALL)
              if ((start_address & 0x1) && *access == ACCESS_TYPE_BYTE)
	         write_data = 0xCC;
#endif /* RISC */
	}

#ifdef RISC
	if (call_type == BIGABUFRW_CALL) {
	   biga_ptr = (struct BIGA_REGS *) sys_info;
	   biga_sfq = (uInt32 *) &biga_ptr->r46.sfq;
	   /*sram_ptr = 0x80000000;*/
#define	BIGA_START	0xA8800000
	   if (biga_ptr == (struct BIGA_REGS *) BIGA_START)
	      biga_int32 = TRUE;
	   else
	      biga_int32 = FALSE;
	}
#endif /* RISC */
#endif /* DIAGTEST */

	for (rc = 0, n = 1; n <= num && rc == 0; n++) {
	   switch(*access) {
	      case ACCESS_TYPE_BYTE:
	         for (addr = start_address; addr <= end_address; addr++) {
	            if (!((addr-start_address)%0x4000)) {
	               if (rc = check_for_break())
	                  break;
#ifdef DIAGTEST
	               if ((call_type == DEBUG_CALL && verbosity) || 
	                   (call_type != DEBUG_CALL && 
	                    diag_verbosity >= VERBOSITY_LEVEL3))
#else /* DIAGTEST */
	               if (verbosity)
#endif /* DIAGTEST */
	                  display_progress(write_progress_msg, access, addr,
	                                   mode, n, call_type);
	            }

#ifdef DIAGTEST
	            if (call_type == MACBUFRW_CALL || 
	                call_type == MACINTLB_CALL ||
	                call_type == MACEXTLB_CALL ||
#ifdef RISC
	                call_type == BIGARXDMA_CALL ||
#endif /* RISC */
	                call_type == RINGLB_CALL) {
	               if (!(addr & 0x1)) {
	                  write_data = data;
	               }
	               else {
	                  write_data = (write_data << 8) | data;
	                  if (!(addr & 0x2)) {
	                     *mac_mdru = write_data;
	                     if (addr == end_address)
	                        *mac_mdrl = 0;
	                  }
	                  else
	                     *mac_mdrl = write_data;
	               }
	            }
	            else
#ifdef MASERATI
	               if (call_type == NVRAMRW_CALL) {
	                  if (!(addr & 0x1))
	                     *((uInt16 *) addr) = data;
	                  else
	                     continue;
	               }
	               else
#endif /* MASERATI */
#ifdef RISC
	                  if (call_type == BIGABUFRW_CALL) {
	                     if (!(addr & 0x2)) {
	                        if (!(addr & 0x1))
	                           write_data = data;
	                        else
	                           write_data = (write_data << 8) | data;
	                     }
	                     else {
	                        write_data = (write_data << 8) | data;
	                        if (addr & 0x1) {
	                           if (biga_int32)
	                              *biga_sfq = write_data;
	                           else {
	                              *biga_sfq = write_data & 0xFFFF0000;
	                              *biga_sfq = write_data << 16;
	                           }
	                           /**((uInt32 *) sram_ptr) = write_data;
	                           sram_ptr += 4;*/
	                        }
	                     }
	                  }
	                  else
#endif /* RISC */
#endif /* DIAGTEST */
	                     *((unsigned char *) addr) = data;

	            if (*mode != MODE_TYPE_NORMAL) {
	               switch (*mode) {
	                  case MODE_TYPE_INCREMENT:
	                     data = (unsigned char) (data + increment);
	                     break;
	                  case MODE_TYPE_COMPLEMENT:
	                     data = (unsigned char) ~data;
	                     break;
	                  case MODE_TYPE_SHIFT:
	                     data = ((unsigned char) (data << 1)) |
	                            ((unsigned char) (data >> 7));
	                     break;
	                  default:
	                     data = (unsigned char) rand();
	                     break;
	               }
	            }
	         }
	         break;
	      case ACCESS_TYPE_WORD:
	         for (addr = start_address; addr <= end_address; addr += 2) {
	            if (!((addr-start_address)%0x4000)) {
	               if (rc = check_for_break())
	                  break;
#ifdef DIAGTEST
	               if ((call_type == DEBUG_CALL && verbosity) || 
	                   (call_type != DEBUG_CALL && 
	                    diag_verbosity >= VERBOSITY_LEVEL3))
#else /* DIAGTEST */
	               if (verbosity)
#endif /* DIAGTEST */
	                  display_progress(write_progress_msg, access, addr,
	                                   mode, n, call_type);
	            }

#ifdef DIAGTEST
	            if (call_type == MACBUFRW_CALL) {
	               if (!(addr & 0x2)) {
	                  *mac_mdru = data;
	                  if (addr == end_address)
	                     *mac_mdrl = 0;
	               }
	               else
	                  *mac_mdrl = data;
	            }
	            else
#ifdef RISC
	               if (call_type == BIGABUFRW_CALL) {
	                  if (!(addr & 0x2))
	                      write_data = data;
	                  else {
	                     if (biga_int32)
	                        *biga_sfq = (write_data << 16) | data;
	                     else {
	                        *biga_sfq = write_data << 16;
	                        *biga_sfq = data << 16;
	                     }
	                   /**((uInt32 *) sram_ptr) = (write_data << 16) | data;
	                     sram_ptr += 4;*/
	                  }
	               }
	               else
#endif /* RISC */
#endif /* DIAGTEST */
	                  *((uInt16 *) addr) = data;

	            if (*mode != MODE_TYPE_NORMAL) {
	               switch (*mode) {
	                  case MODE_TYPE_INCREMENT:
	                     data = (uInt16) (data + increment);
	                     break;
	                  case MODE_TYPE_COMPLEMENT:
	                     data = (uInt16) ~data;
	                     break;
	                  case MODE_TYPE_SHIFT:
	                     data = ((uInt16) (data << 1)) |
	                            ((uInt16) (data >> 15));
	                     break;
	                  default:
	                     data = (uInt16) rand();
	                     break;
	               }
	            }
	         }
	         break;
	      case ACCESS_TYPE_LONG:
	         for (addr = start_address; addr <= end_address; addr += 4) {
	            if (!((addr-start_address)%0x4000)) {
	               if (rc = check_for_break())
	                  break;
#ifdef DIAGTEST
	               if ((call_type == DEBUG_CALL && verbosity) || 
	                   (call_type != DEBUG_CALL && 
	                    diag_verbosity >= VERBOSITY_LEVEL3))
#else /* DIAGTEST */
	               if (verbosity)
#endif /* DIAGTEST */
	                  display_progress(write_progress_msg, access, addr,
	                                   mode, n, call_type);
	            }

#ifdef DIAGTEST
	            if (call_type == MACBUFRW_CALL) {
	               *mac_mdru = data >> 16;
	               *mac_mdrl = data & 0xFFFF;
	            }
	            else
#ifdef RISC
	               if (call_type == BIGABUFRW_CALL) {
	                  if (biga_int32)
	                     *biga_sfq = data;
	                  else {
	                     *biga_sfq = data & 0xFFFF0000;
	                     *biga_sfq = data << 16;
	                  }
	                  /**((uInt32 *) sram_ptr) = data;
	                  sram_ptr += 4;*/
	               }
	               else
#endif /* RISC */
#endif /* DIAGTEST */
	                  *((uInt32 *) addr) = data;

	            if (*mode != MODE_TYPE_NORMAL) {
	               switch (*mode) {
	                  case MODE_TYPE_INCREMENT:
	                     data += increment;
	                     break;
	                  case MODE_TYPE_COMPLEMENT:
	                     data = ~data;
	                     break;
	                  case MODE_TYPE_SHIFT:
	                     data = (data << 1) | (data >> 31);
	                     break;
	                  default:
	                     data = (((uInt32) rand()) << 16) | 
	                             ((uInt32) rand());
	                     break;
	               }
	            }
	         }
	         break;
	   }
	   if (call_type == DEBUG_CALL && rc == 0) {
	      for (start_time = system_tick_time; 
	          (system_tick_time - start_time) < (1000*time_delay); ) {
	         if (rc = check_for_break())
	            break;
	      }
	   }
	}

#ifdef DIAGTEST
	if ((call_type == MACBUFRW_CALL || call_type == MACINTLB_CALL ||
	     call_type == MACEXTLB_CALL || 
#ifdef RISC
	     call_type == BIGARXDMA_CALL ||
#endif /* RISC */
	     call_type == RINGLB_CALL) &&
	     rc == 0 && *access == ACCESS_TYPE_BYTE && !(end_address & 0x1)) {
	   if (!(end_address & 0x2)) {
	      *mac_mdru = (write_data << 8);
	      *mac_mdrl = 0;
	   }
	   else
	      *mac_mdrl = (write_data << 8);
	}
#ifdef RISC
	if (call_type == BIGABUFRW_CALL && rc == 0) {
	   switch (*access) {
	      case ACCESS_TYPE_LONG: break;
	      case ACCESS_TYPE_WORD:
	         if (!(end_address & 0x2)) {
	            if (biga_int32)
	               *biga_sfq = (write_data << 16);
	            else {
	               *biga_sfq = write_data << 16;
	               *biga_sfq = 0x00000000;
	            }
	            /**((uInt32 *) sram_ptr) = (write_data << 16);*/
	         }
	         break;
	      default:
	         if (!(end_address & 0x2)) {
	            if (!(end_address & 0x1)) {
	               if (biga_int32)
	                  *biga_sfq = (write_data << 24);
	               else {
	                  *biga_sfq = (write_data << 24);
	                  *biga_sfq = 0x00000000;
	               }
	               /**((uInt32 *) sram_ptr) = (write_data << 24);*/
	            }
	            else {
	               if (biga_int32)
	                  *biga_sfq = (write_data << 16);
	               else {
	                  *biga_sfq = write_data << 16;
	                  *biga_sfq = 0x00000000;
	               }
	               /**((uInt32 *) sram_ptr) = (write_data << 16);*/
	            }
	         }
	         else {
	            if (!(end_address & 0x1)) {
	               if (biga_int32)
	                  *biga_sfq = (write_data << 8);
	               else {
	                  *biga_sfq = 0x00000000;
	                  *biga_sfq = (write_data << 24);
	               }
	               /**((uInt32 *) sram_ptr) = (write_data << 8);*/
	            }
	         }
	         break;
	   }
	}
#endif /* RISC */
#endif /* DIAGTEST */

        return(rc);
}

#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
int perform_read(
        uInt32          start_address,
        uInt32          end_address,
        char            *access,
        uInt32          num,
        uInt32          time_delay,
	uInt32		verbosity)
{
	uInt32		start_time, end_time, read_data;
	register uInt32	addr, read_addr, n;
	register int	rc, buf_index, buf_len, buf_ovf;
	char		buf[10];
	char		*fmt;

	switch (*access) {
	   case ACCESS_TYPE_BYTE:
	      buf_len = 58; fmt = read_fmt_byte; break;
	   case ACCESS_TYPE_WORD:
	      buf_len = 50; fmt = read_fmt_word; break;
	   case ACCESS_TYPE_LONG:
           default:
	      buf_len = 46;
 fmt = read_fmt_long; break;
           
	}

	buf_ovf = FALSE; buf_index = -1;

	for (rc = 0, n = 1; n <= num && rc == 0; n++) {
	   for (addr = start_address; addr <= end_address && rc == 0; ) {
	      if (!((addr-start_address)%0x4000)) {
	         if (rc = check_for_break())
	            break;
	         if (verbosity) {
	            sprintf(cli_print_buf, read_progress_msg, access, addr, n);
	            put_line(cli_print_buf);
	         }
	      }
	      read_addr = addr;
	      switch(*access) {
	         case ACCESS_TYPE_BYTE:
	            if (rc = byte_read(read_addr, &read_data))
	               break;
	            addr++;
	            break;
	         case ACCESS_TYPE_WORD:
	            if (rc = word_read(read_addr, &read_data))
	               break;
	            addr += 2;
	            break;
	         case ACCESS_TYPE_LONG:
	            if (rc = long_read(read_addr, &read_data))
	               break;
	            addr += 4;
	            break;
	      }
	      sprintf(buf, fmt, read_data);
	      if (verbosity >= VERBOSITY_LEVEL2) {
	         sprintf(cli_print_buf, " %08lX: ", read_addr);
	         strcat(cli_print_buf, buf);
	         strcat(cli_print_buf, "\n");
	         put_line(cli_print_buf);
	      }
	      if (!((read_addr-start_address)%16)) {
	         if (buf_index < (DISPLAY_BUF_SIZE-1))
	            sprintf(cli_display_buf_ptr[++buf_index], " %08lX: ", 
	                    read_addr);
	         else
	            buf_ovf = TRUE;
	      }
	      if (!buf_ovf) {
	         strcat(cli_display_buf_ptr[buf_index], buf);

	      }
	   }

	   if (n < num && rc == 0 && (buf_index < (DISPLAY_BUF_SIZE-1))) {
	      strcpy(cli_display_buf_ptr[++buf_index], "-");
	   }
	}

	fill_ascii_chars(cli_display_buf_ptr, ++buf_index, buf_len);

	print_buf(cli_display_buf_ptr, buf_index);
        return(rc);
}
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */

int perform_compare(
        uInt32          start_address,
        uInt32          end_address,
        uInt32          data,
        char            *access,
        char            *mode,
        uInt32          increment,
        uInt32          num,
        uInt32          time_delay,
	uInt32		verbosity,
	uInt32		sys_info,
	enum CALL_TYPE	call_type)
{
	uInt32		start_time, end_time, read_data;
	register uInt32	addr, n;
	register int	rc, buf_index;
	register char	*fmt;
	register uInt16	*mac_cmdreg1, *mac_marr, *mac_mdru, *mac_mdrl;
	register struct MAC_REGS *mac_ptr;

#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
	switch (*access) {
	   case ACCESS_TYPE_BYTE:
	      fmt = compare_fmt_byte; break;
	   case ACCESS_TYPE_WORD:
	      fmt = compare_fmt_word; break;
	   case ACCESS_TYPE_LONG:
	      fmt = compare_fmt_long; break;
	}
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */

	buf_index = 0;

	if (*mode == MODE_TYPE_RANDOM)
	   srand(data);

#ifdef DIAGTEST
	if (call_type == MACBUFRW_CALL ||
	    call_type == MACINTLB_CALL ||
	    call_type == MACEXTLB_CALL ||
#ifdef RISC
	    call_type == BIGABUFRW_CALL ||
	    call_type == BIGATXDMA_CALL ||
#endif /* RISC */
	    call_type == RINGLB_CALL) {
	   mac_ptr = (struct MAC_REGS *) sys_info;
	   mac_cmdreg1 = (uInt16 *) &mac_ptr->r0.lw.addr0.cmdreg1;
	   mac_marr = (uInt16 *) &mac_ptr->r30.lw.marr;
	   mac_mdru = (uInt16 *) &mac_ptr->data_reg.wd.mdru;
	   mac_mdrl = (uInt16 *) &mac_ptr->data_reg.wd.mdrl;
	}
#endif /* DIAGTEST */

	for (rc = 0, n = 1; n <= num && rc == 0; n++) {
	   switch(*access) {
	      case ACCESS_TYPE_BYTE:
	         for (addr = start_address; addr <= end_address; addr++) {
	            if (!((addr-start_address)%0x4000)) {
	               if (rc = check_for_break())
	                  break;
#ifdef DIAGTEST
	               if ((call_type == DEBUG_CALL && verbosity) || 
	                   (call_type != DEBUG_CALL && 
	                    diag_verbosity >= VERBOSITY_LEVEL3))
#else /* DIAGTEST */
	               if (verbosity)
#endif /* DIAGTEST */
	                  display_progress(compare_progress_msg, access, addr,
	                                   mode, n, call_type);
	            }

#ifdef DIAGTEST
	            if (call_type == MACBUFRW_CALL ||
	                call_type == MACINTLB_CALL ||
	                call_type == MACEXTLB_CALL ||
#ifdef RISC
	                call_type == BIGABUFRW_CALL ||
	                call_type == BIGATXDMA_CALL ||
#endif /* RISC */
	                call_type == RINGLB_CALL) {
	               *mac_marr = addr >> 2;
	               *mac_cmdreg1 = IRMEMWO;
	               if (!(addr & 0x2)) {
	                  read_data = *mac_mdru;
	               }
	               else {
	                  read_data = *mac_mdrl;
	               }
	               if (!(addr & 0x1))
	                  read_data >>= 8;
	               else
	                  read_data &= 0xFF;
	            }
	            else
#ifdef MASERATI
	               if (call_type == NVRAMRW_CALL) {
	                  if (!(addr & 0x1))
	                     read_data = *((uInt16 *) addr) & 0xFF;
	                  else
	                     continue;
	               }
	               else
#endif /* MASERATI */
#endif /* DIAGTEST */
	                  read_data = *((unsigned char *) addr);

	            if (read_data != data) {
#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
	               if (call_type == DEBUG_CALL) {
	                  if (buf_index < (DISPLAY_BUF_SIZE-1)) {
	                     sprintf(cli_display_buf_ptr[++buf_index], fmt,
	                             addr, data, read_data);
	                  }
	               }
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */
#ifdef DIAGTEST
	               if (call_type != DEBUG_CALL) {
	                  sys_fail_address = addr;
	                  sys_data_expected = data;
	                  sys_data_read = read_data;
	                  sys_error_code = DIAG_READ_WRITE_FAIL_ERROR;
	                  if (check_for_test_stop())
	                     return(rc = sys_error_code);
	               }
#endif /* DIAGTEST */
	            }
	            if (*mode != MODE_TYPE_NORMAL) {
	               switch (*mode) {
	                  case MODE_TYPE_INCREMENT:
	                     data = (unsigned char) (data + increment);
	                     break;
	                  case MODE_TYPE_COMPLEMENT:
	                     data = (unsigned char) ~data;
	                     break;
	                  case MODE_TYPE_SHIFT:
	                     data = ((unsigned char) (data << 1)) |
	                            ((unsigned char) (data >> 7));
	                     break;
	                  default:
	                     data = (unsigned char) rand();
	                     break;
	               }
	            }
	         }
	         break;
	      case ACCESS_TYPE_WORD:
	         for (addr = start_address; addr <= end_address; addr += 2) {
	            if (!((addr-start_address)%0x4000)) {
	               if (rc = check_for_break())
	                  break;
#ifdef DIAGTEST
	               if ((call_type == DEBUG_CALL && verbosity) || 
	                   (call_type != DEBUG_CALL && 
	                    diag_verbosity >= VERBOSITY_LEVEL3))
#else /* DIAGTEST */
	               if (verbosity)
#endif /* DIAGTEST */
	                  display_progress(compare_progress_msg, access, addr,
	                                   mode, n, call_type);
	            }

#ifdef DIAGTEST
#ifdef RISC
	            if (call_type == MACBUFRW_CALL ||
	                call_type == BIGABUFRW_CALL) {
#else /* RISC */
	            if (call_type == MACBUFRW_CALL) {
#endif /* RISC */
	               *mac_marr = addr >> 2;
	               *mac_cmdreg1 = IRMEMWO;
	               if (!(addr & 0x2))
	                  read_data = *mac_mdru;
	               else
	                  read_data = *mac_mdrl;
	            }
	            else
#endif /* DIAGTEST */
	               read_data = *((uInt16 *) addr);

	            if (read_data != data) {
#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
	               if (call_type == DEBUG_CALL) {
	                  if (buf_index < (DISPLAY_BUF_SIZE-1)) {
	                     sprintf(cli_display_buf_ptr[++buf_index], fmt,
	                             addr, data, read_data);
	                  }
	               }
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */
#ifdef DIAGTEST
	               if (call_type != DEBUG_CALL) {
	                  sys_fail_address = addr;
	                  sys_data_expected = data;
	                  sys_data_read = read_data;
	                  sys_error_code = DIAG_READ_WRITE_FAIL_ERROR;
	                  if (check_for_test_stop())
	                     return(rc = sys_error_code);
	               }
#endif /* DIAGTEST */
	            }
	            if (*mode != MODE_TYPE_NORMAL) {
	               switch (*mode) {
	                  case MODE_TYPE_INCREMENT:
	                     data = (uInt16) (data + increment);
	                     break;
	                  case MODE_TYPE_COMPLEMENT:
	                     data = (uInt16) ~data;
	                     break;
	                  case MODE_TYPE_SHIFT:
	                     data = ((uInt16) (data << 1)) |
	                            ((uInt16) (data >> 15));
	                     break;
	                  default:
	                     data = (uInt16) rand();
	                     break;
	               }
	            }
	         }
	         break;
	      case ACCESS_TYPE_LONG:
	         for (addr = start_address; addr <= end_address; addr += 4) {
	            if (!((addr-start_address)%0x4000)) {
	               if (rc = check_for_break())
	                  break;
#ifdef DIAGTEST
	               if ((call_type == DEBUG_CALL && verbosity) || 
	                   (call_type != DEBUG_CALL && 
	                    diag_verbosity >= VERBOSITY_LEVEL3))
#else /* DIAGTEST */
	               if (verbosity)
#endif /* DIAGTEST */
	                  display_progress(compare_progress_msg, access, addr,
	                                   mode, n, call_type);
	            }

#ifdef DIAGTEST
#ifdef RISC
	            if (call_type == MACBUFRW_CALL ||
	                call_type == BIGABUFRW_CALL) {
#else /* RISC */
	            if (call_type == MACBUFRW_CALL) {
#endif /* RISC */
	               *mac_marr = addr >> 2;
	               *mac_cmdreg1 = IRMEMWO;
	               read_data = *mac_mdru << 16 | *mac_mdrl;
	            }
	            else
#endif /* DIAGTEST */
	               read_data = *((uInt32 *) addr);

	            if (read_data != data) {
#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
	               if (call_type == DEBUG_CALL) {
	                  if (buf_index < (DISPLAY_BUF_SIZE-1)) {
	                     sprintf(cli_display_buf_ptr[++buf_index], fmt,
	                             addr, data, read_data);
	                  }
	               }
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */
#ifdef DIAGTEST
	               if (call_type != DEBUG_CALL) {
	                  sys_fail_address = addr;
	                  sys_data_expected = data;
	                  sys_data_read = read_data;
	                  sys_error_code = DIAG_READ_WRITE_FAIL_ERROR;
	                  if (check_for_test_stop())
	                     return(rc = sys_error_code);
	               }
#endif /* DIAGTEST */
	            }
	            if (*mode != MODE_TYPE_NORMAL) {
	               switch (*mode) {
	                  case MODE_TYPE_INCREMENT:
	                     data += increment;
	                     break;
	                  case MODE_TYPE_COMPLEMENT:
	                     data = ~data;
	                     break;
	                  case MODE_TYPE_SHIFT:
	                     data = (data << 1) | (data >> 31);
	                     break;
	                  default:
	                     data = (((uInt32) rand()) << 16) | 
	                             ((uInt32) rand());
	                     break;
	               }
	            }
	         }
	         break;
	   }
	   if (call_type == DEBUG_CALL && rc == 0) {
	      for (start_time = system_tick_time; 
	          (system_tick_time - start_time) < (1000*time_delay); ) {
	         if (rc = check_for_break())
	            break;
	      }
	      if (n < num && rc == 0 && (buf_index < (DISPLAY_BUF_SIZE-1))) {
	         strcpy(cli_display_buf_ptr[++buf_index], "-");
	      }
	   }
	}

#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
	if (call_type == DEBUG_CALL) {
	   if (buf_index) {
	      strcpy(cli_display_buf_ptr[0], compare_fail_msg);
	      print_buf(cli_display_buf_ptr, ++buf_index);
	   }
	}
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */

        return(rc);
}

#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
int perform_search(
        uInt32          start_address,
        uInt32          end_address,
        uInt32          data,
        char            *access,
        uInt32          num,
        uInt32          time_delay,
	uInt32		verbosity)
{
	uInt32		start_time, end_time, read_data;
	register uInt32	addr, read_addr, n;
	register int	rc, buf_index, col, buf_ovf;
	char		buf[10];

	buf_ovf = FALSE; buf_index = 0;

	for (rc = 0, n = 1; n <= num && rc == 0; n++) {
	   for (addr = start_address, col=0; addr <= end_address && rc == 0; ) {
	      if (!((addr-start_address)%0x4000)) {
	         if (rc = check_for_break())
	            break;
	         if (verbosity) {
	            sprintf(cli_print_buf, search_progress_msg, access, addr,n);
	            put_line(cli_print_buf);
	         }
	      }
	      read_addr = addr;
	      switch(*access) {
	         case ACCESS_TYPE_BYTE:
	            if (rc = byte_read(read_addr, &read_data))
	               break;
	            addr++;
	            break;
	         case ACCESS_TYPE_WORD:
	            if (rc = word_read(read_addr, &read_data))
	               break;
	            addr += 2;
	            break;
	         case ACCESS_TYPE_LONG:
	            if (rc = long_read(read_addr, &read_data))
	               break;
	            addr += 4;
	            break;
	      }
	      if (read_data == data) {
	         if (!buf_ovf) {
	            if (!(col%8)) {
	               if (buf_index < (DISPLAY_BUF_SIZE-1))
	                  sprintf(cli_display_buf_ptr[++buf_index], " %08lX ",
	                          read_addr);
	               else
	                  buf_ovf = TRUE;
	            }
	            else {
	               sprintf(buf, "%08lX%c", read_addr,
	                      ((col+1)%8) ? ' ' : '\n');
	               strcat(cli_display_buf_ptr[buf_index], buf);
	            }
	            col++;
	         }
	      }
	   }
	   for (start_time = system_tick_time; 
	       (system_tick_time - start_time) < (1000*time_delay)&&rc == 0; ) {
	      if (rc = check_for_break())
	         break;
	   }
	   if (buf_index) {
	      if (!buf_ovf && (col%8))
	         strcat(cli_display_buf_ptr[buf_index], "\n");
	      if (n < num && rc == 0 && (buf_index < (DISPLAY_BUF_SIZE-1))) {
	         strcpy(cli_display_buf_ptr[++buf_index], "-");
	      }
	   }
	}

	if (buf_index) {
	   strcpy(cli_display_buf_ptr[0], search_pass_msg);
	   print_buf(cli_display_buf_ptr, ++buf_index);
	}

        return(rc);
}

#ifdef DEBUGFUNC
#ifndef RISC
int perform_asm(
        uInt32          start_address,
        uInt32          end_address,
	char		*inst,
	int		argc,
	char		*argv[],
	int		operand_index)
{
	register uInt32	addr;
	register int	rc, opcode, inst_index, inst_len, buf_index, i, m_flag;
	uInt16		m_code[10];
	register uInt32	w_data;
	uInt32		r_data;
	char		inst_buf[80];
	register char	*operand_ptr;

	for (opcode = 0; opcode < 16; opcode++) {
	   for (inst_index = 0; instruction[opcode][inst_index][0] != '\0'; 
	        inst_index++) {
	      if (!strcmp_nocase(inst, instruction[opcode][inst_index]))
	         break;
	   }
	   if (instruction[opcode][inst_index][0] != '\0')
	      break;
	}
	if (opcode == 16)
	   return(SYNTAX_INVALID_INSTRUCTION_ERROR);

	strcpy(inst_buf, inst);
	for (i = operand_index; i < argc; i++) {
	   strcat(inst_buf, " ");
	   strcat(inst_buf, argv[i]);
	}
	if (operand_index < argc) {
	   for (i = 0; inst_buf[i] != ' '; i++);
	   operand_ptr = &inst_buf[++i];
	}
	else
	   operand_ptr = NULL;

	switch (opcode) {
	   case 0:
	      inst_len = asm_0(inst_index, operand_ptr, m_code);
	      break;
	   case 1: case 2: case 3:
	      inst_len = asm_1_2_3(inst_index, operand_ptr, m_code);
	      break;
	   case 4:
	      inst_len = asm_4(inst_index, operand_ptr, m_code);
	      break;
	   case 5:
	      inst_len = asm_5(inst_index, operand_ptr, m_code);
	      break;
	   case 6:
	      inst_len = asm_6(inst_index, operand_ptr, m_code);
	      break;
	   case 7:
	      inst_len = asm_7(inst_index, operand_ptr, m_code);
	      break;
	   case 8: case 12:
	      inst_len = asm_8_c(inst_index, operand_ptr, m_code);
	      break;
	   case 9: case 11: case 13:
	      inst_len = asm_9_b_d(inst_index, operand_ptr, m_code);
	      break;
	   case 10:
	      inst_len = asm_a(inst_index, operand_ptr, m_code);
	      break;
	   case 14:
	      inst_len = asm_e(inst_index, operand_ptr, m_code);
	      break;
	   case 15:
	      inst_len = asm_f(inst_index, operand_ptr, m_code);
	      break;
	}
	if (inst_len < 0) {
	   switch (inst_len) {
	      case ASM_INSTRUCTION_NOT_IMPLEMENTED:
	         return(SYNTAX_INSTRUCTION_NOT_IMPLEMENTED_ERROR);
	      case ASM_MISSING_OPERAND:
	         return(SYNTAX_MISSING_OPERAND_ERROR);
	      case ASM_INVALID_OPERAND:
	         return(SYNTAX_INVALID_OPERAND_ERROR);
	      case ASM_OPERAND_OUT_OF_RANGE:
	         return(SYNTAX_OPERAND_OUT_OF_RANGE_ERROR);
	      case ASM_INVALID_INSTRUCTION_FORMAT:
	         return(SYNTAX_INVALID_INSTRUCTION_FORMAT_ERROR);
	   }
	}

	buf_index = 0;
	for (addr = start_address, buf_index = 0; 
	     addr <= end_address && buf_index < DISPLAY_BUF_SIZE;
	     buf_index++) {
	   for (i = 0, m_flag = FALSE; i < inst_len; i += 2) {
	      w_data = (uInt32) m_code[i/2];
	      if (rc = word_read(addr+i, &r_data))
	         return(rc);
	      if (rc = word_write(addr+i, w_data))
	         return(rc);
	      if (r_data != w_data)
	         m_flag = TRUE;
	   }
	   sprintf(cli_display_buf_ptr[buf_index], "%c%08lX: ",
	           (m_flag) ? '*' : ' ', addr);
	   fill_opwords(cli_display_buf_ptr[buf_index], addr, inst_len/2);
	   strcat(cli_display_buf_ptr[buf_index], inst_buf);
	   strcat(cli_display_buf_ptr[buf_index], "\n");
	   addr += inst_len;
	}

	print_buf(cli_display_buf_ptr, buf_index);

        return(rc);
}

int perform_dasm(
        uInt32          start_address,
        uInt32          end_address)
{
	register uInt32	addr;
	uInt32		opword;
	register int	rc, inst_len, buf_index;
	char		inst_buf[80];

	buf_index = 0;
	for (addr = start_address, buf_index = 0; 
	     addr <= end_address && buf_index < DISPLAY_BUF_SIZE;
	     buf_index++) {
	   if (rc = word_read(addr, &opword))
	      return(rc);
	   switch (opword & OPCODE_MASK) {
	      case OPCODE_0000:
	         inst_len = dasm_0(addr, inst_buf);
	         break;
	      case OPCODE_0001: case OPCODE_0010: case OPCODE_0011:
	         inst_len = dasm_1_2_3(addr, inst_buf);
	         break;
	      case OPCODE_0100:
	         inst_len = dasm_4(addr, inst_buf);
	         break;
	      case OPCODE_0101:
	         inst_len = dasm_5(addr, inst_buf);
	         break;
	      case OPCODE_0110:
	         inst_len = dasm_6(addr, inst_buf);
	         break;
	      case OPCODE_0111:
	         inst_len = dasm_7(addr, inst_buf);
	         break;
	      case OPCODE_1000: case OPCODE_1100:
	         inst_len = dasm_8_c(addr, inst_buf);
	         break;
	      case OPCODE_1001: case OPCODE_1011: case OPCODE_1101:
	         inst_len = dasm_9_b_d(addr, inst_buf);
	         break;
	      case OPCODE_1010:
	         inst_len = dasm_a(addr, inst_buf);
	         break;
	      case OPCODE_1110:
	         inst_len = dasm_e(addr, inst_buf);
	         break;
	      case OPCODE_1111:
	         inst_len = dasm_f(addr, inst_buf);
	         break;
	   }
	   if (inst_len == -1) {
	      sprintf(cli_display_buf_ptr[buf_index], 
	              " %08lX: %04X%15c%-8s $%04X\n", addr, opword, ' ',
	              "dc.w", opword);
	      addr += 2;
	   }
	   else {
	      sprintf(cli_display_buf_ptr[buf_index], " %08lX: ", addr);
	      fill_opwords(cli_display_buf_ptr[buf_index], addr, inst_len/2);
	      strcat(cli_display_buf_ptr[buf_index], inst_buf);
	      strcat(cli_display_buf_ptr[buf_index], "\n");
	      addr += inst_len;
	   }
	}

	print_buf(cli_display_buf_ptr, buf_index);

        return(rc);
}
#else /* RISC */
int perform_asm(
        uInt32          start_address,
        uInt32          end_address,
	char		*inst,
	int		argc,
	char		*argv[],
	int		operand_index)
{
	uInt32		addr;
	int		rc, op, funct, rt, rs, rd, shamt;
	int		buf_index, i, m_flag, nop_flag;
	union instruction_type	mc;
	uInt32		w_data, r_data;
	uInt32		target;
	Int32		immediate;
	char		inst_buf[80];
	char		*operand_ptr, *ptr;

	if (!strcmp_nocase(inst, "nop")) {
	   nop_flag = TRUE;
	   operand_ptr = NULL;
	}
	else {
	   nop_flag = FALSE;
	   for (op = 0; op < 64; op++) {
	      if (!strcmp_nocase(inst, instruction[2][op]))
	         break;
	   }
	   if (op == 64) {
	      for (funct = 0; funct < 64; funct++) {
	         if (!strcmp_nocase(inst, instruction[0][funct])) {
	            op = OPCODE_SPECIAL;
	            break;
	         }
	         if (funct < 32&&!strcmp_nocase(inst, 
	                                        cop_instruction[2][funct])) {
	            op = OPCODE_COP0;
	            rs = 16;
	            break;
	         }
	      }
	   }
	   if (op == 64) {
	      for (rt = 0; rt < 32; rt++) {
	         if (!strcmp_nocase(inst, instruction[1][rt])) {
	            op = OPCODE_BCOND;
	            break;
	         }
	         if (!strcmp_nocase(inst, cop_instruction[1][rt])) {
	            op = OPCODE_COP0+rt/8;
	            rs = COP_BC;
	            break;
	         }
	      }
	   }
	   if (op == 64) {
	      for (rs = 0; rs < 32; rs++) {
	         if (!strcmp_nocase(inst, cop_instruction[0][rs])) {
	            op = OPCODE_COP0+rs/8;
	            rs %= 8;
	            break;
	         }
	      }
	   }
	   if (op == 64)
	      return(SYNTAX_INVALID_INSTRUCTION_ERROR);

	   strcpy(inst_buf, inst);
	   for (i = operand_index; i < argc; i++) {
	      strcat(inst_buf, " ");
	      strcat(inst_buf, argv[i]);
	   }
	   if (operand_index < argc) {
	      for (i = 0; inst_buf[i] != ' '; i++);
	      operand_ptr = &inst_buf[++i];
	   }
	   else
	      operand_ptr = NULL;
	}

	for (addr = start_address, buf_index = 0; 
	     addr <= end_address && buf_index < DISPLAY_BUF_SIZE;
	     buf_index++, addr += 4) {
	   rc = mc.mc = 0; ptr = operand_ptr;

	   if (nop_flag == FALSE) {
	      switch (op) {
	         case OPCODE_SPECIAL:
	            mc.r_type.op = op; mc.r_type.funct = funct;
	            if (funct == SPECIAL_SYSCALL || funct == SPECIAL_BREAK) {
	               if (ptr != NULL) {
	                  rc = ASM_UNEXPECTED_OPERAND;
	               }
	            }
	            else {
	               if (ptr == NULL) {
	                  rc = ASM_MISSING_OPERAND;
	               }
	               else
	                  rc = asm_special(&mc, ptr);
	            }
	            break;
	         case OPCODE_BCOND:
	            mc.i_type.op = op; mc.i_type.rt = rt;
	            if (ptr == NULL) {
	               rc = ASM_MISSING_OPERAND;
	            }
	            else
	               rc = asm_bcond(&mc, ptr, addr);
	            break;
	         case OPCODE_COP0: case OPCODE_COP1: 
	         case OPCODE_COP2: case OPCODE_COP3:
	            mc.r_type.op = op;
	            mc.r_type.rs = rs;
	            if (funct < 32) { /* TLBR/TLBWI/TLBWR/TLBP/RFE */
	               if (ptr != NULL) {
	                  rc = ASM_UNEXPECTED_OPERAND;
	               }
	               else {
	                  mc.r_type.funct = funct;
	               }
	               break;
	            }
	            if (ptr == NULL) {
	               rc = ASM_MISSING_OPERAND; break;
	            }
	            if (rt < 32) { /* BC0F/BC0T */
	               if (*ptr == '$')
	                  ptr++;
	               if (!sscanf_x(ptr, fmt_xInt32, &target)) {
	                  rc = ASM_INVALID_OPERAND; break;
	               }
	               immediate = target - addr - 4;
	               if (immediate & 0x3) {
	                  rc = ASM_INVALID_OPERAND; break;
	               }
	               immediate >>= 2;
	               if ((immediate & 0xFFFF0000) != 0 &&
	                   (immediate & 0xFFFF0000) != 0xFFFF0000) {
	                  rc = ASM_OPERAND_OUT_OF_RANGE; break;
	               }
	               mc.i_type.rt = rt%2;
	               mc.i_type.immediate = immediate;
	               break;
	            }
	            rc = asm_cop0(&mc, ptr);
	            break;
	         default:
	            mc.r_type.op = op;
	            if (ptr == NULL) {
	               rc = ASM_MISSING_OPERAND;
	            }
	            else
	               rc = asm_default(&mc, ptr, addr);
	            break;
	      }
   	      switch (rc) {
	         case ASM_INSTRUCTION_NOT_IMPLEMENTED:
	            return(SYNTAX_INSTRUCTION_NOT_IMPLEMENTED_ERROR);
	         case ASM_UNEXPECTED_OPERAND:
	            return(SYNTAX_UNEXPECTED_OPERAND_ERROR);
	         case ASM_MISSING_OPERAND:
	            return(SYNTAX_MISSING_OPERAND_ERROR);
	         case ASM_INVALID_OPERAND:
	            return(SYNTAX_INVALID_OPERAND_ERROR);
	         case ASM_OPERAND_OUT_OF_RANGE:
	            return(SYNTAX_OPERAND_OUT_OF_RANGE_ERROR);
	         case ASM_INVALID_INSTRUCTION_FORMAT:
	            return(SYNTAX_INVALID_INSTRUCTION_FORMAT_ERROR);
	         default: break;
	      }
	   }

	   w_data = mc.mc;
	   if (rc = long_read(addr, &r_data))
	      return(rc);
	   if (rc = long_write(addr, w_data))
	      return(rc);
	   if (r_data != w_data)
	      m_flag = TRUE;
	   else
	      m_flag = FALSE;
	   sprintf(cli_display_buf_ptr[buf_index], 
	           "%c%08lX: %08X%11c%-8s ", (m_flag) ? '*' : ' ', addr, 
	           mc.mc, ' ', inst);
	   if (operand_ptr != NULL)
	      strcat(cli_display_buf_ptr[buf_index], operand_ptr);
	   strcat(cli_display_buf_ptr[buf_index], "\n");
	}

	print_buf(cli_display_buf_ptr, buf_index);

        return(rc);
}

int perform_dasm(
        uInt32          start_address,
        uInt32          end_address)
{
	uInt32		addr;
	union instruction_type	*mc;
	uInt32		opword;
	int		rc, buf_index;
	char		inst_buf[80];

	buf_index = 0;
	for (addr = start_address, buf_index = 0; 
	     addr <= end_address && buf_index < DISPLAY_BUF_SIZE;
	     buf_index++, addr += 4) {
	   mc = (union instruction_type *) addr;
	   if (mc->mc == 0x00000000) {
	      rc = 0; strcpy(inst_buf, "nop");
	   }
	   else {
	      switch (mc->all.op) {
	         case OPCODE_SPECIAL:
	            rc = dasm_special(addr, inst_buf);
	            break;
	         case OPCODE_BCOND:
	            rc = dasm_bcond(addr, inst_buf);
	            break;
	         default:
	            rc = dasm_default(addr, inst_buf);
	            break;
	      }
	   }
	   if (rc)
	      sprintf(cli_display_buf_ptr[buf_index], 
	              " %08lX: %08X%11c%-8s $%08X\n", addr, mc->mc, 
	              ' ', ".word", mc->mc);
	   else
	      sprintf(cli_display_buf_ptr[buf_index], 
	              " %08lX: %08X%11c%s\n", addr, mc->mc, ' ', 
	              inst_buf);
	}

	print_buf(cli_display_buf_ptr, buf_index);

        return(rc);
}

#endif /* RISC */

/*-----------------------------------------------------------------------
 * perform_execute() 
 *-----------------------------------------------------------------------*/
int perform_execute(
        uInt32          start_address,
        uInt32          end_address,
        uInt32          num,
        uInt32          time_delay,
	uInt32		verbosity)
{
	register uInt32	start_time, end_time;
	register uInt32	n;
	register int	rc;

	rc = 0;
	for (n = 1; n <= num && rc == 0; n++) {
	   if (rc = check_for_break())
	      break;
	   if (verbosity) {
	      sprintf(cli_print_buf, execute_progress_msg, start_address, n);
	      put_line(cli_print_buf);
	   }
#ifndef  ATM
	   asm_call(start_address, end_address);
#endif   /* ATM */

	   for (start_time = system_tick_time; 
	       (system_tick_time - start_time) < (1000*time_delay)&&rc == 0; ) {
	      if (rc = check_for_break())
	         break;
	   }
	}

        return(rc);
}
#endif /* DEBUGFUNC */

int perform_test(
        uInt32          write_address,
        uInt32          read_address,
        uInt32          data,
        uInt32          compare_data,
        char            *access,
        uInt32          num,
        uInt32          time_delay,
	uInt32		verbosity)
{
	uInt32		start_time, end_time, read_data;
	register uInt32	n;
	register int	rc, buf_index;
	register char	*fmt;

	switch (*access) {
	   case ACCESS_TYPE_BYTE:
	      fmt = test_fmt_byte; break;
	   case ACCESS_TYPE_WORD:
	      fmt = test_fmt_word; break;
	   case ACCESS_TYPE_LONG:
	      fmt = test_fmt_long; break;
	}

	buf_index = -1; rc = 0;
	for (n = 1; n <= num && rc == 0; n++) {
	   if (rc = check_for_break())
	      break;
	   switch(*access) {
	      case ACCESS_TYPE_BYTE:
	         if (rc = byte_write(write_address, data))
	            break;
	         if (rc = byte_read(read_address, &read_data))
	            break;
	         break;
	      case ACCESS_TYPE_WORD:
	         if (rc = word_write(write_address, data))
	            break;
	         if (rc = word_read(read_address, &read_data))
	            break;
	         break;
	      case ACCESS_TYPE_LONG:
	         if (rc = long_write(write_address, data))
	            break;
	         if (rc = long_read(read_address, &read_data))
	            break;
	         break;
	   }
	   if (rc == 0) {
	      sprintf(cli_print_buf, fmt, data, write_address, read_address,
	              read_data, n);
	      if (verbosity >= VERBOSITY_LEVEL2) {
	         put_line(cli_print_buf);
	      }
	      if (buf_index < (DISPLAY_BUF_SIZE-1)) {
	         if (read_data != compare_data)
	            strcpy(cli_display_buf_ptr[++buf_index], " FAIL: ");
	         else
	            strcpy(cli_display_buf_ptr[++buf_index], " PASS: ");
	         strcat(cli_display_buf_ptr[buf_index], cli_print_buf);
	      }
	   }
	   for (start_time = system_tick_time; 
	       (system_tick_time - start_time) < (1000*time_delay)&&rc == 0; ) {
	      if (rc = check_for_break())
	         break;
	   }
	}

	print_buf(cli_display_buf_ptr, ++buf_index);

        return(rc);
}
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */

char *get_access(
        char            *argp)
{
	register uInt32	i;

	i = (uInt32) argp;

	if (i >= 0 && i < 3)
	   return(Access_type[i]);

	for (i = 0; i < 3; i++) {
           if (!strcmp_v(argp, Access_type[i]))
	      return(Access_type[i]);
	}

        return((char *) 0);
}

char *get_mode(
        char            *argp)
{
	register uInt32	i;

	i = (uInt32) argp;

	if (i >= 0 && i < 5)
	   return(Mode_type[i]);

	for (i = 0; i < 5; i++) {
           if (!strcmp_v(argp, Mode_type[i]))
	      return(Mode_type[i]);
	}

        return((char *) 0);
}

#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
void fill_ascii_chars(
	char		*buf_ptr[],
	int		buf_index,
	int		buf_len)
{
	register int	index, i, j, k;
	int		data;

	for (index = 0; index < buf_index; index++) {

	   if (!strcmp(buf_ptr[index], "-"))
	      continue;
	   for (i = 0; buf_ptr[index][i] != '\0'; i++);
	   k = i;
	   while (i < buf_len)
	      buf_ptr[index][i++] = ' ';

	   for (j = 10; j < k; ) {
	      if (buf_ptr[index][j] != ' ') {
	         sscanf(&buf_ptr[index][j], "%2X", &data);
	         sprintf(&buf_ptr[index][i++], "%c", (isprint(data)) ? 
                                                      data : '.');
	         j += 2;
	      }
	      else
	         j++;
	   }
	   strcat(buf_ptr[index], "\n");
	}
}

#ifdef DEBUGFUNC
#ifndef RISC
int asm_0(
	int		inst_index,
	char		*operand_ptr,
	uInt16		m_code[])
{
	register int	len;

	if (operand_ptr == NULL)
	   return(ASM_MISSING_OPERAND);

	return(ASM_INSTRUCTION_NOT_IMPLEMENTED);
}

int asm_1_2_3(
	int		inst_index,
	char		*operand_ptr,
	uInt16		m_code[])
{
	register int	len;

	if (operand_ptr == NULL)
	   return(ASM_MISSING_OPERAND);

	return(ASM_INSTRUCTION_NOT_IMPLEMENTED);
}

int asm_4(
	enum OPCODE_0100_INSTRUCTION_INDEX	inst_index,
	char		*operand_ptr,
	uInt16		m_code[])
{
	register int	len;
	uInt32		data;

	len = 2;
	if (operand_ptr == NULL) {
	   switch (inst_index) {
	      case BGND:
	         m_code[0] = 0x4AFA; break;
	      case ILLEGAL:
	         m_code[0] = 0x4AFC; break;
	      case RESET:
	         m_code[0] = 0x4E70; break;
	      case NOP:
	         m_code[0] = 0x4E71; break;
	      case RTE:
	         m_code[0] = 0x4E73; break;
	      case RTS:
	         m_code[0] = 0x4E75; break;
	      case TRAPV:
	         m_code[0] = 0x4E76; break;
	      case RTR:
	         m_code[0] = 0x4E77; break;
	      default:
	         return(ASM_MISSING_OPERAND);
	   }
	}
	else {
	   switch (inst_index) {
	      case BKPT: case STOP: case RTD:
	         if (*operand_ptr == '#')
	            operand_ptr++;
	         if (inst_index == BKPT) {
	            m_code[0] = 0x4848;
                    if (!sscanf_x(operand_ptr, fmt_uInt32, &data))
	               return(ASM_INVALID_OPERAND);
	            if (data > 7)
	               return(ASM_OPERAND_OUT_OF_RANGE);
	            m_code[0] |= data;
	         }
	         else {
	            len = 4;
	            if (inst_index == STOP)
	               m_code[0] = 0x4E72;
	            if (inst_index == RTD)
	               m_code[0] = 0x4E74;
	            if (*operand_ptr == '$')
	               operand_ptr++;
                    if (!sscanf_x(operand_ptr, fmt_xInt32, &data))
	               return(ASM_INVALID_OPERAND);
	            m_code[1] = data;
	         }
	         break;
	      default:
	         return(ASM_INSTRUCTION_NOT_IMPLEMENTED);
	   }
	}

	return(len);
}

int asm_5(
	int		inst_index,
	char		*operand_ptr,
	uInt16		m_code[])
{
	register int	len;

	if (operand_ptr == NULL)
	   return(ASM_MISSING_OPERAND);

	return(ASM_INSTRUCTION_NOT_IMPLEMENTED);
}

int asm_6(
	int		inst_index,
	char		*operand_ptr,
	uInt16		m_code[])
{
	register int	len;

	if (operand_ptr == NULL)
	   return(ASM_MISSING_OPERAND);

	return(ASM_INSTRUCTION_NOT_IMPLEMENTED);
}

int asm_7(
	int		inst_index,
	char		*operand_ptr,
	uInt16		m_code[])
{
	register int	len;

	if (operand_ptr == NULL)
	   return(ASM_MISSING_OPERAND);

	return(ASM_INSTRUCTION_NOT_IMPLEMENTED);
}

int asm_8_c(
	int		inst_index,
	char		*operand_ptr,
	uInt16		m_code[])
{
	register int	len;

	if (operand_ptr == NULL)
	   return(ASM_MISSING_OPERAND);

	return(ASM_INSTRUCTION_NOT_IMPLEMENTED);
}

int asm_9_b_d(
	int		inst_index,
	char		*operand_ptr,
	uInt16		m_code[])
{
	register int	len;

	if (operand_ptr == NULL)
	   return(ASM_MISSING_OPERAND);

	return(ASM_INSTRUCTION_NOT_IMPLEMENTED);
}

int asm_a(
	int		inst_index,
	char		*operand_ptr,
	uInt16		m_code[])
{
	return(ASM_INSTRUCTION_NOT_IMPLEMENTED);
}

int asm_e(
	int		inst_index,
	char		*operand_ptr,
	uInt16		m_code[])
{
	register int	len;

	if (operand_ptr == NULL)
	   return(ASM_MISSING_OPERAND);

	return(ASM_INSTRUCTION_NOT_IMPLEMENTED);
}

int asm_f(
	int		inst_index,
	char		*operand_ptr,
	uInt16		m_code[])
{
	return(ASM_INSTRUCTION_NOT_IMPLEMENTED);
}

int dasm_0(
	uInt32		addr,
	char		buf[])
{
	register uInt16	opword;
	register uInt16	operand;
	register uInt32	data;
	register int	opcode, ex_len, len;
	register int	ea_mode, ea_reg, ex_opcode;
	register int	size, p_s, index, a_size;
	register int	da_bit, dr_bit, reg;
	register int	callm_f;
	char		tmp_buf[80], ir_char;

	opword = *((uInt16 *) addr);
	operand = *((uInt16 *) (addr+2));
	data    = (uInt32) operand;
	opcode  = (opword >> 12);
	ex_opcode= (opword & 0x0E00) >> 9;
	p_s     = (opword & 0x0100) >> 8;
	size    = (opword & 0x00C0) >> 6;
	ea_mode = (opword & 0x0038) >> 3;
	ea_reg  = (opword & 0x0007);

	da_bit = (operand & 0x8000) >> 15;
	reg    = (operand & 0x7000) >> 12;
	dr_bit = (operand & 0x0800) >> 11;

	callm_f = FALSE;

	if (p_s == 0x0 && ex_opcode != 0x4) { 
	   /* ORI/ANDI/SUBI/ADDI/EORI/CMPI/CMP2/CHK2/MOVES */

	   if ((ex_opcode == 0x7 && ea_mode == 0x0) || ea_mode == 0x1)
	      return(-1);

	   len = 4;
	   if (ex_opcode != 0x7) { /* != MOVES */
	      switch (size) {
	         case 0x0:
	            if (data > 0xFF)
	               return(-1);
	            break;
	         case 0x1:
	            break;
	         case 0x2:
	            data = ((((uInt32) operand) << 16) |
	                     ((uInt32) *((uInt16 *) (addr+4))));
	            len += 2;
	            break;
	         case 0x3: /* CMP2/CHK2/CALLM */
	            if (ea_mode == 0x0 || ea_mode == 0x3 || ea_mode == 0x4 ||
	               (ea_mode == 0x7 && ea_reg == 4))
	               return(-1);
	            if (ex_opcode == 0x3) { /* CALLM */
	               if (operand & 0xFF00)
	                  return(-1);
	               index = 41;
	               callm_f = TRUE;
	            }
	            else {
	               if (operand & 0x7FF)
	                  return(-1);
	               if (dr_bit) /* CHK2 */
	                  index = 24 + ex_opcode;
	               else /* CMP2 */
	                  index = 21 + ex_opcode;
	            }
	            break;
	      }
	   }
	   else {
	      if (size == 0x3)
	         return(-1);
	   }
	   if (size != 0x3) { /* != CMP2/CHK2/CALLM */
	      switch (ex_opcode) {
	         case 0x0: /* ORI */
	            if (ea_mode == 0x7 && ea_reg != 0 && ea_reg != 1 &&
	                ea_reg != 4)
	               return(-1);
	            index = size;
	            if (ea_mode == 0x7 && ea_reg == 4)
	               index = 3;
   	            break;
	         case 0x1: /* ANDI */
	            if (ea_mode == 0x7 && ea_reg != 0 && ea_reg != 1 &&
	                ea_reg != 4)
	               return(-1);
	            index = 4 + size;
	            if (ea_mode == 0x7 && ea_reg == 4)
	               index = 7;
	            break;
	         case 0x2: /* SUBI */
	            if (ea_mode == 0x7 && ea_reg != 0 && ea_reg != 1)
	               return(-1);
	            index = 12 + size;
	            break;
	         case 0x3: /* ADDI */
	            if (ea_mode == 0x7 && ea_reg != 0 && ea_reg != 1)
	               return(-1);
	            index = 15 + size;
	            break;
	         case 0x5: /* EORI */
	            if (ea_mode == 0x7 && ea_reg != 0 && ea_reg != 1 &&
	                ea_reg != 4)
	               return(-1);
	            index = 8 + size;
	            if (ea_mode == 0x7 && ea_reg == 4)
	               index = 11;
	            break;
	         case 0x6: /* CMPI */
	            if (ea_mode == 0x7 && ea_reg != 0 && ea_reg != 1)
	               return(-1);
	            index = 18 + size;
	            break;
	         case 0x7: /* MOVES */
	            if (ea_mode == 0x7 && ea_reg != 0 && ea_reg != 1)
	               return(-1);
	            if (operand & 0x7FF)
	               return(-1);
	            index = 33 + size;
	            break;
	      }
	   }
	   if ((ex_opcode == 0x0 || ex_opcode == 0x1 || ex_opcode == 0x5) &&
	       (ea_mode == 0x7 && ea_reg == 4)) {
	      if (size == 0x0)
	         strcpy(tmp_buf, "ccr");
	      else
	         if (size == 0x1)
	            strcpy(tmp_buf, "sr");
	         else
	            return(-1);
	   }
	   else {
	      if ((ex_len = get_ea(addr+len, tmp_buf, ea_mode, ea_reg, size))
	                                                                  == -1)
	         return(-1);
	      len += ex_len;
	   }
	   if ((ex_opcode != 0x7 && size != 0x3) || callm_f)
	      sprintf(buf, "%-8s #$%lX, %s", instruction[opcode][index], data,
	              tmp_buf);
	   else {
	      if (ex_opcode == 0x7 && dr_bit) { /* MOVES */
	         if (da_bit) {
	            if (reg == 7)
	               strcpy(buf, "sp, ");
	            else
	               sprintf(buf, "a%u, ", reg);
	         }
	         else
	            sprintf(buf, "d%u, ", reg);
	         strcat(buf, tmp_buf);
	         strcpy(tmp_buf, buf);
	      }
	      sprintf(buf, "%-8s %s", instruction[opcode][index], tmp_buf);
	      if (ex_opcode != 0x7 || (ex_opcode == 0x7 && !dr_bit)) {
	         if (da_bit) {
	            if (reg == 7)
	               strcpy(tmp_buf, ", sp");
	            else
	               sprintf(tmp_buf, ", a%u", reg);
	         }
	         else
	            sprintf(tmp_buf, ", d%u", reg);
	         strcat(buf, tmp_buf);
	      }
	   }
	}
	else { /* BTST/BCHG/BCLR/BSET */
	   if ((p_s == 0 && ex_opcode == 0x4) || p_s == 1) {
	      /* BTST/BCHG/BCLR/BSET */
	      if (ea_mode == 0x1)
	         return(-1);
	      if (size != 0x0) { /* BCHG/BCLR/BSET */
	         if (ea_mode == 0x7 && ea_reg != 0 && ea_reg != 1)
	            return(-1);
	      }
	      else {
	         if (p_s == 0 && ea_mode == 0x7 && ea_reg == 4)
	            return(-1);
	      }

	      len = 2;
	      index = 27 + size;
	      if (p_s == 1) {
	         ir_char = 'd';
	         data = ex_opcode;
	      }
	      else {
	         len += 2;
	         ir_char = '#';
	         if (data > 0xFF)
	            return(-1);
	         if (ea_mode == 0x0)
	            data %= 32;
	         else
	            data %= 8;
	      }

	      if ((ex_len = get_ea(addr+len, tmp_buf, ea_mode, ea_reg, size))
	                                                                  == -1)
	         return(-1);
	      len += ex_len;

	      sprintf(buf, "%-8s %c%u, %s", instruction[opcode][index], ir_char,
	              data, tmp_buf);
	   }
	   else {
	      if (p_s == 1 && ea_mode == 0x1) { /* MOVEP */
	         index = 31 + (size & 0x1);
	         if (size == 0x0 || size == 0x1) {
	            if (ea_reg == 7)
	               sprintf(tmp_buf, "($%X, sp), d%u", data, ex_opcode);
	            else
	               sprintf(tmp_buf, "($%X, a%u), d%u", data, ea_reg, 
	                       ex_opcode);
	         }
	         else {
	            if (ea_reg == 7)
	               sprintf(tmp_buf, "d%u, ($%X, sp)", ex_opcode, data);
	            else
	               sprintf(tmp_buf, "d%u, ($%X, a%u)", ex_opcode, data,
	                       ea_reg);
	         }
	         sprintf(buf, "%-8s %s", instruction[opcode][index], tmp_buf);
	      }
	      else
	         return(-1);
	   }
	}

	return(len);
}

int dasm_1_2_3( /* MOVE.B/MOVE.W/MOVE.L/MOVEA.W/MOVEA.L */
	uInt32		addr,
	char		buf[])
{
	register uInt16	opword;
	register int	opcode;
	register int	src_mode, src_reg;
	register int	dest_mode, dest_reg;
	register int	len, ex_len, size;
	char		tmp_buf[80], *ptr;

	opword = *((uInt16 *) addr);
	opcode = (opword >> 12);
	dest_reg  = (opword & 0x0E00) >> 9;
	dest_mode = (opword & 0x01C0) >> 6;
	src_mode  = (opword & 0x0038) >> 3;
	src_reg   = (opword & 0x0007);

	/* Byte Move not allowed between Address registers */
	if (opcode == 1 && (src_mode == 1 || dest_mode == 1))
	   return(-1);

	len = 2;
	if (opcode == 1)
	   size = 0x0;
	else
	   if (opcode == 2)
	      size = 0x2;
	   else
	      size = 0x1;

	if ((ex_len = get_ea(addr+len, tmp_buf, src_mode, src_reg, size)) == -1)
	   return(-1);
	len += ex_len;

	if (dest_mode == 0x1)
	   ptr = instruction[opcode][1];
	else
	   ptr = instruction[opcode][0];

	sprintf(buf, "%-8s %s, ", ptr, tmp_buf);

	if ((ex_len = get_ea(addr+len, tmp_buf, dest_mode, dest_reg, size)) == 
	                                                                     -1)
	   return(-1);
	len += ex_len;

	strcat(buf, tmp_buf);

	return(len);
}

int dasm_4(
	uInt32		addr,
	char		buf[])
{
	register uInt16	opword;
	register uInt16	operand;
	register uInt32	data, disp;
	register int	opcode, len, ex_len;
	register int	ea_mode, ea_reg, ex_opcode, reg, regl, regh;
	register int	size, p_s, index;
	register int	bgnd_f, illegal_f, link_f, swap_f, bkpt_f, ext_f;
	register int	movem_f, mul_f, mul_l_f, misc_f;
	char		tmp_buf[80], ir_char;

	opword  = *((uInt16 *) addr);
	operand = *((uInt16 *) (addr+2));
	data    = (uInt32) operand;
	opcode  = (opword >> 12);
	ex_opcode = reg = (opword & 0x0E00) >> 9;
	p_s     = (opword & 0x0100) >> 8;
	size    = (opword & 0x00C0) >> 6;
	ea_mode = (opword & 0x0038) >> 3;
	ea_reg  = (opword & 0x0007);

	bgnd_f = illegal_f = link_f = swap_f = bkpt_f = ext_f = movem_f = FALSE;
	mul_f = mul_l_f = misc_f = FALSE;

	if (p_s == 0x0) { 
	   /* NEGX/CLR/NEG/NOT/NBCD/PEA/EXT/EXTB/TST/TAS/BGND/ILLEGAL/
	      MULU.L/MULS.L */

	   len = 2;
	   tmp_buf[0] = '\0';
	   switch (ex_opcode) {
	      case 0x0: /* NEGX/MOVE from SR */
	         if (ea_mode == 0x1 ||
	            (ea_mode == 0x7 && ea_reg != 0 && ea_reg != 1))
	            return(-1);
	         index = size;
	         if (size == 0x3)
	            strcpy(tmp_buf, "sr, ");
   	         break;
	      case 0x1: /* CLR/MOVE from CCR */
	         if (ea_mode == 0x1 ||
	            (ea_mode == 0x7 && ea_reg != 0 && ea_reg != 1))
	            return(-1);
	         index = 4 + size;
	         if (size == 0x3)
	            strcpy(tmp_buf, "ccr, ");
	         break;
	      case 0x2: /* NEG/MOVE to CCR */
	         if (ea_mode == 0x1 ||
	            (ea_mode == 0x7 && ea_reg != 0 && ea_reg != 1 && size != 3))
	            return(-1);
	         index = 8 + size;
	         break;
	      case 0x3: /* NOT/MOVE to SR */
	         if (ea_mode == 0x1 ||
	            (ea_mode == 0x7 && ea_reg != 0 && ea_reg != 1 && size != 3))
	            return(-1);
	         index = 12 + size;
	         break;
	      case 0x4: /* NBCD/LINK.L/SWAP/BKPT/PEA/EXT/MOVEM */
	         if (size == 0x0) { /* NBCD/LINK.L */
	            if (ea_mode == 0x1) { /* LINK.L */
	               len = 6;
	               index = 18;
	               disp = (data << 16) | ((uInt32) *((uInt16 *) (addr+4)));
	               link_f = TRUE;
	            }
	            else { /* NBCD */
	               if (ea_mode == 0x7 && ea_reg != 0 && ea_reg != 1)
	                  return(-1);
	               index = 16;
	            }
	         }
	         else {
	            if (size == 0x1) { /* SWAP/BKPT/PEA */
	               if (ea_mode == 0x0) { /* SWAP */
	                  index = 19;
	                  sprintf(tmp_buf, "d%u", ea_reg);
   	                  swap_f = TRUE;
	               }
	               else {
	                  if (ea_mode == 0x1) { /* BKPT */
	                     index = 20;
	                     sprintf(tmp_buf, "#%u", ea_reg);
	                     bkpt_f = TRUE;
	                  }
	                  else { /* PEA */
	                     if (ea_mode == 0x3 || ea_mode == 0x4 ||
	                        (ea_mode == 0x7 && ea_reg == 4))
	                        return(-1);
	                     index = 21;
	                  }
	               }
	            }
	            else { /* EXT/MOVEM */
	               if (ea_mode == 0x0) { /* EXT */
	                  if (size == 0x2) /* EXT.W */
	                     index = 22;
	                  if (size == 0x3) /* EXT.L */
	                     index = 23;
	                  sprintf(tmp_buf, "d%u", ea_reg);
	                  ext_f = TRUE;
	               }
	               else { /* MOVEM */
	                  len = 4;
	                  if (ea_mode == 0x1 || ea_mode == 0x3 ||
	                     (ea_mode == 0x7 && ea_reg != 0 && ea_reg != 1))
	                     return(-1);
	                  if (size == 0x2) /* MOVEM.W */
	                     index = 25;
	                  else             /* MOVEM.L */
	                     index = 26;
	                  if (get_reg_list(tmp_buf, data, 1))
	                     return(-1);
	                  strcat(tmp_buf, ", ");
	               }
	            }
	         }
	         break;
	      case 0x5: /* TST/TAS */
	         index = 27 + size;
	         if (size == 0x3) { /* TAS/BGND/ILLEGAL */
	            if (ea_mode == 0x1)
	               return(-1);
	            if (ea_mode == 0x7) {
	               if (ea_reg == 2) { /* BGND */
	                  index = 31; bgnd_f = TRUE;
	               }
	               else
	                  if (ea_reg == 4) { /* ILLEGAL */
	                     index = 32; illegal_f = TRUE;
	                  }
	                  else
	                     if (ea_reg != 0 && ea_reg != 1)
	                        return(-1);
	            }
	            else
	               return(-1);
	         }
	         else { /* TST */
	            if (size == 0x0 && ea_mode == 0x1)
	               return(-1);
	         }
	         break;
	      case 0x6: /* MOVEM/MULU.L/MULS.L/DIVU.L/DIVS.L */
	         len = 4;
	         if (size == 0x2 || size == 0x3) { /* MOVEM */
	            if (ea_mode == 0x1 || ea_mode == 0x4 ||
	               (ea_mode == 0x7 && ea_reg == 4))
	               return(-1);
	            if (size == 0x2) /* MOVEM.W */
	               index = 25;
	            else             /* MOVEM.L */
	               index = 26;
	            movem_f = TRUE;
	         }
	         else { /* MULU.L/MULS.L/DIVU.L/DIVS.L */
	            if (ea_mode == 0x1)
	               return(-1);
	            if (operand & 0x83F8)
	               return(-1);
	            regl = (operand & 7000) >> 12;
	            regh = operand & 0x7;
	            if (operand & 0x0400)
	               mul_l_f = TRUE;
	            if (size == 0x0) /* MULU.L/MULS.L */
	               if (!(operand & 0x0800))
	                  index = 33;
	               else
	                  index = 34;
	            else             /* DIVU.L/DIVS.L */
	               if (!(operand & 0x0800))
	                  index = 35;
	               else
	                  index = 36;
	            mul_f = TRUE;
	         }
	         break;
	      case 0x7:
	         switch (size) {
	            case 0x0:
	               return(-1);
	            case 0x1:
	               misc_f = TRUE;
	               switch (ea_mode) {
	                  case 0x0: case 0x1: /* TRAP */
	                     index = 40;
	                     sprintf(tmp_buf, "#%u", (operand & 0xF));
	                     break;
	                  case 0x2: /* LINK.W */
	                     len = 4;
	                     index = 17;
	                     disp = data;
	                     link_f = TRUE;
	                     break;
	                  case 0x3: /* UNLK */
	                     index = 41;
	                     if (ea_reg == 7)
	                        strcpy(tmp_buf, "sp");
	                     else
	                        sprintf(tmp_buf, "a%u", ea_reg);
	                     break;
	                  case 0x4: case 0x5: /* MOVE USP */
	                     index = 42;
	                     if (ea_reg == 7)
	                        if (ea_mode == 0x4)
	                           strcpy(tmp_buf, "sp, usp");
	                        else
	                           strcpy(tmp_buf, "usp, sp");
	                     else
	                        if (ea_mode == 0x4)
	                           sprintf(tmp_buf, "a%u, usp", ea_reg);
	                        else
	                           sprintf(tmp_buf, "usp, a%u", ea_reg);
	                     break;
	                  case 0x6: /* RESET/NOP/STOP/RTE/RTD/RTS/TRAPV/RTR */
	                     if (ea_reg == 2 || ea_reg == 4) { /* STOP/RTD */
	                        len = 4;
	                        sprintf(tmp_buf, "#$%X", operand);
	                     }
	                     index = 43 + ea_reg;
	                     break;
	                  case 0x7: /* MOVEC */
	                     len = 4;
	                     if (ea_reg != 2 && ea_reg != 3)
	                        return(-1);
	                     regl = (operand & 7000) >> 12;
	                     if (ea_reg == 3) {
	                        if (operand & 0x8000)
	                           if (regl == 7)
	                              strcpy(tmp_buf, "sp, ");
	                           else
	                              sprintf(tmp_buf, "a%u, ", regl);
	                        else
	                           sprintf(tmp_buf, "d%u, ", regl);
	                     }
	                     switch (operand & 0x0FFF) {
	                        case 0x000: strcpy(buf, "sfc"); break;
	                        case 0x001: strcpy(buf, "dfc"); break;
	                        case 0x002: strcpy(buf, "cacr"); break;
	                        case 0x003: strcpy(buf, "tc"); break;
	                        case 0x004: strcpy(buf, "itt0"); break;
	                        case 0x005: strcpy(buf, "itt1"); break;
	                        case 0x006: strcpy(buf, "dtt0"); break;
	                        case 0x007: strcpy(buf, "dtt1"); break;
	                        case 0x800: strcpy(buf, "usp"); break;
	                        case 0x801: strcpy(buf, "vbr"); break;
	                        case 0x802: strcpy(buf, "caar"); break;
	                        case 0x803: strcpy(buf, "msp"); break;
	                        case 0x804: strcpy(buf, "isp"); break;
	                        case 0x805: strcpy(buf, "mmusr"); break;
	                        case 0x806: strcpy(buf, "urp"); break;
	                        case 0x807: strcpy(buf, "srp"); break;
	                        default:
	                           return(-1);
	                     }
	                     strcat(tmp_buf, buf);
	                     if (ea_reg == 2) {
	                        if (operand & 0x8000)
	                           if (regl == 7)
	                              strcpy(buf, ", sp");
	                           else
	                              sprintf(buf, ", a%u", regl);
	                        else
	                           sprintf(buf, ", d%u", regl);
	                        strcat(tmp_buf, buf);
	                     }
	                     index = 51;
	                     break;
	               }
	               break;
	            case 0x2: case 0x3: /* JSR/JMP */
	               if (ea_mode == 0x0 || ea_mode == 0x1 ||
	                   ea_mode == 0x3 || ea_mode == 0x4 ||
	                  (ea_mode == 0x7 && ea_reg == 4))
	                  return(-1);
	               index = 50 + size;
	               break;
	         }
	         break;
	   }

	   if (link_f) {
	      if (ea_reg == 7)
	         sprintf(tmp_buf, "sp, #$%lX", disp);
	      else
	         sprintf(tmp_buf, "a%u, #$%lX", ea_reg, disp);
	   }

	   sprintf(buf, "%-8s %s", instruction[opcode][index], tmp_buf);

	   if (bgnd_f || illegal_f || link_f || swap_f || bkpt_f || ext_f ||
	       misc_f)
	      return(len);

	   if ((ex_len = get_ea(addr+len, tmp_buf, ea_mode, ea_reg, size))==-1)
	      return(-1);
	   len += ex_len;

	   strcat(buf, tmp_buf);

	   tmp_buf[0] = '\0';
	   if (size == 0x3) {
	      if (ex_opcode == 0x2)
	         strcpy(tmp_buf, ", ccr");
	      if (ex_opcode == 0x3)
	         strcpy(tmp_buf, ", sr");
	   }
	   if (movem_f) {
	      strcat(buf, ", ");
	      if (get_reg_list(tmp_buf, data, 0))
	         return(-1);
	   }
	   if (mul_f) {
	      if (mul_l_f)
	         sprintf(tmp_buf, ", d%u:d%u", regh, regl);
	      else
	         sprintf(tmp_buf, ", d%u", regl);
	   }
	   strcat(buf, tmp_buf);
	}
	else {
	   if (ea_mode == 0x1)
	      return(-1);
	   len = 2;
	   switch (size) {
	      case 0x0: /* CHK.L */
	         size = 0x2;
	         index = 38;
	         break;
	      case 0x1: 
	         return(-1);
	      case 0x2: /* CHK.W */
	         size = 0x1;
	         index = 37;
	         break;
	      case 0x3: /* EXTB/LEA */
	         if (ea_mode == 0x0) { /* EXTB */
	            if (size == 0x3) {
	               index = 24;
	               ext_f = TRUE;
	            }
	            else
	               return(-1);
	         }
	         else { /* LEA */
	            if (ea_mode == 0x3 || ea_mode == 0x4 ||
	               (ea_mode == 0x7 && ea_reg == 4))
	               return(-1);
	            index = 39;
	         }
	         break;
	   }

	   if (ext_f) {
	       sprintf(tmp_buf, "d%u", ea_reg);
	   }
	   else {
	      if ((ex_len = get_ea(addr+len, tmp_buf, ea_mode, ea_reg, size)) 
	                                                                  == -1)
	         return(-1);
	      len += ex_len;
	   }

	   sprintf(buf, "%-8s %s, ", instruction[opcode][index], tmp_buf);

	   if (ext_f)
	      return(len);

	   if (size == 0x3) { /* LEA */
	      if (reg == 7)
	         strcpy(tmp_buf, "sp");
	      else
	         sprintf(tmp_buf, "a%u", reg);
	   }
	   else { /* CHK */
	      sprintf(tmp_buf, "d%u", reg);
	   }
	   strcat(buf, tmp_buf);
	}

	return(len);
}

int dasm_5( /* ADDQ/SUBQ/DBcc/Scc/TRAPcc */
	uInt32		addr,
	char		buf[])
{
	register Int32	disp;
	register uInt32	label;
	register uInt16	opword;
	register int	opcode, len, ex_len, size;
	register int	ea_mode, ea_reg;
	register int	op_mode, data;
	register int	cond;
	char		tmp_buf[80];

	opword  = *((uInt16 *) addr);
	opcode  = (opword >> 12);
	data    = (opword & 0x0E00) >> 9;
	if (data == 0)
	   data = 8;
	cond    = (opword & 0x0F00) >> 8;
	op_mode = (opword & 0x01C0) >> 6;
	ea_mode = (opword & 0x0038) >> 3;
	ea_reg  = (opword & 0x0007);

	/* Byte operation not allowed when destination is Address reg */
	if (op_mode == 0x0 && ea_mode == 0x1)
	   return(-1);

	switch (op_mode) {
	   case 0x0: case 0x1: case 0x2: case 0x4: case 0x5: case 0x6:
	      /* ADDQ/SUBQ */

	      if (ea_mode == 0x7 && ea_reg != 0 && ea_reg != 1)
	         return(-1);

	      len = 2;
	      size = op_mode%0x4;

	      if ((ex_len = get_ea(addr+len, tmp_buf, ea_mode, ea_reg, size))
                                                                          == -1)
	         return(-1);
	      len += ex_len;

	      if (op_mode == 0x4 || op_mode == 0x5 || op_mode == 0x6)
	         op_mode--;

	      sprintf(buf, "%-8s #%u, %s", instruction[opcode][op_mode], data,
	              tmp_buf);
	      break;
	   default:
	      if (ea_mode == 1) { /* DBcc */

	         len = 4;

	         disp = (Int32) *((uInt16 *) (addr+2));
	         if (disp >= 0x8000)
	            disp -= 0x10000;
	         label = addr + 2 + disp;

	         sprintf(buf, "%-8s d%u, $%lX", instruction[opcode][cond+6],
	                 ea_reg, label);
	      }
	      else {
	         if (ea_mode != 0x7 ||
	            (ea_mode == 0x7 && (ea_reg == 0 || ea_reg == 1))) {/* Scc */

	            len = 2;
	            size = 0;

	            if ((ex_len = get_ea(addr+len, tmp_buf, ea_mode, ea_reg, 
	                                                           size)) == -1)
	               return(-1);
	            len += ex_len;

	            sprintf(buf, "%-8s %s", instruction[opcode][cond+22], 
	                    tmp_buf);
	         }
	         else { /* TRAPcc */

	            strcpy(tmp_buf, instruction[opcode][cond+38]);
	            if (ea_reg == 0x2)
	               strcat(tmp_buf, ".w");
	            if (ea_reg == 0x3)
	               strcat(tmp_buf, ".l");
	            sprintf(buf, "%-8s ", tmp_buf);

	            if (ea_reg == 0x2) {
	               len = 4;
	               disp = (Int32) *((uInt16 *) (addr+2));
	               sprintf(tmp_buf, "#$%lX", disp);
	            }
	            else {
	               if (ea_reg == 0x3) {
	                  len = 6;
	                  disp = (Int32) ((((uInt32) *((uInt16 *) (addr+2))) << 16) |
	                             ((uInt32) *((uInt16 *) (addr+4))));
	                  sprintf(tmp_buf, "#$%lX", disp);
	               }
	               else {
	                  if (ea_reg == 0x4) {
	                     len = 2;
	                     tmp_buf[0] = '\0';
	                  }
	                  else
	                     return(-1);
	               }
	            }
	            strcat(buf, tmp_buf);
	         }
	      }
	      break;
	}

	return(len);
}

int dasm_6( /* Bcc/BRA/BSR */
	uInt32		addr,
	char		buf[])
{
	register Int32	disp;
	register uInt32	label;
	register uInt16	opword;
	register int	opcode, cond, len;

	opword = *((uInt16 *) addr);
	opcode = (opword >> 12);
	cond   = (opword & 0x0F00) >> 8;
	disp   = (opword & 0x00FF);
	if (disp == 0x00) {
	   disp = (Int32) *((uInt16 *) (addr+2));
	   if (disp >= 0x8000)
	      disp -= 0x10000;
	   len = 4;
	}
	else {
	   if (disp == 0xFF) {
	      disp = (Int32) ((((uInt32) *((uInt16 *) (addr+2))) << 16) |
	                       ((uInt32) *((uInt16 *) (addr+4))));
	      len = 6;
	   }
	   else {
	      if (disp >= 0x80)
	         disp -= 0x100;
	      len = 2;
	   }
	}
	label = addr + 2 + disp;

	sprintf(buf, "%-8s $%lX", instruction[opcode][cond], label);

	return(len);
}

int dasm_7( /* MOVEQ */
	uInt32		addr,
	char		buf[])
{
	register Int32	data;
	register uInt16	opword;
	register int	opcode;
	register int	reg;

	opword = *((uInt16 *) addr);
	opcode = opword >> 12;
	reg = (opword & 0x0E00) >> 9;

	if (!(opword & 0x0100)) {
	   data = (Int32) (opword & 0xFF);
	   if (data >= 0x80)
	      data -= 0x100;
	   sprintf(buf, "%-8s #%ld, d%u", instruction[opcode][0], data, reg);
	}
	else
	   return(-1);

	return(2);
}

int dasm_8_c( /* DIVS.W/DIVU.W/OR/PACK/SBCD/UNPK/
                 ABCD/AND/EXG/MULS.W/MULU.W */
	uInt32		addr,
	char		buf[])
{
	register uInt16	adjust;
	register uInt16	opword;
	register int	opcode;
	register int	ea_mode, ea_reg;
	register int	op_mode, reg, len, ex_len, size;
	char		tmp_buf[80];

	opword  = *((uInt16 *) addr);
	opcode  = (opword >> 12);
	reg     = (opword & 0x0E00) >> 9;
	op_mode = (opword & 0x01C0) >> 6;
	ea_mode = (opword & 0x0038) >> 3;
	ea_reg  = (opword & 0x0007);

	switch (op_mode) {
	   case 0x0: case 0x1: case 0x2: case 0x3: case 0x7:
	      /* AND/OR/DIVU.W/DIVS.W/MULU.W/MULS.W */
	      if (ea_mode == 0x1)
	         return(-1);

	      len = 2;
	      if (op_mode <= 0x2)
	         size = op_mode;
	      else
	         if (op_mode == 0x3)
	            size = 0x1;
	         else
	            size = 0x2;

	      if ((ex_len = get_ea(addr+len, tmp_buf, ea_mode, ea_reg, size))
                                                                          == -1)
	         return(-1);
	      len += ex_len;

	      sprintf(buf, "%-8s %s, d%u", instruction[opcode][op_mode], 
	              tmp_buf, reg);

	      break;
	   default:
	      if (ea_mode != 0x0 && ea_mode != 0x1) { /* AND/OR */
	         /* AND/OR */
	         if (ea_mode == 0x7 && ea_reg != 0 && ea_reg != 1)
	            return(-1);

	         len = 2;
	         size = op_mode - 0x4;

	         if ((ex_len = get_ea(addr+len, tmp_buf, ea_mode, ea_reg, size))
                                                                          == -1)
	            return(-1);
	         len += ex_len;

	         sprintf(buf, "%-8s d%u, %s", instruction[opcode][op_mode], reg,
	                 tmp_buf);
	      }
	      else {
	         if (op_mode != 0x4) { /* PACK/UNPK/EXG */
	            if (opcode == 0xC) { /* EXG */

	               len = 2;

	               sprintf(buf, "%-8s ", instruction[opcode][9]);

	               if (op_mode == 0x5) {
	                  if (ea_mode == 0x0)
	                     sprintf(tmp_buf, "d%u, d%u", ea_reg, reg);
	                  else
	                     if (ea_reg == 7)
	                        if (reg == 7)
	                           strcpy(tmp_buf, "sp, sp");
	                        else 
	                           sprintf(tmp_buf, "sp, a%u", reg);
	                     else
	                        if (reg == 7)
	                           sprintf(tmp_buf, "a%u, sp", ea_reg);
	                        else
	                           sprintf(tmp_buf, "a%u, a%u", ea_reg, reg);
	               }
	               else {
	                  if (ea_mode == 0x1)
	                     if (ea_reg == 7)
	                        sprintf(tmp_buf, "sp, d%u", reg);
	                     else
	                        sprintf(tmp_buf, "a%u, d%u", ea_reg, reg);
	                  else
	                     return(-1);
	               }
	            }
	            else { /* PACK/UNPK */

	               len = 4;

	               adjust = *((uInt16 *) (addr+2));
	               if (op_mode == 0x5) /* PACK */
	                  sprintf(buf, "%-8s ", instruction[opcode][9]);
	               else /* UNPK */
	                  sprintf(buf, "%-8s ", instruction[opcode][10]);

	               if (ea_mode == 0x0)
	                  sprintf(tmp_buf, "d%u, d%u, #$%X", ea_reg, reg,
	                          adjust);
	               else {
	                  if (ea_reg == 7)
	                     if (reg == 7)
	                        sprintf(tmp_buf, "-(sp), -(sp), #$%X", 
	                                adjust);
	                     else 
	                        sprintf(tmp_buf, "-(sp), -(a%u), #$%X", reg,
	                                adjust);
	                  else
	                     if (reg == 7)
	                        sprintf(tmp_buf, "-(a%u), -(sp), #$%X", 
	                                ea_reg, adjust);
	                     else
	                        sprintf(tmp_buf, "-(a%u), -(a%u), #$%X", 
	                                ea_reg, reg, adjust);
	               }
	            }
	         }
                 else { /* ABCD/SBCD */

	            len = 2;

	            sprintf(buf, "%-8s ", instruction[opcode][8]);

	            if (ea_mode == 0x0)
	               sprintf(tmp_buf, "d%u, d%u", ea_reg, reg);
	            else {
	               if (ea_reg == 7)
	                  if (reg == 7)
	                     strcpy(tmp_buf, "-(sp), -(sp)");
	                  else 
	                     sprintf(tmp_buf, "-(sp), -(a%u)", reg);
	               else
	                  if (reg == 7)
	                     sprintf(tmp_buf, "-(a%u), -(sp)", ea_reg);
	                  else
	                     sprintf(tmp_buf, "-(a%u), -(a%u)", ea_reg, reg);
	            }
	         }
	         strcat(buf, tmp_buf);
	      }
	      break;
	}

	return(len);
}

int dasm_9_b_d( /* SUB/SUBA/SUBX/CMP/CMPA/CMPM/EOR/ADD/ADDA/ADDX */
	uInt32		addr,
	char		buf[])
{
	register uInt16	opword;
	register int	opcode;
	register int	ea_mode, ea_reg;
	register int	op_mode, reg;
	register int	len, ex_len, size;
	char		tmp_buf[80];

	opword  = *((uInt16 *) addr);
	opcode  = (opword >> 12);
	reg     = (opword & 0x0E00) >> 9;
	op_mode = (opword & 0x01C0) >> 6;
	ea_mode = (opword & 0x0038) >> 3;
	ea_reg  = (opword & 0x0007);

	if (op_mode == 0x0 && ea_mode == 0x1)
	   return(-1);

	switch (op_mode) {
	   case 0x0: case 0x1: case 0x2: case 0x3: case 0x7:
	      /* ADD/ADDA/SUB/SUBA/CMP/CMPA */
	      len = 2;
	      if (op_mode <= 0x2)
	         size = op_mode;
	      else
	         if (op_mode == 0x3)
	            size = 0x1;
	         else
	            size = 0x2;

	      if ((ex_len = get_ea(addr+len, tmp_buf, ea_mode, ea_reg, size))
                                                                          == -1)
	         return(-1);
	      len += ex_len;

	      sprintf(buf, "%-8s %s, ", instruction[opcode][op_mode], tmp_buf);

	      if (op_mode == 0x3 || op_mode == 0x7) {
	         if (reg == 7)
	            strcpy(tmp_buf, "sp");
	         else
	            sprintf(tmp_buf, "a%u", reg);
	      }
	      else
	         sprintf(tmp_buf, "d%u", reg);
	      strcat(buf, tmp_buf);
	      break;
	   default:
	      if ((opcode != 0xB && ea_mode != 0x0 && ea_mode != 0x1) ||
	          (opcode == 0xB && ea_mode != 0x1)) { /* ADD/SUB/EOR */

	         if (ea_mode == 0x7 && ea_reg != 0 && ea_reg != 1)
	            return(-1);

	         len = 2;
	         size = op_mode - 0x4;

	         if ((ex_len = get_ea(addr+len, tmp_buf, ea_mode, ea_reg, size))
                                                                          == -1)
	            return(-1);
	         len += ex_len;

	         sprintf(buf, "%-8s d%u, %s", instruction[opcode][op_mode], reg,
	                 tmp_buf);
	      }
	      else {
	         len = 2;

	         sprintf(buf, "%-8s ", instruction[opcode][op_mode+4]);

	         if (opcode != 0xB) { /* ADDX/SUBX */
	            if (ea_mode == 0x0) {
	               sprintf(tmp_buf, "d%u, d%u", ea_reg, reg);
	            }
	            else {
	               if (ea_reg == 7)
	                  if (reg == 7)
	                     strcpy(tmp_buf, "-(sp), -(sp)");
	                  else 
	                     sprintf(tmp_buf, "-(sp), -(a%u)", reg);
	               else
	                  if (reg == 7)
	                     sprintf(tmp_buf, "-(a%u), -(sp)", ea_reg);
	                  else
	                     sprintf(tmp_buf, "-(a%u), -(a%u)", ea_reg, reg);
	            }
	         }
	         else { /* CMPM */
	            if (ea_reg == 7)
	               if (reg == 7)
	                  strcpy(tmp_buf, "(sp)+, (sp)+");
	               else 
	                  sprintf(tmp_buf, "(sp)+, (a%u)+", reg);
	            else
	               if (reg == 7)
	                  sprintf(tmp_buf, "(a%u)+, (sp)+", ea_reg);
	               else
	                  sprintf(tmp_buf, "(a%u)+, (a%u)+", ea_reg, reg);
	         }

	         strcat(buf, tmp_buf);
	      }
	      break;
	}

	return(len);
}

int dasm_a(
	uInt32		addr,
	char		buf[])
{
	return(-1);
}

int dasm_e( /* ASR/ASL/LSR/LSL/ROXR/ROXL/ROR/ROL/
	       BFTST/BFEXTU/BFCHG/BFEXTS/BFCLR/BFFFO/BFSET/BFINS */
	uInt32		addr,
	char		buf[])
{
	register uInt16	opword;
	register uInt16	operand;
	register int	opcode, len, ex_len;
	register int	ea_mode, ea_reg, ea_smode;
	register int	cnt_reg;
	register int	size, dr, ir, index;
	register int	offset, width, do_bit, dw_bit;
	char		tmp_buf[80], ir_char;

	opword  = *((uInt16 *) addr);
	operand = *((uInt16 *) (addr+2));
	opcode  = (opword >> 12);
	cnt_reg = (opword & 0x0E00) >> 9;
	dr      = (opword & 0x0100) >> 8;
	size    = (opword & 0x00C0) >> 6;
	ir      = (opword & 0x0020) >> 5;
	ea_mode = (opword & 0x0038) >> 3;
	ea_smode= (opword & 0x0018) >> 3;
	ea_reg  = (opword & 0x0007);

	if (size == 0x3) { /* Memory Format */

	   if (ea_mode == 0x7 && ea_reg != 0 && ea_reg != 1)
	      return(-1);

	   if (cnt_reg <= 0x3) { /* ASR/ASL/LSR/LSL/ROXR/ROXL/ROR/ROL */
	      if (ea_mode == 0x0 || ea_mode == 0x1)
	         return(-1);
	      index = 1 + 3*dr + 6*cnt_reg;
	      len = 2;
	   }
	   else { /* BFTST/BFEXTU/BFCHG/BFEXTS/BFCLR/BFFFO/BFSET/BFINS */
	      if (ea_mode == 0x1 || ea_mode == 0x3 || ea_mode == 0x4)
	         return(-1);
	      index = 24 + (cnt_reg - 0x4)*2 + dr;
	      len = 4;
	   }

	   if ((ex_len = get_ea(addr+len, tmp_buf, ea_mode, ea_reg, size))==-1)
	      return(-1);
	   len += ex_len;

	   sprintf(buf, "%-8s %s", instruction[opcode][index], tmp_buf);

	   if (cnt_reg >= 0x4) {
	      /* BFTST/BFEXTU/BFCHG/BFEXTS/BFCLR/BFFFO/BFSET/BFINS */
	      if (operand & 0xF000)
	         return(-1);
	      do_bit = (operand & 0x0800) >> 11;
	      offset = (operand & 0x07C0) >> 6;
	      dw_bit = (operand & 0x0020) >> 5;
	      width  = operand & 0x001F;
	      if (do_bit) {
	         if (offset > 7)
	            return(-1);
	      }
	      if (dw_bit) {
	         if (width > 7)
	            return(-1);
	      }
	      else {
	         if (width == 0)
	            width = 32;
	      }
	      if (do_bit)
	         if (dw_bit)
	            sprintf(tmp_buf, "{d%u:d%u}", offset, width);
	         else
	            sprintf(tmp_buf, "{d%u:%u}", offset, width);
	      else
	         if (dw_bit)
	            sprintf(tmp_buf, "{%u:d%u}", offset, width);
	         else
	            sprintf(tmp_buf, "{%u:%u}", offset, width);
	      strcat(buf, tmp_buf);
	   }
	}
	else { /* Register Format */
	   index = size + 3*dr;
	   len = 2;
	   switch (ea_smode) {
	      case 0x0:
	         break;
	      case 0x1:
	         index += 6;
	         break;
	      case 0x2:
	         index += 12;
	         break;
	      case 0x3:
	         index += 18;
	         break;
	   }
	   if (ir)
	      ir_char = 'd';
	   else {
	      if (cnt_reg == 0) 
	         cnt_reg = 8;
	      ir_char = '#';
	   }
	   sprintf(buf, "%-8s %c%u, d%u", instruction[opcode][index], ir_char,
	           cnt_reg, ea_reg);
	}

	return(len);
}

int dasm_f(
	uInt32		addr,
	char		buf[])
{
	return(-1);
}

void fill_opwords(
	char		buf[],
	uInt32		addr,
	int		len)
{
	register uInt16	opword;
	char		opw_buf[10];
	register char	flag;
	register int	i;

	for (i = 0; i < len && i < 3; i++) {
	   opword = *((uInt16 *) addr);
	   sprintf(opw_buf, "%04X ", opword);
	   strcat(buf, opw_buf);
	   addr += 2;
	}
	for ( ; i < 3; i++) {
	   strcat(buf, "     ");
	}
	if (i < len)
	   strcat(buf, "... ");
	else
	   strcat(buf, "    ");
}

int get_ea(
	uInt32		addr,
	char		buf[],
	int		mode,
	int		reg,
	int		size)
{
	register uInt16	word1, word2, word3;
	register uInt32	disp32;
	register int	da_bit, index_reg, len, bd_size, scale, w_l;
	register char	w_char;
	char		tmp_buf[80];

	word1 = (uInt32) *((uInt16 *) addr);
	word2 = (uInt32) *((uInt16 *) (addr+2));
	word3 = (uInt32) *((uInt16 *) (addr+4));
	disp32 = (((uInt32) word1) << 16) | ((uInt32) word2);
	da_bit = (word1 & 0x8000) >> 15;
	index_reg = (word1 & 0x7000) >> 12;

	len = 0;
	switch (mode) {
	   case 0x0:
	      sprintf(buf, "d%u", reg);
	      break;
	   case 0x1:
	      if (reg == 7)
	         strcpy(buf, "sp");
	      else
	         sprintf(buf, "a%u", reg);
	      break;
	   case 0x2:
	      if (reg == 7)
	         strcpy(buf, "(sp)");
	      else
	         sprintf(buf, "(a%u)", reg);
	      break;
	   case 0x3:
	      if (reg == 7)
	         strcpy(buf, "(sp)+");
	      else
	         sprintf(buf, "(a%u)+", reg);
	      break;
	   case 0x4:
	      if (reg == 7)
	         strcpy(buf, "-(sp)");
	      else
	         sprintf(buf, "-(a%u)", reg);
	      break;
	   case 0x5:
	      if (reg == 7)
	         sprintf(buf, "$%X(sp)", word1);
	      else
	         sprintf(buf, "$%X(a%u)", word1, reg);
	      len = 2;
	      break;
	   case 0x6:
	      len = 2;
	      w_l     = (word1 & 0x0800) >> 11;
	      if (w_l)
	         w_char = 'l';
	      else
	         w_char = 'w';
	      scale   = (word1 & 0x0600) >> 9;
	      if (scale == 0x3)
	         scale = 8;
	      else
	         scale *= 2;

	      if (word1 & 0x0100) {
	         bd_size = (word1 & 0x0030) >> 4;
	         if (bd_size == 0x2) {
	            disp32 = (uInt32) word2;
	            if (disp32)
	               len = 4;
	         }
	         else {
	            if (bd_size == 0x3) {
	               disp32 = (((uInt32) word2) << 16) | ((uInt32) word3);
	               if (disp32)
	                  len = 6;
	            }
	            else
	               disp32 = 0;
	         }
	      }
	      else {
	         disp32 = (uInt32) (word1 & 0xFF);
	      }
	      if (disp32)
	         sprintf(buf, "$%lX(", disp32);
	      else
	         strcpy(buf, "(");

	      if (da_bit) {
	         if (reg == 7) {
	            if (index_reg == 7)
	               sprintf(tmp_buf, "sp, sp.%c", w_char);
	            else
	               sprintf(tmp_buf, "sp, a%u.%c", index_reg, w_char);
	         }
	         else {
	            if (index_reg == 7)
	               sprintf(tmp_buf, "a%u, sp.%c", reg, w_char);
	            else
	               sprintf(tmp_buf, "a%u, a%u.%c", reg, index_reg, w_char);
	         }
	      }
	      else {
	         if (reg == 7)
	            sprintf(tmp_buf, "sp, d%u.%c", index_reg, w_char);
	         else
	            sprintf(tmp_buf, "a%u, d%u.%c", reg, index_reg, w_char);
	      }
	      strcat(buf, tmp_buf);

	      if (scale) {
	         sprintf(tmp_buf, "*%u", scale);
	         strcat(buf, tmp_buf);
	      }

	      strcat(buf, ")");

	      break;
	   case 0x7:
	      switch (reg) {
	         case 0x0:
	            sprintf(buf, "($%X).w", word1);
	            len = 2;
	            break;
	         case 0x1:
	            sprintf(buf, "($%lX).l", disp32);
	            len = 4;
	            break;
	         case 0x2:
	            sprintf(buf, "$%X(pc)", word1);
	            len = 2;
	            break;
	         case 0x3:
	            len = 2;
	            w_l     = (word1 & 0x0800) >> 11;
	            if (w_l)
	               w_char = 'l';
	            else
	               w_char = 'w';
	            scale   = (word1 & 0x0600) >> 9;
	            if (scale == 0x3)
	               scale = 8;
	            else
	               scale *= 2;
	            strcpy(buf, "(");

	            if (word1 & 0x0100) {
	               bd_size = (word1 & 0x0030) >> 4;
	               if (bd_size == 0x2) {
	                  disp32 = (uInt32) word2;
	                  if (disp32)
	                     len = 4;
	               }
	               else {
	                  if (bd_size == 0x3) {
	                     disp32 = (((uInt32) word2) << 16)|((uInt32) word3);
	                     if (disp32)
	                        len = 6;
	                  }
	                  else
	                     disp32 = 0;
	               }
	            }
	            else {
	               disp32 = (uInt32) (word1 & 0xFF);
	            }
	            if (disp32)
	               sprintf(buf, "$%lX(", disp32);
	            else
	               strcpy(buf, "(");

	            if (da_bit) {
	               if (index_reg == 7)
	                  sprintf(tmp_buf, "pc, sp.%c", w_char);
	               else
	                  sprintf(tmp_buf, "pc, a%u.%c", index_reg, w_char);
	            }
	            else {
	               sprintf(tmp_buf, "pc, d%u.%c", index_reg, w_char);
	            }
	            strcat(buf, tmp_buf);

	            if (scale) {
	               sprintf(tmp_buf, "*%u", scale);
	               strcat(buf, tmp_buf);
	            }

	            strcat(buf, ")");

	            break;
	         case 0x4:
	            /*if (size > 0x2)
	               return(-1);*/
	            if (size == 0x0) {
	               if (word1 > 0xFF)
	                  return(-1);
	               disp32 = word1;
	               len = 2;
	            }
	            if (size == 0x1 || size == 0x3) {
	               disp32 = word1;
	               len = 2;
	            }
	            if (size == 0x2)
	               len = 4;
	            sprintf(buf, "#$%lX", disp32);
	            break;
	         default:
	            return(-1);
	      }
	      break;
	   default:
	      return(-1);
	}

	return(len);
}

int get_reg_list(
	char		*buf,
	uInt32		data,
	int		swap_flag)
{
	register uInt32	tmp;
	register int	i, j;

	if (data == 0)
	   return(-1);

	if (swap_flag) {
	   for (i = 0; i < 8; i++) {
	      tmp = data;
	      data = ((tmp & (1 << i)) << (15 - 2*i)) |
	             ((tmp & (1 << (15 - i))) >> (15 - 2*i)) |
	             ((tmp & ~((1 << i) | (1 << (15 - i)))));
	   }
	}

	for (i = j = 0; i < 16; i++) {
	   if (data & (1 << i)) {
	      if (j == 0 || buf[j-1] == '/') {
	         if (i < 8)
	            buf[j++] = 'd';
	         else
	            buf[j++] = 'a';
	      }
	      if (i < 8)
	         buf[j] = 48 + i;
	      else
	         buf[j] = 48 + i - 8;
	      if (i == 7) {
	         j++;
	         buf[j++] = '/';
	      }
	      else {
	         if (data & (1 << (i+1))) {
	            if (j == 1 || buf[j-2] != '-') {
	               j++;
	               buf[j++] = '-';
	               buf[j] = buf[j-3];
	               j++;
	            }
	         }
	         else {
	            j++;
	            buf[j++] = '/';
	         }
	      }
	   }
	}
	buf[j-1] = '\0';

	return(0);
}
#endif /* RISC */
#endif /* DEBUGFUNC */
#endif /* defined(DEBUGFUNC) defined(DEBUGFUNC_P) */

int byte_write(
	uInt32		addr,
	uInt32		data)
{
	*((unsigned char *) addr) = (unsigned char) data;

	return(0);
}

int word_write(
	uInt32		addr,
	uInt32		data)
{
	*((uInt16 *) addr) = (uInt16) data;

	return(0);
}

int long_write(
	uInt32		addr,
	uInt32		data)
{
	*((uInt32 *) addr) = (uInt32) data;

	return(0);
}

int byte_read(
	uInt32		addr,
	uInt32		*datap)
{
	*datap = (uInt32) *((unsigned char *) addr);

	return(0);
}

int word_read(
	uInt32		addr,
	uInt32		*datap)
{
	*datap = (uInt32) *((uInt16 *) addr);

	return(0);
}

int long_read(
	uInt32		addr,
	uInt32		*datap)
{
	*datap = *((uInt32 *) addr);

	return(0);
}

void display_progress(
	char		*msg_fmt,
	char		*access,
	uInt32		addr,
	char		*mode,
	uInt32		num,
	enum CALL_TYPE	call_type)
{
	char		buf[80];

	sprintf(cli_print_buf, msg_fmt, access, addr, mode);
#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
	if (call_type == DEBUG_CALL) {
	   sprintf(buf, " -> %lu", num);
	   strcat(cli_print_buf, buf);
	}
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */
#ifdef DIAGTEST
	if (call_type != DEBUG_CALL) {
	   sprintf(buf, " {%s} -> %lu", diag_test_id, diag_current_runcount);
	   strcat(cli_print_buf, buf);
	   if (sys_specific_info[0] != '\0') {
	      sprintf(buf, " [%s]", sys_specific_info); 
	      strcat(cli_print_buf, buf);
	   }
	}
#endif /* DIAGTEST */
	strcat(cli_print_buf, "\n");
	put_line(cli_print_buf);
}

int check_for_break()
{
	char		c;
	char		*argv[2];
	char		test_id[5];
	register int	i;

	Bgfunc();

	argv[1] = &test_id[0];

	*(uChar *)CNTL_REG_17=1;
	*(uChar *)CNTL_REG_17=0;

	c = (char) checkchar_p();

	if (c != ESC && c != CTL_C) {
	   switch (c) {
	      case CTL_R: 
	         if (sys_term_vt100)
	            set_cli_screen();
	         break;
	      case CTL_T: sys_clock_display = ~sys_clock_display & 0x1; break;
	      case '0':   diag_verbosity = VERBOSITY_LEVEL0; break;
	      case '1':   diag_verbosity = VERBOSITY_LEVEL1; break;
	      case '2':   if (diag_verbosity <= VERBOSITY_LEVEL1) 
	                     put_line("\n");
	                  diag_verbosity = VERBOSITY_LEVEL2; break;
	      case '3':   if (diag_verbosity <= VERBOSITY_LEVEL1)
	                     put_line("\n");
	                  diag_verbosity = VERBOSITY_LEVEL3; break;
	      default: break;
	   }
	   return(0);
	}

#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
#ifdef DIAGTEST
	if (diag_test_mode == FALSE) {
#endif /* DIAGTEST */
	   put_line(debug_break_msg);
	   while (1) {
	      c = (char) getchar_p();
	      if (isalpha(c) && isupper(c))
	         c = tolower(c);
	      if (c == 'y')
	         return(BREAK_DEBUG);
	      if (c == ' ')
	         return(RESUME_DEBUG);
	   }
#ifdef DIAGTEST
	}
#endif /* DIAGTEST */
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */

#ifdef DIAGTEST
	if (diag_verbosity <= VERBOSITY_LEVEL1) 
	    put_line("\n");

	sprintf(cli_print_buf, test_break_msg[0], diag_test_name, diag_test_id,
	        diag_current_runcount);
	put_line(cli_print_buf);
	while (1) {
	   for (i = 1; test_break_msg[i][0] != '\0'; i++) {
	      put_line(test_break_msg[i]);
	   }
	   while (1) {
	      c = (char) getchar_p();
	      /*if (isalpha(c) && isupper(c))
	         c = tolower(c);*/

	      if (c == CTL_A) {
	         put_line(all_quit_msg);
	         return(BREAK_ALL_TESTS);
	      }
	      if (c == CTL_C) {
	         sprintf(cli_print_buf, test_quit_msg, diag_test_name,
	                 diag_test_id, diag_current_runcount);
	         put_line(cli_print_buf);
	         return(BREAK_CURRENT_TEST);
	      }
	      if (c == ' ') {
	         sprintf(cli_print_buf, test_resume_msg, diag_test_name,
	                 diag_test_id, diag_current_runcount);
	         put_line(cli_print_buf);
	         return(RESUME_TEST);
	      }
	      if (c == CTL_R) {
	         sprintf(test_id, "-t%s", diag_test_id);
	         Results(2, argv);
	         break;
	      }
	      /*if (c >= 'a' && c <= 'z') {
	         sprintf(test_id, "-s%c", c);
	         if (Results(2, argv) == 0)
	            break;
	      }*/
	      if (c == CTL_D) {
	         debug_cli();
	         break;
	      }
	   }
	}
#endif /* DIAGTEST */
}

#ifdef DEBUGFUNC
#ifdef RISC
int get_reg(
	char		**buf_ptr,
	uInt32		*r_ptr,
	char		tc,
	int		flag)
{
	char		reg[12];
	int		i;

	if (flag) {
	   for (i= 0; **buf_ptr != ' ' && **buf_ptr != '\0' && 
	              **buf_ptr != tc && i < 10; i++) {
	      reg[i] = **buf_ptr; *buf_ptr += 1;
	   }
	   reg[i] = '\0';
	   if ((**buf_ptr == '\0' && **buf_ptr != tc) || i == 10)
	      return(ASM_INVALID_INSTRUCTION_FORMAT);

	   for (i = 0; i < 32; i++) {
	      if (flag == 2) { /* COP0 */
	         if (!strcmp_nocase(reg, reg_name_c0[i]))
	            break;
	      }
	      else {
	         if (!strcmp_nocase(reg, reg_name[i]) ||
	             !strcmp_nocase(reg, reg_name_a[i]))
	            break;
	      }
	   }
	   if (i == 32)
	      return(ASM_INVALID_OPERAND);
	   *r_ptr = i;
	}
	else {
	   if (isdigit(**buf_ptr)) {
	      reg[0] = **buf_ptr; *buf_ptr += 1;
	      if (isdigit(**buf_ptr)) {
	         reg[1] = **buf_ptr; *buf_ptr += 1; reg[2] = '\0';
	      }
	      else
	         reg[1] = '\0';
	      if (!sscanf_x(reg, fmt_uInt32, r_ptr))
	         return(ASM_INVALID_OPERAND);
	      if (*r_ptr > 31)
	         return(ASM_OPERAND_OUT_OF_RANGE);
	   }
	   else
	      return(ASM_INVALID_OPERAND);
	}

	if (**buf_ptr == ' ') *buf_ptr += 1;

	if (**buf_ptr != tc)
	   return(ASM_INVALID_INSTRUCTION_FORMAT);

	if (tc != '\0') *buf_ptr += 1;
	if (**buf_ptr == ' ') *buf_ptr += 1;

	return(0);
}

int asm_special(
	union instruction_type	*mc,
	char		buf[])
{
	uInt32		rt, rd, rs, shamt;
	int		rc;

	rt = rd = rs = shamt = 0;
	switch (mc->r_type.funct) {
	   case SPECIAL_SLL: case SPECIAL_SRL: case SPECIAL_SRA:
	      if (rc = get_reg(&buf, &rd, ',', 1))
	         return(rc);
	      if (rc = get_reg(&buf, &rt, ',', 1))
	         return(rc);
	      if (rc = get_reg(&buf, &shamt, '\0', 0))
	         return(rc);
	      break;
	   case SPECIAL_SLLV: case SPECIAL_SRLV: case SPECIAL_SRAV:
	      if (rc = get_reg(&buf, &rd, ',', 1))
	         return(rc);
	      if (rc = get_reg(&buf, &rt, ',', 1))
	         return(rc);
	      if (rc = get_reg(&buf, &rs, '\0', 1))
	         return(rc);
	      break;
	   case SPECIAL_JR:
	      if (rc = get_reg(&buf, &rs, '\0', 1))
	         return(rc);
	      break;
	   case SPECIAL_JALR:
	      if (rc = get_reg(&buf, &rs, ',', 1))
	         return(rc);
	      if (rc = get_reg(&buf, &rd, '\0', 1))
	         return(rc);
	      break;
	   case SPECIAL_MFHI: case SPECIAL_MTHI:
	   case SPECIAL_MFLO: case SPECIAL_MTLO:
	      if (rc = get_reg(&buf, &rd, '\0', 1))
	         return(rc);
	      break;
	   case SPECIAL_MULT: case SPECIAL_MULTU:
	   case SPECIAL_DIV:  case SPECIAL_DIVU:
	      if (rc = get_reg(&buf, &rs, ',', 1))
	         return(rc);
	      if (rc = get_reg(&buf, &rt, '\0', 1))
	         return(rc);
	      break;
	   default:
	      if (rc = get_reg(&buf, &rd, ',', 1))
	         return(rc);
	      if (rc = get_reg(&buf, &rs, ',', 1))
	         return(rc);
	      if (rc = get_reg(&buf, &rt, '\0', 1))
	         return(rc);
	      break;
	}
	mc->r_type.rs = rs; mc->r_type.rt = rt; mc->r_type.rd = rd;
	mc->r_type.shamt = shamt;

	return(0);
}

int asm_bcond(
	union instruction_type	*mc,
	char		buf[],
	uInt32		addr)
{
	uInt32		target, rs;
	Int32		immediate;
	int		rc;

	if (rc = get_reg(&buf, &rs, ',', 1))
	   return(rc);

	if (*buf == '$') buf++;
	if (!sscanf_x(buf, fmt_xInt32, &target))
	   return(ASM_INVALID_OPERAND);
	immediate = target - addr - 4;
	if (immediate & 0x3)
	   return(ASM_INVALID_OPERAND);
	immediate >>= 2;
	if ((immediate & 0xFFFF0000) != 0 &&
	    (immediate & 0xFFFF0000) != 0xFFFF0000)
	   return(ASM_OPERAND_OUT_OF_RANGE);

	mc->i_type.rs = rs;
	mc->i_type.immediate = immediate;

	return(0);
}

int asm_default(
	union instruction_type	*mc,
	char		buf[],
	uInt32		addr)
{
	uInt32		rt, rs, target, immediate;
	int		rc, i;
	char		imm[10];

	rt = rs = immediate = 0;
	switch (mc->i_type.op) {
	   case OPCODE_J: case OPCODE_JAL:
	      if (*buf == '$') buf++;
	      if (!sscanf_x(buf, fmt_xInt32, &target))
	         return(ASM_INVALID_OPERAND);
	      if (target & 0x3)
	         return(ASM_INVALID_OPERAND);
	      target &= (target & 0x0FFFFFFF) >> 2;
	      mc->j_type.target = target;
	      break;
	   case OPCODE_BEQ: case OPCODE_BNE:
	   case OPCODE_BLEZ: case OPCODE_BGTZ:
	      if (rc = get_reg(&buf, &rs, ',', 1))
	         return(rc);
	      if (mc->i_type.op == OPCODE_BEQ || mc->i_type.op == OPCODE_BNE) {
	         if (rc = get_reg(&buf, &rt, ',', 1))
	            return(rc);
	      }
	      if (*buf == '$') buf++;
	      if (!sscanf_x(buf, fmt_xInt32, &target))
	         return(ASM_INVALID_OPERAND);
	      immediate = target - addr - 4;
	      if (immediate & 0x3)
	         return(ASM_INVALID_OPERAND);
	      immediate >>= 2;
	      if ((immediate & 0xFFFF0000) != 0 &&
	          (immediate & 0xFFFF0000) != 0xFFFF0000)
	         return(ASM_OPERAND_OUT_OF_RANGE);
	      break;
	   case OPCODE_ADDI: case OPCODE_ADDIU: 
	   case OPCODE_SLTI: case OPCODE_SLTIU:
	   case OPCODE_ANDI: case OPCODE_ORI:
	   case OPCODE_XORI: case OPCODE_LUI:
	      if (rc = get_reg(&buf, &rt, ',', 1))
	         return(rc);
	      if (mc->i_type.op != OPCODE_LUI) {
	         if (rc = get_reg(&buf, &rs, ',', 1))
	            return(rc);
	      }
	      if (*buf == '$') buf++;
	      if (!sscanf_x(buf, fmt_xInt32, &immediate))
	         return(ASM_INVALID_OPERAND);
	      if ((immediate & 0xFFFF0000) != 0)
	         return(ASM_OPERAND_OUT_OF_RANGE);
	      break;
	   default:
	      if (mc->i_type.op == OPCODE_LWC0 || 
	          mc->i_type.op == OPCODE_SWC0) {
	         if (rc = get_reg(&buf, &rt, ',', 2))
	            return(rc);
	      }
	      else {
	         if (rc = get_reg(&buf, &rt, ',', 1))
	            return(rc);
	      }
	      if (*buf == '$') buf++;
	      for (i = 0; *buf != '\0' && *buf != ' ' && *buf != '('; i++)
	         imm[i] = *buf++;
	      imm[i] = '\0';
	      if (*buf == '\0')
	         return(ASM_INVALID_INSTRUCTION_FORMAT);
	      if (!sscanf_x(imm, fmt_xInt32, &immediate))
	         return(ASM_INVALID_OPERAND);
	      if ((immediate & 0xFFFF0000) != 0)
	         return(ASM_OPERAND_OUT_OF_RANGE);
	      if (*buf == ' ') buf++;
	      if (*buf++ != '(')
	         return(ASM_INVALID_INSTRUCTION_FORMAT);
	      if (*buf == ' ') buf++;
	      if (*buf == '\0')
	         return(ASM_INVALID_INSTRUCTION_FORMAT);
	      if (rc = get_reg(&buf, &rs, ')', 1))
	         return(rc);
	      if (*buf != '\0')
	         return(ASM_INVALID_INSTRUCTION_FORMAT);
	}

	if (!(mc->i_type.op == OPCODE_J || mc->i_type.op == OPCODE_JAL)) {
	   mc->i_type.rs = rs; mc->i_type.rt = rt;
	   mc->i_type.immediate = immediate;
	}

	return(0);
}

int asm_cop0(
	union instruction_type	*mc,
	char		buf[])
{
	uInt32		rt, rd;
	int		rc;

	if (rc = get_reg(&buf, &rt, ',', 1))
	   return(rc);

	if (rc = get_reg(&buf, &rd, '\0', 2))
	   return(rc);

	mc->r_type.rt = rt;
	mc->r_type.rd = rd;

	return(0);
}

int dasm_special(
	union instruction_type	*mc,
	char		buf[])
{
	switch (mc->r_type.funct) {
	   case SPECIAL_SLL: case SPECIAL_SRL: case SPECIAL_SRA:
	      if (mc->r_type.rs != 0)
	         return(1);
	      sprintf(buf, "%-8s %s, %s, %u", instruction[0][mc->r_type.funct],
	              reg_name_a[mc->r_type.rd], reg_name_a[mc->r_type.rt],
	              mc->r_type.shamt);
	      break;
	   case SPECIAL_SLLV: case SPECIAL_SRLV: case SPECIAL_SRAV:
	      if (mc->r_type.shamt != 0)
	         return(1);
	      sprintf(buf, "%-8s %s, %s, %s", instruction[0][mc->r_type.funct],
	              reg_name_a[mc->r_type.rd], reg_name_a[mc->r_type.rt],
	              reg_name_a[mc->r_type.rs]);
	      break;
	   case SPECIAL_JR:
	      if (mc->r_type.rt != 0 || mc->r_type.rd != 0 ||
	          mc->r_type.shamt != 0)
	         return(1);
	      sprintf(buf, "%-8s %s", instruction[0][mc->r_type.funct],
	              reg_name_a[mc->r_type.rs]);
	      break;
	   case SPECIAL_JALR:
	      if (mc->r_type.rt != 0 || mc->r_type.shamt != 0)
	         return(1);
	      sprintf(buf, "%-8s %s, %s", instruction[0][mc->r_type.funct],
	              reg_name_a[mc->r_type.rs], reg_name_a[mc->r_type.rd]);
	      break;
	   case SPECIAL_SYSCALL: case SPECIAL_BREAK:
	      if (mc->r_type.rt != 0 || mc->r_type.rd != 0 ||
	          mc->r_type.rs != 0 || mc->r_type.shamt != 0)
	         return(1);
	      sprintf(buf, "%-8s", instruction[0][mc->r_type.funct]);
	      break;
	   case SPECIAL_MFHI: case SPECIAL_MTHI:
	   case SPECIAL_MFLO: case SPECIAL_MTLO:
	      if (mc->r_type.rt != 0 ||
	          mc->r_type.rs != 0 || mc->r_type.shamt != 0)
	         return(1);
	      sprintf(buf, "%-8s %s", instruction[0][mc->r_type.funct],
	              reg_name_a[mc->r_type.rd]);
	      break;
	   case SPECIAL_MULT: case SPECIAL_MULTU:
	   case SPECIAL_DIV:  case SPECIAL_DIVU:
	      if (mc->r_type.rd != 0 || mc->r_type.shamt != 0)
	         return(1);
	      sprintf(buf, "%-8s %s, %s", instruction[0][mc->r_type.funct],
	              reg_name_a[mc->r_type.rs], reg_name_a[mc->r_type.rt]);
	      break;
	   default:
	      if (*instruction[0][mc->r_type.funct] == '\0' ||
	          mc->r_type.shamt != 0)
	         return(1);
	      sprintf(buf, "%-8s %s, %s, %s", instruction[0][mc->r_type.funct],
	              reg_name_a[mc->r_type.rd], reg_name_a[mc->r_type.rs],
	              reg_name_a[mc->r_type.rt]);
	      break;
	}

	return(0);
}

int dasm_bcond(
	union instruction_type	*mc,
	char		buf[])
{
	uInt32		addr, target;
	Int16		immediate;

	addr = (uInt32) mc;
	immediate = mc->i_type.immediate;

	switch (mc->i_type.rt) {
	   case BCOND_BLTZ: case BCOND_BGEZ: 
	   case BCOND_BLTZAL: case BCOND_BGEZAL:
	      target = addr + 4 + (immediate << 2);
	      sprintf(buf, "%-8s %s, $%X", instruction[1][mc->i_type.rt], 
	              reg_name_a[mc->i_type.rs], target);
	      break;
	   default:
	      return(1);
	}

	return(0);
}

int dasm_default(
	union instruction_type	*mc,
	char		buf[])
{
	uInt32		addr, target;
	Int16		immediate;

	addr = (uInt32) mc;
	immediate = mc->i_type.immediate;

	switch (mc->i_type.op) {
	   case OPCODE_J: case OPCODE_JAL:
	      target = (((uInt32) addr) & 0xF0000000) | 
	               (mc->j_type.target << 2);
	      sprintf(buf, "%-8s $%X", instruction[2][mc->j_type.op], target);
	      break;
	   case OPCODE_BEQ: case OPCODE_BNE:
	      target = addr + 4 + (immediate << 2);
	      sprintf(buf, "%-8s %s, %s, $%X", instruction[2][mc->i_type.op], 
	              reg_name_a[mc->i_type.rs], reg_name_a[mc->i_type.rt],
	              target);
	      break;
	   case OPCODE_BLEZ: case OPCODE_BGTZ:
	      if (mc->i_type.rt != 0)
	         return(1);
	      target = addr + 4 + (immediate << 2);
	      sprintf(buf, "%-8s %s, $%X", instruction[2][mc->i_type.op], 
	              reg_name_a[mc->i_type.rs], target);
	      break;
	   case OPCODE_ADDI: case OPCODE_ADDIU: 
	   case OPCODE_SLTI: case OPCODE_SLTIU:
	   case OPCODE_ANDI: case OPCODE_ORI:
	   case OPCODE_XORI: 
	      sprintf(buf, "%-8s %s, %s, $%X", instruction[2][mc->i_type.op],
	              reg_name_a[mc->i_type.rt], reg_name_a[mc->i_type.rs],
	              mc->i_type.immediate);
	      break;
	   case OPCODE_LUI:
	      if (mc->i_type.rs != 0)
	         return(1);
	      sprintf(buf, "%-8s %s, $%X", instruction[2][mc->i_type.op],
	              reg_name_a[mc->i_type.rt], mc->i_type.immediate);
	      break;
	   case OPCODE_COP0: case OPCODE_COP1: 
	   case OPCODE_COP2: case OPCODE_COP3:
	      switch (mc->r_type.rs) {
	         case COP_MF: case COP_CF: case COP_MT: case COP_CT:
	            if (reg_name_c0[mc->r_type.rd][0] == '\0')
	               return(1);
	            sprintf(buf,"%-8s %s, %s",
	                    cop_instruction[0][mc->r_type.rs+
	                                      (mc->i_type.op-OPCODE_COP0)*8],
	                    reg_name_a[mc->r_type.rt],
	                    reg_name_c0[mc->r_type.rd]);
	            break;
	         case COP_BC:
	            switch (mc->i_type.rt) {
	               case BC_F: case BC_T:
	                  target = addr + 4 + (immediate << 2);
	                  sprintf(buf, "%-8s $%X", 
	                          cop_instruction[1][mc->i_type.rt+
	                                         (mc->i_type.op-OPCODE_COP0)*8],
	                          target);
	                  break;
	               default:
	                  return(1);
	            }
	            break;
	         default:
	            if (*cop_instruction[2][mc->r_type.funct] == '\0')
	               return(1);
	            sprintf(buf, "%-8s", cop_instruction[2][mc->r_type.funct]);
	            break;
	      }
	      break;
	   default:
	      if (*instruction[2][mc->i_type.op] == '\0')
	         return(1);
	      if (mc->i_type.op == OPCODE_LWC0 || 
	          mc->i_type.op == OPCODE_SWC0) {
	         if (reg_name_c0[mc->r_type.rt][0] == '\0')
	            return(1);
	         sprintf(buf, "%-8s %s, $%X(%s)", instruction[2][mc->i_type.op],
	                 reg_name_c0[mc->i_type.rt], mc->i_type.immediate,
	                 reg_name_a[mc->i_type.rs]);
	      }
	      else {
	         sprintf(buf, "%-8s %s, $%X(%s)", instruction[2][mc->i_type.op],
	                 reg_name_a[mc->i_type.rt], mc->i_type.immediate,
	                 reg_name_a[mc->i_type.rs]);
	      }
	      break;
	}

	return(0);
}
#endif /* RISC */
#endif /* DEBUGFUNC */
