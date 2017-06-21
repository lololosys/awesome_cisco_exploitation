/* $Id: flash_dvr_spec.h,v 3.2.60.2 1996/08/02 22:52:53 smackie Exp $
 * $Source: /release/112/cvs/Xsys/dev/flash_dvr_spec.h,v $
 *------------------------------------------------------------------
 * flash_dvr_spec.h - public interface spec for flash_dvr
 *
 * February 1993, Dit Morse
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_dvr_spec.h,v $
 * Revision 3.2.60.2  1996/08/02  22:52:53  smackie
 * Attempt to kill two types.h birds with one stone. (CSCdi64993)
 *
 *    o Add typedefs for size-oriented intX and uintX types
 *    o Rename dev_t to flash_dev_t to avoid near-future conflicts
 *
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  19:12:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  21:18:53  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:00:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:19:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:24:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 *------------------------------------------------------------------
 */

#define FLASH_HDR_MAGIC        0xabbabeef /* for structure below */

#define FLASH_FHDR_MAGIC       0xbad00b1e /* file header magic */

/* flash parameters */

#define FLASH_HDR_SIZE  64       /* these are fixed size */
#define FLASH_FHDR_SIZE 64

#define MAX_ROMS 16

#define FLASH_OVERHEAD(files) (FLASH_HDR_SIZE + ((files) * FLASH_FHDR_SIZE))
#define FLASH_AVAILABLE(files) (flash.f_size - FLASH_OVERHEAD(files))
#define MIN_FILE_SIZE ((32 * 1024) + sizeof(flash_fhdr))

#if defined(C7000)
#define FLASH_FILE_CNT 30   /* maximum files we'll support - includes aborts */
#else
#define FLASH_FILE_CNT 15   /* maximum files we'll support - includes aborts */
#endif

#define FLASH_NAME_LEN (64 - 2 * sizeof(ulong) - 2 * sizeof(ushort) \
			- sizeof(uchar *))


/*
 * This struct prepends each file written to flash.
 */

typedef struct flash_file_hdr {
    ulong   ffh_magic;
    ulong   ffh_len;
    ushort  ffh_checksum;
    short   ffh_flags;
    uchar  *ffh_addr;   /* used only in memory, not on flash */
    char    ffh_name[FLASH_NAME_LEN];
} flash_fhdr;

struct flash_t_ {
    ulong    f_free_offset;  /* current write pointer offset               */
    int      f_eraseval;     /* Erase length verify                        */
    uchar    f_filecnt;      /* Number of files written                    */
    boolean  f_infoed;       /* got size/mfg info?                         */
    boolean  f_verbose;      /* print error messages                       */
    boolean  f_erase_needed; /* Is erasure needed in order to write another*/
    boolean  f_proc_level;   /* System's running, so edisms, not DELAY     */
    flash_dev_t   *f_dev;          /* pointer to driver data structure           */
    flash_fhdr f_fhdrs[FLASH_FILE_CNT];  
                             /* Last read file headers                     */
    uchar   *f_addr[FLASH_FILE_CNT];
                             /* address (offset) of the file               */
    ushort  f_cp_cksum[FLASH_FILE_CNT];
                             /* computed as file header is read            */
};

extern flash_t flash;

#define MAX_ROMS_LIST \
         (FLASH_HDR_SIZE - 2*(sizeof(ulong) + sizeof(ushort)))/sizeof(ushort)

typedef struct flash_header {
    ulong   fh_magic;
    ushort  fhx_banks; /* banks of flash represented   */
    ushort  fhx_width; /* width in number of bytes     */
    ulong   fhx_size;  /* total flash size in bytes    */
    ushort  fh_rom_id[MAX_ROMS_LIST]; /* entry for each possible roms */
} flash_header;

enum flash_status {
    FR_NOT_QUERIED,
    FR_NOT_VALID,
    FR_READ_ONLY,
    FR_READ_WRITE
    };

typedef struct flash_geom {
    ushort  fg_status;       /* query status */
    ulong   fg_chipsize;     /* size of one flash rom (bytes) */
    uint    fg_chipcount;    /* total number of chips */
    ulong   fg_flashbase;    /* address of first flash address */
    ulong   fg_banksize;     /* size of each bank of flash (bytes) */
    ulong   fg_size;         /* size of flash roms (bytes) */
    ulong   fg_size_cnfrm;   /* size confirmed after chip query */
    ulong   fg_erase_sector; /* erase sector size in bytes */
    uint    fg_sectors;      /* no of sectors in a chip */
    ushort  fg_cmnd_set;     /* command set to be used */
    uint    fg_banks;	     /* No of banks */
    uint    fg_width;        /* Width of a bank in # of bytes */
    ushort  fg_usec6;        /* count for 6 usec delay */
    ushort  fg_usec10;       /* count for 10 usec delay */
    int     fg_cache_set;    /* cache commands in use */
    char    *fg_devname;     /* Device driver name */
    char    *fg_card;        /* MC+, Clean, IGS */
    char    *fg_ctlr;        /* ENVM or MCI */
    boolean (*fg_program_rom)(                          /* write to flash */
			      ulong data, 
			      volatile ulong *ptr, 
			      ulong mask, 
			      int write_tries);  
    boolean (*fg_program_quad)(       /* fast write long aligned to flash */
			       ulong data, 
			       volatile ulong *ptr);  
    
    boolean (*fg_erase_bank)(                      /* earse bank of flash */
			     flash_dev_t *dev, 
			     int bank); 
    ushort  fg_rom_id[MAX_ROMS];           /* entry for each possible rom */
} flash_geom;

extern flash_geom fg;

/*
 * Manufacturer codes table:
 */

struct flash_info {
    ushort dev_code;     /* Manufacturer and device code(ordered logically) */
    ushort rev_dev_code; /* Manufacturer and device code(reversed) */
    char *manu_dev_code; /* as in the data sheets */
    ulong device_size;	 /* Size of device (bytes) */
    ulong sector_size;	 /* size of an erase sector */
    ushort cmnd_set;	 /* command set type (0=old, 1=new) */
    char *chip_name;	 /* Name of chip */
};

/* flash_dvr routines */

extern flash_dev_t *flash_dvr_init (void);
extern ulong flash_align(ulong addr);
extern boolean run_from_flash(void);
extern ushort flash_query_roms(flash_dev_t *dev);
extern void query_bank(int bankaddr, ulong *manu, ulong *device);
extern boolean flash_erase_bank(flash_dev_t *dev, int bank);
extern ulong flash_read(uchar *buf, uchar *addr, uint len);
extern ulong flash_write(uchar *buf, uchar *addr, ulong len, boolean verbose);
extern void flash_print_chips_known(void);
extern int flash_identify(ushort code);
extern char *flash_manu_code(uint i);
extern char *flash_name(uint i);
extern ulong flash_size(uint i);
extern uchar *flash_address (uchar *addr);
extern void flash_delay(ulong);

/* flash_les, flash_rp1, ... routines */

extern boolean flash_assert_vpp(void);
extern boolean flash_deassert_vpp(void);
extern flash_dev_t *flash_dev_init(void);
extern char *flash_socket(int i);
extern void flash_timer(ushort *v6, ushort *v10, boolean print_timer);
extern boolean sync_flash_header(flash_dev_t *dev);
