/* $Id: builtin.c,v 3.2.54.1 1996/03/21 23:26:38 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/builtin.c,v $
 *------------------------------------------------------------------
 * builtin.c
 *
 * August 1992, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Some Monitor built in commands.
 *------------------------------------------------------------------
 * $Log: builtin.c,v $
 * Revision 3.2.54.1  1996/03/21  23:26:38  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:19:48  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:41:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:04:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:36:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "sh.h"

/*
** Display the command line arguments.
*/
int
echo(int argc, char *argv[])
{
  register i, c;
  register char *cptr;
  long temp;
  enum {NEUTRAL, BACKSLASH, OCTNUM} state = NEUTRAL;

  for(i=1; i<argc; i++) {
    cptr = argv[i];
    while(c = *cptr++) {
      switch(state) {
      case NEUTRAL:
	if(c == '\\') {
	  state = BACKSLASH;
	  continue;
	}
	break;
      case BACKSLASH:
	if(c >= '0' && c <= '7') {
	  --cptr;  /* unget the char */
	  state = OCTNUM;
	  continue;
	}
	switch(c) {
	case 'n': c = '\n'; break;  /* newline */
	case 'b': c = '\b'; break;  /* backspace */
	case 'f': c = '\f'; break;  /* formfeed */
	case 'r': c = '\r'; break;  /* return */
	case '\\': break;           /* backslash */
	case 'c': return(0);           /* end of line, no newline */
	}
	break;
      case OCTNUM:
	cptr += getnnum(cptr, 8, &temp, 3);
	c = temp;
	break;
      }
      putchar(c);
      state = NEUTRAL;
    }
    putchar(' ');  /* put a space between argv's */
  }
  putchar('\n');
  return(0);
}

/*
** These routines provide the MAN monitor shell with an environment
** variable mechanism that closely resembles that of the Bourne shell
** and an alias mechanism.  The major difference is that our environment
** variables have no attributes.  These variables are stored in a table
** in non-volatile RAM as a series of NULL terminated strings.  The first
** string is a variable name, the second is that variables value, the third
** is the next variable name, and so on.  A null variable name terminates
** the table.  A separate table is used for environment variables and
** aliases.
*/
int
unset(int argc, char *argv[])
{
  register i;
  char *tblptr;

  if(argc < 2) {
    printf("usage: %s name1 [name2 ...]\n",argv[0]);
    return(1);
  }
  if(strcmp(argv[0],"unalias") == 0) tblptr = ALIAS_TABLE;
  else tblptr = (NVRAM)->evartbl;
  for(i=1; i<argc; i++) {
    if(unsetvar(tblptr,argv[i]) < 0)
      printf("%s: \"%s\" does not exist\n",argv[0],argv[i]);
  }
  return(0);
}

/*
** This routine is called by both "set" and "alias" commands.
** Display the appropriate table for the user or, if invoked as "alias"
** with arguments, setvar them.
*/
int
setalias(int argc, char *argv[])
{
  register char *tblptr;

  if(strcmp(argv[0],"alias") == 0) {  /* invoked as alias? */
   if(argc > 2) {
alsusage:
      printf("usage: %s [name=value]\n",argv[0]);
      return(1);
    }
    if(argc == 2) {
      if(setvar(ALIAS_TABLE,ALSSIZ,argv[1]) == 0) return(0);
      else goto alsusage;
    }
    tblptr = ALIAS_TABLE;
  } else { 
   if(argc > 1) {
      printf("usage: %s\n",argv[0]);
      return(1);
    }
    tblptr = (NVRAM)->evartbl;
  }
  printtbl(tblptr);
  return(0);
}

int
sleep_cmd(int argc, char *argv[])
{
    long msecs;

    if(argc != 2 || !getnum(argv[1],10,&msecs)) {
	printf("usage: %s milliseconds\n", argv[0]);
	return(1);
    }
    sleep(msecs);
    return(0);
}

int
reset(int argc, char *argv[])
{
    if(argc > 1) {
	if(strcmp(argv[1], "-s") == 0) savenv();  /* save our environment */
	else {
	    printf("usage: %s [-s]\n");
	    return(1);
	}
    }
    do_reset();  /* call asm routine */
}
