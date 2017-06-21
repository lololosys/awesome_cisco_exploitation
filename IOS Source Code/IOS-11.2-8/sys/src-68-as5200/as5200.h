/* $Id: as5200.h,v 1.1.70.3 1996/09/02 10:33:13 styang Exp $
 * $Source: /release/112/cvs/Xsys/src-68-as5200/as5200.h,v $
 *------------------------------------------------------------------
 * as5200.h -- Common AS5200 defines.
 *
 * December 1995, Joe Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: as5200.h,v $
 * Revision 1.1.70.3  1996/09/02  10:33:13  styang
 * CSCdi61183:  Cannot tell hardware revision of cards from command line
 * Branch: California_branch
 * To show hardware version info for the modem card and T1/E1 card.
 *
 * Revision 1.1.70.2  1996/07/10  21:32:17  styang
 * CSCdi55120:  nm: cardTable objects need work
 * Branch: California_branch
 * 1.To provide the t1/e1 firmware version number in the chassis mib.
 * 2.'card descripion' fileds are modified to give beeter description.
 *
 * Revision 1.1.70.1  1996/06/16  21:19:25  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/03  21:25:10  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * General definitions
 */
#define BRASIL_MAX_SLOTS                3
#define AS5200_MAX_SLOTS                BRASIL_MAX_SLOTS
#define BRASIL_SLOT_OFFSET              0x2000
#define BRASIL_CARD_PRESENT_BIT         0x1
#define BRASIL_BD_REV_OFFSET		0x2
#define BRASIL_T1_BD_NR_BUS_PLD_REV_OFFSET 0xa

/*
 * Interrupt Level Enable bits
 */
#define AS5200_LEVEL4_EN  0x01  /* enable level 4 interrupt from T1 nim */
#define AS5200_LEVEL1_EN  0x02  /* enable level 1 interrupt from ASYNC TX */
#define AS5200_LEVEL2_EN  0x04  /* enable level 2 interrupt from ASYNC RX */
#define AS5200_LEVEL3_EN  0x08  /* enable level 3 interrupt from ASYNC MODEM */
#define AS5200_LEVEL6_EN  0x10  /* enable level 6 interrupt from T1 nim */

/*
 * Board ID definitions
 */
#define BRASIL_1E2T_MOTHERBOARD         0x01
#define BRASIL_1R2T_MOTHERBOARD         0x02
#define BRASIL_24_ASYNC                 0x03
#define BRASIL_24_ASYNC_PLUS_MODEMS     0x04
#define BRASIL_T1_DPRI_PLUS_CSU         0x05
#define BRASIL_E1_DPRI_PLUS_MODEMS      0x06
#define BRASIL_COMPRESSION              0x07

/*
 * Define addressing macros
 */
#define BRASIL_GET_SLOT_BASE(slot) \
        ((ulong) (ADRSPC_BRASIL_BASE_SLOT + (BRASIL_SLOT_OFFSET * (slot))))
#define GET_SLOT_BASE(slot) ((ushort *) BRASIL_GET_SLOT_BASE(slot))
  
/*
 * Useful Macros
 */
#define BRASIL_CARD_PRESENT(slot) \
	((~*(ushort *)ADRSPC_BRASIL_MB_CONFIG_REG) & \
	(BRASIL_CARD_PRESENT_BIT << (slot)))

#define BRASIL_BOARD_ID(slot) \
	(GETSHORT(BRASIL_GET_SLOT_BASE(slot)) & 0xff)

#define BRASIL_BOARD_REV(slot) \
        (GETSHORT(BRASIL_GET_SLOT_BASE(slot) + BRASIL_BD_REV_OFFSET) & 0xff)

#define BRASIL_T1_BOARD_NR_BUS_PLD_REV(slot) \
        (GETSHORT(BRASIL_GET_SLOT_BASE(slot) + \
        BRASIL_T1_BD_NR_BUS_PLD_REV_OFFSET) & 0xff)

/*
 * Nim types
 */
#define BRASIL_NIM_MCN          0x00
#define BRASIL_NIM_ASYNC        0x01
#define BRASIL_NIM_ASYNC_MODEM  0x02
#define BRASIL_NIM_COMPRESSION  0x03
#define BRASIL_NIM_UNKNOWN      0xff

/*
 * Prototypes
 */
extern void nim_add_chassis_mib_cards(void);
extern void nim_subsys_init_class(ushort class);
extern ushort nim_get_type(int slot);
extern ushort nim_get_version(int slot);
extern boolean nim_exists(int slot);
    
extern short t1_version;
extern short e1_version;
