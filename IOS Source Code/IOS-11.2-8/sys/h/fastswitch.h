/* $Id: fastswitch.h,v 3.1.8.1 1996/07/08 22:17:00 mwu Exp $
 * $Source: /release/112/cvs/Xsys/h/fastswitch.h,v $
 *------------------------------------------------------------------
 * miscellaneous support for platform independent fastswitching
 *
 * December 1995, Bob O'Grady
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fastswitch.h,v $
 * Revision 3.1.8.1  1996/07/08  22:17:00  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/12  11:10:19  ogrady
 * Placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern paktype *platform_fastsend_getbuffer(int size);
extern void platform_fs_pak_data_copy(paktype *srcpak, paktype *dstpak,
				      uchar *srcptr, uchar *dstptr, int size);


/*
 * set up pak fields required by hwidb->fastsend()
 * this is here to somewhat define the interface to fastsend.
 * being that this is for fastsend it should be called
 *  from interrupt level
 */
static inline void
setup_fastsend_pak (
    paktype *pak,
    idbtype *if_input,
    idbtype *if_output,
    uchar *dgstart,
    int dgsize,
    ushort linktype,
    short acct_proto)
{
    /*
     * if_input needed for some output queueing stuff?
     * don't use set_if_input() - don't want input acct
     */
    pak->if_input = if_input;
    pak->if_output = if_output;
    pak->datagramstart = dgstart;
    pak->datagramsize = dgsize;
    pak->acct_proto = acct_proto;
    pak->linktype = linktype;

    /*
     * hw_output is passed to fastsend as an arg
     */
}
