/* $Id: diag.c,v 1.1.4.1 1996/05/09 15:00:56 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/diag.c,v $
 *------------------------------------------------------------------
 * diag.c -- procedures related to "Diagnostic Commands" included
 *           as part of Diagnostics Driver.
 *
 * 1995, Randy Bau
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: diag.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:56  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:54  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:16  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:05  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:48  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:23  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.2.4.1.4.1  1996/01/18  05:02:16  rlowe
 * Manufacturability changes to bootrom and flash boot code.
 *
 * Branch: ATM21_branch
 *
 * Revision 1.2.4.1  1995/11/16  00:07:36  rbau
 * merge mainline code changes in 1.4 branch
 *
 * Revision 1.3  1995/11/08  00:36:36  rbau
 * added commands to show sars, pmc stats. complet the cvs header
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include	"ctype.h"
#include	"types.h"
#include	"constant.h"
#include	"globdef.h"
#include	"struct.h"

/*---< Prototypes >---*/
extern  void  show_pmdrom(void);
extern  int   set_pmdrom();
extern  void  show_pmc_stats(void);
extern  void  show_stats(void);
extern  int   LpbkSetup ();
extern  int   setup_dvt ();

#define  ATM

	enum		ARGUMENT_OPTION {
	ARGUMENT_OPTION_SECTION_ID		= 's',
	ARGUMENT_OPTION_TEST_ID			= 't',
	ARGUMENT_OPTION_PARAMETER		= 'p'
	};

extern	char			*cli_test_buf_ptr[];
extern	char			*cli_display_buf_ptr[];
extern	char			cli_print_buf[];
#ifdef CALYPSO
extern	char			cli_prompt_line[];
#endif /* CALYPSO */
extern	struct SECTION_TOC	diag_toc[];

extern	char			sys_date[];
extern	char			sys_time[];
extern	char			sys_specific_info[];
extern	Int32			sys_error_code;
extern	uInt32			sys_fail_address;
extern	uInt32			sys_data_expected;
extern	uInt32			sys_data_read;
extern	volatile uInt32		system_tick_time;
extern	int			sys_cmd_no_flag;
extern	int			sys_more_prompt;

	char		test_progress_msg[] = "Running %s Test {%s} -> %lu\n";
	char		test_successful_msg[] = 
	"%s Test {%s} Completed Successfully -> %lu\n";
	char		test_fail_msg[] = 
	"%s Test {%s} Failed -> %lu\n";
	char		test_warning_msg[] = 
	"%s Test {%s} Completed after bypassing errors -> %lu\n";

	char		TEST_FAIL_MSG[] = " *** FAILURE IN %s TEST {%s} *** Last Error Code %ld\n";
	char		TEST_WARNING_MSG[] = 
	" *** ERRORS BYPASSED IN %s TEST {%s} *** Last Error Code %ld\n";

	uInt32		diag_runcount = 1;
	uInt32		diag_maxerrorcount = 0;
	uInt32		diag_timedelaycount = 0;
	uInt32		diag_verbosity = VERBOSITY_LEVEL3;

	uInt32		diag_current_runcount;
	uInt32		diag_test_errorcount;
	uInt32		diag_total_errorcount;
	uInt32		diag_total_errorcount_x;
	uInt32		diag_sect_maxerrorcount;
	uInt32		diag_fail_address;
	uInt32		diag_data_expected;
	uInt32		diag_data_read;
	int		diag_test_mode;
#ifdef CALYPSO
	int		diag_virtual_uart_mode;
#endif /* CALYPSO */

	enum DIAG_ENVIRONMENT	diag_environment = POWER_UP;

	char		diag_allprompt[6] = "False";
	char		diag_npbusarbitration[6] = "True";
	char		diag_fbbusarbitration[6] = "True";
	char		diag_packetsizevariant[6] = "True";
	char		diag_datapatternvariant[6] = "True";
	char		diag_summaryformat[5] = "LIFO";
	char		*diag_test_name;
	char		*diag_test_id;

	char		diag_description[] =
	"General Statistical Results for the last Run Command";
	uInt32		diag_start_time_msec;
	uInt32		diag_end_time_msec;
	uInt32		diag_elapsed_time_msec;
	char		diag_start_time[18];
	char		diag_end_time[18];
	char		diag_elapsed_time[18];

	struct PARAMETER_TABLE	diag_parameter[] =
	{ { (char *) &diag_runcount, parm_runcount, fmt_uInt32,
	    {1}, {1}, {99999999}, NULL, 0 },
	  { (char *) &diag_maxerrorcount, parm_maxerrorcount, fmt_uInt32,
	    {0}, {0}, {99999999}, NULL, 0 },
	  { (char *) &diag_timedelaycount, parm_timedelaycount, fmt_uInt32,
	    {0}, {0}, {99999999}, NULL, 0 },
	  { (char *) &diag_verbosity, parm_verbosity, fmt_uInt32,
	    {VERBOSITY_LEVEL3}, {VERBOSITY_LEVEL0}, {VERBOSITY_LEVEL3},NULL,0 },
	  { (char *)  diag_npbusarbitration, parm_npbusarbitration, fmt_string,
	    {(uInt32) value_flag_true}, {0}, {0}, value_flag, 0 },
	  { (char *)  diag_fbbusarbitration, parm_fbbusarbitration, fmt_string,
	    {(uInt32) value_flag_true}, {0}, {0}, value_flag, 0 },
	  { (char *)  diag_packetsizevariant, parm_packetsizevariant,
	    fmt_string,
	    {(uInt32) value_flag_true}, {0}, {0}, value_flag, 0 },
	  { (char *)  diag_datapatternvariant, parm_datapatternvariant,
	    fmt_string,
	    {(uInt32) value_flag_true}, {0}, {0}, value_flag, 0 },
	  { (char *)  diag_allprompt, parm_allprompt, fmt_string,
	    {(uInt32) value_flag_false}, {0}, {0}, value_flag, 0 },
	  { (char *)  diag_summaryformat, parm_summaryformat, fmt_string,
	    {(uInt32) value_summaryformat_LIFO}, {0}, {0}, 
	    value_summaryformat, 0 },
	  { NULL, NULL, NULL, {0}, {0}, {0}, NULL, 0 } };

	struct RESULTS_TABLE	diag_results[] =
	{ { (char *)  diag_description, result_diag_description, fmt_string,
	    OFF },
	  { (char *)  diag_start_time, result_start_time, fmt_string, OFF },
	  { (char *)  diag_end_time, result_end_time, fmt_string, OFF },
	  { (char *)  diag_elapsed_time, result_elapsed_time, fmt_string, OFF },
	  { (char *) &diag_elapsed_time_msec, result_elapsed_time_msec, "%17u",
            OFF },
	  { (char *) &diag_total_errorcount_x, result_total_errorcount, "%17u", 
	    OFF },
	  { NULL, NULL, NULL, OFF } };

	struct SECTION_TOC *retrieve_section();
	struct TEST_TOC    *retrieve_test();
	void		    initialize_test();
	void		    log_test_status();
	void		    display_test_progress();
	void		    display_test_failure();
	void		    display_test_warning();
	int retrieve_contents(
		char			*sect,
		struct SECTION_TOC	*sect_toc,
		char			*buf_ptr[],
		int			*buf_index,
		char			cmd);

int Contents(
	int		argc,
	char		*argv[])
{
	register char	*sect;
	register int	rc, i;
	int		buf_index;

	for (buf_index = 0, i = 1; i < argc || argc == 1; i++) {
	   if (argc > 1) {
	      sect = argv[i];
	      if (*sect++ != '-')
	         return(SYNTAX_INVALID_ARGUMENT_DELIMITER_ERROR);
	      if (*sect++ != ARGUMENT_OPTION_SECTION_ID)
	         return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
	      if (*sect == '\0') {
	         if (i == (argc-1))
	            return(SYNTAX_SECTION_ID_NOT_SPECIFIED_ERROR);
	         sect = argv[++i];
	      }
	   }
	   else
	      sect = NULL;
	   if (rc = retrieve_contents(sect, diag_toc, cli_display_buf_ptr,
                                      &buf_index, '\0'))
	      return(rc);
	   strcpy(cli_display_buf_ptr[buf_index++], "-");
	   if (argc == 1)
	      break;
	}

	print_buf(cli_display_buf_ptr, buf_index);

	return(rc = 0);
}

/*-------------------------------------------------------------------*/
int Set(
	int		argc,
	char		*argv[])
{
	struct SECTION_TOC	*sect_toc;
	struct TEST_TOC		*test_toc;
	struct PARAMETER_TABLE	*parm_table;
	register char		*sect, *test, *argp, *parm, *value;
	register int		i, j, rc, parm_flag;
	char			parm_buf[80];
	int			buf_index;
#ifndef  ATM
#ifdef CALYPSO
#ifndef RISC
#ifndef BOOT
	char		passwd1[80], passwd2[80];
#endif /* BOOT */
#endif /* RISC */
#endif /* CALYPSO */

	if (argc == 1)
	   return(SYNTAX_MISSING_ARGUMENTS_ERROR);

#ifdef CALYPSO
#ifndef BOOT
#ifndef RISC
	if (argc == 2 && !strcmp_nocase(argv[1], "password")) {
	   put_line("Enter new password: ");
	   get_pline(passwd1, sizeof(passwd1));
	   put_line("Re-enter new password: ");
	   get_pline(passwd2, sizeof(passwd2));

	   if (!strcmp(passwd1, passwd2)) {
	      if (validCresMIB())
	         put_line(" Set Password Successful ...\n");
	      else {
	         passwd1[0] = '\0';
	         put_line(" NVRAM not Initialized, password set to null ...\n");
	      }
	      setPasswd(passwd1);
	   }
	   else
	      put_line(" Set Password Failed, re-entry mismatch ...\n");

	   return(rc = 0);
	}
#endif /* RISC */
	if (argc == 3 && !strcmp_nocase(argv[1], "smac")) {
	   if (!valid_source_mac(argv[2]))
	      return(SYNTAX_INVALID_SOURCE_MAC_SPECIFIED_ERROR);
	   return(rc = 0);
	}

	if (argc == 3 && !strcmp_nocase(argv[1], "dmac")) {
	   if (!valid_dest_mac(argv[2]))
	      return(SYNTAX_INVALID_DESTINATION_MAC_SPECIFIED_ERROR);
	   return(rc = 0);
	}

	if ((argc == 2 || argc == 3) && !strcmp_nocase(argv[1], "pause")) {
	   if (argc == 3 && !strcmp_nocase(argv[2], "off"))
	      sys_more_prompt = FALSE;
	   else
	      sys_more_prompt = TRUE;
	   return(rc = 0);
	}
#endif /* BOOT */
#endif /* CALYPSO */
#endif  /* ATM */

#ifndef RISC
	if (argc == 3 && !strcmp_nocase(argv[1], "date")) {
	   if (!valid_date(argv[2]))
	      return(SYNTAX_INVALID_DATE_SPECIFIED_ERROR);
	   return(rc = 0);
	}

	if (argc == 3 && !strcmp_nocase(argv[1], "time")) {
	   if (!valid_time(argv[2]))
	      return(SYNTAX_INVALID_TIME_SPECIFIED_ERROR);
	   return(rc = 0);
	}

	if (argc == 2 && !strcmp_nocase(argv[1], "pmd"))
	{
	   set_pmdrom();
	   return(rc = 0);
	}

	if (argc == 2 && !strcmp_nocase(argv[1], "loopback"))
	{
       LpbkSetup();
       return(rc = 0);
	}

	if (argc == 2 && !strcmp_nocase(argv[1], "dvt"))
	{
       setup_dvt();
       return(rc = 0);
	}

#endif /* RISC */

#ifdef CALYPSO
	if (argc == 3 && !strcmp_nocase(argv[1], "prompt")) {
	   if (strlen(argv[2]) > PROMPT_LINE_SIZE)
	      return(SYNTAX_PROMPT_STRING_TOO_LARGE_ERROR);
	   if (strlen(argv[2]) == PROMPT_LINE_SIZE && 
	       argv[2][PROMPT_LINE_SIZE-1] != '$')
	      return(SYNTAX_PROMPT_STRING_TOO_LARGE_ERROR);
	   strcpy(cli_prompt_line, argv[2]);
	   for (i = 0; cli_prompt_line[i] != '\0'; i++);
	   if (cli_prompt_line[i-1] == '$') {
	      cli_prompt_line[i-1] = '\0';
	      sys_cmd_no_flag = TRUE;
	   }
	   else
	      sys_cmd_no_flag = FALSE;
	   return(rc = 0);
	}
#endif /* CALYPSO */

	sect = test = NULL;

	buf_index = 0;
	if (argc == 2 && !strcmp(argv[1], "?"))
	   retrieve_contents(NULL, diag_toc, cli_display_buf_ptr, &buf_index, 
                             '\0');
	else {
	   for (i = 1; i < argc; i++) {
	      argp = argv[i];
	      if (*argp == '-' && *(argp+1) == 's') {
	         if (sect != NULL)
	            return(SYNTAX_MORE_THAN_ONE_SECTION_ID_SPECIFIED_ERROR);
	         argp += 2;
		 if (*argp == '\0') {
	            if (i == (argc-1))
		       return(SYNTAX_SECTION_ID_NOT_SPECIFIED_ERROR);
	            argp = argv[++i];
	         }
		 sect = argp;
	      }  
	      else {
	         if (*argp == '-' && *(argp+1) == 't') {
	            if (test != NULL)
		       return(SYNTAX_MORE_THAN_ONE_TEST_ID_SPECIFIED_ERROR);
	            argp += 2;
		    if (*argp == '\0') {
	               if (i == (argc-1))
		          return(SYNTAX_TEST_ID_NOT_SPECIFIED_ERROR);
	               argp = argv[++i];
	            }
		    test = argp;
		 }
	      }
	   }
	   if (sect != NULL && test != NULL)
	      return(SYNTAX_BOTH_SECTION_ID_AND_TEST_ID_SPECIFIED_ERROR);
	   if (sect != NULL) {
	      if ((sect_toc = retrieve_section(sect, diag_toc)) ==
	                                               (struct SECTION_TOC *) 0)
	         return(SYNTAX_SECTION_ID_NOT_FOUND_ERROR);
	      parm_table = sect_toc->parameter_table;
	   }
	   else {
	      if (test != NULL) {
	         if ((test_toc = retrieve_test(test, diag_toc)) ==
	                                                  (struct TEST_TOC *) 0)
		    return(SYNTAX_TEST_ID_NOT_FOUND_ERROR);
	         parm_table = test_toc->parameter_table;
	      }
	      else
	         parm_table = diag_parameter;
	   }
	   for (i = 1, parm_flag = FALSE; i < argc; i++) {
	      parm = argv[i];
	      if (*parm++ != '-')
	         return(SYNTAX_INVALID_ARGUMENT_DELIMITER_ERROR);
	      switch (*parm++) {
	         case ARGUMENT_OPTION_SECTION_ID:
		    if (*parm == '\0')
	               i++;
	            break;
	         case ARGUMENT_OPTION_TEST_ID:
		    if (*parm == '\0')
	               i++;
		    break;
		 case ARGUMENT_OPTION_PARAMETER:
	            if (*parm == '\0') {
	               if (i == (argc-1))
			  return(SYNTAX_PARAMETER_NOT_SPECIFIED_ERROR);
	               parm = argv[++i];
	            }
	            for (value = parm, j = 0; *value != '='&&*value != '\0'; )
	               parm_buf[j++] = *value++;
	            parm_buf[j] = '\0';
                    if (*value == '\0') {
	               if (i == (argc-1))
			  return(SYNTAX_PARAMETER_VALUE_NOT_SPECIFIED_ERROR);
	               value = argv[++i];
	            }
	            if (*value++ != '=')
	               return(SYNTAX_INVALID_PARAMETER_VALUE_DELIMITER_ERROR);
                    if (*value == '\0') {
	               if (i == (argc-1))
			  return(SYNTAX_PARAMETER_VALUE_NOT_SPECIFIED_ERROR);
	               value = argv[++i];
	            }
	            if (rc = set_parameter(parm_buf, parm_table, value))
		       return(rc);
	            parm_flag = TRUE;
		    break;
	         default:
	            return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
	      }
	   }
	   if (parm_flag == FALSE)
	      return(SYNTAX_PARAMETER_NOT_SPECIFIED_ERROR);
	}

	print_buf(cli_display_buf_ptr, buf_index);

	return(rc = 0);
}

int Reset(
	int		argc,
	char		*argv[])
{
	struct SECTION_TOC	*sect_toc;
	struct TEST_TOC		*test_toc;
	struct PARAMETER_TABLE	*parm_table;
	register char		*sect, *test, *argp, *parm;
	register int		i, rc, all_flag, parm_flag;
	int			buf_index;

	all_flag = !strcmp_nocase(argv[1], "all");

#ifdef CALYPSO
	if (argc == 2 && (!strcmp_nocase(argv[1], "prompt") || all_flag)) {
#ifdef RISC
	   strcpy(cli_prompt_line, "SAR>");
#else /* RISC */
	   strcpy(cli_prompt_line, "ACP>");
#endif /* RISC */
	   sys_cmd_no_flag = TRUE;
	   if (!all_flag)
	      return(rc = 0);
	}
#endif /* CALYPSO */

#ifdef CALYPSO
#ifndef RISC
	if (argc == 2 && !strcmp_nocase(argv[1], "acp"))
	   return(reset_nmp());
	if (argc == 2 && !strcmp_nocase(argv[1], "sar"))
	   return(reset_dmp());
#endif /* RISC */
#ifndef BOOT
	if (argc == 2 && !strcmp_nocase(argv[1], "smac")) {
	   return(reset_source_mac());
	}
	if (argc == 2 && !strcmp_nocase(argv[1], "dmac")) {
	   return(reset_dest_mac());
	}
#endif /* BOOT */
#endif /* CALYPSO */

	sect = test = NULL;

	buf_index = 0;
	if (argc == 1) {
	   reset_parameter(NULL, diag_parameter);
	}
	else {
	   if (argc == 2 && (!strcmp(argv[1], "?") || all_flag)) {
	      if (!all_flag)
	         retrieve_contents(NULL, diag_toc, cli_display_buf_ptr,
                                   &buf_index, '\0');
	      else {
	         reset_parameter(NULL, diag_parameter);
	         for (sect_toc = diag_toc; sect_toc->description != NULL;
		      sect_toc++) {
	            reset_parameter(NULL, sect_toc->parameter_table);
		    for (test_toc = sect_toc->test_toc;
			 test_toc->description != NULL; test_toc++) {
	               reset_parameter(NULL, test_toc->parameter_table);
		    }
	         }
	      }
	   }
	   else {
	      for (i = 1; i < argc; i++) {
	         argp = argv[i];
		 if (*argp == '-' && *(argp+1) == 's') {
		    if (sect != NULL)
		       return(SYNTAX_MORE_THAN_ONE_SECTION_ID_SPECIFIED_ERROR);
	            argp += 2;
		    if (*argp == '\0') {
	               if (i == (argc-1))
		          return(SYNTAX_SECTION_ID_NOT_SPECIFIED_ERROR);
	               argp = argv[++i];
	            }
		    sect = argp;
		 }
	         else {
		    if (*argp == '-' && *(argp+1) == 't') {
		       if (test != NULL)
		          return(SYNTAX_MORE_THAN_ONE_TEST_ID_SPECIFIED_ERROR);
	               argp += 2;
		       if (*argp == '\0') {
	                  if (i == (argc-1))
		             return(SYNTAX_TEST_ID_NOT_SPECIFIED_ERROR);
	                  argp = argv[++i];
	               }
		       test = argp;
		    }
	         }
	      }
	      if (sect != NULL && test != NULL)
		 return(SYNTAX_BOTH_SECTION_ID_AND_TEST_ID_SPECIFIED_ERROR);
	      if (sect != NULL) {
	         if ((sect_toc = retrieve_section(sect, diag_toc)) ==
	                                               (struct SECTION_TOC *) 0)
		    return(SYNTAX_SECTION_ID_NOT_FOUND_ERROR);
	         parm_table = sect_toc->parameter_table;
	      }
	      else {
	         if (test != NULL) {
	            if ((test_toc = retrieve_test(test, diag_toc)) ==
	                                                  (struct TEST_TOC *) 0)
		       return(SYNTAX_TEST_ID_NOT_FOUND_ERROR);
	            parm_table = test_toc->parameter_table;
	         }
	         else
	            parm_table = diag_parameter;
	      }
	      for (i = 1, parm_flag = FALSE; i < argc; i++) {
	         parm = argv[i];
	         if (*parm++ != '-')
	            return(SYNTAX_INVALID_ARGUMENT_DELIMITER_ERROR);
	         switch (*parm++) {
	            case ARGUMENT_OPTION_SECTION_ID:
		       if (*parm == '\0')
	                  i++;
		       break;
	            case ARGUMENT_OPTION_TEST_ID:
		       if (*parm == '\0')
	                  i++;
		       break;
		    case ARGUMENT_OPTION_PARAMETER:
	               if (*parm == '\0') {
	                  if (i == (argc-1))
			     return(SYNTAX_PARAMETER_NOT_SPECIFIED_ERROR);
	                  parm = argv[++i];
	               }
	               if (rc = reset_parameter(parm, parm_table))
			  return(rc);
	               parm_flag = TRUE;
		       break;
	            default:
	               return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
	         }
	      }
	      if (parm_flag == FALSE)
	         reset_parameter(NULL, parm_table);
	   }
	}

	print_buf(cli_display_buf_ptr, buf_index);

	return(rc = 0);
}

/*-------------------------------------------------------------------*
 * Show()
 *-------------------------------------------------------------------*/
int Show(
	int		argc,
	char		*argv[])
{
	struct SECTION_TOC	*sect_toc;
	struct TEST_TOC		*test_toc;
	register char		*argp;
	register int		i, rc, all_flag;
	int			buf_index;

	if (argc == 2 && !strcmp_nocase(argv[1], "date")) {
	   sprintf(cli_print_buf, " %s\n", sys_date);
	   put_line(cli_print_buf);
	   return(rc = 0);
	}

	if (argc == 2 && !strcmp_nocase(argv[1], "time")) {
	   sprintf(cli_print_buf, " %s\n", sys_time);
	   put_line(cli_print_buf);
	   return(rc = 0);
	}

   /* show PMD serial data */
	if (argc == 2 && !strcmp_nocase(argv[1], "pmd"))
	{
	   show_pmdrom();    
      return(rc = 0);
	}

   /* show PMC counters */
	if (argc == 2 && !strcmp_nocase(argv[1], "pmc"))
	{
		show_pmc_stats();    
		return(rc = 0);
	}

   /* show SARs packets statistics counters */
	if (argc == 2 && !strcmp_nocase(argv[1], "stats"))
	{
		show_stats();    
		return(rc = 0);
	}

#ifdef CALYPSO
#ifndef BOOT
	if (argc == 2 && !strcmp_nocase(argv[1], "smac")) {
	   return(show_source_mac());
	}

	if (argc == 2 && !strcmp_nocase(argv[1], "dmac")) {
	   return(show_dest_mac());
	}
#endif /* BOOT */
#endif /* CALYPSO */

	buf_index = 0; all_flag = FALSE;
	if (argc == 1) {
	   strcpy(cli_display_buf_ptr[buf_index++], 
                  " Global Parameters for Diagnostic Test Suites Execution\n");
	   retrieve_parameter(diag_parameter, cli_display_buf_ptr, &buf_index);
	}
	else {
	   if (argc == 2 && (!strcmp(argv[1], "?") ||
	                    (all_flag = !strcmp_nocase(argv[1], "all")))) {
	      if (!all_flag)
	         retrieve_contents(NULL, diag_toc, cli_display_buf_ptr,
                                   &buf_index, '\0');
	      else {
	         strcpy(cli_display_buf_ptr[buf_index++], 
                   " Global Parameters for Diagnostic Test Suites Execution\n");
	         retrieve_parameter(diag_parameter, cli_display_buf_ptr, 
                                    &buf_index);
	         strcpy(cli_display_buf_ptr[buf_index++], "=");
	         for (sect_toc = diag_toc; sect_toc->description != NULL;
		      sect_toc++) {
	            if (!(sect_toc->execution_control & INVALID)) {
	               sprintf(cli_display_buf_ptr[buf_index++], 
	                       " %s {%s} Parameters -> %s\n",
                               sect_toc->description, sect_toc->name,
	                       sect_toc->id);
	               retrieve_parameter(sect_toc->parameter_table,
	                                  cli_display_buf_ptr, &buf_index);
	               if (sect_toc->test_toc->description == NULL)
	                  strcpy(cli_display_buf_ptr[buf_index++], "=");
	               else
	                  strcpy(cli_display_buf_ptr[buf_index++], "-");
   	   	       for (test_toc = sect_toc->test_toc;
		           test_toc->description != NULL; test_toc++) {
	                  sprintf(cli_display_buf_ptr[buf_index++], 
	                          " %s {%s} Parameters -> %s\n",
                                  test_toc->description, test_toc->name,
	                          test_toc->id);
	                  retrieve_parameter(test_toc->parameter_table,
	                                    cli_display_buf_ptr, &buf_index);
	                  if ((test_toc+1)->description == NULL)
	                     strcpy(cli_display_buf_ptr[buf_index++], "=");
	                  else
	                     strcpy(cli_display_buf_ptr[buf_index++], "-");
	               }
	            }
	         }
	      }
	   }
	   else {
	      for (i = 1; i < argc; i++) {
	         argp = argv[i];
	         if (*argp++ != '-')
	            return(SYNTAX_INVALID_ARGUMENT_DELIMITER_ERROR);
	         switch (*argp++) {
	            case ARGUMENT_OPTION_SECTION_ID:
	               if (*argp == '\0') {
	                  if (i == (argc-1))
			     return(SYNTAX_SECTION_ID_NOT_SPECIFIED_ERROR);
	                  argp = argv[++i];
	               }
	               if ((sect_toc = retrieve_section(argp, diag_toc)) ==
	                                               (struct SECTION_TOC *) 0)
			  return(SYNTAX_SECTION_ID_NOT_FOUND_ERROR);
	               sprintf(cli_display_buf_ptr[buf_index++], 
	                       " %s {%s} Parameters -> %s\n",
                               sect_toc->description, sect_toc->name,
	                       sect_toc->id);
	               retrieve_parameter(sect_toc->parameter_table,
	                                  cli_display_buf_ptr, &buf_index);
	               strcpy(cli_display_buf_ptr[buf_index++], "-");
		       break;
	            case ARGUMENT_OPTION_TEST_ID:
	               if (*argp == '\0') {
	                  if (i == (argc-1))
			     return(SYNTAX_TEST_ID_NOT_SPECIFIED_ERROR);
	                  argp = argv[++i];
	               }
	               if ((test_toc = retrieve_test(argp, diag_toc)) ==
	                                                  (struct TEST_TOC *) 0)
			  return(SYNTAX_TEST_ID_NOT_FOUND_ERROR);
	               sprintf(cli_display_buf_ptr[buf_index++], 
	                       " %s {%s} Parameters -> %s\n",
                               test_toc->description, test_toc->name,
	                       test_toc->id);
	               retrieve_parameter(test_toc->parameter_table,
	                                  cli_display_buf_ptr, &buf_index);
	               strcpy(cli_display_buf_ptr[buf_index++], "-");
		       break;
	            default:
	               return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
	         }
	      }
	   }
	}

	print_buf(cli_display_buf_ptr, buf_index);

	return(rc = 0);
}

int Run(
	int		argc,
	char		*argv[])
{
	struct SECTION_TOC	*sect_toc;
	struct TEST_TOC		*test_toc;
	struct RESULTS_TABLE	*results_table;
	char		*argp;
	register int	i, rc, rc_i;
	register int	all_flag, ex_flag, list_flag, power_on;
	register int	run_flag, sect_flag;
	register int	no_run_flag, all_skip_flag;
	register uInt32	list_runcount;
	register uInt32	sect_runcount, sect_timedelaycount;
	register uInt32	test_runcount;
	register uInt32 hour, minute, second, elapsed_time;
	uInt32		time_v;
	int		buf_index;
	int		tbuf_index, end_index;
	char		*tbuf_ptr;
	char		start_time_str[18], c;

	diag_test_mode = TRUE;

	if (argc == 1)
	   return(SYNTAX_MISSING_ARGUMENTS_ERROR);

	buf_index = tbuf_index = 0; 
	all_flag = ex_flag = list_flag = FALSE;
	run_flag = no_run_flag = all_skip_flag = TRUE;

	if (argc == 2) {
	   if (!strcmp(argv[1], "?")) {
	      retrieve_contents(NULL, diag_toc, cli_display_buf_ptr, &buf_index,
                                'r');
	      run_flag = FALSE;
	   }
	   else {
	      if (!strcmp(argv[1], "!"))
	         list_flag = TRUE;
	      if (!strcmp_nocase(argv[1], "all"))
	         all_flag = TRUE;
	      if (!strcmp_nocase(argv[1], "exhaustive"))
	         ex_flag = TRUE;
	   }
	}

#ifdef CALYPSO
#ifndef BOOT
	if (argc == 2 || argc == 3) {
	   if (!strcmp_nocase(argv[1], "powerup")) {
	      power_on = FALSE;
	      if (argc == 3 && !strcmp_nocase(argv[2], "exhaustive")) {
	         ex_flag = TRUE;
	         return(rc = power_up_tests(power_on, ex_flag));
	      }
	      else {
	         if (argc == 2) {
	            ex_flag = FALSE;
	            return(rc = power_up_tests(power_on, ex_flag));
	         }
	      }
	   }
	}
#endif /* BOOT */
#endif /* CALYPSO */

	if (run_flag) {

	   diag_start_time_msec = system_tick_time;
	   sprintf(start_time_str, "%s %s", sys_date, sys_time);

	   for (sect_toc = diag_toc; sect_toc->description != NULL; 
                sect_toc++) {
	      if (!list_flag && !all_flag && !ex_flag) {
	         sect_toc->execution_control &= ~RUN;
	      }
	      else {
	         if (list_flag && (sect_toc->execution_control & RUN)
	                       && !(sect_toc->execution_control & INVALID))
	            no_run_flag = FALSE;
	         if ((all_flag || ex_flag) && 
	            !(sect_toc->execution_control & SKIP) &&
	            !(sect_toc->execution_control & INVALID))
	            all_skip_flag = FALSE;
	      }

	      for (test_toc = sect_toc->test_toc; 
                   test_toc->description != NULL; test_toc++) {
	         if (!list_flag && !all_flag && !ex_flag) {
	            test_toc->execution_control &= ~RUN;
	         }
	         else {
	            if (list_flag && (test_toc->execution_control & RUN))
	               no_run_flag = FALSE;
	            if ((all_flag || ex_flag) && 
	               !(test_toc->execution_control & SKIP))
	               all_skip_flag = FALSE;
	         }
	         test_toc->execution_control &= ~INIT;
	      }
	   }

	   for (list_runcount = 1, diag_total_errorcount = 0; 
	        list_runcount <= diag_runcount; list_runcount++) {

	      sect_toc = diag_toc;

	      for (i = 1; (i < argc) || list_flag || all_flag || ex_flag; i++) {
	         if (!list_flag && !all_flag && !ex_flag) {
	            argp = argv[i];
	            if (*argp++ != '-')
	               return(SYNTAX_INVALID_ARGUMENT_DELIMITER_ERROR);
	            switch (*argp++) {
	               case ARGUMENT_OPTION_SECTION_ID:
	                  if (*argp == '\0') {
	                     if (i == (argc-1))
			        return(SYNTAX_SECTION_ID_NOT_SPECIFIED_ERROR);
	                     argp = argv[++i];
	                  }
	                  if ((sect_toc = retrieve_section(argp, diag_toc)) ==
	                                               (struct SECTION_TOC *) 0)
			     return(SYNTAX_SECTION_ID_NOT_FOUND_ERROR);
	                  sect_runcount = *((uInt32 *) sect_toc->
                                                    parameter_table->parameter);
	                  diag_sect_maxerrorcount = *((uInt32 *) (sect_toc->
                                                 parameter_table+1)->parameter);
	                  sect_timedelaycount = *((uInt32 *) (sect_toc->
                                                 parameter_table+2)->parameter);
	                  sect_toc->execution_control |= RUN;
	                  test_toc = sect_toc->test_toc;
	                  sect_flag = TRUE;
		          break;
	               case ARGUMENT_OPTION_TEST_ID:
	                  if (*argp == '\0') {
	                     if (i == (argc-1))
			        return(SYNTAX_TEST_ID_NOT_SPECIFIED_ERROR);
	                     argp = argv[++i];
	                  }
	                  for (sect_toc = diag_toc; 
	                       sect_toc->description != NULL; sect_toc++) {
	                     if (!(sect_toc->execution_control & INVALID)) {
	                        sect_runcount =*((uInt32 *) sect_toc->
	                                            parameter_table->parameter);
	                        diag_sect_maxerrorcount=*((uInt32 *) (sect_toc->
                                                 parameter_table+1)->parameter);
	                        sect_timedelaycount = *((uInt32 *) (sect_toc->
	                                         parameter_table+2)->parameter);
	                        for (test_toc = sect_toc->test_toc; 
	                             test_toc->description != NULL;test_toc++) {
	                           if (!strcmp_nocase(argp, test_toc->name) ||
	                               !strcmp_nocase(argp, test_toc->id))
	                              break;
	                        }
	                        if (test_toc->description != NULL)
	                           break;
	                     }
	                  }
	                  if (sect_toc->description == NULL)
			     return(SYNTAX_TEST_ID_NOT_FOUND_ERROR);
	                  test_toc->execution_control |= RUN;
	                  sect_flag = FALSE;
		          break;
	               default:
	                  return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
	            }
	         }
	         else {
	            sect_flag = TRUE;
	            sect_runcount =*((uInt32 *) sect_toc->parameter_table->
	                                                             parameter);
	            diag_sect_maxerrorcount = *((uInt32 *) (sect_toc->
	                                         parameter_table+1)->parameter);
	            sect_timedelaycount = *((uInt32 *) (sect_toc->
	                                         parameter_table+2)->parameter);
	            test_toc = sect_toc->test_toc;
	            if (list_flag && (!(sect_toc->execution_control & RUN) ||
	                               (sect_toc->execution_control & INVALID)))
	               sect_flag = FALSE;
	            if ((all_flag || ex_flag) && 
	               ((sect_toc->execution_control & SKIP) ||
	                (sect_toc->execution_control & INVALID)))
	               sect_flag = FALSE;
	         }

		 for ( ; test_toc->description != NULL; test_toc++) {

	            diag_test_name = test_toc->name;
	            diag_test_id   = test_toc->id;

	            if (list_flag) {
	               if (!sect_flag && !(test_toc->execution_control & RUN))
	                  continue;
	            }
	            if (all_flag || ex_flag) {
	               if (!sect_flag || (test_toc->execution_control & SKIP))
	                  continue;
	            }
	            if ((all_flag || ex_flag) && diag_allprompt[0] == 'T') {
	               if (diag_verbosity <= VERBOSITY_LEVEL1)
	                  put_line("\n");
	               sprintf(cli_print_buf, 
	                       " Confirm Execution of %s Test {%s} (y/n/q)",
	                       diag_test_name, diag_test_id);
	               put_line(cli_print_buf);
	               c = (char) getchar_p();
	               sprintf(cli_print_buf, "%c\n", c);
	               put_line(cli_print_buf);
	               if (isalpha(c) && isupper(c))
	                  c = tolower(c);
	               if (c != 'y') {
	                  if (c == 'q') {
	                     rc = BREAK_ALL_TESTS;
	                     break;
	                  }
	                  if (!sect_flag)
	                     break;
	                  continue;
	               }
	            }

	            for (test_runcount = 1, diag_test_errorcount = 0; 
	                 test_runcount <= sect_runcount; test_runcount++) {

	               rc_i = test_toc->rc_index;

	               if (test_toc->execution_control & INIT) {
	                  diag_test_errorcount = *((uInt32 *) 
	                              (test_toc->results_table+rc_i+2)->result);
	                  if (diag_test_errorcount > diag_sect_maxerrorcount)
	                     break;
	               }

	               if (!(test_toc->execution_control & INIT))
	                  initialize_test(test_toc);

	               (*((uInt32 *) (test_toc->results_table+rc_i)->result))++;
	               diag_current_runcount = *((uInt32 *) 
	                                (test_toc->results_table+rc_i)->result);

	               if (diag_verbosity >= VERBOSITY_LEVEL1) {
	                  display_test_progress();
	               }

	               sys_error_code = 0; sys_specific_info[0] = '\0';
	               rc = (test_toc->procedure)(ex_flag);  

	               if (rc == BREAK_ALL_TESTS || rc == BREAK_CURRENT_TEST)
	                  break;

	               /* Update Error Code */
	               if (rc)
	                  sys_error_code = rc;
	               *((uInt32 *) (test_toc->results_table+2)->result)
	                                                       = sys_error_code;

	               tbuf_ptr = cli_test_buf_ptr[tbuf_index%TEST_BUF_SIZE];
	               tbuf_index++;

	               if (sys_error_code) {

	                  /*Update Fail_Address/Data_Written/Data_Read fields*/
	                  *((uInt32 *) (test_toc->results_table+rc_i+3)->result)
	                                                     = sys_fail_address;
	                  *((uInt32 *) (test_toc->results_table+rc_i+4)->result)
	                                                    = sys_data_expected;
	                  *((uInt32 *) (test_toc->results_table+rc_i+5)->result)
	                                                        = sys_data_read;
	                  (test_toc->results_table+rc_i+3)->display_control= ON;
	                  (test_toc->results_table+rc_i+4)->display_control= ON;
	                  (test_toc->results_table+rc_i+5)->display_control= ON;
	               }

	               if (rc) {
	                  /* Increment Test Fail Count */
	                  (*((uInt32 *) (test_toc->results_table+rc_i+1)->
	                                                             result))++;
	                  (test_toc->results_table+rc_i+1)->display_control= ON;
	                  log_test_status(TEST_FAIL, tbuf_ptr);
	                  if (diag_verbosity >= VERBOSITY_LEVEL1) {
	                     display_test_failure(rc);
	                  }
	               }
	               else {
	                  if (sys_error_code) {
	                     log_test_status(TEST_WARNING, tbuf_ptr);
	                     if (diag_verbosity >= VERBOSITY_LEVEL1) {
	                        display_test_warning(sys_error_code);
	                     }
	                  }
	                  else
	                     log_test_status(TEST_SUCCESSFUL, tbuf_ptr);
	               }

	               if (diag_test_errorcount != 0) {
	                  *((uInt32 *) (test_toc->results_table+rc_i+2)->
	                                         result) = diag_test_errorcount;
	                  (test_toc->results_table+rc_i+2)->display_control= ON;
	               }

	               if (diag_total_errorcount > diag_maxerrorcount) {
	                  rc = BREAK_ALL_TESTS;
	                  break;
	               }
	               if (rc = check_for_break())
	                  break;

	               for (time_v = system_tick_time; 
	                   (system_tick_time - time_v) < 
	                   (1000*sect_timedelaycount); ) {
	                  if (rc = check_for_break())
	                     break;
	               }
	               if (rc == BREAK_ALL_TESTS || rc == BREAK_CURRENT_TEST)
	                  break;
	            }

	            if (rc == BREAK_ALL_TESTS)
	               break;
	            if (!sect_flag)
	               break;
		 }

	         if (rc == BREAK_ALL_TESTS)
	            break;
	         if (list_flag || all_flag || ex_flag) {
	            sect_toc++;
	            if (sect_toc->description == NULL)
	               break;
	         }
	      }
	      if (rc == BREAK_ALL_TESTS)
	         break;
	      for (time_v = system_tick_time; 
	          (system_tick_time - time_v) < 
	          (1000*diag_timedelaycount); ) {
	         if (rc = check_for_break())
	            break;
	      }
	      if (rc == BREAK_ALL_TESTS)
	         break;
	   }
	   diag_end_time_msec = system_tick_time;
	   sprintf(diag_end_time, "%s %s", sys_date, sys_time);
	   strcpy(diag_start_time, start_time_str);

	   diag_elapsed_time_msec = (diag_end_time_msec - diag_start_time_msec);
	   elapsed_time = diag_elapsed_time_msec + 500; 
	   hour = (elapsed_time/3600000);
	   elapsed_time -= (hour*3600000);
	   minute = (elapsed_time/60000);
	   second = (elapsed_time - (minute*60000))/1000;
	   sprintf(diag_elapsed_time, "        ~%02lu:%02lu:%02lu",
	           hour, minute, second);
	   diag_total_errorcount_x = diag_total_errorcount;
	   for (i = 0; i < 6; i++)
	      diag_results[i].display_control = ON;
	}

	diag_test_mode = FALSE;

	print_buf(cli_display_buf_ptr, buf_index);

	if (run_flag) {
	   if (diag_summaryformat[0] == 'F') {
	      if (tbuf_index > TEST_BUF_SIZE) {
	         end_index = TEST_BUF_SIZE;
	         tbuf_index -= TEST_BUF_SIZE;
	      }
	      else {
	         end_index = tbuf_index;
	         tbuf_index = 0;
	      }
	      for (buf_index = 0; buf_index < end_index; 
	           buf_index++, tbuf_index++) {
	         strcpy(cli_display_buf_ptr[buf_index], 
	                cli_test_buf_ptr[tbuf_index%TEST_BUF_SIZE]);
	      }
	   }
	   else {
	      if (diag_summaryformat[0] == 'L') {
	         if (tbuf_index > TEST_BUF_SIZE) {
	            end_index = TEST_BUF_SIZE;
	            tbuf_index--;
	         }
	         else {
	            end_index = tbuf_index;
	            tbuf_index--;
	         }
	         for (buf_index = 0; buf_index < end_index; 
	              buf_index++, tbuf_index--) {
	            strcpy(cli_display_buf_ptr[buf_index], 
	                   cli_test_buf_ptr[tbuf_index%TEST_BUF_SIZE]);
	         }
	      }
	   }
	   if (diag_verbosity <= VERBOSITY_LEVEL1)
	      put_line("\n");
	   print_buf(cli_display_buf_ptr, buf_index);
	}

	return(rc = 0);
}

int Skip(
	int		argc,
	char		*argv[])
{
	struct SECTION_TOC	*sect_toc;
	struct TEST_TOC		*test_toc;
	register char		*argp;
	register int		i, rc, none_flag;
	int			buf_index;

	if (argc == 1)
	   return(SYNTAX_MISSING_ARGUMENTS_ERROR);

	buf_index = 0; none_flag = FALSE;
	if (argc == 2 && (!strcmp(argv[1], "?") ||
	                 (none_flag = !strcmp_nocase(argv[1], "none")))) {
	   if (!none_flag)
	      retrieve_contents(NULL, diag_toc, cli_display_buf_ptr,
                                &buf_index, 's');
	   else {
	      for (sect_toc = diag_toc; sect_toc->description != NULL;
	           sect_toc++) {
	         sect_toc->execution_control &= ~SKIP;
		 for (test_toc = sect_toc->test_toc;
		      test_toc->description != NULL; test_toc++)
	            test_toc->execution_control &= ~SKIP;
	      }
	   }
	}
	else {
	   for (sect_toc = diag_toc; sect_toc->description != NULL;
	        sect_toc++) {
	      sect_toc->execution_control &= ~SKIP;
	      for (test_toc = sect_toc->test_toc;
		   test_toc->description != NULL; test_toc++)
	         test_toc->execution_control &= ~SKIP;
	   }
	   for (i = 1; i < argc; i++) {
	      argp = argv[i];
	      if (*argp++ != '-')
	         return(SYNTAX_INVALID_ARGUMENT_DELIMITER_ERROR);
	      switch (*argp++) {
	         case ARGUMENT_OPTION_SECTION_ID:
	            if (*argp == '\0') {
	               if (i == (argc-1))
	                  return(SYNTAX_SECTION_ID_NOT_SPECIFIED_ERROR);
	               argp = argv[++i];
	            }
	            if ((sect_toc = retrieve_section(argp, diag_toc)) ==
	                                               (struct SECTION_TOC *) 0)
		       return(SYNTAX_SECTION_ID_NOT_FOUND_ERROR);
	            sect_toc->execution_control |= SKIP;
		    break;
	         case ARGUMENT_OPTION_TEST_ID:
	            if (*argp == '\0') {
	               if (i == (argc-1))
			  return(SYNTAX_TEST_ID_NOT_SPECIFIED_ERROR);
	               argp = argv[++i];
	            }
	            if ((test_toc = retrieve_test(argp, diag_toc)) ==
	                                                  (struct TEST_TOC *) 0)
		       return(SYNTAX_TEST_ID_NOT_FOUND_ERROR);
	            test_toc->execution_control |= SKIP;
		    break;
	         default:
	            return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
	      }
	   }
	}

	print_buf(cli_display_buf_ptr, buf_index);

	return(rc = 0);
}

int Results(
	int		argc,
	char		*argv[])
{
extern	char			*diag_error_info[];
	struct SECTION_TOC	*sect_toc;
	struct TEST_TOC		*test_toc;
	register char		*argp;
	register int		i, rc, all_flag, err_flag, sum_flag;
	int			buf_index;
	uInt32			err;

	buf_index = 0; all_flag = err_flag = sum_flag = FALSE;
	if (argc == 1) {
	   retrieve_results(diag_results, cli_display_buf_ptr, &buf_index, 0);
	}
	else {
	   if (argc == 2 && (!strcmp(argv[1], "?") ||
	                    (all_flag = !strcmp_nocase(argv[1], "all")) ||
#ifdef CALYPSO
	                    (sum_flag = !strcmp_nocase(argv[1], "errcode")) ||
#endif /* CALYPSO */
	                    (err_flag = !strcmp_nocase(argv[1], "errors")))) {
	      if (!all_flag && !err_flag && !sum_flag)
	         retrieve_contents(NULL, diag_toc, cli_display_buf_ptr,
                                   &buf_index, '\0');
	      else {
	         if (all_flag) {
	            if (retrieve_results(diag_results, cli_display_buf_ptr, 
                                         &buf_index, 0))
	               strcpy(cli_display_buf_ptr[buf_index++], "=");
	         }
	         for (sect_toc = diag_toc; sect_toc->description != NULL;
		      sect_toc++) {
	            if (!(sect_toc->execution_control & INVALID)) {
		       for (test_toc = sect_toc->test_toc;
		   	    test_toc->description != NULL; test_toc++) {
#ifdef CALYPSO
	                  if (sum_flag) {
	                     if ((err=*((uInt32 *) (test_toc->results_table+2)->
	                                                           result)) > 0)
	                        if ((test_toc->results_table+2)->
	                                                  display_control == ON)
	                           sprintf(cli_display_buf_ptr[buf_index++], 
	                                   " %s %4u %s\n", test_toc->id, err,
	                                   diag_error_info[err-1]);
	                  }
	                  else {
#endif /* CALYPSO */
	                     if (all_flag || (err_flag && 
	                                   *((uInt32 *) (test_toc->
	                                      results_table+2)->result) > 0))
	                        if (retrieve_results(test_toc->results_table,
	                                        cli_display_buf_ptr, &buf_index,
	                                        test_toc))
	                           strcpy(cli_display_buf_ptr[buf_index++],"-");
#ifdef CALYPSO
	                  }
#endif /* CALYPSO */
		       }
	            }
	         }
	      }
	   }
	   else {
	      for (i = 1; i < argc; i++) {
	         argp = argv[i];
	         if (*argp++ != '-')
	            return(SYNTAX_INVALID_ARGUMENT_DELIMITER_ERROR);
	         switch (*argp++) {
	            case ARGUMENT_OPTION_SECTION_ID:
	               if (*argp == '\0') {
	                  if (i == (argc-1))
			     return(SYNTAX_SECTION_ID_NOT_SPECIFIED_ERROR);
	                  argp = argv[++i];
	               }
	               if ((sect_toc = retrieve_section(argp, diag_toc)) ==
	                                               (struct SECTION_TOC *) 0)
			  return(SYNTAX_SECTION_ID_NOT_FOUND_ERROR);
		       for (test_toc = sect_toc->test_toc;
		   	    test_toc->description != NULL; test_toc++) {
	                  if (retrieve_results(test_toc->results_table,
	                                       cli_display_buf_ptr, &buf_index,
	                                       test_toc))
	                     strcpy(cli_display_buf_ptr[buf_index++], "-");
		       }
		       break;
	            case ARGUMENT_OPTION_TEST_ID:
	               if (*argp == '\0') {
	                  if (i == (argc-1))
			     return(SYNTAX_TEST_ID_NOT_SPECIFIED_ERROR);
	                  argp = argv[++i];
	               }
	               if ((test_toc = retrieve_test(argp, diag_toc)) ==
	                                                  (struct TEST_TOC *) 0)
			  return(SYNTAX_TEST_ID_NOT_FOUND_ERROR);
	               if (retrieve_results(test_toc->results_table,
	                                    cli_display_buf_ptr, &buf_index,
	                                    test_toc))
	                  strcpy(cli_display_buf_ptr[buf_index++], "-");
		       break;
	            default:
	               return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
	         }
	      }
	   }
	}

	print_buf(cli_display_buf_ptr, buf_index);

	return(rc = 0);
}

int retrieve_contents(
	char			*sect,
	struct SECTION_TOC	*sect_toc,
	char			*buf_ptr[],
	int			*buf_index,
	char			cmd)
{
	struct TEST_TOC		*test_toc;
	char			flag;
	register int		rc, index;

	index = *buf_index;
	while (sect_toc->description != NULL) {
	   if (!(sect_toc->execution_control & INVALID)) {
	      if (sect == NULL || !strcmp_nocase(sect, sect_toc->name) ||
	                          !strcmp_nocase(sect, sect_toc->id)) {
	         if ((cmd == 'r' && (sect_toc->execution_control & RUN)) ||
	             (cmd == 's' && (sect_toc->execution_control & SKIP)))
	            flag = '>';
	         else
	            flag = ' ';
	         sprintf(buf_ptr[(*buf_index)++], "%c%s  %s {%s}\n",
	                 flag, sect_toc->id, sect_toc->description,
	                 sect_toc->name);
	         for (test_toc = sect_toc->test_toc;
	              test_toc->description != NULL; test_toc++) {
	            if ((cmd == 'r' && (test_toc->execution_control & RUN)) ||
	                (cmd == 's' && (test_toc->execution_control & SKIP)))
	               flag = '>';
	            else
	               flag = ' ';
	            sprintf(buf_ptr[(*buf_index)++], "   %c%s  %s {%s}\n",
	                    flag, test_toc->id, test_toc->description,
	                    test_toc->name);
	         }
	      }
	   }
	   sect_toc++;
	}

	if (index == *buf_index)
	   return(SYNTAX_SECTION_ID_NOT_FOUND_ERROR);

	return(rc = 0);
}

int set_parameter(
	char			*parm,
	struct PARAMETER_TABLE	*parm_table,
	char			*parm_value)
{
	uInt32		value;
	char		str_value[80], buf[3], *ptr;
	register int	rc, p, a_flag, d_flag, bitmap;

	if (!strcmp(parm_value, "%"))
	   d_flag = TRUE;
	else
	   d_flag = FALSE;

	if (!strcmp(parm_value, "*"))
	   a_flag = TRUE;
	else
	   a_flag = FALSE;

	while (parm_table->parameter != NULL) {
	   if (!strcmp_v(parm, parm_table->name)) {
	      if (!strcmp(parm_table->format, fmt_string)) {
#ifdef CALYPSO
	         if (parm_table->parameter_control & PARAMETER_MVALUE) {
	            if (strlen(parm_value) > parm_table->max_value.length)
	               return(SYNTAX_PARAMETER_LENGTH_OUT_OF_RANGE_ERROR);
	            if (a_flag || !strcmp_v(parm_value, "all")) {
	               if ((p = strcmp_rv("all", parm_table->discrete_value, 
	                                  str_value)) < 0)
	                  return(SYNTAX_INVALID_PARAMETER_VALUE_ERROR);
	               *parm_table->min_value.bitmap_p = 0;
	               while (p--) {
	                  *parm_table->min_value.bitmap_p |= (1 << p);
	               } 
	               strcpy(parm_table->parameter, "All");
	            }
	            else {
	               bitmap = 0;
	               if (d_flag)
	                  ptr = parm_table->default_value.ptr;
	               else
	                  ptr = parm_value;
	               while (1) {
	                  buf[0] = *ptr++; 
	                  if (*ptr == ',' || *ptr == '\0')
	                     buf[1] = '\0';
	                  else {
	                     buf[1] = *ptr++; buf[2] = '\0';
	                  }
	                  if ((p = strcmp_rv(buf, parm_table->discrete_value, 
	                                     str_value)) < 0 ||
	                       strlen(buf) != strlen(str_value))
	                     return(SYNTAX_INVALID_PARAMETER_VALUE_ERROR);
	                  bitmap |= (1 << p);
	                  if (*ptr == '\0')
	                     break;
	                  if (*ptr++ != ',' || *ptr == '\0')
	                     return(SYNTAX_INVALID_PARAMETER_VALUE_ERROR);
	               }
	               *parm_table->min_value.bitmap_p = bitmap;
	               if (d_flag)
	                  strcpy(parm_table->parameter, 
	                         parm_table->default_value.ptr);
	               else
	                  strcpy(parm_table->parameter, parm_value);
	            }
	         }
	         else {
#endif /* CALYPSO */
	            if (d_flag) {
	               strcpy(parm_table->parameter, 
	                      parm_table->default_value.ptr);
	            }
	            else {
	               if (a_flag)
	                  rc = strcmp_rv("all", parm_table->discrete_value, 
	                                 str_value);
	               else
	                  rc = strcmp_rv(parm_value, parm_table->discrete_value,
                                         str_value);
	               if (rc >= 0)
	                  strcpy(parm_table->parameter, str_value);
	               else
	                  return(SYNTAX_INVALID_PARAMETER_VALUE_ERROR);
	            }
#ifdef CALYPSO
	         }
#endif /* CALYPSO */
	      }
	      else {
	         if (d_flag) {
	            *((uInt32 *)parm_table->parameter) = 
	                                        parm_table->default_value.value;
	         }
	         else {
	            if (a_flag) {
	               *((uInt32 *) parm_table->parameter) = 
	                                            parm_table->max_value.value;
	            }
	            else {
	               if (!sscanf_x(parm_value, parm_table->format, &value))
	                  return(SYNTAX_INVALID_PARAMETER_VALUE_ERROR);
	               if (value < parm_table->min_value.value ||
	                   value > parm_table->max_value.value)
	                  return(SYNTAX_PARAMETER_VALUE_OUT_OF_RANGE_ERROR);
	               *((uInt32 *) parm_table->parameter) = value;
	            }
	         }
	      }
	      parm_table->parameter_control |=  PARAMETER_SET;
	      parm_table->parameter_control &= ~PARAMETER_RESET;
	      break;
	   }
	   parm_table++;
	}

	if (parm_table->parameter == NULL)
	   return(SYNTAX_PARAMETER_NOT_FOUND_ERROR);

	return(rc = 0);
}

int reset_parameter(
	char			*parm,
	struct PARAMETER_TABLE	*parm_table)
{
	register int	rc, p, bitmap;
	char		str_value[80], buf[3], *ptr;

	while (parm_table->parameter != NULL) {
	   if (parm == NULL || !strcmp_v(parm, parm_table->name)) {
	      if (!strcmp(parm_table->format, fmt_string)) {
	         strcpy(parm_table->parameter, parm_table->default_value.ptr);
#ifdef CALYPSO
	         if (parm_table->parameter_control & PARAMETER_MVALUE) {
	            if (!strcmp_v(parm_table->parameter, "all")) {
	               p = strcmp_rv("all", parm_table->discrete_value, 
	                             str_value);
	               *parm_table->min_value.bitmap_p = 0;
	               while (p--) {
	                  *parm_table->min_value.bitmap_p |= (1 << p);
	               } 
	            }
	            else {
	               bitmap = 0;
	               ptr = parm_table->parameter;
	               while (1) {
	                  buf[0] = *ptr++; 
	                  if (*ptr == ',' || *ptr == '\0')
	                     buf[1] = '\0';
	                  else {
	                     buf[1] = *ptr++; buf[2] = '\0';
	                  }
	                  p = strcmp_rv(buf, parm_table->discrete_value, 
	                                str_value);
	                  bitmap |= (1 << p);
	                  if (*ptr++ == '\0')
	                     break;
	               }
	               *parm_table->min_value.bitmap_p = bitmap;
	            }
	         }
#endif /* CALYPSO */
	      }
	      else
	         *((uInt32 *)parm_table->parameter) = 
	                                        parm_table->default_value.value;
	      parm_table->parameter_control |=  PARAMETER_RESET;
	      parm_table->parameter_control &= ~PARAMETER_SET;
	      if (parm != NULL)
	         break;
	   }
	   parm_table++;
	}

	if (parm != NULL && parm_table->parameter == NULL)
	   return(SYNTAX_PARAMETER_NOT_FOUND_ERROR);

	return(rc = 0);
}

int retrieve_parameter(
	struct PARAMETER_TABLE	*parm_table,
	char			*buf_ptr[],
	int			*buf_index)
{
	char		m_flag;
	char		r_string[80], f_string[20];
	register int	rc, i, len, max_len, max_len1;
	register int	index;

	for (i = max_len = 0, max_len1 = 10; (parm_table+i)->parameter != NULL;
	     i++) {
	   max_len = max_len > (len = strlen((parm_table+i)->name)) ?
		     max_len : len;
	   if (!strcmp((parm_table+i)->format, fmt_string)) {
	      max_len1 = max_len1 > (len = strlen((parm_table+i)->parameter)) ?
	                 max_len1 : len;
	   }
	}
	sprintf(f_string, "%%%us {%%s}\n", max_len1);

	for (index = *buf_index; parm_table->parameter != NULL;
	     (*buf_index)++, parm_table++) {
	   if (parm_table->parameter_control & PARAMETER_SET)
	      m_flag = '*';
	   else
	      if (parm_table->parameter_control & PARAMETER_RESET)
	         m_flag = '%';
	      else
		 m_flag = ' ';
	   sprintf(buf_ptr[*buf_index], "%c%s", m_flag, parm_table->name);
	   for (len = strlen(parm_table->name); len < max_len; len++)
	      strcat(buf_ptr[*buf_index], " ");
	   strcat(buf_ptr[*buf_index], " : ");
	   if (!strcmp(parm_table->format, fmt_string)) {
	      sprintf(r_string, f_string, 
                      (char *) parm_table->parameter,
	              parm_table->discrete_value);
	   }
	   else {
	   for (len = 10; len < max_len1; len++)
	      strcat(buf_ptr[*buf_index], " ");
	   if (!strcmp(parm_table->format, fmt_xInt32))
	      sprintf(r_string, "0x%08lX {0x%08lX <-> 0x%08lX}\n", 
                      *((uInt32 *) parm_table->parameter),
	              parm_table->min_value.value, parm_table->max_value.value);
	   if (!strcmp(parm_table->format, fmt_xInt16))
	      sprintf(r_string, "    0x%04lX {0x%04lX <-> 0x%04lX}\n", 
                      *((uInt32 *) parm_table->parameter),
	              parm_table->min_value.value, parm_table->max_value.value);
	   if (!strcmp(parm_table->format, fmt_xInt08))
	      sprintf(r_string, "      0x%02lX {0x%02lX <-> 0x%02lX}\n", 
                      *((uInt32 *) parm_table->parameter),
	              parm_table->min_value.value, parm_table->max_value.value);
	   if (!strcmp(parm_table->format, fmt_uInt32))
	      sprintf(r_string, "  %8lu {%lu <-> %lu}\n", 
                      *((uInt32 *) parm_table->parameter),
	              parm_table->min_value.value, parm_table->max_value.value);
	   }
	   strcat(buf_ptr[*buf_index], r_string);
	}

	return(rc = 0);
}

int retrieve_results(
	struct RESULTS_TABLE	*results_table,
	char			*buf_ptr[],
	int			*buf_index,
	struct TEST_TOC		*test_toc)
{
	char		r_string[80];
	register int	i, len, max_len;
	register int	index;

	for (i = max_len = 0; (results_table+i)->result != NULL; i++) {
	   if ((results_table+i)->display_control == ON)
	      max_len = max_len > (len = strlen((results_table+i)->name)) ?
	   	        max_len : len;
	}

	for (index = *buf_index; results_table->result != NULL;
	     results_table++) {
	   if (results_table->display_control == ON) {
	      sprintf(buf_ptr[*buf_index], " %s", results_table->name);
	      for (len = strlen(results_table->name); len < max_len; len++)
	         strcat(buf_ptr[*buf_index], " ");
	      strcat(buf_ptr[*buf_index], " : ");
	      if (strchr(results_table->format, 's'))
	         sprintf(r_string, results_table->format, 
	                 results_table->result);
	      else
	         sprintf(r_string, results_table->format,
                         *((uInt32 *) results_table->result));
	      strcat(buf_ptr[*buf_index], r_string);
	      if (index == *buf_index && test_toc != (struct TEST_TOC *) 0) {
	         sprintf(r_string, " {%s} -> %s", test_toc->name, test_toc->id);
	         strcat(buf_ptr[*buf_index], r_string);
	      }
	      strcat(buf_ptr[*buf_index], "\n");
	      (*buf_index)++;
	   }
	}

	return(index != *buf_index);
}

struct SECTION_TOC *retrieve_section(
	char			*sect,
	struct SECTION_TOC	*sect_toc)
{
	while (sect_toc->description != NULL) {
	   if (!(sect_toc->execution_control & INVALID)) {
	      if (!strcmp_nocase(sect, sect_toc->name) ||
	          !strcmp_nocase(sect, sect_toc->id))
	         return(sect_toc);
	   }
	   sect_toc++;
	}

	return((struct SECTION_TOC *) 0);
}

struct TEST_TOC *retrieve_test(
	char			*test,
	struct SECTION_TOC	*sect_toc)
{
	struct TEST_TOC		*test_toc;

	while (sect_toc->description != NULL) {
	   for (test_toc = sect_toc->test_toc; test_toc->description != NULL;
		test_toc++)
	      if (!strcmp_nocase(test, test_toc->name) ||
	          !strcmp_nocase(test, test_toc->id))
	         return(test_toc);
	   sect_toc++;
	}

	return((struct TEST_TOC *) 0);
}

void initialize_test(
	struct TEST_TOC	*test_toc)
{
	struct PARAMETER_TABLE	*p_table;
	struct RESULTS_TABLE	*r_table;
	register int		i;

	p_table = test_toc->parameter_table;
	r_table = test_toc->results_table + 1;

	sprintf(r_table->result, "%s %s", sys_date, sys_time); /* Time of Run */

	*((Int32 *) (++r_table)->result) = 0; /* Error Code */

	/* Copy Parameter Values */
	while (p_table->parameter != NULL) {
	   if (!strcmp(p_table->format, fmt_string))
	      strcpy((++r_table)->result, p_table->parameter);
	   else
	      *((uInt32 *) (++r_table)->result) = 
	                                     *((uInt32 *) (p_table->parameter));
	   p_table++;
	}
	*(uInt32 *) ((++r_table)->result) = 0; /* Run Count */

	*((uInt32 *) (++r_table)->result) = 0; /* Test Fail Count */
	*((uInt32 *) (++r_table)->result) = 0; /* Error Count */

	for (r_table = test_toc->results_table, i = 0; 
	    (r_table+i)->result != NULL; i++)
           if (i <= test_toc->rc_index)
	      (r_table+i)->display_control = ON;
	   else
	      (r_table+i)->display_control = OFF;

	test_toc->execution_control |= INIT;
}

void display_test_progress()
{
	if (diag_verbosity == VERBOSITY_LEVEL1)
	   put_line(".");
	else {
	   sprintf(cli_print_buf, test_progress_msg,
	           diag_test_name, diag_test_id,
	           diag_current_runcount);
	   put_line(cli_print_buf);
	}
}

void log_test_status(
	enum TEST_STATUS	status,
	char			*test_buf_ptr)
{
	char			*ptr;

	switch(status) {
	   case TEST_FAIL:    ptr = test_fail_msg; break;
	   case TEST_WARNING: ptr = test_warning_msg; break;
	   default:           ptr = test_successful_msg; break;
	}

	sprintf(test_buf_ptr, ptr, diag_test_name, diag_test_id,
	        diag_current_runcount);
}

void display_test_failure(
	int		rc)
{
	if (diag_verbosity == VERBOSITY_LEVEL1)
	   put_line("F");
	else {
	   sprintf(cli_print_buf, TEST_FAIL_MSG,
	           diag_test_name, diag_test_id, rc);
	   put_line(cli_print_buf);
	}
}

void display_test_warning(
	int		rc)
{
	if (diag_verbosity == VERBOSITY_LEVEL1)
	   put_line("W");
	else {
	   sprintf(cli_print_buf, TEST_WARNING_MSG,
	           diag_test_name, diag_test_id, rc);
	   put_line(cli_print_buf);
	}
}

int check_for_test_stop()
{
	diag_test_errorcount++; diag_total_errorcount++;

	if (diag_total_errorcount > diag_maxerrorcount ||
	    diag_test_errorcount  > diag_sect_maxerrorcount)
	   return(1);

	return(0);
}

int Drivers(
	int		argc,
	char		*argv[])
{
	DM_SetupDevices (TRUE) ;

	return (0) ;
}
