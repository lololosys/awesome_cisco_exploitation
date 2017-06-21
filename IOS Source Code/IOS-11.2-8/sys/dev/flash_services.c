/* $Id: flash_services.c,v 3.8.20.7 1996/09/06 21:00:34 snyder Exp $
 * $Source: /release/112/cvs/Xsys/dev/flash_services.c,v $
 *------------------------------------------------------------------
 * flash_services.c -  Generic flash services.
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: flash_services.c,v $
 * Revision 3.8.20.7  1996/09/06  21:00:34  snyder
 * CSCdi68334:  dump unused spinner support, save memory
 *              412 image, 4 data
 * Branch: California_branch
 *
 * Revision 3.8.20.6  1996/08/07  08:58:08  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.8.20.5  1996/07/12  02:00:54  widmer
 * CSCdi52582:  parser should remove the trailing spaces in the filename
 * Branch: California_branch
 *
 * Revision 3.8.20.4  1996/07/03  01:21:14  ramk
 * CSCdi58260:  agent returned out of range and non-enumeration for
 * ciscoFlashMIB
 * Branch: California_branch
 *
 * Revision 3.8.20.3  1996/07/02  01:10:15  ahh
 * CSCdi61851:  RCP: aborted copy from UNIX leaves file open
 * Close the file if checksum verification fails.
 * Branch: California_branch
 *
 * Revision 3.8.20.2  1996/05/07  00:37:55  ssangiah
 * CSCdi56357:  Misleading erase command behavior in the RSP file system.
 *   o Remove erase command from the RSP file system parse chain.
 *   o Clean up the RSP file system parse chain.
 *   o Fix problem with return of error codes by the flash MIB agent.
 * Branch: California_branch
 *
 * Revision 3.8.20.1  1996/03/18  19:12:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/07  08:43:27  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.2  1996/02/20  00:44:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/06  23:06:15  abaindur
 * CSCdi47558:  COPY MOP FLASH fails.
 *  Getting there.... one more try. Fix copy setup/cleanup for relocatable
 *  images. Increase buffer size to deal with the 1492 byte data blocks
 *  plus leftovers from previous block when crossing over from the reloc
 *  tables section to the text section.
 *
 * Revision 3.7  1996/01/30  08:15:38  asastry
 * CSCdi47558:  Fix COPY MOP FLASH feature.
 *
 * Revision 3.6  1996/01/24  20:13:00  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.5.2.1  1996/01/24  21:18:58  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1996/01/18  21:52:37  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/12/18  07:08:45  schaefer
 * CSCdi45771: include slave devices in 75xx flash mib
 *
 * Revision 3.3  1995/11/17  09:01:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:15:37  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:20:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  20:54:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/10/25  21:40:16  sagarwal
 * CSCdi41798:  Flash mib problems on the 4500
 * o Remove check for device subunit to prevent passing of corrupt data
 * o Init chipinfo array from beginning
 * o Make SNMP chip info static (and reupdate on partitioning or insertion
 *   of PCMCIA card) to make getnext retrievals faster
 *
 * Revision 2.8  1995/10/06  18:12:40  enf
 * CSCdi40422:  Cant netboot router with boothelper image
 *
 * Revision 2.7  1995/08/25  11:43:27  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.6  1995/07/29 00:27:33  enf
 * CSCdi37707:  CPUHOG message while booting from flash
 * make flash read more efficient and suspend reads every 512K bytes
 *
 * Revision 2.5  1995/07/27  17:59:08  henniger
 * CSCdi37648:  Err msg wrong if copy & no flash card on Sapphire/Catwoman
 * (flash_copy_setup() and flash_read_setup().)
 *
 * Revision 2.4  1995/07/16  18:03:51  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Bad programmer, no doggie biscuit.
 *
 * Revision 2.3  1995/07/15  02:36:46  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Add a silent_mode parameter to some routines and registries and set it
 * to TRUE when making the calls from SNMP WriteNet, netConfigSet, or
 * hostConfigSet.  Also, add an errmsg for each so the event can still get
 * logged, and make it clear what caused the event.  Fix other callers to
 * pass FALSE so current behavior will be maintained.
 *
 * Revision 2.2  1995/06/29  22:33:02  henniger
 * CSCdi36569:  Boot Load shudnt xsum files twice
 * bootload should not checksum file while loading
 * PCMCIA attribute search should be limited
 *
 * Revision 2.1  1995/06/07  20:25:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <stdlib.h>		/* for atoi() */
#include <ciscolib.h>
#include "ieee.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ui/command.h"
#include "../os/boot.h"
#include "../os/reloc.h" 
#include "../os/sum_magic_number.h"
#include "../dev/monitor1.h"
#include "../dev/flash_defines.h"
#include "../dev/dev_io.h"
#include "../dev/file_io.h"
#include "../dev/flash_services.h"
#include "logger.h"
#include "ttysrv.h"
#include "name.h"
#include "file.h"
#include "address.h"
#include "../ip/ip_registry.h"
#include "../ip/ip.h"
#include "../ip/tftp.h"
#include "../ui/debug.h"
#include "../../boot/emt.h"
#include "../h/image_classify.h"
#include "../ui/parse_util.h" 
#include "../dev/sr_flashmibdefs.h"
#include "parser.h"

/*
 * Globals
 */
static char *const flash_net_protocols[] = {
    "Unknown protocol",
    "TFTP",
    "MOP",
    "FLASH access",
    "ROM access",
    "RCP"
};
static const char flash_net_err_msg[] = "\n%s %s %s failed/aborted\n";
static addrtype hostip_flash;    /* Server address */

/* Table that maintains a list of available devices */
char *phy_dev_database[MAX_DEV_INIT_ENTRIES + 1];


/*************************** Local functions ****************************/

/*
 * Output message if we are running from flash and cannot use the FLH
 * because the boot ROMs are old. Basically give a long sermon on what
 * to do to upgrade flash.
 */
void run_from_flash_msg (void)
{
    ushort orig_config_val, new_config_val;

    /*
     * Read in the config register value.
     * If the lowest nibble is less than 2, set it to 2.
     * Also store the config register value with the lowest nibble
     * changed to a 1 (the value for loading the bootstrap).
     */
    orig_config_val = ~mon_getconfig();
    if ((orig_config_val & 0x0f) < 2)
        orig_config_val = (orig_config_val & 0xfff0) | 2;
    new_config_val  = (orig_config_val & 0xfff0) | 1;


    printf("\n                ---- ******** ----");
    printf("\nCannot copy to Flash while running code from it.");
    printf("\nUse the procedure below based on reloading the ROM-based");
    printf("\nbootstrap image.");
    printf("\nRefer to the product documentation for details.");
    printf("\n                ---- ******** ----");
    printf("\n1. Save configuration (using 'copy running-config "
	   "startup-config')");
    printf("\n   if necessary.");
    printf("\n2. Type in 'config term' to enter configuration mode.");
    printf("\n3. Enter 'config 0x%x' to set the soft configuration register.",
            new_config_val);
    printf("\n4. Enter '^Z' to exit configuration mode.");
    printf("\n5. Enter 'reload' command to reload the system.");
    printf("\n6. If you have a remote Telnet connection, terminate it, give");
    printf("\n   the router some time to reload, then reconnect.");
    printf("\n7. Enter 'enabled' mode and execute the copy command.");
    printf("\n8. Repeat steps 2 to 6 above, but this time in step 3, enter");
    printf("\n   'config 0x%x'.",orig_config_val);
    printf("\n                ---- ******** ----");
}

/*
 * Basic routines to get and dork around with names -
 * server name (or address)
 * source file name
 * destination file name
 */

/*
 * Get a file name. Strip any leading or trailing blanks.
 */
boolean flash_copy_file_name (char *fname, uint len,
			      const char *prompt, boolean verbose)
{
    if (strlen(fname) == 0) {  /* File name not specified */
        if (prompt) {   /* Yes, prompt user for name */
	    parse_line_mode(fname, string_1_mode, len, prompt);
        } else {
	    /* No file name and not supposed to prompt */
            return(FALSE);
	}
    }

    if (strlen(fname) == 0) {  /* A final check for name */
        if (verbose)
            printf("\nERR: File name not specified");
        return (FALSE);
    }
    return (TRUE);
}

/*
 * Get source and destination file names.
 */
static boolean flash_copy_get_names (flash_copy_parms_t *fparms)
{
    char dname[80];

    /*
     * Get file names. If no file name entered, bail out.
     * Point dfname to start of filename.
     */
    if (!flash_copy_file_name(fparms->sfname, (FLASH_FILE_NAME_LEN - 1),
			      fparms->prompt ? "Source file name? " : NULL,
			      TRUE)) {
	fparms->snmp_sts = SNMP_INVALID_SRC_FILE;
        return (FALSE);
    }
    sprintf(dname, "Destination file name [%s]? ", fparms->sfname);
    if (!flash_copy_file_name(fparms->dfname, (FLASH_FILE_NAME_LEN - 1),
			      fparms->prompt ? dname : NULL, FALSE)) {
        strcpy(fparms->dfname, fparms->sfname);  /* set default value */
    }

    return (TRUE);
}
void flash_create_path (flash_copy_parms_t *fparms, const char *fname)
{
    /* Form file path name - <dev>:<file> */
    sprintf(fparms->path, "%s:%s", fparms->devnm, fname);
}
/*
 * Prompt user for server name or address.
 */
boolean flash_copy_get_server (addrtype *host, char *bufp,
			       addrtype def_host, ulong prompt)
{
    nametype *ptr;
    int i;

    if (!prompt)        /* can't ask for host address */
        return (TRUE);

    parse_line_mode(bufp, string_1_mode, FLASH_SERVER_NAME_LEN,
		    "Address or name of remote host [%s]? ",
		    name_addr2string(host));

    if (*bufp) {  /* user entered something */
        if (!numeric_lookup(ADDR_IP, bufp, host)) {
            /*
             * Entered value is not a numeric. See if it's
             * a host name by doing a DNS lookup.
             */
            ptr = name_lookup(bufp, &i, NAM_IP);
            if (ptr) /* name lookup succeeded. Get address */
                address_copy(host, &ptr->v.ip.adr[0]);
            else {
                printf("\nERR: Invalid host address or name");
                return (FALSE);
            }
        }
    } 
    return (TRUE);
}

/*
 * Check to see if the specified file exists on flash. If no
 * partition number is specified as part of the file name then
 * check *all* partitions to locate the file.
 * Note that this function opens the file if it does find the file.
 * It is up to the calling routine to close the file if this function
 * returns TRUE.
 */
boolean flash_locate_and_open_file (flash_copy_parms_t *fparms)
{
    int part_cnt, indx;
    char *colonptr;

    flash_create_path(fparms, fparms->sfname); /* create path */
    fparms->fh = file_open(fparms->path, FILE_FLAGS_RDONLY);
    if (fparms->fh >= 0)
        return (TRUE);

    /*
     * Could not locate the file by searching for it in the the user
     * specified manner. Now, try searching through all partitions
     * *iff* no partition number was specified.
     */
    colonptr = strchr(fparms->devnm, FLASH_PATH_SEPARATOR);
    if (colonptr) /* User must have specified a partition number */
        return (FALSE);

    part_cnt = flash_dev_num_partns(fparms->devnm);
    strcat(fparms->devnm, ":");
    colonptr = fparms->devnm + strlen(fparms->devnm);
    for (indx = 1; indx <= part_cnt; indx++) {
        sprintf(colonptr, "%d", indx);
        flash_create_path(fparms, fparms->sfname); /* create path */
        fparms->fh = file_open(fparms->path, FILE_FLAGS_RDONLY);
        if (fparms->fh >= 0)
            return (TRUE);
    }
    return (FALSE);
}

static void flash_copy_server_addr_init (addrtype *host)
{
    host->type    = ADDR_IP;
    host->ip_addr = -1L;
}

/*
 * Free structures used for relocatable images
 */
static void reloc_image_transfer_cleanup (struct flash_write_args *fa) 
{
    if (fa->headers_block) {
	free(fa->headers_block);
	fa->headers_block = NULL;
    }

    fa->a_out_hdr = NULL;
    fa->sp_hdr = NULL;

    if (fa->ri_info) {
	free(fa->ri_info);
   	fa->ri_info = NULL;
    }
} 

/*
 * Categorize the download into various states depending on the
 * image portion being downloaded.
 * Determine the state based on where we are in the image.
 */
static int reloc_image_transfer_state (struct flash_write_args *fa)
{
    struct special_header *shdr;
    struct reloc_args *ri;
    ulong txt_cksum_start;

/****
    +-----------------------+
    | Exec & spl hdrs       |
    +-----------------------+  <-- spl_hdr_end
    | PIC relocator         |
    +-----------------------+  <-- pic_end
    |                       |
    | Reloc info            |
    +-----------------------+  <-- reloc_tables_end
    |                       |
    |                       |
    |                       |
    | Text                  |
    |                       |
    |                       |
    |                       |
    +-----------------------+  <-- text_seg_end
    |                       |
    | Data                  |
    |                       |
    +-----------------------+  <-- txt_cksum_start
    | Text checksum         |
    +-----------------------+  <-- txt_cksum_end
    |                       |
    | Data cont'd           |
    |                       |
    |                       |
    +-----------------------+  <-- data_seg_end
****/
    /*
     * fa->prev_len contains the start offset of our buffer within 
     * the image.
     * fa->len contains the end offset of our buffer within the
     * image.
     */
    ri = fa->ri_info;
    shdr = fa->sp_hdr;
    txt_cksum_start = ri->txt_cksum_end - sizeof(long);

    if (fa->prev_len >= ri->txt_cksum_end) {
        if (fa->len >= ri->data_seg_end)
            return (LAST_BLOCK_DOWNLOAD);
        else
            return (DATA_DOWNLOAD);
    }
    else if (fa->prev_len >= txt_cksum_start) {
	return (DATA_TC_DOWNLOAD);
    }
    else if (fa->prev_len >= ri->text_seg_end) {
        if (fa->len > txt_cksum_start)
            return (DATA_TC_DOWNLOAD);
        else
            return (DATA_DOWNLOAD);
    }
    else if (fa->prev_len >= ri->reloc_tables_end) {
        if (fa->len > ri->text_seg_end)
            return (TEXT_DATA_DOWNLOAD);
        else
            return (TEXT_DOWNLOAD);
    }
    else if (fa->prev_len >= ri->pic_end) {
        if (fa->len > ri->reloc_tables_end)
            return (RELOC_TEXT_DOWNLOAD);
        else
            return (RELOC_DOWNLOAD);
    }
    else if (fa->prev_len >= ri->spl_hdr_end) {
        if (fa->len > ri->pic_end)
            return (PIC_RELOC_DOWNLOAD);
        else
            return (PIC_DOWNLOAD);
    }
    else {
        if (fa->len > ri->spl_hdr_end)
            return (HEADER_PIC_DOWNLOAD);
        else     
            return (HEADER_DOWNLOAD);
    }
}

static void reloc_image_relocation_offset (struct flash_write_args *fa) 
{
    fstat_t file_info;
    ulong  new_text_link_addr;
    struct special_header *shdr;
    struct reloc_args *ri;
    
    shdr = fa->sp_hdr;
    ri = fa->ri_info;
    file_stat(fa->fh, &file_info);
    
    /* Extract the start of file data (i.e. the aout header) in memory */
    ri->image_mem_base_addr = fa->file_sys_info->phy_start + 
		              file_info.data_start; 

    /* Find out the absolute text base address in flash */
    new_text_link_addr = ri->image_mem_base_addr + ri->reloc_tables_end; 
    fa->text_offset = new_text_link_addr - shdr->r_text;

    /* Update the code entry point in the aout */
    fa->a_out_hdr->a_entry =  ri->image_mem_base_addr + ri->exec_hdr_end;
}

static void reloc_image_set_sections (struct flash_write_args *fa)
{
    struct reloc_args *ri;
    struct special_header *shdr;

    ri = fa->ri_info;
    shdr = fa->sp_hdr;

    /*
     * Set the positions of various sections within a relocatable
     * image.
     */
    ri->exec_hdr_end = SUN_AOUT_HDR_SIZE;
    ri->spl_hdr_end  = ri->exec_hdr_end + shdr->r_hdr_len;
    ri->pic_end	     = ri->spl_hdr_end + shdr->r_pgmsize;
    ri->reloc_tables_end = ri->pic_end + shdr->r_rsize;
    ri->text_seg_end = shdr->r_text_offset + shdr->r_tsize;
    ri->data_seg_end = shdr->r_data_offset + shdr->r_dsize;
    ri->txt_cksum_end = shdr->r_text_cksum_offset + sizeof(long);
}

static boolean reloc_image_cksum_init (struct flash_write_args *fa)
{
    ulong test_cksum;
    ulong dummy_long;
    struct special_header *shdr;

    fa->txt_cksum_known = TRUE;
    fa->spl_hdr_cksum_known = TRUE;
    fa->image_cksum_known = TRUE;
    shdr = fa->sp_hdr;
    dummy_long = 0xdeadbeef;

    /*
     * Determine whether our image understands the checksum
     * algorithms present in the image being downloaded.
     * Set flags and defaults (where necessary).
     * Note: the checksum generation call will return zero if
     * it does not understand the algorithm.
     * If the special header checksum is known, validate it.
     * If an invalid header checksum is detected, return FALSE
     * to abort the transfer.
     */
    test_cksum = reg_invoke_sum_region(shdr->r_hdr_cksum_algor,
                                       (uchar *)&dummy_long,
                                       sizeof(long), FALSE);
    if (test_cksum == 0) { /* header checksum algorithm unknown */
        fa->spl_hdr_cksum_known = FALSE;
        if (fa->text_offset != 0)
	    shdr->r_hdr_cksum_algor = 1;
    }

    /* 
     * Check the special header checksum -
     * Only if we know the checksum algorithm.
     */
    if (fa->spl_hdr_cksum_known) {
        test_cksum = reg_invoke_sum_region(shdr->r_hdr_cksum_algor, 
                                           (uchar *)shdr,
				           shdr->r_hdr_len, FALSE);
    	if (test_cksum != 0xFFFFFFFF) {
	    if (fa->verbose)
		printf("\nCOPY: Header checksum verification failure\n");
	    return(FALSE);    
    	}
    }

    test_cksum = reg_invoke_sum_region(shdr->r_text_cksum_algor, 
                                       (uchar *)&dummy_long,
				       sizeof(long), FALSE);
    if (test_cksum == 0)  /* text checksum algorithm unknown */
	fa->txt_cksum_known = FALSE;

    test_cksum = reg_invoke_sum_region(shdr->r_image_cksum_algor,
                                       (uchar *)&dummy_long,
                                       sizeof(long), FALSE);
    if (test_cksum == 0) {  /* image checksum algorithm unknown */
        fa->image_cksum_known = FALSE;
	if (fa->text_offset != 0)
	    shdr->r_image_cksum_algor = 1;
    }

    return (TRUE);
}

/* 
 * Detects a relocatable image and inits memory for headers.
 * Returns FALSE if could not allocate memory
 */
static boolean reloc_image_transfer_init (struct flash_write_args *fa, 
                                          long len)
{
    struct reloc_args *ri;
    ulong image_type, hdr_len, i;
    
    reg_invoke_classify_image((uchar *)fa->buf, &image_type, 
                               fa->file_sys_info);
    if (image_type != UNCOMPRESSED_68K_REL_IMAGE) {
	fa->relocatable = FALSE;
	return(TRUE);
    }

    fa->relocatable = TRUE;
   
    if (! reg_invoke_check_rel_image(fa->buf, len)) {
	if (fa->verbose)
	    printf("\nCOPY: Bad Relocatable image\n");
    	return(FALSE);
    }
 
    /*
     * Allocate memory for the exec and special headers
     * & then copy over the headers from the input buffer.
     */
    fa->a_out_hdr = (struct sun_exec *) fa->buf;
    fa->sp_hdr = (struct special_header *) (fa->buf +
                                          SUN_AOUT_HDR_SIZE);
    /*
     * Length = exec header length + special header length.
     * Hopefully, the special header checksum has been verified
     * at this point.
     */
    hdr_len = fa->sp_hdr->r_hdr_len + SUN_AOUT_HDR_SIZE;
    fa->headers_block = malloc(hdr_len);

    fa->ri_info = malloc(sizeof(struct reloc_args));
    if ((!fa->ri_info) || (!fa->headers_block)) {
	if (fa->verbose)
	    printf("\nCOPY: Memory allocation failure\n");
	return(FALSE);
    }
    
    /* Copy the a.out, special headers & jump instruction */
    bcopy(fa->buf, fa->headers_block, hdr_len);
    fa->a_out_hdr = (struct sun_exec *) fa->headers_block;
    fa->sp_hdr = (struct special_header *) (fa->headers_block + 
					    SUN_AOUT_HDR_SIZE);

    /* 
     * For the first write to flash, we write a dummy block
     * instead of the headers. Initialize the dummy block to 
     * all FFs so that we can come back and write the actual 
     * header. 
     */
    
    for (i=0; i<hdr_len; i++) 
	fa->buf[i] = (uchar)FLASH_DUMMY_CHAR;  

    /* Initialise the relocation information */
    ri = fa->ri_info;
    ri->num_straddling_bytes = 0;
    ri->reloc_tables_initialised = FALSE;
    for (i = 0; i<NUM_RELOC; i++) {
	fa->reloc_tables[i] = NULL;
	ri->current_pos_tables[i] = 0;
	ri->accumulated_offset[i] = 0;
    }
    
    for (i=0; i<NUM_CKSUMS; i++) 
	fa->cksums[i] = 0;

    reloc_image_set_sections(fa);

    /*
     * Calculate new link addresses, offsets,  modify special header.
     * Update the code entry point in the aout
     */
    reloc_image_relocation_offset(fa);

    return (reloc_image_cksum_init(fa));
}

static boolean reloc_image_next_reloc_entry (struct flash_write_args *fa,
                                             uint reloc_type,
                                             ulong *pos_next_entry,
                                             ulong *value_entry)
{
    ulong current_pos;
    RELOC_OFFSET_TYPE current_byte;
    RELOC_OFFSET_TYPE *first_entry;
    
    first_entry = fa->reloc_tables[reloc_type];
    current_pos = fa->ri_info->current_pos_tables[reloc_type];
    *value_entry = 0;
    
    do {
	if (current_pos == (fa->reloc_table_counts[reloc_type]-1)) 
	    return(FALSE);
        current_pos ++;
	current_byte = *((RELOC_OFFSET_TYPE *)(first_entry + current_pos));
	*value_entry += current_byte;
    } while (current_byte == 255);
    
    *pos_next_entry = current_pos;
    return(TRUE);
}

static boolean reloc_image_prev_reloc_entry (struct flash_write_args *fa, 
                                             uint reloc_type,
					     ulong *pos_prev_entry, 
                                             ulong *value_current_entry)
{
    ulong current_pos;
    RELOC_OFFSET_TYPE current_byte;
    RELOC_OFFSET_TYPE *first_entry;
    
    first_entry = fa->reloc_tables[reloc_type];
    current_pos = fa->ri_info->current_pos_tables[reloc_type];
    current_byte = *((RELOC_OFFSET_TYPE *)(first_entry + current_pos));
    *value_current_entry = 0;
    
    do {
        if (current_pos == 0)
	    return(FALSE);
	*value_current_entry += current_byte;
        current_pos --;
        current_byte = *((RELOC_OFFSET_TYPE *)(first_entry + current_pos));
    } while (current_byte == 255);
    
    *pos_prev_entry = current_pos;
    
    return(TRUE);
}


boolean reloc_image_next_reloc_offset (ulong lower_limit, ulong upper_limit,
			               uint reloc_type, ulong *abs_offset_addr, 
			               struct flash_write_args *fa)
{
    RELOC_OFFSET_TYPE *first_entry;
    ulong start_section;
    ulong pos_prev_entry;
    ulong pos_next_entry;
    ulong value_current_entry;
    ulong value_next_entry;
    ulong *cur_abs_offset;
    
    if (reloc_type == TEXT_F_TEXT) 
	start_section = fa->ri_info->reloc_tables_end;
    else if (reloc_type == TEXT_F_DATA)
	start_section = fa->ri_info->text_seg_end;
    else
	return (FALSE);

    if (upper_limit < lower_limit)
	return(FALSE);
    upper_limit -= start_section;
    lower_limit -= start_section;
    cur_abs_offset = &fa->ri_info->accumulated_offset[reloc_type];
    first_entry = fa->reloc_tables[reloc_type];
    
    /* First reposition the pointer so we point to an offset */
    /* that is less than the supplied lower limit */
    while ((*cur_abs_offset)>lower_limit) {
	if (!reloc_image_prev_reloc_entry(fa, reloc_type, &pos_prev_entry, 
					  &value_current_entry))
	    break;
	*cur_abs_offset -= value_current_entry;
	fa->ri_info->current_pos_tables[reloc_type] = pos_prev_entry;
    }
    /* Now start searching for the first offset that is greater */
    /* than or equal to the supplied lower limit                */
    while ((*cur_abs_offset)<lower_limit) {
	if (!reloc_image_next_reloc_entry(fa, reloc_type, &pos_next_entry,
				          &value_next_entry))
	    break;
	*cur_abs_offset += value_next_entry;
	fa->ri_info->current_pos_tables[reloc_type] = pos_next_entry;	
    }
    
    if (((*cur_abs_offset)>=lower_limit) && ((*cur_abs_offset)<upper_limit)) {
        *abs_offset_addr = (*cur_abs_offset + start_section);
        return(TRUE);
    }
    else
        return(FALSE);
}

static void reloc_image_relocation_offset_init (struct flash_write_args *fa,
                                                uint reloc_type)
{
    RELOC_OFFSET_TYPE current_byte;

    if (fa->ri_info->current_pos_tables[reloc_type] == 0) {
    	current_byte = *(fa->reloc_tables[reloc_type]);
    	fa->ri_info->accumulated_offset[reloc_type] = current_byte;
    	while (current_byte == 255) {
		fa->ri_info->current_pos_tables[reloc_type] ++;
		current_byte = *(fa->reloc_tables[reloc_type] +
				 fa->ri_info->current_pos_tables[reloc_type]);
		fa->ri_info->accumulated_offset[reloc_type] += current_byte;
	}
    }
}
 
static boolean reloc_image_relocation_init (struct flash_write_args *fa,
                                         int num_tables)
{
    int i; 
    struct reloc_args *ri;
    ulong reloc_info_size; 
    
    ri = fa->ri_info;
    if (ri->reloc_tables_initialised) 
	return (TRUE);
    reloc_info_size = 0;

    /*
     * Set pointers to the relocation tables (in flash),
     * set size of each relocation table.
     */
    fa->reloc_tables[TEXT_F_TEXT] = 
        (RELOC_OFFSET_TYPE *)(ri->image_mem_base_addr + ri->pic_end);
    fa->reloc_table_counts[TEXT_F_TEXT] =
        *((ulong *)fa->reloc_tables[TEXT_F_TEXT]);
    reloc_info_size += (fa->reloc_table_counts[TEXT_F_TEXT] + sizeof(ulong));
    if (reloc_info_size > fa->sp_hdr->r_rsize)
        return (FALSE);
    fa->reloc_tables[TEXT_F_TEXT] += sizeof(ulong)/RELOC_OFFSET_SIZE; 
    reloc_image_relocation_offset_init(fa, TEXT_F_TEXT);
 
    for (i=1; i<num_tables; i++) {
	fa->reloc_tables[i] = fa->reloc_tables[i-1]; 
	fa->reloc_tables[i] += fa->reloc_table_counts[i-1];
	fa->reloc_table_counts[i] = *((ulong *)fa->reloc_tables[i]);
	reloc_info_size += (fa->reloc_table_counts[i] + sizeof(ulong));
	if (reloc_info_size > fa->sp_hdr->r_rsize)
	    return (FALSE);
	fa->reloc_tables[i] += sizeof(ulong)/RELOC_OFFSET_SIZE;
 	reloc_image_relocation_offset_init(fa, i);	
    }

    if ((reloc_info_size%sizeof(ulong)) != 0)
        reloc_info_size += (sizeof(ulong) - reloc_info_size%sizeof(ulong));
    if (reloc_info_size != fa->sp_hdr->r_rsize)
	return(FALSE);
    ri->reloc_tables_initialised = TRUE;
    return (TRUE);
}

static ulong reloc_image_txt_cksum_write (struct flash_write_args *fa)
{
    ulong cksum_offset;
    ulong txt_cksum_start;
    ulong num_straddling_bytes = 0;
    ulong new_txt_cksum;
    struct reloc_args *ri;

    ri = fa->ri_info;
    txt_cksum_start = ri->txt_cksum_end - sizeof(ulong);
    cksum_offset = txt_cksum_start - fa->prev_len;
 
    /*
     * If we know the text checksum algorithm, AND the image contains
     * a checksum, update the checksum.
     * If we don't know the algorithm, set the MAGIC value so that
     * the image does not use the checksum when running.
     */
    if (fa->len >= ri->txt_cksum_end) { /* Checksum within block */
   	fa->cksums[ORIG_TXT_CKSUM] = *((ulong *)(fa->buf + cksum_offset));
	if (fa->text_offset == 0)
	    return ((fa->len - fa->prev_len));
	if ((fa->txt_cksum_known) && 
            (fa->cksums[ORIG_TXT_CKSUM] != MAGIC_VALUE)) 
	    new_txt_cksum = fa->cksums[RELOC_TXT_CKSUM];
	else
	    new_txt_cksum = MAGIC_VALUE;
	*((ulong *)(fa->buf + cksum_offset)) = new_txt_cksum;
    } 
    else {  /* Checksum straddles block */
	num_straddling_bytes = fa->len - txt_cksum_start;
	num_straddling_bytes += (txt_cksum_start % sizeof(long));
    }

    return ((fa->len - fa->prev_len - num_straddling_bytes));
} 
	 
static ulong reloc_image_relocate_buffer (struct flash_write_args *fa, 
                                          ulong upper_limit,  
			                  uint reloc_type)
{
    ulong current_pos;
    boolean done;
    ulong next_addr_in_buf;
    ulong abs_pos_addr;
    ulong num_straddling_bytes;
    struct reloc_args *ri;
   
    /*
     * Relocation amount is zero. So just return.
     */
    if (fa->text_offset == 0) 
	return((upper_limit - fa->prev_len)); 

    ri = fa->ri_info;
    num_straddling_bytes = 0;
    current_pos = fa->prev_len;
    done = FALSE;
     
    while (!done) {
 	if (!reloc_image_next_reloc_offset(current_pos, upper_limit, 
                                           reloc_type, &abs_pos_addr, fa)) {
	    done = TRUE;
	    num_straddling_bytes = 0;
	    break;
	}
	
	if ((abs_pos_addr + sizeof(ulong)) > upper_limit) {
	    /* Straddling address */
	    num_straddling_bytes = upper_limit - abs_pos_addr;
	    num_straddling_bytes += (abs_pos_addr % sizeof(long));  
	    done = TRUE;
	    break;
	}

	next_addr_in_buf = abs_pos_addr - fa->prev_len;	
        *(ulong *)(fa->buf + next_addr_in_buf) += fa->text_offset;
	current_pos = abs_pos_addr + sizeof(ulong);
    }
    return((upper_limit - fa->prev_len - num_straddling_bytes));
}

static void reloc_image_calculate_cksum (char *buf, ulong *old_cksum,  
			                 ulong cksum_algo, long buf_len) 
{
    ulong buf_cksum;
   
    buf_cksum = reg_invoke_sum_region(cksum_algo, (uchar *)buf, buf_len, TRUE);
    *old_cksum = reg_invoke_sum_values(cksum_algo, *old_cksum, buf_cksum);
} 
    
     
static ulong reloc_image_transfer_process (struct flash_write_args *fa, 
                                           long len)
{

    ulong new_block_pos;
    ulong write_length;
    struct reloc_args *ri;
    int download_state;
    
    download_state = reloc_image_transfer_state(fa);
    ri = fa->ri_info;
    new_block_pos = ri->num_straddling_bytes;

    /* 
     * Make sure all blocks are greater than 512 bytes long
     * else some corruption/truncation has occurred
     */
    if ((len < TFTP_MAXDATA) && (download_state != LAST_BLOCK_DOWNLOAD)) {
	if (fa->verbose)
	    printf("\nCOPY: Bad image - possibly corrupted\n"); 
	return(0);
    }

    if (download_state != HEADER_DOWNLOAD) 
	reloc_image_calculate_cksum((fa->buf + new_block_pos), 
                                    &fa->cksums[OLD_IMG_CKSUM],
			            fa->sp_hdr->r_image_cksum_algor, 
                                    (len - new_block_pos));
    
    switch(download_state) {
    case HEADER_DOWNLOAD: 
        return (ri->spl_hdr_end);
	break;

    case HEADER_PIC_DOWNLOAD:
        /* 
         * Save downloaded headers and write all FFs to flash.
         * Revisit the saved headers and write them to flash at the
         * end of the download (after updating the checksums).
         */
	write_length = len;
	break;

    case PIC_DOWNLOAD:
    case PIC_RELOC_DOWNLOAD:
    case RELOC_DOWNLOAD:
    case UNKNOWN_STATE:
	write_length = len;
	break;

    case RELOC_TEXT_DOWNLOAD:
        /*
         * Block contains relocation info plus text. We need to blindly
         * write the relocation info to flash. The text needs to be 
         * processed first for relocation.
         * So we will write the reloc info portion and treat the text
         * as straddling bytes. The text will then get processed and
         * written in the TEXT_DOWNLOAD (next) state.
         */
	write_length = ri->reloc_tables_end - fa->prev_len;
        reloc_image_calculate_cksum((fa->buf + write_length), 
                                    &fa->cksums[OLD_TXT_CKSUM],
                	            fa->sp_hdr->r_text_cksum_algor,
                                    (len - write_length));
	break;

    case TEXT_DOWNLOAD:
        /*
         * Block contains only text. Calculate checksum on the
         * virgin data, relocate any references, then recalculate
         * new checksum.
         */
	if (!reloc_image_relocation_init(fa, NUM_RELOC)) {
	    if (fa->verbose)
		printf("\nCOPY: Bad image - possibly corrupted\n");
	    return(0);
	}
	reloc_image_calculate_cksum((fa->buf + new_block_pos), 
                                    &fa->cksums[OLD_TXT_CKSUM],
			            fa->sp_hdr->r_text_cksum_algor, 
                                    (len - new_block_pos));
	write_length = reloc_image_relocate_buffer(fa, fa->len, TEXT_F_TEXT); 
	reloc_image_calculate_cksum(fa->buf, &fa->cksums[RELOC_TXT_CKSUM], 
			            fa->sp_hdr->r_text_cksum_algor, 
                                    write_length);
	break;

    case TEXT_DATA_DOWNLOAD:
        /*
         * Block contains both text and data. Do something similar
         * to the RELOC_TEXT_DOWNLOAD state.
         */
	write_length = ri->text_seg_end - fa->prev_len; 
	reloc_image_calculate_cksum((fa->buf + new_block_pos), 
                                    &fa->cksums[OLD_TXT_CKSUM],
			            fa->sp_hdr->r_text_cksum_algor, 
                                    (write_length - new_block_pos));
	reloc_image_relocate_buffer(fa, ri->text_seg_end, TEXT_F_TEXT);   
	reloc_image_calculate_cksum(fa->buf, &fa->cksums[RELOC_TXT_CKSUM], 
			            fa->sp_hdr->r_text_cksum_algor, 
                                    write_length);
	break;

    case DATA_TC_DOWNLOAD:
        /*
         * Block contains data plus text segment checksum.
         * The text checksum may or may not be complete.
         * The called function will do the Right Thing -
         * either update the checksum if it is complete, or else
         * treat it like straddling bytes for the next cycle.
         */
	write_length = reloc_image_txt_cksum_write(fa);
	write_length = 
            reloc_image_relocate_buffer(fa, (fa->prev_len + write_length), 
                                        TEXT_F_DATA);
	break;

    case DATA_DOWNLOAD:
    case LAST_BLOCK_DOWNLOAD:
        /*
         * Block contains only data. Do relocation and write it
         * to flash.
         */
	write_length = reloc_image_relocate_buffer(fa, fa->len, TEXT_F_DATA);
	break;

    default:
        /*
         * Why did we come here ??
         */
	write_length = len;
	break;	
    }

    if (download_state != HEADER_DOWNLOAD) 
        reloc_image_calculate_cksum(fa->buf, &fa->cksums[RELOC_IMG_CKSUM],
		                    fa->sp_hdr->r_image_cksum_algor, 
                                    write_length);

    return (write_length);  /* How much we processed */
}

static boolean reloc_image_header_write (struct flash_write_args *fa)
{
    int nbytes; 
    ulong file_cur_pos;
    ulong cksum_temp;
    
    if (!fa->relocatable) {
	return(TRUE);
    }
   
    /* 
     * Check the text checksum. 
     * If we know the checksum algorithm, and the checksum is
     * present, verify it.
     */
    if ((fa->txt_cksum_known) && 
        (fa->cksums[ORIG_TXT_CKSUM] != MAGIC_VALUE)) {
	if (fa->cksums[OLD_TXT_CKSUM] != fa->cksums[ORIG_TXT_CKSUM]) {
	    if (fa->verbose)
		printf("\nCOPY: Text checksum verification failure\n");
	    return(FALSE);          
	}
    }
 
    /* 
     * Check the image checksum -
     * Only if we know the checksum algorithm.
     */
    if ((fa->image_cksum_known) && 
	(fa->sp_hdr->r_image_cksum != fa->cksums[OLD_IMG_CKSUM])) {
	    if (fa->verbose)
		printf("\nCOPY: Image checksum verification failure\n");
	    return(FALSE);     
    }
    
    if (fa->text_offset != 0) {
        fa->sp_hdr->r_image_cksum = fa->cksums[RELOC_IMG_CKSUM];
    
    	/*
     	 * Recalculate header checksum.
     	 */
   	fa->sp_hdr->r_text = fa->ri_info->image_mem_base_addr + 
       	                     fa->ri_info->reloc_tables_end; 
    	fa->sp_hdr->r_hdr_cksum = 0;
    	cksum_temp = reg_invoke_sum_region(fa->sp_hdr->r_hdr_cksum_algor, 
        	                           (uchar *)fa->sp_hdr,
			                   fa->sp_hdr->r_hdr_len, FALSE);
    	cksum_temp = 0xffffffff - cksum_temp;
	fa->sp_hdr->r_hdr_cksum = cksum_temp; 
    }
 
    file_cur_pos = file_seek(fa->fh, FILE_START, 0);
    if (file_cur_pos != 0) 
        return(FALSE);
    
    nbytes = file_write(fa->fh, fa->headers_block, TFTP_MAXDATA);
    if (nbytes != TFTP_MAXDATA)
        return(FALSE);
    
    return(TRUE);
}

/*
 * net2flash_write -- Callback routine to perform flash specific
 * actions on each tftp response packet.  Specifically, write tftp 
 * buffer to flash, checking for errors or user interrupt.
 */
static boolean net2flash_write (uchar **buf, long *bytesleft, long len,
				void *opaque_args)
{
    ulong nbytes;
    struct flash_write_args *fa;
    ulong write_length;
    ulong buf_len;
    
    if (len == 0) 	     /* nothing transferred */
	return(TRUE);	
    
    fa = (struct flash_write_args *)opaque_args;
    fa->len += len;  /* Total length read so far */
    
    /* Check for relocatable images if first block */
    if (fa->blocknumber == 1)  {
	if (!reloc_image_transfer_init(fa, len)) return(FALSE);
    }
    
    if (!fa->relocatable) {  /* Hardlinked executable */
    	nbytes = file_write(fa->fh, fa->buf, len);
    	if ((long) nbytes != len)
	    return(FALSE);
    	/*
	 * Only use first TFTP_MAXDATA bytes of buffer.  Reset 
	 * current buffer offset to beginning.
	 */
    	*buf = fa->buf;
	fa->blocknumber ++;
	fa->prev_len += len;
    }
    else {			/* Relocatable Image */

        /* 
         * Get the total length of data. This equals the length
         * passed in plus any bytes left over from previous 
         * buffer.
         */
	buf_len = fa->len - fa->prev_len; 
	write_length = reloc_image_transfer_process(fa, buf_len);
	if (write_length == 0) /* some major problem */
	    return(FALSE);

        /*
         * Now write the data that's been processed. This may
         * or may not be equal to the total length of data in 
         * the buffer.
         */
	nbytes = file_write(fa->fh, fa->buf, write_length);  
	if (nbytes != write_length) /* major write problem */
	    return(FALSE);

        /*
         * How much data was not processed this time but left over
         * for the next round ? Answer is ...
         * Total length of data - length of data written to flash.
         * If there are leftovers, put them in the doggy bag and
         * stash away for the next cycle.
         */
	fa->ri_info->num_straddling_bytes = buf_len - write_length;
	if (fa->ri_info->num_straddling_bytes > 0)  /* left overs */ 
	    bcopy((uchar *)(fa->buf + write_length), (uchar *)fa->buf, 
		           fa->ri_info->num_straddling_bytes);

        /*
         * Bump up buffer pointer so the download protocol does not 
         * eat up our doggy bag but keeps it intact.
         */
	*buf = fa->buf + fa->ri_info->num_straddling_bytes;
	fa->blocknumber ++;
	fa->prev_len += write_length;
    }
    return(TRUE);
}

#define FLASH_MAX_DUMMY   (TFTP_MAXDATA * 2)
/*
 * Dummy callback function that's used during file access check.
 */
static boolean flash_dummy_write (uchar **buf, long *bleft, 
                                  long len, void *op_args)
{
    struct flash_write_args *fa;
    
    fa = (struct flash_write_args *)op_args;

    /*
     * If we have read as much as we want, start ignoring
     * anything that comes after that and return FALSE to
     * terminate the transfer.
     * Else, keep reading till file exhausted.
     */
    if (fa->len < FLASH_MAX_DUMMY) {
        fa->len += len;
        return ((*bleft == 0) ? FALSE : TRUE); /* terminate transfer */
    }
    else {
        *buf = fa->buf + FLASH_MAX_DUMMY;
        return (FALSE);   /* got enough; force a termination */
    }
}

static void flash_decode_file_read_status (int *sts, int *snmp_sts)
{
    switch (*sts) {
        case ERROR_OK:
            *sts = 1;      /* success; something positive */
            return;

        case ERROR_NOMEM:
	    *snmp_sts = SNMP_LOW_RESOURCES;
	    *sts = -1;
	    return;
        case ERROR_TIMEOUT:
	    *snmp_sts = SNMP_FILE_TRANSFER_ERR;
            *sts = -1;     /* failure; but retry */
            return;

        case ERROR_NOFILE:
	    *snmp_sts = SNMP_FILE_OPEN_ERR;
	    *sts = 0;
	    return;

	case ERROR_ABORT:
	case ERROR_TOOBIG:
        default:
	    *snmp_sts = SNMP_UNKNOWN_ERROR; 
            *sts = 0;     /* failure; no retry */
            return;
    }
}

/*
 * The core function that downloads the file and writes to flash.
 * If 'test' is TRUE, it indicates an access check for the file.
 * Just try to download the first two blocks and return without
 * writing to flash. If the access was successful, the calling
 * routine will use the two blocks downloaded to do validations.
 */
static boolean invoke_net_to_flash (flash_copy_parms_t *fparms,
                                     ulong size, boolean test)
{
    struct flash_write_args flash_wargs; 
    filetype tftp_file;                
    fsstat_t file_system_info;
    boolean header_write_status; 
    
    flash_wargs.verbose = (fparms->verbose ? TRUE : FALSE);
    flash_wargs.buf = (char *)&fparms->buff;
    flash_wargs.fh = fparms->fh;
    flash_wargs.len = 0;
    flash_wargs.prev_len = 0;
    flash_wargs.relocatable = FALSE; 	/* Not relocatable, by default */
    flash_wargs.blocknumber = 1;	
    
    /* Set all pointers to NULL */
    flash_wargs.headers_block = NULL;
    flash_wargs.a_out_hdr = NULL; 
    flash_wargs.sp_hdr = NULL;
    flash_wargs.ri_info = NULL;
    
    /* Need to find physical start address of flash */
    fsstat(fparms->devnm, &file_system_info);
    flash_wargs.file_sys_info = &file_system_info;
    
    fparms->datalen = 0;
    fparms->buflen = size;
    
    tftp_file.filename = (char *)&fparms->sfname;
    tftp_file.proto = fparms->copy_option;
    address_copy(&tftp_file.ps.tuba.boothost, &fparms->addr);
    tftp_file.ps.tuba.username = fparms->rcp_username;
    tftp_file.ps.tuba.display = (fparms->verbose ? TRUE : FALSE);
    /*
     * Call the network download function to attempt the read.
     */
    fparms->sts = 
        reg_invoke_file_read(tftp_file.proto, &tftp_file, fparms->buff,
                             &fparms->buflen, flash_wargs.verbose,
                             (test ? flash_dummy_write : net2flash_write),
                             (void *)&flash_wargs);
    
    /* If the image is relocatable, write headers to flash file */
    if (!test) {
    	header_write_status = reloc_image_header_write(&flash_wargs);
        reloc_image_transfer_cleanup(&flash_wargs);
    	if (!header_write_status) {
	    fparms->snmp_sts = SNMP_FILE_TRANSFER_ERR;
	    return(FALSE);
	}
    }
    /* decode the return value */
    flash_decode_file_read_status(&fparms->sts, &fparms->snmp_sts);
    
    if (fparms->sts <= 0)  { /* failure */
        if (test)           /* testing for file access */
            if (flash_wargs.len == size) /* Downloaded all we wanted */ 
                return (TRUE);  /* file access was a success */
        return (FALSE);  /* failure - timeout or abort or ... */
    }
    
    return (TRUE);
}
/*
 * Core function for TFTPing a file out from flash.
 */
void invoke_tftp_from_flash (flash_copy_parms_t *fparms,
                                    ipsocktype *soc)
{
    int blknum;
    ulong nbytes,len;
    boolean result, server;
    fstat_t fsts;

    /*
     * Read a block of Flash data and write it to the server.
     * Loop till entire file read or TFTP failure.
     */
    /* If verbose, we ain't a server; someone's doing "copy flash tftp" */
    server = fparms->verbose ? FALSE : TRUE;
    result = TRUE;
    file_stat(fparms->fh, &fsts);  /* read in file info */
    len = fsts.len;
    for (blknum = 1; result; blknum++) {
        nbytes = file_read(fparms->fh, (char *)fparms->buff, TFTP_MAXDATA);
        len -= nbytes;
        result = reg_invoke_ip_tftp_sendblock(soc, blknum, fparms->buff, 
                                              nbytes, server);
        /*
         * If we read less than specified from flash, we
         * must have reached the end.
         */
        if (nbytes < TFTP_MAXDATA)
            break;
    }
    if (len) {
        if (fparms->verbose) {
            printf("\nCopy Failed.\n");
        }
	fparms->snmp_sts = SNMP_FILE_TRANSFER_ERR;
    }
    if (!result)
	fparms->snmp_sts = SNMP_FILE_TRANSFER_ERR;
}

/*
 * Start a download to flash.
 * Open the file in flash, do an access check, validate the file
 * from the initial blocks downloaded, and get a final confirmation
 * for the copy. Invoke the FLH if we are going to use it, else
 * let the calling function start the download if flash is RW, or 
 * return error.
 */
static boolean flash_download_start (flash_copy_parms_t *fparms,
                                     fsstat_t *fs_info, char *proto)
{
    int reopen_sts;
    boolean erase = FALSE;

    /*
     * Open flash file for writing.
     */
    fparms->fh = file_open(fparms->path,
                 FILE_FLAGS_RW | FILE_FLAGS_CHECKSUM |
                 ((fs_info->status == DEV_STATE_RFF) ? FILE_FLAGS_FORCE:0) |
                 fparms->erase | fparms->verbose | fparms->prompt);
    if (fparms->fh < 0) {
        if (fparms->verbose)
            printf("\nERR: '%s' open - %s",
                    fparms->dfname, file_error_msg(fparms->fh));
	fparms->snmp_sts = SNMP_DEVICE_FULL;
        return (FALSE);
    }
    erase = (file_flags(fparms->fh) & FILE_FLAGS_REOPEN_ERASE) ? TRUE : FALSE;
    if (erase) {
        fs_info->fcnt   = 0;
        fs_info->totcnt = 0;
        fs_info->free   = fs_info->size;
    }
    if (fparms->copy_option != FILE_ACCESS_MOP) {
        if (reg_invoke_flash_download_verify(fparms, fs_info, erase) < 0) {
            file_close(fparms->fh);
            return (FALSE);
        }
    }
    /* in case of RFF, check to see if config has been modified and not saved */
    if (fs_info->status == DEV_STATE_RFF) {
        if (reg_invoke_is_config_dirty() == TRUE) {
            if (yes_or_no("\nSystem configuration has been modified. Save",
			  FALSE, FALSE)) {
                write_memory(NULL); /* save config to NVRAM */
            }
        }
    }
    if (fparms->prompt) { /* Get confirmation */

        printf("\n\nCopy '%s' from server"
               "\n  as '%s' into Flash WITH%s erase? [yes/no]",
               fparms->sfname, fparms->dfname, 
               erase ? "" : "OUT");

        if (!yes_or_no(NULL, FALSE, FALSE)) {
            printf("\n%s - user request", "Copy aborted");
            file_close(fparms->fh);
            return (FALSE);
        }
    }

    switch (fs_info->status) {
    case DEV_STATE_RW:
        reopen_sts = file_reopen(fparms->fh, FILE_FLAGS_REOPEN);
        if (reopen_sts < 0) {
            if (fparms->verbose)
                printf("\nERR: '%s' open - %s",
                        fparms->dfname, file_error_msg(reopen_sts));
	    fparms->snmp_sts = SNMP_DEVICE_ERR;
            file_close(fparms->fh);
            return (FALSE);
        }
        return (TRUE);  /* go back to start the network download */

    case DEV_STATE_RFF:
        fparms->erase = (erase ? 1 : 0);
        reg_invoke_flh_invoke((void *)fparms);
	fparms->snmp_sts = SNMP_DEVICE_READONLY;
        return (FALSE);

    default:
	fparms->snmp_sts = SNMP_UNKNOWN_ERROR;
        return (FALSE);
    }
}

/*
 * Start the copy out of flash. Open the flash file, open an
 * IP socket, and make a network write request.
 */
static ipsocktype * flash_upload_start (flash_copy_parms_t *fparms)
{
    int fh;
    ipsocktype *soc;
    boolean result;
    char path[FLASH_PATH_MALLOC_LEN];

    /*
     * Open file in read-only mode and only if checksum is good.
     */
    strcpy(path, fparms->path);	
    fparms->fh = file_open(path, FILE_FLAGS_RDONLY |
                           FILE_FLAGS_CHECKSUM |
                           fparms->verbose | fparms->prompt);

    if (fparms->fh < 0) {
	fparms->snmp_sts = SNMP_FILE_CHECKSUM_ERR;
        if (fparms->verbose)
            printf("\nERR: '%s' open - %s",
                    fparms->sfname, file_error_msg(fparms->fh));
	strcpy(path, fparms->path);
	fh = file_open(path, FILE_FLAGS_RDONLY |
		       fparms->verbose | fparms->prompt);
	if (fh < 0)
	    fparms->snmp_sts = SNMP_FILE_OPEN_ERR;
	else
	    file_close(fh);
        return (NULL);
    }

    if (fparms->prompt) { /* Get confirmation */
        printf("\nCopy '%s' from Flash to server\n  as '%s'? [yes/no]",
               fparms->sfname, fparms->dfname);
        if (!yes_or_no(NULL, FALSE, FALSE)) {
            printf("\n%s - user request", "Copy aborted");
            file_close(fparms->fh);
            return (NULL);
        }
    }

    /*
     * If we are using RCP,there's nothing more to be done.
     * Return with a non-NULL value.
     */
    if (fparms->copy_option == FILE_ACCESS_RCP)
        return ((ipsocktype *)1); 

    /*
     * Open socket for writing.
     */
    soc = reg_invoke_ip_tftp_open_socket(&fparms->addr, SOCK_ANYSRC);
    if (!soc) { /* error */
        if (fparms->verbose)
            printf(flash_net_err_msg, 
                   flash_net_protocols[fparms->copy_option],
                   "to", name_addr2string(&fparms->addr));
	fparms->snmp_sts = SNMP_FILE_TRANSFER_ERR;
        file_close(fparms->fh);
        return (NULL);
    }

    /*
     * Make network write request using socket opened earlier.
     */

    /*
     * reg_invoke_ip_tftp_WRQ(ip socket, source filename, data transfer mode,
     *                        boolean server : FALSE - display file transfer progress
     *                                         TRUE  - console silent transfer)
     */
    result = reg_invoke_ip_tftp_WRQ(soc, fparms->dfname, "octet",
				    fparms->verbose ? FALSE : TRUE);
    if (result != TRUE) {  /* Write request failed */
        if (fparms->verbose)
            printf("\n%s - TFTP timeout", "Copy aborted");
        reg_invoke_ip_close_socket(soc);
	fparms->snmp_sts = SNMP_FILE_TRANSFER_ERR;
        file_close(fparms->fh);
        return (NULL);
    }
    return (soc);
}

/*
 * Close file and display copy time.
 */
static void flash_copy_end (flash_copy_parms_t *fparms)
{
    file_close(fparms->fh);  /* Close file */

    fparms->elapsed_time = ELAPSED_TIME(fparms->start_time);
    if (fparms->verbose && fparms->elapsed_time) {
        printf("\nFlash copy took ");
        print_ticks_in_dhms(fparms->elapsed_time);
        printf(" [hh:mm:ss]");
        flush();
    }
}

/*
 * Handle network timeouts. In such a case, we would have written
 * good data blocks to flash. File close would give us a good
 * checksum since whatever we have written is good. So go and
 * intentionally corrupt some data to get a bad checksum.
 * Another way could be to have a function that would go dork
 * with the checksum calculated in memory.
 * Either way, both are pretty dirty.
 */
static void flash_copy_abnormal_terminate (flash_copy_parms_t *fparms)
{
    ulong len, seek, pos;

    /*
     * If it was a user abort, just return. We don't need to
     * specifically invalidate the checksum. The file i-o routines
     * will ensure it for us.
     * We only need to take care of the network timeout case.
     */
    if (psipending())
        return;

    /* Get file length by seeking to its end */
    len = file_seek(fparms->fh, FILE_END, 0);
    if (len) {  /* File has something in it */
        /*
         * Start from the first byte and see which one we can
         * corrupt. To corrupt, look for a non-zero byte and
         * make it zero.
         */
        for (pos = 0; pos < len; pos++) {
            seek = file_seek(fparms->fh, FILE_START, pos);
            if (file_read(fparms->fh, (char *)fparms->buff, 1)) {
                if (fparms->buff[0]) {  /* byte non-zero */
                    fparms->buff[0] = 0; /* force it to zero */
                    file_seek(fparms->fh, FILE_START, pos);
                    file_write(fparms->fh, (char *)fparms->buff, 1);
                    break;  /* break out of for loop */
                }
            } else {
                break;   /* file system gone bad */
            }
        }
    }
}

/************************* Global functions ********************************/
/*
 * Various commands, CLI initialted or SNMP initiated or network
 * initiated. Will do copy to/from flash, boot from flash, erase
 * flash, verify checksum of flash file, display directory, etc.
 */


/*
 * Copy a file into flash from a tftp server.
 */
boolean copy_tftp_flash (flash_copy_parms_t *fparms)
{
    fsstat_t fs_info;
    boolean tftp_okay = FALSE;

    fparms->copy_option = FILE_ACCESS_TFTP;
    if (!flash_copy_setup(fparms, &fs_info))
        return (FALSE);

    /*
     * Get IP address of tftp server.
     */
    if (fparms->addr.ip_addr == IPADDR_ZERO)
        address_copy(&fparms->addr, &hostip_flash);
    if (!flash_copy_get_server(&fparms->addr, (char *)fparms->buff,
                               hostip_flash, fparms->prompt))
        return (FALSE);  /* Unknown host; give up */
    /* save host address for next time default */
    address_copy(&hostip_flash, &fparms->addr); 

    /* get file names */
    if (!flash_copy_get_names(fparms))
        return (FALSE);
    flash_create_path(fparms, fparms->dfname);

    /* 
     * check to see if the src/desination length too big
     * only an issue when running flh
     */
    if (fs_info.status == DEV_STATE_RFF && reg_invoke_flh_version() > 0) {
	if (reg_invoke_flh_check_parms(fparms->sfname, fparms->path, 
				       fparms->verbose, FILE_ACCESS_TFTP) == 
	    FALSE) {
	    return(FALSE);
	}
    }

    /*
     * Before we go open the file, and possibly erase out the flash,
     * check if the file is accessible from the server.
     */
    if (fparms->verbose) {
        printf("\nAccessing file '%s' on %s...", fparms->sfname, 
                name_addr2string(&fparms->addr));
        flush();
    }
    /*
     * Call the TFTP function to attempt the test read.
     */
    if (!invoke_net_to_flash(fparms, TFTP_MAXDATA*2, TRUE)) {
        if (fparms->verbose) {
            printf(" [failed]\n");
        }
        return (FALSE);
    }

    /*
     * If the test access succeeded and we are verbose, say OK
     */
    if (fparms->verbose) {
        printf(" [OK]\n");
    }

    if (!flash_download_start(fparms, &fs_info, "TFTP"))
        return (FALSE);

    GET_TIMESTAMP(fparms->start_time);
    /*
     * Now do the actual TFTP transfer into flash.
     */
    if (!(tftp_okay = invoke_net_to_flash(fparms, fs_info.free, FALSE))) {
        flash_copy_abnormal_terminate(fparms);  /* Invalidate file */
        if (fparms->verbose)
            printf(flash_net_err_msg, 
                   flash_net_protocols[fparms->copy_option],
                   "from", name_addr2string(&fparms->addr));
    }

    flash_copy_end(fparms);
    return (tftp_okay);
}

/*
 * Copy a file into flash from a MOP server.
 */
boolean copy_mop_flash (flash_copy_parms_t *fparms)
{
    fsstat_t fs_info;
    struct flash_write_args flash_wargs; 
    filetype mop_file;                
    boolean header_write_status; 

    fparms->copy_option = FILE_ACCESS_MOP;
    if (!flash_copy_setup(fparms, &fs_info))
        return (FALSE);

    /* get file names */
    if (!flash_copy_get_names(fparms))
        return (FALSE);
    flash_create_path(fparms, fparms->dfname);

    /* Need to find physical start address of flash */
    fsstat(fparms->devnm, &fs_info);
    
    fparms->datalen = 0;
    fparms->buflen = 3*TFTP_MAXDATA;

    /* 
     * check to see if the src/desination length too big
     * only an issue when running flh
     */
    if (fs_info.status == DEV_STATE_RFF && reg_invoke_flh_version() > 0) {
	if (reg_invoke_flh_check_parms(fparms->sfname, fparms->path, 
				       fparms->verbose, FILE_ACCESS_MOP) == 
	    FALSE) {
	    return(FALSE);
	}
    }
    /*
     * Before we go open the file, and possibly erase out the flash,
     * check if the file is accessible from the MOP server.
     * We have to disable this feature until we figure out how to get
     * it working. Once a program load has been started, there doesn't
     * seem a way to terminate the load after one or two blocks,
     * except by letting the server time out. Any new assistance
     * volunteer messages are ignored by the server till the time out.
     *****************
    if (fparms->verbose) {
        printf("\nAccessing file '%s' on MOP server...", fparms->sfname);
        flush();
    }
     * Call the MOP function to attempt the read.
     *
    fparms->sts = 
        reg_invoke_file_read(mop_file.proto, &mop_file, fparms->buff,
                             &fparms->buflen, flash_wargs.verbose, 
                             flash_dummy_write, 
                             (void *)&flash_wargs);

    flash_decode_file_read_status(&fparms->sts, &fparms->snmp_sts);

    if (fparms->sts <= 0) {
        if (flash_wargs.len != 3*TFTP_MAXDATA)
            return (FALSE); 
    }

     *****************
     *
     */

    if (!flash_download_start(fparms, &fs_info, "MOP"))
        return (FALSE);

    GET_TIMESTAMP(fparms->start_time);
    /*
     * Now do the actual MOP transfer into flash.
     */
    flash_wargs.verbose = (fparms->verbose ? TRUE : FALSE);
    flash_wargs.buf = (char *)&fparms->buff;
    flash_wargs.fh = fparms->fh;
    flash_wargs.len = 0;
    flash_wargs.prev_len = 0;
    flash_wargs.relocatable = FALSE; 	/* Not relocatable, by default */
    flash_wargs.blocknumber = 1;	

    /* Set all pointers to NULL */
    flash_wargs.headers_block = NULL;
    flash_wargs.a_out_hdr = NULL; 
    flash_wargs.sp_hdr = NULL;
    flash_wargs.ri_info = NULL;
    
    /* Need to find physical start address of flash */
    fsstat(fparms->devnm, &fs_info);
    flash_wargs.file_sys_info = &fs_info;
    
    mop_file.filename = (char *)&fparms->sfname;
    mop_file.proto = FILE_ACCESS_MOP;
    mop_file.ps.mop.interface = NULL;
    mop_file.ps.mop.has_addr = FALSE;
    ieee_zero(mop_file.ps.mop.bootaddr);
    ieee_zero(mop_file.ps.mop.respondaddr);

    fparms->datalen = 0;
    flash_wargs.fh = fparms->fh;
    fparms->buflen = fs_info.free;
    fparms->sts = 
        reg_invoke_file_read(mop_file.proto, &mop_file, fparms->buff,
                             &fparms->buflen, flash_wargs.verbose, 
                             net2flash_write, 
                             (void *)&flash_wargs);

    /* If the image is relocatable, write headers to flash file */
    header_write_status = reloc_image_header_write(&flash_wargs);
    reloc_image_transfer_cleanup(&flash_wargs);
    if (!header_write_status) {
	flash_copy_abnormal_terminate(fparms);
	flash_copy_end(fparms);
	fparms->snmp_sts = SNMP_FILE_TRANSFER_ERR;
	return(FALSE);
    }

    /* decode the return value */
    flash_decode_file_read_status(&fparms->sts, &fparms->snmp_sts);

    if (fparms->sts <= 0) { /* failure */
        flash_copy_abnormal_terminate(fparms);
        if (fparms->verbose)
            printf("\nCopy failed");
    }

    flash_copy_end(fparms);
    return (fparms->sts > 0);
}

/*
 * Copy a file from flash to a tftp server
 */
boolean copy_flash_tftp (flash_copy_parms_t *fparms)
{
    ipsocktype *soc;

    fparms->copy_option = FILE_ACCESS_TFTP;
    if (!flash_read_setup(fparms))
        return (FALSE);

    /*
     * Get IP address of tftp server
     */
    if (fparms->addr.ip_addr == IPADDR_ZERO)
        address_copy(&fparms->addr, &hostip_flash);
    if (!flash_copy_get_server(&fparms->addr, (char *)fparms->buff,
                               hostip_flash, fparms->prompt)) 
        return (FALSE);
    
    /* save host address for next time default */
    address_copy(&hostip_flash, &fparms->addr); 

    /* Get file names. Create path using source file name. */
    if (!flash_copy_get_names(fparms))
        return (FALSE);
    flash_create_path(fparms, fparms->sfname);

    /*
     * Open socket for writing. Make write request to server.
     * If all successful, start download.
     */
    if (!(soc = flash_upload_start(fparms)))
        return (FALSE);

    GET_TIMESTAMP(fparms->start_time); /* note start time */

    invoke_tftp_from_flash(fparms, soc);
    if (fparms->verbose)
        printf("\nUpload to server done");

    reg_invoke_ip_close_socket(soc);
    flash_copy_end(fparms);
    return (TRUE);
}

/*
 * verify the checksum of a file
 */
boolean flash_file_checksum_verify (flash_copy_parms_t *fparms)
{
    int fh;
    char path[FLASH_PATH_MALLOC_LEN];

    if (!flash_read_setup(fparms))
	return (FALSE);

    if (!flash_copy_file_name(fparms->sfname, (FLASH_FILE_NAME_LEN - 1),
			      fparms->prompt ? "Name of file to verify? " :
			      NULL, TRUE)) {
	fparms->snmp_sts = SNMP_INVALID_SRC_FILE;
        return (FALSE);
    }
    flash_create_path(fparms, fparms->sfname);

    /*
     * Don't open files whose checksum we know is bad.
     */
    strcpy(path, fparms->path);
    fparms->fh = file_open(path, FILE_FLAGS_RDONLY |
                                         FILE_FLAGS_CHECKSUM |
                                         fparms->verbose | fparms->prompt);

    if (fparms->fh < 0) {  /* File not found */
        if (fparms->verbose)
            printf("\nERR: file '%s' - %s",
                    fparms->sfname, file_error_msg(fparms->fh));
	fparms->snmp_sts = SNMP_FILE_CHECKSUM_ERR;
	/* Check if file not found or checksum bad */
        strcpy(path, fparms->path);
	fh = file_open(path, FILE_FLAGS_RDONLY | fparms->verbose |
				     fparms->prompt);
	if (fh < 0) 
	    fparms->snmp_sts = SNMP_FILE_OPEN_ERR;
	else
	    file_close(fh);
        return (FALSE);
    }

    file_close(fparms->fh);
    return (TRUE);
}



uint flash_size (char *devnm)
{
    int sts;
    fsstat_t fs;

    sts = fsstat(devnm, &fs);
    return ((sts < 0) ? 0 : fs.size);
}

uint flash_free_space (char *devnm)
{
    int sts;
    fsstat_t fs;

    sts = fsstat(devnm, &fs);
    return ((sts < 0) ? 0 : fs.free);
}

int flash_file_count (char *devnm)
{
    int sts;
    fsstat_t fs;

    sts = fsstat(devnm, &fs);
    return((sts < 0) ? 0 : fs.totcnt);
}
 
/*
 **************  Flash file level services **********************
 */

/*
 * flash_file_match
 * Check if the two file names given are equivalent (as opposed to
 * identical). If they are indeed equivalent, declare that they match.
 * The first parameter refers to the filename in the database of
 * files that have been configured for tftp serving and the second
 * parameter refers to the filename in the tftp request.
 */
boolean flash_file_match (flash_copy_parms_t *fparms_dbase,
                          flash_copy_parms_t *fparms_req)
{
    char *colonptr_dbase, *colonptr_req;

    /* The file name part should match exactly */
    if (strcasecmp(fparms_dbase->sfname, fparms_req->sfname))
        return (FALSE);

   /*
    * If both file names specified partition numbers, then the devnm
    * part of fparms should also match exactly.
    */
    colonptr_dbase = strchr(fparms_dbase->devnm, FLASH_PATH_SEPARATOR);
    colonptr_req = strchr(fparms_req->devnm, FLASH_PATH_SEPARATOR);
    if (colonptr_dbase && colonptr_req)
        return(!strcasecmp(fparms_dbase->devnm, fparms_req->devnm));

   /* Now, check if the incoming request explicitly specified a partition
    * number. If so, we need to honor that.
    */
    if (colonptr_req)
        return(flash_locate_and_open_file(fparms_req));

   /*
    * Since incoming request does not care about which partition the file is
    * in, we should look for an exact match in just the device part of devnm.
    */
    if (colonptr_dbase)
        *colonptr_dbase = '\0';
    return(!strcasecmp(fparms_dbase->devnm, fparms_req->devnm));
}


/*
 * Fill in protocol specific information when parsing a file specification
 * Called by SERVICE_FILE_PARSE, index is FILE_ACCESS_FLASH
 */

void flash_file_parse (const char *keyword, filetype *file, parseinfo *csb)
{
    char *bufptr;

    if (csb->nvgen) {
	nv_add(TRUE, "%s %s", keyword, file->filename);
        /*
         * Now add boot command to ROM monitor BOOT variable.
         */
        bufptr = reg_invoke_monvar_buff(ROMMON_BOOT_VAR);
        if (bufptr) {
            bufptr += strlen(bufptr);
            sprintf(bufptr, "flash:%s,%d;", file->filename, 1);
        }
	return;
    }
}
    

/***----------------------- RCP stuff -----------------------------****/
/*
 * Copy a file into flash from a RCP server.
 */
boolean copy_rcp_flash (flash_copy_parms_t *fparms)
{
    fsstat_t fs_info;
    boolean rcp_okay = FALSE;

    fparms->copy_option = FILE_ACCESS_RCP;
    if (!flash_copy_setup(fparms, &fs_info))
        return (FALSE);

    /*
     * Get IP address of rcp server.
     */
    if (fparms->addr.ip_addr == IPADDR_ZERO)
        address_copy(&fparms->addr, &hostip_flash);
    if (!flash_copy_get_server(&fparms->addr, (char *)fparms->buff,
                               hostip_flash, fparms->prompt))
        return (FALSE);  /* Unknown host; give up */
    /* save host address for next time default */
    address_copy(&hostip_flash, &fparms->addr); 

    /* get file names */
    if (!flash_copy_get_names(fparms))
        return (FALSE);
    flash_create_path(fparms, fparms->dfname);

    /* 
     * check to see if the src/desination length too big
     * only an issue when running flh
     */
    if (fs_info.status == DEV_STATE_RFF && reg_invoke_flh_version() > 0) {
	if (reg_invoke_flh_check_parms(fparms->sfname, fparms->path, 
				       fparms->verbose, FILE_ACCESS_RCP) == 
	    FALSE) {
	    return(FALSE);
	}
    }
    /*
     * Before we go open the file, and possibly erase out the flash,
     * check if the file is accessible from the server.
     */
    if (fparms->verbose) {
        printf("\nAccessing file '%s' on %s...", fparms->sfname, 
                name_addr2string(&fparms->addr));
        flush();
    }
    /*
     * Call the network download function to attempt the test read.
     */
    if (!invoke_net_to_flash(fparms, TFTP_MAXDATA*2, TRUE)) {
        if (fparms->verbose) {
            printf(" [failed]\n");
        }
        return (FALSE);
    }

    /*
     * If the test access succeeded and we are verbose, say OK
     */
    if (fparms->verbose) {
        printf(" [OK]\n");
    }

    if (!flash_download_start(fparms, &fs_info, "TFTP"))
        return (FALSE);

    GET_TIMESTAMP(fparms->start_time); /* note start time */
    /*
     * Now do the actual download into flash.
     */
    if (!(rcp_okay = invoke_net_to_flash(fparms, fs_info.free, FALSE))) {
        flash_copy_abnormal_terminate(fparms);  /* Invalidate file */
        if (fparms->verbose)
            printf(flash_net_err_msg, 
                   flash_net_protocols[fparms->copy_option],
                   "from", name_addr2string(&fparms->addr));
    }

    flash_copy_end(fparms);
    return (rcp_okay);
}

typedef struct flash2rcp_context_ {
    int    fh;
    ushort cksum;
    uchar  *buffer;
} flash2rcp_context;

static long flash2rcp_callback (uchar **destination, 
                                long bytes_to_read, void *arg)
{
    flash2rcp_context *c = (flash2rcp_context *)arg;

    if (psipending())  /* Interrupted; abort */
	return(0);

    *destination = c->buffer;  /* restore buffer start */

    bytes_to_read = min(bytes_to_read, TFTP_MAXDATA);
    return (file_read(c->fh, c->buffer, bytes_to_read));
}

/*
 * copy_flash_rcp
 * Copy from Flash to RCP server.
 */
boolean copy_flash_rcp (flash_copy_parms_t *fparms)
{
    ipsocktype *soc;
    boolean rcp_okay;
    filetype rcp_params;
    flash2rcp_context context;
    fstat_t fs;

    fparms->copy_option = FILE_ACCESS_RCP;
    if (!flash_read_setup(fparms))
        return (FALSE);

    /*
     * Get IP address of rcp server
     */
    if (fparms->addr.ip_addr == IPADDR_ZERO)
        address_copy(&fparms->addr, &hostip_flash);
    if (!flash_copy_get_server(&fparms->addr, (char *)fparms->buff,
                               hostip_flash, fparms->prompt))
        return (FALSE);
    /* save host address for next time default */
    address_copy(&hostip_flash, &fparms->addr); 

    /* Get file names. Create path using source file name. */
    if (!flash_copy_get_names(fparms))
        return (FALSE);
    flash_create_path(fparms, fparms->sfname);

    /*
     * Open socket for writing. Make write request to server.
     * If all successful, start download.
     */
    if (!(soc = flash_upload_start(fparms)))
        return (FALSE);

    context.fh    = fparms->fh;
    context.cksum = 0;
    context.buffer = fparms->buff;

    if (file_stat(fparms->fh, &fs) < 0) { /* File stat problem */
	fparms->snmp_sts = SNMP_FILE_OPEN_ERR;
        return (FALSE);
    }

    GET_TIMESTAMP(fparms->start_time); /* note start time */

    rcp_params.ps.tuba.username = fparms->rcp_username;
    rcp_params.ps.tuba.display = (fparms->verbose ? TRUE : FALSE);

    /*
     * reg_invoke_rcp_write(remote dest file name, remote host ip addr,
     *                      data buffer, number of bytes to send,
     *                      filetype to supply remote username for scope of operation,
     *                      call-back func, call-back func parameters)
     */
    rcp_okay = reg_invoke_rcp_write(fparms->dfname, 
                                    (addrtype *)&fparms->addr, 
                                    (char *)fparms->buff, fs.len,
				    &rcp_params, 
                                    (void *)flash2rcp_callback, 
                                    (void *)&context);

    if (fparms->verbose) {
        if (rcp_okay)
            printf("\nUpload to server done");
        else
            printf("\nUpload to server failed/aborted");
    }

    if (!rcp_okay)
	fparms->snmp_sts = SNMP_FILE_TRANSFER_ERR;

    flash_copy_end(fparms);
    return (rcp_okay);
}

/*
 * flash2rcp_remote
 * Just like flash2rcp, but the rcp was initiated by the remote system.
 */
boolean flash2rcp_remote (flash_copy_parms_t *fparms, void *mumble)
{
    flash2rcp_context context;
    tcbtype *tcb;
    fstat_t fs;
    int rcp_sts;

    tcb = (tcbtype *)mumble;

    if (strlen(fparms->devnm) == 0) /* no device */
        return (FALSE);

    if (strlen(fparms->sfname) == 0) /* no file name */
        return (FALSE);

    fparms->verbose = 0;  /* work silently */
    fparms->prompt  = 0;  /* nothing to prompt for */
    fparms->erase   = 0;  /* we are only reading flash */
    flash_create_path(fparms, fparms->sfname);

    /*
     * Open file in read-only mode and only if checksum is good.
     */
    fparms->fh = file_open(fparms->path, FILE_FLAGS_RDONLY | 
                                         FILE_FLAGS_CHECKSUM);

    if (fparms->fh < 0)   /* File not found */
        return (FALSE);

    context.fh    = fparms->fh;
    context.buffer = fparms->buff;
    context.cksum = 0;

    if (file_stat(fparms->fh, &fs) < 0) /* File stat problem */
        return (FALSE);

    GET_TIMESTAMP(fparms->start_time); /* note start time */

    rcp_sts = reg_invoke_rcp_send(tcb, fparms->dfname, fparms->buff, 
                                  fs.len, flash2rcp_callback,
                                  (void *) &context, FALSE);

    flash_copy_end(fparms);
    return (rcp_sts == ERROR_OK);
}
/*
 * write_file_to_flash
 * write a buffer to flash as a file
 */
boolean write_file_to_flash (flash_copy_parms_t *fparms, void *tcb, 
			     uchar *buffer, ulong buflen)
{
    struct flash_write_args fa;
    fsstat_t fs_info;
    boolean header_write_status = FALSE; /* pessimism */
    boolean rcp_okay = TRUE;  /* optimism */

    fparms->copy_option = FILE_ACCESS_RCP;
    fparms->verbose = 0;  /* work silently */
    fparms->prompt  = 0;  /* nothing to prompt for */
    fparms->erase   = 0;  
    if (!flash_copy_setup(fparms, &fs_info))
        return (FALSE);

    flash_create_path(fparms, fparms->sfname);

    /*
     * Open flash file.
     */
    if (!flash_download_start(fparms, &fs_info, "RCP"))
        return (FALSE);

    fa.fh = fparms->fh;
    fa.buf = buffer;
    fa.len = 0;
    fa.prev_len = 0;
    fa.relocatable = FALSE;
    fa.blocknumber = 1;

        /* Set all pointers to NULL */
    fa.headers_block = NULL;
    fa.a_out_hdr = NULL;
    fa.sp_hdr = NULL;
    fa.ri_info = NULL;
    fa.fh = fparms->fh;
    /* Need to find physical start address of flash */
    fa.file_sys_info = &fs_info;

    fparms->datalen = 0;
    fparms->buflen = buflen;

    GET_TIMESTAMP(fparms->start_time);

    /*
     * Now do the actual download into flash.
     */
    fparms->sts = reg_invoke_rcp_receive(tcb, buffer, &buflen, FALSE,
				         net2flash_write, (void *)&fa);


    /*
     * If the image is relocatable, write headers to flash file
     */
    header_write_status = reloc_image_header_write(&fa);
    reloc_image_transfer_cleanup(&fa);
    if (!header_write_status) {
	flash_copy_abnormal_terminate(fparms);
	flash_copy_end(fparms);
	return(FALSE);
    }

    flash_decode_file_read_status(&fparms->sts, &fparms->snmp_sts);

    if (fparms->sts <= 0) {
        flash_copy_abnormal_terminate(fparms);  /* Invalidate file */
        rcp_okay = FALSE;
    }

    flash_copy_end(fparms);
    return (rcp_okay);
}

/*
 * This function opens the specified file in flash and
 * serves out one block at a time, until either end-of-file
 * or callback function asks for termination.
 */
boolean flash_block_server (flash_copy_parms_t *fparms, uchar *buffer,
                            ulong *buffersize, 
                            boolean (*func)(uchar **buf, long *bytesleft,
                                            long len, void *opaque_args),
                            void *func_arg)
{
    int len;
    boolean okay;
    long blocksize, bytesleft;
    fstat_t file_info;

    if (*buffersize == 0)  /* No buffer ?? */
        return (FALSE);

    blocksize = min(TFTP_MAXDATA, *buffersize);

    flash_create_path(fparms, fparms->sfname);

    fparms->fh = file_open(fparms->path, FILE_FLAGS_RDONLY | 
                                         FILE_FLAGS_CHECKSUM);
    if (fparms->fh < 0) {  /* File open failure */
        return (FALSE);
    }
    if (file_stat(fparms->fh, &file_info) < 0) {
        file_close(fparms->fh);
        return (FALSE);
    }
    bytesleft = file_info.len; /* Bytes left = file length */

    /*
     * Read a block of flash data and ship it out.
     * Loop until end of file or network write error
     * or callback function asks for termination.
     */
    okay = TRUE;  /* Get the loop going */
    while (okay) {

        len = file_read(fparms->fh, buffer, blocksize);

        bytesleft -= len;

        if (func)
            okay = (*func)(&buffer, &bytesleft, len, func_arg);

        if (len < blocksize) /* last block read */
            break;
    }
    file_close(fparms->fh);
    return(okay);
}
/*
 * wrapper for getting partitioned filename for lex.
 */
boolean multi_part_get_file_name (char *filename)
{
    static flash_copy_parms_t fparms;

    fparms.prompt = TRUE;
    fparms.verbose = TRUE;
    fparms.path[0] = '\0';
    fparms.devnm[0] = '\0';
    fparms.sfname[0] = '\0';
    fparms.dfname[0] = '\0';
    strcpy(fparms.devnm,reg_invoke_flash_device_default());
    /* prompt for partition info */
    if (flash_read_setup(&fparms)) {
	/* Get file names. Create path using source file name. */
	if (!get_source_file(filename, FLASH_FILE_NAME_LEN - 1,
			     "copy", TRUE)) {
	    return(FALSE);
	}
	flash_create_path(&fparms, filename);
	flash_copy_end(&fparms);
	strcpy(filename,fparms.path);
	return(TRUE);
    } else {
	return(FALSE);
    }
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

/* Number of flash devices in the system */
int flash_device_count (void)
{
    int num_devices = 0;
    int i;

    for (i=0; i <= MAX_DEV_INIT_ENTRIES; i++)
        if (phy_dev_database[i])
            num_devices ++;
    return (num_devices);
}

/* Check if the device name is valid */
boolean flash_device_is_valid(char *devnm)
{
    char *colon;
    int i;

    /* Now look for a colon in the device name string */
    colon = strchr(devnm, FLASH_PATH_SEPARATOR);
    if (colon)
        *colon++ = 0;  /* term "<dev>" */

    for (i=0; i <= MAX_DEV_INIT_ENTRIES; i++) {
	if ((strcmp(phy_dev_database[i], devnm)) == 0) 
	    return (TRUE);
    }
    return (FALSE);
}

/***********************************************************************
 * Functions to support SNMP queries for partition related information.
 ***********************************************************************/
long flash_partition_free_space (char *devnm)
{
    fsstat_t fs_info;
    if (fsstat(devnm, &fs_info) < 0)
        return (0);
    return(fs_info.free);
}

long flash_partition_filecount (char *devnm)
{
    fsstat_t fs_info;
    if (fsstat(devnm, &fs_info) < 0)
        return (0);
    return(fs_info.totcnt);
}

long flash_partition_chksum_algo (char *devnm)
{
    fsstat_t fs_info;
    if (fsstat(devnm, &fs_info) < 0)
        return D_ciscoFlashPartitionChecksumAlgorithm_undefined;
    return(fs_info.chksum_algo);
}

long flash_partition_upgrade_method (char *devnm)
{
    fsstat_t fs_info;
    if (fsstat(devnm, &fs_info) < 0)
        return D_ciscoFlashPartitionUpgradeMethod_unknown;
    return(fs_info.copy_mode);
}

long flash_partition_need_erasure (char *devnm)
{
    fsstat_t fs_info;
    if (fsstat(devnm, &fs_info) < 0)
        return (TRUE); /* return true by default */
    return(fs_info.ersf);
}

long flash_partition_filename_length (char *devnm)
{
    fsstat_t  fs_info;
    if (fsstat(devnm, &fs_info) < 0)
        return (0);
    return(fs_info.filenm_length);
}


/*
 * Initialize stuff relevant to all flash services
 */
static void flash_services_init (subsystype *subsys)
{
    flash_copy_server_addr_init(&hostip_flash);
}


/*
 * Flash subsystem header for generic software support
 */

#define FLASH_MAJVERSION 1
#define FLASH_MINVERSION 0
#define FLASH_EDITVERSION  1

SUBSYS_HEADER(flash_services,FLASH_MAJVERSION, FLASH_MINVERSION, 
              FLASH_EDITVERSION, flash_services_init, SUBSYS_CLASS_LIBRARY,
	      NULL, NULL);

