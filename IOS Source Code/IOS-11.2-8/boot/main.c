/* $Id: main.c,v 3.2 1995/11/17 07:41:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/main.c,v $
 *------------------------------------------------------------------
 * main test driver for the uncompress module
 *
 * August 1987, Greg Satz
 *
 * Copyright (c) 1987-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: main.c,v $
 * Revision 3.2  1995/11/17  07:41:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../sys/h/hp_a.out.h"
#include "prom2.h"

#define	HASHSIZE	69001
#define	MAXBITS		16
#define	COMPRESSMEM	(HASHSIZE*sizeof(u_short)+(1<<MAXBITS)+8000)

int memsize = 1024 * 1024;
char buf2[BUFSIZ];

main(argc, argv)
    int argc;
    char **argv;
{
    int fd, size;
    char *inp, *outp;
    struct stat statb;
    struct cdata *data;
    struct cload *load;

    if (argc != 2) {
	fprintf(stderr, "usage: main <file>\n");
	exit(1);
    }
    fd = open(argv[1], 0);
    if (fd < 0) {
	perror(argv[1]);
	exit(1);
    }
    if (stat(argv[1], &statb) < 0) {
	perror("stat");
	exit(1);
    }
    setlinebuf(stderr);
    outp = (char *)malloc(2 * statb.st_size);
    if (outp == NULL) {
	fprintf(stderr, "malloc failed getting output buffer\n");
	exit(1);
    }
    inp = (char *)malloc(statb.st_size + COMPRESSMEM);
    if (inp == NULL) {
	fprintf(stderr, "malloc failed getting input buffer\n");
	exit(1);
    }
    if (read(fd, inp, statb.st_size) != statb.st_size) {
	fprintf(stderr, "couldn't read all of %s\n", argv[1]);
	exit(1);
    }
    close(fd);
    inp += sizeof(struct exec);
    data = (struct cdata *)inp;
    load = (struct cload *)(data + 1);
    load->addr = (u_long)load + data->number * sizeof(struct cload);
    if (data->number > 1) {
	struct cload *load2 = load + 1;

	load2->addr = load->addr + load->len;
    }
    if (!(size = uncompress(inp, outp))) {
	fprintf(stderr, "uncompress failed!\n");
	exit(1);
    }
    if (islower(argv[1][0]))
	argv[1][0] = toupper(argv[1][0]);
    else if (isupper(argv[1][0]))
	argv[1][0] = tolower(argv[1][0]);
    else {
	fprintf(stderr, "couldn't create output filename from %s\n", argv[1]);
	exit(1);
    }
    fd = creat(argv[1], 0666);
    if (fd < 0) {
	perror(argv[1]);
	exit(1);
    }
    write(fd, outp, size);
    close(fd);
    exit(0);
}

bcopy(src, dst, cnt)
    register u_char *src, *dst;
    register int cnt;
{
    while(cnt--)
	*dst++ = *src++;
}
