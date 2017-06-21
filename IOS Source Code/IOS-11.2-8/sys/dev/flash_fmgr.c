/* $Id: flash_fmgr.c,v 3.5.22.6 1996/09/06 21:00:30 snyder Exp $
 * $Source: /release/112/cvs/Xsys/dev/flash_fmgr.c,v $
 *------------------------------------------------------------------
 * flash_fmgr.c - High level flash routines - flash file manager
 *
 *         - these routines common for xx/pan/rp1 based platforms
 *
 * 18-December-1990, Chris Shaker
 * April 1991, Steve Elias 
 * June 1991, Howard Straus
 * Aug 1991, Darren Kerr
 * Mar 1992, Dit Morse
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_fmgr.c,v $
 * Revision 3.5.22.6  1996/09/06  21:00:30  snyder
 * CSCdi68334:  dump unused spinner support, save memory
 *              412 image, 4 data
 * Branch: California_branch
 *
 * Revision 3.5.22.5  1996/08/07  08:58:04  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.5.22.4  1996/08/02  22:52:55  smackie
 * Attempt to kill two types.h birds with one stone. (CSCdi64993)
 *
 *    o Add typedefs for size-oriented intX and uintX types
 *    o Rename dev_t to flash_dev_t to avoid near-future conflicts
 *
 * Branch: California_branch
 *
 * Revision 3.5.22.3  1996/07/12  02:00:44  widmer
 * CSCdi52582:  parser should remove the trailing spaces in the filename
 * Branch: California_branch
 *
 * Revision 3.5.22.2  1996/06/28  23:05:30  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.5.22.1  1996/03/18  19:12:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.4  1996/03/09  05:04:24  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.2.3  1996/03/07  08:43:20  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.2  1996/02/20  00:44:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/01/24  20:12:56  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.4.2.1  1996/01/24  21:18:55  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/01/18  21:52:32  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/12/18  18:23:45  amcrae
 * CSCdi17352:  copy tftp flash does not strip whitespace from target
 * filename
 * Moved white space stripping to separate file, and applied it to both
 * target and destination filenames.
 *
 * Revision 3.2  1995/11/17  09:01:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:19:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:54:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/28  00:44:09  sagarwal
 * CSCdi35833:  netToFlashStatus in OLD flashmib returns 3 on 7000 platform
 * o pass correct #define to the net2flash routine
 * o allow init process to dismiss while checksumming flash
 * o made snmp initiated copy to flash console-silent
 *
 * Revision 2.2  1995/06/28  09:23:21  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:24:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* flash_dvr.h     - contains the prototypes for flash driver interfaces   */
/* g_dev.h         - i/f and codes for generic device control i/f routines */

#include "master.h"
#include "../../boot/cpu.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "../les/debug_timers.h"
#include "subsys.h"
#include "interface_private.h"
#include "config.h"
#include "../os/boot.h"
#include "../os/reloc.h"
#include "../dev/g_dev.h"
#include "logger.h"
#include "../dev/flash_spec.h"
#include "../dev/sr_old_lflashmib.h"
#include "ttysrv.h"
#include "name.h"
#include "../h/packet.h"
#include "../ip/ip.h"
#include "file.h"
#include "ieee.h"
#include "../ui/command.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../dev/flash_dvr_spec.h"
#include "../dev/flash_fmgr_private.h"
#include "../parser/parser_defs_flash.h"
#include "../clns/clns.h"
#include <ciscolib.h>
#include "parser.h"
#include "../os/init.h"

flash_t        flash = { 0 };

extern         flash_header flash_hdr;

extern ulong ep[];
extern ulong wp[];

/* #define DEBUG */

#define TFTP_OPEN_FLAGS (DEV_FLAGS_EXCL | DEV_FLAGS_NDLY | DEV_FLAGS_QUIET)

static addrtype hostip_flash;

static const char flash_net_err_msg[] = "\nTransfer %s %s failed/aborted";

static const char flash_memory_str[] = "flash memory";
static const char security_jumper_str[] = "security jumper(12V)";

static boolean get_source_filename(const char *, char *, uint);
static void get_target_filename(const char *, char *, uint);


/***************************************************************************
 *
 *                      Global Flash Routines
 *
 **************************************************************************/
     
/******************************************************************
 * flash_init
 ******************************************************************/
     
/* Notes on initialization, flash states:
 *
 * 1. The existence of flash
 *    is determined in all cases by sizing, using bus error.
 *    Note the odd cases:
 *    merlot: 
 *       ROM Monitor sets the size bits to 2 meg - 
 *       the only size currently supported by merlot.
 *    moosehead:
 *       hardware sets size bits from info on the simms
 *       bug: no simm is reported as 16 megs.
 *
 * 2. Validity of flash:
 *    Is determined by reading the chip ids, and insuring they
 *    are consistent.
 *    Invalid means:
 *    a. id not known
 *    b. not all the same size and maufacturer.
 *    c. unable to read id's (no vpp on merlot/moosehead).
 *
 * 3. Writable:
 *    If:
 *     a. vpp can be enabled
 *     b. flash is valid
 *     c. not run from flash
 *

 * Reporting:
 *
 * 1. existence:
 *    if (flash.f_dev == NULL) NoFlash
 *
 * 2. validity/writable
 *    switch(fg.fg_status) {
 *      case FR_NOT_VALID:   / * could not read ids, or ids not consistent * /
 *      case FR_READ_ONLY:   / * ids ok, but cannot write flash * /
 *      case FR_READ_WRITE:  / * ids ik, can write flash * /
 * 
 *
 * Existence of files:
 *
 *   There are files in flash if flash.f_filecnt > 0.
 */

void flash_init (subsystype *subsys)
{
    leveltype level;
  
    BEG_TIME("flash init");
  
    /* some trivial but necessary initializations */
  
    flash.f_filecnt      = 0;
    flash.f_erase_needed = FALSE;
    flash.f_free_offset  = 0;
    flash.f_verbose      = FALSE;
    flash.f_infoed       = FALSE;           /* whether flash header exists */

    hostip_flash.type    = ADDR_IP;
    hostip_flash.ip_addr = -1L;
    reg_add_file_boot_in_place(FILE_ACCESS_FLASH, boot_flash_file,
                               "boot_flash_file");
    reg_add_flash_print_hardware(flash_print_hardware, "print flash hardware");
  
    if ((flash.f_dev = flash_dvr_init()) == NULL)
        return; /* could not initialize */

    /* open flash, try and open files, and
     * if that fails, try and query roms
     */
    level = set_interrupt_level(ALL_ENABLE);
    if ((flash.f_dev = flash_open(flash.f_dev, DEV_FLAGS_EXCL)) != NULL) {
	if (!flash_read_fhdrs(flash.f_dev))
	    flash_query_roms(flash.f_dev);
	flash_close();

    }
    reset_interrupt_level(level);
}

/******************************************************************
 * show_flash: Identify the type of card, 
 ******************************************************************/

void show_flash (parseinfo *csb)
{
    int all, err, detailed, chips;
  
    /*
     * show the new flash file system if available.
     */
    if ( reg_invoke_rp_flash_show( csb) == TRUE)
        return;

    if (!flash.f_dev) {
        printf("\nFlash: %s not found", flash_memory_str);
        return;
    }
  
    if (flash_open(flash.f_dev, FLASH_OPEN | DEV_FLAGS_QUIET) == NULL)
        return;

    all = GETOBJ(int,2);
    err = GETOBJ(int,3);
    detailed = GETOBJ(int,4);
    chips = GETOBJ(int,5);
  
    flash_print_hardware();

    /* Re-query flash (if not locked) */
    
    flash_query_roms(flash.f_dev);
    switch (fg.fg_status) {
      case FR_READ_WRITE:

	if (all || err || chips)
            flash_print_chips(err);
	break;

      case FR_READ_ONLY:
	if (all || err || chips)
	    printf("Flash: mode is READ-ONLY\n");
	break;
  
      case FR_NOT_VALID:
      default:
	if (all || err || chips) {
	    printf("%s%s",
		   "Flash: Either vpp (12V) jumper is not installed\n"
		   "       or chip ids are unknown.\n");
	    flash_print_chips_known();
        }
	break;
    }
    flash_close();
    if (!chips)
        flash_print_files(all | detailed); /* print file directory */
}

/*************************************************************************
 *  flash print functions    
 *************************************************************************/

void flash_print_chips (int flag)
{
    uint i;
    int  info_idx;

    if (flag) {  /* print error info */
        printf("   Chip    socket   code      bytes     name"
	       "           erase   write\n");
        for (i = 0; i < fg.fg_chipcount; i++) {
            info_idx = flash_identify(fg.fg_rom_id[i]);
	    printf("    %2d      %4s    %4s   0x%06x   %s"
	           "      %5d  %5d\n",
                   i,
	           flash_socket(i),
                   flash_manu_code(info_idx),
                   flash_size(info_idx),
                   flash_name(info_idx),
                   ep[i],
                   wp[i]);
        }
    } else  {
        printf("   Chip    socket   code      bytes     name\n");
        for (i = 0; i < fg.fg_chipcount; i++) {
            info_idx = flash_identify(fg.fg_rom_id[i]);
	    printf("    %2d      %4s    %4s   0x%06x   %s\n",
                   i,
	           flash_socket(i),
                   flash_manu_code(info_idx),
                   flash_size(info_idx),
                   flash_name(info_idx));
        }
    }
}

void flash_print_hardware (void)
{
    if (fg.fg_size) {
        printf("%dK bytes of %s sized on %s",
              fg.fg_size/1024,
              flash_memory_str,
              fg.fg_card);

    } else if (flash.f_dev) {
        printf("%s on %s",
              flash_memory_str,
              fg.fg_card);
    }
    if (fg.fg_ctlr) {
        printf(" (in %s).\n",fg.fg_ctlr);
    } else if (fg.fg_size || flash.f_dev) {
	printf(".\n");
    }
}

void flash_print_avail (void)
{
    ulong avail;

    switch(fg.fg_status) { 
      case FR_READ_ONLY:
      case FR_READ_WRITE:
	avail = fg.fg_size - flash.f_free_offset;
	if (avail < MIN_FILE_SIZE)
	    avail = 0;
	printf(" [%d/%d bytes free/total]", avail, fg.fg_size);
	break;
      default:
	break;
    }  
}
void flash_print_files (boolean all)
{
    int i;
    flash_fhdr *fhp;

    if (flash.f_filecnt) {
	if (all)
	    printf("\nFlash file directory:\n");
	printf("File  name/status\n");
	if (all)
	    printf("        addr     length    fcksum  ccksum\n");
        fhp = &flash.f_fhdrs[0];
        for (i=0; i<flash.f_filecnt; i++, fhp++) {
	    printf(" %d  %s %s %s %s\n",                    
		   i,
		   flash.f_fhdrs[i].ffh_name,
		   (fhp->ffh_flags & FFH_FLAGS_DELETED) ? "" : "[deleted]",
		   (fhp->ffh_flags & FFH_FLAGS_SYS_CONFIG) ? "" : "[config]",
		   (fhp->ffh_checksum ^ flash.f_cp_cksum[i]) 
		       ? "[invalid cksum]" : "");
	    
	    if (all)
	    printf("      0x%8x %7d   0x%4x 0x%4x\n",
		    flash.f_addr[i],
                    flash.f_fhdrs[i].ffh_len,
		    flash.f_fhdrs[i].ffh_checksum,
		    flash.f_cp_cksum[i]);
        }
    } else {
	printf("Flash: No files present.\n");
    }
    flash_print_avail();
}

/****************************************************************** 
 * flash_copy_command(): called from system
 *
 * "copy flash tftp"  or "copy tftp flash"
 * "copy config tftp" or "copy tftp config"
 * "copy verify" checksum verify command
 * "copy ffffffff flash"  erase command 
 * "copy erase flash"  erase command
 * "copy mop flash"  copy to flash from mop server
 *
 ******************************************************************/

void flash_copy_command (parseinfo *csb)
{
    int        i;
    uchar     *bp    = NULL;
    char      *locname = NULL, *remname = NULL;
    addrtype   host;
    nametype  *ptr;
    char *host_namep;
    char *addr=NULL;
    boolean    tftp2f, flash2t, mop2f, rcp2f, flash2r;
    int sts;

    tftp2f = flash2r = mop2f = rcp2f = flash2r = FALSE;
  
    if (!flash.f_dev) {
        printf("\n%s not found", flash_memory_str);
       return;
    }
    host_namep = NULL;
  
    switch (csb->which) {
    case COPY_TFTP_FLASH:
        /*
         * Allow copy to Flash address space only if it's in
         * read-write mode.
         * If memory type in Flash address space cannot be recognized,
         * do not allow the copy.
         */
        sts = flash_query_roms(flash.f_dev);
        if(sts != FR_READ_WRITE) {
            printf("Copy aborted. Cannot write to %s.\n", fg.fg_devname);
            return;
        }
        tftp2f = TRUE;
        flash_print_files(FALSE);
        break;

    case COPY_FLASH_TFTP:
	if (!flash.f_filecnt) {
	    printf("No files in flash.\n");
	    return;
	}
        flash2r = TRUE;
 	flash_print_files(FALSE);
        break;

    case COPY_RCP_FLASH:
        /*
         * Allow copy to Flash address space only if it's in
         * read-write mode.
         * If memory type in Flash address space cannot be recognized,
         * do not allow the copy.
         */
        sts = flash_query_roms(flash.f_dev);
        if(sts != FR_READ_WRITE) {
            printf("Copy aborted. Cannot write to %s.\n", fg.fg_devname);
            return;
        }
        rcp2f = TRUE;
        flash_print_files(FALSE);
        break;
    case COPY_FLASH_RCP:
	if (!flash.f_filecnt) {
	    printf("No files in flash.\n");
	    return;
	}
        flash2t = TRUE;
 	flash_print_files(FALSE);
        break;

    case COPY_MOP_FLASH:
        /*
         * Allow copy to Flash address space only if it's in
         * read-write mode.
         * If memory type in Flash address space cannot be recognized,
         * do not allow the copy.
         */
        sts = flash_query_roms(flash.f_dev);
        if(sts != FR_READ_WRITE) {
            printf("Copy aborted. Cannot write to %s.\n", fg.fg_devname);
            return;
        }
        mop2f = TRUE;
        flash_print_files(FALSE);
	break;

    case COPY_ERASE_FLASH:  
        /*
         * Allow erase of Flash address space only if it's in
         * read-write mode.
         * If memory type in Flash address space cannot be recognized,
         * do not allow the erase.
         */
        sts = flash_query_roms(flash.f_dev);
        if(sts != FR_READ_WRITE) {
            printf("Erase aborted. Cannot erase %s.\n", fg.fg_devname);
            return;
        }
        flash_print_files(FALSE);
        erase_command();
	return;

    case COPY_VERIFY_FLASH:
        flash_print_files(FALSE);
        verify_checksum();
        return;

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
        return;
    }

    if (flash_open(flash.f_dev, FLASH_OPEN) == NULL) {
	printf("Flash: error opening flash\n");
	return;
    }  

/* after this point, can call cp_exit, flash is open, dev_close 
 * brackets properly
 */
  
  /*
   * Allocate buffer for file names. Let size of buffer be the max
   * name length plus 4 bytes.
   * Allocate buffer used for tftp and host input. Let this buffer
   * be twice as large as the max tftp data block size because the
   * tftp read will overflow it.
   */
   if (!(locname = malloc(FLASH_NAME_LEN + 4)) ||
       !(remname = malloc(FLASH_NAME_LEN + 4)) ||
       !(bp = malloc(2*TFTP_MAXDATA))) {
        printf("\nMemory allocation error");
        goto cp_exit;
    }

   /*
    * Get IP address of tftp server
    */
    if (!mop2f) {
        address_copy(&host, &hostip_flash);
	host_namep = name_addr2string(&host);
	parse_line_mode(bp, string_1_mode, 64,
			"Address or name of remote host [%s]? ",
			host_namep);
        if (*bp) {
	    host_namep = NULL;
	    addr = bp;
            if (!numeric_lookup(ADDR_IP, addr, &host)) {
		ptr = name_lookup(addr, &i, NAM_IP);
                if (ptr) {
                  address_copy(&host, &ptr->v.ip.adr[0]);
                } else {
                  printf("\n?Bad address or host name");
                  goto cp_exit;
                }
            }
        } else {
            address_copy(&host, &hostip_flash);
        }
    }

    /*
     * Get source filename.
     */
    if (flash.f_filecnt)
	strcpy(locname, flash.f_fhdrs[flash.f_filecnt - 1].ffh_name);
    else
	*locname = '\0';

    if (!get_source_filename(locname, remname, FLASH_NAME_LEN)) {
	goto cp_exit;
    }

    /*
     * Get target filename.
     */
    get_target_filename(remname, locname, FLASH_NAME_LEN);

    address_copy(&hostip_flash, &host);  /* keep new default */
    flash.f_verbose = TRUE;

    /*
     *
     * this should be made into a registry.  however, it's not as easy
     * as it sounds as we need entry and exit points that are different
     * for each case.  this needs to be done one day to make this a lot
     * neater.  it needs to be 'cross module' so it's not flash specific.
     */
    switch (csb->which) {
      case COPY_FLASH_TFTP:
          flash2tftp(flash.f_dev, &host, remname, locname, bp);
          break;
      case COPY_FLASH_RCP:
          flash2rcp(flash.f_dev, &host, remname, locname);
          break;
      case COPY_RCP_FLASH:
      case COPY_TFTP_FLASH:
      case COPY_MOP_FLASH:
          net2flash(csb->which, flash.f_dev, &host, remname, locname,
		    bp, TRUE, host_namep);
          break;
    }

cp_exit:
    flash.f_verbose = FALSE;
    free(bp);
    free(remname);
    free(locname);
    flash_close();
}


/***************************************************************************
 *
 *                      tftp Copy Support
 *
 **************************************************************************/

/*  
 * Initialize the file header
 */

flash_fhdr *flash_init_fhdr(void)
{
    flash_fhdr *ffp = &flash.f_fhdrs[flash.f_filecnt];

    ffp->ffh_magic = FLASH_FHDR_MAGIC;
    ffp->ffh_checksum = 0;
    ffp->ffh_len = 0;
    ffp->ffh_flags = (short) -1;
    ffp->ffh_addr = (uchar *) -1;
    strcpy(ffp->ffh_name,"");
    return(ffp);
}

static boolean net2flash_dummy (uchar **buf, long *bytesleft, long len,
                                void *op_args)
{
    struct flash_write_args *fa;

    fa = (struct flash_write_args *)op_args;
    if (len > 0)      /* xfer happened */
        (int)fa->ffp = 1; /* indicate success back to calling fn */
    return (FALSE); /* terminate transfer */
}

/* Write a block of tftped data to flash.
 * Compute checksum for block written.
 */
static int write_net_file(flash_fhdr *ffp, uchar *buff, uchar *flash_addr,
		   ulong len, boolean verbose)
{
    ulong nbytes;

    ffp->ffh_checksum = update_cksum(ffp->ffh_checksum, buff, len);
    ffp->ffh_len += len;
    if ((nbytes = flash_write(buff, flash_addr, len, verbose)) != len) {
        ffp->ffh_len -= len;
	if (verbose)
	    printf("\nError programming flash memory.\n");
        return (0);
    }
    return nbytes;
}

/*
 * net2flash_write_ok -- Callback routine to perform flash specific
 * actions on each tftp response packet.  Specifically, write net buffer to
 * flash, checking for errors or user interrupt.
 */
static boolean net2flash_write_ok (uchar **buf, long *bytesleft, long len,
			            void *opaque_args)
{
    int nbytes;
    struct flash_write_args *fa;

    fa = (struct flash_write_args *)opaque_args;
    if (len > 0) {           /* xfer happened */
        if ((nbytes =
	     write_net_file(fa->ffp, fa->buf, fa->flash_addr, len,
			    fa->verbose) ) == 0) {
	    return(FALSE);
	}
        fa->flash_addr += nbytes;
	/*
	 * Only use first TFTP_MAXDATA bytes of buffer.  Reset current buffer
	 * offset to beginning.
	 */
	*buf = fa->buf;
    }
    if (psipending()) {
	printf("\ncopy aborted\n");
	return(FALSE);
    }
    return(TRUE);
}



/******************************************************************
 * oldfs_is_IP_image_file:  check if this is a copy of an IP multi-
 * segment file into the old file system.
  ******************************************************************/

static boolean oldfs_is_IP_multi_segment_file (const char *buff) {
    if (strncmp("./",buff,2) == 0)
	return(TRUE);
    else
	return(FALSE);
}



/******************************************************************
 * net2flash:  copy a code image from tftp server to flash mem.
 * Called from copy_command().
 ******************************************************************/
int net2flash (int mode, flash_dev_t *dev, addrtype *host, const char *remname,
	       const char *locname, uchar *buff, boolean allowerase,
	       const char *host_namep)
{
    uchar *fhdr_addr = 0;
    uchar *flash_addr;
    filetype file;
    flash_fhdr *ffp;
    flash_fhdr *dup_ffp;
    flash_fhdr *dup_fhdr; /* header in flash of dup file */
    int fn;                      /* file number */
    struct flash_write_args fa;
    int tftp_okay;
    int xfer_status;
    ulong buffersize;
  
    /*
     * Before we go open the file, and possibly erase out the flash,
     * check if the file is accessible from the server.
     */
    switch (mode) {
    case COPY_TFTP_FLASH:
    case COPY_TFTP_BOOTFLASH:
    case COPY_RCP_FLASH:
    case COPY_RCP_BOOTFLASH:
        if (flash.f_verbose) {
            printf("\nAccessing file '%s' on %s...", remname,
                    name_addr2string(host));
            flush();
        }
        (int)fa.ffp = -1;
        fa.buf = buff;
        fa.flash_addr = 0;
        fa.verbose = flash.f_verbose;
        buffersize = TFTP_MAXDATA;

        if ((mode == COPY_TFTP_FLASH) || (mode == COPY_TFTP_BOOTFLASH))
            file.proto = FILE_ACCESS_TFTP;
        else
            file.proto = FILE_ACCESS_RCP;
        file.filename = (char*)remname;
        address_copy(&file.ps.tuba.boothost, host);
        file.ps.tuba.boothost.type = ADDR_IP;
        file.ps.tuba.username = NULL;
        file.ps.tuba.display = FALSE;

        psion(stdio);
        reg_invoke_file_read(file.proto, (filetype*)&file, (uchar *)buff,
                             &buffersize, FALSE, net2flash_dummy, 
                             (void *)&fa);
        psioff(stdio);

        if ((int)fa.ffp < 0) { /* failure */
            if (flash.f_verbose)
                printf(" NOT found !\n");
            return (-1);
        }
        if (flash.f_verbose)
            printf(" found !\n");

	/*
	 * Now that the file is found, check for IP multi-segment file.
	 * Migrate to "device:" format.
	 */
	if (oldfs_is_IP_multi_segment_file(buff)) {
	    printf("\n*** IP multi-segment image file ***\n");
	    printf("\nMust use form: copy srcdevice: destdevice:");
	    printf("\n           or: copy srcdevice:filename destdevice:");
            return (-1);
	}

	break;

    default:
	break;
    }

    /*
     * Erase vs. Append vs. Overwrite
     */
    if (flash.f_filecnt >= FLASH_FILE_CNT) {
	if (flash.f_verbose)
	    printf("\nFlash file system full. Maximum file count reached.");
	flash.f_erase_needed = TRUE;
    } else if ((fg.fg_size - flash.f_free_offset) < MIN_FILE_SIZE) {
        if (flash.f_verbose)
	    printf("\nFlash is filled to capacity.");
	flash.f_erase_needed = TRUE;
    }

    if (flash.f_verbose) {
        if (flash.f_erase_needed)
            printf("\nErasure is needed before flash may be written.");
        else
            printf("\n%d bytes available for writing without erasure.",
		   fg.fg_size - flash.f_free_offset - sizeof(flash_fhdr));
        printf("\nErase flash device before writing? ");
        if (confirm(NULL)) {
            if (flash.f_filecnt) {  /* Reconfirm if flash has files */
		if (!yes_or_no("Flash contains files.  "
			       "Are you sure" /* prompt */,
			       FALSE /* default to "no" */,
			       TRUE /* use default */))
                    allowerase = FALSE;
            }
        }
	else
	    allowerase = FALSE;
        flush();
    }
  
    if (flash.f_erase_needed && !allowerase) {
        if (flash.f_verbose)
            printf("\nCopy aborted - device needs erasure");
        return -1; /* erase needed but !allowerase */
    }

    /*
     * If not erasing, check for duplicate file. If duplicate
     * found, prompt user about invalidating first occurrence.
     * If user agrees, go ahead, else abort copy.
     */
    dup_ffp = NULL;  /* assume no duplicate */
    if (!allowerase) {
        dup_ffp = flash_get_fhdr(dev, locname);
        if (dup_ffp && flash.f_verbose) {
	    printf("\nFile '%s' already exists; it will be invalidated!", 
                   locname);
            printf("\nInvalidate existing copy of '%s'? ", locname);
            if (!confirm(NULL)) {
                printf("\nCopy aborted - user request");
                return (-1);
            }
        }
    }

    /*
     *  Have user give one final confirmation
     */
    if (flash.f_verbose) {
        printf("\nCopy '%s' from server\n"
                 "  as '%s' into Flash %s erase? [yes/no] ",
                remname, locname, allowerase ? "WITH" : "WITHOUT");

        if  (!yes_or_no(NULL, FALSE, FALSE)) {
            printf("\nCopy aborted - user request\n");
            return -1;
        }
    }

    /*
     * Now erase flash if we have to
     */
    if (allowerase) {
        if (flash_erase_all(dev) < 0) {
            flash.f_erase_needed = TRUE;
            if (flash.f_verbose)
                printf("\nError erasing Flash EPROM");
            return -1;
        }
        dup_ffp = NULL;   /* Erased flash, so no duplicates */
    }

    /*
     * Get next available file header.
     */
    flash_assert_vpp();
  
    fhdr_addr = (uchar *)(flash.f_free_offset + sizeof(flash_fhdr));

    fn = flash.f_filecnt;
    ffp = flash_init_fhdr();
    strcpy(ffp->ffh_name, locname);
    flash_addr 
	= ffp->ffh_addr 
	    = (uchar *)(flash.f_free_offset + sizeof(flash_fhdr));
    flash.f_addr[fn] = flash_address(flash_addr);
  
    fa.ffp = ffp;
    fa.buf = buff;
    fa.flash_addr = flash_addr;
    fa.verbose = flash.f_verbose;
    buffersize = fg.fg_size - (ulong)flash_addr;
    
    file.filename = (char*)remname;
    switch (mode) {
    case COPY_MOP_FLASH:
    case COPY_MOP_BOOTFLASH:
	file.proto = FILE_ACCESS_MOP;
	break;
    case COPY_TFTP_FLASH:
    case COPY_TFTP_BOOTFLASH:
	file.proto = FILE_ACCESS_TFTP;
	break;
    case COPY_RCP_FLASH:
    case COPY_RCP_BOOTFLASH:
	file.proto = FILE_ACCESS_RCP;
	break;
    default:
	file.proto = FILE_ACCESS_UNKNOWN;
	break;
    }

    if (mode != COPY_MOP_FLASH) {
        address_copy(&file.ps.tuba.boothost, host);
	file.ps.tuba.boothost.type = ADDR_IP;
        file.ps.tuba.username = NULL;
        file.ps.tuba.display = TRUE;
    }
    else {
        file.ps.mop.interface = NULL;  /* idb - don't know */
        file.ps.mop.has_addr = FALSE;  /* MOP server address unknown */
        ieee_zero(file.ps.mop.bootaddr);   /* MOP server address unknown */
        ieee_zero(file.ps.mop.respondaddr);
    }

    psion(stdio);
    tftp_okay = reg_invoke_file_read(file.proto, (filetype*)&file,
				     (uchar *)buff, &buffersize, flash.f_verbose,
				     net2flash_write_ok, (void *)&fa);
    psioff(stdio);

    if ((tftp_okay != ERROR_OK) && flash.f_verbose) {
        if (file.proto == FILE_ACCESS_TFTP || file.proto == FILE_ACCESS_RCP) {
	    printf(flash_net_err_msg, "from", name_addr2string(host));
        } else
	    printf("\n%%MOP_TO_FLASH: copy failed.\n");
    }
  
    /* done, check the transfer, verify the checksum, ... */
    xfer_status = -1; /* assume the worst */
    if (ffp->ffh_len > 0) {
	/* 
	 * We wrote something. Must write the header. 
	 */
        ffp->ffh_checksum = ~ffp->ffh_checksum;
	if (tftp_okay != ERROR_OK) {
	    /* 
	     * tftp transfer not ok 
	     */
            ffp->ffh_flags &= ~FFH_FLAGS_DELETED;
        } else {
	    /*
	     * tftp transfer ok!
	     * compute the check sum and verify
	     * mark file deleted if fails
	     */

	    flash.f_cp_cksum[fn] =
                flash_verify(flash.f_dev, ffp, buff, ffp->ffh_addr, 
			     flash.f_verbose);
           if (flash.f_cp_cksum[fn] != ffp->ffh_checksum) {
		    /* 
		     * verify error!
		     * mark new file deleted
		     */
                    ffp->ffh_flags &= ~FFH_FLAGS_DELETED;
	    } else {
		/* 
		 * verify ok!
		 * if file is duplicate, delete earlier file */
		xfer_status = 0;
		if (dup_ffp) {
		    dup_ffp->ffh_flags &= ~FFH_FLAGS_DELETED;
		    dup_fhdr = (flash_fhdr *)(dup_ffp->ffh_addr 
					      - sizeof(flash_fhdr));
		    flash_write((uchar *)&(dup_ffp->ffh_flags), \
				(uchar *)&(dup_fhdr->ffh_flags), \
				sizeof(dup_ffp->ffh_flags), TRUE);
		}
	    }
        }
	/* write new file's header */
	flash_write((uchar *)ffp, 
			  (uchar *)flash.f_free_offset, sizeof(*ffp), TRUE);
	
	/* update file specific data */
	flash.f_free_offset = 
	  flash_align(flash.f_free_offset + sizeof(flash_fhdr) + ffp->ffh_len);
	flash.f_filecnt++;
    }
    flash_deassert_vpp();
    return(xfer_status);
}

/************************************************************* 
 * flash2tftp -- copy code image from flash mem to tftp server.
 * Called from copy_command().
 *************************************************************/
int flash2tftp (flash_dev_t *dev, addrtype *host, const char *remname,
		const char *locname, uchar *buff)
{
    int blknum;
    ipsocktype *soc;
    boolean result;
    ushort cksum;
    flash_fhdr *fh;
    uchar  *file_addr;
    int nbytes;
    int rem, rlen;
  
    if (!(fh = flash_get_fhdr(dev, remname))) {
        printf("File '%s' not found in flash memory\n", remname);
        return -1;
    }

    if (flash.f_verbose) {
        printf("\nCopy '%s' from Flash to server\n  as '%s'? [yes/no] ",
               remname, locname);
        if (!yes_or_no(NULL, FALSE, FALSE)) {
            printf("\nCopy aborted - user request");
            return (-1);
	}
    }

    if (!(soc = reg_invoke_ip_tftp_open_socket(host, SOCK_ANYSRC))) {
        if (flash.f_verbose)
	    printf(flash_net_err_msg, "to", name_addr2string(host));
        return(-1);
    }
  
    if (flash.f_verbose) {
        printf("\nWriting %s ", remname);
        flush();
    }
    file_addr = fh->ffh_addr;
    cksum = 0;

    /*
     * reg_invoke_ip_tftp_WRQ(ip socket, source filename, data transfer mode,
     *                        boolean server : FALSE - display file transfer progress
     *                                         TRUE  - console silent transfer)
     */
    result = reg_invoke_ip_tftp_WRQ(soc, locname, "octet", FALSE);
    if (result != TRUE) {
        if (flash.f_verbose)
            printf("tftp timeout. Copy aborted.\n");
    } else {
        blknum = 1;
        rem = fh->ffh_len;
        while (rem > 0) {
            rlen = min(rem, TFTP_MAXDATA);
            if ((nbytes = 
               dev_read(dev, (uchar *)buff, file_addr, rlen)) != rlen) {
                if (flash.f_verbose)
	            printf("\nError reading flash. Copy aborted.\n");
	        reg_invoke_ip_close_socket(soc);
	        return(-1);
            }
            rem -= rlen;
            file_addr += rlen;
            cksum = update_cksum(cksum, buff, rlen);
            result = reg_invoke_ip_tftp_sendblock(soc, blknum++, buff,
						  rlen, FALSE);
            if (!result) {
	        break;
	    }
            flush();
        }
    }
  
    if (result) {
        if (flash.f_verbose)
            printf("copy completed.\n");
        cksum = ~cksum;
        if (cksum ^ fh->ffh_checksum) {	/* if checksum mismatch */
            if (flash.f_verbose) {
                printf("checksum failure reading flash: ");
                printf("expected 0x%04x, got 0x%04x, diff 0x%04x\n",
        	        fh->ffh_checksum, cksum, cksum ^ fh->ffh_checksum);
	    }
	    result = FALSE;
        }
    }
    reg_invoke_ip_close_socket(soc);
    if (result) {
        return(0);
    } else {
        return(-1);
    }
}

/*************************************************************
 * flash2rcp -- copy code image from flash mem to remote unix host.
 * Called from copy_command().
 *************************************************************/

typedef struct flash2rcp_context_ {
    flash_dev_t     *dev;
    uchar     *offset;
    ushort    cksum;
    uchar     *buffer;
} flash2rcp_context;

long flash2rcp_callback(uchar **destination, long bytes_read, void *arg)
{
    flash2rcp_context *c;

    if (psipending())
	return(0);

    c = (flash2rcp_context *)arg;

    *destination = c->buffer;

    bytes_read = min(bytes_read, TFTP_MAXDATA);
    bytes_read = dev_read(c->dev, *destination, c->offset, bytes_read);
    c->cksum = update_cksum(c->cksum, *destination, bytes_read);

    c->offset += bytes_read;

    return (bytes_read);
}

void flash2rcp (flash_dev_t *dev, addrtype *host, const char *remname,
		const char *locname)
{
    flash_fhdr *fh;
    flash2rcp_context context;
    int nbytes;

    if (!(fh = flash_get_fhdr(dev, remname))) {
        printf("File '%s' not found in flash memory\n", remname);
        return;
    }

    nbytes = fh->ffh_len;
    if (!(context.buffer = malloc(TFTP_MAXDATA))) {
        printf("\nMemory allocation error");
	return;
    }

    printf("\nWriting %s ", locname);
    flush();

    context.dev    = dev;
    context.offset = fh->ffh_addr;
    context.cksum  = 0;

    psion(stdio);

    /*
     * reg_invoke_rcp_write(remote dest file name, remote host ip addr,
     *                      data buffer, number of bytes to send,
     *                      filetype to supply remote username for scope of operation,
     *                      call-back func, call-back func parameters)
     */
    if (reg_invoke_rcp_write(locname, host, context.buffer, fh->ffh_len,
                             NULL, flash2rcp_callback, (void *) &context)) {

        context.cksum = ~context.cksum;
        if (context.cksum ^ fh->ffh_checksum)   /* if checksum mismatch */
            printf("checksum failure reading flash: "
                 "expected 0x%04x, got 0x%04x, diff 0x%04x\n",
                 fh->ffh_checksum, context.cksum,
                 context.cksum ^ fh->ffh_checksum);
    }

    psioff(stdio);
    free(context.buffer);
    return;
}

/*
 * flash2rcp_remote
 * Just like flash2rcp, but the rcp was initiated by the remote system.
 */
boolean flash2rcp_remote (void *mumble, const char *name)
{
    flash_fhdr *fh;
    flash2rcp_context context;
    flash_dev_t *dev;
    int error;
    tcbtype *tcb;

    tcb = (tcbtype *)mumble;

    dev = flash_open(flash.f_dev, FLASH_OPEN | DEV_FLAGS_QUIET);
    if (!dev)
        return FALSE;

    fh = flash_get_fhdr(dev, name);
    if (!fh) {
	flash_close();
        return FALSE;
    }

    context.dev    = dev;
    context.offset = fh->ffh_addr;
    context.cksum  = 0;
    context.buffer = malloc(TFTP_MAXDATA);
    if (!context.buffer) {
	flash_close();
        return(FALSE);
    }

    error = reg_invoke_rcp_send(tcb, name, context.buffer, fh->ffh_len,
                              flash2rcp_callback, (void *) &context, FALSE);

    if (error == ERROR_OK) {
        context.cksum = ~context.cksum;
        if (context.cksum ^ fh->ffh_checksum)   /* if checksum mismatch */
	    error = ERROR_ACCESS;
    }

    flash_close();
    free(context.buffer);
    return (error == ERROR_OK);
}
/*
 * write_buffer_to_flash
 * write a buffer to flash as a file
 */
boolean write_buffer_to_flash (void *tcb, const char *filename, uchar *buffer,
			       ulong buflen)
{
    flash_dev_t *dev;
    uchar *fhdr_addr = 0;
    uchar *flash_addr;
    flash_fhdr *ffp;
    flash_fhdr *dup_ffp;
    struct flash_write_args fa;
    int fn;                      /* file number */
    int rtn_code;
    boolean rcp_okay;

    dev = flash_open(flash.f_dev, FLASH_OPEN | DEV_FLAGS_QUIET);
    if (dev == NULL) {
        strcpy(buffer, "Problems opening flash for writing\n");
	return(FALSE);
    }

    dup_ffp = flash_get_fhdr(dev, filename);

    if (dup_ffp) {
	strcpy(buffer, "File already exists\n");
	flash_close();
	return(FALSE);
    }
    if (flash.f_filecnt >= FLASH_FILE_CNT) {
	strcpy(buffer, "Maximum file count exceeded\n");
	flash_close();
	return(FALSE);
    } else if ((fg.fg_size - flash.f_free_offset) < MIN_FILE_SIZE) {
	strcpy(buffer, "Flash is filled to capacity\n");
	flash_close();
	return(FALSE);
    }
    if (flash.f_erase_needed) {
        strcpy(buffer, "Erasure is needed before flash may be written\n");
	flash_close();
	return(FALSE);
    }

    /*
     * Get next available file header.
     */
    flash_assert_vpp();
  
    fhdr_addr = (uchar *)(flash.f_free_offset + sizeof(flash_fhdr));

    fn = flash.f_filecnt;
    ffp = flash_init_fhdr();
    strcpy(ffp->ffh_name, filename);

    flash_addr 
	= ffp->ffh_addr 
	    = (uchar *)(flash.f_free_offset + sizeof(flash_fhdr));
    flash.f_addr[fn] = flash_address(flash_addr);

    fa.ffp = ffp;
    fa.buf = buffer;
    fa.flash_addr = flash_addr;
    fa.verbose = FALSE;

    rtn_code = reg_invoke_rcp_receive(tcb, buffer, &buflen, FALSE,
				     net2flash_write_ok, (void *)&fa);

    rcp_okay = TRUE;  /* optimism */
    if (rtn_code != ERROR_OK) {
	strcpy(buffer, "Error in file transfer\n");
        rcp_okay = FALSE;
    }

    if (ffp->ffh_len > 0) {
	/* 
	 * We wrote something. Must write the header. 
	 */
        ffp->ffh_checksum = ~ffp->ffh_checksum;

        if (rtn_code == ERROR_OK) {
	    flash.f_cp_cksum[fn] =
	        flash_verify(dev, ffp, buffer, ffp->ffh_addr, FALSE);
            if (flash.f_cp_cksum[fn] != ffp->ffh_checksum) {
	    /* 
	     * verify error!
	     * mark new file deleted
	     */
                ffp->ffh_flags &= ~FFH_FLAGS_DELETED;
	        strcpy(buffer, "Flash write checksum error\n");
                rcp_okay = FALSE;
            }
	}
        else   /* failure */
            ffp->ffh_flags &= ~FFH_FLAGS_DELETED;

	/* write new file's header */
	flash_write((uchar *)ffp, (uchar *)flash.f_free_offset,
		    sizeof(*ffp), FALSE);
	
	/* update file specific data */
	flash.f_free_offset = 
	  flash_align(flash.f_free_offset + sizeof(flash_fhdr) + ffp->ffh_len);
	flash.f_filecnt++;
    }

    flash_deassert_vpp();
    flash_close();
    return(rcp_okay);
}

/***************************************************************************
 *
 *                      Flash Boot Support
 *
 **************************************************************************/
/******************************************************************
 * Check if system image should be moved from flash to DRAM memory.
 * Called from read_bootfile() in "boot.c"
 ******************************************************************/

boolean boot_flash_file (const char *filename, uchar **bufp, ulong *sizep)
{
    flash_dev_t *dev = flash.f_dev;
    flash_fhdr *fh;
    struct sun_exec *xec_ptr;
    uchar *flash_addr;
  
    if (!dev) {
        *sizep = 0;
        return(FALSE);
    }

    /* Block on open (in case env and stat update running) */
    if (flash_open(dev, DEV_FLAGS_EXCL | DEV_FLAGS_QUIET) == NULL) {
        *sizep = 0;
        return(FALSE);
    }
  
    if (!flash.f_filecnt) {
        flash_close();
        *sizep = 0;
        return(FALSE);
    }
  
    if (! (fh = flash_get_fhdr(dev, filename)) ) {
        flash_close();
        *sizep = 0;
        return(FALSE);
    }

  /* Check entry point in a.out header.
   * If it's equal to the image start address, & system
   * is not a DOS XX, then execute out of flash, else read 
   * image into buffer in RAM for execution from RAM.
   */
    flash_addr = (uchar *)(flash_address(fh->ffh_addr));
    xec_ptr = (struct sun_exec *)flash_addr;
    if ((cpu_type != CPU_XX) && 
        (xec_ptr->a_entry == (ulong)(flash_addr + sizeof(struct sun_exec)))) { 
       /* Run from flash only if entry pt = image start 
        * AND not a DOS XX 
        */
        setstring(&sysconfg.filename, fh->ffh_name);
        *bufp = (uchar *)flash_addr; /* skip a.out header in image */
        *sizep = fh->ffh_len;
        flash_close();
        return(TRUE);
    }
    if (fh->ffh_len > *sizep) { /* File length > buffer */
        flash_close();
        *sizep = 0;
        return (FALSE);
    }

    flash_close();
    *sizep = fh->ffh_len;
    return (FALSE);
}

int flash_read_file (const char *filename, uchar *bufp,
		    ulong *sizep, boolean verbose) 
{
    flash_dev_t *dev;
    flash_fhdr *fh;

    dev = flash.f_dev;
  
    if (!dev) {
        printf("\n%s not found", flash_memory_str);
        return(ERROR_NOFILE);
    /* not the right error */
    }

    /* Block on open (in case env and start update running) */
    if (flash_open(dev, DEV_FLAGS_EXCL | DEV_FLAGS_QUIET) == NULL) {
        flush();
        return(ERROR_NOFILE); /* see file.h */
    }
  
    if (!flash.f_filecnt) {
        printf("\nNo files found in flash memory\n");
        flush();
        dev_close(dev);
        return(ERROR_NOFILE); /* see file.h */
    }
  
    if (! (fh = flash_get_fhdr(dev, filename))) {
        printf("\nFile %s not found in flash memory\n", filename);
        flush();
        dev_close(dev);
        return(ERROR_NOFILE); /* see file.h */
    }

    if (verbose) 
	printf("\nReading %s from flash memory ",fh->ffh_name);
    return (read_flash_boot_file(dev, fh, bufp, sizep, verbose));
}


int read_flash_boot_file (flash_dev_t *dev, flash_fhdr *fh, uchar *bufp,
			  ulong *sizep, boolean netboot)
{
    uchar  *bp;
    uchar  *file_addr;
    ushort cksum;
    ulong nbytes, rem, rlen, i;
    int retries, dots;
   
    BEG_TIME("read flash boot file");

    if ((nbytes = fh->ffh_len) > *sizep) {
        printf(" [buffer overflow - %d/%d bytes]\n", nbytes, *sizep);
        flush();
        flash_close();
        return(ERROR_TOOBIG);
    }
  
    cksum = 0;
    bp = bufp;
    rem = nbytes;
    dots = 0;
    file_addr = fh->ffh_addr;
    while (rem) {
      rlen = (rem > TFTP_MAXDATA) ? TFTP_MAXDATA : rem;
      if ((i = dev_read(dev, (uchar *)bp, file_addr, rlen)) != rlen) {
	/*
	 * Seek again and retry the read.
	 */
	dots = 0;
	for (retries = 2; retries; retries--) {
	  if (netboot) {
	    printc('E');
	    flush();
	  }
	  if ((i = dev_read(dev, (uchar *)bp, file_addr, rlen)) == rlen)
	    break;
	}
	if (!retries) {
	  printf(" [Flash read failed at offset 0x%x]\n", file_addr);
	  flush();
	  flash_close();
	  return(ERROR_UNDEF);
	}
      }
      rem -= rlen;
      bp += rlen;
      file_addr += rlen;
      if ((++dots & 0x7f) == 0) {
	/*  The Boot Load is allowed to suspend.
	 *  Other process used by microcode reload should not suspend,
	 *  because they will cause a SwBus timeout in 7000.
	 */
	if ((!onintstack()) && (system_loading))
	  process_may_suspend();  
	  if (netboot) {
	    printc('r');
	    flush();
	  }
      }
    }
    
    flash_close();
    
    /* accumulate checksum */
    /*  The Boot Load is allowed to suspend.
     *  Other process used by microcode reload should not suspend,
     *  because they will cause a SwBus timeout in 7000.
     */
    if(!onintstack() && (system_loading))
      cksum = update_cksum_may_suspend(cksum, bufp, nbytes); 
    else
      cksum = update_cksum(cksum, bufp, nbytes);
    cksum = ~cksum;
    if (cksum ^ fh->ffh_checksum) {
      printf(" [Flash file checksum error:  expected %04x, got %04x]\n",
	     fh->ffh_checksum, cksum);
      flush();
        return(ERROR_UNDEF);
    }
    if (netboot) {
	printf("\n[OK - %d/%d bytes]\n", nbytes, *sizep);
	flush();
	setstring(&sysconfg.filename, fh->ffh_name);
    }
    *sizep = nbytes;
    return(ERROR_OK);
}

/*
 * Return first bootfile file on flash card, if any.  Else return NULL.
 * Currently returns first valid file.  Should check if executable.
 */
const char *flash_bootfile (void)
{
    flash_fhdr *fh;
    flash_dev_t *dev;

    dev = flash.f_dev;

    if (!dev || (flash.f_filecnt <= 0))
	return(NULL);
    if ((DEV_OK != dev_open(dev, DEV_FLAGS_EXCL|DEV_FLAGS_QUIET)))
	return(NULL);
    if (! (fh = flash_get_fhdr(dev, (char *)NULL))) {
	flash_close();
	return(NULL);
    }
    flash_close();
    return(fh->ffh_name);
}

/**************************************************************
 * flash_read_block: Called from tftp_server.c
 * read a block of data.
 **************************************************************/

int flash_read_block (int offset, uchar *buff, int len) 
{
    int rlen;

    /* check for valid flash addresses */
    if( (offset < 0) || ( (ulong) (offset + len) > fg.fg_size) )
        return(-1);
    if ((DEV_OK != dev_open(flash.f_dev, DEV_FLAGS_EXCL|DEV_FLAGS_QUIET)))
        return(-1);

    rlen = dev_read(flash.f_dev, buff, (uchar *)offset, len);
    flash_close();
    return(rlen);
}


/***************************************************************************
 * flash_file_offset:     Called from tftp_server.c
 * Return offset of start of file data (if file exists in flash).
 ***************************************************************************/

long flash_file_offset (const char *filename) 
{
    ulong i;
  
    if (!flash.f_dev || !fg.fg_size)
        return(-1);
  
    for (i=0; i<flash.f_filecnt; i++) {
        if ((flash.f_fhdrs[i].ffh_flags & FFH_FLAGS_DELETED) &&
              !strcasecmp(filename, flash.f_fhdrs[i].ffh_name))
            return((long)flash.f_fhdrs[i].ffh_addr);	    
    }
    return(-1);
}

/***************************************************************************
 * flash_file_cks:      Called from tftp_server.c
 * Return checksum of selected filename
 ***************************************************************************/
ushort flash_file_cks (const char *filename) 
{
    ulong i;
  
    if (!flash.f_dev || !fg.fg_size)
        return(0);
  
    for (i=0; i<flash.f_filecnt; i++) {
        if ((flash.f_fhdrs[i].ffh_flags & FFH_FLAGS_DELETED) &&
            (!strcasecmp(filename, flash.f_fhdrs[i].ffh_name)))
            return(flash.f_fhdrs[i].ffh_checksum);
    }
    return(0);
}
/******************************************************************
 * flash_delete_file
 ******************************************************************/
int flash_delete_file (flash_dev_t *dev, const char *filename)
{
    ulong i;
    uchar *addr;
    int sts;
    flash_fhdr *ffp = &flash.f_fhdrs[0];
  
    if (flash.f_dev == NULL) {
        printf("\n%s not found", flash_memory_str);
        return(-1);
    }
  
    if (flash.f_infoed) /* flash header present */
    addr = (uchar *)sizeof(flash_header);
    else 
        addr = 0;
    for (i=0; i<flash.f_filecnt; i++, ffp++) {
        if ((ffp->ffh_flags & FFH_FLAGS_DELETED) &&
	    !strcasecmp(filename, ffp->ffh_name)) {
            flash_assert_vpp();
            ffp->ffh_flags &= ~FFH_FLAGS_DELETED;
            sts = flash_write((uchar *)ffp, addr, sizeof(*ffp), TRUE);
            flash_deassert_vpp();
            return(sts);
        }
        addr = (uchar *)flash_align((ulong)(addr + sizeof(flash_fhdr) 
					+ flash.f_fhdrs[i].ffh_len));
    }
    return(-1);
}

/******************************************************************
 * flash_file_length:    Called from tftp_server.c
 * Return length of file (if file exists in flash).
 ******************************************************************/
long flash_file_length (const char *filename) 
{
    ulong i;
    long length;
  
    /*
     *  Use the new file system if available.
     */
    if ( reg_invoke_rp_flash_file_length(filename, &length) == TRUE)
        return( length);

    if (!flash.f_dev || !fg.fg_size)
        return(-1);
  
    for (i=0; i<flash.f_filecnt; i++) {
        if ((flash.f_fhdrs[i].ffh_flags & FFH_FLAGS_DELETED) &&
            !strcasecmp(filename, flash.f_fhdrs[i].ffh_name))
            return(flash.f_fhdrs[i].ffh_len);
    }
    return(-1);
}


/******************************************************************
 * flash_verify:
 * verify correct checksum on the file
 ******************************************************************/
ushort flash_verify (flash_dev_t *dev, flash_fhdr *ffp, uchar *bp, 
		     uchar *addr, boolean verbose)
{
    static char *errid = "\nFlash verification";
    int rlen, rem, i;
    int dots;
    ushort cksum;

    dots = 0;
    cksum = 0;

    if (ffp == NULL) {
	printf("Invalid filename.\n");
	return(-1);
    }
    
    if (verbose)
	printf("Verifying via checksum...\n");
  
    rem = ffp->ffh_len;
    do {
        rlen = min(rem, TFTP_MAXDATA);
        if ((i = dev_read(dev, bp, addr, rlen)) != rlen) {
            printf("Flash read failed at offset 0x%x\n", addr);
            return(0);
        }
        rem -= i;
        addr += i;
        if (((++dots % 32) == 0) && verbose) {
            printc('v');
            flush();
        }
        cksum = update_cksum(cksum, bp, rlen);
        /*
         * Dismiss process every 128K for 10ms to give others a chance
         * to run.
         */
        if ((dots % 256) == 0)
            flash_delay(10);

    } while (rem > 0 && rlen > 0);

    cksum = ~cksum;
    if (verbose) {
        if (cksum ^ ffp->ffh_checksum)
            printf("\n%s checksum error.  Expected %04x, got %04x on length %ld\n",
                   errid, ffp->ffh_checksum, cksum, ffp->ffh_len);
        else
            printf("\n%s successful.  Length = %ld, checksum = 0x%04X\n",
                   errid, ffp->ffh_len, ffp->ffh_checksum);
    }
    return(cksum);
}


/***************************************************************************
 *
 *                      Misc. Flash Routines
 *
 **************************************************************************/

/******************************************************************
 * flash_read_hdr:  Read main flash header struct 
 ******************************************************************/

boolean flash_read_fhdrs (flash_dev_t *dev)
{
    uchar *addr = 0;
    uchar *buff;
    int i;
  
    BEG_TIME("flash read fhdrs");

    flash_hdr.fh_magic = 0;
    if (dev_read(dev, (uchar *)&flash_hdr, addr, sizeof(flash_header))
         != sizeof(flash_header)) {
        flash_hdr.fh_magic = 0;
        return(FALSE);
    }
  
    switch (flash_hdr.fh_magic) {

    case FLASH_FHDR_MAGIC:
        flash.f_erase_needed = FALSE;
        flash.f_free_offset = 0;
        flash.f_infoed = FALSE;
        break;

    case FLASH_HDR_MAGIC:

        flash.f_erase_needed = FALSE;
        flash.f_free_offset = sizeof(flash_header);
        flash.f_infoed = TRUE;
        break;
/****  used to be, but we will never see, and we do support headers
 ****    case FLASH_CSC2_MAGIC:
 ****       return(FALSE);
 ****/
    default:
        return(FALSE);
    }  
  
    if (!(buff = malloc(TFTP_MAXDATA))) {
       printf("\nMemory allocation error");
       return (FALSE);
    }

    flash.f_filecnt = 0;
    for (i=0; i < FLASH_FILE_CNT; i++) {
	if ( (flash.f_free_offset + sizeof(flash_fhdr)) >= fg.fg_size ) 
	    break;                     /* beyond flash mem limit */
	dev_read(dev, (uchar *)&flash.f_fhdrs[i], 
		 (uchar *)flash.f_free_offset, 
		 sizeof(flash_fhdr));
        if (flash.f_fhdrs[i].ffh_magic != FLASH_FHDR_MAGIC)
            break;
        flash.f_fhdrs[i].ffh_addr = 
            (uchar *)(flash.f_free_offset + sizeof(flash_fhdr));
	flash.f_addr[i] = flash_address(flash.f_fhdrs[i].ffh_addr);
        flash.f_free_offset = 
	    flash_align(flash.f_free_offset 
			+ sizeof(flash_fhdr) 
		  + flash.f_fhdrs[i].ffh_len);
	if ( flash.f_free_offset >= fg.fg_size ) {
	    flash.f_free_offset = fg.fg_size;
	    break;                     /* beyond flash mem limit */
	}
	flash.f_cp_cksum[i] = flash_verify(dev, &flash.f_fhdrs[i], buff, 
			 		   flash.f_fhdrs[i].ffh_addr, FALSE);
        flash.f_filecnt++;
    }
    free(buff);
    return ((flash.f_filecnt == 0) ? FALSE : TRUE);
}

/******************************************************************
 * Search through flash file table for file name match
 * and seek to start of the file.
 * If filename is null, return first non-deleted file header.
 * Returns index (0-n) of file header if found,
 * -1 if not found.
 ******************************************************************/
flash_fhdr *flash_get_fhdr (flash_dev_t *dev, const char *filename)
{
    int i;

    if ((i = flash_get_fn(dev, filename)) < 0)
        return (NULL);
    else
	return (&flash.f_fhdrs[i]);
}

int flash_get_fn(flash_dev_t *dev, const char *filename)
{
    ulong i, any;
  
    if (flash.f_filecnt == 0)
        return (-1);

    if (!filename || !*filename)
        any = 1;
    else
        any = 0;
  
    for (i=0; i<flash.f_filecnt; i++) {
        if ((flash.f_fhdrs[i].ffh_flags & FFH_FLAGS_DELETED) &&
            (any || !strcasecmp(filename, flash.f_fhdrs[i].ffh_name))) {
            if (!any)
                break;
            else if (flash.f_fhdrs[i].ffh_flags & FFH_FLAGS_SYS_CONFIG)
                break;
        }
    }
  
    if (i == flash.f_filecnt)
        return(-1);
    else
	return(i);
}

/******************************************************************
 * Common flash open routine (otherwise duplicated a lot)
 ******************************************************************/
flash_dev_t *flash_open (flash_dev_t *dev, int flags)
{
    int err;
  
    if ((err = dev_open(dev, flags) != DEV_OK)) {
	if ((flags & DEV_FLAGS_QUIET) == 0)
	    printf("\nFlash device open error (%d)\n", err);
        return(NULL);
    }
    return(dev);
}

void flash_close (void)
{
    if (flash.f_dev)
	dev_close(flash.f_dev);
}
/****************************************************************** 
 * add_to_cksum(new_word, old_cksum);
 * Accumulates a checksum by adding the new word the old checksum, with
 * end-around carry.  
 * Returns the new checksum value.
 ******************************************************************/
ushort add_to_cksum(ushort new_word, ushort old_cksum)
{
    ulong work;                       /* work value for end-around add */

    work = old_cksum + new_word;      /* add new word to checksum */
    if (work & ~0xFFFF) 
        work++;	                      /* end-around carry */   
    return(work & 0xFFFF);
}

ushort update_cksum(ushort cksum, uchar *wp, ulong i)
{
    if ( (ulong)wp & 0x01 ) {
        cksum = add_to_cksum(*wp++, cksum);
        i--;
    } 

    for ( ; i > 1; i -= sizeof(ushort)) 
	cksum = add_to_cksum(*((ushort *)wp)++, cksum);
    
    if (i == 1) 
	cksum = add_to_cksum(*wp << 8, cksum);
    return(cksum);
}

ushort update_cksum_may_suspend(ushort cksum, uchar *wp, ulong i)
{
  if ( (ulong)wp & 0x01 ) {
    cksum = add_to_cksum(*wp++, cksum);
    i--;
  } 
  
  for ( ; i > 1; i -= sizeof(ushort)) {
    cksum = add_to_cksum(*((ushort *)wp)++, cksum);
    if ((i & 0xffff) <= 1)
      process_may_suspend();
  }
  
  if (i == 1) 
    cksum = add_to_cksum(*wp << 8, cksum);
  return(cksum);
}


/******************************************************************
 * verify_checksum: verify file checksum
 ******************************************************************/
void verify_checksum (void)
{
    uchar *bp;
    char *namep;
    flash_fhdr *ffp;

    bp    = NULL;
    namep = NULL;

    if (!flash.f_filecnt) {
        printf("No files in flash.\n");
        return;
    }

    if (flash_open(flash.f_dev, FLASH_OPEN) == NULL)
        return;

  /* Get filename */
    if (!(namep = malloc(FLASH_NAME_LEN + 4)) || 
        !(bp = malloc(TFTP_MAXDATA))) {
        printf("\nMemory allocation error");
        goto vp_exit;
    }

    printf("\nName of file to verify [%s]? ", 
	 flash.f_fhdrs[flash.f_filecnt - 1].ffh_name);
    rdtty(namep, FLASH_NAME_LEN-1);
    if (!*namep)
        strcpy(namep, flash.f_fhdrs[flash.f_filecnt - 1].ffh_name);
    if ((ffp = flash_get_fhdr(flash.f_dev, namep)) != NULL)
        (void) flash_verify(flash.f_dev, ffp, bp, ffp->ffh_addr, TRUE);
    else {
        printf("Flash file checksum verification aborted.\n");
        printf("File not found - deleted or not present.\n");
    }
vp_exit:
    free(bp);
    free(namep);
    flash_close();
}
  
 /******************************************************************
 * erase_command: erase flash eproms
 ******************************************************************/
int erase_command (void)
{
    char buff[16];
    int sts;
  
    if ( run_from_flash() ) {
        printf("Cannot erase flash when running from flash\n");
        return(-1);
    }

    if (flash_query_roms(flash.f_dev) != FR_READ_WRITE) {
         printf("Flash is in READ-ONLY mode. Cannot copy into flash.\n");
        return(-1);
    }
  
    printf("\nDo you really want to erase your Flash EPROMS [y/n]? ");
    rdtty(buff, 16);
    if ((buff[0] & 0xdf) != 'Y') {
        printf("erase aborted.\n");
        return(-1);
    }
    if (flash_open(flash.f_dev, FLASH_OPEN) == NULL)
        return(-1);
  
    sts = flash_erase_all(flash.f_dev);
    flash_close();
    return(sts);
}  

int flash_erase_all (flash_dev_t *dev)
{
    uint    b;
    boolean erase_ok = 0;
  
    if (fg.fg_banks <= 0) {
        if (flash.f_verbose)
            printf("No flash banks to erase !\n");
        return(-1);
    }

  /* erase all the banks, remember if any failed */  
    if (flash.f_verbose) {
        printf("\nErasing... ");
        flush();
    }
    for (b = 0; b < fg.fg_banks; b++)
        if (!flash_erase_bank(dev, b)) 
            erase_ok = -1;

    flash.f_filecnt = 0;
    flash.f_free_offset = 0;

    if (erase_ok == 0) {
        /* Write out flash header if supported */
        if (sync_flash_header(dev)) {
            /* yes it was supported */
            flash.f_free_offset = sizeof(flash_header);
            flash.f_infoed = TRUE;
        } else {
            flash.f_infoed = FALSE;
        }
        flash.f_erase_needed = FALSE;
    } else {
        flash.f_erase_needed = TRUE;
    }

    return(erase_ok);
}

/************************ SNMP routines ***************************/
/*
 * snmp_flash_exist
 *
 * Just trying to figure out if one of these puppies exists
 */
boolean snmp_flash_exist (void)
{
    return(flash.f_dev != NULL);
}

/*
 * snmp_flash_busy
 *
 * check to see if the device appears available
 */
boolean snmp_flash_busy (void)
{
    if (dev_open(flash.f_dev, FLASH_OPEN|DEV_FLAGS_QUIET) == DEV_OK) {
        flash_close();
        return(TRUE);
    }
    return(FALSE);
}

/*
 * snmp_flash_size
 *
 * how many bytes in here
 */
uint snmp_flash_size (void)
{
    return(fg.fg_size);
}

/*
 * snmp_flash_free
 *
 * how many free bytes
 */
uint snmp_flash_free (void)
{
    return(fg.fg_size - flash.f_free_offset);
}

/*
 * snmp_flash_vpp
 *
 * is dip header in place
 * check to see if this can be programmed
 * RETURN TRUE IF CAN BE PROGRAMMED, I guess.
 * a note on return values is always helpful.
 */
boolean snmp_flash_vpp (void)
{
    return(flash_query_roms(flash.f_dev) == FR_READ_WRITE);
}

/*
 * snmp_flash_filecnt
 *
 * how many files in directory
 */
int snmp_flash_filecnt (void)
{
    return(flash.f_filecnt);
}

/*
 * snmp_flash_card
 * name of card flash is on
 */
uchar *snmp_flash_card (void)
{
    return((uchar *)fg.fg_card);
}

/*
 * snmp_flash_ctlr
 * name of comtroller flash is on
 */
uchar *snmp_flash_ctlr (void)
{
    return((uchar *)fg.fg_ctlr);
}

/*
 * snmp_flash_erase_all
 * erase the flash
 */
int snmp_flash_erase_all (void)
{
    register int result;

    if ((dev_open(flash.f_dev, FLASH_OPEN|DEV_FLAGS_QUIET)) == DEV_OK) {
	if (flash_query_roms(flash.f_dev) == FR_READ_WRITE) {
	    flash.f_verbose = FALSE;
	    if (flash_erase_all(flash.f_dev) == 0)
                result = FLASH_SUCCESS;
            else
                result = FLASH_OP_FAILURE;
        } else {
            result = FLASH_READ_ONLY;
        }
	flash_close();
    } else {
        result = FLASH_OPEN_FAILURE;
    }
    return(result);
}

/*
 * snmp_flash2tftp
 *
 * copy flash entry to tftp host
 */
int snmp_flash2tftp (addrtype *address, const char *filename)
{
    uchar *bp = NULL;
    int result;

    if ((bp = malloc(TFTP_MAXDATA)) != NULL) {
        if (dev_open(flash.f_dev, FLASH_OPEN | DEV_FLAGS_QUIET) == DEV_OK) {
            flash.f_verbose = FALSE;
            if (flash2tftp(flash.f_dev, address, filename, filename, bp) == 0)
                result = FLASH_SUCCESS;
            else
                result = FLASH_OP_FAILURE;
            flash_close();
        } else
            result = FLASH_OPEN_FAILURE;
	free(bp);
    } else
	result = BUFFER_ALLOC_FAILURE;
    return(result);
}

/*
 * copy from tftp host to flash entry
 */
int snmp_tftp2flash (addrtype *address, const char *filename)
{
    uchar *bp;
    int result = TRUE;

    bp = NULL;

    if ((bp = malloc(TFTP_MAXDATA)) != NULL) {
        if (dev_open(flash.f_dev, FLASH_OPEN|DEV_FLAGS_QUIET) == DEV_OK) {
	    if (flash_query_roms(flash.f_dev) == FR_READ_WRITE) {
		flash.f_verbose = FALSE;
		if (net2flash(COPY_TFTP_FLASH, flash.f_dev, address, filename,
			      filename, bp, FALSE, NULL) == 0)
                    result = FLASH_SUCCESS;
                else
                    result = FLASH_OP_FAILURE;
	    } else {
                result = FLASH_READ_ONLY;
            }
	    flash_close();
        } else
            result = FLASH_OPEN_FAILURE;
	free(bp);
    } else
	result = BUFFER_ALLOC_FAILURE;
    return(result);
}

/*
 * snmp_flash_dir_filename
 * get the file name of a flash entry
 */
uchar *snmp_flash_dir_filename (int i)
{
    return((uchar *)flash.f_fhdrs[i].ffh_name);
}

/*
 * snmp_flash_filedeletestatus
 * get the file delete status of a flash entry
 */
boolean snmp_flash_filedeletestatus (int i)
{
    return((flash.f_fhdrs[i].ffh_flags & FFH_FLAGS_DELETED) == 0);
}

/*
 * snmp_flash_filesize
 * get the file size of a flash entry
 */
int snmp_flash_filesize (int i)
{
     return(flash.f_fhdrs[i].ffh_len);
}

static boolean get_source_filename (const char *orig_name,
				    char *sfname, uint len)
{
    parse_line_mode(sfname, string_1_mode, len,
		    "Source file name? [%s] ", orig_name);

    if (strlen(sfname) == 0) {  /* File name not specified */
	if (strlen(orig_name) == 0) {
	    printf("\n?File name needed");
	    return(FALSE);
	} else {
	    strcpy(sfname, orig_name);
	}
    }
    return (TRUE);
}

static void get_target_filename (const char *sfname, char *rfname, uint len)
{
    parse_line_mode(rfname, string_1_mode, len,
		    "Destination file name? [%s] ", sfname);
    if (strlen(rfname) == 0)
	strcpy(rfname, sfname);
}

/*
 * Flash hardware subsystem header
 */

#define FLASH_MAJVERSION 1
#define FLASH_MINVERSION 0
#define FLASH_EDITVERSION  1

SUBSYS_HEADER(flash_hard, FLASH_MAJVERSION, FLASH_MINVERSION,FLASH_EDITVERSION,
	      flash_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);
