/* $Id: macip_db.c,v 3.4.6.4 1996/07/02 17:00:48 slin Exp $
 * $Source: /release/112/cvs/Xsys/atalk/macip_db.c,v $
 *------------------------------------------------------------------
 * macip_db.c   - implement macip data structures.
 *
 * September 1991, Russ Tremain, Tremain Laboratories, under contract
 *  for cisco Systems, Inc.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: macip_db.c,v $
 * Revision 3.4.6.4  1996/07/02  17:00:48  slin
 * CSCdi61526:  MacIP ipgpAssign Error - Overlapping IP address with
 * client range
 * Branch: California_branch
 * - During IP address assignment, skip addresses that fail IP alias chec
 *
 * Revision 3.4.6.3  1996/06/28  23:04:56  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.6.2  1996/04/15  21:12:38  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.4.6.1  1996/03/23  01:28:25  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.4  1996/03/05  22:13:32  kleung
 * CSCdi49904:  atalk_upper vs atalk_cvu are equivalent
 *
 * Revision 3.3  1996/01/18  19:39:33  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  08:44:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:51:04  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:10:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <interface_private.h>
#include "packet.h"
#include "config.h"
#include "../ip/ip.h"
#include "../ip/ipaddress.h"
#include "atalk_private.h"
#include "../atalk/atip_debug.h"
#include "macip_util.h"
#include "atalk_nbp.h"
#include "macip_db.h"
#include "macip_fsm.h"
#include "macip_server.h"
#include "../ip/ip_registry.h"

/* definitions for at_err(): */
#define DEBUGSWITCH (at_debug(appletalk_debug, ATALK_NULL_MASK))

/* to save stack space: */
static key keystor;

/* true if macip configuration is active: */
boolean macip_configured = FALSE;
/* true if initialization okay: */
boolean macip_initialized = FALSE;

/* exported symbol table, database handles for parser: */
symbol_table    macip_symtab;
database    macip_db;

/* exported relation symbols (see macip_db.h): */
symbol server_client, clients, client_ip;
symbol servers; 

static symbol client_ddp, server_resources;

/* id allocator for server id's: */
iddesc   srvr_idh = NILIDDESC;

/* limits: */
#define MAX_SERVERS      64 /* max. number of servers */
#define MAX_SERVER_CLIENTS  1024 /* max. clients per servers */
#define MAX_SERVER_RESOURCES     64 /* max. ip-ranges per server */


/*************************  PRIVATE FUNCTIONS  ************************/
static void
nv_macip_resources (parseinfo *csb, server_rec *ss)
/*
**  write server resources
*/
{
    register key *kk = &keystor;
    register iddesc idh = ss->rsrc_idh;
    register int ii;
    resource_rec *rsrc;

    /* foreach server resource.... */
    for (ii = id_first(idh); ii != NILID; ii = id_next(idh,ii)) {
        /*
        ** server_resources:
        **  (short serverid, short enum, resource_rec *rsrc)
        */
        db_mkkey(kk, 3, server_resources, ss->serverid, ii);
        if (!DB_RETRIEVE((byteptr) &rsrc, kk, sizeof(rsrc)))
        {
            at_err(MODID,"nv_macip_resources:  no resource rec,sid=%d",
                ss->serverid);
            break;
        }

        /* RESOURCE RECORD: */
        nv_write(TRUE, "%s %s %i", csb->nv_command,
            (rsrc->is_static)?"static":"dynamic", rsrc->lowip);

        nv_add(rsrc->lowip != rsrc->highip, " %i", rsrc->highip);

        nv_add(TRUE, " zone ");
	atalk_nv_printzone(at_hex_display(SY_STRING(ss->zone)));
    }
}

/*
 * mdb_purge_active_clients -
 *	delete clients shown by resource allocation mask
 */
static void
mdb_purge_active_clients (server_rec *ss, resource_rec *rr)
{
    register bitarray bb = rr->allocd;
    register int ii;
    register client_rec *cptr;
    ipaddrtype cip;

    /* foreach IP address for resource... */
    for (ii=0; ii< (rr->highip - rr->lowip)+1; ii++) {
        /* if address is allocated... */
        if (bit_val(bb,ii)) {
            cip = rr->lowip+ii;
            /* if client record exists... */
            if ((cptr = mdb_clientbyIP(cip)) != NULL)
                /* ... then nuke the client: */
                mdb_purge_client(ss, cptr->ncli);
            else
		at_err(MODID,"mdb_purge_active_clients:, allocd, no client, %i",
			cip);
        }
    }
}

/*
 * mdb_purge_resource -
 *	
 *	purge a resource and it's clients
 *	
 */
static void
mdb_purge_resource (server_rec *ss, resource_rec *rr)
{
    register key *kk = &keystor;

    /* purge clients: */
    mdb_purge_active_clients(ss, rr);

    /* delete server ref to resource: */
    id_delid(ss->rsrc_idh, rr->resourceid);

    /* delete the resource:
     * server_resources:
     *  (short serverid, short enum, resource_rec *rsrc)
     */
    db_mkkey(kk, 3, server_resources, ss->serverid, rr->resourceid);
    DB_DELETE(kk);

    /* free allocation map: */
    bit_free(&rr->allocd);

    /* free resource record: */
    free(rr);
}

/*
 * dmp_server_resources -
 *	
 *	a resource and it's clients
 *	
 */
static void
dmp_server_resources (short sid, server_rec *ss)
{
    register key *kk = &keystor;
    register iddesc idh = ss->rsrc_idh;
    register int ii;
    resource_rec *rsrc;

    /* foreach server resource.... */
    for (ii = id_first(idh); ii != NILID; ii = id_next(idh,ii)) {
        /*
         * server_resources:
         *  (short serverid, int16 enum, resource_rec *rsrc)
         */
        db_mkkey(kk, 3, server_resources, sid, ii);
        if (!DB_RETRIEVE((byteptr) &rsrc, kk, sizeof(rsrc)))
        {
            at_err(MODID,"dump serv. rsrc:  no resource rec,sid=%d",sid);
            break;
        }

        /* RESOURCE RECORD: */
        printf("\tResource #%d %s %i-%i, %d/%d IP in use\n",
                ii,
                (rsrc->is_static)?"STATIC":"DYNAMIC",
                rsrc->lowip,
                rsrc->highip,
                bit_nset(rsrc->allocd),
                bit_nbits(rsrc->allocd));
    }
}

/*
 * mdb_dump_client -
 *	dump a single client record, check client cross-refs
 */
static void
mdb_dump_client (server_rec *ss, ushort cid)
{
    register key	*kk = &keystor;
    ataddress		*dd;
    ipaddrtype 		cip;
    client_rec		*cptr;
    client_rec		*cptr2;

    /*
     * server_client:   (serverid, enum, client_rec *cptr)
     */
    db_mkkey(kk, 3, server_client, ss->serverid, cid);
    if (!DB_RETRIEVE((byteptr) &cptr, kk, sizeof(cptr))) {
        /* no record! */
        at_err(MODID,"mdb_dump_client:  client not found sid=%d cid=%d",
            ss->serverid, cid);
        return;
    }

    if (cptr->srec != ss)
        printf("bad server crossref!\n");

    if (cid != cptr->ncli)
        printf("bad client enum crossref!\n");

    /*
     * check client_ddp ref:
     * client_ddp:  (ddpnet, ddpnode, client_rec *cptr)
     */
    dd = &cptr->ddpaddr;
    db_mkkey(kk, 3, client_ddp, dd->net, dd->node);
    if (!DB_RETRIEVE((byteptr) &cptr2, kk, sizeof(cptr2))) {
        printf("no ddp crossref!\n");
    }

    if (cptr2 != cptr)
        printf("bad ddp crossref!\n");

    /*
     * cleck client_ip ref:
     * client_ip:   (ipaddr_hi, ipaddr_lo, client_rec *cptr)
     */
    cip = cptr->ipaddr;
    db_mkkey(kk, 3, client_ip, HI_16(cip), LO_16(cip));
    if (!DB_RETRIEVE((byteptr) &cptr2, kk, sizeof(cptr2))) {
        printf("no ip crossref!\n");
    }

    if (cptr2 != cptr)
        printf("Bad IP crossref!\n");

    /* dump the client record: */
    printf("%i@[%s] %ld secs    '%s'\n",
	   cip, ddpAddrStr(dd), ELAPSED_TIME(cptr->timer) / ONESEC,
	   at_hex_display(SY_STRING(ss->zone)));
}

/*
 * mdb_dump_clients -
 *	dump client records for <ss>
 */
static void
mdb_dump_clients (server_rec *ss)
{
    iddesc	idh;
    int		ii;

    /* foreach client.... */
    idh = ss->ncli_idh;
    for (ii = id_first(idh); ii != NILID; ii = id_next(idh,ii)) {
        mdb_dump_client(ss, ii);
    }
}


/*************************  EXPORTED FUNCTIONS  ***********************/

/*
 * ipaddrStr -
 *	return a string representation of an IP address.
 *	can nest up to MAX_NEST calls.
 */
char *
ipaddrStr (ipaddrtype iaddr)
{
#define MAX_NEST    4
#define MAX_IPSTR   (3*4)+3+1   /* 255.255.255.255 + NULL */
    static char ipstr[MAX_NEST][MAX_IPSTR];
    static	int nresult=0;
    int		ii;

    ii = nresult;
    sprintf(ipstr[ii],"%i",iaddr);

    ++nresult;
    if (nresult == MAX_NEST)
        nresult=0;

    return (ipstr[ii]);
}

/*
 * mdb_show_servers -
 *	dump the current macip database
 */
void
mdb_show_servers (void)
{
    key		*kk = &keystor;
    server_rec	*ss;
    int		sid;

    if (srvr_idh == NILIDDESC)
        return;				/* database not inited */

    /* foreach server.... */
    for (sid = id_first(srvr_idh); sid != NILID;
	 sid = id_next(srvr_idh, sid)) {
        /*
         * servers: (short serverid, server_rec *ss)
         */

        db_mkkey(kk, 2, servers, sid);
        if (!DB_RETRIEVE((byteptr) &ss, kk, sizeof(ss))) {
            /* error - id w/no entry */
            at_err(MODID,"show_servers:  no server record, sid=%d",
                sid);
            continue;   /* dump next rec */
        }

        /* otherwise, got a server record.  dump it: */
        printf("MACIP SERVER %d, IP %i,  ZONE '%s' STATE is %s\n",
            sid,
            ss->ipaddr,
            at_hex_display(SY_STRING(ss->zone)),
            macip_stateStr(ss->stateFunc));

        /* print server info: */
        dmp_server_resources(sid, ss);
    }
}

/*
 * mdb_show_clients -
 *	dump macip client info
 */
void
mdb_show_clients (void)
{
    register key *kk = &keystor;
    server_rec *ss;
    register int sid;

    if (srvr_idh == NILIDDESC)
        return;     /* database not inited */

    /* foreach server.... */
    for (sid = id_first(srvr_idh); sid != NILID;
        sid = id_next(srvr_idh, sid))
    {
        /*
         * servers: (short serverid, server_rec *ss)
         */

        db_mkkey(kk, 2, servers, sid);
        if (!DB_RETRIEVE((byteptr) &ss, kk, sizeof(ss))) {
            /* error - id w/no entry */
            at_err(MODID,"mdb_show_clients:  no server record, sid=%d",
                sid);
            continue;   /* dump next rec */
        }

        /* otherwise, got a server record.  dump client info: */
        mdb_dump_clients(ss);
    }
}

/*
 * mdb_init -
 *	init macip data structures.
 *	only supposed to do work once per reload, but may be called
 *	multiple times.
 *	
 */
boolean
mdb_init (void)
{
    static int ninits = 0;
    int badsyms = 0;
    db_result dberr;
    sy_result syerr;

    if (ninits > 0) 
        return(TRUE);       /* already successfully init'ed */

    /* alocate a id-allocator for server id's, with delete cache: */
    if (!id_new(&srvr_idh, 1, MAX_SERVERS, TRUE)) {
        at_err(MODID,"mdb_init: can't get id allocator (serverids)");
        return(FALSE);
    }

    /* alocate a new symbol table for macip: */
    macip_symtab = sy_init("macip_symbols", (PFI)atalk_upper, &syerr);
    if (syerr != SY_OK) {
        /* back out: */
        id_free(&srvr_idh);
        at_err(MODID,"mdb_init: can't init symbol table, %s",
            sy_resultStr(syerr));
        return(FALSE);
    }

    /*
     * WARNING:  SY_ and DB_ procs are actually macro calls,
     * and depend on the MY_SYMTAB & MY_DATABASE defines in
     * macip_parse.h
     */

    /* init symbols: */
    if ((client_ddp = SY_APPEND("client_ddp")) == NILSYM)
        badsyms++;
    if ((client_ip  = SY_APPEND("client_ip")) == NILSYM)
        badsyms++;
    if ((clients        = SY_APPEND("clients")) == NILSYM)
        badsyms++;
    if ((server_client  = SY_APPEND("server_client")) == NILSYM)
        badsyms++;
    if ((server_resources= SY_APPEND("server_resources")) == NILSYM)
        badsyms++;
    if ((servers        = SY_APPEND("servers")) == NILSYM)
        badsyms++;

    if (badsyms > 0) {
        at_err(MODID,"mdb_init: can't init %d relation symbols.",
            badsyms);
        /* back out: */
        id_free(&srvr_idh);
        sy_destroy(macip_symtab);
        return(FALSE);
    }

    /* alocate a new database for macip: */
    macip_db = db_init("macip_database", &dberr);
    if (dberr != DB_OK && dberr != DB_NAME_INUSE) {
        /* back out: */
        id_free(&srvr_idh);
        sy_destroy(macip_symtab);
        at_err(MODID,"mdb_init: can't init database, %s",
            db_resultStr(dberr));
        return(FALSE);
    }

    ninits++;
    return(TRUE);
}

/*
 * mdb_app_resource -
 *	add a new address range to a server resource list, mark as static
 *	if <isstatic> is true.  return true if no errors.
 *	
 */
boolean
mdb_app_resource (boolean negate, ushort sid, ipaddrtype ll, ipaddrtype uu, boolean isstatic)
{
    register key *kk = &keystor;
    server_rec *ss;
    ushort rid;        /* resource-id */
    resource_rec *rsrc;
    idbtype *idb;

    if (ll > uu) {
        printf("\nBad IP range, %i-%i", ll, uu);
        return(FALSE);
    }

    /*
     * get server record:
     *
     * servers: (short serverid, server_rec *ss)
     */

    db_mkkey(kk, 2, servers, sid);
    if (!DB_RETRIEVE((byteptr) &ss, kk, sizeof(ss))) {
        /* error - id w/no entry */
        at_err(MODID,"mdb_app_resource:  no server record, sid=%d",
            sid);
        return(FALSE);
    }

    /* search current resources for duplicate: */
    for (rid = id_first(ss->rsrc_idh); rid != NILID;
        rid = id_next(ss->rsrc_idh, rid))
    {
        /*
         * server_resources:
         *  (ushort serverid, ushort enum, resource_rec *rsrc)
         */

        db_mkkey(kk, 3, server_resources, sid, rid);
        if (!DB_RETRIEVE((byteptr) &rsrc,kk,sizeof(rsrc)))
        {
            /* error - id w/no entry */
        at_err(MODID,"mdb_app_resource:  no resource, sid=%d, rid=%d",
                sid,rid);
            return(FALSE);
        }

        /* if address ranges match exactly... */
        if (rsrc->lowip == ll && rsrc->highip == uu) {
            if (negate) {
                /* purge resource & clients, inform state
                 * machine:
                 */
                mdb_purge_resource(ss,rsrc);
                macip_jam_event(ss, MACIP_FSM_DEL_RESOURCE, (ulong) rid);
                macip_service(ss);  /* do it now! */
                return(TRUE);
            }
            else	/* don't squawk, in case config is re-read */
                return(TRUE);
        }
        else
        if (INRANGE(rsrc->lowip, ll, rsrc->highip)
            || INRANGE(rsrc->lowip, uu, rsrc->highip))
        {
            /* OVERLAPING range: */
	    printf("\nOverlapping IP range, %i-%i", ll, uu);
            return(FALSE);
        }
    }

    /* if negate, no match: */
    if (negate) {
	printf("\nIP range %i - %i not found, can't delete.", ll, uu);
        return(FALSE);
    }

    /* finally, make sure server and client addresses are aliasable
     * to the same cable:
     */
    idb = ipaddr_findidb(ss->ipaddr);
    if (idb == NULL)
	return(FALSE);		/* message printed elsewhere */

    if (ipaddr_findidb(ll) != idb || ipaddr_findidb(uu) != idb) {
	printf("\n%s:  MacIP client range %i-%i not on server cable, %i",
		at_hex_display(SY_STRING(ss->zone)), ll, uu, ss->ipaddr);
	return(FALSE);
    }

    /*
     * set up resource record:
     */
    rsrc = malloc(sizeof(resource_rec));
    if (rsrc == (resource_rec *) NULL) {
        at_err(MODID,"mdb_app_resource:  no mem, resource record, sid=%d",
            sid);
        return(FALSE);
    }

    /* get bitarray to represent IP allocations: */
    if (!bit_new(&rsrc->allocd, uu-ll+1)) {
        at_err(MODID,"mdb_app_resource:  no mem bitmap, sid=%d",
            sid);
        /* back out: */
        free(rsrc);
        return(FALSE);
    }

    /* get next resource-id: */
    if (!id_getid(ss->rsrc_idh, &rid)) {
        at_err(MODID,"mdb_app_resource:  out of resource id's, sid=%d",
            sid);

        /* back out of transaction: */
        bit_free(&rsrc->allocd);
        free(rsrc);
        return(FALSE);
    }

    /* finish initializing resource: */
    rsrc->resourceid = rid;     /* my resource number */
    rsrc->is_static = isstatic;
    rsrc->lowip = ll;
    rsrc->highip = uu;
    rsrc->last = 0;

    /*
     * append server_resources relation:
     *
     * server_resources:
     *  (ushort serverid, ushort enum, resource_rec *rsrc)
     */
    db_mkkey(kk, 3, server_resources, sid, rid);
    if (!DB_APPEND(kk, (byteptr) &rsrc, sizeof(rsrc)))
    {
        at_err(MODID,"mdb_app_resource:  append failed, sid=%d, rid=%d",
            sid, rid);

        /* back out of transaction: */
        bit_free(&rsrc->allocd);
        id_delid(ss->rsrc_idh, rid);
        free(rsrc);

        return(FALSE);
    }

    /* success! */
    macip_add_event(ss, MACIP_FSM_ADD_RESOURCE, (ulong) rid);
    return(TRUE);
}

/*
 * mdb_app_server -
 *	add a new server to database tables.  return true with
 *	server-id in <usid> if successful.
 *	
 *	if statement is negated, then shutdown server, and purge it from db.
 *	
 */
boolean
mdb_app_server (boolean negate, ushort *usid, ipaddrtype iaddr, symbol zname)
{
    register key *kk = &keystor;
    server_rec *ss;
    ushort sid;

    *usid = NILID;  /* result */

    /* if server exists... */
    if (mdb_serverbyzone(&ss, zname)) {
        /* ...and "no server" statement: */
        if (negate) {
            /* ...then nuke it (with precedence): */
            macip_jam_event(ss, MACIP_FSM_DEL_SERVER, (ulong) 0);
            macip_service(ss);  /* do it now! */
            return(TRUE);
        }
        else {
	    if (iaddr == ss->ipaddr)
		/* don't squawk, in case config is being re-read */
		return(TRUE);
	    else
		/* must delete server and re-add to change IP address */
		printf("\n%s:  MacIP zone in use by %i",
		    SY_STRING(zname), ss->ipaddr);

            return(FALSE);
        }
    }

    if (ipaddr_findidb(iaddr) == NULL) {
	printf("\n%s:  MacIP IP %i, no cable", SY_STRING(zname), iaddr);
        return(FALSE);
    }

    /* get next server-id: */
    if (!id_getid(srvr_idh, &sid)) {
        at_err(MODID,"mdb_app_server:  no server id's available!");
        return(FALSE);
    }

    /* allocate a new server record: */
    if ((ss = malloc(sizeof(server_rec))) == NULL) {
        /* no memory - back out: */
        at_err(MODID,"mdb_app_server:  no memory for server record!");
        id_delid(srvr_idh, sid);
        return(FALSE);
    }

    /* set up record to append: */
    ss->serverid = sid;
    ss->ipaddr = iaddr;
    ss->zone = zname;
    ss->ddpidb = NULL;  /* set up when server comes up */
    ss->ipidb = NULL;
    ss->serverUP = FALSE;	/* true when server running */

    macip_init_state(ss);   /* initialize state machine */

    /* allocate resource enumerator: */
    if (!id_new(&ss->rsrc_idh , 1, MAX_SERVER_RESOURCES, TRUE)) {
        /* back out of transaction: */
        id_delid(srvr_idh, sid);
        return(FALSE);
    }

    if (!id_new(&ss->ncli_idh, 1, MAX_SERVER_CLIENTS, TRUE)) {
        /* back out of transaction: */
        id_delid(srvr_idh, sid);
        id_free(&ss->rsrc_idh);
        return(FALSE);
    }

    /*
     * add record to servers relation.
     *
     * servers: (short serverid, server_rec *ss)
     */

    db_mkkey(kk, 2, servers, sid);
    if (!DB_APPEND(kk, (byteptr) &ss, sizeof(ss))) {
        /* error - duplicate entry */
        at_err(MODID,"mdb_app_server:  append failed, servers, sid=%d", sid);

        /* back out of transaction: */
        id_delid(srvr_idh, sid);
        id_free(&ss->rsrc_idh);
        id_free(&ss->ncli_idh);

        return(FALSE);
    }

    /* success, start server, set results: */
    macip_add_event(ss, MACIP_FSM_ADD_SERVER, (ulong) 0);
    *usid = sid;
    macip_configured = TRUE;    /* we have a configd server */
    return(TRUE);
}

/*
 * mdb_serverbyzone -
 *	return server-id associated with <zname> in <usid>.  false if none.
 */
boolean
mdb_serverbyzone (server_rec **uss, symbol zname)
{
    register key *kk = &keystor;
    register ushort sid;
    server_rec *ss;

    *uss = NIL(server_rec);     /* result */

    /* foreach server.... */
    for (sid = id_first(srvr_idh); sid != NILID;
        sid = id_next(srvr_idh, sid))
    {
        /*
         * servers: (short serverid, server_rec *ss)
         */

        db_mkkey(kk, 2, servers, sid);
        if (!DB_RETRIEVE((byteptr) &ss, kk, sizeof(ss))) {
            /* error - id w/no entry */
            at_err(MODID,"mdb_serverbyzone:  no server record, sid=%d",
                sid);
            break;
        }

        /* if symbols match... */
        if (zname == ss->zone) {
            *uss = ss;
            return(TRUE);
        }
    }

    /* not found */
    return(FALSE);
}

/*
 * mdb_serverbyIP -
 *	return first server-id associated with <ipaddr>.
 *	false if none.
 */
boolean
mdb_serverbyIP (server_rec **uss, ipaddrtype sip)
{
    register key *kk = &keystor;
    register ushort sid;
    server_rec *ss;

    *uss = NIL(server_rec);     /* result */

    /* foreach server.... */
    for (sid = id_first(srvr_idh); sid != NILID;
        sid = id_next(srvr_idh, sid))
    {
        /*
         * servers: (short serverid, server_rec *ss)
         */

        db_mkkey(kk, 2, servers, sid);
        if (!DB_RETRIEVE((byteptr) &ss, kk, sizeof(ss))) {
            /* error - id w/no entry */
            at_err(MODID,"mdb_serverbyIP:  no server record, sid=%d",
                sid);
            break;
        }

        /* if IP addresses match... */
        if(sip == ss->ipaddr) {
            *uss = ss;
            return(TRUE);
        }
    }

    /* not found */
    return(FALSE);
}

/*
 * mdb_purge_client -
 *	
 *	purge a client from the database.
 *	
 */
void
mdb_purge_client (server_rec *ss, ushort cid)
{
    key		*kk = &keystor;
    ataddress	*dd;
    ipaddrtype	cip;
    client_rec	*cptr;

    /*
     * server_client:   (serverid, enum, client_rec *cptr)
     */
    db_mkkey(kk, 3, server_client, ss->serverid, cid);
    if (!DB_RETRIEVE((byteptr) &cptr, kk, sizeof(cptr))) {
        /* no record! */
        at_err(MODID,"mdb_purge_client:  client not found sid=%d cid=%d",
            ss->serverid, cid);
        return;
    }

    /* delete server_client ref: */
    DB_DELETE(kk);

    /*
     * delete client_ddp ref:
     * client_ddp:  (ddpnet, ddpnode, client_rec *cptr)
     */
    dd = &cptr->ddpaddr;
    db_mkkey(kk, 3, client_ddp, dd->net, dd->node);
    DB_DELETE(kk);

    /*
     * delete client_ip ref:
     * client_ip:   (ipaddr_hi, ipaddr_lo, client_rec *cptr)
     */
    cip = cptr->ipaddr;
    db_mkkey(kk, 3, client_ip, HI_16(cip), LO_16(cip));
    DB_DELETE(kk);

    /* unalias client IP address: */
    reg_invoke_ipalias_delete(cip);
    
    /* free ip address allocation: */
    mdb_free_ip(ss, cip);

    /* delete client enumerator: */
    id_delid(ss->ncli_idh, cid);

    /* un-malloc client record: */
    free(cptr);
}

/*
 * mdb_purge_server -
 *	
 *	delete server, resources from database.
 *	
 */
void
mdb_purge_server (server_rec *ss)
{
    register key *kk = &keystor;
    register iddesc idh;
    register int ii;
    resource_rec *rsrc;

    /* foreach client.... */
    idh = ss->ncli_idh;
    for (ii = id_first(idh); ii != NILID; ii = id_next(idh,ii)) {
        /* ...purge client info: */
        mdb_purge_client(ss, ii);
    }
    /* destroy client-enumerator handle: */
    id_free(&ss->ncli_idh);

    /* foreach server resource.... */
    idh = ss->rsrc_idh;
    for (ii = id_first(idh); ii != NILID; ii = id_next(idh,ii)) {
        /*
         * server_resources:
         *  (short serverid, short enum, resource_rec *rsrc)
         */
        db_mkkey(kk, 3, server_resources, ss->serverid, ii);
        /* retrieve and free resource record pointer: */
        if (DB_RETRIEVE((byteptr) &rsrc, kk, sizeof(rsrc))) {
            DB_DELETE(kk);
            /* free allocation map: */
            bit_free(&rsrc->allocd);
            /* free resource record: */
            free(rsrc);
        }
    }
    /* destroy resource-id handle: */
    id_free(&ss->rsrc_idh);

    /* de-reference zonename symbol: */
    SY_DELETE(ss->zone);

    /* delete server-id: */
    id_delid(srvr_idh, ss->serverid);

    /* delete server reference record: */
    db_mkkey(kk, 2, servers, ss->serverid);
    DB_DELETE(kk);

    delete_watched_queue(&ss->reqQ);
    delete_watched_queue(&ss->confirmQ);
    /* un-malloc server record: */
    free(ss);
    
    /* if last server, reset configd var: */
    macip_configured = (id_first(srvr_idh) != NILID);
}

void
mdb_nv_macip (parseinfo *csb)
/*
**  write out the current macip config
**  called from nv_macip() in macip_parse.c.
*/
{
    register key *kk = &keystor;
    server_rec *ss;
    register int sid;

    /* foreach server.... */
    for (sid = id_first(srvr_idh); sid != NILID;
        sid = id_next(srvr_idh, sid))
    {
        /*
        ** servers: (short serverid, server_rec *ss)
        */

        db_mkkey(kk, 2, servers, sid);
        if (!DB_RETRIEVE((byteptr) &ss, kk, sizeof(ss))) {
            /* error - id w/no entry */
            at_err(MODID,"nv_macip:  no server record, sid=%d", sid);
            break;
        }

        /* otherwise, got a server record: */
        nv_write(TRUE, "%s server %i", csb->nv_command, ss->ipaddr);

        /* write zone name: */
        nv_add(TRUE, " zone ");
	atalk_nv_printzone(at_hex_display(SY_STRING(ss->zone)));

        /* write resources: */
        nv_macip_resources(csb, ss);
    }
}

/*
 * mdb_clientbyIP -
 *	
 *	return pointer to client record of client with IP address <cip>.
 *	return NULL if not found.
 *	NOTE:  this routine must be fast, as it is called for all IP pkts,
 *	when MACIP is running.
 *	
 */
client_rec *
mdb_clientbyIP (ipaddrtype cip)
{
    register key *kk = &keystor;
    client_rec *cptr;

    /*
     * client_ip:   (ipaddr_hi, ipaddr_lo, client_rec *cptr)
     */
    db_mkkey(kk, 3, client_ip, HI_16(cip), LO_16(cip));
    if (DB_RETRIEVE((byteptr) &cptr, kk, sizeof(cptr)))
        return(cptr);

    /* not found: */
    return(NULL);
}

/*
 * mdb_serviceforIP -
 *	
 *	lookup resource record for server <ss>, IP address <iaddr>.
 *	return NULL if not found.
 *	
 */
resource_rec *
mdb_serviceforIP (server_rec *ss, ipaddrtype iaddr)
{
    register key *kk = &keystor;
    ushort rid;
    resource_rec *rsrc;

    /* search resources list: */
    for (rid = id_first(ss->rsrc_idh); rid != NILID;
        rid = id_next(ss->rsrc_idh, rid))
    {
        /*
         * server_resources:
         *  (ushort serverid, ushort enum, resource_rec *rsrc)
         */

        db_mkkey(kk, 3, server_resources, ss->serverid, rid);
        if (!DB_RETRIEVE((byteptr) &rsrc, kk, sizeof(rsrc)))
        {
            /* error - id w/no entry */
        at_err(MODID,"mdb_serviceforIP:  no resource, sid=%d, rid=%d",
                ss->serverid,rid);
            return(FALSE);
        }

        if (INRANGE(rsrc->lowip, iaddr, rsrc->highip))
            return(rsrc);       /* FOUND */
    }

    /* not found: */
    return(NULL);
}

/*
 * mdb_clientIPbyDDP -
 *	
 *	retrieve a client's IP address based on <dd>.
 *	return NULL if not found.
 *	
 */
ipaddrtype
mdb_clientIPbyDDP (server_rec *ss, ataddress *dd)
{
    key		*kk = &keystor;
    client_rec	*cptr;

    /*
     * client_ddp:  (ddpnet, ddpnode, client_rec *cptr)
     */
    db_mkkey(kk, 3, client_ddp, dd->net, dd->node);
    if (!DB_RETRIEVE((byteptr) &cptr, kk, sizeof(cptr)))
        return (0);			/* not found! */

    /* DDP address is assigned to another server, but not this one: */
    if (cptr->srec != ss)
	return (0);

    return (cptr->ipaddr);
}

/*
 * mdb_app_client -
 *	
 *	add a new client record for server <ss>.
 *	if duplicate ddp or ip, then no can do.
 *	don't add if server not up and routing.
 *	
 */
mdb_result
mdb_app_client (server_rec *ss, ipaddrtype cip, ataddress *dd)
{
    key		key1;
    key		key2;
    key		key3;
    key		*kk;
    client_rec	*cptr;
    ushort	cid;
    resource_rec *rr;

    if (!ss->serverUP)
	return (MDB_SERVER_DOWN);

    /* check client ip requested address is in range for server: */
    if (!mdb_rsrcbyIP(&rr, ss, cip))
	return (MDB_IP_OUT_OF_RANGE);

    /* if client is currently installed, then first delete the old: */
    kk = &key1;
    db_mkkey(kk, 3, client_ddp, dd->net, dd->node);
    if (DB_RETRIEVE((byteptr) &cptr, kk, sizeof(cptr)))
	mdb_purge_client (cptr->srec, cptr->ncli);

    /* get a new client id: */
    if (!id_getid(ss->ncli_idh, &cid))
	return (MDB_NO_IDS);

    /* malloc a client record: */
    cptr = malloc(sizeof(client_rec));
    if (cptr == (client_rec *) NULL) {
        /* back out: */
        id_delid(ss->ncli_idh, cid);
	return (MDB_MALLOC_FAIL);
    }

    /* fill it in: */
    cptr->srec         = ss;		/* ptr to server info */
    cptr->ncli         = cid;		/* save client enumerator */
    cptr->ipaddr       = cip;		/* client IP address */
    cptr->ddpaddr.net  = dd->net;	/* client DDP address */
    cptr->ddpaddr.node = dd->node;
    cptr->ddpaddr.spec = dd->spec;
    GET_TIMESTAMP(cptr->timer);		/* last confirm reply */

    /*
     * append client_ddp ref:
     * client_ddp:  (ddpnet, ddpnode, client_rec *cptr)
     */
    kk = &key1;
    db_mkkey(kk, 3, client_ddp, dd->net, dd->node);
    if (!DB_APPEND(kk, (byteptr) &cptr, sizeof(cptr))) {
	/* if still have a failure... */
	id_delid(ss->ncli_idh, cid);
	free(cptr);
	return (MDB_DB_ERROR);		/* shouldn't fail unless no memory */
    }

    /*
     * append client_ip ref:
     * client_ip:   (ipaddr_hi, ipaddr_lo, client_rec *cptr)
     */
    kk = &key2;
    db_mkkey(kk, 3, client_ip, HI_16(cip), LO_16(cip));
    if (!DB_APPEND(kk, (byteptr) &cptr, sizeof(cptr))) {
        /* back out: */
        DB_DELETE(&key1);
        id_delid(ss->ncli_idh, cid);
        free(cptr);
	return (MDB_DB_IP_DUP);
    }

    /*
     * append server_client:    (serverid, enum, client_rec *cptr)
     */
    kk = &key3;
    db_mkkey(kk, 3, server_client, ss->serverid, cid);
    if (!DB_APPEND(kk, (byteptr) &cptr, sizeof(cptr))) {
        /* back out: */
        DB_DELETE(&key1);
        DB_DELETE(&key2);
        id_delid(ss->ncli_idh, cid);
        free(cptr);
	return (MDB_DB_ERROR);		/* shouldn't fail unless no memory */
    }

    /* alias client IP address: */
    if (!reg_invoke_ipalias_insert(cip, MACIP_ALIASPORT, NULL)) {
        /* back out: */
        DB_DELETE(&key1);
        DB_DELETE(&key2);
        DB_DELETE(&key3);
        id_delid(ss->ncli_idh, cid);
        free(cptr);
	return (MDB_IP_ALIAS_ERROR);
    }

    /* make sure the IP address is marked as allocated: */
    bit_set(rr->allocd, cip - rr->lowip);
    if ((cip - rr->lowip) > rr->last)
    rr->last = (cip - rr->lowip);	/* keep track of max */

    return (MDB_OK);
}

/*
 * mdb_free_ip -
 *	
 *	mark an ip address as free.
 *	DOES NOT free corresponding client record.
 *	
 */
void
mdb_free_ip (server_rec *ss, ipaddrtype iaddr)
{
    register int ii;
    resource_rec *rsrc;

    /* lookup resource record: */
    rsrc = mdb_serviceforIP(ss, iaddr);
    if (rsrc == (resource_rec *) NULL)
        return;     /* bad address */
    
    /* calculate and clear position in bitmap: */
    ii = iaddr - rsrc->lowip;
    bit_clr(rsrc->allocd, ii);
}

/*
 * mdb_next_ip -
 *	
 *	mark the next dynamic IP address as taken.
 *	return 0 if none available.
 *	
 */
ipaddrtype
mdb_next_ip (server_rec *ss)
{
    register key *kk = &keystor;
    register iddesc idh = ss->rsrc_idh;
    register ushort ii;
    ushort nn;
    resource_rec *rsrc;

    /* foreach dynamic server resource.... */
    for (ii = id_first(idh); ii != NILID; ii = id_next(idh,ii)) {
        /*
         * server_resources:
         *  (short serverid, short enum, resource_rec *rsrc)
         */
        db_mkkey(kk, 3, server_resources, ss->serverid, ii);
        if (!DB_RETRIEVE((byteptr) &rsrc, kk, sizeof(rsrc)))
        {
            at_err(MODID,"mdb_next_ip:  no resource rec,sid=%d",
                    ss->serverid);
            continue;
        }

        if (rsrc->is_static)
            continue;

        /* Pick the next ip address, based on the max
	 * address currently allocated.  Skip addresses that fail
	 * IP alias check.
	 */
	for (;;) {
	    ipaddrtype candidate_addr;

	    candidate_addr = 0;
	    
	    nn = rsrc->last;
	    if (bit_next(rsrc->allocd, FALSE, &nn)) {
		/* found one, mark as allocated: */
		bit_set(rsrc->allocd, nn);
		rsrc->last = nn;
		candidate_addr = rsrc->lowip + nn; /* adjust to IP range */
	    } else {
		/* try again, this time starting at 0: */
		nn = 0;
		if (bit_next(rsrc->allocd, FALSE, &nn)) {
		    /* found one, mark as allocated: */
		    bit_set(rsrc->allocd, nn);
		    rsrc->last = nn;
		    candidate_addr = rsrc->lowip + nn; /* adjust to IP range */
		}
	    }
	    if (candidate_addr) {
		if (reg_invoke_ipalias_check(candidate_addr, FALSE, TRUE))
		    return (candidate_addr);
		else if (at_debug(atalkmacip_debug, 0))
		    buginf("\nAT-MacIP: IP alias check failed for %i. "
			   "Check IP ARP and/or alias table", candidate_addr);
	    } else
		break;
	}
    }

    /* no addresses available */
    return(0);
}

/*
 * mdb_rsrcbyIP -
 *	find the resource record of server <ss> serving IP address <cip>.
 *	return resource_rec pointer in <*urr>. false if none.
 *	
 */
boolean
mdb_rsrcbyIP (resource_rec **urr, server_rec *ss, ipaddrtype cip)
{
    key			*kk = &keystor;
    iddesc		idh = ss->rsrc_idh;
    int			ii;
    resource_rec	*rr;

    *urr = (resource_rec *) NILVAL;     /* result */

    /* foreach server resource.... */
    for (ii = id_first(idh); ii != NILID; ii = id_next(idh,ii)) {
        /*
         * server_resources:
         *  (short serverid, short enum, resource_rec *rsrc)
         */
        db_mkkey(kk, 3, server_resources, ss->serverid, ii);
        if (!DB_RETRIEVE((byteptr) &rr, kk, sizeof(rr))) {
            at_err(MODID,"resource_byIP:  no resource rec,sid=%d",
                ss->serverid);
            break;
        }

        /* if cip is in range... */
        if (INRANGE(rr->lowip, cip, rr->highip)) {
            *urr = rr;
            return(TRUE);
        } else {
	    if (at_debug(atalkmacip_debug, ATALK_NULL_MASK)) {
		buginf("\nAT: mdb_rsrcbyIP, resource compare: lo=%i cip=%i"
		       " hi=%i res=%d\n",
		       rr->lowip, cip, rr->highip,
		       INRANGE(rr->lowip, cip, rr->highip));
	    }
	}
    }

    /* not found */
    return (FALSE);
}

/*
 * mdb_serverbyCIP -
 *	lookup a server providing service for <cip>, return server in <*uss>.
 *	false if none.
 *	
 */
boolean
mdb_serverbyCIP (server_rec **uss, ipaddrtype cip)
{
    register key *kk = &keystor;
    register ushort sid;
    server_rec *ss;
    resource_rec *rr;

    *uss = NIL(server_rec);     /* results */

    /* foreach server.... */
    for (sid = id_first(srvr_idh); sid != NILID;
        sid = id_next(srvr_idh, sid))
    {
        /* servers: (short serverid, server_rec *ss) */

        db_mkkey(kk, 2, servers, sid);
        if (!DB_RETRIEVE((byteptr) &ss, kk, sizeof(ss))) {
            /* error - id w/no entry */
            at_err(MODID,"mdb_serverbyCIP:  no server record, sid=%d",
                sid);
            break;
        }

        if (mdb_rsrcbyIP(&rr, ss, cip)) {
            *uss = ss;
            return(TRUE);
        }
    }

    /* not found */
    return(FALSE);
}

/*
 * mdb_check_server -
 *	
 *	is <ss> really a server pointer?  do some sainity checks
 *	to find out.
 *	
 */
boolean
mdb_check_server (server_rec *uss, char *msg)
{
    register key *kk = &keystor;
    server_rec *ss;

    if (ODDPTR(uss)) {
        at_err(MODID,"%s:  ODD server handle, %lx", msg, uss);
        return(FALSE);
    }

    /* servers: (short serverid, server_rec *ss) */
    db_mkkey(kk, 2, servers, uss->serverid);
    if (!DB_RETRIEVE((byteptr) &ss, kk, sizeof(ss))) {
        /* error - no such serverid */
        at_err(MODID,"%s:  UNKNOWN server handle, %lx", msg, uss);
        return(FALSE);
    }

    if (ss != uss) {
        at_err(MODID,"%s:  conflicting handle, sid %d, caller=%lx, db=%lx",
                msg, uss->serverid, uss, ss);
        return(FALSE);
    }

    return(TRUE);
}

/*
 * mdb_no_resources -
 *	true if no resources left for server <ss>
 */
boolean
mdb_no_resources (server_rec *ss)
{
    return(id_first(ss->rsrc_idh) == NILID);
}

/*
 * mdb_cli_add_test -
 *	append fake (or existing) clients for testing
 */
void
mdb_cli_add_test (ipaddrtype cip1, ipaddrtype cip2, ataddress *ddp)
{
    server_rec *ss = NULL;
    mdb_result err;
    ipaddrtype cip;

    /* if cip1 is a server address, then pick the
     * next available address assignment:
     */
    if (mdb_serverbyIP(&ss, cip1)) {
	cip = mdb_next_ip(ss);
	if (!cip) {
	    printf("macip server %i - out of addresses\n", cip1);
	    return;
	}

	if ((err = mdb_app_client(ss, cip, ddp)) != MDB_OK) {
	    printf("can't add %i - append failed, err=%s\n",
		   cip, mdb_resultStr(err));
	    return;
	}

	/* success - increment fake source address: */
	++ddp->node;
	printf("added %i as fake client\n", cip);
	return;
    }

    for (cip = cip1; cip <= cip2; cip++) {
	if (!mdb_serverbyCIP (&ss, cip)) {
	    printf("can't add %i - not served\n", cip);
	    break;
	}

	if ((err = mdb_app_client(ss, cip, ddp)) != MDB_OK) {
	    printf("can't add %i - append failed, err=%s\n",
		cip, mdb_resultStr(err));
	    break;
	}

	/* increment fake source address: */
	++ddp->node;
	printf("added %i as fake client\n", cip);
    }
}

/*
 * mdb_cli_del_test -
 *	delete fake clients
 */
void
mdb_cli_del_test (ipaddrtype cip1, ipaddrtype cip2)
{
    ipaddrtype cip;
    client_rec *cptr;

    for (cip = cip1; cip <= cip2; cip++) {
	if ((cptr = mdb_clientbyIP(cip)) == NULL) {
	    printf("can't delete %i - not a client\n", cip);
	    continue;
	}

	mdb_purge_client(cptr->srec, cptr->ncli);

	printf("purged client %i\n", cip);
    }
}

/*
 * mdb_up_servers -
 *	return a count of servers marked as UP
 */
int
mdb_up_servers (void)
{
    key		*kk = &keystor;
    server_rec	*ss;
    int		sid;
    int		upcount = 0;

    if (srvr_idh == NILIDDESC)
        return (upcount);		/* database not inited */

    /* foreach server.... */
    for (sid = id_first(srvr_idh); sid != NILID;
        sid = id_next(srvr_idh, sid)) {
        /*
         * servers: (short serverid, server_rec *ss)
         */

        db_mkkey(kk, 2, servers, sid);
        if (!DB_RETRIEVE((byteptr) &ss, kk, sizeof(ss))) {
            /* error - id w/no entry */
            at_err(MODID,"mdb_up_servers:  no server record, sid=%d",
                sid);
            continue;			/* dump next rec */
        }

	if (ss->serverUP)
	    ++upcount;
    }

    return (upcount);
}

/*
 * mdb_resultStr
 *	return string representation of mdb_result
 */
char *
    mdb_resultStr (mdb_result mm)
{
    switch (mm) {
      case MDB_SERVER_DOWN:	return ("MDB_SERVER_DOWN");
      case MDB_IP_OUT_OF_RANGE:	return ("MDB_IP_OUT_OF_RANGE");
      case MDB_DB_DDP_DUP:	return ("MDB_DB_DDP_DUP");
      case MDB_DB_IP_DUP:	return ("MDB_DB_IP_DUP");
      case MDB_IP_ALIAS_ERROR:	return ("MDB_IP_ALIAS_ERROR");
      case MDB_NO_IDS:		return ("MDB_NO_IDS");
      case MDB_MALLOC_FAIL:	return ("MDB_MALLOC_FAIL");
      case MDB_DB_ERROR:	return ("MDB_DB_ERROR");
      case MDB_OK:		return ("MDB_OK");
      default:			return ("BAD ARG");
    }
}
