/*-----------------------------------------------------------------------------+
|                                                                              |
| filename: uart_set_baudrate.c                                                |
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
#include <stdint.h>
#include <errno.h>
#include <arch/zxn.h>
#include "libzxn.h"
#include "libuart.h"

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
/*                                                                            */
/*----------------------------------------------------------------------------*/
