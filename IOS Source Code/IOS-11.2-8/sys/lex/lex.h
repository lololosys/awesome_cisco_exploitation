/* $Id: lex.h,v 3.3.62.2 1996/08/07 09:01:30 snyder Exp $
 * $Source: /release/112/cvs/Xsys/lex/lex.h,v $
 *------------------------------------------------------------------
 * lex.h -- LEX Definitions
 *
 * Feb. 1994, David Coli
 *
 * Copyright (c) 1994-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lex.h,v $
 * Revision 3.3.62.2  1996/08/07  09:01:30  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3.62.1  1996/06/24  09:21:24  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.3  1995/11/17  17:42:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:01:06  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:34:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/25  11:44:37  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.1  1995/06/07 21:40:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LEX_H__
#define __LEX_H__

#include "lex_public.h"

#define LEX_HEADER_LEN   	2

/* 
 * Buffer size for idb->namestring for lex remote interface, which is a 
 * concatenation of "LexYYY" and "-Ethernet0" or "-Serial0" .
 */
#define LEX_MAX_NAMESTR   64  


typedef struct lex_info_t_ {
    struct lex_info_t_ *next;
    char *name;
    idbtype *swidb;               
    iabtype *serial_iab;          /* pointer to serial accting block */
    iabtype *ether_iab;           /* pointer to eth interface acct blk */
    uchar  last_bia[8];           /* burned in addr of last lex bound */
} lex_info_t;

/* 
 * lex_idb_info
 * A structure that is hung off of the Lex software IDB and has fields
 * corresponding to every lex-specific configuration command.
 */
struct lex_idb_info {
    int    lex_type_in;           /* input access list for Ethernet types */
    int    lex_address_in;        /* input access list for Ethernet addresses */
    uint   retry_count;           /* number of times to re-send remote command */
    ulong  rcmd_timeout;          /* timeout for re-sending a remote command */
    lex_stats_area_t *lex_stats;  /* pointer to lex device interface stats */
    lex_inventory_msg *inventory; /* Flash size, MAC-addr, serial #, etc. */
    sys_timestamp last_ether_cclear; /* Time since clearing remote ether counters */
    sys_timestamp last_serial_cclear; /* Time since clearing remote serial counters */
    ushort lex_priority_list;     /* list # if priority queuing at lex box */
    char last_remote_ether_lineproto;  
                                  /* last ether lineprotocol rcvd from 1000 */
} ;
typedef struct lex_idb_info lex_idb_info_t;

/* definitions for last known remote ether line protocol */
#define LEXETHER_LINEPROTOCOL_UP     0x1
#define LEXETHER_LINEPROTOCOL_DOWN   0x2
#define LEXETHER_LINEPROTOCOL_UNSURE 0x3

#define LEX_INFO_SIZE	sizeof(lex_info_t)

#define FOR_ALL_LEX(q,x)	\
    for ((x) = (lex_info_t *)((q))->qhead; (x); (x) = (x)->next)

/*
 * Externs
 */
extern queuetype lex_activeQ;
extern queuetype lex_freeQ;
extern const char lex_no_inventory[];

			 
/* Function Prototypes */
void lex_parser_init(void);
void lex_add_parser_callbacks(void);
void lexdump(unsigned char *, int , char *);
void lex_debug_init(void);
void lex_bind(idbtype *, lex_info_t *);
void lex_unbind(lex_info_t *);
lex_info_t *lex_find_free_bymacaddr(uchar const *);
lex_info_t *lex_find_active_byidb(idbtype *, boolean);
lex_info_t *lex_find_free_byidb(idbtype *, boolean);
void lex_bind_bymac(idbtype *, uchar *);
void lex_unbind_byidb(idbtype *);
void lex_show_inventory(idbtype *);
boolean lex_show_controller(hwidbtype *, parseinfo *);
void lex_input_type_list_changed(ulong);
void lex_input_address_list_changed(ulong);
void lex_show_interface(parseinfo *csb);
boolean lex_find_free_bymac(uchar *);
void lex_show_ether(idbtype *, lex_ether_counters_t *,
		    lex_ether_counters_t *);
void lex_show_serial(idbtype *, lex_serial_counters_t *,
		     lex_serial_counters_t *);
void lex_reset(hwidbtype *);
boolean is_lex_configured(void);
boolean setup_lex_iabs (idbtype *, lex_info_t *);
void lex_set_span_mac_offset (hwidbtype *);

#endif
