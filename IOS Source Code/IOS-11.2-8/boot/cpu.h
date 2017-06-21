/* $Id: cpu.h,v 3.8.4.3 1996/04/08 18:13:27 smackie Exp $
 * $Source: /release/112/cvs/Xboot/cpu.h,v $
 *------------------------------------------------------------------
 * cpu.h -- define the cpu types and associated strings
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1996,97 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cpu.h,v $
 * Revision 3.8.4.3  1996/04/08  18:13:27  smackie
 * Add new CPU types for LightStream and Grand Junction. (CSCdi53907)
 * Branch: California_branch
 *
 * Revision 3.8.4.2  1996/04/05  02:41:42  yoko
 * CSCdi53681:  add new cpu types
 * Branch: California_branch
 * define new cpu types and emt calls
 *
 * Revision 3.8.4.1  1996/03/26  18:24:35  enf
 * CSCdi52111:  Need to reserve #defines for new platform
 * Branch: California_branch
 * Define CPU_VOLCANO
 *
 * Revision 3.8  1996/03/09  23:33:31  jturner
 * CSCdi51191:  Make cpu type addition
 * Grab cpu type for new platform development.
 *
 * Revision 3.7  1996/01/30  21:10:55  vandys
 * CSCdi46080:  Deep six CS500 in source base
 *
 * Revision 3.6  1996/01/12  01:34:51  lcheung
 * add new cpu type to arkansas
 * CSCdi46793:  add new cpu type
 *
 * Revision 3.5  1995/12/31  23:25:04  anil
 * i86 CPU
 *
 * Revision 3.4  1995/12/03  17:54:49  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.3  1995/11/20  19:35:09  mbeesley
 * CSCdi43065:  need to reserve cpu types for future development
 *
 * Revision 3.2  1995/11/17  07:40:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:13:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/10/20  00:53:57  ljiang
 * CSCdi42394:  Add vendor codes for cisco partners
 *
 * Revision 2.2  1995/10/04  22:57:24  hampton
 * Added a new define for an unknown processor family.  [CSCdi41098]
 *
 * Revision 2.1  1995/06/07  19:11:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	CPU_SMI8	0	/* never used -- predates cisco */
#define	CPU_CADLINC	1	/* never used -- predates cisco */
#define	CPU_SMI10	2	/* never used -- predates cisco */
#define	CPU_FTI10	3	/* never used -- predates cisco */
#define	CPU_CSC1	4	/* also called P20, 68000 @ 10MHz */
#define	CPU_UNIX	5	/* we are a UNIX user mode process */
#define	CPU_CSC2	6	/* also called PIPPIN, 68020 @ 12MHz */
#define	CPU_MPU12	7	/* never used */
#define	CPU_CSC3	8	/* 68020 @ 30 MHz */
#define CPU_STS1	9	/* 68010 in CMC Small Terminal Server */
#define CPU_PAN		10	/* IGS (was called Pancake, or HYBRIDGE II),
                                   16 MHz 68020 */
#define CPU_MERLOT	11	/* Merlot (Token ring IGS), 20 Mhz 68030 */
#define CPU_LEMONADE	12	/* Lemonade (16 line terminal server) (RIP) */
#define	CPU_CSC4	13	/* 68EC040 @ 25 MHz */
#define	CPU_XX  	14	/* XX (the 4000), 40 Mhz 68030 */
#define CPU_IGS_BRUT    15	/* IGS-BRUT,68030 20 MHz, w/ no CPU memory */
#define CPU_RP1		16	/* 68EC040 @ 25 MHz */
#define CPU_BASS	17	/* Bass and Pinot */
#define CPU_CRISTAL	18	/* Cristal (3 port merlot) */
#define CPU_CANCUN	19	/* Cancun like Cristal with ASIC's and BRUT */
				/* Timers */
#define CPU_SIERRA      20      /* The C4000 with the Orion chip (R4600) */
#define CPU_RSP		21	/* integrated route-switch processor */
#define CPU_SAPPHIRE    22      /* 68360 aka "QUICC chip" */
#define CPU_SYNALC	23	/* Synergy ATM Line Card (25MHz 68EC030) */
#define CPU_VIP		24	/* R4600 based IP running IOS */
#define CPU_C7100	25	/* Predator */
#define CPU_i86		26	/* Intel x86    */
#define CPU_RHINO       27      /* LS1010 ATM Switch*/
#define CPU_BRASIL      28      /* The AS5200, a Cancun superset. Define used */
                                /* only for Brasil family type creation. */
#define CPU_VOLCANO	29	/* 68360 (C100X) + Wan Module slot */
#define CPU_C3500       30      /* MARs   */
#define CPU_NP1         31      /* LightStream NP1 - 25Mhz 68040 */
#define CPU_ASP         32      /* LightStream ASP - 75Mhz PowerPC 603 */
#define CPU_MALIBU      33      /* Grand Junction FE Switch - PowerPC 403 */
#define CPU_RINCON      34      /* StrataCom RINCON Access Switch - 33MHz 
                                   MC68360 */
#define CPU_JANEIRO     35      /* AS5300, an access server router based on 
                                   Sierra platform */ 
#define CPU_BFRP        36      /* BFR Route Processor */
#define CPU_BFLC        37      /* BFR Line Card */

#define NITRO           42      /* Dial Aggregation Cards - Nitro */

/****************************************************************
 * if you add cpu types, please add entries in delay_table
 * for spin wait delays in ../sys/os/delay_table.c, and ../boot/asm.S.
 * and not to forget to add the string to cpu.c.
 * and to the chassis MIB definitions and code as well
 * thank you for your consideration.
 ****************************************************************/

#define CPU_UNKNOWN	43	/* LAST VALUE - unknown processor */

#define	CPU_CISCOPRO_MASK 0x80	/* Mask CPU type for CiscoPro platforms
				   to hide the CPU's true identity, 
				   but only for EMT_PROCTYPE calls */

/*
 * CPU family defines.  As family grow please update the following list
 */

#define FAMILY_UNIX     CPU_UNIX
#define FAMILY_CSC2     CPU_CSC2
#define FAMILY_RP1      CPU_RP1
#define FAMILY_PAN      CPU_PAN
#define FAMILY_XX       CPU_XX
#define FAMILY_SIERRA	CPU_SIERRA
#define FAMILY_RSP      CPU_RSP
#define FAMILY_C1000	CPU_SAPPHIRE
#define FAMILY_SYNALC	CPU_SYNALC
#define FAMILY_VIP	CPU_VIP
#define FAMILY_C7100	CPU_C7100
#define FAMILY_i86		CPU_i86
#define FAMILY_BRASIL   CPU_BRASIL
#define FAMILY_VOLCANO	CPU_VOLCANO
#define FAMILY_C3500    CPU_C3500
#define FAMILY_NP1      CPU_NP1
#define FAMILY_ASP      CPU_ASP
#define FAMILY_MALIBU   CPU_MALIBU
#define FAMILY_JANEIRO  CPU_JANEIRO
#define FAMILY_UNKNOWN	-1

/*
 * VENDOR types defines.  As vendors grow, please update the following list
 */
#define VENDOR_CISCO		1
#define VENDOR_SYNOPTICS	2
#define VENDOR_CHIPCOM		3
#define VENDOR_CABLETRON	4
#define VENDOR_DEC              5
#define VENDOR_NCR		6
#define VENDOR_USROBOTICS       7
#define VENDOR_ALCATEL          8
#define VENDOR_NEC              9
#define VENDOR_DSC              10
#define VENDOR_MICROCOM         11
#define VENDOR_OKI              12
#define VENDOR_UBN              13
#define VENDOR_HP               14
#define VENDOR_MAX              15
/****************************************************************
 * if you add a vendor, you will need to update chassisPartner in the Chassis
 * MIB, enumerations must match, never change an enumeration, only add them.
 * Mail to cana@cisco.com for enumeration numbering assistant.
 *
 * Also, you'll need to add the vendor string to the les/init_les.c.
 */

#define IS_FAMILY(family)	((family) == cpu_family)

#define IS_CPU(cpu)		((cpu) == cpu_type)

#define IS_VENDOR(a)		((a) == cookie->vendor)
