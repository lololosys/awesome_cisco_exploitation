/* $Id: standby.h,v 3.4.60.2 1996/06/18 18:26:06 dawnli Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/standby.h,v $
 *------------------------------------------------------------------
 * standby.h - Definitions for hot standby protocol
 *
 * Sep 1993 Tony Li
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: standby.h,v $
 * Revision 3.4.60.2  1996/06/18  18:26:06  dawnli
 * CSCdi60445:  reloading 4500 breaks HSRP - timers change to zero
 * Branch: California_branch
 *
 * Interface state changes in the time window that standby process has been
 * created but not registered. Standby will stay in init state.
 *
 * Register HSRP before standby process is created.
 *
 * Revision 3.4.60.1  1996/03/18  20:29:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.26.2  1996/03/14  01:07:41  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.4.26.1  1996/02/20  14:30:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/11/19  08:11:01  bcole
 * CSCdi29702:  HSRP: Secondary address support should be improved
 *
 * Abstract implementation of virtual IP address, so that multiple may
 * now be configured per HSRP group.  Add nob.
 *
 * Revision 3.3  1995/11/17  17:37:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:48:59  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:10:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/11  23:51:04  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.1  1995/06/07 21:11:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Hot standby information structure
 */

#define STANDBY_AUTH_LEN 8

STRUCTURE_REF(standbyinfotype);
STRUCTURE_REF(standbyaddrtype);

struct standbyaddrtype_ {
    standbyaddrtype *next_in_group; /* next addr in this group */
    standbyaddrtype *next_in_hash;  /* next addr in this hash bucket */
    ipaddrtype addr;			/* The IP address itself */
    standbyinfotype *sbgroup;		/* the group the IP address is in */
} ;

struct standbyinfotype_ {
    standbyinfotype *next;		/* Chain through standby info */
    standbyaddrtype addrs;		/* Leader Proto IP address(es) */
    idbtype *idb;			/* interface */
    ipaddrtype active;			/* active router's address */
    sys_timestamp activetimer;		/* time we last heard from active */
    ipaddrtype standby;			/* standby's address */
    sys_timestamp standbytimer;		/* time we last heard standby */
    sys_timestamp nexthello;		/* next time to send a hello */
    uchar *trackifs;			/* table of interfaces being tracked */
    uchar *upifs;			/* State of interfaces being tracked */
    uchar *if_pri;			/* table of if prios. for tracking */
    ushort trackif_cnt;			/* Count of interfaces being tracked */
    ushort upif_cnt;			/* # of tracked ifs which are up */
    ushort flags;			/* various bits */
    uchar state;			/* current state */
    uchar group;			/* group number */
    ulong hellotime;			/* time between hello packets (ms) */
    ulong holdtime;			/* hold time for hellos (ms) */
    uchar learned_hellotime;		/* time between hello packets (sec) */
    uchar learned_holdtime;		/* hold time for hellos (sec) */
    uchar priority;			/* current priority */
    uchar cfg_priority;			/* configured priority */
    uchar re_arp;                       /* # of gratuitous arp gonna sent */
    uchar auth[STANDBY_AUTH_LEN+1];	/* authentication string */
} ;

#define STANDBY_ARP_RESENT      3

#define standby_primary_addr(li) (li->addrs.addr)
#define standby_have_addr(li) (standby_primary_addr(li) != SDEFAULT_IP_ADDR)
  
#define standby_ms2sec(time) ((time) / ONESEC)
#define standby_sec2ms(time) ((time) * ONESEC)

/*
 * Per group flags
 */
#define SFLAG_CFG_IP		0x1	/* set if IP configured */
#define SFLAG_CFG_IP_ADDR	0x2	/* set if an IP address configured */
#define SFLAG_CFG_AUTH		0x4	/* set if authentication configured */
#define SFLAG_CFG_PREEMPT	0x8	/* set if preempt configured */
#define SFLAG_CFG_PRIORITY	0x10	/* set if priority configured */
#define SFLAG_CFG_TIMERS	0x20	/* set if timers configured */
#define SFLAG_HWADDR_COERCE	0x40	/* set if if hwaddr was coerced */
#define SFLAG_CFG_GROUP		0x80	/* set if group configured */
#define SFLAG_CFG_TRACK		0x100	/* set if track command configured */

/*
 * Global - per interface - flags
 */
#define SFLAG_CFG_USEBIA        0x1     /* set if should use BIA */
#define SDEFAULT_IP_ADDR	0x0	/* default IP address 0.0.0.0 */
#define SDEFAULT_AUTHSTRING	"cisco"	/* default authentcation string */
#define SDEFAULT_PRIORITY	100	/* default priority */
#define SDEFAULT_HELLOTIME	3	/* default hellotime in sec */
#define SDEFAULT_HOLDTIME	10	/* default holdtime in sec */
#define SDEFAULT_GROUP		0

#define SSTATE_VIRGIN		0x0
#define SSTATE_LEARN		0x1
#define SSTATE_LISTEN		0x2
#define SSTATE_SPEAK		0x4
#define SSTATE_STANDBY		0x8
#define SSTATE_ACTIVE		0x10

/*
 * A speaker is one who should send hellos.
 */
#define SSTATE_SPEAKER (SSTATE_SPEAK | SSTATE_STANDBY | SSTATE_ACTIVE)

typedef struct standbytype_ {
    uchar version;			/* version number */
    uchar opcode;			/* opcode (LOPCODE_xxx) */
    uchar state;			/* sender's state */
    uchar hellotime;			/* hello interval */
    uchar holdtime;			/* holdtime */
    uchar priority;			/* priority */
    uchar group;			/* group number */
    uchar pad;				/* reserved */
    uchar auth[STANDBY_AUTH_LEN];	/* Authentication */
    ipaddrtype address;			/* Leader pseudo-address */
} standbytype;

#define LOPCODE_HELLO		0
#define LOPCODE_COUP		1
#define LOPCODE_RESIGN		2

#define STANDBY_VERSION		0
#define STANDBY_MAXHELLOTIME	255
#define STANDBY_MINCFG		SFLAG_CFG_IP
#define STANDBY_MAXPACKETSIZE	(sizeof(standbytype))
#define STANDBY_802ADDR	{ 0x00, 0x00, 0x0c, 0x07, 0xac, 0x00}
#define STANDBY_PRI_DEC		10
#define STANDBY_GROUPS		256

struct standbyidbtype_ {
    standbyinfotype     *chain;         /* Chain through standby info */
    ulong               flags;          /* Per interface flags */
};

#define RESTART_ACTIVETIMER	\
    (TIMER_START(li->activetimer, \
                 li->learned_holdtime ? \
                 li->learned_holdtime*ONESEC : \
                 li->holdtime))
#define RESTART_STANDBYTIMER	\
    (TIMER_START(li->standbytimer, \
                 li->learned_holdtime ? \
                 li->learned_holdtime*ONESEC : \
                 li->holdtime))
#define FOR_ALL_SBGROUPS(idb, si) \
    if (idb->standbyidb) \
        for (si = idb->standbyidb->chain; si; si = si->next)
#define FOR_ALL_SBADDRS(si, siaddr) \
    for (siaddr = &si->addrs; siaddr; siaddr = siaddr->next_in_group)

/*
 * standby_mincfg
 * Is the HSRP group minimally configured?
 */
static inline boolean standby_mincfg (standbyinfotype *si)
{
    if (!si)
        return(FALSE);
    return(si->flags & STANDBY_MINCFG);
}

/*
 * standby_if_mincfg
 * Is HSRP minimally configured on this interface?
 */
static inline boolean standby_if_mincfg (idbtype *swidb)
{
    standbyinfotype     *si;

    FOR_ALL_SBGROUPS(swidb, si)
        if (standby_mincfg(si))
            return(TRUE);
    return(FALSE);
}

/*
 * standby.c
 */ 
extern void standby_command(parseinfo *csb);
extern void standby_process_init(void);
