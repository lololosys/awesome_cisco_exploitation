/* $Id: cfg_int_atm_c4000.h,v 3.2.62.2 1996/04/25 23:22:27 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/src-c4000/cfg_int_atm_c4000.h,v $
 *------------------------------------------------------------------
 * 4500 ATM NIM platform specific ATM configuration commands.
 *
 * March, 1995  Walter Friedrich
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_atm_c4000.h,v $
 * Revision 3.2.62.2  1996/04/25  23:22:27  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.2.62.1.2.1  1996/04/10  18:28:33  rzagst
 * ATM Modularity: Add () to #define per code review action item.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.2.62.1  1996/03/18  22:01:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  02:00:58  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  17:58:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:45:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:57:33  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.4  1995/06/27  00:30:31  wfried
 * CSCdi36069:  Performance tuning and changes on controller screen for
 * new interfac
 * Included more interface status in the show controller output,
 * forced ATM receiving packets to start at 16 byte boundaries for
 * better DBUS utilization, and made some fastswitching optimizations.
 * Created new ATM interface loopback command options and renamed E3
 * framing modes to reflect current terminology
 *
 * Revision 2.3  1995/06/21  18:16:32  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.2  1995/06/11  03:42:38  awu
 * CSCdi34590:  router incorrectly configs extra rate-queue at boot time.
 *
 * Revision 2.1  1995/06/07  22:03:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * ATM vc_per_vp <n>
 *
 * OBJ(int,1) = VCIs per VPIs
 */
static keyword_options vcs_per_vp_options[]= {
  { "32"," VCs per VP",32 },
  { "64",NULL,64 },
  { "128",NULL,128 },
  { "256",NULL,256 },
  { "512",NULL,512 },
  { "1024",NULL,1024 },
  { "2048",NULL,2048 },
  { "4096",NULL,4096 },
  { "8192",NULL,8192 },
  { NULL,NULL,0 }
};

EOLS    (ci_atm_vc_vp_eol, atm_interface_commands, ATM_CMD_VC_PER_VP);
 
KEYWORD_OPTIONS(ci_atm_vc_vp_num,ci_atm_vc_vp_eol,no_alt,
		vcs_per_vp_options,OBJ(int,1),PRIV_CONF,0);

NVGENS  (ci_atm_vc_vp_nvgen, ci_atm_vc_vp_num,
        atm_interface_commands, ATM_CMD_VC_PER_VP);
KEYWORD (ci_atm_vc_vp, ci_atm_vc_vp_nvgen, ALTERNATE,
        "vc-per-vp", "ATM VCIs per VPI", PRIV_CONF);

/******************************************************************
 * ATM ds3-scramble
 *
 */
EOLS    (ci_atm_ds3scramble_eol, atm_interface_commands, ATM_CMD_DS3SCRAMBLE);
KEYWORD (ci_atm_ds3scramble_kwd, ci_atm_ds3scramble_eol, no_alt,
	"ds3-scramble", "Enable DS3 Scrambling", PRIV_CONF);
ASSERT  (ci_atm_ds3scramble, ci_atm_ds3scramble_kwd, ci_atm_vc_vp,
	is_atm_ds3(csb->interface->hwptr));

/******************************************************************
 * ATM framing [m23adm|cbitplcp|m23plcp]
 *
 * OBJ(int,1) = Framing value
 *
 * Notes: The default mode is C-bit ADM
 */
EOLS    (ci_atm_ds3framing_eol, atm_interface_commands, ATM_CMD_DS3FRAMING);
KEYWORD_ID(ci_atm_ds3framing_m23plcp, ci_atm_ds3framing_eol, no_alt,
        OBJ(int,1), ATM_FRAMING_DS3M23PLCP, "m23plcp",
        "DS3 Framing M23 PLCP", PRIV_CONF);
KEYWORD_ID(ci_atm_ds3framing_cbitplcp, ci_atm_ds3framing_eol,
        ci_atm_ds3framing_m23plcp, OBJ(int,1), ATM_FRAMING_DS3CBITPLCP,
	"cbitplcp", "DS3 Framing C-bit PLCP", PRIV_CONF);
KEYWORD_ID(ci_atm_ds3framing_m23adm, ci_atm_ds3framing_eol,
	ci_atm_ds3framing_cbitplcp, OBJ(int,1), ATM_FRAMING_DS3M23ADM,
	"m23adm", "DS3 Framing M23 ADM", PRIV_CONF);
KEYWORD (ci_atm_ds3framing_kwd, ci_atm_ds3framing_m23adm, no_alt,
        "framing", "ATM DS3 Framing style", PRIV_CONF);
ASSERT  (ci_atm_ds3framing, ci_atm_ds3framing_kwd, ci_atm_ds3scramble,
        is_atm_ds3(csb->interface->hwptr));

/******************************************************************
 * ATM framing [g832adm|g751adm]
 *
 * OBJ(int,1) = Framing value
 *
 * Notes: G.804 is the same as G.832 ADM
 *        G.751 ADM is the same as UNFRAMED but without the PLCP framing
 *	  The default UNFRAMED mode is in fact G.751 with PLCP framing!!!
 */
EOLS    (ci_atm_e3framing_eol, atm_interface_commands, ATM_CMD_E3FRAMING);
KEYWORD_ID(ci_atm_e3framing_g751adm, ci_atm_e3framing_eol, no_alt,
        OBJ(int,1), ATM_FRAMING_E3G751ADM, "g751adm",
        "E3 Framing G.751 ADM", PRIV_CONF);
KEYWORD_ID(ci_atm_e3framing_g832adm, ci_atm_e3framing_eol,
	ci_atm_e3framing_g751adm, OBJ(int,1), ATM_FRAMING_E3G832ADM, "g832adm",
        "E3 Framing G.832 ADM", PRIV_CONF);
KEYWORD (ci_atm_e3framing_kwd, ci_atm_e3framing_g832adm, no_alt,
        "framing", "ATM E3 Framing style", PRIV_CONF);
ASSERT  (ci_atm_e3framing, ci_atm_e3framing_kwd, ci_atm_ds3framing,
        is_atm_e3(csb->interface->hwptr));

#undef	ALTERNATE
#define	ALTERNATE	ci_atm_e3framing

