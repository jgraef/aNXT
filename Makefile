# This Makefile is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

-include Makefile.config

.PHONY: all installfiles install tar.gz clean realclean

all: bin/anxt-config
	make -C libanxt_net/
	make -C libanxt/
	make -C libanxt_file/
	make -C libanxt_tools/
	make -C nxtd/
	make -C examples/
	make -C tools/

##### make config script ####

bin/anxt-config: tools/anxt-config-creator.sh
	sh $^ $(PREFIX) > $@
	chmod 755 $@

##### Install target #####

installfiles:
	mkdir -p $(PATH_LIB)
	cp -R lib/* $(PATH_LIB)
	mkdir -p $(PATH_INCLUDE)
	cp -R include/* $(PATH_INCLUDE)
	mkdir -p $(PATH_BIN)
	cp -R bin/* $(PATH_BIN)
	mkdir -p $(PATH_MAN)
	cp -R doc/man/* $(PATH_MAN)

install: installfiles
	ln -sf $(PATH_LIB)/libanxt.so.1 $(PATH_LIB)/libanxt.so
	ln -sf $(PATH_LIB)/libanxt_tools.so.1 $(PATH_LIB)/libanxt_tools.so
	ln -sf $(PATH_LIB)/libanxt_file.so.1 $(PATH_LIB)/libanxt_file.so
	ln -sf $(PATH_LIB)/libanxt_net.so.1 $(PATH_LIB)/libanxt_net.so

##### Build distributable archive

tar.gz:
	(set -x;ANXT=aNXT-`bin/anxt-config --version` && rm -rf /tmp/$$ANXT && mkdir /tmp/$$ANXT && tar -cvf - * | (cd /tmp/$$ANXT && tar -xf -) && (cd /tmp/$$ANXT && make realclean && cd .. && tar -cf $$ANXT.tar $$ANXT && gzip -9f $$ANXT.tar))

##### Clean target #####

clean:
	rm -f bin/anxt-config
	make -C libanxt/ clean
	make -C libanxt_file/ clean
	make -C libanxt_net/ clean
	make -C libanxt_tools/ clean
	make -C nxtd/ clean
	make -C examples/ clean
	make -C tools/ clean

realclean: clean
	(find . -name .svn -print | xargs rm -rf)
