/* $Id: cam.h,v 3.2.60.1 1996/03/18 20:40:25 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/les/cam.h,v $
 *------------------------------------------------------------------
 * music defines
 *
 * oct 92, southam, smackie, eli
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Longer description here.  m u s i c  d e f i n e s
 *------------------------------------------------------------------
 * $Log: cam.h,v $
 * Revision 3.2.60.1  1996/03/18  20:40:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.1  1996/01/24  22:22:10  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  17:36:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:30:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ---------------------------------------------------------------------
 * This module is the header file for the MUSIC semiconductors
 * MU9C1480 CAM for the DOSXX family of cisco routers/bridges.
 * ---------------------------------------------------------------------
 */

#define CAM_COMMAND(instruction)        rp->ccf = instruction;
#define CAM_COMMAND_DIRECT(instruction) rp->dcr = instruction;
#define CAM_DATA_WR(data)               rp->cdf = data;
#define CAM_DATA_WR_DIRECT(data)        rp->ddr = data;
#define CAM_DATA_RD_DIRECT              rp->ddr;
/*********************************************************************/
/* COMMANDS VIA FIFO */

#define SPS_CR            CAM_COMMAND(0x0000)
#define SPS_MR1           CAM_COMMAND(0x0001)
#define SPS_MR2           CAM_COMMAND(0x0002)
#define SPS_M_AR          CAM_COMMAND(0x0004)
#define SPS_M(addr)       CAM_COMMAND(0x0804) CAM_COMMAND(addr)
#define SPS_M_HM          CAM_COMMAND(0x0005)

#define SPD_CR            CAM_COMMAND(0x0100)
#define SPD_CR_MR1        CAM_COMMAND(0x0140)
#define SPD_CR_MR2        CAM_COMMAND(0x0180)
#define SPD_MR1           CAM_COMMAND(0x0108)
#define SPD_MR2           CAM_COMMAND(0x0110)
#define SPD_M_AR_V        CAM_COMMAND(0x0124)
#define SPD_M_AR_MR1_V    CAM_COMMAND(0x0164)
#define SPD_M_AR_MR2_V    CAM_COMMAND(0x01a4)
#define SPD_M_AR_E        CAM_COMMAND(0x0125)
#define SPD_M_AR_MR1_E    CAM_COMMAND(0x0165)
#define SPD_M_AR_MR2_E    CAM_COMMAND(0x01a5)
#define SPD_M_AR_S        CAM_COMMAND(0x0126)
#define SPD_M_AR_MR1_S    CAM_COMMAND(0x0166)
#define SPD_M_AR_MR2_S    CAM_COMMAND(0x01a6)
#define SPD_M_AR_R        CAM_COMMAND(0x0127)
#define SPD_M_AR_MR1_R    CAM_COMMAND(0x0167)
#define SPD_M_AR_MR2_R    CAM_COMMAND(0x01a7)
#define SPD_M_V(addr)     CAM_COMMAND(0x0924) CAM_COMMAND(addr)
#define SPD_M_MR1_V(addr) CAM_COMMAND(0x0964) CAM_COMMAND(addr)
#define SPD_M_MR2_V(addr) CAM_COMMAND(0x09a4) CAM_COMMAND(addr)
#define SPD_M_E(addr)     CAM_COMMAND(0x0925) CAM_COMMAND(addr)
#define SPD_M_MR1_E(addr) CAM_COMMAND(0x0965) CAM_COMMAND(addr)
#define SPD_M_MR2_E(addr) CAM_COMMAND(0x09a5) CAM_COMMAND(addr)
#define SPD_M_S(addr)     CAM_COMMAND(0x0926) CAM_COMMAND(addr)
#define SPD_M_MR1_S(addr) CAM_COMMAND(0x0966) CAM_COMMAND(addr)
#define SPD_M_MR2_S(addr) CAM_COMMAND(0x09a6) CAM_COMMAND(addr)
#define SPD_M_R(addr)     CAM_COMMAND(0x0927) CAM_COMMAND(addr)
#define SPD_M_MR1_R(addr) CAM_COMMAND(0x0967) CAM_COMMAND(addr)
#define SPD_M_MR2_R(addr) CAM_COMMAND(0x09a7) CAM_COMMAND(addr)
#define SPD_M_HM_V        CAM_COMMAND(0x012c)
#define SPD_M_HM_MR1_V    CAM_COMMAND(0x016c)
#define SPD_M_HM_MR2_V    CAM_COMMAND(0x01ac)
#define SPD_M_HM_E        CAM_COMMAND(0x012d)
#define SPD_M_HM_MR1_E    CAM_COMMAND(0x016d)
#define SPD_M_HM_MR2_E    CAM_COMMAND(0x01ad)
#define SPD_M_HM_S        CAM_COMMAND(0x012e)
#define SPD_M_HM_MR1_S    CAM_COMMAND(0x016e)
#define SPD_M_HM_MR2_S    CAM_COMMAND(0x01ae)
#define SPD_M_HM_R        CAM_COMMAND(0x012f)
#define SPD_M_HM_MR1_R    CAM_COMMAND(0x016f)
#define SPD_M_HM_MR2_R    CAM_COMMAND(0x01af)
#define SPD_M_NF_V        CAM_COMMAND(0x0134)
#define SPD_M_NF_MR1_V    CAM_COMMAND(0x0174)
#define SPD_M_NF_MR2_V    CAM_COMMAND(0x01b4)
#define SPD_M_NF_E        CAM_COMMAND(0x0135)
#define SPD_M_NF_MR1_E    CAM_COMMAND(0x0175)
#define SPD_M_NF_MR2_E    CAM_COMMAND(0x01b5)
#define SPD_M_NF_S        CAM_COMMAND(0x0136)
#define SPD_M_NF_MR1_S    CAM_COMMAND(0x0176)
#define SPD_M_NF_MR2_S    CAM_COMMAND(0x01b6)
#define SPD_M_NF_R        CAM_COMMAND(0x0137)
#define SPD_M_NF_MR1_R    CAM_COMMAND(0x0177)
#define SPD_M_NF_MR2_R    CAM_COMMAND(0x01b7)

#define TCO_CT            CAM_COMMAND(0x0200)
#define TCO_PA            CAM_COMMAND(0x0208)
#define TCO_SC            CAM_COMMAND(0x0210)
#define TCO_NF            CAM_COMMAND(0x0218)
#define TCO_AR            CAM_COMMAND(0x0220)
#define TCO_DS            CAM_COMMAND(0x0228)
#define TCO_PS            CAM_COMMAND(0x0230)
#define TCO_PD            CAM_COMMAND(0x0238)

#define MOV_CR_MR1        CAM_COMMAND(0x0301)
#define MOV_CR_MR2        CAM_COMMAND(0x0302)
#define MOV_CR_AR         CAM_COMMAND(0x0304)
#define MOV_CR_AR_mr1     CAM_COMMAND(0x0344)
#define MOV_CR_AR_mr2     CAM_COMMAND(0x0384)
#define MOV_CR(addr)      CAM_COMMAND(0x0b04) CAM_COMMAND(addr)
#define MOV_CR_mr1(addr)  CAM_COMMAND(0x0b44) CAM_COMMAND(addr)
#define MOV_CR_mr2(addr)  CAM_COMMAND(0x0b84) CAM_COMMAND(addr)
#define MOV_CR_HM         CAM_COMMAND(0x0305)
#define MOV_CR_HM_mr1     CAM_COMMAND(0x0345)
#define MOV_CR_HM_mr2     CAM_COMMAND(0x0385)

#define MOV_MR1_CR        CAM_COMMAND(0x0308) 
#define MOV_MR1_MR2       CAM_COMMAND(0x030a)
#define MOV_MR1_AR        CAM_COMMAND(0x030c)
#define MOV_MR1(addr)     CAM_COMMAND(0x0boc) CAM_COMMAND(addr)
#define MOV_MR1_HP        CAM_COMMAND(0x030d)

#define MOV_MR2_CR        CAM_COMMAND(0x0310) 
#define MOV_MR2_MR1       CAM_COMMAND(0x0311)
#define MOV_MR2_AR        CAM_COMMAND(0x0314)
#define MOV_MR2(addr)     CAM_COMMAND(0x0b14) CAM_COMMAND(addr)
#define MOV_MR2_HP        CAM_COMMAND(0x0315)

#define MOV_M_AR_CR       CAM_COMMAND(0x0320)
#define MOV_M_AR_CR_mr1   CAM_COMMAND(0x0360)
#define MOV_M_AR_CR_mr2   CAM_COMMAND(0x03a0)
#define MOV_M_AR_MR1      CAM_COMMAND(0x0321)
#define MOV_M_AR_MR2      CAM_COMMAND(0x0322)

#define MOV_M_AR_CR_V     CAM_COMMAND(0x0324)
#define MOV_M_AR_CR_mr1_V CAM_COMMAND(0x0364)
#define MOV_M_AR_CR_mr2_V CAM_COMMAND(0x03a4)
#define MOV_M_AR_MR1_V    CAM_COMMAND(0x0325)
#define MOV_M_AR_MR2_V    CAM_COMMAND(0x0326)

#define MOV_M_CR(addr)     CAM_COMMAND(0x0b20)
#define MOV_M_CR_mr1(addr) CAM_COMMAND(0x0b60) CAM_COMMAND(addr)
#define MOV_M_CR_mr2(addr) CAM_COMMAND(0x0ba0) CAM_COMMAND(addr)
#define MOV_M_MR1(addr)    CAM_COMMAND(0x0b21) CAM_COMMAND(addr)
#define MOV_M_MR2(addr)    CAM_COMMAND(0x0b22) CAM_COMMAND(addr)

#define MOV_M_CR_V(addr)     CAM_COMMAND(0x0b24) CAM_COMMAND(addr)
#define MOV_M_CR_mr1_V(addr) CAM_COMMAND(0x0b64) CAM_COMMAND(addr)
#define MOV_M_CR_mr2_V(addr) CAM_COMMAND(0x0ba4) CAM_COMMAND(addr)
#define MOV_M_MR1_V(addr)    CAM_COMMAND(0x0b25) CAM_COMMAND(addr)
#define MOV_M_MR2_V(addr)    CAM_COMMAND(0x0b26) CAM_COMMAND(addr)

#define MOV_M_HM_CR        CAM_COMMAND(0x0328)
#define MOV_M_HM_CR_mr1    CAM_COMMAND(0x0368)
#define MOV_M_HM_CR_mr2    CAM_COMMAND(0x03a8)
#define MOV_M_HM_CR_MR1    CAM_COMMAND(0x0329)
#define MOV_M_HM_CR_MR2    CAM_COMMAND(0x032a)

#define MOV_M_HM_CR_V      CAM_COMMAND(0x032c)
#define MOV_M_HM_CR_mr1_V  CAM_COMMAND(0x036c)
#define MOV_M_HM_CR_mr2_V  CAM_COMMAND(0x03ac)
#define MOV_M_HM_CR_MR1_V  CAM_COMMAND(0x032d)
#define MOV_M_HM_CR_MR2_V  CAM_COMMAND(0x032e)

#define MOV_M_NF_CR        CAM_COMMAND(0x0330)
#define MOV_M_NF_CR_mr1    CAM_COMMAND(0x0370)
#define MOV_M_NF_CR_mr2    CAM_COMMAND(0x03b0)
#define MOV_M_NF_CR_MR1    CAM_COMMAND(0x0331)
#define MOV_M_NF_CR_MR2    CAM_COMMAND(0x0332)

#define MOV_M_NF_CR_V      CAM_COMMAND(0x0334)
#define MOV_M_NF_CR_mr1_V  CAM_COMMAND(0x0374)
#define MOV_M_NF_CR_mr2_V  CAM_COMMAND(0x03b4)
#define MOV_M_NF_CR_MR1_V  CAM_COMMAND(0x0335)
#define MOV_M_NF_CR_MR2_V  CAM_COMMAND(0x0336)

#define VBC_M_AR_V         CAM_COMMAND(0x0424)
#define VBC_M_AR_E         CAM_COMMAND(0x0425)
#define VBC_M_AR_S         CAM_COMMAND(0x0426)
#define VBC_M_AR_R         CAM_COMMAND(0x0427)

#define VBC_M_V(addr)      CAM_COMMAND(0x0c24) CAM_COMMAND(addr)
#define VBC_M_E(addr)      CAM_COMMAND(0x0c25) CAM_COMMAND(addr)
#define VBC_M_S(addr)      CAM_COMMAND(0x0c26) CAM_COMMAND(addr)
#define VBC_M_R(addr)      CAM_COMMAND(0x0c27) CAM_COMMAND(addr)

#define VBC_M_HM_V         CAM_COMMAND(0x042c)
#define VBC_M_HM_E         CAM_COMMAND(0x042d)
#define VBC_M_HM_S         CAM_COMMAND(0x042e)
#define VBC_M_HM_R         CAM_COMMAND(0x042f)

#define VBC_M_ALM_V        CAM_COMMAND(0x043c)
#define VBC_M_ALM_E        CAM_COMMAND(0x043d)
#define VBC_M_ALM_S        CAM_COMMAND(0x043e)
#define VBC_M_ALM_R        CAM_COMMAND(0x043f)

#define CMP_V              CAM_COMMAND(0x0504)
#define CMP_E              CAM_COMMAND(0x0505)
#define CMP_S              CAM_COMMAND(0x0506)
#define CMP_R              CAM_COMMAND(0x0507)

#define SFF                CAM_COMMAND(0x700)
#define CAM_NOP            CAM_COMMAND(0x300)
/*********************************************************************/
/* DIRECT COMMANDS */

#define SPS_CR_DIRECT            CAM_COMMAND_DIRECT(0x0000)
#define SPS_MR1_DIRECT           CAM_COMMAND_DIRECT(0x0001)
#define SPS_MR2_DIRECT           CAM_COMMAND_DIRECT(0x0002)
#define SPS_M_AR_DIRECT          CAM_COMMAND_DIRECT(0x0004)
#define SPS_M_DIRECT(addr)       CAM_COMMAND_DIRECT(0x0804) CAM_COMMAND_DIRECT(addr)
#define SPS_M_HM_DIRECT          CAM_COMMAND_DIRECT(0x0005)

#define SPD_CR_DIRECT            CAM_COMMAND_DIRECT(0x0100)
#define SPD_CR_MR1_DIRECT        CAM_COMMAND_DIRECT(0x0140)
#define SPD_CR_MR2_DIRECT        CAM_COMMAND_DIRECT(0x0180)
#define SPD_MR1_DIRECT           CAM_COMMAND_DIRECT(0x0108)
#define SPD_MR2_DIRECT           CAM_COMMAND_DIRECT(0x0110)
#define SPD_M_AR_V_DIRECT        CAM_COMMAND_DIRECT(0x0124)
#define SPD_M_AR_MR1_V_DIRECT    CAM_COMMAND_DIRECT(0x0164)
#define SPD_M_AR_MR2_V_DIRECT    CAM_COMMAND_DIRECT(0x01a4)
#define SPD_M_AR_E_DIRECT        CAM_COMMAND_DIRECT(0x0125)
#define SPD_M_AR_MR1_E_DIRECT    CAM_COMMAND_DIRECT(0x0165)
#define SPD_M_AR_MR2_E_DIRECT    CAM_COMMAND_DIRECT(0x01a5)
#define SPD_M_AR_S_DIRECT        CAM_COMMAND_DIRECT(0x0126)
#define SPD_M_AR_MR1_S_DIRECT    CAM_COMMAND_DIRECT(0x0166)
#define SPD_M_AR_MR2_S_DIRECT    CAM_COMMAND_DIRECT(0x01a6)
#define SPD_M_AR_R_DIRECT        CAM_COMMAND_DIRECT(0x0127)
#define SPD_M_AR_MR1_R_DIRECT    CAM_COMMAND_DIRECT(0x0167)
#define SPD_M_AR_MR2_R_DIRECT    CAM_COMMAND_DIRECT(0x01a7)
#define SPD_M_V_DIRECT(addr)     CAM_COMMAND_DIRECT(0x0924) CAM_COMMAND_DIRECT(addr)
#define SPD_M_MR1_V_DIRECT(addr) CAM_COMMAND_DIRECT(0x0964) CAM_COMMAND_DIRECT(addr)
#define SPD_M_MR2_V_DIRECT(addr) CAM_COMMAND_DIRECT(0x09a4) CAM_COMMAND_DIRECT(addr)
#define SPD_M_E_DIRECT(addr)     CAM_COMMAND_DIRECT(0x0925) CAM_COMMAND_DIRECT(addr)
#define SPD_M_MR1_E_DIRECT(addr) CAM_COMMAND_DIRECT(0x0965) CAM_COMMAND_DIRECT(addr)
#define SPD_M_MR2_E_DIRECT(addr) CAM_COMMAND_DIRECT(0x09a5) CAM_COMMAND_DIRECT(addr)
#define SPD_M_S_DIRECT(addr)     CAM_COMMAND_DIRECT(0x0926) CAM_COMMAND_DIRECT(addr)
#define SPD_M_MR1_S_DIRECT(addr) CAM_COMMAND_DIRECT(0x0966) CAM_COMMAND_DIRECT(addr)
#define SPD_M_MR2_S_DIRECT(addr) CAM_COMMAND_DIRECT(0x09a6) CAM_COMMAND_DIRECT(addr)
#define SPD_M_R_DIRECT(addr)     CAM_COMMAND_DIRECT(0x0927) CAM_COMMAND_DIRECT(addr)
#define SPD_M_MR1_R_DIRECT(addr) CAM_COMMAND_DIRECT(0x0967) CAM_COMMAND_DIRECT(addr)
#define SPD_M_MR2_R_DIRECT(addr) CAM_COMMAND_DIRECT(0x09a7) CAM_COMMAND_DIRECT(addr)
#define SPD_M_HM_V_DIRECT        CAM_COMMAND_DIRECT(0x012c)
#define SPD_M_HM_MR1_V_DIRECT    CAM_COMMAND_DIRECT(0x016c)
#define SPD_M_HM_MR2_V_DIRECT    CAM_COMMAND_DIRECT(0x01ac)
#define SPD_M_HM_E_DIRECT        CAM_COMMAND_DIRECT(0x012d)
#define SPD_M_HM_MR1_E_DIRECT    CAM_COMMAND_DIRECT(0x016d)
#define SPD_M_HM_MR2_E_DIRECT    CAM_COMMAND_DIRECT(0x01ad)
#define SPD_M_HM_S_DIRECT        CAM_COMMAND_DIRECT(0x012e)
#define SPD_M_HM_MR1_S_DIRECT    CAM_COMMAND_DIRECT(0x016e)
#define SPD_M_HM_MR2_S_DIRECT    CAM_COMMAND_DIRECT(0x01ae)
#define SPD_M_HM_R_DIRECT        CAM_COMMAND_DIRECT(0x012f)
#define SPD_M_HM_MR1_R_DIRECT    CAM_COMMAND_DIRECT(0x016f)
#define SPD_M_HM_MR2_R_DIRECT    CAM_COMMAND_DIRECT(0x01af)
#define SPD_M_NF_V_DIRECT        CAM_COMMAND_DIRECT(0x0134)
#define SPD_M_NF_MR1_V_DIRECT    CAM_COMMAND_DIRECT(0x0174)
#define SPD_M_NF_MR2_V_DIRECT    CAM_COMMAND_DIRECT(0x01b4)
#define SPD_M_NF_E_DIRECT        CAM_COMMAND_DIRECT(0x0135)
#define SPD_M_NF_MR1_E_DIRECT    CAM_COMMAND_DIRECT(0x0175)
#define SPD_M_NF_MR2_E_DIRECT    CAM_COMMAND_DIRECT(0x01b5)
#define SPD_M_NF_S_DIRECT        CAM_COMMAND_DIRECT(0x0136)
#define SPD_M_NF_MR1_S_DIRECT    CAM_COMMAND_DIRECT(0x0176)
#define SPD_M_NF_MR2_S_DIRECT    CAM_COMMAND_DIRECT(0x01b6)
#define SPD_M_NF_R_DIRECT        CAM_COMMAND_DIRECT(0x0137)
#define SPD_M_NF_MR1_R_DIRECT    CAM_COMMAND_DIRECT(0x0177)
#define SPD_M_NF_MR2_R_DIRECT    CAM_COMMAND_DIRECT(0x01b7)

#define TCO_CT_DIRECT            CAM_COMMAND_DIRECT(0x0200)
#define TCO_PA_DIRECT            CAM_COMMAND_DIRECT(0x0208)
#define TCO_SC_DIRECT            CAM_COMMAND_DIRECT(0x0210)
#define TCO_NF_DIRECT            CAM_COMMAND_DIRECT(0x0218)
#define TCO_AR_DIRECT            CAM_COMMAND_DIRECT(0x0220)
#define TCO_DS_DIRECT            CAM_COMMAND_DIRECT(0x0228)
#define TCO_PS_DIRECT            CAM_COMMAND_DIRECT(0x0230)
#define TCO_PD_DIRECT            CAM_COMMAND_DIRECT(0x0238)

#define MOV_CR_MR1_DIRECT        CAM_COMMAND_DIRECT(0x0301)
#define MOV_CR_MR2_DIRECT        CAM_COMMAND_DIRECT(0x0302)
#define MOV_CR_AR_DIRECT         CAM_COMMAND_DIRECT(0x0304)
#define MOV_CR_AR_mr1_DIRECT     CAM_COMMAND_DIRECT(0x0344)
#define MOV_CR_AR_mr2_DIRECT     CAM_COMMAND_DIRECT(0x0384)
#define MOV_CR_DIRECT(addr)      CAM_COMMAND_DIRECT(0x0b04) CAM_COMMAND_DIRECT(addr)
#define MOV_CR_mr1_DIRECT(addr)  CAM_COMMAND_DIRECT(0x0b44) CAM_COMMAND_DIRECT(addr)
#define MOV_CR_mr2_DIRECT(addr)  CAM_COMMAND_DIRECT(0x0b84) CAM_COMMAND_DIRECT(addr)
#define MOV_CR_HM_DIRECT         CAM_COMMAND_DIRECT(0x0305)
#define MOV_CR_HM_mr1_DIRECT     CAM_COMMAND_DIRECT(0x0345)
#define MOV_CR_HM_mr2_DIRECT     CAM_COMMAND_DIRECT(0x0385)

#define MOV_MR1_CR_DIRECT        CAM_COMMAND_DIRECT(0x0308) 
#define MOV_MR1_MR2_DIRECT       CAM_COMMAND_DIRECT(0x030a)
#define MOV_MR1_AR_DIRECT        CAM_COMMAND_DIRECT(0x030c)
#define MOV_MR1_DIRECT(addr)     CAM_COMMAND_DIRECT(0x0boc) CAM_COMMAND_DIRECT(addr)
#define MOV_MR1_HP_DIRECT        CAM_COMMAND_DIRECT(0x030d)

#define MOV_MR2_CR_DIRECT        CAM_COMMAND_DIRECT(0x0310) 
#define MOV_MR2_MR1_DIRECT       CAM_COMMAND_DIRECT(0x0311)
#define MOV_MR2_AR_DIRECT        CAM_COMMAND_DIRECT(0x0314)
#define MOV_MR2_DIRECT(addr)     CAM_COMMAND_DIRECT(0x0b14) CAM_COMMAND_DIRECT(addr)
#define MOV_MR2_HP_DIRECT        CAM_COMMAND_DIRECT(0x0315)

#define MOV_M_AR_CR_DIRECT       CAM_COMMAND_DIRECT(0x0320)
#define MOV_M_AR_CR_mr1_DIRECT   CAM_COMMAND_DIRECT(0x0360)
#define MOV_M_AR_CR_mr2_DIRECT   CAM_COMMAND_DIRECT(0x03a0)
#define MOV_M_AR_MR1_DIRECT      CAM_COMMAND_DIRECT(0x0321)
#define MOV_M_AR_MR2_DIRECT      CAM_COMMAND_DIRECT(0x0322)

#define MOV_M_AR_CR_V_DIRECT     CAM_COMMAND_DIRECT(0x0324)
#define MOV_M_AR_CR_mr1_V_DIRECT CAM_COMMAND_DIRECT(0x0364)
#define MOV_M_AR_CR_mr2_V_DIRECT CAM_COMMAND_DIRECT(0x03a4)
#define MOV_M_AR_MR1_V_DIRECT    CAM_COMMAND_DIRECT(0x0325)
#define MOV_M_AR_MR2_V_DIRECT    CAM_COMMAND_DIRECT(0x0326)

#define MOV_M_CR_DIRECT(addr)     CAM_COMMAND_DIRECT(0x0b20)
#define MOV_M_CR_mr1_DIRECT(addr) CAM_COMMAND_DIRECT(0x0b60) CAM_COMMAND_DIRECT(addr)
#define MOV_M_CR_mr2_DIRECT(addr) CAM_COMMAND_DIRECT(0x0ba0) CAM_COMMAND_DIRECT(addr)
#define MOV_M_MR1_DIRECT(addr)    CAM_COMMAND_DIRECT(0x0b21) CAM_COMMAND_DIRECT(addr)
#define MOV_M_MR2_DIRECT(addr)    CAM_COMMAND_DIRECT(0x0b22) CAM_COMMAND_DIRECT(addr)

#define MOV_M_CR_V_DIRECT(addr)     CAM_COMMAND_DIRECT(0x0b24) CAM_COMMAND_DIRECT(addr)
#define MOV_M_CR_mr1_V_DIRECT(addr) CAM_COMMAND_DIRECT(0x0b64) CAM_COMMAND_DIRECT(addr)
#define MOV_M_CR_mr2_V_DIRECT(addr) CAM_COMMAND_DIRECT(0x0ba4) CAM_COMMAND_DIRECT(addr)
#define MOV_M_MR1_V_DIRECT(addr)    CAM_COMMAND_DIRECT(0x0b25) CAM_COMMAND_DIRECT(addr)
#define MOV_M_MR2_V_DIRECT(addr)    CAM_COMMAND_DIRECT(0x0b26) CAM_COMMAND_DIRECT(addr)

#define MOV_M_HM_CR_DIRECT        CAM_COMMAND_DIRECT(0x0328)
#define MOV_M_HM_CR_mr1_DIRECT    CAM_COMMAND_DIRECT(0x0368)
#define MOV_M_HM_CR_mr2_DIRECT    CAM_COMMAND_DIRECT(0x03a8)
#define MOV_M_HM_CR_MR1_DIRECT    CAM_COMMAND_DIRECT(0x0329)
#define MOV_M_HM_CR_MR2_DIRECT    CAM_COMMAND_DIRECT(0x032a)

#define MOV_M_HM_CR_V_DIRECT      CAM_COMMAND_DIRECT(0x032c)
#define MOV_M_HM_CR_mr1_V_DIRECT  CAM_COMMAND_DIRECT(0x036c)
#define MOV_M_HM_CR_mr2_V_DIRECT  CAM_COMMAND_DIRECT(0x03ac)
#define MOV_M_HM_CR_MR1_V_DIRECT  CAM_COMMAND_DIRECT(0x032d)
#define MOV_M_HM_CR_MR2_V_DIRECT  CAM_COMMAND_DIRECT(0x032e)

#define MOV_M_NF_CR_DIRECT        CAM_COMMAND_DIRECT(0x0330)
#define MOV_M_NF_CR_mr1_DIRECT    CAM_COMMAND_DIRECT(0x0370)
#define MOV_M_NF_CR_mr2_DIRECT    CAM_COMMAND_DIRECT(0x03b0)
#define MOV_M__NFCR_MR1_DIRECT    CAM_COMMAND_DIRECT(0x0331)
#define MOV_M_NF_CR_MR2_DIRECT    CAM_COMMAND_DIRECT(0x0332)

#define MOV_M_NF_CR_V_DIRECT      CAM_COMMAND_DIRECT(0x0334)
#define MOV_M_NF_CR_mr1_V_DIRECT  CAM_COMMAND_DIRECT(0x0374)
#define MOV_M_NF_CR_mr2_V_DIRECT  CAM_COMMAND_DIRECT(0x03b4)
#define MOV_M_NF_CR_MR1_V_DIRECT  CAM_COMMAND_DIRECT(0x0335)
#define MOV_M_NF_CR_MR2_V_DIRECT  CAM_COMMAND_DIRECT(0x0336)

#define VBC_M_AR_V_DIRECT         CAM_COMMAND_DIRECT(0x0424)
#define VBC_M_AR_E_DIRECT         CAM_COMMAND_DIRECT(0x0425)
#define VBC_M_AR_S_DIRECT         CAM_COMMAND_DIRECT(0x0426)
#define VBC_M_AR_R_DIRECT         CAM_COMMAND_DIRECT(0x0427)

#define VBC_M_V_DIRECT(addr)      CAM_COMMAND_DIRECT(0x0c24) CAM_COMMAND_DIRECT(addr)
#define VBC_M_E_DIRECT(addr)      CAM_COMMAND_DIRECT(0x0c25) CAM_COMMAND_DIRECT(addr)
#define VBC_M_S_DIRECT(addr)      CAM_COMMAND_DIRECT(0x0c26) CAM_COMMAND_DIRECT(addr)
#define VBC_M_R_DIRECT(addr)      CAM_COMMAND_DIRECT(0x0c27) CAM_COMMAND_DIRECT(addr)

#define VBC_M_HM_V_DIRECT         CAM_COMMAND_DIRECT(0x042c)
#define VBC_M_HM_E_DIRECT         CAM_COMMAND_DIRECT(0x042d)
#define VBC_M_HM_S_DIRECT         CAM_COMMAND_DIRECT(0x042e)
#define VBC_M_HM_R_DIRECT         CAM_COMMAND_DIRECT(0x042f)

#define VBC_M_ALM_V_DIRECT        CAM_COMMAND_DIRECT(0x043c)
#define VBC_M_ALM_E_DIRECT        CAM_COMMAND_DIRECT(0x043d)
#define VBC_M_ALM_S_DIRECT        CAM_COMMAND_DIRECT(0x043e)
#define VBC_M_ALM_R_DIRECT        CAM_COMMAND_DIRECT(0x043f)

#define CMP_V_DIRECT              CAM_COMMAND_DIRECT(0x0504)
#define CMP_E_DIRECT              CAM_COMMAND_DIRECT(0x0505)
#define CMP_S_DIRECT              CAM_COMMAND_DIRECT(0x0506)
#define CMP_R_DIRECT              CAM_COMMAND_DIRECT(0x0507)

#define SFF_DIRECT                CAM_COMMAND_DIRECT(0x700)
#define CAM_NOP_DIRECT            CAM_COMMAND_DIRECT(0x300)
/*********************************************************************/
/* Control Register bit assignments */
#define CAM_RESET                   0x7ffc

#define CAM_ENABLE_MATCH_FLAG       0x9ffc
#define CAM_DISABLE_MATCH_FLAG      0xbffc

#define CAM_ENABLE_FULL_FLAG        0xe7fc
#define CAM_DISABLE_FULL_FLAG       0xeffc

#define CAM_INPUT_NOT_SCRAMBLED     0xf9fc
#define CAM_INPUT_SCRAMBLED         0xfbfc

#define CAM_64_RAM_0                0xfe3c
#define CAM_48_RAM_16               0xfe7c
#define CAM_32_RAM_32               0xfebc
#define CAM_16_RAM_48               0xfefc
#define CAM_0_RAM_64                0xff3c

#define CAM_COMPARE_MASK_DISABLE    0xffcc
#define CAM_COMPARE_MASK_REG1       0xffdc
#define CAM_COMPARE_MASK_REG2       0xffec

#define CAM_ENABLE_ADDRESS_INC      0xfff0
#define CAM_ENABLE_ADDRESS_DEC      0xfff4
#define CAM_DISABLE_ADDRESS_INC_DEC 0xfff8

/* Segment Register bit assignments */
#define CAM_DST_CNTR_START00_END00 0x07ff
#define CAM_DST_CNTR_START00_END01 0x0fff
#define CAM_DST_CNTR_START00_END10 0x17ff
#define CAM_DST_CNTR_START00_END11 0x1fff
#define CAM_DST_CNTR_START01_END01 0x2fff
#define CAM_DST_CNTR_START01_END10 0x37ff
#define CAM_DST_CNTR_START01_END11 0x3fff
#define CAM_DST_CNTR_START10_END10 0x57ff
#define CAM_DST_CNTR_START10_END11 0x5fff
#define CAM_DST_CNTR_START11_END11 0x7fff

#define CAM_SRC_CNTR_START00_END00 0xf83f
#define CAM_SRC_CNTR_START00_END01 0xf87f
#define CAM_SRC_CNTR_START00_END10 0xf8bf
#define CAM_SRC_CNTR_START00_END11 0xf8ff
#define CAM_SRC_CNTR_START01_END01 0xf97f
#define CAM_SRC_CNTR_START01_END10 0xf9bf
#define CAM_SRC_CNTR_START01_END11 0xf9ff
#define CAM_SRC_CNTR_START10_END10 0xfabf
#define CAM_SRC_CNTR_START10_END11 0xfaff
#define CAM_SRC_CNTR_START11_END11 0xfbff

#define CAM_LOAD_DST_SEG_CNTR_00   0xffc7
#define CAM_LOAD_DST_SEG_CNTR_01   0xffcf
#define CAM_LOAD_DST_SEG_CNTR_10   0xffd7
#define CAM_LOAD_DST_SEG_CNTR_11   0xffdf

#define CAM_LOAD_SRC_SEG_CNTR_00   0xfff8
#define CAM_LOAD_SRC_SEG_CNTR_01   0xfff9
#define CAM_LOAD_SRC_SEG_CNTR_10   0xfffa
#define CAM_LOAD_SRC_SEG_CNTR_11   0xfffb

/*
 * Status Register Bit Definitions
 */
#define CAM_STATUS_FULL_FLAG           0x8000
#define CAM_STATUS_MULTIPLE_MATCH_FLAG 0x4000
#define CAM_STATUS_MATCH_FLAG          0x0001

typedef struct cam_entry cam_entry_t;
struct cam_entry {
    ushort ram;
    ushort cam_seg1;
    ushort cam_seg2;
    ushort cam_seg3;
};

/*
 * Prototypes
 */

extern void show_control_cam(parseinfo *csb);
extern void config_cam(xx_fddi_regs_t *rp);
extern void cam_enable_promiscuous_multicast(xx_fddi_regs_t *rp);
extern void cam_disable_promiscuous_multicast(xx_fddi_regs_t *rp);
extern boolean config_xilinx(xx_fddi_regs_t *rp);
