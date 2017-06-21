/* $Id: asm_4k_vip_cbus.S,v 3.4.12.6 1996/08/20 00:32:33 cheilek Exp $
 * $Source: /release/112/cvs/Xsys/asm-mips/asm_4k_vip_cbus.S,v $
 *------------------------------------------------------------------
 * asm_4k_vip_cbus.S : CBUS/MEMD support for Viper/Checkers
 *
 * March 1995, Michael Beesley 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_vip_cbus.S,v $
 * Revision 3.4.12.6  1996/08/20  00:32:33  cheilek
 * CSCdi58374:  More show diag info needed
 * Branch: California_branch
 * Add CCB_CMD_VIP_GET_CONFIG
 *
 * Revision 3.4.12.5  1996/07/17  01:44:41  fsunaval
 * CSCdi59824:  RIF classification done on the VIP for the 1F.
 * Branch: California_branch
 * 1.Off-load RIF classification from the RSP to the VIP for the 1F.
 *   What's next, fragmentation?
 *
 * Revision 3.4.12.4  1996/05/22  23:26:47  rramacha
 * CSCdi58249:  Enable IP DFS on 4T
 * Branch: California_branch
 * Pass serial encap to serial driver in VIP
 *
 * Revision 3.4.12.3  1996/04/12  17:42:21  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.4.12.2  1996/04/04  20:14:35  getchell
 * CSCdi51696:  None malu attention-->idle Rx buffs==wasted memd
 * Branch: California_branch
 *
 * Revision 3.4.12.1  1996/03/21  22:27:31  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.5  1996/03/18  23:50:08  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.4  1996/02/27  02:24:16  fsunaval
 * CSCdi49935:  Certain interface specific commands broken during
 * VIP2_branch merge
 * Branch: ELC_branch
 *
 * Revision 3.2.2.3  1996/02/10  00:25:42  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.2  1996/02/08  06:44:23  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/23  04:39:16  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.4  1996/02/18  19:38:52  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.3  1996/01/31  23:04:44  rharagan
 * CSCdi47670:  NRZI support does not work on 4T
 * Permit NRZI command to be sent to the serial interfaces.
 *
 * Revision 3.2  1995/11/17  08:41:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:49:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:06:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

/*
 * Include platform specific header file
 */
#if 	defined(RVIP)
#include "asm_4k_vip.h"
#include "asm_4k_rvip.h"
#endif	/* RVIP */

#if 	defined(SVIP)
#include "asm_4k_vip.h"
#include "asm_4k_svip.h"
#endif	/* SVIP */

/*
 * Include memd support files
 */
#include "../hes/if_cmd.h"
#include "../hes/ccb_cmd.h"
#include "../src-vip/vip_memd.h"

/*
 * vip_cbus_intr:
 * Handle a CBus Malu_Attention Interrupt
 */
 #
 # void vip_cbus_intr (void)
 # Entry:  t0 = cmd/arg0
 # 	   t4 = ccb pointer
 #	   k1 = memd_cntl pointer
 #	   ra = return address
 #
FRAME(vip_cbus_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	la	t3,malu_attn_command_table	# get command table address

	/*
	 * Get command from register t0
	 */
	srl	t1,t0,16			# get cmd

	/*
	 * Process the command. Range check the command, create
	 * an index into the command table, lookup the command vector
	 * and call it.
	 */
	sltiu	t2,t1,CCB_CMD_MAX_COMMANDS	# range check command
	bne	t2,zero,cbus_cmd_range_ok	# is it within range ?
	sll	t2,t1,2				# make inst. offset (bd slot)

	b	ccb_unsupported_command		# it is unsupported
	nop					# (bd slot)

cbus_cmd_range_ok:
	addu	t3,t3,t2			# get address of entry
	lw	t2,0(t3)			# read handler address

	j	t2				# call handler
	nop					# (bd slot)

/*
 * Individual command routines :
 *
 * Entry : t0 = cmd/arg0
 *  	   t1 = cmd
 * 	   t4 = ccb pointer
 *	   k1 = memd_cntl pointer
 *	   ra = return address
 *
 * Exit:   Write status / done. Return.
 */

/***************************************************************************/

	/*
	 * CCB_CMD_VERSION		# 0
	 */
ccb_cmd_version:

	/*	
	 * Test for software version
	 */
	andi	t0,t0,0xffff		# get arg
	beq	zero,t0,sw_version      # test for sw_version arg
	subu	t0,t0,1			# (bd slot)

	/*
	 * Test for hardware version
	 */
	beq	zero,t0,hw_version      # test for hw_version arg
	subu	t0,t0,1			# (bd slot)

	/*
	 * Test for VPLD version
	 */
	beq	zero,t0,vpld_version    # test for vpld_version arg
	subu	t0,t0,1			# (bd slot)
	
	/*
	 * Test for Rom Monitor version
	 */
	beq	zero,t0,rom_version     # test for rom_version arg
	nop
	
	/* 
	 * Unsupported argument 
	 */
	li	k1,CCB_DONE_ERROR	# error indication plus done
	j	ra			# return
	sw	k1,CCB_RES0DONE(t4)	# write return code (bd slot)

sw_version:
	lw	k1,MEMD_CNTL_SWVER(k1)	# sw version plus done
	j	ra			# return
	sw	k1,CCB_RES0DONE(t4)	# write return code (bd slot)

hw_version:
	li	k1,CCB_DONE_OK		# unused but ok
	j	ra			# return
	sw	k1,CCB_RES0DONE(t4)	# write return code (bd slot)

vpld_version:
	lw	k1,MEMD_CNTL_VPLDVER(k1) # vpld version plus done
	j	ra			# return
	sw	k1,CCB_RES0DONE(t4)	# write return code (bd slot)

rom_version:
	lw	k1,MEMD_CNTL_ROMVER(k1)	# rom version plus done
	j	ra			# return
	sw	k1,CCB_RES0DONE(t4)	# write return code (bd slot)

/***************************************************************************/

	/*
	 * CCB_CMD_RESET 		# 1
	 */
ccb_cmd_reset:
	li	k1,CCB_DONE_OK		# "done"
	sh	k1,CCB_DONE(t4)		# write return code (bd slot)

	/*
	 * Get memd_cntl and vip_rpsp_icb_array addresses
	 */	
	la	k1,memd_cntl		# get memd_cntl address
	la	t4,vip_rpsp_icb_array	# get vip_rpsp_icb_array address
	move	t0,zero			# zero out count
	li	t1,MAX_VIP_INTERFACES	# set max interface number

cmd_reset_next:
	beq	t0,t1,cmd_reset_done	# are we done ?
	nop				# (bd slot)
	
	lw	AT,0(t4)		# read vip_rpsp_icb_array value

	beq	AT,zero,no_intf_to_rst	# is there an interface ?
	nop				# (bd slot)

	/*	
	 * Read if_fcitype from memd_cntl, convert it to
	 * a ushort, or in bufferin support and write to memd
	 */
	lw	t2,MEMD_CNTL_IFTYPE(k1)
	srl	t2,t2,16
	ori	t2,t2,0x8100
	sh	t2,0(AT)

no_intf_to_rst:
	addiu	t4,t4,4			# increment vip_rpsp_icb_array pointer
	addiu	k1,k1,4			# increment memd_cntl.if_fcitype ptr
	addiu	t0,t0,1			# increment count

	b	cmd_reset_next		# go to the next interface
	nop				# (bd slot)

cmd_reset_done:
	j	ra			# return
	nop				# (bd slot)


/***************************************************************************/

	/*
	 * CCB_CMD_SELECT 		# 2
	 */
ccb_cmd_select:

	andi	t0,t0,0xffff		# get arg

	/*
	 * The RP/SP code does support more than 8 interfaces per
	 * cbus slot. So the interface number is correct as is
	 */
	sll	AT,t0,2			# make selected interface into offset

	/* 
	 * See if the selected interface is out of range 
	 */
	subu 	t1,t0,MAX_VIP_INTERFACES
	bltz	t1,select_in_range
	nop

        li      k1,CCB_DONE_OUT_OF_RANGE # interface is out of range  
        j       ra                      # return
        sw      k1,CCB_RES0DONE(t4)     # write return code (bd slot)

select_in_range:
	addu	AT,k1,AT		# get address into memd_cntl
        lw      t1,MEMD_CNTL_PASCB_TABLE(AT)
 
        bne     t1,zero,selected_int_ok # do we have this interface ?
        nop    				# (bd slot)

        li      k1,CCB_DONE_UNKNOWN     # unknown interface
        j       ra                      # return
        sw      k1,CCB_RES0DONE(t4)     # write return code (bd slot)
 
selected_int_ok:

	sw	t0,MEMD_CNTL_SELECT(k1)	# save selected interface
	lw	AT,MEMD_CNTL_IFTYPE(AT)	# read interface fci_type (plus done)
	
        j       ra                      # return
        sw      AT,CCB_RES0DONE(t4)     # write return code (bd slot)

/***************************************************************************/

	/*
	 * CCB_CMD_MEMD_SELECT 		# 8
	 */
ccb_cmd_memd_select:
	andi	t0,t0,0xffff		# get arg
	la	t1,ADRSPC_CBUS		# get base address of MEMD
	sll	t0,t0,1			# shift address to be bytes
	addu	t1,t1,t0		# get MEMD address (byte address)

	/*
	 * Save the address in memd_cntl and return done
	 */
	li	t0,CCB_DONE_OK
	sh	t0,CCB_DONE(t4)		# write return code

	j	ra			# return
	sw	t1,MEMD_CNTL_MEMD_ADDR(k1) # save address (bd slot)

/***************************************************************************/

	/*
	 * CCB_CMD_MEMD_WRITE 		# 9
	 */
ccb_cmd_memd_write:
	lw	t1,MEMD_CNTL_MEMD_ADDR(k1) # get address from memd control

	li	k1,CCB_DONE_OK		# "done"
	sh	k1,CCB_DONE(t4)		# write return code

	j	ra			# return
	sh	t0,0(t1)		# write to memd (bd slot)

/***************************************************************************/

	/*
	 * CCB_CMD_MEMD_READ	 	# 10
	 */
ccb_cmd_memd_read:
	mfc0	t1,CP0_SREG		# read status register
	li	t2,SREG_DE		# get DE parity bit
	or	t2,t2,t1		# disable parity checking
	mtc0	t2,CP0_SREG		# write to status register

	lw	t0,MEMD_CNTL_MEMD_ADDR(k1) # get address from memd_cntl
	lhu	t0,0(t0)		# read memd

	li	k1,CCB_DONE_OK		# "done" value
	mtc0	t1,CP0_SREG		# restore original status register

	sll	t0,t0,16		# shift left 16
	or	k1,k1,t0		# or in "done"

	j	ra			# return
	sw	k1,CCB_RES0DONE(t4)	# write return code (bd slot)

/***************************************************************************/

	/*
	 * CCB_CMD_RETURN_BUFFER 	# 24
	 */
ccb_cmd_ret_buf:
	lw	t3,MEMD_CNTL_SELECT(k1)	# read selected interface no.
	sll	t3,t3,2			# multiply it by 4
	addu	k1,k1,t3		# add it to memd_cntl

	lw	t3,MEMD_CNTL_RXCOUNT(k1) # read rx_count (plus done)

	j	ccb_deferred_command
	sh	t3,CCB_RES0DONE(t4)	# write return code (bd slot)

/***************************************************************************/

	/*
	 * CCB_CMD_CARRIER	 	# 37
	 */
ccb_cmd_carrier:
	lw	t0,MEMD_CNTL_SELECT(k1)	# read selected interface no.
	sll	t0,t0,2			# multiply it by 4
	addu	k1,k1,t0		# add it to memd_cntl

	lw	t0,MEMD_CNTL_IFCARRIER(k1) # read carrier state (plus done)

	j	ra			# return
	sw	t0,CCB_RES0DONE(t4)	# write return code (bd slot)

/***************************************************************************/

	/*
	 * CCB_CMD_RXERROR	(RPSP)	# 47
	 * CCB_CMD_GLOBAL_PTRS	(RSP)
	 */
ccb_cmd_rxerr_glb_ptr_command:
	j	ccb_deferred_3args_res0_command	# return res0 and defer
	lw	t1,MEMD_CNTL_MAX_LV_LTR(k1)	# maximum love letter
						# plus done (bd slot)

/***************************************************************************/

	/*
	 * Unsupported command
	 */
ccb_unsupported_command:
	li	k1,CCB_DONE_ERROR	# error
	j	ra			# return
	sw	k1,CCB_RES0DONE(t4)	# write return code (bd slot)

/***************************************************************************/

	/*
	 * Queue the command. Non-interface specific
	 */
ccb_deferred_command:

	/*
	 * Make sure malu_q.write+1 != malu_q.read
	 */
	la	k1,malu			# get address of malu structure
	lw	t3,MEMD_CMDQ_WRITE(k1)	# t3 = malu.write
	lw	t2,MEMD_CMDQ_READ(k1)	# t2 = malu.read
	addiu	t3,t3,1			# malu.write += 1
	li	AT,(MALU_ATTN_CMD_DEPTH-1)
	and	t3,t3,AT		# malu.write &= MALU_CMD_DEPTH-1
	bne	t3,t2,command_q_ok	# room on the command Q ?
	nop				# (bd slot)

	/*
	 * No room on the command Q. Assert SW 1 interrupt
	 * and return error. Increment the no_qerr_count
	 */
	la	k1,memd_cntl		# get memd_cntl address
	lw	t2,MEMD_CNTL_NOQERR(k1)	# read no_qerr_count
	addiu	t2,t2,1			# increment no_qerr_count
	sw	t2,MEMD_CNTL_NOQERR(k1)	# save no_qerr_count

	li	k1,CAUSE_IBIT1		# sw 1 interrupt bit
	mtc0	k1,CP0_CAUSE		# assert the net IO interrupt
	li	k1,CMD_RSP_ERR_CMDQFL	# error
	j	ra			# return
	sw	k1,CCB_RES0DONE(t4)	# write return code (bd slot)

command_q_ok:

	/*
	 * Tell the RP we are done
	 */
	li	AT,CCB_DONE_OK		# "done"
	sh	AT,CCB_DONE(t4)		# write return code

	/*
	 * Get cmd_q pointer and write back cmd.write
	 */
	sll	t2,t3,4			# multiplty write * 16
	addu	t2,k1,t2		# get cmd_q addr - 8
	sw	t3,MEMD_CMDQ_WRITE(k1)	# malu.write = t3

	/*
	 * Fill in the fields in malu.cmd_q[malu.write]
	 */
	sw	t0,(MEMD_CMDQ_CMDQ+MEMD_CMD_CMDARG0)(t2)
	sh	zero,(MEMD_CMDQ_CMDQ+MEMD_CMD_DRIVER_CMD)(t2)
	la	k1,memd_cntl
	lw	t0,MEMD_CNTL_SELECT(k1)
	sll	t0,t0,2
	addu	k1,t0,k1
	lw	t0,MEMD_CNTL_PASCB_TABLE(k1)
	sw	t0,(MEMD_CMDQ_CMDQ+MEMD_CMD_PASCB)(t2)

	/*
	 * Assert sw int 1 and return
	 */
	li	k1,CAUSE_IBIT1		# sw 1 interrupt bit
	j	ra			# return
	mtc0	k1,CP0_CAUSE		# assert the net IO interrupt (bd slot)
	
/***************************************************************************/

	/*
	 * Queue the command. Interface specific
	 */
ccb_deferred_intf_command:

	/*
	 * Make sure malu_q.write+1 != malu_q.read
	 */
	la	k1,malu			# get address of malu structure
	lw	t3,MEMD_CMDQ_WRITE(k1)	# t3 = malu.write
	lw	t2,MEMD_CMDQ_READ(k1)	# t2 = malu.read
	addiu	t3,t3,1			# malu.write += 1
	li	AT,(MALU_ATTN_CMD_DEPTH-1)
	and	t3,t3,AT		# malu.write &= MALU_CMD_DEPTH-1
	beq	t3,t2,command_q_not_ok	# room on the command Q ?
	nop				# (bd slot)

	/*
	 * Tell the RP we are done
	 */
	li	AT,CCB_DONE_OK		# "done"
	sh	AT,CCB_DONE(t4)		# write return code

	/*
	 * Get cmd_q pointer and write back cmd.write
	 */
	sll	t2,t3,4			# multiplty write * 16
	addu	t2,k1,t2		# get cmd_q addr - 8
	sw	t3,MEMD_CMDQ_WRITE(k1)	# malu.write = t3

	/*
	 * Fill in the fields in malu.cmd_q[malu.write]
	 */
	sw	t0,(MEMD_CMDQ_CMDQ+MEMD_CMD_CMDARG0)(t2)
	li	AT,0x1
	sh	AT,(MEMD_CMDQ_CMDQ+MEMD_CMD_DRIVER_CMD)(t2)
	la	k1,memd_cntl
	lw	t0,MEMD_CNTL_SELECT(k1)
	sll	t0,t0,2
	addu	k1,t0,k1
	lw	t0,MEMD_CNTL_PASCB_TABLE(k1)
	sw	t0,(MEMD_CMDQ_CMDQ+MEMD_CMD_PASCB)(t2)

	/*
	 * Assert sw int 1 and return
	 */
	li	k1,CAUSE_IBIT1		# sw 1 interrupt bit
	j	ra			# return
	mtc0	k1,CP0_CAUSE		# assert the net IO interrupt (bd slot)

command_q_not_ok:
	/*
	 * No room on the command Q. Assert SW 1 interrupt
	 * and return error. Increment the no_qerr_count
	 */
	la	k1,memd_cntl		# get memd_cntl address
	lw	t2,MEMD_CNTL_NOQERR(k1)	# read no_qerr_count
	addiu	t2,t2,1			# increment no_qerr_count
	sw	t2,MEMD_CNTL_NOQERR(k1)	# save no_qerr_count

	li	k1,CAUSE_IBIT1		# sw 1 interrupt bit
	mtc0	k1,CP0_CAUSE		# assert the net IO interrupt
	li	k1,CMD_RSP_ERR_CMDQFL	# error
	j	ra			# return
	sw	k1,CCB_RES0DONE(t4)	# write return code (bd slot)

/***************************************************************************/
    /*
     * Queue the command. Interface specific.  Three args required
     */
ccb_deferred_3args_intf_command:

    	li	t1,CCB_DONE_OK              # "done"

	/*	
	 * Queue the command. Interface specific.  Three args required
     	 * and combined res0/done is specified in register t1.
     	 */

	/*
     	 * Make sure malu_q.write+1 != malu_q.read
     	 */
    	la	k1,malu	                # get address of malu structure
    	lw	t3,MEMD_CMDQ_WRITE(k1)  # t3 = malu.write
    	lw	t2,MEMD_CMDQ_READ(k1)   # t2 = malu.read
   	addiu	t3,t3,1             	# malu.write += 1
    	li	AT,(MALU_ATTN_CMD_DEPTH-1)
    	and	t3,t3,AT                # malu.write &= MALU_CMD_DEPTH-1
    	beq	t3,t2,command_q_not_ok  # room on the command Q ?
    	nop                         	# (bd slot)

    	/* Get additional arguments */
    	lw	t2,CCB_ARG1RES1(t4)

    	/*
     	 * Tell the RP/RSP we are done
     	 */
    	sw	t1,CCB_RES0DONE(t4)	# write return code

    	/*
     	 * Get cmd_q pointer and write back cmd.write
     	 */
    	sll	t1,t3,4                 # multiplty write * 16
    	addu	t1,k1,t1            	# get cmd_q addr - 8
    	sw	t3,MEMD_CMDQ_WRITE(k1)  # malu.write = t3

    	/*
     	 * Fill in the fields in malu.cmd_q[malu.write]
     	 */
    	sw	t0,(MEMD_CMDQ_CMDQ+MEMD_CMD_CMDARG0)(t1)
    	sw	t2,(MEMD_CMDQ_CMDQ+MEMD_CMD_ARG1ARG2)(t1)
    	li  	AT,0x1
    	sh	AT,(MEMD_CMDQ_CMDQ+MEMD_CMD_DRIVER_CMD)(t1)
    	la	k1,memd_cntl
    	lw	t0,MEMD_CNTL_SELECT(k1)
    	sll	t0,t0,2
    	addu	k1,t0,k1
    	lw	t0,MEMD_CNTL_PASCB_TABLE(k1)
    	sw	t0,(MEMD_CMDQ_CMDQ+MEMD_CMD_PASCB)(t1)

    	/*
     	 * Assert sw int 1 and return
     	 */
    	li	k1,CAUSE_IBIT1          # sw 1 interrupt bit
    	j	ra                      # return
    	mtc0	k1,CP0_CAUSE        	# assert the net IO interrupt (bd slot)

/***************************************************************************/

    /*
     * Queue the command. Non-interface specific.  Three args required
     */
ccb_deferred_3args_command:

    	li	t1,CCB_DONE_OK          # "done"

    	/*
    	 * Queue the command. Non-interface specific.  Three args required
    	 * and combined res0/done is specified in register t1.
    	 */
ccb_deferred_3args_res0_command:

    	/*
    	 * Make sure malu_q.write+1 != malu_q.read
    	 */
	la	k1,malu                 # get address of malu structure
    	lw	t3,MEMD_CMDQ_WRITE(k1)  # t3 = malu.write
    	lw	t2,MEMD_CMDQ_READ(k1)   # t2 = malu.read
    	addiu	t3,t3,1             	# malu.write += 1
    	li	AT,(MALU_ATTN_CMD_DEPTH-1)
    	and	t3,t3,AT                # malu.write &= MALU_CMD_DEPTH-1
    	beq	t3,t2,command_q_not_ok  # room on the command Q ?
    	nop	                        # (bd slot)

    	/* Get additional arguments */
    	lw	t2,CCB_ARG1RES1(t4)

    	/*
    	 * Tell the RP/RSP we are done
    	 */
    	sw	t1,CCB_RES0DONE(t4)	# write return code

    	/*
    	 * Get cmd_q pointer and write back cmd.write
    	 */
    	sll	t1,t3,4                 # multiplty write * 16
    	addu	t1,k1,t1            	# get cmd_q addr - 8
    	sw	t3,MEMD_CMDQ_WRITE(k1)  # malu.write = t3

    	/*
    	 * Fill in the fields in malu.cmd_q[malu.write]
    	 */
    	sw	t0,(MEMD_CMDQ_CMDQ+MEMD_CMD_CMDARG0)(t1)
    	sw	t2,(MEMD_CMDQ_CMDQ+MEMD_CMD_ARG1ARG2)(t1)
    	sh	zero,(MEMD_CMDQ_CMDQ+MEMD_CMD_DRIVER_CMD)(t1)
    	la	k1,memd_cntl
   	lw	t0,MEMD_CNTL_SELECT(k1)
    	sll	t0,t0,2
    	addu	k1,t0,k1
    	lw	t0,MEMD_CNTL_PASCB_TABLE(k1)
    	sw	t0,(MEMD_CMDQ_CMDQ+MEMD_CMD_PASCB)(t1)

    	/*
    	 * Assert sw int 1 and return
    	 */
    	li	k1,CAUSE_IBIT1          # sw 1 interrupt bit
    	j	ra                      # return
    	mtc0	k1,CP0_CAUSE        	# assert the net IO interrupt (bd slot)

/***************************************************************************/
	/*
	 * Queue the command. Non-interface specific.  Four args required
	 */
ccb_deferred_4args_command:

	/*
	 * Make sure malu_q.write+1 != malu_q.read
	 */
	la	k1,malu			# get address of malu structure
	lw	t3,MEMD_CMDQ_WRITE(k1)	# t3 = malu.write
	lw	t2,MEMD_CMDQ_READ(k1)	# t2 = malu.read
	addiu	t3,t3,1			# malu.write += 1
	li	AT,(MALU_ATTN_CMD_DEPTH-1)
	and	t3,t3,AT		# malu.write &= MALU_CMD_DEPTH-1
	beq	t3,t2,command_q_not_ok	# room on the command Q ?
	nop				# (bd slot)


	/* Get additional arguments */
	lw	t1,CCB_ARG1RES1(t4)
	lhu	t2,CCB_RES0DONE(t4)	

	/*
	 * Tell the RP we are done
	 */
	li	AT,CCB_DONE_OK		# "done"
	sh	AT,CCB_DONE(t4)		# write return code

	/*
	 * Get cmd_q pointer and write back cmd.write
	 */
	sw	t3,MEMD_CMDQ_WRITE(k1)	# malu.write = t3
	sll	t3,t3,4			# multiplty write * 16
	addu	t3,k1,t3		# get cmd_q addr - 8

	/*
	 * Fill in the fields in malu.cmd_q[malu.write]
	 */
	sw	t0,(MEMD_CMDQ_CMDQ+MEMD_CMD_CMDARG0)(t3)
	sw	t1,(MEMD_CMDQ_CMDQ+MEMD_CMD_ARG1ARG2)(t3)
	sh	t2,(MEMD_CMDQ_CMDQ+MEMD_CMD_ARG3)(t3)
	sh	zero,(MEMD_CMDQ_CMDQ+MEMD_CMD_DRIVER_CMD)(t3)
	la	k1,memd_cntl
	lw	t0,MEMD_CNTL_SELECT(k1)
	sll	t0,t0,2
	addu	k1,t0,k1
	lw	t0,MEMD_CNTL_PASCB_TABLE(k1)
	sw	t0,(MEMD_CMDQ_CMDQ+MEMD_CMD_PASCB)(t3)

	/*
	 * Assert sw int 1 and return
	 */
	li	k1,CAUSE_IBIT1		# sw 1 interrupt bit
	j	ra			# return
	mtc0	k1,CP0_CAUSE		# assert the net IO interrupt (bd slot)
	
/***************************************************************************/

	/*
	 * CCB_CMD_SYNC_COMMAND	 	# 128
	 */
ccb_cmd_sync_command:
	/*
	 * Increment the counter, and compare last_sync with arg
	 */
	lw	t2,MEMD_CNTL_SYNC_COUNT(k1)
	lhu	t1,MEMD_CNTL_LAST_SYNC(k1)

	addiu	t2,t2,1
	sw	t2,MEMD_CNTL_SYNC_COUNT(k1)

	/*
	 * If last_sync_value equals ARG0, we are still out of 
	 * phase with the RP, so just ignore it and return
	 */
	andi	t0,t0,0xffff            # isolate arg0
	bne	t0,t1,sync_cont
	nop

	j	ra			# return
	nop

sync_cont:
	/*
	 * Save sync value and return it to the RP
	 */
	sh	t0,MEMD_CNTL_LAST_SYNC(k1) # update memd_cntl.last_sync_value

	li	k1,CCB_DONE_OK		# get "done" reply
	sll	t0,t0,16		# shift last_sync_value left 16
	or	k1,k1,t0		# or in the "done" bits

	j	ra			# return
	sw	k1,CCB_RES0DONE(t4)	# write return code (bd slot)

	.set	reorder
	.set	at
ENDFRAME(vip_cbus_intr)

	/*
	 * VIP Malu Attention Command Table (in text segment)
	 */
	.text
	
	.global	malu_attn_command_table

malu_attn_command_table:

	.word	ccb_cmd_version			# (0)  	CCB_CMD_VERSION
	.word	ccb_cmd_reset	                # (1)   CCB_CMD_RESET
	.word	ccb_cmd_select			# (2)  	CCB_CMD_SELECT
	.word	ccb_deferred_intf_command	# (3)	CCB_CMD_PAGE_SELECT
	.word	ccb_deferred_intf_command	# (4)	CCB_CMD_REG_SELECT
	.word	ccb_deferred_intf_command	# (5)	CCB_CMD_REG_WRITE
	.word	ccb_deferred_intf_command	# (6)	CCB_CMD_REG_READ
	.word	ccb_deferred_intf_command	# (7)	Unused
	.word	ccb_cmd_memd_select		# (8)  	CCB_CMD_MEMD_SELECT
	.word	ccb_cmd_memd_write		# (9)  	CCB_CMD_MEMD_WRITE

	.word	ccb_cmd_memd_read		# (10) 	CCB_CMD_MEMD_READ
	.word	ccb_deferred_intf_command	# (11)	CCB_TRIP_MEMB_SELECT
	.word	ccb_deferred_intf_command	# (12)	CCB_TRIP_MEMB_CONT_WR
	.word	ccb_deferred_intf_command	# (13)	CCB_TRIP_MEMB_CONT_RD
	.word	ccb_deferred_intf_command	# (14)	CCB_TRIP_SPYGLASS_SETUP
	.word	ccb_deferred_intf_command	# (15)	CCB_TRIP_MEMB_WRITE
	.word	ccb_deferred_intf_command	# (16)	CCB_TRIP_MEMB_READ
	.word	ccb_deferred_intf_command	# (17)	CCB_MEMA_SELECT
	.word	ccb_deferred_intf_command	# (18)	CCB_MEMA_WRITE
        .word	ccb_deferred_intf_command	# (19)	CCB_MEMA_READ
	.word	ccb_deferred_intf_command	# (20) 	CCB_TRIP_XILINX_ARG
	.word	ccb_deferred_intf_command	# (21)	CCB_TRIP_XILINX_CMD
	.word	ccb_deferred_intf_command	# (22)	CCB_TRIP_TX_DMA_TEST
	.word	ccb_deferred_intf_command	# (23)	CCB_TRIP_RX_DMA_TEST
	.word	ccb_cmd_ret_buf 		# (24)	CCB_CMD_RETURN_BUFFER
	.word	ccb_deferred_3args_command	# (25)	CCB_CMD_TX_PTRS
	.word	ccb_deferred_3args_command	# (26)	CCB_CMD_RX_LIMITS
	.word	ccb_deferred_command		# (27)	CCB_CMD_RAW_QUEUE_PTR
	.word	ccb_deferred_intf_command	# (28)	CCB_TRIP_SPYGLASS_RES
	.word	ccb_deferred_intf_command	# (29)	CCB_TRIP_POKE_CUETRC

	.word	ccb_deferred_intf_command	# (30)
	.word	ccb_deferred_intf_command	# (31)
	.word	ccb_deferred_intf_command	# (32)	CCB_CMD_STATUS
	.word	ccb_deferred_intf_command	# (33)	CCB_CMD_STATION
	.word	ccb_deferred_intf_command	# (34)	CCB_CMD_FSIP_NRZI
	.word	ccb_unsupported_command		# (35)	CCB_CMD_AF_READ
	.word	ccb_deferred_intf_command	# (36)	CCB_CMD_AF_WRITE
	.word	ccb_cmd_carrier			# (37)	CCB_CMD_CARRIER
	.word	ccb_deferred_intf_command	# (38)	CCB_CMD_ERRORS
	.word	ccb_deferred_intf_command	# (39)	

	.word	ccb_deferred_intf_command	# (40) 	CCB_CMD_BUFFER_PARK
	.word	ccb_deferred_intf_command	# (41)	
	.word	ccb_deferred_intf_command	# (42)	CCB_CMD_APPLIQUE
	.word	ccb_deferred_intf_command	# (43)	CCB_CMD_CLOCKRATE
	.word	ccb_deferred_intf_command	# (44)	CCB_CMD_CLOCKTYPE
	.word	ccb_deferred_intf_command	# (45)	CCB_CMD_DELAY
	.word	ccb_deferred_3args_command	# (46)	CCB_CMD_RXPOOL
	.word	ccb_cmd_rxerr_glb_ptr_command	# (47)	CCB_CMD_RXERROR
						#  or   CCB_CMD_GLOBAL_PTRS
	.word	ccb_deferred_intf_command	# (48)	CCB_CMD_RXBYTES
	.word	ccb_deferred_command		# (49)	CCB_CMD_RX_OFFSET
	.word	ccb_deferred_intf_command	# (50) 	CCB_CMD_AF_SIZE
	.word	ccb_deferred_intf_command	# (51)	
	.word	ccb_deferred_intf_command	# (52)	
	.word	ccb_deferred_intf_command	# (53)	
	.word	ccb_deferred_intf_command	# (54)	
	.word	ccb_deferred_intf_command	# (55)	
	.word	ccb_deferred_intf_command	# (56)	
	.word	ccb_deferred_intf_command	# (57)	
	.word	ccb_deferred_intf_command	# (58)	
	.word	ccb_deferred_intf_command	# (59)	

	.word	ccb_deferred_intf_command	# (60) 	
	.word	ccb_deferred_intf_command	# (61)	
	.word	ccb_deferred_intf_command	# (62)	
	.word	ccb_deferred_intf_command	# (63)	CCB_CMD_SERIAL_CRC32
	.word	ccb_deferred_intf_command	# (64)	
	.word	ccb_deferred_intf_command	# (65)	
	.word	ccb_deferred_intf_command	# (66)	
	.word	ccb_deferred_intf_command	# (67)	
	.word	ccb_deferred_intf_command	# (68)	
	.word	ccb_deferred_intf_command	# (69)	

	.word	ccb_deferred_intf_command	# (70) 	
	.word	ccb_deferred_intf_command	# (71)	
	.word	ccb_deferred_intf_command	# (72)	
	.word	ccb_deferred_intf_command	# (73)	
	.word	ccb_deferred_intf_command	# (74)	
	.word	ccb_deferred_intf_command	# (75)	
	.word	ccb_deferred_intf_command	# (76)	
	.word	ccb_deferred_intf_command	# (77)	
	.word	ccb_deferred_intf_command	# (78)	
	.word	ccb_deferred_intf_command	# (79)	

	.word	ccb_deferred_intf_command	# (80) 	
	.word	ccb_deferred_intf_command	# (81)	
	.word	ccb_deferred_intf_command	# (82)	
	.word	ccb_deferred_intf_command	# (83)	
	.word	ccb_deferred_intf_command	# (84)	
	.word	ccb_deferred_intf_command	# (85)	
	.word	ccb_deferred_intf_command	# (86)	
	.word	ccb_deferred_intf_command	# (87)	CCB_CMD_SERIAL_ENCAP
	.word	ccb_deferred_intf_command	# (88)	
	.word	ccb_deferred_intf_command	# (89)	

	.word	ccb_deferred_intf_command	# (90) 	
	.word	ccb_deferred_intf_command	# (91)	
	.word	ccb_deferred_intf_command	# (92)	
	.word	ccb_deferred_intf_command	# (93)	
	.word	ccb_deferred_intf_command	# (94)	
	.word	ccb_deferred_intf_command	# (95)	
	.word	ccb_deferred_intf_command	# (96)	
	.word	ccb_deferred_intf_command	# (97)	
	.word	ccb_deferred_intf_command	# (98)	
	.word	ccb_deferred_intf_command	# (99)	

	.word	ccb_deferred_intf_command	# (100) VIP_1F_BRIDGE	
	.word	ccb_deferred_intf_command	# (101)	VIP_1F_TVX
	.word	ccb_deferred_intf_command	# (102)	VIP_1F_TRT
	.word	ccb_deferred_3args_intf_command # (103)	VIP_1F_STN_ADD
	.word	ccb_deferred_3args_intf_command # (104)	VIP_1F_INTERESTING_ADD
	.word	ccb_deferred_intf_command	# (105)	
						# VIP_1F_ALL_MULTICAST_ADD

	.word	ccb_deferred_intf_command	# (106) VIP_1F_GET_MIB_INFO
	.word	ccb_deferred_3args_intf_command	# (107)	VIP_1F_SRB
	.word	ccb_deferred_intf_command	# (108)	
	.word	ccb_deferred_intf_command	# (109)	

	.word	ccb_deferred_intf_command	# (110) 	
	.word	ccb_deferred_intf_command	# (111)	
	.word	ccb_deferred_intf_command	# (112)	
	.word	ccb_deferred_intf_command	# (113)	
	.word	ccb_deferred_intf_command	# (114)	
	.word	ccb_deferred_intf_command	# (115)	
	.word	ccb_deferred_intf_command	# (116)	
	.word	ccb_deferred_intf_command	# (117)	
	.word	ccb_deferred_intf_command	# (118)	
	.word	ccb_deferred_intf_command	# (119)	

	.word	ccb_deferred_command		# (120) CCB_CMD_VIP_GET_CONFIG
	.word	ccb_deferred_intf_command	# (121)	
	.word	ccb_deferred_intf_command	# (122)	
	.word	ccb_deferred_intf_command	# (123)	
	.word	ccb_deferred_intf_command	# (124)	
	.word	ccb_deferred_3args_intf_command	# (125)	CCB_CMD_VIP_CAM_POPULATE
	.word	ccb_deferred_3args_intf_command	# (126)	CCB_CMD_VIP_CAM_DEPOPULATE
	.word	ccb_deferred_intf_command	# (127)	
	.word	ccb_cmd_sync_command		# (128)	CCB_CMD_SYNC_COMMAND
	.word	ccb_deferred_3args_command	# (129)	CCB_CMD_IPC_QUEUE

	.word	ccb_deferred_4args_command	# (130)	CCB_CMD_IPC_INIT
	.word	ccb_deferred_intf_command	# (131)	
	.word	ccb_deferred_intf_command	# (132)	
	.word	ccb_deferred_intf_command	# (133)	
	.word	ccb_deferred_intf_command	# (134)	
	.word	ccb_deferred_intf_command	# (135)	
	.word	ccb_deferred_intf_command	# (136)	
	.word	ccb_deferred_intf_command	# (137)	
	.word	ccb_deferred_intf_command	# (138)	
	.word	ccb_deferred_intf_command	# (139)	

/*
 * all MEMD read routines, such as vip_memdread8(), vip_memdread16() ,
 * vip_memdread32(), vip_rpsp_memd_readbh(), and vip_rsp_memd_readbh(),
 * must be placed between the two psuedo routines start_memd_read_handlers
 * and end_memd_read_handlers (doesn't occupy space).
 * The SIGBUS signal handler will determine whether bus error
 * is due to a temporary CBUS stall condition by checking
 * 1) the access address is in MEMD location and
 * 2) the access instruction is between start_memd_read_handlers and 
 *    end_memd_read_handlers. 
 */
FRAME(start_memd_read_handlers,sp,0,ra)
ENDFRAME(start_memd_read_handlers)

/*
 * vip_probe_memd:
 * Read MEMD to check cbus status
 */
 #
 # boolean vip_probe_memd(void)
 #
FRAME(vip_probe_memd,sp,0,ra)
	.set	noreorder

	/*
	 * Return good. SIGBUS handler will set v0 to 0 if bus stall occurs
	 * while accessing 0(a0) in the next instruction
	 */
	ori	v0,zero,0x1		# return value is TRUE (bd slot)
	li	a0,(ADRSPC_CBUS+0x10000) # get probe address
	lw	t3,0(a0)		# read ulong from memd

	j	ra			# return
	nop

	.set	reorder
ENDFRAME(vip_probe_memd)

/*
 * vip_memdread8:
 * Perform an 8bit read of memd
 */
 #
 # boolean vip_memdread8(uchar *ptr, uchar *data)
 #
FRAME(vip_memdread8,sp,0,ra)
	.set	noreorder

	/*
	 * Return good. SIGBUS handler will set v0 to 0 if bus stall occurs
	 * while accessing 0(a0) in the next instruction
	 */
	ori	v0,zero,0x1		# return value is TRUE (bd slot)
	lbu	t3,0(a0)		# read uchar from memd

	j	ra			# return
	sb	t3,0(a1)		# write uchar to *data

	.set	reorder
ENDFRAME(vip_memdread8)

/*
 * vip_memdread16:
 * Perform a 16bit read of memd
 */
 #
 # boolean vip_readmemd16(ushort *ptr, ushort *data)
 #
FRAME(vip_memdread16,sp,0,ra)
	.set	noreorder

	/*
	 * Return good. SIGBUS handler will set v0 to 0 if bus stall occurs
	 * while accessing 0(a0) in the next instruction
	 */
	ori	v0,zero,0x1		# return value is TRUE (bd slot)
	lhu	t3,0(a0)		# read ushort from memd

	j	ra			# return
	sh	t3,0(a1)		# write ushort to *data

	.set	reorder
ENDFRAME(vip_memdread16)

/*
 * vip_memdread32:
 * Perform a 32bit read of memd
 */
 #
 # boolean vip_memdread32(ulong *ptr, ulong *data)
 #
FRAME(vip_memdread32,sp,0,ra)
	.set	noreorder

	/*
	 * Return good. SIGBUS handler will set v0 to 0 if bus stall occurs
	 * while accessing 0(a0) in the next instruction
	 */
	ori	v0,zero,0x1		# return value is TRUE (bd slot)
	lw	t3,0(a0)		# read ulong from memd

	j	ra			# return
	sw	t3,0(a1)		# write ulong to *data

	.set	reorder
ENDFRAME(vip_memdread32)

/*
 * vip_rpsp_memd_readbh:
 * Read a buffer header pointer from memd (RPSP). Return it in VIP addressing.
 * Note, if the read returns zero, return NULL.
 */
 #
 # bhdr_t *vip_rpsp_memd_readbh(volatile ushort *)
 #
FRAME(vip_rpsp_memd_readbh,sp,0,ra)
	.set	noreorder

	/*
	 * SIGBUS handler will set v0 to 0 if bus stall occurs
	 * while accessing 0(a0) in the next instruction
	 */
	lhu	v0,0(a0)		# read location (ushort)
	la	t0,ADRSPC_CBUS		# get MEMD base address

	beq	v0,zero,null_rpsp_bhdr	# was it NULL ?
	sll	v0,v0,1			# shift read value left once (x2) (bd)

	j	ra			# return
	or	v0,v0,t0		# or in memd base addr (bd slot)

null_rpsp_bhdr:
	j	ra			# return
	move	v0,zero			# NULL return (bd slot)

	.set	reorder
ENDFRAME(vip_rpsp_memd_readbh)

/*
 * vip_rsp_memd_readbh:
 * Read a buffer header pointer from memd (RSP). Return it in VIP addressing.
 */
 #
 # bhdr_t *vip_rsp_memd_readbh(volatile ulong *)
 #
FRAME(vip_rsp_memd_readbh,sp,0,ra)
	.set	noreorder

	/*
	 * SIGBUS handler will set v0 to 0 if bus stall occurs
	 * while accessing 0(a0) in the next instruction
	 */
	lw	v0,0(a0)		# read location to get buffer header
	la	t0,ADRSPC_CBUS		# get MEMD base address	
	
	li	t1,0x00ffffff		# get lower address bits mask
	beq	v0,zero,null_rsp_bhdr	# did we read zero ?
	and	v0,v0,t1		# mask out RSP upper address bits (bd)

	j	ra			# return
	or	v0,v0,t0		# or in memd base addr (bd slot)

null_rsp_bhdr:
	j	ra			# return
	move	v0,zero			# return NULL (bd slot)

	.set	reorder
ENDFRAME(vip_rsp_memd_readbh)

FRAME(end_memd_read_handlers,sp,0,ra)
ENDFRAME(end_memd_read_handlers)

/* end of file */
