/* $Id: at_rtmp.h,v 3.2.60.2 1996/05/02 08:40:40 dwong Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_rtmp.h,v $
 *------------------------------------------------------------------
 * at_rtmp.h -- definitions for Appletalk RTMP protocol
 *
 * September 1991, David Edwards
 * 
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_rtmp.h,v $
 * Revision 3.2.60.2  1996/05/02  08:40:40  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.2.60.1  1996/03/23  01:27:29  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2  1995/11/17  08:43:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:57:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:50:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:09:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __AT_RTMP_H__
#define __AT_RTMP_H__



#include "../iprouting/route.h"

typedef struct rtmp_path_ {
    struct atalk_path_	rtpath;
} rtmp_path_t;

/* Structure used for calling start_aging function */
typedef struct rtmp_aging_t_ {
    atalkidbtype *idb;
    neighborentry *n;
    paktype *pak;
} rtmp_aging_t;

/*
 * AppleTalk internal interface prototypes
 */
void 		atrtmp_Initialize(void);
void 		atrtmp_StatusReport(void);
void 		atrtmp_WriteNV(char *);

void		atrtmp_SendUpdates(void);

atalk_rdb_t *   atrtmp_GetFirstRoute(void);       
atalk_rdb_t * 	atrtmp_GetNextRoute(atalk_rdb_t*);
		                                  
atalk_rdb_t * 	atrtmp_FindNextRoute(ushort);     
atalk_rdb_t * 	atrtmp_FindRoute(ushort);         
atalk_rdb_t * 	atrtmp_FindProxyRoute(ushort);    
void 		atrtmp_ReleaseRoute(atalk_rdb_t*);

void 		atrtmp_DeleteProxyRoute(ushort);
boolean 	atrtmp_AddProxyRoute(ushort,int,uchar*);
boolean 	atrtmp_IsProxyRoute(ushort net);

void 		atrtmp_ProtectRoute(atalk_rdb_t*);
void 		atrtmp_UnprotectRoute(atalk_rdb_t*);

void 		atrtmp_Ager(void);
ulong 		atrtmp_RouteCount(void);
boolean 	atrtmp_IsRouteValid(ushort,ushort);

ipaddrtype 	atrtmp_FirstHop(paktype*);

void 		atrtmp_SendUpdate(atalkidbtype *, ushort, long, uchar,
				  boolean);

void 		atrtmp_treeprint(void);
void 		atalk_RtmpRouter(void);
void		atrtmp_redist_all(boolean);
void		atrtmp_start_aging(atalkidbtype *, paktype *);
boolean		atrtmp_GetNextTuple(rtmptuple **, int *, ushort *, ushort *,
				    uchar *, uchar);
boolean		rtmp_NetIsValid(atalk_rdb_t *);
void		atrtmp_config_command(parseinfo *);
void		atrtmp_show_private(void);

void		atrtmp_CreateEntry (rbTree *, ushort, ushort,
				    uchar, atalkidbtype *, neighborentry*);
neighborentry *	rtmp_GetNeighbor (paktype *);

void
rtmp_UpdateEntry(atalk_pdb_t* p, uchar dist, atalkidbtype *idb,
		 neighborentry* n);

void            atalk_rtmp_set_timer (atalk_pdb_t *);

/* Inline routines */
extern inline void
atrtmp_SetMetric (atalk_metric_t *metric, uchar hops)
{
    metric->metric = (hops == DDP_POISON_HOPCOUNT)
	? METRIC_INACCESSIBLE
	: atalk_rtmp_bw * hops;
    metric->u.rtmp.hops = hops;
}

extern inline uchar
atrtmp_GetPathMetric (atalk_pdb_t const *p)
{
    return (p->metric.u.rtmp.hops);
}

extern inline uchar
atrtmp_GetRouteMetric (atalk_rdb_t const *r)
{
    return (r->rpath.metric.u.rtmp.hops);
}

/* Temporary routines */

#endif __AT_RTMP_H__
