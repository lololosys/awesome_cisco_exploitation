/*-----------------------------------------------------------------------
    latqr		define lat queued request structure
    
    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO
    
    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved. 
 -----------------------------------------------------------------------*/
#ifndef LATQR_H
#define LATQR_H		0		/* Indicate file has been read	*/

/*----------------------------------------------------------------------
    Define the queued request structure.
 -----------------------------------------------------------------------*/
    typedef struct qr
    {
	struct qr QR_VIEW(qr_link[2]);	/* Link to next, last request	*/
	struct hi HI_VIEW(qr_hi);	/* Link to queued request	*/
    } qr_node, QR_VIEW(qr_ptr);
#endif
