/* $Id: sym2s4rec.c,v 1.1.4.1 1996/05/09 15:00:42 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sartools/sym2s4rec.c,v $
 *------------------------------------------------------------------
 * sym2s4rec.c -- symbol table converter.
 *
 * 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sym2s4rec.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:42  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:39  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:59  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:51  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:25  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:32  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <stdio.h>
#include <fcntl.h>

/*
**	Ratinder P. S. Ahuja
** Derived from gens4rec.c.  This utility generates S4 records from the
** symbolsrec format of the "mips64-elf-ld -oformat symbolsrec".  The output
** is of the format:

$$ hello^M
  /tmp_mnt/home/rahuja/synergy/work/atm.sav/atmizer/lsi/sdk/lib/bo/crt0x.o $a0d80000^M
  hello.o $a0d801e0^M
  gcc2_compiled. $a0d801e0^M
  __gnu_compiled_c $a0d801e0^M
  clrbss.o $a0d80220^M
  __main $a0d80088^M
  clrbss $a0d80220^M
  onintr $a0d80168^M
  printf $a0d80108^M
  vsprintf $a0d80120^M
  _fdata $a0c00000^M
  getenv $a0d80150^M
  _etext $a0d80260^M
  _gp $a0c08010^M
  flush_cache $a0d80180^M
  _DYNAMIC_LINK $0^M
  _open $a0d800c0^M
  getclkfreq $a0d801c8^M
  end $a0c00020^M
  _filebase $a0c00010^M
  _ftext $a0d80000^M
  _start $a0d80000^M
  adr2symoff $a0d80198^M
  sbrk $a0d80034^M
  __bss_start $a0c00010^M
  main $a0d801e0^M
  sym2adr $a0d801b0^M
  _write $a0d800a8^M
  _edata $a0c00010^M
  _end $a0c00020^M
  _ioctl $a0d800f0^M
  _read $a0d80090^M
  _exit $a0d8001c^M
  allocp1 $a0c00008^M
  _fbss $a0c00010^M
  _close $a0d800d8^M
$$ ^M
**
**	The symbols are enclosed in $$ $$
*/

char *usage[] = {
	"usage: sym24srec file.symsrec > <file>",
	"\tconvert the output from symsec file into S4 records suitable",
	"\tfor downloading symbols to LSI Logic's Prom Monitor.",
	0};

main(argc,argv)
int argc;
char *argv[];
{
char buf[200],name[64],fname[64];
char start[10], end[10];
unsigned long addr;
int len,csum,i;
FILE *f1;

	if (argc != 2) {
		for (i=0;usage[i];i++) fprintf(stderr,"%s\n",usage[i]);
		exit(-1);
	}

	if ( (f1 = fopen(argv[1], "r")) == NULL ) {
		fprintf(stderr,"\n\r Can't read %s", argv[1]);
		exit(-1);
	}

	fgets(buf, 100, f1);
	/*
	** First line, first 2 charecters should be $$
	*/
	sscanf(buf,"%s %s",start,fname);
	if(strcmp(start, "$$")) {
		fprintf(stderr, "\n\rInvalid start of symbols: %s for %s", start, name);
		exit(1);
	} else {
		fprintf(stderr, "\n\r Scanning file %s", fname);
	}

	/*
	** Skip over next line
	*/
	fgets(buf, 100, f1);

	/*
	** get symbol name and address
	*/
	while (fgets(buf, 100, f1)) {

		if( (buf[0] == '$') && (buf[1] == '$') ) {
			fprintf(stderr, "\n\r End of symbols");
			break;
		}
		sscanf(buf,"%s $%lx",name, &addr);
		csum = 0;
		len = strlen(name)+8+2;
		printf("S4%02X%08X%s,%02x\n",len,addr,name,csum);

	}/* while */

	/*
	** Write out the rest of the srecords 
	*/
	
	while (fgets(buf, 100, f1)) {
		printf("%s",buf);
	}

	exit(0);
}

