/* $Id: snmp_flash_cmnds.c,v 3.2 1995/11/17 09:01:38 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/dev/snmp_flash_cmnds.c,v $
 *------------------------------------------------------------------
 * snmp_flash_cmnds.c - High level flash routines for SNMP.
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: snmp_flash_cmnds.c,v $
 * Revision 3.2  1995/11/17  09:01:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:20:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:25:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "../dev/flash_defines.h"
#include "../dev/flash_services.h"
#include "../dev/sr_old_lflashmib.h"
#include "../dev/snmp_flash_cmnds.h"
#include "../dev/flash_cmnds.h"

/*
 * Currently there is SNMP support only for the system flash.
 * SNMP support for the bootstrap flash may be added later.
 * The MIB for this needs to be defined.
 */

/*
 ******************** Local functions ******************************
 */
/*
 * Malloc and set up data structure with all parameters for the copy.
 * First ensure that system flash is present.
 */
static flash_copy_parms_t *snmp_init_copy_parms (addrtype *address, 
                                                 char *filename)
{
    flash_copy_parms_t *fparms;

    fparms = reg_invoke_flash_filename_parse(filename);
    if (fparms != NULL) {
        if (address != NULL)
            address_copy(&fparms->addr, address);
        strcpy(fparms->dfname, fparms->sfname);
        fparms->prompt = 0;
        fparms->verbose = 0;
        fparms->erase = 0;
        fparms->sts = 0;
    }
    return (fparms);
}

/*
 ********************* Global functions ******************************
 */
/*
 * Just trying to figure out if this puppy exists
 */
boolean snmp_flash_exist (void)
{
    char *system_flash;

    system_flash = reg_invoke_flash_device_default();
    return (system_flash != NULL);
}

/*
 * check to see if the puppy is available
 */
boolean snmp_flash_busy (void)
{
    char *system_flash;

    system_flash = reg_invoke_flash_device_default();
    return (system_flash ? flash_available(system_flash) : FALSE);
}

/*
 * see how big a bark the puppy has (it's size I mean)
 */
uint snmp_flash_size (void)
{
    char *system_flash;

    system_flash = reg_invoke_flash_device_default();
    return (system_flash ? flash_size(system_flash) : 0);
}

/*
 * see how many free bytes the puppy has
 */
uint snmp_flash_free (void)
{
    char *system_flash;

    system_flash = reg_invoke_flash_device_default();
    return (system_flash ? flash_free_space(system_flash) : 0);
}

/*
 * see if the puppy is programmable.
 * RETURN TRUE IF CAN BE PROGRAMMED, I guess.
 * a note on return values is always helpful.
 */
boolean snmp_flash_vpp (void)
{
    char *system_flash;

    system_flash = reg_invoke_flash_device_default();
    return (system_flash ? flash_status(system_flash) : FALSE);
}

/*
 * see how many beasts the puppy is carrying.
 */
int snmp_flash_filecnt (void)
{
    char *system_flash;

    system_flash = reg_invoke_flash_device_default();
    return (system_flash ? flash_file_count(system_flash) : 0);
}

/*
 * what card is the puppy sitting on ?
 */
uchar *snmp_flash_card (void)
{
    char *system_flash;

    system_flash = reg_invoke_flash_device_default();
    return (system_flash ? flash_card_name(system_flash) : NULL);
}

/*
 * who's the puppy's boss (if any) ?
 */
uchar *snmp_flash_ctlr (void)
{
    char *system_flash;

    system_flash = reg_invoke_flash_device_default();
    return (system_flash ? flash_ctlr_name(system_flash) : NULL);
}

/*
 * give the puppy a soap bath and wipe him clean - real clean.
 */
int snmp_flash_erase_all (void)
{
    flash_copy_parms_t *fparms;
    boolean sts;

    fparms = snmp_init_copy_parms(NULL, "");
    if (fparms == NULL)
        return (FLASH_OP_FAILURE);

    sts = flash_erase_command(fparms);
    free(fparms);
    return (sts ? FLASH_SUCCESS : FLASH_OP_FAILURE);
}

/*
 * copy flash entry to tftp host
 */
int snmp_flash2tftp (addrtype *address, char *filename)
{
    flash_copy_parms_t *fparms;
    boolean sts;

    fparms = snmp_init_copy_parms(address, filename);
    if (fparms == NULL)
        return (FLASH_OP_FAILURE);

    sts = copy_flash_tftp(fparms);
    free(fparms);
    return (sts ? FLASH_SUCCESS : FLASH_OP_FAILURE);
}

/*
 * copy from tftp host to flash entry
 */
int snmp_tftp2flash (addrtype *address, char *filename)
{
    flash_copy_parms_t *fparms;
    boolean sts;

    fparms = snmp_init_copy_parms(address, filename);
    if (fparms == NULL)
        return (FLASH_OP_FAILURE);

    sts = copy_tftp_flash(fparms);
    free(fparms);
    return (sts ? FLASH_SUCCESS : FLASH_OP_FAILURE);
}

/*
 * get the name of a file in this puppy. The file is referenced by a
 * sequence number.
 */
uchar *snmp_flash_dir_filename (int i)
{
    char *system_flash;

    system_flash = reg_invoke_flash_device_default();
    return (system_flash ? flash_file_name(system_flash, i) : NULL);
}

/*
 * is one of our puppy's parts a goner ?
 */
boolean snmp_flash_filedeletestatus (int i)
{
    char *system_flash;

    system_flash = reg_invoke_flash_device_default();
    if (!system_flash)  /* No system flash */
	return (TRUE);

    /* 
     * Return TRUE if file NOT valid; FALSE if valid
     */
    return (flash_file_status(system_flash, i) != FLASH_FILE_VALID);
}

/*
 * get the size of one of our puppy's internal parts
 */
int snmp_flash_filesize (int i)
{
    char *system_flash;

    system_flash = reg_invoke_flash_device_default();
    return (system_flash ? flash_file_size(system_flash, i) : 0);
}
