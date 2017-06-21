/* $Id: clpd6729.h,v 3.1.66.2 1996/07/18 17:57:44 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/filesys/clpd6729.h,v $
 *------------------------------------------------------------------
 * clpd6729.h - Include file for the CLPD6729 driver
 *
 * Novemeber 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clpd6729.h,v $
 * Revision 3.1.66.2  1996/07/18  17:57:44  mbeesley
 * CSCdi63193:  Need support for rev2 gt64010. Teach the monlib and
 * the IOS system images the incompatible differences between rev1 and
 * rev2 gt64010 chips :
 *         o Addressing flip on bytes accesses thru PCI IO space
 *         o Internal PCI config register addressing change
 * Branch: California_branch
 *
 * Revision 3.1.66.1  1996/03/21  22:36:30  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.2  1996/01/03  05:56:55  wfried
 * Branch: ELC_branch
 * Fix PCMCIA controller configuration and isolate Bootflash control
 * software from PCMCIA control software.
 *
 * Revision 3.1.2.1  1995/11/22  20:25:47  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/20  21:36:39  mbeesley
 * Add placeholder files for future development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__clpd6729_h__)

#define __clpd6729_h__

/*
 * Define PCI config register values
 */
#define CLPD6729_VENDOR_ID		0x11001013

/*
 * Enable IO, Mem, Parity, SErr
 */
#define CLPD6279_COMMAND_SETTINGS	0x000001C3

/*
 * Extension registers available in the CLPD6729 chip
 */
#define EXT_IDX_IDX			0x2E	/* Extended Index     */
#define EXT_DT_IDX			0x2F	/* Extended Data      */

#define MAP_0_UPPER_IDX			0x05	/* Win 0 Up Addr bits */
#define MAP_1_UPPER_IDX			0x06	/* Win 1 Up Addr bits */
#define MAP_2_UPPER_IDX			0x07	/* Win 2 Up Addr bits */
#define MAP_3_UPPER_IDX			0x08	/* Win 3 Up Addr bits */
#define MAP_4_UPPER_IDX			0x09	/* Win 4 Up Addr bits */
#define EXT_CTRL_1_IDX			0x03	/* Extension Ctrl 1   */
#define EXT_CTRL_2_IDX			0x0B	/* Extension Ctrl 2   */

#define CTL_1_MGMT_INT_LOW		0x10	/* Mgmt Intr Active Low */

/*---------------------------------------------------------------------------*
 * External references to CLPD6720 functions
 *---------------------------------------------------------------------------*/

extern void 	clpd6729_print_regs(void);
extern int  	clpd6729_pcmcia_supported(void);
extern uchar 	clpd6729_pcmcia_regs_offset(void);

#endif /* !defined(__clpd6729_h__) */

/* end of file */
