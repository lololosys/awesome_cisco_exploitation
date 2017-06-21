/* $Id: config.h,v 3.4.4.13 1996/08/13 18:46:42 widmer Exp $
 * $Source: /release/112/cvs/Xsys/h/config.h,v $
 *------------------------------------------------------------------
 * config.h -- definitions for configuration command parsing
 *
 * 11-June-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: config.h,v $
 * Revision 3.4.4.13  1996/08/13  18:46:42  widmer
 * CSCdi65503:  security hole when using httpd and no enable passwd is set
 * Branch: California_branch
 * Make http authentication work with "enable use-tacacs"
 *
 * Revision 3.4.4.12  1996/08/07  08:58:31  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.4.4.11  1996/07/12  02:01:04  widmer
 * CSCdi52582:  parser should remove the trailing spaces in the filename
 * Branch: California_branch
 *
 * Revision 3.4.4.10  1996/06/17  23:30:19  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.4.4.9  1996/06/06  04:25:23  widmer
 * CSCdi59599:  Add printf style formatting to parse_line
 * Branch: California_branch
 *
 * Revision 3.4.4.8  1996/05/29  14:02:39  bew
 * CSCdi56794:  IOS private configuration not always recovered from NVRAM
 * Branch: California_branch
 * Move the nvtype_private structure reference from config.h to types.h.
 *
 * Revision 3.4.4.7  1996/05/19  00:24:24  rpratt
 * CSCdi57306:  HTML config has problems in new platform
 * Branch: California_branch
 *
 * Revision 3.4.4.6  1996/05/17  11:15:20  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.2.4  1996/05/05  23:04:54  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.3.2.3  1996/04/26  07:52:39  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.3.2.2  1996/04/03  14:25:05  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.2.1  1996/03/17  17:47:53  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.4.4.5  1996/05/09  14:12:57  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.4.4.4  1996/04/27  05:46:50  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.4.4.3.4.1  1996/04/27  06:35:08  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.4.4.3  1996/04/19  15:15:06  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.4.4.2  1996/04/15  14:57:35  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.4.4.1.8.1  1996/04/08  01:45:43  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.4.4.1  1996/03/18  19:36:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.8.1  1996/03/25  02:18:52  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4  1996/03/09  22:54:45  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.3  1996/02/13  08:11:34  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.2.26.1  1996/01/24  21:43:41  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2.40.1  1996/03/22  03:15:31  bbenson
 * Branch: LE_Cal_V111_1_0_3_branch
 * Rearrange cadillac fins on the VTP command parser.  Install
 * electroglide subcommand modes for vtp-domain and vtp-database commands.
 * Running lights coming in next commit.
 *
 * Revision 3.2  1995/11/17  09:11:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:34:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/15  23:24:40  tli
 * CSCdi36981:  Need to be able to generate downward compatible
 * configurations
 *
 * Revision 2.2  1995/06/09  23:39:41  achopra
 * CSCdi04836:  Provide ip-on-break functionality on TS
 *
 * Revision 2.1  1995/06/07  20:34:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "address.h"
#include "linktype.h"

#define SABUFSIZE 3000			/* buffer size for config from term */
#define TOKLEN 85			/* maximum token length */
#define WORD 0				/* gettoken should return a word */
#define LINE 1				/* gettoken should return a line */


/*
 * flags used in csb->flags
 */
#define CONFIG_TERM		0x00000001 /* configuring from a terminal */
#define CONFIG_ABORT		0x00000002 /* need to abort the config */
#define CONFIG_DONE		0x00000004 /* config is done */
#define	CONFIG_HTTP		0x00000008 /* configuring from http */
#define	KEYWORD_MATCHED		0x00000010 /* keyword was matched */
#define	COMMAND_AUTHORIZE	0x00000020 /* expand command into nv_command */
#define	COMMAND_FUNCTION	0x00000040 /* substitute function on accept */
#define	HIDE_PASSWORDS		0x00000080 /* Don't print password during nvgen */
#define	PARSE_EMPTY_STRING	0x00000100 /* Try to parse empty string */

#define NOMEANSZERO	1	/* if "no", assume number 0 */
#define NONULL		2	/* null args not allowed */
#define NOMEANSMINUS	3	/* if "no", assume number -1 */
#define NOMEANSNULL	4	/* if "no", assume null string */
#define NULLMEANSDONE	5	/* if NULL, quiet error return */

#define IN		1	/* parsed "in" keyword */
#define OUT		2	/* parsed "out" keyword */

/*
 * Methods for resolving protocol addresses and configuration information.
 * Note that this list has grown to be used for lots more than resolving 
 * protocol addresses, however.
 */
enum RES {
    RES_NULL,			/* Not resolved */
    RES_HEURISTIC,		/* Heuristics (usually 3MB PUP) */
    RES_RARP,			/* Reverse ARP */
    RES_BOOTP,			/* Boot Protocol */
    RES_CONFIG,			/* Configuration file */
    RES_MANUAL,			/* Manually set */
    RES_NONVOL,			/* Read from non-volatile RAM */
    RES_PRIVATE_NONVOL,		/* Read from private non-volatile RAM */
    RES_WRITING_NONVOL		/* Writing to non-volatile RAM */
};

/*
 * structures for nonvolatile configuration memory
 */

#define NVMAGIC	      0xABCD
#define NVVERSION          1
#define NVCOMPRESSVERSION  2

#define UNIX_COMPRESS      1

struct nvtype_ {
    ushort magic;		/* magic number config image present */
    ushort version;		/* version of NVRAM config structure */
    ushort checksum;		/* checksum for NVRAM config image   */
    ushort system_version;	/* major version of system image     */
    				/*  that created the config in NVRAM */
    uchar *textbase;		/* ptr to start of config text       */
    uchar *textptr;		/* ptr to end of config text         */
    long textsize;
                                /* Note: fields after "textsize" are */
                                /*  ONLY to be used when             */
                                /*  nv->version == NVCOMPRESSVERSION */
    uchar *v1_ptr;              /* place holder for version 1        */
    boolean compressed;         /* TRUE if config is compressed      */
    ushort compress_type;       /* type of compression               */
    long uncompress_size;       /* uncompressed size of config       */
				/* Note: fields after "uncompress_size" */
				/*  are ONLY to be used when         */
				/*  nv->version == NVPRIVATEVERSION  */
};
    
#define NVMAGIC_PRIV	0xFEDC
#define NVVERSION_1_PRIV     1

struct nvtype_private_ {
    ushort magic;		/* priv magic number, shows if present */
    ushort version;		/* version of priv struct */
    uchar *priv_textbase;	/* ptr to start of private config text*/
    uchar *priv_textptr;	/* ptr to end of private config text  */
    long priv_textsize;
};

/*
 * Prototype for NVRAM routine
 */
extern void nv_bcopy(void const *, void *, int);


/*
 * now define a generic named thing.
 */
typedef struct namedthingtype_ {
    struct namedthingtype_ *next;
    NAMEDTHINGMACRO
    uchar data[1];
} namedthingtype;


/************************************************************************/
/*********** START OF NEW PARSER STRUCTURE ******************************/
/************************************************************************/


#define PARSEBUF	256	/* Input buffer length */
#define MAXTOKLEN	21	/* Maximum token length */
#define MAXPROMPTLEN	30	/* Maximum prompt; prompt truncated to fit */

/* Routing process name and its AS or Tag
 */

struct rtrname {
  const char *myname;	   /* Pointer to the routing process name */
  uint	protocol;	   /* Routing process type */
  boolean arg_specified;   /* Indicate whether AS#, pid, or Tag was parsed */
  uint  as;		   /* AS number for IP routing processes that use it */
  char tag[MAXTOKLEN+1];   /* ISO tag */
};

/* Structure for saving blocks of character strings like comments,
 * help output, and NV generation.
 */
typedef struct save_buf_ {
  char *buf;		/* The buffer itself */
  int size;		/* Size of the malloc'ed buffer area */
  int used;		/* Number of characters stored in the buffer */
} save_buf;

/*
 *
 */
typedef struct ambig_struct_ {
    char ambig[PARSEBUF];
    char ambig_save[PARSEBUF];
    int multiple_funcs;
} ambig_struct;

/* The 'transition' struct is the central data structure of the parser.
 * A parse tree is built of transition nodes, each of which references
 * an action routine (func() in the structure).  The action routine
 * will transition to the 'accept' node if the input was matched (or
 * some other action was successful).  The 'alternate' transition is taken
 * if the action failed, and after the accepting node returns (recursion
 * is currently used to transition between nodes).  The 'args' variable
 * may be used to pass additional parameters to the action routine.  It is
 * generally defined as one of several types of structures to allow passing
 * multiple parameters.
 *
 * The parse_token() function handles transitioning between nodes.
 */

typedef void (*trans_func)(parseinfo *, transition *, const void * const);
struct transition_ {
    transition *accept;
    transition *alternate;
    const trans_func func;
    const void * const args;
};

/*
 * Dynamic transitions used for adding parse chains at run time
 */
typedef struct dynamic_transition_ dynamic_transition;
struct dynamic_transition_ {
    dynamic_transition *accept;
    dynamic_transition *alternate;
    const trans_func func;
    const void *args;
};

typedef void (*command_func)(parseinfo *);

/* The parseinfo struct is used by the parser routines for
 * parameter storage between the parser and command action routines.
 * The parser and action functions must agree as to where 
 * values are stored in this struct.  Generally, this is
 * in order in which the values appear in the input stream.
 *
 * Parser internal state info is also kept here.
 */
#define UIARRAY_SIZE 10			/* # of elements in csb->uiarray */
struct parseinfo_ {
/*** Start of cleared area ***/

/*** Note that csb_end_data is defined as txt1 below! ***/


  /* Variable storage for parsed data being passed to functions */
  char txt1[PARSEBUF], txt2[PARSEBUF], txt3[PARSEBUF];	/* String data */
  char txt4[PARSEBUF], txt5[PARSEBUF], txt6[PARSEBUF];	/* String data */
  hwaddrtype hwaddr1, hwaddr2;		/* Hardware addresses */
  hwaddrtype hwaddr3, hwaddr4;		/* Hardware addresses */
  MaskedAddress maskedAddr1, maskedAddr2; /* masked addresses */
  addrtype addr1, addr2, addr3, addr4;	/* Protocol addresses */
  addrtype addr5, addr6, addr7, addr8;	/* Protocol addresses */
  addrtype addr9, addr10;		/* Protocol addresses */
  uint val1, val2, val3, val4, val5;	/* Integer values */
  uint val6, val7, val8, val9, val10;
  uint val11, val12, val13, val14, val15;
  uint val16, val17, val18, val19, val20;
  uint val21, val22;
  uint uiarray[UIARRAY_SIZE];		/* Integer array */
  struct idbtype_ *idb1;		/* IDB for parsing interfaces */
  struct cdbtype_ *cdb1;		/* CDB for parsing controllers */
  void *pdb1;				/* PDB for parsing router processes */
  void *udb1, *udb2;			/* UDB(s) for user defined generic subcommands */
  route_maptype *routemap1;		/* route-map for parsing subcommands */
  /* router_name is a temporary hold-over until the ROUTER macro and action
   * routine find and return a pointer in csb->pdb1.
   */
  struct rtrname router_name;
  int stungroup;			/* Selected stun group for parsing
					 * stun addresses
					 */
  int bstungroup;			/* Selected bstun group for parsing
					 * bstun addresses
					 */
  struct idbtype_ * (*createidb)(uint);
  uint createidb_unit;
  uint createidb_var;
  boolean leave_subcommand;
  parser_mode *pmp;
  uint appn_subcommand_type;            /* type of appn subcommand */
  boolean subif_specified;              /* TRUE if user entered .n */
  uint end_of_data;			/* Dummy to mark end of data area */

/*** End of cleared area ***/

  /* The below two variables are used in determining the command action
   * routine to execute.
   * 'which' is the command type code passed in the EOL structure and is
   * used by the called function to switch out to the command action routine.
   * An example is CMD_CLEAR, which causes the clear_command() function to
   * be called.
   * 'func' is a function code which determines which of several command
   * functions was requested.  An example is CLEAR_VINES_ROUTE which is
   * used by the clear_command() function to decide which action to take.
   */
  uint which;				/* Which command type being executed */
  uint func;				/* Command function being executed */

  boolean permanent;			/* Permanent (TRUE) or
					 * temporary (FALSE) setting
					 */
  boolean sense;			/* FALSE if we parsed a 'no' prefix.  Also
					 * FALSE if a 'default' prefix parsed
					 * because the off state is the default
					 * for most commands.
					 */
  boolean set_to_default;		/* TRUE if we parsed a 'default'
					 * prefix
					 */
/* These variables keep track of config-mode state information.
 * They're only valid inside a subcommand mode (i.e. interface
 * only has meaning during an interface configuration).
 */
  struct idbtype_ *interface;	/* The current interface */
  cdbtype *controller;		/* The current controller */
  void *protocol;		/* ptr to protocol descriptor block (PDB) */
  int prototype;		/* PDB type (PROC_IP or PROC_OSI) */
  route_maptype *route_map;     /* ptr to route-map entry */
  void *userDataBlock[2];	/* ptr to user defined descriptor block (UDB) */
  
  int line1;			/* Starting tty line number */
  int line2;			/* Ending tty line number */
  uint appn_object_type;        /* appn object type */
  void *appn_object_verb;       /* pointer to appn verb */
  hub_t *hub;			/* The current hub */
  int hub_port1;		/* Starting hub port number */
  int hub_port2;		/* Ending hub port number */
  pgen_context *pgendata;	/* Packet generator context */
  void *parse_line_pointer;	/* Pointer to data space used in
				 * parse_line to return parsed information
				 */
  maplist_type *maplist;        /* Current map list */
  mapclass_type *mapclass;      /* Current map class */
  void *laneLecsCfgTbl;		/* ATM Lan Emulation LECS cfg table */
  void *vtp_domain;		/* Context pointer for modifying a domain */

  int resolvemethod;			/* Config source: term, mem, or net */
  uint flags;				/* misc flags */

  /* Batch input buffer.  NULL pointer if no batch buffer. */
  char *batch_buf;
  char *batch_offset;			/* Current offset into batch buffer */

  /* NV generation variables */
  boolean nvgen;			/* We're doing NV generation */
  char nv_command[PARSEBUF];		/* The command being generated */

  /* Address being used for HTTP configuration */
  httpstate *http_state;
  /* Retain mode name after exit before it's lost on exit_config_command */
  const char *http_exited_name;	

  /* Structures for saving comments, help output, and NV generation */
  save_buf comment_save, help_save;
  int saved_help_used;			/* How many bytes of help we've seen */

  /* Parser internal state info */

  /* The number of commands which matched the input stream,
   * the function to call if only one matched, and a pointer to any
   * additional parseinfo structures we created.
   */
  queuetype *tokenQ;			/* Token processing queue. This is a
					 * push-down queue.  The top of the 
					 * stack is the next element to
					 * process.
					 */
  ambig_struct visible_ambig;
  ambig_struct hidden_ambig;

  boolean command_visible;	/* TRUE if current keyword is visible */
  boolean unsupported;		/* TRUE if current keyword is unsupported */

  void (* action_func)(parseinfo *);	/* Command function to call */
  struct parseinfo_ *next;		/* Next structure, or NULL */

  int in_help;				/* We're processing a help request */
  uchar priv_set;			/* If non-zero, then set keywords to
					 * this privilege level.
					 */
  uchar priv;				/* User's current privilege level */
  uchar highest_priv;			/* Highest keyword priv level parsed */
  uchar last_priv;			/* Privilege level of last keyword */
  queuetype *privQ;			/* stack of transitions that will have
					 * their privilege level changed */
  queuetype *priv_nvgenQ;		/* stack for doing NV generation */
  int exit_all;				/* Exit from router flag */
  char prompt[MAXPROMPTLEN+1];		/* Command line prompt */
  char line_buffer[PARSEBUF];		/* String being consumed */
  char *line;				/* Pointer to string being consumed */
  int line_index;			/* Current token ptr into line */
  int error_index;			/* Position of error in input line */
  char *break_chars;			/* Texti break character set */
  char lastchar;			/* last char read by readline() */
  parser_mode *mode;			/* Current parser mode */
  command_func command_function;	/* Substitute function on accept */
  boolean down_gen_failure;		/* TRUE if downward compatible */
					/* nv generation fails */
};

/* Start and end of variable area (for clearing purposes) */

#define csb_start_data txt1	/* First address to clear */
#define csb_end_data end_of_data	/* Last address + 1 to clear */

#define PFLAG_in_help	in_help		/* Help flag */
#define PFLAG_nvgen	nvgen		/* NV gen flag */
#define PFLAG_sense	sense		/* sense flag */
#define	PFLAG_which	which		/* which flag */

#define	PTXT_0 			nv_command
#define PTXT_1 			txt1
#define PTXT_2 			txt2
#define PTXT_3 			txt3
#define PTXT_4 			txt4
#define PTXT_5 			txt5
#define PTXT_6 			txt6
#define	PTXT_prompt		prompt
#define	PTXT_line		line
#define	PTXT_break_chars	break_chars

#define	PCHAR_lastchar		lastchar

#define PIDB_current interface
#define PIDB_1 idb1

#define	PCDB_current	controller
#define	PCDB_1		cdb1

#define	PMXINT_current	mx_int_info
#define	PMXINT_1	mxint1

#define PPDB_current protocol
#define PPDB_1 pdb1
#define PROUTEMAP_1 routemap1

#define PUDB_current1 userDataBlock[0]
#define PUDB_current2 userDataBlock[1]
#define PUDB_1 udb1
#define PUDB_2 udb2

#define PTTY_current tty

#define PTYPE_int(num) val ## num	/* Integer */
#define	PTYPE_char(num)  PCHAR ## num
#define PTYPE_string(num) PTXT_ ## num	/* String */
#define PTYPE_paddr(num) addr ## num	/* Protocol address ptr */
#define PTYPE_hwaddr(num) hwaddr ## num	/* Hardware address ptr */
#define PTYPE_maskedAddr(num) maskedAddr ## num /* masked address ptr */
#define PTYPE_idb(num) PIDB_ ## num	/* Interface data block ptr */
#define	PTYPE_cdb(num) PCDB_ ## num	/* Controller data block ptr */
#define	PTYPE_mxint(num) PMXINT_ ## num	/* Multiplexed interface info */
#define PTYPE_pdb(num) PPDB_ ## num	/* Protocal data block ptr */
#define PTYPE_routemap(num) PROUTEMAP_ ## num	/* route-map data block ptr */

#define PTYPE_udb(num) PUDB_ ## num	/* User Defined data block ptr */

#define PTYPE_tty(num) PTTY_ ## num	/* TTY tt_soc */
#define PTYPE_flag(num) PFLAG_ ## num	/* Parser flags */

#define PPTR_int
#define PPTR_string
#define PPTR_paddr &
#define PPTR_hwaddr &
#define PPTR_maskedAddr &
#define PPTR_idb
#define PPTR_cdb
#define PPTR_mxint
#define PPTR_pdb
#define PPTR_routemap
#define PPTR_udb
#define PPTR_flag
#define PPTR_tty

#define GETOBJ(type,num) (PPTR_ ## type ## (csb->PTYPE_ ## type ## ( ## num ## )))
#define	SETOBJ(type,num) csb->PTYPE_ ## type ## ( ## num ## )
#define ISOBJ(type,num) ((int) GETOBJ(type,num) != 0)

/************************************************************************/
/*********** END OF NEW PARSER STRUCTURE ********************************/
/************************************************************************/


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/
/*
 *    config.c
 */
extern void banner(parseinfo *, char **);
extern void docaps(parseinfo *, ulong, int);
extern void setinteger(parseinfo *, int, void *);
extern void settoken(parseinfo *, char *, void *);
extern void setuser_integer(parseinfo *, int, void *);

extern boolean parse_var_unsigned(char *, char, ulong *);
extern char *gettoken(char *, char *, int, int);
extern int get_ttyline(char *);
extern void *find_namedthing(queuetype *, const char *);
extern void parse_configure(char *, int, int, uchar);
extern void parse_configure_idb(char *, int, int, uchar, idbtype *);
extern boolean parse_line(void *, transition *, uint, const char *, ...);
extern boolean parse_line_mode(void *, parser_mode *, uint, const char *, ...);
extern void reset_host(void);
extern boolean config_before(uint major, uint minor);
extern void config_atleast(parseinfo *csb, uint major, uint minor);
extern void note_write_mem_time(void);
extern char *last_config_time_string(void);
extern char *last_write_mem_time_string(void);


/*
 * exec.c - see command.h for the remainder of the prototypes
 */
extern void help_command(char *const strings[], 
                         const ushort flags[], 
                         const int);

/*
 * parser.c
 */
typedef boolean (*match_gen_func)(parseinfo *, char *, int *, void *,
				  int, uint, struct idbtype_ *);
typedef char * (*short_help_func)(parseinfo *, uint, struct idbtype_ *);

extern void parser_add_protoaddr(const enum LINK, const char *, const char *,
				 const char *, const uint, const uint);
extern void parser_add_address(const uint, short_help_func, match_gen_func);
extern void parser_add_router(const uint, void (*)(parseinfo *, const char *,
						   char *, transition *));
extern void exit_config_submode(parseinfo *);
extern void exit_config_modes(parseinfo *);

#endif __CONFIG_H__

