/* $Id: flash_services_les.c,v 3.3.28.3 1996/05/21 06:26:40 mordock Exp $
 * $Source: /release/112/cvs/Xsys/dev/flash_services_les.c,v $
 * ------------------------------------------------------------------
 * LES-specific routines transplanted from flash_services.c
 *
 * October 1995, Carl Schaefer
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * ------------------------------------------------------------------
 * $Log: flash_services_les.c,v $
 * Revision 3.3.28.3  1996/05/21  06:26:40  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.3.28.2  1996/05/07  00:38:10  ssangiah
 * CSCdi56357:  Misleading erase command behavior in the RSP file system.
 *   o Remove erase command from the RSP file system parse chain.
 *   o Clean up the RSP file system parse chain.
 *   o Fix problem with return of error codes by the flash MIB agent.
 * Branch: California_branch
 *
 * Revision 3.3.28.1  1996/04/22  18:57:47  ssangiah
 * CSCdi54920:  Need to add support for the flash MIB on c7200.
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/18  21:52:40  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  09:01:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:20:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:54:22  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/18  01:22:36  schaefer
 * placeholders for 7500 flash MIB
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "../os/boot.h"
#include <stdlib.h>             /* for atoi() */
#include "../dev/flash_defines.h"
#include "../dev/dev_io.h"
#include "../dev/file_io.h"
#include "../dev/sr_flashmib.h"
#include "../dev/flash_services.h"
#include "../snmp/chassismib_registry.h"
#include "ttysrv.h"
#include "file.h"
#include <ciscolib.h>

#define FLASH_BOOT_NO_BOOT_FILE "Flash boot: No bootable files found in flash.\n"
#define FLASH_BOOT_OPEN_FAILED "Flash boot: File '%s' open failed.\n"
#define FLASH_BOOT_INFO_FAILED "Flash boot: %s file system info call failed.\n"
#define FLASH_BOOT_NO_FILES "Flash boot: Flash device %s empty.\n"
#define FLASH_BOOT_INSUFF_DRAM "Flash boot: DRAM insufficient.\n            File length %d exceeds boot buffer %d.\n"
static const char flash_partition_num_err_msg[] =
    "\nERROR: Invalid partition number, operation aborted.";

/*
 * init table of device names
 */
int flash_init_dev_database (void)
{
    return dev_get_flash_devices(phy_dev_database);
}

/*
 * This function will convert the device state value to a corresponding
 * state value as specified by the flash MIB. If the enumerated state
 * values in the MIB change, or if the possible device status values
 * change, then this function may need to be changed accordingly to 
 * maintain the correct mapping.
 */
static long
flash_map_dev_status (long status)
{
    switch (status) {
    case DEV_STATE_RDONLY:  /* device state read-only */
        status = SNMP_DEV_STATE_RDONLY; /* snmp equivalent state */
        break;
    case DEV_STATE_RFF:  /* device state run-from-flash */
        status = SNMP_DEV_STATE_RFF;  /* snmp equivalent state */
        break;
    case DEV_STATE_RW:  /* device state read-write */
        status = SNMP_DEV_STATE_RW; /* snmp equivalent state */
        break;
    default:
        status = SNMP_DEV_STATE_RDONLY; /* default state is read-only */
        break;
    }
    return(status);
}

long flash_partition_status (char *devnm)
{
    devstat_t devinfo;
    long status;
    if (dev_stat(devnm, &devinfo) != DEV_OK)
        return(0);
    /*
     * The device state will have to be mapped onto a corresponding
     * state as specified in the flash MIB. 
     */
    status = flash_map_dev_status(devinfo.dev_state);
    return(status);
}

/***********************************************************************
 * Functions to support SNMP queries for file related information.
 ***********************************************************************/
int flash_file_index (char *devnm, int fileno)
{
    int fh, sts;
    fstat_t fs;

    fh = file_iopen(devnm, fileno, FILE_FLAGS_RDONLY | FILE_FLAGS_NO_DEL_CHK);
    if (fh < 0) 
        return (0);

    sts = file_stat(fh, &fs);
    file_close(fh);

    return((sts < 0) ? 0 : fs.indx);
}

int flash_file_size (char *devnm, int fileno)
{
    int fh, sts;
    fstat_t fs;

    fh = file_iopen(devnm, fileno, FILE_FLAGS_RDONLY | FILE_FLAGS_NO_DEL_CHK);
    if (fh < 0) 
        return (0);

    sts = file_stat(fh, &fs);
    file_close(fh);

    return((sts < 0) ? 0 : fs.len);
}

ulong flash_file_chksum (char *devnm, int fileno)
{
    int sts;
    fsstat_t fs;

    sts = fsstat(devnm, &fs);
    return (((sts < 0) || ((flash_file_size(devnm, fileno)) == 0)) ? 0 : 
	    fs.chksum[fileno]);
}

/*
 * Whether file is deleted, has invalid chksum, or valid.
 */
int flash_file_status (char *devnm, int fileno)
{
    int fh, sts;
    fstat_t fs;

    fh = file_iopen(devnm, fileno, FILE_FLAGS_RDONLY | FILE_FLAGS_NO_DEL_CHK);
    if (fh < 0)
        return (0);

    sts = file_stat(fh, &fs);
    file_close(fh);

    if (fs.delf)
	return (FLASH_FILE_DELETED);
    else if (!fs.cksf)
	return (FLASH_FILE_INVALID_CHKSUM);
    else 
	return (FLASH_FILE_VALID);
}

/*
 * Return the name of a file given its index within the file system.
 * Return NULL if file not found or invalid index.
 */
uchar *flash_file_name (char *devnm, int fileno)
{
    int fh, sts;
    fstat_t fs;

    fh = file_iopen(devnm, fileno, FILE_FLAGS_RDONLY | FILE_FLAGS_NO_DEL_CHK);
    if (fh < 0)
        return (NULL);

    sts = file_stat(fh, &fs);
    file_close(fh);

    return(((sts < 0) || (fs.len == 0)) ? NULL : (uchar *)fs.name);
}

/*
 * Erase flash and initialize the file system
 */
boolean flash_erase_command (flash_copy_parms_t  *fparms)
{
    boolean erase = FALSE;
    boolean confirm_ok;
    char partition_num[FLASH_PHY_DEV_NAME_LEN];
    int first_partition;
    int fh;
    int partition = 0;
    devstat_t devinfo;
 
    if (fparms->prompt) {
        /* Get device information */
        if (dev_stat(fparms->devnm, &devinfo) != DEV_OK) {
            fparms->snmp_sts = SNMP_DEVICE_OPEN_ERR;
            return (FALSE);
        }
 
        if (devinfo.dev_flags & DEV_FLAGS_NOERASE) {
            fparms->snmp_sts = SNMP_INVALID_OP;
            return FALSE;
        }
 
        /* Display flash partition summary.  */
        if (fparms->verbose && (devinfo.dev_part_cnt > 1)) {
            if(!flash_directory(fparms->devnm, SHOW_FILES_SUMMARY))
                return (FALSE);
        }
 
        /* Ask which partition to erase from only if there are 2 or more r/w
         * partitions.
         */
        if (devinfo.dev_rwcnt >= 2) {
            /* get the partition number from user */
            first_partition = devinfo.dev_first_rw;
            if (!flash_get_partition_number(fparms->devnm, first_partition,
                                            partition_num))
                return (FALSE);
 
            partition = atoi(partition_num);
            /* Partition number is specified.  If number is valid,
             * set device name to the format of <dev>:<partition>.
             */
            if ((partition <= 0) || (partition > devinfo.dev_part_cnt)) {
                printf("\nERROR: Invalid partition number, operation aborted.");
                return (FALSE);
            }
            sprintf(fparms->devnm, "%s%c%s",
                    fparms->devnm, FLASH_PATH_SEPARATOR, partition_num);
        }
        /* if there is 1 r/w partition and 1 r/o partition, default to the r/w
         * partition
         * Set device name to the format of <dev>:<partition>.
         */
        else if ((devinfo.dev_rwcnt == 1) && (devinfo.dev_rocnt == 1)) {
            partition = devinfo.dev_first_rw;
            sprintf(fparms->devnm, "%s:%d", fparms->devnm, partition);
        }
    }
 
    if (fparms->verbose) {  /* Display Device directory */
        if(!flash_directory(fparms->devnm, SHOW_FILES_BRIEF))
            return (FALSE);
    }
 
    if (fparms->prompt) {
        if (devinfo.dev_part_cnt == 1)
            confirm_ok = confirm("\n\nErase flash device? ");
        else {
            sprintf(fparms->buff, "%s%d? ","\n\nErase flash device, partition ",
                    partition);
            confirm_ok = confirm(fparms->buff);
        }
        if (confirm_ok) {
            if (yes_or_no("Are you sure", FALSE, FALSE))
                erase = TRUE;
            else
                printf("%sErase aborted", "");
        }
        else
            printf("%sErase aborted", "\n");
        if (!erase)
            return (FALSE);
    }
 
    if ((fparms->fh = dev_open(fparms->devnm, DEV_FLAGS_RW | fparms->verbose))
        < 0) {
        fparms->snmp_sts = SNMP_DEVICE_READONLY;
        fh = dev_open(fparms->devnm, DEV_FLAGS_RDONLY | fparms->verbose);
        if (fh < 0)
            fparms->snmp_sts = SNMP_DEVICE_OPEN_ERR;
        else
            dev_close(fh);
        return (FALSE);
    }
 
    dev_init(fparms->fh);  /* Clean up our puppy. */
    dev_close(fparms->fh);
    return (TRUE);
}

/*****************************************************************************/

boolean
flash_file_delete (flash_copy_parms_t *fparms)
{
    fparms->snmp_sts = SNMP_INVALID_OP;
    return FALSE;
}

boolean
flash_file_undelete (flash_copy_parms_t *fparms)
{
    fparms->snmp_sts = SNMP_INVALID_OP;
    return FALSE;
}

boolean
flash_device_squeeze (flash_copy_parms_t *fparms)
{
    fparms->snmp_sts = SNMP_INVALID_OP;
    return FALSE;
}

boolean
flash_device_format (flash_copy_parms_t *fparms)
{
    fparms->snmp_sts = SNMP_INVALID_OP;
    return FALSE;
}

int
flash_device_slot (char* devnm)
{
    return NO_SLOTNUMBER;
}


/* Get a partition number, ?<no> for partition directory, ? for full directory 
 * display.  Return partition_num.
 */
#define MAXBUF 20

static boolean flash_parse_partition_number (int *flag, char *partition_num,
					     int default_partition)
{
    char *c, buffer[MAXBUF];

    rdtty(buffer, (MAXBUF-1));
    c = deblank(buffer);
    if (strlen(c) <= 0) {  /* accept the default */
	sprintf(partition_num, "%d", default_partition);
	*flag = FLASH_GOT_PARTITION_NUM;
	return (TRUE);
    }
    if (t_number(c)) {   /* we have a partition number */
	*flag = FLASH_GOT_PARTITION_NUM;
	strcpy(partition_num, c);
	return (TRUE);
    }
    else if (*c == '?') {		/* directory display */
	if (strlen(c) == 1) {	/* we have "?" for full directory display */
	    *flag = FLASH_DIR_DISPLAY;
	    strcpy(partition_num, "0");
	    return (TRUE);
	}
	else {			/* we have ?<no> for partition directory */
	    c++;		/* skip '?' */
	    if (t_number(c)) {
		*flag = FLASH_DIR_DISPLAY;
		strcpy(partition_num, c);
		return (TRUE);
	    }
	}
    }
    else if ((*c == 'q') || (*c == 'Q')){
	printf("\nOperation aborted on user request.\n");
	return (FALSE);
    }
    *flag = FLASH_LOOP_FOR_ANSWER;
    return (TRUE);
}

boolean flash_get_partition_number (char *devnm, int first_partition, 
				    char *partition_num)
{
    int flag;
    char dev_name[FLASH_PHY_DEV_NAME_LEN];
    int partition;
    devstat_t devinfo;

    if (dev_stat(devnm, &devinfo) != DEV_OK)
	return (FALSE);

    flag = FLASH_LOOP_FOR_ANSWER;
    printf("\n\n[Type ?<no> for partition directory; ? for full directory; "
	    "q to abort]");

    while (flag != FLASH_GOT_PARTITION_NUM) {
	printf("\nWhich partition? [default = %d] ", first_partition);
	if (!flash_parse_partition_number(&flag, partition_num, first_partition))
	    return (FALSE);
	
	partition = atoi(partition_num);
	if (flag == FLASH_DIR_DISPLAY) {
	    if (partition == 0) {	
		/* full directory display */ 
		if (!flash_directory(devnm, SHOW_FILES_BRIEF)) 
		    return (FALSE);
	    }
	    else if ((partition > 0) && (partition <= devinfo.dev_part_cnt)) {
		/* Change the device name to for format of <dev>:<partition>
		 * Display the directory of the given partition.
		 */
		sprintf(dev_name, "%s%c%s",
			devnm, FLASH_PATH_SEPARATOR, partition_num);
		if (!flash_directory(dev_name, SHOW_FILES_BRIEF))
		    return (FALSE);
	    }
	    else
		printf("\nInvalid partition number, please try again.");
	}
	else if (flag == FLASH_LOOP_FOR_ANSWER)
	    printf("\nInvalid partition number, please try again.");
    }
    return (TRUE);
}

/*
 * Display list of files, total size, free space left,
 * and access mode.
 */
boolean flash_directory (const char *devnm, int flg)
{
    int sts;

    /*
     * dev_info() will look at flg and display whatever's necessary.
     * It will call a file system specific function to display the
     * device directory, and a driver specific function to display
     * any chip specific info.
     */
    sts = dev_info(devnm, flg);
    if (sts < 0)
        printf("\nERR: %s", dev_error_msg(sts));
    return ((sts < 0) ? FALSE : TRUE);
}

/*
 * See if the desired file is in flash memory.
 * If it is, use TFTP to send it across the network.
 * Returns TRUE if file transfer completed successfully.
 */

boolean flash_tftp_server (flash_copy_parms_t *fparms, void *soc)
{

    if (strlen(fparms->devnm) == 0)  /* no device */
        return (FALSE);

    if (strlen(fparms->sfname) == 0)  /* no file name */
        return (FALSE);

    fparms->verbose = 0;  /* we need to work silently */
    fparms->prompt = 0;   /* nothing to prompt for */
    fparms->erase = 0;    /* we are only reading flash */
    if (!flash_locate_and_open_file(fparms))
        return (FALSE);

    invoke_tftp_from_flash(fparms, soc);

    file_close(fparms->fh);
    return(TRUE);
}

boolean flash_file_boot_in_place (flash_copy_parms_t *fparms, 
                                  uchar **bufpp, ulong *sizep)
{
    uchar *phy_addr; 
    const char *filenm;
    char *strptr;
    char devnm[FLASH_LOG_DEV_NAME_LEN];
    char path[FLASH_PATH_MALLOC_LEN];
    int partnum;
    fstat_t fsts;
    fsstat_t fs_info;
    uchar *fnbuff;
    int fcount = 0;

    if (strlen(fparms->devnm) == 0) { /* no device */
        *sizep = 0;
        return (FALSE);
    }

    /*
     * See if filename specified. Use file open call based
     * on this.
     */
    if (strlen(fparms->sfname) == 0) { /* No file specified */
        filenm = flash_first_bootable_file(fparms); /* Search for one */
        if (filenm) /* A bootable file was found */
            strcpy(fparms->sfname, filenm);
    }
    if (strlen(fparms->sfname)) { /* If file found */
        flash_locate_and_open_file(fparms);
    }
    else { /* No bootable files found */
        printf(FLASH_BOOT_NO_BOOT_FILE);
        flush();
        *sizep = 0;
        return(FALSE);
    }

    if (fparms->fh < 0) {  /* File not found */
        printf(FLASH_BOOT_OPEN_FAILED, fparms->sfname);
        flush();
        *sizep = 0;
        return(FALSE);
    }
    /*
     * Get file system info.
     */
    if (fsstat(fparms->devnm, &fs_info) < 0) { /* Failure */
        printf(FLASH_BOOT_INFO_FAILED, fparms->devnm);
        flush();
        *sizep = 0;
        return (FALSE);
    }

    file_stat(fparms->fh, &fsts);  /* read in file info */
    fnbuff = malloc(TFTP_MAXDATA * 2); /* need to check for malloc failure */
				       /* below */
    phy_addr = (uchar *)(fs_info.phy_start + fsts.data_start);
    fcount = fsts.len;
    if (reg_invoke_flash_needs_buffer()) {
	if (!fnbuff) {		/* we don't have buffer, punt */
	    file_close(fparms->fh);
	    return (FALSE);
	}
	if (!file_read(fparms->fh, fnbuff,TFTP_MAXDATA * 2)) {
	    if (fnbuff)
		free(fnbuff);
	    file_close(fparms->fh);
	    return (FALSE);
	}
	phy_addr = fnbuff;
	fcount = TFTP_MAXDATA * 2;
    }
    file_close(fparms->fh);

    if (reg_invoke_flash_boot_verify(phy_addr, fcount, (void *)&fs_info)) {
	*sizep = 0;
	if (fnbuff)
	    free(fnbuff);
	return (FALSE);
    }
	

    /* Now set sysconfg.filename appropriately */
    sprintf(devnm, "%s", fparms->devnm);
    strptr = strchr(devnm, FLASH_PATH_SEPARATOR);
    if (strptr) {
        *strptr++ = 0;  /* terminate device name string */
        partnum = atoi(strptr);
    }
    else 
        partnum = 1;  /* default partition is the first partition */

    if (partnum > 1) /* add the partition number */
        sprintf(path, "%s%c%d%c%s", devnm, FLASH_PATH_SEPARATOR,
		partnum, FLASH_PATH_SEPARATOR, fsts.name);
    else   /* drop the partition number; it is the default */
        sprintf(path, "%s%c%s", devnm, FLASH_PATH_SEPARATOR, fsts.name);
    setstring(&sysconfg.filename, path);

    if (reg_invoke_RFF_exec_check(phy_addr, (ulong)phy_addr)) {
        *bufpp = phy_addr;
        *sizep = fsts.len;
	if (fnbuff)
	    free(fnbuff);
        return (TRUE);
    }
    if (fnbuff)
	free(fnbuff);
    return (FALSE);
}

/*
 * Return first valid image file on flash card, if any.  Else return NULL.
 * Checks if executable and does other sanity checks. Looks in all
 * partitions for the file.
 */
const char *flash_first_bootable_file (flash_copy_parms_t *fparms)
{
    int sts, cnt, part_no;
    char devnm[FLASH_DEV_NAME_LEN];
    fstat_t fs;
    devstat_t ds;
    fsstat_t fs_info;
    char *fnbuff;
    uchar *phy_start;
    int fcount;
    /*
     * Get device info. Based on the value in fparms->devnm, we
     * will either get consolidated device info, or partition
     * specific info. The sub_unit number in device status will
     * tell us what info this is.
     */
    fparms->dh = dev_stat(fparms->devnm, &ds);
    if (fparms->dh < 0) {
        return (NULL);
    }

    fnbuff = malloc(TFTP_MAXDATA * 2);
    for (part_no = 1; part_no <= ds.dev_part_cnt; part_no++) {
        /*
         * If the device name contained the partition number,
         * break out of this loop once that partition has been 
         * covered.
         */
        if ((ds.dev_subunit > 0) && (part_no > 1))
            break;

        if (ds.dev_subunit == 0)
            sprintf(devnm, "%s:%d", fparms->devnm, part_no);
        else
            strcpy(devnm, fparms->devnm);
        if ((fparms->fh = fsstat(devnm, &fs_info)) < 0)
            continue;
        if (fs_info.fcnt > 0) {  /* flash has some good files */
           /*
            * Open every file in Flash. If success, get its name.
            */
            for (cnt = 0; cnt < fs_info.totcnt; cnt++) {
                fparms->fh = file_iopen(devnm, cnt,
                  FILE_FLAGS_RDONLY | FILE_FLAGS_CHECKSUM | FILE_FLAGS_VERBOSE);
                if (fparms->fh < 0)  /* file open error */
                    continue;  /* skip to next one */
                sts = file_stat(fparms->fh, &fs);  /* get file name */
                if (sts < 0){  /* error in getting file stats */
		    file_close(fparms->fh);
                    continue;  /* skip file */
		}
		if(reg_invoke_flash_needs_buffer()) {
		    if (!file_read(fparms->fh, fnbuff,TFTP_MAXDATA * 2)) {
			if (fnbuff)
			    free(fnbuff);
			file_close(fparms->fh);
			continue;
		    }
		    phy_start = fnbuff;
		    fcount = TFTP_MAXDATA * 2;
		} else {
		    fs.data_start += fs_info.phy_start; /* cvrt to phys addr */
		    phy_start = (uchar *)fs.data_start;
		    fcount = fs.len;
		}
		file_close(fparms->fh);
		/*
		 * Do platform specific validations on image.
		 */
		if (reg_invoke_flash_boot_verify(phy_start, fcount,
						  (void *)&fs_info)) {
		    continue;
		}
                strcpy(fparms->devnm, devnm);
		if (fnbuff)
		    free(fnbuff);
                return(fs.name);
             }
         }
    }
    if (fnbuff)
	free(fnbuff);
    return (NULL); /* Bootable file was not found */
}

/*
 * Read a file from the flash file system into a user provided
 * buffer. If we are booting, see whether the image has to execute
 * in place.
 */
int flash_file_read (flash_copy_parms_t *fparms, uchar *buffer,
                     ulong *buffersize, boolean verbose)
{
    int sts;
    ulong rlen;
    fstat_t fsts;
    fsstat_t fs_info;
    uint copy_size;

    if (strlen(fparms->devnm) == 0) { /* no device */
        *buffersize = 0;
        return (ERROR_NOFILE);
    }

    /*
     * Get file system info.
     */
    if ((sts = fsstat(fparms->devnm, &fs_info)) < 0) { /* Failure */
        printf(FLASH_BOOT_INFO_FAILED, fparms->devnm);
        flush();
        *buffersize = 0;
        return (ERROR_NOFILE);
    }
    if (fs_info.fcnt == 0) { /* no files in flash; bye bye */
        printf(FLASH_BOOT_NO_FILES, fparms->devnm);
        flush();
        *buffersize = 0;
        return (ERROR_NOFILE);
    }

    /*
     * See if filename specified. Use file open call based
     * on this. If filename not specified, try to open first
     * file.
     */
    fparms->fh = -1;  /* assume the worst - error */
    if (strlen(fparms->sfname)) {  /* some file name present */
        flash_create_path(fparms, fparms->sfname); /* create path */
        fparms->fh = file_open(fparms->path, FILE_FLAGS_RDONLY);
    }
    if (fparms->fh < 0) {  /* File not found */
        printf("\n\nFile not found in '%s'\n", fparms->devnm);
        flush();
        *buffersize = 0;
        return (ERROR_NOFILE);
    }

    file_stat(fparms->fh, &fsts);  /* read in file info */

    if (fsts.len > *buffersize) {  /* file length > buffer */
        printf(FLASH_BOOT_INSUFF_DRAM, fsts.len, *buffersize);
        flush();
        *buffersize = 0;
        file_close(fparms->fh);
        return (ERROR_NOFILE);
    }

    /* Read file in MAX_FLASH_READ_COUNT chunks at a shot */

    copy_size = fsts.len;
    rlen = 0;
    while (copy_size) {
	uint count;

	count = file_read (fparms->fh, (char *)buffer + rlen, 
			   min(copy_size, MAX_FLASH_READ_COUNT));
	rlen += count;
	copy_size -= (copy_size > MAX_FLASH_READ_COUNT ? 
		      MAX_FLASH_READ_COUNT : count);
	process_may_suspend();
    }

    file_close(fparms->fh);
    *buffersize = rlen;
    return ((rlen == fsts.len) ? ERROR_OK : ERROR_NOFILE);
}

/*
 **************  Device level services **********************
 */
/* Is the flash device in use or free ?
 */
boolean flash_available (char *devnm)
{
    int dh;
    devstat_t ds;

    dh = dev_stat(devnm, &ds);
    return ((dh < 0) ? FALSE : TRUE);
}

/*
 * Return TRUE if flash programmable, FALSE otherwise
 */
boolean flash_status (char *devnm)
{
    int sts;
    devstat_t ds;

    sts = dev_stat(devnm, &ds);
    return((sts < 0 ) ? 0 : (ds.dev_state == DEV_STATE_RW) ? 1 : 0);
}

uchar *flash_card_name (char *devnm)
{
    int sts;
    devstat_t ds;

    sts = dev_stat(devnm, &ds);
    return((uchar *)((sts < 0 ) ? 0 : ds.dev_name));
}

uchar *flash_ctlr_name (char *devnm)
{
    int sts;
    devstat_t ds;

    sts = dev_stat(devnm, &ds);
    return((uchar *)((sts < 0 ) ? 0 : ds.dev_name));
}

boolean flash_make_devnm (int dev_unit_num, int part_num, char *devnm)
{
    devstat_t devinfo;

    if ((!devnm) || (dev_unit_num > MAX_DEV_INIT_ENTRIES))
        return (FALSE);
    if (!phy_dev_database[dev_unit_num])
        return (FALSE);

    if (dev_stat(phy_dev_database[dev_unit_num], &devinfo)!=DEV_OK)
        return (FALSE);
 
    if (part_num == 0) {
	strcpy(devnm, phy_dev_database[dev_unit_num]);
	return (TRUE);
    }

   if ((part_num<0) || (part_num>devinfo.dev_part_cnt)) 
	return (FALSE);
    else
	sprintf(devnm, "%s%c%d", phy_dev_database[dev_unit_num],
		FLASH_PATH_SEPARATOR, part_num);
    return(TRUE); 
}

/*
 * flash_file_exists
 * Look and see if a file exists in flash.
 */
boolean flash_file_exists (flash_copy_parms_t *fparms)
{
    if (!flash_locate_and_open_file(fparms))
        return (FALSE);
    file_close(fparms->fh);
    return(TRUE);
}

/*
 * Functions to support wrappers for SNMP. The functions required to
 * extract information to respond to SNMP queries for various object
 * families are listed below in the order in which the families
 * appear in the flash MIB.
 */
 
/***********************************************************************
 * Functions to support SNMP queries for device related information.
 ***********************************************************************/
ulong flash_dev_size (char *devnm)
{
    int sts;
    devstat_t devinfo;

    sts = dev_stat(devnm, &devinfo);
    return (((sts < 0) || flash_dev_removed(devnm)) ? 0 : devinfo.dev_size);
}

ulong flash_dev_min_partn_size (char *devnm)
{
    int sts;
    devstat_t devinfo;

    sts = dev_stat(devnm, &devinfo);
    return (((sts < 0) || flash_dev_removed(devnm)) ? 0 : devinfo.dev_banksize);
}

int flash_dev_max_partns (char *devnm)
{
    int sts;
    long min_part_size;
    int max_partns;
    devstat_t devinfo;

    sts = dev_stat(devnm, &devinfo);
    min_part_size = flash_dev_min_partn_size(devnm);

    /*
     * If the device has been removed, return a legal
     * value to conform with MIB specs for ciscoFlashDeviceMaxPartns
     */
    if ((sts < 0) || flash_dev_removed(devnm) || (min_part_size == 0))
        return (FLASH_MIN_PART_CNT);
    max_partns = devinfo.dev_size/min_part_size;
    return ((max_partns <= FLASH_MAX_PART_CNT) ? max_partns : FLASH_MAX_PART_CNT);
}


/* Number of partitions in devnm */
int flash_dev_num_partns (char *devnm)
{
    int sts;
    devstat_t devinfo;

    sts = dev_stat(devnm, &devinfo);
    return (((sts < 0) || flash_dev_removed(devnm)) ? 0 : devinfo.dev_part_cnt);
}

/* Number of chips in device */
int flash_device_chipcnt (char *devnm)
{
    int sts;
    dev_chipstat_t *dev_chipinfo;

    sts = dev_chip_stat(devnm, &dev_chipinfo);
    return ((sts != DEV_OK) ? FLASH_MIN_CHIP_CNT : dev_chipinfo->dev_chips);
}

char *flash_dev_desc (char *devnm)
{
    int sts;
    devstat_t devinfo;

    sts = dev_stat(devnm, &devinfo);
    return ((sts < 0) ? NULL : devinfo.dev_name);
}

char *flash_dev_cntlr (char *devnm)
{
    int sts;
    devstat_t devinfo;

    sts = dev_stat(devnm, &devinfo);
    return ((sts < 0) ? NULL : devinfo.dev_controller);
}

int flash_dev_vpp (char *devnm)
{
    int sts;
    devstat_t devinfo;

    sts = dev_stat(devnm, &devinfo);
    return ((sts < 0) ? VPP_UNKNOWN : devinfo.dev_vpp_status);
}

sys_timestamp flash_dev_init_t (char *devnm)
{
    int sts;
    devstat_t devinfo;

    sts = dev_stat(devnm, &devinfo);
    return ((sts < 0) ? timer_no_time : devinfo.dev_init_time); 
}

boolean flash_dev_removable (char *devnm)
{
    int sts;
    devstat_t devinfo;

    sts = dev_stat(devnm, &devinfo);
    return ((sts < 0) ? FALSE : devinfo.dev_removable);
}

boolean flash_dev_removed (char *devnm)
{
    int sts;
    devstat_t devinfo;

    sts = dev_stat(devnm, &devinfo);
    if (sts < 0)
	return (FALSE);

    switch (devinfo.dev_state) {
      case DEV_STATE_REMOVED:
      case DEV_STATE_NODEV:
	return TRUE;
    }
    return FALSE;
}

/* Check to see if device is RW */
boolean flash_device_is_rw (char *devnm, int *status)
{
    int sts;
    devstat_t devinfo;

    sts = dev_stat(devnm, &devinfo);
    if (sts == DEVERR_NODEV) {
	*status = SNMP_DEVICE_OPEN_ERR;
	return (FALSE);
    }

    if (devinfo.dev_state == DEV_STATE_RW)
	return (TRUE);

    *status = SNMP_DEVICE_READONLY; 
    return (FALSE);
}

/***********************************************************************
 * Functions to support SNMP queries for chip related information.
 ***********************************************************************/
int flash_chip_code (char *devnm, int chipno)
{
    dev_chipstat_t *dev_chipinfo;

    if (dev_chip_stat(devnm, &dev_chipinfo) != DEV_OK)
	return (0);

    return(dev_chipinfo->dev_rom_id[chipno]); 
}

char *flash_chip_desc (char *devnm, int chipnum)
{
    dev_chipstat_t *dev_chipinfo;

    if (dev_chip_stat(devnm, &dev_chipinfo) != DEV_OK)
	return (NULL);

    return(dev_chipinfo->dev_chip_name[chipnum]); 
}

ulong flash_chip_wretries (char *devnm, int chipnum)
{
    dev_chipstat_t *dev_chipinfo;

    if (dev_chip_stat(devnm, &dev_chipinfo) != DEV_OK)
	return (0);

    return(dev_chipinfo->dev_chip_wp[chipnum]); 
}

ulong flash_chip_eretries (char *devnm, int chipnum)
{
    dev_chipstat_t *dev_chipinfo;

    if (dev_chip_stat(devnm, &dev_chipinfo) != DEV_OK)
	return (0);

    return(dev_chipinfo->dev_chip_ep[chipnum]); 
}

int flash_chip_max_wretries (char *devnm)
{
    dev_chipstat_t *dev_chipinfo;

    if (dev_chip_stat(devnm, &dev_chipinfo) != DEV_OK)
	return (0);

    return(dev_chipinfo->dev_chip_wtries); 
}

int flash_chip_max_eretries (char *devnm)
{
    dev_chipstat_t *dev_chipinfo;

    if (dev_chip_stat(devnm, &dev_chipinfo) != DEV_OK)
	return (0);

    return(dev_chipinfo->dev_chip_etries); 
}

/***********************************************************************
 * Functions to support SNMP queries for partition related information.
 ***********************************************************************/
long flash_partition_number (char *devnm)
{
    devstat_t devinfo;
    if (dev_stat(devnm, &devinfo) != DEV_OK)
        return(0);
    return(devinfo.dev_subunit);
}

void flash_partition_chip_bounds (char *devnm, long *startchip,
                                               long *endchip)
{
    dev_chipstat_t *dev_chipinfo;
    long part_num;

    part_num = flash_partition_number(devnm);
    if ((part_num > FLASH_MAX_PART_CNT) ||
         (dev_chip_stat(devnm, &dev_chipinfo) != DEV_OK)) {
	/* In flash MIB chip numbers start from 1 rather than 0 */
        *startchip = 1;
        *endchip = 1;
        return;
    }
    *startchip = dev_chipinfo->dev_partn_chip_bounds[part_num-1] + 1;
    *endchip   = dev_chipinfo->dev_partn_chip_bounds[part_num];
    return;
}

long flash_partition_size (char *devnm)
{
    devstat_t devinfo;
    if (dev_stat(devnm, &devinfo) != DEV_OK)
        return(0);
    return(devinfo.dev_size);
}

char *flash_partition_name (char *devnm)
{
    devstat_t devinfo;
    if (dev_stat(devnm, &devinfo) != DEV_OK)
        return(0);
    return(devinfo.dev_sub_alias);
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
    char partition_num[FLASH_PHY_DEV_NAME_LEN];
    int first_partition, partition;
    devstat_t devinfo;

    /* Get device information */
    if (dev_stat(fparms->devnm, &devinfo) != DEV_OK)  {
	fparms->snmp_sts = SNMP_DEVICE_ERR;
        return (FALSE);
    }

    /* If no device there, then quit now. */
    if (devinfo.dev_state == DEV_STATE_REMOVED) {
        if (fparms->verbose)
            printf("\nError: Device in REMOVED state");
	fparms->snmp_sts = SNMP_DEVICE_ERR;
        return (FALSE);
    }
    if (fparms->prompt) {

	/* Display flash partition summary.  */
	if (fparms->verbose && (devinfo.dev_part_cnt > 1)) {  
	    if(!flash_directory(fparms->devnm, SHOW_FILES_SUMMARY))
	    return (FALSE);
	}
	/* Ask which partition to copy to only if there are 2 or more r/w 
	 *partitions
	 *			OR
	 * 1 r/o + 1 r/w partition AND DFB support in boot roms.
	 * Default partition is the first r/w partition.
	 */
	if ((devinfo.dev_rwcnt >= 2) ||
	    ((devinfo.dev_rocnt == 1) && (devinfo.dev_rwcnt == 1) && 
	     (reg_invoke_flh_version() >= FLASH_FLH_SUPPORT_WITH_DFB))) {
	
	    /* Get the partition number from user. 
	     * Default is the first r/w partition. 
	     */
	    first_partition = devinfo.dev_first_rw;
	    if (!flash_get_partition_number(fparms->devnm, first_partition, 
					    partition_num))
		return (FALSE);
	    partition = atoi(partition_num);
	    /* Partition number is specified.  If number is valid,
	     * set device name to the format of <dev>:<partition>.
	     */
	    if ((partition <= 0) || (partition > devinfo.dev_part_cnt)) {
		printf(flash_partition_num_err_msg);
		return (FALSE);
	    }
	    sprintf(fparms->devnm, "%s%c%s", fparms->devnm,
		    FLASH_PATH_SEPARATOR, partition_num);
	}
	/* If we have 1 r/o and 1 r/w partition, and no DFB support in the boot
	 * roms, default to the first r/w partition. 
	 * Set device name to the format of <dev>:<partition>.
	 */
	else if ((devinfo.dev_rocnt == 1) && (devinfo.dev_rwcnt == 1) && 
		 (reg_invoke_flh_version() <= FLASH_BASIC_FLH_SUPPORT)) 
	    sprintf(fparms->devnm, "%s:%d", fparms->devnm, devinfo.dev_first_rw);
    }

    /*
     * Open device and get file system info. fsstat() returns the
     * status.
     */
    if ((fparms->fh = fsstat(fparms->devnm, fs_info)) < 0) { /* Failure */
	if (fparms->verbose)
	    printf("\nERR: %s", dev_error_msg(fparms->fh));
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

    if (fparms->verbose) {  /* Display Device directory */
        if(!flash_directory(fparms->devnm, SHOW_FILES_BRIEF))
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
    char partition_num[FLASH_PHY_DEV_NAME_LEN];
    int partition;
    devstat_t devinfo;

    /* Get device information */
    if (dev_stat(fparms->devnm, &devinfo) != DEV_OK)  {
	fparms->snmp_sts = SNMP_DEVICE_ERR;
        return (FALSE);
    }

    /* If no device there, then quit now. */
    if (devinfo.dev_state == DEV_STATE_REMOVED) {
        if (fparms->verbose)
            printf("\nError: Device in REMOVED state");
	fparms->snmp_sts = SNMP_DEVICE_ERR;
        return (FALSE);
    }

    if (fparms->prompt) {
	/* Display flash partition summary.  */
	if (fparms->verbose && (devinfo.dev_part_cnt > 1)) {  
	    if(!flash_directory(fparms->devnm, SHOW_FILES_SUMMARY))
		return (FALSE);
	}

	/* Ask which partition to read from only if there are 2 or more 
	 * partitions.
	 */
	if (devinfo.dev_part_cnt >= 2) {
	    /* Get the partition number from user. 
	     * Default is the first partition.
	     */
	    if (!flash_get_partition_number(fparms->devnm, 1, partition_num))
		return (FALSE);

	    partition = atoi(partition_num);
	    /* Partition number is specified.  If number is valid,
	     * set device name to the format of <dev>:<partition>.
	     */
	    if ((partition <= 0) || (partition > devinfo.dev_part_cnt)) {
		printf(flash_partition_num_err_msg);
		return (FALSE);
	    }
	    sprintf(fparms->devnm, "%s%c%s",
		    fparms->devnm, FLASH_PATH_SEPARATOR, partition_num);
	}
    }
    /*
     * Open device and get file system info. fsstat() returns the
     * status.
     */
    fparms->fh = fsstat(fparms->devnm, &fs_info);
    if (fparms->fh < 0) {  /* Failure */
        if (fparms->verbose)
            printf("\nERR: %s", dev_error_msg(fparms->fh));
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

    if (fparms->verbose) {  /* Display Device directory */
        if(!flash_directory(fparms->devnm, SHOW_FILES_BRIEF))
            return (FALSE);
    }
    return (TRUE);
  }

