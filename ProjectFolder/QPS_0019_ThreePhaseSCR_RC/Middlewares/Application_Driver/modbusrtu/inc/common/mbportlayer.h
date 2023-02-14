#ifndef _MB_PORT_LAYER_H
#define _MB_PORT_LAYER_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif
/* ----------------------- Defines ------------------------------------------*/
#ifndef MB_CDECL_SUFFIX
#define MB_CDECL_SUFFIX
#endif
#ifndef MBP_MEMSET
#define MBP_MEMSET( pDest, iCh, nCnt )  memset( pDest, iCh, nCnt )
#endif

/*! \brief If the porting layer detects the end of frame. For RTU this implies the T3.5 character
 *   timeouts whereas for ASCII the porting layer checks itself on the LF character to
 *   avoid blocking on the read.
 *
 * If this macro is set the MODBUS stack does not internally detect the
 * timeout in RTU mode but depends on the porting layer to call the T35 timeout function.
 * It is passed as an additional argument to the eMBPSerialInit function.
 *
 * For a proper implementation within the serial porting layer it should be noted that.
 *   If the timeout is greater than 1.75ms for baudrates > 19200 baud or
 *   3.5 times the character time (3.5 * 1/baudrate * 11) then this signals the end of the
 *   frame and the function shall be called.
 *   Optionally and even better for full compliance to RTU is that if there is a small gap
 *   between characters exceeding 1.5 times the character time the complete frame shall
 *   be dropped. This needs to be fully implemented in the porting layer and although
 *   required by the standard it is most often ignored.
 *\ingroup QPS_MODBUS_INTERNAL_V500_RTU
 */
#ifndef MBP_SERIAL_PORT_DETECTS_TIMEOUT
#define MBP_SERIAL_PORT_DETECTS_TIMEOUT     ( 0 )
#endif

/* ----------------------- Type definitions ---------------------------------*/
/*! \brief An abstract data type used as an interface to the event queues.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 */
typedef UBYTE   xMBPEventType;

/*! \brief A callback function which should be called if the timer has
 *   expired.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *
 * \param xHdl The MODBUS stack handle passed to eMBPortTimerInit.
 * \return The return value is only important if the RTOS can make use of
 *   any additional scheduling information. This function returns \c TRUE if
 *   an internal event has been generated and a context switch should be
 *   performed.
 */
typedef         BOOL( *pbMBPTimerExpiredCB ) ( xMBHandle xHdl );

/*! \brief This function is called by the porting layer if the transmitter
 *   is ready to accept new characters.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *
 * The function should either store a new byte in the pointer \c ubValue
 * and return \c TRUE indicating that there is a character to transmit or
 * it should return \c FALSE. If the function returns \c FALSE the transmitter
 * is automatically disabled by the porting layer.
 *
 * \param xHdl The handle passed to the eMBPSerialInit function.
 * \param pubValue A pointer where the character to transmit should be stored.
 * \return \c TRUE if there are more characters to transmit. Otherwise \c
 *   FALSE and the transmitter should be disabled.
 */
typedef         BOOL( *pbMBPSerialTransmitterEmptyAPIV1CB ) ( xMBHandle xHdl, UBYTE * pubValue ) MB_CDECL_SUFFIX;

/*! \brief This function should be called when a new character has been
 *   received by the porting layer.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *
 * \param xHdl The handle passed to the eMBPortSerialInit function.
 * \param ubValue The character received. Only valid characters should be
 *   passed to the stack.
 */
typedef void    ( *pvMBPSerialReceiverAPIV1CB ) ( xMBHandle xHdl, UBYTE ubValue );

/*! \brief Called by the porting layer if the transmitter can accept new
 *   data and APIV2 is enabled.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *
 * This function should be called by the porting layer when the transmitter
 * is enabled an new character can be accepted. The MODBUS stack will write
 * up to \c usBufferMax character into the buffer \c pubBufferOut. It will
 * store the number of bytes written in the variable \c pusBufferWritten.
 *
 * \param xHdl The handle passed to the eMBPSerialInit function.
 * \param pubBufferOut A buffer of \c usBufferMax bytes.
 * \param pusBufferWritten Set by the stack to the number of bytes written.
 *
 * \return \c FALSE if no more characters will be sent. In this case the
 *  transmitter should be disabled automatically by the porting layer.
 *  Otherwise \c TRUE to indicate that there are more characters waiting.
 */
typedef         BOOL( *pbMBPSerialTransmitterEmptyAPIV2CB ) ( xMBHandle xHdl, UBYTE * pubBufferOut,
                                                              USHORT usBufferMax,
                                                              USHORT * pusBufferWritten ) MB_CDECL_SUFFIX;

/*! \brief Called by the porting layer if new data is available and API V2
 *   is enabled.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *
 * If the receiver is enabled the porting layer should call this function
 * whenever there is new data available. The number of bytes available
 * is passed in \c usBufferLen.
 *
 * \param pubBufferIn Buffer holding \c usBufferLen bytes.
 * \param usBufferLen Length of the buffer.
 */
typedef void    ( *pvMBPSerialReceiverAPIV2CB ) ( xMBHandle xHdl, const UBYTE * pubBufferIn,
                                                  USHORT usBufferLen ) MB_CDECL_SUFFIX;

/*! \brief Abstract type which points either to the API V1 or API V2
 *   functions depending on the setting of MBS_SERIAL_API_VERSION.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *
 * If API V1 is enabled a variable of this type holds a function pointer
 * of type pbMBPSerialTransmitterEmptyAPIV1CB. If API V2 is enabled
 * it holds a pointer of type pbMBPSerialTransmitterEmptyAPIV2CB.
 */
#if defined( MBP_FORCE_SERV2PROTOTYPES) && ( MBP_FORCE_SERV2PROTOTYPES == 1 )
typedef pbMBPSerialTransmitterEmptyAPIV2CB pbMBPSerialTransmitterEmptyCB;
#elif defined( MBP_FORCE_SERV1PROTOTYPES) && ( MBP_FORCE_SERV1PROTOTYPES == 1 )
typedef pbMBPSerialTransmitterEmptyAPIV1CB pbMBPSerialTransmitterEmptyCB;
#elif defined( MBP_CORRECT_FUNCPOINTER_CAST ) && ( MBP_CORRECT_FUNCPOINTER_CAST == 1 )
typedef void    ( *pbMBPSerialTransmitterEmptyCB ) ( void );
#else
typedef void   *pbMBPSerialTransmitterEmptyCB;
#endif

/*! \brief Abstract type which points either to the API V1 or API V2
 *   functions depending on the setting of MBS_SERIAL_API_VERSION.
 * \ingroup QPS_MODBUS_INTERNAL_V500_RTU
 *
 * If API V1 is enabled a variable of this type holds a function pointer
 * of type pvMBPSerialReceiverAPIV1CB. If API V2 is enabled it holds a
 * pointer of type pvMBPSerialReceiverAPIV2CB.
 */
#if defined( MBP_FORCE_SERV2PROTOTYPES) && ( MBP_FORCE_SERV2PROTOTYPES == 1 )
typedef pvMBPSerialReceiverAPIV2CB pvMBPSerialReceiverCB;
#elif defined( MBP_FORCE_SERV1PROTOTYPES) && ( MBP_FORCE_SERV1PROTOTYPES == 1 )
typedef pvMBPSerialReceiverAPIV1CB pvMBPSerialReceiverCB;
#elif defined( MBP_CORRECT_FUNCPOINTER_CAST ) && ( MBP_CORRECT_FUNCPOINTER_CAST == 1 )
typedef void    ( *pvMBPSerialReceiverCB ) ( void );
#else
typedef void   *pvMBPSerialReceiverCB;
#endif
 /* ----------------------- Function prototypes ( Timer functions ) --------- */

/*! \addtogroup QPS_MODBUS_INTERNAL_V500_RTU
 * @{
 */

/*! \brief Creates a new Timer and returns a handle to this timer.
 *
 * Timers are a generic instruction. If the timer has been started by a call
 * to eMBPTimerStart a callback should be made when the timer has expired
 * using the function pointer pbMBPTimerExpiredFN with the handle xHdl as an
 * argument.<br>
 *
 * The following semantics are important for the implementation of the porting
 * layer:
 * - If a timer has been started and it has expired a callback should be made.
 * - All timers are singleshot. Therefore it does not reenable itself.
 * - It must be possible to stop an already running timer. In this case
 *   no callback should be made.
 * - If a timer is closed all its resources should be released and the handle
 *   should no longer be valid.
 * - Starting a timer when it is already running resets its timeout.
 * - Stopping a timer multiple times has no side effects other than the
 *   disabling of the timer.
 * - All timers must start disarmed.i
 *
 * \param xTimerHdl A pointer to a timer handle. If the function returns
 *   eMBErrorCode::MB_ENOERR the handle should be valid.
 * \param usTimeOut1ms Timeout in milliseconds.
 * \param pbMBPTimerExpiredFN A pointer to a function.
 * \param xHdl A MODBUS handle.
 *
 * \return eMBErrorCode::MB_ENOERR if a new timer has been created.
 *   eMBErrorCode::MB_EINVAL if either xTimerHdl equals \c NULL or
 *   pbMBPTimerExpiredFN equals \c NULL or xHdl equals MBP_HDL_INVALID. In
 *   case no more timers can be created the function should return
 *   eMBErrorCode::MB_ENORES. All other errors should be mapped
 *   to eMBErrorCode::MB_EPORTERR.
 */
eMBErrorCode    eMBPTimerInit( xMBPTimerHandle * xTimerHdl, USHORT usTimeOut1ms,
                               pbMBPTimerExpiredCB pbMBPTimerExpiredFN, xMBHandle xHdl );

/*! \brief Releases this timer.
 *
 * \param xTimerHdl A timer handle. If not valid the function should do
 *   nothing.
 */
void            vMBPTimerClose( xMBPTimerHandle xTimerHdl );

/*! \brief Changes the timeout for a timer.
 *
 * \param xTimerHdl A valid timer handle.
 * \param usTimeOut1ms The new timeout in milliseconds.
 * \return eMBErrorCode::MB_ENOERR if the timeout has been changed.
 *   eMBErrorCode::MB_EINVAL if the timer handle was not valid. All other
 *   errors should be mapped to eMBErrorCode::MB_EPORTERR.
 */
eMBErrorCode    eMBPTimerSetTimeout( xMBPTimerHandle xTimerHdl, USHORT usTimeOut1ms );

/*! \brief Starts the timer.
 *
 * When a timer has been started the timer module should perform a callback
 * after the timeout for this timer has been elapsed or the timer has been
 * disabled before it has expired.
 *
 * \param xTimerHdl A valid timer handle.
 * \return eMBErrorCode::MB_ENOERR if the timer has been started.
 *   eMBErrorCode::MB_EINVAL if the timer handle was not valid. All other
 *   errors should be mapped to  eMBErrorCode::MB_EPORTERR.
 */
eMBErrorCode    eMBPTimerStart( xMBPTimerHandle xTimerHdl );

/*! \brief Stops the timer.
 *
 * If the timer has been stopped no callbacks should be performed.
 *
 * \param xTimerHdl A valid timer handle.
 * \return eMBErrorCode::MB_ENOERR if the timer has been stopped.
 *   eMBErrorCode::MB_EINVAL if the timer handle was not valid. All other
 *   errors should be mapped to eMBErrorCode::MB_EPORTERR.
 */
eMBErrorCode    eMBPTimerStop( xMBPTimerHandle xTimerHdl );

/*! @} */

/* ----------------------- Function prototypes ( Event Handlers )------------*/

/*! \addtogroup QPS_MODBUS_INTERNAL_V500_RTU
 * @{
 */

/*! \brief Creates a new event handler.
 *
 * An event handler allows the posting of an event. At most one event is
 * posted at once and this event can be retrieved later by calling
 * bMBPEventGet. The function bMBPEventGet should return \c FALSE if
 * no event is available.
 *
 * \note If an RTOS is available this function should be implemented by
 *   message queues with a depth of one. If an event is posted a message
 *   should be placed in the queue. The function bMBPEventGet should
 *   try to get a message from the queue and it is allowed to block.
 *
 * \note If no RTOS is used the function eMBPEventPost could simply
 *   place the event into a global variable. The function bMBPEventGet
 *   checks if an event has been posted and has not been processed (
 *   Processed means it has been returned by a previous call). If no
 *   event is ready it should return FALSE. Otherwise the event is removed
 *  and the function returns TRUE.
 *
 * \param pxEventHdl A pointer to an event handle. If the function returns
 *   eMBErrorCode::MB_ENOERR the value should contain a valid event handle.
 * \return eMBErrorCode::MB_ENOERR if the event handle has been created.
 *   eMBErrorCode::MB_EINVAL if pxEventHdl was \c NULL. All other errors
 *   should be mapped to eMBErrorCode::MB_EPORTERR.
 */
eMBErrorCode    eMBPEventCreate( xMBPEventHandle * pxEventHdl );

/*! \brief Post a event to an event queue.
 *
 * \param xEventHdl A valid event handle.
 * \param xEvent The event to post. Any previous event will be overwritten.
 * \return eMBErrorCode::MB_ENOERR if the event has been posted.
 *   eMBErrorCode::MB_EINVAL if the handle was not valid. All other errors
 *   should be mapped to eMBErrorCode::MB_EPORTERR.
 */
eMBErrorCode    eMBPEventPost( const xMBPEventHandle xEventHdl, xMBPEventType xEvent );

/*! \brief Get an event from the queue.
 *
 * \param xEventHdl A valid handle created by eMBPortEventCreate.
 * \param pxEvent If the function returns TRUE the pointer is updated
 *   to hold the stored event. Otherwise it is left unchanged.
 * \return TRUE if an event was available in the queue. Otherwise FALSE.
 */
BOOL            bMBPEventGet( const xMBPEventHandle xEventHdl, xMBPEventType * pxEvent );

/*! \brief Releases the event queue.
 *
 * \param xEventHdl A event handle to remove.
 */
void            vMBPEventDelete( xMBPEventHandle xEventHdl );

/*! @} */

/* ----------------------- Function prototypes ( Serial functions ) ---------*/

/*! \addtogroup QPS_MODBUS_INTERNAL_V500_RTU
 *
 * @{
 */

/*! \brief This function should initialize a new serial port and return a
 *   handle to it.
 *
 * \note The serial port should start in the disabled mode. I.e. it should
 *   behave the same as if the transmitter and the receiver has been disabled
 *   by the appropriate calls.
 *
 * \param pxSerialHdl A pointer to a serial handle. If the function returns
 *   MBP_ENOERR this value must hold a valid handle.
 * \param ucPort A porting layer dependent number to distinguish between
 *   different serial ports.
 * \param ulBaudRate The baudrate. For example 38400.
 * \param ucDataBits Number of databits. Values used are 8 and 7.
 * \param eParity The parity.
 * \param ucStopBits Either one or two stopbits.
 * \param xMBHdl A MODBUS stack handle. This should be passed in every
 *   callbacks made by the serial porting layer.
 * \param bFrameTimeoutFN If MBP_SERIAL_PORT_DETECTS_TIMEOUT is active and
 *   this argument is not equal to NULL the porting layer needs to detect the
 *   timeouts. See the documentation of MBP_SERIAL_PORT_DETECTS_TIMEOUT.
 * \param eMode If the serial port detects the timeout the argument includes 
 *   wheter ASCII or RTU is used.
 * \return eMBErrorCode::MB_ENOERR if a new serial port instances has been
 *   created. If pxSerialHdl equals \c NULL or one of the arguments is not
 *   valid it should return eMBErrorCode::MB_EINVAL. All other errors should
 *   be mapped to eMBErrorCode::MB_EPORTERR.
 */
eMBErrorCode    eMBPSerialInit( xMBPSerialHandle * pxSerialHdl, UCHAR ucPort, ULONG ulBaudRate,
                                UCHAR ucDataBits, eMBSerialParity eParity, UCHAR ucStopBits, xMBHandle xMBHdl
#if MBP_SERIAL_PORT_DETECTS_TIMEOUT == 1
								, pbMBPTimerExpiredCB pbFrameTimeoutFN, eMBSerialMode eMode
#endif
                                );

/*! \brief Close a serial port.
 *
 * This function should release all resources used by this instance such that
 * it can be used again.
 *
 * \param xSerialHdl A valid handle for a serial port.
 * \return eMBErrorCode::MB_ENOERR If the port has been released.
 *   eMBErrorCode::MB_EAGAIN if the function should be called again because
 *   a shutdown is not possible right now. eMBErrorCode::MB_EINVAL if the
 *   handle is not valid. All other errors should be mapped to
 *   eMBErrorCode::MB_EPORTERR.
 */
eMBErrorCode    eMBPSerialClose( xMBPSerialHandle xSerialHdl );

/*! \brief Enables the transmitter and registers a callback or disables it.
 *
 * After the transmitter has been enabled the callback function should be
 * called until it returns \c FALSE indicating that no more characters should
 * be transmitted.
 *
 * \param xSerialHdl A valid handle for a serial port.
 * \param pbMBPTransmitterEmptyFN A pointer to the callback function or \c
 *   NULL if the transmitter should be disabled.
 * \return eMBErrorCode::MB_ENOERR is the transmitter has been enabled.
 *   eMBErrorCode::MB_EINVAL if the handle is not valid. All other errors
 *   should be mapped to eMBErrorCode::MB_EPORTERR.
 */
eMBErrorCode    eMBPSerialTxEnable( xMBPSerialHandle xSerialHdl,
                                    pbMBPSerialTransmitterEmptyCB pbMBPTransmitterEmptyFN );

/*! \brief Enables the receiver and registers a callback or disables it.
 *
 * After the receiver has been enabled the callback function should be
 * called for every new character. Only valid characters should be passed
 * to the stack.
 *
 * \param xSerialHdl A valid handle for a serial port.
 * \param pvMBPReceiveFN  A pointer to the callback function or \c NULL if
 *   the receiver should be disabled.
 * \return eMBErrorCode::MB_ENOERR is the receiver has been disabled.
 *   eMBErrorCode::MB_EINVAL if the handle is not valid. All other errors
 *   should be mapped to eMBErrorCode::MB_EPORTERR.
 */
eMBErrorCode    eMBPSerialRxEnable( xMBPSerialHandle xSerialHdl, pvMBPSerialReceiverCB pvMBPReceiveFN );

/*! @} */

/* ----------------------- Function prototypes ( Library ) ------------------*/
/*! \addtogroup QPS_MODBUS_INTERNAL_V500_RTU
 * @{
 */

/*! \brief If the advanced startup/shutdown functionallity has been
 *   enabled this function is called when the first TCP or RTU/ASCII
 *   instance is created.
 *
 * This functionallity can be enabled by setting the macro
 * MBP_ADVA_STARTUP_SHUTDOWN_ENABLED to 1 in the file mbmconfig.h or mbsconfig.h.
 */
void            vMBPLibraryLoad( void );

/*! \brief If the advanced startup/shutdown functionallity has been
 *   enabled this function is called when the last TCP or RTU/ASCII
 *   has been removed.
 *
 * This functionallity can be enabled by setting the macro
 * MBP_ADVA_STARTUP_SHUTDOWN_ENABLED to 1 in the file mbmconfig.h or mbsconfig.h.
 */
void            vMBPLibraryUnload( void );

/*! @} */

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
