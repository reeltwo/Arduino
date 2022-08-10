TARGET?=ESP32
PWD=$(shell pwd)
# FIRMWARE=	R2UppitySpinnerV3/ \
# 	 		DomeControlFirmware/ \
# 	 		DomeSensorFirmware32/ \
# 	 		DroidRemoteFirmware/ \
# 	 		AstroPixelsPlus/ \
# 	 		DarthServoFirmware/ \
# 	 		WLogicEngine32/ \
# 	 		Penumbra/ \
# 	 		DomeFirmware/ \
# 	 		BodyFirmware/ \
# 	 		MagicPanelFirmware/ \
# 	 		DomeButtonFirmware/ \
# 	 		FrontSkinFirmware/

FIRMWARE=$(patsubst %.auto,%,$(wildcard */.auto))
$(warning $(FIRMWARE))

TARGETS := all clean
EXAMPLE_DIRS=$(wildcard libraries/Reeltwo/examples/*/.)

# foo/.all bar/.all foo/.clean bar/.clean
EXAMPLE_DIRS_TARGETS := \
	$(foreach t,$(TARGETS),$(addsuffix $t,$(EXAMPLE_DIRS)))

FIRMWARE_TARGETS := \
	$(foreach t,$(TARGETS),$(addsuffix $t,$(FIRMWARE)))

.PHONY: $(TARGETS) $(EXAMPLE_DIRS_TARGETS) $(FIRMWARE_TARGETS)

$(TARGETS): % : $(addsuffix %,$(EXAMPLE_DIRS))
	@echo 'Done "$*" target'

# $(TARGETS): % : $(addsuffix %,$(FIRMWARE))
# 	@echo 'Done "$*" target'

$(EXAMPLE_DIRS_TARGETS):
	@RESULT=0 ;\
	 echo "TARGET?=$(TARGET)" > $(@D)/Makefile ;\
	 echo "PORT?=/dev/ttyUSB0" >> $(@D)/Makefile ;\
	 echo "PROJECT_LIBRARIES=$(PWD)/libraries" >> $(@D)/Makefile ;\
	 echo "GITHUB_REPOS= \\" >> $(@D)/Makefile ;\
	 echo "reeltwo/Reeltwo \\" >> $(@D)/Makefile ;\
	 echo "adafruit/Adafruit_NeoPixel \\" >> $(@D)/Makefile ;\
	 echo "FastLED/FastLED \\" >> $(@D)/Makefile ;\
	 echo "" >> $(@D)/Makefile ;\
	 echo "include $(PWD)/Arduino.mk" >> $(@D)/Makefile ;\
	 cd $(@D) ;\
	 CHECKREQUIRES=`sed -n '1{/^\/\/REQUIRES:/p};q' *.ino` ;\
	 if [ "$$CHECKREQUIRES" = "" ] || [ "$$CHECKREQUIRES" = "//REQUIRES:ESP32" ]; then \
	 	echo "Building $(@D)" ;\
	 	$(MAKE) $(@F:.%=%);\
	 	RESULT=$$? ;\
	 fi ;\
	 exit $$RESULT

$(FIRMWARE_TARGETS):
	@RESULT=0 ;\
	 cd $(@D) ;\
	 CHECKREQUIRES=`sed -n '1{/^\/\/REQUIRES:/p};q' *.ino` ;\
	 if [ "$$CHECKREQUIRES" = "" ] || [ "$$CHECKREQUIRES" = "//REQUIRES:ESP32" ]; then \
	 	echo "Building $(@D)" ;\
	 	$(MAKE) $(@F:.%=%);\
	 	RESULT=$$? ;\
	 fi ;\
	 exit $$RESULT

docs:
	@cd libraries/Reeltwo ;\
	 doxygen docs/ReelTwo.doxygen ; \
	 if [ $$? -eq 0 ]; then \
	 	git status docs/html | grep "Changes not staged" ; \
		if [ $$? -eq 0 ]; then \
		 	cd docs ; \
		 	rm -rf reeltwo-doc html/reeltwo-doc.tgz ; \
		 	mkdir -p reeltwo-doc ; \
		 	rsync -au html index.html reeltwo-doc ; \
		 	tar zcf html/reeltwo-doc.tgz reeltwo-doc/* ; \
		 	rm -rf reeltwo-doc ; \
		fi \
	 fi

status:
	@for dir in $(FIRMWARE) libraries/Reeltwo ; do \
		cd $$dir ; \
		if [ -d .git ]; then \
			git status | grep "nothing to commit" > /dev/null ; \
			if [ $$? -ne 0 ]; then \
				echo Changes in: $$dir ; \
				git status ; \
			else \
				echo Clean $$dir ; \
			fi \
		fi ; \
		cd $(PWD) ; \
	done

release:
	@cd libraries/Reeltwo ; \
	RELEASE=$$(date '+%y.%-m.%-d') ; \
	sed -i "s/^[#]*\s*version=.*/version=$$RELEASE/" library.properties ; \
	echo RELEASE: $$RELEASE ; \
	rm -rf Reeltwo Reeltwo.zip ; \
	mkdir Reeltwo ; \
	rsync -au CMakeLists.txt component.mk docs documentation.html  library.properties LICENSE README.md src Reeltwo/ ; \
	rm Reeltwo/docs/html/*.tgz ; \
	zip -qr9 Reeltwo.zip Reeltwo ; \
	rm -rf Reeltwo
