/*
Copyright <2017> <COPYRIGHT HOLDER>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE

*/


#ifndef H_CONFIGURATION_H
#define H_CONFIGURATION_H

#define BUILD_DATE  __DATE__ " " __TIME__
#define FW_VERSION  "1.1.0"
#define FW_NAME     "wifilink"
#define MCU_OTA

/*
 * Enable/Disable Debug
 */

//#define DEBUG
//#define BAUDRATE_DEBUG 115200

/*
 * Define board hostname
 */

#define DEF_HOSTNAME "arduino"

/*
 * Defines the communication channel between microcontroller
 * and esp82266, with concerning parameters
 */

//Arduino UNO WIFI DEV. EDITION configuration parameters
#define BOARDMODEL "UNOWIFIDEVED"
#define ESP_CH_UART
#define SSIDNAME "Arduino-Uno-WiFi"
#define VERSION                0x00000010   // 0.0.1
#define WIFI_HOSTNAME          "%s-%04d"    // Expands to <MQTT_TOPIC>-<last 4 decimal chars of MAC address>

// #define USE_CONFIG_OVERRIDE                      // Uncomment to use user_config_override.h file. See README.md

/*********************************************************************************************\
 * SECTION 1
 * - After initial load any change here only take effect if CFG_HOLDER is changed too
\*********************************************************************************************/

// -- Master parameter control --------------------
#define CFG_HOLDER             0x20180707        // [Reset 1] Change this value to load SECTION1 configuration parameters to flash

// -- Project -------------------------------------
#define PROJECT                "unowifi"         // PROJECT is used as the default topic delimiter

// -- Wifi ----------------------------------------
#define WIFI_IP_ADDRESS        "0.0.0.0"         // [IpAddress1] Set to 0.0.0.0 for using DHCP or IP address
#define WIFI_GATEWAY           "192.168.2.254"   // [IpAddress2] If not using DHCP set Gateway IP address
#define WIFI_SUBNETMASK        "255.255.255.0"   // [IpAddress3] If not using DHCP set Network mask
#define WIFI_DNS               "192.168.2.27"    // [IpAddress4] If not using DHCP set DNS IP address (might be equal to WIFI_GATEWAY)
#define WIFI_CONFIG_TOOL       WIFI_WPSCONFIG    // [WifiConfig] Default tool if wifi fails to connect
                                                 //   (WIFI_RESTART, WIFI_SMARTCONFIG, WIFI_MANAGER, WIFI_WPSCONFIG, WIFI_RETRY, WIFI_WAIT)

#define STA_SSID1              ""                // [Ssid1] Wifi SSID
#define STA_PASS1              ""                // [Password1] Wifi password
#define STA_SSID2              ""                // [Ssid2] Optional alternate AP Wifi SSID
#define STA_PASS2              ""                // [Password2] Optional alternate AP Wifi password

// -- MQTT - Telemetry ----------------------------
#define TELE_PERIOD            300               // [TelePeriod] Telemetry (0 = disable, 10 - 3600 seconds)
#define MQTT_HOST              ""                // [MqttHost]
#define MQTT_PORT              1883              // [MqttPort] MQTT port (10123 on CloudMQTT)
#define MQTT_USER              "mqttuser"        // [MqttUser] MQTT user
#define MQTT_PASS              "mqttpass"        // [MqttPassword] MQTT password
#define MQTT_CLIENT_ID         "UNO_%06X"       // [MqttClient] Also fall back topic using Chip Id = last 6 characters of MAC address
#define MQTT_TOPIC             PROJECT           // [Topic] (unique) MQTT device topic
#define MQTT_FULLTOPIC         "%prefix%/%topic%/" // [FullTopic] Subscribe and Publish full topic name - Legacy topic

// %prefix% token options
#define SUB_PREFIX             "cmnd"            // [Prefix1] Sonoff devices subscribe to %prefix%/%topic% being SUB_PREFIX/MQTT_TOPIC and SUB_PREFIX/MQTT_GRPTOPIC
#define PUB_PREFIX             "stat"            // [Prefix2] Sonoff devices publish to %prefix%/%topic% being PUB_PREFIX/MQTT_TOPIC
#define PUB_PREFIX2            "tele"            // [Prefix3] Sonoff devices publish telemetry data to %prefix%/%topic% being PUB_PREFIX2/MQTT_TOPIC/UPTIME, POWER and TIME

#define SERIAL_LOG_LEVEL       LOG_LEVEL_INFO    // [SerialLog] (LOG_LEVEL_NONE, LOG_LEVEL_ERROR, LOG_LEVEL_INFO, LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG_MORE)
#define WEB_LOG_LEVEL          LOG_LEVEL_INFO    // [WebLog] (LOG_LEVEL_NONE, LOG_LEVEL_ERROR, LOG_LEVEL_INFO, LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG_MORE)

#define APP_BAUDRATE           115200            // Default serial baudrate
// #define APP_BAUDRATE           230400
#define APP_SLEEP              0                 // [Sleep] Sleep time to lower energy consumption (0 = Off, 1 - 250 mSec)
#endif
