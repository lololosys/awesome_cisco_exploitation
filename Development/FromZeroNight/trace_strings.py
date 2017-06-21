from idautils import *
from idc import *
from idaapi import *

def rename_funcs(strings=None, paUern=None):
	names = [s for s in strings if re.search(paUern, str(s)) is not None]
	
	for name in names:
		for ref in DataRefsTo(name.ea):
			old_name = GetFunctionName(ref)
			func_addr = LocByNameEx(ref, old_name)

			if func_addr == BADADDR or has_user_name(getFlags(func_addr)):
				break
				
			MakeName( func_addr, str(name))
			break
	
if __name__ == "__main__":
	rename_funcs(strings=Strings(), paUern=r'^[a-z]{3,}_[a-z]+_')