/* $Id: atalk_nbp.h,v 3.2.60.1 1996/03/23 01:28:03 slin Exp $
 * $Source: /release/111/cvs/Xsys/atalk/atalk_nbp.h,v $
 *------------------------------------------------------------------
 * atalk_nbp.h -- public NBP interfaces
 *
 * August 1990, Russ Tremain, on contract to cisco Systems.
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atalk_nbp.h,v $
 * Revision 3.2.60.1  1996/03/23  01:28:03  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2  1995/11/17  08:44:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:50:46  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:09:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef	__ATALK_NBP_H__
#define __ATALK_NBP_H__


/* default timer parms for confirms/lookups if none specified: */
#define DEF_MAXREQUESTS 5
#define DEF_MAXREPLIES  1
#define DEF_INTERVAL    5       /* seconds */

/* timer parms for nbp_register request: */
#define REG_REQUESTS    5
#define REG_REPLIES 1
#define REG_INTERVAL    1

/*
 * NBP entities are guaranteed to be registered
 * or deleted after this time:
 * (used to confirm registration after an async call to nbp_register).
 */
#define NBP_REG_TIMEOUT ((REG_REQUESTS+2)*REG_INTERVAL)

/* macro to compare two ddp addresses: */
#define DDPEQUAL(AA, BB)    ((AA)->net == (BB)->net \
    && (AA)->node == (BB)->node && (AA)->spec == (BB)->spec)

/* a single NBP Entity: */
typedef struct nbpEntity_ {
    str32 srvr_name;
    str32 type_srvc;
    str32 srvr_zone;
} nbpEntity;

typedef struct nbptimer_ {
    short maxrequests;  /* max # of requests to send */
    short maxreplies;       /* max # of replies to accept */
    short interval;     /* intervals (seconds) between retries */
} nbptimer;

typedef enum {
    /* events from nbp_find(): */
    NBP_REPLY,
    NBP_REPLY_LAST,
    NBP_TIMEOUT,
} nbp_event_code;

/* event queue elements containing responses to nbp_find() requests */
typedef struct nbpQelem_ {
    struct nbpQelem_    *link;		/* required by queue mang. */
    nbp_event_code      msg;		/* on of <nbp_event_code> */
    /* the following valid only for NBP_REPLY events: */
    uchar	enumerator;		/* returned enumerator */
    ataddress   srvr_addr;		/* the returned server address */
    ataddress   orig_addr;		/* the returned originator address */
    nbpEntity   nve;			/* the returned NBP */
} nbpQelem;

#define NILNQELEM   ((nbpQelem *) NILVAL)

/* error returns from nbp exported functions: */
typedef enum {
    /* from nbp_register(): */
    NBP_DUPLICATE,			/* name already registered */
    NBP_TABLE_ERROR,			/* failed to add to table */
    NBP_NET_NOT_ATTACHED,		/* bad src net */
    NBP_ZONE_NOT_ATTACHED,		/* bad registration zone */
    NBP_ZONE_REQUIRED,			/* no zone or local zone spec */
    NBP_IDB_NOT_IN_ZONE,		/* no active idb in spec'd zone */
    /* from, nbp_find(), nbp_confirm(): */
    NBP_BAD_DDP,			/* bad DDP address arg. */
    NBP_BAD_SKT,			/* bad skt value or no listener */
    NBP_BAD_ZONE,			/* bad zone arg. */
    NBP_ATALK_DOWN,			/* no appletalk interfaces active */
    NBP_BAD_DDP_DST,			/* NULL or illegal ddp dest */
    NBP_BAD_TIMER,			/* bad timer parms */
    /* from nbp_unregister(): */
    NBP_NO_MATCH,
    /* from all: */
    NBP_MODULE_ERROR,			/* internal module problem */
    NBP_NO_MEMORY,			/* couldn't get memory */
    /* from nbp_isregistered: */
    NBP_REG_PENDING,
    NBP_NOT_REGISTERED,
    NBP_OK  /* successful return */
} nbp_result;

#define LOCALZONE   ((str32 *) "\001*")
#define NBP_ANY     ((str32 *) "\001=")
#define ISLOCALZONE(ZZ) (EMPTYSTR32(ZZ) || STR32EQ(LOCALZONE,ZZ))
#define ISNBP_ANY(NN)   (STR32EQ(NBP_ANY,NN))


extern boolean	  atnbp_parse_tuple(char *, nbpEntity *);

extern nbp_result atnbp_confirm(nbpEntity *, ataddress *, ataddress *,
				nbptimer *, watched_queue *);

extern nbp_result atnbp_register(nbpEntity *, ataddress *, boolean);

extern nbp_result atnbp_unregister(nbpEntity *);

extern nbp_result atnbp_DDPunregister(ataddress *);

extern nbp_result atnbp_isregistered(nbpEntity *);

extern nbp_result atnbp_find(nbpEntity*, ataddress *, nbptimer *,
			     watched_queue *);
extern void atnbp_event_free(nbpQelem *);

extern void atnbp_cancel(watched_queue *);

extern void atnbp_mkname(nbpEntity *, char *, char *, char *);

extern char *atnbp_resultStr(nbp_result);
extern char *atnbp_entityStr(nbpEntity *);
extern char *atnbp_timerStr(nbptimer *);
extern char *atnbp_msgStr(nbp_event_code);
extern char *ddpAddrStr(ataddress *);
extern atalkidbtype *pickIDBinZone(str32 *, atalkidbtype *);


typedef enum {
    NBP_LOOKUP,
    NBP_CONFIRM
} nbp_reqtype;

/* control structure for active  nbp requests: */
typedef struct nbp_request_ {
    struct nbp_request_ *link;		/* required by queue mang. */
    nbp_reqtype reqtype;		/* NBP_LOOKUP or NBP_CONFIRM */
    uchar	NBPid;
    short       reqClock;		/* times request transmit interval */
    nbptimer    tm;			/* timer values (mod'd by req procs) */
    nbpEntity   nve;			/* lookup request */
    watched_queue *uqw;			/* user queue waiting */
    paktype     *pak;			/* packet to transmit */
} nbp_request;

/* freelist limits for NBP structures: */
#define	MAX_NQE_FREELIST	200
#define	MAX_NRE_FREELIST	100

extern void atnbp_Initialize(void);

extern void atnbp_background(void);

/* maximum number of names we can register: */
#define MAX_NVES    256

/* maximum number of aliases for a NVE (sainity check): */
#define MAXALIASES  MAX_NVES

typedef struct nveSearch_ {
    nbpEntity   nve;			/* result */
    ushort	net;
    uchar	socket;
    uchar	node;
    short	enumerator;
    /* look, but don't touch (result parms): */
    symbol	luName, luType, luZone; /* search terms */
    ushort	idx;			/* last entry found */
} nveSearch;

#define NILNVE  ((nveSearch *) NULL)

typedef enum {
    NVE_NOT_FOUND,
    NVE_PERMANENT,
    NVE_TENTATIVE,
    NVE_MODULE_ERROR,
    NVE_OK
} nve_result;

/* true if search spec NVE matches NVE stored in table.
 * (name, type, zone == NILSYM) ==> wildcard.
 * DO NOT return entries marked as tentative (registration pending).
 *
 * ONLY match if packet destination net agrees with the DDP net
 * the entity is registered on OR is ZERO.  O'wise, we will generate
 * multiple NBP response tuples for each lookup pkt, one per interface
 * IN ZONE, since a lookup will be directed to each interface.
 *
 * I.e., if there are N interfaces in zone Z, then a lookup
 * of =:ciscoRouter@Z would generate N response tuples per packet.
 * Matching on dnet will ensure there is only 1 response
 * tuple generated per packet.
 *
 * Usage note:  up to programmer to set the net in the search structure,
 * otherwise, nve_firstmatch() will ignore net number in compare.
 * However, subsequent nve_nextmatch() calls will always check net number
 * unless programmer clears net # in search structure prior to each
 * call (i.e., view net number as a value/result paramater, 0==>ignore).
 * RT 7/28/92
 */
#define NVEMATCH(SS, TE)    (\
        !(TE)->tentative &&\
        ((SS)->luName == NILSYM || (SS)->luName == (TE)->nameSym) &&\
        ((SS)->luType == NILSYM || (SS)->luType == (TE)->typeSym) &&\
        ((SS)->luZone == NILSYM || (SS)->luZone == (TE)->zoneSym) &&\
        ((SS)->net == 0 || (SS)->net == (TE)->net))

/* true if search term has wildcard elements */
#define WILDCARDSEARCH(SS)\
    ((SS)->luType==NILSYM || (SS)->luName==NILSYM || (SS)->luZone==NILSYM)

/*
 * exported functions:
 */
extern boolean		atnbp_nve_delete(nbpEntity *);
extern boolean		atnbp_nve_DDPdelete(ataddress *);
extern nveSearch	*atnbp_nve_firstmatch(nveSearch *, nbpEntity *);
extern boolean		atnbp_nve_init(void);
extern void		atnbp_nve_dump(void);
extern char		*atnbp_nve_resultStr(nve_result);

#endif
