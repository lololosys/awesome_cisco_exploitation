/* $Id: mon_ralib.c,v 3.2.58.1 1996/03/21 23:32:20 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-rsp/mon_ralib.c,v $
 *------------------------------------------------------------------
 * mon_ralib.c
 *
 * Mar 95, Steve J. Zhang
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * A module to bridge the gap between the PCMCIA driver and 
 * ROM Monitor
 *------------------------------------------------------------------
 * $Log: mon_ralib.c,v $
 * Revision 3.2.58.1  1996/03/21  23:32:20  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:24:12  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:46:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/09  13:18:36  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:41:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "monitor.h"
#include "signal_4k.h"
#include "monlib.h"

/********************************************************
 * Temporary function
 ********************************************************
 */

void createlevel(int level, void (*fn)(void), char *str)
{
}


void process_sleep_for(ulong msecs)
{
}


int
slot0_size()
{
    return(0x1400000);
}

int
slot1_size()
{
    return(0x1400000);
}

/*
void 
monlib_vector_init(void)
{
   struct mon_iface *mifp = &m_iface;
   ralib_vector_table_t *ralib = &ralib_vector_table;

   mifp->ra_read = ralib_read;
   mifp->ra_write = ralib->write;
   mifp->ra_status = ralib->status;
   mifp->ra_control = ralib->control;
   mifp->ra_yield = (int (*)())ralib->yield;
   mifp->ra_time = (int (*)())ralib->time;
   mifp->ra_getdevidx = ralib->get_dev_num;
}
  
*/ 

