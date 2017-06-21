/* $Id: glue_aout.c,v 3.2.60.1 1996/06/28 23:05:23 hampton Exp $
 * $Source: /release/112/cvs/Xsys/czip/glue_aout.c,v $
 *------------------------------------------------------------------
 * glue_aout.c - glue a zipped router image to the extractor
 *
 * December 1994, Michael Beesley
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: glue_aout.c,v $
 * Revision 3.2.60.1  1996/06/28  23:05:23  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:58:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:22:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../h/cisco_a.out.h"
#include "../os/sum_magic_number.h"

#define ZIP_COMMAND     "zip"
#define UNZIP_COMMAND	"unzip"

/*
 * Globals
 */
extern char *optarg;
extern int optind;

char *router_name = (char *)0;
char *extract_name = (char *)0;
char *output_name = (char *)0;
int unzip = 0;
unsigned long magic_variable = MAGIC_VALUE;

/*
 * for debugging set to 1
 */
int debug = 0;

/*
 * usage: 
 * Print out usage message
 */ 
void usage (char *name)
{
    fprintf(stderr, "Usage: %s [-arguments]\n", name);
    fprintf(stderr, "\t -r <input router image>\n");
    fprintf(stderr, "\t -z <self-extractor>\n");
    fprintf(stderr, "\t -o <output filename>\n");
    fprintf(stderr, "\t -u <unzip the image>\n");
    fprintf(stderr, "\nglue_aout -r xx-k-m -z czip_aout -o xx-k-mz");
    fprintf(stderr, "\nglue_aout -u -r xx-k-mz -o xx-k-mz\n");
    exit(1);
}

/*
 * command_line :
 * Parse command line arguments
 */
int command_line (int argc, char **argv) 
{
    int c, cnt;

    cnt = 0;
    while ((c = getopt(argc, argv, "ur:z:o:")) != -1) {
	switch (c) {
	case 'u': unzip = 1; break;
	case 'r': router_name = optarg; cnt++; break;
	case 'z': extract_name = optarg; cnt++; break;
	case 'o': output_name = optarg; cnt++; break;
	default : usage(argv[0]); break;
        }
    }
}
 
/*
 * unzip_image : Unzip an image
 * Take router_name as input (zipped file)
 * Take output_name as output file
 */
int unzip_image (void)
{
    FILE 	      	*router_file, *outtemp;
    struct exec		*hd;
    struct stat 	stat_buf;
    int			rfd, bytes, outfd, nwords, i;
    unsigned long	*ptr, *current, sum, rimage_size, rcomp_size,
    			rsum, usum, router_size;
    char 		*router_image, cmd_buf[80], outfile_temp[80];

    /* Check input arguments */
    if (!router_name || !output_name) {
	return(0);
    }

    if (debug) {
	fprintf(stderr, "router_name       : %s\n", router_name);
	fprintf(stderr, "output_name       : %s\n", output_name);
    }

    /*
     * Create the temp file
     */
    sprintf(outfile_temp, "%s.zipped", output_name);

    /*
     * Open the zipped archive (router_name)
     */
    router_file = fopen(router_name, "r");
    if (!router_file) {
	fprintf(stderr, "Error : failed to open %s for reading\n", 
		router_name);
	exit(1);
    }

    /*
     * Get the input file size
     */
    rfd = fileno(router_file);
    fstat(rfd, &stat_buf);
    router_size = stat_buf.st_size;
    if (router_size <= 0) {
	fprintf(stderr, "Error : input file size is %d\n",
		router_size);
	exit(1);
    }
    if (debug) {
	fprintf(stderr, "router size       : 0x%x bytes\n", router_size);
    }

    /*
     * Read the complete input file
     */
    router_image = malloc(router_size);
    if (!router_image) {
	fprintf(stderr, "Error : failed to malloc space\n");
	exit(1);
    }
    bytes = read(rfd, router_image, router_size);
    if (bytes != router_size) {
	fprintf(stderr, "Error : failed to read complete file, %s\n",
		router_name);
	exit(1);
    }

    /*
     * Check that it is a.out executable
     */
    hd = (struct exec *)router_image;
    if (debug) {
	fprintf(stderr, "hd->a_dynamic     : %d\n", hd->a_dynamic);
	fprintf(stderr, "hd->a_toolversion : %d\n", hd->a_toolversion);
	fprintf(stderr, "hd->a_machtype    : %d\n", hd->a_machtype);
	fprintf(stderr, "hd->a_magic       : %d\n", hd->a_magic);
	fprintf(stderr, "hd->a_text        : %d\n", hd->a_text);
	fprintf(stderr, "hd->a_data        : %d\n", hd->a_data);
	fprintf(stderr, "hd->a_bss         : %d\n", hd->a_bss);
	fprintf(stderr, "hd->a_syms        : %d\n", hd->a_syms);
	fprintf(stderr, "hd->a_entry       : %d\n", hd->a_entry);
	fprintf(stderr, "hd->a_trsize      : %d\n", hd->a_trsize);
	fprintf(stderr, "hd->a_drsize      : %d\n", hd->a_drsize);
    }
    if (hd->a_magic != CISCOMAGIC) {
	fprintf(stderr, "Error : file %s does not have an a.out header\n",
		router_name);
	exit(1);
    }

    /*
     * Calculate pointer to magic stuff
     */
    ptr = (unsigned long *)((unsigned long)router_image + 
			    sizeof(struct exec) + hd->a_text);
    for (i=0, bytes=0; i < hd->a_data; i += 4) {
	if (*ptr == magic_variable) {
	    if (debug) {
		fprintf(stderr, "magic number      : 0x%x\n", *ptr);
	    }
	    bytes = 1;
	    ptr += 1;
	    break;
	}
	ptr += 1;
    }
    if (!bytes) {
	fprintf(stderr, "Error : failed to find magic variable in %s\n",
		router_name);
	exit(1);
    }

    /*
     * Read in other values
     */
    rimage_size = *ptr++;
    rcomp_size = *ptr++;
    rsum = *ptr++;
    usum = *ptr++;

    /*
     * Ensure compressed checksum matches
     */
    nwords = rcomp_size / sizeof(unsigned long);
    current = ptr;
    sum = 0;
    while (nwords--) {
	ADD_AROUND(*current++, sum);
    }
    if (sum != rsum) {
	fprintf(stderr, "Error : compressed checksum does not agree with\n",
		        "        calculated value for %s\n", router_name);
	exit(1);
    }

    /*
     * Write the image out
     */
    outtemp = fopen(outfile_temp, "w");
    if (!outtemp) {
	fprintf(stderr, "Error : failed to open %s for writing\n",
		outfile_temp);
	exit(1);
    }
    outfd = fileno(outtemp);
    bytes = write(outfd, (void *)ptr, rcomp_size);
    if (bytes != rcomp_size) {
	fprintf(stderr, "Error : failed to write %s\n", outfile_temp);
	exit(1);
    }

    /*
     * Close the file and unzip it
     */
    fclose(outtemp);
    sprintf(cmd_buf, "%s -p %s > %s", UNZIP_COMMAND, outfile_temp, 
	    output_name);
    printf("Unzipping %s to %s\n", router_name, output_name);
    bytes = system(cmd_buf);
    if (bytes != 0) {
	fprintf(stderr, "Error : system call to %s failed\n", 
		UNZIP_COMMAND);
	exit(1);
    }

    /*
     * Remove the temp file
     */
    bytes = remove(outfile_temp);
    if (bytes != 0) {
	fprintf(stderr, "Error : failed to remove %s\n", outfile_temp);
	exit(1);
    }

    /* All done */
    return(1);
}

/*
 * main: 
 * zip up the router executable, checksum it and glue
 * it together with the unzip executable to make an a.out
 *
 * Or if -u is on the commmand line unzip it.
 */
int main (int argc, char *argv[])
{
    int	      		infd, exfd, outfd, nwords;
    FILE		*router_file, *extract_file, *output_file;
    struct exec 	hd;
    struct stat 	stat_buf;
    unsigned long	router_image_size, router_comp_size, bytes;
    unsigned long	sum, unsum, exf_data_size, *current;
    unsigned char	*comp_image, *exf_data, *uncomp_image;
    char 		cmd_buf[80], router_comp_name[80];
    int			temp, i;

    command_line(argc, argv);
    if (unzip) {
	if (!unzip_image()) {
	    usage(argv[0]);
	} else {
	    exit(0);
	}
    } else {
	if (!router_name || !extract_name || !output_name) {
	    usage(argv[0]);
	    exit(1);
	}
    }
    
    if (debug) {
	fprintf(stderr, "router_name    : %s\n", router_name);
	fprintf(stderr, "extract_name   : %s\n", extract_name);
	fprintf(stderr, "output_name    : %s\n", output_name);
    }

    /*
     * Open the router input file
     */
    router_file = fopen(router_name, "r");
    if (!router_file) {
	fprintf(stderr, "Error : failed to open %s for reading\n", 
		router_name);
	exit(1);
    }

    /*
     * Get the router input file size
     */
    infd = fileno(router_file);
    fstat(infd, &stat_buf);
    router_image_size = stat_buf.st_size;
    if (router_image_size <= 0) {
	fprintf(stderr, "Error : router input file size is %d\n",
		router_image_size);
	exit(1);
    }
    if (debug) {
	fprintf(stderr, "Input router image size : 0x%x bytes\n",
		router_image_size);
    }

    /*
     * Checksum the uncompressed router image
     */
    uncomp_image = malloc(router_image_size);
    if (!uncomp_image) {
	fprintf(stderr, "Error : failed to malloc space\n");
	exit(1);
    }
    bytes = read(infd, uncomp_image, router_image_size);
    if (bytes != router_image_size) {
	fprintf(stderr, "Error : failed to read uncompressed router image\n");
	exit(1);
    }
    fclose(router_file);

    nwords = router_image_size / sizeof(unsigned long);
    unsum = 0;
    current = (unsigned long *)uncomp_image;
    if (debug) {
	fprintf(stderr, "Checksumming 0x%x words\n", nwords);
    }
    while (nwords--) {
	ADD_AROUND(*current++, unsum);
    }

    printf("Checksum of uncompressed image is 0x%x\n", unsum);

    /*
     * Call the compressor to compress up the image (always zip)
     */
    sprintf(cmd_buf, "%s -q -k %s.gz %s",
	    ZIP_COMMAND, router_name, router_name);
    printf("Compressing %s using %s\n", router_name, ZIP_COMMAND);
    temp = system(cmd_buf);
    if (temp != 0) {
	fprintf(stderr, "Error : system call to %s failed\n",
		ZIP_COMMAND);
	exit(1);
    }

    /*
     * Make the compressed file name and open it
     */
    sprintf(router_comp_name, "%s.gz", router_name);
    router_file = fopen(router_comp_name, "r");
    if (!router_file) {
	fprintf(stderr, "Error : failed to open %s for reading\n",
		router_comp_name);
	exit(1);
    }
    infd = fileno(router_file);
    fstat(infd, &stat_buf);
    router_comp_size = stat_buf.st_size;
    if (router_comp_size <= 0) {
	fprintf(stderr, "Error : compressed file size is %d\n",
		router_comp_size);
	exit(1);
    }
    if (router_comp_size >= router_image_size) {
	fprintf(stderr, "Error : compressed size > uncompressed size\n");
	exit(1);
    }
    if (debug) {
	fprintf(stderr, "Compressed router image size is 0x%x bytes\n",
		router_comp_size);
    }

    /*
     * Read the compressed file 
     */
    comp_image = malloc(router_comp_size);
    if (!comp_image) {
	fprintf(stderr, "Error : failed to malloc space\n");
	exit(1);
    }
    bytes = read(infd, comp_image, router_comp_size);
    if (bytes != router_comp_size) {
	fprintf(stderr, "Error : failed to read compressed router image\n");
	exit(1);
    }

    /*
     * Checksum the compressed router
     */
    nwords = router_comp_size / sizeof(unsigned long);
    sum = 0;
    current = (unsigned long *)comp_image;
    if (debug) {
	fprintf(stderr, "Checksumming 0x%x words\n", nwords);
    }
    while (nwords--) {
	ADD_AROUND(*current++, sum);
    }

    printf("Checksum of compressed image is 0x%x\n", sum);

    /*
     * Open the extractor a.out and read a.out header
     */
    extract_file = fopen(extract_name, "r");
    if (!extract_file) {
	fprintf(stderr, "Error : failed to open %s\n",
		extract_name);
    }
    exfd = fileno(extract_file);
    bytes = read(exfd, (void *)&hd, sizeof(struct exec));
    if (bytes != sizeof(struct exec)) {
	fprintf(stderr, "Error : failed to read %s's a.out header\n",
		extract_file);
	exit(1);
    }
	
    if (debug) {
	fprintf(stderr, "extract_header.a_dynamic     : %d\n", hd.a_dynamic);
	fprintf(stderr, "extract_header.a_toolversion : %d\n", 
		hd.a_toolversion);
	fprintf(stderr, "extract_header.a_machtype    : %d\n", hd.a_machtype);
	fprintf(stderr, "extract_header.a_magic       : %d\n", hd.a_magic);
	fprintf(stderr, "extract_header.a_text        : %d\n", hd.a_text);
	fprintf(stderr, "extract_header.a_data        : %d\n", hd.a_data);
	fprintf(stderr, "extract_header.a_bss         : %d\n", hd.a_bss);
	fprintf(stderr, "extract_header.a_syms        : %d\n", hd.a_syms);
	fprintf(stderr, "extract_header.a_entry       : %d\n", hd.a_entry);
	fprintf(stderr, "extract_header.a_trsize      : %d\n", hd.a_trsize);
	fprintf(stderr, "extract_header.a_drsize      : %d\n", hd.a_drsize);
    }

    /*
     * Adjust the fields in the header
     */
    exf_data_size = hd.a_data;
    hd.a_data = hd.a_data + router_comp_size + (sizeof(unsigned long) * 5);
    
    /*
     * Open output file
     */
    output_file = fopen(output_name, "w");
    if (!output_file) {
	fprintf(stderr, "Error : could not open %s for writing\n",
		output_name);
	exit(1);
    }
    outfd = fileno(output_file);
    
    /*
     * Write new aout header
     */
    bytes = write(outfd, (void *)&hd, sizeof(struct exec));
    if (bytes != sizeof(struct exec)) {
	fprintf(stderr, "Error : failed to write new exec\n");
	exit(1);
    }

    /*
     * Write extractor text and data segment
     */
    exf_data = malloc(hd.a_text + exf_data_size);
    if (!exf_data) {
	fprintf(stderr, "Error : failed malloc\n");
	exit(1);
    }
    bytes = read(exfd, exf_data, hd.a_text + exf_data_size);
    if (bytes != hd.a_text + exf_data_size) {
	fprintf(stderr, "Error : failed to read %s\n", extract_name);
	exit(1);
    }
    bytes = write(outfd, exf_data, hd.a_text + exf_data_size);
    if (bytes != hd.a_text + exf_data_size) {
	fprintf(stderr, "Error : failed to write %s\n", output_name);
	exit(1);
    }

    /*
     * Write magic numbers and variables
     */
    bytes = write(outfd, &magic_variable, sizeof(unsigned long));
    if (bytes != sizeof(unsigned long)) {
	fprintf(stderr, "Error : failed to write magic_variable\n");
	exit(1);
    }
    bytes = write(outfd, &router_image_size, sizeof(unsigned long));
    if (bytes != sizeof(unsigned long)) {
	fprintf(stderr, "Error : failed to write router_image_size\n");
	exit(1);
    }
    bytes = write(outfd, &router_comp_size, sizeof(unsigned long));
    if (bytes != sizeof(unsigned long)) {
	fprintf(stderr, "Error : failed to write router_comp_size\n");
	exit(1);
    }
    bytes = write(outfd, &sum, sizeof(unsigned long));
    if (bytes != sizeof(unsigned long)) {
	fprintf(stderr, "Error : failed to write compressed sum\n");
	exit(1);
    }
    bytes = write(outfd, &unsum, sizeof(unsigned long));
    if (bytes != sizeof(unsigned long)) {
	fprintf(stderr, "Error : failed to write uncompressed sum\n");
	exit(1);
    }

    /*
     * Write the complete compressed image to output
     */
    bytes = write(outfd, comp_image, router_comp_size);
    if (bytes != router_comp_size) {
	fprintf(stderr, "Error : failed to write compressed image\n");
	exit(1);
    }

    printf("Glued %s and %s to form %s\n",
	   router_name, extract_name, output_name);
    exit(0);
}

/* end of file */
