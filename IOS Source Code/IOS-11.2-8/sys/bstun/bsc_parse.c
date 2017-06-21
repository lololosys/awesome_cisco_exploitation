/* $Id: bsc_parse.c,v 3.7.8.3 1996/09/10 08:48:22 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_parse.c,v $
 *------------------------------------------------------------------
 * bsc_parse.c - Parses the BSC configuration commands
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bsc_parse.c,v $
 * Revision 3.7.8.3  1996/09/10  08:48:22  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.7.8.2  1996/06/17  08:30:02  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.7.8.1  1996/05/17  10:40:42  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.2.2  1996/04/11  08:19:37  jbalestr
 * CSCdi51807:  need a test command to transmit raw bisync frames
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.2.1  1996/03/17  17:35:52  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.7  1996/02/27  18:44:59  rbatz
 * CSCdi49933:  Command "full-duplex" displayed incorrectly as
 *              "full_duplex".  Also, make 11.0, 11.1 "show int"
 *              for half-duplex the same.
 *
 * Revision 3.6  1996/02/13  21:27:28  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.5  1996/02/07  16:10:15  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/02/01  06:00:10  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/29  07:26:32  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  08:48:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/03  01:04:04  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.3  1995/06/21  02:51:10  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.2  1995/06/16  06:24:41  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:14:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "../if/network.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_bsc.h"
#include "../parser/parser_defs_half_duplex.h"
#include "../wan/serial_parser.h"
#include "bstun.h"
#include "bsc.h"
#include "bsc_lcb.h"
#include "bsc_conf.h"


extern queuetype bstun_pinfoQ;


/*----------------------------------------------------------------------------*/
/* [no] bsc primary. */

static void
bsc_parse_primary (parseinfo* csb)
{
   hwidbtype* idb = csb->interface->hwptr;

   if (csb->nvgen)                      /* NV-Ram generation. */
      BscWriteConfig(idb->lcb, BSC_CMD_PRIMARY);
   else {
      if (csb->sense)                /* Config command. */
         BscKeyPrimary(idb, bsc_query_local_ack_capable(idb));
      else                           /* Config command negation. */
         BscNKeyPrimary(idb);
      (*idb->reset)(idb);
   }
}  /* bsc_parse_primary. */


/*----------------------------------------------------------------------------*/
/* [no] bsc secondary. */

static void
bsc_parse_secondary (parseinfo *csb)
{

   hwidbtype *idb = csb->interface->hwptr;

   if (csb->nvgen)                       /* NV-Ram generation. */
      BscWriteConfig(idb->lcb, BSC_CMD_SECONDARY);
   else {
      if (csb->sense)                 /* Config command. */
         BscKeySecondary(idb, bsc_query_local_ack_capable(idb));
      else                            /* Config command negation. */
         BscNKeySecondary(idb);
      (*idb->reset)(idb);
   }

}  /* bsc_parse_secondary. */


/*----------------------------------------------------------------------------*/
/* [no] bsc contention. */

static void
bsc_parse_contention (parseinfo *csb)
{
   hwidbtype *idb = csb->interface->hwptr;

   if (csb->nvgen)
      BscWriteConfig(idb->lcb, BSC_CMD_CONTENTION);
   else {
      if (!bsc_query_local_ack_capable(idb)) {
         if (csb->sense)
            BscKeyContention(idb);
         else
            BscNKeyContention(idb);
         (*idb->reset)(idb);
      }
      else
	 printf("\nBSC contention is not valid when using local-ack.");
   }

}  /* bsc_parse_contention. */


/*----------------------------------------------------------------------------*/
/* [no] bsc char-set <ebcdic | ascii> */

static void
bsc_parse_charset (parseinfo *csb)
{
   hwidbtype *idb = csb->interface->hwptr;

   if (csb->nvgen)
      BscWriteConfig(idb->lcb, BSC_CMD_CHARSET);
   else {
      if (csb->sense)
         BscKeyCharSet(idb->lcb, GETOBJ(int,1));
      else
         BscNKeyCharSet(idb->lcb, GETOBJ(int,1));
      (*idb->reset)(idb);
   }

}  /* bsc_parse_charset. */


/*----------------------------------------------------------------------------*/
/* [no] bsc fdx. */

void
bsc_parse_fdx (parseinfo *csb)
{
   if (!csb->nvgen) {
        printf("\nConverting 'bsc fdx' to 'full-duplex'");
        csb->which = CFG_HDX_SET_FULL_DUPLEX;
        serial_half_duplex_translate_old_cmds(csb);
   }
}  /* bsc_parse_fdx. */


/*----------------------------------------------------------------------------*/
/* [no] bsc pause <n-tenths-of-seconds> */

static void
bsc_parse_pause (parseinfo *csb)
{
   hwidbtype *idb = csb->interface->hwptr;

   if (csb->nvgen) {
      if (bsc_query_local_ack_capable(idb))
         BscWriteConfig(idb->lcb, BSC_CMD_PAUSE);
   }
   else {
      if (bsc_query_local_ack_capable(idb)) {
         if (csb->sense)
            BscKeyPause(idb->lcb, GETOBJ(int,1));
         else
            BscNKeyPause(idb->lcb);
      }
      else
         printf("\nBSC pause valid with local-ack only.");
   }

}  /* bsc_parse_pause. */


/*----------------------------------------------------------------------------*/
/* [no] bsc servlim <servlim-count> */

static void
bsc_parse_servlim (parseinfo *csb)
{
   hwidbtype *idb = csb->interface->hwptr;

   if (csb->nvgen) {
      if (bsc_query_local_ack_capable(idb))
         BscWriteConfig(idb->lcb, BSC_CMD_SERVLIM);
   }
   else {
      if (bsc_query_local_ack_capable(idb)) {
         if (csb->sense)
            BscKeyServLim(idb->lcb, GETOBJ(int,1));
         else
            BscNKeyServLim(idb->lcb);
      }
      else
         printf("\nCommand only available with bsc-local-ack bstun group.");
   }

}  /* bsc_parse_servlim. */


/*----------------------------------------------------------------------------*/
/* [no] bsc poll-timeout <n-tenths-of-secs> */

static void
bsc_parse_poll_timeout (parseinfo *csb)
{
   hwidbtype *idb = csb->interface->hwptr;

   if (csb->nvgen) {
      if (bsc_query_local_ack_capable(idb))
         BscWriteConfig(idb->lcb, BSC_CMD_POLL_TIMEOUT);
   }
   else {
      if (bsc_query_local_ack_capable(idb)) {
         if (csb->sense)
            BscKeyPollTimeout(idb->lcb, GETOBJ(int,1));
         else
            BscNKeyPollTimeout(idb->lcb);
      }
      else
         printf("\nCommand only available with bsc-local-ack bstun group.");
   }

}  /* bsc_parse_poll_timeout. */


/*--------------------------------------------------------------------------*/
/* [no] bsc host-timeout <n-tenths-of-secs> */

static void
bsc_parse_host_timeout (parseinfo *csb)
{
   hwidbtype *idb = csb->interface->hwptr;
   lcb_t* lcb = idb->lcb;

   if (csb->nvgen) {
      if (bsc_query_local_ack_capable(idb))
         BscWriteConfig(lcb, BSC_CMD_HOST_TIMEOUT);
   }
   else {
      if (bsc_query_local_ack_capable(idb) && (lcb->poll_role == PR_POLLEE)) {
         if (csb->sense)
            BscKeyHostTimeout(lcb, GETOBJ(int,1));
         else
            BscNKeyHostTimeout(lcb);
      }
      else
         printf("\nCommand only available with bsc-local-ack bstun group running as secondary.");
   }

}  /* bsc_parse_host_timeout. */

/*----------------------------------------------------------------------------*/
/* [no] bsc retries <n-retries> */

static void
bsc_parse_retries (parseinfo *csb)
{
   hwidbtype *idb = csb->interface->hwptr;

   if (csb->nvgen) {
      if (bsc_query_local_ack_capable(idb))
         BscWriteConfig(idb->lcb, BSC_CMD_RETRIES);
   }
   else {
      if (bsc_query_local_ack_capable(idb)) {
         if (csb->sense)
            BscKeyRetries(idb->lcb, GETOBJ(int,1));
         else
            BscNKeyRetries(idb->lcb);
      }
      else
         printf("\nCommand only available with bsc-local-ack bstun group.");
   }

}  /* bsc_parse_retries. */


/*----------------------------------------------------------------------------*/
/* [no] bsc retries <n-retries> */

static void
bsc_parse_specpoll (parseinfo *csb)
{
   hwidbtype *idb = csb->interface->hwptr;

   if (csb->nvgen) {
      if (bsc_query_local_ack_capable(idb))
         BscWriteConfig(idb->lcb, BSC_CMD_SPECPOLL);
   }
   else {
      if (bsc_query_local_ack_capable(idb)) {
         if (csb->sense)
            BscKeySpecPoll(idb->lcb);
         else
            BscNKeySpecPoll(idb->lcb);
      }
      else
         printf("\nCommand only available with bsc-local-ack bstun group.");
   }

}  /* bsc_parse_specpoll. */


/*----------------------------------------------------------------------------*/

void
bscif_command (parseinfo *csb)
{
  hwidbtype* idb = csb->interface->hwptr;

  if ((idb->type == IDBTYPE_HD64570) ||
      (idb->type == IDBTYPE_CD2430) ){   /* Mask out hardware not supported. */
     if (idb->lcb) {                    /* Handle configured guys only. */
        switch (csb->which) { 

        case BSC_CMD_PRIMARY:
           bsc_parse_primary(csb);
           break;
    
        case BSC_CMD_SECONDARY:
           bsc_parse_secondary(csb);
           break;

        case BSC_CMD_CONTENTION:
           bsc_parse_contention(csb);
           break;

        case BSC_CMD_CHARSET:
           bsc_parse_charset(csb);
           break;

        case BSC_CMD_FDX:
           bsc_parse_fdx(csb);
           break;

        case BSC_CMD_PAUSE:
           bsc_parse_pause(csb);
           break;

        case BSC_CMD_SERVLIM:
           bsc_parse_servlim(csb);
           break;

        case BSC_CMD_POLL_TIMEOUT:
           bsc_parse_poll_timeout(csb);
           break;

        case BSC_CMD_RETRIES:
           bsc_parse_retries(csb);
           break;

        case BSC_CMD_SPECPOLL:
           bsc_parse_specpoll(csb);
           break;

        case BSC_CMD_HOST_TIMEOUT:
           bsc_parse_host_timeout(csb);
           break;

        default:
           bad_parser_subcommand(csb, csb->which);
	   break;
        }  /* switch (csb->which). */
     } else if (!csb->nvgen) {
        printf("\nA BSC bstun group for this interface must be defined first.");
     }
  } else if (!csb->nvgen) {
    printf("\nCannot configure BSC on %s with this hardware.",
           idb->hw_namestring);
  }

}  /* bscif_command. */


/*----------------------------------------------------------------------------*/

void
bsc_show_command (parseinfo *csb)
{
    hwidbtype *idb;
    uint group, address;
    boolean first;
    
    group = GETOBJ(int,1);
    address = GETOBJ(int,2);

    automore_enable(NULL);
    first = TRUE;
    FOR_ALL_HWIDBS(idb) {
        if (((group == 0) || (group == idb->bstun_group)) 
         && (idb->poll_protocol_flags & PP_ACTIVE)
         && (idb->lcb != NULL)) {
           if (first) {
              first = FALSE;
           } else {
              automore_conditional(0);
           }
           bsc_show_interface(idb, address);
        }
    }
    automore_disable();

}  /* show_bsc. */


