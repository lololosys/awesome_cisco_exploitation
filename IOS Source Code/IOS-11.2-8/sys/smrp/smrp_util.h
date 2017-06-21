/* $Id: smrp_util.h,v 3.2 1995/11/17 18:54:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smrp/smrp_util.h,v $
 *------------------------------------------------------------------
 * Support routines for SMRP.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_util.h,v $
 * Revision 3.2  1995/11/17  18:54:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:25:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/08/01  00:23:18  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.4  1995/07/25  21:29:42  slin
 * CSCdi37501:  Timing problem can cause legit forward entry to be removed.
 *
 * Revision 2.3  1995/07/18  19:47:48  kleung
 * CSCdi37260:  The first MC group should be randomly selected.
 *
 * Revision 2.2  1995/06/14  22:55:22  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:45:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __SMRP_UTIL_H__
#define __SMRP_UTIL_H__

/*
 *==================================================================
 * SMRP Utility Definitions and Data structures
 *==================================================================
 */


/*
 *------------------------------------------------------------------
 * SMRPNetworkBytes - Byte access to data values
 *------------------------------------------------------------------
 */
typedef union {
    ulong data;
    uchar bytes[4];
    ushort words[2];
} SMRPNetworkBytes;

/*
 *==================================================================
 * SMRP Internal Entry Points defined by smrp_util.
 *==================================================================
 */
void smrp_util_Initialize(void);
extern boolean smrp_LinkPortToIdb(SMRPPort *, idbtype *);
extern SMRPPort* smrp_FindPortFromIdb(SMRPIdbsb *, int);
extern ulong smrp_MakePortID(SMRPNet const *);
boolean smrp_can_add_hwaddr(hwidbtype *);
boolean smrp_IsSpecialMulticastID(ulong);
ushort smrp_Random(int);

const char* smrp_ForwardStateMessage(ForwardState);
const char* smrp_ForwardStateShortMessage(ForwardState);
const char* smrp_StateMessage(NodeState);
const char* smrp_StateShortMessage(NodeState);
const char* smrp_ErrorMessage(ulong);
const char* smrp_TransStateMessage(RequestState);
boolean smrp_IsValidNetRange(const SMRPNetRange*);
void smrp_PrintNetRangeKey(char*,treeKey);
const char* smrp_PrintNetRange(SMRPNetRange const *);
void smrp_PrintGroupKey(char*,treeKey);
void smrp_SetupHeader(MCHeader*,uchar,ushort,SMRPGroupID);
boolean smrp_IsFirstPreferred(UnicastAddr,UnicastAddr);
void smrp_PutThreeBytes(uchar*, ulong);
ulong smrp_GetThreeBytes(uchar*);
const char* smrp_PrintCmdType(uchar);
const char* smrp_PrintPacketType(uchar);
void smrp_assert(char *, int);
void smrp_Error(char const *);
void smrp_SetCommonMulticasts(SMRPPort *);
SMRPGroupID smrp_MapSpecialAddrToGroupID(UnicastAddr);
boolean smrp_IsSpecialGroupID(SMRPGroupID);
uchar *smrp_DumpBuffer(uchar*,uint,uchar*);

#endif __SMRP_UTIL_H__
