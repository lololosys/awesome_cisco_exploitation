/*
 *	P A R S E R _ D E F S _ O S P F . H
 *
 * $Id: parser_defs_ospf.h,v 3.1.40.2 1996/04/20 08:03:48 myeung Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/parser_defs_ospf.h,v $
 * 
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_ospf.h,v $
 * Revision 3.1.40.2  1996/04/20  08:03:48  myeung
 * CSCdi54385:  Add show ip ospf maxage for debugging MAXAGE LSA problem
 * Branch: California_branch
 *
 * Revision 3.1.40.1  1996/04/16  19:02:09  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/01  04:58:46  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:51:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:30:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* OSPF defines */

/*
 * OSPF interface command
 */
enum int_ospf_command_mode {
    OSPF_IF_COST,
    OSPF_RXMT_INTERVAL,
    OSPF_TRANS_DELAY,
    OSPF_PRIORITY,
    OSPF_HELLO_INTERVAL,
    OSPF_DEAD_INTERVAL,
    OSPF_NETWORK_TYPE,
    OSPF_AUTH_CT_KEY,
    OSPF_AUTH_MD_KEY,
    OSPF_DEMAND_CIRCUIT,
};

/*
 * OSPF router area command
 */
enum rtr_ospf_area_command_mode {
    OSPF_AREA,
    OSPF_AREA_VIRTUAL,
    OSPF_AREA_RANGE,
    OSPF_AREA_NSSA,
    OSPF_AREA_STUB,
    OSPF_AREA_DEFAULTCOST,
    OSPF_AREA_AUTH,
    OSPF_AREA_DELETE,
};


/* OSPF virtual link parameter defines */
#define OSPF_VIRT_AUTH_CT   0x01
#define OSPF_VIRT_HELLO	    0x02
#define OSPF_VIRT_DEAD	    0x04
#define OSPF_VIRT_RETRANS   0x08
#define OSPF_VIRT_TRANS	    0x10
#define OSPF_VIRT_AUTH_MD   0x20

/*
 *  OSPF show commands
 */
enum show_ospf_command_mode {
    OSPF_DEFAULT,
    OSPF_DATABASE,
    OSPF_BORDER_ROUTER,
    OSPF_NEIGHBOR,
    OSPF_INTERFACE,
    OSPF_VLINKS,
    OSPF_SPF_STAT,
    OSPF_DELETE_LIST,
    OSPF_EVENTS,
    OSPF_RETX_LIST,
    OSPF_REQ_LIST,
    OSPF_SUMMARY_ADDRESS,
    OSPF_DATABASE_SUMMARY,
    OSPF_MAXAGE_LIST,
    OSPF_BAD_CHECKSUM_QUEUE,
};
/*
 * OSPF router ospf commands
 */
enum rtr_ospf_command_mode {
    OSPF_AUTO_COST,
    OSPF_TYPE7_SUMMARY,
    OSPF_LOG_ADJ_CHG,
};

/*
 * OSPF distance command parameters
 */
#define OSPF_DIST_INTRA		0x0001
#define OSPF_DIST_INTER		0x0002
#define OSPF_DIST_EXTERNAL	0x0004
