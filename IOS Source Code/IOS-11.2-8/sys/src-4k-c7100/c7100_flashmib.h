/* $Id: c7100_flashmib.h,v 3.1.68.2 1996/04/22 18:58:47 ssangiah Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100_flashmib.h,v $
 * ------------------------------------------------------------------
 * c7100_flashmib.h - Flash MIB support definitions
 *
 * October 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * ------------------------------------------------------------------
 * $Log: c7100_flashmib.h,v $
 * Revision 3.1.68.2  1996/04/22  18:58:47  ssangiah
 * CSCdi54920:  Need to add support for the flash MIB on c7200.
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/03/21  23:47:53  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.1  1995/11/22  21:43:09  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/21  01:37:42  mbeesley
 * Add placeholder files for new development.
 *
 * ------------------------------------------------------------------
 * $Endlog$
 */

/*
 * hardwired devices
 */
#define C7100DEV_SLOT0		0
#define C7100DEV_SLOT1		1
#define C7100DEV_BOOTFLASH	2

#define C7100DEVNAME_SLOT0	"slot0"
#define C7100DEVNAME_SLOT1	"slot1"
#define C7100DEVNAME_BOOTFLASH	"bootflash"

/*
 * Function prototypes
 */
extern void c7100_flashmib_init(void);
extern void c7100_flashmib_adjust(void);

/* end of file */
