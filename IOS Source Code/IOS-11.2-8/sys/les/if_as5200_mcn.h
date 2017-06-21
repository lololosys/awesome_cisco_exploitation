/* $Id: if_as5200_mcn.h,v 3.1.64.2 1996/08/28 12:57:09 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_as5200_mcn.h,v $
 *------------------------------------------------------------------
 * if_as5200_mcn.h -- The Multi channel NIM defines for the as52xx.
 *
 * December, 1995 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_as5200_mcn.h,v $
 * Revision 3.1.64.2  1996/08/28  12:57:09  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.64.1  1996/06/16  21:14:50  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/19  23:48:00  jchlin
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**************************************************************************
  The REGISTER MAP of the Multi-chanel NIM
  ----------------------------------------
  slot address + 0000h        Board ID
               + 0002h        Board rev Level
	       + 0004h        Reserved Configuration
	       + 0006h        PRI control 1
	       + 0008h        PRI status
	       + 000Eh        PRI control 2

  slot address + 0010h        Page register

  slot address + 0020h        start of m32 1 specific registers

  slot address + 0030h        start of m32 2 specific registers

  slot address + 0100h        start of M8980 registers

  slot address + 0180h        M8980 ODE

  **************************************************************************/

#ifndef IF_AS5200_MCN_H
#define IF_AS5200_MCN_H

#if defined(BRASIL)

#define BITS(pos)                  (((ushort) 1) << (pos))
#define BITL(pos)                  (((ulong) 1) << (pos))

#define MAX_DTMF_UNIT_PER_SLOT         2
#define MAX_MCN_SUBCONT_PER_SLOT       2
#define AS5200_MCN_UNITS            (MAX_MCN_SUBCONT_PER_SLOT * AS5200_MAX_SLOTS)
/*
 * The offsets to different register blocks from the slot base.
 */
#define MCN_REG_OFFSET         	    0x0000
#define MCN_M32_REG_OFFSET(subcont)    (0x0020 + (0x10 * subcont))

/*
 * the MCN supports the following port adaptors
 */
#define MCN_EMPTY    0            /* no port adaptor plugged into the mcn */
#define MCN_PA_T1_U2 BRASIL_T1_DPRI_PLUS_CSU       /* Dual T1 port adaptor */
#define MCN_PA_E1_U2 BRASIL_E1_DPRI_PLUS_CSU       /* Dual E1 port adaptor */

#define MCN_PA_T1    0xe0        /* common T1 adapter type */
#define MCN_PA_E1    0xe1        /* common E1 adapter type */

/*
 * the MCN reset control masks
 */
#define MCN_M32_RESET_MASK    0xfe
#define MCN_M32_ENABLE_MASK   0x01
#define MCN_ENABLE_MASK	          0x0001
#define MCN_FRAMING_ENABLE_MASK   0x0010
#define MCN_LED_DISABLE           0x00ff

#define MCN_M32_1_INTERRUPRT    BITS(3)
#define MCN_M32_2_INTERRUPRT    BITS(4)

/*
 * the states defined for controlling MCN line clock source selection
 * during automatic loopback.
 */
#define MCN_IDLE_STATE		1
#define MCN_PRI_IN_LOOP_STATE	2
#define MCN_SEC_IN_LOOP_STATE	3

/* The MCN registers of the NIM */
typedef struct mcn_block mcn_block_t;
struct mcn_block {
    volatile ushort nim_type;         /* nim type*/
    volatile ushort nim_revision;     /* revision # */
    ushort   reserve1;
    volatile short pri_control1;      /* point to neat pri control 1 register*/
    volatile short pri_status;        /* point to neat pri status register*/
    ushort   reserve2[2];
    volatile short pri_control2;      /*point to neat pri control 2 register*/
    volatile short page_ram;          /*point to the local ram page register*/
};    

typedef struct m32_instance m32_instance_t;
typedef struct dsx1_instance  dsx1_instance_t;
typedef struct m32_block    m32_block_t;
typedef struct dtmf_instance  dtmf_instance_t;

/* The Multi-channel NIM instance */
typedef struct mcn_instance mcn_instance_t;

typedef struct mcn_subinstance_t_ {
    m32_instance_t  *m32_ds;        /* Munich32 instance structure */
    union {
        dsx1_instance_t *dsx1;          /* E1 ot T1 instance structure */
    }info;
} mcn_subinstance_t;

struct mcn_instance {
    mcn_block_t *mcn_regs;          /* Ptr to NIM slot base */
    ulong mcn_cardtype;             /* Installed port adapter type    */
    ulong total_subcont;            /* the number of DSX1 subconts installed */
    ulong total_dtmf;               /* the number of DTMFs installed */
    dtmf_instance_t *dtmf_unit[MAX_DTMF_UNIT_PER_SLOT];
    mcn_subinstance_t mcn_subcont[MAX_MCN_SUBCONT_PER_SLOT];
    uchar loopback_state;           /* For clk src select during auto loop */
};


/*
 * Inline statements 
 */
inline static void mcn_reset_m32(mcn_instance_t *mcn_instance) 
{
   mcn_instance->mcn_regs->pri_control1 &= MCN_M32_RESET_MASK;
}


/* ---------------------------------
 * macros to get instance structures
 * --------------------------------- */
/*
 * macro to get the mcn instance structure using the slot
 */
#define SINGLE_SUBCONT          0
#define MCN_INSTANCE(slot)  (mcn_ds[slot])
#define M32_INSTANCE(slot,subcont)  ((mcn_ds[slot] \
                                      && (subcont < mcn_ds[slot]->total_subcont)) \
                                     ? mcn_ds[slot]->mcn_subcont[subcont].m32_ds : NULL)
#define DSX1_INSTANCE(slot,subcont) ((mcn_ds[slot] \
                                      && (subcont < mcn_ds[slot]->total_subcont)) \
                                     ? mcn_ds[slot]->mcn_subcont[subcont].info.dsx1 : NULL)
#define DTMF_INSTANCE(slot,subcont) ((mcn_ds[slot] \
                                      && (subcont < mcn_ds[slot]->total_dtmf)) \
                                     ? mcn_ds[slot]->dtmf_unit[subcont] : NULL)

#define DELAY_ACTION_REQUEST() (usecdelay(M32_AR_SPIN_WAIT_USEC*2))
/*
 * Prototypes
 */
uint mcn_get_adapter_type(uint);
void mcn_set_vectors(uint);

/*
 * external references and declarations.
 */
extern void t1_dtmf_startup(mcn_instance_t *ds_mcn, uint subcont);
extern void t1_download(int slot);
extern void e1_dtmf_startup(mcn_instance_t *ds_mcn, uint subcont);
extern void e1_download(int slot);
extern void dsx1_set_runts_handle(hwidbtype *idb);

extern mcn_instance_t *mcn_ds[];
#endif
#endif		/* to prevent multiple inclusion*/




