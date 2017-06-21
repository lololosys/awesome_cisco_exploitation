/* $Id: if_pas_epad.h,v 3.1.64.8 1996/09/09 23:34:43 ptran Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_pas_epad.h,v $
 *------------------------------------------------------------------
 * if_pas_epad.h - Platform independent header file for Encryption 
 *			Port Adaptor driver.
 *
 * August 1995, Paulina D. Tran
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_pas_epad.h,v $
 * Revision 3.1.64.8  1996/09/09  23:34:43  ptran
 * CSCdi68463:  ESA driver cleanup
 * Branch: California_branch
 *
 * Revision 3.1.64.7  1996/08/16  00:01:16  ptran
 * Increase the descriptor ring and decrease the holdq size.  Some clean
 * up.
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.6  1996/07/03  20:43:31  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.5  1996/06/29  01:10:50  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.4  1996/06/27  07:04:35  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.3  1996/05/28  18:12:39  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Copy the encryption session control block to descriptor ring entry to
 * reduce number of DMA setup per packet.
 * Uses the PCI mailboxes to post the ring position intead of updating
 * ownership bit.
 * Branch: California_branch
 *
 * Revision 3.1.64.2  1996/04/29  16:59:47  ptran
 * CSCdi56030:  Remove the support of bitflip mode from EPA driver.
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/04/19  15:26:12  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.8  1996/04/17  17:50:04  ptran
 * Increase the size of the descriptor ring to handle larger packet.
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.7  1996/04/16  21:54:56  ptran
 * EPA driver clean up. Removed statistic counters used for debugging
 * purposes.
 * Removed parser commands used for testing. Modified to compile in
 * California
 * branch.  Incorporate code review comments.
 *
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.5  1996/02/14  23:45:06  ptran
 * Changed the default of 4E to use I/O mapped I/O.  This change only
 * effect
 * IosSec_branch.
 * Added parser commands to test the encryption key management.
 * Fixed lost of pak and particles when descriptor error occur.
 * Added statistic counters and parser command to display these
 * statistic.
 *
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.4  1996/02/02  23:14:46  ptran
 * Commit first release of EPA firmware.
 * Branch: IosSec_branch
 *
 * $Log: if_pas_epad.h,v $
 * Revision 3.1.64.8  1996/09/09  23:34:43  ptran
 * CSCdi68463:  ESA driver cleanup
 * Branch: California_branch
 *
 * Revision 3.1.64.7  1996/08/16  00:01:16  ptran
 * Increase the descriptor ring and decrease the holdq size.  Some clean
 * up.
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.6  1996/07/03  20:43:31  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.5  1996/06/29  01:10:50  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.4  1996/06/27  07:04:35  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.3  1996/05/28  18:12:39  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Copy the encryption session control block to descriptor ring entry to
 * reduce number of DMA setup per packet.
 * Uses the PCI mailboxes to post the ring position intead of updating
 * ownership bit.
 * Branch: California_branch
 *
 * Revision 3.1.64.2  1996/04/29  16:59:47  ptran
 * CSCdi56030:  Remove the support of bitflip mode from EPA driver.
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/04/19  15:26:12  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.8  1996/04/17  17:50:04  ptran
 * Increase the size of the descriptor ring to handle larger packet.
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.7  1996/04/16  21:54:56  ptran
 * EPA driver clean up. Removed statistic counters used for debugging
 * purposes.
 * Removed parser commands used for testing. Modified to compile in
 * California
 * branch.  Incorporate code review comments.
 *
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.5  1996/02/14  23:45:06  ptran
 * Changed the default of 4E to use I/O mapped I/O.  This change only
 * effect
 * IosSec_branch.
 * Added parser commands to test the encryption key management.
 * Fixed lost of pak and particles when descriptor error occur.
 * Added statistic counters and parser command to display these
 * statistic.
 *
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.3  1996/01/31  04:48:19  xliu
 * Branch: IosSec_branch
 * Updated epa driver.
 *
 * Revision 3.1.2.2  1996/01/08  19:23:57  ptran
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.1  1995/12/06  21:49:01  rmontino
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/09  12:57:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/23  02:27:37  ptran
 * Placeholder file for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __IF_PAS_EPAD_H__
#define __IF_PAS_EPAD_H__

/*****************************************************************************
 * AMCC's PCI configuration registers 
 *****************************************************************************/
typedef struct amcc_config_regs {
    volatile ushort vid;		/* Vendor Identification */
    volatile ushort did;		/* Device Identification */
    volatile ushort pcicmd;		/* PCI command register */
    volatile ushort pcists;		/* PCI status register */
    volatile ulong rid_clcd;		/* Revision Identification & 
					 * Class Code Register 
					 */
    volatile uchar clcd;		/* cache line size register */
    volatile uchar lat;			/* Master Latency Timer */
    volatile uchar hdr;			/* Header type */
    volatile uchar bist;		/* Build-in Self Test */
    volatile ulong badr0;		/* Base Address Registers (0-5) */
    volatile ulong badr1;
    volatile ulong badr2;
    volatile ulong badr3;
    volatile ulong badr4;
    volatile ulong badr5;
    volatile ulong reserved0;
      volatile ulong reserved1;
    volatile ulong exrom;		/* Expansion ROM Base Address */
    volatile ulong reserved2;
    volatile ulong reserved3;
    volatile uchar intln;		/* Interrupt Line */
    volatile uchar intpin;		/* Interrupt Pin */
    volatile uchar mingnt;		/* Minimum grant */
    volatile uchar maxlat;		/* Maximum Latency */
} amcc_cfg_regs_t;

/*****************************************************************************
 * PCI operation registers 
 *****************************************************************************/
typedef struct amcc_pcibus_op_regs_ {
    volatile ulong omb1;	/* Outgoing Mailbox Register 1 */
    volatile ulong omb2; 	/* Outgoing Mailbox Register 2 */
    volatile ulong omb3;	/* Outgoing Mailbox Register 3 */
    volatile ulong omb4;	/* Outgoing Mailbox Register 4 */
    volatile ulong imb1;	/* Ingoing Mailbox Register 1 */
    volatile ulong imb2;	/* Ingoing Mailbox Register 2 */
    volatile ulong imb3;	/* Ingoing Mailbox Register 3 */		
    volatile ulong imb4;	/* Ingoing Mailbox Register 4 */
    volatile ulong fifo;	/* FIFO Register Port (bidirectional) */
    volatile ulong mwar;	/* Master Write Address Register */
    volatile ulong mwtc;	/* Master Write Transfer Count Register */
    volatile ulong mrar;	/* Master Read Address Register */
    volatile ulong mrtc;	/* Master Read Transfer Count Register */
    volatile ulong mbef;	/* Mailbox Empty/Full Status */
    volatile ulong intcsr;	/* Interrupt Control/Status Register */
    volatile ulong mcsr;	/* Bus Master Control/Status Register */
} amcc_op_regs_t;

/*****************************************************************************
 * PCI  
 *****************************************************************************/
#define EPAD_LATENCY_TIMER_VALUE     0xF800 /* Latency timer value */
#define EPAD_BUS_MSTR_RD_XFER_ENABLE 0x4000     /* Bit 14 */
#define EPAD_BUS_MSTR_WR_XFER_ENABLE 0x0400     /* bit 10 */
#define EPAD_BUS_MSTR_FIFO_RD_MNGM   0x2000	/* Bit 13 */
#define EPAD_BUS_MSTR_FIFO_WR_MNGM   0x0200	/* bit 9 */
#define EPAD_INTCSR_INBOX4_ENABLE    0x00001C00 /* bits 8-12 */
#define EPAD_INTCSR_INBOX4_MASK      0xFFFFECFF /* bits 8-12 */
#define EPAD_INTCSR_INBOX3_ENABLE    0x00001800 /* bits 8-12 */
#define EPAD_INTCSR_INBOX3_MASK      0xFFFFE8FF /* bits 8-12 */
#define AMCC_VENDOR_DEVICE_ID 0x475010E8 /* AMCC Default */ 

/*****************************************************************************
 * Descriptor ring  
 *****************************************************************************/
#define EPAD_FW_OWN  (uchar)0x80 /* Buffer is owned by firmware */

#define EPAD_FIRST   (uchar)0x20 /* first buffer of a packet */
#define EPAD_LAST    (uchar)0x40 /* last buffer of a packet */
#define EPAD_MID     (uchar)0x00 /* middle buffer of a packet */
#define EPAD_ONLY    (uchar)0x60 /* only buffer of a packet */

#define EPAD_NORMAL  0x00 /* status of the operation is normal */
#define EPAD_ERROR   0x0F /* error found in descriptor info */
#define ERR_ZERO_DATA 		0x1  /* data of length zero */
#define ERR_DATA_TOO_LARGE	0x2  /* data length is larger than maximum
				      * size allow
				      */
#define ERR_NUM_BUF		0x4  /* number of buffer error */
#define ERR_BUF_ADDR		0x8  /* buffer address error */


/*****************************************************************************
 * Global limits and defs             
 *****************************************************************************/
#define EPAD_FRAME_DEVICE               0 /* Treat as 2 devices */
#define EPAD_KEY_DEVICE                 1

#define EPAD_FRAME_RING_ENTRIES         128
#define EPAD_FRAME_RING_INDEX_MASK      0x7f
#define EPAD_MAX_FRAME_BUFS_TO_SERVICE  EPAD_FRAME_RING_ENTRIES

#define EPAD_KEY_RING_ENTRIES           8
#define EPAD_KEY_RING_INDEX_MASK        0x07
#define EPAD_MAX_KEY_BUFS_TO_SERVICE    EPAD_KEY_RING_ENTRIES

#define EPAD_RESET_INDEX(index)         (index) = 0
#define EPAD_ADVANCE_INDEX(index,mask)  (index) = (((index) + 1) & (mask));
#define EPAD_RETRACT_INDEX(index,mask)  (index) = (((index) - 1) & (mask));

#define EPAD_FRAME_HOLDQ_SIZE   100
#define EPAD_KEY_HOLDQ_SIZE     15

#define EPAD_FRAME_PARTICLE_SIZE 	32 	/* Size of particles in pool for encryption
                                      header */
#define EPAD_KEY_PARTICLE_SIZE		512

#define EPAD_SND_INTR      0xAABBCCDD /* written to mbx 4, any value ok */
#define EPAD_CNTL_BLK_SIZE	24	/* size of encryption control block */

/***************************************************************************** 
 * Driver specific byffer descriptor structure 
 *****************************************************************************/
typedef struct epad_desc_ {
    volatile uchar control;  /* bit 0-3  Status  0-normal 1-descriptor error 
                                bit 4    Unused
                                bit 5    First buffer in packet
                                bit 6    Last buffer in packet
                                bit 7    Owner 0-driver, 1-firmware */
    volatile uchar numbuf;   /* number of buffer used in this packet */
    volatile ushort length;  /* number of bytes in the buffer */
    volatile uchar *buffer;  /* address of the buffer */
    char cntl_blk[EPAD_CNTL_BLK_SIZE];	/* encryption control block */
} epad_desc_t;

/***************************************************************************** 
 * Driver specific descriptor shadow structure 
 *****************************************************************************/
typedef struct epad_shadow_t_ {
    boolean continuous;		/* FALSE = particles */
    paktype *pak;               /* Continuous buffers go to pak */
    particletype *mp;           /* Particles go to my static pak */
    hwidbtype *outidb;          /* Output IDB */
    ushort flags;               /* Taken from pak, passed to callback */
    ushort conn_id;
    void *if_input;
    char *network_start;
} epad_shadow_t;

/*****************************************************************************
 * Driver instance structure 
 *****************************************************************************/
typedef struct epad_instance_ {
                                         
    pas_cbtype pacb;                    /* Universal PA device control */
                                        /* The pacb->slotunit tells us
                                           we are frame/key "device" */
    amcc_op_regs_t *op_regs; /* Pointer to PCI bus operation register */

    epad_desc_t *beg;	                /* Beginning of the desc ring */
    uint head;                          /* Read from head */
    uint tail;                          /* Write to tail  */
    uint count;                         /* num desc used */
    epad_shadow_t *shadow;	        /* Beginning of the shadow ring */
    
    paktype *pak;			/* My static pak for output */

    ulong num_ring_entries;             /* # of desc in ring */
    ulong max_bufs_serv;                /* Max to process in epad_interrupt */
    ulong adv_idx_mask;                 /* Forces the index to wrap */
    ulong holdq_size;                   /* Capacity of holdq in paks */

    ulong num_snt;                      /* Only for accounting */
    ulong num_rcv;
    ulong bytes_snt;
    ulong bytes_rcv;

    ulong num_too_big;
    ulong num_nohdr;
    ulong num_desc_err;
    ulong num_status_err;
    ulong num_holdq_full;
    ulong num_no_hdr_buffer;
    ulong num_no_buffer;
    ulong num_drop;

    ulong num_hq_enq;			/* Only for debug */
    ulong num_hq_deq;
    ulong num_hq_req;

    void (*callback)(ushort cryptoflags, paktype *pak); 
                                        /* Crypto post process */
    sys_timestamp last_clear;
} epad_instance_t;

/*****************************************************************************
 * PCI mailbox communication
 *****************************************************************************/
#define EPAD_CMD_RESP_OK     0x00000001     /* Ok result code from epa fw */
#define EPAD_CMD_RESP_FAIL   0x00000000

/* msg command */
#define EPAD_CMD_DOWNLOAD_READY      0x1
#define EPAD_CMD_DOWNLOAD            0x2
#define EPAD_CMD_DOWNLOAD_REPORT     0x3
#define EPAD_CMD_DOWNLOAD_DONE       0x4
#define EPAD_CMD_DOWNLOAD_EXEC       0x5
#define EPAD_CMD_INIT_BLOCK_HIGH     0x6
#define EPAD_CMD_INIT_BLOCK_LOW      0x7
#define EPAD_SND_HI                  0x8
#define EPAD_SND_LOW                 0x9

/* empty/full status register */
#define OMB_FULL  0x0000ffff	/* all PCI 4 outgoing mailboxes are full */
#define OMB1_FULL 0x0000000f	/* Outgoing Mailbox 1 is full */
#define OMB2_FULL 0x000000f0	/* Outgoing Mailbox 2 is full */
#define OMB3_FULL 0x00000f00	/* Outgoing Mailbox 3 is full */
#define OMB4_FULL 0x0000f000	/* Outgoing Mailbox 4 is full */

#define IMB_FULL  0xffff0000	/* all PCI 4 incomming mailboxes are full */
#define IMB1_FULL 0x000f0000	/* Incomming Mailbox 1 is full */
#define IMB2_FULL 0x00f00000	/* Incomming Mailbox 2 is full */
#define IMB3_FULL 0x0f000000	/* Incomming Mailbox 3 is full */
#define IMB4_FULL 0xf0000000	/* Incomming Mailbox 4 is full */

#define EPAD_READ_RETRY	2000000
typedef struct epacmd_ {
    char        call_id;
    char        status;
} epacmd_t;

/*****************************************************************************
 * Externs
 *****************************************************************************/
extern pa_wrapper_t epad_wrapper;
extern hwidbtype *epad_gidb[2];
extern parser_extension_request epadtest_chain_init_table[];
extern void epa_nmi_handler (void *);
extern void epad_fastsend (hwidbtype *idb, paktype *pak);
extern void epad_platform_init(hwidbtype *);

/* firmware specific variables initialized in epad_fw.c */
extern unsigned int EPAD_FW_NUM_REGIONS;
extern unsigned int EPAD_FW_CRC;
extern unsigned long epad_fw_load_addr[];
extern unsigned char *epad_fw_code[];
extern unsigned long epad_fw_code_size[];

/* crypto engine call back */
/*extern void crypto_register_hwidb (hwidbtype *idb, ulong hwversion, ulong serialnum);*/
extern void crypto_register_hwidb (int device, hwidbtype *idb);
extern void crypto_engine_callback (ushort type, paktype *pak);
extern void crypto_engine_key_callback (ushort type, paktype *pak);

#endif /*__IF_PAS_EPAD_H__*/
