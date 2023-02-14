#ifndef _MB_FRAME_H
#define _MB_FRAME_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

/*!
 * \addtogroup QPS_MODBUS_INTERNAL_V500_RTU
 * @{
 */

/* ----------------------- Defines ------------------------------------------*/
#define MB_PDU_SIZE_MAX             ( 253 )     /*!< Maximum size of a PDU. QPS: Internal Macro do not change this*/
#define MB_PDU_SIZE_MIN             ( 1 )       /*!< Function Code QPS: Internal macro do not change this*/
#define MB_PDU_FUNC_OFF             ( 0 )       /*!< Offset of function code in PDU. QPS: Internal macro do not change this*/
#define MB_PDU_EX_CODE_OFF          ( 1 )       /*!< Offset of exception code in PDU. QPS: Internal macro do not change this*/
#define MB_PDU_DATA_OFF             ( 1 )       /*!< Offset for response data in PDU. QPS: Internal macro do not change this*/
#define MB_PDU_EX_RESP_SIZE         ( 2 )       /*!< Size of an exception response. QPS: Internal macro do not change this*/
#define MB_SER_BROADCAST_ADDR       ( 0 )       /*!< MODBUS serial broadcast address. QPS: Internal macro do not change this*/
#define MB_SER_SLAVE_ADDR_MIN       ( 1 )       /*!< Smallest valid serial address. QPS: Internal macro do not change this*/
#define MB_SER_SLAVE_ADDR_MAX       ( 247 )     /*!< Biggest valid serial address. QPS: Internal macro do not change this*/

/*! \brief Test if a MODBUS function byte in a MODBUS PDU is an exception. QPS: Internal Macro do not change this.
 *
 * \param ubFuncByte The byte in the MODBUS PDU.
 * \return \c TRUE if this function code is an exception. Otherwise \c FALSE.
 */
#define MB_PDU_FUNC_ISEXCEPTION( ubFuncByte ) ( ( ( ubFuncByte ) & 0x80 ) != 0 )

/*! \brief Test if a MODBUS function byte in a MODBUS PDU is an exception.
 *   for a specific exception. QPS: Internal Macro do not change this.
 *
 * \param ubFuncByte The byte in the MODBUS PDU.
 * \param ubFuncExpected Any MODBUS function code less than 128.
 * \return \c TRUE if this function code is an exception. Otherwise \c FALSE.
 */
#define MB_PDU_FUNC_ISEXCEPTION_FOR( ubFuncByte, ubFuncExpected ) \
    ( ( ubFuncByte ) == ( ( ubFuncExpected ) + 0x80 ) )

/*! \brief Check if the slave address ucAddr can be used for a transaction
 *  requiring an reply.QPS: Internal Macro do not change this.
 *
 * \param ucAddr the slave address.
 * \return \c TRUE if ucAddr is a valid slave address but not the broadcast
 *   address.
 */
#define MB_IS_VALID_READ_ADDR( ucAddr ) \
    ( ( ( ucAddr ) >= MB_SER_SLAVE_ADDR_MIN ) && ( ( ucAddr ) <= MB_SER_SLAVE_ADDR_MAX ) )

/*! \brief Check if the slave address ucAddr can be used for a transaction
    *   not necessarily requiring an reply.QPS: Internal Macro do not change this.
    *
    * \param ucAddr the slave address.
    * \return \c TRUE if ucAddr is a valid slave address including the broadcast
    *   address.
    */
#define MB_IS_VALID_WRITE_ADDR( ucAddr ) \
    ( ( ( ucAddr ) == MB_SER_BROADCAST_ADDR ) || MB_IS_VALID_READ_ADDR( ucAddr ) )

/* ----------------------- Type definitions ---------------------------------*/

/*! \brief MODBUS exception defined for the MODBUS PDU.
 * \ingroup QPS_MODBUS_CUSTOMER_V500_RTU
 */
typedef enum
{
    MB_PDU_EX_NONE = 0x00,
    MB_PDU_EX_ILLEGAL_FUNCTION = 0x01,
    MB_PDU_EX_ILLEGAL_DATA_ADDRESS = 0x02,
    MB_PDU_EX_ILLEGAL_DATA_VALUE = 0x03,
    MB_PDU_EX_SLAVE_DEVICE_FAILURE = 0x04,
    MB_PDU_EX_ACKNOWLEDGE = 0x05,
    MB_PDU_EX_SLAVE_BUSY = 0x06,
    MB_PDU_EX_NOT_ACKNOWLEDGE = 0x07,
    MB_PDU_EX_MEMORY_PARITY_ERROR = 0x08,
    MB_PDU_EX_GATEWAY_PATH_UNAVAILABLE = 0x0A,
    MB_PDU_EX_GATEWAY_TARGET_FAILED = 0x0B
} eMBException;

/* ----------------------- Function prototypes ------------------------------*/

/*!
 * @}
 */

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
