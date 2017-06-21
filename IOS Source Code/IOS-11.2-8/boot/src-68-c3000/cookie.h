/* $Id: cookie.h,v 3.3.10.3 1996/06/04 20:46:54 cshen Exp $
 * $Source: /release/112/cvs/Xboot/src-68-c3000/cookie.h,v $
 *------------------------------------------------------------------
 * cookie.h -- IGS Cookie definition (Signetics N82S123N):
 *
 * September 4, 1990, Chris Shaker
 *
 * Copyright (c) 1990-1996,97 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cookie.h,v $
 * Revision 3.3.10.3  1996/06/04  20:46:54  cshen
 * CSCdi59428:  Add Corsica support to c2500 11.0(10) bootrom
 * Branch: California_branch
 * Reserve interface cookie for Corsica
 *
 * Revision 3.3.10.2  1996/04/05  02:41:58  yoko
 * CSCdi53681:  add new cpu types
 * Branch: California_branch
 * define new cpu types and emt calls
 *
 * Revision 3.3.10.1  1996/03/26  18:24:46  enf
 * CSCdi52111:  Need to reserve #defines for new platform
 * Branch: California_branch
 * Define CPU_VOLCANO
 *
 * Revision 3.3  1996/02/27  05:51:18  afactor
 * CSCdi47194:  Add cookie capability for jalapeno mechanical option
 *
 * Revision 3.2  1995/12/03  17:55:18  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.1  1995/11/09  09:09:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/22  01:11:46  enf
 * CSCdi40869:  Add frankenstorm to 11.0
 *
 * Revision 2.3  1995/08/17  17:56:54  jcchan
 * CSCdi38927:  Reserve cookie value for new product development
 *
 * Revision 2.2  1995/08/05  20:03:18  ssangiah
 * CSCdi38231:  Reserve cookie value for new development
 *
 * Revision 2.1  1995/06/07  19:15:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.12  1995/04/25 19:06:42  enf
 * CSCdi33100:  Add new interface type
 *
 * Revision 1.11  1995/03/21  23:45:48  gstovall
 * CSCdi31294:  Add support for new platform
 * Bring the joy of Sapphire to the 10.3 mainline.
 *
 * Revision 1.10  1995/03/07  06:36:51  vnguyen
 * CSCdi30540:  need to clean up hub repeater code
 *              - remove code that support token ring hub
 *              - move hub misc register from pcmap.h to hub.h
 *
 * Revision 1.9  1995/02/10  01:14:19  gstovall
 * CSCdi29698:  Missing placeholder for new products
 * Add missing placeholder defs for Sapphire.
 *
 * Revision 1.8  1994/10/25  17:42:33  vnguyen
 * CSCdi25888:  reserve cookie value for new product
 *
 * Revision 1.7  1994/07/13  21:59:56  sgarcia
 * CSCdi22064:  define new interface types
 * Campeche and Galapagos
 *
 * Revision 1.6  1994/04/22  21:45:04  jcchan
 * CSCdi19271:  New codes need to be defined. See description for details.
 *
 * Revision 1.5  1994/04/12  23:15:02  jturner
 * CSCdi18945:  No igs-if or igs-df images in 10.0
 *         - added code to support build of subset images.
 * CSCdi18946:  Cookie pointer not initialized after freespace_init()
 *         - properly initialized the cookie pointer.
 * CSCdi18947:  SNMP variable needed to tell what subset image is supported
 *         - added variable feature_set and function feature_set_init()
 *                 to determine what subset image is supported.
 *
 * Revision 1.4  1994/02/16  18:47:28  vandys
 * CSCdi17453:  Need placeholder for Copan support
 * Remove new CPU type per enf's suggestions; add
 * Copan config cookie values.
 *
 * Revision 1.3  1994/01/04  08:11:45  rnardin
 * Add PROCESSOR_SIERRA.
 *
 * Revision 1.2  1993/12/21  06:25:42  enf
 * Cancun boot support
 *
 * Revision 1.1  1993/09/24  16:33:20  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.7  1993/06/25  05:08:57  smackie
 * Bring modular system to Aries 9.21(0.10) patch level.
 *
 * CSCdi11223:  merge 9.14 low end code into aries
 *
 * Revision 1.6  1993/04/20  20:04:12  enf
 * CSCdi10417:  Hide commands not valid for hardware config
 *
 * Revision 1.5  1992/09/19  21:54:31  gstovall
 * Added changes to support the Cristal low-end router.  Added new processor
 * type and converted a few if-else to switch statements.
 *
 * Revision 1.4  1992/08/31  20:37:56  dcoli
 * Changes for Brut.
 *
 * Revision 1.3  1992/07/08  08:15:15  schaefer
 * remove cookie-based software capability bits
 *
 * Revision 1.2  1992/04/17  23:48:53  schaefer
 * fix typo in .cvsignore
 * ROM monitor changes to read image capability bits from cookie PROM
 *
 * Revision 1.1  1992/03/07  23:57:33  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __COOKIE_H__
#define __COOKIE_H__

typedef struct cookie cookie_t;
struct cookie {							/* byte #  */
    uchar interfaces;           /* Which interfaces does it have?      0   */
    uchar vendor;               /* Which vendors version of the IGS    1   */
    uchar ether0_hw_addr[6];    /* Increment for ethernet 1           2-7  */
    uchar processor;            /* Which processor is this?           8    */
    uchar hw_rework[4];         /* Mfg. Rework number (deviation)     9-12 */
    uchar UNUSED_1[3];          /* Unused (padding)                  13-15 */
    uchar serial[4];            /* BCD packed 8 digit serial number  16-19 */
    uchar UNUSED_2[7];          /* Unused                            20-26 */
    uchar magic[2];             /* Magic number for old cookies      27-28 */
    uchar capabilities[2];      /* capabilities (future stuff)       29-30 */
    uchar version;              /* cookie version number              31    */
};

/*
 * Vendor codes: now in cpu.h
 */


/*
 * Interface codes:
 */
#define INTERFACE_1E1T		1
#define INTERFACE_2E		2
#define INTERFACE_1R1T		3
#define INTERFACE_1R1E		4
#define INTERFACE_2R		5
#define INTERFACE_1E1B		6
#define INTERFACE_1E2T		7
#define INTERFACE_1R2T		8
#define INTERFACE_1E1T1B	9
#define INTERFACE_1R1T1B	10
#define INTERFACE_1E2T1B	11
#define INTERFACE_1R2T1B	12
#define INTERFACE_1R1B		13
#define INTERFACE_1E2T8A	14
#define INTERFACE_1R2T8A	15
/*
 * For Fiji, which is a derivative of Cancun, there will be an ISA
 * bus interface. This interface will be in various combinations with
 * an Ethernet or TR, serial and BRI. To avoid proliferating the no of
 * interface codes, only two new codes will be introduced. These codes
 * will tell us if the board is the Ethernet or TR version with ISA
 * bus and 1 serial interface. The presence of the 2nd serial and
 * BRI will be detected from the daughter card indicator flag in
 * ADRSPC_FIJI_CONTROL. This is being done to allow the daughter card
 * to be field upgradable without having to change the cookie value.
 */
#define INTERFACE_1E1T1I        16    /* I = ISA bus interface */
#define INTERFACE_1R1T1I        17
#define INTERFACE_1R2T1E        18    /* next 3 for campeche */
#define INTERFACE_2R2T          19
#define INTERFACE_2E2T          20
/*
 * Galapagos is hublet derivative of Cancun.  Define two interface
 * codes, and use HUB_MISC_REG to determine if daughtercard is
 * present.
 * Token Ring Galapagos got canned.  Removed its cookie value of 22.
 */
#define INTERFACE_1E2Thub	21
#define INTERFACE_1E2T1Bhub     23    /* cabrillo */
/* 'U' interface is an integrated NT1 on a BRI interface */
#define INTERFACE_1E1N		24    /* FrankenStorm -- Sapphire with NT1 */
#define INTERFACE_1E2T1B2AS   25    /* Ethernet Cobra */
#define INTERFACE_1R2T1B2AS   26    /* Token Ring Cobra */
/*
 * For BOA project, the W interface is the modularize WAN module port
 *  which allow integrated DSU/CSU module or the regular 5-in-1 serial
 *  module.
 */
#define INTERFACE_1E1B2W	27 	/* Ethernet BOA */
#define INTERFACE_1R1B2W	28 	/* Token Ring BOA */
/*
 * Corsica is a c2520 plus 8 async ports and it is in Catalyst 5000
 *  line card format.
 */
#define INTERFACE_1E2T1B2AS8A   29      /* Corsica */

/*
 * Processor codes:
 * When adding a new processor code, please make sure to also add it
 * to boot/src-c4000/cookie.h
 */
#define PROCESSOR_PAN		0 /* IGS processor */
#define PROCESSOR_MERLOT	1 /* Token ring IGS processor */
#define PROCESSOR_XX		2 
#define PROCESSOR_BRUT          3
#define PROCESSOR_BASS          4
#define PROCESSOR_CRISTAL       5
#define PROCESSOR_CANCUN        6
#define PROCESSOR_SIERRA        7
#define PROCESSOR_SAPPHIRE      8
#define PROCESSOR_VOLCANO	9
#define PROCESSOR_C3500         10
#define PROCESSOR_JANEIRO	11
#define PROCESSOR_MAX		12
#define CISCOPRO_FLAG		0x80 /* Bit is set if platform is branded
				        for CiscoPro images only */

/*
 * Magic numbers necessary to honor capability[1] bits
 */
#define COOKIE_MAGIC_0          0xDE
#define COOKIE_MAGIC_1          0xAD

/*
 * cookie->capabilities[1] bits
 * Features are encoded bitwise in cababilites,
 * In decoding, 0 = include feature; 1 exclude feature:
 *      Bit             Feature
 *      ---             -------
 *       0              CiscoET Platform - Extended Temperature
 *       1              Unused
 *       2              Unused
 *       3              Unused
 *       4              Unused
 *       5              Unused
 *       6              Unused
 *       7              CiscoPro Platform
 */
#define CISCOET_PLATFORM	0x01 /* Platform branded for Extended
					temperature mechanical option */
#define CISCOPRO_PLATFORM	0x80 /* Platform branded for CiscoPro images
					only */

/*
 * Kludge here:  CiscoPro boxes come with the PROCESSOR_CISCOPRO bit set in 
 *	cookie.processor and the CISCOPRO_PLATFORM bit set in
 *	cookie.capabilities[1].  The PROCESSOR_CISCOPRO bit disallows pre-
 *	CiscoPro images from running on CiscoPro boxes, even if old ROMs
 *	are installed.  This bit is turned off by the ROM monitor right
 *	after reading the cookie, so all other ROM monitor code will run
 *	as always (only post-CiscoPro ROM monitors do this).  The ciscopro_
 *	flag allows the CPU_CISCOPRO bit to be turned on appropriately
 *	in the CPU type returned from the EMT_PROCTYPE trap.  This is
 *	intended to confuse old system images that have no business running
 *	on a CiscoPro platform (ie. unknown CPU type).  Newer system images
 *	mask the CPU_CISCOPRO bit back off, but only if the image is branded
 *	for use on a CiscoPro platform (ie. CISCOPRO_IMAGE is defined).
 *	(whew!!, but it works).
 */

#endif __COOKIE_H__
