/* $Id: mfp.h,v 1.1.4.1 1996/05/09 15:01:13 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/mfp.h,v $
 *------------------------------------------------------------------
 * mfp.h -- Definitions for using the MultiFunction Peripheral (MFP)
 *          chip.  The MFP (MC68901) is a Motorola part containing
 *          USARTs and timers.
 *
 * ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Copyright (c) 1990 Xlnt Designs Inc XDI
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mfp.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:13  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:12  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:36  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:21  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:54:07  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:40  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
							mfp.h

	Definitions for using the MultiFunction Peripheral (MFP) chip.  The 
	MFP (MC68901) is a Motorola part containing USARTs and timers.

	Created 12-27-90
	RSM
*/
#ifndef _MFP_H_
#define _MFP_H_

/*
 *	Register offsets.
 * 
 *	On the ATM board, the MFP is connected to D[24..31] and the address 
 *      is continuous, i.e. the least significant bit of the register offset is
 *	tied to A0
 */
#define MFP_GPDR	 (0x00 ) 
#define MFP_AER	 	 (0x01 )
#define MFP_DDR		 (0x02 )
#define MFP_IERA	 (0x03 )
#define MFP_IERB	 (0x04 )
#define MFP_IPRA	 (0x05 ) 
#define MFP_IPRB	 (0x06 )
#define MFP_ISRA	 (0x07 )
#define MFP_ISRB	 (0x08 ) 
#define MFP_IMRA	 (0x09 )
#define MFP_IMRB	 (0x0a )
#define MFP_VR		 (0x0b ) 
#define MFP_TACR	 (0x0c )
#define MFP_TBCR	 (0x0d ) 
#define MFP_TCDCR	 (0x0e )
#define MFP_TADR	 (0x0f ) 
#define MFP_TBDR	 (0x10 ) 
#define MFP_TCDR	 (0x11 )
#define MFP_TDDR	 (0x12 ) 
#define MFP_SCR		 (0x13 )
#define MFP_UCR		 (0x14 )
#define MFP_RSR		 (0x15 ) 
#define MFP_TSR		 (0x16 )
#define MFP_UDR		 (0x17 ) 

/*
	General Purpose I/O Data Register (GPDR), Active Edge Register (AER), and
	Data Direction Register (DDR) bit defintions.
*/
#define GPIP7_BIT	0x80
#define GPIP6_BIT	0x40
#define GPIP5_BIT	0x20
#define GPIP4_BIT	0x10
#define GPIP3_BIT	0x08
#define GPIP2_BIT	0x04
#define GPIP1_BIT	0x02
#define GPIP0_BIT	0x01

/*
	Defintions for the vector register (VR).  

	The upper four bits are used to specify vector number to processor,
	the lower four bits are the interrupt reason.
*/
#define INT_GPIP7		0x0f
#define INT_GPIP6		0x0e
#define INT_TIMER_A		0x0d
#define INT_RCV_BUF_FUL		0x0c
#define INT_RCV_ERROR		0x0b
#define INT_XMIT_BUF_EMPTY	0x0a
#define INT_XMIT_ERROR		0x09
#define INT_TIMER_B		0x08
#define INT_GPIP5		0x07
#define INT_GPIP4		0x06
#define INT_TIMER_C		0x05
#define INT_TIMER_D		0x04
#define INT_GPIP3		0x03
#define INT_GPIP2		0x02
#define INT_GPIP1		0x01
#define INT_GPIP0		0x00

/*
	Interrupt Enable Register A (IERA), Interrupt Mask Register A (IMRA),
	and Interrupt In-Service Register A (ISRA).
*/
#define EN_DUART		0x80
#define EN_CAM			0x40
#define EN_TIMER_A		0x20
#define EN_RCV_BUF_FULL		0x10
#define EN_RCV_ERROR		0x08
#define EN_XMIT_BUF_EMPTY	0x04
#define EN_XMIT_ERROR		0x02
#define EN_TIMER_B		0x01

/*
 *	Interrupt Enable Register B (IERB), Interrupt Mask Register B (IMRB),
 *	and Interrupt In-Service Register B (ISRB).
 */
#define EN_DMP			0x80
#define EN_MAC_INT2		0x40
#define EN_TIMER_C		0x20
#define EN_TIMER_D		0x10
#define EN_MAC_INT1		0x08
#define EN_PHYB			0x04
#define EN_PHYA			0x02
#define EN_SONIC_T		0x01

/*
 *	Timer Control Registers A,B and CD (TACR, TBCR,TCDCR).  Timer control
 *	register A and B are identical, TCDCR serves both timer C and timer D.
 *	Because A and B have more functionality they need a seperate register.
 *	For exact usage of these registers please refer to the HW spec.
 */
#define TIMER_AB_RESET		0x10
#define TIMER_AB_PULSE_200	0x0f
#define TIMER_AB_PULSE_100	0x0e
#define TIMER_AB_PULSE_64	0x0d
#define TIMER_AB_PULSE_50	0x0c
#define TIMER_AB_PULSE_16	0x0b
#define TIMER_AB_PULSE_10	0x0a
#define TIMER_AB_PULSE_4	0x09
#define TIMER_EVENT_COUNT_MODE	0x08

#define TIMER_DELAY_MODE_200	0x07
#define TIMER_DELAY_MODE_100	0x06
#define TIMER_DELAY_MODE_64	0x05
#define TIMER_DELAY_MODE_50	0x04
#define TIMER_DELAY_MODE_16	0x03
#define TIMER_DELAY_MODE_10	0x02
#define TIMER_DELAY_MODE_4	0x01

/*
 *	USART Control Reigister (UCR)
 */
#define CLK_16			0x80
#define CLK_8_BIT_CHARS		0x00
#define CLK_7_BIT_CHARS		0x20
#define CLK_6_BIT_CHARS		0x40
#define CLK_5_BIT_CHARS		0x60

#define SYNC_MODE		0x00
#define ASYNC_1_STOP_BIT	0x08
#define ASYNC_1_HALF_STOP_BIT	0x10
#define ASYNC_2_STOP_BITS	0x18

#define PARITY_ENABLE		0x04
#define EVEN_PARITY		0x02
#define ODD_PARITY		0x00

/*
 * 	Receiver Status Register (RSR)
 */
#define RCV_BUFFER_FULL		0x80
#define RCV_OVERRUN_ERROR	0x40
#define RCV_PARITY_ERROR	0x20
#define RCV_FRAME_ERROR		0x10
#define RCV_BREAK_DETECT	0x08
#define RCV_CHAR_IN_PROGRESS	0x04
#define RCV_SYNC_STRIP		0x02
#define RECEIVER_ENABLE		0x01

/*
 *	Transmitter Status Register (TSR).
 */
#define XMIT_BUFFER_EMPTY	0x80
#define XMIT_UNDERRUN_ERROR	0x40
#define XMIT_AUTO_TURN		0x20
#define XMIT_END_OF_XMIT	0x10
#define XMIT_BREAK_ENABLE	0x08
#define XMIT_LOOPBACK_MODE	0x06
#define XMIT_HIGH		0x04
#define XMIT_LOW		0x02
#define XMIT_HIGH_IMPEDANCE	0x00
#define TRANSMITTER_ENABLE	0x01
/*#define MAX_TIMER_DVAL	0xff	 max timer value */

#endif  /* _MFP_H_ */

#define MFP_BASE_VECTOR			64
