/* $Id: pmd_eeprom.c,v 1.1.4.1 1996/05/09 14:55:07 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/pmd_eeprom.c,v $
 *------------------------------------------------------------------
 * pmd_eeprom.c
 *
 * January 1995, rbau
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pmd_eeprom.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:07  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:34  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:22  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:41  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:34  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:02  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.2  1995/09/30  00:15:05  rbau
 * added API to read PMD interface mode(i.e. fiber, UTP.)
 *
 * Revision 1.1  1995/09/14  02:02:54  rbau
 * export PMD serial EEPROM outside the off-line diag's domain
 *
 * Revision 1.5  1995/08/29  20:09:45  rbau
 * modified set_pmd command to be compatible with NMP's CLI.
 *
 * Revision 1.4  1995/08/07  18:48:11  rbau
 * added support for PMD name
 *
 * Revision 1.3  1995/08/01  20:37:14  rbau
 * added support for vendor feature in set/show pmd command.
 *
 * Revision 1.2  1995/05/31  16:51:01  rbau
 * added support functions for show/set pmd cli
 *
 * Revision 1.1  1995/02/17  23:41:51  rbau
 * initial check-in
 *
 * Revision 1.1  1995/01/27  00:57:24  rbau
 * Initial revision
 *
 * 01/25/95  rbau
 * - created using catalyst versions of eeprom.c/h as a base.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*******************
 *  Include files  *
 *******************/
#include "types.h"
#include "pmd_eeprom.h"

/*******************************************************************
	Global Data 
*******************************************************************/

/* shadow image for serial EEPROM */

char  ctrl_reg10_img;
char  ctrl_reg11_img;
char  ctrl_reg12_img;
char  ctrl_reg13_img;
struct   pmd_data prom;    /* pmd serial eeprom shadow */

#define  STATUS_REG        0x38000000

#define  CNTL_REG_0  0x38000000
#define  CNTL_REG_10 (CNTL_REG_0 + 0x10)
#define  CNTL_REG_11 (CNTL_REG_0 + 0x11)
#define  CNTL_REG_12 (CNTL_REG_0 + 0x12)
#define  CNTL_REG_13 (CNTL_REG_0 + 0x13)
#define  CNTL_REG_14 (CNTL_REG_0 + 0x14)

#define  SERIAL_WR_PROTECT_MASK  0x01
#define  SERIAL_PRGM_ENABLE_MASK 0x01
#define  SERIAL_EEPROM_DI_MASK   0x01
#define  SERIAL_EEPROM_CS_MASK   0x01
#define  SERIAL_EEPROM_SK_MASK   0x01
#define  SERIAL_EEPROM_DO_MASK   0x80


/**********
 * MACROS *
 **********/
#define TOGGLE_CLOCK    ctrl_reg12_img |= SERIAL_EEPROM_SK_MASK; \
                        *(char *)CNTL_REG_12 = ctrl_reg12_img; \
                        ctrl_reg12_img &= ~SERIAL_EEPROM_SK_MASK; \
                        *(char *)CNTL_REG_12 = ctrl_reg12_img

#define SELECT_EEPROM   ctrl_reg11_img |= SERIAL_EEPROM_CS_MASK; \
                        *(char *)CNTL_REG_11 = ctrl_reg11_img

#define DESELECT_EEPROM ctrl_reg11_img &= ~SERIAL_EEPROM_CS_MASK; \
                        *(char *)CNTL_REG_11 = ctrl_reg11_img

#define RESET_DI        ctrl_reg10_img &= ~SERIAL_EEPROM_DI_MASK; \
                        *(char *)CNTL_REG_10 = ctrl_reg10_img

#define SET_DI          ctrl_reg10_img |= SERIAL_EEPROM_DI_MASK; \
                        *(char *)CNTL_REG_10 = ctrl_reg10_img

#define  DEFAULT_PMD_NAME  "WS-X5155"
#define  DEFAULT_PMD_TYPE  32           /* multi-mode fiber */

/*---< Function Prototypes >---*/
void rd_eeprom_c();
void rd_eeprom();
void send_addr();
uInt16 read_word();
void write_eeprom();
void write_word();
void set_protect_reg();
uInt16 init_ser_eeprom();
void write_ser_eeprom();
void write_enable();
void disp_ser_eeprom();
int eeprom_checksum();
void write_disable();
void eeprom_generateChecksum();
int rd_pmd_type (void);
int rd_pmd_mode (void);

#define  MAX_INPUT_LEN  20


/********************************************************************

	Procedure name: rd_eeprom_c()

	Description: Reads n words from eeprom from specified address
		     to buffer.

	Inputs: start_addr is addr in EEPROM, length is number
		of words and buffer is pointer to buffer.

	Outputs:

	Side Effects:

********************************************************************/
void rd_eeprom_c(start_addr, length, buffer)
uInt16	start_addr;
uInt16	length;
uInt16	*buffer;
{
   uInt16 *p, i;
   Int16 s = spl7();

   /* set read code with address */
   start_addr &= E_ADDR_MASK;
   start_addr |= E_RD_CODE;

   if ((length == 0) || (length > E_MAX_LEN)) {
		splx(s);
     	return;
	}

   SELECT_EEPROM;

   /* send addr */
   send_addr(start_addr);

   p = buffer;

   /* read n words into buffer */
   for (i = 0; i < length; i++)
   {
	*p++ = read_word();
   }
   DESELECT_EEPROM;
   splx(s);
}


/********************************************************************

	Procedure name: rd_eeprom()

	Description: Reads n words from eeprom from specified address
		     to buffer.

	Inputs: start_addr is addr in EEPROM, length is number
			  of 16-bit words and buffer is pointer to buffer.

	Outputs:

	Side Effects:

********************************************************************/
void rd_eeprom(start_addr, length, buffer)
uInt16	start_addr;
uInt16	length;
uInt16	*buffer;
{
   uInt16 *p, i;
   Int16 s = spl7();

	if ((length == 0) || (length > E_MAX_LEN)) {
   		splx(s);
     	return;
	}

   	p = buffer;

   	/* read n words into buffer */
   	for (i = 0; i < length; i++)
   	{

   	   /* set read code with address */
   	   start_addr &= E_ADDR_MASK;
   	   start_addr |= E_RD_CODE;

   	   SELECT_EEPROM;
	
   	   /* send addr */
   	   send_addr(start_addr++);

   	   *p++ = read_word();

   	   DESELECT_EEPROM;
  	}
  	splx(s);
}



/********************************************************************

	Procedure name: send_addr(start_addr);

	Description: Sends the start clock & 3 bit opcode & 
		     6 bit address

	Inputs:

	Outputs:

	Side Effects:

********************************************************************/
void send_addr(start_addr)
uInt16	start_addr;	/* this has opcode & start address */
{
   uInt16	i,mask;

   RESET_DI;
   SELECT_EEPROM;
   TOGGLE_CLOCK;

   for( i = 0, mask = 0x0100; i < 9; i++)
   {
	/* set or clear port bit depending on 9th bit */
	if (start_addr & mask) {
		SET_DI;
	}
	else {
		RESET_DI;
	}
	TOGGLE_CLOCK;
	mask = mask >> 1;	/* shift to next bit */
   }
   RESET_DI;
}



/********************************************************************

	Procedure name: read_word();

	Description: Reads next sixteen bits from EEPROM

	Inputs:

	Outputs:

	Side Effects:

********************************************************************/
uInt16 read_word()
{
   register uInt16 result = 0;
   register uInt16 i;

   for( i = 0; i < 16; i++)
   {
      TOGGLE_CLOCK;
      /* data is read into bit 15 -> 0 */
      result |= (*((uInt16 *)STATUS_REG) & SERIAL_EEPROM_DO_MASK) ? (0x8000 >> i) : 0;
   }
   return(result);
}


/********************************************************************

	Procedure name: write_eeprom()

	Description: 

	Inputs:		length is number of 16-bit words

	Outputs:

	Side Effects:

********************************************************************/
void write_eeprom(start_addr, length, buffer)
uInt16	start_addr;
uInt16	length;
uInt16	*buffer;
{
   uInt16 *p, i, j;
   Int16 s = spl7();

   if ((length == 0) || (length > E_MAX_LEN)) {
		splx(s);
     	return;
	}

   /* send the write enable instruction */
   write_enable();

   p = buffer;

   /* write n words from buffer */
   for (i = 0; i < length; i++)
   {
      SELECT_EEPROM;

      /* set read code with address */
      start_addr &= E_ADDR_MASK;
      start_addr |= E_WR_CODE;

      /* send addr */
      send_addr(start_addr);

      write_word(p++);

      start_addr++;

 
      DESELECT_EEPROM;

      TOGGLE_CLOCK;

      /* wait until write is done */
      SELECT_EEPROM;

      for(j = 0; j < 6250; j++)   /* 6250 * 1.6 uS = 10 ms (approx)*/
      {
         TOGGLE_CLOCK;

	    if (*((volatile uInt16 *)STATUS_REG) & SERIAL_EEPROM_DO_MASK)
		break;
      }
      DESELECT_EEPROM;
   }

   /* send the write disable instruction */
   write_disable();
   splx(s);
}


/********************************************************************

	Procedure name: write_enable()

	Description: Sends the write enable instruction to serial PROM

	Inputs:

	Outputs:

	Side Effects:

********************************************************************/
void write_enable()
{
   uInt16	code;

   *(uChar *)CNTL_REG_14 = 0x00;  /* disable write protection */
   *(uChar *)CNTL_REG_13 = 0x01;  /* enable program enable */

   SELECT_EEPROM;

   code = E_WEN_CODE;

   send_addr(code);

   DESELECT_EEPROM;

   TOGGLE_CLOCK;
}



/********************************************************************

	Procedure name: write_disable()

	Description: Sends the write disable instruction to serial PROM

	Inputs:

	Outputs:

	Side Effects:

********************************************************************/
void write_disable()
{
   uInt16	code;

   SELECT_EEPROM;

   code = E_WDS_CODE;

   send_addr(code);

   DESELECT_EEPROM;

   TOGGLE_CLOCK;

   *(uChar *)CNTL_REG_14 = 0x00;  /* deactivate write protection */
   *(uChar *)CNTL_REG_13 = 0x00;  /* deactivate Program Enable */
}


/********************************************************************

	Procedure name: write_word();

	Description: Writes word specified by pointer to EEPROM
		     Assumes address has been written out

	Inputs:

	Outputs:

	Side Effects:

********************************************************************/
void write_word(buffer)
uInt16	*buffer;
{
   uInt16	i;

   for (i=0; i < 16; i++)
   {
	if (*buffer & (0x8000 >> i)) {
		SET_DI;
	}
	else {	
		RESET_DI;
	}
	TOGGLE_CLOCK;
   }
   RESET_DI;
}


/********************************************************************

	Procedure name: set_protect_reg()

	Description: Sets the protect register to address specified.
		     This prevents any writes to all addresses >=
		     specified address. Note that this instruction
		     can be done only when PRE signal is pulled high.
		     The PREN instruction has to given before the
		     PRWRITE is executed.

	Inputs:

	Outputs:

	Side Effects:

********************************************************************/
void set_protect_reg(address)
uInt16	address;
{
   /* give the PREN command */

   /* give the PRWRITE command */

}



/********************************************************************
 
 	Procedure: eeprom_generateChecksum
 
 	Description: Checksums the Serial EEPROM protected area.
 
 	Inputs:	None
 
 	Outputs: None
 
 	Side Effects: Inserts checksum in last two bytes of EEPROM.
 
 ********************************************************************/
void eeprom_generateChecksum()
{
	uInt16 chksum;
	uInt16 *ckptr;
	int i, rom_size;

	/* Do Checksum */
	chksum = 0;		/* Init Checksum value */
	rom_size = (sizeof(struct pmd_data) - 2)/2;
	ckptr = &prom.data_valid; 	/* Initialize the word pointer. */
	for (i=0; i < rom_size; i++, ckptr++)
		chksum += *ckptr;

	/* Save Checksum value */
   prom.checksum = 0x5555 - chksum;

} /* eeprom_generateChecksum */


/********************************************************************

	Procedure name: eeprom_checksum()

	Description: Computes the checksum of size PROT_AREA_SIZE in
		 buffer specified.

	Inputs:

	Outputs:

	Side Effects:

********************************************************************/
int eeprom_checksum()
{
	uInt16 chksum;
	uInt16 *ckptr;
	int i, rom_size;

	rd_eeprom(0, E_MAX_LEN, (uInt16 *)&prom);

	/* Do Checksum */
	chksum = 0;		/* Init Checksum value */
	rom_size = (sizeof(struct pmd_data) - 2)/2;
	ckptr = &prom.data_valid;	/* Initialize the word pointer. */

	for (i=0; i < rom_size; i++, ckptr++)
		chksum += *ckptr;

	/* Check for correct value */
	if (((chksum + *ckptr) & 0xffff) == 0x5555)
		return(0);		/* return success */

	return(-1);			/* else return failure */
}



/*--------------------------------------------------------------------*
 * rd_pmd_type()
 *    read in PMD type
 * return: 0  eeprom checksum failed
 *         else, PMD type(one based )
 *--------------------------------------------------------------------*/
int
rd_pmd_type()
{
   uInt32   rc;
   uInt16   type;
   uInt16   addr;

   struct pmd_data  pmd_eeprom;

   rc = eeprom_checksum();
   if (rc)
      return(0);    /* PMD eeprom checksum failed */

   addr = ((char *)&pmd_eeprom.feature - (char *)&pmd_eeprom.data_valid)/2;
   rd_eeprom_c(addr, 1, (uInt16 *)&type);
#ifdef  DEBUG
	printf("PMD_type = %d\n", (type & 0x03)+ 1);
#endif /* DEBUG */

   return( (type & 0x03)+ 1);

}  /* rd_pmd_type() */

/*--------------------------------------------------------------------*
 * rd_pmd_mode ()
 *    read in PMD mode
 * return: 0  eeprom checksum failed
 *         else, PMD mode 32 = Multi-mode fiber
 *               PMD mode 33 = Single-mode fiber
 *               PMD mode 34 = UTP 
 *--------------------------------------------------------------------*/
int
rd_pmd_mode ()
{
   uInt32   rc;
   uInt16   mode;
   uInt16   addr;

   struct pmd_data  pmd_eeprom;

   rc = eeprom_checksum();
   if (rc)
      return(0);    /* PMD eeprom checksum failed */

   addr = ((char *)&pmd_eeprom.pmd_type - (char *)&pmd_eeprom.data_valid)/2;
   rd_eeprom_c(addr, 1, (uInt16 *)&mode);
#ifdef  DEBUG
	printf("PMD_mode = %d\n", mode);
#endif /* DEBUG */

   return(mode);

}  /* rd_pmd_mode () */

