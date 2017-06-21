/* $Id: fslib_internals.h,v 3.2.60.3 1996/04/22 18:58:16 ssangiah Exp $
 * $Source: /release/112/cvs/Xsys/filesys/fslib_internals.h,v $
 *------------------------------------------------------------------
 * Include file used internal to the FSLIB functions
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fslib_internals.h,v $
 * Revision 3.2.60.3  1996/04/22  18:58:16  ssangiah
 * CSCdi54920:  Need to add support for the flash MIB on c7200.
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/04/03  15:43:52  wfried
 * CSCdi52833:  PCMCIA Flash File System not working
 * Branch: California_branch
 * Fix Predator PCMCIA flash file system support. Major issues are byte
 * swapping, and PCMCIA controller timing.
 *
 * Revision 3.2.60.1  1996/03/18  19:35:46  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:36:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:50:11  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:08:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:57:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:32:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__fslib_internals_h__)
#define __fslib_internals_h__

#include "filesys.h"
#include "ralib.h"
#include "fslib.h"

/*----------------------------------------------------------------------------*
 * Typedefs for the file system
 * 
 * fslib_error -- an error code from the file system.  These are non-positive
 *   numbers that indicate success (0) or some sort of failure (negative).
 *   These are often used on top of other integer values (for example, file
 *   descriptors are positive, so any negative file descriptor can be
 *   construed as an error code).
 * fslib_ptr -- a pointer into the file system space for a device.  This is an
 *   offset from the base address of the device and is typically in the range
 *   0 to 64M.  In some cases a negative number is used to denote some sort of
 *   error or invalid condition.  It is OK, but not generally recommended, to
 *   assign fslib_error values to an fslib_ptr.
 *----------------------------------------------------------------------------*/
typedef int   fslib_error;
typedef ulong fslib_ptr;

/*----------------------------------------------------------------------------*
 * Typedefs for the READ, WRITE and ERASE functions
 *----------------------------------------------------------------------------*/
typedef int (* fslib_read_fn_t)(int dev_num, fslib_ptr src, char *dst, ulong length);
typedef int (* fslib_write_fn_t)(int dev_num, char *src, fslib_ptr dst, ulong length);
typedef int (* fslib_erase_fn_t)(int dev_num, fslib_ptr sector, ulong length);
typedef int (* fslib_init_fn_t)(int dev_num);
typedef int (* fslib_show_info_fn_t)(int dev_num);


/*----------------------------------------------------------------------------*
 * Device structure
 *----------------------------------------------------------------------------*/
typedef struct fslib_device_
{
    uchar                   valid;
    uchar                   file_open_for_write;
    uchar                   writable;
    uchar                   unused[1];
    uchar                   dev_id[FSLIB_DEV_ID_MAX];
    ulong                   dev_num;
    fslib_ptr               file_system_end;
    fslib_ptr               monlib_end;
    fslib_ptr               bad_sector_map_end;
    fslib_ptr               squeeze_log_end;
    fslib_ptr               squeeze_buffer_end;
    ulong                   last_sector;
    fslib_ptr               next_new_file_hdr_ptr;
    ulong                   next_index;
    fslib_read_fn_t         read_fn;
    fslib_write_fn_t        write_fn;
    fslib_erase_fn_t        erase_fn;
    fslib_device_info_block_t  dib;
    int                     format_semaphore;
    int                     open_semaphore;
    int                     is_busy;
    int                     busy_reason;
} fslib_device_t;

#define FSLIB_MAX_DEVICE   8

extern fslib_device_t FSLIB_device_table[];

extern fslib_device_t *get_dev (const char *dev_id);

/*----------------------------------------------------------------------------*
 * Structure that defined the different write/erase algorithms
 *----------------------------------------------------------------------------*/
typedef struct fslib_prog_rec_ {
    ulong                   algorithm;
    fslib_read_fn_t         read_fn;
    fslib_write_fn_t        write_fn;
    fslib_erase_fn_t        erase_fn;
    fslib_init_fn_t         init_fn;
    fslib_show_info_fn_t    show_info_fn;
} fslib_prog_rec_t;

extern fslib_prog_rec_t  FSLIB_prog_table[];


/*----------------------------------------------------------------------------*
 * File Header
 *----------------------------------------------------------------------------*/
#define FSLIB_FILE_HDR_MAGIC     0x07158805

#define FSLIB_FILE_FLAGS_READ_ONLY   0x00000000
#define FSLIB_FILE_FLAGS_HIDDEN      0x00000000
#define FSLIB_FILE_FLAGS_LINK        0x00000000
#define FSLIB_FILE_FLAGS_TRUNCATED   0x00000000

#define FSLIB_HDR_FLAGS_IN_USE_BIT   0x00000001
#define FSLIB_HDR_FLAGS_CREATED_BIT  0x00000002
#define FSLIB_HDR_FLAGS_VALID_BIT    0x00000004
#define FSLIB_HDR_FLAGS_ERROR_BIT    0x00000008
#define FSLIB_HDR_FLAGS_IN_USE   FSLIB_HDR_FLAGS_IN_USE_BIT
#define FSLIB_HDR_FLAGS_CREATED  (FSLIB_HDR_FLAGS_IN_USE | FSLIB_HDR_FLAGS_CREATED_BIT)
#define FSLIB_HDR_FLAGS_VALID    (FSLIB_HDR_FLAGS_CREATED | FSLIB_HDR_FLAGS_VALID_BIT)
#define FSLIB_HDR_FLAGS_ERROR    ( FSLIB_HDR_FLAGS_IN_USE_BIT  \
                                 | FSLIB_HDR_FLAGS_CREATED_BIT \
                                 | FSLIB_HDR_FLAGS_VALID_BIT   \
                                 | FSLIB_HDR_FLAGS_ERROR_BIT   )

typedef struct fslib_file_hdr_
{
    ulong     magic;
    ulong     index;
    char      file_name[FSLIB_FILE_NAME_MAX + 1];
    ulong     length;
    ulong     next;
    ulong     crc;
    ulong     type;
    ulong     time;
    ulong     file_flags;
    ulong     hdr_flags;
    ushort    deleted;
    ushort    undeleted;
    ulong     unused[6];
} fslib_file_hdr_t;

/*----------------------------------------------------------------------------*
 *
 *----------------------------------------------------------------------------*/
#define FSLIB_FILE_STATE_CLOSED   ((uchar)0)
#define FSLIB_FILE_STATE_OPENING  ((uchar)1)
#define FSLIB_FILE_STATE_OPEN     ((uchar)2)
#define FSLIB_FILE_STATE_ERROR    ((uchar)3)

#define FSLIB_FILE_TYPE_NORMAL    ((uchar)0)
#define FSLIB_FILE_TYPE_DIRECTORY ((uchar)1)

typedef struct fslib_file_rec_
{
    fslib_device_t   *dev;
    fslib_file_hdr_t  hdr;
    fslib_ptr         hdr_phys_ptr;
    fslib_ptr         start_phys_ptr;
    fslib_ptr         curr_phys_ptr;
    fslib_ptr         old_file_hdr_ptr;
    ulong             curr_ptr;
    uchar             state;
    uchar             type;
    uchar             writable;
    uchar             readable;
    int               fd;
    uint              flag;
} fslib_file_rec_t;

#define FSLIB_NO_VERBOSE 0x0001		/* disable verbose if set */
#define FSLIB_NO_CRC     0x0002		/* no crc checking if set */
#define	FSLIB_FILE_ERR   0x0010		/* file needs to close with error */

#define FSLIB_MAX_FILE_REC 8

extern fslib_file_rec_t FSLIB_file_rec[];


/*----------------------------------------------------------------------------*
 * Structure to hold state of a squeeze operation
 *----------------------------------------------------------------------------*/
#define FSLIB_SQUEEZE_START_OF_HDR   0
#define FSLIB_SQUEEZE_MIDDLE_OF_HDR  1
#define FSLIB_SQUEEZE_DATA           2
#define FSLIB_SQUEEZE_DONE           3
#define FSLIB_SQUEEZE_ERROR          4

#define FSLIB_NOT_DONE               0
#define FSLIB_DONE_END_OF_FS         1
#define FSLIB_DONE_PARTIAL_FILE_HDR  2
#define FSLIB_DONE_ERROR             3

typedef struct fslib_squeeze_state_rec_ {
    int          state;
    int          done;
    int          file_index;
    int          files_removed;
    fslib_ptr    old_hdr_ptr;
    fslib_ptr    next_old_hdr_ptr;
    fslib_ptr    rd_ptr;
    fslib_ptr    rd_ptr_save;
    fslib_ptr    wr_ptr;
    int          bytes_left_this_hdr;
    int          bytes_left_this_file;
    int          bytes_left_this_sector;
    int          last_status_length;
    fslib_ptr    log_ptr;
    char        *ram_ptr;
    char        *ram_buffer; 
   fslib_device_t *dev;
} fslib_squeeze_state_rec_t;

/*----------------------------------------------------------------------------*
 * Macro for 4-byte alignment so we don't have constants all over the code
 *----------------------------------------------------------------------------*/
#define ROUND_UP_TO_4_BYTES(_ptr)  (((_ptr) + 3) & 0xfffffffc) 

/*----------------------------------------------------------------------------*
 * Structure to hold state of a squeeze operation
 *----------------------------------------------------------------------------*/
#define FSLIB_REGION_DIB             0
#define FSLIB_REGION_BAD_SECTOR_MAP  1
#define FSLIB_REGION_MONLIB          2
#define FSLIB_REGION_FILE_SYSTEM     3
#define FSLIB_REGION_SQUEEZE_LOG     4
#define FSLIB_REGION_SQUEEZE_BUFFER  5
#define FSLIB_REGION_UNKNOWN         6

/*----------------------------------------------------------------------------*
 * Return values from good_sector
 *----------------------------------------------------------------------------*/
#define FSLIB_SECTOR_BAD     0
#define FSLIB_SECTOR_GOOD    1
#define FSLIB_SECTOR_SPARED  2

/*----------------------------------------------------------------------------*
 * Flash file system error codes
 *----------------------------------------------------------------------------*/
#define FSLIB_OK                                          0
#define FSLIB_ERROR                                      -1
#define FSLIB_ERROR_INVALID_FILE_ID                      -2
#define FSLIB_ERROR_INVALID_DEVICE_ID                    -3
#define FSLIB_ERROR_INVALID_FILE_NAME                    -4
#define FSLIB_ERROR_INVALID_FLAGS                        -5
#define FSLIB_ERROR_INVALID_WHENCE                       -6
#define FSLIB_ERROR_UNKNOWN_DEVICE                       -7
#define FSLIB_ERROR_NO_DEVICE                            -8
#define FSLIB_ERROR_FILE_NOT_FOUND                       -9
#define FSLIB_ERROR_FILE_NOT_UNIQUE                      -10
#define FSLIB_ERROR_FILE_NOT_UNDELETABLE                 -11
#define FSLIB_ERROR_DEVICE_IS_READ_ONLY                  -12
#define FSLIB_ERROR_FILE_IS_READ_ONLY                    -13
#define FSLIB_ERROR_NO_MORE_FILE_RECS                    -14
#define FSLIB_ERROR_BAD_FILE_DESCRIPTOR                  -15
#define FSLIB_ERROR_FILE_NOT_OPEN_FOR_READ               -16
#define FSLIB_ERROR_FILE_NOT_OPEN_FOR_WRITE              -17
#define FSLIB_ERROR_BUFFER_NOT_BIG_ENOUGH                -18
#define FSLIB_ERROR_FILE_TYPE_ALREADY_SET                -19
#define FSLIB_ERROR_FILE_TYPE_SET_THE_SAME               -20
#define FSLIB_ERROR_FILE_SYSTEM_FULL                     -21
#define FSLIB_ERROR_CANNOT_OPEN_ANOTHER_FILE_FOR_WRITE   -22
#define FSLIB_ERROR_FILE_NOT_DIRECTORY                   -23
#define FSLIB_ERROR_BAD_DIB                              -24
#define FSLIB_ERROR_SECTOR_NOT_ERASED                    -25
#define FSLIB_ERROR_COULD_NOT_READ_ENTIRE_FILE_HDR       -26
#define FSLIB_ERROR_FIXING_CREATED_FILE_HDR              -27
#define FSLIB_ERROR_NULL_FILE_NAME_PTR                   -28
#define FSLIB_ERROR_INVALID_IN_USE_FILE_HDR              -29
#define FSLIB_ERROR_INVALID_CREATED_FILE_HDR             -30
#define FSLIB_ERROR_INVALID_CREATED_FILE_HDR_NAME        -31
#define FSLIB_ERROR_INVALID_CREATED_FILE_HDR_MAGIC       -32
#define FSLIB_ERROR_INVALID_CREATED_FILE_HDR_DELETE      -33
#define FSLIB_ERROR_INVALID_FILE_HDR_CRC                 -34
#define FSLIB_ERROR_INVALID_FILE_HDR_NAME                -35
#define FSLIB_ERROR_INVALID_FILE_HDR_MAGIC               -36
#define FSLIB_ERROR_INVALID_FILE_HDR_NEXT                -37
#define FSLIB_ERROR_READING_FILE_FOR_CRC                 -38
#define FSLIB_ERROR_RALIB_FLASH_PTR                      -39
#define FSLIB_ERROR_RALIB_DEV_NUM                        -40
#define FSLIB_ERROR_RALIB_MODE                           -41
#define FSLIB_ERROR_RALIB_UNKNOWN                        -42
#define FSLIB_ERROR_READING_FOR_ERASE_CHECK              -43
#define FSLIB_ERROR_FILE_OPEN_FOR_WRITE                  -44
#define FSLIB_ERROR_READING_SQUEEZE_LOG                  -45
#define FSLIB_ERROR_FIXING_SQUEEZE_LOG                   -46
#define FSLIB_ERROR_INVALID_SQUEEZE_LOG_ENTRY            -47
#define FSLIB_ERROR_READING_DATA_DURING_SQUEEZE          -48
#define FSLIB_ERROR_WRITING_SECTOR_DURING_SQUEEZE        -49
#define FSLIB_ERROR_SQUEEZE_LOG_FULL                     -50
#define FSLIB_ERROR_WRITING_SQUEEZE_LOG_1                -51
#define FSLIB_ERROR_WRITING_SQUEEZE_LOG_2                -52
#define FSLIB_ERROR_WRITING_SQUEEZE_LOG_3                -53
#define FSLIB_ERROR_SECTOR_COPY_SRC                      -54
#define FSLIB_ERROR_SECTOR_COPY_DST                      -55
#define FSLIB_ERROR_SECTOR_COPY_READ                     -56
#define FSLIB_ERROR_SECTOR_COPY_WRITE                    -57
#define FSLIB_ERROR_INIT_DEVICE_NO_DEVS                  -58
#define FSLIB_ERROR_READING_DIB                          -59
#define FSLIB_ERROR_REMOVING_SPARE_SECTOR                -60
#define FSLIB_ERROR_SETTING_SPARE_SECTOR                 -61
#define FSLIB_ERROR_READING_BAD_SECTOR_MAP               -62
#define FSLIB_ERROR_WRITING_BAD_SECTOR_MAP               -63
#define FSLIB_ERROR_COULD_NOT_ALLOC_SQUEEZE_BUFFER       -64
#define FSLIB_ERROR_NO_READ_FUNCTION                     -65
#define FSLIB_ERROR_NO_WRITE_FUNCTION                    -66
#define FSLIB_ERROR_NO_ERASE_FUNCTION                    -67
#define FSLIB_ERROR_ERASING_SECTOR                       -68
#define FSLIB_ERROR_DEVICE_NOT_READY                     -69
#define FSLIB_ERROR_WRITING_DEVICE                       -70
#define FSLIB_ERROR_ERASING_DEVICE                       -71
#define FSLIB_ERROR_VPP_LOW                              -72
#define FSLIB_ERROR_BEING_FORMAT                         -73
#define FSLIB_ERROR_FILE_SYSTEM_INCONSISTANT             -74
#define FSLIB_ERROR_DEVICE_NAME_BUFFER_TOO_SMALL         -75
#define FSLIB_ERROR_NO_MONLIB_ON_DEV                     -76
#define FSLIB_ERROR_NO_MEMORY_FOR_MONLIB_BUF             -77
#define FSLIB_ERROR_NO_MEMORY_FOR_FLASH_READ             -78
#define FSLIB_ERROR_NO_MEMORY_FOR_BAD_SECTOR_MAP         -79
#define FSLIB_ERROR_READING_MONLIB                       -80
#define FSLIB_ERROR_MONLIB_SIZE                          -81
#define FSLIB_ERROR_BAD_SECTOR_NOT_ALLOWED               -82
#define FSLIB_ERROR_COPY_MONLIB_TO_DEV                   -83
#define FSLIB_ERROR_DEVICE_BUSY                          -84
#define FSLIB_ERROR_NO_ALGORITHM                         -85
#define FSLIB_ERROR_WRITING_DIB                          -86
#define FSLIB_ERROR_BAD_SECTOR_DEVICE_SIZE               -87

#define VPP_DELAY	3000		/* Turn On VPP delay (us)	*/
#define RDY_DELAY	32		/* Cmd->Rdy delay (us)		*/
#define FLASH_OP_COUNT	64

/*----------------------------------------------------------------------------*
 * Defines for the "prog_algorithm" field in the DIB
 * These are the programming algorithms that are supported
 *   0 - invalid (used to denote the end of the list of supported
 *       types in the programming array)
 *   1 - any read only card that requires no special commands to read
 *       (this is the default set of routines that are used if a card
 *        with an unrecognized "prog_algorithm" is found).
 *   2 - RAM card: any card that requires no special commands to read
 *       or write or to switch between read and write modes.  The
 *       functions for this type of card make sure that writes which
 *       start and/or end on odd boundaries don't corrupt data.
 *       Erases to 0x00
 *   3 - RAM card: (same as above)
 *       Erases to 0xff
 *   4 - Intel Series 2+ flash cards (any size in general, but
 *       specifically the 4 and 20 MB versions).
 *   5 - Internal flash bank on the RSP
 *----------------------------------------------------------------------------*/
#define FSLIB_PROG_LIST_END                 0
#define FSLIB_PROG_ROM_CARD                 1
#define FSLIB_PROG_RAM_CARD_00              2
#define FSLIB_PROG_RAM_CARD_FF              3
#define FSLIB_PROG_INTEL_SERIES_2PLUS_CARD  4
#define FSLIB_PROG_RSP_INTERNAL_FLASH       5
#define FSLIB_PROG_MAX                      5

/*----------------------------------------------------------------------------*
 * Structure to hold state of a squeeze operation
 *----------------------------------------------------------------------------*/
#define FSLIB_SQUEEZE_LOG_FLAGS_IN_USE_BIT   0x01
#define FSLIB_SQUEEZE_LOG_FLAGS_VALID_BIT    0x02
#define FSLIB_SQUEEZE_LOG_FLAGS_INVALID_BIT  0x04

#define FSLIB_SQUEEZE_LOG_FLAGS_IN_USE       FSLIB_SQUEEZE_LOG_FLAGS_IN_USE_BIT

#define FSLIB_SQUEEZE_LOG_FLAGS_VALID        (  FSLIB_SQUEEZE_LOG_FLAGS_IN_USE_BIT   \
					  | FSLIB_SQUEEZE_LOG_FLAGS_VALID_BIT ) 

#define FSLIB_SQUEEZE_LOG_FLAGS_INVALID      (  FSLIB_SQUEEZE_LOG_FLAGS_IN_USE_BIT   \
					  | FSLIB_SQUEEZE_LOG_FLAGS_VALID_BIT     \
					  | FSLIB_SQUEEZE_LOG_FLAGS_INVALID_BIT )

#define FSLIB_SQUEEZE_LOG_TYPE_WRITE  0xaa
#define FSLIB_SQUEEZE_LOG_TYPE_COPY   0x55

typedef struct fslib_squeeze_log_rec_ {
    uchar    flags;
    uchar    type;
    ushort   state;
    union {
	struct {
	    fslib_ptr    wr_ptr;
	} copy;
	struct {
	    fslib_ptr    old_hdr_ptr;
	    fslib_ptr    wr_ptr;
	    int          file_index;
	} start_of_hdr;
	struct {
	    fslib_ptr    old_hdr_ptr;
	    fslib_ptr    wr_ptr;
	    int          file_index;
	    int          bytes_left_this_hdr;
	} middle_of_hdr;
	struct {
	    fslib_ptr    next_old_hdr_ptr;
	    fslib_ptr    rd_ptr;
	    fslib_ptr    wr_ptr;
	    int          file_index;
	    int          bytes_left_this_file;
	} data;
    } s;
} fslib_squeeze_log_rec_t;

#define	FLASH_NAME_LEN_MAX      32

/*
 * This structure is used for format device.
  */
typedef struct format_dev_parm_
{
    uint erase_state;
    int alg_num;
    int device_size;
    int sector_size;
    int devnum;
    uint sector_num;
    void *ptblp;
    fslib_device_info_block_t *dibp;
    char flash_name[FLASH_NAME_LEN_MAX + 1];
} format_dev_parm_t;

extern int analyze_flash_device(format_dev_parm_t *);

#define FLASH_CARD0_NAME_STR    "Flash PCMCIA card at slot 0"
#define FLASH_CARD1_NAME_STR    "Flash PCMCIA card at slot 1"
#define FLASH_SIMM_NAME_STR     "Flash internal SIMM"

/*----------------------------------------------------------------------------*
 * global variables
 *----------------------------------------------------------------------------*/
extern int FSLIB_file_descriptor;
extern ralib_vector_table_t *FSLIB_ralib;
extern fslib_vector_table_t  FSLIB_vector_table;


/*----------------------------------------------------------------------------*
 * external function declarations for functions that are only used
 * internal to the file system and the file system device drivers.
 *----------------------------------------------------------------------------*/
extern int convert_ralib_error_code (int ralib_error_code);
extern int FSLIB_build_dib(fslib_device_info_block_t *dib,
			   ulong sector_size,
			   ulong num_sectors,
			   ulong num_spare_sectors,
			   ulong monlib_length,
			   ulong prog_algorithm,
			   ulong erased_state,
			   const char *volume_id);


/*----------------------------------------------------------------------------*
 * Defines for length limits when printing status updates
 *----------------------------------------------------------------------------*/
#define FSLIB_CALC_CRC_STATUS_LENGTH_LIMIT              (32 * 1024)
#define FSLIB_FIX_CREATED_FILE_HDR_STATUS_LENGTH_LIMIT  (32 * 1024)

/*----------------------------------------------------------------------------*
 * 
 *----------------------------------------------------------------------------*/
extern void FSLIB_dev_init(void);

#endif /* !defined(__fslib_internals_h__) */
