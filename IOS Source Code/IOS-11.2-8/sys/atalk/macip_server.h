/* $Id: macip_server.h,v 3.2 1995/11/17 08:45:12 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atalk/macip_server.h,v $
 *------------------------------------------------------------------
 * macip_server.h	- information for users of macip_server module.
 *
 * September 1991, Russ Tremain, Tremain Laboratories, under contract
 * 	for cisco Systems, Inc.
 *
 * Copyright (c) 1991-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: macip_server.h,v $
 * Revision 3.2  1995/11/17  08:45:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:59:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:51:16  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/10/18  21:47:22  spreissm
 * CSCdi41076:  Software forced crash, PC 0x10BD58 - memory corruption
 *         - macip_atp_service was reusing a buffer w/o first checking
 *           size.
 *
 * Revision 2.1  1995/06/07  20:10:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__macip_server_h__
#define	__macip_server_h__

#define	MACIP_VERSION	1
extern gccontext_t *macip_gccontext;

/* server timeout values: */
#define NBP_CONFIRM_TIME	ONEMIN
#define CLIENT_TIMEOUT		(NBP_CONFIRM_TIME*10)

/* state machine timeouts: */
#define RESOURCE_WAIT_TIME	ONEMIN
#define CONFIG_WAIT_TIME	(3*ONEMIN)
#define ZONE_WAIT_TIME		(15*ONESEC)
#define	SERVER_REG_WAIT		(2*ONESEC)

/* client NBP search parms: */
#define MAX_CLIENT_REQUESTS	5
#define MAX_CLIENT_REPLIES	500	/* s.b. dynamic */

/* IPGP message size */
#define	IPGPMSGSIZE	128

/*
 * ATP packet used by client MacIP programs to request
 * name assignment and lookup services.
 */
typedef struct IPGP_ {
	ulong		op;		/* opcode */
	ipaddrtype	ipaddress;	/* my IP address (or lookup reply)*/
	ipaddrtype	ipname;		/* address of my name server */
	ipaddrtype	ipbroad;	/* my broadcast address */
	ulong		unused1;	/* UNUSED */
	ipaddrtype	ipsubnet;	/* subnet mask */
	ulong		unused2;	/* UNUSED */
	ulong		unused3;	/* UNUSED */
	ulong		unused4;	/* UNUSED */
	char		msg[IPGPMSGSIZE];	/* domain name extension or
					 * error message (+ NULL) */
} IPGP;
#define	ipgpMinSize	36

/* IPGP op codes */
#define	ipgpAssign	1	/* assign new IP (dynamic) address */
#define	ipgpName	2	/* OBSOLETE */
#define	ipgpServer	3	/* return server information */
#define	ipgpRelease	4	/* release this IP address */
#define	ipgpStatic	5	/* verify this static IP address is mine */
#define	ipgpError	-1	/* error return op code; string=message */

/* IPGP messages */
#define	IPGP_MSG_SUCCESS	""
#define	IPGP_MSG_BAD_OP		"bad op"
#define	IPGP_MSG_NO_RESOURCES	"no free address"
#define	IPGP_MSG_INUSE		"address in use"
#define IPGP_MSG_NO_DOMAIN_NAME "configuration error - no domain name set"

/* how many server-event records to malloc before garbage collection: */
#define	MAX_MSE_FREELIST	10

extern void macip_start(void);
extern boolean macip_empty_trash(void);
extern void macip_shutdown(void);
extern void macip_clearQs(server_rec *ss);
extern void macip_clear_stateQ(server_rec *ss);
extern boolean macip_zone_seeded(server_rec *ss);
extern boolean macip_start_server(server_rec *ss);
extern boolean macip_client_search(server_rec *ss);
extern void macip_client_service(server_rec *ss);
extern void macip_confirm_clients(server_rec *ss);
extern void macip_stop_server(server_rec *ss);
extern boolean macip_ddp_down(server_rec *ss);
extern boolean macip_ip_down(server_rec *ss);
extern void macip_show_traffic(void);
extern void macip_show_clients(void);
extern void macip_show_servers(void);
extern void macip_service(server_rec *ss);
extern macipServerEvent *macip_get_server_event(void);
extern void macip_free_server_event(macipServerEvent *mse);
extern boolean macip_enqueue(paktype *pak);

#endif
