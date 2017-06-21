/* $Id: xmodem_generic.c,v 3.1.14.1 1996/06/16 21:18:31 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/xmodem_generic.c,v $
 *------------------------------------------------------------------
 * Xmodem and Ymodem-batch protocol routines
 *
 * May 1996, Padmaja Gogineni
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xmodem_generic.c,v $
 * Revision 3.1.14.1  1996/06/16  21:18:31  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/05/10  17:48:08  pgoginen
 * Placeholder files for new development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Includes
 */

#include "types.h"
#include "../src/fsio.h"
#include "flash_fs_les.h"
#include "xmodem_generic.h"
#include "../src/console.h"
#include "extern.h"
#include "dev.h"
#include "mon_boot.h"
#include "fstat.h"
#include "simpfsio.h"

/*
 *  CRC table of values
 */

unsigned short crctab[256] = {
    0x0000,  0x1021,  0x2042,  0x3063,  0x4084,  0x50a5,  0x60c6,  0x70e7,
    0x8108,  0x9129,  0xa14a,  0xb16b,  0xc18c,  0xd1ad,  0xe1ce,  0xf1ef,
    0x1231,  0x0210,  0x3273,  0x2252,  0x52b5,  0x4294,  0x72f7,  0x62d6,
    0x9339,  0x8318,  0xb37b,  0xa35a,  0xd3bd,  0xc39c,  0xf3ff,  0xe3de,
    0x2462,  0x3443,  0x0420,  0x1401,  0x64e6,  0x74c7,  0x44a4,  0x5485,
    0xa56a,  0xb54b,  0x8528,  0x9509,  0xe5ee,  0xf5cf,  0xc5ac,  0xd58d,
    0x3653,  0x2672,  0x1611,  0x0630,  0x76d7,  0x66f6,  0x5695,  0x46b4,
    0xb75b,  0xa77a,  0x9719,  0x8738,  0xf7df,  0xe7fe,  0xd79d,  0xc7bc,
    0x48c4,  0x58e5,  0x6886,  0x78a7,  0x0840,  0x1861,  0x2802,  0x3823,
    0xc9cc,  0xd9ed,  0xe98e,  0xf9af,  0x8948,  0x9969,  0xa90a,  0xb92b,
    0x5af5,  0x4ad4,  0x7ab7,  0x6a96,  0x1a71,  0x0a50,  0x3a33,  0x2a12,
    0xdbfd,  0xcbdc,  0xfbbf,  0xeb9e,  0x9b79,  0x8b58,  0xbb3b,  0xab1a,
    0x6ca6,  0x7c87,  0x4ce4,  0x5cc5,  0x2c22,  0x3c03,  0x0c60,  0x1c41,
    0xedae,  0xfd8f,  0xcdec,  0xddcd,  0xad2a,  0xbd0b,  0x8d68,  0x9d49,
    0x7e97,  0x6eb6,  0x5ed5,  0x4ef4,  0x3e13,  0x2e32,  0x1e51,  0x0e70,
    0xff9f,  0xefbe,  0xdfdd,  0xcffc,  0xbf1b,  0xaf3a,  0x9f59,  0x8f78,
    0x9188,  0x81a9,  0xb1ca,  0xa1eb,  0xd10c,  0xc12d,  0xf14e,  0xe16f,
    0x1080,  0x00a1,  0x30c2,  0x20e3,  0x5004,  0x4025,  0x7046,  0x6067,
    0x83b9,  0x9398,  0xa3fb,  0xb3da,  0xc33d,  0xd31c,  0xe37f,  0xf35e,
    0x02b1,  0x1290,  0x22f3,  0x32d2,  0x4235,  0x5214,  0x6277,  0x7256,
    0xb5ea,  0xa5cb,  0x95a8,  0x8589,  0xf56e,  0xe54f,  0xd52c,  0xc50d,
    0x34e2,  0x24c3,  0x14a0,  0x0481,  0x7466,  0x6447,  0x5424,  0x4405,
    0xa7db,  0xb7fa,  0x8799,  0x97b8,  0xe75f,  0xf77e,  0xc71d,  0xd73c,
    0x26d3,  0x36f2,  0x0691,  0x16b0,  0x6657,  0x7676,  0x4615,  0x5634,
    0xd94c,  0xc96d,  0xf90e,  0xe92f,  0x99c8,  0x89e9,  0xb98a,  0xa9ab,
    0x5844,  0x4865,  0x7806,  0x6827,  0x18c0,  0x08e1,  0x3882,  0x28a3,
    0xcb7d,  0xdb5c,  0xeb3f,  0xfb1e,  0x8bf9,  0x9bd8,  0xabbb,  0xbb9a,
    0x4a75,  0x5a54,  0x6a37,  0x7a16,  0x0af1,  0x1ad0,  0x2ab3,  0x3a92,
    0xfd2e,  0xed0f,  0xdd6c,  0xcd4d,  0xbdaa,  0xad8b,  0x9de8,  0x8dc9,
    0x7c26,  0x6c07,  0x5c64,  0x4c45,  0x3ca2,  0x2c83,  0x1ce0,  0x0cc1,
    0xef1f,  0xff3e,  0xcf5d,  0xdf7c,  0xaf9b,  0xbfba,  0x8fd9,  0x9ff8,
    0x6e17,  0x7e36,  0x4e55,  0x5e74,  0x2e93,  0x3eb2,  0x0ed1,  0x1ef0
};

char *error_messages[MAX_XMODEM_ERRS] = {
  "\nSender aborted download...\n",
  "\nSender using different protocol - aborting download...\n",
  "\nTimeout waiting for data - aborting download...\n",
  "\nTimeout waiting for header - aborting download...\n",
  "\nError syncing with sender - aborting download...\n",
  "\nERR:File not a valid executable\n",
  "\nERR:Image size exceeds free flash space\n",
  "\nERR:Image size exceeds free mem space\n",
  "\nTruncated image received - aborting download...\n"
};

/* option flags */
int     crcmode;        /* CRC or checksums? */
int     ymdmbat;        /* YMODEM batch protocol */
int     flashcopy;      /* Copy to Flash or DRAM */
int     loadimage;      /* Launch image on xmodem completion */

uchar *mem_ptr, *image_load_start_ptr;
int firstblock;
long expect_filesize, recv_filesize, ymodem_filesize;
flash_fhdr_t ffh;
long tensecs, loop_for_onesec;
int error_msg;

#if defined(MARS)
  static char flash_dev_name[] = "flash:";
  /*
  * supported_optstr - options implemented for this platform
  * input_optstr - options accepted from user
  * the "-s" option is accepted from the user for cross platform
  * compatibility - Volcano plans to support this option
  */
  static char supported_optstr[] = "cyrx";
  static char input_optstr[] = "cyrxs";
#else
  static char flash_dev_name[] = "bootflash:";
  static char supported_optstr[] = "cy";
  static char input_optstr[] = "cy";
#endif

int xmodem (int argc, char* argv[])
{

    int i, has_option;
    int exitcode;
    long onesec;
    static char filename[80];
#if defined(MARS)
    extern int optind;
    extern char *optarg;
    char loadname[BOOTSTRSIZ];
#endif 

    has_option = 0;
    flashcopy = 1;	 /* Default is copy to flash */
    loadimage = 0;        /* Launch image by default */
    crcmode = 0;
    ymdmbat = 0;
    error_msg = -1;

    /* Process the arguments. option string is cy - 'c' for 
       requesting CRC-16, 'y' for ymodem-batch protocol. */ 

#if !defined(MARS)
    if(argc < 2 || argc > 3){ 
	usage(argv[0]);
	return(1);
    }
#endif 

    while((i = getopt(argc,argv,input_optstr)) >= 0) {
	has_option = 1;
        switch(i) {
	case 'c':    /* use crc-16 instead of 16-bit  checksum */
	    crcmode = 1;
	    break;
	case 'y':
	    ymdmbat = 1;
	    break;
#if defined(MARS)
	case 'r':
	    flashcopy = 0;
	    break;
	case 'x':
	    loadimage = 1;
	    break;
	case 's':
	    puts("\nThe -s speed option is not supported on this platform\n");
	    return(1);
   	    break; 
#endif
	default:
	    usage(argv[0]);
	    return(1);
	}
    }

#if defined(MARS)
    if (flashcopy && ((argc - optind) != 1)) {
	/* need filename for copy to flash */
	usage(argv[0]);
	return(1);
    }
#else
    if(has_option && (argc == 2)) {
	usage(argv[0]);
	return(1);
    }
#endif

    puts("Do not start the sending program yet...\n");
    if(!crcmode)                    /* default for Ymodem is CRC-16 */
        crcmode = ymdmbat ? 1 : 0;

    onesec = 100 * timer_calibrate(10000);

    /* Used for xm_getchar, takes onesec to go thru the the loop 
       loop_for_onesec times
     */ 
    loop_for_onesec = onesec/5;                  
    tensecs = 10 * loop_for_onesec;

    if (flashcopy) {

	strcpy(filename, argv[argc-1]);

        /* Display the contents of bootflash */
        disp_flash();

       /* Warn the user this operation erases everything in bootflash
        *  Give him a chance to bail out 
        */
       puts("\nWARNING: All existing data in bootflash will be lost!");
    }

    puts("\nInvoke this application only for disaster recovery.");
    if(getc_answer("\nDo you wish to continue? y/n", "yn", 'n' ) == 'n') {
	printf("\nDownload aborted - user request\n");
	return(1);
    }

    /* Get free mem size and bootflash size. We need enough DRAM to
     * hold the image before we burn it into flash  
     */
    if((flashcopy && !xmodem_flash_size()) || !xmodem_freemem()) { 
	printf("\nERR: Not enough memory to download image\n");
	return(1);
    }

    image_load_start_ptr = xmodem_download_start_address();


    /* We are ready to receive the file now. Prompt the user to start
     * the sending program now.
     */
    printf("Ready to receive file %s ...\n", filename);

    exitcode = receive();

    /* pause for sender to stop */

    sleep(100);                    

    if(exitcode == 0){
	if (flashcopy) 
            if (!xmodem_flash_image_copy(filename))
	        return (1);

	puts("\nDownload Complete!\n");

#if defined(MARS)
        if (flashcopy)
	    sprintf(loadname, "%s%s", DEV_FLASH_ID, filename);
	else
	    sprintf(loadname, "%s%x", DEV_KMEM1_ID, 
				      (image_load_start_ptr - mainmemstart()));

 	return(loadprog(loadname, loadimage, 0, 0));
#else 
	return(0);
#endif
    }
    else {
        xm_flush_rx_console();
	if (error_msg >= 0)
	    puts(error_messages[error_msg]);
	return(1);
    }
}

void usage (char *arg)
{
    printf("usage: %s [-%s] <destination filename> \n",arg,supported_optstr);
    puts("-c  CRC-16 \n");
    puts("-y  ymodem-batch protocol \n");
#if defined(MARS)
    puts("-r  copy image to dram for launch \n");
    puts("-x  do not launch on download completion \n");
#endif
    return;
}
    
int receive ()
{
    int retval;

    /* pause for sender to start */

    sleep(30000); 

    /* start the transfer */
    firstblock = 1;
    recv_filesize = ymodem_filesize = 0;	
    mem_ptr = image_load_start_ptr;
 
    /* flush the console to discard chars from startup wait */
    xm_flush_rx_console();
 
    if(ymdmbat) {
	if(getblockzero() == -1) {
	    xm_flush_rx_console();
	    error_msg = HDR_TIMEOUT;
	    abort();
	    return(-1);
	}
	if(checkheader() == -1) {
	    error_msg = SENDER_ABORT; 
	    return(-1);
	}
    }

    retval = getfile();

    if (ymdmbat && (retval == 0))
	getlastblock();

    xm_flush_rx_console();
    return(retval);
}

int getfile ()
{

    int blocknum, currentblock, prev_err_block;
    long curr_blocksize;
    int successive_sync_errors = 0;
    ulong *mptr;
    uchar *block_ptr;
    int image_validated;

    firstblock = 1; 
    blocknum = 0;
    currentblock = 0;
    prev_err_block = 0;
    image_validated = 0;


    mem_ptr = image_load_start_ptr;
    block_ptr = mem_ptr;

    while(1) {
	mem_ptr = block_ptr;

	currentblock = getnextblock(&curr_blocksize);

	if(currentblock == ((blocknum +1) & 0xff)) {   /* In sequence */

	    blocknum++;
	    recv_filesize += curr_blocksize;

            if ((currentblock == 1 && firstblock) || !image_validated) {
	        firstblock = 0;

                /*
                 * Do checks to see if image is valid or not
                 */
                if ((image_validated = xmodem_valid_image(image_load_start_ptr,                  &expect_filesize, recv_filesize)) < 0) {    
		    abort();
                    return (-1);
		}
	    }
	    block_ptr = mem_ptr;
	    prev_err_block = successive_sync_errors = 0;
	    conout(ACK);
	}
	else if(currentblock == (blocknum & 0xff)) {  /* duplicate
							 block */
	    if (!ymdmbat && (currentblock == 0)) {
               error_msg = SENDER_PROTO_MISMATCH;
	       conout(NAK);
	       xm_flush_rx_console();
               abort();
               return (-1);
            }
	    prev_err_block = successive_sync_errors = 0;
	    conout(ACK);
	}
	else if (currentblock == SEENEOT) {       /* End of transfer */
	    /*
	     * Did we receive a truncated file?
	     */
	    if (expect_filesize && (recv_filesize < expect_filesize)) {
		abort();
	        error_msg = TRUNCATED_IMAGE;
		return(-1);
	    }
	    conout(ACK);
	    return(0);
	}
	else if(currentblock == TIMEOUT) {      /* timeout */
	    error_msg = DATA_TIMEOUT;
	    return(-1);
	}
	else if(currentblock == GOT_ABORT) {     /* sender aborted */
	    error_msg = SENDER_ABORT;
	    return(-1);
	}
	else  {
	    if(currentblock == prev_err_block) {
		successive_sync_errors++;
	    }
	    prev_err_block = currentblock;
	    conout(NAK);
	    if(successive_sync_errors == RETRYMAX) {
	        xm_flush_rx_console();
		abort();
		error_msg = XMODEM_SYNC_ERROR;
		return(-1);
	    }
	}
    }
	
}

int getnextblock (long *blocksize)
{
    int ch, currentblock, blocklength;
    int cksum1, cksum2;
    int trys, blocknum_complmnt, i;
    char prevchar;
    ushort crc;
    int checksum, normalexit;
    uchar *start_ptr, *tmp_ptr;

    trys = 0;
    prevchar = 0;
    normalexit = 1;
    currentblock = 0;
    blocknum_complmnt = 0;

    start_ptr = mem_ptr;
    while(trys++ < RETRYMAX) {

	mem_ptr = start_ptr; 

	if(firstblock) {
	    if(crcmode)
		conout(CRCCHR);
	    else
		conout(NAK);

	}
	if((trys > 1) && (!firstblock)) {
	    conout(NAK);
	}
	ch = xm_getchar(tensecs);

	if((ch == STX) || (ch == SOH)) {
	    if(ch == STX)
		blocklength = BBUFSIZ;
	    else if(ch == SOH) 
		blocklength = SBUFSIZ;
	    prevchar = 0;
	    if ((currentblock = xm_getchar(loop_for_onesec)) < 0) {
   		xm_flush_rx_console();
		continue;
	    }
	    if((blocknum_complmnt = xm_getchar(loop_for_onesec)) < 0) {
		xm_flush_rx_console();
		continue;
	    }
 	    if((currentblock+blocknum_complmnt) == 0xff) {
		crc = checksum = 0;
		for(i = blocklength; i > 0; --i) {
		    if((ch = xm_getchar(loop_for_onesec)) < 0) {
			normalexit = 0;
			break;
		    }
		    *mem_ptr++ = ch;
		}
		if(normalexit == 0) {
		    normalexit = 1;
		    xm_flush_rx_console();
		    continue;
		}

		if ((cksum1 = xm_getchar(loop_for_onesec)) < 0) {
                    /* no crc, or checksum */
                    xm_flush_rx_console();
                    continue;
                }

		if (crcmode)
                    if((cksum2 = xm_getchar(loop_for_onesec)) < 0) {
                        xm_flush_rx_console();
                        continue;
                    }

		/* Calculate the block checksum */
		for (tmp_ptr = start_ptr; tmp_ptr < mem_ptr; tmp_ptr++) {
		     crc = CRC16(*tmp_ptr, crc);
		     checksum += *tmp_ptr;
		}

		if(crcmode) {
		    crc = CRC16(cksum1, crc);
		    crc = CRC16(cksum2, crc);
	    
		    if(crc & 0xffff) {
			xm_flush_rx_console();
		    }
		    else {
			*blocksize = blocklength;
			return(currentblock);
		    }
		}
		else if(((checksum - cksum1) & 0xff) == 0) {
		    *blocksize = blocklength;
		    return(currentblock);
		}
		else {
		    xm_flush_rx_console();
		}
	    }
	    else {
		xm_flush_rx_console(); 
	    }
	}
	else if(ch == EOT) {
	    if (prevchar == EOT)
		return(SEENEOT);
	    else {
	      prevchar = EOT;
	      xm_flush_rx_console();
	    }
	}  
	else if(ch == CAN) {
	    if(prevchar == CAN)
		return(GOT_ABORT);
	    else {
		prevchar = CAN;
	    }
	}
	else if(ch == TIMEOUT) {
	    prevchar = 0;
	    xm_flush_rx_console();
	}	    
	else {
	    prevchar = 0;
	    xm_flush_rx_console();
	}

    }
    xm_flush_rx_console();   
    abort();
    return(TIMEOUT);
}


void abort (void)
{
    /* two CANs are enough. Send more anyway in case they are lost */
    
    conout(CAN);
    conout(CAN);
    conout(CAN);
    conout(CAN);
    return;
}


int checkheader ()
{
    uchar *tmp;

    tmp = image_load_start_ptr;

    /* Check file name first */ 
    if (!strlen(tmp))
	return (-1);


    tmp += (strlen(tmp) + 1); 	
    /* Since filesize is optional, don't perform any checks */
    getnum((char *) tmp, 10, &ymodem_filesize, 0); 
    return(1);
}

int getblockzero ()
{
    int ch;
    long blocksize;

    if((ch = getnextblock(&blocksize)) != 0) {
	return(-1);
    }
    conout(ACK);
    return(1);
	
}

void getlastblock ()
{
    uchar *end_hdr;
    long blocksize;

    end_hdr = mem_ptr;
    conout(CRCCHR); 
   
    if (getnextblock(&blocksize) == 0) {
	if (!strlen(end_hdr)) {
	    /* No file name specified - end of transfer */
	    conout(ACK);
	    return;
	}
    }

    /* Either we received a non header block or a non null   */
    /* filename was specified  - send an abort to the server */
    abort();
}
 
static int xm_getchar (int waitfor)
{

    do {
	if(constat()) return(conin());
    } while(waitfor--);
    return(TIMEOUT);
}

static void xm_flush_rx_console ()
{
    /* Flush the receive stream */
    while (xm_getchar(loop_for_onesec) != TIMEOUT) ;
}

int disp_flash ()
{

    int fd;
    char buffer[80];
    
    
    sprintf(buffer, "%s.", flash_dev_name);

    if((fd = open(buffer, O_RDONLY)) < 0) {
        printf("dir: cannot open device \"%s\"\n", flash_dev_name);
        return(1);
    }
    
    /* Opened device is closed within this routine */

    return(print_dir_info(fd));
}

void flash_ffh_checksum ()
{
    
    int i, nbytes;
    ulong chksum;
    ushort *start;

    start = (ushort *)(image_load_start_ptr);
    nbytes = ffh.ffh_len;
    chksum = 0;
    chksum = fchksum_body(chksum, start, nbytes);

    ffh.ffh_checksum = ~chksum;  /* ones compliment */
}

boolean xmodem_flash_image_copy (char *filename)
{
        flash_fhdr_t *mem_fhdr;
	int size;

        /* Compute the checksum for the file and fill the fields
         * for the flash file header
         */

	size = (expect_filesize ? expect_filesize : recv_filesize);

        ffh.ffh_checksum = 0;
        ffh.ffh_magic = FLASH_FHDR_MAGIC;
        ffh.ffh_len = size;
        ffh.ffh_flags = -1;
        ffh.ffh_addr = (uchar *)FLASH_FHDR_SIZE;
        /* fill in name field */
        strncpy(ffh.ffh_name, filename, sizeof(ffh.ffh_name));
        /* make sure its terminated */
        ffh.ffh_name[sizeof(ffh.ffh_name)] = '\0';

        flash_ffh_checksum();

        /* Write the flash header at before the start of the image */
	mem_fhdr = (flash_fhdr_t *) (image_load_start_ptr - FLASH_FHDR_SIZE);

        *mem_fhdr = ffh;

        /* We have the image in DRAM. Now, erase bootflash and program it
         * with the downloaded image
         */
	if (!xmodem_flash_erase())
	    return (FALSE);
	puts("\n");
	if (!xmodem_load_flash((unsigned long) (image_load_start_ptr - FLASH_FHDR_SIZE), 
			  	(size + FLASH_FHDR_SIZE)))
	    return (FALSE);

  	return (TRUE); 
} 
   





