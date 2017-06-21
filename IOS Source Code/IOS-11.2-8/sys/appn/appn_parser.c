/* $Id: appn_parser.c,v 3.3.12.3 1996/06/28 23:04:25 hampton Exp $
 * $Source: /release/112/cvs/Xsys/appn/appn_parser.c,v $
 *------------------------------------------------------------------
 * One line description here.
 *
 * May 1995, Steve Berl
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: appn_parser.c,v $
 * Revision 3.3.12.3  1996/06/28  23:04:25  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.12.2  1996/06/17  23:28:33  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.3.12.1  1996/03/18  18:49:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:27:20  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  00:15:41  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/13  19:18:14  lpereira
 * CSCdi45471:  appn/qllc pvc broken
 *
 * Revision 3.2  1995/11/17  08:37:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:52:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/11  02:22:33  dmccowan
 * CSCdi30561:  need to support APING on our router
 *
 * Revision 2.3  1995/06/22  15:51:48  dmccowan
 * CSCdi36064:  APPN doesnt check malloc return
 *
 * Revision 2.2  1995/06/16  21:25:14  widmer
 * CSCdi35996:  appn code doesnt compile due to parser changes
 *
 * Revision 2.1  1995/06/07  20:03:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include "config.h"
#include "parser.h"
#include "../parser/parser_defs_exec.h"
#include "appn_parser.h"
#include "appn_subsys.h"
#define NO_APPN_GETOBJ
#include "appn_function.h"

static appn_csb_t *create_appn_csb(parseinfo *csb);
static inline void destroy_appn_csb(appn_csb_t *appn_csb);

static appn_csb_t *create_appn_csb (parseinfo *csb)
{
  appn_csb_t *appn_csb_p;

  appn_csb_p = malloc(sizeof(appn_csb_t));

  if (!appn_csb_p)
    {
    printf(nomemory);
    return(NULL);
    }

  appn_csb_p->nvgen = csb->nvgen;
  appn_csb_p->sense = csb->sense;
#ifdef APPN_NEEDS_DISTINGUISHABLE_DEFAULT_KEYWORD
  appn_csb_p->set_to_default = csb->set_to_default;
#endif
  appn_csb_p->val1 = GETOBJ(int,1);
  appn_csb_p->val2 = GETOBJ(int,2);
  appn_csb_p->val3 = GETOBJ(int,3);
  appn_csb_p->val4 = GETOBJ(int,4);
  appn_csb_p->val5 = GETOBJ(int,5);
  appn_csb_p->val6 = GETOBJ(int,6);
  appn_csb_p->val7 = GETOBJ(int,7);
  appn_csb_p->val8 = GETOBJ(int,8);
  appn_csb_p->val9 = GETOBJ(int,9);
  appn_csb_p->val10 = GETOBJ(int,10);
  appn_csb_p->val11 = GETOBJ(int,11);
  appn_csb_p->val12 = GETOBJ(int,12);
  appn_csb_p->val13 = GETOBJ(int,13);
  appn_csb_p->val14 = GETOBJ(int,14);
  appn_csb_p->val15 = GETOBJ(int,15);
  appn_csb_p->val16 = GETOBJ(int,16);
  appn_csb_p->val17 = GETOBJ(int,17);
  appn_csb_p->val18 = GETOBJ(int,18);
  strcpy(appn_csb_p->nv_command, GETOBJ(string,0));
  strcpy(appn_csb_p->txt1, GETOBJ(string,1));
  strcpy(appn_csb_p->txt2, GETOBJ(string,2));
  strcpy(appn_csb_p->txt3, GETOBJ(string,3));
  strcpy(appn_csb_p->txt4, GETOBJ(string,4));
  strcpy(appn_csb_p->txt5, GETOBJ(string,5));
  appn_csb_p->idb1 = GETOBJ(idb,1);
  appn_csb_p->hwaddr1 = *GETOBJ(hwaddr,1);
  appn_csb_p->addr1 = *GETOBJ(paddr,1);
  appn_csb_p->appn_object_verb = csb->appn_object_verb;
  appn_csb_p->appn_object_type = csb->appn_object_type;
  appn_csb_p->appn_subcommand_type = csb->appn_subcommand_type;
  appn_csb_p->which = csb->which;
  appn_csb_p->original_csb_p = csb;

  return(appn_csb_p);
}

static inline void destroy_appn_csb (appn_csb_t *appn_csb_p)
{
   free(appn_csb_p);
}

void appn_nv_write (boolean predicate, char *string, ...)
{
  va_list args;
  char output_line[APPN_PARSEBUF];
  int len;

  if (!predicate)
     return;
  else {
     va_start(args, string);
     len = _doprnt(output_line, NULL, string, args, FALSE);
     output_line[len] = '\0';
     va_end(args);
     nv_write(TRUE, output_line);
  }
}

void appn_nv_add (boolean predicate, char *string, ...)
{
  va_list args;
  char output_line[APPN_PARSEBUF];
  int len;

  if (!predicate)
     return;
  else {
     va_start(args, string);
     len = _doprnt(output_line, NULL, string, args, FALSE);
     output_line[len] = '\0';
     va_end(args);
     nv_add(TRUE, output_line);
  }
}

void appn_update_mode (appn_csb_t *appn_csb_p)
{
   parseinfo *csb;

   csb = (parseinfo *)appn_csb_p->original_csb_p;

   switch (csb->appn_object_type) {
      case DEFCP_POSITION:
         set_mode_byname(&csb->mode, "appn-cp", MODE_VERBOSE);
         csb->appn_object_type = DEFCP_POSITION;
         appn_csb_p->appn_object_type = DEFCP_POSITION;
         break;
      case DEFLINK_POSITION:
         set_mode_byname(&csb->mode, "appn-link", MODE_VERBOSE);
         csb->appn_object_type = DEFLINK_POSITION;
         appn_csb_p->appn_object_type = DEFLINK_POSITION;
         break;
      case DEFPORT_POSITION:
         set_mode_byname(&csb->mode, "appn-port", MODE_VERBOSE);
         csb->appn_object_type = DEFPORT_POSITION;
         appn_csb_p->appn_object_type = DEFPORT_POSITION;
         break;
      case DEFRLU_LOC_POSITION:
         set_mode_byname(&csb->mode, "appn-ploc", MODE_VERBOSE);
         csb->appn_object_type = DEFRLU_LOC_POSITION;
         appn_csb_p->appn_object_type = DEFRLU_LOC_POSITION;
         break;
      case DEFMODE_POSITION:
         set_mode_byname(&csb->mode, "appn-mode", MODE_VERBOSE);
         csb->appn_object_type = DEFMODE_POSITION;
         appn_csb_p->appn_object_type = DEFMODE_POSITION;
         break;
      case DEFCOS_POSITION:
         set_mode_byname(&csb->mode, "appn-cos", MODE_VERBOSE);
         csb->appn_object_type = DEFCOS_POSITION;
         appn_csb_p->appn_object_type = DEFCOS_POSITION;
         break;
      case DEFCN_POSITION:
         set_mode_byname(&csb->mode, "appn-cn", MODE_VERBOSE);
         csb->appn_object_type = DEFCN_POSITION;
         appn_csb_p->appn_object_type = DEFCN_POSITION;
         break;
      default:
         buginf("%% Error: invalid appn_object_type in appn_update_mode.");
   }
}

boolean appn_confirm(char *string)
{
   return(confirm(string));
}

void appn_show_command (parseinfo *csb)
{
  appn_csb_t *appn_csb_p;

  appn_csb_p = create_appn_csb(csb);
  if (!appn_csb_p)
     return;
  automore_enable(NULL);
  appn_show_function(appn_csb_p);
  automore_disable();
  destroy_appn_csb(appn_csb_p);
}

void startappn_command (parseinfo *csb)
{
  appn_csb_t *appn_csb_p;

  appn_csb_p = create_appn_csb(csb);
  if (!appn_csb_p)
     return;
  if (csb->which == CMD_STARTAPPN)
     appn_csb_p->which = APPN_CMD_STARTAPPN;
  else if (csb->which == CMD_STOPAPPN)
     appn_csb_p->which = APPN_CMD_STOPAPPN;
  else if (csb->which == EXEC_APING)
     appn_csb_p->which = EXEC_APING;
  else {
     printf("\n%% Error: invalid csb->which in startappn_command.");
     destroy_appn_csb(appn_csb_p);
     return;  
  }
  startappn_function(appn_csb_p);
  destroy_appn_csb(appn_csb_p);
}

void dump_appn_commands (parseinfo *csb)
{
  appn_csb_t *appn_csb_p;

  appn_csb_p = create_appn_csb(csb);
  if (!appn_csb_p)
     return;
  dump_appn_function(appn_csb_p);
  destroy_appn_csb(appn_csb_p);
}

void debug_appn_commands (parseinfo *csb)
{
  appn_csb_t *appn_csb_p;

  appn_csb_p = create_appn_csb(csb);
  if (!appn_csb_p)
     return;
  debug_appn_function(appn_csb_p);
  destroy_appn_csb(appn_csb_p);
}

void appn_trace_cmd (parseinfo *csb)
{
  appn_csb_t *appn_csb_p;

  appn_csb_p = create_appn_csb(csb);
  if (!appn_csb_p)
     return;
  appn_trace_function(appn_csb_p);
  destroy_appn_csb(appn_csb_p);
}

void appn_command (parseinfo *csb)
{
  appn_csb_t *appn_csb_p;

  appn_csb_p = create_appn_csb(csb);
  if (!appn_csb_p)
     return;
  appn_config_function(appn_csb_p);
  csb->appn_object_verb = appn_csb_p->appn_object_verb;
  destroy_appn_csb(appn_csb_p);
}

void appn_exit_appn_command(appn_csb_t *appn_csb_p)
{
   exit_appn_command((parseinfo *)appn_csb_p->original_csb_p);
}
