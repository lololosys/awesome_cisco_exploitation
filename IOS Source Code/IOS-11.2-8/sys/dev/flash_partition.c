/* $Id: flash_partition.c,v 3.3.42.2 1996/07/17 03:16:01 echeng Exp $
 * $Source: /release/112/cvs/Xsys/dev/flash_partition.c,v $
 *------------------------------------------------------------------
 * flash_partition.c - Flash partitioning code.
 *
 * April 1995, Suresh Sangiah
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_partition.c,v $
 * Revision 3.3.42.2  1996/07/17  03:16:01  echeng
 * CSCdi54639:  Flashless 3000 does not boot
 * Branch: California_branch
 * Free up entry in both the two init_table; delete the useless flag
 * value DEV_UNCRT_CLEAR
 *
 * Revision 3.3.42.1  1996/03/18  19:12:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.1  1996/03/07  08:43:23  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3  1996/01/06  03:22:12  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.2  1995/11/17  09:01:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:19:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/30  18:52:10  widmer
 * CSCdi36635:  nv_add string should not contain newline
 * Fix dlsw NV generation
 *
 * Revision 2.1  1995/06/07  20:24:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdlib.h>		/* for atoi() */
#include <ciscolib.h>
#include "flash_defines.h"
#include "dev_io.h"
#include "../les/flash_fs_les.h"
#include "sr_flashmib.h"
#include "flash_partition.h"
#include "config.h"
#include "sys_registry.h"
#include "flash_registry.h"

/*
 * Flash device alias
 */
static char *system_flash = "flash";

static boolean config_cmnd_installed = FALSE;
static boolean config_cmnd_accepted  = FALSE;

static char *error_prefix =
    "\n%Error: ";
static char *msg_erasure_needed =
    "%sErasure of partition %d required.";
static char *msg_invalid_partcnt_param =
    "%sInvalid number of partitions specified.";
static char *msg_no_dev =
    "%sNo such device.";
static char *msg_toobig_partcnt_param =
    "%sDevice cannot support more than %d partitions.";
static char *msg_invalid_partsize_paramcnt =
    "%sToo %s partition size parameters.";
static char *msg_invalid_partsize_param =
    "%sInvalid partition size parameters(s).";
static char *msg_device_busy =
    "%sSystem flash in use.";
static char *msg_no_memory =
    "%sNo memory.";
static char *msg_bootrom_upgrade_needed =
    "%sBoot ROM upgrade needed to partition flash.";

/***************************************************************************
 * Support functions for the partitioning code. These functions are common
 * to both the configuration command and SNMP initiated partitioning
 * requests.
 ***************************************************************************/
/*
 * This is the default flash partitioning scheme on this platform.
 */
static void set_default_partition_bounds (char *devnm, int *part_bounds)
{
    int no_of_banks, no_of_parts, banks_per_part, part_no;
    devstat_t devinfo;

    dev_stat(devnm, &devinfo);
    if (devinfo.dev_banksize) {
    	no_of_banks = (devinfo.dev_size)/(devinfo.dev_banksize);
    } else {
	no_of_banks = 0;
    }
    banks_per_part = no_of_banks/DEFAULT_PART_CNT;
    /*
     * Take care of the case where there are not enough banks to have
     * the default number of partitions.
     */
    if (banks_per_part != 0)
        no_of_parts = DEFAULT_PART_CNT;
    else
        no_of_parts = 1;

   /*
    * If we cannot have equal sized partitions, then make the first
    * partition the largest one. All the others will be equal sized.
    */
    part_bounds[0] = no_of_parts;
    part_bounds[1] = 0;
    part_bounds[2] = no_of_banks - ((no_of_parts - 1) * banks_per_part);
    for (part_no = 3; part_no <= no_of_parts; part_no++)
        part_bounds[part_no] = part_bounds[part_no - 1] + banks_per_part;
    part_bounds[no_of_parts + 1] = no_of_banks;

    return;
}
/*
 * Check the partition bounds array and report whether the default
 * partitioning is specified.
 */
static boolean are_these_default_parts (int *part_bounds)
{
   /*
    * The following check to see if we are currently partitioned in the
    * the default manner will have to change if the default is something
    * other than one partition. We will then have to check not only for the
    * total number of partitions, but also for the size of each partition.
    */
    if (part_bounds[0] == DEFAULT_PART_CNT)  /* We have default parts */
        return(TRUE);
    return(FALSE);
}

/*
 * This is how the flash seems to be currently partitioned. This
 * function gets called only when the partitioning specified by
 * the config command (if any) and the default partitioning method
 * for this platform are not consistent with the way the currently
 * existing file systems in flash define the existing partitions.
 */
static void existing_fs_partition_bounds (dev_info_t *dev,
                                 fs_bounds_t *fsbounds, int *part_bounds)
{
    int indx, part_no;

    /*
     * Take care of the case where there is a hole before the first
     * file system in flash.
     */
    part_bounds[1] = 0;
    if (fsbounds[0].start_bank == 0)
        indx = 1;
    else
        indx = 0;
    part_no = 1;

    /* Start a partition at the beginnibg of each file system */
    while (fsbounds[indx].start_bank != -1) {
        part_no++;
        part_bounds[part_no] = fsbounds[indx].start_bank;
        indx++;
    }
    part_bounds[0] = part_no;
    /* Set the number of banks in part_bounds */
    part_bounds[part_no + 1] = (dev->dev_size)/(dev->dev_banksize);

    return;
}

/*
 * Examine the existing file systems in flash, and the requested
 * partitioning scheme and decide if they are consistent with each other.
 */
static boolean are_these_acceptable_partitions(fs_bounds_t *fsbounds,
                                               int *part_bounds,
                                               boolean verbose)
{
    int i, j, k;
    boolean is_exception, partition_found;

    /*
     * Condition 1: No partition boundary should fall within a file
     *              system. It is acceptable, however, if a partition
     *              boundary coincides with a file system starting bank.
     *              (Check also for the corner case where a file in a
     *              file system ends right at a bank boundary, and the
     *              the next file starts at the start of a bank boundary.
     *              In this case, it would be acceptable if a partition
     *              boundary falls at this place within the file system).
     */
    for (k = 1; k <= part_bounds[0]; k++)  {
        j = 0;
        while (fsbounds[j].start_bank != -1) {
            if((part_bounds[k] > fsbounds[j].start_bank) &&
               (part_bounds[k] <= fsbounds[j].end_bank)) {
                /* Check for the corner case */
                i = 0;
                is_exception = FALSE;
                while (fsbounds[j].exception_bank[i] != -1) {
                    if (part_bounds[k] == fsbounds[j].exception_bank[i]) {
                        is_exception = TRUE;
                        break;
                    }
                    i++;
                }
                if (!is_exception) {
                    if (verbose)
                        printf(msg_erasure_needed, error_prefix,
                               fsbounds[j].partnum);
                    return (FALSE);
                }
            }
            j++;
        }
    }

    /*
     * Condition 2: For every file system, there should exist a partition
     *              boundary that coincides with its starting bank number.
     */
    j = 0;
    while (fsbounds[j].start_bank != -1) {
        partition_found = FALSE;
        for (k=1; k<= part_bounds[0]; k++)
            if (part_bounds[k] == fsbounds[j].start_bank) {
                partition_found = TRUE;
                continue;
            }
        if (!partition_found) {
            if (verbose)
                printf(msg_erasure_needed, error_prefix, fsbounds[j].partnum);
            return (FALSE);
        }
        j++;
    }

    /* The Law of Acceptable Partitions Hath Been Met. */
    return (TRUE);
}

/*
 * This function decides what the partitioning scheme for flash on the
 * box will be.
 */
int decide_partitions (dev_info_t *dev, fs_bounds_t *fsbounds,
                                                 int *part_bounds){
    /*
     * Check if the default partitioning can be allowed.
     */
    set_default_partition_bounds(dev->dev_alias, part_bounds);
    if (are_these_acceptable_partitions(fsbounds, part_bounds, FALSE))
        return (part_bounds[0]);

    /*
     * The default partitioning scheme is not acceptable.
     * Keep the one that already exists on the device.
     */
    existing_fs_partition_bounds(dev, fsbounds, part_bounds);
    config_cmnd_installed = TRUE;
    return (part_bounds[0]);
}

/*
 * Checks to see if requested partitioning is the same as the partitioning
 * that already exists on the device.
 */
static boolean same_as_existing_partitions (char *devnm, int *part_bounds)
{
    int i, size, part_size_in_MB[FLASH_MAX_PART_CNT + 1];
    devstat_t devinfo;

    dev_stat(devnm, &devinfo);
    dev_gather_config_info(devnm, part_size_in_MB);
    for (i = 1; i <= part_bounds[0]; i++) {
        size = (part_bounds[i + 1] - part_bounds[i]) * devinfo.dev_banksize;
        size = size / ONE_MB ;          /* Size in megabytes */
        if (size != part_size_in_MB[i]) /* Compare with gathered info */
            return (FALSE);
    }
    return (TRUE);
}

/*
 * Callback function for NV generation
 */
static void write_partitions_to_nvram (parseinfo *csb, char *devnm)
{
    int i, part_size_in_MB[FLASH_MAX_PART_CNT + 1];

    if (!config_cmnd_installed) /* No config command present */
        return;

    dev_gather_config_info(devnm, part_size_in_MB);

    if (are_these_default_parts(part_size_in_MB)) {/* Default parts */
        config_cmnd_installed = FALSE; /* Do not show config command */
        return;
    }

    /* We have non default partitions configured */
    if (config_cmnd_installed) {
	nv_write(TRUE, "%s", csb->nv_command);
	nv_add(TRUE, " %d", part_size_in_MB[0]);
	for (i=1; i <= part_size_in_MB[0]; i++)
	    nv_add(TRUE, " %d", part_size_in_MB[i]);
	nv_write(TRUE,"!");
    }
}


/*
 * This function determines what partition number a file system resides in.
 * Given the file system bounds determined by sniffing, this will initialize
 * the partnum field of the fsbounds structure for all file systems on flash
 * with their corresponding partition numbers. This information is useful
 * for printing out informative error messages when a partitioning request 
 * cannot be satisfied.
 */
void flash_filesys_partnums (char *devnm, fs_bounds_t *fsbounds,
                             int *part_sizes_in_MB)
{
    devstat_t devinfo;
    int part_start_banks[FLASH_MAX_PART_CNT+1];
    int i;

    /*
     * First we need to find out which banks the partitions start on.
     */
    dev_stat(devnm, &devinfo);
    for (i = 1; i <= devinfo.dev_part_cnt; i++) {
        if (i == 1)
            part_start_banks[i] = 0; /* First partn has to start at bank 0 */
        else {
            /* This partition's starting bank is the sum of the previous
             * partition's start bank and the size of the previous partition
             * in banks.
             */
            part_start_banks[i] =
                part_start_banks[i-1] + 
                ((part_sizes_in_MB[i-1]*ONE_MB)/devinfo.dev_banksize);
        }
    }    

    /* Now, find which partition the sniffed file systems are in. This is 
     * done by locating the partition whose starting bank is the same as
     * the starting bank for a file system.
     */
    i = 1;
    while (fsbounds->start_bank != -1) {
        while (fsbounds->start_bank != part_start_banks[i])
            i++;
        fsbounds->partnum = i;
        fsbounds++;
    }
}

/**************************************************************************
 * Support functions specifically for SNMP initiated partitioning requests.
 **************************************************************************/
/*
 * This function extracts strings from the OctetStrings that SNMP uses.
 * The destination device for the partition command and the specified
 * sizes for the partitions are available after a call to this function.
 */
static boolean snmp_extract_strings(ciscoFlashPartitioningEntry_t *entry,
                                    char **dest_str, char **size_str)
{
    /* Allocate space for the destination device name */
    *dest_str= malloc(entry->ciscoFlashPartitioningDestinationName->length + 1);
    if (!(*dest_str)) { /* No memory */
        entry->ciscoFlashPartitioningStatus =  /* Set error status */
                         D_ciscoFlashPartitioningStatus_partitioningNoMemory;
        return(FALSE);
    }
    /* Copy over the destination device name */
    memcpy(*dest_str, entry->ciscoFlashPartitioningDestinationName->octet_ptr,
           entry->ciscoFlashPartitioningDestinationName->length);
    /* Null terminate the string */
    *(*dest_str + entry->ciscoFlashPartitioningDestinationName->length) = '\0';

    /* Allocate space for the string specifying partition sizes */
    *size_str = malloc(entry->ciscoFlashPartitioningPartitionSizes->length + 1);
    if (!(*size_str)) {  /* No memory */
        free(*dest_str); /* Release previously allocated memory */
        entry->ciscoFlashPartitioningStatus =  /* Set error status */
                         D_ciscoFlashPartitioningStatus_partitioningNoMemory;
        return(FALSE);
    }
    /* Copy over string specifying the partition sizes requested */
    memcpy(*size_str, entry->ciscoFlashPartitioningPartitionSizes->octet_ptr,
           entry->ciscoFlashPartitioningPartitionSizes->length);
    /* Null terminate the string */
    *(*size_str + entry->ciscoFlashPartitioningPartitionSizes->length) = '\0';

    return (TRUE);
}

/*
 * Validate the destination name provided by the user via SNMP
 * Currently the only valid destination device is system flash.
 * If we ever allow partitioning of other devices, this will
 * have to be changed appropriately.
 */
static boolean snmp_validate_dest_name (char *dest_name)
{
    char *tmptr, *workptr, *tailptr;
    tmptr = dest_name;

    /* Get rid of leading white space */
    workptr = tailptr = deblank(dest_name);
    /* Get rid of trailing white space */
    while ((*tailptr != ' ') && (*tailptr != '\t') && (*tailptr != '\0'))
        tailptr++;
    *tailptr = '\0';

    /* If dest name was an empty string, assume default device */
    if (strlen(workptr) == 0) {
        free(tmptr);
        return (TRUE);
    }

    /* Check to see if specified device is a valid one */
    if (!strcmp(system_flash, workptr)) {
        free(tmptr);
        return (TRUE);
    }
    free(tmptr);
    return (FALSE);
}

/*
 * This function parses the number of partitions requested via the
 * SNMP command, as also the sizes of the requested partitions, if
 * any sizes are provided. This also does some sanity checking of
 * the string containing the partition sizes. Note that size 
 * specifications for partitions should be separated by colons.
 */
static boolean snmp_parse_sizes (char *size_str, int *part_size)
{
    char *headptr, *tailptr, *tmptr;
    int c;
    boolean proceed;
    int indx, length;

    /* Initialize the part_size array; part_size[0] is already initialized */
    for (indx = 1; indx <= FLASH_MAX_PART_CNT; indx++)
        part_size[indx] = 0;
    tmptr = size_str; /* Keep a copy of this so that it can be freed later */

    /* Get rid of leading white space */
    headptr = tailptr = deblank(size_str);
    length = strlen(headptr);
    if (length == 0) {  /* Empty size string */
        free(tmptr);
        return (TRUE);
    }

    for (indx = 0; indx < length; indx++) {
        c = *(headptr + indx);
        if (!(c >= '0' && c <= '9')) { 
            if ((c == ' ') || (c == '\t') ||
                (c == ':')) {  /* White space or a colon */
                *(headptr + indx) = ':'; /* Replace white space with colon */
                continue;
            }
            else { /* Illegal character in the size string */
                free(tmptr);
                return (FALSE);
            }
        }
        continue;
    }

    proceed = TRUE;
    for (indx = 1; (indx <= FLASH_MAX_PART_CNT) && proceed; indx++) {
        tailptr = strchr(headptr, ':');
        if (strlen(tailptr)) { /* Found a colon */
            *tailptr = '\0';  /* Null terminate headptr */
            tailptr++;
            while (*tailptr == ':')  /* Skip all successive colons */
                tailptr++;
            part_size[indx] = atoi(headptr); /* Note specified size */
            headptr  = tailptr;
        }
        else { /* Colon not found */
            if (strlen(headptr))  /* Last partition size specification */
                part_size[indx] = atoi(headptr);
            proceed = FALSE;
        }
    }

    free(tmptr);
    return (TRUE);
}



/************************************************************************
 *             Partitioning Request Validation Functions.
 ************************************************************************/

/*
 * This function validates the partitioning request, whether through a
 * config command or via SNMP, to see if the request is valid. Thus,
 * this function lies in the code-paths for both SNMP initiated and
 * config command initiated partitioning requests.
 */
boolean validate_partitioning_request (char *devnm, int *part_size,
                                       int *part_bounds, boolean verbose)
{
    devstat_t devinfo;
    ulong req_size;
    int no_of_parts, part_no, sts;
    int i, no_of_banks, banks_per_part;

    no_of_parts = part_size[0];
    if (no_of_parts == 0) {/* Should not happen */
        if (verbose)
            printf(msg_invalid_partcnt_param, error_prefix);
        return (FALSE);
    }
    sts = dev_stat(devnm, &devinfo);
    if (sts < 0) {
        if (verbose)
            printf(msg_no_dev, error_prefix);
        return (FALSE);
    }
    no_of_banks = devinfo.dev_size / devinfo.dev_banksize;
    
    if (no_of_parts > no_of_banks) { /* Too many partitions specified */
        if (verbose)
            printf(msg_toobig_partcnt_param, error_prefix, no_of_banks);
        return (FALSE);
    }
    
    if (part_size[no_of_parts + 1] != 0) {/* Extra size parameter */
        if (verbose)
            printf(msg_invalid_partsize_paramcnt, error_prefix, "many");
        return (FALSE);
    }

    if (part_size[1] == 0) { /* Number of partitions specified; no sizes */
        /* Try to have equal sized partitions. If that's not possible,
         *  make the first one the largest. */
        banks_per_part = no_of_banks / no_of_parts;
        if (banks_per_part == 0) /* Specified number of partitions not poss. */           return (FALSE);
        part_bounds[0] = no_of_parts;
        part_bounds[1] = 0; /* First partition always starts at bank 0 */
        part_bounds[2] = no_of_banks - ((no_of_parts - 1) * banks_per_part);
        for (part_no = 3; part_no <= no_of_parts; part_no++)
            part_bounds[part_no] = part_bounds[part_no - 1] + banks_per_part;
        part_bounds[no_of_parts + 1] = no_of_banks;
        return (TRUE);
    }
    
    for (i = 0; i <= no_of_parts; i++)
        if (part_size[i] == 0) {
            if (verbose)
                printf(msg_invalid_partsize_paramcnt, error_prefix, "few");
            return (FALSE);
        }

    /* Calculate total size of requested partitions */
    req_size = 0;
    for (part_no = 1; part_no <= no_of_parts; part_no++)
        req_size += (part_size[part_no] * ONE_MB);
    if (req_size != devinfo.dev_size) {/* Req size not same as avail size */
        if (verbose)
            printf(msg_invalid_partsize_param, error_prefix);
        return (FALSE);
    }
    
    /* Make sure each partition size is an integral number of banks */
    for (part_no = 1; part_no <= no_of_parts; part_no++)
        if(((part_size[part_no] * ONE_MB) % devinfo.dev_banksize) != 0) {
            if (verbose)
                printf(msg_invalid_partsize_param, error_prefix);
            return (FALSE);
        }
    
    /* Passed all checks. Set the partition bounds array */
    part_bounds[0] = no_of_parts;
    part_bounds[1] = 0;
    for (part_no = 2; part_no <= no_of_parts; part_no++)
        part_bounds[part_no] = part_bounds[part_no - 1] +
                      (part_size[part_no - 1] * ONE_MB)/devinfo.dev_banksize;
    part_bounds[no_of_parts+1] = no_of_banks;

    return (TRUE);
}

/*
 * Validate the config command entered by the user. Make sure that there
 * just the right number of parameters, and the partition sizes, if
 * specified add up to the totlal size of the device.
 */
static boolean validate_config_request (char *devnm, parseinfo *csb,
                                             int *part_bounds)
{
    int part_size[FLASH_MAX_PART_CNT + 1];

    part_size[0] = GETOBJ(int,1);

    /* Collect all the parameters entered by the user */
    part_size[1] = GETOBJ(int, 2);
    part_size[2] = GETOBJ(int, 3);
    part_size[3] = GETOBJ(int, 4);
    part_size[4] = GETOBJ(int, 5);
    part_size[5] = GETOBJ(int, 6);
    part_size[6] = GETOBJ(int, 7);
    part_size[7] = GETOBJ(int, 8);
    part_size[8] = GETOBJ(int, 9);

    if (validate_partitioning_request(devnm, part_size, part_bounds, TRUE))
        return (TRUE);
    else
        return (FALSE);
}

/*
 * Validate the various parameters provided by the user via SNMP
 * for the partitioning operation.
 */
static boolean validate_snmp_request (char *devnm, 
                                      ciscoFlashPartitioningEntry_t *entry,
                                      int *part_bounds)
{
    int part_size[FLASH_MAX_PART_CNT + 1];
    char *dest_str = NULL; 
    char *size_str = NULL;
    devstat_t devinfo;

    /* Extract the destination device and partition size strings */
    if (!snmp_extract_strings(entry, &dest_str, &size_str))
        return (FALSE);

    /* Validate the destination device name provided */
    if (!snmp_validate_dest_name(dest_str)) {
        entry->ciscoFlashPartitioningStatus = /* Set error status */
                  D_ciscoFlashPartitioningStatus_partitioningInvalidDestName;
        free(size_str);
        return (FALSE);
    }

    /* Check to see if device is present */
    if ((dev_stat(devnm, &devinfo) < 0) || 
	 (devinfo.dev_state == DEV_STATE_REMOVED)) {
        entry->ciscoFlashPartitioningStatus = /* Set error status */
                  D_ciscoFlashPartitioningStatus_partitioningInvalidDestName;
        free(size_str);
        return (FALSE);
    }

    /* part_size[0] will store the number of partitions requested */
    part_size[0] = entry->ciscoFlashPartitioningPartitionCount;
    /* Sanity check the requested number of partitions */
    if ((part_size[0] < 1) || (part_size[0] > 8)) {
        entry->ciscoFlashPartitioningStatus =
            D_ciscoFlashPartitioningStatus_partitioningInvalidPartitionCount;
        free(size_str);
        return (FALSE);
    } 

    /*
     * Parse the partition sizes requested from the size string and
     * sanity check it.
     */
    if (!snmp_parse_sizes(size_str, part_size)) {
        entry->ciscoFlashPartitioningStatus =
            D_ciscoFlashPartitioningStatus_partitioningInvalidPartitionSizes; 
        return (FALSE);
    }

    /* Validate the partitioning request */
    if (validate_partitioning_request(devnm, part_size, part_bounds, FALSE)) { 
        return (TRUE);
    }
    else { /* Set error ststus */
        entry->ciscoFlashPartitioningStatus =
                 D_ciscoFlashPartitioningStatus_partitioningInvalidOperation;
        return (FALSE);
    }
}



/****************************************************************************
 *           The functions that actually do the partititioning.
 ****************************************************************************/
/*
 * Function common to the partitioning code path irrespective of whether
 * the partitioning request came via a config command or via SNMP.
 */
static int partition_flash_common (char *devnm, int *part_bounds,
                                   boolean verbose)
{
    dev_info_t *dev_new, *dev_old, *workptr;
    dev_cons_info_t *devcons;
    fs_bounds_t fsbounds[FLASH_MAX_PART_CNT];
    int part_sizes_in_MB[FLASH_MAX_PART_CNT+1];
    int i, indx;

    config_cmnd_accepted = FALSE;
    /* Initialize fsbounds */
    for (indx = 0; indx < FLASH_MAX_PART_CNT; indx++) {
        fsbounds[indx].start_bank = -1;
        fsbounds[indx].end_bank   = -1;
        fsbounds[indx].partnum    = -1;
        for (i = 0; i < FLASH_FILE_CNT; i++)
            fsbounds[indx].exception_bank[i] = -1;
    }
    
    /* Gather information about how the flash is currently partitioned */
    dev_gather_config_info(devnm, part_sizes_in_MB);

    devcons = dev_move(devnm); /* Move old data structs away */

    if (devcons == NULL) {  /* Someone is using the device */
        if (verbose)
            printf(msg_device_busy, error_prefix);
        return(DEVERR_EXCL);
    }
    dev_old = devcons->devinfo; /* Get a pointer to old structure */
    dev_new = dev_re_create(devcons); /* Make a new one */
    if (dev_new == NULL) {
        if (verbose)
            printf(msg_no_memory, error_prefix);
        dev_unmove(); /* Undo the move */
        return(DEVERR_NOMEM);
    }
    /* Sniff using new data structure for file systems on devivce */
    reg_invoke_file_system_sniff(DEVICE_LES_FLASH, dev_new->dev_alias,
                                 (void *)fsbounds);
    /* Associate the sniffed file systems with the partitions they reside on */
    flash_filesys_partnums(devnm, fsbounds, part_sizes_in_MB);

    if (are_these_acceptable_partitions(fsbounds, part_bounds, verbose)) {
        devcons->dev_part_cnt = part_bounds[0]; /* Update part_cnt */
        dev_clone(dev_new, part_bounds); /* Clone new structs */
        workptr = dev_new;
        /*
         * Initialize file system in each partition, but do not
         * regenerate file checksums.
         */
        for (indx = 1; workptr && (indx <= devcons->dev_part_cnt); indx++) {
            reg_invoke_flash_device_file_system(DEVICE_LES_FLASH,
                                            workptr->dev_sub_alias, FALSE);
                workptr = workptr->next_partition;
        }
        /* Now copy over file cksum info from old file system structures */
        reg_invoke_map_file_cksums(DEVICE_LES_FLASH, dev_old->fs_spec_info,
                                                       dev_new->fs_spec_info);
        /* Note initialization time */
        GET_TIMESTAMP(devcons->dev_init_time);
        config_cmnd_accepted = TRUE;
    }
    else { /* The requested partitions are not acceptable */
        config_cmnd_accepted = FALSE;
    }
    if (!config_cmnd_accepted) {
        dev_uncreate(dev_new, DEV_UNCRT_PARTIAL);
        dev_unmove(); /* Move the old structs back into dev_init_table */
    }
    else { /* Config command was accepted */
        reg_invoke_replace_flash_devices_entry(devnm, (void *)dev_new);
        dev_uncreate(dev_old, DEV_UNCRT_PARTIAL);
    }

    return(DEV_OK);
}

/*
 * The callback function for the partitioning config command.
 */
void partition_flash_command (parseinfo *csb)
{
    int part_bounds[FLASH_MAX_PART_CNT+2];
    
    /*
     * First check if the partitioning command is allowed on this 
     * box
     */
    if ((!reg_invoke_allow_flash_partitioning()) && (!csb->nvgen)) {
        printf(msg_bootrom_upgrade_needed, error_prefix);
        return;
    }
    
    if (csb->nvgen) {  /* NV generation is being done */
        write_partitions_to_nvram(csb, system_flash);
        return;
    }

    if (csb->sense) { /* There wasn't the 'no' prefix */
        if (!validate_config_request(system_flash, csb, part_bounds)) 
            return;
    }
    else   /* The no prefix was present */
        set_default_partition_bounds(system_flash, part_bounds);

    if (same_as_existing_partitions(system_flash, part_bounds)) {
        /* Check if this was a non-default request */
        if (!are_these_default_parts(part_bounds))
            config_cmnd_installed = TRUE;
        return;
    }

    partition_flash_common(system_flash, part_bounds, TRUE);

    if (config_cmnd_accepted) {
        if (csb->sense)
            config_cmnd_installed = TRUE;
        else
            config_cmnd_installed = FALSE;
    }
    return;
}

/*
 * The callback function for SNMP initiated partitioning requests.
 */
boolean partition_flash_snmp (ciscoFlashPartitioningEntry_t *entry)
{
    boolean default_parts = FALSE;
    int part_bounds[FLASH_MAX_PART_CNT+2];
    int sts;
    sys_timestamp  partitioning_time;

    /* Validate the request */
    if (!validate_snmp_request(system_flash, entry, part_bounds))
        return (FALSE);

    /* Check to see if flash is already in the requested state */
    if (same_as_existing_partitions(system_flash, part_bounds)) {
        /* Check if this was a non-default request */
        if (!(default_parts = are_these_default_parts(part_bounds)))
            config_cmnd_installed = TRUE;
        entry->ciscoFlashPartitioningTime = 0; /* since nothing was done! */
        entry->ciscoFlashPartitioningStatus =
                  D_ciscoFlashPartitioningStatus_partitioningOperationSuccess;
        return (TRUE);
    }

    GET_TIMESTAMP(partitioning_time);
    sts = partition_flash_common(system_flash, part_bounds, FALSE);
    if (sts == DEV_OK) 
        entry->ciscoFlashPartitioningTime = ELAPSED_TIME(partitioning_time);
    else {
        switch (sts) {
        case DEVERR_NOMEM:
            entry->ciscoFlashPartitioningStatus =
                        D_ciscoFlashPartitioningStatus_partitioningNoMemory;
            break;
        case DEVERR_EXCL:
            entry->ciscoFlashPartitioningStatus =
                      D_ciscoFlashPartitioningStatus_partitioningDeviceBusy;
            break;
        default:
            entry->ciscoFlashPartitioningStatus =
                  D_ciscoFlashPartitioningStatus_partitioningUnknownFailure;
            break;
        }
        return (FALSE);
    }

    if (config_cmnd_accepted) { /* Partitioning request was accepted */
        if (!default_parts)
            config_cmnd_installed = TRUE;
        else
            config_cmnd_installed = FALSE;
        entry->ciscoFlashPartitioningStatus =
                  D_ciscoFlashPartitioningStatus_partitioningOperationSuccess;
        return (TRUE);
    }
    else { /* Partitioning request was denied */
        entry->ciscoFlashPartitioningStatus =
                 D_ciscoFlashPartitioningStatus_partitioningInvalidOperation;
        return (FALSE);
    }
}
