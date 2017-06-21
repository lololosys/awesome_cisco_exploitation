/* $Id: srb_multiring.h,v 3.1.2.1 1996/07/23 18:53:58 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/srb_multiring.h,v $
 *------------------------------------------------------------------
 * srb_multiring.h -- SRB multiring definition and accessor routines
 *
 * July 1996, Paul Pearce
 *
 * Copyright (c) 1996 by Cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: srb_multiring.h,v $
 * Revision 3.1.2.1  1996/07/23  18:53:58  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *
 * Revision 3.1  1996/07/23  18:45:35  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SRB_MULTIRING_H__
#define __SRB_MULTIRING_H__

#include "interface.h"


#define MULTIRING_BIT_NONE   0x00000000
#define MULTIRING_BIT_APOLLO 0x00000001
#define MULTIRING_BIT_APPLE  0x00000002
#define MULTIRING_BIT_CLNS   0x00000004
#define MULTIRING_BIT_DECNET 0x00000008
#define MULTIRING_BIT_IP     0x00000010
#define MULTIRING_BIT_NOVELL 0x00000020
#define MULTIRING_BIT_OTHER  0x00000040
#define MULTIRING_BIT_VINES  0x00000080
#define MULTIRING_BIT_XNS    0x00000100
#define MULTIRING_BITS_ALL   0x000001FF


static inline ulong srb_get_multiring_bits (idbtype *swidb)
{
    return(swidb->srb_multiring_bits);
}


static inline void srb_set_multiring_bits (idbtype *swidb, ulong bits)
{
    swidb->srb_multiring_bits = bits;
}

static inline ulong srb_get_allroutes_bits (idbtype *swidb)
{
    return(swidb->srb_allroutes_bits);
}


static inline void srb_set_allroutes_bits(idbtype *swidb, ulong bits)
{
    swidb->srb_allroutes_bits = bits;
}


static inline boolean srb_multiring_any_enabled(idbtype *swidb)
{
    return(swidb->srb_multiring_bits != MULTIRING_BIT_NONE);
}


static inline boolean srb_multiring_none_enabled(idbtype *swidb)
{
    return(swidb->srb_multiring_bits == MULTIRING_BIT_NONE);
}


static inline boolean srb_multiring_apollo_enabled(idbtype *swidb)
{
    return(swidb->srb_multiring_bits & MULTIRING_BIT_APOLLO);
}


static inline boolean srb_multiring_apple_enabled(idbtype *swidb)
{
    return(swidb->srb_multiring_bits & MULTIRING_BIT_APPLE);
}


static inline boolean srb_multiring_clns_enabled(idbtype *swidb)
{
    return(swidb->srb_multiring_bits & MULTIRING_BIT_CLNS);
}


static inline boolean srb_multiring_decnet_enabled(idbtype *swidb)
{
    return(swidb->srb_multiring_bits & MULTIRING_BIT_DECNET);
}


static inline boolean srb_multiring_ip_enabled(idbtype *swidb)
{
    return(swidb->srb_multiring_bits & MULTIRING_BIT_IP);
}


static inline boolean srb_multiring_novell_enabled(idbtype *swidb)
{
    return(swidb->srb_multiring_bits & MULTIRING_BIT_NOVELL);
}


static inline boolean srb_multiring_other_enabled(idbtype *swidb)
{
    return(swidb->srb_multiring_bits & MULTIRING_BIT_OTHER);
}


static inline boolean srb_multiring_vines_enabled(idbtype *swidb)
{
    return(swidb->srb_multiring_bits & MULTIRING_BIT_VINES);
}


static inline boolean srb_multiring_xns_enabled(idbtype *swidb)
{
    return(swidb->srb_multiring_bits & MULTIRING_BIT_XNS);
}


static inline boolean srb_multiring_all_enabled(idbtype *swidb)
{
    return(swidb->srb_multiring_bits & MULTIRING_BITS_ALL);
}


static inline void srb_multiring_enable_all(idbtype *swidb)
{
    swidb->srb_multiring_bits = MULTIRING_BITS_ALL;
}

static inline void srb_multiring_disable_all(idbtype *swidb)
{
    swidb->srb_multiring_bits = MULTIRING_BIT_NONE;
}

#endif __SRB_MULTIRING_H__
