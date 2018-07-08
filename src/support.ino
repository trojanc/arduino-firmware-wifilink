
#define OSWATCH_RESET_TIME 120

/*********************************************************************************************\
 * Watchdog extension (https://github.com/esp8266/Arduino/issues/1532)
\*********************************************************************************************/

Ticker tickerOSWatch;
static unsigned long oswatch_last_loop_time;
byte oswatch_blocked_loop = 0;

/*********************************************************************************************\
 * Wifi
\*********************************************************************************************/

#define WIFI_CONFIG_SEC        180  // seconds before restart
#define WIFI_CHECK_SEC         20   // seconds
#define WIFI_RETRY_OFFSET_SEC  20   // seconds
uint8_t wifi_counter;
uint8_t wifi_retry_init;
uint8_t wifi_retry;
uint8_t wifi_status;
uint8_t wps_result;
uint8_t wifi_config_type = 0;
uint8_t wifi_config_counter = 0;


boolean WifiConfigCounter()
{
  if (wifi_config_counter) {
    wifi_config_counter = WIFI_CONFIG_SEC;
  }
  return (wifi_config_counter);
}

void WifiBegin(uint8_t flag)
{
  const char kWifiPhyMode[] = " BGN";
  WiFi.disconnect(true);    // Delete SDK wifi config
  delay(200);
  WiFi.mode(WIFI_STA);      // Disable AP mode
  if (Settings.sleep) {
#ifndef ARDUINO_ESP8266_RELEASE_2_4_1     // See https://github.com/arendst/Sonoff-Tasmota/issues/2559
    WiFi.setSleepMode(WIFI_LIGHT_SLEEP);  // Allow light sleep during idle times
#endif
  }
//  if (WiFi.getPhyMode() != WIFI_PHY_MODE_11N) {
//    WiFi.setPhyMode(WIFI_PHY_MODE_11N);
//  }
  if (!WiFi.getAutoConnect()) {
    WiFi.setAutoConnect(true);
  }
//  WiFi.setAutoReconnect(true);
  switch (flag) {
  case 0:  // AP1
  case 1:  // AP2
    Settings.sta_active = flag;
    break;
  case 2:  // Toggle
    Settings.sta_active ^= 1;
  }        // 3: Current AP
  if (0 == strlen(Settings.sta_ssid[1])) {
    Settings.sta_active = 0;
  }
  if (Settings.ip_address[0]) {
    WiFi.config(Settings.ip_address[0], Settings.ip_address[1], Settings.ip_address[2], Settings.ip_address[3]);  // Set static IP
  }
  WiFi.hostname(my_hostname);
  WiFi.begin(Settings.sta_ssid[Settings.sta_active], Settings.sta_pwd[Settings.sta_active]);
  snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_WIFI D_CONNECTING_TO_AP "%d %s " D_IN_MODE " 11%c " D_AS " %s..."),
    Settings.sta_active +1, Settings.sta_ssid[Settings.sta_active], kWifiPhyMode[WiFi.getPhyMode() & 0x3], my_hostname);
  AddLog(LOG_LEVEL_INFO);
}

void OsWatchTicker()
{
  unsigned long t = millis();
  unsigned long last_run = abs(t - oswatch_last_loop_time);
  if (last_run >= (OSWATCH_RESET_TIME * 1000)) {
    // RtcSettings.oswatch_blocked_loop = 1;
    // RtcSettingsSave();
    ESP.reset();  // hard reset
  }
}

void OsWatchInit()
{
//   oswatch_blocked_loop = RtcSettings.oswatch_blocked_loop;
//   RtcSettings.oswatch_blocked_loop = 0;
  oswatch_last_loop_time = millis();
  tickerOSWatch.attach_ms(((OSWATCH_RESET_TIME / 3) * 1000), OsWatchTicker);
}

void OsWatchLoop()
{
  oswatch_last_loop_time = millis();
}

extern "C" {
#include "user_interface.h"
}

void WifiWpsStatusCallback(wps_cb_status status);

void WifiWpsStatusCallback(wps_cb_status status)
{
/* from user_interface.h:
  enum wps_cb_status {
    WPS_CB_ST_SUCCESS = 0,
    WPS_CB_ST_FAILED,
    WPS_CB_ST_TIMEOUT,
    WPS_CB_ST_WEP,      // WPS failed because that WEP is not supported
    WPS_CB_ST_SCAN_ERR, // can not find the target WPS AP
  };
*/
  wps_result = status;
  if (WPS_CB_ST_SUCCESS == wps_result) {
    wifi_wps_disable();
  } else {
    snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_WIFI D_WPS_FAILED_WITH_STATUS " %d"), wps_result);
    AddLog(LOG_LEVEL_DEBUG);
    wifi_config_counter = 2;
  }
}

boolean WifiWpsConfigDone(void)
{
  return (!wps_result);
}


boolean WifiWpsConfigBegin(void)
{
  wps_result = 99;
  if (!wifi_wps_disable()) {
    return false;
  }
  if (!wifi_wps_enable(WPS_TYPE_PBC)) {
    return false;  // so far only WPS_TYPE_PBC is supported (SDK 2.0.0)
  }
  if (!wifi_set_wps_cb((wps_st_cb_t) &WifiWpsStatusCallback)) {
    return false;
  }
  if (!wifi_wps_start()) {
    return false;
  }
  return true;
}

void WifiCheckIp()
{
  if ((WL_CONNECTED == WiFi.status()) && (static_cast<uint32_t>(WiFi.localIP()) != 0)) {
    wifi_counter = WIFI_CHECK_SEC;
    wifi_retry = wifi_retry_init;
    AddLog_P((wifi_status != WL_CONNECTED) ? LOG_LEVEL_INFO : LOG_LEVEL_DEBUG_MORE, S_LOG_WIFI, PSTR(D_CONNECTED));
    if (wifi_status != WL_CONNECTED) {
//      AddLog_P(LOG_LEVEL_INFO, PSTR("Wifi: Set IP addresses"));
      Settings.ip_address[1] = (uint32_t)WiFi.gatewayIP();
      Settings.ip_address[2] = (uint32_t)WiFi.subnetMask();
      Settings.ip_address[3] = (uint32_t)WiFi.dnsIP();
    }
    wifi_status = WL_CONNECTED;
  } else {
    wifi_status = WiFi.status();
    switch (wifi_status) {
      case WL_CONNECTED:
        AddLog_P(LOG_LEVEL_INFO, S_LOG_WIFI, PSTR(D_CONNECT_FAILED_NO_IP_ADDRESS));
        wifi_status = 0;
        wifi_retry = wifi_retry_init;
        break;
      case WL_NO_SSID_AVAIL:
        AddLog_P(LOG_LEVEL_INFO, S_LOG_WIFI, PSTR(D_CONNECT_FAILED_AP_NOT_REACHED));
        if (WIFI_WAIT == Settings.sta_config) {
          wifi_retry = wifi_retry_init;
        } else {
          if (wifi_retry > (wifi_retry_init / 2)) {
            wifi_retry = wifi_retry_init / 2;
          }
          else if (wifi_retry) {
            wifi_retry = 0;
          }
        }
        break;
      case WL_CONNECT_FAILED:
        AddLog_P(LOG_LEVEL_INFO, S_LOG_WIFI, PSTR(D_CONNECT_FAILED_WRONG_PASSWORD));
        if (wifi_retry > (wifi_retry_init / 2)) {
          wifi_retry = wifi_retry_init / 2;
        }
        else if (wifi_retry) {
          wifi_retry = 0;
        }
        break;
      default:  // WL_IDLE_STATUS and WL_DISCONNECTED
        if (!wifi_retry || ((wifi_retry_init / 2) == wifi_retry)) {
          AddLog_P(LOG_LEVEL_INFO, S_LOG_WIFI, PSTR(D_CONNECT_FAILED_AP_TIMEOUT));
        } else {
          AddLog_P(LOG_LEVEL_DEBUG, S_LOG_WIFI, PSTR(D_ATTEMPTING_CONNECTION));
        }
    }
    if (wifi_retry) {
      if (wifi_retry_init == wifi_retry) {
        WifiBegin(3);  // Select default SSID
      }
      if ((Settings.sta_config != WIFI_WAIT) && ((wifi_retry_init / 2) == wifi_retry)) {
        WifiBegin(2);  // Select alternate SSID
      }
      wifi_counter = 1;
      wifi_retry--;
    } else {
      WifiConfig(Settings.sta_config);
      wifi_counter = 1;
      wifi_retry = wifi_retry_init;
    }
  }
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
        if ((WIFI_SMARTCONFIG == wifi_config_type) && WiFi.smartConfigDone()) {
          wifi_config_counter = 0;
        }
        if ((WIFI_WPSCONFIG == wifi_config_type) && WifiWpsConfigDone()) {
          wifi_config_counter = 0;
        }
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

void WifiConfig(uint8_t type)
{
  if (!wifi_config_type) {
    if (type >= WIFI_RETRY) {  // WIFI_RETRY and WIFI_WAIT
      return;
    }
    WiFi.disconnect();        // Solve possible Wifi hangs
    wifi_config_type = type;
    wifi_config_counter = WIFI_CONFIG_SEC;   // Allow up to WIFI_CONFIG_SECS seconds for phone to provide ssid/pswd
    wifi_counter = wifi_config_counter +5;
    blinks = 1999;
    if (WIFI_RESTART == wifi_config_type) {
      restart_flag = 2;
    }
    else if (WIFI_SMARTCONFIG == wifi_config_type) {
      AddLog_P(LOG_LEVEL_INFO, S_LOG_WIFI, PSTR(D_WCFG_1_SMARTCONFIG " " D_ACTIVE_FOR_3_MINUTES));
      WiFi.beginSmartConfig();
    }
    else if (WIFI_WPSCONFIG == wifi_config_type) {
      if (WifiWpsConfigBegin()) {
        AddLog_P(LOG_LEVEL_INFO, S_LOG_WIFI, PSTR(D_WCFG_3_WPSCONFIG " " D_ACTIVE_FOR_3_MINUTES));
      } else {
        AddLog_P(LOG_LEVEL_INFO, S_LOG_WIFI, PSTR(D_WCFG_3_WPSCONFIG " " D_FAILED_TO_START));
        wifi_config_counter = 3;
      }
    }
    AddLog_P(LOG_LEVEL_INFO, S_LOG_WIFI, PSTR(D_WCFG_2_WIFIMANAGER " " D_ACTIVE_FOR_3_MINUTES));
    WifiManagerBegin();
  }
}



void AddLog_P(byte loglevel, const char *formatP)
{
  snprintf_P(log_data, sizeof(log_data), formatP);
  AddLog(loglevel);
}

void AddLog_P(byte loglevel, const char *formatP, const char *formatP2)
{
  char message[100];

  snprintf_P(log_data, sizeof(log_data), formatP);
  snprintf_P(message, sizeof(message), formatP2);
  strncat(log_data, message, sizeof(log_data));
  AddLog(loglevel);
}

void AddLogSerial(byte loglevel, uint8_t *buffer, int count)
{
  snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_SERIAL D_RECEIVED));
  for (int i = 0; i < count; i++) {
    snprintf_P(log_data, sizeof(log_data), PSTR("%s %02X"), log_data, *(buffer++));
  }
  AddLog(loglevel);
}

void SetSerialBaudrate(int baudrate)
{
  Settings.baudrate = baudrate / 1200;
  if (Serial.baudRate() != baudrate) {
    if (seriallog_level) {
      snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_APPLICATION D_SET_BAUDRATE_TO " %d"), baudrate);
      AddLog(LOG_LEVEL_INFO);
    }
    delay(100);
    Serial.flush();
    Serial.begin(baudrate);
    delay(10);
    Serial.println();
  }
}

void AddLog(byte loglevel)
{
  char mxtime[10];  // "13:45:21 "

  snprintf_P(mxtime, sizeof(mxtime), PSTR("%02d" D_HOUR_MINUTE_SEPARATOR "%02d" D_MINUTE_SECOND_SEPARATOR "%02d "), RtcTime.hour, RtcTime.minute, RtcTime.second);
if (loglevel <= seriallog_level) {
    Serial.printf("%s%s\n", mxtime, log_data);
  }
  if (loglevel <= Settings.weblog_level) {
    // Delimited, zero-terminated buffer of log lines.
    // Each entry has this format: [index][log data]['\1']
    if (!web_log_index) web_log_index++;   // Index 0 is not allowed as it is the end of char string
    while (web_log_index == web_log[0] ||  // If log already holds the next index, remove it
           strlen(web_log) + strlen(log_data) + 13 > WEB_LOG_SIZE)  // 13 = web_log_index + mxtime + '\1' + '\0'
    {
      char* it = web_log;
      it++;                                // Skip web_log_index
      it += strchrspn(it, '\1');           // Skip log line
      it++;                                // Skip delimiting "\1"
      memmove(web_log, it, WEB_LOG_SIZE -(it-web_log));  // Move buffer forward to remove oldest log line
    }
    snprintf_P(web_log, sizeof(web_log), PSTR("%s%c%s%s\1"), web_log, web_log_index++, mxtime, log_data);
    if (!web_log_index) web_log_index++;   // Index 0 is not allowed as it is the end of char string
  }

}

// Get span until single character in string
size_t strchrspn(const char *str1, int character)
{
  size_t ret = 0;
  char *start = (char*)str1;
  char *end = strchr(str1, character);
  if (end) ret = end - start;
  return ret;
}

void GetLog(byte idx, char** entry_pp, size_t* len_p)
{
  char* entry_p = NULL;
  size_t len = 0;

  if (idx) {
    char* it = web_log;
    do {
      byte cur_idx = *it;
      it++;
      size_t tmp = strchrspn(it, '\1');
      tmp++;                             // Skip terminating '\1'
      if (cur_idx == idx) {              // Found the requested entry
        len = tmp;
        entry_p = it;
        break;
      }
      it += tmp;
    } while (it < web_log + WEB_LOG_SIZE && *it != '\0');
  }
  *entry_pp = entry_p;
  *len_p = len;
}

String GetResetReason()
{
  char buff[32];
  if (oswatch_blocked_loop) {
    strncpy_P(buff, PSTR(D_JSON_BLOCKED_LOOP), sizeof(buff));
    return String(buff);
  } else {
    return ESP.getResetReason();
  }
}