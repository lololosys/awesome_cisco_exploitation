/* $Id: cfg_key.h,v 3.4.54.3 1996/08/12 16:00:57 widmer Exp $
 * $Source: /release/112/cvs/Xsys/crypto/cfg_key.h,v $
 *------------------------------------------------------------------
 * Global key management commands
 *
 * September 1995, Andy Heffernan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_key.h,v $
 * Revision 3.4.54.3  1996/08/12  16:00:57  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.54.2  1996/06/17  23:29:43  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.4.54.1  1996/04/19  14:55:26  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.2  1996/02/29  19:03:29  bew
 * Branch: IosSec_branch
 * Commit of the private/encrypted NVRAM feature. This is in support
 * of the crypto project and allows portions of NVRAM to be
 * encrypted, and a portion to be private key storage for the IOS.
 *
 * Revision 3.1.2.1  1995/12/17  06:22:50  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.4  1995/12/01  15:56:03  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.3  1995/11/18  01:59:53  ahh
 * CSCdi42411:  Key commands not NVGENed properly
 *
 * Revision 3.2  1995/11/17  08:56:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:12:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:53:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/11  19:35:24  pst
 * Add placeholders
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * key-string <password> | send-lifetime <lifetime> |
 *	accept-lifetime <lifetime>
 *
 * <lifetime> = hh:mm:ss {Mon dd yyyy | dd Mon yyyy} 
 *		{ hh:mm:ss {Mon dd yyyy | dd Mon yyyy} | infinite | <number> }
 * 
 * no { key-string | send-lifetime | accept-lifetime }
 *
 * OBJ(string,1) -- keystring
 * OBJ(int,1) -- encryption type
 *     -- or --
 * OBJ(int,1) -- accept or send
 * OBJ(int,2-7) -- start hh:mm:ss month day year
 * OBJ(int,8-13) -- stop hh:mm:ss month day year
 * OBJ(int,14) -- duration
 *
 */

enum {
    KEY_STRING,
    KEY_LIFETIME
};

EOLNS	(conf_kc_key_exit_eol, exit_keychain_config_submode);
KEYWORD	(conf_kc_key_exit_kwd, conf_kc_key_exit_eol, NONE,
	 "exit", "Exit from key-chain key configuration mode", PRIV_MIN);
TESTVAR	(conf_kc_key_exit, NONE, conf_kc_key_exit_kwd, NONE,
	 NONE, ALTERNATE, sense, FALSE);

EOLS    (conf_kc_key_lifetime_eol, key_chain_key_command, KEY_LIFETIME);

NUMBER  (conf_kc_key_lifetime2_year, conf_kc_key_lifetime_eol, no_alt,
	 OBJ(int,13), CLOCK_MIN_YEAR, CLOCK_MAX_YEAR,
	 "Year to stop");
NUMBER  (conf_kc_key_lifetime2_day_2, conf_kc_key_lifetime2_year, no_alt,
	 OBJ(int,12), 1, 31,
	 "Day of the month to stop");
SPLIT   (conf_kc_key_lifetime2_test, conf_kc_key_lifetime2_day_2,
	 conf_kc_key_lifetime2_year,
	 OBJ(int,12), 0);
MONTH   (conf_kc_key_lifetime2_month, conf_kc_key_lifetime2_test, no_alt,
	 OBJ(int,11),
	 "Month of the year to stop");
NUMBER  (conf_kc_key_lifetime2_day, conf_kc_key_lifetime2_month,
	 conf_kc_key_lifetime2_month,
	 OBJ(int,12), 1, 31,
	 "Day of the month to stop");
TIMESEC (conf_kc_key_lifetime2_time, conf_kc_key_lifetime2_day, no_alt,
	 OBJ(int,8), OBJ(int,9), OBJ(int,10),
	 "Time to stop");

PARAMS_KEYONLY(conf_kc_key_lifetime2_dur, conf_kc_key_lifetime2_time,
	 "duration", OBJ(int,14), 1, MAXINT-1,
	 key_chain_key_command, KEY_LIFETIME,
	 "Set key lifetime duration", "Seconds", PRIV_CONF);

KEYWORD_ID(conf_kc_key_lifetime2_inf, conf_kc_key_lifetime_eol,
	 conf_kc_key_lifetime2_dur,
	 OBJ(int,14), -1,
	 "infinite", "Never expires", PRIV_CONF);

NUMBER  (conf_kc_key_lifetime1_year, conf_kc_key_lifetime2_inf, no_alt,
	 OBJ(int,7), CLOCK_MIN_YEAR, CLOCK_MAX_YEAR,
	 "Year to start");
NUMBER  (conf_kc_key_lifetime1_day_2, conf_kc_key_lifetime1_year, no_alt,
	 OBJ(int,6), 1, 31,
	 "Day of the month to start");
SPLIT   (conf_kc_key_lifetime1_test, conf_kc_key_lifetime1_day_2,
	 conf_kc_key_lifetime1_year,
	 OBJ(int,6), 0);
MONTH   (conf_kc_key_lifetime1_month, conf_kc_key_lifetime1_test, no_alt,
	 OBJ(int,5),
	 "Month of the year to start");
NUMBER  (conf_kc_key_lifetime1_day, conf_kc_key_lifetime1_month,
	 conf_kc_key_lifetime1_month,
	 OBJ(int,6), 1, 31,
	 "Day of the month to start");
TIMESEC (conf_kc_key_lifetime1_time, conf_kc_key_lifetime1_day, no_alt,
	 OBJ(int,2), OBJ(int,3), OBJ(int,4),
	 "Time to start");

NOPREFIX(conf_kc_key_lifetime_noprefix, conf_kc_key_lifetime1_time,
	 conf_kc_key_lifetime_eol);
NVGENS	(conf_kc_key_lifetime_nvgen, conf_kc_key_lifetime_noprefix,
	 key_command_key_keyword_nvgen, KEY_LIFETIME);

KEYWORD_ID(conf_kc_key_slifetime, conf_kc_key_lifetime_nvgen,
	 conf_kc_key_exit,
	 OBJ(int,1), KEY_SEND_LIFETIME,
	 "send-lifetime", "Set send lifetime of key", PRIV_CONF);
KEYWORD_ID(conf_kc_key_alifetime, conf_kc_key_lifetime_nvgen,
	 conf_kc_key_slifetime,
	 OBJ(int,1), KEY_ACCEPT_LIFETIME,
	 "accept-lifetime", "Set accept lifetime of key", PRIV_CONF);

EOLS    (conf_kc_key_string_eol, key_chain_key_command, KEY_STRING);
TEXT	(conf_kc_key_string_passwd, conf_kc_key_string_eol, no_alt,
	 OBJ(string,1), "The key");
TEST_MULTIPLE_FUNCS(conf_kc_key_string_test, conf_kc_key_string_passwd,
	 no_alt, no_alt);
DECIMAL (conf_kc_key_string_enctype, conf_kc_key_string_passwd,
	 conf_kc_key_string_test,
	 OBJ(int,1), ENCRYPT_NONE, ENCRYPT_TEMP,
	 "Encryption type (0 to disable encryption, 7 for proprietary)");
NOPREFIX(conf_kc_key_noprefix, conf_kc_key_string_enctype,
	 conf_kc_key_string_eol);
NVGENS	(conf_kc_key_nvgen, conf_kc_key_noprefix,
	 key_command_key_keyword_nvgen, KEY_STRING);
KEYWORD_MM(conf_kc_key_string, conf_kc_key_nvgen, conf_kc_key_alifetime,
	 "key-string", "Set key string", PRIV_CONF, 4);

NO_OR_DEFAULT(conf_kc_key_no, conf_kc_key_string, PRIV_MIN | PRIV_NONVGEN);

HELP	(top_conf_kc_key, conf_kc_key_no,
	 "Key-chain key configuration commands:\n");

/******************************************************************************
 * [no] key <num>
 *
 * OBJ(int,1)
 */

enum {
    KEY_ID
};
#define MAX_KEY_ID	MAXINT

EOLNS	(conf_kc_exit_eol, exit_config_submode);
KEYWORD	(conf_kc_exit_kwd, conf_kc_exit_eol, NONE,
	 "exit", "Exit from key-chain configuration mode", PRIV_MIN);
TESTVAR	(conf_kc_exit, NONE, conf_kc_exit_kwd, NONE,
	 NONE, ALTERNATE, sense, FALSE);

EOLS    (conf_kc_key_id_eol, key_chain_command, KEY_ID);
NUMBER	(conf_kc_key_id_num, conf_kc_key_id_eol, NONE,
	 OBJ(int,1), 0, MAX_KEY_ID, "Key identifier");
NVGENS	(conf_kc_key_id_nvgen, conf_kc_key_id_num,
	 key_command_key_nvgen, KEY_ID);
KEYWORD (conf_kc_key_id, conf_kc_key_id_nvgen, conf_kc_exit,
	 "key", 
	 "Configure a key", PRIV_CONF);

NO_OR_DEFAULT(conf_kc_no, conf_kc_key_id, PRIV_MIN | PRIV_NONVGEN);

HELP	(top_conf_kc, conf_kc_no,
	 "Key-chain configuration commands:\n");

/******************************************************************************
 * [no] key config-key <key-number> <value>
 */

enum {
    KEY_CHAIN,
    KEY_CONFIG_KEY
};

EOLS    (conf_key_config_eol, key_command, KEY_CONFIG_KEY);
TEXT	(conf_key_config_value, conf_key_config_eol, no_alt,
	 OBJ(string,1), "The config key");
NOPREFIX(conf_key_config_no, conf_key_config_value, conf_key_config_eol);
NUMBER  (conf_key_config_num, conf_key_config_no, no_alt,
	 OBJ(int,2), 1, 3, "Configuration key number");
NVGENS	(conf_key_config_nvgen, conf_key_config_num,
	 key_command_nvgen, KEY_CONFIG_KEY);
KEYWORD	(conf_key_config, conf_key_config_nvgen, no_alt,
	 "config-key", "Set a private configuration key", PRIV_ROOT);

/******************************************************************************
 * [no] key { chain <name> }
 *
 * OBJ(string,1) -- key-chain name
 */

EOLS    (conf_key_chain_eol, key_command, KEY_CHAIN);
STRING  (conf_key_chain_name, conf_key_chain_eol, NONE,
	 OBJ(string,1), "Key-chain name");

NVGENS	(conf_key_chain_nvgen, conf_key_chain_name,
	 key_command_nvgen, KEY_CHAIN);
KEYWORD (conf_key_chain, conf_key_chain_nvgen, conf_key_config,
	 "chain", 
	 "Key-chain management", PRIV_CONF);

KEYWORD (configure_key, conf_key_chain, ALTERNATE,
	 "key", 
	 "Key management", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	configure_key
