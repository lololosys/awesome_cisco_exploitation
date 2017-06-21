/* $Id: at_util.h,v 3.4.6.2 1996/06/29 00:07:15 thille Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_util.h,v $
 *------------------------------------------------------------------
 * at_util.h -- definitions for Appletalk utility module
 *
 * September 1991, David Edwards
 * 
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_util.h,v $
 * Revision 3.4.6.2  1996/06/29  00:07:15  thille
 * CSCdi61558:  Trim a little fat in the appletalk text strings
 * Clean up some literal strings and get back 580 bytes.
 *
 * Revision 3.4.6.1  1996/03/23  01:27:57  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.4  1996/03/05  22:13:48  kleung
 * CSCdi49904:  atalk_upper vs atalk_cvu are equivalent
 *
 * Revision 3.3  1995/11/17  08:44:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:06:14  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:58:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:50:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/05  22:35:17  kleung
 * CSCdi39766:  Clean up atalk_print_network_header().
 *
 * Revision 2.2  1995/06/14  22:51:05  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Change SMRP socket from 65 to 10.
 *
 * Revision 2.1  1995/06/07  20:09:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __AT_UTIL_H__
#define __AT_UTIL_H__

#ifndef	__ATALK_PRIVATE_H__
#include "../atalk/atalk_private.h"
#endif

/*
 * Search parameters to 'util_FirstZipnet' and 'util_NextZipnet'
 */
#define AT_UTIL_ZIPNET_ACCEPT_ANY 0
#define AT_UTIL_ZIPNET_NO_DELETED 1

/*
 * AppleTalk private prototypes
 */
boolean		atutil_RangesOverlap(ushort, ushort, ushort, ushort);
void 		atalkif_ClearLocalConfig(atalkidbtype* idb);           
void 		atutil_Initialize(void);                          
void 		atutil_StatusReport(void);                        
boolean 	atalkif_InterfaceValid(atalkidbtype*);
atalk_pdb_t *	atalkif_CreateConnectedPath(atalkidbtype *);
void 		atalkif_ClearNames(atalkidbtype*);
boolean 	atalkif_HalfRouter(atalkidbtype*);
boolean 	atalkif_InterfaceConfigured(atalkidbtype*);
ataddrtype 	atalkif_CableBroadcast(atalkidbtype*);
char * 		atalkif_netname(atalkidbtype*);
char * 		atalkif_nodename(atalkidbtype*);
boolean		atalkif_BroadcastForMe(atalkidbtype*,ataddrtype);
void 		atalk_clear_zones(atalk_rdb_t *);
void 		atalkif_clear_zone(atalkidbtype *,ziptabletype *);
void 		atalk_PrintActiveSockets(void);
zipifentry * 	atalkif_findZone(atalkidbtype* idb,ziptabletype* zone);
zipnetentry * 	atalk_firstZone(atalk_rdb_t*);
int		atalk_CountAndFilterZones(atalk_rdb_t *, int);
void 		atalk_PrintSocket(sockTypes);
void 		atalk_bugreport(int*,char*);
void 		atalk_debugmsg(char*);
void 		atalk_assert(char*,int);
void 		at_err(char *file, int line, int debugsw, char *msgin, ...);
void 		atalk_netname(ushort start, ushort end, char*);
zipnetentry * 	atalkzn_netname(zipnetentry *, char *);
zipnetentry * 	atalkzn_nextNet(zipnetentry *);
zipnetentry * 	atalk_firstNet(ziptabletype *);
void 		atalk_ddpname(ddpTypes,char*);                            
char* 		at_sockname(int);                            
treeLink *	atutil_KeepFirstEntry(rbTree *);
treeLink *	atutil_KeepNextEntry(rbTree *, treeLink*);
void		atutil_ReleaseEntry(treeLink*);
treeLink *	atutil_GetFirstEntry(rbTree *);
treeLink *	atutil_GetNextEntry(rbTree *, treeLink*);
void		atutil_ProtectEntry(treeLink *);
void		atutil_UnprotectEntry(treeLink *);
atalk_pdb_t *	atalkif_GetIdbPath(atalkidbtype* idb);
char const *    atutil_statusString(opStatus);
int             at_printf_littlea(const char **, char **, tt_soc *, va_list *,
				  boolean, int, int, int, int);
int             at_printf_biga(const char **, char **, tt_soc *, va_list *,
			       boolean, int, int, int, int);

/*******************************************************************
 * Packet Routines
 *******************************************************************/
paktype * 	atalk_pak_duplicate(paktype *);
paktype * 	atalk_pak_reply(paktype *pak, ddpTypes pt, ulong size);
paktype * 	atalk_pak_create(atalkidbtype *, ataddrtype, sockTypes,
				 sockTypes, ddpTypes, ulong);
boolean		atalk_pak_newInit(paktype *, atalkidbtype *, ulong,
				  ushort, uchar, uchar, uchar, uchar);
void 		atalk_pak_hearself(paktype*);                           
void 		atalk_pak_setbroadcast(paktype*);                       
void 		atalk_pak_setzonemcast(paktype*,ushort);                
void		at_pak_dumphdr(paktype *, idbtype *, char *);
void 		at_pak_printIhdr(paktype *, atalkidbtype *, boolean);
void 		at_pak_printOhdr(paktype *, atalkidbtype *, boolean);
void 		atalk_pak_printbad(paktype*,char*, atalkidbtype*);            
void 		atalk_pak_printgood(paktype*,char*, atalkidbtype*);           
void 		atalk_pak_printforus(paktype*,int);                     
void 		atalk_pak_printprocess(paktype*,atalkdebugtype *,char*);
void 		atalk_pak_printfail(paktype*,atalkdebugtype *,char*);   
void 		atalk_pak_setdnet(paktype*,ushort);
void 		atalk_pak_setsnet(paktype*,ushort);
ushort 		atalk_pak_hops(paktype*);
void 		atalk_pak_sethops(paktype*,ushort);
void 		atalk_pak_setdnode(paktype*,uchar);
void 		atalk_pak_setsnode(paktype*,uchar);
sockTypes 	atalk_pak_ssock(paktype*);
sockTypes 	atalk_pak_dsock(paktype*);
void 		atalk_pak_setssock(paktype*,sockTypes);
void 		atalk_pak_setdsock(paktype*,sockTypes);
void 		atalk_pak_setlength(paktype*,ulong);
ushort 		atalk_pak_cksum(paktype*);
boolean 	atalk_pak_needscksum(paktype*);
void 		atalk_pak_setcksum(paktype*,ushort);
void 		atalk_pak_inithdrptr(paktype*);
int 		atalk_pak_initdataptr(paktype*);

boolean 	atalk_ValidNodeAddress(ataddrtype);
ddpTypes	atalk_ValidPacket(paktype *, ushort, boolean);
atalk_rdb_t * 	atalkif_GetRdbEntry(idbtype*);
netTypes 	atalkif_srcnettype(atalkidbtype*,ushort);
void 		atalk_add_zonelist(atalk_rdb_t* net,zipifentry*);
void 		atalk_FreeZoneEntries(atalk_rdb_t*);
void 		atalkif_clear_route_zones(atalkidbtype*);
atalk_rdb_t *	atalkif_GetRouteEntry(idbtype*);
void 		atalkif_clear_zones(atalkidbtype*);
boolean 	atalkif_set_primary(atalkidbtype* idb,ziptabletype*);
boolean 	atalkif_addZone(atalkidbtype* idb,ziptabletype*);
zipnetentry * 	atalkzn_nextZone(zipnetentry*);
zipnetentry * 	atalk_only_zone(atalk_rdb_t*);
zipnetentry * 	atalk_join_zipnet(atalk_rdb_t*,ziptabletype*);
zipnetentry * 	atalk_find_and_use_zipnet(atalk_rdb_t*,ziptabletype*);
netTypes 	atalkif_dstnettype(atalkidbtype*,ushort);
void 		atalkif_SetOperational(atalkidbtype*);
char * 		atalkif_ShortStatus(atalkidbtype*); 
char * 		atalkif_opstatusmsg(atalkidbtype*); 
char * 		atalkif_addrstatemsg(atalkidbtype*);
char * 		atalk_state_name(at_state_t);  
boolean 	atalkif_IsMyAddress(atalkidbtype*,ataddrtype);
boolean 	atalkif_static_addresses(atalkidbtype*);
boolean 	atalkif_hasSingleZone(atalkidbtype*);
void 		atalkif_SetOurZone(atalkidbtype*,ziptabletype*);
void 		atalkif_SetOurMulticasts(atalkidbtype*, uchar const *);
void 		atalkif_InstallZones(atalkidbtype*);
void 		atalkif_AcquireZones(atalkidbtype*);
boolean 	atalkif_CheckZones(atalkidbtype*);
uchar 		atalk_upper(uchar c);
int 		atalk_hex_filter(char *src, char *dst, int dlen);
char *		at_hex_display(char *str);
boolean 	atalkif_unregister(atalkidbtype *idb);
void 		atalkif_ClearOurMulticasts(atalkidbtype*);
void 		atalkif_ResetMulticasts(atalkidbtype*);
watched_queue * atalk_SocketQueue(uchar sock);
boolean 	atalk_BindSocket(watched_queue *, char *, uchar, sockfunc_t);
void 		atalk_UnBindSocket(uchar sock);
boolean 	atalk_IsBoundSocket(sockTypes sock);
sockTypes	atalk_SelectSocket(void);
boolean		atalk_SocketPreQueue(uchar, paktype *);
void            atalk_SocketMarkInput(uchar sock);
boolean 	atalkif_ValidFQAddress(atalkidbtype* idb,ulong addr);
boolean 	atalkif_ValidAddress(atalkidbtype*,ataddrtype);
void 		appletalk_zonename(uchar*,uint,char*);
void		atalk_nv_printzone(char *);
ziptabletype * 	atalkif_primary_zone(atalkidbtype*);
char *		atalk_PathType(atalk_pdb_t*);
boolean		atalk_path_internal(atalk_pdb_t *);
atalkidbtype * 	atalk_findIDB(ushort);
ulong 		atalk_tr_broadcasts(atalkidbtype*);
ushort 		rotate_short(ushort);
uint 		atalk_zonesum(uchar*,int);
int 		atalk_random(int);
boolean		atalk_validSrcAddress(atalkidbtype *, ataddrtype);

/*******************************************************************
 * listItem Routines
 *******************************************************************/
boolean 	ListItemActive(listItem* i);
void 		InitListItem(listItem* i);
void 		ProtectListItem(listItem* i);
void * 		ListItemData(listItem* i);
void 		SetListItemData(listItem*,void*);
boolean 	UnprotectListItem(listItem* i);
void 		UnlinkListItem(listItem* i,listItem** r);
void 		DeleteListItem(listItem* i);
listItem * 	GetNextItem(listItem*);
listItem * 	SListSearch(ulong key,listItem* first);
listItem * 	FindNextItem(listItem*,listProc);
boolean 	ListItemDeleted(listItem* i);
void 		InsertListItem(ulong,listItem*,listItem**);

char *		atalk_statStr(int nn);
boolean 	atalk_on_idb(idbtype *idb);

static inline boolean atutil_NodeIsBroadcast(uchar node) {
    return ((node == ATALK_BROAD_ADDR));
}
static inline opStatus 
atalkif_opstatus(atalkidbtype* idb) {
    return (opStatus) idb->atalk_status;
}

static inline ataddrtype 
atalk_CableRange (atalk_rdb_t* r) {
    return (r->dstrngstr << 16) | (r->dstrngend); 
}

static inline ataddrtype 
atalkif_CableRange (atalkidbtype* idb) {
    return (idb->atalk_cablestart << 16) | (idb->atalk_cableend); 
}

static inline ATAddress 
atutil_DestFQA (paktype* pak) { 
    return *((ATAddress*)&pak->atalk_dstfqa); 
}

static inline ATAddress 
atutil_SrcFQA (paktype* pak) { 
    return *((ATAddress*)&pak->atalk_srcfqa); 
}

static inline ddpTypes
atalk_pak_getpt (paktype* pak)
{
    ddptype* ddp = (ddptype*) pak->network_start;
    if (pak->linktype == LINK_APPLETALK)
	return (ddpTypes) (ddp->l).ddppt;
    return (ddpTypes) ddp->s.ddppt;
}

static inline ataddrtype 
atalk_address (ushort net,uchar node) { 
    return (((ataddrtype)(net & 0xffff) << 8)|(node & 0xff)); 
}

static inline ataddrtype 
atutil_DestAddr (paktype* pak) { 
    ATAddress a=atutil_DestFQA(pak); 
    return (atalk_address(a.f.net,a.f.node)); 
}

static inline ataddrtype 
atutil_SrcAddr (paktype* pak) { 
    ATAddress a=atutil_SrcFQA(pak); 
    return (atalk_address(a.f.net,a.f.node));
}

static inline ataddrtype 
atutil_CableRange (ushort dstrngstr, ushort dstrngend) {
    return (dstrngstr << 16) | (dstrngend); 
}

static inline ushort 
atalkif_MyNet (atalkidbtype* idb) {
    return idb->atalknet;
}

static inline uchar 
atalkif_MyNode (atalkidbtype* idb) {
    return idb->atalknode;
}

static inline ataddrtype 
atalkif_MyAddress (atalkidbtype* idb) {
    return atalk_address(idb->atalknet, idb->atalknode);
}

static inline ulong 
atutil_SetPakAddress (ushort net,uchar node,uchar sock) {
    ATAddress fq;
    fq.f.net = net; fq.f.node = node; fq.f.spec = sock;
    return (fq.l);
}

static inline boolean 
atalk_net_extended (atalk_rdb_t* net) { 
    return (net->dstrngend != 0); 
}

/*
 * atalk_net_connected
 * Consistent checks for a directly connected network (or a proxy).
 * Both types of routes will have installed paths which have composite
 * metrics of 1.
 */
static inline boolean
atalk_net_connected (atalk_rdb_t* net)
{
    if (net == NULL)
	return (FALSE);
    if (net->rpath.metric.metric <= AT_METRIC_PROXY)
	return (TRUE);
   return (FALSE);
}

/*
 * atalk_path_proxy
 * Consistent checks for a proxy path.
 */
static inline boolean
atalk_path_proxy (atalk_pdb_t* p)
{
    if (p == NULL)
	return (FALSE);
    if ((p->metric.metric == AT_METRIC_PROXY))
	return (TRUE);
    return (FALSE);
}
/*
 * atalk_path_proxy
 * Consistent checks for a proxy path.
 */
static inline boolean
atalk_net_proxy (atalk_rdb_t* r)
{
    if (r == NULL)
	return (FALSE);
    if ((r->rpath.metric.metric == AT_METRIC_PROXY))
	return (TRUE);
    return (FALSE);
}

static inline ushort
atalk_pak_rawlength (paktype* pak)
{
    uchar* ddp = pak->network_start;
    /* First 2 bytes of DDP header is length field */
    return (GETSHORT(ddp) & DDPLENGTHMASK);
}

static inline ushort 
atalk_pak_dnet (paktype* pak) {
    return atutil_DestFQA(pak).f.net; 
}

static inline ushort 
atalk_pak_snet (paktype* pak) {
    return (atutil_SrcFQA(pak).f.net);
}

static inline uchar 
atalk_pak_dnode (paktype* pak) {
    return (atutil_DestFQA(pak).f.node);
}

static inline uchar 
atalk_pak_snode (paktype* pak) {
    return (atutil_SrcFQA(pak).f.node);
}

static inline boolean 
atalkif_operational (atalkidbtype *idb) {
    return (interface_up(idb->phys_idb) &&
	    idb->aarp_tentative == AARP_ADDR_SET &&
	    idb->atalk_enabled && atalk_running && idb->atalk_operational);
}

extern inline void
atalkif_SetIdbPath (atalkidbtype *idb, atalk_pdb_t *p) {
    idb->atalk_path = p;
}

extern char cisco_NBP_type[];
extern const uchar apple_ether_multicast[];

#endif __AT_UTIL_H__
