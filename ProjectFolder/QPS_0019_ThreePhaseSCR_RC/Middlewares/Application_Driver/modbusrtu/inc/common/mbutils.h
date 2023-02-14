#ifndef _MB_UTILS_H
#define _MB_UTILS_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif
/* ----------------------- Defines ------------------------------------------*/

/*!
 * \addtogroup QPS_MODBUS_INTERNAL_V500_RTU
 * @{
 */

/*! \brief Rounds an integer number to the next multiple of 2.QPS: Internal Macro do not change this.
 *
 * \param num The number to round.
 */
#define MB_CEIL_2( num )       			( ( ( ( num ) + 1 ) / 2 ) * 2 )

/* \brief Perform integer division where the result is rounded
 *  toward +infinity.QPS: Internal Macro do not change this.
 *
 * \param dividend Divided
 * \param divisor  Divisor
 */
#define MB_INTDIV_CEIL( dividend, divisor )		\
	( ( ( dividend ) + ( divisor ) - 1 ) / ( divisor ) )

/* \brief Perform integer division where the result is rounded
 *   toward the nearest integer number.QPS: Internal Macro do not change this.
 *
 * \param dividend Divided
 * \param divisor  Divisor
 */
#define MB_INTDIV_ROUND( dividend, divisor )	\
	( ( ( dividend ) + ( divisor ) / 2 ) / ( divisor ) )

/* \brief Perform integer division where the result is rounded
 *   toward -infinity.QPS: Internal Macro do not change this.
 *
 * \param dividend Divided
 * \param divisor  Divisor
 */
#define MB_INTDIV_FLOOR( dividend, divisor )	\
	( ( dividend ) / ( divisor ) )

/*! \brief Wrap around index in ring buffer
 */
#define MB_UTILS_RINGBUFFER_INCREMENT( idx, buffer ) do { \
    idx++; \
    if( idx >= MB_UTILS_NARRSIZE( buffer ) ) { \
        idx = 0; \
    } \
} while( 0 )

/*! \brief Calculate the number of elements in an array which size is known
 *   at compile time.
 * \param x The array.
 */

#define MB_UTILS_NARRSIZE( x ) ( sizeof( x ) / sizeof( ( x )[ 0 ] ) )
/*! \brief Checks if a handle is valid.
 *
 * This method checks if a handle is valid. It uses the ubIdx index to
 * check if the pointer points to a valid handle.
 *
 * \param pxHdl A pointer to a handle.
 * \param arxHdl An array of handles.
 * \return A boolean value.
 */
#if ( ( defined( MBM_ENABLE_FULL_API_CHECKS ) && ( MBM_ENABLE_FULL_API_CHECKS == 1 ) ) || \
      ( defined( MBS_ENABLE_FULL_API_CHECKS ) && ( MBS_ENABLE_FULL_API_CHECKS == 1 ) ) )
#define MB_IS_VALID_HDL( pxHdl, arxHdl ) \
    ( ( ( pxHdl ) != NULL ) && \
      ( ( ( size_t )( ( pxHdl )->ubIdx ) ) < MB_UTILS_NARRSIZE( arxHdl ) ) && \
      ( ( pxHdl ) == &arxHdl[ ( pxHdl )->ubIdx ] ) )
#else
#define MB_IS_VALID_HDL( pxHdl, arxHdl ) \
    ( ( ( pxHdl ) != NULL ) && ( pxHdl ) == &arxHdl[ ( pxHdl )->ubIdx ] )
#endif
/*!
 * @}
 */

/* ----------------------- Type definitions ---------------------------------*/

/* ----------------------- Function prototypes ------------------------------*/

/*! \brief Map a MODBUS exception code to an application error code.
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 *
 * \param ubMBException a MODBUS exception code from a MODBUS frame.
 * \return The MODBUS exception converted to an application error. This is used only if there is any special case like customer defined error code must be used instead of pre-defined one.
 */
    eMBErrorCode eMBExceptionToErrorcode( UBYTE ubMBException );


/*! \brief CRC16 table implementation for modbus. QPS:Customer may use this if one wants to create frame manually and wants different CRC16.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 */
USHORT
usUtlCRC16GetTab( UBYTE ubIdx );

/*! \brief On-the-fly CRC16 calculation
 *
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 * This function shall be initially called with usCRC set to 0xFFFFU.
 * If the frame has a valid CRC16 then if all bytes have been processed
 * the final calculated value is 0x0000.QPS:Customer may use this if one wants to create frame manually and wants different CRC16.
 *
 * \param usCRC Currently calculated checksum. Initialize with 0xFFFFU.
 * \param ubData New byte used to update CRC value.
 * \return Newly calculated checksum.
 */
USHORT 
prvCRC16Update( USHORT usCRC, UBYTE ubData );

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
