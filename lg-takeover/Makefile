#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
# SPECS is the directory containing the important build and link files
#---------------------------------------------------------------------------------
export TARGET		:=	$(shell basename $(CURDIR))
export BUILD		?=	debug

R_SOURCES			:=	source ../library
SOURCES				:=	

R_INCLUDES			:=	source ../library
INCLUDES 			:=

DATA				:=	data

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH			:=	-march=armv8-a -mtune=cortex-a8 -mfloat-abi=softfp -mthumb-interwork -mthumb

CFLAGS			:=	-g -std=c11 -Wall -Werror -Os -fomit-frame-pointer \
				-ffunction-sections -Wno-error=unused-function -Wno-unused-function \
				-fno-builtin-printf \
				$(ARCH) $(INCLUDE)

CFLAGS			+=	-D_GNU_SOURCE -D_FILE_OFFSET_BITS=64

CXXFLAGS		:=	$(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11

ASFLAGS			:=	-g $(ARCH) -x assembler-with-cpp
LDFLAGS			 =	-nostartfiles -g --specs=../stub.specs $(ARCH) -Wl,--gc-sections,-Map,$(TARGET).map

LIBS			:=

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS			:=

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export ROOTDIR	:=	$(CURDIR)
export OUTPUT	:=	$(CURDIR)/$(TARGET)

SOURCES         := $(SOURCES) $(foreach dir,$(R_SOURCES), $(dir) $(filter %/, $(wildcard $(dir)/*/)))
INCLUDES        := $(INCLUDES) $(foreach dir,$(R_INCLUDES), $(dir) $(filter %/, $(wildcard $(dir)/*/)))

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

EXCLUSIONS	:=	ccplex

SOURCE_LIST	 =	$(filter-out $(foreach exc,$(EXCLUSIONS),$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.$(exc).$(1))))), $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.$(1)))))

CFILES		:=	$(call SOURCE_LIST,c)
CPPFILES	:=	$(call SOURCE_LIST,cpp)
SFILES		:=	$(call SOURCE_LIST,s)
SCCFILES	:=	$(call SOURCE_LIST,S)
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o) $(SCCFILES:.S=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean all

#---------------------------------------------------------------------------------
all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(OUTPUT).elf $(OUTPUT).bin $(BUILD) payload.bin payload-alt.bin aboot-payload.bin


#---------------------------------------------------------------------------------
else

DEPENDS		:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
payload.bin: $(OUTPUT).bin
	cd $(ROOTDIR); armips $(ROOTDIR)/sbl_rop.s
	cd $(ROOTDIR); armips $(ROOTDIR)/sbl_rop_alt.s

$(OUTPUT).bin	:	$(OUTPUT).elf
$(OUTPUT).elf	:	aboot-payload.c $(OFILES)
aboot-payload.c		:	aboot-payload.bin

aboot-payload.bin:
	cd $(ROOTDIR); armips aboot_rop.s #&& python3 ../lg-tools/lg-restoc.py aboot-payload.bin source/aboot-payload.c

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.bin: %.elf
	@$(OBJCOPY) -O binary $< $@
	@echo built ... $(notdir $@)


-include $(DEPENDS)


#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
