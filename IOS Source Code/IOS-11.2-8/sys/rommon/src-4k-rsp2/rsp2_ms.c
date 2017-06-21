/* $Id: rsp2_ms.c,v 3.7.10.3 1996/08/06 20:37:19 tkam Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-rsp2/rsp2_ms.c,v $
 *------------------------------------------------------------------
 * rsp2_ms.c
 *
 * Oct, 95, Steve J. Zhang
 *
 * Copyright (c) 1995 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp2_ms.c,v $
 * Revision 3.7.10.3  1996/08/06  20:37:19  tkam
 * CSCdi59454:  Misspelling of word master when dual RSP2 is reloaded
 * Branch: California_branch
 * Added extra linefeed to prevent overlaying the last line of IOS message.
 *
 * Revision 3.7.10.2  1996/04/19  18:03:48  mansonw
 * CSCdi54937:  Usage message for ROMMOM setms command has extra character
 * Branch: California_branch
 * deleted extra character
 *
 * Revision 3.7.10.1  1996/03/21  23:33:05  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/19  01:01:48  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/08  08:59:46  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/01  04:24:39  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.7  1996/02/22  02:59:39  tkam
 * CSCdi49547:  commingled rommon messages when using Y-cable
 *
 * Revision 3.6  1996/02/09  00:02:22  tkam
 * CSCdi47873:  HSA ROMMON needs to add flow control to dbus download
 * Add download flow control to HSA ROMMON.
 *
 * Revision 3.5  1996/02/01  02:27:39  mansonw
 * CSCdi47569:  HSA slave refuses to switch to master state
 * Fixed the HSA slave to gain master during hot insertion/removal
 *
 * Revision 3.4  1996/01/21  06:26:51  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 * Fix the dbus handler linker script and change the arbitration timer
 *
 * Revision 3.3  1996/01/17  23:32:29  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:47:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:09:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  22:32:55  szhang
 * placeholder for rsp2_ms.c
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* -----------------------------------------------------------------------
 * This module contains code for master and slave operation.
 * -----------------------------------------------------------------------
 */
#include "monitor.h"
#include "signal_4k.h"
#include "dclasic.h"

#include "dgbus.h"
#include "ci_dbus.h"

/* ERROR CODE */
#define MS_MASTER_SLAVE_SET   0
#define MS_ERROR_SET_SLAVE 0xF1
#define MS_ERROR_SET_MASTER 0xF2
#define MS_ERROR_GLOBAL_LOCK 0xF3


#define DEFAULT_TIMER1  0x1
#define DEFAULT_TIMER2  0x4000
#define DEFAULT_TIMER3  0x8000
#define DEFAULT_TIMER_MARGIN  0x2000

#define NVRAM_STRUCT_START  ((struct nvram *)(KSEG1_ADRSPC_NVRAM))


void dump_ms_status();
int get_slot();

char *this_rsp2_state[] = {"This RSP2 is non-participant",
			   "This RSP2 is system slave",
			   "This RSP2 is system master",
			   "This RSP2 is not plugged in"};

char *other_rsp2_state[] = {"Other RSP2 is non-participant",
			    "Other RSP2 is system slave",
			    "Other RSP2 is system master",
			    "Other RSP2 is not plugged in"};

char *this_rsp2_msg[] = {"RSP2 is non-participant",
			 "RSP2 is system slave",
			 "RSP2 is system master",
			 "RSP2 is not plugged in"};

char *other_rsp2_msg[] = {"RSP2 is non-participant",
			  "RSP2 is system slave",
			  "RSP2 is system master",
			  "RSP2 is not plugged in"};

char *RCV_present[] = {"Other RSP2 is not installed",
		       "Other RSP2 is installed"};

/* 
** This is used to calculate the other slot #
*/
#define MAX_PAIR 8
int slot_pairs[MAX_PAIR] = {0, 0, 3, 2, 0, 0, 7, 6};

unsigned long *ms_timers[2][2] = {&NVRAM_STRUCT_START->ms_timer2,
				  &NVRAM_STRUCT_START->ms_timer3,
			          &NVRAM_STRUCT_START->ms_timer1,
			          &NVRAM_STRUCT_START->ms_timer3};


struct ms_state {
    unsigned long state;
    unsigned long crashed;
    void (*action)();
};

/*
** This RSP2 state transition table
** 
*/
struct ms_state this_states[STATE_MAX][INPUT_MAX] = {
    STATE_UNKNOWN, FALSE, 0,
    STATE_SLAVE, FALSE, 0,
    STATE_MASTER, FALSE, 0,
    STATE_UNKNOWN, FALSE, 0,

    STATE_NON_PART, TRUE, do_reset,
    STATE_UNKNOWN, FALSE, 0,
    STATE_MASTER, FALSE, 0,
    STATE_UNKNOWN, FALSE, 0,

    STATE_NON_PART, FALSE, do_reset,
    STATE_SLAVE, FALSE, 0,
    STATE_UNKNOWN, FALSE, 0,
    STATE_UNKNOWN, FALSE, 0,

    STATE_NON_PART, FALSE, 0,
    STATE_SLAVE, FALSE, 0,
    STATE_MASTER, FALSE, 0,
    STATE_UNKNOWN, FALSE, 0};

/*
** Other RSP2 state transition table
*/
struct ms_state other_states[STATE_MAX][INPUT_MAX] = {
    STATE_UNKNOWN, FALSE, 0,
    STATE_SLAVE, FALSE, 0,
    STATE_MASTER, FALSE, 0,
    STATE_UNPLUGGED, FALSE, 0,
    
    STATE_NON_PART, FALSE, 0,
    STATE_UNKNOWN, FALSE, 0,
    STATE_MASTER, FALSE, 0,
    STATE_UNPLUGGED, FALSE, 0,

    STATE_NON_PART, FALSE, do_reset, 
    STATE_SLAVE, FALSE, do_reset, 
    STATE_UNKNOWN, 0, 0,
    STATE_UNPLUGGED, FALSE, do_reset,

    STATE_NON_PART, FALSE, 0,
    STATE_SLAVE, FALSE, 0,
    STATE_MASTER, FALSE, 0,
    STATE_UNKNOWN, FALSE, 0};


/*
** This function is to arbitration the Master/Slave status in 
** the dual RSP2 situation. 
**/
int
ms_arbitration()
{
    int even_slot;
    int timer_value;
    int ms_status;

    /*
     * Initialize the timers if they are not initialized.
     */

    if (!nvramvalid()) { /* use checksum to check validity of nvram */
	NVRAM_SAV->ms_timer1 = DEFAULT_TIMER1;
	NVRAM_SAV->ms_timer2 = DEFAULT_TIMER2;
	NVRAM_SAV->ms_timer3 = DEFAULT_TIMER3;
	NVRAM_SAV->ms_timer_margin = DEFAULT_TIMER_MARGIN;
    }

    if ((NVRAM_SAV->default_master != 0) && (NVRAM_SAV->default_master != 1))
	NVRAM_SAV->default_master = 0;

    if ((NVRAM_SAV->crashed != 0) && (NVRAM_SAV->crashed != 1))
	NVRAM_SAV->crashed = 0;

    if ((NVRAM_SAV->this_current_state < STATE_NON_PART) || 
	(NVRAM_SAV->this_current_state > STATE_MASTER))
	NVRAM_SAV->this_current_state = STATE_NON_PART;

    if ((NVRAM_SAV->other_current_state < STATE_NON_PART) ||
	(NVRAM_SAV->other_current_state > STATE_MASTER))
	NVRAM_SAV->other_current_state = STATE_NON_PART;

    if ((NVRAM_SAV->slave_boot != TRUE) && (NVRAM_SAV->slave_boot != FALSE))
	NVRAM_SAV->slave_boot = FALSE;

    if ((REG_IRSP2DBUS_FPGA->MasterSlaveStatus & MS_STATUS_OTHER_IRSP2_PRESENT ) == 0) {
	ms_status = set_master();
	NVRAM_SAV->crashed = FALSE;
	return(ms_status);
    }
    else {
	even_slot = get_slot();
	timer_value = *(ms_timers[NVRAM_SAV->default_master][NVRAM_SAV->crashed]);
	if (even_slot == FALSE)
	    timer_value += NVRAM_SAV->ms_timer_margin;
#ifdef DEBUG
        printf("\ntimer_value = %x\n", timer_value);
#endif
        delay_10us(timer_value * 3); /* times 3 to change arbitration timer */
                                     /* without changing the default value */
	ms_status = set_master();
	NVRAM_SAV->crashed = FALSE;
	return(ms_status);
    }
} /* ms_arbitration */


/*
** This function set the machine to be master.
** If set_master() fails, it should return various error code.
**/

int
set_master()
{
    /* request the global lock */
    REG_FPGA->cbus_lock |= CBUS_GLOBAL_LOCK_REQ;

    if ((REG_IRSP2DBUS_FPGA->MasterSlaveStatus & MS_STATUS_OTHER_IRSP2_MASK) == MS_STATUS_OTHER_IRSP2_MASTER) {
	/* set the RSP2 to be slave */
	REG_IRSP2DBUS_FPGA->MasterSlaveCmd |= MS_CMD_IRSP2_SLAVE;
	if (REG_IRSP2DBUS_FPGA->MasterSlaveStatus & MS_STATUS_THIS_IRSP2_SLAVE == 0) {
	    REG_FPGA->cbus_lock &= ~CBUS_GLOBAL_LOCK_REQ;
	    return(MS_ERROR_SET_SLAVE);
	}
	else {
	    REG_FPGA->cbus_lock &= ~CBUS_GLOBAL_LOCK_REQ;
	    return(MS_MASTER_SLAVE_SET);
	}
    } /* other RSP2 is master */

    if (REG_FPGA->cbus_lock & CBUS_GLOBAL_LOCK_GRANT) {
	/* set this RSP2 to be master */
	REG_IRSP2DBUS_FPGA->MasterSlaveCmd |= MS_CMD_IRSP2_MASTER;
	if ((REG_IRSP2DBUS_FPGA->MasterSlaveStatus & MS_STATUS_THIS_IRSP2_MASK) == MS_STATUS_THIS_IRSP2_MASTER) {
	    REG_FPGA->cbus_lock &= ~CBUS_GLOBAL_LOCK_REQ;
	    return(MS_ERROR_SET_MASTER);
	}
	else {
	    REG_FPGA->cbus_lock &= ~CBUS_GLOBAL_LOCK_REQ;
	    return(MS_MASTER_SLAVE_SET);
	}
    }
    else {
	REG_FPGA->cbus_lock &= ~CBUS_GLOBAL_LOCK_REQ;
	return(MS_ERROR_GLOBAL_LOCK);
    }
} /* set_master */



/*
** Function: setvars()
**
** Desc: sets the master slave arbitration timers
**
*/
int
setvars(int argc, char *argv[])
{

    unsigned long ivalue;

    if (argc != 3) {
usage:
        printf("usage: %s <variable name> value\n", argv[0]);
        printf("       where <variable name >: mstimer1, mstimer2, mstimer3,\n");
        printf("                               ms_timer_margin, crashed, slave_boot\n");
        return(1);
    }
 
    getnum(argv[2], 16, (long *)&ivalue);
 
    if (strcmp(argv[1], "mstimer1") == 0) {
        NVRAM_SAV->ms_timer1 = ivalue;
        return(0);
    } else if (strcmp(argv[1], "mstimer2") == 0) {
        NVRAM_SAV->ms_timer2 = ivalue;
        return(0);
    } else if (strcmp(argv[1], "mstimer3") == 0) {
        NVRAM_SAV->ms_timer3 = ivalue;
        return(0);
    } else if (strcmp(argv[1], "ms_timer_margin") == 0) {
        NVRAM_SAV->ms_timer_margin = ivalue;
        return(0);
    } else if (strcmp(argv[1], "crashed") == 0) {
        NVRAM_SAV->crashed = ivalue;
        return(0);
    } else if (strcmp(argv[1], "slave_boot") == 0) {
        NVRAM_SAV->slave_boot = ivalue;
        return(0);
    } else
        goto usage;
} 


/*
** Function: setms()
**
** Desc: set the master and slave variable
*/
int 
setms(int argc, char *argv[])
{
    if (argc != 2) {
usage:
	printf("usage: %s [master|slave]\n", argv[0]);
	return(1);
    }

    if (strcmp(argv[1], "master") == 0) {
	NVRAM_SAV->default_master = TRUE;
	return(0);
    } else if (strcmp(argv[1], "slave") == 0) {
	NVRAM_SAV->default_master = FALSE;
	return(0);
    } else
	goto usage;
}


/*
** Function: showreg()
**
** Desc: show the register and its meaning
*/
int 
showreg(int argc, char *argv[])
{
    if (argc != 1) {
usage:
	printf("usage: %s \n");
	return(1);
    }

    dump_ms_status();
    return(0);
}

void
dump_ms_status()
{
    unsigned short ms_status;
    unsigned int bit_value;
    int this_slot, other_slot;

    ms_status = REG_IRSP2DBUS_FPGA->MasterSlaveStatus;
    this_slot = slot_num();
    other_slot = slot_pairs[this_slot];

    printf("Master Slave Status Register: %x\n", ms_status);
    
    bit_value = ms_status & MS_STATUS_THIS_IRSP2_MASK;
    printf("Bit [1:0] = %d, means: %s\n", bit_value, this_rsp2_state[bit_value]);
    printf("This RSP2 is in SLOT %d\n", this_slot); 

    bit_value = (ms_status & MS_STATUS_OTHER_IRSP2_MASK) >> MS_STATUS_OTHER_IRSP2_SHIFT;
    printf("Bit [3:2] = %d, means: %s\n", bit_value, other_rsp2_state[bit_value]);
    printf("Other RSP2 is in SLOT %d\n", other_slot);

    bit_value = (ms_status & MS_STATUS_OTHER_IRSP2_PRESENT) >> MS_STATUS_OTHER_IRSP2_PRESENT_SHIFT;
    printf("Bit [4] = %d, means: %s\n", bit_value, RCV_present[bit_value]);

}


/*
** Function: showvars()
** 
** Desc: show the three timer value
*/
int
showvars()
{
    printf("mstimer1 = %x\n", NVRAM_SAV->ms_timer1);
    printf("mstimer2 = %x\n", NVRAM_SAV->ms_timer2);
    printf("mstimer3 = %x\n", NVRAM_SAV->ms_timer3);
    printf("ms_timer_margin = %x\n", NVRAM_SAV->ms_timer_margin);
    printf("default_master = %x\n", NVRAM_SAV->default_master);
    printf("crashed = %x\n", NVRAM_SAV->crashed);
    printf("slave_boot = %x\n", NVRAM_SAV->slave_boot);
    printf("this_current_state = %x\n", NVRAM_SAV->this_current_state);
    printf("other_current_state = %x\n", NVRAM_SAV->other_current_state);
    
    return(0);
}

/*
** Function: get_slot()
**
** Desc: return true if the slot# is even, otherwise, false.
*/
int 
get_slot()
{
    unsigned short phys_slot_num;

    phys_slot_num = (REG_IRSP2DBUS_FPGA->PhysSlot & IRSP2_PHYS_SLOT_MASK) >> IRSP2_PHYS_SLOT_SHIFT;

    if (phys_slot_num & 1) 
	return(FALSE);
    else
	return(TRUE);

} /* get_slot */


/*
** Function: slot_num()
**
** Desc: return slot number of this processor.
*/
int 
slot_num()
{
    unsigned short phys_slot_num;

    phys_slot_num = (REG_IRSP2DBUS_FPGA->PhysSlot & IRSP2_PHYS_SLOT_MASK) >> IRSP2_PHYS_SLOT_SHIFT;

    return(phys_slot_num);

} /* slot_num */

/*
** Function: unmask_ms_int()
**
** Desc: Unmask the Master/Slave State Change interrupt 
*/
void
unmask_ms_int()
{
    int this_slot, other_slot;
    
    NVRAM_SAV->this_current_state = REG_IRSP2DBUS_FPGA->MasterSlaveStatus & MS_STATUS_THIS_IRSP2_MASK;
    NVRAM_SAV->other_current_state = (REG_IRSP2DBUS_FPGA->MasterSlaveStatus & MS_STATUS_OTHER_IRSP2_MASK) >> MS_STATUS_OTHER_IRSP2_SHIFT;

    this_slot = slot_num();
    other_slot = slot_pairs[this_slot];

if (NVRAM_SAV->this_current_state < STATE_UNPLUGGED)
    printf("\nSLOT %d %s\n", this_slot, this_rsp2_msg[NVRAM_SAV->this_current_state]);
if (NVRAM_SAV->other_current_state < STATE_UNPLUGGED)
    printf("SLOT %d %s\n", other_slot, other_rsp2_msg[NVRAM_SAV->other_current_state]);

    delay_10us(1);

    REG_IRSP2DBUS_FPGA->DBusFPGAIntMask &= ~IRSP2DBUS_FPGA_INT_MS_CHANGE_MASK;

} /* unmask_ms_int */


/*
** Function: handle_ms_int()
**
** Desc: handle the Master/Slave state change interrupt
*/
void
handle_ms_int()
{
    unsigned long this_current_state;
    unsigned long other_current_state;
    unsigned long this_state_change;
    unsigned long other_state_change;
#ifdef DEBUG
    printf("handle_ms_int\n");
#endif
    this_state_change = REG_IRSP2DBUS_FPGA->MasterSlaveStatus & MS_STATUS_THIS_IRSP2_MASK;
    other_state_change = (REG_IRSP2DBUS_FPGA->MasterSlaveStatus & MS_STATUS_OTHER_IRSP2_MASK) >> MS_STATUS_OTHER_IRSP2_SHIFT;

#ifdef DEBUG
    printf("REG_IRSP2DBUS_FPGA->MasterSlaveStatus = 0x%x\n",
      REG_IRSP2DBUS_FPGA->MasterSlaveStatus);
    printf("before:NVRAM_SAV->this_current_state = 0x%x, NVRAM_SAV->other_current_state = 0x%x\n",
	   NVRAM_SAV->this_current_state, NVRAM_SAV->other_current_state);
    printf("before:NVRAM_SAV->crashed = 0x%x\n", NVRAM_SAV->crashed);
#endif

    if (NVRAM_SAV->this_current_state != this_state_change) {
#ifdef DEBUG
	printf("This RSP2 change state to %d\n", this_state_change);
#endif
	this_current_state = NVRAM_SAV->this_current_state;  /* save it */
	NVRAM_SAV->this_current_state = this_states[this_current_state][this_state_change].state;
	NVRAM_SAV->crashed = this_states[this_current_state][this_state_change].crashed;
	
	if (this_states[this_current_state][this_state_change].action != 0) {
	    REG_IRSP2DBUS_FPGA->DBusFPGAIntMask |= IRSP2DBUS_FPGA_INT_MS_TIMER_TO_MASK;
	    REG_IRSP2DBUS_FPGA->MasterSlaveTimerEnable &= ~MS_IRSP2_TIMER_ENABLE;
	    REG_IRSP2DBUS_FPGA->MasterSlaveTimer = MS_IRSP2_TIMER_MAX_COUNT;
	    (*this_states[this_current_state][this_state_change].action)();
	}
    }

    if (this_state_change != STATE_MASTER){
	if (NVRAM_SAV->other_current_state != other_state_change) {
#ifdef DEBUG
	    printf("Other RSP2 change state to %d<1>\n", other_state_change);
#endif
	    other_current_state = NVRAM_SAV->other_current_state;  /* save it */
	    NVRAM_SAV->other_current_state = other_states[other_current_state][other_state_change].state;
	    if (other_states[other_current_state][other_state_change].action != 0) {
		REG_IRSP2DBUS_FPGA->DBusFPGAIntMask |= IRSP2DBUS_FPGA_INT_MS_TIMER_TO_MASK;
		REG_IRSP2DBUS_FPGA->MasterSlaveTimerEnable &= ~MS_IRSP2_TIMER_ENABLE;
		REG_IRSP2DBUS_FPGA->MasterSlaveTimer = MS_IRSP2_TIMER_MAX_COUNT;
		(*other_states[other_current_state][other_state_change].action)();
	    }
	}
    } else {
        if (NVRAM_SAV->other_current_state != other_state_change) {
#ifdef DEBUG
            printf("Other RSP2 change state to %d<2>\n", other_state_change);
#endif
            other_current_state = NVRAM_SAV->other_current_state;  /* save it */
            NVRAM_SAV->other_current_state = other_states[other_current_state][other_state_change].state;
        }
    }
#ifdef DEBUG
    printf("after:this_state_change = 0x%x, other_state_change = 0x%x\n",
	   this_state_change, other_state_change);
    printf("after:NVRAM_SAV->this_current_state = 0x%x, NVRAM_SAV->other_current_state = 0x%x\n",
	   NVRAM_SAV->this_current_state, NVRAM_SAV->other_current_state);
    printf("after:NVRAM_SAV->crashed = 0x%x\n", NVRAM_SAV->crashed);
#endif
} /* handle_ms_int */


/*
** Function: enable_ms_timer()
**
** Desc: enable the Master/Slave Timer, and counter2 in DCL in order
**       to reset the Master/Slave Timer.
*/
void
enable_ms_timer()
{
/*    signal(SIGALRM, SIG_IGN); */
    
    REG_IRSP2DBUS_FPGA->MasterSlaveTimer = MS_IRSP2_TIMER_MAX_COUNT;

    DCL_ASIC->ct2 = 1000;   /* set to 1000 usec second time out */
    DCL_ASIC->ccr |= CCR_CT2_EN;

    REG_IRSP2DBUS_FPGA->MasterSlaveTimerEnable |= MS_IRSP2_TIMER_ENABLE;

} /* enable_ms_timer */


#define DBUS_MEM_SIZE 64
 
/* return the corresponding slave/master slot */
uint
get_slave_slot(uint master_slot)
{
    return(slot_pairs[master_slot]);
}
 
 
#define RESET_ACTION_OFFSET 2
/* To reset the slave */
void
reset_slave(uint master_slot)
{
    uint slave_slot;
    volatile uchar *slave_dbus_addr;
 
    slave_slot = get_slave_slot(master_slot);
    if (slave_slot) {
        slave_dbus_addr = (uchar *)compute_dbus_address(slave_slot);
        if (slave_dbus_addr != 0)
            *(slave_dbus_addr + RESET_ACTION_OFFSET) = 0;
    }
}


/*
** Function: resetboth()
**
** Desc: set the current master to non-participant so that
**       the two RSP2 gets into reset() together
*/
int
resetboth()
{
    if (is_this_master()) {
        if ((REG_IRSP2DBUS_FPGA->MasterSlaveStatus & MS_STATUS_OTHER_IRSP2_MASK)
            != MS_STATUS_OTHER_IRSP2_NOPWR)
            reset_slave((uint)slot_num());
/*      REG_IRSP2DBUS_FPGA->MasterSlaveCmd |= MS_CMD_IRSP2_NON_PART; */
        REG_FPGA->sw_sys_rst_reg = 0; /* wrtie to software system to cause self reset */
        return(0);
    } else {
        printf("Only master RSP2 can issue this command\n");
        return(1);
    }
}

/*
** Function: is_this_master()
**
** Desc: check this RSP2 if it is master
**
*/
int 
is_this_master()
{
    if ((REG_IRSP2DBUS_FPGA->MasterSlaveStatus & MS_STATUS_THIS_IRSP2_MASK) == MS_STATUS_THIS_IRSP2_MASTER) 
	return(TRUE);
    else
	return(FALSE);
} /* is_this_master */



/*
**
** Function: fusion()
**
** Desc: Display the key developers of the RSP2 project
*/

#define MAX_HEROS  16
char *heros[MAX_HEROS] = {"Graeme Fraser", "Bill Swift", 
			  "David Civello", "Marty Cieslak",
			  "Paul Bradley", "Kamal Avalni",
			  "Yie-Fong Dan", "Duc Hoang",
			  "Sonny Tran", "Chi Khuong",
			  "Michael Laor", "Thomas Boures",
			  "Einat Yogev", "Manson Wong",
			  "Steve J. Zhang", "Hector Briones"};

int
fusion()
{
    int ix; 

    printf("Salute to The RSP2 Heros !!!\n");
    for (ix = 0; ix < MAX_HEROS; ix++) {
	printf("%s\n", heros[ix]);
    }
}
