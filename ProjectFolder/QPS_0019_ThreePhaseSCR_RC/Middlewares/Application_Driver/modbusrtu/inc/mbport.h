#ifndef _MB_PORT_H
#define _MB_PORT_H

#include <assert.h>
#include "main.h"
#ifdef __cplusplus
extern          "C"
{
#endif
/*! \addtogroup QPS_MODBUS_CUSTOMER_V500_RTU
 * @{
 */
/* ----------------------- Defines ------------------------------------------*/
/*! \brief QPS: Internal Macro do not change this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 */
#define INLINE                              inline
/*! \brief QPS: Internal Macro do not change this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 */
#define STATIC                              static
/*! \brief QPS: Internal Macro do not change this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 */
#define PR_BEGIN_EXTERN_C                   extern "C" {
/*! \brief QPS: Internal Macro do not change this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 */
#define	PR_END_EXTERN_C                     }
/*! \brief QPS: Internal Macro do not change this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 */
#define MBP_ASSERT( x )                     ( ( x ) ? ( void )0 : vMBPAssert( ) )
/*! \brief QPS: Internal Macro do not change this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 */
#define MBP_ENTER_CRITICAL_SECTION( )       vMBPEnterCritical( )
/*! \brief QPS: Internal Macro do not change this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 */
#define MBP_EXIT_CRITICAL_SECTION( )        vMBPExitCritical( )
/*! \brief QPS: Internal Macro do not change this.
 */
#ifndef TRUE
#define TRUE                                ( BOOL )1
#endif
/*! \brief QPS: Internal Macro do not change this.
 */
#ifndef FALSE
#define FALSE                               ( BOOL )0
#endif
/*! \brief QPS: Internal Macro do not change this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 */
#define MBP_EVENTHDL_INVALID                NULL
/*! \brief QPS: Internal Macro do not use this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
#define MBP_TIMERHDL_INVALID                NULL
/*! \brief QPS: Internal Macro do not use this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
#define MBP_SERIALHDL_INVALID               NULL
/*! \brief QPS: Internal Macro do not change this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 */
#define QPS_MODBUS_STM32_USED				(1)
/*! \brief QPS: Internal Macro do not change this.
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 */
#define MB_UART_1				  	(0)
/*! \brief QPS: Internal Macro do not change this.
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 */
#define MB_UART_2				  	(1)
/*! \brief QPS: Internal Macro do not change this.
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 */
#define MB_UART_3				  	(2)
/*! \brief QPS: Internal Macro do not change this.
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 */
#define MB_TIM2					  	(0)
/*! \brief QPS: Internal Macro do not change this.
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 */
#define MB_TIM3					  	(1)
/*! \brief QPS: Internal Macro do not change this.
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 */
#define MB_TIM4					  	(2)
/*! \brief Depending on the application requirement user can either use MB_UART_1, MB_UART_2 or MB_UART_3 of STM32F1XX controller.
 * Also note that if CubeMX is used to configure the pins of controller, one should not initialize \ref MBM_SERIAL_PORT pins during code generation.
 *\ref eMBMSerialInit function call will take care of UART and timer initialization part. By adjusting following macro, either UART1,2,3 can be used.
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 */
#ifndef MBM_SERIAL_PORT
#define MBM_SERIAL_PORT           ( MB_UART_1 )
#endif

/*! \brief Depending on the application requirement user can either use MB_TIM2, MB_TIM3 or MB_TIM4 of STM32F1XX controller.
 * Also note that if CubeMX is used to configure the pins of controller, one should not initialize \ref MBM_TIMER_STM timers during code generation.
 *\ref eMBMSerialInit function call will take care of UART and timer initialization part. By adjusting following macro, either TIM2,3,4 can be used.
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 */
#ifndef MBM_TIMER_STM
#define MBM_TIMER_STM           ( MB_TIM2 )
#endif

/*! \brief Serial baud-rate setting. Only one baud-rate given by the following value can be set through out the project.
 * Important Note : Make sure that \ref UART_BAUDRATE_MAX > \ref MBM_SERIAL_BAUDRATE > \ref UART_BAUDRATE_MIN
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 */
#ifndef MBM_SERIAL_BAUDRATE
#define MBM_SERIAL_BAUDRATE       ( 115200 )
#endif

/*! \brief Parity can be MB_PAR_EVEN, MB_PAR_ODD or MB_PAR_NONE \ref eMBSerialParity
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 *  */
#ifndef MBM_PARITY
#define MBM_PARITY                ( MB_PAR_NONE )
#endif

/*! \brief Default response timeout for slave. If you don't know the slave response time keep the default value. \ref eMBMSetSlaveTimeout can also be used to change this value during runtime.
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 * */
#ifndef MBM_DEFAULT_RESPONSE_TIMEOUT
#define MBM_DEFAULT_RESPONSE_TIMEOUT            ( 1000 )
#endif

/*! \brief ALWAYS KEEP THIS 0. In a very exceptional case, where very low baudrate is used or slave device itself has not met the modbus 3.5 timer spec, only then this can be used to increase the
 *  modbus master 3.5 timer to a manual value given by \ref QPS_STM32F1_USETIMER_RECEIVE_MANUAL_VALUE . But keep-in-mind that changing this value will violate modbus spec. This is only provided for debugging purpose.
 *  \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 *  */
#ifndef QPS_STM32F1_USETIMER_RECEIVE_MANUAL
#define QPS_STM32F1_USETIMER_RECEIVE_MANUAL			(0)
#endif

#if (QPS_STM32F1_USETIMER_RECEIVE_MANUAL == 1)
/*! \brief refer \ref QPS_STM32F1_USETIMER_RECEIVE_MANUAL for details. This macro is effective only if \ref QPS_STM32F1_USETIMER_RECEIVE_MANUAL = 1
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 *  */
#define QPS_STM32F1_USETIMER_RECEIVE_MANUAL_VALUE	(10)
#endif
/*! \brief This is given for user-flexibility. This can be used to provide a small wait time for UART peripheral after transmit of query before
 * enabling the serial receive peripheral. Wait time is given by value \ref QPS_STM32_WAIT_TIME_MANUAL_VALUE . This might be useful to user for low-baudrates.
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 *  */
#ifndef QPS_STM32_WAIT_TIME_MANUAL
#define QPS_STM32_WAIT_TIME_MANUAL				( 0 )
#endif

#if (QPS_STM32_WAIT_TIME_MANUAL == 1)
/*! \brief refer \ref QPS_STM32_WAIT_TIME_MANUAL for details. This macro is effective only if \ref QPS_STM32_WAIT_TIME_MANUAL = 1
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 *  */
#define QPS_STM32_WAIT_TIME_MANUAL_VALUE		(50)
#endif

/*! \brief This is used for a special case when modbus slave use noparity,1stopbit config in its implementation.
 * Usually modbus must have 11-bits, 1STRT,8DATA,1PAR,1STP. when no parity used, we must use 2 stop bits to compensate it.
 * If user decide not to do it, he may choose \ref QPS_STM32F1_STOPBITVALUE_FORNOPAR = 1. As per modbus spec it should be put as 2.
 * Note that this macro is effective only if \ref MBM_PARITY is set to \ref MB_PAR_NONE
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 *  */
#ifndef QPS_STM32F1_STOPBITVALUE_FORNOPAR
#define QPS_STM32F1_STOPBITVALUE_FORNOPAR		(2)
#endif
/*! \brief Index invalid. QPS: Internal Macro do not change this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
#define IDX_INVALID				( 255 )
/*! \brief Minimum Baudrate supported by this stack.
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 *  */
#define UART_BAUDRATE_MIN		( 300 )
/*! \brief Maximum baudrate supported by this stack.
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 *  */
#define UART_BAUDRATE_MAX		( 115201 )
/*! \brief Number of serial handles to be used simulataneously.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
#define MAX_SERIL_HDLS			(1)
/*! \brief Number of timer handles to be used simulataneously.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
#define MAX_TIMER_HDLS          ( 3 )

/* ----------------------- hash defines ------------------------------*/
/*! \brief This TTL to RS485 converter chip Tx/Rx Enable pin configuration
 * Very Important : USER must decide the pin number and also the initialization of port for this output.
 * One may choose to use CubeMX to generate initialization code, then following macro should be left blank as it is.
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 *  */
#define RS_485_UART_INIT(  )	\
do { \
} while( 0 )
/*! \brief User must configure the code for enabling DE (uC UART TX) here
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 *  */
#define RS_485_UART_ENABLE_TX(  )	\
do {\
	HAL_GPIO_WritePin(uC_RS485_DE_GPIO_Port, uC_RS485_DE_Pin, GPIO_PIN_SET);\
} while( 0 )//DE_ACTIVEHIGH,RE_INACTIVE
/*! \brief User must configure the code for enabling RE (uC UART RX) here
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 *  */
#define RS_485_UART_DISABLE_TX(  ) \
do { \
	HAL_GPIO_WritePin(uC_RS485_DE_GPIO_Port, uC_RS485_DE_Pin, GPIO_PIN_RESET); \
} while( 0 )//RE_ACTIVELOW,DE_INACTIVE

/* ----------------------- Type definitions ---------------------------------*/
/*! \brief QPS: Internal Macro do not use this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
typedef void      *xMBPEventHandle;
/*! \brief QPS: Internal Macro do not use this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
typedef void      *xMBPTimerHandle;
/*! \brief QPS: Internal Macro do not use this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
typedef void      *xMBPSerialHandle;
/*! \brief QPS: Internal Macro do not use this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
typedef char      BOOL;
/*! \brief QPS: Internal Macro do not use this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
typedef char      BYTE;
/*! \brief QPS: Internal Macro do not use this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
typedef unsigned char UBYTE;
/*! \brief QPS: Internal Macro do not use this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
typedef unsigned char UCHAR;
/*! \brief QPS: Internal Macro do not use this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
typedef char      CHAR;
/*! \brief QPS: Internal Macro do not use this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */

typedef unsigned short USHORT;
/*! \brief QPS: Internal Macro do not use this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
typedef short     SHORT;
/*! \brief QPS: Internal Macro do not use this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
typedef unsigned long ULONG;
/*! \brief QPS: Internal Macro do not use this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
typedef long      LONG;


#if (QPS_MODBUS_STM32_USED == 1)
/*! \brief QPS: Internal Macro do not use this. User may not use this name.
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 *  */
extern TIM_HandleTypeDef htim2;

#if (MBM_SERIAL_PORT == MB_UART_1)
/*! \brief QPS: Internal Macro. This will get updated when there is any specific receive error.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
extern UART_HandleTypeDef huart1;
/*! \brief QPS: Internal Macro do not use this. User may not use this name.
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 *  */
extern UART_HandleTypeDef *huart1p;

#elif (MBM_SERIAL_PORT == MB_UART_2)
/*! \brief QPS: Internal Macro. This will get updated when there is any specific receive error.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
extern UART_HandleTypeDef huart2;
/*! \brief QPS: Internal Macro. This will get updated when there is any specific receive error.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
extern UART_HandleTypeDef *huart2p;
#elif (MBM_SERIAL_PORT == MB_UART_3)
/*! \brief QPS: Internal Macro. This will get updated when there is any specific receive error.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
extern UART_HandleTypeDef huart3;
/*! \brief QPS: Internal Macro. This will get updated when there is any specific receive error.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
extern UART_HandleTypeDef *huart3p;
#else
#error "Wrong uart config"
#endif
/*! \brief QPS: Internal Macro. This will get updated when there is any specific receive error.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
extern volatile uint32_t UART_Rx_ErrorCode;
#endif
/* ----------------------- Function prototypes ------------------------------*/
/*! \brief QPS: Internal Macro do not use this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
void  vMBPEnterCritical( void );
/*! \brief QPS: Internal Macro do not use this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
void  vMBPExitCritical( void );
/*! \brief QPS: Internal Macro do not use this.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *  */
void  vMBPAssert( void );

/*! \brief User must Call the following function from USARTX_IRQHandler(), X may be 1,2,3 depending on \ref MBM_SERIAL_PORT value used
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 *  */
void  MBMSER_IRQHandler(void);
/*! \brief User must Call the following function from TIMX_IRQHandler(). X may be 2,3,4 depending on \ref MBM_TIMER_STM
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 *  */
void  MBMTIM_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif
