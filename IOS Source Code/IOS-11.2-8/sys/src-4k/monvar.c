/* $Id: monvar.c,v 3.3.46.1 1996/03/18 21:57:53 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-4k/monvar.c,v $
 *------------------------------------------------------------------
 * interface to boot variables
 *
 * October 1994, Carl Schaefer
 * Oct 1993, Lele Nardin
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: monvar.c,v $
 * Revision 3.3.46.1  1996/03/18  21:57:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.2  1996/03/07  10:43:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/02/20  17:49:58  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/05  07:05:59  dbath
 * CSCdi43178:  rommon boot info not kept in sync on slave
 * Ship the three boot related environment variables, and the config
 * register setting, across to the slave whenever autosync is enabled,
 * or whenever the user manually syncs the slave config.
 *
 * Revision 3.2  1995/11/17  18:40:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:20:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:52:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "sys_registry.h"
#include "../dev/monitor1.h"
#include <string.h>

/*
 * Global BOOT variables.
 */
static char *BOOT_var_name = BOOT_VAR_NAME_STRING;
static char BOOT_var_val[MAX_BOOTVAR_LEN+1];
static char BOOT_var_val_last[MAX_BOOTVAR_LEN+1];

static char *CONFIG_var_name = CONFIG_VAR_NAME_STRING;
static char CONFIG_var_val[MAX_BOOTVAR_LEN+1];
static char CONFIG_var_val_last[MAX_BOOTVAR_LEN+1];

static char *BOOTLDR_var_name = BOOTLDR_VAR_NAME_STRING;
static char BOOTLDR_var_val[MAX_BOOTVAR_LEN+1];
static char BOOTLDR_var_val_last[MAX_BOOTVAR_LEN+1];

static int ROMMONvar_set (char *val, char *last, boolean vocal)
{
    char *ptr;
    int ret;

    reg_invoke_set_slave_monvar(val, vocal);

    ptr = strchr(val, '=');
    if (ptr && !strncmp(last, ptr+1, MAX_BOOTVAR_LEN))
	return 0;

    ret = mon_set_var(val);
    if (!ptr || (ret < 0)) {
	last[0] = '\0';
    } else {
	strncpy(last, ptr+1, MAX_BOOTVAR_LEN);
	last[MAX_BOOTVAR_LEN] = '\0';
    }
    return ((ret < 0) ? 0 : ret);
}

static char *BOOTvar_get (void)
{
    int ret;

    BOOT_var_val_last[0] = '\0';
    ret = mon_get_var(BOOT_var_name,
		BOOT_var_val_last, sizeof(BOOT_var_val_last));
    return ((ret < 0) ? NULL : BOOT_var_val_last);
}

static int BOOTvar_set (boolean vocal)
{
    return ROMMONvar_set(BOOT_var_val, BOOT_var_val_last, vocal);
}

static void BOOTvar_open (void)
{
    strcpy(BOOT_var_val, BOOT_var_name);
    strcat(BOOT_var_val, "=");
}

static void BOOTvar_add (char *val)
{
    strcat(BOOT_var_val, val);
}

static char *BOOTvar_buff (void)
{
    return (BOOT_var_val);
}

static char *CONFIGvar_get (void)
{
    int ret;

    CONFIG_var_val_last[0] = '\0';
    ret = mon_get_var(CONFIG_var_name,
		CONFIG_var_val_last, sizeof(CONFIG_var_val_last));
    return ((ret < 0) ? NULL : CONFIG_var_val_last);
}

static int CONFIGvar_set (boolean vocal)
{
    return ROMMONvar_set(CONFIG_var_val, CONFIG_var_val_last, vocal);
}

static void CONFIGvar_open (void)
{
    strcpy(CONFIG_var_val, CONFIG_var_name);
    strcat(CONFIG_var_val, "=");
}

static void CONFIGvar_add (char *val)
{
    strcat(CONFIG_var_val, val);
}

static char *CONFIGvar_buff (void)
{
    return (CONFIG_var_val);
}

static char *BOOTLDRvar_get (void)
{
    int ret;

    BOOTLDR_var_val_last[0] = '\0';
    ret = mon_get_var(BOOTLDR_var_name,
		BOOTLDR_var_val_last, sizeof(BOOTLDR_var_val_last));
    return ((ret < 0) ? NULL : BOOTLDR_var_val_last);
}

static int BOOTLDRvar_set (boolean vocal)
{
    return ROMMONvar_set(BOOTLDR_var_val, BOOTLDR_var_val_last, vocal);
}

static void BOOTLDRvar_open (void)
{
    strcpy(BOOTLDR_var_val, BOOTLDR_var_name);
    strcat(BOOTLDR_var_val, "=");
}

static void BOOTLDRvar_add (char *val)
{
    strcat(BOOTLDR_var_val, val);
}

static char *BOOTLDRvar_buff (void)
{
    return (BOOTLDR_var_val);
}

static void monvar_subsys_init (subsystype *subsys)
{
	/*
	 * Fetch these now so we don't have to do it later when
	 * interrupts disabled by the EMT could cause packet loss.
	 */
    BOOTvar_get();
    CONFIGvar_get();
    BOOTLDRvar_get();

	/*
	 * Registry routines for BOOT variable.
	 */
    if (platform_nvvar_support(ROMMON_BOOT_VAR)) {
        reg_add_monvar_get(ROMMON_BOOT_VAR, BOOTvar_get, "BOOTvar_get");
        reg_add_monvar_set(ROMMON_BOOT_VAR, BOOTvar_set, "BOOTvar_set");
        reg_add_monvar_open(ROMMON_BOOT_VAR, BOOTvar_open, "BOOTvar_open");
        reg_add_monvar_add(ROMMON_BOOT_VAR, BOOTvar_add, "BOOTvar_add");
        reg_add_monvar_buff(ROMMON_BOOT_VAR, BOOTvar_buff, "BOOTvar_buff");
    }


	/*
	 * Registry routines for CONFIG_FILE variable.
	 */
    if (platform_nvvar_support(ROMMON_CONFIG_VAR)) {
        reg_add_monvar_get(ROMMON_CONFIG_VAR, CONFIGvar_get, "CONFIGvar_get");
        reg_add_monvar_set(ROMMON_CONFIG_VAR, CONFIGvar_set, "CONFIGvar_set");
        reg_add_monvar_open(ROMMON_CONFIG_VAR, CONFIGvar_open, "CONFIGvar_open");
        reg_add_monvar_add(ROMMON_CONFIG_VAR, CONFIGvar_add, "CONFIGvar_add");
        reg_add_monvar_buff(ROMMON_CONFIG_VAR, CONFIGvar_buff, "CONFIGvar_buff");
    }

	/*
	 * Registry routines for BOOTLDR variable.
	 */
    if (platform_nvvar_support(ROMMON_BOOTLDR_VAR)) {
        reg_add_monvar_get(ROMMON_BOOTLDR_VAR, BOOTLDRvar_get, "BOOTLDRvar_get");
        reg_add_monvar_set(ROMMON_BOOTLDR_VAR, BOOTLDRvar_set, "BOOTLDRvar_set");
        reg_add_monvar_open(ROMMON_BOOTLDR_VAR, BOOTLDRvar_open, "BOOTLDRvar_open");
        reg_add_monvar_add(ROMMON_BOOTLDR_VAR, BOOTLDRvar_add, "BOOTLDRvar_add");
        reg_add_monvar_buff(ROMMON_BOOTLDR_VAR, BOOTLDRvar_buff, "BOOTLDRvar_buff");
     }

    return;
}

SUBSYS_HEADER(monvar,			/* name */
	      1,			/* major version */
	      0,			/* minor version */
	      1,			/* edit version */
	      monvar_subsys_init,	/* init function */
	      SUBSYS_CLASS_KERNEL,	/* subsystem class */
	      NULL,			/* sequence list */
	      NULL			/* prerequisite list */
	      );
