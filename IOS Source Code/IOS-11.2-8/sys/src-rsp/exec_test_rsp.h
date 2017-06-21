/* $Id: exec_test_rsp.h,v 3.2.62.2 1996/08/30 06:58:18 mcieslak Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/exec_test_rsp.h,v $
 *------------------------------------------------------------------
 * parse chain for RSP specific test commands
 *
 * Octobert 1994, Carl Schaefer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_test_rsp.h,v $
 * Revision 3.2.62.2  1996/08/30  06:58:18  mcieslak
 * CSCdi67677:  Need to enhance RSP cache and watch test commands
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/12  16:07:35  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:46:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:57:41  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:27:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * test rsp debug {off|on}
 *
 *	OBJ(int,1) = { OFF | ON }
 */
EOLNS	(rsp_debug_eol, rsp_test_debug_command);

KEYWORD_ID(rsp_debug_off, rsp_debug_eol, no_alt,
	   OBJ(int,1), TEST_RSP_DEBUG_STATE_OFF,
	   "off", "disable RSP debug messages", 0);

KEYWORD_ID(rsp_debug_on, rsp_debug_eol, rsp_debug_off,
	   OBJ(int,1), TEST_RSP_DEBUG_STATE_ON,
	   "on", "enable RSP debug messages", 0);

KEYWORD(test_rsp_debug, rsp_debug_on, no_alt, "debug",
	"Disable/Enable RSP debug messages", 0);


/******************************************************************
 * test rsp auto-interface-restart {off|on}
 *
 * 	OBJ(int,1) = { OFF | ON }
 */

EOLNS	(rsp_air_eol, rsp_test_air_command);

KEYWORD_ID(rsp_air_off, rsp_air_eol, no_alt,
	   OBJ(int,1), TEST_RSP_AIR_STATE_OFF,
	   "off", "disable automatic network interface restart", 0);

KEYWORD_ID(rsp_air_on, rsp_air_eol, rsp_air_off,
	   OBJ(int,1), TEST_RSP_AIR_STATE_ON,
	   "on", "enable automatic network interface restart", 0);

KEYWORD(test_rsp_air, rsp_air_on, test_rsp_debug, "auto-interface-restart",
	"Disable/Enable automatic network interface restart", 0);

/******************************************************************
 * test rsp ds1201 { read <addr> | write <addr> <byte> }
 *    OBJ(int,1) = { read | write }
 *    OBJ(int,2) = <addr>
 *    OBJ(int,3) = <byte>
 */

EOLNS	(ds1201_eol, test_rsp_ds1201);

GENERAL_NUMBER(ds1201_val, ds1201_eol, no_alt,
	       OBJ(int,3), 0, 0xff, "value",
	       NUMBER_HEX | HEX_NO_ZEROX_OK | NUMBER_WS_OK |
	       NUMBER_HELP_CHECK);

GENERAL_NUMBER(ds1201_addrval, ds1201_val, no_alt,
	       OBJ(int,2), 0, 0x7f, "address",
	       NUMBER_HEX | HEX_NO_ZEROX_OK | NUMBER_WS_OK |
	       NUMBER_HELP_CHECK);

KEYWORD_ID(ds1201_write, ds1201_addrval, no_alt,
	   OBJ(int,1), TEST_RSP_DS1201_WRITE,
	   "write", "write a byte", 0);

GENERAL_NUMBER(ds1201_addr, ds1201_eol, no_alt,
	       OBJ(int,2), 0, 0x7f, "address",
	       NUMBER_HEX | HEX_NO_ZEROX_OK | NUMBER_WS_OK |
	       NUMBER_HELP_CHECK);

KEYWORD_ID(ds1201_read, ds1201_addr, ds1201_write,
	   OBJ(int,1), TEST_RSP_DS1201_READ,
	   "read", "read a byte", 0);

KEYWORD(test_rsp_ds1201, ds1201_read, test_rsp_air,
	"ds1201", "read/write DS1201", 0);

/******************************************************************
 * test rsp watch { addr } [ mask ]
 *    OBJ(int,1) == addr
 *    OBJ(int,2) == mask
 * test rsp watch control { off | read | write | on }
 * test rsp watch action { crash | continue }
 */
EOLNS	(rsp_watchaddr_eol, rsp_watchaddr_command);

HEXNUM	(rsp_watch_mask, rsp_watchaddr_eol, rsp_watchaddr_eol,
	 OBJ(int,2), "address mask (don't care == 1)");

HEXNUM	(rsp_watch_addr, rsp_watch_mask, no_alt,
	 OBJ(int,1), "physical address to watch");

EOLNS	(rsp_watchact_eol, rsp_watchact_command);

KEYWORD_ID(rsp_watchact_continue, rsp_watchact_eol, no_alt,
	   OBJ(int,1), TEST_RSP_WATCH_ACT_CONTINUE,
	   "continue", "", 0);

KEYWORD_ID(rsp_watchact_crash, rsp_watchact_eol, rsp_watchact_continue,
	   OBJ(int,1), TEST_RSP_WATCH_ACT_CRASH,
	   "crash", "", 0);

KEYWORD(rsp_watch_act, rsp_watchact_crash, rsp_watch_addr,
	"action", "action to take on watch event", 0);

EOLNS	(rsp_watchctl_eol, rsp_watchctl_command);

KEYWORD_ID(rsp_watchctl_write, rsp_watchctl_eol, no_alt,
	   OBJ(int,1), DCL_WATCH_WRITE,
	   "write", "watch writes only", 0);

KEYWORD_ID(rsp_watchctl_read, rsp_watchctl_eol, rsp_watchctl_write,
	   OBJ(int,1), DCL_WATCH_READ,
	   "read", "watch reads only", 0);

KEYWORD_ID(rsp_watchctl_off, rsp_watchctl_eol, rsp_watchctl_read,
	   OBJ(int,1), DCL_WATCH_INACTIVE,
	   "off", "disable watching", 0);

KEYWORD_ID(rsp_watchctl_any, rsp_watchctl_eol, rsp_watchctl_off,
	   OBJ(int,1), DCL_WATCH_ANYCYCLE,
	   "any", "watch all cycles", 0);

KEYWORD(rsp_watch_ctl, rsp_watchctl_any, rsp_watch_act,
	"control", "enable/disable watch", 0);

KEYWORD(test_rsp_watch, rsp_watch_ctl, test_rsp_ds1201,
	"watch", "set watch register", 0);

/******************************************************************
 * test rsp write-buffer {cpu|memd|io} {off|on|read-around-write}
 *
 * 	OBJ(int,1) = cpu|memd|io
 *	OBJ(int,2)  = { OFF | ON | READ_AROUND_WRITE }
 */

EOLNS	(test_rsp_wb_eol, rsp_test_wb_command);

KEYWORD_ID(test_rsp_wbstate_raw, test_rsp_wb_eol, no_alt,
	   OBJ(int,2), TEST_RSP_WBSTATE_RAW,
	   "read-around-write", "Enable write buffer and read-around-write",
	   0);

NEQ	(test_rsp_wbstate_raw_test, test_rsp_wbstate_raw, no_alt,
	 OBJ(int,1), TEST_RSP_WB_IO);

KEYWORD_ID(test_rsp_wbstate_on, test_rsp_wb_eol, test_rsp_wbstate_raw_test,
	   OBJ(int,2), TEST_RSP_WBSTATE_ON,
	   "on", "Enable write buffer", 0);

KEYWORD_ID(test_rsp_wbstate_off, test_rsp_wb_eol, test_rsp_wbstate_on,
	   OBJ(int,2), TEST_RSP_WBSTATE_OFF,
	   "off", "Disable write buffer", 0);

KEYWORD_ID(test_rsp_wb_io, test_rsp_wbstate_off, no_alt,
	   OBJ(int,1), TEST_RSP_WB_IO,
	   "io", "I/O memory system write buffer", 0);

KEYWORD_ID(test_rsp_wb_memd, test_rsp_wbstate_off, test_rsp_wb_io,
	   OBJ(int,1), TEST_RSP_WB_MEMD,
	   "memd", "MEMD memory write buffer", 0);

KEYWORD_ID(test_rsp_wb_cpu, test_rsp_wbstate_off, test_rsp_wb_memd,
	   OBJ(int,1), TEST_RSP_WB_CPU,
	   "cpu", "CPU memory write buffer", 0);

KEYWORD(test_rsp_writebuf, test_rsp_wb_cpu, test_rsp_watch,
	"write-buffer", "change memory system write buffer configuration", 0);

/******************************************************************
 * test rsp [cache {cpu|memd-fastswitch|memd-optimum-and-flow|buffer-headers}
 *			{write-through-write-allocate|
 *			 write-through|uncached|write-back} ]
 * 
 * 	OBJ(int,1) = cpu|memd-wb|memd-wt|buffer-headers
 *	OBJ(int,2) = policy
 */

EOLNS	(test_rsp_cache_eol, rsp_test_cache_command);

KEYWORD_ID(test_rsp_cachepolicy_wtwa, test_rsp_cache_eol, NONE,
	   OBJ(int,2), TLB_CACHE_WTWA,
	   "write-allocate",
	   "Enable caching, write-through, write-allocate", 0);

KEYWORD_ID(test_rsp_cachepolicy_wt, test_rsp_cache_eol,
	   test_rsp_cachepolicy_wtwa,
	   OBJ(int,2), TLB_CACHE_WT,
	   "write-through", "Enable caching, write-through", 0);

KEYWORD_ID(test_rsp_cachepolicy_wb, test_rsp_cache_eol,
	   test_rsp_cachepolicy_wt,
	   OBJ(int,2), TLB_CACHE_NONCOHERENT,
	   "write-back", "Enable caching, write-back", 0);

KEYWORD_ID(test_rsp_cachepolicy_un, test_rsp_cache_eol,
	   test_rsp_cachepolicy_wb,
	   OBJ(int,2), TLB_UNCACHED,
	   "uncached", "Disable caching", 0);

KEYWORD_ID(test_rsp_cache_k0, test_rsp_cachepolicy_un, no_alt,
	   OBJ(int,1), TEST_RSP_CACHE_KSEG0,
	   "kseg0", "Kernel address space zero", 0);

KEYWORD_ID(test_rsp_cache_bufhdr, test_rsp_cachepolicy_un, test_rsp_cache_k0,
	   OBJ(int,1), TEST_RSP_CACHE_BUFHDR,
	   "buffer-headers", "buffer header memory", 0);

SET	(test_rsp_cache_memd_optimum_and_flow_set, test_rsp_cachepolicy_un,
	   OBJ(int,1), TEST_RSP_CACHE_MEMD_OPTIMUM_AND_FLOW);

KEYWORD_MM(test_rsp_cache_memd_optimum_and_flow,
	   test_rsp_cache_memd_optimum_and_flow_set,
	   test_rsp_cache_bufhdr,
	   "memd-optimum-and-flow",
	   "packet memory view used by optimum and flow code", 0, 6);

SET	(test_rsp_cache_memd_fastswitch_set, test_rsp_cachepolicy_un,
	 OBJ(int,1), TEST_RSP_CACHE_MEMD_FASTSWITCH);

KEYWORD_MM(test_rsp_cache_memd_fastswitch, test_rsp_cache_memd_fastswitch_set,
	   test_rsp_cache_memd_optimum_and_flow,
	   "memd-fastswitch", "packet memory view used by fastswitch code",
	   0, 6);

KEYWORD_ID(test_rsp_cache_cpu, test_rsp_cachepolicy_un,
	   test_rsp_cache_memd_fastswitch, OBJ(int,1), TEST_RSP_CACHE_CPU,
	   "cpu", "CPU memory", 0);

KEYWORD(test_rsp_cache, test_rsp_cache_cpu, test_rsp_writebuf,
	"cache", "change memory region cache policy", 0);

/******************************************************************
 * test rsp stall
 */
EOLNS	(test_rsp_stall_eol, test_rsp_stall);

KEYWORD	(test_rsp_stall, test_rsp_stall_eol, test_rsp_cache,
	"stall", "simulate cbus stall condition", 0);

/******************************************************************
 * test rsp slot [ { mask | unmask } <slot number> ]
 *	OBJ(int,1) = { mask | unmask | print }
 *	OBJ(int,2) = <slot number>
 */

EOLNS	(test_rsp_slot_eol, test_rsp_slot);

NUMBER	(test_rsp_slot_number, test_rsp_slot_eol, no_alt,
	 OBJ(int, 2), 0, MAX_SLOTS, "slot number");

KEYWORD_ID(test_rsp_slot_unmask, test_rsp_slot_number, test_rsp_slot_eol,
	   OBJ(int, 1), TEST_SLOT_UNMASK,
	   "unmask", "unmask presence", 0);

KEYWORD_ID(test_rsp_slot_mask, test_rsp_slot_number, test_rsp_slot_unmask,
	   OBJ(int, 1), TEST_SLOT_MASK,
	   "mask", "mask presence", 0);

KEYWORD_ID(test_rsp_slot, test_rsp_slot_mask, test_rsp_stall,
	   OBJ(int, 1), TEST_SLOT_PRINT,
	   "slot", "simulate card insertion & removal", 0);

KEYWORD(test_rsp, test_rsp_slot, ALTERNATE,
	"rsp", NULL /* NOHELP */, PRIV_ROOT|PRIV_NOHELP);

#undef	ALTERNATE
#define	ALTERNATE	test_rsp
