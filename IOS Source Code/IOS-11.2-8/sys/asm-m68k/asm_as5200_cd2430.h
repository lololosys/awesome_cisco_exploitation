/* $Id: asm_as5200_cd2430.h,v 3.1.18.1 1996/06/16 21:10:58 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/asm-m68k/asm_as5200_cd2430.h,v $
 *------------------------------------------------------------------
 * asm_as5200_cd2430.h AS5200 Async Interrupt Handler Wrapper Defines.
 *
 * Feb 96, Eddy Lem
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_as5200_cd2430.h,v $
 * Revision 3.1.18.1  1996/06/16  21:10:58  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/02/20  19:22:11  elem
 * Place holder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../src-68-as5200/as5200_pcmap.h"

/*
 * Brasil Modem Board Addresses
 */

/* hard code these because assembler did not like complicated calculations */
#define ADRSPC_BRASIL_SLOT1_BASE	0x0213A000
#define ADRSPC_BRASIL_SLOT2_BASE	0x0213C000

/* Base offset of Modem Board Interrupt Autovector Register */
#define MDB_IAVRT_OFFSET	0x70
#define MDB_IAVRR_OFFSET	0x72
#define MDB_IAVRM_OFFSET	0x74

/* Modem Board Interrupt Autovector Register addresses */
/* for each slot, for each type (T-tx, R-rx, M-modem */
#define ADRSPC_BRASIL_SLOT0_MDB_IAVRT	\
			(ADRSPC_BRASIL_BASE_SLOT + MDB_IAVRT_OFFSET)
#define ADRSPC_BRASIL_SLOT0_MDB_IAVRR	\
			(ADRSPC_BRASIL_BASE_SLOT + MDB_IAVRR_OFFSET)
#define ADRSPC_BRASIL_SLOT0_MDB_IAVRM	\
			(ADRSPC_BRASIL_BASE_SLOT + MDB_IAVRM_OFFSET)

#define ADRSPC_BRASIL_SLOT1_MDB_IAVRT	\
			(ADRSPC_BRASIL_SLOT1_BASE + MDB_IAVRT_OFFSET)
#define ADRSPC_BRASIL_SLOT1_MDB_IAVRR	\
			(ADRSPC_BRASIL_SLOT1_BASE + MDB_IAVRR_OFFSET)
#define ADRSPC_BRASIL_SLOT1_MDB_IAVRM	\
			(ADRSPC_BRASIL_SLOT1_BASE + MDB_IAVRM_OFFSET)

#define ADRSPC_BRASIL_SLOT2_MDB_IAVRT	\
			(ADRSPC_BRASIL_SLOT2_BASE + MDB_IAVRT_OFFSET)
#define ADRSPC_BRASIL_SLOT2_MDB_IAVRR	\
			(ADRSPC_BRASIL_SLOT2_BASE + MDB_IAVRR_OFFSET)
#define ADRSPC_BRASIL_SLOT2_MDB_IAVRM	\
			(ADRSPC_BRASIL_SLOT2_BASE + MDB_IAVRM_OFFSET)

/*
 * Interrupt Enable Bit Mask
 */
#define BRASIL_ASYNC_INTR_ENABLE	0x000E /* tx+rx+modem */

/*
 * Interrupt Status Bit Masks
 */
#define BRASIL_TXINT_SLOT0_BIT		4
#define BRASIL_TXINT_SLOT1_BIT		5
#define BRASIL_TXINT_SLOT2_BIT		6

#define BRASIL_RXEXCEP_SLOT0_BIT	0
#define BRASIL_RXEXCEP_SLOT1_BIT	1
#define BRASIL_RXEXCEP_SLOT2_BIT	2

#define BRASIL_MODINT_SLOT0_BIT		3
#define BRASIL_MODINT_SLOT1_BIT		4
#define BRASIL_MODINT_SLOT2_BIT		5

#define BRASIL_SRAM_BASED_CD24XX	0x0100
