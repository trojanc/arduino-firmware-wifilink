/*
 * Settings.cpp
 *
 *  Created on: Jul 7, 2018
 *      Author: Charl-PC
 */


/*
  settings.ino - user settings for Sonoff-Tasmota

  Copyright (C) 2018  Theo Arends

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*********************************************************************************************\
 * Config - Flash
\*********************************************************************************************/


extern "C" {
#include "spi_flash.h"
}
#include "eboot_command.h"
#include "settings.h"
#include "Configuration.h"

extern "C" uint32_t _SPIFFS_end;

// From libraries/EEPROM/EEPROM.cpp EEPROMClass
#define SPIFFS_END          ((uint32_t)&_SPIFFS_end - 0x40200000) / SPI_FLASH_SEC_SIZE
#define SETTINGS_LOCATION   SPIFFS_END  // No need for SPIFFS as it uses EEPROM area
#define CFG_ROTATES         8           // Number of flash sectors used (handles uploads)

uint32_t settings_hash = 0;
uint32_t settings_location = SETTINGS_LOCATION;

/********************************************************************************************/
/*
 * Based on cores/esp8266/Updater.cpp
 */
void SetFlashModeDout()
{
	uint8_t *_buffer;
	uint32_t address;

	eboot_command ebcmd;
	eboot_command_read(&ebcmd);
	address = ebcmd.args[0];
	_buffer = new uint8_t[FLASH_SECTOR_SIZE];

	if (ESP.flashRead(address, (uint32_t*)_buffer, FLASH_SECTOR_SIZE)) {
		if (_buffer[2] != 3) {  // DOUT
			_buffer[2] = 3;
			if (ESP.flashEraseSector(address / FLASH_SECTOR_SIZE)) ESP.flashWrite(address, (uint32_t*)_buffer, FLASH_SECTOR_SIZE);
		}
	}
	delete[] _buffer;
}

uint32_t GetSettingsHash()
{
	uint32_t hash = 0;
	uint8_t *bytes = (uint8_t*)&Settings;

	for (uint16_t i = 0; i < sizeof(SYSCFG); i++) {
		hash += bytes[i]*(i+1);
	}
	return hash;
}

void SettingsSave(){
	settings_location = SETTINGS_LOCATION;
	Settings.save_flag++;
	ESP.flashEraseSector(settings_location);
	ESP.flashWrite(settings_location * SPI_FLASH_SEC_SIZE, (uint32*)&Settings, sizeof(SYSCFG));
	settings_hash = GetSettingsHash();
}

void SettingsSaveAll(){
	SettingsSave();
}


void WifiCheck(uint8_t param)
{
  wifi_counter--;
  switch (param) {
  case WIFI_SMARTCONFIG:
  case WIFI_MANAGER:
  case WIFI_WPSCONFIG:
    WifiConfig(param);
    break;
  default:
    if (wifi_config_counter) {
      wifi_config_counter--;
      wifi_counter = wifi_config_counter +5;
      if (wifi_config_counter) {
        // if ((WIFI_SMARTCONFIG == wifi_config_type) && WiFi.smartConfigDone()) {
        //   wifi_config_counter = 0;
        // }
        // if ((WIFI_WPSCONFIG == wifi_config_type) && WifiWpsConfigDone()) {
        //   wifi_config_counter = 0;
        // }
        if (!wifi_config_counter) {
          if (strlen(WiFi.SSID().c_str())) {
            strlcpy(Settings.sta_ssid[0], WiFi.SSID().c_str(), sizeof(Settings.sta_ssid[0]));
          }
          if (strlen(WiFi.psk().c_str())) {
            strlcpy(Settings.sta_pwd[0], WiFi.psk().c_str(), sizeof(Settings.sta_pwd[0]));
          }
          Settings.sta_active = 0;
          snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_WIFI D_WCFG_1_SMARTCONFIG D_CMND_SSID "1 %s"), Settings.sta_ssid[0]);
          AddLog(LOG_LEVEL_INFO);
        }
      }
      if (!wifi_config_counter) {
        if (WIFI_SMARTCONFIG == wifi_config_type) {
          WiFi.stopSmartConfig();
        }
        SettingsSdkErase();
        restart_flag = 2;
      }
    } else {
      if (wifi_counter <= 0) {
        AddLog_P(LOG_LEVEL_DEBUG_MORE, S_LOG_WIFI, PSTR(D_CHECKING_CONNECTION));
        wifi_counter = WIFI_CHECK_SEC;
        WifiCheckIp();
      }
      if ((WL_CONNECTED == WiFi.status()) && (static_cast<uint32_t>(WiFi.localIP()) != 0) && !wifi_config_type) {
        if (!mdns_begun) {
          mdns_begun = MDNS.begin(my_hostname);
          snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_MDNS "%s"), (mdns_begun) ? D_INITIALIZED : D_FAILED);
          AddLog(LOG_LEVEL_INFO);
        }
          StartWebserver(WiFi.localIP());
          MDNS.addService("http", "tcp", 80);
      } else {
        mdns_begun = false;
      }
    }
  }
}

/*********************************************************************************************\
 * Config Save - Save parameters to Flash ONLY if any parameter has changed
\*********************************************************************************************/

uint32_t GetSettingsAddress(){
	return settings_location * SPI_FLASH_SEC_SIZE;
}

void SettingsDefault()
{
	SettingsDefaultSet1();
  	SettingsDefaultSet2();
	
	SettingsSave();
}


void SettingsDefaultSet1(){
	memset(&Settings, 0x00, sizeof(SYSCFG));

	Settings.cfg_holder = CFG_HOLDER;
	Settings.version = VERSION;
}

void SettingsDefaultSet2(){
	
	memset((char*)&Settings +16, 0x00, sizeof(SYSCFG) -16);

	strlcpy(Settings.sta_ssid[0], STA_SSID1, sizeof(Settings.sta_ssid[0]));
	strlcpy(Settings.sta_pwd[0], STA_PASS1, sizeof(Settings.sta_pwd[0]));
	strlcpy(Settings.sta_ssid[1], STA_SSID2, sizeof(Settings.sta_ssid[1]));
	strlcpy(Settings.sta_pwd[1], STA_PASS2, sizeof(Settings.sta_pwd[1]));
	strlcpy(Settings.hostname, WIFI_HOSTNAME, sizeof(Settings.hostname));

	Settings.mqtt_port = MQTT_PORT;
	Settings.mqtt_retry = MQTT_RETRY_SECS;
	strlcpy(Settings.mqtt_host, MQTT_HOST, sizeof(Settings.mqtt_host));
	strlcpy(Settings.mqtt_client, MQTT_CLIENT_ID, sizeof(Settings.mqtt_client));
	strlcpy(Settings.mqtt_user, MQTT_USER, sizeof(Settings.mqtt_user));
	strlcpy(Settings.mqtt_pwd, MQTT_PASS, sizeof(Settings.mqtt_pwd));
	strlcpy(Settings.mqtt_topic, MQTT_TOPIC, sizeof(Settings.mqtt_topic));
	strlcpy(Settings.mqtt_prefix[0], SUB_PREFIX, sizeof(Settings.mqtt_prefix[0]));
	strlcpy(Settings.mqtt_prefix[1], PUB_PREFIX, sizeof(Settings.mqtt_prefix[1]));
	strlcpy(Settings.mqtt_prefix[2], PUB_PREFIX2, sizeof(Settings.mqtt_prefix[2]));

	Settings.weblog_level = WEB_LOG_LEVEL;
	Settings.tele_period = TELE_PERIOD;
}

void SettingsLoad(){
	ESP.flashRead(settings_location * SPI_FLASH_SEC_SIZE, (uint32*)&Settings, sizeof(SYSCFG));

	if(Settings.cfg_holder != CFG_HOLDER){
		SettingsDefault();
	}
	settings_hash = GetSettingsHash();
}

void SettingsErase(uint8_t type)
{
	/*
    0 = Erase from program end until end of physical flash
    1 = Erase SDK parameter area at end of linker memory model (0x0FDxxx - 0x0FFFFF) solving possible wifi errors
	 */

	bool result;

	uint32_t _sectorStart = (ESP.getSketchSize() / SPI_FLASH_SEC_SIZE) + 1;
	uint32_t _sectorEnd = ESP.getFlashChipRealSize() / SPI_FLASH_SEC_SIZE;
	if (1 == type) {
		_sectorStart = SETTINGS_LOCATION +2;  // SDK parameter area above EEPROM area (0x0FDxxx - 0x0FFFFF)
		_sectorEnd = SETTINGS_LOCATION +5;
	}

	for (uint32_t _sector = _sectorStart; _sector < _sectorEnd; _sector++) {
		result = ESP.flashEraseSector(_sector);
	}
}

bool SettingsEraseConfig(void) {
	const size_t cfgSize = 0x4000;
	size_t cfgAddr = ESP.getFlashChipSize() - cfgSize;

	for (size_t offset = 0; offset < cfgSize; offset += SPI_FLASH_SEC_SIZE) {
		if (!ESP.flashEraseSector((cfgAddr + offset) / SPI_FLASH_SEC_SIZE)) {
			return false;
		}
	}
	return true;
}

void SettingsSdkErase()
{
	WiFi.disconnect(true);    // Delete SDK wifi config
	SettingsErase(1);
	SettingsEraseConfig();
	delay(1000);
}

/********************************************************************************************/
