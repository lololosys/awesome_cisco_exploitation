/* $Id: svip_cyatest.c,v 3.1.2.2 1996/08/12 16:07:38 widmer Exp $
 * $Source: /release/112/cvs/Xsys/src-svip/svip_cyatest.c,v $
 *------------------------------------------------------------------
 * svip_cyatest.c - DMA Engine test support for Viper2
 *
 * Juin 1996, Oumar Ndiaye
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "interface_private.h"
#include "sched.h"
#include "stacks.h"
#include "packet.h"
#include "ttysrv.h"
#include "config.h"
#include "parser.h"
#include <ctype.h>
#undef   toupper
#include <ciscolib.h>

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../src-svip/svip.h"
#include "../src-svip/svip_pcmap.h"
#include "../src-svip/svip_asic.h"

#define	ALTERNATE	NONE

EOLNS    (cya_test_bringup_eol, cya_test_bringup);

KEYWORD (cya_test_bringup, cya_test_bringup_eol, no_alt,
	 "bringup", "Test cya bringup",
	 PRIV_ROOT);

KEYWORD_MM (test_cya, cya_test_bringup, no_alt,
	    "cya", "CYA test system",
 	    PRIV_ROOT, 3); 

#undef  ALTERNATE
#define ALTERNATE test_cya

LINK_POINT(test_cya_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * CYA Parse chains
 */

static parser_extension_request cya_chain_init_table[] = {
    { PARSE_ADD_TEST_CMD, &pname(test_cya_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * cya_ui_init :
 * Initialize CYA specific parse chains
 */
static void cya_ui_init (subsystype *subsys)
{
    /*
     * Hook in the CYA parse chains
     */
    parser_add_command_list(cya_chain_init_table,"CYA system commands");
}


/*
 * cya test subsystem header
 */
 
#define CYA_MAJVERSION    1
#define CYA_MINVERSION    0
#define CYA_EDITVERSION   1
 
SUBSYS_HEADER(cya, CYA_MAJVERSION, CYA_MINVERSION,
              CYA_EDITVERSION,
              cya_ui_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);

#define BUFLEN 			80


/*
 * CYA bring up debug code starts here. ondiaye
 */

typedef struct debug_double_word_ {
    ulong :32 ;
    volatile ulong reg;
} debug_double_word_t;

static debug_double_word_t *debug_pmem_start, *debug_reg_start, 
    *debug_xmem_start, *debug_ymem_start;

#define  QE_RUN_BIT  0x4

int debug_stop_qe (void)

{
    ulong cya_temp;
    printf ("debug_stop_qe\n");
    cya_temp= CYA_ASIC->qe.diag.opt;
    if( cya_temp & QE_RUN_BIT ){
        CYA_ASIC->qe.diag.opt =cya_temp &(~QE_RUN_BIT);
	return(1);
    }else {
        return(0);
    }
}

#define QE_JS_WAIT_DISABLE_BIT 0x20

void debug_start_qe (void)

{
    ulong cya_temp;
    printf ("debug_start_qe\n");
    cya_temp= CYA_ASIC->qe.diag.opt;
    CYA_ASIC->qe.diag.opt = cya_temp | QE_RUN_BIT | QE_JS_WAIT_DISABLE_BIT;
}

#define   QE_DEBUG_PC_BIT 0x10

void debug_eeprom_qe (void)

{
    ulong cya_temp;
    printf ("debug_eeprom_qe\n");
    cya_temp=  CYA_ASIC->qe.diag.opt;
    CYA_ASIC->qe.diag.opt = cya_temp | QE_DEBUG_PC_BIT;
}

void init_cya_debug(void)

{
    printf ("\ninit_cya_debug\n");
    debug_pmem_start= (debug_double_word_t *) CYA_ASIC->qe_pgm_mem;
    printf ("cya pmem start= %0x\n", debug_pmem_start); 
    debug_reg_start = (debug_double_word_t *) &(CYA_ASIC->qe);
    printf ("cya reg start= %0x\n", debug_reg_start);
    debug_xmem_start= (debug_double_word_t *) &(CYA_ASIC->qe.xmem);
    printf ("cya xmem start= %0x\n", debug_xmem_start);
    debug_ymem_start= (debug_double_word_t *) &(CYA_ASIC->qe.ymem);
    printf ("cya ymem start= %0x\n", debug_ymem_start);
}

void cya_dump_reg (void)

{
    char *registers1[]={"acc", "ix", "rtc",  "ix_sel", "jix", "cmd_reg", "", 
			 "diag_options", "diag_sstep", "", "", "", "a_op", 
			 "b_op", "", "pc", "diag_cmd", "diag_cmd_vld", 
			 "diag_cmd_rsp", "hi_int_cause", "lo_int_cause"};
    int i, qe_was_running;
    ulong reg_data;
    debug_double_word_t *reg_addr;
    printf ("cya_dump_reg\n");
    
    qe_was_running= debug_stop_qe();
    reg_addr= debug_reg_start;
    printf ("\n QE Internal Registers:\n");
    for (i=0; i<6; i++) { /* from acc */
        reg_data= reg_addr->reg; 
	printf ("%s: 0x%08x\n", registers1[i], reg_data);
	reg_addr++;
    }
    reg_addr++;
    for (++i; i<8; i++) { /* from diag_option */
        reg_data= reg_addr->reg; 
	printf ("%s: 0x%08x\n", registers1[i], reg_data);
	reg_addr++;
    }
    reg_addr +=4;
    for (i=i+4; i<14; i++) { /* from a_op */
        reg_data= reg_addr->reg; 
	printf ("%s: 0x%08x\n", registers1[i], reg_data);
	reg_addr++;
    }
    reg_addr ++;
    for (++i; i<21; i++) { /* from pc */
        reg_data= reg_addr->reg; 
	printf ("%s: 0x%08x\n", registers1[i], reg_data);
	reg_addr++;
    }
    if (qe_was_running)
        debug_start_qe();
}

void cya_dump_xmem (void)

{
    int i, qe_was_running, xmem_zie=64;  
    ulong reg_data;
    debug_double_word_t *reg_addr;

    printf ("cya_dump_xmem\n");
    qe_was_running= debug_stop_qe();
    reg_addr= debug_xmem_start;
    printf ("\n QE Xmem Content:\n");
    for (i=0; i<xmem_zie; i++) {
        reg_data= reg_addr->reg; 
	printf ("0x%2x: 0x%08x\n", 0x80+i, reg_data);
	reg_addr++;
    }
    if(qe_was_running)
        debug_start_qe();
}  

void cya_dump_ymem(void)

{
    int i, qe_was_running, ymem_zie=64;  
    ulong reg_data;
    debug_double_word_t *reg_addr;

    printf ("cya_dump_ymem\n");
    qe_was_running= debug_stop_qe();
    reg_addr= debug_ymem_start;
    printf ("\n QE Ymem Content:\n");
    for (i=0; i<ymem_zie; i++) { 
        reg_data= reg_addr->reg; 
	printf ("0x%2x: 0x%08x\n", 0xc0+i, reg_data);
	reg_addr++;
    }
    if(qe_was_running)
        debug_start_qe();
}  
/* ondiaye */
#define DEC21140_CSR6_TX_TR_1024		0x0000C000
#define DEC21140_CSR6_TX_TR_512			0x00008000
#define DEC21140_CSR6_TX_TR_256			0x00004000
#define DEC21140_CSR6_TX_TR_128			0x00000000
ulong ondiaye_csr6_15_14= DEC21140_CSR6_TX_TR_128; /* will be set in svip_cyatest.c */

void cya_sstep(void)

{
    char cmdchar;
    int buflen=80;
    char buff[buflen];
    boolean parsed;
    int i, qe_was_running, flag=1, num_sstep=0;
    ulong inst=0, prev_pc, a_op, b_op;
    debug_double_word_t *pmem_addr;
    
    qe_was_running= debug_stop_qe();
    
    printf ("\nEnter one of the following Single Step options:");
    printf ("\n1     - single step 1 instruction");	    
    printf ("\n2     - single step 20 instruction");	    
    printf ("\n3     - single step 50 instruction");	    
    printf ("\n4     - single step 100 instruction");	    
    printf ("\n5     - single step 200 instruction");	    
    printf ("\n6     - single step 500 instruction");	    
    printf ("\n7     - single step 1000 instruction");	    
    printf ("\n?     - to print this help message\n");	    
    
    cmdchar = '?';  
    while (flag) {
        bzero(buff, buflen);
	parsed = rdtty(buff, buflen-1);
	if (!null(buff) && parsed)
	    cmdchar = buff[0]; /* don't convert to upper case */
	if (stdio->statbits & (IDLETIMEOUT | HANGUP | CARDROP | IDLE))
	    return;
	automore_enable(NULL);
	switch (cmdchar) {
	case '?':
	  printf ("\nEnter one of the following options:");
	  printf ("\n1     - single step 1 instruction");	    
	  printf ("\n2     - single step 20 instruction");	    
	  printf ("\n3     - single step 50 instruction");	    
	  printf ("\n4     - single step 100 instruction");	    
	  printf ("\n5     - single step 200 instruction");	    
	  printf ("\n6     - single step 500 instruction");	    
	  printf ("\n7     - single step 1000 instruction");	    
	  printf ("\n8     - single step 5000 instruction");	    
	  printf ("\n9     - single step 10000 instruction");	    
	  printf ("\n?     - to print this help message\n");	    
	  break;
      
	case '1':
	  num_sstep= 1;
	  flag=0;
ondiaye_csr6_15_14= DEC21140_CSR6_TX_TR_128;
	  break;
	  
	case '2':
	  num_sstep= 20;
	  flag=0;
ondiaye_csr6_15_14= DEC21140_CSR6_TX_TR_256;
	  break;
	  
	case '3':
	  num_sstep= 50;
	  flag=0;
ondiaye_csr6_15_14= DEC21140_CSR6_TX_TR_512;
	  break;
	  
	case '4':
	  num_sstep= 100;
	  flag=0;
ondiaye_csr6_15_14= DEC21140_CSR6_TX_TR_1024;
	  break;
	  
	case '5':
	  num_sstep= 200;
	  flag=0;
	  break;
	  
	case '6':
	  num_sstep= 500;
	  flag=0;
	  break;
	  
	case '7':
	  num_sstep= 1000;
	  flag=0;
	  break;
	  
	case '8':
	  num_sstep= 5000;
	  flag=0;
	  break;
	  
	case '9':
	  num_sstep= 10000;
	  flag=0;
	  break;
	  
	default:
	  printf ("\nunknown option \"%c\"", cmdchar);
	  cmdchar = '?';
	  break;
	  
      }
    }
    printf("cya_sstep: Number of steps= %d:\n", num_sstep);

    if (num_sstep) {
      /* single step starts here */
        prev_pc= CYA_ASIC->qe.diag.pc;
	printf ("  %s:  %s:    %s         %s\n", "pc", "inst", "a_op", "b_op");
	for (i=0; i< num_sstep; i++) {
	    CYA_ASIC->qe.diag.ss= 1;
	    a_op= CYA_ASIC->qe.diag.aop;
	    b_op= CYA_ASIC->qe.diag.bop;
	    pmem_addr= debug_pmem_start + prev_pc;
	    inst= pmem_addr->reg;
	    printf("0x%03x:0x%06x:  0x%08x   0x%08x\n", prev_pc, inst, a_op, b_op);
	    prev_pc= CYA_ASIC->qe.diag.pc;    
	}
    }
    if(qe_was_running)
        debug_start_qe();
}

void cya_io (void)

{
    char buff[BUFLEN];
    boolean parsed;
    uint bytes = 4;
    ulong addr = 0;
    ulong last_addr = 0;
    ulong data = 0;
    char rw = 'R';
    int qe_was_running;

#define CYA_START_ADDR  0x10048004
#define CYA_END_ADDR  0x1004986c

    /*
     * Loop until the user quits or there is an error
     */
    
    while (TRUE) {

        addr = last_addr;
        printf ("\n  CYA Physical Address , [0x%08x]? :  ", addr);
	parsed = rdtty(buff, BUFLEN);
	
	if ( toupper(buff[0]) == 'Q') {
	    return;
	}

	if (!null(buff) && parsed) {
	    addr = parse_unsigned(buff,&parsed);
	    last_addr = addr;
	}
	if ( ( addr < CYA_START_ADDR) || ( addr > CYA_END_ADDR ) ) {
	    printf ("\nInvalid Physical address 0x%08x", addr);
	    break;
	}	      

	printf ("\n  Read or Write at Offset 0x%08x [%c]? :  ",addr, rw);
	parsed = rdtty(buff, BUFLEN);
	if (!null(buff) && parsed) 
	    rw = toupper(buff[0]);
	
       	if (rw == 'R') {
	  
	    printf ("\n  Number of bytes to read [%d]?:  ", bytes);
	    
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed) {
	        bytes = parse_unsigned(buff,&parsed);
		if ((bytes > 4) || (bytes < 1)){
		    printf ("\n%%Illegal value \n");
		    return;
		}
	    }

	    /* get  the cya virtual address */
	    addr = ( (ulong) CYA_ASIC & 0xFFFF0000 ) | ( addr & 0x0000FFFF );
	    qe_was_running= debug_stop_qe();
	    
	    switch (bytes) {
	    case 1:
	      data = *((uchar *)addr);
	      break;
	    case 2:
	      data = *((ushort *)addr);
	      break;
	    case 3:
	      printf ("\nnot supported");
	      break;
	    case 4:
	      data = *((ulong *)addr);
	      break;
	    default:
	      printf ("\nnot supported");
	      break;
	      
	    }
	    if ( qe_was_running ) {
	        debug_start_qe();
	    }
	    printf ("\n  Read 0x%x, from address 0x%08x\n",
		    data, addr);
	    
       } else if (rw == 'W') {
	   printf ("\n  Number of bytes to write [%d]?:  ", bytes);
	   
	   parsed = rdtty(buff, BUFLEN);
	   if (!null(buff) && parsed) {
	       bytes = parse_unsigned(buff,&parsed);
	       if ((bytes > 4) || (bytes < 1)){
		   printf ("\n%%Illegal value \n");
		   return;
	       }
	   }

	   printf ("\n  Write data  [%x]?:  ", data);
	   
	   parsed = rdtty(buff, BUFLEN);
	   if (!null(buff) && parsed) {
	       data = parse_unsigned(buff,&parsed);
	   }

	   printf ("\n  Writing 0x%x, to address 0x%08x\n",
		   data, addr);

	   /* get  the cya virtual address */
	   addr = ( (ulong ) CYA_ASIC & 0xFFFF0000 ) | ( addr & 0x0000FFFF );
	   qe_was_running= debug_stop_qe();
	   
	   switch (bytes) {
	   case 1:
	     *((uchar *)addr) = data;
	     break;
	   case 2:
	     *((ushort *)addr) = data;
	     break;
	   case 3:
	     printf ("\nnot supported");
	     break;
	   case 4:
	     *((ulong *)addr) = data;
	     break;
	   default:
	     printf ("\nnot supported");
	     break;
	     
	   }
	   if ( qe_was_running ) {
	       debug_start_qe();
	   }
       } else {
	   printf ("\n%%Invalid selector\n");
	   return;
       }
    }
}

void cya_test_bringup (parseinfo *csb)

{
    char cmdchar;
    int buflen=80;
    char buff[buflen];
    boolean parsed;
    
    init_cya_debug();
    printf ("\n\nCYA test commands: type ? for help\n");
    cmdchar = '?';
    while (TRUE) {
        bzero(buff, buflen);
	parsed = rdtty(buff, buflen-1);
	if (!null(buff) && parsed)
	    cmdchar = toupper(buff[0]);
	if (stdio->statbits & (IDLETIMEOUT | HANGUP | CARDROP | IDLE))
	    return;
	automore_enable(NULL);
	switch (cmdchar) {
	case 'H':
	case '?':
	    printf ("\n");
	    printf ("\nG     - dump reGisters");	    
	    printf ("\nX     - dump Xmem");
	    printf ("\nY     - dump Ymem");
	    printf ("\nS     - Single Step cya");
	    printf ("\nQ     - quit\n");
	    printf ("\nI     - cya I/O\n");  
	    break;
	case 'G':
	    cya_dump_reg();
	    cmdchar = '?';
	    break;

	case 'X':
	    cya_dump_xmem();
	    cmdchar = '?';
	    break;
      
	case 'Y':
	    cya_dump_ymem();
	    cmdchar = '?';
	    break;
      
	case 'S':
	    cya_sstep();
	    cmdchar = '?';
	    break;

	case 'I':
	    cya_io();
	    cmdchar = '?';
	    break;
      
	case 'Q':
	    return;
	default:
	    printf ("\nunknown command \"%c\"", cmdchar);
	    cmdchar = '?';
	    break;
	}
	automore_disable();
    }
}


/*********************** CYA TEST CODE ENDS HERE *********************/


/* end of file */






