/* $Id: ipmobile.h,v 3.2 1995/11/17 17:34:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/ipmobile.h,v $
 *------------------------------------------------------------------
 * ipmobile.h -- Definitions for IP mobility
 *
 * Nov. 1993 Tony Li
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipmobile.h,v $
 * Revision 3.2  1995/11/17  17:34:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:07:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Useful constants
 */

#define MOBILE_ARP_DEFKEEP	(5*ONEMIN) /* default periodic arp interval */
#define MOBILE_ARP_DEFHOLD	(3*MOBILE_ARP_DEFKEEP) /* defualt holdtime */
#define MOBILE_DISTANCE 	180


/*
 * Mobile host database entry
 */

typedef struct mhdbtype_ {
    struct mhdbtype_ *next;		/* Link to next */
    ipaddrtype lower;			/* Lower bound */
    ipaddrtype upper;			/* Upper bound, 0.0.0.0 if unused */
    boolean privacy;			/* TRUE if privacy configured at HA */
    char *password;			/* pointer to password, NULL if none */
    uchar enctype;			/* Encryption mechanism for config */
    uchar netenctype;			/* Encryption mechanism */
} mhdbtype;

/*
 * Structures for messages
 */


/*
 * ipmobile_chain.c
 */ 
void ip_mobile_parser_init(void);

/*
 * ipmobile.c
 */ 
mhdbtype *ip_mobile_mhdblookup(ipaddrtype address);
boolean ip_mobile_arp_accept(idbtype *idb, ipaddrtype address);
void ip_mobile_arp_new_host(uchar *modeptr, ipaddrtype address, idbtype *idb);
void ip_mobile_command(parseinfo *csb);
void ip_mobile_host_command(parseinfo *csb);
void ip_mobile_init(void);
