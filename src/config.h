
/*
 * Firmware version and build date
 */
#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

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
#define BAUDRATE_COMMUNICATION 115200 // assuming use of Serial1
#define SSIDNAME "Arduino-Uno-WiFi"

#endif // SRC_CONFIG_H_
