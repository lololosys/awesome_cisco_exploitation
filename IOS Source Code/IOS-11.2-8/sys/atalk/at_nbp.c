/* $Id: at_nbp.c,v 3.5.6.6 1996/07/10 22:18:34 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_nbp.c,v $
 *------------------------------------------------------------------
 * atalk_nbp.c -- handle Appletalk name-binding protocol
 *
 * October 1988, Bill Palmer
 * ATp2 support, August 1990, David Edwards
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_nbp.c,v $
 * Revision 3.5.6.6  1996/07/10  22:18:34  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.5.6.5  1996/07/04  00:42:04  kleung
 * CSCdi61668:  AT: Router sends NBP LkUp instead of NBP FwdRq
 * Branch: California_branch
 *
 * Revision 3.5.6.4  1996/07/01  20:50:15  kleung
 * CSCdi61799:  Avoid crash when sending NBP LkUp to BAD route.
 * Branch: California_branch
 *
 * Revision 3.5.6.3  1996/06/28  00:06:19  kleung
 * CSCdi60845:  Crash in idb_get_swsb() called from atalk_check().
 * Branch: California_branch
 *
 * Revision 3.5.6.2  1996/03/23  01:27:07  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.5.6.1  1996/03/18  18:52:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.2  1996/03/16  06:26:52  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.2.1  1996/03/07  08:29:08  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5  1996/03/05  22:13:44  kleung
 * CSCdi49904:  atalk_upper vs atalk_cvu are equivalent
 *
 * Revision 3.4  1996/01/18  19:39:26  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/17  08:43:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:05:49  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:57:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:50:13  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/18  06:17:44  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.1  1995/06/07 20:08:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include <interface.h>
#include "packet.h"
#include "address.h"
#include "../ip/ip.h"
#include "../os/signal.h"
#include "atalk_private.h"
#include "at_nbp.h"

/*
 * Local Types.
 */
/*
 * structure of records stored in nveSequential relation:
 */
typedef struct nveEntry_ {
    symbol	nameSym, typeSym, zoneSym;
    ushort	net;
    uchar	socket;
    uchar	node;
    short	enumerator;
    boolean	tentative;		/* T if entry is pending reg'tion */
} nveEntry;

/*
 * Local storage.
 */
static watched_queue *atalknbpQ;

/* exported symbol table, database handles for module & module users: */
static symbol_table    nve_symtab;
database	nve_db;

/* WARNING:  these must be defined for SY_ and DB_ macros to function: */
#define MY_SYMTAB   nve_symtab
#define MY_DATABASE nve_db

/* id allocator for registration table: */
static iddesc nveidh;

/* "relation" names: */
static symbol nveSockets, nveEntities, nveSequential;


/* global scratch vars to save on stack space
 * WARNING:  these vars are used by several routines, don't nest!
 */
static key gkstor;
static nveEntry gtabstor;
static nveSearch gsearchstor;

static gccontext_t *nbp_gccontext;



/* definitions for at_err(): */
#define	DEBUGSWITCH (at_debug(atalknbp_debug, ATALK_NULL_MASK) \
                     || at_debug(appletalk_debug, ATALK_NULL_MASK))

static boolean	atnbp_brrq(paktype*,nbpOps*,boolean);
static void	atnbp_send_fwdreq(paktype* pak, atalk_rdb_t* net);
static void	atnbp_send_lkup(paktype*, atalk_rdb_t*, ulong);
static boolean	atnbp_nve_add(nbpEntity *, ataddress *, boolean);
static boolean	atnbp_nve_setsearch(nveSearch *, nbpEntity *);
static char	*atnbp_reqtypeStr(nbp_reqtype);
static char	*nbp_requestStr(nbp_request *);
static void	atnbp_reply_handler(paktype *, nbptype *);
static void	atnbp_qNewReq (nbp_request *);
static void	atnbp_reqfree(nbp_request *);
static nbp_request*
                atnbp_reqget(nbp_reqtype, nbpEntity *, nbptimer *,
			     watched_queue *);
static void	atnbp_lookup_handler(paktype *, nbptype *);
static boolean  atnbp_nve_purge(nveSearch *);


static char 	*entityStr_buf;
static char 	*requestStr_buf;
static char 	*nvedump_buf;

static nve_result     atnbp_nve_status(nbpEntity *);
static nve_result     atnbp_nve_reg_complete(nbpEntity *);
static nveSearch        *atnbp_nve_nextmatch(nveSearch *);

/*
 * atnbp_print_tuple
 * print out an NBP tuple
 */
uchar *
atnbp_print_tuple (nbptuple *tp)
{
    uchar buf[100];
    uchar *p, *q;

    if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
	buginf("\nAT: %d.%d, skt %d, enum %d, name: ",
	       GETSHORT(tp->net), tp->node, tp->socket,tp->enumerator);
    p = (uchar *)tp+NBPTUPLESIZE; /* not sizeof(nbptuple)! */
    memset(buf, 0, sizeof(buf));
    q = &buf[0];
    if (*p > NBPNAMELEN) {
	if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
	    buginf("?NameBad? ");
	return (NULL);
    }
    bcopy(p+1,q,(int)*p);		/* copy object name */
    q += (int)*p;
    *q++ = ':';
    p += (int)*p + 1;
    if (*p > NBPTYPELEN) {
	if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
	    buginf("?TypeBad? ");
	return (NULL);
    }
    bcopy(p+1,q,(int)*p);		/* copy type */
    q += (int)*p;
    *q++ = '@';
    p += (int)*p + 1;
    if (*p > ZONELEN) {
	if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
	    buginf("?ZoneBad? ");
	return (NULL);
    }
    if (*p > 0) {			/* copy zone name */
	bcopy(p+1,q,(int)*p);
	q += (int)*p;
    } else {				/* null zone name is "*" */
	*q++ = '*';
    }
    *q++ = '\0';
    p += (int)*p + 1;
    if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
	buginf("%s ",buf); 
    return (p);
}

/*
 * atnbp_parse_tuple
 *
 * Takes a raw ASCII string, checks that the syntax meets NBP
 * syntax requirements and stuffs the result into an NBP tuple
 * data structure.
 *
 * Returns TRUE if the string parsed correctly, FALSE if
 * any of the components were too long.
 */
boolean
atnbp_parse_tuple (char *raw, nbpEntity *nve)
{
    char	*rp;
    char       *position;
    
    rp = raw;
    memset(nve->srvr_name.str, 0, sizeof(str32));
    memset(nve->type_srvc.str, 0, sizeof(str32));
    memset(nve->srvr_zone.str, 0, sizeof(str32));

    position = strchr(rp, ':');
    if (position == NULL || (position - rp) > NBPNAMELEN)
	return FALSE;
    strncpy(nve->srvr_name.str + 1, rp, position - rp);
    nve->srvr_name.str[0] = strlen(nve->srvr_name.str + 1);
    rp = position + 1;

    position = strchr(rp, '@');
    if (position == NULL)
	return FALSE;
    *position = '\0';
    nve->type_srvc.str[0] = atalk_hex_filter(rp, nve->type_srvc.str + 1,
					     NBPNAMELEN);
    rp = position + 1;

    nve->srvr_zone.str[0] = atalk_hex_filter(rp, nve->srvr_zone.str + 1,
					     NBPNAMELEN);
    return (TRUE);
}

/*
 * atnbp_print_pkt
 * print out contents of an NBP packet
 */

#define NUM_NBP_NAMES 5
static char const *nbp_names[] =
    { "", "BrRq", "LkUp", "LkUp-Reply", "FwdReq", "???" };

void atnbp_print_pkt (nbptype *nbp)
{
    nbptuple	*tp;
    int		tuple_count;
    int		nbp_type;

    tuple_count = nbp_get_count(nbp);
    nbp_type = nbp_get_type(nbp);

    if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
	buginf("\nAT: NBP ctrl = %s, ntuples = %d, id = %d  ",
	       nbp_names[(nbp_type < NUM_NBP_NAMES) ? nbp_type : NUM_NBP_NAMES],
	       tuple_count, nbp->NBPid);

    tp = (nbptuple *)((uchar *)nbp+sizeof(nbptype));
    while (tuple_count-- >  0) {
	tp = (nbptuple *)atnbp_print_tuple(tp);
	if (tp == NULL)
	    break;
    }
}

/*
 * nbp_DecodeOps
 * Validate and decode an NBP packet.  Fill in the nbpOps structure
 * and return TRUE on success.  
 */
boolean
atnbp_DecodeOps (paktype* pak, nbpOps* op)
{
    nbptype	*nbp;
    uchar	*pz;
    int		plen;

    nbp = (nbptype *)pak->transport_start;
    plen = pak->atalk_datalen;
    plen -= NBP_OVERHEADBYTES+NBPTUPLESIZE;
    if (plen < 0) {
	atalk_pak_printfail(pak, atalknbp_debug, "NBP name invalid");
	return (FALSE);
    }
    op->opcode = nbp_get_type(nbp);
    op->nbpId = nbp->NBPid;
    pz = (uchar *)nbp+NBP_OVERHEADBYTES; /* point at tuple */
    op->entity.net = GETSHORT(pz);
    pz += sizeof(short);
    op->entity.node = *pz++;
    op->entity.spec = *pz++;
    op->nbpEnum = *pz++;
    /* 
     * pz now contains a pointer to the name len
     */
    op->name.len = *pz++;
    op->name.p = pz;
    plen -= op->name.len + 1;
    pz += op->name.len;
    if ((op->name.len > NBPNAMELEN) || (plen < 0) || (op->name.len == 0)) {
	atalk_pak_printfail(pak, atalknbp_debug, "NBP name invalid");
	return (FALSE);
    }
    op->type.len = *pz++;
    plen -= op->type.len + 1;
    op->type.p = pz;
    pz += op->type.len;
    if ((op->type.len > NBPTYPELEN) || (plen <= 0) || (op->type.len == 0)) {
	atalk_pak_printfail(pak, atalknbp_debug, "NBP type invalid");
	return (FALSE);
    }
    op->zone.len = *pz++;
    op->zone.p = pz;
    plen -= op->zone.len + 1;
    if ((op->zone.len > ZONELEN) || (plen < 0)) {
	atalk_pak_printfail(pak, atalknbp_debug, "NBP zone invalid");
	return (FALSE);
    }
    if (op->zone.len == 0) {     /* zero zone length? */
	op->zone.p = (uchar *)"*";  /* use "*" as the zone name */
	op->zone.len = 1;
	op->zhash = 0;
    } else {
	op->zhash = atalk_zonesum(op->zone.p,op->zone.len);
    }
    return (TRUE);
}

/*
 * Extract one nbp tuple from the list and fill in only the necessary
 * data in nbpOps.  Return false if the packet is bad.
 */
boolean
atnbp_DecodeTuple(uchar *tuplelist, nbpOps *op)
{
    uchar *pz;

    pz = tuplelist;
    op->entity.net = GETSHORT(pz);
    pz += sizeof(short);
    op->entity.node = *pz++;
    op->entity.spec = *pz++;
    op->nbpEnum = *pz++;
    /* 
     * pz now contains a pointer to the name len
     */
    op->name.len = *pz++;
    op->name.p = pz;
    pz += op->name.len;
    if ((op->name.len > NBPNAMELEN) || (op->name.len == 0))
	return FALSE;

    op->type.len = *pz++;
    op->type.p = pz;
    pz += op->type.len;
    if ((op->type.len > NBPTYPELEN) || (op->type.len == 0))
	return FALSE;

    op->zone.len = *pz++;
    op->zone.p = pz;
    pz += op->zone.len;
    if (op->zone.len > ZONELEN)
	return FALSE;

    return (TRUE);
}


static void
atalk_nbp_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;

    process_watch_queue(atalknbpQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(atalknbpQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&atalknbpQ);
    atalk_nbp_pid = 0;
}

/*
 * atalk_nbp
 *
 * Appletalk Name Binding Protocol process function.
 */
process 
atalk_nbp (void)
{
    paktype	*pak;
    nbpOps	nbp;
    atalk_rdb_t	*r;
    rbTree	*tree;
    boolean	forcelkup;

    process_wait_on_system_init();
    
    /*
     * Set up this process' world.
     */
    signal_permanent(SIGEXIT, atalk_nbp_teardown);
    atalknbpQ = create_watched_queue("AT NBP packets", 0, 0);
    process_watch_queue(atalknbpQ, ENABLE, RECURRING);

    atalk_BindSocket(atalknbpQ,"NIS",ATALK_SOCK_NBP, NULL);

    while (TRUE) {
	process_wait_for_event();

	while ((pak = process_dequeue(atalknbpQ)) != NULL) {
	    atalkidbtype *atalkidb;
	    
	    if (pak == NULL || !atalk_check(pak, DDP_TYPE_NBP, ATALK_NBP_BAD, 
					    pak->line_input))
		continue;		/* no pak, checksum or other error */

	    atalk_stat[ATALK_NBP_IN]++;

	    if (!atnbp_DecodeOps(pak, &nbp)) {
		atalk_dispose(ATALK_NBP_BAD,pak);
		continue;
	    }
	    atalkidb = atalk_getidb(pak->if_input);
	    /*
	     * At some point in the future, we'll have to decide which
	     * tree this packet is destined for. ???
	     */
	    tree = atroute_GetTree();
	    switch (nbp.opcode) {
	    case NBP_BrRq:
		if (!pak->line_input && !atalkif_operational(atalkidb)) {
		    atalk_pak_printfail(pak,atalknbp_debug,"not operational");
		    atalk_dispose(ATALK_NOT_OPERATIONAL,pak);
		    continue;
		}
		atalk_pak_printprocess(pak,atalknbp_debug,
				       "NBP broadcast request");
		if (atroute_IsProxyRoute(tree,atalk_pak_dnet(pak))) 
		    forcelkup = TRUE;
		else 
		    forcelkup = FALSE;
		if (!atnbp_brrq(pak, &nbp, forcelkup)) {
		    atalk_pak_printfail(pak,atalknbp_debug,
				        "error handling brrq");
		    atalk_dispose(ATALK_NBP_FAILED, pak);
		    continue;
		}
		break;
	    case NBP_LkUp:
		atalk_pak_printprocess(pak,atalknbp_debug,"NBP lookup");
		atnbp_lookup_handler(pak, (nbptype*)pak->transport_start);
		break;
	    case NBP_FwdReq:
		if (atroute_IsProxyRoute(tree,atalk_pak_dnet(pak))) {
		    atalk_pak_printprocess(pak,atalknbp_debug,"NBP proxy");
		    if (!atnbp_brrq(pak, &nbp, TRUE)) {
			atalk_pak_printfail(pak, atalknbp_debug,
					    "error handling proxy");
			atalk_dispose(ATALK_NBP_FAILED, pak);
			continue;
		    }
		    atalk_dispose(ATALK_NBP_PROXY, pak);
		    continue;
		}
		if (!atalkif_operational(atalkidb)) {
		    atalk_pak_printfail(pak,atalknbp_debug,"not operational");
		    atalk_dispose(ATALK_NOT_OPERATIONAL,pak);
		    continue;
		}
		atalkidb = atalk_findIDB(atalk_pak_dnet(pak));
		if (atalkidb != NULL)
		    r = atroute_GetIdbRoute(atalkidb);
		else 
		    r = NULL;
		if (r == NULL) {
		    atalk_pak_printfail(pak, atalknbp_debug,
					"NBP fwdreq unexpected");
		    atalk_dispose(ATALK_NBP_BAD, pak);
		    continue;
		}
		atalk_pak_printprocess(pak,atalknbp_debug,
				       "NBP forward to lkup");
		atnbp_send_lkup(pak,r,nbp.zhash);
		break;
	    case NBP_LkUpReply:
		atalk_pak_printprocess(pak,atalknbp_debug,"handle NBP reply");
		atnbp_reply_handler(pak, (nbptype*)pak->transport_start);
		break;
	    default:
		atalk_pak_printbad(pak,"unknown NBP function", atalkidb);
		atalk_dispose(ATALK_NBP_BAD, pak);
		continue;
	    }
	    datagram_done(pak);
	}
    }
}

/*
 * atnbp_namematch
 *
 * return TRUE if two names are equal (case insensitive)
 * wild contains appropriate wildcard character - NBP_ZONE_WILD
 * or NBP_NAME_WILD
 */
boolean 
atnbp_namematch (uchar* s1,uchar* s2,int len1,int len2,uchar wild)
{
    uchar c = 0;
    uchar d = 0;
    int cnt = 0;

    if ((*s1 == wild) || (*s2 == wild))
	return (TRUE);
    if (len1 != len2)
	return (FALSE);
    while ((cnt < len1) && ((c = atalk_upper(*s1++)) == (d = atalk_upper(*s2++))))
	cnt++;
    return ((c == d) && (cnt <= len1));
}

/*
 * atnbp_brrq
 * handle NBP BrRq request
 */
static boolean
atnbp_brrq (paktype *pak, nbpOps* op, boolean forcelkup)
{
    atalk_rdb_t		*r;
    ziptabletype	*zt = NULL;
    zipnetentry		*zn = NULL;
    rbTree		*tree;
    atalkidbtype        *idb;
    
    /*
     * If we are a FTZ, then all requests received via that port
     * are converted to a lkup and sent to that cable ONLY.  We
     * don't even examine the contents.  This closes several security
     * holes where a luser could thwart the FTZ one-way security
     * by sending brrq's to us for valid zones.  Also, FTZ causes each
     * FTZ to behave independently of others with the same zone name.
     */
    idb = atalk_getidb(pak->if_input);

    if (atalk_ftzPortCheck(idb)) {
	if ((r = atroute_GetIdbRoute(idb))) {
	    zn = atalk_firstZone(r);
	    if (zn)
		zt = zn->zone;
	    atnbp_send_lkup(pak,r,(zt) ? zt->hashval : 0);
	} else {
	    atalk_pak_printfail(pak, atalknbp_debug, "No route for FTZ");
	    return (FALSE);
	}
	return (TRUE);
    }

    tree = atroute_GetTree();
    /*
     * for each network in the desired zone, send a directed broadcast LkUp
     * with DDP destination of $ff, using source net and node of the BrRq
     * sender as the source net and node of the broadcast
     */
    if ((op->zone.len == 1) && (*(op->zone.p) == '*')) {
	/* 
	 * calculate zone for this packet 
	 */
	r = atroute_FindRoute(tree,atalk_pak_snet(pak), FALSE);
	if (r == NULL) {
	    atalk_pak_printfail(pak, atalknbp_debug, "No route to srcnet");
	    return (FALSE);     /* and we can't do anything if it does */
	}
	if (atalk_net_proxy(r)) {
	    atalk_pak_printfail(pak, atalknbp_debug, "Proxy to '*' zone");
	    ReleaseRoute(r);
	    return (FALSE);
	}
	if (atalk_net_extended(r)) {
	    atalk_pak_printfail(pak, atalknbp_debug,
				"Zone '*' from extended net");
	    ReleaseRoute(r);
	    return (FALSE);
	}
	zn = atalk_firstZone(r);
	ReleaseRoute(r);
	if (zn) {
	    op->zone.p = (zn->zone)->name;
	    op->zone.len = (zn->zone)->length;
	} else {
	    atalk_pak_printfail(pak, atalknbp_debug, "No zone info for '*'");
	    return (FALSE);
	}
    }
    zt = atzip_FindZone(op->zone.p, op->zone.len);
    if (zt == NULL) {
	atalk_pak_printfail(pak, atalknbp_debug, "NBP zone unknown");
	return (FALSE);
    }
    zn = atalk_firstNet(zt);
    while (zn != NULL) {
	if (!atalk_net_proxy(zn->net)) {
	    if (atalk_net_connected(zn->net) ||	
                (!atalk_net_extended(zn->net) && forcelkup)) {
		atnbp_send_lkup(pak,zn->net,zt->hashval);
	    } else {
		atnbp_send_fwdreq(pak, zn->net);
	    }
	} else {
	    if (atalk_path_internal(zn->net->best_path)) {
		atnbp_send_lkup(pak, zn->net,zt->hashval);
	    }
	}

	zn = atalkzn_nextNet(zn);
    }
    return (TRUE);
}

/*
 * nbp_send_fwdreq
 * convert a BrRq into a fwdreq
 */
static void
atnbp_send_fwdreq (paktype* pak, atalk_rdb_t* r)
{
    paktype	*np;
    nbptype	*nbp;
    
    np = atalk_pak_duplicate(pak);
    if (np == NULL) {
	atalk_stat[ATALK_NO_BUFFERS]++;
	return;
    }
   /* Clear multicast */
    np->atalk_dstmcast = 0;
 
    atalk_pak_setdnet(np, r->dstrngstr);
    atalk_pak_setdnode(np, ATALK_ANY_ROUTER);

    /* Set source net and node only if request came from directly
     * connected net and fwdreq packet is going out same interface.
     * Allows access-group to filter on proper source address and
     * prevents gleaners from getting confused.
     */
    if (atalkif_SrcAddrType(r->rpath.idb, atalk_pak_srcaddr(pak))
	== srcOnThisCable) {
	atalk_pak_setsnet(np, atalkif_MyNet(r->rpath.idb));
	atalk_pak_setsnode(np, atalkif_MyNode(r->rpath.idb));
    }

    nbp = (nbptype *) np->transport_start;
    nbp_set_type(nbp, NBP_FwdReq);
    atalk_DDPcksum(np);
    if (atalk_send(np))
	atalk_stat[ATALK_NBP_FWD]++;
    else
	atalk_stat[ATALK_NBP_FAILED]++;
}

/*
 * atnbp_send_lkup
 * Handle forward requests for ATp2 extended nets.
 */
static void
atnbp_send_lkup (paktype* pak, atalk_rdb_t* r, ulong zonemc)
{
    paktype	*np;
    paktype	*cnp;
    nbptype	*nbp;

    if (r == NULL)
	return;
    /* Route has been poisoned already. */
    if (r->rpath.idb == NULL) {
        atalk_stat[ATALK_NBP_FAILED]++;
        return;
    }

    np = atalk_pak_duplicate(pak);
    if (np == NULL) {
	atalk_stat[ATALK_NO_BUFFERS]++;
	return;
    }
   /* Clear multicast */
    np->atalk_dstmcast = 0;
 
   if (atalk_net_extended(r)) {
	/* see page 8-18 to 8-20, inside AT, 2nd Ed.  */
	atalk_pak_setdnet(np,0);
	np->if_output = r->rpath.idb->phys_idb;
	if (zonemc != 0)
	    atalk_pak_setzonemcast(np,zonemc);
	else
	    atalk_pak_setbroadcast(np);
    } else {
	atalk_pak_setdnet(np,r->dstrngstr);
    }
    atalk_pak_setdnode(np, ATALK_BROAD_ADDR);

    /* Set source net and node only if request came from directly
     * connected net and lkup packet is going out same interface.
     * Allows access-group to filter on proper source address and
     * prevents gleaners from getting confused.
     */
    if (atalkif_SrcAddrType(r->rpath.idb, atalk_pak_srcaddr(pak))
	== srcOnThisCable) {
	atalk_pak_setsnet(np, atalkif_MyNet(r->rpath.idb));
	atalk_pak_setsnode(np, atalkif_MyNode(r->rpath.idb));
    }

    nbp = (nbptype *) np->transport_start;
    nbp_set_type(nbp, NBP_LkUp);
    atalk_DDPcksum(np);
    if (atalk_net_connected(r) && !atalk_path_internal(r->best_path)) {
	cnp = atalk_pak_duplicate(np);
	if (cnp != NULL) {

	    /* Clear multicast */
	    cnp->atalk_dstmcast = 0;

	    cnp->if_input = r->rpath.idb->phys_idb;
	    cnp->if_output = NULL;
	    process_enqueue_pak(atalknbpQ, cnp);
	    atalk_stat[ATALK_NBP_FWD]++;
	} else
	    atalk_stat[ATALK_NO_BUFFERS]++;
    }
    if (atalk_send(np))
	atalk_stat[ATALK_NBP_FWD]++;
    else
	atalk_stat[ATALK_NBP_FAILED]++;
}


/*************************  PRIVATE FUNCTIONS  ************************/

/*
 * nbp_mkpak -
 *	
 *	allocate and set up an NBP brrq or lkup pak.
 *	
 */
static paktype *
nbp_mkpak (nbp_request *req, atalkidbtype *idb, ataddress *dst, ataddress *src)
{
    ulong	size;
    paktype	*pak;
    str32	*pstr;
    nbptype	*nbpcntl;
    nbptuple	*tup;

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

    /* calculate size of pkt: */
    size = NBP_OVERHEADBYTES + NBPTUPLESIZE;
    size += STR32LEN(&req->nve.srvr_name)+1;    /* +1 for length byte */
    size += STR32LEN(&req->nve.type_srvc)+1;
    size += STR32LEN(&req->nve.srvr_zone)+1;

    /* get a pkt: */
    pak = atalk_pak_create(idb,
			   atalk_address(dst->net,dst->node),
			   src->spec,
			   ATALK_SOCK_NBP, /* pkt always to NIS */
			   DDP_TYPE_NBP,
			   size);

    if (pak == NULL)
        return (NULL);	/* no memory */

    /* set control portion: */
    nbpcntl = (nbptype *) pak->transport_start;

    if (req->reqtype == NBP_LOOKUP)
        nbp_set_type(nbpcntl, NBP_BrRq);
    else
        nbp_set_type(nbpcntl, NBP_LkUp);

    nbp_set_count(nbpcntl, 1);
    nbpcntl->NBPid = req->NBPid;

    /* set tuple: */
    tup = (nbptuple *)((uchar *) nbpcntl + NBP_OVERHEADBYTES);

    PUTSHORT(tup->net, src->net);
    tup->node = src->node;
    tup->socket = src->spec;
    tup->enumerator = 0;    /* not used for LkUp */

    /* copy NVE: */
    pstr = (str32 *) ((uchar *) tup + NBPTUPLESIZE);

    STR32CPY(pstr, &req->nve.srvr_name);
    (uchar *) pstr += STR32LEN(pstr) + 1;

    STR32CPY(pstr, &req->nve.type_srvc);
    (uchar *) pstr += STR32LEN(pstr) + 1;

    STR32CPY(pstr, &req->nve.srvr_zone);
    (uchar *) pstr += STR32LEN(pstr) + 1;

    atalk_DDPcksum(pak);
    return (pak);
}

/*
 * zoneExists -
 *	
 *	true if <zone> is a valid zone
 *	
 */
static boolean
zoneExists (str32 *zone)
{
    /* if local zone... */
    if (ISLOCALZONE(zone))
        return (TRUE);			/* ...always match */

    return (atzip_FindZone((uchar *) STR32PTR(zone), STR32LEN(zone))
            != (ziptabletype *) NULL);
}

/*
 * zoneAssigned -
 *	
 *	true if <zone> is associated with interface <idb>
 *	
 */
static boolean
zoneAssigned (atalkidbtype *idb, str32 *zone)
{
    zipifentry	*zif;
    str32	ifzone;

    if (at_debug(atalknbp_debug, ATALK_NULL_MASK)) {
        buginf("\nAT: zoneAssigned idb:'%s' zone:'%s'",
	       idb_get_namestring(idb->phys_idb), STR32PTR(zone));
    }

    /* if local zone... */
    if (ISLOCALZONE(zone))
        return (TRUE);			/* ...always match */

    /* foreach zone attached to this interface... */
    for (zif = atalkif_firstZone(idb);
	 zif != NULL;
	 zif = atalkif_nextZone(zif))
    {
        /* make a str32: */
        str32ninit(&ifzone,(char *) zif->zone->name,zif->zone->length);

        /* ...if match <zone> (case-insensitive)... */
        if (str32match(&ifzone,zone))
            return (TRUE);		/* ...then found */
    }

    /* no matching zone found */
    return (FALSE);
}

/*
 * pick_atalkIDB -
 *	
 *	return an active appletalk interface, or NULL.
 *	
 */
static atalkidbtype *
pick_atalkIDB (void)
{
    atalkidbtype *idb;

    /* foreach each interface... */
    FOR_ALL_ATIDBS(idb) {
	if (atalkif_operational(idb))
            return (idb);
    }
    return (NULL);
}

/*
 * pickIDBinZone -
 *	
 *	returns an interface associated with <zone>, or NULL
 *	if <hint> is non-NULL, then perfer it over a random idb.
 *	
 */
atalkidbtype *
pickIDBinZone (str32 *zone, atalkidbtype *hint)
{
    atalkidbtype *idb;
    
    /* if hint provided... */
    if (hint != NULL) {
	if (atalkif_operational(hint)) {
	    if (zoneAssigned(hint, zone))
		return(hint);
	}
    }

    /* foreach each interface... */
    FOR_ALL_ATIDBS(idb) {
	if (!atalkif_operational(idb))
	    continue;

        if (zoneAssigned(idb, zone))
            return(idb);
    }

    return (NULL);
}

static void
nbp_queuereq (queuetype *queue, nbp_request *req)
{
    enqueue(queue, req);
    atmaint_update_nbprequest_timer();
}

static void
nbp_requeuereq (queuetype *queue, nbp_request *req)
{
    requeue(queue, req);
    atmaint_update_nbprequest_timer();
}

/*
 * atnbp_qReq -
 *	
 *	queue a new NBP_LOOKUP or NBP_CONFIRM request.
 *	dst is NULL if lkup.
 *	
 */
static nbp_result
atnbp_qReq (nbp_reqtype rtype, nbpEntity *nve, ataddress*dst, ataddress *src,
	  nbptimer *tm, watched_queue *replyQ)
{
    atalkidbtype	*idb;
    nbptimer	stdtm;
    nbp_request *req;
    ataddress	ldst;			/* dst is NULL for lookup */

    if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3)) {
        /* dump parms: */
        buginf("\nAT: atnbp_qReq: %s: '%s' dst(%s)",
	       atnbp_reqtypeStr(rtype), atnbp_entityStr(nve), ddpAddrStr(dst));
        buginf("   src(%s) tm(%s) UQ: %lx <<\n", ddpAddrStr(src),
	       atnbp_timerStr(tm), replyQ);
    }

    /* set default originator: */
    if (src->spec == 0)
        src->spec = ATALK_SOCK_NBP;

    /* if bad socket value or not bound... */
    if (!INRANGE(ATALK_MIN_SOCKET,src->spec,ATALK_MAX_SOCKET)
		|| !atalk_IsBoundSocket(src->spec))
        return (NBP_BAD_SKT);

    /* make sure zone is valid: */
    if (!zoneExists(&nve->srvr_zone))
        return (NBP_BAD_ZONE);

    /* if src net non-zero... */
    if (src->net != 0) {
        /* ...must be assigned to an interface: */
        if ((idb = atalk_findIDB(src->net)) == NULL)
            return (NBP_BAD_DDP);
    } else {				/* ...net not specified */
        /* if lookup and local zone... */
        if (rtype == NBP_LOOKUP && ISLOCALZONE(&nve->srvr_zone))
            /* ...send to "default" interface? */
            return (NBP_BAD_ZONE);

        /* pick an interface to orig. request: */
        if ((idb = pick_atalkIDB()) == NULL)
            return (NBP_ATALK_DOWN);
    }
    /* now have an operational idb to originate pkt from */

    /* inform caller of the address we're sending on: */
    src->node = atalkif_MyNode(idb);
    if (src->net == 0)
        src->net = atalkif_MyNet(idb);

    /* if no timer parms supplied... */
    if (tm == (nbptimer *) NULL) {
        tm = &stdtm;

        /* then use default parms: */
        tm->maxrequests = DEF_MAXREQUESTS;
        tm->maxreplies  = DEF_MAXREPLIES;
        tm->interval    = DEF_INTERVAL;
    } else {				/* check timer parms: */
	if (tm->maxrequests <=0 || tm->maxreplies <=0 || tm->interval <= 0)
	    return (NBP_BAD_TIMER);
    }

    if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3))
        buginf("\nAT: atnbp_qReq: %s\n", atnbp_timerStr(tm));

    /* set/check dest. parms: */

    if (rtype == NBP_CONFIRM) {
        if (dst == (ataddress *) NULL)
            return (NBP_BAD_DDP_DST);
        tm->maxreplies  = 1;		/* never accept multiple replies */

        if (dst->net == 0)
            return (NBP_BAD_DDP_DST);
    } else {				/* NBP_LOOKUP request */
        dst = &ldst;			/* dst is NULL for lookup */
        dst->net = src->net;		/* send as if came from local net */
        dst->node = ATALK_BROAD_ADDR;	/* broadcast */
    }

    /* queue lookup request: */
    req = atnbp_reqget(rtype, nve, tm, replyQ);
    if (req == NULL)
        return (NBP_NO_MEMORY);

    req->pak = nbp_mkpak(req,idb,dst,src);
    if (req->pak == NULL) {
        atnbp_reqfree(req);
        return (NBP_NO_MEMORY);
    }

    atnbp_qNewReq(req);

    return (NBP_OK);
}


/*************************  EXPORTED FUNCTIONS  ***********************/

/*
 * nbp_confirm -
 *	
 *	confirm that <nve> is still a valid binding at <dst>.
 *	if so, post a NBP_REPLY_LAST event, else NBP_TIMEOUT event.
 *	
 */
nbp_result
atnbp_confirm (nbpEntity *nve, ataddress *dst, ataddress *src,
	     nbptimer *tm, watched_queue *replyQ)
{
    return (atnbp_qReq(NBP_CONFIRM, nve, dst, src, tm, replyQ));
}

/*
 * atnbp_find -
 *	
 *	lookup an nbp entity.
 *	
 */
nbp_result
atnbp_find (nbpEntity *nve, ataddress *src, nbptimer *tm,
	    watched_queue *replyQ)
{
    if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3))
        buginf("\nAT: nbp_find: %s\n", atnbp_timerStr(tm));

    return (atnbp_qReq(NBP_LOOKUP, nve, (ataddress *) NULL, src, tm, replyQ));
}

/*
 * nbp_isregistered -
 *	
 *	NBP_OK if <nve> is registered.
 *	NBP_REG_PENDING if registraion of <nve> is pending.
 *	NBP_NOT_REGISTERED if <nve> is not registered.
 *	
 */
nbp_result
atnbp_isregistered (nbpEntity *nve)
{
    nve_result status;
    
    status = atnbp_nve_status(nve);
    if (status == NVE_NOT_FOUND) {
        return (NBP_NOT_REGISTERED);
    } else if (status == NVE_TENTATIVE) {
        return (NBP_REG_PENDING);
    } else if (status == NVE_PERMANENT) {
        return (NBP_OK);
    } else {
        return (NBP_MODULE_ERROR);
    }
}

/*
 * atnbp_register -
 *	
 *	register <nve> from <src>, and install in our NVE table.
 *	if <async> is TRUE, then enter in table but mark as tentative.
 *	
 *	For async calls, registration is successful iff nbp_isregistered()
 *	returns NBP_OK after NBP_REG_TIMEOUT seconds.
 *	
 */
nbp_result
atnbp_register (nbpEntity *nve, ataddress *src, boolean async)
{
    atalkidbtype	*idb;
    int		rgskt = src->spec;   /* save registration skt */
    nbptimer	tm;
    nbp_result	err;
    nbpQelem	*ee;
    watched_queue *qptr;

    /* if bad socket value or not bound... */
    if (!INRANGE(ATALK_MIN_SOCKET,rgskt,ATALK_MAX_SOCKET)
		|| !atalk_IsBoundSocket(rgskt))
        return (NBP_BAD_SKT);

    /* if src net non-zero... */
    if (src->net != 0) {
        /* ...must be assigned to an interface: */
        if ((idb = atalk_findIDB(src->net)) == NULL)
            return (NBP_NET_NOT_ATTACHED);

        /* ...and zone must be assigned to same interface */
        if (!zoneAssigned(idb, &nve->srvr_zone))
            return (NBP_ZONE_NOT_ATTACHED);
    } else {
	/*
	 * ...net not specified, pick up interface in zone:
	 */
        /* if no net, must specified zone: */
        if (ISLOCALZONE(&nve->srvr_zone))
            return (NBP_ZONE_REQUIRED);

        /* pick an interface in zone: */
        if ((idb = pickIDBinZone(&nve->srvr_zone, NULL)) == NULL)
            return (NBP_IDB_NOT_IN_ZONE);
    }
    /*
     * args okay, send lookup:
     */
    src->net  = atalkif_MyNet(idb);
    src->node = 0;			/* result parm */

    /* note - direct lookup replies to NBP socket, but
     * will reg. in NVE table on <rgskt>, if no replies.
     */
    src->spec = ATALK_SOCK_NBP;		/* don't filter results */

    /* if async call... */
    if (async) {
        qptr = NULL;
    } else {
        qptr = create_watched_queue("AT Registration Queue", 0, 0);
    }

    /* set up reg. timer: */
    tm.maxrequests  = REG_REQUESTS;
    tm.maxreplies   = REG_REPLIES;
    tm.interval     = REG_INTERVAL;

    if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3))
        buginf("\nAT: nbp_register, calling nbp_find async=%d", async);

    if ((err = atnbp_find(nve, src, &tm, qptr)) != NBP_OK) {
        if (qptr != NULL)
            delete_watched_queue(&qptr);
        return (err);
    }
    
    /* if async call, tentatively enter in table and return: */
    if (async) {
        src->spec = rgskt;
        if (atnbp_nve_add(nve, src, TRUE))
            return (NBP_REG_PENDING);
        else
            return (NBP_DUPLICATE);
    }

    
    /* if synchronous call, wait for event: */
    process_watch_queue(qptr, ENABLE, ONE_SHOT);
    process_wait_for_event();
    process_watch_queue(qptr, DISABLE, ONE_SHOT);
    
    ee = process_dequeue(qptr);

    switch(ee->msg) {
    case NBP_REPLY_LAST:
        /* registered in another node, save address for caller diags: */
        bcopy(&ee->srvr_addr, src, sizeof(ataddress));

        /* free the used queue element: */
        atnbp_event_free(ee);
        delete_watched_queue(&qptr);
        return (NBP_DUPLICATE);

    case NBP_TIMEOUT:
        /* success! */
        atnbp_event_free(ee);
        delete_watched_queue(&qptr);
        break;

    default:
        /* internal error */
	if (at_debug(atalkerr_debug, ATALK_NULL_MASK))
	    buginf("\nAT: nbp_register: unexpected msg, %d\n",ee->msg);
        atnbp_event_free(ee);
        atnbp_cancel(qptr);
        delete_watched_queue(&qptr);
        return (NBP_MODULE_ERROR);
    }

    /* enter NVE in our table: */
    src->spec = rgskt;
    if (atnbp_nve_add(nve, src, FALSE))
        return (NBP_OK);
    else
        return (NBP_TABLE_ERROR);
}

/*
 * atnbp_resultStr -
 *	
 *	return char translation of result code.
 *	
 */
char *
atnbp_resultStr (nbp_result ii)
{
    switch(ii) {
      case NBP_ATALK_DOWN:		return("NBP_ATALK_DOWN");
      case NBP_BAD_DDP:			return("NBP_BAD_DDP");
      case NBP_BAD_DDP_DST:		return("NBP_BAD_DDP_DST");
      case NBP_BAD_SKT:			return("NBP_BAD_SKT");
      case NBP_BAD_TIMER:		return("NBP_BAD_TIMER");
      case NBP_BAD_ZONE:		return("NBP_BAD_ZONE");
      case NBP_DUPLICATE:		return("NBP_DUPLICATE");
      case NBP_IDB_NOT_IN_ZONE:		return("NBP_IDB_NOT_IN_ZONE");
      case NBP_MODULE_ERROR:		return("NBP_MODULE_ERROR");
      case NBP_NET_NOT_ATTACHED:  	return("NBP_NET_NOT_ATTACHED");
      case NBP_NOT_REGISTERED:    	return("NBP_NOT_REGISTERED");
      case NBP_NO_MATCH:		return("NBP_NO_MATCH");
      case NBP_NO_MEMORY:		return("NBP_NO_MEMORY");
      case NBP_OK:			return("NBP_OK");
      case NBP_REG_PENDING:		return("NBP_REG_PENDING");
      case NBP_TABLE_ERROR:		return("NBP_TABLE_ERROR");
      case NBP_ZONE_NOT_ATTACHED: 	return("NBP_ZONE_NOT_ATTACHED");
      case NBP_ZONE_REQUIRED:		return("NBP_ZONE_REQUIRED");
      default:				return("nbp_resultStr:  bad arg");  
    }
}

/*
 * atnbp_mkname -
 *	
 *	initialize an NVE
 *	
 */
void
atnbp_mkname (nbpEntity *nve, char *cname, char *ctype, char *czone)
{
    str32init(&nve->srvr_name,cname);
    str32init(&nve->type_srvc,ctype);
    str32init(&nve->srvr_zone,czone);
}

/*
 * atnbp_unregister -
 *	unregister <nve>
 *	must be called once for each alias.
 *	
 */
nbp_result
atnbp_unregister (nbpEntity *nve)
{
    if (atnbp_nve_delete(nve))
        return (NBP_OK);
    else
        return (NBP_NO_MATCH);
}

/*
 * atnbp_nve_DDPdelete -
 *	delete all entities registered to a DDP address.
 *	if socket number is zero, then delete all entries matching
 *	on (net, nodeID), o'wise match on (net, nodeID, socket).
 *	
 *	returns FALSE if nothing deleted.
 */
boolean
atnbp_nve_DDPdelete (ataddress *ddp)
{
    key		kk;
    nveSearch	ss;
    nveEntry	tab;
    ushort	idx;
    int		ndeletes;

    /* foreach nve entity... */
    for (idx = id_first(nveidh), ndeletes = 0;
	idx != NILID;
	idx = id_next(nveidh,idx))
    {

	/* get each record until match: */
	db_mkkey(&kk, 2, nveSequential, idx);
	if (!DB_RETRIEVE((byteptr) &tab, &kk, sizeof(nveEntry))) {
	    /* internal error! (no key for idx) */
	    continue;
	}

	/* if matches DDP address... */
	if (tab.net == ddp->net && tab.node == ddp->node &&
			(ddp->spec == 0 || ddp->spec == tab.socket))
	{
	    /* set up search structure to use common purge routine: */ 
	    ss.net	= tab.net;
	    ss.socket	= tab.socket;
	    ss.node	= tab.node;
	    ss.enumerator = tab.enumerator;
	    ss.luName	= tab.nameSym;
	    ss.luType	= tab.typeSym;
	    ss.luZone	= tab.zoneSym;
	    ss.idx	= idx;

	    atnbp_nve_purge(&ss);	/* ...then delete it */
	    ++ndeletes;
	}
    }

    return (ndeletes > 0);
}

/*
 * nbp_DDPunregister -
 *	unregister everything registered to a DDP address.
 *	ignore socket address unless non-zero.
 *	
 */
nbp_result
atnbp_DDPunregister (ataddress *ddp)
{
    if (atnbp_nve_DDPdelete(ddp))
        return (NBP_OK);
    else
        return (NBP_NO_MATCH);
}


/*
 * nbp_entityStr -
 *	warning: don't nest!
 */
char *
atnbp_entityStr (nbpEntity *nve)
{
    if (nve) {
        sprintf(entityStr_buf, "%s:%s@%s",
		at_hex_display(STR32PTR(&nve->srvr_name)),
		at_hex_display(STR32PTR(&nve->type_srvc)),
		at_hex_display(STR32PTR(&nve->srvr_zone)));
        return (entityStr_buf);
    }
    return ("NULL_NBPENTITY");
}

/*
 * nbp_timerStr -
 *	don't nest!
 */
char *
atnbp_timerStr (nbptimer *tm)
{
    char *buf = string_getnext();

    if (tm) {
        sprintf(buf, "(%dq,%dp,%di)",
		tm->maxrequests, tm->maxreplies, tm->interval);
        return (buf);
    }

    return ("NULL_NBPTIMER");
}

/*
 * ddpAddrStr -
 *	nest!
 */
char *
ddpAddrStr (ataddress *ddp)
{
    char *buf = string_getnext();

    if (ddp) {
        sprintf(buf, "%dn,%da,%ds", ddp->net, ddp->node, ddp->spec);
        return (buf);
    }

    return ("NULL_DDPADDR");
}

/*
 * atnbp_msgStr -
 *	
 */
char *
atnbp_msgStr (nbp_event_code msg)
{
    switch(msg) {
      case NBP_REPLY:     	return("NBP_REPLY");
      case NBP_REPLY_LAST:	return("NBP_REPLY_LAST");
      case NBP_TIMEOUT:		return("NBP_TIMEOUT");
      default:			return("atnbp_msgStr:  bad arg"); 
    }
}


/* queue for storing pending nbp requests (peeked at by at_maint): */
static queuetype nbpRequestQ;

/* allocator for nbp id's: */
static iddesc nbpidh;

/* private free lists: */
static queuetype nqe_freeQ;
static queuetype nre_freeQ;

/*************************  PRIVATE FUNCTIONS  ************************/

/*
 * send_confirm -
 *	
 */
static void
atnbp_send_confirm (paktype *pak)
{

    paktype *dup;
    
    dup = atalk_pak_duplicate(pak);
    if (dup == NULL) {
        if (   at_debug(atalknbp_debug, ATALK_NULL_MASK)
	    || at_debug(atalkerr_debug, ATALK_NULL_MASK))
            buginf("\nAT: NBP, send_confirm, unabled to pak_duplicate");
        return;
    }

    if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3)) {
        short	size;
        ddplong	*ddp;

        size = pak->atalk_datalen;

        ddp = (ddplong *)dup->network_start;

        ddp->ddppt = 2;

        dup->datagramstart = (uchar *)ddp;
        dup->datagramsize = size + DDP_OVERHEADBYTES;
        dup->transport_start = (uchar *) ddp + DDP_OVERHEADBYTES;
        dup->atalk_datalen = size;
        atalk_pak_setlength(dup, dup->atalk_datalen);

        atalk_pak_printgood (pak, "Confirm Original",
			     atalk_getidb(pak->if_input));
        atalk_pak_printgood (dup, "Confirm Dup",
			     atalk_getidb(pak->if_input));
    }
    atalk_send(dup);
}

/*
 * nbp_nextid -
 *	
 *	allocate next avaliable nbp id, wrap around if at top of range.
 *	
 */
static uchar
nbp_nextid (void)
{
    ushort ii;

    ATASSERT(id_getid(nbpidh, &ii));

    return ((uchar) ii);
}

/*
 * get_nbp_msg -
 *	allocate and initialize an nbp reply queue element
 */
static nbpQelem *
get_nbp_msg (nbp_event_code msg)
{
    nbpQelem *nqe = NILNQELEM;

    if (QUEUEEMPTY(&nqe_freeQ))
	nqe = malloc(sizeof(nbpQelem));
    else
	nqe = dequeue(&nqe_freeQ);

    if (nqe == (nbpQelem *) NULL)
        return (NILNQELEM);

    memset(nqe, 0, sizeof(nbpQelem));

    nqe->msg = msg;

    return (nqe);
}

/*
 * nbp_empty_trash -
 *	
 *	maintenance routine to return excess unused storage.
 *	called by at_maint(), thru garbage collection table.
 *	
 */
static boolean
nbp_empty_trash (void)
{
    nbpQelem	*nqe;
    nbp_request	*req;

    while (QUEUESIZE(&nqe_freeQ) >= MAX_NQE_FREELIST) {
	nqe = dequeue(&nqe_freeQ);
	free(nqe);
    }

    while (QUEUESIZE(&nre_freeQ) >= MAX_NRE_FREELIST) {
	req = dequeue(&nre_freeQ);
	free(req);
    }
    return (TRUE);
}

/*************************  EXPORTED FUNCTIONS  ***********************/

/*
 * atnbp_reqfree -
 *	return an nbp request structure to free list, schedule
 *	garbage collection if needed.
 *	
 */
static void
atnbp_reqfree (nbp_request *req)
{
    if (req->pak != (paktype *) NULL) {
        datagram_done(req->pak);
	req->pak = NULL;
    }

    enqueue (&nre_freeQ, req);

    if (QUEUESIZE(&nre_freeQ) > MAX_NRE_FREELIST)
	atmaint_ScheduleGCRun(nbp_gccontext);
}

/*
 * nbpsys_event_free -
 *	return an nbp event element to free list, schedule
 *	garbage collection if needed.
 *	
 */
void
atnbp_event_free (nbpQelem  *nqe)
{
    enqueue (&nqe_freeQ, nqe);

    if (QUEUESIZE(&nqe_freeQ) > MAX_NQE_FREELIST)
	atmaint_ScheduleGCRun(nbp_gccontext);
}

/*
 * atnbp_reqget -
 *	
 *	allocate and initialize an NBP request record.
 *	
 */
static nbp_request *
atnbp_reqget (nbp_reqtype reqtype, nbpEntity *nve,
	      nbptimer *tm, watched_queue *uqw)
{
    nbp_request *req;

    if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3))
        buginf("\nAT: atnbp_reqget: %s\n", atnbp_timerStr(tm));

    if (QUEUEEMPTY(&nre_freeQ))
	req = malloc(sizeof(nbp_request));
    else
	req = dequeue(&nre_freeQ);

    if (req == (nbp_request *) NULL)
        return ((nbp_request *) NULL);

    /* initialize request: */
    req->link		= NULL;
    req->reqtype	= reqtype;
    req->reqClock	= 1;		/* force transmit on first queueing */
    req->NBPid		= nbp_nextid();
    req->uqw		= uqw;
    bcopy(nve, &req->nve, sizeof(nbpEntity));

    req->tm.maxrequests = tm->maxrequests;
    req->tm.maxreplies  = tm->maxreplies;
    req->tm.interval    = tm->interval;

    req->pak = (paktype *) NULL;

    if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3))
        buginf("\nAT: NBP nbp_getreq:  %s\n", nbp_requestStr(req));

    return (req);
}

/*
 * atnbp_background -
 *	
 *	background process for retransmitting NBP lookup requests.
 *	if timeout, post NBP_TIMEOUT event.  o'wise, send another
 *	LkUp (nbp_confirm) or BrRq (nbp_find) if it's time.
 *	
 *	called once per second from atalk_maint().
 *	
 */
void
atnbp_background (void)
{
    queuetype	*pending = &nbpRequestQ;
    nbp_request *req;
    int		ii;
    int		jj;
    nbpQelem	*nqe;
    nbpOps	op;
    boolean     dodebug;
    
    /* if no pending requests... */
    if (QUEUEEMPTY(pending))
        return;

    dodebug = at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3);

    if (dodebug)
        buginf("\nAT: NBP nbp_background N=%d\n", QUEUESIZE(pending));

    /* foreach pending request... */
    for (ii=0, jj=QUEUESIZE(pending); jj > 0; ii++, jj--) {
        req = dequeue(pending);
	if (!req)
	    break;

        if (dodebug)
            buginf("\nAT: NBP nbp_background req[%d]: %s",
		   ii, nbp_requestStr(req));

        /* if max requests sent (and waited min interval)... */
        if (req->tm.maxrequests <= 0 && req->reqClock <= 0) {

            if (dodebug)
                buginf("\nAT: NBP TIMEOUT req[%d]: uqw=%lx", ii, req->uqw);

            /* otherwise, timeout event to user queue: */
            if (req->uqw != NULL) {
                /* ...then post NBP_TIMEOUT event: */
                nqe =  get_nbp_msg(NBP_TIMEOUT);
                if (nqe == NILNQELEM) {
                    /* no memory.  TODO: stats */
		    nbp_queuereq(pending, req);
                    return;
                }
                process_enqueue(req->uqw, nqe);
            } else {			/* no user queue */
                nbp_result nbpstat;

                /* registrtion pending? */
                nbpstat = atnbp_isregistered(&req->nve);
                if (dodebug)
                    buginf("\nAT: NBP   NULL uqw, nbpstat=%s\n",
			   atnbp_resultStr(nbpstat));

                if (nbpstat == NBP_REG_PENDING) {
                    nve_result err;

                    /*
		     * yes, mark as permanent since
                     * no replies before timeout
                     */
                    err = atnbp_nve_reg_complete(&req->nve);
                    if (err != NVE_OK)
			at_err(MODID,
			       "nbp_background: nve_reg_complete error, %s",
			       atnbp_nve_resultStr(err));
                }
            }

            atnbp_reqfree(req);		/* deallocate request */

            continue;			/* next request */
        }

        /* otherwise, see if it's time to send a request: */
        --req->reqClock;
        if (req->reqClock <= 0 && req->tm.maxrequests > 0) {
            /* reset request clock: */
            req->reqClock = req->tm.interval;

            /* decrement requests sent: */
            --req->tm.maxrequests;

	    (void) atnbp_DecodeOps(req->pak, &op);

            /* send request: */
            switch (req->reqtype) {
	      case NBP_LOOKUP:
                atnbp_brrq(req->pak,&op,FALSE);
                break;
	      case NBP_CONFIRM:
                atnbp_send_confirm(req->pak);
                break;
	      default:
                break;
            }
        }

        /* more requests left, return to queue: */
        nbp_queuereq(pending, req);
    }
}

/*
 * atnbp_qNewReq -
 *	
 *	process for retransmitting NBP lookup requests.
 *	timeout, post NBP_TIMEOUT event.  o'wise, send another
 *	(nbp_confirm) or BrRq (nbp_find) if it's time.
 *	
 *	once per second from atalk_maint().
 */
static void
atnbp_qNewReq (nbp_request *req)
{
    /* jam on front of queue: */
    nbp_requeuereq(&nbpRequestQ, req);

    if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3))
        buginf("\nAT: NBP nbp_qNewReq: %s", nbp_requestStr(req));
}

/*
 * atnbp_cancel -
 *	cancel all requests pending on <userQ>
 */
void
atnbp_cancel (watched_queue *userQ)
{
    queuetype	*pending = &nbpRequestQ;
    nbp_request *req;
    int		ii;

    /* foreach pending request... */
    for (ii = QUEUESIZE(pending); ii > 0; ii--) {
        req = dequeue(pending);
	if (!req)
	    break;

        /* ...if associated with <userQ> */
        if (req->uqw == userQ) {
            /* ...deallocate request: */
            atnbp_reqfree(req);
        } else {			/* no match - put back */
            nbp_queuereq(pending, req);
	}
    }
}

/*
 * atnbp_lookup_handler -
 *	handle an LkUp request directed at us.
 *	search our registered NVE table, respond if match, o'wise drop
 *	
 *	called from atalk_nbp() process, which awakens and processes
 *	npb LkUp packets posted by atalk_enqueue().
 *	
 *	
 */
static void
atnbp_lookup_handler (paktype *pak, nbptype *nbpcntl)
{
    str32		*pstr;
    nbptuple		*tup;
    static nveSearch	srch;
    nveSearch		*ss = &srch;
    static nbpEntity	nvestor;
    nbpEntity		*nve = &nvestor;
    short		nbpid;
    atalkidbtype	*idb;		/* this will be the input idb */
    paktype		*pak_out;	/* new pak for response */
    int			paklen;

    extern symbol_table nve_symtab;	/* optimization: peek at nve table */

    if (pak == NULL)
	return;

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

    /* count the number of lookup requests we process: */
    atalk_stat[ATALK_NBP_LKUP]++;

    if (at_debug(atalknbp_debug, ATALK_NULL_MASK)) {
        buginf("\nAT: NBP, LkUp handler, ntups=%d\n", nbp_get_count(nbpcntl));
	atnbp_print_pkt(nbpcntl);	/* print out contentes of nbp packet */
    }
    /* spec allows only one request per pkt: */
    if (nbp_get_count(nbpcntl) != 1) {
        return;				/* ???TODO: stats */
    }

    tup = (nbptuple *) ((uchar *) nbpcntl + NBP_OVERHEADBYTES),
    pstr = (str32 *) ((uchar *) tup + NBPTUPLESIZE);
    nbpid = nbpcntl->NBPid;

    /* get NVE: */
    str32ninit(&nve->srvr_name, STR32PTR(pstr), STR32LEN(pstr));
    (uchar *) pstr += STR32LEN(pstr) + 1;

    str32ninit(&nve->type_srvc, STR32PTR(pstr), STR32LEN(pstr));
    (uchar *) pstr += STR32LEN(pstr) + 1;
    /*
     * optimization: do a quick check here to see if the
     * type of service is a wildcard or we possibly have it registered -
     * if not, bail out:
     */
    if (!ISNBP_ANY(&nve->type_srvc)) {
	if (sy_symbol32(nve_symtab, &nve->type_srvc) == NILSYM) {
	    /* not a wildcard, and we don't have it, quit early: */
	    if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3))
		buginf("\nAT: NBP LkUp  no match!");
	    return;			/* no match */
        }
    }

    str32ninit(&nve->srvr_zone, STR32PTR(pstr), STR32LEN(pstr));
    (uchar *) pstr += STR32LEN(pstr) + 1;

    idb = atalk_getidb(pak->if_input);

    /* if zone is "*", then map to primary interface zone: */
    if (ISLOCALZONE(&nve->srvr_zone)) {
        ziptabletype *zz;

        zz = atalkif_primary_zone(idb);
        if (zz != NULL)
            str32ninit(&nve->srvr_zone,(char *) zz->name,zz->length);
    }

    if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3)) {
        buginf("\nAT: NBP LkUp for '%s', dst net %d, receive net %d",
	       atnbp_entityStr(nve), atalk_pak_dnet(pak),
	       atalkif_MyNet(idb));
    }
    /* if pak dst net is bcast or same as input idb... */
    if (   (idb != NULL)
	&& (atalkif_srcnettype(idb, atalk_pak_dnet(pak)) > netBadForCable)) {
	/* ...then use receive interface to src return pkt */
	;
    } else {
	/*
	 * directed bcast, find dst idb. If we can't find one, then
	 * fire the response back out the input interface.
	 */
	idb = atalk_findIDB(atalk_pak_dnet(pak));
	if (idb == NULL)
	    idb = atalk_getidb(pak->if_input);
    }

    if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3)) {
        buginf("\nAT: NBP LkUp, assigned idb net is %d, receive net %d",
	       atalkif_MyNet(idb), atalk_pak_dnet(pak));
	buginf("\nAT: LkUp %s", atnbp_entityStr(nve));
    }

    if (!atnbp_nve_setsearch(ss, nve)) {
        if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3))
            buginf("\nAT: NBP  no match! for %s", atnbp_entityStr(nve));
        return;				/* not in table or bad args */
    }

    /* NVE MUST MATCH destination net.  (see NVEMATCH in atalk_nbp.h) */
    ss->net = atalkif_MyNet(idb);

    if ((ss = atnbp_nve_firstmatch(ss, nve)) == NILNVE) {
        if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3))
            buginf("\nAT: NBP,  no match for %s (firstmatch)!",
		   atnbp_entityStr(nve));
        return;
    }

    /* we have a match, allocate a reply pkt: */
    if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3)) {
        buginf("\nAT: NBP  pak to %d on %s",
	       atalk_pak_dnet(pak), idb_get_namestring(idb->phys_idb));
    }

    /* don't know exact size of reply, get max DDP: */
    pak_out = atalk_pak_create(idb,
			       atalk_address(GETSHORT(tup->net),tup->node),
			       ATALK_SOCK_NBP,
			       tup->socket,
			       DDP_TYPE_NBP,
			       DDP_MAX_DATABYTES);
    
    if (pak_out == NULL) {
        return;				/* no memory  */
    }

    if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3)) {
        buginf("  new pak if_input is %0lx, if_output is %0lx\n",
	       pak_out->if_input,
	       pak_out->if_output);
    }

    /* set pointer to data area of new pkt: */
    nbpcntl = (nbptype *) pak_out->transport_start;

    ss = ss;
    nbp_set_type(nbpcntl, NBP_LkUpReply);
    nbp_set_count(nbpcntl, 0);
    nbpcntl->NBPid = (uchar) nbpid;
    paklen = NBP_OVERHEADBYTES;
    tup = (nbptuple *) ((uchar *) nbpcntl + NBP_OVERHEADBYTES);
    pstr = (str32 *) ((uchar *) tup + NBPTUPLESIZE);
	 
    /* for each matching entry... */
    while (ss != NILNVE) {
        /* ...add to reply: */
        PUTSHORT(tup->net, ss->net);
        tup->node   = ss->node;
        tup->socket = ss->socket;
        tup->enumerator = ss->enumerator;
        paklen += NBPTUPLESIZE;

        if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3))
            buginf("\nAT: lookup_handler: adding '%s'",
		   atnbp_entityStr(&ss->nve));
        str32cpy(pstr, &ss->nve.srvr_name);
        paklen += STR32LEN(pstr) + 1;
        (uchar *) pstr += STR32LEN(pstr) + 1;

        str32cpy(pstr, &ss->nve.type_srvc);
        paklen += STR32LEN(pstr) + 1;
        (uchar *) pstr += STR32LEN(pstr) + 1;

        str32cpy(pstr, &ss->nve.srvr_zone);
        paklen += STR32LEN(pstr) + 1;
        (uchar *) pstr += STR32LEN(pstr) + 1;

        /* any room left in this pkt? */
        if ((paklen + NBPTUPLESIZE + 33*3) > DDP_MAX_DATABYTES)
            break;  /* TODO:  send multiple replies if necessary */

	ss = atnbp_nve_nextmatch(ss);	/* note - use net from firstmatch */
	nbp_set_count(nbpcntl,nbp_get_count(nbpcntl) + 1);
	tup = (nbptuple *) pstr;
	pstr = (str32 *) ((uchar *) tup + NBPTUPLESIZE);
    }

    if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
	atnbp_print_pkt(nbpcntl);	/* print out contents of nbp packet */

    /* send the pkt: */
    atalk_pak_setlength(pak_out, paklen);
    atalk_DDPcksum(pak_out);

    if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3)) {
        ataddress dst,src;

        dst.net  = atalk_pak_dnet(pak_out);
        dst.node = atalk_pak_dnode(pak_out);
        dst.spec = atalk_pak_dsock(pak_out);
        src.net  = atalk_pak_snet(pak_out);
        src.node = atalk_pak_snode(pak_out);
        src.spec = atalk_pak_ssock(pak_out);

        buginf("\nAT: NBP   sending reply, from (%s) on idb %0lx",
	       ddpAddrStr(&src), pak_out->if_input);
	buginf("\n   to (%s) on idb %0lx, nbp len %d, ddp len %d"
	       " cksum=%d compute=%d\n",
	       ddpAddrStr(&dst),
	       pak_out->if_output,
	       pak_out->atalk_datalen,
	       atalk_pak_rawlength(pak_out),
	       atalk_pak_cksum(pak_out),
	       atalk_compute_crc(pak_out) );
	
        if (pak_out->if_input != NULL)
            buginf("\n   if_input:  %s", idb_get_namestring(pak->if_input));
        if (pak_out->if_output != NULL)
            buginf("\n   if_output:  %s", idb_get_namestring(pak->if_output));
    }

    if (atalk_send(pak_out))
        atalk_stat[ATALK_NBP_OUT]++;
    else
	atalk_stat[ATALK_NBP_FAILED]++;
}

/*
 * atnbp_reply_handler -
 *	
 *	handle an LkUp-Reply directed at us.
 *	search our list of pending requests, and post results if
 *	a request waiting.  if no request pending, do nothing.
 *	
 *	called from atalk_nbp() process, which awakens and processes
 *	npb LkUp-Reply packets posted by enqueue().
 *	
 *	note:  <nbpcntl> is a pointer to beginning of nbp data in <pak>.
 *	
 */
static void
atnbp_reply_handler (paktype *pak, nbptype *nbpcntl)
{
    int		ii;
    str32	*pstr;
    queuetype	*pending = &nbpRequestQ;
    nbp_request *req = NULL;
    nbptuple	*tup;
    boolean	found = FALSE;
    nbpQelem	*nqe;

    if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3))
        buginf("\nAT: NBP, nbp_reply_handler Q=%lx, size=%d",
	       pending, QUEUESIZE(pending));

    /* find request for this pkt: */
    for (ii = QUEUESIZE(pending); ii > 0; ii--) {
        req = dequeue(pending);
	if (req == NULL)
	    break;

	found = (nbpcntl->NBPid == req->NBPid);
        if (found) {
            break;
	}

        /* not the one - return to queue: */
        nbp_queuereq(pending, req);
    }

    /* if no request pending... */
    if (!found) {
        if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3))
            buginf("\nAT: NBP, reply_handler, no match for nbpid %d",
		   nbpcntl->NBPid);
        return;				/* pkt freed in atalk_nbp() */
					/* TODO:  stats */
    }

    if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3))
        buginf("\nAT: NBP, reply_handler; found match, req %s",
	       nbp_requestStr(req));

    /* we found request, post response(s): */

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


    ii=0;
    tup = (nbptuple *) ((uchar *) nbpcntl + NBP_OVERHEADBYTES);
    pstr = (str32 *) ((uchar *) tup + NBPTUPLESIZE);
    while ((ii < nbp_get_count(nbpcntl)) && (req->tm.maxreplies > 0)) {
	if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3)) {
	    buginf("\nAT: NBP reply_handler, in loop "
		   "ii=%d, ntups=%d, maxreplies=%d",
		   ii, nbp_get_count(nbpcntl), req->tm.maxreplies);
	}
	/* get an event record: */
	nqe = get_nbp_msg((req->tm.maxreplies > 1) ? NBP_REPLY : NBP_REPLY_LAST);
	if (nqe == NILNQELEM) {
	    /* no memory.  TODO: stats */
	    nbp_queuereq(pending, req);
	    return;
	}

	str32cpy(&nqe->nve.srvr_name, pstr);
	(uchar *) pstr += STR32LEN(pstr) + 1;
	
	str32cpy(&nqe->nve.type_srvc, pstr);
	(uchar *) pstr += STR32LEN(pstr) + 1;
	
	/* if node didn't specify zone... */
	if (ISLOCALZONE(pstr))
	    /* ...then use request zone: */
	    str32cpy(&nqe->nve.srvr_zone, &req->nve.srvr_zone);
	else
	    str32cpy(&nqe->nve.srvr_zone, pstr);
	(uchar *) pstr += STR32LEN(pstr) + 1;
	
	/* copy nbp server data: */
	nqe->enumerator = tup->enumerator;
	
	nqe->srvr_addr.net  = GETSHORT(tup->net);
	nqe->srvr_addr.node = tup->node;
	nqe->srvr_addr.spec = tup->socket;
	
	/* copy the originator of packet for network debugging: */
	nqe->orig_addr.net  = atalk_pak_snet(pak);
	nqe->orig_addr.node = atalk_pak_snode(pak);
	nqe->orig_addr.spec = atalk_pak_ssock(pak);
	
	/* enqueue on user's queue: */
	if (req->uqw != NULL) {
	    process_enqueue(req->uqw, nqe);
	} else {
	    /* no user queue, is async reg. pending? */
	    if (atnbp_isregistered(&nqe->nve) == NBP_REG_PENDING) {
		/* yes, delete from table: */
		atnbp_nve_delete(&nqe->nve);
	    }
	    atnbp_event_free(nqe);
	}
	ii++;
	--req->tm.maxreplies;
	tup = (nbptuple *) pstr;
	pstr = (str32 *) ((uchar *) tup + NBPTUPLESIZE);
    }
    
    /* if we've posted requested number of replies... */
    if (req->tm.maxreplies <= 0) {
        if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3))
            buginf("\nAT: NBP  freeing request maxreplies=%d",
		   req->tm.maxreplies);
        /* ..then deallocate request: */
        atnbp_reqfree(req);
    } else {				/* otherwise, request still active: */
        if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3))
            buginf("\nAT: NBP  still active, maxreplies=%d",
		   req->tm.maxreplies);
        nbp_queuereq(pending, req);
    }
}

/*
 * atnbp_Initialize -
 *	initialize nbp data structures,
 *	called from atalk_init(), once per reload.
 */
void
atnbp_Initialize (void)
{
    /* init queue w/no size restraints */
    queue_init(&nbpRequestQ, 0);

    /* initialize free list queues: */
    queue_init(&nqe_freeQ, 0);
    queue_init(&nre_freeQ, 0);

    /* setup nbp ID allocator: */
    id_new(&nbpidh, 0, 255, FALSE);

    entityStr_buf = malloc(sizeof(char) * (3*(ZONELEN*3) + 10));
    if (entityStr_buf == NULL)
	errmsg(AT_ERR_NOMEM, "nbp entity buffer", __LINE__, __FILE__);

    requestStr_buf = malloc(sizeof(char) * 200);
    if (requestStr_buf == NULL)
	errmsg(AT_ERR_NOMEM, "nbp request buffer", __LINE__, __FILE__);

    atnbp_nve_init();

    nbp_gccontext = atmaint_AddGCContext(nbp_empty_trash, 0);
}

/*
 * nbp_reqtypeStr -
 *	nbp data structures,
 *	from atalk_init(), once per reload.
 *	reinit option?
 *	
 */
static char *
atnbp_reqtypeStr (nbp_reqtype tt)
{
    switch (tt) {
      case NBP_LOOKUP:	return("NBP_LOOKUP");
      case NBP_CONFIRM:	return("NBP_CONFIRM");
      default:		return("nbp_reqtypeStr:  bad arg"); 
    }
}

/*
 * nbp_requestStr -
 *	data structures,
 *	atalk_init(), once per reload.
 *	option?
 *	
 */
static char *
nbp_requestStr (nbp_request *rr)
{
    if (rr) {
        sprintf(requestStr_buf,
		"req: %s clk=%d id=%d uQ=%lx pak=%lx tm=%s\n\t '%s'",
		atnbp_reqtypeStr(rr->reqtype),
		rr->reqClock,
		rr->NBPid,
		rr->uqw,
		rr->pak,
		atnbp_timerStr(&rr->tm),
		atnbp_entityStr(&rr->nve));
        return (requestStr_buf);
    }

    return ("NULL_REQUEST");
}

/*************************  PRIVATE FUNCTIONS  ************************/


#if 0
/*
 * get_ddpaddr -
 *	
 *	switches
 *	
 */
boolean
atalk_get_ddpaddr (char *ln, ataddress *ddp)
{
    boolean parsed = FALSE;

    /* get net #: */
    printf("Net # [%d]:  ", ddp->net); flush();
    if (!rdtty(ln,MAXLINE))
        return (FALSE);
    if (!EMPTYSTR(ln)) {
        ddp->net = ATOI(ln, &parsed);
	if (!parsed)
	    return (FALSE);
    }
    
    /* get node #: */
    printf("Node # [%d]:  ", ddp->node); flush();
    if (!rdtty(ln,MAXLINE))
        return (FALSE);
    if (!EMPTYSTR(ln)) {
        ddp->node = ATOI(ln, &parsed);
	if (!parsed)
	    return (FALSE);
    }

    /* get socket #: */
    printf("Socket # [%d]:  ", ddp->spec); flush();
    if (!rdtty(ln,MAXLINE))
        return (FALSE);
    if (!EMPTYSTR(ln)) {
        ddp->spec = ATOI(ln, &parsed);
	if (!parsed)
	    return (FALSE);
    }

    return (TRUE);
}
#endif




/*************************  EXPORTED FUNCTIONS  ***********************/


/*************************  PRIVATE FUNCTIONS  ************************/

/*
 * nve_setresult -
 *	set results of successful search
 */
static void
nve_setresult (nveSearch *ss, nveEntry *tab, ushort idx)
{

    str32init(&ss->nve.srvr_name, SY_STRING(tab->nameSym));
    str32init(&ss->nve.type_srvc, SY_STRING(tab->typeSym));
    str32init(&ss->nve.srvr_zone, SY_STRING(tab->zoneSym));

    ss->net     = tab->net;
    ss->socket  = tab->socket;
    ss->node    = tab->node;
    ss->enumerator  = tab->enumerator;

    /* save index from nveSequential: */
    ss->idx     = idx;

}

/*
 * atnbp_nve_purge  -
 *	internal routine to purge an nve from database
 */
static boolean
atnbp_nve_purge (nveSearch *ss)
{
    key *kk = &gkstor;			/* use global scratch key */

    /* DELETE from:
     *  nveEntities:  (nameSym, typeSym, zoneSym, idx)
     *  nveSequential:  (idx, nveEntry)
     *  nveSockets:  (socket, node, net, enumerator, idx)
     */

    db_mkkey(kk,4, nveEntities, ss->luName, ss->luType, ss->luZone);
    DB_DELETE(kk);

    db_mkkey(kk, 2, nveSequential, ss->idx);
    DB_DELETE(kk);

    db_mkkey(kk,5,nveSockets,ss->socket,ss->net,ss->node,ss->enumerator);
    DB_DELETE(kk);

    /* return idx to pool: */
    id_delid(nveidh, ss->idx);

    /* decrement/delete symbol refs: */
    SY_DELETE(ss->luName);
    SY_DELETE(ss->luType);
    SY_DELETE(ss->luZone);

    return (TRUE);
}


/*************************  EXPORTED FUNCTIONS  ***********************/

/*
 * atnbp_nve_add -
 *	
 *	add an new NVE to our table.
 *	called from atnbp_register().
 *	
 *	if <tentative> is TRUE, then mark entry as tentative
 *	(registration is pending).
 *	
 */
static boolean
atnbp_nve_add (nbpEntity *nve, ataddress *src, boolean tentative)
{
    symbol	nameSym;
    symbol	typeSym;
    symbol	zoneSym; 
    key		key1;
    key		key2;
    key		key3;
    nveEntry	tab;
    ushort	idx;
    short	nbpenum;

    if (at_debug(atalknbp_debug, ATALK_LOGLEVEL_MASK, 3))
        buginf("AT: atnbp_nve_add: add '%s' @(%s)\n",
	       atnbp_entityStr(nve), ddpAddrStr(src));

    /* don't allow "*" to be registered: */
    if (ISLOCALZONE(&nve->srvr_zone)) {
        if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
            buginf("AT: atnbp_nve_add:  local zone<<\n");
        return (FALSE);
    }

    nameSym = SY_APPEND32(&nve->srvr_name);
    typeSym = SY_APPEND32(&nve->type_srvc);
    zoneSym = SY_APPEND32(&nve->srvr_zone);

    /*
     * if already in table...
     * nveEntities:  (nameSym, typeSym, zoneSym, idx)
     */
    db_mkkey(&key1,4,nveEntities,nameSym,typeSym,zoneSym);
    if (DB_RETRIEVE((byteptr) &idx, &key1, sizeof(ushort))) {
        if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
	    buginf("AT: atnbp_nve_add, nveEntities [%d] exists! '%s'(%s.%d %s.%d %s.%d)<<\n",
                idx,
                atnbp_entityStr(nve),
                SY_STRING(nameSym), nameSym,
                SY_STRING(typeSym), typeSym,
                SY_STRING(zoneSym), zoneSym);
        return (FALSE);
    }

    /* not in table - add: */
    if (!id_getid(nveidh, &idx)) {
        if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
            buginf("\nAT: atnbp_nve_add, no id's\n");
        return (FALSE);			/* no id's left (hard to believe!) */
    }

    if (!DB_APPEND(&key1, (byteptr) &idx, sizeof(ushort))) {
        id_delid(nveidh, idx);
        if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
            buginf("AT: atnbp_nve_add, nveEntities append\n");
        return (FALSE);			/* no memory */
    }

    if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
        buginf("\nAT: atnbp_nve_add, appended nveEntities [%d] '%s'(%s.%d %s.%d %s.%d)\n",
                idx,
                atnbp_entityStr(nve),
                SY_STRING(nameSym), nameSym,
                SY_STRING(typeSym), typeSym,
                SY_STRING(zoneSym), zoneSym);

    /*
     * add to:
     * nveSockets:  (socket, node, net, nbpenum, idx)
     * NOTE:  as per spec, can have multiple names for the
     * same socket.
     */

    /* find the next available nbpenum for this address: */
    for (nbpenum = 1; nbpenum < MAXALIASES; nbpenum++) {
        db_mkkey(&key2,5,
            nveSockets,src->spec,src->net,src->node,nbpenum);
        if (!DB_RETRIEVE(NILPTR, &key2, sizeof(ushort)))
            break;
    }

    if (nbpenum >= MAXALIASES) {
        /* internal error: */
        at_err(MODID,"atnbp_nve_add:  warning: MAXALIASES exceeded!");
        id_delid(nveidh, idx);
        return (FALSE);
    }

    if (!DB_APPEND(&key2, (byteptr) &idx, sizeof(ushort))) {
        /* failed - backout: */
        DB_DELETE(&key1);
        id_delid(nveidh, idx);
        if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
            buginf("\nAT: atnbp_nve_add, nveSockets append failed");
        return (FALSE);			/* FAILED - no memory */
    }

    /* add to:
     *  nveSequential:  (idx, nveEntry)
     */

    tab.nameSym = nameSym;
    tab.typeSym = typeSym;
    tab.zoneSym = zoneSym;
    tab.net     = src->net;
    tab.socket  = src->spec;
    tab.node    = src->node;
    tab.enumerator  = nbpenum;
    tab.tentative   = tentative;

    db_mkkey(&key3, 2, nveSequential, idx);
    if (!DB_APPEND(&key3, (byteptr) &tab, sizeof(nveEntry))) {
        /* failed - backout: */
        DB_DELETE(&key1);
        DB_DELETE(&key2);
        id_delid(nveidh, idx);
        if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
            buginf("\nAT: atnbp_nve_add, nveSequential append failed, no memory");
        return (FALSE);			/* FAILED - no memory */
    }

    return (TRUE);
}

/*
 * atnbp_nve_setsearch -
 *	
 *	lookup symbols associated with <nve>, check for wildcards.
 *	
 */
static boolean
atnbp_nve_setsearch (nveSearch *ss, nbpEntity *nve)
{

    /* set defaults: */
    memset(ss, 0, sizeof(nveSearch));
    ss->idx     = NILID;

    /* NOTE:  do type match first, as can bale out early if
     * specified, i.e., *:LaserWriter@*
     */

    /* if type of service has wildcard... */
    if (ISNBP_ANY(&nve->type_srvc))
        ss->luType = NILSYM;    /* ...do wildcard match */
    else {
        ss->luType = SY_SYMBOL32(&nve->type_srvc);
        if (ss->luType == NILSYM) {
            if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
                buginf("\nAT: atnbp_nve_setsearch:  no luType, '%s'",
		       STR32PTR(&nve->type_srvc));
            return (FALSE);
        }
    }

    if (ISNBP_ANY(&nve->srvr_name)) {
        ss->luName = NILSYM;
    } else {
        ss->luName = SY_SYMBOL32(&nve->srvr_name);
        /* if symbol not entered... */
        if (ss->luName == NILSYM) {
            if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
                buginf("\nAT: atnbp_nve_setsearch:  no luName, '%s'",
		       STR32PTR(&nve->srvr_name));
            return (FALSE);		/* then not in table */
        }
    }

    if (ISLOCALZONE(&nve->srvr_zone)) {
        /* match on any zone: */
        ss->luZone = NILSYM;
    } else {
        ss->luZone = SY_SYMBOL32(&nve->srvr_zone);
        if (ss->luZone == NILSYM) {
            if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
                buginf("\nAT: atnbp_nve_setsearch:  no luZone, '%s'",
		       STR32PTR(&nve->srvr_zone));
            return (FALSE);
        }
    }

    return (TRUE);
}

/*
 * nve_firstmatch -
 *	
 *	return first registered NVE entry matching <nve>.
 *	initializes <nss>, which must be allocated by caller.
 *	
 */
nveSearch *
atnbp_nve_firstmatch (nveSearch *ss, nbpEntity *nve)
{
    key		*kk = &gkstor;		/* use global scratch key */
    nveEntry	*tab = &gtabstor;
    ushort	idx;

    /* if wildcards... */
    if (WILDCARDSEARCH(ss)) {
        /* ...then must do a sequential match: */
        if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
            buginf("\nAT: atnbp_nve_firstmatch:  sequential for '%s', DDP net=%d",
		   atnbp_entityStr(nve), ss->net);

	/* optimize key creation: */
        idx = id_first(nveidh);
	db_mkkey(kk, 2, nveSequential, idx);

        for (; idx != NILID; idx = id_next(nveidh,idx)) {
            /* get each record until match: */
	    kk->keys[1] = idx;		/* optimize key creation */
            if (!DB_RETRIEVE((byteptr) tab,kk,sizeof(nveEntry))) {
                /* internal error! */
                at_err(MODID,"atnbp_nve_firstmatch: bad idx!");
                return (NILNVE);
            }

            /* compare using wild cards */
            if (NVEMATCH(ss, tab)) {
                /* copy result, return pointer: */
                nve_setresult(ss, tab, idx);
                return (ss);
            }
        }

        /* none found: */
        if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
            buginf("\nAT: atnbp_nve_firstmatch, search failed");
        return (NILNVE);
    }

    /*
     * otherwise, look in:
     * nveEntities:  (ss->luName, ss->luType, ss->luZone, idx)
     */
    db_mkkey(kk, 4, nveEntities, ss->luName, ss->luType, ss->luZone);
    if (!DB_RETRIEVE((byteptr) &idx,kk,sizeof(ushort))) {
        /* no match: */
        return (NILNVE);
    }


    /* found - use index to retrieve nveEntry record: */
    db_mkkey(kk, 2, nveSequential, idx);
    if (!DB_RETRIEVE((byteptr) tab,kk,sizeof(nveEntry))) {
        /* internal error! */
        at_err(MODID,"atnbp_nve_firstmatch: bad idx (2)!");
        return (NILNVE);
    }

    /* if net specified it must match: */
    if (ss->net !=0 && ss->net != tab->net)
        return (NILNVE);

    /* if registration pending, don't return: */
    if (tab->tentative) {
        return (NILNVE);
    }

    /* copy result into search structure and return pointer to it: */
    nve_setresult(ss, tab, idx);

    return (ss);
}

/*
 * nve_nextmatch -
 *	
 *	get the next record in nveSequential that matches search spec in <ss>.
 *	
 */
static nveSearch *
atnbp_nve_nextmatch (nveSearch *ss)
{
    ushort	idx = ss->idx;
    key		*kk = &gkstor;		/* use global scratch key */
    nveEntry	*tab = &gtabstor;

    /* if finished scan, or no records allocated */
    if (idx == NILID || id_first(nveidh) == NILID)
        return (NILNVE);

    /* if no wildcard, no more matches: */
    if (!WILDCARDSEARCH(ss))
        return (NILNVE);

    /* resume search at next index: */
    for (idx = id_next(nveidh,idx); idx != NILID; idx = id_next(nveidh,idx)) {
        db_mkkey(kk, 2, nveSequential, idx);
        if (!DB_RETRIEVE((byteptr) tab,kk,sizeof(nveEntry))) {
            /* internal error! */
            at_err(MODID,"atnbp_nve_firstmatch: bad idx!");
            return (NILNVE);
        }

        /* compare using wild cards */
        if (NVEMATCH(ss, tab)) {
            /* copy result, return pointer: */
            nve_setresult(ss, tab, idx);
            return (ss);
        }
    }

    /* none found: */
    ss->idx = NILID;
    return (NILNVE);
}

/*
 * atnbp_nve_delete -
 *	
 *	delete a registered NVE if it exists.  true if found & deleted.
 *	
 */
boolean
atnbp_nve_delete (nbpEntity *nve)
{
    nveSearch *ss = &gsearchstor;

    if (!atnbp_nve_setsearch(ss, nve))
        return (FALSE);		/* terms not in symbol table */

    /* Don't care if we match net.  (see NVEMATCH in atalk_nbp.h) */
    ss->net = 0;

    /* if <nve> not in table... */
    if (atnbp_nve_firstmatch(ss, nve) == NILNVE)
        return (FALSE);

    return (atnbp_nve_purge(ss));
}

/*
 * atnbp_nve_init -
 *	
 *	init module.  okay to call multiple times.
 *	
 */
boolean
atnbp_nve_init (void)
{
    static int ninits = 0;
    int badsyms = 0;
    db_result dberr;
    sy_result syerr;

    if (ninits > 0)
        return(TRUE);       /* already init'd */

    /* set up id allocator to reuse deleted id's: */
    if (!id_new(&nveidh, 1, MAX_NVES, TRUE)) {
        at_err(MODID,"atnbp_nve_init: can't allocate ID handle.");
        return(FALSE);
    }

    /* alocate a new symbol table for NVE's: */
    nve_symtab = sy_init("nve_symbols", (PFI)atalk_upper, &syerr);
    if (syerr != SY_OK) {
        at_err(MODID,"mdb_init: can't init symbol table, %s",
            sy_resultStr(syerr));
        /* backout: */
        id_free(&nveidh);
        return(FALSE);
    }

    if ((nveSockets = SY_APPEND("nveSockets")) == NILSYM)
        badsyms++;
    if ((nveEntities = SY_APPEND("nveEntities")) == NILSYM)
        badsyms++;
    if ((nveSequential = SY_APPEND("nveSequential")) == NILSYM)
        badsyms++;

    if (badsyms > 0) {
        /* backout: */
        id_free(&nveidh);
        sy_destroy(nve_symtab);
        return(FALSE);
    }

    /* alocate a new database for storing NVE's: */
    nve_db = db_init("nve_database", &dberr);
    if (dberr != DB_OK && dberr != DB_NAME_INUSE) {
        id_free(&nveidh);
        sy_destroy(nve_symtab);
        at_err(MODID,"mdb_init: can't init database, %s",
            db_resultStr(dberr));
        return(FALSE);
    }

    nvedump_buf = malloc(sizeof(char) * 80);
    if (nvedump_buf == NULL)
	errmsg(AT_ERR_NOMEM, "nve dump buffer", __LINE__, __FILE__);
    else
	sprintf(nvedump_buf,"%-5s%-4s%-4s %24s%16s%24s",
		"Net", "Adr", "Skt", "Name", "Type", "Zone");

    ++ninits;
    return(TRUE);
}

/*
 * atnbp_nve_dump -
 *	
 *	print the name registration table
 *	
 */
void
atnbp_nve_dump (void)
{
    key		*kk = &gkstor;		/* use global scratch key */
    nveEntry	*tab = &gtabstor;
    ushort	idx;

    automore_header(nvedump_buf);

    for (idx = id_first(nveidh); idx != NILID; idx = id_next(nveidh,idx)) {
        /* nveSequential:  (idx, nveEntry) */
        db_mkkey(kk, 2, nveSequential, idx);
        if (!DB_RETRIEVE((byteptr) tab, kk, sizeof(nveEntry))) {
            at_err(MODID,"atnbp_nve_dump:  missing nve entry, #%d", idx);
            continue;   /* error - not found */
        }

        /* dump the entry: */
        printf("\n%-5d%-4d%-4d %24s%16s%24s%s",
	       tab->net,
	       tab->node,
	       tab->socket,
	       at_hex_display(SY_STRING(tab->nameSym)),
	       at_hex_display(SY_STRING(tab->typeSym)),
	       at_hex_display(SY_STRING(tab->zoneSym)),
	       tab->tentative?"*":"");
	
    }
    automore_header(NULL);
}

/*
 * nve_snmpget
 *	
 *	
 */
boolean
nve_snmpget (long nbpIndex, uchar** objectp, uchar** typep, uchar** zonep)
{
    key		*kk = &gkstor;		/* use global scratch key */
    nveEntry	*tab = &gtabstor;
    ushort	idx;

    for (idx = id_first(nveidh);
	 idx != NILID;
	 idx = id_next(nveidh,idx)) {

        /* nveSequential:  (idx, nveEntry) */
        db_mkkey(kk, 2, nveSequential, idx);
        if (!DB_RETRIEVE((byteptr) tab, kk, sizeof(nveEntry))) {
            at_err(MODID,"atnbp_nve_dump:  missing nve entry, #%d", idx);
            continue;   /* error - not found */
        }

	if (idx == nbpIndex)
	    break;
    }

    if (idx == NILID)
	return (FALSE);
    else {
	*objectp = at_hex_display(SY_STRING(tab->nameSym));
	*typep = at_hex_display(SY_STRING(tab->typeSym));
	*zonep = at_hex_display(SY_STRING(tab->zoneSym));
	return (TRUE);
    }
}

/*
 * nve_status -
 *	lookup and report the status of <nve>
 */
static nve_result
atnbp_nve_status (nbpEntity *nve)
{
    key		*kk = &gkstor;		/* use global scratch key */
    nveSearch	*ss = &gsearchstor;
    nveEntry	*tab = &gtabstor;
    ushort	idx;

    if (!atnbp_nve_setsearch(ss, nve))
        return (NVE_NOT_FOUND);		/* not in table or bad args */

    /* Don't care if we match net.  (see NVEMATCH in atalk_nbp.h) */
    ss->net = 0;

    /*
     * nveEntities:  (ss->luName, ss->luType, ss->luZone, idx)
     */
    db_mkkey(kk, 4, nveEntities, ss->luName, ss->luType, ss->luZone);
    if (!DB_RETRIEVE((byteptr) &idx,kk,sizeof(ushort))) {
        /* no match: */
        return (NVE_NOT_FOUND);		/* not in table or bad args */
    }

    /* found - use index to retrieve nveEntry record: */
    db_mkkey(kk, 2, nveSequential, idx);
    if (!DB_RETRIEVE((byteptr) tab, kk, sizeof(nveEntry))) {
        /* internal error! */
        at_err(MODID,"nve_status: bad idx!");
        return (NVE_MODULE_ERROR);
    }

    if (tab->tentative)
        return (NVE_TENTATIVE);
    else
        return (NVE_PERMANENT);
}

/*
 * nve_reg_complete -
 *	mark <nve> as registered
 */
static nve_result
atnbp_nve_reg_complete (nbpEntity *nve)
{
    key		*kk = &gkstor;		/* use global scratch key */
    nveSearch	*ss = &gsearchstor;
    nveEntry	*tab = &gtabstor;
    ushort	idx;

    if (!atnbp_nve_setsearch(ss, nve))
        return (NVE_NOT_FOUND);		/* not in table or bad args */

    /* Don't care if we match net.  (see NVEMATCH in atalk_nbp.h) */
    ss->net = 0;

    /*
     * nveEntities:  (ss->luName, ss->luType, ss->luZone, idx)
     */
    db_mkkey(kk, 4, nveEntities, ss->luName, ss->luType, ss->luZone);
    if (!DB_RETRIEVE((byteptr) &idx,kk,sizeof(ushort))) {
        /* no match: */
        return (NVE_NOT_FOUND);  /* not in table or bad args */
    }

    /* found - use index to retrieve nveEntry record: */
    db_mkkey(kk, 2, nveSequential, idx);
    if (!DB_RETRIEVE((byteptr) tab,kk,sizeof(nveEntry))) {
        /* internal error! */
        at_err(MODID,"nve_reg_complete: retrieve failed");
        return (NVE_MODULE_ERROR);
    }

    tab->tentative = FALSE;
    if (!DB_REPLACE(kk, (byteptr) tab, sizeof(nveEntry))) {
        at_err(MODID,"nve_reg_complete: replace failed");
        return (NVE_MODULE_ERROR);
    }

    return (NVE_OK);
}

/*
 * atnbp_nve_resultStr -
 *	
 *	return char translation of result code.
 *	
 */
char *
atnbp_nve_resultStr (nve_result ii)
{
    switch(ii) {
      case NVE_NOT_FOUND: 	return ("NVE_NOT_FOUND,");
      case NVE_PERMANENT: 	return ("NVE_PERMANENT,");
      case NVE_TENTATIVE: 	return ("NVE_TENTATIVE,");
      case NVE_MODULE_ERROR:  	return ("NVE_MODULE_ERROR,");
      case NVE_OK:        	return ("NVE_OK");
      default:            	return ("atnbp_nve_resultStr:  bad arg");
    }
}
