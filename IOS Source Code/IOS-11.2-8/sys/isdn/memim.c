/* $Id: memim.c,v 3.2.60.2 1996/04/30 23:08:00 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/memim.c,v $
 *------------------------------------------------------------------
 * memim.c  Management Entity Management Info Message Module
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: memim.c,v $
 * Revision 3.2.60.2  1996/04/30  23:08:00  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/09  02:34:57  rbeach
 * CSCdi53168:  SPIDS on an MBRI for 5ESS always get sent out BRI0.
 * The spid and eid were being sent out on BRI 0 for all the other
 * BRI interfaces. This was caused by an incorrect dsl value. This
 * only occurs with a switchtype of basic-5ess.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:42:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:14:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/25  11:44:23  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.1  1995/06/07 21:17:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       memim.c                                                 
DESCRIPTION:  Management Entity Management Info Message Module        
VERSION:      3.21                                                    

*************************************************************************
****copyright_c**********************************************************

                         COPYRIGHT STATEMENT

                        Copyright (c) 1991,92
                          by Telenetworks
                        Petaluma, CA   94952
                           (707) 778-8737

All rights reserved. Copying, compilation, modification, distribution
or any other use whatsoever of this material is strictly prohibited
except in accordance with a Software License Agreement with
Telenetworks.

****copyright_c**********************************************************
****history**************************************************************
****history**************************************************************
*************************************************************************


GENERAL NOTES



*************************************************************************
*************************************************************************
*/


#define MEMIM_C


/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/

/* Standard C library functions/definitions
*/
/* """
#include <stdio.h>
""" */

#include "master.h"

#include "flags.h"
#include "gendef.h"
#include "exec.h"
#include "execuser.h"
#include "lif.h"
/*===*/
#include "tune.h"
#include "gp.h"
#include "me.h"
#include "ccbri.h"

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/

/* Operation Classes - PAGE VI-5
*/
#define OC_UNCONF_OP                   0x00
#define OC_CONF_OP                     0x10
#define OC_RET_RES                     0x20
#define OC_RET_RES_CON                 0x30
#define OC_RET_ERR                     0x40
#define OC_REJECT                      0x50

/* Operation Types - PAGE VI-6
*/
#define OT_ACTION                      0x00
#define OT_EVENT_REP                   0x01
#define OT_GET                         0x02
#define OT_SET                         0x03

/* Management Operation Codes - PAGE VI-43
*/
#define MOC_INIT_REQ                   0x01
#define MOC_EPSSCH                     0x02
#define MOC_ACT_LOOPB                  0x03
#define MOC_DEACT_LOOPB                0x04
#define MOC_RESET                      0x08

/* Management Parameter Group Codes - PAGE VI-44
*/
#define MPGC_SERV_PROF_INFO            0x00
#define MPGC_ADDR_INFO                 0x01
#define MPGC_CALL_STAT_INFO            0x04

/* Management Error Codes - PAGE VI-44
*/
#define MEC_UNSPEC_ERROR               0x00
#define MEC_PROTO_VIOLATION            0x01
#define MEC_UNRECOG_OPER               0x02
#define MEC_ASYNC_NOT_SUPP             0x03

/* parameter identifier - PAGE VI-45
*/
#define PI_SERV_PROFILE                0x01
#define PI_ENDPOINT                    0x02
#define PI_CALL_REF                    0x08
#define PI_CALL_STATE                  0x09
#define PI_CHANNEL                     0x0a
#define PI_LOGICAL_LINK                0x0b
#define PI_SERV_STATE                  0x11
#define PI_SERV_MSG_ERR_CODE           0x12
#define PI_LOOPB_LOC                   0x13
#define PI_CAUSE                       0x30

/* to be resolved
*/
#define ERR_FAILED                     "failed"


/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/


/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/

PRIVATE char module[] = "memim";
PRIVATE H_ID hostcb;	/*===*/


/************************************************************************
*               F U N C T I O N    D E C L A R A T I O N S              *
*************************************************************************
*/

PRIVATE void   near mail               (int, int, int, int, int, PKT_ID);
PRIVATE int    near mim_unpack         (BYTE *, int *, int *, int *, int *);
PRIVATE PKT_ID near mk_ack_usid_tid    (int, int, int);
PRIVATE PKT_ID near mk_init_req        (BYTE *);
PRIVATE void   near send_init_req      (int, int, int, int);
PRIVATE void   near send_ack_usid_tid  (int, int, int, int, int, int);

/************************************************************************
*                   P R I V A T E    F U N C T I O N S                  *
*************************************************************************
*/

/*******
* mail *
********
*
*/
PRIVATE void near mail(dest, event, dslch, ces, call_id, pkt)

int dest;                                         /* receiving task id */
int event;                        /* the event which has just occurred */
int dslch;                       /* digital subscriber loop channel id */
int ces;                                 /* connection endpoint suffix */
int call_id;
PKT_ID pkt;                                          /* packet to send */

{
   int result;
   BYTE priority;

   /*
    * if this is a 5ess spid, bump the priority of the mail, so to make
    * sure it gets out before the Setup message.
    */
   if (event == MNL_MIM_REQ)
      priority = EXPRESS_MAIL;
   else
      priority = REGULAR_MAIL;


   result = LIF_SendPkt
      (
      0,                                      /* protocol - don't care */
      ME_ID,                                        /* source layer id */
      0,                                /* sender's state - don't care */
      dest,                                    /* destination layer id */
      event,                                          /* event to mail */
      dslch,                                /* digital subscriber loop */
      0,                       /* service access point id - don't care */
      ces,                               /* connection endpoint suffix */
      0,                               /* logical link id - don't care */
      call_id,
      0,                                    /* channel id - don't care */
      TRUE,                                            /* free packet? */
      pkt,                                           /* packet address */
      NOW,                                           /* delay in msecs */
      priority                                        /* mail priority */
      );

   if (result == ERR)
      LIF_Fatal(module, "mail", ERR_FAILED);
}


/*************
* mim_unpack *
**************
*
*/
PRIVATE int near mim_unpack(info, mieLen, opClass, opType, code)

BYTE *info;                                           /* incoming info */
int *mieLen;
int *opClass;
int *opType;
int *code;

{
   int i;

   i = 0;

   if (info[i++] != Qdot931) /* check for Q.931 protocol discriminator */
      goto err;
   if (info[i++] != 0x01)                     /* call reference length */
      goto err;
   if (info[i++] != 0x00)                            /* call reference */
      goto err;
   if (info[i++] != 0x00)  /* escape to specific message coding scheme */
      goto err;
   if (info[i++] != 0xf7)                              /* message type */
      goto err;
   if (info[i++] != 0x96)                /* locking shift to codeset 6 */
      goto err;
   if (info[i++] != 0x7a)                /* management info element id */
      goto err;

   *mieLen = info[i++];              /* management info element length */

   if (info[i++] != 0x03)         /* management protocol discriminator */
      goto err;
   if ((info[i++] & 0x7f) != 0x00)        /* must be zero. see pg VI-4 */
      goto err;

   *opClass = info[i] & 0x70;                              /* op-class */
   *opType = info[i++] & 0x03;                              /* op-type */
   *code = 0x7f & info[i++];                  /* management code field */

   return i;

   err:                                            /* non-fatal errors */

   return ERR;
}


/******************
* mk_ack_usid_tid *
*******************
*
*/
PRIVATE PKT_ID near mk_ack_usid_tid(usid, tid, epsf)

int usid;                                            /* user select id */
int tid;                                                /* terminal id */
int epsf;                                     /* endpoint service flag */

{
   PKT_ID pkt;
   BYTE *info;

   /* since this message type is infrequent, we will err on the side
    * of caution and allocate large info type instead of small one.
    */
   if ((pkt = LIF_GetPkt(LRG_INFO_BTYPE, DATA_OFFSET, FALSE)) == NULL)
      goto fatal;

   pkt->HeadLen = DATA_OFFSET;       /* leave space for layer 2 header */

   info = pkt->Info;

   *info++ = Qdot931;                  /* Q.931 protocol discriminator */
   *info++ = 0x01;                            /* call reference length */
   *info++ = 0x00;                                   /* call reference */
   *info++ = 0x00;         /* escape to specific message coding scheme */
   *info++ = 0xf7;                                     /* message type */
   *info++ = 0x96;                       /* locking shift to codeset 6 */

   *info++ = 0x7a;                       /* management info element id */
   *info++ = 0x08;                                    /* length of mie */

   *info++ = 0x03;                /* management protocol discriminator */
   *info++ = 0x80;                          /* transaction id, pg VI-4 */
   *info++ = 0x80 | OC_RET_RES | OT_SET;           /* op-class/op-type */
   *info++ = 0x81;                              /* address information */

   *info++ = 0x82;                                /* epid parameter id */
   *info++ = 0x02;                                   /* length of epid */
   *info++ = usid;                                     /* usid of epid */
   *info++ = (epsf ? 0x80 : 0x00) | tid;                /* tid of epid */

   pkt->InfoLen = info - pkt->Info;

   return pkt;

fatal:

   LIF_Fatal(module, "mk_ack_usid_tid", ERR_FAILED);
   return NULL;
   /*lint -unreachable */
}


/**************
* mk_init_req *
***************
*
*/
PRIVATE PKT_ID near mk_init_req(spid)

BYTE *spid;                                      /* service profile id */

{
   PKT_ID pkt;
   BYTE *info;
   int spidLen;                           /* service profile id length */
   int i;

   for (spidLen = 0; spid[spidLen]; ++spidLen)      /* get spid length */
      ;

   /* since this message type is infrequent, we will err on the side
    * of caution and allocate large info type instead of small one.
    */
   if ((pkt = LIF_GetPkt(LRG_INFO_BTYPE, DATA_OFFSET, FALSE)) == NULL)
      goto fatal;

   pkt->HeadLen = DATA_OFFSET;       /* leave space for layer 2 header */

   info = pkt->Info;

   *info++ = Qdot931;                  /* Q.931 protocol discriminator */
   *info++ = 0x01;                            /* call reference length */
   *info++ = 0x00;                                   /* call reference */
   *info++ = 0x00;         /* escape to specific message coding scheme */
   *info++ = 0xf7;                                     /* message type */
   *info++ = 0x96;                       /* locking shift to codeset 6 */

   *info++ = 0x7a;                       /* management info element id */
   *info++ = 0x06 + spidLen;                          /* length of mie */

   *info++ = 0x03;                /* management protocol discriminator */
   *info++ = 0x80;                          /* transaction id, pg VI-4 */
   *info++ = 0x80 | OC_UNCONF_OP | OT_EVENT_REP;   /* op-class/op-type */
   *info++ = 0x81;                           /* initialization request */

   *info++ = 0x81;                                /* spid parameter id */
   *info++ = spidLen;                                /* length of spid */
   for (i = 0; i < spidLen; ++i)               /* write spid parameter */
      *info++ = spid[i];

   pkt->InfoLen = info - pkt->Info;

   return pkt;

fatal:

   LIF_Fatal(module, "mk_init_req", ERR_FAILED);
   return NULL;
   /*lint -unreachable */
}


/****************
* send_init_req *
*****************
*
*/
PRIVATE void near send_init_req(dslch, specifier, ces, call_id)

int dslch;                       /* digital subscriber loop channel id */
int specifier;
int ces;                                 /* connection endpoint suffix */
int call_id;

{
   PKT_ID pkt;

   /*
    * Will will only send an initialization request if we have a spid,
    * make sure the values are within range.
    */
   if ((specifier == 0) && (ces == 1)) {
       if (! (np_fit[dslch][specifier].spid[0] == '\0')) {
           pkt = mk_init_req(np_fit[dslch][specifier].spid);
           mail(L3_ID, MNL_MIM_REQ, dslch, ces, call_id, pkt);
       }
   }
}

/********************
* send_ack_usid_tid *
*********************
*
*/
PRIVATE void near send_ack_usid_tid(dslch, ces, call_id, usid, tid, epsf)

int dslch;                       /* digital subscriber loop channel id */
int ces;                                 /* connection endpoint suffix */
int call_id;
int usid;                                            /* user select id */
int tid;                                                /* terminal id */
int epsf;                                     /* endpoint service flag */

{
   PKT_ID pkt;

   pkt = mk_ack_usid_tid(usid, tid, epsf);
   mail(L3_ID, MNL_MIM_REQ, dslch, ces, call_id, pkt);
}


/************************************************************************
*                   P U B L I C    F U N C T I O N S                    *
*************************************************************************
*/


/**********************
* ME_got_mnl_est_conf *
***********************
*
*/
PUBLIC void ME_got_mnl_est_conf(dslch, ces, call_id)

int dslch;                       /* digital subscriber loop channel id */
int ces;                                 /* connection endpoint suffix */
int call_id;

{
   /* === added the following code */
   int i = 0;
   int specifier;

   /* In this case there is a CCB active, in which case normal 
   *  terminal initialization activities are underway which have
   *  been initiated by the User by sending SPID.
   */ 
   if ( CCB )
      {   
      hostcb =  &Host_CB [Dsl_ID] [ CCB->Host_Specifier ];
	  specifier = CCB->Host_Specifier;
      }
   else
      { 
      /* Unexpected situation, we should have a CCB at this point
      */
	  specifier = ces;
      hostcb = &Host_CB [Dsl_ID] [ i ];
      while ( ( hostcb->Ces !=  ces ) && ( i < MAX_HOSTS ) )
         {
         i++;
         hostcb = &Host_CB [Dsl_ID] [ i ];
         }  
      } 
	/* === added the above code */
   send_init_req(dslch, specifier, ces, call_id);	/*===*/
}


/*********************
* ME_got_mnl_mim_ind *
**********************
*
*/
PUBLIC BOOLEAN ME_got_mnl_mim_ind(dslch, ces, call_id, pkt)

int dslch;                       /* digital subscriber loop channel id */
int ces;                                 /* connection endpoint suffix */
int call_id;
PKT_ID pkt;                                    /* info packet received */

{
   PKT_ID out_pkt;
   BOOLEAN rc;

   int mieLen, opClass, opType, code, i, specifier;

   rc = FALSE;
   /* In this case there is a CCB active, in which case normal 
   *  terminal initialization activities are underway which have
   *  been initiated by the User by sending SPID.
   */ 
   if ( CCB )
      {   
      hostcb =  &Host_CB [Dsl_ID] [ CCB->Host_Specifier ];
	  specifier = CCB->Host_Specifier;	/* === */
      }
   else
      /* In this case the NL_MIM_IND is unsolicited-we hope that sending
      *  back the "ack" satisfies the network.
      */ 
      { 
      i = 0;
      hostcb = &Host_CB [Dsl_ID] [ i ];
      while ( ( hostcb->Ces !=  ces ) && ( i < MAX_HOSTS ) )
         {
         i++;
         hostcb = &Host_CB [Dsl_ID] [ i ];
         /* Stop the timer in case its running 
         */  
         LIF_StopTimer ( hostcb->Timer_T415  );
         }   
      specifier = i;	/* === */
      if ( i >= MAX_HOSTS )
         goto err;        
      }

   if (pkt == NULL || pkt->Info == NULL)   /* check that there is info */
      goto fatal;

   if ((i = mim_unpack(pkt->Info, &mieLen, &opClass, &opType, &code)) == ERR)
      goto err;

   if (opClass == OC_UNCONF_OP && opType == OT_EVENT_REP && code == MOC_INIT_REQ)
      {
      /* We have received an initialization request.  See pg VI-54
       */
      /* send_init_req(dslch, ces, call_id);	=== */
      send_init_req(dslch, specifier, ces, call_id);
      }
   else if (opClass == OC_CONF_OP && opType == OT_SET && code == MPGC_ADDR_INFO)
      {
      /* We have received an assign usid tid message.
       *  See pg VI-46 and VI-55
       */
      if ((pkt->Info[i++] & 0x7f) != PI_ENDPOINT)          /* param id */
         goto err;
      if (pkt->Info[i++] != 2)                         /* check length */
         goto err;

      hostcb->epid.usid =  pkt->Info[i++];
      hostcb->epid.tid = 0x7f & pkt->Info[i];
      hostcb->epid.epsf = ((pkt->Info[i] & 0x80) != 0);
      hostcb->epid.valid = TRUE;
      send_ack_usid_tid(dslch, ces, call_id, hostcb->epid.usid, hostcb->epid.tid,
                        hostcb->epid.epsf);
      rc = TRUE; 
      }
   else
      {
      /* we will send the message back the same way we received it
      *  but will change op_class from 1 to 2
      */
      if ((out_pkt = LIF_GetPkt(LRG_INFO_BTYPE, DATA_OFFSET, FALSE)) == NULL)
         goto fatal;

      LIF_CpyPkt(out_pkt, pkt);
      /* IMPORTANT: LIF_CpyPkt has copied the InfoType of pkt to out_pkt. 
      *  This could have been SML_INFO_BTYPE. Make sure here that the 
      *  InfoType of out_pkt is properly set to LRG_INFO_BTYPE.
      */
      out_pkt->InfoType = LRG_INFO_BTYPE;

      if (out_pkt->Info[10] & 0x10)
         {
         out_pkt->Info[10] &= ~0x10;
         out_pkt->Info[10] |= 0x20;
         }
      else if (out_pkt->Info[10] & 0x20)
         {
         out_pkt->Info[10] &= ~0x20;
         out_pkt->Info[10] |= 0x10;
         }

      mail(L3_ID, MNL_MIM_REQ, dslch, ces, call_id, out_pkt);
      }

err:

   LIF_FreePkt(pkt);
   /* return;	=== */
   return ( rc );

fatal:

   LIF_Fatal(module, "got_mnl_mim_ind", ERR_FAILED);
   /*lint -unreachable */
   return ( rc );	/* === */
}
