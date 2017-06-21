/* $Id: smrp_fast.h,v 3.2.64.1 1996/05/02 08:44:42 dwong Exp $
 * $Source: /release/112/cvs/Xsys/smrp/smrp_fast.h,v $
 *------------------------------------------------------------------
 * SMRP fast switching definitions
 *
 * August 1995, Kent Leung
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_fast.h,v $
 * Revision 3.2.64.1  1996/05/02  08:44:42  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.2  1995/11/17  18:53:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:25:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/05  02:20:10  kleung
 * Placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SMRP_FAST_H__
#define __SMRP_FAST_H__

#define FAST_SERIAL
#undef FAST_OLD_SERIAL

#include "../util/tree.h"
#include "interface_private.h"
#include "../atalk/atalk_private.h"
#include "../atalk/at_fast.h"

#define SMRPMCACHE_TIMEOUT (10*ONEMIN)

/*
 * SMRP fast-switching cache entry.
 *
 * The cache contains the network layer fast switching cache
 * structure and the network layer header info. 
 *
 */
typedef struct SMRPCache_ {

    ushort      mac_len;
    uchar*      mac_ptr;
    union {
	atcachetype     at;
    } mac;

    ushort      net_len;
    union {
	ddptype         at;
    } net;

} SMRPCache;

/* Define SMRPForward struct */
#include "../smrp/smrp_group.h"


/*
 * Forward declarations.
 */

extern void smrpfast_Initialize(void);
extern void smrpfast_InitPortMCacheData(SMRPPort*);
extern void smrpfast_ReinitPort(SMRPPort*);
extern void smrpfast_SetupPort(SMRPPort*);

extern SMRPCache* smrpfast_CreateCache(void);
extern void smrpfast_AddEntry(SMRPForward*);
extern void smrpfast_DeleteEntry(SMRPForward*);
extern boolean smrpfast_UpdateMCache(paktype*,SMRPForward**,SMRPChildPortLink**);
extern void smrpfast_fscache_age(void);
extern void smrpfast_fscache_flush(void);
extern boolean smrpfast_InvalidateFwdCache(treeLink*, void*);
extern void smrpfast_InvalidateAllCache(char*, SMRPPort*);

extern char* smrpfast_Reason(SMRPPort*);

extern uchar* smrpfast_DumpCacheMacHdr(SMRPCache*, uchar*);
extern uchar* smrpfast_DumpCacheNetHdr(SMRPCache*, uchar*);

extern inline treeLink* smrpfast_RBTreeSearch_inline(rbTree*, treeKey);
extern boolean smrpfast_fastswitch(paktype*);
extern boolean smrpfast_mci_fastswitch(hwidbtype*, mciregtype*, ushort, ushort);

#endif __SMRP_FAST_H__
