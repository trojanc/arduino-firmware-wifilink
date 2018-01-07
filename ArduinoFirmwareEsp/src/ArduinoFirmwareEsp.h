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

#include "firmware.h"
#include "CommLgc.h"
#include "Configuration.h"
#include "Webserver.h"

// Functions


void initMDNS(void);

void initHostname(void);

void wifiLed(void);

void setWiFiConfig(void);

#include <FS.h>
#include <ArduinoJson.h>
#include <Hash.h>
#include <ESP8266WebServer.h>

#if defined(MCU_OTA)
#include <dfu.h>
#include <dfu-host.h>
#include <dfu-cmd.h>
#include "config.h"
#include <user_config.h>
#include <dfu-internal.h>
#include <esp8266-serial.h>
#include <dfu-esp8266.h>
#if defined(STAROTTO)
#include <dfu-stm32.h>
#elif defined(UNOWIFIDEVED) || defined (GENERIC_ESP8266)
#include <stk500-device.h>
#include <dfu-stk500.h>
#endif

static int _setup_dfu(void);

void _handle_Mcu_OTA(void);
#endif


#endif /* SRC_ARDUINOFIRMWAREESP_H_ */
