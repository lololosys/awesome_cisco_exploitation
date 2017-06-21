/* $Id: ilmi_encode.c,v 3.4.40.4 1996/09/11 17:38:35 snyder Exp $
 * $Source: /release/112/cvs/Xsys/atm/ilmi_encode.c,v $
 *------------------------------------------------------------------
 *
 * March, 1995   Kartik Chandrasekhar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ilmi_encode.c,v $
 * Revision 3.4.40.4  1996/09/11  17:38:35  snyder
 * CSCdi68797:  char pointers to char arrays and make const
 *              120 data, 152 image
 * Branch: California_branch
 *
 * Revision 3.4.40.3  1996/08/23  16:48:59  kartik
 * CSCdi56903:  ILMI: problem with subinter > 128 (0x80)
 * Branch: California_branch
 *
 * Revision 3.4.40.2  1996/07/10  22:20:39  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.4.40.1  1996/03/18  18:59:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.2  1996/03/07  08:30:40  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.14.1  1996/02/20  13:35:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/05  22:29:53  kartik
 * CSCdi42922:  ILMI should do bookkeeping about registrations/deregs
 * ILMI address registration will now ignore the selector byte.
 *
 * Revision 3.3  1995/11/24  16:37:18  jjohnson
 * CSCdi42637:  platform-specific code should be moved out of sub_snmp
 * Install an API for accessing platform-specific SNMP objects
 *
 * Revision 3.2  1995/11/17  08:46:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/19  21:28:13  kartik
 * CSCdi36096:  Fixed mallocs in ILMI which were not being checked for
 * return
 * code.
 *
 * Revision 2.2  1995/06/18  20:31:45  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:12:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "string.h"
#include "interface_private.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_platform_api.h"
#include "ilmi_public.h"
#include "ilmi_private.h"
#include "ilmi_encode.h"


static const unsigned char zero_int_code[] = {0x2,0x1,0x0};
static const unsigned char comm_str_code[] = {0x4,0x4,0x49,0x4c,0x4d,0x49};
static const uchar null_value_buf[] = {5,0};
static const unsigned char ilmitrap_agent_addrbuf[] = {0x40,0x4,0x0,0x0,0x0,0x0}; 
static const unsigned char trap_spec_VCCchange[] = {0x2,0x1,0x2}; 
static const unsigned char trap_gen_enterprise[] = {0x2,0x1,0x6}; 
static const unsigned char trap_gen_coldstart[] = {0x2,0x1,0x0}; 
static const uchar atmforum_ent_namebuf[] = {6,7,43,6,1,4,1,130,97};
static const uchar vccchange_name_buf[] = {6,12,43,6,1,4,1,130,97,2,5,1,1};
static const uchar vccchange_name_bufspec[] = {1,3,2,20};



/* 
 * This is the routine called to encode a trap pdu. Currently it can encode 
 * two types of traps. These are :
 * 1.  ilmiColdStart.
 * 2.  ilmiVCCChange.
 * The fields vpi,vci are required for the VccChangeTrap.
 */

boolean
ilmi_trap_encode (ilmiTrapType type, int vpi, int vci, int status, uchar **buffer, int *length)
{
	uchar *buf,*trapbuf,*tmpbuf;
	int len,trapindex,pktlenindex,pdulenindex,vblistindex,tmpindex;
	int value[4],ii,time;
	OID *sysoid;

	if((type != ilmiColdStart) && (type != ilmiVCCChange)) {
	    ilmi_log_error(ILMI_ID,ilmiIllegalTrapType, "<ilmi_trap_encode>");
	    return (FALSE);
	}

	/*
	 * Trap Pkt  Header
	 */
	 trapindex = 0;
	 trapbuf =  malloc(2);
	 if(!trapbuf)
	    return (FALSE);
	 *trapbuf = 0x30;                 /* Sequence Tag */
	 pktlenindex = trapindex + 1;     /* We assign length later. */
	 trapindex += 2;

	/*
         * Version
  	 */
	/* Construct the Trap PDU. */
	tmpbuf = trapbuf;
	trapbuf = realloc(trapbuf,trapindex+ZEROINT_LEN);
	if(!trapbuf) {
	    free(tmpbuf);
	    return (FALSE);
	}
	bcopy(zero_int_code,trapbuf+trapindex,ZEROINT_LEN);
	trapindex += ZEROINT_LEN;

	/*
	 * Community
	 */
	tmpbuf = trapbuf;
	trapbuf = realloc(trapbuf,trapindex+COMMSTR_LEN);
	if(!trapbuf) {
	    free(tmpbuf);
	    return (FALSE);
	}
	bcopy(comm_str_code,trapbuf+trapindex,COMMSTR_LEN);
	trapindex += COMMSTR_LEN;

	/*
	 * Trap PDU specific Tag.
	 */
	tmpbuf = trapbuf;
	trapbuf = realloc(trapbuf,trapindex+2);
	if(!trapbuf) {
	    free(tmpbuf);
	    return (FALSE);
	}
	*(trapbuf+trapindex) = 164 ;
	pdulenindex = trapindex+1;			/* We assign length later. */
	trapindex += 2;

	/*
	 * Enterprise
	 */
	 if(type == ilmiColdStart) {
	     sysoid = snmp_platform_get_oid(SNMP_PLATFORM_OID_SYS_OBJECT_ID);
	     if (!ilmi_name_encode(sysoid,&buf,&len)) {
		 free(trapbuf);
		 return (FALSE);
	     }
	     tmpbuf = trapbuf;
	     trapbuf = realloc(trapbuf,trapindex+len);
	     if(!trapbuf) {
		 free(buf);
		 free(tmpbuf);
		 return (FALSE);
	     }
	     bcopy(buf,trapbuf+trapindex,len);
	     free(buf);
	     trapindex += len;
	 }
	 else if (type == ilmiVCCChange) {
	     tmpbuf = trapbuf;
	     trapbuf = realloc(trapbuf,trapindex+ATMFORUM_NAMEBUFLEN);
	     if(!trapbuf) {
		 free(tmpbuf);
		 return (FALSE);
	     }
	     bcopy(atmforum_ent_namebuf,trapbuf+trapindex,ATMFORUM_NAMEBUFLEN);
	     trapindex += ATMFORUM_NAMEBUFLEN;
	 }

	 /*
	  * Agent Address
	  */
	tmpbuf = trapbuf;
	trapbuf = realloc(trapbuf,trapindex+ILMITRAP_AGENT_NAMEBUFLEN);
	if(!trapbuf) {
	    free(tmpbuf);
	    return (FALSE);
	}
	bcopy(ilmitrap_agent_addrbuf,trapbuf+trapindex,ILMITRAP_AGENT_NAMEBUFLEN);
	trapindex += ILMITRAP_AGENT_NAMEBUFLEN;

	/*
	 * Generic Trap Number
	 */
	tmpbuf = trapbuf;
	trapbuf = realloc(trapbuf,trapindex+TRAPNUM_BUFLEN);
	if(!trapbuf) {
	    free(tmpbuf);
	    return (FALSE);
	}
	if(type == ilmiColdStart)
	    bcopy(trap_gen_coldstart,trapbuf+trapindex,TRAPNUM_BUFLEN);
	else if (type == ilmiVCCChange)
	    bcopy(trap_gen_enterprise,trapbuf+trapindex,TRAPNUM_BUFLEN);
	trapindex += TRAPNUM_BUFLEN;

	/*
	 * Specific Trap Number
	 */
	tmpbuf = trapbuf;
	trapbuf = realloc(trapbuf,trapindex+TRAPNUM_BUFLEN);
	if(!trapbuf) {
	    free(tmpbuf);
	    return (FALSE);
	}
	if(type == ilmiColdStart)
	    bcopy(zero_int_code,trapbuf+trapindex,TRAPNUM_BUFLEN);
	else if (type == ilmiVCCChange)  {
	    bcopy(trap_spec_VCCchange,trapbuf+trapindex,TRAPNUM_BUFLEN);
	    value[0] = 0; /* Port is always zero */
	    value[1] = vci;
	    value[2] = vpi;
	    value[3] = status;
	}
	trapindex += TRAPNUM_BUFLEN;

	
	/* 
	 * Time Tick 
	 */
	time = GetTimeNow();
	if(!ilmi_int_encode(time,&buf,&len)) {
	    free(trapbuf);
	    return (FALSE);
	}
	*buf = 0x43;   /* TimeTicks Tag */
	tmpbuf = trapbuf;
	trapbuf = realloc(trapbuf,trapindex+len);
	if(!trapbuf) {
	    free(buf);
	    free(tmpbuf);
	    return (FALSE);
	}
	bcopy(buf,trapbuf+trapindex,len);
	free(buf);
	trapindex += len;

	/*
	 * VarBind List Sequence Tag
	 */

	tmpbuf = trapbuf;
	trapbuf = realloc(trapbuf,trapindex+2);
	if(!trapbuf) {
	    free(tmpbuf);
	    return (FALSE);
	}
	*(trapbuf+trapindex) = 0x30;  /* Sequence Tag */
	vblistindex = trapindex+1;
	trapindex += 2;

	if (type == ilmiVCCChange) {
	    for(ii=0; ii<4; ii++) {
	        tmpbuf = trapbuf;
		trapbuf = realloc(trapbuf,trapindex+2);
	        if(!trapbuf) {
		    free(tmpbuf);
		    return (FALSE);
	        }
		*(trapbuf+trapindex) = 0x30;  /* Sequence Tag */
		tmpindex = trapindex+1;
		trapindex += 2;

		/* Name */
	        tmpbuf = trapbuf;
		trapbuf = realloc(trapbuf,trapindex+VCCCHANGE_NAMEBUFLEN);
	        if(!trapbuf) {
		    free(tmpbuf);
		    return (FALSE);
	        }
		bcopy(vccchange_name_buf,trapbuf+trapindex,VCCCHANGE_NAMEBUFLEN-1);
		*(trapbuf+trapindex+VCCCHANGE_NAMEBUFLEN-1) = vccchange_name_bufspec[ii];
		trapindex += VCCCHANGE_NAMEBUFLEN;

		/* Value */
		if (!ilmi_int_encode(value[ii],&buf,&len)) {
		    free(trapbuf);
		    return (FALSE);
		}
	        tmpbuf = trapbuf;
		trapbuf = realloc(trapbuf,trapindex+len);
	        if(!trapbuf) {
		    free(buf);
		    free(tmpbuf);
		    return (FALSE);
	        }
		bcopy(buf,trapbuf+trapindex,len);
		free(buf);
		trapindex += len;

		*(trapbuf+tmpindex) = trapindex - tmpindex - 1;
	    }
	}

	*(trapbuf+pktlenindex) = trapindex - pktlenindex -  1;
	*(trapbuf+pdulenindex) = trapindex - pdulenindex -  1;
	*(trapbuf+vblistindex) = trapindex - vblistindex -  1;
	*buffer = trapbuf;
	*length = trapindex;

	return (TRUE);
}



boolean
ilmi_req_encode (ilmi_msgblock *msg)
{
    ObjectSyntax value;
    int reqindex,pktlenindex,pdulenindex,vblistindex,vbindex;
    int len;
    uchar *reqbuf,*buf, *tmpbuf;
	OID *tmpname=NULL;


    /*
     * Check that we have enough info.
     */
    if((!msg) ||
       (!msg->vblist) ||
       (!msg->vblist->name)) {
	return (FALSE);
    }

    /*
     * Request Pkt  Header
     */
    reqindex = 0;
    reqbuf =  malloc(2);
    if(!reqbuf)
	return (FALSE);
    *reqbuf = 0x30;                 /* Sequence Tag */
    pktlenindex = reqindex + 1;     /* We assign length later. */
    reqindex += 2;

    /* Construct the Trap PDU. */

    /*
     * Version
     */

    tmpbuf = reqbuf;
    reqbuf = realloc(reqbuf,reqindex+ZEROINT_LEN);
    if(!reqbuf) {
	free(tmpbuf);
	return (FALSE);
    }
    bcopy(zero_int_code,reqbuf+reqindex,ZEROINT_LEN);
    reqindex += ZEROINT_LEN;

    /*
     * Community
     */
    tmpbuf = reqbuf;
    reqbuf = realloc(reqbuf,reqindex+COMMSTR_LEN);
    if(!reqbuf) {
	free(tmpbuf);
	return (FALSE);
    }
    bcopy(comm_str_code,reqbuf+reqindex,COMMSTR_LEN);
    reqindex += COMMSTR_LEN;

    /*
     * Trap PDU specific Tag.
     */
    tmpbuf = reqbuf;
    reqbuf = realloc(reqbuf,reqindex+2);
    if(!reqbuf) {
	free(tmpbuf);
	return (FALSE);
    }
    switch(msg->oper) {
    case ilmiGet   :
	*(reqbuf+reqindex) = 160 ;
	break;
    case ilmiGetNext :
	*(reqbuf+reqindex) = 161 ;
	break;
    case ilmiSet :
	*(reqbuf+reqindex) = 163 ;
	break;
    case ilmiGetResp :
    case ilmiTrap:
    default:
	return (FALSE);
	break;
    }
    pdulenindex = reqindex+1;			/* We assign length later. */
    reqindex += 2;

    /*
     * Request ID
     */

    if(!ilmi_int_encode(msg->req_id,&buf,&len)) {
	free(reqbuf);
	return (FALSE);
    }
    tmpbuf = reqbuf;
    reqbuf = realloc(reqbuf,reqindex+len);
    if(!reqbuf) {
	free(buf);
	free(tmpbuf);
	return (FALSE);
    }
    bcopy(buf,reqbuf+reqindex,len);
    free(buf);
    reqindex += len;

    /*
     * Error Status
     */
    tmpbuf = reqbuf;
    reqbuf = realloc(reqbuf,reqindex+ZEROINT_LEN);
    if(!reqbuf) {
	free(tmpbuf);
	return (FALSE);
    }
    bcopy(zero_int_code,reqbuf+reqindex,ZEROINT_LEN);
    reqindex += ZEROINT_LEN;

    /*
     * Error Index
     */
    tmpbuf = reqbuf;
    reqbuf = realloc(reqbuf,reqindex+ZEROINT_LEN);
    if(!reqbuf) {
	free(tmpbuf);
	return (FALSE);
    }
    bcopy(zero_int_code,reqbuf+reqindex,ZEROINT_LEN);
    reqindex += ZEROINT_LEN;

    /*
     * VB List Tag
     */
    tmpbuf = reqbuf;
    reqbuf = realloc(reqbuf,reqindex+2);
    if(!reqbuf) {
	free(tmpbuf);
	return (FALSE);
    }
    *(reqbuf+reqindex) = 0x30;
    vblistindex = reqindex+1;			/* We assign length later. */
    reqindex += 2;

    /*
     * VB Tag
     */
    tmpbuf = reqbuf;
    reqbuf = realloc(reqbuf,reqindex+2);
    if(!reqbuf) {
	free(tmpbuf);
	return (FALSE);
    }
    *(reqbuf+reqindex) = 0x30;
    vbindex = reqindex+1;			/* We assign length later. */
    reqindex += 2;

    /*
     * Name
     */


    if (msg->address_reg_flag == TRUE) {
	
	/* Zero out the last byte for o/g address reg/dereg. */
	
	tmpname = CloneOID(msg->vblist->name);
	if(!tmpname) {
	    free(reqbuf);
	    return (FALSE);
	}

	*(tmpname->oid_ptr + tmpname->length - 1) = 0;

	if (!ilmi_name_encode(tmpname, &buf, &len)) {
	    free(reqbuf);
	    FreeOID(tmpname);
	    return (FALSE);
	}

	FreeOID(tmpname);
	
    } else {
	
	if (!ilmi_name_encode(msg->vblist->name, &buf, &len)) {
	    free(reqbuf);
	    return (FALSE);
	}
    }

    
    
    tmpbuf = reqbuf;
    reqbuf = realloc(reqbuf,reqindex+len);
    if(!reqbuf) {
	free(buf);
	free(tmpbuf);
	return (FALSE);
    }
    bcopy(buf,reqbuf+reqindex,len);
    free(buf);
    reqindex += len;
    

    /*
     * Value
     * Currently we only support integer values.
     */
    value = msg->vblist->value;
    switch (value.type) {
    case COUNTER_TYPE:
    case GAUGE_TYPE:
    case TIME_TICKS_TYPE:
    case INTEGER_TYPE:
	if (!ilmi_int_encode(value.sl_value,&buf,&len)) {
	    free(reqbuf);
	    return (FALSE);
	}
	tmpbuf = reqbuf;
	reqbuf = realloc(reqbuf,reqindex+len);
	if(!reqbuf) {
	    free(buf);
	    free(tmpbuf);
	    return (FALSE);
	}
	bcopy(buf,reqbuf+reqindex,len);
	free(buf);
	reqindex += len;
	break;
    case NULL_TYPE:
    default:
	tmpbuf = reqbuf;
	reqbuf = realloc(reqbuf,reqindex+NULL_VALUEBUF_LEN);
	if(!reqbuf) {
	    free(tmpbuf);
	    return (FALSE);
	}
	bcopy(null_value_buf,reqbuf+reqindex,NULL_VALUEBUF_LEN);
	reqindex += NULL_VALUEBUF_LEN;
	break;
    }

    *(reqbuf+vbindex) = reqindex - vbindex - 1;
    *(reqbuf+vblistindex) = reqindex - vblistindex - 1;
    *(reqbuf+pdulenindex) = reqindex - pdulenindex - 1;
    *(reqbuf+pktlenindex) = reqindex - pktlenindex - 1;
    msg->encodedReq = reqbuf;
    msg->encodedReqLen = reqindex;

    return (TRUE);
}




/*
 * I N T E G E R   E N C O D I N G 
 */


boolean
ilmi_int_encode (int val, unsigned char **buffp, int *len)
{
    unsigned tmp,numbytes;
    unsigned char *bp, *tmpbuf;
    int index=0;

    tmp = val;
    if(val < 0)
	tmp = -val;

    index=0;
    bp = malloc(2);  /* Tag */
    if(!bp)
	return (FALSE);
    *(bp+index) = 0x2;
    index += 2;

    if (tmp < (unsigned) FIRSTBYTE_HIBIT) {
	numbytes = (unsigned char) 1;
	*(bp+1) = numbytes ;
    }
    else if (tmp < (unsigned) SECONDBYTE_HIBIT) {
	numbytes = (unsigned char) 2;
	*(bp+1) = numbytes ;
    }
    else if (tmp < (unsigned) THIRDBYTE_HIBIT) {
	numbytes = (unsigned char) 3;
	*(bp+1) = numbytes ;
    }
    else if (tmp < (unsigned) FOURTHBYTE_HIBIT) {
	numbytes = (unsigned char) 4;
	*(bp+1) = numbytes ;
    } else {
	*(bp + 1) = 5 ;
	tmpbuf = bp;
	bp = realloc(bp,index+1);
	if(!bp) {
	    free(tmpbuf);
	    return (FALSE);
	}
	*(bp+index+1) = (unsigned char) 0;
	index++;
	numbytes = 4;
    }

    tmpbuf = bp;
    bp = realloc(bp,index+numbytes);
    if(!bp) {
	free(tmpbuf);
	return (FALSE);
    }
    bcopy(((char *) &val) + (sizeof(int) - numbytes),bp+index,numbytes);
    index += numbytes;
    *len = index;
    *buffp = bp;

    return (TRUE);
}


/*
 * L E N G T H  E N C O D I N G
 */
boolean
ilmi_length_encode (num,buf,len)
int num;
uchar **buf;
int  *len;
{

    /*
     *  Length is less than 128.
     */

    if(num < FIRSTBYTE_HIBIT) {
	*buf = malloc(1) ;
	if(!(*buf))
	    return (FALSE);
	**buf = num;
	*len = 1;
	return (TRUE);
    }

    if (num <= 255) {
	*buf = malloc(2) ;
	if(!(*buf))
	    return (FALSE);
	**buf = FIRSTBYTE_HIBIT | 1;
	*(*buf+1) = num & 0xff;
	*len = 2;
	return (TRUE);
    }

    /* else assumming num < 16 bits long */
    *buf = malloc(3) ;
    if(!(*buf))
	return (FALSE);
    **buf = FIRSTBYTE_HIBIT | 2;
    *(*buf+1) = num >> 8;
    *(*buf+2) = num & 0xff;
    *len = 3;

    return (TRUE);
}


boolean
ilmi_name_encode (oid,buffer,len)
OID *oid;
unsigned char **buffer;
int *len;
{
    unsigned char *nmbuf, *tmpbuf;
    int ii,index;
    unsigned long *optr;
    

    index=0;
    nmbuf = malloc(3);
    if(!nmbuf)
	return (FALSE);

    *nmbuf = 0x6;   /* OID Tag */
    /* Assign length later */
    *(nmbuf+2) = 0x2b;
    index += 3;


    /*
     * Exclude the first two numbers in the OID
     */

    for(ii=2,optr = oid->oid_ptr+2; ii<oid->length; ii++,optr++) {
	if(*optr < FIRSTBYTE_HIBIT) {
	    tmpbuf = nmbuf;
	    nmbuf = realloc(nmbuf,1+index);
	    if(!nmbuf) {
		free(tmpbuf);
		return (FALSE);
	    }
	    *(nmbuf+index) = *optr;
	    index += 1;
	} else {
	    if(*optr >= (unsigned short)(SECONDBYTE_HIBIT >> 1)) {
		tmpbuf = nmbuf;
		nmbuf = realloc(nmbuf,1+index);
		if(!nmbuf) {
		    free(tmpbuf);
		    return (FALSE);
		}
		*(nmbuf+index) = (unsigned char) (*optr >> 14) | FIRSTBYTE_HIBIT;
		index += 1;
	    }
	    tmpbuf = nmbuf;
	    nmbuf = realloc(nmbuf,2+index);
	    if(!nmbuf) {
		free(tmpbuf);
		return (FALSE);
	    }
	    *(nmbuf+index) = (unsigned char) (*optr >> 7) | FIRSTBYTE_HIBIT;
	    *(nmbuf+index+1) = (unsigned char) (*optr & ~FIRSTBYTE_HIBIT);
	    index += 2;
	}
    }

    *(nmbuf+1) = index-2; /* Fill the length field */
    *buffer = nmbuf;
    *len = index; /* Include the tag and the length field */
    return (TRUE);
}

      
