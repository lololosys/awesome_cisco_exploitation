/* $Id: vtp_debug.c,v 1.1.44.5 1996/09/13 21:47:42 thille Exp $
 * $Source: /release/112/cvs/Xsys/vtp/vtp_debug.c,v $
 *------------------------------------------------------------------
 * vtp_debug.c: DEBUG command code
 *
 * January, 1996,  Percy P. Khabardar
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: vtp_debug.c,v $
 * Revision 1.1.44.5  1996/09/13  21:47:42  thille
 * CSCdi69139:  IOS includes in platform independent VTP code.
 * Branch: California_branch
 * Remove the inclusion of IOS include files from source files shared with
 * non-IOS platforms.  Add comments so someone else won't make the same
 * mistake later.  Move the inclusions to platform specific include file
 * vtp_debug.h.
 *
 * Revision 1.1.44.4  1996/08/28  13:20:16  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.44.3  1996/07/25  09:56:48  snyder
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
 * Revision 1.1.44.2  1996/06/18  00:51:01  cyoung
 * CSCdi60069:  router crashes when writing vtp info to tftp file
 * CSCdi59372:  wrong behavior when conflicting vlan info is received
 * CSCdi59933:  RSP crashes for creating-deleting-recreating vtp-domains
 * CSCdi60044:  7000 image crash
 * CSCdi60394:  [sh vtp trunk] crashes 7000
 * CSCdi60528:  rsp can't change vtp database until someone else does
 * CSCdi60541:  Attempt to delete a vlan crashes image
 * CSCdi60632:  cannot set password for vtp management-domain
 * CSCdi60384:  configuration timestamp is not displayed correctly
 * Plus some Cat5k bug fixes rolled in as well as dependant code clean up.
 * Branch: California_branch
 *
 * Revision 1.1.44.1  1996/05/09  14:48:29  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.1.74.1  1996/04/27  07:38:31  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.62.2  1996/04/17  21:56:57  cyoung
 * CSCdi53840:  error message should pop up once the error occurs
 * Use errmsg to provide timely VTP error logging.
 *
 * CORE - retrieve domain/vlan functions modify input parameters instead
 *        of affecting global structure.
 *        Convert macros to functions (change_to_client,
 *        change_to_transparent).
 *        domain/vlan notifications now pass before and after data so that
 *        transistion actions can be done.
 *        rip out duplicate add/changes from the verify/validate
 *        routines.
 *        vtp_vlan_change_notification is called without checking for
 *        changes.
 *        do not change to client as a result of failed vlan change
 *        notification of domain revision number change notification.
 *  DEP - support new function declarations, fix bug where trying to
 *        configure swidb when no more can be created.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.62.1  1996/04/08  02:42:21  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.50.1  1996/03/22  09:44:59  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.34.1  1996/03/05  05:37:30  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.14.1  1996/02/27  21:20:24  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.2.1  1996/01/23  00:47:15  percyk
 * Branch: LE_Cal_V111_0_16_branch
 * add default vlans at startup time.  add meat to debug files.
 *
 * Revision 1.1  1996/01/23  00:18:33  percyk
 * placeholder files for vtp debugging
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
 
 
/* Declare the initialized text for the debugging array */

#include "../ui/debug_macros.h"
#include "vtp_debug_flags.h"

#define __DECLARE_DEBUG_ARR__
#include "../ui/debug_macros.h"
#include "vtp_debug_flags.h"

void vtp_hexdump(unsigned char *buf, unsigned long bc, char *src);

void vtp_log_packet (unsigned char* buf, unsigned long bc, char* src)
{ 
    if (vtp_debug_packet) 
	vtp_hexdump( buf, bc, src);
}

boolean is_vtp_debug_enabled (void) {
    return vtp_debug_event;
}
/*
 * hexdump of incoming and outgoing vtp packets.
 *
 * bc = byte count, buf is start of the buffer.
 */
void vtp_hexdump (unsigned char *buf, unsigned long bc, char *src)
{
 
   static char hex[] = "0123456789ABCDEF";
   char line[16 * 3 + 2 + 16 + 1];
   int i, j;
   unsigned char c;
 
   if (bc == 0)          /* if none to dump, exit */
      return;

   buginf("\n");
 
   for (i = 0; i < sizeof(line); i++)
       line[i] = ' ';
 
   line[sizeof(line) - 1] = 0;
 
   for (i = 0; i < bc; i += 16) {
 
      for (j = 0; j < 16; j++) {
         line[j * 3] = hex[(buf[i + j] >> 4) & 0xf];
         line[j * 3 + 1] = hex[buf[i + j] & 0xf];
         c = buf[i + j] & 0x7f;
         if ( (c < ' ') || (c > 126) )
            c = '.';
 
         line[50 + j] = c;
      }
 
      if ( (bc - i) < 16 ) {              /* if partial line */
         for (j = bc - i; j < 16; j++) {
            line[j * 3]    = ' ';
            line[j *3 + 1] = ' ';
            line[50 + j]   = ' ';
         }
      }
 
      buginf("\n%s: %s", src, line);
 
   }
   buginf("\n");
 
}

/*
 * v t p _ d e b u g _ a l l ( )
 *
 * vtp_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the vtp debug flags at once.
 * The argument is TRUE for "debug all", FALSE for "undebug all".
 */
void vtp_debug_all (boolean flag)
{
    generic_debug_all(vtp_debug_arr, flag);
}
 
 
/*
 * v t p _ d e b u g _ s h o w ( )
 *
 * vtp_debug_show is called to display the values of all the vtp
 * debugging variables.
 */
void vtp_debug_show (void)
{
    debug_show_flags( &(vtp_debug_arr[0]), "Generic vtp" );
}
 
 
/*
 * v t p _ d e b u g _ i n i t ( )
 *
 * vtp_debug_init is called at vtp startup.  It registers the
 * routines to maintain and display the vtp debug flags, and makes
 * sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when vtp is started.
 */
void vtp_debug_init (void)
{
 
    /* register for "debug all" and "show debug" events.  A real programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list.  Oh, well...
     */
    (void)reg_add_debug_all(vtp_debug_all, "vtp_debug_all");
    (void)reg_add_debug_show(vtp_debug_show, "vtp_debug_show");
 
    /* make sure the debug flags are right at startup.  If "debug all"
     * is in effect when vtp is initialized, we want to turn on all
     * the vtp debugging right now.
     */
    vtp_debug_all(debug_all_p());
}


