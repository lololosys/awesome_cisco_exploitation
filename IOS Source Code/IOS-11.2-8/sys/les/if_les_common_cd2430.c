/* $Id: if_les_common_cd2430.c,v 3.1.2.3 1996/07/26 22:08:33 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_common_cd2430.c,v $
 *------------------------------------------------------------------
 * if_les_common_cd2430.c - Common code for the synchronous and
 *                          asynchronous interface drivers for the Cirrus
 *                          CL-CD2430 controller
 *
 * April 1996, Jose Hernandez
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les_common_cd2430.c,v $
 * Revision 3.1.2.3  1996/07/26  22:08:33  jbalestr
 * CSCdi64159:  Modular cobra commit missed some bug fixes for 11.2
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/07/04  02:07:05  jhernand
 * CSCdi61640:  Cobra code does not conform to Cisco guidelines
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/06/17  08:33:50  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.1  1996/04/22  21:44:20  samuell
 * Placeholder files for porting Modular 2500 Cobra platform from 11.1.
 * Modular 2500 Cobra files were derived from the original Cobra 11.1
 * Release by code separation into platform-independent code and 2500
 * platform-dependent code files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "logger.h"
#include "stacks.h"
#include "subsys.h"
#include "../if/network.h"
#include "../les/if_les.h"
#include "../les/if_les_common_cd2430.h"
#include "../dev/file_io.h"

cd2430_channel_inst_t *cd2430_channel_info[CD2430MAX * CD2430_CHIPCHAN];
cd2430_quadart *cd2430_reg_table[CD2430MAX];
hwidbtype *cd2430_sync_idbs[CD2430MAX*CD2430_CHIPCHAN];


#define CD2402_INSTR_SIZE	18 /* Size in bits of a CD2402 instruction */
#define CHAR_SIZE		8  /* Size in bits of a character */

/*
 * sendccr()
 *      Send a command to the CD2430 ccr register
 */
void sendccr (cd2430_quadart *csr, uchar cmd)
{
    while (csr->ccr)
        ;
    csr->ccr = cmd;
    while (csr->ccr)
        ;
}


/*
 * store_2402()
 *      Store next 18-bit value to 2402
 */
static inline void store_2402 (cd2430_quadart *csr, ulong val)
{
    /*
     * Feed value, strobe dmr
     */
    csr->airl = val;
    csr->airm = val >> 8;
    csr->airh = val >> 16;
    csr->dmr = DMR_SSTEP|DMR_CLKDIS;
}

/*
 * cd2402_download()
 *      Download firmware to the programmable flavor of 24XX
 */
void cd2402_download (cd2430_quadart *csr, uchar *cd2402_firmware)
{
    uchar *p;
    uint index;
    ulong val;
    leveltype level;

    level = raise_interrupt_level(ALL_DISABLE);

    /*
     * Set jmp 0x1fff instruction
     */
    csr->airl = 0xff;
    csr->airm = 0x3f;
    csr->airh = 0x3;

    /*
     * Set data write mode w. clock stopped
     */
    csr->mtcr = 1;
    csr->dmr = DMR_CLKDIS;
    csr->mtcr = 2;

    /*
     * Encapsulate repetetive loop condition
     */
#define STORE(v) store_2402(csr, v); \
    if (++index >= cd2402_firmware_size) break;

    /*
     * Extract 18-bit words from the byte array, write them to the chip
     */
    index = 0;
    p = cd2402_firmware;
    for (;;) {
        /*
         * Word 1: lies on aligned byte
         */
        val =   (*p++);
        val |=  (*p++ << 8);
        val |= ((*p & 0x3) << 16);
        STORE(val);

        /*
         * Word 2: lies two bits up on byte
         */
        val =  ((*p++ >> 2) & 0x3F);
        val |=  (*p++ << 6);
        val |= ((*p & 0xF) << 14);
        STORE(val);

        /*
         * Word 3: lies four bits up on byte
         */
        val =  ((*p++ >> 4) & 0xF);
        val |=  (*p++ << 4);
        val |= ((*p & 0x3F) << 12);
        STORE(val);

        /*
         * Word 4: lies six bits up on byte, last byte is aligned,
         * and thus we start over at word 1 next time.
         */
        val =  ((*p++ >> 6) & 0x3);
        val |=  (*p++ << 2);
        val |=  (*p++ << 10);
        STORE(val);
    }
#undef STORE

    /*
     * Fill in trailing storage with no-op
     */
    for ( ; index <= 0x1fff; ++index) {
        store_2402(csr, 0x1e000);
    }

    /*
     * Now tell him to run the code
     */
    csr->airl = 0xff;                   /* jump 0x1fff */
    csr->airm = 0x3f;
    csr->airh = 0x3;
    csr->mtcr = 0x1;                    /* single step mode */
    csr->dmr = DMR_SSTEP|DMR_CLKDIS;    /* step */
    csr->dmr = DMR_SSTEP|DMR_CLKDIS;    /* step */
    csr->mtcr = 0x3;                    /* end single step */
    csr->dmr = 0;                       /* run */

    reset_interrupt_level(level);
}

/*
 * flash_get_firmware()
 * 	This function checks both partitions in flash for the presence of
 * a CD2402 firmware file. If present, it copies the file into an array
 * and returns the pointer to that array. If present, this firmware
 * overrides the linked-in firmware, and is used to run the Cirrus
 * controllers.
 */
uchar *flash_get_firmware (void)
{
    int fh, index;
    uchar *flash_firmware;
    uchar *cp;
    uchar file1[50], file2[50];
 
   /*
    * Make strings for the file names. This is needed because of an oddity
    * with the file_open() routine. Using the file name string directly
    * will lead to text segment corruption.
    */
    strcpy(file1, "flash:1:cd2402-FIRMWARE");
    strcpy(file2, "flash:2:cd2402-FIRMWARE");

    /*
     * Check the first partition for existence of firmware file.
     */
    fh = file_open(file1, FILE_FLAGS_RDONLY | FILE_FLAGS_CHECKSUM);
 
    /*
     * If the first partition does not have it, try the second partition.
     */
    if (fh < 0)
        fh = file_open(file2, FILE_FLAGS_RDONLY | FILE_FLAGS_CHECKSUM);
 
    /*
     * If the file is not in flash, return now.
     */
    if (fh < 0)
        return (NULL);
 
    /*
     * The firmware file exists in flash. Allocate memory for the array
     * that will store the firmware, and read the firmware into that
     * array. The exact amount of memory to hold a firmware array is
     * allocated since the firmware consists of exactly 8 Kb of 18-bit
     * instructions, and we can thus calculate the memory required to
     * store the file as 8-bit characters.
     */
    flash_firmware = malloc((cd2402_firmware_size*CD2402_INSTR_SIZE)/CHAR_SIZE);
    if (!flash_firmware) { /* No memory to store array */
        file_close(fh);
        return (NULL);
    }
 
    cp = flash_firmware;
    index = 0;
    while (file_read(fh, cp++, 1))
        if (++index > ((cd2402_firmware_size*CD2402_INSTR_SIZE)/CHAR_SIZE))
            break;
    /*
     * Make sure that the firmware file in flash was the right size.
     */
    if (index != ((cd2402_firmware_size*CD2402_INSTR_SIZE)/CHAR_SIZE)) {
        free(flash_firmware);
        flash_firmware = NULL;
    }
 
    /*
     * Now clean up and return.
     */
    file_close(fh);
    return (flash_firmware);
}

/*
 * cd2430_show_registers_global()
 * This function will print out the global registers in a CD2430 controller.
 */
void cd2430_show_registers_global (cd2430_quadart *reg_table)
{
    /*
     * Print the global registers
     */
    printf("\nGlobal registers");
    printf(
"\n  rpilr 0x%x, rir 0x%x, risr 0x%x, rfoc 0x%x, rdr 0x%x",
        reg_table->rpilr, reg_table->rir, reg_table->risr,
                                  reg_table->rfoc, reg_table->rdr);
    printf(
"\n  tpilr 0x%x, tir 0x%x, tisr 0x%x, tftc 0x%x, tdr 0x%x",
        reg_table->tpilr, reg_table->tir, reg_table->tisr,
                                  reg_table->tftc, reg_table->tdr);
    printf(
"\n  mpilr 0x%x, mir 0x%x, misr 0x%x",
        reg_table->mpilr, reg_table->mir, reg_table->misr);
    printf(
"\n  bercnt 0x%x, stk 0x%x",
        reg_table->bercnt, reg_table->stk);
}

/*
 * cd2430_copy_per_channel_registers should be called with interrupts disabled
 * and the channel register set.
 */
void cd2430_copy_per_channel_registers(cd2430_quadart *reg_table,
				       per_channel_registers *pcr)
{
   pcr->car = reg_table->car;
   pcr->cmr = reg_table->cmr;
   pcr->cor1 = reg_table->cor1;
   pcr->cor2 = reg_table->cor2;
   pcr->cor3 = reg_table->cor3;
   pcr->cor4 = reg_table->cor4;
   pcr->cor5 = reg_table->cor5;
   pcr->cor6 = reg_table->cor6;
   pcr->cor7 = reg_table->cor7;
   pcr->schr1 = reg_table->schr1;
   pcr->schr2 = reg_table->schr2;
   pcr->schr3 = reg_table->schr3;
   pcr->schr4 = reg_table->schr4;
   pcr->scrl = reg_table->scrl;
   pcr->scrh = reg_table->scrh;
   pcr->lnxt = reg_table->lnxt;
   pcr->cpsr = reg_table->cpsr;
   pcr->rbpr = reg_table->rbpr;
   pcr->rcor = reg_table->rcor;
   pcr->tbpr = reg_table->tbpr;
   pcr->tcor = reg_table->tcor;
   pcr->ccr = reg_table->ccr;
   pcr->stcr = reg_table->stcr;
   pcr->csr = reg_table->csr;
   pcr->msvr_rts = reg_table->msvr_rts;
   pcr->msvr_dtr = reg_table->msvr_dtr;
   pcr->livr = reg_table->livr;
   pcr->ier = reg_table->ier;
   pcr->licr = reg_table->licr;
   pcr->dmabsts = reg_table->dmabsts;
   pcr->arbadrl = reg_table->arbadrl;
   pcr->arbadru = reg_table->arbadru;
   pcr->brbadrl = reg_table->brbadrl;
   pcr->brbadru = reg_table->brbadru;
   pcr->arbcnt = reg_table->arbcnt;
   pcr->brbcnt = reg_table->brbcnt;
   pcr->arbsts = reg_table->arbsts;
   pcr->brbsts = reg_table->brbsts;
   pcr->rcbadrl = reg_table->rcbadrl;
   pcr->rcbadru = reg_table->rcbadru;
   pcr->atbadru = reg_table->atbadru;
   pcr->atbadrl = reg_table->atbadrl;
   pcr->btbadru = reg_table->btbadru;
   pcr->btbadrl = reg_table->btbadrl;
   pcr->btbcnt = reg_table->btbcnt;
   pcr->atbcnt = reg_table->atbcnt;
   pcr->btbsts = reg_table->btbsts;
   pcr->atbsts = reg_table->atbsts;
   pcr->tcbadrl = reg_table->tcbadrl;
   pcr->tcbadru = reg_table->tcbadru;
   pcr->tpr = reg_table->tpr;
   pcr->rtpr = reg_table->rtpr;
   pcr->gt1 = reg_table->gt1;
   pcr->ttr = pcr->ttr;
}

/*
 * cd2430_show_registers_perchannel ()
 * This routine will print the values of the per-channel registers for
 * the controller whose register-table structure is passed in as an 
 * argument. Note that the calling routine must have set the channel
 * access register on the controller to the appropriate channel prior
 * to calling this routine.
 */
void cd2430_show_registers_perchannel (per_channel_registers *reg_table)
{
    /*
     * Print the per-channel registers
     */
    printf("\nPer-channel registers for channel %d", reg_table->car);
    printf(
"\n  Option registers");
    printf(
"\n  0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
    reg_table->cor1, reg_table->cor2, reg_table->cor3,
    reg_table->cor4, reg_table->cor5, reg_table->cor6, reg_table->cor7);

    printf(
"\n  Command and status registers");
    printf(
"\n  cmr 0x%02x, ccr 0x%02x, csr 0x%02x, msvr-rts 0x%02x, msvr-dtr 0x%02x",
    reg_table->cmr, reg_table->ccr, reg_table->csr, reg_table->msvr_rts,
    reg_table->msvr_dtr);

    printf(
"\n  Clock option registers");
    printf(
"\n  rcor 0x%02x, rbpr 0x%02x, tcor 0x%02x, tbpr 0x%02x",
    reg_table->rcor, reg_table->rbpr, reg_table->tcor, reg_table->tbpr);

    printf(
"\n  Interrupt registers");
    printf(
"\n  ier 0x%02x, livr 0x%02x, licr 0x%02x",
    reg_table->ier, reg_table->livr, reg_table->licr);

    printf(
"\n  DMA buffer status 0x%02x", reg_table->dmabsts);

    printf(
"\n  DMA receive registers");
    printf(
"\n  arbaddr 0x%x, arbcnt %d, arbsts 0x%x",
    reg_table->arbadrl | (reg_table->arbadru << 16),
    reg_table->arbcnt, reg_table->arbsts);
    printf(
"\n  brbaddr 0x%x, brbcnt %d, brbsts 0x%x",
    reg_table->brbadrl | (reg_table->brbadru << 16),
    reg_table->brbcnt, reg_table->brbsts);
    printf(
"\n  rcbaddr 0x%x", reg_table->rcbadrl | (reg_table->rcbadru << 16));

    printf(
"\n  DMA transmit registers");
    printf(
"\n  atbaddr 0x%x, atbcnt %d, atbsts 0x%x",
    reg_table->atbadrl | (reg_table->atbadru << 16),
    reg_table->atbcnt, reg_table->atbsts);
    printf(
"\n  btbaddr 0x%x, btbcnt %d, btbsts 0x%x",
    reg_table->btbadrl | (reg_table->btbadru << 16),
    reg_table->btbcnt, reg_table->btbsts);
    printf(
"\n  tcbaddr 0x%x", reg_table->tcbadrl | (reg_table->tcbadru << 16));

    printf(
"\n  Special character registers");
    printf(
"\n  schr1 0x%02x, schr2 0x%02x, schr3 0x%02x, schr4 0x%02x",
    reg_table->schr1, reg_table->schr2, reg_table->schr3, reg_table->schr4);
    printf(
"\n  scrl 0x%x, scrh 0x%x, lnxt 0x%x",
    reg_table->scrl, reg_table->scrh, reg_table->lnxt);
}

