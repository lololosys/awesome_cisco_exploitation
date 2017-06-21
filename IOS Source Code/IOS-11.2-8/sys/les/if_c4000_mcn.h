/* $Id: if_c4000_mcn.h,v 3.3.62.2 1996/06/16 21:15:52 vnguyen Exp $ 
 * $Source: /release/112/cvs/Xsys/les/if_c4000_mcn.h,v $
 *------------------------------------------------------------------
 * if_c4000_mcn.h -- The Multi channel NIM defines for the 4xxx.
 *
 * March 1994, Manoj Leelanivas.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_c4000_mcn.h,v $
 * Revision 3.3.62.2  1996/06/16  21:15:52  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.3.62.1  1996/03/18  20:41:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  22:23:47  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  17:39:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:47  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:34:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**************************************************************************
  The REGISTER MAP of the Multi-chanel NIM
  ----------------------------------------
  slot address + 0000h        NIM type
               + 0002h        NIM rev Level
	       + 0004h        adapter type
	       + 0006h        reset control
	       + 0008h        m32 int vector
	       + 000Ah        adapter #1 int vector
	       + 000Ch        adapter #2 int vector
	       + 000Eh        led register

  slot address + 0010h        125 us tick timer high(only lower 2 bits valid)
               + 0012h        125 us tick timer low
               + 0014h        timer enable
	       + 0016h        timer status and clear
	       + 0018h        timer overflow

  slot address + 0040h        start of m32 specific registers

  slot address + 0080h        start of T1 specific registers

  slot address + 0100h        start of multi-bri specific registers

  slot address + 0200h        start of E1 specific registers

  slot address + 0800h - 09FFh reserved for T1 framer
  slot address + 1800h - 1BFFh reserved for T1 framer

  **************************************************************************/

#ifndef IF_C4000_MCN_H
#define IF_C4000_MCN_H

#if defined(XX) | defined(SIERRA)

#define BITS(pos)                  (((ushort) 1) << (pos))
#define BITL(pos)                  (((ulong) 1) << (pos))

#define MAX_MCN_SUBCONT_PER_SLOT       1
#define C4000_MCN_UNITS            (MAX_MCN_SUBCONT_PER_SLOT * C4000_NUM_SLOTS)
/*
 * The offsets to different register blocks from the slot base.
 */
#define MCN_REG_OFFSET         	0x0000
#define MCN_COUNTER_REG_OFFSET	0x0010
#define MCN_M32_REG_OFFSET(subcont) (0x0040)  /* the subcont will be used for the future */
#define MCN_MBRI_REG_OFFSET	0x0100

/*
 * the MCN supports the following port adaptors
 */
#define MCN_EMPTY 0            /* no port adaptor plugged into the mcn */
#define MCN_PA_BRI_SINGLE 1    /* 1 BRI port adaptor (4 ports) */
#define MCN_PA_T1_U1 3         /* T1 port adaptor with 1 m32 */
#define MCN_PA_E1_U1 4         /* E1 port adaptor with 1 m32 */
#define MCN_PA_BRI_DUAL 0x11   /* 2 BRI port adaptors (8 ports) */

#define MCN_PA_T1    0x0e0     /* common T1 adapter type */
#define MCN_PA_E1    0x0e1     /* common E1 adapter type */

/*
 * the MCN reset control masks
 */
#define MCN_M32_RESET_MASK    0x01
#define MCN_M32_ENABLE_MASK   0xfe
#define MCN_ADAPTER1_RESET_MASK   0x02
#define MCN_ADAPTER1_ENABLE_MASK  0xfd
#define MCN_ADAPTER2_RESET_MASK   0x04
#define MCN_ADAPTER2_ENABLE_MASK  0xfb
#define MCN_ENABLE_MASK	      0x0000
#define MCN_LED_DISABLE       0x00ff

/* The MCN registers of the NIM */
typedef struct mcn_block mcn_block_t;
struct mcn_block {
    volatile ushort nim_type;		/* nim type*/
    volatile ushort nim_revision;	/* revision # */
    volatile ushort adapter_type;	/* which adapter MBRI, E1 or T1 */
    volatile ushort reset_control;	/* reset controls for m32 & adapters */
    volatile ushort m32_vector;
    volatile ushort adapter1_vector;
    volatile ushort adapter2_vector;
    volatile ushort led_control;	/* control to light up leds */
};

/*
 * The defines related to the 125 usec tick timer.
 */
#define MCN_COUNTER_ENABLE  0x0001	/* interrupt enable */
#define MCN_COUNTER_DISABLE 0x0000	/* interrupt disable */
#define MCN_MAX_TIMER_ENTRIES 50	/* max. preallocated elements
					   for the deltaQ */


/* Counter register structure common to the NIM*/
typedef struct mcn_counter_block mcn_counter_block_t;
struct mcn_counter_block {
    volatile ushort ctr_high;	      /* counter register high.tick = 125 us */
    volatile ushort ctr_low;	      /* counter register low. tick = 125 us */
    volatile ushort int_enable;	      /* write a 1 to bit 0 will enable int */
    volatile ushort status;	      /* read a 1 in bit 0 =>service needed */
    volatile ushort overflow;	      /* overflow after interrupt */
};


/*
 * Timer queue element to handle multiple timeout requests coming from
 * different interfaces. ie, a deltaQ element.
 */
typedef struct mcn_timerbuf mcn_timerbuf_t;
struct mcn_timerbuf {
    struct mcn_timerbuf *next;	/* next entry */
    ushort slot;		/* slot # */
    ushort subunit;		/* interface # */
    ulong sequence;             /* Identity of this element, in case u 
                                   want to selectively deq it */
    ulong timeout;		/* basically the timeout needed. */
    ulong timeout_type;		/* which type of timeout */
    ulong delta;		/* delta for timeout relative to prev element*/
};

typedef struct m32_instance m32_instance_t;
typedef struct mb_instance  mb_instance_t;
typedef struct dsx1_instance  dsx1_instance_t;

typedef struct m32_block    m32_block_t;
typedef struct mb_block     mb_block_t;

/* The Multi-channel NIM instance */
typedef struct mcn_instance mcn_instance_t;

typedef struct mcn_subinstance_t_ {
    m32_instance_t *m32_ds;         /* Munich32 instance structure */
    union {
        dsx1_instance_t *dsx1;          /* E1 ot T1 instance structure */
        mb_instance_t *mb_ds;           /* Multi BRI instance structure */
    }info;
} mcn_subinstance_t;

struct mcn_instance {
    mcn_block_t *mcn_regs;          /* Ptr to NIM slot base */
    ulong mcn_cardtype;             /* Installed port adapter type    */
    ulong total_subcont;            /* the number of subconts installed */
    mcn_counter_block_t *ctr_regs;  /* Ptr to counter registers */
    mcn_timerbuf_t	*q_head;    /* The head of the deltaQ for this MCN */
    mcn_subinstance_t mcn_subcont[MAX_MCN_SUBCONT_PER_SLOT];
};


/*
 * Inline statements 
 */
inline static void mcn_reset_m32(mcn_instance_t *mcn_instance)
{
   mcn_instance->mcn_regs->reset_control = MCN_M32_RESET_MASK;
}


/* ---------------------------------
 * macros to get instance structures
 * --------------------------------- */
/*
 * macro to get the mcn instance structure using the slot
 */
#define SINGLE_SUBCONT          0
#define MCN_INSTANCE(slot)  (mcn_ds[slot])
#define M32_INSTANCE(slot,subcont)  ((mcn_ds[slot]) ? \
                                     mcn_ds[slot]->mcn_subcont[subcont].m32_ds : NULL)
#define MB_INSTANCE(slot,subcont)   ((mcn_ds[slot]) ? \
                                     mcn_ds[slot]->mcn_subcont[subcont].info.mb_ds : NULL)
#define DSX1_INSTANCE(slot,subcont) ((mcn_ds[slot]) ? \
                                     mcn_ds[slot]->mcn_subcont[subcont].info.dsx1 : NULL)
#define DELAY_ACTION_REQUEST()
/*
 * Prototypes
 */
uint mcn_get_adapter_type(uint);
void mcn_set_vectors(uint);
void mcn_deltaQ_enQ(uint, mcn_timerbuf_t *);
mcn_timerbuf_t *mcn_deltaQ_deQ_head(uint);
mcn_timerbuf_t *mcn_deltaQ_deQ_element(uint, uint);

/*
 * external references and declarations.
 */
extern queuetype *McnTimerPoolQ;
extern mcn_instance_t *mcn_ds[];
extern void dsx1_set_runts_handle(hwidbtype *idb);
#endif
#endif		/* to prevent multiple inclusion*/



