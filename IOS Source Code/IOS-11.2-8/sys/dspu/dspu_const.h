/* $Id: dspu_const.h,v 3.1 1995/11/09 11:28:02 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/dspu_const.h,v $
 *------------------------------------------------------------------
 * DSPU Constants.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * This defines commonly used constants not specific to any modules.
 *------------------------------------------------------------------
 * $Log: dspu_const.h,v $
 * Revision 3.1  1995/11/09  11:28:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/14  06:15:09  smackie
 * Pre-emptive strike on namespace confusion. Rename DSPU source files that
 * use generic names for specific tasks. (CSCdi40350)
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __DSPU_CONST_H__
#define __DSPU_CONST_H__


#define MAX_ENTRYNAME_LN  10            /* PU name                */
#define MAC_ADDR_LN       6             /* Length of mac address. */

#ifndef IEEEBYTES
#define IEEEBYTES         MAC_ADDR_LN
#endif

#define NoSap             0xff          /* sap number undefined */
#define NoXid             0L            /* xid undefined       */
#define BadQllcSubaddr    0xfe          /* invalid qllc sub address */
#define DEFAULT_SAP       8
#define DEFAULT_ACTWINDOW 5

#define INFINITE_MAX_RETRY_COUNT   0xff
#define INFINITE_MAX_RETRY_TIMEOUT 0xffff

#define UNDEFINED         0xffff

/*
 * Define maximum number of bytes to be displayed for I-frame trace data
 *
 * The character buffer length must be calculated as follows:
 *
 *   Each hex byte takes 2 characters 
 *     (2*MAX_IFRAME_TRC_DATA_LN)
 *
 *   After every 8 bytes, insert space character 
 *     (MAX_IFRAME_TRC_DATA_LN/8)
 *
 *   After every 32 bytes, insert newline & space characters 
 *     (2*MAX_IFRAME_TRC_DATA/32)
 *
 *   Add one additional character for termination
 *
 *    
 * Output is displayed as follows:
 *   Debug header...data:
 *    xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx 
 *    xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx 
 *    xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx 
 *    xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx 
 *
 */
#define MAX_IFRAME_TRC_DATA_LN   128

#define MAX_IFRAME_TRC_BUFFER_LN (2*MAX_IFRAME_TRC_DATA_LN) +      \
                                 (MAX_IFRAME_TRC_DATA_LN/8) +      \
                                 (2*MAX_IFRAME_TRC_DATA_LN/32) + 1

#endif  /* __DSPU_CONST_H__ */
/*---- End include file: dspu_const.h. */









