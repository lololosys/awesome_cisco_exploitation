/* $Id: at_domain.c,v 3.7.6.5 1996/08/28 12:36:21 thille Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_domain.c,v $
 *------------------------------------------------------------------
 * AppleTalk Internetworking.
 *
 * November 1994, Grisha Kotlyar
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_domain.c,v $
 * Revision 3.7.6.5  1996/08/28  12:36:21  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.6.4  1996/07/10  22:17:51  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.7.6.3  1996/06/28  00:06:15  kleung
 * CSCdi60845:  Crash in idb_get_swsb() called from atalk_check().
 * Branch: California_branch
 *
 * Revision 3.7.6.2  1996/03/23  01:26:00  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.7.6.1  1996/03/18  18:51:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.3  1996/03/16  06:26:31  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.2.2  1996/03/07  08:28:28  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.2.1  1996/02/20  00:16:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/03/04  23:27:10  kleung
 * CSCdi50023:  Update show appletalk commands to handle VIP interfaces.
 *
 * Revision 3.6  1996/02/08  03:18:32  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.5  1996/02/07  16:09:37  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/02/01  05:58:55  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/18  19:39:11  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  08:42:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  20:49:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/07/21  01:22:22  kleung
 * CSCdi37111:  ATIP remap entries not cleaned up when range overflows.
 *
 * Revision 2.5  1995/07/13  06:49:29  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.4  1995/06/10 12:55:11  dwong
 * CSCdi33321:  RTMP non-extended routes are converted to extended routes
 *              Skip the conversion process for the poison routes.
 *
 * Revision 2.3  1995/06/09  21:28:45  kleung
 * CSCdi35461:  AppleTalk domain does not process the first RTMP tuple
 * correctly.
 *
 * Revision 2.2  1995/06/09  19:31:37  kleung
 * CSCdi35461:  AppleTalk domain does not process the first RTMP tuple
 * correctly.
 *
 * Revision 2.1  1995/06/07  20:07:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include"logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include <ciscolib.h>
#include "mgd_timers.h"
#include "registry.h"
#include "subsys.h"
#include <interface.h>
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_atalk.h"
#include "atalk_private.h"
#include "at_registry.h"
#include "at_domain_debug.h"
#include "at_static.h"
#include "at_domain.h"

enum {
    ATTIMER_DOMAIN_ZONE,
    ATTIMER_DOMAIN_LOOP,
    ATTIMER_DOMAIN_GC
};

#define AT_DOMAIN 0


#define ZIP_QUERY_INTERVAL        ONESEC
#define DOMAIN_MAX_ZIP_RETRIES    10
#define Protected(e) e->busy ? TRUE : FALSE

#define LOOP_MIN_DATA_SIZE 27
#define LOOP_NAME_SIZE     8
#define LOOP_MAX_RETRIES   3

#define LOOP_NBP_ID        99

/*
 *
 * Externs
 */
extern neighborentry *maint_GetNextNeighbor(neighborentry *);

/*
 * Internal structures 
 */


/* Temporary route list */
typedef struct TempRouteList_ {
    struct TempRouteList_ *next;
    ushort Start;
    ushort End;
    uchar dist;
    neighborentry *neighbor;
} TempRouteList;

/* 
 * Private declarations
 */
static void atdomain_remap_parse(parseinfo *);
static void atdomain_parse_no_remap(int, ushort, ushort, int);
static void atdomain_parse_no_domain(int, uchar *, ushort);
static void atdomain_group_parse(parseinfo *);
static void atdomain_parse(parseinfo *);

static void atdomain_showDomainRemap(at_Domain *);
static void ShowDomain(at_Domain *);
static void atdomain_DeleteIDB_fromDomain(atalkidbtype *);
static at_Domain *FindDomain_by_idb(atalkidbtype *);
static at_RemapTable *atdomain_RemapProcess(at_SubDomain *, ushort, ushort,
					    uchar, neighborentry *);

static at_Domain *Domain_of_Net(ushort);
static void UpdateDomain(at_Domain *);
static void Domain_ShutDown(at_Domain *);
static void ShowRemap(at_Domain *, int, parseinfo *);
static void showSubDomainAv(at_SubDomain *);
static void LoopInspection(at_RemapTable *);
static boolean LoopSuspect(ushort, ushort, uchar *, ulong, int);
static void StartLoopDetection(at_RemapTable *);
static void SendLoopPackets(GeneralDomainList *);
static void SendLoop(atalkidbtype *, uchar *, uint);

static void AddToTempRouteList(ushort, ushort, uchar, neighborentry *);
static void Add_to_UnZippedList(DomainNeighbor *, at_RemapTable *);
static ushort at_ReshuffleRemapList (at_SubDomain *, ushort);

static void at_PutAvRemapCell(at_SubDomain *, ushort, ushort);
static void at_DeleteAvRemapCell(at_SubDomain *, at_AvailableRemap *);

static at_RemapTable *at_FindRemap(at_SubDomain *, ushort, ushort);
static boolean at_FindAvRemapCell (at_SubDomain *, ushort, ushort);

static GeneralDomainList *at_FindGeneralDomainElement(GeneralDomainList *d_ptr, ushort net);
static dqueue_t *at_insque(dqueue_t *, dqueue_t *);
static dqueue_t *at_remque(dqueue_t *);
static boolean at_RemapCellSanityCheck(at_SubDomain *, ushort, ushort);
static at_RemapTable *Remap_PutNewEntry(at_SubDomain *, ushort, ushort, uchar,
					neighborentry *, ushort);
static ushort GetAvailRemap(at_SubDomain *, ushort);
static boolean ConnectedNet(ushort net);
static boolean LegitNet(ushort net);
static void atdomain_RemapNBP(paktype *, int);
static void DeleteTempList(void);
static void Domain_AddNewZone(at_RemapTable *r_ptr, uchar *zone, uint len);
static void Domain_FinishZipMerge(idbtype *idb, at_RemapTable *remap_ptr);
static void Domain_AddRemapRoute(at_RemapTable *remap_ptr);
static void Domain_AddRemapZone(at_RemapTable *remap_ptr);
static void  domain_process(void);
static void  domain_start(void);
static void Domain_Loop_Process(void);
static void Domain_GC(void);
static void Delete_DeadRemaps(at_SubDomain *sd);
static Domain_ZoneList *Find_DomainZone(at_RemapTable *remap_ptr, uchar *zone, uint len);
static void ClearRemapZones(at_RemapTable *remap_ptr);
static void RemoveUnZippedEntry(at_RemapTable *remap_ptr);
static GeneralDomainList *GetNextUnqueried(DomainNeighbor *n_ptr);
static void Domain_SendQueries(void);
static int domain_SendRTMPNeighborQueries (DomainNeighbor *n_ptr);
static DomainNeighbor *GetDomainNeighbor(neighborentry *n);
static void ProtectRemap(at_RemapTable *remap_ptr);
static void UnProtectRemap(at_RemapTable *remap_ptr);
static void at_domain_nv_write(void);
static void DeleteAvRemapList(at_SubDomain *sd);
static boolean DeleteRemapTable(at_SubDomain *sd);
static void DeleteAllSubDomains(at_Domain *dm_ptr);
static void Collect_DomainNeighbors(atalkidbtype *idb);
static void Remap_GotoState(at_RemapTable *remap_ptr, RemapStatus State);
static void Delete_idbs_fromDomain(at_Domain *dm_ptr);
static at_RemapTable *KeepFirstRemap(at_SubDomain *sd);
static at_RemapTable *KeepNextRemap(at_SubDomain *sd, at_RemapTable *prev);
/*static boolean Domain_Has_idb(at_Domain *dm_ptr, idbtype *idb);*/
static void PutNewIdb(at_Domain *dm_ptr, atalkidbtype *idb, boolean operational);
static boolean Validate_Range(ushort Start, ushort End, int dir);
static void DeleteSubDomain(at_SubDomain *sd);
static void CleanSubDomain(at_SubDomain *sd);
static void hop_preservation_parse(parseinfo *csb);
static void hop_reduction_parse(parseinfo *csb);
static boolean atdomain_NetSanityCheck(atalkidbtype *idb, ushort net);
static void atdomain_CreateAvList(at_SubDomain *sd);
static void Domain_ZIP_GC(void);
static void CreateOUTRemaps(atalk_rdb_t *r);
static boolean ActiveDomain(at_Domain *dm_ptr);
static void CleanUpDomain(at_Domain *dm_ptr);
static void CreateAvRemap(at_SubDomain *sd, ushort Start, ushort End);
/* kleung - TESTING
static void LockAvCell(at_SubDomain *sd, ushort Start, ushort End);
 */
static char*  PrintRemapStatus(RemapStatus r_status);
static void at_LoopForDomain(int domain);
static void InsertDomainRoute(at_RemapTable *remap_ptr);
static void DeleteGeneralDomainList(GeneralDomainList *hdr);
static void ReleaseRemap(at_RemapTable *remap_ptr);
static void atdomain_PurgeAllDomainRoutes(int domain);
static void CheckForDomainRouter(void);

static void atdomain_init(subsystype *);
void atdomain_parser_init(void);
void atdomain_debug_init(void);


/************************************************* 
 * globals 
 */
static pid_t         domain_pid;
static mgd_timer     domain_timer;
static mgd_timer     domain_ZoneTimer;    /* Next time to send zone requests */
static ulong         domain_ZoneInterval; /* Domain ZIP interval */

static mgd_timer     domain_LoopTimer;    /* Next time to send a Loop Packet */
static ulong         domain_LoopInterval; /* Domain Loop interval */
static mgd_timer     domain_GCTimer;      /* Next time to do Garbage Collection */
static ulong         domain_GCInterval;   /* Domain GC interval */


static GeneralDomainList 
                     *LoopListHdr;     /* Header of the Loop List */

static int            LoopId;           /* Loop Id for this domain process */

/* Last ZIP queried net, if == 0 then start with the beginning */ 
static ushort last_queried_net;

static TempRouteList TempHdr;          /* Header of the TempRouteList */

static DomainNeighbor *DomainNeighborHdr; /* Header of the DomainNeighbor List*/

static int            NumOfUnzippedNets;  /* How many Unzipped nets we have */

static at_Domain     *DomainListHdr;

static at_Domain     *DomainListTail;



#define ATDOMAIN_MAJVERSION	1
#define ATDOMAIN_MINVERSION	0
#define ATDOMAIN_EDITVERSION	1

SUBSYS_HEADER(atdomain, ATDOMAIN_MAJVERSION, ATDOMAIN_MINVERSION, 
	      ATDOMAIN_EDITVERSION,
	      atdomain_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: atalk, eigrp", "req: atalk");


/*******************************************/
/* OK, here is where all the fun begins... */

static void
atdomain_init (subsystype *subsys)
{

    atdomain_parser_init();
    atdomain_debug_init();
    
    mgd_timer_init_parent(&domain_timer, NULL);
    mgd_timer_init_leaf(&domain_ZoneTimer, &domain_timer, ATTIMER_DOMAIN_ZONE,
			NULL, FALSE);
    mgd_timer_init_leaf(&domain_LoopTimer, &domain_timer, ATTIMER_DOMAIN_LOOP,
			NULL, FALSE);
    mgd_timer_init_leaf(&domain_GCTimer, &domain_timer, ATTIMER_DOMAIN_GC,
			NULL, FALSE);
}

/****************************************************************** 
 * Routine :      atdomain_parse(csb)
 *
 * Purpose :      Parser for '<no> appl domain <number> name <name>'
 *
 * Called from :  atalk_command(at_parse.c)
 */

void atdomain_parse (parseinfo *csb)
{
    uchar name[100];
    ushort namelen;
    int   new_number;
    at_Domain *new_dm, *dm_ptr;
    
    if (csb->nvgen) {
	at_domain_nv_write();
	return;
    }
    
    /* Kick off domain process if it is the first time */
    domain_start();

    switch(GETOBJ(int,2)) {
      case ATALK_DOMAIN_REMAP:
	atdomain_remap_parse(csb);
	return;

      case ATALK_DOMAIN_HOP_REDUCTION:
	hop_reduction_parse(csb);
	return;

      case ATALK_DOMAIN_HOP_PRESERVE:
	hop_preservation_parse(csb);
	return;

      case ATALK_DOMAIN_NAME:
	break;

      default:
	printf("\n%% Unknown Domain command");
	return;


    }
	
    /* Put new domain here */
    new_number = GETOBJ(int,1);

    namelen = atalk_hex_filter(GETOBJ(string,1), (char *)name, 100);
    if (namelen == 0) {
	printf("\n%% Domain name should be specified");
	return;
    }
    if (namelen > 32) {
	printf("\n%% Domain Name can not exceed 32 characters.");
	return;
    }
    if (!csb->sense) {
	atdomain_parse_no_domain(new_number, name, namelen);
	return;
    }

    dm_ptr = atdomain_FindDomain_by_number(new_number);

    if (dm_ptr) {
	strcpy(dm_ptr->name,name);
	dm_ptr->namelen = namelen;
        return;
    }
    
    dm_ptr = DomainListHdr;
    while (dm_ptr) {
	if (!strncasecmp(dm_ptr->name,name,namelen)) {
	    printf("\n%Name %s is used by AppleTalk Domain %d. Select a different name",
		   dm_ptr->name, dm_ptr->number);
	    return;
	}
	dm_ptr = (at_Domain *)dm_ptr->link.flink;
    }
	    
    
    new_dm = malloc(sizeof(at_Domain));
    if (!new_dm) {
	errmsg(AT_ERR_NOMEM, "AT new domain", __LINE__, __FILE__);
	return;
    }
    new_dm->link.flink = NULL;
    new_dm->link.blink = NULL;
    strcpy(new_dm->name, name);
    new_dm->namelen = namelen;
    new_dm->number  = new_number;
    new_dm->in      = NULL;
    new_dm->out     = NULL;
    new_dm->idbHdr  = NULL;
    new_dm->cluster = FALSE;
    new_dm->clsize  = 0;
    new_dm->hop_rdctn   = FALSE;
    new_dm->hop_prsrv   = FALSE;
    new_dm->active  = FALSE;

    if (DomainListHdr == NULL) {
	DomainListHdr = new_dm;
	DomainListTail= new_dm;
	atalk_DomainRouter = TRUE;
    } else {
	at_insque(&new_dm->link, &DomainListTail->link);
	DomainListTail = new_dm;
    }
    if (at_debug(atalkdomain_debug, ATALK_NULL_MASK))
	buginf("\nAT-DOMAIN: Created new domain %d [%s]",
	       new_dm->number, new_dm->name);
}

/*********************************************************** 
 * Routine :      atdomain_remap_parse(void)
 *
 * Purpose :      Parser for 'apple domain remap...' command
 *
 * Called from :  atdomain_parse()
 */

static void
atdomain_remap_parse (parseinfo *csb)
{

    int domain_number;
    at_Domain   *dm_ptr;
    ushort      remap_start;
    ushort      remap_end;
    int         direction;
    at_SubDomain *sub_ptr;


    domain_number = GETOBJ(int,1);
    remap_start   = GETOBJ(int,3);
    remap_end     = GETOBJ(int,4);
    direction     = GETOBJ(int,5);

    if (!csb->sense) {
	atdomain_parse_no_remap(domain_number, remap_start,
				remap_end, direction);
	return;
    }

    if (remap_start == 0 || remap_end == 0) {
	printf("\n%% No AppleTalk range specified");
	return;
    }

    dm_ptr = atdomain_FindDomain_by_number(domain_number);

    if (!dm_ptr) {
	printf("\n%% Domain %d not found ", domain_number);
	return;
    }

    if (direction == IN) {
	if (dm_ptr->in) {
	    if (dm_ptr->in->remap_start != remap_start ||
	       dm_ptr->in->remap_end   != remap_end) {

		printf("\n%% Inbound remap already exists.");
		printf("\n%% It has to be deleted first",
		       dm_ptr->number);
	    }
	    return;
	}
    } else if (direction == OUT) {
	if (dm_ptr->out) {
	    if (dm_ptr->out->remap_start != remap_start ||
	       dm_ptr->out->remap_end   != remap_end) {
		printf("\n%% Outbound remap already exists.");
		printf("\n%% It has to be deleted first",
		       dm_ptr->number);
	    }
	    return;
	}
    }

    if (!Validate_Range(remap_start,remap_end, direction))
        return;
    
    /* Create new SubDomain */
    sub_ptr = malloc(sizeof(at_SubDomain));
    if (!sub_ptr) {
	errmsg(AT_ERR_NOMEM, "AT new subdomain", __LINE__, __FILE__);
	return;
    }
    sub_ptr->RemapListHdr = NULL;
    sub_ptr->RemapListTail = NULL;
    sub_ptr->AvRemapHdr   = NULL;
    sub_ptr->AvRemapTail  = NULL;
    sub_ptr->remap_start = remap_start;
    sub_ptr->remap_end   = remap_end;
    sub_ptr->domain_number = dm_ptr->number;
    if (direction == IN)
	sub_ptr->flags = inbound_r;
    else if (direction == OUT)
	sub_ptr->flags = outbound_r;

    /* Now create Available Remap list */
    atdomain_CreateAvList(sub_ptr);

    if (direction == IN) 
	dm_ptr->in = sub_ptr;
    if (direction == OUT) 
	dm_ptr->out = sub_ptr;

    /* Whenever we create a new subdomain, 
     * we have to update the papa-domain
     */
    UpdateDomain(dm_ptr);
}

/************************************************************* 
 * Routine :      atdomain_parse_no_remap(void)
 *
 * Purpose :      Handles 'no appl domain remap...' command
 *
 * Called from :  atdomain_remap_parse()
 */

static void
atdomain_parse_no_remap (int domain_number, ushort remap_start,
			 ushort remap_end, int direction)
{
    at_Domain *dm_ptr;

    dm_ptr = atdomain_FindDomain_by_number(domain_number);
    if (!dm_ptr)
	return;

    if (direction == IN) {
	if (!dm_ptr->in)
	    return;
	DeleteSubDomain(dm_ptr->in);
	dm_ptr->in = NULL;
    } else if (direction == OUT) {
       if (!dm_ptr->out)
	   return;
       DeleteSubDomain(dm_ptr->out);
       dm_ptr->out = NULL;
    }
    UpdateDomain(dm_ptr);
}

/****************************************************************** 
 * Routine :      atdomain_parse_no_domain(csb)
 *
 * Purpose :      Parser for '<no> appl domain <number> name <name>'
 *
 * Called from :  atalk_command(at_parse.c)
 */

void atdomain_parse_no_domain (int number, uchar *name, ushort len)
{
    at_Domain *dm_ptr;

    dm_ptr = atdomain_FindDomain_by_number(number);
    if (!dm_ptr) {
	printf("\n%% Could not find domain %d", number);
	return;
    }
    
    if ( len != dm_ptr->namelen ||
       (strncasecmp(dm_ptr->name, name, len))) {
	printf("\n%% Ignoring : Domain name is wrong. ");
	return;
    }

    atdomain_DeleteDomain(number);

}




/****************************************************************** 
 * Routine :      atdomain_group_parse(csb)
 *
 * Purpose :      Parser for interface '<no> appl domain-group <number>'
 *
 * Called from :  atalk_command(at_parse.c)
 */

void
atdomain_group_parse (parseinfo *csb)
{
    atalkidbtype *idb;
    at_Domain *dm_ptr;
    int     domain_number;
    
    domain_number = GETOBJ(int,1);
    idb           = atalk_getidb(csb->interface);

    dm_ptr = atdomain_FindDomain_by_number(domain_number);
    if (!dm_ptr) {
	printf("\n%% Domain %d not found ",domain_number);
	return;
    }
    
    
    if (!csb->sense) {
	if (dm_ptr != FindDomain_by_idb(idb)) {
	    printf("\n%% Domain %d is not on this interface", domain_number);
	    return;
	}
	/* These routines will take care of the rest */
	if(!atalkif_AURPTunnel(idb))
	    atalkif_DisablePort(idb);
	atdomain_DeleteIDB_fromDomain(idb);    
	if (!ActiveDomain(dm_ptr))  /* No active interfaces have this domain */
	    CleanUpDomain(dm_ptr);
	idb->atalk_domain = 0;
	idb->atalk_dminfo = NULL;
	if(atalkif_AURPTunnel(idb)) 
	    reg_invoke_atalk_aurpif_clear(idb->phys_idb);
/*	    aurp_config_tunnel(idb, TRUE, AURP_IP_DI);*/
	else
	    atalkif_RestartPort(idb);

	return;

    }

    if (idb->atalk_dminfo && idb->atalk_dminfo->number == domain_number)
	return; /* Already there */


    /* First disable the port 
     * it will call atdomain_DisablePort(idb)
     */
    if(!atalkif_AURPTunnel(idb)) {
	atalkif_DisablePort(idb);  
	atdomain_DeleteIDB_fromDomain(idb);    
    }

    PutNewIdb(dm_ptr,idb, FALSE);

    /* Provide a new dm_info into the idb */
    idb->atalk_domain = domain_number;
    idb->atalk_dminfo = dm_ptr;

    if(atalkif_AURPTunnel(idb)) {
	reg_invoke_atalk_aurpif_clear(idb->phys_idb);

    } else {
	atalkif_RestartPort(idb);
    }
}

/****************************************************************** 
 * Routine :      UpdateDomain(dm_ptr)
 *
 * Purpose :      Updates Domain dm_ptr : Called whenever there is 
 *                a global change in any of subdomains
 *
 * Called from :  atdomain_remap_parse(),at_parse_no_remap()
 */

static void
UpdateDomain (at_Domain *dm_ptr)
{

    domain_idbList *i_ptr;
    atalkidbtype        *idb;
 
    if (at_debug(atalkdomain_debug, ATALK_NULL_MASK))
	buginf("\nAT-DOMAIN: UpdateDomain for domain %d [%s]",
	       dm_ptr->number, dm_ptr->name);

    
    i_ptr = dm_ptr->idbHdr;

    /* OK, first let's see if there are any routes
     * that are the part of the inbound remap-list
     * Find them and lock the available remap 
     */

    while (i_ptr) {

	idb = i_ptr->idb;

	/* First disable the port 
	 * it will call atdomain_DisablePort(idb)
	 */
	if(!atalkif_AURPTunnel(idb))
	    atalkif_DisablePort(idb);  
        else
            atdomain_DisablePort(idb);
	
	/* Provide a new dm_info into the idb */
	idb->atalk_domain = dm_ptr->number;
	idb->atalk_dminfo = dm_ptr;

	/* Restart the port */
	if(atalkif_AURPTunnel(idb)) 
/*	    aurp_config_tunnel(idb, TRUE, AURP_IP_DI); */
	    reg_invoke_atalk_aurpif_clear(idb->phys_idb);
        else	
	    atalkif_RestartPort(idb);

	i_ptr = i_ptr->next;
    }
}

/****************************************************************** 
 * Routine :      atdomain_DeleteDomain(number)
 *
 * Purpose :      Deletes Domain <number>
 *
 * Called from :  atdomain_parse_no_domain()
 */

void
atdomain_DeleteDomain (int domain_number)
{
    at_Domain *dm_ptr;

    dm_ptr = atdomain_FindDomain_by_number(domain_number);

    if (!dm_ptr)
	return;

    if (at_debug(atalkdomain_debug, ATALK_NULL_MASK))
	buginf("\nAT-DOMAIN: DeleteDomain for domain %d [%s]",
	       dm_ptr->number, dm_ptr->name);

    DeleteAllSubDomains(dm_ptr); 
    Delete_idbs_fromDomain(dm_ptr);

    if (dm_ptr == DomainListHdr) 
	DomainListHdr =  (at_Domain *)dm_ptr->link.flink;
	
    if (dm_ptr == DomainListTail)
	DomainListTail = (at_Domain *)dm_ptr->link.blink;
	
    at_remque(&dm_ptr->link);
    
    free(dm_ptr);
    if (DomainListHdr == NULL)
	atalk_DomainRouter = FALSE;

    CheckForDomainRouter();
    
}

/****************************************************************** 
 * Routine :      DeleteAllSubDomains(dm_ptr)    
 *
 * Purpose :      Deletes inbound and outbound subdomains
 *
 * Called from :  atdomain_DeleteDomain()
 */

static void
DeleteAllSubDomains (at_Domain *dm_ptr)
{
    if (!dm_ptr)
	return;

    DeleteSubDomain(dm_ptr->in);
    dm_ptr->in  = NULL;

    DeleteSubDomain(dm_ptr->out);
    dm_ptr->out = NULL;
}

/****************************************************************** 
 * Routine :      DeleteSubDomains(sd)     
 *
 * Purpose :      Removes Subdomain sd
 *
 * Called from :  DeleteAllSubDomains()
 */

static void
DeleteSubDomain (at_SubDomain *sd)
{
    if (sd) {
	if (at_debug(atalkdomain_debug, ATALK_NULL_MASK))
	    buginf("\nAT-DOMAIN: DeleteSubDomain for domain %d ",
		   sd->domain_number);
	CleanSubDomain(sd);
	if (DeleteRemapTable(sd)) {
	    DeleteAvRemapList(sd);
	    free(sd);
	}

    }
}

/****************************************************************** 
 * Routine :      CleanUpDomain(dm_ptr)
 *
 * Purpose :      Cleans up both subdomains in the domain,
 *                initializes available remap lists
 *
 * Called from :  atdomain_group_parse(),atdomain_DisablePort()
 */

static void
CleanUpDomain (at_Domain *dm_ptr)
{

    DomainNeighbor  *n_ptr = DomainNeighborHdr;
    GeneralDomainList *l_ptr, *l_prev;
    


    if (at_debug(atalkdomain_debug, ATALK_NULL_MASK))
	buginf("\nAT-DOMAIN: CleanUpDomain for domain %d [%s]",
	       dm_ptr->number, dm_ptr->name);
    
    /* First let's get rid of all UnZipped entries 
     * with the remaps pointing to this domain
     */
    
    while (n_ptr) {
	if (   n_ptr->neighbor && n_ptr->neighbor->port
	    && n_ptr->neighbor->port->atalk_dminfo == dm_ptr) {
	    DeleteGeneralDomainList(n_ptr->UnZippedHdr);
	    n_ptr->UnZippedHdr = NULL;
	}
	n_ptr = n_ptr->next;
    }
    
    /*
     * Now delete all the loop entries pointing 
     * to this domain
     */
    l_ptr = LoopListHdr;
    l_prev = l_ptr;
    while (l_ptr) {
	if (l_ptr->remap_ptr->neighbor && 
	    l_ptr->remap_ptr->neighbor->port &&
	    l_ptr->remap_ptr->neighbor->port->atalk_dminfo == dm_ptr) {
	    
	    if (l_ptr == LoopListHdr) {
		LoopListHdr = l_ptr->next;
		l_ptr = l_ptr->next;
		free(l_prev);
		l_prev = l_ptr;
	    } else {
		l_prev->next = l_ptr->next;
		free(l_ptr);
		l_ptr = l_prev->next;
	    }
	} else {
	    l_prev = l_ptr;
	    l_ptr  = l_ptr->next;
	}
    }
	    

    CleanSubDomain(dm_ptr->in);
    CleanSubDomain(dm_ptr->out);
}

static void
DeleteGeneralDomainList (GeneralDomainList *hdr)
{
    GeneralDomainList *d_ptr, *d_prev;

    d_ptr = hdr;

    while (d_ptr) {
	d_prev = d_ptr;
	d_ptr  = d_ptr->next;
	free(d_prev);
    }
}

/****************************************************************** 
 * Routine :      CleanSubDomain(sd)
 *
 * Purpose :      Deletes Remap and AvailRemap Tables from subdomain
 *
 *
 * Called from :  DeleteSubDomain(),CleanUpDomain()
 */

static void
CleanSubDomain (at_SubDomain *sd)
{

    if(!sd)
	return;
    if (at_debug(atalkdomain_debug, ATALK_NULL_MASK))
	buginf("\nAT-DOMAIN: CleanSubDomain for %s subdomain %d ",
	       (sd->flags & inbound_r) ? "inbound" : "outbound",
	       sd->domain_number);

    /* OK, first let's purge all the routes off the routing table
     * We can do it, since CleanSubDomain is always called 
     * _after_ the atalkif_DisablePort, therefore it assures us
     * of at least 1 Notify Neighbor then
     */
    if (sd->flags & inbound_r) { /* Onlly for inbound routes */
	atdomain_PurgeAllDomainRoutes(sd->domain_number);
    }
 
    if (sd) {
	if (DeleteRemapTable(sd)) {
	    DeleteAvRemapList(sd);
	    atdomain_CreateAvList(sd);
	}
    }
}

/****************************************************************** 
 * Routine :      DeleteAvRemapList(sd)
 *
 * Purpose :      Removes Available Remap List from subdomain
 *
 * Called from :  CleanSubDomain(), LockAvCell()
 */

static void
DeleteAvRemapList (at_SubDomain *sd)
{
    at_AvailableRemap *av_ptr, *av_prev;
    
    if (!sd)
	return;

    if (at_debug(atalkdomain_debug, ATALK_NULL_MASK))
	buginf("\nAT-DOMAIN: DeleteAvRemapList for %s subdomain %d ", 
	       (sd->flags & inbound_r) ? "inbound" : "outbound",
	       sd->domain_number);
     
    av_ptr = sd->AvRemapHdr;

    while (av_ptr) {
	av_prev = av_ptr;
	av_ptr  = (at_AvailableRemap *)av_ptr->link.flink;
	at_remque(&av_prev->link);
	free(av_prev);
    }
    sd->AvRemapHdr  = NULL;
    sd->AvRemapTail = NULL;
}

/****************************************************************** 
 * Routine :      DeleteRemapTable(sd)
 *
 * Purpose :      Removes Remap Table from subdomain
 *
 * Called from :  CleanSubDomain()
 */

static boolean
DeleteRemapTable (at_SubDomain *sd)
{
    at_RemapTable *remap_ptr, *prev_remap;
    boolean Deleted_OK = TRUE;

    if (!sd)
	return (FALSE);


    if (at_debug(atalkdomain_debug, ATALK_NULL_MASK))
	buginf("\nAT-DOMAIN: DeleteRemapTable for subdomain %d ",
	       sd->domain_number);

    remap_ptr = sd->RemapListHdr;

    while (remap_ptr) {
	prev_remap = remap_ptr;
	remap_ptr = (at_RemapTable *)remap_ptr->link.flink;
	if (!atdomain_DeleteRemap(sd,prev_remap, TRUE))
	    Deleted_OK = FALSE;
    }
    return (Deleted_OK);
}

/****************************************************************** 
 * Routine :      atdomain_DisablePort(idb)
 *
 * Purpose :      Disables Domain operations on the interface
 *
 * Called from :  atalkif_DisablePort(at_maint.c)
 */

void
atdomain_DisablePort (atalkidbtype *idb)
{
    domain_idbList *i_ptr;

    if (!idb || !idb->atalk_dminfo)
	return;

    if (at_debug(atalkdomain_debug, ATALK_NULL_MASK))
	buginf("\nAT-DOMAIN: atdomain_DisablePort for %s",
	       idb_get_namestring(idb->phys_idb));

    if (!idb->atalk_operational) {
	i_ptr = idb->atalk_dminfo->idbHdr;
	while (i_ptr) {
	    if (i_ptr->idb == idb) {
		i_ptr->active = FALSE;
		if (!ActiveDomain(idb->atalk_dminfo))
		    CleanUpDomain(idb->atalk_dminfo);
		    
		break;
	    }
	    i_ptr = i_ptr->next;
	}
    }
}

/****************************************************************** 
 * Routine :      atdomain_InitialAdjust(idb)
 *
 * Purpose :      Supposed to be called when interface becomes operational.
 *
 * Called from :  maint_StateHandler()
 */

void
atdomain_InitialAdjust (atalkidbtype *idb)
{
    at_Domain    *dm_ptr;
    at_SubDomain *sd;
    atalk_rdb_t   *r;
    rbTree        *tree;
    at_RemapTable *remap_ptr;
    ushort Start,End;

    if (!idb)
	return;
    
    if (at_debug(atalkdomain_debug, ATALK_NULL_MASK))
	buginf("\nAT-DOMAIN: DomainInitialAdjust for %s",
	       idb_get_namestring(idb->phys_idb));
 
    /* collect all the existing
     * Domain neighbors that point to this idb
     */
    Collect_DomainNeighbors(idb);

    dm_ptr = idb->atalk_dminfo;

    if (!dm_ptr)
	return;

    PutNewIdb(dm_ptr,idb, TRUE);
    dm_ptr->active = TRUE;
    atalk_DomainRouter = TRUE;
    atalk_DomainRouter_running = TRUE;

    sd = dm_ptr->out;

    if (!sd)
	return;

    /* Now go through the whole routing table */
    tree = atroute_GetTree();
    for (r = KeepFirstRoute(tree); r != NULL; r = KeepNextRoute(tree, r)) {
	if(!r->rpath.idb)
	    continue;
	r->domain = r->rpath.idb->atalk_domain;
	
	if (idb == r->rpath.idb) {
	    /* OK, what is happening here is :
             * Since we just came up, then there are (probably)
	     * routes in the routing table that point to 
             * this idb that were remapped on outbound before!.
	     * Since now these routes belong to this domain,
	     * then we will have them on the inbound from now on.
             * Therefore, find them and delete them from the 
             * outbound remapping tables
             */

	    remap_ptr  = at_FindRemap(sd,r->dstrngstr,r->dstrngend);

	    if (remap_ptr) {
		if (!atdomain_DeleteRemap(sd,remap_ptr, FALSE))
		    continue;
		
		if (sd)  {
		    Start = r->dstrngstr;
		    End   = r->dstrngend;
		    if (End == 0)
			End = Start;

		    if (!at_FindAvRemapCell(sd, Start, End))
			at_PutAvRemapCell(sd, Start, End);
		}
	    }
	    continue;
	}

	/* Now create out remaps for this route on all the 
         * appropriate domains
	 */
	CreateOUTRemaps(r);
    }
}

/****************************************************************** 
 * Routine :      PutNewIdb(dm_ptr,idb)
 *
 * Purpose :      Puts a new idb to idb list of a domain
 *
 * Called from :  atdomain_group_parse(),Domain_InitialAdjust()
 */

static void
PutNewIdb (at_Domain *dm_ptr, atalkidbtype *idb, boolean operational)
{
    domain_idbList *i_ptr ,*new_i;

    if (!dm_ptr || !idb)
	return;

    new_i = malloc(sizeof(domain_idbList));
    if (!new_i) {
	errmsg(AT_ERR_NOMEM, "AT new IDB list", __LINE__, __FILE__);	
	return;
    }
    
    new_i->next = NULL;
    new_i->idb  = idb;
    new_i->active = operational;

    if (dm_ptr->idbHdr == NULL) {
	dm_ptr->idbHdr = new_i;
	return;
    }

    i_ptr = dm_ptr->idbHdr;
    
    while (i_ptr) {
	if (i_ptr->idb == idb) {
	    i_ptr->active = operational;
	    free(new_i);
	    return;
	}
	if (i_ptr->next == NULL)
	    break;
	i_ptr = i_ptr->next;
    }
    i_ptr->next = new_i;
}

/****************************************************************** 
 * Routine :      atdomain_DeleteIDB_fromDomain(idb)
 *
 * Purpose :      Removes this idb from the Domain's Idb list
 *
 * Called from :  atdomain_group_parse()
 */

static void
atdomain_DeleteIDB_fromDomain (atalkidbtype *idb)
{
    
    domain_idbList   *i_ptr, *i_prev;
    at_Domain        *dm_ptr;

    if (!idb->atalk_domain || !idb->atalk_dminfo)
	return;
    

    if (at_debug(atalkdomain_debug, ATALK_NULL_MASK))
	buginf("\nAT-DOMAIN: DeleteIDBfromDomain for %s Domain %d",
	       idb_get_namestring(idb->phys_idb), idb->atalk_domain);

    dm_ptr = idb->atalk_dminfo;
    
    i_ptr  = dm_ptr->idbHdr;
    i_prev = i_ptr;

    while (i_ptr) {
	if (i_ptr->idb == idb) {
	    if (dm_ptr->idbHdr->idb == idb)
		dm_ptr->idbHdr = dm_ptr->idbHdr->next;
	    i_prev->next = i_ptr->next;
	    free(i_ptr);
	    break;
	}
	i_prev = i_ptr;
	i_ptr  = i_ptr->next;
    }
}

/****************************************************************** 
 * Routine :      ActiveDomain(dm_ptr)
 *
 * Purpose :      Returns TRUE if Domain is active
 *
 * Called from :  atdomain_group_parse(), atdomain_DisablePort()
 */

boolean
ActiveDomain (at_Domain *dm_ptr)
{
    domain_idbList *i_ptr;
    boolean        Active = FALSE;
    

    i_ptr = dm_ptr->idbHdr;
    while (i_ptr) {
	if (i_ptr->active) {
	    Active = TRUE;
	    break;
	}
	i_ptr = i_ptr->next;
    }
    dm_ptr->active = Active;
    /* Now see if we are still a domain router */
    if(!Active) 
	CheckForDomainRouter();

    return (dm_ptr->active);
}

/****************************************************************** 
 * Routine :      FindDomain_by_idb(idb)
 *
 * Purpose :      Name tells it all.
 *
 * Called from :  atdomain_group_parse(), atdomain_DeleteDomain(),
 *                RemapProcess(), show routines, hop_reduction_parse()
 *                
 */

static at_Domain *
FindDomain_by_idb (atalkidbtype *idb)
{
    at_Domain *dm_ptr = DomainListHdr;
    domain_idbList   *i_ptr;

    while (dm_ptr) {
	i_ptr = dm_ptr->idbHdr;
	while (i_ptr) {
	    if (i_ptr->idb == idb)
		return (dm_ptr);
	    i_ptr = i_ptr->next;
	}
	dm_ptr = (at_Domain *)dm_ptr->link.flink;
    }
    return (NULL);
}

/*********************************************************** 
 * Routine :       atdomain_DDPinProcess
 *
 * Purpose :       Process DDP packet on inbound domain
 *
 * Called from :   atalk_input(at_input.c)
 */

void
atdomain_DDPinProcess (paktype *pak)
{
    atalkidbtype *idb_in;
    ushort  src_net, dest_net;
    ushort  remap_net, domain_net;
    at_SubDomain *sd_in, *sd_out;


    if (pak == NULL) 
	return;
    
    /* RTMPs are a separate case */
    if (atalk_pak_getpt(pak) == DDP_TYPE_RTMPReq || 
       atalk_pak_getpt(pak) == DDP_TYPE_RTMPRsp )
	return;
    
    idb_in = atalk_getidb(pak->if_input);
    if (!idb_in || !idb_in->atalk_domain || !idb_in->atalk_dminfo)
	return;
	
    sd_in  = idb_in->atalk_dminfo->in;
    sd_out = idb_in->atalk_dminfo->out;
    
    if (sd_in) {
	src_net = atalk_pak_snet(pak);
	if (LegitNet(src_net)) {
	
	    /* Now get remapped  net number */
	    remap_net = atdomain_FindForwardRemap(sd_in, src_net);
	    if (remap_net) 
		atalk_pak_setsnet(pak, remap_net);
	}
    }
    if (sd_out) {

	dest_net = atalk_pak_dnet(pak);
	if (LegitNet(dest_net)) {
	
	    /* Now get outr internal  net number */
	    domain_net = atdomain_FindBackwardRemap(sd_out, dest_net);

	    if (domain_net) 
		atalk_pak_setdnet(pak, domain_net);

	}

    }

    /* Now take care of NBP packets */
     if (atalk_pak_getpt(pak) == DDP_TYPE_NBP) 
        atdomain_RemapNBP(pak, IN);

    /* And last but not least : recompute the checksum */
    atalk_DDPcksum(pak);
}

/******************************************************** 
 * Routine :       atdomain_DDPoutProcess
 *
 * Purpose :       Process DDP packet on outbound domain
 *
 * Called from :   atalk_send1(at_input.c)
 */

void
atdomain_DDPoutProcess (paktype *pak)
{
    atalkidbtype *idb_out;
    ushort   source_net, dest_net;
    ushort   remap_net, domain_net;
    at_SubDomain *sd_in, *sd_out;


    if (pak == NULL)
	return;
    
    idb_out = atalk_getidb(pak->if_output);
    
    if (!idb_out || !idb_out->atalk_domain || !idb_out->atalk_dminfo)
	return;

    sd_in  = idb_out->atalk_dminfo->in;
    sd_out = idb_out->atalk_dminfo->out;

    if (sd_in) {
	dest_net = atalk_pak_dnet(pak);
	if (LegitNet(dest_net)) {

	    /* Now get remapped  net number */
	    domain_net = atdomain_FindBackwardRemap(sd_in, dest_net);
	    if (domain_net) 
		atalk_pak_setdnet(pak, domain_net);

	} else {
	    return;
	}
    }

    if (sd_out) {
	source_net = atalk_pak_snet(pak);
	if (LegitNet(source_net)) {

	    /* Now get remapped  net number */
	    remap_net = atdomain_FindForwardRemap(sd_out, source_net);
	    if (remap_net) 
		atalk_pak_setsnet(pak, remap_net);

	}
    }

    /* Now take care of NBP packets */
     if (atalk_pak_getpt(pak) == DDP_TYPE_NBP) 
        atdomain_RemapNBP(pak, OUT);
    
    /* Take care of checksumming here */
    atalk_DDPcksum(pak);
}

/****************************************************************** 
 * Routine :      atdomain_InsertNeighbor(n)
 *
 * Purpose :      Inserts a new AAppleTalk neihbor into 
 *                Domain Neighbor List
 *
 * Called from :  Collect_DomainNeighbors(), atmaint_CreateNeihgbor()
 */

void
atdomain_InsertNeighbor (neighborentry *n)
{

    DomainNeighbor *n_ptr, *new_dn;
    atalkidbtype        *idb;

    if (!n)
	return;

    idb = n->port;
    if (!idb->atalk_domain ||  !idb->atalk_dminfo)
	return;

    if (at_debug(atalkdomain_debug, ATALK_NULL_MASK))
	buginf("\nAT-DOMAIN: InsertNeighbor %a", n->address);

    n_ptr = DomainNeighborHdr;
    while (n_ptr) {

	if (n_ptr->neighbor == n)
	    return;
	if (n_ptr->next == NULL)
	    break;

	n_ptr = n_ptr->next;
    }

    new_dn = malloc(sizeof(DomainNeighbor));
    if (!new_dn) {
	errmsg(AT_ERR_NOMEM, "AT new domain neighbor", __LINE__, __FILE__);		return;
    }
    new_dn->next = NULL;
    new_dn->neighbor = n;
    /* And now we should not forget 
     * to protect this neighbor pointer
     */
    atmaint_ProtectNeighbor(n);
    
    new_dn->UnZippedHdr = NULL;
    
    if (DomainNeighborHdr == NULL)
	DomainNeighborHdr = new_dn;
    else
	n_ptr->next = new_dn;
}

/****************************************************************** 
 * Routine :      atdomain_DeleteNeighbor(n)
 *
 * Purpose :      Delete AppleTalk neihbor from 
 *                Domain Neighbor List
 *
 * Called from :  atmaint_DeleteNeighbor(at_maint.c)
 */

void
atdomain_DeleteNeighbor (neighborentry *n)
{
    DomainNeighbor *n_ptr, *n_prev;    
    at_SubDomain   *sd;
    at_Domain      *dm_ptr = NULL;
    at_RemapTable  *remap_ptr;

    if (!n)
	return;

    if (at_debug(atalkdomain_debug, ATALK_NULL_MASK))
	buginf("\nAT-DOMAIN: DeleteDomainNeighbor %a", n->address);

    /* Get the domain out of this neighbor */
    if (n->port)
	dm_ptr = n->port->atalk_dminfo;

    /* Delete the neighbor from the remaps */
    if (dm_ptr) {
	sd = dm_ptr->in;
	if (sd) {
	    remap_ptr = sd->RemapListHdr;
	    while (remap_ptr) {
		if (remap_ptr->neighbor == n) {
		    atmaint_UnprotectNeighbor(n);
		    remap_ptr->neighbor = NULL;
		}
		remap_ptr = (at_RemapTable *)remap_ptr->link.flink;
	    }
	}
    }

    n_ptr = DomainNeighborHdr;
    n_prev = n_ptr;
    while (n_ptr) {

	if (n_ptr->neighbor == n) {
	    /* OK, delete it */
	    if (n_ptr == DomainNeighborHdr) 
		DomainNeighborHdr = n_ptr->next;
	    else
		n_prev->next = n_ptr->next;
	    atmaint_UnprotectNeighbor(n);
	    free(n_ptr);
	    return;
	}
	n_prev = n_ptr;
	n_ptr = n_ptr->next;
    }
}

/****************************************************************** 
 * Routine :      atdomain_Initialize()
 *
 * Purpose :      Initializes global domain variables
 *
 * Called from :  domain_start()
 */
    
void
atdomain_Initialize (void)
{

    if (at_debug(atalkdomain_debug, ATALK_NULL_MASK))
	buginf("\nAT-DOMAIN: atdomain_Initialize");

    /*
     * Initialiaze global variables here 
     * so we will not lose them when restarting 
     * in the future releases of the OS 
     */

    DomainListHdr  = NULL;
    DomainListTail = NULL;

    LoopListHdr    = NULL;

    TempHdr.next = NULL;
    TempHdr.Start= 0;
    TempHdr.End  = 0;
    TempHdr.dist = 0;
    TempHdr.neighbor = NULL;

    last_queried_net = 0;

    NumOfUnzippedNets = 0;
    /* Initialize Domain Neighbor Hdr */
    DomainNeighborHdr = NULL;

    /*XXX change this ??? */
    domain_ZoneInterval = 5 *ONESEC;
    domain_LoopInterval = 5 *ONESEC;
    domain_GCInterval   = 10*ONESEC;
}

/****************************************************************** 
 * Routine :      atdomain_PathUpdate(r)
 *
 * Purpose :      Creates all the remaps for the newly created route
 *
 * Called from :  at_AddPath(at_route.c)
 */

void
atdomain_PathUpdate (atalk_rdb_t *r)
{
    atalkidbtype   *new_idb;
    int old_domain;

    if (!r)
	return;

    old_domain = r->domain;

    new_idb = r->rpath.idb;
    if (!new_idb)
        return;
    
    /*
     * If the route comes back to us from another domain,
     * then we should assume that this is because 
     * of the count to infinity and this route is on the 
     * way to be deleted. So, ignore it for domain processing
     */
    if (old_domain && new_idb->atalk_domain != old_domain)
	return;

    /* Now check if the domain changed */
    if (old_domain && old_domain == new_idb->atalk_domain) 
	atdomain_RemapUpdate(r);
    
    CreateOUTRemaps(r);
    r->domain = new_idb->atalk_domain;
}

/****************************************************************** 
 * Routine :      CreateOUTRemaps(r)
 *
 * Purpose :      Creates outbound remaps for this route
 *
 * Called from :  Domain_InitialAdjust(), atdomain_PathUpdate()
 */

static void
CreateOUTRemaps (atalk_rdb_t *r)
{
    at_Domain *dm_ptr = DomainListHdr;
    at_SubDomain *sd;
    at_RemapTable *remap_ptr;

    if (!r || !r->rpath.idb)
	return;
    /* Now go through the all domains and create out remaps*/
    while (dm_ptr) {

	if ((dm_ptr->number != r->rpath.idb->atalk_domain) && dm_ptr->active) {

	    sd = dm_ptr->out;
	    if (sd) {

		if (!atdomain_FindForwardRemap(sd,r->dstrngstr)) {
                    /* Don't allow conflict routes to be remapped for AURP */
                    if (atdomain_PathInAURPInbound(r->best_path) &&
                        dm_ptr->in && atutil_RangesOverlap(r->dstrngstr,r->dstrngend,
                        dm_ptr->in->remap_start, dm_ptr->in->remap_end)) {

                        printf("\nAT: Net %d-%d conflict with AURP domain's"
                            " inbound range %d-%d\n",
                            r->dstrngstr,r->dstrngend,
                            dm_ptr->in->remap_start, dm_ptr->in->remap_end);
        	        dm_ptr = (at_Domain *)dm_ptr->link.flink;
                        continue;
		    }

		    remap_ptr = atdomain_RemapProcess(sd,r->dstrngstr,
						      r->dstrngend,
						      r->rpath.metric.u.rtmp.hops,
						      r->rpath.gwptr);
		    if(remap_ptr)
			Remap_GotoState(remap_ptr, Established);
		}
	    }
	}
	dm_ptr = (at_Domain *)dm_ptr->link.flink;
    }
}

/****************************************************************** 
 * Routine :      atdomain_RemapUpdate()
 *
 * Purpose :      Updates the remapping entry corresponding to this r
 *
 * Called from :  atdomain_PathUpdate()
 */

void
atdomain_RemapUpdate (atalk_rdb_t *r)
{
    at_RemapTable *remap_ptr;
    at_SubDomain  *sd;
    ushort        domain_net;

    if (!r)
	return;

    /* OK, let's find the remapping entry */

    if (!r->rpath.idb)
	return;
    if (!r->rpath.idb->atalk_dminfo)
	return;
    sd = r->rpath.idb->atalk_dminfo->in;
    if (!sd) 
	return;
    domain_net = atdomain_FindBackwardRemap(sd,r->dstrngstr);
    if (!domain_net)
	return;
    remap_ptr = at_FindRemap(sd,domain_net,0);
    if (!remap_ptr)
	return;
    /* Now update remap_ptr */
    remap_ptr->dist = r->rpath.metric.u.rtmp.hops;
    remap_ptr->neighbor = r->rpath.gwptr;
}

/****************************************************************** 
 * Routine :      atdomain_CreateRemap(idb,Start,End)
 *
 * Purpose :      Creates remap for the New EIGRP or AURP route
 *               
 * Called from :  atigrp_buildpak(atigrp2.c), at_aurp.c
 */

at_RemapTable *
atdomain_CreateRemap (atalkidbtype *idb, ushort domain_start,
		      ushort domain_end)
{
    at_RemapTable *remap_ptr;
    at_SubDomain  *sd;

    if (!idb || !idb->atalk_dminfo)
	return (NULL);
    if (!idb->atalk_dminfo->active)
	return (NULL);
    if (!atdomain_NetSanityCheck(idb,domain_start))
	return (NULL);

    sd = idb->atalk_dminfo->in;
    
    remap_ptr = at_FindRemap(sd,domain_start,domain_end);
    if (remap_ptr) 
	return (remap_ptr);
    
    remap_ptr = atdomain_RemapProcess(sd,domain_start,domain_end,1,NULL);

    if (remap_ptr) 
	Remap_GotoState(remap_ptr, Established);

    return (remap_ptr);
}

/****************************************************************** 
 * Routine :      atdomain_NetSanityCheck(idb,net)
 *
 * Purpose :      Make sure that we can remap this net
 *
 * Called from :  atdomain_CreateRemap()
 */

static boolean
atdomain_NetSanityCheck (atalkidbtype *idb, ushort net)
{
    at_Domain *dm_ptr;
    atalk_rdb_t *r;
    rbTree      *tree;
    atalkidbtype     *r_idb;

    if (!LegitNet(net))
	return (FALSE);
    
    dm_ptr = idb->atalk_dminfo;
    if (!dm_ptr)
	return (FALSE);

    if (ConnectedNet(net)) {
	tree = atroute_GetTree();
	r = atroute_FindRoute(tree,net, FALSE);
	if (!r)
	    return (FALSE);
	r_idb = r->rpath.idb;
	if (r_idb) {
	    if (r_idb->atalk_domain == dm_ptr->number) {
		ReleaseRoute(r);
		return (FALSE);
	    }
	}
	ReleaseRoute(r);
    }
    return (TRUE);
}

/****************************************************************** 
 * Routine :      atdomain_GetRemapOut(idb,net)
 *
 * Purpose :      Returns an outbound remap for the net
 *
 * Called from :  at_igrp2.c, at_rtmp.c 
 */

ushort
atdomain_GetRemapOut (atalkidbtype *idb, ushort net)
{
    at_SubDomain *sd;
    ushort remap_net = 0;

    if (!idb || !idb->atalk_dminfo)
	return (0);

    /* If the net is from the same domain, get domain net to
       send back.  Else use the remap out range. This is for
       EIGRP case. */

    /* Try to do a backward remap on an inbound range. */
    sd = idb->atalk_dminfo->in;
    if (sd) {
	remap_net = atdomain_FindBackwardRemap(sd, net);
	if (remap_net)
	    return remap_net;
    }
    
    /* Net isn't part of inbound range, so remap on outbound range. */
    sd = idb->atalk_dminfo->out;
    if (!sd)
	return (0);
    remap_net = atdomain_FindForwardRemap(sd, net);
    return (remap_net);
}

/****************************************************************** 
 * Routine :      atdomain_FilterNet(idb,net,direction)
 *
 * Purpose :      Domain Split-Horizon routine
 *
 * Called from :  rtmp_GetNextRoute(at_rtmp.c), atigrp2_getdest(),
 *                atigrp2_advertise_request()
 */

boolean
atdomain_FilterNet (atalkidbtype *idb, ushort net, domain_direction dir)
{
    at_SubDomain *sd ;

    if (!idb || !idb->atalk_dminfo)
	return (FALSE);

    if (dir == FromDomain) {
	sd = idb->atalk_dminfo->out;
	if (!sd)
	    return (FALSE);
	if (net >= sd->remap_start && net <= sd->remap_end) 
	    return (TRUE);
    } else if (dir == ToDomain) {
	sd = idb->atalk_dminfo->in;
	if (!sd)
	    return (FALSE);
	if (Domain_of_Net(net) == idb->atalk_dminfo) 
	    return (TRUE);
    }
    return (FALSE);
}

/****************************************************************** 
 * Routine :      Domain_of_Net(net)
 *
 * Purpose :      Returns Domain of this net
 *
 * Called from :  atdomain_FilterNet()
 */

static at_Domain *
Domain_of_Net (ushort net)
{
    atalk_rdb_t *r = NULL;
    rbTree      *tree;

    tree = atroute_GetTree();
    r = atroute_LookupRoute(tree,net,FALSE);
    if (!r)
	return (NULL);
    return (r->rpath.idb->atalk_dminfo);
}

/*****************************************************************
 * Routine :        atdomain_ProcessRTMP(paktype *pak)
 *
 * Purpose :        Process RTMP packet
 *         
 *
 * Called from :    rtmp_HandleResponse(at_rtmp.c)
 */

void
atdomain_ProcessRTMP (paktype *pak)
{
    rtmptuple *rt;
    int nbytes;
    neighborentry* neighbor;
    atalk_pdb_t *p;
    atalk_rdb_t *r;
    rbTree	*p_tree, *r_tree;
    ushort rngstart, rngend;
    boolean extended, valid;
    uchar dist;
    atalkidbtype* idb;
    at_RemapTable *remap_ptr;
    ushort   clsize = 0;
    at_SubDomain *sd;


    idb = atalk_getidb(pak->if_input);
    if (!idb->atalk_domain || !idb->atalk_dminfo)
	return;
    if (!idb->atalk_dminfo->active)
	return;
    neighbor = rtmp_GetNeighbor(pak);
    if (neighbor == NULL)
	return;

    sd = idb->atalk_dminfo->in;
    
    rt = (rtmptuple *)(((uchar *)pak->transport_start) + RTMPHDRBYTES);
    nbytes = pak->atalk_datalen - RTMPHDRBYTES;

    extended = atalkif_net_extended(idb);
    p_tree = atroute_GetPTree(AT_PATH_RTMP);
    r_tree = atroute_GetTree();

    atalk_pak_printprocess(pak, atalkrtmp_debug, "RTMP data");

    /* Skip the first "tuple" - checked already.  For nonextended
     * networks, this is the version number; for extended networks
     * this is the locally connected network.  So safe to skip.
     */
    if (!atrtmp_GetNextTuple(&rt,&nbytes,&rngstart,&rngend,&dist,
			     RTMPTUPLEVERSION)) 
	return;

    /* 
     * Loop through all routing tuples
     */
    while (nbytes > 0) {
	if (!atrtmp_GetNextTuple(&rt,&nbytes,&rngstart,&rngend,&dist,
				 RTMPTUPLEVERSION)) {
	    atalk_pak_printfail(pak,atalkrtmp_debug,"RTMP bad tuple");
	    neighbor->rtpacketsaborted++;
	    break;
	}
	/*
	 * Check for admin distance and add it to provided distance.
	 */
	if (idb->atalk_distance) {
	    dist += idb->atalk_distance / RTMP_BANDWIDTH;
	}
	if (rngend == 0)
	    neighbor->rtnonextendedrts++;
	else
	    neighbor->rtextendedrts++;
	
	/*
	 * Even though it will get remapped, we still check for
	 * incoming ACL's
	 */
	if (  idb->atalk_route_filter_in &&
	    ! atalk_routecheck(rngstart, rngend, idb->atalk_route_filter_in))
	    continue;

	/* Now check if this is a known (remapped) route */
	remap_ptr = at_FindRemap(sd, rngstart, rngend);
	
	if (!remap_ptr) {
	    if (dist >= DDP_MAX_HOPCOUNT)
		continue;

	    if (idb->atalk_dminfo->in && idb->atalk_dminfo->cluster) {
		if (rngend)
		    clsize += (rngstart - rngend + 1);
		else
		    clsize++;
		
		AddToTempRouteList(rngstart,rngend, dist, neighbor);
		if (clsize >= idb->atalk_dminfo->clsize) {
		    /* Aha, time to create a cluster */
#ifdef cluster_on
		    if(!at_CreateCluster(idb,&route_list_hdr, clsize)) {
			printf("\n Could not create a cluster with the size of %d",clsize);
		    }
		    
		    clsize = 0;
		    at_FreeRouteList(&route_list_hdr);
#endif		 
		}
		/* And go to the next tuple */
		continue;
	    }

	    /* Check for connected net */
	    if (ConnectedNet(rngstart))
		continue;

	    remap_ptr = atdomain_RemapProcess(sd, rngstart, rngend,
					      dist, neighbor);
	    Remap_GotoState(remap_ptr, UnZipped);
	    
	    continue;
	}
	if (remap_ptr->status != Established) 
	    continue;			/* Let remapping module deal with it */

	/* Now get remapped start and end of the range */
	if (rngend)
	    rngend = remap_ptr->remap_start + rngend - rngstart;

	rngstart   = remap_ptr->remap_start;

	valid = FALSE;
	r = NULL;
	p = NULL;

	if ((dist <= DDP_MAX_HOPCOUNT) || (dist == DDP_POISON_HOPCOUNT)) {
	    r = atroute_GetValidRoute(r_tree,rngstart,rngend,&valid,
				      (dist == DDP_POISON_HOPCOUNT));
	}

	/*
	 * If we get an invalid route, then tuple conflicts with something
	 * in our routing table.  Skip this tuple.
	 */
	if (!valid) {
	    neighbor->rtbadrts++;
	    continue;
	}

	if (r && valid && atalk_net_connected(r)) {
	    if (dist == DDP_POISON_HOPCOUNT)
		neighbor->rtbadrts++;	/* poisoning one of our ifs */
	    else
		neighbor->rtignoredrts++;
	} else {			/* didn't have route or !connected */
	    /*
	     * If the tuple doesn't have the same width as the
	     * route entry but it has the same lower end as the route,
	     * then ignore this tuple. If the tuple really does
	     * represent the new path for this route, then
	     * we'll time out the route and re-learn from
	     * the new extended route.
	     */
	    if (   r
		&& ((r->dstrngstr == rngstart) && (r->dstrngend != rngend))) {
		neighbor->rtignoredrts++;
		continue;
	    }
	    p = atroute_GetValidPath(p_tree,rngstart,rngend,&valid,(dist == DDP_POISON_HOPCOUNT));
	    if (p) {			/* found a RTMP path route */
		if ((dist == DDP_MAX_HOPCOUNT) && (idb->atalk_compatibility)) {
		    dist = DDP_POISON_HOPCOUNT;
		    neighbor->oldcisco = TRUE;
		}
		if ((dist == DDP_POISON_HOPCOUNT) && 
		    (atrtmp_GetPathMetric(p) <= DDP_MAX_HOPCOUNT))
		    neighbor->rtpoisonedrts++;
		else
		    neighbor->rtoldrts++;
		rtmp_UpdateEntry(p, dist, idb, neighbor);
	    } 

	    else {

#if 0
               /* For redistribution of RTMP routes */

		if (valid) {
		    if (dist < DDP_MAX_HOPCOUNT) {
			rtmp_CreateEntry(p_tree,rngstart,rngend,dist,idb,
					 neighbor);
			neighbor->rtnewrts++;
			/* Now let's check if we need to put it into
			   UnZipped Queue
			 */
			r = atroute_LookupRoute(r_tree,rngstart,FALSE);
			if(r && r->rpath.type == AT_PATH_RTMP ) {
			    atmaint_ResetRequestInfo(r);
			    remap_ptr->neighbor = neighbor;
			    Remap_GotoState(remap_ptr, UnZipped);
			}
		    } else {
			if (   (dist == DDP_MAX_HOPCOUNT)
			    && (neighbor->port->atalk_compatibility))
			    neighbor->oldcisco = TRUE;
			neighbor->rtignoredrts++;
		    }
		}
#endif
	    }
	}
    }					/* end loop through routing tuples */

    if (clsize) {
#ifdef cluster_on
	/* Aha, time to create a cluster */
	if (!at_CreateCluster(idb,&route_list_hdr, clsize)) 
		printf("\n Could not create a cluster with the size of %d",clsize);
	DeleteTempList();
#endif
    }	
	
    if (at_debug(atalkrtmp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	buginf("\nAT: RTMP from %a (new %d,old %d,bad %d,ign %d, dwn %d)",
	       neighbor->address, neighbor->rtnewrts, neighbor->rtoldrts,
	       neighbor->rtbadrts, neighbor->rtignoredrts, 
	       neighbor->rtpoisonedrts);
    neighbor->badroutes += neighbor->rtbadrts;
    neighbor->newroutes += neighbor->rtnewrts;
}

/*
 * Routine :         atdomain_RemapProcess(sd, Start, End, dist, neighbor)
 *
 * Purpose :         Does all Remap Processing and returns the new Remap
 *
 *
 * Called from :     atdomain_ProcessRTMP
 */

static at_RemapTable *
atdomain_RemapProcess (at_SubDomain *sd, ushort Start, ushort End, 
		       uchar dist, neighborentry *neighbor)
{
    ushort RemapedNet;
    ushort RangeSize;
    boolean Extended;
    at_RemapTable *remap_ptr;
    at_Domain     *dm_ptr;

    if (!sd)
	return (NULL);

    dm_ptr = atdomain_FindDomain_by_number(sd->domain_number);

    if (!dm_ptr || !dm_ptr->active)
	return (NULL);


    /* Don't remap network 0 */
    if (!Start) {
        if (at_debug(atalkremap_debug, ATALK_NULL_MASK))
	    buginf("\nAT-REMAP: Net %d domain %s will not be remapped", 
		   Start, dm_ptr->name);
        return (NULL);
    }

    Extended = (End) ? TRUE : FALSE;
    if (Extended)
	RangeSize = End - Start + 1;
    else
	RangeSize = 1;

    RemapedNet = GetAvailRemap(sd, RangeSize);
    if (RemapedNet)
	if (at_debug(atalkremap_debug, ATALK_NULL_MASK))
	    buginf("\nAT-REMAP: RemapProcess for net %d domain %s Remaped Net %d", 
		   Start, dm_ptr->name, RemapedNet);
    
    if (RemapedNet == 0){
	/* Overflow of the range */
	errmsg(AT_ERR_OVERFLOW, (sd->flags & inbound_r) ? "INBOUND" : "OUTBOUND",
	       sd->domain_number);

	if (!Extended)
	    buginf("\nAT-REMAP:  Could not find a remap for net %d", Start);
	else
	    buginf("\nAT-REMAP:  Could not find a remap for cable %d-%d", Start, End);

	Domain_ShutDown(dm_ptr);

	return (NULL);
    }

    /* Now create a new Remap entry */
    remap_ptr = Remap_PutNewEntry(sd,Start, End, dist, neighbor, RemapedNet);
    return (remap_ptr);
}

/****************************************************************** 
 * Routine :      Domain_ShutDown(dm_ptr)
 *
 * Purpose :      Shut Down the domain
 *
 * Called from :  at_RemapProcessor(), at_LoopForDomain()
 */

static void
Domain_ShutDown (at_Domain *dm_ptr)
{
    domain_idbList *i_ptr, *i_prev;
    atalkidbtype *idb;

    if (!dm_ptr)
	return;
    if (!dm_ptr->active)
	return;

    i_ptr = dm_ptr->idbHdr;

    buginf("\nAT-DOMAIN: Disabling Domain %d [ %s ]",
	   dm_ptr->number,dm_ptr->name);
    /* Shut down all the interfaces of this domain */
    while (i_ptr) {
	i_prev = i_ptr;
	i_ptr = i_ptr->next;

	buginf("\nAT-DOMAIN: Disabling interface %s ",
	       idb_get_namestring(i_prev->idb->phys_idb));
	idb = i_prev->idb;

	/* This will go in as soon as dsa fixes EIGRP 
	   interface shutting
	 */
	if(atalkif_AURPTunnel(idb))
	    shutdown_interface(idb->phys_idb, TRUE);
	    
	else
	    atalkif_DisablePort(idb);

	i_prev->active = FALSE;
    }
    dm_ptr->active = FALSE;
    CleanUpDomain(dm_ptr);
}

/* 
 * Routine :       atdomain_FindForwardRemap(sd, net)
 *
 * Purpose :       Finds and returns a forward remap for net
 *                 Forward Remap is the remap from domain to remap
 * Called from :   
 */
ushort
atdomain_FindForwardRemap (at_SubDomain *sd, ushort net)
{
    ushort RemapNet = 0;
    at_RemapTable   *remap_ptr;

    if (!sd)
	return (0);

    if (net == 0)
	return (0);

    remap_ptr = sd->RemapListHdr;

    while (remap_ptr) {
	if (remap_ptr->domain_end == 0) {
	    if (remap_ptr->domain_start == net) {
		RemapNet = remap_ptr->remap_start;
		break;
	    }
	} else {
	    if (  (remap_ptr->domain_start <= net)
		&& (net <= remap_ptr->domain_end)) {
		RemapNet = net - remap_ptr->domain_start + remap_ptr->remap_start;
		break;
	    }
	}
	remap_ptr = (at_RemapTable *)remap_ptr->link.flink;
    }
    return (RemapNet);
}

/********************************************************************
 * Routine :       atdomain_FindBackwardRemap(sd, net)
 *
 * Purpose :       Finds and return a backward remap for net
 *                 Backward Remap is the remap from remapped to domain
 * Called from :
 */
ushort
atdomain_FindBackwardRemap (at_SubDomain *sd, ushort net)
{
    ushort        DomainNet = 0;
    at_RemapTable *remap_ptr;

    if (!sd)
	return (0);
    if (net == 0)
	return (0);
    remap_ptr = sd->RemapListHdr;

    while (remap_ptr) {
	if (remap_ptr->domain_end == 0) {
	    if (net == remap_ptr->remap_start) {
		DomainNet = remap_ptr->domain_start;
		break;
	    }
	} else {
	    if (   (net >= remap_ptr->remap_start)
		&& (net <= (remap_ptr->remap_start + remap_ptr->domain_end - remap_ptr->domain_start))) {
		DomainNet = remap_ptr->domain_start + net - remap_ptr->remap_start;
		break;
	    }
	}
	remap_ptr = (at_RemapTable *)remap_ptr->link.flink;
    }
    return (DomainNet);
}

/********************************************************************
 * Routine :       atdomain_ndb_new(idb,Start,End)
 *
 * Purpose :       Informs us of the new ndb created for EIGRP route Start
 *
 * Called from :   atigrp2_ndb_new(at_igrp2.c)
 */

void
atdomain_ndb_new (atalkidbtype *idb, ushort Start, ushort End)
{
    at_Domain *dm_ptr = DomainListHdr;
    at_SubDomain *sd;
    ushort    domain_net;
    at_RemapTable *remap_ptr;

    dm_ptr = idb->atalk_dminfo;
    if (dm_ptr == NULL)
	return;
    sd = dm_ptr->in;
    if (sd == NULL)
	return;
    
    /* Now get remap ptr */
    domain_net = atdomain_FindBackwardRemap(sd,Start);
    if (domain_net) {
	remap_ptr = at_FindRemap(sd,domain_net,0);
	if (remap_ptr) 
	    remap_ptr->protocol_links |= IN_USE_BY_EIGRP;
    }

}




/********************************************************************
 * Routine :       atdomain_route_done(Start,End)
 *
 * Purpose :       Informs us that main routing table is done with this route 
 *
 * Called from :   atroute_DeleteRoute(at_route.c)
 */

void
atdomain_route_done (ushort Start, ushort End)
{
    at_Domain *dm_ptr = DomainListHdr;
    at_SubDomain *sd;
    ushort    domain_net;
    at_RemapTable *remap_ptr;

    while(dm_ptr) {
	sd = dm_ptr->in;
	if(sd) {
	    if (Start >= sd->remap_start && Start <= sd->remap_end) {
		/* Find Remap */
		domain_net = atdomain_FindBackwardRemap(sd, Start);
		if(domain_net) {
		    remap_ptr = at_FindRemap(sd,domain_net, 0);
		    if(remap_ptr) {
			remap_ptr->protocol_links &= ~IN_USE_BY_RTMP;
			atdomain_DeleteDomainRoute(dm_ptr->number, Start, End);
		    }
		}
		return;		
	    }

	}
	dm_ptr = (at_Domain *)dm_ptr->link.flink;
   }
    
    atdomain_DeleteDomainRoute(0,Start,End);
}





/********************************************************************
 * Routine :       atdomain_ndb_done(Start,End)
 *
 * Purpose :       Informs us that dual is done with this ndb EIGRP route Start
 *
 * Called from :   atigrp2_ndb_done(at_igrp2.c)
 */

void
atdomain_ndb_done (ushort Start, ushort End)
{
    at_Domain *dm_ptr = DomainListHdr;
    at_SubDomain *sd;
    ushort    domain_net;
    at_RemapTable *remap_ptr;

    while (dm_ptr) {
	sd = dm_ptr->in;
	if (sd) {
	    if (Start >= sd->remap_start && Start <= sd->remap_end) {
		/* Find Remap */
		domain_net = atdomain_FindBackwardRemap(sd,Start);
		if (domain_net) {
		    remap_ptr = at_FindRemap(sd,domain_net,0);
		    if (remap_ptr) {
			remap_ptr->protocol_links &= ~IN_USE_BY_EIGRP;
			atdomain_DeleteDomainRoute(dm_ptr->number,Start,End);
		    }
		}
		return;
	    }
	}
	dm_ptr = (at_Domain *)dm_ptr->link.flink;
    }
    
    atdomain_DeleteDomainRoute(0,Start,End);
}

/****************************************************************** 
 * Routine :      atdomain_DeleteDomainRoute(domain,Start,End)
 *
 * Purpose :      Delete all remaps for this route
 *
 * Called from :  atdomain_DeleteRoute(at_route.c)
 */

void
atdomain_DeleteDomainRoute (int domain, ushort Start, ushort End)
{
    at_SubDomain  *sd;
    at_Domain     *dm_ptr;
    ushort        domain_start, remap_Start;
    ushort        domain_end, remap_End;
    at_RemapTable *remap_ptr;
    atalk_rdb_t   *rt;
    rbTree        *tree;

    /*
     * First let's see if the entry is still in the 
     * routing table
     */

    tree = atroute_GetTree();
    rt = atroute_FindRoute(tree,Start, FALSE);
    if (rt) {
	ReleaseRoute(rt);
	return;
    }
    
    
    /* Now for AURP */
    if(reg_invoke_atalk_aurp_FindPathentry(Start,End))
        return;

    /* OK, now let's find remap_ptr on the inbound */
    dm_ptr = atdomain_FindDomain_by_number(domain);
    if (dm_ptr) {
	sd = dm_ptr->in;
	if (sd) {
	    /* Ok, let's try to find a remap */
	    domain_start = atdomain_FindBackwardRemap(sd,Start);
	    if (domain_start) {
		if (End)
		    domain_end = domain_start + End - Start;
		else
		    domain_end = 0;
		remap_ptr = at_FindRemap(sd,domain_start,domain_end);
		if (remap_ptr) {
		    if (remap_ptr->protocol_links || Protected(remap_ptr))
			return;
		    if (atdomain_DeleteRemap(sd,remap_ptr, FALSE))
			if(sd->RemapListHdr != NULL)
			    atdomain_UnLockAvCell(sd,Start,End);
		}
	    }
	}
    }


    /* Now delete all OUT remaps */
    dm_ptr = DomainListHdr;
    while (dm_ptr) {
	if (dm_ptr->number != domain) {
	    sd = dm_ptr->out;
	    if (sd) {
		remap_ptr = at_FindRemap(sd, Start, End);
		if (remap_ptr) {
		    if (!remap_ptr->protocol_links && !Protected(remap_ptr)) {
			remap_Start = remap_ptr->remap_start;
			remap_End   = remap_ptr->remap_end;
			if (atdomain_DeleteRemap(sd,remap_ptr,FALSE)) {
			    if(sd->RemapListHdr != NULL)
				CreateAvRemap(sd,remap_Start,remap_End);
			}
		    }
		}
	    }
	}
	dm_ptr = (at_Domain *)dm_ptr->link.flink;
    }
}

/****************************************************************** 
 * Routine :      CreateAvRemap(sd, Start, End)
 *
 * Purpose :      Inserts new Available Cell [Start,End] to 
 *                this subdomain.
 *
 * Called from :  atdomain_DeleteDomainRoute(), LockAvCell()
 */

static void
CreateAvRemap (at_SubDomain *sd, ushort Start, ushort End)
{

    if (sd )  {

	if (End == 0) {

	    if (!at_FindAvRemapCell(sd,Start, Start))
		at_PutAvRemapCell(sd, Start, Start );
	} else {
	    if (!at_FindAvRemapCell(sd, Start, End))
		at_PutAvRemapCell(sd, Start, End);
	}
    }
}

/****************************************************************** 
 * Routine :      at_domain_ZIPXReply(pak)
 *
 * Purpose :      Processes the Extended ZIP reply
 *
 * Called from :  
 */

void
atdomain_ZipXReply (paktype *pak)
{
    int cnt;
    ziptype *zhdr;
    zipentry *zip;
    ushort net;
    ataddrtype zipaddr = atalk_pak_srcaddr(pak);
    boolean err = FALSE;
    int len;
    at_RemapTable  *remap_ptr;
    GeneralDomainList *d_ptr;
    DomainNeighbor *n_ptr;
    neighborentry  *n;


    if (at_debug(atalkpkt_debug, ATALK_NULL_MASK))
	buginf("\nAT: ZIP Extended reply rcvd from %a", zipaddr);
    zhdr = (ziptype *)pak->transport_start;
    cnt = zhdr->cnt;			/* get network count */
    zip = (zipentry *)(((uchar *)zhdr) + ZIPTYPEBYTES); /* skip ZIP header */
    net = GETSHORT(zip->net);

    if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("\nAT: %d zones for %d, ZIP XReply, src %a",cnt,net,
	       zipaddr);
    n     = atmaint_FindNeighbor(zipaddr, atalk_getidb(pak->if_input));
    if ( !n)
	return;
    n_ptr = GetDomainNeighbor(n);
    if (!n_ptr)
	return;

    d_ptr = at_FindGeneralDomainElement(n_ptr->UnZippedHdr, net); 

    if (!d_ptr) /* If not found, then we do not expect any more zones */
        return;
	
    remap_ptr = d_ptr->remap_ptr;
    
    if (remap_ptr->neighbor && remap_ptr->neighbor->address != zipaddr) {
        if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
		buginf("\nAT: Ignoring ZIP from %a, updating via %a",
		       zipaddr,remap_ptr->neighbor->address);
	return;

    }
    if (remap_ptr->zonecnt == 0)
	remap_ptr->zonecnt = cnt;
    len = pak->atalk_datalen;
    len -= ZIPTYPEBYTES;

    while ((len >= ZIPENTRYBYTES) && (remap_ptr->zonecnt > 0)) {
 	if ((GETSHORT(zip->net) != net)||(zip->len == 0)|| 
	    (zip->len > ZONELEN)) {
	    err = TRUE;
	    break;
	}
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: net %d, zonelen %d, name %*s",net,zip->len, 
		   zip->len, zip->name);

	Domain_AddNewZone(remap_ptr, zip->name, zip->len);

	if (at_debug(atalk_events, ATALK_NULL_MASK))
	    errmsg(AT_ERR_NEWZONE, zip->len, zip->name);
	remap_ptr->zonecnt--;
	len -= ZIPENTRYBYTES + zip->len - 1;
	zip = (zipentry *)(((uchar *)zip) + ZIPENTRYBYTES + zip->len - 1);
    }
    
    if (err) {
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: Bad reply for %d, ZIPxreply pkt, src %a",net,
		   zipaddr);
	ClearRemapZones(remap_ptr);
    }

    Domain_FinishZipMerge(pak->if_input, remap_ptr);
}

/****************************************************************** 
 * Routine :      atdomain_ProcessZIP(pak)
 *
 * Purpose :      Process non-extended ZIPs
 *
 * Called from :  
 */

boolean
atdomain_ProcessZIP (paktype *pak)
{
    ataddrtype zipaddr;
    neighborentry *n;
    atalkidbtype *idb;
    
    if (atdomain_CheckForDiscovery(pak))
	return (FALSE);
    idb = atalk_getidb(pak->if_input);
    
    if (idb->atalk_dminfo && !idb->atalk_dminfo->in)
	return (FALSE);

    zipaddr = atalk_pak_srcaddr(pak);
    n = atmaint_FindNeighbor(zipaddr, idb);
    if (!n || (n->reachable_by & ATALK_IGRP_ENABLED))
	return (FALSE);

    return (TRUE);
}

/********************************************************************
 * Routine :     atdomain_ZIPReply(paktype *pak)
 *
 * Purpose :     Process incoming ZIP Reply (nonextended) packets
 *
 * Called from : zip_MergeReply(at_zip.c)
 */

void
atdomain_ZIPReply (paktype *pak)
{
    int cnt;
    ziptype *zhdr;
    zipentry *zip;
    ataddrtype zipaddr;
    ushort net;
    at_RemapTable  *remap_ptr;
    GeneralDomainList *d_ptr;
    TempRouteList     *t_ptr;
    DomainNeighbor    *n_ptr;
    neighborentry     *n;
    at_SubDomain      *sd;
    

    if (!pak)
	return;
    
    sd = atalk_getidb(pak->if_input)->atalk_dminfo->in;

    zipaddr = atalk_pak_srcaddr(pak);

    n     = atmaint_FindNeighbor(zipaddr, atalk_getidb(pak->if_input));
    if (!n)
	return;
    n_ptr = GetDomainNeighbor(n);
    if (!n_ptr)
	return;

    /* XXX for Cayman zipaddr == 0 */
    zhdr = (ziptype *)pak->transport_start;
    cnt = zhdr->cnt;		/* get network count */
    if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("\nAT: %d zones in ZIPreply pkt, src %a", cnt,zipaddr);
    if (cnt == 0)
	return;
    zip = (zipentry *)(((uchar *)zhdr) + ZIPTYPEBYTES); /* skip ZIP header */

    while (cnt > 0) {
	if ((zip->len > ZONELEN) || (zip->len == 0))
	    break;
	
	net = GETSHORT(zip->net);

	AddToTempRouteList(net, 0, 0, NULL);
	d_ptr = at_FindGeneralDomainElement(n_ptr->UnZippedHdr, net); 
	if (!d_ptr) /* If not found, then we do not expect any more zones */
	    return;
	remap_ptr = d_ptr->remap_ptr;
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: net %d, zonelen %d, name %*s",
		    net, zip->len, zip->len, zip->name);
	if (remap_ptr->neighbor && remap_ptr->neighbor->address == zipaddr) {
		Domain_AddNewZone(remap_ptr, zip->name, zip->len);

		if (at_debug(atalk_events, ATALK_NULL_MASK))
		    errmsg(AT_ERR_NEWZONE, zip->len, zip->name);
	
	} 

	zip = (zipentry *)(((uchar *)zip) + ZIPENTRYBYTES + zip->len - 1);
	cnt--;
    }

    if ((cnt != 0) && at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("\nAT: Invalid zone in reply from %a",zipaddr);

    /* OK, now go through the Temp list we created and finish 
     * the zone merging 
     */
    
    t_ptr = TempHdr.next;
    while (t_ptr) {
	remap_ptr = at_FindRemap(sd, t_ptr->Start, t_ptr->End);
	if (remap_ptr)
	    Domain_FinishZipMerge(pak->if_input, remap_ptr);

	t_ptr = t_ptr->next;

    }
    /* And now we shall get rid of the Temp List that we created here */
    DeleteTempList();
}

/****************************************************************** 
 * Routine :      atdomain_CheckForDiscovery(pak)
 *
 * Purpose :      Check if the input interface is in the middle
 *                of discovery
 *
 * Called from :  atdomain_ProcssZIP()
 */

boolean
atdomain_CheckForDiscovery (paktype *pak)
{
    
    atalkidbtype *idb;
    int cnt;
    ziptype *zhdr;
    zipentry *zip;
    ataddrtype zipaddr;
    ushort net;

    if (!pak)
	return (FALSE);

    idb = atalk_getidb(pak->if_input);
    if (!idb)
	return (FALSE);
    zipaddr = atalk_pak_srcaddr(pak);
        /* XXX for Cayman zipaddr == 0 */
    zhdr = (ziptype *)pak->transport_start;
    cnt = zhdr->cnt;		/* get network count */
    if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("\nAT: %d zones in ZIPreply pkt, src %a", cnt,zipaddr);
    if (cnt == 0)
	return (FALSE);
    zip = (zipentry *)(((uchar *)zhdr) + ZIPTYPEBYTES); /* skip ZIP header */
    if ((zip->len > ZONELEN) || (zip->len == 0))
	return (FALSE);
	
    net = GETSHORT(zip->net);
	
    if (atalk_netonly(idb->atalk_confaddr) == net)
	return (TRUE);
    return (FALSE);
}

/****************************************************************** 
 * Routine :      ClearRemapZones(remap_ptr)
 *
 * Purpose :      Removes zone list from the remap entry
 *
 * Called from :  at_domain_ZIPXReply(pak)
 */

static void
ClearRemapZones (at_RemapTable *remap_ptr)
{
    Domain_ZoneList *z_ptr, *z_prev;

    if (!remap_ptr)
	return;

    z_ptr = remap_ptr->ZoneListHdr;
    while (z_ptr) {
	z_prev = z_ptr;
	z_ptr  = z_ptr->next;
	free(z_prev);
    }
    remap_ptr->ZoneListHdr = NULL;
    remap_ptr->zonecnt     = 0;
}

/****************************************************************** 
 * Routine :      Domain_FinishMerge(idb,remap_ptr)
 *
 * Purpose :      Finishes an aquisition of the zone list
 *
 * Called from :  
 */

static void
Domain_FinishZipMerge (idbtype *idb, at_RemapTable *remap_ptr)
{
    /* Are we done acquiring zones?? */
    if (remap_ptr->zonecnt)
	return;

    if (remap_ptr->ZoneListHdr) {

	/* Now let's delete UnZipped Entry */
/*	UnProtectRemap(remap_ptr); */
	RemoveUnZippedEntry(remap_ptr);
	
	LoopInspection(remap_ptr);
    }
}

/****************************************************************** 
 * Routine :      RemoveUnZippedEntry(remap_ptr)
 *
 * Purpose :      Removes this remap from the UnZipped List
 *                of the Domain neighbor
 *
 * Called from :  
 */

static void
RemoveUnZippedEntry (at_RemapTable *remap_ptr)
{
    GeneralDomainList *d_ptr, *d_prev;
    DomainNeighbor    *n_ptr;
    
    n_ptr = GetDomainNeighbor(remap_ptr->neighbor);
    if(!n_ptr)
	return;

    d_ptr = n_ptr->UnZippedHdr;
    d_prev = d_ptr;
    while (d_ptr) {
	if (d_ptr->remap_ptr == remap_ptr) {
	    if (d_ptr == n_ptr->UnZippedHdr) 
		n_ptr->UnZippedHdr = d_ptr->next;
	    d_prev->next = d_ptr->next;
	    free(d_ptr);
	    NumOfUnzippedNets--;
	    return;
	}
	d_prev = d_ptr;
	d_ptr  = d_ptr->next;
    }
}

/****************************************************************** 
 * Routine :      GetDomainNeighbor(n)
 *
 * Purpose :      Get a pointer to a domain neighbor
 *
 * Called from :  
 */

static DomainNeighbor *
GetDomainNeighbor (neighborentry *n)
{
    DomainNeighbor *n_ptr = DomainNeighborHdr;
    

    while (n_ptr) {
	if (n_ptr->neighbor == n) 
	    return (n_ptr);
	n_ptr = n_ptr->next;
    }
    return (NULL);
}

/****************************************************************** 
 * Routine :      Domain_AddRemapZone(remap_ptr)
 *
 * Purpose :      Inserts the zone list of the remap
 *                into the main zone table
 *
 * Called from :  
 */

static void 
Domain_AddRemapZone (at_RemapTable *remap_ptr)
{
    Domain_ZoneList *z_ptr;
    atalk_rdb_t *r;
    rbTree      *tree;

    tree = atroute_GetTree();
    r = atroute_FindRoute(tree,remap_ptr->remap_start, FALSE);
    if (!r)
	return;

    z_ptr = remap_ptr->ZoneListHdr;
    while (z_ptr) {

	atzip_AddZone(r, z_ptr->name, z_ptr->len);
	if (at_debug(atalk_events, ATALK_NULL_MASK))
	    errmsg(AT_ERR_NEWZONE, z_ptr->len, z_ptr->name);

	z_ptr = z_ptr->next;

    }
    atroute_path_redist_rbshell((treeLink *)r->best_path, FALSE); 
    ReleaseRoute(r);
}

/****************************************************************** 
 * Routine :      Domain_AddRemapRoute(remap_ptr)
 *
 * Purpose :      Inserts this remap into the main routing table
 *
 * Called from :  
 */

static void
Domain_AddRemapRoute (at_RemapTable *remap_ptr)
{
    neighborentry *neighbor;
    ushort        Start, End = 0;
    uchar         dist;
    rbTree        *p_tree, *r_tree;
    atalkidbtype       *idb;
    atalk_rdb_t   *r;

    if (!remap_ptr)
	return;

    neighbor = remap_ptr->neighbor;
    if(!neighbor) {
	if(at_debug(atalkremap_debug, ATALK_NULL_MASK))
	    buginf("\nAT-REMAP: Could not add remap %d to the routing table",
		   remap_ptr->remap_start);
	return;
    }
    idb      = neighbor->port;
    Start    = remap_ptr->remap_start;
    if (remap_ptr->domain_end )
	End      = remap_ptr->remap_start + 
	           remap_ptr->domain_end - remap_ptr->domain_start;

    dist     = remap_ptr->dist;
    
    if (neighbor->reachable_by & ATALK_RTMP_ENABLED) {
	p_tree = atroute_GetPTree(AT_PATH_RTMP);	
	r_tree = atroute_GetTree();
	r = atroute_LookupRoute(r_tree,Start, FALSE);
	if(r && r->rpath.type == AT_PATH_RTMP)
	    return;
	atrtmp_CreateEntry(p_tree, Start, End, dist, idb, neighbor);
	neighbor->rtnewrts++;
	neighbor->newroutes += neighbor->rtnewrts;
    }
}

/****************************************************************** 
 * Routine :      DeleteTempList()
 *
 * Purpose :      Removes the remap global temp list
 *
 * Called from :  
 */

static void
DeleteTempList (void)
{
    TempRouteList *t_ptr, *t_prev;
    
    t_ptr = TempHdr.next;

    while (t_ptr) {
	t_prev = t_ptr;
	t_ptr = t_ptr->next;
        free(t_prev);
    }

    TempHdr.next = NULL;
    TempHdr.Start= 0;
    TempHdr.End  = 0;
    TempHdr.dist = 0;
    TempHdr.neighbor = NULL;
}

/****************************************************************** 
 * Routine :      AddtoTempRouteList(Start,End,dist,neighbor)
 *
 * Purpose :      Adds a new element to global Temp List
 *
 * Called from :  
 */

static void
AddToTempRouteList (ushort Start,ushort End,
		    uchar dist, neighborentry *neighbor)

{
    TempRouteList *t_ptr, *new_tr;

    t_ptr = &TempHdr;
    
    while (t_ptr) {
	if (t_ptr->Start == Start) 
	    return;
	if (t_ptr->next == NULL)
	    break;
	
	t_ptr = t_ptr->next;
    }
    new_tr = malloc(sizeof(TempRouteList));
    
    if (!new_tr) {
	errmsg(AT_ERR_NOMEM, "AT new Temp List", __LINE__, __FILE__);
	return;
    }

    new_tr->Start = Start;
    new_tr->End   = End;
    new_tr->dist  = dist;
    new_tr->neighbor = neighbor;
    new_tr->next  = NULL;

    t_ptr->next   = new_tr;
}
    

/***************************/
void
show_appletalk_remap (parseinfo *csb)
{
    at_Domain  *dm_ptr = DomainListHdr;
    int domain_number;
    int domain_dir;

    domain_number = GETOBJ(int,1);

    if (domain_number) {
	dm_ptr = atdomain_FindDomain_by_number(domain_number);
	if (!dm_ptr) {
	    printf("\n%% Domain %d does not exist",domain_number);
	    return;
	}

	automore_enable("\n              AppleTalk   Remapping  Table :\n");
	printf         ("             ------------------------------");

	printf("\n");

	domain_dir  = GETOBJ(int,2);
	if (domain_dir) {
	    ShowRemap(dm_ptr,domain_dir,csb);
	    return;
	}

	atdomain_showDomainRemap(dm_ptr);
	return;
    }

    automore_enable("\n              AppleTalk   Remapping  Table :\n");
    printf         ("             ------------------------------");

    printf("\n");

    while (dm_ptr) {
	atdomain_showDomainRemap(dm_ptr);
	dm_ptr = (at_Domain *)dm_ptr->link.flink;
    }
}

static void 
atdomain_showSubDomainRemap (at_SubDomain *sd)
{
    at_RemapTable *remap_ptr;

    if (!sd)
	return;
    
    printf("\n");
    printf("\n             Direction : %s",(sd->flags & inbound_r) ? "IN" : "OUT");
    printf("\n");

    printf("\n       Domain Net(Cable)       Remapped to         Status");
    for(remap_ptr = KeepFirstRemap(sd); remap_ptr != NULL;
	remap_ptr = KeepNextRemap(sd,remap_ptr)) {
	     
	if (remap_ptr->domain_end == 0) 
	    printf("\n       %5d                   %5d                %s",
		   remap_ptr->domain_start, remap_ptr->remap_start, 
		   PrintRemapStatus(remap_ptr->status));
	else
	    printf("\n       %5d - %5d           %5d - %5d        %s",
		   remap_ptr->domain_start, remap_ptr->domain_end,
		   remap_ptr->remap_start, 
		   remap_ptr->remap_start + remap_ptr->domain_end - 
		   remap_ptr->domain_start, 
		   PrintRemapStatus(remap_ptr->status));

    }
    printf("\n");
}

static void
atdomain_showDomainRemap (at_Domain *dm_ptr)
{

    printf("\n\n");
    printf("\n             Domain %d : %s   State : %s", 
	   dm_ptr->number,dm_ptr->name, dm_ptr->active ? "Active" : "Nonactive");
    printf("\n          ------------------------------------------");

    atdomain_showSubDomainRemap(dm_ptr->in);
    atdomain_showSubDomainRemap(dm_ptr->out);
}



void
show_appletalk_domain (parseinfo *csb)
{
    int domain_number;
    at_Domain *dm_ptr;

    domain_number = GETOBJ(int,1);
    
    if (domain_number) {
	dm_ptr = atdomain_FindDomain_by_number(domain_number);
	if (!dm_ptr) {
	    printf("\n%% Domain %d does not exist",domain_number);
	    return;
	}
	automore_enable("\n        AppleTalk   Domain   Information: \n");
	printf("\n");
	
	ShowDomain(dm_ptr);
	return;
    }
    
    dm_ptr = DomainListHdr;

    automore_enable("\n        AppleTalk   Domain   Information: \n");
    printf("\n");
    
    while (dm_ptr) {
	ShowDomain(dm_ptr);
	dm_ptr = (at_Domain *)dm_ptr->link.flink;
    }
}

static void
ShowDomain (at_Domain *dm_ptr)
{
    domain_idbList *i_ptr = dm_ptr->idbHdr;
    int i = 0;;

    printf("\n        Domain %6d   Name : %s ",dm_ptr->number,dm_ptr->name);
    printf("\n      ---------------------------");
    for(i=dm_ptr->namelen; i > 0; i--)
	printf("-");
    printf("\n        State                : %s",
	   dm_ptr->active ? "Active" : "Nonactive");

    if (dm_ptr->in) 
	printf("\n        Inbound remap range  : %d-%d",
	       dm_ptr->in->remap_start,dm_ptr->in->remap_end);
    else
	printf("\n        Inbound remap range  : None");
    
    if (dm_ptr->out)
	printf("\n        Outbound remap range : %d-%d",
	       dm_ptr->out->remap_start,dm_ptr->out->remap_end);
    else
	printf("\n        Outbound remap range : None");

    printf("\n        Hop reduction        : %s",
	   dm_ptr->hop_rdctn ? "ON" : "OFF");
    printf("\n        Interfaces in domain :");
    if (i_ptr == NULL) {
	printf("\n              NONE");
    } else {
	while (i_ptr) {
	    printf("\n          %18s : %s",
		   idb_get_namestring(i_ptr->idb->phys_idb),
		   i_ptr->active ? "Enabled" : "Disabled");
	    i_ptr = i_ptr->next;
	}
    }

    printf("\n\n");
}

void
ShowRemap (at_Domain *dm_ptr, int domain_dir,parseinfo *csb)
{
    int remap_dir;
    ushort net;
    at_SubDomain *sd;
    ushort remap_net = 0;

#define TO_DOMAIN 1
#define FROM_DOMAIN 2

    remap_dir = GETOBJ(int,3);
    net       = GETOBJ(int,4);

    switch (domain_dir) {
      case TO_DOMAIN : 
	  sd = dm_ptr->out;
	  if (!sd) {
	      printf("\n No outbound remap-range for domain %d",dm_ptr->number);
	      return;
	  }
	  break;
      case FROM_DOMAIN :
	  sd = dm_ptr->in;
	  if (!sd) {
	      printf("\n No inbound remap-range for domain %d",dm_ptr->number);
	      return;
	  }
	  break;

      default :
	  return;
    }
    /*
     * Stands for 'sh appl remap domain %d in/out' command
     */
    if (remap_dir == 0) {

	printf("\n\n");
	printf("\n             Domain %d : %s   State : %s", 
	       dm_ptr->number,dm_ptr->name,
	       dm_ptr->active ? "Active" : "Nonactive");
	printf("\n          ------------------------------------------");

	atdomain_showSubDomainRemap(sd);
	return;
    }
    if (remap_dir == TO_DOMAIN) {
	remap_net = atdomain_FindForwardRemap(sd,net);
	if (remap_net) {
	    printf("\n For for net %d  the Remap  is  %d",
		   net,remap_net);
	    return;
	}
    } else {
	remap_net = atdomain_FindBackwardRemap(sd,net);
	if (remap_net) {
	    printf("\n For the Remap %d  the Domain  net is  %d",
		   net,remap_net);
	    return;
	}
    }
    if (!remap_net)
	printf("\n No match");
}

void
atalk_show_pvt_remap (void)
{
    at_SubDomain  *sd;
    at_Domain     *dm_ptr;

    dm_ptr = DomainListHdr;
    while (dm_ptr) {
	printf("\nDomain %d\n", dm_ptr->number);
        sd = dm_ptr->in;
	if (sd) {
	    printf(" + Inbound available list\n");
	    showSubDomainAv(sd);
	}
	
        sd = dm_ptr->out;
	if (sd) {
	    printf(" + Outbound available list\n");
	    showSubDomainAv(sd);
	}
	
	dm_ptr = (at_Domain *)dm_ptr->link.flink;
    }
    
}


static void
showSubDomainAv (at_SubDomain *sd)
{
    at_AvailableRemap *av_ptr;
    
    av_ptr = sd->AvRemapHdr;
    if (!av_ptr) {
	printf("\tWarning: empty list, should not happen!\n");
    } else {
	do {
	    printf("\tstart: %d, end: %d\n", av_ptr->Start, av_ptr->End);
	    av_ptr = (at_AvailableRemap *)av_ptr->link.flink;
        } while (av_ptr);
    }
}

/****************************************************************** 
 * Routine :      at_LoopForDomain(domain)
 *
 * Purpose :      There is a loop on domain. Shut it off.
 *
 * Called from :  atdomain_NBPLoop()
 */

static void
at_LoopForDomain (int domain)
{
    at_Domain   *dm_ptr;

    if (!domain)
	return;

    dm_ptr = atdomain_FindDomain_by_number(domain);
    if (!dm_ptr)
	return;

    errmsg(AT_ERR_LOOP, dm_ptr->number, dm_ptr->name);

    /*
     * The order is very important :
     * First shut down the domain,
     * then purge all the routes pointing
     * to this domain
     */
    Domain_ShutDown(dm_ptr);
}

/****************************************************************** 
 * Routine :      LoopInspection(remap_ptr)
 *
 * Purpose :      Does a loop inspection for this remap.
 *                If passes, then move remp to Established State
 *
 * Called from :  Domain_FinishZipMerge()
 */

void
LoopInspection (at_RemapTable *remap_ptr)
{
    int suspect_domain;

    if (remap_ptr == NULL)
	return;
    if (remap_ptr->neighbor == NULL)
	return;
    if (remap_ptr->neighbor->port == NULL)
	return;
    if (remap_ptr->neighbor->port->atalk_dminfo == NULL)
	return;

    suspect_domain = remap_ptr->neighbor->port->atalk_dminfo->number;

    if (remap_ptr->ZoneListHdr == NULL)
	return;

    if (LoopSuspect(remap_ptr->remap_start, remap_ptr->remap_end,
		    remap_ptr->ZoneListHdr->name,remap_ptr->ZoneListHdr->len,
		    suspect_domain)) {

	Remap_GotoState(remap_ptr, Suspect);
	StartLoopDetection(remap_ptr);

    } else {
	InsertDomainRoute(remap_ptr);
    }
}

/****************************************************************** 
 * Routine :      LoopSuspect(Start,End,Zonelist, domain)
 *
 * Purpose :      Returns true if this route is loop-suspect
 *
 * Called from :  LoopInspection()
 */

static boolean
LoopSuspect (ushort Start, ushort End, uchar *suspect_zone,
	     ulong suspect_zone_len, int suspect_domain)
{
    atalk_rdb_t   *r;
    rbTree        *tree;
    boolean       ZoneListCheck = FALSE;
    zipnetentry   *zipentry;

    tree = atroute_GetTree();
    for (r = KeepFirstRoute(tree); r != NULL; r = KeepNextRoute(tree, r)) {
	
	if(Start == r->dstrngstr)
	    continue;

	if (End == 0) {
	    if (r->dstrngend == 0)
		ZoneListCheck = TRUE;
	} else {
	    if ((End - Start) == (r->dstrngend - r->dstrngstr))
		ZoneListCheck = TRUE;
	}
	if (ZoneListCheck) {
	    zipentry = r->zonelist;
	    while (zipentry) {
		if (atnbp_namematch(suspect_zone,zipentry->zone->name,
				    suspect_zone_len, zipentry->zone->length,
				    NBP_ZONE_WILD)) {
		    ReleaseRoute(r);
		    return (TRUE);
		}
		zipentry = zipentry->nextZone;
	    }
	}
	ZoneListCheck = FALSE;
    }
    return (FALSE);
}

/****************************************************************** 
 * Routine :      StartLoopDetection(remap_ptr)
 *
 * Purpose :      Kicks off the loop detection for this remap
 *
 * Called from :  
 */

static void
StartLoopDetection (at_RemapTable *remap_ptr)
{
    GeneralDomainList *new_l,*l_ptr = LoopListHdr;
    
    /* First let's see if this remap 
     * is already in the Loop Queue
     * If it is not then put it in.
     * If the Loop Queue does not exist,
     * then create it 
     */
    while (l_ptr) {
	if (l_ptr->remap_ptr == remap_ptr)
	    return;
	if (l_ptr->next == NULL)
	    break;
	l_ptr = l_ptr->next;
    }
    /* Create a new Loop Element */
    new_l = malloc(sizeof(GeneralDomainList));
    if (!new_l) {
	errmsg(AT_ERR_NOMEM, "AT new Loop element", __LINE__, __FILE__);
	return;
    }
    new_l->remap_ptr = remap_ptr;
    new_l->next = NULL;
    new_l->retries   = 0;
    TIMER_STOP(new_l->timer);

    if (l_ptr == NULL)
	LoopListHdr = new_l;
    else
	l_ptr->next = new_l;

    /* Send the first series of Loop Packets */
    SendLoopPackets(new_l);
}



/****************************************************************** 
 * Routine :      at_SendLoopPackets(l_ptr)
 *
 * Purpose :      Sends off a bunch of Loop packets for this 
 *                loop entry
 *
 * Called from :  at_StartLoopDetection()
 */

static void
SendLoopPackets (GeneralDomainList *l_ptr)
{
    at_Domain *suspect_domain;
    atalkidbtype   *idb;
    
    if (l_ptr->remap_ptr->neighbor && l_ptr->remap_ptr->neighbor->port)
	suspect_domain = l_ptr->remap_ptr->neighbor->port->atalk_dminfo;

    FOR_ALL_ATIDBS(idb) {
	
	if (idb->atalk_enabled && idb->atalk_operational) {
	    SendLoop(idb, NULL, 0);
	}
    }

    l_ptr->retries++;
}

/****************************************************************** 
 * Routine :      atdomain_SendLoop(idb)
 *
 * Purpose :      Sends a Loop packet on this interface
 *
 * Called from :  SendLoopPackets()
 */

static void
SendLoop (atalkidbtype *idb, uchar *suspect_zone, uint suspect_len)
{
    ataddress	dst,src;
    ulong	size;
    paktype	*pak;
    uchar	*pstr;
    nbptype	*nbpcntl;
    nbptuple	*tup;
    char	type[33], zone[33];
    uchar	LoopNameSize;


    /* Review: an nbp pkt:
     * nbptype:
     *  unsigned control : 4;
     *  unsigned ntuples : 4;
     *  uchar NBPid;
     *      NBP_OVERHEADBYTES = 2
     * nbptuple:  
     *  uchar net[DDPNETBYTES];
     *  uchar nodeID;
     *  uchar socket;   
     *  uchar enumerator;
     *      NBPTUPLESIZE = 5
     * + nbpEntity:
     *  str32 srvr_name
     *  str32 type_srvc
     *  str32 srvr_zone
     */
    
    LoopNameSize = sizeof(int) + sizeof(idb);
    strcpy(type,LOOP_STRING);
    if(suspect_zone == NULL || suspect_len == 0) {
	if(idb->zonelist)
	    bcopy(idb->zonelist->zone->name,zone, idb->zonelist->zone->length);
	else
	    return;
    }
    else {
	bcopy(suspect_zone, zone, suspect_len);

    }
    
        /* calculate size of pkt: */
    size = NBP_OVERHEADBYTES + NBPTUPLESIZE;
    size += LoopNameSize + 1; /* 1 for the length byte */
    size += strlen(type)+1;
/*    size += strlen(zone)+1; */
    if(suspect_len)
	size += suspect_len + 1;
    else
	size += idb->zonelist->zone->length + 1;

    src.net  = atalkif_MyNet(idb);

    src.node = atalkif_MyNode(idb);
    src.spec = ATALK_SOCK_NBP;

    dst.net = 0;
    dst.node = ATALK_BROAD_ADDR; /* broadcast */
    dst.spec = ATALK_SOCK_NBP;

    /* get a pkt: */
    pak = atalk_pak_create(
        idb,
        atalk_address(dst.net,dst.node),
        src.spec,
        ATALK_SOCK_NBP,     /* pkt always to NIS */
        DDP_TYPE_NBP,
        size);

    if (pak == NULL) {
/*	buginf("\nAT-DOMAIN Could not create AT packet "); */
        return; /* no memory */
    }

    /* set control portion: */
    nbpcntl = (nbptype *) pak->transport_start;
 
    nbp_set_type(nbpcntl, NBP_BrRq);   

    nbp_set_count(nbpcntl, 1);
    nbpcntl->NBPid = LOOP_NBP_ID;

    /* set tuple: */
    tup = (nbptuple *)((uchar *) nbpcntl + NBP_OVERHEADBYTES);

    PUTSHORT(tup->net, src.net); 

    tup->node = src.node;
    tup->socket = src.spec;
    tup->enumerator = 0;    /* not used for LkUp */


    /* copy NVE: */
    pstr = ((uchar *) tup + NBPTUPLESIZE);

    *pstr = LoopNameSize;
    pstr++;

    PUTLONG(pstr,LoopId);
    pstr += sizeof(int);

    PUTLONG(pstr,idb->atalk_domain);
    pstr += sizeof(int);

    *pstr = strlen(type);
    pstr++;

    bcopy(type,pstr,strlen(type));
    (uchar *) pstr += strlen(type);
    if(suspect_len)
	*pstr = suspect_len;
    else
	*pstr = idb->zonelist->zone->length;

    pstr++;

    if(suspect_len)
	bcopy(zone,pstr,suspect_len);
    else
	bcopy(zone,pstr,idb->zonelist->zone->length);

    atalk_DDPcksum(pak);

    if (atalk_send(pak))
        atalk_stat[ATALK_NBP_OUT]++;
    else
	atalk_stat[ATALK_NBP_FAILED]++;
}

/****************************************************************** 
 * Routine :      atdomain_NBPLoop(pak)
 *
 * Purpose :      Checks whether a loop packet arrived, 
 *                checks for a loop
 *
 * Called from :  at_nbp.c
 */

boolean
atdomain_NBPLoop (paktype *pak)
{
    atalkidbtype  *idb, *new_idb, *idb_in;
    uchar    *nbp_ptr;
    nbptype  *nbp;
    uchar    *type_ptr;
    char     type[33];
    ushort   typelen;
    boolean  Suspect = FALSE;
    long     new_id;
    int      suspect_domain;

    if (!pak)
	return (FALSE);

    if (atalk_pak_getpt(pak) != DDP_TYPE_NBP)
	return (FALSE);

    if (pak->atalk_datalen < LOOP_MIN_DATA_SIZE )
	return (FALSE);

    idb_in = atalk_getidb(pak->if_input);
    if (!idb_in) 
	return (FALSE);

    nbp     = (nbptype *)pak->transport_start;
    
    if (!nbp) 
	return (FALSE);

    nbp_ptr  = (uchar *)nbp + sizeof(nbptype);

    nbp_ptr += NBPTUPLESIZE;
    
    type_ptr =  nbp_ptr;
    type_ptr += *type_ptr + 1; /*Skip the object */
    typelen  = *type_ptr;
    type_ptr++;
    sstrncpy(type,type_ptr,typelen+1);
    if (atnbp_namematch(type,LOOP_STRING,typelen,strlen(LOOP_STRING),
			NBP_ZONE_WILD)) {
	
	if (*nbp_ptr != LOOP_NAME_SIZE) 
	    return (FALSE);
	nbp_ptr++;
	
	bcopy(nbp_ptr,&new_id,sizeof(int));

	if (new_id != LoopId) {
	    return (FALSE);
	}
	nbp_ptr += sizeof(int);
	bcopy(nbp_ptr,&new_idb,sizeof(idb));
	
	bcopy(nbp_ptr,&suspect_domain,sizeof(int));

	if (idb_in->atalk_domain != suspect_domain) 
	    Suspect = TRUE;

    }
    if (!Suspect) {
	return (FALSE);
    }
    /* OK ,there is a Loop there */
    at_LoopForDomain(suspect_domain);
    at_LoopForDomain(idb_in->atalk_domain);

    return (TRUE);
}

/***************************************************
  Private stuff 
 ***************************************************/


/********************************************************* 
 * Routine :      at_domain_nv_write(void)
 *
 * Purpose :      Writes all the 'domain ...' commands 
 *                to nv-ram
 *
 * Called from :  atdomain_parse()
 */
static void
at_domain_nv_write (void)
{
    at_Domain *dm_ptr = DomainListHdr;
        
    while (dm_ptr) {
	
	nv_write(TRUE, "appletalk domain %d name ",
		 dm_ptr->number);
	atalk_nv_printzone(dm_ptr->name);

	if(dm_ptr->in) 
	    nv_write(TRUE,
		 "appletalk domain %d remap-range in %d-%d",
		 dm_ptr->number, dm_ptr->in->remap_start, 
		 dm_ptr->in->remap_end);

	if(dm_ptr->out)
	nv_write(TRUE,
		 "appletalk domain %d remap-range out %d-%d",
		 dm_ptr->number, dm_ptr->out->remap_start, 
		 dm_ptr->out->remap_end);

	nv_write(dm_ptr->hop_rdctn, "appletalk domain %d hop-reduction",
		 dm_ptr->number);
	
	dm_ptr = (at_Domain *)dm_ptr->link.flink;
    }
}

/********************************************************************* 
 * Routine :      hop_reduction_parse(csb)
 *
 * Purpose :      Parser for '<no> appl domain <number> hop reduction'
 *
 * Called from :  atdomain_parse()
 */

static void
hop_reduction_parse (parseinfo *csb)
{
    int domain_number;
    at_Domain *dm_ptr;
    
    domain_number = GETOBJ(int,1);
    
    dm_ptr = atdomain_FindDomain_by_number(domain_number);
    if (!dm_ptr) {
	printf("\n%% AppleTalk Domain %d not found", domain_number);
	return;
    }

    if (!csb->sense) {
	dm_ptr->hop_rdctn = FALSE;
        UpdateDomain(dm_ptr);
	return;
    }

    if (dm_ptr->hop_prsrv) {
	printf("\n%% AT_DOMAIN: Hop reduction cannot be enabled: "
               "hop preservation is ON");
	return;
    }
    dm_ptr->hop_rdctn = TRUE;
    UpdateDomain(dm_ptr);
}

/******************************************************************** 
 * Routine :      hop_preservation_parse(csb)
 *
 * Purpose :      Parser for '<no> appl domain <number> hop preserver'
 *
 * Called from :  atdomain_parse
 */

static void
hop_preservation_parse (parseinfo *csb)
{
    int domain_number;
    at_Domain *dm_ptr;
    
    domain_number = GETOBJ(int,1);
    
    dm_ptr = atdomain_FindDomain_by_number(domain_number);
    if (!dm_ptr) {
	printf("\n%% AppleTalk Domain %d not found", domain_number);
	return;
    }
    if (csb->nvgen) {
	nv_write(dm_ptr->hop_prsrv, "appletalk domain %d hop-preservation", 
		 dm_ptr->number);
	return;
    }

    if (!csb->sense) {
	dm_ptr->hop_prsrv = FALSE;
	return;
    }

    if (dm_ptr->hop_rdctn) {
	printf("\n%%AT-DOMAIN: Hop preservation can not be enabled"
	       " : hop reduction is ON");
	return;
    }
    dm_ptr->hop_prsrv = TRUE;
}

/****************************************************************** 
 * Routine :      domain_start()
 *
 * Purpose :      Kicks off AT Domain process
 *
 * Called from :  
 */

static void
domain_start (void)
{
    static boolean domain_started = FALSE;

    /* Check if we already ran this */
    if (domain_started)
	return;

    if (at_debug(atalkdomain_debug, ATALK_NULL_MASK))
	buginf("\nAT-DOMAIN: Domain Process Activated");

    domain_started = TRUE;

    /*
     * Initialiaze global variables here 
     * so we will not lose them when restarting 
     * in the future releases of the OS 
     */

    atdomain_Initialize();
    
    LoopId = atalk_random(0xffff);

    mgd_timer_start(&domain_ZoneTimer, domain_ZoneInterval);
    
    mgd_timer_start(&domain_LoopTimer, domain_LoopInterval);

    mgd_timer_start(&domain_GCTimer,   domain_GCInterval);

    domain_pid = process_create(domain_process, "AT Domain", NORMAL_STACK,
				PRIO_NORMAL);
}

/****************************************************************** 
 * Routine :      domain_process()
 *
 * Purpose :      Send ZIP periodic ZIP requests, ZIP GC
 *
 * Called from :  cforked off the domain_start
 */

static process
domain_process (void)
{
    ulong major, minor;
    mgd_timer *expired_timer;
    
    /* wait for system initialization */
    process_wait_on_system_init();

    process_watch_mgd_timer(&domain_timer, ENABLE);

    while (TRUE) {
	process_wait_for_event();

	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	    case TIMER_EVENT:
		while (mgd_timer_expired(&domain_timer)) {
		    expired_timer = mgd_timer_first_expired(&domain_timer);
		    switch (mgd_timer_type(expired_timer)) {
		    case ATTIMER_DOMAIN_ZONE:
			Domain_SendQueries();   /* Send any outstanding ZIPs */
			mgd_timer_start(expired_timer, domain_ZoneInterval);
			break;
		    case ATTIMER_DOMAIN_LOOP:
			Domain_Loop_Process();  /* Send all outstanding Loop packets */
			mgd_timer_start(expired_timer, domain_LoopInterval);
			break;
		    case ATTIMER_DOMAIN_GC:
			Domain_GC();
			mgd_timer_start(expired_timer, domain_GCInterval);
			break;
		    default:
			mgd_timer_stop(expired_timer);
			break;
		    }
		}
		break;
	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}
    }
}

/*******************************************************************
 *
 * Routine :       atdomain_RemapNBP(pak, direction)
 *
 * Purpose :       Remap NBP tuples
 *                 NBP tuples are the only DDP (besides RTMP and ZIP)
 *                 packets that supposed to have a net numbers inside
 *
 * Called from :   atdomain_DDPoutProcess
 */
static void
atdomain_RemapNBP (paktype *pak, int direction)
{
    ushort   nbp_net, remap_net;
    atalkidbtype  *idb;
    uchar    *nbp_ptr;
    nbptype  *nbp;
    nbptuple *tp;
    ushort   tuple_count;
    at_SubDomain *sd = NULL;


    if (direction == IN) {
	idb  = atalk_getidb(pak->if_input);
	if (idb == NULL)
	    return;
	if (!idb->atalk_domain || !idb->atalk_dminfo)
	    return;

	sd = idb->atalk_dminfo->in;
    } else if (direction == OUT) {
	idb  = atalk_getidb(pak->if_output);
	if (idb == NULL)
	    return;
	if (!idb->atalk_domain || !idb->atalk_dminfo)
	    return;

	sd = idb->atalk_dminfo->out;
    }
    
    if (!sd)
	return;

    nbp     = (nbptype *)pak->transport_start;
    tp      = (nbptuple *)((uchar *)nbp + sizeof(nbptype));

    if (!nbp)
	return;

    tuple_count = nbp_get_count(nbp);

    while (tuple_count-- > 0) {
	
	nbp_net = GETSHORT(tp);
	remap_net = atdomain_FindForwardRemap(sd, nbp_net);

	if (remap_net)
	    PUTSHORT((uchar *)tp, remap_net);

	/* Get the pointer to the next tuple */
	nbp_ptr  = (uchar *)tp;
        nbp_ptr += NBPTUPLESIZE;

	/* Object field */
	nbp_ptr += *nbp_ptr;
	nbp_ptr++;
	
	/* Type field */
	nbp_ptr += *nbp_ptr;
	nbp_ptr++;
	
	/* Zone field */
	nbp_ptr += *nbp_ptr;
	nbp_ptr++;
	
	tp = (nbptuple *)nbp_ptr;
    }
}

/****************************************************************** 
 * Routine :      LegitNet(net)
 *
 * Purpose :      Sanity check if this is a legit AT net
 *
 * Called from :  
 */

static boolean
LegitNet (ushort net)
{
    return (!(net == ATALK_CABLE_ALLNETS || net >= ATALK_STARTUP_START));
}

/****************************************************************** 
 * Routine :      ConnectedNet(net)
 *
 * Purpose :      Returns TRUE if net is a Connected Net
 *
 * Called from :  
 */

/* Check if this (net) is a directly connected one */
static boolean
ConnectedNet (ushort net)
{
    boolean RETURN;
    atalk_rdb_t *r;
    rbTree *tree;

    if (net == 0)
	return (FALSE);

    tree = atroute_GetTree();
    r = atroute_FindRoute(tree, net, FALSE);
    if (r == NULL)
	return (FALSE);
    RETURN = atalk_net_connected(r);
    ReleaseRoute(r);
    return (RETURN);
}

/****************************************************************** 
 * Routine :      Remap_PutNewEntry(sd,Start,End,dist,neighbor,Remap)
 *
 * Purpose :      Creates a new remap entry and fills it in.
 *                Returns Pointer to created remap.
 *
 * Called from :  at_RemapProcess()
 */

static at_RemapTable 
*Remap_PutNewEntry (at_SubDomain *sd, ushort Start, ushort End, uchar dist,
		    neighborentry *neighbor, ushort Remap)
{
    at_RemapTable *remap_ptr;

    /* Time to create a new remap entry */
    remap_ptr = malloc(sizeof(at_RemapTable));
    if (remap_ptr == NULL) {
	errmsg(AT_ERR_NOMEM, "AT new remap ", __LINE__, __FILE__);
        return (NULL);
    }
    remap_ptr->link.flink   = NULL;
    remap_ptr->domain_start = Start;
    remap_ptr->domain_end   = End;
    remap_ptr->remap_start  = Remap;
    if (End)
	remap_ptr->remap_end      = Remap + End - Start;
    else
	remap_ptr->remap_end      = 0;
    remap_ptr->dist         = dist;
    remap_ptr->status       = UnAssigned;
    remap_ptr->neighbor     = neighbor;
    /* Let's not forget to protect this neighbor
     * while we are going to use it 
     */
    atmaint_ProtectNeighbor(neighbor);
    
    remap_ptr->ZoneListHdr  = NULL;
    remap_ptr->zonecnt      = 0;
    remap_ptr->busy         = 0;
    remap_ptr->protocol_links = 0;

    if (sd->RemapListHdr == NULL) {
	remap_ptr->link.blink = NULL;
	sd->RemapListHdr  = remap_ptr;
	sd->RemapListTail = remap_ptr;
    } else {
	at_insque(&remap_ptr->link, &sd->RemapListTail->link);
	sd->RemapListTail = remap_ptr;
    }
    return (remap_ptr);
}

/****************************************************************** 
 * Routine :      Add_to_UnZippedList(n_ptr,remap_ptr)
 *
 * Purpose :      Adds new remap to the UnZipped List for 
 *                this domain neighbor n_ptr
 *
 * Called from :  Remap_GotoState()
 */

static void 
Add_to_UnZippedList (DomainNeighbor *n_ptr,at_RemapTable *remap_ptr)
{

    GeneralDomainList *d_ptr, *new_d;
    sys_timestamp     now;


    GET_TIMESTAMP(now);
    
    d_ptr = n_ptr->UnZippedHdr;

    while (d_ptr) {
	if (d_ptr->remap_ptr == remap_ptr)
	    return;
        if (d_ptr->next == NULL)
	  break;
	d_ptr = d_ptr->next;

    }

    new_d = malloc(sizeof(GeneralDomainList));
    if (!new_d) {
	errmsg(AT_ERR_NOMEM, "AT new UnZipped element", __LINE__, __FILE__);
	return;
    }
    new_d->remap_ptr = remap_ptr;
/*    ProtectRemap(remap_ptr);*/
    new_d->retries   = 0;
    new_d->timer     = now;
    new_d->next      = NULL;

    TIMER_START(new_d->timer, ZIP_QUERY_INTERVAL);

    /* OK, see if the unzipped list is empty */
    if (n_ptr->UnZippedHdr == NULL) 
	n_ptr->UnZippedHdr = new_d;
    else  /* Not empty */
	d_ptr->next = new_d;

    NumOfUnzippedNets++;
}

/****************************************************************** 
 * Routine :      Domain_ZIP_GC()
 *
 * Purpose :      Makes sure that RTMP remap entry is 
 *                not stuck in the UnZipped State
 *
 * Called from :  domain_process()
 */

static void 
Domain_ZIP_GC (void)
{

    DomainNeighbor  *n_ptr;
    GeneralDomainList *d_ptr,*d_prev;
    at_RemapTable   *remap_ptr;

    n_ptr = DomainNeighborHdr;
    while (n_ptr) {
	d_ptr = n_ptr->UnZippedHdr;
	d_prev = d_ptr;
	while (d_ptr) {
	    d_prev = d_ptr;
	    d_ptr = d_ptr->next;
	    if (d_prev->retries > DOMAIN_MAX_ZIP_RETRIES) {
		remap_ptr = d_prev->remap_ptr;
/*		UnProtectRemap(remap_ptr);*/
		RemoveUnZippedEntry(remap_ptr);
		Remap_GotoState(remap_ptr,Dead);
	    }
	}
	n_ptr = n_ptr->next;
    }
}

/****************************************************************** 
 * Routine :      Domain_GC()
 *
 * Purpose :      Domain Garabage Collection
 *
 * Called from :  domain_process()
 */
static void 
Domain_GC (void)
{
    at_Domain    *dm_ptr = DomainListHdr;
    at_SubDomain *sd;

    Domain_ZIP_GC();        /* Garbage collection of Domain ZIPs*/    

    while (dm_ptr) { /*Go through all domains */
	sd = dm_ptr->in;
	Delete_DeadRemaps(sd);
	dm_ptr = (at_Domain *)dm_ptr->link.flink;
    }
}

/****************************************************************** 
 * Routine :      Delete_DeadRemaps(sd)
 *
 * Purpose :      Try to remove all the Dead remaps
 *                from this inbound subdomain.
 *
 * Called from :  domain_process()
 */

static void 
Delete_DeadRemaps (at_SubDomain *sd)
{
    at_RemapTable *remap_ptr, *prev_remap;

    if (!sd) 
	return;

    remap_ptr = sd->RemapListHdr;

    while (remap_ptr) {
	prev_remap = remap_ptr;
	remap_ptr  = (at_RemapTable *)remap_ptr->link.flink;
	if (prev_remap->status == Dead &&
	   !prev_remap->busy  &&
	   !prev_remap->protocol_links) {
	    atdomain_DeleteDomainRoute(sd->domain_number,
				       prev_remap->remap_start,
				       prev_remap->remap_end);
	}
    }
}

/****************************************************************** 
 * Routine :      Domain_Loop_Process()
 *
 * Purpose :      Domain Loop Queue Processing
 *
 * Called from :  domain_process()
 */
static void 
Domain_Loop_Process (void)
{
    GeneralDomainList *l_ptr, *l_prev;
    at_RemapTable     *remap_ptr;

    l_ptr = LoopListHdr;
    l_prev= l_ptr;
    while (l_ptr) {

	if (l_ptr->retries >= LOOP_MAX_RETRIES) {
	    /* OK,assume no loop here */
	    remap_ptr = l_ptr->remap_ptr;
	    /* Go to Established :
	     *  Insert it to the main routing table 
	     */
	    InsertDomainRoute(remap_ptr);
	    /* Now let's delete this loop element */
	    if (l_ptr == LoopListHdr) {
		LoopListHdr = l_ptr->next;
		l_ptr = l_ptr->next;
		free(l_prev);
		l_prev = l_ptr;
	    } else {
		l_prev->next = l_ptr->next;
		free(l_ptr);
		l_ptr = l_prev->next;
	    }
	} else {
	    SendLoopPackets(l_ptr);
	    l_prev = l_ptr;
	    l_ptr  = l_ptr->next;
	}
    }
}

/****************************************************************** 
 * Routine :      Domain_SendQueries()
 *
 * Purpose :      Goes through the UnZipped List of 
 *                all domain neighbors and sends 
 *                periodic queries (ZIPs) to all unzipped
 *                remaps
 *
 * Called from :  domain_process()
 */

static void 
Domain_SendQueries (void)
{
    DomainNeighbor  *n_ptr;
    ushort          pcnt;
    int             stat = 0;


    for (n_ptr = DomainNeighborHdr; n_ptr != NULL; n_ptr = n_ptr->next) {
	
	if (n_ptr->neighbor->state != neighborUp)
	    continue;
	    
	if (TIMER_RUNNING_AND_SLEEPING(n_ptr->neighbor->qtimer))
	    continue;
	TIMER_STOP(n_ptr->neighbor->qtimer);

	pcnt = 0;
	
	while (pcnt < NEIGHBOR_PACKET_LIMIT) {
	    if (n_ptr->neighbor->reachable_by & ATALK_RTMP_ENABLED)
		stat = domain_SendRTMPNeighborQueries(n_ptr);
	    if (stat != 0)
		pcnt++;
	    
	    if (stat <= 0)
		break;
	}
    }
}

/****************************************************************** 
 * Routine :      domain_SendRTMPNeighborQueries(n_ptr)
 *
 * Purpose :      Send ZIP requests to the RTMP peer
 *
 * Called from :  Domain_SendQueries()
 */

static int 
domain_SendRTMPNeighborQueries (DomainNeighbor *n_ptr)
{


    paktype      *pak;
    ziptype      *zpkt;
    uchar        *zip;
    int           qcnt;
    sys_timestamp timeo;
    int           bytes;
    GeneralDomainList *d_ptr;

    if (!n_ptr)
	return (FALSE);

    if (!n_ptr->UnZippedHdr)
	return (FALSE);

    pak = atalk_pak_create(n_ptr->neighbor->port, n_ptr->neighbor->address,
			   ATALK_SOCK_ZIP, ATALK_SOCK_ZIP, DDP_TYPE_ZIP, 
			   DDP_MAX_DATABYTES);
    if (pak == NULL) {
	if (   at_debug(atalkerr_debug, ATALK_INTERFACE_MASK, n_ptr->neighbor->port->phys_idb)
	    || at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, n_ptr->neighbor->port->phys_idb)) {
	    buginf("\nAT: domain_SendRTMPNeighborQueries getbuffer() failure");
	}
	atalk_stat[ATALK_NO_BUFFERS]++;
	return (FALSE);
    }
    pak->if_output = n_ptr->neighbor->port->phys_idb;
    zpkt = (ziptype *)pak->transport_start;
    zpkt->cmd = ZIP_Query;
    zip = ((uchar *)zpkt)+ZIPTYPEBYTES;
    bytes = pak->atalk_datalen - ZIPTYPEBYTES;
    qcnt = 0;
    timeo = n_ptr->neighbor->qtimer;
   
    d_ptr = GetNextUnqueried(n_ptr);

    while ((d_ptr != NULL) && (qcnt++ < NEIGHBOR_QUERIES_PER_PACKET)) {
	
	PUTSHORT((uchar *)zip, d_ptr->remap_ptr->domain_start);
	zip   += DDPNETBYTES;
	bytes -= DDPNETBYTES;
	d_ptr->retries++;
	d_ptr  = GetNextUnqueried(n_ptr);  
    }
    
    if (at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, n_ptr->neighbor->port->phys_idb)) {
	buginf("\nAT: domain_SendRTMPNeighborQueries, sending %d queries to %#a",
	       qcnt, n_ptr->neighbor->address);
    }
    atalk_pak_setlength(pak,ZIPTYPEBYTES + (DDPNETBYTES * qcnt));
    zpkt->cnt = qcnt;
    atalk_DDPcksum(pak);
    if (atalk_send(pak)) {
	n_ptr->neighbor->qcount += qcnt;
	n_ptr->neighbor->qtimer  = timeo;
	atalk_stat[ATALK_ZIP_OUT]++;
	return (last_queried_net == 0) ? -1 : 1; /* Indicate if scan completed */
    }
    return (FALSE);
}

/****************************************************************** 
 * Routine :      GetNextUnqueried(n_ptr)
 *
 * Purpose :      Returns next Unqueried Domain neighbor
 *
 * Called from :  domain_SendRTMPNeighborQueries()
 */

static GeneralDomainList *
GetNextUnqueried (DomainNeighbor *n_ptr)
{

    GeneralDomainList *d_ptr;

    if (!n_ptr) {
	last_queried_net = 0;
	return (NULL);
    }
	
    if (last_queried_net == 0) {
	if (n_ptr->UnZippedHdr)
	    last_queried_net = n_ptr->UnZippedHdr->remap_ptr->domain_start;
	else
	    last_queried_net = 0;
	return (n_ptr->UnZippedHdr);
    }

    d_ptr = n_ptr->UnZippedHdr;
    
    while (d_ptr) {
	if (d_ptr->remap_ptr->domain_start == last_queried_net) {
	    if (d_ptr->next) {
		last_queried_net = d_ptr->next->remap_ptr->domain_start;
		return (d_ptr->next);
	    } else {
		last_queried_net = 0;
	    }
	}
	d_ptr = d_ptr->next;
    }
    return (NULL);
}



/****************************************************************** 
 * Routine :      FindGeneralDomainElement(hdr,net)
 *
 * Purpose :      Find a General Domain element with 
 *                a domain net = net in the List hdr.
 *
 * Called from :  atdomain_ZipXReply, atdomain_ZIPReply
 */

static GeneralDomainList * 
at_FindGeneralDomainElement (GeneralDomainList *hdr, ushort net)
{
    GeneralDomainList *d_ptr = hdr;
    at_RemapTable *remap_ptr;


    while (d_ptr) {
	remap_ptr = d_ptr->remap_ptr;
	if (remap_ptr && remap_ptr->domain_start == net)
	    return(d_ptr);
	d_ptr =  d_ptr->next;
    }
    return (NULL);
}

/****************************************************************** 
 * Routine :      Domain_AddNewZone(remap_ptr,zone,len)
 *
 * Purpose :      Adds a new Domain_Zonelist element to remap
 *
 * Called from :  
 */

static void 
Domain_AddNewZone (at_RemapTable *remap_ptr, uchar *zone, uint len)
{
    Domain_ZoneList *zlist_ptr, *z_ptr;
    
    
    if (!remap_ptr)
	return;
    
    if (Find_DomainZone(remap_ptr, zone ,len))
	return;

    /* Now let's create a new zone element and insert it */

    z_ptr = malloc(sizeof(Domain_ZoneList));
    if (!z_ptr) {
	errmsg(AT_ERR_NOMEM, "AT new zone element", __LINE__, __FILE__);
	return;
    }
    
    bcopy(zone, z_ptr->name, len);
    z_ptr->len  = len;
    z_ptr->next = NULL;


    zlist_ptr = remap_ptr->ZoneListHdr;
    if (zlist_ptr == NULL) {
	/* This zone will be the default one then */
	remap_ptr->ZoneListHdr = z_ptr;
	return;
    }

    while (zlist_ptr->next) {
	zlist_ptr = zlist_ptr->next;
    }

    zlist_ptr->next = z_ptr;
}

/****************************************************************** 
 * Routine :      Find_DomainZone(remap_ptr,zone,len)
 *
 * Purpose :      Finds a Domain_ZoneList element in the remap
 *
 * Called from :  
 */

static Domain_ZoneList *
Find_DomainZone (at_RemapTable *remap_ptr, uchar *zone, uint len)
{
    Domain_ZoneList *z_ptr;

    if (!remap_ptr)
	return (NULL);

    z_ptr = remap_ptr->ZoneListHdr;

    while (z_ptr) {
	if (atnbp_namematch(zone, z_ptr->name, len, z_ptr->len, NBP_ZONE_WILD))
	    return (z_ptr);

	z_ptr = z_ptr->next;
    }
    return (NULL);
}

/****************************************************************** 
 * Routine :      GetAvailRemap(sd,RangeSize)
 *
 * Purpose :      Returns the Start of the Available Range
 *                with this RangeSize in this subdomain
 *
 * Called from :  
 */

static ushort 
GetAvailRemap (at_SubDomain *sd, ushort RangeSize)
{
    at_AvailableRemap *av_ptr;
    ushort ReturnRange = 0;

    av_ptr = sd->AvRemapHdr;
    
    while (av_ptr) {
	if ((av_ptr->End - av_ptr->Start + 1) >= RangeSize) {
	    ReturnRange = av_ptr->Start;
	    if ((av_ptr->End - av_ptr->Start + 1) == RangeSize) 
		at_DeleteAvRemapCell(sd, av_ptr);
	    else
		av_ptr->Start += RangeSize;

	    return (ReturnRange);
	} else {
	    av_ptr = (at_AvailableRemap *)av_ptr->link.flink;
	}
    }
    return (at_ReshuffleRemapList(sd, RangeSize));
}

/****************************************************************** 
 * Routine :      at_ReshuffleRemapList(sd,RangeSize)
 *
 * Purpose :      If GetAvailRemap was unsuccessful,then 
 *                at_ReshuffleRemapList is called.
 *                It goes through the Available List
 *                and tries to concatinate all the 
 *                adjesent holes.
 *
 * Called from :  
 */

static ushort 
at_ReshuffleRemapList (at_SubDomain *sd, ushort RangeSize)
{
    at_AvailableRemap *root_ptr, *av_ptr;
    boolean DeleteEntry;
    boolean FOUND = FALSE;
    boolean RemapNet = 0;

    if (at_debug(atalkremap_debug, ATALK_NULL_MASK))
	buginf("\nAT-REMAP: ReshuffleRemapList for subdomain %d ",sd->domain_number);


    root_ptr = sd->AvRemapHdr;
    while (root_ptr) {
	
	av_ptr = (at_AvailableRemap *)root_ptr->link.flink;
	DeleteEntry   = FALSE;
	
	while (av_ptr) {
	    if (av_ptr->Start == (root_ptr->End + 1)) {
		
		DeleteEntry   = TRUE;
		if ((av_ptr->End - root_ptr->Start + 1) >= RangeSize) 
		    FOUND = TRUE;

		av_ptr->Start = root_ptr->Start;
		break;
	    }
	    if (av_ptr->End == (root_ptr->Start - 1)) {
		DeleteEntry   = TRUE;
		if ((root_ptr->End - av_ptr->Start + 1) >= RangeSize)
		    FOUND = TRUE;

		av_ptr->End = root_ptr->End;
		break;
	    }
	    
	    av_ptr = (at_AvailableRemap *)av_ptr->link.flink;
	}

	root_ptr  = (at_AvailableRemap *)root_ptr->link.flink;

	if (DeleteEntry) {
	    /* Delete root_ptr */
	    at_DeleteAvRemapCell(sd,(at_AvailableRemap *)root_ptr->link.blink);
	    if (FOUND) {
		RemapNet = av_ptr->Start;
	        if((av_ptr->End - av_ptr->Start + 1) == RangeSize) 
		    at_DeleteAvRemapCell(sd, av_ptr);
	        else
		    av_ptr->Start += RangeSize;

		break;
	    }
	} 
    }
    return (RemapNet);
}

/****************************************************************** 
 * Routine :      at_FindAvRemapCell(sd,Start,End)
 *
 * Purpose :      Finds Available Remap cell [Start,End] 
 *                in this subdomain
 *
 * Called from :  
 */

static boolean 
at_FindAvRemapCell (at_SubDomain *sd_ptr, ushort Start, ushort End)
{
    at_AvailableRemap *av_ptr;
    
    av_ptr = sd_ptr->AvRemapHdr;

    while (av_ptr) {
	if (av_ptr->Start == Start && av_ptr->End == End)
	    return (TRUE);
	av_ptr = (at_AvailableRemap *)av_ptr->link.flink;
    }
    return (FALSE);
}

/****************************************************************** 
 * Routine :      ProtectRemap(remap_ptr)
 *
 * Purpose :      Protects this remap
 *
 * Called from :  
 */

static void 
ProtectRemap (at_RemapTable *remap_ptr)
{
    remap_ptr->busy++;
}

/****************************************************************** 
 * Routine :      UnProtectRemap(remap_ptr)
 *
 * Purpose :      Unprotects this remap
 *
 * Called from :  
 */

static void 
UnProtectRemap (at_RemapTable *remap_ptr)
{
    if (remap_ptr->busy)
	remap_ptr->busy--;
}

/****************************************************************** 
 * Routine :      atdomain_DeleteRemap(sd,remap_ptr,force_remove)
 *
 * Purpose :      Removes remap from subdomain
 *
 * Called from :  
 */

boolean atdomain_DeleteRemap (at_SubDomain *sd, at_RemapTable *remap_ptr,
			      boolean force)
{
    if (!sd)
	return (FALSE);

    if (remap_ptr == NULL)
	return (FALSE);
    
    /*
     * make sure that we don't delete it if some protocols still need it 
     * unless this is a force remove
     */
    if (!force && remap_ptr->protocol_links) {
	return (FALSE);
    }

    /* Now we should make sure that 
     * this remap entry is deleted from everywhere 
     * The only place it could be is in Domain Neighbor List, so make sure 
     * that we don't have any pointers there.
     * Also, show_appletalk_remap might hold it as well
     */
    if (Protected(remap_ptr)) {
	/* OK, delete it later */
	Remap_GotoState(remap_ptr, Dead);
	return (FALSE);
    }
    
    /*
     * Now, before we delete it 
     * we should release this remap_ptr from 
     * all the queues (like loop or unzipped queues)
     */
    ReleaseRemap(remap_ptr);

    /* And now let's be rid of it */

    if (at_debug(atalkremap_debug, ATALK_NULL_MASK))
	buginf("\nAT-REMAP: Remap for net %d %s subdomain %d has been deleted",
	       remap_ptr->domain_start,
	       (sd->flags & inbound_r) ?  "inbound" : "outbound",
	       sd->domain_number);

    if (remap_ptr == sd->RemapListHdr &&
       remap_ptr == sd->RemapListTail) {
	sd->RemapListHdr  = NULL;
	sd->RemapListTail = NULL;
	DeleteAvRemapList(sd);
	atdomain_CreateAvList(sd);
	atmaint_UnprotectNeighbor(remap_ptr->neighbor);
	free(remap_ptr);
	return (TRUE);
    }
    if (remap_ptr == sd->RemapListHdr) {
	sd->RemapListHdr = (at_RemapTable *)sd->RemapListHdr->link.flink;
	sd->RemapListHdr->link.blink = NULL;
	atmaint_UnprotectNeighbor(remap_ptr->neighbor);
	free(remap_ptr);
	return (TRUE);
    }

    if (remap_ptr == sd->RemapListTail) {
	sd->RemapListTail = (at_RemapTable *)sd->RemapListTail->link.blink;
	sd->RemapListTail->link.flink = NULL;
	atmaint_UnprotectNeighbor(remap_ptr->neighbor);
	free(remap_ptr);
	return (TRUE);
    }

    at_remque(&remap_ptr->link);
    atmaint_UnprotectNeighbor(remap_ptr->neighbor);
    free(remap_ptr);

    return (TRUE);
}

/****************************************************************** 
 * Routine :      at_FindRemap(sd,Start,End)
 *
 * Purpose :      Finds a remap in the subdomain remapping table
 *                that has domain range [Start,End]
 * Called from :  
 */

static at_RemapTable *
at_FindRemap (at_SubDomain *sd, ushort Start, ushort End)
{

 
    at_RemapTable   *remap_ptr;
    
    if (!sd)
	return (NULL);

    remap_ptr = sd->RemapListHdr;

    while (remap_ptr) {
	if (atutil_RangesOverlap(remap_ptr->domain_start,
				 remap_ptr->domain_end, Start, End)) {
	    return (remap_ptr);
	}
	remap_ptr = (at_RemapTable *)remap_ptr->link.flink;
    }
    return (NULL);
}

/****************************************************************** 
 * Routine :      LockkAvCell(sd,Start,End)
 *
 * Purpose :      Locks up an available cell [Start,End]
 *                in subdomain
 *
 * Called from :  
 */
/* not currently used */
#if 0
static void LockAvCell(at_SubDomain *sd, ushort Start, ushort End)
{

    if (Start <= sd->remap_start) {
	if (End >= sd->remap_end) 
	    DeleteAvRemapList(sd);
	else 
	    if (End >= sd->remap_start)
		sd->AvRemapHdr->Start = End + 1;
    } else {
	sd->AvRemapHdr->End = Start - 1;
	if (End < sd->remap_end) 
	    CreateAvRemap(sd, End + 1, sd->remap_end);
    }
}
#endif

/****************************************************************** 
 * Routine :      atdomain_UnLockAvCell(sd,Start,End)
 *
 * Purpose :      Unlocks Available Cell [Start,End] from subdomain
 *
 * Called from :  
 */

void
atdomain_UnLockAvCell (at_SubDomain *sd, ushort Start, ushort End)
{
    if (Start <= sd->remap_start) {
	
	if (End >= sd->remap_end) 
	    CreateAvRemap(sd,sd->remap_start,sd->remap_end);
	else
	    if (End >= sd->remap_start)
		CreateAvRemap(sd,sd->remap_start,End);
    } else {
	if (End >= sd->remap_end) { 
	    if (Start <= sd->remap_end)
		CreateAvRemap(sd, Start, sd->remap_end);
	} else {
	    CreateAvRemap(sd, Start, End);
	}
    }
}

/****************************************************************** 
 * Routine :      at_PutAvRemapCell(sd,Start,End)
 *
 * Purpose :      Creates a new Available cell [Start,End] in 
 *                subdomain
 *
 * Called from :  
 */

static void 
at_PutAvRemapCell (at_SubDomain *sd, ushort Start, ushort End)
{
    at_AvailableRemap *av_ptr;


    if (!sd)
	return;

    if ( !at_RemapCellSanityCheck(sd, Start, End))
	return;
    
    if (at_debug(atalkremap_debug, ATALK_NULL_MASK))
	buginf("\nAT-REMAP: Creating new Available Cell [%d-%d] for %s subdomain %d",
	       Start, End, 
	       (sd->flags & inbound_r) ? "inbound" : "outbound",
	       sd->domain_number);

    av_ptr = malloc(sizeof(at_AvailableRemap));
    
    if (av_ptr == NULL) {
	errmsg(AT_ERR_NOMEM, "AT new Avail element", __LINE__, __FILE__);
	return;
    }
    av_ptr->link.flink  = NULL;
    av_ptr->link.blink  = NULL;
    av_ptr->Start = Start;
    av_ptr->End   = End;
    
    if ( sd->AvRemapHdr == NULL) {
	sd->AvRemapHdr  = av_ptr;
	sd->AvRemapTail = av_ptr;
    } else {
	at_insque(&av_ptr->link, &sd->AvRemapTail->link);
	sd->AvRemapTail = av_ptr;
    }
}

/****************************************************************** 
 * Routine :      at_RemapCellSanityCheck(sd,Start,End)
 *
 * Purpose :      Sanity Check for this will-be-AvRange
 *
 * Called from :  
 */

static boolean
at_RemapCellSanityCheck (at_SubDomain *sd, ushort Start, ushort End)
{
    if (sd->remap_start == 0)
	return (FALSE);
    if (Start > End )
	return (FALSE);
    if ((Start < sd->remap_start) || (End   > sd->remap_end))
	return (FALSE);
    return (TRUE);
}

/****************************************************************** 
 * Routine :      at_DeleteAvRemapCell(sd,cell)   
 *
 * Purpose :      Removes cell from the Available List of subdomain
 *
 * Called from :  
 */
    
static void 
at_DeleteAvRemapCell (at_SubDomain *sd, at_AvailableRemap *cell)
{
    if (at_debug(atalkremap_debug, ATALK_NULL_MASK))
	buginf("\nAT-REMAP: DeleteAvailableCell for subdomain %d , cell = [%d - %d]",
	       sd->domain_number, cell->Start, cell->End);

    if(cell == sd->AvRemapHdr && cell == sd->AvRemapTail) {
	sd->AvRemapHdr  = NULL;
	sd->AvRemapTail = NULL;
    }
    if (cell == sd->AvRemapHdr) {
	sd->AvRemapHdr = (at_AvailableRemap *)cell->link.flink;
	sd->AvRemapHdr->link.blink = NULL;
    }
    if (cell == sd->AvRemapTail) {
	sd->AvRemapTail   = (at_AvailableRemap *)cell->link.blink;
	sd->AvRemapTail->link.flink = NULL;
    }

    at_remque(&cell->link);
    free(cell);
}

/****************************************************************** 
 * Routine :      atdomain_FindDomain_by_number(number)
 *
 * Purpose :      Finds a Domain by the domain number
 *
 * Called from :  
 */
	
at_Domain *
atdomain_FindDomain_by_number (int domain_number)
{
    at_Domain *dm_ptr = DomainListHdr;

    while (dm_ptr) {
	if (dm_ptr->number == domain_number)
	    return (dm_ptr);
	dm_ptr = (at_Domain *)dm_ptr->link.flink;
    }
    return (NULL);
}

/****************************************************************** 
 * Routine :      Collect_DomainNeighbors(idb)
 *
 * Purpose :      Makes all the neighbors pointing 
 *                to this idb as domain neighbors
 *
 * Called from :  
 */

static void 
Collect_DomainNeighbors (atalkidbtype *idb)
{

    neighborentry *n;

    n = atmaint_GetNextNeighbor(NULL);
    while (n) {
	if (n->port == idb) {
	    atdomain_InsertNeighbor(n);
	}
	n = atmaint_GetNextNeighbor(n);
    }

}

static dqueue_t *
at_insque (dqueue_t *elem, dqueue_t *prev)
{
    elem->flink = prev->flink;
    elem->blink = prev;
    if (prev->flink)
	prev->flink->blink = elem;
    prev->flink = elem;

    return elem;
}

static dqueue_t *
at_remque (dqueue_t *elem)
{
    if (!elem)
	return NULL;
    if (elem->blink)
	elem->blink->flink = elem->flink;
    if (elem->flink)
	elem->flink->blink = elem->blink;

    return (elem);
}

/****************************************************************** 
 * Routine :      Remap_GotoState(remap,State)
 *
 * Purpose :      Remap State Handler
 *
 * Called from :  
 */

static void
Remap_GotoState (at_RemapTable *remap_ptr, RemapStatus State)
{
    DomainNeighbor *n_ptr;

    if (!remap_ptr)
	return;

    switch(State) {
      case UnZipped:
	n_ptr = GetDomainNeighbor(remap_ptr->neighbor);

	if (!n_ptr)
	    return;
	/* OK, now we should put this remap entry into the UnZipped List */
	Add_to_UnZippedList(n_ptr, remap_ptr);
	remap_ptr->status = UnZipped;
	break;

      case Established:
	remap_ptr->status = Established;
	break;

      case Dead:
	remap_ptr->status = Dead;
	break;

      case Suspect:
	remap_ptr->status = Suspect;
	break;

      default:
	break;
    }
}

/****************************************************************** 
 * Routine :      Delete_idbs_fromDomain(dm_ptr)
 *
 * Purpose :      Gets rid of IDBList
 *
 * Called from :  
 */

static void
Delete_idbs_fromDomain (at_Domain *dm_ptr)
{

    domain_idbList *i_ptr ,*i_prev;

    i_ptr  = dm_ptr->idbHdr;

    while (i_ptr) {
	i_ptr->idb->atalk_domain = 0;
	i_ptr->idb->atalk_dminfo = NULL;
	if (i_ptr->active) {
	    if (atalkif_AURPTunnel(i_ptr->idb)) 
		reg_invoke_atalk_aurpif_clear(i_ptr->idb->phys_idb); 
/*	    aurp_config_tunnel(i_ptr->idb, TRUE, AURP_IP_DI); */
	    atalkif_RestartPort(i_ptr->idb);
        }
	i_prev = i_ptr;
	i_ptr  = i_ptr->next;
	free(i_prev);
    }
}

/****************************************************************** 
 * Routine :      KeepFirstRemap(sd)
 *
 * Purpose :      For show commands : locks remap entry
 *
 * Called from :  
 */

static at_RemapTable *
KeepFirstRemap (at_SubDomain *sd)
{
    if (!sd || !sd->RemapListHdr)
	return (NULL);
    
    ProtectRemap(sd->RemapListHdr);
    return (sd->RemapListHdr);
}

/****************************************************************** 
 * Routine :      KeepNextRemap(sd,remap)
 *
 * Purpose :      For show commands : Keeps next remap
 *
 * Called from :  
 */

static at_RemapTable *
KeepNextRemap (at_SubDomain *sd, at_RemapTable *prev)
{
    at_RemapTable *remap_ptr;

    if (!sd)
	return (NULL);
    if (!prev)
	return (NULL);

    UnProtectRemap(prev);
    
    remap_ptr = (at_RemapTable *)prev->link.flink;

    if (remap_ptr)
	ProtectRemap(remap_ptr);
    
    return (remap_ptr);
}


/****************************************************************** 
 * Routine :      InsertDomainRoute(remap_ptr)
 *
 * Purpose :      Puts this remap to the main routing 
 *                and zone tables.
 *                Goes to Established
 *
 * Called from :  
 */

static void
InsertDomainRoute (at_RemapTable *remap_ptr)
{
    if (!remap_ptr)
	return;

    /* Add route */
    Domain_AddRemapRoute(remap_ptr);
    /* Add zone (list) */
    Domain_AddRemapZone(remap_ptr);
	
    /* Change the status */
    Remap_GotoState(remap_ptr, Established); 
}

/****************************************************************** 
 * Routine :      Validate_Range(Start,End,dir)
 *
 * Purpose :      Validates remapping range [Start;End]
 *
 * Called from :  
 */

static boolean
Validate_Range (ushort Start, ushort End, int dir)
{
    atalk_rdb_t   *r;
    rbTree        *tree;
    at_Domain *dm_ptr = DomainListHdr;


    if (Start > End) {
	printf("\n%% Illegal AppleTalk range");
	return (FALSE);
    }
    if (End >= ATALK_STARTUP_START) {
	printf("\n%%Illegal AppleTalk range");
	return (FALSE);
    }

    if (Start == 0) {
	printf("\n%%Illegal AppleTalk range");
	return (FALSE);
    }

    if (dir == OUT)
	return (TRUE);
	
    while (dm_ptr) {
	if (dm_ptr->in) {
		
	    if (atutil_RangesOverlap(dm_ptr->in->remap_start,
				     dm_ptr->in->remap_end,
				     Start, End)) {
		printf("\n%%AT-DOMAIN : Remapping range overlaps with the "
		       "inbound range of domain %d ", dm_ptr->number);
		return (FALSE);
	    }
	}
	dm_ptr = (at_Domain *)dm_ptr->link.flink;
    }

    /* Now check for the all nets in routing table */

    tree = atroute_GetTree();
    for (r = KeepFirstRoute(tree); r != NULL; r = KeepNextRoute(tree, r)) {
	if (atutil_RangesOverlap(r->dstrngstr,r->dstrngend,Start,End)) {
	    printf("\n%% Remap range overlaps with the net %d-%d",
		   r->dstrngstr, r->dstrngend);
	    ReleaseRoute(r);
	    return(FALSE);
	}
    }
    return (TRUE);
}

/****************************************************************** 
 * Routine :      atdomain_CreateAvList(sd)
 *
 * Purpose :      Creates Available List for this subdomain
 *
 * Called from :  
 */
			     
static void
atdomain_CreateAvList (at_SubDomain *sd)
{

    at_AvailableRemap *av_ptr;

    if (!sd)
	return;
    if (sd->remap_start == 0)
	return;
    
    av_ptr = malloc(sizeof(at_AvailableRemap));
    if (!av_ptr) {
	errmsg(AT_ERR_NOMEM, "AT new av element", __LINE__, __FILE__);
	return;
    }

    av_ptr->Start = sd->remap_start;
    av_ptr->End   = sd->remap_end;
    av_ptr->link.flink  = NULL;
    av_ptr->link.blink  = NULL;
    sd->AvRemapHdr = av_ptr;
    sd->AvRemapTail= av_ptr;
}

/****************************************************************** 
 * Routine :      PrintRemapStatus(r_status)
 *
 * Purpose :      Returns an ASCII string for this r_status
 *
 * Called from :  
 */

static char *
PrintRemapStatus (RemapStatus r_status)
{
    switch(r_status) {
      case UnAssigned :
	  return "UnAssigned";
      case UnZipped :
	  return "UnZipped";
      case Suspect :
	  return "Suspect";
      case Established :
	  return "Good";
      case Dead :
	  return "Bad";
      default :
	  return "Unknown";
   }
}

/****************************************************************** 
 * Routine :      atdomain_CheckForLoop(r,zone,len)
 *
 * Purpose :      Checks for a Domain Loop whenever a new zone is added
 *
 * Called from :  atzip_AddZone(at_zip.c)
 */

void
atdomain_CheckForLoop (atalk_rdb_t *r, uchar *zone, ulong len)
{
    ushort Start, End;
    int    suspect_domain;
    atalkidbtype *idb;
    
    if (!r)
	return;

    Start = r->dstrngstr;
    End   = r->dstrngend;

    idb = r->rpath.idb;
    if (!idb) {
	return;
    }
    suspect_domain = idb->atalk_domain;

    if (LoopSuspect(Start,End,zone,len,suspect_domain)) {
	/* OK, send Loop packets */

	FOR_ALL_ATIDBS(idb) {
	    if (idb->atalk_enabled && idb->atalk_operational) {
		SendLoop(idb, zone, len);
	    }
	}
    }
}

/****************************************************************** 
 * Routine :      ReleaseRemap(at_RemapTable *remap_ptr)
 *
 * Purpose :      Releases this remmap from all outstanding queues
 *
 * Called from :  atdomain_DeleteDomainRoute
 *
 */
static void
ReleaseRemap (at_RemapTable *remap_ptr)
{
    GeneralDomainList *l_ptr, *l_prev;

    /* OK, delete it from the Loop Queue */
    l_ptr = LoopListHdr;
    l_prev = l_ptr;

    while (l_ptr) {
	if (l_ptr->remap_ptr == remap_ptr) {
	    /* Delete it */
	    if (l_ptr == LoopListHdr)
		LoopListHdr = l_ptr->next;
	    else 
		l_prev->next = l_ptr->next;
	 
	    free(l_ptr);
	    break;
	}
	l_prev = l_ptr;
	l_ptr  = l_ptr->next;
    }
    /* And now Remove it from UnZipped Queue */
    RemoveUnZippedEntry(remap_ptr);
}

/****************************************************************** 
 * Routine :      atdomain_PurgeAllDomainRoutes(domain)
 *
 * Purpose :      Purges all routes from the main Routing Table
 *                which came from this domain.
 *
 *
 * Called from :  CleanSubDomain(),at_LoopForDomain()
 *
 */

static void
atdomain_PurgeAllDomainRoutes(int domain)
{
    atalk_rdb_t *r , *prev;
    rbTree	*tree;

    tree = atroute_GetTree();
    r    = KeepFirstRoute(tree);
    while (r) {
	prev = r;
	r = KeepNextRoute(tree,r);
	if (prev->rpath.idb && prev->rpath.idb->atalk_domain == domain) {
	    if(!(prev->rpath.metric.metric == AT_METRIC_FLOATING))
	       atroute_ClearPathsAndRoutes(prev);
	}
    }
}

void
atdomain_command (parseinfo *csb)
{
    if (csb == NULL)
	return;
    
    if (csb->nvgen) {
	switch (csb->which) {
	  case ATALK_DOMAIN_HOP_REDUCTION:
	  case ATALK_DOMAIN_HOP_PRESERVE:
	  case ATALK_DOMAIN:
	    atdomain_parse(csb);
	    break;
	    
	  case ATALK_DOMAIN_REMAP:
	    atdomain_remap_parse(csb);
	    break;
	    
	  default:
	    printf("\n%% NVGEN: unknown command - %s", csb->nv_command);
	    break;
	}
	return;
    }
    
    switch (csb->which) {
      case ATALK_DOMAIN_HOP_REDUCTION:
      case ATALK_DOMAIN_HOP_PRESERVE:
      case ATALK_DOMAIN:
	atdomain_parse(csb);
	break;
	
      case ATALK_DOMAIN_REMAP:
	atdomain_remap_parse(csb);
	break;
	
      default:
	buginf("\nAT: Unhandled configuration command");
	break;
    }
}

void
atdomain_if_command (parseinfo *csb)
{
    atalkidbtype	*idb;

    if (csb == NULL)
	return;

    idb = atalk_getidb(csb->interface);
    if (idb == NULL)
	return;
    
    if (csb->nvgen) {
	switch (csb->which) {
	  case ATALK_DOMAIN_GROUP:
	    if (idb->atalk_domain && idb->atalk_dminfo)
		nv_write(TRUE, "%s %d ",
			 csb->nv_command, idb->atalk_dminfo->number);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }
    
    switch (csb->which) {
      case ATALK_DOMAIN_GROUP:
	if (!idb->atalk_enabled) {
	    printf(atalk_badcmd, atalk_not_enabled);
	    return;
	}
	atdomain_group_parse(csb);
	break;
	
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

static void 
CheckForDomainRouter (void)
{
    at_Domain *dm_ptr = DomainListHdr;
    
    while (dm_ptr) {

	if(dm_ptr->active) {
	    atalk_DomainRouter_running = TRUE;
	    return;
	}
	dm_ptr = (at_Domain *)dm_ptr->link.flink;
    }
    atalk_DomainRouter_running = FALSE;

}

/*
 * atdomain_InsideInboundRemap
 *
 * Check if the specified cable-range is within any of the inbound ranges
 * currently configured.
 */
boolean
atdomain_InsideInboundRemap (ushort Start, ushort End)
{

    at_Domain *dm_ptr = DomainListHdr;

    while (dm_ptr) {
	if (dm_ptr->in && atutil_RangesOverlap(Start, End,
					       dm_ptr->in->remap_start,
					       dm_ptr->in->remap_end)) {
            return (TRUE);
        }
	dm_ptr = (at_Domain *)dm_ptr->link.flink;
    }
    return (FALSE);
}

/********************************************************************
 * Routine :       atdomain_QueueEvent(idbtype,Start,End)
 *
 * Purpose :       Informs us that AURP is using this route Start
 *
 * Called from :   queue_event(at_aurp.c)
 */

void
atdomain_QueueEvent (atalkidbtype* idb, ushort Start, ushort End)
{
    at_Domain *dest_domain = DomainListHdr;
    at_Domain *src_domain = NULL;
    at_SubDomain *sd;
    at_RemapTable *remap_ptr;

    if (!idb)
        return;

    /* For a domain with a AURP tunnel, protect the remap entry */
    src_domain = idb->atalk_dminfo;
    while (dest_domain) {
        if (dest_domain == src_domain) {
            dest_domain = (at_Domain *)dest_domain->link.flink;
            continue;
	}
	sd = dest_domain->out;
	if (sd) {
            if (atdomain_AURPTunnel(dest_domain)) {
		remap_ptr = at_FindRemap(sd,Start,0);
		if (remap_ptr) 
		    remap_ptr->protocol_links |= IN_USE_BY_AURP;
	    }
	}
	dest_domain = (at_Domain *)dest_domain->link.flink;
   }
}

/********************************************************************
 * Routine :       atdomain_DeletePathEntry(Start,End)
 *
 * Purpose :       Informs us that AURP is done with this route Start
 *
 * Called from :   aurp_DeletePathEntry(at_aurp.c)
 */

void
atdomain_DeletePathEntry (ushort Start, ushort End)
{
    at_Domain *dm_ptr = DomainListHdr;
    at_SubDomain *sd;
    at_RemapTable *remap_ptr;
    ushort domain_net;

    while (dm_ptr) {
	sd = dm_ptr->out;
	if (sd) {
            if (atdomain_AURPTunnel(dm_ptr)) {
		domain_net = atdomain_FindBackwardRemap(sd,Start);
		if (!domain_net) {
             	    dm_ptr = (at_Domain *)dm_ptr->link.flink;
                    continue;
		}
  	        remap_ptr = at_FindRemap(sd,domain_net,0);
		if (remap_ptr) {
		    remap_ptr->protocol_links &= ~IN_USE_BY_AURP;
		    atdomain_DeleteDomainRoute(0, remap_ptr->domain_start, 
					       remap_ptr->domain_end);
		}
	    }
	}
	dm_ptr = (at_Domain *)dm_ptr->link.flink;
   }
}

/********************************************************************
 * Routine :       atdomain_MatchAnyAURPTunnel(idb)
 *
 * Purpose :       Returns TRUE if domain matches any AURP tunnels'
 *                 domains.   
 *
 * Called from :   queue_event(at_aurp.c)
 */

boolean
atdomain_MatchAnyAURPTunnel (atalkidbtype* idb)
{
    at_Domain *dm_ptr = DomainListHdr;
    int domain = 0;

    /* Default domain don't match anything */
    if (idb && idb->atalk_dminfo && idb->atalk_dminfo->number)
        domain = idb->atalk_dminfo->number;
    else
        return (FALSE);

    while (dm_ptr) {
        if (atdomain_AURPTunnel(dm_ptr)) {
	    if (dm_ptr->number == domain)
                return (TRUE);
	}
	dm_ptr = (at_Domain *)dm_ptr->link.flink;
    }
    return (FALSE);
}

/********************************************************************
 * Routine :       atdomain_PathInAURPInbound(path)
 *
 * Purpose :       Returns TRUE if path conflicts with any
 *                 AURP tunnel's inbound remap range.
 *
 * Called from :   queue_event(at_aurp.c)
 */

boolean
atdomain_PathInAURPInbound (atalk_pdb_t* p)
{
    at_Domain *dm_ptr = DomainListHdr;

    while (dm_ptr) {
        if (atdomain_AURPTunnel(dm_ptr)) {
            if (dm_ptr->in && atutil_RangesOverlap(p->dstrngstr,p->dstrngend,
                dm_ptr->in->remap_start, dm_ptr->in->remap_end)) 
            return (TRUE);
	}
	dm_ptr = (at_Domain *)dm_ptr->link.flink;
    }
    return (FALSE);
}

/****************************************************************** 
 * Routine :      atdomain_AURPTunnel(dm_ptr)
 *
 * Purpose :      Returns TRUE if Domain has a AURP tunnel interface
 *
 * Called from :  atdomain_QueueEvent()
 */

boolean
atdomain_AURPTunnel (at_Domain *dm_ptr)
{
    domain_idbList *i_ptr;
    boolean        IsTunnel = FALSE;
    
    i_ptr = dm_ptr->idbHdr;
    while (i_ptr) {
	if (atalkif_AURPTunnel(i_ptr->idb)) {
	    IsTunnel = TRUE;
	    break;
	}
	i_ptr = i_ptr->next;
    }
    return (IsTunnel);
}
