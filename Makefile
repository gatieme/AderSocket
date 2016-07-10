ROOT=.
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

DIRS=simple fork pthread lcpthread select poll epoll


all:
	for i in $(DIRS); do \
		(cd $$i && echo "making $$i" && $(MAKE) ) || exit 1; \
	done

clean:
	for i in $(DIRS); do \
		(cd $$i && echo "cleaning $$i" && $(MAKE) clean) || exit 1; \
	done


github:
	make clean
	git add -A
	git commit -m $(GITHUB_COMMIT)
	git push origin master
