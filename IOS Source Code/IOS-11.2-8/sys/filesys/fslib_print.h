/* $Id: fslib_print.h,v 3.2 1995/11/17 09:08:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/fslib_print.h,v $
 *------------------------------------------------------------------
 * Include file for print utilities for debugging the flash file system
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fslib_print.h,v $
 * Revision 3.2  1995/11/17  09:08:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:32:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__fslib_print_h__)
#define __fslib_print_h__

#include "filesys.h"
#include "fslib_internals.h"

#if (defined(STANDALONE) || defined(FSLIB))
#define buginf printf
#endif

#if (!defined(STANDALONE) || defined(FSLIB))
#ifndef RSP_ROM
#include "../os/printf.h"
#endif
#endif


extern int global_indent_level;
extern int global_print;
extern int global_print_level;
extern int global_print_mask;

/*----------------------------------------------------------------------------*
 * The fields of the level parameter to the PRT macro
 * There are three fields
 *   type -- identifies on of 16 types (bit fields)
 *   flag -- a few special meaning bits
 *----------------------------------------------------------------------------*/
#define FSLIB_PRT_TYPE_MASK    0x03ffffff
#define FSLIB_PRT_FLAG_MASK    0xfc000000

/*----------------------------------------------------------------------------*
 * definitions for the flags
 *----------------------------------------------------------------------------*/
#define FSLIB_PRT_ENTRY        0x08000000
#define FSLIB_PRT_EXIT         0x00000000
#define FSLIB_PRT_ZERO_INDENT  0x10000000
#define FSLIB_PRT_NO_INDENT    0x20000000
#define FSLIB_PRT_BUGINF       0x40000000
#define FSLIB_PRT_PRINTF       0x80000000


/*----------------------------------------------------------------------------*
 * The levels and the definitions for the entry, exit and no-indent defines
 *
 * The levels are used as follows
 * FSLIB levels (0 to 15)
 *    0 -- FSLIB main entry point routines
 *    1 -- High level read utilities or utilities specific to a single function
 *    2 -- High level write utilities or utilities specific to a single function
 *    3 -- Middle level read utilities or utilities called seldom
 *    4 -- Middle level write utilities or utilities called seldom
 *    5 -- Low level read utilities or utilities called often
 *    6 -- Low level write utilities or utilities called often
 *    7 -- dev_read 
 *    8 -- dev_write
 *    9 -- erase_sector
 *   10 -- Device read_fn
 *   11 -- Device write_fn
 *   12 -- Device erase_fn
 *   13 -- Write utilities
 *   14 -- Comments and Errors
 * RALIB levels (16-23)
 *   15 -- Interrupt functions
 *   16 -- Main RALIB functions
 *   17 -- RALIB read
 *   18 -- RALIB write
 *   19 -- low level read functions
 *   20 -- low level write functions
 *   21 -- 
 *   22 -- CLPD6720 high level functions
 *   23 -- CLPD6720_reg_read and CLPD6720_reg_write
 *   24 -- Read Data
 *   25 -- Write Data
 *----------------------------------------------------------------------------*/
#define PRT_L0   (1 << 0)
#define PRT_L1   (1 << 1)
#define PRT_L2   (1 << 2)
#define PRT_L3   (1 << 3)
#define PRT_L4   (1 << 4)
#define PRT_L5   (1 << 5)
#define PRT_L6   (1 << 6)
#define PRT_L7   (1 << 7)
#define PRT_L8   (1 << 8)
#define PRT_L9   (1 << 9)
#define PRT_L10  (1 << 10)
#define PRT_L11  (1 << 11)
#define PRT_L12  (1 << 12)
#define PRT_L13  (1 << 13)
#define PRT_L14  (1 << 14)
#define PRT_L15  (1 << 15)
#define PRT_L16  (1 << 16)
#define PRT_L17  (1 << 17)
#define PRT_L18  (1 << 18)
#define PRT_L19  (1 << 19)
#define PRT_L20  (1 << 20)
#define PRT_L21  (1 << 21)
#define PRT_L22  (1 << 22)
#define PRT_L23  (1 << 23)
#define PRT_L24  (1 << 24)
#define PRT_L25  (1 << 25)

#define P0E    (PRT_L0  | FSLIB_PRT_ENTRY)
#define P0X    (PRT_L0  | FSLIB_PRT_EXIT)
#define P0     (PRT_L0  | FSLIB_PRT_NO_INDENT)
#define P1E    (PRT_L1  | FSLIB_PRT_ENTRY)
#define P1X    (PRT_L1  | FSLIB_PRT_EXIT)
#define P1     (PRT_L1  | FSLIB_PRT_NO_INDENT)
#define P2E    (PRT_L2  | FSLIB_PRT_ENTRY)
#define P2X    (PRT_L2  | FSLIB_PRT_EXIT)
#define P2     (PRT_L2  | FSLIB_PRT_NO_INDENT)
#define P3E    (PRT_L3  | FSLIB_PRT_ENTRY)
#define P3X    (PRT_L3  | FSLIB_PRT_EXIT)
#define P3     (PRT_L3  | FSLIB_PRT_NO_INDENT)
#define P4E    (PRT_L4  | FSLIB_PRT_ENTRY)
#define P4X    (PRT_L4  | FSLIB_PRT_EXIT)
#define P4     (PRT_L4  | FSLIB_PRT_NO_INDENT)
#define P5E    (PRT_L5  | FSLIB_PRT_ENTRY)
#define P5X    (PRT_L5  | FSLIB_PRT_EXIT)
#define P5     (PRT_L5  | FSLIB_PRT_NO_INDENT)
#define P6E    (PRT_L6  | FSLIB_PRT_ENTRY)
#define P6X    (PRT_L6  | FSLIB_PRT_EXIT)
#define P6     (PRT_L6  | FSLIB_PRT_NO_INDENT)
#define P7E    (PRT_L7  | FSLIB_PRT_ENTRY)
#define P7X    (PRT_L7  | FSLIB_PRT_EXIT)
#define P7     (PRT_L7  | FSLIB_PRT_NO_INDENT)
#define P8E    (PRT_L8  | FSLIB_PRT_ENTRY)
#define P8X    (PRT_L8  | FSLIB_PRT_EXIT)
#define P8     (PRT_L8  | FSLIB_PRT_NO_INDENT)
#define P9E    (PRT_L9  | FSLIB_PRT_ENTRY)
#define P9X    (PRT_L9  | FSLIB_PRT_EXIT)
#define P9     (PRT_L9  | FSLIB_PRT_NO_INDENT)
#define P10E   (PRT_L10  | FSLIB_PRT_ENTRY)
#define P10X   (PRT_L10  | FSLIB_PRT_EXIT)
#define P10    (PRT_L10  | FSLIB_PRT_NO_INDENT)
#define P11E   (PRT_L11  | FSLIB_PRT_ENTRY)
#define P11X   (PRT_L11  | FSLIB_PRT_EXIT)
#define P11    (PRT_L11  | FSLIB_PRT_NO_INDENT)
#define P12E   (PRT_L12  | FSLIB_PRT_ENTRY)
#define P12X   (PRT_L12  | FSLIB_PRT_EXIT)
#define P12    (PRT_L12  | FSLIB_PRT_NO_INDENT)
#define P13E   (PRT_L13  | FSLIB_PRT_ENTRY)
#define P13X   (PRT_L13  | FSLIB_PRT_EXIT)
#define P13    (PRT_L13  | FSLIB_PRT_NO_INDENT)
#define P14E   (PRT_L14  | FSLIB_PRT_ENTRY)
#define P14X   (PRT_L14  | FSLIB_PRT_EXIT)
#define P14    (PRT_L14  | FSLIB_PRT_NO_INDENT)
#define P15E   (PRT_L15  | FSLIB_PRT_ENTRY)
#define P15X   (PRT_L15  | FSLIB_PRT_EXIT)
#define P15    (PRT_L15  | FSLIB_PRT_NO_INDENT)
#define P16E   (PRT_L16  | FSLIB_PRT_ENTRY)
#define P16X   (PRT_L16  | FSLIB_PRT_EXIT)
#define P16    (PRT_L16  | FSLIB_PRT_NO_INDENT)
#define P17E   (PRT_L17  | FSLIB_PRT_ENTRY)
#define P17X   (PRT_L17  | FSLIB_PRT_EXIT)
#define P17    (PRT_L17  | FSLIB_PRT_NO_INDENT)
#define P18E   (PRT_L18  | FSLIB_PRT_ENTRY)
#define P18X   (PRT_L18  | FSLIB_PRT_EXIT)
#define P18    (PRT_L18  | FSLIB_PRT_NO_INDENT)
#define P19E   (PRT_L19  | FSLIB_PRT_ENTRY)
#define P19X   (PRT_L19  | FSLIB_PRT_EXIT)
#define P19    (PRT_L19  | FSLIB_PRT_NO_INDENT)
#define P20E   (PRT_L20  | FSLIB_PRT_ENTRY)
#define P20X   (PRT_L20  | FSLIB_PRT_EXIT)
#define P20    (PRT_L20  | FSLIB_PRT_NO_INDENT)
#define P21E   (PRT_L21  | FSLIB_PRT_ENTRY)
#define P21X   (PRT_L21  | FSLIB_PRT_EXIT)
#define P21    (PRT_L21  | FSLIB_PRT_NO_INDENT)
#define P22E   (PRT_L22  | FSLIB_PRT_ENTRY)
#define P22X   (PRT_L22  | FSLIB_PRT_EXIT)
#define P22    (PRT_L22  | FSLIB_PRT_NO_INDENT)
#define P23E   (PRT_L23  | FSLIB_PRT_ENTRY)
#define P23X   (PRT_L23  | FSLIB_PRT_EXIT)
#define P23    (PRT_L23  | FSLIB_PRT_NO_INDENT)
#define P24E   (PRT_L24  | FSLIB_PRT_ENTRY)
#define P24X   (PRT_L24  | FSLIB_PRT_EXIT)
#define P24    (PRT_L24  | FSLIB_PRT_NO_INDENT)
#define P25E   (PRT_L25  | FSLIB_PRT_ENTRY)
#define P25X   (PRT_L25  | FSLIB_PRT_EXIT)
#define P25    (PRT_L25  | FSLIB_PRT_NO_INDENT)

#define PCOM    P14
#define PRDATA (PRT_L24 | FSLIB_PRT_ZERO_INDENT | FSLIB_PRT_NO_INDENT)
#define PWDATA (PRT_L25 | FSLIB_PRT_ZERO_INDENT | FSLIB_PRT_NO_INDENT)
#define PERR   (PRT_L14 | FSLIB_PRT_ZERO_INDENT | FSLIB_PRT_NO_INDENT)
#define P15INT (P15 | FSLIB_PRT_BUGINF)
#define P23INT (P23 | FSLIB_PRT_BUGINF)

#define FSLIB_PRT_FN  printf

#if defined(FSLIB)
extern int (*printf_ptr)();
#define printf  printf_ptr
#endif

#if defined(FSLIB_NO_DEBUG)
#define PRT(level,params)
#else

/******************************************************************************
 * Name: PRT
 *
 *******************************************************************************/
#define PRT(level,params)                                                      \
    do {                                                                       \
        int indent_count;                                                      \
        int indent_level;                                                      \
                                                                               \
        if (global_print == FALSE) {                                           \
            break;                                                             \
	}                                                                      \
                                                                               \
        if (((level) & FSLIB_PRT_NO_INDENT) != 0) {                            \
            indent_level = global_indent_level;                                \
        } else {                                                               \
            if (((level) & FSLIB_PRT_ENTRY) != 0) {                            \
                indent_level = global_indent_level;                            \
                global_indent_level++;                                         \
            } else {                                                           \
                global_indent_level--;                                         \
                indent_level = global_indent_level;                            \
            }                                                                  \
            if (global_indent_level < 0) {                                     \
                global_indent_level = 0;                                       \
            }                                                                  \
        }                                                                      \
                                                                               \
        if (((level) & global_print_mask) == 0) {                              \
            break;                                                             \
	}                                                                      \
                                                                               \
        if (global_print_level > indent_level) {                               \
            if (((level) & FSLIB_PRT_ZERO_INDENT) == 0) {                      \
                for (indent_count = 0; indent_count < indent_level; indent_count++) { \
		    if ((level) & FSLIB_PRT_PRINTF)                            \
                        printf(".   ");                                        \
		    else if ((level) & FSLIB_PRT_BUGINF)                       \
                        buginf(".   ");                                        \
                    else                                                       \
                        FSLIB_PRT_FN(".   ");                                  \
                }                                                              \
            }                                                                  \
            if ((level) & FSLIB_PRT_PRINTF)                                    \
                printf params;                                                 \
	    else if ((level) & FSLIB_PRT_BUGINF)                               \
                buginf params;                                                 \
	    else                                                               \
                FSLIB_PRT_FN params;                                           \
	}                                                                      \
                                                                               \
   } while(FALSE)
 
#endif



/*----------------------------------------------------------------------------*
 * Externs for the functions provided in fsilb_print.c
 *----------------------------------------------------------------------------*/
extern void ulong_dump(ulong *data,int length);
extern void ushort_dump(ushort *data,int length);
extern void char_dump(char *data,int length);
extern void print_file_hdr(fslib_file_hdr_t *fh);
extern void print_dib(fslib_device_info_block_t *dib);
extern void print_bad_sector_map(char *bsm, int length);
extern void print_dev(fslib_device_t *dev,int flags);
extern void print_file_rec(fslib_file_rec_t *fp,int flags);
extern void print_file_recs(int flags);
extern void print_squeeze_log_rec(fslib_squeeze_log_rec_t *sl);
extern void print_dirent_header(void);
extern void print_dirent_header_short(void);
extern void print_dirent(dirent_t *de);
extern void print_dirent_short(dirent_t *de);
extern void print_stat(stat_t *st);
extern void print_file_sys_stat(file_sys_stat_t *fss);
extern boolean check_monlib_mismatch(const char *, boolean);
#endif /* !defined(__fslib_print_h__) */
