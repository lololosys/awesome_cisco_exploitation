/* $Id: cfg_int_half_duplex.h,v 3.2.20.1 1996/06/17 08:35:15 jhernand Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_int_half_duplex.h,v $
 *------------------------------------------------------------------
 * cfg_int_half_duplex.h - Commands to configure half-duplex,
 *                         full-duplex, and related parameters.
 *
 * February, 1996, Bob Batz
 * Modification of Suresh Sangiah's version in 11.0.
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_half_duplex.h,v $
 * Revision 3.2.20.1  1996/06/17  08:35:15  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.2  1996/02/13  21:25:27  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.1  1996/02/06  17:10:52  rbatz
 * Placeholder for half-duplex changes
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Note: The "full-duplex" command is used for non-serial interfaces
 *       so this file should not be "serial-only".
 *  
 *         *** Visible configuration commands ***
 *
 * [no] half-duplex
 * [no] full-duplex
 *
 * [no] half-duplex timer rts-timeout <timer-val>
 * [no] half-duplex timer cts-delay <timer-val>
 * [no] half-duplex timer rts-drop-delay <timer-val>
 * [no] half-duplex timer cts-drop-timeout <timer-val>
 * [no] half-duplex timer dcd-txstart-delay <timer-val>
 * [no] half-duplex timer dcd-drop-delay <timer-val>
 * [no] half-duplex timer transmit-delay <timer-val>
 *
 * [no] half-duplex controlled-carrier
 *
 *         *** Hidden configuration commands ***
 *
 * [no] half-duplex test dsr
 * [no] half-duplex test dcd
 * [no] half-duplex test rts
 * [no] half-duplex test dtr
 * [no] half-duplex test cts
 * [no] half-duplex test loopback
 * [no] half-duplex test transmit <num>
 *
 * [no] half-duplex log enable
 * half-duplex log clear
 */

/*
 * full-duplex
 */
EOLS	(ci_full_duplex_eol, media_half_or_full_duplex_params_command, 
         CFG_HDX_SET_FULL_DUPLEX);
KEYWORD	(ci_full_duplex, ci_full_duplex_eol, NONE,
	 "full-duplex",  "Configure full-duplex operational mode", PRIV_CONF);

/*
 * half-duplex
 */
EOLS    (ci_half_duplex_eol,
         media_half_or_full_duplex_params_command,
         CFG_HDX_SET_HALF_DUPLEX);

/*
 * half-duplex log clear
 */
EOLS    (ci_half_duplex_log_clear_eol, 
         media_half_or_full_duplex_params_command,
         CFG_HDX_LOG_CLEAR);

KEYWORD (ci_half_duplex_log_clear,
         ci_half_duplex_log_clear_eol,
         no_alt, 
         "clear", "Clear the half-duplex log", PRIV_CONF | PRIV_HIDDEN);

/*
 * [no] half-duplex log enable
 */
EOLS	(ci_half_duplex_log_enable_eol,
         media_half_or_full_duplex_params_command,
	 CFG_HDX_LOG_ENABLE);

NOPREFIX (ci_half_duplex_log_enable_noprefix, 
	  ci_half_duplex_log_enable_eol,
	  ci_half_duplex_log_enable_eol);

KEYWORD	(ci_half_duplex_log_enable, ci_half_duplex_log_enable_noprefix,
	 ci_half_duplex_log_clear,
	 "enable", "Enable half-duplex logging", PRIV_CONF | PRIV_HIDDEN);

KEYWORD	(ci_half_duplex_log, ci_half_duplex_log_enable,
	 ci_half_duplex_eol,
	 "log", "Half-duplex logging commands", PRIV_CONF | PRIV_HIDDEN);

/*
 * Check to see if the half-duplex event logging commands are supported
 * on this serial interface. Currently, this is supported only on the
 * Cirrus (CD2430) serial interfaces.
 */
IFELSE	(ci_half_duplex_log_check,
	 ci_half_duplex_log,
	 ci_half_duplex_eol,
	 csb->interface->hwptr->type == IDBTYPE_CD2430);

/*
 * half-duplex test transmit <val>
 */
EOLS	(ci_half_duplex_test_tx_eol,
         media_half_or_full_duplex_params_command,
	 CFG_HDX_TEST_TRANSMIT);

NUMBER	(ci_half_duplex_test_tx_num, ci_half_duplex_test_tx_eol,
	 ci_half_duplex_test_tx_eol,
	 OBJ(int, 1), 1, 1000, "Number of packets to transmit");

NOPREFIX (ci_half_duplex_test_tx_noprefix, ci_half_duplex_test_tx_num,
          ci_half_duplex_test_tx_eol);

KEYWORD (ci_half_duplex_test_tx, ci_half_duplex_test_tx_noprefix,
         no_alt,
         "transmit", "Test packet transmission", PRIV_CONF | PRIV_HIDDEN);

/*
 * [no] half-duplex test loopback
 */
EOLS    (ci_half_duplex_test_loop_eol,
         media_half_or_full_duplex_params_command,
         CFG_HDX_TEST_LOOPBACK);
 
NOPREFIX (ci_half_duplex_test_loop_noprefix,
          ci_half_duplex_test_loop_eol,
          ci_half_duplex_test_loop_eol);
 
KEYWORD (ci_half_duplex_test_loop, ci_half_duplex_test_loop_noprefix,
         ci_half_duplex_test_tx,
         "loopback", "Test loopback", PRIV_CONF | PRIV_HIDDEN);

/*
 * [no] half-duplex test dcd
 */
EOLS    (ci_half_duplex_test_dcd_eol,
         media_half_or_full_duplex_params_command,
         CFG_HDX_TEST_DCD);
 
NOPREFIX (ci_half_duplex_test_dcd_noprefix,
          ci_half_duplex_test_dcd_eol,
          ci_half_duplex_test_dcd_eol);
 
KEYWORD (ci_half_duplex_test_dcd, ci_half_duplex_test_dcd_noprefix,
         ci_half_duplex_test_loop,
         "dcd", "Test DCD", PRIV_CONF | PRIV_HIDDEN);

/*
 * [no] half-duplex test dsr
 */
EOLS    (ci_half_duplex_test_dsr_eol,
         media_half_or_full_duplex_params_command,
         CFG_HDX_TEST_DSR);
 
NOPREFIX (ci_half_duplex_test_dsr_noprefix,
          ci_half_duplex_test_dsr_eol,
          ci_half_duplex_test_dsr_eol);
 
KEYWORD (ci_half_duplex_test_dsr, ci_half_duplex_test_dsr_noprefix,
         ci_half_duplex_test_dcd,
         "dsr", "Test DSR", PRIV_CONF | PRIV_HIDDEN);

/*
 * [no] half-duplex test dtr
 */
EOLS    (ci_half_duplex_test_dtr_eol,
         media_half_or_full_duplex_params_command,
         CFG_HDX_TEST_DTR);
 
NOPREFIX (ci_half_duplex_test_dtr_noprefix,
          ci_half_duplex_test_dtr_eol,
          ci_half_duplex_test_dtr_eol);
 
KEYWORD (ci_half_duplex_test_dtr, ci_half_duplex_test_dtr_noprefix,
         ci_half_duplex_test_dsr,
         "dtr", "Test DTR", PRIV_CONF | PRIV_HIDDEN);

/*
 * [no] half-duplex test cts
 */
EOLS    (ci_half_duplex_test_cts_eol,
         media_half_or_full_duplex_params_command,
         CFG_HDX_TEST_CTS);
 
NOPREFIX (ci_half_duplex_test_cts_noprefix,
          ci_half_duplex_test_cts_eol,
          ci_half_duplex_test_cts_eol);
 
KEYWORD (ci_half_duplex_test_cts, ci_half_duplex_test_cts_noprefix,
         ci_half_duplex_test_dtr,
         "cts", "Test CTS", PRIV_CONF | PRIV_HIDDEN);

/*
 * [no] half-duplex test rts
 */
EOLS    (ci_half_duplex_test_rts_eol,
         media_half_or_full_duplex_params_command,
         CFG_HDX_TEST_RTS);
 
NOPREFIX (ci_half_duplex_test_rts_noprefix,
          ci_half_duplex_test_rts_eol,
          ci_half_duplex_test_rts_eol);
 
KEYWORD (ci_half_duplex_test_rts, ci_half_duplex_test_rts_noprefix,
         ci_half_duplex_test_cts,
         "rts", "Test RTS", PRIV_CONF | PRIV_HIDDEN);

KEYWORD	(ci_half_duplex_test, ci_half_duplex_test_rts,
	 ci_half_duplex_log_check,
	 "test", "Test and debug commands", PRIV_CONF | PRIV_HIDDEN);

/*
 * Check to see if the hidden test commands are available on this serial
 * interface. Currently, only the Cirrus (CD2430) serial interfaces allow
 * this option.
 */
IFELSE	(ci_half_duplex_test_check,
	 ci_half_duplex_test,
	 ci_half_duplex_log_check,
	 csb->interface->hwptr->type == IDBTYPE_CD2430);

/*
 * [no] half-duplex controlled-carrier
 */
EOLS	(ci_half_duplex_contr_carrier_eol,
         media_half_or_full_duplex_params_command,
	 CFG_HDX_CONTROLLED_CARRIER);

NOPREFIX (ci_half_duplex_contr_carrier_noprefix,
	  ci_half_duplex_contr_carrier_eol,
	  ci_half_duplex_contr_carrier_eol);

KEYWORD	(ci_half_duplex_contr_carrier, ci_half_duplex_contr_carrier_noprefix,
	 ci_half_duplex_test_check,
	 "controlled-carrier", "Enable controlled carrier operation", 
         PRIV_CONF); 

/*
 * Test to see if the controlled-carrier option is available on this 
 * serial interface. Currently, only the Cirrus (CD2430) serial interfaces
 * support this option.
 */
IFELSE	(ci_half_duplex_contr_carrier_check,
	 ci_half_duplex_contr_carrier,
	 ci_half_duplex_test_check,
	 csb->interface->hwptr->type == IDBTYPE_CD2430);

/*
 * [no] half-duplex timer transmit-delay <val>
 */
EOLS    (ci_half_duplex_timer_transmit_delay_eol,
         media_half_or_full_duplex_params_command,
         CFG_HDX_TRANSMIT_DELAY);
 
NUMBER  (ci_half_duplex_timer_transmit_delay_val,
         ci_half_duplex_timer_transmit_delay_eol, no_alt,
         OBJ(int, 1), 0, 4400, "Value in milliseconds");
 
NOPREFIX (ci_half_duplex_timer_transmit_delay_noprefix,
          ci_half_duplex_timer_transmit_delay_val,
          ci_half_duplex_timer_transmit_delay_eol);
 
KEYWORD (ci_half_duplex_timer_transmit_delay,
         ci_half_duplex_timer_transmit_delay_noprefix,
         no_alt,
         "transmit-delay",
         "Time delay prior to start of transmission",
         PRIV_CONF);

/*
 * [no] half-duplex timer dcd-drop-delay <val>
 */
EOLS    (ci_half_duplex_timer_dcd_drop_delay_eol, 
         media_half_or_full_duplex_params_command,
         CFG_HDX_DCD_DROP_DELAY);
 
NUMBER  (ci_half_duplex_timer_dcd_drop_delay_val,
         ci_half_duplex_timer_dcd_drop_delay_eol, no_alt,
         OBJ(int, 1), 0, 4400, "Value in milliseconds");
 
NOPREFIX (ci_half_duplex_timer_dcd_drop_delay_noprefix, 
          ci_half_duplex_timer_dcd_drop_delay_val,
          ci_half_duplex_timer_dcd_drop_delay_eol);
 
KEYWORD (ci_half_duplex_timer_dcd_drop_delay, 
	 ci_half_duplex_timer_dcd_drop_delay_noprefix,
         ci_half_duplex_timer_transmit_delay,
         "dcd-drop-delay", 
         "Time delay between end of transmission and deassertion of DCD", 
         PRIV_CONF);

/*
 * [no] half-duplex timer dcd-txstart-delay <val>
 */
EOLS    (ci_half_duplex_timer_dcd_txstart_delay_eol, 
         media_half_or_full_duplex_params_command,
         CFG_HDX_DCD_TXSTART_DELAY);
 
NUMBER  (ci_half_duplex_timer_dcd_txstart_delay_val,
         ci_half_duplex_timer_dcd_txstart_delay_eol, no_alt,
         OBJ(int, 1), 0, 4400, "Value in milliseconds");
 
NOPREFIX (ci_half_duplex_timer_dcd_txstart_delay_noprefix, 
          ci_half_duplex_timer_dcd_txstart_delay_val,
          ci_half_duplex_timer_dcd_txstart_delay_eol);
 
KEYWORD (ci_half_duplex_timer_dcd_txstart_delay, 
	 ci_half_duplex_timer_dcd_txstart_delay_noprefix,
         ci_half_duplex_timer_dcd_drop_delay,
         "dcd-txstart-delay", 
         "Time delay between assertion of DCD and start of transmission", 
         PRIV_CONF);

/*
 * [no] half-duplex timer cts-drop-timeout <val>
 */
EOLS    (ci_half_duplex_timer_cts_drop_timeout_eol, 
         media_half_or_full_duplex_params_command,
         CFG_HDX_CTS_DROP_TIMEOUT);

NUMBER  (ci_half_duplex_timer_cts_drop_timeout_val,
         ci_half_duplex_timer_cts_drop_timeout_eol, no_alt,
         OBJ(int, 1), 0, 1140000, "Value in milliseconds");

NOPREFIX (ci_half_duplex_timer_cts_drop_timeout_noprefix, 
          ci_half_duplex_timer_cts_drop_timeout_val,
          ci_half_duplex_timer_cts_drop_timeout_eol);

KEYWORD (ci_half_duplex_timer_cts_drop_timeout, 
	 ci_half_duplex_timer_cts_drop_timeout_noprefix,
         ci_half_duplex_timer_dcd_txstart_delay,
         "cts-drop-timeout", "CTS drop timeout value", PRIV_CONF);

/*
 * [no] half-duplex timer rts-drop-delay <val>
 */
EOLS    (ci_half_duplex_timer_rts_drop_delay_eol, 
         media_half_or_full_duplex_params_command,
         CFG_HDX_RTS_DROP_DELAY);

NUMBER  (ci_half_duplex_timer_rts_drop_delay_val,
         ci_half_duplex_timer_rts_drop_delay_eol, no_alt,
         OBJ(int, 1), 0, 1140000, "Value in milliseconds");

NOPREFIX (ci_half_duplex_timer_rts_drop_delay_noprefix, 
	  ci_half_duplex_timer_rts_drop_delay_val,
          ci_half_duplex_timer_rts_drop_delay_eol);

KEYWORD (ci_half_duplex_timer_rts_drop_delay, 
	 ci_half_duplex_timer_rts_drop_delay_noprefix,
	 ci_half_duplex_timer_cts_drop_timeout,
         "rts-drop-delay", "RTS drop delay value", PRIV_CONF);
/*
 * The rest of the half-duplex timer commands apply currently only to
 * the Cirrus (CD2430) serial interfaces for which the half-duplex state 
 * machine implementation is such that these additional timers are defined 
 * and made tunable. The state machine is documented in the user manuals 
 * where the meanings of all these timers is explained.
 * 
 * Hence, commands to tune these timers will be blocked on other serial
 * interfaces where they are not supported. 
 */
IFELSE	(ci_half_duplex_all_timers_supported_check,
	 ci_half_duplex_timer_rts_drop_delay,
	 no_alt,
	 csb->interface->hwptr->type == IDBTYPE_CD2430);

/*
 * [no] half-duplex timer cts-delay <val>
 */
EOLS    (ci_half_duplex_timer_cts_delay_eol,
         media_half_or_full_duplex_params_command,
         CFG_HDX_CTS_DELAY);

NUMBER	(ci_half_duplex_timer_cts_delay_val_other, 
	 ci_half_duplex_timer_cts_delay_eol, no_alt,
	 OBJ(int, 1), 0, 1140000, "Value in milliseconds");

NUMBER  (ci_half_duplex_timer_cts_delay_val_mci,
         ci_half_duplex_timer_cts_delay_eol, no_alt,
         OBJ(int, 1), 0, 290, "Value in milliseconds");

NUMBER	(ci_half_duplex_timer_cts_delay_val_hitachi,
	 ci_half_duplex_timer_cts_delay_eol, no_alt,
	 OBJ(int, 1), 0, 425, "Value in milliseconds");

IFELSE	(ci_half_duplex_timer_cts_delay_not_hitachi,
         ci_half_duplex_timer_cts_delay_val_mci,
         ci_half_duplex_timer_cts_delay_val_other,         
	 csb->interface->hwptr->type == IDBTYPE_MCISERIAL);

IFELSE	(ci_half_duplex_timer_cts_delay_val,
         ci_half_duplex_timer_cts_delay_val_hitachi,
         ci_half_duplex_timer_cts_delay_not_hitachi,         
	 csb->interface->hwptr->type == IDBTYPE_HD64570);

NOPREFIX (ci_half_duplex_timer_cts_delay_noprefix, 
	  ci_half_duplex_timer_cts_delay_val,
          ci_half_duplex_timer_cts_delay_eol);

KEYWORD (ci_half_duplex_timer_cts_delay, 
	 ci_half_duplex_timer_cts_delay_noprefix,
	 ci_half_duplex_all_timers_supported_check,
         "cts-delay", "CTS delay value", PRIV_CONF);

/*
 * [no] half-duplex timer rts-timeout <val>
 */
EOLS	(ci_half_duplex_timer_rts_timeout_eol, 
         media_half_or_full_duplex_params_command,
	 CFG_HDX_RTS_TIMEOUT);

NUMBER	(ci_half_duplex_timer_rts_timeout_val_other, 
	 ci_half_duplex_timer_rts_timeout_eol, no_alt,
	 OBJ(int, 1), 0, 1140000, "Value in milliseconds");

NUMBER	(ci_half_duplex_timer_rts_timeout_val_mci,
	 ci_half_duplex_timer_rts_timeout_eol, no_alt,
	 OBJ(int, 1), 0, 290, "Value in milliseconds");

NUMBER	(ci_half_duplex_timer_rts_timeout_val_hitachi,
	 ci_half_duplex_timer_rts_timeout_eol, no_alt,
	 OBJ(int, 1), 0, 425, "Value in milliseconds");

IFELSE	(ci_half_duplex_timer_rts_timeout_not_hitachi,
         ci_half_duplex_timer_rts_timeout_val_mci,
         ci_half_duplex_timer_rts_timeout_val_other,         
	 csb->interface->hwptr->type == IDBTYPE_MCISERIAL);

IFELSE	(ci_half_duplex_timer_rts_timeout_val,
         ci_half_duplex_timer_rts_timeout_val_hitachi,
         ci_half_duplex_timer_rts_timeout_not_hitachi,         
	 csb->interface->hwptr->type == IDBTYPE_HD64570);

NOPREFIX (ci_half_duplex_timer_rts_timeout_noprefix, 
	  ci_half_duplex_timer_rts_timeout_val,
	  ci_half_duplex_timer_rts_timeout_eol);

KEYWORD	(ci_half_duplex_timer_rts_timeout, 
	 ci_half_duplex_timer_rts_timeout_noprefix,
	 ci_half_duplex_timer_cts_delay,
	 "rts-timeout", "RTS timeout value", PRIV_CONF);

KEYWORD	(ci_half_duplex_timer, ci_half_duplex_timer_rts_timeout,
	 ci_half_duplex_contr_carrier_check,
	 "timer", "Half-duplex timer tuning commands", PRIV_CONF);

IFELSE	(ci_half_duplex_serial_check,
         ci_half_duplex_timer,
         ci_half_duplex_eol,         
	 csb->interface->hwptr->status & IDB_SERIAL);

/* 
 * Do all the NVGENS for half-duplex up front.  That way we can control
 * the order in which things are written to nvram.
 */
NVGENS	(ci_half_duplex_nvgen, 
         ci_half_duplex_serial_check,
         media_half_or_full_duplex_params_command,
	 CFG_HDX_SET_HALF_DUPLEX);

KEYWORD	(ci_half_duplex, ci_half_duplex_nvgen, ci_full_duplex,
	 "half-duplex", "Configure half-duplex and related commands", PRIV_CONF);

ASSERT	(ci_half_duplex_assert, ci_half_duplex, ALTERNATE,
         ((csb->interface->hwptr->status & IDB_SERIAL)  ||
          (csb->interface->hwptr->type == IDBTYPE_FEIP)) );

#undef ALTERNATE
#define ALTERNATE ci_half_duplex_assert
