/* $Id: latnd.h,v 3.2 1995/11/17 17:34:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/latnd.h,v $
 *------------------------------------------------------------------
 * $Log: latnd.h,v $
 * Revision 3.2  1995/11/17  17:34:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:25:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

 /*-----------------------------------------------------------------------
    latnd		define lat node structure

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/
#ifndef LATND_H
#define LATND_H		0		/* Indicate file has been read	*/
#include "latco.h"

/*----------------------------------------------------------------------
    Define the retain node constants
 -----------------------------------------------------------------------*/
#define	ND_AGE_LIMIT	(60 * 60 * 8)	/* Maximum length to age	*/
#define ND_AGE_NEVER	0x7fff		/* Never delete aged nodes	*/

/*----------------------------------------------------------------------
    Define the node state flags.
 -----------------------------------------------------------------------*/
#define ND_INIT		0x01		/* Node needs initialization	*/
#define ND_HOLD		0x02		/* Node is on hold list		*/
#define ND_UNKNOWN	0x40		/* Node is unknown		*/
#define ND_UNREACH	0x80		/* Node is unreachable		*/

/*----------------------------------------------------------------------
    Define the node structure.
 -----------------------------------------------------------------------*/
    typedef struct nd
    {
	struct nd ND_VIEW(nd_link[2]);	/* Link to next node		*/
	struct ls LS_VIEW(nd_ls[2]);	/* Link to learned services	*/
	word    nd_host_addr[3];	/* Remote host address		*/
	idbtype *nd_idb;		/* Interface pointer            */
	int	nd_timer;		/* Service(s) entry age		*/
	int	nd_usage;		/* Entry usage counter		*/
	word	nd_flags;		/* Node state flags		*/
	word	nd_dll_max;		/* Maximum message size		*/
	byte	nd_seq;			/* Entry sequence number	*/
	byte	nd_changes;		/* Last service changes		*/
	byte	nd_acl[GROUPS];		/* Groups for this entry	*/
 	byte	nd_node[ND_STR];	/* Node name  string		*/
	byte	nd_ident[ID_STR];	/* Node ident string		*/
	byte	nd_prot_hi;		/* LAT protocol, high version	*/
	byte	nd_prot_lo;		/* LAT protocol, low version	*/
	byte	nd_prot_ver;		/* LAT protocol version		*/
	byte	nd_prot_eco;		/* LAT protocol ECO		*/
	byte	nd_mc_timer;		/* Node multicast timer		*/
	byte	nd_classes[SVC_CLASSES]; /* Service classes */

#if LE_ND_STATS
	word	nd_facility;		/* Remote facility number	*/
	byte	nd_prod_code;		/* Remote product code		*/
	byte	nd_prod_ver;		/* Remote product version	*/
	co_node	nd_stats;		/* Node statistics		*/
#endif
   } nd_node, ND_VIEW(nd_ptr);
#endif
