/* $Id: appn_function.h,v 3.3.12.1 1996/05/17 10:39:02 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/appn/appn_function.h,v $
 *------------------------------------------------------------------
 * Declarations of calls from sys into appn
 *
 * May 1995, Steve Berl
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: appn_function.h,v $
 * Revision 3.3.12.1  1996/05/17  10:39:02  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.2.2  1996/05/14  08:05:49  pleung
 * Branch: IbuMod_Calif_branch
 * Sync to California_baseline_960512
 *
 * Revision 3.3.2.1  1996/03/30  05:50:52  mregan
 * CSCdi53126:  APPN needs to support VDLC
 * Branch: IbuMod_Calif_branch
 * add VDLC support to APPN
 *
 * Revision 3.3  1996/02/13  19:18:11  lpereira
 * CSCdi45471:  appn/qllc pvc broken
 *
 * Revision 3.2  1995/11/17  08:37:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:52:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/12  17:19:35  mregan
 * CSCdi29195:  APPN MIB values incorrect
 * update APPN MIB with latest SNMP changes.
 *
 * Revision 2.2  1995/08/11  02:22:31  dmccowan
 * CSCdi30561:  need to support APING on our router
 *
 * Revision 2.1  1995/06/07  20:03:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * WARNING: If you modify the file, appn must be recompiled to pick up
 * the changes. Please send email to "appn-code" if you feel a need to
 * modify this file.
 */

/*
 * This file contains declarations of functions that are implemented
 * in the appn source tree, but must be called from modules that live 
 * in the sys tree. The declarations must be here in the sys tree
 * because no files from sys may include headers from the appn tree.
 * This allows them to build even when the appn tree is not present.
 *
 * Care must be used in adding #includes to this file, since this will
 * create dependancies in appn on source code from sys and this is
 * bad. Only sys header files that are part of the binary api to IOS may
 * be included here.
 */

#include "types.h"
#include "address.h"

#ifndef APPN_FUNCTION_H_INCLUDED
#define APPN_FUNCTION_H_INCLUDED

#define APPN_PARSEBUF 256  /* PARSEBUF in config.h */

#define APPN_CMD_STARTAPPN        32
#define APPN_CMD_STOPAPPN         33

#define DEFCP_POSITION            0
#define DEFCOS_POSITION           2
#define DEFMODE_POSITION          3
#define DEFPORT_POSITION          5
#define DEFCN_POSITION            6
#define DEFLINK_POSITION          7
#define DEFRLU_LOC_POSITION      11
#define DEFSTART_POSITION        22

#define APPN_PORT_RSRB_TYPE       1
#define APPN_PORT_VDLC_TYPE       2

#define  AP_LOW                                 (0x00)
#define  AP_MEDIUM                              (0x01)
#define  AP_HIGH                                (0x02)
#define  AP_NETWORK                             (0x03)

#define  AP_NO                                  (0x00)
#define  AP_YES                                 (0x01)

#define  AP_SEC_NONSECURE                       (0x01)
#define  AP_SEC_PUBLIC_SWITCHED_NETWORK         (0x20)
#define  AP_SEC_UNDERGROUND_CABLE               (0x40)
#define  AP_SEC_SECURE_CONDUIT                  (0x60)
#define  AP_SEC_GUARDED_CONDUIT                 (0x80)
#define  AP_SEC_ENCRYPTED                       (0xA0)
#define  AP_SEC_GUARDED_RADIATION               (0xC0)
#define  AP_SEC_MAXIMUM                         (0xFF)

#define  AP_PROP_DELAY_MINIMUM                  (0x00)
#define  AP_PROP_DELAY_LAN                      (0x4C)
#define  AP_PROP_DELAY_TELEPHONE                (0x71)
#define  AP_PROP_DELAY_PKT_SWITCHED_NET         (0x91)
#define  AP_PROP_DELAY_SATELLITE                (0x99)
#define  AP_PROP_DELAY_MAXIMUM                  (0xFF)

#define  AP_SECONDARY                           (0x00)
#define  AP_PRIMARY                             (0x01)
#define  AP_NEGOTIABLE                          (0x03)

#define  DEFAULT_SAP                            (0x04)

#define  AP_LEARN                               (0x00)
#define  AP_LEN                                 (0x01)
#define  AP_NN                                  (0x02)
#define  AP_EN                                  (0x03)
#define  AP_PU2                                 (0x04)


typedef struct {
  void *original_csb_p;
  uint which;
  boolean nvgen;
  boolean sense;
  void *appn_object_verb;
  uint appn_object_type;
  uint appn_subcommand_type;
  uint val1,val2,val3,val4,val5,val6,val7,val8,val9,
       val10,val11,val12,val13,val14,val15,val16,val17,val18;
  void *idb1;
  hwaddrtype hwaddr1;
  addrtype addr1;
  char nv_command[APPN_PARSEBUF]; 
  char txt1[APPN_PARSEBUF],txt2[APPN_PARSEBUF],txt3[APPN_PARSEBUF],
  txt4[APPN_PARSEBUF],txt5[APPN_PARSEBUF]; 
} appn_csb_t;

void appn_show_function(appn_csb_t *);
void startappn_function(appn_csb_t *);
void dump_appn_function(appn_csb_t *);
void debug_appn_function(appn_csb_t *);
void appn_trace_function(appn_csb_t *);
void appn_config_function(appn_csb_t *);
void appn_exit_appn_command(appn_csb_t *);
boolean get_appn_verb(int *, void **, char **);
void *appn_get_membase(void);
ulong appn_get_memlen(void);
void appn_nv_write(boolean, char *, ...); 
void appn_nv_add(boolean, char *, ...); 
void appn_push_node(appn_csb_t *);
void appn_update_mode(appn_csb_t *);
boolean appn_confirm(char *);
extern void appn_parser_init(void),
            init_ibmappnmib(subsystype *subsys), clear_config_array(void);
extern int appn_status_byte;
extern int appn_status(void);
void clear_config_array(void);

#ifndef NO_APPN_GETOBJ

#define	PTXT_0 			nv_command
#define PTXT_1 			txt1
#define PTXT_2 			txt2
#define PTXT_3 			txt3
#define PTXT_4 			txt4
#define PTXT_5 			txt5
#define PIDB_1                  idb1

#define PTYPE_int(num) val ## num	/* Integer */
#define PTYPE_string(num) PTXT_ ## num	/* String */
#define PTYPE_paddr(num) addr ## num	/* Protocol address ptr */
#define PTYPE_hwaddr(num) hwaddr ## num	/* Hardware address ptr */
#define PTYPE_idb(num) PIDB_ ## num	/* Interface data block ptr */

#define PPTR_int
#define PPTR_string
#define PPTR_paddr &
#define PPTR_hwaddr &
#define PPTR_idb

#define GETOBJ(type,num) (PPTR_ ## type ## (csb->PTYPE_ ## type ## ( ## num ## )))
#define ISOBJ(type,num) ((int) GETOBJ(type,num) != 0)
#endif

#endif
