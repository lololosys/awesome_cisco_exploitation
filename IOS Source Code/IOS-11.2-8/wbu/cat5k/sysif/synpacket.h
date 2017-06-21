/* $Id: synpacket.h,v 1.1.14.1 1996/05/09 15:01:54 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/sysif/synpacket.h,v $
 *------------------------------------------------------------------
 * synpacket.h -- Synergy packet structures that are used commonly.
 *
 * December, 1994, Vinod
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: synpacket.h,v $
 * Revision 1.1.14.1  1996/05/09  15:01:54  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.24.1  1996/04/29  19:42:29  cakyol
 * missed these during the sync
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.12.1  1996/04/08  01:43:20  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.6.1  1996/03/22  09:14:00  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/20  11:42:00  rlowe
 * Add Synergy content to placeholder files.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/20  11:30:44  rlowe
 * Placeholders for Cat5k ATM line card -- more headers from Synergy
 * syn_common.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.9  1995/10/04  01:07:31  kchristi
 * Fix CVS directives so that cvs-fix-merge script will work.
 *
 * Revision 1.8  1995/04/06  23:56:24  ktomkins
 * 	added "|| defined(GCCMIPS)" to use the packed attribute in monterey
 *
 * Revision 1.7  1995/03/20  17:59:50  umahajan
 * Made change in SAGE_RX_HDR to correct the spelling to BPDU from BDPU.
 *
 * Revision 1.6  1995/03/19  20:59:21  umahajan
 * Corrected SAGE_TX_HEADER - BDPU to BPDU.
 *
 * Revision 1.5  1995/03/01  22:41:12  umahajan
 * Fixed for multiple PAK definitions.
 *
 * Revision 1.4  1995/02/10  04:10:34  umahajan
 * Changed SYN_HDR from 6 to 12.
 *
 * Revision 1.3  1995/02/03  04:08:31  vinod
 * Added PAK definition and conditinal compile GCC68K flag.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*++

Module Name:

    synpacket.h

Revision History:
  12/21/94 - Vinod - Created
  01/18/94 - Vinod - Checking in, may need some more work.
  

Abstract:

   Defines the Synergy packet structures that are used commonly across
   different cards.
   
--*/

#ifndef _SYN_PACKET_H
#define _SYN_PACKET_H

#ifndef PAK
#if defined(GCC68K) || defined(GCCMIPS)
#define PAK __attribute__ ((packed))
#else   /* GCC68K */
#define PAK
#endif  /* GCC68K */
#endif  /* PAK */


#define SYNHDR_SIZE        12
#define MAX_ENET_PACKET    1514


/*
// SAGE TX Header
*/
typedef struct _SAGE_TX_HEADER {

   struct {
       
       uChar Type        : 4 PAK;
       uChar UserDefined : 4 PAK;

   } TypeUser PAK;
   
   struct {
   
       uChar DisableNewLearn   : 1 PAK;
       uChar DisableIndexLearn : 1 PAK;
       uChar DontForward       : 1 PAK;
       uChar Monitor           : 1 PAK;
       uChar IgnoreColorType   : 1 PAK;
       uChar IndexRoute        : 1 PAK;
       uChar DontLearn         : 1 PAK;
       uChar BPDU              : 1 PAK;
       
   } ControlByte PAK;
   
   struct {
   
       uInt32 VLan     : 15 PAK;
       uInt32 Index    : 17 PAK;
   
   } VLanIndex PAK;

   uChar Status PAK;
   
   uChar Parity PAK;
   
   struct {
   
       uInt16 Unused           : 13 PAK;
       uInt16 SwapAddress      : 1 PAK;
       uInt16 NativeAddress    : 1 PAK;
       uInt16 GenerateCRC      : 1 PAK;
   
   } PacketControl PAK;
   
   uInt16 Reserved PAK;
   
} SAGE_TX_HDR, *PSAGE_TX_HDR;




/*
// SAGE RX Header
*/
typedef struct _SAGE_RX_HEADER {

   struct {
       
       uChar Type        : 4 PAK;
       uChar UserDefined : 4 PAK;

   } TypeUser  PAK;
   
   struct {
   
       uChar DisableNewLearn   : 1  PAK;
       uChar DisableIndexLearn : 1  PAK;
       uChar DontForward       : 1  PAK;
       uChar Monitor           : 1  PAK;
       uChar IgnoreColorType   : 1  PAK;
       uChar IndexRoute        : 1  PAK; 
       uChar DontLearn         : 1  PAK;
       uChar BPDU              : 1  PAK;
       
   } ControlByte  PAK;
   
   struct {
   
       uInt32 VLan     : 15  PAK;
       uInt32 Index    : 17  PAK;
   
   } VLanIndex  PAK;

   struct {
   
       uChar SourceMatch   : 1  PAK;
       uChar DestMatch     : 1  PAK;
       uChar NonNativeAddr : 1  PAK;
       uChar CRCInvalid    : 1  PAK;
       uChar Unused        : 3  PAK;
       uChar BPDUPacket    : 1  PAK;
       
   } RxStatus  PAK;
   
   uChar Parity  PAK;
   
   struct {
   
       uInt16 Unused           : 13  PAK;
       uInt16 SwapAddress      : 1  PAK;
       uInt16 NativeAddress    : 1  PAK;
       uInt16 GenerateCRC      : 1  PAK;
   
   } PacketControl  PAK; 
   
   uInt16 Reserved  PAK;
   
} SAGE_RX_HDR, *PSAGE_RX_HDR;


/*
// Synergy header + data
*/
 
typedef struct _SYNERGY_FRAME {

   union {
       /*
       // Generic Headers
       */
       uChar SynergyHeader[SYNHDR_SIZE];    /* space for synergy header */

       /*
       // SAGE Tx header
       */
       SAGE_TX_HDR SageTxHeader;

       /*
       // SAGE Rx header
       */
       SAGE_RX_HDR SageRxHeader;
          
   } SynHdrUnion;
       
   uChar DestinationAddr[6]  PAK;  /* 6 byte dest addr  */
   uChar SourceAddress[6]  PAK;    /* 6 byte src addr   */
   
   /*
   // Data space = Max size ethernet frame less SA DA space
   */
   uChar EnetData[MAX_ENET_PACKET-12]  PAK;
   
   /*
   // Frame CRC as reported by the SAGE
   */
   uInt32 FrameCRC  PAK;
 
} SYNERGY_FRAME, *PSYNERGY_FRAME;

#endif /*_SYN_PACKET_H*/
























