/* $Id: clock.c,v 1.1.4.1 1996/05/09 15:00:50 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/clock.c,v $
 *-----------------------------------------------------------------
 * clock.c -- manages the Real-time clock
 *
 * October 1991, ?
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clock.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:50  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:48  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.2  1996/04/26  05:51:32  rlowe
 * Treat compiler warnings as errors.  Eliminate such warnings from
 * current ACP source.  Revise LCP handshake pulse width to be closer
 * to 1.0 usec.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:09  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:59  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:42  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:18  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*********************************************************************

        File:      	clock.c
	
	Version:	1.3
	
        Last Modified:  11/20/91

	Description:
	This code manages the Real-time clock for the HUB. The 
	clock is a part of the NVRAM.

	NOTE: This file is exclusive to the Boot PROM code. The 
	main code has its own version of this file (in the 
	system directory).
 
        Modification History:
	10/29/91 - Vinod - Included in the boot code.
	11/01/91 - Vinod - Split showtime into print_time and 
	showtime.
	11/20/91 - Vinod - Added days of the week, procedures 
	hextobcd(), bcdtohex(), get/save_baud_nvram.
	07/26/93 - Chandra - Introduced get/save CTS3 status routines
	11/30/93 - Chandra - Ported to Calypso

*********************************************************************/

/*******************
 *  Include files  *
 *******************/
#include "types.h"
/*#include "bootrom.h"*/
#include "clock.h"


uChar days[8][4] = {"   ", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

/*******************
 *  External Data  *
 *******************/


/*************************
 *  External Procedures  *
 *************************/

uChar hextobcd(uChar);
uChar bcdtohex(uChar);
uInt16 get_baud_nvram();
void save_baud_nvram(uInt16);
void wr_clk_access_pattern(uChar *);
void read_time(union clock_struct *);
void write_time(union clock_struct *);
int set_clock(struct realtime *);
void get_clock(struct realtime *);
void showclock();



/********************************************************************
 *
 *      Procedure: hextobcd
 *
 *      Description:
 *      Converts hex value to equivalent BCD value.
 *
 *      Inputs: The HEX value (a byte)
 *
 *      Outputs: The BCD value (a byte)
 *
 *      Side Effects:
 *
 ********************************************************************/
uChar
hextobcd(uChar hex)
{
  uChar hinib, lonib, bcd;

  if (hex > 99) return((uChar)0);

  hinib = (uChar)(((uInt16)hex)/10);
  lonib = (uChar)(((uInt16)hex) % 10);

  bcd = (uChar)(((hinib & 0x0f) << 4) | (lonib & 0x0f));
  return(bcd);
}


/********************************************************************
 *
 *      Procedure: bcdtohex
 *
 *      Description:
 *      Converts a BCD value to the equivalent HEX character.
 *
 *      Inputs: The BCD value (a byte)
 *
 *      Outputs: The HEX value (a byte)
 *
 *      Side Effects:
 *
 ********************************************************************/
uChar
bcdtohex(uChar bcd)
{
  uChar hinib, lonib, hex;

  hinib = (uChar)((bcd & 0xf0) >> 4);
  lonib = (uChar)(bcd & 0x0f);

  if ((hinib > 9) || (lonib > 9))
    return((uChar)'0');

  hex = (uChar)((((uInt16)hinib * 10) + (uInt16)lonib) & 0xff);

  return(hex);
}



/********************************************************************
 *
 *      Procedure: get_baud_nvram
 *
 *      Description:
 *      Gets the last set value used of the baud rate. This is 
 *      stored in the NVRAM.
 *
 *      Inputs: Nothing
 *
 *      Outputs: The last used Badu rate.
 *
 *      Side Effects:
 *
 ********************************************************************/
uInt16
get_baud_nvram()

{
  uChar *nvr = (uChar *)(SCRATCH_ADDR - 2);
  union wb {
    uInt16 wrd;
    struct bytes {
      uChar hi_b;
      uChar lo_b;
    } byts;
  } word_byte;

  word_byte.byts.hi_b = *(nvr++);
  word_byte.byts.lo_b = *(++nvr);

  return(word_byte.wrd);
}



/********************************************************************
 *
 *      Procedure: save_baud_nvram
 *
 *      Description:
 *      Save the currently used Baud rate in the NVRAM
 *
 *      Inputs: The current Baud rate
 *
 *      Outputs: Nothing
 *
 *      Side Effects:
 *
 ********************************************************************/
void
save_baud_nvram(uInt16 badrate)
{
  uChar *nvr = (uChar *)(SCRATCH_ADDR - 2);
  union wb {
    uInt16 wrd;
    struct bytes {
      uChar hi_b;
      uChar lo_b;
    } byts;
  } word_byte;

  word_byte.wrd = badrate;

  *(nvr++) = word_byte.byts.hi_b;
  *(++nvr) = word_byte.byts.lo_b;

  return;
}

int
get_CTS3_nvram()

{
  uChar *nvr = (uChar *)(SCRATCH_ADDR - 4);

  return(*nvr);
}

void
save_CTS3_nvram(int CTS3)
{
  uChar *nvr = (uChar *)(SCRATCH_ADDR - 4);

  *nvr = CTS3;

  return;
}


/********************************************************************
 *
 *      Procedure: wr_clk_access_pattern()
 *
 *      Description:
 *      Writes the 64 bit match pattern to access the clock
 *
 *      Inputs: An address in NVRAM space.
 *
 *      Outputs: Nothing
 *
 *      Side Effects:
 *
 ********************************************************************/

void wr_clk_access_pattern(uChar *nvr_ptr)
{
  uInt32 i, j, pattern;

  /* We need to write the magic bit pattern to the NVRAM */
  for (i = 0; i < 2; i++)
    for (j = 0, pattern = 0x5CA33AC5; j < 32; j++, pattern >>= 1)
      *nvr_ptr = pattern;

  return;
}



/********************************************************************
 *
 *      Procedure: read_time()
 *
 *      Description:
 *      Reads the time from the real-time clock.
 *
 *      Inputs: Pointer to a clock structure.
 *
 *      Outputs: Nothing, Reads the current time into the structure 
 *               pointed to by the passed pointer.
 *
 *      Side Effects:
 *
 ********************************************************************/

void 
read_time(union clock_struct *clock)

{
  uChar *nvram_ptr = (uChar *)(SCRATCH_ADDR);
  uChar save_char;
  uInt32 *lw_ptr, i;
  int s = spl7();

  /* First save the location this may corrupt */
  save_char = *nvram_ptr; 

  /* Write pattern to access the clock */
  wr_clk_access_pattern(nvram_ptr);

  /* Now read the clock data on DQ0 */
  lw_ptr = (uInt32 *)&clock->lw.clk_low;
  *lw_ptr = 0;
  for (i = 0; i < 32; i++)
    *lw_ptr |= ((uInt32)(*nvram_ptr & 0x01) << i);

  /* Repeat for the high long word of the clock */
  lw_ptr = (uInt32 *)&clock->lw.clk_high;
  *lw_ptr = 0;
  for (i = 0; i < 32; i++)
    *lw_ptr |= ((uInt32)(*nvram_ptr & 0x01) << i);

  /* Restore the corrupted byte */
  *nvram_ptr = save_char;

  splx(s);

  return;
}



/********************************************************************
 *
 *      Procedure: write_time()
 *
 *      Description:
 *      Sets the real-time clock.
 *
 *      Inputs: Pointer to a clock structure that has the current 
 *              time.
 *
 *      Outputs: Nothing
 *
 *      Side Effects:
 *
 ********************************************************************/

void 
write_time(union clock_struct *clock)
{
  uChar *nvram_ptr = (uChar *)(SCRATCH_ADDR);
  uChar save_char;
  uInt32 *lw_ptr, i;
  int s = spl7();

  /* First save the location this may corrupt */
  save_char = *nvram_ptr;

  /* Write pattern to access the clock */
  wr_clk_access_pattern(nvram_ptr);

  /* Now write the clock data on DQ0 */
  lw_ptr = (uInt32 *)&clock->lw.clk_low;
  for (i = 0; i < 32; i++)
    {
      *nvram_ptr = (uChar)*lw_ptr;
      *lw_ptr >>= 1;
    }

  /* Repeat for the High long word of the clock */
  lw_ptr = (uInt32 *)&clock->lw.clk_high;
  for (i = 0; i < 32; i++)
    {
      *nvram_ptr = (uChar)*lw_ptr;
      *lw_ptr >>= 1;
    }

  /* Restore the corrupted byte */
  *nvram_ptr = save_char;

  splx(s);

  return;

}



/********************************************************************
 *
 *      Procedure: set_clock
 *
 *      Description:
 *      Sets the clock.
 *
 *      Inputs: A Pointer to realtime structure with all fields 
 *              filled in.
 *
 *      Outputs: 0  -> Clock programmed OK
 *               -1 -> Some Error.
 *
 *      Side Effects:
 *
 ********************************************************************/

int 
set_clock(struct realtime *toset)

{
  union clock_struct set_time, get_time;

  set_time.parts.osc_bar = 0;
  set_time.parts.reset_bar = 1;
  set_time.parts.format = 0;
  set_time.parts.mon = hextobcd(toset->month);
  set_time.parts.dt = hextobcd(toset->date);
  set_time.parts.yr = hextobcd(toset->year);
  set_time.parts.weekday = hextobcd(toset->day);
  set_time.parts.hr = hextobcd(toset->hour);
  set_time.parts.min = hextobcd(toset->minute);
  set_time.parts.sec = hextobcd(toset->second);
  set_time.parts.centisecond = 0;

  write_time(&set_time);

  /* Now check for proper programming of the clock */
  read_time((union clock_struct *)&get_time);

/*  if (get_time != set_time)
    return(-1);*/

  return(0);  
}

/********************************************************************
 *
 *      Procedure: print_time
 *
 *      Description:
 *      Displays the current time according to the real-time clock.
 *
 *      Inputs: The clock structure after a call to read_time().
 *
 *      Outputs: Nothing 
 *
 *      Side Effects:
 *
 ********************************************************************/
/* #ifndef GNU */
#if  1
void
print_time(union clock_struct *clok)
{

    /* Print header */
/*  printf("Osc Enabled: %s ", (clok->parts.osc_bar ? "Off":" On"));
    printf("Reset bit ignored: %s ", (clok->parts.reset_bar ? "Y":"N"));
    printf("12/24 hour clock? : %s\r\n",(clok->parts.format ? "12":"24")); */

    if ((clok->parts.weekday < 1) || (clok->parts.weekday > 7))
      clok->parts.weekday = 0;

    printf("%s ",days[clok->parts.weekday]);
    printf("%2.2x/", clok->parts.mon);
    printf("%2.2x/", clok->parts.dt);
    printf("19%2.2x  ", clok->parts.yr);
    printf("%2.2x:",clok->parts.hr);
    printf("%2.2x:",clok->parts.min);
    printf("%2.2x.",clok->parts.sec);
    printf("%2.2x\r\n",clok->parts.centisecond);

    return;
}
#endif


/********************************************************************
 *
 *      Procedure: get_clock
 *
 *      Description:
 *      Gets the current time.
 *
 *      Inputs: A Pointer to realtime structure.
 *
 *      Outputs: Nothing, Reads the current time and copies to the 
 *               passed structure.
 *
 *      Side Effects:
 *
 ********************************************************************/

void
get_clock(struct realtime *toget)

{
  union clock_struct get_time;

  read_time((union clock_struct *)&get_time);

  toget->month = bcdtohex(get_time.parts.mon);
  toget->date = bcdtohex(get_time.parts.dt);
  toget->year = bcdtohex(get_time.parts.yr);
  toget->day = bcdtohex(get_time.parts.weekday);
  toget->hour = bcdtohex(get_time.parts.hr);
  toget->minute = bcdtohex(get_time.parts.min);
  toget->second = bcdtohex(get_time.parts.sec);
  toget->hundredth = bcdtohex(get_time.parts.centisecond);
/* RLB DBG */
  print_time(&get_time);

  return;  
}


/********************************************************************
 *
 *      Procedure: showclock
 *
 *      Description:
 *      Displays the current time according to the real-time clock.
 *
 *      Inputs: Nothing
 *
 *      Outputs: Nothing 
 *
 *      Side Effects:
 *
 ********************************************************************/

#ifndef GNU
void 
showclock()
{

  union clock_struct clock;

  /* Read time into structure */
  read_time((union clock_struct *)&clock);

  /* Print the time */
  printf("\r\n");
  print_time(&clock);

  return;	 
}
#endif












