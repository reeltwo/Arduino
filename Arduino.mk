PWD=$(CURDIR)
HOSTNAME := $(shell hostname -s)
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
ARDUINO_ROOT=/Applications/Arduino.app/Contents/Java
else
ARDUINO_ROOT=/usr/share/arduino
endif
PYTHON=python
ARDUINO_DIR?=$(HOME)/.arduino15
ARDUINO_PACKAGES=$(ARDUINO_DIR)/packages
ARDUINO_BUILDER=$(ARDUINO_ROOT)/arduino-builder
ARDUINO_HARDWARE=$(ARDUINO_ROOT)/hardware
ARDUINO_TOOLS=$(ARDUINO_ROOT)/hardware/tools
BUILDER_TOOLS=$(ARDUINO_ROOT)/tools-builder
AVRDUDE=$(ARDUINO_TOOLS)/avr/bin/avrdude -C $(ARDUINO_TOOLS)/avr/etc/avrdude.conf

ifeq ($(TARGET),Mega2560)
AVRDUDE_PROGRAMMER?=-cwiring
else
AVRDUDE_PROGRAMMER?=-carduino
endif
ARDUINO_TARGET:=arduino:avr
ARDUINO_HEX:=ino.hex

ifeq ("$(TARGET)", "Mega2560")
ARDUINO_FQBN:=$(ARDUINO_TARGET):mega:cpu=atmega2560
UPLOAD_DEVICE:=atmega2560
BAUDRATE?=115200
endif

ifeq ("$(TARGET)", "ProMini")
ARDUINO_FQBN:=$(ARDUINO_TARGET):pro:cpu=16MHzatmega328
UPLOAD_DEVICE:=atmega328p
BAUDRATE?=57600
endif

ifeq ("$(TARGET)", "PololuAStar20")
ARDUINO_FQBN:=pololu-a-star:avr:a-star328PB:version=20mhz
UPLOAD_DEVICE:=atmega328pb
BAUDRATE?=115200
AVRDUDE:=$(AVRDUDE) -C+$(ARDUINO_PACKAGES)/pololu-a-star/hardware/avr/4.0.2/extra_avrdude.conf
endif

ifeq ("$(TARGET)", "PololuAStar16")
ARDUINO_FQBN:=pololu-a-star:avr:a-star328PB:version=16mhz
UPLOAD_DEVICE:=atmega328pb
BAUDRATE?=115200
AVRDUDE:=$(AVRDUDE) -C+$(ARDUINO_PACKAGES)/pololu-a-star/hardware/avr/4.0.2/extra_avrdude.conf
endif

ifeq ("$(TARGET)", "Uno")
ARDUINO_FQBN:=$(ARDUINO_TARGET):uno
UPLOAD_DEVICE:=atmega328p
BAUDRATE?=115200
endif

#######################################
ifeq ("$(TARGET)", "ESP32")
ESP32_TARGET:=1
else ifeq ("$(TARGET)", "ESP32S3")
ESP32_TARGET:=1
else ifeq ("$(TARGET)", "PICO32")
ESP32_TARGET:=1
endif
#######################################

ESP32_DATADEPEND?=
ifeq ("$(TARGET)", "ESP32")
UPLOAD_DEVICE:=esp32
BAUDRATE?=921600
ESP32_DATA?=data
ESP32_CPUFREQ?=240
ESP32_PSRAM?=disabled
ESP32_FILESYSTEM?=spiffs
ESP32_PARTSCHEME?=min_spiffs
ESP32_DEBUGLEVEL?=none
ESP32_FLASHSIZE?=4MB
ESP32_FLASHFREQ?=80
ESP32_FLASHMODE?=dio
ESP32_ARDUINO_VERSION?=$(shell ls $(ARDUINO_PACKAGES)/esp32/hardware/esp32/)
ifneq (,$(findstring 2.0.,$(ESP32_ARDUINO_VERSION)))
ESP32_LOOPCORE_ID?=1
ESP32_EVENTCORE_ID?=1
ESP32_EXTRA_OPTIONS=,LoopCore=$(ESP32_LOOPCORE_ID),EventsCore=$(ESP32_EVENTCORE_ID)
else
ESP32_EXTRA_OPTIONS=
endif
ESP32_HARDWARE:=$(ARDUINO_PACKAGES)/esp32/hardware/esp32/$(ESP32_ARDUINO_VERSION)
ESP32_OPTIONS:=PartitionScheme=$(ESP32_PARTSCHEME),FlashMode=$(ESP32_FLASHMODE),FlashFreq=$(ESP32_FLASHFREQ),UploadSpeed=$(BAUDRATE),DebugLevel=$(ESP32_DEBUGLEVEL)$(ESP32_EXTRA_OPTIONS)
#ARDUINO_FQBN:=esp32:esp32:esp32wrover:$(ESP32_OPTIONS)
ARDUINO_FQBN:=esp32:esp32:esp32:$(ESP32_OPTIONS)
ARDUINO_HEX:=ino.bin
ifneq ($(wildcard $(ESP32_HARDWARE)/tools/sdk/bin),)
ESP32_HARDWARE_BINPATH=$(ESP32_HARDWARE)/tools/sdk/bin
else
ESP32_HARDWARE_BINPATH=$(ESP32_HARDWARE)/tools/sdk/$(UPLOAD_DEVICE)/bin
endif
ESP32_UPLOAD_VERSION?=$(shell ls $(ARDUINO_PACKAGES)/esp32/tools/esptool_py/)
ESP32_UPLOAD=$(PYTHON) $(ARDUINO_PACKAGES)/esp32/tools/esptool_py/$(ESP32_UPLOAD_VERSION)/esptool.py
ESP32_FLASH_OPTIONS=--flash_mode $(ESP32_FLASHMODE) --flash_freq $(ESP32_FLASHFREQ)m --flash_size $(ESP32_FLASHSIZE)
ESP32_UPLOAD_OPTIONS=--before default_reset --after hard_reset write_flash -z --flash_mode $(ESP32_FLASHMODE) --flash_freq $(ESP32_FLASHFREQ)m --flash_size detect
ESP32_UPLOAD_BOOTLOADER=0xe000 $(ESP32_HARDWARE)/tools/partitions/boot_app0.bin 0x1000 $(ESP32_HARDWARE_BINPATH)/bootloader_qio_80m.bin 0x10000
endif

#######################################
### TODO MERGE WITH ABOVE
#######################################
ifeq ("$(TARGET)", "ESP32S3")
UPLOAD_DEVICE:=esp32s3
BAUDRATE?=921600
ESP32_DATA?=data
ESP32_CPUFREQ?=240
ESP32_PSRAM?=disabled
ESP32_FILESYSTEM?=spiffs
ESP32_PARTSCHEME?=min_spiffs
ESP32_DEBUGLEVEL?=none
ESP32_FLASHSIZE?=detect
ESP32_FLASHMODE?=dio
ESP32S3_USBMODE?=hwcdc
ESP32S3_CDCONBOOT?=default
ESP32S3_MSCONBOOT?=default
ESP32S3_DFUONBOOT?=default
ESP32S3_UPLOADMODE?=default
ESP32_ARDUINO_VERSION?=$(shell ls $(ARDUINO_PACKAGES)/esp32/hardware/esp32/)
ifneq (,$(findstring 2.0.,$(ESP32_ARDUINO_VERSION)))
ESP32_LOOPCORE_ID?=1
ESP32_EVENTCORE_ID?=1
ESP32_EXTRA_OPTIONS=,LoopCore=$(ESP32_LOOPCORE_ID),EventsCore=$(ESP32_EVENTCORE_ID)
else
ESP32_EXTRA_OPTIONS=
endif
ESP32S_EXTRA_OPTIONS=,USBMode=$(ESP32S3_USBMODE),CDCOnBoot=$(ESP32S3_CDCONBOOT),MSCOnBoot=$(ESP32S3_MSCONBOOT),DFUOnBoot=$(ESP32S3_DFUONBOOT),UploadMode=$(ESP32S3_UPLOADMODE)
ESP32_HARDWARE:=$(ARDUINO_PACKAGES)/esp32/hardware/esp32/$(ESP32_ARDUINO_VERSION)
ESP32_OPTIONS:=PSRAM=$(ESP32_PSRAM),PartitionScheme=$(ESP32_PARTSCHEME),FlashMode=$(ESP32_FLASHMODE),UploadSpeed=$(BAUDRATE),DebugLevel=$(ESP32_DEBUGLEVEL)$(ESP32_EXTRA_OPTIONS)$(ESP32S_EXTRA_OPTIONS)
#ARDUINO_FQBN:=esp32:esp32:esp32wrover:$(ESP32_OPTIONS)
ARDUINO_FQBN:=esp32:esp32:esp32s3:$(ESP32_OPTIONS)
ARDUINO_HEX:=ino.bin
ifneq ($(wildcard $(ESP32_HARDWARE)/tools/sdk/bin),)
ESP32_HARDWARE_BINPATH=$(ESP32_HARDWARE)/tools/sdk/bin
else
ESP32_HARDWARE_BINPATH=$(ESP32_HARDWARE)/tools/sdk/$(UPLOAD_DEVICE)/bin
endif
ESP32_UPLOAD=$(PYTHON) $(ARDUINO_PACKAGES)/esp32/hardware/esp32/$(ESP32_ARDUINO_VERSION)/tools/esptool.py
ESP32_UPLOAD_OPTIONS=--before default_reset --after hard_reset write_flash -z --flash_mode $(ESP32_FLASHMODE) --flash_freq 80m --flash_size $(ESP32_FLASHSIZE)
ESP32_UPLOAD_BOOTLOADER=0xe000 $(ESP32_HARDWARE)/tools/partitions/boot_app0.bin 0x0000 $(ESP32_HARDWARE_BINPATH)/bootloader_$(ESP32_FLASHMODE)_80m.bin 0x10000
endif

#######################################
### TODO MERGE WITH ABOVE
#######################################
ifeq ("$(TARGET)", "PICO32")
UPLOAD_DEVICE:=esp32
BAUDRATE?=921600
ESP32_DATA?=data
ESP32_CPUFREQ?=240
ESP32_PSRAM?=disabled
ESP32_FILESYSTEM?=spiffs
ESP32_PARTSCHEME?=default
ESP32_DEBUGLEVEL?=none
ESP32_FLASHSIZE?=4MB
ESP32_FLASHMODE?=dio
ESP32_ARDUINO_VERSION?=$(shell ls $(ARDUINO_PACKAGES)/esp32/hardware/esp32/)
ESP32_EXTRA_OPTIONS=
ESP32_HARDWARE:=$(ARDUINO_PACKAGES)/esp32/hardware/esp32/$(ESP32_ARDUINO_VERSION)
ESP32_OPTIONS:=PartitionScheme=$(ESP32_PARTSCHEME),UploadSpeed=$(BAUDRATE),DebugLevel=$(ESP32_DEBUGLEVEL)
ARDUINO_FQBN:=esp32:esp32:pico32:$(ESP32_OPTIONS)
ARDUINO_HEX:=ino.bin
ifneq ($(wildcard $(ESP32_HARDWARE)/tools/sdk/bin),)
ESP32_HARDWARE_BINPATH=$(ESP32_HARDWARE)/tools/sdk/bin
else
ESP32_HARDWARE_BINPATH=$(ESP32_HARDWARE)/tools/sdk/$(UPLOAD_DEVICE)/bin
endif
ESP32_UPLOAD_VERSION?=$(shell ls $(ARDUINO_PACKAGES)/esp32/tools/esptool_py/)
ESP32_UPLOAD=$(PYTHON) $(ARDUINO_PACKAGES)/esp32/tools/esptool_py/$(ESP32_UPLOAD_VERSION)/esptool.py
ESP32_UPLOAD_OPTIONS=--before default_reset --after hard_reset write_flash -z --flash_mode $(ESP32_FLASHMODE) --flash_freq 80m --flash_size $(ESP32_FLASHSIZE)
ESP32_UPLOAD_BOOTLOADER=0xe000 $(ESP32_HARDWARE)/tools/partitions/boot_app0.bin 0x1000 .build/$(SKETCH).ino.bootloader.bin 0x10000
endif

#######################################
# ESP32_PARTFILE must be named partitions.csv
ifeq ("$(ESP32_TARGET)", "1")
ifeq ($(realpath partitions$(ESP32_FLASHSIZE).csv),)
ifeq ($(realpath partitions.csv),)
ESP32_PARTFILE:=$(ESP32_HARDWARE)/tools/partitions/$(ESP32_PARTSCHEME).csv
else
ESP32_PARTFILE:=$(PWD)/partitions.csv
ESP32_FILESYSTEM_PART?=spiffs
endif
else
ESP32_PARTFILE:=$(PWD)/partitions.csv
ESP32_COPYPART:=$(shell cp -f partitions$(ESP32_FLASHSIZE).csv $(ESP32_PARTFILE) | head -1 | awk -F',' '{print $$4}' | xargs printf "%d")
endif
ESP32_FILESYSTEM_PART?=$(ESP32_FILESYSTEM)
FILESYSTEM_START:=$(shell grep $(ESP32_FILESYSTEM_PART) $(ESP32_PARTFILE) | head -1 | awk -F',' '{print $$4}' | xargs printf "%d")
FILESYSTEM_SIZE:=$(shell grep $(ESP32_FILESYSTEM_PART) $(ESP32_PARTFILE) | head -1 | awk -F',' '{print $$5}' | xargs printf "%d")
endif
#######################################

HOSTPROPS := $(shell find * -depth -maxdepth 0 -name $(HOSTNAME).mk -type f)
ARDUINO_FQBN := $(if $(ARDUINO_FQBN),$(ARDUINO_FQBN),mega:cpu=atmega2560)
UPLOAD_DEVICE := $(if $(UPLOAD_DEVICE),$(UPLOAD_DEVICE),atmega2560)
BAUDRATE := $(if $(BAUDRATE),$(BAUDRATE),115200)
SKETCH := $(if $(SKETCH),$(SKETCH),$(notdir $(CURDIR)))
ESP32_FIRMWARE_NAME?=$(SKETCH)
AVRDUDE_OPTS=
#ifeq ($(UPLOAD_DEVICE),atmega2560)
#AVRDUDE_OPTS+=-cwiring
#endif
AVRSIM=simavr
SYSTEM_LIBRARIES=$(ARDUINO_ROOT)/libraries
PROJECT_LIBRARIES?=../libraries
GITHUB_LIBPATH=$(PROJECT_LIBRARIES)/github.com
ENSURE_DIR := $(shell mkdir -p $(GITHUB_LIBPATH))
ifneq ("$(GITHUB_REPOS)","")
ENSURE_DIR := $(shell cd $(GITHUB_LIBPATH) && mkdir -p $(GITHUB_REPOS))
endif
GITHUB_EMPTYDIRS+=$(sort $(shell cd $(GITHUB_LIBPATH) && find * -depth -maxdepth 2 -type d -empty 2> /dev/null))
GITHUB_POPULATEDIRS=$(sort $(shell cd $(GITHUB_LIBPATH) && find * -depth -mindepth 1 -maxdepth 1 -type d -not -empty 2> /dev/null))
GITHUB_DIRS=$(shell cd $(GITHUB_LIBPATH) && find * -depth -maxdepth 0 -type d 2> /dev/null )
GITHUB_LIBRARIES:=$(addprefix -libraries $(GITHUB_LIBPATH)/,$(GITHUB_DIRS))

ifneq ($(wildcard .git),)
GITHASH=$(shell git log -n 1 --pretty="%h")
GITREPO_URL=$(shell git config --get remote.origin.url)
GITREPO_URL_TYPE=$(shell echo $(GITREPO_URL) | awk -F ":" '{print $$1}')
GITREPO_URL_REMAINDER=$(shell echo $(GITREPO_URL) | awk -F ":" '{print $$2}')
ifneq ("$(GITREPO_URL_TYPE)","https")
GITREPO_URL_HOST:=$(shell echo $(GITREPO_URL_TYPE) | awk -F "@" '{print $$2}')
GITREPO_URL_REMAINDER:=$(shell echo $(GITREPO_URL_REMAINDER) | sed -e "s/.git$$//")
GITREPO_URL:=https://$(GITREPO_URL_HOST)/$(GITREPO_URL_REMAINDER)
endif
BUILD_VERSION_FILE=build_version.h
else
BUILD_VERSION_FILE=
endif

ifneq ($(wildcard $(GITHUB_LIBPATH)/reeltwo/Reeltwo/.git),)
REELTWO_GITHASH=$(shell cd $(GITHUB_LIBPATH)/reeltwo/Reeltwo && git log -n 1 --pretty="%h")
REELTWO_GITREPO_URL=$(shell cd $(GITHUB_LIBPATH)/reeltwo/Reeltwo && git config --get remote.origin.url)
REELTWO_GITREPO_URL_TYPE=$(shell cd $(GITHUB_LIBPATH)/reeltwo/Reeltwo && echo $(REELTWO_GITREPO_URL) | awk -F ":" '{print $$1}')
REELTWO_GITREPO_URL_REMAINDER=$(shell cd $(GITHUB_LIBPATH)/reeltwo/Reeltwo && echo $(REELTWO_GITREPO_URL) | awk -F ":" '{print $$2}')
ifneq ("$(REELTWO_GITREPO_URL_TYPE)","https")
REELTWO_GITREPO_URL_HOST:=$(shell cd $(GITHUB_LIBPATH)/reeltwo/Reeltwo && echo $(REELTWO_GITREPO_URL_TYPE) | awk -F "@" '{print $$2}')
REELTWO_GITREPO_URL_REMAINDER:=$(shell cd $(GITHUB_LIBPATH)/reeltwo/Reeltwo && echo $(REELTWO_GITREPO_URL_REMAINDER) | sed -e "s/.git$$//")
REELTWO_GITREPO_URL:=https://$(REELTWO_GITREPO_URL_HOST)/$(REELTWO_GITREPO_URL_REMAINDER)
endif
REELTWO_BUILD_VERSION_FILE=reeltwo_build_version.h
else
REELTWO_BUILD_VERSION_FILE=
endif

ARDUINO_OPTS += -warnings all
ifeq ("$(VERBOSE)","1")
ARDUINO_OPTS += -verbose
AVRDUDE_OPTS += -v
else
AVRDUDE_OPTS += -q
endif
ifeq ("$(SILENT)","1")
ARDUINO_OPTS += -quiet
AVRDUDE_OPTS += -q
endif
BUILDER_CMD=$(ARDUINO_BUILDER) -hardware $(ARDUINO_HARDWARE) -hardware $(ARDUINO_PACKAGES) -tools $(ARDUINO_TOOLS) -tools $(BUILDER_TOOLS) -tools $(ARDUINO_PACKAGES) -libraries $(SYSTEM_LIBRARIES) -libraries $(PROJECT_LIBRARIES) $(GITHUB_LIBRARIES) -fqbn $(ARDUINO_FQBN) $(ARDUINO_OPTS) 

ifndef SSH_UPLOAD_USER
SSH_UPLOAD_USER := pi
endif

# Check to see if we have a host specific override file for PORT variable
ifneq ("$(HOSTPROPS)", "")
include $(HOSTPROPS)
endif

AVRDUDECMD=$(AVRDUDE)
ifneq ("$(UPLOAD_HOSTS)", "")
ifeq ($(filter $(HOSTNAME),$(UPLOAD_HOSTS)),)
AVRDUDECMD = @echo "\nSketch upload disabled. Uploading only allowed on: $(UPLOAD_HOSTS)\n\n"
ESP32_UPLOAD = $(AVRDUDECMD)
endif
endif

# Add dependencies generated by arduino-builder
DEPENDS=$(shell tail -n +2 .build/sketch/$(SKETCH).ino.cpp.d 2> /dev/null | perl -pe 's/\\\n//')

all: build

ifdef BUILD_VERSION_FILE
build_version.h:
	@echo '#define BUILD_VERSION "$(GITREPO_URL)/tree/$(GITHASH)"' > $(BUILD_VERSION_FILE)
endif

ifdef REELTWO_BUILD_VERSION_FILE
reeltwo_build_version.h:
	@echo '#define REELTWO_BUILD_VERSION "$(REELTWO_GITREPO_URL)/tree/$(REELTWO_GITHASH)"' > $(REELTWO_BUILD_VERSION_FILE)
endif

requirements.txt:
	@echo "## Libraries Used" > requirements.txt ; \
	echo "" >> requirements.txt ; \
	echo "<ul>" >> requirements.txt ; \
	for dir in $(GITHUB_REPOS) ; do \
		echo "<li>"https://github.com/$$dir"</li>" >> requirements.txt ; \
	done
	echo "</ul>" >> requirements.txt ; \
	start='## Libraries Used' ; \
	end='<\/ul>' ; \
	replacement='## REQUIREMENTS' ; \
	mkdir -p .build ; \
	if [ -f README.md ]; then \
		cat README.md | sed -e "/^ *$$start/,/^ *$$end/c$$replacement" | sed -e "/$$replacement/{r requirements.txt" -e 'd}' > .build/README.md ; \
		diff README.md .build/README.md > /dev/null ; \
		if [ $$? -eq 1 ]; then  \
			cp -f .build/README.md README.md ; \
		fi \
	fi

github_clone:
	@for dir in $(GITHUB_EMPTYDIRS) ; do \
		echo Cloning : https://github.com/$$dir ; \
		git clone https://github.com/$$dir $(GITHUB_LIBPATH)/$$dir ; \
	done

github_pull:
	@for dir in $(GITHUB_POPULATEDIRS) ; do \
		echo Refresh : $(GITHUB_LIBPATH)/$$dir ; \
		cd $(GITHUB_LIBPATH)/$$dir && git pull ; \
	done

ifneq ("$(ESP32_UPLOAD)", "")
.build/$(SKETCH).spiffs.bin: $(ESP32_DATADEPEND) $(wildcard $(DATA)/*)
	@mkdir -p .build
	@if [ -d "$(PWD)/$(ESP32_DATA)" ]; \
	 then \
		echo Packaging SPIFFS file system ; \
		echo $(ARDUINO_PACKAGES)/esp32/tools/mkspiffs/0.2.3/mkspiffs -c $(PWD)/$(ESP32_DATA) -p 256 -b 4096 -s $(FILESYSTEM_SIZE) .build/$(SKETCH).spiffs.bin ; \
		$(ARDUINO_PACKAGES)/esp32/tools/mkspiffs/0.2.3/mkspiffs -c $(PWD)/$(ESP32_DATA) -p 256 -b 4096 -s $(FILESYSTEM_SIZE) .build/$(SKETCH).spiffs.bin ; \
	 fi

.build/$(SKETCH).ffat.bin: $(ESP32_DATADEPEND) $(wildcard $(DATA)/*)
	@mkdir -p .build
	@if [ -d "$(PWD)/$(ESP32_DATA)" ]; \
	 then \
		echo Packaging FatFS file system ; \
		$(ESP32_HARDWARE)/tools/mkfatfs -c $(PWD)/$(ESP32_DATA) -t fatfs -s $(FILESYSTEM_SIZE) .build/$(SKETCH).$(ESP32_FILESYSTEM).bin ; \
	 fi

.build/$(SKETCH).littlefs.bin: $(ESP32_DATADEPEND) $(wildcard $(DATA)/*)
	@mkdir -p .build
	@if [ -d "$(PWD)/$(ESP32_DATA)" ]; \
	 then \
		echo Packaging LittleFS file system ; \
		echo $(ESP32_PARTFILE) ; \
		echo $(ARDUINO_PACKAGES)/esp32/tools/mklittlefs/3.0.0-gnu12-dc7f933/mklittlefs -c $(PWD)/$(ESP32_DATA) -s $(FILESYSTEM_SIZE) .build/$(SKETCH).$(ESP32_FILESYSTEM).bin ; \
		$(ARDUINO_PACKAGES)/esp32/tools/mklittlefs/3.0.0-gnu12-dc7f933/mklittlefs -c $(PWD)/$(ESP32_DATA) -s $(FILESYSTEM_SIZE) .build/$(SKETCH).$(ESP32_FILESYSTEM).bin ; \
	 fi

data: .build/$(SKETCH).$(ESP32_FILESYSTEM).bin
else
data:
endif

.build/$(SKETCH).$(ARDUINO_HEX): $(SKETCH).ino $(DEPENDS)
	@mkdir -p .build
	@echo $(BUILDER_CMD) -build-path $(PWD)/.build $(SKETCH).ino
	@$(BUILDER_CMD) -build-path $(PWD)/.build $(SKETCH).ino

build: github_clone data $(BUILD_VERSION_FILE) $(REELTWO_BUILD_VERSION_FILE) requirements.txt .build/$(SKETCH).$(ARDUINO_HEX)

ifneq ("$(ESP32_UPLOAD)", "")
.build/$(SKETCH).$(ESP32_FILESYSTEM).bin.flashed: data
	@if [ -d $(PWD)/$(ESP32_DATA) ]; \
	then \
		echo "\nUploading $(ESP32_FILESYSTEM) $(HOSTNAME)" ;\
		echo $(ESP32_UPLOAD) --chip $(UPLOAD_DEVICE) --port $(PORT) --baud $(BAUDRATE) $(ESP32_UPLOAD_OPTIONS) $(FILESYSTEM_START) .build/$(SKETCH).$(ESP32_FILESYSTEM).bin ; \
		$(ESP32_UPLOAD) --chip $(UPLOAD_DEVICE) --port $(PORT) --baud $(BAUDRATE) $(ESP32_UPLOAD_OPTIONS) $(FILESYSTEM_START) .build/$(SKETCH).$(ESP32_FILESYSTEM).bin ; \
	fi
	@touch .build/$(SKETCH).$(ESP32_FILESYSTEM).bin.flashed
upload_data: .build/$(SKETCH).$(ESP32_FILESYSTEM).bin.flashed
else
upload_data:
endif

firmware: build 
	@echo $(ESP32_UPLOAD) --chip $(UPLOAD_DEVICE) merge_bin -o .build/$(ESP32_FIRMWARE_NAME)-$(GITHASH).bin $(ESP32_FLASH_OPTIONS) $(ESP32_UPLOAD_BOOTLOADER) .build/$(SKETCH).$(ARDUINO_HEX) 0x8000 .build/$(SKETCH).ino.partitions.bin
	@$(ESP32_UPLOAD) --chip $(UPLOAD_DEVICE) merge_bin -o .build/$(ESP32_FIRMWARE_NAME)-$(GITHASH).bin $(ESP32_FLASH_OPTIONS) $(ESP32_UPLOAD_BOOTLOADER) .build/$(SKETCH).$(ARDUINO_HEX) 0x8000 .build/$(SKETCH).ino.partitions.bin

firmware_data: build 
	@echo $(ESP32_UPLOAD) --chip $(UPLOAD_DEVICE) merge_bin -o .build/$(ESP32_FIRMWARE_NAME)-$(GITHASH).bin $(ESP32_FLASH_OPTIONS) $(ESP32_UPLOAD_BOOTLOADER) .build/$(SKETCH).$(ARDUINO_HEX) 0x8000 .build/$(SKETCH).ino.partitions.bin $(FILESYSTEM_START) .build/$(SKETCH).$(ESP32_FILESYSTEM).bin
	@$(ESP32_UPLOAD) --chip $(UPLOAD_DEVICE) merge_bin -o .build/$(ESP32_FIRMWARE_NAME)-$(GITHASH).bin $(ESP32_FLASH_OPTIONS) $(ESP32_UPLOAD_BOOTLOADER) .build/$(SKETCH).$(ARDUINO_HEX) 0x8000 .build/$(SKETCH).ino.partitions.bin $(FILESYSTEM_START) .build/$(SKETCH).$(ESP32_FILESYSTEM).bin

upload: upload_data
ifneq ("$(SSH_UPLOAD_HOST)", "")
	@echo "\nUploading to $(SSH_UPLOAD_HOST)"
	scp .build/$(SKETCH).$(ARDUINO_HEX) $(SSH_UPLOAD_USER)@$(SSH_UPLOAD_HOST):roms
	ssh $(SSH_UPLOAD_USER)@$(SSH_UPLOAD_HOST) roms/flash.sh $(SKETCH) $(shell strings .build/$(SKETCH).ino.elf | grep -m 1 ReelTwoSMQ.h 2> /dev/null)
	@echo
else ifneq ("$(ESP32_UPLOAD)", "")
	@echo "\nUploading on $(HOSTNAME)"
	@echo $(ESP32_UPLOAD) --chip $(UPLOAD_DEVICE) --port $(PORT) --baud $(BAUDRATE) $(ESP32_UPLOAD_OPTIONS) $(ESP32_UPLOAD_BOOTLOADER) .build/$(SKETCH).$(ARDUINO_HEX) 0x8000 .build/$(SKETCH).ino.partitions.bin
	@$(ESP32_UPLOAD) --chip $(UPLOAD_DEVICE) --port $(PORT) --baud $(BAUDRATE) $(ESP32_UPLOAD_OPTIONS) $(ESP32_UPLOAD_BOOTLOADER) .build/$(SKETCH).$(ARDUINO_HEX) 0x8000 .build/$(SKETCH).ino.partitions.bin
	@echo
else ifeq ("$(AVRDUDE_PROGRAMMER)", "-cusbtiny")
	@echo "\nUploading on $(HOSTNAME)"
	$(AVRDUDECMD) -p$(UPLOAD_DEVICE) $(AVRDUDE_PROGRAMMER) $(AVRDUDE_OPTS) -U flash:w:.build/$(SKETCH).$(ARDUINO_HEX):i
	@echo
else
	@echo "\nUploading on $(HOSTNAME)"
	$(AVRDUDECMD) -p$(UPLOAD_DEVICE) $(AVRDUDE_PROGRAMMER) $(AVRDUDE_OPTS) -P$(PORT) -b$(BAUDRATE) -D -U flash:w:.build/$(SKETCH).$(ARDUINO_HEX):i
	@echo
endif

run:
	$(AVRSIM) -m $(UPLOAD_DEVICE) .build/$(SKETCH).ino.elf

debug:
	@echo "\nStart avr-gdb process:\n"
	@echo $(ARDUINO_TOOLS)/avr/bin/avr-gdb $(PWD)/.build/$(SKETCH).ino.elf -ex '"target remote localhost:1234"'
	@echo
	@$(AVRSIM) -m $(UPLOAD_DEVICE) -g .build/$(SKETCH).ino.elf

clean:
	@rm -rf $(PWD)/.build $(PWD)/.lib requirements.txt $(BUILD_VERSION_FILE)

clean_data:
	@rm -f .build/$(SKETCH).$(ESP32_FILESYSTEM).bin .build/$(SKETCH).$(ESP32_FILESYSTEM).bin.flashed
