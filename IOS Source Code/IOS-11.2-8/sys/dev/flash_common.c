/* $Id: flash_common.c,v 3.4.28.2 1996/09/06 21:00:27 snyder Exp $
 * $Source: /release/112/cvs/Xsys/dev/flash_common.c,v $
 *------------------------------------------------------------------
 * flash_common.c - platform independent flash memory support
 *
 * 26-August-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * We have two sets of flash support, one for the AGS family and
 * one for the more modern memory-mapped platforms (c3000, c4000, c7000).
 * This code is common to both sets.
 *
 * One day we will have a single set of flash support.
 *
 *------------------------------------------------------------------
 * $Log: flash_common.c,v $
 * Revision 3.4.28.2  1996/09/06  21:00:27  snyder
 * CSCdi68334:  dump unused spinner support, save memory
 *              412 image, 4 data
 * Branch: California_branch
 *
 * Revision 3.4.28.1  1996/03/18  19:12:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.4.1  1996/03/07  08:43:07  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4  1996/01/15  18:03:37  schaefer
 * CSCdi45761: support RSP in new chassis
 * remove redundant reg_add
 *
 * Revision 3.3  1995/11/29  18:44:30  banderse
 * CSCdi43438:  CIP needs default microcode flash image
 * Added release specific default flash image; changes to ensure flash file
 * system is initialized completely before IP download.
 *
 * Revision 3.2  1995/11/17  09:00:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:19:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/09  12:59:57  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:24:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "flash_registry.h"
#include "packet.h"
#include "subsys.h"
#include "address.h"
#include "config.h"
#include "file.h"
#include "flash_spec.h"
#include "../os/boot.h"
#include "../ip/ip.h"
#include "../ip/tftp.h"
#include "../ui/debug.h"
#include "../ip/udp_debug.h"
#include "../ip/tftp_debug.h"
#include "interface_private.h"
#include "../ip/ip_registry.h"

/*
 * real bad hack to put this here.  but because of the way the flash
 * files are set up, can't include the proper header file.  someone
 * more adventurous than i should really make the flash files follow
 * some sort of logic.
 */
extern boolean flash2rcp_remote(void *tcb, char *name);

/*
 * flash_tftp_server
 * See if the desired file is in flash memory.
 * If it is, use TFTP to send it across the network.
 * Returns TRUE if file transfer completed successfully.
 */

boolean flash_tftp_server (ipsocktype *soc, const char *filename,
                           boolean debug_flag)
			   
{
    int flash_offset, flash_length;
    int rem, blk, len, retries;
    uchar *block;
    boolean okay;
    ushort cks;

    block = NULL;
    if ((flash_offset = flash_file_offset(filename)) > 0) {
	block = malloc(TFTP_MAXDATA);
	if (!block)
	    return(FALSE);
	flash_length = flash_file_length(filename);
	retries = 0;
	cks = 0;
	okay = TRUE;
	for (blk = 1, rem = flash_length; okay ; ) {
	    len = (rem > TFTP_MAXDATA) ? TFTP_MAXDATA : rem;
	    if (flash_read_block(flash_offset, block, len) == len) {
		cks = update_cksum(cks, block, len);
		if (rem <= TFTP_MAXDATA) {	/* last block */
		    cks = ~cks;
		    if (cks != flash_file_cks(filename)) {
			/*
			 * Checksum error reading from flash.
			 */
			reg_invoke_ip_tftp_ERROR(soc, ERROR_UNDEF, "Checksum error");
			free(block);
			return(FALSE);
		    }
		}
		okay = reg_invoke_ip_tftp_sendblock(soc, blk, block,
						    len, TRUE);
		flash_offset += len;
		blk++;
		retries = 0;
		if ((rem -= len) == 0) {	/* all finished */
		    if (len == TFTP_MAXDATA) {
			(void) reg_invoke_ip_tftp_sendblock(soc, blk, block,
							    0, TRUE);
		    }
		    free(block);
		    return(TRUE);
		}
	    } else {
		/*
		 * Flash might have been locked by another process
		 */
		if (debug_flag)
		    buginf("\nTFTP: Flash resources in use, socket %#x", soc);
		okay = (++retries < 100);
		process_sleep_for(ONESEC);
	    }
	} 
	if (okay) {
	    free(block);
	    return(TRUE);
	}
    }
    free(block);
    return(FALSE);

}

boolean flash_block_server (const char *filename, uchar *buffer, ulong *buffersize,
                            boolean verbose,
                            boolean (*func)(uchar **buf, long *bytesleft,
                                            long len, void *opaque_args),
                            void *func_arg)
{
    int flash_offset, flash_length;
    int rem, len, retries;
    boolean okay;
    ushort cks;
    long bytesleft;

    if ((flash_offset = flash_file_offset(filename)) > 0) {
        bytesleft = flash_length = flash_file_length(filename);
        retries = 0;
        cks = 0;
        okay = TRUE;
        for (rem = flash_length; okay ; ) {
	    len = (rem > *buffersize) ? *buffersize : rem;

            if (flash_read_block(flash_offset, buffer, len) == len) {
                cks = update_cksum(cks, buffer, len);
                bytesleft -= len;
                if (rem <= *buffersize) {      /* last block */
                    cks = ~cks;
                    if (cks != flash_file_cks(filename)) {
                        /*
                         * Checksum error reading from flash.
                         */
                        if (verbose)
                            printf("\nChecksum error");
                        return(FALSE);
                    }
                }
                if (func) {
                    if ((*func)(&buffer, &bytesleft, len, func_arg) == FALSE)
			return(FALSE);
		}

                flash_offset += len;
                retries = 0;
                if ((rem -= len) == 0) {        /* all finished */
                    if (len == *buffersize) {
                        if (func)
                            okay = (*func)(&buffer, &bytesleft, 0, func_arg);
                    }
                    return(TRUE);
                }
            } else {
                /*
                 * Flash might have been locked by another process
                 */
                if (verbose)
                    buginf("\nFlash resources in use");
                okay = (++retries < 100);
                process_sleep_for(ONESEC);
            }
        }
    }
    return(FALSE);
}


/*
 * flash_file_read
 * Read a file from the flash file system.
 * Called by SERVICE_FILE_READ, index is FILE_ACCESS_FLASH
 */

static int flash_file_read (filetype *confg, uchar *buffer, 
	    		   ulong *buffersize, boolean verbose,
			   boolean (*func)(uchar **buf, long *bytesleft,
			   long len, void *opaque_args), void *func_arg)
{
    return (flash_read_file(confg->filename, buffer, buffersize, verbose));
}

/*
 * flash_file_exists
 * Return TRUE if the named file exists.
 * Called by SERVICE_FILE_EXISTS, index is FILE_ACCESS_FLASH.
 */

boolean flash_file_exists (const char *file)
{
    return((flash_file_length(file) != -1));
}

/*
 * flash_file_match
 * For the high-end, this is just a !strcasecmp()
 */
static boolean flash_file_match_wrapper (char *file_dbase, char *file_req)
{
    return (!strcasecmp(file_dbase, file_req));
}

/*
 * flash_file_source
 * Return string indicating the source of a file
 * Called by SERVICE_FILE_SOURCE, index is FILE_ACCESS_FLASH
 */

static void flash_file_source (filetype *file, uchar *buffer)
{

    if (file->proto == FILE_ACCESS_SLOT0)
    {
      sprintf(buffer, "slot0");
    }
    else if (file->proto == FILE_ACCESS_SLOT1)
    {
      sprintf(buffer, "slot1");
    }
    else
    {
      sprintf(buffer, "flash");
    }
   
}

/*
 *
 * flash_bootfile_parse
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
	printf("\n%% Bad flash arguments");
	return(FALSE);
    }
    file->proto = FILE_ACCESS_FLASH;
    return(TRUE);
}

/*
 * flash_file_parse
 * Fill in protocol specific information when parsing a file specification
 * Called by SERVICE_FILE_PARSE, index is FILE_ACCESS_FLASH
 */

static void flash_file_parse (filetype *file, parseinfo *csb)
{
    if (csb->nvgen) {
	nv_add(TRUE, "flash %s", file->filename);
	return;
    }
    file->proto = FILE_ACCESS_FLASH;
}

static boolean system_flash_rcp_remote (void *mumble, char *name)
{
    return(flash2rcp_remote(mumble, name));
}

/*
 * write_to_flash
 * Return TRUE if buffer written to flash as a file
 */

static boolean write_to_flash (void *tcb, char *filename, uchar *buffer,
			       ulong buflen)
{
    return(write_buffer_to_flash(tcb, filename, buffer, buflen));
}

/*
 * flash_common_init
 * Do flash initializations common to all platforms
 */

static void flash_common_init (subsystype *subsys)
{
    /*
     * Initialize parser
     */
    flash_parser_init();

    /*
     * Register some callback functions.
     */
    reg_add_file_read(FILE_ACCESS_FLASH, flash_file_read, "flash_file_read");
    reg_add_file_source(FILE_ACCESS_SLOT0,
			flash_file_source, "flash_file_source");
    reg_add_file_source(FILE_ACCESS_SLOT1,
			flash_file_source, "flash_file_source");
    reg_add_file_source(FILE_ACCESS_FLASH,
			flash_file_source, "flash_file_source");
    reg_add_file_exists(FILE_ACCESS_FLASH, flash_file_exists,
			"flash_file_exists");
    reg_add_flash_file_match(flash_file_match_wrapper, "flash_file_match");
    reg_add_bootfile_parse(flash_bootfile_parse, "flash_bootfile_parse");
    reg_add_flash_bootfile_default(flash_bootfile, "flash_bootfile");
    reg_add_flash_file_server(flash_tftp_server, "flash_tftp_server");
    reg_add_file_parse(FILE_ACCESS_FLASH,
		       flash_file_parse, "flash_file_parse");
    reg_add_rcp_flash_server(system_flash_rcp_remote,
			     "system_flash_rcp_remote");
    reg_add_write_to_flash(write_to_flash, "write_to_flash");
    reg_add_flash_block_server(flash_block_server, "flash_block_server");
}

/*
 * Flash subsystem header for generic software support
 */

#define FLASH_MAJVERSION 1
#define FLASH_MINVERSION 0
#define FLASH_EDITVERSION  1

SUBSYS_HEADER(flash_soft,FLASH_MAJVERSION, FLASH_MINVERSION, FLASH_EDITVERSION,
	      flash_common_init, SUBSYS_CLASS_DRIVER,
	      "seq: flash_devices",
	      NULL);
