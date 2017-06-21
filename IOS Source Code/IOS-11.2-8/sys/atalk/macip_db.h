/* $Id: macip_db.h,v 3.3.58.1 1996/03/23 01:28:27 slin Exp $
 * $Source: /release/111/cvs/Xsys/atalk/macip_db.h,v $
 *------------------------------------------------------------------
 * macip_db.h	- information for users of macip data structures.
 * prereqs:
 *	macip_util.h
 *	nbpapi.h
 *
 * September 1991, Russ Tremain, Tremain Laboratories, under contract
 * 	for cisco Systems, Inc.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: macip_db.h,v $
 * Revision 3.3.58.1  1996/03/23  01:28:27  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.3  1995/11/17  08:44:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:06:26  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:58:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:51:05  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/13  06:50:23  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.1  1995/06/07 20:10:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__macip_db_h__
#define	__macip_db_h__

/* true if initialization okay: */
extern boolean macip_initialized;

/* true if macip configuration is active: */
extern boolean macip_configured;

extern symbol_table	macip_symtab;
extern database		macip_db;
extern iddesc	srvr_idh;

/* WARNING:  these must be defined for SY_ and DB_ macros to function: */
#define	MY_SYMTAB	macip_symtab
#define	MY_DATABASE	macip_db

/* relation defs: */

/* server statement:
 *	servers:	(ushort serverid, server_rec *sptr)
 */

typedef struct  server_rec_ {
	ushort	serverid;		/* server ID */
	ipaddrtype ipaddr;		/* server IP address */
	ataddress reqaddr;		/* server DDP address */
	symbol	zone;			/* server appletalk zone */
	idbtype *ipidb;			/* IP interface descriptor */
	atalkidbtype *ddpidb;		/* DDP interface descriptor */
	iddesc rsrc_idh;	/* resource record enumerator allocator */
	iddesc ncli_idh;	/* client record enumerator allocator */
	watched_queue  *reqQ;		/* client request packets */
	watched_queue  *confirmQ;	/* NBP confirm events */
	queuetype	stateQ;		/* state event queue */
	PFV		stateFunc;	/* current state routine */
	PFV		prevStateFunc;	/* "any state" return to function */
	boolean		timeout;	/* TRUE if TIMEOUT event */
	sys_timestamp	timer;		/* timer */
	ulong		delay;		/* delay before TIMEOUT
					 * zero ==> no timeout events.
					 */
	boolean		serverUP;	/* TRUE if server is routing */
} server_rec;

extern symbol servers;

/* from static, dynamic statements:
 *	server_resources:
 *		(short serverid, short enum, resource_rec *rsrc)
 */

typedef struct  resource_rec_ {
	ushort resourceid;		/* my resource number */
	boolean	is_static;
	ipaddrtype lowip;
	ipaddrtype highip;
	bitarray allocd;
	int last;		/* last IP we handed out */
} resource_rec;

/* clients:
 *	server_client:	(serverid, enum, client_rec *cptr)
 *	client_ip:	(ipaddr_hi, ipaddr_lo, client_rec *cptr)
 *
 */
extern symbol server_client, client_ip;

typedef struct  client_rec_ {
	server_rec	*srec;		/* ptr to server info */
	ushort		ncli;		/* client enumerator */
	ipaddrtype	ipaddr;		/* client IP address */
	ataddress	ddpaddr;	/* client DDP address */
	sys_timestamp	timer;		/* time of last confirm reply */
} client_rec;

/* error returns from macip_db exported functions: */
typedef enum {
    /* from mdb_app_client(): */
    MDB_SERVER_DOWN,		/* server not yet routing */
    MDB_IP_OUT_OF_RANGE,	/* IP not in service range */
    MDB_DB_DDP_DUP,		/* attempt to add duplicate DDP address */
    MDB_DB_IP_DUP,		/* attempt to add duplicate IP address */
    MDB_IP_ALIAS_ERROR,		/* couldn't insert an ip alias */
    /* generic errors: */
    MDB_NO_IDS,			/* run out of id's */
    MDB_MALLOC_FAIL,		/* no memory */
    MDB_DB_ERROR,		/* database operation failed */
    MDB_OK			/* operation was successful */
} mdb_result;

/* exported funtions: */
extern boolean mdb_init(void);
extern boolean mdb_app_resource(boolean negate, ushort sid, ipaddrtype ll, ipaddrtype uu, boolean isstatic);
extern boolean mdb_app_server(boolean negate, ushort *sid, ipaddrtype addr, symbol zname);
extern boolean mdb_serverbyzone(server_rec **uss, symbol zname);
extern boolean mdb_serverbyIP(server_rec **uss, ipaddrtype sip);
extern char *ipaddrStr(ipaddrtype iaddr);
extern void mdb_purge_client(server_rec *ss, ushort cid);
extern void mdb_purge_server(server_rec *ss);
extern void mdb_nv_macip(parseinfo *);
extern client_rec * mdb_clientbyIP(ipaddrtype cip);
extern resource_rec * mdb_serviceforIP(server_rec *ss, ipaddrtype iaddr);
extern ipaddrtype mdb_clientIPbyDDP(server_rec *ss, ataddress *dd);
extern mdb_result mdb_app_client(server_rec *ss, ipaddrtype cip, ataddress *dd);
extern void mdb_free_ip(server_rec *ss, ipaddrtype iaddr);
extern ipaddrtype mdb_next_ip(server_rec *ss);
extern void mdb_show_servers(void);
extern void mdb_show_clients(void);
extern boolean mdb_rsrcbyIP(resource_rec **urr, server_rec *ss, ipaddrtype ip);
extern boolean mdb_serverbyCIP(server_rec **uss, ipaddrtype cip);
extern boolean mdb_check_server(server_rec *uss, char *msg);
extern boolean mdb_no_resources(server_rec *ss);
extern int mdb_up_servers(void);
extern char *mdb_resultStr (mdb_result mm);
extern void mdb_cli_add_test(ipaddrtype cip1, ipaddrtype cip2, ataddress *ddp);
extern void mdb_cli_del_test(ipaddrtype cip1, ipaddrtype cip2);

#endif
