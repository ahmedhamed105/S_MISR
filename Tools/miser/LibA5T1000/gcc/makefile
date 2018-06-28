###################################################
# makefile for GNU Make V3.8
# !201507 Copy from T1000
###################################################
COMPILER_DIR=C:\DevTools\gcc_v49_2014q4
TOOLS_DIR=..\..\tools
#Ignore Uncalled Func (Reduce Code Size): 0(no ignore); 1(ingore)
IGNORE_UNCALLED_FUNC=1
WARNING= -Wall -Wno-main -Wno-unused-value -Wno-pointer-sign -Werror-implicit-function-declaration -Wno-format

###################################################
# MEMORY - currently support APP_DLL, MFG type ONLY!
###################################################
ifeq ($(APP_TYPE), MFG)
### MFG Type
	START_OF_FLASH=0xE0270000
	START_OF_SRAM=0xE13D4000
	SIZE_OF_SRAM=128k
	SIZE_OF_HEAP=16k
	APP_MFG=1
else
### APP_DLL Type
	START_OF_FLASH=0x01000000
	START_OF_SRAM=0x01800000
	SIZE_OF_SRAM=4096k
	SIZE_OF_HEAP=16k
	APP_MFG=0
endif

###################################################
# Source file directory
###################################################
OBJDIR=obj
SRCDIR=src
INCDIR=src
LSTDIR=lst
COMMON_SRCDIR=$(BUILD_DIR)/gcc
COMMON_INCDIR=../LibProj

###################################################
# Source file list
###################################################
C_SRC=$(sort $(notdir $(wildcard $(SRCDIR)/*.c)))
A_SRC=$(sort $(notdir $(wildcard $(SRCDIR)/*.s)))
COMMON_C_SRC=$(sort $(notdir $(wildcard $(COMMON_SRCDIR)/*.c)))

###################################################
# Gen object file name automatically
###################################################
C_OBJ=$(sort $(addprefix $(OBJDIR)/, $(C_SRC:.c=.o)))
A_OBJ=$(sort $(addprefix $(OBJDIR)/, $(A_SRC:.s=.o)))
COMMON_C_OBJ=$(sort $(addprefix $(OBJDIR)/, $(COMMON_C_SRC:.c=.o)))

OUT_FILE=$(PRJ_NAME)
BIN_FILE=$(PRJ_NAME).bin
TMS_FILE=$(PRJ_NAME).tms
MAP_FILE=$(PRJ_NAME).map
SCRIPT_FILE=$(PRJ_NAME).ld

###################################################
# Tools setup
###################################################
CROSS=arm-none-eabi
LIBCDIR=$(COMPILER_DIR)/$(CROSS)/lib/thumb -L $(COMPILER_DIR)/lib/gcc/$(CROSS)/4.7.2/thumb 
INCLUDES=-I $(INCDIR) -I $(COMMON_INCDIR) -I $(BUILD_DIR) -I $(BUILD_DIR)/gcc -I $(COMPILER_DIR)/$(CROSS)/include
DEFINES=-D__NO_CTYPE -DSVN_REV=$(SVN_REC)

# MCU name and submodel
MCU      = -mcpu=cortex-a5
THUMB    = -mthumb
THUMB_IW = -mthumb-interwork
CFLAGS_OPT = -mstructure-size-boundary=32 -mno-unaligned-access
#Set Optimization: -Os for space(default); -O3 for speed
ifeq ($(SET_OPT), )
	CFLAGS_OPT += -Os
else
	CFLAGS_OPT += $(SET_OPT)
endif
#Remove uncalled functions [-ffunction-sections]
ifeq ($(IGNORE_UNCALLED_FUNC), 1)
	CFLAGS_OPT += -ffunction-sections
endif

CC=$(CROSS)-gcc
CFLAGS=-c -g $(THUMB) $(MCU) $(THUMB_IW) $(CFLAGS_OPT) $(WARNING) -fsigned-char $(INCLUDES) -D__pcs= -D$(MODEL_NAME)=1 -DAPP_MFG=$(APP_MFG) $(BUILD_DEF)

ASM=$(CROSS)-gcc
ASFLAGS=-D__ASSEMBLY__ -g $(THUMB) $(MCU) $(THUMB_IW) $(CFLAGS_OPT) -c -Wall $(INCLUDES) -D__pcs=

LINK=$(CROSS)-gcc
LFLAGS_OPT =
#Remove uncalled functions [-Wl,--gc-sections]
ifeq ($(IGNORE_UNCALLED_FUNC), 1)
	LFLAGS_OPT += -Wl,--gc-sections
endif
LFLAGS=$(LFLAGS_OPT) -nostartfiles -nodefaultlibs -nostdlib -Wl,-Map=$(MAP_FILE),--cref -L $(LIBCDIR) -L $(BUILD_DIR)/gcc -static -T $(SCRIPT_FILE)
LIBS = -lsyscall
#include optional libraries if exist
ifeq ($(notdir $(wildcard $(COMMON_SRCDIR)/libbasecall.a)), libbasecall.a)
	LIBS += -lbasecall
endif
ifeq ($(notdir $(wildcard $(COMMON_SRCDIR)/libsptutil.a)), libsptutil.a)
	LIBS += -lsptutil
endif
ifeq ($(notdir $(wildcard $(COMMON_SRCDIR)/libspglib.a)), libspglib.a)
	LIBS += -lspglib
endif
ifeq ($(notdir $(wildcard $(COMMON_SRCDIR)/libspqr.a)), libspqr.a)
	LIBS += -lspqr
endif
LIBS += -lc -lnosys -lm -lgcc

AR=$(CROSS)-ar
ARFLAGS=-rc

OBJCOPY=$(CROSS)-objcopy
#use hex for debug
#OCFLAGS=-S -O ihex
OCFLAGS=-S -O binary

#
BIN2TMS=$(TOOLS_DIR)/bin2tms
TMSFLAGS=-mA5-T1000 -j 

LOAD_APP=$(TOOLS_DIR)\Load.bat

############################################################################
# Extra objects
############################################################################
ifeq ($(WITH_EXTRA), 1)
	include extra_gcc.mk
endif

############################################################################
# Linker script
############################################################################
APP_OFFSET=0x100

SIGNBIN=echo No Sign Applied .. 
ifeq ($(SIGN), 1)
## Normal sign (nor)
	SIGNBIN=$(TOOLS_DIR)/NorSign.bat $(BIN_FILE)
else ifeq ($(SIGN), 2)
## Security Application sign (app)
	SIGNBIN=$(TOOLS_DIR)/AppSign.bat $(BIN_FILE)
else ifeq ($(SIGN), 3)
## System sign (sys)
	SIGNBIN=$(TOOLS_DIR)/SysSign.bat $(BIN_FILE)
endif

#Linker - currently removed little-end defined
LD_SCRIPT="OUTPUT_ARCH(arm) \
			ENTRY (cstartup) \
			SECTIONS {	\
				_start_of_flash = $(START_OF_FLASH); \
				_start_of_data = $(START_OF_SRAM); \
				_end_of_sram = _start_of_data + $(SIZE_OF_SRAM); \
				_start_of_header = _start_of_flash + 0x20; \
				_start_of_app = _start_of_flash + 0x50; \
				. = _start_of_flash; \
				.startup : { KEEP(*(sect_startup)) } \
				. = _start_of_header; \
				.header : {	KEEP(*(sect_apphdr)) } \
				. = _start_of_app + $(APP_OFFSET); \
				.text : ALIGN(4) { \
       				KEEP(*(.text* .glue_7* .gnu.linkonce.t.*)) \
			        KEEP(*(.init)) \
			        KEEP(*(.fini)) \
					*crtbegin.o(.ctors) \
					*crtbegin?.o(.ctors) \
					*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors) \
					*(SORT(.ctors.*)) \
					*(.ctors) \
 					*crtbegin.o(.dtors) \
 					*crtbegin?.o(.dtors) \
 					*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors) \
 					*(SORT(.dtors.*)) \
 					*(.dtors) \
       				KEEP(*(.rodata* .gnu.linkonce.r.*)) \
					KEEP(*(.eh_frame*)) \
	             } \
				.ARM.extab : ALIGN(4) {	*(.ARM.extab* .gnu.linkonce.armextab.*) } \
				__exidx_start = .;	\
				.ARM.exidx : ALIGN(4) { *(.ARM.exidx* .gnu.linkonce.armexidx.*) } \
				__exidx_end = .; \
				.text_kek : ALIGN(4) { KEEP(*(sect_kek)) } \
				_end_of_app = ALIGN(4); \
				. = _start_of_data; \
				.data : AT (_end_of_app) { \
					KEEP(*(.data*)) \
		 	        . = ALIGN(4); \
			        __preinit_array_start = .; \
			        KEEP (*(.preinit_array)) \
			        __preinit_array_end = .; \
			        . = ALIGN(4); \
			        __init_array_start = .; \
			        KEEP (*(SORT(.init_array.*))) \
			        KEEP (*(.init_array)) \
			        __init_array_end = .; \
			        . = ALIGN(4); \
			        __fini_array_start = .; \
			        KEEP (*(.fini_array)) \
			        KEEP (*(SORT(.fini_array.*))) \
			        __fini_array_end = .; \
				    _end_of_data = ALIGN(4); \
				} \
				. = _end_of_data; \
				_start_of_bss = .; \
				.bss : { \
					*(.bss*) \
					*(COMMON) \
				} \
				_end = ALIGN(4); \
				_end_of_bss = (_end + $(SIZE_OF_HEAP)); \
				_end_of_flash = _end_of_app + SIZEOF(.data); \
			}"

###################################################
# Compile C files
###################################################
$(OBJDIR)/%.o : $(SRCDIR)/%.c
	@echo [$<]
	@$(CC) $(CFLAGS) $< -o $@ -Wa,-a=$(LSTDIR)/$(@F:.o=.lst)

###################################################
# Assembly ASM files
###################################################
$(OBJDIR)/%.o : $(SRCDIR)/%.s
	@echo [$<]
	@$(ASM) $(ASFLAGS) $< -o $@ -Wa,-a=$(LSTDIR)/$(@F:.o=.lst)
	
###################################################
# Compile Common C files
###################################################
$(OBJDIR)/%.o : $(COMMON_SRCDIR)/%.c
	@echo [$<]
	@$(CC) $(CFLAGS) $< -o $@ -Wa,-a=$(LSTDIR)/$(@F:.o=.lst)
	

###################################################
# initial environment
###################################################
init_env :
	@echo .
	@echo [******** Ready Env ********]
ifeq ($(wildcard $(OBJDIR)),)
	mkdir $(OBJDIR)
endif
ifeq ($(wildcard $(LSTDIR)),)
	mkdir $(LSTDIR)
endif
ifneq ($(wildcard $(SCRIPT_FILE)),)
	@del /F $(SCRIPT_FILE)
endif
	@del /F $(PROJ_NAME).*
	@echo .
	@echo [********* Compile *********]

###################################################
# Create binary file
###################################################
$(BIN_FILE) : $(A_OBJ) $(C_OBJ) $(COMMON_C_OBJ) $(EXTRA_OBJ)
	@echo .
	@echo [******** Link  $(OUT_FILE) ********]
	@echo_plus -n $(LD_SCRIPT) > $(SCRIPT_FILE)
	@$(LINK) $(LFLAGS) -o $(OUT_FILE) -Wl,--start-group $(A_OBJ) $(C_OBJ) $(COMMON_C_OBJ) $(EXTRA_OBJ) $(LIBS) -Wl,--end-group
	$(OBJCOPY) $(OCFLAGS) $(OUT_FILE) $(BIN_FILE)

###################################################
# Create TMS file
###################################################
$(TMS_FILE) : $(BIN_FILE)
	@echo .
	@echo [****** Sign  $(BIN_FILE) ******]
	@call $(SIGNBIN)
	@$(BIN2TMS) $(TMSFLAGS) $(BIN_FILE)

###################################################
# Start here!!
###################################################
.PHONY: build
build :	init_env $(BIN_FILE) $(TMS_FILE)
	@echo .
	@echo [***** Build Completed *****]

###################################################
# Download Load Application
##################################################
.PHONY: load
load:
	@echo .
	@echo [****** Download  TMS ******]
	@$(LOAD_APP) $(TMS_FILE) $(TOOLS_DIR)

###################################################
# Clear all object file and output file
###################################################
.PHONY: clean
clean :
	@echo .
	@echo [** Delete Output files **]
ifneq ($(wildcard obj),)
	@del /F /Q obj\*.*
	@rmdir obj
endif
ifneq ($(wildcard lst),)
	@del /F /Q lst\*.*
	@rmdir lst
endif
	@del /F /Q $(PRJ_NAME).*

