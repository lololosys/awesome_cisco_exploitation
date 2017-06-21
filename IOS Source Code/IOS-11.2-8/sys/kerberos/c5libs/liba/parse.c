/* $Id: parse.c,v 3.2.60.1 1996/04/29 20:12:15 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/c5libs/liba/parse.c,v $
 *------------------------------------------------------------------
 * parse.c - Converts string int Principle ID
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parse.c,v $
 * Revision 3.2.60.1  1996/04/29  20:12:15  che
 * CSCdi55541:  Router Crashes When using Kerberos Authentication
 * Branch: California_branch
 * Add missing "break" into CyberSAFE code.
 *
 * Revision 3.2  1995/11/17  17:44:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original parse.c 3.2 6/14/94*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Rewritten by Theodore Ts'o to properly handle arbitrary quoted
 * characters in the principal name.
 */


#include <csfc5/header.h>

/*
 * converts a single-string representation of the name to the
 * multi-part principal format used in the protocols.
 * 
 * Conventions:  / is used to separate components.  If @ is present in the
 * string, then the rest of the string after it represents the realm name.
 * Otherwise the local realm name is used.
 * 
 * error return:
 *    CSFC5_STS_PARSE_MALFORMED    badly formatted string
 *
 * also returns system errors:
 *    CSF_STS_ENOMEM    csfCmalloc failed/out of memory
 *
 * get_default_realm() is called; it may return other errors.
 */

#define csfC5parse_ESCCHR		'\\'
#define csfC5parse_EOS			'\0'
#define csfC5parse_FCN		2


/*
 * May the fleas of a thousand camels infest the ISO, they who think
 * that arbitrarily large multi-component names are a Good Thing.....
 */

csf_sts csfC5strTo_prnId (
	const char *name,
	csfC5_prnId *nprincipal
){
	const char *cp;
	char *q;
	int i, c, size;
	int components = 0;
	const char *parsed_realm = NULL;
	int fcompsize[csfC5parse_FCN];
	int realmsize = 0;
	char *default_realm = NULL;
	char *tmpdata;
	csfC5_prnId tprinc;
	csf_sts rsts = CSF_STS_OK;


	/*
	 * Pass 1.  Find out how many components there are to the name,
	 * and get string sizes for the first FCN components.
	 */

	size = 0;
	for (i =0, cp = name; (c = *cp); cp++) {
		switch (c) {

		case csfC5parse_ESCCHR:
			c = *++cp;
			if (!c) {
				/* csfC5parse_ESCCHR can't be at the last
				 * character of the name!
				 */
				return CSFC5_STS_PARSE_MALFORMED;
			}
			++size;
			break;

		case CSFC5c_NAMEC_COMPONENT_SEP:
			if (parsed_realm) {
				/* Shouldn't see a component separator
				 * after we've parsed out the realm name!
				 */
				return CSFC5_STS_PARSE_MALFORMED;
			}
			if (i < csfC5parse_FCN) {
				fcompsize[i] = size;
			}
			size = 0;
			++i;
			break;

		case CSFC5c_NAMEC_REALM_SEP:
			if ((parsed_realm) || (!*(cp+1))) {
				/* Multiple realm separators and/or
				 * null realm names are not allowed!
				 */
				return CSFC5_STS_PARSE_MALFORMED;
			}
			parsed_realm = cp + 1;
			if (i < csfC5parse_FCN) {
				fcompsize[i] = size;
			}
			size = 0;
			break;

		default:
			++size;
			break;
		}
	}

	if (parsed_realm) {
		realmsize = size;
	}
	else {
		if (i < csfC5parse_FCN) {
			fcompsize[i] = size;
		}
	}

	components = i + 1;


	/*
	 * Now, we allocate the principal component pieces
	 */

	tprinc.name.pca = (csfC5_prnNC *)
		csfCcmalloc( sizeof(csfC5_prnNC) * components );
	if (!tprinc.name.pca) {
		rsts = CSF_STS_ENOMEM;
		goto clean_exit;
	}
	tprinc.name.pcn = components;


	/*
	 * If a realm was not found, then we need to find the defualt
	 * realm....
	 */

	if (!parsed_realm) {
		rsts = csfC5default_HostRealm( &default_realm );
		if (rsts) {
			/* can't get default realm -- surrender */
			return rsts;
		}
		tprinc.realm.namec.dl =
			realmsize = csfCstrlen(default_realm);
	}


	/*
	 * Pass 2.  Happens only if there were more than FCN
	 * components; if this happens, someone should be shot
	 * immediately.  Nevertheless, we will attempt to handle said
	 * case..... <martyred sigh>
	 */

	if (components >= csfC5parse_FCN) {
		size = 0;
		parsed_realm = NULL;
		for (i = 0, cp = name; (c = *cp); cp++) {
			switch (c) {

			case csfC5parse_ESCCHR:
				++cp;
				++size;
				break;

			case CSFC5c_NAMEC_COMPONENT_SEP:
				tprinc.name.pca[i].namec.dl = size;
				size = 0;
				++i;
				break;

			case CSFC5c_NAMEC_REALM_SEP:
				tprinc.name.pca[i].namec.dl = size;
				size = 0;
				parsed_realm = cp+1;
				break;

			default:
				++size;
				break;

			}
		}

		if (parsed_realm) {
			tprinc.realm.namec.dl = size;
		}
		else {
			tprinc.name.pca[i].namec.dl = size;
		}

		_csfDBGassert( (i + 1) == components );
	} 
	else {
		/*
		 * If there were fewer than FCOMPSIZE components (the
		 * usual case), then just copy the sizes to the
		 * principal structure
		 */
		for (i = 0; i < components; i++) {
			tprinc.name.pca[i].namec.dl = fcompsize[i];
		}
		tprinc.realm.namec.dl = realmsize;
	}


	/*    
	 * Now, we need to allocate the space for the strings themselves.....
	 */

	tmpdata = csfCmalloc( realmsize + 1 );
	if (!tmpdata) {
		rsts = CSF_STS_ENOMEM;
		goto clean_exit;
	}

	tprinc.realm.namec.dp = tmpdata;
	for (i = 0; i < components; i++) {
		char *tmpdata;

		tmpdata = csfCmalloc( tprinc.name.pca[i].namec.dl + 1 );
		if (!tmpdata) {
			rsts = CSF_STS_ENOMEM;
			goto clean_exit;
		}
		tprinc.name.pca[i].namec.dp = tmpdata;
	}


	/*
	 * Pass 3.  Now we go through the string a *third* time, this
	 * time filling in the csfC5_prnId structure which we just
	 * allocated.
	 */

	q = tprinc.name.pca[0].namec.dp;
	for (i = 0, cp = name; (c = *cp); cp++) {
		switch (c) {

		case csfC5parse_ESCCHR:
			cp++;
			switch (c = *cp) {
			case 'n':
				*q++ = '\n';
				break;
			case 't':
				*q++ = '\t';
				break;
			case 'b':
				*q++ = '\b';
				break;
			case '0':
				*q++ = '\0';
				break;
			default:
				*q++ = c;
				break;
			}
			break;

		case CSFC5c_NAMEC_COMPONENT_SEP:
			++i;
			*q++ = csfC5parse_EOS;
			q = tprinc.name.pca[i].namec.dp;
			break;

		case CSFC5c_NAMEC_REALM_SEP:
			++i;
			*q++ = csfC5parse_EOS;
			q = tprinc.realm.namec.dp;
			break;

		default:
			*q++ = c;
			break;

		}
	}

	*q++ = csfC5parse_EOS;
	if (!parsed_realm) {
		csfCstrcpy( tprinc.realm.namec.dp, default_realm );
	}


	/*
	 * Alright, we're done.  Now stuff this monstrosity
	 * into the return variable, and let's get out of here.
	 */

	tprinc.name.prn_type = CSFC5c_NT_PRINCIPAL;

clean_exit:
	if (rsts) {
		csfC5free_prnId( &tprinc );
	}

	*nprincipal = tprinc;
	return rsts;
}


#ifdef UNIT_TEST

#include <csfc5/header.h>

main()
{
	char *s;
	csf_sts e;
	csfC5_prnId p;
	int z;

	s = csfCmalloc( BUFSIZ );
	while (1) {
		csfC5clear_prnId( &p );
		printf( "Principal name: " );
		if (gets( s )) {
			printf( "Before csfC5strTo_prnId, principal = %s\n", s );
			e = csfC5strTo_prnId( s, &p );
			if (e) {
				printf( "csfC5strTo_prnId failed %08X", e );
			}
			else {
				*s = '\0';
				(void) csfC5toStr_prnId( p, &s, &z );
				printf( "After csfC5strTo_prnId, principal = %s\n", s );
				csfC5free_prnId( &p );
			}
		}
		else {
			putc( '\n', stdout );
			csfCfree( s );
			exit( 0 );
		}
	}
}

#endif /* UNIT_TEST */
