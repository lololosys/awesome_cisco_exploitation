'''
	File name: dump_sections.py
'''

import idautils
import idc
import idaapi

SECTION_NAME = '.e1t1_fw'
FOUND_FLAG = False

for sec in idautils.Segments():
	sec_name = idc.SegName(sec)
	
	if sec_name == SECTION_NAME:
		FOUND_FLAG = True
		start = idc.GetSegmentAttr(sec, idc.SEGATTR_START)
		end = idc.GetSegmentAttr(sec, idc.SEGATTR_END)

		with open(SECTION_NAME+".bin", 'wb') as f:
			while start < end:
				if not idc.hasValue(idc.GetFlags(start)):
					continue
					
				data = idc.Byte(start)
				f.write(chr(data))
				start += 1
				
if not FOUND_FLAG:
	print "[+] Could not found %s section " %(SECTION_NAME)
else:
	print "[+] Done "