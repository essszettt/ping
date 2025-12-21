/*-----------------------------------------------------------------------------+
|                                                                              |
| filename: esp.c                                                              |
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

/*============================================================================*/
/*                               Includes                                     */
/*============================================================================*/
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "libzxn.h"
#include "uart.h"
#include "esp.h"

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

/*============================================================================*/
/*                               Prototypen                                   */
/*============================================================================*/

/*============================================================================*/
/*                               Klassen                                      */
/*============================================================================*/

/*============================================================================*/
/*                               Implementierung                              */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* esp_open()                                                                 */
/*----------------------------------------------------------------------------*/
uint8_t esp_open(esp_t* pState)
{
  if (0 != pState)
  {
    memset(pState, 0, sizeof(pState));

    uart_open(&pState->tUart, 0x00);
    uart_set_baudrate(&pState->tUart, 115200);
    uart_flush(&pState->tUart);

    pState->uiState = ESP_OPEN;
    return EOK;
  }

  return EINVAL;
}


/*----------------------------------------------------------------------------*/
/* esp_close()                                                                */
/*----------------------------------------------------------------------------*/
uint8_t esp_close(esp_t* pState)
{
  if ((0 != pState) && (ESP_OPEN == pState->uiState))
  {
    uart_close(&pState->tUart);
    pState->uiState = ESP_CLOSED; 
    return EOK;
  }

  return EINVAL;
}


/*----------------------------------------------------------------------------*/
/* esp_transmit()                                                             */
/*----------------------------------------------------------------------------*/
uint8_t esp_transmit(esp_t* pState, char_t* acBuffer)
{
  if ((0 != pState) && (ESP_OPEN == pState->uiState))
  {
    if (acBuffer)
    {
      while (*acBuffer)
      {
        if (EOK != uart_tx_byte(&pState->tUart, *acBuffer++))
        {
          return EBREAK;
        }
      }

      return EOK;
    }
  }

  return EINVAL;
}


/*----------------------------------------------------------------------------*/
/* esp_receive()                                                              */
/*----------------------------------------------------------------------------*/
uint8_t esp_receive(esp_t* pState, char_t* acBuffer, uint16_t uiSize)
{
  uint8_t uiReturn = EINVAL;

  if (acBuffer && uiSize)
  {
    pState->acEnd   = acBuffer + uiSize;
    pState->acIndex = acBuffer;

    acBuffer[0] = '\0';
    pState->uiPrev = '\0';

    for (;;)
    {
      if (EOK != uart_rx_byte(&pState->tUart, &pState->uiCurr))
      {
        uiReturn = EBREAK;
        break;
      }

      if ((pState->acIndex + 1) < pState->acEnd)
      {
        *pState->acIndex = pState->uiCurr;
        ++pState->acIndex;
        *pState->acIndex = '\0';
      }
      else
      {
        uiReturn = EOVERFLOW;
      }

      if (('\r' == pState->uiPrev) && ('\n' == pState->uiCurr))
      {
        uiReturn = EOK;
        break;
      }

      pState->uiPrev = pState->uiCurr;
    }
  }
  
  return uiReturn;
}


/*----------------------------------------------------------------------------*/
/*                                                                            */
/*----------------------------------------------------------------------------*/
