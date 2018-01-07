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

#include <FS.h>
#include <ArduinoJson.h>
#include <Hash.h>
#include <ESP8266WebServer.h>

// Functions


void initMDNS(void);

void initHostname(void);

void wifiLed(void);

void setWiFiConfig(void);

#endif /* SRC_ARDUINOFIRMWAREESP_H_ */
