/*
 * ArduinoFirmwareEsp.h
 *
 *  Created on: Jan 7, 2018
 *      Author: Charl-PC
 */

#ifndef SRC_ARDUINOFIRMWAREESP_H_
#define SRC_ARDUINOFIRMWAREESP_H_


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <stdint.h>
#include "firmware.h"
#include "CommLgc.h"
#include "i18n.h"
#include "Configuration.h"
#include "settings.h"
#ifdef USE_CONFIG_OVERRIDE
  #include "user_config_override.h"         // Configuration overrides for user_config.h
#endif
#include "WebServer.h"


enum LoggingLevels {LOG_LEVEL_NONE, LOG_LEVEL_ERROR, LOG_LEVEL_INFO, LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG_MORE, LOG_LEVEL_ALL};
enum TopicOptions { CMND, STAT, TELE, nu1, RESULT_OR_CMND, RESULT_OR_STAT, RESULT_OR_TELE };


#define CONFIG_FILE_SIGN       0xA5         // Configuration file signature
#define CONFIG_FILE_XOR        0x5A         // Configuration file xor (0 = No Xor)
#define LOGSZ                  512          // Max number of characters in log
#define TOPSZ                  100          // Max number of characters in topic string
#define WEB_LOG_SIZE           4000         // Max number of characters in weblog
#define INPUT_BUFFER_SIZE      512          // Max number of characters in (serial and http) command buffer
#define CMDSZ                  24           // Max number of characters in command
#define MESSZ                  (MQTT_MAX_PACKET_SIZE -TOPSZ -7)  // Max number of characters in JSON message
#define MQTT_RETRY_SECS        10           // Minimum seconds to retry MQTT connection

#define MQTT_TOKEN_PREFIX      "%prefix%"   // To be substituted by mqtt_prefix[x]
#define MQTT_TOKEN_TOPIC       "%topic%"    // To be substituted by mqtt_topic, mqtt_grptopic, mqtt_buttontopic, mqtt_switchtopic

// Functions


void initMDNS(void);

void initHostname(void);

void wifiLed(void);

void setWiFiConfig(void);

#include <Hash.h>
#include <ESP8266WebServer.h>
#include <dfu.h>
#include <dfu-host.h>
#include <dfu-cmd.h>
#include "config.h"
#include <user_config.h>
#include <dfu-internal.h>
#include <esp8266-serial.h>
#include <dfu-esp8266.h>
#include <stk500-device.h>
#include <dfu-stk500.h>

static int _setup_dfu(void);

void _handle_Mcu_OTA(void);


#endif /* SRC_ARDUINOFIRMWAREESP_H_ */
