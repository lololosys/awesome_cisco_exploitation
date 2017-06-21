/* $Id: flash_private.h,v 3.3.60.2 1996/08/28 12:40:29 thille Exp $
 * $Source: /release/112/cvs/Xsys/dev/flash_private.h,v $
 *------------------------------------------------------------------
 * flash_private.h - private defs/decl for ags+ flash support 
 *                   (for flash.c only - wholly copied from flash.h)
 *
 * 18-December-1990, Chris Shaker
 * April 1991, Steve Elias
 * Octoberfest-1991, Darren Kerr
 * February 1993, Dit Morse
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_private.h,v $
 * Revision 3.3.60.2  1996/08/28  12:40:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.60.1  1996/07/12  02:00:48  widmer
 * CSCdi52582:  parser should remove the trailing spaces in the filename
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/17  09:01:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:15:32  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:19:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/18  19:40:41  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:24:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 *------------------------------------------------------------------
 */

#define FLASH_HDR_MAGIC        0xABBADABA
#define FLASH_FHDR_MAGIC       0xBAD00B1E

#define MAX_RETRIES 20 

#define MC_TEST_PATTERN		0x89ABCDEFL

#define MC_NVRAM_ADDRESS	0x0		/* NVRAM base address */
#define MC_NVRAM_SIZE		32768		/* 32K bytes NV memory */ 
#define MC_VPP_CONTROL		0x7fe000	/* MC+ VPP Control register */
#define MC_ROM_ADDRESS		0x800000
#define MC_ROM0			MC_ROM_ADDRESS
#define MC_ROM8			0xC00000	/* ROM8 */
#define MAX_ROMS		16
#define MAX_FLASH_BANKS         4
#define MAX_ROM_MFGS	        5
#define MAX_ROM_ADDR            0x800000       /* 8MB max addressable */
#define MCPLUS_MAX_SIZE         0x400000       /* 4MB max */

/*
 * Only one bit is defined in the CPP control register on the MC+:
 */
#define VPP_ON		0x0001
#define VPP_OFF		0x0000
#define VPP_MASK	0x0001

/*
 * Definitions for MC tests:
 */
#define MC_MAXIN		10
#define MC_DEF_TIMES		1
#define MC_DEF_START		0x0
#define MC_DEF_LEN		0xE00000
#define MC_DEF_DATA		0x1234


/*
 * Intel Flash EPROM command codes:
 */
#define FLASH_READ_CMD	0
#define FLASH_INFO_CMD	0x90
#define FLASH_ERASE_CMD	0x20
#define FLASH_ERASE_VER	0xA0
#define FLASH_PROG_CMD	0x40
#define FLASH_PROG_VER	0xC0
#define FLASH_RESET_CMD	0xFF


/*
 * Round up flash header size to 64 bytes.
 */
#define FLHDR_RESERVED (64 - sizeof(ulong) - MAX_ROMS * sizeof(ushort))

/*
 * This is written to the header of flash the first
 * time it's programmed.  This enables software to
 * read the size and types of flash roms installed
 * after the security jumper is installed.
 * (The security jumper prevents VPP from being asserted,
 * so the ROM ID flash command would not work).
 */
typedef struct flash_header {
    ulong   fh_magic;
    ushort  fh_rom_id[MAX_ROMS];
    char    fh_reserved[FLHDR_RESERVED];
} flash_header;


/*
 * This struct prepends each file written to flash.
 */
#define FLASH_NAME_LEN (64 - 3 * sizeof(ulong) - 2 * sizeof(ushort))
typedef struct flash_file_hdr {
    ulong   ffh_magic;
    ulong   ffh_len;
    ushort  ffh_checksum;
    short   ffh_flags;
    long    ffh_reserved;
    char    ffh_name[FLASH_NAME_LEN];
} flash_fhdr;

/*
 * Structure used by callback routine tftp2flash_write_ok
 */
struct flash_write_args {
    flash_fhdr *ffp;
    ulong dev;
    uchar *buf;
    boolean verbose;
};

#define FFH_FLAGS_DELETED    0x0001

#define FLASH_HDR_SIZE sizeof(flash_header)
#define FLASH_FHDR_SIZE sizeof(flash_fhdr)
#define FLASH_OVERHEAD(files) (FLASH_HDR_SIZE + ((files) * FLASH_FHDR_SIZE))
#define FLASH_AVAILABLE(files) (flash.f_size - FLASH_OVERHEAD(files))
#define MIN_FILE_SIZE ((32 * 1024) + sizeof(flash_fhdr))

#define FLASH_FILE_CNT 15   /* maximum files we'll support - includes aborts */

struct flash_t_ {
    ulong   f_cur;          /* current logical offset from start of ROM   */
    ulong   f_size;         /* size in bytes of flash roms                */
    ulong   f_maxseek;      /* maximum seek offset                        */
    ulong   f_chipsize;     /* size of one flash roms                     */
    ulong   f_chipcount;    /* number of flash roms                       */
    ulong   f_free_offset;  /* current write pointer offset               */
    int     f_eraseval;     /* Erase length verify                        */
    boolean f_infoed;       /* got size/mfg info?                         */
    boolean f_vpp_on;       /* VPP on for flash write/erase               */
    boolean f_vpp_works;    /* VPP and INFO commands worked during init   */
    boolean f_verbose;      /* print error messages                       */
    boolean f_erase_needed; /* Is erasure needed in order to write another*/
    boolean f_proc_level;   /* System's running, so edisms instead of DELAY*/
    ulong   f_dev;          /* Index of driver                            */
    char    *f_devname;     /* Device driver name                         */
    char    *f_card;        /* MC+ or Clean                               */
    char    *f_ctlr;        /* ENVM or MCI                                */
    char    f_info_idx[MAX_ROMS];/* Index of flash info in table          */
    char    f_filecnt;      /* Number of files written                    */
    char    f_banks;        /* Number of ROM banks (MC+==2)               */
    flash_header f_hdr;     /* Last read flash header                     */
    flash_fhdr   f_fhdrs[FLASH_FILE_CNT];  /* Last read file headers      */
};

/*
 * Manufacturer codes table:
 */

struct flash_info {
    ushort manu_dev_code;	/* Manufacturer and device code */
    long device_size;		/* Size of device (bytes) */
    char *name;			/* Name string */
};

/*
 * mcdvr.c
 */
#ifndef MIN
#define MIN(x,y)  ((x)<(y)?(x):(y))
#endif 


/*
 * Prototypes:
 *
 * flash.c
 */
extern void flash_print_hardware(void);
extern void flash_print_chips(void);
extern int net2flash(int , ulong, addrtype *, const char *, const char *, uchar *,
		     boolean);
extern int flash2tftp(ulong, addrtype *, const char *, const char *, uchar *);
extern void flash2rcp(ulong, addrtype *host, const char *remname, const char *locname);
extern int flash_read_file(const char *, uchar *, ulong *, boolean);
extern int flash_delete_file (int dev, const char *filename);
extern const char *flash_bootfile(void);
extern int flash_read_block(int, uchar *, int) ;
extern ushort flash_file_cks(const char *);
extern int flash_verify(int, flash_fhdr *, char *, boolean);
extern boolean flash_probe(ulong);
extern int flash_erase_all(ulong);
extern boolean flash_query_roms(int);
extern int flash_info_index(ushort);
extern void flash_read_fhdrs(int);
extern flash_fhdr *flash_get_fhdr(ulong, const char *);
extern boolean flash_vpp_set(ulong, int);
extern void flash_delay(ulong);
extern int flash_open(void);
extern ushort add_to_cksum(ushort, ushort);
extern void erase_command(void);
extern int set_usec_delay(void);

/*
 * Prototypes:
 *
 * mcdvr.c
 */
extern boolean mc_program_rom(ushort , uint );
extern ushort mc_read_short(uint );
extern void mc_write_short(ushort , uint );
