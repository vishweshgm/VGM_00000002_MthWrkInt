#ifndef _MBMCONFIG_H
#define _MBMCONFIG_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

/* ----------------------- Defines ------------------------------------------*/
/*! \mainpage Documentation for ETHERSYS - STM32 MODBUS STACK
 *
 * \section intro_sec Introduction
 *
 * This stack is available for MODBUS-RTU implementation in STM32F1XX.
 * \section req_sec	A. Requirements covered in stack
 * \subsection step1 1: Modbus Function Support
 * Following function API are exposed:
 * READ_COILS   		01 (0x01)
 * READ_DISC_INPUTS   	02 (0x02)
 * RD_HOLDING  			03 (0x03)
 * RD_INPUT_REGS   		04 (0x04)
 * WRITE_SINGLE_COIL   	05 (0x05)
 * WR_SINGLE_REG   		06 (0x06)
 * WRITE_COILS   		15 (0x0F)
 * WR_MUL_REGS   		16 (0x10)
 * REPORT_SLAVEID 		17 (0x11)
 * RD_FILES  			20 (0x14)
 * WR_FILES   			21 (0x15)
 * RDWR_MUL_REGS 		23 (0x17)
 * \subsection step2 2: This library APIs will be specific for MODBUS MASTER implementation only.
 * \subsection step3 3: ToolchainUsed for library generation
 * --> Standard tool-chain of STM32 GNU Tools (arm-none-eabi-gcc), using linux makefile
 * \subsection step4 4: Should be based on STM32CUBE
 * --> For this requirement, UART and TIMER peripherals are coded in low-level driver.
 * Hence a project can be started from scratch using STM32CUBE. One has to just call interrupt handlers as an additional config.
 * The details are available in this document. One may refer to demo Project shared to understand quickly.
 * \subsection step5 5: Require Modbus master  Read Register Holding immediately . You can provide the *.lib or *.a library with the interface header file.
 * All the functions are supported in this release.
 * \subsection step6 6: Should expose RS485 Enable pin & its Port.
 * This is exposed via macro
 * \subsection step7 7: Should expose the Modbus Serial(UART) port
 * All 3 UART port of STM32F103C8T6 is made available to configure through macro.
 * \subsection step8 8: 3.	Should expose maximum Slave support (ex. #define for MB_TOTAL_SLAVE_NUM , REG_HOLDING_START , REG_HOLDING_NREGS )
 * Maximum number of slaves supported by modbus is availbel with this stack.
 *
 * \section install_sec B. Installation
 * Kindly refer to the demo project shared. Its easy to integrate via STM32CubeIDE by OpenST.
 *\section use_sec C. How to use this document?
 * Please open the Modules Section of this document. All the APIs and Macros, enus etc. are divided into two groups.
 * QPS_MODBUS_INTERNAL_V500_RTU & QPS_MODBUS_CUSTOMER_V500_RTU
 * Both groups together contain all the details of the stack implemented.
 * However it is advised that user must only change/use the APIs / MACROS listed under QPS_MODBUS_CUSTOMER_V500_RTU
 * User needs to go through every macro and understand what each macro does before changing any value of it.
 * Each macro/api is well documented for not leaving any room for confusion. But still feel free to contact quadpowersys@gmail.com in case of any queries.
 * \section hist_sec D. Version History
 * \subsection v500 : v5.0.0 >> Initial
 * \subsection v600 : v6.0.0 >> Listed Below
 * -->Provided Flexibility to use TIM2,TIM3 & TIM4. macro MBM_TIMER_STM added for this. User must be careful to call proper TIMx IRQ_Handler function.
 */
#ifdef __cplusplus
PR_END_EXTERN_C
#endif

#define MBM_ASCII_BACKOF_TIME_MS                ( 0 ) 

#endif
