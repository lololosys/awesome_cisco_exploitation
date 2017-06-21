/* $Id: pred_nvmonvars.h,v 3.1.68.1 1996/03/21 23:31:42 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-c7100/pred_nvmonvars.h,v $
 *------------------------------------------------------------------
 * pred_nvmonvars.h -- Predator NVRAM support
 *
 * November 1995, Steve Zhang
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pred_nvmonvars.h,v $
 * Revision 3.1.68.1  1996/03/21  23:31:42  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.5  1996/02/15  02:51:43  mbeesley
 * CSCdi48910:  Add storage of fault history for crashes. Increase the
 * monitors storage area from 512 to 768 bytes for crash history.
 * Branch: ELC_branch
 *
 * Revision 3.1.2.4  1996/02/13  00:43:15  mbeesley
 * CSCdi48770:  Make reset_io() work, add support for fault history
 * Branch: ELC_branch
 *
 * Revision 3.1.2.3  1995/12/05  23:12:28  mbeesley
 * Branch: ELC_branch
 * Add complete support for discontigous NVRAM.
 *
 * Revision 3.1.2.2  1995/12/01  04:23:56  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.1.2.1  1995/11/23  19:14:48  szhang
 * Branch: ELC_branch
 * Initial source code commit to ELC branch for Predator (C7100) ROM
 * monitor.
 *
 * Revision 3.1  1995/11/23  00:55:13  mbeesley
 * Add placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __NVMONVARS_H__
#define __NVMONVARS_H__

/*
 * This structure defines the layout of the monitor portion of the 
 * non-volatile RAM on the board.  This area is used to store monitor
 * information.
 */
#define EVTSIZ           512  /* the environment table size */
#define ALSSIZ           256  /* the alias table size */
#define NVMAGIC    -17958194  /* magic number for the NV RAM */
#define FLTSIZ	         768  /* Fault history buffer */

struct nvram {                  /* the first 6 items must not be moved! */
  long testarea;                /* transient area for testing, must be 1st */
  unsigned long diagreset;      /* for diagnostic reset test */
  unsigned long rdiagreset;     /* checksum for diagreset */
  short autobootstate;          /* for the autoboot state machine */
  short autobootcount;          /* retry counter for the autoboot mechanism */
  char *autobootsptr;           /* string pointer for the autoboot mechanism */
  unsigned short confreg;       /* our virtual configuration register */
  unsigned short rconfreg;      /* our ones complement virtual configuration 
				   register */
  long magic;                   /* NVMAGIC (checksumming starts here) */
  char condev, pollcon;         /* console flags (future use) */
  unsigned char conbaud;        /* the console baud rate (future use) */
  unsigned char auxbaud;        /* the auxillary port baud rate (future) */
  unsigned char killchar;       /* the kill line character (future)*/
  unsigned char erasechar;      /* the erase character (future) */
  unsigned char diagflag;       /* LOOPONERR | STOPONERR | CONTINUOUS, etc. */
  unsigned char pad0[5];        /* pad for future expansion */
  unsigned short pad1[4];       /* for future expansion */
  int memopsiz;                 /* operation size for memory routines */
  unsigned long srcaddr;        /* source address for the menu diags */
  unsigned long destaddr;       /* destination address for the menu diags */
  unsigned long start;          /* start address for the menu diags */
  unsigned long length;         /* length for the menu diags */
  unsigned long value;          /* data value for the menu diags */
  unsigned long trigger;        /* scope trigger address */
  long sys_ret_info[BOOTDATA+1];/* contains data about last reboot */
  unsigned long pad2[4];        /* for future expansion */
  char diaglist[32];            /* storage for our diag groups */
  char evartbl[EVTSIZ];         /* our monitor environment table */
  char aliastbl[ALSSIZ];        /* our monitor alias table */
  char fault_history[FLTSIZ];	/* fault history buffer */
  unsigned long pad3[4];        /* for future expansion */
  short chksum;                 /* area for table checksum - must be last */
};

extern struct nvram nvram;      /* the in-core copy of the structure */
extern struct nvram *nvram_ptr; /* pointer to the NVRAM copy of the 
				   structure */

/*
 * Prototypes
 */
extern int 	nv_valid_confreg(void);
extern ushort 	nv_read_ushort(ushort *ptr);

#define NVRAM           (&nvram)
#define NVRAM_SAV       (nvram_ptr)

/*
 * defines for fields within the nvram structure
 *
 * This was done for portability.  Not all platforms have the same
 * nvram structure - some are restricted to using a minimal amount
 * of memory.  These defines are used in the C modules for those
 * fields which may not be in every platforms nvram structure.
 */
#define ALIAS_TABLE     (NVRAM)->aliastbl
#define CONDEV          (NVRAM)->condev
#define DIAGFLAG        (NVRAM)->diagflag
#define MEMOP_SIZE      (NVRAM)->memopsiz
#define MEMOP_SRCADDR   (NVRAM)->srcaddr
#define MEMOP_DESTADDR  (NVRAM)->destaddr
#define MEMOP_LENGTH    (NVRAM)->length
#define MEMOP_START     (NVRAM)->start
#define MEMOP_VALUE     (NVRAM)->value
#define MEMOP_TRIGGER   (NVRAM)->trigger

/* Macro for determining if the confreg is valid in NVRAM */
#define NV_VALID_CONFREG 	nv_valid_confreg()
#define NV_WR_CONFREG(val) 	nv_wr_confreg(val)

/* defines for diagflag */
#define D_STOPONERR     0x01        /* stop when an error occurs */
#define D_LOOPONERR     0x02        /* loop when an error occurs */
#define D_CONTINUOUS    0x04        /* loop when an error occurs */
#define D_QUIETMODE     0x08        /* quiet all messages */
#define D_NESTED        0x10        /* this is a nested diagnostic */

/* Prototypes */

extern int 	nvramvalid(void);
extern void 	nvraminit(void);
extern int 	sync(int argc, char *argv[]);
extern int 	savenv(void);

/* alias.c */
extern char 	*getvar(char *tblptr, char *vname);
extern int 	setvar(char *tblptr, int tblsiz, char *strptr);
extern int 	unsetvar(char *tblptr, char *var);
extern int 	printtbl(char *tblptr);

#endif  /* __NVMONVARS_H__ */
