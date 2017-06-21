/* $Id: rp_flash.c,v 3.5.20.2 1996/05/21 10:04:06 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c7000/rp_flash.c,v $
 *------------------------------------------------------------------
 * RP specific flash file system functions
 *
 * April 1995, Szewei Ju
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rp_flash.c,v $
 * Revision 3.5.20.2  1996/05/21  10:04:06  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.20.1  1996/03/18  22:00:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.2  1996/03/07  10:43:37  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.14.1  1996/02/20  17:57:13  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/01/29  07:29:56  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:38:02  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/29  18:45:54  banderse
 * CSCdi43438:  CIP needs default microcode flash image
 * Added release specific default flash image; changes to ensure flash file
 * system is initialized completely before IP download.
 *
 * Revision 3.2  1995/11/17  18:44:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:27:22  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/09/27  21:53:01  izhu
 * CSCdi41172:  7000 dir/show flash command not consistant w/ 7500
 *
 * Revision 2.5  1995/08/03  21:38:53  izhu
 * CSCdi36236:  arizona rxboot fails to boot from flash w/o filename
 * added code to handle the case when file_id is NULL in
 * rp_file_boot_in_place()
 *
 * Revision 2.4  1995/07/14  19:38:12  izhu
 * CSCdi37095:  copy command doesnt support file-id
 * For 7000, add parser link points to support file-id as RSP did in
 * src-rsp/rsp_flash.c
 * Since dev/flash_chain.c is shared by other platforms, we first
 * need to check if it is 7000 before adding those parser link points.
 *
 * Revision 2.3  1995/06/28  09:30:57  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/09  00:33:04  hampton
 * Remove more useless includes of obsolete files.  [CSCdi35520]
 *
 * Revision 2.1  1995/06/07 22:55:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "config.h"
#include "parser.h"
#include "subsys.h"
#include "file.h"
#include <string.h>
#include <ciscolib.h>
#include "../dev/flash_registry.h"
#include "../dev/monitor1.h"
#include "../dev/flash_spec.h"
#include "../dev/flash_dvr_spec.h"
#include "../dev/flash_fmgr_private.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_flash.h"
#include "../parser/parser_defs_config.h"

#include "../filesys/filesys.h"
#include "../filesys/ralib.h"
#include "../filesys/fslib.h"
#include "../filesys/fslib_internals.h"
#include "../filesys/flash_command.h"
#include "../filesys/flh_config.h"
#include "../filesys/fs_rec.h"
#include "../filesys/rp_fs_dev.h"

static int RFSS_old_filesys_open_fd;        /* global current open fd     */
static sys_timestamp RFSS_timeout;          /* global current timestamp   */

#define RFSS_TIMEOUT (60 * ONESEC)          /* needs to be longer than
					       a read of the largest file */
/*
 *   Default flash device.
 */
#define DEFAULT_DEV_ID	"flash"

/*----------------------------------------------------------
 * If "file_id" contains no device_id or device id is "flash",
 * return a pointer to the filename string so the 
 * flash "interceptor" function can call the old flash function.
 * Otherwise, return NULL.
 */
static const char *is_embedded_flash (const char *file_id)
{
    const char *cp;
    int len;

    if ( (cp = strchr( file_id, ':')) != NULL) {
	len = cp - file_id;
        if( strncmp( file_id, FLASH_DEV_NAME, len) == 0) {
            cp++;           /* device id is flash, point to filename */
        } else {
            cp = NULL;      /* return NULL */
        }
	return( cp);
    }
    return(file_id);       /* no device id */
}


/*-----------------------------------------------------------
 */
static int flash_filesys_read_file (const char *file_id, uchar *bufferp,
                        ulong *sizep, boolean verbose)
{
    stat_t status;
    int fd;
    int  cnt, size, rlen;
    uchar *bufp = bufferp;
    ulong crc, bufsize;

    if (FS_stat(file_id, &status) != FSLIB_OK) {
        printf("\nFile %s not exist", file_id);
        return ERROR_NOFILE;
    }
    bufsize = *sizep;
    if ((ulong)status.st_size > bufsize) {
        printf("\nFile read buffer too small, need %d bytes, get %d bytes",
                 (ulong)status.st_size, bufsize);
        return ERROR_TOOBIG;
    }
    fd = FS_open(file_id, O_RDONLY, 0);
    if (fd < 0) {
        printf("\nFile %s open error (%d)", file_id, fd);
        return ERROR_ACCESS;
    }
    if (get_interrupt_level() == 0) {
        printf("\nReading \"%s\" from slot0 ", file_id);
        flush();
    }
    size = status.st_size;
    while ( size) {
        rlen = min( 0x4000, size);
        if ( (cnt=FS_read(fd, bufp, rlen)) != rlen) {
            printf("\nFile %s read error (%d)", file_id, cnt);
            FS_close(fd);
            return ERROR_READ;
        }
        size -= rlen;
        bufp += rlen;
	if ( get_interrupt_level() == 0) {
            printf("r");
            flush();
            ralib_yield(10000);
	}
    }
    FS_close(fd);

    if ( get_interrupt_level() == 0) {
        printf("\nVerifying crc ... ");
        flush();
    }
    if ( (crc=rp_calc_crc( bufferp, status.st_size, TRUE)) != status.crc ) {
        printf("crc error, expect %8x, actual %8x", status.crc, crc);
        flush();
        return ERROR_READ;
    }
    if ( get_interrupt_level() == 0) {
        printf("[OK]\n");
        flush();
    }
    *sizep = status.st_size;
    return ERROR_OK;
}


/*----------------------------------------------------------
 * slot0_bootfile finds the first executable system image
 * on return, it will set a pointer to the filename string
 */
static dirent_t slot0_bootfile_dent;

static const char *slot0_bootfile (void)
{
    int dfd, actual;
    dirent_t *de;
    char file_id[FSLIB_DEV_ID_MAX+1+2];

    /*
     * Fix up the special directory file ID for FS_open()
     */
    strcpy( file_id, "slot0:.\0");

    if ( (dfd = FS_open( file_id,O_RDONLY,0)) <= 0) {
        printf("\nError: FS_open (%s,%d)\n", file_id, dfd);
    } else {
        de = &slot0_bootfile_dent;
        do {
            actual = FS_getdents( dfd, (char *)de, sizeof(dirent_t));
            if (actual == sizeof(dirent_t)) {
            /*
             * check file type and get the first undeleted
             * executable system image file
             */
                if (de->deleted)
                    continue;
                if (de->valid == FALSE)
                    continue;
                if (de->type == FILE_TYPE_SYS_CONFIG)
                    continue;

                FS_close(dfd);
                return( de->d_name);
            }
        } while (actual == sizeof(dirent_t));
        FS_close(dfd);
    }
    return(NULL);
}

boolean get_flash_device_info (int devnum, int *size, int *sector, char *name)
{
    format_dev_parm_t fp_rec;
    format_dev_parm_t *fpp;
    int value;

    /*
     * Make sure flash devices are initialized.
     */
    value = ralib_device_init(devnum);
    if (value < 0)
	return FALSE;
    fpp = &fp_rec;
    memset(fpp, 0, sizeof(format_dev_parm_t));
    fpp->devnum = devnum;

    if (analyze_flash_device(fpp) >= 0) {
	*size = fpp->device_size;
	*sector = fpp->sector_size;
	sstrncpy(name, fpp->flash_name, FLASH_NAME_LEN_MAX);
	return TRUE;
    }
    return FALSE;
}

/*----------------------------------------------------------
 * Replace the embedded flash_print_hardware() registry function
 * called from the reg_invoke_flash_print_hardware()
 */
static void rp_flash_print_hardware (void)
{
    int size;
    int sector;
    char name[FLASH_NAME_LEN_MAX + 1];
    int retcode;

    flash_print_hardware();
    /*
     *  Eventually we'll ask fs to return a list
     *  of devices. For now, we simply assumes
     *  device number of 0
     */
    memset(name, 0, FLASH_NAME_LEN_MAX + 1);
    retcode = get_flash_device_info(0, &size, &sector, name);
    if (retcode == TRUE) {
        printf("%dK bytes of %s (sector size %dK).\n",
            size/1024, name, sector/1024);
        flush();
    }
}


/*----------------------------------------------------------
 * Return the initial flash device
 */
static char *default_flash_device (void)
{
    return DEFAULT_DEV_ID;
}


/*----------------------------------------------------------
 * new show flash command to support flash card 
 */
static boolean rp_show_flash_command (parseinfo *csb)
{
    int all, err, detailed, chips, filesys;
    const char *dev_id;

    dev_id = get_default_dev_id();
    if ( strcmp( dev_id, FLASH_DEV_NAME) == 0)
        return FALSE;

    all = GETOBJ(int,2);
    err = GETOBJ(int,3);
    detailed = GETOBJ(int,4);
    chips = GETOBJ(int,5);
    filesys = GETOBJ(int,8);

    if( !all && !err && !detailed && !chips && !filesys ){
	SETOBJ(int, 1) = DIR_ALL;
	dir_command(csb);
	return TRUE;
    }

    if ( all ) {
	SETOBJ(int, 1) = SHOW_FLASH_FILE_ALL;
    }else if ( chips ) {
	SETOBJ(int, 1) = SHOW_FLASH_FILE_CHIPS;
    }else if ( filesys ){
	SETOBJ(int, 1) = SHOW_FLASH_FILE_FILESYS;
    }
    show_flash_file_command( csb);
    return TRUE;
}


/*----------------------------------------------------------
 * Intercept "flash_file_length" and redirect to new flash file system
 * "flash_file_length" doesn't have a registry function
 * so we put a new registry function into itself to redirect it here
 */
boolean rp_flash_file_length (const char *file_id, long *length)
{
    stat_t status;

    if ( get_dev_id_by_file_id( file_id) == NULL)
        return FALSE;   /* no device id, return and execute the old code */

    *length = FS_stat(file_id, &status);
    if ( *length < 0)
        *length = -1;
    else
        *length = status.st_size;

    return TRUE;
}


/*----------------------------------------------------------
 * rp_file_boot_in_place
 *
 * called by reg_invoke_file_boot_in_place() from read_bootfile()
 * in os/boot.c, used by bootload process for manual/auto booting
 */
static boolean rp_file_boot_in_place (const char *file_id, uchar **bufp, 
                                         ulong *sizep)
{
    char dffid[FS_FILE_ID_MAX + 1];
    const char *dev_id;
    boolean retcode;
    const char *filename;

    if (file_id == NULL) {
        dev_id = reg_invoke_default_flash_device();
        if (dev_id == NULL) {
            return FALSE;
        }
        sprintf(dffid, "%s:", dev_id);
        setstring(&sysconfg.filename, dffid);
        file_id = &dffid[0]; /* promotes to a const char string */
    }

    if ( (filename = is_embedded_flash(file_id)) != NULL)
        retcode = boot_flash_file( filename, bufp, sizep);
    else
        retcode = rsp_file_boot_in_place( file_id, bufp, sizep);

    return( retcode);
}


/*----------------------------------------------------------
 * Intercept "flash_file_read" and redirect to new flash file system
 */
static int rp_flash_file_read ( filetype *confg, uchar *buffer,
                           ulong *buffersize, boolean verbose,
                           boolean (*func)(uchar **buf, long *bytesleft,
                           long len, void *opaque_args), void *func_arg)
{
    int retcode;
    const char *filename;

    if ( (filename = is_embedded_flash( confg->filename)) != NULL)
        retcode = flash_read_file( filename, buffer, buffersize, verbose);
    else {
        retcode = flash_filesys_read_file( confg->filename, buffer, 
                                           buffersize, verbose);
    }
    return( retcode);
} 


/*----------------------------------------------------------
 * Intercept "flash_file_source" and redirect to new flash file system
 */
static void rp_flash_file_source ( filetype *file, uchar *buffer)
{
    if ( is_embedded_flash( file->filename))
        sprintf(buffer, "flash");
    else
        sprintf(buffer, "slot0");
}


/*----------------------------------------------------------
 * Intercept "flash_file_exists" and redirect to new flash file system
 */
static boolean rp_flash_file_exists (const char *file_id)
{
    boolean retcode;
    const char *filename;

    if ( (filename = is_embedded_flash( file_id)) != NULL)
        retcode = flash_file_exists( filename);
    else
        retcode = rsp_flash_file_exists( file_id);

    return( retcode);
} 


/*----------------------------------------------------------
 * find the first bootable image on all flash device
 * The embedded flash is searched first.
 */
static const char *rp_flash_bootfile_default( void)
{
    const char *filename;

    if ( (filename = flash_bootfile()) != NULL)
        return( filename);

    return( slot0_bootfile());
}


/*----------------------------------------------------------
 * Intercept "flash_tftp_server" and redirect to new flash file system
 */
static boolean rp_flash_tftp_server (ipsocktype *soc, const char *file_id,
                                     boolean debug_flag)
{
    boolean retcode;
    const char *filename;

    if ( (filename = is_embedded_flash(file_id)) != NULL)
        retcode = flash_tftp_server( soc, filename, debug_flag);
    else
        retcode = rsp_flash_tftp_server( soc, file_id);

    return( retcode);
}


/*----------------------------------------------------------
 * Replace the old flash_file_parse() to support flash card
 */
static void rp_flash_file_parse (filetype *file, parseinfo *csb)
{
    char *bufptr;
    const char *filename;

    if (csb->nvgen) {
        nv_add(TRUE, "flash %s", file->filename);
        /*
         * Now add boot command to ROM monitor BOOT variable.
         */
        bufptr = reg_invoke_monvar_buff(ROMMON_BOOT_VAR);
        if (bufptr) {
            bufptr += strlen(bufptr);
            if ( (filename = is_embedded_flash(file->filename)) != NULL)
                sprintf(bufptr, "flash:%s,%d;", filename, 1);
            else if ( file->filename)
                sprintf(bufptr, "%s,%d;", file->filename, 1);
            else
                sprintf(bufptr, "flash:,%d;", 1);
        }
        return;
    }
    file->proto = FILE_ACCESS_FLASH;
}


/*----------------------------------------------------------
 * Intercept "flash_block_server" and redirect to new flash file system
 */
static boolean rp_flash_block_server (const char *file_id, uchar *buffer, 
                               ulong *buffersize, boolean verbose,
                               boolean (*func)(uchar **buf, long *bytesleft,
                                               long len, void *opaque_args),
                               void *func_arg)
{
    boolean retcode;
    const char *filename;

    if ( (filename = is_embedded_flash(file_id)) != NULL)
        retcode = flash_block_server( filename, buffer, buffersize, 
                                           verbose, func, func_arg);
    else
        retcode = rsp_flash_block_server( file_id, buffer, 
                                       (int)*buffersize, func, func_arg);
                                             
    return( retcode);
}

/* This rp_flash_file_system() is the same as rsp_flash_file_system()
 * in filesys/ui_command.c
 * We created this duplicate function because rsp_flash_file_system() is
 * static
 */
static boolean rp_flash_file_system (void)
{
        return TRUE;
}


/*----------------------------------------------------------
 * RP flash card driver subsystem initialization
 *
 * "rp_flash_card_driver" must be initialized after "flash_hard"
 *
 * "flash_hard" -> "rp_flash_hard" -> "flash_soft"
 * -> "rp_flash_soft"
 */
static void rp_flash_hard_init (subsystype *subsys)
{
    /*
     * Replace the embedded flash registry functions
     */
    if (!system_loading) {
        reg_delete_flash_print_hardware();
        reg_add_flash_print_hardware( rp_flash_print_hardware, 
                                        "rp_flash_print_hardware");
    }
	reg_add_default_flash_device( default_flash_device,
					"default flash device");
    /*
     * Boot system related registry functions
     */
    reg_delete_file_boot_in_place( FILE_ACCESS_FLASH);
    reg_add_file_boot_in_place( FILE_ACCESS_FLASH, rp_file_boot_in_place, 
				       "rp_file_boot_in_place");

    /*
     * RP specific new flash registry functions
     */
    reg_add_rp_flash_show( rp_show_flash_command, 
				 "rp show flash command");
    reg_add_rp_flash_file_length( rp_flash_file_length,
                                  "rp_flash_file_length");

    /* This is added to distinguish RP from other platforms so that
     * we can extend commands w/ file-id for RP in dev/flash_chain.c
     * This is deleted later when reg_add_rsp_flash_file_system() is 
     * called again in init_flash_command() in filesys/flash_command.c
     */
    reg_add_rsp_flash_file_system(rp_flash_file_system,
				   "rsp flash file sys");

}


/*
 * RP flash card driver subsystem header
 */
#define RP_FLASH_HARD_MAJVERSION		1
#define RP_FLASH_HARD_MINVERSION		0
#define RP_FLASH_HARD_EDITVERSION	1

SUBSYS_HEADER(rp_flash_hard, RP_FLASH_HARD_MAJVERSION, 
	      RP_FLASH_HARD_MINVERSION, RP_FLASH_HARD_EDITVERSION, 
	      rp_flash_hard_init, SUBSYS_CLASS_DRIVER,
              "seq: flash_hard",
              "req: flash_hard");



static void add_flash_prefix (char **filename_p, char *full_name) {
    /*
     * The following code is necessary to support the
     * old file system, and preserve the previous user 
     * interface. 
     * If the embedded flash is desired, but is not
     * prefixed with "flash:" , then it must be added.
     */
    if (!is_flash_file(*filename_p)) {
	get_whole_file_id(*filename_p, full_name, FLASH_NAME_LEN);
	*filename_p = full_name;
    }
}



/******************************************************************************
 * Name: RFSS_watchdog_start
 *
 * Description: start the timer for the remote file open timeout
 *
 * Input: open file desciptor
 *
 * Return Value:
 *
 ****************************************************************************/

static void RFSS_watchdog_start (char *file_id, int fd) {

    /*
     * The old file system only allows one file open at a time.
     * A timeout process is used to ensure that a crashed/removed
     * remote file opener does not leave that device in an 
     * unuseable state...
     * Test is perfomed to verify that the file actually opened -
     * thus, there should not be a race condition.  The file
     * system itself will prevent multiple simultaneous file opens.
     */
    if (fd >= 0)
	if (!is_new_flash_file(file_id)) {
	    RFSS_old_filesys_open_fd = fd;
	    TIMER_START(RFSS_timeout,RFSS_TIMEOUT);
	}
}

/******************************************************************************
 * Name: RFSS_watchdog_stop
 *
 * Description: stop the timer for the remote file open timeout
 *
 * Input: open file dscriptor
 *
 * Return Value:
 *
 ****************************************************************************/
static void RFSS_watchdog_stop (int fd) {

    if (fd == RFSS_old_filesys_open_fd)
	TIMER_STOP(RFSS_timeout);
}

/******************************************************************************
 * Name: RFSS_watchdog_BLOCK
 *
 * Description: determine if any device timers have expired
 *
 * Input: 
 *
 * Return Value:
 *
 ****************************************************************************/
static boolean RFSS_watchdog_BLOCK (void) {

    if (TIMER_RUNNING_AND_AWAKE(RFSS_timeout))
	return(FALSE);
    else
	return(TRUE);
}
 
/******************************************************************************
 * Name: RFSS_watchdog_process
 *
 * Description: process which closes the remotely opened file upon timeout
 *
 * Input: 
 *
 * Return Value:
 *
 ****************************************************************************/

static void RFSS_watchdog_process (void) {

    for (;;) {
	edisms((blockproc *)RFSS_watchdog_BLOCK, 0);
	TIMER_STOP(RFSS_timeout);
	FS_close(RFSS_old_filesys_open_fd);
    }
}

/*----------------------------------------------------------
 * RP flash card "soft" subsystem initialization
 * "rp_flash_soft" must be initialized after "flash_soft"
 */
static void rp_flash_soft_init (subsystype *subsys)
{
    flash_config_init();	/* Init flash config */
    init_flash_command();	/* Init flash file system command */

    /*
     * Replace the embedded flash registry functions
     */
    reg_delete_file_read( FILE_ACCESS_FLASH);
    reg_add_file_read( FILE_ACCESS_FLASH, rp_flash_file_read, 
                                              "rp_flash_file_read");
    reg_delete_file_source( FILE_ACCESS_FLASH);
    reg_add_file_source( FILE_ACCESS_FLASH, rp_flash_file_source,
                                               "rp_flash_file_source");
    reg_delete_file_exists(FILE_ACCESS_FLASH);
    reg_add_file_exists(FILE_ACCESS_FLASH, rp_flash_file_exists,
                                               "rp_flash_file_exists");
    reg_delete_flash_bootfile_default();
    reg_add_flash_bootfile_default( rp_flash_bootfile_default,
                                       "rp_flash_bootfile_default");
    reg_delete_flash_file_server();
    reg_add_flash_file_server( rp_flash_tftp_server, "rp_flash_tftp_server");

    reg_delete_file_parse(FILE_ACCESS_FLASH);
    reg_add_file_parse(FILE_ACCESS_FLASH, rp_flash_file_parse,
                                          "rp_flash_file_parse");
/*
 * The following registry functions do not need to be changed
 */
/*  reg_add_flash_file_match */
/*  reg_add_bootfile_parse */
/*  reg_add_rcp_flash_server */
/*  reg_add_write_to_flash */

    reg_delete_flash_block_server();
    reg_add_flash_block_server( rp_flash_block_server,"rp_flash_block_server");

    /*
     * The RP now provides the remote file system capability.
     * Perform initialization. (argument = MASTER)
     */

    RFSS_init(TRUE);

    reg_add_RFSS_watchdog_start(RFSS_watchdog_start,"RFSS_watchdog_start");
    reg_add_RFSS_watchdog_stop(RFSS_watchdog_stop,  "RFSS_watchdog_stop");
    reg_add_add_flash_prefix(add_flash_prefix, "add_flash_prefix");

    process_create(RFSS_watchdog_process, "RFSS watchdog",
		   NORMAL_STACK, PRIO_NORMAL);
}


/*
 * RP flash card "soft" subsystem header
 */
#define RP_FLASH_SOFT_MAJVERSION		1
#define RP_FLASH_SOFT_MINVERSION		0
#define RP_FLASH_SOFT_EDITVERSION	1

SUBSYS_HEADER(rp_flash_soft, RP_FLASH_SOFT_MAJVERSION, 
	      RP_FLASH_SOFT_MINVERSION, RP_FLASH_SOFT_EDITVERSION, 
	      rp_flash_soft_init, SUBSYS_CLASS_DRIVER,
              "seq: flash_soft",
              "req: flash_soft");

/*
 * RP flash card subsystem header
 */
#define RP_FLASH_DEVICES_MAJVERSION		1
#define RP_FLASH_DEVICES_MINVERSION		0
#define RP_FLASH_DEVICES_EDITVERSION	1

SUBSYS_HEADER(flash_devices, RP_FLASH_DEVICES_MAJVERSION, 
	      RP_FLASH_DEVICES_MINVERSION, RP_FLASH_DEVICES_EDITVERSION, 
	      NULL, SUBSYS_CLASS_DRIVER,
              "seq: rp_flash_hard rp_flash_file_system flash_file_system",
              NULL);





