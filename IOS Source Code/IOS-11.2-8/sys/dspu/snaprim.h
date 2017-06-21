/* $Id: snaprim.h,v 3.3.10.1 1996/05/17 10:49:43 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/snaprim.h,v $
 *------------------------------------------------------------------
 * DSPU SNA Primitives.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * Header file for snaprim. Primitives to decode SNA TH and RH headers
 * and to build SNA messages.  
 *------------------------------------------------------------------
 * $Log: snaprim.h,v $
 * Revision 3.3.10.1  1996/05/17  10:49:43  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.2.2  1996/04/22  00:06:40  lcox
 * CSCdi55112:  mem corruption on ACTPU when NMVT rsp owed downstream
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.2.1  1996/03/22  18:51:07  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.3  1996/02/26  19:04:20  ppearce
 * CSCdi49872:  DSPU cannot establish connection with VTAM through CIP
 *   Insert CV 0x0E and CV 0x10 into XID3
 *
 * Revision 3.2  1995/11/17  09:06:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:30:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:30:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __SNAPRIM_H                         
#define __SNAPRIM_H

#include "dtypes.h"       /* Defines types word, hword, byte, etc */ 
#include "bfrprim.h"         

#define COLD 0x1
#define ERP  0x2


/*=====================================================================*
* SessionType 
*======================================================================*
*
* Description:
*   Enumeration of various session types.   
*   
**/
typedef enum SessionType {
             SSCP_PU, 
             SSCP_LU, 
             LU_LU,
             IND_LU
            } SessionType;

/*=====================================================================*
* RequestResponseType
*======================================================================*
*
* Description:
*   Enumeration of values for Request/Response Indicator. 
*
**/
typedef enum RequestResponseType {
             SNA_NO_RH, 
             SNA_REQUEST, 
             SNA_RESPONSE,
             SNA_INVALID_FID,
             SNA_RU_LEN_ERROR,
             SNA_INCOMPLETE_RH
            } RequestResponseType;
 
/*=====================================================================*
* ResponseType
*======================================================================*
*
* Description:
*   Enumeration of values for Response Type Indicator.   
*   
**/
typedef enum ResponseType {
             SNA_POSITIVE, 
             SNA_NEGATIVE
            } ResponseType;

/*=====================================================================*
* RUType
*======================================================================*
*
* Description:
*   Enumeration of values for Request/Response Unit Types. 
*   
**/
typedef enum RUType {
             SNA_OTHER, 
             SNA_ACTPU, 
             SNA_DACTPU, 
             SNA_ACTLU, 
             SNA_DACTLU, 
             SNA_BIND, 
             SNA_UNBIND, 
             SNA_SDT,
             SNA_CLEAR,
             SNA_NMVT, 
             SNA_RECFMS, 
             SNA_NOTIFY,
             SNA_REQDISCONT,
             SNA_INITSELF,
             SNA_NSPE
            } RUType;

/*=====================================================================*
* SNANotifyType
*======================================================================*
*
* Description:
*   Enumeration of LU availability. 
*   
**/
typedef enum SNANotifyType {
             LU_INHIBITED,             /* SLU INHIBITED (0x00) */
             LU_NOT_AVAILABLE,         /* SLU DISABLED (0x01)  */
             LU_AVAILABLE              /* SLU ENABLED (0x03)   */
            } SNANotifyType;

/*=====================================================================*
* SNA
*======================================================================*
*
* Description:
*   SNA structure. 
*   
**/
typedef struct {
               byte        TH0;       /* Transmission Header       */
               byte        TH1;
               byte        DAF;       /* Destination Address Field */
               byte        OAF;       /* Origin Address Field      */
               byte        SNF[2];    /* Sequence Number Field     */
               byte        RH0;       /* Request Header	           */
               byte        RH1;
               byte        RH2;
               byte        ruData[1]; /* Data array                */
              }SNA; 


/*=====================================================================*
* SNASessionFlow
*======================================================================*
*
* Description:
*   Session flow structure. 
*   
**/
typedef struct{
               SessionType sessionType;
               byte        luNumber;
              } SNASessionFlow; 


/*=====================================================================*
* SNAMU
*======================================================================*
*
* Description:
*   Structure which is returned when an MU is decoded. 
*   
**/
typedef struct{
               RequestResponseType reqRsp;
               RUType              ruType;
               hword               systemSense;
               hword               userSense;
               ResponseType        rspType;
               boolean             enable;
               boolean             fmd;
               byte                pacing;
              } SNAMU;
              

/*=====================================================================*
* SNARspProtocol
*======================================================================*
*
* Description:
*   Enumeration of chain response protocols. 
*   
**/
typedef enum SNARspProtocol {
             NoResponse,
             ExceptionResponse,
             DefiniteResponse,
             DefiniteOrExceptionResponse
            } SNARspProtocol;

/*=====================================================================*
* SNASendRcvMode
*======================================================================*
*
* Description:
*   Enumeration of normal-flow send/receive modes. 
*   
**/
typedef enum SNASendRcvMode {
             FullDuplex,
             HalfDuplexContention,
             HalfDuplexFlipFlop
            } SNASendRcvMode;

/*=====================================================================*
* BindData
*======================================================================*
*
* Description:
*   Structure which is returned when an BIND request is decoded. 
*   
**/
typedef struct{
               byte           slu;            /* DAF */
               byte           plu;            /* OAF */
               byte           tsProfile;
               byte           fmProfile;
               byte           luType;
               boolean        pluChaining;    /* multiple-RU chains */
               SNARspProtocol sluRspProtocol;
               SNASendRcvMode sendRcvMode;
               byte           bracketSupport; /* 0:none, n:byte no */
               boolean        bsSecMaySendEB;
               uint           sSendPacingWindow; /* secondary */
              } BindData;



/*=====================================================================*
* DecodeSession
*======================================================================*
*
* Description:
*   This function decodes the given CLSI buffer to retrieve the type of 
* session and LU number. 
*   - Both OAF and DAF are zero then SSCP-PU session, LU number is not
*     applicable.     
*   - One of OAF or DAF is zero then SSCP-LU session, and LU number is 
*     non-zero OAF or DAF.   
*   - OAF and DAF both non zero then either LU-LU or IND-LU session. If
*     the direction is to the Host and DAF is 1 then, LU-LU session 
*     (otherwise IND-LU session). 
*
**/
void  DecodeSession
        (DataReq_t      * bufferP
        ,SNASessionFlow * sessionP 
        );
                       

/*=====================================================================*
* DecodeMu
*======================================================================*
*
* Description:
*   This function examines the MU to determine the following:
*     - if MU is a Request or Response
*     - RU type
*     - Sense Data   
*     - Positive or Negative Rsp
*     - 'enable' field (see comment below)
*
**/
boolean DecodeMu
          (DataReq_t * bufferP
          ,SNAMU     * snaMuP
          );
            
            
/*=====================================================================*
* DecodeBind
*======================================================================*
*
* Description:
*   This function examines an MU already known to be a BIND REQUEST
*   (from DecodeMu) to determine the following:
*     - TS & FM Profile
*     - Normal Send/Receive Mode
*     - Brackets support
*     - Chains support
*     - Pacing support
*   and optionally to extract user data.
*
*   FALSE is returned if the MU is an invalid BIND request.
*   The BIND is invalid if: 
*     - MU length is less than 27 bytes
*     - MU is short according to the length fields that are used to
*       interpret the bind beyond the 26th byte.
**/
boolean DecodeBind
          (DataReq_t * bufferP
          ,BindData  * bindDataP
          ,uint        userDataBufferLength
          ,char      * userDataBuffer
          ,uint      * userDataLength
          );



/*=====================================================================*
* DecodeLuLuFmd
*======================================================================*
*
* Description:
*   This function examines an MU already known to be an FMD REQUEST
*   (from DecodeMu) to extract the MU data to a separate buffer.
*   The length returned is the total length of data in the MU - if the
*   destination buffer is too short, the data is truncated to that
*   length.
*   The lastFrameWasEC flag is set to the EC flag when the RH is present,
*   so that it can be recorded for the endData calculation.
*   The endData flag is TRUE when EBIU is set and we are currently
*   accumulating an only or last in chain.
**/
void DecodeLuLuFmd
          (DataReq_t * bufferP
          ,uint        userDataBufferLength
          ,char      * userDataBuffer
          ,uint      * userDataLength
          ,boolean   * lastFrameWasEC
          ,boolean   * endData
          );


/*=====================================================================*
* ChangeMuToPosRsp 
*======================================================================*
*
* Description:
*   Changes the given MU to a positive rsp. It changes the RRI and the  
* RTI bits to Response and positive. SDI is set to indicate that no   
* sense data is included. BCI and ECI are set indicate that the MU is a  
* single element chain. BBIUI and EBIUI are set to indicate that the  
* MU is a whole BIU. Byte 2 of RH is set to zero. The length of the
* data area is calculated and set.
*   FALSE is returned if the MU was originally a response or if
* the request did not request definite response (i.e. ERI=1, or
* DR1I=DR2I=0).
*   TRUE is returned if the response is ready to send.
*    
**/
boolean ChangeMuToPosRsp
          (DataReq_t     * bufferP
          );
   

/*=====================================================================*
* ChangeActpuToPosRsp
*======================================================================*
*
* Description:
*   Changes the ACTPU to a positive rsp by setting the activation type, 
* ACTPU name, and resetting the data length.    
*
**/
boolean ChangeActpuToPosRsp
          (DataReq_t * bufferP
          ,byte* puName
          ,byte        activationType
          );


/*=====================================================================*
* ChangeActluToPosRsp
*======================================================================*
*
* Description:
*    Change actlu request to a response. Note that data length required
* for this response may be bigger than the existing buffer. If so we
* need to create a new buffer.
*   
*
**/
DataReq_t * ChangeActluToPosRsp
          (DataReq_t * oldP
          ,SNANotifyType notifyType
          );


/*=====================================================================*
* ChangeMuToNegRsp
*======================================================================*
*
* Description:
*   Changes the MU to a negative response by changing the RRI to RSP, 
* RTI to NEGATIVE, and SDI to SD(Sense Data Included). BCI and ECI 
* are set to indicate no chaining. BBIUI and EBIUI indicate the rsp
* is a whole BIU. Byte 2 of TH is set to zero. The RU is truncated
* (if longer) to 3 bytes for FMD NS category or 1 byte otherwise.   
*   FALSE is returned if the MU was originally a response or if
* the request indicated no response (DR1I and DR2I both off).
*   TRUE is returned if the response is ready to send.
**/
boolean ChangeMuToNegRsp
          (DataReq_t * bufferP
          ,hword       systemSense
          ,hword       userSense
          );


/*===========================================================================*
* ChangeMuToIPR
*============================================================================*
*
* Description:
*   Converts a request that has been sent exception or no response with
*   the pacing bit on to an Isolated Pacing Response.
*   FALSE is returned if the MU is not a first or only segment, the original
*   is already a response or if the pacing bit is not on in the request.
*   TRUE is returned if the response is ready to send.
**/
boolean ChangeMuToIPR
          (DataReq_t * bufferP
          );


/*=====================================================================*
* BuildActpu
*======================================================================*
*
* Description:
*   ACTPU is sent by the SSCP to activate a session with the PU, and to 
* obtain information about the PU.  
*
**/
DataReq_t * BuildActpu(void);
               
               
/*=====================================================================*
* BuildDactpu
*======================================================================*
*
* Description:
*  DACTPU is sent to deactivate the session between the SSCP and the PU.  
*    
**/
DataReq_t* BuildDactpu(void);

/*=====================================================================*
* BuildActlu
*======================================================================*
*
* Description:
*
**/
DataReq_t* BuildActlu
             (byte lu
             );


/*=====================================================================*
* BuildDactlu
*======================================================================*
*
* Description:
*   DACTLU is sent to deactivate the session between the SSCP and the LU.   
*
**/
DataReq_t* BuildDactlu
             (byte lu
             );

/*=====================================================================*
* BuildUnbind
*======================================================================*
*
* Description:
*   UNBIND is sent to deactivate an active session between two LUs. 
*
**/
DataReq_t* BuildUnbind 
             (byte lu
             ,byte unbindType
             ,BlockingType blkType
             );

/*=====================================================================*
* BuildNotify
*======================================================================*
*
* Description:
*   Notify is used to send information from an LU to an SSCP.
*
**/
DataReq_t* BuildNotify
             (byte lu
             ,SNANotifyType notifyType
             );


/*=====================================================================*
* BuildNmvtPsid
*======================================================================*
*
* Description:
*
**/
DataReq_t * BuildNmvtPsid
              (byte lu
              );


/*=====================================================================*
* BuildNmvtRsp
*======================================================================*
*
* Description:
*   Build a NMVT +ve Rsp.
*
**/
DataReq_t * BuildNmvtRsp (
                          hword      sequenceNumber);


/*============================================================================*/
/* Build_Xid3_Req: Construct an XID(3) ID.REQ signal. */
/*============================================================================*/

IdReq_t *
Build_Xid3_Req (byte* xid,
                byte* puName,
                hword maxIFrame,
                byte  windowSize,
                boolean xid3Negotiation);

/*============================================================================*/
/* Build_Xid3_Rsp: Construct an XID(3) ID.RSP signal. */
/*============================================================================*/

IdRsp_t *
Build_Xid3_Rsp (byte* xid,
                byte* puName,
                hword maxIFrame,
                byte  windowSize);


/*===========================================================================*
* BuildSscpLuDataReq
*============================================================================*
*
* Description:
*   Construct an SSCP-LU session PIU
*
**/
DataReq_t *
BuildSscpLuDataReq (byte     oaf,
                    uint     piuDataLen, /* Length of PIU data       */
                    char*    piuData,    /* PIU data                 */
                    boolean* defreq      /* def rsp request (return) */
                    );

/*===========================================================================*
* BuildLuLuDataReq
*============================================================================*
*
* Description:
*   Construct an LU-LU session PIU
*
**/
DataReq_t *
BuildLuLuDataReq (byte     oaf,
                  byte     daf,
                  hword    snaId,     /* sequence number     */
                  byte     rh1,
                  byte     rh2,
                  uint     ruDataLen, /* Length of RU data   */
                  char*    ruData     /* RU data             */
                  );

/*===========================================================================*
* SetPacingBit
*============================================================================*
*
* Description:
*   Turn on the pacing bit in a PIU.
**/
void
SetPacingBit (DataReq_t* bufferP);

/*===========================================================================*
* BuildInitself
*============================================================================*
*
* Description:
*   Construct an INITSELF.
*
**/
DataReq_t *
BuildInitself (byte     oaf,
               hword    snaId,
               char*    modeName,
               uint     applNameLen,
               char*    applName,
               uint     userDataLen,
               char*    userData
              );


/*======================================================================*/
/* QueryNmvtPsid: Check if message is a NMVT Psid format. */
/*======================================================================*/

boolean
QueryNmvtPsid (DataReq_t* bufferP);


hword ReturnSequenceNumber(DataReq_t* clsiBfrP);

void SaveSequenceNumber(DataReq_t* clsiBfrP, hword sequenceNumber);

/* inline function to speed up process
 *
 * void UpdateOAF(SNA *snaP, byte oaf);
 * void UpdateDAF(SNA *snaP, byte oaf);
 */
#define UpdateOAF(snaP,oaf) ((SNA*)snaP)->OAF = oaf
#define UpdateDAF(snaP,daf) ((SNA*)snaP)->DAF = daf

/* hword GetSnaSystemSense(SNAMU* snaMuP) */
#define GetSnaSystemSense(snaMuP)  (snaMuP->systemSense)

/* hword GetSnaUserSense(SNAMU* snaMuP) */
#define GetSnaUserSense(snaMuP)    (snaMuP->userSense)

/* return major vector from clsi buffer */
byte* GetNmvtMV(DataReq_t* clisBfrP);

#endif  /* __SNAPRIM_H */
/*---- End include file: snaprim.h. */

