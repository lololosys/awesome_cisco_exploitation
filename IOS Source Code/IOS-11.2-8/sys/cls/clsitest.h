/* $Id: clsitest.h,v 3.2 1995/11/17 08:55:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/cls/clsitest.h,v $
 *------------------------------------------------------------------
 * CLSI test header file
 *
 * 12-September-1994	Steve Chiang
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This file decllares all prototypes used by clsitest.c and defines
 * all buginf() macros,
 *
 *------------------------------------------------------------------
 * $Log: clsitest.h,v $
 * Revision 3.2  1995/11/17  08:55:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:11:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/11  04:20:13  ppearce
 * CSCdi38628:  DSPU RSRB statement needs configurable bridge number
 *   Add fBridgeNumber capability for CLSI/RSRB Enable.Req
 *
 * Revision 2.2  1995/07/02  05:15:29  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.1  1995/06/07  20:20:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __CLSITEST_H__
#define __CLSITEST_H__

/* 
 * CLSI PRIMITIVES
 */
#define CLSISTART 1
#define CLSISTOP  2
#define CLSIDEBUG 3

extern void clsitest_parser_init(void);
extern void clsitest_command (parseinfo *);

#define ERR	-1

void retrieve_clsi_msg(TCLSIQueue *aQueue, TCLSIMsg *cls_msg);
int clsitest_start(void);
void clsitest_stop(void);

int enable_req(parseinfo *csb);
int activate_sap_req(parseinfo *csb);
int clsi_flow_req(parseinfo *csb);
int open_stn_req(parseinfo *csb);
int req_opnstn_req(parseinfo *csb);
int close_stn_req(parseinfo *csb);
int connect_req(parseinfo *csb);
int disconnect_req(parseinfo *csb);
int id_req(parseinfo *csb);
int signal_stn_req(parseinfo *csb);
int data_req(parseinfo *csb);

void show_open_stn_cnf(TCLSIMsg *cls_msg);
void show_close_stn_cnf(TCLSIMsg *cls_msg);
void show_connect_cnf(TCLSIMsg *cls_msg);
void show_disconnect_cnf(TCLSIMsg *cls_msg);
void show_signal_stn_cnf(TCLSIMsg *cls_msg);
void show_data_cnf(TCLSIMsg *cls_msg);
void show_req_opnstn_cnf(TCLSIMsg *cls_msg);
void show_id_cnf(TCLSIMsg *cls_msg);
void show_enable_cnf(TCLSIMsg *cls_msg);
void show_disable_cnf(TCLSIMsg *cls_msg);
void show_deactivate_sap_cnf(TCLSIMsg *cls_msg);
void show_activate_sap_cnf(TCLSIMsg *cls_msg);

void show_connect_ind(TCLSIMsg *cls_msg);
void show_connected_ind(TCLSIMsg *cls_msg);
void show_disconnect_ind(TCLSIMsg *cls_msg);
void show_id_ind(TCLSIMsg *cls_msg);
void show_id_stn_ind(TCLSIMsg *cls_msg);
void show_data_ind(TCLSIMsg *cls_msg);
void show_flow_ind(TCLSIMsg *cls_msg);

int connect_rsp(parseinfo *csb);
int disconnect_rsp(parseinfo *csb);
int id_rsp(parseinfo *csb);

/*
 * Code Review comment for CSCdi30022 pointed out that these
 * should really be functions and not macros. This is asking for
 * trouble. sberl 2/21/95
 */
#define NewLine	buginf("\n")

#define PrintClsiHdr(prim) \
	buginf("\n\tHeaderLength:    %d", prim->fCLSIHdr.fHeaderLength); \
	buginf("\n\tDataSize:        %d", prim->fCLSIHdr.fDataSize); \
	buginf("\n\tSelector:        0x%x", prim->fCLSIHdr.fSelector); \
	buginf("\n\tIdentifierType:  %d", prim->fCLSIHdr.fIdentifierType); \
	buginf("\n\tIdentifier:      %d", prim->fCLSIHdr.fIdentifier); 

#define PrintPortNumber(prim)	\
	buginf("\n\tPortNumber:      0x%x", (u_short) ((prim)->fPortId).fTypeInfo.port_num.unit)

#define PrintPortType(prim)	\
	buginf("\n\tPortType:        0x%x", (prim)->fDLCType)

#define PrintCorrelator(prim)	\
	buginf("\n\tCorrelator:      0x%x", (u_long) ((prim)->fCorrelator))

#define PrintPCepId(prim)	\
	buginf("\n\tPCepId:          0x%x", (u_long) (prim->fCLSIHdr.fPCepId))

#define PrintActionCode(prim) \
	buginf("\n\tActionCode:      0x%x", prim->fCLSIHdr.fActionCode)

#define PrintVersion(prim)	\
	buginf("\n\tVersion:         0x%x", prim->fVersion)

#define PrintModifyFlags(prim)	\
	buginf("\n\tModifyFlags:     0x%02x%02x%02x", \
		(u_char) (prim->fCLSIHdr.fModifyFlags[0]), \
		(u_char) (prim->fCLSIHdr.fModifyFlags[1]),\
		(u_char) (prim->fCLSIHdr.fModifyFlags[2]))

#define PrintUSapId(prim) \
	buginf("\n\tUSapId:          0x%x", prim->fCLSIHdr.fUSapId)

#define PrintUCepId(prim) \
	buginf("\n\tUCepId:          0x%x", prim->fCLSIHdr.fUCepId)

#define PrintFlowReq(prim) \
	buginf("\n\tFlowReq:      0x%x", prim->fCLSIHdr.fFlowAction);

#define PrintFlowInd(prim) \
        buginf("\n\tFlowInd:      0x%x", prim->fCLSIHdr.fFlowLevel);

#define PrintReserveLength(prim)	\
	buginf("\n\tReserveLength:   0x%x", prim->fReserveLength)

#define PrintLocalMac(parm)	\
	buginf("\n\tLocalMac:        0x"); \
	for (i=0; i<IEEEBYTES; i++) \
		buginf("%02x", (u_char) (parm->fLocalMac[i]))

#define PrintTrSpeed(parm) \
	buginf("\n\tTrSpeed:         %d", parm->fTrSpeed); 

#define PrintMaxIFieldLength(parm)	\
	buginf("\n\tMaxIFieldLength: 0x%x (%d)\n", \
	parm->fMaxIFieldLength, parm->fMaxIFieldLength)

#define PrintMacaddr(prim)	\
	buginf("\n\tVMacaddr:        0x%x, prim->fVMacaddr")

#define PrintRsrbOptionFlags(prim)	\
        buginf("\n\tRsrbOptionFlags: 0x%x", prim->fRsrbOptionFlags)

#define PrintLocalRing(prim)	\
	buginf("\n\tLocalRing:       0x%x", prim->fLocalRing)

#define PrintBridgeNumber(prim) \
	buginf("\n\tBridgeNumber:    0x%x", prim->fBridgeNumber)

#define PrintTargetRing(prim)	\
	buginf("\n\tTargetRing:      0x%x\n", prim->fTargetRing);

#define PrintType1Length(prim)	\
	buginf("\n\tType1Length:     0x%x", prim->fType1Length);

#define PrintType2Length(prim)	\
	buginf("\n\tType2Length:     0x%x\n", prim->fType2Length);

#define	PrintPSapId(prim)	\
	buginf("\n\tPSapId:          0x%x", prim->fCLSIHdr.fPSapId)

#define PrintStatus(prim)	\
	buginf("\n\tStatus:          0x%04x", prim->fCLSIHdr.fStatus.fRetCode)

#define PrintStatusCode(prim)	\
	switch(prim->fCLSIHdr.fStatus.fRetCode) {\
	case CLS_OK: buginf(" (CLS_OK)"); break; \
	case CLS_OUT_OF_MEMORY: buginf(" (CLS_OUT_OF_MEMORY)"); break; \
	case CLS_UNKNOWN_PORT: buginf(" (CLS_UNKNOWN_PORT)"); break; \
	case CLS_UNKNOWN_SAP: buginf(" (CLS_UNKNOWN_SAP)"); break; \
	case CLS_UNKNOWN_CEP: buginf(" (CLS_UNKNOWN_CEP)"); break; \
	case CLS_UNKNOWN_IDENTIFIER_TYPE: buginf(" (CLS_UNKNOWN_IDENTIFIER_TYPE)"); break; \
	case CLS_SAP_BUSY: buginf(" (CLS_SAP_BUSY)"); break; \
	case CLS_UNKNOWN_PRIMITIVE: buginf(" (CLS_UNKNOWN_PRIMITIVE)"); break; \
	case CLS_UNKNOWN_DLC_TYPE: buginf(" (CLS_UNKNOWN_DLC_TYPE)"); break; \
	case CLS_NO_REM_STN: buginf(" (CLS_NO_REM_STN)"); break; \
	case CLS_NO_XID_RSP: buginf(" (CLS_NO_XID_RSP)"); break; \
	case CLS_DISC_ERROR: buginf(" (CLS_DISC_ERROR)"); break; \
	case CLS_INACTIVITY_TIMEOUT: buginf(" (CLS_INACTIVITY_TIMEOUT)"); break; \
	default: buginf(" (Unknown Status Code:0x%x", prim->fCLSIHdr.fStatus.fRetCode); break; }

#define PrintXID1(prim)	\
	buginf("\n\tXID:             0x"); \
	data = ((char *) &(prim->fXID)) + prim->fReserveLength; \
	if ((prim->fCLSIHdr.fDataSize - 6 - prim->fReserveLength) > 0) \
	for (i=0; i<(prim->fCLSIHdr.fDataSize - 6 - prim->fReserveLength); i++) \
		buginf("%02x", (u_char) data[i]); \
	else buginf("0");

#define PrintXID(prim)	\
	buginf("\n\tXID:             0x"); \
	data = ((char *) &(prim->fXID)) + prim->fReserveLength; \
	if (id_len > 0) \
	for (i=0; i<id_len; i++) \
		buginf("%02x", (u_char) data[i]); \
	else buginf("0");

#define PrintDataLen(prim) \
	buginf("\n\tData Len:        %d", prim->fCLSIHdr.fDataSize - 12 - prim->fReserveLength);

/*
	buginf("\n\tData Len:        %d", prim->fCLSIHdr.fDataSize - prim->fReserveLength - ((ulong) &prim->fData[0] - (ulong) &prim->fCLSIHdr + 4));
*/

#define PrintData(prim)	\
	buginf("\n\tData:            0x"); \
	data = ((char *) &(prim->fData)) + prim->fReserveLength; \
	if ((prim->fCLSIHdr.fDataSize - 12 - prim->fReserveLength) > 0) \
	for (i=0; i<(prim->fCLSIHdr.fDataSize - 12 - prim->fReserveLength); i++) { \
		buginf("%02x", (u_char)data[i]); \
		if (i && !(i%30)) buginf("\n\t"); \
	} else buginf("0");

#define PrintIdStn(prim)	\
	buginf("\n\tVersion:         0x%x", prim->fVersion); \
	buginf("\n\tCorrelator:      0x%x", prim->fCorrelator); \
	buginf("\n\tXIDCmdRsp:       0x%x", prim->fXIDCmdRsp); \
	buginf("\n\tXIDPollFinal:    0x%x", prim->fXIDPollFinal); \
	buginf("\n\tMacType:         0x%x", prim->fMacType); \
	buginf("\n\tLocalMacLength:  0x%x", prim->fLocalMacLength); \
	buginf("\n\tLocalMacOffset:  0x%x", prim->fLocalMacOffset); \
	buginf("\n\tLocalMac:        0x"); \
	data = ((u_char *) cls_msg) + prim->fLocalMacOffset; \
	if (prim->fLocalMacLength > 0) \
	for (i=0; i<prim->fLocalMacLength; i++) \
		buginf("%02x", (u_char) data[i]); \
	else	buginf("0"); \
	buginf("\n\tRemoteMaclength: 0x%x", prim->fRemoteMacLength); \
	buginf("\n\tRemoteMacOffset: 0x%x", prim->fRemoteMacOffset); \
	buginf("\n\tRemoteMac:       0x"); \
	data = ((u_char *) cls_msg) + prim->fRemoteMacOffset; \
	if (prim->fRemoteMacLength) \
	for (i=0; i<prim->fRemoteMacLength; i++) \
		buginf("%02x", (u_char) data[i]); \
	else	buginf("0"); \
	buginf("\n\tRoutinglength:   0x%x", prim->fRoutingLength); \
	buginf("\n\tRoutingOffset:   0x%x", prim->fRoutingOffset); \
	buginf("\n\tRif:             0x"); \
	data = ((char *) cls_msg) + prim->fRoutingOffset; \
	if (prim->fRoutingLength) \
	for (i=0; i<prim->fRoutingLength; i++) \
		buginf("%02x", (u_char) data[i]); \
	else buginf("0"); \
	buginf("\n\tLSap:            0x%x", prim->fLSap); \
	buginf("\n\tRSap:            0x%x", prim->fRSap); \
	buginf("\n\tXIDOffset:       0x%x", prim->fXIDOffset); \
	buginf("\n\tXIDLength:       0x%x", prim->fXIDLength); \
	buginf("\n\tXID:             0x", prim->fXIDLength); \
	data = ((char *) cls_msg) + prim->fXIDOffset; \
	if (prim->fXIDLength)	\
	for (i=0; i<prim->fXIDLength; i++) \
		buginf("%02x", (u_char) data[i]); \
	else buginf("0");

#define PrintLinkStatus(prim)	\
	buginf("\n\tLinkStatus:      0x%x", prim->fCLSIHdr.fLinkStatus)

#define PrintSapLLcParms(prim, parm)	\
	buginf("\n\tUSapId:          0x%x", prim->fCLSIHdr.fUSapId); \
	buginf("\n\tVersion:         0x%x", prim->fVersion); \
	buginf("\n\tPortNumber:      0x%x", prim->fPortId); \
	buginf("\n\tPortType:        0x%x", prim->fDLCType); \
	buginf("\n\tCorrelator:      0x%x", prim->fCorrelator); \
	buginf("\n\tSapValue:        0x%x", parm->fSapValue); \
	buginf("\n\tUserClass:       0x%x", parm->fUserClass); \
	buginf("\n\tSapFlags:        0x%x", parm->fSapFlags); \
	buginf("\n\tT1:              0x%x", parm->fT1); \
	buginf("\n\tT2:              0x%x", parm->fT2); \
	buginf("\n\tTi:              0x%x", parm->fTi); \
	buginf("\n\tMaxOut:          0x%x", parm->fMaxOut); \
	buginf("\n\tMaxRetryCount:   0x%x", parm->fMaxRetryCount); \
	buginf("\n\tMaxIn:           0x%x", parm->fMaxIn); \
	buginf("\n\tDynamicWindow:   0x%x", parm->fDynamicWindow); \
	buginf("\n\tMaxOutIncr:      0x%x", parm->fMaxOutIncr); \
	buginf("\n\tMaxIField:       0x%x", parm->fMaxIField); \
	buginf("\n\tXIDRetries:      0x%x", parm->fXIDRetries); \
	buginf("\n\tXIDTimer:        0x%x\n", parm->fXIDTimer);


#define PrintReqOpnStnLLCParms(parm)	\
	buginf("\n\tDestAddr:"); \
	buginf("\n\t    Hardware type:   0x%x", parm->fDestAddr.type); \
	buginf("\n\t    Length:          0x%x", parm->fDestAddr.length); \
	buginf("\n\t    Target type:     0x%x", parm->fDestAddr.target_type); \
	buginf("\n\t    Addr:            0x"); \
	if (parm->fDestAddr.length) \
		for (i=0; i<parm->fDestAddr.length; i++) \
			buginf("%02x", (u_char) (parm->fDestAddr.addr[i])); \
	else \
		buginf("0"); \
	buginf("\n\tSrcAddr:"); \
	buginf("\n\t    Hardware type:   0x%x", parm->fSrcAddr.type); \
	buginf("\n\t    Length:          0x%x", parm->fSrcAddr.length); \
	buginf("\n\t    Target type:     0x%x", parm->fSrcAddr.target_type); \
	buginf("\n\t    Addr:            0x"); \
	if (parm->fSrcAddr.length) \
		for (i=0; i<parm->fSrcAddr.length; i++) \
			buginf("%02x", (u_char) (parm->fSrcAddr.addr[i])); \
	else \
		buginf("0"); \
	buginf("\n\tDestSap:         0x%x", parm->fDestSap); \
	buginf("\n\tSrcSap:          0x%x", parm->fSrcSap); \
	buginf("\n\tT1:              0x%x", parm->fT1); \
	buginf("\n\tT2:              0x%x", parm->fT2); \
	buginf("\n\tTi:              0x%x", parm->fTi); \
	buginf("\n\tMaxOut:          0x%x", parm->fMaxOut); \
	buginf("\n\tMaxRetryCount:   0x%x", parm->fMaxRetryCount); \
	buginf("\n\tMaxIn:           0x%x", parm->fMaxIn); \
	buginf("\n\tDynamicWindow:   0x%x", parm->fDynamicWindow); \
	buginf("\n\tMaxOutIncr:      0x%x", parm->fMaxOutIncr); \
	buginf("\n\tMaxIField:       0x%x", parm->fMaxIField); \
	buginf("\n\tXIDRetries:      0x%x", parm->fXIDRetries); \
	buginf("\n\tXIDTimer:        0x%x", parm->fXIDTimer); \
	buginf("\n\tExplorerOption:  0x%x", parm->fExplorerOption); \
	buginf("\n\tExplorerRetries: 0x%x", parm->fExplorerRetries); \
	buginf("\n\tExplorerTimer:   0x%x", parm->fExplorerTimer); \
	buginf("\n\tRifLength:       0x%x", parm->fRifLength); \
	buginf("\n\tRif:             0x"); \
	if (parm->fRifLength) \
		for (i=0; i<parm->fRifLength; i++) \
			buginf("%02x", (u_char) (parm->fRif[i])); \
	else \
		buginf("0"); \
	buginf("\n");

#endif /* __CLSITEST_H__ */
