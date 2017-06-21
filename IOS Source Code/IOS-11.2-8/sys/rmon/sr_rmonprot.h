/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_rmonprot.h,v 3.3 1996/02/15 22:35:17 jjohnson Exp $
 * $Source: /release/111/cvs/Xsys/rmon/sr_rmonprot.h,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_rmonprot.h,v $
 * Revision 3.3  1996/02/15  22:35:17  jjohnson
 * CSCdi49107: rmon incorrectly issues an initial event for deltaValue
 * alarms.  another patch that i supplied to the vendor.  for deltaValue
 * alarms, don't take the first sample until half the interval has
 * expired, and don't allow an initial alarm until two samples have been
 * taken (i.e. one interval has elapsed)
 *
 * Revision 3.2  1995/11/17  18:39:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1994 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 52.227-7013
 *  and in similar clauses in the FAR and NASA FAR Supplement.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

/* k_alarm.c  */
void SendRisingOrFallingAlarm SR_PROTOTYPE((eventEntry_t *evp,unsigned long type,alarmEntry_t *alarmp));
void CheckAlarmValue SR_PROTOTYPE((alarmEntry_t *alarmp,long risingDiff,long fallingDiff));
void AlarmCallback SR_PROTOTYPE((TimeOutDescriptor *tdp));
void CheckAlarmTable SR_PROTOTYPE((void));
void *SetAlarmTimer SR_PROTOTYPE((long initial,long interval,alarmEntry_t *alarmp));
int k_alarmEntry_initialize SR_PROTOTYPE((void));
int k_alarmEntry_terminate SR_PROTOTYPE((void));
void alarmTableDeleteCallback SR_PROTOTYPE((TimeOutDescriptor *tdp));
void DeletealarmTableEntry SR_PROTOTYPE((int index));
void k_alarmEntryFreeUserpartData SR_PROTOTYPE((alarmEntry_t *data));
void alarmEntry_free SR_PROTOTYPE((alarmEntry_t *data));

/* k_capt.c  */
void NotifyBuffers SR_PROTOTYPE((long channelIndex));
void NotifyBuffersOfError SR_PROTOTYPE((void));
captureBufferEntry_t *AllocateCaptureBuffer SR_PROTOTYPE((bufferControlEntry_t *bcp));
void FreeCaptureBuffer SR_PROTOTYPE((captureBufferEntry_t *bp,bufferControlEntry_t *bcp));
void GetCaptureBuffers SR_PROTOTYPE((bufferControlEntry_t *bcp));
captureBufferEntry_t *GetNextCaptureBuffer SR_PROTOTYPE((bufferControlEntry_t *bcp));
void AddCaptureBuffer SR_PROTOTYPE((captureBufferEntry_t *bp,bufferControlEntry_t *bcp));
void ProcessPacketCapture SR_PROTOTYPE((PacketState *psp));
int k_bufferControlEntry_initialize SR_PROTOTYPE((void));
int k_bufferControlEntry_terminate SR_PROTOTYPE((void));
void bufferControlTableDeleteCallback SR_PROTOTYPE((TimeOutDescriptor *tdp));
void DeletebufferControlTableEntry SR_PROTOTYPE((int index));
void k_bufferControlEntryFreeUserpartData SR_PROTOTYPE((bufferControlEntry_t *data));
void bufferControlEntry_free SR_PROTOTYPE((bufferControlEntry_t *data));

/* k_event.c  */
logEntry_t *CreateLogEntry SR_PROTOTYPE((long index,OctetString *descr));
void FreeLogEntry SR_PROTOTYPE((logEntry_t *logp));
void DeleteLogEntry SR_PROTOTYPE((eventEntry_t *evp));
void DeleteLogTable SR_PROTOTYPE((eventEntry_t *evp));
void AddLogEntry SR_PROTOTYPE((eventEntry_t *evp,OctetString *descr));
void FireEvent SR_PROTOTYPE((long eventIndex,
			     void (*alarmFunction)(eventEntry_t *evp,unsigned long type,alarmEntry_t *alarmp),
			     unsigned long dataval,void *datap));
int k_eventEntry_initialize SR_PROTOTYPE((void));
int k_eventEntry_terminate SR_PROTOTYPE((void));
int k_logEntry_initialize SR_PROTOTYPE((void));
int k_logEntry_terminate SR_PROTOTYPE((void));
void eventTableDeleteCallback SR_PROTOTYPE((TimeOutDescriptor *tdp));
void DeleteeventTableEntry SR_PROTOTYPE((int index));
void k_eventEntryFreeUserpartData SR_PROTOTYPE((eventEntry_t *data));
void eventEntry_free SR_PROTOTYPE((eventEntry_t *data));

/* k_filt.c  */
void SendPacketMatchAlarm SR_PROTOTYPE((eventEntry_t *evp,unsigned long dummyval,channelEntry_t *cp));
void ActivateChannel SR_PROTOTYPE((long channelIndex));
int IsChannelActive SR_PROTOTYPE((long channelIndex,PacketState *psp));
void SetChannelDataControl SR_PROTOTYPE((long eventIndex));
void ProcessChannels SR_PROTOTYPE((PacketState *psp));
void ProcessFilters SR_PROTOTYPE((PacketState *psp));
int k_filterEntry_initialize SR_PROTOTYPE((void));
int k_filterEntry_terminate SR_PROTOTYPE((void));
int k_channelEntry_initialize SR_PROTOTYPE((void));
int k_channelEntry_terminate SR_PROTOTYPE((void));
void filterTableDeleteCallback SR_PROTOTYPE((TimeOutDescriptor *tdp));
void DeletefilterTableEntry SR_PROTOTYPE((int index));
void k_filterEntryFreeUserpartData SR_PROTOTYPE((filterEntry_t *data));
void filterEntry_free SR_PROTOTYPE((filterEntry_t *data));
void channelEntry_free SR_PROTOTYPE((channelEntry_t *data));

/* k_hist.c  */
void UpdateHistoryStatistics SR_PROTOTYPE((PacketState *psp));
void NextHistoryBucketCallback SR_PROTOTYPE((TimeOutDescriptor *tdp));
void GetHistoryBuckets SR_PROTOTYPE((historyControlEntry_t *hcp));
void *SetHistoryBucketTimer SR_PROTOTYPE((historyControlEntry_t *hcp));
int k_historyControlEntry_initialize SR_PROTOTYPE((void));
int k_historyControlEntry_terminate SR_PROTOTYPE((void));
void historyControlTableDeleteCallback SR_PROTOTYPE((TimeOutDescriptor *tdp));
void DeletehistoryControlTableEntry SR_PROTOTYPE((int index));
void k_historyControlEntryFreeUserpartData SR_PROTOTYPE((historyControlEntry_t *data));
void historyControlEntry_free SR_PROTOTYPE((historyControlEntry_t *data));

/* k_host.c  */
void HostEntry_free SR_PROTOTYPE((hostEntry_t *hep));
void DeleteHostEntryTable SR_PROTOTYPE((hostControlEntry_t *hcp));
void DeleteLRUHostEntry SR_PROTOTYPE((hostControlEntry_t *hcp));
hostEntry_t *AddNewHostEntry SR_PROTOTYPE((hostControlEntry_t *hcp,PacketState *psp));
void ResequenceHostTable SR_PROTOTYPE((hostControlEntry_t *hcp));
void ProcessHostEntry SR_PROTOTYPE((PacketState *psp));
int k_hostControlEntry_initialize SR_PROTOTYPE((void));
int k_hostControlEntry_terminate SR_PROTOTYPE((void));
void hostControlTableDeleteCallback SR_PROTOTYPE((TimeOutDescriptor *tdp));
void DeletehostControlTableEntry SR_PROTOTYPE((int index));
void k_hostControlEntryFreeUserpartData SR_PROTOTYPE((hostControlEntry_t *data));
void hostControlEntry_free SR_PROTOTYPE((hostControlEntry_t *data));

/* k_mtrx.c  */
void MatrixEntry_free SR_PROTOTYPE((matrixSDEntry_t *mep));
void DeleteMatrixEntryTable SR_PROTOTYPE((matrixControlEntry_t *mcp));
void DeleteLRUMatrixEntry SR_PROTOTYPE((matrixControlEntry_t *mcp));
int CompareMatrixDSEntries SR_PROTOTYPE((matrixDSEntry_t **dsp1 ,matrixDSEntry_t **dsp2 ));
matrixSDEntry_t *AddNewMatrixEntry SR_PROTOTYPE((matrixControlEntry_t *mcp,PacketState *psp));
void ResequenceMatrixTable SR_PROTOTYPE((matrixControlEntry_t *mcp));
void ProcessMatrixEntry SR_PROTOTYPE((PacketState *psp));
int k_matrixControlEntry_initialize SR_PROTOTYPE((void));
int k_matrixControlEntry_terminate SR_PROTOTYPE((void));
void matrixControlTableDeleteCallback SR_PROTOTYPE((TimeOutDescriptor *tdp));
void DeletematrixControlTableEntry SR_PROTOTYPE((int index));
void k_matrixControlEntryFreeUserpartData SR_PROTOTYPE((matrixControlEntry_t *data));
int IncOctetString SR_PROTOTYPE((OctetString *dstosp,OctetString *srcosp));
void matrixControlEntry_free SR_PROTOTYPE((matrixControlEntry_t *data));

/* k_stats.c  */
void UpdatePacketStatistics SR_PROTOTYPE((PacketState *psp));
int k_etherStatsEntry_initialize SR_PROTOTYPE((void));
int k_etherStatsEntry_terminate SR_PROTOTYPE((void));
void etherStatsTableDeleteCallback SR_PROTOTYPE((TimeOutDescriptor *tdp));
void DeleteetherStatsTableEntry SR_PROTOTYPE((int index));
void k_etherStatsEntryFreeUserpartData SR_PROTOTYPE((etherStatsEntry_t *data));
void etherStatsEntry_free SR_PROTOTYPE((etherStatsEntry_t *data));

/* k_topn.c  */
hostControlEntry_t *GetValidHCP SR_PROTOTYPE((long index));
void GetTopNTable SR_PROTOTYPE((hostTopNControlEntry_t *tncp));
void ClearTopNValidData SR_PROTOTYPE((hostTopNControlEntry_t *tncp));
void FreeTopNHostTable SR_PROTOTYPE((hostTopNControlEntry_t *tncp));
int CloneHostTable SR_PROTOTYPE((hostTopNControlEntry_t *tncp));
unsigned long GetHostEntryRate SR_PROTOTYPE((long base,hostEntry_t *hep));
void TopNCallback SR_PROTOTYPE((TimeOutDescriptor *tdp));
void CancelTopNTimer SR_PROTOTYPE((hostTopNControlEntry_t *tncp));
void *SetTopNTimer SR_PROTOTYPE((hostTopNControlEntry_t *tncp));
int k_hostTopNControlEntry_initialize SR_PROTOTYPE((void));
int k_hostTopNControlEntry_terminate SR_PROTOTYPE((void));
void hostTopNControlTableDeleteCallback SR_PROTOTYPE((TimeOutDescriptor *tdp));
void DeletehostTopNControlTableEntry SR_PROTOTYPE((int index));
void k_hostTopNControlEntryFreeUserpartData SR_PROTOTYPE((hostTopNControlEntry_t *data));
void hostTopNControlEntry_free SR_PROTOTYPE((hostTopNControlEntry_t *data));

/* rmontrap.c  */
void SendTrapToV1PartyCommunity SR_PROTOTYPE((OID *enterprise,long genTrap,long specTrap,VarBind *vb,char *entV2Trap,OctetString *community));
void RmonTrap SR_PROTOTYPE((long genTrap,long specTrap,VarBind *vb,OID *enterprise,char *entV2Trap,eventEntry_t *evp));

/* rmonmain.c  */
int CheckEntryStatus SR_PROTOTYPE((long new,long old));
int CheckEthernetDataSource SR_PROTOTYPE((OID *source));
int CmpDataSource SR_PROTOTYPE((OID *s1,OID *s2));
int IsBroadcast SR_PROTOTYPE((unsigned char *p));
void CancelEntryStatusTimeout SR_PROTOTYPE((int id));
int SetEntryStatusTimeout SR_PROTOTYPE((long wait,void *data,void (*callback)(TimeOutDescriptor *tdp)));
void TimerInfoFree SR_PROTOTYPE((void *info));
void UpdateTables SR_PROTOTYPE((PacketState *psp));

/* rmonagt.c  */
VarBind *i_GetMib SR_PROTOTYPE((OID *object));
void RMONCollision SR_PROTOTYPE((int ifnum,unsigned long cnt));
void RMONDrop SR_PROTOTYPE((int ifnum,unsigned long cnt));
void StoreRMONFrame SR_PROTOTYPE((unsigned char *p,int len,int ifnum,int error));
int DoRMON SR_PROTOTYPE((void *arg));
int InitRMON SR_PROTOTYPE((void));

/* startup.c */
void SetEtherStatsMonitor SR_PROTOTYPE((long index, OID *source, char *owner));
void SetHistoryMonitor SR_PROTOTYPE((long index, OID *source, char *owner, long requested, long interval));

/* table.c  */
int CmpValues SR_PROTOTYPE((int nominator,SnmpType *typep,unsigned long *olddata,unsigned long newval));
int CopyValues SR_PROTOTYPE((int nominator,SnmpType *typep,unsigned long *new,unsigned long *old));
void TransferValues SR_PROTOTYPE((int nominator,SnmpType *typep,unsigned long *new,unsigned long *old));
int CopyEntries SR_PROTOTYPE((int maxNominator,SnmpType *typep,unsigned long *new,unsigned long *old));
void TransferEntries SR_PROTOTYPE((int maxNominator,SnmpType *typep,unsigned long *new,unsigned long *old));
int SearchTable SR_PROTOTYPE((RmonTable *rtp,int st));
int NewTableEntry SR_PROTOTYPE((RmonTable *rtp));
void PrintTable SR_PROTOTYPE((RmonTable *rtp));
void RemoveTableEntry SR_PROTOTYPE((RmonTable *rtp,int index));

