/* $Id: trip_test.c,v 3.3.58.4 1996/08/26 15:07:35 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/hes/trip_test.c,v $
 *------------------------------------------------------------------
 * trip_test.c -- CTR specific diagnostic routines
 *
 * December 1991 - David R. Hampton
 * 
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: trip_test.c,v $
 * Revision 3.3.58.4  1996/08/26  15:07:35  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.3.58.3  1996/07/09  05:51:35  ppearce
 * CSCdi59527:  inlines wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.3.58.2  1996/05/17  11:19:38  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.46.2  1996/04/26  15:19:09  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.46.1  1996/04/03  14:32:45  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.58.1  1996/03/18  19:44:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.24.3  1996/03/14  00:56:51  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.3.24.2  1996/03/07  09:41:00  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.24.1  1996/01/24  21:48:33  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  09:21:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:29:20  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:43:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:45:45  hampton
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
#include "ttysrv.h"
#include "if_mci.h"
#include "if_fci.h"
#include "if_cmd.h"
#include "../if/tring.h"
#include "if_trip.h"
#include "cbus_test.h"
#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.h"

#define BUFLEN 80

static uchar testda[6] = {0x00, 0x00, 0x30, 0x99, 0x99, 0x99};
static uchar testsa[6] = {0x00, 0x00, 0x30, 0x11, 0x11, 0x11};
static int ctr_delay = 100;

extern boolean fddi_verbose;
extern boolean fddi_reporting;
extern char *fddi_escape;



/*
 * 
 */
static void ctr_af_rw_cmd (mcitype *mci)
{
    char buff[BUFLEN], *p;
    uchar values[16];
    int i, num_valid;
    ushort addr, value, repeat, max_addr, count, sresult;
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
    }
    if (!parsed)
	return;
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
    regptr->argreg = addr;
    regptr->cmdreg = FCI_CMD_AF_SELECT;
    num_valid = 0;
    for (count = 0; count < repeat; count++) {
	if (stdio->automore_flush) {
	    reset_interrupt_level(level);
	    return;
	}
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
		regptr->argreg = addr;
		regptr->cmdreg = FCI_CMD_AF_SELECT;
	    }
	    regptr->cmdreg = FCI_CMD_AF_READ;
	    sresult = regptr->cmdreg;
	    for (i = 0; i < ctr_delay; i++) {
		sresult = regptr->cmdreg;
		if ((sresult == FCI_RSP_OKAY) || (sresult >= FCI_RSP_ERR_UNK))
		    break;
	    }
	    if (sresult != FCI_RSP_OKAY) {
		printf("\nmci: command %04x failed, result %04x.",
		       FCI_CMD_AF_READ, sresult);
		return;
	    }
	    if (i != 0)
		printf("\nHad to read cmdreg %d times", i+1);
	    value = regptr->argreg;
	    values[count % 16] = value & 0xFF;
	    num_valid++;
	} else {
	    regptr->argreg = value;
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

/*
 * ctr_af_fill (mci)
 * fill the address filter with a value
 */

static void ctr_af_fill (mcitype *mci)
{
    mciregtype *regptr;
    ushort val, indx, max;
    leveltype level;
    boolean ok;
    char buff[BUFLEN];

    regptr = (mciregtype *) mci->regptr;
    cbus_select(mci, default_interface);
    printf("\n  fill AF with: ");
    ok = rdtty(buff, BUFLEN);
    if (!ok || null(buff))
	return;
    val = parse_unsigned(buff, &ok);
    if (!ok)
	return;
    level = raise_interrupt_level(NETS_DISABLE);
    cbus_select(mci, default_interface);
    regptr->cmdreg = FCI_CMD_AF_SIZE;
    max = regptr->argreg;
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_AF_SELECT;
    for (indx = 0; indx < max; indx++) {
	regptr->argreg = val;
	regptr->cmdreg = FCI_CMD_AF_WRITE;
    }
    reset_interrupt_level(level);
}

/*
 * fddi_af_test
 * run tests on the address filter.
 * returns FALSE if user quit out of error display
 */

static boolean ctr_af_test (
    mcitype *mci)
{
    mciregtype *regptr;
    uint i;
    ushort sresult, max_addr, j;
    boolean errors;
    
    regptr = mci->regptr;
    regptr->cmdreg = FCI_CMD_AF_SIZE;
    sresult = regptr->cmdreg;
    for (j = 0; j < ctr_delay; j++) {
	sresult = regptr->cmdreg;
	if ((sresult == FCI_RSP_OKAY) || (sresult >= FCI_RSP_ERR_UNK))
	    break;
    }
    if (sresult != FCI_RSP_OKAY) {
	printf("\nmci: command %04x failed, result %04x.", FCI_CMD_AF_SIZE,
	       sresult);
	if (stdio->automore_flush)
	    return(FALSE);
	return(FALSE);
    }
    if (j != 0)
	printf("\nHad to read cmdreg %d times", j+1);
    max_addr = regptr->argreg;

    /*
     * af - all zeros
     */
    if (fddi_verbose)
	printf("\nmci: writing address filter with all zeros...");
    if (stdio->automore_flush)
	return(FALSE);
    errors = FALSE;
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_AF_SELECT;
    for (i = 0; i < max_addr; i++) {
        if (psipending())
            return(FALSE);
	regptr->argreg = 0;
	regptr->cmdreg = FCI_CMD_AF_WRITE;
    }

    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_AF_SELECT;
    for (i = 0; i < max_addr; i++) {
        if (psipending())
            return(FALSE);
	regptr->cmdreg = FCI_CMD_AF_READ;
	sresult = regptr->argreg & 0xff;
	if ((sresult != 0) && fddi_reporting) {
	    errors = TRUE;
	    printf("\nmci: wrote af loc %#x with zeros, read 0x%-04x",
		   i, sresult);
	    if (stdio->automore_flush)
		return(FALSE);
	}

    }
    if (fddi_verbose && !errors)
	printf("[ok]");

    if (fddi_verbose)
	printf("\nmci: writing address filter with all ones...");
    if (stdio->automore_flush)
	return(FALSE);
    errors = FALSE;
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_AF_SELECT;
    for (i = 0; i < max_addr; i++) {
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
    for (i = 0; i < max_addr; i++) {
        if (psipending())
            return(FALSE);
	regptr->cmdreg = FCI_CMD_AF_READ;
	sresult = regptr->argreg & 0xff;
	if ((sresult != 0xff) && fddi_reporting) {
	    errors = TRUE;
	    printf("\nmci: wrote af loc %#x with ones, read 0x%-04x",
		   i, sresult);
	    if (stdio->automore_flush)
		return(FALSE);
	}
    }
    if (fddi_verbose && !errors)
	printf("[ok]");

    if (fddi_verbose)
	printf("\nmci: writing address filter with address+1...");
    if (stdio->automore_flush)
	return(FALSE);
    errors = FALSE;
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_AF_SELECT;
    for (i = 0; i < max_addr; i++) {
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
    for (i = 0; i < max_addr; i++) {
        if (psipending())
            return(FALSE);
	regptr->cmdreg = FCI_CMD_AF_READ;
	sresult = regptr->argreg & 0xff;
	if ((sresult != ((i + 1) & 0xFF)) && fddi_reporting) {
	    errors = TRUE;
	    printf("\nmci: wrote af loc %#x with %d, read 0x%-04x",
		   i, (i + 1) & 0xFF, sresult);
	    if (stdio->automore_flush)
		return(FALSE);
	}
    }
    if (fddi_verbose && !errors)
	printf("[ok]");

    return(TRUE);
}

/*
 * ctr_mema_dump
 * step through mema reading contents
 */

static boolean ctr_mema_dump (
    mcitype *mci)
{
    char buff[BUFLEN], *p;
    boolean parsed;
    int i, j, start, end, mema_size;
    ushort sresult[8];
    mciregtype *regptr;
    leveltype level;
    
    regptr = mci->regptr;
    regptr->argreg = FCI_MEMSIZE_MEMA;
    regptr->cmdreg = FCI_CMD_MEMSIZE;
    mema_size = regptr->argreg;

    printf("\nstart [0]:");
    start = 0;
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
        if (!null(buff)) {
	    p = deblank(buff);
	    start = parse_unsigned(p, &parsed);
	}
    }
    if (!parsed)
	return(FALSE);
    printf("\nend [%x]:", mema_size);
    end = mema_size;
    parsed = rdtty(buff, BUFLEN);
    if (parsed) {
        if (!null(buff)) {
	    p = deblank(buff);
	    end = parse_unsigned(p, &parsed);
	    if (end > mema_size)
		end = mema_size;
	}
    } else {
	return(FALSE);
    }

    for (i = start; i < end; i += 8) {
	printf("\n%-06x: ", i);
        level = raise_interrupt_level(NETS_DISABLE);
        (void) cbus_select(mci, default_interface);
        regptr->argreg = i;
        regptr->cmdreg = FCI_CTRLR_ARGUMENT;
        regptr->argreg = FCI_CTR_MEMA_SELECT;
        regptr->cmdreg = FCI_CTRLR_CMD;
	for (j = 0; j < 8; j++) {
	    regptr->argreg = FCI_CTR_MEMA_READ;
	    regptr->cmdreg = FCI_CTRLR_CMD;
	    sresult[j] = regptr->argreg;
	}
        reset_interrupt_level(level);
        for (j = 0; j < 8; j++)
            printf("%-04x ", sresult[j]);
	if (stdio->automore_flush)
	    return(FALSE);
    }
    return(TRUE);
}

/*
 * ctr_type_memb
 * type out a range of locations in memb
 */

static void ctr_type_memb (mcitype *mci, int low, int high)
{
    leveltype level;
    mciregtype *regptr;
    int i, j;
    ushort sresult;
    char buffer[BUFLEN];
    hwidbtype *idb;
    mcitype *cbus;

    regptr = mci->regptr;
    cbus = (mcitype *)cbusQ.qhead;
    idb = cbus->interface_list[default_interface];  /* hack, hack */
    for (i = low; i <= high; i+=16) {
	sprintf(buffer, "\n%-06x: ", i);
	level = raise_interrupt_level(NETS_DISABLE);
	ctr_memb_setup(idb, i);
	for (j = 0; (j < 8) && (i+(j*2) <= high); j++) {
	    regptr->argreg = FCI_CTR_MEMB_NC_READ;
	    regptr->cmdreg = FCI_CTRLR_CMD;
	    DELAY(10);
	    sresult = regptr->argreg;
	    sprintf(buffer+strlen(buffer), "%-04x ", sresult);
	}
	reset_interrupt_level(level);
	printf(buffer);
	if (stdio->automore_flush)
	    return;
    }
}


/*
 * ctr_memb_dump
 * step through memb reading contents
 */

static boolean ctr_memb_dump (mcitype *mci)
{
    int i, j;
    ushort sresult;
    mciregtype *regptr;
    
    regptr = mci->regptr;
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MEMB_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    for (i = 0; i < CTR_MEMB_SIZE; i += 16) {
	printf("\n%-06x: ", i);
	for (j = 0; j < 8; j++) {
	    regptr->argreg = FCI_CTR_MEMB_NC_READ;
	    regptr->cmdreg = FCI_CTRLR_CMD;
	    DELAY(10);
	    sresult = regptr->argreg;
	    printf("%-04x ", sresult);
	}
	if (stdio->automore_flush)
	    return(FALSE);
    }
    return(TRUE);
}

/*
 * ctr_memb_test
 * run tests of memb
 * returns FALSE if the user quit out of error display
 */

static boolean ctr_memb_test (mcitype *mci, boolean contention)
{
    int i, j;
    ushort sresult, rcmd, wcmd, value;
    boolean errors;
    mciregtype *regptr;

    regptr = mci->regptr;
    if (contention) {
	rcmd = FCI_CTR_MEMB_C_READ;
	wcmd = FCI_CTR_MEMB_C_WRITE;
    } else {
	rcmd = FCI_CTR_MEMB_NC_READ;
	wcmd = FCI_CTR_MEMB_NC_WRITE;
    }

    /*
     * memb -- write zeros
     */
    if (fddi_verbose)
	printf("\nmci: testing memb with zeros...");
    if (stdio->automore_flush)
	return(FALSE);
    errors = FALSE;
    regptr->argreg = SPY_CMDBLK_END;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MEMB_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    for (i = SPY_CMDBLK_END; i < CTR_MEMB_SIZE; i+=2) {
	regptr->argreg = wcmd;
	regptr->cmdreg = FCI_CTRLR_CMD;
    }
    regptr->argreg = SPY_CMDBLK_END;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MEMB_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    for (i = SPY_CMDBLK_END; i < CTR_MEMB_SIZE; i+=2) {
        if (psipending())
            return(FALSE);
	regptr->argreg = rcmd;
	regptr->cmdreg = FCI_CTRLR_CMD;
	sresult = regptr->cmdreg;
	for (j = 0; j < ctr_delay; j++) {
	    sresult = regptr->cmdreg;
	    if ((sresult == FCI_RSP_OKAY) || (sresult >= FCI_RSP_ERR_UNK))
		break;
	}
	if (sresult != FCI_RSP_OKAY) {
	    printf("\nmci: command %04x failed, result %04x.", rcmd, sresult);
	    if (stdio->automore_flush)
		return(FALSE);
	    continue;
	}
	sresult = regptr->argreg;
	if ((sresult != 0) && fddi_reporting) {
	    errors = TRUE;
	    if (stdio->automore_flush)
		return(FALSE);
	    printf("\nmci: wrote memb loc 0x%-04x with zeros, read 0x%-04x",
		   i, sresult);
	}
    }
    if (fddi_verbose && !errors)
	printf("[ok]");
    if (psipending())
	return(FALSE);

    /*
     * memb -- write ones
     */
    if (fddi_verbose)
	printf("\nmci: testing memb with all ones...");
    if (stdio->automore_flush)
	return(FALSE);
    errors = FALSE;
    regptr->argreg = SPY_CMDBLK_END;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MEMB_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    regptr->argreg = 0xffff;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    for (i = SPY_CMDBLK_END; i < CTR_MEMB_SIZE; i+=2) {
        if (psipending())
            return(FALSE);
	regptr->argreg = wcmd;
	regptr->cmdreg = FCI_CTRLR_CMD;
    }
    regptr->argreg = SPY_CMDBLK_END;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MEMB_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    for (i = SPY_CMDBLK_END; i < CTR_MEMB_SIZE; i+=2) {
        if (psipending())
            return(FALSE);
	regptr->argreg = rcmd;
	regptr->cmdreg = FCI_CTRLR_CMD;
	sresult = regptr->cmdreg;
	for (j = 0; j < ctr_delay; j++) {
	    sresult = regptr->cmdreg;
	    if ((sresult == FCI_RSP_OKAY) || (sresult >= FCI_RSP_ERR_UNK))
		break;
	}
	if (sresult != FCI_RSP_OKAY) {
	    printf("\nmci: command %04x failed, result %04x.", rcmd, sresult);
	    if (stdio->automore_flush)
		return(FALSE);
	    continue;
	}
	sresult = regptr->argreg;
	if ((sresult != 0xffff) && fddi_reporting) {
	    errors = TRUE;
	    if (stdio->automore_flush)
		return(FALSE);
	    printf("\nmci: wrote memb loc 0x%-04x with all ones, read 0x%-04x",
		   i, sresult);
	}
    }
    if (fddi_verbose && !errors)
	printf("[ok]");
    if (psipending())
	return(TRUE);
   
    /*
     * memb -- write address + 1
     */
    if (fddi_verbose)
	printf("\nmci: testing memb with address+1...");
    if (stdio->automore_flush)
	return(FALSE);
    errors = FALSE;
    regptr->argreg = SPY_CMDBLK_END;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MEMB_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    for (i = SPY_CMDBLK_END; i < CTR_MEMB_SIZE; i+=2) {
        if (psipending())
            return(FALSE);
	value = (((i+1) & 0xff) << 8) | ((i+2) & 0xff);
	regptr->argreg = value;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = wcmd;
	regptr->cmdreg = FCI_CTRLR_CMD;
    }
    regptr->argreg = SPY_CMDBLK_END;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MEMB_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    for (i = SPY_CMDBLK_END; i < CTR_MEMB_SIZE; i+=2) {
        if (psipending())
            return(FALSE);
	value = (((i+1) & 0xff) << 8) | ((i+2) & 0xff);
	regptr->argreg = rcmd;
	regptr->cmdreg = FCI_CTRLR_CMD;
	sresult = regptr->cmdreg;
	for (j = 0; j < ctr_delay; j++) {
	    sresult = regptr->cmdreg;
	    if ((sresult == FCI_RSP_OKAY) || (sresult >= FCI_RSP_ERR_UNK))
		break;
	}
	if (sresult != FCI_RSP_OKAY) {
	    printf("\nmci: command %04x failed, result %04x.", rcmd, sresult);
	    if (stdio->automore_flush)
		return(FALSE);
	    continue;
	}
	sresult = regptr->argreg;
	if ((sresult != value) && fddi_reporting) {
	    errors = TRUE;
	    if (stdio->automore_flush)
		return(FALSE);
	    printf("\nmci: wrote memb loc %#x with 0x%-04x, read 0x%-04x",
		   i, value, sresult);
	}
    }
    if (fddi_verbose && !errors)
	printf("[ok]");
    return(TRUE);
}

/*
 * ctr_memb_writeonce
 * Write once and return
 */

static void ctr_memb_writeonce (mcitype *mci, ushort loc, ushort data)
{
    mciregtype *regptr;
    
    regptr = mci->regptr;
    regptr->argreg = loc;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MEMB_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    regptr->argreg = data;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MEMB_NC_WRITE;
    regptr->cmdreg = FCI_CTRLR_CMD;

    regptr->argreg = loc;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MEMB_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    regptr->argreg = FCI_CTR_MEMB_NC_READ;
    regptr->cmdreg = FCI_CTRLR_CMD;
    data = regptr->argreg;
    printf("\nRead a 0x%-04x from memb location 0x%-04x", data, loc);
}

/*
 * ctr_memb_readdonce
 * Read once from selected location
 */

static void ctr_memb_readonce (mcitype *mci, ushort loc)
{
    mciregtype *regptr;
    ushort result;
    
    regptr = mci->regptr;
    regptr->argreg = loc;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MEMB_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    regptr->argreg = FCI_CTR_MEMB_NC_READ;
    regptr->cmdreg = FCI_CTRLR_CMD;
    DELAY(10);
    result = regptr->argreg;
    printf("\nRead a 0x%-04x from memb location 0x%-04x", result, loc);
}

/*
 * ctr_memb_readwrite
 * Special read/write testing for scoping mema failures.
 */
static void ctr_memb_readwrite (
    mcitype *mci,
    ushort loc,
    ushort data,
    boolean contention)
{
    mciregtype *regptr;
    ushort result, rcmd, wcmd;

    regptr = mci->regptr;
    result = 0;
    fddi_psistart();
    if (contention) {
	rcmd = FCI_CTR_MEMB_C_READ;
	wcmd = FCI_CTR_MEMB_C_WRITE;
    } else {
	rcmd = FCI_CTR_MEMB_NC_READ;
	wcmd = FCI_CTR_MEMB_NC_WRITE;
    }
    printf("\n%sriting memb location 0x%-03x with data 0x%-04x. Read with %s",
	   (contention ? "Contention w" : "W"), loc, data, fddi_escape);
    while (!psipending()) {
	regptr->argreg = loc;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_CTR_MEMB_SELECT;
	regptr->cmdreg = FCI_CTRLR_CMD;
	regptr->argreg = data;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = wcmd;
	regptr->cmdreg = FCI_CTRLR_CMD;
    }
    fddi_psiclear();
    printf("\n%seading memb location 0x%-03x. End test with %s", 
	   (contention ? "Contention r" : "R"), loc, fddi_escape);
    while (!psipending()) {
	regptr->argreg = loc;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_CTR_MEMB_SELECT;
	regptr->cmdreg = FCI_CTRLR_CMD;
	regptr->argreg = rcmd;
	regptr->cmdreg = FCI_CTRLR_CMD;
	result = regptr->argreg;
    }
    fddi_psiend();
    printf("\nRead a 0x%-03x from 0x%-04x", result, loc);
}

/*
 * ctr_spyglass_memb_test
 * run tests of memb through the spyglass
 * returns FALSE if user quit out of error display
 */

static boolean ctr_spyglass_memb_test (mcitype *mci, boolean contention)
{
    leveltype level;
    ushort i;
    ushort sresult, value;
    boolean errors;
    mciregtype *regptr;
    int j;

    /*
     * memb -- write zeros
     */
    if (fddi_verbose) {
	printf("\nmci: writing memb through spyglass with zeros...");
	if (stdio->automore_flush)
	    return(FALSE);
    }
    errors = FALSE;
    regptr = mci->regptr;
    level = raise_interrupt_level(NETS_DISABLE);
    cbus_select(mci, default_interface);
    regptr->argreg = SPY_CMDBLK_END;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_SPYG_MEM_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    regptr->argreg = 0x0000;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    for (i = SPY_CMDBLK_END; i < CTR_MEMB_SIZE; i+=2) {
	regptr->argreg = FCI_CTR_SPYG_MEM_WRITE;
	regptr->cmdreg = FCI_CTRLR_CMD;
	DELAY(1);
    }
    reset_interrupt_level(level);

    if (fddi_verbose) {
	printf("\nmci: reading memb directly...");
	if (stdio->automore_flush)
	    return(FALSE);
    }
    level = raise_interrupt_level(NETS_DISABLE);
    cbus_select(mci, default_interface);
    regptr->argreg = SPY_CMDBLK_END;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MEMB_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    for (i = SPY_CMDBLK_END; i < CTR_MEMB_SIZE; i+=2) {
	regptr->argreg = FCI_CTR_MEMB_C_READ;
	regptr->cmdreg = FCI_CTRLR_CMD;
	sresult = regptr->cmdreg;
	for (j = 0; j < ctr_delay; j++) {
	    sresult = regptr->cmdreg;
	    if ((sresult == FCI_RSP_OKAY) || (sresult >= FCI_RSP_ERR_UNK))
		break;
	}
	if (sresult != FCI_RSP_OKAY) {
	    reset_interrupt_level(level);
	    printf("\nmci: address %04x, command %04x failed, result %04x.",
		   i, FCI_CTR_MEMB_C_READ, sresult);
	    if (stdio->automore_flush)
		return(FALSE);
	    level = raise_interrupt_level(NETS_DISABLE);
	    cbus_select(mci, default_interface);
	    continue;
	}
	sresult = regptr->argreg;
	if ((sresult != 0) && fddi_reporting) {
	    reset_interrupt_level(level);
	    errors = TRUE;
	    if (stdio->automore_flush)
		return(FALSE);
	    printf("\nmci: wrote memb loc 0x%-04x with zeros, read 0x%-04x",
		   i, sresult);
	    level = raise_interrupt_level(NETS_DISABLE);
	    cbus_select(mci, default_interface);
	}
    }
    reset_interrupt_level(level);
    if (fddi_verbose && !errors)
	printf("[ok]");
    
    /*
     * memb -- write ones
     */
    if (fddi_verbose) {
	printf("\nmci: writing memb directly with all ones...");
	if (stdio->automore_flush)
	    return(FALSE);
    }
    errors = FALSE;
    level = raise_interrupt_level(NETS_DISABLE);
    cbus_select(mci, default_interface);
    regptr->argreg = SPY_CMDBLK_END;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MEMB_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    regptr->argreg = 0xffff;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    for (i = SPY_CMDBLK_END; i < CTR_MEMB_SIZE; i+=2) {
	regptr->argreg = FCI_CTR_MEMB_C_WRITE;
	regptr->cmdreg = FCI_CTRLR_CMD;
    }
    if (fddi_verbose) {
	reset_interrupt_level(level);
	printf("\nmci: reading memb through spyglass...");
	if (stdio->automore_flush)
	    return(FALSE);
	level = raise_interrupt_level(NETS_DISABLE);
	cbus_select(mci, default_interface);
    }
    regptr->argreg = SPY_CMDBLK_END;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_SPYG_MEM_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    for (i = SPY_CMDBLK_END; i < CTR_MEMB_SIZE; i+=2) {
	regptr->argreg = FCI_CTR_SPYG_MEM_READ;
	regptr->cmdreg = FCI_CTRLR_CMD;
	DELAY(1);
	regptr->argreg = FCI_CTR_SPYG_MEM_READ_RSLT;
	regptr->cmdreg = FCI_CTRLR_CMD;
	sresult = regptr->cmdreg;
	for (j = 0; j < ctr_delay; j++) {
	    sresult = regptr->cmdreg;
	    if ((sresult == FCI_RSP_OKAY) || (sresult >= FCI_RSP_ERR_UNK))
		break;
	}
	if (sresult != FCI_RSP_OKAY) {
	    reset_interrupt_level(level);
	    printf("\nmci: address %04x, command %04x failed, result %04x.",
		   i, FCI_CTR_SPYG_MEM_READ_RSLT, sresult);
	    if (stdio->automore_flush)
		return(FALSE);
	    level = raise_interrupt_level(NETS_DISABLE);
	    cbus_select(mci, default_interface);
	    continue;
	}
	sresult = regptr->argreg;
	if ((sresult != 0xffff) && fddi_reporting) {
	    reset_interrupt_level(level);
	    errors = TRUE;
	    if (stdio->automore_flush)
		return(FALSE);
	    printf("\nmci: wrote memb loc 0x%-04x with all ones, read 0x%-04x",
		   i, sresult);
	    level = raise_interrupt_level(NETS_DISABLE);
	    cbus_select(mci, default_interface);
	}
    }
    reset_interrupt_level(level);
    if (fddi_verbose && !errors)
	printf("[ok]");
    
    /*
     * memb -- write address + 1
     */
    if (fddi_verbose) {
	printf("\nmci: writing memb through spyglass with address+1...");
	if (stdio->automore_flush)
	    return(FALSE);
    }
    errors = FALSE;
    level = raise_interrupt_level(NETS_DISABLE);
    cbus_select(mci, default_interface);
    regptr->argreg = SPY_CMDBLK_END;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_SPYG_MEM_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    for (i = SPY_CMDBLK_END; i < CTR_MEMB_SIZE; i+=2) {
	value = (((i+1) & 0xff) << 8) | ((i+2) & 0xff);
	regptr->argreg = value;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_CTR_SPYG_MEM_WRITE;
	regptr->cmdreg = FCI_CTRLR_CMD;
	DELAY(1);
    }
    if (fddi_verbose) {
	reset_interrupt_level(level);
	printf("\nmci: reading memb directly...");
	if (stdio->automore_flush)
	    return(FALSE);
	level = raise_interrupt_level(NETS_DISABLE);
	cbus_select(mci, default_interface);
    }
    regptr->argreg = SPY_CMDBLK_END;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MEMB_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    for (i = SPY_CMDBLK_END; i < CTR_MEMB_SIZE; i+=2) {
	value = (((i+1) & 0xff) << 8) | ((i+2) & 0xff);
	regptr->argreg = FCI_CTR_MEMB_C_READ;
	regptr->cmdreg = FCI_CTRLR_CMD;
	sresult = regptr->cmdreg;
	for (j = 0; j < ctr_delay; j++) {
	    sresult = regptr->cmdreg;
	    if ((sresult == FCI_RSP_OKAY) || (sresult >= FCI_RSP_ERR_UNK))
		break;
	}
	if (sresult != FCI_RSP_OKAY) {
	    reset_interrupt_level(level);
	    printf("\nmci: address %04x, command %04x failed, result %04x.",
		   i, FCI_CTR_MEMB_C_READ, sresult);
	    if (stdio->automore_flush)
		return(FALSE);
	    level = raise_interrupt_level(NETS_DISABLE);
	    cbus_select(mci, default_interface);
	    continue;
	}
	sresult = regptr->argreg;
	if ((sresult != value) && fddi_reporting) {
	    reset_interrupt_level(level);
	    errors = TRUE;
	    if (stdio->automore_flush)
		return(FALSE);
	    printf("\nmci: wrote memb loc %#x with 0x%-04x, read 0x%-04x",
		   i, value, sresult);
	    level = raise_interrupt_level(NETS_DISABLE);
	    cbus_select(mci, default_interface);
	}
    }
    reset_interrupt_level(level);
    if (fddi_verbose && !errors)
	printf("[ok]");

    /*
     * memb -- write address + 2
     */
    if (fddi_verbose) {
	printf("\nmci: writing memb directly with address+2...");
	if (stdio->automore_flush)
	    return(FALSE);
    }
    errors = FALSE;
    level = raise_interrupt_level(NETS_DISABLE);
    cbus_select(mci, default_interface);
    regptr->argreg = SPY_CMDBLK_END;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MEMB_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    for (i = SPY_CMDBLK_END; i < CTR_MEMB_SIZE; i+=2) {
	value = (((i+2) & 0xff) << 8) | ((i+3) & 0xff);
	regptr->argreg = value;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_CTR_MEMB_C_WRITE;
	regptr->cmdreg = FCI_CTRLR_CMD;
    }
    if (fddi_verbose) {
	reset_interrupt_level(level);
	printf("\nmci: reading memb through spyglass...");
	if (stdio->automore_flush)
	    return(FALSE);
	level = raise_interrupt_level(NETS_DISABLE);
	cbus_select(mci, default_interface);
    }
    regptr->argreg = SPY_CMDBLK_END;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_SPYG_MEM_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    for (i = SPY_CMDBLK_END; i < CTR_MEMB_SIZE; i+=2) {
	value = (((i+2) & 0xff) << 8) | ((i+3) & 0xff);
	regptr->argreg = FCI_CTR_SPYG_MEM_READ;
	regptr->cmdreg = FCI_CTRLR_CMD;
	DELAY(1);
	regptr->argreg = FCI_CTR_SPYG_MEM_READ_RSLT;
	regptr->cmdreg = FCI_CTRLR_CMD;
	sresult = regptr->cmdreg;
	for (j = 0; j < ctr_delay; j++) {
	    sresult = regptr->cmdreg;
	    if ((sresult == FCI_RSP_OKAY) || (sresult >= FCI_RSP_ERR_UNK))
		break;
	}
	if (sresult != FCI_RSP_OKAY) {
	    reset_interrupt_level(level);
	    printf("\nmci: address %04x, command %04x failed, result %04x.",
		   i, FCI_CTR_SPYG_MEM_READ_RSLT, sresult);
	    if (stdio->automore_flush)
		return(FALSE);
	    level = raise_interrupt_level(NETS_DISABLE);
	    cbus_select(mci, default_interface);
	    continue;
	}
	sresult = regptr->argreg;
	if ((sresult != value) && fddi_reporting) {
	    reset_interrupt_level(level);
	    errors = TRUE;
	    if (stdio->automore_flush)
		return(FALSE);
	    printf("\nmci: wrote memb loc %#x with 0x%-04x, read 0x%-04x",
		   i, value, sresult);
	    level = raise_interrupt_level(NETS_DISABLE);
	    cbus_select(mci, default_interface);
	}
    }
    reset_interrupt_level(level);
    if (fddi_verbose && !errors)
	printf("[ok]");

    return(TRUE);
}

#define NUM_PH_REGS 32
static void ctr_read_ph_registers (mcitype *mci)
{
    ushort *ptr, registers[NUM_PH_REGS];
    mciregtype *regptr;
    int i, address;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    (void) cbus_select(mci, default_interface);
    regptr = mci->regptr;
    regptr->argreg = 0xFE80;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_SPYG_MEM_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    ptr = registers;

    for (i = 0, address = 0xFE80; i < NUM_PH_REGS; address +=2, i++) {
	regptr->argreg = FCI_CTR_SPYG_MEM_READ;
	regptr->cmdreg = FCI_CTRLR_CMD;
	DELAY(1);
	regptr->argreg = FCI_CTR_SPYG_MEM_READ_RSLT;
	regptr->cmdreg = FCI_CTRLR_CMD;
	*ptr++ = regptr->argreg;
    }
    reset_interrupt_level(level);
    printf("\n");

    ptr = registers;
    printf("TXCMD:     %04x          RXCMD:   %04x\n", ptr[0], ptr[8]);
    printf("TXSTS:     %04x          RXSTS:   %04x\n", ptr[1], ptr[9]);
    printf("FTOK/PCFE: %04x          INTCTL:  %04x\n", ptr[2], ptr[10]);
    printf("----/PTT:  %04x          RINGSTS: %04x\n", ptr[3], ptr[11]);
    printf("TCP:       %04x          RCP:     %04x\n", ptr[4], ptr[12]);
    printf("---:       %04x          DMTC:    %04x\n", ptr[5], ptr[13]);
    printf("TAC:       %04x          RAC:     %04x\n", ptr[6], ptr[14]);
    printf("TLC:       %04x          RLC:     %04x\n", ptr[7], ptr[15]);
    printf("\n");
    printf("XSA:       %04x.%04x.%04x\n", ptr[16], ptr[17], ptr[18]);
    printf("UIA:       %04x.%04x.%04x\n", ptr[24], ptr[25], ptr[26]);
    printf("FNA:       %04x.%04x.%04x\n", 0xC000,  ptr[21], ptr[22]);
    printf("UGA:       %04x.%04x.%04x\n", ptr[28], ptr[29], ptr[30]);
    printf("LRC0:      %04x\n", ptr[20]);
    printf("Source:    %04x\n", ptr[19]);
    printf("Target:    %04x\n", ptr[23]);
    printf("Unknown27: %04x\n", ptr[27]);
    printf("Unknown31: %04x\n", ptr[31]);
}

static void ctr_write_ph_registers (mcitype *mci, ushort address, ushort data)
{
    mciregtype *regptr;

    regptr = mci->regptr;
    regptr->argreg = address;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_SPYG_MEM_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;

    regptr->argreg = data;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_SPYG_MEM_WRITE;
    regptr->cmdreg = FCI_CTRLR_CMD;
}

static void ctr_send_spyglass_command (mcitype *mci, ushort command,
				       ushort arg)
{
    mciregtype *regptr;

    regptr = mci->regptr;
    regptr->argreg = arg;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_POKE_CUETRC_ARG0;
    regptr->cmdreg = FCI_CTRLR_CMD;

    regptr->argreg = command;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_POKE_CUETRC_CMD;
    regptr->cmdreg = FCI_CTRLR_CMD;
}

/*
 * ctr_writepattern
 */
static void ctr_writepattern (mcitype *mci, int address, char mem_type)
{
    register ushort i;
    mciregtype *regptr;
    leveltype level;

    printf("\nWriting pattern 0,1,2,... to MEM%c at %#x (%#o)", mem_type,
	   address, address);
    regptr = mci->regptr;
    level = raise_interrupt_level(NETS_DISABLE);
    regptr->argreg = address;
    if (mem_type == 'A') {
	regptr->cmdreg = MCI_CMD_MEMA_SELECT;
    } else if (mem_type == 'B') {
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_CTR_MEMB_SELECT;
	regptr->cmdreg = FCI_CTRLR_CMD;
    } else {
	regptr->cmdreg = MCI_CMD_MEMD_SELECT;
    }

    for (i = 0; i < 256; i++) {
	if (mem_type == 'A') {
	    regptr->argreg = i;
	    regptr->cmdreg = MCI_CMD_MEMA_WRITE;
	} else if (mem_type == 'B') {
	    regptr->argreg = i;
	    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	    regptr->argreg = FCI_CTR_MEMB_NC_WRITE;
	    regptr->cmdreg = FCI_CTRLR_CMD;
	} else {
	    regptr->write0short = i;
	}
    }
    reset_interrupt_level(level);
}

/*
 * ctr_clear_memb
 */
static void ctr_clear_memb (mcitype *mci)
{
    register int i;
    mciregtype *regptr;
    leveltype level;

    printf("\nZeroing memb.\n");
    regptr = mci->regptr;
    level = raise_interrupt_level(NETS_DISABLE);
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CTR_MEMB_SELECT;
    regptr->cmdreg = FCI_CTRLR_CMD;
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    for (i = 0; i < CTR_MEMB_SIZE; i += 2) {
	regptr->argreg = FCI_CTR_MEMB_NC_WRITE;
	regptr->cmdreg = FCI_CTRLR_CMD;
    }
    reset_interrupt_level(level);
}


/*
 * fddi_mci_interactive_list
 */
static void fddi_mci_interactive_fill (mcitype *mci, int *laddr, char *lmem)
{
    int value;
    boolean parsed;
    char buff[BUFLEN];
    
    while (TRUE) {
	if (stdio->statbits & (IDLETIMEOUT | HANGUP | IDLE | CARDROP))
	    return;
	value = *laddr;
	printf("\ndata address [%#x]: ", value);
	parsed = rdtty(buff, BUFLEN);
	if (parsed) {
	    if (!null(buff))
		value = parse_unsigned(buff, &parsed);
	    parsed = parsed && (value <= 0xffff);
	}
	if (!parsed)
	    continue;
	*laddr = value;
	printf("\nmemory to fill [MEM-%c]: ", *lmem);
	parsed = rdtty(buff, BUFLEN);
	if (parsed) {
	    if (!null(buff)) {
		parsed = ((toupper(*buff) == 'A') ||
			  (toupper(*buff) == 'B') ||
			  (toupper(*buff) == 'D'));
	    }
	}
	if (!parsed)
	    continue;
	*lmem = toupper(*buff);
	return;
    }
}

static uchar token_test[IEEEBYTES] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};

static void ctr_odd_byte_start (
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
	    regptr->write0short = AC_PRI0;
	    regptr->write0short = (FC_TYPE_LLC << 8) | token_test[0];
	    regptr->write0short = (token_test[1] << 8) | token_test[2];
	    regptr->write0short = (token_test[3] << 8) | token_test[4];
	    regptr->write0short = (token_test[5] << 8) | token_test[0];
	    regptr->write0short = (token_test[1] << 8) | token_test[2];
	    regptr->write0short = (token_test[3] << 8) | token_test[4];
	    regptr->write0short = (token_test[5] << 8) | 0x04;
	    regptr->write0short = 0x0403;
	    regptr->write0short = 0x1112;
	    switch (bufcnt & 0x3) {
	      case 0:
		regptr->write0short = 0x13FF;
		regptr->write0short = 0xFFFF;
		regptr->write0short = 0xFFFF;
		bytes = 0x14;
		break;
	      case 1:
		regptr->write0short = 0x1314;
		regptr->write0short = 0xFFFF;
		regptr->write0short = 0xFFFF;
		bytes = 0x15;
		break;
	      case 2:
		regptr->write0short = 0x1314;
		regptr->write0short = 0x15FF;
		regptr->write0short = 0xFFFF;
		regptr->write0short = 0xFFFF;
		bytes = 0x16;
		break;
	      case 3:
		regptr->write0short = 0x1314;
		regptr->write0short = 0x1516;
		regptr->write0short = 0xFFFF;
		regptr->write0short = 0xFFFF;
		bytes = 0x17;
		break;
	    }
	} else {
	    /*
	     * even.  First word gets AC/FC
	     */
	    regptr->write0short = (AC_PRI0 << 8) | FC_TYPE_LLC;
	    regptr->write0short = (token_test[0] << 8) | token_test[1];
	    regptr->write0short = (token_test[2] << 8) | token_test[3];
	    regptr->write0short = (token_test[4] << 8) | token_test[5];
	    regptr->write0short = (token_test[0] << 8) | token_test[1];
	    regptr->write0short = (token_test[2] << 8) | token_test[3];
	    regptr->write0short = (token_test[4] << 8) | token_test[5];
	    regptr->write0short = 0x0404;
	    regptr->write0short = 0x0311;
	    regptr->write0short = 0x1213;
	    switch (bufcnt & 0x3) {
	      case 0:
		regptr->write0short = 0xFFFF;
		regptr->write0short = 0xFFFF;
		bytes = 0x14;
		break;
	      case 1:
		regptr->write0short = 0x14FF;
		regptr->write0short = 0xFFFF;
		regptr->write0short = 0xFFFF;
		bytes = 0x15;
		break;
	      case 2:
		regptr->write0short = 0x1415;
		regptr->write0short = 0xFFFF;
		regptr->write0short = 0xFFFF;
		bytes = 0x16;
		break;
	      case 3:
		regptr->write0short = 0x1415;
		regptr->write0short = 0x16FF;
		regptr->write0short = 0xFFFF;
		regptr->write0short = 0xFFFF;
		bytes = 0x17;
		break;
	    }
	}
	regptr->argreg = bytes | (((bufcnt >> 2) & 1) ? MCI_TX_ODDALIGN : 0);
	regptr->cmdreg = FCI_CMD_TX0_START;
	reset_interrupt_level(level);
    }
}


static void ctr_test_rif_match (
    mcitype *mci)
{
    char buff[BUFLEN];
    mciregtype *regptr;
    leveltype level;
    trrif_t *trh;
    hwidbtype *idb;
    ushort *rd, temp;
    int i, j, bp;
    mcitype *cbus;
    boolean okay;
    srb_vector_table_t *srbV_p;
    srb_triplet_t srb_triplet;

    cbus = (mcitype *)cbusQ.qhead;
    regptr = mci->regptr;
    idb = cbus->interface_list[default_interface];  /* hack, hack */
  
    srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) {
        srbV_p->srb_sb_get_triplet(idb->firstsw, &srb_triplet);
    } else {
        srb_triplet.localRing  = SRB_INVALID_RING_NUM;
        srb_triplet.bridgeNum  = SRB_INVALID_BRIDGE_NUM;
        srb_triplet.remoteRing = SRB_INVALID_RING_NUM;
    }
    
    level = raise_interrupt_level(NETS_DISABLE);
    (void) cbus_select(mci, default_interface);

    trh = (trrif_t *)buff;
    trh->ac = AC_PRI0;
    trh->fc = FC_TYPE_LLC;
    ieee_copy(testda, trh->daddr);
    ieee_copy(idb->hardware, trh->saddr);
    trh->saddr[0] |= TR_RII;

    /*
     * Forward...
     */
    trh->rc_dlf = 0x30;
    for (i = 6; i < 18; i += 2) {
	trh->rc_blth = i;
	rd = trh->rd;
	if (i > 6)
	    for (j = 0; j < i-6; j += 2)
		*rd++ = i*35;
	*rd++ = (srb_triplet.localRing << 4) | srb_triplet.bridgeNum;
	*rd++ = (srb_triplet.remoteRing << 4);
	*rd++ = 0x0404;
	*rd++ = 0x0300;
	
	regptr->argreg = mci->system_buffer_pool;
	regptr->cmdreg = FCI_CMD_TX0_RESERVE;
	if ((temp = regptr->cmdreg) != FCI_RSP_OKAY) {
	    buginf("\nNo system buffer");
	    for (bp = 0; (bp < mci->if_sizes_count) &&
		 (BUFLEN > mci->if_sizes[bp]); bp++) ;
	    regptr->argreg = bp;
	    regptr->cmdreg = FCI_CMD_TX0_RESERVE;
	    okay = ((temp = regptr->cmdreg) == MCI_RSP_OKAY);
	    while ((++bp < mci->if_sizes_count) && !okay) {
		buginf("\nNo buffer in pool %d", bp-1);
		regptr->argreg = bp;
		regptr->cmdreg = FCI_CMD_TX0_RESERVE;
		okay = ((temp = regptr->cmdreg) == MCI_RSP_OKAY);
	    }
	    if (!okay) {
		reset_interrupt_level(level);
		buginf("\nBad response (%04x) to reserve", temp);
		return;
	    }
	}
	regptr->argreg = 0;
	regptr->cmdreg = FCI_CMD_TX0_SELECT;
	buffer2mci(buff, &regptr->write0long, BUFLEN);
	regptr->argreg = BUFLEN;
	regptr->cmdreg = FCI_CMD_TX0_START;
    }

    /*
     * ...and backward
     */
    trh->rc_dlf = 0xB0;
    for (i = 6; i < 18; i += 2) {
	trh->rc_blth = i;
	rd = trh->rd;
	if (i > 6)
	    for (j = 0; j < i-6; j += 2)
		*rd++ = i*35;
	*rd++ = (srb_triplet.remoteRing << 4) | srb_triplet.bridgeNum;
	*rd++ = (srb_triplet.localRing << 4);
	*rd++ = 0x0404;
	*rd++ = 0x0300;
	
	regptr->argreg = mci->system_buffer_pool;
	regptr->cmdreg = FCI_CMD_TX0_RESERVE;
	if ((temp = regptr->cmdreg) != FCI_RSP_OKAY) {
	    buginf("\nNo system buffer");
	    for (bp = 0; (bp < mci->if_sizes_count) &&
		 (BUFLEN > mci->if_sizes[bp]); bp++) ;
	    regptr->argreg = bp;
	    regptr->cmdreg = FCI_CMD_TX0_RESERVE;
	    okay = ((temp = regptr->cmdreg) == MCI_RSP_OKAY);
	    while ((++bp < mci->if_sizes_count) && !okay) {
		buginf("\nNo buffer in pool %d", bp-1);
		regptr->argreg = bp;
		regptr->cmdreg = FCI_CMD_TX0_RESERVE;
		okay = ((temp = regptr->cmdreg) == MCI_RSP_OKAY);
	    }
	    if (!okay) {
		reset_interrupt_level(level);
		buginf("\nBad response (%04x) to reserve", temp);
		return;
	    }
	}
	regptr->argreg = 0;
	regptr->cmdreg = FCI_CMD_TX0_SELECT;
	buffer2mci(buff, &regptr->write0long, BUFLEN);
	regptr->argreg = BUFLEN;
	regptr->cmdreg = FCI_CMD_TX0_START;
    }
    reset_interrupt_level(level);
}

/*
 *
 */
static void ctr_test_dma (mcitype *mci)
{
    char buff[BUFLEN];
    int pool, i, j;
    ushort cbus_port, temp, bytes;
    ulong ltemp;
    mciregtype *regptr;
    leveltype level;
    boolean parsed = FALSE;

    ltemp = 0;
    regptr = mci->regptr;
    printf("\n  pool: ");
    if (rdtty(buff, BUFLEN)) {
	if (null(buff))
	    return;
	pool = parse_unsigned(buff, &parsed);
	if ((!parsed) || pool <= 7)
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
	parsed = parsed && (bytes >= 16);
	if (!parsed) {
	    printf("\nToo small.");
	    return;
	}
    } else
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
    for (i = 0; i < 4; i++) {
	for (j = 0; j < 4; j++) {
	    printf("\nPress return to send %s/%s start %s/%s end frame.",
		   (i / 2) ? "odd" : "even", (i % 2) ? "odd" : "even", 
		   (((i+j)%4) / 2) ? "odd" : "even",
		   ((i+j) % 2) ? "odd" : "even");
	    if (!rdtty(buff, BUFLEN))
		return;

	    bytes = ((TRING_ENCAPBYTES + 3)/4) + j;
	    regptr->argreg = pool;
	    regptr->cmdreg = FCI_CMD_TX0_RESERVE;
	    if (regptr->cmdreg != FCI_RSP_OKAY) {
		printf("TX0_RESERVE failed.  Aborting");
		return;
	    }
	    regptr->argreg = (i / 2);
	    regptr->cmdreg = FCI_CMD_TX0_SELECT;
	    if (i & 1) {
		/*
		 * odd.  do the odd fc dance
		 */
		regptr->write0short = AC_PRI0;
		regptr->write0short = (FC_TYPE_LLC << 8) | testda[0];
		regptr->write0short = *(ushort *)&testda[1];
		regptr->write0short = *(ushort *)&testda[3];
		regptr->write0short = (testda[5] << 8) | testsa[0];
		regptr->write0short = *(ushort *)&testsa[1];
		regptr->write0short = *(ushort *)&testsa[3];
		regptr->write0short = (testsa[5] << 8) | ((i*4)+j);
		ltemp = 0xf;
	    } else {
		/*
		 * even.  First word gets AC/FC
		 */
		regptr->write0short = (AC_PRI0 << 8) | FC_TYPE_LLC;
		regptr->write0short = *(ushort *)&testda[0];
		regptr->write0short = *(ushort *)&testda[2];
		regptr->write0short = *(ushort *)&testda[4];
		regptr->write0short = *(ushort *)&testsa[0];
		regptr->write0short = *(ushort *)&testsa[2];
		regptr->write0short = *(ushort *)&testsa[4];
		regptr->write0short = ((i*4)+j) << 8 | 0xF;
		ltemp = 0x10;
	    }
	    while (ltemp < bytes - 1)
		regptr->write0short = (ltemp << 8) | (ltemp + 1);
	    if (ltemp < bytes)
		regptr->write0short = ltemp << 8;
	    regptr->argreg = bytes | ((i & 1) ? 0x4000 : 0);
	    regptr->cmdreg = FCI_CMD_TX0_START;
	}
    }
    reset_interrupt_level(level);
}


static ulong bhigh = 0, blow = 0;

/*
 * ctr specific commands
 */
void ctr_specific (mcitype *mci)
{
    char cmdchar;
    char buff[BUFLEN], *args;
    boolean parsed, parsedi, parsedj;
    ulong arglong;
    ulong command;
    int rcount;
    int address, data, i, j, laddr;
    char lmem;

    printf("\n\nCTR interface commands");
    arglong = command = 0;
    cmdchar = '?';
    rcount = 1;
    while (TRUE) {
	printf("\nctr (? for help) [%c]: ", cmdchar);
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
	    printf("\nA                        - r/w address filter");
	    printf("\nB                        - address filter test");
	    printf("\nC                        - execute dci command");
	    printf("\nD                        - send spyglass command");
	    printf("\nE                        - Odd byte start");
	    printf("\nF                        - memb diagnostic (contention)");
	    printf("\nG                        - memb diagnostic (non-contention)");
	    printf("\nH<address> <data>        - memb read/write test (contention)");
	    printf("\nI<address> <data>        - memb read/write test (non-contention)");
	    printf("\nJ                        - test transmit DMA machine");
	    printf("\nK                        - read spyglass PH registers");
	    printf("\nL<address> <data>        - write spyglass PH register");
	    printf("\nM                        - fill address filter");
	    printf("\nP                        - rif match test");
	    printf("\nQ                        - quit");
	    printf("\nR<address>               - read memb once");
	    printf("\nS<address> <data>        - write memb once");
	    printf("\nT<low> <high>            - list memb contents");
	    printf("\nU<address>               - write pattern in memory");
	    printf("\nV<address>               - spyglass memb test");
	    printf("\nW                        - zero memb");
	    printf("\nX                        - dump memb");
	    printf("\nY                        - dump mema");
	    break;

	  case 'A':
	    ctr_af_rw_cmd(mci);
	    break;

	  case 'B':
	    fddi_psistart();
	    for (;;)
		if (ctr_af_test(mci) == FALSE)
		    break;
	    fddi_psiend();
	    break;

	  case 'C':
	    dci_command(mci, &arglong, &command, &rcount);
	    break;

	  case 'D':
	    address = parse_unsigned(args, &parsed);
	    args = findarg(args);
	    data = parse_unsigned(args, &parsed);
	    ctr_send_spyglass_command(mci, address, data);
	    break;

	  case 'E':
	    ctr_odd_byte_start(mci);
	    break;

	  case 'F':
	    printf("\nRunning memb contention test until ctrl-^ typed...");
	    fddi_psistart();
	    for (;;)
		if (ctr_memb_test(mci, TRUE) == FALSE)
		    break;
	    fddi_psiend();
	    break;

	  case 'G':
	    printf("\nRunning memb non-contention test until ctrl-^ typed...");
	    fddi_psistart();
	    for (;;)
		if (ctr_memb_test(mci, FALSE) == FALSE)
		    break;
	    fddi_psiend();
	    break;

	  case 'H':
	    address = parse_unsigned(args, &parsed);
	    args = (char *) deblank(args);
	    while ((*args != ' ') && (*args != '\0') && (*args != '\t'))
		args++;
	    data = parse_unsigned(args, &parsed);
	    ctr_memb_readwrite(mci, address, data, TRUE);
	    break;

	  case 'I':
	    address = parse_unsigned(args, &parsed);
	    args = (char *) deblank(args);
	    while ((*args != ' ') && (*args != '\0') && (*args != '\t'))
		args++;
	    data = parse_unsigned(args, &parsed);
	    ctr_memb_readwrite(mci, address, data, FALSE);
	    break;

	  case 'J':
	    ctr_test_dma(mci);
	    break;

	  case 'K':
	    ctr_read_ph_registers(mci);
	    break;

	  case 'L':
	    address = parse_unsigned(args, &parsed);
	    args = (char *) deblank(args);
	    while ((*args != ' ') && (*args != '\0') && (*args != '\t'))
		args++;
	    data = parse_unsigned(args, &parsed);
	    ctr_write_ph_registers(mci, address, data);
	    break;

	  case 'M':
	    ctr_af_fill(mci);
	    break;

	  case 'P':
	    ctr_test_rif_match(mci);
	    break;

	  case 'Q':
	    automore_disable();
	    return;

	  case 'R':
	    address = parse_unsigned(args, &parsed);
	    for (i = 0; i < rcount; i++)
		ctr_memb_readonce(mci, address);
	    break;

	  case 'S':
	    address = parse_unsigned(args, &parsed);
	    args = (char *) deblank(args);
	    while ((*args != ' ') && (*args != '\0') && (*args != '\t'))
		args++;
	    data = parse_unsigned(args, &parsed);
	    for (i = 0; i < rcount; i++)
		ctr_memb_writeonce(mci, address, data);
	    break;

	  case 'T':
	    i = parse_unsigned(args, &parsedi);
	    args = (char *) deblank(args);
	    while ((*args != ' ') && (*args != '\0') && (*args != '\t'))
		args++;
	    j = parse_unsigned(args, &parsedj);
	    if ((!parsedi) || (!parsedj)) {
		cbus_address(&blow, &bhigh, 0xfff);
	    } else {
		blow = i;
		bhigh = j;
	    }
	    ctr_type_memb(mci, blow, bhigh);
	    break;

	  case 'U':
	    fddi_mci_interactive_fill(mci, &laddr, &lmem);
	    ctr_writepattern(mci, laddr, lmem);
	    break;

	  case 'V':
	    printf("\nRunning spyglass memb test until ctrl-^ typed...");
	    fddi_psistart();
	    while (TRUE) {
		if (!ctr_spyglass_memb_test(mci, TRUE) || psipending()) {
		    break;
		}
	    }
	    fddi_psiend();
	    break;

	  case 'W':
	    ctr_clear_memb(mci);
	    break;

	  case 'X':
	    ctr_memb_dump(mci);
	    break;

	  case 'Y':
	    ctr_mema_dump(mci);
	    break;

	  default:
	    printf("\nunknown command \"%c\"", cmdchar);
	    cmdchar = '?';
	    break;
	}
	automore_disable();
    }
}
