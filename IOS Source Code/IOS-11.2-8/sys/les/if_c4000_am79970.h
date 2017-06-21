/* $Id: if_c4000_am79970.h,v 3.3 1995/11/17 17:38:46 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/if_c4000_am79970.h,v $
 *------------------------------------------------------------------
 * if_c4000_am79970.h - am79970 specific defines.
 *
 * Aug 1994, Manoj Leelanivas
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c4000_am79970.h,v $
 * Revision 3.3  1995/11/17  17:38:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:55  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/14  21:37:57  yoko
 * CSCdi35809:  Program Guinness LEDs
 * Enable Link status, RX, and TX LEDs
 *
 * Revision 2.1  1995/06/07  21:33:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_C4000_AM79970_H__
#define __IF_C4000_AM79970_H__

#define AM79970_VENDOR_DEVICE_ID 	0x20001022

/*
 * The BCR definitions which are relevant to AM79970.
 */

/*
 * BCR2 is the media specific bus configuration register.
 */
#define AM79970_BCR2	2
#define AM79970_BCR2_ASEL_ENABLE  0x0001	/* default is enabled*/
#define AM79970_BCR2_ASEL_DISABLE 0x0000

/*
 * Led specific.
 */
#define AM79970_BCR4	4                 /* bottom (Link status) LED */
#define AM79970_BCR4_LINKSTE  0x0040
#define AM79970_BCR4_LEDOUT   0x8000

#define AM79970_BCR5	5                 /* mid (RX) LED for Rev1 NIM */
#define AM79970_BCR5_RX       0x0007 

#define AM79970_BCR6	6                 /* mid (RX) LED for Rev2 NIM */
#define AM79970_BCR6_RX       0x0004

#define AM79970_BCR7	7                 /* top (TX) LED */
#define AM79970_BCR7_TX       0x0010

/*
 * Full duplex control
 */
#define AM79970_BCR9	9
#define AM79970_BCR9_FDEN	0x0001

/*
 * BCR18 controls the burst size/type of the bus interface.
 */
#define AM79970_BCR18	18
#define AM79970_BCR18_MEMCMD	0x0200
#define AM79970_BCR18_EXTREQ    0x0100
#define AM79970_BCR18_DWIO   	0x0080
#define AM79970_BCR18_BREADE 	0x0040
#define AM79970_BCR18_BWRITE 	0x0020
#define AM79970_BCR18_LINBC 	0x0001

#define AM79970_BCR19	19

/*
 * BCR20 is the register needed for setting the AM79970 in
 * lance compatible mode.
 */
#define AM79970_BCR20	20
#define AM79970_BCR20_LANCE_SW_STYLE 		0x0000
#define AM79970_BCR20_CSRPCNET       		0x0200
#define AM79970_BCR20_SIZE32       		0x0100
#define AM79970_BCR20_AM79970_SW_STYLE       	0x0103

#define AM79970_BCR22	22

/*
 * The CSR definitions which are relevant to AM79970.
 */

/*
 * AM79970_CSR3
 */
#define AM79970_CSR3    3
#define AM79970_CSR3_DXSUFLO            0x0040

/*
 * AM79970_CSR4 bus interface. We do not use any interrupt bits in
 * this register; mask all interrupts.
 */
#define AM79970_CSR4	4
#define AM79970_CSR4_DMAPLUS		0x4000
#define AM79970_CSR4_AUTO_PAD_XMT   	0x0800
#define AM79970_CSR4_AUTO_STRP_RCV  	0x0400
#define AM79970_CSR4_MFCOM		0x0100
#define AM79970_CSR4_RCVCCOM		0x0020
#define AM79970_CSR4_TXSTRTM   	        0x0004
#define AM79970_CSR4_JABM   		0x0001

/*
 * AM79970_CSR5 bus interface. The error bits are pretty
 * important and if ever we decide to check for them in
 * the wrapper....
 */
#define AM79970_CSR5	5
#define AM79970_CSR5_SINT	 	0x0800
#define AM79970_CSR5_SINT_ENABLE    	0x0400
#define AM79970_CSR5_SUSPEND    	0x0001

/* 
 * AM79970_CSR80 DMA transfer counter and FIFO  
 * threshold control
 */
#define AM79970_CSR80   80

/*
 * The AM79970 mode register bits.
 * Note that these register bits have to be used along
 * with the LANCE bits.
 */
#define AM79970_DRCVBC	       0x4000
#define AM79970_DRCVPA	       0x2000
#define AM79970_DLNKTST        0x1000
#define AM79970_DAPC   	       0x0800
#define AM79970_MENDECL        0x0400
#define AM79970_SQUELCH        0x0200     /* should stop the lance */
#define AM79970_PORTSEL        0x0010     /* choose 10baset with ASEL = 0 */

/*
 * The AM79C970A specific registers
 */
struct eim_am79970_regs {
    volatile unsigned short aprom[8];
    volatile unsigned short rdp;
    volatile unsigned short rap;
    volatile unsigned short reset;
    volatile unsigned short bdp;
};

/*
 * The AM79C970A specific stuff
 * as well as a Lance instance, since the chip is programmed in the
 * Lance compatible mode. Note that the rdp and rap will
 * be mapped into the lance instance.
 */
typedef struct am79970_instance am79970_instance_t;
struct am79970_instance {
    struct eim_am79970_regs 	*am79970_regaddr;
    lance_instance_t            *lance_ds;
    boolean link_status;
    boolean squelch_status;
};

/*
 * Product specific.
 */
extern void am79970_set_vectors(hwidbtype *);

/*
 * Macro for accessing instance.
 */
#define AM79970_INSTANCE(slot, device_no)  ((PCI_NIM_INSTANCE(slot)) ? \
    PCI_NIM_INSTANCE(slot)->device_ds[device_no] : NULL)
#endif






