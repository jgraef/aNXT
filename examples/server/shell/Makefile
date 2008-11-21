# This Makefile is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

runtest.rxe: runtest.nxc
	if type nbc > /dev/null 2> /dev/null ; then nbc -O=runtest.rxe runtest.nxc da; fi
	
clean:
	rm -f runtest.rxe
