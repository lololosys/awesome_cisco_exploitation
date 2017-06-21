/* $Id: asm_as5200.h,v 3.1.62.1 1996/06/16 21:10:56 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/asm-m68k/asm_as5200.h,v $
 *------------------------------------------------------------------
 * Assembly definitions for as5200 assembly wrappers
 *
 * December 1995, Joe Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_as5200.h,v $
 * Revision 3.1.62.1  1996/06/16  21:10:56  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/20  05:55:17  jturner
 * Placeholder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../../boot/src-68-c3000/pcmap.h"
#include "../src-68-as5200/as5200_pcmap.h"

/*
 * Registers to save on interrupt
 */
#define REGISTERS	d0-d3/a0-a3

/*
 * Level 3 System Control Register 4 (SCR4) interrupt bit definition.
 */
#define SCR4_LEV3_CH_C_INT_BIT 10   /* int bit for serial ch c state change */
 
/*
 * Level 4 System Control Register 4 (SCR4) interrupt bit definitions.
 */
#define SCR4_LEV4_CH_A_INT_BIT 0   /* int bit for ch a state change */
#define SCR4_LEV4_CH_B_INT_BIT 1   /* int bit for ch b state change */
#define SCR4_LEV4_CH_C_INT_BIT 2   /* int bit for ch c state change */
#define SCR4_LEV4_CH_D_INT_BIT 3   /* int bit for ch d state change */
 
/*
 * Bits for Campeche interrupt wrappers
 */
#define CAMPECHE_ETHER_NET_C_BIT 4 /* Campeche Enet Ch C bit */
#define CAMPECHE_2E2T_MASK      0x0016
