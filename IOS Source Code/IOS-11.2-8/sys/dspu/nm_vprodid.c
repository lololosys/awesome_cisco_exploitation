/* $Id: nm_vprodid.c,v 3.2 1995/11/17 09:06:04 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/nm_vprodid.c,v $
 *------------------------------------------------------------------
 * SNA Network Management Product Major Vector Request Psid API
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nm_vprodid.c,v $
 * Revision 3.2  1995/11/17  09:06:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:30:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"    /* strlen */
#include "mv_datah.h"
#include "mv_parse.h"
#include "mv_sr.h"
#include "snautil.h"
#include "snanm_conf.h"


#define MV_RqstProdid 0x8090
#define MaxReplyLen   58
/*=====================================================================*
* VReceivedCmd
*======================================================================*
*
* Description:
*
*   Receive vital product command service
**/

static word VReceivedCmd (MVReceiver* this, MV* vector, hword vecLen,
              char* correlator, hword correlatorLength)
{
    DataHeader* head;
    const char* location = GetFpLocation();
    hword       len;
    byte replyProductSetId[MaxReplyLen];

    head = DataHeader_new();
    head->dataBytes = replyProductSetId;
    head->next = 0;
    len = strlen(location);
    if (len == 0){
       location = "unknown";
       len = 7;
    }
      
    head->dataLength = len + 8;
    replyProductSetId[0] = 0;
    replyProductSetId[1] = len + 8;    /* major vector len */
    replyProductSetId[2] = 0;
    replyProductSetId[3] = 0x90;      /* replyProductSet id mv */
    replyProductSetId[4] = len + 4;   /* Product Set attributes len */
    replyProductSetId[5] = 0x84;      /* Product Set attributes sub vec */
    replyProductSetId[6] = len + 2;   /* physical location len */
    replyProductSetId[7] = 0;         /* physical location sub vec */
    ConvertAsciiToEbcdic(location, &replyProductSetId[8], len);
    MV_SendVector(head, correlator,correlatorLength);
    DataHeader_deleteChain(head);       /* remove all data chain */
    return 0;
}

/*=====================================================================*
* VprodidInit
*======================================================================*
*
* Description:
*
*   Vital product API registration
**/

void VprodidInit (void)
{
   MV_OpRcv_RC rc = MV_SV_OK;
   MVR_MVReceiver(MV_RqstProdid, 0, VReceivedCmd, sizeof(MVReceiver), &rc);
}

