/*
 *	P A R S E R _ D E F S _ F D D I . H
 *
 * $Id: parser_defs_fddi.h,v 3.1 1996/02/01 04:58:23 hampton Exp $
 * $Source: /release/111/cvs/Xsys/if/parser_defs_fddi.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_fddi.h,v $
 * Revision 3.1  1996/02/01  04:58:23  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:50:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:29:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* FDDI interface command identifiers
 */
#define FDDI_PHY_A               1<<0
#define FDDI_PHY_B               1<<1
#define FDDI_PHY_BOTH            ((FDDI_PHY_A) | (FDDI_PHY_B))

#define CMT_CONNECT              0
#define CMT_DISCONNECT           1

#define FDDI_INTFC_TRT           1
#define FDDI_INTFC_CMT_S_BITS    2
#define FDDI_INTFC_TVX           3
#define FDDI_INTFC_TL_MIN        4
#define FDDI_INTFC_C_MIN         5
#define FDDI_INTFC_TB_MIN        6
#define FDDI_INTFC_T_OUT         7
#define FDDI_INTFC_DUP_CHECK     8
#define FDDI_INTFC_SMT_FRAMES    9
#define FDDI_INTFC_BURST_COUNT  10
#define FDDI_INTFC_ENCAP        11
#define FDDI_INTFC_FASTCMT      12
#define FDDI_INTFC_MAXRATE      13

