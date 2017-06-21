/* $Id: sse_test.c,v 3.3.26.2 1996/04/29 10:02:25 mdb Exp $
 * $Source: /release/112/cvs/Xsys/hes/sse_test.c,v $
 *------------------------------------------------------------------
 * sse_test.c -- Corona debugging code.
 *
 * April 1993 Steve Jacobson
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sse_test.c,v $
 * Revision 3.3.26.2  1996/04/29  10:02:25  mdb
 * The 96q1 gcc is much more particular about putting parentheses around
 * comparisons for some operands ("&", "^" and "|") [CSCdi56015].
 * Branch: California_branch
 *
 * Revision 3.3.26.1  1996/03/18  19:44:04  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.2  1996/03/07  09:40:57  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.2.1  1996/01/24  21:48:31  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1996/01/18  22:47:41  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  09:21:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:43:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:45:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Define this symbol if this is being compiled into the diagnostic rom
   monitor. */

#undef DRM
/* #define DRM 1 */

#ifdef DRM
#include "corona_tree.h"
#include "../../defs.h"
#else /* DRM */
#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "if_mci.h"
#include "ttysrv.h"
#include "if_fci.h"
#endif /* !DRM */
#include "sse_test.h"

/*
typedef int boolean;
*/

/*
 * Forward declarations
 */
static char * get_token(char *ptr1, char *ptr2);
static boolean tree_asm(char *buf, unsigned *result);
static void tree_print(union instr inst_union);
static void tree_mem_read(union instr *inst_union, int index);
static void tree_mem_write(union instr *inst_union, int inex);
static void upshift(char *data_buf, unsigned last_byte_index);
				        /* upshifts string data		*/
static ushort cmd_op(char *buf);	/* looks up command mnemonic	*/
#ifdef DRM
void local_buserr_handler();    /* handler for bus error traps	*/
#endif
static unsigned short read_absolute(short *abs_addr);
                                        /* read 68K address space */ 
static void write_absolute(short *abs_addr, unsigned short data);
					/* write 68K address space	*/
static unsigned short read_multibus_mem(int mbus_addr, unsigned int
					*mbus_abs_addr); 
static void write_multibus_mem(int mbus_addr, unsigned short mbus_data,
			       unsigned int *mbus_abs_addr);
static unsigned short read_multibus_io(int mbus_addr, unsigned int
				       *mbus_abs_addr); 
static void write_multibus_io(int mbus_addr, unsigned short mbus_data, unsigned
			      int *mbus_abs_addr);
static ushort ex_sp_cmd_l(mciregtype *sp_regptr, ushort cmd, uint larg);
					/* execute SP command		*/
static mciregtype * get_sp_reg_addr(mciregtype *sp_regptr, char *read_buf,
				    unsigned *slot);
                                        /* calculate sp register addr */
static unsigned char read_multibus_io_byte(int mbus_addr, unsigned int
					   *mbus_abs_addr); 
static void write_multibus_io_byte(int mbus_addr, unsigned char mbus_data,
				   unsigned int *mbus_abs_addr);
static unsigned char read_absolute_byte(unsigned char *abs_addr);
static void write_absolute_byte(unsigned char *abs_addr, unsigned char data);
static unsigned short read_memd(mciregtype *sp_regptr, int memd_addr);
				/* read memD from SP		*/
static void write_memd(mciregtype *sp_regptr, int memd_addr, unsigned short
		       memd_data);  /* write memD to SP		*/ 
void sse_debugger(void);


/* Global Variables */

#ifdef DRM
extern void (*buserrhandler)();         /* routine to bus error abort */
extern char abortstate;                 /* what to do with abort */
#else
static void (*buserrhandler)(void);         /* routine to bus error abort */
static char abortstate;                 /* what to do with abort */
#endif

#ifdef NOTDEF
union instr Prg_lt[65536];	/* < instruction array		*/
union instr Prg_eq[65536];	/* = instruction array		*/
union instr Prg_gt[65536];	/* > instruction array		*/
#endif

static unsigned int Err_addr;		/* error address if bus timeout	*/

#ifdef NOTDEF
char Read_buf[160];		/* buffer for input data	*/
#endif

#ifdef NOBODY_CALLS_ME
void sse_debug_outer (void)
{
#ifdef DRM
    int i;				/* general purpose		*/ 
#endif


    printf("\nCorona debugger, version 00.25, 05/14/93\n");

/***
    for (i=0; i <= 0xFFFF; i++) {
        Prg_lt[i].word = 0x0000C500;
        Prg_lt[i].fields.addr = (unsigned short)i + 1;
        Prg_lt[i].fields.opcode = i & 0x3F;
        Prg_lt[i].fields.cksum = (i >> 6) & 0x1;
        Prg_lt[i].fields.read = (i >> 7) & 0x1;
        Prg_eq[i].word = 0x00006900;
        Prg_eq[i].fields.addr = (unsigned short)i + 1;
        Prg_eq[i].fields.opcode = i & 0x3F;
        Prg_eq[i].fields.cksum = (i >> 6) & 0x1;
        Prg_eq[i].fields.read = (i >> 7) & 0x1;
        Prg_gt[i].word = 0x00003A00;
        Prg_gt[i].fields.addr = (unsigned short)i + 1;
        Prg_gt[i].fields.opcode = i & 0x3F;
        Prg_gt[i].fields.cksum = (i >> 6) & 0x1;
        Prg_gt[i].fields.read = (i >> 7) & 0x1;

    }

***/

    sse_debugger();	/* Corona SSE debugger			*/

}

#ifdef NOTDEF
        gets(Read_buf);  /* console input to Read_buf	*/
        Buf[0] = NULL;
        in1 = -1;
        in2 = -1;
        in3 = -1;
        ic = sscanf(Read_buf, "%s %x %*[,;] %x %*[,;] %x",
        Buf, &in1, &in2, &in3);
        if (ic > 0) {	/* if more than 0 strings read	*/
            switch (Buf[0])
            {
	    }
	}
}
#endif
#endif

void sse_debugger (void)	/* Corona SSE debugger			*/
{
    union instr instr[3];	/* Single instruction line		*/
    int i;
    int j;
    int in1;		/* break mode read parameter		*/
    int in2;		/* break mode read parameter		*/
    int in3;		/* break mode read parameter		*/
    int in4;		/* break mode read parameter		*/
    int ic;			/* break mode input parm count		*/
    char bbuf[22];		/* disassemble mnemonic buffer		*/
    char read_buf[60];	/* input buffer				*/
    unsigned input1;	/* read parameter			*/
    char *ptr;		/* return pointer for gets		*/
    unsigned inst_word;	/* single, 32 bit tree instruction	*/
    
    boolean parsed;		/* gets return				*/
    short cmdval;		/* for main switch statement		*/
    unsigned short data;	/* short data word for multibus		*/
    unsigned int mb_addr;	/* absolute multibus address		*/
    char old_abortstate;	/* abort state save variable		*/
    unsigned int tree_addr;	/* tree memory instruction address	*/
    unsigned int *tree_arr;	/* tree program array in structure	*/
    mciregtype *sp_regptr;	/* SP register address space on mbus	*/
    unsigned char cdata;	/* short data word for multibus		*/
    unsigned short *multibus;
    unsigned int sp_slot;	/* slot number of Corona board		*/
    uchar *byte_arr;	/* packet data array in structure	*/
#ifdef DRM
    union instr inst_union;	/* Single instruction word		*/
    unsigned int slot;	/* slot number of dbus board		*/
    unsigned input2;	/* read parameter			*/
    int num_char;		/* disassemble: number of instr chars	*/
    short opcode;		/* assemble: opcode number of mnemonic	*/
    int ret;		/* read_con() return; 0 or ESC		*/
#else
    boolean abort;
#endif
    
    /* abortstate should be ABORT_INTERNAL on entry		*/
    old_abortstate = abortstate;
    
    sp_slot = -1;                    /* unitialized slot #	*/
    sp_regptr = (mciregtype *) 0x0;  /* points to SP registers	*/
    sp_regptr = get_sp_reg_addr (sp_regptr, read_buf, &sp_slot);
    
#ifdef INITIAL_RESET_NEEDED	/* commented out 7/21 */
    in4 = 0x30000 + (0x40 * sp_slot);

    write_multibus_io_byte(in4 + 5, 0x02, &mb_addr);
    printf ("\n0x%-06x  (0x%-08x) = 0x%-02x",
	    in4 + 5, mb_addr, 0x02);
    
    write_multibus_io_byte(in4 + 2, 0x00, &mb_addr);
    printf ("\n0x%-06x  (0x%-08x) = 0x%-02x",
	    in4 + 2, mb_addr, 0x00);
    
    write_multibus_io_byte(in4 + 5, 0x00, &mb_addr);
    printf ("\n0x%-06x  (0x%-08x) = 0x%-02x",
	    in4 + 5, mb_addr, 0x00);
#ifdef DRM
    delay (100);
#else
    DELAY(100);
#endif /* !DRM */
    
    write_multibus_io_byte(in4 + 5, 0xC2, &mb_addr);
    printf ("\n0x%-06x  (0x%-08x) = 0x%-02x",
	    in4 + 5, mb_addr, 0xC2);
#endif  /* INITIAL_RESET_NEEDED */
    
    cmdval = QM_ENTRY;  /* question mark (?)			*/
    
    while (TRUE) {
	abort = FALSE;
	printf ("\nCorona Debugger:  ");
	
#ifdef NOTDEF
	slot = -1;                       /* unitialized slot #	*/
	regptr = (mciregtype *) 0x0;     /* points to registers	*/
#endif
	
	in1 = -1;
	in2 = -1;
	in3 = -1;
	in4 = -1;
	ic = 0;
#ifdef DRM
	parsed = (int) gets(read_buf);
#else
	parsed = (int) rdtty(read_buf, sizeof(read_buf));
#endif	
	if (!null(read_buf) && parsed) {
	    /*
	      cmdchar = cvu(read_buf[0]);
	      ptr = read_buf;
	      */
	    ptr = get_token(read_buf, bbuf);
	    ic = 0;
	    /*
	      cmdchar = read_buf[0];
	      printf ("\nptr1 = *%c* 0x%-02x 0x%-08x", *ptr, *ptr, ptr);
	      */
	    /* discard first token */
	    /*
	      ptr = get_token(read_buf, bbuf);
	      printf ("\ncmdval = 0x%-08x", cmdval);
	      */
	    
	    if (ptr != NULL) {
		upshift(bbuf, 10);
		cmdval = cmd_op(bbuf);
		ic++;
		/* get second token */
		ptr = get_token(ptr, bbuf);
		if (ptr != NULL) {
		    if (!null(bbuf)) {
			/*
			  in1 = parse_hex_long(bbuf);
			  */
			if (bbuf[1] == 'x' || bbuf[1] == 'X') {
			    bbuf[0] = bbuf[1] = '0';
			}
#ifdef DRM
			in1 = gethexnum(bbuf);
#else
			in1 = parse_hex_long(bbuf);
#endif /* !DRM */			
			ic++;
		    }
		    
		    /* get third token */
		    ptr = get_token(ptr, bbuf);
		    if (ptr != NULL) {
			if (!null(bbuf)) {
			    if (bbuf[1] == 'x' || bbuf[1] == 'X') {
				bbuf[0] = bbuf[1] = '0';
			    }
#ifdef DRM
			    in2 = gethexnum(bbuf);
#else
			    in2 = parse_hex_long(bbuf);
#endif /* !DRM */			
			    ic++;
			}
			
			/* get fourth token */
			ptr = get_token(ptr, bbuf);
			if (ptr != NULL) {
			    if (!null(bbuf)) {
				if (bbuf[1] == 'x' || bbuf[1] == 'X') {
				    bbuf[0] = bbuf[1] = '0';
				}
#ifdef DRM
				in3 = gethexnum(bbuf);
#else
				in3 = parse_hex_long(bbuf);
#endif /* !DRM */			
				ic++;
			    }
			    
			    /* get fifth token */
			    ptr = get_token(ptr, bbuf);
			    if (ptr != NULL) {
				if (!null(bbuf)) {
				    if (bbuf[1] == 'x' || bbuf[1] == 'X') {
					bbuf[0] = bbuf[1] = '0';
				    }
#ifdef DRM
				    in4 = gethexnum(bbuf);
#else
				    in4 = parse_hex_long(bbuf);
#endif /* !DRM */			
				    ic++;
				}
			    }
			}
		    }
		}
	    }
	}
	
	switch (cmdval) {  
	    
	case AT_ENTRY:	/* assemble	*/
	    if (ic < 2 || ic > 3) {
		printf("\n  Usage:  at <addr>[,<count>]\n");
	    } else {
		if (in2 < 1)  { in2 = 1; }
		i=in1;
		while (i < in1+in2) {
		    tree_mem_read(instr, i);
		    
		    inst_word = instr[0].word;
		    ptr = read_buf;
		    /*
		      printf ("\nptr = *%c* 0x%-02x 0x%-08x 0x%-08x",
		      *ptr, *ptr, ptr, &ptr);
		      printf ("\nread_buf[0] = *%c*", read_buf[0]);
		      */
		    
		    printf ("\n%-04x:  < instruction? [", i);
		    tree_print(instr[0]);
		    printf ("]:  ");
		    
#ifdef DRM
		    parsed = (int) gets(read_buf);
#else
		    parsed = (int) rdtty(read_buf, sizeof(read_buf));
#endif
		    /*
		      printf ("\nret = *%c* 0x%-02x 0x%-08x 0x%-08x",
		      &ptr, &ptr, ptr, *ptr);
		      */
		    
		    if (tree_asm(read_buf, &inst_word)) {
			/*
			  printf ("\ngood:  result = 0x%-08x",
			  inst_word);
			  */
			instr[0].word = inst_word;
		    } else {
			printf("\n  Address 0x%-04x, < instruction:  ", i);
			printf("Syntax:  [RD+][CK+] instr address data\n\n");
		    }
		    
		    inst_word = instr[1].word;
		    ptr = read_buf;
		    
		    printf ("%-04x:  = instruction? [", i);
		    tree_print(instr[1]);
		    printf ("]:  ");
		    
#ifdef DRM
		    parsed = (int) gets(read_buf);
#else
		    parsed = (int) rdtty(read_buf, sizeof(read_buf));
#endif			
		    if (tree_asm(read_buf, &inst_word)) {
			instr[1].word = inst_word;
		    } else {
			printf("\n  Address 0x%-04x, = instruction:  ", i);
			printf("Syntax:  [RD+][CK+] instr address data\n\n");
		    }
		    
		    inst_word = instr[2].word;
		    ptr = read_buf;
		    
		    printf ("%-04x:  > instruction? [", i);
		    tree_print(instr[2]);
		    printf ("]:  ");
		    
#ifdef DRM
		    parsed = (int) gets(read_buf);
#else
		    parsed = (int) rdtty(read_buf, sizeof(read_buf));
#endif
		    
		    if (tree_asm(read_buf, &inst_word)) {
			instr[2].word = inst_word;
		    } else {
			printf("\n  Address 0x%-04x, > instruction:  ", i);
			printf("Syntax:  [RD+][CK+] instr address data\n\n");
		    }
		    
		    tree_mem_write(instr, i);
		    i++;
		}
	    }
	    break;
	case DT_ENTRY:	/* disassemble	*/
	    if (ic < 2 || ic > 3) {
		printf ("\n  Usage:  dt <addr>[,<count>]\n");
	    } else {
		if (in2 < 1)  { in2 = 1; }
		i=in1;
		while (i < in1+in2)
		    {
			tree_mem_read(instr, i);
			
			printf("\n%-04x:", i);
			
			tree_print(instr[0]);
			tree_print(instr[1]);
			tree_print(instr[2]);
			
			i++;
		    }
	    }
	    break;
	case ESP_ENTRY:	/* execute SP command	*/
	    if (ic < 3 || ic > 3) {
		printf("\n  Usage:  esp <cmd>,<arg>\n");
	    } else {
		/*****
		  if (sp_regptr == 0) {
		  printf ("\nSP slot?:  ");
		  if (parsed = (int) gets(read_buf)) {
		  if (!null(read_buf)) {
		  if (read_buf[1] == 'x' || read_buf[1] == 'X') {
		  read_buf[0] = read_buf[1] = '0';
		  }
		  input1 = gethexnum(read_buf);
		  sp_regptr = (mciregtype *) (0x1110C000 +
		  input1 * 0x40);
		  }
		  }
		  }
		  printf ("\nSlot = 0x%-04x 0x%-04x", input1, sp_regptr);
		  *****/
		sp_regptr = get_sp_reg_addr (sp_regptr, read_buf, &sp_slot);
		data = ex_sp_cmd_l(sp_regptr, in1, in2);
		
		printf ("\nSP result = 0x%-04x\n", data);
	    }
	    break;
	case LSTP_ENTRY:	/* list available tree programs	*/
	    if (ic < 1 || ic > 1) {
		printf("\n  Usage:  lstp\n");
	    } else {
		if (COR_TREE_TESTS > 0) {
		    for (i=0; i < COR_TREE_TESTS; i++) {
			printf("\nTest 0x%-02x:  %s, ts=%-04x, ICB=%-02x",
			       i, cor_tree_tests[i].name,
			       cor_tree_tests[i].tree_size,
			       cor_tree_tests[i].icb_num);
			printf(", ps=%-02x, pa=%-04x",
			       cor_tree_tests[i].packet_size,
			       cor_tree_tests[i].packet_addr);
			printf(", rs=%-02x, ra=%-04x",
			       cor_tree_tests[i].rewrite_size,
			       cor_tree_tests[i].rewrite_addr);
		    }
		} else {
		    printf("\nNo tree tests available.\n");
		}
	    }
	    break;
	case LPD_ENTRY:	/* load packet data	*/
	    if (ic < 2 || ic > 2) {
		printf("\n  Usage:  lpd <prog #>\n");
	    } else {
		sp_regptr = get_sp_reg_addr (sp_regptr, read_buf, &sp_slot);
		if (in1 < COR_TREE_TESTS) {
		    if (cor_tree_tests[in1].packet_size == 0) {
			printf("\n*** Test %d has no packet data.\n", in1);
		    } else {
			printf("\nLoading %s packet data...",
			       cor_tree_tests[in1].name);
			byte_arr = cor_tree_tests[in1].packet;
			/*
			  printf ("\npacket_size = %d", 
			  cor_tree_tests[in1].packet_size);
			  */
			/* use in4 as memD address		*/
			in4 = cor_tree_tests[in1].packet_addr;
			for (i = 0;
			     i < cor_tree_tests[in1].packet_size;
			     i = i + 2) {
			    data = (byte_arr[i] << 8) | byte_arr[i+1];
			    /*
			      printf ("\n0x%-06x:  0x%-04x", in4, data);
			      */
			    write_memd(sp_regptr, in4, data);
			    in4++;
			}
			printf("  Loaded.\n");
		    }
		} else {
		    printf("\n*** Test %d does not exist.\n", in1);
		}
	    }
	    break;
	case LTP_ENTRY:	/* load tree program	*/
	    if (ic < 2 || ic > 2) {
		printf("\n  Usage:  ltp <prog #>\n");
	    } else {
		if (in1 < COR_TREE_TESTS) {
		    printf("\nLoading %s test...",
			   cor_tree_tests[in1].name);
		    tree_arr = cor_tree_tests[in1].treecode;
		    /*
		      printf ("\ntree_size = %d", 
		      cor_tree_tests[in1].tree_size);
		      */
		    i = 0;
		    while (i < cor_tree_tests[in1].tree_size) {
			tree_addr = tree_arr[i++];
			instr[0].word = tree_arr[i++];
			instr[1].word = tree_arr[i++];
			instr[2].word = tree_arr[i++];
			/*
			  printf ("\n0x%-04x:  0x%-08x  0x%-08x  0x%-08x", 
			  tree_addr, instr[0].word, instr[1].word,
			  instr[2].word);
			  */
			tree_mem_write(instr, tree_addr);
		    }
		    printf("  Loaded.\n");
		} else {
		    printf("\nTest %d does not exist.\n", in1);
		}
	    }
	    break;
	case H_ENTRY:	/* help		*/
	case QM_ENTRY:
	    printf ("\nat:\tassemble tree instruction\t");
	    printf ("dt:\tdisassemble tree instruction\n");
	    printf ("esp:\texecute SP command\n");
	    printf ("h or ?:\thelp\n");
	    printf ("init:\tinitialize Corona board\n");
	    printf ("lpd:\tload packet data\n");
	    printf ("lstp:\tlist available tree programs\n");
	    printf ("ltp:\tload tree program\t\t");
	    printf ("vtp:\tverify tree program\n");
	    printf ("q:\tquit Corona debugger\n");
	    printf ("rmd:\tread memD\t\t\t");
	    printf ("wmd:\twrite memD\n");
	    printf ("rmi:\tread multibus I/O\t\t");
	    printf ("wmi:\twrite multibus I/O\n");
	    printf ("rmib:\tread multibus I/O, byte addr\t");
	    printf ("wmib:\twrite multibus I/O, byte addr\n");
	    printf ("rm:\tread multibus\t\t\t");
	    printf ("wm:\twrite multibus\n");
	    printf ("rms:\tread multibus, scope loop\t");
	    printf ("wms:\twrite multibus, scope loop\n");
	    printf ("rrr:\tread SSE result registers\n");
	    printf ("rt:\tread tree instruction\t\t");
	    printf ("wt:\twrite tree instruction\n");
	    printf ("ssed:\tissue done to SSE\n");
	    printf ("ssesh:\tstart SSE hardware\t\t");
	    printf ("ssesp:\tstart SSE program\n");
	    break;
	case INIT_ENTRY:	/* initialize Corona board	*/
	    if (ic < 2 || ic > 2) {
		printf("\n  Usage:  init <board #>\n");
	    } else {
		/***
		  regptr = get_sp_reg_addr (regptr, read_buf, &slot);
		  ***/
		in4 = 0x30000 + (0x40 * in1);
		
		write_multibus_io_byte(in4 + 5, 0x02, &mb_addr);
		printf ("\n0x%-06x  (0x%-08x) = 0x%-02x",
			in4 + 5, mb_addr, 0x02);
		
		write_multibus_io_byte(in4 + 2, 0x00, &mb_addr);
		printf ("\n0x%-06x  (0x%-08x) = 0x%-02x",
			in4 + 2, mb_addr, 0x00);
		
		write_multibus_io_byte(in4 + 5, 0x00, &mb_addr);
		printf ("\n0x%-06x  (0x%-08x) = 0x%-02x",
			in4 + 5, mb_addr, 0x00);
		
#ifdef DRM
		delay (100);
#else
		DELAY(100);
#endif
		
		write_multibus_io_byte(in4 + 5, 0xC2, &mb_addr);
		printf ("\n0x%-06x  (0x%-08x) = 0x%-02x",
			in4 + 5, mb_addr, 0xC2);
	    }
	    break;
	case Q_ENTRY:	/* exit user program	*/
	    printf ("\n");
	    abortstate = old_abortstate;
	    return;
	    break;
	case RM_ENTRY:	/* read multibus */
	    if (ic < 2 || ic > 3) {
		printf("\n  Usage:  rm <addr>[,<count>]\n");
	    } else {
		if (in2 < 1)   { in2 = 1; }
		/* addresses must be even (8 bit address, 16 bit data) */
		in1 = in1 & 0xFFFFFFFE;
		for (i=in1; i < in1+(in2 << 1); i=i+2) {
		    /* address >> 1, as array indexing undoes this     */
		    data = read_multibus_mem(i, &mb_addr);
		    printf ("\n0x%-06x  (0x%-08x):  0x%-04x",
			    i, mb_addr, data);
		}
	    }
	    break;
	case RMD_ENTRY:	/* read memD */
	    if (ic < 2 || ic > 3) {
		printf("\n  Usage:  rmd <addr>[,<count>]\n");
	    } else {
		if (in2 < 1)   { in2 = 1; }
		sp_regptr = get_sp_reg_addr (sp_regptr, read_buf, &sp_slot);
		for (i=in1; i < in1+in2; i++) {
		    data = read_memd(sp_regptr, i);
		    printf ("\nmemD  0x%-06x:  0x%-04x", i, data);
		}
	    }
	    break;
	case RMI_ENTRY:	/* read multibus I/O	*/
	    if (ic < 2 || ic > 3) {
		printf("\n  Usage:  rmi <addr>[,<count>]\n");
	    } else {
		if (in2 < 1)   { in2 = 1; }
		/* addresses must be even (8 bit address, 16 bit data) */
		in1 = in1 & 0xFFFFFFFE;
		for (i=in1; i < in1+(in2 << 1); i=i+2) {
		    /* address >> 1, as array indexing undoes this     */
		    data = read_multibus_io(i, &mb_addr);
		    printf ("\n0x%-06x  (0x%-08x):  0x%-04x",
			    i, mb_addr, data);
		}
	    }
	    break;
	case RMIB_ENTRY:	/* read multibus I/O, byte addressed	*/
	    if (ic < 2 || ic > 3) {
		printf("\n  Usage:  rmib <addr>[,<count>]\n");
	    } else {
		if (in2 < 1)   { in2 = 1; }
		for (i=in1; i < in1+in2; i++) {
		    cdata = read_multibus_io_byte(i, &mb_addr);
		    printf ("\n0x%-06x  (0x%-08x):  0x%-02x",
			    i, mb_addr, cdata);
		}
	    }
	    break;
	case RMS_ENTRY:	/* read multibus, scope loop	*/
	    if (ic < 2 || ic > 4) {
		printf("\n  Usage:  rms <addr>[,<range count>[,loop count]]\n");
	    } else {
		if (in2 < 1)   { in2 = 1; }
		/* addresses must be even (8 bit address, 16 bit data) */
		in1 = in1 & 0xFFFFFFFE;
		i = 0;     /* Keep compiler from whining */
		data = 0;  /* Keep compiler from whining */
		if (in3 < 1) {
		    /* No loop count specified, loop infinitely.	*/
		    while (!abort) {
			for (i=in1; i < in1+(in2 << 1); i=i+2) {
			    data = read_multibus_mem(i, &mb_addr);
			}
		    }
		} else {
		    /* Loop count specified, loop loop count.	*/
		    for (j=0; j < in3 && !abort; j++) {
			for (i=in1; i < in1+(in2 << 1); i=i+2) {
			    data = read_multibus_mem(i, &mb_addr);
			}
		    }
		}
		printf ("\n0x%-06x  (0x%-08x):  0x%-04x",
			i-2, mb_addr, data);
	    }
	    break;
	case RRR_ENTRY:	/* read SSE result registers	*/
	    if (ic < 1 || ic > 1) {
		printf("\n  Usage:  rrr\n");
	    } else {
		sp_regptr = get_sp_reg_addr (sp_regptr, read_buf, &sp_slot);
		
		for (i=0; i < 0x10; i++) {
		    data = ex_sp_cmd_l(sp_regptr, FCI_CMD_SSE_READ_RR, i);
		    printf ("\nResult Register 0x%x:  0x%-02x",
			    i, (uchar) data);
		}
	    }
	    break;
	case RT_ENTRY:	/* read in hex	*/
	    /*    printf("\nin1 = %x, in2 = %x, in3 = %x",
		  in1, in2, in3); */
	    if (ic < 2 || ic > 3) {
		printf("\n  Usage:  rt <addr>[,<count>]\n");
	    } else {
		if (in2 < 1)   { in2 = 1; }
		for (i=in1; i < in1+in2; i++) {
		    tree_mem_read(instr, i);
		    printf ("\n0x%-04x:  0x%-08x  0x%-08x  0x%-08x",
			    i, instr[0].word, instr[1].word, instr[2].word);
		}
	    }
	    break;
	case SSED_ENTRY:	/* issue done to SSE	*/
	    if (ic < 1 || ic > 1) {
		printf("\n  Usage:  ssed\n");
	    } else {
		sp_regptr = get_sp_reg_addr (sp_regptr, read_buf, &sp_slot);
		data = ex_sp_cmd_l(sp_regptr, FCI_CMD_SSE_DONE, 0);
	    }
	    break;
	case SSESH_ENTRY:	/* start SSE hardware	*/
	    if (ic < 1 || ic > 2) {
		printf("\n  Usage:  ssesh [,<start addr>]\n");
	    } else {
		if (in1 < 1)   { in1 = 0x0000; }
		multibus = (unsigned short *) MULTIBUS_SSE_START_VECTOR;
		for (i=0; i < 2; i++) {
		    mb_addr = (unsigned int) &multibus[i];
		    write_absolute((short *)mb_addr, in1);
		    /*
		      printf ("\nssesh:  0x%-08x 0x%-08x 0x%-04x",
		      multibus, &multibus[i], in1);
		      */
		    printf ("\n0x%-06x  (0x%-08x) = 0x%-04x",
			    SSE_START_VECTOR+i+i, mb_addr, in1);
		}
	    }
	    break;
	case SSESP_ENTRY:	/* start SSE program	*/
	    if (ic < 2 || ic > 3) {
		printf("\n  Usage:  ssesp <ICB #>[,<packet addr>]\n");
	    } else {
		if (in2 < 1)   { in2 = 0x1000; }
		sp_regptr = get_sp_reg_addr (sp_regptr, read_buf, &sp_slot);
		data = ex_sp_cmd_l(sp_regptr, FCI_CMD_MEMD_SELECT, in2);
		data = ex_sp_cmd_l(sp_regptr, FCI_CMD_SSE_START, in1);
	    }
	    break;
	case VTP_ENTRY:	/* verify tree program	*/
	    if (ic < 2 || ic > 2) {
		printf("\n  Usage:  vtp <prog #>\n");
	    } else {
		if (in1 < COR_TREE_TESTS) {
		    printf("\nVerifying %s test...",
			   cor_tree_tests[in1].name);
		    tree_arr = cor_tree_tests[in1].treecode;
		    /*
		      printf ("\ntree_size = %d", 
		      cor_tree_tests[in1].tree_size);
		      */
		    in4 = 0;    /* error counter */
		    i = 0;
		    while ((i < cor_tree_tests[in1].tree_size) && !abort) {
			tree_addr = tree_arr[i];
			
			tree_mem_read(instr, tree_addr);
			
			if ((instr[0].word != tree_arr[i + 1])
			    | (instr[1].word != tree_arr[i + 2])
			    | (instr[2].word != tree_arr[i + 3])) {
			    in4++;
			    printf("\n*** Verify Failure:  ");
			    printf("\n    Program:      ");
			    printf ("0x%-04x:  0x%-08x  0x%-08x  0x%-08x", 
				    tree_addr, tree_arr[i + 1],
				    tree_arr[i + 2], tree_arr[i + 3]);
			    printf("\n    Tree memory:  ");
			    printf ("0x%-04x:  0x%-08x  0x%-08x  0x%-08x", 
				    tree_addr, instr[0].word, instr[1].word,
				    instr[2].word);
			}
			i = i + 4;
		    }
		    if (in4 == 0) {
			printf("  Verified.  No errors.\n");
		    } else {
			printf("\n\n*** Verify failed with 0x%-x errors.\n",
			       in4);
		    }
		} else {
		    printf("\nTest %d does not exist.\n", in1);
		}
	    }
	    break;
	case WM_ENTRY:	/* write multibus */
	    if (ic < 3 || ic > 4) {
		printf("\n  Usage:  wm <addr>,<data>[,<count>]\n");
	    } else {
		if (in3 < 1)   { in3 = 1; }
		/* addresses must be even (8 bit address, 16 bit data) */
		in1 = in1 & 0xFFFFFFFE;
		/* address >> 1, as array indexing undoes this     */
		for (i=in1; i < in1+(in3 << 1); i=i+2) {
		    write_multibus_mem(i, in2, &mb_addr);
		    if (i == in1) {
			printf ("\n0x%-06x  (0x%-08x) = 0x%-04x",
				i, mb_addr, in2);
		    }
		}
		if (in3 > 1) {
		    printf (";  count = 0x%x", in3);
		}
	    }
	    break;
	case WMD_ENTRY:	/* write memD	*/
	    if (ic < 3 || ic > 4) {
		printf("\n  Usage:  wmd <addr>,<data>[,<count>]\n");
	    } else {
		if (in3 < 1)   { in3 = 1; }
		sp_regptr = get_sp_reg_addr (sp_regptr, read_buf, &sp_slot);
		for (i=in1; i < in1+in3; i++) {
		    write_memd(sp_regptr, i, in2);
		    if (i == in1) {
			printf ("\nmemD  0x%-06x = 0x%-04x", i, in2);
		    }
		}
		if (in3 > 1) {
		    printf (";  count = 0x%x", in3);
		}
	    }
	    break;
	case WMI_ENTRY:	/* write multibus I/O	*/
	    if (ic < 3 || ic > 4) {
		printf("\n  Usage:  wmi <addr>,<data>[,<count>]\n");
	    } else {
		if (in3 < 1)   { in3 = 1; }
		/* addresses must be even (8 bit address, 16 bit data) */
		in1 = in1 & 0xFFFFFFFE;
		/* address >> 1, as array indexing undoes this     */
		for (i=in1; i < in1+(in3 << 1); i=i+2) {
		    write_multibus_io(i, in2, &mb_addr);
		    if (i == in1) {
			printf ("\n0x%-06x  (0x%-08x) = 0x%-04x",
				i, mb_addr, in2);
		    }
		}
		if (in3 > 1) {
		    printf (";  count = 0x%x", in3);
		}
	    }
	    break;
	case WMIB_ENTRY:	/* write multibus I/O, byte addressed	*/
	    if (ic < 3 || ic > 4) {
		printf("\n  Usage:  wmib <addr>,<data>[,<count>]\n");
	    } else {
		if (in3 < 1)   { in3 = 1; }
		for (i=in1; i < in1+in3; i++) {
		    write_multibus_io_byte(i, in2, &mb_addr);
		    if (i == in1) {
			printf ("\n0x%-06x  (0x%-08x) = 0x%-02x",
				i, mb_addr, in2);
		    }
		}
		if (in3 > 1) {
		    printf (";  count = 0x%x", in3);
		}
	    }
	    break;
	case WMS_ENTRY:	/* write multibus, scope loop	*/
	    if (ic < 3 || ic > 5) {
		printf("\n  Usage:  wms <addr>,<data>[,<range count>[,loop count]]\n");
	    } else {
		if (in3 < 1)   { in3 = 1; }
		/* addresses must be even (8 bit address, 16 bit data) */
		in1 = in1 & 0xFFFFFFFE;
		i = 0;     /* Keep compiler from whining */
		if (in4 < 1) {
		    /* No loop count specified, loop infinitely.	*/
		    while (!abort) {
			for (i=in1; i < in1+(in3 << 1); i=i+2) {
			    write_multibus_mem(i, in2, &mb_addr);
			}
		    }
		} else {
		    /* Loop count specified, loop loop count.	*/
		    for (j=0; j < in4 && !abort; j++) {
			for (i=in1; i < in1+(in3 << 1); i=i+2) {
			    write_multibus_mem(i, in2, &mb_addr);
			}
		    }
		}
		printf ("\n0x%-06x  (0x%-08x) = 0x%-04x",
			i-2, mb_addr, in2);
	    }
	    break;
	case WT_ENTRY:	/* write in hex	*/
	    if (ic < 2 || ic > 3) {
		printf ("\n  Usage:  wt <addr>[,<count>]\n");
	    } else {
		if (in2 < 1)   { in2 = 1; }
		
		tree_mem_read(instr, in1);
		
		printf ("\n0x%-04x:  < value? [0x%-08x]:  ", 
			in1, instr[0].word);
		/**
		  ptr = gets(read_buf);
		  **/
		/*
		  printf ("\nret = *%c* 0x%-02x 0x%-08x 0x%-08x",
		  &ptr, &ptr, ptr, *ptr);
		  */
		/**
		  if (*ptr != 0) {
		  ret = sscanf(read_buf, "%x", &input1);
		  }
		  **/
		
#ifdef DRM
		parsed = (int) gets(read_buf);
#else
		parsed = (int) rdtty(read_buf, sizeof(read_buf));
#endif
		
		if (parsed) {
		    if (!null(read_buf)) {
			if (read_buf[1] == 'x' || read_buf[1] == 'X') {
			    read_buf[0] = read_buf[1] = '0';
			}
#ifdef DRM
			input1 = gethexnum(read_buf, buf);
#else
			input1 = parse_hex_long(read_buf);
#endif /* !DRM */			
			/*
			  printf ("\nparsed = %d, input1 = 0x%-08x",
			  parsed, input1);
			  */
			instr[0].word = input1;
		    }
		}
		
		/*
		  printf ("\nret = %d, input1 = 0x%-08x",
		  ret, input1);
		  */
		
		printf ("0x%-04x:  = value? [0x%-08x]:  ", 
			in1, instr[1].word);
#ifdef DRM
		parsed = (int) gets(read_buf);
#else
		parsed = (int) rdtty(read_buf, sizeof(read_buf));
#endif
		
		if (parsed) {
		    if (!null(read_buf)) {
			if (read_buf[1] == 'x' || read_buf[1] == 'X') {
			    read_buf[0] = read_buf[1] = '0';
			}
#ifdef DRM
			input1 = gethexnum(read_buf);
#else
			input1 = parse_hex_long(read_buf);
#endif /* !DRM */			
			instr[1].word = input1;
		    }
		}
		
		printf ("0x%-04x:  > value? [0x%-08x]:  ", 
			in1, instr[2].word);
#ifdef DRM
		parsed = (int) gets(read_buf);
#else
		parsed = (int) rdtty(read_buf, sizeof(read_buf));
#endif
		if (parsed) {
		    if (!null(read_buf)) {
			if (read_buf[1] == 'x' || read_buf[1] == 'X') {
			    read_buf[0] = read_buf[1] = '0';
			}
#ifdef DRM
			input1 = gethexnum(read_buf);
#else
			input1 = parse_hex_long(read_buf);
#endif
			
			instr[2].word = input1;
		    }
		}
		
		for (i=in1; i < in1+in2; i++)
		    {
			tree_mem_write(instr, i);
		    }
	    }
	    break;
	default:
	    printf ("\n  Unknown command\n");
	    break;
	}  /*  end switch(cmdval)  */
	
    }  /*  end while (TRUE)  */
    
}  /*  end sse_debugger()  */

#ifdef NOTDEF
}  /*  end if (ic > 0)  */
#endif

static int op_mnem (unsigned int opcode, unsigned char *buf)
{
    int i;
    struct mnemonic_tbl *ptr;

    /* printf("op_mnem():  opcode = 0x%x\n", opcode); */

    /* compare mnemonic in each table entry, looking for match  */
    for (ptr=Op_tbl; ptr->opcode < END_TBL; ptr++) {
        /* if match found, return string, # of parms    */
        if (opcode == ptr->opcode)
        {
/*
            printf("\nop_mnem():  opcode found!  ptr->opcode = 0x%x  ", 
                ptr->opcode);
*/
            for (i=0; i <= ptr->num_char; i++) /* copy all chars + 1 null */
            {
                buf[i] = ptr->mnem[i];
            }
            return (ptr->num_char);
        }
    }    
    buf[0] = '?';       /* error - mnemonic not found   */
    buf[1] = '?';
    buf[2] = '?';
    buf[3] = '\0';
    return (0);         /* zero parameters      */
}


static char *get_token (char *ptr1, char *ptr2)
{
/*
    printf("\nget_token() start:  ptr1 = *%c* 0x%-08x 0x%-08x 0x%-08x",
        *ptr1, *ptr1, ptr1, &ptr1);
    printf("\nptr1 = 0x%-08x", ptr1);
    printf("\nget_token1():  char = *%c* 0x%-02x 0x%-08x", *ptr1,*ptr1,ptr1);
    printf("\nptr1 = *%c* 0x%-08x 0x%-08x 0x%-08x", *ptr1, *ptr1, ptr1, &ptr1);
*/

    while (*ptr1 == ' ' || *ptr1 == ',' || *ptr1 == ';'
        || *ptr1 == '+' || *ptr1 == '|' || *ptr1 == TAB
        || *ptr1 == CR  || *ptr1 == '\0') {
/*
        printf("\nget_token2():  char = *%c* 0x%-02x 0x%-08x", *ptr1,*ptr1,ptr1);
*/
        if(*ptr1 == CR || *ptr1 == '\0') {
            return(NULL);
        }
        ptr1++;
    }

    while (!(*ptr1 == ' ' || *ptr1 == ',' || *ptr1 == ';'
          || *ptr1 == '+' || *ptr1 == '|' || *ptr1 == TAB
          || *ptr1 == CR  || *ptr1 == '\0') ) {
/*
        printf("\nget_token3():  char = *%c* 0x%-02x 0x%-08x", *ptr1,*ptr1,ptr1);
*/
        *ptr2 = *ptr1;
        ptr1++;
        ptr2++;
    }

    *ptr2 = '\0';
    return(ptr1);
}

 
static ushort mnem_op (char *buf)
{
    int i;
    char comp_flag;
    struct mnemonic_tbl *ptr;
 
/*    printf("mnem_op() output\n"); */
 
    /* compare mnemonic in each table entry, looking for match  */
    for (ptr=Op_tbl; ptr->opcode < END_TBL; ptr++)
    {
        comp_flag = TRUE;
        for (i=0; i <= ptr->num_char; i++) /* compare all chars + 1 null */
        {
            if (buf[i] != ptr->mnem[i])  comp_flag = FALSE;
        }
        if (comp_flag)  /* match found - return opcode  */
        {
            return (ptr->opcode);
        }
    }    
    return (END_TBL);   /* error - mnemonic not found   */
}

 
static boolean tree_asm (char *buf, unsigned *result)
{
    int i;
    char *ptr;			/* return pointer for gets		*/
    char bbuf[22];		/* disassemble mnemonic buffer		*/
    short opcode;		/* assemble: opcode number of mnemonic	*/
    boolean major_opcode;   	/* major opcode found?			*/
    unsigned input1;		/* scanf read parameter			*/
    unsigned temp_instr;	/* Temporary instruction holder		*/
#ifdef DRM
    int ret;			/* read_con() return; 0 or ESC		*/
#endif
 
    ptr = buf;
    temp_instr = *result;
    *result = 0;
    major_opcode = FALSE;

    /* Get from one to three mnemonics, [rd][+| ,;][ck][+| ,;]instr
       where instr represents the mnemonic for a major opcode.
       The required  major opcode must come last.		*/
    for (i = 0; i < 3; i++) {
        ptr = get_token(ptr, bbuf);
        /* If no tokens (user hit return), copy old instruction
           back out as result and return TRUE.  If not enough
           tokens, return FALSE.				*/
        if (ptr == NULL && i == 0) {
            *result = temp_instr;
            return (TRUE);
        } else {
            if (ptr == NULL) {
                return (FALSE);
            }
        }
/*
        printf ("\ntree_asm():  ptr = *%c* 0x%-02x 0x%-08x",
            *ptr, *ptr, ptr);
        ptr = get_token(ptr, bbuf);
*/
        upshift(bbuf, 20);
        opcode = mnem_op(bbuf);
/*
        printf ("\ntree_asm():  opcode = 0x%-04x", opcode);
*/

        if (opcode == -1) {
            return (FALSE);
        } else {
            *result = *result | opcode;
        }
        if (opcode < 0x40) {
            /* major opcode found */
            major_opcode = TRUE;
            break;
        }
    }

/*
    printf ("\ntree_asm():  Done with opcode analysis.");
    printf ("\ntree_asm():  result = 0x%-08x 0x%-08x 0x%-08x",
            *result, *result, result);
*/
    if (!major_opcode) {
        return (FALSE);
    }


    /* Get address hex value (mandatory) */
    ptr = get_token(ptr, bbuf);
    if (ptr == NULL) {
        return (FALSE);
    }

/***
    ret = sscanf(bbuf, "%x", &input1);
***/

    if (!null(bbuf)) {
        if (bbuf[1] == 'x' || bbuf[1] == 'X') {
            bbuf[0] = bbuf[1] = '0';
        }
#ifdef DRM
        input1 = gethexnum(bbuf);
#else
	input1 = parse_hex_long(bbuf);
#endif
    } else {
        return (FALSE);
    }

#ifdef DRM
    if (input1 < 0 || input1 > 0xFFFF) {
#else
    if (input1 > 0xFFFF) {
#endif
        return (FALSE);
    }
    *result = *result | (input1 << 16);


    /* Get data hex value (mandatory) */
    ptr = get_token(ptr, bbuf);
    if (ptr == NULL) {
        return (FALSE);
    }
/*
    printf ("\ntree_asm():  ptr = *%c* 0x%-02x 0x%-08x",
        *ptr, *ptr, ptr);
*/

/***
    ret = sscanf(bbuf, "%x", &input1);
***/
/*
    printf ("\ntree_asm():  ret = %d, input1 = 0x%-08x",
        ret, input1);
*/

    if (!null(bbuf)) {
        if (bbuf[1] == 'x' || bbuf[1] == 'X') {
            bbuf[0] = bbuf[1] = '0';
        }
#ifdef DRM
        input1 = gethexnum(bbuf);
#else
	input1 = parse_hex_long(bbuf);
#endif
    } else {
        return (FALSE);
    }

#ifdef DRM
    if (input1 < 0 || input1 > 0xFF) {
#else
    if (input1 > 0xFF) {
#endif
        return (FALSE);
    }
    *result = *result | (input1 << 8);
/*
    printf ("\ntree_asm():  result = 0x%-08x 0x%-08x 0x%-08x",
        *result, result, &result);
*/


    return (TRUE);

}

 
static void tree_print (union instr inst_union)
{
    int i;
    int num_char;		/* disassemble: number of instr chars	*/
    char bbuf[22];		/* disassemble mnemonic buffer		*/

    num_char = op_mnem(inst_union.fields.opcode, bbuf);

    printf(" ");

    if (!inst_union.fields.read 
        && !inst_union.fields.cksum) {
        printf ("      ");
    } else {
    if (!inst_union.fields.read 
        || !inst_union.fields.cksum) {
            printf ("   ");
        }
    }

    if (inst_union.fields.read) {
        printf ("RD+");
    }
    if (inst_union.fields.cksum) {
        printf ("CK+");
    }
/*
    printf("%-9s", bbuf);
*/
    for (i=0; i < num_char; i++)
        printf("%c", bbuf[i]);

    for (i=num_char; i < 10; i++)
        printf(" ");

    printf ("%-04x %-02x",
        inst_union.fields.addr, inst_union.fields.data);
}

 
static void tree_mem_read (union instr *inst_union, int index)
{
    unsigned short *multibus;
    unsigned short mb_data;

    multibus = (unsigned short *) MULTIBUS_M_ADDR_BASE;

    /* The machine is byte addressed.  Because multibus is a short
       word array, the index is shifted << 1 by the compiler.
       We supply an index of 0x100000 for the start of tree memory
       (really is 0x200000), knowing it will be upshifted.  
       Index is pre-shifted left 3, because each 96 bit tree instruction
       occupies 16 bytes of address space.			*/
    index = (index << 3) + 0x100000;

    mb_data = read_absolute(&multibus[index++]);
    inst_union[0].word = (read_absolute(&multibus[index++]) | (mb_data << 16));
    mb_data = read_absolute(&multibus[index++]);
    inst_union[1].word = (read_absolute(&multibus[index++]) | (mb_data << 16));
    mb_data = read_absolute(&multibus[index++]);
    inst_union[2].word = (read_absolute(&multibus[index++]) | (mb_data << 16));

/***
    printf ("\ntree_mem_read():  0x%-08x 0x%-08x 0x%-08x",
        index, multibus, &multibus[index]);
***/
}

 
static void tree_mem_write (union instr *inst_union, int index)
{
    unsigned short *multibus;
#ifdef DRM
    unsigned short mb_data;
#endif

    multibus = (unsigned short *) MULTIBUS_M_ADDR_BASE;

    /* The machine is byte addressed.  Because multibus is a short
       word array, the index is shifted << 1 by the compiler.
       We supply an index of 0x100000 for the start of tree memory
       (really is 0x200000), knowing it will be upshifted.  
       Index is pre-shifted left 3, because each 96 bit tree instruction
       occupies 16 bytes of address space.			*/
    index = (index << 3) + 0x100000;

    write_absolute(&multibus[index++], inst_union[0].word >> 16);
    write_absolute(&multibus[index++], (ushort)inst_union[0].word);
    write_absolute(&multibus[index++], inst_union[1].word >> 16);
    write_absolute(&multibus[index++], (ushort)inst_union[1].word);
    write_absolute(&multibus[index++], inst_union[2].word >> 16);
    write_absolute(&multibus[index++], (ushort)inst_union[2].word);

/***
    printf ("\ntree_mem_write():  0x%-08x 0x%-08x 0x%-08x",
        index, multibus, &multibus[index]);
***/
}


static unsigned short read_multibus_mem (
    int mbus_addr,			/* multibus byte address input	*/
    unsigned int *mbus_abs_addr)	/* absolute address output	*/
{
    unsigned short *multibus;
    unsigned short mb_data;

    multibus = (unsigned short *) MULTIBUS_M_ADDR_BASE;

    /* The machine is byte addressed.  Because multibus is a short
       word array, the index is shifted << 1 by the compiler.
       Since we are already providing a byte address offset index,
       shift it >> 1 to compensate.				*/
    mbus_addr = mbus_addr >> 1;
    *mbus_abs_addr = (unsigned int) &multibus[mbus_addr];

    mb_data = read_absolute((short *)*mbus_abs_addr);

/***
    printf ("\nread_multibus_mem():  0x%-08x 0x%-08x 0x%-08x 0x%-04x",
        mbus_addr, multibus, &multibus[mbus_addr], mb_data);
***/
    return (mb_data);
}
 

static void write_multibus_mem (
    int mbus_addr,			/* multibus byte address input	*/
    unsigned short mbus_data,		/* multibus data input		*/
    unsigned int *mbus_abs_addr)	/* absolute address output	*/
{
    unsigned short *multibus;

    multibus = (unsigned short *) MULTIBUS_M_ADDR_BASE;

    /* The machine is byte addressed.  Because multibus is a short
       word array, the index is shifted << 1 by the compiler.
       Since we are already providing a byte address offset index,
       shift it >> 1 to compensate.				*/
    mbus_addr = mbus_addr >> 1;
    *mbus_abs_addr = (unsigned int) &multibus[mbus_addr];

    write_absolute((short *)*mbus_abs_addr, mbus_data);

/***
    printf ("\nwrite_multibus_mem():  0x%-08x 0x%-08x 0x%-08x 0x%-04x",
        mbus_addr, multibus, &multibus[mbus_addr], mbus_data);
***/
}
 

static unsigned char read_multibus_io_byte (
    int mbus_addr,			/* multibus byte address input	*/
    unsigned int *mbus_abs_addr)	/* absolute address output	*/
{
    unsigned char *multibus;
    unsigned char mb_data;

    multibus = (unsigned char *) MULTIBUS_IO_ADDR_BASE;

    *mbus_abs_addr = (unsigned int) &multibus[mbus_addr];

    mb_data = read_absolute_byte((unsigned char *)*mbus_abs_addr);

/***
    printf ("\nread_multibus_io_byte():  0x%-08x 0x%-08x 0x%-08x 0x%-02x",
        mbus_addr, multibus, &multibus[mbus_addr], mb_data);
***/
    return (mb_data);
}
 

static void write_multibus_io_byte (
    int mbus_addr,			/* multibus byte address input	*/
    unsigned char mbus_data,		/* multibus data input		*/
    unsigned int *mbus_abs_addr)	/* absolute address output	*/
{
    unsigned char *multibus;

    multibus = (unsigned char *) MULTIBUS_IO_ADDR_BASE;

    *mbus_abs_addr = (unsigned int) &multibus[mbus_addr];

    write_absolute_byte((unsigned char *)*mbus_abs_addr, mbus_data);

/***
    printf ("\nwrite_multibus_io_byte():  0x%-08x 0x%-08x 0x%-08x 0x%-02x",
        mbus_addr, multibus, &multibus[mbus_addr], mbus_data);
***/
}


static unsigned short read_multibus_io (
    int mbus_addr,			/* multibus byte address input	*/
    unsigned int *mbus_abs_addr)	/* absolute address output	*/
{
    unsigned short *multibus;
    unsigned short mb_data;

    multibus = (unsigned short *) MULTIBUS_IO_ADDR_BASE;

    /* The machine is byte addressed.  Because multibus is a short
       word array, the index is shifted << 1 by the compiler.
       Since we are already providing a byte address offset index,
       shift it >> 1 to compensate.				*/
    mbus_addr = mbus_addr >> 1;
    *mbus_abs_addr = (unsigned int) &multibus[mbus_addr];

    mb_data = read_absolute((short *)*mbus_abs_addr);

/***
    printf ("\nread_multibus_io():  0x%-08x 0x%-08x 0x%-08x 0x%-04x",
        mbus_addr, multibus, &multibus[mbus_addr], mb_data);
***/
    return (mb_data);
}
 

static void write_multibus_io (
    int mbus_addr,			/* multibus byte address input	*/
    unsigned short mbus_data,		/* multibus data input		*/
    unsigned int *mbus_abs_addr)	/* absolute address output	*/
{
    unsigned short *multibus;

    multibus = (unsigned short *) MULTIBUS_IO_ADDR_BASE;

    /* The machine is byte addressed.  Because multibus is a short
       word array, the index is shifted << 1 by the compiler.
       Since we are already providing a byte address offset index,
       shift it >> 1 to compensate.				*/
    mbus_addr = mbus_addr >> 1;
    *mbus_abs_addr = (unsigned int) &multibus[mbus_addr];

    write_absolute((short *)*mbus_abs_addr, mbus_data);

/***
    printf ("\nwrite_multibus_io():  0x%-08x 0x%-08x 0x%-08x 0x%-04x",
        mbus_addr, multibus, &multibus[mbus_addr], mbus_data);
***/
}
 

static void local_read_buserr_handler (void)
{
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    printf("\n*** Read timeout at address 0x%-08x", Err_addr);
}


static void local_write_buserr_handler (void)
{
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    printf("\n*** Write timeout at address 0x%-08x", Err_addr);
}


static unsigned char read_absolute_byte (
    unsigned char *abs_addr)		/* absolute address input	*/
{
    unsigned char data;

/***
    if (touch_device(abs_addr)) {
        printf("\nRead timeout at address 0x%-08x", abs_addr);
    }
***/
    Err_addr = (unsigned int)abs_addr;
    abortstate = ABORT_INTERNAL;
    buserrhandler = &local_read_buserr_handler;
    data = *abs_addr;
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    buserrhandler = 0;

/***
    printf ("\nread_absolute_byte():  0x%-08x 0x%-02x", abs_addr, data);
***/
    return (data);
}
 

static void write_absolute_byte (
    unsigned char *abs_addr,	/* absolute address input	*/
    unsigned char data)		/* data input			*/
{

/***
    if (touch_device(abs_addr)) {
        printf("\nWrite timeout at address 0x%-08x", abs_addr);
    }
***/
    Err_addr = (unsigned int)abs_addr;
    abortstate = ABORT_INTERNAL;
    buserrhandler = &local_write_buserr_handler;
    *abs_addr = data;
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    buserrhandler = 0;

/***
    printf ("\nwrite_absolute_byte():  0x%-08x 0x%-02x", abs_addr, data);
***/
}
 

static unsigned short read_absolute (
    short *abs_addr)			/* absolute address input	*/
{
    unsigned short data;

/***
    if (touch_device(abs_addr)) {
        printf("\nRead timeout at address 0x%-08x", abs_addr);
    }
***/
    Err_addr = (unsigned int)abs_addr;
    abortstate = ABORT_INTERNAL;
    buserrhandler = &local_read_buserr_handler;
    data = *abs_addr;
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    buserrhandler = 0;

/***
    printf ("\nread_absolute():  0x%-08x 0x%-04x", abs_addr, data);
***/
    return (data);
}
 

static void write_absolute (
    short *abs_addr,		/* absolute address input	*/
    unsigned short data)	/* data input			*/
{

/***
    if (touch_device(abs_addr)) {
        printf("\nWrite timeout at address 0x%-08x", abs_addr);
    }
***/
    Err_addr = (unsigned int)abs_addr;
    abortstate = ABORT_INTERNAL;
    buserrhandler = &local_write_buserr_handler;
    *abs_addr = data;
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    asm("   nop");
    buserrhandler = 0;

/***
    printf ("\nwrite_absolute():  0x%-08x 0x%-04x", abs_addr, data);
***/
}
 

static void upshift (char *data_buf, unsigned last_byte_index)
{
    unsigned i;

    /* A - Z = 0x41 - 0x5A.  a - z = 0x61 - 0x7A. */

    /* Read through all output bytes and maybe upshift  */
    for (i=0; i<last_byte_index; i++)
    {
        if(data_buf[i] > 0x60 && data_buf[i] < 0x7B) {
            data_buf[i] = data_buf[i] & 0xDF;
        }
/*
        printf("%2c,", data_buf[i]);
*/
    }
}


static ushort cmd_op (char *buf)
{
    int i;
    char comp_flag;
    struct command_tbl *ptr;
 
/*    printf("cmd_op() output\n"); */
 
    /* compare command in each table entry, looking for match  */
    for (ptr=Cmd_tbl; ptr->command < END_TBL; ptr++)
    {
        comp_flag = TRUE;
        for (i=0; i <= ptr->num_char; i++) /* compare all chars + 1 null */
        {
            if (buf[i] != ptr->mnem[i])
                comp_flag = FALSE;
        }
        if (comp_flag)  /* match found - return command  */
        {
            return (ptr->command);
        }
    }    
    return (END_TBL);   /* error - mnemonic not found   */
}

 
/*
 * ex_sp_cmd_l()
 * Issue command to SP and read result.  writes 32 bit argument to largreg.
 */
 
static ushort ex_sp_cmd_l (register mciregtype *sp_regptr, ushort cmd,
			   uint larg)
{
    ushort sresult, response;
   
    sp_regptr->largreg = larg;
    sp_regptr->cmdreg = cmd;
    response = sp_regptr->cmdreg;
    sresult = sp_regptr->argreg;
   
    if (response != FCI_RSP_OKAY)   {
        printf("\n*** ex_sp_cmd_l error: Argreg: 0x%-04x, Cmdreg: 0x%-04x",
            sresult, response);
        printf("\n    command was: 0x%-04x  Argument was: 0x%-08x", cmd, larg);
        return (-1);
    }
    return (sresult);
}

 
/*
 * get_sp_reg_addr()
 * Issue command to SP and read result.  writes 32 bit argument to largreg.
 */
 
static mciregtype * get_sp_reg_addr (
     register mciregtype *sp_regptr,
     char *read_buf,		/* input buffer			*/
     unsigned *slot)		/* user entered slot number	*/
{
    boolean parsed;	/* gets return				*/

    if (sp_regptr == 0) {
        printf ("\nSP slot?:  ");
#ifdef DRM
	parsed = (int) gets(read_buf);
#else
	parsed = (int) rdtty(read_buf, sizeof(read_buf));
#endif

        if (parsed) {
            if (!null(read_buf)) {
                if (read_buf[1] == 'x' || read_buf[1] == 'X') {
                    read_buf[0] = read_buf[1] = '0';
                }
#ifdef DRM
                *slot = gethexnum(read_buf);
#else
		*slot = parse_hex_long(read_buf);
#endif
                sp_regptr = (mciregtype *) (0x1110C000 +
                    *slot * 0x40);
            }
        }
    }
/***
    printf ("\nSlot = 0x%-04x 0x%-04x", *slot, sp_regptr);
***/
    return (sp_regptr);
}


static unsigned short read_memd (
    mciregtype *sp_regptr,	/* SP register address space on mbus	*/
    int memd_addr)		/* memD word address input	*/
{
    unsigned short data;

    data = ex_sp_cmd_l(sp_regptr, FCI_CMD_MEMD_SELECT, memd_addr);
    data = sp_regptr->readshort;

/**
    printf ("\nread_memd():  0x%-06x 0x%-04x", memd_addr, data);
**/
    return (data);
}


static void write_memd (
    mciregtype *sp_regptr,	/* SP register address space on mbus	*/
    int memd_addr,		/* memD word address input	*/
    unsigned short memd_data)	/* memD data input		*/
{

    ex_sp_cmd_l(sp_regptr, FCI_CMD_MEMD_SELECT, memd_addr);
    sp_regptr->write0short = memd_data;

/**
    printf ("\nwrite_memd():  0x%-06x 0x%-04x", memd_addr, memd_data);
**/
}


