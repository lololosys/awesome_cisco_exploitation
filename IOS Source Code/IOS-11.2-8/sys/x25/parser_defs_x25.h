/*
 *	P A R S E R _ D E F S _ X 2 5 . H
 *
 * $Id: parser_defs_x25.h,v 3.1 1996/02/01 05:14:43 hampton Exp $
 * $Source: /release/111/cvs/Xsys/x25/parser_defs_x25.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_x25.h,v $
 * Revision 3.1  1996/02/01  05:14:43  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.4  1996/01/29  07:01:09  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.3  1995/11/21  08:27:13  ahh
 * CSCdi42413:  XOT: cannot tune TCP keepalive behavior
 * Allow keepalive rate to be controlled for XOT connections.
 *
 * Revision 3.2  1995/11/17  18:53:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:31:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* X.25 encapsulation option flags */
#define ENCAPS_LAPB_X25_DCE		0x01	/* used for both LAPB & X.25 */
#define ENCAPS_X25_DDN			0x02
#define ENCAPS_X25_BFE			0x04
#define ENCAPS_X25_IETF			0x08

/*
 *  X29 commands
 */
#define X29_PROFILE              1
#define X29_ACCESS_LIST          2
#define X29_INVITECLEAR_TIME	 3

/*
 *  "x25 map" and "x25 pvc <#> <protocol>..." values
 */

/* option codes */

#define X25MAP_BROADCAST	0x00000001	/* valid for maps and PVCs... */
#define X25MAP_PACKETSIZE	0x00000002
#define X25MAP_WINDOWSIZE	0x00000004
#define X25MAP_PASSIVE		0x00000008
#define X25MAP_METHOD		0x00000010
#define X25MAP_REVERSE		0x00000020	/* valid only for maps... */
#define X25MAP_ACCEPT_REVERSE	0x00000040
#define X25MAP_CUG		0x00000080
#define X25MAP_NVC		0x00000100
#define X25MAP_THROUGHPUT	0x00000200
#define X25MAP_NUID		0x00000400
#define X25MAP_NUDATA		0x00000800
#define X25MAP_RPOA		0x00001000
#define X25MAP_TDELAY		0x00002000
#define X25MAP_NO_INCOMING	0x00004000
#define X25MAP_NO_OUTGOING	0x00008000
#define X25MAP_IDLE		0x00010000
#define X25MAP_PBPCOMPRESS	0x00020000

/* method values */
#define X25METHOD_CISCO		0
#define X25METHOD_IETF		1
#define X25METHOD_SNAP		2
#define X25METHOD_MULTI		3


/*
 * X25 facilities types
 */
#define FACIL_PACKET		0
#define FACIL_WINDOW		1
#define FACIL_REVERSE		2
#define FACIL_CUG		3
#define FACIL_THROUGHPUT	4
#define FACIL_RPOA		5
#define FACIL_TDELAY		6


/* X25 defines */
#define X25_HOST		 0
#define X25_ROUTING		 1
#define X25_ROUTE		 2
#define X25_RPOA		 3
#define X25_WIN			 4
#define X25_WOUT		 5
#define X25_IPS			 6
#define X25_OPS			 7
#define X25_MODULO		 8
#define X25_ADDRESS		 9
#define X25_T10			10
#define X25_T20			11
#define X25_T11			12
#define X25_T21			13
#define X25_T12			14
#define X25_T22			15
#define X25_T13			16
#define X25_T23			17
#define X25_LIC			18
#define X25_HIC			19
#define X25_LTC			20
#define X25_HTC			21
#define X25_LOC			22
#define X25_HOC			23
#define X25_FACILITY		24
#define X25_MAP			25
#define X25_PVC			26
#define X25_IDLE		27
#define X25_TH			28
#define X25_NVC			29
#define X25_DEFAULT		30
#define X25_SUPPRESS_SRC	31
#define X25_SUPPRESS_DST	32
#define X25_ACCEPTREV		33
#define X25_LINKSTART		34
#define X25_IPTOS		35
#define X25_HOLDTIME		36
#define X25_SRCADDR		37
#define X25_PADACCESS		38
#define X25_HOLDQUEUE		39
#define X25_ROUTEALIAS		40
#define X25_ROUTEIP		41
#define X25_ROUTEINTERFACE	42
#define	X25_BFE_EMER		43
#define	X25_BFE_DECISION	44
#define	X25_BFE_REMOTE		45
#define	X25_XOT_CONFIRM_SVC_RESET	46
#define	X25_TCP_KEEP_PERIOD	47
#define	X25_TCP_KEEP_TRIES	48

#define X25_MOD8		 8
#define X25_MOD128	       128

#define X25_SERVICE_IF		 0
#define X25_SERVICE_IP		 1
#define X25_SERVICE_IPPT	 2
#define X25_SERVICE_CLNPPT	 3
#define X25_SERVICE_ALIAS	 4
#define X25_SERVICE_PAD		 5
#define X25_SERVICE_FUNC	 6
#define NSERVICES		 7

#define X25_FUNC_CHARGEN	 0
#define X25_FUNC_DISCARD	 1
#define X25_FUNC_ECHO		 2
#define X25_FUNC_HELP		 3
#define NFUNCS			 4

#define X25_ROUTE_POSITION	 1

/*
 *  BFE exec commands
 */
#define PARSER_BFE_ENTER        1
#define PARSER_BFE_LEAVE        2

/* X25 show commands */
enum {
    SHOW_X25_BFE,
    SHOW_X25_MAP,
    SHOW_X25_PAD,
    SHOW_X25_ROUTE,
    SHOW_X25_VC,
};

/* X25 clear commands */
#define CLEAR_VC 1
