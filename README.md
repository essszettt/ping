               _            
        ____  (_)___  ____ _
       / __ \/ / __ \/ __ `/
      / /_/ / / / / / /_/ / 
     / .___/_/_/ /_/\__, /  
    /_/            /____/   


Application to check availability of hosts via ICMP-PING.

This application uses the command "AT+PING" from ESP8266 to execute pings to hosts via ethernet/WiFi.

**It is important, that the baudrate of the ESP8266 is set to "115200 bit/s" (default).**

By default five PINGs are sent per host. The number of PINGs can be specified by commandline option "c". If the number of PINGs is set to "0" then PINGs are sent in an endless loop. This loop can be interrupted by pressing "C", "Q", "BREAK" or "CAPS+SPACE" ...

![ping.bmp](https://github.com/essszettt/ping/blob/main/doc/ping.bmp)

---

### USAGE

![help.bmp](https://github.com/essszettt/ping/blob/main/doc/help.bmp)

---

### HISTORY

- 0.0.4   First public release to test
- 0.0.7   Added timeout detection; fixed user break
- 0.0.8   Fixed some internal errors; improved optimization
- 0.0.9   Implemented "libzxn" as static library
- 0.0.10  Fixed timeouts in UART communication (tested on MAME without ESP8255)
- 0.0.11  Using new static library for UART & ESP (based on old code)
