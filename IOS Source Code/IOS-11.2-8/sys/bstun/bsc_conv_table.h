/* $Id: bsc_conv_table.h,v 3.2.60.4 1996/07/30 20:39:22 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_conv_table.h,v $
 *------------------------------------------------------------------
 * bsc_conv_table.h - BSC coversion table
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * Converts EBCDIC and ASCII characters to inputs for the RBB FSM.
 *
 *------------------------------------------------------------------
 * $Log: bsc_conv_table.h,v $
 * Revision 3.2.60.4  1996/07/30  20:39:22  jbalestr
 * CSCdi62962:  bsc discards frames from Fujitsu ACT4000 ATM
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/07/22  18:22:13  snyder
 * CSCdi63677:  fix a pointer problem, and move more text to data
 *              moves 2692 bytes to data, 16 out of image completely
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/06/17  08:29:42  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.2.60.1  1996/05/17  10:40:11  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/10  08:26:11  jbalestr
 * CSCdi54180:  bsc framing does not see some poll sequences
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/04/03  13:49:57  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  08:48:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  06:24:24  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:13:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __BSC_CONV_TABLE_H
#define __BSC_CONV_TABLE_H

#include "types.h"
#include "bsc.h"

/*---- Exported typedefs, etc. */

/*----------------------------------------------------------------------------*/
/* Please keep the enum list below and the corresponding cs tables in
   bsc_conv_table.c in sync. */

typedef enum {
        CS_SYN, CS_SOH, CS_STX, CS_ENQ, CS_ETX, CS_ETB, CS_EOT, CS_ACK0,
        CS_ACK1, CS_WACK, CS_RVI, CS_DLE, CS_NAK, CS_ESC, CS_SPACE,
        CS_PERCENT, CS_UCB, CS_UCR, CS_GPOLL, CS_PAD , CS_ITB
        } bsc_cs_table_e;

extern const uchar bsc_to_ebcdic[];
extern const uchar bsc_to_ascii[];

/*----------------------------------------------------------------------------*/

typedef enum BSC_Input {
        SYN , SOH , STX , DLE   , ENQ   , PAD   , SNCH , OTHER , ETX , EOT ,
        ETB , ITB , NAK , ACK02 , ACK12 , WACK2 , RVI2 , BAD
        } BSC_Input;

/* BAD type is for ASCII bad parity */

extern const uchar ebcdic_to_bsc[];
extern const uchar ascii_to_bsc[];
extern const uchar ascii_transparent_to_bsc[];

extern const uchar ascii_selpol_table[];
extern const uchar ebcdic_selpol_table[];

extern const uchar ascii_polsel_table[];
extern const uchar ebcdic_polsel_table[];

extern const uchar ascii_dev_address_table[];
extern const uchar ebcdic_dev_address_table[];

extern void bsc_crc16_rev_update( ushort, ushort* );
extern void bsc_lrc_update( ushort, ushort* );

extern const uchar bsc_odd_parity_table[];

#endif  /* __BSC_CONV_TABLE_H. */
/*---- End include file: bsc_conv_table.h. */
