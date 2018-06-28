###################################################
# makefile for GNU Make V3.8
###################################################

###################################################
# Define Project Name
###################################################
MODEL_NAME=A5T1000
PRJ_NAME=app
SUBVERSION=0

###################################################
# Application Type
#   MFG(mfg), APP_DLL(other app or dll)
# Sign type - 0(none, NoSign); 1(*.nor, NorSign);
#             2(*.app, AppSign); 3(*.sys, SysSign)
# WITH_EXTRA - include extra_gcc.mk or not
###################################################
APP_TYPE=APP_DLL
SIGN=1
WITH_EXTRA=0
BUILD_DIR=../Lib$(MODEL_NAME)

###################################################
###################################################
.PHONY: all
all:
	@make build -s -f $(BUILD_DIR)/gcc/makefile \
			MODEL_NAME=$(MODEL_NAME) \
			BUILD_DIR=$(BUILD_DIR) \
			PRJ_NAME=$(PRJ_NAME) \
			SUBVERSION=$(SUBVERSION) \
			APP_TYPE=$(APP_TYPE) \
			SIGN=$(SIGN) WITH_EXTRA=$(WITH_EXTRA)

###################################################
# Universal Loader parameter
###################################################
COMPORT=
LOADKEY=
AID=

###################################################
# Load TMS to unit
###################################################
.PHONY: load
load :
	@make load -s -f $(BUILD_DIR)/gcc/makefile \
			PRJ_NAME=$(PRJ_NAME) 
	
###################################################
# Clear all object file and output file
###################################################
.PHONY: clean
clean :
	@echo .
	@echo [* Delete all output files *]
	@del /F /Q obj\*.o
	@del /F /Q lst\*.lst
	@del /F /Q $(PRJ_NAME)
	@del /F /Q $(PRJ_NAME).a79
	@del /F /Q $(PRJ_NAME).bin
	@del /F /Q $(PRJ_NAME).tms
	@del /F /Q $(PRJ_NAME).map
	@del /F /Q 0001.tmp
	@del /F /Q /S *.bak
	@del /F /Q *.ld

