/* $Id: rtt_sscpruecho.h,v 3.1.4.2 1996/06/07 22:24:44 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/dspu/rtt_sscpruecho.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor sscp Ru probe Header file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the  definitions for
 * the sscp Ru probe implementation for the RTT Monitor.
 *------------------------------------------------------------------
 * $Log: rtt_sscpruecho.h,v $
 * Revision 3.1.4.2  1996/06/07  22:24:44  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  10:49:22  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.1  1996/03/22  18:50:57  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.1  1996/03/20  17:54:51  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _SSCPRUECHO_H_
#define _SSCPRUECHO_H_

#include "../dspu/luapi.h"
#include "../rtt/rtt_lib.h"

typedef struct { 
    byte    TH0;			/* Transmission Header          */
    byte    TH1;			/* reserved                     */
    byte    DAF;			/* TH Destination Address Field */
    byte    OAF;			/* TH Origin Address Field      */
    hword   SNF;			/* TH Sequence Number Field     */
    byte    RH0;			/* Request Header               */
    byte    RH1;			/*                              */
    byte    RH2;			/*                              */
    byte    RU0;			/* RU Header                    */
    byte    RU1;			/*                              */
    byte    RU2;			/*                              */
    byte    RU3;			/* Repeat Data This Times, Not in response  */
    byte    RUDataSize;			/* RU Data Size                 */
    byte    RUData[1];			/* RU Data Start                */
} SNA_SSCP_FRAME;

#define SSCP_HEADER_SIZE 13

extern int rtt_sscp_ru_echo_exists SR_PROTOTYPE((void));

extern void rtt_sscp_ru_echo_activate SR_PROTOTYPE((rttMonCtrlAdminQItem
						    *inRttMonCtrlAdminQItem));

extern int rtt_sscp_ru_echo_validate SR_PROTOTYPE((rttMonCtrlAdminEntry_t 
						   *ctrlAdminEntry, 
						   void *specificEntry,
						   int rttObject));

#endif /* _SSCPRUECHO_H_ */
