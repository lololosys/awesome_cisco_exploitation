/* $Id: cbus_test.c,v 3.5.4.3 1996/08/07 17:59:11 kmitchel Exp $
 * $Source: /release/112/cvs/Xsys/hes/cbus_test.c,v $
 *------------------------------------------------------------------
 * cbus_test.c -- cbus/cbus2 diagnostic routines
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cbus_test.c,v $
 * Revision 3.5.4.3  1996/08/07  17:59:11  kmitchel
 * CSCdi49854:  DBUSINTERR after RSP dbus write; ineffective recvry of
 * disabled card
 * Branch: California_branch
 *
 * Revision 3.5.4.2  1996/05/22  13:50:53  getchell
 * CSCdi57387:  corrupt pool pointer, VIP2 dies, box not healthy;VIP2
 * removed
 * Branch: California_branch
 *
 * Revision 3.5.4.1  1996/03/18  19:39:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/16  06:49:15  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.2  1996/03/07  09:38:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  00:50:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/03/07  12:30:41  dkerr
 * CSCdi50949:  Customer can crash router getting cbus lowcore dump
 * Set lowcore capture trigger by default and have results displayable
 * from "show controller cxbus memd-core".
 *
 * Revision 3.4  1996/02/08  20:10:57  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.3  1995/11/17  09:14:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:25:45  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:37:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  20:59:34  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/09/13  23:31:49  gchristy
 * CSCdi40315:  Add test subset back into -p image
 * - Remove bridging dependencies from cbus_test.c.
 *
 * Revision 2.6  1995/08/09  00:19:18  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.5  1995/08/08  16:46:22  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.4  1995/06/28 09:24:23  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/18  23:16:30  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:04:19  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:38:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "interface_private.h"
#include "../hes/cbus_registry.h"
#include "ttysrv.h"
#include "../if/ether.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/if_fip.h"
#include "../hes/cbus_test.h"
#include "../hes/if_hes_common.h"
#include "config.h"
#include "dgbus.h"
#include "dbus.h"
#include "subsys.h"
#include "../ui/common_strings.h"

#define BUFLEN 			80
#define BUF_WORDS 		4500
#define MEC_MEMA_WORDS 		(1024 * 2)
#define MEC_AF_WORDS		(1024*2)
#define AF_STEP 0x1

#define WAITDELAY 100000
#define RXOFFSET 12

#define TESTBYTES 88
#define TESTLONGS ((TESTBYTES+3)/4)

static ulong fddi_fc_test_frame[22] =
{
    0x50505050,
    0x11111111,
    0x11112222,
    0x22222222,
    0x03987655,
    0x0abcdef0,
    0x12345678,
    0x90abcdef,
    0x01234567,
    0x890abcde,
    0x22222222,
    0x22222222,
    0x22222222,
    0x22222222,
    0xaaaaaabb,
    0xbbbbbbcc,
    0xccccccdd,
    0xddddddee,
    0xeeeeeeff,
    0xffffff00,
    0x00000011,
    0x11111122
};

extern void sse_debugger(void);

ushort cbus_select(mcitype *mci, int unit);
static ushort cbus_ctrlr_select (mcitype *mci, int ctrlr);

extern boolean zebra_debug;

boolean fddi_verbose;
boolean fddi_reporting;
static boolean fddi_noint;
static mcitype fddi_mcidata;
int default_interface;
static int slot, slotunit;
static boolean fddi_station_a = TRUE;
static uchar fddi_sa[IEEEBYTES] = {0x00, 0x00, 0x0c, 0x11, 0x11, 0x11};
static uchar fddi_da[IEEEBYTES] = {0x00, 0x00, 0x0c, 0x22, 0x22, 0x22};
static ushort bct[9000];

char *fddi_escape = "^^";

#define DEFAULT_TRT  0x000186a0
#define DEFAULT_TMAX 0x001f78c0
#define DEFAULT_TVX 123
#define CLAIM_BUF_SIZE		62
#define CLAIM_DA_OFFSET		5
#define CLAIM_SA_OFFSET		11
#define BEACON_SA_OFFSET    	51

static ushort fddi_claim_beacon_data[CLAIM_BUF_SIZE / 2] 
    = {0x0011,0xffff,0xc300,0x0100,0x0200,0x0300,0x0100, 
	   0x0200, 0x03ff, 0xfe79,
	   0x6000,	/* compliment of default trt */
	   0,0,0,0,0,0,0,0,0,
	   0x0011,0xffff,0xc200,0x0000,0x0000,0x0000, 0x0100,
	   0x0200, 0x0300, 0x0000,
	   0x0000};

ulong MaxMemD;


/*
 * dci_command()
 * issue a command directly to a daughter card
 */

void dci_command (
    mcitype *mci,
    ulong *argument,
    ulong *command,
    int *rcount)
{
    char buff[BUFLEN];
    ushort sresult, response;
    boolean parsed;
    int j;
    mciregtype *regptr;
    leveltype level;

    regptr = mci->regptr;
    printf("\nrepeat count [%d]: ", *rcount);
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
        if (!null(buff))
 	    *rcount = (int) parse_unsigned(buff, &parsed);
    }
    printf("\ncommand [%d]: ", *command);
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
 	if (!null(buff))
 	    *command = parse_unsigned(buff, &parsed);
    } else {
 	printf("\n%% bad command code");
 	return;
    }
    printf("\nargument [%d]: ", *argument);
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
 	if (!null(buff))
	    *argument = parse_unsigned(buff, &parsed);
    } else {
 	printf("\n%% bad argument");
 	return;
    }
    
    sresult = 0;
    response = 0;
    level = raise_interrupt_level(NETS_DISABLE);
    (void) cbus_select(mci, default_interface);
    cbus_ctrlr_select(mci, slot);
    for (j = 0; j < *rcount; j++) {
	if (mci->flags & CBUSII)
	    regptr->largreg = *argument;
	else
	    regptr->argreg = (ushort) (*argument);
 	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
 	regptr->argreg = *command;
 	regptr->cmdreg = FCI_CTRLR_CMD;
  	if (*command == FCI_CMD_RESET) {
  	    DELAY(5000);
  	}
	response = regptr->cmdreg;
	sresult = regptr->argreg;
	if (response != MCI_RSP_OKAY) {
	    buginf("\nbad response: 0x%-04x, iteration: %d", response, j);
	}
    }
    reset_interrupt_level(level);
    printf("\nvalue returned: 0x%-04x\n", sresult, sresult);
}

/*
 * cbus_ctrlr_select 
 * assumes that interrupts are already disabled
 */
static ushort cbus_ctrlr_select (mcitype *mci, int ctrlr)
{
    ushort sresult;

    mci->regptr->argreg = ctrlr;
    mci->regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
    sresult = mci->regptr->cmdreg;
    return sresult;
}    

/*
 * cbus_select
 * select the specified interface and return the port type
 */
ushort cbus_select (mcitype *mci, int unit)
{
    ushort sresult;
    int slot2, slotunit2;

    if (unit == default_interface) {
	slot2 = slot;
    } else {
	if (!cbus_unit2slotunit(unit, &slot2, &slotunit2)) {
	    printf("\n%% invalid interface %d", unit);
	    slot2 = slot;
	}
    }
    if ((mci->hw_version == CBUS_HARDWARE) ||
	(mci->hw_version == CBUSII_HARDWARE)
	|| reg_invoke_switch_proc_present(pslot_to_dbus(mci->unit))) {
	if (cbus_ctrlr_select(mci, slot2) != MCI_RSP_OKAY)
	    return(0xffff);
    }

    mci->regptr->argreg = (ushort) unit;
    mci->regptr->cmdreg = FCI_CMD_SELECT;
    sresult = mci->regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
	buginf("\ncbus_select: failed on interface %d - response: %x",
	       unit, sresult);
	return(0xffff);
    }
    sresult = mci->regptr->argreg;
    return(sresult);
}


/*
 * cbus_reset
 * reset controller and wait for mci to get its act together.
 */

static void cbus_reset (mcitype *mci)
{
#ifdef C7000
    dgtype* dgbus;
    uchar bits;

    dgbus = pslot_to_dbus(mci->unit);
    bits = dbus_prot_rd(&dgbus->diag_control);
    dbus_slot_reset(mci->unit);
    /* make sure board is started */
    dbus_prot_wr(mci->unit, &dgbus->diag_control, bits | MASTER_ENABLE);
    DELAY(5000);
#else
    mci->regptr->argreg = FCI_HARD_RESET;
    mci->regptr->cmdreg = FCI_CMD_RESET;
    DELAY(5000);
#endif
}


/*
 * cbus_soft_reset
 * reset daughter cards and wait for them to come back
 */

static void cbus_soft_reset (mcitype* mci)
{
#ifdef C7000
    cxbus_soft_reset(mci);
#else
    mci->regptr->argreg = FCI_SOFT_RESET;
    mci->regptr->cmdreg = FCI_CMD_RESET;
    mci->regptr->argreg = slot;
    mci->regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
    mci->regptr->argreg = slot;
    mci->regptr->cmdreg = FCI_CTRLR_RESET;
    mci->regptr->argreg = slot;
    mci->regptr->cmdreg = FCI_CMD_CTRLR_LOAD;
#endif
}

/*
 * cbus_reset_daughter
 * Step on a daughtercard.
 */

static void cbus_reset_daughter (mcitype *mci, int interface)
{
    mciregtype *regptr;
    int slot2, slotunit2;

    if (interface == default_interface) {
	slot2 = slot;
    } else {
	if (!cbus_unit2slotunit(interface, &slot2, &slotunit2)) {
	    printf("\n%% invalid interface %d", interface);
	    slot2 = slot;
	}
    }

    regptr = mci->regptr;
    cbus_ctrlr_select(mci, slot2);
    regptr->cmdreg = FCI_CMD_CTRLR_RESET;
    DELAY(FCI_SOFT_RESET_TIME);
    regptr->argreg = slot2;
    regptr->cmdreg = FCI_CMD_CTRLR_LOAD;
    DELAY(FCI_SOFT_RESET_TIME);
}

/*
 * cbus_create_pool
 * create a pool of fci buffers
 */

static void cbus_create_pool (
    mcitype *mci,
    int pool,
    int overhead,
    int size,
    int count)
{
    mciregtype *regptr;
    ushort sresult;
    int i;
    
    regptr = mci->regptr;
    printf("\npool %d: %d buffers with overhead=%d, size=%d",
	   pool, count, overhead, size);
    regptr->argreg = overhead;
    regptr->cmdreg = FCI_CMD_OVERHEAD;
    regptr->argreg = size;
    regptr->cmdreg = FCI_CMD_ARGUMENT;
    for (i = 0; i < count; i++) {
	regptr->argreg = pool;
	regptr->cmdreg = FCI_CMD_TX_ALLOCATE;
	sresult = regptr->cmdreg;
	if (sresult != MCI_RSP_OKAY) {
	    if (fddi_reporting)
		printf("\nmci: tx_allocate failed - %#x", sresult);
	    return;
	}
    }
}


/*
 * cbus_rx_buffersetup
 * set up rx buffer parameters
 */

static void cbus_rx_buffersetup (
    mcitype *mci,
    int unit,
    int pool,
    int offset,
    int size,
    int limit)
{
    mciregtype *regptr;
    ushort sresult, type;
    
    printf("\nrx for unit %d: pool %d, offset %d, size %d, limit %d",
	   unit, pool, offset, size, limit);
    type = cbus_select(mci, unit);
    regptr = mci->regptr;
    
    /*
     * set pool index
     */
    printf("\nfddi_rx_buffersetup(): creating buffer pool");
    regptr->argreg = pool;
    regptr->cmdreg = FCI_CMD_ARGUMENT;
    regptr->argreg = FCI_ASSIGN_POOLINDEX;
    regptr->cmdreg = FCI_CMD_EXECUTE;
    sresult = regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
	printf("\nmci: rx_allocate (pool index) failed - %#x", sresult);
	return;
    }
    
    /* 
     * set maximum byte count for this interface.
     */
    printf("\nfddi_rx_buffersetup(): Setting byte count");
    regptr->argreg = size; 
    regptr->cmdreg = FCI_CMD_ARGUMENT;
    regptr->argreg = FCI_MAX_BUFFERSIZE;
    regptr->cmdreg = FCI_CMD_EXECUTE;
    sresult = regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
	printf("\nmci: rx_allocate (byte count) failed - %#x", sresult);
	return;
    }
    
    /*
     * set relative receive offset.
     */
    printf("\nfddi_rx_buffersetup(): Setting relative receive offset");
    regptr->argreg = offset;
    regptr->cmdreg = FCI_CMD_ARGUMENT;
    regptr->argreg = FCI_RX_OFFSET;
    regptr->cmdreg = FCI_CMD_EXECUTE;
    sresult = regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
	printf("\nmci: rx_allocate (offset) failed - %#x", sresult);
	return;
    }

    /*
     * set up receive queue limits
     */
    printf("\nfddi_rx_buffersetup(): Setting receive queue limits");
    regptr->argreg = limit;
    regptr->cmdreg = FCI_CMD_ARGUMENT;
    regptr->argreg = FCI_RX_BUFFER_LIMIT;
    regptr->cmdreg = FCI_CMD_EXECUTE;
    sresult = regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
	printf("\nmci: rx_allocate (limit) failed - %#x", sresult);
	return;
    }
    
    /*
     * Setup the default receive offset to 1.
     */
    printf("\nfddi_rx_buffersetup(): Setting receive offset to 1");
    cbus_ctrlr_select(mci, slot);
    if (FCITYPE(type) == FCI_TYPE_FDDIT)
	regptr->argreg = 1;
    else
	regptr->argreg = 1 | (1 << 4);	/* turn on rxoffset 01, and full */
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FDDI_HOLD_REG;
    regptr->cmdreg = FCI_CTRLR_CMD;
}


/*
 * cbus_tx_buffersetup
 * set up tx parameters for an interface
 */

static void cbus_tx_buffersetup (
    mcitype *mci,
    int unit,
    int total,
    int switched)
{
    mciregtype *regptr;
    
    cbus_select(mci, unit);
    regptr = mci->regptr;
    printf("\ntx for unit %d: total limit %d, switch limit %d",
	   unit, total, switched);
    
    /*
     * set up transmit queue limits
     */
    regptr->argreg = total;
    regptr->cmdreg = FCI_CMD_QUEUE;
    regptr->argreg = switched;
    regptr->cmdreg = FCI_CMD_SLIMIT;
}


/*
 * bia_stationaddress
 * read and print out station address of an interface
 */ 

static void bia_stationaddress (mcitype *mci, int unit)
{
    ushort i;
    ushort sresult;
    
    (void) cbus_select(mci, unit);
    if (fddi_verbose)
	printf(", bia: ");
    for (i = 0; i < IEEEBYTES; i++) {
	mci->regptr->argreg = i;
	mci->regptr->cmdreg = FCI_CMD_STATION;
	sresult = mci->regptr->cmdreg;
	if (sresult != MCI_RSP_OKAY)
	    printf("\nstation failed: code %04x for offset %d" , sresult, i);
	sresult = mci->regptr->argreg;
	if (fddi_verbose)
	    printf("%-02x", sresult & 0xff);
    }
}


/*
 * cbus_devicename
 * translate an mci device type code into a string
 */

static char *cbus_devicename (ushort typecode)
{
    switch (FCITYPE(typecode)) {
      case FCI_TYPE_ETHER:    return("ethernet");
      case FCI_TYPE_SERIAL:   return("serial");
      case FCI_TYPE_FDDI:     return("fddi");
      case FCI_TYPE_ULTRA:    return("ultra");
      case FCI_TYPE_HSSI:     return("hssi");
      case FCI_TYPE_CTR:      return("ctr");
      case FCI_TYPE_FDDIT:    return("fddit");
      case FCI_TYPE_ATM:      return("atm");
      default:		      return("unknown");
    }
}

/*======================================================================
 *
 * ATM specific routines
 *
 *======================================================================*/

static void atm_specific (
    mcitype *mci)
{
    mciregtype *regptr = mci->regptr;
    ulong tx_proc = 1;
    ulong rd_addr = 0xa2000000;
    ulong wr_addr = 0x9fc1fdf8;
    ulong wr_data = 0;
    long dump_len = 16;
    ulong addr, mem_val;
    ushort i, j;
    leveltype level;
    ushort response;
    char cmdchar;
    char buff[BUFLEN], *args;
    boolean parsed;
    ulong arglong;
    ulong command;
    int rcount;

    printf("\n\nATM commands");
    arglong = command = 0;
    cmdchar = '?';
    rcount = 1;
    while (TRUE) {
	printf("\natm (? for help) [%c]: ", cmdchar);
	parsed = rdtty(buff, BUFLEN);
	if (!null(buff) && parsed)
	    cmdchar = toupper(buff[0]);
	args = findarg(buff);
	if (stdio->statbits & (IDLETIMEOUT | HANGUP | CARDROP | IDLE))
	    return;

	switch (cmdchar) {
	case 'H':
	case '?':
	    printf("\n?     - Help aip cbus test.");
	    printf("\nc     - execute dci Command");
	    printf("\nq     - Quit");
	    printf("\nr     - Read memory");
	    printf("\np     - Processor select (tx:1,rx:0)");
	    printf("\nw     - Write memory");
	    break;

	case 'C':
	    dci_command(mci, &arglong, &command, &rcount);
	    break;

	case 'P':   /* to select processor which we are interested */
	    printf("\nprocessor cmd [%x]: ", tx_proc);
	    if (!(parsed = rdtty(buff, BUFLEN)))
		break;
	    if (!null(buff))
		tx_proc = parse_unsigned(buff,&parsed);
	    if (tx_proc > 1) {
		printf("invalid processor select val %d\n",tx_proc);
		break;
	    }
	    
	    level = raise_interrupt_level(NETS_DISABLE);

	    (void) cbus_select(mci, default_interface);
	    cbus_ctrlr_select(mci, slot);
	    regptr->largreg = tx_proc;
	    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	    /* select processor: Tx/Rx. 66 - CCB_AIP_CMD_PROC_SELEC */
	    regptr->argreg = (ushort) 66;  
	    regptr->cmdreg = FCI_CTRLR_CMD;

	    DELAY(100);
	    response = regptr->cmdreg;

	    reset_interrupt_level(level);
	    if (response != MCI_RSP_OKAY) {
		printf("\nbad response: %x", response);
		break;
	    }
	    else
		printf("\ncommand successful\n");
	    break;

	case 'Q':
	    return;

	case 'R':
	    printf("\nmemory read address [%x]: ",rd_addr);
	    if (!(parsed = rdtty(buff, BUFLEN)))
	      break;
	    if (!null(buff))
	      rd_addr = parse_hex_long(buff);

	    level = raise_interrupt_level(NETS_DISABLE);
	    (void) cbus_select(mci, default_interface);
	    cbus_ctrlr_select(mci, slot);
	    regptr->largreg = rd_addr;
	    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	    
	    regptr->argreg = 67;  /* CCB_AIP_CMD_ADDR_SELECT */
	    regptr->cmdreg = FCI_CTRLR_CMD;

	    DELAY(100);
	    response = regptr->cmdreg;
	    reset_interrupt_level(level);
	    if (response != MCI_RSP_OKAY) {
		printf("\nbad response: %x", response);
		break;
	    }
	    printf("\nnumber of words [%d]: ",dump_len);
	    if (!(parsed = rdtty(buff, BUFLEN)))
	        break;
	    if (!null(buff))
	        dump_len = parse_unsigned(buff,&parsed);
	    if (dump_len < 0) {
	        printf("\nbad length %d\n",dump_len);
	        break;
	    }

	    automore_enable(NULL);
	    for (i=j=0,addr=rd_addr; i < dump_len; i++,addr += 4) {
		level = raise_interrupt_level(NETS_DISABLE);
		if (!tx_proc) {
		    /* in tx processor, the read address will automatically be
		       increased by 4 after each read. In rx processor,
		       the address increment is not done to speed up the
		       response time. So here we only increase the reading 
		       address to rx processor */
		    cbus_ctrlr_select(mci, slot);
		    
		    regptr->largreg = addr;
		    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	    
		    regptr->argreg = 67;    /* CCB_AIP_CMD_ADDR_SELECT */
		    regptr->cmdreg = FCI_CTRLR_CMD;
		    DELAY(10);
		}
		cbus_ctrlr_select(mci, slot);
		
		regptr->largreg = addr;
		regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	    
		regptr->argreg = 69; /* CCB_AIP_CMD_MEM_READ */
		regptr->cmdreg = FCI_CTRLR_CMD;

		if (!tx_proc) {
		    /* to read stuff out from rx processor, the AIP receive
		       processor will not return the result to the RP
		       directly. Instead, it puts the result to the
		       dual port memory in the AIP board. Therefore we need
		       an additional command to inform the AIP tx processor
		       to read it out from the dual port memory and return
		       the result. 75 is the command arg to do that.
		     */
		    DELAY(10);
		    cbus_ctrlr_select(mci, slot);
		    
		    regptr->largreg = addr;
		    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	    
		    regptr->argreg = 75;  /* retrieve val from dual_mem */
		    regptr->cmdreg = FCI_CTRLR_CMD;
		}

		DELAY(100);
		response = regptr->cmdreg;
		mem_val = regptr->largreg;
		reset_interrupt_level(level);

		if (response != MCI_RSP_OKAY) {
		    printf("\nbad response: %x", response);
		    automore_disable();
		    break;
		}

		if (j == 0)
		    printf("%08x:", addr);
		printf(" %08x",mem_val);
		if (++j == 4) {
		    printf("\n");
		    j = 0;
		}
		
	    }
	    printf("\n");
	    automore_disable();
	    break;

	case 'W':
	    printf("\nmemory write address [%x]: ", wr_addr);
	    if ((parsed = rdtty(buff, BUFLEN))) {
		if (!null(buff))
		    wr_addr = parse_hex_long(buff);
	    }
	    else
		break;

	    level = raise_interrupt_level(NETS_DISABLE);
	    (void) cbus_select(mci, default_interface);
	    cbus_ctrlr_select(mci, slot);
	    
	    regptr->largreg = wr_addr;
	    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	    
	    regptr->argreg = 67;  /* CCB_AIP_CMD_ADDR_SELECT */
	    regptr->cmdreg = FCI_CTRLR_CMD;

	    DELAY(100);
	    response = regptr->cmdreg;
	    reset_interrupt_level(level);
	    if (response != MCI_RSP_OKAY) {
		printf("\nbad response: %x", response);
		break;
	    }

	    printf("\nmemory write data [0x%x]: ", wr_data);
	    if ((parsed = rdtty(buff, BUFLEN))) {
		if (!null(buff))
		    wr_data = parse_hex_long(buff);
	    } else
		break;

	    level = raise_interrupt_level(NETS_DISABLE);
	    cbus_ctrlr_select(mci, slot);
	    regptr->largreg = wr_data;
	    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	    
	    regptr->argreg = 74; /* AIP_CMD_DEBUG_WR */
	    regptr->cmdreg = FCI_CTRLR_CMD;

	    DELAY(100);
	    response = regptr->cmdreg;
	    reset_interrupt_level(level);
	    if (response != MCI_RSP_OKAY) {
		printf("\nbad response: %x", response);
		break;
	    }
	    printf("command success\n");
	    break;

	  default:
	    printf("\nunknown command \"%c\"", cmdchar);
	    cmdchar = '?';
	    break;
	}
    }
}


/*======================================================================
 *
 * Ethernet specific routines
 *
 *======================================================================*/

static uchar ether_test[IEEEBYTES] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};

static void ether_odd_byte_start (
    mcitype *mci)
{
    char c;
    int bufcnt;
    ushort temp, bytes;
    ulong ltemp;
    mciregtype *regptr;
    leveltype level;

    for (bufcnt = 0; bufcnt < 16; bufcnt++) {
	printf("\nPress any key to send %s/%s start, %s/%s end frame.",
	       (bufcnt & 8) ? "odd" : "even", (bufcnt & 4) ? "odd" : "even",
	       (bufcnt & 2) ? "odd" : "even", (bufcnt & 1) ? "odd" : "even");
	c = ttyin(stdio);
	c &= 0177;
	if ((c == 'Q') || (c == 'q'))
	    return;

	/*
	 * Select the interface each time.
	 */
	level = raise_interrupt_level(NETS_DISABLE);
	regptr = mci->regptr;
	regptr->argreg = default_interface;
	regptr->cmdreg = FCI_CMD_SELECT;
	if ((temp = regptr->cmdreg) != FCI_RSP_OKAY) {
	    reset_interrupt_level(level);
	    printf("\nBad response at that port, %04x", temp);
	    return;
	}
	if ((ltemp = regptr->argreg) == 0xffff) {
	    reset_interrupt_level(level);
	    printf("\nBad result code at that port, %04x", ltemp);
	    return;
	}

	/*
	 * Build and send the frame.
	 */
	regptr->argreg = 0;
	regptr->cmdreg = FCI_CMD_TX0_RESERVE;
	if ((temp = regptr->cmdreg) != FCI_RSP_OKAY) {
	    reset_interrupt_level(level);
	    printf("\nBad response (%04x) to reserve", temp);
	    return;
	}
	regptr->argreg = (bufcnt >> 3);
	regptr->cmdreg = FCI_CMD_TX0_SELECT;
	bytes = 0;
	if ((bufcnt >> 2) & 1) {
	    /*
	     * odd byte start
	     */
	    regptr->write0short = ether_test[0];
	    regptr->write0short = (ether_test[1] << 8) | ether_test[2];
	    regptr->write0short = (ether_test[3] << 8) | ether_test[4];
	    regptr->write0short = (ether_test[5] << 8) | ether_test[0];
	    regptr->write0short = (ether_test[1] << 8) | ether_test[2];
	    regptr->write0short = (ether_test[3] << 8) | ether_test[4];
	    regptr->write0short = (ether_test[5] << 8) | 0x04;
	    regptr->write0short = 0x0403;
	    regptr->write0short = 0x0F10;
	    regptr->write0short = 0x1112;
	    switch (bufcnt & 0x3) {
	      case 0:
		regptr->write0short = 0x1300;
		bytes = 0x14;
		break;
	      case 1:
		regptr->write0short = 0x1314;
		bytes = 0x15;
		break;
	      case 2:
		regptr->write0short = 0x1314;
		regptr->write0short = 0x1500;
		bytes = 0x16;
		break;
	      case 3:
		regptr->write0short = 0x1314;
		regptr->write0short = 0x1516;
		bytes = 0x17;
		break;
	    }
	} else {
	    /*
	     * even.  First word gets AC/FC
	     */
	    regptr->write0short = (ether_test[0] << 8) | ether_test[1];
	    regptr->write0short = (ether_test[2] << 8) | ether_test[3];
	    regptr->write0short = (ether_test[4] << 8) | ether_test[5];
	    regptr->write0short = (ether_test[0] << 8) | ether_test[1];
	    regptr->write0short = (ether_test[2] << 8) | ether_test[3];
	    regptr->write0short = (ether_test[4] << 8) | ether_test[5];
	    regptr->write0short = 0x0404;
	    regptr->write0short = 0x030F;
	    regptr->write0short = 0x1011;
	    regptr->write0short = 0x1213;
	    switch (bufcnt & 0x3) {
	      case 0:
		bytes = 0x14;
		break;
	      case 1:
		regptr->write0short = 0x1400;
		bytes = 0x15;
		break;
	      case 2:
		regptr->write0short = 0x1415;
		bytes = 0x16;
		break;
	      case 3:
		regptr->write0short = 0x1415;
		regptr->write0short = 0x1600;
		bytes = 0x17;
		break;
	    }
	}
	regptr->argreg = bytes | (((bufcnt >> 2) & 1) ? MCI_TX_ODDALIGN : 0);
	regptr->cmdreg = FCI_CMD_TX0_START;
	reset_interrupt_level(level);
    }
}

static void ether_specific (
    mcitype *mci)
{
    char cmdchar;
    char buff[BUFLEN], *args;
    boolean parsed;
    ulong arglong;
    ulong command;
    int rcount;

    printf("\n\nEthernet commands");
    arglong = command = 0;
    cmdchar = '?';
    rcount = 1;
    while (TRUE) {
	printf("\nethernet (? for help) [%c]: ", cmdchar);
	parsed = rdtty(buff, BUFLEN);
	if (!null(buff) && parsed)
	    cmdchar = toupper(buff[0]);
	args = findarg(buff);
	if (stdio->statbits & (IDLETIMEOUT | HANGUP | CARDROP | IDLE))
	    return;
	automore_enable(NULL);
	switch (cmdchar) {
	  case '?':
	    printf("\n");
	    printf("\nC                        - execute dci command");
	    printf("\nE                        - Test Odd Byte Start");
	    printf("\nQ                        - quit");
	    break;

	  case 'C':
	    dci_command(mci, &arglong, &command, &rcount);
	    break;

	  case 'E':
	    ether_odd_byte_start(mci);
	    break;

	  case 'Q':
	    return;
	  default:
	    printf("\nunknown command \"%c\"", cmdchar);
	    cmdchar = '?';
	    break;
	}
	automore_disable();
    }
}

/*======================================================================
 *
 *			FDDI specific routines
 *
 *======================================================================*/

static void af_rw_cmd (mcitype *mci)
{
    char buff[BUFLEN], *p;
    uchar values[16];
    int i, num_valid;
    ushort addr, value, repeat, max_addr, count;
    boolean parsed, writing;
    mciregtype *regptr;
    leveltype level;

    regptr = mci->regptr;
    writing = FALSE;
    printf("\nr/w? [r] ");
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
        if (!null(buff)) {
	    p = deblank(buff);
	    writing = (toupper(*p) == 'W') ? TRUE : FALSE;
	}
    }
    if (!parsed)
	return;
    addr = 0;
    printf("\naddress (absolute): ");
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
 	if (!null(buff))
	    addr = parse_unsigned(buff, &parsed);
	else
	    return;
    }
    if (!parsed)
	return;
    value = 0xff;
    if (writing) {
	printf("\nvalue [0xff]: ");
	parsed = rdtty(buff, BUFLEN);
	if (parsed) {
	    if (!null(buff))
		value = parse_unsigned(buff, &parsed);
	}
    }
    if (!parsed)
	return;
    printf("\nrepeat [1]: ");
    repeat = 1;
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
	if (!null(buff))
	    repeat = parse_unsigned(buff, &parsed);
    } else {
	return;
    }
    level = raise_interrupt_level(NETS_DISABLE);
    (void) cbus_select(mci, default_interface);
    regptr->cmdreg = FCI_CMD_AF_SIZE;
    max_addr = regptr->argreg;
    if (addr >= max_addr) {
	reset_interrupt_level(level);
	printf("\naddr %04x too big.  max is %04x",
	       addr, max_addr - 1);
	return;
    }
    if (addr + repeat > max_addr)
	repeat = max_addr - addr;
    regptr->argreg = (ushort) (addr - 0x100);	/* adjust for uCode
						   modification */
    regptr->cmdreg = FCI_CMD_AF_SELECT;
    num_valid = 0;
    for (count = 0; count < repeat; count++) {
	if (!writing) {
	    if ((count % 16) == 0 && count != 0) {
		reset_interrupt_level(level);
		printf("\naf[%04x]: ", addr);
		for (i = 0; i < num_valid; i++)
		    printf("%02x ", values[i]);
		addr += num_valid;
		num_valid = 0;
		level = raise_interrupt_level(NETS_DISABLE);
		(void) cbus_select(mci, default_interface);
		regptr->argreg = (ushort) (addr - 0x100);
		regptr->cmdreg = FCI_CMD_AF_SELECT;
	    }
	    regptr->cmdreg = FCI_CMD_AF_READ;
	    value = regptr->argreg;
	    values[count % 16] = value >> 8;
	    num_valid++;
	} else {
	    regptr->argreg = (value << 8);
	    regptr->cmdreg = FCI_CMD_AF_WRITE;
	}
    }
    reset_interrupt_level(level);
    if (!writing) {
	if (num_valid) {
	    printf("\naf[%04x]: ", addr);
	    for (i = 0; i < num_valid; i++)
		printf("%02x ", values[i]);
	}
	printf("\n");
    }
}


static void fddi_transmit_sabme (mcitype *mci)
{
    char buff[BUFLEN];
    uchar pakit[80];
    ushort *ps, temp;
    ulong ltemp;
    int num_buffs, bytes, cnt;
    mciregtype *regptr;
    leveltype level;
    boolean parsed;

    regptr = mci->regptr;
    ps = (ushort *) &pakit[1];
    pakit[0] = 0x50;
    ps[0] = 0x0800;
    ps[1] = 0x5a87;
    ps[2] = 0x8d38;
    ps[3] = 0x0200;
    ps[4] = 0x0804;
    ps[5] = 0x0880;
    ps[6] = 0xf0f0;
    ps[7] = 0x7f01;
    ps[8] = 0x0203;
    ps[9] = 0x0405;
    ps[10] = 0x0607;
    ps[11] = 0x0809;
    ps[12] = 0x0a0b;
    ps[13] = 0x0c0d;
    ps[14] = 0x0e0f;
    printf("\n  number of buffers: ");
    if (rdtty(buff, BUFLEN)) {
	if (null(buff))
	    return;
	num_buffs = parse_unsigned(buff, &parsed);
	if (!parsed || num_buffs <= 0)
	    return;
    } else
	return;
    printf("\n  bytes: ");
    if (rdtty(buff, BUFLEN)) {
	if (null(buff))
	    return;
	bytes = parse_unsigned(buff, &parsed);
	if (!parsed || bytes < 13) {
	    printf("\nToo small.");
	    return;
	}
    } else
	return;
    level = raise_interrupt_level(NETS_DISABLE);
    regptr->argreg = default_interface;
    regptr->cmdreg = FCI_CMD_SELECT;
    temp = regptr->cmdreg;
    ltemp = regptr->argreg;
    if (temp != FCI_RSP_OKAY || ltemp == 0xffff) {
	reset_interrupt_level(level);
	printf("\nNobody is home at that port, %04x %04x", temp, ltemp);
	return;
    }
    for (cnt = 0; cnt < num_buffs; cnt++) {
	regptr->argreg = 0;
	regptr->cmdreg = FCI_CMD_TX0_RESERVE;
	if (regptr->cmdreg != FCI_RSP_OKAY)
	    break;
	regptr->argreg = 0;
	regptr->cmdreg = FCI_CMD_TX0_SELECT;
	buffer2mci(&pakit[0], &regptr->write0long, bytes);
	regptr->argreg = default_interface;
	regptr->cmdreg = FCI_CMD_SELECT;
	regptr->argreg = bytes;
	regptr->cmdreg = FCI_CMD_TX0_START;
    }
    reset_interrupt_level(level);
    printf("\n  transmitted %d buffers", cnt);
}

/*
 * To acheive the proper results it is assumed that no
 * intervening transmit occurs.  This is only true if no autonomous
 * switching is occuring.  Nothing happens from this processor because
 * we turn interrupts off while we are dorking.
 */

static void fddi_blast (mcitype *mci)
{
    char buff[BUFLEN];
    int i, sap;
    uchar da[6], sa[6], *p;
    int num_buffs, pool, bufcnt;
    ushort cbus_port, temp, bytes;
    ulong ltemp;
    mciregtype *regptr;
    leveltype level;
    boolean parsed = FALSE;

    ltemp = temp = 0;
    regptr = mci->regptr;
    printf("\n  da: ");
    if (rdtty(buff, BUFLEN)) {
        if (null(buff))
	    return;
	if (!parse_ether_address(buff, da)) {
	    printf("\nmalformed hex mac address");
	    return;
	}
    } else
	return;
    printf("\n  sa: ");
    if (rdtty(buff, BUFLEN)) {
        if (null(buff))
	    return;
	if (!parse_ether_address(buff, sa)) {
	    printf("\nmalformed hex mac address");
	    return;
	}
    } else
	return;
    printf("\n  number of buffers: ");
    if (rdtty(buff, BUFLEN)) {
	if (null(buff))
	    return;
	num_buffs = parse_unsigned(buff, &parsed);
	parsed = parsed && (num_buffs > 0);
	if (!parsed)
	    return;
    } else
	return;
    printf("\n  pool: ");
    if (rdtty(buff, BUFLEN)) {
	if (null(buff))
	    return;
	pool = parse_unsigned(buff, &parsed);
	parsed = parsed && (pool <= 7);
	if (!parsed)
	    return;
    } else
	return;
    printf("\n  cbus port: ");
    if (rdtty(buff, BUFLEN)) {
	if (null(buff))
	    return;
	cbus_port = parse_unsigned(buff, &parsed);
	if (!parsed)
	    return;
    } else
	return;
    printf("\n  bytes: ");
    if (rdtty(buff, BUFLEN)) {
	if (null(buff))
	    return;
	bytes = parse_unsigned(buff, &parsed);
	parsed = parsed && (bytes >= 13);
	if (!parsed) {
	    printf("\nToo small.");
	    return;
	}
    } else
	return;
    printf("\n  sap: ");
    if (rdtty(buff, BUFLEN))
	sap = ((null(buff) ? 0 : parse_unsigned(buff, &parsed)) & 0xff);
    else
	return;
    if (!parsed)
	return;
    level = raise_interrupt_level(NETS_DISABLE);
    regptr->argreg = cbus_port;
    regptr->cmdreg = FCI_CMD_SELECT;
    if ((temp = regptr->cmdreg) != FCI_RSP_OKAY ||
	(ltemp = regptr->argreg) == 0xffff) {
	reset_interrupt_level(level);
	printf("\nNobody is home at that port, %04x %04x", temp, ltemp);
	return;
    }
    for (bufcnt = 0; bufcnt < num_buffs; bufcnt++) {
	regptr->argreg = pool;
	regptr->cmdreg = FCI_CMD_TX0_RESERVE;
	if (regptr->cmdreg != FCI_RSP_OKAY)
	    break;
	regptr->argreg = (bufcnt >> 1);
	regptr->cmdreg = FCI_CMD_TX0_SELECT;
	if (bufcnt & 1) {
	    /*
	     * odd.  do the odd fc dance
	     */
	    regptr->write0short = 0x50;
	    p = da;
	    for (i = 0; i < 3; i++) {
		temp = (*p++) << 8;
		temp |= (*p++);
		regptr->write0short = temp;
	    }
	    p = sa;
	    for (i = 0; i < 3; i++) {
		temp = (*p++) << 8;
		temp |= (*p++);
		if (i == 2 && sap && (temp & 0xff) == 0)
		    temp |= (bufcnt & 0xff);
		regptr->write0short = temp;
	    }
	    if (sap) {
		regptr->write0short = (sap << 8) | sap;
		regptr->write0short = (0x7f << 8) | 0;
		ltemp = 0x10;
	    } else {
		regptr->write0short = (bufcnt << 8) | 0x0e;
		ltemp = 0xf;
	    }
	} else {
	    /*
	     * even.  First word gets FC/DA
	     */
	    p = da;
	    regptr->write0short = (0x50 << 8) | (*p++);
	    for (i = 0; i < 2; i++) {
		temp = (*p++) << 8;
		temp |= (*p++);
		regptr->write0short = temp;
	    }
	    temp = (*p++) << 8;
	    p = sa;
	    temp |= (*p++);
	    regptr->write0short = temp;
	    for (i = 0; i < 2; i++) {
		temp = (*p++) << 8;
		temp |= (*p++);
		regptr->write0short = temp;
	    }
	    if (sap) {
		if (!*p)
		    temp = (bufcnt << 8) | sap;
		else
		    temp = ((*p++) << 8) | sap;
		regptr->write0short = temp;
		regptr->write0short = (sap << 8) | 0x7f;
		ltemp = 0x10;
	    } else {
		temp = ((*p++) << 8) | bufcnt;
		regptr->write0short = temp;
		ltemp = 0xe;
	    }
	}
	while (ltemp < bytes - 1) {
	    temp = ((ushort) ltemp << 8) | ((ushort) ltemp + 1);
	    regptr->write0short = temp;
	    ltemp += 2;
	}
	if (ltemp < bytes) {
	    temp = ((ushort) ltemp << 8);
	    regptr->write0short = temp;
	}
	regptr->argreg = bytes | ((bufcnt & 1) ? 0x4000 : 0);
	regptr->cmdreg = FCI_CMD_TX0_ENQUEUE;
    }
    reset_interrupt_level(level);
    printf("\n  Found %d buffers", bufcnt);
}

/*
 * fddi_set_address
 */

static void fddi_set_address (void)
{
    boolean ok;
    char buff[BUFLEN], cmdchar;
    
    
    /* 
     * Prompt for the Source Address.
     */
    fddi_station_a = TRUE;
    printf("\nStation [A or B] default A: ");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff)) {
	cmdchar = toupper(buff[0]);
	if (cmdchar == 'B')
	    fddi_station_a = FALSE;
    }    
}


static void fddi_select_controller (
    mcitype *mci,
    int  controller)
{
    ushort sresult;
    leveltype level;

    cbus_ctrlr_select(mci, controller);
    if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
	level = set_interrupt_level(ALL_ENABLE);
	printf("\nError %-04x, ctrlr select", sresult);
	reset_interrupt_level(level);
    }
}


/*
 * fddi_memdtest
 * Test MemD from the fddi side.
 */

static void fddi_memdtest (mcitype *mci)
{
    boolean parsed, reporting;
    int rcount, j, k, registers, controller;
    int start, result, pass, loop, tests;
    ulong size, i;
    char buff[BUFLEN];
    ushort sresult;
    leveltype level;

    rcount = 1;
    controller = 0;
    registers = 57;
    pass = 0;
    loop = 1;
    tests = 0x1f;

    printf("\nVerbose mode [y]: ");                /* reporting mode */
    reporting = ((rdtty(buff, BUFLEN) &&
		  (toupper(buff[0]) == 'N')) ? FALSE : TRUE);
    if (reporting)
	printf("\nRunning in Verbose Mode");
    
    printf("\nSelect controller [%d]: ", controller);  /* controller */
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
 	if (!null(buff))
	    controller = parse_unsigned(buff, &parsed);
    }
    printf("\nRepeat count [%d]: ", rcount);         /* repeat count */
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
	if (!null(buff))
	    rcount = parse_unsigned(buff, &parsed);
    }
    printf("\nLoop count [%d]: ", loop);         /* repeat count */
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
	if (!null(buff))
	    loop = parse_unsigned(buff, &parsed);
    }
    printf("\nWhich tests [%d]: ", tests);         /* repeat count */
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
	if (!null(buff))
	    tests = parse_unsigned(buff, &parsed);
    }

    level = raise_interrupt_level(NETS_DISABLE);
    mci->regptr->argreg = 1;
    mci->regptr->cmdreg = FCI_CMD_MEMSIZE;
    if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
	reset_interrupt_level(level);
	printf("\nError %-04x, cmd_memsize", sresult);
	level = raise_interrupt_level(NETS_DISABLE);
	fddi_select_controller(mci, controller);
    }
    if (mci->flags & CBUS)
	size = (ulong) (mci->regptr->argreg);
    else
	size = mci->regptr->largreg;
    reset_interrupt_level(level);
    printf("\nMemory Size: %-04x",size);
    level = raise_interrupt_level(NETS_DISABLE);
    fddi_select_controller(mci, controller);
    start = 0x100;
    
    /*
     * Set Up controller
     */
    fddi_select_controller(mci, controller);
    mci->regptr->argreg = controller;
    mci->regptr->cmdreg = FCI_CMD_CTRLR_LOAD;
    if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
	reset_interrupt_level(level);
	printf("\nError %-04x, ctrlr load", sresult);
	level = raise_interrupt_level(NETS_DISABLE);
	fddi_select_controller(mci, controller);
    }
    
    /*
     * Check to see if controller running
     */
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    mci->regptr->argreg = FCI_CTRLR_VERSION;
    mci->regptr->cmdreg = FCI_CTRLR_CMD;
    if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
	reset_interrupt_level(level);
	printf("\nError %-04x, controller version #", sresult);
	level = raise_interrupt_level(NETS_DISABLE);
	fddi_select_controller(mci, controller);
    }
    sresult = mci->regptr->argreg;
    reset_interrupt_level(level);
    printf("\nMicroCode Version # %d.%d",((sresult>>8)&0xff),(sresult&0xff));
    
    for (j=0;j<rcount;j++) {
	for (k=1;k<=loop;k++) {
	    if ((tests & 0x01) == 0)
		break;
	    if (reporting) {
		printf("\nWriting from cBus memd with all zeros...");
		if (mci->flags & CBUSII)
		    printf("\nReading from daughter page %d. Expecting 0x0000",
			   (k&0xf));
		else
		    printf("\nReading from daughter. Expecting 0x0000");
	    }
	    level = raise_interrupt_level(NETS_DISABLE);
	    fddi_select_controller(mci, controller);
	    if (mci->flags & CBUS)
		mci->regptr->argreg = start;
	    else
		mci->regptr->largreg = start;
	    mci->regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	    if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		reset_interrupt_level(level);
		printf("\nError %-04x, memd select", sresult);
		level = raise_interrupt_level(NETS_DISABLE);
		fddi_select_controller(mci, controller);
	    }
	    for (i=start; i<size; i++)               /* Write All zeros */
		mci->regptr->write0short = 0;
	    if (mci->flags & CBUS)
		mci->regptr->argreg = start;
	    else
		mci->regptr->largreg = start;
	    mci->regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	    if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		reset_interrupt_level(level);
		printf("\nError %-04x, memd select", sresult);
		level = raise_interrupt_level(NETS_DISABLE);
		fddi_select_controller(mci, controller);
	    }
	    if (mci->flags & CBUSII) {
		mci->regptr->argreg = (k & 0xf);         /* Set page number */
		mci->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
		mci->regptr->argreg = FCI_CTRLR_PAGE_SELECT;
		mci->regptr->cmdreg = FCI_CTRLR_CMD;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x, page select", sresult);
		    goto quit;
		}
	    }
	    for (i=start; i<size; i++) {              /* Read via FDDI */
		if (mci->flags & CBUS)
		    mci->regptr->argreg = i;
		else
		    mci->regptr->largreg = i;
		mci->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x ctrlr address %-04x", sresult, i);
		    goto quit;
		}
		mci->regptr->argreg = FCI_CTRLR_MEMD_SELECT;
		mci->regptr->cmdreg = FCI_CTRLR_CMD;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x ctrlr memd select %-04x",
			   sresult, i);
		    goto quit;
		}
		mci->regptr->argreg = FCI_CTRLR_MEMD_READ;
		mci->regptr->cmdreg = FCI_CTRLR_CMD;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x ctrlr memd read %-04x", sresult, i);
		    goto quit;
		}
		result = mci->regptr->argreg;
		if (result != 0x0) {
		    reset_interrupt_level(level);
		    if (mci->flags & CBUS)
			printf("\nError, location 0x%-08x read 0x%-04x expecting 0x0000", i, result);
		    else
			printf("\nError, location 0x%-08x page 0x%-04x read 0x%-04x expecting 0x0000",
			       i, (k & 0xF), (result & 0xFFFF));
		    if (psipending())
			return;
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
	    }
	    reset_interrupt_level(level);
	}
	for (k=1;k<=loop;k++) {
	    if ((tests & 0x02) == 0)
		break;
	    if (reporting) {
		printf("\nWriting memd from cBus with all ones...");
		if (mci->flags & CBUSII)
		    printf("\nReading from daughter page %d. Expecting 0xFFFF",
			   (k&0xf));
		else
		    printf("\nReading from daughter. Expecting 0xFFFF");
	    }
	    level = raise_interrupt_level(NETS_DISABLE);
	    fddi_select_controller(mci, controller);
	    if (mci->flags & CBUS)
		mci->regptr->argreg = start;
	    else
		mci->regptr->largreg = start;
	    mci->regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	    if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		reset_interrupt_level(level);
		printf("\nError %-04x, memd select", sresult);
		level = raise_interrupt_level(NETS_DISABLE);
		fddi_select_controller(mci, controller);
	    }
	    for (i=start; i<size; i++)               /* Write All ones */
		mci->regptr->write0short = 0xFFFF;
	    if (mci->flags & CBUS)
		mci->regptr->argreg = start;
	    else
		mci->regptr->largreg = start;
	    mci->regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	    if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		reset_interrupt_level(level);
		printf("\nError %-04x, memd select", sresult);
		level = raise_interrupt_level(NETS_DISABLE);
		fddi_select_controller(mci, controller);
	    }
	    if (mci->flags & CBUSII) {
		mci->regptr->argreg = (k & 0xf);         /* Set page number */
		mci->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
		mci->regptr->argreg = FCI_CTRLR_PAGE_SELECT;
		mci->regptr->cmdreg = FCI_CTRLR_CMD;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x, page select", sresult);
		    goto quit;
		}
	    }
	    for (i=start; i<size; i++) {              /* Read via FDDI */
		if (mci->flags & CBUS)
		    mci->regptr->argreg = i;
		else
		    mci->regptr->largreg = i;
		mci->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x ctrlr address %-04x", sresult, i);
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
		mci->regptr->argreg = FCI_CTRLR_MEMD_SELECT;
		mci->regptr->cmdreg = FCI_CTRLR_CMD;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x ctrlr memd select %-04x",
			   sresult, i);
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
		mci->regptr->argreg = FCI_CTRLR_MEMD_READ;
		mci->regptr->cmdreg = FCI_CTRLR_CMD;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x ctrlr memd read %-04x", sresult, i);
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
		result = mci->regptr->argreg;
		if (result != 0xFFFF) {
		    reset_interrupt_level(level);
		    if (mci->flags & CBUS)
			printf("\nError, location 0x%-08x read 0x%x expecting 0xFFFF", i, result);
		    else
			printf("\nError, location 0x%-08x page 0x%-04x read 0x%-04x expecting 0xFFFF",
			       i, (k & 0xF), (result & 0xFFFF));
		    if (psipending())
			return;
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
	    }
	    reset_interrupt_level(level);
	}
	for (k=1;k<=loop;k++) {
	    if ((tests & 0x04) == 0)
		break;
	    if (reporting) {
		printf("\nWriting memd from cBus with address+1...");
		if (mci->flags & CBUSII)
		    printf("\nReading from daughter page %d. Expecting address+1...", (k&0xf));
		else
		    printf("\nReading from daughter. Expecting address+1...");
	    }
	    level = raise_interrupt_level(NETS_DISABLE);
	    fddi_select_controller(mci, controller);
	    if (mci->flags & CBUS)
		mci->regptr->argreg = start;
	    else
		mci->regptr->largreg = start;
	    mci->regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	    if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		reset_interrupt_level(level);
		printf("\nError %-04x, memd select", sresult);
		level = raise_interrupt_level(NETS_DISABLE);
		fddi_select_controller(mci, controller);
	    }
	    for (i=start; i<size; i++)               /* Write address+1 */
		mci->regptr->write0short = i+1;
	    if (mci->flags & CBUS)
		mci->regptr->argreg = start;
	    else
		mci->regptr->largreg = start;
	    mci->regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	    if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		reset_interrupt_level(level);
		printf("\nError %-04x, memd select", sresult);
		level = raise_interrupt_level(NETS_DISABLE);
		fddi_select_controller(mci, controller);
	    }
	    if (mci->flags & CBUSII) {
		mci->regptr->argreg = (k & 0xf);         /* Set page number */
		mci->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
		mci->regptr->argreg = FCI_CTRLR_PAGE_SELECT;
		mci->regptr->cmdreg = FCI_CTRLR_CMD;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x, page select", sresult);
		    goto quit;
		}
	    }
	    for (i=start; i<size; i++) {              /* Read via FDDI */
		if (mci->flags & CBUS)
		    mci->regptr->argreg = i;
		else
		    mci->regptr->largreg = i;
		mci->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x ctrlr address %-04x", sresult, i);
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
		mci->regptr->argreg = FCI_CTRLR_MEMD_SELECT;
		mci->regptr->cmdreg = FCI_CTRLR_CMD;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x ctrlr memd select %-04x", sresult, i);
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
		mci->regptr->argreg = FCI_CTRLR_MEMD_READ;
		mci->regptr->cmdreg = FCI_CTRLR_CMD;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x ctrlr memd read %-04x", sresult, i);
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
		result = mci->regptr->argreg;
		if (result != (0xffff & (i+1))) {
		    mci->regptr->argreg = 0;
		    mci->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
		    mci->regptr->argreg = FCI_CTRLR_VERSION;
		    mci->regptr->cmdreg = FCI_CTRLR_CMD;
		    reset_interrupt_level(level);
		    if (mci->flags & CBUS)
			printf("\nError, location 0x%-08x read 0x%x expecting 0x%04x", i,result,i+1);
		    else
			printf("\nError, location 0x%-08x page 0x%-04x read 0x%-04x expecting 0x%-04x",
			       i, (k & 0xF), (result & 0xFFFF), ((i+1) & 0xFFFF));
		    if (psipending())
			return;
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
	    }
	    reset_interrupt_level(level);
	}

	for (k=1;k<=loop;k++) {
	    if ((tests & 0x08) == 0)
		break;
	    if (reporting) {
		if (mci->flags & CBUSII)
		    printf("\nWriting memd from daughter page %d with 0xFFFF",
			   (k&0xf));
		else
		    printf("\nWriting memd from daughter with 0xFFFF");
		printf("\nReading from cBus. Expecting all ones");
	    }
	    level = raise_interrupt_level(NETS_DISABLE);
	    fddi_select_controller(mci, controller);
	    if (mci->flags & CBUSII) {
		mci->regptr->argreg = (k & 0xf);         /* Set page number */
		mci->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
		mci->regptr->argreg = FCI_CTRLR_PAGE_SELECT;
		mci->regptr->cmdreg = FCI_CTRLR_CMD;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x, page select", sresult);
		    goto quit;
		}
	    }
	    for (i=start; i<size;i++) {           /* Write ones via FDDI */
		if (mci->flags & CBUS)
		    mci->regptr->argreg = i;
		else
		    mci->regptr->largreg = i;
		mci->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x ctrlr address %-04x", sresult, i);
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
		mci->regptr->argreg = FCI_CTRLR_MEMD_SELECT;
		mci->regptr->cmdreg = FCI_CTRLR_CMD;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x ctrlr memd select %-04x",
			   sresult, i);
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
		mci->regptr->argreg = 0xFFFF;
		mci->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x ctrlr data %-04x", sresult, i);
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
		mci->regptr->argreg = FCI_CTRLR_MEMD_WRITE;
		mci->regptr->cmdreg = FCI_CTRLR_CMD;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x ctrlr memd write %-04x", sresult, i);
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
	    }
	    if (mci->flags & CBUS)
		mci->regptr->argreg = start;
	    else
		mci->regptr->largreg = start;
	    mci->regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	    if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		reset_interrupt_level(level);
		printf("\nError %-04x, memd select", sresult);
		level = raise_interrupt_level(NETS_DISABLE);
		fddi_select_controller(mci, controller);
	    }
	    for (i=start; i<size; i++) {
		result = mci->regptr->readshort;
		if (result != 0xffff) {
		    reset_interrupt_level(level);
		    if (mci->flags & CBUS)
			printf("\nError, location 0x%-08x read 0x%-04x expecting 0xFFFF",
			       i, (result & 0xFFFF));
		    else
			printf("\nError, location 0x%-08x page 0x%-04x read 0x%-04x expecting 0xFFFF",
			       i, (k & 0xF), (result & 0xFFFF));
		    if (psipending())
			return;
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
	    }
	    reset_interrupt_level(level);
	}
	
	for (k=1;k<=loop;k++) {
	    if ((tests & 0x10) == 0)
		break;
	    if (reporting) {
		if (mci->flags & CBUSII)
		    printf("\nWriting memd from daughter page %d with address+1...", (k&0xf));
		else
		    printf("\nWriting memd from daughter with address+1...");
		printf("\nReading from cBus. Expecting address+1...");
	    }
	    level = raise_interrupt_level(NETS_DISABLE);
	    fddi_select_controller(mci, controller);
	    if (mci->flags & CBUSII) {
		mci->regptr->argreg = (k & 0xf);         /* Set page number */
		mci->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
		mci->regptr->argreg = FCI_CTRLR_PAGE_SELECT;
		mci->regptr->cmdreg = FCI_CTRLR_CMD;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x, page select", sresult);
		    goto quit;
		}
	    }
	    for (i=start; i<size;i++) {           /* Write address via FDDI */
		if (mci->flags & CBUS)
		    mci->regptr->argreg = i;
		else
		    mci->regptr->largreg = i;
		mci->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x ctrlr address %-04x", sresult, i);
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
		mci->regptr->argreg = FCI_CTRLR_MEMD_SELECT;
		mci->regptr->cmdreg = FCI_CTRLR_CMD;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x ctrlr memd select %-04x",
			   sresult, i);
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
		mci->regptr->argreg = i+1;
		mci->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x ctrlr data %-04x", sresult, i);
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
		mci->regptr->argreg = FCI_CTRLR_MEMD_WRITE;
		mci->regptr->cmdreg = FCI_CTRLR_CMD;
		if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		    reset_interrupt_level(level);
		    printf("\nError %-04x ctrlr memd write %-04x", sresult, i);
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
	    }
	    if (mci->flags & CBUS)
		mci->regptr->argreg = start;
	    else
		mci->regptr->largreg = start;
	    mci->regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	    if ((sresult = mci->regptr->cmdreg) != MCI_RSP_OKAY) {
		reset_interrupt_level(level);
		printf("\nError %-04x, memd select", sresult);
		level = raise_interrupt_level(NETS_DISABLE);
		fddi_select_controller(mci, controller);
	    }
	    for (i=start; i<size; i++) {
		result = mci->regptr->readshort;
		if (result != (0xffff & (i+1))) {
		    reset_interrupt_level(level);
		    if (mci->flags & CBUS)
			printf("\nError, location 0x%-08x read 0x%-04x expecting 0x%x",
			       i, (result & 0xFFFF), ((i+1) & 0xFFFF));
		    else
			printf("\nError, location 0x%-08x page 0x%-04x read 0x%-04x expecting 0x%x",
			       i, (k & 0xF), (result & 0xFFFF),
			       ((i+1) & 0xFFFF));
		    if (psipending())
			return;
		    level = raise_interrupt_level(NETS_DISABLE);
		    fddi_select_controller(mci, controller);
		}
	    }
	    reset_interrupt_level(level);
	}
	
	if (psipending())
	    return;
	pass = pass +1;
	if (pass > 10) {
	    printf("\nTotal of %d passes of 10...");
	    pass = 0;
	}
    }
 quit: 
    return;
}


/*
 * fddi_fill_af (mci)
 * fill the address filter with a value
 */

static void fddi_fill_af (mcitype *mci)
{
    mciregtype *regptr;
    ushort val, type, indx, max, mask;
    leveltype level;
    boolean ok, domask;
    char buff[BUFLEN];

    regptr = (mciregtype *) mci->regptr;
    type = cbus_select(mci, default_interface);
    mask = domask = 0;
    if (FCITYPE(type) == FCI_TYPE_FDDIT) {
	printf("\n  af_mask: ");
	ok = rdtty(buff, BUFLEN);
	if (ok && !null(buff)) {
	    mask = parse_unsigned(buff, &ok);
	    if (!ok)
	        return;
	    domask = 1;
	}
    }
    printf("\n  fill AF with: ");
    ok = rdtty(buff, BUFLEN);
    if (!ok || null(buff))
	return;
    val = parse_unsigned(buff, &ok);
    if (!ok)
        return;
    level = raise_interrupt_level(NETS_DISABLE);
    type = cbus_select(mci, default_interface);
    regptr->cmdreg = FCI_CMD_AF_SIZE;
    max = regptr->argreg;
    if (FCITYPE(type) == FCI_TYPE_FDDIT) 
	regptr->argreg = FDDIT_AF_FC_PAGE;
    else
	regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_AF_SELECT;
    for (indx = 0; indx < max; indx++) {
	regptr->argreg = val << 8;
	regptr->cmdreg = FCI_CMD_AF_WRITE;
    }
    if (domask) {
	cbus_ctrlr_select(mci, slot);
	regptr->argreg = mask;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_FDDI_AF_MASK;
	regptr->cmdreg = FCI_CTRLR_CMD;
    }
    reset_interrupt_level(level);
}


/*
 * fddi_bypass_switch
 */

static void fddi_bypass_switch (mcitype *mci)
{
    mciregtype *regptr;
    ushort sresult;
    int count;
    boolean ok;
    char buff[BUFLEN];
    leveltype level;

    regptr = (mciregtype *)mci->regptr;

    /*
     * Determine if the bypass switch is present.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    (void) cbus_select(mci, default_interface);
    regptr->argreg = 2 + (ENDECA << 2);
    regptr->cmdreg = FDDI_CMD_EN_SELECT;
    if ((sresult = regptr->cmdreg) != MCI_RSP_OKAY) {
	printf("\nError (%-04): endec select command", sresult);
	reset_interrupt_level(level);
	return;
    }
    regptr->cmdreg = FDDI_CMD_EN_READ;
    if ((sresult = regptr->cmdreg) != MCI_RSP_OKAY) {
	printf("\nError (%-04): endec read data", sresult);
	reset_interrupt_level(level);
	return;
    }
    sresult = regptr->argreg;
    printf("\nBypass switch is%spresent: %-04x", ((sresult & 0x8) ? " not " :
						    " "), sresult);
    
    /*
     * Read and write  the bypass switch.
     */
    regptr->argreg = 0x0002;
    regptr->cmdreg = FDDI_CMD_EN_SELECT;
    if ((sresult = regptr->cmdreg) != MCI_RSP_OKAY) {
	printf("\nError (%-04): select bypass switch", sresult);
	return;
    }
    printf("\nSetting bypass switch to BYPASS -- writing 0x0003");
    count = 1;
    printf("\nEnter number of times to write [1]: ");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff))
	count = parse_unsigned(buff, &ok);
    while (count--) {
	regptr->argreg = 0x3;
	regptr->cmdreg = FDDI_CMD_EN_WRITE;
	if ((sresult = regptr->cmdreg) != MCI_RSP_OKAY) {
	    printf("\nError (%-04): write bypass switch", sresult);
	    reset_interrupt_level(level);
	    return;
	}
    }
    if (yes_or_no("\nSet bypass switch to CONNECT ??", FALSE, TRUE) == FALSE) {
	printf("\nBypass switch left in bypass mode.");
	reset_interrupt_level(level);
	return;
    }
    printf("\nSetting bypass switch to CONNECT -- writing 0x0007");
    count = 1;
    printf("\nEnter number of times to write [1]: ");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff))
	count = parse_unsigned(buff, &ok);
    while (count--) {
	regptr->argreg = 0x7;
	regptr->cmdreg = FDDI_CMD_EN_WRITE;
	if ((sresult = regptr->cmdreg) != MCI_RSP_OKAY) {
	    printf("\nError (%-04): write bypass switch", sresult);
	    reset_interrupt_level(level);
	    return;
	}
    }
    printf("\nBypass switch left in connect mode.");
    reset_interrupt_level(level);
}

/*
 * fddi_hold_reg
 * torque the fddi hold or fddi_ctl register
 */

static void fddi_hold_reg (mcitype *mci)
{
    mciregtype *regptr;
    ushort val;
    leveltype level;
    boolean ok;
    char buff[BUFLEN];

    regptr = (mciregtype *) mci->regptr;
    printf("\n  new hold/ctl reg value: ");
    ok = rdtty(buff, BUFLEN);
    if (!ok || null(buff))
	return;
    val = parse_unsigned(buff, &ok);
    if (!ok)
	return;
    level = raise_interrupt_level(NETS_DISABLE);
    (void) cbus_select(mci, default_interface);
    cbus_ctrlr_select(mci, slot);
    regptr->argreg = val;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FDDI_HOLD_REG;
    regptr->cmdreg = FCI_CTRLR_CMD;
    reset_interrupt_level(level);
}

/*
 * fddi_alu_dump_registers
 * Function to read ALL fddi alu registers.
 */

static void fddi_alu_dump_registers (mcitype *mci)
{
    mciregtype *regptr;
    int indx, sresult;
    ushort regs[64], type;
    leveltype level;
    
    regptr = mci->regptr;
    
    /*
     * Select the proper controller -- based on default interface.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    type = cbus_select(mci, default_interface);
    cbus_ctrlr_select(mci, slot);
    for (indx = 0; indx < 64; indx++) {
	regptr->argreg = indx;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = 4;		/* register select */
	regptr->cmdreg = FCI_CTRLR_CMD;
	regptr->argreg = 6;		/* register read */
	regptr->cmdreg = FCI_CTRLR_CMD;
	sresult = regptr->cmdreg;
	if (regptr->cmdreg != MCI_RSP_OKAY) {
	    reset_interrupt_level(level);
	    printf("\nError 0x%-04x when reading Controller Register %x",
		   sresult,indx);
	    return;
	}
	regs[indx] = regptr->argreg;
	
    }
    reset_interrupt_level(level);

    if (FCITYPE(type) == FCI_TYPE_FDDIT) {
	printf("\n");
	fddit_print_regs(regs);
	return;
    }
    printf("\nFDDI registers - pointers");
    printf("\n  ccbptr 0x%-04x, icbptr 0x%-04x, regptr 0x%-04x, memptr 0x%-04x, af_addr 0x%-04x",
	   regs[0], regs[1], regs[2], regs[3], regs[4]);
    printf("\n  tx_cur 0x%-04x, tx_pnd 0x%-04x, tx_res 0x%-04x, tx_clm 0x%-04x, tx_bea 0x%-04x", 
	   regs[5], regs[6], regs[7], regs[8], regs[9]);
    printf("\n  rx_cur 0x%-04x, rx_pnd 0x%-04x, rx_recvd 0x%-04x, rx_free 0x%-04x",
	   regs[10], regs[11], regs[12], regs[13]);
    printf("\n  tx_clbn 0x%-04x, rx_error 0x%-04x, rx_head 0x%-04x, rx_tail 0x%-04x", 
	   regs[16], regs[17], regs[61], regs[62]);
    printf("\nFDDI registers - counts");
    printf("\n  tx_cur_cnt %d, tx_clm_len %d, tx_bea_len %d, tx_offset %d",
	   regs[21], regs[22], regs[23], regs[24]);
    printf("\n  tx_pnd_cnt %d, rx_pnd_type %d, rx_max_size %d, rx_offset_word %d",
	   regs[25], regs[26], regs[27], regs[28]);
    printf("\n  rx_rcv_cnt %d, rx_rmsk 0x%-04x, enable 0x%-04x, rx_offset_byte %d",
	   regs[29], regs[30], regs[31], regs[32]);
    printf("\n  ones_err 0x%-04x, zero_err 0x%-04x, ones_err_cnt %d, zero_err_cnt %d",
	   regs[18], regs[19], regs[33], regs[34]);
    printf("\nFDDI registers - data");
    printf("\n  argreg 0x%-04x, cmdreg 0x%-04x, temp 0x%-04x, temp1 0x%-04x",
	   regs[35], regs[36], regs[37], regs[38]);
    printf("\n  temp2 0x%-04x, temp3 0x%-04x, temp4 0x%-04x, temp5 0x%-04x", 
	   regs[39], regs[40], regs[41], regs[42]); 
    printf("\n  tx_state 0x%-04x, clm_bn_flg 0x%-04x, rx_remainder %-04d",
	   regs[43], regs[44], regs[45]);
    printf("\n  robin 0x%-04x, fsvld 0x%-04x", 
	   regs[46], regs[47]);
    printf("\nFDDI registers - counters");
    printf("\n  lstinc 0x%-04x, errinc 0x%-04x, frinc 0x%-04x",
	   regs[49], regs[50], regs[51]);
    printf("\n  fr_int_cnt 0x%-04x, eof 0x%-04x, xmtabti 0x%-04x, claim 0x%-04x",
	   regs[52], regs[53], regs[54], regs[55]);
    printf("\n  beacon 0x%-04x, async 0x%-04x, sync 0x%-04x, frmcpd 0x%-04x", 
	   regs[56], regs[57], regs[58], regs[59]);
    printf("\n  token 0x%-04x, sync_req 0x%-04x, async_req 0x%-04x, int_stat 0x%-04x", 
	   regs[60], regs[61], regs[62], regs[63]);

    printf("\n\n Diagnostic Registers:");
    printf("\n Allones Errors %d  Last Allones 0x%-04x",regs[55], regs[56]);
    printf("\n Zeros Errors   %d  Last Zeros   0x%-04x",regs[57], regs[51]);
}

/*
 * fddi_put_endec_reg
 */

static void fddi_put_endec_reg (
    mciregtype *regptr,
    int endec,
    int cr,
    int value)
{
    ushort sresult;
    
    regptr->argreg = ENDEC_CMD + (endec << 2);
    regptr->cmdreg = FDDI_CMD_EN_SELECT;
    regptr->argreg = cr;
    regptr->cmdreg = FDDI_CMD_EN_WRITE;
    sresult = regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
	printf("\nError on WRITE to endec CR write, error 0x%-04x", sresult);
	return;
    }
    regptr->argreg = ENDEC_DATA + (endec << 2);
    regptr->cmdreg = FDDI_CMD_EN_SELECT;
    regptr->argreg = value;
    regptr->cmdreg = FDDI_CMD_EN_WRITE;
    sresult = regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
	printf("\nError on WRITE to CR%-01d error 0x%-04x", cr, sresult);
	return;
    }
}


/*
 * fddi_get_endec_reg
 */

static ushort fddi_get_endec_reg (
    mciregtype *regptr,
    int endec,
    int cr)
{
    int sresult;

    sresult = ENDEC_CMD + (endec << 2);
    regptr->argreg = sresult;
    regptr->cmdreg = FDDI_CMD_EN_SELECT;
    regptr->argreg = cr;
    regptr->cmdreg = FDDI_CMD_EN_WRITE;
    sresult = regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
	buginf("\nget_endec_reg: Error writing cr select: %04x", sresult);
	return(-1);
    }
    sresult = ENDEC_DATA + (endec << 2);
    regptr->argreg = sresult;
    regptr->cmdreg = FDDI_CMD_EN_SELECT;
    regptr->cmdreg = FDDI_CMD_EN_READ;
    sresult = regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
	buginf("\nget_endec_reg: Error writing cr read: %04x", sresult);
	return(-1);
    }
    sresult = regptr->argreg;
    sresult = (sresult & 0x7);
    return(sresult);
}


/*
 * fddi_dump_endec
 */

static void fddi_dump_endec (mcitype *mci)
{
    mciregtype *regptr;
    leveltype level;
    
    /*
     * Display the current Endec registers.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    (void) cbus_select(mci, default_interface);
    regptr = mci->regptr;
    printf("\nA: %x %x %x %x, status %x",
	   fddi_get_endec_reg(regptr, ENDECA, FDDI_EN_RW_CR0),
	   fddi_get_endec_reg(regptr, ENDECA, FDDI_EN_RW_CR1),
	   fddi_get_endec_reg(regptr, ENDECA, FDDI_EN_RW_CR2),
	   fddi_get_endec_reg(regptr, ENDECA, FDDI_EN_RW_CR3),
	   fddi_get_endec_reg(regptr, ENDECA, FDDI_EN_R_STATUS));
    printf("\nB: %x %x %x %x, status %x",
	   fddi_get_endec_reg(regptr, ENDECB, FDDI_EN_RW_CR0),
	   fddi_get_endec_reg(regptr, ENDECB, FDDI_EN_RW_CR1),
	   fddi_get_endec_reg(regptr, ENDECB, FDDI_EN_RW_CR2),
	   fddi_get_endec_reg(regptr, ENDECB, FDDI_EN_RW_CR3),
	   fddi_get_endec_reg(regptr, ENDECB, FDDI_EN_R_STATUS));
    reset_interrupt_level(level);
}


/*
 * fddi_endec_write_reg
 */

static void fddi_endec_write_reg (mcitype *mci)
{
    mciregtype *regptr;
    boolean ok;
    int cr, value, endec;
    char buff[BUFLEN];
    leveltype level;
    
    regptr = mci->regptr;
    endec = ENDECA;
    printf("\n  write Endec? [A]: ");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff)) {
	if (toupper(buff[0]) == 'B')
	    endec = ENDECB;
    }
    cr = 1;
    printf("\n  Endec%c register (status is 4) [1]: ",
	   (endec == ENDECA) ? 'A' : 'B');
    ok = rdtty(buff, BUFLEN);
    if (!null(buff)) {
	cr = parse_unsigned(buff, &ok);
	if ((!ok) || (cr > 4)) {
 	    printf("\n%% Bad CR register -- assuming CR1");
	    cr = 1;
	}
    }
    if (cr == 3)
	cr = 4;
    else if (cr == 4)
	cr = 3;
    value = 0;
    printf("\n  value: ");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff))
	value = parse_unsigned(buff, &ok);
    level = raise_interrupt_level(NETS_DISABLE);
    (void) cbus_select(mci, default_interface);
    fddi_put_endec_reg(regptr, endec, cr, value);
    reset_interrupt_level(level);
}


/*
 * fddi_endec_read_reg
 */

static void fddi_endec_read_reg (mcitype *mci)
{
    mciregtype *regptr;
    boolean endec, ok;
    char buff[BUFLEN];
    int cr, reg;
    leveltype level;

    regptr = mci->regptr;
    endec = ENDECA;
    printf("\n  read Endec [A]: ");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff)) {
	if (toupper(buff[0]) == 'B')
	    endec = ENDECB;
    }
    cr = 1;
    printf("\n  Endec%c register [status is 4]: ",
	   (endec == ENDECA) ? 'A' : 'B');
    ok = rdtty(buff, BUFLEN);
    if (!null(buff)) {
	cr = parse_unsigned(buff, &ok);
	if ((!ok) || (cr > 4)) {
 	    printf("\n%% Bad CR register -- assuming CR1");
	    cr = 1;
	}
    }
    reg = cr;
    if (cr == 3) reg = 4;
    if (cr == 4) reg = 3;
    level = raise_interrupt_level(NETS_DISABLE);
    (void) cbus_select(mci, default_interface);
    printf("\n  Endec%c %d: %-01x", (endec == ENDECA) ? 'A' : 'B',
	   cr, fddi_get_endec_reg(regptr, endec, reg));
    reset_interrupt_level(level);
}


/*
 * fddi_formac_reset
 * Issue a reset to the formac.
 */
static boolean fddi_formac_reset (mcitype *mci)
{
    mciregtype *regptr;
    ushort sresult;
    
    regptr = mci->regptr;
    regptr->argreg = FDDI_FM_W_RESET;
    regptr->cmdreg = FDDI_CMD_FM_SELECT;
    sresult = regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
	printf("\nError when selecting a formac register = 0x%-04x", sresult);
	return(FALSE);
    }
    
    regptr->argreg = 0;
    regptr->cmdreg = FDDI_CMD_FM_WRITE;
    sresult = regptr->cmdreg;
    if (sresult != regptr->cmdreg) {
	printf("\nError when writing to FDDI_FM_W_RESET register = 0x%-04x",
	       sresult);
	return(FALSE);
    }
    return(TRUE);
}


static ushort fddi_get_formac_reg (mcitype *mci, ushort reg)
{
    mciregtype *regptr;

    regptr = mci->regptr;
    regptr->argreg = reg;
    regptr->cmdreg = FDDI_CMD_FM_SELECT;
    if (regptr->cmdreg != MCI_RSP_OKAY) {
	printf("\nError on select FDDI register %d, error 0x%-04x", 
	       reg, regptr->cmdreg);
	return(0);
    }
    regptr->cmdreg = FDDI_CMD_FM_READ;
    return(regptr->argreg);
}


static void fddi_put_formac_reg (
    mcitype *mci,
    ushort reg,
    ushort val)
{
    mciregtype *regptr;

    regptr = mci->regptr;
    regptr->argreg = reg;
    regptr->cmdreg = FDDI_CMD_FM_SELECT;
    if (regptr->cmdreg != MCI_RSP_OKAY) {
	printf("\nError on select FDDI register %d, error 0x%-04x", 
	       reg, regptr->cmdreg);
	return;
    }
    regptr->argreg = val;
    regptr->cmdreg = FDDI_CMD_FM_WRITE;
}


/*
 * fddi_dump_formac
 * Read all the formac registers.
 */

static void fddi_dump_formac (mcitype *mci)
{
    int i;
    mciregtype *regptr;
    ushort sresult[16];
    leveltype level;
    
    regptr = mci->regptr;

    level = raise_interrupt_level(NETS_DISABLE);
    (void) cbus_select(mci, default_interface);	 /* select default interface */
    
    /*
     * The formac has 15 associated registers. Some are read only, some
     * are read write and others are write only.
     *
     * lsbs: xxxx  tneg: xxxx  thtt: xxxx  trth: xxxx  mir: xxxxxxxx
     * tmax: xxxx  tvxt: xxxx  tpri: xxxx
     * mode: xxxx  stat: xxxx  imsk: xxxx  stmc: xxxx
     */
    for (i = 0; i < 16; i++)
	sresult[i] = fddi_get_formac_reg(mci,i);
    reset_interrupt_level(level);
    printf("\n    lsbs: %04x  tneg: %04x  thtt: %04x  trth: %04x  mir: %04x%04x",
	   sresult[0], sresult[1], sresult[4], sresult[5], sresult[2],
	   sresult[3]);
    printf("\n    tmax: %04x  tvxt: %04x  tpri: %04x",
	   sresult[6], sresult[8], sresult[15]);
    printf("\n    mode: %04x  stat: %04x  imsk: %04x  stmc: %04x",
	   sresult[10], sresult[12], sresult[11], sresult[9]);
    reset_interrupt_level(level);
}


static void fddi_read_formac_reg (mcitype *mci)
{
    mciregtype *regptr;
    boolean ok;
    ushort reg, val;
    char buff[BUFLEN];
    leveltype level;
    
    regptr = mci->regptr;
    printf("\nreg: ");
    reg = FDDI_FM_R_STATUS;		/* default to status register */
    ok = rdtty(buff, BUFLEN);
    if (!null(buff)) {
	reg = parse_unsigned(buff, &ok);
	if ((!ok) || (reg > 15)) {
 	    printf("\nbad register -- assuming Status (12)");
	    reg = FDDI_FM_R_STATUS;
	}
    }
    level = raise_interrupt_level(NETS_DISABLE);
    (void) cbus_select(mci, default_interface);
    val = fddi_get_formac_reg(mci, reg);
    reset_interrupt_level(level);
    printf("\nReg %02x = %04x", reg, val);
}


static void fddi_write_formac_reg (mcitype *mci)
{
    mciregtype *regptr;
    boolean ok;
    ushort reg, val;
    char buff[BUFLEN];
    leveltype level;
    
    regptr = mci->regptr;
    printf("\n  reg: ");
    reg = FDDI_FM_W_RESET_ADDRPTR;	/* default to status register */
    ok = rdtty(buff, BUFLEN);
    if (!null(buff)) {
	reg = parse_unsigned(buff, &ok);
	if ((!ok) || (reg > 15)) {
 	    printf("\nbad register -- assuming Clr Addr Pntr (12)");
	    reg = FDDI_FM_W_RESET_ADDRPTR;
	}
    }
    val = 0;
    printf("\n val: ");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff))
	val = parse_unsigned(buff, &ok);
    level = raise_interrupt_level(NETS_DISABLE);
    (void) cbus_select(mci, default_interface);
    fddi_put_formac_reg(mci, reg, val);
    reset_interrupt_level(level);
}


/*
 * fddi_claim_buffer
 * Generate a claim buffer. Assumes that the buffer pool has already 
 * been setup.
 */

static void fddi_claim_buffer (mcitype *mci)
{
    mciregtype *regptr;
    ushort sresult;
    int i;
    uchar *p;
    
    p = (uchar *) &fddi_claim_beacon_data;
    if (fddi_station_a) {
	ieee_copy(fddi_sa, &p[CLAIM_DA_OFFSET]);
	ieee_copy(fddi_sa, &p[CLAIM_SA_OFFSET]);
	ieee_copy(fddi_sa, &p[BEACON_SA_OFFSET]);
    } else {
	ieee_copy(fddi_da, &p[CLAIM_DA_OFFSET]);
	ieee_copy(fddi_da, &p[CLAIM_SA_OFFSET]);
	ieee_copy(fddi_da, &p[BEACON_SA_OFFSET]);				  
    }
    regptr = mci->regptr;
    
    /*
     * Get a buffer and construct the data.
     */
    regptr->argreg = FCI_BUF_POOL_0;
    regptr->cmdreg = FCI_CMD_TX0_RESERVE;
    sresult = regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
	printf("\nFCI%d: Claim/Beacon TX0_RESERVE failed 0x%-04x",
	       mci->unit, sresult);
	return;
    }
    
    /* 
     * Copy the data into the buffer and send it onto the FDDI.
     */
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_TX0_SELECT;
    for (i = 0; i < CLAIM_BUF_SIZE / 2; i++) 	
	regptr->write0short = fddi_claim_beacon_data[i];
    
    
    regptr->argreg = 0;
    regptr->cmdreg = FDDI_CMD_CLAIM_BEACON;
    sresult = regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
	printf("\nFCI%d: Claim/Beacon FDDI_CMD_CLAIM_BEACON failed 0x%-04x",
	       mci->unit, sresult);
	return;
    }
    printf("\nFCI%d: Claim/Beacon buffer sent to FDDI daughter board",
	   mci->unit);
}


/*
 * fddi_formac
 * The interface has been enabled -- now startup the formac chip.
 */

static void fddi_formac (mcitype *mci)
{
    mciregtype *regptr;
    ushort mode, sresult, *psa, s, data;
    boolean ok;
    char buff[BUFLEN], c;
    int indx, fddi_indx,  trt, tmax;
    
    regptr = mci->regptr;
    
    /*
     * Reset the endec(s)
     */
    printf("\nResetting ENDEC A:  ");
    fddi_put_endec_reg(regptr, ENDECA, FDDI_EN_RW_CR2, ENDEC_RESET);
    printf("\nResetting ENDEC B:  ");
    fddi_put_endec_reg(regptr, ENDECB, FDDI_EN_RW_CR2, ENDEC_RESET);
    
    /*
     * Reset the stupid chip i.e. now in initialize mode.
     */
    printf("\nResetting FORMAC:  ");
    if (fddi_formac_reset(mci) == FALSE)
	return;
    
    /*
     * Place FORMAC in initialize mode.
     */
    regptr->argreg = FDDI_FM_RW_MODE_REG;
    regptr->cmdreg = FDDI_CMD_FM_SELECT;
    regptr->argreg = 0;
    regptr->cmdreg = FDDI_CMD_FM_WRITE;	
    
    /*
     * Default is ENABLE REPEAT FILTER.
     */
    mode = 4;
    printf("\nEnter CR0 value [4] :");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff))
	mode = parse_unsigned(buff, &ok);
    
    fddi_put_endec_reg(regptr, ENDECA, FDDI_EN_RW_CR0, mode);
    fddi_put_endec_reg(regptr, ENDECB, FDDI_EN_RW_CR0, mode);
    
    /*
     * Default is select TA bus and loopback.
     */
    mode = 4;
    printf("\nEnter CR1 value [4] :");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff))
	mode = parse_unsigned(buff, &ok);
    
    fddi_put_endec_reg(regptr, ENDECA, FDDI_EN_RW_CR1, mode);
    fddi_put_endec_reg(regptr, ENDECB, FDDI_EN_RW_CR1, mode);
    
    /* 
     * Reset the address pointer.
     */
    regptr->argreg = FDDI_FM_W_RESET_ADDRPTR;
    regptr->cmdreg = FDDI_CMD_FM_SELECT;
    regptr->argreg = 0;
    regptr->cmdreg = FDDI_CMD_FM_WRITE;
    
    /*
     * Select the address register -- and write them.
     */
    regptr->argreg = FDDI_FM_RW_ADDR_REG;
    regptr->cmdreg = FDDI_CMD_FM_SELECT;
    
    trt = DEFAULT_TRT;
    trt = -trt;
    if (fddi_station_a)
	psa = (ushort *)&fddi_sa;
    else
	psa = (ushort *)&fddi_da;
    for (indx = 0; indx < 8; indx++) {
	if (indx == 1)
	    regptr->argreg = *psa++;
	else if (indx == 2)
	    regptr->argreg = *psa++;
	else if (indx == 3)
	    regptr->argreg = *psa;
	else if (indx == 6)
	    regptr->argreg = (trt >> 16);
	else if (indx == 7)
	    regptr->argreg = trt; 
	else 
 	    regptr->argreg = indx;
	regptr->cmdreg = FDDI_CMD_FM_WRITE;
	sresult = regptr->cmdreg;
	if (sresult != MCI_RSP_OKAY)
	    printf("\nError in write to formac address register 0x%-04x",
		   sresult);
    }
    
    /*
     * Now read back what we wrote into the address registers.
     */
    regptr->argreg = FDDI_FM_W_RESET_ADDRPTR;
    regptr->cmdreg = FDDI_CMD_FM_SELECT;
    regptr->argreg = 0;
    regptr->cmdreg = FDDI_CMD_FM_WRITE;
    
    regptr->argreg = FDDI_FM_RW_ADDR_REG;
    regptr->cmdreg = FDDI_CMD_FM_SELECT;
    printf("\n Address register space  ");
    for (indx = 0; indx < 8; indx++) {
    	regptr->cmdreg = FDDI_CMD_FM_READ;
	sresult = regptr->argreg;
	printf(" 0x%-04x", sresult);
    }
    
    
    /*
     * Select the TMAX register.
     */
    regptr->argreg = FDDI_FM_RW_TMAX;
    regptr->cmdreg = FDDI_CMD_FM_SELECT;
    
    /*
     * Write the value.
     */
    tmax = DEFAULT_TMAX;
    tmax = (tmax >> 5);
    regptr->argreg = tmax;
    regptr->cmdreg = FDDI_CMD_FM_WRITE;
    fddi_dump_formac(mci);
    
    /*
     * Select the TVX register.
     */
    regptr->argreg = FDDI_FM_RW_TVX;
    regptr->cmdreg = FDDI_CMD_FM_SELECT;
    
    /*
     * Write the value.
     */
    mode = DEFAULT_TVX;
    regptr->argreg = -mode;
    regptr->cmdreg = FDDI_CMD_FM_WRITE;
    
    /*
     * Setup broadcast address.
     */
    
    for (indx = 0; indx < 6; indx++) {
	c = baddr[indx];
	fddi_indx = indx;
	if (indx == 1)
	    fddi_indx = 4;
	else if (indx == 3)
	    fddi_indx = 6;
	else if (indx == 4)
	    fddi_indx = 1;
	s = (fddi_indx << 8) | c;
	regptr->argreg = s;
	regptr->cmdreg = FCI_CMD_AF_SELECT;
	regptr->cmdreg = FCI_CMD_AF_READ;
	data = regptr->argreg;
	data |= 0x0200;
	regptr->argreg = s;
	regptr->cmdreg = FCI_CMD_AF_SELECT;
	regptr->argreg = data;
	regptr->cmdreg = FCI_CMD_AF_WRITE;
    }
    
    /*
     * Setup station address.
     */
    for (indx = 0; indx < 6; indx++) {
	if (fddi_station_a)
	    c = fddi_sa[indx];
	else
	    c = fddi_da[indx]; 
	fddi_indx = indx;
	if (indx == 1)
	    fddi_indx = 4;
	else if (indx == 3)
	    fddi_indx = 6;
	else if (indx == 4)
	    fddi_indx = 1;
	s = (fddi_indx << 8) | c;
	regptr->argreg = s;
	regptr->cmdreg = FCI_CMD_AF_SELECT;
	regptr->cmdreg = FCI_CMD_AF_READ;
	data = regptr->argreg;
	data |= 0x0100;
	regptr->argreg = s;
	regptr->cmdreg = FCI_CMD_AF_SELECT;
	regptr->argreg = data;
	regptr->cmdreg = FCI_CMD_AF_WRITE;
    }
    
    mode = 0x1688;
    printf("\nEnter Formac interrupt mask [0x1688]:");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff))
	mode = parse_unsigned(buff, &ok);
    
    regptr->argreg = FDDI_FM_RW_INT_MASK;
    regptr->cmdreg = FDDI_CMD_FM_SELECT;
    regptr->argreg = mode;
    regptr->cmdreg = FDDI_CMD_FM_WRITE;
    
    printf("\nEnter Formac mode register setting [0x6020]:");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff))
	mode = parse_unsigned(buff, &ok);
    
    /*
     * Select the mode register.
     */
    regptr->argreg = FDDI_FM_RW_MODE_REG;
    regptr->cmdreg = FDDI_CMD_FM_SELECT;
    
    /*
     * Write the value.
     */
    
    regptr->argreg = mode;
    regptr->cmdreg = FDDI_CMD_FM_WRITE;
    
    /* 
     * Enable the FDDI daughter board interface.
     */
    printf("\nIssuing interface enable"); 
    regptr->argreg = MCI_IF_ENABLE;
    regptr->cmdreg = FCI_CMD_STATUS;  
}


/*
 * fddi_enable_fddi
 * do everything needed to turn formac on completely.
 */

static void fddi_enable_fddi (mcitype *mci)
{
    mciregtype *regptr;
    boolean ok;
    int number;
    ushort sresult;
    char buff[BUFLEN];
/*    leveltype level; */

/*    level = raise_interrupt_level(NETS_DISABLE); */
    regptr = mci->regptr;
    cbus_reset(mci);
    cbus_soft_reset(mci);
    
    printf("\nCreating pool 7 with one (1) 0x100 - 0xA buffer");
    cbus_create_pool(mci, 7, 0, 0x100 - 0xA, 1);
    /*
     * Setup some buffer pools
     */
    if (mci->flags & CBUS)
	number = 10;
    else
	number = 100;
    printf("\nEnter number of buffers to create [%d]: ", number);
    ok = rdtty(buff, BUFLEN);
    if (!null(buff))
	number = parse_unsigned(buff, &ok);
    cbus_create_pool(mci, FCI_BUF_POOL_0, 0, 4500/2, number);
    
    cbus_rx_buffersetup(mci, default_interface, FCI_BUF_POOL_0 ,
			FDDI_RECEIVE_WORD_OFFSET, 4500, number/2);
    cbus_tx_buffersetup(mci, default_interface, number/2, 4);
    cbus_select(mci, default_interface); /*select default interface*/
    regptr->argreg = MCI_IF_ENABLE;
    regptr->cmdreg = FCI_CMD_STATUS;
    sresult = regptr->cmdreg;
    printf("\nResult of interface enable: 0x%-04x",sresult);
    if (yes_or_no("Quit", FALSE, TRUE) == FALSE) {
/*	reset_interrupt_level(level); */
	return;
    }
    printf("\nCreating claim buffer");
    fddi_claim_buffer(mci);
    fddi_formac(mci);
/*    reset_interrupt_level(level); */
}


/*
 * fddi_force_claim_beacon
 */

static void fddi_force_claim_beacon (mcitype *mci)
{
    mciregtype *regptr;
    boolean ok;
    char buff[BUFLEN], cmdchar;
    int rcount, indx;
    ushort ring_op;
    leveltype level;

    regptr = mci->regptr;
    printf("\nForce Claim or Beacon [C | B]: ");
    ok = rdtty(buff, BUFLEN);
    cmdchar = 0;
    if (!null(buff))
	cmdchar = toupper(buff[0]);
    rcount = 1;
    printf("\nRepeat count [%d]: ", rcount);
    ok = rdtty(buff, BUFLEN);
    if (ok) {
	if (!null(buff))
	    rcount = parse_unsigned(buff, &ok);
    }    
    level = raise_interrupt_level(NETS_DISABLE);
    (void) cbus_select(mci, default_interface);
    for (indx = 0; indx < rcount; indx++) {
	regptr->argreg = default_interface;
	regptr->cmdreg = FCI_CMD_SELECT;
	
	/* 
	 * Ensure that the ring is operational.
	 */
	if (rcount > 1) {
	    do {
		regptr->argreg = FDDI_FM_R_STATUS;
		regptr->cmdreg = FDDI_CMD_FM_SELECT;
		regptr->cmdreg = FDDI_CMD_FM_READ;
		ring_op = (regptr->argreg & 0x8000);
	    } while (!ring_op);
	    switch (cmdchar) {
	      case 'B':
		regptr->argreg = FDDI_FM_W_BEACON_LISTEN;
		break;
	      case 'C':
		/*
		 * Force the ring to go down.
		 */
		regptr->argreg = FDDI_FM_W_CLAIM_LISTEN;
		break;
	      default:
		reset_interrupt_level(level);
		return;
	    }
	    regptr->cmdreg = FDDI_CMD_FM_SELECT;
	    regptr->argreg = 0;
	    regptr->cmdreg = FDDI_CMD_FM_WRITE;
	}
    }
    reset_interrupt_level(level);
}

/*
 * cbus_address
 * get a range of memory addresses
 */

void cbus_address (ulong *low, ulong *high, ulong max)
{
    ulong value;
    char buff[BUFLEN];
    boolean parsed;
    boolean okflag;

    while (TRUE) {
	if (stdio->statbits & (IDLETIMEOUT | CARDROP | IDLE | HANGUP))
	    return;
	value = *low;
        printf("\nlow address [0x%0x]: ", value);
	parsed = rdtty(buff, BUFLEN);
	if (parsed) {
	    if (!null(buff))
                value = parse_unsigned(buff, &okflag);
	    parsed = parsed && okflag && (value <= max);
	}
	if (!parsed)
	    continue;
	*low = value;
        value = (*high > *low) ? *high : *low + 0x100;
        printf("\nhigh address [0x%0x]: ", value);
	parsed = rdtty(buff, BUFLEN);
	if (parsed) {
	    if (!null(buff))
                value = parse_unsigned(buff, &okflag);
	    parsed = parsed && okflag && ((value > 0) && (value <= max));
	}
	if (!parsed)
	    continue;
	*high = value;
	return;
    }
}

/*
 * cbus_more
 * Asks the question " --More-- ", and returns TRUE if the user responsed
 * with a SPACE, "Y", or "y".  Returns FALSE otherwise.  Assumes STDIO.
 * Assumes TRUE always if screen length is zero.
 */

static boolean cbus_more (void)
{
    char c;

    if (stdio->tty_length == 0)
	return(TRUE);
    while (TRUE) {
	printf("\n --More-- ");
	flush();
	c = ttyin(stdio);
	c &= 0177;
	switch (c) {
	  case 040: 
	  case 'Y': 
	  case 'y': 
	      return(TRUE);
	  case '?':
	      printf("\nType a space to continue, anything else to quit");
	      break;
	  default: 
	      return(FALSE);
	}
    }
}

/*
 * cbus_quit_test
 * Like quit_display, only copes with interrupts 
 */

static boolean cbus_quit_test (int n)
{
    boolean code;
    
    if (n && (n % 22) == 0) {
	if (fddi_noint) {
	    set_interrupt_level(ALL_ENABLE);
	    code = cbus_more() ? FALSE : TRUE;
	    raise_interrupt_level(NETS_DISABLE);
	} else
	    code = cbus_more() ? FALSE : TRUE;
    } else
	code = FALSE;
    return(code);
}

/*
 * cbus_type_mema
 * type out a range of locations in mema
 *  
 */

static void cbus_type_mema (
    mcitype *mci,
    int low,
    int high)
{
    mciregtype *regptr;
    int i;
    ushort sresult;
    int linect;
    leveltype level;
    
    linect = 0;
    regptr = mci->regptr;
    for (i = low; i < high+1; i++) {
	level = raise_interrupt_level(NETS_DISABLE);
	regptr->argreg = i;
	regptr->cmdreg = FCI_CMD_MEMA_SELECT;
	regptr->cmdreg = FCI_CMD_MEMA_READ;
	sresult = regptr->argreg;
	reset_interrupt_level(level);
	if (cbus_quit_test(linect++))
	    return;
	printf("\nmema 0%-04o 0x%-04x: 0%-06o 0x%-04x", i, i, sresult, 
	       sresult);
    }
}

/*
 * cbus_type_memd
 * type out a range of locations in memd
 */

static void cbus_type_memd (
    mcitype *mci,
    ulong low,
    ulong high)
{
    mciregtype *regptr;
    ulong i;
    ushort result;
    int linect;
    leveltype level;
    
    linect = 0;
    regptr = mci->regptr;
    for (i = low; i < high+1; i++) {
	level = raise_interrupt_level(NETS_DISABLE);
	if (mci->flags & CBUSII)
	    regptr->largreg = i;
	else
	    regptr->argreg = i;
	regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	result = regptr->readshort;
	reset_interrupt_level(level);
	if (cbus_quit_test(linect++))
	    return;
        printf("\nmemd 0x%05x: 0x%04x", i, result);
    }
}

/*
 * cbus_type_afmem
 * type out a range of locations in a address filter ram
 */

static void cbus_type_afmem (
    mcitype *mci,
    int low,
    int high)
{
    mciregtype *regptr;
    register int i;
    ushort result;
    int linect;
    leveltype level;
    
    linect = 0;
    regptr = mci->regptr;
    for (i = low; i < high+1; i++) {
	level = raise_interrupt_level(NETS_DISABLE);
	cbus_select(mci, default_interface);
	regptr->argreg = i;
	regptr->cmdreg = FCI_CMD_AF_SELECT;
	if (regptr->cmdreg != FCI_RSP_OKAY)
	    buginf("\nError (%#x) address filter select", regptr->cmdreg);
	regptr->cmdreg = FCI_CMD_AF_READ;
	if (regptr->cmdreg != FCI_RSP_OKAY)
	    buginf("\nError (%#x) address filter read", regptr->cmdreg);
	result = regptr->argreg;
	reset_interrupt_level(level);
	if (cbus_quit_test(linect++))
	    return;
	printf("\nafmem 0%-06o 0x%-04x: 0%-06o 0x%-04x", i, i, result, result);
    }
}

/*
 * cbus_daughter_mema
 * Read the mema on a daughter card.
 */
static void cbus_daughter_mema (
    mcitype *mci,
    int low,
    int high)
{
    int linect, i, controller = 0;
    mciregtype *regptr;
    leveltype level;
    ushort result;
    char buff[BUFLEN];
    boolean parsed = FALSE;
	
    printf("\nSelect controller [%d]: ", controller);
    if (rdtty(buff, BUFLEN)) {
	if (!null(buff))
	    controller = parse_unsigned(buff, &parsed);
	if ((!parsed) || (controller > 3))
	    controller = 2;
    }
    linect = 0;
    regptr = mci->regptr;
    for (i = low; i < high+1; i++) {
	level = raise_interrupt_level(NETS_DISABLE);
	cbus_ctrlr_select(mci, controller);
	regptr->argreg = i;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_CMD_CTRLR_MEMA_SELECT;
	regptr->cmdreg = FCI_CTRLR_CMD;
	regptr->argreg = FCI_CMD_CTRLR_MEMA_READ;
	regptr->cmdreg = FCI_CTRLR_CMD;
	result = regptr->argreg;
	reset_interrupt_level(level);
	if (cbus_quit_test(linect++))
	    return;
	printf("\ndaughter mema 0%-06o 0x%-04x: 0%-06o 0x%-04x", i, i,
	       result, result);
    }
}

/*
 * fddi_loop_transmit
 * transmit a buffer for the fddi_mci_loopback routine
 */

static void fddi_loop_transmit (mcitype *mci, int unit)
{
    mciregtype *regptr;
    ushort sresult, tx_offset, rx_offset, type;
    int i;
    sys_timestamp start_time;
    int count, tx_count, rx_count, tx_size;
    boolean ok, send_immediate, receive, even;
    char buff[BUFLEN], cmdchar;
    uchar *p;

    type = cbus_select(mci, default_interface);
    tx_offset = 0;
    printf("\nEnter transmit offset [0]: ");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff))
	tx_offset = parse_unsigned(buff, &ok);
    fddi_fc_test_frame[0] = 0;
    p = (char *)&fddi_fc_test_frame;
    p += tx_offset;
    *p = 0x50;
    cmdchar = 'N';
    printf("\nSend synchronous FC [n]: ");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff))
	cmdchar = toupper(buff[0]);
    if (cmdchar == 'Y')
	*p = 0xD0;
    p++;
    cmdchar = 'O';
    printf("\nDestination??  - Yourself, Other, Broadcast [Y | O | B]: ");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff))
	cmdchar = toupper(buff[0]);
    if (fddi_station_a) {
	ieee_copy(fddi_sa, &p[6]);
	if (cmdchar == 'Y')
	    ieee_copy(fddi_sa, &p[0]);
	else if (cmdchar == 'B')
	    ieee_copy(baddr, &p[0]);
	else
	    ieee_copy(fddi_da, &p[0]);
    } else {
	ieee_copy(fddi_da, &p[6]);
	if (cmdchar == 'Y')
	    ieee_copy(fddi_da, &p[0]);
	else if (cmdchar == 'B')
	    ieee_copy(baddr, &p[0]);
	else
	    ieee_copy(fddi_sa, &p[0]);
    }
    rx_offset = 0;
    printf("\nEnter receive offset [0]: ");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff))
	rx_offset = parse_unsigned(buff, &ok);
    
    regptr = mci->regptr;
    
    cbus_ctrlr_select(mci, slot);
    regptr->argreg = rx_offset | (1 << 4);
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FDDI_HOLD_REG;
    regptr->cmdreg = FCI_CTRLR_CMD;

    /*
     * If address filter == 0x0100, receive the frame.
     */
    regptr->argreg = 0x0100;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FDDI_HEARSELF;
    regptr->cmdreg = FCI_CTRLR_CMD;
    
    count = 1;
    printf("\nEnter transmit loop count [1]:");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff))
	count = parse_unsigned(buff, &ok);
    if (!ok)
	count = 1;		/* bad parse, choose default */
    printf("\nTransmit count == %d", count);
    
    /*
     * See if send immediate is required.
     */
    send_immediate = FALSE;
    printf("\nIssue send immediate [n]: ");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff)) {
	cmdchar = toupper(buff[0]);
	if (cmdchar == 'y')
	    send_immediate = TRUE;
    }
    receive = FALSE;
    printf("\nReceive while transmitting [n]: ");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff)) {
	cmdchar = toupper(buff[0]);
	if (cmdchar == 'Y')
	    receive = TRUE;
    }
    tx_size = 100;
    printf("\nFrame size to send [%d]: ", tx_size);
    ok = rdtty(buff, BUFLEN);
    if (!null(buff)) {
	tx_size = parse_unsigned(buff, &ok);
	if (!ok)
	    tx_size = 100;	/* on bad parse, choose default */
    }
    even = TRUE;
    printf("\nEven or odd word [E] :");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff))
	cmdchar = toupper(buff[0]);
    if (cmdchar == 'O')
	even = FALSE;

    tx_count = 0;
    rx_count = 0;
    GET_TIMESTAMP(start_time);
 loop:
    tx_count++;
    if (psipending()) {
	printf("\nReceived %d Transmitted %d datagrams", rx_count, tx_count);
	return;
    }
    regptr->argreg = FCI_BUF_POOL_0;
    regptr->cmdreg = FCI_CMD_TX0_RESERVE;
    if ((sresult = regptr->cmdreg) != MCI_RSP_OKAY) {
	printf("\nloop_transmit: TX0_RESERVE failed - %#x", sresult);
	return;
    }
    
    /*
     * Set pointer to start of buffer (TX0_SELECT).
     */
    if (even)
	regptr->argreg = 0;
    else
	regptr->argreg = 1;
    regptr->cmdreg = FCI_CMD_TX0_SELECT;
    if ((sresult = regptr->cmdreg) != MCI_RSP_OKAY)
	printf("\nloop_transmit: TX0_SELECT failed = %#x", sresult);
    
    /*
     * Fill transmit buffer with recognizable pattern.
     * Then write byte length and start transmission.
     */
    for (i = 0; i < TESTLONGS; i++)
	regptr->write0long = fddi_fc_test_frame[i];
    
    regptr->write0long = tx_count;
    
    /*
     * Send the TX buffer
     */
    regptr->argreg = tx_size | ((tx_offset & 0x1) << 14);
    regptr->cmdreg = FCI_CMD_TX0_START;
    if ((sresult = regptr->cmdreg) != MCI_RSP_OKAY) {
	printf("\nloop_transmit: TX0_START failed - %#x", sresult);
    }
    
    if (send_immediate) {
 	regptr->argreg = FDDI_FM_W_SEND_NOW; 
	regptr->cmdreg = FDDI_CMD_FM_SELECT; 
	regptr->argreg = 3; 
	regptr->cmdreg = FDDI_CMD_FM_WRITE; 
    }
    if (receive) {
    receive_loop:
	regptr->cmdreg = FCI_CMD_RX_SETUP;
	sresult = regptr->argreg;
	if (sresult == default_interface) {
	    rx_count++;
	    regptr->cmdreg = FCI_CMD_RX_FLUSH;
	    goto receive_loop;
	}
    }
    
    if (--count)
	goto loop;
    printf("\nReceived %d Transmitted %d datagrams in %d ms", 
	   rx_count, tx_count, ELAPSED_TIME(start_time));
}

/*
 * fddi_loop_receive
 * Receive a buffer for fddi_mci_loopback routine
 */

static void fddi_loop_receive (register mcitype *mci)
{
    ushort index, length, status;
    ushort sresult, rx_offset;
    mciregtype *regptr;
    int rcv_count;
    boolean ok;
    char buff[BUFLEN];
    
    regptr = mci->regptr;
    rx_offset = 0;
    printf("\nEnter receive offset [0]: ");
    ok = rdtty(buff, BUFLEN);
    if (!null(buff))
	rx_offset = parse_unsigned(buff, &ok);
    
    rcv_count = 0;
    regptr->cmdreg = FCI_CMD_RX_SETUP;
    sresult = regptr->cmdreg;
    index = regptr->argreg;
 loop:
    if ((sresult != MCI_RSP_OKAY) || (index > 24)) {
	regptr->cmdreg = FCI_CMD_RX_FLUSH;
	if (index != 0xFFFF)
	    printf("\nRX_SETUP response 0x%-04x, index 0x%-04x", 
		   sresult, index);
    }
    else if (index != default_interface) {
	if (index != 0xFFFF) {
	    length = regptr->rxlength;
	    status = regptr->rxstatus;
	    printf("\nInterface %d length %d bytes, status %-04x, response %-04x", 
		   index, length, status, sresult);
	}
    } else
	rcv_count++;
    regptr->cmdreg = FCI_CMD_RX_FLUSH;
    
    if (psipending()) {
	printf("\nReceived %d datagrams", rcv_count);
	return;
    }
    /*
     * See if more (any) data is present.
     */
    regptr->cmdreg = FCI_CMD_RX_SETUP;
    sresult = regptr->cmdreg;
    index = regptr->argreg;
    goto loop;
}

/*
 * Direct cam access
 */

static void fddi_cam_access (mcitype *mci)
{
    mciregtype *regptr;
    leveltype level;
    char buff[BUFLEN], word[BUFLEN], *bufptr;
    ushort arg0, arg1, arg2, arg3, arg4, cmd, sresult;
    boolean parsed;

    (void) cbus_select(mci, default_interface);
    while (1) {
	printf("\ncmd [args...]: ");
	if (!rdtty(buff, BUFLEN) || null(buff))
	    break;
	bufptr = &buff[0];
	bufptr = gettoken(bufptr, word, BUFLEN, 0);
	if (word[0] == '\0')
	    break;
	cmd = (ushort) parse_unsigned(word, & parsed);
	if (!parsed)
	    break;
	bufptr = gettoken(bufptr, word, BUFLEN, 0);
	arg0 = (ushort) parse_unsigned(word, &parsed);
	bufptr = gettoken(bufptr, word, BUFLEN, 0);
	arg1 = (ushort) parse_unsigned(word, &parsed);
	bufptr = gettoken(bufptr, word, BUFLEN, 0);
	arg2 = (ushort) parse_unsigned(word, &parsed);
	bufptr = gettoken(bufptr, word, BUFLEN, 0);
	arg3 = (ushort) parse_unsigned(word, &parsed);
	bufptr = gettoken(bufptr, word, BUFLEN, 0);
	arg4 = (ushort) parse_unsigned(word, &parsed);
	regptr = mci->regptr;
	level = raise_interrupt_level(NETS_DISABLE);
	cbus_ctrlr_select(mci, slot);
	regptr->argreg = arg0;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_FDDI_CAM_WR_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	regptr->argreg = arg1;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_FDDI_CAM_WR_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	regptr->argreg = arg2;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_FDDI_CAM_WR_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	regptr->argreg = arg3;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_FDDI_CAM_WR_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	regptr->argreg = arg4;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_FDDI_CAM_WR_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	regptr->argreg = cmd;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_FDDI_CAM_CMD;
	regptr->cmdreg = FCI_CTRLR_CMD;
	sresult = regptr->cmdreg;
	if (sresult != MCI_RSP_OKAY) {
	    reset_interrupt_level(level);
	    printf("\n  cmd failed: %-04x", sresult);
	    level = raise_interrupt_level(NETS_DISABLE);
	}
	regptr->argreg = FCI_FDDI_CAM_RD_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	arg0 = regptr->argreg;
	regptr->argreg = FCI_FDDI_CAM_RD_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	arg1 = regptr->argreg;
	regptr->argreg = FCI_FDDI_CAM_RD_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	arg2 = regptr->argreg;
	regptr->argreg = FCI_FDDI_CAM_RD_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	arg3 = regptr->argreg;
	regptr->argreg = FCI_FDDI_CAM_RD_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	arg4 = regptr->argreg;
	regptr->argreg = FDDI_CAM_NOP;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_FDDI_CAM_CMD;
	regptr->cmdreg = FCI_CTRLR_CMD;
	reset_interrupt_level(level);
	printf("\nresultant args: 0x%-04x 0x%-04x 0x%-04x 0x%-04x 0x%-04x",
	       arg0, arg1, arg2, arg3, arg4);
    }
}

/*
 * cam dump
 *
Addr Mac          Ram   Mac          Ram   Mac          Ram   Mac          Ram
000: 000000000000 0000  000000000000 0000  000000000000 0000  000000000000 0000
004: 000000000000 0000  000000000000 0000  000000000000 0000  000000000000 0000
 */

static void fddi_cam_dump (mcitype *mci)
{
    mciregtype *regptr;
    leveltype level;
    ushort arg0, arg1, arg2, arg3, arg4, sresult;
    int i;

    (void) cbus_select(mci, default_interface);
    regptr = mci->regptr;
    level = raise_interrupt_level(NETS_DISABLE);
    cbus_ctrlr_select(mci, slot);
    regptr->argreg = FCRWC_NFA;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FDDI_CAM_WR_ARG;
    regptr->cmdreg = FCI_CTRLR_CMD;
    regptr->argreg = FDDI_CAM_RD_CTL;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FDDI_CAM_CMD;
    regptr->cmdreg = FCI_CTRLR_CMD;
    sresult = regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
	reset_interrupt_level(level);
	printf("\n  read of NFA failed: %-04x", sresult);
	level = raise_interrupt_level(NETS_DISABLE);
    }
    regptr->argreg = FCI_FDDI_CAM_RD_ARG;
    regptr->cmdreg = FCI_CTRLR_CMD;
    arg0 = regptr->argreg;
    regptr->argreg = FDDI_CAM_NOP;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_FDDI_CAM_CMD;
    regptr->cmdreg = FCI_CTRLR_CMD;
    reset_interrupt_level(level);
    printf("\nNFA: %03x", arg0 & 0x3ff);
    automore_enable("\nAddr Mac          Ram   Mac          Ram   Mac          Ram   Mac          Ram");
    for (i = 0; i < 0x400; i++) {
	level = raise_interrupt_level(NETS_DISABLE);
	cbus_ctrlr_select(mci, slot);
	regptr->argreg = i & 0x3ff;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_FDDI_CAM_WR_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	regptr->argreg = FDDI_CAM_RD_CAM;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_FDDI_CAM_CMD;
	regptr->cmdreg = FCI_CTRLR_CMD;
	sresult = regptr->cmdreg;
	if (sresult != MCI_RSP_OKAY) {
	    reset_interrupt_level(level);
	    printf("\n  FDDI_CAM_RD_CAM failed: %-04x", sresult);
	    level = raise_interrupt_level(NETS_DISABLE);
	}
	regptr->argreg = FCI_FDDI_CAM_RD_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	arg0 = regptr->argreg;
	regptr->argreg = FCI_FDDI_CAM_RD_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	arg1 = regptr->argreg;
	regptr->argreg = FCI_FDDI_CAM_RD_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	arg2 = regptr->argreg;
	regptr->argreg = FCI_FDDI_CAM_RD_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	arg3 = regptr->argreg;
	regptr->argreg = FCI_FDDI_CAM_RD_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	arg4 = regptr->argreg;
	regptr->argreg = FDDI_CAM_NOP;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_FDDI_CAM_CMD;
	regptr->cmdreg = FCI_CTRLR_CMD;
	reset_interrupt_level(level);
	if ((i % 4) == 0)
	    printf("\n%03x: %04x%04x%04x %04x", i, arg0, arg1, arg2, arg3);
	else
	    printf("  %04x%04x%04x %04x", arg0, arg1, arg2, arg3);
    }
    automore_disable();
}

/*
 * cbus_setup_unit
 * determine which unit, set up data structures
 */

static mcitype *cbus_setup_unit (void)
{
    mcitype *mci;
    uchar *mcibase;
    mciregtype *regptr;
    char buff[BUFLEN];
    boolean parsed;
    int unit;
    
    /*
     * figure out which unit number
     */
    unit = 0;
    mci = (mcitype*)cbusQ.qhead;
    unit = mci->unit;
    printf("\ncontroller number [%d]: ", unit);
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
	if (!null(buff))
	    unit = parse_unsigned(buff, &parsed);
	parsed = parsed && (unit <= FCI_MAX_CTRLR_NUMBER);
    }
    if (!parsed) {
	printf("\n%% bad controller number -- assuming 0");
	unit = 0;
    }
    
    /*
     * set up data block
     */
    mcibase = (uchar *) MCISTART + MCIBYTES*unit;
    if (touch_device(mcibase) == FALSE) {
	printf("\nmci: unit %d does not exist\n", unit);
	return(NULL);
    }
    mci = (mcitype *) & fddi_mcidata;
    mci->unit = unit;
    mci->regptr = (mciregtype *) mcibase;
    regptr = mci->regptr;
    regptr->argreg = FCI_HW_VERSION;
    regptr->cmdreg = FCI_CMD_VERSION;
    mci->hw_version = (regptr->argreg >> 8);
    if (mci->hw_version == CBUS_HARDWARE) {
	mci->flags = CBUS;
	mci->max_interfaces = CBUS1_MAX_INTERFACES;
    } else if ((mci->hw_version == CBUSII_HARDWARE) ||
	       reg_invoke_switch_proc_present(pslot_to_dbus(mci->unit))) {
	mci->flags = CBUSII;
	mci->max_interfaces = CBUS2_MAX_INTERFACES;
    } else {
	mci->max_interfaces = MCI_MAXUNITS;
    }
    default_interface = 0;

    printf("\ndefault interface number [%d]: ", default_interface);
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
	if (!null(buff))
	    default_interface = parse_unsigned(buff, &parsed);
	parsed = parsed && (default_interface < mci->max_interfaces);
    }
    if (!parsed) {
	printf("\n%% bad interface number - assuming 0");
	default_interface = 0;
    }
    cbus_unit2slotunit(default_interface, &slot, &slotunit);
    return(mci);
}

/*
 * cbus_mema_test
 * run tests of mema
 * returns FALSE if user quit out of error display
 */

#define CBUS1_MEMA_BASE  0400
#define CBUS2_MEMA_BASE	 0000

static boolean cbus_mema_test (mcitype *mci, int *linect)
{
    register ushort i;
    ushort sresult, mema_base;
    boolean errors;
    
    /*
     * mema -- write zeros
     */
    mema_base = ((mci->flags & CBUS) ? CBUS1_MEMA_BASE : CBUS2_MEMA_BASE);
    if (fddi_verbose)
	printf("\nmci: writing mema with zeros...");
    errors = FALSE;
    mci->regptr->argreg = mema_base;
    mci->regptr->cmdreg = FCI_CMD_MEMA_SELECT;
    for (i = mema_base; i < MEMA_WORDS; i++) {
	mci->regptr->argreg = 0;
	mci->regptr->cmdreg = FCI_CMD_MEMA_WRITE;
    }
    mci->regptr->argreg = mema_base;
    mci->regptr->cmdreg = FCI_CMD_MEMA_SELECT;
    for (i = mema_base; i < MEMA_WORDS; i++) {
	mci->regptr->cmdreg = FCI_CMD_MEMA_READ;
	sresult = mci->regptr->argreg;
	if ((sresult != 0) && fddi_reporting) {
	    errors = TRUE;
	    if (cbus_quit_test((*linect)++))
		return(FALSE);
	    printf("\nmci: wrote mema loc 0x%-04x with zeros, read 0x%-04x",
		   i, sresult);
	}
    }
    if (fddi_verbose && !errors)
	printf("[ok]");
    
    /*
     * mema -- write ones
     */
    if (fddi_verbose)
	printf("\nmci: writing mema with all ones...");
    errors = FALSE;
    mci->regptr->argreg = mema_base;
    mci->regptr->cmdreg = FCI_CMD_MEMA_SELECT;
    for (i = mema_base; i < MEMA_WORDS; i++) {
	mci->regptr->argreg = 0xffff;
	mci->regptr->cmdreg = FCI_CMD_MEMA_WRITE;
    }
    mci->regptr->argreg = mema_base;
    mci->regptr->cmdreg = FCI_CMD_MEMA_SELECT;
    for (i = mema_base; i < MEMA_WORDS; i++) {
	mci->regptr->cmdreg = FCI_CMD_MEMA_READ;
	sresult = mci->regptr->argreg;
	if ((sresult != 0xffff) && fddi_reporting) {
	    errors = TRUE;
	    if (cbus_quit_test((*linect)++))
		return(FALSE);
	    printf("\nmci: wrote mema loc 0x%-04x with all ones, read 0x%-04x",
		   i, sresult);
	}
    }
    if (fddi_verbose && !errors)
	printf("[ok]");
    
    /*
     * mema -- write address + 1
     */
    if (fddi_verbose)
	printf("\nmci: writing mema with address+1...");
    errors = FALSE;
    mci->regptr->argreg = mema_base;
    mci->regptr->cmdreg = FCI_CMD_MEMA_SELECT;
    for (i = mema_base; i < MEMA_WORDS; i++) {
	mci->regptr->argreg = i+1;
	mci->regptr->cmdreg = FCI_CMD_MEMA_WRITE;
    }
    mci->regptr->argreg = mema_base;
    mci->regptr->cmdreg = FCI_CMD_MEMA_SELECT;
    for (i = mema_base; i < MEMA_WORDS; i++) {
	mci->regptr->cmdreg = FCI_CMD_MEMA_READ;
	sresult = mci->regptr->argreg;
	if ((sresult != i+1) && fddi_reporting) {
	    errors = TRUE;
	    if (cbus_quit_test((*linect)++))
		return(FALSE);
	    printf("\nmci: wrote mema loc %#x with 0x%-04x, read 0x%-04x",
		   i, i+1, sresult);
	}
    }
    if (fddi_verbose && !errors)
	printf("[ok]");
    return(TRUE);
}

/*
 * cbus_memd_test
 * run tests of memd
 * returns FALSE if user quit out of error display
 */

#define MEMD_STEP  1		/* 1 == look at every word of memory */
#define MEMD_BASE  0x100	/* don't run tests below here */

static boolean cbus_memd_test (mcitype *mci, int *linect)
{
    ulong i;
    ushort sexpect, sresult;
    boolean errors;
    
    /*
     * memd - all zeros
     */
    if (fddi_verbose)
	printf("\nmci: writing memd with all zeros...");
    errors = FALSE;
    if (mci->flags & CBUS)
	mci->regptr->argreg = MEMD_BASE;
    else
	mci->regptr->largreg = MEMD_BASE;
    mci->regptr->cmdreg = FCI_CMD_MEMD_SELECT;
    for (i = MEMD_BASE; i < MEMD_WORDS; i += MEMD_STEP)
	mci->regptr->write0short = 0L;
    if (mci->flags & CBUS)
	mci->regptr->argreg = MEMD_BASE;
    else
	mci->regptr->largreg = MEMD_BASE;
    mci->regptr->cmdreg = FCI_CMD_MEMD_SELECT;
    for (i = MEMD_BASE; i < MEMD_WORDS; i += MEMD_STEP) {
	sresult = mci->regptr->readshort;
	if ((sresult != 0) && fddi_reporting) {
	    errors = TRUE;
	    if (cbus_quit_test((*linect)++))
		return(FALSE);
	    printf("\nmci: wrote memd loc %#x with zeros, read 0x%-04x",
		   i, sresult);
	}
    }
    if (fddi_verbose && !errors)
	printf("[ok]");
    
    /*
     * memd - all ones
     */
    if (fddi_verbose)
	printf("\nmci: writing memd with all ones...");
    errors = FALSE;
    if (mci->flags & CBUS)
	mci->regptr->argreg = MEMD_BASE;
    else
	mci->regptr->largreg = MEMD_BASE;
    mci->regptr->cmdreg = FCI_CMD_MEMD_SELECT;
    for (i = MEMD_BASE; i < MEMD_WORDS; i += MEMD_STEP)
	mci->regptr->write0short = 0xffff;
    if (mci->flags & CBUS)
	mci->regptr->argreg = MEMD_BASE;
    else
	mci->regptr->largreg = MEMD_BASE;
    mci->regptr->cmdreg = FCI_CMD_MEMD_SELECT;
    for (i = MEMD_BASE; i < MEMD_WORDS; i += MEMD_STEP) {
	sresult = mci->regptr->readshort;
	if ((sresult != 0xffff) && fddi_reporting) {
	    errors = TRUE;
	    if (cbus_quit_test((*linect)++))
		return(FALSE);
	    printf("\nmci: wrote memd loc %#x with ones, read 0x%-04x",
		   i, sresult);
	}
    }
    if (fddi_verbose && !errors)
	printf("[ok]");
    
    /*
     * memd - address+1
     */
    if (fddi_verbose)
	printf("\nmci: writing memd with address+1...");
    errors = FALSE;
    mci->regptr->argreg = MEMD_BASE;
    mci->regptr->cmdreg = FCI_CMD_MEMD_SELECT;
    for (i = MEMD_BASE; i < MEMD_WORDS; i += MEMD_STEP)
	mci->regptr->write0short = ((ushort)i)+1;
    if (mci->flags & CBUS)
	mci->regptr->argreg = MEMD_BASE;
    else
	mci->regptr->largreg = MEMD_BASE;
    mci->regptr->cmdreg = FCI_CMD_MEMD_SELECT;
    for (i = MEMD_BASE; i < MEMD_WORDS; i += MEMD_STEP) {
	sresult = mci->regptr->readshort;
	sexpect = ((ushort) i) + 1;
	if ((sresult != sexpect) && fddi_reporting) {
	    errors = TRUE;
	    if (cbus_quit_test((*linect)++))
		return(FALSE);
	    printf("\nmci: wrote memd loc %#x with 0x%-04x, read 0x%-04x",
		   i, sexpect, sresult);
	}
    }
    if (fddi_verbose && !errors)
	printf("[ok]");
    return(TRUE);
}

/*
 * fddi_mema_readwrite
 * Special read/write testing for scoping mema failures.
 */

static void fddi_mema_readwrite (
    mcitype *mci,
    ushort loc,
    ushort data)
{
    mciregtype *regptr;
    ushort result;
    regptr = mci->regptr;
    result = 0;
    fddi_psistart();
    printf("\nWriting mema location %-03x with data %-04x. Read with %s",
	   loc, data, fddi_escape);
    while (!psipending()) {
	regptr->argreg = loc;
	regptr->cmdreg = FCI_CMD_MEMA_SELECT;
	mci->regptr->argreg = data;
	mci->regptr->cmdreg = FCI_CMD_MEMA_WRITE;
    }
    fddi_psiclear();
    printf("\nReading mema location %-03x. End test with %s", loc,
	   fddi_escape);
    while (!psipending()) {
	regptr->argreg = loc;
	regptr->cmdreg = FCI_CMD_MEMA_SELECT;
	mci->regptr->cmdreg = FCI_CMD_MEMA_READ;
	result = mci->regptr->argreg;
    }
    printf("\nRead a %-03x from %-04x", result, loc);
}

/*
 * fddi_addressfilter_readwrite
 * Special read/write testing for scoping af mem
 */

static boolean fddi_af_readwrite (
    mcitype *mci,
    ushort unit,
    ushort address,
    ushort data)
{
    mciregtype *regptr;
    ushort result;
    
    regptr = mci->regptr;
    
    /*
     * Select the interface after resetting world
     */
    cbus_reset(mci);
    if (!cbus_select(mci, unit))
	return(FALSE);
    printf("\nReading AF%d location %-04x.  Go to writing with %s",
	   unit, address, fddi_escape);
    while (!psipending()) {
	regptr->argreg = address;
	regptr->cmdreg = FCI_CMD_AF_SELECT;
	regptr->cmdreg = FCI_CMD_AF_READ;
	result = regptr->argreg;
    }
    fddi_psiclear();
    printf("\nWriting AF%d location %-04x with data %-04x.  End test with %s",
	   unit, address, data, fddi_escape);
    while (!psipending()) {
	regptr->argreg = address;
        regptr->cmdreg = FCI_CMD_AF_SELECT;
        regptr->argreg = data<<8;
        regptr->cmdreg = FCI_CMD_AF_WRITE;
    }
    return(TRUE);
}

/*
 * fddi_memd_readwrite
 * Special read/write testing for scoping memd failures.
 */

static void fddi_memd_readwrite (
    mcitype *mci,
    ulong loc,
    ushort data)
{
    mciregtype *regptr;
    ushort result;
    
    result = 0;
    regptr = mci->regptr;
    printf("\nReading memd location %-04x.  Go to writing with %s",
	   loc, fddi_escape);
    while (!psipending()) {
	if (mci->flags & CBUS)
	    regptr->argreg = loc;
	else
	    regptr->largreg = loc;
	regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	result = regptr->readshort;
    }
    fddi_psiclear();
    printf("\nWriting memd location %-04x with data %-04x.  End test with %s",
	   loc, data, fddi_escape);
    while (!psipending()) {
	if (mci->flags & CBUS)
	    regptr->argreg = loc;
	else
	    regptr->largreg = loc;
	regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	regptr->write0short = data;
    }
    printf("\nRead a %-04x from memd location %-05x",result,loc);
}

/*
 * fddi_memd_write
 * Write once and return
 */

static void fddi_memd_write (mcitype *mci, ulong loc, ushort data)
{
    mciregtype *regptr;
    
    regptr = mci->regptr;
    if (mci->flags & CBUS) {
	regptr->argreg = loc;
	regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	regptr->write0short = data;
	regptr->argreg = loc;
    } else {
	regptr->largreg = loc;
	regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	regptr->write0short = data;
	regptr->largreg = loc;
    }
    regptr->cmdreg = FCI_CMD_MEMD_SELECT;
    data = regptr->readshort;
    printf("\nRead a %-04x from memd location %-04x", data, loc);
}

/*
 * fddi_memd_read
 * Read once from selected location
 */

static void fddi_memd_read (mcitype *mci, ulong loc)
{
    mciregtype *regptr;
    ushort result;
    
    regptr = mci->regptr;
    if (mci->flags & CBUS)
	regptr->argreg = loc;
    else
	regptr->largreg = loc;
    regptr->cmdreg = FCI_CMD_MEMD_SELECT;
    result = regptr->readshort;
    printf("\nRead a %-04x from memd location %-04x", result, loc);
}

/* fddi_memd_load
 * Load up all of memd with data
 */

static void fddi_memd_load (mcitype *mci, ushort data)
{
    mciregtype *regptr;
    int i;
    
    regptr = mci->regptr;
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_MEMD_SELECT;
    for (i=0; i<=MaxMemD; i++) 
	regptr->write0short = data;
}

/*
 * cbus_execute_special
 * execute special diagnostic tests
 */

static void cbus_execute_special (
    mcitype *mci,
    uchar cmdchar,
    char *args)
{
    ushort unit, sresult, data;
    ulong address;
    boolean parsed = FALSE;
    
    fddi_verbose = FALSE;
    fddi_reporting = FALSE;
    switch (cmdchar) {
      case 'A':
        address = parse_unsigned(args, &parsed);
        args = (char *) deblank(args);
        while ((*args != ' ') && (*args != '\0') && (*args != '\t'))
            args++;
        data = parse_unsigned(args, &parsed);
        fddi_mema_readwrite(mci, address, data);
	break;
      case 'B':
	data = parse_unsigned(args, &parsed);
	printf("\nRunning bus test with 0x%-04x until ctrl/c typed...", data);
	while (TRUE) {
	    mci->regptr->argreg = (ushort) data;
	    sresult = mci->regptr->argreg;
	    if (psipending())
		break;
	}
	break;
      case 'C':
	address = parse_unsigned(args, &parsed);
	args = (char *) deblank(args);
	while ((*args != ' ') && (*args != '\0') && (*args != '\t'))
	    args++;
	data = parse_unsigned(args, &parsed);
	fddi_mema_readwrite(mci, address, data);
	break;    
      case 'D':
        address = parse_unsigned(args, &parsed);
	args = (char *) deblank(args);
	while ((*args != ' ') && (*args != '\0') && (*args != '\t'))
	    args++;
        data = parse_unsigned(args, &parsed);
	fddi_memd_readwrite(mci, address, data);
	break;
      case 'F':
	unit = parse_unsigned(args, &parsed);
	args = (char *) deblank(args);
	while ((*args != ' ') && (*args != '\0') && (*args != '\t'))
	    args++;
	address = parse_unsigned(args, &parsed);
	args = (char *) deblank(args);
	while ((*args != ' ') && (*args != '\0') && (*args != '\t'))
	    args++;
	data = parse_unsigned(args, &parsed);
	fddi_af_readwrite(mci, unit, address, data);
	break;
      case 'W':
        address = parse_unsigned(args, &parsed);
	args = (char *) deblank(args);
	while ((*args != ' ') && (*args != '\0') && (*args != '\t'))
	    args++;
        data = parse_unsigned(args, &parsed);
	fddi_memd_write(mci, address, data);
	break;
      case 'R':
	address = parse_unsigned(args, &parsed);
	fddi_memd_read(mci, address);
	break;
      case 'L':
	data = parse_unsigned(args, &parsed);
	fddi_memd_load(mci, data);
	break;
      default:
	printf("\nSecond character of x command runs test:\n");
	printf("\nA                        - mema diagnostic");
	printf("\nB<data>                  - bus test");
	printf("\nC<address> <data>        - mema read/write test");
	printf("\nD<address> <data>        - memd read/write test");
	printf("\nE                        - memd diagnostic");
	printf("\nF<unit> <address> <data> - address filter read/write test");
	printf("\nR<address>               - read memd once");
	printf("\nW<address> <data>        - write memd once");
	printf("\nL<data>                  - fill memd with data");
	break;
    }
    fddi_verbose = TRUE;
    fddi_reporting = TRUE;
}

/*
 * cbus_execute_command
 * given a command code, execute and return result
 */

static void cbus_execute_command (
    mcitype *mci,
    ulong *arglong,
    int *command,
    int *rcount)
{
    char buff[BUFLEN];
    ushort sresult, response;
    boolean parsed;
    int j;
    
    printf("\nrepeat count [%d]: ", *rcount);
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
        if (!null(buff))
	    *rcount = (int) parse_unsigned(buff, &parsed);
    }
    printf("\nargument [%d]: ", *arglong);
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
	if (!null(buff))
	    *arglong = parse_unsigned(buff, &parsed);
    }
    if (!parsed) {
	printf("\n%% bad argument");
	return;
    }
    printf("\ncommand [%d]: ", *command);
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
	if (!null(buff))
	    *command = parse_unsigned(buff, &parsed);
    }
    if (!parsed) {
	printf("\n%% bad command code");
	return;
    }
    
    sresult = 0;
    response = 0;
    for (j = 0; j < *rcount; j++) {
	if (mci->flags & CBUSII)
	    mci->regptr->largreg = *arglong;
	else
	    mci->regptr->argreg = (ushort) (*arglong);
	mci->regptr->cmdreg = *command;
	if (*command == FCI_CMD_RESET) {
	    DELAY(5000);
	}
	response = mci->regptr->cmdreg;
	sresult = mci->regptr->argreg;
	if (response != MCI_RSP_OKAY) {
	    printf("\nbad response: 0x%-04x on iteration %d", response, j);
	    return;
	}
    }
    printf("\nvalue returned: 0x%-04x or 0%-06o", sresult, sresult);
}


/*
 * cbus_daughter_command
 * given a command code, execute and return result
 */

static void cbus_daughter_command (mcitype *mci,
			    ulong *arglong,
			    int *command,
			    int *rcount)
{
    char buff[BUFLEN];
    leveltype level;
    ushort sresult, response;
    boolean parsed;
    int j;

    printf("\nrepeat count [%d]: ", *rcount);
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
        if (!null(buff))
	    *rcount = (int) parse_unsigned(buff, &parsed);
    }
    printf("\ndaughter argument [%d]: ", *arglong);
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
	if (!null(buff))
	    *arglong = parse_unsigned(buff, &parsed);
    }
    if (!parsed) {
	printf("\n%% bad argument");
	return;
    }
    printf("\ndaughter command [%d]: ", *command);
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
	if (!null(buff))
	    *command = parse_unsigned(buff, &parsed);
    }
    if (!parsed) {
	printf("\n%% bad command code");
	return;
    }
    
    sresult = 0;
    response = 0;
    for (j=0; j<*rcount; j++) {
	level = raise_interrupt_level(NETS_DISABLE);
	cbus_select(mci, default_interface);    
	if ((sresult=cbus_ctrlr_select(mci, slot)) != MCI_RSP_OKAY) {
	    reset_interrupt_level(level);
	    printf("\n% select failed on controller %d - response: %x",
		   slot, sresult);
	    return;
	}
	mci->regptr->argreg = (ushort) (*arglong);
	mci->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	response = mci->regptr->cmdreg;
	if (response != MCI_RSP_OKAY) {
	    reset_interrupt_level(level);
	    printf("\nError: failed to give arg, response=0x%x, it=%d",
		   response, j);
	    return;
	}
	mci->regptr->argreg = *command;
	mci->regptr->cmdreg = FCI_CTRLR_CMD;
	response = mci->regptr->cmdreg;
	sresult = mci->regptr->argreg;
	if (response != MCI_RSP_OKAY) {
	    reset_interrupt_level(level);
	    printf("\nbad response: 0x%x, iteration %d", response, j);
	    return;
	}
	reset_interrupt_level(level);
    }
    printf("\nvalue returned: 0x%x", sresult);
}


/*
 * cbus_bus_test
 * make sure we can read/write commands to this device
 */

static boolean cbus_bus_test (mcitype *mci, int *linect)
{
    uint i;
    ushort sresult;
    boolean errors;
    
    /*
     * make sure we can read and write the alu registers
     */
    if (fddi_verbose)
	printf("\nmci: starting bus test...");
    errors = FALSE;
    for (i = 0; i < (0xffff & MEMD_WORDS); i++) {
	mci->regptr->argreg = (ushort) i;
	sresult = mci->regptr->argreg;
	if ((sresult != i) && fddi_reporting) {
	    errors = TRUE;
	    if (cbus_quit_test((*linect)++))
		return(FALSE);
	    printf("\nmci: bus or alu problem: wrote 0x%-04x, read 0x%-04x",
		   i, sresult);
	}
    }
    if (fddi_verbose && !errors)
	printf("[ok]");
    return(TRUE);
}

/*
 * cbus_config
 * report configuration of this controller.
 */

static void cbus_config (mcitype *mci)
{
    register int i;
    ushort sresult, typecode;
    ulong lresult;
    mciregtype *regptr;

    /*
     * reset controller.
     */
    cbus_reset(mci);
    regptr = mci->regptr;
    
    /*
     * read version number.
     */
    regptr->argreg = FCI_VERSION_SW;
    regptr->cmdreg = FCI_CMD_VERSION;
    lresult = (ulong) regptr->argreg;
    regptr->argreg = FCI_VERSION_HW;
    regptr->cmdreg = FCI_CMD_VERSION;
    sresult = regptr->argreg;
    if (fddi_verbose)
	printf("\ncbus: f/w version: %04x   h/w version: %04x", lresult,
	       sresult);
    
    /*
     * determine initial memory size
     */
    regptr->argreg = 1;
    regptr->cmdreg = FCI_CMD_MEMSIZE;
    if (mci->flags & CBUS) {
	sresult = regptr->argreg;
	lresult = (ulong) sresult;
    } else
	lresult = regptr->largreg;
    if (fddi_verbose)
	printf("\nmci: memd size is 0x%-08x words", lresult);
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_MEMSIZE;
    if (mci->flags & CBUS) {
	sresult = regptr->argreg;
	lresult = (ulong) sresult;
    } else
	lresult = regptr->largreg;
    if (fddi_verbose)
	printf("\nmci: mema size is 0x%-08x words", lresult);
    
    /*
     * scan controller reporting interfaces
     */
    cbus_soft_reset(mci);
    for (i = 0; i < mci->max_interfaces; i++) {
	regptr->argreg = i;
	regptr->cmdreg = FCI_CMD_SELECT;
	sresult = regptr->cmdreg;
	if (sresult == MCI_RSP_ERR_NPRS)
	    continue;
	if (sresult != MCI_RSP_OKAY) {
	    if (fddi_verbose)
		printf("\nmci: interface %d select error %#x", i, sresult);
	} else {
	    typecode = regptr->argreg;
	    if (fddi_verbose)
	        printf("\nmci: interface %d is %s", i,
		       cbus_devicename(typecode));
	    if ((FCITYPE(typecode) == FCI_TYPE_ETHER) ||
		(FCITYPE(typecode) == FCI_TYPE_FDDI) ||
		(FCITYPE(typecode) == FCI_TYPE_FDDIT))
	        bia_stationaddress(mci, i);
	}
    }
}


/*
 * fddi_af_test
 * run tests on the address filter.
 * returns FALSE if user quit out of error display
 */

static boolean fddi_af_test (mcitype *mci, int *linect)
{
    mciregtype *regptr;
    register uint i;
    ushort sresult;
    boolean errors;
    
    regptr = mci->regptr;

    /*
     * af - all zeros
     */
    if (fddi_verbose)
	printf("\nmci: writing address filter with all zeros...");
    errors = FALSE;
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_AF_SELECT;
    for (i = 0; i < MEC_AF_WORDS; i += AF_STEP) {
        if (psipending())
            return(FALSE);
	regptr->argreg = 0;
	regptr->cmdreg = FCI_CMD_AF_WRITE;
    }

    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_AF_SELECT;
    for (i = 0; i < MEC_AF_WORDS; i += AF_STEP) {
        if (psipending())
            return(FALSE);
	regptr->cmdreg = FCI_CMD_AF_READ;
	sresult = regptr->argreg;
	sresult = ((sresult >> 8) & 0xff);
	if ((sresult != 0) && fddi_reporting) {
	    errors = TRUE;
	    if (cbus_quit_test((*linect)++)) {
		return(FALSE);
	    }
	    printf("\nmci: wrote af loc %#x with zeros, read 0x%-04x",
		   i, sresult);
	}

    }
    if (fddi_verbose && !errors)
	printf("[ok]");

    if (fddi_verbose)
	printf("\nmci: writing address filter with all ones...");
    errors = FALSE;
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_AF_SELECT;
    for (i = 0; i < MEC_AF_WORDS; i += AF_STEP) {
        if (psipending())
            return(FALSE);
	regptr->argreg = 0xffff;
	regptr->cmdreg = FCI_CMD_AF_WRITE;
    }

    /*
     * Read back the data.
     */
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_AF_SELECT;
    for (i = 0; i < MEC_AF_WORDS; i += AF_STEP) {
        if (psipending())
            return(FALSE);
	regptr->cmdreg = FCI_CMD_AF_READ;
	sresult = regptr->argreg;
	sresult = ((sresult >> 8) & 0xff);
	if ((sresult != 0xff) && fddi_reporting) {
	    errors = TRUE;
	    if (cbus_quit_test((*linect)++)) {
		return(FALSE);
	    }
	    printf("\nmci: wrote af loc %#x with ones, read 0x%-04x",
		   i, sresult);
	}
    }
    if (fddi_verbose && !errors)
	printf("[ok]");

    if (fddi_verbose)
	printf("\nmci: writing address filter with address+1...");
    errors = FALSE;
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_AF_SELECT;
    for (i = 0; i < MEC_AF_WORDS; i += AF_STEP) {
        if (psipending())
            return(FALSE);
	regptr->argreg = (i + 1) & 0xFF;
	regptr->cmdreg = FCI_CMD_AF_WRITE;
    }

    /*
     * Read back the data.
     */
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_AF_SELECT;
    for (i = 0; i < MEC_AF_WORDS; i += AF_STEP) {
        if (psipending())
            return(FALSE);
	regptr->cmdreg = FCI_CMD_AF_READ;
	sresult = regptr->argreg;
	sresult = ((sresult >> 8) & 0xff);
	if ((sresult != ((i + 1) & 0xFF)) && fddi_reporting) {
	    errors = TRUE;
	    if (cbus_quit_test((*linect)++)) {
		return(FALSE);
	    }
	    printf("\nmci: wrote af loc %#x with %d, read 0x%-04x",
		   i, (i + 1) & 0xFF, sresult);
	}
    }
    if (fddi_verbose && !errors)
	printf("[ok]");

    return(TRUE);
}

/*
 * mec_af_test
 * run memory test over ethernet address filter memories
 */

static boolean mec_af_test (mcitype *mci, int *linect, int unit)
{
    ushort i;
    ushort sresult, data;
    boolean errors;
    
    /*
     * select the interface
     */
    cbus_soft_reset(mci);
    if (!cbus_select(mci, unit))
	return(FALSE);
    
    /*
     * ethernet address memory -- write zeros
     */
    if (fddi_verbose)
	printf("\nmci: writing interface %d memory with zeros...", unit);
    errors = FALSE;
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = FCI_CMD_AF_SELECT;
    for (i = 0; i < MEC_AF_WORDS; i += AF_STEP) {
	mci->regptr->argreg = 0;
	mci->regptr->cmdreg = FCI_CMD_AF_WRITE;
    }
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = FCI_CMD_AF_SELECT;
    for (i = 0; i < MEC_AF_WORDS; i += AF_STEP) {
	mci->regptr->cmdreg = FCI_CMD_AF_READ;
	sresult = mci->regptr->argreg;
	sresult &= 0xff00;
	if ((sresult != 0) && fddi_reporting) {
	    errors = TRUE;
	    if (cbus_quit_test((*linect)++))
		return(FALSE);
	    printf("\nmci: wrote af%d mem loc %#x with zeros, read 0x%-04x",
		   unit, i, sresult);
	}
    }
    if (fddi_verbose && !errors)
	printf("[ok]");
    
    /*
     * ethernet address memory -- write ones
     */
    if (fddi_verbose)
	printf("\nmci: writing interface %d memory with ones...", unit);
    errors = FALSE;
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = FCI_CMD_AF_SELECT;
    for (i = 0; i < MEC_AF_WORDS; i += AF_STEP) {
	mci->regptr->argreg = 0xffff;
	mci->regptr->cmdreg = FCI_CMD_AF_WRITE;
    }
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = FCI_CMD_AF_SELECT;
    for (i = 0; i < MEC_AF_WORDS; i += AF_STEP) {
	mci->regptr->cmdreg = FCI_CMD_AF_READ;
	sresult = mci->regptr->argreg;
	sresult &= 0xff00;
	if ((sresult != 0xff00) && fddi_reporting) {
	    errors = TRUE;
	    if (cbus_quit_test((*linect)++))
		return(FALSE);
	    printf("\nmci: wrote af%d mem loc %#x with ones, read 0x%-04x",
		   unit, i, sresult);
	}
    }
    if (fddi_verbose && !errors)
	printf("[ok]");
    
    /*
     * ethernet address memory -- write address + 1
     */
    if (fddi_verbose)
	printf("\nmci: writing interface %d memory with address+1...", unit);
    errors = FALSE;
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = FCI_CMD_AF_SELECT;
    for (i = 0; i < MEC_AF_WORDS; i += AF_STEP) {
	mci->regptr->argreg = ((i+1) << 8);	/* data write in high byte */
	mci->regptr->cmdreg = FCI_CMD_AF_WRITE;
    }
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = FCI_CMD_AF_SELECT;
    for (i = 0; i < MEC_AF_WORDS; i += AF_STEP) {
	mci->regptr->cmdreg = FCI_CMD_AF_READ;
	sresult = mci->regptr->argreg;
	sresult &= 0xff00;
	data = ((i+1) << 8);
	if ((sresult != data) && fddi_reporting) {
	    errors = TRUE;
	    if (cbus_quit_test((*linect)++))
		return(FALSE);
	    printf("\nmci: wrote af%d mem loc %#x with 0x%-04x, read 0x%-04x",
		   unit, i, data, sresult);
	}
    }
    if (fddi_verbose && !errors)
	printf("[ok]");
    return(TRUE);
}


/*
 * cbus_basic_tests
 *
 * Run basic multibus access tests
 * See what the configuration looks like
 * test cbus mema
 * test cbus memd
 * other things
 */



static void cbus_create_memory (mcitype *mci)
{
    int txtimes, pass;
    mciregtype *regptr;
    
    regptr = mci->regptr;
    txtimes = 4;				/* number of tx buffers */
    pass = 5;					/* number of passes */
    cbus_reset(mci);				/* reset the world */
    cbus_soft_reset(mci);			/* start controllers */
    
    /*
     * use pool FCI_BUF_POOL_0.
     * no overhead, buf_words data words, rxoffset is rx deposit.
     * allocate 10 total buffers, 5 max rx and 5 max for tx.
     * preserve 2 from switching.
     */
    cbus_create_pool(mci, FCI_BUF_POOL_0, 0, BUF_WORDS, 500);
    cbus_rx_buffersetup(mci, default_interface, FCI_BUF_POOL_0, RXOFFSET,
			BUF_WORDS, 500);
    cbus_tx_buffersetup(mci, default_interface, 500, 2);
    printf("\nbailing out after carving up memory");
}

static void cbus_basic_tests (mcitype *mci)
{
    int linect, i;
    long passes;
    char buff[BUFLEN];
    boolean parsed;
    
    /*
     * get number of passes
     */
    passes = 1;
    printf("\npasses [%d]: ", passes);
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
	if (!null(buff))
	    passes = parse_unsigned(buff, &parsed);
	parsed = parsed && (passes > 0);
    }
    if (!parsed) {
	printf("\n%% bad pass count");
	return;
    }
    
    /*
     * ask about verbose and error reporting modes.
     */
    printf("\nreport errors [y]: ");
    fddi_reporting = (rdtty(buff, BUFLEN) &&
		      toupper(*buff) == 'N') ? FALSE : TRUE;
    if (fddi_reporting) {
	printf("\nverbose mode [y]: ");
	fddi_verbose = (rdtty(buff, BUFLEN) && toupper(*buff) == 'N') ?
	    FALSE : TRUE;
    } else {
	fddi_verbose = FALSE;
    }
    
    linect = 0;
    raise_interrupt_level(NETS_DISABLE); 
    while (passes--) {
	
	/*
	 * run bus tests
	 */
	if (cbus_bus_test(mci, &linect) == FALSE)
	    goto done;
	
	/*
	 * report configuration
	 */
	cbus_config(mci);
	
	/*
	 * run mema tests
	 */
	if (cbus_mema_test(mci, &linect) == FALSE)
	    goto done;
	
	/*
	 * run memd tests
	 */ 
	if (cbus_memd_test(mci, &linect) == FALSE)
	    goto done;
	
	if (mci->flags & CBUS) {
	    /*
	     * Run address filter tests
	     */
	    if (fddi_af_test(mci, &linect) == FALSE)
		goto done;
	    
	    /*
	     * run ethernet memory diagnostics
	     */
	    for (i = 0; i < mci->max_interfaces; i++) {
		if (mec_af_test(mci, &linect, i) == FALSE)
		    goto done;
		printf("\nskipping filter loopback tests....");
	    }
	}
    }
 done:
    set_interrupt_level(ALL_ENABLE);
}

/*
 * fddi_keepalive_adjust
 * Disable keepalive on all interfaces
 */

static void fddi_keepalive_adjust (void)
{
    hwidbtype *idb;
    
    FOR_ALL_HWIDBS(idb)
	idb->nokeepalive = TRUE;
}

/*
 * cbus_list
 * Follow a single MCI linked list
 */

static void cbus_list (
    mcitype *mci,
    int head,
    char *name,
    boolean mema)
{
    ushort ptr, tail, lastone;
    mciregtype *regptr;
    int count;
    leveltype level;
    
    regptr = mci->regptr;
    level = raise_interrupt_level(NETS_DISABLE);
    regptr->argreg = head;
    if (mema) {
	regptr->cmdreg = FCI_CMD_MEMA_SELECT;
	regptr->cmdreg = FCI_CMD_MEMA_READ;
	ptr = regptr->argreg;
	regptr->cmdreg = FCI_CMD_MEMA_READ;
	tail = regptr->argreg;
    } else {
	regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	ptr = regptr->readshort;
	tail = regptr->readshort;
    }
    reset_interrupt_level(level);
    lastone = 0;
    printf("\n\n%s list has following elements:\n", name);
    count = 0;
    while (ptr) {
	count++;
	if ((count % 12) == 0)
	    printf("\n");
  	printf("%-04x ", ptr);
	lastone = ptr;
	level = raise_interrupt_level(NETS_DISABLE);
	regptr->argreg = ptr;
	regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	ptr = regptr->readshort;
	reset_interrupt_level(level);
	if (lastone == ptr) break;
	if (psipending())
	    break;
    }
    printf("(%d total)", count);
    if (tail != lastone && ((head < 0xee || head > 0xfd) || mema))
	printf("\nTail pointer should be %-04x, found %-04x", tail, lastone);
    if (ptr == lastone)
      printf("\nFound Pointer to itself %-04x",ptr);
}

/*
 * cbus_follow_lists
 * Follow all normal linked lists in the microce
 */

static void cbus_follow_lists (mcitype *mci)
{
#define MEMA_LIST 	TRUE
    cbus_list(mci, 034, "Transmit", MEMA_LIST);
    cbus_list(mci, 036, "Raw input", MEMA_LIST);
    cbus_list(mci, 056, "Host input", MEMA_LIST);
    cbus_list(mci, 0100, "Ethernet 0 RX", MEMA_LIST);
    cbus_list(mci, 0130, "Serial 0 RX", MEMA_LIST);
    cbus_list(mci, 0160, "Ethernet 1 RX", MEMA_LIST);
    cbus_list(mci, 0210, "Serial 1 RX", MEMA_LIST);
}

/*
 * cbus_interactive_list
 */
static void cbus_interactive_list (
    mcitype *mci,
    int *laddr,
    boolean *lmem)
{
    int value;
    boolean mem, parsed;
    char buff[BUFLEN];
    
    while (TRUE) {
	if (stdio->statbits & (IDLETIMEOUT | HANGUP | IDLE | CARDROP))
	    return;
	value = *laddr;
        printf("\nlink address [%0x]: ", value);
	parsed = rdtty(buff, BUFLEN);
	if (parsed) {
	    if (!null(buff))
                value = parse_unsigned(buff, &parsed);
	    parsed = parsed && (value <= 0xffff);
	}
	if (!parsed)
	    continue;
	*laddr = value;
	mem = *lmem;
	printf("\nmemory to search [MEM-%s]: ", (mem ? "A" : "D"));
	parsed = rdtty(buff, BUFLEN);
	if (parsed) {
	    if (!null(buff)) {
		parsed = ((toupper(*buff) == 'A') || (toupper(*buff) == 'D'));
		mem = FALSE;
		if (toupper(*buff) == 'A')
		    mem = TRUE;
	    }
	}
	if (!parsed)
	    continue;
	*lmem = mem;
	return;
    }
}


/*
 * cbus_search
 * Search mema and memd for a pattern
 */

static void cbus_search (mcitype *mci)
{
    char buff[BUFLEN];
    mciregtype *regptr;
    ushort pattern, result;
    int linect;
    ulong i;
    boolean parsed;
#ifdef C7000
    int memdbase = 0x20;	/* skip local locks */
#else
    int memdbase = 0;
#endif

    pattern = 0;
    printf("\nPattern: ");
    if (rdtty(buff, BUFLEN))
        pattern = (ushort) parse_unsigned(buff, &parsed);
    else
	return;
    linect = 0;
    regptr = mci->regptr;
    printf("\nSearching for %-04x...", pattern);
    for (i = 0; i < MEMA_WORDS; i++) {
	if (mci->flags & CBUS)
	    regptr->argreg = i;
	else
	    regptr->largreg = i;
	regptr->cmdreg = FCI_CMD_MEMA_SELECT;
	regptr->cmdreg = FCI_CMD_MEMA_READ;
	result = regptr->argreg;
	if (result == pattern) {
	    if (cbus_quit_test(linect++))
		return;
	    printf("\nmema 0x%-04x: 0x%-04x", i, result);
	}
    }
    for (i = memdbase; i < MEMD_WORDS; i++) {
	if (mci->flags & CBUS)
	    regptr->argreg = i;
	else
	    regptr->largreg = i;
	regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	result = regptr->readshort;
	if (result == pattern) {
	    if (cbus_quit_test(linect++))
		return;
	    printf("\nmemd 0x%-05x: 0x%-04x", i, result);
	}
    }
}

#ifndef STANDALONE
/*
 * Interrupt routines for running with system software
 */

void fddi_psistart (void)
{
    psion(stdio);
}

void fddi_psiclear (void)
{
    psioff(stdio);
    psion(stdio);
}

void fddi_psiend (void)
{
    psioff(stdio);
}
#endif STANDALONE

#ifdef STANDALONE

/*
 * Interrupt routines for running stand alone
 */

void fddi_psistart ()
{
    abort = FALSE;
}

void fddi_psiclear ()
{
    abort = FALSE;
}

void fddi_psiend ()
{
    abort = FALSE;
}

boolean psipending ()
{
    return(!abort);
}
#endif STANDALONE

/*
 * cbus_cache_lists
 * Function to print out the contents of the mema cache .
 */

static void cbus_cache_lists (mcitype *cbus)
{
#define CA_VERSION	01203
#define HASH_BASE 	01300
    
    mciregtype *regptr;
    leveltype level;
    int indx, linect, jndx, mac_size, depth;
    ushort uslink, usentry, usvalue, usversion, usentry_version;
    boolean entry_found;
    ipaddrtype addr;	

    addr = 0;
    linect = 0;
    entry_found = FALSE;
    regptr = cbus->regptr;

    /*
     * Get global version number.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    regptr->argreg = CA_VERSION;
    regptr->cmdreg = FCI_CMD_MEMA_SELECT;
    regptr->cmdreg = FCI_CMD_MEMA_READ;
    usversion = regptr->argreg;
    reset_interrupt_level(level);

    /*
     * Scan the hash table
     */
    for (indx = 0; indx < NETHASHLEN; indx++) {
	level = raise_interrupt_level(NETS_DISABLE);
	regptr->argreg = HASH_BASE + indx;
	regptr->cmdreg = FCI_CMD_MEMA_SELECT;
	regptr->cmdreg = FCI_CMD_MEMA_READ;
	uslink = regptr->argreg;
	reset_interrupt_level(level);
	depth = 0;
	while (uslink) {
	    entry_found = TRUE;
	    mac_size = 1;
	    if (cbus_quit_test(linect++))
		return;
	    usentry = uslink;
	    level = raise_interrupt_level(NETS_DISABLE);
	    
	    /*
	     * Get link to next entry.
	     */
	    regptr->argreg = usentry;
	    regptr->cmdreg = FCI_CMD_MEMA_SELECT;
	    regptr->cmdreg = FCI_CMD_MEMA_READ;
	    uslink = regptr->argreg;		/* link to next entry */
	    reset_interrupt_level(level);
	    printf("\n%-02x/%3d %-04x ", indx, depth, usentry);

	    /*
	     * Get version of entry.
	     */
	    level = raise_interrupt_level(NETS_DISABLE);
	    regptr->argreg = usentry + 1;
	    regptr->cmdreg = FCI_CMD_MEMA_SELECT;
	    regptr->cmdreg = FCI_CMD_MEMA_READ;
	    usentry_version = regptr->argreg;
	    
	    /*
	     * Get length of mac encapsulation.
	     */
	    regptr->argreg = usentry + 2;
	    regptr->cmdreg = FCI_CMD_MEMA_SELECT;
	    regptr->cmdreg = FCI_CMD_MEMA_READ;
	    mac_size = regptr->argreg;

	    /*
	     * Get the IP address.
	     */
	    regptr->argreg = usentry + 4;
	    regptr->cmdreg = FCI_CMD_MEMA_SELECT;
	    regptr->cmdreg = FCI_CMD_MEMA_READ;
	    addr = regptr->argreg;
	    addr = (addr << 16);
	    regptr->argreg = usentry + 5;
	    regptr->cmdreg = FCI_CMD_MEMA_SELECT;
	    regptr->cmdreg = FCI_CMD_MEMA_READ;
	    addr |= regptr->argreg;
	    reset_interrupt_level(level);
	    printf("%c%15i ", ((usversion == usentry_version) ? '*' : ' '),
		   addr);
	    for (jndx = 0; jndx < mac_size; jndx++) {
		level = raise_interrupt_level(NETS_DISABLE);
		regptr->argreg = usentry + jndx + 6;
		regptr->cmdreg = FCI_CMD_MEMA_SELECT;
		regptr->cmdreg = FCI_CMD_MEMA_READ;
		usvalue = regptr->argreg;
		reset_interrupt_level(level);	
		printf("%-04x", usvalue);
	    }
	    depth++;
	}
    }
    if (!entry_found)
	printf("\nCache empty");
    
}



#if 0
/*
 * cbus_fci_page
 * Select a page for read/write
 */

static void cbus_fci_page (mcitype *mci)
{
    int controller, page, data, loop, temp, temp1;
    ulong address, result;
    boolean read, parsed, reporting;
    leveltype level;
    char buff[BUFLEN];
    
    controller = 0;
    page = 0;
    address = 0;
    data = 0;
    loop = 1;
    printf("\nSelect controller [%d]: ", controller);  /* controller */
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
 	if (!null(buff))
	    controller = parse_unsigned(buff, &parsed);
    }
    printf("\nSelect Page number [%d]: ", page);   /* Page */
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
 	if (!null(buff))
	    page = (int) parse_unsigned(buff, &parsed);
    }
    printf("\nSelect Absolute Address [%d]: ", address);  /* address */
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
 	if (!null(buff))
	    address = parse_unsigned(buff, &parsed);
    }
    printf("\nSet Loop Count [%d]: ", loop);  /* address */
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
 	if (!null(buff))
	    loop = (int) parse_unsigned(buff, &parsed);
    }
    printf("\nReport [y]: ");                /* reporting mode */
    reporting = ((rdtty(buff, BUFLEN) &&
		  (toupper(buff[0]) == 'N')) ? FALSE : TRUE);
    printf("\nRead Data [y]: ");                /* reporting mode */
    read = ((rdtty(buff, BUFLEN) &&
	     (toupper(buff[0]) == 'N')) ? FALSE : TRUE);
    
    if (!read) {
	printf("\nEnter Write Data [%d]: ", data);  /* address */
	parsed = rdtty(buff, BUFLEN);
	if (parsed) {
	    if (!null(buff))
		data = parse_unsigned(buff, &parsed);
	}
    }
    level = raise_interrupt_level(NETS_DISABLE);
    if (reporting)
	printf("\nReset Complex");
    cbus_reset(mci);                            /* Hard Reset */
#ifdef C7000
    cxbus_soft_reset(mci);			/* Soft reset  */
    cbus_ctrlr_select(mci, controller);
#else
    mci->regptr->argreg = FCI_SOFT_RESET;           /* Soft reset */
    mci->regptr->cmdreg = FCI_CMD_RESET;
    DELAY(100);
    
    cbus_ctrlr_select(mci, controller);

    mci->regptr->argreg = controller;
    mci->regptr->cmdreg = FCI_CMD_CTRLR_LOAD; 	    /* Make sure running */
#endif
    mci->regptr->argreg = page;                     /* Select page */
    mci->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    mci->regptr->argreg = FCI_CTRLR_PAGE_SELECT;
    mci->regptr->cmdreg = FCI_CTRLR_CMD;
    if ((result = mci->regptr->cmdreg) != MCI_RSP_OKAY)
	buginf("\nError %-04x, page select", result);
    result = mci->regptr->argreg;
    reset_interrupt_level(level);
    if (reporting)
	printf("\nPage number is 0x%-04x", result);
    while (loop--) {
	level = raise_interrupt_level(NETS_DISABLE);
	mci->regptr->largreg = address;                /* Select Memory */
	mci->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	mci->regptr->argreg = FCI_CTRLR_MEMD_SELECT;
	mci->regptr->cmdreg = FCI_CTRLR_CMD;
	if ((result = mci->regptr->cmdreg) != MCI_RSP_OKAY)
	    buginf("\nError %-04x, memd select", result);
	if (reporting) {
	    result = mci->regptr->argreg;
	    printf("\nWritten to Page register is: 0x%-04x", result);
	}
	if (!read) {
	    mci->regptr->argreg = data;                /* Write data */
	    mci->regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	    mci->regptr->argreg = FCI_CTRLR_MEMD_WRITE;
	    mci->regptr->cmdreg = FCI_CTRLR_CMD;
	    if ((result = mci->regptr->cmdreg) != MCI_RSP_OKAY)
		buginf("\nError %-04x, memd write", result);
	    if (reporting) {
		result = mci->regptr->argreg;
		temp  = result >> 12;
		temp1 = result & 0xfff;
		printf("\nController computes address: 0x%-04x, Page: 0x%-02x, Relative address: 0x%-03x", result, temp, temp1);
	    }
	    reset_interrupt_level(level);
	} else {
	    mci->regptr->argreg = FCI_CTRLR_MEMD_READ;   /* Read Data */
	    mci->regptr->cmdreg = FCI_CTRLR_CMD;
	    if (reporting) {
		result = mci->regptr->argreg;
		printf("\n Read : 0x%-04x", result);
	    }
	    reset_interrupt_level(level);
	}
    }
}

#endif



/*
 * lowcore_command
 *
 * Special lowcore commands
 */
static void lowcore_command (mciregtype *regptr)

{
    printf("No longer supported.\n");
    printf("Try 'show controller cxbus [memd-core | memd-capture]'\n");
}


/*
 * fsip_type_sram
 * type out a range of locations in memd
 */

static void fsip_type_sram (
    ushort slot,
    dgtype *dgp,
    ulong low,
    ulong high,
    ulong ldata,
    boolean write)
{
    ulong i;
    ulong data;
    uchar cdata[4];
    uchar parity;
    int linect;
    leveltype level;

    dbus_prot_wr(slot, &dgp->diag_control, 4);
    dbus_prot_wr(slot, &dgp->last_command_or_address, 0);
    DELAY(2);
    dbus_prot_wr(slot, &dgp->diag_status, DBUS_RESET_VALUE);
    dgp->ucode_command_and_data = 0x10;
    dgp->ucode_command_and_data = 0x10;
    dgp->ucode_command_and_data = 0x22;
    
    dgp->ucode_command_and_data = low & 0xff;
    dgp->ucode_command_and_data = (low>>8) & 0xff;
    dgp->ucode_command_and_data = (low>>16) & 0xff;
    dgp->ucode_command_and_data = (low>>24) & 0xff;

    linect = 0;
    for (i = low; i < high+1; i+=4) {
	level = raise_interrupt_level(NETS_DISABLE);
	dgp->ucode_command_and_data = 0x10;
	dgp->ucode_command_and_data = 0x20;
	dgp->ucode_command_and_data = 0xE2;

	cdata[0] = dgp->ucode_command_and_data;
	cdata[1] = dgp->ucode_command_and_data;
	cdata[2] = dgp->ucode_command_and_data;
	cdata[3] = dgp->ucode_command_and_data;

	data = *(ulong *)&cdata[0];
	
	dgp->ucode_command_and_data = 0x10;
	dgp->ucode_command_and_data = 0x21;
	dgp->ucode_command_and_data = 0xE0;

	parity = dgp->ucode_command_and_data;
	reset_interrupt_level(level);
	
	if (cbus_quit_test(linect++))
	    return;
	printf("\nsram 0x%08x: 0x%08x (parity=%02x)", i, data, parity);
    }
}


#define MAX_FSIP_SRAM (128 * 1024)

/*
 * fsip_specific
 *
 * Special fsip commands
 */
static void fsip_specific (mcitype *mci)
{
    ulong mhigh = MAX_FSIP_SRAM, mlow = 0;
    dgtype *dgp;
    volatile uchar *diag;
    char *str;
    boolean parsed;
    char cmdchar = '?';
    char buff[BUFLEN];
    int last_reg = 5;
    uchar reg_data = 0;
    ushort slot2 = 2;
    ulong last_interface = 2;
    int interface = -1;
    ulong last_icmd = 0;
    ulong last_iarg = 0;
    ulong last_ccmd = 0;
    ulong last_carg = 0;
    ulong sresult;
    ulong ldata;
    ushort response;
    ushort i, j, sdata;
    int linect;
    leveltype level;
    mciregtype* regptr;

    regptr = mci->regptr;
    dgp = pslot_to_dbus(slot2);
    diag = (uchar *)dgp;
    
    while (TRUE) {
	printf("\n%s (? for help) [%c]: ", "fsip", cmdchar);
	parsed = rdtty(buff, BUFLEN);
	if (!null(buff) && parsed)
	    cmdchar = toupper(buff[0]);
	if (stdio->statbits & (IDLETIMEOUT | HANGUP | CARDROP | IDLE))
	    return;
	fddi_psistart();
	switch (cmdchar) {
	case 'H':
	case '?':
	    printf("\n");
	    printf("\na - read All 6 diag registers");
	    printf("\nc - Controller command");
	    printf("\nd - Download ucode from rom");
	    printf("\ni - Interface command");
	    printf("\nl - Load and run ucode from rom");
	    printf("\nm - dump formatted sip memory");
	    printf("\np - pull info from logger (CIP)");
	    printf("\nr - Read diag registers");
	    printf("\ns - list Sram contents");
	    printf("\nt - sloT select");
	    printf("\nw - Write diag registers");
	    break;
	case 'A':
	    printf("0x00: %02x %02x %02x %02x  %02x %02x\n",
		   diag[0], diag[1], diag[2], diag[3], diag[4], diag[5]);
	    break;
	case 'B':
	    if (interface < 0) 
		interface = cbus_slotunit2unit(slot2, last_interface);
	    printf("\nabsolute interface [%d]: ", interface);
	    if (!(parsed = rdtty(buff, BUFLEN)))
		break;
	    if (!null(buff)) {
		interface = parse_unsigned(buff, &parsed);
		cbus_unit2slotunit(interface, (int *)&slot2, (int *)&last_interface);
	    }
	    if (last_interface < 4)
		str = "fsip_modA";
	    else
		str = "fsip_modB";
	    
	    cbus_address(&mlow, &mhigh, MAX_FSIP_SRAM);

	    level = raise_interrupt_level(NETS_DISABLE);
	    
	    regptr->argreg = (ushort) interface;
	    regptr->cmdreg = FCI_CMD_SELECT;
	    
	    cbus_ctrlr_select(mci, slot2);

	    regptr->largreg = mlow;
	    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	    
            /* CCB_CMD_FSIP_MEMA_ADDR */
	    regptr->argreg = FCI_CMD_CTRLR_MEMA_SELECT; 
	    regptr->cmdreg = FCI_CTRLR_CMD;

	    reset_interrupt_level(level);

	    linect = 0;
	    for (i=0; i<mhigh-mlow; i+=2,mlow+=2) {
		level = raise_interrupt_level(NETS_DISABLE);
		
		cbus_ctrlr_select(mci, slot2);

                /* CCB_CMD_MEMA_READ */
		regptr->argreg = FCI_CMD_CTRLR_MEMA_READ; 
		regptr->cmdreg = FCI_CTRLR_CMD;
	    
		response = regptr->cmdreg;
		sdata = regptr->argreg;
		
		reset_interrupt_level(level);
		
		if (response != MCI_RSP_OKAY) {
		    printf("\nbad response: %x", response);
		    break;
		}
		if ((i % 16) == 0) {
		    if (cbus_quit_test(linect++))
			break;
		    printf("\n%s+%04x:  %04x", str, mlow, sdata);
		} else if ((i % 8) == 0)
		    printf("  %04x", sdata);
		else
		    printf(" %04x", sdata);
	    }
	    printf("\n");
	    break;
	    
	case 'C':
	    printf("\ncontroller cmd [%d]: ", last_ccmd);
	    if (!(parsed = rdtty(buff, BUFLEN)))
		break;
	    if (!null(buff))
		last_ccmd = parse_unsigned(buff, &parsed);
	    
	    printf("\ncontroller %d arg [%d]: ", slot2, last_carg);
	    if (!(parsed = rdtty(buff, BUFLEN)))
		break;
	    if (!null(buff))
		last_carg = parse_unsigned(buff, &parsed);
	    
	    level = raise_interrupt_level(NETS_DISABLE);
	    
	    cbus_select(mci, cbus_slotunit2unit(slot2, last_interface));
	    cbus_ctrlr_select(mci, slot2);
	    
	    regptr->largreg = last_carg;
	    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	    
	    regptr->argreg = (ushort) last_ccmd;
	    regptr->cmdreg = FCI_CTRLR_CMD;
	    
	    DELAY(100);
	    response = regptr->cmdreg;
	    sresult = regptr->largreg;
	    reset_interrupt_level(level);
	    if (response != MCI_RSP_OKAY) {
		printf("\nbad response: %x", response);
		break;
	    }
	    printf("\nresult = %x", sresult);
	    break;
	    
	case 'D':
	    /* halt card but do not load */
	    dbus_prot_wr(slot, &dgp->diag_control, WCS_LOADED);
	    dbus_prot_wr(slot, &dgp->last_command_or_address, 0);
	    DELAY(2); /* allow for pulse stretching by dbus XILINX */

	    dbus_prot_wr(slot, &dgp->diag_status, DBUS_RESET_VALUE); /* Reset */
	    /* clear WCS_LOADED to force reload */
	    dbus_prot_wr(slot, &dgp->diag_control, 0);
	    break;
	    
	case 'I':
	    printf("\nrelative interface of slot %d [%d]: ",
		   slot2, last_interface);
	    if (!(parsed = rdtty(buff, BUFLEN)))
		break;
	    if (!null(buff))
		last_interface = parse_unsigned(buff, &parsed);

	    printf("\ninterface cmd [%d]: ", last_icmd);
	    if (!(parsed = rdtty(buff, BUFLEN)))
		break;
	    if (!null(buff))
		last_icmd = parse_unsigned(buff, &parsed);
	    
	    printf("\ninterface arg [%d]: ", last_iarg);
	    if (!(parsed = rdtty(buff, BUFLEN)))
		break;
	    if (!null(buff))
		last_iarg = parse_unsigned(buff, &parsed);
	    
	    level = raise_interrupt_level(NETS_DISABLE);
	    
	    cbus_select(mci, cbus_slotunit2unit(slot2, last_interface));
	    
	    regptr->largreg = last_iarg;
	    regptr->cmdreg = (ushort) last_icmd;

	    DELAY(100);
	    response = regptr->cmdreg;
	    sresult = regptr->largreg;
	    reset_interrupt_level(level);
	    if (response != MCI_RSP_OKAY) {
		printf("\nbad response: %x", response);
		break;
	    }
	    printf("\nresult = %x", sresult);
	    break;
	    
	case 'L':
	    /* halt card but do not load */
	    dbus_prot_wr(slot, &dgp->diag_control, WCS_LOADED);
	    /* fsip reset */
	    dbus_prot_wr(slot, &dgp->last_command_or_address, 0);
	    DELAY(2); /* allow for pulse stretching by dbus XILINX */

	    dbus_prot_wr(slot, &dgp->diag_status, DBUS_RESET_VALUE); /* Reset */
	    /* clear WCS_LOADED to force reload */
	    dbus_prot_wr(slot, &dgp->diag_control, 0);
	    /* JEFF T. says this is not needed */
	    dbus_prot_wr(slot, &dgp->diag_status, DBUS_RESET_VALUE); /* Reset */
	    DELAY(DBUS_ROM_LOAD_TIME);
	    
	    dbus_prot_wr(slot, &dgp->diag_control,
			 MASTER_ENABLE | ENABLE_LED | WCS_LOADED);
	    break;
	case 'M':
	case 'N':
	    printf("\nrelative interface of slot %d [%d]: ",
		   slot2, last_interface);
	    if (!(parsed = rdtty(buff, BUFLEN)))
	      break;
	    if (!null(buff))
	      last_interface = parse_unsigned(buff, &parsed);
	    if (cmdchar == 'M') {
		sdata = 51; /* CCB_CMD_SIP_ADDR */
		j = 0xc0;
	    } else {
		sdata = 54; /* CCB_CMD_DOHC_ADDR */
		j = 0x40;
	    }

	    level = raise_interrupt_level(NETS_DISABLE);
	    
	    cbus_select(mci, cbus_slotunit2unit(slot2, last_interface));
	    cbus_ctrlr_select(mci, slot2);

	    regptr->largreg = 0;
	    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	    
	    regptr->argreg = sdata;
	    regptr->cmdreg = FCI_CTRLR_CMD;

	    reset_interrupt_level(level);
	    
	    for (i=0; i<j; i+=2) {
		level = raise_interrupt_level(NETS_DISABLE);
		
		cbus_ctrlr_select(mci, slot2);

		regptr->argreg = i;
		regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	    
		regptr->argreg = 52; /* CCB_CMD_MEMA_READ */
		regptr->cmdreg = FCI_CTRLR_CMD;
	    
		response = regptr->cmdreg;
		sdata = regptr->argreg;
		
		reset_interrupt_level(level);
		
		if (response != MCI_RSP_OKAY) {
		    printf("\nbad response: %x", response);
		    break;
		}
		if ((i % 16) == 0)
		    printf("sip%d+%04x:  %04x", last_interface, i, sdata);
		else if ((i % 8) == 0)
		    printf("  %04x", sdata);
		else if ((i % 16) == 14)
		    printf(" %04x\n", sdata);
		else
		    printf(" %04x", sdata);
	    }
	    printf("\n");
	    break;
	case 'P':
#ifdef C7000
	    logger_drain(slot2);
	    console_drain(slot2);
#endif
	    break;
	case 'Q':
	    return;
	case 'R':
	    printf("\nregister read address [0x%0x]: ", last_reg);
	    parsed = rdtty(buff, BUFLEN);
	    if (parsed) {
		if (!null(buff))
		    last_reg = parse_unsigned(buff, &parsed);
		parsed = ((last_reg >= 0) && (last_reg <= 0x40));
	    }
	    if (!parsed)
		break;
	    printf("register 0x%02x: %02x\n", last_reg, diag[last_reg]);
	    break;
	case 'S':
	    cbus_address(&mlow, &mhigh, 2*MAX_FSIP_SRAM);
	    printf("\nPattern: ");
	    parsed = rdtty(buff, BUFLEN);
	    if (parsed) {
		if (!null(buff)) {
		    ldata = parse_unsigned(buff, &parsed);
		    fsip_type_sram(slot, dgp, mlow, mhigh, ldata, TRUE);
		} else {
		    fsip_type_sram(slot, dgp, mlow, mhigh, 0, FALSE);
		}
	    }
	    break;
	case 'T':
	    printf("\nsloT [%d]: ", slot2);
	    parsed = rdtty(buff, BUFLEN);
	    if (parsed) {
		if (!null(buff))
		    slot2 = parse_unsigned(buff, &parsed);
		parsed = (slot2 <= 7);
	    }
	    if (!parsed)
		break;
	    printf("slot=%d\n", slot2);
	    dgp = pslot_to_dbus(slot2);
	    diag = (uchar *)dgp;
	    break;
	case 'W':
	    printf("\nregister write address [0x%0x]: ", last_reg);
	    parsed = rdtty(buff, BUFLEN);
	    if (parsed) {
		if (!null(buff))
		    last_reg = parse_unsigned(buff, &parsed);
		parsed = ((last_reg >= 0) && (last_reg <= 0x40));
	    }
	    if (!parsed)
		break;

	    printf("\nregister write data [0x%0x]: ", reg_data);
	    parsed = rdtty(buff, BUFLEN);
	    if (parsed) {
		if (!null(buff))
		    reg_data = parse_unsigned(buff, &parsed) & 0xff;
	    } else
		break;
	    printf("write 0x%02x to register 0x%02x\n", reg_data, last_reg);
	    diag[last_reg] = reg_data;
	    break;
	default:
	    printf("\nunknown command \"%c\"", cmdchar);
	    cmdchar = '?';
	    break;
	}
	fddi_psiend();
    }
}

/*
 * fddi specific commands
 */

static void fddi_specific (mcitype *mci)
{
    ulong mhigh, mlow;
    char cmdchar;
    char buff[BUFLEN];
    ushort type;
    boolean parsed;
    ulong arglong;
    ulong command;
    int rcount, linect;
    mciregtype *regptr;
    ushort result;
    leveltype level;

    printf("\n\nFDDI interface commands");
    arglong = command = 0;
    cmdchar = '?';
    rcount = 1;
    while (TRUE) {
	printf("\nfddi (? for help) [%c]: ", cmdchar);
	parsed = rdtty(buff, BUFLEN);
	if (!null(buff) && parsed)
	    cmdchar = toupper(buff[0]);
	if (stdio->statbits & (IDLETIMEOUT | HANGUP | CARDROP | IDLE))
	    return;
	switch (cmdchar) {
	  case '?':
	    printf("\n");
	    printf("\na - r/w address filter");
	    printf("\nb - blast (transmit a block of buffers)");
	    printf("\nc - execute dci command");
	    printf("\nd - dump memd");
	    printf("\nf - fill address filter");
	    printf("\ng - test fddi bypass switch");
	    printf("\nh - torque fddi ctl/hold reg");
	    printf("\ni - select default interface");
            printf("\nk - memd test");
	    printf("\nl - af mem test");
	    printf("\nm - dump mema");
	    printf("\nn - dump endec registers");
	    printf("\no - read endec");
	    printf("\np - write endec");
	    printf("\nq - exit fddi specific");
	    printf("\nr - dump (read) fddi alu registers");
	    printf("\ns - set mac level addresses");
	    printf("\nt - transmit a fddi frame (test SABME)");
	    printf("\nu - dump formac registers");
	    printf("\nv - read formac reg");
	    printf("\nw - write formac reg");
	    printf("\nx - fire up the formac");
	    printf("\ny - force claim or beacon state");
	    printf("\nz - receive loop");
	    printf("\n0 - access cam via diagnostic interface");
	    printf("\n1 - dump cam");
	    printf("\n2 - try multibus read timeout");
	    printf("\n3 - try multibus write timeout");
	    printf("\n\n'c' rules of radix type-in and display apply.\n");
	    break;
	  case 'A':
	    af_rw_cmd(mci);
	    break;
	  case 'B':
	    fddi_blast(mci);
	    break;
	  case 'C':
	    dci_command(mci, &arglong, &command, &rcount);
	    break;
	  case 'D':
	    cbus_address(&mlow, &mhigh, MaxMemD);
	    cbus_type_memd(mci, mlow, mhigh);
	    break;
	  case 'F':
	    fddi_fill_af(mci);
	    break;
	  case 'G':
	    cbus_select(mci, default_interface);
	    fddi_bypass_switch(mci);
	    break;
	  case 'H':
	    fddi_hold_reg(mci);
	    break;
	  case 'I':
	    default_interface++;
	    if (default_interface >= mci->max_interfaces)
		default_interface = 0;
	    type = cbus_select(mci, default_interface);
	    printf("\nselecting interface %d, %s", default_interface,
		   cbus_devicename(type));
	    if (type != FCI_TYPE_FDDI)
		return;
	    break;
	  case 'K':
	    fddi_psistart();
	    fddi_memdtest(mci);
	    fddi_psiend();
	    break;
	case 'L':
	    linect = 0;
	    printf("\nRunning af test until ctrl-^ typed...");
	    while (!psipending()) {
		if (!fddi_af_test(mci, &linect))
		    break;
	    }
	    break;
	  case 'M':
	    cbus_address(&mlow, &mhigh, 0xffff);
	    cbus_daughter_mema(mci, mlow, mhigh);
	    break;
	  case 'N':
	    fddi_dump_endec(mci);
	    break;
	  case 'O':
	    fddi_endec_read_reg(mci);
	    break;
	  case 'P':
	    fddi_endec_write_reg(mci);
	    break;
	  case 'Q':
	    return;
	  case 'R':
	    fddi_alu_dump_registers(mci);
	    break;
	  case 'S':
	    fddi_set_address();
	    break;
	  case 'T':
	    fddi_transmit_sabme(mci);
	    break;
#ifndef notdef
	    fddi_psistart();
	    cbus_select(mci, default_interface);
	    fddi_loop_transmit(mci, default_interface);
	    fddi_psiend();
	    break;	    
#endif
	  case 'U':
	    fddi_dump_formac(mci);
	    break;
	  case 'V':
	    fddi_read_formac_reg(mci);
	    break;
	  case 'W':
	    fddi_write_formac_reg(mci);
	    break;
	  case 'X':
	    fddi_enable_fddi(mci);
	    break;
	  case 'Y':
	    fddi_force_claim_beacon(mci);
	    break;
	  case 'Z':
	    fddi_psistart();
	    cbus_select(mci, default_interface);
	    fddi_loop_receive(mci);
	    fddi_psiend();
	    break;
	  case '0':
	    fddi_cam_access(mci);
	    break;
	  case '1':
	    fddi_cam_dump(mci);
	    break;
	  case '2':
	  case '3':
	    level = raise_interrupt_level(NETS_DISABLE);
	    regptr = mci->regptr;
	    regptr->argreg = 0x8000;	/* Invalidates the entire cache */
	    regptr->cmdreg = FCI_CACHE_INVALIDATE;
	    if (cmdchar == '2')
		result = regptr->cmdreg;
	    else if (cmdchar == '3')
		regptr->argreg = 0;
	    reset_interrupt_level(level);
	    break;
	  default:
	    printf("\nunknown command \"%c\"", cmdchar);
	    cmdchar = '?';
	    break;
	}
    }
}

/*
 * cbus_find_dci
 * select the current interface and call its device proc
 */

static void cbus_find_dci (mcitype *mci)
{
    ushort type;

    type = cbus_select(mci, default_interface);
    if ((mci->hw_version == CBUS_HARDWARE) ||
	(mci->hw_version == CBUSII_HARDWARE) ||
	reg_invoke_switch_proc_present(pslot_to_dbus(mci->unit))) {
	switch (FCITYPE(type)) {
	  case FCI_TYPE_FDDI:
	  case FCI_TYPE_FDDIT:
	    fddi_specific(mci);
	    break;
	  case FCI_TYPE_ETHER:
	    ether_specific(mci);
	    break;
	  case FCI_TYPE_CTR:
	    ctr_specific(mci);
	    break;
	  case FCI_TYPE_SERIAL:
	    fsip_specific(mci);
	    break;
	  case FCI_TYPE_ATM:
	    atm_specific(mci);
	    break;
	  default:
	    printf("\nInterface %d: No dci specific proc installed for type %d, %s",
		   default_interface, type, cbus_devicename(type));
	    break;
	}
    } else if (mci->hw_version == MCI_HARDWARE) {
	if ((default_interface & 1) == 0) {
	    ether_specific(mci);
	} else {
	    printf("\nInterface %d: No interface specific proc installed for type %d, %s",
		   default_interface, type, cbus_devicename(type));
	}
    } else {
	printf("\nInterface %d: No interface specific proc installed for type %d, %s",
	       default_interface, type, cbus_devicename(type));
    }
}


static int bce_create (mciregtype *regptr,
		       ushort da01,
		       ushort da23,
		       ushort da45,
		       ushort ifa,
		       ushort flags,
		       ulong in_permit,
		       ulong out_permit)
{
    int response, bce;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    regptr->argreg = FCI_AB_HASH_TABLE;
    regptr->cmdreg = FCI_SELECT_HASH_TABLE;
    response = regptr->cmdreg;
    if ( response != MCI_RSP_OKAY) {
	printf("\nbad response: %x from FCI_SELECT_HASH_TABLE", response);
	reset_interrupt_level(level);	
	return(0);
    }
    regptr->argreg = BCE_LENGTH;
    regptr->cmdreg = FCI_CACHE_ALLOC_DYNAMIC;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
	printf("\nbad response: %x from FCI_CACHE_ALLOC_DYNAMIC", response);
	reset_interrupt_level(level);	
	return(0);
    }
    if (!(bce = regptr->argreg)){
	reset_interrupt_level(level);	
	return(0);
    }

    regptr->rw_cache = da45;
    regptr->rw_cache = da23;
    regptr->rw_cache = da01;
    regptr->rw_cache = ifa;
    regptr->rw_cache = flags;
    regptr->rw_cache = (ushort)((in_permit >> 16) & 0xffff);
    regptr->rw_cache = (ushort)(in_permit & 0xffff);
    regptr->rw_cache = (ushort)((out_permit >> 16) & 0xffff);
    regptr->rw_cache = (ushort)(out_permit & 0xffff);
    regptr->rw_cache = 0; /* rx_count_low*/
    regptr->rw_cache = 0; /* rx_count_high*/
    regptr->rw_cache = 0; /* tx_count_low*/
    regptr->rw_cache = 0; /* tx _count_high*/

    regptr->cmdreg = FCI_CACHE_INSERT;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) 
	printf("\nbad response: %x from FCI_CACHE_INSERT", response);
    reset_interrupt_level(level);	
    
    return(bce);
}
static ushort bce_read (mciregtype *regptr,
			ushort bce,
			ushort field)
{
    leveltype level;
    int response;
    ushort result;

    level = raise_interrupt_level(NETS_DISABLE);

    regptr->argreg = bce;
    regptr->cmdreg = FCI_CACHE_SELECT_ABS;
    response = regptr->cmdreg;
    if ( response != MCI_RSP_OKAY) {
	printf("\nbad response: %x from FCI_CACHE_SELECT_ABS", response);
	reset_interrupt_level(level);	
	return(0);
    }
    
    regptr->argreg = field;
    regptr->cmdreg = FCI_CACHE_SELECT;
    response = regptr->cmdreg;
    if ( response != MCI_RSP_OKAY) {
	printf("\nbad response: %x from FCI_CACHE_SELECT", response);
	reset_interrupt_level(level);	
	return(0);
    }

    result = regptr->rw_cache;

    /*
     * Our friends IP and IPX expect the cache pointer to be zero,
     * so we do them the courtesy without checking the return status.
     */
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CACHE_SELECT_ABS;

    reset_interrupt_level(level);	
    return(result);
}
static ushort bce_write (mciregtype *regptr,
			 ushort bce,
			 ushort field,
			 ushort data)
{
    leveltype level;
    int response;

    level = raise_interrupt_level(NETS_DISABLE);

    regptr->argreg = bce;
    regptr->cmdreg = FCI_CACHE_SELECT_ABS;
    response = regptr->cmdreg;
    if ( response != MCI_RSP_OKAY) {
	printf("\nbad response: %x from FCI_CACHE_SELECT_ABS", response);
	reset_interrupt_level(level);	
	return(0);
    }
    
    regptr->argreg = field;
    regptr->cmdreg = FCI_CACHE_SELECT;
    response = regptr->cmdreg;
    if ( response != MCI_RSP_OKAY) {
	printf("\nbad response: %x from FCI_CACHE_SELECT", response);
	reset_interrupt_level(level);	
	return(0);
    }

    regptr->rw_cache = data;

    /*
     * Our friends IP and IPX expect the cache pointer to be zero,
     * so we do them the courtesy without checking the return status.
     */
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CACHE_SELECT_ABS;

    reset_interrupt_level(level);	
    return(1);
}

static ushort bce_free (mciregtype *regptr,
			ushort bce,
			ushort size)
{
    leveltype level;
    int response;

    level = raise_interrupt_level(NETS_DISABLE);

    regptr->argreg = FCI_AB_HASH_TABLE;
    regptr->cmdreg = FCI_SELECT_HASH_TABLE;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
	printf("\nbad response: %x from FCI_SELECT_HASH_TABLE", response);
	reset_interrupt_level(level);	
	return(0);
    }

    regptr->largreg = (size << 16)| bce;
    regptr->cmdreg = FCI_CACHE_FREE_DYNAMIC;
    response = regptr->cmdreg;
    reset_interrupt_level(level);	
    if (response != MCI_RSP_OKAY) {
	printf("\nbad response: %x from FCI_HASH_FREE_DYNAMIC", response);
	return(0);
    }
    return(1);
}

/*
 * ab_command
 *
 * Special autonomous bridging commands
 */
static void ab_commands (mciregtype *regptr)
{

#ifndef C7000
#define AB_HASH_BASE   0x0D00
#else
#define AB_HASH_BASE   0x1400
#endif

    boolean parsed;
    char cmdchar = '?';
    char buff[BUFLEN];
    leveltype level;
    ushort da01=0;
    ushort da23=0x0c01;
    ushort da45=0;

    ushort bce = 0;
    ushort ifa = 1;
    ushort flags = 0;
    ulong in_permit = 0;
    ulong out_permit = 0;
    boolean entry_found;
    int indx, linect, depth;
    ushort uslink, usentry;
    int iterations = 1000;
    int maxentries = 5000;
    int entries;
    ushort rxcount_low, rxcount_high, txcount_low, txcount_high;
    int response;

    level = raise_interrupt_level(NETS_DISABLE);

    regptr->argreg = 0;
    regptr->cmdreg = FCI_AS_CAPABILITY;
    reset_interrupt_level(level);	
    if(!(regptr->argreg & FCI_CAP_BRIDGE_AUTO)){
	printf("\nAutonomous bridging not supported in microcode");
	return;
    }

    while (TRUE) {
	printf("\n%s (? for help) [%c]: ", "auto_bridge", cmdchar);
	parsed = rdtty(buff, BUFLEN);
	if (!null(buff) && parsed)
	    cmdchar = toupper(buff[0]);
	if (stdio->statbits & (IDLETIMEOUT | HANGUP | CARDROP | IDLE))
	    return;
	fddi_psistart();
	switch (cmdchar) {
	  case '?':
	    printf("\n");
	    printf("\na - Annihilate cache");
	    printf("\nc - Create/Insert cache entry");
	    printf("\nd - Delete cache entry");
	    printf("\nf - Bridge group flags");
	    printf("\nh - Bridge group head");
	    printf("\nl - Loop creating/deleting cache entries");
	    printf("\nm - Modify cache entry");
	    printf("\nn - Bridge group next");
	    printf("\np - Print cache table");
	    printf("\nq - Return to higher level");
	    printf("\nt - Clear BCE counters");
	    printf("\nx - Auto flood");
	    break;

	  case 'A':   /* Annihilate the cache */
	    level = raise_interrupt_level(NETS_DISABLE);

	    regptr->argreg = 0x8000;
	    regptr->cmdreg = FCI_CACHE_INVALIDATE;

	    reset_interrupt_level(level);	
	    printf("\ncaches annihilated");
	    break;
	    
	  case 'C':    /* Create a BCE */
	    printf("\nMAC address [%04x.%04x.%04x]: ",da01,da23,da45);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed){
		da01 = (ushort) parse_hex_long(buff);
		da23 = (ushort) parse_hex_long(&buff[5]);
		da45 = (ushort) parse_hex_long(&buff[10]);
	    }

	    printf("\nIFA [0x%04x]: ",ifa);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed)
		ifa = (ushort) parse_unsigned(buff, &parsed);

	    printf("\nBCE_ACTION [0x%04x]: ",flags);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed)
		flags = (ushort) parse_unsigned(buff, &parsed);

	    printf("\nInput Permit [0x%08x]: ",in_permit);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed)
		in_permit = parse_unsigned(buff, &parsed);

	    printf("\nOutput Permit [0x%08x]: ",out_permit);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed)
		out_permit =  parse_unsigned(buff, &parsed);

/*
*	    printf("\n   Mac Addr        IFA     Flgs      Permit");
*	    printf("\n%04x.%04x.%04x : 0x%04x : 0x%04x : 0x%04x%04x", 
*                   da01,da23,da45,ifa,flags,permit_low,permit_high);
*/
	    bce = bce_create(regptr,da01,da23,da45,ifa,flags,
			     in_permit, out_permit);
	    if (bce) {
		printf("\nallocated bce = 0x%x", bce);
	    } else {
		return;
	    }
	    break;

	  case 'D':          /* Delete a cache entry */
	    printf("\nfree bce [0x%04x]: ",bce);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed)
		bce = (ushort) parse_unsigned(buff, &parsed);

	    if (!bce_free(regptr, bce, BCE_LENGTH))
		return;

/*	    printf("\nbce 0x%04x freed",bce); */
	    break;

	  case 'F':          /* Bridge group flags */
	    printf("\nIFA [0x%04x]: ",ifa);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed){
		ifa = (ushort) parse_unsigned(buff, &parsed);
	    }

	    printf("\nFlags [0x%04x]: ",flags);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed){
		flags = (ushort) parse_unsigned(buff, &parsed);
	    }

	    level = raise_interrupt_level(NETS_DISABLE);

	    regptr->argreg = ifa;
	    regptr->cmdreg = FCI_CMD_SELECT;
	    response = regptr->cmdreg;
	    if (response != MCI_RSP_OKAY) {
		printf("\nbad response: %x from FCI_CMD_SELECT", response);
		reset_interrupt_level(level);
		return;
	    }

	    regptr->argreg = flags;
	    regptr->cmdreg = FCI_BRIDGE_GROUP_FLAGS;
	    response = regptr->cmdreg;
	    if (response != MCI_RSP_OKAY) {
		printf("\nbad response: %x from FCI_BRIDGE_GROUP_FLAGS",
		       response);
		reset_interrupt_level(level);
		return;
	    }

	    reset_interrupt_level(level);
	    break;

	  case 'H':          /* Bridge group head */
	    printf("\nIFA [0x%04x]: ",ifa);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed){
		ifa = (ushort) parse_unsigned(buff, &parsed);
	    }

	    printf("\nHead (MEMD icb addr)[0x%04x]: ",flags);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed){
		flags = (ushort) parse_unsigned(buff, &parsed);
	    }

	    level = raise_interrupt_level(NETS_DISABLE);

	    regptr->argreg = ifa;
	    regptr->cmdreg = FCI_CMD_SELECT;
	    response = regptr->cmdreg;
	    if (response != MCI_RSP_OKAY) {
		printf("\nbad response: %x from FCI_CMD_SELECT", response);
		reset_interrupt_level(level);
		return;
	    }

	    regptr->argreg = flags;
	    regptr->cmdreg = FCI_BRIDGE_GROUP_NUMBER;
	    response = regptr->cmdreg;
	    if (response != MCI_RSP_OKAY) {
		printf("\nbad response: %x from FCI_BRIDGE_GROUP_FLAGS",
		       response);
		reset_interrupt_level(level);
		return;
	    }

	    reset_interrupt_level(level);

	    break;
	  case 'L':    /* iterative Loop */
	    
	    printf("\nNumber of iterations [%d]: ",iterations);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed)
		iterations = (ushort) parse_unsigned(buff, &parsed);

	    printf("\nNumber of entries [%d]: ",maxentries);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed)
		maxentries = (ushort) parse_unsigned(buff, &parsed);

	    da01 = 0x1cd0;
	    da23 = 0xa5a5;
	    ifa = 1;
	    psion(stdio);
	    printf("%s\n", type_escape_seq_abort_msg);
	    flush();
	    for (indx = 0; (indx < iterations) && (psipending() == FALSE); 
		 indx++) {
		for (entries = 0;(entries < maxentries) &&
		     (psipending() == FALSE);
		     entries++){

		    if (!(bct[entries] = bce_create (regptr,da01,da23,entries,
                                           ifa,flags,in_permit,out_permit))){
			printf("\nCreate error at entry = 0x%x",entries);
			return;
		    }
		}
		for (entries = 0; entries < maxentries;  entries++){
		    if (!bce_free(regptr, bct[entries], BCE_LENGTH)){
			printf("\nFree error at entry = %x",entries);
			return;
		    }
		}
		printf(".");
		flush();
		process_may_suspend();
	    }
	    psioff(stdio);
	    break;

	  case 'M':   /* Modify fields in the BCE */
	    printf("\nBCE [0x%04x]: ",bce);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed)
		bce = (ushort) parse_unsigned(buff, &parsed);

	    ifa = bce_read(regptr, bce, BCE_IFA_OFFSET);
	    printf("\nIFA [0x%04x]: ",ifa);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed){
		ifa = (ushort) parse_unsigned(buff, &parsed);
		bce_write(regptr, bce, BCE_IFA_OFFSET, ifa);
	    }

	    ifa = bce_read(regptr, bce, BCE_ACTION_OFFSET);
	    printf("\nFlags [0x%04x]: ",flags);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed){
		flags = (ushort) parse_unsigned(buff, &parsed);
		bce_write(regptr, bce, BCE_ACTION_OFFSET, flags);
	    }

	    flags = bce_read(regptr, bce, BCE_PERMIT_IN_OFFSET);
	    in_permit = bce_read(regptr, bce, BCE_PERMIT_IN_OFFSET+1);
	    in_permit |= (flags << 16);
	    printf("\nInput Permit [0x%08X]: ",in_permit);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed){
		in_permit = parse_unsigned(buff, &parsed);
		flags = (ushort)(in_permit>>16) & 0xffff;
		bce_write(regptr, bce, BCE_PERMIT_IN_OFFSET, flags);
		flags = (ushort)(in_permit & 0xffff);
		bce_write(regptr, bce, BCE_PERMIT_IN_OFFSET+1, flags);
	    }

	    flags = bce_read(regptr, bce, BCE_PERMIT_OUT_OFFSET);
	    out_permit = bce_read(regptr, bce, BCE_PERMIT_OUT_OFFSET+1);
	    out_permit |= (flags << 16);
	    printf("\nOutput Permit [0x%08X]: ",out_permit);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed){
		out_permit = parse_unsigned(buff, &parsed);
		flags = (ushort)(out_permit>>16) & 0xffff;
		bce_write(regptr, bce, BCE_PERMIT_OUT_OFFSET, flags);
		flags = (ushort)(out_permit & 0xffff);
		bce_write(regptr, bce, BCE_PERMIT_IN_OFFSET+1, flags);

	    }

	    break;

	  case 'N':          /* Bridge group next */
	    printf("\nIFA [0x%04x]: ",ifa);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed){
		ifa = (ushort) parse_unsigned(buff, &parsed);
	    }

	    printf("\nNext (MEMD icb addr)[0x%04x]: ",flags);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed){
		flags = (ushort) parse_unsigned(buff, &parsed);
	    }

	    level = raise_interrupt_level(NETS_DISABLE);

	    regptr->argreg = ifa;
	    regptr->cmdreg = FCI_CMD_SELECT;
	    response = regptr->cmdreg;
	    if (response != MCI_RSP_OKAY) {
		printf("\nbad response: %x from FCI_CMD_SELECT", response);
		reset_interrupt_level(level);
		return;
	    }

	    regptr->argreg = flags;
	    regptr->cmdreg = FCI_BRIDGE_GROUP_NEXT;
	    response = regptr->cmdreg;
	    if (response != MCI_RSP_OKAY) {
		printf("\nbad response: %x from FCI_BRIDGE_GROUP_FLAGS",
		       response);
		reset_interrupt_level(level);
		return;
	    }

	    reset_interrupt_level(level);

	    break;

	  case 'P':   /* Print the hash table */
    
	    linect = 0;
	    entry_found = FALSE;

	    /*
	     * Scan the hash table
	     */
	    printf("\n# lnk  addr  Mac Address   IFA  Flgs in_permit  out_prmt rxcount  txcount");

	    psion(stdio);

	    for (indx = 0; indx < NETHASHLEN; indx++) {
		level = raise_interrupt_level(NETS_DISABLE);
		regptr->argreg = AB_HASH_BASE + indx;
		regptr->cmdreg = FCI_CMD_MEMA_SELECT;
		regptr->cmdreg = FCI_CMD_MEMA_READ;
		uslink = regptr->argreg;
		reset_interrupt_level(level);
		depth = 0;
		while (uslink && (uslink != 0xffff)) {
		    entry_found = TRUE;
		    if (cbus_quit_test(linect++))
		      break;
		    usentry = uslink;
		    level = raise_interrupt_level(NETS_DISABLE);
	    
		    /*
		     * Get link to next entry.
		     */
		    regptr->argreg = usentry;
		    regptr->cmdreg = FCI_CMD_MEMA_SELECT;
		    regptr->cmdreg = FCI_CMD_MEMA_READ;
		    uslink = regptr->argreg;	/* link to next entry */
		    reset_interrupt_level(level);
		    printf("\n%-02x/%3d %-04x ", indx, depth, usentry);

		    /*
		     * Get entry fields 
		    */
		    level = raise_interrupt_level(NETS_DISABLE);

		    regptr->argreg = usentry + 1;
		    regptr->cmdreg = FCI_CMD_MEMA_SELECT;

		    regptr->cmdreg = FCI_CMD_MEMA_READ;
		    da45 = regptr->argreg;

		    regptr->cmdreg = FCI_CMD_MEMA_READ;
		    da23 = regptr->argreg;

		    regptr->cmdreg = FCI_CMD_MEMA_READ;
		    da01 = regptr->argreg;

		    regptr->cmdreg = FCI_CMD_MEMA_READ;
		    ifa = regptr->argreg;

		    regptr->cmdreg = FCI_CMD_MEMA_READ;
		    flags = regptr->argreg;

		    regptr->cmdreg = FCI_CMD_MEMA_READ;
		    rxcount_high = regptr->argreg;
		    
		    regptr->cmdreg = FCI_CMD_MEMA_READ;
		    rxcount_low = regptr->argreg;
		    
		    regptr->cmdreg = FCI_CMD_MEMA_READ;
		    txcount_high = regptr->argreg;
		    
		    regptr->cmdreg = FCI_CMD_MEMA_READ;
		    txcount_low = regptr->argreg;
		    
		    regptr->cmdreg = FCI_CMD_MEMA_READ;
		    entries= regptr->argreg;
		    
		    regptr->cmdreg = FCI_CMD_MEMA_READ;
		    in_permit = regptr->argreg;
		    in_permit |= entries << 16;

		    regptr->cmdreg = FCI_CMD_MEMA_READ;
		    entries= regptr->argreg;
		    
		    regptr->cmdreg = FCI_CMD_MEMA_READ;
		    out_permit = regptr->argreg;
		    out_permit |= entries << 16;

		    reset_interrupt_level(level);
		    printf("%04x.%04x.%04x %04x %04x %04x%04x %04x%04x %08X %08X", 
			   da01,da23,da45,ifa,flags,rxcount_high,rxcount_low,
			   txcount_high,txcount_low, in_permit, out_permit);

		    depth++;
		}
	    }
	    if (!entry_found)
	      printf("\nCache empty");

	    psioff(stdio);
	    break;

	  case 'Q':
	    return;

	  case 'T':     /* Clear BCE counters */
	    printf("\nBCE [0x%04x]: ",bce);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed)
		bce = (ushort) parse_unsigned(buff, &parsed);

	    level = raise_interrupt_level(NETS_DISABLE);

	    regptr->argreg = bce+BCE_RX_OFFSET;
	    regptr->cmdreg = FCI_MEMA_READ_CLEAR_LONG;
	    printf("\nrxcount = %X",regptr->largreg);

	    regptr->argreg = bce+BCE_TX_OFFSET;
	    regptr->cmdreg = FCI_MEMA_READ_CLEAR_LONG;
	    printf("   txcount = %X",regptr->largreg);

	    reset_interrupt_level(level);

	    break;

	  default:
	    printf("\nunknown command \"%c\"", cmdchar);
	    cmdchar = '?';
	    break;
	}
	fddi_psiend();
    }
}


/*
 * cbus_mci_command
 * mci diagnostic console
 */

void cbus_mci_command (parseinfo *csb)
{
    mcitype *mci;
    int flag;
    ulong ahigh, alow;
    ulong mhigh, mlow;
    ulong afhigh, aflow;
    ushort type;
    int meclow, mechigh;
    int laddr;
    int argument, command;
    ulong arglong;
    char cmdchar, *title;
    char buff[BUFLEN];
    boolean parsed, lmem;
    int rcount;
    leveltype level;
    
    printf("mci diagnostic console program\n");
    mci = (mcitype *) cbus_setup_unit();
    if (mci == NULL)
	return;
    MaxMemD = (mci->flags & CBUSII) ? 0x3ffff : MEMD_WORDS;
    alow = mlow = aflow = laddr = meclow = 0;
#ifdef C7000
    mlow = 0x20;		/* skip local locks! */
#endif
    ahigh = MEMA_WORDS;
    mhigh = MaxMemD;
    afhigh = ((32 * 256) - 1);
    mechigh = MEC_MEMA_WORDS;
    cmdchar = '?';
    argument = 0;
    arglong = 0;
    command = 0;
    rcount = 1;
    flag = 0;
    fddi_noint = lmem = FALSE;
    if ((mci->hw_version == CBUSII_HARDWARE) ||
	reg_invoke_switch_proc_present(pslot_to_dbus(mci->unit)))
	title = "cbusii";
    else if (mci->hw_version == CBUS_HARDWARE)
	title = "cbus";
    else if (mci->hw_version == MCI_HARDWARE)
	title = "mci";
    else
	title = "sci";
    
    while (TRUE) {
	printf("\n%s (? for help) [%c]: ", title, cmdchar);
	parsed = rdtty(buff, BUFLEN);
	if (!null(buff) && parsed)
	    cmdchar = toupper(buff[0]);
	if (stdio->statbits & (IDLETIMEOUT | HANGUP | CARDROP | IDLE))
	    return;
	fddi_psistart();
	switch (cmdchar) {
	case 'H':
	case '?':
	    printf("\n");
	    printf("\na - list mema contents");
	    printf("\nb - do basic tests");
	    printf("\nc - execute command code");
	    printf("\nd - list memd contents");
	    printf("\ne - list af mem contents");
	    printf("\nf - follow linked lists");
	    printf("\ng - cache memory");
	    printf("\ni - select default interface");
	    printf("\nj - interactive linked lists");
	    printf("\nk - memd test");
            printf("\nl - autonomous bridging commands");
	    printf("\nm - list daughter mema contents");
	    printf("\nn - low core dump utilities");
	    printf("\no - step on cbus");
            printf("\np - fsiP special commands");
	    printf("\nq - exit mci-dcp");
	    printf("\nr - page");
	    printf("\ns - search memory for pattern");
	    printf("\nt - carve up memory");
	    printf("\nx - special diagnostic commands");
	    printf("\nu - sse debugger");
	    printf("\nv - set verbosity level");
	    printf("\ny - step on interface");
	    printf("\nz - toggle zebra");
	    printf("\n+ - interface specific command");
	    printf("\n- - daughter card command");
	    printf("\n\n'c' rules of radix type-in and display apply.\n");
	    break;
	case 'A':
	    cbus_address(&alow, &ahigh, MaxMemD);
	    cbus_type_mema(mci, alow, ahigh);
	    break;
	case 'B':
	    fddi_noint = TRUE;
	    fddi_keepalive_adjust();
	    cbus_basic_tests(mci);
	    fddi_noint = FALSE;
	    set_interrupt_level(ALL_ENABLE);
	    break;
	case 'C':
	    cbus_execute_command(mci, &arglong, &command, &rcount);
	    break;
	case 'D':
	    cbus_address(&mlow, &mhigh, MaxMemD);
	    cbus_type_memd(mci, mlow, mhigh);
	    break;
	case 'E':
	    cbus_address(&aflow, &afhigh, 0x1fff);
	    cbus_type_afmem(mci, aflow, afhigh);
	    break;
	case 'F':
	    cbus_follow_lists(mci);
	    break;
	case 'G':
	    cbus_cache_lists(mci);
	    break;
	case 'I':
	    default_interface++;
	    if (default_interface >= mci->max_interfaces)
		default_interface = 0;
	    type = cbus_select(mci, default_interface);
	    printf("\nselecting interface %d, %s", default_interface,
		   cbus_devicename(type));
	    break;
	case 'J':
	    cbus_interactive_list(mci, &laddr, &lmem);
	    cbus_list(mci, laddr, "entered list", lmem);
	    break;
	case 'K':
	    fddi_psistart();
	    fddi_memdtest(mci);
	    fddi_psiend();
	    break;
        case 'L':
            ab_commands(mci->regptr);
            break;
	case 'M':
	    cbus_address(&mlow, &mhigh, 0xffff);
	    cbus_daughter_mema(mci, mlow, mhigh);
	    break;
	case 'N':
	    lowcore_command(mci->regptr);
	    break;
	case 'O':
	    mci = (mcitype *) cbusQ.qhead;
	    level = raise_interrupt_level(NETS_DISABLE);
	    cbus_init(mci, FALSE);
	    reset_interrupt_level(level);
	    break;
        case 'P':
            fsip_specific(mci);
            break;
	case 'Q':
	    return;
#ifdef notdef
	case 'R':
	    cbus_fci_page(mci);
            break;
#endif
	case 'S':
	    cbus_search(mci);
	    break;	    
	case 'T':
	    automore_enable(NULL);
    	    cbus_create_memory(mci);
	    automore_disable();
	    break;
	case 'X':
	    cbus_execute_special(mci, toupper(buff[1]), &buff[2]);
	    break;
#ifdef C7000
	case 'U':
	    sse_debugger();
	    break;
#endif
	case 'V':
	    printf("Verbose is currently %s.  Reporting is currently %s.\n",
		   fddi_verbose ? "ON" : "OFF", fddi_reporting ? "ON" : "OFF");
	    printf("Turn verbose %s? [n]", fddi_verbose ? "OFF" : "ON");
	    if (rdtty(buff, BUFLEN) && (toupper(buff[0]) == 'Y'))
		fddi_verbose = !fddi_verbose;
	    printf("Turn reporting %s? [n]", fddi_reporting ? "OFF" : "ON");
	    if (rdtty(buff, BUFLEN) && (toupper(buff[0]) == 'Y'))
		fddi_reporting = !fddi_reporting;
	    break;
	case 'Y':
	    cbus_reset_daughter(mci, default_interface);
	    break;
	case 'Z':
	    zebra_debug = (zebra_debug ? 0 : 1);
	    printf("zebra is now %s.\n", (zebra_debug ? "on" : "off"));
	    break;
	case '+':
	    cbus_find_dci(mci);
	    break;
	case '-':
	    cbus_daughter_command(mci, &arglong, &command, &rcount);
	    break;
	default:
	    printf("\nunknown command \"%c\"", cmdchar);
	    cmdchar = '?';
	    break;
	}
	fddi_psiend();
    }
}

#ifndef C7000
dgtype* pslot_to_dbus(int slot)
{
    return(NULL);		/* no dbus on AGS+ */
}
void dbus_prot_wr (uint slot, protected_uchar *reg, uchar value)
{
                 		/* no dbus on AGS+ */
}
#endif


/*
 *
 */
static void cbus_test_init (subsystype *subsys)
{
    cbus_parser_test_init();
}


/*
 * cBus test subsystem header
 */
#define	CBUSTST_MAJVERSION	1
#define	CBUSTST_MINVERSION	0
#define	CBUSTST_EDITVERSION	1

SUBSYS_HEADER(cbus_test, CBUSTST_MAJVERSION,
	      CBUSTST_MINVERSION, CBUSTST_EDITVERSION,
	      cbus_test_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);
