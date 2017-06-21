/* $Id: c1000_flash.h,v 3.2.62.1 1996/03/18 21:57:16 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-36-c1000/c1000_flash.h,v $
 *------------------------------------------------------------------
 * Sapphire PCMCIA flash platform specific code.
 * c1000_flash.h - Platform specific Flash routines (for 1000 and under).
 *
 * Jan 95      Mick Henniger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c1000_flash.h,v $
 * Revision 3.2.62.1  1996/03/18  21:57:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  22:50:02  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  18:39:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:19:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/22  04:36:39  enf
 * CSCdi40869:  Add frankenstorm to 11.0
 *
 * Revision 2.1  1995/06/07  22:52:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __QUICC_FLASH_H__
#define __QUICC_FLASH_H__

#define FLASH_BASE      ADRSPC_PCMCIA_MEM
#define SUN             struct sun_exec

#define PCM_FLASH_SOCK  CLPD6720_SOCKET_1  /* flash card socket             */
#define PCM_FLASH_DWIN  CLPD6720_WINDOW_0  /* 16 mb window for common data  */
#define PCM_FLASH_AWIN  CLPD6720_WINDOW_1  /* 16 mb window for attribute mem*/
#define PCM_FLASH_IOWIN CLPD6720_WINDOW_2  /* 64 Kbyte window for IO space  */

#include "../dev/flash_codes.h"

#endif /* __QUICC_FLASH_H__ */
