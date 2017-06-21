/* $Id: ether_private.h,v 3.1.42.2 1996/08/13 02:18:39 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/if/ether_private.h,v $
 *------------------------------------------------------------------
 * ether_private.h  -- Ethernet/IEEE802.3 private definitions
 *
 * December 1995, Greg Stovall
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ether_private.h,v $
 * Revision 3.1.42.2  1996/08/13  02:18:39  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.1.42.1  1996/03/18  20:13:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/01/24  22:18:06  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.1  1996/01/23  21:45:00  gstovall
 * Placeholders for the king.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ETHER_PRIVATE_H__
#define __ETHER_PRIVATE_H__

#define ETHER_HOLDQ 40			/* maximum ethernet hold queue */
#define ETHER_MAXDGRAM_DEF 1524         /* bytes, for buffer size checking */
#define ETHER_MAXPAK (ETHER_ARPA_ENCAPBYTES + MAXETHERSIZE)
#define ETHER_BUFFERSIZE_BYTES 1520

/*
 * 10 Mb ether bandwidth defines
 */
#define ETHER_DELAY 100			/* for IGRP on Ethernets */
#define VISIBLE_ETHER_BANDWIDTH 10000	/* for IGRP on Ethernets */

/*
 * 100 Mb ether (fast ethernet) bandwidth defines
 */
#define FETHER_DELAY 10			      /* Delay (100 usec) */
#define FETHER_VISIBLE_BANDWIDTH 100000	      /* Bandwidth (100Mbps) */

/*
 * Ethernet subblock definition
 */
typedef struct ethersb_ {
    ulong output_one_collision;  /* no. of packets with just one collision */
    ulong output_more_collision; /* no. of multi collision errors, tx ok */
    ulong output_late_collision; /* no. of late collision errors */
    ulong output_excessive_collision; /* Number of excessive collisions */
    ulong output_deferred;       /* tx delayed but successful */
    ulong output_lost_carrier;   /* no. lost carrier events */
    ulong output_babbles;	 /* no. of babble events */

    /* 
     * These are the duplicates of the above for interactive work. They
     * contain the values of the variables as of the last time "clear
     * counter" was done for this interface. (The "si_" prefix stands for
     * "show interface"
     */
    ulong si_output_one_collision;  /* no. of single collisions, but tx ok */
    ulong si_output_more_collision; /* no. of single collisions, but tx ok */
    ulong si_output_late_collision; /* no. of late collision errors */
    ulong si_output_excessive_collision; /* # excessive collisions */
    ulong si_output_deferred;       /* no. tx deferred but ok */
    ulong si_output_lost_carrier;   /* no. lost carrier events */
    ulong si_output_babbles;        /* no. of babble events */

    /*
     * Fast Ethernet fields
     */
    ulong feip_no_carrier;      /* carrier not present during transmit */
    ulong feip_multicast;       /* frame has multicast address */
    ulong feip_watchdog;        /* receive watchdog expired on frame
				   greater than 2048 bytes */
    ulong feip_fullduplex;      /* Fast ethernet full duplex switch */

    /*
     * These are the duplicates of the above feip statistics for
     * interactive work. They contain the values of the variables
     * as of the last time "clear counter" was done for this interface
     * (the "si_" prefix stands for "show interface").
     */
    ulong si_feip_no_carrier;  /* carrier not present during transmit */
    ulong si_feip_multicast;   /* frame has multicast address */
    ulong si_feip_watchdog;    /* receive watchdog expired on frame
				  greater than 2048 bytes */

    /*
     * Ethernet media type.
     * For FEIP, this will be ether ETHER_MEDIA_TYPE_FEIP_RJ45 or
     * ETHER_MEDIA_TYPE_FEIP_MII. For the 4xxx series of routers,
     * this is either ETHER_MEDIA_TYPE_AUI or ETHER_MEDIA_TYPE_10BASET.
     */
    enum MEDIA_TYPE media_type;

    /*
     * Vector to hold pre-existing state/config vector which was
     * in place before we initialized an ethernet IDB as an ethernet IDB.
     * If the ethernet state/config vector is not able to handle one
     * or more items, that itemlist should be passed to the "super"
     * function.
     */
    hwidb_state_config_t	super_state_config;
} ethersb;

/**********************************************************************
 *
 * Prototypes
 *
 **********************************************************************/
boolean send_loop(hwidbtype *, uchar *);

/*
 * Externs
 */
extern void ether_parser_init(void);

#endif  __ETHER_PRIVATE_H__
