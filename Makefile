# the directory this project is in
# must be defined for generic makefile to work
export PROJECT_DIR := $(dir $(realpath ${MAKEFILE_LIST}))

# to define to ArduinoCore root directory 
CORE_DIR := ${PROJECT_DIR}../ArduinoCore/

# other arduino librairies project pathes this project depends on
#export DEPENDENCIES := ${CORE_DIR} ${CORE_DIR}../ArduinoLibs/ ${CORE_DIR}../ArduinoTools/

# generate assembler source code also
export WITH_ASSEMBLY := no

# generate eeprom image
export WITH_EEPROM := no

# print size of geretated segments 
export WITH_PRINT_SIZE := no

# only for programs : launch upload
export WITH_UPLOAD := yes

# where to upload
export UPLOAD_DEVICE := /dev/ttyACM0
export TARGET := Uno

#export MAIN_SOURCE := ${PROJECT_DIR}main.cpp

# call lib.makefile for a utilities library or bin.makefile for a program
all upload console:
	${MAKE} -f ${CORE_DIR}etc/bin.makefile $@

clean:
ifeq (${TARGET},)
	rm -rf ${PROJECT_DIR}target/*
endif
ifneq (${TARGET},)
	rm -rf ${PROJECT_DIR}target/${TARGET}
endif

alternate: export ALL_SOURCES ?= $(wildcard alternate/*.cpp)
alternate: export MAIN_SOURCE ?= alternate/firmware.ino

firmware: export ALL_SOURCES ?= $(wildcard firmware/*.cpp firmware/*.ino)
firmware: export MAIN_SOURCE ?= firmware/firmware.ino

alternate: all
firmware: console
	echo $(MAIN_SOURCE) / $(ALL_SOURCES)

.PHONY: firmware