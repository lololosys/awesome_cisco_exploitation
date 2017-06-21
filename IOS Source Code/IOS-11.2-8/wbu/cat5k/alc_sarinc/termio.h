/* $Id: termio.h,v 1.1.4.1 1996/05/09 15:00:12 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarinc/termio.h,v $
 *-----------------------------------------------------------------
 * termio.h --  
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: termio.h,v $
 * Revision 1.1.4.1  1996/05/09  15:00:12  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:08  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:20  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:18  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:37:48  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:24  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _TERMIO_
#define _TERMIO_

#define FILEOFFSET	200	/* all files start at fd=200 */
#define ETHERFD		(FILEOFFSET-1)	/* special fd value for ethernet */

/* operation codes for ioctl */
#define TCGETA 1
#define TCSETAF 2
#define TCSETAW 3
#define SETINTR 4
#define SETSANE 5
#define FIONREAD 6
#define GETINTR 7
#define GETTERM 8
#define SETTERM 9
#define SETNCNE 10
#define CBREAK 11
#define TERMTYPE 12
#define DEVENTRY 13
#define TXRDY 14

/* iflags */
#define ISTRIP 0x0020
#define ICRNL  0x0040
#define IXON   0x0400
#define IXANY  0x0800
#define IXOFF  0x1000

/* oflags */
#define ONLCR  0x0004

/* lflags */
#define ICANON 0x0002
#define ECHO   0x0008
#define ECHOE  0x0010

/* cflags */
#define	CBAUD	0000017
#define	B0	0
#define	B50	0000001	
#define	B75	0000002
#define	B110	0000003
#define	B134	0000004
#define	B150	0000005
#define	B200	0000006
#define	B300	0000007
#define	B600	0000010
#define	B1200	0000011
#define	B1800	0000012
#define	B2400	0000013
#define	B4800	0000014
#define	B9600	0000015
#define	B19200	0000016
#define	B38400	0000017

/* cc definitions */
#define VINTR 0
#define VERASE 2
#define VEOL 5
#define VEOL2 6
#define V_START 8
#define V_STOP 9

/* operation codes for device specific driver */
#define OP_INIT 1
#define OP_TX 2
#define OP_RX 3
#define OP_RXRDY 4
#define OP_TXRDY 5
#define OP_BAUD 6

#define DEV_MAX 8
#define STDIN 0
#define STDOUT 1
#define STDERR 2

/* operation codes for ttctl */
#define TT_CM 1		/* cursor movement */
#define TT_CLR 2	/* clear screen */
#define TT_CUROFF 3	/* switch cursor off */
#define TT_CURON 4	/* switch cursor on */

#ifdef LANGUAGE_C
#define CNTRL(x) (x & 0x1f)

#define NCC 23
struct termio {
	unsigned short c_iflag;
	unsigned short c_oflag;
	unsigned short c_cflag;
	unsigned short c_lflag;
	unsigned char c_cc[NCC];
	};

#include "stdio.h"

typedef struct ConfigEntry {
	Addr devinfo;
	int chan;
	iFunc *handler;
	int rxqsize;
	int brate;
	} ConfigEntry;

#include "mips.h"
#include "queue.h"

typedef struct DevEntry {
	int txoff;
	int qsize;
	Queue *rxq;
	Addr sio;
	int chan;
	int rxoff;
	int pollin;
	iFunc *handler;
	jmp_buf *intr;
	char *tname;
	iFunc *tfunc;
	struct termio t;
	} DevEntry;

typedef struct File {
	int dev;
	int valid;
	} File;

typedef struct Ramfile {
	char *name;
	int open;
	unsigned long base;
	unsigned long size;
	unsigned long posn;
	} Ramfile;

extern DevEntry DevTable[DEV_MAX];
extern File _file[OPEN_MAX];
extern Ramfile _mfile[];
extern int *curlst;

#endif /* LANGUAGE_C */

#endif /* _TERMIO_ */

