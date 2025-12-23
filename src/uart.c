/*-----------------------------------------------------------------------------+
|                                                                              |
| filename: uart.c                                                             |
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

/*============================================================================*/
/*                               Includes                                     */
/*============================================================================*/
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <arch/zxn.h>
#include <input.h>
#include <input/input_zx.h>

#include "libzxn.h"
#include "uart.h"

/*============================================================================*/
/*                               Defines                                      */
/*============================================================================*/
/*!
Default value for communication timeouts (in [ms])
*/
#define uiUART_DEFAULT_TIMEOUT (1000)

/*============================================================================*/
/*                               Namespaces                                   */
/*============================================================================*/

/*============================================================================*/
/*                               Konstanten                                   */
/*============================================================================*/

/*============================================================================*/
/*                               Variablen                                    */
/*============================================================================*/
/*!
Videotimings to calculate prescaler value for required baudrate
*/
static const uint32_t g_uiVideoTiming[] =
{
  CLK_28_0,
  CLK_28_1,
  CLK_28_2,
  CLK_28_3,
  CLK_28_4,
  CLK_28_5,
  CLK_28_6,
  CLK_28_7
};

/*!
Definition of the UART control register (IO port)
{until it is added to headers}
*/
__sfr __banked __at 0x153b IO_153B;

/*============================================================================*/
/*                               Strukturen                                   */
/*============================================================================*/

/*============================================================================*/
/*                               Typ-Definitionen                             */
/*============================================================================*/

/*============================================================================*/
/*                               Prototypen                                   */
/*============================================================================*/
/*!
This function initializes the timeout counter at the start of a read/write
operation.
@param pState Pointer to device structure
*/
void uart_init_timeout(uart_t* pState);

/*!
This function decrements the timeout counter. If "0" is reached, a ETIMEOUT is
signaled.
@param pState Pointer to device structure
@return EOK = timeout running; ETIMEOUT = end reached
*/
uint8_t uart_check_timeout(uart_t* pState);

/*============================================================================*/
/*                               Klassen                                      */
/*============================================================================*/

/*============================================================================*/
/*                               Implementierung                              */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* uart_init_timeout()                                                        */
/*----------------------------------------------------------------------------*/
void uart_init_timeout(uart_t* pState)
{
  if (pState)
  {
    pState->uiTimeout_ = pState->uiTimeout;
  }
}

/*----------------------------------------------------------------------------*/
/* uart_check_timeout()                                                       */
/*----------------------------------------------------------------------------*/
uint8_t uart_check_timeout(uart_t* pState)
{
  return (--pState->uiTimeout_ ? EOK : ETIMEOUT);
}


/*----------------------------------------------------------------------------*/
/* uart_open()                                                                */
/*----------------------------------------------------------------------------*/
uint8_t uart_open(uart_t* pState, uint8_t uiDevice)
{
  if (pState)
  {
    memset(pState, 0, sizeof(pState));

    pState->uiCtrl = IO_153B & 0x40;

    /* Select UART: 0x00 = ESP, ... */
    IO_153B = uiDevice;

    pState->uiState = UART_OPEN;
    uart_set_timeout(pState, 1000);
    return EOK;
  }

  return EINVAL;
}


/*----------------------------------------------------------------------------*/
/* uart_close()                                                               */
/*----------------------------------------------------------------------------*/
uint8_t uart_close(uart_t* pState)
{
  if (pState && (UART_OPEN == pState->uiState))
  {
    IO_153B = pState->uiCtrl;

    pState->uiState = UART_CLOSED;
    return EOK;
  }

  return EINVAL;
}


/*----------------------------------------------------------------------------*/
/* uart_set_baudrate()                                                        */
/*----------------------------------------------------------------------------*/
uint8_t uart_set_baudrate(uart_t* pState, uint32_t uiBaudrate)
{
  if (pState && (UART_OPEN == pState->uiState))
  {
    pState->uiPrescaler = g_uiVideoTiming[(ZXN_READ_REG(REG_VIDEO_TIMING) & 0x07)] / uiBaudrate;

    IO_153B = (IO_153B & 0x40) | 0x10 | (uint8_t) (pState->uiPrescaler >> 14);
    IO_143B = 0x80 | (uint8_t) (pState->uiPrescaler >> 7);
    IO_143B = (uint8_t) (pState->uiPrescaler) & 0x7f;

    pState->uiBaudrate = uiBaudrate;
    return EOK;
  }

  return EINVAL;
}


/*----------------------------------------------------------------------------*/
/* uart_set_timeout()                                                         */
/*----------------------------------------------------------------------------*/
uint8_t uart_set_timeout(uart_t* pState, uint32_t uiTimeout)
{
  if (pState && (UART_OPEN == pState->uiState))
  {
    pState->uiTimeout   = (uiTimeout ? uiTimeout : uiUART_DEFAULT_TIMEOUT);
    pState->uiTimeout  *= 1000;
    pState->uiTimeout <<= zxn_getspeed();

    return EOK;
  }

  return EINVAL;
}


/*----------------------------------------------------------------------------*/
/* uart_flush()                                                               */
/*----------------------------------------------------------------------------*/
uint8_t uart_flush(uart_t* pState)
{
  uart_init_timeout(pState);

  if (pState && (UART_OPEN == pState->uiState))
  {
    while (IO_133B & 0x01)
    {
      pState->uiBuffer = IO_143B;

      if (EOK != uart_check_timeout(pState))
      {
        return ETIMEOUT;
      }
    }

    return EOK;
  }

  return EINVAL;
}


/*----------------------------------------------------------------------------*/
/* uart_tx_byte()                                                             */
/*----------------------------------------------------------------------------*/
uint8_t uart_tx_byte(uart_t* pState, uint8_t uiData)
{
  uart_init_timeout(pState);

  if (pState && (UART_OPEN == pState->uiState))
  {
    while (IO_133B & 0x02)
    {
      if (EOK != uart_check_timeout(pState))
      {
        return ETIMEOUT;
      }
    }
    
    IO_133B = uiData;

    return EOK;
  }

  return EINVAL;
}


/*----------------------------------------------------------------------------*/
/* uart_rx_byte()                                                             */
/*----------------------------------------------------------------------------*/
uint8_t uart_rx_byte(uart_t* pState, uint8_t* pData)
{
  uart_init_timeout(pState);

  if (pState && (UART_OPEN == pState->uiState))
  {
    if (0 != pData)
    {
      while (!(IO_133B & 0x01))
      {
        if (EOK != uart_check_timeout(pState))
        {
          return ETIMEOUT;
        }
      }

      *pData = IO_143B;

      return EOK;
    }
  }

  return EINVAL;
}


/*----------------------------------------------------------------------------*/
/* uart_tx()                                                                  */
/*----------------------------------------------------------------------------*/
uint8_t uart_tx(uart_t* pState, uint8_t* uiData, uint16_t uiLen)
{
  if (pState && (UART_OPEN == pState->uiState))
  {
    if (uiData && uiLen)
    {
      pState->uiIdx = 0;

      while (pState->uiIdx < uiLen)
      {
        if (EOK != uart_tx_byte(pState, uiData[pState->uiIdx]))
        {
          return EBREAK;
        }

        ++pState->uiIdx;
      }

      return EOK;
    }
  }

  return EINVAL;
}


/*----------------------------------------------------------------------------*/
/*                                                                            */
/*----------------------------------------------------------------------------*/
