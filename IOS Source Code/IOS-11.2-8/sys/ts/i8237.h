/* $Id: i8237.h,v 3.2 1995/11/17 18:54:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/i8237.h,v $
 *------------------------------------------------------------------
 * i8237.h - Intel 8237 Programmable DMA Controller Bit Definitions 
 *
 * Copyright (c) 1986-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: i8237.h,v $
 * Revision 3.2  1995/11/17  18:54:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:09:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Command Register Definitions */
#define NormalMode 	0   	/* memory to memory transfer disable */
#define MemtoMemEn  	0x1   	/* memory to memory transfer enable */
#define Chn0AddHoldDis  0   	/* Channel 0 address hold disable */
#define Chn0AddHoldEn   0x2   	/* Channel 0 address hold enable */
#define ContEnable    	0   	/* Controller Enable */
#define ContDisable  	0x4 	/* Controller Disable */
#define NormalTiming	0   	/* Normal timing */
#define CompTiming  	0x8 	/* Compressed timing */
#define FixedPri    	0   	/* Fixed priority */
#define RotatingPri 	0x10	/* Rotating priority */
#define LateWrite   	0   	/* Late write selection */
#define ExtWrite    	0x20	/* Extended write selection */
#define DREQhi	    	0   	/* DREQ sense active high */
#define DREQlo	    	0x40   	/* DREQ sense active low */
#define DACKlo	    	0   	/* DACK sense active low */
#define DACKhi	    	0x80   	/* DACK sense active high */

/* Mode Register Definitions */
#define Chn0Select  	0   	/* Channel 0 select */
#define Chn1Select  	0x1   	/* Channel 1 select */
#define Chn2Select  	0x2   	/* Channel 2 select */
#define Chn3Select  	0x3   	/* Channel 3 select */
#define VerifyXfer  	0   	/* Verify transfer */
#define WriteXfer   	0x4 	/* Write transfer */
#define ReadXfer    	0x8 	/* Read transfer */
#define AutoInitDis 	0   	/* Autoinitialization disable */
#define AutoInitEn 	0x10   	/* Autoinitialization enable */
#define AddrInc	    	0   	/* Address increment select */
#define AddrDec	    	0x20	/* Address decrement select */
#define DemandMode  	0   	/* Demand mode select */
#define SingleMode  	0x40	/* Single mode select */
#define BlockMode   	0x80	/* Block mode select */
#define CascadeMode 	0xC0 	/* Cascade mode select */

/* Request Register Definitions */
#define RSelectChn0   	0    	/* Select channel 0 */
#define RSelectChn1   	0x1    	/* Select channel 1 */
#define RSelectChn2   	0x2   	/* Select channel 2 */
#define RSelectChn3   	0x3    	/* Select channel 3 */
#define ResRqBit    	0   	/* Reset request bit */
#define SetRqBit    	0x4 	/* Set request bit */

/* Set/Reset Mask Register Definitions */
#define MSelectChn0   	0    	/* Select channel 0 */
#define MSelectChn1   	0x1    	/* Select channel 1 */
#define MSelectChn2   	0x2   	/* Select channel 2 */
#define MSelectChn3   	0x3    	/* Select channel 3 */
#define ResMaskBit    	0   	/* Reset mask bit */
#define SetMaskBit    	0x4 	/* Set mask bit */

/* Write Mask Register Definitions */
#define ClrChn0MB    	0    	/* Clear channel 0 mask bit */
#define SetChn0MB    	0x1    	/* Set channel   0 mask bit */
#define ClrChn1MB    	0    	/* Clear channel 1 mask bit */
#define SetChn1MB    	0x2    	/* Set channel   1 mask bit */
#define ClrChn2MB    	0    	/* Clear channel 2 mask bit */
#define SetChn2MB    	0x4    	/* Set channel   2 mask bit */
#define ClrChn3MB    	0    	/* Clear channel 3 mask bit */
#define SetChn3MB    	0x8    	/* Set channel   3mask bit */

/* Status Register Definitions */
#define Chn0TC	    	0x1   	/* Channel 0 has reached TC */
#define Chn1TC	    	0x2   	/* Channel 1 has reached TC */
#define Chn2TC	    	0x3   	/* Channel 2 has reached TC */
#define Chn3TC	    	0x8   	/* Channel 3 has reached TC */
#define Chn0Request  	0x10 	/* Channel 0  request */
#define Chn1Request  	0x20 	/* Channel 1  request */
#define Chn2Request  	0x40 	/* Channel 2  request */
#define Chn3Request  	0x80 	/* Channel 3  request */

