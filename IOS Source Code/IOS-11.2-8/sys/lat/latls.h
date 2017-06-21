/*-----------------------------------------------------------------------
    latls		define lat learned service structure
    
    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO
    
    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved. 
 -----------------------------------------------------------------------*/
#ifndef LATLS_H
#define LATLS_H		0		/* Indicate file has been read	*/

/*----------------------------------------------------------------------
    Define the learned service flags.
 -----------------------------------------------------------------------*/
#define LS_PERM		0x01		/* Service is permanent		*/
#define LS_AGED		0x02		/* Service has aged		*/

/*----------------------------------------------------------------------
    Define the learned services structure.
 -----------------------------------------------------------------------*/
    typedef struct ls
    {
	struct ls LS_VIEW(ls_alpha[2]);	/* Link to next service (alpha)	*/
	struct ls LS_VIEW(ls_host[2]);	/* Link to next service (host)	*/
	struct nd ND_VIEW(ls_nd);	/* Pointer to node structure	*/
	byte	ls_flags;		/* Service flags		*/
	byte	ls_rating;		/* Service rating		*/
	byte	ls_service[SV_STR];	/* Service name  string		*/
	byte	ls_ident[ID_STR];	/* Service ident string		*/
    } ls_node, LS_VIEW(ls_ptr);
#endif
