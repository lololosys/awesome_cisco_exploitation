/* $Id: snmp_flashmib_cmnds.c,v 3.3.60.5 1996/06/28 23:05:33 hampton Exp $
 * $Source: /release/112/cvs/Xsys/dev/snmp_flashmib_cmnds.c,v $
 *------------------------------------------------------------------
 * snmp_flashmib_cmnds.c - High level flash routines to support the
 *			   Dual Flash Bank (DFB) flash MIB
 *
 * April 1995, Shalabh Agarwal
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snmp_flashmib_cmnds.c,v $
 * Revision 3.3.60.5  1996/06/28  23:05:33  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.60.4  1996/05/21  06:26:42  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.3.60.3  1996/05/07  00:38:14  ssangiah
 * CSCdi56357:  Misleading erase command behavior in the RSP file system.
 *   o Remove erase command from the RSP file system parse chain.
 *   o Clean up the RSP file system parse chain.
 *   o Fix problem with return of error codes by the flash MIB agent.
 * Branch: California_branch
 *
 * Revision 3.3.60.2  1996/04/22  18:57:50  ssangiah
 * CSCdi54920:  Need to add support for the flash MIB on c7200.
 * Branch: California_branch
 *
 * Revision 3.3.60.1  1996/03/18  19:13:02  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  08:43:33  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  13:46:19  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  09:01:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:15:45  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:20:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:54:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/13  04:06:55  hampton
 * The Fall of the House of Msclock.  [CSCdi40182]
 *
 * Revision 2.1  1995/06/07 20:25:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdlib.h>		/* for atoi */
#include <ciscolib.h>
#include "subsys.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "../dev/flash_defines.h"
#include "../dev/flash_services.h"
#include "../dev/sr_flashmib.h"
#include "../dev/sr_flashmibdefs.h"
#include "../dev/snmp_flashmib_cmnds.h"
#include "../dev/flash_cmnds.h"
#include "../dev/file_io.h"
#include "../snmp/chassismib_registry.h"
#include "../snmp/sr_old_lchassismib.h"     /* for make_card_indexOID() */
#include "../snmp/snmp_api.h"

static int snmp_copy_op_status(int);

/*
 ******************** Local functions ******************************
 */
/*
 * Malloc and set up data structure with all parameters for the copy.
 * First ensure that system flash is present.
 */
static flash_copy_parms_t *snmp_init_copy_parms (addrtype *address, 
                                                 char *src_file, char *dst_file,
						 boolean erase, int direction)
{
    flash_copy_parms_t *fparms;
    char *system_flash;

    fparms = malloc(sizeof(flash_copy_parms_t));
    if (fparms == NULL)
	return (NULL);

    fparms->snmp_sts = SNMP_NO_ERROR;

    /* If we are writing to flash, we need to parse the destination filename
     * to get the device name and the filename.  Since the parse routine
     * stores the filename in sfname, we'll copy sfname to dfname.
     */
    if (direction == TO_FLASH) {
	/* If no destination filename, default device to system flash, and
	 * filename to source filename. */
	if (strlen(dst_file) == 0) {
	    system_flash = reg_invoke_flash_device_default();
	    sprintf(dst_file, "%s:%s", system_flash, src_file);
	}
	if (snmp_process_name(dst_file, fparms)) {
	    strcpy(fparms->dfname, fparms->sfname);
	    strcpy(fparms->sfname, src_file);
	}
	else 
	    fparms->snmp_sts = SNMP_DEVICE_OPEN_ERR;
    } 
    /* If we are reading from flash, we need to parse the source filename
     * to get the device name and the filename.  
     */ 
    else {
	if (!snmp_process_name(src_file, fparms))
	    fparms->snmp_sts = SNMP_DEVICE_OPEN_ERR;

	if (strlen(dst_file) != 0)
	    strcpy(fparms->dfname, dst_file);
	else
	    strcpy(fparms->dfname, fparms->sfname);
    }
    if (address != NULL)
	address_copy(&fparms->addr, address);

    /* Need to check if device is present else abort operation */
    if (snmp_flash_dev_removed(fparms->devnm))
	fparms->snmp_sts = SNMP_DEVICE_OPEN_ERR;

    fparms->prompt = 0;
    fparms->verbose = 0;
    fparms->erase = 0;
    fparms->sts = 0;

    if (erase)
	fparms->erase = FILE_FLAGS_ERASE;

    return (fparms);
}


/*
 * Initialize the flash devices database that is used by the SNMP
 * related code to map device numbers to actual devices in the
 * system. Note that device numbers start from 1.
 */
int snmp_init_dev_database (void)
{
    return flash_init_dev_database();
}

/*
 * Given the device number and partition numbers, this function makes 
 * the character string that refers to the device under consideration.
 * This device name can then be passed to lower level functions to 
 * perform operations on or extract information about the device.
 */
void snmp_make_devnm (int devnum, int partnum, char *devname)
{
    if (!flash_make_devnm(devnum, partnum, devname))
        *devname = '\0'; /* Initialize devname to an empty string */
}

/*
 * This function will initialize the appropriate fields in fparms
 * after parsing them from the provided destination name. Note that
 * though leading and trailing white space is allowed in specifying
 * the destination name, none is allowed within the destination 
 * name string itself. Also, only numeric characters are allowed in
 * specifying the partition number. The fparms structure initialized 
 * here is later used by the lower layer functions.
 */
boolean snmp_process_name (char *dest_name, flash_copy_parms_t *fparms)
{
    char tmp_devnm[FLASH_PATH_MALLOC_LEN], devnm[FLASH_DEV_NAME_LEN];
    char *strptr, *tmptr, *first_colon, *sec_colon;
    ulong length, indx;
    
    /* We are in the SNMP path; Ask lower layers to be quiet */
    fparms->prompt = 0;
    fparms->verbose = 0;

    length = strlen(dest_name);
    if (length >= FLASH_PATH_MALLOC_LEN) /* Too long a path name */
        return (FALSE);

    /* Make a local copy of the destination name */
    strcpy(tmp_devnm, dest_name);
    strptr = deblank(tmp_devnm); /* Get rid of leading white space */
    /* Get rid of trailing white space */
    if (strptr) {
        tmptr = strptr + strlen(strptr) - 1;  /* pt to last char of string */
        while ((tmptr >= strptr) && ((*tmptr == ' ') || (*tmptr == '\t')))
            *tmptr-- = 0;
    }
    else  /* Just a blank string provided; not good */
        return (FALSE);

    /* No white space allowed within the path specified */
    length = strlen(strptr);
    for (indx = 0; indx <= length; indx++)
        if ((*(strptr + indx) == ' ') || (*(strptr + indx) == '\t'))
            return (FALSE);
    
    /* Now look for colons in the destination name string */
    first_colon = strchr(strptr, FLASH_PATH_SEPARATOR);
    if (first_colon)  /* We found a ':' */
        sec_colon = strchr(first_colon + 1, FLASH_PATH_SEPARATOR);
    else
        sec_colon = NULL;

    if (sec_colon) { /* <dev>:<part>:<file> case */
        *sec_colon++ = 0;  /* term "<dev>:<part>" string; point to <file> */
        /* Make sure the <part> parameter is all numeric */
        for (indx = 1; *(first_colon + indx) != '\0'; indx++)
            if (!isdigit(*(first_colon + indx)))
                return (FALSE);
        strcpy(fparms->sfname, sec_colon);  /* copy <filename> to sfname */
        strcpy(devnm, strptr); 
	if (flash_device_is_valid(devnm))
	    strcpy(fparms->devnm, strptr); /* copy <dev>:<part> to devnm */
	else 
	    return (FALSE);
         /* Reject dest name if invalid partition number specified */
        if (atoi(first_colon + 1) > flash_dev_num_partns(fparms->devnm))
            return (FALSE);
        return (TRUE);
    }

    if (first_colon) { /* <dev>:<file> or <part>:<file> case */
        *first_colon++ = 0;  /* term "<dev>" or "<part>" string; */
        if(isdigit(*strptr)) { /* <part>:<file> case */
            /* Make sure the <part> parameter is all numeric */
            for (indx = 1; *(strptr + indx) != '\0'; indx ++)
                if (!isdigit(*(strptr + indx)))
                    return (FALSE);
	    /* Prefix the default device name to the partn number */
	    sprintf(fparms->devnm, "%s:%s", 
	           reg_invoke_flash_device_default(), strptr); 
            /* Reject dest name if invalid partition number specified */
            if (atoi(strptr) > flash_dev_num_partns(fparms->devnm))
                return (FALSE);
        } 
        else { /* <dev>:<file> case */
	    strcpy(devnm, strptr); 
	    if (flash_device_is_valid(devnm))
		strcpy(fparms->devnm, strptr);
	    else 
		return (FALSE);
	}
        /* Copy the filename to sfname */
        strcpy(fparms->sfname, first_colon);
        return (TRUE);
    }
    else { /* <file> case */
	char* cp;

        /* Initialize devnm with the default device name */
	cp = reg_invoke_flash_device_default();
	if (!cp)
	    cp = reg_invoke_default_flash_device();
	if (!cp)
	    return FALSE;
        strcpy(fparms->devnm, cp);
        strcpy(fparms->sfname, strptr); /* Copy filename to sfname */
        return (TRUE);
    }
}


/*
 * Wrapper functions to serve SNMP queries for objects in the various
 * information groups in the flash MIB. The wrapper functions for the
 * various groups appear in the same order that the groups themselves
 * appear in the flash MIB.
 */

/***********************************************************************
 * Wrapper functions for SNMP queries for objects in the Device 
 * Information Group.
 ***********************************************************************/

/*
 * Number of flash devices in the system. This uses the device database
 * built by snmp_init_dev_database. Hence, a call to this function should
 * be made onlt after the database has been initialized.
 */
int snmp_flash_device_count (void)
{
    return (flash_device_count());
}

/* Storage capacity of the flash device */
ulong snmp_flash_dev_size (char *devnm)
{
    return (flash_dev_size(devnm));
}

/* Minimum partition size of the device */
ulong snmp_flash_dev_min_partn_size (char *devnm)
{
    return (flash_dev_min_partn_size(devnm));
}

/*
 * Maximum number of partitions possible on device 
 * A value of 1 means partitioning not possible 
 */
int snmp_flash_dev_max_partns (char *devnm)
{
    return (flash_dev_max_partns(devnm));
}

/* Number of partitions in devnm */
int snmp_flash_dev_num_partns (char *devnm)
{
    return (flash_dev_num_partns(devnm));
}

/* Number of chips in device */
int snmp_flash_device_chipcnt (char *devnm)
{
    return (flash_device_chipcnt(devnm));
}

/* Descriptive name for device */
char *snmp_flash_dev_desc (char *devnm)
{
    return (flash_dev_desc(devnm));
}

/*
 * Controller on which device sits - in case of low end
 * a null is returned for the main processor board 
 */
char *snmp_flash_dev_cntlr (char *devnm)
{
    return (flash_dev_cntlr(devnm));
}

/* 
 * Index in the chassis mib card table for the 
 * controller of the flash
 */
OID *snmp_flash_dev_card_index (char* devnm)
{
    int slot;
    int card_index;
    OID *card_index_oid;

    slot = flash_device_slot(devnm);
    card_index = reg_invoke_chassismib_find_slot_card_index(slot);
    card_index_oid = make_card_indexOID(card_index);

    return (card_index_oid);
}

/* Vpp status of the flash device */
int snmp_flash_dev_vpp (char *devnm)
{
    switch (flash_dev_vpp(devnm)) {
	case VPP_OFF:
	    return (D_ciscoFlashDeviceProgrammingJumper_notInstalled);
	    break;
	case VPP_ON:
	    return (D_ciscoFlashDeviceProgrammingJumper_installed);
	    break;
	case VPP_UNKNOWN:
	default:
	    return (D_ciscoFlashDeviceProgrammingJumper_unknown);
	    break;
    }
}

/* System time when the device was last repartitioned */
sys_timestamp snmp_flash_dev_init_t (char *devnm)
{
    return (flash_dev_init_t(devnm));
}

/*
 * Is this a user removable device 
 * At present no platforms support this facility
 */
boolean snmp_flash_dev_removable (char *devnm)
{
    if (flash_dev_removable(devnm))
	return (D_ciscoFlashDeviceRemovable_true);
    else
	return (D_ciscoFlashDeviceRemovable_false);
}

/*
 * Is the removable device present in its PCMCIA slot
 * At present no platforms support this facility
 */
boolean snmp_flash_dev_removed (char *devnm)
{
    return (flash_dev_removed(devnm));
}

/***********************************************************************
 * Wrapper functions for SNMP queries for objects in the Chip
 * Information Group.
 ***********************************************************************/
/* Get the manufacturer and device code for the chip.
 * The chip code is in the format of <manufacturer code>:<device code>.
 * Each of size one byte.
 */
void snmp_flash_chipcode (char *dev_name, int chip_num, uchar *chipcode_str)
{
    uchar temp_str[6];  /* enough to hold upper byte, lower byte and : */
    uint i;
    int code;

    if ((code = flash_chip_code(dev_name, chip_num)) == 0)
	sprintf(temp_str, "0000\0");
    else
	sprintf(temp_str, "%x", code);
    
    /* Now add : between the upper and lower byte. */
    for (i = 0; i < strlen(temp_str)/2; i++)
	chipcode_str[i] = temp_str[i];
    chipcode_str[i] = ':';
    for (i = 2; i < strlen(temp_str); i++)
	chipcode_str[i+1] = temp_str[i];
    chipcode_str[i+1] = '\0';

}

/* Get the flash chip description corresponding to the chip code,
 * in the format of <manufacturer> <chip type>.  i.e.  INTEL 27F008SA
 */
uchar *snmp_flash_chipdesc (char *dev_name, int chip_num)
{
    return ((uchar *)flash_chip_desc(dev_name, chip_num));
}

/* Get the number of write retries that were done on the chip */ 
int snmp_flash_chip_wretries (char *dev_name, int chip_num)
{
    return (flash_chip_wretries(dev_name, chip_num));
}

/* Get the number of erase retries that were done on the chip */ 
int snmp_flash_chip_eretries (char *dev_name, int chip_num)
{
    return (flash_chip_eretries(dev_name, chip_num));
}

/* Get the max number of write retries */
int snmp_flash_chip_max_wretries (char *dev_name)
{
    return (flash_chip_max_wretries(dev_name));
}

/* Get the max number of erase retries */
int snmp_flash_chip_max_eretries (char *dev_name)
{
    return (flash_chip_max_eretries(dev_name));
}

/***********************************************************************
 * Wrapper functions for SNMP queries for objects in the Partition
 * Information Group.
 ***********************************************************************/

/* Partition number of device devnm */
long snmp_flash_partition_number (char *devnm)
{
    long part_num;
    part_num = flash_partition_number(devnm);
    return(part_num);
}

/* Start and end chip boundaries for device devnm */
void snmp_flash_partition_chip_bounds (char *devnm, long *startchip,
                                                    long *endchip)
{
    flash_partition_chip_bounds(devnm, startchip, endchip);
    return;
}

/* Size of devnm */
long snmp_flash_partition_size (char *devnm)
{
    long size = 0;
    size = flash_partition_size(devnm);
    return(size);
}

/* Free space in devnm */
long snmp_flash_partition_free_space (char *devnm)
{
    long free_space = 0;
    free_space = flash_partition_free_space(devnm);
    return(free_space);
}

/* Number of files in devnm */
long snmp_flash_partition_filecount (char *devnm)
{
    long filecount = 0;
    filecount = flash_partition_filecount(devnm);
    return(filecount);
}

/* Checksum algorithm for the file system on devnm */
long snmp_flash_partition_chksum_algo (char *devnm)
{
    long chksum_algo = D_ciscoFlashPartitionChecksumAlgorithm_undefined;
    chksum_algo = flash_partition_chksum_algo(devnm);
    return(chksum_algo);
}

/* The status (ro, rw, rff etc.) of devnm */
long snmp_flash_partition_status (char *devnm)
{
    long status = D_ciscoFlashPartitionStatus_readOnly;
    status = flash_partition_status(devnm);
    return(status);
}

/* Copy mode (direct, rxbootFLH etc.) for copies into devnm */
long snmp_flash_partition_upgrade_method (char *devnm)
{
    long method = D_ciscoFlashPartitionUpgradeMethod_unknown;
    method = flash_partition_upgrade_method(devnm);
    return(method);
}

/* String alias for devnm */
char *snmp_flash_partition_name (char *devnm)
{
    char *part_name = NULL;
    part_name = flash_partition_name(devnm);
    return(part_name);
}

/* Does devnm need erasure before a copy can be done into it? */
long snmp_flash_partition_need_erasure (char *devnm)
{
    long need_erasure = 1; /* TRUE by default */
    need_erasure = flash_partition_need_erasure(devnm);
    if (!need_erasure)
        need_erasure = 2; /* TruthValue is false */
    else
        need_erasure = 1; /* TruthValue is true */
    return(need_erasure);
}

/* Maximum file name length supported by file system on devnm */
long snmp_flash_partition_filename_length (char *devnm)
{
    long length = 0;
    length = flash_partition_filename_length(devnm);
    return(length);
}

/***********************************************************************
 * Wrapper functions for SNMP queries for objects in the File
 * Information Group.
 ***********************************************************************/
/* Get the file number.  Need to add 1 because the file index in the SNMP 
 * code starts from 1, but the index in the low level code starts from 0. */
int snmp_flash_fileindx (char *devnm, int file_num)
{
    return (flash_file_index(devnm, file_num)+1);
}

/*
 * get the size of one of our puppy's internal parts
 */
int snmp_flash_file_size (char *dev_name, int i)
{

    return (flash_file_size(dev_name, i));
}

/* Get the checksum of a file. */
void snmp_flash_filechksum (char *dev_name, int i, uchar *chksum_str)
{
    sprintf(chksum_str, "0x%x", flash_file_chksum(dev_name, i));
}

/*
 * get the status of a file
 */
int snmp_flash_filestatus (char *dev_name, int i)
{

    return (flash_file_status(dev_name, i));
}

/*
 * get the name of a file in this puppy. The file is referenced by the
 * device number, partition number and file number.
 */
uchar *snmp_flash_filename (char* dev_name, int i)
{

    return (flash_file_name(dev_name, i));
}

/***********************************************************************
 * Wrapper functions for SNMP Copy Operation Table. 
 ***********************************************************************/
/*
 * copy from tftp host to flash 
 */
int snmp_tftp_to_flash (addrtype *address, char *sfname, char *dfname, 
			long *time_elapsed, boolean erase)
{
    flash_copy_parms_t *fparms;
    int sts = SNMP_NO_ERROR;

    *time_elapsed = 0;

    fparms = snmp_init_copy_parms(address, sfname, dfname, erase, TO_FLASH);
    if (fparms == NULL) 
        return (D_ciscoFlashCopyStatus_copyNoMemory);

    /* Check to see if the device exists */
    if ((sts = fparms->snmp_sts) != SNMP_NO_ERROR) {
	free (fparms);
	return (snmp_copy_op_status(sts));
    }

    /* tftp to flash is disallowed if the copy mode of the device is
     * FS_COPY_UNKNOWN */
    if (flash_partition_upgrade_method(fparms->devnm) == FS_COPY_UNKNOWN) {
	free (fparms);
	return (D_ciscoFlashCopyStatus_copyDeviceNotProgrammable);
    }
    
    if (copy_tftp_flash(fparms)) 
	sts = D_ciscoFlashCopyStatus_copyOperationSuccess;
    else 
	sts = snmp_copy_op_status(fparms->snmp_sts);
		
    *time_elapsed = fparms->elapsed_time;
    free(fparms);
    return (sts);
}

int snmp_flash_to_tftp (addrtype *address, char *sfname, char *dfname,
                        long *time_elapsed)
{
    flash_copy_parms_t *fparms;
    int sts = SNMP_NO_ERROR;

    *time_elapsed = 0;

    fparms = snmp_init_copy_parms(address, sfname, dfname, FALSE, FROM_FLASH);
    if (fparms == NULL)
        return (D_ciscoFlashCopyStatus_copyNoMemory);

    /* Check to see if the device exists */
    if ((sts = fparms->snmp_sts) != SNMP_NO_ERROR) {
        free (fparms);
        return (snmp_copy_op_status(sts));
    }

    if (copy_flash_tftp(fparms))
        sts = D_ciscoFlashCopyStatus_copyOperationSuccess;
    else
        sts = snmp_copy_op_status(fparms->snmp_sts);
    *time_elapsed = fparms->elapsed_time;
    free(fparms);
    return (sts);
}

/*
 * copy from rcp host to flash 
 */
int snmp_rcp_to_flash (addrtype *address, char *sfname, char *dfname, 
		       char *user, long *time_elapsed, boolean erase)
{
    flash_copy_parms_t *fparms;
    int sts = SNMP_NO_ERROR;

    *time_elapsed = 0;

    fparms = snmp_init_copy_parms(address, sfname, dfname, erase, TO_FLASH);
    if (fparms == NULL) 
        return (D_ciscoFlashCopyStatus_copyNoMemory);

    /* Check to see if the device exists */
    if ((sts = fparms->snmp_sts) != SNMP_NO_ERROR) {
	free (fparms);
	return (snmp_copy_op_status(sts));
    }

    /* Copy to flash is disallowed if the copy mode of the device is
     * FS_COPY_UNKNOWN */
    if (flash_partition_upgrade_method(fparms->devnm) == FS_COPY_UNKNOWN) {
	free (fparms);
	return (D_ciscoFlashCopyStatus_copyDeviceNotProgrammable);
    }
    
    /* Store the rcp user name */
    fparms->rcp_username = user;
    
    if (copy_rcp_flash(fparms))
	sts = D_ciscoFlashCopyStatus_copyOperationSuccess;
    else 
	sts = snmp_copy_op_status(fparms->snmp_sts);
    *time_elapsed = fparms->elapsed_time;
    free(fparms);
    return (sts);
}

int snmp_flash_to_rcp (addrtype *address, char *sfname, char *dfname,
                       char *user, long *time_elapsed)
{
    flash_copy_parms_t *fparms;
    int sts = SNMP_NO_ERROR;

    *time_elapsed = 0;

    fparms = snmp_init_copy_parms(address, sfname, dfname, FALSE, FROM_FLASH);
    if (fparms == NULL)
        return (D_ciscoFlashCopyStatus_copyNoMemory);

    /* Check to see if the device exists */
    if ((sts = fparms->snmp_sts) != SNMP_NO_ERROR) {
        free (fparms);
        return (snmp_copy_op_status(sts));
    }
    
    /* Store the rcp user name */
    fparms->rcp_username = user;

    if (copy_flash_rcp(fparms))
        sts = D_ciscoFlashCopyStatus_copyOperationSuccess;
    else
        sts = snmp_copy_op_status(fparms->snmp_sts);
    *time_elapsed = fparms->elapsed_time;
    free(fparms);
    return (sts);
}

/*
 * Copy FLH log to tftp server
 */
int snmp_flhlog_to_server (addrtype *address, char *dfname,
                           long *time_elapsed)
{
    flash_copy_parms_t *fparms;
    char *tmptr;
    int sts = SNMP_NO_ERROR;
   
    *time_elapsed = 0;
    /* First make sure that this command is supported on this platform */
    if (reg_invoke_flh_version() >= FLASH_BASIC_FLH_SUPPORT) { 
        tmptr = deblank(dfname); /* Get rid of leading white space */
        dfname = tmptr;
        if (!strlen(dfname))  /* No destination name given */
            sprintf(dfname, "%s-flh-log", hostname);

        /* Malloc a flash_copy_parms_t structure */
        fparms = malloc(sizeof(flash_copy_parms_t));
        if (fparms == NULL)
            return (D_ciscoFlashCopyStatus_copyNoMemory);
        /* Ask lower layers to be quiet when doing their jobs */
        fparms->prompt = 0;
        fparms->verbose = 0;
        if (address != NULL)
            address_copy(&fparms->addr, address);
        strcpy(fparms->dfname, dfname);
    
        /* Invoke lower layer function to do the copy */
        if (reg_invoke_copy_flhlog_tftp(fparms)) 
            sts = D_ciscoFlashCopyStatus_copyOperationSuccess;
        else 
            sts = snmp_copy_op_status(fparms->snmp_sts);

	*time_elapsed = fparms->elapsed_time;
        free(fparms);
    }
    else
        sts = D_ciscoFlashCopyStatus_copyInvalidOperation;

    return (sts);
}

/*
 * Convert flash error codes to Copy status
 */
static int snmp_copy_op_status (int flash_status)
{
    switch (flash_status) {
	case SNMP_NO_ERROR:
	    return (D_ciscoFlashCopyStatus_copyOperationSuccess);
	    break;
	case SNMP_INVALID_OP:
	    return (D_ciscoFlashCopyStatus_copyInvalidOperation);
	    break;
	case SNMP_INVALID_PROTO:
	    return (D_ciscoFlashCopyStatus_copyInvalidProtocol);
	    break;
	case SNMP_INVALID_SRC_FILE:
	    return (D_ciscoFlashCopyStatus_copyInvalidSourceName);
	    break;
	case SNMP_INVALID_DEST_FILE:
	    return (D_ciscoFlashCopyStatus_copyInvalidDestName);
	    break;
	case SNMP_INVALID_SRVR_ADDR:
	    return (D_ciscoFlashCopyStatus_copyInvalidServerAddress);
	    break;
	case SNMP_DEVICE_BUSY:
	    return (D_ciscoFlashCopyStatus_copyDeviceBusy);
	    break;
	case SNMP_DEVICE_OPEN_ERR:
	    return (D_ciscoFlashCopyStatus_copyDeviceOpenError);
	    break;
	case SNMP_DEVICE_ERR:
	    return (D_ciscoFlashCopyStatus_copyDeviceError);
	    break;
	case SNMP_DEVICE_READONLY:
	    return (D_ciscoFlashCopyStatus_copyDeviceNotProgrammable);
	    break;
	case SNMP_DEVICE_FULL:
	    return (D_ciscoFlashCopyStatus_copyDeviceFull);
	    break;
	case SNMP_FILE_OPEN_ERR:
	    return (D_ciscoFlashCopyStatus_copyFileOpenError);
	    break;
	case SNMP_FILE_TRANSFER_ERR:
	    return (D_ciscoFlashCopyStatus_copyFileTransferError);
	    break;
	case SNMP_FILE_CHECKSUM_ERR:
	    return (D_ciscoFlashCopyStatus_copyFileChecksumError);
	    break;
 	case SNMP_LOW_RESOURCES:
	    return (D_ciscoFlashCopyStatus_copyNoMemory);
	    break;
	default:
	    return (D_ciscoFlashCopyStatus_copyUnknownFailure);
	    break;
    }
}

/*************************************************************************
 * Wrapper function for the SNMP Partitioning Operations table.
 *************************************************************************/
boolean partition_flash_snmp_wrapper (ciscoFlashPartitioningEntry_t *entry)
{
    return (partition_flash_snmp(entry));
}

/*************************************************************************
 * Wrapper function for the SNMP Miscellaneous  Operations table.
 *************************************************************************/
/*
 * Function to translate the error codes set in the snmp_sts field
 * of the flash_copy_parms_t structure to SNMP specific error codes
 * for the MiscOp family of flash operations.
 */
int snmp_copy_misc_op_status (int flash_status)
{
    switch (flash_status) {
        case SNMP_NO_ERROR:
            return (D_ciscoFlashMiscOpStatus_miscOpOperationSuccess);
            break;
	case SNMP_INVALID_OP:
	    return (D_ciscoFlashMiscOpStatus_miscOpInvalidOperation);
	    break;
        case SNMP_INVALID_SRC_FILE:
        case SNMP_INVALID_DEST_FILE:
            return (D_ciscoFlashMiscOpStatus_miscOpInvalidDestName);
            break;
        case SNMP_DEVICE_BUSY:
            return (D_ciscoFlashMiscOpStatus_miscOpDeviceBusy);
            break;
        case SNMP_DEVICE_OPEN_ERR:
            return (D_ciscoFlashMiscOpStatus_miscOpDeviceOpenError);
            break;
        case SNMP_DEVICE_ERR:
            return (D_ciscoFlashMiscOpStatus_miscOpDeviceError);
            break;
        case SNMP_DEVICE_READONLY:
            return (D_ciscoFlashMiscOpStatus_miscOpDeviceNotProgrammable);
            break;
        case SNMP_FILE_OPEN_ERR:
            return (D_ciscoFlashMiscOpStatus_miscOpFileOpenError);
            break;
        case SNMP_FILE_CHECKSUM_ERR:
            return (D_ciscoFlashMiscOpStatus_miscOpFileChecksumError);
            break;
        case SNMP_FILE_DELETE_FAIL:
            return (D_ciscoFlashMiscOpStatus_miscOpFileDeleteFailure);
            break;
        case SNMP_FILE_UNDELETE_FAIL:
            return (D_ciscoFlashMiscOpStatus_miscOpFileUndeleteFailure);
            break;
        case SNMP_LOW_RESOURCES:
            return (D_ciscoFlashMiscOpStatus_miscOpNoMemory);
            break;
        case SNMP_DEVICE_SQUEEZE_FAIL:
            return (D_ciscoFlashMiscOpStatus_miscOpSqueezeFailure);
            break;
        case SNMP_DEVICE_FORMAT_FAIL:
            return (D_ciscoFlashMiscOpStatus_miscOpFormatFailure);
            break;
        case SNMP_UNKNOWN_ERROR:
        default:
            return (D_ciscoFlashMiscOpStatus_miscOpUnknownFailure);
            break;
    }
}

/* SNMP wrapper for the flash device erase command */
boolean snmp_flash_erase (ciscoFlashMiscOpEntry_t *entry,
                          flash_copy_parms_t  *fparms)
{
    return(flash_erase_command(fparms));
}
    
/* SNMP wrapper for the flash file verify command */
boolean snmp_flash_verify (ciscoFlashMiscOpEntry_t *entry,
                           flash_copy_parms_t  *fparms)
{
    return(flash_file_checksum_verify(fparms));
}

/*
 * SNMP wrapper for the flash file delete command. 
 */
boolean snmp_flash_delete (ciscoFlashMiscOpEntry_t *entry,
                           flash_copy_parms_t  *fparms)
{
    return flash_file_delete(fparms);
}

/*
 * SNMP wrapper for the flash file undelete command.
 */
boolean snmp_flash_undelete (ciscoFlashMiscOpEntry_t *entry,
                             flash_copy_parms_t  *fparms)
{
    return flash_file_undelete(fparms);
}

/*
 * SNMP wrapper for the flash file squeeze command.
 */
boolean snmp_flash_squeeze (ciscoFlashMiscOpEntry_t *entry,
                            flash_copy_parms_t  *fparms)
{
    return flash_device_squeeze(fparms);
}

/*
 * SNMP wrapper for the flash file format command.
 */
boolean snmp_flash_format (ciscoFlashMiscOpEntry_t *entry,
                            flash_copy_parms_t  *fparms)
{
    return flash_device_format(fparms);
}
      
