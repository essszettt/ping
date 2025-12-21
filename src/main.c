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
#include <string.h>
#include <errno.h>
#include <arch/zxn.h>
#include <input.h>
#include <input/input_zx.h>

#include "libzxn.h"
#include "uart.h"
#include "esp.h"
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
int showInfoEsp(void);

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

      case ACTION_ESPINFO:
        g_tState.iExitCode = showInfoEsp();
        break;

      case ACTION_PING:
        g_tState.iExitCode = ping();
        break;
    }
  }

  return (int) (EOK == g_tState.iExitCode ? 0 : zxn_strerror(g_tState.iExitCode));
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
        g_tState.eAction = ACTION_ESPINFO;
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
          fprintf(stderr, "option %s requires a value\n", acArg);
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
          fprintf(stderr, "option %s requires a value\n", acArg);
          iReturn = EINVAL;
          break;
        }
      }
      else
      {
        fprintf(stderr, "unknown option: %s\n", acArg);
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
        fprintf(stderr, "unexpected extra argument: %s\n", acArg);
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
        fprintf(stderr, "no hostname specified\n");
        iReturn = EINVAL;
      }
    }
  }

  DBGPRINTF(__FUNCTION__ "() - action   = %d\n", g_tState.eAction);
  DBGPRINTF(__FUNCTION__ "() - host     = %s\n", g_tState.acHost);
  DBGPRINTF(__FUNCTION__ "() - count    = %u\n", g_tState.uiCount);
  DBGPRINTF(__FUNCTION__ "() - interval = %u\n", g_tState.uiInterval);

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

  printf("%s\n\n", VER_FILEDESCRIPTION_STR);

  printf("%s host [-c x][-i x][-q][-h][-v][-V]\n\n", acAppName);
  //      0.........1.........2.........3.
  printf(" host        host to ping\n");
  printf(" -c[ount]    stop after x pings\n");
  printf(" -i[nterval] delay betw. pings\n");
  printf(" -q[uiet]    no screen output\n");
  printf(" -h[elp]     print this help\n");
  printf(" -v[ersion]  print version info\n");
  printf(" -V[ersion]  print ESP version\n");

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

  printf("%s " VER_LEGALCOPYRIGHT_STR "\n", acBuffer);

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

  //      0.........1.........2.........3.
  printf(" Version %s (%s)\n", VER_FILEVERSION_STR, acBuffer);
  printf(" Stefan Zell (info@diezells.de)\n");

  return EOK;
}


/*----------------------------------------------------------------------------*/
/* showInfoEsp()                                                              */
/*----------------------------------------------------------------------------*/
int showInfoEsp(void)
{
  printf("ESP8266 version\n");

  snprintf(g_tState.esp.acTxBuffer, sizeof(g_tState.esp.acTxBuffer), sCMD_AT_GMR "\r\n");

  if (EOK == esp_transmit(&g_tState.tEsp, g_tState.esp.acTxBuffer))
  {
    DBGPRINTF(">>> %s", g_tState.esp.acTxBuffer);

    for ( ; ; )
    {
      if (EOK == esp_receive(&g_tState.tEsp, g_tState.esp.acRxBuffer, sizeof(g_tState.esp.acRxBuffer)))
      {
        DBGPRINTF("<<< %s", g_tState.esp.acRxBuffer);

        if (0 != strstr(g_tState.esp.acRxBuffer, "OK"))
        {
          break;
        }
        else if (0 != strstr(g_tState.esp.acRxBuffer, "ERROR"))
        {
          break;
        }
        else if (0 != strstr(g_tState.esp.acRxBuffer, "FAIL"))
        {
          break;
        }
        else
        {
          if (zxn_rtrim(g_tState.esp.acRxBuffer))
          {
            printf("  %s\n", g_tState.esp.acRxBuffer);
          }
        }
      }
      else
      {
        break;
      }
    }
  }
  else if (!g_tState.bQuiet)
  {
    fprintf(stderr, "unable to send command to ESP8255\n");
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

  /* Create PING command */
  snprintf(g_tState.esp.acTxBuffer, sizeof(g_tState.esp.acTxBuffer), sCMD_AT_PING "=\"%s\"\r\n", g_tState.acHost);

  if (!g_tState.bQuiet)
  {
    printf("Pinging %s ...\n", g_tState.acHost);
  }

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
      DBGPRINTF(">>> %s", g_tState.esp.acTxBuffer);
    }
    else
    {
      iReturn = EBREAK;
      goto EXIT_PING;
    }

    /* Read response from ESP8266 */
    for ( ; ; )
    {
      if (EOK == (uiResult = esp_receive(&g_tState.tEsp, g_tState.esp.acRxBuffer, sizeof(g_tState.esp.acRxBuffer))))
      {
        DBGPRINTF("<<< %s", g_tState.esp.acRxBuffer);

        if (0 != strstr(g_tState.esp.acRxBuffer, "OK"))
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

          if (!g_tState.bQuiet)
          {
            printf("response from %s: time=%u ms\n", g_tState.acHost, g_tState.stats.uiTime);
          }
          break;
        }
        else if (0 != strstr(g_tState.esp.acRxBuffer, "ERROR")) /* unknown host */
        {
          if (!g_tState.bQuiet)
          {
            fprintf(stderr, "unknown host \"%s\"\n", g_tState.acHost);
          }

          iReturn = ERANGE;
          goto EXIT_PING;
        }
        else if (0 != strstr(g_tState.esp.acRxBuffer, "FAIL")) /* TIMEOUT */
        {
          if (!g_tState.bQuiet)
          {
            printf("timeout\n", g_tState.acHost, g_tState.stats.uiTime);
          }

          break;
        }
        else if (0 != strstr(g_tState.esp.acRxBuffer, "+"))
        {
          sscanf(g_tState.esp.acRxBuffer, "+%u", &g_tState.stats.uiTime);
        }
      }
      else
      {
        iReturn = ENOTSUP;
        bFinished = true;
      }
    }

    /* Interval */
    if (0 != g_tState.uiInterval)
    {
      zxn_sleep_ms(g_tState.uiInterval);
    }

    /* User break ? */
    if (in_key_pressed(IN_KEY_SCANCODE_SPACE | 0x8000)) /* CAPS + SPACE */
    {
      bFinished = true;
    } 

    /* Count reached ? */
    if (0 != g_tState.uiCount)
    {
      if (g_tState.stats.uiPings >= g_tState.uiCount)
      {
        bFinished = true;
      }
    }
  }
  while (!bFinished);

  /* Create statistics */
  if (!g_tState.bQuiet)
  {
    printf("\n--- %s ping statistics ---\n", g_tState.acHost);
    printf("%u transmitted, %u received, time %u ms\n",
           g_tState.stats.uiPings,
           g_tState.stats.uiPongs,
           ((uint16_t) g_tState.stats.uiTotal));
    printf("rtt min/avg/max = %u/%u/%u [ms]\n",
           (UINT16_MAX != g_tState.stats.uiMin ? g_tState.stats.uiMin : 0),
           ((uint16_t) (g_tState.stats.uiTotal / g_tState.stats.uiPongs)),
           g_tState.stats.uiMax);
  }

EXIT_PING:

  return (EOK != iReturn ? iReturn : (0 != g_tState.stats.uiPongs ? EOK : ETIMEOUT));
}


/*----------------------------------------------------------------------------*/
/*                                                                            */
/*----------------------------------------------------------------------------*/
