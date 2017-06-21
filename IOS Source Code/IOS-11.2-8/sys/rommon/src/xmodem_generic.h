/* $Id: xmodem_generic.h,v 3.1.14.1 1996/06/16 21:18:32 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/xmodem_generic.h,v $
 *------------------------------------------------------------------
 * xmodem_generic.h -- Header file for X/Ymodem protocol defines
 *
 * May 1996, Padmaja Gogineni
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xmodem_generic.h,v $
 * Revision 3.1.14.1  1996/06/16  21:18:32  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/05/10  17:48:07  pgoginen
 * Placeholder files for new development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __XMODEM_GENERIC_H__
#define __XMODEM_GENERIC_H__
 
/*  ASCII Constants  used in data transfer */

#define      SOH  	001 
#define	     STX	002
#define      EOT	004
#define      ACK  	006
#define      NAK  	025
#define	     CAN	030
#define	     ESC	033

/*  defines  */

#define      TIMEOUT  	-1
#define      RETRYMAX  	10    /* max retries to be made in certain handshaking                               routines */
#define	     BBUFSIZ	1024  /* Big block size */
#define	     SBUFSIZ	128   /* Small block size */
#define      CRCCHR	'C'   /* CRC request character */
#define      SEENEOT    -3    /* reached end of transmission */
#define      GOT_ABORT  -2

#define      AOUT_MAGIC 0x00020107

#define      FALSE      0
#define      TRUE       1
#define      NULL       0
 
/* Error messages */
enum ERROR_MSGS {
    SENDER_ABORT,
    SENDER_PROTO_MISMATCH,
    DATA_TIMEOUT,
    HDR_TIMEOUT,
    XMODEM_SYNC_ERROR,
    INVALID_IMAGE,
    INSUFFICIENT_FLASH,
    INSUFFICIENT_DRAM,
    TRUNCATED_IMAGE,
    MAX_XMODEM_ERRS
};

#define CRC16(cp, crc) ( crctab[((crc >> 8) & 255)] ^ (crc << 8) ^ cp)

extern char *error_messages[];
extern int error_msg;
extern long ymodem_filesize;

/* Protypes */
extern int xmodem(int argc, char *argv[]);
extern int receive(void);
extern int getfile(void);
extern int getnextblock(long *blocksize);
extern void abort(void);
extern void usage(char *arg);
extern int getblockzero(void);
extern void getlastblock (void);
extern int checkheader(void);
extern int disp_flash(void);
extern void flash_ffh_checksum();
extern boolean xmodem_flash_image_copy(char *filename);

static int xm_getchar(int waitfor);
static void xm_flush_rx_console();

/* Platform specific calls */
extern ulong xmodem_flash_size();
extern ulong xmodem_freemem();
extern uchar *xmodem_download_start_address();
extern int xmodem_flash_erase();
extern int xmodem_load_flash(ulong source, int size);
extern int xmodem_valid_image(uchar *header_ptr, long *file_size, 
                              long block_size);
extern int xmodem_chk_image_size(long file_size);

#endif /* __XMODEM_GENERIC_H__ */



