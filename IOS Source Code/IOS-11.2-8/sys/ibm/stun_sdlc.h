/* $Id: stun_sdlc.h,v 3.2 1995/11/17 09:24:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/stun_sdlc.h,v $
 *------------------------------------------------------------------
 * stun_sdlc.h -- Definitions for SDLC format of Serial Tunnel
 *
 * March 1991,Joel P. Bion
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stun_sdlc.h,v $
 * Revision 3.2  1995/11/17  09:24:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:49:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _STUN_P_SDLC_H_
#define _STUN_P_SDLC_H_

#define SDLC_MIN_ADDRESS (1)
#define SDLC_MAX_ADDRESS (255)

/* This is a value for N(R) or N(S) that indicates "unknown" */
#define SDLC_UNKNOWN_WINDOW (128)

/* Default poll interval (msec) */
#define SDLC_PROXY_POLL_DEFAULT (50)

/* Default primary passthrough (sec) */
#define SDLC_PRIMARY_PASSTHROUGH_DEFAULT (20)

/*
 * Support for proxy-poll... a record attached to an IDB.
 * Address[SDLC_MAX_ADDRESS + 1] stores *default* values (for the "all" category).
 */
typedef struct sdlc_proxy_address_ {
    uchar snr;
    uchar pnr;
    uchar pending_modulo;	/* Pending modulo -- 1 == computing */
    uchar modulo;
    boolean permanent;	/* Permanent (set by user) */
    boolean primary;	/* Primary side of link? */
    boolean discovery;		/* If true, discovery was done for this */
    queuetype s_to_p_queue;	/* Queue of packets waiting for primary */
    uchar state;		/* Point in state machine */
    ushort next_in_proxy;	/* Next entry in S1 state. */
    ushort prev_in_proxy;	/* Previous entry in S1 state */
    ushort poll_gen_rep;	/* Polls generated/replied */
} sdlc_proxy_address;

#define SDLC_PROXY_HEAD  (SDLC_MAX_ADDRESS + 1)
struct sdlc_proxy_interface_ {
    sdlc_proxy_address proxy_info[SDLC_MAX_ADDRESS + 2];
    boolean lock;		/* Locked data structure? */
};

typedef struct sdlc_header_ {
    uchar address;
    uchar control1;
    uchar control2;
} sdlc_header;

#define SDLC_IS_S_FRAME(a) (((a)->control1 & 0x03) == 0x01)
#define SDLC_S_FRAME_MASK (0x0c)
#define SDLC_S_FRAME_RR (0x00)
#define SDLC_S_FRAME_RNR (0x04)
#define SDLC_S_FRAME_REJ (0x08)

#define SDLC_IS_I_FRAME(a) (((a)->control1 & 0x01) == 0x00)

#define SDLC_IS_U_FRAME(a) (((a)->control1 & 0x03) == 0x03)
#define SDLC_U_FRAME_MASK  (0xEC)
#define SDLC_U_FRAME_UP    (0x20)
#define SDLC_U_FRAME_UI    (0x00)
#define SDLC_U_FRAME_SABM  (0x2c)
#define SDLC_U_FRAME_SNRM  (0x80)
#define SDLC_U_FRAME_SNRME (0xCC)
#define SDLC_U_FRAME_DISC  (0x40)
#define SDLC_U_FRAME_SIM   (0x04)
#define SDLC_U_FRAME_XID   (0xAC)
#define SDLC_U_FRAME_TEST  (0xE0)
#define SDLC_U_FRAME_CFGR  (0xC4)
#define SDLC_U_FRAME_UA    (0x60)
#define SDLC_U_FRAME_RIM   (0x04)
#define SDLC_U_FRAME_DM    (0x0C)
#define SDLC_U_FRAME_FRMR  (0x84)
#define SDLC_U_FRAME_RD    (0x40)
#define SDLC_U_FRAME_BCN   (0xEC)

#define SDLC_128_POLL (0x01)
#define SDLC_128_POLL_MASK (0x01)
#define SDLC_128_NR_MASK (0xFE)
#define SDLC_128_NR_SHIFT (0x01)
#define SDLC_128_NS_MASK (0xFE)
#define SDLC_128_NS_SHIFT (0x01)

#define SDLC_8_POLL (0x10)
#define SDLC_8_POLL_MASK (0x10)
#define SDLC_8_NR_MASK (0xE0)
#define SDLC_8_NR_SHIFT (0x05)
#define SDLC_8_NS_MASK (0x0E)
#define SDLC_8_NS_SHIFT (0x01)

#define SDLC_IS_RR (0x01)
#define SDLC_RR_MASK (0x0F)
#define SDLC_RR_8_LEN (2)
#define SDLC_RR_128_LEN (3)

#define SDLC_RR(A) (((A)->control1 & SDLC_RR_MASK) == SDLC_IS_RR)

#define SDLC_SNRM(A) ((SDLC_IS_U_FRAME((A))) && \
		      (((A)->control1 & SDLC_U_FRAME_MASK) == \
		       SDLC_U_FRAME_SNRM))

#define SDLC_SNRME(A) ((SDLC_IS_U_FRAME((A))) && \
		       (((A)->control1 & SDLC_U_FRAME_MASK) == \
			SDLC_U_FRAME_SNRME))

#define SDLC_UA(A) ((SDLC_IS_U_FRAME((A))) && \
		    (((A)->control1 & SDLC_U_FRAME_MASK) == \
		     SDLC_U_FRAME_UA))

#define SDLC_SNRM_SNRME(A) (SDLC_SNRM((A)) || SDLC_SNRME((A)))

#define SDLC_PF_SET(A, MODULO) (SDLC_IS_U_FRAME((A)) ? ((A)->control1 & SDLC_8_POLL_MASK) : \
				(((MODULO) == 128) ? \
				 ((A)->control2 & SDLC_128_POLL_MASK) : \
				 ((A)->control1 & SDLC_8_POLL_MASK)))

#define SDLC_NR(A, MODULO) (SDLC_IS_U_FRAME((A)) ? (129) : \
			    (((MODULO) == 128) ? \
			     (((A)->control2 & SDLC_128_NR_MASK) >> SDLC_128_NR_SHIFT) : \
			     (((A)->control1 & SDLC_8_NR_MASK) >> SDLC_8_NR_SHIFT)))

#define SDLC_NS(A, MODULO) (((MODULO) == 128) ? \
			    (((A)->control1 & SDLC_128_NS_MASK) >> SDLC_128_NS_SHIFT) : \
			    (((A)->control1 & SDLC_8_NS_MASK) >> SDLC_8_NS_SHIFT))

#define SDLC_RR_AND_PF(A, MODULO) (SDLC_RR((A)) && SDLC_PF_SET((A), (MODULO)))

#define SDLC_RR_PF_AND_NR(A, MODULO, NRV) \
           (SDLC_RR_AND_PF((A), (MODULO)) && ((NRV) == \
					      (SDLC_NR((A), (MODULO)))))

#define SDLC_PROXY_STATE_0 0
#define SDLC_PROXY_STATE_1 1
#define SDLC_PROXY_STATE_2 2
#define SDLC_PROXY_STATE_3 3
#define SDLC_PROXY_STATE_4 4
#define SDLC_PROXY_STATE_5 5
#define SDLC_PROXY_STATE_6 6
#define SDLC_PROXY_STATE_7 7

/*
 * Proxy packet type
 */
typedef struct sdlc_proxy_hdr_ {
    uchar sdlc_address;
    uchar sdlc_control1;
    uchar sdlc_control2;
} sdlc_proxy_hdr;

#define sdlc_proxy_headstart(ptr) ((ptr)->network_start)

#endif
