/* $Id: regdefs.c,v 1.1.4.1 1996/05/09 15:00:28 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/regdefs.c,v $
 *------------------------------------------------------------------
 * regdefs.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: regdefs.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:28  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:25  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:42  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:37  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:11  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:06  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <string.h>
#include <defines.h>
#include "pmon.h"

/*************************************************************
*  regdefs.c
*	This file contains all the register definitions used
*	by the 'r' (display regs) command. There are two types
*	of definitions: RegList that defines all the registers, 
*	and RegSpec's that define the bit fields within a register.
*/

char *bsvalues[] = {"2","4","8","16",0};

#ifdef LR33300
char *pgszval333[] = {"16", "32", "64", "128", "256", "512", "1024", "2048", 0};
#endif

char *pgszval33k[] = {"64","128","256","512","1024","2048","4096","8192",0};

#ifdef LR33000 /* 33000 & 33050 */
RegSpec Cfgreg33000[] = {
	{1,23,"ICD",2,0,0},
	{1,22,"DCD",2,0,0},
	{2,20,"IBS",0,bsvalues,0},
	{2,18,"DBS",0,bsvalues,0},
	{1,17,"IW",2,0,0},
	{4,13,"IWAIT",10,0,0},
	{1,12,"PW",2,0,0},
	{4,8,"PWAIT",10,0,0},
	{1,7,"CS",2,0,0},
	{1,6,"PE",2,0,0},
	{1,5,"DGE",2,0,0},
	{1,4,"BFD",2,0,0},
	{1,3,"REN",2,0,0},
	{1,2,"RPC",2,0,0},
	{1,1,"CL",2,0,0},
	{1,0,"DCE",2,0,0},
	{0}};

RegSpec Cfgreg33050[] = {
	{1,31,"WBE",2,0,0},
	{1,30,"BEN",2,0,0},
	{3,27,"PGSZ",0,pgszval33k,0},
	{1,25,"IW8",2,0,0},
	{1,24,"PW8",2,0,0},
	{1,23,"ICD",2,0,0},
	{1,22,"DCD",2,0,0},
	{2,20,"IBS",0,bsvalues,0},
	{2,18,"DBS",0,bsvalues,0},
	{1,17,"IW",2,0,0},
	{4,13,"IWAIT",10,0,0},
	{1,12,"PW",2,0,0},
	{4,8,"PWAIT",10,0,0},
	{1,7,"CS",2,0,0},
	{1,6,"PE",2,0,0},
	{1,5,"DGE",2,0,0},
	{1,4,"BFD",2,0,0},
	{1,3,"REN",2,0,0},
	{1,2,"RPC",2,0,0},
	{1,1,"CL",2,0,0},
	{1,0,"DCE",2,0,0},
	{0}};
#endif

#ifdef LR33020 /* 33020 & 33120 */
RegSpec Cfgreg33120[] = {
	{1,31,"XLE",2,0,0},
	{1,30,"BEN",2,0,0},
	{3,27,"PGSZ",0,pgszval33k,0},
	{1,23,"ICD",2,0,0},
	{1,22,"DCD",2,0,0},
	{2,20,"IBS",0,bsvalues,0},
	{2,18,"DBS",0,bsvalues,0},
	{1,17,"IW",2,0,0},
	{4,13,"IWAIT",10,0,0},
	{1,12,"PW",2,0,0},
	{4,8,"PWAIT",10,0,0},
	{3,5,"BNK",10,0,0},
	{1,4,"BFD",2,0,0},
	{1,3,"REN",2,0,0},
	{1,2,"RPC",2,0,0},
	{1,1,"CL",2,0,0},
	{1,0,"DCE",2,0,0},
	{0}};

RegSpec Cfgreg33020[] = {
	{1,30,"BEN",2,0,0},
	{3,27,"PGSZ",0,pgszval33k,0},
	{1,23,"ICD",2,0,0},
	{1,22,"DCD",2,0,0},
	{2,20,"IBS",0,bsvalues,0},
	{2,18,"DBS",0,bsvalues,0},
	{1,17,"IW",2,0,0},
	{4,13,"IWAIT",10,0,0},
	{1,12,"PW",2,0,0},
	{4,8,"PWAIT",10,0,0},
	{3,5,"BNK",10,0,0},
	{1,4,"BFD",2,0,0},
	{1,3,"REN",2,0,0},
	{1,2,"RPC",2,0,0},
	{1,1,"CL",2,0,0},
	{1,0,"DCE",2,0,0},
	{0}};

RegSpec c2pscomm[] = { /* creg25 */
	{1,7,"IO",2,0,0},
	{1,4,"CLKINH",2,0,0},
	{1,3,"RCVINT",2,0,0},
	{1,2,"TXINT",2,0,0},
	{1,1,"TXEN",2,0,0},
	{1,0,"INTHTX",2,0,0},
	{0}};

RegSpec c2psstat[] = { /* creg24 */
	{1,7,"FERR",2,0,0},
	{1,6,"PAR",2,0,0},
	{1,5,"RXIN",2,0,0},
	{1,4,"RXBF",2,0,0},
	{1,3,"TXBE",2,0,0},
	{1,2,"TXIN",2,0,0},
	{1,1,"CLKX",2,0,0},
	{1,0,"CLK",2,0,0},
	{0}};

RegSpec c2config120[] = { /* 33120 creg9 */
	{1,24,"BENDS",2,0,0},	
	{1,23,"BW8",2,0,0},
	{1,22,"INLVDBW",2,0,0},
	{1,21,"QSF",2,0,0},
	{1,19,"HCUR",2,0,0},
	{1,18,"DRAM",2,0,0},
	{1,17,"SAM",2,0,0},
	{1,16,"VRAM",2,0,0},
	{1,15,"D3",2,0,0},
	{3,12,"IORV3",10,0,0},
	{4,8,"IOWAIT3",10,0,0},
	{1,7,"D2",2,0,0},
	{3,4,"IORV2",10,0,0},
	{4,0,"IOWAIT2",10,0,0},
	{0}};

RegSpec c2config020[] = { /* 33020 creg9 */
	{1,21,"QSF",2,0,0},
	{1,19,"HCUR",2,0,0},
	{1,18,"DRAM",2,0,0},
	{1,17,"SAM",2,0,0},
	{1,16,"VRAM",2,0,0},
	{1,15,"D3",2,0,0},
	{3,12,"IORV3",10,0,0},
	{4,8,"IOWAIT3",10,0,0},
	{1,7,"D2",2,0,0},
	{3,4,"IORV2",10,0,0},
	{4,0,"IOWAIT2",10,0,0},
	{0}};

RegSpec c2vhwconfig[] = { /* dreg23 */
	{1,31,"EN",2,0,0},
	{1,18,"CROSSINV",2,0,0},
	{1,17,"CROSSDAT",2,0,0},
	{1,16,"CROSSEN",2,0,0},
	{1,15,"CSYNCEN",2,0,0},
	{1,14,"OVRSCN",2,0,0},
	{1,13,"VEND",2,0,0},
	{1,12,"VSYNCOUTEN",2,0,0},
	{1,11,"VSYNCINEN",2,0,0},
	{1,10,"HWCRSR",2,0,0},
	{2,8,"SHFT",10,0,0},
	{1,7,"VSPOS",2,0,0},
	{1,6,"HSPOS",2,0,0},
	{1,5,"VSINT",2,0,0},
	{1,4,"HLINT",2,0,0},
	{1,1,"VLINTEN",2,0,0},
	{1,0,"HLINTEN",2,0,0},
	{0}};

RegSpec c2srcshift120[] = { /* 33120 creg1 */
	{5,5,"INC",10,0,0,},
	{5,0,"SHIFT",10,0,0},
	{0}};

char *pixszvalues[] = {"1","2","4","8","16","32","X","X",0};

RegSpec c2gcpcntrl[] = { /* creg4 */
	{2,13,"SPCLWE",10,0,0},
	{1,12,"YDIR",2,0,0},
	{1,11,"XDIR",2,0,0},
	{1,9,"WO",2,0,0},
	{1,8,"MW",2,0,0},
	{1,6,"MASK",2,0,0,},
	{1,4,"EXPND",2,0,0,},
	{1,3,"TRAN",2,0,0,},
	{3,0,"PIXSIZ",0,pixszvalues,0},
	{0}};
#endif /* LR33020 */

#ifdef LR33300	/* used for 33300 & 33310 */

#if 0
char *dbsvalues[] = {"2", "4", "8", "16", "32", "64", "U","U",0};
char *dptvalues[] = {"1","2","4","8", 0};
char *rpcvalues[] = {"2", "3", "4", "5", 0};
char *rcdvalues[] = {"2","3","4","5", 0};
char *clvalues[] = {"1.5", "2.5", "3.5", "4.5", "5.5", "6.5", "7.5", "8.5", 0};
#endif

RegSpec BiuCfgreg[] = {
        {1,17,"NOSTR",2,0,0},
        {1,16,"LDSCH",2,0,0},
	{1,15,"BGNT",2,0,0},
	{1,14,"NOPAD",2,0,0},
	{1,13,"RDPRI",2,0,0},
	{1,12,"INTP",2,0,0},
	{1,11,"IS1",2,0,0},
	{1,10,"IS0",2,0,0},
	{2,8,"IBLKSZ",0,bsvalues,0},
	{1,7,"DS1",2,0,0},
	{1,6,"DS0",2,0,0},
	{2,4,"DBLKSZ",0,bsvalues,0},
	{1,3,"RAM",2,0,0},
	{1,2,"TAG",2,0,0},
	{1,1,"INV",2,0,0},
	{1,0,"LOCK",2,0,0},
	{0}};

#ifdef ATMIZER
char *cellszvals[] = {"64", "60", "56", "52", 0};
char *bufszvals[] = {"4", "8", "16", "32", 0};
char *cbsvals[] = {"2", "4", "-", "-", 0};

RegSpec Scrreg[] = {
	{2,30,"RO",0,cellszvals,0},
	{2,28,"TO",0,cellszvals,0},
	{1,27,"RI",2,0,0},
	{1,26,"TI",2,0,0},
	{1,24,"BM",2,0,0},
	{3,21,"HH",2,0,0},
	{2,19,"CBS",0,cbsvals,0},
	{3,16,"BS",0,bufszvals,0},
	{1,15,"CRCERR",2,0,0},
	{1,14,"SAFE",2,0,0},
	{6,8,"CH",10,0,0},
	{1,6,"BOCERR",2,0,0},
	{1,5,"RWD",2,0,0},
	{1,4,"AWD",2,0,0},
	{3,0,"TAF",10,0,0},
        {0}};
#else

#if 0
RegSpec DramCfgreg[] = {
        {1,28,"DLP1",2,0,0}, 
        {1,27,"SYNC",2,0,0},
        {1,26,"SCFG",2,0,0},
        {1,25,"DMARDY",2,0,0}, 
        {3,22,"DMABLK",0,dbsvalues,0}, 
        {2,20,"DPTH",0,dptvalues,0}, 
	{1,19,"RDYW",2,0,0},
	{3,16,"PGSZ",0,pgszval333,0},
	{1,15,"PGMW",2,0,0},
	{2,13,"RFWE",2,0,0},
	{1,12,"RFEN",2,0,0},
	{1,11,"RDYEN",2,0,0},
	{1,10,"BFD",2,0,0},
	{1,9,"PE",2,0,0},
	{2,7,"RPC",0,rpcvalues,0},
	{2,5,"RCD",0,rcdvalues,0},
	{1,4,"CS",2,0,0},
	{3,1,"CL",0,clvalues,0},
	{1,0,"DCE",2,0,0},
        {0}};
#else
RegSpec DramCfgreg[] = {
        {1,28,"DLP1",2,0,0}, 
        {1,27,"SYNC",2,0,0},
        {1,26,"SCFG",2,0,0},
        {1,25,"DMARDY",2,0,0}, 
        {3,22,"DMABLK",10,0,0}, 
        {2,20,"DPTH",10,0,0}, 
	{1,19,"RDYW",2,0,0},
	{3,16,"PGSZ",10,0,0},
	{1,15,"PGMW",2,0,0},
	{2,13,"RFWE",2,0,0},
	{1,12,"RFEN",2,0,0},
	{1,11,"RDYEN",2,0,0},
	{1,10,"BFD",2,0,0},
	{1,9,"PE",2,0,0},
	{2,7,"RPC",10,0,0},
	{2,5,"RCD",10,0,0},
	{1,4,"CS",2,0,0},
	{3,1,"CL",10,0,0},
	{1,0,"DCE",2,0,0},
        {0}};
#endif

RegSpec SpecN[] = {
        {4,24,"INHIBIT",10,0,0},
        {1,23,"EXTGNT",2,0,0}, 
        {1,22,"16WIDE",2,0,0}, 
        {1,21,"8WIDE",2,0,0}, 
        {1,20,"PENA",2,0,0}, 
        {1,19,"CACHED",2,0,0}, 
        {2,17,"CSDLY",10,0,0}, 
        {1,16,"BLKENA",2,0,0}, 
        {3,13,"BLKWAIT",10,0,0}, 
        {6,7,"REC",10,0,0}, 
        {1,6,"WAITENA", 2,0,0}, 
        {6,0,"WAIT",10,0,0}, 
        {0}}; 
#endif
#endif
       
#ifdef FLOATINGPT
char *rmvalues[] = {"RN","RZ","RP","RM",0};

RegSpec cp1_csr[] = {
	{1,23,"C",2,0,0},
	{1,17,"EE",2,0,0},
	{1,16,"EV",2,0,0},
	{1,15,"EZ",2,0,0},
	{1,14,"EO",2,0,0},
	{1,13,"EU",2,0,0},
	{1,12,"EI",2,0,0},
	{1,11,"TV",2,0,0},
	{1,10,"TZ",2,0,0},
	{1, 9,"TO",2,0,0},
	{1, 8,"TU",2,0,0},
	{1, 7,"TI",2,0,0},
	{1, 6,"SV",2,0,0},
	{1, 5,"SZ",2,0,0},
	{1, 4,"SO",2,0,0},
	{1, 3,"SU",2,0,0},
	{1, 2,"SI",2,0,0},
	{1, 0,"RM",0,rmvalues,0},
	{0}};
#endif

RegSpec TimerCnl[] = {
	{1,2,"CE",2,0,0},
	{1,1,"IE",2,0,0},
	{1,0,"INT",2,0,0},
	{0}};

RegSpec Stat[] = {
	{4,28,"CU",2,0,0},
	{1,22,"BEV",2,0,0},
	{1,21,"TS",2,0,1},
	{1,20,"PE",2,0,1},
	{1,19,"CM",2,0,0},
	{1,18,"PZ",2,0,0},
	{1,17,"SWC",2,0,0},
	{1,16,"ISC",2,0,0},
	{8,8,"IM&SW",2,0,0},
	{1,5,"KUo",2,0,0},
	{1,4,"IEo",2,0,0},
	{1,3,"KUp",2,0,0},
	{1,2,"IEp",2,0,0},
	{1,1,"KUc",2,0,0},
	{1,0,"IEc",2,0,0},
	{0}};

char *excodes[] = {
	"Int","MOD","TLBL","TLBS","AdEL","AdES","IBE","DBE",
	"Sys","Bp","RI","CpU","Ovf","Resv","Resv","Resv",0};

RegSpec Caus[] = {
	{1,31,"BD",2,0,1},
	{2,28,"CE",10,0,1},
	{6,10,"IP",2,0,1},
	{2,8,"SW",2,0,0},
	{4,2,"EXCODE",0,excodes,1},
	{0}};

RegSpec PRid[] = {
	{8,8,"IMP",10,0,1},
	{8,0,"Rev",10,0,1},
	{0}};

RegSpec DCIC[] = {
	{1,31,"TR",2,0,0},
	{1,30,"UD",2,0,0},
	{1,29,"KD",2,0,0},
	{1,28,"TE",2,0,0},
	{1,27,"DW",2,0,0},
	{1,26,"DR",2,0,0},
	{1,25,"DAE",2,0,0},
	{1,24,"PCE",2,0,0},
	{1,23,"DE",2,0,0},
	{1,13,"D",2,0,0},
	{1,12,"I",2,0,0},
	{1,5,"T",2,0,0},
	{1,4,"W",2,0,0},
	{1,3,"R",2,0,0},
	{1,2,"DA",2,0,0},
	{1,1,"PC",2,0,0},
	{1,0,"DG",2,0,0},
	{0}};

RegList reglist[] = {
	{&Gpr[0],0,"zero","0",0,(F_ALL|F_ANAME|F_GPR|F_RO)},
	{&Gpr[1],0,"at","1",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[2],0,"v0","2",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[3],0,"v1","3",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[4],0,"a0","4",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[5],0,"a1","5",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[6],0,"a2","6",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[7],0,"a3","7",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[8],0,"t0","8",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[9],0,"t1","9",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[10],0,"t2","10",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[11],0,"t3","11",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[12],0,"t4","12",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[13],0,"t5","13",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[14],0,"t6","14",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[15],0,"t7","15",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[16],0,"s0","16",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[17],0,"s1","17",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[18],0,"s2","18",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[19],0,"s3","19",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[20],0,"s4","20",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[21],0,"s5","21",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[22],0,"s6","22",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[23],0,"s7","23",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[24],0,"t8","24",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[25],0,"t9","25",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[26],0,"k0","26",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[27],0,"k1","27",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[28],0,"gp","28",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[29],0,"sp","29",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[30],0,"s8","30",0,(F_ALL|F_ANAME|F_GPR)},
	{&Gpr[31],0,"ra","31",0,(F_ALL|F_ANAME|F_GPR)},
	{&Hi,0,"HI","HI",0,F_ALL},
	{&Lo,0,"LO","LO",0,F_ALL},
	{&Status,Stat,"C0_SR","SR",0,(F_ALL)},
	{&Cause,Caus,"C0_CAUSE","CAUSE",0,(F_ALL)},
	{&Prid,PRid,"C0_PRID","PRID",0,(F_ALL)},
	{&Epc,0,"C0_EPC","EPC",0,(F_ALL)},
	{&Badva,0,"C0_BADVA","BADVA",0,(F_ALL)},
#ifdef LR33000
	{&Bpc,0,"C0_BPC","BPC",0,(F_ALL)},
	{&Bda,0,"C0_BDA","BDA",0,(F_ALL)},
	{&Dcic,DCIC,"C0_DCIC","DCIC",0,(F_ALL)},
#ifdef ATMIZER
	{&Bpcm,0,"C0_BPCM","BPCM",0,(F_333)},
	{&Bdam,0,"C0_BDAM","BDAM",0,(F_333)},
	{(word *)M_BIU,BiuCfgreg,"M_BIU","BIU",0,(F_333)},
	{(word *)M_CREDIT,0,"M_CREDIT","CREDIT",0,(F_ATM|F_RO)},
	{(word *)M_CONFIG,0,"M_CONFIG","CONFIG",0,(F_ATM|F_WO)},
	{(word *)M_STALL,0,"M_STALL","STALL",0,(F_ATM|F_WO)},
	{(word *)M_PRPC,0,"M_PRPC","PRPC",0,(F_ATM)},
	{(word *)M_RXBUFPTR,0,"M_RXBUFPTR","RXBUFPTR",0,(F_ATM|F_RO)},
	{(word *)M_TXFIFO,0,"M_TXFIFO","TXFIFO",0,(F_ATM|F_WO)},
	{(word *)M_RXACK,0,"M_RXACK","RXACK",0,(F_ATM|F_WO)},
	{(word *)M_GPRR,0,"M_GPRR","GPRR",0,(F_ATM|F_WO)},
	{(word *)M_SCR,Scrreg,"M_SCR","SCR",0,(F_ATM)},
	{(word *)M_AHI,0,"M_AHI","AHI",0,(F_ATM|F_WO)},
	{(word *)M_CRC32,0,"M_CRC32","CRC32",0,(F_ATM)},
	{(word *)M_ASUBR,0,"M_ASUBR","ASUBR",0,(F_ATM|F_WO)},
#else /* !ATMIZER */
	{(word *)M_CFGREG,Cfgreg33000,"M_CFG","CFG",0,(F_00)},
	{(word *)M_CFGREG,Cfgreg33050,"M_CFG","CFG",0,(F_50)},
#ifdef LR33020
	{(word *)M_CFGREG,Cfgreg33020,"M_CFG","CFG",0,(F_020)},
	{(word *)M_CFGREG,Cfgreg33120,"M_CFG","CFG",0,(F_120)},
#endif /* !LR33020 */
#ifdef LR33300
	{&Bpcm,0,"C0_BPCM","BPCM",0,(F_333)},
	{&Bdam,0,"C0_BDAM","BDAM",0,(F_333)},
	{(word *)M_BIU,BiuCfgreg,"M_BIU","BIU",0,(F_333)},
	{(word *)M_DRAM,DramCfgreg,"M_DRAM","DRAM",0,(F_333)},
	{(word *)M_SRAM,SpecN,"M_SRAM","SRAM",0,(F_333)},
	{(word *)M_SPEC0,SpecN,"M_SPEC0","SPEC0",0,(F_333)},
	{(word *)M_SPEC1,SpecN,"M_SPEC1","SPEC1",0,(F_333)},
	{(word *)M_SPEC2,SpecN,"M_SPEC2","SPEC2",0,(F_333)},
	{(word *)M_SPEC3,SpecN,"M_SPEC3","SPEC3",0,(F_333)},
#endif
	{(word *)M_TC1,TimerCnl,"M_TC1","TC1",0,(F_00|F_50|F_333)},
	{(word *)M_TIC1,0,"M_TIC1","TIC1",0,(F_00|F_50|F_333)},
	{(word *)M_TC2,TimerCnl,"M_TC2","TC2",0,(F_ALL)},
	{(word *)M_TIC2,0,"M_TIC2","TIC2",0,(F_ALL)},
	{(word *)M_RTIC,0,"M_RTIC","RTIC",0,(F_ALL)},
#endif /* !ATMIZER */
#else /* must be plain vanilla R3000 */
	{&Context,0,"C0_CONTEXT","CONTEXT",0,(F_ALL)},
	{&Entrylo,0,"C0_ENTRYLO","ENTRYLO",0,(F_ALL)},
	{&Entryhi,0,"C0_ENTRYHI","ENTRYHI",0,(F_ALL)},
	{&Index,0,"C0_INDEX","INDEX",0,(F_ALL)},
	{&Random,0,"C0_RANDOM","RANDOM",0,(F_ALL)},
#endif /* LR33000 */
#ifdef FLOATINGPT
	{&Fcr,cp1_csr,"C1_CSR","CSR",0,(F_ALL)},
	{&Fid,0,"C1_FRID","FRID",0,(F_ALL|F_RO)},
#endif
#ifdef LR33020
#ifdef ALPHABETICAL
	{(word *)cXc2,0,"C2_COLOR0","COLOR0",2,(F_20|F_CF)},
	{(word *)cXc2,0,"C2_COLOR1","COLOR1",3,(F_20|F_CF)},
	{(word *)cXc2,c2config020,"C2_CONFIG","CONFIG",9,(F_020|F_CF)},
	{(word *)cXc2,c2config120,"C2_CONFIG","CONFIG",9,(F_120|F_CF)},
	{(word *)cXc2,0,"C2_PLANEMASK","PLANEMASK",6,(F_20|F_CF)},
	{(word *)cXc2,0,"C2_RASTEROP","RASTEROP",5,(F_20|F_CF)},
	{(word *)cXc2,0,"C2_SRCSHIFT","SRCSHIFT",1,(F_020|F_CF)},
	{(word *)cXc2,c2srcshift120,"C2_SRCSHIFT","SRCSHIFT",1,(F_120|F_CF)},
	{(word *)cXc2,0,"C2_SRCSKEW","SRCSKEW",0,(F_20|F_CF)},
	{(word *)cXc2,c2gcpcntrl,"C2_GCPCNTRL","GCPCNTRL",4,(F_20|F_CF)},
	{(word *)cXc2,c2psstat,"C2_PSSTAT","PSSTAT",24,(F_20|F_CF|F_RO)},
	{(word *)cXc2,c2pscomm,"C2_PSCOMM","PSCOMM",25,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_BLANKE","BLANKE",21,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_BLANKS","BLANKS",18,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_CURDISP","CURDISP",6,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_DESTCURR","DESTCURR",13,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_DESTDATA","DESTDATA",29,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_DESTLINE","DESTLINE",12,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_DESTPITCH","DESTPITCH",14,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_GBLKSIZE","GBLKSIZE",15,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_HWCRSR","HWCRSR",22,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_HLINTR","HLINTR",17,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_HWCRSRSTART","HWCRSRSTART",2,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_HWCRSRCURR","HWCRSRCURR",3,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_LEFTMASK","LEFTMASK",30,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_LINECOUNT","LINECOUNT",7,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_NXTDISP","NXTDISP",5,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_PSRCVB","PSRCVB",25,(F_20|F_CF|F_RO)},
	{(word *)mXc2,0,"C2_PSTXB","PSTXB",24,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_RIGHTMASK","RIGHTMASK",31,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SAMEXTENT","SAMEXTENT",4,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SCRSTART","SCRSTART",0,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SCRPITCH","SCRPITCH",1,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SERPULS","SERPULS",16,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SRCDATA","SRCDATA",28,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SRCCURR","SRCCURR",10,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SRCLINE","SRCLINE",9,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SRCPITCH","SRCPITCH",11,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SYNCS","SYNCS",19,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SYNCRESET","SYNCRESET",20,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_VBLKSIZE","VBLKSIZE",8,(F_20|F_CF)},
	{(word *)mXc2,c2vhwconfig,"C2_VHWCONFIG","VHWCONFIG",23,(F_20|F_CF)},
#else /* sorted by type */
	{(word *)cXc2,c2gcpcntrl,"C2_GCPCNTRL","GCPCNTRL",4,(F_20|F_CF)},
	{(word *)cXc2,0,"C2_SRCSHIFT","SRCSHIFT",1,(F_020|F_CF)},
	{(word *)cXc2,c2srcshift120,"C2_SRCSHIFT","SRCSHIFT",1,(F_120|F_CF)},
	{(word *)mXc2,0,"C2_SRCLINE","SRCLINE",9,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SRCCURR","SRCCURR",10,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SRCPITCH","SRCPITCH",11,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SRCDATA","SRCDATA",28,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_GBLKSIZE","GBLKSIZE",15,(F_20|F_CF)},
	{(word *)cXc2,0,"C2_SRCSKEW","SRCSKEW",0,(F_20|F_CF)},
	{(word *)cXc2,0,"C2_COLOR0","COLOR0",2,(F_20|F_CF)},
	{(word *)cXc2,0,"C2_COLOR1","COLOR1",3,(F_20|F_CF)},
	{(word *)cXc2,0,"C2_RASTEROP","RASTEROP",5,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_LEFTMASK","LEFTMASK",30,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_RIGHTMASK","RIGHTMASK",31,(F_20|F_CF)},
	{(word *)cXc2,0,"C2_PLANEMASK","PLANEMASK",6,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_DESTLINE","DESTLINE",12,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_DESTCURR","DESTCURR",13,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_DESTPITCH","DESTPITCH",14,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_DESTDATA","DESTDATA",29,(F_20|F_CF)},
	{(word *)mXc2,c2vhwconfig,"C2_VHWCONFIG","VHWCONFIG",23,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SCRSTART","SCRSTART",0,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SCRPITCH","SCRPITCH",1,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_CURDISP","CURDISP",6,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_NXTDISP","NXTDISP",5,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_LINECOUNT","LINECOUNT",7,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_VBLKSIZE","VBLKSIZE",8,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_BLANKE","BLANKE",21,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_BLANKS","BLANKS",18,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SYNCS","SYNCS",19,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SYNCRESET","SYNCRESET",20,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SERPULS","SERPULS",16,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_HLINTR","HLINTR",17,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_SAMEXTENT","SAMEXTENT",4,(F_20|F_CF)},
	{(word *)cXc2,c2config020,"C2_CONFIG","CONFIG",9,(F_020|F_CF)},
	{(word *)cXc2,c2config120,"C2_CONFIG","CONFIG",9,(F_120|F_CF)},
	{(word *)mXc2,0,"C2_HWCRSR","HWCRSR",22,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_HWCRSRSTART","HWCRSRSTART",2,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_HWCRSRCURR","HWCRSRCURR",3,(F_20|F_CF)},
	{(word *)cXc2,c2psstat,"C2_PSSTAT","PSSTAT",24,(F_20|F_CF|F_RO)},
	{(word *)cXc2,c2pscomm,"C2_PSCOMM","PSCOMM",25,(F_20|F_CF)},
	{(word *)mXc2,0,"C2_PSRCVB","PSRCVB",25,(F_20|F_CF|F_RO)},
	{(word *)mXc2,0,"C2_PSTXB","PSTXB",24,(F_20|F_CF)},
#endif
#endif
	{0}};

