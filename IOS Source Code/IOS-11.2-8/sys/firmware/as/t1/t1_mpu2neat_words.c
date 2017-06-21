/* $Id: mpu2neat_words.c,v 1.8.6.2 1997/02/21 07:41:06 jliu Exp $
 * $Source: /cscka/t1_neat/src/t1/mpu2neat_words.c,v $
 *------------------------------------------------------------------
 * The file utility converting S-Record file into C data array file in words.
 *
 * September 1996 - Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mpu2neat_words.c,v $
 * Revision 1.8.6.2  1997/02/21 07:41:06  jliu
 *    o Resolved CSCdi85835 bug and fix a problem in make.rule file
 *    o Resolved CSCdi85835 bug and fix a problem in make.rule file
 *
 * Revision 1.8.6.1  1997/01/17 23:50:59  jliu
 * 	o Fixed potential race condition between IOS and firmware
 *
 * Revision 1.8  1996/10/16 18:46:14  jchlin
 * CSCdi72629:
 * Loose the timing check for the ringing cadence and do the fast ringing
 * for loopstart and groundstart incoming calls to make the incoming
 * call setup time shorter.
 *
 * Revision 1.7  1996/10/10  23:46:12  jchlin
 * The Version 4 contains the following fixes for CSCdi71060:
 * o       Set the framer with default value of SF and AMI during the
 *         initialization of framer.
 * o       Enable the robbed bit interrupt mask in framer chip.
 * o       Change the timer timeout process so that it can process
 *         the timeout timer in the iteration of certain number
 *         before it releases CPU control.
 *
 * Revision 1.6  1996/09/25  19:04:51  jliu
 *   o updated the firmware version to 3 for CSCdi69976 bug fix
 *
 * Revision 1.5  1996/09/10  02:55:00  jchlin
 * Change the firmware version number and comments from 1 to 2.
 *
 * Revision 1.4  1996/09/08  23:13:44  jchlin
 * o       Use const for every variables and array of firmware to save
 *         space in the system flash memory. See CSCdi67027.
 * o       Add one more firmware comment description in the firmware file.
 *
 * Revision 1.3  1996/09/08  01:21:19  jchlin
 * o       Add the CVS Log Header at the beginning of file.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define NEAT_VERSION_1_DESC " * initial version with Framer, FDL, and RBS/E&M\n"
#define NEAT_VERSION_2_DESC " * Framer, FDL, and RBS/E&M/Loopstart/Groundstart/busyout-dsx0.\n"
#define NEAT_VERSION_3_DESC " * includes CSCdi69976 bug fix \n" 
#define NEAT_VERSION_4_DESC " * includes CSCdi71060 bug fix \n" 
#define NEAT_VERSION_5_DESC " * includes CSCdi72629 bug fix \n" 
#define NEAT_VERSION_6_DESC " * includes CSCdi77222 bug fix \n"
#define NEAT_VERSION_7_DESC " * includes CSCdi85835 bug fix \n"
#define NEAT_VERSION_8_DESC " * includes CSCdi85287, CSCdi91038, CSCdi92824,\n"\
                            " *          CSCdj00011 bug fixes\n"
#define NEAT_VERSION_9_DESC " * includes CSCdj09779 bug fix \n"

#define NEAT_VERSION     9

#define MAXINLINE       60
#define MAXOUTLINE      80
#define TRUE            1
#define FALSE           0
#define BLOCKSIZE       90000

static char block[BLOCKSIZE];
int fw_size;
/*
 * start_output : Initialize the output file
 */
void start_output (FILE *f)
{
    fprintf(f, "/*\n * NEAT T1 Firmware, Version %d\n", NEAT_VERSION);
    fprintf(f, "%s", NEAT_VERSION_2_DESC);
    fprintf(f, "%s", NEAT_VERSION_3_DESC);
    fprintf(f, "%s", NEAT_VERSION_4_DESC);
    fprintf(f, "%s", NEAT_VERSION_5_DESC);
    fprintf(f, "%s", NEAT_VERSION_6_DESC);
    fprintf(f, "%s", NEAT_VERSION_7_DESC);
    fprintf(f, "%s", NEAT_VERSION_8_DESC);
    fprintf(f, "%s", NEAT_VERSION_9_DESC);
    fprintf(f, " */\n\n");

    /*
     * Output the verson string
     */
    fprintf(f, "const short t1_version = %d"
            ";\n\n", NEAT_VERSION);
    fprintf(f, "/*\n * NEAT T1 Firmware\n */\n");
    fprintf(f, "const unsigned short t1_firmware[] = {\n");
}

void convert (FILE *outf)
{
    int i, j, line_size = 20, index = 0;
    char outline[MAXOUTLINE];
    int size = fw_size;

    while (size > 0) {
        for (i = index, j =0; i < line_size + index; ) {

                 outline[j++] = ' ';
                 outline[j++] = ' ';
                 outline[j++] = ' ';
                 outline[j++] = ' ';
                 outline[j++] = ' ';
                 outline[j++] = ' ';
                 outline[j++] = '0';
                 outline[j++] = 'x';
                 outline[j++] = block[i++];
                 outline[j++] = block[i++];
                 outline[j++] = block[i++];
                 outline[j++] = block[i++];
                 outline[j++] = ',';
             }
        if (size != line_size)
             outline[j++] = ',';
        outline[--j] = '\n';
        outline[++j] = '\0';
        fputs (outline, outf);
        index +=20;
        size -=20;
        if (size <= line_size)
            line_size = size;
        }
}

void block_transfer (FILE *inf)
{
    int  ix, index=0, addr=0,len=0, total=0, line_len;
    char line[MAXINLINE];

         fgets (line, MAXINLINE, inf);
      while (!((line[0] == 'S') && (line[1] == '9'))) {

         if ( line [2] > '9')
             line[2] -=7;
         len = line[2] - '0';
         if ( line [3] > '9')
             line[3] -=7;
         len = len * 16 + line[3] - '0';
         line_len = ((len - 3) << 1) + 8;
         fw_size = fw_size + ((len - 3) << 1);

         if ( line [4] > '9')
             line[4] -=7;
         addr = line[4] - '0';
         if (line[5] > '9')
             line[5] -=7;
         addr = addr * 16 + line[5] - '0';
         if (line[6] > '9')
             line[6] -=7;
         addr = addr * 16 + line[6] - '0';
         if (line[7] > '9')
             line[7] -=7;
         addr = addr * 16 + line[7] - '0';
         index = addr << 1;

         /*
          * here is the reason why line_len need to add 8 when do calculation
          * above, simply because ix start from 8.
          */
         for (ix = 8; ix < line_len; ix++) {
             block[index++] = line[ix];
         }
         fgets (line, MAXINLINE, inf);
      }
}

/*
 * section_output : Output a section
 */
void section_output (FILE *in, FILE *out)
{
    char iline[MAXINLINE];
    int  block_size=0;

    fgets (iline, MAXINLINE, in);

    if ((iline[0] != 'S') && (iline[1] != '0')) {
       printf("Error - line header byte doesn't match S0\n");
       exit(1);
    }

    block_transfer(in);
    convert (out);
}


/*
 * finish_output : Finish off the output
 */
void finish_output (FILE *f)
{
    fw_size = fw_size >> 2;
    printf("The total words is %d\n", fw_size);
    fprintf(f, "\n};\n");
    fprintf(f, "\n/*\n * NEAT firmware length (in words)\n */\n");
    fprintf(f, "const int t1_firmware_words = %d;\n", fw_size);
    fprintf(f, "\n/* End of file */\n");
}


int main (int argc, char *argv[])
{
    FILE *fopen(), *in_file, *out_file;
    char *program_name;
    char filename[20];


    if (argc > 1)
       in_file = fopen (argv[1], "r");
    else {
       printf("Usage: t1 neat.ram.srec <c_output_file>\n");
       exit(1);
    }

    if (in_file == NULL) {
        /*
         * unable to read/write filename passed as first argument
         */
        printf("Unable to find read file \n");
        exit(1);
    }

    if (argc > 2)
        program_name = argv[2];
    else 
        program_name = "t1_firmware.c";

    out_file = fopen (program_name, "w");
   
    if ( out_file == NULL) {
        printf("Unable to create write file \n");
        exit(1);
    }
        start_output(out_file);
        section_output(in_file, out_file);
        finish_output(out_file);

    close(in_file);

    /*
     * We are done
     */
    exit(0);
}

