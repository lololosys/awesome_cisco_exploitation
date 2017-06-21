/* $Id: eeprom.c,v 1.1.4.1 1996/05/09 15:01:02 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/eeprom.c,v $
 *------------------------------------------------------------------
 * eeprom.c -- Functions to access the serial eeprom(NS93C46)
 *
 * January 1995, rlb
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: eeprom.c,v $
 * Revision 1.1.4.1  1996/05/09  15:01:02  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:00  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:23  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:11  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:55  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:30  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.6  1995/09/14  02:45:51  rbau
 * moved out the driver portion to pmd_eeprom.c in drivers/pmc. retain the functions to support command line interface to set/show pmd.
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
extern  char  ctrl_reg10_img;
extern  char  ctrl_reg11_img;
extern  char  ctrl_reg12_img;
extern  char  ctrl_reg13_img;

#define  DEFAULT_PMD_NAME  "WS-X5155"
#define  DEFAULT_PMD_TYPE  32           /* multi-mode fiber */

/*---< Function Prototypes >---*/
int  set_pmdrom(char *);
void show_pmdrom(void);
extern char *strchr(char *, char);

extern struct pmd_data prom; 
#define  MAX_INPUT_LEN  20

char tmp_buf[MAX_INPUT_LEN];  

/*--------------------------------------------------------------------*
 * set_pmdrom()
 *--------------------------------------------------------------------*/
int
set_pmdrom(char *param)
{
   char *cp;
	int  err;
	uInt32 tmp;

	rd_eeprom(0, E_MAX_LEN, (uInt16 *)&prom);

   prom.data_valid = 0x55aa;   
   memset((char *)&prom.empty, 0, sizeof(prom.empty));

   do
   {
      printf("   Enter Serial Number [%ld]: ", prom.serial_number); 
      get_line(tmp_buf, MAX_INPUT_LEN);  

      err = 0;
      if (strlen(tmp_buf) != 0)
      {
        /*  if (!sscanf_x(tmp_buf, fmt_Int32, &prom.serial_number))  */
         if (!sscanf_x(tmp_buf, "lX", &prom.serial_number))
		   	err = 1;
      }
   } while (err);

   /* HW version */
   do 
   {
      printf("   Enter H/W Version [%d.%d]: ",
				prom.hw_version_high, prom.hw_version_low);
      get_line(tmp_buf, MAX_INPUT_LEN);
      err = 0;
      if (strlen(tmp_buf) != 0)
   	{
         cp = (char *)strchr(tmp_buf, '.');
         *cp = '\0';  
         if (!sscanf_x(tmp_buf, "%d", &tmp))
            err = 1;
         else
            prom.hw_version_high = (uInt16)tmp;

         if (!sscanf_x(cp+1, "%d", &tmp))
				err = 1;
         else
            prom.hw_version_low = (uInt16)tmp;
      }
   } while (err);

   /* PMD module type */
   do
   {
		/* default to multi-mode fiber */
		prom.pmd_type = (uInt16)DEFAULT_PMD_TYPE;
      printf("   Enter PMD Module Type [%d]: ", prom.pmd_type);
      err = 0;
      get_line(tmp_buf,5);
      if (strlen(tmp_buf) != 0)
      {
         if (!sscanf_x(tmp_buf, "%d", &tmp))
            err = 1;
         else
            prom.pmd_type = (uInt16)tmp;
      }
   } while (err);

	/* PMD module string */
	do
	{
		if ( strlen(prom.pmd_name) > 8 )
		{
			strcpy(prom.pmd_name, DEFAULT_PMD_NAME);
		}

		printf("   Enter PMD Module String [%s]: ", prom.pmd_name);
		err = 0;
		get_line(tmp_buf,10);

      if (strlen(tmp_buf) != 0)
		{
			if ( strlen(tmp_buf) > 8)
				err = 1;
			else
				strcpy(prom.pmd_name, tmp_buf); 
		}
   } while (err);

   /* PMD feature index */
   do
   {
      printf("   Enter PMD H/W Changes [%d]: ", prom.feature);
      err = 0;
      get_line(tmp_buf,3);
      if (strlen(tmp_buf) != 0)
      {
         if (!sscanf_x(tmp_buf, "%d", &tmp))
            err = 1;
         else
            prom.feature = (uInt16)tmp;
      }
   } while (err);


   /* ring the bell */
   printf("%c",0x07);

   printf("Update Serial EEPROM on ATM PMD module with the above information (y/n) [n]? ");
   get_line(tmp_buf, 3);

   if (strchr(tmp_buf, 'y') != (char *)0 )
   {
      eeprom_generateChecksum();
      write_eeprom(0, E_MAX_LEN, (uInt16 *)&prom.data_valid);
   }
   return(1);
}

/*--------------------------------------------------------------------*
 * show_pmdrom()
 *--------------------------------------------------------------------*/
void
show_pmdrom(void)
{
	int  rc;

   rc = eeprom_checksum();
   if (rc)
	{
		printf("\nPMD eeprom checksum failed\n");
		return;
	}

	printf("PMD Serial EEPROM Contents:\n");
	rd_eeprom_c(0, E_MAX_LEN, (uInt16 *)&prom.data_valid);
	printf("   PMD valid  : 0x%x\n",  prom.data_valid);
	printf("   PMD checksum  : 0x%x\n",  prom.checksum);
	printf("   PMD serial number: : %ld\n",  prom.serial_number);
	printf("   PMD h/w version %d.%d\n",
			prom.hw_version_high,prom.hw_version_low);
	printf("   PMD type : %d\n",  prom.pmd_type);
	printf("   PMD string : %s\n",  prom.pmd_name);
	printf("   PMD h/w changes: : 0x%d\n",  prom.feature);

} /* show_pmdrom() */

