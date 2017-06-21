/* $Id: at_domain.h,v 3.2.60.1 1996/03/23 01:26:03 slin Exp $
 * $Source: /release/111/cvs/Xsys/atalk/at_domain.h,v $
 *------------------------------------------------------------------
 * Header for at_domain.c
 *
 * November 1994, Grisha Kotlyar
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_domain.h,v $
 * Revision 3.2.60.1  1996/03/23  01:26:03  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2  1995/11/17  08:42:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:49:38  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/21  16:33:42  kleung
 * CSCdi37111:  ATIP remap entries not cleaned up when range overflows.
 *
 * Revision 2.1  1995/06/07  20:07:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__AT_DOMAIN_H__
#define	__AT_DOMAIN_H__


typedef struct at_AvailableRemap_ {
    dqueue_t	link;
    ushort	Start;
    ushort	End;
} at_AvailableRemap;


/* Zone List here */
typedef struct Domain_ZoneList_ {
    struct Domain_ZoneList_ *next;
    uchar  name[ZONELEN];         /* Zone name, not NULL terminated */
    ushort len;                   /* Length of the zone */
} Domain_ZoneList;


/* Different statuses for at_RemapTable */
typedef enum {
    UnAssigned = 0, 
    UnZipped,
    Suspect,
    Established,
    Dead,
} RemapStatus;

              
typedef struct at_RemapTable_ {
    dqueue_t link;
    ushort domain_start;           /* Beginning of the "real" net*/
    ushort domain_end;             /* End of the "real" net */
    ushort remap_start;            /* Start of the remapped */
    ushort remap_end;              /* End of the remapped */
    uchar  dist;                   /* Distance */
    neighborentry   *neighbor;     /* Pointer to the responsible neighbor */
    RemapStatus status;            /* Status of this route */
    ulong  cluster;                /* Cluster number */
    ushort zonecnt;                /* How many zones are in the zone list */
    Domain_ZoneList  *ZoneListHdr; /* Header of the zone list */
    ushort busy;                   /* How many links to this struct */
    ushort protocol_links;         /* Protocols linked to this remap */
} at_RemapTable;

#define NOT_IN_USE      0
#define IN_USE_BY_RTMP  0x0001
#define IN_USE_BY_EIGRP 0x0002
#define IN_USE_BY_AURP  0x0004

typedef struct domain_idbList_ {
    struct domain_idbList_ *next;
    atalkidbtype *idb;
    boolean active;
} domain_idbList;


typedef enum {
    FromDomain,
    ToDomain
} domain_direction;

typedef struct at_SubDomain_ {
    int    domain_number;          /* Domain number */
    ushort flags;                  /* Flags */
    ushort remap_start;            /* Start of remap range */
    ushort remap_end;              /* End of remap range */
    at_AvailableRemap *AvRemapHdr; /* Head of the Available Remap List */
    at_AvailableRemap *AvRemapTail;/* Tail of the Available Remap List */
    at_RemapTable     *RemapListHdr;  /* Head of the Remap Table */
    at_RemapTable     *RemapListTail; /* Tail of the Remap Table */

} at_SubDomain;

/* SubDomain flags */    
#define active_r   0x0001
#define outbound_r 0x0004
#define inbound_r  0x0002

typedef struct at_Domain_ {
    dqueue_t link;
    int    number;                 /* Domain number */
    ushort namelen;                /* Length of the domain name */
    uchar  name[ZONELEN];          /* Domain name */
    boolean active;                /* Active  domain ? */
    at_SubDomain *in;              /* Sub Domain on input */
    at_SubDomain *out;             /* Sub Domain on output*/
    domain_idbList      *idbHdr;   /* List of all idbs in this domain */
    boolean cluster;               /* TRUE if clustering is ON */
    ushort  clsize;                /* Size of the cluster */
    boolean hop_rdctn;             /* TRUE if hop reduction is ON */
    boolean hop_prsrv;             /* TRUE if hop preservation is ON */
    
} at_Domain;

typedef struct GeneralDomainList_ {
    struct GeneralDomainList_ *next;
/*    idbtype               *idb; */
    at_RemapTable         *remap_ptr;
    ushort                retries;
    sys_timestamp         timer;
} GeneralDomainList;



typedef struct DomainNeighbor_ {
    struct DomainNeighbor_ *next;
    neighborentry          *neighbor;
    GeneralDomainList      *UnZippedHdr;
} DomainNeighbor;


#define LOOP_STRING "LoopBack"

ushort atdomain_FindForwardRemap(at_SubDomain *, ushort);
ushort atdomain_FindBackwardRemap(at_SubDomain *, ushort);
void atdomain_ProcessRTMP(paktype *);
void atdomain_ZipXReply(paktype *);
void atdomain_ZIPReply(paktype *pak);
void atdomain_DDPinProcess(paktype *pak);
void atdomain_DDPoutProcess(paktype *pak);
void atdomain_DeleteDomainRoute(int, ushort, ushort);

void show_appletalk_remap(parseinfo *csb);
void show_appletalk_domain(parseinfo *csb);

void atdomain_InsertNeighbor(neighborentry *);
void atdomain_DeleteDomain(int);
void atdomain_PathUpdate(atalk_rdb_t *);
void atdomain_RemapUpdate(atalk_rdb_t *);
void atdomain_Initialize(void);
void atdomain_DisablePort(atalkidbtype *);
void atdomain_InitialAdjust(atalkidbtype *);
boolean atdomain_CheckForDiscovery(paktype *);
ushort atdomain_GetRemapOut(atalkidbtype *idb,ushort net);

void atalk_show_pvt_remap(void);


at_RemapTable	*atdomain_CreateRemap(atalkidbtype *, ushort, ushort);
at_Domain	*atdomain_FindDomain_by_number(int);

boolean		atdomain_ProcessZIP(paktype *);
boolean 	atdomain_FilterNet(atalkidbtype *, ushort, domain_direction);
boolean		atdomain_NBPLoop(paktype *);
boolean		atdomain_InsideInboundRemap(ushort, ushort);
boolean		atdomain_DeleteRemap(at_SubDomain*, at_RemapTable*,boolean);
boolean		atdomain_MatchAnyAURPTunnel(atalkidbtype* idb);
boolean		atdomain_PathInAURPInbound(atalk_pdb_t* p);
boolean		atdomain_AURPTunnel(at_Domain *dm_ptr);

void		atdomain_ndb_new(atalkidbtype *, ushort, ushort);
void		atdomain_ndb_done(ushort, ushort );
void		atdomain_CheckForLoop(atalk_rdb_t *, uchar *, ulong);
void		atdomain_StartLoopDetection(at_RemapTable *);
void		atdomain_route_done(ushort, ushort);
void		atdomain_DeleteNeighbor(neighborentry *);
void		atdomain_UnLockAvCell(at_SubDomain *, ushort, ushort);
void		atdomain_route_done(ushort Start, ushort End);
void		atdomain_DeleteNeighbor(neighborentry *n);

void		atdomain_QueueEvent(atalkidbtype* idb, ushort Start,
				    ushort End);
void		atdomain_DeletePathEntry(ushort Start, ushort End);

#endif
