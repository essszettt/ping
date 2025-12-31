/*-----------------------------------------------------------------------------+
|                                                                              |
| filename: main.c                                                             |
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

/*============================================================================*/
/*                               Includes                                     */
/*============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <intrinsic.h>
#include <arch/zxn.h>
#include <input.h>
#include <input/input_zx.h>

#include "libzxn.h"
#include "libuart.h"
#include "libesp.h"
#include "ping.h"
#include "version.h"

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
Global data of the application
*/
static appstate_t g_tState;

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
This function is called once at startup of the application to reserve required
resources.
*/
void _construct(void);

/*!
This function is called once at termination of the application to release all
used resources.
*/
void _destruct(void);

/*!
Application local "printf" that handels option "-q" ("quiet") and is able to
print to stdout/stderr.
@param pStream Stream to print to ("stdout", "stderr")
@param acFmt Format string (see "printf")
@return Errorcode (see "printf"); negative values signaling errors
*/
int app_printf(FILE* pStream, char_t* acFmt, ...);

/*!
This function parses all given commandline arguments/options
*/
int parseArguments(int argc, char* argv[]);

/*!
Print help of the application
*/
int showHelp(void);

/*!
Print version information of the application
*/
int showInfo(void);

/*!
Print version info of ESP8266
*/
int showInfoEx(void);

/*!
Execute pings to given host
*/
int ping(void);

/*============================================================================*/
/*                               Klassen                                      */
/*============================================================================*/

/*============================================================================*/
/*                               Implementierung                              */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* _construct()                                                               */
/*----------------------------------------------------------------------------*/
void _construct(void)
{
  if (!g_tState.bInitialized)
  {
    g_tState.eAction    = ACTION_NONE;
    g_tState.bQuiet     = false;
    g_tState.uiCount    = uiDEFAULT_COUNT;
    g_tState.uiInterval = uiDEFAULT_INTERVAL;
    g_tState.acHost[0]  = '\0';
    g_tState.uiCpuSpeed = zxn_getspeed();
    g_tState.iExitCode  = EOK;

    zxn_setspeed(RTM_28MHZ);
    esp_open(&g_tState.tEsp);

    g_tState.bInitialized = true;
  }
}


/*----------------------------------------------------------------------------*/
/* _destruct()                                                                */
/*----------------------------------------------------------------------------*/
void _destruct(void)
{
  if (g_tState.bInitialized)
  {
    esp_close(&g_tState.tEsp);
    zxn_setspeed(g_tState.uiCpuSpeed);
  }
}


/*----------------------------------------------------------------------------*/
/* main()                                                                     */
/*----------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
  _construct();
  atexit(_destruct);

  if (EOK == (g_tState.iExitCode = parseArguments(argc, argv)))
  {
    switch (g_tState.eAction)
    {
      case ACTION_NONE:
        g_tState.iExitCode = EOK;
        break;

      case ACTION_HELP:
        g_tState.iExitCode = showHelp();
        break;

      case ACTION_INFO:
        g_tState.iExitCode = showInfo();
        break;

      case ACTION_INFOEX:
        g_tState.iExitCode = showInfoEx();
        break;

      case ACTION_PING:
        g_tState.iExitCode = ping();
        break;
    }
  }

  return (int) (EOK == g_tState.iExitCode ? 0 : zxn_strerror(g_tState.iExitCode));
}


/*----------------------------------------------------------------------------*/
/* app_printf()                                                               */
/*----------------------------------------------------------------------------*/
int app_printf(FILE* pStream, char_t* acFmt, ...)
{
  int iReturn = EOK;

  if (pStream && acFmt) 
  {
    if (!g_tState.bQuiet)
    {
      va_list args;
      va_start(args, acFmt);

      iReturn = vfprintf(pStream, acFmt, args);

      va_end(args);
    }
  }
  else
  {
    iReturn = -EINVAL;
  }

  return iReturn;
}


/*----------------------------------------------------------------------------*/
/* parseArguments()                                                           */
/*----------------------------------------------------------------------------*/
int parseArguments(int argc, char* argv[])
{
  int iReturn = EOK;

  /* Defaults */
  g_tState.eAction = ACTION_NONE;

  int i = 1;
  while (i < argc)
  {
    const char_t* acArg = argv[i];

    if ('-' == acArg[0]) /* Options */
    {
      if ((0 == strcmp(acArg, "-h")) || (0 == stricmp(acArg, "--help")))
      {
        g_tState.eAction = ACTION_HELP;
      }
      else if ((0 == strcmp(acArg, "-v")) || (0 == stricmp(acArg, "--version")))
      {
        g_tState.eAction = ACTION_INFO;
      }
      else if ((0 == strcmp(acArg, "-V")) /* || (0 == stricmp(acArg, "--Version")) */)
      {
        g_tState.eAction = ACTION_INFOEX;
      }
      else if ((0 == strcmp(acArg, "-q")) || (0 == stricmp(acArg, "--quiet")))
      {
        g_tState.bQuiet = true;
      }
      else if ((0 == strcmp(acArg, "-c")) || (0 == stricmp(acArg, "--count")))
      {
        if ((i + 1) < argc)
        {
          g_tState.uiCount = strtoul(argv[++i], 0, 0);
        }
        else
        {
          app_printf(stderr, "option %s requires a value\n", acArg);
          iReturn = EINVAL;
          break;
        }
      }
      else if ((0 == strcmp(acArg, "-i")) || (0 == stricmp(acArg, "--interval")))
      {
        if ((i + 1) < argc)
        {
          g_tState.uiInterval = strtoul(argv[++i], 0, 0);
        }
        else
        {
          app_printf(stderr, "option %s requires a value\n", acArg);
          iReturn = EINVAL;
          break;
        }
      }
      else
      {
        app_printf(stderr, "unknown option: %s\n", acArg);
        iReturn = EINVAL;
        break;
      }
    }
    else /* Arguments */
    {
      if ('\0' == g_tState.acHost[0])
      {
        snprintf(g_tState.acHost, sizeof(g_tState.acHost), "%s", acArg);
      }
      else
      {
        app_printf(stderr, "unexpected extra argument: %s\n", acArg);
        iReturn = EINVAL;
        break;
      }
    }

    ++i;
  }

  if (EOK == iReturn)
  {
    if (ACTION_NONE == g_tState.eAction)
    {
      if ('\0' != g_tState.acHost[0])
      {
        g_tState.eAction = ACTION_PING;
      }
      else
      {
        app_printf(stderr, "no hostname specified\n");
        iReturn = EINVAL;
      }
    }
  }

  DBGPRINTF("parseargs() - action   = %d\n", g_tState.eAction);
  DBGPRINTF("parseargs() - host     = %s\n", g_tState.acHost);
  DBGPRINTF("parseargs() - count    = %u\n", g_tState.uiCount);
  DBGPRINTF("parseargs() - interval = %u\n", g_tState.uiInterval);

  return iReturn;
}


/*----------------------------------------------------------------------------*/
/* showHelp()                                                                 */
/*----------------------------------------------------------------------------*/
int showHelp(void)
{
  char_t acAppName[0x10];
  strncpy(acAppName, VER_INTERNALNAME_STR, sizeof(acAppName));
  strupr(acAppName);

  app_printf(stdout, "%s\n\n", VER_FILEDESCRIPTION_STR);

  app_printf(stdout, "%s host [-c x][-i x][-q][-h][-v][-V]\n\n", acAppName);
  //                  0.........1.........2.........3.
  app_printf(stdout, " host        host to ping\n");
  app_printf(stdout, " -c[ount]    stop after x pings\n");
  app_printf(stdout, " -i[nterval] delay betw. pings\n");
  app_printf(stdout, " -q[uiet]    no screen output\n");
  app_printf(stdout, " -h[elp]     print this help\n");
  app_printf(stdout, " -v[ersion]  print version info\n");
  app_printf(stdout, " -V[ersion]  print ext. version\n");

  return EOK;
}


/*----------------------------------------------------------------------------*/
/* showInfo()                                                                 */
/*----------------------------------------------------------------------------*/
int showInfo(void)
{
  char_t acBuffer[0x10];
  uint16_t uiVersion;

  strncpy(acBuffer, VER_INTERNALNAME_STR, sizeof(acBuffer));
  strupr(acBuffer);

  app_printf(stdout, "%s " VER_LEGALCOPYRIGHT_STR "\n", acBuffer);

  if (ESX_DOSVERSION_NEXTOS_48K != (uiVersion = esx_m_dosversion()))
  {
    snprintf(acBuffer, sizeof(acBuffer), "NextOS %u.%02u",
             ESX_DOSVERSION_NEXTOS_MAJOR(uiVersion),
             ESX_DOSVERSION_NEXTOS_MINOR(uiVersion));
  }
  else
  {
    strncpy(acBuffer, "48K mode", sizeof(acBuffer));
  }

  //                  0.........1.........2.........3.
  app_printf(stdout, " Version %s (%s)\n", VER_FILEVERSION_STR, acBuffer);
  app_printf(stdout, " Stefan Zell (info@diezells.de)\n");

  return EOK;
}


/*----------------------------------------------------------------------------*/
/* showInfoEx()                                                               */
/*----------------------------------------------------------------------------*/
int showInfoEx(void)
{
  char_t acBuffer[0x10];

  strncpy(acBuffer, VER_INTERNALNAME_STR, sizeof(acBuffer));
  strupr(acBuffer);

  app_printf(stdout, "%s: Espressif ESP8266\n", acBuffer);

  /* Initialize UART/ESP */
  esp_flush(&g_tState.tEsp);

  /* Read version information */
  if (EOK == esp_transmit(&g_tState.tEsp, sCMD_AT_GMR "\r\n"))
  {
    while (ESP_LINE_DATA == esp_receive_ex(&g_tState.tEsp, g_tState.esp.acRxBuffer, sizeof(g_tState.esp.acRxBuffer)))
    {
      zxn_rtrim(g_tState.esp.acRxBuffer);
      app_printf(stdout, " %s\n", g_tState.esp.acRxBuffer);
    }
  }
  else
  {
    app_printf(stderr, "unable to send " sCMD_AT_GMR " to ESP8266\n");
  }

  /* Read local IP addresses */
  if (EOK == esp_transmit(&g_tState.tEsp, sCMD_AT_CIPSTA_CUR "?" "\r\n"))
  {
    while (ESP_LINE_DATA == esp_receive_ex(&g_tState.tEsp, g_tState.esp.acRxBuffer, sizeof(g_tState.esp.acRxBuffer)))
    {
      zxn_rtrim(g_tState.esp.acRxBuffer);
      app_printf(stdout, " %s\n", g_tState.esp.acRxBuffer);
    }
  }
  else
  {
    app_printf(stderr, "unable to send " sCMD_AT_CIPSTA_CUR " to ESP8266\n");
  }

  return EOK;
}


/*----------------------------------------------------------------------------*/
/* ping()                                                                     */
/*----------------------------------------------------------------------------*/
int ping(void)
{
  int iReturn = EOK;
  uint8_t uiResult;

  /* Initialize UART/ESP */
  esp_flush(&g_tState.tEsp);

  /* Create PING command */
  snprintf(g_tState.esp.acTxBuffer, sizeof(g_tState.esp.acTxBuffer), sCMD_AT_PING "=\"%s\"\r\n", g_tState.acHost);

#if 0
  putchar(0x04);
  putchar(0x00);
#endif

  app_printf(stdout, "pinging %s ..\n", g_tState.acHost);

  g_tState.stats.uiTotal = 0;
  g_tState.stats.uiTime  = 0;
  g_tState.stats.uiMin   = UINT16_MAX;
  g_tState.stats.uiMax   = 0;
  g_tState.stats.uiPings = 0;
  g_tState.stats.uiPongs = 0;

  bool bFinished = false;
  do
  {
    /* Send request to ESP8266 */
    if (EOK == esp_transmit(&g_tState.tEsp, g_tState.esp.acTxBuffer))
    {
      ++g_tState.stats.uiPings;
    }
    else
    {
      iReturn = EBREAK;
      goto EXIT_PING;
    }

    /* Read response from ESP8266 */
    for ( ; ; )
    {
      uiResult = esp_receive_ex(&g_tState.tEsp, g_tState.esp.acRxBuffer, sizeof(g_tState.esp.acRxBuffer));  

      if (ESP_LINE_DATA == uiResult)
      {
        sscanf(g_tState.esp.acRxBuffer, "+%u", &g_tState.stats.uiTime);
      }
      else if (ESP_LINE_OK == uiResult)
      {
        ++g_tState.stats.uiPongs;

        g_tState.stats.uiTotal += g_tState.stats.uiTime;

        if (g_tState.stats.uiTime < g_tState.stats.uiMin)
        {
          g_tState.stats.uiMin = g_tState.stats.uiTime;
        }

        if (g_tState.stats.uiTime > g_tState.stats.uiMax)
        {
          g_tState.stats.uiMax = g_tState.stats.uiTime;
        }

        app_printf(stdout, "response from %s: time=%u ms\n", g_tState.acHost, g_tState.stats.uiTime);
        break;
      }
      else if (ESP_LINE_ERROR == uiResult)
      {
        app_printf(stderr, "unknown host \"%s\"\n", g_tState.acHost);
        iReturn = ERANGE;
        goto EXIT_PING;
      }
      else if (ESP_LINE_FAIL == uiResult)
      {
        app_printf(stdout, "timeout\n");
        break;
      }
      else
      {
        app_printf(stderr, "communication error\n");
        iReturn = ENOTSUP;
        goto EXIT_PING;
      }
    }

    /* User break ? */
    if (0 != (g_tState.iKey = in_inkey()))
    {
      switch (g_tState.iKey)
      {
        case ' ':
        case 'c':
        case 'C':
        case 'q':
        case 'Q':
          bFinished = true;
          break;
      }
    }

   #if 0
    if (in_key_pressed(IN_KEY_SCANCODE_SPACE | 0x8000)) /* CAPS + SPACE */
    {
      bFinished = true;
    } 
   #endif

    /* Count reached ? */
    if (0 != g_tState.uiCount)
    {
      if (g_tState.stats.uiPings >= g_tState.uiCount)
      {
        bFinished = true;
      }
    }

    /* Interval */
    if ((0 != g_tState.uiInterval) && !bFinished)
    {
      zxn_sleep_ms(g_tState.uiInterval);
    }
  }
  while (!bFinished);

  /* Create statistics */
  app_printf(stdout, "\n--- %s statistics ---\n", g_tState.acHost);
  app_printf(stdout, "%u transmitted, %u received, time %u ms\n",
                      g_tState.stats.uiPings,
                      g_tState.stats.uiPongs,
                      ((uint16_t) g_tState.stats.uiTotal));
  app_printf(stdout, "rtt min/avg/max = %u/%u/%u [ms]\n",
                      (UINT16_MAX != g_tState.stats.uiMin ? g_tState.stats.uiMin : 0),
                      ((uint16_t) (g_tState.stats.uiTotal / g_tState.stats.uiPongs)),
                      g_tState.stats.uiMax);

  /* Wait until break-key is released */
  while (0 != (g_tState.iKey = in_inkey()))
  {
    intrinsic_nop();
  }

EXIT_PING:

#if 0
  putchar(0x04);
  putchar(0x01);
#endif

  return (EOK != iReturn ? iReturn : (0 != g_tState.stats.uiPongs ? EOK : ETIMEOUT));
}


/*----------------------------------------------------------------------------*/
/*                                                                            */
/*----------------------------------------------------------------------------*/
