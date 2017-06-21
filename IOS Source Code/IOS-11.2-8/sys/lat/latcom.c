/* $Id: latcom.c,v 3.2.62.2 1996/07/29 19:02:16 snyder Exp $
 * $Source: /release/112/cvs/Xsys/lat/latcom.c,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: latcom.c,v $
 * Revision 3.2.62.2  1996/07/29  19:02:16  snyder
 * CSCdi64402:  declare arrays in lat that should be const, const
 *              save 1084 data, 68 out of image
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  20:39:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/03/07  10:04:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2  1995/11/17  17:33:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:25:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    latcom		(i) define lat common variables

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latas.h"
#include "latcb.h"
#include "lathi.h"
#include "latls.h"
#include "latnd.h"
#include "latob.h"
#include "latqr.h"
#include "latsb.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

/*----------------------------------------------------------------------
    Declare our copyright statement.
 -----------------------------------------------------------------------*/
#ifdef NOBODY_CALLS_ME
    static const char	lat_id_1[] = "SuperLAT. Copyright 1990, Meridian\n\r";
    static const char	lat_id_2[] = "Technology Corporation, St. Louis,\n\r";
    static const char	lat_id_3[] = "MO. All rights reserved\n\r";
#endif /* NOBODY_CALLS_ME */

/*----------------------------------------------------------------------
    Define engine-wide time units.
 -----------------------------------------------------------------------*/
    int		tics_uncounted = 0;	/* Uncounted tics		*/
    int		tu_msec_tic = MSEC_TIC;	/* Milliseconds/tic 		*/
    int		tu_secs_toc = SECS_TOC;	/* Seconds/toc			*/

/*----------------------------------------------------------------------
    Define engine-wide run-time options.
 -----------------------------------------------------------------------*/
    int		le_nd_retain = LE_ND_RETAIN; /* Retain remote nodes	*/
    int		le_ls_dups   = LE_LS_DUPS;   /* Allow duplicate names	*/

#if LE_OBJECTS
/*----------------------------------------------------------------------
    Define engine-wide list headers.
 -----------------------------------------------------------------------*/
    ob_ptr	ob_head[2] = {0,0};	/* Object list header		*/
    int		ob_maximum = OB_MAXIMUM;/* Maximum objects		*/
    int		ob_current = 0;		/* Current objects		*/
    int		ob_highest = 0;		/* Highest objects		*/
#else
    ob_node	ob_dummy;		 /* Dummy object structure	*/
#endif

#if (LE_OBJECTS || CME_SLAT) && LE_OBJECT_HIC
    qr_ptr	qr_head[2] = {0,0};	/* Queued request header	*/
    int		qr_maximum = QR_MAXIMUM;/* Maximum queued requests	*/
    int		qr_current = 0;		/* Current queued requests	*/
    int		qr_highest = 0;		/* Highest queued requests	*/
    int		qr_changes = 0;		/* Possible queue changes	*/
    word	qr_que_no  = 0;		/* Current queue number		*/
#endif

/*----------------------------------------------------------------------
    Define the lookaside list headers for permanent services, nodes.
 -----------------------------------------------------------------------*/
    int		ls_hold_num = 0;	/* Number  services held	*/
    ls_ptr	ls_hold[2]  = {0,0};	/* Learned services, lookaside	*/

    int		nd_hold_num = 0;	/* Number remote nodes held	*/
    nd_ptr	nd_hold[2]  = {0,0};	/* Remote node     , lookaside	*/

/*----------------------------------------------------------------------
    Define the pending reject tables. These are only used to reject
    a start when no session block can be allocated.
 -----------------------------------------------------------------------*/
    cb_ptr	reject_cb[SB_REJECTS] = {0};	/* CB of rejected slot	*/
    byte	reject_id[SB_REJECTS] = {0};	/* ID of rejected slot	*/
    int		reject_index = 0;		/* # of pending rejects	*/

/*----------------------------------------------------------------------
    Define and initialize the object mapping array used to find objects
    associated with advertised services.
 -----------------------------------------------------------------------*/
#if LE_OBJECTS
    ob_ptr	ob_map[AS_OBJECTS] = {0};
#endif

/*----------------------------------------------------------------------
    Define pointer to messages to ourselves (head/tail pointer).
 -----------------------------------------------------------------------*/
    of_ptr	of_local[2] = {0,(of_ptr)&of_local[0]};

/*----------------------------------------------------------------------
    Define the local error variables.
 -----------------------------------------------------------------------*/
    int		lat_error = 0;		/* Current local error		*/

/*----------------------------------------------------------------------
    Define the temporary names, addresses,
 -----------------------------------------------------------------------*/
    byte	as_temp[SV_STR] = {0};	/* Local AS name		*/
    byte	ls_temp[SV_STR] = {0};  /* Local LS name		*/
    byte        nd_temp[ND_STR] = {0};  /* Local ND name		*/
    byte	ob_temp[OB_STR] = {0};	/* Local OB name		*/
    byte	id_temp[ID_STR] = {0};  /* Local ID string		*/

    word	nd_addr[3]      = {0};  /* Local ND address		*/
    word	src_addr[3]	= {0};	/* Local source address		*/
    word	dst_addr[3]	= {0};	/* Local destination address	*/

    hi_node	hi_dummy;		/* Dummy HI structure	*/

/*-----------------------------------------------------------------------
    Define legal character, conversion array. A non-zero value is legal.
    A value of 2 is a lower-case character.
 -----------------------------------------------------------------------*/
    const byte c_codes[256] =
    {
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	/*   0 -   f		*/
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	/*  10 -  1f		*/
        0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,0,	/*  20 -  2f		*/
        1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,	/*  30 -  3f		*/
        0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	/*  40 -  4f		*/
        1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,	/*  50 -  5f		*/
        0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,	/*  60 -  6f		*/
        2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,	/*  70 -  7f		*/
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	/*  80 -  8f		*/
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	/*  90 -  9f		*/
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	/*  a0 -  af		*/
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	/*  b0 -  bf		*/
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	/*  c0 -  cf		*/
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	/*  d0 -  df		*/
        2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,	/*  e0 -  ef		*/
        2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,	/*  f0 -  ff		*/
    };

/*----------------------------------------------------------------------
    Define and initialize the local system paramaters.
 -----------------------------------------------------------------------*/

    int lat_host_delay = 0;

    hs_node hs_local =	 		/* Local system parameters	*/
    {
	0,				/* Link to next host structure	*/

	{0,0},				/* AS list header		*/
	{0,0},				/* LS list header		*/
	{0,0},				/* HI list header		*/

	{{0,0}},			/* ND hash table		*/
	{0},				/* CB look table		*/
	{0},				/* Circuit sequence numbers	*/
	1,				/* Current circuit index	*/

#if NET_ORDER
	{0x0900,0x2b00,0x000f},		/* LAT master multicast addr.	*/
	{0x0900,0x2b02,0x0104},		/* LAT slave multicast addr.	*/
	0x6004,				/* Ethernet protocol number	*/
#else
	{0x0009,0x002b,0x0f00},		/* LAT master multicast addr.	*/
	{0x0009,0x022b,0x0401},		/* LAT slave multicast addr.	*/
	0x0460,				/* Ethernet protocol number	*/
#endif
	0x05,0x05,			/* High, low version supported	*/
	0x05,0x02,			/* ECO, protocol version	*/
	0,				/* Facility code		*/
	SUPERLAT_CODE,			/* Product  code		*/
	SUPERLAT_VER,			/* Product  version		*/

	CB_MAXIMUM,			/* Maximum number of circuits	*/
	SB_MAXIMUM,			/* Maximum number of sessions	*/
	AS_MAXIMUM,			/* Maximum advertised services	*/
	LS_MAXIMUM,			/* Maximum learned services	*/
	ND_MAXIMUM,			/* Maximum remote nodes		*/
	HI_MAXIMUM,			/* Maximum HIC requests		*/

	0,				/* Current number of circuits	*/
	0,				/* Current number of sessions	*/
	0,				/* Current advertised services	*/
	0,				/* Current learned services	*/
	0,				/* Current remote nodes		*/
	0,				/* Current HIC requests		*/

	0,				/* Highest number of circuits	*/
	0,				/* Highest number of sessions	*/
	0,				/* Highest advertised services	*/
	0,				/* Highest learned services	*/
	0,				/* Highest remote nodes		*/
	0,				/* Highest HIC requests		*/

	DLL_MAX,			/* Maximum datalink output size	*/
        INP_MAX,			/* Maximum datalink input  size	*/

#if !LE_HOST && !LE_OBJECT_HIC
	SVC_ANNC_EN,			/* No starts, no commands	
					   svc. announcements enabled	*/
#endif
#if  LE_HOST && !LE_OBJECT_HIC
	SVC_ANNC_EN | RI_START,		/* Allows starts, no commands	
					   svc. announcements enabled	*/
#endif
#if !LE_HOST &&  LE_OBJECT_HIC
	SVC_ANNC_EN | RI_COMMAND,	/* No starts, allows commands	
					   svc. announcements enabled	*/
#endif
#if  LE_HOST &&  LE_OBJECT_HIC
	SVC_ANNC_EN | RI_START | RI_COMMAND,	
					/* Allows starts, commands
					   svc. announcements enabled	*/
#endif

	CB_MAX_SES,			/* Maximum sessions/circuit	*/
	CB_S_RCV_EXTRA,			/* Maximum extra receives (S)	*/
	CB_H_RCV_EXTRA,			/* Maximum extra receives (H)	*/
	CB_DEF_TRY,			/* Default circuit retries	*/
	CB_VC_MSEC,			/* Default circuit timer   (ms)	*/
	CB_HD_MSEC,			/* Default host delat      (ms)	*/
	CB_KA_SECS,			/* Default keep-alive timer(s)	*/
	CB_PROGRESS,			/* Host progress interval  (s)	*/

	SB_MAX_DATA,			/* Maximum data slot size	*/
	SB_MAX_ATTN,			/* Maximum attention slot size	*/

	HI_STS_SEC,			/* HIC status  timer (s)	*/
	HI_CMD_SEC,			/* HIC command timer (s)	*/
	HI_DEF_TRY,			/* HIC command retries		*/
	0,				/* HIC next request number	*/

	DLL_MAX - SVC_OVERHEAD,		/* AS remaining message space	*/
	0,				/* Seconds till next multicast	*/
	AS_MC_SEC,			/* Current AS multicast timer	*/
	0,				/* Current AS sequence number	*/
	SVC_IDENT | SVC_GROUPS,		/* Changes in next AS message 	*/
	0,				/* Changes in last AS message	*/
	0,				/* Rating for next message	*/

	ND_AGE_MAX,			/* Maximum remote node age	*/
	ND_INTERVAL,			/* Interval to purge nodes	*/

	{1,0x01,0},			/* Groups for AS processing	*/
#if LE_OBJECTS
	{0,0x00,0},			/* Groups for LS processing	*/
#else
	{1,0x01,0},			/* Groups for LS processing	*/
#endif
    	{0},				/* Local node name  string	*/
	{0},				/* Local node ident string 	*/
	{1,SVC_CLASS_TERM},		/* Local node service classes	*/

#if LE_HS_STATS
	{0,				/* Seconds since zeroed		*/
	0,				/* Total messages received	*/
	0,				/* Total messages transmitted	*/
	0,				/* Total slots received		*/
	0,				/* Total slots transmitted	*/
	0,				/* Total bytes received		*/
	0,				/* Total bytes transmitted	*/
	0,				/* Total duplicate messages	*/
	0,				/* Total retransmitted messages	*/
	0,				/* Total bad circuit messages	*/
	0,				/* Total bad circuit slots	*/
	0,				/* Total accepted host-initiate	*/
	0,				/* Total rejected host-initiate	*/
	0},				/* Total multiple nodes seen	*/

	{0,				/* Total circuits created	*/
	0,				/* Total sessions created	*/
	0,				/* Total frames received	*/
	0,				/* Total frames transmitted	*/
	0,				/* Total illegal frames		*/
	0,				/* Total circuit timeouts	*/
	0,				/* Total service messages xmit.	*/
	0,				/* Total service messages recv.	*/
	0,				/* Total service messages used	*/
	0,				/* Total service msgs discarded	*/
	{0,0,0},			/* Source address of last error	*/
	0,			/* Source interface of last error */
	{0,0,0,0,0,0,0,0,0}},		/* Error reasons mask		*/
#endif
	0xf0f0				/* End-of-structure		*/
    };

    hs_ptr	hs = &hs_local;		/* Only node defined		*/

void latcom_init (void)
{
    memset(&ob_dummy, 0, sizeof(ob_dummy));
    memset(&hi_dummy, 0, sizeof(hi_dummy));
}
