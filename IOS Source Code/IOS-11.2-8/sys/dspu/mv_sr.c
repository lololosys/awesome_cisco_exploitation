/* $Id: mv_sr.c,v 3.2.60.1 1996/03/18 19:34:01 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/dspu/mv_sr.c,v $
 *------------------------------------------------------------------
 * SNA Network Management Product Major Vector API
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mv_sr.c,v $
 * Revision 3.2.60.1  1996/03/18  19:34:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:46:00  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:48:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:05:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:56:37  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/20  17:03:31  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.1  1995/06/07 20:29:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "../ibm/ibm_types.h"

#include "mv_sr.h"
#include "dspu_fp.h"
#include "mv_parse.h"
#include "mv_datah.h"
#include "mv_sent.h"
#include "mv_parse.h"
#include "mv_prodid.h"
#include "snautil.h"



static queuetype mvsrQ;    /* header of the queue */


static word
MVSR_Receive(MV*     majorVector,
             hword   vecLen,
             char*   correlator,
             hword   correlatorLength);

static void FpIsInactive(void);
static void FpIsActive(void);

/*=====================================================================*
* MVSRinit
*======================================================================*
*
* Description:
*
*   This is called by system initialistion to register the API that we are
* ready to accept runcmd
**/
void MVSRinit (void)
{
   RegisterReceive(MVSR_Receive);    /* setup receive function */
   RegisterFpUp(FpIsActive);
   RegisterFpDown(FpIsInactive);
   queue_init(&mvsrQ, 0);
   
}

/*=====================================================================*
* MajorVectorClient
*======================================================================*
*
* Description:
*
*   Check if any clients wants to handle a major vector. Return TRUE
* if there is a client
**/
static boolean MajorVectorClient (hword mvId)
{
    MVReceiver* p = mvsrQ.qhead;
    while (p && p->applIdLen){
       if (p->mvId == mvId)
          return TRUE;
       p = p->next;
    }
    return FALSE;
}

/*=====================================================================*
* FindClients
*======================================================================*
*
* Description:
*
*   Find if there is a client wants to accept a major vector and apppl name
**/
static MVReceiver* FindClients (hword mvId, byte* applName)
{
    MVReceiver* p = mvsrQ.qhead;
    if (applName == 0){
        while (p && p->applIdLen)
            p = p->next;
        while (p && p->mvId != mvId)
            p = p->next;
        return p;
    }
    else {
      while (p && p->applIdLen){
         if (p->mvId == mvId && bcmp(p->applId, applName, 8) == 0)
             return p;
         p = p->next;
     }
    }
    return 0;
}

static boolean isFpActive;

/*=====================================================================*
* FpIsInactive, FpIsActive
*======================================================================*
*
* Description:
*
*   These are 2 virtual funtions to handle focal point pu is active or inactive.
* Note that focal point PU is part of DSPU and is therefore not allow to call us
* directly.
**/
static void FpIsInactive (void)
{
   MVReceiver* client;
   if (isFpActive)
      buginf("\nSNA: Connection to Focal Point SSCP lost");
   isFpActive = FALSE;
   for (client = mvsrQ.qhead; client; client = client->next)
        client->fpInactive(client);       /* tell the clients */
}

static void FpIsActive (void)
{
   SetSequenceNumber(GetFocalPuName());
   buginf("\nSNA: Connection to Focal Point SSCP established");
   isFpActive = TRUE;
}

/*=====================================================================*
* MVSR_Receive
*======================================================================*
*
* Description:
*
*   This funtion receives the runcmd that we had registered sent from focal point.
**/
#define NameListSubVec 0x6
#define DestApplName   0x50
static word
MVSR_Receive (MV*     majorVector,
             hword   vecLen,
             char*   correlator,
             hword   correlatorLength)
{
   SV* svP;
   byte* applName = 0;
   MVReceiver* client;

   if ((svP = MV_FindSV(majorVector, NameListSubVec)) != 0 &&
        (svP = SV_FindSV(svP,DestApplName )) != 0){
          applName = (byte*)svP + 2;
   }
   if ((client = FindClients(MV_Key(majorVector),applName)) != 0)
      return client->vecRcv(client, majorVector, vecLen,correlator,correlatorLength); 

   else if (applName != 0){
      if ((client = FindClients(MV_Key(majorVector),0)) != 0)
         return client->vecRcv(client, majorVector, vecLen,correlator,correlatorLength); 
      else if (MajorVectorClient(MV_Key(majorVector)))
         return IllegalAppl;    /* wrong appl name */
   }
   return NoClients;
}

/* A dummy routine for the clients when fp is not active */
static void FpInactive(MVReceiver* this)
{
}

/*=====================================================================*
* MV_OpenReceive
*======================================================================*
*
* Description:
*
*   This funtion open the receiver by putting it in the queue.
**/
MV_OpRcv_RC
MV_OpenReceive (MVReceiver* this)
{
   this->fpInactive = FpInactive;
   if (FindClients(this->mvId, this->applIdLen? this->applId: 0))
       return MV_OpRcv_Duplicate;
   else if (this->applIdLen)
       p_requeue(&mvsrQ, this);
   else
      p_enqueue(&mvsrQ, this);
   return MV_OpRcv_OK;
}


/*=====================================================================*
* MVR_MVReceiver
*======================================================================*
*
* Description:
*
*   This is the construtor for a client who register for a major vector
**/

MVReceiverGeneric*
MVR_MVReceiver (hword mvId,               /* major vector */
                char * applId,            /* null or id   */
                MVReceive* rcvProc,       /* proc to received */
                hword sizeofMVReceiverGeneric,
                MV_OpRcv_RC* rc)

{
   MVReceiver* this = malloc(sizeofMVReceiverGeneric);
   if (this) {
      this->applIdLen = applId? strlen(applId): 0;
      if (this->applIdLen > 8){
         *rc = MV_OpRcv_Invalid;
      }
      else{
         if (this->applIdLen)
           ConvertToEbcdicName(applId, this->applId);

        this->vecRcv = rcvProc;
        this->mvId = mvId;
        *rc = MV_OpenReceive(this);
      }
     if (*rc != MV_SV_OK){
        free(this);
        this = 0;
     }
   }
   else
      *rc = MV_OpRcv_NoMem;
   return this;
}

/*=====================================================================*
* MVR_Delete, MV_CloseReceive
*======================================================================*
*
* Description:
*
*   This is the destructor. 
**/

void MVR_Delete (MVReceiver* this)   /* calls deregistration */
{
   free(this);
}

/* deregister receive */
hword 
MV_CloseReceive (MVReceiver* client)
{
   p_unqueue(&mvsrQ, client);         /* remove from the queue */
   MVR_Delete(client);
   return 0;
}

/*=====================================================================*
* InsertVector
*======================================================================*
*
* Description:
*
*   For every major vector sent, we will insert a sub vector. Normally this is 
* the product set id. Here we create a new data chain pointing to the new vector,
* and split the first data chain to point into 2.
**/

static boolean InsertVector (DataHeader* chain,   /* current data chain */
                  byte* sv,                      /* location to be added */              
                  byte* newSv,                   /* new vector */
                  byte  newVecLen)
{
   DataHeader* newChain;
   DataHeader* residueChain;
   MV* mv;

   if ((newChain = DataHeader_new()) == 0 ||
            (residueChain = DataHeader_new()) == 0){
      if (newChain)
          DataHeader_delete(newChain);
      return FALSE;
   }
   newChain->dataBytes = newSv;
   newChain->dataLength = newVecLen;
   newChain->next = residueChain;
   residueChain->next = chain->next;
   residueChain->dataBytes = (byte*)sv;
   residueChain->dataLength = chain->dataLength - (residueChain->dataBytes - chain->dataBytes);
   mv = (MV*)chain->dataBytes;
   SetMvLen(mv,MV_Len(mv) + newVecLen);

   chain->next = newChain;
   chain->dataLength = sv - chain->dataBytes;
   return TRUE;
}

/*=====================================================================*
* MV_SendVector, MV_SendAlert, MV_PuRegSend
*======================================================================*
*
* Description:
*
*   Send data to the focal point. 
**/

MV_SV_RC       /* (OK, NoFp, Overflow) */
MV_SendVector (DataHeader* chain, char* correlator, hword correlatorLength)
{
   if (InsertVector(chain, (byte*)&((MV*)chain->dataBytes)->sv, (byte*)&productSetId, sizeof(productSetId)))
       return MV_VectorSent(chain, correlator, correlatorLength);
   return MV_SV_NoMem;
}



MV_SV_RC       /* (OK, NoFp, Overflow) */
MV_SendAlert (MV* alert)
{
   DataHeader* chain;
   MV_SV_RC rc = MV_SV_NoMem;

   if ((chain = DataHeader_new()) == 0)
       return MV_SV_NoMem;

   chain->next = 0;
   chain->dataBytes = (byte*)alert;
   chain->dataLength = MV_Len(alert);
   if (InsertVector(chain, (byte*)&alert->sv, (byte*)&productSetId, sizeof(productSetId)))
       rc = MV_VectorSent(chain, 0, 0);

   DataHeader_deleteChain(chain);
   if (rc !=  MV_SV_OK){
       uchar* sv = (uchar*) MV_FindSV(alert, 0x92);
       char* reason = "";
       if (rc == MV_SV_NoFP){
           reason = "Focal point PU not configured";
       }
       else if (rc == MV_SV_Overflow)
           reason = "Focal point buffer overflowed";
       if (sv){
          buginf("\nSNA: Alert %02x%02x%02x%02x not sent, %s",
                         sv[7], sv[8], sv[9], sv[10], reason);
       }
       else
          buginf("\nSNA: Alert unknown ID not sent, %s",reason);
   }
   return rc;
}

MV_SV_RC       /* (OK, NoFp, Overflow) */
MV_PuRegSend (DataHeader* chain)
{
   if (InsertVector(chain, (byte*)&((MV*)chain->dataBytes)->sv, 
                    (byte*)&productSetId, sizeof(productSetId)))
        return MV_VectorSent(chain, 0, 0);
   return MV_SV_NoMem;
}

/* API functions for SNA Message to Operator support */

/*=====================================================================*
* mto_fill_nameVector
*======================================================================*
*
* Description:
*
* This function is called to place an array of ASCII unsigned characters
* into the name vector structure that can be sent in an unsolicited Message 
* to Operator to the focal point.  This function should be called before 
* invoking MV_MessageToOperator.
**/
void mto_fill_nameVector (struct mto_name_subvect * nameVect,
                          uchar * buff)
{
   ConvertToEbcdicName(buff, nameVect->name);
   nameVect->next = NULL;
}

/*=====================================================================*
* mto_fill_selfDefTextVector
*======================================================================*
*
* Description:
*
* This function is called to place a null-terminated ASCII message string 
* into a subvector structure that can be sent in an unsolicited Message 
* to Operator to the focal point.  This function should be called before
* invoking MV_MessageToOperator.
**/
boolean mto_fill_selfDefTextVector (struct mto_selfDefText_subvect * subVect,
                                    unsigned char * buff)
{
   unsigned char buffLength = strlen(buff);

   if (buffLength >= 80) {
       subVect->next = NULL;
       return FALSE;
   }

   subVect->selfDefinedTextLen = 2 + 6 + 2 + buffLength;
   subVect->selfDefinedTextKey = MTO_SDT_KEY;
   subVect->codeSetLen = 6;
   subVect->codeSet = MTO_CODESET_KEY;
   subVect->charSetHo = 2;
   subVect->charSetLo = 0x80;
   subVect->codePageHi= 1;
   subVect->codePageLo = 0xf4;
   subVect->textLen = 2 + buffLength;
   subVect->textKey = MTO_SDTSUB_KEY;
   ConvertAsciiToEbcdic(buff, subVect->textBuff, buffLength);
   subVect->next = NULL;
  
   return TRUE;
}

/*=====================================================================*
* MV_MessageToOperator
*======================================================================*
*
* Description:
*   This function is called to send an "Unsolicited Message to Operator"
*   to the focal point.  This type of message is often sent when a NetView 
*   Alert is not appropriate, but it is desirable to send some notification 
*   to NetView.  The buffer used for Unsolicited Message to Operator is 
*   constructed as follows:
*  
*   Byte 0-3:         2-Byte Length and Key for the "Send Message to Operator"
*                     Major Vector (Key = 0x006F). 
*   Byte 4-5:         Length and Key (0x06) for the "Name List" SubVector.
*   Byte 6-7:         Length and Key (0x01) for "Associated Resource Name List."
*   Byte 8-I:         Length + EBCDIC character array for each name in list.
*   Byte (I+1)-K:     Length + Key (0x50) + non-NULL-terminted string 
*                     describing the Destination Application Name.
*   Byte (K+1)-(K+4): 2-byte Text Length and text key (0x1300) where length
*                     includes all text that follows.   
*/
MV_SV_RC MV_MessageToOperator (uchar* applName,
                               struct mto_name_subvect * nameList,
                               struct mto_selfDefText_subvect * textList)
{
    uchar   buf[MTO_BUFFSIZE];
    ushort len = 0;
    uchar  oldTextLen = 0;
    struct mto_major_vector * majVect;
    struct mto_name_subvect * nlVect;
    struct mto_applName_subvect * applVect;
    struct mto_text_subvect * textVect;
    struct mto_selfDefText_subvect * sdtVect;
    DataHeader vec;

    majVect = (struct mto_major_vector *) buf;
    majVect->mvLenHi = 0;
    majVect->mvLenLo = 0;
    majVect->mvKeyHi = 0x00;
    majVect->mvKeyLo = MTO_MV_KEY;
    majVect->nameListLen = 0x02;
    majVect->nameListKey = MTO_NL_KEY;

    if (nameList == NULL) {
       /* If no name vectors, we don't need
        * majVect->resourceNameLen and majVect->resourceNameKey */    
        len += sizeof(struct mto_major_vector) - 2;
    }
    else {
        majVect->associatedNameLen = 0x02;
        majVect->associatedNameKey = MTO_AN_KEY;
        len += sizeof(struct mto_major_vector);

        while (nameList) {
            nlVect = (struct mto_name_subvect *) &buf[len];
            memcpy(nlVect->name, nameList->name, MTO_NAMELEN);
            nlVect->nameLen = MTO_NAMELEN + 1;
            majVect->associatedNameLen += MTO_NAMELEN + 1;
            len += MTO_NAMELEN + 1;
            nameList = nameList->next;
            if (len > (MTO_BUFFSIZE - 20))
                return MV_SV_Overflow;
        }
    }

    /* Fill in the application vector */
    if (applName)
    {
        applVect = (struct mto_applName_subvect *) &buf[len];
        applVect->applNameLen = MTO_NAMELEN + 2;
        applVect->applNameKey = MTO_AP_KEY;
        ConvertToEbcdicName(applName, applVect->applName);
        len += MTO_NAMELEN + 2;
    }

    /* Update the Major Vector Length and the Name List Subvector length */
    majVect->mvLenLo = len;
    majVect->nameListLen = len - 4;

    /* Fill in the text length and key */
    textVect = (struct mto_text_subvect *) &buf[len];
    textVect->textLenHi = 0;
    textVect->textLenLo = 4;
    textVect->textKeyHi = MTO_TX_KEY;
    textVect->textKeyLo = 0x00;
    len += sizeof(struct mto_text_subvect);

    /* Copy in the text subvectors */
    while (textList) {
        sdtVect = (struct mto_selfDefText_subvect *) &buf[len];
        memcpy(sdtVect, textList, textList->selfDefinedTextLen);
        len += textList->selfDefinedTextLen; 
        
        /* Adjust text len assuming that 0 < selfDefinedTextLen < 255 */
        oldTextLen = textVect->textLenLo;
        textVect->textLenLo += textList->selfDefinedTextLen;
        if (textVect->textLenLo < oldTextLen) /* Did it wrap */
           (textVect->textLenHi)++;

        textList = textList->next;
        if (len > (MTO_BUFFSIZE - 5))
            return MV_SV_Overflow;
    }

    vec.dataBytes = (byte*) &buf;
    vec.dataLength = len;
    vec.next = 0;

    return MV_VectorSent(&vec, 0, 0);
}
