/* $Id: nv_config_eeprom.h,v 3.2.62.2 1996/06/16 21:16:40 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/nv_config_eeprom.h,v $
 *------------------------------------------------------------------
 *  nv_config_eeprom.h - Definitions for Catalyst (28C256 : 32K x 8)
 *                       and (28C64: 8K x 8) EEPROMS.  Used by Cancun
 *                       and Sapphire for configuration memory.
 *
 * December 1994, David Coli
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nv_config_eeprom.h,v $
 * Revision 3.2.62.2  1996/06/16  21:16:40  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2.62.1  1996/03/18  20:43:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  22:24:53  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  17:42:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:34:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/07  03:10:32  dcoli
 * CSCdi35405:  C1000 NVRAM : simultaneous config accesses can cause crash
 * Use nv_writeflag instead of unnecessary variable eeprom_writingmemory.
 *
 * Revision 2.1  1995/06/07  21:39:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define EEPROM_TIMEOUT  1000

#if defined (PAN) || defined (BRASIL)
#define NV_PAGESIZE     0x40
#define NV_PAGEMASK     0x3f
#endif

#if defined (SAPPHIRE)
#define NV_PAGESIZE     0x20
#define NV_PAGEMASK     0x1f
#endif

/*
 *  Prototypes
 */

nvtype *nv_eeprom_getptr(void);
void nv_eeprom_done(nvtype *);
boolean nv_eeprom_badptr(void);
void malloc_config_eeprom_memory(void);
void eeprom_nvwritebyte(uchar, uchar *, int);
uchar eeprom_nvreadbyte(uchar *, int);
