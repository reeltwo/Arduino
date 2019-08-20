SUBDIRS = $(filter-out libraries/,$(sort $(dir $(shell ls -d */Makefile))))
SUBDIRS := $(SUBDIRS)

MAKE_OPTS =
ifeq ("$(SILENT)","1")
MAKE_OPTS += -s
endif

all:
	@for dir in $(SUBDIRS) ; do \
		make $(MAKE_OPTS) -C $$dir ; \
	done

upload:
	@for dir in $(SUBDIRS) ; do \
		make $(MAKE_OPTS) -C $$dir upload ; \
	done

build:
	@for dir in $(SUBDIRS) ; do \
		make $(MAKE_OPTS) -C $$dir build ; \
	done

clean:
	@for dir in $(SUBDIRS) ; do \
		make $(MAKE_OPTS) -C $$dir clean ; \
	done
