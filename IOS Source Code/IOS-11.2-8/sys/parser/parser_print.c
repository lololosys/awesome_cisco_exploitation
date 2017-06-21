/* $Id: parser_print.c,v 3.4.4.2 1996/06/28 23:25:16 hampton Exp $
 * $Source: /release/112/cvs/Xsys/parser/parser_print.c,v $
 *------------------------------------------------------------------
 * P A R S E R _ P R I N T . C  (formerly P R I N T . C)
 *
 * More-style pretty-printing routines for the new parser.
 *
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_print.c,v $
 * Revision 3.4.4.2  1996/06/28  23:25:16  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.4.1  1996/03/18  21:33:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  07:22:49  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/03/07  10:40:21  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  16:49:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/03/09  22:57:10  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.3  1996/02/07  21:15:27  widmer
 * CSCdi48343:  Remove unused #ifdef STANDALONE from parser code
 *
 * Revision 3.2  1995/12/17  18:35:21  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.1  1995/12/15  03:46:20  thille
 * Parser organizational cleanup.  Rename core parser files to be
 * parser_<foo>.  Start by adding placeholders for the new filenames.
 *
 * OLD LOG as print.c:
 * -------------------
 * Revision 3.2  1995/11/17  18:53:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/20  17:03:10  widmer
 * CSCdi35981:  compressconfigurationcontrollers is a mouthfull
 *
 * Revision 2.1  1995/06/07  22:32:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#include <string.h>
#include "ttysrv.h"
#include "config.h"
#include "parser.h"

#define fputs(str,stdout) puts(stdio, str, strlen(str))
#define getchar() getc(stdio)
#define putchar(c) printc((c))

#define SCREEN_WIDTH stdio->tty_width
#define SCREEN_LINES stdio->tty_length

typedef enum {
    HELP_TEXT, HELP_SHORT, HELP_LONG
} helpType;

#define FIELDS 4

static int line_index;          /* Lines printed since the last more?
                                 * prompt */

/*
 *  Forward references
 */
static int count_lines(save_buf * pbuffer);
static void find_field_widths(save_buf *pbuffer, short field_width[]);
static void sort_short_help(char *buffer, int length);
static int count_final_newlines(save_buf * pbuffer);
static int logical_line_length(char *s, helpType * ptype);
static void print_lines(char **, short [], boolean, boolean);
static void sort_lines(char **line_starts, int num);
static char **record_line_starts(save_buf * pbuf, int line_cnt);
static void paged_output(char *, int, short[], boolean, boolean);

/* cisco's strncpy puts a '\0' at the end of every destination.
 * This is at odds with UNIX's strncpy, which doesn't write '\0'
 * if it reachs the max length.
 */

static inline void unix_strncpy (char *dst, char *src, ulong max)
{
    while ((max-- > 0) && (*dst++ = *src++));
}

/*
 * This function (print_buf) takes a save_buf and prints it on stdout,
 * after doing various formating functions:
 * 
 * - Lines of text are seperated by \n characters, but there's no guarentee
 * that the last line will be TERMINATED by a \n.
 * 
 * - Fields within lines are seperated by \t characters.  Fields are sized
 * dynamically; a field assumes the size of its largest component (plus
 * two chars padding).  If a line contains no \t characters, it does not
 * have fields and does not affect any field widths.
 * 
 * - Automatic wrap-around onto a new line may occur at any space (' ')
 * character.  If the line is wrapped within a field, the new line is
 * indented to the start of the field.
 * 
 * - After a screen-full of lines, a "More?" prompt is presented and accepts
 * single character input to control further output. Both '\r' and 'q' are
 * supported as in UNIX more.
 * 
 * - The size of the screen (both length and width) comes from the stdio
 * tt_soc structure. Here is what is not done:
 * 
 * - The more function doesn't support backing up in the input, although this
 * is certainly doable, since everything is in a buffer.
 * 
 * - The field function doesn't support wrapping any fields but the last one
 * on a line.  Predictable but bizarre things will occur if interior
 * fields extend past the end of line.
 * 
 */

void print_buf (save_buf buffer, boolean do_buginf, boolean do_formatting)
{
    /* Dynamically determined field widths */
    short field_width[FIELDS] =
    {0, 0, 0, 0};
    char **line_starts = NULL;
    int final_newlines;
    int line_cnt = 2;

    /* Start line_cnt at 2 so we always malloc something below ! */

    /* Don't crash if no help buffer yet. */
    if (buffer.buf == NULL) {
	return;
    }

    /*
     * We really need to ignore \n\n's at the end of the buffer. So we'll
     * count them and treat them specially. For now, we just won't output
     * any of the final \n's !
     */
    final_newlines = count_final_newlines(&buffer);

    line_cnt=count_lines(&buffer);

    /*
     * The variable line_cnt is now the number of lines seen. There should
     * be one newline starting each line, plus maybe a newline at the end.
     * We now need to allocate an array of pointers and record the
     * beginning of each useful line in it (i.e. we don't need to
     * alphabetize repeated newlines).
     * 
     * If we want to be hyper about memory, we should maybe make sure that
     * there's no interruption between line_starts allocation and freeing.
     * 
     * NOTE: record_line_starts sorts TABBED lines, so you have to
     * find field widths *after* calling it !
     *
     */

    line_starts = record_line_starts(&buffer, line_cnt);

    find_field_widths(&buffer, field_width);

    /* Print the paused output to screen. */
    line_index = 0;
    if (do_formatting) {
	automore_enable(NULL);
    }
    if (buffer.buf[0] != '\n') {
	if (do_buginf) {
	    buginf("\n");
	} else {
	    printc('\n');	/* Make sure there is a leading newline */
	}
    }
    if (line_starts == NULL) {
        /* Couldn't malloc, just dump it all out as is ! */
        paged_output(buffer.buf, buffer.used, field_width,
		     do_buginf, do_formatting);
    } else {
        print_lines(line_starts, field_width, do_buginf, do_formatting);
        free(line_starts);
	line_starts = NULL;
    }
    if (do_formatting) {
	automore_disable();
    }
}

static int count_lines (save_buf *pbuffer)
{
    int line_cnt = 0;
    int buffer_index;

    for (buffer_index = 0; buffer_index < pbuffer->used; buffer_index++) {
	if (pbuffer->buf[buffer_index]=='\n') {
	    line_cnt++;
	}
    }

    /* Add one for final line: */
    return(line_cnt+1);
}

static void find_field_widths (save_buf *pbuffer, short field_width[])
{
    int char_index = 0;
    int field_index = 0;
    int buffer_index;

    /*
     * Run through the buffer once to figure out field widths.  Perhaps it
     * would be best to optionally disable this feature, for printing
     * large things like configuration files that won't use fields anyway.
     * 
     * To avoid lots of looping, we start by counting the widths of each
     * field into field_width.  After we've gone through the buffer, we
     * insert padding (2 chars per field) and sum up the relative field
     * widths into absolute tab positions.
     * 
     * To keep things consistent, we use one set of field widths for printing
     * the entire print buffer.
     * 
     */

    for (buffer_index = 0; buffer_index < pbuffer->used; buffer_index++) {
        switch (pbuffer->buf[buffer_index]) {
        case '\n':
            if (field_index && field_width[field_index] < char_index) {
                field_width[field_index] = char_index;
            }
            char_index = 0;
            field_index = 0;
            /* line_cnt++; */
            /*
             * This will overestimate if we later ignore \n\n sequences or
             * print in chunks, but that's ok, can be dealt with if it
             * becomes a problem.
             */
            break;

        case '\t':
            if (field_width[field_index] < char_index) {
                field_width[field_index] = char_index;
            }
            char_index = 0;
            if (field_index < (FIELDS-1)) {
                field_index++;
            } else {
                field_index = 0;
		/*
		 * Next keyword hangs off end of line and we don't care
		 * what the field width is, so skip it.
		 */
		buffer_index++;
		while (pbuffer->buf[buffer_index] &&
		       (pbuffer->buf[buffer_index] != '\n') &&
		       (pbuffer->buf[buffer_index] != '\t')) {
		    buffer_index++;
		}
	    }
            break;

        default:
            char_index++;
            break;
        }
    }

    if (field_index && field_width[field_index] < char_index) {
        field_width[field_index] = char_index;
    }

    /*
     * Adjust field widths to be starting positions vice widths.
     */
    field_width[0] += 2;
    for (field_index = 1; field_index < FIELDS; field_index++) {
        field_width[field_index] += field_width[field_index - 1] + 2;
    }
}

static int count_final_newlines (save_buf * pbuffer)
{
    int chars_to_use;
    int buffer_index;
    int final_newlines = 0;

    buffer_index = pbuffer->used - 1;

    while ((buffer_index >= 0) && (pbuffer->buf[buffer_index] == '\n')) {
        buffer_index--;
    }
    chars_to_use = buffer_index + 1;
    pbuffer->buf[chars_to_use] = '\0';
    final_newlines = pbuffer->used - chars_to_use;
    pbuffer->used = chars_to_use;
    return(final_newlines);    /* number of final newlines */
}


static void paged_output (char *buf, int used, short field_width[],
			  boolean do_buginf, boolean do_formatting)
{
    int char_index = 0;
    int field_index = 0;
    int buffer_index;
    int word_len;
    void (* output)(const char *, ...);

    if (do_buginf) {
	output = buginf;
    } else {
	output = (void *) printf;	/* Hey, it works */
    }

    /*
     * Now we run through the buffer "for real".
     */
    for (buffer_index = 0; buffer_index < used; buffer_index++) {
        switch (buf[buffer_index]) {
	  case '\t':
            if (do_formatting && (field_index < FIELDS)) {
                for (; char_index < field_width[field_index]; char_index++) {
                    (*output)(" ");
                }
                field_index++;
                break;
            }
            /* Otherwise, fall through to newline */
	  case '\n':
	    (*output)("\n");
            char_index = 0;
            field_index = 0;
            line_index++;
            break;
	  case ' ':
	    for (word_len = 0; buffer_index + word_len + 1 < used &&
		 !isspace(buf[buffer_index + word_len + 1]);
		 word_len++) {
		;
	    }
	    if ((! SCREEN_WIDTH) ||
		((char_index + word_len + 1) < SCREEN_WIDTH) ||
		!do_formatting) {
		(*output)(" ");
		char_index++;
	    } else {
		(*output)("\n");
		char_index = 0;
		line_index++;
		if (field_index) {
		    for (; char_index < field_width[field_index - 1];
			 char_index++) {
			(*output)(" ");
		    }
		}
	    }
	    break;
	  default:
            (*output)("%c", buf[buffer_index]);
            char_index++;
            break;
        }
    }
    if (char_index) {
	(*output)("\n");
    }
}

/*
 * Sort groups of lines.  A group is a set of lines of the same type,
 * i.e. ones from short help, long help, or just text which needs
 * to be displayed.  Only help strings should be sorted before ouput.
 */
static char **record_line_starts (save_buf * pbuf, int line_cnt)
{
    char **line_starts = NULL;
    int current_line = 0;
    int buffer_index = 0;
    helpType type = HELP_TEXT;
    helpType prevtype = HELP_TEXT;
    int len;
    int scan_line = 0;

    line_cnt++;
    /* reserve room for NULL marker at end, too */

    line_starts = malloc(sizeof(char *) * line_cnt);

    if (line_starts == NULL) {
        /*
         * If this is NULL (malloc failed), we'll just have to do without
         * alphabeticizing the output !
         */
        return(NULL);
    }

    buffer_index = 0;
    while ((buffer_index < pbuf->used)
           && (pbuf->buf[buffer_index] != '\0')) {

        line_starts[scan_line] = pbuf->buf + buffer_index;
        prevtype = type;
        len = logical_line_length(line_starts[scan_line], &type);

        if ((prevtype == HELP_LONG) && (type != prevtype)) {
            /* Got long chunk that needs sorting. */
            sort_lines(line_starts + current_line, scan_line - current_line);
            /* Start recording the next chunk. */
            current_line = scan_line;
        }

        scan_line++;
        buffer_index += len;

        if (type == HELP_SHORT) {
            /* Each short help line needs sorting by itself. */
            sort_short_help(line_starts[current_line], len);
            /* Don't maintain a "chunk". */
            current_line = scan_line;
        }
        else if (type == HELP_TEXT) {
            /* Don't maintain a "chunk". */
            current_line = scan_line;
        }
    }

    /* Sort final chunk. */
    if (type == HELP_LONG) {
        sort_lines(line_starts + current_line, scan_line - current_line);
        current_line = scan_line;
    }

    line_starts[current_line++] = NULL; /* mark end of array */
    return(line_starts);
}

/*
 * Perform bubble sort on <num> logical lines.
 * 
 * ASSUMES: line_starts is not NULL.
 * 
 */
static void sort_lines (char **line_starts, int num)
{
    int i;
    int j;
    char *s;

    for (i = 0; i < num; i++) {
        for (j = i + 1; j < num; j++) {
            /*
             * Note: we don't care about stuff beyond logical line length,
             * because if we swap two lines based on what's way out there,
             * it won't show. So we don't need strncmp() here.
             */
            if (strcmp(line_starts[i], line_starts[j]) > 0) {
                s = line_starts[i];
                line_starts[i] = line_starts[j];
                line_starts[j] = s;
            }
        }
    }
}

static void sort_short_help (char *buffer, int length)
{
    char *spare;
    int i;
    int ctab = 0;
    struct tabstruct {
        char *field;
        int length;
    };
    struct tabstruct *tabfields = NULL;

    /* allocate room for a temporary copy (and safety margin) */
    spare = malloc((length + 2));
    if (spare == NULL) {
        /*
         * If we can't allocate this paltry amount of memory, then we'll
         * just live without sorting.
         */
        return;
    }

    {
        int tabs = 1;

        /*
         * Count tabs and allocate room for pointers to fields ending in
         * tabs.
         */
        for (i = 1; i < length; i++) {
            if (buffer[i - 1] == '\t') {
                tabs++;
            }
        }

        tabfields = (struct tabstruct *)
            malloc((sizeof(struct tabstruct) * tabs));

        if (tabfields == NULL) {
            /* Oh, well. Gotta do without a sort. */
            free(spare);
	    spare = NULL;
            return;
        }
    }

    /*
     * Avoid storing a pointer to the '\n' at the end of this line, if
     * there is one.
     */
    if ((length > 0) && (buffer[length - 1] == '\n')) {
        length--;
    }

    /* Store pointers to tabbed fields. */
    {
        int last = 0;

        tabfields[ctab++].field = buffer;

        for (i = 1; i < length; i++) {
            if (buffer[i - 1] == '\t') {
                tabfields[ctab].field = buffer + i;
                tabfields[ctab - 1].length = i - last;
                last = i;
                ctab++;
            }
        }
        tabfields[ctab - 1].length = i - last;
    }

    {
        int j;
        int len;
        struct tabstruct tmp;

        /* Sort them. */
        for (i = 0; i < ctab; i++) {
            for (j = i + 1; j < ctab; j++) {
                len = min(tabfields[i].length, tabfields[j].length);
                if (strncmp(tabfields[i].field, tabfields[j].field, len) > 0) {
                    tmp = tabfields[i];
                    tabfields[i] = tabfields[j];
                    tabfields[j] = tmp;
                }
            }
        }
    }

    {
        int len;
        char *s = spare;

        /* Output to spare buffer. */
        for (i = 0; i < ctab; i++) {
            len = tabfields[i].length;
            unix_strncpy(s, tabfields[i].field, len);
            s += len;
        }

        /*
         * Now copy back to buffer (but don't put \0 at end -- length
         * should be unaltered.
         */
        spare[length] = '\0';
        s = spare;
        while (*s != '\0') {
            *buffer++ = *s++;
        }
    }

    free(tabfields);
    free(spare);
    tabfields = NULL;
    spare = NULL;
}

/*
 * ASSUMES: line_starts is not NULL, with end entry marked by NULL.
 * 
 */
static void print_lines (char **line_starts, short field_width[],
			 boolean do_buginf, boolean do_formatting)
{
    int i = 0;
    char *s;
    helpType junk;

    while (line_starts[i] != NULL) {
        s = line_starts[i];
	paged_output(s, logical_line_length(s, &junk),
		     field_width, do_buginf, do_formatting);
        i++;
    }
}

/*
 * Figure out the length of the logical line in s. Also figure out what
 * type the line is.
 * 
 * ASSUMES: buffer end marked with '\0'.
 * 
 */
static int logical_line_length (char *s, helpType * ptype)
{
    int i;
    boolean looking = FALSE;

    *ptype = HELP_TEXT;

    for (i = 0; s[i] != '\0'; i++) {
        switch (s[i]) {
        case '\n':
            looking = TRUE;
            if ((i > 0) && (s[i - 1] == '\t')) {
                /* \t\n is the sign of a SHORT help line. */
                *ptype = HELP_SHORT;
            }
            break;
        case '\t':
            /* \n\t is a continuation line, can only be type LONG */
            *ptype = HELP_LONG;
            looking = FALSE;
            break;
        default:
            if (looking) {
                return(i);
            }
            break;
        }
    }
    if ((i > 0) && (s[i - 1] == '\t')) {
        /* \t\0 at the end is also a short help line. */
        *ptype = HELP_SHORT;
    }
    /* At end of buffer: */
    return(i);
}

/* print_caret  - prints a caret at the specified location */

void print_caret (register int offset)
{
    printf("%-*c", 1 + offset, '^');
}
