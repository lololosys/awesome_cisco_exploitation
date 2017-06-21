/* $Id: mv_sent.c,v 3.2.58.1 1996/03/18 19:33:59 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/dspu/mv_sent.c,v $
 *------------------------------------------------------------------
 * SNA Network Management Major Vector sent to focal point
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mv_sent.c,v $
 * Revision 3.2.58.1  1996/03/18  19:33:59  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  08:45:58  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  13:48:31  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:05:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/20  17:03:30  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.2  1995/08/25 11:43:33  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.1  1995/06/07 20:29:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include "../ibm/ibm_types.h"
#include "dspu_fp.h"
#include "mv_sr.h"
#include "mv_datah.h"
#include "dspu_pu.h"
#include "snaprim.h"
#include "snapiu.h"



/*=====================================================================*
*  CreateNmvtClsi
*======================================================================*
*
* Description:
* Construct a nmvt data request buffer.
* 1. construct nmvt header, and set up nmvt sequence and correlator.
* 2. copy major vector. If major vector is replyRuncmd and it is the
*    firstNMVT, then we need to add vector 44.
* 3. Then copy the rest of data. Note that the last data chain may
*    not all copied. 
*
*/

typedef enum NmvtSequence {
   OnlyNmvt = 0,
   LastNmvt = 0x20,
   FirstNmvt = 0x40,
   MiddleNmvt = 0x60
}NmvtSequence;

#define SolicitedNmvt  0x80

static const byte nmvtHeader[] = {
   0x2c, 00, 00, 00, 00, 01,   /* th */
   0xb, 0x80, 00,              /* rh */
   0x41,0x03, 0x8d, 0, 0,      /* ns header */
   0, 0,                       /* prid, offset = 14  */
   SolicitedNmvt               /* solicited nmvt */
};

static const byte replyRuncmd[] = {
   0,    0x61,       /* reply runcmd  */
   0x12, 0x44,       /* 18 bytes, sub vec 44 */
   0x04, 0x01, 0, 1, /* count subfield */
   0x06, 0x10, 0, 0, 0, 0,  /* nmvt counts */
   0x06, 0x11, 0, 0, 2, 0   /* maximum nmvt size = 512 */
};


#define SubVecLen (sizeof(replyRuncmd) - 2)
/* The maximum data length sent by the RUNCMD is restricted to 5 buffers with 1024 bytes.
 * refer to nm_runcmd. 
 * The maximum length pass to the host will be:
 * 1024*5 + 200 bytes overhead(productset id etc) + 11 * (NmvtHeader + replyRuncmd) <
 *   521 * 11.
 * 11 buffers of 521 bytes data length must be enough to hold the data
 */
#define MaxBuf           11
#define MaxSscpDataLength 521

static DataReq_t* CreateNmvtClsi(DataHeader** head,   /* first of data chain */
                                 byte        sequence,
                                 byte*       correlator,
                                 hword       correlatorLen)
{
    byte* vector;
    DataHeader* dataP = *head;
    DataReq_t* clsiBfrP;
    hword       dataLenToSend; /* data to vector     */

    if ((dataLenToSend = DataHeaderFindTotalLen(dataP) + sizeof(nmvtHeader)) > MaxSscpDataLength){
        dataLenToSend = MaxSscpDataLength;
        if (sequence == OnlyNmvt)
            sequence = FirstNmvt;
     }
    else if (sequence == MiddleNmvt)
       sequence = LastNmvt;

    clsiBfrP = Cnstrct_DataReq(NON_BLOCKING, dataLenToSend);
    if (clsiBfrP != 0){
       SNA* snaP = (SNA *)ReturnDataHandle(clsiBfrP);
       
       /* copy nmvt header */
       memcpy((byte*) snaP, nmvtHeader, sizeof(nmvtHeader));
       dataLenToSend -= sizeof(nmvtHeader);  /* dataLenToSend = room allowed to copy data */
       snaP->ruData[7] |= sequence;
       vector = &snaP->ruData[8];      /* start of major vector */

       if (correlatorLen == 0 || correlator == 0){
          snaP->ruData[5] = 0;
          snaP->ruData[6] = 0;
          snaP->ruData[7] &= ~SolicitedNmvt;  /* no correlator => unsolicited */
       }
       else {
          snaP->ruData[5] = correlator[0];
          snaP->ruData[6] = correlator[1];

          /* check if to add vector 44 for solicted nmvt */
          if (sequence == FirstNmvt){

             /* add sub vector 44, add 18 bytes to major vector count */

             hword mvLen = MV_Len(((MV*)(dataP->dataBytes))) + SubVecLen;     /* 18 bytes of vec 44 */
             SetMvLen(((MV*)vector), mvLen);    /* set major vector length */
             memcpy(vector + 2, replyRuncmd, sizeof(replyRuncmd));

             /* note that we had copied 4 bytes of data (MV Len and Key) from the data chain.
              * and 18 bytes of vector 44. 
              */
             vector += SubVecLen + 4;         /* next sub vector */
             dataP->dataBytes += 4;   /* bypass the major vector */
             dataP->dataLength -= 4;
             dataLenToSend -= SubVecLen + 4;        /* we have used up 22 bytes */
          }
       }

       while (dataP){       /* if some more to send */
          if (dataLenToSend >= dataP->dataLength){
              dataLenToSend -= dataP->dataLength;
              memcpy(vector, dataP->dataBytes, dataP->dataLength);
              vector += dataP->dataLength;
              dataP = dataP->next;
	  }
          else {
             memcpy(vector, dataP->dataBytes, dataLenToSend);    /* add the rest */
             dataP->dataBytes += dataLenToSend;
             dataP->dataLength -= dataLenToSend;
             break;
          }
       }
       *head = dataP;
    }
   return clsiBfrP;
}
  


/*=====================================================================*
*  MV_VectorSent
*======================================================================*
*
* Description:
*   This is called by MVSR to send a major vector
*/

/* WARNING:  this routine can be reentrant or called by interrupt */
   
#define DateTimeRelative   0x42


MV_SV_RC
MV_VectorSent(DataHeader* vector, char* correlator, hword correlatorLen)
{
   hword totalBuf = 0;
   hword i;
   NmvtSequence sequence = OnlyNmvt;
   DataReq_t* buffer[MaxBuf];
   MV* mv;
   byte* sv;
   MV_SV_RC rc;

   while (vector){
        DataReq_t* buf;
        buf = CreateNmvtClsi(&vector, sequence, correlator, correlatorLen);
        sequence = MiddleNmvt;
        if (buf && totalBuf < MaxBuf)
             buffer[totalBuf++] = buf;
        else {
            for (i = 0; i < totalBuf; i++)
                DspuCLSIMsgDestroy(buffer[i]);
            return MV_SV_Overflow;
        }

   }

   /* Now check if there is date time subvector, if so, update to
    * the current time
    */
   mv = (MV*)GetNmvtMV(buffer[0]);    /* get major vector */
   sv = (byte*) MV_FindSV(mv, DateTimeRelative);  /* check if any time */
   if (sv != 0){
      ulong clock;

      GET_TIMESTAMP32(clock);	/* this operation is atomic */
      sv[2] = 0xa0;       /* in milliseconds */
      sv[3] = clock >> 24;
      sv[4] = clock >> 16;
      sv[5] = clock >> 8;
      sv[6] = clock;
   }

   /* If there is a 44 subvector, update total number of buffers */

   if ((sv = (byte*) MV_FindSV(mv, 0x44)) != 0){
       sv[11] = totalBuf;   /* number of nmvt */
   }
   if ((rc = Fp_VectorSent(totalBuf, buffer)) == MV_SV_OK)
       return MV_SV_OK;

   for (i = 0; i < totalBuf; i++)
      DspuCLSIMsgDestroy(buffer[i]);
   return rc;

}

