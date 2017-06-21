/* $Id: file.h,v 3.2.60.1 1996/09/06 21:00:50 snyder Exp $
 * $Source: /release/112/cvs/Xsys/h/file.h,v $
 *------------------------------------------------------------------
 * file.h - Data Structures for the Router's Filesystem
 *
 * June 30, 1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Definitions for files that reside on other hosts or in our flash memory.
 * Methods for retrieving those files, including TFTP, MOP, and so forth.
 *
 *------------------------------------------------------------------
 * $Log: file.h,v $
 * Revision 3.2.60.1  1996/09/06  21:00:50  snyder
 * CSCdi68334:  dump unused spinner support, save memory
 *              412 image, 4 data
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:12:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:35:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/25  08:34:12  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.1  1995/06/07  20:35:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __FILE_H__
#define __FILE_H__

/*
 * We really do have a filesystem in the router.  It is rather crude at the
 * moment, but we have files, they live in different places, and we
 * have different "drivers" for accessing those files.  The different places
 * include hosts on the network, other routers (tftp service), and flash
 * memory if we have it.  The drivers for accessing these files include
 * TFTP, MOP, and flash memory drivers.
 *
 * At present we hang our file descriptors (filetype) on linked lists.
 * Some file descriptors are created when the system starts up, such
 * as the ones for "network-confg" or "router-confg".  Others are created
 * by the various "boot" commands.
 *
 */

#define IEEEBYTES 6

/*
 * A list of file access methods.
 * The bootstrap monitor knows about the TFTP, MOP, and FLASH codes,
 * so don't change their values.
 */
typedef enum {
    FILE_ACCESS_UNKNOWN = 0,
    FILE_ACCESS_TFTP = 1,
    FILE_ACCESS_MOP = 2,
    FILE_ACCESS_FLASH = 3,
    FILE_ACCESS_ROM = 4,
    FILE_ACCESS_RCP = 5,
    FILE_ACCESS_SLOT0 = 6,
    FILE_ACCESS_SLOT1 = 7,
    FILE_ACCESS_FTP = 8
} file_access_protocol;

/*
 * Data structures for specific file access methods. 
 */
typedef struct {
    addrtype boothost;	 /* Protocol address of boot host to load from */
    addrtype respondent; /* Protocol address of host that responded */
    boolean display;     /* Display progress of transfer on console? */
    char *username;      /* rcp username for snmp initiated transfers */
} file_access_tuba;

typedef struct {
    uchar bootaddr[IEEEBYTES];	/* MAC address of boot host to load from */
    uchar respondaddr[IEEEBYTES]; /* MAC address of host that responded */
    hwidbtype *interface;	/* Interface to boot from */
    boolean has_addr;		/* TRUE if an address was specified */
} file_access_mop;

/*
 * filetype
 * The main file descriptor data structure.
 * It started out as a way of TFTP loading files.
 * It has generalized and will continue to generalize.
 */

struct filetype_ {
    struct filetype_ *next;	
    char *filename;			/* the filename */
    void (*resetsubr)(void);		/* routine to perform reset */
    boolean persistence;		/* TRUE if probing for file desired */
    boolean defaultname;		/* TRUE if no boot command seen */
    boolean responder;			/* TRUE if a responder seen */
    file_access_protocol proto;		/* access protocol to use */
    union {
	file_access_tuba tuba;
	file_access_mop mop;
    } ps;				/* Protocol specific fields */
};

/*
 * Error returns from file operations.
 * The positive numbers are part of the TFTP protocol specification.
 * The negative numbers are cisco specific codes that apply to all
 * file access methods.
 */
#define ERROR_PROTOCOL  -8              /* error in communication protocol */
#define ERROR_READ      -7              /* Read (from flash) Failed */
#define ERROR_WRITE     -6              /* Write (to flash) Failed */
#define ERROR_ABORT	-5		/* Transfer aborted */
#define ERROR_TOOBIG	-4		/* File too big for buffer */
#define ERROR_TIMEOUT	-3		/* Transfer timed out */
#define ERROR_NOMEM	-2		/* No memory for structures */
#define ERROR_OK	-1		/* Internal success indicator */

#define	ERROR_UNDEF	0		/* Not defined */
#define	ERROR_NOFILE	1		/* File not found */
#define	ERROR_ACCESS	2		/* Access violation */
#define	ERROR_NOSPACE	3		/* Disk full or allocation exceeded */
#define	ERROR_ILLOP	4		/* Illegal TFTP operation */
#define	ERROR_UNKID	5		/* Unknown transfer ID */
#define	ERROR_EXISTS	6		/* File already exists */
#define	ERROR_NOUSER	7		/* No such user */


#define DOT_CFG    ".cfg"
#define DASH_CONFG "-confg"
#define MAX_DOS_FILENAME 8  /* Maximum length of a DOS file name */
#define DEFAULT_DOS_CONFIG_FILE   "ciscortr.cfg"
#define DEFAULT_UNIX_CONFIG_FILE  "router-confg"


/*
 * Prototypes
 */

extern void file_xfer_progress(int, int);

extern filetype netconfg, hostconfg, sysconfg;
extern queuetype netconfgQ, hostconfgQ, sysconfgQ;


#endif /* __FILE_H__ */
