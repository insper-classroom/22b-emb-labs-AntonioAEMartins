
#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/include/m2m_wifi.h"

/** Wi-Fi Settings */
#define MAIN_WLAN_SSID                    "#EleNao" /**< Destination SSID */
#define MAIN_WLAN_AUTH                    M2M_WIFI_SEC_WPA_PSK /**< Security manner */
#define MAIN_WLAN_PSK                     "1223334444" /**< Password for Destination SSID */

/** Using broadcast address for simplicity. */
#define MAIN_SERVER_PORT                    (5000)

/** Weather information provider server. */
#define MAIN_SERVER_NAME                    "192.168.0.138"

/** IP address parsing. */
#define IPV4_BYTE(val, index)               ((val >> (index * 8)) & 0xFF)

/** Send buffer of TCP socket. */
#define MAIN_PREFIX_BUFFER                  "GET /status HTTP/1.1\r\n Accept: */*\r\n\r\n"

/** Receive buffer size. */
#define MAIN_WIFI_M2M_BUFFER_SIZE           1400

#define MAIN_HEX2ASCII(x)                   (((x) >= 10) ? (((x) - 10) + 'A') : ((x) + '0'))


#ifdef __cplusplus
}
#endif

#endif /* MAIN_H_INCLUDED */
