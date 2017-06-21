/* $Id: bstun_debug.c,v 3.2.60.4 1996/09/11 17:38:42 snyder Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bstun_debug.c,v $
 *------------------------------------------------------------------
 * bstun_debug.c -- Debugging routines for BSTUN
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bstun_debug.c,v $
 * Revision 3.2.60.4  1996/09/11  17:38:42  snyder
 * CSCdi68797:  char pointers to char arrays and make const
 *              120 data, 152 image
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/09/11  01:12:01  snyder
 * CSCdi68742:  vigilante code review of CSCdi62395 results in more consts
 *              24 from data, 20 from image
 *              Can I rent someone's life, please?
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/09/10  08:48:38  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/07/25  09:41:45  snyder
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
 * Revision 3.2  1995/11/17  08:49:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  06:24:50  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:14:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "../ui/debug.h"
#include "bstun.h"
#include "bstun_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "bstun_debug_flags.h"

static const char bstun_str[] = "BSTUN";

/* Other initialized storage */
debug_group  bstun_debug_group;
uint         bstun_debug_buffer_size = 20;

boolean is_bstun_debugging_group (uint group) {
    return (is_debugging_group(&bstun_debug_group, group));
}

void
bstun_debug_command (parseinfo *csb)
{
    uint group = GETOBJ(int,1);
    uint buff_size = GETOBJ(int,2);

    if ((group != 0) && (bstun_pinfo[group] == NULL)) {
        printf("\nno such bstun group");
       return;
    }

    switch (csb->which) {
       
    case DEBUG_BSTUN_PACKET:
       bstun_debug_packet = debug_set_flag(&bstun_debug_group,csb->sense, B_DEBUG_PACKET, group);
       print_debug_message(bstun_str,FALSE, csb->sense, B_DEBUG_PACKET, group);
       if (buff_size) {
          bstun_debug_buffer_size = debug_display_size(bstun_str, csb->sense, buff_size);
       }
       break;

    case DEBUG_BSTUN_EVENT:
       bstun_debug_event = debug_set_flag(&bstun_debug_group,csb->sense, B_DEBUG_EVENT, group);
       print_debug_message(bstun_str,FALSE, csb->sense, B_DEBUG_EVENT,group);
       break;

    default:
       /* If there's no special case, use the generic handler */
       debug_command(csb);
       break;
    }
}


static void
bstun_debug_all (boolean flag)
{
    bstun_debug_event = flag;
    debug_set_flag(&bstun_debug_group, bstun_debug_event, B_DEBUG_EVENT, 0);

    bstun_debug_packet = flag;
    debug_set_flag(&bstun_debug_group, bstun_debug_packet, B_DEBUG_PACKET, 0);
}

static void
bstun_debug_show (void)
{
   /*---- let 'em know full state. */
   if (bstun_debug_group.flags) {
      printf("\n%s:",bstun_str);
      print_debug_message(bstun_str,TRUE, bstun_debug_event, B_DEBUG_EVENT, 0);
      print_debug_message(bstun_str,TRUE, bstun_debug_packet, B_DEBUG_PACKET, 0);
      if (bstun_debug_group.count) {
          int i;
          printf("\n    Group numbers being debugged:\n    ");
          for (i=1; i<=255; i++) {
              if (is_bstun_debugging_group(i)) {
                  printf("%d ", i);
              }
          }
      }
      bstun_debug_buffer_size = debug_display_size(bstun_str, TRUE, bstun_debug_buffer_size);
   }
}


void
bstun_debug_init (void)
{
    reg_add_debug_all(bstun_debug_all, "bstun_debug_all");
    reg_add_debug_show(bstun_debug_show, "bstun_debug_show");
    bstun_debug_all(debug_all_p());
}


/****************************************************************
 * Generic debug handling routines shared between BSC and BSTUN.
 * They are stored here because all system will have BSTUN.
 ****************************************************************/

static inline boolean bstun_bit_test (const char *arr, uint n) {
    return (arr[n/8] & (1<<(n%8)));
}

static inline void bstun_bit_set (char *arr, uint n) {
    arr[n/8] |= (1<<(n%8));
}

static inline void bstun_bit_unset (char *arr, uint n) {
    arr[n/8] &= ~(1<<(n%8));
}

boolean is_debugging_group (const debug_group *debug, uint group)
{
    return (debug->flags && ((debug->count == 0) || bstun_bit_test(debug->group, group)));
}

void debug_group_clear (debug_group *debug)
{
    /*
     * clear the array of all group numbers
     */
    int i;
    for (i=0; i < B_DEBUG_GROUP_SIZE; i++) {
       debug->group[i] = 0;
    }
    debug->count = 0;
}

boolean debug_set_flag (
    debug_group *debug,
    uint flag_on,
    uint bitmask,
    uint group ) 
{
    uint retcode = TRUE;
    if (flag_on) {
        if (group == 0) {
            debug->flags |= bitmask;
        } else {
            if (!bstun_bit_test(debug->group,group)) {
               bstun_bit_set(debug->group,group);
               debug->count++;
               debug->flags |= bitmask;
            }
        }
    } else {
        /*
         * no group number means no deb all
         * for that debug type
         */
        if (group == 0) {
            debug->flags &= ~bitmask;
            retcode = FALSE;
        } else {
            if (bstun_bit_test(debug->group,group)) {
               bstun_bit_unset(debug->group,group);
               debug->count--;
               if (debug->count == 0) {
                   debug->flags = 0;                   
               }
            }
        }
        if (debug->flags == 0) {
            debug_group_clear(debug);
            retcode = FALSE;
        }
    }
    return (retcode);
}

void print_debug_message (
    const char *msg,
    boolean    newline,
    uint       flag_on,
    uint       fn_event,
    uint       group )
{
    static const char event_str[] = "event";
    static const char pak_str[]   = "packet";
    const char* fn_str;
    const char* descr;

    if (newline) {
       printf("\n  ");
    }

    switch (fn_event) {
    case B_DEBUG_EVENT :
        fn_str = event_str;
        break;
    case B_DEBUG_PACKET :
        fn_str = pak_str;
        break;
    default :
        fn_str = "";
    }

    descr = (flag_on)? "on" : "off";

    if (group == 0) {
        printf("%s %s debugging is %s", msg, fn_str, descr);
    } else {
        printf("%s %s debugging for group %d is %s", msg, fn_str, group, descr);
    }
}

uint debug_display_size (const char* msg, uint sense, uint new_size)
{
    static const char *deb_buf   = "packet display size for debugging is";
    uint debug_buffer_size = (sense) ? new_size : 20;
    printf("\n   %s %s %d", msg, deb_buf, debug_buffer_size);
    return (debug_buffer_size);
}

#define MAX_BYTES_LINE 40
static inline uchar bsc_to_hex (uchar c)
{
   return (uchar) ((c > 9) ? 
		   ((int) 'A' + ((int) c) - 10) : ((int) '0' + ((int) c)));
}

void
debug_output_frame_data (
    const uchar* frame,
    uint datasize,
    uint display_size )
{
   static char line_data[(MAX_BYTES_LINE*2) + 1];
   char* data_ptr;
   uint i , notfirst = 0 , remaining;

   /*
    * restrict display size to maximum data
    */
   remaining = (display_size<datasize) ? display_size : datasize;

   /*
    * put first 20 bytes on same line and upto max bytes on additional lines
    */
   while (remaining) {
      uint this_line_size = (notfirst) ? MAX_BYTES_LINE : 20;
      if (remaining < this_line_size) {
          this_line_size = remaining;
      }
      data_ptr = line_data;
      for (i = 0; i < this_line_size ; i++, frame++ ) {
          *data_ptr++ = bsc_to_hex((*frame & 0xf0) >> 4);
          *data_ptr++ = bsc_to_hex(*frame & 0x0f);
      }
      *data_ptr = '\0';
      buginf("%c %s", (notfirst)?'\n':' ', line_data);
      notfirst++;
      remaining -= this_line_size;
   }
}  /* bsc_output_frame. */
