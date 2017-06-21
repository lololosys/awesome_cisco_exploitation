
IOS ELF Sections

-	SHN_UNDEF – 
This section does not contain any data and its size is 0. Don’t know the purpose of the section. It may change during the installation/running of the IOS firmware/kernel

-	.text – 
This section contains the code to be run. 
It’s type is SHT_PROGBITS, this is a special section that holds program and control information, the attributes used are SHF_ALLOC and SHF_EXECINSTR which means that this section contains machine instructions (code which are executed) and this section occupies memory during the process execution.

-	.rodata – 
This section contains the read only data related to the firmware (cannot be sure).

-	.inital_fw – 
This section contains read only data related to the initial firmware unpacking/loading – cannot be sure
 
-	.e1t1_fw – 
This section contains read only data related to e1 & t1 telecommunication protocols (transferring simultaneously voice conversions) – Cannot be sure 
Opening up the section’s content, revealed that the first  4 bytes of the section are 0x504b0304. Searching this magic on Google revealed that this is a ZIP file.

-	.eh_frame - 
This section contains read only data related to exceptions. When using exceptions in languages like C++ additional information must be provided to the runtime environment that describes the call frames that much be unwind during the process of an exception

-	.sdata2
This section contains read-write data. Cannot be sure.

-	.data
This section contains read-write data related to the firmware/kernel.

-	.got2
This section contains read-write data related to PIC and relocations.

-	.ctors
This section contains read-write data related to the constructors for the .text.

-	.sdata
This section contains read-write data. Cannot be sure.

-	.sbss
This section contain the uninitialized data.

-	.shstrtab
This section contain the section header string table.

