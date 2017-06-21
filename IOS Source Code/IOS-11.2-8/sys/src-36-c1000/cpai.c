/* $Id: cpai.c,v 3.3.62.2 1996/06/12 19:56:57 jng Exp $
 * $Source: /release/112/cvs/Xsys/src-36-c1000/cpai.c,v $
 *------------------------------------------------------------------
 * CPAI ASIC 5:1 interface routines (e.g. Catwoman Serial I/F)
 *
 * April 1995, Henniger 
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cpai.c,v $
 * Revision 3.3.62.2  1996/06/12  19:56:57  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.3.62.1  1996/03/18  21:57:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  22:50:07  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  18:40:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:29:36  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:19:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:52:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"                /* define boolean                    */
#include "interface_private.h"     /* define hwidbtype                  */
#include "../src-36/quicc.h"       /* define quicc_dpr_t                */
#include "../src-36/quicc_lib.h"   /* prototype for  getmbar()          */
#include "cpai.h"
#include "init_c1005.h"

/*======================================================================*/
/* Prototype Definitions.                                               */
/*======================================================================*/

static uint cpai_reg_read(void);
static uint cpai_read_cid(void);
static void cpai_reg_write(uint value, uint mask);
static void cpai_clk_out(uint base_val, uint clock_mask);
static void cpai_check_clocks(void);

/*======================================================================*/
/* Global Variables.                                                    */
/*======================================================================*/

static volatile ulong  *cpai_tctrl_addr;
static volatile ushort *cpai_tsense_addr;
static volatile ushort *cpai_hshake_addr;

static uint cpai_sw_register;


/*======================================================================*/
/* Code routines.                                                       */
/*======================================================================*/

/*----------------------------------------------------------------------*/
/*  cable_present     : returns if a cable is plugged in.               */
/*  - cpai_reg_read()'s the 8 bit cable ID, and checks for no cable     */
/*----------------------------------------------------------------------*/

boolean cable_present (void)
{
    unsigned int retval = cpai_reg_read() & (CPAI_IDREG_MASK | CPAI_DCE_MODE);
    return ( retval != (CPAI_ID_NONE | CPAI_DCE_MODE) );
}

/*----------------------------------------------------------------------*/
/*  cable_type        : Returns enumerated cable type.                  */
/*----------------------------------------------------------------------*/

cable_type_t cable_type (void)
{
    unsigned int regval = cpai_reg_read();

    if (regval & CPAI_DCE_MODE) {
        switch (regval & CPAI_IDREG_MASK) {
          case CPAI_ID_NONE :  return(NO_CABLE);
          case CPAI_ID_232  :  return(DCE_RS232);
          case CPAI_ID_449  :  return(DCE_RS449);
          case CPAI_ID_530  :  return(DCE_RS530);
          case CPAI_ID_V35  :  return(DCE_V35);
          case CPAI_ID_X21  :  return(DCE_X21);
          default           :  return(UNKNOWN_CABLE);
        }
    } else {
        switch (regval & CPAI_IDREG_MASK) {
          case CPAI_ID_232  :  return(DTE_RS232);
          case CPAI_ID_449  :  return(DTE_RS449);
          case CPAI_ID_530  :  return(DTE_RS530);
          case CPAI_ID_V35  :  return(DTE_V35);
          case CPAI_ID_X21  :  return(DTE_X21);
          default           :  return(UNKNOWN_CABLE);
        }
    }
}

/*----------------------------------------------------------------------*/
/*  cable_dte         : Returns if cable type is DTE.                   */
/*  - cpai_reg_read()'s the 8 bit cable ID, and returns DCE/DTE bit     */
/*----------------------------------------------------------------------*/

inline boolean cable_dte (void)
{
    unsigned int retval = cpai_reg_read() & CPAI_DCE_MODE;
    return ( retval != CPAI_DCE_MODE);
}


/*----------------------------------------------------------------------*/
/*  c1000_show_cable_state: prints out cable type.                      */
/*  - cpai_reg_read()'s the 8 bit cable ID, and prints it out.          */
/*----------------------------------------------------------------------*/

void c1000_show_cable_state (hwidbtype* idb)
{
    boolean cable_is_dte;
    cable_type_t  cable = cable_type();

    if (cable  == NO_CABLE) {
        printf("No serial cable attached\n");
	return;
    }
    /* Print the applique type */

    if ( (cable_is_dte = cable_dte()) == FALSE ) {
        printf("Unsupported DCE ");
    } else {
        printf("DTE ");
    }

    /*
     * Print up the interface type
     */

    switch ( cable )
    {
       case DTE_RS232    : 
       case DCE_RS232    : printf("V.24 (RS-232)");
			   break;
       case DTE_RS449    :
       case DCE_RS449    : printf("V.11 (RS-449)");
			   break;
       case DTE_RS530    :
       case DCE_RS530    : printf("530");
			   break;
       case DTE_V35      :
       case DCE_V35      : printf("V.35");
			   break;
       case DTE_X21      :
       case DCE_X21      : printf("V.11 (X.21)");
			   break;
       case NO_CABLE     :
       case UNKNOWN_CABLE: printf("Unknown (%d)", cable);
			   break;
    }
    printf(" serial cable attached. ");
    if (!idb->idb_tty) {
	if ( cable_is_dte == TRUE) {
	    cpai_check_clocks();
	} else  {
	    printf("no clock.");
	}
    }
    printc('\n');
}

/*----------------------------------------------------------------------*/
/* cpai_check_clocks() will check for Tx and Rx clocks and print out    */
/* whether it detects activity there.                                   */
/*----------------------------------------------------------------------*/

static void cpai_check_clocks (void)
{
    sys_timestamp stop_time;
    unsigned short on_val=0, off_val = 0xffff;
    int rxclock=0, txclock=0;
    volatile ushort *cpai_clock_addr;

    cpai_clock_addr = &CPAI_CLOCK_PORT;

    /* for one clock tick, watch the clock inputs for changes. */

    process_may_suspend();  /*  Suspend if a high priority process must run. */
    TIMER_START(stop_time, REFRESHTIME * 2);
    while (SLEEPING(stop_time))
    {
        on_val  |= *cpai_clock_addr;
        off_val &= *cpai_clock_addr;
    }
    if ( (on_val & DEVICE_SERIAL_RXC) &&
	 !(off_val & DEVICE_SERIAL_RXC)  )
	rxclock = 1;

    if ( (on_val & DEVICE_SERIAL_TXC) &&
	 !(off_val & DEVICE_SERIAL_TXC)  )
        txclock = 1;
            
    if (rxclock && txclock) { 
	printf("TX and RX clocks detected.");
    } else if (rxclock) {
	printf("RX clock detected.");
    } else if (txclock) {
	printf("TX clock detected.");
    } else {
	printf("clocks stopped.");
    }
    printc('\n');
    process_may_suspend();  /*  Suspend if a high priority process must run. */
}


/*----------------------------------------------------------------------*/
/* read dcd pin, set dtr pin.                                           */
/*----------------------------------------------------------------------*/

boolean dcd_on (void)
{
    QASSERT(cpai_hshake_addr != NULL);
    return ((*cpai_hshake_addr & DCD_MASK) ? FALSE : TRUE);
}

boolean cts_on ()
{
    QASSERT(cpai_hshake_addr != NULL);
    return ((*cpai_hshake_addr & CTS_MASK) ? FALSE : TRUE);
}

boolean dsr_on ()
{
    QASSERT(cpai_hshake_addr != NULL);
    return ((*cpai_hshake_addr & DSR_MASK) ? FALSE : TRUE);
}

boolean rts_on ()
{
    QASSERT(cpai_hshake_addr != NULL);
    return ((*cpai_hshake_addr & RTS_MASK) ? FALSE : TRUE);
}

boolean dtr_on ()
{
    QASSERT(cpai_hshake_addr != NULL);
    return ((*cpai_hshake_addr & DTR_MASK) ? FALSE : TRUE);
}

void dtr_assert (boolean dtr_state)
{
    leveltype level;
    QASSERT(cpai_hshake_addr != NULL);
    level = raise_interrupt_level(NETS_DISABLE);
    if (dtr_state)
	*cpai_hshake_addr &= ~DTR_MASK;
    else
	*cpai_hshake_addr |= DTR_MASK;
    reset_interrupt_level(level);
}

void rts_assert (boolean rts_state)
{
    leveltype level;    
    QASSERT(cpai_hshake_addr != NULL);
    level = raise_interrupt_level(NETS_DISABLE);
    if (rts_state)
	*cpai_hshake_addr &= ~RTS_MASK;
    else
	*cpai_hshake_addr |= RTS_MASK;
    reset_interrupt_level(level);
}

/*----------------------------------------------------------------------*/
/* c1000_show_serial_state(idb): prints out state of handshake lines    */
/*      Reads the port directly and prints out the handshake lines.     */
/*----------------------------------------------------------------------*/

void c1000_show_serial_state (hwidbtype* idb)
{
    const char *fmt_str = "     DCD=%s  DSR=%s  DTR=%s  RTS=%s  CTS=%s\n";
    ushort modem_control;
    
    QASSERT(cpai_hshake_addr != NULL);
    modem_control = *cpai_hshake_addr;
    
    printf("\n");
    if (!idb->idb_tty) {
	if (idb->dte_interface) {
	    printf(fmt_str,
		   ( dcd_on()                  ? "up" : "down"),
		   ((modem_control & DSR_MASK) ? "down" : "up"),
		   ((modem_control & DTR_MASK) ? "down" : "up"),
		   ((modem_control & RTS_MASK) ? "down" : "up"),
		   ((modem_control & CTS_MASK) ? "down" : "up"));
	} 
	else 
	    {
		printf("DCE Mode is not supported.");
	    }
    }
}


/*----------------------------------------------------------------------*/
/* Set/clear various mode bits:                                         */
/*   cpai_set_{loop,invert...dcd_invert}(TRUE)  activates mode.         */
/*   cpai_set_{loop,invert...dcd_invert}(FALSE) de-activates mode.      */
/*----------------------------------------------------------------------*/

inline void cpai_set_loop (boolean value) {   
    cpai_reg_write(value ? CPAI_LOOP : 0,CPAI_LOOP); 
}
inline void cpai_set_txc_invert (boolean value) {   
    cpai_reg_write(value ? CPAI_INVERT : 0, CPAI_INVERT); 
}
inline void cpai_set_no_txce (boolean value) {   
    cpai_reg_write(value ? CPAI_NO_TXCE : 0, CPAI_NO_TXCE);
}
inline void cpai_set_sync (boolean value) {   
    cpai_reg_write(value ? CPAI_SYNC : 0, CPAI_SYNC);
}
inline void cpai_set_dcd_invert (boolean value) {   
    cpai_reg_write(value ? CPAI_DCD_INVERT : 0, CPAI_DCD_INVERT);
}

/*----------------------------------------------------------------------*/
/* cpai_read_modes()  - Same as cpai_reg_read but returns stale cable id*/
/* added for performance, for clients who do not need cable id.         */
/*----------------------------------------------------------------------*/

inline static uint cpai_read_modes (void)
{
    return(cpai_sw_register);
}

/*----------------------------------------------------------------------*/
/* Set/clear various mode bits:                                         */
/*   cpai_get_{loop,invert...dcd_invert}() returns TRUE if mode active. */
/*   cpai_get_{loop,invert...dcd_invert}() returns FALSE if inactive.   */
/*----------------------------------------------------------------------*/

inline boolean cpai_get_loop (void) {
    return(cpai_read_modes() & CPAI_LOOP ? TRUE : FALSE);
}
inline boolean cpai_get_txc_invert (void) {            
    return(cpai_read_modes() & CPAI_INVERT ? TRUE : FALSE);
}
inline boolean cpai_get_no_txce (void) {            
    return(cpai_read_modes() & CPAI_NO_TXCE ? TRUE : FALSE);
}
inline boolean cpai_get_sync (void) {            
    return(cpai_read_modes() & CPAI_SYNC ? TRUE : FALSE);
}
inline boolean cpai_get_dcd_invert (void) {            
    return(cpai_read_modes() & CPAI_DCD_INVERT ? TRUE : FALSE);
}


/*----------------------------------------------------------------------*/
/* cpai_init():  init's pointers to ports A, B, and C.                  */
/*               (Can this be done via idb references?)                 */
/*----------------------------------------------------------------------*/
void cpai_init (void)
{
    cpai_tctrl_addr  = &CPAI_TCTRL_PORT;
    cpai_tsense_addr = &CPAI_TSENSE_PORT;
    cpai_hshake_addr = &CPAI_HSHAKE_PORT;
    *cpai_tctrl_addr  &= ~CPAI_TCTRL_MASK;

    /*-----------------------------------------------------------------*/
    /*  Set init state to all on, then reg_write then all to zero.     */
    /*  required since reg_write only acts on differences.             */
    /*-----------------------------------------------------------------*/

    cpai_sw_register = CPAI_MODES_MASK;
    cpai_sw_register &= ~CPAI_SYNC;     /* turn synchronizer off        */
    /* Put the hardware in initialized state.                           */

    cpai_reg_write(CPAI_SYNC,CPAI_MODES_MASK);

    cpai_reg_read();     /* read the cable ID to get cpai out of reset  */

}

/*----------------------------------------------------------------------*/
/* cpai_reg_read(): uses TCTRL2..0 with TSENSE to build up bit pattern  */
/*       for the cable ID, then updates  & returns cpai_sw_register     */
/*----------------------------------------------------------------------*/
static unsigned int cpai_reg_read (void)
{
    uint cable_id=0;
    leveltype level;
    int numtries;

    QASSERT(cpai_tctrl_addr != NULL);

    /*--------------------------------------------------------------------*/
    /* read the port with all these bits cleared. use this for writes     */
    /*--------------------------------------------------------------------*/

    level = raise_interrupt_level(NETS_DISABLE);
    for (numtries = 0; numtries < 5 ; numtries++) {
        if ( (cable_id = cpai_read_cid()) == cpai_read_cid())
	    break;
    }
    cpai_sw_register  &= ~(CPAI_IDREG_MASK|CPAI_DCE_MODE);
    cpai_sw_register  |= cable_id;
    reset_interrupt_level(level);
    return(cpai_sw_register);
}



static unsigned int cpai_read_cid (void)
{
    int cable_id=0;
    int i;
    unsigned int tmp;

    QASSERT(cpai_tctrl_addr != NULL);

    *cpai_tctrl_addr = ((*cpai_tctrl_addr)&(~CPAI_TCTRL_CID_MASK));
    tmp = *cpai_tctrl_addr;
    
    /*--------------------------------------------------------------------*/
    /* for each bit, incr. TCTRL1..0, & read the TSENSE bit into cable_id */
    /* Note: TCTRL2 is always 0, since only 4 bits is needed.             */
    /*--------------------------------------------------------------------*/

    for (i = 3<<CPAI_TCTRL_SHIFT ; i >=0 ; i -= (1<<CPAI_TCTRL_SHIFT)) {
        *cpai_tctrl_addr = tmp | (i<< CPAI_TCTRL_SHIFT);
        cable_id = cable_id <<1;
        if (*cpai_tsense_addr & CPAI_TSENSE_MASK) {
            cable_id++;
        }
    }
    *cpai_tctrl_addr = tmp;
    return(cable_id);
}

/*--------------------------------------------------------------------------*/
/* cpai_reg_write() :  takes bits in cpai_sw_register and updates the HW    */
/*--------------------------------------------------------------------------*/

static void cpai_reg_write (unsigned int value,unsigned  int mask)
{
    unsigned int old_value, tmp;
    leveltype level;


    level = raise_interrupt_level(NETS_DISABLE);

    /*----------------------------------------------------------------------*/
    /* First, update cpai_sw_register to new state.                         */
    /*----------------------------------------------------------------------*/

    old_value = cpai_sw_register;
    cpai_sw_register = (cpai_sw_register & ~mask) | (value & mask);

    /*----------------------------------------------------------------------*/
    /* XOR new and old value to determine if bits should be updated in HW.  */
    /* Since DCD, TXCE are written together, and LOOP/INVERT are written    */
    /* in same clock, do these together. SYNC_OFF can be done by itself.    */
    /*----------------------------------------------------------------------*/

    /*----------------------------------------------------------------------*/
    /* Check first to see if we are supposed to change the SYNC bit         */
    /*----------------------------------------------------------------------*/

    if (  (old_value ^ cpai_sw_register) & CPAI_SYNC) {
        tmp = CPAI_TCTRL_SIDLE;
	if (!(cpai_sw_register & CPAI_SYNC))
	    tmp |= CPAI_TCTRL_D0;
        cpai_clk_out(tmp,CPAI_TCTRL_CLK4);
    }
    /*----------------------------------------------------------------------*/
    /* Next Check to see if DCD_INVERT or NO_TXCE have changed state */
    /*----------------------------------------------------------------------*/

    if (  (old_value ^ cpai_sw_register) & (CPAI_DCD_INVERT | CPAI_NO_TXCE)) {
        tmp = CPAI_TCTRL_IDLE;
	if (cpai_sw_register & CPAI_DCD_INVERT )
	    tmp |= CPAI_TCTRL_D1 ;
	if (cpai_sw_register & CPAI_NO_TXCE )
	    tmp |= CPAI_TCTRL_D0;
        cpai_clk_out(tmp,CPAI_TCTRL_CLK4);
    }
    /*----------------------------------------------------------------------*/
    /* Finally check to see if LOOPBACK or INVERT have changed state */
    /*----------------------------------------------------------------------*/

    if (  (old_value ^ cpai_sw_register) & (CPAI_LOOP | CPAI_INVERT) ) {
        tmp = CPAI_TCTRL_IDLE;
	if (cpai_sw_register & CPAI_LOOP)
	    tmp |= CPAI_TCTRL_D1 ;
	if (cpai_sw_register & CPAI_INVERT)
	    tmp |= CPAI_TCTRL_D0;
        cpai_clk_out(tmp,CPAI_TCTRL_CLK2);
    }
    reset_interrupt_level(level);
}

/*--------------------------------------------------------------------------*/
/* cpai_clk_out(v,m) - write value v, then v+m, then v again to tctrl.      */
/*--------------------------------------------------------------------------*/
static void cpai_clk_out (unsigned int base_val,unsigned  int clock_mask)
{
    unsigned int tmpoff;

    QASSERT(cpai_tctrl_addr != NULL);
    tmpoff = (*cpai_tctrl_addr) & (~CPAI_TCTRL_MASK);
    base_val |= tmpoff;

    *cpai_tctrl_addr  = tmpoff;                 /* turn all bits off.    */
    *cpai_tctrl_addr  = base_val;               /* turn on data and IDLE */
    *cpai_tctrl_addr  = base_val | clock_mask;  /* Rising Clock Edge     */
    *cpai_tctrl_addr  = base_val;               /* Falling Clock Edge    */
    *cpai_tctrl_addr  = tmpoff;                 /* Turn all bits off.    */
}
