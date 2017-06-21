/* $Id: keyman.c,v 3.13.4.6 1996/07/16 08:00:53 raj Exp $
 * $Source: /release/112/cvs/Xsys/crypto/keyman.c,v $
 *------------------------------------------------------------------
 * Key management
 *
 * September 1995, Andy Heffernan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: keyman.c,v $
 * Revision 3.13.4.6  1996/07/16  08:00:53  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Inserted patches which were missing and update dirresp code as well.
 *
 * Revision 3.13.4.5  1996/06/08  19:41:24  che
 * CSCdi57850:  password encrytion not working correctly for vty
 * Branch: California_branch
 * Implement proper handling of line password structures when
 * password encryption is turned on.
 *
 * Revision 3.13.4.4  1996/05/21  09:44:27  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.13.4.3  1996/04/19  14:56:11  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.7  1996/04/09  20:02:48  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.1.2.6  1996/03/19  07:48:35  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.2.5  1996/02/29  19:03:41  bew
 * Branch: IosSec_branch
 * Commit of the private/encrypted NVRAM feature. This is in support
 * of the crypto project and allows portions of NVRAM to be
 * encrypted, and a portion to be private key storage for the IOS.
 *
 * Revision 3.1.2.4  1996/02/21  03:05:29  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 3.1.2.3  1996/01/23  04:59:10  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 3.1.2.2  1995/12/19  03:41:35  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 3.1.2.1  1995/12/17  06:22:54  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.13.4.2  1996/04/15  14:57:23  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.13.4.1  1996/03/18  19:10:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.2.3  1996/03/16  06:32:57  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.8.2.2  1996/03/07  08:42:04  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.2.1  1996/02/20  00:22:31  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.13  1996/03/09  22:54:27  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.12  1996/02/22  14:32:30  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.11  1996/02/07  16:10:35  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.10  1996/01/29  07:27:02  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.9  1996/01/29  00:29:54  billw
 * CSCdi13190:  Increase password characters encryted
 * Make sure that the maximum length password that can be encrypted is
 * the maximum length that will be accepted.
 *
 * Revision 3.8  1996/01/18  20:41:02  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.7  1996/01/03  03:31:56  ahh
 * CSCdi45528:  Move key manager out of core
 *
 * Revision 3.6  1995/12/17  18:25:31  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/12/02  10:47:43  pst
 * CSCdi44892:  tell the user which keys happen to be active right now
 * (make debugging a tad easier on them if they're crazy enough to use
 *  timed keys)
 *
 * Revision 3.4  1995/12/01  15:56:04  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.3  1995/11/18  01:59:54  ahh
 * CSCdi42411:  Key commands not NVGENed properly
 *
 * Revision 3.2  1995/11/17  08:57:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:14:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:53:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/11  19:35:27  pst
 * Add placeholders
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "config.h"
#include "parser.h"
#include "interface_private.h"
#include "subsys.h"
#include "sys_registry.h"
#include "login.h"
#include "logger.h"
#include "../parser/macros.h"
#include "../parser/parser_actions.h"
#include "../os/clock.h"
#include "../os/nv.h"
#include "keyman.h"

#undef KEY_DEBUGGING
#ifdef KEY_DEBUGGING
#define KEY_DEBUG(s) buginf s
#else
#define KEY_DEBUG(s)
#endif

/*
 * Pointers used in parseinfo structure.
 */

#define csb_keychainptr	userDataBlock[0]
#define csb_keyptr	userDataBlock[1]

/*
 * Structures used internally
 */

STRUCTURE_REF(key_lifetime);
STRUCTURE_REF(keytype);
STRUCTURE_REF(key_chain);

struct key_lifetime_ {
    clock_epoch start_time, stop_time;
    ushort flags;			/* lifetime flags */
};

#define KLF_SET		0x0001		/* lifetime value set */
#define KLF_INFINITE	0x0002		/* lifetime is infinite */
#define KLF_DURATION	0x0004		/* display duration for stop_time */

struct keytype_ {
    int id;				/* ID of this key */
    password_struct keytext;		/* Text of the key */
    key_lifetime accept_lifetime;	/* Accept lifetime */
    key_lifetime send_lifetime;		/* Send lifetime */
    list_element key_element;		/* Queueing */
    key_chain *chain;			/* Key chain this key is on */
};


#define KEY_VALID(key) (((key)->keytext.pw != NULL))
/*
 * usability kludge: keys are always valid if no time is set
 *
 *			&& ((key)->accept_lifetime.flags & KLF_SET) \
 *			&& ((key)->send_lifetime.flags & KLF_SET))
 */
#define KEY_LIFETIME_SET(key)	(((key)->accept_lifetime.flags & KLF_SET) || \
				 ((key)->send_lifetime.flags & KLF_SET))
#define KEY_NULL(key)	(((key)->keytext.pw == NULL) && !KEY_LIFETIME_SET(key))

struct key_chain_ {
    char *name;				/* Name of this key-chain */
    list_header keys;			/* List of keys on key-chain */
    list_element key_chain_element;	/* Queueing */
};

static list_header key_chains;
static parser_mode *keychain_config_mode;


/*
 * key_find_key
 *
 * Return the key with the given id from the chain.
 */

static keytype *
key_find_key (key_chain *chain, int id)
{
    keytype *key;
    list_element *el;

    FOR_ALL_DATA_IN_LIST(&chain->keys, el, key) {
	if (key->id == id)
	    return(key);
    }
    return(NULL);
}

/*
 * key_create_key
 *
 * Create a key and set its id.
 */

static keytype *
key_create_key (int id)
{
    keytype *key;

    key = malloc(sizeof(keytype));
    if (key == NULL)
	return(NULL);

    key->id = id;

    return(key);
}

/*
 * key_add_key
 *
 * Add a key to a chain.  Keys are sorted by key-id.
 */

static boolean
keysort (void *vkey1, void *vkey2)
{
    keytype *key1, *key2;

    key1 = vkey1;
    key2 = vkey2;
    return(key1->id - key2->id);
}

static void
key_add_key (key_chain *chain, keytype *key)
{
    list_insert(&chain->keys, &key->key_element, key, keysort);
    key->chain = chain;
}

/*
 * key_find_or_create_key
 *
 * Find or create a key on the given key chain.
 */

static keytype *
key_find_or_create_key (key_chain *chain, int id)
{
    keytype *key;

    key = key_find_key(chain, id);
    if (key == NULL) {
	key = key_create_key(id);
	if (key == NULL)
	    return(NULL);
	key_add_key(chain, key);
    }
    return(key);
}

/*
 * key_delete_key
 */

static void
key_delete_key (keytype *key)
{
    free(key->keytext.pw);
    free(key);
}

/*
 * key_find_key_chain
 *
 */

static key_chain *
key_find_key_chain (char *name)
{
    key_chain *chain;
    list_element *el;

    FOR_ALL_DATA_IN_LIST(&key_chains, el, chain) {
	if (strcmp(chain->name, name) == 0)
	    return(chain);
    }
    return(NULL);
}

/*
 * key_create_key_chain
 *
 */

static key_chain *
key_create_key_chain (char *name)
{
    key_chain *chain;

    chain = malloc(sizeof(key_chain));
    if (chain == NULL)
	return(NULL);

    setstring(&chain->name, name);
    list_create(&chain->keys, 0, "Keys", 0);

    return(chain);
}

/*
 * key_find_or_create_key_chain
 *
 */

static key_chain *
key_find_or_create_key_chain (char *name)
{
    key_chain *chain;

    chain = key_find_key_chain(name);
    if (chain == NULL) {
	chain = key_create_key_chain(name);
	if (chain == NULL)
	    return(NULL);
	list_enqueue(&key_chains, &chain->key_chain_element, chain);
    }
    return(chain);
}

/*
 * key_delete_key_chain
 */

static void
key_delete_key_chain (key_chain *chain)
{
    keytype *key;
    list_element *el, *next;

    /* Remove the key chain from the master list if it's there. */
    if (LIST_ELEMENT_QUEUED(&chain->key_chain_element))
	list_remove(&key_chains, &chain->key_chain_element, NULL);

    /* Remove and delete all the keys on the chain. */
    FOR_ALL_DATA_IN_LIST_W_NEXT(&chain->keys, el, next, key) {
	list_remove(&chain->keys, &key->key_element, NULL);
	key_delete_key(key);
    }
    list_destroy(&chain->keys);

    /* Free the key chain superstructure. */
    free(chain->name);
    free(chain);
}

/*
 * Externals
 */

static boolean
key_alive_internal (keytype *key, clock_epoch *when,
		    enum key_lifetime_type type)
{
    key_lifetime *lifetime;

    if (type == KEY_SEND_LIFETIME)
	lifetime = &key->send_lifetime;
    else
	lifetime = &key->accept_lifetime;

    if (!(lifetime->flags & KLF_SET))		/* always valid if unset */
	return(TRUE);

    if ((clock_epoch_compare(&lifetime->start_time, when) <= 0) &&
	((clock_epoch_compare(&lifetime->stop_time, when) >= 0) ||
	 lifetime->flags & KLF_INFINITE))
	return(TRUE);
    else
	return(FALSE);
}

/*
 * key_number_live_keys
 *
 * Return the number of keys that are alive now.
 */

/*static*/ int
key_number_live_keys (char *name, clock_epoch *when,
		      enum key_lifetime_type type)
{
    key_chain *chain;
    list_element *el;
    keytype *key;
    int count;
    clock_epoch now;

    if (when == NULL) {
	clock_get_time(&now);
	when = &now;
    }

    chain = key_find_key_chain(name);
    if (chain == NULL)
	return(0);

    count = 0;
    FOR_ALL_DATA_IN_LIST(&chain->keys, el, key) {
	if (KEY_VALID(key) && key_alive_internal(key, when, type))
	    count++;
    }
    return(count);
}

/*
 * key_get_live_keys
 *
 * Return the text strings of the keys that are alive at a certain
 * point in time.  A NULL clock_epoch pointer means now.  The
 * strings returned must be freed later with key_free_keys().
 */

static int
key_get_live_keys (char *name, clock_epoch *when, char **textptrs, int num,
		   enum key_lifetime_type type)
{
    key_chain *chain;
    list_element *el;
    keytype *key;
    int count;
    clock_epoch now;

    if (when == NULL) {
	clock_get_time(&now);
	when = &now;
    }

    chain = key_find_key_chain(name);
    if (chain == NULL)
	return(0);

    count = 0;
    FOR_ALL_DATA_IN_LIST(&chain->keys, el, key) {
	if (KEY_VALID(key) && key_alive_internal(key, when, type)) {
	    char clear_password[PASSWORDLEN+1];		/* XXX too short */

	    if (key->keytext.encrypt_type == ENCRYPT_TEMP) {
		decrypt(key->keytext.pw, clear_password, PASSWORDLEN);
		setstring(&textptrs[count++], clear_password);
	    } else {
		setstring(&textptrs[count++], key->keytext.pw);
	    }
	    if (count == num)
		break;
	}
    }
    return(count);
}

/*
 * key_get_key_id
 */
static int key_get_key_id (char *chain_name, char *key_text)
{
    key_chain *chain;
    list_element *el;
    keytype *key;

    chain = key_find_key_chain(chain_name);
    if (chain == NULL)
	return(-1L);

    FOR_ALL_DATA_IN_LIST(&chain->keys, el, key) {    
	char clear_password[PASSWORDLEN+1];		/* XXX too short */
	if (key->keytext.encrypt_type == ENCRYPT_TEMP) {
	    decrypt(key->keytext.pw, clear_password, PASSWORDLEN);
	    if (!strcmp (clear_password, key_text))
	       return(key->id);
	} else {
	    if (!strcmp (key->keytext.pw, key_text))
		return(key->id);
	}
    }
    return(-1L);
}

/*
 * key_get_next_live_key
 *
 * Return the next live key (with an id >= to the one specified) from chain.
 * A NULL clock_epoch pointer means now.  The
 */

static char *
key_get_next_live_key (char *name, int *id,
		       clock_epoch *when, enum key_lifetime_type type,
		       char *storage, int size)
{
    key_chain *chain;
    list_element *el;
    keytype *key;
    clock_epoch now;

    if (when == NULL) {
	clock_get_time(&now);
	when = &now;
    }

    chain = key_find_key_chain(name);
    if (chain == NULL)
	return NULL;

    FOR_ALL_DATA_IN_LIST(&chain->keys, el, key) {
	if (key->id < *id)
	    continue;

	if (KEY_VALID(key) && key_alive_internal(key, when, type)) {
	    *id = key->id;
	    if (key->keytext.encrypt_type == ENCRYPT_TEMP)
		decrypt(key->keytext.pw, storage, size);
	    else
		strncpy(storage, key->keytext.pw, size);
	    return storage;
	}
    }
    return NULL;
}

/*
 * key_free_keys
 *
 * Free the text strings returned by key_get_alive_keys().
 */

static void
key_free_keys (char **textptrs, int num)
{
    int i;

    for (i = 0; i < num; i++) {
	free(textptrs[i]);
	textptrs[i] = NULL;
    }
}

/*
 * key_alive
 *
 * Tell if a key is alive at a certain point in time.
 */

static boolean
key_alive (char *name, int id, clock_epoch *when, enum key_lifetime_type type)
{
    key_chain *chain;
    keytype *key;
    clock_epoch now;

    clock_get_time(&now);

    chain = key_find_key_chain(name);
    if (chain == NULL)
	return(FALSE);

    key = key_find_key(chain, id);
    if (key == NULL)
	return(FALSE);

    return(key_alive_internal(key, &now, type));
}

/*
 * key_alive_now
 *
 * Tell if a key is alive now.
 */

/*static*/ boolean
key_alive_now (char *name, int id, enum key_lifetime_type type)
{
    clock_epoch now;

    clock_get_time(&now);
    return(key_alive(name, id, &now, type));
}

/*
 * key_text
 *
 * Fetch the key text for a given key
 * The key text string must be freed with key_text_free().
 */

/*static*/ char *
key_text (char *name, int id, char **textptr)
{
    key_chain *chain;
    keytype *key;

    chain = key_find_key_chain(name);
    if (chain == NULL)
	return(NULL);

    key = key_find_key(chain, id);
    if (key == NULL)
	return(NULL);

    if (key->keytext.encrypt_type == ENCRYPT_TEMP) {
	char clear_password[PASSWORDLEN+1];	/* XXX too short */

	decrypt(key->keytext.pw, clear_password, PASSWORDLEN);
	setstring(textptr, clear_password);
    } else {
	setstring(textptr, key->keytext.pw);
    }
    return(*textptr);
}

/*
 * key_text_free
 *
 * Free the text string returned by key_text().
 */

/*static*/ void
key_text_free (char **textptr)
{
    free(*textptr);
    *textptr = NULL;
}

/*
 * Parser support and user interface
 */

static void
exit_keychain_config_submode (parseinfo *csb)
{
    csb->mode = keychain_config_mode;
}

/*
 * Parse chains for top-level show commands
 */
#define ALTERNATE	NONE
#include "exec_show_key.h"
LINK_POINT(key_show, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for top-level configuration commands
 */
#define ALTERNATE	NONE
#include "cfg_key.h"
LINK_POINT(key_cfg, ALTERNATE);
#undef ALTERNATE

static parser_extension_request key_chain_table[] = {
    { PARSE_ADD_CFG_TOP_CMD, &pname(key_cfg) },
    { PARSE_ADD_SHOW_CMD, &pname(key_show) },
    { PARSE_LIST_END, NULL }
};

/*
 * Show command(s)
 */

static void
show_key_lifetime (key_lifetime *lifetime, char *lifetime_name)
{
    char buf1[32], buf2[32];
    char *s1, *s2;
    timeval start_time, stop_time;

    if (lifetime->flags & KLF_SET) {
	clock_epoch_to_timeval(&lifetime->start_time, &start_time, 0);
	format_time(buf1, sizeof(buf1), "%T %E", &start_time, NULL);
	s1 = buf1;
    } else {
	s1 = "always valid";			/* always valid if unset */
    }
    if (lifetime->flags & KLF_SET) {
	if (lifetime->flags & KLF_INFINITE) {
	    s2 = "infinite";
	} else if (lifetime->flags & KLF_DURATION) {
	    sprintf(buf2, "%d seconds", lifetime->stop_time.epoch_secs -
		    lifetime->start_time.epoch_secs);
	    s2 = buf2;
	} else {
	    clock_epoch_to_timeval(&lifetime->stop_time, &stop_time, 0);
	    format_time(buf2, sizeof(buf2), "%T %E", &stop_time, NULL);
	    s2 = buf2;
	}
    } else {
	s2 = "always valid";			/* always valid if unset */
    }
    printf("\n        %s (%s) - (%s)", lifetime_name, s1, s2);
}

static void
show_key (keytype *key)
{
    char clear_password[PASSWORDLEN+1];
    clock_epoch now;

    clock_get_time(&now);

    printf("\n  %c key %d -- text \"", KEY_VALID(key) ? ' ' : '*', key->id);
    if (key->keytext.pw == NULL)
	printf("(unset)\"");
    else if (key->keytext.encrypt_type == ENCRYPT_TEMP) {
	decrypt(key->keytext.pw, clear_password, PASSWORDLEN);
	printf("%s\"", clear_password);
    } else
	printf("%s\"", key->keytext.pw);

    show_key_lifetime(&key->accept_lifetime, "accept lifetime");
    if (key_alive_internal(key, &now, KEY_ACCEPT_LIFETIME))
	printf(" [valid now]");

    show_key_lifetime(&key->send_lifetime, "send lifetime");
    if (key_alive_internal(key, &now, KEY_SEND_LIFETIME))
	printf(" [valid now]");
}

static void
show_key_chain (key_chain *chain)
{
    keytype *key;
    list_element *el;

    printf("\nKey-chain %s:", chain->name);
    FOR_ALL_DATA_IN_LIST(&chain->keys, el, key) {
	show_key(key);
    }
}

#ifdef KEY_DEBUGGING
static void
key_test (char *name)
{
    clock_epoch now;
    int num, num_returned;
    char **textptrs;
    int i;
    char *keytext;

    /*
     * We want to send a packet.
     * Find out what keys are active now.
     */
    clock_get_time(&now);

    num = key_number_live_keys(name, &now, KEY_SEND_LIFETIME);
    if (num == 0) {
	printf("\nno live keys now");
	return;
    }

    textptrs = malloc(sizeof(char *) * num);
    if (textptrs == NULL) {
	printf(nomemory);
	return;
    }

    num_returned = key_get_live_keys(name, &now, textptrs, num,
				     KEY_SEND_LIFETIME);
    for (i = 0; i < num_returned; i++)
	printf("\nkey %d: %s", i, textptrs[i]);
    key_free_keys(textptrs, num_returned);
    free(textptrs);

    /*
     * We got key 0 (or 1 or 2 or 3...).
     * Find out if it is valid.
     */
    keytext = NULL;
    for (i = 0; i < 10; i++) {
	if (key_alive(name, i, &now, KEY_ACCEPT_LIFETIME)) {
	    printf("\nkey %d (%s) is valid now", i, key_text(name, i,
							     &keytext));
	    key_text_free(&keytext);
	} else
	    printf("\nkey %d is not valid now", i);
    }

}
#endif

void
show_key_command (parseinfo *csb)
{
    key_chain *chain;
    list_element *el;
    char *name;

    name = GETOBJ(string,1);
    if (name && name[0] != '\0') {
	chain = key_find_key_chain(name);
	if (chain) {
	    automore_enable(NULL);
	    show_key_chain(chain);
	}
#ifdef KEY_DEBUGGING
	key_test(name);
#endif
    } else if (!LIST_EMPTY(&key_chains)) {
	automore_enable(NULL);
	FOR_ALL_DATA_IN_LIST(&key_chains, el, chain) {
	    if (automore_quit())
		break;
	    show_key_chain(chain);
	}
	automore_disable();
    }
}

/*
 * Configuration command helpers
 */

static void
key_set_text (keytype *key, uchar encrypt_type, char *keytext)
{
    char temppassword[PASSWORDLEN_ENCRYPTED+1];

    /*
     * If we are encrypting and we got a non-encrypted
     * password, encrypt it.
     */
    if ((encrypt_type == ENCRYPT_NONE) && password_encryption_enable) {
	memset(temppassword, 0, sizeof(temppassword));
	encrypt_type = encrypt(keytext, temppassword, 0,
			       PASSWORDLEN_ENCRYPTED);
	keytext = temppassword;
    }
    setstring(&key->keytext.pw, keytext);
    key->keytext.encrypt_type = encrypt_type;
}

static void
key_set_lifetime (key_lifetime *lifetime, timeval *start_tv, timeval *stop_tv,
		  long duration, boolean infinite)
{
    clock_timeval_to_epoch(start_tv, &lifetime->start_time);
    if (infinite) {
	lifetime->flags |= KLF_INFINITE;
    } else {
	lifetime->flags &= ~KLF_INFINITE;
	if (duration) {
	    clock_timeval_to_epoch(start_tv, &lifetime->stop_time);
	    lifetime->stop_time.epoch_secs += duration;
	    lifetime->flags |= KLF_DURATION;
	} else {
	    clock_timeval_to_epoch(stop_tv, &lifetime->stop_time);
	    lifetime->flags &= ~KLF_DURATION;
	}
    }
    lifetime->flags |= KLF_SET;
}

static void
key_set_send_lifetime (keytype *key, timeval *start_tv, timeval *stop_tv,
		       long duration, boolean infinite)
{
    key_set_lifetime(&key->send_lifetime, start_tv, stop_tv, duration,
		     infinite);
}

static void
key_set_accept_lifetime (keytype *key, timeval *start_tv, timeval *stop_tv,
			 long duration, boolean infinite)
{
    key_set_lifetime(&key->accept_lifetime, start_tv, stop_tv, duration,
		     infinite);
}

/*
 * key_nv_lifetime
 *
 * Helper code to nvgen a lifetime.
 */

static void
key_nv_lifetime (key_lifetime *lifetime, key_chain *chain,
		 keytype *key, char *prefix)
{
    char buf1[32], buf2[32];
    char *s2;
    timeval start_time, stop_time;

    if (lifetime->flags & KLF_SET) {
	clock_epoch_to_timeval(&lifetime->start_time, &start_time, 0);
	format_time(buf1, sizeof(buf1), "%T %E", &start_time, NULL);
	if (lifetime->flags & KLF_INFINITE) {
	    s2 = "infinite";
	} else if (lifetime->flags & KLF_DURATION) {
	    sprintf(buf2, "duration %d", lifetime->stop_time.epoch_secs -
		    lifetime->start_time.epoch_secs);
	    s2 = buf2;
	} else {
	    clock_epoch_to_timeval(&lifetime->stop_time, &stop_time, 0);
	    format_time(buf2, sizeof(buf2), "%T %E", &stop_time, NULL);
	    s2 = buf2;
	}
	nv_write(TRUE, "%s %s %s", prefix, buf1, s2);
    }
}

/*
 * key_command_key_keyword_nvgen
 *
 * Handle NVGEN chores for the keywords in the key-chain-key submode
 */

void
key_command_key_keyword_nvgen (parseinfo *csb)
{
    key_chain *chain;
    keytype *key;
    char temppassword[PASSWORDLEN_ENCRYPTED+1];

    key = csb->csb_keyptr;
    chain = key->chain;

    /*
     * XXX We are in a sub-sub-mode, so we need to indent twice,
     * but nv_write() can't handle this.  It just knows to indent
     * once because we are not in config_mode.  So we throw in
     * an extra space emitting the keywords here.  BAD.
     */
    switch (csb->which) {
    case KEY_STRING:
	nv_password(csb, " key-string", PRIV_MAX, NULL, &key->keytext,
		    temppassword, PASSWORDLEN_ENCRYPTED);
	break;
    case KEY_LIFETIME:
	switch (GETOBJ(int,1)) {
	case KEY_SEND_LIFETIME:
	    key_nv_lifetime(&key->send_lifetime, chain, key,
			    " send-lifetime");
	    break;
	case KEY_ACCEPT_LIFETIME:
	    key_nv_lifetime(&key->accept_lifetime, chain, key,
			    " accept-lifetime");
	    break;
	}
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * key_command_key_nvgen
 *
 * Handle NVGEN chores for the keywords in the key-chain submode
 */

void
key_command_key_nvgen (parseinfo *csb)
{
    char *nv_command = NULL;
    key_chain *chain;
    keytype *key;
    list_element *el;

    switch (csb->which) {
    case KEY_ID:
	setstring(&nv_command, csb->nv_command);

	chain = csb->csb_keychainptr;
	FOR_ALL_DATA_IN_LIST(&chain->keys, el, key) {
	    nv_write(TRUE, "%s %d", csb->nv_command, key->id);
	    if (set_mode_byname(&csb->mode, "key-chain-key", MODE_VERBOSE)) {
		*csb->nv_command = '\0';
		csb->csb_keyptr = key;
		recurse_node(csb, get_mode_nv(csb));
		strcpy(csb->nv_command, nv_command);
	    }
	}

	free(nv_command);
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * key_command_nvgen
 *
 * Handle NVGEN chores for the key keywords.
 */

void
key_command_nvgen (parseinfo *csb)
{
    char *nv_command = NULL;
    key_chain *chain;
    list_element *el;

    switch (csb->which) {
    case KEY_CHAIN:
	if (LIST_EMPTY(&key_chains))
	    break;

	nv_write(TRUE, "!");

	setstring(&nv_command, csb->nv_command);

	FOR_ALL_DATA_IN_LIST(&key_chains, el, chain) {
	    if (!LIST_EMPTY(&chain->keys)) {
		nv_write(TRUE, "%s %s", csb->nv_command, chain->name);
		if (set_mode_byname(&csb->mode, "key-chain", MODE_VERBOSE)) {
		    *csb->nv_command = '\0';
		    csb->csb_keychainptr = chain;
		    recurse_node(csb, get_mode_nv(csb));
		    strcpy(csb->nv_command, nv_command);
		}
	    }
	}

	free(nv_command);
	break;
    case KEY_CONFIG_KEY:
	nv_store_master_keys();
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * key_command
 *
 * Parse top-level "key" config commands.
 */

void
key_command (parseinfo *csb)
{
    key_chain *chain;

    switch (csb->which) {
    case KEY_CHAIN:
	if (csb->sense) {
	    chain = key_find_or_create_key_chain(GETOBJ(string,1));
	    if (chain == NULL) {
		printf(nomemory);
		return;
	    }
	    csb->csb_keychainptr = chain;
	    set_mode_byname(&csb->mode, "key-chain", MODE_VERBOSE);
	} else {
	    chain = key_find_key_chain(GETOBJ(string,1));
	    if (chain == NULL) {
		printf("\n%% No such key-chain");
		return;
	    }
	    key_delete_key_chain(chain);
	}
	break;
    case KEY_CONFIG_KEY:
	/* 
	 * Only allow the key to be set if it is coming from from a 
	 * 'configure terminal' or from private NVRAM!
	 */
	if ((csb->resolvemethod == RES_PRIVATE_NONVOL) ||
	     (csb->flags & CONFIG_TERM)) {
	    nv_set_master_key(GETOBJ(int,2), GETOBJ(string,1), csb->sense);
	} 
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * key_chain_command
 *
 * Parse subcommands under key-chain mode.
 */

void
key_chain_command (parseinfo *csb)
{
    key_chain *chain;
    keytype *key;

    chain = csb->csb_keychainptr;
    if (chain == NULL) {
	/* "shouldn't happen" */
	return;
    }

    switch (csb->which) {
    case KEY_ID:
	if (csb->sense) {
	    key = key_find_or_create_key(chain, GETOBJ(int,1));
	    if (key == NULL) {
		printf(nomemory);
		return;
	    }
	    csb->csb_keyptr = key;
	    set_mode_byname(&csb->mode, "key-chain-key", MODE_VERBOSE);
	} else {
	    key = key_find_key(chain, GETOBJ(int,1));
	    if (key == NULL) {
		printf("\n%% No such key on key-chain %s", chain->name);
		return;
	    }
	    list_remove(&chain->keys, &key->key_element, NULL);
	    key_delete_key(key);
	    if (LIST_EMPTY(&chain->keys)) {
		key_delete_key_chain(chain);
		exit_config_submode(csb);
	    }
	}
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * key_chain_key_command
 *
 * Parse subcommands under key-chain-key mode.
 */

void
key_chain_key_command (parseinfo *csb)
{
    key_chain *chain;
    keytype *key;
    timeval start_tv, stop_tv;
    long duration;

    key = csb->csb_keyptr;
    if (key == NULL) {
	/* "shouldn't happen" */
	return;
    }
    chain = key->chain;

    switch (csb->which) {
    case KEY_STRING:
	if (csb->sense) {
	    key_set_text(key, GETOBJ(int,1), GETOBJ(string,1));
	} else {
	    free(key->keytext.pw);
	    key->keytext.pw = NULL;
	    if (KEY_NULL(key)) {
		chain = key->chain;
		list_remove(&chain->keys, &key->key_element, NULL);
		key_delete_key(key);
		csb->csb_keyptr = NULL;
		if (LIST_EMPTY(&chain->keys)) {
		    key_delete_key_chain(chain);
		    csb->csb_keychainptr = NULL;
		    exit_config_submode(csb);
		}
	    }
	}
	break;
    case KEY_LIFETIME:
	if (csb->sense) {
	    start_tv.hour = GETOBJ(int,2);
	    start_tv.minute = GETOBJ(int,3);
	    start_tv.second = GETOBJ(int,4);
	    start_tv.millisecond = 0;
	    start_tv.month = GETOBJ(int,5);
	    start_tv.day = GETOBJ(int,6);
	    start_tv.year = GETOBJ(int,7);
	    start_tv.tz_offset = 0;
	    stop_tv.hour = GETOBJ(int,8);
	    stop_tv.minute = GETOBJ(int,9);
	    stop_tv.second = GETOBJ(int,10);
	    stop_tv.millisecond = 0;
	    stop_tv.month = GETOBJ(int,11);
	    stop_tv.day = GETOBJ(int,12);
	    stop_tv.year = GETOBJ(int,13);
	    stop_tv.tz_offset = 0;
	    duration = GETOBJ(int,14);
	    switch (GETOBJ(int,1)) {
	    case KEY_ACCEPT_LIFETIME:
		key_set_accept_lifetime(key, &start_tv, &stop_tv, duration,
					duration == -1);
		break;
	    case KEY_SEND_LIFETIME:
		key_set_send_lifetime(key, &start_tv, &stop_tv, duration,
				      duration == -1);
		break;
	    }
	} else {
	    switch (GETOBJ(int,1)) {
	    case KEY_ACCEPT_LIFETIME:
		memset(&key->accept_lifetime, 0, sizeof(key_lifetime));
		break;
	    case KEY_SEND_LIFETIME:
		memset(&key->send_lifetime, 0, sizeof(key_lifetime));
		break;
	    }
	    if (KEY_NULL(key)) {
		chain = key->chain;
		list_remove(&chain->keys, &key->key_element, NULL);
		key_delete_key(key);
		csb->csb_keyptr = NULL;
		if (LIST_EMPTY(&chain->keys)) {
		    key_delete_key_chain(chain);
		    csb->csb_keychainptr = NULL;
		    exit_config_submode(csb);
		}
		
	    }
	}
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * key_init
 *
 * Initialize the key subsystem.
 */

static void
key_init (subsystype *subsys)
{
    parser_add_command_list(key_chain_table, "key");
    keychain_config_mode = parser_add_mode("key-chain", "config-keychain",
					   "Key-chain configuration mode",
					   TRUE, FALSE, "configure", NULL,
					   NULL, &pname(top_conf_kc), NULL);
    parser_add_mode("key-chain-key", "config-keychain-key",
		    "Key-chain key configuration mode",
		    TRUE, FALSE, "key-chain", NULL, NULL,
		    &pname(top_conf_kc_key), NULL);

    list_create(&key_chains, 0, "Key chains", 0);

    /*
     * Add some stubs (ugh)
     * These functions could also be exported:
     *     key_number_live_keys
     *     key_alive
     *     key_alive_now
     *     key_text
     *     key_text_free
     * (The typecast below is so that clock_epoch doesn't appear
     * in sys_registry.reg)
     */
    reg_add_key_get_live_keys((service_key_get_live_keys_type)
			      key_get_live_keys, "");
    reg_add_key_free_keys(key_free_keys, "");
    reg_add_key_get_next_live_key((service_key_get_next_live_key_type)
				  key_get_next_live_key, "");
    reg_add_key_get_key_id(key_get_key_id, "");
    reg_add_key_text(key_text, "");
    reg_add_key_text_free(key_text_free, "");
    reg_add_key_alive_now((service_key_alive_now_type)
			  key_alive_now, "");
}

/*
 * Key subsystem header
 */

#define KEYMAN_MAJVERSION	1
#define KEYMAN_MINVERSION	0
#define KEYMAN_EDITVERSION	1

SUBSYS_HEADER(keyman, KEYMAN_MAJVERSION, KEYMAN_MINVERSION,
	      KEYMAN_EDITVERSION, key_init, SUBSYS_CLASS_LIBRARY,
	      NULL,
	      NULL);
