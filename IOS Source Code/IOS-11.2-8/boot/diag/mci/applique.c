/* $Id: applique.c,v 3.2.58.1 1996/05/17 10:22:57 ppearce Exp $
 * $Source: /release/112/cvs/Xboot/diag/mci/applique.c,v $
 *------------------------------------------------------------------
 * Applique
 *
 * By Bruce Sherry, Access Associates Inc.
 * February 5, 1990
 *
 * Copyright (c) 1992,1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Appliques test routines:
 *  applique_type: determines what type of applique is attached to a port.
 *  pingit: sends and receives 1 packet on the specified ports.
 *  ping_applique: calls pingit to send all packet sizes.
 *  clocks_on: sets the clocks on the specified port to the specified
 *    divisor.
 *  check_applique: Main routine, determins which ports to use, which types
 *    of applique to test, and so forth.
 *------------------------------------------------------------------
 * $Log: applique.c,v $
 * Revision 3.2.58.1  1996/05/17  10:22:57  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.46.1  1996/04/03  13:11:17  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  07:42:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:07:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../../defs.h"
#include "../../types.h"
#include "../../cpu.h"
#include "../h/ether.h"
#include "../h/if_mci.h"
#include "../../test.h"
#include "../../reason.h"
#include "../../extern.h"

extern void alloc_diag_mem();
extern void init_error_struct();
extern mcitype *setup_unit();

extern uchar *packet1;
extern uchar *packet2;
int aborts, serial_divider;

/*
 * applique_type
 *
 * Parameters:
 *	mci	which mci to talk to
 *	unit to talk to on the mci
 *
 * returns:
 *	-2 if interface not serial
 *	-1 if no interface
 *	0 if no applique
 *	n for cisco applique type n
 */

#define APP_COUNT 11

int applique_type(mci, unit)
    mcitype *mci;
    int	unit;
{
    ushort result;
    int i, code, shift;
    
    ushort app_cmds[APP_COUNT] = {0, 1, 2, 3, 0, 4, 5, 6, 7, 4, 0};
    uchar app_uses[APP_COUNT] =  {1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0};
    
    mci->regptr->argreg = unit;
    mci->regptr->cmdreg = MCI_CMD_SELECT;
    result = mci->regptr->cmdreg;
    if (result != MCI_RSP_OKAY)
	return(-1);
    result = mci->regptr->argreg;
    if (result == 0)
	return(-2);
    for (i = 0, code = 0, shift = 0; i < APP_COUNT; i++) {
	mci->regptr->argreg = app_cmds[i];
	mci->regptr->cmdreg = MCI_CMD_APPLIQUE;
	result = mci->regptr->argreg;            
	if (result & MCI_APP_SENSE0) {
	    code = 0;	/* not a valid cisco applique */
	    break;
	}
	if (app_uses[i]) {
	    if (result & MCI_APP_SENSE1)
		code |= (1 << shift);
	    shift++;
	}
    }
    return code;
}

/*   
 * pingit
 *
 * Sends one packet on the "out" port, and receives it on the "in" port.
 * Compares data and lengths to be sure it got the right packet.
 *
 * Parameters:
 * 	mci: interface board to use.
 * 	out: port to send the packets from.
 *	in:  port to receive the packets from.
 *  packet: pointer to the data to send.
 *	rpacket: pointer to the place to put the received packet.
 *	len: Length of the packet.
 *	report: Print error messages if errors happen.
 * 
 * Returns:
 *	True if things went OK.
 *	False if not.
 *
 */

pingit(mci, out, in, tpacket, rpacket, len, report)
    mcitype *mci;
    int	out, in, len;
    ulong *tpacket;
    ulong *rpacket;
    boolean report;
{
    ushort sresult, rstatus = 0;
    int	waiting, rlength = 0, i;
    uchar *sent, *recvd;
    
    sent = (uchar*) tpacket;
    recvd = (uchar*) rpacket;
    mci->regptr->argreg = out;
    mci->regptr->cmdreg = MCI_CMD_SELECT;
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_TX0_RESERVE;
    sresult = mci->regptr->cmdreg;
    if (sresult != MCI_RSP_OKAY) {
	if (report) {
	    printf("\nApplique: TX0_RESERVE failed - %#x", sresult);
	    printf("\n? Possible output hang on interface %d", out);
	}
	return(FALSE);
    }
    
    /*
     * Set pointer to start of buffer (TX0_SELECT).
     */
    
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_TX0_SELECT;
    
    /*
     * Put packet into transmit buffer.
     * Then write byte length and start transmission.
     */
    
    for(i = 0; i <= len; i += 4)
	mci->regptr->write0long = *tpacket++;
    mci->regptr->argreg = out;
    mci->regptr->cmdreg = MCI_CMD_SELECT;
    mci->regptr->argreg = len;
    mci->regptr->cmdreg = MCI_CMD_TX0_START;
    
    /*
     * Wait for packet to come back.
     *
     */
    
    mci->regptr->argreg = in;
    mci->regptr->cmdreg = MCI_CMD_SELECT;
    if (cpu_type == CPU_CSC3) /* Adjust for processor speed */
	waiting = 15;
    else
	waiting = 1;
    waiting = waiting * serial_divider * MCI_CLK_GEN2 * len;
    while (waiting--) {
	mci->regptr->cmdreg = MCI_CMD_RX_SETUP;
	sresult = mci->regptr->argreg;
	rlength = mci->regptr->rxlength;
	rstatus = (mci->regptr->rxstatus & MCI_SX_MASK);    
	if (sresult != 0xFFFF) {
	    if ((rlength == 0) || ((rstatus & MCI_SX_ABORT) == MCI_SX_ABORT)) {
                mci->regptr->cmdreg = MCI_CMD_RX_FLUSH;
		aborts++;
		}
	    else
		break;
	}
    }
    if (waiting <= 0){
	if (report)
	    printf("\nApplique Error: Packet sent, but not received!!!");
	return(FALSE);
    }
    
    if (sresult != in) {
	if (report)
	    printf("\nAppleque Error: Packet received on port%x in error!!!",
	        sresult);
	return(FALSE);
    }
    /*
     * Read packet if it comes back.
     */
    
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_RX_SELECT;
    for(i = 0; i <= len; i += 4)
        *rpacket++ = mci->regptr->readlong;
    mci->regptr->cmdreg = MCI_CMD_RX_FLUSH;
    if (rlength != len) {
	if (report) {
	    printf("\nApplique Error: packet length sent : %d, received %d",
	     	    len, rlength);
	    printf("\n                Argreg = 0x%x, receive status = 0x%x",
		    sresult, rstatus);
	}
        for (i = 0; i < len; i++) {
	    if (recvd[i] != sent[i]) {
		if (report)
		    printf("\n                Data Miscompare on byte %d, is 0x%x, SB 0x%x",
		            i, recvd[i], sent[i]);
		break;
	    }
        }
	return(FALSE);
    }
    if (rstatus != 0) {
	if (report)
	    printf("\nApplique Error: receive status = 0x%x, should be 0.",
	            rstatus);
	return(FALSE);
	}
    return(TRUE);
}

/*
 * ping_applique
 *
 * This routine sends a series of packets out the "out" port, expecting them
 * to come back on the "in" port.
 *
 * Parameters:
 * 	mci: interface board to use.
 * 	out: port to send the packets from.
 *	in:  port to receive the packets from.
 * 
 * Returns:
 *	True if things went OK.
 *	False if not.
 *
 */

#define MINPACKET 60
#define MAXPACKET 150

boolean ping_applique(mci, out, in)
    mcitype *mci;
    int out, in;
    
{
    uchar *tpacket;
    uchar *rpacket;
    int i, j;
    
    tpacket = packet1;
    rpacket = packet2;
    
    for(i = MINPACKET; i <= MAXPACKET; i++) {
        for(j = 0; j <= MAXPACKET; j++)
	    tpacket[j] = (uchar) (i + j);
	if(i == MINPACKET)
	    printf("\nApplique: send on %d - ", out);
	else if ((i & 0x3) == 0)
	    printf("!");

	if (!pingit(mci, out, in, tpacket, rpacket, i, TRUE))
	    return(FALSE);
        for (j = 0; j < i; j++) {
	    if (rpacket[j] != tpacket[j]) {
		printf("\nApplique Error: Data Miscompare on byte %d, is 0x%x, SB 0x%x",
		    i, rpacket[j], tpacket[j]);
	        return(FALSE);
	    }

	}
    }
    return(TRUE);
}

/*
 * ports_on
 *
 * Turns on the mci ports, sets clock and loopback if required.
 * 
 * Parameters:
 *  mci: interface board to use.
 *  port1, port2: mci interface ports to use.
 *  clock: port to set the clock on, -1 for none.
 *  divider: clock rate divider.
 *  loop: port to set into loopback, -1 for none.
 *
 */

void ports_on(mci, port1, port2, clock, divider, loop)
    mcitype *mci;
    int port1, port2;
    short clock, divider, loop;
{
    int result;

    mci->regptr->argreg = port1;
    mci->regptr->cmdreg = MCI_CMD_SELECT;
    mci->regptr->argreg = MCI_IF_NOLOOPBACK;
    mci->regptr->cmdreg = MCI_CMD_STATUS;
    mci->regptr->argreg = MCI_IF_DISABLE;
    mci->regptr->cmdreg = MCI_CMD_STATUS;
    mci->regptr->argreg = port2;
    mci->regptr->cmdreg = MCI_CMD_SELECT;
    mci->regptr->argreg = MCI_IF_NOLOOPBACK;
    mci->regptr->cmdreg = MCI_CMD_STATUS;
    mci->regptr->argreg = MCI_IF_DISABLE;
    mci->regptr->cmdreg = MCI_CMD_STATUS;
    if (clock >= 0) {
	mci->regptr->argreg = clock;
	mci->regptr->cmdreg = MCI_CMD_SELECT;
    	mci->regptr->argreg = divider;
    	mci->regptr->cmdreg = MCI_CMD_CLOCKRATE;
    	mci->regptr->argreg = MCI_CLK_GEN2;
    	mci->regptr->cmdreg = MCI_CMD_CLOCKTYPE;
    }
    mci->regptr->argreg = port1;
    mci->regptr->cmdreg = MCI_CMD_SELECT;
    mci->regptr->argreg = MCI_IF_ENABLE;
    mci->regptr->cmdreg = MCI_CMD_STATUS;
    mci->regptr->argreg = port2;
    mci->regptr->cmdreg = MCI_CMD_SELECT;
    mci->regptr->argreg = MCI_IF_ENABLE;
    mci->regptr->cmdreg = MCI_CMD_STATUS;
    if (loop >= 0) {
    	mci->regptr->argreg = loop;
    	mci->regptr->cmdreg = MCI_CMD_SELECT;
	mci->regptr->argreg = MCI_IF_LOOPBACK;
	mci->regptr->cmdreg = MCI_CMD_STATUS;
    }
}
	
/*
 * check_applique
 *
 * A good applique of the opposite sex is to be installed on the second
 * port.
 *
 * First finds out which M/SCI to use, then locates the first two ports.
 * It then checks the second port to see what kind of applique it has, waits
 * while the operator checks the lights, should be no clocks, loopback or
 * data, then checks normal data transmission, waits for the operator to
 * check that there are now data and clock lights, checks loopback, and waits
 * for the operator to verify that the loopback lights work.
 *
 */

void check_applique()
    
{
    int app1, app2;
    int port1, port2;
    int mcis_found, i;
    mcitype *mci;
    char buf[10];
    uchar *tpacket;
    uchar *rpacket;
    
    char  send;		/* Send status				*/
    char  recv;		/* Receive status			*/
    char  lcllp;	/* Loop Command local loop		*/
    char  lcrlp;	/* Loop Command remote loop		*/
    char  rcllp;	/* Receive Loop local loop		*/
    char  rcrlp;	/* Receive Loop remote loop		*/
    
    aborts = 0;
    alloc_diag_mem();

    tpacket = packet1;
    rpacket = packet2;

    for (i = 0; i < 100; i++)
    	tpacket[i] = i + 10;
    
    mcis_found = mci_findcontroller();
    if (mcis_found < 1)	{
	printf("\nError: No MCI's Found!!!\n");
	return;
    }
    mci = setup_unit();
    mci_reset(mci);
    mci_config(mci);
    port1 = -1;
    port2 = -1;
    
    for (i = 0; i < MCI_MAXUNITS; i++) {
	if (mci->interfacetype[i] == MCI_TYPE_SERIAL){
	    if (port1 < 0)
		port1 = i;
	    else {
		port2 = i;
		break;

	    }
	}
    }
    
    app1 = applique_type(mci,port1);
    app2 = applique_type(mci,port2);
    
    switch (app2) {
      case -2:
	printf("\nProgram Error: Port%d not serial!!\n", port2);
	return;
	break;

      case -1:
	printf("\nApplique Error: Unable to talk to port%d on MCI\n", port2);
	return;
	break;
	
      case 0:
      case 1:
	printf("\nApplique Error: Missing or broken applique on port%d\n", port2);
	return;
	break;
	
      case MCI_DCE_RS232:
	if (app1 != MCI_DTE_RS232) {
	    printf("\nApplique Error: Incorrect type, reports: %d, should be %d\n",
		   app1, MCI_DTE_RS232);
	    return;
	}
	else
	    printf("\n\nApplique: Checking RS232 DTE applique.\n");
	break;
	
      case MCI_DTE_RS232:
	if (app1 != MCI_DCE_RS232) {
	    printf("\nApplique Error: Incorrect type, reports: %d, should be %d\n",
		   app1, MCI_DCE_RS232);
	    return;
	}
	else
	    printf("\n\nApplique: Checking RS232 DCE applique.\n");
	break;
	
      case MCI_DCE_RS449:
	if (app1 != MCI_DTE_RS449) {
	    printf("\nApplique Error: Incorrect type, reports: %d, should be %d\n",
		   app1, MCI_DTE_RS449);
	    return;
	}
	else
	    printf("\n\nApplique: Checking RS449 DTE applique.\n");
	break;
	
      case MCI_DTE_RS449:
	if (app1 != MCI_DCE_RS449) {
	    printf("\nApplique Error: Incorrect type, reports: %d, should be %d\n",
		   app1, MCI_DCE_RS449);
	    return;
	}
	else
	    printf("\n\nApplique: Checking RS449 DCE applique.\n");
	break;
	
      case MCI_DCE_V35:
	if (app1 != MCI_DTE_V35) {
	    printf("\nApplique Error: Incorrect type, reports: %d, should be %d\n",
		   app1, MCI_DTE_V35);
	    return;
	}
	else
	    printf("\n\nApplique: Checking V.35 DTE applique.\n");
	break;
	
      case MCI_DTE_V35:
	if (app1 != MCI_DCE_V35) {
	    printf("\nApplique Error: Incorrect type, reports: %d, should be %d\n",
		   app1, MCI_DCE_V35);
	    return;
	}
	else
	    printf("\n\nApplique: Checking V35 DCE applique.\n");
	break;
	
      default:
	printf("\nApplique Error: Missing or broken applique on port%d\n", port2);
	return;
	break;
	
    }
    
    allocate_buffers(mci,port1,0,800,0,1,2);
    allocate_buffers(mci,port2,0,800,0,1,2);
    
    printf("\nCheck that all lights but power are off,");
    printf("\nthen press return or enter when ready.");
    gets(buf);
	
    if ((app1 == MCI_DTE_RS232) || (app1 == MCI_DCE_RS232))
	serial_divider = 32;
    else
/* Set clock back to 4 MB for Jim O'Neil.  10-22-90  BS */
/*	if ((app1 == MCI_DTE_RS449) || (app1 == MCI_DCE_RS449)) */
	    serial_divider = 1;
/*	else
	    serial_divider = 2; */

    switch (app1) {
	
      case MCI_DTE_RS232:
      case MCI_DTE_RS449:
      case MCI_DTE_V35:
	
	ports_on(mci, port1, port2, port2, serial_divider, -1);

	printf("\nCheck that the DTR and RTS lights are now on, then plug the");
	printf("\nappliques together and press return or enter when ready.");
        gets(buf);
	    
        printf("\nCheck that all lights but both loopbacks are on,");
        printf("\nthen press return or enter when ready.");
        gets(buf);
	
	if (ping_applique(mci, port1, port2))
	    send = 'P';
	else
	    send = 'F';

	if (ping_applique(mci, port2, port1))
	    recv = 'P';
	else
	    recv = 'F';

	ports_on(mci, port1, port2, port2, serial_divider, port1);

	/*					      
	 * This next instruction is to clear some errors that
	 *  sometimes happen when you switch loopback.
	 */

	pingit(mci, port1, port1, tpacket, rpacket, 10, FALSE);

	printf("\nCheck that all lights but DCE OK are now on,");
	printf("\nthen press return or enter when ready.");
        gets(buf);
	
	if(ping_applique(mci, port1, port1))
	    lcllp = 'P';
	else
	    lcllp = 'F';
	
	printf("\n\nFinal Status:\nSend Receive Loopback Aborts");
	printf("\n  %c     %c        %c       %d\n", send, recv, lcllp, aborts);
	
	break;
	
      case MCI_DCE_RS232:
      case MCI_DCE_RS449:
      case MCI_DCE_V35:
	
	ports_on(mci, port1, port2, port1, serial_divider, -1);

	if (app1 == MCI_DCE_RS449)
	    printf("\nCheck that the RXC, RXD, RTS and CTS lights are now on, then plug the");
	else
	    printf("\nCheck that the RXC, TXD, TXC, DTR and RTS lights are now on, then plug the");

	printf("\nappliques together and press return or enter when ready.");
        gets(buf);

        printf("\nCheck that all lights but both loopbacks are on,");
        printf("\nthen press return or enter when ready.");
        gets(buf);
	
	if (ping_applique(mci, port1, port2))
	    send = 'P';
	else
	    send = 'F';

	if (ping_applique(mci, port2, port1))
	    recv = 'P';
	else
	    recv = 'F';
	
	ports_on(mci, port1, port2, port1, serial_divider, port2);

	/*					      
	 * These next instructions are to clear some errors that
	 *  sometimes happen when you switch loopback.
	 */

	pingit(mci, port1, port1, tpacket, rpacket, 10, FALSE);
	pingit(mci, port2, port2, tpacket, rpacket, 10, FALSE);

	printf("\nCheck that all lights but DCE OK are now on,");
	printf("\nthen press return or enter when ready.");
        gets(buf);
	
	if(ping_applique(mci, port1, port1))
	    rcllp = 'P';
	else
	    rcllp = 'F';

	if(ping_applique(mci, port2, port2))
	    rcrlp = 'P';
	else
	    rcrlp = 'F';
	
	ports_on(mci, port1, port2, port1, serial_divider, port1);

	/*					      
	 * These next instructions are to clear some errors that
	 *  sometimes happen when you switch loopback.
	 */

	pingit(mci, port1, port1, tpacket, rpacket, 10, FALSE);
	pingit(mci, port2, port2, tpacket, rpacket, 10, FALSE);

	printf("\nCheck that only the DTE loopback and OK lights are now out,");
	printf("\nthen press return or enter when ready.");
        gets(buf);
	
	if(ping_applique(mci, port1, port1))
	    lcllp = 'P';
	else
	    lcllp = 'F';

	if(ping_applique(mci, port2, port2))
	    lcrlp = 'P';
	else
	    lcrlp = 'F';
	
	printf("\n\nFinal Status:");
	printf("\n             Remoteloopback  Localloopback ");
	printf("\nSend Receive  Near     Far    Near    Far  Aborts");
	printf("\n  %c     %c      %c        %c      %c       %c     %d\n",
	       send, recv, rcllp, rcrlp, lcllp, lcrlp, aborts);
	
	break;
    }
}
