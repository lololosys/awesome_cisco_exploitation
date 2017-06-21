/* $Id: cfg_modem.h,v 3.1.64.2 1996/09/09 20:56:34 jchlin Exp $
 * $Source: /release/112/cvs/Xsys/les/cfg_modem.h,v $
 *------------------------------------------------------------------
 *  C F G _  M O D E M . H
 *
 * Nov 1995, Viet Nguyen
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_modem.h,v $
 * Revision 3.1.64.2  1996/09/09  20:56:34  jchlin
 * CSCdi66525:  ct1/rbs cannot busy back the CO trunk when theres no free
 * modem
 * Branch: California_branch
 * o       The bugfix will busy dsx0s back to CO thru RBS when there's
 *         no free modem to accept incoming calls.
 * o       The modem pool is changed to have multiple pools for different
 *         types of modems.
 * o       Add hidden debug commands.
 *
 * Revision 3.1.64.1  1996/06/16  21:14:02  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/21  23:39:16  vnguyen
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] modem busyout-rbs-dsx0-threshold
 *
 */
EOLS    (modem_busyout_rbs_dsx0_eol, modem_mgmt_command, MODEM_BUSYOUT_RBS_DSX0);
NUMBER  (modem_busyout_rbs_dsx0_num, modem_busyout_rbs_dsx0_eol, NONE,
         OBJ(int,1), MIN_FREE_MODEM_THRESHOLD, MAX_FREE_MODEM_THRESHOLD,
         "low water mark of free modems");
NOPREFIX (modem_busyout_rbs_dsx0_no, modem_busyout_rbs_dsx0_num,
          modem_busyout_rbs_dsx0_eol);
NVGENS  (modem_busyout_rbs_dsx0_nvgen, modem_busyout_rbs_dsx0_no,
         modem_mgmt_command, MODEM_BUSYOUT_RBS_DSX0);
KEYWORD (modem_busyout_rbs_dsx0, modem_busyout_rbs_dsx0_nvgen, NONE,
         "busyout-rbs-dsx0-threshold", "busyout rbs/dsx0 thrshold of free modems",
         PRIV_CONF | PRIV_HIDDEN);

/****************************************************************
 * [no] modem fast-answer
 *
 */
EOLS    (modem_fast_answer_eol, modem_mgmt_command, MODEM_FAST_ANSWER);
KEYWORD (modem_fast_answer, modem_fast_answer_eol, modem_busyout_rbs_dsx0 ,
	  "fast-answer", "Enable fast answering on modems", PRIV_CONF);

/***************************************************************
 * modem a-law
 *
 */
EOLS	(modem_a_law_eol, modem_mgmt_command, MODEM_A_LAW);
KEYWORD (modem_a_law, modem_a_law_eol, modem_fast_answer,
         "a-law", "Forcing modem to A-law",
         PRIV_CONF | PRIV_HIDDEN);

/***************************************************************
 * [no] modem startup-test
 *
 */
EOLS	(modem_startup_eol, modem_mgmt_command, MODEM_STARTUP_TEST);
KEYWORD (modem_startup_test, modem_startup_eol, modem_a_law,
         "startup-test", "Execute modem startup testing when system reloads",
         PRIV_CONF);

/***************************************************************
 * [no] modem recovery-time <min>
 *
 */
EOLS    (modem_recovery_time_eol, modem_mgmt_command, MODEM_RECOVERY_TIME); 
NUMBER  (modem_recovery_time_num, modem_recovery_time_eol, NONE, 
         OBJ(int,1), MIN_WDT_VALUE, MAX_WDT_VALUE, "Recovery time in minutes"); 
NOPREFIX (modem_recovery_time_no, modem_recovery_time_num, 
          modem_recovery_time_eol);
NVGENS  (modem_recovery_time_nvgen, modem_recovery_time_no, 
         modem_mgmt_command, MODEM_RECOVERY_TIME);
KEYWORD (modem_recovery_time, modem_recovery_time_nvgen, modem_startup_test,
         "recovery-time", "Interval before recovering hung modem", PRIV_CONF);

/***************************************************************
 * [no] modem buffer-size <number>
 *
 */
EOLS    (modem_buffer_size_eol, modem_mgmt_command, MODEM_BUFFER_SIZE); 
NUMBER  (modem_buffer_size_num, modem_buffer_size_eol, NONE, OBJ(int,1), 
         MIN_MODEM_BUFFER_SIZE, MAX_MODEM_BUFFER_SIZE, "Number of events"); 
NOPREFIX (modem_buffer_size_no, modem_buffer_size_num, modem_buffer_size_eol);
NVGENS  (modem_buffer_size_nvgen, modem_buffer_size_no, 
         modem_mgmt_command, MODEM_BUFFER_SIZE);
KEYWORD (modem_buffer_size, modem_buffer_size_nvgen, NONE,
         "buffer-size", "Size of modem history event buffer", PRIV_CONF);

/***************************************************************
 * [no] modem poll retry <number>
 *
 */
EOLS    (modem_poll_retry_eol, modem_mgmt_command, MODEM_POLL_RETRY); 
NUMBER  (modem_poll_retry_num, modem_poll_retry_eol, NONE, OBJ(int,1), 
         MIN_POLL_RETRY, MAX_POLL_RETRY, "Number of poll retry attempts"); 
NOPREFIX (modem_poll_retry_no, modem_poll_retry_num, modem_poll_retry_eol);
NVGENS  (modem_poll_retry_nvgen, modem_poll_retry_no, 
         modem_mgmt_command, MODEM_POLL_RETRY);
KEYWORD (modem_poll_retry, modem_poll_retry_nvgen, NONE,
         "retry", "Retry attempts for polling", PRIV_CONF);

/***************************************************************
 * [no] modem poll time <sec>
 *
 */
EOLS    (modem_poll_time_eol, modem_mgmt_command, MODEM_POLL_TIME); 
NUMBER  (modem_poll_time_num, modem_poll_time_eol, NONE, OBJ(int,1), 
         MIN_POLL_TIME, MAX_POLL_TIME, "Poll time in seconds"); 
NOPREFIX (modem_poll_time_no, modem_poll_time_num, modem_poll_time_eol);
NVGENS  (modem_poll_time_nvgen, modem_poll_time_no, 
         modem_mgmt_command, MODEM_POLL_TIME);
KEYWORD (modem_poll_time, modem_poll_time_nvgen, modem_poll_retry,
         "time", "Interval to poll modems status", PRIV_CONF);

KEYWORD (modem_poll, modem_poll_time, modem_buffer_size,
         "poll", "Modem out of band polling", PRIV_CONF);
/***************************************************************
 * check to see if it is manageable modem image, only certain
 * management commands are valid for manageable image
 */
ASSERT  (modem_manageable, modem_poll, modem_recovery_time, 
         reg_invoke_modem_mgmt_exists());

KEYWORD (modem_top, modem_manageable, ALTERNATE,
         "modem", "Modem Management configuration commands", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	modem_top
