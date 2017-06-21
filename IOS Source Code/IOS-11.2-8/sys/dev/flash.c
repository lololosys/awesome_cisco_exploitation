/* $Id: flash.c,v 3.7.18.5 1996/09/06 21:00:24 snyder Exp $
 * $Source: /release/112/cvs/Xsys/dev/flash.c,v $
 *------------------------------------------------------------------
 * flash.c - High level flash routines
 *
 * 18-December-1990, Chris Shaker
 * April 1991, Steve Elias 
 * June 1991, Howard Straus
 * Aug 1991, Darren Kerr
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash.c,v $
 * Revision 3.7.18.5  1996/09/06  21:00:24  snyder
 * CSCdi68334:  dump unused spinner support, save memory
 *              412 image, 4 data
 * Branch: California_branch
 *
 * Revision 3.7.18.4  1996/08/02  22:52:47  smackie
 * Attempt to kill two types.h birds with one stone. (CSCdi64993)
 *
 *    o Add typedefs for size-oriented intX and uintX types
 *    o Rename dev_t to flash_dev_t to avoid near-future conflicts
 *
 * Branch: California_branch
 *
 * Revision 3.7.18.3  1996/07/12  02:00:40  widmer
 * CSCdi52582:  parser should remove the trailing spaces in the filename
 * Branch: California_branch
 *
 * Revision 3.7.18.2  1996/05/21  06:26:36  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.7.18.1  1996/03/18  19:12:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.3  1996/03/09  05:04:20  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.2.2  1996/03/07  08:43:01  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/02/20  00:44:31  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/09  22:49:43  smackie
 * Remove cross-class sequence from AGS flash subsystem (CSCdi48574)
 *
 * Revision 3.6  1996/02/07  16:10:53  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.5  1996/01/24  20:12:53  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.4  1996/01/18  21:52:25  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1996/01/11  02:23:24  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * changed registry function ip_tuba_move_address to inline
 * ip_copy_address().
 *
 * Revision 3.2  1995/11/17  09:00:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:18:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/07/16  18:03:48  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Bad programmer, no doggie biscuit.
 *
 * Revision 2.4  1995/07/15  02:36:43  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Add a silent_mode parameter to some routines and registries and set it
 * to TRUE when making the calls from SNMP WriteNet, netConfigSet, or
 * hostConfigSet.  Also, add an errmsg for each so the event can still get
 * logged, and make it clear what caused the event.  Fix other callers to
 * pass FALSE so current behavior will be maintained.
 *
 * Revision 2.3  1995/06/18  19:40:39  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  12:59:54  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:24:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "../../boot/cpu.h"
#include "interface_private.h"
#include "subsys.h"
#include "../dev/dev.h"
#include "config.h"
#include "file.h"
#include "logger.h"
#include "sys_registry.h"
#include "../ip/ip_registry.h"
#include "../dev/flash_registry.h"
#include "../os/boot.h"
#include "../dev/flash_spec.h"
#include "../dev/flash_private.h"
#include "../dev/sr_old_lflashmib.h"
#include "ttysrv.h"
#include "../../boot/src-68-ags/defs_csc2.h"
#include "../../boot/i8254.h"
#include "name.h"
#include "../h/packet.h"
#include "../ip/ip.h"
#include "parser.h"
#include "../parser/parser_defs_flash.h"
#include "../hes/ec.h"
#include "../os/free.h"
#include "../snmp/chassismib_registry.h"
#include "../ui/command.h"
#include "ieee.h"
#include "../clns/clns.h"

#define mc_debug 0

flash_t flash;			/* Global FLASH struct used by drivers  */
void (*usec_delay)(uint);	/* function pointer to appropriate */
				/* usec delay routine              */

char *flash_sockets[MAX_ROMS] = {
    "U2 ", 		/* 0 */
    "U19", 		/* 1 */
    "U3 ", 		/* 2 */
    "U20", 		/* 3 */
    "U4 ", 		/* 4 */
    "U21", 		/* 5 */
    "U5 ", 		/* 6 */
    "U22", 		/* 7 */
    "U9 ", 		/* 8 */
    "U26", 		/* 9 */
    "U10", 		/* 10 */
    "U27", 		/* 11 */
    "U11", 		/* 12 */
    "U28", 		/* 13 */
    "U12", 		/* 14 */
    "U29"		/* 15 */
};

/*
 * Manufacturer codes table:
 */
struct flash_info flash_info_tbl[MAX_ROM_MFGS] = {
    /* manu_dev_code, device_size, name */
    {(ushort)0x89B4, 0x20000L, "INTEL 28F010"}, 
    {(ushort)0x01A7, 0x20000L, "AMD   28F010"}, 
    {(ushort)0x1CD0, 0x20000L, "M5M28F101P  "}, 
    {(ushort)0x89BD, 0x40000L, "INTEL 28F020"}, 
    {(ushort)0x012A, 0x40000L, "AMD   28F020"}, 
};

extern flash_dev_t devtable[];

static const char flash_net_err_msg[] = "\nTransfer %s %s failed/aborted";

static const char flash_memory_str[] = "flash memory";
static const char security_jumper_str[] = "security jumper";
static addrtype hostip_flash;

static boolean get_source_filename(const char *, char *, uint);
static void get_target_filename(const char *, char *, uint);

/*
 * if (flash present)
 *     add(MC+);
 * if (nvram via multibus) {
 *     if (envm present) {
 *	   add(ENVM);		-- postpone this until ec_show is valid
 *     } else {
 *	   if (multibus memory present)
 *	       add(CSC-MT);
 *	   else
 *	       add(CSC-M);
 *     }
 * } else {
 *     if (nvram via MCI && !flash)
 *	   add(CSC_MC);
 * }
 */

static inline void
decipher_memoptions (void)
{
    extern idbtype* nv_idbp;

    if (flash.f_dev) {
	char* descr;

	if (ec_flash_cmdfails == 0)
	    descr = "pBus FLASH";
	else
	    descr = "xBus FLASH";
	reg_invoke_chassismib_add_card_entry(
		      D_cardType_csc_mcplus, NO_HOTSWAP, descr, NO_SERIALNUM,
		      NO_HWVERSION, NO_SWVERSION, NO_SLOTNUMBER);
    }
    if (nvsize && !nv_idbp) {
	if (!ec_present) {
	    if (mempool_find_by_class(MEMPOOL_CLASS_MULTIBUS)) {
		reg_invoke_chassismib_add_card_entry(
			      D_cardType_csc_mt, NO_HOTSWAP,
			      "Multibus NVRAM & interface memory",
			      NO_SERIALNUM, NO_HWVERSION, NO_SWVERSION,
			      NO_SLOTNUMBER);
	    } else {
		reg_invoke_chassismib_add_card_entry(
			      D_cardType_csc_m, NO_HOTSWAP, "Multibus NVRAM",
			      NO_SERIALNUM,
			      NO_HWVERSION, NO_SWVERSION, NO_SLOTNUMBER);
	    }
	}
    } else {
	if (nvsize && nv_idbp && !flash.f_dev) {
	    reg_invoke_chassismib_add_card_entry(
			  D_cardType_csc_mc, NO_HOTSWAP, "xBus NVRAM", 
			  NO_SERIALNUM, NO_HWVERSION, NO_SWVERSION, 
			  NO_SLOTNUMBER);
	}
    }
    return;
}

/***************************************************************************
 *
 *                      Global Flash Routines
 *
 **************************************************************************/

/*
 * Identify the type of MC card, amount of NVRAM installed, and which
 * FLASH EPROM parts are present (MC+ only):
 */

void show_flash (parseinfo *csb)
{
    int dev;
    int all, detailed, chips;
    
    if (!flash.f_dev) {
	printf("\n%s not found", flash_memory_str);
	return;
    }

    all = GETOBJ(int,2);
    detailed = GETOBJ(int,4);
    chips = GETOBJ(int,5);

    /*
     * Re-query flash (if not locked)
     */
    if ((dev = dev_open(flash.f_dev, FLASH_OPEN|DEV_FLAGS_QUIET)) >= 0) {
	flash_read_fhdrs(dev);
	flash_query_roms(dev);
	dev_close(dev);
    }

    automore_enable(NULL);
    flash_print_hardware();
    
    if (!flash.f_vpp_works)
	printf("  %s is uninstalled, so %s is read-only\n",
	       security_jumper_str, flash_memory_str);

    if (all || chips) {
        flash_print_chips();
    }

    if (!chips)
        flash_print_files(all | detailed);

    automore_disable();
}

void flash_print_chips (void)
{
    int i, info_idx;

    for (i = 0; i < flash.f_chipcount; i++) {
        info_idx = flash.f_info_idx[i];
        printf("   ROM %d, %s, code 0x%x, size 0x%x, name %s\n",
	       i,
	       flash_sockets[i],
	       flash_info_tbl[info_idx].manu_dev_code,
	       flash_info_tbl[info_idx].device_size,
	       flash_info_tbl[info_idx].name);
    }
}

void flash_print_hardware (void)
{
    if (flash.f_size) {
	printf("%dK bytes of %s on %s (via %s).\n",
	       flash.f_size/1024,
	       flash_memory_str,
	       flash.f_card,
	       flash.f_ctlr);
    } else if (flash.f_dev) {
	printf("%s on %s (via %s).\n",
	       flash_memory_str,
	       flash.f_card,
	       flash.f_ctlr);
    }
}

/* 
 * flash_copy_command()
 * "copy flash tftp"  or "copy tftp flash"
 * "copy config tftp" or "copy tftp config"
 */
void flash_copy_command (parseinfo *csb)
{
    int dev;
    int i;
    uchar *bp;
    char *namep, *addr=NULL;
    char *rfname;
    char *host_namep = NULL;
    addrtype host;
    nametype *ptr;
    boolean t2f, f2t, r2f, f2r;
    boolean mop2f;    

    host_namep = NULL;
    t2f = f2t = r2f = f2r = mop2f = FALSE;

    switch (csb->which) {
      case COPY_TFTP_FLASH:
	t2f = TRUE;
	flash_print_files(FALSE);
	break;
      case COPY_FLASH_TFTP:
	f2t = TRUE;
	flash_print_files(FALSE);
	break;
      case COPY_RCP_FLASH:
	r2f = TRUE;
	flash_print_files(FALSE);
	break;
      case COPY_FLASH_RCP:
	f2r = TRUE;
	flash_print_files(FALSE);
	break;
      case COPY_ERASE_FLASH:
	flash_print_files(FALSE);
	erase_command();
	return;
      case COPY_VERIFY_FLASH:
	flash_print_files(FALSE);
	verify_checksum();
	return;
      case COPY_MOP_FLASH:
	mop2f = TRUE;
	flash_print_files(FALSE);
	break;
      case COPY_RUNNING_CONFIG_TFTP:
      case COPY_STARTUP_CONFIG_TFTP:
      case COPY_RUNNING_CONFIG_RCP:
      case COPY_STARTUP_CONFIG_RCP:
      case COPY_TFTP_RUNNING_CONFIG:
      case COPY_TFTP_STARTUP_CONFIG:
      case COPY_RCP_RUNNING_CONFIG:
      case COPY_RCP_STARTUP_CONFIG:
      default:
	bad_parser_subcommand(csb, csb->which);
	return;
    }

    /*
     * Allocate buffer used for tftp and host input
     * need twice the buffer size since during reads, we overflow
     * the size of one tftp segment.
     */
    if (!(bp = malloc(2*TFTP_MAXDATA))) {
	printf(nomemory);
	return;
    }

    /*
     * Allocate buffer for source file name
     */
    if (!(namep = malloc(FLASH_NAME_LEN))) {
	free(bp);
	printf(nomemory);
	return;
    }

    /*
     * Allocate buffer for remote file name
     */
    if (!(rfname = malloc(FLASH_NAME_LEN))) {
	free(bp);
	free(namep);
	printf(nomemory);
	return;
    }
    
    dev = flash_open();
    
    if (dev == 0) {
	free(bp);
	free(namep);
	free(rfname);
	return;
    }

    /*
     * Get latest info (user may have installed security jumper)
     */
    flash_read_fhdrs(dev);
    flash_query_roms(dev);

    if ((t2f || r2f) && !flash.f_vpp_works) {
	printf("%s %s must be strapped to modify %s\n",
	       flash.f_card, security_jumper_str, flash_memory_str);
	free(bp);
	free(namep);
	free(rfname);
	dev_close(dev);
	return;
    }

    if (!mop2f) {
	host_namep = name_addr2string(&hostip_flash);
	/*
	 * Get IP address of tftp server
	 */
	parse_line_mode(bp, string_1_mode, FLASH_NAME_LEN,
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
		    free(bp);
		    free(namep);
		    free(rfname);
		    dev_close(dev);
		    return;
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
	strcpy(namep, flash.f_fhdrs[flash.f_filecnt - 1].ffh_name);
    else
	*namep = '\0';

    if (!get_source_filename(namep, rfname, FLASH_NAME_LEN)) {
        free(bp);
        free(namep);
        free(rfname);
        dev_close(dev);
	return;
    }

    /*
     * Get target filename.
     */
    get_target_filename(rfname, namep, FLASH_NAME_LEN);

    address_copy(&hostip_flash, &host);  /* keep new default */

    flash.f_verbose = TRUE;

    /*
     * this should be made into a registry.  however, it's not as easy
     * as it sounds as we need entry and exit points that are different
     * for each case.  this needs to be done one day to make this a lot
     * neater.  it needs to be 'cross module' so it is not flash specific.
     */

    if (t2f || r2f || mop2f) {
	net2flash(csb->which, dev, &host, rfname, namep, bp, flash.f_verbose);
    } else if (f2t) {
	flash2tftp(dev, &host, rfname, namep, bp);
    } else if (f2r)
	flash2rcp(dev, &host, rfname, namep);

    flash.f_verbose = FALSE;
    free(bp);
    free(namep);
    free(rfname);
    dev_close(dev);
}

/***************************************************************************
 *
 *                      Copy Commands
 *
 **************************************************************************/

static boolean net2flash_dummy (uchar **buf, long *bytesleft, long len,
                                void *op_args)
{
    struct flash_write_args *fa;

    fa = (struct flash_write_args *)op_args;
    if (len > 0)      /* xfer happened */
        (int)fa->ffp = 1; /* indicate success back to calling fn */
    return (FALSE); /* terminate transfer */
}

/*
 * net2flash_write_ok -- Callback routine to perform flash specific
 * actions on each tftp response packet.  Specifically, compute checksum,
 * and write tftp buffer to flash.
 */
boolean net2flash_write_ok (uchar **buf, long *bytesleft, long len,
			     void *opaque_args)
{
    ushort *wp;
    int i, nbytes;
    struct flash_write_args *fa;

    fa = (struct flash_write_args *)opaque_args;

    if (len > 0) {           /* xfer happened */
	if (len & 1) { /* our stupid drivers don't support odd */
	    fa->buf[len] = 0;
	    len++;
	}
	fa->ffp->ffh_len += len;
	for (wp = (ushort *)fa->buf, i = len/sizeof(ushort); i; i--)
	    fa->ffp->ffh_checksum = add_to_cksum(*wp++, fa->ffp->ffh_checksum);
	if ((nbytes = dev_write(fa->dev, (char *)fa->buf, len)) != len) {
	    if (fa->verbose)
		printf("\nError programming flash memory");
	    return(FALSE);
	}
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

/*
 * net2flash --  copy a code image from network server to flash mem.
 * Called from copy_command().
 */
int net2flash (int mode, ulong dev, addrtype *host, const char *remname,
	       const char *locname, uchar *buff, boolean eraseok)
{
    ulong romaddr;
    struct flash_write_args fa;
    int tftp_okay;
    ulong buffersize;
    filetype file;
    flash_fhdr *ffp, *dup_ffp;

    romaddr = 0;
    
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
        fa.verbose = TRUE;
        buffersize = TFTP_MAXDATA;

        if ((mode == COPY_TFTP_FLASH) || (mode == COPY_TFTP_BOOTFLASH))
            file.proto = FILE_ACCESS_TFTP;
        else
            file.proto = FILE_ACCESS_RCP;
        file.filename = (char *)remname;
        address_copy(&file.ps.tuba.boothost, host);
        file.ps.tuba.boothost.type = ADDR_IP;
	file.ps.tuba.username = NULL;
	file.ps.tuba.display = (flash.f_verbose ? TRUE : FALSE);

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
    } else if ((flash.f_size - flash.f_free_offset) < MIN_FILE_SIZE) {
	if (flash.f_verbose)
	    printf("\nFlash is filled to capacity.");
	flash.f_erase_needed = TRUE;
    }
    if (flash.f_verbose) {
	if (flash.f_erase_needed)
	    printf("\nErasure is needed before flash may be written.");
	else
	    printf("\n%d bytes available for writing without erasure.",
		   flash.f_size - flash.f_free_offset - sizeof(flash_fhdr));
	printf("\nErase flash device before writing? ");
        if (confirm(NULL)) {
            if (flash.f_filecnt) {  /* Reconfirm if flash has files */
		if (!yes_or_no("Flash contains files.  "
			       "Are you sure" /* prompt */,
			       FALSE /* default to "no" */,
			       TRUE /* use default */))
                    eraseok = FALSE;
            }
        }
	else
	    eraseok = FALSE;
	flush();
    }
  
    if (flash.f_erase_needed && !eraseok) {
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
    if (!eraseok) {  /* don't erase */
        dup_ffp = flash_get_fhdr(dev, locname);
        if (dup_ffp && flash.f_verbose) {
            printf("\nFile '%s' already exists; it will be invalidated!",
                   locname);
            printf("\nInvalidate existing copy of '%s'? ",locname);
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
                remname, locname, eraseok ? "WITH" : "WITHOUT");

        if  (!yes_or_no(NULL, FALSE, FALSE)) {
            printf("\nCopy aborted - user request\n");
            return -1;
        }
    }

    /*
     * Now erase flash if we have to
     */
    if (eraseok) {
        if (flash_erase_all(dev) < 0) {
            flash.f_erase_needed = TRUE;
            if (flash.f_verbose)
                printf("\nError erasing Flash EPROM");
            return -1;
        }
        dup_ffp = NULL;  /* Erased flash, so no duplicates */
    }

    /*
     * Get next available file header.
     */
    flash_vpp_set(dev, VPP_ON);
    romaddr = flash.f_free_offset + sizeof(flash_fhdr);
    dev_ioctl(dev, IOC_FLASH_SEEK, &romaddr);
    
    ffp = &flash.f_fhdrs[flash.f_filecnt & 0x0f];
    
    strcpy(ffp->ffh_name, locname);
    
    ffp->ffh_checksum = 0;
    ffp->ffh_magic = FLASH_FHDR_MAGIC;
    ffp->ffh_len = 0;
    ffp->ffh_flags = (short) -1;
    ffp->ffh_reserved = (long) -1;
    
    fa.ffp = ffp;
    fa.dev = dev;
    fa.buf = buff;
    fa.verbose = flash.f_verbose;
    buffersize = flash.f_size - romaddr;
    
    file.filename = (char *)remname;
    
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
	bcopy(host, &file.ps.tuba.boothost, sizeof(addrtype));
	file.ps.tuba.boothost.type = ADDR_IP;
	file.ps.tuba.username = NULL;
        file.ps.tuba.display = TRUE;
    } else {
	file.ps.mop.interface = NULL;  /* idb - don't know */
	file.ps.mop.has_addr = FALSE;  /* MOP server address unknown */
	ieee_zero(file.ps.mop.bootaddr);   /* MOP server address unknown */
	ieee_zero(file.ps.mop.respondaddr);
    }
    
    psion(stdio);
    tftp_okay = reg_invoke_file_read(file.proto, (filetype*)&file,
				     (uchar *)buff, &buffersize, TRUE,
				     net2flash_write_ok, (void *)&fa);
    psioff(stdio);
    
    if ((tftp_okay != ERROR_OK) && flash.f_verbose) {
	if (file.proto == FILE_ACCESS_TFTP || file.proto == FILE_ACCESS_RCP) {
	    printf(flash_net_err_msg, "from", name_addr2string(host));
	} else {
	    printf("\n%%MOP_TO_FLASH: copy failed.\n");
	}
    }
    
    if (ffp->ffh_len > 0) {
	/* We wrote something. Must sync the header. */
	ffp->ffh_checksum = ~ffp->ffh_checksum;  /* negate checksum */
	romaddr = flash.f_free_offset + sizeof(flash_fhdr);

        /* Seek to beginning of file data */
	dev_ioctl(dev, IOC_FLASH_SEEK, &romaddr);
        /*
         * Download was successful. Was the flash write good ?
         */ 
        if (tftp_okay == ERROR_OK) {
	    if (flash_verify(dev, ffp, (char *)buff, flash.f_verbose) != 0)
                tftp_okay = ERROR_WRITE;
        }

	if (tftp_okay != ERROR_OK)  /* Something went wrong. Delete file */
	    ffp->ffh_flags &= ~FFH_FLAGS_DELETED;
	dev_ioctl(dev, IOC_FLASH_SEEK, &flash.f_free_offset);
	dev_write(dev, (char *)ffp, sizeof(*ffp));
        /*
         * Bump up file count and update f_free_offset to point
         * to start of next file.
         */
	flash.f_filecnt++;
	flash.f_free_offset += ffp->ffh_len + sizeof(flash_fhdr);

        /*
         * If TFTP was OK and there's a duplicate file,
         * delete duplicate
         */
        if ((tftp_okay == ERROR_OK) && dup_ffp) {
            flash_vpp_set(dev, VPP_OFF);
            flash_delete_file(dev, locname);
        }
    }
    flash_vpp_set(dev, VPP_OFF);
    /*
     * Need to re-seek after turning vpp off for the envm.
     */
    dev_ioctl(dev, IOC_FLASH_SEEK, &flash.f_free_offset);
    return((tftp_okay == ERROR_OK) ? 0 : -1);
}

/* 
 * flash2tftp -- copy code image from flash mem to tftp server.
 * Called from copy_command().
 *
 * Assumes that we've seeked to the start of the file's data.
 */
int flash2tftp (ulong dev, addrtype *host, const char *locname,
		const char *remname, uchar *buff)
{
    int blknum, i;
    ipsocktype *soc;
    boolean result;
    ushort *wp;
    ushort cksum;
    flash_fhdr *fh;
    int nbytes;
    int rem, rlen;
    
    if (! (fh = flash_get_fhdr(dev, locname))) {
        if (flash.f_verbose)
	    printf("File '%s' not found in flash memory\n", locname);
	return(-1);
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
    cksum = 0;				/* initialize checksum accumulation */

    /*
     * reg_invoke_ip_tftp_WRQ(ip socket, source filename, data transfer mode,
     *                        boolean server : FALSE - display file transfer progress
     *                                         TRUE  - console silent transfer)
     */
    result = reg_invoke_ip_tftp_WRQ(soc, remname, "octet", FALSE);
    if (result == TRUE) {
	blknum = 1;
	rem = fh->ffh_len;
	while (rem > 0) {
	    rlen = min(rem, TFTP_MAXDATA);
	    if ((nbytes = dev_read(dev, (char *)buff, rlen) != rlen)) {
                if (flash.f_verbose)
  		    printf("\nError reading flash at offset %d (%x)\n",
		           fh->ffh_len - rem, fh->ffh_len - rem);
		reg_invoke_ip_close_socket(soc);
		return -1;
	    }
	    wp = (ushort *)buff;
	    rem -= rlen;
	    for (i = 0; i < (rlen / sizeof(ushort)); i++) {
		/* 512 byte block */
		cksum = add_to_cksum(*wp++, cksum); /* accumulate checksum */
	    }
	    result = reg_invoke_ip_tftp_sendblock(soc, blknum++, buff,
						  rlen, FALSE);
	    if (!result) {
		break;
	    }
	    flush();
	}
    }
    
    if (result) {
	cksum = ~cksum;
	if (cksum ^ fh->ffh_checksum) {	/* if checksum mismatch */
            if (flash.f_verbose) {
	        printf("\nchecksum failure reading flash: ");
	        printf("expected 0x%04x, got 0x%04x, diff 0x%04x\n",
		       fh->ffh_checksum, cksum, cksum ^ fh->ffh_checksum);
	    }
	    result = FALSE;
	}
    }
    reg_invoke_ip_close_socket(soc);
    if (result) {
        return 0;
    } else {
        return -1;
    }
}

/*************************************************************
 * flash2rcp -- copy code image from flash mem to remote unix host.
 * Called from copy_command().
 *************************************************************/

typedef struct flash2rcp_context_ {
    int       dev;
    ushort    cksum;
    uchar     *buffer;
} flash2rcp_context;

long flash2rcp_callback(uchar **destination, long bytes_read, void *arg)
{
    flash2rcp_context *c;

    if (psipending())
	return(0);

    c = (flash2rcp_context *) arg;

    *destination = c->buffer;

    bytes_read = min(bytes_read, TFTP_MAXDATA);
    bytes_read = dev_read(c->dev, *destination, bytes_read);
    if (bytes_read == -1)
	return(0);
    c->cksum = update_cksum(c->cksum, *destination, bytes_read);

    return (bytes_read);
}

void flash2rcp (ulong dev, addrtype *host, const char *locname,
		const char *remname)
{
    flash_fhdr *fh;
    flash2rcp_context context;

    if (!(fh = flash_get_fhdr(dev, locname))) {
        printf("File '%s' not found in flash memory\n", locname);
        return;
    }

    if (!(context.buffer = malloc(TFTP_MAXDATA))) {
        printf("\nMemory allocation error");
	return;
    }

    printf("\nWriting %s ", remname);
    flush();

    context.dev    = dev;
    context.cksum  = 0;

    psion(stdio);

    /*
     * reg_invoke_rcp_write(remote dest file name, remote host ip addr,
     *                      data buffer, number of bytes to send,
     *                      filetype to supply remote username for scope of operation,
     *                      call-back func, call-back func parameters)
     */
    if (reg_invoke_rcp_write(remname, host, context.buffer, fh->ffh_len,
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
boolean flash2rcp_remote (void *mumble, char *name)
{
    flash_fhdr *fh;
    flash2rcp_context context;
    int dev;
    int error;
    tcbtype *tcb;

    tcb = (tcbtype *)mumble;

    dev = flash_open();
    if (dev == 0)
        return FALSE;

    /*
     * Get latest info (user may have installed security jumper)
     */
    flash_read_fhdrs(dev);
    flash_query_roms(dev);

    fh = flash_get_fhdr(dev, name);
    if (!fh) {
        dev_close(dev);
        return FALSE;
    }

    context.buffer = malloc(TFTP_MAXDATA);
    if (!context.buffer) {
        dev_close(dev);
        return(FALSE);
    }

    context.dev   = dev;
    context.cksum = 0;

    error = reg_invoke_rcp_send(tcb, name, context.buffer, fh->ffh_len,
                              flash2rcp_callback, (void *) &context, FALSE);

    if (error == ERROR_OK) {
        context.cksum = ~context.cksum;
        if (context.cksum ^ fh->ffh_checksum)   /* if checksum mismatch */
	    error = ERROR_ACCESS;
    }

    free(context.buffer);
    dev_close(dev);
    return (error == ERROR_OK);
}

/*
 * write_buffer_to_flash
 * write a buffer to flash as a file
 */
boolean write_buffer_to_flash (void *tcb, const char *filename, uchar *buffer,
			       ulong buflen)
{
    flash_fhdr *ffp;
    int dev, rtn_code;
    ulong romaddr;
    struct flash_write_args fa;
    boolean rcp_okay;

    dev = flash_open();
    if (dev == 0) {
	/*
	 * kludgy way to return eror code, but use buffer.  can't
	 * write it so make use of it...
	 */
	strcpy(buffer, "Problems opening flash for writing\n");
	dev_close(dev);
	return(FALSE);
    }

    /*
     * Get latest info (user may have installed security jumper)
     */
    flash_read_fhdrs(dev);
    flash_query_roms(dev);

    if (!flash.f_vpp_works) {
	strcpy(buffer, "Flash is strapped secure\n");

	dev_close(dev);
	return(FALSE);
    }

    ffp = flash_get_fhdr(dev, filename);
    if (ffp) {
	strcpy(buffer, "File already exists\n");
	dev_close(dev);
	return(FALSE);
    }
    
    /*
     * Erase vs. Append vs. Overwrite
     */
    if (flash.f_filecnt >= FLASH_FILE_CNT) {
	strcpy(buffer, "Maximum file count exceeded\n");
	dev_close(dev);
	return(FALSE);
    } else if ((flash.f_size - flash.f_free_offset) < MIN_FILE_SIZE) {
	strcpy(buffer, "Flash is filled to capacity\n");
	dev_close(dev);
	return(FALSE);
    }
    
    /*
     * Get next available file header.
     */
    flash_vpp_set(dev, VPP_ON);
    romaddr = flash.f_free_offset + sizeof(flash_fhdr);
    dev_ioctl(dev, IOC_FLASH_SEEK, &romaddr);
    
    ffp = &flash.f_fhdrs[flash.f_filecnt & 0x0f];
    
    strcpy(ffp->ffh_name, filename);

    ffp->ffh_checksum = 0;
    ffp->ffh_magic = FLASH_FHDR_MAGIC;
    ffp->ffh_len = 0;
    ffp->ffh_flags = (short) -1;
    ffp->ffh_reserved = (long) -1;

    fa.ffp = ffp;
    fa.buf = buffer;
    fa.dev = dev;
    fa.verbose = FALSE;
    
    rtn_code = reg_invoke_rcp_receive(tcb, buffer, &buflen, FALSE,
				      net2flash_write_ok, (void *)&fa);

    rcp_okay = TRUE;  /* optimism */
    if (rtn_code != ERROR_OK) {
	strcpy(buffer, "Error in file transfer\n");
        rcp_okay = FALSE;
    }

    if (ffp->ffh_len > 0) {
	/* We wrote something. Must sync the header. */
	ffp->ffh_checksum = ~ffp->ffh_checksum;  /* negate checksum */
	romaddr = flash.f_free_offset + sizeof(flash_fhdr);

        /* Seek to beginning of file data */
	dev_ioctl(dev, IOC_FLASH_SEEK, &romaddr);
        /*
         * Download was successful. Was the flash write good ?
         */ 
        if (rcp_okay) {
	    if (flash_verify(dev, ffp, (char *) buffer, TRUE) != 0)
                rcp_okay = FALSE;
        }

	if (!rcp_okay)  /* Something went wrong. Delete file */
	    ffp->ffh_flags &= ~FFH_FLAGS_DELETED;
	dev_ioctl(dev, IOC_FLASH_SEEK, &flash.f_free_offset);
	dev_write(dev, (char *)ffp, sizeof(*ffp));
        /*
         * Bump up file count and update f_free_offset to point
         * to start of next file.
         */
	flash.f_filecnt++;
	flash.f_free_offset += ffp->ffh_len + sizeof(flash_fhdr);
    }
    flash_vpp_set(dev, VPP_OFF);
    /*
     * Need to re-seek after turning vpp off for the envm.
     */
    dev_ioctl(dev, IOC_FLASH_SEEK, &flash.f_free_offset);
    dev_close(dev);
    return(rcp_okay);
}

/***************************************************************************
 *
 *                      flash boot support
 *
 **************************************************************************/

/*
 * flash_read_file
 * move system image from flash to memory.
 */

int flash_read_file (const char *filename, uchar *bufp,
		     ulong *sizep, boolean netboot) 
{
    uchar  *bp;
    ushort *wp;
    ushort cksum;
    flash_fhdr *fh;
    int nbytes, rlen, rem, i, retries, dots;
    int dev;

    /* Block on open (in case env and stat update running) */
    if ((dev = dev_open(flash.f_dev, DEV_FLAGS_EXCL|DEV_FLAGS_QUIET)) < 0) {
	printf("\nCouldn't access flash memory\n");
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

    if (netboot) {
	printf("\nBooting %s from flash memory ", fh->ffh_name);
	flush();
    }

    if ((nbytes = fh->ffh_len) > *sizep) {
	printf(" [buffer overflow - %d/%d bytes]\n",
	       nbytes, *sizep);
	flush();
	dev_close(dev);
	return(ERROR_TOOBIG);
    }
    
    bp = bufp;
    rem = nbytes;
    dots = 0;
    do {
	rlen = (rem > TFTP_MAXDATA) ? TFTP_MAXDATA : rem;
	if ((i = dev_read(dev, (char *)bp, rlen)) != rlen) {
	    /*
	     * Seek again and retry the read.
	     */
	    dots = 0;
	    for (retries = 2; retries; retries--) {
		if (netboot) {
		    printc('E');
		    flush();
		}
		dev_ioctl(dev, IOC_FLASH_SEEK, &flash.f_cur);
		if ((i = dev_read(dev, (char *)bp, rlen)) == rlen)
		    break;
	    }
	    if (!retries) {
		printf(" [Read failed at offset 0x%x]\n", flash.f_cur);
		flush();
		dev_close(dev);
		return(ERROR_UNDEF);
	    }
	}
	rem -= rlen;
	bp += rlen;
	if ((++dots % 10) == 0) {
	    if (netboot) {
		printc('R');
		flush();
	    }
	}
    } while (rem > 0);
    dev_close(dev);
    
    /* accumulate checksum */
    for (i=nbytes/2, cksum=0, wp=(ushort *)bufp; i>0; i--)
	cksum = add_to_cksum(*wp++, cksum);
    cksum = ~cksum;
    if (cksum ^ fh->ffh_checksum) {
	printf(" [Checksum error:  expected %04x, got %04x]\n",
	       fh->ffh_checksum, cksum);
	flush();
	return(ERROR_UNDEF);
    }
    if (netboot) {
	printf(" [OK - %d/%d bytes]\n", nbytes, *sizep);
	flush();
	/*
	 * So we know what we booted...
	 */
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
    int dev;

    if (flash.f_filecnt <= 0)
	return(NULL);
    if ((dev = dev_open(flash.f_dev, DEV_FLAGS_EXCL|DEV_FLAGS_QUIET)) < 0)
	return(NULL);
    if (! (fh = flash_get_fhdr(dev, (char *)NULL))) {
	dev_close(dev);
	return(NULL);
    }
    dev_close(dev);
    return(fh->ffh_name);
}


#define TFTP_OPEN_FLAGS (DEV_FLAGS_EXCL | DEV_FLAGS_NDLY | DEV_FLAGS_QUIET)

/*
 * Called from tftp_server.c
 * Open flash device, seek to specified offset, and read a block of data.
 */
int flash_read_block (int offset, uchar *buff, int len) 
{
    int sts, retries;
    int dev;

    if (!flash.f_dev || !flash.f_size)
	return(-1);
    
    if ((dev = dev_open(flash.f_dev, TFTP_OPEN_FLAGS)) < 0)
	return(-1);

    if (dev_ioctl(dev, IOC_FLASH_SEEK, &offset) < 0) {
	dev_close(dev);
	return(-1);
    }

    if ((sts = dev_read(dev, (char *)buff, len)) != len) {
	for (retries = 2; retries; retries--) {
	    dev_ioctl(dev, IOC_FLASH_SEEK, &offset);
	    if ((sts = dev_read(dev, (char *)buff, len)) == len)
		break;
	}
	if (!retries) {
	    dev_close(dev);
	    return(-1);
	}
    }
    dev_close(dev);
    return(sts);
}

/*
 * Called from tftp_server.c
 * Return offset of start of file data (if file exists in flash).
 */
long flash_file_offset (const char *filename) 
{
    ulong i, addr;
    
    if (!flash.f_dev || !flash.f_size)
	return(-1);
    
    addr = sizeof(flash_header);
    for (i=0; i<flash.f_filecnt; i++) {
	addr +=  sizeof(flash_fhdr);
	if ((flash.f_fhdrs[i].ffh_flags & FFH_FLAGS_DELETED) &&
	    !strcasecmp(filename, flash.f_fhdrs[i].ffh_name))
	    return(addr);	    
	addr += flash.f_fhdrs[i].ffh_len;
    }
    return(-1);
}

/*
 * Called from tftp_server.c
 * Return checksum of selected filename
 */
ushort flash_file_cks (const char *filename)
{
    ulong i;
    
    if (!flash.f_dev || !flash.f_size)
	return((ushort) 0);
    
    for (i=0; i<flash.f_filecnt; i++) {
	if ((flash.f_fhdrs[i].ffh_flags & FFH_FLAGS_DELETED) &&
	    (!strcasecmp(filename, flash.f_fhdrs[i].ffh_name))) {
	    return(flash.f_fhdrs[i].ffh_checksum);
	}
    }
    return((ushort) 0);
}

int flash_delete_file (int dev, const char *filename)
{
    ulong i, addr;
    flash_fhdr *ffp = &flash.f_fhdrs[0];
    
    addr = sizeof(flash_header);
    for (i=0; i<flash.f_filecnt; i++, ffp++) {
	if ((ffp->ffh_flags & FFH_FLAGS_DELETED) &&
	    !strcasecmp(filename, ffp->ffh_name)) {
	    flash_vpp_set(dev, VPP_ON);
	    dev_ioctl(dev, IOC_FLASH_SEEK, &addr);
	    ffp->ffh_flags &= ~FFH_FLAGS_DELETED;
	    return(dev_write(dev, (char *)ffp, sizeof(*ffp)));
	}
	addr += flash.f_fhdrs[i].ffh_len + sizeof(flash_fhdr);
    }
    return(-1);
}

/*
 * Called from tftp_server.c
 * Return length of file (if file exists in flash).
 */
long flash_file_length (const char *filename)
{
    ulong i;
    
    if (!flash.f_dev || !flash.f_size)
	return(-1);
    
    for (i=0; i<flash.f_filecnt; i++) {
	if ((flash.f_fhdrs[i].ffh_flags & FFH_FLAGS_DELETED) &&
	    !strcasecmp(filename, flash.f_fhdrs[i].ffh_name))
	    return(flash.f_fhdrs[i].ffh_len);
    }
    return(-1);
}

    
/*
 * Go through same code path as "flash_to_mem"
 */
int flash_verify (int dev, flash_fhdr *ffp, char *bp, boolean verbose)
{
    static char *errid = "\nFlash verification";
    ushort *wp;
    ushort cksum;		/* temp to accumulate checksum */
    int rlen;
    ulong rem;
    int i, retries, dots;

    if (verbose)
	printf("Verifying via checksum...\n");

    rem = ffp->ffh_len;
    cksum = 0, dots = 0;
    do {
	rlen = (rem > TFTP_MAXDATA) ? TFTP_MAXDATA : rem;
	if ((i = dev_read(dev, bp, rlen)) != rlen) {
	    /*
	     * Seek again and retry the read.
	     */
	    dots = 0;
	    for (retries = 2; retries; retries--) {
		if (verbose) {
		    printc('.'); flush();
		}
		dev_ioctl(dev, IOC_FLASH_SEEK, &flash.f_cur);
		if ((i = dev_read(dev, bp, rlen)) == rlen)
		    break;
	    }
	    if (!retries) {
		if (verbose)
		    printf("Flash read failed at offset 0x%x\n",
			   flash.f_cur);
		return(-1);
	    }
	}
	rem -= i;
	if (verbose) {
	    if ((++dots % 10) == 1) {
		printc('V');
		flush();
	    }
	}
	for (i=(rlen+1)/sizeof(ushort), wp=(ushort *)bp; i>0; i--)
	    cksum = add_to_cksum(*wp++, cksum);

        /*
         * Dismiss process every 128K for 10ms to give others a chance
         * to run.
         */
        if ((dots % 256) == 0)
            flash_delay(10);   /* dismiss for 10ms to give others a chance */
    } while (rem > 0 && rlen > 0);

    cksum = ~cksum;
    if (cksum ^ ffp->ffh_checksum) {
	if (verbose)
	    printf("\n%s checksum error.  Expected %04x, got %04x on length %ld\n",
		   errid, ffp->ffh_checksum, cksum, ffp->ffh_len);
	return(-1);
    }
    if (verbose)
	printf("\n%s successful.  Length = %ld, checksum = 0x%04X\n",
	       errid, ffp->ffh_len, ffp->ffh_checksum);
    return(0);
}


/***************************************************************************
 *
 *                      Misc. Flash Routines
 *
 **************************************************************************/


/*
 * Probe device for presense of flash.
 * We do this for the MC+ thru MCI or ENV by issuing
 * VPP on/off commands.
 */
boolean flash_probe (ulong dev)
{
    ulong on, off;

    on = VPP_ON;
    off = VPP_OFF;
    
    if (dev > DEV_NUL && dev < DEV_CNT) {
	if (dev_ioctl(dev, IOC_FLASH_VPP, &off) != 0 ||
	    dev_ioctl(dev, IOC_FLASH_VPP, &on ) != 0 ||
	    dev_ioctl(dev, IOC_FLASH_VPP, &off) != 0) {
	    return(FALSE);
	}
	flash.f_dev = dev;
	flash.f_devname = devtable[dev].dev_name;
	flash.f_card = "MC+ card";
	flash.f_ctlr = devtable[dev].controller;
	flash.f_banks = 2;
	flash.f_maxseek = MCPLUS_MAX_SIZE;
	return(TRUE);
    }
    
    /*
     * Put in here some other code for non-MC+ flash...
     */
    return(FALSE);
}

int flash_erase_all (ulong dev)
{
    ulong off, i;

    if (!flash.f_vpp_works)
	return(-1);

    if (!flash_vpp_set(dev, VPP_ON))
	return(-1);
    
    psion(stdio);
    if (flash.f_verbose) {
        printf("\nClearing and initializing flash memory (please wait) ");
        flush();
        process_sleep_for(2000); /* Allow time for escape */
    }
    
    for (off=0; off<flash.f_size; off+=flash.f_chipsize*flash.f_banks) {
        if (flash.f_verbose) {
	    while (getc(stdio) != -1)
	        ;
	}
	if (psipending() ||
	    dev_ioctl(dev, IOC_FLASH_SEEK, (uchar *)&off) < 0 ||
	    dev_ioctl(dev, IOC_FLASH_ZERO, NULL) < 0)
	    break;
        if (flash.f_verbose) {
	    printc('#');
	    flush();
	}

	if (off == 0) {
	    memset(&flash.f_fhdrs, 0, FLASH_FILE_CNT * sizeof(flash_fhdr));
	    flash.f_filecnt = 0;
	}
	
        if (flash.f_verbose) {
	    while (getc(stdio) != -1)
	        ;
	}
	if (psipending() ||
	    dev_ioctl(dev, IOC_FLASH_SEEK, (uchar *)&off) < 0 ||
	    dev_ioctl(dev, IOC_FLASH_ERASE, NULL) < 0)
	    break;
        if (flash.f_verbose) {
	    printc('#');
	    flush();
	}
	dev_ioctl(dev, IOC_FLASH_SEEK, (uchar *)&off);
	dev_ioctl(dev, IOC_FLASH_RESET, NULL);
    }
    
    psioff(stdio);
    if (flash.f_verbose)
        printc('\n');
    if (off < flash.f_size) {
	dev_ioctl(dev, IOC_FLASH_SEEK, (uchar *)&off);
	dev_ioctl(dev, IOC_FLASH_RESET, NULL);
	flash_vpp_set(dev, VPP_OFF);
	return(-1);
    }
    off = 0;
    flash.f_hdr.fh_magic = FLASH_HDR_MAGIC;
    for (i=0; i<FLHDR_RESERVED; i++)
	flash.f_hdr.fh_reserved[i] = -1;
    dev_ioctl(dev, IOC_FLASH_SEEK, (uchar *)&off);
    dev_write(dev, (char *)&flash.f_hdr, sizeof(flash_header));
    flash.f_free_offset = sizeof(flash_header);
    flash_vpp_set(dev, VPP_OFF);
    return(0);
}

/*
 * flash_query_roms
 * Read the manufacturer and device codes from each device.
 * Compute the total amount of ROM.
 * This routine must use spin wait delays so that
 * it can be called from the scheduler context.
 */
boolean flash_query_roms (int dev)
{
    uchar buf[MAX_FLASH_BANKS  * 2];
    ushort devs[MAX_FLASH_BANKS];
    int i, j, banks, idx;
    uint addr, missing;

    addr = missing = 0;
    
    /*
     * Assert VPP:
     */
    flash_vpp_set(dev, VPP_ON);
    
    banks = flash.f_banks;

    for (i = 0; !missing && i < MAX_ROMS; i+=banks) {
	
	dev_ioctl(dev, IOC_FLASH_SEEK, &addr);

	/*
	 * Issue two consecutive reset commands.
	 */
	dev_ioctl(dev, IOC_FLASH_RESET, NULL);
        (void) (*usec_delay)(6);
	dev_ioctl(dev, IOC_FLASH_RESET, NULL);
        (void) (*usec_delay)(6);

	/*
	 * Now issue CODE ID command.
	 */
	dev_ioctl(dev, IOC_FLASH_INFO, NULL);
        (void) (*usec_delay)(6);

	/*
	 * Read Manufacturer and Device codes from chips
	 */
	dev_read(dev, (char *)buf, flash.f_banks * 2);

	/*
	 * Restore devices for reading.
	 */
	dev_ioctl(dev, IOC_FLASH_READ, NULL);
	
	/* Get Manufacturer code for bank */
	for (j=0; j<banks; j++)
	  devs[j] = buf[j] << 8;
	    
	/* Get Device code for bank, and OR it w/ Manufacturer ID */
	for (j=0; j<banks; j++)
	  devs[j] |= buf[j + banks];

	/* Check for chip size compatibility */
	for (j=0; j<banks; j++) {
	    if ((idx = flash_info_index(devs[j])) < 0) {
		missing++;
		break;
	    }
	    if (i == 0 && j == 0) {
		flash.f_vpp_works = TRUE;
		flash.f_chipsize = flash_info_tbl[idx].device_size;
		flash.f_chipcount = 0;
		flash.f_size = 0;
	    } else if (flash.f_chipsize != flash_info_tbl[idx].device_size) {
		printf("MC+: ROM %d, %s, ", i+j, flash_sockets[i+j]);
		printf("mismatched ROM sizes: 0x%x != 0x%x, manu/dev = 0x%x\n",
		       flash_info_tbl[idx], flash.f_chipsize, devs[j]);
		flash.f_size = 0;
		break;
	    }
	    /*
	     * Set rom id in header and verify that they match what we read
	     * earlier from the flash header.  If they don't match
	     * exactly, then we need to erase and write out a new header.
	     */
	    if (flash.f_hdr.fh_rom_id[i+j] != devs[j]) {
		flash.f_hdr.fh_rom_id[i+j] = devs[j];
		flash.f_erase_needed = TRUE;
	    }
	    flash.f_info_idx[i+j] = idx;
	    flash.f_size += flash.f_chipsize;
	    flash.f_chipcount++;
	}
    }

    /*
     * De-assert VPP
     */
    flash_vpp_set(dev, VPP_OFF);
    
    flash.f_maxseek = flash.f_size - banks;

    return(flash.f_size && flash.f_vpp_works);
}

/* * For a given manufacturer/device code, return the index of
 * the flash rom information table for that device.
 */
int flash_info_index (ushort code)
{
    int i;
    
    for (i = 0; i < MAX_ROM_MFGS; i++)
	if (flash_info_tbl[i].manu_dev_code == code)
	    return(i);
    return(-1);
}

/*
 * Read main flash header struct from the front of flash memory,
 * then read all file headers as well.
 */
void flash_read_fhdrs (int dev)
{
    ulong addr;
    int i, idx;

    addr = 0;
    flash.f_size = 0;
    flash.f_filecnt = 0;
    flash.f_chipcount = 0;
    flash.f_chipsize = 0;
    flash.f_vpp_on = FALSE;
    flash.f_vpp_works = FALSE;
    flash.f_verbose = FALSE;
    
    dev_ioctl(dev, IOC_FLASH_SEEK, &addr);
    dev_read(dev, (char *)&flash.f_hdr, sizeof(flash_header));
    
    if (flash.f_hdr.fh_magic == FLASH_HDR_MAGIC) {
	for (i=0; i<MAX_ROMS; i++) {
	    if ((idx = flash_info_index(flash.f_hdr.fh_rom_id[i])) < 0)
		/* Out of roms */
		break;
	    flash.f_chipcount++;
	    /* SHOULD always be the same. */
	    if (!flash.f_chipsize)
		flash.f_chipsize = flash_info_tbl[idx].device_size;
	    else if (flash.f_chipsize != flash_info_tbl[idx].device_size) {
		flash.f_chipsize = 0;
		flash.f_size = 0;
		flash.f_hdr.fh_magic = 0; /* Now must erase! */
		break;
	    }
	    flash.f_info_idx[i] = idx;
	    flash.f_size += flash.f_chipsize;
	}
    }

    flash.f_free_offset = sizeof(flash_header);
    
    for (i=0; flash.f_size && i<FLASH_FILE_CNT; i++) {
	dev_ioctl(dev, IOC_FLASH_SEEK, &flash.f_free_offset);
	dev_read(dev, (char *)&flash.f_fhdrs[i], sizeof(flash_fhdr));
	if (flash.f_fhdrs[i].ffh_magic != FLASH_FHDR_MAGIC)
	    break;
	flash.f_free_offset += sizeof(flash_fhdr) + flash.f_fhdrs[i].ffh_len;
	flash.f_filecnt++;
    }
    flash.f_erase_needed = (flash.f_size == 0);
    return;
}

/*
 * Search through flash file table for file name match
 * and seek to start of the file.
 * If filename is null, return first non-deleted file header.
 * Returns pointer to flash file header of matching file name,
 * or NULL if not found.
 */
flash_fhdr *flash_get_fhdr (ulong dev, const char *filename)
{
    ulong i, addr, any;

    if (!filename || !*filename)
	any = 1;
    else
	any = 0;
    
    addr = sizeof(flash_header);
    for (i=0; i<flash.f_filecnt; i++) {
	addr +=  sizeof(flash_fhdr);
	if ((flash.f_fhdrs[i].ffh_flags & FFH_FLAGS_DELETED) &&
	    (any || !strcasecmp(filename, flash.f_fhdrs[i].ffh_name)))
	    break;
	addr += flash.f_fhdrs[i].ffh_len;
    }

    if (i == flash.f_filecnt)
	return(flash_fhdr *) NULL;

    dev_ioctl(dev, IOC_FLASH_SEEK, (char *) &addr);

    return(&flash.f_fhdrs[i]);
}

/*
 * Turn VPP ON or OFF (but only if it changes), and delay
 * the appropriate time for ramp up or ramp down (spin delay
 * during system init, disms during normal runtime).
 */
boolean flash_vpp_set (ulong dev, int on)
{
    int millis;
    
    if (on) {
	if (flash.f_vpp_on)
	    return(TRUE);
	millis = 100;         /* VPP ramp up time */
    } else {
	if (!flash.f_vpp_on)
	    return(TRUE);
	millis = 10;          /* VPP ramp down time */
    }

    if (dev_ioctl(dev, IOC_FLASH_VPP, &on) != 0) {
	if (flash.f_verbose)
	    printf("Unable to turn programming voltage %s\n",
		   on ? "on" : "off");
	return(FALSE);
    }

    flash_delay(millis);
    
    return(TRUE);
}

void flash_delay (ulong milliseconds)
{
    if (flash.f_proc_level)
	process_sleep_for(milliseconds);
    else
	DELAY(milliseconds);
}

/*
 * Common flash open routine (otherwise duplicated a lot)
 */
int flash_open (void)
{
    int dev, offset;

    offset = 0;

    if ((dev = dev_open(flash.f_dev, FLASH_OPEN)) < 0) {
	printf("\nFlash device open error (%d)\n", flash.f_dev);
	return(0);
    }
    if (dev_ioctl(dev, IOC_FLASH_SEEK, &offset) != DEV_OK) {
	printf("\nCouldn't address flash location 0\n");
	dev_close(dev);
	return(0);
    }
    return(dev);
}

/* 
 * add_to_cksum(new_word, old_cksum);
 * Accumulates a checksum by adding the new word the old checksum, with
 * end-around carry.  
 * Returns the new checksum value.
 */
ushort add_to_cksum (ushort new_word, ushort old_cksum)
{
   u_long work;			/* work value for end-around add */
 
   work = old_cksum + new_word;	/* add new word to checksum */
    if (work & ~0xFFFF) 
    	work = ++work & 0xFFFF;		/* end-around carry */   
    return(work);
}
/*
 * update_cksum(ushort cksum, uchar *wp, ulong i, char v)
 * cksum a block, adding it to the old checksum.
 * the char v will print if provided to show Checksum
 * progress.
 */
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

/*
 * 68020 (CSC/3) usecd<elay routine
 * MIN: 6us
 * MAX: 10us
 * This routine is NOT GENERIC in that is only supports 6 & 10us calls.
 * If given a value (0->6), it returns in 6us.  If given any (value>6),
 * it returns in 10us.  This was tuned specifically with the delay
 * values needed for flash programming.  The 8254 timer IS NOT USED...
 * this is because it appeared to be flakey on the CSC/3 and didn't
 * return consistent data.  If someone has the inclination (need), this
 * routine can be made more general purpose (like the 68040 routine).
 * The assumumption is made that the instruction cache is enabled and has
 * been seeded with a dummy call to this routine prior to use.
 */
void usecdelay_68020 (uint n)
{
    WASTETIME(4);
    if (n <= 6) {
	return;
    }
    WASTETIME(8);
}

/*
 * 68040 (CSC/4) usecdelay routine
 * MIN: 2us
 * This is truely a generic usec delay routine in that it accepts any
 * delay value from 2us and up to "max unsigned short".  This routine
 * should be accurate within 1us for any value you seed it with.  This
 * is assuming the data and instruction cache's are enabled and they've 
 * been primed prior to your official call.  Currently usecdelay_68020 
 * and usecdelay_68040 are enabled in the function set_usec_delay()  
 * which references cpu_type. If your call requires the delay be very
 * accurate, all interrupts must be disabled prior to the call. Only
 * values > 10us use the 8254 timer... other's between 2 & 10us were
 * just tuned individually to support small values.  Please study the
 * below routine carefully before making any changes...(and then, ONLY
 * with an ocilloscope.
 */
void usecdelay_68040 (uint n)
{
    ushort tmp;

    if (n <= 10) {
        if (n <= 2) return;
        WASTETIME(0);
        if (n <= 3) return;
        WASTETIME(0);
        if (n <= 4) return;
        WASTETIME(0);
        if (n <= 5) return;
        WASTETIME(0);
        if (n <= 6) return;
        WASTETIME(0);
        if (n <= 7) return;
        WASTETIME(0);
        if (n <= 8) return;
        WASTETIME(0);
        if (n <= 9) return;
        WASTETIME(0);
        if (n <= 10) return;
    }
    n -= 10;  /* subtract some overhead amount */
    while (n) {
        tmp = n & 0x7fff;
        if (tmp == 0)
	    tmp = 0x8000;
        n -= tmp;
        tmp += 0x4000;  /* pad this amount so timer never times out */
        ctaddr->control = SEL_COUNTER1 | RW_LSBMSB | CT_MODE4;
        ctaddr->counter1 = tmp;  /* program lsb */
        ctaddr->counter1 = tmp >> 8;  /* program msb */
        WASTETIME(2);  /* give timer a moment to load */
        while (tmp > 0x4000) {  /* break if below our pad value */
            ctaddr->control = SEL_COUNTER1 | CT_MODE4;  /* latch counter 1 */
            WASTETIME(2);  /* give timer a moment to latch count */
            tmp = (ctaddr->counter1 & 0xff);  /* get lsb */
            tmp |= (ctaddr->counter1 << 8);  /* get msb */
        }
    }
    ctaddr->control = SEL_COUNTER1;  /* stop counting */
}

/*
 *  Setup the appropriate usec_delay routine based on cpu_type
 */
int set_usec_delay (void)
{
    ulong family;

    family = cpu_type;
    switch (family) {
      case CPU_CSC3:
        usec_delay = usecdelay_68020;
        return(TRUE);
        break;
      case CPU_CSC4:
        usec_delay = usecdelay_68040;
        return(TRUE);
        break;
      default:
        usec_delay = NULL; /* cpu_type is not supported yet */
        return(FALSE);
    }
}

/*
 * erase_command()
 * erase flash eproms
 */
void erase_command (void)
{
    char buff[16];
    int  dev;
    int sts;
    
    if ((dev = flash_open()) == 0)
      return;
    
    printf("Do you really want to erase your flash eeproms? ");
    rdtty(buff, 16);
    if ((buff[0] & 0xdf) != 'Y') {
	printf("erase aborted.\n");
	dev_close(dev);
	return;
    }
    sts = flash_erase_all(dev);
    dev_close(dev);
    return;
}

/***************************************************************************
 *
 *                      Nvram and Flash TEST Routines
 *
 **************************************************************************/

#ifdef NVRAM_DRIVER

/*
 * mc_nvram_test
 * Test the MC or MC+ NVRAM. Copy the configuration to RAM before beginning
 * and restore when done. Warn the user if we can't get enough RAM to do this.
 */

void mc_nvram_test (void)
{
    ushort expected, actual;
    int errors, times, i;
    nvtype *nv_copy;		/* Pointer to base of NV memory */
    uint addr, data;
    char buffer[MC_MAXIN];
    boolean parsed;

    parsed = FALSE;
    
    printf("\nTest NVRAM on MC card [y/n] ? ");
    if (!doit())
	return;
    
    nv_writeenable(NULL);	/* Make nv_done restore NVRAM */
    nv_copy =  nv_getptr(); /* NVRAM contents to RAM @ nv_copy */
    if (!nv_copy)
	return;
    
    errors = 0;
    
    times = MC_DEF_TIMES;
    printf("Number of times to iterate [%d]? ", times);
    rdtty(buffer, MC_MAXIN);
    if (strlen(buffer)) {
	times = parse_unsigned(buffer, &parsed); /* ??? */
    }
    if (!times)
	times = MC_DEF_TIMES;
    
    /*
     * Allow any keyboard input to abort the test:
     */
    for (i = 0; ((i < times) && (!serial_inputpending(stdio))); i++) {
	printf(".");
	
	/*
	 * Try writing short 0x0 to all of NVRAM:
	 */
	expected = 0x0;
	for (addr = 0; addr < (uint)nvsize; addr++)
	    mc_write_short(expected, addr);
	
	for (addr = 0; addr < (uint)nvsize; addr++) {
	    actual = mc_read_short(addr);
	    if (actual != expected) {
		errors++;
		printf("Error in NVRAM at 0x%06x - expected 0x%04x, got 0x%04x\n", 
		       addr, expected, actual);
	    }
	}
	
	/*
	 * Try writing short 0xFFFF to all of NVRAM:
	 */
	expected = 0xFFFF;
	for (addr = 0; addr < (uint)nvsize; addr++)
	    mc_write_short(expected, addr);
	
	for (addr = 0; addr < (uint)nvsize; addr++) {
	    actual = mc_read_short(addr);
	    if (actual != expected) {
		errors++;
		printf("Error in NVRAM at 0x%06x - expected 0x%04x, got 0x%04x\n", 
		       addr, expected, actual);
	    }
	}
	
	/*
	 * Try writing short 0xA55A to all of NVRAM:
	 */
	expected = 0xA55A;
	for (addr = 0; addr < (uint)nvsize; addr++)
	    mc_write_short(expected, addr);
	
	for (addr = 0; addr < (uint)nvsize; addr++) {
	    actual = mc_read_short(addr);
	    if (actual != expected) {
		errors++;
		printf("Error in NVRAM at 0x%06x - expected 0x%04x, got 0x%04x\n", 
		       addr, expected, actual);
	    }
	}
	
	/*
	 * Try writing the long word memory address to each NVRAM location:
	 */
	for (addr = 0; addr < (uint)nvsize; addr += 2)
	    mc_write_long(addr, addr);
	
	for (addr = 0; addr < (uint)nvsize; addr += 2) {
	    data = mc_read_long(addr);
	    if (data != addr) {
		errors++;
		printf("Error in NVRAM at 0x%06x - expected 0x%08x, got 0x%08x\n", 
		       addr, addr, data);
	    }
	}
	flash_delay(ONESEC/10);		/* Let scheduler run */
    }
    
    if (errors)
	printf("\nEncountered %d errors during %d tests.\n", errors, times);
    else
	printf("\nNo errors encountered in %d tests.\n", times);
    
    nv_done(nv_copy);		/* Restore NVRAM contents */
}

/*
 * mc_flash_test
 * Test the flash EPROMs. Do read/write test. Let user know stats on each
 * part.
 */

void mc_flash_test (void)
{
    uint i, j;
    char buffa[16];
    
    
    if (!flash_size) {
	printf("Flash not available\n");
	return;
    }
    
    printf("The MC+ flash eeproms can be reprogrammed about 10, 000 times.\n");
    printf("These tests will use three of these cycles.  OK to proceed? ");
    
    rdtty(buffa, 16);
    if ((buffa[0] & 0xdf) != 'Y') {
	printf("flash eeprom test aborted.\n");
	return;
    }
    
    printf("\nerasing...  ");
    if (!mc_erase_roms()) {
	printf("flash eeproms WOULD NOT ERASE!\n");
	return;
    }
    
    printf("data=address test...  writing...  ");
    mc_assert_vpp();
    
    for (i = MC_ROM_ADDRESS ; i < MC_ROM_ADDRESS + flash_size/2; i = i+2) {
	mc_program_rom((ushort)(i >> 16), i);
	mc_program_rom((ushort)(i & 0xffff), i+1);
    }
    mc_deassert_vpp();
    
    printf("verifying...  ");
    
    for (i = MC_ROM_ADDRESS ; i < MC_ROM_ADDRESS + flash_size/2; i = i+2) {
	
	if ((i & 0xffff) != mc_read_short(i+1)) {
	    printf("\nverification FAILURE!  %8x: %4x %4x\n", 
		   i, mc_read_short(i), mc_read_short(i+1));
	    badpart(i+1, i & 0xffff);
	    return;
	}
	if ((i >> 16) != mc_read_short(i)) {
	    printf("\nverification FAILURE!  %8x: %4x %4x\n", 
		   i, mc_read_short(i), mc_read_short(i+1));
	    badpart(i, i >> 16);
	    return;
	}
    }
    printf("passed.\nerasing...  ");
    
    if (!mc_erase_roms()) {
	printf("flash eeproms WOULD NOT ERASE!\n");
	return;
    }
    
    printf("AAAA test...  ");
    mc_assert_vpp();
    for (i = MC_ROM_ADDRESS ; i < MC_ROM_ADDRESS + flash_size/2; i++)
	mc_program_rom(0xAAAA, i);
    mc_deassert_vpp();
    
    for (i = MC_ROM_ADDRESS ; i < MC_ROM_ADDRESS + flash_size/2; i++)
	if ((j = mc_read_short(i)) != 0xAAAA) {
	    printf("\nverification FAILURE!  %8x: %4x\n", i, j);
	    badpart(i, 0xAAAA);
	    return;
	}
    
    printf("passed.\nerasing...  ");
    if (!mc_erase_roms()) {
	printf("flash eeproms WOULD NOT ERASE!\n");
	return;
    }
    
    printf("5555 test...  ");
    mc_assert_vpp();
    for (i = MC_ROM_ADDRESS ; i < MC_ROM_ADDRESS + flash_size/2; i++)
	mc_program_rom(0x5555, i);
    mc_deassert_vpp();
    
    for (i = MC_ROM_ADDRESS ; i < MC_ROM_ADDRESS + flash_size/2; i++)
	if ((j = mc_read_short(i)) != 0x5555) {
	    printf("\nverification FAILURE!  %8x: %4x\n", i, j);
	    badpart(i, 0x5555);
	    return;
	}
    
    printf("passed.\n");
}



/*
 * mc_read_test
 * Simple test that reads MC card address space (for board debugging):
 */

void mc_read_test (void)
{
    int len, times, i;
    uint start;
    char buffer[MC_MAXIN];
    boolean parsed;
    
    start = MC_DEF_START;
    printf("Starting address on %s card [%d]? ", flash.f_card, start);
    rdtty(buffer, MC_MAXIN);
    if (strlen(buffer)) {
	start = parse_unsigned(buffer, &parsed); /* ??? */
    }
    
    len = MC_DEF_LEN;
    printf("\nLength of address space to read [0x%x]? ", len);
    rdtty(buffer, MC_MAXIN);
    if (strlen(buffer)) {
	len = parse_unsigned(buffer, &parsed); /* ??? */
    }
    if (!len)
	len = MC_DEF_LEN;
    
    times = MC_DEF_TIMES;
    /*    printf("Number of times to iterate [%d]? ", times);
	  rdtty(buffer, MC_MAXIN);
	  if (strlen(buffer)) {
	  times = parse_unsigned(buffer, &parsed);
	  }
	  if (!times)
	  times = MC_DEF_TIMES;*/
    times = 1;
    
    printf("\nReading %d byte region starting at 0x%x (type char to abort)\n", 
	   len, start, times);
    
    while (times && (!serial_inputpending(stdio))) {
	for (i=start; i < start+len; i++) {
	    ushort kuk = mc_read_short(i);
	    if (!(i % 16))
		printf("\n%x: ", i);
	    printf("%x ", kuk);
	}
	printf("\n");
	
	/* putchar('.'); */
	mc_read(start, NULL, len);
	times--;
    }
    
    printf("\nDone\n");
}

/*
 * mc_write_test
 * Simple test that writes to an MC card address (for board debugging):
 */

void mc_write_test (void)
{
    uint start;
    int times;
    char buffer[MC_MAXIN];
    ushort data;
    boolean parsed;
    
    start = MC_DEF_START;
    printf("Address to write on %s card [%d]? ", flash.f_card, start);
    rdtty(buffer, MC_MAXIN);
    if (strlen(buffer)) {
	start = parse_unsigned(buffer, &parsed); /* ??? */
    }
    
    data = MC_DEF_DATA;
    printf("Data to write [%d]? ", data);
    rdtty(buffer, MC_MAXIN);
    if (strlen(buffer)) {
	data = parse_unsigned(buffer, &parsed);	/* ??? */
    }
    
    times = MC_DEF_TIMES;
    printf("Number of times to iterate [%d]? ", times);
    rdtty(buffer, MC_MAXIN);
    if (strlen(buffer)) {
	times = parse_unsigned(buffer, &parsed); /* ??? */
    }
    if (!times)
	times = MC_DEF_TIMES;
    
    
    printf("\nWriting %04x to address 0x%06x %d times (type char to abort)\n", 
	   data, start, times);
    
    while (times && (!serial_inputpending(stdio))) {
	/*	mc_write_short(data, start);*/
	mc_assert_vpp(0);
	mc_program_rom(data, start);
	mc_deassert_vpp(0);
	times--;
    }
    
    printf("\nDone\n");
}


/*
 * mc_vpp_test
 * Assert VPP on MC+ card so it can be measured:
 */

void mc_vpp_test (void)
{
    char junk;
    int  fl;
    int  on, off;

    on = VPP_ON; off = VPP_OFF;
    
    if ((fl = flash_open()) <= 0)
	return;
    
    printf("Toggling VPP on and off. Type <Return> to end:");
    while (!serial_inputpending(stdio)) {
	dev_ioctl(fl, IOC_FLASH_VPP, &on);
	dev_ioctl(fl, IOC_FLASH_VPP, &off);
	flash_delay(ONESEC/10);	/* Wait at least 100 ms */
    }
    while (serial_inputpending(stdio))
	junk = getc(stdio);
    printf("\nVPP toggle test done\n");
    
    printf("\nType <Return> to begin VPP level test:");
    (void)rdtty(&junk, 1);
    dev_ioctl(fl, IOC_FLASH_VPP, &on);
    printf("VPP asserted. Type <Return> to deassert VPP:");
    (void)rdtty(&junk, 1);
    dev_ioctl(fl, IOC_FLASH_VPP, &off);
    printf("VPP deasserted. VPP level test done\n");
}


/* Not until someone wants to take ownership of
 * this stuff.
 */

/*
 * tftp2nvram --  copy a configuration image from tftp server to NVRAM.
 */
void tftp2nvram (void)
{
    addrtype respondent;
    int i, 
    parsed, 
    ret = 0;
    long buffersize;
    char *buff;
    char fn[FLASH_NAME_LEN];
    nvtype *nvp;
    int nvhandle;
    int nbytes;
    filetype file;
    
    if (!nvsize) {
	printf("\nno NVRAM present.  command aborted.");
	return;
    } 
    
    /*
     * Get or default boot hostname
     */
    file.filename = NULL;
    file.proto = FILE_ACCESS_TFTP;
    file.ps.tuba.boothost.type = ADDR_IP;
    file.ps.tuba.boothost.ip_addr = -1L;
    file.ps.tuba.username = NULL;
    file.ps.tuba.display = TRUE);

    printf("Address of remote host [%i]? ", file.ps.tuba.boothost.ip_addr);
    if (parsed = rdtty(fn, FLASH_NAME_LEN)) {
	if (!null(fn)) { 
	    nametype *ptr;
	    parsed = parse_ip_address(fn, &file.ps.tuba.boothost.ip_addr);
	    ptr = (nametype *) name_lookup(fn, &i, NAM_IP);
	    if (ptr) {
		bcopy(&ptr->v.ip.addr[0].addr, &file.ps.tuba.boothost,
		      sizeof(addrtype));
		parsed = TRUE;
	    }
	}
    } 
    if (!parsed) {
	printf("\n?Bad IP address or host name");
	return;
    } 
    
    /*
     * Get configuration filename
     */
    printf("\nName of TFTP configuration file to copy into NVRAM? ");
    parsed = rdtty(fn, 63);
    if (!parsed) {
	printf("\n?Bad file name");
	return;
    }
    file.filename = fn;
    
    /*
     *  Have user confirm the configuration request 
     */
    printf("\ncopy %s from %i into NVRAM?  ", file.filename,
	   file.ps.tuba.boothost.ip_addr);
    if (!confirm(NULL)) {
	printf("aborted.\n");
	return;
    }
    
    buffersize = nvsize;		/* set buffer size */
    buff = malloc(nvsize);      /* allocate a Rx buffer */
    if (!buff) {			/* insufficient memory available */
	printf("Insufficient memory for NVRAM buffer.\n");
	return;
    }
    ret = reg_invoke_file_read(file.proto,
			  (filetype *)&file, (uchar *)buff, &buffersize,
			  TRUE, NULL, NULL);

    /* 
      transfer a good NVRAM configuration image from the Rx 
      buffer to the NVRAM itself 
      */
    nvp = (nvtype *)buff;             /* look at received buffer */
    
    if (nvp->magic != NVMAGIC) {      /* does magic number match */
	printf("\n%s not a config file (magic = 0x%x): NVRAM unchanged.",
	       fn, nvp->magic);
	free(buff);
	return;
    }
    
    if (ret == ERROR_OK) {		  /* file read successfully */
	printf("\n%s received successfully, %d bytes.", fn, nvp->textsize + 
	       sizeof(nvtype));	
	
	if ((nvhandle = dev_open(NvramDev, DEV_FLAGS_EXCL)) < 0) {
	    printf("\nerror %d opening NVRAM.", nvhandle);
	    free(buff);
	    return;  
	}
	if ((nbytes = dev_write(nvhandle, buff, nvsize)) != nvsize) {
	    printf("\nerror writing NVRAM; expected %d, actual %d", 
		   nvsize, nbytes);
	}
	else {
	    printf("\n%d bytes written to NVRAM.", nvsize);
	}
	dev_close(nvhandle);
    }
    else {
	printf("\nfile transfer failed. NVRAM unchanged."); 
    }
    
    free(buff);
    return;
}


/* 
 * nvram2tftp -- copy code image from NVRAM to tftp server.
 */
void nvram2tftp (void)
{
    char *buff = malloc(TFTP_MAXDATA);  /* when is this freed? */
    int ramp, i, parsed;
    file_access_tuba tuba;
    
    ramp = MC_NVRAM_ADDRESS;
   
    tuba.boothost.type = IP_ADDR;
    tuba.boothost.ip_addr = -1L;

    printf("Address of remote host [%s]? ", name_addr2string(&tuba.boothost));
    if (parsed = rdtty(buff, TFTP_MAXDATA)) {
	if (!null(buff)) { 
	    nametype *ptr;
	    parsed = parse_ip_address(buff, &tuba.boothost);
	    ptr = (nametype *) name_lookup(buff, &i, NAM_IP);
	    if (ptr) {
		bcopy(&ptr->v.ip.adr[0].addr, &tuba.boothost,
		    sizeof(addrtype));
	        parsed = TRUE;
	    }
	}
    } 
    if (tftp_wnvram(tuba.boothost.ip_addr))
	printf("\nsuccessful tftp write.");
    else
	printf("\ntftp write error.");
}


/*
 * tftp_wnvram(host);
 * The high level access for writing NVRAM contents to some host using TFTP.
 * Returns TRUE if file successfully xferred, FALSE if some error.
 */
boolean tftp_wnvram (ipaddrtype host)
{
    boolean result;
    char fn[FLASH_NAME_LEN];
    nvtype *nvp;
    int nvhandle;
    int nbytes;
    filetype file;
    
    if (!nvsize) {               /* no NVRAM present */
	printf("\nno NVRAM present.  command aborted.");
	return(FALSE);
    } 
    
    printf("filename to write on tftp host? ");
    rdtty(fn, FLASH_NAME_LEN);
    if (!strlen(fn)) {	 /* no filename, no transfer */
	printf("\ncommand aborted.");
	return(FALSE);
    }
    
    flush();
    
    if ((nvhandle = dev_open(NvramDev, DEV_FLAGS_EXCL)) < 0) {
	printf("\ntftp_wnvram: error %d opening NVRAM", nvhandle);
        return(FALSE);
    }
    
    nvp = malloc(nvsize);
    if ((nbytes = dev_read(nvhandle, (char *)nvp, nvsize)) != nvsize) {
	printf("\nerror reading NVRAM: returned %d, expected %d", 
	       nbytes, nvsize);
	dev_close(nvhandle);
	return(FALSE);
    }
    dev_close(nvhandle);          /* close the NVRAM driver */

    file.proto = FILE_ACCESS_TFTP;
    file.name = fn;			
    ip_copy_address(host, &file.ps.tuba.boothost);
    result = reg_invoke_file_write(file.proto, &file
				   (uchar *)nvp,
				   (uint)nvp->textsize+sizeof(nvtype));
    printf("\n%d config bytes written to %s", 
	   nvp->textsize + sizeof(nvtype), fn);
    
    free(nvp);                /* free the allocated buffer */
    return(result);
}

#endif /* NVRAM_DRIVER */

/* snmp support */

/*
 * snmp_flash_exist
 *
 * Just trying to figure out if one of these puppies exists
 */
boolean snmp_flash_exist (void)
{
    return(flash.f_dev != 0);
}

/*
 * snmp_flash_busy
 *
 * check to see if the device appears available
 */
boolean snmp_flash_busy (void)
{
    int dev;

    if ((dev = dev_open(flash.f_dev, FLASH_OPEN|DEV_FLAGS_QUIET)) >= 0) {
        flash_read_fhdrs(dev);
        flash_query_roms(dev);
        dev_close(dev);
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
    return(flash.f_size);    
}

/*
 * snmp_flash_free
 *
 * how many free bytes
 */
uint snmp_flash_free (void)
{
    return(flash.f_size - flash.f_free_offset);
}

/*
 * snmp_flash_vpp
 *
 * is dip header in place
 * check to see if this can be programed
 */
boolean snmp_flash_vpp (void)
{
    return(flash.f_vpp_works);
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
    return((uchar *)flash.f_card);
}

/*
 * snmp_flash_ctlr
 * name of comtroller flash is on
 */
uchar *snmp_flash_ctlr (void)
{
    return((uchar *)flash.f_ctlr);
}

/*
 * snmp_flash_erase_all
 * erase the flash
 */
int snmp_flash_erase_all (void)
{
    int dev;
    int status;

    if (flash.f_vpp_works) {
	if ((dev = dev_open(flash.f_dev, FLASH_OPEN|DEV_FLAGS_QUIET)) >= 0) {
	    flash.f_verbose = FALSE;
	    flash_query_roms(dev);
	    status = flash_erase_all(dev);
	    dev_close(dev);
	    if (status)
                return(FLASH_OP_FAILURE);
            else
		return(FLASH_SUCCESS);
	}
	return(FLASH_OPEN_FAILURE);
    }
    return(FLASH_READ_ONLY);
}

/*
 * snmp_flash2tftp
 *
 * copy flash entry to tftp host
 */
int snmp_flash2tftp (addrtype *address, const char *filename)
{
    uchar *bp = NULL;
    int result = FLASH_SUCCESS;
    int dev = -1;

    if ((bp = malloc(TFTP_MAXDATA)) != NULL) {
        if ((dev = dev_open(flash.f_dev, FLASH_OPEN|DEV_FLAGS_QUIET)) >= 0) {
            flash.f_verbose = FALSE;
            flash_query_roms(dev);
            if (flash2tftp(dev, address, filename, filename, bp) != 0)
                result = FLASH_OP_FAILURE;
            dev_close(dev);
        } else {
            result = FLASH_OPEN_FAILURE;
	}
	free (bp);
    } else {
	result = BUFFER_ALLOC_FAILURE;
    }
    
    return(result);
}

/*
 * copy from tftp host to flash entry
 */
int snmp_tftp2flash (addrtype *address, const char *filename)
{
    uchar *bp = NULL;
    int result;
    int dev;

    result = FLASH_SUCCESS;

    if (!flash.f_vpp_works)
	return(FLASH_READ_ONLY);

    if ((bp = malloc(TFTP_MAXDATA)) != NULL) {
        if ((dev = dev_open(flash.f_dev, FLASH_OPEN|DEV_FLAGS_QUIET)) >= 0) {
            flash.f_verbose = FALSE;
            flash_query_roms(dev);
            if (net2flash(COPY_TFTP_FLASH, dev, address, filename, filename,
			  bp, FALSE) != 0)
                result = FLASH_OP_FAILURE;
            dev_close(dev);
        } else
            result = FLASH_OPEN_FAILURE;
	free (bp);
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


/*
 * flash_print_avail
 */
void flash_print_avail (void)
{
    int avail;

    avail = flash.f_size - flash.f_free_offset;
    if (avail < MIN_FILE_SIZE) {
	avail = 0;
    }
    printf(" [%d/%d bytes free/total]", avail, flash.f_size);
}


/*
 * flash_print_files
 * print contents of flash
 */
void flash_print_files (boolean all)
{
    int i;

    if (flash.f_filecnt) {
	printf("  Contains:\n");
	for (i=0; i<flash.f_filecnt; i++) {
	    if ((flash.f_fhdrs[i].ffh_flags & FFH_FLAGS_DELETED) == 0) {
                /* Deleted or invalidated file */
		printf("    %s (%d) [invalidated]\n",
		       flash.f_fhdrs[i].ffh_name,
		       flash.f_fhdrs[i].ffh_len);
	    }  else {
		printf("    %s (%d)\n",
		       flash.f_fhdrs[i].ffh_name,
		       flash.f_fhdrs[i].ffh_len);
	    }
	}
    } else {
	printf("Flash: No files present.\n");
    }
    flash_print_avail();
}


/*
 * verify_checksum
 */
void verify_checksum (void)
{
    uchar *bp = NULL;
    char *namep = NULL;
    flash_fhdr *ffp;
    int dev;

    if (!flash.f_filecnt) {
	printf("No files in flash.\n");
	return;
    }

    /* Get filename */
    if (!(namep = malloc(FLASH_NAME_LEN)) ||
	!(bp = malloc(TFTP_MAXDATA))) {
	printf("\nMemory allocation error");
    } else {
	printf("\nName of file to verify [%s]? ",
	       flash.f_fhdrs[flash.f_filecnt - 1].ffh_name);
	rdtty(namep, FLASH_NAME_LEN-1);
	if (!*namep) {
	    strcpy(namep, flash.f_fhdrs[flash.f_filecnt - 1].ffh_name);
	}
	dev = flash_open();
	if (dev <= 0) {
	    printf("\n%% Error opening flash");
	} else {
	    if ((ffp = flash_get_fhdr(dev, namep)) != NULL) {
		(void)flash_verify(dev, ffp, bp, TRUE);
	    } else {
		printf("Flash file checksum verification aborted.\n");
		printf("File not found - deleted or not present.\n");
	    }
	    dev_close(dev);

	}
    }
    free(bp);
    free(namep);
}


/*
 * flash_init_ags
 */

void flash_init_ags (subsystype *subsys)
{
    int dev;
    int d, i;
    uchar *cp;

    hostip_flash.type = ADDR_IP;
    hostip_flash.ip_addr = -1L;

    /*
     * Init flash header to all ones
     */
    cp = (uchar *)&flash.f_hdr;
    for (i=0; i<sizeof(flash_header); i++)
	*cp++ = 0xFF;
    
    for (dev=DEV_NUL; dev<=DEV_MAX; dev++) {
	/*
	 * Call device-specific initialization routine.
	 * Verify that controller (MCI or ENVM) is present.
	 */
	if (dev_init(dev) != 0)
	    continue;

	/*
	 * Get exclusive access
	 */
	if ((d = dev_open(dev, DEV_FLAGS_EXCL)) < 0)
	    continue;

	/*
	 * Try to touch the flash card through specific controller.
	 */
	if (!flash_probe(d)) {
	    dev_close(d);
	    continue;
	}

	/*
	 * Setup appropriate usec_delay routine based on cpu_type
	 */
	if (!set_usec_delay())
            continue;

	/*
	 * Read flash header and any file info.
	 */
	flash_read_fhdrs(d);
	
	/*
	 * Now try to assert VPP to verify rom info.
	 */
	flash_query_roms(d);

	dev_close(d);

	/*
	 * Let's break here because we found a flash device.  
         *
         * Note:  This assumes that the (envm) is always the first 
         * device in the table, and therefore has been initialized.
         * If this ordering changes, this break must be removed since
	 * some devices (e.g., envm) have non-flash functionality.
	 * It's more consistent to initialize them here.
	 */
         break;
    }
    decipher_memoptions();
    reg_add_flash_print_hardware(flash_print_hardware, "print flash hardware");
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
    return(TRUE);
}

static void get_target_filename (const char *sfname, char *rfname, uint len)
{
    parse_line_mode(rfname, string_1_mode, len,
		    "Destination file name? [%s] ", sfname);

    if (strlen(rfname) == 0)
	strcpy(rfname, sfname);
}

/*
 * Flash subsystem header for AGS platforms
 *
 * The NVRAM code in the AGS will discover flash cards sitting on
 * various network interface cards.  We use its results when setting
 * up flash on the AGS.
 */

#define FLASH_AGS_MAJVERSION 1
#define FLASH_AGS_MINVERSION 0
#define FLASH_AGS_EDITVERSION  1

SUBSYS_HEADER(flash_ags, FLASH_AGS_MAJVERSION, FLASH_AGS_MINVERSION,
	      FLASH_AGS_EDITVERSION, flash_init_ags, SUBSYS_CLASS_DRIVER,
	      "seq: nvram",
	      NULL);
