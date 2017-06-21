/* $Id: map3270.c,v 3.3.28.3 1996/06/28 23:30:46 hampton Exp $
 * $Source: /release/112/cvs/Xsys/tn3270/map3270.c,v $
 *------------------------------------------------------------------
 * MAP3270.C - Reads and parses ttycaps.
 *
 * June 1991, Robert Snyder
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: map3270.c,v $
 * Revision 3.3.28.3  1996/06/28  23:30:46  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.28.2  1996/04/29  10:03:15  mdb
 * The 96q1 gcc is much more particular about putting parentheses around
 * comparisons for some operands ("&", "^" and "|") [CSCdi56015].
 * Branch: California_branch
 *
 * Revision 3.3.28.1  1996/03/18  22:19:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.1  1996/03/07  10:54:01  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3  1996/01/22  09:06:56  mdb
 * CSCdi47263:  change definition of NULL to ((void *) 0) from 0
 *
 * Revision 3.2  1995/11/17  18:51:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/10  21:17:50  satya
 * CSCdi36172:  tn3270 doesnt check malloc return code
 *
 * Revision 2.1  1995/06/07  23:07:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*  This program reads a description file, somewhat like /etc/termcap,
    that describes the mapping between the current terminal's keyboard and
    a 3270 keyboard.
 */

#include "master.h"
#include <ctype.h>
#include <ciscolib.h>
#include "connect.h"
#include "ttysrv.h"

#include "tn3270.h"			/* Defines CISCO */
#include "proto.h"
#include "vt_interface.h"
#include "tn3270_private.h"


#ifndef NULL
#define NULL ((void *) 0)
#endif
#ifndef EOF
#define EOF (-1)
#endif

void FreeKeymap(struct vtinfo_type * vtinfo);
static void insert_help_entry(char *key, stringWithLength * parse);
static int translate_to_int(char *string);
static void AddPrintables(struct vtinfo_type * vtinfo);

#define IsPrint(c)	((isprint(c) && !isspace(c)) || ((c) == ' '))


static char *keyslookup[2];


/* this is the list of types returned by the lex processor */
#define LEX_CHAR	400	       /* plain unadorned character */
#define LEX_ESCAPED	LEX_CHAR+1     /* escaped with \ */
#define LEX_CARETED	LEX_ESCAPED+1  /* escaped with ^ */
#define LEX_END_OF_FILE LEX_CARETED+1  /* end of file encountered */
#define LEX_ILLEGAL	LEX_END_OF_FILE+1	/* trailing escape character */

/* the following is part of our character set dependancy... */
#define ESCAPE		0x1b
#define TAB		0x09
#define NEWLINE		0x0a
#define CARRIAGE_RETURN 0x0d
#define SS3		0x8f
#define	SS2		0x8e
#define CSI		0x9B
#define DCS		0x90
static char *uncontrol(unsigned int c);

#define panic(s)	{ fprintf(stderr, s); exit(1); }

static int doPaste = 1;		       /* should we have side effects */


#include "generic.h"
#include "def_ascebc.h"

static int hextoi(char c)
{



    if (!isxdigit(c))
	return 0;

    if (isdigit(c))
	return (c - '0');

    return (toupper(c) - 'A' + 10);

}





static int GetC(VtinfoPtr vtinfo)
{
    int character;
    int check_for_slash_return;

    if ((*(vtinfo->environPointer)) == 0) {
	vtinfo->whichkey++;	       /* default map */
	vtinfo->environPointer = *vtinfo->whichkey;
    }
    if (*(vtinfo->environPointer)) {
	character = 0xff & *vtinfo->environPointer++;
        check_for_slash_return = character == '\\';
        while (check_for_slash_return) {
            if (((0xff&*vtinfo->environPointer) == '\n') ||
                ((0xff&*vtinfo->environPointer) == '\r')) {
		    /* skip past the \n */
                vtinfo->environPointer++;
                /* get the next char */
                character = 0xff&*vtinfo->environPointer++;
                check_for_slash_return = character == '\\';
                if ((!*vtinfo->environPointer) && (check_for_slash_return))
                    character = EOF;
	    } else
                check_for_slash_return = FALSE;
        }
    } else {
	character = EOF;
    }
    return (character);
}

lexicon Get(VtinfoPtr vtinfo)
{
    lexicon c;
    lexicon *pC = &c;
    int character;
    int value;

    if (!vtinfo->Empty) {
	*pC = vtinfo->lifo[vtinfo->rp];
	vtinfo->rp++;
	if (vtinfo->rp == sizeof(vtinfo->lifo) / sizeof(lexicon)) {
	    vtinfo->rp = 0;
	}
	if (vtinfo->rp == vtinfo->wp) {
	    vtinfo->Empty = 1;
	}
	vtinfo->Full = 0;
    } else {
	character = GetC(vtinfo);
	switch (character) {
	  case EOF:
	    pC->type = LEX_END_OF_FILE;
	    break;
	  case '^':
	    character = GetC(vtinfo);
	    if (!IsPrint(character)) {
		pC->type = LEX_ILLEGAL;
	    } else {
		pC->type = LEX_CARETED;
		if (character == '?') {
		    character |= 0x40; /* rubout */
		} else {
		    character &= 0x1f;
		}
	    }
	    break;
	  case '\\':
	    character = GetC(vtinfo);
	    if (!IsPrint(character)) {
		pC->type = LEX_ILLEGAL;
	    } else {
		pC->type = LEX_ESCAPED;
		switch (character) {
		  case 'E':
		  case 'e':
		    character = ESCAPE;
		    break;
		  case '\\':
		    character = '\\';
		    break;
		  case '^':
		    character = '^';
		    break;
		  case 't':
		    character = TAB;
		    break;
		  case 'n':
		    character = NEWLINE;
		    break;
		  case 'r':
		    character = CARRIAGE_RETURN;
		    break;
		  case '2':
		    character = SS2;
		    break;
		  case '3':
		    character = SS3;
		    break;
		  case 'C':
		    character = CSI;
		    break;
		  case 'D':
		    character = DCS;
		    break;
		  case '0':
		    character = GetC(vtinfo);
		    if ((character != 'x') && (character != 'X')) {
			pC->type = LEX_ILLEGAL;
			break;
		    }
		    character = GetC(vtinfo);
		    if (!isxdigit(character)) {
			pC->type = LEX_ILLEGAL;
			break;
		    }
		    value = hextoi(character) << 4;
		    character = GetC(vtinfo);
		    if (!isxdigit(character)) {
			pC->type = LEX_ILLEGAL;
			break;
		    }
		    value = value + hextoi(character);
		    pC->type = LEX_ESCAPED;
		    character = value;
		    break;
		  default:
		    pC->type = LEX_ILLEGAL;
		    break;
		}
	    }
	    break;
	  default:
	    if ((IsPrint(character)) || isspace(character)) {
		pC->type = LEX_CHAR;
	    } else {
		pC->type = LEX_ILLEGAL;
	    }
	    break;
	}
	pC->value = character;
    }
    return (*pC);
}

static void UnGet(VtinfoPtr vtinfo, lexicon c)
{
    if (vtinfo->Full) {
	printf("attempt to put too many characters in lifo\n");
	fatal(1);
	/* NOTREACHED */
    } else {
	vtinfo->lifo[vtinfo->wp] = c;
	vtinfo->wp++;
	if (vtinfo->wp == sizeof(vtinfo->lifo) / sizeof(lexicon)) {
	    vtinfo->wp = 0;
	}
	if (vtinfo->wp == vtinfo->rp) {
	    vtinfo->Full = 1;
	}
	vtinfo->Empty = 0;
    }
}


static stringWithLength *
 GetQuotedString(VtinfoPtr vtinfo)
{
    lexicon lex;
    static stringWithLength output = {0};	/* where return value is held */
    char *pointer = output.array;

    lex = Get(vtinfo);
    if ((lex.type != LEX_CHAR) || (lex.value != '\'')) {
	UnGet(vtinfo, lex);
	return (0);
    }
    while (1) {
	lex = Get(vtinfo);
	if ((lex.type == LEX_CHAR) && (lex.value == '\'')) {
	    break;
	}
	if ((lex.type == LEX_CHAR) && !IsPrint(lex.value)) {
	    UnGet(vtinfo, lex);
	    return (0);		       /* illegal character in quoted string */
	}
	if (pointer >= output.array + sizeof output.array) {
	    return (0);		       /* too long */
	}
	*pointer++ = lex.value;
    }
    output.length = pointer - output.array;
    return (&output);
}


static int GetCharacter(VtinfoPtr vtinfo, int character)
{
    lexicon lex;

    lex = Get(vtinfo);

    if ((lex.type != LEX_CHAR) || (lex.value != character)) {
	UnGet(vtinfo, lex);
	return (0);
    }
    return (1);
}


static stringWithLength *
 GetAlphaMericString(VtinfoPtr vtinfo)
{
    lexicon lex;
    static stringWithLength output = {0};
    char *pointer = output.array;

#define	IsAlnum(c)	(isalnum(c) || (c == '_') \
					|| (c == '-') || (c == '.'))

    lex = Get(vtinfo);

    if ((lex.type != LEX_CHAR) || !IsAlnum(lex.value)) {
	UnGet(vtinfo, lex);
	return (0);
    }
    while ((lex.type == LEX_CHAR) && IsAlnum(lex.value)) {
	*pointer++ = lex.value;
	lex = Get(vtinfo);
    }
    UnGet(vtinfo, lex);
    *pointer = 0;
    output.length = pointer - output.array;
    return (&output);
}


/* eat up characters until a new line, or end of file.	returns terminating
	character.
 */

static lexicon
 EatToNL(VtinfoPtr vtinfo)
{
    lexicon lex;

    lex = Get(vtinfo);

    while (!((lex.type != LEX_ESCAPED) && (lex.type != LEX_CARETED) &&
	     (lex.value == '\n')) && (!(lex.type == LEX_END_OF_FILE))) {
	lex = Get(vtinfo);
    }
    if (lex.type != LEX_END_OF_FILE) {
	return (Get(vtinfo));
    } else {
	return (lex);
    }
}


static void GetWS(VtinfoPtr vtinfo)
{
    lexicon lex;

    lex = Get(vtinfo);

    while ((lex.type == LEX_CHAR) &&
	   (isspace(lex.value) || (lex.value == '#'))) {
	if (lex.value == '#') {
	    lex = EatToNL(vtinfo);
	} else {
	    lex = Get(vtinfo);
	}
    }
    UnGet(vtinfo, lex);
}


static void FreeState(struct state_type * pState)
{
    free(pState);
}


static struct state_type *GetState(void)
{
    struct state_type *pState;

    pState = malloc(sizeof(struct state_type));

    if (pState) {
        pState->result = STATE_NULL;
        pState->next = 0;
        return (pState);
    } else {
        tn3270_debug(0,0,"Error allocating memory\n");
        return (NULL);
    }
}


static struct state_type *
 FindMatchAtThisLevel(struct state_type * pState, int character)
{
    while (pState) {
	if (pState->match == character) {
	    return (pState);
	}
	pState = pState->next;
    }
    return (0);
}


static struct state_type *
 PasteEntry(struct state_type * head, char *string, int count, char *identifier)
{
    struct state_type *pState, *other;

    if (!doPaste) {		       /* flag to not have any side effects */
	return ((struct state_type *) 1);
    }
    if (!count) {
	return (head);		       /* return pointer to the parent */
    }
    if ((head->result != STATE_NULL) && (head->result != STATE_GOTO)) {
	/*
	 * this means that a previously defined sequence is an initial part
	 * of this one.
	 */
	printf("Conflicting entries found when scanning %s\n",
	       identifier);
	return (0);
    }
    pState = GetState();
    if (!pState) {
        tn3270_debug(0,0,"GetState Malloc failure\n");
        return(0);
    }
    pState->match = *string;
    if (head->result == STATE_NULL) {
	head->result = STATE_GOTO;
	head->address = pState;
	other = pState;
    } else {			       /* search for same character */
	if ((other = FindMatchAtThisLevel(head->address, *string)) != 0) {
	    FreeState(pState);
	} else {
	    pState->next = head->address;
	    head->address = pState;
	    other = pState;
	}
    }
    return (PasteEntry(other, string + 1, count - 1, identifier));
}


static void strlcat(stringWithLength * s1, stringWithLength * s2)
{
    int j;

    for (j = 0; j < s2->length; j++)
	s1->array[s1->length++] = s2->array[j];

}






static
int GetInput(VtinfoPtr vtinfo, int tc, char *identifier)
{
    stringWithLength *outputString;
    struct state_type *head;
    struct state_type fakeQueue;
    stringWithLength key_seq;

    key_seq.length = 0;

    if (doPaste) {
	head = vtinfo->head;	       /* always points to level above this
				        * one */
    } else {
	head = &fakeQueue;	       /* don't have any side effects... */
    }

    if ((outputString = GetQuotedString(vtinfo)) == 0) {
	return (0);
    } else if ((0) && (IsPrint(outputString->array[0]))) {	/* 0 HACK WLP */
	printf(
	       "first character of sequence for %s is not a control type character\n",
	       identifier);
	return (0);
    } else {
	if ((head = PasteEntry(head, outputString->array,
			       outputString->length, identifier)) == 0) {
	    return (0);
	}
	if (doPaste) {
	    outputString->array[outputString->length] = 0;
	    strlcat(&key_seq, outputString);
	}
	GetWS(vtinfo);
	while ((outputString = GetQuotedString(vtinfo)) != 0) {

	    if ((head = PasteEntry(head, outputString->array,
				   outputString->length, identifier)) == 0) {
		return (0);
	    }
	    if (doPaste) {
		outputString->array[outputString->length] = 0;
		strlcat(&key_seq, outputString);
	    }
	    GetWS(vtinfo);
	}
    }

    if (doPaste)
	insert_help_entry(identifier, &key_seq);


    if (!doPaste) {
	return (1);
    }
    if ((head->result != STATE_NULL) && (head->result != tc)) {
	/*
	 * this means that this sequence is an initial part of a previously
	 * defined one.
	 */
	printf("Conflicting entries found when scanning %s\n",
	       identifier);
	return (0);
    } else {
	head->result = tc;
	return (1);		       /* done */
    }
}


static
int GetDefinition(VtinfoPtr vtinfo)
{
    stringWithLength *string;
    int Tc;

    GetWS(vtinfo);
    if ((string = GetAlphaMericString(vtinfo)) == 0) {
	return (0);
    }
    string->array[string->length] = 0;
    if (doPaste) {
	if ((Tc = translate_to_int(string->array)) == -1) {
	    if (1) {
		printf("%s: unknown 3270 key identifier\n",
		       string->array);
	    }
	    Tc = STATE_NULL;
	}
    } else {
	Tc = STATE_NULL;	       /* XXX ? */
    }


    GetWS(vtinfo);
    if (!GetCharacter(vtinfo, '=')) {
	printf(
	       "Required equal sign after 3270 key identifier %s missing\n",
	       string->array);
	return (0);
    }
    GetWS(vtinfo);
    if (!GetInput(vtinfo, Tc, string->array)) {
	printf("Missing definition part for 3270 key %s\n",
	       string->array);
	return (0);
    } else {
	GetWS(vtinfo);
	while (GetCharacter(vtinfo, '|')) {
	    GetWS(vtinfo);
	    if (!GetInput(vtinfo, Tc, string->array)) {
		printf("Missing definition part for 3270 key %s\n",
		       string->array);
		return (0);
	    }
	    GetWS(vtinfo);
	}
    }
    GetWS(vtinfo);

    if (!GetCharacter(vtinfo, ';')) {
	printf("Missing semi-colon for 3270 key %s\n", string->array);
	return (0);
    }
    return (1);
}


static
int GetDefinitions(VtinfoPtr vtinfo)
{
    if (!GetDefinition(vtinfo)) {
	return (0);
    } else {
	while (GetDefinition(vtinfo)) {
	    ;
	}
    }
    return (1);
}

static
int GetBegin(VtinfoPtr vtinfo)
{
    GetWS(vtinfo);
    if (!GetCharacter(vtinfo, '{')) {
	return (0);
    }
    return (1);
}

static
int GetEnd(VtinfoPtr vtinfo)
{
    GetWS(vtinfo);
    if (!GetCharacter(vtinfo, '}')) {
	return (0);
    }
    return (1);
}

static
int GetName(VtinfoPtr vtinfo)
{
    if (!GetAlphaMericString(vtinfo)) {
	return (0);
    }
    GetWS(vtinfo);
    while (GetAlphaMericString(vtinfo)) {
	GetWS(vtinfo);
    }
    return (1);
}

static
int GetNames(VtinfoPtr vtinfo)
{
    GetWS(vtinfo);
    if (!GetName(vtinfo)) {
	return (0);
    } else {
	GetWS(vtinfo);
	while (GetCharacter(vtinfo, '|')) {
	    GetWS(vtinfo);
	    if (!GetName(vtinfo)) {
		return (0);
	    }
	}
    }
    return (1);
}

static
int GetEntry0(VtinfoPtr vtinfo)
{
    if (!GetBegin(vtinfo)) {
	printf("no '{'\n");
	return (0);
    } else if (!GetDefinitions(vtinfo)) {
	printf("unable to parse the definitions\n");
	return (0);
    } else if (!GetEnd(vtinfo)) {
	printf("No '}' or scanning stopped early due to error.\n");
	return (0);
    } else {
	/* done */
	return (1);
    }
}


static
int GetEntry(VtinfoPtr vtinfo)
{
    if (!GetNames(vtinfo)) {
	printf("Invalid name field in entry.\n");
	return (0);
    } else {
	return (GetEntry0(vtinfo));
    }
}


/* position ourselves within a given filename to the entry for the current
 *	KEYBD (or TERM) variable
 */

int Position(VtinfoPtr vtinfo, char *filename, char *keybdPointer)
{
    lexicon lex;
    stringWithLength *name = 0;
    stringWithLength *oldName;

#define	Return(x) {doPaste = 1; return(x);}

    doPaste = 0;

    lex = Get(vtinfo);
    while (lex.type != LEX_END_OF_FILE) {
	UnGet(vtinfo, lex);
	/* now, find an entry that is our type. */
	GetWS(vtinfo);
	oldName = name;
	if ((name = GetAlphaMericString(vtinfo)) != 0) {
	    if (!strcasecmp(name->array, keybdPointer)) {
		/* need to make sure there is a name here... */
		lex.type = LEX_CHAR;
		lex.value = 'a';
		UnGet(vtinfo, lex);
		Return(1);
	    }
	} else if (GetCharacter(vtinfo, '|')) {
	    ;			       /* more names coming */
	} else {
	    lex = Get(vtinfo);
	    UnGet(vtinfo, lex);
	    if (lex.type != LEX_END_OF_FILE) {
		if (!GetEntry0(vtinfo)) {	/* start of an entry */
		    printf(
			   "error was in entry for %s in file %s\n",
			(oldName) ? oldName->array : "(unknown)", filename);
		    Return(0);
		}
	    }
	}
	lex = Get(vtinfo);
    }
    printf("Unable to find entry for %s in file %s\n", keybdPointer,
	   filename);
    Return(0);
}


/*
 * InitControl - our interface to the outside.	What we should
 *  do is figure out keyboard (or terminal) type, set up file pointer
 *  (or string pointer), etc.
 */

static struct state_type *
InitControl(VtinfoPtr vtinfo, int pickyarg)
#ifdef  Doc
    int pickyarg;		       /* Should we be picky? */
#endif
{
char *name;
tty_3270def *km;
char *kms;
conntype *conn;



/*    Position(vtinfo, vtinfo->environPointer, "generic");*/

    conn = vtinfo->connection; 

    vtinfo->whichkey = 0;

    name = conn->tty->keymaptype ? conn->tty->keymaptype : conn->tty->termtype;

    if (name) {
	km = keymap_list;
	while (km) {
            kms = km->def;
            if (strcmp (name,km->def_name) == 0) {
                vtinfo->whichkey = keyslookup;
                keyslookup[0] = km->def;
                keyslookup[1] = 0;
                kms = 0;
                km = 0;
            }
         
            while (kms && *kms) {
                if (!strcasecmp (kms,name)) {
                    vtinfo->whichkey = keyslookup;
                    keyslookup[0] = km->def;
                    keyslookup[1] = 0;
                    kms = 0;
                    km = 0;
                } else
                    while (kms && *kms) {
                        if (*kms == '|') {
                            kms++;
                            break;
                        }
                        kms++;
                        if ((*kms == '\0') || (*kms == '{'))
                            kms = 0;
                    }
            }
            if (km)
                km = km->next_def;
        }
    }
    if (!vtinfo->whichkey) {
        km = keymap_list;
        while (km) {
            kms = km->def;
            if (strcmp("default",  km->def_name) == 0) {
                vtinfo->whichkey = keyslookup;
                keyslookup[0] = km->def;
                keyslookup[1] = 0;
                kms = 0; /* drops us out of while loop */
                km = 0;  /* drops us out of while loop */
            }

            while (kms && *kms) {
                if (!strcasecmp(kms, "default")) {
                    vtinfo->whichkey = keyslookup;
                    keyslookup[0] = km->def;
                    keyslookup[1] = 0;
                    kms = 0; /* drops us out of while loop */
                    km = 0;  /* drops us out of while loop */
                } else
                    while (kms && *kms) {
                        /* advance to the next name */
                        if (*kms == '|') {
			    kms ++;
			    break;
			}
                        kms ++;
                        if ((*kms == '\0') || *kms == '{')
                            kms = 0;
                    }
            }
            if (km)
                km = km->next_def;
        }
    }
    if (!vtinfo->whichkey) {
        printf("Using default key mappings.\n");
        vtinfo->whichkey = keysgeneric;
    }

    vtinfo->environPointer =
        *vtinfo->whichkey;	/* use default table */

    (void) GetEntry(vtinfo);
    return (vtinfo->head->address);
}


void insert_help_entry(key, parse)
    char *key;
    stringWithLength *parse;
{
/*
int i;
	printf ("Insert help %s '",key);
	for (i=0;i<parse->length;i++)
		printf ("%s",uncontrol((unsigned char ) parse->array[i]));
	printf ("'\n");

  */
}


int translate_to_int(string)
    char *string;


{
    return (keyname_to_int(string));
}









/* MARK
 * Construct a control character sequence
 * for a special character.
 */
char *
 uncontrol(c)
    unsigned int c;
{
    static char buf[6];

    if (c == 0x7f)
	return ("^?");
    if (c == '\377') {
	return "-1";
    }
    if (c > 0x7f) {
	sprintf(buf, "\\0x%02x", (unsigned char) c);
    } else if (c >= 0x20) {
	buf[0] = c;
	buf[1] = 0;
    } else {
	buf[0] = '^';
	buf[1] = '@' + c;
	buf[2] = 0;
    }
    return (buf);
}

void print_level(int val)
{

    while (val--)
	printf("\t");

}



void print_node(struct state_type * head, int level)
{

    while (head) {
	print_level(level);
	printf("Node at %08x %s %02x %d",
	       head,
	       uncontrol((unsigned char) head->match),
	       (unsigned char) head->match,
	       head->result);
	if (head->result >= 0)
	    printf(" %s\n", int_to_keyname(head->result));
	else
	    printf(" %d\n", head->result);


	print_node(head->address, level + 1);
	head = head->next;
    }


}



void init_keymap(struct vtinfo_type * vtinfo)
{
    struct state_type *head;


    head = malloc(sizeof(struct state_type));

    if (!head) {
        tn3270_debug(0,0,"Error allocating memory\n");
        return;
    }

    head->match = 0;
    head->result = STATE_NULL;
    head->next = NULL;
    head->address = NULL;

    vtinfo->head = head;
    vtinfo->Empty = 1;
    vtinfo->Full = 0;
    vtinfo->rp = 0;
    vtinfo->wp = 0;
    vtinfo->lifo[0].type = 0;
    vtinfo->lifo[0].value = 0;


/*
    vtinfo->whichkey = keysgeneric;
    vtinfo->environPointer = *vtinfo->whichkey;
*/

    InitControl(vtinfo, 1);
    AddPrintables(vtinfo);


    /*
     * Get rid of the fake top of the state table so we now point to the
     * first level of the state table with all the beginings of key sequenses
     */
    vtinfo->head = vtinfo->head->address;
    vtinfo->state = vtinfo->head;

    /* print_node (vtinfo->head,0); */
    free(head);
}

/*
 * This routine will add to out keyboard state table
 * all the printable characters that have not been added
 * thru the keymap file.
 */

void AddPrintables(struct vtinfo_type * vtinfo)
{
    int i = 0;
    struct state_type *head;
    struct state_type *new;

    head = vtinfo->head->address;

    /*
     * If head is 0 we have no key mappings... so dont add any more, since
     * this should not occur
     */
    if (!head)
	return;

    while (default_keys[i].name) {
	if (!FindMatchAtThisLevel(head, default_keys[i].input)) {
	    new = GetState();
            if (!new) {
                tn3270_debug(0,0,"GetState malloc failure\n");
                return;
            }
	    new->result = keyname_to_int(default_keys[i].name);
	    new->match = (default_keys[i].input);
	    new->next = head;
	    new->address = NULL;
	    vtinfo->head->address = new;
	    head = new;
	}
	i++;
    }





}




void FreeNode(struct state_type * state)
{
    struct state_type *next = 0;

    while (state) {
	next = state->next;
	FreeNode(state->address);
	free(state);
	state = next;
    }
}


void FreeKeymap(struct vtinfo_type * vtinfo)
{

    FreeNode(vtinfo->head);

}
