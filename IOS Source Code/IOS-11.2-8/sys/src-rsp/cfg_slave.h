/* $Id: cfg_slave.h,v 3.4.6.1 1996/08/12 16:07:32 widmer Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/cfg_slave.h,v $
 *------------------------------------------------------------------
 * cfg_slave.h - parser tree for slave management
 *
 * August 1995, David Bath
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_slave.h,v $
 * Revision 3.4.6.1  1996/08/12  16:07:32  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/03/05  01:25:11  dbath
 * CSCdi47229:  HSA slave reload and slave <image> commands need to work
 * Allow non-bundled slave images to be used in the slave.
 *
 * Revision 3.3  1996/02/16  05:55:52  dbath
 * CSCdi49182:  Help for HSA slave global config cmnd is wrong
 * So fix it dear Henry, dear Henry...
 *
 * Revision 3.2  1995/11/17  18:46:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:27:42  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/07  16:54:10  dbath
 * Placeholders for new development in VIP_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/************************************************************
 * [no]slave { reload | image {system | flash <fname>} | 
 *             default-slot <slotnum> | auto-sync} 
 *
 * OBJ(int,1) = default slot number for default-slot
 * OBJ(string,1) = flash file name
 *
 */

/* slave default-slot <slotnum> */
static void conf_slave_slot_range (parseinfo *csb, uint *lower, uint *upper)
{
    *lower = lowest_dual_slot;
    *upper = highest_dual_slot;
}

/* slave default-slot n */
EOLNS    (conf_slave_slot_eols, rsp_slave_slot_cmd);
NUMBER_FUNC  (conf_slave_slot_number, conf_slave_slot_eols, no_alt, OBJ(int,1),
	      conf_slave_slot_range, "slot number");
NOPREFIX(conf_slave_slot_no, conf_slave_slot_number, conf_slave_slot_eols);
NVGENNS  (conf_slave_slot_nvgen, conf_slave_slot_number, rsp_slave_slot_cmd);
KEYWORD (conf_slave_slot_kwd, conf_slave_slot_nvgen, no_alt, 
	 "default-slot", "Set default slave slot", PRIV_CONF);

/* slave reload */
EOLNS	(conf_slave_rel_eol, rsp_slave_reload_cmd);
KEYWORD	(conf_slave_rel_kwd, conf_slave_rel_eol, conf_slave_slot_kwd, 
	 "reload", "Reload slave processor", PRIV_CONF);

/* slave image flash <fname>*/
EOLS	(conf_slave_img_flsh_file_eol, rsp_slave_image_cmd, UCODE_FLASH);
STRING	(conf_slave_img_flsh_file, conf_slave_img_flsh_file_eol, no_alt,
	 OBJ(string,1), "flash file name");
NOPREFIX(conf_slave_img_flsh_noprefix, conf_slave_img_flsh_file,
	 conf_slave_img_flsh_file_eol);
KEYWORD	(conf_slave_img_flsh_kwd, conf_slave_img_flsh_noprefix, no_alt, 
	 "flash", " load image from flash file system", PRIV_CONF);

/* slave image system */
EOLS	(conf_slave_img_sys_eol, rsp_slave_image_cmd, UCODE_SYSTEM);
KEYWORD	(conf_slave_img_sys_kwd, conf_slave_img_sys_eol, 
	 conf_slave_img_flsh_kwd, 
	 "system", " load image from system bundle", PRIV_CONF);

/* slave image ...*/
NVGENNS  (conf_slave_img_nvgen, conf_slave_img_sys_kwd,
	 rsp_slave_image_nvgen);
KEYWORD	(conf_slave_img_kwd, conf_slave_img_nvgen, conf_slave_rel_kwd, 
	 "image", "Slave image location", PRIV_CONF);

/* slave auto-sync config*/
EOLNS	(conf_slave_sync_cfg_eol, rsp_slave_auto_sync_cmd);
KEYWORD	(conf_slave_sync_cfg_kwd, conf_slave_sync_cfg_eol, NONE, 
	 "config", "Set auto sync mode for slave startup-config", PRIV_CONF);

/* slave auto-sync */
KEYWORD	(conf_slave_sync_kwd, conf_slave_sync_cfg_kwd, conf_slave_img_kwd, 
	 "auto-sync", "Set auto sync mode for slave", PRIV_CONF);

#ifdef FUTURE_FEATURE            /* Chain this back in when it's ready */
/* slave buffer-memory */
EOLNS	(conf_slave_buf_eol, rsp_slave_buff_cmd);
KEYWORD	(conf_slave_buf_kwd, conf_slave_buf_eol, conf_slave_sync_kwd, 
	 "buffer-memory", "Enable use of slave buffer memory", PRIV_CONF);
#endif

KEYWORD	(conf_slave_kwd, conf_slave_sync_kwd, ALTERNATE, "slave",
	 "Configure HSA slave processor", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_slave_kwd


