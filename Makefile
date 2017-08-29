#  Copyright (C) 2016,2017 by Andy Uribe CA6JAU
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
	CLEANCMD=del /S *.o *.hex *.bin *.elf GitVersion.h
	MDBIN=md $@
else ifdef SystemRoot
	ASOURCES=$(shell dir /S /B *.s)
	CSOURCES=$(shell dir /S /B *.c)
	CXXSOURCES=$(shell dir /S /B *.cpp)
	CLEANCMD=del /S *.o *.hex *.bin *.elf GitVersion.h
	MDBIN=md $@
else
	ASOURCES=$(shell find . -name '*.s')
	CSOURCES=$(shell find . -name '*.c')
	CXXSOURCES=$(shell find . -name '*.cpp')
	CLEANCMD=rm -f $(OBJECTS) $(BINDIR)/$(BINELF) $(BINDIR)/$(BINHEX) $(BINDIR)/$(BINBIN) GitVersion.h
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
INC_F4= . STM32F4XX_Lib/CMSIS/Include/ STM32F4XX_Lib/Device/ STM32F4XX_Lib/STM32F4xx_StdPeriph_Driver/include/
INCLUDES_F4=$(INC_F4:%=-I%)
INC_F7= . STM32F7XX_Lib/CMSIS/Include/ STM32F7XX_Lib/Device/ STM32F7XX_Lib/STM32F7xx_StdPeriph_Driver/inc/
INCLUDES_F7=$(INC_F7:%=-I%)

# Find libraries
INCLUDES_LIBS_F4=STM32F4XX_Lib/CMSIS/Lib/GCC/libarm_cortexM4lf_math.a
INCLUDES_LIBS_F7=STM32F7XX_Lib/CMSIS/Lib/GCC/libarm_cortexM7lfsp_math.a

# Create object list
OBJECTS=$(ASOURCES:%.s=%.o)
OBJECTS+=$(CSOURCES:%.c=%.o)
OBJECTS+=$(CXXSOURCES:%.cpp=%.o)

# Define output files ELF & IHEX
BINELF=outp.elf
BINHEX=outp.hex
BINBIN=outp.bin

# MCU FLAGS
MCFLAGS_F4=-mcpu=cortex-m4 -mthumb -mlittle-endian -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb-interwork
MCFLAGS_F7=-mcpu=cortex-m7 -mthumb -mlittle-endian -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb-interwork

# COMPILE FLAGS
# STM32F4 Discovery board:
DEFS_DIS=-DUSE_STDPERIPH_DRIVER -DSTM32F4XX -DSTM32F40_41xxx -DSTM32F4_DISCOVERY -DHSE_VALUE=$(OSC) -DMADEBYMAKEFILE
# MMDVM-Pi board:
DEFS_PI=-DUSE_STDPERIPH_DRIVER -DSTM32F4XX -DSTM32F446xx -DSTM32F4_PI -DHSE_VALUE=$(OSC) -DMADEBYMAKEFILE
# STM32F4 Nucleo-64 F446RE board:
DEFS_NUCLEO=-DUSE_STDPERIPH_DRIVER -DSTM32F4XX -DSTM32F446xx -DSTM32F4_NUCLEO -DHSE_VALUE=$(OSC) -DMADEBYMAKEFILE
# STM32F7 Nucleo-144-F767ZI board:
DEFS_NUCLEO_F767=-DUSE_HAL_DRIVER -DSTM32F767xx -DSTM32F7XX -DSTM32F7_NUCLEO -DHSE_VALUE=$(OSC) -DMADEBYMAKEFILE

CFLAGS_F4=-c $(MCFLAGS_F4) $(INCLUDES_F4)
CXXFLAGS_F4=-c $(MCFLAGS_F4) $(INCLUDES_F4)
CFLAGS_F7=-c $(MCFLAGS_F7) $(INCLUDES_F7)
CXXFLAGS_F7=-c $(MCFLAGS_F7) $(INCLUDES_F7)

# LINKER FLAGS
LDFLAGS_F4 =-T stm32f4xx_link.ld $(MCFLAGS_F4) --specs=nosys.specs $(INCLUDES_LIBS_F4)
LDFLAGS_F7 =-T stm32f7xx_link.ld $(MCFLAGS_F7) --specs=nosys.specs $(INCLUDES_LIBS_F7)

# COMMON FLAGS
CFLAGS=-Os -ffunction-sections -fdata-sections -fno-builtin -Wno-implicit -DCUSTOM_NEW -DNO_EXCEPTIONS
CXXFLAGS=-Os -fno-exceptions -ffunction-sections -fdata-sections -fno-builtin -fno-rtti -DCUSTOM_NEW -DNO_EXCEPTIONS
LDFLAGS=-Os --specs=nano.specs

# Build Rules
.PHONY: all release dis pi nucleo clean

# Default target: STM32F4 Nucleo F446RE board
all: nucleo

pi: GitVersion.h
pi: CFLAGS+=$(CFLAGS_F4) $(DEFS_PI)
pi: CXXFLAGS+=$(CXXFLAGS_F4) $(DEFS_PI)
pi: LDFLAGS+=$(LDFLAGS_F4)
pi: release

nucleo: GitVersion.h
nucleo: CFLAGS+=$(CFLAGS_F4) $(DEFS_NUCLEO)
nucleo: CXXFLAGS+=$(CXXFLAGS_F4) $(DEFS_NUCLEO)
nucleo: LDFLAGS+=$(LDFLAGS_F4)
nucleo: release

dis: GitVersion.h
dis: CFLAGS+=$(CFLAGS_F4) $(DEFS_DIS)
dis: CXXFLAGS+=$(CXXFLAGS_F4) $(DEFS_DIS)
dis: LDFLAGS+=$(LDFLAGS_F4)
dis: release

f767: GitVersion.h
f767: CFLAGS+=$(CFLAGS_F7) $(DEFS_NUCLEO_F767)
f767: CXXFLAGS+=$(CXXFLAGS_F7) $(DEFS_NUCLEO_F767)
f767: LDFLAGS+=$(LDFLAGS_F7)
f767: release

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
	/usr/bin/openocd -f /usr/share/openocd/scripts/interface/stlink-v2-1.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg -c "program bin/$(BINELF) verify reset exit"
endif

ifneq ($(wildcard /usr/local/bin/openocd),)
	/usr/local/bin/openocd -f /usr/local/share/openocd/scripts/interface/stlink-v2-1.cfg -f /usr/local/share/openocd/scripts/target/stm32f4x.cfg -c "program bin/$(BINELF) verify reset exit"
endif

ifneq ($(wildcard /opt/openocd/bin/openocd),)
	/opt/openocd/bin/openocd -f /opt/openocd/share/openocd/scripts/interface/stlink-v2-1.cfg -f /opt/openocd/share/openocd/scripts/target/stm32f4x.cfg -c "program bin/$(BINELF) verify reset exit"
endif

deploy-f7:
ifneq ($(wildcard /usr/bin/openocd),)
	/usr/bin/openocd -f /usr/share/openocd/scripts/interface/stlink-v2-1.cfg -f /usr/share/openocd/scripts/target/stm32f7x.cfg -c "program bin/$(BINELF) verify reset exit"
endif

ifneq ($(wildcard /usr/local/bin/openocd),)
	/usr/local/bin/openocd -f /usr/local/share/openocd/scripts/interface/stlink-v2-1.cfg -f /usr/local/share/openocd/scripts/target/stm32f7x.cfg -c "program bin/$(BINELF) verify reset exit"
endif

ifneq ($(wildcard /opt/openocd/bin/openocd),)
	/opt/openocd/bin/openocd -f /opt/openocd/share/openocd/scripts/interface/stlink-v2-1.cfg -f /opt/openocd/share/openocd/scripts/target/stm32f7x.cfg -c "program bin/$(BINELF) verify reset exit"
endif

deploy-pi:
ifneq ($(wildcard /usr/local/bin/stm32flash),)
	-/usr/local/bin/stm32flash -i 20,-21,21:-20,21 /dev/ttyAMA0
	-/usr/local/bin/stm32ld /dev/ttyAMA0 57600 bin/outp.bin
	/usr/local/bin/stm32flash -v -w bin/outp.bin -g 0x0 -R -c /dev/ttyAMA0
endif

ifneq ($(wildcard /usr/bin/stm32flash),)
	-/usr/bin/stm32flash -i 20,-21,21:-20,21 /dev/ttyAMA0
	-/usr/bin/stm32ld /dev/ttyAMA0 57600 bin/outp.bin
	/usr/bin/stm32flash -v -w bin/outp.bin -g 0x0 -R -c /dev/ttyAMA0
endif

# Export the current git version if the index file exists, else 000...
GitVersion.h:
ifdef SYSTEMROOT
	echo #define GITVERSION "0000000" > $@
else ifdef SystemRoot
	echo #define GITVERSION "0000000" > $@
else
ifneq ("$(wildcard .git/index)","")
	echo "#define GITVERSION \"$(shell git rev-parse --short HEAD)\"" > $@
else
	echo "#define GITVERSION \"0000000\"" > $@
endif
endif
