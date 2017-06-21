/* $Id: ismagic.c,v 3.2.58.1 1996/03/21 23:27:21 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/ismagic.c,v $
 *------------------------------------------------------------------
 * ismagic.c
 *
 * September 1993, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Convenient magic number verifier.
 *------------------------------------------------------------------
 * $Log: ismagic.c,v $
 * Revision 3.2.58.1  1996/03/21  23:27:21  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:27  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:42:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** Check to see if there is a magic number at the location.
** If there is return it to the caller.
** This routine assumes that the pointer is properly aligned for
** a given hardware platform.
*/
#include "monitor.h"
#include "dib.h"
#include "sh.h"
#include "fstat.h"
#include "getdents.h"
#include "simpfsio.h"
#include "mon_elf.h"
#include "fsio.h"

#ifdef COFFSUPPORT
#include "a.out.h"
#endif   /* COFFSUPPORT */

int
ismagic(unsigned long *ptr)
{
    unsigned long magic;

    magic = *ptr;
    switch(magic) {
    case FLASH_FHDR_MAGIC:
    case ELFMAGIC:

#ifdef COFFSUPPORT
    case MIPSEBMAGIC_3:
#endif   /* COFFSUPPORT */

    case MONSH_MAGIC:
    case FS_DEVICE_INFO_BLOCK_MAGIC:
	return(magic);
    }
    if((magic & 0xffff) == OMAGIC) return(OMAGIC);
    if(((magic >> 16) & 0xffff) == COMPRESSMAGIC)
	return(COMPRESSMAGIC);
    return(0);
}
