STM32F7XX library for MMDVM project

This is the STM32F7XX_Lib directory structure with just the needed code for MMDVM.
Copy this STM32F7XX_Lib directory inside the MMDVM source code project. You have to
conserve the STM32F7XX_Lib folder name: MMDVM/STM32F7XX_Lib

The source of CMSIS library and some include files is from STM32Cube_FW_F7_V1.7.0:
http://www.st.com/content/st_com/en/products/embedded-software/mcus-embedded-software/stm32-embedded-software/stm32cube-embedded-software/stm32cubef7.html

The adaptation to STM32F7 Standard Peripheral Library (STM32F7XX_Lib/STM32F7xx_StdPeriph_Driver) is from:
https://gitlab.com/caesar-lab/stm32f7-legacy-library

In order to run the MMDVM firmware correctly, there are the following modifications
already done in this package:

1) STM32F7XX_Lib/Device/startup/startup_stm32f7xx.c :
- Added C++ global constructors support
- Changed initial stack pointer
- Changed support for STM32F767

2) STM32F7XX_Lib/Device/system_stm32f7xx.c :
- Changed PLL_M definition (support "integer" only crystal frequencies: 8, 12, 16 MHz, etc)
- Added functions: SetSysClock(), MPU_Config(), CPU_CACHE_Enable() from examples in https://gitlab.com/caesar-lab/stm32f7-legacy-library

3) STM32F7XX_Lib/Device/stm32f7xx_hal_conf.h :
- Peripheral header file inclusions
