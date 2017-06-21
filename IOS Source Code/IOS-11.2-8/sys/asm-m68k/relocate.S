| $Id: relocate.S,v 3.2 1995/11/17 08:40:11 hampton Exp $
| $Source: /swtal/cherf.111/ios/sys/asm-m68k/relocate.S,v $
|------------------------------------------------------------------
| relocate.S - Position-independent relocating loader
|
| May 20, 1992, Kevin Paul Herbert
|
| Copyright (c) 1992-1994 by cisco Systems, Inc.
| All rights reserved.
| 
| This module relocates the image that it is linked with in order
| to run at whatever PC location it has been copied to. It uses
| relocation fixup information which has been placed at the end
| of the initialized data segment by the "reloc" program.
|
| This module must be linked first in the load module, and control
| must be transferred to the beginning of this routine. Upon completion
| of relocation, we call the routine __start (or _start from C), passing
| all registers unchanged from when we were called initially.
|
| This module uses the stack, and assumes that one has been set up by
| the caller.
|------------------------------------------------------------------
| $Log: relocate.S,v $
| Revision 3.2  1995/11/17  08:40:11  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:55  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:06:14  hampton
| Bump version numbers from 1.x to 2.x.
|
| $Endlog$
|------------------------------------------------------------------
|
| The stack is used to keep all values required
| for this program to work on. The organization
| of these values is as follows :
|
| 0.  Unused                      
| 1.  text checksum offset in data
| 2.  text checksum
| 3.  image checksum
| 4.  size of relocation tables
| 5.  size of relocate program
| 6.  size of data
| 7.  size of text                   
| 8.  bss relocation amount
| 9.  data relocation amount
| 10. text relocation amount
| 11. relocation table address
| 12. final data address
| 13. final text address
| 14. linked data address
| 15. linked text address
| 16. current data address
| 17. current text address           a0 always points to this location
|------------------------------------------------------------------
| Values are set in this stack in the following order :
|
| 1.  linked text address
| 2.  linked data address
| 3.  size of text
| 4.  size of data
| 5.  size of relocate program
| 6.  size of relocation tables
| 7.  image checksum
| 8.  text checksum
| 9.  text checksum offset in data
| 10. current text address
| 11. current data address
| 12. final text address
| 13. final data address
| 14. relocation table address
| 15. text relocation amount
| 16. data relocation amount
| 17. bss relocation amount
|------------------------------------------------------------------
| Register a0 always points to current text address (context table base)
| Register a4 always points to start of this program
|------------------------------------------------------------------

#define CTS			18	/* Context Table Size */
|
| Offsets of each parameter in the context table, starting
| from the base of the table.
|
#define C_TEXT_ADDR		0
#define C_DATA_ADDR		4
#define L_TEXT_ADDR		8
#define L_DATA_ADDR		12
#define F_TEXT_ADDR		16
#define F_DATA_ADDR		20
#define R_TABLE_ADDR		24
#define TEXT_RELOC_AMT		28
#define DATA_RELOC_AMT		32
#define BSS_RELOC_AMT		36
#define TEXT_SIZE		40
#define DATA_SIZE		44
#define PIC_SIZE		48
#define RELOC_SIZE		52
#define IMAGE_CKSUM		56
#define TEXT_CKSUM		60
#define TEXT_CKSUM_OFFSET	64
|
| Offsets of values in the special header
|
#define R_MAGIC_OFFSET		(16)
#define R_VERSION_OFFSET	(R_MAGIC_OFFSET + 4)
#define R_HDR_LEN_OFFSET	(R_VERSION_OFFSET + 4)
#define R_TEXT_OFFSET		(R_HDR_LEN_OFFSET + 4)
#define R_DATA_OFFSET		(R_TEXT_OFFSET + 4)
#define R_TSIZE_OFFSET		(R_DATA_OFFSET + 4)
#define R_DSIZE_OFFSET		(R_TSIZE_OFFSET + 4)
#define R_BSIZE_OFFSET		(R_DSIZE_OFFSET + 4)
#define R_PGMSIZE_OFFSET	(R_BSIZE_OFFSET + 4)
#define R_RSIZE_OFFSET		(R_PGMSIZE_OFFSET + 4)
#define R_DEF_TEXT_OFFSET	(R_RSIZE_OFFSET + 4)
#define R_DEF_DATA_OFFSET	(R_DEF_TEXT_OFFSET + 4)
#define R_DEF_RAM_START_OFFSET	(R_DEF_DATA_OFFSET + 4)
#define R_TEXT_OFF_OFFSET	(R_DEF_RAM_START_OFFSET + 4)
#define R_DATA_OFF_OFFSET	(R_TEXT_OFF_OFFSET + 4)
#define R_DEF_TC_OFFSET		(R_DATA_OFF_OFFSET + 4)
#define R_RFR_TC_OFFSET		(R_DEF_TC_OFFSET + 4)
#define R_TC_OFF_OFFSET		(R_RFR_TC_OFFSET + 4)
#define R_TC_OFF_DATA_OFFSET	(R_TC_OFF_OFFSET + 4)
#define R_IC_OFFSET		(R_TC_OFF_DATA_OFFSET + 4)
#define R_DEF_IC_OFFSET		(R_IC_OFFSET + 4)
#define R_HC_OFFSET		(R_DEF_IC_OFFSET + 4)
#define R_TC_ALGOR_OFFSET	(R_HC_OFFSET + 4)
#define R_IC_ALGOR_OFFSET	(R_TC_ALGOR_OFFSET + 4)
#define R_HC_ALGOR_OFFSET	(R_IC_ALGOR_OFFSET + 4)
#define R_PLATFORM_FAM_OFFSET	(R_HC_ALGOR_OFFSET + 4)

|
|
	.text

	.globl	__relocate

| WE START HERE !! -------------------------------------------------

__relocate:
	moveml #0xfffc, sp@-	| Save all registers

| Create space on stack for working values.
| Set a0 as shown.
|
|           STACK
|----------------------------------+
| 00. Unused                       |
|----------------------------------|
| 01. empty                        |
|----------------------------------|
| 02. empty                        |
|----------------------------------|
| 03. empty                        |
|----------------------------------|
| 04. empty                        |
|----------------------------------|
| 05. empty                        |
|----------------------------------|
| 06. empty                        |
|----------------------------------|
| 07. empty                        |  
|----------------------------------|
| 08. empty                        |
|----------------------------------|
| 09. empty                        |
|----------------------------------|
| 10. empty                        |
|----------------------------------|
| 11. empty                        |
|----------------------------------|
| 12. empty                        |
|----------------------------------|
| 13. empty                        |
|----------------------------------|
| 14. empty                        |
|----------------------------------|
| 15. empty                        |
|----------------------------------|
| 16. empty                        |
|----------------------------------|
| 17. empty                        |  <-- a0/a2 
|----------------------------------+
|
	subl #(CTS*4),sp	| Allocate space for context tables
	movel sp, a0		| a0 -> context table start
	movel a0, a2		| copy a0 to a2

| Get PC address of __relocate (this very program).
| Using that, point to the start of the special header.
| Then transfer values from spl_hdr to stack.

| PC address of this program -> a4 -> a5
| Decrement a5 to point to the length of the special header.
| The length will be stored in the last long of the special
| header, just before the start of this program.
| Using the length, set a5 to point to start of the special header.

	lea __relocate, a4  	| point to relocate itself
        movel a4, a5            | save a4 in a5
        subl #(1*4), a5         | Set a5 to point to length of spl_hdr
	subl a5@, a5		| Subtract header length
	addl #(1*4), a5		| Then bump up a5 so it points to start
				| of the special header

| Validate the special header checksum before picking out values
| from it.
|
|       | Exec header           |
|       +-----------------------+  <-- a5, a1
|       |                       |
|       | Special Header        |
|       |                       |
|       +-----------------------+  <-- a4
|       | PIC loader            |
|

	movel a5, a1		| pointers to start of special header
	cmpl #0x8002b0ee, a1@(R_MAGIC_OFFSET)
	bne  SHC_err_msg	| Spl header magic mismatch

	movel #-1, d2		| clear checksum counter

SHC1:
	movel d2, d3		| Save current checksum
	addl a1@+, d2
	cmpl d2, d3		| Have we wrapped around ?
	jls  SHC2		| jump if no wraparound
	addql #1, d2

SHC2:
	cmpl a1, a4		| Are we done ?
	jhi SHC1		| Nope. Loop back.
	
  	cmpl #-1, d2		| Checksum match (== all FFs) ?

   	bne  SHC_err_msg	| Display error and terminate

| Special Header looks OK. So....
| Transfer values from special header to stack.
| a5 points to start of special header.
| a2 points to start of context table in stack.

	movel a5@(R_TEXT_OFFSET), a2@(L_TEXT_ADDR)
	movel a5@(R_DATA_OFFSET), a2@(L_DATA_ADDR)
	movel a5@(R_TSIZE_OFFSET), a2@(TEXT_SIZE)
	movel a5@(R_DSIZE_OFFSET), a2@(DATA_SIZE)
	movel a5@(R_PGMSIZE_OFFSET), a2@(PIC_SIZE)
	movel a5@(R_RSIZE_OFFSET), a2@(RELOC_SIZE)
	movel a5@(R_IC_OFFSET), a2@(IMAGE_CKSUM)
	movel a5@(R_RFR_TC_OFFSET), a2@(TEXT_CKSUM)
	movel a5@(R_TC_OFF_DATA_OFFSET), a2@(TEXT_CKSUM_OFFSET)

|        
|           STACK
|----------------------------------+
| 00. Unused                       |
|----------------------------------+
| 01. text checksum offset in data |  <-- a2      (9)
|----------------------------------+
| 02. text checksum in DRAM        |  <-- a2      (8)
|----------------------------------+
| 03. image checksum               |  <-- a2      (7)
|----------------------------------|
| 04. relocation table size        |  <-- a2      (6)
|----------------------------------|
| 05. this program size            |  <-- a2      (5)
|----------------------------------|
| 06. data size                    |  <-- a2      (4)
|----------------------------------|
| 07. text size                    |  <-- a2      (3)
|----------------------------------|
| 08.                              |
|----------------------------------|
| 09.                              |
|----------------------------------|
| 10.                              |
|----------------------------------|
| 11.                              |
|----------------------------------|
| 12.                              |
|----------------------------------|
| 13.                              |
|----------------------------------|
| 14. linked data address          |  <-- a2      (2)
|----------------------------------|       
| 15. linked text address          |  <-- a2      (1)
|----------------------------------|
| 16.                              |
|----------------------------------|
| 17.                              |  <-- a0 
|----------------------------------+

|
| Get the current start-of-text address. Use a5 to hold the address.
| Start with a5 pointing to the start of this program.
| Then increment it first by the size of this program, 
| then by the size of the relocation tables.

	movel a4, a5			| set a5 to start of this program 
	addl a2@(PIC_SIZE), a5		| add size of this program
        addl a2@(RELOC_SIZE), a5  	| add reloc table size 

| Store the current start addresses of the text and data sections

        movel a5, a2@(C_TEXT_ADDR)   	| store current text address
	addl a2@(TEXT_SIZE), a5      	| add text size to current text address
        movel a5, a2@(C_DATA_ADDR)   	| store current data address

|           STACK
|----------------------------------+
| 00. Unused                       |
|----------------------------------+
| 01. text checksum offset in data | 
|----------------------------------+
| 02. text checksum in DRAM        | 
|----------------------------------+
| 03. image checksum               | 
|----------------------------------|
| 04. relocation table size        |  
|----------------------------------|
| 05. this program size            |  
|----------------------------------|
| 06. data size                    |
|----------------------------------|
| 07. text size                    |  
|----------------------------------|
| 08.                              |
|----------------------------------|
| 09.                              |
|----------------------------------|
| 10.                              |
|----------------------------------|
| 11.                              |
|----------------------------------|
| 12.                              |
|----------------------------------|
| 13.                              |
|----------------------------------|
| 14. linked data address          | 
|----------------------------------|       
| 15. linked text address          | 
|----------------------------------+
| 16. current data address         |  <-- a2     (2)
|----------------------------------|
| 17. current text address         |  <-- a0/a2  (1)
|----------------------------------+
|
| Is the image checksum OK ?
| Start from the beginning of this program upto the
| end of the data section (= current data address + data size).
| a3 -> moves from start of __relocate to end of image data
| d1 -> count of long words to checksum
| d2 -> checksum
| d3 -> scratchpad register for detecting checksum wraparound

	movel a2@(C_DATA_ADDR), d1
	addl a2@(DATA_SIZE), d1	| make d1 point to end of data
	movel a4, a5		| set a5 to start of this program
				| Our checksumming starts from here
	movel #-1, d2		| clear checksum counter

IC1:
	movel d2, d3		| Save current checksum
	addl a5@+, d2
	cmpl d2, d3		| Have we wrapped around ?
	jls  IC2		| jump if no wraparound
	addql #1, d2

IC2:
	cmpl a5, d1		| Are we done ?
	jhi IC1			| Nope. Loop back.

| Ok. Checksum calculated. Now validate it.
| ---------- Validate image checksum ---------------
|
	movel a2@(IMAGE_CKSUM), d3
  	cmpl d3, d2		| Checksum match ?

   	bne  IC_err_msg		| Display error and terminate
	
| Are we running where we are supposed to ??

IC_OK:
        movel a2@(C_TEXT_ADDR), d2 | d2 = current text address
        movel a2@(L_TEXT_ADDR), d1 | d1 = text link address
        cmpl d1, d2             | compare curr text addr with text link addr
        beq L2                  | jump if they are equal, ie., we are
				| running where we should be...

|
|           STACK
|----------------------------------+
| 00. Unused                       |
|----------------------------------+
| 01. text checksum offset in data |  
|----------------------------------+
| 02. text checksum in DRAM        |  
|----------------------------------+
| 03. image checksum               |  
|----------------------------------|
| 04. relocation table size        |  
|----------------------------------|
| 05. this program size            |  
|----------------------------------|
| 06. data size                    | 
|----------------------------------|
| 07. text size                    |  
|----------------------------------|
| 08.                              |
|----------------------------------|
| 09.                              |
|----------------------------------|
| 10.                              |
|----------------------------------|
| 11.                              |
|----------------------------------|
| 12.                              |
|----------------------------------|
| 13.                              |
|----------------------------------|
| 14. linked data address          | 
|----------------------------------|       
| 15. linked text address          |  
|----------------------------------+
| 16. current data address         | 
|----------------------------------|
| 17. current text address         |  <-- a0/a2
|----------------------------------+

| NOPE. Current text address != text link address.
| This means we have to do fixups

| Update the text segment checksum for a RFR image.
| We do this only if we are relocating, which means we are
| running from DRAM.

	movel a2@(C_DATA_ADDR), d1      | get data section start in d1
 	addl a2@(TEXT_CKSUM_OFFSET), d1	| d1 now points to text checksum
	movel d1, a3			| set a3 to point to text checksum
 	movel a2@(TEXT_CKSUM), a3@	| update text checksum

|
| Figure out the final start addresses for the text and data
| sections. As a bonus, get the relocation table address.
| When running out of DRAM, the text section is relocated
| such that it immediately follows this program, overwriting the
| relocation tables. The data section follows the text section.
| The text and data sections are moved only after they have been
| relocated and the relocation tables are no longer of any use.

        movel a4, a5            | a5 points to start of this program
	addl a2@(PIC_SIZE), a5	| add size of this program
	movel a5, a2@(F_TEXT_ADDR)
	movel a5, a2@(R_TABLE_ADDR)

	addl a2@(TEXT_SIZE), a5 | make a5 point to start of data
	movel a5, a2@(F_DATA_ADDR)

|           STACK
|----------------------------------+
| 00. Unused                       |
|----------------------------------+
| 01. text checksum offset in data |  
|----------------------------------+
| 02. text checksum in DRAM        |  
|----------------------------------+
| 03. image checksum               |  
|----------------------------------|
| 04. relocation table size        |  
|----------------------------------|
| 05. this program size            |  
|----------------------------------|
| 06. data size                    | 
|----------------------------------|
| 07. text size                    |  
|----------------------------------|
| 08.                              |
|----------------------------------|
| 09.                              |
|----------------------------------|
| 10.                              |
|----------------------------------|
| 11. relocation table address     |  <-- a3   (3)
|----------------------------------|
| 12. final data address           |  <-- a3   (2)
|----------------------------------|
| 13. final text address           |  <-- a3   (1)
|----------------------------------|
| 14. linked data address          | 
|----------------------------------|       
| 15. linked text address          |  
|----------------------------------+
| 16. current data address         | 
|----------------------------------|
| 17. current text address         |  <-- a0
|----------------------------------+

|
| Since we are moving the text section to immediately follow
| this program, we have to dork with the branch instruction
| at the end of this program so that we drop down to the text 
| section.

	movel a2@(R_TABLE_ADDR), d1
        subl #(4), d1           | d1 = address of bra disp in this program
        movel d1, a5
        movel #(4), a5@         | force bra disp = 4

        bra L4

| We come here if the current text address is the
| same as the link address (which means no fixups).
| Set the final addresses for text and data

L2:
	movel a2, a5
        movel a2@(L_TEXT_ADDR), a5@(F_TEXT_ADDR)
        movel a2@(L_DATA_ADDR), a5@(F_DATA_ADDR)

|           STACK
|----------------------------------+
| 00. Unused                       |
|----------------------------------+
| 01. text checksum offset in data |  
|----------------------------------+
| 02. text checksum in DRAM        |  
|----------------------------------+
| 03. image checksum               |  
|----------------------------------|
| 04. relocation table size        |  
|----------------------------------|
| 05. this program size            |  
|----------------------------------|
| 06. data size                    | 
|----------------------------------|
| 07. text size                    |  
|----------------------------------|
| 08.                              |
|----------------------------------|
| 09.                              |
|----------------------------------|
| 10.                              |
|----------------------------------|
| 11.                              |
|----------------------------------|
| 12. final data address           |  <-- a5   (2)
|----------------------------------|
| 13. final text address           |  <-- a5   (1)
|----------------------------------|
| 14. linked data address          |  
|----------------------------------|       
| 15. linked text address          | 
|----------------------------------+
| 16. current data address         | 
|----------------------------------|
| 17. current text address         |  <-- a0
|----------------------------------+

|
| Set the relocation table address.
| First get text segment start address. Then push
| it back by the size of the relocation table.

L3:
	movel a2@(C_TEXT_ADDR), a5
	subl a2@(RELOC_SIZE), a5
	movel a5, a2@(R_TABLE_ADDR)

|           STACK
|----------------------------------+
| 00. Unused                       |
|----------------------------------|
| 01. text checksum offset in data |  
|----------------------------------+
| 02. text checksum in DRAM        |  
|----------------------------------+
| 03. image checksum               |  
|----------------------------------|
| 04. relocation table size        |  <-- a2   (1)
|----------------------------------|
| 05. this program size            |
|----------------------------------|
| 06. data size                    |
|----------------------------------|
| 07. text size                    |  
|----------------------------------|
| 08.                              |
|----------------------------------|
| 09.                              |
|----------------------------------|
| 10.                              |
|----------------------------------|
| 11. relocation table address     |  <-- a2   (2)
|----------------------------------|
| 12. final data address           |
|----------------------------------|
| 13. final text address           |
|----------------------------------|
| 14. linked data address          |
|----------------------------------|       
| 15. linked text address          |
|----------------------------------|
| 16. current data address         | 
|----------------------------------|
| 17. current text address         |  <-- a0
|----------------------------------+

|
| Calculate relocation amounts for text, data & bss
| Relocation amount = final address - link address

L4:
	movel a2@(F_TEXT_ADDR), d1
	subl a2@(L_TEXT_ADDR), d1
	movel d1, a2@(TEXT_RELOC_AMT)

	movel a2@(F_DATA_ADDR), d1
	subl a2@(L_DATA_ADDR), d1
	movel d1, a2@(DATA_RELOC_AMT)

	movel d1, a2@(BSS_RELOC_AMT)

|           STACK
|----------------------------------+
| 00. Unused                       |
|----------------------------------|
| 01. text checksum offset in data |  
|----------------------------------+
| 02. text checksum in DRAM        |  
|----------------------------------+
| 03. image checksum               |  
|----------------------------------|
| 04. relocation table size        | 
|----------------------------------|
| 05. this program size            |
|----------------------------------|
| 06. data size                    |
|----------------------------------|
| 07. text size                    | 
|----------------------------------|
| 08. bss relocation amount        |  <-- a2   (3)
|----------------------------------|
| 09. data relocation amount       |  <-- a2   (2)
|----------------------------------|
| 10. text relocation amout        |  <-- a2   (1)
|----------------------------------|
| 11. relocation table address     | 
|----------------------------------|
| 12. final data address           | 
|----------------------------------|
| 13. final text address           | 
|----------------------------------|
| 14. linked data address          | 
|----------------------------------|       
| 15. linked text address          | 
|----------------------------------|
| 16. current data address         | 
|----------------------------------|
| 17. current text address         |  <-- a0/a3
|----------------------------------+

|
L8:
        movel a2@(R_TABLE_ADDR), d1  | d1 = reloc table address
        movel d1, a2            | a2 = reloc table address
	moveq #1, d3		| two regions - text and data

	movel a0, a1
        addl #TEXT_RELOC_AMT, a1 | point a1 to text reloc amount    

|           STACK
|----------------------------------+
| 00. Unused                       |
|----------------------------------|
| 01. text checksum offset in data |  
|----------------------------------+
| 02. text checksum in DRAM        |  
|----------------------------------+
| 03. image checksum               |  
|----------------------------------|
| 04. relocation table size        | 
|----------------------------------|
| 05. this program size            |
|----------------------------------|
| 06. data size                    |
|----------------------------------|
| 07. text size                    |  
|----------------------------------|
| 08. bss relocation amount        |
|----------------------------------|
| 09. data relocation amount       |
|----------------------------------|
| 10. text relocation amount       |  <-- a1 
|----------------------------------|
| 11. relocation table address     |  <-- a2
|----------------------------------|
| 12. final data address           |
|----------------------------------|
| 13. final text address           |
|----------------------------------|
| 14. linked data address          |
|----------------------------------|       
| 15. linked text address          | 
|----------------------------------|
| 16. current data address         | 
|----------------------------------|
| 17. current text address         |  <-- a0/a1/a2
|----------------------------------+

|
|
| This is the outer section loop. We come here once for each section.
|
| a0 -> base of text/data address table 
| a1 -> base of text/data relocation amount table
| a2 -> pointer to relocation table start address
| d3 = count of regions left to do (regions = text + data)

L10:
	moveq #2, d0		| 3 sections to change - text, data, bss
        movel a1, a3

|
| This is the outer loop of relocation. We perform relocation for each
| type of reference within the section.
|
| a0 -> base of text/data address table 
| a1 -> base of text/data relocation amount table
| a2 -> pointer to relocation table start address
| a3 -> base of text/data relocation amount table (= a1)
| a5 = start address of section being relocated
|
| d0 = Count of target sections to do this image section
| d3 = Count of sections left to do
|


L20:
	movel a0@, a5		| a5 = Base address of section being relocated
	movel a3@+,d2		| Get change amount for this section
	movel a2@+,d4		| Get relocation count for section
        cmpl #0, d4             | Relocation count = 0 ?
	beq L40			| No relocation for this section
	
|
| This is the heart of relocation.
|
| a0 -> base of text/data address table 
| a1 -> base of text/data relocation amount table
| a2 -> pointer to relocation table start address
| a3 -> base of text/data relocation amount table (= a1)
| a5 = start address of section being relocated
|
| d0 = Count of target sections to do this image section
| d2 = change amount for this relocation section
| d3 = Count of sections left to do
| d4 = # of entries in relocation table
| 

L30:
	movel #0, d1
	moveb a2@+,d1
	addl d1, a5
	subql #1, d4		| Count one more item done
        cmpb #255, d1           | More deltas ?
	beq L30

        cmpl #0, d2             | Relocation amount = 0 ?
        beq L35                 | Yes. Jump to L35

	addl d2, a5@		| Perform the relocation

L35:
        cmpl #0, d4             | Relocation count = 0 ?
	bne L30			| Loop until we are done

|
| We have relocated the section. Check for more to do.
| If image is not at the final address, move it there.

L40:
	dbf d0, L20		| Loop for all regions within a section
	addl #(1*4), a0		| a0 -> current data address
	dbf d3, L10		| Loop for all sections

L50:
        subl #(2*4), a0         | restore a0
        addl #(3*4), a1         | restore a1

	movel a0@(C_TEXT_ADDR), a5
	movel a0@(F_TEXT_ADDR), a3
        cmpl a5, a3             | current text addr = final addr ?
        beq L90                 | Yes. Jump to L90
 
	movel a0@(TEXT_SIZE), d1
	addl a0@(DATA_SIZE), d1
        addl a5, d1             | d1 now contains current end of data

L55:
        movel a5@+, a3@+        | move from current to final address
        cmpl d1, a5             | reached the end ?
        blt L55
        bra L90			| done

|
|
|-------------------------------------------------------------------------
|L60:
|        movel a0, a5
|        addl #(1*4), a5         | point a5 to current data address
|        movel a5@, a2           | a2 = current data address
|        addl #(4*4), a5         | point a5 to final data address
|        movel a5@, a3           | a3 = final data address
|        cmpl a2, a3             | current data address = final data address ?
|        beq L70                 | Jump if yes. Data need not be moved
|  
|        movel a1, a5
|        addl #(1*4), a5         | point a5 to data size
|        movel a5@, d1           | d1 = data size
|        addl a2, d1             | d1 = end of data section
|
|L65:
|        movel a2@+, a3@+        | copy data
|        cmpl d1, a2
|        blt L65
|
|| Initialize bss area
|
|L70:
|        movel a0, a5
|        addl #(5*4), a5         | point a5 to final data address
|        movel a5@, a2           | a2 = final data address
|        movel a1, a5
|        addl #(1*4), a5         | point a5 to data size
|        movel a5@, a3           | a3 = current data size
|        addl a3, a2             | add data size to address to get bss addr
|        movel a4, a5
|        subl #(3*4), a5         | point a5 to r_bsize in spl_hdr
|        movel a5@, a3           | a3 = bss size
|        addl a2, a3             | a3 = end of bss
|
|L75:
|        clrl a2@+               | clear
|        cmpl a3, a2             | reached end of bss ?
|        blt L75                 | No. Loop back.
|        
|-------------------------------------------------------------------------

|                 0 000000001111111111222222222 2 3
|                 1 234567890123456789012345678 9 0
Icksummsg: .asciz "\n** Image checksum error **\n"
Icksummsg_end:

	.even
IC_err_msg:
	lea Icksummsg, a1	 | a1 points to start of message
	movel a1@, d1  		 | dummy instruction

IC_err_msg1:
	moveb a1@+, d1
	cmpb #0, d1		 | end of string ?
	beq LERR
	extbl d1
	movel d1, sp@-	 	 | push character onto stack
	movel #1, sp@-		 | push trap type onto stack
	jbsr _emt_call		 | make EMT call for char output
	addl #8, sp		 | pop character from stack
	bra IC_err_msg1		 | Loop back.

|                 0 00000000111111111122222222223 3 3
|                 1 23456789012345678901234567890 1 2
SHcksummsg: .asciz "\n** Header checksum error **\n"
SHcksummsg_end:

	.even
SHC_err_msg:
	lea SHcksummsg, a1	 | a1 points to start of message
	movel a1@, d1  		 | dummy instruction

SHC_err_msg1:
	moveb a1@+, d1
	cmpb #0, d1		 | end of string ?
	beq LERR
	extbl d1
	movel d1, sp@-	 	 | push character onto stack
	movel #1, sp@-		 | push trap type onto stack
	jbsr _emt_call		 | make EMT call for char output
	addl #8, sp		 | pop character from stack
	bra SHC_err_msg1	 | Loop back.

| EMT call for console output
|
| emt_call
| ROM monitor emulation trap support
| 
| int emt_call (traptype[,arg1[,arg2[,arg3]]])
| 

_emt_call:
	link a6,#0
	movl a6@(20),sp@-
	movl a6@(16),sp@-
	movl a6@(12),sp@-
	movl a6@(8),sp@-
	trap #15
|
| The next instruction fixes
| up the stack for us
|
	unlk a6
	rts

| Clean up stack space used

LERR:
|------------- The error exit point ---------------------
| Catastrophic error. Return back to ROM monitor.

	addl #(CTS*4),sp	| Clean up context space
	moveml sp@+,#0x3fff	| Restore registers
	rts
	
|------------- The normal exit point --------------------
L90:
	addl #(CTS*4),sp	| Clean up context space
	moveml sp@+,#0x3fff	| Restore registers
	nop
