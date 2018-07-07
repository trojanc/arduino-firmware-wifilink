/*
 * WebServer.h
 *
 *  Created on: Jan 7, 2018
 *      Author: Charl-PC
 */

#ifndef SRC_WEBSERVER_H_
#define SRC_WEBSERVER_H_

#include <Arduino.h>
#include <FS.h>
#include <ESP8266mDNS.h>
#include <core_version.h>

#include "firmware.h"
#include "CommLgc.h"
#include "Configuration.h"

// Functions

IPAddress stringToIP(String address);

void initWebServer(void);

void handleWebServer(void);

#endif /* SRC_WEBSERVER_H_ */
