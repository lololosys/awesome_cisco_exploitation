/* $Id: if_svip.h,v 3.1.58.1 1996/03/21 22:40:49 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/hes/if_svip.h,v $
 *------------------------------------------------------------------
 * if_svip.h:  SVIP specific constants & definitions
 *
 * November 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_svip.h,v $
 * Revision 3.1.58.1  1996/03/21  22:40:49  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.6.1  1996/02/10  00:04:17  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1996/01/02  06:10:29  getchell
 * Placeholder file for VIP development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Prototypes
 */
extern void svip_debug_init(void);
extern void svip_platform_init(subsystype *);
extern void svip_show_cbus(ushort);
