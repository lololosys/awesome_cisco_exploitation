/* $Id: as5200_wrappers.h,v 1.1.50.1 1996/06/16 21:19:41 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/src-68-as5200/as5200_wrappers.h,v $
 *------------------------------------------------------------------
 * Assembly wrapper prototypes for Brasil.
 *
 * January 1996, Joe Turner
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: as5200_wrappers.h,v $
 * Revision 1.1.50.1  1996/06/16  21:19:41  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1996/01/09  23:08:41  jturner
 * Placeholder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Brasil assembly language level 4 interrupt wrapper.
 */
extern void level3_brasil(void);
extern void level4_brasil(void);
extern void level6_brasil(void);
extern void level4_hi_irq_dispatch(void);
