/*-----------------------------------------------------------------------------+
|                                                                              |
| filename: ping.h                                                             |
| project:  ZX Spectrum Next - PING                                            |
| author:   Stefan Zell                                                        |
| date:     12/07/2025                                                         |
|                                                                              |
+------------------------------------------------------------------------------+
|                                                                              |
| description:                                                                 |
|                                                                              |
| Application to ping remote hosts (using ESP32s "AT+PING")                    |
| (based on "espbaud" from Allen Albright)                                     |
|                                                                              |
+------------------------------------------------------------------------------+
|                                                                              |
| Copyright (c) 12/07/2025 STZ Engineering                                     |
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

#if !defined(__PING_H__)
  #define __PING_H__

/*============================================================================*/
/*                               Includes                                     */
/*============================================================================*/

/*============================================================================*/
/*                               Defines                                      */
/*============================================================================*/
/*!
Maximum length of the hostname
*/
#define uiMAX_HOST_NAME (0x100)

/*!
Maximum length of a AT command to ESP8266
*/
#define uiMAX_LEN_CMD (0x80)

/*!
ESP command to send a PING request
*/
#define sCMD_AT_PING "AT+PING"

/*!
ESP command to read version information
*/
#define sCMD_AT_GMR "AT+GMR"

/*!
Default value for number of ping
*/
#define uiDEFAULT_COUNT (5)

/*!
Default value for interval between pings [ms]
*/
#define uiDEFAULT_INTERVAL (100)

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
Enumeration/list of all actions the application can execute
*/
typedef enum _action
{
  ACTION_NONE = 0,
  ACTION_HELP,
  ACTION_INFO,
  ACTION_PING
} action_t;

/*!
In dieser Struktur werden alle globalen Daten der Anwendung gespeichert.
*/
typedef struct _appstate
{
  /*!
  If this flag is set, then this structure is initialized
  */
  bool bInitialized;

  /*!
  Action to execute (help, version, ping, ...)
  */
  action_t eAction;

  /*!
  If this flag is set, no messages are printed to the console while pinging.
  */
  bool bQuiet;

  /*!
  Number of repetitions; "0" = endless
  */
  uint16_t uiCount;

  /*!
  Interval between repetitions in [ms]
  */
  uint16_t uiInterval;

  /*!
  Name of the host to ping
  */
  char_t acHost[uiMAX_HOST_NAME];

  /*!
  Backup: Current speed of Z80N
  */
  uint8_t uiCpuSpeed;

  /*!
  Statistical information
  */
  struct
  {
    /*!
    Sum of the duration of all pings
    */
    uint32_t uiTotal;

    /*!
    Duration of last ping
    */
    uint16_t uiTime;

    /*!
    Duration of the fastest ping
    */
    uint16_t uiMin;

    /*!
    Duration of the slowest ping
    */
    uint16_t uiMax;

    /*!
    Total number of pings
    */
    uint16_t uiPings;

    /*!
    Number of successful responses
    */
    uint16_t uiPongs;
  } stats;

  /*!
  */
  esp_t tEsp;

  struct
  {
    /*!
    Buffer for response from ESP8266
    */
    char_t acTxBuffer[uiMAX_LEN_CMD];

    /*!
    Buffer for response from ESP8266
    */
    char_t acRxBuffer[uiMAX_LEN_CMD];
  } esp;
  
  /*!
  Exitcode of the application, that is handovered to BASIC
  */
  int iExitCode;
} appstate_t;

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
/*                                                                            */
/*----------------------------------------------------------------------------*/

#endif /* __PING_H__ */
