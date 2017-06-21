/* $Id: cli.c,v 1.1.4.1 1996/05/09 15:00:49 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/cli.c,v $
 *-----------------------------------------------------------------
 * cli.c -- Command Line Interpreter (CLI)
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cli.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:49  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:47  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:08  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:58  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:41  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:17  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************/
/*                                                                            */
/* DIAGNOSTICS DRIVER                                                         */
/*                                                                            */
/* This file contains procedures related to "Command Line Interpreter" (CLI)  */
/* that are included as part of Diagnostics Driver.                           */
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
/* 12/21/94  rlb
 *    adapted to Synergy ATM card
 */

#include	"ctype.h"
#include	"types.h"
#include	"constant.h"
#include	"struct.h"
#include	"vardef.h"
#include	"globdef.h"

	enum		ARGUMENT_OPTION {
	ARGUMENT_OPTION_COMMAND			= 'c',
	ARGUMENT_OPTION_NUMBER_OF_COMMANDS	= 'n'
	};

	enum		COMMAND_TYPE {
	INFO_CMD = 0,
	EXEC_CMD = 1,
	DEBUG_CMD = 2,
	LOADGEN_CMD = 3,
	MISC_CMD = 4
	};

#define	WRITE_INDEX	0
#define	READ_INDEX	1
#define	COMPARE_INDEX	2
#define	SEARCH_INDEX	3
#define	ASM_INDEX	4
#define	DASM_INDEX	5
#define	EXECUTE_INDEX	6
#define	TEST_INDEX	7

#define	CONTENTS_INDEX	0
#define	SET_INDEX	1
#define	RESET_INDEX	2
#define	SHOW_INDEX	3
#define	RUN_INDEX	4
#define	SKIP_INDEX	5
#define	RESULTS_INDEX	6

extern	char		sys_logo1[];
extern	char		sys_logo2[];
extern	char		sys_date[];
extern	char		sys_time[];
extern	char		sys_title[];
extern	char		sys_cpr1[];
extern	char		sys_cpr2[];
extern	int		sys_help_sort;

#ifdef DIAGTEST
extern	int		diag_test_mode;
#endif /* DIAGTEST */

extern	int		sys_cmd_no_flag;
extern	int		sys_term_vt100;

	char		*cli_command_type[] =
	{ "Informational Commands:",
	  "Test Execution Commands:",
	  "Debug Commands:",
	  "Load Generation Commands:",
	  "Miscellaneous Commands:" };

	enum USER_LEVEL	cli_user_level;
	int		cli_history_number;

	char		*cli_argv[MAX_COMMAND_ARGUMENTS];
	char		cli_argb[MAX_COMMAND_ARGUMENTS][COMMAND_LINE_SIZE];
#ifndef CALYPSO
	char		cli_prompt_line[PROMPT_LINE_SIZE+1] = "=>";
#else /* CALYPSO */
#ifdef RISC
	char		cli_prompt_line[PROMPT_LINE_SIZE+1] = "SAR>";
#else /* !RISC */
	char		cli_prompt_line[PROMPT_LINE_SIZE+1] = "ACP>";
#endif /* RISC */
#endif /* CALYPSO */
	char		cli_command_line[COMMAND_LINE_SIZE];
	char		cli_history_buf[HISTORY_BUF_SIZE][HISTORY_LINE_SIZE];
	char		cli_display_buf[DISPLAY_BUF_SIZE][DISPLAY_LINE_SIZE];
	char		*cli_display_buf_ptr[DISPLAY_BUF_SIZE];

	char		cli_print_buf[PRINT_LINE_SIZE];

#ifdef DIAGTEST
	char		cli_test_buf[TEST_BUF_SIZE][TEST_LINE_SIZE];
	char		*cli_test_buf_ptr[TEST_BUF_SIZE];
	char		*debug_cli_argv[MAX_COMMAND_ARGUMENTS];
	char		debug_cli_argb[MAX_COMMAND_ARGUMENTS][COMMAND_LINE_SIZE];
	char		debug_cli_command_line[COMMAND_LINE_SIZE];
#endif /* DIAGTEST */

	void		set_cli_screen();
	void		reset_cli_screen();
	void		display_cli_heading();
	void		display_cli_time();
	void		display_cli_copyright();
	void		syntax_error_handler();
	void		update_history();
	char		*search_history();
	struct COMMAND_TABLE *seacrch_command();

/*---------------------------------------------------------------
 * cli()
 *---------------------------------------------------------------*/
#ifdef CALYPSO
int cli(
	int	power_on)
#else /* CALYPSO */
int cli()
#endif /* CALYPSO */
{
	int		i;
	int		rc;

	for (i = 0; i < MAX_COMMAND_ARGUMENTS; i++) {
	   cli_argv[i] = &cli_argb[i][0];
#ifdef DIAGTEST
	   debug_cli_argv[i] = &debug_cli_argb[i][0];
#endif /* DIAGTEST */
	}

	for (i = 0; i < DISPLAY_BUF_SIZE; i++)
	   cli_display_buf_ptr[i] = &cli_display_buf[i][0];

#ifdef DIAGTEST
	for (i = 0; i < TEST_BUF_SIZE; i++)
	   cli_test_buf_ptr[i] = &cli_test_buf[i][0];
#endif /* DIAGTEST */
	cli_history_number = 1;

	if ((cli_user_level = display_logo()) == USER_LOGOUT)
	   return(cli_user_level);

#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
#ifdef DIAGTEST
	if (cli_user_level == USER_GENERAL) {
	   debug_command_table[WRITE_INDEX].display_control   = OFF;
	   debug_command_table[READ_INDEX].display_control    = OFF;
	   debug_command_table[COMPARE_INDEX].display_control = OFF;
	   debug_command_table[SEARCH_INDEX].display_control  = OFF;
	   debug_command_table[TEST_INDEX].display_control    = OFF;
	}
#ifdef DEBUGFUNC
	if (cli_user_level == USER_GENERAL || 
	    cli_user_level == USER_CRESCENDO) {
	   debug_command_table[ASM_INDEX].display_control     = OFF;
	   debug_command_table[DASM_INDEX].display_control    = OFF;
	   debug_command_table[EXECUTE_INDEX].display_control = OFF;
	}
#endif /* DEBUGFUNC */
#endif /* DIAGTEST */
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */

#ifdef CALYPSO
	if (!power_on) {
#endif /* CALYPSO */
	   if (sys_term_vt100)
	      set_cli_screen();

	   Version(0, 0);
	   rc = cli_command_loop(cli_command_line, cli_argv, 0);

	   if (sys_term_vt100)
	      reset_cli_screen();

	   return(rc);
#ifdef CALYPSO
	}
	else
	   return(0);
#endif /* CALYPSO */
}

/*----------------------------------------------------------------------
 * debug_cli()
 *----------------------------------------------------------------------*/
#ifdef DIAGTEST
int debug_cli()
{
	register int	rc;

	diag_command_table[RUN_INDEX].display_control   = OFF;

	update_history("debug", cli_history_buf, cli_history_number);
	cli_history_number++;
	rc = cli_command_loop(debug_cli_command_line, debug_cli_argv, 1);

	diag_command_table[RUN_INDEX].display_control   = ON;

	diag_test_mode = TRUE;

	return(rc);
}
#endif /* DIAGTEST */

/*----------------------------------------------------------------------
 * display_logo()
 *----------------------------------------------------------------------*/
int display_logo()
{
	enum USER_LEVEL	user_level;
	char		c;
	register uInt32	start_time;

	/*for (c = 0, start_time = system_tick_time; 
	    (system_tick_time - start_time) < 10000; ) {
	   if (dgetch(&c) != 0 && c == CTL_D)
	      break;
	}
	if (c != CTL_D)
	   return(user_level = USER_LOGOUT);*/

	/* Home/Clear */
	/*sprintf(cli_print_buf, "\n\n%c[1;1H%c[2J%c[0m%s\n\n", ESC, ESC, ESC,
	        sys_logo1);
	put_line(cli_print_buf);

	sprintf(cli_print_buf, "%s\n\n", sys_logo2);
	put_line(cli_print_buf);*/

	/* Insert LOGO Screen code here */
	return(user_level = USER_ROOT);
}

/*----------------------------------------------------------------------
 * set_cli_screen()
 *----------------------------------------------------------------------*/
void set_cli_screen()
{
	if (sys_term_vt100) {
	   /* Home/Clear/Underscore */
	   sprintf(cli_print_buf, "%c[1;1H%c[2J%c[0;4m", ESC, ESC, ESC);
	   put_line(cli_print_buf);

	   display_cli_heading();

	   display_cli_copyright();

	   /* Set_Scroll/Prompt_Position/Bold */
	   sprintf(cli_print_buf, "%c[3;21r%c[3;1H%c[0;1m", ESC, ESC, ESC);
	   put_line(cli_print_buf);
	}
}

/*----------------------------------------------------------------------
 * reset_cli_screen()
 *----------------------------------------------------------------------*/
void reset_cli_screen()
{
	/* Reset_Scroll/Normal/Home/Clear */
	sprintf(cli_print_buf, "%c[1;24r%c[0m%c[1;1H%c[2J", ESC, ESC, ESC, ESC);
	put_line(cli_print_buf);
}

/*----------------------------------------------------------------------
 * display_cli_heading()
 *----------------------------------------------------------------------*/
void display_cli_heading()
{
	register int	i, l_blanks;
	char		buf[100];

	if (sys_term_vt100) {
	   l_blanks = (64 - strlen(sys_title))/2;

	   for (i = 0; i < l_blanks; i++) {
	      buf[i] = ' ';
	   }
	   strcpy(&buf[i], sys_title);
	   for (i += strlen(sys_title); i < 64; i++) {
	      buf[i] = ' ';
	   }
	   buf[i] = '\0';

	   /* Save_cursor/Display/Restore_cursor */
	   sprintf(cli_print_buf, "%c7%c[1;1H%c[0;4m%s%s%s%c8", ESC, ESC, ESC,
	           sys_date, buf, sys_time, ESC);
	   put_line(cli_print_buf);
	}
}

/*----------------------------------------------------------------------
 * display_cli_time()
 *----------------------------------------------------------------------*/
void display_cli_time()
{
	if (sys_term_vt100) {
	   /* Save_cursor/Display/Restore_cursor */
	   sprintf(cli_print_buf, "%c7%c[1;73H%c[0;4m%s%c8", ESC, ESC, ESC,
	           sys_time, ESC);
	   put_line(cli_print_buf);
	}
}

/*----------------------------------------------------------------------
 * display_cli_copyright()
 *----------------------------------------------------------------------*/
void display_cli_copyright()
{
	register int	l_blanks;

	/* Display Copyright-1 */
	l_blanks = (80 - strlen(sys_cpr1))/2;
	sprintf(cli_print_buf, "%c[23;%uH%s", ESC, l_blanks, sys_cpr1);
	put_line(cli_print_buf);

	/* Display Copyright-2 */
	l_blanks = (80 - strlen(sys_cpr2))/2;
	sprintf(cli_print_buf, "%c[24;%uH%s", ESC, l_blanks, sys_cpr2);
	put_line(cli_print_buf);
}

/*----------------------------------------------------------
 * cli_command_loop()
 *----------------------------------------------------------*/
int cli_command_loop(
	char		command_line[],
	char		*argv[],
	int		debug_flag)
{
	register int	i, j, k, argc;
	register int	rc, command_error, history_execution, hist_ref;
	register int	command_index;
	register char	*cmd_ptr;
	uInt32		command_number;
	char		Command_line[COMMAND_LINE_SIZE];
	char		num[10];
#ifndef BOOT
#ifdef RISC
	extern int	switchtoNMP;
#else /* RISC */
	extern int	switchtoDMP_VConsole;
#endif /* RISC */
#endif /* BOOT */

	while (1) {

#ifdef DIAGTEST
	   diag_test_mode = FALSE;
#endif /* DIAGTEST */

#ifdef CALYPSO
#ifndef BOOT
#ifdef RISC
	   if (!switchtoNMP)
#else /* RISC */
	   if (!switchtoDMP_VConsole)
#endif /* RISC */
#endif /* BOOT */
	   if (sys_cmd_no_flag)
	      sprintf(cli_print_buf, "%s%u%% ", 
	              (debug_flag) ? "->" : cli_prompt_line, 
	              cli_history_number);
	   else
	      sprintf(cli_print_buf, "%s%% ", 
	              (debug_flag) ? "->" : cli_prompt_line);
#else /* CALYPSO */
	   sprintf(cli_print_buf, "%s%u%% ", 
	           (debug_flag) ? "->" : cli_prompt_line, 
	           cli_history_number);
#endif /* CALYPSO */
#ifdef CALYPSO
#ifndef BOOT
#ifdef RISC
	   if (!switchtoNMP)
#else /* RISC */
	   if (!switchtoDMP_VConsole)
#endif /* RISC */
#endif /* BOOT */
#endif /* CALYPSO */
	   put_line(cli_print_buf);

	   get_line(command_line, 
                    COMMAND_LINE_SIZE - strlen(cli_print_buf));

	   command_error = history_execution = FALSE;
	   if (cli_history_number > 1) {
	      if (*command_line == '.' && *(command_line+1) == '\0') {
		 command_index = (cli_history_number-1)%HISTORY_BUF_SIZE - 1;
		 if (command_index < 0)
		    command_index = HISTORY_BUF_SIZE - 1;
                 strcpy(command_line, cli_history_buf[command_index]);
	         history_execution = TRUE;
	      }
              else {
	         if (*command_line == '!') {
	   	    if (*(command_line+1) == '!') {
		       command_index=(cli_history_number-1)%HISTORY_BUF_SIZE-1;
		       if (command_index < 0)
		          command_index = HISTORY_BUF_SIZE - 1;
		       strcpy(Command_line, command_line+2);
		       sprintf(command_line, "%s%s",
                               cli_history_buf[command_index], Command_line);
	               history_execution = TRUE;
		    }
		    else {
	               if (*(command_line+1) == '-') {
	                  j = 2; hist_ref = TRUE;
	               }
	               else {
	                  j = 1; hist_ref = FALSE;
	               }
	               for (i = j, k = 0; isdigit(*(command_line+i)) && k < 8; 
	                    i++, k++)
	                  num[k] = *(command_line+i);
	               num[k] = '\0';
		       if (sscanf_x(num, fmt_uInt32, &command_number)) {
	                  if (hist_ref == TRUE)
	                     command_number = cli_history_number-command_number;
		          if (command_number < cli_history_number &&
		   	     (cli_history_number - command_number) <=
                                                             HISTORY_BUF_SIZE) {
			     command_index = command_number%HISTORY_BUF_SIZE-1;
			     if (command_index < 0)
			        command_index = HISTORY_BUF_SIZE - 1;
	                     /*for (i = j; isdigit(*(command_line+i)); i++);*/
	                     strcpy(Command_line, command_line+i);
			     sprintf(command_line, "%s%s",
			          cli_history_buf[command_index], Command_line);
	                     history_execution = TRUE;
		          }
		          else {
			     command_error = TRUE;
			  }
		       }
		       else {
		          if ((cmd_ptr = search_history(command_line+1,
					                cli_history_buf,
                                                 cli_history_number)) != NULL) {
			     strcpy(command_line, cmd_ptr);
	                     history_execution = TRUE;
		          }
		          else {
			     command_error = TRUE;
			  }
		       }
		    }
	         }
	      }
	   }

	   if (history_execution) {
	      sprintf(cli_print_buf, "%s\n", command_line);
	      put_line(cli_print_buf);
	   }


	   if (!command_error) {
	      if (*command_line != '\0') {
	         update_history(command_line, cli_history_buf, 
                                cli_history_number);
	         cli_history_number++;
	      }
	      for (cmd_ptr = command_line; *cmd_ptr != '\0'; ) {
	         for (i = 0; *cmd_ptr != ';' && *cmd_ptr != '\0'; i++)
	            Command_line[i] = *cmd_ptr++;

	         /* chop off trailing blanks in intermediate Command_line*/
	         for (i--; Command_line[i] == ' '; i--);
	         Command_line[++i] = '\0';

	         /* chop off leading blanks in original command_line */
	         if (*cmd_ptr == ';')
	            cmd_ptr++;
	         while (*cmd_ptr == ' ')
	            cmd_ptr++;

                 argc = parse_line(Command_line, argv);
	         if ((rc = execute_command(argc, argv)) == CLI_EXIT)
		    break;
	         if (rc == CLI_INVALID_COMMAND)
	            syntax_error_handler(help_syntax, CLI_INVALID_COMMAND);
	      }
	      if (rc == CLI_EXIT)
		 break;
	   }
	   else
	      syntax_error_handler(history_syntax, CLI_COMMAND_NOT_IN_HISTORY);
	}

	return(rc);
}

/*----------------------------------------------------------------------
 * parse_line()
 *----------------------------------------------------------------------*/
int parse_line(
	char		*command_line,
	char		*argv[])
{
	register int	i, j, argc;

	for (i = 0; *command_line != '\0'; i++) {
	   for (j = 0; *command_line != ' ' && *command_line != '\0'; ) {
	      argv[i][j++] = *command_line++;
	   }
	   argv[i][j] = '\0';

	   while (*command_line == ' ')
	      command_line++;
	}

	return(argc = i);
}

/*----------------------------------------------------------------------
 * update_history()
 *----------------------------------------------------------------------*/
void update_history(
	char		*command_line,
	char		history_buf[][HISTORY_LINE_SIZE],
	int		history_number)
{
	register int	history_index;

	history_index = history_number%HISTORY_BUF_SIZE - 1;
	if (history_index < 0)
	   history_index = HISTORY_BUF_SIZE - 1;

	strcpy(history_buf[history_index], command_line);
}

/*----------------------------------------------------------------------
 * search_history()
 *----------------------------------------------------------------------*/
char *search_history(
	char		*command_line,
	char		history_buf[][HISTORY_LINE_SIZE],
	int		history_number)
{
	register int	history_number_min, history_number_max;
	register int	history_index;

	if (history_number <= HISTORY_BUF_SIZE+1)
	   history_number_min = 1;
	else
	   history_number_min = history_number - HISTORY_BUF_SIZE;
	history_number_max = history_number - 1;

	for (history_number = history_number_max;
             history_number >= history_number_min; history_number--) {
	   history_index = history_number%HISTORY_BUF_SIZE - 1;
	   if (history_index < 0)
	      history_index = HISTORY_BUF_SIZE - 1;
	   if (!strcmp_v(command_line, history_buf[history_index]))
	      return(history_buf[history_index]);
	}

	return((char *) 0);
}

/*----------------------------------------------------------------------
 * search_command()
 *----------------------------------------------------------------------*/
struct COMMAND_TABLE *search_command(
	struct COMMAND_TABLE	*cmd_table,
	char			*cmd)
{
	while (cmd_table->command != NULL) {
	   if (cmd_table->display_control == ON &&
               !strncmp_v(cmd, cmd_table->command, cmd_table->abbrev))
	      return(cmd_table);
	   cmd_table++;
	}

	return((struct COMMAND_TABLE *) 0);
}

/*----------------------------------------------------------------------
 * execute_command()
 *----------------------------------------------------------------------*/
int execute_command(
	int		argc,
	char		*argv[])
{
	struct COMMAND_TABLE	*cmd_table;
	register int		rc;

	if (argc == 1 && !strcmp(argv[0], "?"))
	   return(Help(argc, argv));

	if ((cmd_table = search_command(info_command_table, argv[0])) ==
						     (struct COMMAND_TABLE *) 0)
#ifdef DIAGTEST
	   if ((cmd_table = search_command(diag_command_table, argv[0])) ==
						     (struct COMMAND_TABLE *) 0)
#endif /* DIAGTEST */
#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
	      if ((cmd_table = search_command(debug_command_table, argv[0])) ==
						     (struct COMMAND_TABLE *) 0)
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */
#ifdef LOADGEN
	         if ((cmd_table = search_command(loadgen_command_table,argv[0]))
					          == (struct COMMAND_TABLE *) 0)
#endif /* LOADGEN */
	            if ((cmd_table = search_command(misc_command_table,argv[0]))
					          == (struct COMMAND_TABLE *) 0)
		       return(CLI_INVALID_COMMAND);

	rc = (cmd_table->procedure)(argc, argv);

	if (rc > SYNTAX_ERROR_START && rc < SYNTAX_ERROR_END)
	   syntax_error_handler(cmd_table->syntax, rc);

	return(rc);
}

/*----------------------------------------------------------------------
 * Help()
 *----------------------------------------------------------------------*/
int Help(
	int		argc,
	char		*argv[])
{
	struct COMMAND_TABLE	*cmd_table;
	register char		*cmd;
	int			buf_index, all_flag;
	register int		rc, i, j, k;
	char			tmpbuf[DISPLAY_LINE_SIZE];

	if (argc == 2 && !strcmp_nocase(argv[1], "all"))
	   all_flag = TRUE;
	else
	   all_flag = FALSE;

	for (buf_index = 0, i = 1; i < argc || argc == 1; i++) {
	   if (argc > 1) {
	      if (all_flag) {
	         cmd = argv[1];
	      }
	      else {
	         cmd = argv[i];
	         if (*cmd++ != '-')
	            return(SYNTAX_INVALID_ARGUMENT_DELIMITER_ERROR);
	         if (*cmd++ != ARGUMENT_OPTION_COMMAND)
	            return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
	         if (*cmd == '\0') {
	            if (i == (argc-1))
		      return(SYNTAX_COMMAND_NOT_SPECIFIED_ERROR);
	            cmd = argv[++i];
	         }
	      }
	   }
	   else
	      cmd = NULL;
	   if (!retrieve_help(cmd, info_command_table, cli_display_buf_ptr,
                              &buf_index, INFO_CMD) || argc == 1 || all_flag)
#ifdef DIAGTEST
	      if (!retrieve_help(cmd, diag_command_table, cli_display_buf_ptr,
                                 &buf_index, EXEC_CMD) || argc == 1 || all_flag)
#endif /* DIAGTEST */
#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
	         if (!retrieve_help(cmd, debug_command_table,
                      cli_display_buf_ptr, &buf_index, DEBUG_CMD) || 
	              argc == 1 || all_flag)
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */
#ifdef LOADGEN
	            if (!retrieve_help(cmd, loadgen_command_table,
                       cli_display_buf_ptr,&buf_index, LOADGEN_CMD) || 
	               argc == 1 || all_flag)
#endif /* LOADGEN */
	               if (!retrieve_help(cmd, misc_command_table,
                                     cli_display_buf_ptr, &buf_index, MISC_CMD))
	                  return(SYNTAX_COMMAND_NOT_FOUND_ERROR);
	   if (argc == 1 || all_flag)
	      break;
	}

	if (cmd == NULL || all_flag) {
	   if (sys_help_sort) {
	      for (i = 0; i < buf_index; i++) {
	         k = 0;
	         while (cli_display_buf_ptr[k][0] == '\0') k++;
	         for (j = k; j < buf_index; j++) {
	            if (cli_display_buf_ptr[j][0] != '\0') {
	               if (*((uInt32 *) cli_display_buf_ptr[k]) >
	                   *((uInt32 *) cli_display_buf_ptr[j])) {
	                  k = j;
	               }
	               if (*((uInt32 *) cli_display_buf_ptr[k]) ==
	                   *((uInt32 *) cli_display_buf_ptr[j])) {
	                  if (*((uInt32 *) &cli_display_buf_ptr[k][4]) >
	                      *((uInt32 *) &cli_display_buf_ptr[j][4])) {
	                     k = j;
	                  }
	               }
	               if (all_flag)
	                  while (cli_display_buf_ptr[j][0] != '-') j++;
	            }
	         }
	         strcpy(cli_display_buf_ptr[buf_index+i], 
	                cli_display_buf_ptr[k]);
	         cli_display_buf_ptr[k++][0] = '\0';
	         if (all_flag) {
	            while (cli_display_buf_ptr[k][0] != '-') {
	               strcpy(cli_display_buf_ptr[buf_index+(++i)],
	                      cli_display_buf_ptr[k]);
	               cli_display_buf_ptr[k++][0] = '\0';
	            }
	            strcpy(cli_display_buf_ptr[buf_index+(++i)],
	                   cli_display_buf_ptr[k]);
	            cli_display_buf_ptr[k++][0] = '\0';
	         }
	      }
	      for (i = 0; i < buf_index; i++)
	         strcpy(cli_display_buf_ptr[i], 
	                cli_display_buf_ptr[buf_index+i]);
	   }
	}

	print_buf(cli_display_buf_ptr, buf_index);

	return(rc = 0);
}

/*----------------------------------------------------------------------
 * History()
 *----------------------------------------------------------------------*/
int History(
	int		argc,
	char		*argv[])
{
	register int	history_number, history_number_min, history_number_max;
	register int	history_index, buf_index;
	register int	rc;
	register char	*argp;
	uInt32		num;

	if (argc > 1) {
	   argp = argv[1];
	   if (*argp++ != '-')
	      return(SYNTAX_INVALID_ARGUMENT_DELIMITER_ERROR);
	   if (*argp++ != ARGUMENT_OPTION_NUMBER_OF_COMMANDS)
	      return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
	   if (*argp == '\0') {
	      if (argc == 2)
	         return(SYNTAX_NO_OF_COMMANDS_NOT_SPECIFIED_ERROR);
	      if (argc > 3)
	         return(SYNTAX_TOO_MANY_ARGUMENTS_ERROR);
	      argp = argv[2];
	   }
	   else
	      if (argc > 2)
	         return(SYNTAX_TOO_MANY_ARGUMENTS_ERROR);
	   if (!sscanf_x(argp, fmt_uInt32, &num))
	      return(SYNTAX_INVALID_NUMBER_OF_COMMANDS_ERROR);
	   if (num < 1 || num > HISTORY_BUF_SIZE)
	      return(SYNTAX_NUMBER_OF_COMMANDS_OUT_OF_RANGE_ERROR);
	}
	else
	   num = HISTORY_BUF_SIZE;

	if (cli_history_number <= HISTORY_BUF_SIZE+1) {
	   if (num >= cli_history_number)
	      num = cli_history_number - 1;
	}
	history_number_min = cli_history_number - num;
	history_number_max = cli_history_number - 1;

	for (history_number = history_number_min, buf_index = 0;
             history_number <= history_number_max; 
	     history_number++, buf_index++) {
	   history_index = history_number%HISTORY_BUF_SIZE - 1;
	   if (history_index < 0)
	      history_index = HISTORY_BUF_SIZE - 1;
	   sprintf(cli_display_buf_ptr[buf_index], "%4u %s\n", history_number,
		   cli_history_buf[history_index]);
	}

	print_buf(cli_display_buf_ptr, buf_index);

	return(rc = 0);
}

/*----------------------------------------------------------------------
 * Keys()
 *----------------------------------------------------------------------*/
int Keys(
	int		argc,
	char		*argv[])
{
	register int	rc, i;
	register int	buf_index;

	for (buf_index = i = 0; keys_info[i][0] != '\0'; i++, buf_index++) {
	   sprintf(cli_display_buf_ptr[buf_index], " %s\n", keys_info[i]);
	}

	print_buf(cli_display_buf_ptr, buf_index);

	return(rc = 0);
}

/*----------------------------------------------------------------------
 * Clear()
 *----------------------------------------------------------------------*/
int Clear(
	int		argc,
	char		*argv[])
{
	register int	rc;

	if (argc > 1)
	   return(SYNTAX_UNEXPECTED_ARGUMENT_ERROR);

	if (sys_term_vt100)
	   set_cli_screen();

	return(rc = 0);
}

/*----------------------------------------------------------------------
 * Exit()
 *----------------------------------------------------------------------*/
int Exit(
	int		argc,
	char		*argv[])
{
	register int	rc;

	if (argc > 1)
	   return(SYNTAX_UNEXPECTED_ARGUMENT_ERROR);

	return(rc = CLI_EXIT);
}

/*----------------------------------------------------------------------
 * retrieve_help()
 *----------------------------------------------------------------------*/
int retrieve_help(
	char			*cmd,
	struct COMMAND_TABLE	*cmd_table,
	char			*buf_ptr[],
	int			*buf_index,
	enum COMMAND_TYPE	command_type)
{
	char			**syntax;
	register int		index, all_flag;

	if (!strcmp_nocase(cmd, "all"))
	   all_flag = TRUE;
	else
	   all_flag = FALSE;

	index = *buf_index;

	if (!sys_help_sort) {
	   if (cmd == NULL || all_flag) {
	      sprintf(buf_ptr[(*buf_index)++], "%s\n", 
	              cli_command_type[command_type]);
	   }
	}

	while (cmd_table->command != NULL) {
	   if (cmd_table->display_control == ON) {
	      if (cmd == NULL || all_flag || !strncmp_v(cmd, cmd_table->command,
                                                        cmd_table->abbrev)) {
	         sprintf(buf_ptr[(*buf_index)++], " %s\n", 
	                 cmd_table->description);
	         if (cmd != NULL || all_flag) {
		    for (syntax = cmd_table->syntax; *syntax != NULL; syntax++)
		       sprintf(buf_ptr[(*buf_index)++], " %s\n", *syntax);
	         }
	         if (cmd != NULL || all_flag)
	            strcpy(buf_ptr[(*buf_index)++], "-");
	      }
	   }
	   cmd_table++;
	}

	if (!sys_help_sort) {
	   if (cmd == NULL || all_flag) {
	      if (index != *buf_index) {
	         if (command_type != MISC_CMD) {
	            if (all_flag)
	               (*buf_index)--;
	            strcpy(buf_ptr[(*buf_index)++], "=");
	         }
	      }
	   }
	}

	return(index != *buf_index);
}

/*----------------------------------------------------------------------
 * syntax_error_handler()
 *----------------------------------------------------------------------*/
void syntax_error_handler(
        char			**syntax,
        enum CLI_RETURN_CODE 	error_code)
{
        register char           *error_message;

        switch(error_code) {
           case CLI_INVALID_COMMAND:
              error_message = cli_invalid_command;
              break;
           case CLI_COMMAND_NOT_IN_HISTORY:
              error_message = cli_command_not_in_history;
              break;
           case SYNTAX_UNEXPECTED_ARGUMENT_ERROR:
              error_message = Syntax_unexpected_argument_error;
              break;
           case SYNTAX_MISSING_ARGUMENTS_ERROR:
              error_message = Syntax_missing_arguments_error;
              break;
           case SYNTAX_TOO_FEW_ARGUMENTS_ERROR:
              error_message = Syntax_too_few_arguments_error;
              break;
           case SYNTAX_TOO_MANY_ARGUMENTS_ERROR:
              error_message = Syntax_too_many_arguments_error;
              break;
           case SYNTAX_INVALID_ARGUMENT_DELIMITER_ERROR:
              error_message = Syntax_invalid_argument_delimiter_error;
              break;
           case SYNTAX_INVALID_ARGUMENT_OPTION_ERROR:
              error_message = Syntax_invalid_argument_option_error;
              break;
#if defined(DEBUGFUNC) || defined(DEBUGFUNC_P)
           case SYNTAX_START_ADDRESS_NOT_SPECIFIED_ERROR:
              error_message = Syntax_start_address_not_specified_error;
              break;
           case SYNTAX_INVALID_START_ADDRESS_ERROR:
              error_message = Syntax_invalid_start_address_error;
              break;
           case SYNTAX_MORE_THAN_ONE_START_ADDRESS_SPECIFIED_ERROR:
              error_message = Syntax_more_than_one_start_address_specified_error;
              break;
           case SYNTAX_END_ADDRESS_NOT_SPECIFIED_ERROR:
              error_message = Syntax_end_address_not_specified_error;
              break;
           case SYNTAX_INVALID_END_ADDRESS_ERROR:
              error_message = Syntax_invalid_end_address_error;
              break;
           case SYNTAX_MORE_THAN_ONE_END_ADDRESS_SPECIFIED_ERROR:
              error_message = Syntax_more_than_one_end_address_specified_error;
              break;
           case SYNTAX_LENGTH_NOT_SPECIFIED_ERROR:
              error_message = Syntax_length_not_specified_error;
              break;
           case SYNTAX_INVALID_LENGTH_ERROR:
              error_message = Syntax_invalid_length_error;
              break;
           case SYNTAX_MORE_THAN_ONE_LENGTH_SPECIFIED_ERROR:
              error_message = Syntax_more_than_one_length_specified_error;
              break;
           case SYNTAX_BOTH_END_ADDRESS_AND_LENGTH_SPECIFIED_ERROR:
              error_message =Syntax_both_end_address_and_length_specified_error;
              break;
           case SYNTAX_DATA_NOT_SPECIFIED_ERROR:
              error_message = Syntax_data_not_specified_error;
              break;
           case SYNTAX_INVALID_DATA_ERROR:
              error_message = Syntax_invalid_data_error;
              break;
           case SYNTAX_MORE_THAN_ONE_DATA_SPECIFIED_ERROR:
              error_message = Syntax_more_than_one_data_specified_error;
              break;
           case SYNTAX_ACCESS_NOT_SPECIFIED_ERROR:
              error_message = Syntax_access_not_specified_error;
              break;
           case SYNTAX_INVALID_ACCESS_ERROR:
              error_message = Syntax_invalid_access_error;
              break;
           case SYNTAX_MORE_THAN_ONE_ACCESS_SPECIFIED_ERROR:
              error_message = Syntax_more_than_one_access_specified_error;
              break;
           case SYNTAX_MODE_NOT_SPECIFIED_ERROR:
              error_message = Syntax_mode_not_specified_error;
              break;
           case SYNTAX_INVALID_MODE_ERROR:
              error_message = Syntax_invalid_mode_error;
              break;
           case SYNTAX_MORE_THAN_ONE_MODE_SPECIFIED_ERROR:
              error_message = Syntax_more_than_one_mode_specified_error;
              break;
           case SYNTAX_INCREMENT_NOT_SPECIFIED_ERROR:
              error_message = Syntax_increment_not_specified_error;
              break;
           case SYNTAX_INVALID_INCREMENT_ERROR:
              error_message = Syntax_invalid_increment_error;
              break;
           case SYNTAX_MORE_THAN_ONE_INCREMENT_SPECIFIED_ERROR:
              error_message = Syntax_more_than_one_increment_specified_error;
              break;
#ifdef DEBUGFUNC
           case SYNTAX_INSTRUCTION_NOT_SPECIFIED_ERROR:
              error_message = Syntax_instruction_not_specified_error;
              break;
           case SYNTAX_UNEXPECTED_OPERAND_ERROR:
              error_message = Syntax_unexpected_operand_error;
              break;
           case SYNTAX_MISSING_OPERAND_ERROR:
              error_message = Syntax_missing_operand_error;
              break;
           case SYNTAX_INVALID_OPERAND_ERROR:
              error_message = Syntax_invalid_operand_error;
              break;
           case SYNTAX_OPERAND_OUT_OF_RANGE_ERROR:
              error_message = Syntax_operand_out_of_range_error;
              break;
           case SYNTAX_INVALID_INSTRUCTION_ERROR:
              error_message = Syntax_invalid_instruction_error;
              break;
           case SYNTAX_INVALID_INSTRUCTION_FORMAT_ERROR:
              error_message = Syntax_invalid_instruction_format_error;
              break;
           case SYNTAX_INSTRUCTION_NOT_IMPLEMENTED_ERROR:
              error_message = Syntax_instruction_not_implemented_error;
              break;
           case SYNTAX_MORE_THAN_ONE_INSTRUCTION_SPECIFIED_ERROR:
              error_message = Syntax_more_than_one_instruction_specified_error;
              break;
#endif /* DEBUGFUNC */
           case SYNTAX_NUMBER_OF_TIMES_NOT_SPECIFIED_ERROR:
              error_message = Syntax_number_of_times_not_specified_error;
              break;
           case SYNTAX_INVALID_NUMBER_OF_TIMES_ERROR:
              error_message = Syntax_invalid_number_of_times_error;
              break;
           case SYNTAX_MORE_THAN_ONE_NUMBER_OF_TIMES_SPECIFIED_ERROR:
              error_message = Syntax_more_than_one_number_of_times_specified_error;
              break;
           case SYNTAX_TIME_DELAY_NOT_SPECIFIED_ERROR:
              error_message = Syntax_time_delay_not_specified_error;
              break;
           case SYNTAX_INVALID_TIME_DELAY_ERROR:
              error_message = Syntax_invalid_time_delay_error;
              break;
           case SYNTAX_MORE_THAN_ONE_TIME_DELAY_SPECIFIED_ERROR:
              error_message = Syntax_more_than_one_time_delay_specified_error;
              break;
           case SYNTAX_VERBOSITY_NOT_SPECIFIED_ERROR:
              error_message = Syntax_verbosity_not_specified_error;
              break;
           case SYNTAX_INVALID_VERBOSITY_ERROR:
              error_message = Syntax_invalid_verbosity_error;
              break;
           case SYNTAX_MORE_THAN_ONE_VERBOSITY_SPECIFIED_ERROR:
              error_message = Syntax_more_than_one_verbosity_specified_error;
              break;
           case SYNTAX_VERBOSITY_OUT_OF_RANGE_ERROR:
              error_message = Syntax_verbosity_out_of_range_error;
              break;
           case SYNTAX_WRITE_ADDRESS_NOT_SPECIFIED_ERROR:
              error_message = Syntax_write_address_not_specified_error;
              break;
           case SYNTAX_INVALID_WRITE_ADDRESS_ERROR:
              error_message = Syntax_invalid_write_address_error;
              break;
           case SYNTAX_MORE_THAN_ONE_WRITE_ADDRESS_SPECIFIED_ERROR:
              error_message = Syntax_more_than_one_write_address_specified_error;
              break;
           case SYNTAX_READ_ADDRESS_NOT_SPECIFIED_ERROR:
              error_message = Syntax_read_address_not_specified_error;
              break;
           case SYNTAX_INVALID_READ_ADDRESS_ERROR:
              error_message = Syntax_invalid_read_address_error;
              break;
           case SYNTAX_MORE_THAN_ONE_READ_ADDRESS_SPECIFIED_ERROR:
              error_message = Syntax_more_than_one_read_address_specified_error;
              break;
           case SYNTAX_COMPARE_DATA_NOT_SPECIFIED_ERROR:
              error_message = Syntax_read_address_not_specified_error;
              break;
           case SYNTAX_INVALID_COMPARE_DATA_ERROR:
              error_message = Syntax_invalid_compare_data_error;
              break;
           case SYNTAX_MORE_THAN_ONE_COMPARE_DATA_SPECIFIED_ERROR:
              error_message = Syntax_more_than_one_compare_data_specified_error;
              break;
#endif /* defined(DEBUGFUNC) || defined(DEBUGFUNC_P) */
           case SYNTAX_COMMAND_NOT_SPECIFIED_ERROR:
              error_message = Syntax_command_not_specified_error;
              break;
           case SYNTAX_COMMAND_NOT_FOUND_ERROR:
              error_message = Syntax_command_not_found_error;
              break;
           case SYNTAX_NO_OF_COMMANDS_NOT_SPECIFIED_ERROR:
              error_message = Syntax_no_of_commands_not_specified_error;
              break;
           case SYNTAX_INVALID_NUMBER_OF_COMMANDS_ERROR:
              error_message = Syntax_invalid_number_of_commands_error;
              break;
           case SYNTAX_NUMBER_OF_COMMANDS_OUT_OF_RANGE_ERROR:
              error_message = Syntax_number_of_commands_out_of_range_error;
              break;
           case SYNTAX_SECTION_ID_NOT_SPECIFIED_ERROR:
              error_message = Syntax_section_id_not_specified_error;
              break;
           case SYNTAX_SECTION_ID_NOT_FOUND_ERROR:
              error_message = Syntax_section_id_not_found_error;
              break;
           case SYNTAX_MORE_THAN_ONE_SECTION_ID_SPECIFIED_ERROR:
              error_message = Syntax_more_than_one_section_id_specified_error;
              break;
           case SYNTAX_TEST_ID_NOT_SPECIFIED_ERROR:
              error_message = Syntax_test_id_not_specified_error;
              break;
           case SYNTAX_TEST_ID_NOT_FOUND_ERROR:
              error_message = Syntax_test_id_not_found_error;
              break;
           case SYNTAX_MORE_THAN_ONE_TEST_ID_SPECIFIED_ERROR:
              error_message = Syntax_more_than_one_test_id_specified_error;
              break;
           case SYNTAX_BOTH_SECTION_ID_AND_TEST_ID_SPECIFIED_ERROR:
              error_message =Syntax_both_section_id_and_test_id_specified_error;
              break;
           case SYNTAX_PARAMETER_NOT_SPECIFIED_ERROR:
              error_message = Syntax_parameter_not_specified_error;
              break;
           case SYNTAX_PARAMETER_NOT_FOUND_ERROR:
              error_message = Syntax_parameter_not_found_error;
              break;
           case SYNTAX_PARAMETER_VALUE_NOT_SPECIFIED_ERROR:
              error_message = Syntax_parameter_value_not_specified_error;
              break;
           case SYNTAX_INVALID_PARAMETER_VALUE_ERROR:
              error_message = Syntax_invalid_parameter_value_error;
              break;
           case SYNTAX_INVALID_PARAMETER_VALUE_DELIMITER_ERROR:
              error_message = Syntax_invalid_parameter_value_delimiter_error;
              break;
           case SYNTAX_PARAMETER_VALUE_OUT_OF_RANGE_ERROR:
              error_message = Syntax_parameter_value_out_of_range_error;
              break;
           case SYNTAX_PARAMETER_LENGTH_OUT_OF_RANGE_ERROR:
              error_message = Syntax_parameter_length_out_of_range_error;
              break;
#ifndef RISC
           case SYNTAX_INVALID_DATE_SPECIFIED_ERROR:
              error_message = Syntax_invalid_date_specified_error;
              break;
           case SYNTAX_INVALID_TIME_SPECIFIED_ERROR:
              error_message = Syntax_invalid_time_specified_error;
              break;
#endif /* RISC */
#ifdef CALYPSO
           case SYNTAX_PROMPT_STRING_TOO_LARGE_ERROR:
              error_message = Syntax_prompt_string_too_large_error;
              break;
           case SYNTAX_ERROR_CODE_NOT_SPECIFIED_ERROR:
              error_message = Syntax_error_code_not_specified_error;
              break;
           case SYNTAX_INVALID_ERROR_CODE_ERROR:
              error_message = Syntax_invalid_error_code_error;
              break;
           case SYNTAX_ERROR_CODE_OUT_OF_RANGE_ERROR:
              error_message = Syntax_error_code_out_of_range_error;
              break;
#ifndef BOOT
           case SYNTAX_INVALID_SOURCE_MAC_SPECIFIED_ERROR:
              error_message = Syntax_invalid_source_mac_specified_error;
              break;
           case SYNTAX_INVALID_DESTINATION_MAC_SPECIFIED_ERROR:
              error_message = Syntax_invalid_destination_mac_specified_error;
              break;
#endif /* BOOT */
#endif /* CALYPSO */
        }

        sprintf(cli_print_buf, " *** %s ***\n", error_message);
	put_line(cli_print_buf);

	while (*syntax != NULL) {
	   sprintf(cli_print_buf, " %s\n", *syntax++);
	   put_line(cli_print_buf);
	}
}
