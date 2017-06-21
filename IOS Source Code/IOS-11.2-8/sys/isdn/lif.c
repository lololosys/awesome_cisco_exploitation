/* $Id: lif.c,v 3.2.60.2 1996/04/30 23:07:52 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/lif.c,v $
 *------------------------------------------------------------------
 * lif.c  MTEX interface functions common to all ISDN layers  
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lif.c,v $
 * Revision 3.2.60.2  1996/04/30  23:07:52  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  20:31:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:51:52  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  14:32:06  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:41:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:14:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:07:26  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/09  13:10:43  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:16:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       lif.c                                                   
DESCRIPTION:  MTEX interface functions common to all ISDN layers      
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

Layer InterFace gives LIFe to tired tasks.  Just kidding.  This module
contains functions to interface with the Real-time MultiTasing
EXecutive (MTEX) that are common to all ISDN layer tasks.


ORGANIZATION OF FUNCTIONS

The functions in this module are arranged alphabetically.

*************************************************************************
*************************************************************************
*/


#define     LIF_C                                       /* module name */

/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/
#include    "master.h"
#include    <stdlib.h>		/* for abs() */
#include    "flags.h"
#include    "gendef.h"
#include    "tune.h"
#include    "exec.h"
#include    "execuser.h"
#include    "lif.h"
#include    "../ui/debug.h"
#include    "../wan/isdn_debug.h"
#include    "cc_chan.h"
#include    "gp.h"

/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/

/*
 * Counters for the memory usage
 */
int Used_prim;
int Used_pkt;
int Used_header;
int Used_small;
int Used_lrg;
int Used_pkg;

/*
*  Tasks often have timers that need to be set often.  The
*  layer interface timer table stores task timer information (the
*  primitive to send as timed mail and the timer block and sequence #
*  associated with this timed mail).  The same primitive is used for a
*  particular timer; therefore, once the timer is 'added' (LIF_AddTimer),
*  it can be started (LIF_StartTimer) and stopped (LIF_StopTimer) as
*  often as required and no storage need be allocated/deallocated.
*/
typedef struct
   {
   MAILBOX_ID  DestMB;                  /* dest mailbox for timed mail */
   PRIM_ID     Prim;                           /* address of primitive */
   TIMER_ID    TimerBlock;           /* address of timer block in MTEX */
   INT16       Sn;                                       /* sequence # */
   UINT32      Ticks;                           /* length of timer === */
   BOOLEAN     Running;                           /* is timer running? */
   }
LIF_TIMER;                                        /* timer table entry */

PRIVATE LIF_TIMER LIF_TimerTable [LIF_MAX_TIMERS];      /* timer table */

#ifdef TRACING
PUBLIC unsigned LIF_Trace = 0xFFFF;       /* enable trace of all tasks */
#endif

/************************************************************************
*                              M A C R O S                              *
*************************************************************************
*/

/* Translate buffer type to MTEX memory type (an integer).
*/
#define LIF_BufToMem(btype) ((int)btype)

/************************************************************************
*               F U N C T I O N    D E C L A R A T I O N S              *
*************************************************************************
*/
PRIVATE void        LIF_PutPrim     (PRIM_ID, INT16, INT16, BYTE, \
                                          INT16, INT16, BYTE, BYTE, BYTE, \
                                          INT16, INT16, INT16, BOOLEAN, \
                                          PKT_ID);

static void update_mem_count (BUFFER_TYPE, int);

/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S                *
*************************************************************************
*/

/************************************************************************
* LIF_AddTimerP
*************************************************************************
*
* Add an entry to the timer table.
*
* Return: Index to timer table entry, if successful; ERR, otherwise.
*
*/
PUBLIC INT16 LIF_AddTimerP (source_id, dest_id, event, dsl_id, sapi, ces,
   call_id, chan_id, msecs, pkt)

INT16 source_id;                               /* source of timed mail */
INT16 dest_id;                            /* destination of timed mail */
INT16 event;                                     /* event to be mailed */
BYTE dsl_id;                                /* digital subscriber loop */
BYTE sapi;                                           /* sapi of sender */
BYTE ces;                                             /* ces of sender */
INT16 call_id;                                              /* call ID */
INT16 chan_id;                                           /* channel ID */
UINT32 msecs;                              /* length of timer in msecs */
PKT_ID pkt;                             /* packet to go with primitive */
{
   INT16 i;
   INT16 ret_code;
   BOOLEAN freepkt;
   LIF_TIMER *t;
   MAILBOX_ID dest_mb;

#ifdef DEBUG
   printf("LIF_AddTimer ()\n");
#endif

   ret_code = ERR;

   /* Check source
    */
   if (LIF_ID_To_MB (source_id) == NULL)
      {
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("lif", "LIF_AddTimer", "Unknown source task id.");
#endif
      goto Bailout;
      }

   /* Check destination
    */
   if ((dest_mb = LIF_ID_To_MB (dest_id)) == NULL)
      {
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("lif", "LIF_AddTimer", "Unknown destination task id.");
#endif
      goto Bailout;
      }

   for (i=0, t=(LIF_TIMER *)&LIF_TimerTable[0]; i<LIF_MAX_TIMERS; i++, t++)
      {
      if (t->Prim == NULL)                          /* available entry */
         {
         /* Prepare timer primitive for future uses
          */
         if ((t->Prim = LIF_GetPrim (TRUE)) == NULL)
            {
#ifdef DISPLAY_LOCAL_ERR
            LIF_Error ("lif", "LIF_AddTimer", "Can't get primitive.");
#endif
            }
         else
            {
            /* Pack the primitive (FFS - when a V120 layer 3 is
             *  implemented, LIF_AddTimer() will require an 'lli'
             *  parameter because links will be sorted on logical
             *  link id (LLI)).
             */
            if (pkt != NULL)
               freepkt = TRUE;
            else
               freepkt = FALSE;
            LIF_PutPrim (t->Prim, 0, source_id, 0, dest_id, event, dsl_id,
               sapi, ces, 0, call_id, chan_id, freepkt, pkt);
            t->DestMB = dest_mb;
            t->Ticks = msecs/SOFT_TICK_LEN;
            t->Running = FALSE;
            ret_code = i;
#ifdef DEBUG
            LIF_DispPrim (t->Prim);
#endif
            }
         break;
         }
      }

#ifdef MEM_STATS
   if (ret_code != ERR)
      {
      if (++LIF_UsedTimers > LIF_MaxTimers)
         LIF_MaxTimers = LIF_UsedTimers;
      }
#endif

Bailout:

   return (ret_code);
}

/************************************************************************
* LIF_Block
*************************************************************************
*
* Block and let another task run.
*
*/
PUBLIC void LIF_Block (source)
int source;                                          /* Task to block. */
{
   EXT_MEM    ExtMem;
   MAILBOX_ID mailbox_id;

   if (( mailbox_id = LIF_ID_To_MB( source )) == NULL )
      {
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("lif", "LIF_Block", "Unknown source.");
#endif
      }
   else
      {
      MTEX_SendMail( mailbox_id, DOWN, &ExtMem, REGULAR_MAIL);
      MTEX_ReadMail( mailbox_id, &ExtMem);
      }
}

/************************************************************************
* LIF_CpyPkt
*************************************************************************
*
* Copy a packet.  Assume storage allocated for packet AND for any
* 'payload' (header and/or info).
*
*/
PUBLIC void LIF_CpyPkt (d, s)

PKT_ID d;                                        /* destination packet */
PKT_ID s;                                             /* source packet */
{
   d->HeadLen = s->HeadLen;
   d->InfoLen = s->InfoLen;
   d->InfoType = s->InfoType;

   if (s->Header)
      LIF_N_Cpy (d->Header, s->Header, s->HeadLen);
   if (s->Info)
      LIF_N_Cpy (d->Info, s->Info, s->InfoLen);
}

/************************************************************************
* LIF_CpyPrim
*************************************************************************
*
* Copy a primitive.  Assume storage allocated for primitive BUT NOT for
* any packet.
*
* Return: NOERR, if successful; ERR, otherwise.
*
*/
PUBLIC INT16 LIF_CpyPrim (d_prim, s_prim)

PRIM_ID d_prim;                               /* destination primitive */
PRIM_ID s_prim;                                    /* source primitive */
{
   INT16 ret_code;

   ret_code = NOERR;

   LIF_N_Cpy((BYTE *)d_prim, (BYTE *)s_prim, LIF_GetBufSize(PRIM_BTYPE));

   if (s_prim->Pkt)
      {
      if ((d_prim->Pkt = LIF_GetPkt (s_prim->Pkt->InfoType,
         s_prim->Pkt->Info - s_prim->Pkt->Header, FALSE)) == NULL)
         {
#ifdef DISPLAY_LOCAL_ERR
         LIF_Error ("lif", "LIF_CpyPrim", "Can't get packet.");
#endif
         ret_code = ERR;
         }
      else
         {
         LIF_CpyPkt (d_prim->Pkt, s_prim->Pkt);
         }
      }

   return (ret_code);
}

/************************************************************************
* LIF_Dispose
*************************************************************************
*
* Dispose of any recognized mail type.
* At present, only recognizes PKG_ID, SBC_ID, PRIM_ID
*
*/
PUBLIC void LIF_Dispose (ptr)
void *ptr;
{
   if (((PRIM_ID) ptr)->BufType == PRIM_BTYPE) {
      if (((PRIM_ID) ptr)->FreePkt)
         LIF_FreePkt (((PRIM_ID) ptr)->Pkt);
      LIF_FreePrim ((PRIM_ID) ptr);
   } else if (((PKG_ID) ptr)->BufType == PKG_BTYPE) {
      LIF_FreePkg((PKG_ID) ptr);
   } else {
      LIF_Error("LIF", "LIF_Dispose", "Unrecognized Buffer Type");
   }
}

#ifndef NO_PKG
#ifdef DEBUG
/************************************************************************
* LIF_DispPkg
*************************************************************************
*
* Display the contents of a package.
*
*/
PUBLIC void LIF_DispPkg(Pkg)

PKG_ID Pkg;                                      /* package to display */
{
   int i;

   printf("----> PACKAGE <----\n");
   printf("  BufType=%d  SourceID=%d  DestId=%d InfoLen=%d  InfoBufType=%d  MsgType=%d\n",
      Pkg->BufType, Pkg->SourceID, Pkg->DestID, Pkg->InfoLen,
      Pkg->InfoBufType, Pkg->MsgType);

   printf("<< Info(%d)>>", Pkg->InfoLen);
   for (i=0; i<Pkg->InfoLen; i++)                              /* info */
      printf(" %02x ",Pkg->PkgInfo[i]);
   printf("<<\n");
}
#endif
#endif                                                       /* NO_PKG */

#ifdef DEBUG
/************************************************************************
* LIF_DispPkt
*************************************************************************
*
* Display the contents of a packet.
*
*/
PUBLIC void LIF_DispPkt (p)

PKT_ID p;                                         /* packet to display */
{
   int i;

   printf("----> PACKET <----\n");
   printf("  Header (%d) >>", p->Header);
   for (i=0; i<p->HeadLen; i++)                              /* header */
      printf(" %02x ",p->Header[i]);
   printf("<<   Info (%d) >>", p->Info);
   for (i=0; i<p->InfoLen; i++)                                /* info */
      printf(" %02x ",p->Info[i]);
   printf("<<\n");
}
#endif

#ifdef DEBUG
/************************************************************************
* LIF_DispPrim
*************************************************************************
*
* Display the contents of a primitive.
*
*/
PUBLIC void LIF_DispPrim (prim)

PRIM_ID prim;                                  /* primitive to display */
{
   int i;

   printf("----> PRIMITIVE <----\n");
   printf("  BufType=%d  L2_Protocol=%d\n", prim->BufType, prim->L2_Protocol);
   printf("  Source_ID=%d  SourceState=%d  Dest_ID=%d  Prim_ID=%x\n",
      prim->Source_ID, prim->SourceState, prim->Dest_ID, prim->Prim_ID);
   printf("  Sapi=%d  Ces=%d  LLI=%d  Call_ID=%d  DSL_Chan_ID=%d\n",
      prim->Sapi, prim->Ces, prim->LLI, prim->Call_ID, prim->DSL_Chan_ID);

   if (prim->Pkt)
      {
      printf("  Packet= Head(%d)>>", prim->Pkt->Header);
      for (i=0; i<prim->Pkt->HeadLen; i++)                   /* header */
         printf(" %02x ",prim->Pkt->Header[i]);
      printf("<< Info(%d)>>", prim->Pkt->Info);
      for (i=0; i<prim->Pkt->InfoLen; i++)                     /* info */
         printf(" %02x ",prim->Pkt->Info[i]);
      printf("<<\n");
      }
}
#endif

/************************************************************************
* LIF_EnterCS
*************************************************************************
*
* Enter a critical section.
*
*/
PUBLIC void LIF_EnterCS ()

{
   EnterCS();
}

/************************************************************************
* LIF_EnterICS
*************************************************************************
*
* Enter a critical section while in an ISR.
*
*/
PUBLIC void LIF_EnterICS ()

{
   EnterICS();
}

/************************************************************************
* LIF_EnterNPS
*************************************************************************
*
* Enter a non-preemptive section.
*
*/
PUBLIC void LIF_EnterNPS ()

{
   EnterNPS();
}

#ifndef NO_LIF_ERROR
/************************************************************************
* LIF_Error
*************************************************************************
*
* Handle error condition.
*
* Note that under compile time flag NO_LIF_ERROR, this function is
* assumed to be defined elsewhere.  This allows the method of
* message display to be handled in different ways (for example,
* see tracker.c).
*
*/
PUBLIC void LIF_Error (mod, fn, err_msg)
char *mod;                              /* module where error occurred */
char *fn;                             /* function where error occurred */
char *err_msg;                                        /* error message */
{
#ifdef GLOBAL_DEBUG
   buginf("\nISDN ERROR:  Module-%s  Function-%s  Error-%s", mod, fn, err_msg);
   flush();
#endif
}
#endif                                                 /* NO_LIF_ERROR */

/************************************************************************
* LIF_ExitCS
*************************************************************************
*
* Exit a critical section.
*
*/
PUBLIC void LIF_ExitCS ()

{
   ExitCS();
}

/************************************************************************
* LIF_ExitICS
*************************************************************************
*
* Exit a critical section while in an ISR.
*
*/
PUBLIC void LIF_ExitICS ()

{
   ExitICS();
}

/************************************************************************
* LIF_ExitNPS
*************************************************************************
*
* Exit a non-preemptive section.
*
*/
PUBLIC void LIF_ExitNPS ()

{
   ExitNPS();
}

/************************************************************************
* LIF_Fatal
*************************************************************************
*
* Just die.
*
*/
PUBLIC void LIF_Fatal(file, func, mess)

char *file;                         /* file module where error occured */
char *func;                            /* function where error occured */
char *mess;                                           /* error message */
{
    SAVE_CALLER();

#ifdef DISPLAY_LOCAL_ERR
#ifdef GLOBAL_DEBUG	/* === */
   LIF_Error(file, func, mess);
#endif				/* === */
#endif
   buginf("\nLIF_Fatal called from %s 0x%x, func = %s, string = %s",
          file, caller(), func, mess);
   ExecExit();
}

/************************************************************************
* LIF_FreeBuffer
*************************************************************************
*
* Deallocated a buffer.
*
*/
PUBLIC void LIF_FreeBuffer (type, b)
BUFFER_TYPE type;   /* buffer type */
BYTE *b;            /* buffer addr */
{
    /*
     * never return a NULL buffer
     */
     if (b) {
        free(b);
        update_mem_count(type, MINUS_ONE);
    }
}

#ifndef NO_PKG
/************************************************************************
* LIF_FreePkg
*************************************************************************
*
* Deallocate a package.
*
*/
PUBLIC void LIF_FreePkg (Pkg)
PKG_ID  Pkg;                                        /* package to free */
{
   EXT_MEM GenAddr;

   if (Pkg) {                            /* never return a NULL buffer */
      if (Pkg->PkgInfo) {                /* never return a NULL buffer */
         free(Pkg->PkgInfo);
         update_mem_count(Pkg->InfoBufType, MINUS_ONE);
      }
      GenAddr.Addr = (BYTE *) Pkg;
      free(GenAddr.Addr);
      Used_pkg--;
  }
}
#endif                                                       /* NO_PKG */

/************************************************************************
* LIF_FreePkt
*************************************************************************
*
* Deallocated a packet.
*
*/
PUBLIC void LIF_FreePkt (p)
PKT_ID p;                                            /* packet to free */
{
   EXT_MEM gen_addr;

   if (p) {                              /* never return a NULL buffer */
      if (p->Header) {                   /* never return a NULL buffer */
         free(p->Header);
         Used_header--;
      }
      gen_addr.Addr = (BYTE *) p;
      free(gen_addr.Addr);
      Used_pkt--;
   }
}

/************************************************************************
* LIF_FreePrim
*************************************************************************
*
* De-allocate storage for a primitive.
*
*/
PUBLIC void LIF_FreePrim (prim)
PRIM_ID prim;                             /* addr of primitive to free */
{
   EXT_MEM gen_addr;                         /* generic memory address */ 

   if (prim) {                           /* never return a NULL buffer */
      gen_addr.Addr = (BYTE *) prim;
      free(gen_addr.Addr);
      Used_prim--;
   }
}

/************************************************************************
* LIF_GetBuffer
*************************************************************************
*
* Get a buffer from the given memory pool.
*
* Return: address of buffer, if successful; NULL, otherwise.
*
*/
PUBLIC BYTE *LIF_GetBuffer (buffer_type, zero)
BUFFER_TYPE buffer_type;                      /* type of buffer to get */
BOOLEAN zero;                              /* initialize storage to 0? */
{
   EXT_MEM gen_addr;                          /* address of new buffer */
   int size;

   size = LIF_GetBufSize(buffer_type);
   gen_addr.Addr = malloc_named(size, "isdn-buffer");
   if (gen_addr.Addr == NULL) {
      LIF_Error ("lif", "LIF_GetBuffer", "Can't get block.");
   } else {
      update_mem_count(buffer_type, PLUS_ONE);
   }
   return (gen_addr.Addr);
}

/************************************************************************
* LIF_GetBufSize
*************************************************************************
*
* Get the length of a buffer from the buffer type.
*
* Return: length of buffer.
*
*/
PUBLIC INT16 LIF_GetBufSize (buffer_type)
BUFFER_TYPE buffer_type;                  /* type of buffer to examine */
{
   int size;
   switch (buffer_type) {
       case LRG_INFO_BTYPE:
           size = LRG_INFO_LEN;
           break;
       case PRIM_BTYPE:
           size = BLKSIZE(sizeof(struct Primitive));
           break;
       case PKT_BTYPE:
           size = BLKSIZE(sizeof(struct Packet));
           break;
       case HEADER_BTYPE:
           size = HEADER_LEN;
           break;
       case SML_INFO_BTYPE:
           size = SML_INFO_LEN;
           break;
       case PKG_BTYPE:
           size = (sizeof(PKG));
           break;
       default:
           size = LRG_INFO_LEN;
           break;
   }
   return (size);
}

#ifdef TASK_L2
/************************************************************************
* LIF_GetDriverID
*************************************************************************
*
* Map the parameters of a message to be sent out over a Layer 2 HDLC
* driver to the driver's task ID.
*
* This code is functionally part of L2_Mail but is inserted here for
* user customization.  As the characterization of the HDLC driver(s) is
* exceedingly dependent upon the application, it is left to the user to
* modify this function as required.  It is assumed here, by default, that
* a single DSL with a 2B+D configuration exists.  Further HDLC message
* rerouting may be performed through modification of LIF_ID_To_MB.
*
*/
PUBLIC int LIF_GetDriverID (proto, dsl_id, dsl_chan_id)

int proto;                                            /* HDLC protocol */
int dsl_id;                  /* Digital Subscriber Loop (interface) ID */
int dsl_chan_id;        /* D or B channel over which msg is being sent */
   {
#ifdef TASK_V3
   static int id_list [] =
      {
      HDLC_D_ID, HDLC_B1_ID, HDLC_B2_ID
      };

   /*
    * check the array bounds
    */
   if ((unsigned) dsl_chan_id > DIM (id_list))
      LIF_Fatal ("lif", "LIF_GetDriverID", "bad dsl channel id");

   return id_list [dsl_chan_id];
#else
   int dest;                                    /* Destination task ID */

   /* No use is currently made of this parameter although any application
   *  using multiple DSLs must consider it.  "Using" it here prevents a
   *  compiler warning for a non-referenced parameter.
   */
   Trashcan = dsl_id;

   switch ( proto )
      {
      case L2P_LAPD:
         dest = HDLC_D_ID;
         break;
      case L2P_LAPB:
         if ( dsl_chan_id == 1 )
            dest = HDLC_B1_ID;
         else if ( dsl_chan_id == 2 )
            dest = HDLC_B2_ID;
         else
            dest = 0;                            /* Error case for now */
         break;
      case L2P_V120:
         dest = HDLC_V_ID;
         break;
      default:
         dest = 0;                                       /* Error case */
      }
   return (dest);
#endif
   }
#endif

#ifndef NO_PKG
/************************************************************************
* LIF_GetPkg
*************************************************************************
*
* Get a package header.
*
* Return:   Address of package, if successful; NULL, otherwise.
*
*/
PUBLIC PKG_ID LIF_GetPkg ()
{
   PKG_ID  p;                                       /* package to pack */

   /*
    * Get package
    */
   p = malloc_named(sizeof(PKG), "isdn-pkg");
   if (p == NULL) {
       LIF_Error("lif", "LIF_GetPkg", "Can't get buffer.");
       goto Bailout;
   }
   Used_pkg++;
   p->BufType = PKG_BTYPE;                   /* identify the mail type */

Bailout:
   return (p);
}
#endif                                                       /* NO_PKG */

/************************************************************************
* LIF_GetPkt
*************************************************************************
*
* Get any storage a packet requires and assign its fields.
*
* Return:   Address of packet, if successful; NULL, otherwise.
*
*/
PUBLIC PKT_ID LIF_GetPkt (type, offset, zero)

BUFFER_TYPE type;                            /* type of info requested */
INT16 offset;                         /* offset to info part of packet */
BOOLEAN zero;                              /* initialize storage to 0? */
{
   PKT_ID p;                                         /* packet to pack */

   /*
    * Get packet
    */
   p = malloc_named(sizeof(PKT), "isdn-pkt");
   if (p == NULL) {
      LIF_Error ("lif", "LIF_GetPkt", "Can't get packet.");
      goto Bailout;
   }
   Used_pkt++;
   p->InfoType = type;
   p->HeadLen = offset;

   /*
    * Get header/info
    */
   p->Header = malloc_named(LIF_GetBufSize(type), "isdn-buffer");
   if (p->Header == NULL) {
      LIF_FreeBuffer (PKT_BTYPE, (BYTE *) p);
      LIF_Error ("lif", "LIF_GetPkt", "Can't get buffer.");
      p = NULL;
      goto Bailout;
   }
   Used_header++;
   if (type != HEADER_BTYPE)
      p->Info = p->Header + offset;                        /* set info */
   else
      p->Info = NULL;

Bailout:
   return (p);
}

/************************************************************************
* LIF_GetPrim
*************************************************************************
*
* Allocate (and, optionally, clear) storage for a primitive.
*
* Return:   Address of new primitive, if successful; NULL, otherwise.
*
*/
PUBLIC PRIM_ID LIF_GetPrim (zero)

BOOLEAN zero;                              /* initialize storage to 0? */
{
   PRIM_ID prim;

   prim = malloc_named(sizeof(PRIM), "isdn-prim");
   if (prim == NULL) { 
      LIF_Error ("lif", "LIF_GetPrim", "Can't get primitive.");
   } else {
      Used_prim++;
      prim->BufType = PRIM_BTYPE;
   }
   return (prim);
}

/************************************************************************
* LIF_ID_To_MB
*************************************************************************
*
* Translate task id to MTEX mailbox.
*
* Return: MTEX mailbox address if task exists, NULL if not.
*
*/
PUBLIC MAILBOX_ID LIF_ID_To_MB (id)
INT16 id;        /* layer id to translate */
{
    MAILBOX_ID mb;

    switch (id) {
        case HOST_ID:
            mb = HOST_MB;
            break;

        case ME_ID:
            mb = ME_MB;
            break;

        case CC_ID:
            mb = CC_MB;
            break;

        case L3_ID:
            mb = L3_MB;
            break;

        case L2_ID:
            mb = L2_MB;
            break;

        case HDLC_D_ID:
            mb = HDLC_D_MB;
            break;

        default:
            mb = NULL;
            break;
    }
    return (mb);
}

/************************************************************************
* LIF_Init
*************************************************************************
*
* Initialize the switch type for this application and the Timer Table.
*
*/
PUBLIC void LIF_Init ()
{
    INT16 i;
    LIF_TIMER *t;

    /*
     *  Initialize Timer Table
     */
    for (i=0, t=(LIF_TIMER *)&LIF_TimerTable[0]; i<LIF_MAX_TIMERS; i++, t++) {
        t->DestMB = NULL;
        t->Prim = NULL;
        t->TimerBlock = NULL;
        t->Sn = 0;
        t->Ticks = 0;
        t->Running = FALSE;
    }

    LIF_UsedTimers = 0;
    LIF_MaxTimers = 0;

    switch (SwitchType) {
        case BRI_DMS100_STYPE:
        case BRI_NI1_STYPE:
        case BRI_5ESS_STYPE:
            FitTerm = TRUE;
            break;

        default:
            FitTerm = FALSE;
            break;
    }

    for (i=0; i<MAX_HOSTS; i++) {
        np_fit[0][i].Bearer_Type  = 2;
        np_fit[0][i].Bearer_Routing = FALSE;
        np_fit[0][i].Host_ID        = HOST_ID;
        np_fit[0][i].Ces        = i+1;
        np_fit[0][i].Initializing_Terminal = TRUE;
    }
}

#ifndef NO_PKG
/************************************************************************
* LIF_MailPkg 
*************************************************************************
*
* Mail a message package.
*/
PUBLIC INT16 LIF_MailPkg( pkg, msecs, priority, dest_map )

PKG_ID  pkg;                                        /* package to send */
UINT32  msecs;  /* if != 0, send delayed mail with this delay in msecs */
BYTE    priority;                     /* mail priority (0 through 255) */
unsigned dest_map;                 /* bitmap: determines where to send */
{
   MAILBOX_ID dest_mb;
   EXT_MEM gen_addr;
   INT16 ret_code;
   TIMER_ID tid;                                      /* MTEX timer id */
   INT16 sn;                                       /* MTEX timer seq # */

   ret_code = ERR;                                 /* assume the worst */

#ifdef TRACING
   /* don't trace delayed mail or on explicit request not to
   */
   if ((!msecs) && (dest_map & MAIL_TO_TRACER))
      {
      if (LIF_TraceOk(pkg->SourceID, pkg->DestID))
         {
         if (LIF_TracePkg(pkg) == ERR)
            goto Bailout;
         }
      }
#endif

   dest_mb = LIF_ID_To_MB (pkg->DestID);
   if (((dest_map & MAIL_TO_DEST) == 0) || (dest_mb == NULL))
      {
      LIF_Dispose(pkg);
#ifndef TASK_TRACER
#ifndef REMOTE_TRACER
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("lif", "LIF_MailPkg", "Unknown destination task id.");
#endif
      goto Bailout;
#endif
#endif
      }
   else
      {
      gen_addr.Addr = (BYTE *) pkg;
      if (msecs)
         {
         if (MTEX_SetTimer (dest_mb, &gen_addr,
            (UINT32) (msecs / SOFT_TICK_LEN), &tid, &sn) == ERR)
            {
#ifdef DISPLAY_LOCAL_ERR
            LIF_Error ("lif", "LIF_MailPkg", "MTEX_SetTimer failed.");
#endif
            goto Bailout;
            }
         }
      else
         {
         if (MTEX_SendMail (dest_mb, ((pkg->SourceID > pkg->DestID) ?
               DOWN : UP), &gen_addr, priority) == ERR)
            {
#ifdef DISPLAY_LOCAL_ERR
            LIF_Error ("lif", "LIF_MailPkg", "MTEX_SendMail failed.");
#endif
            goto Bailout;
            }
         }
      }

   ret_code = NOERR;

Bailout:

   return(ret_code);
}
#endif                                                /* ifndef NO_PKG */

/************************************************************************
* LIF_MailPrim
*************************************************************************
*
* Mail a message primitive.  This function is provided specifically
* for access to the mail machinery by TaskView (and others who have
* already built a Primitive, and do not want to disassemble it in
* order to use LIF_SendPkt).
*/
PUBLIC INT16 LIF_MailPrim( p, source_id, dest_id, msecs, priority, dest_map )

PRIM_ID p;                                        /* primitive to send */
INT16   source_id;                                  /* source layer id */
INT16   dest_id;                               /* destination layer id */
UINT32  msecs;  /* if != 0, send delayed mail with this delay in msecs */
BYTE    priority;                     /* mail priority (0 through 255) */
unsigned dest_map;                 /* bitmap: determines where to send */
{
   MAILBOX_ID dest_mb;
   EXT_MEM gen_addr;
   INT16 ret_code;
   TIMER_ID tid;                                      /* MTEX timer id */
   INT16 sn;                                       /* MTEX timer seq # */

   ret_code = ERR;                                 /* assume the worst */

#ifdef TRACING
   /* don't trace delayed mail or on explicit request not to
   */
   if ((!msecs) && (dest_map & MAIL_TO_TRACER))
      {
      if (LIF_TraceOk(source_id, dest_id))
         {
         if (LIF_TracePrim(p) == ERR)
            goto Bailout;
         }
      }
#endif

   dest_mb = LIF_ID_To_MB (dest_id);
   if (((dest_map & MAIL_TO_DEST) == 0) || (dest_mb == NULL))
      {
      LIF_Dispose(p);
#ifndef TASK_TRACER
#ifndef REMOTE_TRACER
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("lif", "LIF_MailPrim", "Unknown destination task id.");
#endif
      goto Bailout;
#endif
#endif
      }
   else
      {
      gen_addr.Addr = (BYTE *) p;
      if (msecs)
         {
         if (MTEX_SetTimer (dest_mb, &gen_addr,
            (int) (msecs / SOFT_TICK_LEN), &tid, &sn) == ERR)
            {
#ifdef DISPLAY_LOCAL_ERR
            LIF_Error ("lif", "LIF_MailPrim", "MTEX_SetTimer failed.");
#endif
            goto Bailout;
            }
         }
      else
         {
         if (MTEX_SendMail (dest_mb, ((source_id > dest_id) ? DOWN : UP), &gen_addr, priority) == ERR)
            {
#ifdef DISPLAY_LOCAL_ERR
            LIF_Error ("lif", "LIF_MailPrim", "MTEX_SendMail failed.");/*===*/
#endif
            goto Bailout;
            }
         }
      }

   ret_code = NOERR;

Bailout:

   return(ret_code);
}

/************************************************************************
* LIF_MailSlotsAvail
*************************************************************************
*
*  Returns the number of open mail slots between a source and destination
*  task.  Returns ERR if the destination task does not exist.
*/
PUBLIC INT16 LIF_MailSlotsAvail(sid, did)

INT16 sid;                                           /* source task id */
INT16 did;                                      /* destination task id */
{
   MAILBOX_ID mb;

   mb = LIF_ID_To_MB(did);
   return (mb == NULL) ? ERR : TestMailAvail(mb, (sid > did) ? DOWN :
      UP);
}

/************************************************************************
* LIF_MailSlotsLimit
*************************************************************************
*
*  Returns the maximum number of mail slots between a source and destination
*  task.  Returns ERR if the destination task does not exist.
*/
PUBLIC INT16 LIF_MailSlotsLimit(sid, did)

INT16 sid;                                           /* source task id */
INT16 did;                                      /* destination task id */
{
   MAILBOX_ID mb;

   mb = LIF_ID_To_MB(did);
   return (mb == NULL) ? ERR : TestMailLimit(mb, (sid > did) ? DOWN :
      UP);
}

/************************************************************************
* LIF_N_Cpy
*************************************************************************
*
* Copy n bytes from source to destination
*
*/
PUBLIC void LIF_N_Cpy (d, s, n)

BYTE *d;                                                /* destination */
BYTE *s;                                                     /* source */
INT16 n;                                    /* number of bytes to copy */
{
   int i;

   for (i=0; i<n; i++)
      *d++ = *s++;
}

/************************************************************************
* LIF_PutPrim
*************************************************************************
*
* Pack a primitive.
*
*/
PRIVATE void LIF_PutPrim (prim, proto, source_id, state, dest_id, event,
   dsl_id, sapi, ces, lli, call_id, chan_id, free,
   pkt)

PRIM_ID prim;                                     /* primitive to pack */
INT16 proto;                                               /* protocol */
INT16 source_id;                                          /* source ID */
BYTE state;                                            /* sender state */
INT16 dest_id;                                       /* destination ID */
INT16 event;                                                  /* event */
BYTE dsl_id;                                /* digital subscriber loop */
BYTE sapi;                                                     /* SAPI */
BYTE ces;                                                       /* CES */
INT16 lli;                                          /* logical link id */
INT16 call_id;                                              /* call ID */
INT16 chan_id;                                           /* channel ID */
BOOLEAN free;                                    /* OK to free packet? */
PKT_ID pkt;                                       /* address of packet */
{
   prim->L2_Protocol  = proto;
   prim->Source_ID    = source_id;
   prim->SourceState  = state;
   prim->Dest_ID      = dest_id;
   prim->Prim_ID      = event;
   prim->Sapi         = sapi;
   prim->Ces          = ces;
   prim->LLI          = lli;
   prim->Call_ID      = call_id;
   prim->Dsl_ID       = dsl_id;
   prim->DSL_Chan_ID  = chan_id;
   prim->FreePkt      = free;
   prim->Pkt          = pkt;
}

/************************************************************************
* LIF_ReadMail
*************************************************************************
*
* Provide a standard centralized mailbox read function to
* insulate the applications from the executive where required.
*
*/
PUBLIC BYTE *LIF_ReadMail( task_id )

INT16 task_id;
{
   EXT_MEM gen_addr;
   MAILBOX_ID mb;

   if ((mb = LIF_ID_To_MB (task_id)) == NULL)
      {
#ifndef TASK_TRACER
#ifndef REMOTE_TRACER
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("lif", "LIF_ReadMail", "Unknown task id.");
#endif
#endif
#endif
      gen_addr.Addr = NULL;
      }
   else
      {
         MTEX_ReadMail( mb, &gen_addr );
      }

   return( gen_addr.Addr );
}

/************************************************************************
* LIF_RemoveTimer
*************************************************************************
*
* Remove an entry from the timer table
*
*/
PUBLIC void LIF_RemoveTimer (handle)

INT16 handle;                      /* index to the entry to be removed */
{
   LIF_TIMER *t;

#ifdef DEBUG
   printf("LIF_RemoveTimer () %3d\n", handle);
#endif

   t = &LIF_TimerTable [handle];

   if (t->Running)
      LIF_StopTimer (handle);
   if (t->Prim != NULL)
      LIF_Dispose (t->Prim);
   t->DestMB     = NULL;
   t->Prim       = NULL;
   t->TimerBlock = NULL;
   t->Sn         = 0;
   t->Ticks      = 0;
   t->Running    = FALSE;
#ifdef MEM_STATS
   --LIF_UsedTimers;
#endif
}

#ifndef NO_PKG
/************************************************************************
* LIF_SendBuf
*************************************************************************
*
* Send Package Buffer. Assumes Info part has been allocated & filled.
*
* Return: NOERR, if successful; ERR, otherwise.
*
*/
PUBLIC INT16 LIF_SendBuf(source_id, dest_id, infolen, info_buftype, info,
   msgtype,  msecs,  priority)

INT16       source_id;                              /* source layer id */
INT16       dest_id;                           /* destination layer id */
INT16       infolen;                                    /* info length */
BUFFER_TYPE info_buftype;                 /* BUFFER_TYPE used for info */
BYTE *      info;                                     /* stuff to send */
BYTE        msgtype;                                   /* message type */
UINT32      msecs;       /* if != 0, send delayed mail with this delay */
BYTE        priority;                 /* mail priority (0 through 255) */
{
   INT16       retcode;
   PKG_ID      pkg;

   retcode = ERR;

   /* Get a package
    */
   if ((pkg = LIF_GetPkg()) == NULL)
      {
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("lif", "LIF_SendBuf", "Can't get package.");
#endif
      goto Bailout;
      }

   /* Pack the package
    */
   pkg->SourceID    = source_id;
   pkg->DestID      = dest_id;
   pkg->InfoLen     = infolen;
   pkg->MsgType     = msgtype;
   pkg->PkgInfo     = info;
   pkg->InfoBufType = info_buftype;

   retcode = LIF_MailPkg(pkg, msecs, priority, MAIL_TO_DEST | MAIL_TO_TRACER);

Bailout:

   return(retcode);
}
#endif                                                       /* NO_PKG */

/************************************************************************
* LIF_SendPkt
*************************************************************************
*
* Send packet.
*
* Return: NOERR, if successful; ERR, otherwise.
*
* FFS - What about channel ID?
*
*/
PUBLIC INT16 LIF_SendPkt (proto, source_id, state, dest_id, event, dsl_id,
   sapi, ces, lli, call_id, chan_id, free, pkt, msecs,
   priority)

INT16 proto;                                               /* protocol */
INT16 source_id;                                    /* source layer id */
BYTE state;                                          /* sender's state */
INT16 dest_id;                                 /* destination layer id */
INT16 event;                                          /* event to mail */
BYTE dsl_id;                                /* digital subscriber loop */
BYTE sapi;                                                     /* SAPI */
BYTE ces;                                                       /* CES */
INT16 lli;                                          /* logical link id */
INT16 call_id;                                              /* call id */
INT16 chan_id;                                           /* channel ID */
BOOLEAN free;                                          /* free packet? */
PKT_ID pkt;                                       /* address of packet */
UINT32 msecs;   /* if != 0, send delayed mail with this delay in msecs */
BYTE priority;                        /* mail priority (0 through 255) */
{
    INT16 ret_code = ERR;
    PRIM_ID p;                                     /* primitive to send */

    /*
     * Get a primitive
     */
    if ((p = LIF_GetPrim (TRUE)) == NULL) {
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("lif", "LIF_SendPkt", "Can't get primitive.");
#endif
    } else {
        /*
         * Pack the primitive
         */
        LIF_PutPrim (p, proto, source_id, state, dest_id, event, dsl_id, sapi,
           ces, lli, call_id, chan_id, free, pkt);

        ret_code = LIF_MailPrim( p, source_id, dest_id, msecs, priority,
                     MAIL_TO_DEST | MAIL_TO_TRACER);
      }
   return(ret_code);
}

/************************************************************************
* LIF_Sleep
*************************************************************************
*
* Go to sleep for a few msecs and give other tasks a chance to run.
* Returns ERR if a timer could not be set or NOERR if it did sleep.
*
* All variables defined in this function must be automatic (not static)
* to allow re-entrancy.
*/
PUBLIC INT16 LIF_Sleep(msec)

INT16 msec;
{
   MAILBOX TmpMb;                                      /* temp mailbox */
   TIMER_ID Timer;                                    /* polling timer */
   EXT_MEM ExtMem;                          /* extended memory address */
   INT16 junk;                                       /* junk parameter */

   MailBoxInit(&TmpMb, 1);
   if (MTEX_SetTimer(&TmpMb, &ExtMem, (int) (msec / SOFT_TICK_LEN),
       &Timer, &junk) == ERR)
      goto Bailout;
   MTEX_ReadMail(&TmpMb, &ExtMem);
   return NOERR;

Bailout:

#ifdef DISPLAY_LOCAL_ERR
   LIF_Error("lif", "LIF_Sleep", "can't set timer");
#endif
   return ERR;
}

/************************************************************************
* LIF_StartTimer
*************************************************************************
*
* Start a timer.
*
*/
PUBLIC void LIF_StartTimer (handle)

INT16 handle;                         /* index to LIF_TimerTable entry */
{
   EXT_MEM     gen_addr;
   LIF_TIMER * t;

   if (TimersOn)
      {
      t = &LIF_TimerTable[handle];
      if (t->Running)
         {
#ifdef DEBUG
         printf("RESTART TIMER %3d\n", handle);
#endif
         LIF_StopTimer (handle);
         }
#ifdef DEBUG
      else
         {
         printf("START   TIMER %3d\n", handle);
         }
#endif

      gen_addr.Addr = (BYTE *) t->Prim;
      if (MTEX_SetTimer (t->DestMB, &gen_addr,
          t->Ticks, &t->TimerBlock, &t->Sn) == ERR)
         {
#ifdef DISPLAY_LOCAL_ERR
         LIF_Error ("lif", "LIF_StartTimer", "MTEX_SetTimer Failed");
#endif
         goto Bailout;
         }
      t->Running = TRUE;
      }

Bailout:

   return;
}

/************************************************************************
* LIF_StopTimer
*************************************************************************
*
* Stop a timer.
*
*/
PUBLIC void LIF_StopTimer (handle)

INT16 handle;                         /* index to LIF_TimerTable entry */
{
   LIF_TIMER *t;

   /* Clear the timer regardless of TimersOn.  TimersOn may have been
    *  set to FALSE since the timer was started.  Gratuitous ClearTimers
    *  are OK.
    */
   t = &LIF_TimerTable[handle];
   if (t->Running)
      {
#ifdef DEBUG
      printf("STOP    TIMER %3d\n", handle);
#endif
      MTEX_ClearTimer (t->TimerBlock, t->Sn, t->DestMB);
      t->Running = FALSE;
      }
}

/************************************************************************
* LIF_TestMail
*************************************************************************
*
* Test if a task has mail available in its mailbox.
*
* Return: TRUE or FALSE.
*
*/
PUBLIC BOOLEAN LIF_TestMail( task_id )

INT16 task_id;                                         /* task to test */
{
   return ( TestMail( LIF_ID_To_MB( task_id ) ) ? TRUE : FALSE );
}

/************************************************************************
* LIF_TimerRunning
*************************************************************************
*
* Test if a timer is running.
*
* Return: TRUE or FALSE.
*
*/
PUBLIC BOOLEAN LIF_TimerRunning (handle)

INT16 handle;                         /* index to LIF_TimerTable entry */
{
   return (LIF_TimerTable[handle].Running);
}

#ifdef TRACING
/************************************************************************
* LIF_TraceOk
*************************************************************************
*
* Decide if a message should fork for tracing by TaskView.
*
*/
PUBLIC BOOLEAN LIF_TraceOk(src, dst)

int src;                                            /* source layer id */
int dst;                                       /* destination layer id */
{
   BOOLEAN trace_flag;

#ifdef TASK_DPDRV
   if (LIF_ID_To_MB(TRACER_ID) == DPDRV_Write_MB &&
       LIF_ID_To_MB(dst) == DPDRV_Write_MB)
      trace_flag = FALSE;
   else
      trace_flag = TRUE;
#else
   trace_flag = (dst != TRACER_ID) &&
      (LIF_Trace == 0xffff || src == LIF_Trace || dst == LIF_Trace);
#endif

   return trace_flag;
}

#ifndef NO_PKG
/************************************************************************
* LIF_TracePkg
*************************************************************************
*
*  Handles tracing of packages.
*
*/
PUBLIC int LIF_TracePkg(pkg)

PKG_ID pkg;                                        /* package to trace */
{

   /*
   *  Sends copy of package to task tracer
   */
   PKG_ID      new_pkg;                 /* package to send to taskview */
   EXT_MEM     gen_addr;
   MAILBOX_ID  tracer_mb;
   int         result;

   result = ERR;

   if ((new_pkg = LIF_GetPkg()) == NULL)
      {
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("LIF", "LIF_TracePkg", "Can't get package.");
#endif
      goto bailout;
      }

   if (LIF_CpyPkg (new_pkg, pkg) == ERR)
      {
      LIF_Dispose (new_pkg);
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("LIF", "LIF_TracePkg", "Can't copy package.");
#endif
      goto bailout;
      }

   gen_addr.Addr = (BYTE *) new_pkg;

   if ((tracer_mb = LIF_ID_To_MB(TRACER_ID)) == NULL)
      {
      LIF_Dispose (new_pkg);
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("LIF", "LIF_TracePkg", "Bad Trace ID");
#endif
      goto bailout;
      }
   if (MTEX_SendMail(tracer_mb, UP, &gen_addr, UP_MAIL) == ERR)
      {
      LIF_Dispose (new_pkg);
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("LIF", "LIF_TracePkg", "MTEX_SendMail failed.");
#endif
      goto bailout;
      }

   result = NOERR;

bailout:

   return result;
}
#endif                                                       /* NO_PKG */

/************************************************************************
* LIF_TracePrim
*************************************************************************
*
*  Handles tracing of primitives
*
*/
PUBLIC int LIF_TracePrim(prim)

PRIM_ID prim;                                    /* primitive to trace */
{
   /*
   *  Sends copy of primitive to task tracer
   */
   PRIM_ID new_prim;                  /* primitive to send to taskview */
   EXT_MEM gen_addr;
   MAILBOX_ID  tracer_mb;
   int result;

   result = ERR;

   if ((new_prim = LIF_GetPrim (TRUE)) == NULL)
      {
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("LIF", "LIF_TracePrim", "Can't get primitive.");
#endif
      goto bailout;
      }

   if (LIF_CpyPrim (new_prim, prim) == ERR)
      {
      LIF_FreePrim (new_prim);
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("LIF", "LIF_TracePrim", "Can't copy primitive.");
#endif
      goto bailout;
      }
   new_prim->FreePkt = TRUE;   /* TaskView should always free this pkt */
   gen_addr.Addr = (BYTE *) new_prim;

   if ((tracer_mb = LIF_ID_To_MB(TRACER_ID)) == NULL)
      {
      LIF_Dispose (new_prim);
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("LIF", "LIF_TracePrim", "Bad Trace ID");
#endif
      goto bailout;
      }

   if (MTEX_SendMail (tracer_mb, UP, &gen_addr, UP_MAIL) == ERR)
      {
      LIF_Dispose (new_prim);
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("LIF", "LIF_TracePrim", "MTEX_SendMail failed.");
#endif
      goto bailout;
      }

   result = NOERR;

bailout:

   return result;
}
#endif

/************************************************************************
* LIF_UpdateTimer
*************************************************************************
*
* Update an entry in the timer table.
*
* Return: NOERR, if successful; ERR, otherwise.
*
*/
PUBLIC INT16 LIF_UpdateTimer (handle, source_id, dest_id, event, dsl_id,
   sapi, ces, call_id, chan_id, msecs)

INT16 handle;                         /* index to LIF_TimerTable entry */
INT16 source_id;                               /* source of timed mail */
INT16 dest_id;                            /* destination of timed mail */
INT16 event;                                     /* event to be mailed */
BYTE dsl_id;                                /* digital subscriber loop */
BYTE sapi;                                           /* sapi of sender */
BYTE ces;                                             /* ces of sender */
INT16 call_id;                                              /* call ID */
INT16 chan_id;                                           /* channel ID */
UINT32 msecs;                              /* length of timer in msecs */
{
   int ret_code;
   LIF_TIMER *t;
   MAILBOX_ID dest_mb;

#ifdef DEBUG
   printf("LIF_UpdateTimer ()\n");
#endif

   ret_code = ERR;

   /* Check source
    */
   if (LIF_ID_To_MB (source_id) == NULL)
      {
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("lif", "LIF_UpdateTimer", "Unknown source task id.");
#endif
      goto Bailout;
      }

   /* Check destination
    */
   if ((dest_mb = LIF_ID_To_MB (dest_id)) == NULL)
      {
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("lif", "LIF_UpdateTimer", "Unknown destination task id.");
#endif
      goto Bailout;
      }

   t = &LIF_TimerTable[handle];

   if (t->Prim)
      {
      /* Pack the primitive with the updated values
       */
      LIF_PutPrim (t->Prim, 0, source_id, 0, dest_id, event, dsl_id, sapi,
         ces, 0, call_id, chan_id, FALSE, NULL);
      t->DestMB = dest_mb;
      t->Ticks = msecs/SOFT_TICK_LEN;
      ret_code = NOERR;
#ifdef DEBUG
      LIF_DispPrim (t->Prim);
#endif
      }

Bailout:

   return (ret_code);
}

/*
 * Updates the counters for how much memory is being used. Keeping
 * indivdual counters may be useful for finding memory leaks.
 */
void update_mem_count (BUFFER_TYPE mem_type, int increm)
{
    switch (mem_type) {
        case PRIM_BTYPE:
            Used_prim += increm;
            break;
        case PKT_BTYPE:
            Used_pkt += increm;
            break;
        case HEADER_BTYPE:
            Used_header += increm;
            break;
        case SML_INFO_BTYPE:
            Used_small += increm;
            break;
        case LRG_INFO_BTYPE:
            Used_lrg += increm;
            break;
        case PKG_BTYPE:
        default:
            Used_pkg +=increm;
            break;
    }
}
