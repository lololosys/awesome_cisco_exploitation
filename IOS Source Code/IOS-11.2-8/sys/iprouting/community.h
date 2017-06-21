/* $Id: community.h,v 3.3.46.3 1996/08/21 08:03:57 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/community.h,v $
 *------------------------------------------------------------------
 * community.h -- IP community list definitions
 *
 * January 1995, Ravishankar Chandrasekaran
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: community.h,v $
 * Revision 3.3.46.3  1996/08/21  08:03:57  rchandra
 * CSCdi66662:  BGP: Route Reflector not enabled with peer-groups
 * Branch: California_branch
 * - set the reflector-id when peer-group members are configured
 * - in bgp_send_update do not dereference path if there is no best path
 * - all 'show ip bgp community <community>' to work in priv user mode.
 *
 * Revision 3.3.46.2  1996/07/29  21:33:14  rchandra
 * CSCdi64437:  BGP: community display is not user-friendly
 * Branch: California_branch
 * - add user friendly community display
 * - add update throttling code (CSCdi64451)
 * - allow 'set community <additive>' during redistribution into BGP
 *
 * Revision 3.3.46.1  1996/03/28  23:59:15  rchandra
 * CSCdi52098:  BGP: exact keyword not accepted by route-map in community
 * match
 * Branch: California_branch
 * - fix my silly bug in handling exact community matching
 * - limit the range of valid community to 0xFFFF0000
 * - fix a cosmetic bug in show ip community-list
 *
 * Revision 3.3  1995/12/11  07:43:10  rchandra
 * CSCdi45046:  BGP: prefer external route over confederation external
 * route
 * - prefer external route over the confederation external route
 * - do not drop routes with 'no-export' community in the sub-AS boundary
 * - add 'local-AS' special community
 *
 * Revision 3.2  1995/11/17  17:32:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:05:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __COMMUNITY_H__
#define	__COMMUNITY_H__

/*
 * community related defines.
 * Value OxFFFFFF00 to 0xFFFFFFFF are reserved for well known communities.
 */
#define COMMUNITY_LEN(x)	((x) << 2)	/* count -> len convertion */
#define COMMUNITY_COUNT(x)	((x) >> 2)	/* len -> count convertion */
#define COMMUNITY_MAX		0xFFFFFFFFU	/* Max value */
#define COMMUNITY_INTERNET	0		/* Everybody is member of it */
#define COMMUNITY_NOEXPORT	0xFFFFFF01U	/* not sent to EBGP peer */
#define COMMUNITY_NOADV		0xFFFFFF02U	/* not advertise to any peer */
#define COMMUNITY_LOCALAS	0xFFFFFF03U	/* not sent outside local AS */

#define MAX_COMLIST_ENTRY	16
#define MAX_STD_COMLIST		99
#define MAX_EXT_COMLIST		MAXREGEXPACC

#define PARSE_COMMUNITYLIST_INIT  1
#define PARSE_COMMUNITYLIST_ENTRY 2
#define PARSE_COMMUNITYLIST_END   3

typedef struct comlist_item_ {
    struct comlist_item_	*next;		/* link to next entry. */
    ulong			community[MAX_COMLIST_ENTRY];	 
    int				count;
    boolean			grant;
} comlist_item;


extern boolean new_community_format;

extern void    ip_community_command(parseinfo *);
extern void    ip_community_print_array(ulong *, int);
extern void    ip_community_show_list(parseinfo *);
extern void    ip_community_nv_array(ulong *, int);
extern void    ip_community_nvgen(parseinfo *);
extern boolean ip_community_check(ulong *, int, int, boolean);

#endif __COMMUNITY_H__
