#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

3DSTEX := 3dstex
TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
#
# NO_SMDH: if set to anything, no SMDH file is generated.
# ROMFS is the directory which contains the RomFS, relative to the Makefile (Optional)
# APP_TITLE is the name of the app stored in the SMDH file (Optional)
# APP_DESCRIPTION is the description of the app stored in the SMDH file (Optional)
# APP_AUTHOR is the author of the app stored in the SMDH file (Optional)
# ICON is the filename of the icon (.png), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.png
#     - icon.png
#     - <libctru folder>/default_icon.png
#---------------------------------------------------------------------------------
TARGET   := citro3d_test
BUILD    := build
SOURCES  := source
GRAPHICS := gfx
DATA     := data
INCLUDES := include
ROMFS    := romfs

APP_TITLE       := citro3d test
APP_DESCRIPTION := v1.0
APP_AUTHOR      := mtheall
ICON            :=

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH     := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

CFLAGS   := -g -Wall -O3 -mword-relocations \
            -ffunction-sections \
            $(ARCH)

CFLAGS   +=  $(INCLUDE) -D__3DS__

CXXFLAGS := $(CFLAGS) -fno-rtti -std=gnu++11

ASFLAGS  := -g $(ARCH)
LDFLAGS   = -specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(TARGET).map

LIBS     := -lcitro3d -lctru

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS  := $(CTRULIB) #$(CURDIR)/../..


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT  :=  $(CURDIR)/$(TARGET)
export TOPDIR  :=  $(CURDIR)

export VPATH   :=  $(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
                   $(foreach dir,$(DATA),$(CURDIR)/$(dir)) \
                   $(foreach dir,$(GRAPHICS),$(CURDIR)/$(dir))

export DEPSDIR :=  $(CURDIR)/$(BUILD)

CFILES    := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CXXFILES  := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES    := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
PICAFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.v.pica)))
BINFILES  := $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))
PNGFILES  := $(foreach dir,$(GRAPHICS),$(notdir $(wildcard $(dir)/*.png)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CXXFILES)),)
  export LD := $(CC)
else
  export LD := $(CXX)
endif
#---------------------------------------------------------------------------------

export OFILES   := $(addsuffix .o,$(BINFILES)) \
                   $(PICAFILES:.v.pica=.shbin.o) \
                   $(CXXFILES:.cpp=.o) \
                   $(CFILES:.c=.o) \
                   $(SFILES:.s=.o)

export INCLUDE  := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
                   $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                   -I$(CURDIR)/$(BUILD)

export LIBPATHS :=  $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

PNGROMFS := $(addprefix romfs/,$(PNGFILES:.png=.bin))

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.png)
	ifneq (,$(findstring $(TARGET).png,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).png
	else
		ifneq (,$(findstring icon.png,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.png
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_SMDH)),)
	export _3DSXFLAGS += --smdh=$(CURDIR)/$(TARGET).smdh
endif

ifneq ($(ROMFS),)
	export _3DSXFLAGS += --romfs=$(CURDIR)/$(ROMFS)
endif

.PHONY: $(BUILD) clean all

#---------------------------------------------------------------------------------
all: $(BUILD)

$(BUILD): $(PNGROMFS)
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).3dsx $(OUTPUT).smdh $(TARGET).elf $(PNGROMFS)

$(ROMFS)/%.rgba.bin: %.rgba.png
	@$(3DSTEX) -o $@ --rgba $<

$(ROMFS)/%.rgb.bin: %.rgb.png
	@$(3DSTEX) -o $@ --rgb $<

$(ROMFS)/%.rgba5551.bin: %.rgba5551.png
	@$(3DSTEX) -o $@ --rgba5551 $<

$(ROMFS)/%.rgb565.bin: %.rgb565.png
	@$(3DSTEX) -o $@ --rgb565 $<

$(ROMFS)/%.rgba4.bin: %.rgba4.png
	@$(3DSTEX) -o $@ --rgba4 $<

$(ROMFS)/%.la.bin: %.la.png
	@$(3DSTEX) -o $@ --la $<

$(ROMFS)/%.hilo.bin: %.hilo.png
	@$(3DSTEX) -o $@ --hilo $<

$(ROMFS)/%.l.bin: %.l.png
	@$(3DSTEX) -o $@ --l $<

$(ROMFS)/%.a.bin: %.a.png
	@$(3DSTEX) -o $@ --a $<

$(ROMFS)/%.la4.bin: %.la4.png
	@$(3DSTEX) -o $@ --la4 $<

$(ROMFS)/%.l4.bin: %.l4.png
	@$(3DSTEX) -o $@ --l4 $<

$(ROMFS)/%.a4.bin: %.a4.png
	@$(3DSTEX) -o $@ --a4 $<

$(ROMFS)/%.etc1.bin: %.etc1.png
	@$(3DSTEX) -o $@ --etc1 $<

$(ROMFS)/%.etc1a4.bin: %.etc1a4.png
	@$(3DSTEX) -o $@ --etc1a4 $<

$(ROMFS)/%.bin: %.png
	@$(3DSTEX) -o $@ $<

#---------------------------------------------------------------------------------
else

DEPENDS := $(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
ifeq ($(strip $(NO_SMDH)),)
.PHONY: all
all	:	$(OUTPUT).3dsx $(OUTPUT).smdh
$(OUTPUT).smdh : $(TOPDIR)/Makefile
$(OUTPUT).3dsx: $(OUTPUT).smdh
endif
$(OUTPUT).3dsx: $(OUTPUT).elf
$(OUTPUT).elf:  $(OFILES)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o: %.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
# rules for assembling GPU shaders
#---------------------------------------------------------------------------------
define shader-as
	$(eval CURBIN := $(patsubst %.shbin.o,%.shbin,$(notdir $@)))
	picasso -o $(CURBIN) $1
	bin2s $(CURBIN) | $(AS) -o $@
	echo "extern const u8" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"_end[];" > `(echo $(CURBIN) | tr . _)`.h
	echo "extern const u8" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"[];" >> `(echo $(CURBIN) | tr . _)`.h
	echo "extern const u32" `(echo $(CURBIN) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`_size";" >> `(echo $(CURBIN) | tr . _)`.h
endef

%.shbin.o : %.v.pica %.g.pica
	@echo $(notdir $^)
	@$(call shader-as,$^)

%.shbin.o : %.v.pica
	@echo $(notdir $<)
	@$(call shader-as,$<)

%.shbin.o : %.shlist
	@echo $(notdir $<)
	@$(call shader-as,$(foreach file,$(shell cat $<),$(dir $<)/$(file)))

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
