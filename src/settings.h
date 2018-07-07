/*
 * settings.h
 *
 *  Created on: Jul 7, 2018
 *      Author: Charl-PC
 */

#ifndef SRC_SETTINGS_H_
#define SRC_SETTINGS_H_

#include <Esp.h>
#include <ESP8266WiFi.h>

#define PARAM8_SIZE  18                    // Number of param bytes
typedef union {                            // Restricted by MISRA-C Rule 18.4 but so usefull...
  uint32_t data;                           // Allow bit manipulation using SetOption
  struct {
    uint32_t save_state : 1;
    uint32_t stop_flash_rotate : 1;
  };
} SysBitfield;

struct SYSCFG {
  unsigned long cfg_holder;
  unsigned long save_flag;
  unsigned long version;
  unsigned long bootcount;
  SysBitfield   flag;                      // 010 Add flag since 5.0.2
  int16_t       save_data;
  char          sta_ssid[2][33];
  char          sta_pwd[2][65];
  char          hostname[33];
  char          syslog_host[33];
  char          mqtt_host[33];
  uint16_t      mqtt_port;
  char          mqtt_client[33];
  char          mqtt_user[33];
  char          mqtt_pwd[33];
  char          mqtt_topic[33];
  char          mqtt_fulltopic[100];
  uint32_t      ip_address[4];
} Settings;


// Functions
void SettingsDefault(void);
void SettingsDefaultSet1(void);
void SettingsDefaultSet2(void);
void SettingsLoad(void);
void SettingsSave(void);
#endif /* SRC_SETTINGS_H_ */
