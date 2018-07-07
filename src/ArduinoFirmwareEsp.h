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
#include "Configuration.h"
#include "settings.h"
#ifdef USE_CONFIG_OVERRIDE
  #include "user_config_override.h"         // Configuration overrides for user_config.h
#endif
#include "WebServer.h"




#define CONFIG_FILE_SIGN       0xA5         // Configuration file signature
#define CONFIG_FILE_XOR        0x5A         // Configuration file xor (0 = No Xor)
// Functions


void initMDNS(void);

void initHostname(void);

void wifiLed(void);

void setWiFiConfig(void);

#include <FS.h>
#include <ArduinoJson.h>
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
