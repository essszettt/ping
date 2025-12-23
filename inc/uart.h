/*-----------------------------------------------------------------------------+
|                                                                              |
| filename: uart.h                                                             |
| project:  ZX Spectrum Next - libuart                                         |
| author:   Stefan Zell                                                        |
| date:     12/14/2025                                                         |
|                                                                              |
+------------------------------------------------------------------------------+
|                                                                              |
| description:                                                                 |
|                                                                              |
| Driver for UART on ZX Spectrum Next                                          |
|                                                                              |
+------------------------------------------------------------------------------+
|                                                                              |
| Copyright (c) 12/14/2025 STZ Engineering                                     |
|                                                                              |
| This software is provided  "as is",  without warranty of any kind, express   |
| or implied. In no event shall STZ or its contributors be held liable for any |
| direct, indirect, incidental, special or consequential damages arising out   |
| of the use of or inability to use this software.                             |
|                                                                              |
| Permission is granted to anyone  to use this  software for any purpose,      |
| including commercial applications,  and to alter it and redistribute it      |
| freely, subject to the following restrictions:                               |
|                                                                              |
| 1. Redistributions of source code must retain the above copyright            |
|    notice, definition, disclaimer, and this list of conditions.              |
|                                                                              |
| 2. Redistributions in binary form must reproduce the above copyright         |
|    notice, definition, disclaimer, and this list of conditions in            |
|    documentation and/or other materials provided with the distribution.      |
|                                                                          ;-) |
+-----------------------------------------------------------------------------*/

#if !defined(__UART_H__)
  #define __UART_H__

/*============================================================================*/
/*                               Includes                                     */
/*============================================================================*/

/*============================================================================*/
/*                               Defines                                      */
/*============================================================================*/

/*============================================================================*/
/*                               Namespaces                                   */
/*============================================================================*/

/*============================================================================*/
/*                               Konstanten                                   */
/*============================================================================*/

/*============================================================================*/
/*                               Variablen                                    */
/*============================================================================*/

/*============================================================================*/
/*                               Strukturen                                   */
/*============================================================================*/

/*============================================================================*/
/*                               Typ-Definitionen                             */
/*============================================================================*/
/*!
This enumeration describes all states of a UART connection
*/
enum
{
  /*!
  UART connection is closed
  */
  UART_CLOSED = 0x00,

  /*!
  UART connection is open and ready for data transfers
  */
  UART_OPEN = 0x10
}; 

/*!
Structure to describe a UART connection
*/
typedef struct _uart
{
  /*!
  Current state of the UART connection:
  - "0x00" = not connected
  - "0x10" = connected
  */
  uint8_t uiState;

  /*!
  Backup of the UART ctrl register.
  */
  uint8_t uiCtrl;

  /*!
  Current prescaler value
  */
  uint32_t uiPrescaler;

  /*!
  Current baudrate
  */
  uint32_t uiBaudrate;

  /*!
  Init-value of the timeout counter
  */
  uint32_t uiTimeout;

  /*!
  Current value of the timeout counter
  */
  uint32_t uiTimeout_;

  /*!
  Counter for block transfers
  */
  uint16_t uiIdx;

  /*!
  Buffer to read data from UART
  */
  uint8_t uiBuffer;
} uart_t;

/*============================================================================*/
/*                               Prototypen                                   */
/*============================================================================*/
/*!
Open a uart connection.
@param pState Pointer to device structure
@param uiDevice UART device to use: 0x00 = ESP8266, ... 
@return EOK = no error
*/
uint8_t uart_open(uart_t* pState, uint8_t uiDevice);

/*!
Close a UART connection
@param pState Pointer to device structure
@return EOK = no error
*/
uint8_t uart_close(uart_t* pState);

/*!
Flush all enquened data from UART
@param pState Pointer to device structure
@return EOK = no error
*/
uint8_t uart_flush(uart_t* pState);

/*!
Set the current baudrate of the UART connection
@param pState Pointer to device structure
@param uiBaudrate Baudrate to set (default: 115200 bit/s)
@return EOK = no error
*/
uint8_t uart_set_baudrate(uart_t* pState, uint32_t uiBaudrate);

/*!
Set the current timeout of the UART connection (in [ms])
@param pState Pointer to device structure
@param uiTimeout Timeout to set (default: 2000 ms)
@return EOK = no error
*/
uint8_t uart_set_timeout(uart_t* pState, uint32_t uiTimeout);

/*!
Send one byte to UART
@param pState Pointer to device structure
@param uiData Byte to send
@return EOK = no error
*/
uint8_t uart_tx_byte(uart_t* pState, uint8_t uiData);

/*!
Read one byte from UART
@param pState Pointer to device structure
@param pData Pointer to a buffer for received byte
@return EOK = no error
*/
uint8_t uart_rx_byte(uart_t* pState, uint8_t* pData);

/*!
Send block of data to UART
@param pState Pointer to device structure
@param uiData Pointer to data to be sent
@param uiLen Length of data to send (bytes)
@return EOK = no error
*/
uint8_t uart_tx(uart_t* pState, uint8_t* uiData, uint16_t uiLen);

/*============================================================================*/
/*                               Klassen                                      */
/*============================================================================*/

/*============================================================================*/
/*                               Implementierung                              */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*----------------------------------------------------------------------------*/

#endif /* __UART_H__ */
