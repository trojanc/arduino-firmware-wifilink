/*
 * firmware.h
 *
 *  Created on: Jan 7, 2018
 *      Author: Charl-PC
 */

#ifndef SRC_FIRMWARE_H_
#define SRC_FIRMWARE_H_

#include <Arduino.h>
#include <ESP8266WebServer.h>

extern ESP8266WebServer server;
extern String dhcp;
extern String staticIP_param;
extern String netmask_param;
extern String gateway_param;
#endif /* SRC_FIRMWARE_H_ */
