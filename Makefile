
all: doc plugin
	make -C ./src
doc: 
	doxygen

plugin:
	make -C ./src/plugins

install:
	cp bin/* /var/qmail/bin/
	qmailctl restart

clean: 
	rm -rf obj/*

format:
	find . -name "*.c" | xargs indent
	find . -name "*.h" | xargs indent

mrproper: clean
	rm -rf plugins/*
	rm -rf bin/*
	rm -rf doc/*
	find . -name "*.c~" | xargs rm
	find . -name "*.h~" | xargs rm
