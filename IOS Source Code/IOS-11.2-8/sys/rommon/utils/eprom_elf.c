/* $Id: eprom_elf.c,v 3.2 1995/11/17 18:48:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/utils/eprom_elf.c,v $
 *------------------------------------------------------------------
 * eprom_elf.c
 *
 * January 1994, Michael Beesley
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * ELF file checksumming program.
 *------------------------------------------------------------------
 * $Log: eprom_elf.c,v $
 * Revision 3.2  1995/11/17  18:48:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:10:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:42:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* 
** purpose : To calculate the ERPOM checksum value and put it back to 
**	     the proper location before any 's' file and 'pcode' file 
**	     are create. Total sum of EPROM data should be equal to zero.
** input   : 1) target binary file 
**           2) syms which contains symbols and addresses
** syntax  : sum <symbols file> <binary file>
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mon_elf.h"

#define GETCHECKSUM 1
#define GETETEXT    2

main(argc,argv)
    int  argc;
    char *argv[];
{
    FILE  *fp;
    char   buf[30];        /* buffer to store string */
    int    labelflags;     /* 0:label found 1:search for label */
    static char label1[] = "checksum";
    static char label2[] = "etext";

    Elf32_Phdr pgm_hdr;
    Elf32_Ehdr elf_hdr;
    long text_start;
    unsigned int i;
    unsigned int epromsize;
    unsigned int start_addr;
    unsigned int chksumloc;  /* checksum location */    
    unsigned int addr;       /* label address */
    unsigned short sum=0;
    unsigned short tempsum=0;

    printf("working on EPROM checksum ... ");

    if(argc!=3) {
	fprintf(stderr,"Usage : eprom_elf <symbols file> <target file>\n");
	exit(1);
    }
    
    /* open symbols file to search the address for label */ 
    if((fp = fopen(argv[1],"r")) == NULL) {
	fprintf(stderr, "Can't open %s \n",argv[1]);
	exit(2);
    }

    labelflags = GETCHECKSUM+GETETEXT;  /* initialize flags */
    (void)fscanf(fp,"%*s %*s");         /* strip off junk on first line */

    /* look for "checksum" address and size of eprom */
    while(!feof(fp)&&fscanf(fp,"%s %x",buf,&addr)==2){
	if(labelflags&GETCHECKSUM) {
	    if(!strcmp(buf,label1)) {
		chksumloc = addr;
		labelflags -= GETCHECKSUM;         /* found label1 */
	    }
	}
	if(labelflags&GETETEXT) {
	    if(!strcmp(buf,label2)) {
		epromsize = addr;
		labelflags -= GETETEXT;            /* found label2 */
	    }
	}
    }
    fclose(fp);

    if ( labelflags != 0 ) {
	fprintf(stderr, "Error finding labels, %d\n", labelflags);
	exit(9);
    }

    /* open code file to perform EPROM checksum     */
    /* rule : the total sum will store in checksum (2 bytes long) */
    if((fp = fopen(argv[2],"r+b")) == NULL) {
	fprintf(stderr, "Can't open %s \n",argv[2]);
	exit(3);
    }

    /*
     * Read the ELF header
     */
    if(fread((char *)(&elf_hdr),1,sizeof(elf_hdr),fp) !=
       sizeof(elf_hdr)) {
	fprintf(stderr, "Can't read %s's ELF header\n", argv[2]);
	exit(1);
    }

    if (elf_hdr.e_type != ET_EXEC) {
	fprintf(stderr, "Error, not an executable file\n");
	exit(1);
    }

    /*
     * Check for existance of program header
     */
    if (elf_hdr.e_phoff == 0 || elf_hdr.e_phnum == 0) {
	fprintf(stderr, "Error, no program header\n");
	exit(1);
    }

    /*
     * Read the program headers. Pick the one that has none-zero
     * file size
     */
    if (fseek(fp, elf_hdr.e_phoff, 0) != 0) {
	fprintf(stderr, "Error, failed to seek to program headers\n");
	exit(1);
    }

    for (i=0; i<elf_hdr.e_phnum; i++) {

	if (fread((char *)&pgm_hdr, 1, elf_hdr.e_phentsize, fp) != 
	    elf_hdr.e_phentsize) {
	    fprintf(stderr, "Error, failed to read program header %d\n", i);
	    exit(1);
	}

	if (pgm_hdr.p_filesz)
	    break;
    }

    if (i == elf_hdr.e_phnum) {
	fprintf(stderr, "Error, failed to find non-zero phdr\n");
	exit(1);
    }

    epromsize = epromsize - pgm_hdr.p_vaddr;
    chksumloc = chksumloc - pgm_hdr.p_vaddr;
    text_start = pgm_hdr.p_offset;

    if(fseek(fp,text_start,0) != 0) {
	fprintf(stderr, "%s : Error seeking to load block\n");
	exit(4);
    }
    for( i=0 ; i<epromsize ; i++ ) sum += getc(fp);

    /*
     * Seek to the data segment and install checksum
     */
    if(fseek(fp,(text_start+chksumloc),0) != 0) {
	fprintf(stderr, "%s : Error Seeking to checksum\n");
	exit(5);
    }
    putw(sum,fp);  /* store checksum value */
    rewind(fp);

    /* sanitary check */
    if(fseek(fp,text_start,0) != 0) {
	fprintf(stderr, "%s : Error Seeking.\n",argv[2]);
	exit(6);
    }    
    /* total sum */
    for( i=0 ; i<epromsize ; i++ ) tempsum += getc(fp);
    rewind(fp);

    if(fseek(fp,(text_start+chksumloc),0) != 0) {
	fprintf(stderr, "%s : Error Seeking to checksum\n");
	exit(7);
    } 

    if(tempsum!=sum) {
	fprintf(stderr,"Error on sanitary check.\n");
	exit(8);
    }

    fclose(fp);  /* done with input file - code */ 
    printf("done.\n");

    exit(0);
}






