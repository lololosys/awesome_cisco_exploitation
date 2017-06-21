/* $Id: nim.c,v 3.2.58.1 1996/03/21 23:30:52 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-c4000/nim.c,v $
 *------------------------------------------------------------------
 * nim.c
 *
 * August 1993, Yvette Yuen
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * NIM polling routines.
 *------------------------------------------------------------------
 * $Log: nim.c,v $
 * Revision 3.2.58.1  1996/03/21  23:30:52  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:23:20  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:45:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "monitor.h"
#include "signal_4k.h"
#include "nim.h"
#include "error.h"
#include "mon_defs.h"

int slotindex[3]={0,1,2};
char illtypemsg[] = "Do not recognize interface type: %#x";
char vacantmsg[]  = "vacant ";

char *nimtypestr[] = { 
    "FDDI (Single Attach, No Bridge Hardware)",
    "FDDI (Dual Attach, No Bridge Hardware)",
    "FDDI (Single Attach, With Bridge Hardware)",
    "FDDI (Dual Attach, With Bridge Hardware)",
    "ETHERNET (LANCE Type)",
    "Token-Ring",
    "Single Serial I/O",
    "Dual Serial I/O",
    "Dual Ethernet, Single Port",
    "Dual Ethernet, Dual Port",
    "Dual Token-Ring, Single Port",
    "Dual Token-Ring, Dual Port",
    "Quad Serial",
    "Multi-channel BRI"
};
#define MAXNIMID (sizeof(nimtypestr)/sizeof(char *))
struct slotdata slotinfo[MAXSLOTS];

static int 
vacslot(void)
{
    cterr('w',0,"This slot is vacant. Can't perform any test.");
    return(1);
}

static int 
illid(void)
{
    cterr('f',0,"Found illegal ID on interface module. Please check interface card.");
    return(1);
    
}

static int 
notavail(void)
{
    cterr('w',0,"Test is not available for now. Sorry!");
    return(1);
}

/*
** checkslot : check interface on each slot then update the module information
**             structure.
*/
struct slotdata *
checkslot(struct slotdata *slot) 
{
    unsigned short *baseaddress;            /* interface */ 
    char i;
    unsigned short id;                      /* module type code */
    unsigned short idmask=0x00ff;
    SIG_PF savfcn;
    int test;
 
    savfcn=signal(SIGBUS,SIG_IGN);          /* set to ignore bus errors */
    baseaddress = (unsigned short *)KSEG1_ADRSPC_SLOT_1;

    for(i=0 ; i<MAXSLOTS ; i++) {
	test = chkberr(baseaddress, BW_16BITS, 1);
	if (test) {    			    /* slot is vacant */
	    slot[i].type      = vacantmsg;
	    slot[i].id        = VACCODE;
	    slot[i].nimfunc   = vacslot;
	    slot[i].nimfparam = (int *)&slotindex[0]; 
	} else {                            /* module occupy slot */
	    id = *baseaddress;		    /* read the NIM ID */
	    id &= idmask;                   /* get the ID number */
	    slot[i].id   = id;              /* update module info table */
	    if (id > MAXNIMID) {            /* module with illegal type code */
		slot[i].type      = illtypemsg;
		slot[i].nimfunc   = illid;
		slot[i].nimfparam = (int *)&slotindex[0]; 
	    } else {                        /* module with legal type code */
		slot[i].base = baseaddress;
		slot[i].type = nimtypestr[slot[i].id];
		slot[i].nimfunc   = notavail;
		slot[i].nimfparam = (int *)&slotindex[i]; 
	    }
	}
	slot[i].slotnum = i+1; 
	baseaddress     = (unsigned short *)((int)baseaddress + 
					     (int)SLOT_OFFSET);
    }
    signal(SIGBUS,savfcn);
    return(slot);            /* return the pointer to module info structure */
}

/* 
** checknim : to check interface module on each slot
*/
int 
checknims(void) 
{
    struct slotdata *slot;
    char i;

    reset_io();
    slot = checkslot(slotinfo);
    for (i=0 ; i<MAXSLOTS ; i++) {
	printf("slot%1d : ",i+1);
	printf(slot[i].type, slot[i].id);
	putchar('\n');
    }
    printf("Done poll NIMs test\n");
    return(0);
}

/*
** testnims : test NIMs on XX/Sierra platform
*/  
void
testnims(unsigned long slotnum)
{
    struct slotdata slot;

    testname("NIM at slot %1d",slotnum);
    reset_io();
    checkslot(slotinfo);                /* go check all slot types */ 
    slot = slotinfo[slotnum-1];         /* point to the proper slot 
					   information structure */
    (*slot.nimfunc)(*slot.nimfparam);   /* perform NIM test on this slot */
    prcomplete(testpass, errcount, 0);
}







