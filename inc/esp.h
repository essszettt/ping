/*-----------------------------------------------------------------------------+
|                                                                              |
| filename: esp.h                                                              |
| project:  ZX Spectrum Next - PING                                            |
| author:   Stefan Zell                                                        |
| date:     12/14/2025                                                         |
|                                                                              |
+------------------------------------------------------------------------------+
|                                                                              |
| description:                                                                 |
|                                                                              |
| Driver for ESP8266 on ZX Spectrum Next                                       |
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

#if !defined(__ESP_H__)
  #define __ESP_H__

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
  ESP_CLOSED = 0x00,
  ESP_OPEN = 0x10
}; 

/*!
Structure to describe a ESP8266 connection
*/
typedef struct _esp
{
  uint8_t uiState;
  uart_t tUart;

  uint8_t uiPrev;
  uint8_t uiCurr;
  const char_t* acEnd;
  char_t* acIndex;

} esp_t;

/*============================================================================*/
/*                               Prototypen                                   */
/*============================================================================*/
/*!
Open connection to ESP8266
@param pState Pointer to device structure
@return EOK = no error
*/
uint8_t esp_open(esp_t* pState);

/*!
Close connection to ESP8266
@param pState Pointer to device structure
@return EOK = no error
*/
uint8_t esp_close(esp_t* pState);

/*!
Sending a AT-command to ESP8266. The command must be terminated with CR+LF.
@param acBuffer Pointer to a buffer containing the data to send (incl. CR+LF)
@return EOK = no error; EBREAK = user break
*/
uint8_t esp_transmit(esp_t* pState, char_t* acBuffer);

/*!
Reading one line in textmode from ESP8266.
@param acBuffer Pointer to a buffer to copy the line to
@param uiSize Size of the buffer [byte]
@return EOK = no error; EBREAK = user break
*/
uint8_t esp_receive(esp_t* pState, char_t* acBuffer, uint16_t uiSize);

/*============================================================================*/
/*                               Klassen                                      */
/*============================================================================*/

/*============================================================================*/
/*                               Implementierung                              */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*----------------------------------------------------------------------------*/

#endif /* __ESP_H__ */
