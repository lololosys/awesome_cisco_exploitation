/* $Id: at_route.h,v 3.2.60.2 1996/05/02 08:40:30 dwong Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_route.h,v $
 *------------------------------------------------------------------
 * at_route.h -- definitions for Appletalk routing module.
 *
 * January 1992, David S.A. Stine
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_route.h,v $
 * Revision 3.2.60.2  1996/05/02  08:40:30  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.2.60.1  1996/03/23  01:27:23  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2  1995/11/17  08:43:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:57:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:50:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/10  12:55:25  dwong
 * CSCdi33321:  RTMP non-extended routes are converted to extended routes
 *              Skip the conversion process for the poison routes.
 *
 * Revision 2.1  1995/06/07  20:09:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __AT_ROUTE_H__
#define __AT_ROUTE_H__

/*
 * Constants
 */
extern const ushort kMaxFreeRdbNodes;
extern const ushort ATALK_MIN_REDIST;

/*
 * Typedefs
 */
typedef boolean (*atroute_leaker)(atalk_pdb_t *);
typedef	void    (*atroute_unleaker)(atalk_pdb_t *);
typedef void 	(*atroute_all_leaker)(boolean);
typedef void	(*atroute_validate)(ushort, ushort);
/*
 * Relationships of comparisons of route metrics
 */
typedef enum {
    ATALK_METRIC_EQ=1,
    ATALK_METRIC_NE,
    ATALK_METRIC_GT,
    ATALK_METRIC_GE,
    ATALK_METRIC_LT,
    ATALK_METRIC_LE
} atalk_metric_compare_t;


#define	RTMP_BANDWIDTH	(256524800U)

/*
 * pdb/rdb datastructure cast macros.
 */
#define route_Cast(_l_) (atalk_rdb_t*)(_l_)
#define path_Cast(_l_) (atalk_pdb_t*)(_l_)

/*
 * Path/Route protection macros. I didn't want to make these
 * inlines, since inlines generate a bunch of jacket code we really
 * don't need just for a couple of typecasts.
 */
#define KeepFirstPath(_t_) (atalk_pdb_t*)atutil_KeepFirstEntry(_t_)
#define KeepNextPath(_t_,_p_) (atalk_pdb_t*)atutil_KeepNextEntry(_t_,&_p_->link)
#define GetFirstPath(_t_) (atalk_pdb_t*)atutil_GetFirstEntry(_t_)
#define GetNextPath(_t_,_p_) (atalk_pdb_t*)atutil_GetNextEntry(_t_,&_p_->link)

#define KeepFirstRoute(_t_) (atalk_rdb_t*)atutil_KeepFirstEntry(_t_)
#define KeepNextRoute(_t_,_r_) (atalk_rdb_t*)atutil_KeepNextEntry(_t_,&_r_->link)
#define GetFirstRoute(_t_) (atalk_rdb_t*)atutil_GetFirstEntry(_t_)
#define GetNextRoute(_t_,_r_) (atalk_rdb_t*)atutil_GetNextEntry(_t_,&_r_->link)

#define ProtectRoute(_r_) atutil_ProtectEntry(&_r_->link)

#define ReleaseRoute(_r_) atutil_ReleaseEntry(&_r_->link)

#define RouteProtected(_r_) RBTreeNodeProtected((treeLink *)&_r_->link)

void 		atroute_Initialize(void);
void 		atroute_StatusReport(void);

void		atroute_ForAllRouteTreesDo(routeProc);
void		atroute_ClearPathsAndRoutes(atalk_rdb_t *);
atalk_rdb_t * 	atroute_FindRoute(rbTree *, ushort, boolean);
atalk_rdb_t * 	atroute_LookupRoute(rbTree *, ushort, boolean);
atalk_pdb_t *	atroute_LookupPath(rbTree *, ushort);

boolean 	atroute_AddProxyPath(ushort net,int len,uchar* buf, 
					boolean internal);
void 		atroute_DeleteProxyPath(ushort net);
boolean 	atroute_IsProxyRoute(rbTree *,ushort net);

atalk_rdb_t *	atroute_GetFirstRoute(rbTree *);
atalk_rdb_t *	atroute_GetValidRoute(rbTree *,ushort, ushort, boolean *, boolean);
void 		atroute_ProtectRoute(atalk_rdb_t*);
void 		atroute_UnprotectRoute(atalk_rdb_t*);
void 		atroute_DeleteRoute(atalk_rdb_t *);

atalk_pdb_t *	atroute_GetValidPath(rbTree *, ushort, ushort, boolean*, boolean);
atalk_pdb_t *	atroute_GetMatchingPath(rbTree *, ushort, ushort,
					atalkidbtype *, neighborentry *,
					boolean *);

boolean 	atroute_BestPath(atalk_pdb_t *);
boolean 	atroute_BestPathOnRoute(atalk_rdb_t *);
void 		atroute_UpdateRoute(atalk_rdb_t*, atalk_pdb_t*);
void 		atroute_Ager(void);
boolean		atroute_path_redistribute(atalk_pdb_t *);
boolean		atroute_path_unredistribute(atalk_pdb_t *);
boolean		atroute_path_redist_rbshell(treeLink *, void *);
ulong 		atrtmp_RouteCount(void);
boolean 	atrtmp_IsRouteValid(ushort,ushort);

ataddrtype 	atroute_FirstHop(paktype*);

rbTree *	atroute_GetTree(void);
rbTree *	atroute_GetPTree(ushort);

void 		atroute_InitPathTree(ushort, char *);
void 		atroute_DeletePathTree(ushort);
void 		atroute_ReplacePath(atalk_pdb_t *, atalk_metric_t *,
				    idbtype *, neighborentry *);
void 		atroute_SetPathState(atalk_pdb_t *, at_state_t);
void 		atroute_DeletePath(atalk_pdb_t *);
void 		atroute_ClearAllPaths(atalk_rdb_t *);

short           atroute_GetEqualPathCount(atalk_pdb_t *);
void            atroute_ClearEqualPathCount(atalk_pdb_t *);
void            atroute_UpdateEqualPathCount(atalk_pdb_t *, boolean);
void            atroute_UpdateEqualPath(atalk_pdb_t *, idbtype *, neighborentry *);
void            atroute_DeleteEqualPath(atalk_pdb_t *, idbtype *);
void            atroute_DeleteEqualPathList(atalk_pdb_t *);
boolean         atroute_AddEqualPath(atalk_pdb_t *, idbtype *, ushort, ushort,
 				     neighborentry *, ushort, atalk_metric_t *);

atalk_pdb_t *	atroute_AddPath(rbTree *, atalkidbtype *, ushort, ushort,
				neighborentry *,
				boolean (*path_action)(atalk_pdb_t *,
							at_state_t),
				boolean (*path_use)(atalk_pdb_t *,
						    at_path_use_t),
				ulong, ushort, atalk_metric_t *);
ulong 		atroute_RouteCount(void);
atalk_pdb_t *	atalkif_GetPdbList(idbtype *);
void 		atroute_MapMetric(atalk_metric_t *, atalk_metric_t *);
boolean 	atroute_PathIsPoisoned(atalk_pdb_t *);
char const *	atroute_rp_type(at_path_t);
boolean 	atroute_IsRouteValid(rbTree *,ushort, ushort);
void 		atroute_printtrees(void);
void 		atroute_WriteNV(char *);
atalk_rdb_t *	atroute_GetRouteFromPath(atalk_pdb_t *);
char		atroute_TypeInitial(atalk_rdb_t *);
char		atroute_PathState(atalk_pdb_t *);
atalk_rdb_t *	atroute_GetIdbRoute(atalkidbtype *);
void		atroute_pathGC(at_path_t, char const *);
boolean		atroute_CleanUpRoute(void);
void		atroute_RoutePoisonSent(atalk_rdb_t *);
idbtype *       atroute_SwitchEqualPath(atalk_rdb_t *, ataddrtype *);
void		atroute_NewPathMetric(atalk_rdb_t *, atalk_pdb_t *);
boolean		atroute_DefaultPathAction(atalk_pdb_t *, at_state_t);
boolean 	atroute_MetricCompare(atalk_metric_t *, atalk_metric_t *,
				      atalk_metric_compare_t);
void		atroute_RegisterProtocol(at_path_t, ulong, ulong, ulong,
					 atroute_leaker, atroute_unleaker,
					 atroute_all_leaker,
					 atroute_validate);
void		atroute_UnregisterProtocol(at_path_t);
void		atroute_clear_redisted_paths(void);
void		atroute_redist_all(boolean);
void		atroute_validate_all(ushort, ushort);

#endif	__AT_ROUTE_H__
