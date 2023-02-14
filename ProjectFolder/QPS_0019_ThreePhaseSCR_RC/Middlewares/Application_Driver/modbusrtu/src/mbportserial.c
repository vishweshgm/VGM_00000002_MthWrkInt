/* 
 * MODBUS Library: Skeleton port
 * Copyright (c) 2008 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * $Id: mbportserial.c,v 1.1 2008-04-06 07:46:23 cwalter Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include <stdlib.h>

/* ----------------------- Platform includes --------------------------------*/
#include "mbport.h"
#include "stm32f4xx_hal.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "common/mbtypes.h"
#include "common/mbportlayer.h"
#include "common/mbframe.h"
#include "common/mbutils.h"

/* ----------------------- Defines ------------------------------------------*/


#define HDL_RESET( x ) do { \
	( x )->ubIdx = IDX_INVALID; \
	( x )->pbMBPTransmitterEmptyFN = NULL; \
	( x )->pvMBPReceiveFN = NULL; \
	( x )->xMBMHdl = MB_HDL_INVALID; \
} while( 0 );

/* ----------------------- Type definitions ---------------------------------*/
typedef struct
{
    UBYTE           ubIdx;
    pbMBPSerialTransmitterEmptyAPIV1CB pbMBPTransmitterEmptyFN;
    pvMBPSerialReceiverAPIV1CB pvMBPReceiveFN;
    xMBHandle       xMBMHdl;
} xSerialHandle;


UART_HandleTypeDef huart1;
UART_HandleTypeDef *huart1p;
volatile uint32_t UART_Rx_ErrorCode;
/* ----------------------- Static variables ---------------------------------*/
STATIC xSerialHandle xSerialHdls[MAX_SERIL_HDLS];
STATIC BOOL     bIsInitalized = FALSE;
UBYTE recvBufferu8[256];

/* ----------------------- Static functions ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/

eMBErrorCode
eMBPSerialInit( xMBPSerialHandle * pxSerialHdl, UCHAR ucPort, ULONG ulBaudRate,
                UCHAR ucDataBits, eMBSerialParity eParity, UCHAR ucStopBits, xMBHandle xMBMHdl )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    UBYTE           ubIdx;
    uint32_t tmpreg;
    uint32_t pclk;
    MBP_ENTER_CRITICAL_SECTION(  );

    if( !bIsInitalized )
    {
        for( ubIdx = 0; ubIdx < MB_UTILS_NARRSIZE( xSerialHdls ); ubIdx++ )
        {
            HDL_RESET( &xSerialHdls[ubIdx] );
        }
    }

    if( NULL == pxSerialHdl )
    {
    	eStatus = MB_EINVAL;
    }
    else if(( IDX_INVALID == xSerialHdls[0].ubIdx ) )
    {
    	if( ( ulBaudRate > UART_BAUDRATE_MIN ) && ( ulBaudRate < UART_BAUDRATE_MAX ) && ( MB_HDL_INVALID != xMBMHdl ) )
    	{
    		RS_485_UART_INIT(  );
    		RS_485_UART_ENABLE_TX(  );
    		huart1p = &huart1;
    		huart1.Init.Parity = UART_PARITY_NONE;
    		huart1.Init.WordLength = UART_WORDLENGTH_8B;
    		huart1.Init.StopBits = UART_STOPBITS_1;
    		huart1.Instance = USART1;
    		huart1.Init.BaudRate = 9600;
    		huart1.Init.Mode = UART_MODE_TX_RX;
    		huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    		huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    		if (HAL_UART_Init(&huart1) != HAL_OK) Error_Handler();
            *pxSerialHdl = &xSerialHdls[0];
            xSerialHdls[0].ubIdx = 0;
            xSerialHdls[0].xMBMHdl = xMBMHdl;
            eStatus = MB_ENOERR;
        }
        else
        {
            eStatus = MB_EINVAL;
        }
    }
    else
    {
        eStatus = MB_ENORES;
    }
    MBP_EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBPSerialClose( xMBPSerialHandle xSerialHdl )
{
    eMBErrorCode    eStatus = MB_EINVAL;
    xSerialHandle  *pxSerialIntHdl = xSerialHdl;

    MBP_ENTER_CRITICAL_SECTION(  );
    if( MB_IS_VALID_HDL( pxSerialIntHdl, xSerialHdls ) )
    {
        if( ( pxSerialIntHdl->pbMBPTransmitterEmptyFN == NULL ) && ( pxSerialIntHdl->pvMBPReceiveFN == NULL ) )
        {
        	__HAL_UART_DISABLE(huart1p);
        	HDL_RESET( pxSerialIntHdl );
            eStatus = MB_ENOERR;
        }
        else
        {
            eStatus = MB_EAGAIN;
        }
    }
    MBP_EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode eMBPSerialTxEnable( xMBPSerialHandle xSerialHdl, pbMBPSerialTransmitterEmptyCB pbMBPTransmitterEmptyFN )
{
	eMBErrorCode    eStatus = MB_EINVAL;
	xSerialHandle  *pxSerialIntHdl = xSerialHdl;

	MBP_ENTER_CRITICAL_SECTION(  );
	if( MB_IS_VALID_HDL( pxSerialIntHdl, xSerialHdls ) )
	{
		eStatus = MB_ENOERR;
		if( NULL != pbMBPTransmitterEmptyFN )
		{
			MBP_ASSERT( NULL == pxSerialIntHdl->pbMBPTransmitterEmptyFN );
			pxSerialIntHdl->pbMBPTransmitterEmptyFN = pbMBPTransmitterEmptyFN;
			RS_485_UART_ENABLE_TX(  );
			__HAL_UART_ENABLE_IT(huart1p, UART_IT_TXE);
		}
		else
		{
			pxSerialIntHdl->pbMBPTransmitterEmptyFN = NULL;
			__HAL_UART_DISABLE_IT(huart1p, UART_IT_TXE);
		}
	}
	MBP_EXIT_CRITICAL_SECTION(  );
	return eStatus;
}

eMBErrorCode eMBPSerialRxEnable( xMBPSerialHandle xSerialHdl, pvMBPSerialReceiverCB pvMBPReceiveFN )
{
	eMBErrorCode    eStatus = MB_EINVAL;
	xSerialHandle  *pxSerialIntHdl = xSerialHdl;

	MBP_ENTER_CRITICAL_SECTION(  );
	if( MB_IS_VALID_HDL( pxSerialIntHdl, xSerialHdls ) )
	{
		eStatus = MB_ENOERR;
		if( NULL != pvMBPReceiveFN )
		{
			MBP_ASSERT( NULL == pxSerialIntHdl->pvMBPReceiveFN );
			pxSerialIntHdl->pvMBPReceiveFN = pvMBPReceiveFN;
			__HAL_UART_ENABLE_IT(huart1p, UART_IT_RXNE);
//			__HAL_UART_ENABLE_IT(huart1p, UART_IT_PE);
//			__HAL_UART_ENABLE_IT(huart1p, UART_IT_ERR);
			RS_485_UART_DISABLE_TX(  );
		}
		else
		{
			pxSerialIntHdl->pvMBPReceiveFN = NULL;
			__HAL_UART_DISABLE_IT(huart1p, UART_IT_RXNE);
//			__HAL_UART_DISABLE_IT(huart1p, UART_IT_PE);
//			__HAL_UART_DISABLE_IT(huart1p, UART_IT_ERR);
			RS_485_UART_ENABLE_TX(  );
		}
	}
	MBP_EXIT_CRITICAL_SECTION(  );
	return eStatus;
}

STATIC void prrvUSARTTxISR( void )
{
	BOOL            bHasMoreData = TRUE;
	UBYTE           ubTxByte;

	if( NULL != xSerialHdls[0].pbMBPTransmitterEmptyFN )
	{
		bHasMoreData = xSerialHdls[0].pbMBPTransmitterEmptyFN( xSerialHdls[0].xMBMHdl, &ubTxByte );
	}

	if( !bHasMoreData )
	{
		xSerialHdls[0].pbMBPTransmitterEmptyFN = NULL;
		RS_485_UART_DISABLE_TX(  );
		__HAL_UART_DISABLE_IT(huart1p, UART_IT_TC);
	}
	else
	{
		if (huart1.Init.WordLength == UART_WORDLENGTH_9B)
		{
			huart1.Instance->DR = (uint16_t)(ubTxByte & (uint16_t)0x01FF);
		}
		else
		{
			huart1.Instance->DR = (uint8_t)(ubTxByte & (uint8_t)0x00FF);
		}
	}
	return;
}

STATIC void prrvUSARTRxISR( void )
{
	UBYTE           ubUDR = 0;
	if (huart1.Init.WordLength == UART_WORDLENGTH_9B)
	{
		ubUDR = (uint16_t)(huart1.Instance->DR & (uint16_t)0x00FF);
	}
	else
	{
		if (huart1.Init.Parity == UART_PARITY_NONE)
		{
			ubUDR = (uint8_t)(huart1.Instance->DR & (uint8_t)0x00FF);
		}
		else
		{
			ubUDR = (uint8_t)(huart1.Instance->DR & (uint8_t)0x007F);
		}
	}

	MBP_ASSERT( IDX_INVALID != xSerialHdls[0].ubIdx );
	if( NULL != xSerialHdls[0].pvMBPReceiveFN )
	{
		xSerialHdls[0].pvMBPReceiveFN( xSerialHdls[0].xMBMHdl, ubUDR );
		/*bMBMSerialRTUReceiverAPIV1CB*/
	}
	return;
}

void MBMSER_IRQHandler(void)
{
	uint32_t isrflags   = READ_REG(huart1.Instance->SR);
	uint32_t cr1its     = READ_REG(huart1.Instance->CR1);
	uint32_t cr3its     = READ_REG(huart1.Instance->CR3);

	uint32_t errorflags = 0x00U;
	errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
	if (errorflags == RESET)
	{
		if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
		{
			prrvUSARTRxISR();
			return;
		}
	}
	else
	{
		UART_Rx_ErrorCode = errorflags;
	}

	if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
	{
		prrvUSARTTxISR();
		return;
	}


	if (((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
	{
		prrvUSARTTxISR();
		return;
	}
}
