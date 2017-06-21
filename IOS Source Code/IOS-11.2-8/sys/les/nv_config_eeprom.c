/* $Id: nv_config_eeprom.c,v 3.3.48.2 1996/05/29 14:04:54 bew Exp $
 * $Source: /release/112/cvs/Xsys/les/nv_config_eeprom.c,v $
 *------------------------------------------------------------------
 * nv_config_eeprom.c - Code for writing/reading the Catalyst
 *                      (28C256 : 32K x 8) and (28C64: 8K x 8) EEPROMS.
 *                      Used by Cancun and Sapphire for configuration memory.
 *
 * December 1994, David Coli
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nv_config_eeprom.c,v $
 * Revision 3.3.48.2  1996/05/29  14:04:54  bew
 * CSCdi56794:  IOS private configuration not always recovered from NVRAM
 * Branch: California_branch
 * The private NVRAM structure priv_textbase wasn't properly getting turned
 * into an offset in NVRAM.
 *
 * Revision 3.3.48.1  1996/03/18  20:43:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.16.1  1996/01/24  22:24:51  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/12/07  22:30:40  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.2  1995/11/17  17:42:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:34:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/07  03:10:31  dcoli
 * CSCdi35405:  C1000 NVRAM : simultaneous config accesses can cause crash
 * Use nv_writeflag instead of unnecessary variable eeprom_writingmemory.
 *
 * Revision 2.3  1995/07/18  06:47:42  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.2  1995/06/09 13:11:45  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:39:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "../les/if_les.h"
#include "../os/nv.h"
#include "../../boot/src-68-c3000/confreg.h"
#include "../les/nv_config_eeprom.h"
#include "../ui/command.h"

static uchar *config_eeprom_ptr = NULL;


/* eeprom support needed for cancun */
void eeprom_nvwritebyte (uchar byte, uchar *base, int offset)
{
    int time;
    volatile uchar *ptr;

    ptr = NVADDRESS(base, offset); /* Get the address */

    *ptr = byte;  /* write byte */

    /*
     * Having written the byte, give some time for the
     * write to complete. Then start comparing. If the
     * comparison fails even after a thousand attempts,
     * give up and return. Is a thousand too low ?? Or
     * too high ?? Dunno. Looks reasonable.
     */
    for (time = EEPROM_TIMEOUT; time; time--) {
        if (*ptr == byte) {
            break;
        }
        process_sleep_for(1);
    }
    if (time == 0)
         printf("\nEEPROM write error - timeout");
}

uchar eeprom_nvreadbyte (uchar *base, int offset)
{
    volatile uchar *ptr;

    ptr = NVADDRESS(base, offset);	/* Get the offset */

    return(*ptr);
}

/*
 * This function writes bytes_to_write bytes into NVRAM
 * starting at the address nvaddr from the buffer pointed
 * to by srcptr.
 * Range checks to ensure that nvaddr does not extend beyond
 * the NVRAM are expected to be done by the calling routine.
 * That is, nvaddr must be less than end of NVRAM; also,
 * nvaddr + bytes_to_write must be less than end of NVRAM.
 */
static boolean nv_eeprom_page_write (uchar *srcptr, uchar *nvaddr,
				     int bytes_to_write)
{
    leveltype sts;
    sys_timestamp time_now, time_prev;
    int i;

    /* Disable all interrupts except NMI */
    sts = raise_interrupt_level(ALL_DISABLE);

    /*
     * Wait out till an NMI has occurred. Then go in and do our
     * writes. Each consecutive write should be within 100 usecs,
     * else, the chip will exit from write mode and go into read
     * mode. So we don't want any interrupts within our write loop.
     * Considering that there are around just four instructions
     * in the write loop, the worst case max write time should be
     * ~ 64 * 4 = 256 usecs.
     * So, hopefully we should be long done before the next NMI.
     */

    GET_TIMESTAMP(time_prev);
    GET_TIMESTAMP(time_now);
    while (TIMERS_EQUAL(time_now, time_prev)) {
        GET_TIMESTAMP(time_now);
    }

    while (bytes_to_write != 0) { /* Write full or partial page */
        *nvaddr++ = *srcptr++;
        bytes_to_write--;
    }

    reset_interrupt_level(sts);  /* Restore interrupt level */

    /*
     * Now check last location and loop till the value read
     * matches value written. To check the last written location,
     * decrement pointers by one.
     */
    for (nvaddr--, srcptr--, i = EEPROM_TIMEOUT; i > 0; i--) {
        if ( *nvaddr == *srcptr)
            break;
        process_sleep_for(1);  /* dismiss current process for a little while */
    }
    return ((i == 0) ? FALSE : TRUE);
}

/* 
 * routine that uses page mode writes to write nv ram more quickly 
 */

static void eeprom_nvwritepagemode (uchar *srcptr, uchar *nvaddr, int size)
{
    int bytes_to_write;
    int page_cnt=0;

    while (size > 0) {  /* Something left to write */
        /*
         * Are we writing into NVRAM starting at an offset that's
         * not a page (64 byte) boundary ?
         * If so, write partial page so that next write (if needed)
         * starts on a page boundary.
         */
        if ((ulong)nvaddr & NV_PAGEMASK) {
            /*
             * Get remainder bytes in page.
             */
            bytes_to_write = NV_PAGESIZE - ((ulong)nvaddr & NV_PAGEMASK);
            if (bytes_to_write > size) /* We need to write even less */
                bytes_to_write = size;
        }
        else  /* Write full page or whatever's left */
            bytes_to_write = (size < NV_PAGESIZE) ? size : NV_PAGESIZE;

        /*
         * If first attempt at write fails, try again before giving
         * up with a timeout error.
         */
        if (!nv_eeprom_page_write(srcptr, nvaddr, bytes_to_write)) {
            if (!nv_eeprom_page_write(srcptr, nvaddr, bytes_to_write)) {
                printf("\nEEPROM write error - timeout");
                return;
            }
        }

        /*
         * Bump up our pointers and decrement size.
         */
        nvaddr += bytes_to_write;
        srcptr += bytes_to_write;
        size   -= bytes_to_write;

        /*
         * Do not hog the CPU for more than 400 ms.  
         * Note we wait out 1 tick (4ms) before writing a page.
         */
        if (page_cnt > 100) {
             page_cnt = 0;
             process_sleep_for(1);
        } else {
             page_cnt++;
        }

    }
}


/******************************************************************************/
/*  NEW Common serial eeprom routines for config memory                       */
/******************************************************************************/

/*
 * malloc_config_eeprom_memory:
 * Preallocate memory block to use for reading and writing
 * of EEPROM.
 */

void malloc_config_eeprom_memory (void)
{
    if (!config_eeprom_ptr) { /* make sure we only do this once */
	config_eeprom_ptr = malloc_named(nvsize, "Cfg EEPROM Copy");
	if (!config_eeprom_ptr)
	    printf(nomemory);
        else
            write_terminal_nv_fallback = config_eeprom_ptr + sizeof(nvtype);
    }
}


/*
 * nv_getptr
 *
 * Return pointer to base of non-volatile memory.
 * Since we are using a serial EEPROM that cannot be directly accessed 
 * like other NVRAM devices, we keep a shadow copy of NVRAM in RAM, and 
 * use that until it is time to write it back to the real NVRAM.  
 * So return a pointer to the shadow memory.
 */

nvtype *nv_eeprom_getptr (void)
{
    nvtype *nv_info;
    nvtype_private *private_nv_info;
    uchar *nptr = get_nvbase();
    int i;

    nv_info = (nvtype *)config_eeprom_ptr;
    if (config_eeprom_ptr) {
        for (i = 0; i < nvsize; i++)
            config_eeprom_ptr[i] = eeprom_nvreadbyte(nptr, i);
        if ((nv_info->magic == NVMAGIC) &&
            (ipcrc((ushort *)nv_info, nvsize) == 0)) {
            /*
             * Good config in NVRAM. Adjust textbase to point to
             * our shadow NVRAM. Then recalculate checksum since
             * we are changing textbase.
             */
            nv_info->textbase += (ulong) nv_info; /* adjust to real pointer */
            /* 
             * If there is a private area, adjust priv_textbase.
             */
            private_nv_info = nv_private_getptr(nv_info);
            if (((char *)private_nv_info < ((char *)&nv_info->magic + nvsize)) 
	          && (private_nv_info->magic == NVMAGIC_PRIV)) {
                private_nv_info->priv_textbase += (ulong) nv_info;
	    }
            nv_info->checksum = 0;
            nv_info->checksum = ipcrc((ushort *)nv_info, nvsize);
        }
        else {
            /*
             * Corrupted config in NVRAM. Adjust textbase anyway
             * since we have a shadow NVRAM.
             */
            nv_info->textbase += (ulong) nv_info;
        }
    } else
        printf(nomemory);
    return(nv_info);

}


/*
 * nv_done
 *
 * Handles writing configuration data to memory.
 */

void nv_eeprom_done (nvtype *ptr)
{
    ushort *nv_magic_ptr = (ushort *)get_nvmagic();
    uchar *nptr = (uchar *)get_nvbase();
    nvtype_private *private_nv_info;
    uchar *mptr = (uchar *)ptr;

    if (nv_writeflag) { /* Actually write memory to eeprom */
       /* 
        * If there is a private area, make priv_textbase into an offset too.
	* Do this before ptr->textbase itself becomes an offset!
        */
        private_nv_info = nv_private_getptr(ptr);
        if (((char *)private_nv_info < ((char *)&ptr->magic + nvsize)) &&
           (private_nv_info->magic == NVMAGIC_PRIV)) {
            private_nv_info->priv_textbase -= (ulong) ptr;
        }
        ptr->textbase -= (ulong) ptr;        /* make into offset */
        ptr->checksum = 0;
        ptr->checksum = ipcrc((ushort *)ptr, nvsize);
        eeprom_nvwritepagemode(mptr, nptr, nvsize);
        eeprom_nvwritebyte(CONFIG_MAGIC >> 8, (uchar *)nv_magic_ptr, 0);
        eeprom_nvwritebyte(CONFIG_MAGIC & 255, (uchar *)nv_magic_ptr, 1);
    }
}


/*
 * nv_badptr
 *
 * Given a correctly checksummed portion of non-volatile memory, examine
 * the data structure components more closely for sanity.
 * If the configuration magic number is set, return FALSE to tell caller
 * that our NVRAM memory is OK.
 */

boolean nv_eeprom_badptr (void)
{
    ushort *nv_magic_ptr = get_nvmagic();

    if(((CONFIG_MAGIC >> 8) == eeprom_nvreadbyte((uchar *)nv_magic_ptr,0)) &&
       ((CONFIG_MAGIC & 0xff) == eeprom_nvreadbyte((uchar *)nv_magic_ptr,1))) 
        return(FALSE);
    else
        return(TRUE);
}
