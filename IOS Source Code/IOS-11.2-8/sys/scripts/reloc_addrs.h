/* $Id: reloc_addrs.h,v 3.1.24.1 1996/06/16 21:19:20 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/scripts/reloc_addrs.h,v $
 *------------------------------------------------------------------
 * reloc_addrs.h : Per-platform addresses for relocatable image.
 *
 * Oct 1994, Ashwin Baindur
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: reloc_addrs.h,v $
 * Revision 3.1.24.1  1996/06/16  21:19:20  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/02/13  20:55:00  hampton
 * Add a new file for the per-platform addresses of relocatable images.
 * [CSCdi48844]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if defined(XX)
 
#define DEFAULT_TEXT_BASE 0x12000 /* Default text start on XX (in SRAM) */
#define DEFAULT_DATA_BASE 0x20000 /* Default data base - XX */

 
#endif /* XX */
 
#if defined(PAN)
 
#define DEFAULT_TEXT_BASE 0x3000060 /* Default text base - for bc */
                                    /* bc = backward compatibility */
#define DEFAULT_DATA_BASE 0x1000    /* Default data base - most products */
 
#endif /* PAN only */
 
#if defined(BRASIL)
 
#define DEFAULT_TEXT_BASE 0x22000060 /* Default text base - Brasil */
#define DEFAULT_DATA_BASE 0x5000    /* Default data base - Brasil */

#endif /* BRASIL only */
