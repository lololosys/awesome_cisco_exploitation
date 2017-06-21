/* $Id: flash_services.h,v 3.2.60.3 1996/07/12 02:00:59 widmer Exp $
 * $Source: /release/112/cvs/Xsys/dev/flash_services.h,v $
 *------------------------------------------------------------------
 * flash_services.h -  Generic flash services.
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: flash_services.h,v $
 * Revision 3.2.60.3  1996/07/12  02:00:59  widmer
 * CSCdi52582:  parser should remove the trailing spaces in the filename
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/05/21  06:26:39  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.2.60.1  1996/05/07  00:38:04  ssangiah
 * CSCdi56357:  Misleading erase command behavior in the RSP file system.
 *   o Remove erase command from the RSP file system parse chain.
 *   o Clean up the RSP file system parse chain.
 *   o Fix problem with return of error codes by the flash MIB agent.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:01:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:20:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:54:21  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/29  00:27:36  enf
 * Revision 2.3  1995/08/25  11:43:30  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.2  1995/07/29 00:27:36  enf
 * CSCdi37707:  CPUHOG message while booting from flash
 * make flash read more efficient and suspend reads every 512K bytes
 *
 * Revision 2.1  1995/06/07  20:25:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __FLASH_SERVICES_H__
#define __FLASH_SERVICES_H__

#include "../os/reloc.h"
#include "../dev/file_io.h"

#define	MAX_FLASH_READ_COUNT		(512 * ONE_K)
/* flags for getting a partition number or display */
#define FLASH_DIR_DISPLAY		1
#define FLASH_GOT_PARTITION_NUM		2
#define FLASH_LOOP_FOR_ANSWER		3

/*
 * Structure used by TFTP and MOP callback routines. 
 */

#define FLASH_DUMMY_CHAR 0xff

/* The six different relocation table types */
enum RELOC_TABLES {
    TEXT_F_TEXT, 	/* Text references from text */
    DATA_F_TEXT,	/* Data references from text */
    BSS_F_TEXT, 
    TEXT_F_DATA,
    DATA_F_DATA, 
    BSS_F_DATA, 
    NUM_RELOC 
};

/* Different tftp block characteristics */
enum STATES {
    HEADER_DOWNLOAD,		/* A.out & special padded to tftp block		 */
    HEADER_PIC_DOWNLOAD,	/* Special header & PIC code			 */
    PIC_DOWNLOAD,		/* Downloading blocks from the PIC code		 */
    PIC_RELOC_DOWNLOAD,		/* Last of PIC & beginning of reloc tables	 */
    RELOC_DOWNLOAD,		/* Downloading Relocation Tables  		 */
    RELOC_TEXT_DOWNLOAD,	/* Last segment containing relocation tables	 */
    TEXT_DOWNLOAD,		/* Downloading text segment blocks 		 */
    TEXT_DATA_DOWNLOAD,		/* Contains end of text & beginning of data	 */ 
    DATA_DOWNLOAD,		/* Contains data segment blocks			 */
    DATA_TC_DOWNLOAD,		/* Data block containing text checksum		 */
    LAST_BLOCK_DOWNLOAD,	/* Last block of the image			 */
    UNKNOWN_STATE
};		

enum CHECKSUMS {
    OLD_IMG_CKSUM,  		/* Image checksum before relocation */
    RELOC_IMG_CKSUM,		/* Image checksum after relocation  */
    OLD_TXT_CKSUM,
    RELOC_TXT_CKSUM,
    ORIG_TXT_CKSUM,		/* Text checksum extracted from the */
				/* downloaded image itself 	    */
    NUM_CKSUMS
};
	
struct reloc_args {
    int   num_straddling_bytes;	/* The number of bytes of straddling addr  */
				/* within the current buffer		   */
    boolean reloc_tables_initialised;

    /* Current entry in relocation table being processed  */
    int current_pos_tables[NUM_RELOC]; 
    /* Sum total of diff. offsets upto current_pos_tables */
    ulong accumulated_offset[NUM_RELOC]; 

    ulong image_mem_base_addr;  /* The physical memory address in flash of */
				/* the start of the downloaded file	   */
    /* Image section lengths */
    ulong exec_hdr_end;
    ulong spl_hdr_end;
    ulong pic_end;
    ulong reloc_tables_end;
    ulong text_seg_end;
    ulong txt_cksum_end;
    ulong data_seg_end;
};

struct flash_write_args {
    int   fh;                	/* file handle */
    uchar *buf;              	/* read buffer pointer */
    ulong len;               	/* length of data written */
    ulong prev_len;
    boolean verbose;

    /* Fields added for relocatability */
    boolean relocatable;
    int     blocknumber;	/* TFTP block number of buffer buf         */
    fsstat_t *file_sys_info;	
    char     *headers_block;	/* Copy of the first (headers) block       */
    struct sun_exec *
             a_out_hdr;  
    struct special_header *
	     sp_hdr;
    struct reloc_args *	
	     ri_info;

    /* Flash memory pointers to relocation tables */
    RELOC_OFFSET_TYPE *		
	     reloc_tables[NUM_RELOC];
    ulong reloc_table_counts[NUM_RELOC];

    ulong new_text_link_addr;
    long  text_offset;		/* Offset to be applied to all   */
				/* text addresses                */

    /* Fields for checksumming */
    ulong cksums[NUM_CKSUMS]; 
    boolean txt_cksum_known;
    boolean spl_hdr_cksum_known;
    boolean image_cksum_known;
};

/*
 * data
 */
extern char* phy_dev_database[];

/*
 * Prototypes
 */
extern boolean flash_directory(const char *, int);
extern void run_from_flash_msg (void);
extern boolean flash_copy_setup (flash_copy_parms_t *, fsstat_t *);
extern boolean flash_read_setup (flash_copy_parms_t *);
extern boolean copy_tftp_flash(flash_copy_parms_t *);
extern boolean copy_mop_flash(flash_copy_parms_t *);
extern boolean copy_flash_tftp(flash_copy_parms_t *);
extern boolean flash_erase_command(flash_copy_parms_t *);
extern boolean flash_file_checksum_verify(flash_copy_parms_t *);

extern boolean flash_tftp_server(flash_copy_parms_t *, void *);
extern int flash_file_read(flash_copy_parms_t *, uchar *, ulong *, boolean);
extern const char *flash_first_bootable_file(flash_copy_parms_t *);
extern boolean flash_file_boot_in_place(flash_copy_parms_t *, 
                                        uchar **, ulong *);

extern boolean copy_rcp_flash(flash_copy_parms_t *);
extern boolean flash2rcp_remote(flash_copy_parms_t *fparms, void *);
extern boolean copy_flash_rcp(flash_copy_parms_t *);
extern void flash2rcp(uchar *buff, addrtype *host, char *remname, int fh);

extern boolean flash_available(char *);
extern uint flash_size(char *);
extern uint flash_free_space(char *);
extern int flash_file_count(char *);
extern boolean flash_status(char *);
extern uchar *flash_card_name(char *);
extern uchar *flash_ctlr_name(char *);

extern boolean flash_file_exists(flash_copy_parms_t *fparms);
extern boolean flash_file_match(flash_copy_parms_t *fparms1,
                                flash_copy_parms_t *fparms2);
extern boolean write_file_to_flash(flash_copy_parms_t *, void *, 
                                   uchar *, ulong);
extern boolean flash_copy_get_server(addrtype *host, char *bufp,
				     addrtype def_host, ulong prompt);
extern boolean flash_copy_file_name(char *fname, uint len,
				    const char *prompt, boolean verbose);
extern boolean flash_block_server(flash_copy_parms_t *, uchar *, 
                                  ulong *, 
                                  boolean (*func)(uchar **, long *, 
                                                  long, void *),
                                  void *);
extern void flash_file_parse(const char *, filetype *, parseinfo *);
extern const char *flash_bootfile(const char *);
extern boolean multi_part_get_file_name(char *);
extern boolean flash_get_partition_number(char *, int , char *);
extern boolean flash_locate_and_open_file (flash_copy_parms_t *);
void invoke_tftp_from_flash (flash_copy_parms_t *, ipsocktype *);
void flash_create_path (flash_copy_parms_t *, const char *);

/*
 * Prototypes for SNMP related functions 
 */

/* Device Information Group related functions */
extern int flash_init_dev_database(void);
extern boolean flash_make_devnm (int, int, char *);
extern ulong flash_dev_size (char *);
extern ulong flash_dev_min_partn_size (char *);
extern int flash_dev_max_partns (char *);
extern int flash_dev_num_partns (char *);
extern char *flash_dev_desc (char *);
extern char *flash_dev_cntlr (char *);
extern sys_timestamp flash_dev_init_t (char *);
extern int flash_dev_vpp (char *);
extern boolean flash_dev_removable (char *);
extern boolean flash_dev_removed (char *);
extern int flash_device_count(void);
extern int flash_device_chipcnt(char *devnm);
extern boolean flash_device_is_rw(char *devnm, int *status);
extern boolean flash_device_is_valid(char *devnm);
extern int flash_device_slot(char* devnm);

/* Chip Group related functions */
extern int flash_chip_code(char *devnm, int chipno);
extern char *flash_chip_desc(char *devnm, int chipnum);
extern ulong flash_chip_wretries(char *devnm, int chipnum);
extern ulong flash_chip_eretries(char *devnm, int chipnum);
extern int flash_chip_max_wretries(char *devnm);
extern int flash_chip_max_eretries(char *devnm);

/* Partition Information Group related functions */
extern long flash_partition_number(char *devnm);
extern void flash_partition_chip_bounds(char *devnm, long *startchip,
                                                        long *endchip);
extern long flash_partition_size(char *devnm);
extern long flash_partition_free_space(char *devnm);
extern long flash_partition_filecount(char *devnm);
extern long flash_partition_chksum_algo(char *devnm);
extern long flash_partition_status(char *devnm);
extern long flash_partition_upgrade_method(char *devnm);
extern char *flash_partition_name(char *devnm);
extern long flash_partition_need_erasure(char *devnm);
extern long flash_partition_filename_length(char *devnm);

/* File Group related functions */
extern int flash_file_index(char *, int);
extern int flash_file_size(char *, int);
extern ulong flash_file_chksum(char *devnm, int fileno);
extern boolean flash_file_status(char *, int);
extern uchar *flash_file_name(char *, int);

#endif /* __FLASH_SERVICES_H__ */
