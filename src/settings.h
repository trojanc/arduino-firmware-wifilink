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

struct TIME_T {
  uint8_t       second;
  uint8_t       minute;
  uint8_t       hour;
  uint8_t       day_of_week;               // sunday is day 1
  uint8_t       day_of_month;
  uint8_t       month;
  char          name_of_month[4];
  uint16_t      day_of_year;
  uint16_t      year;
  unsigned long days;
  unsigned long valid;
} RtcTime;


typedef union {                            // Restricted by MISRA-C Rule 18.4 but so usefull...
  uint32_t data;                           // Allow bit manipulation using SetOption
  struct {
    uint32_t save_state : 1;
    uint32_t stop_flash_rotate : 1;
    uint32_t mqtt_response : 1;
  };
} SysBitfield;

struct SYSCFG {
  unsigned long cfg_holder;
  unsigned long save_flag;
  unsigned long version;
  unsigned long bootcount;
  SysBitfield   flag;
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
  uint16_t      mqtt_retry;
  char          mqtt_prefix[3][11];
  uint32_t      ip_address[4];
  byte          weblog_level;
  uint16_t      tele_period;
  byte          sta_active;
} Settings;


// Functions
void SettingsDefault(void);
void SettingsDefaultSet1(void);
void SettingsDefaultSet2(void);
void SettingsLoad(void);
void SettingsSave(void);
#endif /* SRC_SETTINGS_H_ */
