/* $Id: dspu_alert.c,v 3.1.62.2 1996/05/17 10:47:27 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_alert.c,v $
 *------------------------------------------------------------------
 * DSPU Alert PassThrough services
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * This module contains the methods for processing inbound Alerts,
 * and adding the appropriate hierarchical vectors.
 *------------------------------------------------------------------
 * $Log: dspu_alert.c,v $
 * Revision 3.1.62.2  1996/05/17  10:47:27  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.50.4  1996/05/03  17:18:16  schiang
 * CSCdi56627:  DSPU/NCIA interface is broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.50.3  1996/04/26  07:35:07  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.1.50.2  1996/04/03  14:02:28  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.1.50.1  1996/03/28  21:07:24  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *   Missed files during VDLC commit
 *
 * Revision 3.1.62.1  1996/03/18  19:33:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.28.2  1996/03/07  08:45:22  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1.28.1  1996/02/20  13:47:22  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.1  1995/11/09  11:27:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/20  17:02:43  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>

#include "dspu.h"
#include "dspu_pu.h"
#include "dspu_ls.h"
#include "dspu_sap.h"
#include "dspu_adaptor.h"
#include "dspu_alert.h"
#include "snaprim.h"
#include "snapiu.h"
#include "snautil.h"
#include "dspu_error.h"
#include "dspu_fp.h"

#include "interface_generic.h"

/*---------------------------------------------------------------------*
* ALERT data and constant 
*----------------------------------------------------------------------*
*
* Description:
*   Hierachy Name list, Hierachy Resource and CNM vector data
* 
*
**/


#define NAME 0,0,0,0,0,0,0,0 
#define ALERT             0
#define BasicAlert        0x91
#define GenericAlert      0x92
#define HierachyName      0x03
#define HierachyResource  0x05

/* 
*   There are 3 alert data supported, generic alert, basic alert and recfms.
* The basic principle is to add 3 name lists so netview can display the system
* correctly as shown
*
*   hostPu -- dspu --  downStreamPu
*
* where hostPu is the upstream pu name defined, dspu is the lan where
* downStreamPu is connected. downStreamPu is the name list sent by the
* down stream in the alert. If there is no name list, then downStream
* pu name is used.
*
* Implementation:
*   3 tables are used for the 3 alerts. The basic alert will use control
* vector 3 (HierachyName), generic alert will use control vector 5
* (HierachyResource). The recfms will use cnm vector.
* First the alert data is scanned to check which type of alert should be added.
* If there is no name entries, then we add 3 name entries.
* If there is one to 3 name entries, then they are appended to the first 2 
* entries in the table, the last entry in the table is removed.
* if there is 4 name entries, then they are appended to the table, the
* last 2 entries in the table are removed.
* If there are 5 entries, then the first entry is deleted and append to the
* first entry of the table.
*
*/
 

/* buffer usage to hold 5 hierachy name list */
#define MaxAlertBufSize   (43+2*(1+8+4))

/*---------------------------------------------------------------------*
* Basic alert 
*----------------------------------------------------------------------*/

static const byte hierachyName[] = {
   43,            /* Sub vector length */      
   HierachyName,  /* Hierachy Name list  */ 
   0,             /* 2: reserved */
   3,             /* 3: Number of name entries */
   9,             /* 4: length of PU name     */
   NAME,          /* 5: up PU name     */
   0xe2, 0xd7,0x40,0x40,   /* 13: service point */
   9,             /* 17: length of lan name     */
   NAME,          /* 18: lan name     */
   0xd3,0xc1,0xd5,0x40,   /* 26: lan */
   9,             /* 30: */
   NAME,          /* 31: down stream pu name */
   0xc3,0xe3,0xd9,0xd3  /* 39: controller */
   
};

#define BhostName        5
#define BlanName         18
#define BpuName          31
#define BvectorLen       (9+4)
#define BheaderLen       4

/*---------------------------------------------------------------------*
* Generic alert 
*----------------------------------------------------------------------*/

static const byte hierachyResource[] = {
   38, 
   HierachyResource,  /* 1: Hierachy/Resource List MS Commonn SubVector */
   36,                /* 2: length of Hierachy/Resource List sub filed  */
   0x10,              /* 3: Hierachy name list key */
   0,                 /* 4: complete */
   9,                 /* 5: length, 8 name + this */
   NAME,              /* 6: upstream ebcdic name */
   0x20,              /* 14:flag        */
   0xf4,              /* 15: CP */
   9,                 /* 16: length, 8 name + this */
   NAME,              /* 17: LAN name */
   0,                 /* 25: flag      */
   0x39,              /* 26: lan       */
   9,                 /* 27: len */
   NAME,              /* 28: downstream pu name */
   0,                 /* 36: flag   */
   0xf1               /* 37: pu     */
};


#define GhostName        6
#define GlanName         17
#define GpuName          28
#define GvectorLen       (1+8+1+1)
#define GheaderLen       5

#define AddNameListLen   36


/*---------------------------------------------------------------------*
* CNM vector 
*----------------------------------------------------------------------*/
static const byte cnmVector[] = {
   42,            /* 0 Sub vector length */
   0xc,           /* 1: vector type */     
   2,             /* 2: Hierachy Name option value  */ 
   3,             /* 3: Number of name entries */
   8,             /* 4: length of PU name     */
   NAME,          /* 5: up PU name     */
   0xe2, 0xd7,0x40,0x40,   /* 13: service point */
   8,             /* 17: length of PU name     */
   NAME,          /* 18: lan name     */
   0xd3,0xc1,0xd5,0x40,   /* 26: lan */
   8,             /* 30: */
   NAME,          /* 31: down stream pu name */
   0xc3,0xe3,0xd9,0xd3  /* 39: controller */
};

#define ChostName        5
#define ClanName         18
#define CpuName          31
#define CvectorLen       (1+8+4)
#define CheaderLen       4

#define AddCnmVector    sizeof(cnmVector)
#define AppendCnmVector 13


static char *GetDspuDlcName(AdaptorId *idP)
{

  static char dlcName[9];


  switch(idP->dlcType) {

      case DSPU_DLC_SDLC:
      case DSPU_DLC_QLLC:
      case DSPU_DLC_ETHERNET:
      case DSPU_DLC_TOKEN_RING:
      case DSPU_DLC_RSRB:
      case DSPU_DLC_VDLC:
      case DSPU_DLC_NCIA:
      case DSPU_DLC_FRAME_RELAY:
      case DSPU_DLC_FDDI:
      case DSPU_DLC_CHANNEL:
        sprintf(dlcName, "%s\0", idP->dlcIdb->hwptr->hw_short_namestring);
        break;

      case DSPU_DLC_UNDEFINED:
      default:
        sprintf(dlcName, "\0");
        break;

  } /* End switch(dlcType) */

  return(dlcName);

} /* End GetDspuDlcName() */


#ifdef PHIL
static void SetName(byte* ascii, byte* ebcdic)
{
   byte ascii_strlen = strlen(ascii);
   if (ascii_strlen > 8)
     ascii_strlen = 8;

   memset(ebcdic, 0x40, 8);
   ConvertAsciiToEbcdic(ascii, ebcdic,  ascii_strlen);
}
#endif

/*---------------------------------------------------------------------*
* InsertData 
*----------------------------------------------------------------------*
*
* Description:
*    A local function to add data into an existing area.
*
* Replace a subvector at addLocn with the new subvector(dataLocn). 
* (clsiBfrP) at the location(addLocn).
* Because the existing buffer may not be big enough, so we need to
* create a new buffer and return this new buffer. If we cannot get a buffer
* simply return the old buffer
*/ 
static DataReq_t*       /* new buffer */
InsertData(Pu*        this,
           byte*     msVector,             /* major vector location */
           DataReq_t* clsiBfrP,       /* old buffer */
           byte*      addLocn,        /* add location */
           hword      subVecLen,      /* data length to be deleted */
           byte*      subVec,         /* sub vector location */
           byte       items,           /* number of itmes          */
           byte       hostName,        /* hostName offset of dataLocn */
           byte       lanName,         /* lanName offset of dataLocn */
           byte       puName,          /* pu name  offset of dataLocn */
           byte*      dataLocn)        /* data location */
{
   byte* oldP;
   byte* newP;
   byte  addDataLen = *dataLocn;    /* vector length */
   hword offset;
   hword dataLength = GetDataLength(clsiBfrP);
   byte  addLen;
   DataReq_t* bufferP;

   if (msVector == 0)     /* recfms use different vector */
       addDataLen++;

   addLen = addDataLen - subVecLen;
   
    /* get a new frame + data length to be added */
   bufferP = Cnstrct_DataReq (NON_BLOCKING, dataLength + addDataLen - subVecLen); 
   if (bufferP == 0)     /* if cannot get one */
      return bufferP;    /* no alert converstion */
   
   if (items > 0){
      ConvertToEbcdicName(GetPuName(GetFocalPu()), &dataLocn[hostName]);
      if (items > 1){
         AdaptorId* adaptorIdP = SapGetAdaptorId(LsGetSap(PuGetParentLs(this)));
         ConvertToEbcdicName(GetDspuDlcName(adaptorIdP), &dataLocn[lanName]);
         if (items > 2)
             ConvertToEbcdicName(GetPuName(this), &dataLocn[puName]);
      }
   }
   oldP = ReturnDataHandle(clsiBfrP);
   newP = ReturnDataHandle(bufferP);
   
   if (msVector){
       if (subVec != 0)             /* if this is to change vect length */
           *subVec += addLen;       /*  change sub vector length */
       msVector[1] += addLen;      /* add least sig byte */
       if (msVector[1] < addLen)      /* check if overflow */
           msVector[0]++;
   }
   offset = addLocn - oldP ;
   
   memcpy(newP, oldP, offset);   /* copy up to addLocn */

   memcpy(newP + offset, dataLocn, addDataLen);  /* insert new data */

   memcpy(newP + offset + addDataLen, oldP + offset + subVecLen, dataLength - offset - subVecLen);
   
   DspuCLSIMsgDestroy(clsiBfrP);    /* free the old buffer */
   return bufferP;
}




/*---------------------------------------------------------------------*
*  NmvtData
*----------------------------------------------------------------------*
*
* Description:
*    A local function for NMVT.
*
* 1. Scan through major vector to look for ALERT
* 2. Scan through common vector to look for Generic, Basic, Hierachy Name
*    and Hierachy Resource.
*    if (Name or Resource vector found, append the hierachy resource.
*    else add Name on Basic or Resource on Hierachy.
* 3. Call InsertData to add a new name list.
*/ 
   
static DataReq_t*                            /* new NMVT alert */
NmvtData(Pu* this,             /* downstream pu  */
           DataReq_t* clsiBfrP)     /* NMVT alert message */
{
   SNA       * snaP;

   byte*     vector = 0; /* pointer to common vector, the first byte is
                         * length followed by key
                         */
   byte*     subVec = 0;    /* pointer to the subvector of common vector, this
                         * is the subfiled of Hierachy Resource (0x05)
                         */
   short     subLen = 0;    /* length left for iteration of sub field */
                         

   hword msLen;         /* major vector length */
   hword svLen = 0;         /* subvector length == vector[0] */
   int   msLenLeft;     /* major vector length left, for iteration */
   byte* msVector;      /* major vector */
   hword lenLeft;       /* sub vector iteration length */
   byte  nameList;       /* number of name list */
   byte  items = 0;          /* number of name items      */
   
   /* The resource list to be added is defined in hierachyName and 
    * hierachyResource. This data is copied into buffer area and the
    * pu name is copied to the buffer
    */
   byte  buffer[MaxAlertBufSize];
   
   /* At the end of iteration, the followings are the result */
   
   enum AlertType {
     NoAlert,             /* NMVT is not an alert */
     BasicAlertType,      /* Basic Alert but not name list */
     GenericAlertType,    /* Generic Alert but not name list */
     HierachyNameType,    /* Hierachy name vector found */
     HierachyResourceType, /* Hierachy Resource vector found */
     HierachyResourceFieldType, /* Hierachy Resource field  found */
     PassThrough          /* dont do anything, just pass through */
   } alertType;
   
   alertType  = NoAlert;  /* ALERT not found */


#define SnaHeader  9
#define MinMVlen   9
   
   /* Scan through major vector. There can be more than one Major vector
    * There must be at least 9 bytes to hold a major vector.
    */
   
   snaP = (SNA *)ReturnDataHandle(clsiBfrP);
   msVector = &snaP->ruData[8];     /* get major vector */
   
   for (lenLeft = GetDataLength(clsiBfrP) - SnaHeader; 
        lenLeft > MinMVlen; 
        lenLeft -= msLen, msVector += msLen) {
      
      msLen = (msVector[0] << 8) + msVector[1];  /* major vector length */
      
      if (msVector[2] == (ALERT >> 8) && msVector[3] == ALERT) {
         
         boolean breakLoop = FALSE;
         /* ALERT is found, start to iterate  MS common vector */
         
         vector = &msVector[4];      /* point to MS common vector */
         
         for (msLenLeft = msLen - 4; 
              msLenLeft > 0; 
              msLenLeft -= svLen, vector += svLen) {
            
            svLen = vector[0];   /* save svLen because vector[0] can 
                                  * change*/
            switch (vector[1]){
              case BasicAlert:
                 memcpy(buffer,hierachyName, sizeof(hierachyName));
                 alertType = BasicAlertType;
                 break;
                 
              case GenericAlert:
                 memcpy(buffer,hierachyResource,sizeof(hierachyResource));
                 alertType = GenericAlertType;
                 break;
                 
              case HierachyName:
              
                 breakLoop = TRUE;
                 alertType = HierachyNameType;  /* terminate iteration*/
                 nameList = vector[3];          /* number of namelist */
                 memcpy(buffer,hierachyName, sizeof(hierachyName));
                 if (nameList < 4){              /* if less than 4     */
                    memcpy(&buffer[30], &vector[4], vector[0] - 4);
                    buffer[3] = vector[3] + 2;           /* add 2 more */
                    buffer[0] = vector[0] + 2 * BvectorLen;
                    items = 2;
                 }
                 else if (nameList == 4){
                    memcpy(&buffer[17], &vector[4], vector[0] - 4);
                    buffer[3] = vector[3] + 1;           /* add 1 more */
                    buffer[0] = vector[0] + BvectorLen;
                    items = 1;
                 }
                 else if (nameList == 5){
                    byte len = vector[4] + 4;
                    memcpy(&buffer[17], &vector[4 + len], vector[0] - 4 - len);
                    buffer[0] = vector[0] + BvectorLen - len;
                    items = 1;
                 }
                 else
                    alertType = PassThrough;
                 break;
              
              case HierachyResource:
              
                 /* scan through the resource list to find a hierachy name
                  * list (0x10). 
                  */
                  
                 breakLoop = TRUE;
                 subVec =  &vector[2];
                 
                 for (subLen = vector[2]; subLen > 0; 
                      subLen -= subVec[0], subVec += subVec[0]){
                   
                   if (subVec[1] == 0x10){
                   
                      byte  fieldLen;       /* field length left   */
                      byte  addLen = 0;
                      byte* subField = &subVec[3]; /* 3 = len+key+complete */

                      alertType = HierachyResourceFieldType; 
                      nameList = 0;
                      for (fieldLen = subVec[0] - 3;
                           fieldLen > 0;
                           fieldLen -= (subField[0] + 2),  /*2 = flag + id */
                           subField += (subField[0] + 2)){
                         
                         nameList++;
                      }
                      
                      memcpy(buffer,hierachyResource,sizeof(hierachyResource));
                      if (nameList < 4){              /* if less than 4     */
                         memcpy(&buffer[27], &subVec[3], subVec[0] - 3);
                         addLen = 2 * GvectorLen;
                         items = 2;
                      }
                      else if (nameList == 4){
                         memcpy(&buffer[16], &subVec[3], subVec[0] - 3);
                         items = 1;
                         addLen = GvectorLen;
                      }
                      else if (nameList == 5){
                         byte len = subVec[3] + 2;         /* length of data to be deleted */
                         memcpy(&buffer[16], &subVec[3 + len], subVec[0] - 3 - len);
                         addLen = GvectorLen - len;
                         items = 1;
                      }
                      else
                         alertType = PassThrough;

                      if (alertType != PassThrough){
                          buffer[2] = subVec[0] + addLen;
		      }
                      break;
                      
                   }/* 0x10*/
                }/* for */
                
                if (subLen <= 0){
                
                   /* cannot find Hierachy Name List (0x10), add one and  */
                

                   alertType = HierachyResourceType;  /* break the loop */
                }
                break;
                
                /* at the end of this operation, vector point to the
                 * end of the HierachyResource vector. This is the place
                 * that this name list to be inserted
                 */
           
            } /*switch*/
           if (breakLoop)
              break;
        }/*subvector loop */
      }/* ALERT */
      if (alertType != NoAlert)
         break;
   }
   
   switch(alertType){
   
     case BasicAlertType:  /* 0x91, add vector 0x3 */
        /* vector 0x91 without vector 0x3 should be illegal */
       return InsertData(this, msVector,
                clsiBfrP,       /* old buffer */
                vector,        /* end of vector location */
                0,             /* no data to be deleted */
                0,             /* subvector length had been updated */
                 3,             /* number of item added      */
                BhostName, BlanName, BpuName, /* name offsets */
                buffer);       /* data location */
   
     case GenericAlertType:   /* 0x92, add vector 0x5 */
       /* vector 0x92 without 0x5 should be illegal */
       return InsertData(this, msVector,
                clsiBfrP,       /* old buffer */
                vector,        /* add to the end of vector location */
                0,             /* data length to be deleted */
                0,             /* subvector length had been updated */
                3,             /* number of item added      */
                GhostName, GlanName, GpuName, /* name offsets */
                buffer);       /* data location */

     case HierachyResourceFieldType:
        /* vector 5 found and there is subfield 0x10 which is saved
         * at buffer 
         */
      
            return InsertData(this, msVector,
                clsiBfrP,       /* old buffer */
                subVec,        /* sub vector location */
                *subVec,       /* data length to be deleted */
                vector,
                items,             /* number of item added      */
                GhostName - 2, GlanName - 2, GpuName - 2, /* name offsets */
                buffer + 2);       /* data location */
    
     case HierachyNameType:
        /* vector 0x3 found, the new one is now in buffer,
         * delete the old one
         */
        return InsertData(this, msVector,
                clsiBfrP,       /* old buffer */
                vector,        /* subvector location */
                *vector,       /* data length to be deleted */
                vector,        /* vector length */
                items,             /* number of item added      */
                BhostName, BlanName, BpuName, /* name offsets */
                buffer);       /* data location */
     
     case HierachyResourceType:
        /* vector 0x5 found and there is no hierachy name list, add
         * the hierachy nameList at the end of 0x5 vector. This should
         * be illegal. 
         */
       return InsertData(this, msVector,
                clsiBfrP,       /* old buffer */
                vector + svLen,    /* add to the end of vector location */
                0,             /* data length to be deleted */
                vector,        /* subVector location */
                3,             /* number of item added      */
                GhostName - 2, GlanName - 2, GpuName - 2, /* name offsets */
                buffer + 2);       /* data location */
     
     
     default:
       return clsiBfrP;  
   }
         /* data location */
}

static DataReq_t*                            /* new NMVT alert */
RecfmsData (Pu* this,      /* downstream pu  */
              DataReq_t* clsiBfrP)    /* NMVT alert message */
{
   SNA       * snaP;
   
   byte*     vector;
   byte*     subVec;
   byte      buffer[sizeof(cnmVector) + 2*CvectorLen];
   byte*     ptr = buffer + sizeof(cnmVector);
   byte      items = 3;
   byte      entries;
   byte      deleteLen = 0;
   byte      addLen =sizeof(cnmVector); 

   
   memcpy(buffer, cnmVector, sizeof(cnmVector));
   snaP = (SNA *)ReturnDataHandle(clsiBfrP);
   for (vector =  &snaP->ruData[20]; vector[0] != 0; vector += vector[0] + 1){

      if ((vector[1] & 0x3f) == 0xc){
         subVec = &vector[4];
         ptr = buffer + 4;
         if ((entries = vector[3]) < 6) {     /* less than 6 entries */
            memcpy(buffer, cnmVector, sizeof(cnmVector));
            if (entries == 5){
                items = 1;
                ptr = buffer + 17;
                deleteLen = subVec[0] + 4 + 1;
                subVec += deleteLen;
                buffer[0] = CvectorLen - deleteLen;
                buffer[3] = 5;
	    }
            else if (entries == 4){
                items = 1;
                ptr = buffer + 17;
                buffer[3] = 5;
                buffer[0] = vector[0] + CvectorLen;
	     }
            else {
                items = 2;
                ptr = buffer + 30;
                buffer[3] = vector[3] + 2;
                buffer[0] = vector[0] + 2 *CvectorLen;
	     }
         }
         else
            return clsiBfrP;
         
         while (entries--){
            byte len = subVec[0] + 4 + 1; 
            memcpy(ptr, subVec, len);
            deleteLen += len;
            addLen += len;
            subVec += len;
            ptr += len;
         }
      }
      return InsertData(this, 0,
                clsiBfrP,      /* old buffer */
                vector,        /* end of vector location */
                deleteLen,     /* no data to be deleted */
                0,             /* subvector length had been updated */
                items,             /* number of item added      */
                ChostName, ClanName, CpuName, /* name offsets */
                buffer);       /* data location */
   }
   return clsiBfrP;
}
   
   
   

DataReq_t*                            /* new NMVT alert */
AlertPassThru(Pu* this,             /* pu name */
                DataReq_t* clsiBfrP)     /* NMVT alert message */
{
   SNA       * snaP;

   snaP = (SNA *)ReturnDataHandle(clsiBfrP);
   if (snaP->ruData[0] == 0x41 && snaP->ruData[1] == 0x3)
   {
      if (snaP->ruData[2] == NMVT){
         return NmvtData(this, clsiBfrP);
      }
      else if (snaP->ruData[2] == RECFMS && 
              (snaP->ruData[7] & 0x3f) == 0){   /* type 0 */
         return RecfmsData(this, clsiBfrP);
      }
   }
   return clsiBfrP;     /* pass directly */
}

