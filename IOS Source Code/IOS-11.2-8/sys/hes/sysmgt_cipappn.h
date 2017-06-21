/* $Id: sysmgt_cipappn.h,v 3.1.4.1 1996/05/17 11:19:20 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/hes/sysmgt_cipappn.h,v $
 *------------------------------------------------------------------
 * appn mib rp-cip communication structure
 *
 * Jan 96, Barry Chan
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sysmgt_cipappn.h,v $
 * Revision 3.1.4.1  1996/05/17  11:19:20  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.1  1996/03/21  03:21:47  bchan
 * CSCdi52147:  Add tn3270 server new files to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/03/20  07:01:19  bchan
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef sysmgt_cipappn_h
#define sysmgt_cipappn_h


#define STRING_SIZE_UNDEF  (24)
#define STRING_SIZE_18     (18+1)
#define STRING_SIZE_17     (17+1)
#define STRING_SIZE_9      (9+1)
#define STRING_SIZE_8      (8+1)
#define STRING_SIZE_6      (6+1)
#define STRING_SIZE_5      (5+1)
#define STRING_SIZE_3      (3+1)


/* 
 * Note - The lengths of the following structures must be 4-byte multiples.
 *        In addition, 'long' elements must be 4-byte aligned.
 */


/*
 * The ibmappnGeneralInfoAndCaps family. 
 */


typedef
struct _ibmappnGeneralInfoAndCaps_T {

        char            ibmappnNodeCpName[STRING_SIZE_17];
        char            ibmappnNodeNetid[STRING_SIZE_8];
        char            ibmappnNodeBlockNum[STRING_SIZE_3];
        char            ibmappnNodeIdNum[STRING_SIZE_5];
        char            align[3];
        long            ibmappnNodeType;
        unsigned long   ibmappnNodeUpTime;
        long            ibmappnNodeNegotLs;
        long            ibmappnNodeSegReasm;
        long            ibmappnNodeBindReasm;
        long            ibmappnNodeParallelTg;
        long            ibmappnNodeService;
        long            ibmappnNodeAdaptiveBindPacing;
} ibmappnGeneralInfoAndCaps_T;



/*
 * The ibmappnNnUniqueInfoAndCaps family. 
 */


typedef
struct _ibmappnNnUniqueInfoAndCaps_T {

        long            ibmappnNodeNnRcvRegChar;
        long            ibmappnNodeNnGateway;
        long            ibmappnNodeNnCentralDirectory;
        long            ibmappnNodeNnTreeCache;
        long            ibmappnNodeNnTreeUpdate;
        long            ibmappnNodeNnRouteAddResist;
        long            ibmappnNodeNnIsr;
        long            ibmappnNodeNnFrsn;
} ibmappnNnUniqueInfoAndCaps_T;


/*
 * The ibmappnEnUniqueCaps family. 
 */


typedef
struct _ibmappnEnUniqueCaps_T {
  
        long            ibmappnNodeEnSegGen;
        long            ibmappnNodeEnModeCosMap;
        long            ibmappnNodeEnLocateCdinit;
        long            ibmappnNodeEnSendRegNames;
        long            ibmappnNodeEnSendRegChar;
} ibmappnEnUniqueCaps_T;



/*
 * The ibmappnNodePortEntry family. 
 */


typedef
struct _ibmappnNodePortEntry_T {
        char            ibmappnNodePortName[STRING_SIZE_8];
        char            align[3];

        long            ibmappnNodePortState;
        long            ibmappnNodePortDlcType;
        long            ibmappnNodePortPortType;
        long            ibmappnNodePortSIMRIM;
        long            ibmappnNodePortLsRole;
        long            ibmappnNodePortMaxRcvBtuSize;
        long            ibmappnNodePortMaxIframeWindow;
        unsigned long   ibmappnNodePortDefLsGoodXids;
        unsigned long   ibmappnNodePortDefLsBadXids;
        unsigned long   ibmappnNodePortDynLsGoodXids;
        unsigned long   ibmappnNodePortDynLsBadXids;
} ibmappnNodePortEntry_T;



/*
 * The ibmappnNodePortIpEntry family. 
 */


typedef
struct _ibmappnNodePortIpEntry_T {
        char            ibmappnNodePortIpName[STRING_SIZE_8];
        char            align[3];
  
        long            ibmappnNodePortIpPortNum;
} ibmappnNodePortIpEntry_T;



/*
 * The ibmappnNodePortDlsEntry family. 
 */


typedef
struct _ibmappnNodePortDlsEntry_T {
        char            ibmappnNodePortDlsName[STRING_SIZE_8];
  
        char            ibmappnNodePortDlsMac[6];
        char            ibmappnNodePortDlsSap;
} ibmappnNodePortDlsEntry_T;



/*
 * The ibmappnNodePortTrEntry family. 
 */


typedef
struct _ibmappnNodePortTrEntry_T {
        char            ibmappnNodePortTrName[STRING_SIZE_8];
  
        char            ibmappnNodePortTrMac[6];
        char            ibmappnNodePortTrSap;
} ibmappnNodePortTrEntry_T;



/*
 * The ibmappnNodePortDlcTraceEntry family. 
 */


typedef
struct _ibmappnNodePortDlcTraceEntry_T {
        long            ibmappnNodePortDlcTracIndex;
        char            ibmappnNodePortDlcTracPortName[STRING_SIZE_UNDEF];
  
        long            ibmappnNodePortDlcTracDlcType;
        char            ibmappnNodePortDlcTracLocalAddr[STRING_SIZE_UNDEF];
        char            ibmappnNodePortDlcTracRemoteAddr[STRING_SIZE_UNDEF];
        long            ibmappnNodePortDlcTracMsgType;
        long            ibmappnNodePortDlcTracCmdType;
        long            ibmappnNodePortDlcTracUseWan;
} ibmappnNodePortDlcTraceEntry_T;



/*
 * The ibmappnNodeLsEntry family. 
 */


typedef
struct _ibmappnNodeLsEntry_T {
        char            ibmappnNodeLsName[STRING_SIZE_8];
  
        char            ibmappnNodeLsPortName[STRING_SIZE_8];
        char            ibmappnNodeLsCpName[STRING_SIZE_17];
        char            ibmappnNodeLsBlockNum[STRING_SIZE_3];
        char            ibmappnNodeLsIdNum[STRING_SIZE_5];
        char            align[2];
        long            ibmappnNodeLsDlcType;
        long            ibmappnNodeLsDynamic;
        long            ibmappnNodeLsState;
        long            ibmappnNodeLsTgNum;
        long            ibmappnNodeLsLimResource;
        long            ibmappnNodeLsMigration;
        long            ibmappnNodeLsCpCpSession;
        long            ibmappnNodeLsTargetPacingCount;
        long            ibmappnNodeLsMaxSendBtuSize;
        long            ibmappnNodeLsEffCap;
        long            ibmappnNodeLsConnCost;
        long            ibmappnNodeLsByteCost;
        long            ibmappnNodeLsSecurity;
        long            ibmappnNodeLsDelay;
        long            ibmappnNodeLsUsr1;
        long            ibmappnNodeLsUsr2;
        long            ibmappnNodeLsUsr3;
        unsigned long   ibmappnNodeLsInXidBytes;
        unsigned long   ibmappnNodeLsInMsgBytes;
        unsigned long   ibmappnNodeLsInXidFrames;
        unsigned long   ibmappnNodeLsInMsgFrames;
        unsigned long   ibmappnNodeLsOutXidBytes;
        unsigned long   ibmappnNodeLsOutMsgBytes;
        unsigned long   ibmappnNodeLsOutXidFrames;
        unsigned long   ibmappnNodeLsOutMsgFrames;
        unsigned long   ibmappnNodeLsEchoRsps;
        long            ibmappnNodeLsCurrentDelay;
        long            ibmappnNodeLsMaxDelay;
        long            ibmappnNodeLsMinDelay;
        unsigned long   ibmappnNodeLsMaxDelayTime;
        unsigned long   ibmappnNodeLsGoodXids;
        unsigned long   ibmappnNodeLsBadXids;
        long            ibmappnNodeLsSubState;
        unsigned long   ibmappnNodeLsStartTime;
        unsigned long   ibmappnNodeLsActiveTime;
        unsigned long   ibmappnNodeLsCurrentStateTime;
} ibmappnNodeLsEntry_T;



/*
 * The ibmappnNodeLsIpEntry family. 
 */


typedef
struct _ibmappnNodeLsIpEntry_T {
        char            ibmappnNodeLsIpName[STRING_SIZE_8];
        char            align[3];
  
        long            ibmappnNodeLsIpState;
        unsigned long   ibmappnNodeLsLocalIpAddr;
        long            ibmappnNodeLsLocalIpPortNum;
        unsigned long   ibmappnNodeLsRemoteIpAddr;
        long            ibmappnNodeLsRemoteIpPortNum;
} ibmappnNodeLsIpEntry_T;



/*
 * The ibmappnNodeLsDlsEntry family. 
 */


typedef
struct _ibmappnNodeLsDlsEntry_T {
        char            ibmappnNodeLsDlsName[STRING_SIZE_8];
  
        char            ibmappnNodeLsLocalDlsMac[6];
        char            ibmappnNodeLsLocalDlsSap;
        char            ibmappnNodeLsRemoteDlsMac[6];
        char            ibmappnNodeLsRemoteDlsSap;
        char            align;
        long            ibmappnNodeLsDlsState;
} ibmappnNodeLsDlsEntry_T;



/*
 * The ibmappnNodeLsTrEntry family. 
 */


typedef
struct _ibmappnNodeLsTrEntry_T {
        char            ibmappnNodeLsTrName[STRING_SIZE_8];
  
        char            ibmappnNodeLsLocalTrMac[6];
        char            ibmappnNodeLsLocalTrSap;
        char            ibmappnNodeLsRemoteTrMac[6];
        char            ibmappnNodeLsRemoteTrSap;
        char            align;
        long            ibmappnNodeLsTrState;
} ibmappnNodeLsTrEntry_T;



/*
 * The ibmappnNodeLsStatusEntry family. 
 */


typedef
struct _ibmappnNodeLsStatusEntry_T {
        long            ibmappnNodeLsStatusIndex;
  
        unsigned long   ibmappnNodeLsStatusTime;
        char            ibmappnNodeLsStatusLsName[STRING_SIZE_17];
        char            ibmappnNodeLsStatusCpName[STRING_SIZE_18];
        char            align[3];
        char            ibmappnNodeLsStatusNodeId[STRING_SIZE_UNDEF];
        long            ibmappnNodeLsStatusTgNum;
        char            ibmappnNodeLsStatusGeneralSense[STRING_SIZE_UNDEF];
        long            ibmappnNodeLsStatusNofRetry;
        char            ibmappnNodeLsStatusEndSense[STRING_SIZE_UNDEF];
        char            ibmappnNodeLsStatusXidLocalSense[STRING_SIZE_UNDEF];
        char            ibmappnNodeLsStatusXidRemoteSense[STRING_SIZE_UNDEF];
        long            ibmappnNodeLsStatusXidByteInError;
        long            ibmappnNodeLsStatusXidBitInError;
        long            ibmappnNodeLsStatusDlcType;
        char            ibmappnNodeLsStatusLocalAddr[STRING_SIZE_UNDEF];
        char            ibmappnNodeLsStatusRemoteAddr[STRING_SIZE_UNDEF];
} ibmappnNodeLsStatusEntry_T;



/*
 * The ibmappnSnmpInformation family. 
 */


typedef
struct _ibmappnSnmpInformation_T {

        unsigned long   ibmappnSnmpInPkts;
        unsigned long   ibmappnSnmpInGetRequests;
        unsigned long   ibmappnSnmpInGetNexts;
        unsigned long   ibmappnSnmpInSetRequests;
        unsigned long   ibmappnSnmpInTotalVars;
        unsigned long   ibmappnSnmpInGetVars;
        unsigned long   ibmappnSnmpInGetNextVars;
        unsigned long   ibmappnSnmpInSetVars;
        unsigned long   ibmappnSnmpOutNoSuchNames;
        unsigned long   ibmappnSnmpOutGenErrs;
} ibmappnSnmpInformation_T;



/*
 * The ibmappnMemoryUse family. 
 */


typedef
struct _ibmappnMemoryUse_T {
  
        long            ibmappnMemorySize;
        long            ibmappnMemoryUsed;
        long            ibmappnMemoryWarnThresh;
        long            ibmappnMemoryCritThresh;
} ibmappnMemoryUse_T;



/*
 * The ibmappnXidInformation family. 
 */


typedef
struct _ibmappnXidInformation_T {
  
        unsigned long   ibmappnNodeDefLsGoodXids;
        unsigned long   ibmappnNodeDefLsBadXids;
        unsigned long   ibmappnNodeDynLsGoodXids;
        unsigned long   ibmappnNodeDynLsBadXids;
} ibmappnXidInformation_T;



/*
 * The ibmappnNnTopo family. 
 */


typedef
struct _ibmappnNnTopo_T {
  
        long            ibmappnNnTopoMaxNodes;
        unsigned long   ibmappnNnTopoCurNumNodes;
        unsigned long   ibmappnNnTopoInTdus;
        unsigned long   ibmappnNnTopoOutTdus;
        unsigned long   ibmappnNnTopoNodeLowRsns;
        unsigned long   ibmappnNnTopoNodeEqualRsns;
        unsigned long   ibmappnNnTopoNodeGoodHighRsns;
        unsigned long   ibmappnNnTopoNodeBadHighRsns;
        unsigned long   ibmappnNnTopoNodeStateUpdates;
        unsigned long   ibmappnNnTopoNodeErrors;
        unsigned long   ibmappnNnTopoNodeTimerUpdates;
        unsigned long   ibmappnNnTopoNodePurges;
        unsigned long   ibmappnNnTopoTgLowRsns;
        unsigned long   ibmappnNnTopoTgEqualRsns;
        unsigned long   ibmappnNnTopoTgGoodHighRsns;
        unsigned long   ibmappnNnTopoTgBadHighRsns;
        unsigned long   ibmappnNnTopoTgStateUpdates;
        unsigned long   ibmappnNnTopoTgErrors;
        unsigned long   ibmappnNnTopoTgTimerUpdates;
        unsigned long   ibmappnNnTopoTgPurges;
        unsigned long   ibmappnNnTopoTotalRouteCalcs;
        unsigned long   ibmappnNnTopoTotalRouteRejs;
} ibmappnNnTopo_T;



/*
 * The ibmappnNnTopoRouteEntry family. 
 */


typedef
struct _ibmappnNnTopoRouteEntry_T {
        char            ibmappnNnTopoRouteCos[STRING_SIZE_UNDEF];
  
        unsigned long   ibmappnNnTopoRouteTrees;
        unsigned long   ibmappnNnTopoRouteCalcs;
        unsigned long   ibmappnNnTopoRouteRejs;
} ibmappnNnTopoRouteEntry_T;



/*
 * The ibmappnNnAdjNodeEntry family. 
 */


typedef
struct _ibmappnNnAdjNodeEntry_T {
        char            ibmappnNnAdjNodeAdjName[STRING_SIZE_17];
        char            align[2];
  
        long            ibmappnNnAdjNodeCpCpSessStatus;
        unsigned long   ibmappnNnAdjNodeOutOfSeqTdus;
        long            ibmappnNnAdjNodeLastFrsnSent;
        long            ibmappnNnAdjNodeLastFrsnRcvd;
} ibmappnNnAdjNodeEntry_T;



/*
 * The ibmappnNnTopologyEntry family. 
 */


typedef
struct _ibmappnNnTopologyEntry_T {
        char            ibmappnNnNodeName[STRING_SIZE_17];
        char            align[2];
  
        long            ibmappnNnNodeFrsn;
        long            ibmappnNnNodeEntryTimeLeft;
        long            ibmappnNnNodeType;
        long            ibmappnNnNodeRsn;
        long            ibmappnNnNodeRouteAddResist;
        long            ibmappnNnNodeCongested;
        long            ibmappnNnNodeIsrDepleted;
        long            ibmappnNnNodeEndptDepleted;
        long            ibmappnNnNodeQuiescing;
        long            ibmappnNnNodeGateway;
        long            ibmappnNnNodeCentralDirectory;
        long            ibmappnNnNodeIsr;
        long            ibmappnNnNodeChainSupport;
} ibmappnNnTopologyEntry_T;



/*
 * The ibmappnNnTgTopologyEntry family. 
 */


typedef
struct _ibmappnNnTgTopologyEntry_T {
        long            ibmappnNnTgNum;
        char            ibmappnNnTgOwner[STRING_SIZE_17];
        char            ibmappnNnTgDest[STRING_SIZE_17];
  
        char            ibmappnNnTgDlcData[9];
        char            align[3];
        long            ibmappnNnTgFrsn;
        long            ibmappnNnTgEntryTimeLeft;
        long            ibmappnNnTgDestVirtual;
        long            ibmappnNnTgRsn;
        long            ibmappnNnTgOperational;
        long            ibmappnNnTgQuiescing;
        long            ibmappnNnTgCpCpSession;
        long            ibmappnNnTgEffCap;
        long            ibmappnNnTgConnCost;
        long            ibmappnNnTgByteCost;
        long            ibmappnNnTgSecurity;
        long            ibmappnNnTgDelay;
        long            ibmappnNnTgModemClass;
        long            ibmappnNnTgUsr1;
        long            ibmappnNnTgUsr2;
        long            ibmappnNnTgUsr3;
} ibmappnNnTgTopologyEntry_T;



/*
 * The ibmappnNnTopologyFREntry family. 
 */


typedef
struct _ibmappnNnTopologyFREntry_T {
        long            ibmappnNnNodeFRFrsn;
        char            ibmappnNnNodeFRName[STRING_SIZE_17];
        char            align[2];
  
        long            ibmappnNnNodeFREntryTimeLeft;
        long            ibmappnNnNodeFRType;
        long            ibmappnNnNodeFRRsn;
        long            ibmappnNnNodeFRRouteAddResist;
        long            ibmappnNnNodeFRCongested;
        long            ibmappnNnNodeFRIsrDepleted;
        long            ibmappnNnNodeFREndptDepleted;
        long            ibmappnNnNodeFRQuiescing;
        long            ibmappnNnNodeFRGateway;
        long            ibmappnNnNodeFRCentralDirectory;
        long            ibmappnNnNodeFRIsr;
        long            ibmappnNnNodeFRChainSupport;
} ibmappnNnTopologyFREntry_T;



/*
 * The ibmappnNnTgTopologyFREntry family. 
 */


typedef
struct _ibmappnNnTgTopologyFREntry_T {
        long            ibmappnNnTgFRNum;
        long            ibmappnNnTgFRFrsn;
        char            ibmappnNnTgFROwner[STRING_SIZE_17];
        char            ibmappnNnTgFRDest[STRING_SIZE_17];
  
        char            ibmappnNnTgFRDlcData[9];
        char            align[3];
        long            ibmappnNnTgFREntryTimeLeft;
        long            ibmappnNnTgFRDestVirtual;
        long            ibmappnNnTgFRRsn;
        long            ibmappnNnTgFROperational;
        long            ibmappnNnTgFRQuiescing;
        long            ibmappnNnTgFRCpCpSession;
        long            ibmappnNnTgFREffCap;
        long            ibmappnNnTgFRConnCost;
        long            ibmappnNnTgFRByteCost;
        long            ibmappnNnTgFRSecurity;
        long            ibmappnNnTgFRDelay;
        long            ibmappnNnTgFRModemClass;
        long            ibmappnNnTgFRUsr1;
        long            ibmappnNnTgFRUsr2;
        long            ibmappnNnTgFRUsr3;
} ibmappnNnTgTopologyFREntry_T;



/*
 * The ibmappnLocalGeneral family. 
 */


typedef
struct _ibmappnLocalGeneral_T {
  
        char            ibmappnLocalNodeName[STRING_SIZE_17];
        char            align[2];
        long            ibmappnLocalNodeType;
} ibmappnLocalGeneral_T;



/*
 * The ibmappnLocalNnSpecific family. 
 */


typedef
struct _ibmappnLocalNnSpecific_T {
  
        long            ibmappnLocalNnRsn;
        long            ibmappnLocalNnRouteAddResist;
        long            ibmappnLocalNnCongested;
        long            ibmappnLocalNnIsrDepleted;
        long            ibmappnLocalNnEndptDepleted;
        long            ibmappnLocalNnQuiescing;
        long            ibmappnLocalNnGateway;
        long            ibmappnLocalNnCentralDirectory;
        long            ibmappnLocalNnIsr;
        long            ibmappnLocalNnChainSupport;
        long            ibmappnLocalNnFrsn;
} ibmappnLocalNnSpecific_T;



/*
 * The ibmappnLocalTgEntry family. 
 */


typedef
struct _ibmappnLocalTgEntry_T {
        long            ibmappnLocalTgNum;
        char            ibmappnLocalTgDest[STRING_SIZE_17];
  
        char            ibmappnLocalTgDlcData[9];
        char            align;
        long            ibmappnLocalTgDestVirtual;
        long            ibmappnLocalTgRsn;
        long            ibmappnLocalTgQuiescing;
        long            ibmappnLocalTgOperational;
        long            ibmappnLocalTgCpCpSession;
        long            ibmappnLocalTgEffCap;
        long            ibmappnLocalTgConnCost;
        long            ibmappnLocalTgByteCost;
        long            ibmappnLocalTgSecurity;
        long            ibmappnLocalTgDelay;
        long            ibmappnLocalTgModemClass;
        long            ibmappnLocalTgUsr1;
        long            ibmappnLocalTgUsr2;
        long            ibmappnLocalTgUsr3;
} ibmappnLocalTgEntry_T;



/*
 * The ibmappnLocalEnEntry family. 
 */


typedef
struct _ibmappnLocalEnEntry_T {
        char            ibmappnLocalEnName[STRING_SIZE_17];
        char            align[2];
  
        long            ibmappnLocalEnEntryTimeLeft;
        long            ibmappnLocalEnType;
} ibmappnLocalEnEntry_T;



/*
 * The ibmappnLocalEnTgEntry family. 
 */


typedef
struct _ibmappnLocalEnTgEntry_T {
        long            ibmappnLocalEnTgNum;
        char            ibmappnLocalEnTgOrigin[STRING_SIZE_17];
        char            ibmappnLocalEnTgDest[STRING_SIZE_17];
  
        char            ibmappnLocalEnTgDlcData[9];
        char            align[3];
        long            ibmappnLocalEnTgEntryTimeLeft;
        long            ibmappnLocalEnTgDestVirtual;
        long            ibmappnLocalEnTgOperational;
        long            ibmappnLocalEnTgCpCpSession;
        long            ibmappnLocalEnTgEffCap;
        long            ibmappnLocalEnTgConnCost;
        long            ibmappnLocalEnTgByteCost;
        long            ibmappnLocalEnTgSecurity;
        long            ibmappnLocalEnTgDelay;
        long            ibmappnLocalEnTgModemClass;
        long            ibmappnLocalEnTgUsr1;
        long            ibmappnLocalEnTgUsr2;
        long            ibmappnLocalEnTgUsr3;
} ibmappnLocalEnTgEntry_T;



/*
 * The ibmappnDirPerf family. 
 */


typedef
struct _ibmappnDirPerf_T {
  
        long            ibmappnDirMaxCaches;
        unsigned long   ibmappnDirCurCaches;
        unsigned long   ibmappnDirCurHomeEntries;
        unsigned long   ibmappnDirRegEntries;
        unsigned long   ibmappnDirInLocates;
        unsigned long   ibmappnDirInBcastLocates;
        unsigned long   ibmappnDirOutLocates;
        unsigned long   ibmappnDirOutBcastLocates;
        unsigned long   ibmappnDirNotFoundLocates;
        unsigned long   ibmappnDirNotFoundBcastLocates;
        unsigned long   ibmappnDirLocateOutstands;
} ibmappnDirPerf_T;



/*
 * The ibmappnDirEntry family. 
 */


typedef
struct _ibmappnDirEntry_T {
        char            ibmappnDirLuName[STRING_SIZE_17];
  
        char            ibmappnDirServerName[STRING_SIZE_17];
        char            ibmappnDirLuOwnerName[STRING_SIZE_17];
        char            align[2];
        long            ibmappnDirLuLocation;
        long            ibmappnDirType;
        long            ibmappnDirWildCard;
} ibmappnDirEntry_T;



/*
 * The ibmappnCosModeEntry family. 
 */


typedef
struct _ibmappnCosModeEntry_T {
        char            ibmappnCosModeName[STRING_SIZE_8];
  
        char            ibmappnCosModeCosName[STRING_SIZE_8];
        char            align[2];
} ibmappnCosModeEntry_T;



/*
 * The ibmappnCosNameEntry family. 
 */


typedef
struct _ibmappnCosNameEntry_T {
        char            ibmappnCosName[STRING_SIZE_8];
        char            align[3];
  
        long            ibmappnCosTransPriority;
} ibmappnCosNameEntry_T;



/*
 * The ibmappnCosNodeRowEntry family. 
 */


typedef
struct _ibmappnCosNodeRowEntry_T {
        long            ibmappnCosNodeRowIndex;
        char            ibmappnCosNodeRowName[STRING_SIZE_8];
        char            align[3];
  
        char            ibmappnCosNodeRowWgt[STRING_SIZE_UNDEF];
        long            ibmappnCosNodeRowResistMin;
        long            ibmappnCosNodeRowResistMax;
        long            ibmappnCosNodeRowMinCongestAllow;
        long            ibmappnCosNodeRowMaxCongestAllow;
} ibmappnCosNodeRowEntry_T;



/*
 * The ibmappnCosTgRowEntry family. 
 */


typedef
struct _ibmappnCosTgRowEntry_T {
        long            ibmappnCosTgRowIndex;
        char            ibmappnCosTgRowName[STRING_SIZE_8];
        char            align[3];
  
        char            ibmappnCosTgRowWgt[STRING_SIZE_UNDEF];
        long            ibmappnCosTgRowEffCapMin;
        long            ibmappnCosTgRowEffCapMax;
        long            ibmappnCosTgRowConnCostMin;
        long            ibmappnCosTgRowConnCostMax;
        long            ibmappnCosTgRowByteCostMin;
        long            ibmappnCosTgRowByteCostMax;
        long            ibmappnCosTgRowSecurityMin;
        long            ibmappnCosTgRowSecurityMax;
        long            ibmappnCosTgRowDelayMin;
        long            ibmappnCosTgRowDelayMax;
        long            ibmappnCosTgRowUsr1Min;
        long            ibmappnCosTgRowUsr1Max;
        long            ibmappnCosTgRowUsr2Min;
        long            ibmappnCosTgRowUsr2Max;
        long            ibmappnCosTgRowUsr3Min;
        long            ibmappnCosTgRowUsr3Max;
} ibmappnCosTgRowEntry_T;



/*
 * The ibmappnIsInGlobal family. 
 */


typedef
struct _ibmappnIsInGlobal_T {
  
        long            ibmappnIsInGlobeStatus;
        long            ibmappnIsInGlobeRscv;
        unsigned long   ibmappnIsInGlobeRscvTime;
        long            ibmappnIsInGlobeCtrStatus;
        unsigned long   ibmappnIsInGlobeCtrStatusTime;
} ibmappnIsInGlobal_T;



/*
 * The ibmappnIsInEntry family. 
 */


typedef
struct _ibmappnIsInEntry_T {
        char            ibmappnIsInFqLuName[STRING_SIZE_17];
        char            ibmappnIsInPcid[STRING_SIZE_8];
  
        char            ibmappnIsInPriLuName[STRING_SIZE_17];
        char            ibmappnIsInSecLuName[STRING_SIZE_17];
        char            ibmappnIsInModeName[STRING_SIZE_8];
        char            ibmappnIsInCosName[STRING_SIZE_8];
        char            ibmappnIsInPsAdjCpName[STRING_SIZE_17];
        char            ibmappnIsInSsAdjCpName[STRING_SIZE_17];
        char            align[3];

        long            ibmappnIsInTransPriority;
        long            ibmappnIsInSessType;
        long            ibmappnIsInSessState;
        unsigned long   ibmappnIsInSessStartTime;
        unsigned long   ibmappnIsInSessUpTime;
        unsigned long   ibmappnIsInCtrUpTime;
        unsigned long   ibmappnIsInP2SFmdPius;
        unsigned long   ibmappnIsInS2PFmdPius;
        unsigned long   ibmappnIsInP2SNonFmdPius;
        unsigned long   ibmappnIsInS2PNonFmdPius;
        unsigned long   ibmappnIsInP2SFmdBytes;
        unsigned long   ibmappnIsInS2PFmdBytes;
        unsigned long   ibmappnIsInP2SNonFmdBytes;
        unsigned long   ibmappnIsInS2PNonFmdBytes;
        long            ibmappnIsInPsAdjTgNum;
        long            ibmappnIsInPsSendMaxBtuSize;
        long            ibmappnIsInPsSendPacingType;
        unsigned long   ibmappnIsInPsSendRpc;
        unsigned long   ibmappnIsInPsSendNxWndwSize;
        long            ibmappnIsInPsRecvPacingType;
        unsigned long   ibmappnIsInPsRecvRpc;
        unsigned long   ibmappnIsInPsRecvNxWndwSize;
        long            ibmappnIsInSsAdjTgNum;
        long            ibmappnIsInSsSendMaxBtuSize;
        long            ibmappnIsInSsSendPacingType;
        unsigned long   ibmappnIsInSsSendRpc;
        unsigned long   ibmappnIsInSsSendNxWndwSize;
        long            ibmappnIsInSsRecvPacingType;
        unsigned long   ibmappnIsInSsRecvRpc;
        unsigned long   ibmappnIsInSsRecvNxWndwSize;
        char            ibmappnIsInRouteInfo[STRING_SIZE_UNDEF];
} ibmappnIsInEntry_T;


   
typedef enum sysmgt_appn_table_ {
  SYSMGT_appnGeneralInfoAndCapsData,
  SYSMGT_appnNnUniqueInfoAndCapsData,
  SYSMGT_appnEnUniqueCapsData,
  SYSMGT_appnNodePortEntryData,
  SYSMGT_appnNodePortIpEntryData,
  SYSMGT_appnNodePortDlsEntryData,
  SYSMGT_appnNodePortTrEntryData, 
  SYSMGT_appnNodePortDlcTraceEntryData,
  SYSMGT_appnNodeLsEntryData,
  SYSMGT_appnNodeLsIpEntryData,
  SYSMGT_appnNodeLsDlsEntryData,
  SYSMGT_appnNodeLsTrEntryData,
  SYSMGT_appnNodeLsStatusEntryData,
  SYSMGT_appnSnmpInformationData,
  SYSMGT_appnMemoryUseData,
  SYSMGT_appnXidInformationData,
  SYSMGT_appnNnTopoData,
  SYSMGT_appnNnTopoRouteEntryData,
  SYSMGT_appnNnAdjNodeEntryData,
  SYSMGT_appnNnTopologyEntryData,
  SYSMGT_appnNnTgTopologyEntryData,
  SYSMGT_appnNnTopologyFREntryData,
  SYSMGT_appnNnTgTopologyFREntryData,
  SYSMGT_appnLocalGeneralData,
  SYSMGT_appnLocalNnSpecificData,
  SYSMGT_appnLocalTgEntryData,
  SYSMGT_appnLocalEnEntryData,
  SYSMGT_appnLocalEnTgEntryData,
  SYSMGT_appnDirPerfData,
  SYSMGT_appnDirEntryData,
  SYSMGT_appnCosModeEntryData,
  SYSMGT_appnCosNameEntryData,
  SYSMGT_appnCosNodeRowEntryData,
  SYSMGT_appnCosTgRowEntryData,
  SYSMGT_appnIsInGlobalData,
  SYSMGT_appnIsInEntryData,
  SYSMGT_APPN_MAXIMUM
} sysmgt_appn_table_t;

void init_cipappnmib(void);

#endif
