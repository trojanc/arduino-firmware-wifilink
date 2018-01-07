/*
 * ArduinoMcuOTA.h
 *
 *  Created on: Jan 7, 2018
 *      Author: Charl-PC
 */

#ifndef SRC_ARDUINOMCUOTA_H_
#define SRC_ARDUINOMCUOTA_H_


#include "firmware.h"


#include <dfu.h>
#include <dfu-host.h>
#include <dfu-cmd.h>
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

#endif /* SRC_ARDUINOMCUOTA_H_ */
