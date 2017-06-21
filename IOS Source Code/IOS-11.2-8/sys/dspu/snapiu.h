/* $Id: snapiu.h,v 3.2.58.1 1996/05/17 10:49:37 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/snapiu.h,v $
 *------------------------------------------------------------------
 * DSPU SNA PIU Definitions.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * Defines the bytes in the Transmission and Request/Response
 * headers. The request codes in the Request/Response Unit are
 * also defined. 
 *------------------------------------------------------------------
 * $Log: snapiu.h,v $
 * Revision 3.2.58.1  1996/05/17  10:49:37  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.46.1  1996/03/22  18:51:01  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.2  1995/11/17  09:06:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:30:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:30:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SNABITS_H                         
#define __SNABITS_H


/*----- Transmission Headers (6 Bytes) -----*/

#define TH0_FID   0xf0	               /* Mask for Format ID field  */
#define TH0_FID2  0x20                 /* Type of FID               */

#define TH0_BBIU  0x08                 /* Begin Basic Info Unit     */
#define TH0_EBIU  0x04                 /* End Basic Info Unit       */
#define TH0_BBI   TH0_BBIU             /* Whole BBIU                */
#define TH0_EBI   TH0_EBIU             /*   (BBIU, EBIU)            */
#define TH0_MPF   (TH0_BBIU+TH0_EBIU)  /* Mapping Field             */

#define TH0_EFI   0x01                 /* Expedited Flow Indicator  */

/* Note: TH1 is reserved */

#define SSCP   0                       /* BIUs to the PU T2.0 and 
                                          SSCP always have DAF=0.   */               
#define PU     0                       /* BIUs from the PU T2.0 and
                                          SSCP always have OAF=0.   */
#define DEPENDANT_LU  1

/* Note: TH4 and TH5 is the Sequence Number Field */


/*----- Request/Response Headers -----*/

/*-- Byte 0 --*/
#define RH0B0_RRI         0x80          /* Req/Res Indic. Mask       */
#define RH0B0_RSP         RH0B0_RRI     /* Response value            */ 
#define RH0B0_RQ          0             /* Request value             */ 

#define RH0B1B2_Category  0x60          /* Category Mask             */
#define RH0B1B2_FMD       0             /* FM Data value             */
#define RH0B1B2_NC        0x20          /* Network Control value     */
#define RH0B1B2_DFC       0x40          /* Data Flow Control value   */
#define RH0B1B2_SC        0x60          /* Session Control value     */

#define RH0B4_FI          0x08          /* Format Indicator Mask     */
#define RH0B4_FMH         RH0B4_FI      /* Format Header Included    */
#define RH0B4_NSH         RH0B4_FI      /* Field Format with NS header */

#define RH0B5_SDI         0x04          /* Sense Data Indicator      */
#define RH0B5_SD          RH0B5_SDI     /* Sense Data included       */

#define RH0B6_BCI         0x02          /* Req - Begin Chain Ind     */
#define RH0B6_BC          RH0B6_BCI     /* First in chain            */ 

#define RH0B7_ECI         0x01          /* Req - End Chain Ind       */
#define RH0B7_EC          RH0B7_ECI     /* Last in chain             */ 

/*-- Byte 1 --*/
#define RH1B0_DR1I        0x80          /* Definite Rsp 1 Indicator  */
#define RH1B0_DR1         RH1B0_DR1I    /* Definite Rsp 1            */

#define RH1B1_LCCI        0x40          /* Req - Len Check Compress  */
#define RH1B1_LCC         RH1B1_LCCI    /* RU is compressed          */

#define RH1B2_DR2I        0x20          /* Definite Rsp 2 Indicator  */
#define RH1B2_DR2         RH1B2_DR2I    /* Definite Rsp 2            */

#define RH1B3_ERI         0x10          /* Req - Exception Rsp Ind.  */   
#define RH1B3_ER          RH1B3_ERI     /* Req - Exception Rsp       */ 

                                        /* Indicates in a request the
                                           form of rsp requested.    */
#define RH1_RR            (RH1B3_ERI+RH1B2_DR2I+RH1B0_DR1I)  

#define RH1B3_RTI         0x10          /* Rsp - Response Type Ind.  */   
#define RH1B3_N           RH1B3_RTI     /* Rsp - Negative Rsp        */   
#define RH1B3_P           0             /* Rsp - Positive Rsp        */   

#define RH1B5_RLWI        0x04          /* Req - Larger Window Ind.  */
#define RH1B5_RLW         RH1B5_RLWI    /* Req - Larger Pacing Win   */
 
#define RH1B6_QRI         0x02          /* Queued Response Indicator */
#define RH1B6_QR          RH1B6_QRI     /* Enqueue rsp in TC queues  */

#define RH1B7_PI          0x01          /* Pacing Indicator          */
#define RH1B7_PAC         RH1B7_PI      /* PAC                       */

/*-- Byte 2 --*/
#define RH2B0_BBI         0x80          /* Req - Begin Bracket Ind.  */             
#define RH2B0_BB          RH2B0_BBI     /* Req - BB                  */

#define RH2B1_EBI         0x40          /* Req - End Bracket Ind.    */
#define RH2B1_EB          RH2B0_EBI     /* Req - EB                  */

#define RH2B2_CDI         0x20          /* Req - Change Direction Ind*/
#define RH2B2_CD          RH2B2_CDI     /* Req - Change Direction    */

#define RH2B4_CSI         0x01          /* Req - Code Selection Ind. */
#define RH2B4_CS          RH2B4_CSI     /* Req - Code 1              */
 
#define RH2B5_EDI         0x04          /* Req - Enciphered Data Ind.*/
#define RH2B5_ED          RH2B5_EDI     /* Req - RU is enciphered    */

#define RH2B6_PDI         0x02          /* Req - Padded Data Ind.    */
#define RH2B6_PD          RH2B6_PDI     /* Req - RU is Padded        */

#define RH2B6_CEBI        0x01          /* Req - Cond. End Bracket Ind.*/
#define RH2B6_CEB         RH2B6_CEBI    /* Req - Cond. End LU 6.2      */



/*-----	Request	Codes -----*/
#define	ACTPU 	0x11
#define DACTPU 	0x12
#define ACTLU	0x0d
#define DACTLU  0x0e
#define BIND	0x31
#define UNBIND  0x32
#define SDT     0xa0
#define CLEAR   0xa1

#define NMVT    0x8d  /* Note: FMD NS Headers-third byte is request code */ 
#define RECFMS  0x84
#define NOTIFY  0x20
#define REQDISCONT 0x1B
#define INITSELF 0x81
#define NSPE    0x04


/*----- SenseCode used at DSPU -----*/
#define ResourceNotAvail0801 0x0801
#define   rnaNotReady        0x0009
#define SessionLimitExceeded 0x0805
#define   sleDependentLu     0x0008
#define ResourceNotAvailable 0x0807
#define InvalidParameter     0x0835
#define RULengthError        0x1002
#define FunctionNotSupported 0x1003
#define DataTrafficReset     0x2005
#define IncompleteRH         0x4005
#define LinkFailure          0x8002
#define NoSession            0x8005
#define InvalidFID           0x8006


/*----- Some BIND RU fields -----*/
#define BIND4B0_PLUChain     0x80             /* PLU Chaining Use   */
#define BIND4B0_MultipleRU   BIND4B0_PLUChain /* multiple-RU chains */
#define BIND4B7_SEBI         0x01             /* Send End Bracket   */

#define BIND5B2B3_SLURsp     0x30             /* SLU chain rsp prot */
#define BIND5B2B3_NO         0x00             /* no response        */
#define BIND5B2B3_ER         0x10             /* exception response */
#define BIND5B2B3_DR         0x20             /* definite response  */
#define BIND5B2B3_BOTH       BIND5B2B3_SLURsp /* ER or DR           */
#define BIND5B7_SEBI         0x01             /* Send End Bracket   */

#define BIND6B2_BU           0x20             /* Brackets Usage     */

#define BIND7B0B1_SRM        0xC0          /* Send/Rcv Mode         */
#define BIND7B0B1_FDX        0x00          /* full-duplex           */
#define BIND7B0B1_HDXCNT     0x40          /* half-duplex contention*/
#define BIND7B0B1_HDXFF      0x80          /* half-duplex flip-flop */

#define BIND8B2B7_SSWS       0x3f             /* send pacing window */

#define BIND26B4B7_CLen      0x0f             /* crypt opts length  */



#endif  /* __SNABITS_H */
