#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/3ds_rules

export CITRO3D_MAJOR	:= 1
export CITRO3D_MINOR	:= 6
export CITRO3D_PATCH	:= 0

VERSION	:=	$(CITRO3D_MAJOR).$(CITRO3D_MINOR).$(CITRO3D_PATCH)

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
#---------------------------------------------------------------------------------
TARGET		:=	citro3d
SOURCES		:=	source source/maths
DATA		:=	data
INCLUDES	:=	include

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

CFLAGS	:=	-g -Wall -Werror -mword-relocations \
			-ffunction-sections -fdata-sections \
			$(ARCH) $(BUILD_CFLAGS)

CFLAGS	+=	$(INCLUDE) -DARM11 -D_3DS -DCITRO3D_BUILD

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11

ASFLAGS	:=	-g $(ARCH) $(DEFINES)

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(CTRULIB)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
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
			$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I.

.PHONY: clean all doc

#---------------------------------------------------------------------------------
all: lib/libcitro3d.a lib/libcitro3dd.a

doc:
	@doxygen Doxyfile

dist-bin: all
	@tar --exclude=*~ -cjf citro3d-$(VERSION).tar.bz2 include lib

dist-src:
	@tar --exclude=*~ -cjf citro3d-src-$(VERSION).tar.bz2 include source Makefile

dist: dist-src dist-bin

install: dist-bin
	mkdir -p $(DESTDIR)$(DEVKITPRO)/libctru
	bzip2 -cd citro3d-$(VERSION).tar.bz2 | tar -xf - -C $(DESTDIR)$(DEVKITPRO)/libctru

lib:
	@[ -d $@ ] || mkdir -p $@

release:
	@[ -d $@ ] || mkdir -p $@

debug:
	@[ -d $@ ] || mkdir -p $@

lib/libcitro3d.a : lib release $(SOURCES) $(INCLUDES)
	@$(MAKE) BUILD=release OUTPUT=$(CURDIR)/$@ \
	BUILD_CFLAGS="-DNDEBUG=1 -O2 -fomit-frame-pointer -fno-math-errno" \
	DEPSDIR=$(CURDIR)/release \
	--no-print-directory -C release \
	-f $(CURDIR)/Makefile

lib/libcitro3dd.a : lib debug $(SOURCES) $(INCLUDES)
	@$(MAKE) BUILD=debug OUTPUT=$(CURDIR)/$@ \
	BUILD_CFLAGS="-DDEBUG=1 -Og" \
	DEPSDIR=$(CURDIR)/debug \
	--no-print-directory -C debug \
	-f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr release debug lib

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT)	:	$(OFILES)

#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
