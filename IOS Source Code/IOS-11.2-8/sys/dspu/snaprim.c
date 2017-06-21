/* $Id: snaprim.c,v 3.5.4.4 1996/07/29 21:47:08 snyder Exp $
 * $Source: /release/112/cvs/Xsys/dspu/snaprim.c,v $
 *------------------------------------------------------------------
 * DSPU SNA primitives.  
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * Includes all the Utility functions for handling SNA Headers -
 * both TH and RH fields.  It includes functions for both decoding
 * and creating SNA Headers.
 *------------------------------------------------------------------
 * $Log: snaprim.c,v $
 * Revision 3.5.4.4  1996/07/29  21:47:08  snyder
 * CSCdi64412:  declare some arrays const that should be sub_sna_pu
 *              moves 2376 out of data, saves 116 as well
 * Branch: California_branch
 *
 * Revision 3.5.4.3  1996/05/21  21:53:02  mdb
 * The 96q1 gcc is much more particular about putting parentheses around
 * comparisons for some operands ("&", "^" and "|") [CSCdi56015].
 *
 * Revision 3.5.4.2  1996/05/17  10:49:40  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.2.4  1996/04/22  00:06:42  lcox
 * CSCdi55112:  mem corruption on ACTPU when NMVT rsp owed downstream
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.2.3  1996/04/03  14:03:59  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.2.2  1996/03/22  18:51:04  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.4.2.1  1996/03/17  17:38:45  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.5.4.1  1996/03/18  19:34:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  06:40:58  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/03/07  08:46:15  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:48:53  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/03/11  18:24:49  ppearce
 * CSCdi51263:  Provide a more elegant fix for CSCdi49872
 *
 * Revision 3.4  1996/02/27  06:09:45  ppearce
 * CSCdi49872:  DSPU cannot establish connection with VTAM through CIP
 *   Code review comments
 *
 * Revision 3.3  1996/02/26  19:04:17  ppearce
 * CSCdi49872:  DSPU cannot establish connection with VTAM through CIP
 *   Insert CV 0x0E and CV 0x10 into XID3
 *
 * Revision 3.2  1995/11/17  09:06:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:30:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/20  17:03:39  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.3  1995/09/14 06:15:47  smackie
 * Pre-emptive strike on namespace confusion. Rename DSPU source files that
 * use generic names for specific tasks. (CSCdi40350)
 *
 * Revision 2.2  1995/08/09  05:09:20  ppearce
 * CSCdi38299:  DSPU does not recognize 2-byte ACTLU response
 *
 * Revision 2.1  1995/06/07  20:30:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"   /* For strlen                  */
#include <string.h>

#include "dspu.h"
#include "snaprim.h"  /* Header file                 */   
#include "snautil.h"  /*                             */   
#include "bfrprim.h"  /*                             */   
#include "snapiu.h"   /* PIU byte defines            */
#include "dspu_util.h"
#include "dspu_pu.h"
#include "dspu_error.h"    /* error file                  */
#include "mv_prodid.h"


/*---- Defines needed locally to this module.  */
#define FixedVtam41bug

#define PU_NAME_SZ    8
#define CV0E_MAX_SZ (PU_NAME_SZ+3)      /* cvKey + cvLen + netNameType = +3 */
#define CV10_SZ     (sizeof(ProductId))

#define XID3_SZ      29                 /* Length of XID3 (without CVs)     */

                                        /* Total Length of XID3 (with CVs)  */
#define XID3_MAX_SZ (XID3_SZ+CV0E_MAX_SZ+CV10_SZ)

#define XXXX 0                          /* "XXXX" => entries will be updated */
#define TH_RH_LEN 9
#define TH_LEN    6


/*---- Local static variables. */

static hword snaIdentifier;
static word  psidNmvtTransOrder;


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
 *     the direction is to the Host and DAF is 1 or, direction is from
 *     the Host and the OAF is 1 then, LU-LU session (otherwise IND-LU 
						      *     session).  
						      *  
						      **/
void DecodeSession
(DataReq_t      * bufferP
 ,SNASessionFlow * sessionP 
 )
{    
   SNA *	 snaP;
   
   snaP = (SNA *)ReturnDataHandle(bufferP);
   
   /* Determine the session type and the LU number */
   if ((snaP->OAF == 0) && (snaP->DAF == 0))  {
      /* SSCP-PU Session */
      sessionP->sessionType = SSCP_PU;                 
      sessionP->luNumber = 0;
   }
   else if ((snaP->OAF == 0) || (snaP->DAF == 0)) {
      /* SSCP-LU Session */
      sessionP->sessionType = SSCP_LU;             
      sessionP->luNumber = (snaP->DAF) == 0? snaP->OAF:snaP->DAF;
   }
   else   {
      /* LU-LU or IND-LU Session */
      if (snaP->OAF == 1 || snaP->DAF == 1)  {
	 /* LU-LU Session */
	 sessionP->sessionType = LU_LU;             
	 sessionP->luNumber = snaP->OAF == 1? snaP->DAF: snaP->OAF;
      }
      else { 
	 /* IND-LU Session */
	 sessionP->sessionType = IND_LU;              
	 sessionP->luNumber = 0;
      }
   }
}                       


/*=====================================================================*
* DecodeMu
*======================================================================*
*
* Description:
*   This function examines the MU to determine the following:
*     - TH has Format ID FID2
*     - if MU is a Request or Response
*     - RU type
*     - Sense Data   
*     - Positive or Negative Rsp
*     - 'enable' field (see comment below)
*
*   FALSE is returned if the MU is invalid SNA data. Invalid SNA data
*   is defined as: 
*     - no TH
*     - TH and RH is less than 9 and the BBIU is set.
**/
boolean DecodeMu
          (DataReq_t * bufferP
          ,SNAMU     * snaMuP
          )
{
  SNA   * snaP;
  byte    index = 0; 
  hword   length;

#define SnaHeader  9  

  snaP = (SNA *)ReturnDataHandle(bufferP);
  length = GetDataLength(bufferP);
    
  /* The enable field of struct SNAMU is set to TRUE if one of the   
   * following is true :                                             
   *   1. Notify Req.                                                
   *      The vector X'0C' has 'SLU capability enabled'              
   *   2. ActLU Rsp.                                                 
   *      The vector X'0C' has 'SLU capability enabled'              
   *   3. ActPU Req.                                                 
   *      The vector X'80' has 'Sending node supports unsolicited    
   *      NMVTs for PSID' set.                                       
   */
  snaMuP->enable = FALSE;           

  if (length < 6)  {
    /* Incomplete TH. 
     */
     return FALSE;
  }
  else if ((length < TH_RH_LEN) && ((snaP->TH0 & TH0_BBIU) != 0)) {
    /* Incomplete RH. (First segment and TH+RH length invalid)
     */
    snaMuP->reqRsp = SNA_INCOMPLETE_RH; 
    return TRUE;    
  }
  else if ((snaP->TH0 & TH0_BBIU) == 0) {
    /* SNA data with no RH. (No first segment)  
     */
    snaMuP->reqRsp = SNA_NO_RH; 
    return TRUE;  
  }  
  else if ((snaP->TH0 & TH0_FID) != TH0_FID2) {
    /* Invalid FID found. Only FID2 is supported.
     * This is not an error, it is still SNA data
     * but it is not supported. Thus, TRUE is
     * returned.
     */
    snaMuP->reqRsp = SNA_INVALID_FID;
    return TRUE;
  } 
  else  
  {  
    /* Determine if the RH is a response header */
    if ((snaP->RH0 & RH0B0_RRI) == RH0B0_RSP)
    {
      /* If the response type is negative, then the first four bytes   
       * of the RU is the sense data. This is followed by the response 
       * category. If the response type is positive then the response  
       * category starts at the first byte of the RU. 'index' is set   
       * to the start of the response category in either case.         
       */  
      
      if ((snaP->RH1 & RH1B3_RTI) == RH1B3_N) 
      {
        /* RUType type is negative, so extract the sense data 
         */
        snaMuP->systemSense = (snaP->ruData[0] << 8)+ snaP->ruData[1];
        snaMuP->userSense = (snaP->ruData[2] << 8) + snaP->ruData[3];
      
        /* Set the index to the response category.           
         */
        index = 4; 

        snaMuP->rspType = SNA_NEGATIVE;      
      }
      else
      {
        /* RUType type is positive, so no sense data 
         */
        snaMuP->systemSense = SNA_POSITIVE;  /* Not relevant */
        snaMuP->userSense = SNA_POSITIVE;    /* Not relevant */

        /* Set the index to the response category.           
         */
        index = 0; 

        snaMuP->rspType = SNA_POSITIVE;
      }      
    }
            
    /* Determine the RU Type 
     */
    if ((snaP->RH0 & RH0B1B2_Category) == RH0B1B2_FMD)  
    {
      snaMuP->fmd = TRUE;

      /* Note: Third byte is request code 
       */
      snaMuP->ruType = SNA_OTHER;

      if (snaP->ruData[index] == 0x01) {

        if (snaP->ruData[index+2] == REQDISCONT)
        snaMuP->ruType = SNA_REQDISCONT;

        else if (snaP->ruData[index+2] == INITSELF)
          snaMuP->ruType = SNA_INITSELF;

        else if (snaP->ruData[index+2] == NSPE)
          snaMuP->ruType = SNA_NSPE;

      }
      else if (snaP->ruData[index] == 0x41) {

        if (snaP->ruData[index+2] == RECFMS)
        snaMuP->ruType = SNA_RECFMS;

        else if (snaP->ruData[index+2] == NMVT)
          snaMuP->ruType = SNA_NMVT;

      }
      else if (snaP->ruData[index] == 0x81) {

        if (snaP->ruData[index+2] == NOTIFY)
      { 
        snaMuP->ruType = SNA_NOTIFY;
        
        /* Check if the vector exists 
         */
        if (length > 14) /* 6 bytes for TH, 3 bytes for RH */
        {
          if ((snaP->ruData[3] == 0x0C) &&
              (snaP->ruData[5] == 0x03))    /* Vector Data */
          {
            /* SLU capability is enabled:          
             *   sessions can be queued or started 
             */
            snaMuP->enable = TRUE;         
          }
        }
      }  

      }  

    }
    else if ((snaP->RH0 & RH0B1B2_Category) == RH0B1B2_SC)
    {
      snaMuP->fmd = FALSE;

      if (snaP->ruData[index] == ACTPU)
      {
        if (length - SnaHeader < 9) {
          /* SNA data with invalid RU length.  
           */
          snaMuP->reqRsp = SNA_RU_LEN_ERROR; 
          return TRUE;   
        }        

        snaMuP->ruType = SNA_ACTPU;

        /* Determine if the vector is included 
         */
        if ( ((snaP->RH0 & RH0B0_RRI) == RH0B0_RQ) && 
              (length > 20) )  
        {
          if ((snaP->ruData[9] == 0x80) &&   /* PU Capabilities 0x80 Control Vector */
              (snaP->ruData[10] >= 0x01) &&  /* Vector Data Field length */
              (snaP->ruData[11] & 0x80) == 0x80) /* Supports unsolicited NMVTs for PSID */
          {
            /* Sending node supports unsolicited NMVTs for PSID 
             */ 
            snaMuP->enable = TRUE;         
          }  
        } 
      }  
      else if (snaP->ruData[index] == DACTPU) {
        if (length - SnaHeader < 2) {
          /* SNA data with invalid RU length.  
           */
          snaMuP->reqRsp = SNA_RU_LEN_ERROR; 
          return TRUE;   
        }     
        snaMuP->ruType = SNA_DACTPU;
      } 
      else if (snaP->ruData[index] == ACTLU)        
      {
        if ((length - SnaHeader) < 2) {
          /* SNA data with invalid RU length.  
           */
          snaMuP->reqRsp = SNA_RU_LEN_ERROR; 
          return TRUE;   
        }     
        snaMuP->ruType = SNA_ACTLU;

        if ( ((snaP->RH0 & RH0B0_RRI) == RH0B0_RSP) &&
             (snaMuP->rspType == SNA_POSITIVE)){
           if ((length - SnaHeader) <= 3)
             snaMuP->enable = TRUE;         
           else if (snaP->ruData[3] == 0x0C) 
              snaMuP->enable = (snaP->ruData[5] == 0x03);    /* Vector Data */
           else if (snaP->ruData[3] == 0x0 && length > 17 && snaP->ruData[8] == 0x0C)
              snaMuP->enable = (snaP->ruData[10] == 0x03);    /* Vector Data */
           else
              snaMuP->enable = FALSE;
	}
      }  
      else if (snaP->ruData[index] == DACTLU) 
        snaMuP->ruType = SNA_DACTLU;
      else if (snaP->ruData[index] == BIND)        
        snaMuP->ruType = SNA_BIND;
      else if (snaP->ruData[index] == UNBIND)        
        snaMuP->ruType = SNA_UNBIND;
      else if (snaP->ruData[index] == SDT)        
        snaMuP->ruType = SNA_SDT;
      else if (snaP->ruData[index] == CLEAR)        
        snaMuP->ruType = SNA_CLEAR;
      else 
        snaMuP->ruType = SNA_OTHER;
    }
    else {
      snaMuP->fmd = FALSE;
      snaMuP->ruType = SNA_OTHER;
    }

    /* Check if MU is a REQUEST or RESPONSE 
     */
    if ((snaP->RH0 & RH0B0_RRI) == RH0B0_RQ)     
    {
      snaMuP->reqRsp = SNA_REQUEST;
      snaMuP->systemSense = SNA_POSITIVE;  /* Not relevant */
      snaMuP->userSense   = SNA_POSITIVE;  /* Not relevant */
      snaMuP->rspType     = SNA_POSITIVE;  /* Not relevant */
    }  
    else 
      snaMuP->reqRsp = SNA_RESPONSE;

    /* Check pacing indicator */
    snaMuP->pacing = snaP->RH1 & RH1B7_PI;
  }

  return TRUE;
}                       


/*=====================================================================*
* DecodeBind
*======================================================================*
*
* Description:
*   This function examines an MU already known to be a BIND REQUEST (from
*   DecodeMu) to determine the following:
*     - LU type no
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
          )
{
  SNA   * snaP;
  byte    index; 
  hword   ruLength;
  hword   uLength;

  snaP = (SNA *)ReturnDataHandle(bufferP);
  ruLength = GetDataLength(bufferP) - TH_RH_LEN;
  if (ruLength < 27)  {
    /* Incomplete BIND. 
     */
     return FALSE;
  }
  bindDataP->slu = snaP->DAF;
  bindDataP->plu = snaP->OAF;
  bindDataP->fmProfile = snaP->ruData[2];
  bindDataP->tsProfile = snaP->ruData[3];
  if ((snaP->ruData[4] & BIND4B0_PLUChain) == BIND4B0_MultipleRU)
    bindDataP->pluChaining = TRUE;
  else
    bindDataP->pluChaining = FALSE;

  bindDataP->sluRspProtocol = NoResponse +
                      ((snaP->ruData[5] & BIND5B2B3_SLURsp) >> 4);

  bindDataP->sendRcvMode = FullDuplex +
                         ((snaP->ruData[7] & BIND7B0B1_SRM) >> 6);

  bindDataP->bsSecMaySendEB = FALSE;
  if (snaP->ruData[4] & BIND4B7_SEBI)
    bindDataP->bracketSupport = 4;  /* brackets specified in byte4 */
  else if (snaP->ruData[5] & BIND5B7_SEBI) {
    bindDataP->bracketSupport = 5;  /* brackets specified in byte5 */
    bindDataP->bsSecMaySendEB = TRUE;
  }
  else if (snaP->ruData[6] & BIND6B2_BU)
    bindDataP->bracketSupport = 6;  /* brackets specified in byte6 */
  else
    bindDataP->bracketSupport = 0;  /* no brackets */

  bindDataP->sSendPacingWindow = (snaP->ruData[8] & BIND8B2B7_SSWS);

  bindDataP->luType = snaP->ruData[14];

  /* find user data, if present.  skip over cryptography options
   * and primary LU name
   */
  index = 27 + (snaP->ruData[26] & BIND26B4B7_CLen);
  if (index >= ruLength) {
     return FALSE;  /* Incomplete BIND  */
  }
  index += snaP->ruData[index] + 1;
  if (index >= ruLength) {
     return FALSE;  /* Incomplete BIND  */
  }
  uLength = snaP->ruData[index];
  *userDataLength = uLength;
  if (index + uLength >= ruLength) {
     return FALSE;  /* Incomplete BIND  */
  }
  
  /* copy as much of userData as given buffer will fit */
  if (uLength > userDataBufferLength)
     uLength = userDataBufferLength;
  index++;
  while (uLength) {
     uLength--;
     userDataBuffer[uLength] = snaP->ruData[index+uLength];
  }

  return TRUE;
}                      

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
          ) {

  SNA *  snaP = (SNA *)ReturnDataHandle(bufferP);
  uint   length = GetDataLength(bufferP);
  uint   dataCopyLen;
  void * dataCopySrc;
  
  if ((snaP->TH0 & TH0_BBIU) == 0) {
    /* not a first segment - no RH present */
    dataCopyLen = length - TH_LEN;
    dataCopySrc = &(snaP->RH0);
  }
  else {
    *lastFrameWasEC = snaP->RH0 & RH0B7_ECI;
    dataCopyLen = length - TH_RH_LEN;
    dataCopySrc = &(snaP->ruData[0]);
  }
  *endData = (*lastFrameWasEC) && (snaP->TH0 & TH0_EBIU);
  *userDataLength = dataCopyLen;          /* return total count */
  if (dataCopyLen > userDataBufferLength)
    dataCopyLen = userDataBufferLength;        /* truncate copy */
  memcpy(userDataBuffer, dataCopySrc, dataCopyLen);

}


/*=====================================================================*
 * ChangeMuToPosRsp 
 *======================================================================*
 * Description:
 *   Changes the given MU to a positive rsp. It changes the RRI and the  
 * RTI bits to Response and positive. SDI is set to indicate that no   
 * sense data is included. BCI and ECI are set indicate that the MU is a  
 * single element chain. BBIUI and EBIUI are set to indicate that the  
 * MU is a whole BIU. Byte 2 of RH is set to zero. The length of the
 * data area is calculated and set.
 *   FALSE is returned if the MU is not a first or only segment, or was
 * originally a response or if the request did not request definite
 * response (i.e. ERI=1, or DR1I=DR2I=0).
 *   TRUE is returned if the response is ready to send.
 *
 **/
boolean ChangeMuToPosRsp(DataReq_t * bufferP
                        )
{
   byte 	          length;
   SNA           * snaP;
   SNASessionFlow  session;
   
   snaP = (SNA *)ReturnDataHandle(bufferP);
   
   /* Make sure it is a first or only segment. */
   if ((snaP->TH0 & TH0_BBIU) == 0)
      return FALSE;
   
   /* Determine if the MU is already a response, in which case this is 
    * an error, and FALSE is returned.                                 
    */
   if ((snaP->RH0 & RH0B0_RRI) == RH0B0_RSP)
      return FALSE;     
   
   /* Make sure that the request is for definite response. */
   if ( ((snaP->RH1 & RH1B3_ERI) == RH1B3_ER) ||
        (((snaP->RH1 & (RH1B0_DR1I|RH1B2_DR2I)) == 0)) )
      return FALSE;

   DecodeSession(bufferP,&session); 
   
   length = TH_RH_LEN + 1;	/* TH is 6 bytes and RH is 3 bytes */
   if ((snaP->RH0 & RH0B1B2_Category) != RH0B1B2_FMD)  {
      /* Response Category is NC, DFC or SC 
       */
      length = TH_RH_LEN + 1; /* 1 byte for request code */
   }
   else if ( ((snaP->RH0 & RH0B4_FI) == RH0B4_NSH) &&
            (session.sessionType != LU_LU)     &&
            ( (snaP->ruData[0] == 0x01) ||
	     (snaP->ruData[0] == 0x41) ||
	     (snaP->ruData[0] == 0x81) ) ) {
      /* Response Category is FMD */
      length = TH_RH_LEN + 3; /* 3 bytes for the request code */
   }               
   /* Set the length of the data area */
   SetDataLength(bufferP, length);
   
   /* Set the RH and TH */
   snaP->RH0 = (snaP->RH0 | ( RH0B0_RSP	/* Response                */
			     + RH0B6_BC	/* First in chain          */
			     + RH0B7_EC	/* Last in chain           */
			     )) & ~RH0B5_SD; /* Sense Data Not Included */

   snaP->RH1 = snaP->RH1 | RH1B3_P; /* Positive Response       */

   snaP->RH1 &= ~RH1B3_ER; 

   snaP->TH0 = snaP->TH0 | ( TH0_BBIU /* Whole BIU               */
			    + TH0_EBIU
			    );

   snaP->RH2 = 0;                                 
 
   /* Interchange OAF and DAF */  
   swab(snaP->OAF,snaP->DAF);

   return TRUE;
}                       


/*=====================================================================*
* ChangeActpuToPosRsp
*======================================================================*
*
* Description:
*   Changes the ACTPU to a positive rsp by setting the activation type, 
* ACTPU name, and resetting the data length.    
*
**/
#define ActpuFormat1   0x10
boolean ChangeActpuToPosRsp
          (DataReq_t * bufferP
          ,byte* puName
          ,byte        activationType 
          )
{
  const static char format1Data[] = {
      0,0,                /* reserved */
      0x7,               /* PU FMD-RU-USAGE */
      1,                 /* PU can receive FMD */
      0,0,0,0,0,0        /* reserved */
   };
  SNA  * snaP;

  if (ChangeMuToPosRsp(bufferP) == FALSE)
    return FALSE;
        
  snaP = (SNA *)ReturnDataHandle(bufferP);

  /* Note: for byte 1 - bit 0-1 00 reserved                 */
  /*                  - bit 2-3 00 format 0                 */
  /*                  - bit 4-7 activationType              */
  snaP->ruData[1] = activationType | ActpuFormat1;  
  
  /* copy 8 bytes of pu name into sna buffer */
  SetResourceNameInEbcdic(puName, &snaP->ruData[2]);
  
  memcpy(&snaP->ruData[10],format1Data,10);
  SetDataLength(bufferP, TH_RH_LEN + 10 + 10);  

  return TRUE;
}                       

/*=====================================================================*
* ChangeActluToPosRsp
*======================================================================*
*    
* Description:
*
*    Change actlu request to a response. We expect that 99.999% of the time, the
*    CLSI buffer containing the ACTLU request will have sufficient storage to house
*    the response. [We want to re-use the buffer if possible.] On the rare occasion
*    it doesn't we'll block waiting for a new CLSI buffer.
*
**/
DataReq_t*
ChangeActluToPosRsp (DataReq_t* oldP, SNANotifyType notifyType)

{

  SNA* snaP;
  byte  FMandTSprofile;
  DataReq_t* bufferP;
  size_t remBufSize;
  boolean newBufferCreated;

  
  /*---- Can we re-use the existing CLSI buffer? */
  remBufSize = CLSIMsgSpaceAvail((TCLSIMsg*) oldP) - offsetof(DataReq_t,fData[0]);
  if (remBufSize < (TH_RH_LEN + 16)) {

     /*---- Nope. Get new buffer. */
     bufferP = Cnstrct_DataReq(BLOCKING, TH_RH_LEN + 11);
     newBufferCreated = TRUE;
     snaP = (SNA *)ReturnDataHandle(bufferP);
  
     /*---- Copy TH + RH and the first byte of RU = (ACTLU). */
     memcpy(snaP, ReturnDataHandle(oldP), TH_RH_LEN + 1);
  }
  else {

     /*---- Yep. Can re-use buffer. */
     bufferP = oldP;
     snaP = (SNA *)ReturnDataHandle(bufferP);
     newBufferCreated = FALSE;
  }


  /* Preserve the FM profile and TS profile */
  FMandTSprofile = snaP->ruData[2];  

  if (!ChangeMuToPosRsp(bufferP)){
    if (newBufferCreated)
       CLSIMsgDestroy((TCLSIMsg *)bufferP);     /* cant pass it */
    return 0;    
  }

  /* Reset the data length since ChangeMuToPosRsp assumes data
   * length is a minimum.                                      
   */  
  SetDataLength(bufferP, TH_RH_LEN + 16);

  /* Set the RSP(ACTLU) data */
  snaP->ruData[1] = ERP; 
                                    
  snaP->ruData[2] = FMandTSprofile;
  
  /* The LU-LU Session Services Capabilities Control Vector */
  /* Vector Header */
  
  snaP->ruData[3]  = 0;
  snaP->ruData[4]  = 0x85;
  snaP->ruData[5]  = 0;
  snaP->ruData[6]  = 0;
  snaP->ruData[7]  = 0;

  snaP->ruData[8] = 0x0C;           /* Key field                 */
  snaP->ruData[9] = 0x06;           /* Length of vector data     */   
                                     
  /* Vector Data:  snaP->ruData[10] */
  /* 0-3 is reserved           */
                                    /* 4-7 SLU capability is     */
                                    /* inhibited: sessions can   */
                                    /* be neither queued nor     */
                                    /* started.                  */
  switch (notifyType)
  {
    case LU_INHIBITED:
      /* SLU capability is inhibited: */
      /*   sessions can be neither queued nor started */
      snaP->ruData[10] = 0x00;   /* Vector Data */
      break;

    case LU_AVAILABLE:
      /* SLU capability is enabled: */
      /*   sessions can be queued or started */
      snaP->ruData[10] = 0x03;   /* Vector Data */
      break;

    case LU_NOT_AVAILABLE:
      /* SLU capability is disabled: */
      /*   sessions can be queued but not started */
      snaP->ruData[10] = 0x01;   /* Vector Data */
      break;
  }

  snaP->ruData[11] = 0x00;           /* LU_LU session limit-      */
  snaP->ruData[12] = 0x01;           /*   session limit of 1      */
  
  snaP->ruData[13] = 0x00;           /* LU_LU session count       */
  snaP->ruData[14] = 0x00;
  
  snaP->ruData[15] = 0x00;          /* Reserved                  */
  
  if (newBufferCreated)
     CLSIMsgDestroy((TCLSIMsg *)oldP);
  return bufferP;
}                       


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
*   
**/
boolean ChangeMuToNegRsp
          (DataReq_t * bufferP
          ,hword       systemSense
          ,hword       userSense
          )
{
  hword length;
  SNA * snaP;
  hword j;
  
  snaP = (SNA *)ReturnDataHandle(bufferP);

  /* Make sure it is a first or only segment. */
  if ((snaP->TH0 & TH0_BBIU) == 0)
     return FALSE;
   
  /* Determine if the MU is already a response, in which case this is */
  /* an error, and FALSE is returned.                                 */
  if ((snaP->RH0 & RH0B0_RRI) == RH0B0_RSP)
    return FALSE;     

  /* Make sure that the request is for definite or exception response. */
  if ( ((snaP->RH1 & (RH1B0_DR1I|RH1B2_DR2I)) == 0) )
    return FALSE;     
   
  length = 1;
  if ( (snaP->OAF == SSCP) &&                   /* Not LU-LU Session */
            ((snaP->RH0 & RH0B4_FI) == RH0B4_NSH) && /* NS Header         */ 
            ((snaP->ruData[0] == 0x01) ||
             (snaP->ruData[0] == 0x41) ||
             (snaP->ruData[0] == 0x81)) ) 
  {
    /* Must be FMD NS Header - 3 Bytes */
    length = 3;           
  }
   
  /* Set the length of the data area */
  /* Calculate length of data to be returned */
  /* 	TH         - 6                       */                        
  /* 	RH         - 3                       */                        
  /* 	Sense Data - 4                       */                        
  /*    Data       - 0, 1 or 3  (length)     */
  SetDataLength(bufferP, 6 + 3 + 4 + length);

  /* Move data whether regardless and insert sense 
     data in the first 4 bytes of RU               */
  
  for(j=0; j<length; j++){
    snaP->ruData[j+4] = snaP->ruData[j];
  }
  snaP->ruData[0] = (byte)(systemSense >> 8);
  snaP->ruData[1] = (byte)(systemSense);
  snaP->ruData[2] = (byte)(userSense >> 8);
  snaP->ruData[3] = (byte)(userSense);

  snaP->RH0 = snaP->RH0 | ( RH0B0_RSP    /* Response                */
                          + RH0B5_SD     /* Sense Data Included     */
                          + RH0B6_BC     /* First in chain          */
                          + RH0B7_EC     /* Last in chain           */
                          );
  snaP->RH1 = snaP->RH1 | RH1B3_N;       /* Negative Response       */

  snaP->TH0 = snaP->TH0 | ( TH0_BBIU     /* Whole BIU               */
                          + TH0_EBIU
                          );
  /* Set byte 2 of RH to 0   */
  snaP->RH2 = 0;                                 
 
  /* Interchange OAF and DAF */
  swab(snaP->OAF,snaP->DAF);

  return TRUE;
}                     
  

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
boolean ChangeMuToIPR (DataReq_t * bufferP)
{
  SNA * snaP;
  
  snaP = (SNA *)ReturnDataHandle(bufferP);

  /* Make sure it is a first or only segment. */
  if ((snaP->TH0 & TH0_BBIU) == 0)
     return FALSE;
   
  /* Determine if the MU is already a response, in which case this is */
  /* an error, and FALSE is returned.                                 */
  if ((snaP->RH0 & RH0B0_RRI) == RH0B0_RSP)
    return FALSE;     

  if ((snaP->RH1 & RH1B7_PI) == 0)
     return FALSE;

  /* Set the length of the data area */
  SetDataLength(bufferP, TH_RH_LEN);
   
  /* Set the TH */
  snaP->RH0 = 0x83;
  snaP->RH1 = 0x01;
  snaP->RH2 = 0x00;

  /* Interchange OAF and DAF */
  swab(snaP->OAF,snaP->DAF);

  return TRUE;
}


/*=====================================================================*
* BuildActpu
*======================================================================*
*
* Description:
*   ACTPU is sent by the SSCP to activate a session with the PU, and to 
* obtain information about the PU.  
*   
**/
DataReq_t * BuildActpu
              (
              )
{
  SNA       * snaP;
  DataReq_t * bufferP;
  
  /* TH=6, RH=3, RU=9 */
  
  if ((bufferP = Cnstrct_DataReq (NON_BLOCKING, 18)) == 0)
     return 0;
  
  snaP = (SNA *)ReturnDataHandle(bufferP);

  /* Set the TH */
  snaP->TH0 = 0x2D;        /* FID2, whole BIU, Expedited       */
  snaP->TH1 = 0;           /* Reserved                          */
  snaP->DAF = PU;	   
  snaP->OAF = SSCP;
  snaIdentifier++;
  snaP->SNF[0] = (byte)snaIdentifier >> 8;
  snaP->SNF[1] = (byte)snaIdentifier;
  
  /* Set the 1st byte of RH */
  snaP->RH0 = 0x6B;        /* RRI    - Request                  */
                           /* RU Cat - Session Control          */
                           /* FI     - FM header follows        */ 
                           /* SDI    - Sense Data not included  */
                           /* BCI    - First in chain           */
                           /* ECI    - Last in chain            */

  /* Set the 2nd byte of RH */
  snaP->RH1 = 0x80;        /* DR1I   - Def. Rsp. 1 Indicator    */
                           /* LCCI   - RU is not compressed     */
                           /* ERI    - DR1=1, DR1=1, ER=0       */
                           /*          definite rsp requested   */
                           /* RLWI   - No larger pacing window  */           
                           /* QRI    - Response bypass TC queue */
                           /* PI     - No pacing                */
                           
  /* Set the 3rd byte of RH */
  snaP->RH2 = 0x00;        /* BBI    - Begin Bracket            */
                           /* EBI    - End Bracket              */
                           /* CDI    - Do not change direction  */
                           /* CSI    - Code 0 Selection         */
                           /* EDI    - RU is not enciphered     */
                           /* PDI    - RU is not padded         */
                           /* CEBI   - No conditional end brack.*/

  /* Set the RU */                           
  snaP->ruData[0] = 0x11;  /* Actpu Request code */
  snaP->ruData[1] = 0x01;  /* Type cold */ 
  snaP->ruData[2] = 0x01;  /* FM profile 0, TS profile 1 */
  snaP->ruData[3] = 0x05;  /* PUtype = 5, */
  snaP->ruData[4] = 0x00;  /* 6 byte field that specifies SSCP ID */
  snaP->ruData[5] = 0x00;  
  snaP->ruData[6] = 0x00;  
  snaP->ruData[7] = 0x00;  
  snaP->ruData[8] = 0x00;  
  /* No Control Vectors */

  return bufferP;
}                       
               
               
/*=====================================================================*
* BuildDactpu
*======================================================================*
*
* Description:
*  DACTPU is sent to deactivate the session between the SSCP and the PU.  
*   
**/
DataReq_t * BuildDactpu
              (
              )
{
  SNA       * snaP;
  DataReq_t * bufferP;
  
  /* TH=6, RH=3, RU=2 */
  
  if ((bufferP = Cnstrct_DataReq (NON_BLOCKING, 11)) == 0)
     return 0;

  snaP = (SNA *)ReturnDataHandle(bufferP);

  /* Set the TH */
  snaP->TH0 = 0x2D;        /* FID2, whole BIU, Expedited flow   */
  snaP->TH1 = 0;           /* Reserved                          */
  snaP->DAF = PU;	   
  snaP->OAF = SSCP;	   
  snaIdentifier++;	   
  snaP->SNF[0] = (byte)snaIdentifier >> 8;
  snaP->SNF[1] = (byte)snaIdentifier;
   
  /* Set the 1st byte of RH */
  snaP->RH0 = 0x6B;        /* RRI    - Request                  */
                           /* RU Cat - Session Control          */
                           /* FI     - FM header follows        */ 
                           /* SDI    - Sense Data not included  */
                           /* BCI    - First in chain           */
                           /* ECI    - Last in chain            */

  /* Set the 2nd byte of RH */
  snaP->RH1 = 0x80;        /* DR1I   - Def. Rsp. 1 Indicator    */
                           /* LCCI   - RU is not compressed     */
                           /* ERI    - DR1=1, DR2=1, ER=0       */
                           /*          definite rsp requested   */
                           /* RLWI   - No larger pacing window  */           
                           /* QRI    - Response bypass TC queue */
                           /* PI     - No pacing                */
                           
  /* Set the 3rd byte of RH */
  snaP->RH2 = 0x00;        /* BBI    - Begin Bracket            */
                           /* EBI    - End Bracket              */
                           /* CDI    - Do not change direction  */
                           /* CSI    - Code 0 Selection         */
                           /* EDI    - RU is not enciphered     */
                           /* PDI    - RU is not padded         */
                           /* CEBI   - No conditional end brack.*/
  
  /* Set the RU */
  snaP->ruData[0] = 0x12;  /* Dactpu Request code               */
  snaP->ruData[1] = 0x01;  /* Final use,                        */
                           /* physical connection may be broken */ 
  return bufferP;
}                       

/*=====================================================================*
* BuildActlu
*======================================================================*
*
* Description:
*   ACTLU is sent from an SSCP to an LU to activate a session between the 
* SSCP and the LU and to establish common session parameters. 
*   
**/
DataReq_t * BuildActlu
              (byte lu
              )
{
  SNA       * snaP;
  DataReq_t * bufferP;
  
  /* TH=6, RH=3, RU=3 */
  
  if ((bufferP = Cnstrct_DataReq (NON_BLOCKING, 12)) == 0)
     return 0;

  snaP = (SNA *)ReturnDataHandle(bufferP);

  /* Set the TH */
  snaP->TH0 = 0x2D;        /* FID2, whole BIU, Expedited flow   */
  snaP->TH1 = 0;           /* Reserved                          */
  snaP->DAF = lu;	   
  snaP->OAF = SSCP;	   
  snaIdentifier++;	   
  snaP->SNF[0] = (byte)snaIdentifier >> 8;
  snaP->SNF[1] = (byte)snaIdentifier;
   
  /* Set the 1st byte of RH */
  snaP->RH0 = 0x6B;        /* RRI    - Request                  */
                           /* RU Cat - Session Control          */
                           /* FI     - FM header follows        */ 
                           /* SDI    - Sense Data not included  */
                           /* BCI    - First in chain           */
                           /* ECI    - Last in chain            */

  /* Set the 2nd byte of RH */
  snaP->RH1 = 0x80;        /* DR1I   - Def. Rsp. 1 Indicator    */
                           /* LCCI   - RU is not compressed     */
                           /* ERI    - DR1=1, DR2=1, ER=0       */
                           /*          definite rsp requested   */
                           /* RLWI   - No larger pacing window  */           
                           /* QRI    - Response bypass TC queue */
                           /* PI     - No pacing                */
                           
  /* Set the 3rd byte of RH */
  snaP->RH2 = 0x00;        /* BBI    - Begin Bracket            */
                           /* EBI    - End Bracket              */
                           /* CDI    - Do not change direction  */
                           /* CSI    - Code 0 Selection         */
                           /* EDI    - RU is not enciphered     */
                           /* PDI    - RU is not padded         */
                           /* CEBI   - No conditional end brack.*/
  
  /* Set the RU */
  snaP->ruData[0] = 0x0D;  /* Actlu Request code */
  snaP->ruData[1] = 0x01;  /* Format 0, Type Activation req = retired */ 
  snaP->ruData[2] = 0x01;  /* FM profile 0, TS profile 1 */

  return bufferP;
}            


/*=====================================================================*
* BuildDactlu
*======================================================================*
*
* Description:
*   DACTLU is sent to deactivate the session between the SSCP and the LU.   
*   
**/
DataReq_t * BuildDactlu
              (byte lu     
              )
{
  SNA       * snaP;
  DataReq_t * bufferP;
  /* TH=6, RH=3, RU=1 */
  
  if ((bufferP = Cnstrct_DataReq (NON_BLOCKING, 10)) == 0)
     return 0;


  snaP = (SNA *)ReturnDataHandle(bufferP);

  /* Set the TH */
  snaP->TH0 = 0x2D;        /* FID2, whole BIU, Expedited flow   */
  snaP->TH1 = 0;           /* Reserved                          */
  snaP->DAF = lu;          /* Destination is the lu       */	   
  snaP->OAF = SSCP;	   
  snaIdentifier++;	   
  snaP->SNF[0] = (byte)snaIdentifier >> 8;
  snaP->SNF[1] = (byte)snaIdentifier;
  
  /* Set the 1st byte of RH */
  snaP->RH0 = 0x6B;        /* RRI    - Request                  */
                           /* RU Cat - Session Control          */
                           /* FI     - FM header follows        */ 
                           /* SDI    - Sense Data not included  */
                           /* BCI    - First in chain           */
                           /* ECI    - Last in chain            */

  /* Set the 2nd byte of RH */
  snaP->RH1 = 0x80;        /* DR1I   - Def. Rsp. 1 Indicator    */
                           /* LCCI   - RU is not compressed     */
                           /* ERI    - DR1=1, DR1=1, ER=0       */
                           /*          definite rsp requested   */
                           /* RLWI   - No larger pacing window  */           
                           /* QRI    - Response bypass TC queue */
                           /* PI     - No pacing                */
                           
  /* Set the 3rd byte of RH */
  snaP->RH2 = 0x00;        /* BBI    - Begin Bracket            */
                           /* EBI    - End Bracket              */
                           /* CDI    - Do not change direction  */
                           /* CSI    - Code 0 Selection         */
                           /* EDI    - RU is not enciphered     */
                           /* PDI    - RU is not padded         */
                           /* CEBI   - No conditional end brack.*/
  
  /* Set the RU */
  snaP->ruData[0] = 0x0E;  /* Dactlu Request code */

  return bufferP;
}            

/*=====================================================================*
* BuildUnbind
*======================================================================*
*
* Description:
*   UNBIND is sent to deactivate an active session, this is sent to
* the host. 
*   
**/
DataReq_t * BuildUnbind
              (byte lu
              ,byte unbindType
              ,BlockingType blkType
              )
{
  SNA       * snaP;
  DataReq_t * bufferP;

  /* TH=6, RH=3, RU=2 */
  
  if ((bufferP = Cnstrct_DataReq( blkType, 11)) == 0)
     return 0;

  snaP = (SNA *)ReturnDataHandle(bufferP);

  /* Set the TH */
  snaP->TH0 = 0x2D;        /* FID2, whole BIU, Expedited flow   */
  snaP->TH1 = 0;           /* Reserved                          */
  snaP->DAF = DEPENDANT_LU;/* Destination is the SSCP           */  	   
  snaP->OAF = lu;	     
  snaIdentifier++;	   
  snaP->SNF[0] = (byte)snaIdentifier >> 8;
  snaP->SNF[1] = (byte)snaIdentifier;
  
  /* Set the 1st byte of RH */
  snaP->RH0 = 0x6B;        /* RRI    - Request                  */
                           /* RU Cat - Session Control          */
                           /* FI     - FM header follows        */ 
                           /* SDI    - Sense Data not included  */
                           /* BCI    - First in chain           */
                           /* ECI    - Last in chain            */

  /* Set the 2nd byte of RH */
  snaP->RH1 = 0x80;        /* DR1I   - Def. Rsp. 1 Indicator    */
                           /* LCCI   - RU is not compressed     */
                           /* ERI    - DR1=1, DR1=1, ER=0       */
                           /*          definite rsp requested   */
                           /* RLWI   - No larger pacing window  */           
                           /* QRI    - Response bypass TC queue */
                           /* PI     - No pacing                */
                           
  /* Set the 3rd byte of RH */
  snaP->RH2 = 0x00;        /* BBI    - Begin Bracket            */
                           /* EBI    - End Bracket              */
                           /* CDI    - Do not change direction  */
                           /* CSI    - Code 0 Selection         */
                           /* EDI    - RU is not enciphered     */
                           /* PDI    - RU is not padded         */

  /* Set the RU */
  snaP->ruData[0] = 0x32;       /* Unbind Request code */
  snaP->ruData[1] = unbindType; 

  return bufferP;
}            

/*=====================================================================*
* BuildNotify
*======================================================================*
*
* Description:
*   Notify is used to send information from an LU to an SSCP;
* Note: we dont build notify from  SSCP to an LU.  
*
**/
DataReq_t *BuildNotify
             (byte          lu
             ,SNANotifyType notifyType
             )
{
  SNA       * snaP;
  byte        i;
  DataReq_t * bufferP;
  
  /* TH=6, RH=3, RU=19 */

  if ((bufferP = Cnstrct_DataReq (NON_BLOCKING, 28)) == 0)
     return 0;

  snaP = (SNA *)ReturnDataHandle(bufferP);

  /* Set the TH */
  snaP->TH0 = 0x2C;        /* FID2, whole BIU, Normal flow      */
  snaP->TH1 = 0;           /* Reserved                          */
  snaP->DAF = SSCP;	   
  snaP->OAF = lu;	   
  snaIdentifier++;	   
  snaP->SNF[0] = (byte)snaIdentifier >> 8;
  snaP->SNF[1] = (byte)snaIdentifier;
  
  /* Set the 1st byte of RH */
  snaP->RH0 = 0x0B;        /* RRI    - Request                  */
                           /* RU Cat - FMD                      */
                           /* FI     - FM header follows        */ 
                           /* SDI    - Sense Data not included  */
                           /* BCI    - First in chain           */
                           /* ECI    - Last in chain            */

  /* Set the 2nd byte of RH */
  snaP->RH1 = 0x80;        /* DR1I   - Def. Rsp. 1 Indicator    */
                           /* LCCI   - RU is not compressed     */
                           /* ERI    - DR1=1, DR1=1, ER=0       */
                           /*          definite rsp requested   */
                           /* RLWI   - No larger pacing window  */           
                           /* QRI    - Response bypass TC queue */
                           /* PI     - No pacing                */
                           
  /* Set the 3rd byte of RH */
  snaP->RH2 = 0x00;        /* BBI    - Begin Bracket            */
                           /* EBI    - End Bracket              */
                           /* CDI    - Do not change direction  */
                           /* CSI    - Code 0 Selection         */
                           /* EDI    - RU is not enciphered     */
                           /* PDI    - RU is not padded         */
                           /* CEBI   - No conditional end brack.*/
  
  /* Set the RU */
  snaP->ruData[0] = 0x81;       /* Notify Request code */
  snaP->ruData[1] = 0x06;       
  snaP->ruData[2] = 0x20;       
  snaP->ruData[3] = 0x0c;       /* LU-LU Sess. Services Cap NOTIFY vector */
  snaP->ruData[4] = 0x0e;       /* Length of Vector Data Field - 14 bytes*/

  switch (notifyType)
  {
    case LU_INHIBITED:
      /* SLU capability is inhibited: */
      /*   sessions can be neither queued nor started */
      snaP->ruData[5] = 0x00;   /* Vector Data */
      break;

    case LU_AVAILABLE:
    /* SLU capability is enabled: */
    /*   sessions can be queued or started */
      snaP->ruData[5] = 0x03;   /* Vector Data */
      break;

    case LU_NOT_AVAILABLE:
    /* SLU capability is disabled: */
    /*   sessions can be queued but not started */
      snaP->ruData[5] = 0x01;   /* Vector Data */
      break;
  }

  /* Remaining bytes are Retired */
  snaP->ruData[6] = 0x00;       
  snaP->ruData[7] = 0x01;       
  snaP->ruData[8] = 0x00;       
  snaP->ruData[9] = 0x00;      
  snaP->ruData[10] = 0x00;      

  for(i=11; i<19; i++)
    snaP->ruData[i] = 0x40;   /* Set all space characters to 0x40 or */
                              /* omitted                             */
  return bufferP;
}            

/*=====================================================================*
* IntToEbcdic
*======================================================================*
*
* Description:
*   Convert integer to EBCDIC.
*
**/
static void IntToEbcdic(byte* ebcdic, byte data)
{
   byte hundred = data % 100;
   ebcdic[0] = data/100 + 0xf0;
   ebcdic[1] = hundred/10 + 0xf0;
   ebcdic[2] = hundred % 10 + 0xf0;
   
}


static byte nmvtPsid[] = {
0x2C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x80, 0x00, 0x41, 
0x03, 0x8D, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 59, 0x00, 
0x90, 0x0A, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
XXXX, 0x0D, 0x82, 0x05, 0x10, XXXX, XXXX, XXXX, 0x03, 0x20,
0x01, 0x03, 0x30, 0x02, 0x19, 0x10, 0x00, 0x16, 0x11, 0x09, 
0x13, 0x00, 0x12, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x07, 
0x42, 0xEF, XXXX, XXXX, XXXX, XXXX 
};


/* We need to check if the NMVT sent to the host is sent by us. We compare
 * 15 bytes starting from RH of the NMVT to be sent to the host by the PU. If
 * they are the same, we set a flag in order to free the buffer when the 
 * response is received. Note that we start RH because the sequence number
 * of the TH is changed.
 */
boolean QueryNmvtPsid(DataReq_t* bufferP)
{
   return Memcmp (ReturnDataHandle(bufferP) +6, nmvtPsid+6, 15) == 0;
}


/*=====================================================================*
* BuildNmvtPsid
*======================================================================*
*
* Description:
*   A VTAM enhancement called Dynamic Definition of Dependent Logical
* Units (DDDLU) to define and activate LUs automatically, when the
* associated device is powered on. 
*
**/
DataReq_t * BuildNmvtPsid
              (byte lu
              )
{
  DataReq_t * bufferP;
  SNA       * snaP;

  bufferP = Cnstrct_DataReq (BLOCKING, sizeof(nmvtPsid));

  snaP = (SNA *)ReturnDataHandle(bufferP);

  nmvtPsid[30] = lu;

  IntToEbcdic(&nmvtPsid[35], lu);     
                           
  nmvtPsid[72] = (byte) (psidNmvtTransOrder >> 24);
  nmvtPsid[73] = (byte) (psidNmvtTransOrder >> 16);
  nmvtPsid[74] = (byte) (psidNmvtTransOrder >> 8);
  nmvtPsid[75] = (byte) (psidNmvtTransOrder);

  psidNmvtTransOrder++;	 /* Incremented for each record sent indicating  */
                         /*   order of transmission.*/   
  memcpy(snaP, nmvtPsid, sizeof(nmvtPsid)); 

  snaIdentifier++;	   
  snaP->SNF[0] = (byte)snaIdentifier >> 8;
  snaP->SNF[1] = (byte)snaIdentifier;
  
  return bufferP;
}              


const static byte nmvtRsp[] = {
0x2C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x80, 0x00, 0x41, 0x03, 0x8D 
};

/*=====================================================================*
* BuildNmvtRsp
*======================================================================*
*
* Description:
*   Build a NMVT +ve Rsp.
*
**/
DataReq_t * BuildNmvtRsp (
                          hword      sequenceNumber)
{
  DataReq_t * bufferP;
  SNA       * snaP;

  bufferP = Cnstrct_DataReq (NON_BLOCKING, sizeof(nmvtRsp));

  snaP = (SNA *)ReturnDataHandle(bufferP);

  memcpy(snaP, nmvtRsp, sizeof(nmvtRsp)); 
  snaP->SNF[0] = (byte)(sequenceNumber >> 8) & 0xff;
  snaP->SNF[1] = (byte)(sequenceNumber & 0xff);
  
  return bufferP;
}              


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
                    )
{
  SNA       * snaP;
  DataReq_t * bufferP;

  if ((bufferP = Cnstrct_DataReq (NON_BLOCKING, piuDataLen)) == 0)
     return 0;

  snaP = (SNA *)ReturnDataHandle(bufferP);

  /* Copy the PIU data */
  memcpy(&snaP->TH0, piuData, piuDataLen);

  /* Plug oaf */
  snaP->OAF = oaf;

  /* check RRI and DR1I,DR2I,ERI to see if it a definite response request */
  if (piuDataLen < TH_RH_LEN)
     *defreq = FALSE; /* no RU - PIU too small */
  else if ((snaP->TH0 & TH0_BBIU) == 0)
     *defreq = FALSE; /* no RU - not first or only segment */
  else if ((snaP->RH0 & RH0B0_RRI) == RH0B0_RSP)
     *defreq = FALSE; /* response */
  else if ((snaP->RH1 & RH1B3_ERI) == RH1B3_ER)
     *defreq = FALSE; /* exception response request */
  else if (((snaP->RH1 & (RH1B0_DR1I|RH1B2_DR2I)) == 0))
     *defreq = FALSE;  /* no response request */
  else
     *defreq = TRUE;
  
  return bufferP;
}


/*===========================================================================*
* BuildLuLuDataReq
*============================================================================*
*
* Description:
*   Construct an LU-LU session PIU.
*
**/
DataReq_t *
BuildLuLuDataReq (byte     oaf,
                  byte     daf,
                  hword    snaId,
                  byte     rh1,
                  byte     rh2,
                  uint     ruDataLen, /* Length of RU data   */
                  char*    ruData     /* RU data             */
                  )
{
  SNA       * snaP;
  DataReq_t * bufferP;

  if ((bufferP = Cnstrct_DataReq (NON_BLOCKING, TH_RH_LEN + ruDataLen)) == 0)
     return 0;

  snaP = (SNA *)ReturnDataHandle(bufferP);

  /* Set the TH */
  snaP->TH0 = 0x2C;        /* FID2, whole BIU, Normal flow      */
  snaP->TH1 = 0;           /* Reserved                          */
  snaP->DAF = daf;         
  snaP->OAF = oaf;
  snaP->SNF[0] = (byte)(snaId >> 8);
  snaP->SNF[1] = (byte)snaId;
  
  snaP->RH0 = 0x03;
  snaP->RH1 = rh1;
  snaP->RH2 = rh2;

  /* Copy the RU data */
  memcpy(&snaP->ruData[0], ruData, ruDataLen);

  return bufferP;
}


/*===========================================================================*
* SetPacingBit
*============================================================================*
*
* Description:
*   Turn on the pacing bit in a PIU.
*
**/
void
SetPacingBit (DataReq_t* bufferP) {
  SNA       * snaP = (SNA *)ReturnDataHandle(bufferP);
  snaP->RH1 |= RH1B7_PAC;
}


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
              )
{
  SNA       * snaP;
  DataReq_t * bufferP;
  uint bufferLen = TH_RH_LEN + 17 + applNameLen + userDataLen;
  uint x;

  if ((bufferP = Cnstrct_DataReq (NON_BLOCKING, bufferLen)) == 0)
     return 0;

  snaP = (SNA *)ReturnDataHandle(bufferP);

  /* Set the TH */
  snaP->TH0 = 0x2C;        /* FID2, whole BIU, Normal flow      */
  snaP->TH1 = 0;           /* Reserved                          */
  snaP->DAF = 0;           /* sscp */
  snaP->OAF = oaf;
  snaP->SNF[0] = (byte)snaId >> 8;
  snaP->SNF[1] = (byte)snaId;
  
  snaP->RH0 = 0x0B;
  snaP->RH1 = 0x80;
  snaP->RH2 = 0x00;

  /* Set the RU */
  snaP->ruData[0] = 0x01;       /* Initself Request code */
  snaP->ruData[1] = 0x06;       
  snaP->ruData[2] = 0x81;       
  snaP->ruData[3] = 0;       

  /* Copy the mode name */
  memcpy(&snaP->ruData[4], modeName, 8);

  /* Add in the application name */
  snaP->ruData[12] = 0xF3;
  snaP->ruData[13] = applNameLen;
  if (applNameLen)
     memcpy(&snaP->ruData[14], applName, applNameLen);

  x = 14 + applNameLen;
  snaP->ruData[x++] = 0;   /* retired field */
  snaP->ruData[x++] = 0;   /* retired field */

  /* Add in the user data */
  snaP->ruData[x++] = userDataLen;
  if (userDataLen)
     memcpy(&snaP->ruData[x], userData, userDataLen);

  return bufferP;
}


/*=====================================================================*
* SnaInsertXid3Parms
*======================================================================*
*
* Description:
*   Insert XID3 parameters into structure.
*
**/
static void
SnaInsertXid3Parms (byte *xidP,
                    byte* xid,
                    byte* puName,
                    hword maxIFrame,
                    byte windowSize,
                    boolean xid3negotiation)

{

  byte cv_offset;  /* Offset of control vectors within XID3       */


  xidP[0] = 0x32;  /* 0-3 Format of XID I-field            */
                   /*   for T2.1 - T2.1|4|5 node exchanges */
                   /* 4-7 Type of the XID sending node     */
                   /*   T2                                 */
  xidP[1] = XID3_SZ;  /* Length in binary, of variable        */
                   /* format XID I-field  (29 bytes)       */

  memcpy(&xidP[2], xid, 4);   /* copy 4 bytes of id */

  xidP[6] = 0;     /* Reserved */
  xidP[7] = 0;     
  
  xidP[8] = 0x80;  /* Characteristics of the XID sender           */
                   /* INIT-SELF can not be sent to the XID sender */
                   /* BIND may be sent to the XID sender          */
                   /* This node can generate BIND PIU segments    */
                   /* This node can  receive BIND PIU segments    */
                   
  xidP[9] = 0x4;  /* ACTPU for an SSCP-PU session requested      */
                   /* The sender is not a network node            */
                   /* CP services not requested or provided       */
                   /* CP-CP sessions not supported on this TG     */
                   /* Exchange state indicators                   */
                   /* Nonactivation exchange started by secondary */
                   /*   station support                           */
                   /* CP name can not change                      */

#ifdef FixedVtam41bug  
  if (xid3negotiation)
     xidP[9] = 8;  /* prenegotiation exchange                     */
#endif
          
  xidP[10] = 0x11; /* adapt. BIND pacing supported (sender)       */
                   /* adapt. BIND pacing supported (receiver)     */
                   /* receiving node generate a topology update   */
                   /* supports receiving ACTPU with X'80' vector  */
                   /* APPN border node not supported              */
                   /* Adaptive BIND applies to BINDS for BOTH     */
                   /* independent and dependent LUs unless overridden */
                   
  xidP[11] = 0;    /* Reserved */
  xidP[12] = 0;    
  xidP[13] = 0;    
  xidP[14] = 0;    
                                 
  xidP[15] = 0;    /* parallel TG not supported */
  
  xidP[16] = 0;    /* Transmission group number */
  
  xidP[17] = 0x01; /* SDLC                      */
  

  xidP[18] = 11; /* Length of SDLC            */
  
  xidP[19] = 0x41; /* ABM combined station                          */
                   /* sender is a secondary  link station            */
                   /* Link Stat. transmit-receive is two-way simul. */

  xidP[20] = 0;    /* XID is not the initiator of a nonactivation   */

  xidP[21] = (byte)(maxIFrame >> 8);
  xidP[22] = (byte)(maxIFrame);

  xidP[23] = 0;    /* SNA link profile */     
                   
  xidP[24] = 0;    /* SIM and RIM not supported */     

  xidP[25] = 0;    /* Reserved */
  xidP[26] = 0;    /* Reserved */
  
  xidP[27] = windowSize;    
  
  xidP[28] = 0;    /* Reserved */

  /*
   * Set cv_offset to start of XID3 control vector fields
   *
   */
  cv_offset = XID3_SZ;


  /* 
   * Add CV 0x0E -- Network Name control vector
   *     Network Name type: 0xF4 (CP name)
   *
   * Update XID3 length field to include total bytes added with CV 0x0E
   *
   */
  xidP[cv_offset++] = 0x0e;                   /* CV 0x0E key            */
  xidP[cv_offset++] = PU_NAME_SZ+1;           /* CV 0x0E length         */
  xidP[cv_offset++] = 0xf4;                   /* Network name type 0xF4 */
  ConvertToEbcdicPuName(puName, &xidP[cv_offset]);
  cv_offset += PU_NAME_SZ;
  xidP[1] += (PU_NAME_SZ+3);
  

  /* 
   * Add CV 0x10 -- Product Set ID control vector
   *
   *   Since static CV 0x10 vector uses LT rule for vector key/length
   *     must reset key/length to use KL rule
   *
   * Update XID3 length field to include total bytes added with CV 0x10
   *
   */
  memcpy(&xidP[cv_offset], &productSetId, CV10_SZ);
  xidP[cv_offset]   = 0x10;
  xidP[cv_offset+1] = (CV10_SZ-2);
  xidP[1] += CV10_SZ;             

}  /* SnaInsertXid3Parms() */


/*=====================================================================*
* Build_Xid3_Req
*======================================================================*
*
* Description:
*   Construct an XID(3) ID.REQ signal.
*
**/
IdReq_t *
Build_Xid3_Req (byte* xid,
                byte* puName,
                hword maxIFrame,
                byte  windowSize,
                boolean xid3Negotiation)

{

  IdReq_t *idReqP;
  byte xidLen;
  byte xidP[XID3_MAX_SZ];
  

  SnaInsertXid3Parms(xidP, xid, puName, maxIFrame, windowSize, xid3Negotiation);
  xidLen = xidP[1];  /* xidP[1] contains XID length after SnaInsert */
  idReqP = Cnstrct_IdReq (NON_BLOCKING, 0L, xidP, xidLen);

  return (idReqP);

}  /* Build_Xid3_Req. */


/*=====================================================================*
* Build_Xid3_Rsp
*======================================================================*
*
* Description:
*   Construct an XID(3) ID.RSP signal.
*
**/
IdRsp_t *
Build_Xid3_Rsp ( byte* xid,
                byte* puName,
                hword maxIFrame,
                byte  windowSize)

{

  IdRsp_t *idRspP;
  byte xidLen;
  byte xidP[XID3_MAX_SZ];

  SnaInsertXid3Parms(xidP, xid, puName, maxIFrame, windowSize, FALSE);
  xidLen = xidP[1];  /* xidP[1] contains XID length after SnaInsert */
  idRspP = Cnstrct_IdRsp (NON_BLOCKING, 0L, xidP, xidLen);

  return (idRspP);

}  /* Build_Xid3_Rsp. */


/*=====================================================================*
* ReturnSequenceNumber
*======================================================================*
*
* Description:
*   Returns the sequence number.
*
**/
hword ReturnSequenceNumber(DataReq_t* clsiBfrP)
{
  SNA * snaP;
 
  snaP = (SNA *)ReturnDataHandle(clsiBfrP);
  return (snaP->SNF[0] << 8) + snaP->SNF[1];
}                        

/*=====================================================================*
* SaveSequenceNumber
*======================================================================*
*
* Description:
*   Saves the given sequence number.
*
**/
void SaveSequenceNumber(DataReq_t* clsiBfrP, 
                        hword      sequenceNumber)
{
  SNA * snaP;

  snaP = (SNA *)ReturnDataHandle(clsiBfrP);
  snaP->SNF[0] = (byte)(sequenceNumber >> 8) & 0xff;
  snaP->SNF[1] = (byte)(sequenceNumber & 0xff);
} 

/*=====================================================================*
* GetNmvtMV
*======================================================================*
*
* Description:
*   Returns major vector of NMVT
*
**/
byte* GetNmvtMV(DataReq_t* clsiBfrP)
{
   SNA* snaP = (SNA *)ReturnDataHandle(clsiBfrP);
   if (snaP->ruData[0] == 0x41 && snaP->ruData[1] == 0x3 && 
       snaP->ruData[2] == NMVT){
           return &snaP->ruData[8];
   }
   return 0;
}
