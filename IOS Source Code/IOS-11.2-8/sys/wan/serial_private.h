/* $Id: serial_private.h,v 3.1.42.4 1996/07/28 07:33:15 irfan Exp $
 * $Source: /release/112/cvs/Xsys/wan/serial_private.h,v $
 *------------------------------------------------------------------
 * serial_private.h -- synchronous serial line private definitions
 *
 * December 1995, Greg Stovall
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: serial_private.h,v $
 * Revision 3.1.42.4  1996/07/28  07:33:15  irfan
 * CSCdi60568:  v120 autodetect not working
 * Branch: California_branch
 * allow serial interfaces to autodetect encaps on the wire for incoming
 * connections.
 *
 * Revision 3.1.42.3  1996/07/08  22:58:58  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.1.42.2  1996/06/17  08:36:59  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.1.42.1  1996/03/18  22:48:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.2  1996/02/20  21:53:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.1.2.1  1996/01/24  23:32:11  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.1  1996/01/23  21:43:13  gstovall
 * Placeholders for the king.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SERIAL_PRIVATE_H__
#define __SERIAL_PRIVATE_H__

/*
 * Flap protection constants
 */
#define SERIAL_FLAP_RATE    2000 /* Maximum allowed interface flaps a second */
#define SERIAL_FLAP_BACKOFF 3    /* Backoff interval after flap protection */

/*
 * Bandwidth values for display
 */
#define VISIBLE_MCI_SERIAL_BANDSLOW 56  
#define VISIBLE_MCI_SERIAL_BANDFAST 1544
#define VISIBLE_FCI_SERIAL_BANDDS3  45045
#define VISIBLE_FCI_SERIAL_BANDOC3  155000

/*
 * Delay values
 */
#define MCI_SERIAL_DELAY 2000		/* 20 millisecond */
#define HSSI_SERIAL_DELAY 20            /* 200 microsecond */
#define POS_SERIAL_DELAY 10

/*
 * Serial subblock definition
 */
typedef struct serialsb_ {
    boolean hssi_loop_req;      /* loopback requested remotely */
    boolean hssi_iclk;          /* Set internal or external clocking */
    boolean hssi_lc_inhibit;    /* Disable LC input on hssi interface */

    /*
     * Generic serial interface statistics. Note that these are duplicated
     * below for the users who like to "clear counters" for interactive
     * work. If you change/add/delete one of these, check to see if you
     * need to do the same to one of the "si_" prefixed variables below.
     * (sigh).
     */
    ulong applique;	    /* no. of applique errors (link ctrlr) */
    ulong rx_disabled;	    /* no. of times receiver disabled (link) */
    ulong rx_ais;	    /* no. rx alarm indication signals*/
    ulong rx_lof;	    /* no. rx loss of frame signals*/
    ulong rx_los;	    /* no. rx loss of signal*/
    ulong rx_rai;	    /* no. remote alarm indicator signals*/
    
    /* 
     * These are the duplicates of the above for interactive work. They
     * contain the values of the variables as of the last time "clear
     * counter" was done for this interface. (The "si_" prefix stands for
     * "show interface"
     */
    ulong si_applique;	    /* no. of applique errors (link ctrlr) */
    ulong si_rx_disabled;   /* no. of times receiver disabled (link) */
    ulong si_rx_ais;	    /* no. rx alarm indication signals*/
    ulong si_rx_lof;	    /* no. rx loss of frame signals*/
    ulong si_rx_los;	    /* no. rx loss of signal*/
    ulong si_rx_rai;	    /* no. remote alarm indicator signals*/

    int serial_protocol;	        /* firmware protocol type code */
    sys_timestamp serial_restarttimer;	/* timer for periodic resets */
    ulong serial_restartdelay;  	/* value for periodic resets */
    ulong serial_restartdelay_def;	/* default value for resets */
    int serial_clockindex;              /* index into a baud rate array */
    boolean dce_terminal_timing_enable;
    boolean invert_txc;
    boolean nrzi_enable;	
    boolean crc_32_enabled;      /* CRC-32/CRC-16 computation flag */
    ushort start_slot;           /* For G.703 interfaces */
    ushort stop_slot;            /* For G.703 interfaces */
    boolean timeslot16_enabled;  /* For G.703 interfaces */
    boolean crc4_enabled;        /* For G.703 interfaces */
    boolean clocksource_enabled; /* For G.703 interfaces, set for */
                                 /* internal, reset for line recovered */
    boolean rx_sync_disable;     /* 5/1 port adapter command on 4T */
    boolean ignore_dcd;          /* ignore dcd command */
    boolean ignore_local_loopback; /* ignore local loopback signal */
    boolean downiflooped;        /* force interface down when looped */
    boolean posi_clock;          /* packet over sonet clocking */ 
    boolean posi_loop_local;     /* packet over sonet serial loop */
    boolean posi_loop_external;  /* packet over sonet front end loop */
    boolean posi_framing;        /* sonet/sdh mode*/  
    boolean controlled_carrier;  /* HDX controlled carrier enabling flag */
    ulong hdx_rts_timeout;
    ulong hdx_cts_delay;
    ulong hdx_rts_drop_delay;
    ulong hdx_cts_drop_timeout;
    ulong hdx_dcd_txstart_delay;
    ulong hdx_dcd_drop_delay;
    ulong hdx_transmit_delay;
 
    /*
     * Fields used in debug code for some serial interfaces.
     */
    boolean hdx_log_enable;       /* Whether to log half-duplex events */

    /*
     * Vector to hold pre-existing state/config vector which was
     * in place before we initialized a serial IDB as a serial IDB.
     * If the serial state/config vector is not able to handle one
     * or more items, that itemlist should be passed to the "super"
     * function.
     */
    hwidb_state_config_t super_state_config;
    hwidb_state_config_t hw_super_state_config;

    ser_autodetect_info *ser_autodetect;  /* Serial autodetect info */

    /*
     * Hdlc keepalive stuff
     */
    ulong hdlc_myseq;		/* current probe sequence number */
    ulong hdlc_yoursseen;		/* what we believe of other side */
    ulong hdlc_mineseen;		/* what other side believes of us */
    ulong hdlc_unique_sent;         /* last unique seq - loopback detect */
    ulong hdlc_unique_rcvd;         /* last unique seq - loopback detect */
    sys_timestamp hdlc_last_keep;   /* time last keepalive arrived */
    tinybool hdlc_got_keep;
    tinybool hdlc_got_slarp;
    uchar   hdlc_no_keep_count;

} serialsb;


/*
 * Prototypes:
 */
boolean serial_high_priority(ushort);
void serial_idb_init(hwidbtype *, ushort, ulong, ulong, ulong);
boolean serial_pulse_DTR(hwidbtype *idb);
void create_serial_sb(hwidbtype *hwidb);

/*
 * Externs
 */
extern void serial_parser_init(void);

#endif __SERIAL_PRIVATE_H__
