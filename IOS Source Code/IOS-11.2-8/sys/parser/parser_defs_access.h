/*
 *	P A R S E R _ D E F S _ A C C E S S . H
 *
 * $Id: parser_defs_access.h,v 3.2 1995/11/17 18:48:58 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/parser_defs_access.h,v $
 * 
 * Copyright (c) 1985-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_access.h,v $
 * Revision 3.2  1995/11/17  18:48:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:28:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Access list defines */
#define ACC_NOVELLSAP		 0
#define ACC_NOVELLEXT		 1
#define ACC_NOVELLSTD		 2
#define ACC_ETHERNET		 3
#define ACC_APPLETALK		 4
#define ACC_XNSEXT		 5
#define ACC_XNSSTD		 6
#define ACC_DECNET		 7
#define ACC_IPEXT		 8
#define ACC_IPSTD		 9
#define ACC_TYPE		10

/*
 * IP extended access list items
 */
#define IP_PROT_NAME    	"ip"
#define TCP_PROT_NAME   	"tcp"
#define UDP_PROT_NAME   	"udp"
#define ICMP_PROT_NAME  	"icmp"
#define IGRP_PROT_NAME  	"igrp"
#define	IGMP_PROT_NAME		"igmp"
#define	GRE_PROT_NAME		"gre"
#define	NOSIP_PROT_NAME		"nos"
#define EIGRP_PROT_NAME 	"eigrp"
#define OSPF_PROT_NAME  	"ospf"
#define IPINIP_PROT_NAME	"ipinip"
#define DVMRP_PROT_NAME 	"dvmrp"

/*
 * Extended Access list defines.  Used for both IP and Bridging.
 */
#define OPC_NONE       		0
#define OPC_LT         		1
#define OPC_GT         		2
#define OPC_EQ         		3
#define OPC_NEQ        		4
#define	OPC_AND	       		5	/* Bridging only */
#define	OPC_XOR	       		6	/* Bridging only */
#define OPC_RANGE      		7	/* IP only */
#define OPC_ONEBYTE   		8	/* IP only */
#define OPC_TWOBYTES		9	/* IP only */

#define OPC_NONE_STR    "none"
#define OPC_LT_STR      "lt"
#define OPC_GT_STR      "gt"
#define OPC_EQ_STR      "eq"
#define OPC_NEQ_STR     "neq"
#define OPC_AND_STR	"and"		/* Bridging only */
#define OPC_XOR_STR	"xor"		/* Bridging only */
#define OPC_RANGE_STR	"range"		/* IP only */
#define OPC_ESTAB_STR   "established"	/* IP only */

#define PRECEDENCE_NETWORK	7
#define PRECEDENCE_INTERNET	6
#define PRECEDENCE_CRITICAL	5
#define PRECEDENCE_FLASHOVER	4
#define PRECEDENCE_FLASH	3
#define PRECEDENCE_IMMEDIATE	2
#define PRECEDENCE_PRIORITY	1
#define PRECEDENCE_ROUTINE	0

#define PRECEDENCE_NETWORK_STR		"network"	
#define PRECEDENCE_INTERNET_STR		"internet"	
#define PRECEDENCE_CRITICAL_STR		"critical"	
#define PRECEDENCE_FLASHOVER_STR	"flash-override"	
#define PRECEDENCE_FLASH_STR		"flash"	
#define PRECEDENCE_IMMEDIATE_STR	"immediate"	
#define PRECEDENCE_PRIORITY_STR		"priority"	
#define PRECEDENCE_ROUTINE_STR		"routine"	

#define TOS_NORMAL_NAME		"normal"
#define TOS_DELAY_NAME		"min-delay"
#define TOS_THROUGH_NAME	"max-throughput"
#define TOS_RELIABLE_NAME	"max-reliability"
#define TOS_COST_NAME		"min-monetary-cost"

