/* $Id: rtt_dependluecho.h,v 3.1.4.2 1996/06/07 22:24:37 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/dspu/rtt_dependluecho.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor dependant lu probe Header file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the  definitions for
 * the dependant lu probe implementation for the RTT Monitor.
 *------------------------------------------------------------------
 * $Log: rtt_dependluecho.h,v $
 * Revision 3.1.4.2  1996/06/07  22:24:37  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  10:49:18  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.3  1996/05/14  06:18:33  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 3.1.2.2  1996/04/19  02:39:12  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 3.1.2.1  1996/03/22  18:50:55  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.1  1996/03/20  17:54:47  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _DEPENDLUECHO_H_
#define _DEPENDLUECHO_H_

#include "../dspu/luapi.h"
#include "../rtt/rtt_lib.h"

#define BINDDATA_BUFF_SIZE  64
#define INIT_SELF_SIGNATURE "NSPECHO"

typedef struct _applicationHandle_t {
    rttMonCtrlAdminQItem    *inRttMonCtrlAdminQItem;
    watched_boolean         *receiveFinished;
    void                    *currentLUHandle;
    long                    maxRespBufferLen;
    long                    receivedBufferLen;
    long                    IDofMySelf;
    long                    errorSense;
    boolean                 closeIndicationCalled;
} applicationHandle_t;

#define D_echoTarget_timeout            1
#define D_echoTarget_connectionLost     2
#define D_echoTarget_internalError      3
#define D_echoTarget_ayyOK              4

extern int rtt_lu0_echo_exists SR_PROTOTYPE((void));

extern void rtt_lu0_echo_activate SR_PROTOTYPE((rttMonCtrlAdminQItem 
						*inRttMonCtrlAdminQItem));

extern int rtt_lu0_echo_validate SR_PROTOTYPE((rttMonCtrlAdminEntry_t 
					       *ctrlAdminEntry, 
					       void *specificEntry,
					       int rttObject));

extern int rtt_lu2_echo_exists SR_PROTOTYPE((void));

extern void rtt_lu2_echo_activate SR_PROTOTYPE((rttMonCtrlAdminQItem 
						*inRttMonCtrlAdminQItem));

extern int rtt_lu2_echo_validate SR_PROTOTYPE((rttMonCtrlAdminEntry_t 
					       *ctrlAdminEntry, 
					       void *specificEntry,
					       int rttObject));

extern void rtt_depend_lu_split_address SR_PROTOTYPE((OctetString *target,
						      char **hostname,
						      long  *hostnamelen,
						      char **applname,
						      char **modename,
						      long   IDofMySelf));

extern void rtt_dump_lu_return_code SR_PROTOTYPE((LuApiReturnCode lu_return_code,
						  long            IDofMySelf));

extern void rtt_dumpApplicationHandle SR_PROTOTYPE((applicationHandle_t
						    *applicationHandle));

extern void rtt_dumpRespBuffer SR_PROTOTYPE((applicationHandle_t  
					     *applicationHandle,
					     unsigned char        *bufferPtr,
					     unsigned int          bufferLen));

extern void rtt_dumpSendBuffer SR_PROTOTYPE((unsigned char        *bufferPtr,
					     unsigned int          bufferLen,
					     long                  IDofMySelf));

extern void rtt_stripTrailingBlanks SR_PROTOTYPE((char *instring));

extern void rtt_closeIndication SR_PROTOTYPE((void          *applicationHandle,
					      unsigned char *bufferPtr));

#endif /* _DEPENDLUECHO_H_ */
