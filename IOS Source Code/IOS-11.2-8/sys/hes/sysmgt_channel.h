/* $Id: sysmgt_channel.h,v 3.1 1997/08/02 20:57:59 corourke Exp $
 * $Source: /trunk/arkansas/cvs/Xsys/hes/sysmgt_channel.h,v $
 *------------------------------------------------------------------
 * sysmgt_channel.h - channel event types.
 *
 * July 1997, Chris O'Rourke
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 */
 

#define CHANNEL_IMPL_INCIDENT   1  /* Implicit Incident */
#define CHANNEL_BER_EXCEEDED    2  /* Bit-Error-Rate Exceeded */
#define CHANNEL_LINK_FAILED     3  /* Link Failed */
#define CHANNEL_NOS             4  /* Not Operational Sequence Recognized */
#define CHANNEL_SEQ_TO          5  /* Link Failure - Sequence Timeout */
#define CHANNEL_INVALID_SEQ     6  /* Link Failure - Invalid Sequence */


struct cip_linktype_ {
    ulong       slot;                  /* cip slot number */
    ulong       which;                 /* dtr_brd number  */   
    ulong       dtr_brd_status;
    ulong       dtr_brd_signal;
    ulong       link_incident_cause;
    } ;


typedef enum channel_event_code_ {
    CARD_LINK_FAILURE_EVENT,         /* Card Link Failure */
    MAX_CHANNEL_EVENTS
}channel_event_code_t;

typedef struct chan_link_event_ { 
        ushort          dbcb_flags;            /* dbcb flags*/
        ushort          pacb_flags;            /* pacb flags*/
        uchar           which;                 /* dtr_brd number  */
        uchar           link_incident_cause;   /* daughter board failure cause */
} chan_link_event_t;
