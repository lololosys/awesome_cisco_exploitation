/* $Id: utils.h,v 1.1.8.1 1997/04/01 19:51:30 jchlin Exp $
 * $Source: /cscka/t1_neat/src/common/utils.h,v $
 *------------------------------------------------------------------
 * Common utilities.
 *
 * January 1997, Chiung-Hsi Jonathan Lin
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: utils.h,v $
 * Revision 1.1.8.1  1997/04/01 19:51:30  jchlin
 * o       CSCdi85287: busyout the channel after call is disconnected.
 * o       CSCdi91038: LS/GS is waiting 1 second to answer the IC to
 *                     avoid false signalling now.
 * o       CSCdi92824: Fix the race conditiion between CO and CSM during
 *                     disconnecting the modem calls.
 * o       CSCdj00011: Sending Ground Ring under GS provision when the time
 *                     slot is busyout.
 *
 * Revision 1.1  1997/01/03 02:06:10  jchlin
 * File placeholders for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 *      Set the status register to the passed value (used to reset after
 *      an set_ipl() call.
 */
#define reset_ipl(v)    {                               \
        ushort val = v;                                 \
        asm ("movw      %0,sr" : /* no outputs */ : "d" (val));         \
}

extern void stack_dump(char*);
extern ushort set_ipl(register ushort);
