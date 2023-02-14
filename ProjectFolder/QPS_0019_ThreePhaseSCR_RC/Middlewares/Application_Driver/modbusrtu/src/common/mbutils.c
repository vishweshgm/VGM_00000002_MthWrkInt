/* 
 * MODBUS Slave Library: A portable MODBUS slave for MODBUS ASCII/RTU/TCP.
 * Copyright (c) 2007 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * $Id: mbutils.c,v 1.2 2008-03-06 22:01:48 cwalter Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "mbport.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "common/mbtypes.h"
#include "common/mbframe.h"
#include "common/mbutils.h"

/* ----------------------- Platform includes --------------------------------*/

/* ----------------------- Modbus includes ----------------------------------*/

/* ----------------------- Defines ------------------------------------------*/

/* ----------------------- Type definitions ---------------------------------*/

/* ----------------------- Static variables ---------------------------------*/

/* ----------------------- Static functions ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/
USHORT
usUtlCRC16GetTab( UBYTE ubIdx )
{
    USHORT          usCRC16;
    USHORT          usC;
    UBYTE           i;

    usCRC16 = 0;
    usC = ( USHORT ) ubIdx;

    for( i = 0; i < 8; i++ )
    {

        if( ( usCRC16 ^ usC ) & 0x0001 )
        {
            usCRC16 = ( usCRC16 >> 1 ) ^ ( USHORT ) 0xA001U;
        }
        else
        {
            usCRC16 = usCRC16 >> 1;
        }
        usC = usC >> 1;
    }
    return usCRC16;
}

USHORT
prvCRC16Update( USHORT usCRC, UBYTE ubData )
{
    UBYTE           ubCRCLow = usCRC & 0xFF;
    UBYTE           ubCRCHigh = usCRC >> 8U;

    UBYTE           ubIndex = ubCRCLow ^ ubData;
    USHORT          usCRCTableValue = usUtlCRC16GetTab( ubIndex );

    ubCRCLow = ubCRCHigh ^ ( UBYTE ) ( usCRCTableValue & 0xFF );
    ubCRCHigh = ( UBYTE ) ( usCRCTableValue >> 8U );

    return ( ( USHORT ) ubCRCHigh << 8U ) | ( USHORT ) ubCRCLow;
}

eMBErrorCode
eMBExceptionToErrorcode( UBYTE eMBPDUException )
{
    eMBErrorCode    eStatus = MB_EIO;

    switch ( eMBPDUException )
    {
    case MB_PDU_EX_ILLEGAL_FUNCTION:
        eStatus = MB_EX_ILLEGAL_FUNCTION;
        break;
    case MB_PDU_EX_ILLEGAL_DATA_ADDRESS:
        eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;
        break;
    case MB_PDU_EX_ILLEGAL_DATA_VALUE:
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
        break;
    case MB_PDU_EX_SLAVE_DEVICE_FAILURE:
        eStatus = MB_EX_SLAVE_DEVICE_FAILURE;
        break;
    case MB_PDU_EX_ACKNOWLEDGE:
        eStatus = MB_EX_ACKNOWLEDGE;
        break;
    case MB_PDU_EX_SLAVE_BUSY:
        eStatus = MB_EX_SLAVE_BUSY;
        break;
    case MB_PDU_EX_MEMORY_PARITY_ERROR:
        eStatus = MB_EX_MEMORY_PARITY_ERROR;
        break;
    case MB_PDU_EX_GATEWAY_PATH_UNAVAILABLE:
        eStatus = MB_EX_GATEWAY_PATH_UNAVAILABLE;
        break;
    case MB_PDU_EX_GATEWAY_TARGET_FAILED:
        eStatus = MB_EX_GATEWAY_TARGET_FAILED;
        break;
    default:
        break;
    }
    return eStatus;
}
