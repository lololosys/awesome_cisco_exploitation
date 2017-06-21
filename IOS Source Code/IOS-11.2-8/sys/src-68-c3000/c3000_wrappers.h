/* $Id: c3000_wrappers.h,v 3.2.62.4 1996/06/16 21:19:49 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c3000/c3000_wrappers.h,v $
 *------------------------------------------------------------------
 *
 * Assembly wrapper prototypes for c3000
 *
 * April 1993, Dit Morse
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c3000_wrappers.h,v $
 * Revision 3.2.62.4  1996/06/16  21:19:49  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2.62.3  1996/06/13  19:09:06  etrehus
 * Commit boa to California Branch
 *
 * Revision 3.2.62.2  1996/04/19  20:24:08  etrehus
 * CSCdi51928:  3000 restarts after SCHED-2-WATCH error
 * Added temporary level 3 interrupt handler if mk5025 is present during
 * boot sequence to work around hardware shortcommings, cause the interrupt
 * cannot be disabled at the source.
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  21:59:15  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/09  05:12:19  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.1  1996/02/20  17:56:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:43:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:25:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:54:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Assembly language level 4 interrupt wrappers:
 */

extern void level4_cristal(void),      level4_brut(void), level4_pan(void); 
extern void level4_campeche_2r2t(void);
extern void level4_campeche_2e2t(void);
extern void level4_campeche_1r2t1e(void);
extern void level4_pan_e0(void),       level4_pan_e1(void);
extern void level4_pan_t0(void),       level4_pan_t1(void);
extern void level4_cristal_tnil(void),  level4_cristal_tt(void);
extern void level4_cristal_nilt(void),  level4_cristal_nilthd(void);
extern void level4_cristal_thdnil(void),level4_cristal_thdthd(void);
extern void level4_cristal_thdt(void), level4_cristal_tthd(void);
extern void level4_pan_r(void),        level4_pan_b(void); 
extern void level4_nop(void);
extern void level6_nop(void);
extern void level3_cristal(void),      level3_pan(void); 
extern void level3_boa_csu_dsu_int(void); 
extern void level2_hub(void);

/*
 * wrappers added to support bisynch
 */
extern void level4_cristal_bscnil(void), level4_cristal_nilbsc(void);
extern void level4_cristal_bsct(void),   level4_cristal_tbsc(void);
extern void level4_cristal_bscthd(void), level4_cristal_thdbsc(void);
extern void level4_cristal_bscbsc(void);


extern void no_mk5025_exception_handler(void);
extern void no_mk5025_exception_handler_2(void);
