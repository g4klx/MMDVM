STM32F4XX library for MMDVM project

This is the STM32F4XX_Lib directory structure with just the needed code for MMDVM.
Copy this STM32F4XX_Lib directory inside the MMDVM source code project. You have to
conserve the STM32F4XX_Lib folder name: MMDVM/STM32F4XX_Lib

The source of this package can be found here:
http://www.st.com/en/embedded-software/stsw-stm32065.html

In order to run the MMDVM firmware correctly, there are the following modifications
already done in this package:

1) STM32F4XX_Lib/Device/startup/startup_stm32f4xx.c :
- Added C++ global constructors support
- Changed initial stack pointer

2) STM32F4XX_Lib/Device/system_stm32f4xx.c :
- Changed PLL_M definition (support "integer" only crystal frequencies: 8, 12, 16 MHz, etc)

3) STM32F4XX_Lib/Device/stm32f4xx_conf.h :
- Peripheral header file inclusions
