/* $Id: xx_wrappers.h,v 3.2 1995/11/17 18:44:01 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/src-68-c4000/xx_wrappers.h,v $
 *------------------------------------------------------------------
 * Assembly wrapper prototypes for 4000 
 *
 * May 1993, Scott Mackie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: xx_wrappers.h,v $
 * Revision 3.2  1995/11/17  18:44:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:25:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:55:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Ethernet NIM (Single and Dual) Prototypes
 */
extern void eim_slot1a(void), eim_slot2a(void), eim_slot0a(void);
extern void eim_slot1b(void), eim_slot2b(void), eim_slot0b(void);
extern void eim_slot1a_jump_entry(void),
            eim_slot2a_jump_entry(void),
            eim_slot0a_jump_entry(void);
extern void eim_slot1b_jump_entry(void),
            eim_slot2b_jump_entry(void),
            eim_slot0b_jump_entry(void);

/*
 * Serial NIM (Dual) Prototypes
 */
extern void dsim_slot0(void), dsim_slot1(void), dsim_slot2(void);

/*
 * FDDI NIM Prototypes
 */
extern void fim_slot1(void), fim_slot2(void), fim_slot3(void);
extern void fim_slot1_jump_entry(void),
            fim_slot2_jump_entry(void),
            fim_slot3_jump_entry(void);
extern void cam_slot1(void), cam_slot2(void), cam_slot3(void);
extern void rt_timer_nmi_wrapper(void);

/*
 * Token Ring NIM (Single and Dual) Prototypes
 */
extern void trim_slot1a(void), trim_slot1b(void);
extern void trim_slot2a(void), trim_slot2b(void);
extern void trim_slot3a(void), trim_slot3b(void);
extern void trim_slot1a_jump_entry(void), trim_slot1b_jump_entry(void);
extern void trim_slot2a_jump_entry(void), trim_slot2b_jump_entry(void);
extern void trim_slot3a_jump_entry(void), trim_slot3b_jump_entry(void);

/*
 * MCN NIM based on Munich 32 prototypes
 */
extern void m32im_slot0(void), m32im_slot1(void), m32im_slot2(void);

/*
 * Serial NIM (Quad) Prototypes
 */
extern void qsim_slot1a(void), qsim_slot2a(void), qsim_slot0a(void);
extern void qsim_slot1b(void), qsim_slot2b(void), qsim_slot0b(void);
