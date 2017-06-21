/* $Id: image_stamp.c,v 3.1.64.1 1996/04/23 00:09:14 dcoli Exp $
 * $Source: /release/112/cvs/Xsys/scripts/image_stamp.c,v $
 *------------------------------------------------------------------
 * image_stamp.c -- stamp A.out header with cputype info
 *
 * September 1995, Eric Fronberg
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: image_stamp.c,v $
 * Revision 3.1.64.1  1996/04/23  00:09:14  dcoli
 * CSCdi44581:  Warn user when copying enterprise image to ciscoPro router
 * flash
 * Branch: California_branch
 * Stamp a.out header with a bit identifying the image as a ciscoPro
 * image.
 *
 * Revision 3.1  1995/11/09  13:10:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/23  16:24:20  enf
 * CSCdi40950:  CSCdi40338 on 11.0
 *
 * Revision 2.2  1995/09/23  05:58:06  enf
 * CSCdi39189:  100X need insurance against loading incorrect image
 *
 * Revision 2.1  1995/09/23  05:36:34  enf
 * placeholder for CSCdi39189
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* file to stamp into a.out hdr info on what 
 * platform the image should run on.
 */

#include <stdio.h>
#include "../os/reloc.h"
#include "../h/cisco_a.out.h"

char *invocation_name;

void usage()
{
    printf("\nusage: %s -f family_val -m magic_val [-c ciscopro_flag] [-i interface_val] filename\n",invocation_name);

}    
/* 
 * parse_hex
 * get a hex value from a string 
 */

unsigned long parse_hex (char *string)
{
    int i;
    unsigned long result;

    /* 
     * skip over any leading '0x' 
     */
    if (string && string[0] == '0' && 
	(string[1] == 'x' || string[1] == 'X')) {
	string +=2;
    }

    if (sscanf(string, "%x", &result) != 1) {
	usage();
	printf("Invalid parameter - %s\n", string);
	exit(1);
    }
    return(result);
}

main(int argc, char *argv[])
{
    
    FILE *fd;
    int family = 0;
    int interface = 0;
    int magic = 0;
    struct exec aout;
    struct family_check *fc;
    int ciscopro_flag = 0; /* ciscopro_flag not required and defaults to 0  */
                           /* (false) because some platforms do not support */
                           /* ciscoPro.                                     */

    invocation_name = argv[0];
    argv++;
    /* parse options on command line */
    
    while (*argv && argv[0][0] == '-') {
	char flag;
	flag = argv[0][1];
	argv++;

	switch (flag) {
	  case 'F':		/* family */
	  case 'f':
	    family = parse_hex(argv[0]);
	    break;
	  case 'i':		/* interface */
	  case 'I':
	    interface = parse_hex(argv[0]);
	    break;
	  case 'm':		/* magic */
	  case 'M':
	    magic = parse_hex(argv[0]);
	    break;
	  case 'c':		/* ciscopro_flag */
	  case 'C':
	    ciscopro_flag = parse_hex(argv[0]);
	    break;
	  default:
	    usage();
	    exit(1);
	}

	argv++;
    }
    /* check to see if  -f, -m flags used */
    if ( !family || !magic) {
	usage();
	printf("-m and -f flags required!\n");
	exit(1);
    }
    /* argv should now point to file to open */
    if (!*argv) {
	usage();
	printf("Filename not specified\n");
	exit(1);
    }
    fd = fopen(*argv,"r+");
    if (!fd) {
	perror(*argv);
	exit(1);
    }
    /* put us at the beginning of the file */
    fseek(fd, 0L, 0);
    /* get a.out header of file */
    fread(&aout, sizeof(aout), 1, fd);
    if (aout.a_machtype != CISCOTYPE || aout.a_magic != CISCOMAGIC) {
	printf("File %s does not have a valid a.out header\n", *argv);
	exit(1);
    }
    if (!aout.a_trsize) {
	fc = (struct family_check *)&aout.a_trsize ;
	fc->magic = magic;
	fc->family = family;
	fc->interface = interface;
	if (ciscopro_flag)
	    fc->flags |= CISCOPRO_IMAGE_FLAG;
	else
	    fc->flags &= ~CISCOPRO_IMAGE_FLAG;
	fseek(fd, 0L, 0);
	fwrite(&aout, sizeof(aout), 1, fd);
    } else {
	printf("%s: a.out header already stamped!\n", *argv);
	exit(1);
    }
    fclose(fd);
    exit(0);
}
