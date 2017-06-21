/* $Id: dnconversion.h,v 3.2 1995/11/17 08:59:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/decnet/dnconversion.h,v $
 *------------------------------------------------------------------
 * dnconversion.h -- Definitions for decnet Phase IV / Phase V conversion 
 * 
 * May 1990, P. Knibbe
 *
 * Copyright (c) 1990-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dnconversion.h,v $
 * Revision 3.2  1995/11/17  08:59:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:16:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:23:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __DNCONVERSION_H__
#define __DNCONVERSION_H__

#include "../clns/clns.h"
#include "../decnet/decnet.h"

/*
 * The following constants are used to mark each adjacency
 *    Adjacencies can be 
 *        a) Phase IV only
 *        b) Phase V only
 *        c) Phase IV or Phase V
 *
 * End systems adjacencies will be marked 'PhaseIV only' or PHaseV only'
 * Routers will be marked according to which packet types they will 
 * accept. (NOTE that Phase V routers may accept either packet format).
 *
 * Definitions can be found in decnet.h.
 */

#define PHASEIVPLUS_NSP 0x83
#define PHASEIV_NOT_NSP 0x83
#define NSP 32                           /* NSP selector */
#define PHASEVADDRTYPE_SIZE 9

#define phasevaddrtype struct phasevaddrtype_
phasevaddrtype {
    ushort loc_area;
    uchar id[4];
    uchar addr[2];
    uchar sel;
};

/*
 * dncnv_chain.c
 */
extern void dncnv_parser_init(void);

/*
 * dnconversion.c
 */
extern void dn_inform_clns (uchar, uchar);
extern void dn_build_clns_prefix (uchar, uchar *);
extern boolean dn_check_clns_prefix (uchar);
extern void dn_clear_discard_routes(void);
extern void dn_add_phaseIVroute (dnnet *, uchar, int, int);

#endif __DNCONVERSION_H__
