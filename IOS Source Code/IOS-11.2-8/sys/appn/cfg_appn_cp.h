/* 
 * $Id: cfg_appn_cp.h,v 3.1.40.2 1996/08/12 16:00:15 widmer Exp $
 * $Source: /release/112/cvs/Xsys/appn/cfg_appn_cp.h,v $
 *------------------------------------------------------------------
 *
 * Configuration Commands for APPN
 *
 * August 1994, Dave McCowan
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 *
 * $Log: cfg_appn_cp.h,v $
 * Revision 3.1.40.2  1996/08/12  16:00:15  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.40.1  1996/05/17  10:39:09  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.26.1  1996/03/30  01:35:40  pleung
 * Branch: IbuMod_Calif_branch
 * Central Resource Registration (CRR)
 *
 * Revision 3.1  1996/02/01  04:56:46  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1995/12/01  15:56:49  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  18:42:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:49:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/31  19:46:58  dmccowan
 * CSCdi36614:  add memory options to appn
 * add minimum-memory control-point configuration command
 *
 * Revision 2.2  1995/07/27  22:29:50  dmccowan
 * CSCdi36612:  minor problems with appn commands
 *
 * Revision 2.1  1995/06/07  22:01:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
*/

/******************************************************************
 * appn control-point <cpname>
 *    ...subcommands...
 */

/*
 *   exit
 */
EOLNS	(appn_cp_exit_eol, exit_appn_command);
KEYWORD	(appn_cp_exit_kwd, appn_cp_exit_eol, NONE,
	 "exit", "Exit from appn-control-point configuration mode", PRIV_CONF);
TESTVAR	(appn_cp_exit, NONE, appn_cp_exit_kwd, NONE, NONE, ALTERNATE,
	 sense, FALSE);
#undef	ALTERNATE
#define ALTERNATE appn_cp_exit


/*
 *   [no] complete
 */
EOLS      (cfg_appn_cp_comp_eol, appn_command, APPN_CP_COMP);
KEYWORD_ID(cfg_appn_cp_comp, cfg_appn_cp_comp_eol, ALTERNATE,
           appn_subcommand_type, APPN_CP_COMP,
           "complete", "Complete control point definition.", PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_comp

/*
 *   [no] crr
 */
EOLS      (cfg_appn_cp_crr_eol, appn_command, APPN_CP_CRR);
KEYWORD_ID(cfg_appn_cp_crr, cfg_appn_cp_crr_eol, ALTERNATE,
           appn_subcommand_type, APPN_CP_CRR,
           "crr", "Central Resource Registration", PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_crr

/*
 *    [no] dlur max-pus <max # of PUs supported>
 */
EOLS	  (cfg_appn_cp_dlur_eol, appn_command, APPN_CP_DLUR);
NUMBER    (cfg_dlur_maxpus_num, cfg_appn_cp_dlur_eol, no_alt,
           OBJ(int,1), 1, 100000, APPN_DLUR_MAXPUS_VAL_HELP);
KEYWORD   (cfg_dlur_maxpus, cfg_dlur_maxpus_num, 
           cfg_appn_cp_dlur_eol,
           APPN_DLUR_MAXPUS_KEY,
           APPN_DLUR_MAXPUS_KEY_HELP, PRIV_CONF);
NOPREFIX  (cfg_dlur_maxpus_noprefix, cfg_dlur_maxpus, 
	   cfg_appn_cp_dlur_eol);
NVGENS    (cfg_appn_cp_dlur_nvgen, cfg_dlur_maxpus_noprefix,
           appn_command, APPN_CP_DLUR);
KEYWORD_ID(cfg_appn_cp_dlur, cfg_appn_cp_dlur_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_CP_DLUR,
           APPN_CP_DLUR_KEY, 
           APPN_CP_DLUR_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_dlur

/*
 *    [no] bkup-dlus <bkup-dlus-name>
 */
EOLS	  (cfg_appn_cp_bkup_dlus_eol, appn_command, APPN_CP_BKUP_DLUS);
APPN_FQNAME(cfg_appn_cp_bkup_dlus_name, cfg_appn_cp_bkup_dlus_eol, no_alt,
            OBJ(string,1), APPN_CP_BKUP_DLUS_VAL_HELP);
NOPREFIX  (cfg_appn_cp_bkup_dlus_noprefix, cfg_appn_cp_bkup_dlus_name, 
					cfg_appn_cp_bkup_dlus_eol);
NVGENS    (cfg_appn_cp_bkup_dlus_nvgen, cfg_appn_cp_bkup_dlus_noprefix,
           appn_command, APPN_CP_BKUP_DLUS);
KEYWORD_ID(cfg_appn_cp_bkup_dlus, cfg_appn_cp_bkup_dlus_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_CP_BKUP_DLUS,
           APPN_CP_BKUP_DLUS_KEY, 
           APPN_CP_BKUP_DLUS_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_bkup_dlus

/*
 *    [no] dlus <dlus-name>
 */
EOLS	  (cfg_appn_cp_dlus_eol, appn_command, APPN_CP_DLUS);
APPN_FQNAME(cfg_appn_cp_dlus_name, cfg_appn_cp_dlus_eol, no_alt,
            OBJ(string,1), APPN_CP_DLUS_VAL_HELP);
NOPREFIX  (cfg_appn_cp_dlus_noprefix, cfg_appn_cp_dlus_name, 
					cfg_appn_cp_dlus_eol);
NVGENS    (cfg_appn_cp_dlus_nvgen, cfg_appn_cp_dlus_noprefix,
           appn_command, APPN_CP_DLUS);
KEYWORD_ID(cfg_appn_cp_dlus, cfg_appn_cp_dlus_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_CP_DLUS,
           APPN_CP_DLUS_KEY, 
           APPN_CP_DLUS_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_dlus


/*
 *    [no] buffer-percent <percentage>
 */
EOLS	  (cfg_appn_cp_buf_percent_eol, appn_command, APPN_CP_BUF_PERCENT);
NUMBER    (cfg_appn_cp_buf_percent_num, cfg_appn_cp_buf_percent_eol, no_alt,
           OBJ(int,3), 1, 100, APPN_CP_BUF_PERCENT_VAL_HELP);
NOPREFIX  (cfg_appn_cp_buf_percent_noprefix, cfg_appn_cp_buf_percent_num, 
	   cfg_appn_cp_buf_percent_eol);
NVGENS    (cfg_appn_cp_buf_percent_nvgen, cfg_appn_cp_buf_percent_noprefix,
           appn_command, APPN_CP_BUF_PERCENT);
KEYWORD_ID(cfg_appn_cp_buf_percent, cfg_appn_cp_buf_percent_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_CP_BUF_PERCENT,
           APPN_CP_BUF_PERCENT_KEY, 
           APPN_CP_BUF_PERCENT_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_buf_percent

/*
 *    [no] trace-settings <flags>
 */
EOLS	  (cfg_appn_trace_flags_eol, appn_command, APPN_TRACE_FLAGS);
HEXNUM    (cfg_appn_trace_flags_num, cfg_appn_trace_flags_eol, no_alt,
           OBJ(int,3), APPN_TRACE_FLAGS_VAL_HELP);
NOPREFIX  (cfg_appn_trace_flags_noprefix, cfg_appn_trace_flags_num, 
	   cfg_appn_trace_flags_eol);
NVGENS    (cfg_appn_trace_flags_nvgen, cfg_appn_trace_flags_noprefix,
           appn_command, APPN_TRACE_FLAGS);
KEYWORD_ID(cfg_appn_trace_flags, cfg_appn_trace_flags_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_TRACE_FLAGS,
           APPN_TRACE_FLAGS_KEY, 
           APPN_TRACE_FLAGS_KEY_HELP, PRIV_CONF|PRIV_HIDDEN);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_trace_flags

/*
 *    [no] maximum-memory <memory>
 */
EOLS	  (cfg_appn_cp_mem_eol, appn_command, APPN_CP_MEM);
NUMBER    (cfg_appn_cp_mem_num, cfg_appn_cp_mem_eol, no_alt,
           OBJ(int,3), 3000000, 128000000, APPN_CP_MEM_VAL_HELP);
NOPREFIX  (cfg_appn_cp_mem_noprefix, cfg_appn_cp_mem_num, 
	   cfg_appn_cp_mem_eol);
NVGENS    (cfg_appn_cp_mem_nvgen, cfg_appn_cp_mem_noprefix,
           appn_command, APPN_CP_MEM);
KEYWORD_ID(cfg_appn_cp_mem, cfg_appn_cp_mem_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_CP_MEM,
           APPN_CP_MEM_KEY, 
           APPN_CP_MEM_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_mem

/*
 *    [no] minimum-memory <memory>
 */
EOLS	  (cfg_appn_cp_min_eol, appn_command, APPN_CP_MIN);
NUMBER    (cfg_appn_cp_min_num, cfg_appn_cp_min_eol, no_alt,
           OBJ(int,3), 1000000, 64000000, APPN_CP_MIN_VAL_HELP);
NOPREFIX  (cfg_appn_cp_min_noprefix, cfg_appn_cp_min_num, 
	   cfg_appn_cp_min_eol);
NVGENS    (cfg_appn_cp_min_nvgen, cfg_appn_cp_min_noprefix,
           appn_command, APPN_CP_MIN);
KEYWORD_ID(cfg_appn_cp_min, cfg_appn_cp_min_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_CP_MIN,
           APPN_CP_MIN_KEY, 
           APPN_CP_MIN_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_min

/*
 *    [no] interrupt_switched
 */
EOLS      (cfg_appn_cp_intr_sw_eol, appn_command, APPN_CP_INTR_SW);
KEYWORD_ID(cfg_appn_cp_intr_sw, cfg_appn_cp_intr_sw_eol,ALTERNATE,
           appn_subcommand_type, APPN_CP_INTR_SW,
           APPN_CP_INTR_SW_KEY, 
	   APPN_CP_INTR_SW_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_intr_sw

/*
 *    [no] max-cached-trees <trees>
 */
EOLS	  (cfg_appn_cp_trees_eol, appn_command, APPN_CP_TREES);
NUMBER    (cfg_appn_cp_trees_num, cfg_appn_cp_trees_eol, no_alt,
           OBJ(int,3), 0, 32767, APPN_CP_TREES_VAL_HELP);
NOPREFIX  (cfg_appn_cp_trees_noprefix, cfg_appn_cp_trees_num, cfg_appn_cp_trees_eol);
NVGENS    (cfg_appn_cp_trees_nvgen, cfg_appn_cp_trees_noprefix,
           appn_command, APPN_CP_TREES);
KEYWORD_ID(cfg_appn_cp_trees, cfg_appn_cp_trees_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_CP_TREES,
           APPN_CP_TREES_KEY, 
           APPN_CP_TREES_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_trees

/*
 *    [no] safe-store-host ip-address <host ip address> directory <file path>
 */
EOLS	  (cfg_appn_cp_store_host_eol, appn_command, APPN_CP_CYCLES);
STRING    (cfg_appn_cp_store_host_path, cfg_appn_cp_store_host_eol, no_alt,
           OBJ(string,1), APPN_CP_HOST_DIR_VAL_HELP);
KEYWORD   (cfg_appn_cp_store_host_dir, cfg_appn_cp_store_host_path, no_alt, 
	   APPN_CP_HOST_DIR_KEY,
	   APPN_CP_HOST_DIR_KEY_HELP, PRIV_CONF);
IPADDR	  (cfg_appn_cp_store_host_ipaddr, cfg_appn_cp_store_host_dir, no_alt,
	   OBJ(paddr,1), APPN_CP_HOST_IP_VAL_HELP);
KEYWORD   (cfg_appn_cp_store_host_addr, cfg_appn_cp_store_host_ipaddr, no_alt, 
	   APPN_CP_HOST_IP_KEY,
	   APPN_CP_HOST_IP_KEY_HELP, PRIV_CONF);
NOPREFIX  (cfg_appn_cp_store_host_noprefix, cfg_appn_cp_store_host_addr, 
	   cfg_appn_cp_store_host_eol);
NVGENS    (cfg_appn_cp_store_host_nvgen, cfg_appn_cp_store_host_noprefix,
           appn_command, APPN_CP_HOST);
KEYWORD_ID(cfg_appn_cp_store_host, cfg_appn_cp_store_host_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_CP_HOST,
           APPN_CP_HOST_KEY, 
           APPN_CP_HOST_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_store_host

/*
 *    [no] safe-store-cycles <number of cycles>
 */
EOLS	  (cfg_appn_cp_store_cycle_eol, appn_command, APPN_CP_CYCLES);
NUMBER    (cfg_appn_cp_store_cycle_num, cfg_appn_cp_store_cycle_eol, no_alt,
           OBJ(int,3), 1, 255, APPN_CP_CYCLES_VAL_HELP);
NOPREFIX  (cfg_appn_cp_store_cycle_noprefix, cfg_appn_cp_store_cycle_num, 
	   cfg_appn_cp_store_cycle_eol);
NVGENS    (cfg_appn_cp_store_cycle_nvgen, cfg_appn_cp_store_cycle_noprefix,
           appn_command, APPN_CP_CYCLES);
KEYWORD_ID(cfg_appn_cp_store_cycle, cfg_appn_cp_store_cycle_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_CP_CYCLES,
           APPN_CP_CYCLES_KEY, 
           APPN_CP_CYCLES_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_store_cycle

/*
 *    [no] save-store-interval <interval>
 */
EOLS	  (cfg_appn_cp_store_eol, appn_command, APPN_CP_STORE);
NUMBER    (cfg_appn_cp_store_num, cfg_appn_cp_store_eol, no_alt,
           OBJ(int,3), 0, 32767, APPN_CP_STORE_VAL_HELP);
NOPREFIX  (cfg_appn_cp_store_noprefix, cfg_appn_cp_store_num, cfg_appn_cp_store_eol);
NVGENS    (cfg_appn_cp_store_nvgen, cfg_appn_cp_store_noprefix,
           appn_command, APPN_CP_STORE);
KEYWORD_ID(cfg_appn_cp_store, cfg_appn_cp_store_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_CP_STORE,
           APPN_CP_STORE_KEY, 
           APPN_CP_STORE_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_store

/*
 *    [no] max-cache-entries <cache>
 */
EOLS	  (cfg_appn_cp_cache_eol, appn_command, APPN_CP_CACHE);
NUMBER    (cfg_appn_cp_cache_num, cfg_appn_cp_cache_eol, no_alt,
           OBJ(int,3), 0, 32767, APPN_CP_CACHE_VAL_HELP);
NOPREFIX  (cfg_appn_cp_cache_noprefix, cfg_appn_cp_cache_num, cfg_appn_cp_cache_eol);
NVGENS    (cfg_appn_cp_cache_nvgen, cfg_appn_cp_cache_noprefix,
           appn_command, APPN_CP_CACHE);
KEYWORD_ID(cfg_appn_cp_cache, cfg_appn_cp_cache_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_CP_CACHE,
           APPN_CP_CACHE_KEY, 
           APPN_CP_CACHE_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_cache

/*
 *    [no] route-additional-resistance <resist>
 */
EOLS	  (cfg_appn_cp_resist_eol, appn_command, APPN_CP_RESIST);
NUMBER    (cfg_appn_cp_resist_num, cfg_appn_cp_resist_eol, no_alt,
           OBJ(int,3), 0, 255, APPN_CP_RESIST_VAL_HELP);
NOPREFIX  (cfg_appn_cp_resist_noprefix, cfg_appn_cp_resist_num, cfg_appn_cp_resist_eol);
NVGENS    (cfg_appn_cp_resist_nvgen, cfg_appn_cp_resist_noprefix,
           appn_command, APPN_CP_RESIST);
KEYWORD_ID(cfg_appn_cp_resist, cfg_appn_cp_resist_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_CP_RESIST,
           APPN_CP_RESIST_KEY, 
           APPN_CP_RESIST_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_resist

/*
 *    [no] xid-id-number <nodeid>
 */
EOLS	  (cfg_appn_cp_xid_eol, appn_command, APPN_CP_XID);
HEXDIGIT  (cfg_appn_cp_xid_xid, cfg_appn_cp_xid_eol, no_alt,
           OBJ(int,3), 0, 0xFFFFF, APPN_CP_XID_VAL_HELP);
NOPREFIX  (cfg_appn_cp_xid_noprefix, cfg_appn_cp_xid_xid, cfg_appn_cp_xid_eol);
NVGENS    (cfg_appn_cp_xid_nvgen, cfg_appn_cp_xid_noprefix,
           appn_command, APPN_CP_XID);
KEYWORD_ID(cfg_appn_cp_xid, cfg_appn_cp_xid_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_CP_XID,
           APPN_CP_XID_KEY, APPN_CP_XID_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_xid

/*
 *    [no] xid-block-number <blockid>
 */
EOLS	  (cfg_appn_cp_block_eol, appn_command, APPN_CP_BLOCK);
HEXDIGIT  (cfg_appn_cp_block_num, cfg_appn_cp_block_eol, no_alt,
           OBJ(int,3), 0, 0xFFF, APPN_CP_BLOCK_VAL_HELP);
NOPREFIX  (cfg_appn_cp_block_noprefix, cfg_appn_cp_block_num, cfg_appn_cp_block_eol);
NVGENS    (cfg_appn_cp_block_nvgen, cfg_appn_cp_block_noprefix,
           appn_command, APPN_CP_BLOCK);
KEYWORD_ID(cfg_appn_cp_block, cfg_appn_cp_block_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_CP_BLOCK,
           APPN_CP_BLOCK_KEY, APPN_CP_BLOCK_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_block

/*
 *    [no] dlus-retry-interval <seconds>
 */
EOLS    (cfg_appn_cp_dlus_retry_int_eol, appn_command, APPN_CP_DLUS_RETRY_INT);
NUMBER    (cfg_appn_cp_dlus_retry_int_num, cfg_appn_cp_dlus_retry_int_eol, no_alt,
           OBJ(int,3), 5, 600, APPN_CP_DLUS_RETRY_INT_VAL_HELP);
NOPREFIX  (cfg_appn_cp_dlus_retry_int_noprefix, cfg_appn_cp_dlus_retry_int_num,
         cfg_appn_cp_dlus_retry_int_eol);
NVGENS    (cfg_appn_cp_dlus_retry_int_nvgen, cfg_appn_cp_dlus_retry_int_noprefix,
           appn_command, APPN_CP_DLUS_RETRY_INT);
KEYWORD_ID(cfg_appn_cp_dlus_retry_int, cfg_appn_cp_dlus_retry_int_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_CP_DLUS_RETRY_INT,
           APPN_CP_DLUS_RETRY_INT_KEY,
           APPN_CP_DLUS_RETRY_INT_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_dlus_retry_int

/*
 *    [no] prefer-active-dlus
 */
EOLS      (cfg_appn_cp_prefer_act_eol, appn_command, APPN_CP_PREFER_ACT);
KEYWORD_ID(cfg_appn_cp_prefer_act, cfg_appn_cp_prefer_act_eol,ALTERNATE,
           appn_subcommand_type, APPN_CP_PREFER_ACT,
           APPN_CP_PREFER_ACT_KEY,
         APPN_CP_PREFER_ACT_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cp_prefer_act
