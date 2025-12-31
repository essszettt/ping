/*-----------------------------------------------------------------------------+
|                                                                              |
| filename: esp_receive_ex.c                                                   |
| project:  ZX Spectrum Next - libesp                                          |
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
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "libzxn.h"
#include "libuart.h"
#include "libesp.h"

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
/* esp_receive_ex()                                                           */
/*----------------------------------------------------------------------------*/
uint8_t esp_receive_ex(esp_t* pState, char_t* acBuffer, uint16_t uiSize)
{
  if (EOK == esp_receive(pState, acBuffer, uiSize))
  {
    if (0 == strcmp(acBuffer, sESP_RESP_OK "\r\n"))
    {
      return ESP_LINE_OK;
    }
    else if (0 == strcmp(acBuffer, sESP_RESP_ERROR "\r\n"))
    {
      return ESP_LINE_ERROR;
    }
    else if (0 == strcmp(acBuffer, sESP_RESP_FAIL "\r\n"))
    {
      return ESP_LINE_FAIL;
    }
    else
    {
      return ESP_LINE_DATA;
    }
  }

  return ESP_LINE_FATAL;
}


/*----------------------------------------------------------------------------*/
/*                                                                            */
/*----------------------------------------------------------------------------*/
