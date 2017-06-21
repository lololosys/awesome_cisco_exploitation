/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * flash_cmnds_hes.c - High level user interface for flash commands
 *
 * January 1997, Derek Taubert
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log$
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "subsys.h"
#include "config.h"
#include "../dev/flash_defines.h"
#include "../dev/flash_services.h"
#include "../dev/dev_io.h"
#include "../dev/file_io.h"
#include "../filesys/flash_command.h"
#include "logger.h"
#include "ttysrv.h"
#include "name.h"
#include "file.h"
#include "address.h"
#include "../os/init.h"

#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_flash.h"
#include "../parser/parser_defs_config.h"
#include "../dev/sr_flashmib.h"


static char *system_flash;

/********************  Local functions ****************************/
/*
 * Scan the file name and see which format it is in :
 * <file-name> or
 * <dev>:<file-name> or
 * <dev>:<partition>:<file-name>
 * Separate out <file-name> from the rest. Supply default <dev>
 * if necessary.
 * Return device name.
 */
static void *flash_filename_process (const char *filenm)
{
    char *first_colon, *sec_colon, *tmptr;
    char *strptr;
    flash_copy_parms_t *fparms;

    if ((fparms = malloc(sizeof(flash_copy_parms_t))) == NULL) /* failure */
        return (NULL);
    strcpy(fparms->devnm, system_flash); /* Initialize to default device */
    strcpy(fparms->dfname, filenm ? filenm : ""); /* Store file name */

    /* Remove leading and trailing whitespace from input string */
    strptr = deblank(fparms->dfname);
    if (strptr) {
        tmptr = strptr + strlen(strptr) - 1;  /* last char of string */
        /* skip any trailing white spaces */
        while ((tmptr >= strptr) && ((*tmptr == ' ') || (*tmptr == '\t')))
            *tmptr-- = 0;
        strcpy(fparms->sfname, strptr);
    }
    else {
        strcpy(fparms->sfname, "");
        return (fparms);
    }

    first_colon = strchr(strptr, FLASH_PATH_SEPARATOR);
    if (first_colon)  /* We found a ':' */
        sec_colon = strchr(first_colon + 1, FLASH_PATH_SEPARATOR);
    else
        sec_colon = NULL;

    if (sec_colon) {  /*  <dev>:<partition>:<file-name> case */
        *sec_colon++ = 0;  /* term <dev>:<partition> */
        strcpy(fparms->devnm, strptr);
        strcpy(fparms->sfname, sec_colon);
    }
    else {
        if (first_colon) { /* either <dev>:<file> or <part>:<file> */
            *first_colon++ = 0;  /* point to start of <file> */

            if (isdigit(*strptr))	/* partition? */
                sprintf(fparms->devnm, "%s:%s", system_flash, strptr);
            else                        /* <device> */
                strcpy(fparms->devnm, strptr);

            strcpy(fparms->sfname, first_colon);
        }
    }
    return (fparms);
}

/****************************************************************** 
 * flash_copy_command(): 
 *
 * "copy flash tftp"        : Copy from system flash to tftp server
 * "copy bootflash tftp"    : Copy from bootstrap flash to tftp server
 * "copy tftp flash"        : Copy from tftp server to system flash
 * "copy tftp bootflash"    : Copy from tftp server to boot flash
 * "copy verify flash"      : Verify system flash file checksum 
 * "copy verify bootflash"  : Verify boot flash file checksum 
 * "copy ffffffff flash"    : system flash erase command 
 * "copy ffffffff bootflash": boot flash erase command 
 * "copy erase flash"       : system flash erase command 
 * "copy erase bootflash"   : boot flash erase command 
 * "copy config tftp" or "copy tftp config" 
 * "copy mop flash"         : Copy from MOP server to system flash
 * "copy mop bootflash"     : Copy from MOP server to boot flash
 * "copy rcp flash"         : copy from server to system flash via rcp
 * "copy rcp bootflash"     : copy from server to boot flash via rcp
 * "copy flash rcp          : copy from system flash to server via rcp
 * "copy bootflash rcp      : copy from bootstrap flash to server via rcp
 * "copy config rcp" or "copy rcp config"
 *
 * the various different calls should be made into registries.
 * should be 'cross module' instead of flash specific so more
 * functionality can be added.
 ******************************************************************/
void flash_copy_command (parseinfo *csb)
{
    flash_copy_parms_t *fparms;

    switch (csb->which) {  /* See if system has flash */

    case COPY_TFTP_FLASH:
    case COPY_FLASH_TFTP:
    case COPY_RCP_FLASH:
    case COPY_FLASH_RCP:
    case COPY_VERIFY_FLASH:
    case COPY_MOP_FLASH:
    case COPY_ERASE_FLASH:
	rsp_flash_copy_command(csb);
	return;

    case COPY_RUNNING_CONFIG_TFTP:
    case COPY_STARTUP_CONFIG_TFTP:
    case COPY_RUNNING_CONFIG_RCP:
    case COPY_STARTUP_CONFIG_RCP:
    case COPY_TFTP_RUNNING_CONFIG:
    case COPY_TFTP_STARTUP_CONFIG:
    case COPY_RCP_RUNNING_CONFIG:
    case COPY_RCP_STARTUP_CONFIG:
	break;

    default:
        printf("\n%% Unknown copy option.\n");
        return;
    }

    fparms = malloc(sizeof(flash_copy_parms_t));
    if (fparms == NULL) { /* no memory :-( */
        printf("\n%%COPY: Insufficient memory");
        return;
    }
    fparms->prompt  = FILE_FLAGS_PROMPT;
    fparms->verbose = FILE_FLAGS_VERBOSE;

    psion(stdio);  /* Enable user interrupts */

    switch (csb->which) {
#ifdef NVRAM_DRIVER
    case COPY_TFTP_CONFIG:
        tftp2nvram();
        break;

    case COPY_CONFIG_TFTP:
        nvram2tftp();
        break;
#endif

    case COPY_RUNNING_CONFIG_TFTP:
    case COPY_STARTUP_CONFIG_TFTP:
    case COPY_RUNNING_CONFIG_RCP:
    case COPY_STARTUP_CONFIG_RCP:
    case COPY_TFTP_RUNNING_CONFIG:
    case COPY_TFTP_STARTUP_CONFIG:
    case COPY_RCP_RUNNING_CONFIG:
    case COPY_RCP_STARTUP_CONFIG:

    default:
        if (!csb->nvgen) {
            printf(noasm);
        }
        break;
    }
    free(fparms);
    psioff(stdio);  /* Disable user interrupts */
}

/***********************************************************************
 *                   Other system Flash services
 **********************************************************************/
/*
 * Wrapper for function that TFTP serves a system flash file.
 */ 
static boolean flash_tftp_server_wrapper (ipsocktype *soc, const char *filename,
                                          boolean debug_flag)
{
    return rsp_flash_tftp_server(soc, filename);
}

/*
 * Wrapper for function that returns the name of the first file in
 * system flash. File should be valid. Should check if executable.
 */
static const char *flash_bootfile_default_wrapper (void)
{
    return rsp_flash_first_bootable_file();
}

/*
 * Wrapper for flash_file_read() that reads a file from system flash.
 * Called by SERVICE_FILE_READ, index = FILE_ACCESS_FLASH
 */

static int flash_file_read_wrapper (filetype *confg, uchar *buffer, 
                                    ulong *buffersize, boolean verbose,
                                    boolean (*func)(uchar **buf, 
                                    long *bytesleft, long len, 
                                    void *opaque_args),
			            void *func_arg)
{
    int value;

    value = rsp_flash_file_read(confg->filename,
        buffer, *buffersize);
    if (value > 0) {
	*buffersize = value;
	return ERROR_OK;
    }
    return ERROR_NOFILE;
}

/*
 * Wrapper for file_boot_in_place().
 * Called from read_bootfile() in os/boot.c to determine whether
 * specified file should execute in-place or not.
 */
static boolean file_boot_in_place_wrapper (const char *filename,
                                           uchar **bufpp,
                                           ulong *sizep)
{
    return rsp_file_boot_in_place(filename, bufpp, sizep);
}

/*
 * system_flash_file_exists
 * Return TRUE if the named file exists.
 * Called by SERVICE_FILE_EXISTS, index is FILE_ACCESS_FLASH.
 */
static boolean flash_file_exists_wrapper (const char *file)
{
    return rsp_flash_file_exists(file);
}

/*
 * Wrapper for function to match a filename in the database of files
 * to be tftp served against the filename that has come in with the
 * tftp request.
 */
static boolean flash_file_match_wrapper (char *file_dbase, char *file_req)
{
    flash_copy_parms_t *fparms_dbase = NULL;
    flash_copy_parms_t *fparms_req = NULL;
    boolean sts = FALSE;

    if ((fparms_dbase = flash_filename_process(file_dbase)) &&
        (fparms_req = flash_filename_process(file_req)))
        sts = flash_file_match(fparms_dbase, fparms_req);

    if (fparms_dbase)
        free(fparms_dbase);
    if (fparms_req)
        free(fparms_req);
    return (sts);
}

/*
 * Wrapper for flash_file_parse().
 * Fill in protocol specific information when parsing a file specification
 * Called by SERVICE_FILE_PARSE, index is FILE_ACCESS_FLASH
 */

static void flash_file_parse_wrapper (filetype *file, parseinfo *csb)
{
    rsp_flash_file_parse(file->filename, csb);
}

static boolean system_flash_rcp_remote (void *mumble, char *name)
{
    flash_copy_parms_t *fparms;
    boolean sts = FALSE;

    if ((fparms = flash_filename_process(name)) != NULL) {
        sts = flash2rcp_remote(fparms, mumble);
        free(fparms);
    }
    return (sts);
}

/* 
 * Wrapper for flash_block_server
 */
static boolean system_flash_block_server (const char *filename, uchar *buffer,
                                   ulong *buffersize, boolean verbose,
                                   boolean (*func)(uchar **buf, long *bytesleft,
                                                   long len, void *opaque_args),
                                   void *func_arg)
{
    return rsp_flash_block_server(filename, buffer, *buffersize,
            func, func_arg);
}

/*
 * write_to_flash
 * Return TRUE if buffer written to flash as a file
 */
static boolean write_to_flash (void * tcb, char * filename, uchar *buffer,
			       ulong buflen)
{
    flash_copy_parms_t *fparms;
    boolean sts = FALSE;

    if ((fparms = flash_filename_process(filename)) != NULL) {
        sts = write_file_to_flash(fparms, tcb, buffer, buflen);
        free(fparms);
    }
    return (sts);
}
/*--------------------------- end of wrappers ------------------------*/

/*
 * Wrapper for flash_file_source().
 * Called by SERVICE_FILE_SOURCE, index is FILE_ACCESS_FLASH
 */
static void flash_file_source (filetype *file, uchar *buffer)
{
    rsp_flash_file_source(file->filename, buffer);
}

/*
 * Given a protocol name and parts of a source address, figure out if
 * Flash should be used to access the file.  Set up fields in file
 * descriptor as appropriate. Returns TRUE if there was a match.
 * Called by SERVICE_BOOTFILE_PARSE, index is FILE_ACCESS_FLASH
 */
static boolean flash_bootfile_parse (filetype *file, char *protocol,
                                     char *src1, char *src2)
{
    if (!strmatch(protocol, "flash", FALSE))
        return(FALSE);
    if (!null(src1) || !null(src2)) {
        printf("\n%% Invalid flash boot arguments");
        return(FALSE);
    }
    file->proto = FILE_ACCESS_FLASH;
    return(TRUE);
}

static void flash_services_init (subsystype *subsys)
{
    /*
     * Register some callback functions.
     */
    reg_add_file_exists(FILE_ACCESS_FLASH, flash_file_exists_wrapper,
                        "flash_file_exists");
    reg_add_flash_file_match(flash_file_match_wrapper, "flash_file_match");
    reg_add_file_read(FILE_ACCESS_FLASH, flash_file_read_wrapper, 
                      "flash_file_read");
    reg_add_file_parse(FILE_ACCESS_FLASH, flash_file_parse_wrapper, 
                       "flash_file_parse");
    reg_add_file_boot_in_place(FILE_ACCESS_FLASH, file_boot_in_place_wrapper,
                               "flash_file_boot_in_place");
    reg_add_flash_bootfile_default(flash_bootfile_default_wrapper,
                                   "flash_bootfile");
    reg_add_flash_file_server(flash_tftp_server_wrapper, "flash_tftp_server");

    reg_add_file_source(FILE_ACCESS_FLASH,
			flash_file_source, "flash_file_source");
    reg_add_file_source(FILE_ACCESS_SLOT0,
			flash_file_source, "flash_file_source");
    reg_add_file_source(FILE_ACCESS_SLOT1,
			flash_file_source, "flash_file_source");
    reg_add_bootfile_parse(flash_bootfile_parse, "flash_bootfile_parse");
    reg_add_flash_filename_parse(flash_filename_process,
                                 "flash filename parse");

    reg_add_rcp_flash_server(system_flash_rcp_remote,
			     "system_flash_rcp_remote");
    reg_add_write_to_flash(write_to_flash, "write_to_flash");
    reg_add_flash_block_server(system_flash_block_server, 
			       "flash_block_server");
    reg_add_get_num_flash_part(dev_get_num_flash_part,"dev_get_number_flash_part");
    reg_add_multi_part_get_file_name(multi_part_get_file_name,
				     "multi_part_get_file_name");

    system_flash = reg_invoke_flash_device_default();
    if (system_flash == NULL) /* no device */
        system_flash = "";    /* set to a NULL string */
}

/*
 * System Flash subsystem header for generic software support
 */

#define SYSFLASH_MAJVERSION 1
#define SYSFLASH_MINVERSION 0
#define SYSFLASH_EDITVERSION  1

SUBSYS_HEADER(flash_services_wrappers, SYSFLASH_MAJVERSION, SYSFLASH_MINVERSION, 
              SYSFLASH_EDITVERSION, flash_services_init, SUBSYS_CLASS_DRIVER,
	      "seq: flash_devices", NULL);
