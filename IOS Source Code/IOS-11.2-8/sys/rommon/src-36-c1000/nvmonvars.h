/* $Id: nvmonvars.h,v 3.2.58.1 1996/03/21 23:29:11 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36-c1000/nvmonvars.h,v $
 *------------------------------------------------------------------
 * nvmonvars.h
 *
 * December 1994, Rob Clevenger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The Monitor's NVRAM structure.  This is the scaled down version
 * for Sapphire which has only 8k of nvram.
 *------------------------------------------------------------------
 * $Log: nvmonvars.h,v $
 * Revision 3.2.58.1  1996/03/21  23:29:11  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:22:08  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:44:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:39:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __NVMONVARS_H__
#define __NVMONVARS_H__

#include "cookie.h"

/*
** Special Sapphire cookie stuff.
** The Sapphire platform keeps the cookie info in NVRAM as it does
** not have a separate cookie PROM or SEEPROM.
*/
struct nvcookie {
    unsigned short magic;
    struct cookie cookie;
    unsigned short chksum;
};
#define NVCOOKIE_MAGIC   0x8d74

/*
** This structure defines the layout of the monitor portion of the 
** non-volatile RAM on the board.  This area is used to store monitor
** information.
*/
#define EVTSIZ           472     /* the environment variable table size */
#define ALSSIZ           128     /* the alias table size */
#define NVMAGIC    -17958194     /* magic number for the NV RAM */

struct nvram {
  struct nvcookie nvcookie;      /* mfg programs cookie info here */
  long testarea;                 /* transient area for testing and sizing */
  short autobootstate;           /* for the autoboot state machine */
  short autobootcount;           /* retry counter for the autoboot mechanism */
  char *autobootsptr;            /* string pointer for the autoboot mechanism */
  unsigned short confreg;        /* our virtual configuration register */
  unsigned short rconfreg;       /* our ones complement virtual configuration register */
  long magic;                    /* NVMAGIC (checksumming starts here) */
  char evartbl[EVTSIZ];          /* our monitor environment table */
  long pad[8];                   /* for future use */
  long sys_ret_info[BOOTDATA+1]; /* contains data about last reboot */
  short chksum;                  /* area for table checksum - last chksummed field */
  struct nvcookie nvcookie_bkup; /* backup copy of cookie */
};

extern struct nvram nvram;       /* the in-core copy of the structure */
extern struct nvram *nvram_ptr;  /* pointer to the NVRAM copy of the structure */

#define NVRAM           (&nvram)
#define NVRAM_SAV       (nvram_ptr)

/*
** The Sapphire platform has limited NVRAM space.  Therefore the
** number of variables stored in the monitor's nvram structure was
** trimmed to a minimum.  The following defines are for those
** variables that are in the nvram structure on other platforms.
** This was done for portability.
** These externs are in the file init.c.
*/
extern char aliastbl[ALSSIZ];          /* our monitor alias table */
extern unsigned char conbaud;          /* the console baud rate (future use) */
extern unsigned char killchar;         /* the kill line character (future)*/
extern unsigned char erasechar;        /* the erase character (future) */
extern unsigned char diagflag;         /* LOOPONERR | STOPONERR | CONTINUOUS, etc. */
extern int memop_siz;                  /* operation size for memory routines */
extern unsigned long memop_srcaddr;    /* source address for the menu diags */
extern unsigned long memop_destaddr;   /* destination address for the menu diags */
extern unsigned long memop_start;      /* start address for the menu diags */
extern unsigned long memop_length;     /* length for the menu diags */
extern unsigned long memop_value;      /* data value for the menu diags */
extern unsigned long memop_trigger;    /* scope trigger address */
extern char diaglist[32];              /* storage for our diag groups */
#define ALIAS_TABLE     aliastbl
#define CONDEV          condev
#define DIAGFLAG        diagflag
#define MEMOP_SIZE      memop_siz
#define MEMOP_SRCADDR   memop_srcaddr
#define MEMOP_DESTADDR  memop_destaddr
#define MEMOP_LENGTH    memop_length
#define MEMOP_START     memop_start
#define MEMOP_VALUE     memop_value
#define MEMOP_TRIGGER   memop_trigger

/* Macro for determining if the confreg is valid in NVRAM */
#define NV_VALID_CONFREG ((NVRAM_SAV)->magic == NVMAGIC && (NVRAM_SAV)->confreg == (unsigned short)(~(NVRAM_SAV)->rconfreg))
#define NV_WR_CONFREG(val) nv_wr_confreg(val)

/* defines for diagflag */
#define D_STOPONERR     0x01        /* stop when an error occurs */
#define D_LOOPONERR     0x02        /* loop when an error occurs */
#define D_CONTINUOUS    0x04        /* loop when an error occurs */
#define D_QUIETMODE     0x08        /* quiet all messages */
#define D_NESTED        0x10        /* this is a nested diagnostic */

/* related prototypes */    
extern int nvramvalid(void);
extern void nvraminit(void);
extern int sync(int argc, char *argv[]);
extern int savenv(void);
extern short chksum(unsigned short *addr, int size);
extern nvprotect(int argc, char *argv[]);
extern void nv_wr_confreg(unsigned short val);
extern void put_cookie(struct cookie *cptr);
extern struct cookie *get_cookie(void);
extern int nvcookie_valid(struct nvcookie *nvcp);

/* alias.c */
extern char *getvar(char *tblptr, char *vname);
extern int setvar(char *tblptr, int tblsiz, char *strptr);
extern int unsetvar(char *tblptr, char *var);
extern int printtbl(char *tblptr);

#endif  /* __NVMONVARS_H__ */
