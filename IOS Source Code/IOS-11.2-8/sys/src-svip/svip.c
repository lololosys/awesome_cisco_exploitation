/* $Id: svip.c,v 3.1.62.9 1996/08/28 13:15:03 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-svip/svip.c,v $
 *------------------------------------------------------------------
 * svip.c - SVIP specific routines
 *
 * 10/02/95  Srini		 - Its VIPER now..
 * December 94, David Getchell
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: svip.c,v $
 * Revision 3.1.62.9  1996/08/28  13:15:03  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.62.8  1996/06/17  23:37:32  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.1.62.7  1996/06/07  05:10:44  getchell
 * CSCdi59206:  EMT_RELOAD, EMT_CPU_EXCEPTION does call svip_reload()
 * Branch: California_branch
 *
 * Revision 3.1.62.6  1996/06/03  18:04:44  getchell
 * CSCdi52344:  scheduler allocate needed for debug under load
 * Branch: California_branch
 *
 * Revision 3.1.62.5  1996/05/18  17:54:21  kao
 * CSCdi54117:  vip2 looses it with serial encaps other than HDLC
 * cbus stall handling plus two s/w work-around for CYA h/w bugs
 * Branch: California_branch
 *
 * Revision 3.1.62.4  1996/04/12  17:44:04  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.1.62.3  1996/04/10  03:38:57  dlobete
 * CSCdi53121:  Distributed IP Access Lists need to run at interrupt-level
 * Branch: California_branch
 *
 * Revision 3.1.62.2  1996/04/06  21:56:25  ondiaye
 * CSCdi52394:  svip support for merged qe codes header file
 *              Also takes care of CSCdi52731 and the DRQ dealock.
 * Branch: California_branch
 *
 * Revision 3.1.62.1  1996/03/21  23:51:18  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.4  1996/03/18  23:36:11  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.10.3  1996/02/21  20:37:16  getchell
 * CSCdi49474:  Time to turn on IPC/DFS on viper
 * Do some cleanup while at it...
 * Branch: ELC_branch
 *
 * Revision 3.1.10.2  1996/02/16  02:24:29  mbeesley
 * CSCdi48910:  Add storage of fault history for crashes
 * Branch: ELC_branch
 *
 * Revision 3.1.10.1  1996/02/10  00:21:54  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/26  21:18:33  getchell
 * Placeholder files for Viper development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "sched.h"
#include "logger.h"
#include "ttysrv.h"
#include "stacks.h"
#include "config.h"
#include "parser.h"

#include "../dev/monitor1.h"
#include "../hes/dgbus.h"
#include "../ts/nonvol_x2444.h"
#include "../os/gdb.h"
#include "../pas/if_pas.h"
#include "../pas/msg_pas.c"		/* Not a typo, see logger.h */
#include "../src-vip/vip_dbus.h"
#include "../src-vip/vip_memd.h"

#include "../src-svip/svip_asic.h"
#include "../src-svip/svip_nevada.h"
#include "../src-svip/svip.h"
#include "../pas/pas_id.h"
#include "../hes/if_fci.h"


tinybool        svip_netint_throttle_configured;  /* is it configured */
usec_timestamp  netint_return;          /* last time net intr retr. */

/*
 * Local variables
 */
static tinybool        netint_throttle_active; /* is net intr masked */
static ulong           netint_throttle_count;  /* throttle activation count */
static ulong           netint_timer_count;     /* timer intr. count */
static ulong           netint_ticks;           /* max ticks in intr */
static ulong           netint_mask_ticks;      /* ticks to mask intr */
static ulong           netint_mask_usec;       /* usecs to mask intr */
static usec_timestamp  netint_exit;            /* when to mask intr */

/*
 * Port Adaptor ID eeprom management data structures
 */
pas_management_t pa_bay0_management = {
    0,						/* Bay 0 */

    sizeof(IO_ASIC->cookie),			/* 16 bit register */
    COOKIE_PA0_TCLK,			        /* PA 0 clock mask */
    &IO_ASIC->cookie,				/* Clock control register */

    sizeof(IO_ASIC->cookie),			/* 16 bit register */
    COOKIE_PA0_TEECS,			        /* PA 0 select mask */
    &IO_ASIC->cookie,				/* Select control register */

    sizeof(IO_ASIC->cookie),			/* 16 bit register */
    COOKIE_PA0_TDIN,			        /* DATAIN mask */
    &IO_ASIC->cookie,				/* DATAIN register */

    sizeof(IO_ASIC->cookie),			/* 16 bit register */
    COOKIE_PA0_TDOUT,			        /* DATAOUT mask */
    &IO_ASIC->cookie				/* DATAOUT register */
};

pas_management_t pa_bay1_management = {
    1,						/* Bay 1 */

    sizeof(IO_ASIC->cookie),			/* 16 bit register */
    COOKIE_PA2_TCLK,				/* PA 1 clock mask */
    &IO_ASIC->cookie,				/* Clock control register */

    sizeof(IO_ASIC->cookie),			/* 16 bit register */
    COOKIE_PA2_TEECS,				/* PA 1 select mask */
    &IO_ASIC->cookie,				/* Select control register */

    sizeof(IO_ASIC->cookie),			/* 16 bit register */
    COOKIE_PA2_TDIN,				/* DATAIN mask */
    &IO_ASIC->cookie,				/* DATAIN register */

    sizeof(IO_ASIC->cookie),			/* 16 bit register */
    COOKIE_PA2_TDOUT,				/* DATAOUT mask */
    &IO_ASIC->cookie				/* DATAOUT register */
};

pas_management_t *pas_management_list[MAX_PA_BAYS] = {
    &pa_bay0_management,
    &pa_bay1_management
};

static const pma_err_parse pma_errs[] = {
    { ERRU_PCI_TX_ERR, 0,	"PCI Transmit Parity error" },
    { ERRU_MTARGET_ABRT, 0,	"PCI Master Transfer Target Abort" },
    { 0, ERRL_PBUS_PERR,	"Packet Bus Write Parity error" },
    { 0, ERRL_PCI_PERR,		"PCI Parity error" },
    { 0, ERRL_PCI_RETRY_TMOUT,	"PCI Retry time-out" },
    { 0, ERRL_TRDY_TMOUT,	"PCI TRDY time-out" },
    { 0, ERRL_IRDY_TMOUT,	"PCI IRDY time-out" },
    { 0, ERRL_DEVSEL_TMOUT,	"PCI DEVSEL time-out" },
    { 0, ERRL_GNT_TMOUT,	"PCI GNT time-out" },
    { 0, ERRL_PCI_TADDR_TOOBIG,	"PCI Target ADDR exceeds SRAM size" },
    { 0, ERRL_PCI_SERR,		"PCI SERR" },
    { 0, ERRL_PKTMEM_PERR,	"Packet Memory Read Parity error" },
    { 0, ERRL_UNUSED,		"Unknown PMA error 0x00000080" },
    { 0, ERRL_PBUS_ADDR_TOOBIG,	"Packet Bus Addr exceeds SRAM size" },
    { 0, ERRL_PCI_MA_ERR2,	"PCI Master Access failed" },
    { 0, ERRL_PBUS_TMOUT_CYA,	"Packet Bus timeout CYA" },
    { 0, ERRL_PBUS_TMOUT_CPU,	"Packet Bus timeout CPU" },
    { 0, ERRL_PBUS_SM_ERR,	"Packet Bus State Machine error" },
    { 0, ERRL_ILEG_CPU_BWRITE,	"Illegal CPU Burst Write" },
    { 0, ERRL_PCI_MA_ERR1,	"PCI Master Access not enabled" },
    { 0, 0,			NULL }
};

long brief_bus_stall_time = R4K_USEC2COUNT(BRIEF_BUS_STALL_TIME);
long cya_drain_memd_req_time = R4K_USEC2COUNT(100);

/*
 * flush_io_wbuffer:
 * Flush the IO write buffer
 */
int flush_io_wbuffer (void)
{
    /* Read anything off Orion */
    return(IO_ASIC->cr1);
}

/*
 * turn_on_pa_led :
 * Turn on the PA's RUN lamp to indicate that we're up and running
 */
void turn_on_pa_led (int pa_bay)
{
    if (pa_bay == PA0) {
	IO_ASIC->cr1 |= CR1_ENB_PA0_LED ;
    } else if (pa_bay == PA1) {
	IO_ASIC->cr1 |= CR1_ENB_PA1_LED ;
    } else {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
    }
}

/*
 * turn_off_pa_led :
 * Turn off the PA's RUN lamp to indicate that we're down and dieing
 */
void turn_off_pa_led (int pa_bay)
{
    if (pa_bay == PA0) {
	IO_ASIC->cr1 &= (~CR1_ENB_PA0_LED);
    } else if (pa_bay == PA1) {
	IO_ASIC->cr1 &= (~CR1_ENB_PA1_LED);
    } else {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
    }
}

/*
 * pas_pa_present :
 * See if a Port Adaptor is present in the selected bay
 * Use voltage compatibility bits to determine PA presense.
 * 
 */
boolean pas_pa_present (uint pa_bay)
{
    if (pa_bay == PA0) {
	return ((IO_ASIC->nev_isr & NEV_ISR_PA0_5V_COMP) ||
		 (IO_ASIC->nev_isr & NEV_ISR_PA0_3V_COMP) ) ;
    } else if (pa_bay == PA1) {
	return ((IO_ASIC->nev_isr & NEV_ISR_PA1_5V_COMP) ||
		 (IO_ASIC->nev_isr & NEV_ISR_PA1_3V_COMP) ) ;
    } else {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
	return FALSE;
    }

}

/*
 * pas_control_power:
 * Control power on a PA. Only applies to checkers hw rev 2.
 */
void pas_control_power (uint pa_bay, boolean onoff)
{
  
}

static boolean
need_turbo_setting(void)

{

      switch (get_pa_type(0)) {
      case PA_HARDWARE_CT3:
      case PA_HARDWARE_M4T:
      case PA_HARDWARE_M8T_V35:
      case PA_HARDWARE_M8T_X21:
      case PA_HARDWARE_M8T_RS232:
      case PA_HARDWARE_DUAL_HSSI:
        return (FALSE);	

      }
      switch (get_pa_type(1)) {
      case PA_HARDWARE_M4T:
      case PA_HARDWARE_M8T_V35:
      case PA_HARDWARE_M8T_X21:
      case PA_HARDWARE_M8T_RS232:
      case PA_HARDWARE_DUAL_HSSI:
        return (FALSE);	
      }
      /*
       * FIXME: Disable turbo in general for now.
       */
      return (FALSE);
}

/*
 * pas_control_reset:
 * Control reset signal to PA's
 */
void pas_control_reset (uint pa_bay, boolean onoff)
{
    if (onoff) {

	/*
	 * Assert reset
	 */
	if (pa_bay == PA0) {
	    IO_ASIC->cr2 &= ~NEV_CR2_RESET_PA0;
	} else if (pa_bay == PA1) {
	    IO_ASIC->cr2 &= ~NEV_CR2_RESET_PA1;
	} else {
	    errmsg(&msgsym(BADPABAY, PA), pa_bay);
	}

    } else {
      
        /* 
	 * Set pci bus arbiter to turbo mode: Don't set it for CT3 or 
	 * Mueslix based PA's.
	 */
	if (need_turbo_setting()) { 
	    IO_ASIC->cr2 |= CSR2_TURBO_MODE;  
	}
 
	/*
	 * De-assert reset
	 */
	if (pa_bay == PA0) {
	    IO_ASIC->cr2 |= NEV_CR2_RESET_PA0;
	} else if (pa_bay == PA1) {
	    IO_ASIC->cr2 |= NEV_CR2_RESET_PA1;
	} else {
	    errmsg(&msgsym(BADPABAY, PA), pa_bay);
	}

    }
}

/*
 * pas_deactivate_bay :
 * Put the specified port adaptor in reset
 */
void pas_deactivate_bay (uint pa_bay)
{
    if (pa_bay == PA0) {
	IO_ASIC->cr2 &= ~NEV_CR2_RESET_PA0;  /* Reset PA1 - active low */
    } else if (pa_bay == PA1) {
	IO_ASIC->cr2 &= ~NEV_CR2_RESET_PA1;  /* Reset PA1 - active low */
    } else {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
    }
    pas_control_power(pa_bay, FALSE);
}

/*
 * pas_nmi_dispatcher :
 * Call all the interrupt handlers associated with all Ports requesting
 * a Nevada PA NMI interrupt (in a sequence based on their priorities).
 */
void pas_nmi_dispatcher (void)
{
    pa_nmi_handler_t *hdlr_ptr;
    ushort int_status;
    ushort mir = 0;

    int_status = IO_ASIC->pa_isr2;

    if (int_status & PA_ISR2_PA0_NMI_MASK) {
        mir += 1;
    }
    if (int_status & PA_ISR2_PA1_NMI_MASK) {
        mir += 2;
    }

    /* Dispatch all slot interrupt handlers ONCE */
    while (mir) {
        hdlr_ptr = pa_nmi_dispatch_table[mir];
        (hdlr_ptr->func)(hdlr_ptr->arg);
        mir &= hdlr_ptr->mask;
    }

}

/*
 * vip_stop_microcode :
 * Halt the microcode
 */
void vip_stop_microcode (void)
{
    dbus_hardware_regs *r;
    void (*fnc)(void);

    *(volatile ulong *)(R4K_K1BASE + 0x24C) = 0x22222334;

    r = (dbus_hardware_regs *)(ADRSPC_CYA_ASIC);
    r->reset_reg = COLD_RESET;

    /* Clear VIP software ready and port I/O ready */
    r->status &= ~(DBUS_VIP_READY | DBUS_PORT_IO_AVAIL);

    fnc = (void (*)(void))(PHY_TO_KSEG1(PHY_ADRSPC_PROM));
    for (;;) {
	(*fnc)();
    }
}

/*
 * test_parity_io:
 * Test IO memory parity errors
 */
void test_parity_io (void)
{
    printf("\n%%not implemented");
    return;
}

typedef struct mgmt_vector_t {
    vector handler;
    ulong  param;
} mgmt_vector;

struct critical_mgmt_handler_list_t {
    mgmt_vector    list[MAX_VIP_INTERFACES];
    int next;
} critical_mgmt_handler_cb;

/*
 * add_critical_mgmt_handler :
 * add one critical managemnet handler to VIP2
 */
boolean add_critical_mgmt_handler(vector mgmt, ulong param)
{
    leveltype level;
    int i;
    mgmt_vector *vp;

    vp = &critical_mgmt_handler_cb.list[0];
    /*
     * Since this general purpose interface routine could be called by driver
     * from any interrupt level, it is safe to disable all interrupt.
     * It is ok since this routine only runs a few usec.
     */
    level = disable_interrupts();
    for (i=0; i<MAX_VIP_INTERFACES; i++, vp++) {
	if (vp->handler == NULL) {
	    vp->handler = mgmt;
	    vp->param = param;
	    if (critical_mgmt_handler_cb.next < i + 1) {
		critical_mgmt_handler_cb.next = i + 1;
		enable_interrupts(level);
		return(TRUE);
	    }
	}
    }
    enable_interrupts(level);
    return(FALSE);
}

/*
 * del_critical_mgmt_handler :
 * delete one critical managemnet handler from VIP2
 */
boolean del_critical_mgmt_handler(vector mgmt, ulong param)
{
    leveltype level;
    int i, valid_entry;
    mgmt_vector *vp;

    vp = &critical_mgmt_handler_cb.list[0];
    valid_entry = 0;
    /*
     * Since this general purpose interface routine could be called by driver
     * from any interrupt level, it is safe to disable all interrupt.
     * It is ok since this routine only runs a few usec.
     */
    level = disable_interrupts();
    for (i=0; i<critical_mgmt_handler_cb.next; i++, vp++) {
	if (vp->handler == mgmt && vp->param == param) {
	    if (critical_mgmt_handler_cb.next == i + 1) {
		/*
		 * move "next" to the first non-empty (valid) entry
		 */
		critical_mgmt_handler_cb.next = valid_entry + 1;
		enable_interrupts(level);
		return(TRUE);
	    }
	}
	/*
	 * remember the non-empty (valid) entry
	 */
	if (vp->handler)
	    valid_entry = i;
    }
    enable_interrupts(level);
    return(FALSE);
}

/*
 * dispatch_critical_mgmt_handlers :
 * to dispatch all registered critical management routines
 */
void dispatch_critical_mgmt_handlers(ulong stall_condition)
{
    int i;
    mgmt_vector *vp;

    vp = &critical_mgmt_handler_cb.list[0];
    for (i=0; i<critical_mgmt_handler_cb.next; i++, vp++) {
        if (vp->handler) {
            (*vp->handler)(vp->param);
	    /*
	     * critical management routine may take very long time
	     * check whether cbus stall has ended after each dispatch
	     * to prevent pending malu command timeout
	     */
	    if (stall_condition == CYA_CBUS_STALL_END) {
		/*
		 * We've called this routine while we're waiting for the
		 * CYA_CBUS_STALL_END indication
		 */
		if (CYA_ASIC->cntrl.oisr & CYA_CBUS_STALL_END)
		    break;
	    } else {
		/*
		 * We've called this routine while we're waiting for the
		 * CYA_STALL_INDICATION to go away
		 */
		if (!(CYA_ASIC->cntrl.oisr & CYA_STALL_INDICATION))
		    break;
	    }
	}
    }
}

/*
 * recover_from_cya_hang:
 * 1 in 32 CYA may hang when only one memd access is pending and
 * cbus stall occur. This routine will get CYA out of trouble
 */
void recover_from_cya_hang (void)
{
    /*
     * This is a S/W work-around for a CYA H/W bug
     * wait for QE to finish its previous write and then use the write
     * register of QE to initiate an extra MEMD write to wake up
     * CYA if it is hung.
     * Don't write to QE's write data register as QE which has stopped may just
     * put data to write data register
     */
    if (CYA_ASIC->qe.memd_if.stat & QE_CBUS_WRITE_COMPLETE) {
        /*
         * The last bufhdr is reserved for this special VIP2 work-around.
         * Bufhdr is from 0x2000 to 0x10000. The last one is in 0xFFF0.
         */
        CYA_ASIC->qe.memd_if.cwa32 = 0xFFF0;
    }
}

/*
 * svip_cbus_stall_handler:
 * Handle an SVIP Cbus Stall interrupt
 */
static void svip_cbus_stall_handler (r4k_context_t *ptr)
{
    ulong oisr;
    usec_timestamp start_time;
 
    DAS_DEBUG(0x77777777);
 
    oisr = CYA_ASIC->cntrl.oisr;
    if (!(oisr & CYA_CBUS_STALL_START)) {
        /*
         * this may be called from svip_error_interrupt_handler
         */
        return;
    }
 
    stop_qe();
    USEC_GET_TIMESTAMP(start_time);
 
    vsys_vec.bus_stall++;
 
    if (oisr & CYA_RD_DURING_STALL) {
        /*
         * This is a S/W work-around for a CYA H/W bug.
         * When memd read collides with cbus stall, CYA_CBUS_STALL_START
         * cannot be cleared until the pending memd read has completed
         * after cbus stall is over. Because there may be many memd write
         * and post read and the worst case is about 100 us for the pending
         * memd read to be disptached. The work-around is to clear
         * CYA_STALL_INDICATION wait for at least 100 us and check
         * CYA_STALL_INDICATION again. If flag is not raised again for the
         * 100 us period, the pending memd read must have completed.
         */
        for (;;) {
            usec_timestamp dispatch_time;
 
            CYA_ASIC->cntrl.oicr = CYA_STALL_INDICATION;
            /*
             * Once clear CYA_STALL_INDICATION wait at least 100 us
             */
            USEC_GET_TIMESTAMP(dispatch_time);
            recover_from_cya_hang();
            /*
             * dispatch critical PA mgmt routines only the stall is longer than
             * 500 us
             */
            if (USEC_ELAPSED_TIME(start_time) > brief_bus_stall_time)
                dispatch_critical_mgmt_handlers(CYA_STALL_INDICATION);
            /*
             * wait at least 100 us after clearing CYA_STALL_INDICATION
             */
            while (USEC_ELAPSED_TIME(dispatch_time) < cya_drain_memd_req_time);
 
            if (!(CYA_ASIC->cntrl.oisr & CYA_STALL_INDICATION))
                break;
        }
    }

    /*
     * CYA_CBUS_STALL_END can be set only after CYA_CBUS_STALL_START is cleared
     * Wait until CYA_CBUS_STALL_END is set
     */
    CYA_ASIC->cntrl.oicr = CYA_CBUS_STALL_START;
    for (;;) {
        recover_from_cya_hang();
        /*
         * dispatch critical PA mgmt routines only the stall is longer than
         * 500 us
         */
        if (USEC_ELAPSED_TIME(start_time) > brief_bus_stall_time)
            dispatch_critical_mgmt_handlers(CYA_CBUS_STALL_END);
        if (CYA_ASIC->cntrl.oisr & CYA_CBUS_STALL_END)
            break;
    }
 
    /*
     * clear all remaining bits set in oisr register
     */
    oisr = CYA_ASIC->cntrl.oisr &
                (CYA_BUS_ERROR_EVENTS | CYA_STALL_INDICATION
                | CYA_STALL_IN_PROGRESS | CYA_CBUS_STALL_END);
    if (oisr)
        CYA_ASIC->cntrl.oicr = oisr;
 
    /*
     * cbus stall interrupt blocks the interrupt to update timestamp
     * need to correct the timestamp for long cbus stall
     */
    check_clock_loss();
    start_qe();
}

/*
 * throttle_netio:
 * Either enable or disable netio interrupts. When TRUE, disable
 * the interrupts. Else enable them. Note, this is called from process
 * level, netio level, timer interrupt level and OIR interrupt level.
 */
static inline void throttle_netio (boolean throttle)
{
    if (throttle) {
	/*
	 * Turn all network IO interrupts off including PA interrupts and
         * DMA interrupts
	 */
	IO_ASIC->pa_imr = 0;
	CYA_ASIC->cntrl.oier &= ~CYA_QE_DMA_INTR;

    } else {
	/*
	 * Turn network IO interrupts on
	 */

	IO_ASIC->tcr = 0;
	IO_ASIC->pa_imr = memd_cntl.pa_int_mask;
	CYA_ASIC->cntrl.oier |= CYA_QE_DMA_INTR;

    }
}

/*
 * throttle_off_inline:
 * Re-enable netio interrupts.
 */
static inline void throttle_off_inline (void)
{
    netint_throttle_active = FALSE;
    USEC_GET_TIMESTAMP(netint_exit);
    netint_exit += netint_ticks;
    throttle_netio(FALSE);
}

/*
 * netint_throttle_off:
 * Re-enable netio interrupts. Called from timer tick.
 */
void netint_throttle_off (void)
{
    netint_timer_count += 1;
    throttle_off_inline();
}

/*
 * process_throttle_off:
 * Re-enable netio interrupts. Called from scheduler and shutdown code.
 */
static void process_throttle_off (void)
{
    leveltype level;

    level = raise_interrupt_level(NMI_LEVEL);

    throttle_off_inline();

    reset_interrupt_level(level);
}

/*
 * svip_netint_throttle_init:
 * Initialize net IO interrupt throttling
 */
void svip_netint_throttle_init (void)
{
    /*
     * Init timestamps
     */
    USEC_GET_TIMESTAMP(netint_return);
    netint_exit = netint_return;

    /*
     * Init variables
     */
    netint_throttle_active = FALSE;
    netint_throttle_count = 0;
    netint_timer_count = 0;
    netint_ticks = R4K_USEC2COUNT(NETINT_DEFAULT);
    netint_mask_usec = NETINT_MASK_DEFAULT;
    netint_mask_ticks = R4K_USEC2COUNT(netint_mask_usec);
    svip_netint_throttle_configured = NETINT_THROTTLE_DEFAULT;

}

/*
 * svip_netint_throttle_setup:
 * Called at the beginning of the net IO intr loop. Setup the exit timer
 * for the interrupt handler
 */
void svip_netint_throttle_setup (void)
{
    ulong diff;
    usec_timestamp now;

    USEC_GET_TIMESTAMP(now);
    diff = now - netint_return;
    if (diff >= netint_mask_ticks) {
	netint_exit = now + netint_ticks;
    } else {
	netint_exit += diff;
    }
}

/*
 * svip_netint_throttle_check:
 * Called from the net IO interrupt handler every time it loops.
 * Check to see if we need to disable the interrupts. If we disabled the
 * interrupts, return TRUE.
 */
boolean svip_netint_throttle_check (void)
{

    if (USEC_AWAKE(netint_exit)) {

	/*
	 * Account for it and disable the interrupts
	 */
	netint_throttle_active = TRUE;
	netint_throttle_count += 1;
	throttle_netio(TRUE);

	/*
	 * Startup the nevada timer to allow re-enabling of
	 * the net IO interrupt but don't start it yet
	 */
	IO_ASIC->tvr = netint_mask_usec;
	
	return(TRUE);
    }
    return(FALSE);
}

/*
 * svip_sched_loop_end:
 * Called at the bottom of the scheduler loop. Turns off throttling
 * IE turns net IO interrupts back on if active and nothing is
 * happening at process level
 */
void svip_sched_loop_end (boolean ran_norm)
{
    if (svip_netint_throttle_configured) {
	if (!ran_norm && netint_throttle_active) {
	    process_throttle_off();
	}
    }
}

/*
 * sched_alloc_command:
 * Handle "scheduler allocate" config command
 */
void sched_alloc_command (parseinfo *csb)
{
    if (csb->nvgen) {
	/*
	 * emit a config line if any value differs from its default
	 */
	if ((svip_netint_throttle_configured != NETINT_THROTTLE_DEFAULT) ||
	    (netint_ticks != R4K_USEC2COUNT(NETINT_DEFAULT)) ||
	    (netint_mask_usec != NETINT_MASK_DEFAULT)) {

	    nv_write(TRUE, "%s%s",
		     (svip_netint_throttle_configured) ? "" : "no ",
		     csb->nv_command);
	    /*
	     * emit parameters only if it's ON
	     */
	    if (svip_netint_throttle_configured)
		nv_add(TRUE, " %u %u",
		       R4K_COUNT2USEC(netint_ticks), netint_mask_usec);
	}
	return;
    }

    if (csb->set_to_default) {
	csb->sense = NETINT_THROTTLE_DEFAULT;
	GETOBJ(int,1) = 0;
    }
    if (csb->sense && !svip_netint_throttle_configured) {
	/*
	 * extra init if it's being switched on
	 */
	USEC_GET_TIMESTAMP(netint_return);
	netint_exit = netint_return;
    }
    if (GETOBJ(int,1) == 0) {
	netint_ticks = R4K_USEC2COUNT(NETINT_DEFAULT);
	netint_mask_usec = NETINT_MASK_DEFAULT;
    } else {
	netint_ticks = R4K_USEC2COUNT(GETOBJ(int,1));
	netint_mask_usec = GETOBJ(int,2);
    }
    netint_mask_ticks = R4K_USEC2COUNT(netint_mask_usec);
    svip_netint_throttle_configured = csb->sense;
}

/*
 * vip_netint_throttle_show:
 * Display info about network IO interrupt throttling
 */
void vip_netint_throttle_show (void)
{
    printf("VIP2 Network IO Interrupt Throttling:\n");
    printf(" throttle count=%u, timer count=%u\n",
	   netint_throttle_count, netint_timer_count);
    printf(" active=%u, configured=%u\n", 
	   netint_throttle_active, svip_netint_throttle_configured);
    printf(" netint usec=%u, netint mask usec=%u\n",
	   R4K_COUNT2USEC(netint_ticks), netint_mask_usec);
}

/*
 * install_cbus_stall_handler :
 * Install the CBus Stall interrupt handler and Initialize the CYA
 * This routine must be called before the CYA is utilized (memd accesses)
 */
void install_cbus_stall_handler (void)
{
    ulong temp;

    /*
     * Create the interrupt stack
     */
    createlevel(LEVEL_CBUS_STALL, (void (*)(void))svip_cbus_stall_handler,
		"CBUS Stall Interrupt");

    /* We had a reset here of the CYA but that ends up wiping out the logical
       slot number :-(   */
    temp = CYA_ASIC->cfg;
    temp &= ~CYA_MCR_BCR;
    CYA_ASIC->cfg = temp;

    /*
     * Set up the CYA Other Interrupt Enable Register
     */
    CYA_ASIC->cntrl.oier = 
        CYA_BUS_ERROR_EVENTS    |
	CYA_CBUS_STALL_START    |
	CYA_QE_DMA_INTR         |
	CYA_DBUS_INTR		|
        CYA_MALU_ATTN;		 
}

/*
 * install_cbus_handler :
 * Install the CBus interrupt handler and initialize command Queue
 */
void install_cbus_handler (void)
{
    /*
     * Create the interrupt stack
     */
    createlevel(LEVEL_CBUS, NULL, "CBUS Interrupt");

    /*
     * Initialize the vip generic memd support. We are a Viper.
     */
    vip_init_memd_support(TRUE);
}


/*
 * svip error_interrupt_handler:
 * Handle all flavors of viper hardware errors.
 * The Viper Nevada equivalent supports an error interrupt status register 
 * That is edge sensitive.  Reading the register clears the interrupts.
 */

static void svip_error_interrupt_handler (r4k_context_t *ptr)
{
    ushort nevada_error;
    ulong err_addr;
    struct {
        ulong upper;
        ulong lower;
    } split64;
    char trace_msg[130];

    boolean got_error = FALSE;      /* Deal with spurious interrupts */
    boolean print_pma_reg = FALSE;

    DAS_DEBUG(0x77777755);
    /*
     * Get some info from the hardware
     */
    nevada_error = IO_ASIC->be_isr;
    sprintf(trace_msg, "Nevada Error register %#x\n", nevada_error);
    ttyprintf(CONTTY, "\n\n%s",trace_msg);
    vip_logger_write_str(LOG_ALERT, trace_msg);

    /*
     * Decode the error in detail
     */
    if (nevada_error & BE_ISR_CYA_ERR_INTR) {
	/*
	 * CYA error interrupt - we need to read the cya for status
	 * cbus stall should not come here normally. It did when there was
	 * spurious interrupt in the earlier version. Those spurious
	 * interrupts are fixed now. However we keep this protection
	 * in case there is one more spurious interrupt condition remains.
	 */
	CYA_ASIC->cntrl.eicr = CYA_ASIC->cntrl.eisr;
	svip_cbus_stall_handler(ptr);
	DAS_DEBUG(0x77777788);
	return;
    }

    if (nevada_error & BE_ISR_CYA_ERR_INTR) {
  	/*
  	 * CYA error interrupt - we need to read the cya for status
  	 */
 	sprintf(trace_msg, "CYASIC Error register %#x, Other Intrpt register %#x\n",
 		CYA_ASIC->cntrl.eisr, CYA_ASIC->cntrl.oisr);
 	ttyprintf(CONTTY, "\n\n%s",trace_msg);
 	vip_logger_write_str(LOG_ALERT, trace_msg);
 
  	CYA_ASIC->cntrl.eicr = CYA_ASIC->cntrl.eisr;
 
 	sprintf(trace_msg, "CYBUS Error register %#x, PKT Bus Error register %#x\n",
 		CYA_ASIC->cntrl.cber, CYA_ASIC->cntrl.pber);
 	ttyprintf(CONTTY, "%s", trace_msg);
 	vip_logger_write_str(LOG_ALERT, trace_msg);
 	got_error = TRUE;
    }
  
    if (nevada_error & BE_ISR_PMA_ERR_INTR) {
	/*
	 * PMA error interrupt
	 */
	got_error = TRUE;
	print_pma_reg = TRUE;
    }

    if (nevada_error & BE_ISR_PCI0_SYS_ERR) {
	/*
	 * PCI System error - PCI bus 0
	 */
	sprintf(trace_msg,"PCI bus 0 system error\n");
	ttyprintf(CONTTY, "%s", trace_msg);
	vip_logger_write_str(LOG_ALERT, trace_msg);
        got_error = TRUE;
	print_pma_reg = TRUE;
    }

    if (nevada_error & BE_ISR_PCI1_SYS_ERR) {
	/*
	 * PCI System error - PCI bus 1
	 */
	sprintf(trace_msg,"PCI bus 1 system error\n");
	ttyprintf(CONTTY, "%s", trace_msg);
	vip_logger_write_str(LOG_ALERT, trace_msg);
        got_error = TRUE;
	print_pma_reg = TRUE;
    }

    if (nevada_error & BE_ISR_PCI2_SYS_ERR) {
	/*
	 * PCI System error - PCI bus 2
	 */
	sprintf(trace_msg,"PCI bus 2 system error\n");
	ttyprintf(CONTTY, "%s", trace_msg);
	vip_logger_write_str(LOG_ALERT, trace_msg);
        got_error = TRUE;
	print_pma_reg = TRUE;  
    }

    if (nevada_error & BE_ISR_PCI0_PAR_ERR) {
	/*
	 * PCI Parity error - PCI bus 0
	 */
	sprintf(trace_msg,"PCI bus 0 parity error\n");
	ttyprintf(CONTTY, "%s", trace_msg);
	vip_logger_write_str(LOG_ALERT, trace_msg);
        got_error = TRUE;
	print_pma_reg = TRUE;
    }

    if (nevada_error & BE_ISR_PCI1_PAR_ERR) {
	/*
	 * PCI Parity error - PCI bus 1
	 */
	sprintf(trace_msg,"PCI bus 1 parity error\n");
	ttyprintf(CONTTY, "%s", trace_msg);
	vip_logger_write_str(LOG_ALERT, trace_msg);
        got_error = TRUE;
	print_pma_reg = TRUE;
    }

    if (nevada_error & BE_ISR_PCI2_PAR_ERR) {
	/*
	 * PCI Parity error - PCI bus 2
	 */
	sprintf(trace_msg,"PCI bus 2 parity error\n");
	ttyprintf(CONTTY, "%s", trace_msg);
	vip_logger_write_str(LOG_ALERT, trace_msg);
        got_error = TRUE;
	print_pma_reg = TRUE;
    }

    if (print_pma_reg) {
	const pma_err_parse *pma_err;
 
	move64((u64 *)&PMA_ASIC->err_reg, (u64 *)&split64);
	sprintf(trace_msg,"PMA error register = %08x%08x\n", split64.upper,
		split64.lower);
	ttyprintf(CONTTY, "%s", trace_msg);
	vip_logger_write_str(LOG_ALERT, trace_msg);
 
	for (pma_err=&pma_errs[0]; pma_err->err_desc; pma_err++) {
	    if ((split64.upper & pma_err->err_upper_mask) ||
		(split64.lower & pma_err->err_lower_mask)) {
		sprintf(trace_msg, "    %s\n", pma_err->err_desc);
		ttyprintf(CONTTY, "%s", trace_msg);
		vip_logger_write_str(LOG_ALERT, trace_msg);
	    }
	}
	sprintf(trace_msg, "    PCI master address = %07x\n",
		split64.upper & ERRU_PCI_MASTER_ADDR);
	ttyprintf(CONTTY, "%s", trace_msg);
	vip_logger_write_str(LOG_ALERT, trace_msg);
    }
 
    if (nevada_error & BE_ISR_RENO_WR_PAR_ERR01) {
	/*
	 * Reno Write Parity Error - bytes 0 and 1
	 */
	sprintf(trace_msg,"Reno write parity error - bytes 0 & 1\n");
	ttyprintf(CONTTY, "%s", trace_msg);
	vip_logger_write_str(LOG_ALERT, trace_msg);
        got_error = TRUE;
    }

    if (nevada_error & BE_ISR_RENO_RD_PAR_ERR01) {
	/*
	 * Reno Read Parity Error - bytes 0 and 1
	 */
	sprintf(trace_msg,"Reno read parity error - bytes 0 & 1\n");
	ttyprintf(CONTTY, "%s", trace_msg);
	vip_logger_write_str(LOG_ALERT, trace_msg);
	got_error = TRUE;
    }

    if (nevada_error & BE_ISR_RENO_WR_PAR_ERR23) {
	/*
	 * Reno Write Parity Error - bytes 2 and 3
	 */
	sprintf(trace_msg,"Reno write parity error - bytes 2 & 3\n");
	ttyprintf(CONTTY, "%s", trace_msg);
	vip_logger_write_str(LOG_ALERT, trace_msg);
	got_error = TRUE;
    }

    if (nevada_error & BE_ISR_RENO_RD_PAR_ERR23) {
	/*
	 * Reno Read Parity Error - bytes 2 and 3
	 */
	sprintf(trace_msg,"Reno read parity error - bytes 2 & 3\n");
	ttyprintf(CONTTY, "%s", trace_msg);
	vip_logger_write_str(LOG_ALERT, trace_msg);
	got_error = TRUE;
    }

    if (nevada_error & BE_ISR_DRAM_RENO_WR_BUS_ERR) {
	/*
	 * DRAM Reno write bus error
	 */
	sprintf(trace_msg,"DRAM Reno write bus error\n");
	ttyprintf(CONTTY, "%s", trace_msg);
	vip_logger_write_str(LOG_ALERT, trace_msg);
        err_addr = *(volatile ulong *)(ADRSPC_DRAM_WBERR);
	sprintf(trace_msg," Error address = 0x%08x, %s", err_addr,
                (err_addr & 0x01) ? "(valid bit set)\n" :
                                    "(valid bit NOT set)\n");
	ttyprintf(CONTTY, "%s", trace_msg);
	vip_logger_write_str(LOG_ALERT, trace_msg);	got_error = TRUE;
	got_error = TRUE;
    }

    if (nevada_error & BE_ISR_RENO_IO_BUS_ERR) {
	/*
	 * I/O Reno bus error
	 */
	sprintf(trace_msg,"IO Reno bus error\n");
	ttyprintf(CONTTY, "%s", trace_msg);
	vip_logger_write_str(LOG_ALERT, trace_msg);
        err_addr = *(volatile ulong *)(ADRSPC_IO_WBERR);
	sprintf(trace_msg," Error address = 0x%08x, %s", err_addr,
                (err_addr & 0x01) ? "(valid bit set)\n" :
                                    "(valid bit NOT set)\n");
	ttyprintf(CONTTY, "%s", trace_msg);
	vip_logger_write_str(LOG_ALERT, trace_msg);
	got_error = TRUE;
    }

    if (got_error) {
	/*
	 * Flush the write buffers and reload
	 */
	*(volatile ulong *)(R4K_K1BASE + 0x24C) = 0x22222299;

	flush_io_wbuffer();
	mon_cpu_exception(SIGRELOAD, 0, ptr);
    }
}

/*
 * svip_read_bus_error_handler:
 * If memory access which causes bus error is a MEMD access
 * then simply return to execute the instruction which hit bus error
 * The instruction will succeeds once cbus stall which blocks MEMD access
 * is over.
 */
static void svip_read_bus_error_handler (int signal_no, int code, r4k_context_t *sc)
{
    union instr_r4k *op;

    DAS_DEBUG(0x77777766);
    op = check_load_inst(sc);
    if (op) {
	ulong addr;

	addr = r4k_get_cpu_reg(sc, op->i_t.rs);
	addr += op->i_t.s_imd;
	if (is_memd_addr(addr)) {
	    return;
	}
    }
    handle_exception(signal_no, code);
}

/*
 * install_svip_error_handler:
 * Install the error handler. Note, interrupts are locked out
 */
void install_svip_error_handler (void)
{
    SIG_PF sigbus_save;

    sigbus_save = signal(SIGBUS, (SIG_PF)svip_read_bus_error_handler);

    /* 
     * Create Error handler interrupt stack
     */

    createlevel(LEVEL_ERROR, (void (*)(void))svip_error_interrupt_handler, 
		"Error Interrupt");

    /* Enable packet bus parity errors */
    IO_ASIC->cr1 |= CR1_PB_ERR_EN;

    /* FIXME - Anything to do for PMA? */

    /* Set up the CYA Error Interrupt Enable Register */
    CYA_ASIC->cntrl.eier |= 
	CYA_BE_FROM_PMA_ON_CYA  | 
	CYA_OVF_INT_ARB_FIFO    | 
	CYA_DMA_XMT_ERR         | 
        CYA_DMA_RCV_ERR         | 
        CYA_QE_PGM_MPAR_INTR    | 
        CYA_ILL_ADRS_FROM_CPU   | 
	CYA_RETRY_NOT_SAME_ADRS | 
	CYA_INV_BE_FROM_CPU   	|
	CYA_SINGLE_IDLE_CYCLE  	|
	CYA_ODD_PBUS_GNTS  	|
	CYA_PAR_ERR_PBUS  	|
	CYA_PAR_ERR_INTERNAL  	|
	CYA_PAR_ERR_CBUS  	|
        CYA_MISSING_ACK_CBUS  	|
        CYA_NACK_PRESENT_CBUS;
}

void
get_pa_conf_type(void)

{
    ushort i;
    vsys_vec.pa_conf_type= 1;   /* just make sure it's init to 1 */
    for(i=0; i<=1; i++){           
      switch (get_pa_type(i)) {
      case PA_HARDWARE_1FE: 
	  vsys_vec.pa_conf_type &= PA_FAST; 
	  break;
      case PA_HARDWARE_1FE_TX_ISL:
	  vsys_vec.pa_conf_type &= PA_FAST; 
	  break;
      case PA_HARDWARE_1FE_FX_ISL :
	  vsys_vec.pa_conf_type &= PA_FAST; 
	  break;
      case PA_HARDWARE_1FE_TX_NISL : 
	  vsys_vec.pa_conf_type &= PA_FAST; 
	  break;
      case PA_HARDWARE_C7100_IO1FE :
	  vsys_vec.pa_conf_type &= PA_FAST; 
	  break;
      case PA_HARDWARE_FDDI_DAS_MM: 
	  vsys_vec.pa_conf_type &= PA_FAST; 
	  break;
      case PA_HARDWARE_FDDI_DAS_SS: 
	  vsys_vec.pa_conf_type &= PA_FAST; 
	  break;
      case PA_HARDWARE_CE1:
	  vsys_vec.pa_conf_type &= PA_FAST;
	  break;
      case PA_HARDWARE_CT1:
	  vsys_vec.pa_conf_type &= PA_FAST;
	  break;
      case PA_HARDWARE_CT3:
	  vsys_vec.pa_conf_type &= PA_FAST;
	  break;
      case PA_HARDWARE_T1_1PORT_CSU:
	  vsys_vec.pa_conf_type &= PA_FAST;
	  break;
      case PA_HARDWARE_E1_1PORT:
	  vsys_vec.pa_conf_type &= PA_FAST;
	  break;
      case PA_HARDWARE_UNKNOWN : 
	  vsys_vec.pa_conf_type &= PA_FAST;
	  break;
      default: 
	  vsys_vec.pa_conf_type &= PA_SLOW; 
      break;
    } 
  }
}

boolean
is_channelized(ushort pa_bay)
  
{
  
    switch (get_pa_type(pa_bay)) {
    case PA_HARDWARE_CE1: 
        return (TRUE);
    case PA_HARDWARE_CT1: 
	return (TRUE);
    case PA_HARDWARE_CT3:
	return (TRUE);
    case PA_HARDWARE_T1_1PORT_CSU:
	return (TRUE);          
    case PA_HARDWARE_T1_2PORT_CSU:
	return (TRUE);
    case PA_HARDWARE_E1_1PORT:
	return (TRUE);
    case PA_HARDWARE_E1_2PORT:
	return (TRUE);
    default:
	return (FALSE);
    }
}
 
/* end of file */
