/* $Id: cfg_int_backup.h,v 3.3.28.2 1996/05/21 09:59:11 thille Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_int_backup.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ B A C K U P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_backup.h,v $
 * Revision 3.3.28.2  1996/05/21  09:59:11  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.28.1  1996/04/27  06:39:02  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.2.20.2  1996/04/17  03:56:53  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.2.20.1  1996/02/23  20:53:45  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.3  1996/01/18  02:14:15  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  18:42:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:03:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * backup load { <enable-threshold> | never } { <disable-load> | never }
 * no backup load [{ <enable-threshold> | never } { <disable-load> | never }]
 *
 * OBJ(int,1) = <enable-threshold>	(-1 = never)
 * OBJ(int,2) = <disable-threshold>	(-1 = never)
 */

EOLS	(ci_backup_load_eol, backup_command, BACKUP_LOAD);

/* <disable-load> */
NUMBER	(ci_backup_load_dis, ci_backup_load_eol, no_alt,
	 OBJ(int,2), 0, 100, "Percentage");
KEYWORD_ID (ci_backup_load_neverdis, ci_backup_load_eol, ci_backup_load_dis, 
	    OBJ(int,2), -1,
	 "never", "Never deactivate the backup line", PRIV_CONF);

/* <enable-load> */
NUMBER	(ci_backup_load_en, ci_backup_load_neverdis, no_alt,
	 OBJ(int,1), 0, 100, "Percentage");
KEYWORD_ID (ci_backup_load_neveren, ci_backup_load_neverdis, ci_backup_load_en,
	    OBJ(int,1), -1,
	 "never", "Never activate the backup line", PRIV_CONF);

NOPREFIX (ci_backup_load_noprefix, ci_backup_load_neveren, ci_backup_load_eol);
NVGENS	(ci_backup_load_nvgen, ci_backup_load_noprefix, 
	 backup_command, BACKUP_LOAD);
KEYWORD	(ci_backup_load, ci_backup_load_nvgen, no_alt,
	 "load", "Load thresholds for line up or down transitions", PRIV_CONF);

/******************************************************************
 * backup interface <interface>
 * no backup interface <interface>
 *
 * OBJ(idb,1) = <interface>
 */
EOLS	(ci_backup_if_eol, backup_command, BACKUP_INTERFACE);
FORWARD_INTERFACE (ci_backup_if_name, ci_backup_if_eol, no_alt,
	   OBJ(idb,1), IFTYPE_HWIDB     );
NVGENS	(ci_backup_if_nvgen, ci_backup_if_name, 
	 backup_command, BACKUP_INTERFACE);
KEYWORD	(ci_backup_interface, ci_backup_if_nvgen, ci_backup_load,
	 common_str_interface, "Configure an interface as a backup",
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * backup delay { <enable-delay> | never } { disable-delay | never }
 * no backup delay [{ <enable-delay> | never } { disable-delay | never }]
 *
 * OBJ(int,1) = <enable-delay>	(-1 = never)
 * OBJ(int,2) = <disable-delay>	(-1 = never)
 */
EOLS	(ci_backup_delay_eol, backup_command, BACKUP_DELAY);

/* <disable-delay> */
NUMBER	(ci_backup_del_dis, ci_backup_delay_eol, no_alt,
	 OBJ(int,2), 0, -2, "Seconds");
KEYWORD_ID (ci_backup_del_neverdis, ci_backup_delay_eol, ci_backup_del_dis, 
	    OBJ(int,2), -1,
	    "never", "Never deactivate the backup line", PRIV_CONF);

/* <enable-delay> */
NUMBER	(ci_backup_del_en, ci_backup_del_neverdis, no_alt,
	 OBJ(int,1), 0, -2, "Seconds");
KEYWORD_ID (ci_backup_del_neveren, ci_backup_del_neverdis, ci_backup_del_en, 
	    OBJ(int,1), -1,
	    "never", "Never activate the backup line", PRIV_CONF);

NOPREFIX (ci_backup_del_noprefix, ci_backup_del_neveren, ci_backup_delay_eol);
NVGENS	(ci_backup_del_nvgen, ci_backup_del_noprefix, 
	 backup_command, BACKUP_DELAY);
KEYWORD	(ci_backup_delay, ci_backup_del_nvgen, ci_backup_interface,
	 "delay", "Delays before backup line up or down transitions",
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************/
KEYWORD	(ci_backup, ci_backup_delay, NONE,
	 "backup", "Modify dial-backup parameters", PRIV_CONF|PRIV_SUBIF);

ASSERT (ci_backup_test, ci_backup, ALTERNATE,
        ((csb->interface->hwptr) &&
	 ((csb->interface->hwptr->status & IDB_DIALER) == 0) &&
	 !is_ddr(csb->interface->hwptr)));
#undef	ALTERNATE
#define	ALTERNATE	ci_backup_test
