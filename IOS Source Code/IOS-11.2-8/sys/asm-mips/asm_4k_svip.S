/* $Id: asm_4k_svip.S,v 3.2.58.8 1996/08/21 06:17:40 drowell Exp $
 * $Source: /release/112/cvs/Xsys/asm-mips/asm_4k_svip.S,v $
 *------------------------------------------------------------------
 * asm_4k_svip.S : Assembly routines for VIPER
 *
 * September 1995, David Getchell
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: asm_4k_svip.S,v $
 * Revision 3.2.58.8  1996/08/21  06:17:40  drowell
 * CSCdi65683:  GDB on VIP is broke
 * Branch: California_branch
 * Made software interrupt 0s call the asm_4k.S spurious interrupt handler
 * to make sure that GDB gets initialized correctly.
 *
 * Revision 3.2.58.7  1996/08/08  21:06:16  mbeesley
 * CSCdi65492:  r4k_cpu_level format can be more efficient. Convert it
 * to a format where bits [5..3] indicate the interrupt level.
 * Branch: California_branch
 *
 * Revision 3.2.58.6  1996/07/10  02:37:19  getchell
 * Added crash context mechanism which is shown via diag command and test
 * cbus (vc command)
 * CSCdi60415:  VIP difficult to debug in the field
 * Branch: California_branch
 *
 * Revision 3.2.58.5  1996/06/07  05:10:34  getchell
 * CSCdi59206:  EMT_RELOAD, EMT_CPU_EXCEPTION does call svip_reload()
 * Branch: California_branch
 *
 * Revision 3.2.58.4  1996/06/03  18:04:29  getchell
 * CSCdi52344:  scheduler allocate needed for debug under load
 * Branch: California_branch
 *
 * Revision 3.2.58.3  1996/05/18  17:53:22  kao
 * CSCdi54117:  vip2 looses it with serial encaps other than HDLC
 * Fix spurious interrupt conditions; add protection not to corrupt stack
 * when spurious interrupt happens on top of exception handler
 * Branch: California_branch
 *
 * Revision 3.2.58.2  1996/05/08  02:21:18  getchell
 * Remove posted read of malu-attention command.
 * CSCdi56421:  tftpboot via EIP fails w/ rsp-boot-mz.960501
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/03/21  22:27:19  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/02/16  03:07:10  getchell
 * CSCdi49146:  optimize svip wrappers for amdp2 and dec21140
 * Changed how dtq and drq were timesharing with the wrappered interfaces.
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1996/02/10  00:25:31  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  08:40:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:06:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "asm_4k_svip.h"
#include "asm_4k_vip.h"
#include "../../boot/emt.h"

/*
 * Include memd support files
 */
#include "../hes/ccb_cmd.h"
#include "../src-vip/vip_memd.h"

/*
 * interrupt_jump_table:
 * Used by the interrupt handler to jump to the appropriate routine.
 * The prioritization of interrupts is done by this table.
 * Note this table is in the .text segment, hence read only.
 *
 * 	CPU Line	Priority	Description
 *	------------------------------------------------------------
 *	INT0		3		GDB Debugging (software int)
 *	INT1				unused (software int)
 *
 *	INT2		1		PA 1 packet
 *	INT3		1		PA 2 packet
 *	INT4		1		DMA Engine
 *	INT5		2		PA management
 *	INT6		3,5,6,7		External Interrupt
 *	INT7		4		Timer
 */
	.text
	.align 2

	.globl	interrupt_jump_table
	.globl	svip_cbus_intr

 #
 # void ((*interrupt_jump_table)())[64]
 #
interrupt_jump_table:			/*	       765432		*/
	.word	svip_sw_intr		/* IP [7..2] = 000000, S/W Int	*/
	.word	svip_pa_lo_intr		/* IP [7..2] = 000001, PA1	*/
	.word	svip_pa_lo_intr		/* IP [7..2] = 000010, PA2	*/ 
	.word	svip_pa_lo_intr		/* IP [7..2] = 000011, PA1,PA2	*/
	.word	svip_pa_lo_intr		/* IP [7..2] = 000100, DMA 	*/
	.word	svip_pa_lo_intr		/* IP [7..2] = 000101, DMA 	*/
	.word	svip_pa_lo_intr		/* IP [7..2] = 000110, DMA	*/ 
	.word	svip_pa_lo_intr		/* IP [7..2] = 000111, DMA	*/
	.word	svip_pa_nmi_intr	/* IP [7..2] = 001000, PA mang	*/
	.word	svip_pa_nmi_intr	/* IP [7..2] = 001001, PA mang	*/
	.word	svip_pa_nmi_intr	/* IP [7..2] = 001010, PA mang	*/
	.word	svip_pa_nmi_intr	/* IP [7..2] = 001011, PA mang	*/
	.word	svip_pa_nmi_intr	/* IP [7..2] = 001100, PA mang	*/
	.word	svip_pa_nmi_intr	/* IP [7..2] = 001101, PA mang	*/
	.word	svip_pa_nmi_intr	/* IP [7..2] = 001110, PA mang	*/
	.word	svip_pa_nmi_intr	/* IP [7..2] = 001111, PA mang	*/
	.word	svip_ext_intr		/* IP [7..2] = 010000, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 010001, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 010010, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 010011, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 010100, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 010101, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 010110, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 010111, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 011000, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 011001, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 011010, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 011011, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 011100, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 011101, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 011110, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 011111, External	*/

	.word	r4k_nmi_intr		/* IP [7..2] = 100000, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 100001, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 100010, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 100011, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 100100, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 100101, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 100110, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 100111, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 101000, Cbus	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 101001, Cbus	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 101010, Cbus	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 101011, Cbus	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 101100, Cbus	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 101101, Cbus	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 101110, Cbus	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 101111, Cbus	*/
	.word	svip_ext_intr		/* IP [7..2] = 110000, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 110001, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 110010, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 110011, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 110100, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 110101, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 110110, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 110111, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 111000, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 111001, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 111010, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 111011, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 111100, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 111101, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 111110, External	*/
	.word	svip_ext_intr		/* IP [7..2] = 111111, External	*/

/*
 * Table driving the external interrupt controller (Nevada equivalent)
 *
 * 	Intrpt Stat Reg 	Priority	Description
 *	----------------------------------------------------------
 *	Bit 4			7		Dbus Interrupt
 *	Bit 5			5		Cbus Interrupt
 *	Bit 6			3		External Timer
 *	Bit 7			6		Cbus Stall
 *      Bit 10                  3               Bus Errors
 */

	.text
	.align 2

svip_ext_jump_table:
                                        /*             a987654            */
	.word	svip_errs_intr		/* SR [7..4] = 0000000, Bus Err	*/

	.word	svip_dbus_intr		/* SR [7..4] = 0000001, Dbus	*/

	.word   svip_cbus_intr		/* SR [7..4] = 0000010, Cbus	*/
	.word	svip_dbus_intr		/* SR [7..4] = 0000011, Dbus	*/

	.word	svip_ext_timer_intr	/* SR [7..4] = 0000100, Ext Timer */
	.word	svip_dbus_intr		/* SR [7..4] = 0000101, Dbus	*/
	.word	svip_cbus_intr		/* SR [7..4] = 0000110, Cbus	*/
	.word	svip_dbus_intr		/* SR [7..4] = 0000111, Dbus	*/

	.word	svip_cbus_stall		/* SR [7..4] = 0001000, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 0001001, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 0001010, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 0001011, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 0001100, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 0001101, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 0001110, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 0001111, Dbus       */

	.word	svip_errs_intr		/* SR [7..4] = 0010000, Bus Err	*/
	.word	svip_dbus_intr		/* SR [7..4] = 0010001, Dbus	*/
	.word   svip_cbus_intr		/* SR [7..4] = 0010010, Cbus	*/
	.word	svip_dbus_intr		/* SR [7..4] = 0010011, Dbus       */
	.word	svip_ext_timer_intr	/* SR [7..4] = 0010100, Ext Timer */
	.word	svip_dbus_intr		/* SR [7..4] = 0010101, Dbus	*/
	.word	svip_cbus_intr		/* SR [7..4] = 0010110, Cbus	*/
	.word	svip_dbus_intr		/* SR [7..4] = 0010111, Dbus       */

	.word	svip_cbus_stall		/* SR [7..4] = 0011000, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 0011001, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 0011010, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 0011011, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 0011100, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 0011101, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 0011110, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 0011111, Dbus       */

	.word	svip_errs_intr		/* SR [7..4] = 0100000, Bus Err	*/
	.word	svip_dbus_intr		/* SR [7..4] = 0100001, Dbus	*/
	.word   svip_cbus_intr		/* SR [7..4] = 0100010, Cbus	*/
	.word	svip_dbus_intr		/* SR [7..4] = 0100011, Dbus	*/
	.word	svip_ext_timer_intr	/* SR [7..4] = 0100100, Ext Timer */
	.word	svip_dbus_intr		/* SR [7..4] = 0100101, Dbus	*/
	.word	svip_cbus_intr		/* SR [7..4] = 0100110, Cbus	*/
	.word	svip_dbus_intr		/* SR [7..4] = 0100111, Dbus	*/

	.word	svip_cbus_stall		/* SR [7..4] = 0101000, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 0101001, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 0101010, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 0101011, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 0101100, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 0101101, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 0101110, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 0101111, Dbus       */

	.word	svip_errs_intr		/* SR [7..4] = 0110000, Bus Err	*/
	.word	svip_dbus_intr		/* SR [7..4] = 0110001, Dbus	*/
	.word   svip_cbus_intr		/* SR [7..4] = 0110010, Cbus	*/
	.word	svip_dbus_intr		/* SR [7..4] = 0110011, Dbus	*/
	.word	svip_ext_timer_intr	/* SR [7..4] = 0110100, Ext Timer */
	.word	svip_dbus_intr		/* SR [7..4] = 0110101, Dbus	*/
	.word	svip_cbus_intr		/* SR [7..4] = 0110110, Cbus	*/
	.word	svip_dbus_intr		/* SR [7..4] = 0110111, Dbus	*/

	.word	svip_cbus_stall		/* SR [7..4] = 0111000, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 0111001, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 0111010, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 0111011, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 0111100, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 0111101, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 0111110, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 0111111, Dbus       */

	.word	svip_errs_intr		/* SR [7..4] = 1000000, Bus Err	*/
	.word	svip_dbus_intr		/* SR [7..4] = 1000001, Dbus	*/
	.word   svip_cbus_intr		/* SR [7..4] = 1000010, Cbus	*/
	.word	svip_dbus_intr		/* SR [7..4] = 1000011, Dbus	*/
	.word	svip_ext_timer_intr	/* SR [7..4] = 1000100, Ext Timer */
	.word	svip_dbus_intr		/* SR [7..4] = 1000101, Dbus	*/
	.word	svip_cbus_intr		/* SR [7..4] = 1000110, Cbus	*/
	.word	svip_dbus_intr		/* SR [7..4] = 1000111, Dbus	*/

	.word	svip_cbus_stall		/* SR [7..4] = 1001000, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 1001001, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 1001010, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 1001011, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 1001100, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 1001101, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 1001110, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 1001111, Dbus       */

	.word	svip_errs_intr		/* SR [7..4] = 1010000, Bus Err	*/
	.word	svip_dbus_intr		/* SR [7..4] = 1010001, Dbus	*/
	.word   svip_cbus_intr		/* SR [7..4] = 1010010, Cbus	*/
	.word	svip_dbus_intr		/* SR [7..4] = 1010011, Dbus	*/
	.word	svip_ext_timer_intr	/* SR [7..4] = 1010100, Ext Timer */
	.word	svip_dbus_intr		/* SR [7..4] = 1010101, Dbus	*/
	.word	svip_cbus_intr		/* SR [7..4] = 1010110, Cbus	*/
	.word	svip_dbus_intr		/* SR [7..4] = 1010111, Dbus	*/

	.word	svip_cbus_stall		/* SR [7..4] = 1011000, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 1011001, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 1011010, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 1011011, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 1011100, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 1011101, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 1011110, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 1011111, Dbus       */

	.word	svip_errs_intr		/* SR [7..4] = 1100000, Bus Err	*/
	.word	svip_dbus_intr		/* SR [7..4] = 1100001, Dbus	*/
	.word   svip_cbus_intr		/* SR [7..4] = 1100010, Cbus	*/
	.word	svip_dbus_intr		/* SR [7..4] = 1100011, Dbus	*/
	.word	svip_ext_timer_intr	/* SR [7..4] = 1100100, Ext Timer */
	.word	svip_dbus_intr		/* SR [7..4] = 1100101, Dbus	*/
	.word	svip_cbus_intr		/* SR [7..4] = 1100110, Cbus	*/
	.word	svip_dbus_intr		/* SR [7..4] = 1100111, Dbus	*/

	.word	svip_cbus_stall		/* SR [7..4] = 1101000, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 1101001, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 1101010, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 1101011, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 1101100, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 1101101, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 1101110, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 1101111, Dbus       */

	.word	svip_errs_intr		/* SR [7..4] = 1110000, Bus Err	*/
	.word	svip_dbus_intr		/* SR [7..4] = 1110001, Dbus	*/
	.word   svip_cbus_intr		/* SR [7..4] = 1110010, Cbus	*/
	.word	svip_dbus_intr		/* SR [7..4] = 1110011, Dbus	*/
	.word	svip_ext_timer_intr	/* SR [7..4] = 1110100, Ext Timer */
	.word	svip_dbus_intr		/* SR [7..4] = 1110101, Dbus	*/
	.word	svip_cbus_intr		/* SR [7..4] = 1110110, Cbus	*/
	.word	svip_dbus_intr		/* SR [7..4] = 1110111, Dbus	*/

	.word	svip_cbus_stall		/* SR [7..4] = 1111000, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 1111001, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 1111010, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 1111011, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 1111100, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 1111101, Dbus       */
	.word	svip_cbus_stall		/* SR [7..4] = 1111110, Cbus_Stall */
	.word	svip_dbus_intr		/* SR [7..4] = 1111111, Dbus       */

/*
 * Some externals to allow gas to do gp addressing
 */
	.extern r4k_cpu_level,4
	.extern int_count,4
	.extern schedflag,4
	.extern current_count,4
	.extern profile_enabled,4
        .extern spurcnt, 4

/*
 * Some macros to trim down the file
 */
#define SAVE_T0_T3				\
	sd	t0,SP_OFS(REG_T0)(sp);		\
	sd	t1,SP_OFS(REG_T1)(sp);		\
	sd	t2,SP_OFS(REG_T2)(sp);		\
	sd	t3,SP_OFS(REG_T3)(sp);

#define SAVE_VOLATILE_REGS			\
	sdc1	FPU_R0,SP_OFS(REG_K0)(sp);	\
	sdc1	FPU_R1,SP_OFS(REG_K1)(sp);	\
	sdc1	FPU_R2,SP_OFS(REG_AT)(sp);	\
	dmfc0	t0,CP0_EPC;			\
	mfc0	t1,CP0_SREG;			\
	mflo	t2;				\
	mfhi	t3;				\
	sd	t0,SP_OFS(REG_EPC)(sp);		\
	sw	t1,SP_OFS(REG_SREG)(sp);	\
	sd	t2,SP_OFS(REG_LO)(sp);		\
	sd	t3,SP_OFS(REG_HI)(sp);

#define SAVE_V0_A3				\
	sd	v0,SP_OFS(REG_V0)(sp);		\
	sd	v1,SP_OFS(REG_V1)(sp);		\
	sd 	a0,SP_OFS(REG_A0)(sp);		\
	sd 	a1,SP_OFS(REG_A1)(sp);		\
	sd 	a2,SP_OFS(REG_A2)(sp);		\
	sd 	a3,SP_OFS(REG_A3)(sp);

#define SAVE_T4_RA				\
	sd	t4,SP_OFS(REG_T4)(sp);		\
	sd	t5,SP_OFS(REG_T5)(sp);		\
	sd	t6,SP_OFS(REG_T6)(sp);		\
	sd	t7,SP_OFS(REG_T7)(sp);		\
	sd	t8,SP_OFS(REG_T8)(sp);		\
	sd	ra,SP_OFS(REG_RA)(sp);		\
	sd	t9,SP_OFS(REG_T9)(sp);

#define SAVE_S0_GP                              \
        sd      s0,SP_OFS(REG_S0)(sp);          \
        sd      s1,SP_OFS(REG_S1)(sp);          \
        sd      s2,SP_OFS(REG_S2)(sp);          \
        sd      s3,SP_OFS(REG_S3)(sp);          \
        sd      s4,SP_OFS(REG_S4)(sp);          \
        sd      s5,SP_OFS(REG_S5)(sp);          \
        sd      s6,SP_OFS(REG_S6)(sp);          \
        sd      s7,SP_OFS(REG_S7)(sp);          \
        sd      s8,SP_OFS(REG_S8)(sp);          \
        sd      zero,SP_OFS(REG_ZERO)(sp);      \
        sd      gp,SP_OFS(REG_GP)(sp);           
 

#define RESTORE_V0_A3				\
	ld	v0,SP_OFS(REG_V0)(sp);		\
	ld	v1,SP_OFS(REG_V1)(sp);		\
	ld 	a0,SP_OFS(REG_A0)(sp);		\
	ld 	a1,SP_OFS(REG_A1)(sp);		\
	ld 	a2,SP_OFS(REG_A2)(sp);		\
	ld 	a3,SP_OFS(REG_A3)(sp);

#define RESTORE_T1_T3_EPC_LO_HI			\
	ld	t1,SP_OFS(REG_EPC)(sp);		\
	ld	t2,SP_OFS(REG_LO)(sp);		\
	ld	t3,SP_OFS(REG_HI)(sp);		\
	dmtc0	t1,CP0_EPC;			\
	mtlo	t2;				\
	mthi	t3;                             \
	ld	t1,SP_OFS(REG_T1)(sp);		\
	ld	t2,SP_OFS(REG_T2)(sp);		\
	ld	t3,SP_OFS(REG_T3)(sp);	

#define RESTORE_T4_RA				\
	ld	t4,SP_OFS(REG_T4)(sp);		\
	ld	t5,SP_OFS(REG_T5)(sp);		\
	ld	t6,SP_OFS(REG_T6)(sp);		\
	ld	t7,SP_OFS(REG_T7)(sp);		\
	ld	t8,SP_OFS(REG_T8)(sp);		\
	ld	t9,SP_OFS(REG_T9)(sp);		\
	ld	ra,SP_OFS(REG_RA)(sp);

#define RESTORE_K0_K1_AT			\
	ld	k1,SP_OFS(REG_K1)(sp);		\
	ld	k0,SP_OFS(REG_K0)(sp);		\
	ld	AT,SP_OFS(REG_AT)(sp);

	.text
	.align 2

/*
 * svip_sw_intr:
 * Handle a software or spurious interrupt
 *      - k0 has copy of cause register
 */
 #
 # void svip_sw_intr (void)
 #
FRAME(svip_sw_intr, sp, 0, ra)
        .set    noreorder
        .set    noat
 
        /*
         * This routine checks to see if sw 1 is asserted. If so
         * it calls the network IO interrupt handling code after
         * de-asserting sw 1. If a sw 0 interrupt is asserted,
	 * call the spurious interrupt handling code in asm_4k.S.
	 * This will take care of making sure GDB sees the sw 0.
	 * Otherwise it's a spurious interrupt so count it and
	 * return.
         * Note:  sp has already been copied to AT
         */
        mfc0    k1,CP0_SREG                     # re-read status register
        nop                                     # load delay
 
        andi    k1,k1,CAUSE_IMASK               # get interrupt enable bits
        and     k1,k1,k0                        # get asserted interrupt bits
 
        andi    k1,k1,CAUSE_IBIT1               # mask sw interrupt 1
        bne     k1,zero,svip_sw1_intr           # is it sw 1 ?
        nop                                     # (bd slot)
 
        mfc0    k1,CP0_SREG                     # re-read status register
        nop                                     # load delay
 
        andi    k1,k1,CAUSE_IMASK               # get interrupt enable bits
        and     k1,k1,k0                        # get asserted interrupt bits
 
        andi    k1,k1,CAUSE_IBIT0               # mask sw interrupt 0
        beq     k1,zero,svip_spur_intr          # is it a spurious interrupt ?
        nop                                     # (bd slot)
 
	mfc0	k1,CP0_SREG			# reload k1 with status reg.
	j	r4k_spurious_callback		# call spurious handler
	nop					# (bd slot)
 
svip_spur_intr:
        lw      k1,spurcnt              # k1 = spurcnt
        addiu   k1,k1,1                 # increment spurious count
        sw      k1,spurcnt              # spurcnt = k1

        dmfc1   k0,FPU_R0                       # restore k0
        dmfc1   k1,FPU_R1                       # restore k1
        dmfc1   AT,FPU_R2                       # restore AT

        /*
         * Dismiss the interrupt
         */
        eret                                    # return
        nop                                     # not executed
 
svip_sw1_intr:
        j       svip_pa_lo_intr                 # call net io handler
        nop                                     # (nd slot)

	.set reorder
	.set at
ENDFRAME(svip_sw_intr)

/*
 * svip_ext_intr :
 * Handle External (Nevada equivalent) interrupts
 *
 * Entry:      k0 is saved in FPU_R0
 *             k1 is saved in FPU_R1
 *             AT is saved in FPU_R2
 *             sp is saved in AT
 *             k1 is zero
 */
 #
 # void svip_ext_intr (void)
 #
FRAME(svip_ext_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * Read external interrupt controller to determine cause
	 */
	lhu	k0,IO_NEV_ISR_REG	# read the "normal" cause bits
	la	k1,svip_ext_jump_table	# get address of jump table
	andi	k0,k0,SVIP_EXT_INT_MASK	# isolate interrupt values
	srl	k0,k0,SVIP_EXT_INT_SHIFT # Form table index

	addu	k1,k1,k0		# get address of table entry
	lw	k1,0(k1)		# get handler address from table

	j	k1			# call the handler
	addiu	k1,zero,1		# set k1 to 1 (bd slot)

	.set	reorder
	.set	at
ENDFRAME(svip_ext_intr)

/*
 * svip_intr_dispatch:
 * Dispatch an interrupt handler for interrupts handled thru the
 * external interrupt controller. Save remaining context and call the 
 * handler. Re-install the old level, the context and dismiss the 
 * interrupt handler. This is a general purpose dispatch for interrupts 
 * serviced through the external interrupt controller.
 *
 * Note:  t0-t3 have already been saved.
 *	  Stack has been set up with a frame and old sp is stacked
 * 	  Old level is in k0.
 *	  External interrupt controller has been handled
 *	  EXL Bit is still asserted
 *	  AT has address of wrapper/driver routine
 *        k1 has sreg mask value	
 */
 #
 # void svip_intr_dispatch (void)
 #
FRAME(svip_intr_dispatch,sp,0,ra)
	.set	noreorder
	.set	noat

	sdc1	FPU_R0,SP_OFS(REG_K0)(sp)
	sdc1	FPU_R1,SP_OFS(REG_K1)(sp)
	sdc1	FPU_R2,SP_OFS(REG_AT)(sp)
	dmfc0	t0,CP0_EPC
	mfc0	t1,CP0_SREG
	mflo	t2
	mfhi	t3
	sd	t0,SP_OFS(REG_EPC)(sp)
	sw	t1,SP_OFS(REG_SREG)(sp)
	sd	t2,SP_OFS(REG_LO)(sp)
	sd	t3,SP_OFS(REG_HI)(sp)

	dmfc0   t0,CP0_ERR_EPC                  # get Error EPC
        mfc0    t3,CP0_CAUSE                    # get Cause register
        dmfc0   t2,CP0_BADVADDR                 # get Bad Vaddr
	sd      t0,SP_OFS(REG_ERR_EPC)(sp)      # save Error EPC
        sw      t3,SP_OFS(REG_CAUSE)(sp)        # save Cause
        sd      t2,SP_OFS(REG_BADVADDR)(sp)     # save Bad Vaddr
 

	/*
	 * Re-enable interrupts so Malu Attention does not wait too long.
	 * We want to mask out PA IO, DMA, SW1
	 *
	 * Note:  There seems to be a problem with the processor when EXL 
	 * gets de-asserted while IE is asserted and there is another
	 * interrupt pending (the interrupt is taken but the EPC register
	 * does not get re-latched). To work arround this, disable IE,
	 * reset the EXL bit, and then re-enable IE.
	 */
	li	t3,~SREG_IE			# get IE mask
	and	t3,t3,t1			# de-assert IE
	mtc0	t3,CP0_SREG			# move to status register

	and	t3,t3,k1			# and sreg mask into sreg copy
	mtc0	t3,CP0_SREG			# clear EXL (IE disabled)

	/*
	 * We can end up in an interrupt handler with IE off if the
	 * interrupt occurs at the next instruction after a move to the
	 * status register that turns IE off. So we turn it back on.
	 */
	and	t1,t1,k1			# and it into sreg
	ori	t1,t1,SREG_IE			# set IE
	mtc0	t1,CP0_SREG			# write back sreg with EXL=0

	/*
	 * Save the remaining context
	 */
	SAVE_V0_A3				/* save v0,v1,a0..a3 */
	SAVE_T4_RA				/* save t4..t9,ra */
	SAVE_S0_GP                              /* save s0-s8,zero,gp */
	jal	AT				# call handler/driver
	move	a0,sp				# set up param 1. This gives 
						# handlers a context ptr

	/*
	 * restore the context. We restore the stacked status register
	 * which guarantees that EXL will be asserted through critical 
	 * section
	 *
	 * Note: The R4400 processor does not like s/w to change
	 * the state of EXL with IE enabled, so disable it during change.
	 */
	RESTORE_V0_A3				/* restore v0,v1,a0..a3 */

	lw	t5,SP_OFS(REG_SREG)(sp)		# load Status reg
	ld	t0,SP_OFS(REG_T0)(sp)		# restore t0

	mfc0	t6,CP0_SREG			# read sreg
	li	t4,~SREG_IE			# get IE mask

	and	t6,t6,t4			# disable IE bit
	mtc0	t6,CP0_SREG			# re-write sreg (EXL=0,IE=0)

	and	t7,t5,t4			# disable IE in sreg copy
	mtc0	t7,CP0_SREG			# re-write sreg (EXL=1,IE=0)
	nop					# let sreg settle

	mtc0	t5,CP0_SREG			# restore sreg (EXL=1)
	nop					# let sreg settle

	RESTORE_T1_T3_EPC_LO_HI			/* restore T1..T3,EPC,Lo,Hi */

	/*
	 * We need to restore the interrupted level (both the
	 * status register and the external mask)
	 */
	sw	k0,r4k_cpu_level		# restore old level
	la	k1,r4k_level_table		# get table address
	addu	k1,k1,k0			# get address
	lbu	k0,4(k1)			# read nevada intr mask
	la	t9,IO_ASIC_BASE			# get nevada base addr
	sb	k0,IO_IMASK_OFFSET(t9)		# write nevada int. mask

	lbu	t8,IO_IMASK_OFFSET(t9)		# sync the write

	/*
	 * Restore remaining context
	 */
	RESTORE_T4_RA				/* restore t4..t9, ra */
	RESTORE_K0_K1_AT			/* restore k0, k1, AT */

	/*
	 * move back onto the original stack
	 * We dont have to increment the stack pointer
	 */
	ld	sp,SP_OFS(REG_SP)(sp)		# restore old sp

	/*
	 * dismiss the interrupt
	 */
	eret					# return
	nop					# does not get executed

	.set	reorder
	.set	noat
ENDFRAME(svip_intr_dispatch)

/*
 * svip_cbus_stall:
 * Handle a Level 6 Cbus Stall Interrupt 
 * Entry:      k0 is saved in FPU_R0
 *             k1 is saved in FPU_R1
 *             AT is saved in FPU_R2
 *             sp is saved in AT
 *             k1 is one        FIXME - needed?
 */
 #
 # void svip_cbus_stall (void)
 #
	.extern l6stk,4
	.extern l6cnt,4
	.extern lev6subr,4

FRAME(svip_cbus_stall,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 7 stack
	 * It dereferences off lev7subr to the wrapper/driver
	 */
	lw	sp,l6stk			# move onto level 7 stack
	lw	k0,l6cnt			# load counter

	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame on new stack
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,l6cnt			# save the counter

	SAVE_T0_T3				/* save t0 .. t3 */

	/*
	 * Save the interrupted level and install new level
	 */
	lw	k0,r4k_cpu_level		# save old level
	li	t1,(LEVEL6 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t1,r4k_cpu_level		# save new level

	la	t2,r4k_level_table		# get level table address
	lbu	t1,(4+(8*LEVEL6))(t2)		# read new mask value
	la	t0,IO_ASIC_BASE			# get nevada base addr
	sb	t1,IO_IMASK_OFFSET(t0)		# write nevada int. mask
	lbu	t1,IO_IMASK_OFFSET(t0)		# sync the write

	/*
	 * Load lev6subr into AT and call the dispatch routine
	 */
	li	k1,LEVEL6_SREG_MASK             # load k1 with sp
	j	svip_intr_dispatch		# call dispatcher
	lw	AT,lev6subr			# get handler (bd slot)

	.set	reorder
	.set	at
ENDFRAME(svip_cbus_stall)

/*
 * svip_cbus_intr:
 * Handle a Level 5 Cbus Interrupt 
 * Entry:      k0 is saved in FPU_R0
 *             k1 is saved in FPU_R1
 *             AT is saved in FPU_R2
 *             sp is saved in AT
 *             k1 is one        FIXME - needed?
 *
 * Note that this routine does not use svip_intr_dispatch as it does
 * not save or restore all of the registers
 *
 */
 #
 # void svip_cbus_intr (void)
 #
	.extern l5stk,4
	.extern l5cnt,4
	.extern lev5subr,4

FRAME(svip_cbus_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This interrupt handler runs on the level5 stack
	 */
	lw	sp,l5stk			# move onto level 5 stack
	lw	k0,l5cnt			# load counter

	/*
	 * Make a frame. Note: We deliberately make a frame 
	 * that is at least 32 bytes bigger then we need so
	 * that the create dirty exclusive does not wipe out
	 * other valuable information.
	 */
	addiu	sp,sp,-VIP_CBUS_STACK_SIZE	# make a frame

	/*
	 * Create cache lines - saves cache line fills
	 */
	cache	3*4+1,64(sp)			# avoid loading cache line
	cache	3*4+1,32(sp)			# avoid loading cache line
	cache 	3*4+1,0(sp)			# avoid loading cache line

	addiu	k0,k0,1				# increment counter
	sw	k0,l5cnt			# save the counter

	/*
	 * Save sp, t0..t3 as working registers
	 */
	sd	AT,PUSH_SP_OFS(sp)		# save old sp
	sdc1	FPU_R0,PUSH_K0_OFS(sp)		# save k0 copy
	sdc1	FPU_R1,PUSH_K1_OFS(sp)		# save k1 copy
	sdc1	FPU_R2,PUSH_AT_OFS(sp)		# save AT copy
	sd	t0,PUSH_T0_OFS(sp)		# save t0
	sd	t1,PUSH_T1_OFS(sp)		# save t1
	sd	t2,PUSH_T2_OFS(sp)		# save t2
	sd	t3,PUSH_T3_OFS(sp)		# save t3
	sd	t4,PUSH_T4_OFS(sp)		# save t4

	/*
	 * Save volatile registers (SREG, EPC)
	 */
	sd	ra,PUSH_RA_OFS(sp)		# save ra
	dmfc0	t0,CP0_EPC			# get EPC register
	mfc0	t1,CP0_SREG			# get status register
	sd	t0,PUSH_EPC_OFS(sp)		# save EPC
	sw	t1,PUSH_SREG_OFS(sp)		# save SREG

	/*
	 * Save the interrupted level and install new level
	 */
	lw	k0,r4k_cpu_level		# save old level
	li	t4,(LEVEL5 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t4,r4k_cpu_level		# save new level

	la	t0,r4k_level_table		# get level table address
	lbu	t4,(4+(8*LEVEL5))(t0)		# read new mask value
	la	t0,IO_ASIC_BASE			# get nevada base addr
	sb	t4,IO_IMASK_OFFSET(t0)		# write nevada int. mask

	lbu	t4,IO_IMASK_OFFSET(t0)		# flush reno

	/*
	 * Re-enable exceptions (to handle cbus stall)
	 */
	li	t3,~SREG_IE			# get IE mask
	and	t3,t3,t1			# de-assert IE
	mtc0	t3,CP0_SREG			# write to SREG

	li	t2,LEVEL5_SREG_MASK		# get SREG mask for level 6
	and	t3,t3,t2			# clear IE and EXL
	mtc0	t3,CP0_SREG			# set SREG

	/*
	 * We can end up in an interrupt handler with IE off if the
	 * interrupt occurs at the next instruction after a move to the
	 * status register that turns IE off. So we turn it back on.
	 */
	and	t1,t1,t2			# clear IE and EXL
	ori	t1,t1,SREG_IE			# enable IE
	mtc0	t1,CP0_SREG			# write status register
	nop					# let it settle

	la	k1,memd_cntl			# get address of memd_cntl
	lw	t4,MEMD_CNTL_CCB(k1)		# read ccbptr

	/* Clear the malu-attention interrupt */
	la	t1,CYA_OIC_REG			# address of clear register
	ori	t0,zero,CYA_MALU_ATTN	        # MALU_ATTN mask
	sw	t0,0(t1)			# Clear the interrupt

	#
	# This code had to be moved below the malu-attention code.
	# t0 is a parameter to vip_cbus_intr, and it was being
	# overwritten by 0x1 from CYA_MALU_ATTN.
	#
	# The lw could be moved into the delay slot of the jal.
	#
	# Could save cycles by referencing CYA_OIC_REG
	# as offset from the CYA base address.
	#
	lw	t0,CCB_CMDARG0(t4)		# read cmd/arg0

	jal	vip_cbus_intr
	nop

	/*
	 * Restore the context.  We restore the stacked status register
	 * which guarantees that EXL will be asserted through critical 
	 * section.
	 *
	 * Note:  The R4400 processor does not like s/w to change
	 * the state of EXL with IE enabled, so disable it during change.
	 */
	lw	t1,PUSH_SREG_OFS(sp)		# load Status reg
	ld	t0,PUSH_EPC_OFS(sp)		# load EPC

	mfc0	t2,CP0_SREG			# read sreg
	li	t3,~SREG_IE			# get IE mask

	and	t2,t2,t3			# disable IE bit
	mtc0	t2,CP0_SREG			# re-write sreg (EXL=0,IE=0)
	nop

	and	t3,t3,t1			# disable IE in sreg copy
	mtc0	t3,CP0_SREG			# re-write sreg (EXL=1,IE=0)
	nop					# let sreg settle

	mtc0	t1,CP0_SREG			# restore sreg (EXL=1)
	nop					# let sreg settle

	ld	t2,PUSH_T2_OFS(sp)		# restore t2
	ld	t3,PUSH_T3_OFS(sp)		# restore t3
	ld	t4,PUSH_T4_OFS(sp)		# restore t4
	dmtc0	t0,CP0_EPC			# restore EPC

	ld	t1,PUSH_T1_OFS(sp)		# restore t1
	ld	t0,PUSH_T0_OFS(sp)		# restore t0

	/*
	 * We need to restore the interrupted level (both the
	 * status register and the external mask)
	 */
	sw	k0,r4k_cpu_level		# restore old level
	la	k1,r4k_level_table		# get table address
	addu	k1,k1,k0			# get address
	lbu	k0,4(k1)			# read nevada intr mask
	la	k1,IO_ASIC_BASE			# get nevada base addr
	sb	k0,IO_IMASK_OFFSET(k1)		# write nevada int. mask

	lbu	k0,IO_IMASK_OFFSET(k1)		# sync the write

	/*
	 * Restore k0, k1, AT, sp, ra
	 */
	ld	ra,PUSH_RA_OFS(sp)		# restore ra
	ld	k1,PUSH_K1_OFS(sp)		# restore k1
	ld	k0,PUSH_K0_OFS(sp)		# restore k0
	ld	AT,PUSH_AT_OFS(sp)		# restore AT
	ld	sp,PUSH_SP_OFS(sp)		# restore old sp

	/*
	 * dismiss the interrupt
	 */
	eret					# return
	nop					# does not get executed

ENDFRAME(svip_cbus_intr)

/*
 * svip_dbus_intr:
 * Handle a Dbus (Level 7) Interrupt
 */
 #
 # void svip_dbus_intr (void)
 #
	.extern l7stk,4
	.extern l7cnt,4
	.extern lev7subr,4
	.extern vip_dbus_context,4

FRAME(svip_dbus_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 7 stack
	 * It dereferences off lev7subr to the wrapper/driver
	 * Note:  sp has already been copied to AT
	 */
	lw	sp,l7stk			# move onto level 7 stack
	lw	k0,l7cnt			# load counter

	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame on new stack
	sw	sp,vip_dbus_context             # save ptr to context

	/*
         * Create cache lines to save cache line fills.  Uses create
         * dirty exclusive to create cache lines for stack variables,
         * avoiding unnecessary cache line fills from memory.
	 */
	cache	3*4+1,288(sp)			# avoid loading cache line
	cache	3*4+1,256(sp)			# avoid loading cache line
	cache	3*4+1,224(sp)			# avoid loading cache line
	cache	3*4+1,192(sp)			# avoid loading cache line
	cache	3*4+1,96(sp)			# avoid loading cache line
	cache	3*4+1,64(sp)			# avoid loading cache line
	cache	3*4+1,32(sp)			# avoid loading cache line
	cache 	3*4+1,0(sp)			# avoid loading cache line

	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,l7cnt			# save the counter

	SAVE_T0_T3				/* save t0 .. t3 */

	/*
	 * Save the interrupted level and install new level
	 */
	lw	k0,r4k_cpu_level		# save old level
	li	t1,(LEVEL7 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t1,r4k_cpu_level		# save new level

	/*
	 * Load lev7subr into AT and call the dispatch routine
	 */
	li	k1,LEVEL7_SREG_MASK		# get new sreg mask
	j	svip_intr_dispatch		# call dispatcher
	lw	AT,lev7subr			# get handler (bd slot)

	.set	reorder
	.set	at
ENDFRAME(svip_dbus_intr)

/*
 * svip_errs_intr:
 * Handle a Level 3 Interrupt (Bus error of some sort)
 */
 #
 # void svip_errs_intr (void)
 #
	.extern l3stk,4
	.extern l3cnt,4
	.extern lev3subr,4

FRAME(svip_errs_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 3 stack
	 * It dereferences off lev3subr to the wrapper/driver
	 * Note:  sp has already been copied to AT
	 */
	lw	sp,l3stk			# move onto level 3 stack
	lw	k0,l3cnt			# load counter

	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame on new stack
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,l3cnt			# save the counter

	SAVE_T0_T3				/* save t0 .. t3 */

	/*
	 * Save the interrupted level and install new level
	 */
	lw	k0,r4k_cpu_level		# save old level
	li	t1,(LEVEL3 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t1,r4k_cpu_level		# save new level

	la	t2,r4k_level_table		# get level table address
	lbu	t1,(4+(8*LEVEL3))(t2)		# read new mask value
	la	t0,IO_ASIC_BASE			# get nevada base addr
	sb	t1,IO_IMASK_OFFSET(t0)		# write nevada int. mask
	lbu	t1,IO_IMASK_OFFSET(t0)		# sync the write

	/*
	 * Load lev3subr into AT and call the dispatch routine
	 */
	li	k1,LEVEL3_SREG_MASK		# get new sreg mask
	j	svip_intr_dispatch		# call dispatcher
	lw	AT,lev3subr			# get handler (bd slot)

	.set	reorder
	.set	at
ENDFRAME(svip_errs_intr)

/*
 * svip_ext_timer_intr:
 * Handle a Level 3 External Timer Interrupt 
 */
 #
 # void svip_ext_timer_intr(void)
 #

FRAME(svip_ext_timer_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 3 stack
	 * Note:  sp has already been copied to AT
	 */
	lw	sp,l3stk			# move onto level 3 stack
	lw	k0,l3cnt			# load counter

	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame on new stack
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,l3cnt			# save the counter

	SAVE_T0_T3				/* save t0 .. t3 */

	/*
	 * Save the interrupted level and install new level
	 */
	lw	k0,r4k_cpu_level		# save old level
	li	t1,(LEVEL3 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t1,r4k_cpu_level		# save new level

	la	t2,r4k_level_table		# get level table address
	lbu	t1,(4+(8*LEVEL3))(t2)		# read new mask value
	la	t0,IO_ASIC_BASE			# get nevada base addr
	sb	t1,IO_IMASK_OFFSET(t0)		# write nevada int. mask
	lhu	t1,IO_TVR_OFFSET(t0)		# sync the write and clear
  						# the interrupt
	/*
	 * Load handler address into AT and call the dispatch routine
	 */
	li	k1,LEVEL3_SREG_MASK		# get new sreg mask
	la	AT,netint_throttle_off		# get handler (bd slot)
	j	svip_intr_dispatch		# call dispatcher
	nop

	.set	reorder
	.set	at
ENDFRAME(svip_ext_timer_intr)

/*
 * svip_unused_intr:
 * Handle a case where Nevada equivalent reports an interrupt 
 * that is unused. We treat this as a gross hardware failure
 * allthough it could be considered a spurious interrupt.
 */
FRAME(svip_unused_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the present stack. It calls the unused
	 * interrupt driver which will crash the system. Before
	 * calling the handler, install Level 3 priorities (so we can
	 * be interrupted by the timer, Malu Attention and Dbus).
	 */
	lhu	a0,IO_EXT_INT_LO_REG		# read cause bits

	li	t1,(LEVEL3 << R4K_LEVEL_SHIFT)	# get Level 3
	sw	t1,r4k_cpu_level		# set Level 3
	la	t2,r4k_level_table		# get level table address
	lbu	t1,(4+(8*LEVEL3))(t2)		# read new mask value

	la	t0,IO_ASIC_BASE			# get nevada base addr
	sb	t1,IO_IMASK_OFFSET(t0)		# write nevada int. mask


	mfc0	a1,CP0_SREG			# read status register
	lw	t3,(8*LEVEL3)(t2)		# read new IP bits for Level 4

	li	t4,~SREG_IE			# get IE mask
	and	t4,a1,t4			# clear IE
	mtc0	t4,CP0_SREG			# write status register
	nop					# let it settle
		
	li	t5,~(CAUSE_IMASK|SREG_EXL)	# get IP[7..0]|SREG_EXL mask
	and 	t5,t4,t5			# clear IP[7..0],EXL
	or	t5,t5,t3			# insert Level 3 IP bits
	mtc0	t5,CP0_SREG			# write status register
	nop					# let it settle

	/*
	 * We can end up in an interrupt handler with IE off if the
	 * interrupt occurs at the next instruction after a move to the
	 * status register that turns IE off. So we turn it back on.
	 */
	ori	t5,t5,SREG_IE			# enable IE
	mtc0	t5,CP0_SREG			# write status register
	nop					# let it settle

	la	t2,IO_ASIC_BASE			# get nevada base addr
	lbu	t1,IO_IMASK_OFFSET(t2)		# read nevada int. mask (to
						# sync the posted writes)

	jal	r4k_unused_handler		# call unused handler
	nop					# (bd slot)

	/*
	 * The unsued handler should never return. If it does,
	 * call crashdump(0)
	 */
	jal	crashdump			# call crashdump
	move	a0,zero				# (bd slot)

	.set	reorder
	.set	at
ENDFRAME(svip_unused_intr)

/*
 * svip_pa_nmi_intr:
 * Handle a Level 2 Interrupt (PA management)
 */
 #
 # void svip_pa_nmi_intr (void)
 #
	.extern l2stk,4
	.extern l2cnt,4
	.extern lev2subr,4

FRAME(svip_pa_nmi_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 2 stack
	 * It dereferences off lev2subr to the wrapper/driver
	 * Note:  sp has already been copied to AT
	 */
	lw	sp,l2stk			# move onto level 2 stack
	lw	k0,l2cnt			# load counter

	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame on new stack
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,l2cnt			# save the counter

	SAVE_T0_T3				/* save t0..t3 */

	/*
	 * Save the interrupted level and install new level
	 */
	lw	k0,r4k_cpu_level		# save old level
	li	t1,(LEVEL2 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t1,r4k_cpu_level		# save new level

	SAVE_VOLATILE_REGS			/* save volatile registers */

	/*
	 * Re-enable interrupts so Malu_Attention can be serviced
	 *
	 * Note:  There seems to be a problem with the processor when EXL 
	 * gets de-asserted while IE is asserted and there is another
	 * interrupt pending (the interrupt is taken but the EPC register
	 * does not get re-latched). To work arround this, disable IE,
	 * reset the EXL bit, and then re-enable IE.
	 */
	li	t3,~SREG_IE			# get IE mask
	and	t3,t3,t1			# de-assert IE
	mtc0	t3,CP0_SREG			# move to status register

	li	t2,LEVEL2_SREG_MASK		# get sreg mask
	and	t0,t2,t3			# and sreg copy with mask
	mtc0	t0,CP0_SREG			# EXL=0, IE=0

	/*
	 * We can end up in an interrupt handler with IE off if the
	 * interrupt occurs at the next instruction after a move to the
	 * status register that turns IE off. So we turn it back on.
	 */
	and	t0,t2,t1			# and it into sreg
	ori	t0,t0,SREG_IE			# set IE
	mtc0	t0,CP0_SREG			# write back sreg with EXL=0

	lw	t0,lev2subr			# get handler address

	SAVE_V0_A3				/* save v0,v1,a0..a3 */
	SAVE_T4_RA				/* save t4..t9,ra */

	/*
	 * Call the level 2 handler
	 */
	jal	t0				# call level 2 handler
	nop					# (bd slot)

	/*
	 * restore the context. We restore the stacked status register
	 * which guarantees that EXL will be asserted through critical 
	 * section
	 *
	 * Note: The R4400 processor does not like s/w to change
	 * the state of EXL with IE enabled, so disable it during change.
	 */
	RESTORE_V0_A3				/* restore v0,v1,a0..a3 */

	lw	t5,SP_OFS(REG_SREG)(sp)		# load Status reg
	ld	t0,SP_OFS(REG_T0)(sp)		# restore t0

	mfc0	t6,CP0_SREG			# read sreg
	li	t4,~SREG_IE			# get IE mask

	and	t6,t6,t4			# disable IE bit
	mtc0	t6,CP0_SREG			# re-write sreg (EXL=0,IE=0)

	and	t7,t5,t4			# disable IE in sreg copy
	mtc0	t7,CP0_SREG			# re-write sreg (EXL=1,IE=0)
	nop					# let sreg settle

	mtc0	t5,CP0_SREG			# restore sreg
	nop					# let sreg settle

	RESTORE_T1_T3_EPC_LO_HI			/* restore t1..t3,EPC,Lo,Hi */

	/*
	 * restore old cpu level
	 */
	sw	k0,r4k_cpu_level		# restore old level

	RESTORE_T4_RA				/* restore t4..t9,ra */
	RESTORE_K0_K1_AT			/* restore k0,k1,AT */

	/*
	 * move back onto the original stack
	 * We dont have to increment the stack pointer
	 */
	ld	sp,SP_OFS(REG_SP)(sp)		# restore old sp

	/*
	 * dismiss the interrupt
	 */
	eret					# return
	nop					# does not get executed

	.set	reorder
	.set	at
ENDFRAME(svip_pa_nmi_intr)

/*
 * svip_pa_lo_intr :
 * Handle a Level 1 interrupt (PA Net IO, MALU Cmd)
 * Upon entry:
 *	k0 saved in FPU_R0
 *	k1 saved in FPU_R1
 *	AT saved in FPU_R2
 *	sp saved in AT
 */
	.extern l1stk, 4
	.extern l1cnt, 4

FRAME(svip_pa_lo_intr, sp, 0, ra)
	.set noreorder
	.set noat

	/*
	 * Move onto level 1 stack and save context
	 */
	lw	sp,l1stk			# move onto level 1 stack
	lw	k0,l1cnt			# load counter

        /*
         * Save some registers so we have something to play with
         */
        dmtc1   AT,FPU_SP_REG                   # save sp
        dmtc1   t0,FPU_T0_REG                   # save t0
        dmtc1   t1,FPU_T1_REG                   # save t1
        dmtc1   t2,FPU_T2_REG                   # save t2
        dmtc1   t3,FPU_T3_REG                   # save t3

        /*
         * increment interrupt counter
         */
	addiu	k0,k0,1				# increment counter
	sw	k0,l1cnt			# save counter

	/*
	 * save volatile context
	 */
	dmfc1	t0,FPU_R0			# get copy of k0
	dmfc1	t1,FPU_R1			# get copy of k1
	dmfc1	t2,FPU_R2			# get copy of AT
	dmtc1	t0,FPU_K0_REG			# save k0
	dmtc1	t1,FPU_K1_REG			# save k1
	dmtc1	t2,FPU_AT_REG			# save AT
	dmfc0	t0,CP0_EPC			# get EPC reg
	mfc0	t1,CP0_SREG			# get Status reg
	mflo	t2				# get LO
	mfhi	t3				# get HI
	dmtc1	t0,FPU_EPC_REG			# save EPC reg
	dmtc1	t1,FPU_SREG_REG			# save status reg
	dmtc1	t2,FPU_LO_REG			# save lo
	dmtc1	t3,FPU_HI_REG			# save hi

	/*
	 * Install our level
	 */
	lw      k0,r4k_cpu_level                # read old value
	li      t2,(LEVEL1 << R4K_LEVEL_SHIFT)  # get new level (bits [5..3])
	sw      t2,r4k_cpu_level                # register new value

	/*
	 * Re-enable interrupts so Malu Attention does not wait too long.
	 * We want to mask out PA IO and DMA 
	 *
	 * Note:  There seems to be a problem with the processor when EXL 
	 * gets de-asserted while IE is asserted and there is another
	 * interrupt pending (the interrupt is taken but the EPC register
	 * does not get re-latched). To work arround this, disable IE,
	 * reset the EXL bit, and then re-enable IE.
	 */
	li	t3,~SREG_IE			# get IE mask
	and	t3,t3,t1			# de-assert IE
	mtc0	t3,CP0_SREG			# move to status register

	li      t2,LEVEL1_SREG_MASK             # get sreg mask
	and     t0,t2,t3                        # insert new bits
	mtc0    t0,CP0_SREG                     # write new sreg copy

	li	t2,LEVEL1_SREG_MASK		# get new mask
	and	t0,t2,t3			# and it into sreg copy
	mtc0	t0,CP0_SREG			# clear EXL (IE disabled)

	/*
	 * We can end up in an interrupt handler with IE off if the
	 * interrupt occurs at the next instruction after a move to the
	 * status register that turns IE off. So we turn it back on.
	 */
	and     t0,t2,t1                        # and into original sreg
	ori     t0,t0,SREG_IE                   # set IE
	mtc0    t0,CP0_SREG                     # write sreg (EXL=0,IE=1)
	nop					# let it settle

	/*
	 * save some more context
	 */
	dmtc1	v0,FPU_V0_REG			# save v0
	dmtc1	v1,FPU_V1_REG			# save v1
	dmtc1	a0,FPU_A0_REG			# save a0
	dmtc1	a1,FPU_A1_REG			# save a1
	dmtc1	a2,FPU_A2_REG			# save a2
	dmtc1	a3,FPU_A3_REG			# save a3
	dmtc1	t4,FPU_T4_REG			# save t4
	dmtc1	t5,FPU_T5_REG			# save t5
	dmtc1	t6,FPU_T6_REG			# save t6
	dmtc1	t7,FPU_T7_REG			# save t7
	dmtc1	t8,FPU_T8_REG			# save t8
	dmtc1	t9,FPU_T9_REG			# save t9
	dmtc1	ra,FPU_RA_REG			# save ra

	dmtc1	s0,FPU_S0_REG			# save s0
	dmtc1	s1,FPU_S1_REG			# save s1
	dmtc1	s2,FPU_S2_REG			# save s2

	/*
	 * If throttling is configured, set it up
	 */
	lbu	t2,svip_netint_throttle_configured

	beq	t2,zero,level1_play_it_again	# nope, hop on by
	dmtc1	s3,FPU_S3_REG			# save s3 (bd slot)

	jal	svip_netint_throttle_setup	# setup throttling
	nop					# (bd slot)

level1_play_it_again:
	/*
	 * Clear software interrupts
	 */
	mtc0	zero,CP0_CAUSE			# clear sw int 0/1

	/* Once thru this loop check malu queue */
	jal	vip_malu_attn_queue_check
	move	v0,zero	                        # clear work done flag

	/* Clear the dma interrupt */
	la	t1,CYA_OIC_REG			# address of clear register
	li	t0,CYA_QE_DMA_INTR	        # DMA interrupt bit mask 
	sw	t0,0(t1)			# Clear the interrupt

	/* See if PA0 needs attention */
	lhu	s3,(IO_PA_ISR1_REG)		# read intrp status
	andi	k1,s3,0x00ff			# and out everything except
						# PA0s eight net io interrupts
	beq	zero,k1,svip_cont1          
	nop					# (bd slot)

	/*
	 * Call port adaptor zero's wrapper
	 */
	la	s0,pa_jumptable			# get jump table address
	lw	s2,PA_BASE_OFS(s0)		# get PA base address
	lw	t3,PA_WRAPPER_OFS(s0)		# get wrapper

	jal	s1,t3				# call the wrapper
	andi	a2,s3,0x00ff			# interrupting devices 

	/*
	 * See if anything to do for PA1
	*/
	lhu	s3,(IO_PA_ISR1_REG)		# read intrp status

svip_cont1:
	srl	s3,s3,8                         # shift off PA0 bits
	andi	a2,s3,0x00ff			# and out everything except
						# PA1s eight net io interrupts

	beq	zero,a2,svip_cont2              # branch if no work
	addu	k1,k1,v0			# update "work_to_do"
						# from PA 0s wrapper (bd slot)
	/*
	 * Call port adaptor one wrapper
	 */
	la	s0,pa_jumptable			# get jump table address
	daddiu	s0,s0,PA_TABLE_SIZE		# slot 1s table address
	lw	s2,PA_BASE_OFS(s0)		# get PA base address
	lw	t3,PA_WRAPPER_OFS(s0)		# get wrapper
						# PA1s eight net io interrupts
	jal	s1,t3				# call the wrapper
	nop

 	addu	k1,k1,v0			# update "work_to_do"
						# from PA 1s wrapper (bd slot)
svip_cont2:

	li	s1,(CAUSE_IBIT4|CAUSE_IBIT3|CAUSE_IBIT2|CAUSE_IBIT1)

        /*
         * If throttling is enabled, call the check routine
         */
        lbu     a2,svip_netint_throttle_configured
 
        beq     a2,zero,no_throttling           # no throttling, jump on by
        mfc0    s0,CP0_CAUSE                    # read cause register(bd slot)

        jal     svip_netint_throttle_check      # call the throttle checker
        nop                                     # (bd slot)
 
	beq	zero,v0,no_throttling           # no throttling, jump on by
        mfc0    s0,CP0_CAUSE                    # read cause register (bd)

	and	s0,s0,s1			# check IP4,IP3, IP2

	bne	s0,zero,level1_play_it_again    # start all over
	li	s1,1				# (bd slot)

	la      s3,IO_ASIC_BASE                 # need to turn on timer

	beq	zero,zero,throttle_dismiss      # exit quickly
	sh	s1,IO_TCR_OFFSET(s3)            # start timer (bd slot)

no_throttling:

	and	s0,s0,s1			# check IP4,IP3, IP2

	bne	s0,zero,level1_play_it_again    # start all over
	nop

	/*
	 * Make sure this gets called once per interrupt
	*/
	jal	vip_dmaq_consumer
	nop

	/*
	 * Call the cleanup routine
	 */
	jal	vip_idle_chores			# call idle chore handler
	nop					# (bd slot)

throttle_dismiss:

	/*
	USEC_GET_TIMESTAMP(netint_return);
	*/
	mfc0	a1,CP0_COUNT
	la	a2,netint_return
	sw	a1,0(a2)

        /*
         * Restore context and dismiss a level 1 interrupt
         *      - k0 has previous level
         */
	dmfc1	v0,FPU_V0_REG			# restore v0
	dmfc1	v1,FPU_V1_REG			# restore v1
	dmfc1	a0,FPU_A0_REG			# restore a0
	dmfc1	a1,FPU_A1_REG			# restore a1
	dmfc1	a2,FPU_A2_REG			# restore a2
	dmfc1	a3,FPU_A3_REG			# restore a3
        dmfc1   t4,FPU_T4_REG                   # restore t4
        dmfc1   t5,FPU_T5_REG                   # restore t5
        dmfc1   t6,FPU_T6_REG                   # restore t6
        dmfc1   t7,FPU_T7_REG                   # restore t7
        dmfc1   t8,FPU_T8_REG                   # restore t8
        dmfc1   t9,FPU_T9_REG                   # restore t9
 
        dmfc1   t1,FPU_SREG_REG                 # get status reg
        dmfc1   ra,FPU_RA_REG                   # restore ra
        dmfc1   s0,FPU_S0_REG                   # restore s0
        dmfc1   s1,FPU_S1_REG                   # restore s1
 
        /*
         * Note: The R4400 processor does not like s/w to change
         * the state of EXL with IE enabled, so disable it during change.
         */
        mfc0    t2,CP0_SREG                     # read sreg
        li      t3,~SREG_IE                     # get IE bit mask
 
        and     t2,t2,t3                        # disable IE bit
        mtc0    t2,CP0_SREG                     # re-write sreg (EXL=0,IE=0)
 
        and     t3,t3,t1                        # disable IE in sreg copy
        mtc0    t3,CP0_SREG                     # re-write sreg (EXL=1,IE=0)
        nop                                     # let sreg settle
 
        mtc0    t1,CP0_SREG                     # restore sreg (EXL=1,IE=1)
 
        dmfc1   s2,FPU_S2_REG                   # restore s2
        dmfc1   s3,FPU_S3_REG                   # restore s3

	/*
	 * We need to restore the interrupted level. There is
	 * no need to restore the external mask as it does not change
	 * between level 0 and level 1.
	 */
	sw	k0,r4k_cpu_level		# restore old level

	/* Restore volatile registers */
        dmfc1   t0,FPU_EPC_REG                  # get EPC reg
        dmfc1   t2,FPU_LO_REG                   # get lo
        dmfc1   t3,FPU_HI_REG                   # get hi
 
        dmtc0   t0,CP0_EPC                      # restore EPC reg
        mtlo    t2                              # restore LO
        mthi    t3                              # restore HI
 
        dmfc1   sp,FPU_SP_REG                   # restore sp
        dmfc1   t0,FPU_T0_REG                   # restore t0
        dmfc1   t1,FPU_T1_REG                   # restore t1
        dmfc1   t2,FPU_T2_REG                   # restore t2
        dmfc1   t3,FPU_T3_REG                   # restore t3
 
        dmfc1   AT,FPU_AT_REG                   # restore AT
        dmfc1   k0,FPU_K0_REG                   # restore k0
        dmfc1   k1,FPU_K1_REG                   # restore k1
 
	/*
	* Dismiss the interrupt
	*/
	eret                                    # return
	nop                                     # not executed

	.set reorder
	.set at
ENDFRAME(svip_pa_lo_intr)

 #
 # boolean pending_cbus_dbus_intrpt (void)
 #
FRAME(pending_cbus_dbus_intrpt,sp,0,ra)
        .set    noreorder
 
	/*
	 * This implementation of this routine checks to see if a nevada
	 * interrupt is pending.
	*/
        mfc0    v0,CP0_CAUSE    # v0 = CP0 Cause
	li	a0,CAUSE_IBIT6
        j       ra		# return
        and     v0,v0,a0	# (bd slot)
 
        .set    reorder
ENDFRAME(pending_cbus_dbus_intrpt)

/*
 * pa_default_wrapper:
 * Default PA interrupt wrapper for SVIP
 *
 * Entry : s2 = scratch register
 *	   s1 = return address
 *	   s0 = &pa_jumptable[pa_number]
 *	   a0 = pa number
 *
 * Exit  : return TRUE if we need to be called again before
 *         the net IO loop dismisses the interrupt
 */
 #
 # int pa_default_wrapper(void)
 #
FRAME(pa_default_wrapper,sp,0,ra)
	.set	noreorder

	/*
	 * In the future this wrapper should verify that there is
	 * no outstanding interrupts for the particular PA.
	 * For the time being, we simply return FALSE
	 */
	j	s1			# return
	move	v0,zero			# return FALSE (bd slot)

	.set 	reorder
ENDFRAME(pa_default_wrapper)

/*
 * emt_call
 * ROM Monitor system call
 * Note : We pass parameters in registers. This limits the total number
 *        to four (a0..a3). Also, the ROM Monitor syscall handler may
 *        not support branch delay slot, so ensure that the syscall
 *        instruction is not in one.
 * We provide this routine so that vip2 can trap emt-calls and provide
 * extra reload support
 */
 
 #
 # int emt_call (int emt_type, [,arg1[,arg2,[arg3]]])
 #
FRAME(emt_call,sp,4,ra)
        .set    noreorder
 
	li	t0,EMT_CPU_EXCEPTION
	bne	a0,t0,emt_call1
	nop
	move	a0,a1
	move	a1,a2
	jal	svip_reload            
	move	a2,a3
	j	emt_call2
	nop

emt_call1:
	li	t0,EMT_RELOAD
	bne	a0,t0,emt_call2
	nop
	move	a0,zero
	move	a1,zero
	jal	svip_reload            
	move	a2,zero

emt_call2:
        syscall                 # Take a syscall trap. Arguments are allready
                                # in registers a0..a3
        nop                     # A bit paranoid !!
 
        j       ra              # return
        nop                     # bd slot
 
        .set    reorder
ENDFRAME(emt_call)
 

/*
 * Table of interrupt masks for Status register and Nevada Register
 *
 * Note, Nevada mask is inverse polarity IE 0->enabled, 1->disabled.
 * The sw interrupts 0 is enabled for all levels except
 * level 7. sw interrupt 1 is only enabled for level 0.
 */
 #
 # struct levels {
 #	ulong s_reg;
 #	uchar nev_mask;
 #	uchar pad[3];
 # } r4k_level_table[8];
 #
	.globl	r4k_level_table

	.text
	.align	3

r4k_level_table:

/*
 *	Bit Definitions				
 *	---------------
 *	IP 7	- Internal Timer	Nevada Bit 7 : Cbus Stall
 *	IP 6	- Nevada		Nevada Bit 6 : External Timer
 *	IP 5	- PA management		Nevada Bit 5 : Cbus
 *	IP 4	- DMA			Nevada Bit 4 : Dbus
 *	IP 3	- PA2			Nevada Bit 3 : Bus Error
 *	IP 2 	- PA1			Nevada Bit 2 : unused
 *	IP 1	- SW			Nevada Bit 1 : unused
 *	IP 0	- GDB			Nevada Bit 0 : unused
 *
 * Software levels:
 *	1	DMA, PA2, PA1, SW
 *	2	PA management
 *	3	Bus Errors, GDB, External Timer
 *	4	Orion Timer
 *	5	Cbus
 *	6	Cbus stall
 *	7	Dbus
 *
 * DON'T CHANGE THE STATUS REG MASKS WITHOUT CHANGING MASKS IN ASM_4K_SVIP.H
 *
 *			0 : Masked	0 : Masked
 *			1 : Enabled	1 : Enabled
 *
 *	Level		Status Reg	Nevada Mask
 *			IP 7..0		Bit 7..0
 *	---------------------------------------------
 *                      76543210        76543210
 *	0		11111111	11111000
 *	1		11100001	11111000
 *	2		11000001	11111000
 *	3		11000000	10110000
 *	4		01000000	10110000
 *	5		01000000	10010000
 *	6		01000000	00010000
 *	7		00000000	00000000
 */
	.word	0x0000ff00, 0xf8000000		# Level 0
	.word	0x0000e100, 0xf8000000		# Level 1
	.word	0x0000c100, 0xf8000000		# Level 2
	.word	0x0000c000, 0xb0000000		# Level 3
	.word	0x00004000, 0xb0000000		# Level 4
	.word	0x00004000, 0x90000000		# Level 5
	.word	0x00004000, 0x10000000		# Level 6
	.word	0x00000000, 0x00000000		# Level 7

/* end of file */
