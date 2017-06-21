/* $Id: sysmgt_dlurmib.h,v 3.1.4.1 1996/05/17 11:19:23 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/hes/sysmgt_dlurmib.h,v $
 *------------------------------------------------------------------
 * dlur mib rp-cip communication structure
 *
 * Jan 96, Barry Chan
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sysmgt_dlurmib.h,v $
 * Revision 3.1.4.1  1996/05/17  11:19:23  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.1  1996/03/21  03:21:47  bchan
 * CSCdi52147:  Add tn3270 server new files to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/03/20  07:01:12  bchan
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef sysmgt_dlurmib_h
#define sysmgt_dlurmib_h


#define PuNameLen   18
#define DlusNameLen 18


/* 
 * Note - The lengths of the following structures must be 4-byte multiples.
 *        In addition, 'long' elements must be 4-byte aligned.
 */


/*
 * The dlurNodeCapabilities family. 
 */


typedef
struct _dlurNodeCapabilities_T {
        unsigned char   dlurReleaseLevel[4];
        long            dlurANSsupport;
        long            dlurMultiSubnetSupport;
} dlurNodeCapabilities_T;


/*
 * The dlurPuEntry family. 
 */


typedef
struct _dlurPuEntry_T {
        long            dlurPuIndex;
        long            dlurPuStatus;
        long            dlurPuANSsupport;
        long            dlurPuLocation;
        long            dlurPuDlusSessnStatus;
        char            dlurPuName[PuNameLen];
        char            dlurPuActiveDlusName[DlusNameLen];
        char            dlurPuDefPrimDlusName[DlusNameLen];
        char            dlurPuDefBackDlusName[DlusNameLen];
} dlurPuEntry_T;


/*
 * The dlurDlusEntry family. 
 */


typedef
struct _dlurDlusEntry_T {
        long            dlurDlusIndex;
        char            dlurDlusName[DlusNameLen];
        char            align[2];
        long            dlurDlusSessnStatus;
} dlurDlusEntry_T;


typedef enum sysmgt_dlur_table_ {
    SYSMGT_DLUR_NODE_CAP,
    SYSMGT_DLUR_PU_ENTRY,
    SYSMGT_DLUR_DLUS_ENTRY,
    SYSMGT_DLUR_MAXIMUM
} sysmgt_dlur_table_t;

void init_cipdlurmib(void);


#endif



