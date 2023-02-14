#ifndef _MB_TYPES_H
#define _MB_TYPES_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

/*! \addtogroup QPS_MODBUS_CUSTOMER_V500_RTU
 * @{
 */

/* ----------------------- Defines ------------------------------------------*/

/*! \brief An invalid MASTER handle. */
#define MB_HDL_INVALID                         ( NULL )

/*! \brief Tests if an error code of the type eMBErrorCode is an exception.
 *
 * \param eStatus The error code returned by any of the API functions.
 */
#define MB_ERROR_IS_EXCEPTION( eStatus )       \
    ( ( ( eStatus ) >= MB_EX_ILLEGAL_FUNCTION ) && \
      ( ( eStatus ) <= MB_EX_GATEWAY_TARGET_FAILED ) ? TRUE : FALSE )

/* ----------------------- Type definitions ---------------------------------*/

/*! \brief A handle used to hold either MASTER or SLAVE stack handles. */
typedef void   *xMBHandle;

/*!
 * \brief Error code reported by the protocol stack.
 */
typedef enum
{
    MB_ENOERR = 0,              /*!< No error. */
    MB_ENOREG = 1,              /*!< Illegal register address. */
    MB_EINVAL = 2,              /*!< Illegal argument. */
    MB_EPORTERR = 3,            /*!< Porting layer error. */
    MB_ENORES = 4,              /*!< Insufficient resources. */
    MB_EIO = 5,                 /*!< I/O error. */
    MB_EILLSTATE = 6,           /*!< Protocol stack in illegal state. */
    MB_EAGAIN = 7,              /*!< Retry I/O operation. */
    MB_ETIMEDOUT = 8,           /*!< Timeout error occurred. */
    MB_EX_ILLEGAL_FUNCTION = 10,        /*!< Illegal function exception. */
    MB_EX_ILLEGAL_DATA_ADDRESS = 11,    /*!< Illegal data address. */
    MB_EX_ILLEGAL_DATA_VALUE = 12,      /*!< Illegal data value. */
    MB_EX_SLAVE_DEVICE_FAILURE = 13,    /*!< Slave device failure. */
    MB_EX_ACKNOWLEDGE = 14,     /*!< Slave acknowledge. */
    MB_EX_SLAVE_BUSY = 15,      /*!< Slave device busy. */
    MB_EX_MEMORY_PARITY_ERROR = 16,     /*!< Memory parity error. */
    MB_EX_GATEWAY_PATH_UNAVAILABLE = 17,        /*!< Gateway path unavailable. */
    MB_EX_GATEWAY_TARGET_FAILED = 18    /*!< Gateway target device failed to respond. */
} eMBErrorCode;

/*!
 * \brief Modes supported by the serial protocol stack.
 */
typedef enum
{
    MB_RTU,                     /*!< RTU transmission mode. */
    MB_ASCII                    /*!< ASCII transmission mode. */
} eMBSerialMode;

/*!
 * \brief Parity for serial transmission mode.
 */
typedef enum
{
    MB_PAR_ODD,                 /*!< ODD parity. */
    MB_PAR_EVEN,                /*!< Even parity. */
    MB_PAR_NONE                 /*!< No parity. */
} eMBSerialParity;
/* ----------------------- Function prototypes ------------------------------*/

/*! @} */

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
