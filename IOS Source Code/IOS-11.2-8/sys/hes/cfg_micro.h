/* $Id: cfg_micro.h,v 3.2.40.3 1996/08/12 16:01:28 widmer Exp $
 * $Source: /release/112/cvs/Xsys/hes/cfg_micro.h,v $
 *------------------------------------------------------------------
 * C F G _ M I C R O . H
 *
 * Downloadable microcode parser support
 *
 * William H. Palmer, May 1993
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_micro.h,v $
 * Revision 3.2.40.3  1996/08/12  16:01:28  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.40.2  1996/07/08  22:24:59  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.2.40.1  1996/04/03  22:48:54  jwelder
 * CSCdi47348:  lingering references to rvip
 * Branch: California_branch
 *
 * Revision 3.2  1996/02/01  23:33:12  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.1  1996/02/01  04:58:01  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:44:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:20:14  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:11:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CFG_MICRO_SET_PREFS   	0
#define CFG_MICRO_RELOAD	CFG_MICRO_SET_PREFS+1

/******************************************************************
 * [no] microcode { FIP | SIP | FSIP | HIP | EIP | SP | TRIP  | AIP | MIP |
 		    CIP | FEIP| RVIP | SVIP | POSIP }
 *		    {ROM | SYSTEM | FLASH <flash file name> } [slot]
 *
 * OBJ(int,1)    = UCODE_FIP | UCODE_SIP | UCODE_FSIP | UCODE_HIP  |
 *		   UCODE_EIP | UCODE_SP  | UCODE_TRIP | UCODE_AIP  |
 * 		   UCODE_MIP | UCODE_CIP | UCODE_FEIP | UCODE_RVIP |
 *                 UCODE_SVIP | UCODE_POSIP
 * OBJ(int,2)    = { UCODE_ROM | UCODE_SYSTEM | UCODE_FLASH }
 * OBJ(int,3)    = <slot number>
 * OBJ(string,1) =  <file name>
 */

EOLS	(conf_micro_if_eol, microcode_command, UCODE_SET_PREFS);

/* slot numbers */

DECIMAL	(conf_micro_num, conf_micro_if_eol, conf_micro_if_eol,
	 OBJ(int,3), 0, MAX_SLOTS,
	 "Enter a valid slot number");
NOPREFIX(conf_micro_num_noprefix, conf_micro_num, conf_micro_if_eol);


/******************************************************************
 * [no] microcode reload
 */

EOLS	(conf_micro_rel_eol, microcode_command, UCODE_RELOAD);
KEYWORD	(conf_micro_reload, conf_micro_rel_eol, no_alt,
	 "reload", "force reloading of microcode",
	 PRIV_CONF);
NOPREFIX(conf_micro_reload_noprefix, conf_micro_reload, no_alt);

STRING	(conf_micro_flash_file, conf_micro_num, no_alt,
	 OBJ(string,1), "flash file name");

NOPREFIX(conf_micro_flash_file_noprefix, conf_micro_flash_file,
	 conf_micro_if_eol);

KEYWORD_ID(conf_micro_src_flash, conf_micro_flash_file_noprefix, no_alt,
	   OBJ(int,2), UCODE_FLASH,
	   "flash", " load microcode from flash file system",
	   PRIV_CONF);

KEYWORD_ID(conf_micro_src_sys, conf_micro_num_noprefix, conf_micro_src_flash,
	   OBJ(int,2), UCODE_SYSTEM,
	   "system", " load microcode from system image",
	   PRIV_CONF);

KEYWORD_ID(conf_micro_src, conf_micro_num_noprefix, conf_micro_src_sys,
	   OBJ(int,2), UCODE_ROM,
	   "rom", " load microcode from onboard ROM",
	   PRIV_CONF);

KEYWORD_ID(conf_micro_if_cip, conf_micro_src, conf_micro_reload_noprefix,
	   OBJ(int,1), UCODE_CIP,
	   "CIP", "Channel Interface Processor", PRIV_CONF);

KEYWORD_ID(conf_micro_if_rvip, conf_micro_src, conf_micro_if_cip,
	   OBJ(int,1), UCODE_RVIP,
	   "VIP", "Versatile Interface Processor", PRIV_CONF);

KEYWORD_ID_MM(conf_micro_if_svip, conf_micro_src, conf_micro_if_rvip,
	   OBJ(int,1), UCODE_SVIP,
	   "VIP2", "Versatile Interface Processor 2", PRIV_CONF, 4);

KEYWORD_ID(conf_micro_if_feip, conf_micro_src, conf_micro_if_svip,
	   OBJ(int,1), UCODE_FEIP,
	   "FEIP", "Fast Ethernet Interface Processor", PRIV_CONF);

KEYWORD_ID(conf_micro_if_posip, conf_micro_src, conf_micro_if_feip,
	   OBJ(int,1), UCODE_POSIP,
	   "POSIP", "Packet over Sonet Interface Processor", PRIV_CONF);

KEYWORD_ID(conf_micro_if_trip, conf_micro_src, conf_micro_if_posip,
	   OBJ(int,1), UCODE_TRIP,
	   "TRIP", "Token Ring Interface Processor", PRIV_CONF);

KEYWORD_ID(conf_micro_if_ssp, conf_micro_src, conf_micro_if_trip,
	   OBJ(int,1), UCODE_SSP,
	   "SSP", "Silicon Switch Processor", PRIV_CONF);

KEYWORD_ID(conf_micro_if_sp, conf_micro_src, conf_micro_if_ssp,
	   OBJ(int,1), UCODE_SP,
	   "SP", "Switch Processor", PRIV_CONF);

KEYWORD_ID(conf_micro_if_aip, conf_micro_src, conf_micro_if_sp,
	   OBJ(int,1), UCODE_AIP,
	   "AIP", "ATM Interface Processor", PRIV_CONF);

KEYWORD_ID(conf_micro_if_sip, conf_micro_src, conf_micro_if_aip,
	   OBJ(int,1), UCODE_SIP,
	   "SIP", "Serial Interface Processor", PRIV_CONF);

KEYWORD_ID(conf_micro_if_mip, conf_micro_src, conf_micro_if_sip,
	   OBJ(int,1), UCODE_MIP,
	   "MIP", "Multi-Channel Interface Processor", PRIV_CONF);

KEYWORD_ID(conf_micro_if_hip, conf_micro_src, conf_micro_if_mip,
	   OBJ(int,1), UCODE_HIP,
	   "HIP", "HSSI Interface Processor", PRIV_CONF);

KEYWORD_ID(conf_micro_if_fsip, conf_micro_src, conf_micro_if_hip,
	   OBJ(int,1), UCODE_FSIP,
	   "FSIP", "Fast Serial Interface Processor", PRIV_CONF);

KEYWORD_ID(conf_micro_if_fip, conf_micro_src, conf_micro_if_fsip,
	   OBJ(int,1), UCODE_FIP,
	   "FIP", "FDDI Interface Processor", PRIV_CONF);

KEYWORD_ID(conf_micro_interface, conf_micro_src, conf_micro_if_fip,
	   OBJ(int,1), UCODE_EIP,
	   "EIP", "Ethernet Interface Processor", PRIV_CONF);

NVGENNS	(conf_micro_nvgen, conf_micro_interface, nv_ucode);

/******************************************************************/
KEYWORD	(cfg_microcode, conf_micro_nvgen, ALTERNATE,
	  "microcode", "Downloadable microcode commands", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cfg_microcode
