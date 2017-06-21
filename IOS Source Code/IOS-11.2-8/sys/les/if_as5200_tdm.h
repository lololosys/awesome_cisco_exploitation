/* $Id: if_as5200_tdm.h,v 3.1.72.3 1996/08/28 12:57:19 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_as5200_tdm.h,v $
 *------------------------------------------------------------------
 * if_as5200_tdm.h -- definitions for Mitel MT8980/MT8985
 *      TDM (Time Division Multiplexing) chip
 *
 * October 1995, Joe Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_as5200_tdm.h,v $
 * Revision 3.1.72.3  1996/08/28  12:57:19  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.72.2  1996/07/02  23:55:18  jturner
 * CSCdi61176:  post-brasil-commit code cleanup in tdm, asm, src and
 * makefile
 * Branch: California_branch
 *
 * Revision 3.1.72.1  1996/06/16  21:15:02  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/03  21:26:48  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * TDM Enumerations
 */
typedef enum {
	TDM_ISDN_STREAM,
	TDM_PRI_STREAM,
	TDM_MODEM_STREAM,
	TDM_DIGIT_COLLECT_STREAM,
	TDM_TONE_STREAM,
        TDM_STREAM_MAX_LENGTH
} stream_type_t;

typedef enum {
	ST0,
	ST1,
	ST2,
	ST3,
	ST4,
	ST5,
	ST6,
	ST7
} stream_t;

typedef enum {
    DIALTONE_RX,	/* Used for connect_special dialtone connect */
    BUSYTONE_RX,	/* Used for connect_special busytone connect */
    DEFAULT_RX,		/* Used for connect_special tone connection */
    DEFAULT_TX,		/* Used for connect_special digitcollect disconnect */
    DEFAULT_RX_TX 	/* Used for connect_common disconnect */
} special_chan_t;

/*
 * The Brasil MT8980/MT8985 TDM register structure.
 * A word about alignment (pun intended).   All  structure  elements
 * are  word  aligned,  so device registers that in reality are only
 * eight bits wide, actually consume two byte addresses in  the  I/O
 * map and therefore are defined as shorts.
 */
typedef struct tdm_regs_t tdm_regs;
struct tdm_regs_t {
    volatile unsigned short	control_reg;
    unsigned short		pad[31];
    volatile unsigned short	channel[32];
    volatile unsigned short	ode_reg;
};

typedef struct ch_info_t_ ch_info_t;
struct ch_info_t_ {
    unsigned int	slot;		/* Physical card slot */
    unsigned int	unit;		/* Interface # or digit_collector # */
    stream_type_t	stream_type;	/* ISDN, PRI, MODEM, DIGIT_COLLECT... */
    unsigned int	stream;		/* Stream: st0, st1, st2... */
    unsigned long	channel;	/* Timeslot to be connected */
    boolean		channel_group_used;	/* For fraction T1/E1 */
};

/*
 * General TDM defines
 */
#define TDM_MAX_STREAMS			8
#define TDM_MAX_CHANNELS		32
#define TDM_T1_MAX_CHANNELS		24
#define TDM_E1_MAX_CHANNELS		32
#define TDM_ODE_ENABLE			0x0001
#define TDM_ODE_DISABLE			0x0000
#define TDM_CH_OUTPUT_ENABLE		0x01
#define TDM_CH_OUTPUT_DISABLE		0x00
#define TDM_OFFSET			0x100
#define TDM_MOTHERBOARD_SLOT		0xffffffff
#define TDM_MASK			0xff

/*
 * Digit collection defines
 * Brasil has 2 digit collectors.
 */
#define TDM_DC_STREAM			ST3
#define TDM_DC1_CARD_CH			0
#define TDM_DC2_CARD_CH			2
#define TDM_DC1_BP_CH			4
#define TDM_DC2_BP_CH			5

/*
 * Dialtone defines
 */
#define TDM_DIALTONE_STREAM		ST3
#define TDM_DIALTONE_CARD_CH		0
#define TDM_DIALTONE_BP_CH		1

/*
 * Busytone defines
 */
#define TDM_BUSYTONE_STREAM		ST3
#define TDM_BUSYTONE_CARD_CH		1
#define TDM_BUSYTONE_BP_CH		3

/*
 * Control Register bits
 */
#define	TDM_SPLIT_MEMORY		0x80
#define	TDM_MESSAGE_MODE		0x40
#define	TDM_MEM_SELECT1			0x10
#define	TDM_MEM_SELECT0			0x08
#define TDM_STREAM_ADDRESS_BIT2		0x04
#define TDM_STREAM_ADDRESS_BIT1		0x02
#define TDM_STREAM_ADDRESS_BIT0		0x01
#define TDM_STREAM_ADDRESS_BITS		(TDM_STREAM_ADDRESS_BIT0 | \
					 TDM_STREAM_ADDRESS_BIT1 | \
					 TDM_STREAM_ADDRESS_BIT2)
#define TDM_CHANNEL_BITS		0x1f
#define TDM_DATA_MEMORY			0x08
#define TDM_CONNECTION_MEMORY_LOW	0x10
#define TDM_CONNECTION_MEMORY_HIGH	0x18

/*
 * Connection Memory High Bits
 * Please note that bit 6, the V/C delay mode bit does not exist  on
 * the MT8980.  Actually, the only difference between the MT8980 and
 * the MT8985 is the V/C delay bit.
 */
#define	TDM_CMEM_H_VC_DELAY_MODE	0x40
#define TDM_CMEM_H_MESSAGE_CHANNEL	0x04
#define TDM_CMEM_H_CST0_BIT		0x02
#define TDM_CMEM_H_OUTPUT_ENABLE	0x01
#define TDM_CMEM_H_OUTPUT_DISABLE	0x0

/*
 * Connection Memory Low Bits
 */
#define	TDM_CMEM_L_ST_ADDR2	0x80
#define	TDM_CMEM_L_ST_ADDR1	0x40
#define	TDM_CMEM_L_ST_ADDR0	0x20
#define	TDM_CMEM_L_CH_ADDR4	0x10
#define	TDM_CMEM_L_CH_ADDR3	0x08
#define TDM_CMEM_L_CH_ADDR2	0x04
#define TDM_CMEM_L_CH_ADDR1	0x02
#define TDM_CMEM_L_CH_ADDR0	0x01

#define TDM_CMEM_L_ST_SHIFT	5

/*
 * Useful TDM macros
 */
#define TDM_ADDRESS(slot) \
        (tdm_regs *)(ADRSPC_BRASIL_BASE_SLOT + \
        (BRASIL_SLOT_OFFSET * (slot)) + TDM_OFFSET)

#define TDM_SIZE (sizeof(tdm_regs))

/*
 * External declarations:
 */
 
extern boolean tdm_debug;     /* debug flag for tdm */
 
/*
 * Prototypes
 */
tdm_regs *tdm_getregs(ch_info_t *tdm);

/*
 * Externs
 */
extern void tdm_connect_dc(ch_info_t *modem_tx, ch_info_t *digit_collect);
extern void tdm_disconnect_dc(ch_info_t *modem_tx, ch_info_t *digit_collect);
extern boolean tdm_connect_twoway(ch_info_t *tdm_in, ch_info_t *tdm_out);
extern boolean tdm_connect_special(special_chan_t ch_type, ch_info_t *dev_ch);
extern void tdm_debug_init(void);
extern void tdm_parser_init(void);
