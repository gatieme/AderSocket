ROOT=.
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

MAKE_DIR=make -C


SUBDIRS=simple fork pthread lcpthread select poll epoll

.PHONY:all $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE_DIR) $@


.PHONY : clean
clean:
	for i in $(SUBDIRS); do \
		(cd $$i && echo "cleaning $$i" && $(MAKE) clean) || exit 1; \
	done


github:
	make clean
	git add -A
	git commit -m $(GITHUB_COMMIT)
	git push origin master
