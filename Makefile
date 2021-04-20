#  Copyright (C) 2016,2017,2018 by Andy Uribe CA6JAU
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

# MMDVM source files
MMDVM_PATH=.

# STM32 library paths
F4_LIB_PATH=./STM32F4XX_Lib
F7_LIB_PATH=./STM32F7XX_Lib

# MCU external clock frequency (Hz)
CLK_MMDVM_PI=12000000
CLK_NUCLEO=8000000
CLK_12MHZ=12000000

# Directory Structure
BINDIR=bin
OBJDIR_F4=obj_f4
OBJDIR_F7=obj_f7

# Output files
BINELF_F4=mmdvm_f4.elf
BINHEX_F4=mmdvm_f4.hex
BINBIN_F4=mmdvm_f4.bin
BINELF_F7=mmdvm_f7.elf
BINHEX_F7=mmdvm_f7.hex
BINBIN_F7=mmdvm_f7.bin

# Header directories
INC_F4= . $(F4_LIB_PATH)/CMSIS/Include/ $(F4_LIB_PATH)/Device/ $(F4_LIB_PATH)/STM32F4xx_StdPeriph_Driver/include/
INCLUDES_F4=$(INC_F4:%=-I%)
INC_F7= . $(F7_LIB_PATH)/CMSIS/Include/ $(F7_LIB_PATH)/Device/ $(F7_LIB_PATH)/STM32F7xx_StdPeriph_Driver/inc/
INCLUDES_F7=$(INC_F7:%=-I%)

# CMSIS libraries
INCLUDES_LIBS_F4=$(F4_LIB_PATH)/CMSIS/Lib/GCC/libarm_cortexM4lf_math.a
INCLUDES_LIBS_F7=$(F7_LIB_PATH)/CMSIS/Lib/GCC/libarm_cortexM7lfsp_math.a

# STM32F4 Standard Peripheral Libraries source path
STD_LIB_F4=$(F4_LIB_PATH)/STM32F4xx_StdPeriph_Driver/source

# STM32F7 Standard Peripheral Libraries source path
STD_LIB_F7=$(F7_LIB_PATH)/STM32F7xx_StdPeriph_Driver/src

# STM32F4 system source path
SYS_DIR_F4=$(F4_LIB_PATH)/Device
STARTUP_DIR_F4=$(F4_LIB_PATH)/Device/startup

# STM32F7 system source path
SYS_DIR_F7=$(F7_LIB_PATH)/Device
STARTUP_DIR_F7=$(F7_LIB_PATH)/Device/startup

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

# "SystemRoot" is only defined in Windows
ifdef SYSTEMROOT
	CLEANCMD=del /S *.o *.hex *.bin *.elf GitVersion.h
	MDDIRS=md $@
else ifdef SystemRoot
	CLEANCMD=del /S *.o *.hex *.bin *.elf GitVersion.h
	MDDIRS=md $@
else
	CLEANCMD=rm -f $(OBJ_F4) $(OBJ_F7) $(BINDIR)/*.hex $(BINDIR)/*.bin $(BINDIR)/*.elf GitVersion.h
	MDDIRS=mkdir $@
endif

# Default reference oscillator frequencies
ifndef $(OSC)
	ifeq ($(MAKECMDGOALS),pi)
		OSC=$(CLK_MMDVM_PI)
	else ifeq ($(MAKECMDGOALS),pi-f722)
		OSC=$(CLK_MMDVM_PI)
	else ifeq ($(MAKECMDGOALS),drcc_nqf)
		OSC=$(CLK_12MHZ)
	else
		OSC=$(CLK_NUCLEO)
	endif
endif

# Build object lists
CXXSRC=$(wildcard $(MMDVM_PATH)/*.cpp)
CSRC_STD_F4=$(wildcard $(STD_LIB_F4)/*.c)
SYS_F4=$(wildcard $(SYS_DIR_F4)/*.c)
STARTUP_F4=$(wildcard $(STARTUP_DIR_F4)/*.c)
CSRC_STD_F7=$(wildcard $(STD_LIB_F7)/*.c)
SYS_F7=$(wildcard $(SYS_DIR_F7)/*.c)
STARTUP_F7=$(wildcard $(STARTUP_DIR_F7)/*.c)
OBJ_F4=$(CXXSRC:$(MMDVM_PATH)/%.cpp=$(OBJDIR_F4)/%.o) $(CSRC_STD_F4:$(STD_LIB_F4)/%.c=$(OBJDIR_F4)/%.o) $(SYS_F4:$(SYS_DIR_F4)/%.c=$(OBJDIR_F4)/%.o) $(STARTUP_F4:$(STARTUP_DIR_F4)/%.c=$(OBJDIR_F4)/%.o)
OBJ_F7=$(CXXSRC:$(MMDVM_PATH)/%.cpp=$(OBJDIR_F7)/%.o) $(CSRC_STD_F7:$(STD_LIB_F7)/%.c=$(OBJDIR_F7)/%.o) $(SYS_F7:$(SYS_DIR_F7)/%.c=$(OBJDIR_F7)/%.o) $(STARTUP_F7:$(STARTUP_DIR_F7)/%.c=$(OBJDIR_F7)/%.o)

# MCU flags
MCFLAGS_F4=-mcpu=cortex-m4 -mthumb -mlittle-endian -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb-interwork
MCFLAGS_F7=-mcpu=cortex-m7 -mthumb -mlittle-endian -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb-interwork

# Compile flags
# STM32F4 Discovery board:
DEFS_DIS=-DUSE_STDPERIPH_DRIVER -DSTM32F4XX -DSTM32F40_41xxx -DSTM32F4_DISCOVERY -DHSE_VALUE=$(OSC) -DMADEBYMAKEFILE
# MMDVM-Pi board:
DEFS_PI=-DUSE_STDPERIPH_DRIVER -DSTM32F4XX -DSTM32F446xx -DSTM32F4_PI -DHSE_VALUE=$(OSC) -DMADEBYMAKEFILE
# MMDVM-F4M F0DEI board:
DEFS_F4M=-DUSE_STDPERIPH_DRIVER -DSTM32F4XX -DSTM32F446xx -DSTM32F4_F4M -DHSE_VALUE=$(OSC) -DMADEBYMAKEFILE
# STM32F4 Nucleo-64 F446RE board:
DEFS_NUCLEO=-DUSE_STDPERIPH_DRIVER -DSTM32F4XX -DSTM32F446xx -DSTM32F4_NUCLEO -DHSE_VALUE=$(OSC) -DMADEBYMAKEFILE
# STM32F7 Nucleo-144 F767ZI board:
DEFS_NUCLEO_F767=-DUSE_HAL_DRIVER -DSTM32F767xx -DSTM32F7XX -DSTM32F7_NUCLEO -DHSE_VALUE=$(OSC) -DMADEBYMAKEFILE
# MMDVM-Pi F722 board:
DEFS_PI_F722=-DUSE_HAL_DRIVER -DSTM32F722xx -DSTM32F7XX -DSTM32F722_PI -DHSE_VALUE=$(OSC) -DMADEBYMAKEFILE
# MMDVM-F7M F0DEI board:
DEFS_F7M=-DUSE_HAL_DRIVER -DSTM32F722xx -DSTM32F7XX -DSTM32F722_F7M -DHSE_VALUE=$(OSC) -DMADEBYMAKEFILE
# MMDVM_RPT_Hat F0DEI, DB9MAT, DF2ET board:
DEFS_RPT_HAT=-DUSE_HAL_DRIVER -DSTM32F722xx -DSTM32F7XX -DSTM32F722_RPT_HAT -DHSE_VALUE=$(OSC) -DMADEBYMAKEFILE
# RB STM32F4_DVM board:
DEFS_DVM=-DUSE_STDPERIPH_DRIVER -DSTM32F4XX -DSTM32F446xx -DSTM32F4_DVM -DHSE_VALUE=$(OSC) -DMADEBYMAKEFILE
# RB STM32F7 DVM board:
DEFS_DVM722=-DUSE_HAL_DRIVER -DSTM32F722xx -DSTM32F7XX -DSTM32F7_DVM -DHSE_VALUE=$(OSC) -DMADEBYMAKEFILE
# MMDVM_RPT_Hat BG4TGO, BG5HHP board:
DEFS_RPT_HAT_TGO=-DUSE_STDPERIPH_DRIVER -DSTM32F4XX -DSTM32F40_41xxx -DSTM32F4_RPT_HAT_TGO -DHSE_VALUE=$(OSC) -DMADEBYMAKEFILE
# DRCC_DVM BG7NQF board:
DEFS_DRCC_DVM=-DUSE_STDPERIPH_DRIVER -DSTM32F4XX -DSTM32F446xx -DDRCC_DVM -DHSE_VALUE=$(OSC) -DMADEBYMAKEFILE
# WA0EDA F405 MTR2K, MASTR3 board:
DEFS_EDA_405=-DUSE_STDPERIPH_DRIVER -DSTM32F4XX -DSTM32F40_41xxx -DSTM32F4_EDA_405 -DHSE_VALUE=$(OSC) -DMADEBYMAKEFILE
# WA0EDA F446 MTR2K, MASTR3 board:
DEFS_EDA_446=-DUSE_STDPERIPH_DRIVER -DSTM32F4XX -DSTM32F446xx -DSTM32F4_EDA_446 -DHSE_VALUE=$(OSC) -DMADEBYMAKEFILE


# Build compiler flags
CFLAGS_F4=-c $(MCFLAGS_F4) $(INCLUDES_F4)
CXXFLAGS_F4=-c $(MCFLAGS_F4) $(INCLUDES_F4)
CFLAGS_F7=-c $(MCFLAGS_F7) $(INCLUDES_F7)
CXXFLAGS_F7=-c $(MCFLAGS_F7) $(INCLUDES_F7)

# Linker flags
LDFLAGS_F4 =-T stm32f4xx_link.ld $(MCFLAGS_F4) --specs=nosys.specs $(INCLUDES_LIBS_F4)
LDFLAGS_F767 =-T stm32f767_link.ld $(MCFLAGS_F7) --specs=nosys.specs $(INCLUDES_LIBS_F7)
LDFLAGS_F722 =-T stm32f722_link.ld $(MCFLAGS_F7) --specs=nosys.specs $(INCLUDES_LIBS_F7)

# Common flags
CFLAGS=-Os -ffunction-sections -fdata-sections -fno-builtin -Wno-implicit -DCUSTOM_NEW -DNO_EXCEPTIONS
CXXFLAGS=-Os -fno-exceptions -ffunction-sections -fdata-sections -fno-builtin -fno-rtti -DCUSTOM_NEW -DNO_EXCEPTIONS
LDFLAGS=-Os --specs=nano.specs -Wl,-Map=bin/mmdvm.map

# Build Rules
.PHONY: all release dis pi pi-f722 f4m nucleo f767 dvm drcc_nqf clean

# Default target: Nucleo-64 F446RE board
all: nucleo

pi: GitVersion.h
pi: CFLAGS+=$(CFLAGS_F4) $(DEFS_PI)
pi: CXXFLAGS+=$(CXXFLAGS_F4) $(DEFS_PI)
pi: LDFLAGS+=$(LDFLAGS_F4)
pi: release_f4

pi-f722: GitVersion.h
pi-f722: CFLAGS+=$(CFLAGS_F7) $(DEFS_PI_F722)
pi-f722: CXXFLAGS+=$(CXXFLAGS_F7) $(DEFS_PI_F722)
pi-f722: LDFLAGS+=$(LDFLAGS_F722)
pi-f722: release_f7

f4m: GitVersion.h
f4m: CFLAGS+=$(CFLAGS_F4) $(DEFS_F4M)
f4m: CXXFLAGS+=$(CXXFLAGS_F4) $(DEFS_F4M)
f4m: LDFLAGS+=$(LDFLAGS_F4)
f4m: release_f4

f7m: GitVersion.h
f7m: CFLAGS+=$(CFLAGS_F7) $(DEFS_F7M)
f7m: CXXFLAGS+=$(CXXFLAGS_F7) $(DEFS_F7M)
f7m: LDFLAGS+=$(LDFLAGS_F722)
f7m: release_f7

rpt_hat: GitVersion.h
rpt_hat: CFLAGS+=$(CFLAGS_F7) $(DEFS_RPT_HAT)
rpt_hat: CXXFLAGS+=$(CXXFLAGS_F7) $(DEFS_RPT_HAT)
rpt_hat: LDFLAGS+=$(LDFLAGS_F722)
rpt_hat: release_f7

rpt_hat_tgo: GitVersion.h
rpt_hat_tgo: CFLAGS+=$(CFLAGS_F4) $(DEFS_RPT_HAT_TGO)
rpt_hat_tgo: CXXFLAGS+=$(CXXFLAGS_F4) $(DEFS_RPT_HAT_TGO)
rpt_hat_tgo: LDFLAGS+=$(LDFLAGS_F4)
rpt_hat_tgo: release_f4

nucleo: GitVersion.h
nucleo: CFLAGS+=$(CFLAGS_F4) $(DEFS_NUCLEO)
nucleo: CXXFLAGS+=$(CXXFLAGS_F4) $(DEFS_NUCLEO)
nucleo: LDFLAGS+=$(LDFLAGS_F4)
nucleo: release_f4

dis: GitVersion.h
dis: CFLAGS+=$(CFLAGS_F4) $(DEFS_DIS)
dis: CXXFLAGS+=$(CXXFLAGS_F4) $(DEFS_DIS)
dis: LDFLAGS+=$(LDFLAGS_F4)
dis: release_f4

f767: GitVersion.h
f767: CFLAGS+=$(CFLAGS_F7) $(DEFS_NUCLEO_F767)
f767: CXXFLAGS+=$(CXXFLAGS_F7) $(DEFS_NUCLEO_F767)
f767: LDFLAGS+=$(LDFLAGS_F767)
f767: release_f7

dvm: GitVersion.h
dvm: CFLAGS+=$(CFLAGS_F4) $(DEFS_DVM) -DDRCC_DVM_446
dvm: CXXFLAGS+=$(CXXFLAGS_F4) $(DEFS_DVM) -DDRCC_DVM_446
dvm: LDFLAGS+=$(LDFLAGS_F4)
dvm: release_f4

dvm722: GitVersion.h
dvm722: CFLAGS+=$(CFLAGS_F7) $(DEFS_DVM722) -DDRCC_DVM_722
dvm722: CXXFLAGS+=$(CXXFLAGS_F7) $(DEFS_DVM722) -DDRCC_DVM_722
dvm722: LDFLAGS+=$(LDFLAGS_F722)
dvm722: release_f7

drcc_nqf: GitVersion.h
drcc_nqf: CFLAGS+=$(CFLAGS_F4) $(DEFS_DRCC_DVM) -DDRCC_DVM_NQF
drcc_nqf: CXXFLAGS+=$(CXXFLAGS_F4) $(DEFS_DRCC_DVM) -DDRCC_DVM_NQF
drcc_nqf: LDFLAGS+=$(LDFLAGS_F4)
drcc_nqf: release_f4

hhp446: GitVersion.h
hhp446: CFLAGS+=$(CFLAGS_F4) $(DEFS_DRCC_DVM) -DDRCC_DVM_HHP446
hhp446: CXXFLAGS+=$(CXXFLAGS_F4) $(DEFS_DRCC_DVM) -DDRCC_DVM_HHP446
hhp446: LDFLAGS+=$(LDFLAGS_F4)
hhp446: release_f4

eda405: GitVersion.h
eda405: CFLAGS+=$(CFLAGS_F4) $(DEFS_EDA_405)
eda405: CXXFLAGS+=$(CXXFLAGS_F4) $(DEFS_EDA_405)
eda405: LDFLAGS+=$(LDFLAGS_F4)
eda405: release_f4

eda446: GitVersion.h
eda446: CFLAGS+=$(CFLAGS_F4) $(DEFS_EDA_446)
eda446: CXXFLAGS+=$(CXXFLAGS_F4) $(DEFS_EDA_446)
eda446: LDFLAGS+=$(LDFLAGS_F4)
eda446: release_f4

release_f4: $(BINDIR)
release_f4: $(OBJDIR_F4)
release_f4: $(BINDIR)/$(BINHEX_F4)
release_f4: $(BINDIR)/$(BINBIN_F4)

release_f7: $(BINDIR)
release_f7: $(OBJDIR_F7)
release_f7: $(BINDIR)/$(BINHEX_F7)
release_f7: $(BINDIR)/$(BINBIN_F7)

$(BINDIR):
	$(MDDIRS)

$(OBJDIR_F4):
	$(MDDIRS)

$(OBJDIR_F7):
	$(MDDIRS)

$(BINDIR)/$(BINHEX_F4): $(BINDIR)/$(BINELF_F4)
	$(CP) -O ihex $< $@
	@echo "Objcopy from ELF to IHEX complete!\n"

$(BINDIR)/$(BINBIN_F4): $(BINDIR)/$(BINELF_F4)
	$(CP) -O binary $< $@
	@echo "Objcopy from ELF to BINARY complete!\n"

$(BINDIR)/$(BINELF_F4): $(OBJ_F4)
	$(CXX) $(OBJ_F4) $(LDFLAGS) -o $@
	@echo "Linking complete!\n"
	$(SIZE) $(BINDIR)/$(BINELF_F4)

$(BINDIR)/$(BINHEX_F7): $(BINDIR)/$(BINELF_F7)
	$(CP) -O ihex $< $@
	@echo "Objcopy from ELF to IHEX complete!\n"

$(BINDIR)/$(BINBIN_F7): $(BINDIR)/$(BINELF_F7)
	$(CP) -O binary $< $@
	@echo "Objcopy from ELF to BINARY complete!\n"

$(BINDIR)/$(BINELF_F7): $(OBJ_F7)
	$(CXX) $(OBJ_F7) $(LDFLAGS) -o $@
	@echo "Linking complete!\n"
	$(SIZE) $(BINDIR)/$(BINELF_F7)

$(OBJDIR_F4)/%.o: $(MMDVM_PATH)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
	@echo "Compiled "$<"!\n"

$(OBJDIR_F7)/%.o: $(MMDVM_PATH)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
	@echo "Compiled "$<"!\n"

$(OBJDIR_F4)/%.o: $(STD_LIB_F4)/%.c
	$(CC) $(CFLAGS) $< -o $@
	@echo "Compiled "$<"!\n"

$(OBJDIR_F7)/%.o: $(STD_LIB_F7)/%.c
	$(CC) $(CFLAGS) $< -o $@
	@echo "Compiled "$<"!\n"

$(OBJDIR_F4)/%.o: $(SYS_DIR_F4)/%.c
	$(CC) $(CFLAGS) $< -o $@
	@echo "Compiled "$<"!\n"

$(OBJDIR_F4)/%.o: $(STARTUP_DIR_F4)/%.c
	$(CC) $(CFLAGS) $< -o $@
	@echo "Compiled "$<"!\n"

$(OBJDIR_F7)/%.o: $(SYS_DIR_F7)/%.c
	$(CC) $(CFLAGS) $< -o $@
	@echo "Compiled "$<"!\n"

$(OBJDIR_F7)/%.o: $(STARTUP_DIR_F7)/%.c
	$(CC) $(CFLAGS) $< -o $@
	@echo "Compiled "$<"!\n"

clean:
	$(CLEANCMD)

deploy:
ifneq ($(wildcard /usr/bin/openocd),)
	/usr/bin/openocd -f /usr/share/openocd/scripts/interface/stlink-v2-1.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg -c "program bin/$(BINELF_F4) verify reset exit"
endif

ifneq ($(wildcard /usr/local/bin/openocd),)
	/usr/local/bin/openocd -f /usr/local/share/openocd/scripts/interface/stlink-v2-1.cfg -f /usr/local/share/openocd/scripts/target/stm32f4x.cfg -c "program bin/$(BINELF_F4) verify reset exit"
endif

ifneq ($(wildcard /opt/openocd/bin/openocd),)
	/opt/openocd/bin/openocd -f /opt/openocd/share/openocd/scripts/interface/stlink-v2-1.cfg -f /opt/openocd/share/openocd/scripts/target/stm32f4x.cfg -c "program bin/$(BINELF_F4) verify reset exit"
endif

deploy-f7:
ifneq ($(wildcard /usr/bin/openocd),)
	/usr/bin/openocd -f /usr/share/openocd/scripts/interface/stlink-v2-1.cfg -f /usr/share/openocd/scripts/target/stm32f7x.cfg -c "program bin/$(BINELF_F7) verify reset exit"
endif

ifneq ($(wildcard /usr/local/bin/openocd),)
	/usr/local/bin/openocd -f /usr/local/share/openocd/scripts/interface/stlink-v2-1.cfg -f /usr/local/share/openocd/scripts/target/stm32f7x.cfg -c "program bin/$(BINELF_F7) verify reset exit"
endif

ifneq ($(wildcard /opt/openocd/bin/openocd),)
	/opt/openocd/bin/openocd -f /opt/openocd/share/openocd/scripts/interface/stlink-v2-1.cfg -f /opt/openocd/share/openocd/scripts/target/stm32f7x.cfg -c "program bin/$(BINELF_F7) verify reset exit"
endif

deploy-pi:
ifneq ($(wildcard /usr/local/bin/stm32flash),)
	-/usr/local/bin/stm32flash -i 20,-21,21:-20,21 /dev/ttyAMA0
	/usr/local/bin/stm32flash -v -w bin/$(BINBIN_F4) -g 0x0 -R /dev/ttyAMA0
endif

ifneq ($(wildcard /usr/bin/stm32flash),)
	-/usr/bin/stm32flash -i 20,-21,21:-20,21 /dev/ttyAMA0
	/usr/bin/stm32flash -v -w bin/$(BINBIN_F4) -g 0x0 -R /dev/ttyAMA0
endif

deploy-f4m: deploy-pi
deploy-dvm: deploy-pi
deploy-eda405: deploy-pi
deploy-eda446: deploy-pi

deploy-pi-f7:
ifneq ($(wildcard /usr/local/bin/stm32flash),)
	-/usr/local/bin/stm32flash -i 20,-21,21:-20,21 /dev/ttyAMA0
	/usr/local/bin/stm32flash -v -w bin/$(BINBIN_F7) -g 0x0 -R /dev/ttyAMA0

else ifneq ($(wildcard /usr/bin/stm32flash),)
	-/usr/bin/stm32flash -i 20,-21,21:-20,21 /dev/ttyAMA0
	/usr/bin/stm32flash -v -w bin/$(BINBIN_F7) -g 0x0 -R /dev/ttyAMA0
endif

deploy-f7m:   deploy-pi-f7
deploy-rpt_hat: deploy-pi-f7

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

flash_f4:
	@echo "flashing firmware..."
	st-flash write bin/$(BINBIN_F4) 0x8000000
