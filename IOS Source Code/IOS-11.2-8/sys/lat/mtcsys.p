/*-----------------------------------------------------------------------
    mtcsys              (h) Meridian system and configuration definitions
    (MTCSYS.PH)

    Copyright 1991      Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/
#ifndef MTCSYS_H
#define MTCSYS_H        0               /* Indicate file has been read  */

/*-----------------------------------------------------------------------
 ************************************************************************
    Non-configurable section (convenient common definitions).
 ************************************************************************
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    Define convenient logical values and standard function return values.
 -----------------------------------------------------------------------*/
#define TRUE            1               /* True logical value           */
#define FALSE           0               /* False logical value          */

#define SUCCESS         TRUE            /* Standard success return      */
#define FAILURE         FALSE           /* Standard failure return      */

/*-----------------------------------------------------------------------
    Define standard character constants.
 -----------------------------------------------------------------------*/
#define EOL             '\n'            /* End of line character        */
#define EOS             '\0'            /* End of string character      */

/*-----------------------------------------------------------------------
    Define standard macros.
 -----------------------------------------------------------------------*/
#define DIM(x)    (sizeof(x)/sizeof(*(x))) /* Dimension of an array     */
#define HIBYTE(x) (((x) >> 8) & 0xff)      /* High byte of 16-bit word  */
#define LOBYTE(x) ((x) & 0xff)             /* Low byte of 16-bit word   */
#define HIWORD(x) (((x) >> 16) & 0xffffL)  /* Upper half of 32-bit word */
#define LOWORD(x) ((x) & 0xffffL)          /* Lower half of 32-bit word */

/*-----------------------------------------------------------------------
 ************************************************************************
    Configurable section.
 ************************************************************************
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    Define fundamental data types for use by all Meridian products.
 -----------------------------------------------------------------------*/
    typedef unsigned char  uint8;       /* Unsigned  8-bit quantity     */
    typedef unsigned short uint16;      /* Unsigned 16-bit quantity     */
    typedef unsigned long  uint32;      /* Unsigned 32-bit quantity     */
    typedef long           sint32;      /* Signed   32-bit quantity     */

    typedef uint8          byte;        /* Unsigned  8-bit quantity     */
    typedef uint16         word;        /* Unsigned 16-bit quantity     */
    typedef uint32         count;       /* Unsigned 32-bit quantity     */

/*-----------------------------------------------------------------------
    The following definition should be set to 1 for processors such as
    M680x0 and SPARC, whose host order is the same as Internet and
    Ethernet network order.  It should be set to 0 for processors such
    as VAX and Intel, whose host order is swapped from standard network
    order.  (Note that LAT messages use an opposite byte ordering.)
 -----------------------------------------------------------------------*/
#define NET_ORDER               0       /* Processor byte ordering      */

/*-----------------------------------------------------------------------
    The following definition should be set to 1 for processors such as
    RISC or M68000 which must have 32-bit quantities aligned on 32-bit
    boundaries for memory storage or retrieval, or for compilers which
    will pad structure definitions to long word boundaries.  For processors
    such as Intel, VAX, or later M680x0, which will manipulate 32-bit
    quantities on any boundary, this definition can be set to 0.
 -----------------------------------------------------------------------*/
#define LONG_ALIGN              0       /* Memory addressing alignment  */

/*-----------------------------------------------------------------------
    Define function definition macro.  The define below should be set to
    1 if your compiler allows specifying the types of the parameters in
    a function prototype.  It should be set to 0 if your compiler uses
    the original K & R function prototypes which only specify the function
    return type.
 -----------------------------------------------------------------------*/
#define PROTOTYPE_PARAMETERS    1       /* Use paramters in prototypes  */

#if PROTOTYPE_PARAMETERS
#define fncdef(a,b,c,d) c a d;          /* External function prototyping*/
#else
#define fncdef(a,b,c,d) c a();          /* External function prototyping*/
#endif

/*-----------------------------------------------------------------------
    Define the Ethernet address type and constants.
 -----------------------------------------------------------------------*/
#define ETHER_ADDR_LEN          6       /* Ethernet address len (bytes) */
#define ETHER_BUFF_MAXLEN       1500    /* Max Ethernet data len (bytes)*/

    typedef uint16 ether_addr[3];       /* Ethernet address type        */

/*-----------------------------------------------------------------------
    Define macros for copying and comparing Ethernet addresses.  The
    macros must be defined compatibly with the definition of ether_addr
    above.  If your implementation needs to deal with Ethernet addresses
    in more than one format (for example as an array of bytes and an array
    of words), you can define ether_cast to cast Ethernet addresses into
    the type of ether_addr.
 -----------------------------------------------------------------------*/
#define ether_cast
/* #define ether_cast   (uint16 *) */

#define ether_mov(dst,src)  ((ether_cast(dst))[0] = (ether_cast(src))[0], \
                             (ether_cast(dst))[1] = (ether_cast(src))[1], \
                             (ether_cast(dst))[2] = (ether_cast(src))[2])

#define ether_equ(dst,src)  ((ether_cast(dst))[0] == (ether_cast(src))[0] && \
                             (ether_cast(dst))[1] == (ether_cast(src))[1] && \
                             (ether_cast(dst))[2] == (ether_cast(src))[2])

/*----------------------------------------------------------------------
    Bit masks for of_size field in of_node structure used in transmitting
    network messages.
 -----------------------------------------------------------------------*/
#define OF_LENGTH               0x0fff  /* Length field in of_size      */
#define OF_REXMIT               0x1000  /* Retransmit frame             */
#define OF_OUTPUT               0x2000  /* Frame is being output        */
#define OF_DELETE               0x4000  /* Delete frame on I/O complete */

/*----------------------------------------------------------------------
    Common Ethernet input/output frame definitions.  To use these common
    definitions set the COMMON_ETHER_DEFS variable to 1.  To use protocol-
    specific versions of the input and output frame structures, set
    COMMON_ETHER_DEFS to 0.
 -----------------------------------------------------------------------*/
#define COMMON_ETHER_DEFS       0       /* Use common or individual defs*/

#if COMMON_ETHER_DEFS

#define IF_VIEW(x)              *(x)    /* Pointer to input frame data  */
#define OF_VIEW(x)              *(x)    /* Pointer to output frame data */
#define OF_NODE_VIEW(x)         *(x)    /* Pointer to of_node structure */

    typedef uint8 IF_VIEW(if_byte);     /* Pointer to input  frames     */
    typedef uint8 OF_VIEW(of_byte);     /* Pointer to output frames     */

    typedef struct of
    {
        struct of OF_NODE_VIEW(of_link);/* Engine's transmit queue link */
        struct of OF_NODE_VIEW(of_i_o); /* Environment's output queue link*/
        of_byte         of_data;        /* Pointer to output data       */
        uint16          of_size;        /* Size of output frame + flags */
        int             of_timer;       /* Timer for retransmissions    */
        int             of_count;       /* Counter for various purposes */
        ether_addr      of_dst;         /* Destination Ethernet address */
        ether_addr      of_src;         /* Source Ethernet address      */
        uint16          of_protocol;    /* Ethernet type for protocol   */
    } of_node, OF_NODE_VIEW(of_ptr);

#endif /* COMMON_ETHER_DEFS */

/*-----------------------------------------------------------------------
 ************************************************************************
    Configuration for protocol engines to be used in platform.
 ************************************************************************
 -----------------------------------------------------------------------*/
#define BUILD_HPT               0       /* Set to 1 to build HPT        */
#define BUILD_SLAT              1       /* Set to 1 to build SuperLAT   */
#define CME_SLAT                0       /* Set to 1 for CME w/ SuperLAT */

/*-----------------------------------------------------------------------
    SuperLAT Definitions.
 -----------------------------------------------------------------------*/
#if BUILD_SLAT
#include "latsys.h"
#endif /* BUILD_SLAT */

/*-----------------------------------------------------------------------
    High Performance Telnet (HPT) Definitions.
 -----------------------------------------------------------------------*/
#if BUILD_HPT
#include "hptsys.h"
#endif /* BUILD_HPT */

/*-----------------------------------------------------------------------
    Connection Management Environment (CME) Definitions.
 -----------------------------------------------------------------------*/
#if CME_SLAT || BUILD_HPT
#include "cmesys.h"
#endif

#endif /* MTCSYS_H */
