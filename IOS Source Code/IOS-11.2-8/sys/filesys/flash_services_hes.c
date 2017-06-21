/* $Id: flash_services_hes.c,v 3.1.2.3 1996/08/28 12:42:38 thille Exp $
 * $Source: /release/112/cvs/Xsys/filesys/flash_services_hes.c,v $
 *------------------------------------------------------------------
 * flash_services_hes.c : Flash utility routines for HES.
 *
 * April 1996, Suresh Sangiah
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_services_hes.c,v $
 * Revision 3.1.2.3  1996/08/28  12:42:38  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.2  1996/05/07  00:39:38  ssangiah
 * CSCdi56357:  Misleading erase command behavior in the RSP file system.
 *   o Remove erase command from the RSP file system parse chain.
 *   o Clean up the RSP file system parse chain.
 *   o Fix problem with return of error codes by the flash MIB agent.
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/04/22  18:58:13  ssangiah
 * CSCdi54920:  Need to add support for the flash MIB on c7200.
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/20  03:13:19  ssangiah
 * Placeholder files for new platform flash MIB support.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../filesys/msg_filesys.c"	/* Not a typo, see logger.h */
#include <ciscolib.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "../snmp/snmp_api.h"
#include "../dev/sr_flashmib.h"
#include "../snmp/sr_old_lchassismib.h"
#include "../dev/flash_defines.h"
#include "../dev/file_io.h"
#include "../dev/dev_io.h"
#include "../dev/sr_flashmibdefs.h"
#include "../dev/sr_flashmibtype.h"
#include "../dev/snmp_flashmib_cmnds.h"
#include "../dev/flash_services.h"
#include "../filesys/ralib.h"
#include "../filesys/filesys.h"
#include "../filesys/fslib_internals.h"
#include "../filesys/flash_config.h"
#include "../src-4k-c7100/c7100_flashmib.h"
/* possible conflict with 11.2 */
#include "file.h"



/***********************************************************************
 * Functions to support SNMP queries for file related information.
 ***********************************************************************/
/*
 * fib to flash MIB init code
 */
int flash_init_dev_database (void)
{
    return 1;
}
 
int flash_file_index (char *devnm, int fileno)
{
    stat_t stat;
 
    if (FS_istat(devnm, fileno + 1, &stat) != FS_OK)
        return -1;
    return fileno;
}
 
int flash_file_size (char *devnm, int fileno)
{
    stat_t stat;
 
    if (FS_istat(devnm, fileno + 1, &stat) != FS_OK)
        return -1;
    return stat.st_size;
}
 
ulong flash_file_chksum (char *devnm, int fileno)
{
    stat_t stat;
 
    if (FS_istat(devnm, fileno + 1, &stat) != FS_OK)
        return -1;
    return stat.crc;
}
 
/*
 * Whether file is deleted, has invalid chksum, or valid.
 */
int flash_file_status (char *devnm, int fileno)
{
    stat_t stat;
 
    if (FS_istat(devnm, fileno + 1, &stat) != FS_OK)
        return -1;
    if (!stat.valid)
        return D_ciscoFlashFileStatus_invalidChecksum;
    if (stat.deleted)
        return D_ciscoFlashFileStatus_deleted;
    return D_ciscoFlashFileStatus_valid;
}
 
/*
 * Return the name of a file given its index within the file system.
 * Return NULL if file not found or invalid index.
 */
uchar *flash_file_name (char *devnm, int fileno)
{
    stat_t stat;
    static char name[sizeof(stat.name)];
 
    if (FS_istat(devnm, fileno + 1, &stat) != FS_OK)
        return NULL;
    strcpy(name, stat.name);
    return (uchar*) name;
}

/***************************************************************************
 *                    Global File-io Routines
 **************************************************************************/
/*
 * file_open
 * Open a file by name.
 */
int file_open (const char *nm, int flags)
{
    int fd, fs_flags;
 
    fs_flags = O_CRC_OFF;
    if (flags & FILE_FLAGS_CHECKSUM)
        fs_flags &= ~O_CRC_OFF;
    if (flags & FILE_FLAGS_RDONLY)
        fs_flags |= O_RDONLY;
    if (flags & FILE_FLAGS_RW)
        fs_flags |= (O_WRONLY | O_APPEND | O_TRUNC | O_CREAT);
    if (!(flags & FILE_FLAGS_PROMPT) && !(flags & FILE_FLAGS_VERBOSE)) 
        fs_flags |= O_VERBOSE_OFF;
    fd = FS_open(nm, fs_flags, 0);
    if (fd > 0)
        return fd;
    switch (fd % FS_ERROR_CODE_GRANULARITY) {
      case FSLIB_ERROR_INVALID_DEVICE_ID:       return FOPEN_INV_DEV;
      case FSLIB_ERROR_UNKNOWN_DEVICE:
      case FSLIB_ERROR_NO_DEVICE:               return FOPEN_DEVERR;
      case FSLIB_ERROR_FILE_NOT_FOUND:          return FOPEN_NOFILE;
      case FSLIB_ERROR_FILE_SYSTEM_FULL:        return FOPEN_FSFULL;
    }
    return FOPEN_FSERR;
}

/*
 * file_dev_open
 * Open a file with an internal device name.
 */
int file_dev_open (const char *dev, const char *nm, int flags)
{
    char buf[FSLIB_DEV_ID_MAX + sizeof(":") + FSLIB_FILE_NAME_MAX];
 
    sprintf(buf, "%s:%s", dev, nm);
    return file_open(buf, flags);
}

/*
 * Open a file by its index within the file system.
 */
int file_iopen (const char *devnm, int fileno, int flags)
{
    stat_t stat;
 
    if (FS_istat(devnm, fileno + 1, &stat) != FS_OK)
        return file_dev_open(devnm, stat.name, flags);
    return FOPEN_NOFILE;
}
/*
 * Reopen a file already opened earlier. We use this when writing
 * a file. The reopen goes and completes any file system specific
 * function that was pending, eg., file system erase.
 * We need to do this because we need to squeeze in some validations
 * between opening a file and actually creating the directory entry
 * for it. The initial open gets all the user options (erase, invalidate
 * duplicate...). The reopen does the actual erase and directory
 * entry creation.
 */
int file_reopen (int fh, int flags)
{
    return fh;
}
/*
 * Generic file read based on file handle, buffer, and length.
 */
ulong file_read (int fh, char *buf, ulong len)
{
    int bytes;
 
    bytes = FS_read(fh, buf, len);
    if (bytes > 0)
        return bytes;
    return 0;
}
 
/*
 * Generic file write based on file handle, buffer, and length.
 */
ulong file_write (int fh, char *buf, ulong len)
{
    int bytes;
 
    bytes = FS_write(fh, buf, len);
    if (bytes > 0)
        return bytes;
    return 0;
}
 
/*
 * Seek to a position within the file. Validate position and
 * ignore it if invalid. Return current position within file.
 * The offset may be relative to the start or end of the file,
 * or the current position in the file.
 */
ulong file_seek (int fh, int pos, int offset)
{
    int whence, ret;
 
    switch (pos) {
      case FILE_START:  whence = SEEK_SET;      break;
      case FILE_CURR:   whence = SEEK_CUR;      break;
      case FILE_END:    whence = SEEK_END;      break;
      default:          whence = -1;            break;
    }
    ret = FS_lseek(fh, offset, whence);
    if (ret > 0)
        return ret;
    return 0;
}
 
/*
 * Generic file close based on file handle.
 */
void file_close (int fh)
{
    FS_close(fh);
    return;
}
 
ulong file_flags (int fh)
{
    return 0;
}
 
/*
 * Provide some generic info for a specified file.
 */
int file_stat (int fh, fstat_t *fs)
{
    int ret;
    stat_t stat;
    static char name[FSLIB_FILE_NAME_MAX + 1];
 
    bzero(fs, sizeof(*fs));
    ret = FS_fstat(fh, &stat);
    if (ret < 0)
        return -1;
    fs->indx = stat.st_ino;
    strcpy(name, stat.name);
    fs->name = name;
    fs->len = stat.st_size;
    fs->cksf = stat.valid;
    fs->delf = stat.deleted;
    return 1;
}
 
/*
 * File system generic info.
 */
int fsstat (const char *devnm, fsstat_t *fst)
{
    file_sys_stat_t status;
 
    if (FS_fsstat(devnm, &status) == FSLIB_OK) {
        fst->size = status.bytes_available + status.bytes_used;
        fst->free = status.bytes_available;
        if (status.writable)
            fst->status = DEV_STATE_RW;
        else
            fst->status = DEV_STATE_RDONLY;
        fst->copy_mode = D_ciscoFlashPartitionUpgradeMethod_direct;
        fst->delsz = status.files_deleted_bytes;
        fst->delcnt = status.files_deleted;
        fst->fcnt = status.files_ok;
        fst->totcnt = (status.files_ok +
                       status.files_deleted +
                       status.files_with_errors);
        fst->ersf = FALSE;
        fst->filenm_length = FSLIB_FILE_NAME_MAX;
        fst->chksum_algo = D_ciscoFlashPartitionChecksumAlgorithm_simpleCRC;
        return 1;  /* some positive number to indicate success */
    }
    return DEVERR_INIT;
}

long flash_partition_status (char *devnm)
{
    file_sys_stat_t status;
 
    if (FS_fsstat(devnm, &status) == FSLIB_OK) {
        if (status.writable)
            return D_ciscoFlashPartitionStatus_readWrite;
    }
    return D_ciscoFlashPartitionStatus_readOnly;
}
 
boolean
flash_file_delete (flash_copy_parms_t *fparms)
{
    char buf[FSLIB_DEV_ID_MAX + sizeof(":") + FSLIB_FILE_NAME_MAX];
 
    sprintf(buf, "%s:%s", fparms->devnm, fparms->sfname);
    if (FS_delete(buf) == FSLIB_OK)
        return TRUE;
    fparms->snmp_sts = SNMP_FILE_DELETE_FAIL;
    return FALSE;
}

/*
 * flash_erase_command()
 *	On the new high-end flash file system, the erase operation is
 * not yet supported.
 */
boolean
flash_erase_command (flash_copy_parms_t *fparms)
{
    fparms->snmp_sts = SNMP_INVALID_OP;
    return FALSE;
}
 
boolean
flash_file_undelete (flash_copy_parms_t *fparms)
{
    int ix;
 
    ix = 1;
    for (;;) {
        stat_t stat;
 
        if (FS_istat(fparms->devnm, ix, &stat) != FS_OK) {
            fparms->snmp_sts = SNMP_FILE_OPEN_ERR;
            return FALSE;
        }
        if (strcmp(fparms->sfname, stat.name) == 0)
            break;
        ++ix;
    }
 
    if (FS_undelete(fparms->devnm, ix, 0) == FSLIB_OK)
        return TRUE;
    fparms->snmp_sts = SNMP_FILE_UNDELETE_FAIL;
    return FALSE;
}
 
boolean
flash_device_squeeze (flash_copy_parms_t *fparms)
{
    if (FS_squeeze(fparms->devnm, SQUEEZE_QUIET) == FSLIB_OK)
        return TRUE;
    fparms->snmp_sts = SNMP_DEVICE_SQUEEZE_FAIL;
    return FALSE;
}

/*
 * flash_device_format()
 *	This routine is used to provide no-frills support for formatting
 * devices via SNMP. Support for specifying spare sectors, volume name
 * or an alternate monlib are not supported by the MIB yet.
 * NOTE: In particular, while formatting via SNMP no device protection
 *       check is made.
 */
boolean
flash_device_format (flash_copy_parms_t *fparms)
{
    format_parm_t fm_rec;
    format_parm_t *fmp = &fm_rec;
    char *dev_id;
    int error;

    dev_id = fparms->devnm;

    /* 
     * Assume defaults for those formatting parameters, support for which
     * is not present in the flash MIB yet.
     */
    fmp->spare = 0;
    fmp->volume_id[0] = '\0';
    fmp->monlib = NULL;
    fmp->monlib_bndl = NULL;
    fmp->monlib_bndl_size = 0;

    /*
     * Instruct the format routine to work silently and then activate
     * the device format operation.
     */
    FS_ioctl(dev_id, IOCTL_SET_VERBOSE, (void *)FALSE);
    error = FS_ioctl(dev_id, IOCTL_FORMAT_FLASH, (void *)fmp);
    if (error < 0) {
        fparms->snmp_sts = SNMP_DEVICE_FORMAT_FAIL;
        return (FALSE);
    }

    return (TRUE);
}

/*
 * partition_flash_snmp()
 *      Partitioning of flash file systems is not supported on
 * high-end platforms yet.
 */
boolean partition_flash_snmp (ciscoFlashPartitioningEntry_t *entry)
{
    entry->ciscoFlashPartitioningStatus =
         D_ciscoFlashPartitioningStatus_partitioningInvalidOperation;
    return FALSE;
}


/*
 * Several routines still don't speak the filesys code and assume we are
 * in a dev_io environment w/ repsect to flash devices.  Code below removes
 * the few remaining dependancies on dev/dev_io.c in an attempt to move away
 * from this.
 */

/*
 * ###############################
 * Remove usage of dev_chip_stat()
 * ###############################
 */

/* Number of chips in device */
int flash_device_chipcnt (char *devnm)
{
    return FLASH_MIN_CHIP_CNT;
}

/***********************************************************************
 * Functions to support SNMP queries for chip related information.
 ***********************************************************************/
int flash_chip_code (char *devnm, int chipno)
{
    return 0;
}

char *flash_chip_desc (char *devnm, int chipnum)
{
    return NULL;
}

ulong flash_chip_wretries (char *devnm, int chipnum)
{
    return 0;
}

ulong flash_chip_eretries (char *devnm, int chipnum)
{
    return 0;
}

int flash_chip_max_wretries (char *devnm)
{
    return 0;
}

int flash_chip_max_eretries (char *devnm)
{
    return 0;
}

/***********************************************************************
 * Functions to support SNMP queries for partition related information.
 ***********************************************************************/
void flash_partition_chip_bounds (char *devnm, long *startchip,
                                               long *endchip)
{
    *startchip = 1;
    *endchip = 1;
}

/*
 * ##########################
 * Remove usage of dev_stat()
 * ##########################
 */

/*
 **************  Device level services **********************
 */
/* Is the flash device in use or free ?
 */
boolean flash_available (char *devnm)
{
    int err;
    file_sys_stat_t fss;

    err = FS_fsstat(devnm, &fss);
    return ((err != FSLIB_OK) ? FALSE : TRUE);
}

/*
 * Return TRUE if flash programmable, FALSE otherwise
 */
boolean flash_status (char *devnm)
{
    int err;
    file_sys_stat_t fss;

    err = FS_fsstat(devnm, &fss);
    if (err != FSLIB_OK)
	return FALSE;
    return ((fss.writable) ? TRUE : FALSE);
}

uchar *flash_card_name (char *devnm)
{
    return flash_dev_desc(devnm);
}

uchar *flash_ctlr_name (char *devnm)
{
    return flash_dev_desc(devnm);
}

/*
 **************  Flash file level services **********************
 */
boolean flash_make_devnm (int dev_unit_num, int part_num, char *devnm)
{
    if ((!devnm) || (dev_unit_num > MAX_DEV_INIT_ENTRIES))
	return (FALSE);
    if (!phy_dev_database[dev_unit_num])
	return (FALSE);
 
    if (part_num == 0 || part_num == 1) {
	strcpy(devnm, phy_dev_database[dev_unit_num]);
	return (TRUE);
    }
    return (FALSE);	/* filesys flash devices only have 1 partition */
}

/***********************************************************************
 * Functions to support SNMP queries for device related information.
 ***********************************************************************/
ulong flash_dev_size (char *devnm)
{
    int err;
    file_sys_stat_t fss;

    err = FS_fsstat(devnm, &fss);
    return ((err != FSLIB_OK) ? 0 : fss.dib.length);
}

ulong flash_dev_min_partn_size (char *devnm)
{
    int err;
    file_sys_stat_t fss;

    err = FS_fsstat(devnm, &fss);
    return ((err != FSLIB_OK) ? 0 : fss.dib.length);
}

int flash_dev_max_partns (char *devnm)
{
    return 1;
}

/* Number of partitions in devnm */
int flash_dev_num_partns (char *devnm)
{
    return (flash_dev_removed(devnm) ? 0 : 1);
}

char *flash_dev_desc (char *devnm)
{
    /* FIXME - these should come from the devices (maybe via FS_ioctl()) */
    if (!strcmp(devnm, "slot0")) {
	return "PCMCIA Slot 0 Flash";
    } else if (!strcmp(devnm, "slot1")) {
	return "PCMCIA Slot 1 Flash";
    } else if (!strcmp(devnm, "bootflash")) {
	return "Boot Flash";
    } else if (!strcmp(devnm, "slaveslot0")) {
	return "Slave PCMCIA Slot 0 Flash";
    } else if (!strcmp(devnm, "slaveslot1")) {
	return "Slave PCMCIA Slot 1 Flash";
    } else if (!strcmp(devnm, "slavebootflash")) {
	return "Slave Boot Flash";
    }
    return NULL;
}

char *flash_dev_cntlr (char *devnm)
{
    int err, fs_dev_type;

    err = FS_ioctl(devnm, IOCTL_GET_DEV_TYPE, &fs_dev_type);
    if (err == FSLIB_OK){
	if (fs_dev_type & IOCTL_DEV_TYPE_REMOVABLE)
	    return(platform_get_string(PLATFORM_STRING_PCMCIA_CONTROLLER));
    } 
    return NULL;

}

int flash_dev_vpp (char *devnm)
{
    return VPP_UNKNOWN;
}

sys_timestamp flash_dev_init_t (char *devnm)
{
    /* FIXME */
    return timer_no_time;
}

boolean flash_dev_removable (char *devnm)
{
    int err, fs_dev_type;

    err = FS_ioctl(devnm, IOCTL_GET_DEV_TYPE, &fs_dev_type);
    return ((err != FSLIB_OK) ? FALSE :
	    ((fs_dev_type & IOCTL_DEV_TYPE_REMOVABLE) ? TRUE : FALSE));
}

boolean flash_dev_removed (char *devnm)
{
    int err;
    file_sys_stat_t fss;

    err = FS_fsstat(devnm, &fss);
    return ((err != FSLIB_OK) ? TRUE : FALSE);
}

/* Check to see if device is RW */
boolean flash_device_is_rw (char *devnm, int *status)
{
    int err;
    file_sys_stat_t fss;

    err = FS_fsstat(devnm, &fss);
    if (err != FSLIB_OK) {
	*status = SNMP_DEVICE_OPEN_ERR;
	return FALSE;
    }
    if (fss.writable)
	return TRUE;

    *status = SNMP_DEVICE_READONLY;
    return FALSE;
}

/***********************************************************************
 * Functions to support SNMP queries for partition related information.
 ***********************************************************************/
long flash_partition_number (char *devnm)
{
    int err;
    file_sys_stat_t fss;

    err = FS_fsstat(devnm, &fss);
    return ((err != FSLIB_OK) ? 0 : 1);
}

long flash_partition_size (char *devnm)
{
    int err;
    file_sys_stat_t fss;

    err = FS_fsstat(devnm, &fss);
    return ((err != FSLIB_OK) ? 0 : fss.dib.length);
}

char *flash_partition_name (char *devnm)
{
    int err;
    file_sys_stat_t fss;

    err = FS_fsstat(devnm, &fss);
    return ((err != FSLIB_OK) ? NULL : devnm);
}

/*
 * #########################################
 * Functions from dev/dev_io.c we still need
 * #########################################
 */

/*
 * return number of flash partitions
 */
int dev_get_num_flash_part (void)
{
    return 1;		/* filesys flash devices only have one partition */
}

/*
 * Build table of physical device names for SNMP agent
 */
int dev_get_flash_devices (char **devnm_database)
{
    int i, devs, dev_name_size, err;
    char dev_name_buffer[FS_DEV_NAMES_MAX + 1];
    char *dev_name, *tmp_ptr;
    uint dev_type;

    for (i=0; i<MAX_DEV_INIT_ENTRIES; i++)
	if (devnm_database[i]) {
	    free(devnm_database[i]);
	    devnm_database[i] = NULL;
	}

    if (FS_getdevicenames(dev_name_buffer, FS_DEV_NAMES_MAX) != FS_OK) {
	/* flag an error? */
	return 0;
    }

    devs = 1;		/* start at 1, not zero */
    dev_name_size = strlen(dev_name_buffer);
    for (i=0, dev_name = dev_name_buffer; i<=dev_name_size; i++) {
	if (!dev_name_buffer[i] || dev_name_buffer[i] == ' ') {
	    dev_name_buffer[i] = 0;
	    err = FS_ioctl(dev_name, IOCTL_GET_DEV_TYPE, &dev_type);
	    if ((err == FSLIB_OK) && (dev_type & IOCTL_DEV_TYPE_FLASH)) {
		tmp_ptr = malloc(FS_DEV_ID_MAX+1);
		if (tmp_ptr) {
		    strncpy(tmp_ptr, dev_name, FS_DEV_ID_MAX);
		    tmp_ptr[FS_DEV_ID_MAX] = 0;
		}
		devnm_database[devs++] = tmp_ptr;
	    }
	    dev_name = dev_name_buffer + i + 1;
	}
    }
    return devs;
}

/*
 * Do initial checks to prepare ourselves for the flash download.
 * See whether we can write to Flash. If not, see if we are
 * running from flash, and if so, whether we can use the FLH.
 * Display any warning messages if necessary and basically flag
 * red or green. If green, display the flash directory.
 */
boolean flash_copy_setup (flash_copy_parms_t *fparms, fsstat_t *fs_info)
{
    int flh_ver;

    /*
     * Open device and get file system info. fsstat() returns the
     * status.
     */
    if ((fparms->fh = fsstat(fparms->devnm, fs_info)) < 0) { /* Failure */
	fparms->snmp_sts = SNMP_DEVICE_OPEN_ERR;
	return (FALSE);
    }
    /*
     * See if we can do the copy in the first place. We may be running
     * from flash and the boot ROMs may not be supporting the FLH.
     */
    if (fs_info->status != DEV_STATE_RW) {

	fparms->snmp_sts = SNMP_DEVICE_READONLY;

        /*
         * We are not in a RW state. Are we running out of Flash ?
         * If not, bail out because it means we have not been able
         * to query and recognize the chips.
         * If running out of Flash, see if we can use the FLH.
         */
        if (fs_info->status != DEV_STATE_RFF) {
            if (fparms->verbose)
                printf("\nERR: Device in READ-ONLY state");
	    fparms->snmp_sts = SNMP_DEVICE_ERR;
            return (FALSE);
        }

        flh_ver = reg_invoke_flh_version();
        if (flh_ver < FLASH_BASIC_FLH_SUPPORT) {     /* FLH NOT supported */
            if (fparms->verbose)
                run_from_flash_msg();
            return (FALSE);
        }
	else if (flh_ver < FLASH_FLH_SUPPORT_WITH_RCP) { /* Pre-RCP version */
	    if (fparms->copy_option == FILE_ACCESS_RCP) {
	        if (fparms->verbose)
		    /* FLH using rcp not supported */
		    printf("\n%%Installed boot ROMs do not support "
			   "RCP for Flash Load Helper");
		return(FALSE);
	    }
	}
        /*
         * Is it ok to use FLH ? 
         * Check where the user is issuing this command from and
         * the state of the boot bits.
         * Get a confirmation from the user.
         */
        if (!reg_invoke_flh_ok_check(fparms->prompt)) 
            return (FALSE);
    }

    return (TRUE);
}

/*
 * Do initial setup for uploading a file out of flash.
 * get file system info and see if we have any valid files in
 * the first place. Display flash directory if ok.
 */
boolean flash_read_setup (flash_copy_parms_t *fparms)
{
    fsstat_t fs_info;

    /*
     * Open device and get file system info. fsstat() returns the
     * status.
     */
    fparms->fh = fsstat(fparms->devnm, &fs_info);
    if (fparms->fh < 0) {  /* Failure */
	fparms->snmp_sts = SNMP_DEVICE_OPEN_ERR;
        return (FALSE);
    }
    /*
     * Now see if there are any good files in the file system.
     * If not, bye bye.
     */
    if (fs_info.fcnt == 0) { /* # of good files */
        if (fparms->verbose)
            printf("\nNo files on device");
	fparms->snmp_sts = SNMP_INVALID_SRC_FILE;
        return (FALSE);
    }

    return (TRUE);
  }
      
