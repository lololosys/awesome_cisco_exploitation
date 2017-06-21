/* $Id: bsc_debug.c,v 3.2.60.4 1996/09/11 01:12:00 snyder Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_debug.c,v $
 *------------------------------------------------------------------
 * bsc_debug.c - debug support
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * debug interface hooks 
 * 
 *------------------------------------------------------------------
 * $Log: bsc_debug.c,v $
 * Revision 3.2.60.4  1996/09/11  01:12:00  snyder
 * CSCdi68742:  vigilante code review of CSCdi62395 results in more consts
 *              24 from data, 20 from image
 *              Can I rent someone's life, please?
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/09/10  08:48:01  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/07/25  09:41:44  snyder
 * CSCdi63981:  eliminate unused code, fix bugs, make a common routine
 *              1. create a routine call generic_debug_all and
 *                 make most debugs use it
 *              2. eliminate if (*_debug_inited) return, most never set
 *              the var
 *                 to TRUE making the code useless.
 *              3. declare some constant arrays const
 *              4. fix bugs found along the way
 *              5. save 2768 bytes from image, 660 out of data section
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/05/17  10:40:13  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/26  07:32:26  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2.48.1  1996/04/03  13:50:00  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  08:48:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  06:24:25  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:13:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_generic.h"
#include "config.h"
#include "sys_registry.h"
#include "../ui/debug.h"

#include "bsc_debug.h"
#include "bstun.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "bsc_debug_flags.h"

static const char bsc_str[] = "BSC";

debug_group  bsc_debug_group;
uint         bsc_debug_buffer_size = 20;

boolean is_bsc_debugging_group (uint group) {
    return (is_debugging_group(&bsc_debug_group, group));
}

/*----------------------------------------------------------------------------*/
/* bsc_debug_command: En/Dis-able BSC debugging on single a interface. */
/*----------------------------------------------------------------------------*/

void
bsc_debug_command (parseinfo *csb)
{
    uint group = GETOBJ(int,1);
    uint buff_size = GETOBJ(int,2);

    if ((group != 0) && (bstun_pinfo[group] == NULL)) {
        printf("\nno such bstun group");
       return;
    }

    switch (csb->which) {

    case DEBUG_BSC_EVENT:
       bsc_debug_event = debug_set_flag(&bsc_debug_group,csb->sense, B_DEBUG_EVENT, group);
       print_debug_message(bsc_str,FALSE, csb->sense, B_DEBUG_EVENT,group);
       break;

    case DEBUG_BSC_PACKET:
       bsc_debug_packet = debug_set_flag(&bsc_debug_group,csb->sense, B_DEBUG_PACKET, group);
       print_debug_message(bsc_str,FALSE, csb->sense, B_DEBUG_PACKET, group);

       if (buff_size) {
          bsc_debug_buffer_size = debug_display_size(bsc_str, csb->sense, buff_size);
       }
       break;

    default:
	/* If there's no special case, use the generic handler */
       debug_command(csb);
       break;
    }
}  /* bsc_debug_command. */


/*----------------------------------------------------------------------------*/
/* bsc_debug_all: En/Dis-able BSC debugging on all interfaces. */
/*----------------------------------------------------------------------------*/

static void
bsc_debug_all (boolean flag)
{
    bsc_debug_event = flag;
    debug_set_flag(&bsc_debug_group, bsc_debug_event, B_DEBUG_EVENT, 0);

    bsc_debug_packet = flag;
    debug_set_flag(&bsc_debug_group, bsc_debug_packet, B_DEBUG_PACKET, 0);

}  /* bsc_debug_all. */


/*----------------------------------------------------------------------------*/
/* bsc_debug_show: Show state of BSC debugging. */
/*----------------------------------------------------------------------------*/

static void
bsc_debug_show (void)
{
   /*---- let 'em know full state. */
   if (bsc_debug_group.flags) {
      printf("\n%s:", bsc_str);
      print_debug_message(bsc_str,TRUE, bsc_debug_event, B_DEBUG_EVENT, 0);
      print_debug_message(bsc_str,TRUE, bsc_debug_packet, B_DEBUG_PACKET, 0);
      if (bsc_debug_group.count) {
          int i;
          printf("\n    Group numbers being debugged:\n    ");
          for (i=1; i<=255; i++) {
              if (is_bsc_debugging_group(i)) {
                  printf("%d ", i);
              }
          }
      }
      bsc_debug_buffer_size = debug_display_size(bsc_str, TRUE, bsc_debug_buffer_size);
   }

}  /* bsc_debug_show. */


/*----------------------------------------------------------------------------*/
/* bsc_debug_init: Register BSC debug functions with system. */
/*----------------------------------------------------------------------------*/

void
bsc_debug_init (void)
{
    reg_add_debug_all(bsc_debug_all, "bsc_debug_all");
    reg_add_debug_show(bsc_debug_show, "bsc_debug_show");
    bsc_debug_all(debug_all_p());

}  /* bsc_debug_init. */
