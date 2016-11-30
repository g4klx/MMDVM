#  Copyright (C) 2016 by Andy Uribe CA6JAU
#  Copyright (C) 2016 by Jim McLaughlin KI6ZUM

#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.

#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.

#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

# Directory Structure
BINDIR=bin

# Find source files
# "SystemRoot" is only defined in Windows
ifdef SYSTEMROOT
	ASOURCES=$(shell dir /S /B *.s)
	CSOURCES=$(shell dir /S /B *.c)
	CXXSOURCES=$(shell dir /S /B *.cpp)
	CLEANCMD=del /S *.o *.hex *.bin *.elf
	MDBIN=md $@
else ifdef SystemRoot
	ASOURCES=$(shell dir /S /B *.s)
	CSOURCES=$(shell dir /S /B *.c)
	CXXSOURCES=$(shell dir /S /B *.cpp)
	CLEANCMD=del /S *.o *.hex *.bin *.elf
	MDBIN=md $@
else
	ASOURCES=$(shell find . -name '*.s')
	CSOURCES=$(shell find . -name '*.c')
	CXXSOURCES=$(shell find . -name '*.cpp')
	CLEANCMD=rm -f $(OBJECTS) $(BINDIR)/$(BINELF) $(BINDIR)/$(BINHEX) $(BINDIR)/$(BINBIN)
	MDBIN=mkdir $@
endif

# Default reference oscillator frequencies
ifndef $(OSC)
	ifeq ($(MAKECMDGOALS),pi)
		OSC=12000000
	else
		OSC=8000000
	endif
endif

# Find header directories
INC= . STM32F4XX_Lib/CMSIS/Include/ STM32F4XX_Lib/Device/ STM32F4XX_Lib/STM32F4xx_StdPeriph_Driver/include/
INCLUDES=$(INC:%=-I%)

# Find libraries
INCLUDES_LIBS=STM32F4XX_Lib/CMSIS/Lib/GCC/libarm_cortexM4lf_math.a
LINK_LIBS=

# Create object list
OBJECTS=$(ASOURCES:%.s=%.o)
OBJECTS+=$(CSOURCES:%.c=%.o)
OBJECTS+=$(CXXSOURCES:%.cpp=%.o)

# Define output files ELF & IHEX
BINELF=outp.elf
BINHEX=outp.hex
BINBIN=outp.bin

# MCU FLAGS
MCFLAGS=-mcpu=cortex-m4 -mthumb -mlittle-endian \
-mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb-interwork

# COMPILE FLAGS
# STM32F4 Discovery board:
DEFS_DIS=-DUSE_STDPERIPH_DRIVER -DSTM32F4XX -DSTM32F40_41xxx -DSTM32F4_DISCOVERY -DHSE_VALUE=$(OSC)
# Pi board:
DEFS_PI=-DUSE_STDPERIPH_DRIVER -DSTM32F4XX -DSTM32F446xx -DSTM32F4_PI -DHSE_VALUE=$(OSC)
CFLAGS=-c $(MCFLAGS) $(INCLUDES)
CXXFLAGS=-c $(MCFLAGS) $(INCLUDES)

# LINKER FLAGS
LDSCRIPT=stm32f4xx_link.ld
LDFLAGS =-T $(LDSCRIPT) $(MCFLAGS) --specs=nosys.specs $(INCLUDES_LIBS) $(LINK_LIBS)

# Build Rules
.PHONY: all release dis pi debug clean

# Default target: STM32F4 Discovery board
all: dis

pi: CFLAGS+=$(DEFS_PI) -Os -ffunction-sections -fdata-sections -fno-builtin -Wno-implicit -DCUSTOM_NEW -DNO_EXCEPTIONS
pi: CXXFLAGS+=$(DEFS_PI) -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-builtin -fno-rtti -DCUSTOM_NEW -DNO_EXCEPTIONS
pi: LDFLAGS+=-Os --specs=nano.specs
pi: release

dis: CFLAGS+=$(DEFS_DIS) -Os -ffunction-sections -fdata-sections -fno-builtin -Wno-implicit -DCUSTOM_NEW -DNO_EXCEPTIONS
dis: CXXFLAGS+=$(DEFS_DIS) -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-builtin -fno-rtti -DCUSTOM_NEW -DNO_EXCEPTIONS
dis: LDFLAGS+=-Os --specs=nano.specs
dis: release

debug: CFLAGS+=-g
debug: CXXFLAGS+=-g
debug: LDFLAGS+=-g
debug: release

release: $(BINDIR)
release: $(BINDIR)/$(BINHEX)
release: $(BINDIR)/$(BINBIN)

$(BINDIR):
	$(MDBIN)

$(BINDIR)/$(BINHEX): $(BINDIR)/$(BINELF)
	$(CP) -O ihex $< $@
	@echo "Objcopy from ELF to IHEX complete!\n"
	
$(BINDIR)/$(BINBIN): $(BINDIR)/$(BINELF)
	$(CP) -O binary $< $@
	@echo "Objcopy from ELF to BINARY complete!\n"

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
	$(CLEANCMD)
	
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
