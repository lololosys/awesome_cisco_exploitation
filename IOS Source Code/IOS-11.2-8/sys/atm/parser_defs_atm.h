/* $Id: parser_defs_atm.h,v 3.1.38.3 1996/05/09 14:11:36 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/atm/parser_defs_atm.h,v $
 *------------------------------------------------------------------
 * parser/parser_defs_atm.h: ATM parsing specific constants
 *
 * November, 1993, Rick Li Fo Sjoe
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_atm.h,v $
 * Revision 3.1.38.3  1996/05/09  14:11:36  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.38.2.2.1  1996/04/27  06:33:58  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.38.2  1996/04/25  23:02:54  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.38.1.4.1  1996/04/10  23:35:55  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.1.38.1  1996/03/29  19:28:32  grobelch
 *   Provide traffic shaping for SVCs when possible.
 *   Also:
 *   - Specify map-class rate parameters in kbps.
 *   - Provide a per-interface command to indicate
 *     whether SVC should be established if signalled
 *     parameters cannot be installed.
 *   - Don't attach ATM submaps to map-class block
 *   - Check for map-class params prior to using
 *     them (they can be null).
 *   - Fix help-strings for "clp1" parameters to
 *     indicate for clp0&1 flows.
 *   - Signal best effort if only pcr for clp0&1 are
 *     specified.
 *   - map-class parameters override map-list where
 *     they overlap.
 *   - Fix a couple of erroneous variable setting/usages
 *     in static-map handling.
 * CSCdi51550:  ATMSIG: SVCs should attempt to use traffic shaping
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/01  04:57:10  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.4  1996/01/29  07:00:45  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.3  1996/01/06  02:55:48  ronnie
 * CSCdi46517:  Cant turn off multiple arp server feature
 *
 * Revision 3.2  1995/11/17  18:49:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/26  15:15:20  schrupp
 * CSCdi37690:  Support ATM ESI addressing with ILMI prefix
 *
 * Revision 2.2  1995/06/27  00:30:38  wfried
 * CSCdi36069:  Performance tuning and changes on controller screen for
 * new interfac
 * Included more interface status in the show controller output,
 * forced ATM receiving packets to start at 16 byte boundaries for
 * better DBUS utilization, and made some fastswitching optimizations.
 * Created new ATM interface loopback command options and renamed E3
 * framing modes to reflect current terminology
 *
 * Revision 2.1  1995/06/07  22:28:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Define the ATM commands supported 
 */
enum	ATM_CMD	{
	ATM_CMD_PVC,		/* PVC management */
	ATM_CMD_IDLETIMEOUT,	/* seconds idle before shutting down SVC */
	ATM_CMD_AAL,		/* AAL selection */
	ATM_CMD_ADDRESS,	/* Set ATM interface address */
	ATM_CMD_TXBUFF,		/* Set TX buffers */
	ATM_CMD_RXBUFF,		/* Set RX buffers */
	ATM_CMD_EXCQ,		/* Set Exception Queue length */
	ATM_CMD_MTU,		/* Set ATM MTU size */
	ATM_CMD_STATIC,		/* STATIC Command ----OBSOLETE---- */
	ATM_CMD_VC,		/* Set Maximum VC */
	ATM_CMD_RATEQ,		/* Create a Rate-Queue */
	ATM_CMD_RAWQ,		/* Set RAW queue length*/
	ATM_CMD_VC_PER_VP, 	/* Maximum VCs per VPI */
	ATM_CMD_VPFILTER,	/* Set AAL3/4-AAL5 filtering */
	ATM_CMD_NSAP,		/* Set interface NSAP address */
	ATM_CMD_ESI,		/* Set interface ESI address */
	ATM_CMD_E3FRAMING,	/* Set E3 Framing type */
	ATM_CMD_DS3FRAMING,	/* Set DS3 Framing type */
	ATM_CMD_CLOCKING,	/* Set clocking source */
	ATM_CMD_SONET,		/* Set STS-3C or STM-1 SONET mode */
	ATM_CMD_SMDS,		/* Set Interface E.164 address */
	ATM_CMD_MULTICAST,	/* Set Interface E.164 Multicast address */
	ATM_CMD_MID_PER_VC,	/* Set MID-per-VC limit */
	ATM_CMD_DS3SCRAMBLE,	/* Enable/Disable DS3 scrambling */
	ATM_CMD_1577EXT,	/* Specify the 1577 extension type */
	ATM_CMD_ARPSERV,	/* Configure the IP ARP Server */
	ATM_CMD_ADDREG,		/* Enable/Set-Interval for Addr Registration */
	ATM_CMD_ILMIPOLL_INTERVAL, /* Enable/Set-Interval for ILMI Poll */
	ATM_CMD_MTP_INTERVAL,   /* Set minimum multipoint AddParty interval */
	ATM_CMD_UNI_VERSION,    /* set uni version */
	ATM_CMD_MTP_SIG,	/* Enable multipoint signalling */
	ATM_CMD_MTUREJECT,      /* Either warn or reject call on mtu mismatch*/
        ATM_CMD_CLASS,           /* Set default map-class for interface */
	ATM_CMD_SIG_TRAF_SHAPE,  /* Determine strictness of traffic shaping*/
};


#define ATM_MAP_VC   1
#define ATM_MAP_NSAP 2
#define	ATM_MAP_SMDS 3

#define ATM_MAP_MIN  1
#define ATM_MAP_MAX  16777215

/* max rate in signalling messages, in kbps (whole cell) */
/* ( (ATM_MAP_MAX * 424) /1000 )  */
#define ATM_MAP_RATE_MIN 0
#define ATM_MAP_RATE_MAX 7113539

#define ATM_MAP_BROAD  1
#define ATM_MAP_FWPK0  2
#define ATM_MAP_BKPK0  3
#define ATM_MAP_FWPK1  4
#define ATM_MAP_BKPK1  5
#define ATM_MAP_FWST0  6
#define ATM_MAP_BKST0  7
#define ATM_MAP_FWST1  8
#define ATM_MAP_BKST1  9
#define ATM_MAP_FWMX0  10
#define ATM_MAP_BKMX0  11
#define ATM_MAP_FWMX1  12
#define ATM_MAP_BKMX1  13
#define ATM_MAP_TRFMGT 14
#define ATM_MAP_AAL5MUX 15
#define ATM_MAP_LAYER2ID 16

#define ATM_FRAMING_DEFAULT	0
#define ATM_FRAMING_E3G751PLCP	0	/* E3 G.751 w/PLCP framing = UNFRAMED */
#define ATM_FRAMING_E3G832ADM	1	/* E3 G.832 ATM direct mapping = G804 */
#define ATM_FRAMING_E3G751ADM	2	/* E3 G.751 ATM direct mapping */
#define ATM_FRAMING_DS3CBITADM  0	/* DS3 C-bit ATM direct mapping */
#define ATM_FRAMING_DS3CBITPLCP	1	/* DS3 C-bit w/PLCP framing */
#define ATM_FRAMING_DS3M23ADM	2	/* DS3 M-23 ATM direct mapping */
#define ATM_FRAMING_DS3M23PLCP	3	/* DS3 M-23 w/PLCP framing */

#define	ATM_CLOCK_INTERNAL	0	/* TX clock generated internally */
#define	ATM_CLOCK_LINE		1	/* TX clock from line */

#define	ATM_SONET_STS3C		0	/* SONET in STS3C mode */
#define	ATM_SONET_STM1		1	/* SONET in STM1 mode */

#define ATM_MAX_TIMER 4294967		/* 4294967 secs = 49.7 days */


/* ATM show command */
enum {
    SHOW_ATM_ARPSERVER,
    SHOW_ATM_ILMI,
    SHOW_ATM_INT,
    SHOW_ATM_MAP,
    SHOW_ATM_TRAFFIC,
    SHOW_ATM_VC,
};
