/* $Id: parser_defs_smrp.h,v 3.3 1996/01/29 07:01:11 hampton Exp $
 * $Source: /release/111/cvs/Xsys/smrp/parser_defs_smrp.h,v $
 *------------------------------------------------------------------
 * Parser definitions for SMRP.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_smrp.h,v $
 * Revision 3.3  1996/01/29  07:01:11  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:53:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:45:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __parser_defs_smrp_h__
#define __parser_defs_smrp_h__

/* Constants for smrp_command() */

enum smrp_command_enums {
    SMRP_ROUTING
};

enum smrpif_command_enums {
    SMRP_MCROUTEC,
    SMRP_PROTO
};

/* SMRP clear commands */
enum { 
    CLEAR_SMRP_MCACHE,
    CLEAR_SMRP_NEIGHBOR,
    CLEAR_SMRP_ROUTE,
    CLEAR_SMRP_TRAFFIC,
};

/* SMRP show commands */
enum { 
    SHOW_SMRP_FWD,
    SHOW_SMRP_GLOBALS,
    SHOW_SMRP_GROUP,
    SHOW_SMRP_MCACHE,
    SHOW_SMRP_NBR,
    SHOW_SMRP_PORT,
    SHOW_SMRP_PRIVATE,
    SHOW_SMRP_ROUTE,
    SHOW_SMRP_TRAFFIC,
};

/* Constants for smrp_show_commands() */
enum { 
    SMRP_SHOW_TRAFFIC_ALL = 1,
    SMRP_SHOW_TRAFFIC_GROUP,
    SMRP_SHOW_TRAFFIC_NBR,    
    SMRP_SHOW_TRAFFIC_PORT,      
    SMRP_SHOW_TRAFFIC_ROUTE,     
    SMRP_SHOW_TRAFFIC_TRANS 
};    
 
#define SMRP_SHOW_PVT_ALL           0
#define SMRP_SHOW_PVT_REQ           (SMRP_SHOW_PVT_ALL+1)
#define SMRP_SHOW_PVT_RESP          (SMRP_SHOW_PVT_REQ+1)

#define SMRPRANGE(name, acc, alt, start, end)\
  HELP (SRH ## name, NONE, "Start-End\t");\
  WHITESPACE(SR4 ## name, acc, SRH ## name);\
  IDECIMAL(SR3 ## name, SR4 ## name, SRH ## name, end, 1, 0xffff);\
  CHARACTER(SR1 ## name, SR3 ## name, SRH ## name, '-');\
  IDECIMAL(SR0 ## name, SR1 ## name, alt, start, 1, 0xffff);\
  HELP (name, SR0 ## name, "  Start-End\tSMRP network range\n")

#define SMRPGROUPADDR(name, acc, alt, start, end)\
  HELP (SRH ## name, NONE, "network.socket\t");\
  WHITESPACE(SR4 ## name, acc, SRH ## name);\
  IDECIMAL(SR3 ## name, SR4 ## name, SRH ## name, end, 1, 255);\
  CHARACTER(SR1 ## name, SR3 ## name, SRH ## name, '.');\
  IDECIMAL(SR0 ## name, SR1 ## name, alt, start, 1, 65536);\
  HELP (name, SR0 ## name, "  network.socket\tSMRP group address\n")

#define SMRPTESTGRPADDR(name, acc, alt, start, end)\
  HELP (SRH ## name, NONE, "network.socket\t");\
  WHITESPACE(SR4 ## name, acc, SRH ## name);\
  IDECIMAL(SR3 ## name, SR4 ## name, SRH ## name, end, 0, 255);\
  CHARACTER(SR1 ## name, SR3 ## name, SRH ## name, '.');\
  IDECIMAL(SR0 ## name, SR1 ## name, alt, start, 0, 65536);\
  HELP (name, SR0 ## name, "  network.socket\tSMRP group address\n")

#endif /* __parser_defs_smrp_h__ */
