/* $Id: clsdriver.h,v 3.2.58.1 1996/03/18 20:11:12 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/ibm/clsdriver.h,v $
 *------------------------------------------------------------------
 * Definitions for CLS driver.
 *
 * April 1995, Vipin Rawat
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clsdriver.h,v $
 * Revision 3.2.58.1  1996/03/18  20:11:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  09:41:26  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  14:20:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:22:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/25  01:34:42  vrawat
 * CSCdi37602:  fix prioritization using new cls primitive
 *
 * Revision 2.2  1995/07/02  05:15:57  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.1  1995/06/07  20:46:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CLSDRIVER_H__
#define __CLSDRIVER_H__

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "address.h"
#include "../cls/clsi_msg.h"
#include "../cls/clsi_queue.h"

#define CLSSEND_CLOSED         -1      /* failure -- connection gone */
#define CLSSEND_QUEUED         0       /* successful -- queued for output */
#define CLSSEND_QFULL          1       /* failure -- output queue full */
#define CLSSEND_SENT           2       /* successful -- (future) */

#define CLSSEND_OK(x)      ((x) == CLSSEND_QUEUED || (x) == CLSSEND_SENT)
#define CLSSEND_ERROR(x)   !(CLSSEND_OK(x))

/* Default values, can be configured */ 
#define  LISTNING_SAP_0    0xA0
#define  LISTNING_SAP_1    0xA2
#define  LISTNING_SAP_2    0xA4
#define  LISTNING_SAP_3    0xA6

#define  NUM_LISTNING_SAPS 1

STRUCTURE_REF(GnDLUCep);
STRUCTURE_REF(GnDLUPort);
STRUCTURE_REF(GnDLUEntity);
STRUCTURE_REF(GnSapStruct);

struct GnDLUEntity_
{
    GnDLUEntity* pNext;
    queuetype qPortList; 
    void (* connectRet) (void* pArgPointer, boolean IsError) ;
    void (* rddata) (void* thisCep, paktype *pak); 
    void (* fini) (void* cookie);    
    void (* err) (void* pArgPointer);
    boolean (* senderErr) (void *pArgPointer);
    int   iType ; /* 1:STUN , may be other values if other DLUs exists*/
};


struct GnSapStruct_
{
    GnSapStruct*  pNext;  /* without this can't handle free pool(queuetype)*/
    boolean       IsIncoming; 
    byte          SapValue;
    word          PSapId;
    GnDLUPort*    pDluPort;
    GnDLUCep*     pCep;   /* most probably this will only be used for connecting side */
    hword         fType2Length; /* set from actsap.cnf */ 
};


struct GnDLUPort_ 
{
    GnDLUPort*   pNext;
    GnDLUEntity* pDlu; 
    CLSPortID    PortId;
    int          NumListningSaps;
    GnSapStruct  SapStruct[4];
    queuetype    qCepList; 
};


struct GnDLUCep_
{
    GnDLUCep*     pNext;
    GnSapStruct*  pSapStruct; 
    GnDLUEntity*  pDlu;      /* pointer to DLU that owns */
    byte          RPeerSap;      /* remote sap, listning sap for connecting side*/
    hwaddrtype    LocalHWAddress; 
    word          PCepId;                   
    void*         pArgPointer;         /* stp pointer etc..*/
    boolean       IsFirstPacket;       /* valid only on listning side */ 
    ulong         Priority;
};

/* Global Parameters */
boolean IsClsdriverRunning;
byte    UserConfiguredLisnSap;
byte   aListningSaps[4];
queuetype  qDLUList; 

boolean GnInit(void);
int GnDluReadPut(TCLSIQueue* dluReadQ, TCLSIMsg* theMsg);
GnDLUEntity* GnDLUCreate(int thisType);
GnDLUPort* GnDLUPortCreate(void);
void GnDLUAddPort(GnDLUEntity* this, GnDLUPort* thePort);
GnDLUPort*  GnDLURemovePort(GnDLUEntity* this, GnDLUPort* thePort);
GnDLUPort* GnDLUFindPort(GnDLUEntity* this, CLSPortID* portID);
void GnDLUPortDestroy(GnDLUPort* this);
GnSapStruct* GnDLUSapCreate(void);
GnDLUCep* GnDLUCepCreate(void);
void GnDLUAddCep(GnDLUEntity* this, GnDLUCep* thisCep);
GnDLUCep*  GnDLURemoveCep(GnDLUPort* thisPort, GnDLUCep* thisCep);
void GnDLUCepDestroy(GnDLUCep* thisCep);
void GnShutdown(void);
void GnDLUShutdown (GnDLUEntity *thisDlu);
GnDLUEntity* GnFindDlu(int thisType) ;
GnDLUPort* GnEnablePort(int type, idbtype* thisIdb);
void GnDLUSendEnable(GnDLUPort* thisPort) ;
void GnDLUSendDisable(GnDLUPort* thisPort);
void GnDLUSendActListningSaps(TCLSIMsg* theMsg);
void GnDLUSendActSap(GnDLUPort* thisPort, GnSapStruct* thisUSapId);
void GnDLUSendReqOpnStn(GnDLUCep* theCep);
void GnDLUSendConnectReq(GnDLUCep* theCep);
void GnDLUSendModifyStnReq(GnDLUCep* theCep);
void GnDLUSendConnectRsp(GnDLUCep* theCep);
void GnDLUSendDataReq(GnDLUCep* theCep, paktype *pak);
void GnDLUSendSignalStn(GnDLUCep* theCep);
void GnDLUSendDiscReq(GnDLUCep* theCep);
void GnDLUSendDiscRsp(GnDLUCep* theCep);
void GnDLUSendCloseStnReq(GnDLUCep* theCep);
void GnDLUSendDeactSap(GnSapStruct *theUSapId);
void GnDisablePort( GnDLUPort* thisPort);
/* tcp like */
void clsdriver_start(void);
void clsdriver_open(GnDLUPort* thisPort,byte thisSapValue, GnSapStruct* thisSap);
int  clsdriver_send(GnDLUCep *thisCep, paktype *pak, boolean force);
void clsdriver_close(GnDLUCep *thisCep);
void clsdriver_abort(GnDLUCep *thisCep);
/* end tcp like */ 
boolean GnDluBlock(void);
forktype GnDluMain (void);

byte* CLSIMsgGetDataPtr(TCLSIMsg* theMsg);
size_t CLSIMsgGetDataSize(TCLSIMsg* theMsg);
void hwaddr_to_dlci (hwaddrtype *address, ushort* dlci);
void dlci_to_hwaddr(ushort* dlci, hwaddrtype* address);
void GnDLUSendIDReq(GnDLUCep* theCep, byte* xidData, size_t xidLength);
void GnDLUSendIDRsp(GnDLUCep* theCep, byte* xidData, size_t xidLength);
void GnDLUSendConnectRsp(GnDLUCep* theCep);
GnSapStruct* GnDLUFindSap(GnDLUPort *thisPort, byte thisSapValue) ;
#endif
















