/* $Id: sierra_wrappers.h,v 3.2 1995/11/17 18:41:32 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-4k-c4000/sierra_wrappers.h,v $
 *------------------------------------------------------------------
 * Assembly wrapper prototypes for R4k based Cisco 4000 
 *
 * November 1993, Michael Beesley
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: sierra_wrappers.h,v $
 * Revision 3.2  1995/11/17  18:41:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:23:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:53:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Ethernet NIM (Single and Dual) Prototypes
 */
extern void eim_wrapper(void);

/*
 * Serial NIM (Dual) Prototypes
 */
extern void dsim_wrapper(void);

/*
 * FDDI NIM Prototypes
 */
extern void fddi_wrapper(void);

/*
 * ATM NIM Prototypes
 */
extern void atm_wrapper(void);

/*
 * Token Ring NIM Prototypes
 */
extern void trim_wrapper(void);

/*
 * Serial NIM (Quad) Prototypes
 */
extern void qsim_wrapper(void);

/*
 * Munich32 NIM Prototypes
 */
extern void m32im_wrapper(void);

/*
 * 6 Ethernet NIM (AM79970) Prototypes
 */
extern void am79970_wrapper(void);

/*
 * NMI wrappers
 */
extern void r4k_4ms_interrupt(void);
extern void r4k_rt_timer_interrupt(void);

/* End of file */

