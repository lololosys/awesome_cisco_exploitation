/* $Id: if_c3000_cd2430.h,v 3.1.2.5 1996/08/28 12:57:27 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c3000_cd2430.h,v $
 *------------------------------------------------------------------
 * if_c3000_cd2430.h - Definitions for the Cirrus CL-CD2430
 *                     controller (for c3000 platform only)
 *
 * May 1996, Jose Hernandez
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c3000_cd2430.h,v $
 * Revision 3.1.2.5  1996/08/28  12:57:27  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.4  1996/07/25  23:44:53  jhernand
 * CSCdi57040:  SDLC test fails intermittently on Async/Sync ports OK on
 *              Sync port
 * Add the TXCRXC control bit to Low Speed Serial Device Register Hi
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/07/05  19:31:39  jhernand
 * CSCdi61429:  Modify Cirrus Logic CD2430 initialization by adding Timeout
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/07/04  02:07:08  jhernand
 * CSCdi61640:  Cobra code does not conform to Cisco guidelines
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/06/17  08:33:33  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.1  1996/05/13  18:56:09  samuell
 * Placeholder files for porting Modular 2500 Cobra platform from 11.1.
 * Modular 2500 Cobra files were derived from the original Cobra 11.1
 * Release by code separation into platform-independent code and 2500
 * platform-dependent code files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __IF_C3000_CD2430_H__
#define __IF_C3000_CD2430_H__

/*
 * CD2430 register layout.
 * The names are taken from the Cirrus spec sheet.
 */
typedef unsigned char cd2430_res;
typedef volatile unsigned char cd2430_rw, cd2430_ro, cd2430_wo;
typedef volatile unsigned short cd2430_wrw, cd2430_wro;

typedef struct cd2430_quadart_ {
    cd2430_res res1[7]; /* 00..06 */
    cd2430_rw cor7;     /* 07           Channel Option Register 7 */
#define tsmap2 cor7     /*                ...name in PPP mode */
    cd2430_res res2[1]; /* 08 */
    cd2430_rw livr;     /* 09           Local Interrupt Vector Register */
    cd2430_res res3[6]; /* 0a..0f */
    cd2430_rw cor1;     /* 10           Channel Option Register 1 */
    cd2430_rw ier;      /* 11           Interrupt Enable Register */
    cd2430_rw stcr;     /* 12           Special Transmit Command Register */
    cd2430_rw ccr;      /* 13           Channel Command Register */
    cd2430_rw cor5;     /* 14           Channel Option Register 5 */
    cd2430_rw cor4;     /* 15           Channel Option Register 4 */
    cd2430_rw cor3;     /* 16           Channel Option Register 3 */
    cd2430_rw cor2;     /* 17           Channel Option Register 2 */
    cd2430_rw cor6;     /* 18           Channel Option Register 6 */
#define tsmap1 cor6     /*                ...name in PPP mode */
    cd2430_ro dmabsts;  /* 19           DMA Buffer Status */
    cd2430_ro csr;      /* 1a           Channel Status Register */
    cd2430_rw cmr;      /* 1b           Channel Mode Register */
    cd2430_rw schr4;    /* 1c           Special Character Register 4 */
#define txmap4 schr4    /*               ...its name in PPP mode */
    cd2430_rw schr3;    /* 1d           Special Character Register 3 */
#define txmap3 schr3    /*               ...its name in PPP mode */
    cd2430_rw schr2;    /* 1e           Special Character Register 2 */
#define txmap2 schr2    /*               ...its name in PPP mode */
    cd2430_rw schr1;    /* 1f           Special Character Register 1 */
#define txmap1 schr1    /*               ...its name in PPP mode */
    cd2430_rw rxmap4;   /* 20           Receive control char map PPP */
    cd2430_rw rxmap3;   /* 21            ...four bytes in a row */
    cd2430_rw scrh;     /* 22           Special Character Range high */
#define rxmap2 scrh     /*                ...its name in PPP mode */
    cd2430_rw scrl;     /* 23           Special Character Range low */
#define rxmap1 scrl     /*                ...its name in PPP mode */
    cd2430_wrw rtpr;    /* 24           Receive Timeout Period Register */
    cd2430_rw licr;     /* 26           Local Interrupting Channel Register */
    cd2430_rw dbadj;    /* 27		DMA Buffer Adjust Register */
    cd2430_res res5[1]; /* 28 */
    cd2430_ro ttr;      /* 29           Transmit Timer Register */
    cd2430_wrw gt1;     /* 2a..2b       General Timer 1 */
    cd2430_res res6[2]; /* 2c..2d */
    cd2430_rw lnxt;     /* 2e           Literal Next Character */
#define tsmap3 lnxt     /*                ...name in PPP mode */
    cd2430_res res7[1]; /* 2f */
    cd2430_ro rfoc;     /* 30           Receive FIFO Output Count */
    cd2430_res res8[7]; /* 31..37 */
    cd2430_wro tcbadru; /* 38           Transmit Current Buffer Address upper */
    cd2430_wro tcbadrl; /* 3a           Transmit Current Buffer Address lower */
    cd2430_wro rcbadru; /* 3c           Receive Current Buffer Address upper */    cd2430_wro rcbadrl; /* 3e           Receive Current Buffer Address lower */    cd2430_wrw arbadru; /* 40           A Receive Buffer Address upper */
    cd2430_wrw arbadrl; /* 42           A Receive Buffer Address lower */
    cd2430_wrw brbadru; /* 44           B Receive Buffer Address upper */
    cd2430_wrw brbadrl; /* 46           B Receive Buffer Address lower */
    cd2430_wrw brbcnt;  /* 48           B Receive Buffer Byte Count */
    cd2430_wrw arbcnt;  /* 4a           A Receive Buffer Byte Count */
    cd2430_res res9[2]; /* 4c..4d */
    cd2430_rw brbsts;   /* 4e           B Receive Buffer Status */
    cd2430_rw arbsts;   /* 4f           A Receive Buffer Status */
    cd2430_wrw atbadru; /* 50           A Transmit Buffer Address upper */
    cd2430_wrw atbadrl; /* 52           A Transmit Buffer Address lower */
    cd2430_wrw btbadru; /* 54           B Transmit Buffer Address upper */
    cd2430_wrw btbadrl; /* 56           B Transmit Buffer Address lower */
    cd2430_wrw btbcnt;  /* 58           B Transmit Buffer Byte Count */
    cd2430_wrw atbcnt;  /* 5a           A Transmit Buffer Byte Count */
    cd2430_res resa[2]; /* 5c..5d */
    cd2430_rw btbsts;   /* 5e           B Transmit Buffer Status */
    cd2430_rw atbsts;   /* 5f           A Transmit Buffer Status */
    cd2430_res resb[32];/* 60..7f */
    cd2430_ro tftc;     /* 80           Transmit FIFO Transfer Count */
    cd2430_ro gfrcr;    /* 81           Global Firmware Revision Code */
    cd2430_res resc[2]; /* 82..83 */
    cd2430_wo reoir;    /* 84           Receive End of Interrupt Register */
    cd2430_wo teoir;    /* 85           Transmit End of Interrupt Register */
    cd2430_wo meoir;    /* 86           Modem End of Interrupt Register */
    cd2430_res resd[1]; /* 87*/
    cd2430_wrw risr;    /* 88           Receive Interrupt Status Register */
    cd2430_ro tisr;     /* 8a           Transmit Interrupt Status Register */
    cd2430_ro misr;     /* 8b           Modem Interrupt Status Register */
    cd2430_res rese[2]; /* 8c..8d */
    cd2430_rw bercnt;   /* 8e           Bus Error Retry Count */
    cd2430_res resf[49];/* 8f..bf */
    cd2430_rw tcor;     /* c0           Transmit Clock Option Register */
    cd2430_res resg[2]; /* c1..c2 */
    cd2430_rw tbpr;     /* c3           Transmit Baud Rate Period Register */
    cd2430_res resh[3]; /* c4..c6 */
    cd2430_ro rbcr;     /* c7           Receive Bit Count Register */
    cd2430_rw rcor;     /* c8           Receive Clock Option Register */
    cd2430_res resi[2]; /* c9..ca */
    cd2430_rw rbpr;     /* cb           Receive Baud Rate Period Register */
    cd2430_res resj[2]; /* cc..cd */
    cd2430_ro tbcr;     /* ce           Transmit Bit Count Register */
    cd2430_res resj1[7];/* cf..d5 */
    cd2430_rw cpsr;     /* d6           CRC Polynomial Select Register */
    cd2430_res resJ[3]; /* d7..d9 */
    cd2430_rw tpr;      /* da           Timer Period Register */
    cd2430_res resk[3]; /* db..dd */
    cd2430_rw msvr_rts; /* de           Modem Signal Value--RTS */
    cd2430_rw msvr_dtr; /* df           Modem Signal Value--DTR */
    cd2430_rw tpilr;    /* e0           Transmit Priority Interrupt Level */
    cd2430_rw rpilr;    /* e1           Receive Priority Interrupt Level */
    cd2430_ro stk;      /* e2           Stack Register */
    cd2430_rw mpilr;    /* e3           Modem Priority Interrupt Level */
    cd2430_res resl[8]; /* e4..eb */
    cd2430_ro tir;      /* ec           Transmit Interrupt Register */
    cd2430_ro rir;      /* ed           Receive Interrupt Register */
    cd2430_rw car;      /* ee           Channel Access Register */
    cd2430_ro mir;      /* ef           Modem Interrupt Register */
    cd2430_rw airh;     /* f0           Auxilary Instruction Register 17..16 */    cd2430_rw mtcr;     /* f1           MTCR */
    cd2430_rw airl;     /* f2            ...AIR bits 7..0 */
    cd2430_rw airm;     /* f3            ... bits 15..8 */
    cd2430_res resm[2]; /* f4..f5 */
    cd2430_wo dmr;      /* f6           DMA Mode Register */
    cd2430_rw btcr;     /* f7           BTCR */
    cd2430_ro rdr;      /* f8           Read/Write Data Register */
#define tdr rdr
    cd2430_res reso[7]; /* f9..ff */
} cd2430_quadart;


/* Location of vectors & size */
#define CD2430Vector 64         /* Base vector for CD2430 */
#define CD2430VectorCount 4     /* Number of vectors per controller */
#define CD2430ADDRESS(unit) \
    (cd2430_quadart *)(0x2100000 + (unit)*sizeof(cd2430_quadart))
#define CD2430ADDRESS_COBRA(unit) \
    (cd2430_quadart *)(0x2134100 + (unit)*sizeof(cd2430_quadart))
#define LSS_LED_ACTIVATE(ctx)  \
    *(ushort *)(0x2134080) = ctx->led_mask;

/*
 * Write the value of LICR for a particular controller. The value of
 * LICR is used to index into an array to get at information
 * corresponding to that channel. The index value has to be determined
 * by how the LICR is programmed.
 *
 *         +--+--+--+--+--+--+--+--+
 *         |C3|C2|C1|C0|L1|L0|C5|C4|
 *         +--+--+--+--+--+--+--+--+
 * Here the Cn's refer to the controller number, and the Ln's refer to
 * the channel number. Note how the bits for the controller number are
 * spread around in the LICR register. We have a limit of 64 controllers
 * in the system for this scheme to work. This is not a problem for the
 * foreseeable future. Note also that the interrupt wrappers for the
 * CD2430 will have to do the rotate operations necessary to extract the
 * index number from the LICR's contents. The following macro is 
 * concerned only with programming the LICR's contents at init time
 * correctly.
 */ 
#define CD2430LICR_WRITE(reg_table, ctrl) \
    { \
        char temp; \
        temp = ctrl & 0x0F; \
        reg_table->licr |= (temp << 4); \
        temp = ctrl & 0x30; \
        temp = (temp >> 4) & 0x3; \
        reg_table->licr |= temp; \
    }


/*
 * Macros for the CD2430 driver to get at the device registers on the
 * Cobra platform.
 */
#define CD2430_LSSDEVREGLO(unit) (ADRSPC_LSS_REG_BASE \
                                  + (unit - cd2430Base)*4)
#define CD2430_LSSDEVREGHI(unit) (ADRSPC_LSS_REG_BASE + 2 \
                                  + (unit - cd2430Base)*4)

/*
 * VTOP()
 *      Virtual to physical
 *
 * It appears that virtual address == physical address on all PAN variants.
 * Nevertheless, old experiences have given me a lasting desire to underscore
 * those places where I intend to use an address for physical (DMA) access.
 * In those places, this macro (the identity function) is used.
 */
#define VTOP(addr) ((ulong)addr)

/*
 * Macros defined for 2500 Cobra platform as part of Cobra code
 * separation for 4500 Harp project.
 */

#define CD2430_VALID_PORT(idb) ((idb->unit < cd2430Base) || (idb->unit >= (cd2430Base+cd2430ports)))

#define CD2430_SET_LIVR_SYNC(idb) (reg_table->livr = 0x40 | (ctx->ctrl << 2))

#define CD2430_SET_LIVR_ASYNC(tty) (csr->livr = 0x40 | (ctrl << 2))

#define CD2430_GET_CHAN_INST(idb) (&cd2430_channel_info[(ctx->ctrl*CD2430_CHIPCHAN) + ctx->chan])

#define CD2430_SET_MODEM_CTRL_TXC_INV (*(ctx->serial_dev_reg_lo) |= DEVICE_LSS_TXC_INV)

#define CD2430_RESET_MODEM_CTRL_TXC_INV (*(ctx->serial_dev_reg_lo) &= ~DEVICE_LSS_TXC_INV)

#define CD2430_SET_MODEM_CTRL_DCE_TERM (*(ctx->serial_dev_reg_lo) |= DEVICE_LSS_DCE_TERM)

#define CD2430_RESET_MODEM_CTRL_DCE_TERM (*(ctx->serial_dev_reg_lo) &= ~DEVICE_LSS_DCE_TERM)

#define CD2430_SET_HELPER_MASK(idb_slot,ctx) (helper_mask |= ctx->helper_mask)

#define SET_CD2430_SYNC_IDBS(idb) (cd2430_sync_idbs[idb->unit - cd2430Base] = idb)

#define CD2430_SET_CLOCK_PRE_DIVIDER(ctx,pre_divider) \
        *(ctx->serial_dev_reg_lo) &= COBRA_PRE_DIVIDER_MASK; \
        *(ctx->serial_dev_reg_lo) |= (pre_divider << 5);

#define CD2430_SERIAL_FLAG_DEFAULT (SERIAL_FLAGS_SYNCASYNC | SERIAL_FLAGS_CAN_ALT_FLAG | \
                          SERIAL_FLAGS_CAN_NRZI | SERIAL_FLAGS_CAN_INT_TX | \
                          SERIAL_FLAGS_NO_DTE_INT_TX | \
                          SERIAL_FLAGS_CAN_CLK_INV | \
                          SERIAL_FLAGS_CAN_IGNORE_DCD | \
			  SERIAL_FLAGS_CAN_IGNORE_LOCAL_LOOPBACK)

/*
 * Macros defined and used by 4500 and other platforms but not 2500
 */

#define CD2430_SET_PLATFORM_HW_SYNC

/*
 * Data structures global to the CD2430 subsystem.
 */
extern cd2430_channel_inst_t *cd2430_channel_info[CD2430MAX * CD2430_CHIPCHAN];
extern cd2430_quadart *cd2430_reg_table[CD2430MAX];
extern hwidbtype *cd2430_sync_idbs[CD2430MAX*CD2430_CHIPCHAN];
extern void (*cd2430_vectors[]);        /* Front-line interrupt vectors */
extern int cd2430_controllers;         /* Number of CD2430 chips in system */
extern int cd2430ports;                /* Number of CD2430 ports in system */
extern int cd2430_istate;              /* Non-zero if in interrupt state */
extern int cd2430Base;                 /* Starting line number for CD2430's */
extern uint cd2430clock;                /* Clock applied to chips */
extern ushort lss_misc;                 /* Misc bits describing async options */
extern int cd2430_platform_type;
extern uchar cd2402_firmware[];
extern uchar initial_gfrcr[CD2430MAX];  /* cd2430 chip rev level */

/*
 * Defines to identify the platform on which the sync or async driver is
 * running.
 */
#define PLATFORM_COPAN  1
#define PLATFORM_COBRA  2

#define CD2430_RESET_TIMEOUT  (3 * ONESEC) /* Reset timeout interval */

/* Prototypes */
extern void sendccr(cd2430_quadart *, uchar);
extern void cd2430_controller_init(int);
extern void cd2430_show_registers_global(cd2430_quadart *);
extern boolean cd2430_show_controller(hwidbtype *, parseinfo *);
extern void cd2402_download (cd2430_quadart *, uchar *);
extern uchar *flash_get_firmware (void);

/*
 * Inline function declarations
 *      These inline functions are used by sync driver code.
 */

/*
 * channel_mode_sync()
 *      Report if the channel is currently in sync mode.
 * The parameter passed into this function will be idb->unit. Since the
 * CD2430 sync IDBs array as well as the channel info array store their
 * elements starting from 0, we will have to subtract cd2430Base from
 * the passed in parameter to get at the right element for this interface.
 */
static inline boolean
channel_mode_sync (hwidbtype *idb)
{
    return ((cd2430_channel_info[idb->unit - cd2430Base])->aid ==
                                         cd2430_sync_idbs[idb->unit - cd2430Base]);
}

#endif  /* __IF_C3000_CD2430_H__ */



