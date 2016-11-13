###
# GNU ARM Embedded Toolchain
CC=arm-none-eabi-gcc
CXX=arm-none-eabi-g++
LD=arm-none-eabi-ld
AR=arm-none-eabi-ar
AS=arm-none-eabi-as
CP=arm-none-eabi-objcopy
OD=arm-none-eabi-objdump
NM=arm-none-eabi-nm
SIZE=arm-none-eabi-size
A2L=arm-none-eabi-addr2line

###
# Directory Structure
BINDIR=bin

###
# Find source files
ASOURCES=$(shell find . -name '*.s')
CSOURCES=$(shell find . -name '*.c')
CXXSOURCES=$(shell find . -name '*.cpp')
# Find header directories
INC=$(shell find . -name '*.h' -exec dirname {} \; | uniq)
INCLUDES=$(INC:%=-I%)
# Find libraries
INCLUDES_LIBS=libarm_cortexM4lf_math.a
LINK_LIBS=
# Create object list
OBJECTS=$(ASOURCES:%.s=%.o)
OBJECTS+=$(CSOURCES:%.c=%.o)
OBJECTS+=$(CXXSOURCES:%.cpp=%.o)
# Define output files ELF & IHEX
BINELF=outp.elf
BINHEX=outp.hex

###
# MCU FLAGS
MCFLAGS=-mcpu=cortex-m4 -mthumb -mlittle-endian \
-mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb-interwork
# COMPILE FLAGS
DEFS=-DUSE_STDPERIPH_DRIVER -DSTM32F4XX -DSTM32F407VG
CFLAGS=-c $(MCFLAGS) $(DEFS) $(INCLUDES)
CXXFLAGS=-c $(MCFLAGS) $(DEFS) $(INCLUDES)
# LINKER FLAGS
LDSCRIPT=stm32f4xx_link.ld
LDFLAGS =-T $(LDSCRIPT) $(MCFLAGS) --specs=nosys.specs $(INCLUDES_LIBS) $(LINK_LIBS)

###
# Build Rules
.PHONY: all release release-memopt debug clean

all: release-memopt

release-memopt: DEFS+=-DCUSTOM_NEW -DNO_EXCEPTIONS
release-memopt: CFLAGS+=-Os -ffunction-sections -fdata-sections -fno-builtin
release-memopt: CXXFLAGS+=-Os -fno-exceptions -ffunction-sections -fdata-sections -fno-builtin -fno-rtti
release-memopt: LDFLAGS+=-Os --specs=nano.specs
release-memopt: release

debug: CFLAGS+=-g
debug: CXXFLAGS+=-g
debug: LDFLAGS+=-g
debug: release

release: $(BINDIR)/$(BINHEX)

$(BINDIR)/$(BINHEX): $(BINDIR)/$(BINELF)
	$(CP) -O ihex $< $@
	@echo "Objcopy from ELF to IHEX complete!\n"

$(BINDIR)/$(BINELF): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@
	@echo "Linking complete!\n"
	$(SIZE) $(BINDIR)/$(BINELF)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
	@echo "Compiled "$<"!\n"

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
	@echo "Compiled "$<"!\n"

%.o: %.s
	$(CC) $(CFLAGS) $< -o $@
	@echo "Assambled "$<"!\n"

clean:
	rm -f $(OBJECTS) $(BINDIR)/$(BINELF) $(BINDIR)/$(BINHEX) $(BINDIR)/output.map

deploy:
ifneq ($(wildcard /usr/bin/openocd),)
	/usr/bin/openocd -f /usr/share/openocd/scripts/board/stm32f4discovery.cfg -c "program bin/$(BINELF) verify reset exit"
endif

ifneq ($(wildcard /usr/local/bin/openocd),)
	/usr/local/bin/openocd -f /usr/local/share/openocd/scripts/board/stm32f4discovery.cfg -c "program bin/$(BINELF) verify reset exit"
endif

ifneq ($(wildcard /opt/openocd/bin/openocd),)
	/opt/openocd/bin/openocd -f /opt/openocd/share/openocd/scripts/board/stm32f4discovery.cfg -c "program bin/$(BINELF) verify reset exit"
endif

