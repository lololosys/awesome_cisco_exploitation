/* $Id: dlu.h,v 3.3 1995/12/08 17:22:23 sberl Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/cls/dlu.h,v $
 *------------------------------------------------------------------
 * Data Link User Structure Definitions
 *
 * September 1994, Meng Lyu
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: dlu.h,v $
 * Revision 3.3  1995/12/08  17:22:23  sberl
 * CSCdi44998:  Removing FRAS/BAN from subinterface crashes router
 * 1) Added checking for fSapState before calling functions to activate
 * and deactivate the sap. Don't want to deactivate a sap that is not
 * active, nor activate one that is already active. Defined an enum to
 * represent the sap state instead of useing 0 and 1.
 *
 * 2) Fix loop in DLURemoveBanCep so that when list is modified from
 * within the loop, we start again back at the beginning of the list.
 *
 * 3) Validate cep in close_stn.cnf to handle case where DLU did not wait
 * for confirm before deleting the cep control block. If we get a confirm
 * for a cep that doesn't exist any more, just drop the confirm message.
 *
 * 4) Generic cleanup and commenting.
 *
 * Revision 3.2  1995/11/17  08:56:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:12:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:53:32  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/24  23:48:12  vrawat
 * CSCdi42311:  FRAS to SDLC does not recover properly
 *
 * Revision 2.4  1995/10/11  04:20:23  ppearce
 * CSCdi38628:  DSPU RSRB statement needs configurable bridge number
 *   Add fBridgeNumber capability for CLSI/RSRB Enable.Req
 *
 * Revision 2.3  1995/09/15  17:08:59  sberl
 * CSCdi40292:  FRAS backup needs to retry
 * Implement retry logic for fras backup command. We will not try to
 * activate the primary link several times before switching over to the
 * backup.
 *
 * Revision 2.2  1995/07/20  21:13:29  kchiu
 * CSCdi37258:  FRAS Dial-Backup for multiple stations failed
 *
 * Revision 2.1  1995/06/07  20:20:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __DLU_H__
#define __DLU_H__
#define MAX_LOAD_DLCI     5

STRUCTURE_REF(TDLUCvt);
STRUCTURE_REF(TDLUSap);
STRUCTURE_REF(TDLUCep);
STRUCTURE_REF(TDLUPort);
STRUCTURE_REF(TDLUEntity);
STRUCTURE_REF(TDLUBackup);
STRUCTURE_REF(TDLUBanDlci);
STRUCTURE_REF(TDLUBan);


void DLUInit(TDLUEntity *);
void DLUShutdown(TDLUEntity *);


typedef boolean (*actsap_t)(TDLUEntity *, TDLUSap *);

struct TDLUCvt_
{
    TDLUCvt* fNext;
    byte fClassId[4];
    TDLUEntity* fDlu;			
    TDLUCep *fPriCep;
    TDLUCep* fSecCep;
    int  fCvtState;
};

struct TDLUEntity_
{
    TDLUEntity* fNext;
    byte fClassId[4];
    TCLSIQInit* fQinit;
    TCLSIQueue* fWriteQ;
    TCLSIQueue* fReadQ;
    queuetype fPortList;
    queuetype fSapList;
    queuetype fCepList;
    queuetype fCvtList;
    queuetype fBanCmdList;
};

void DLURemoveBanCmd(TDLUEntity*, TDLUBan*);
TDLUBan* DLUBanCmdCreate(TDLUBan*);
void DLUBanCmdDestroy(TDLUBan*);
void DLUAddBanCmd(TDLUEntity*, TDLUBan*);

/* Converter list management methods */

TDLUCvt*  DLURemoveCvt(TDLUEntity*, TDLUCvt*);
void DLUAddCvt(TDLUEntity*, TDLUCvt*);

/* CEP list management methods */

void DLUAddCep(TDLUEntity*, TDLUCep*);
TDLUCep*  DLURemoveCep(TDLUEntity*, TDLUCep*);

/* SAP list management methods */

void DLUAddSap(TDLUEntity*, TDLUSap*);
TDLUSap*  DLURemoveSap(TDLUEntity*, TDLUSap*);
TDLUSap* DLUFindSap(TDLUEntity*, word);

/* PORT list management methods */

void DLUAddPort(TDLUEntity* , TDLUPort* );
TDLUPort*  DLURemovePort(TDLUEntity* , TDLUPort* );
TDLUPort* DLUFindPort(TDLUEntity*, CLSPortID*);


typedef enum {
    DLU_PORT_STATE_RESET,
    DLU_PORT_STATE_ACTIVE
} DLUPortState_e;


struct TDLUPort_
{
    TDLUPort* fNext;
    byte fClassId[4];
    TDLUEntity* fDlu;			
    CLSPortID fPortId;
    CLSDLCType_e fDLCType;
    hword fSrcRn;
    hword fBridge;
    hword fTargetRn;
    DLUPortState_e fPortState;
    int fRefCount;
};

TDLUPort* DLUPortCreate(void);
void DLUPortDestroy(TDLUPort* );

void DLUSendPortEnable(TDLUEntity* , TDLUPort*, uchar*, hword, hword, hword);

typedef enum {
    DLU_SAP_STATE_RESET,
    DLU_SAP_STATE_ACTIVE
} DLUSapState_e;

struct TDLUSap_
{
    TDLUSap* fNext;
    byte fClassId[4];
    TDLUEntity* fDlu;			
    TDLUPort* fPortPtr;
    word fUSapId;
    word fPSapId;
    word fSapNumber;
    CLSPortID fPortId;
    CLSDLCType_e fDLCType;
    DLUSapState_e fSapState;
    int fRefCount;
    actsap_t fActivate;
    actsap_t fDeactivate;
};


TDLUSap* DLUSapCreate(TDLUPort*);
void DLUSapDestroy(TDLUSap* );

struct TDLUCep_
{
    TDLUCep* fNext;
    byte fClassId[4];
    TDLUEntity* fDlu;			/* pointer to DLU that owns */
					/* this Cep */
    TDLUSap* fSapPtr;			/* pointer to Sap that owns */
					/* this Cep */
    TDLUCvt* fCvtPtr;
    TDLUCep* fCepPtr;
    TCLSIMsg* fMsg;
    hwaddrtype fDstAddr;
    word fPCepId;
    word fUCepId;
    int fCepState;
    int fXidState;
    uchar fRole;
    uchar fRSap;
    uchar fLSap;
    byte fRifLength;
    byte fRif[SRB_MAXRIF];
    byte fXidLength;
    byte *fXidPtr;
    byte fExplorerOption;
    byte fL3Pid;			/* Layer 3 Protocol ID		*/
    uchar fFlags;
    byte fSendWindow;			/* zero, unless learned from XID3 */
    TDLUCep* fCepOrigPtr;
    ushort fDlci;                       /* added for FRAS BAN - vipin sep12 */
    hwaddrtype fSrcAddr ;               /* added for FRAS BAN - vipin sep14 */
    uchar fFrasType;
    uchar fBniAddr[IEEEBYTES];
#define  FRAS_BNN	0x01
#define  FRAS_BAN	0x02
    uchar fReqOpnStnMaxTry;
#define  REQ_OPEN_STN_MAX_TRY	2
#define  FDLCSW_SHOW	0x01
#define  FDLCSW_NVGEN	0x02
#define  FDLCSW_DELETE	0x04
    uchar fConnectionInit;
#define  INBOUND_CONNECTION  0x01
#define  OUTBOUND_CONNECTION 0x02
};

struct TDLUBackup_
{
    idbtype *fIdb;
    uchar   fVirAddr[IEEEBYTES];
    hword   fLocalRing;
    hword   fBridgeNumber;
    hword   fTargetRing;
    uchar   fHostAddr[IEEEBYTES];
};

struct TDLUBanDlci_
{
    ushort   number;
    ushort   session;
    uchar   status;
};

struct TDLUBan_
{
    TDLUBan *fNext;
    idbtype *fIdb;
    idbtype *fLan_idb;
    uchar   fDlciMacAddr[IEEEBYTES];
    uchar   fBniMacAddr[IEEEBYTES];
    hword   fLocalRing;
    hword   fBridgeNumber;
    hword   fTargetRing;
    uchar   fTotalDlci;
    uchar   fAvailDlci;
    TDLUBanDlci   fDlci[MAX_LOAD_DLCI];
    uchar   fType;
};


#define D_XID_IDLE	0
#define D_XID_SEND	1
#define D_XID_XCHG	2
#define D_XID_CMD	3
#define D_XID_RSP	4

#define DLCSW_XID_NONE			0
#define DLCSW_XID_PROXY			1
#define DLCSW_XID_FORWARD		2
#define DLCSW_XID_CONSUME		3

#define DLU_DLCI_ACTIVE		1
#define DLU_DLCI_INACTIVE	2
#define NO_DLCI			0xff

TDLUCep* DLUCepCreate(TDLUSap*);
void DLUCepDestroy(TDLUCep*);

TDLUCvt* DLUCvtCreate(TDLUEntity *);
void DLUCvtDestroy(TDLUCvt*);

extern void DLUSendPortDisable(TDLUEntity*, TDLUPort*);
extern boolean DLUSendActSap(TDLUEntity *, TDLUSap *);
extern boolean DLUSendDeactRing(TDLUEntity *, TDLUSap *);
extern boolean DLUSendActRing(TDLUEntity *, TDLUSap *);
extern boolean DLUSendDeactSap(TDLUEntity *, TDLUSap *);
extern void DLUSendReqOpnStn(TDLUCep*);
extern void DLUSendIDReq(TDLUCep*, byte*, size_t);
extern void DLUSendIDRsp(TDLUCep*, byte*, size_t);
extern void DLUSendConnectReq(TDLUCep*);
extern void DLUSendConnectRsp(TDLUCep*);
extern void DLUSendSignalStn(TDLUCep*);
extern void DLUSendDiscReq(TDLUCep*);
extern void DLUSendDiscRsp(TDLUCep*);
extern void DLUSendCloseStnReq(TDLUCep*);
extern void DLUPortActSap(TCLSIMsg*);
extern void DLUSendFlowRq(TDLUCep *, int);
extern boolean DLURouteDestroy(TDLUCep *, TDLUCep *);
extern void DLUReplaceBackupCep(TDLUCep *);
extern void DLUBanEnable(TDLUBan *, CLSDLCType_e);
extern void DLUBanCepEnable(TDLUCep *);
extern ushort DLUGetBanDlci(TDLUCep *);
extern void DLUUpdateDlciStatus(TDLUCep *, uchar);
extern void DLUUpdateAvailDlci(TDLUBan *);
extern void DLUBanRouteDestroy(TDLUCep *, TDLUCep *);
extern void SetBanDlciMacAddr(hwidbtype *, uchar *);
#endif
