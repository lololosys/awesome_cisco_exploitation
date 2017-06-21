/* $Id: translate.h,v 3.6.6.1 1996/04/29 17:32:57 eschaffe Exp $
 * $Source: /release/112/cvs/Xsys/pt/translate.h,v $
 *------------------------------------------------------------------
 * Translate.h -- Data definitions for protocol translation
 *
 * May 1990,  Bill Westfield
 *
 * Copyright (c) 1987-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: translate.h,v $
 * Revision 3.6.6.1  1996/04/29  17:32:57  eschaffe
 * CSCdi54905:  vty-async packets going from x25 to ethernet are delayed
 * Branch: California_branch
 *
 * Revision 3.6  1996/03/05  08:52:31  billw
 * CSCdi49662:  Random crashes tacacs+ accounting may be implicated.
 * Increase stack size of tty_daemon and protocol translation processes.
 *
 * Revision 3.5  1996/02/22  05:20:33  fox
 * CSCdi35908:  Should allow CHAP and PAP clients to same dialin pool
 * Add support for multiple acceptable inbound PPP authentication methods
 *
 * Revision 3.4  1996/01/23  23:45:34  vandys
 * CSCdi45547:  Desire rotary behavior on PT TCP destinations
 * Add rotor option to randomize destination for TCP host lists
 *
 * Revision 3.3  1995/11/20  23:22:06  ahh
 * CSCdi41919:  PT: cannot specify source interface for TCP connections
 * Add source-interface option to outgoing TCP translation and telnet.
 * Also convert some parser defines to enums.
 *
 * Revision 3.2  1995/11/17  19:00:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:02:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:22:32  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/10/24  05:07:29  eschaffe
 * CSCdi41300:  translate commands keyword options should nv-genn in lower
 * case
 *
 * Revision 2.3  1995/10/13  21:48:11  eschaffe
 * CSCdi18102:  Require one translate statment to multiple hosts
 *         Add "host-name" Telnet/TCP outgoing option.
 *
 * Revision 2.2  1995/09/18  19:38:06  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.1  1995/06/07  22:35:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TRANSLATE_H__
#define __TRANSLATE_H__

#include "access.h"
#include "../x25/x25.h"
#include "../wan/parser_defs_ppp.h"
#include "../tcp/telnet.h"

#define TRANS_NONE -1
#define TRANS_TCP 0
#define TRANS_X25 1
#define TRANS_LAT 2
#define TRANS_SLIP_PPP 3
#define TRANS_AUTOCOMMAND 4
#define TRANS_NPROT 5
#define INPROT(val) (val / TRANS_NPROT)
#define OUTPROT(val) (val % TRANS_NPROT)
#define TRANSTYPE(IN, OUT) (IN*TRANS_NPROT + OUT)
#define TRANS_QUEUE_MAX 30
#define TRANS_LINE_COUNT 4
#define MAXCHRS 80

/*
 * Define default stack size for cfork() in pt.
 */
#define PT_DEFAULT_STACK_SIZE 1200

#ifdef SV_STR
#if (SV_STR != 17)
#error translate.h: SV_STR has the wrong value
#endif
#else
#define SV_STR 17
#endif

#ifdef OB_STR
#if (OB_STR != 17)
#error translate.h: OB_STR has the wrong value
#endif
#else
#define OB_STR 17
#endif

#ifdef ND_STR
#if (ND_STR != 17)
#error translate.h: ND_STR has the wrong value
#endif
#else
#define ND_STR 17
#endif

typedef struct trans_tcpinfo_ {
    ipaddrtype ipaddress;
    int tcpport;
    nametype *tcpnptr;
    uint tcpflags;
    idbtype *source_idb;
} trans_tcpinfo;

/*
 * Bits in "tcpflags".  Note that the low 16 bits are reserved for
 * values from tcp/telnet.h's OPENFL_* values.
 */
#define TRANSF_TCP_STREAM	OPENFL_STREAM
#define TRANSF_TCP_ROTOR	OPENFL_ROTOR
#define TRANSF_TCP_BINARY	0x00010000
#define TRANSF_TCP_PRINTER	0x00020000
#define TRANSF_TCP_IACHACK	0x00040000
#define TRANSF_TCP_HOST_NAME	0x00080000

typedef struct trans_x25info_ {
    uchar x121addr[X25_ADDRESS_LENGTH]; /* (outgoing) x121 address */
    uchar cud[X25_CALL_USER_DATA];	/* additional call user data */
    ulong cudprefix;			/* replacement for protocol id */
    ulong x25flags;			/* random flags */
    struct profiletype_ *profile;	/* initial X.3 profile to use */
} trans_x25info;

#define TRANSF_X25_REVERSE       0x00000001
#define TRANSF_X25_ACCEPTREVERSE 0x00000002
#define TRANSF_X25_PVC           0x00000004
#define TRANSF_X25_PRINTER       0x00000008
#define TRANSF_X25_NOREVERSE     0x00000010
#define TRANSF_X25_ALL           0x0000001F

typedef struct trans_latinfo_ {
    uchar latservice[SV_STR+1];
    uchar latport[OB_STR+1];
    uchar latnode[ND_STR+1];
    ulong latflags;
    void *latas;
} trans_latinfo;
#define TRANSF_LAT_BINARY        0x00000001
#define TRANSF_LAT_UNADVERTISED  0x00000002

typedef struct trans_slip_ppp_info_ {
    ipaddrtype ipaddress;
    char *poolname;            /* Use a specific local pool name*/
    int	proto;
    short keepalive_period;
    ushort mtu;
    uchar slip_ppp_flags;
    uchar authtypes[PPP_AUTH_NUM];
    idbtype *ipx_idb;
    char aaa_method[32];
    int confreq_resttime;      /* Timeout time for CONFREQ in miliseconds */
} trans_slip_ppp_info;
#define TRANSF_SLIP_PPP_HEADERCOMPRESSED  0x00000001
#define TRANSF_SLIP_PPP_ROUTING           0x00000002
#define TRANSF_SLIP_PPP_PASSIVE           0x00000004
#define TRANSF_PPP_AUTH                   0x00000008
#define TRANSF_SLIP_PPP_USE_TACACS        0x00000020

typedef struct trans_autoinfo_ {
    char *autocommand_text;
} trans_autoinfo;


struct translatetype_ {
    struct translatetype_ *next;
    int type;
    int accessclass;
    ulong flags;
    ulong active;
    ulong totaluses;
    ulong failures;
    ulong maxusers;
    ulong maxallowed;
    union {
	trans_tcpinfo tcp;
	trans_x25info x25;
	trans_latinfo lat;
    } in;
    union {
	trans_tcpinfo tcp;
	trans_x25info x25;
	trans_latinfo lat;
	trans_slip_ppp_info slip_ppp;
	trans_autoinfo autocommand;
    } out;
};

#define TRANSF_GLOB_LOCAL       0x00000001
#define TRANSF_GLOB_LOGIN       0x00000002
#define TRANSF_GLOB_QUIET       0x00000004
#define TRANSF_GLOB_SWAP        0x00000008
#define TRANSF_GLOB_ALL         0x0000000F


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

extern queuetype profileQ;
extern char *padques;

/*
 * pt_parse.c
 */
void translate_command(parseinfo *csb);
boolean translate_delete(translatetype *trans);
void nv_translate(void);
void pt_do_login(translatetype *trans, tt_soc *tty);
char *pt_trans_string(int prot);
int pt_default_translate_command(parseinfo *csb, translatetype *trans, int dir);
boolean pt_default_add_alias(translatetype *trans);
boolean pt_default_compare_entry(translatetype *trans1, translatetype *trans2);
boolean pt_default_delete_alias(translatetype *trans);
void pt_init(subsystype *subsys);

/*
 * translate_chain.c
 */
extern void pt_parser_init(void);
extern boolean translate_debug;
#endif __TRANSLATE_H__
