/* $Id: nvmonvars.h,v 3.1 1995/11/09 09:10:24 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c7000/nvmonvars.h,v $
 *------------------------------------------------------------------
 * Support for nvram environment variables on the 70x0 RP
 *
 * January 1995, Stig Thormodsrud
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nvmonvars.h,v $
 * Revision 3.1  1995/11/09  09:10:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:17:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1995/06/07 09:05:16  smackie
 * Fold back Arizona_branch into mainline.
 *
 * Revision 1.1.4.1  1995/05/02  23:22:37  sthormod
 * Branch: Arizona_branch
 * Rom monitor support for RP1 flash credit card
 *
 * Revision 1.1  1995/04/23  00:22:52  sthormod
 * Placeholders for Arizona branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define EVTSIZ        400      /* bytes for environment variables */
#define ALSSIZ          0      /* bytes for alias variables */
#define NVMAGIC       -17958194

/*
 * Differences between this and the sierra rom monitor:
 * -  Sierra keeps a ram copy of this structure.  The RP1 only has 4K
 *    of RAM for both it's bss & stack, therefore we can not afford to
 *    allocate a RAM copy.
 * -  Sierra allocates the 1st field for sizing nvram.  The RP1 has a
 *    fixed nvram size.  Sigh.
 * -  The RP1's nvram allows ONLY 8-bit access.  Use wrapper routines
 *    to read/write long/short.  Beware of bcopy().  Also note that
 *    the rom monitor version of strcpy() calls bcopy().
 * -  Originaly the RP1 reserved the last 1 Kbyte of nvram for
 *    environmental monitor "last words", but only ~600 bytes is
 *    needed.  If one of these older images has an ENVM shutdown, it
 *    may overwrite this portion of nvram.  Ugh!
 * -  Sierra's chksum covers the confreg[] too, but the 7000 had
 *    already put it's confreg at the top of nvram.  At least we have
 *    a magic number.
 *  
 *
 * Warning:  This structure has been carefully setup to fix exactly in
 *           the "free" nvram area.  Increasing the size of this
 *           structure will probably cause a bus error as we try to
 *           write off the end of nvram.
 */
struct nvram {                 
  short autobootstate;          /* for the autoboot state machine */
  short autobootcount;          /* retry counter for the autoboot mechanism */
  char *autobootsptr;           /* string pointer for the autoboot mechanism */
  
  ulong magic;                  /* NVMAGIC (checksumming starts here) */
  char evartbl[EVTSIZ];         /* our monitor environment table */
  ushort chksum;                 
  uchar pad2[1];
  uchar confreg_magic[2];
  uchar confreg[2];
};

extern struct nvram *nvram;      /* pointer to physical nvram */

#define NVRAM           (nvram)




