#ifndef _SRC_I18N_H_
#define _SRC_I18N_H_


// support.ino
#define D_OSWATCH "osWatch"
#define D_BLOCKED_LOOP "Blocked Loop"
#define D_WPS_FAILED_WITH_STATUS "WPSconfig FAILED with status"
#define D_ACTIVE_FOR_3_MINUTES "active for 3 minutes"
#define D_FAILED_TO_START "failed to start"
#define D_PATCH_ISSUE_2186 "Patch issue 2186"
#define D_CONNECTING_TO_AP "Connecting to AP"
#define D_IN_MODE "in mode"
#define D_CONNECT_FAILED_NO_IP_ADDRESS "Connect failed as no IP address received"
#define D_CONNECT_FAILED_AP_NOT_REACHED "Connect failed as AP cannot be reached"
#define D_CONNECT_FAILED_WRONG_PASSWORD "Connect failed with AP incorrect password"
#define D_CONNECT_FAILED_AP_TIMEOUT "Connect failed with AP timeout"
#define D_ATTEMPTING_CONNECTION "Attempting connection..."
#define D_CHECKING_CONNECTION "Checking connection..."
#define D_QUERY_DONE "Query done. MQTT services found"
#define D_MQTT_SERVICE_FOUND "MQTT service found on"
#define D_FOUND_AT "found at"
#define D_SYSLOG_HOST_NOT_FOUND "Syslog Host not found"

// webserver.ino
#define D_MINIMAL_FIRMWARE_PLEASE_UPGRADE "MINIMAL firmware - please upgrade"
#define D_WEBSERVER_ACTIVE_ON "Web server active on"
#define D_WITH_IP_ADDRESS "with IP address"
#define D_WEBSERVER_STOPPED "Web server stopped"
#define D_FILE_NOT_FOUND "File Not Found"
#define D_REDIRECTED "Redirected to captive portal"
#define D_WIFIMANAGER_SET_ACCESSPOINT_AND_STATION "Wifimanager set AccessPoint and keep Station"
#define D_WIFIMANAGER_SET_ACCESSPOINT "Wifimanager set AccessPoint"
#define D_TRYING_TO_CONNECT "Trying to connect device to network"

#define WCFG_MAX_STRING_LENGTH 12
#define D_WCFG_0_RESTART "Restart"
#define D_WCFG_1_SMARTCONFIG "SmartConfig"
#define D_WCFG_2_WIFIMANAGER "WifiManager"
#define D_WCFG_3_WPSCONFIG "WPSConfig"
#define D_WCFG_4_RETRY "Retry"
#define D_WCFG_5_WAIT "Wait"

#define D_UPLOAD_ERR_1 "No file selected"
#define D_UPLOAD_ERR_2 "Not enough space"
#define D_UPLOAD_ERR_3 "Magic byte is not 0xE9"
#define D_UPLOAD_ERR_4 "Program flash size is larger than real flash size"
#define D_UPLOAD_ERR_5 "Upload buffer miscompare"
#define D_UPLOAD_ERR_6 "Upload failed. Enable logging 3"
#define D_UPLOAD_ERR_7 "Upload aborted"
#define D_UPLOAD_ERR_8 "File invalid"
#define D_UPLOAD_ERR_9 "File too large"
#define D_UPLOAD "Upload"
#define D_ENTER_COMMAND "Enter command"
#define D_UPGRADE_STARTED "Upgrade started"
#define D_RESTART_IN "Restart in"
#define D_SECONDS "seconds"
#define D_HTML_LANGUAGE "en"
#define D_MQTT_PARAMETERS "MQTT parameters"
#define D_MQTT_HOST "MQTT Host"
#define D_MQTT_PORT "MQTT Port"
#define D_MQTT_CLIENT "MQTT Client"
#define D_MQTT_USER "MQTT User"
#define D_MQTT_TOPIC "MQTT Topic"
#define D_MQTT_GROUP_TOPIC "MQTT Group Topic"
#define D_MQTT_FULL_TOPIC "MQTT Full Topic"
#define D_WIFI_PARAMETERS "Wifi parameters"
#define D_SAVE_CONFIGURATION "Save configuration"
#define D_CONFIGURATION_SAVED "Configuration saved"
#define D_CONFIGURATION "Configuration"
#define D_SAVE "Save"
#define D_CLIENT "Client"
#define D_FULL_TOPIC "Full Topic"
#define D_USER "User"
#define D_PORT "Port"
#define D_HOSTNAME "Hostname"
#define D_CONNECTED "Connected"
#define D_AS "as"
#define D_SET_BAUDRATE_TO "Set Baudrate to"
#define D_FILE "File"
#define D_HOST "Host"
#define D_TOPIC "Topic"
#define D_PASSWORD "Password"
#define D_AP1_SSID "AP1 SSId"
#define D_AP1_PASSWORD "AP1 Password"
#define D_ATTEMPTING_CONNECTION "Attempting connection..."
#define D_AP2_SSID "AP2 SSId"
#define D_AP2_PASSWORD "AP2 Password"
#define D_ASTERIX "********"
#define D_SUCCESSFUL "Successful"
#define D_RESTARTING "Restarting"
#define D_FIRMWARE_UPGRADE "Firmware Upgrade"
#define D_UPLOAD_DONE "Upload done"
#define D_ONLINE "Online"
#define D_SUBSCRIBE_TO "Subscribe to"
#define D_RETAINED "retained"
#define D_OFFLINE "Offline"
#define D_RSLT_INFO "INFO"
#define D_JSON_RESTARTREASON "RestartReason"

#define D_ATTEMPTING_CONNECTION "Attempting connection..."
#define D_CHECKING_CONNECTION "Checking connection..."
#define D_BOOT_COUNT "Boot Count"
#define D_RETRY_IN "Retry in"
#define D_UNIT_SECOND "sec"
#define D_JSON_FALLBACKTOPIC "FallbackTopic"
#define D_CONNECT_FAILED_TO "Connect failed to"
#define D_JSON_VERSION "Version"
#define D_CMND_IPADDRESS "IPAddress"
#define D_CMND_HOSTNAME "Hostname"
  #define D_JSON_WEBSERVER_MODE "WebServerMode"
#define D_JSON_BLOCKED_LOOP "Blocked Loop"
#define D_INITIALIZED "Initialized"
#define D_ADMIN "Admin"
#define D_CONFIGURE_MQTT "Configure MQTT"
#define D_FAILED "Failed"
#define D_RSLT_RESULT "RESULT"
#define D_LOG_RESULT "RSL: "       // Result
#define D_LOG_APPLICATION "APP: "  // Application
#define D_LOG_UPLOAD "UPL: "       // Upload
#define D_LOG_COMMAND "CMD: "      // Command
#define D_LOG_SERIAL "SER: "       // Serial
#define D_LOG_HTTP "HTP: "         // HTTP webserver
#define D_LOG_MQTT "MQT: "         // MQTT
#define D_LOG_WIFI "WIF: "         // Wifi
#define D_LOG_MDNS "DNS: "         // mDNS
#define D_RECEIVED "Received"
#define D_CONSOLE "Console"
#define D_LWT "LWT"

#define D_CMND_SSID "SSId"
#define D_CMND_UPGRADE "Upgrade"
#define D_JSON_SUCCESSFUL "Successful"
#define D_JSON_RESTARTING "Restarting"
#define D_JSON_FAILED "Failed"
// "2017-03-07T11:08:02" - ISO8601:2004
#define D_YEAR_MONTH_SEPARATOR "-"
#define D_MONTH_DAY_SEPARATOR "-"
#define D_DATE_TIME_SEPARATOR "T"
#define D_HOUR_MINUTE_SEPARATOR ":"
#define D_MINUTE_SECOND_SEPARATOR ":"

const char S_LOG_HTTP[] PROGMEM = D_LOG_HTTP;
const char S_FIRMWARE_UPGRADE[] PROGMEM = D_FIRMWARE_UPGRADE;
const char S_CONFIGURE_MQTT[] PROGMEM = D_CONFIGURE_MQTT;
const char S_SAVE_CONFIGURATION[] PROGMEM = D_SAVE_CONFIGURATION;
const char S_CONSOLE[] PROGMEM = D_CONSOLE;
const char S_LWT[] PROGMEM = D_LWT;
const char S_OFFLINE[] PROGMEM = D_OFFLINE;
const char S_LOG_MQTT[] PROGMEM = D_LOG_MQTT;
const char S_RSLT_RESULT[] PROGMEM = D_RSLT_RESULT;
const char S_LOG_WIFI[] PROGMEM = D_LOG_WIFI;

#define PRFX_MAX_STRING_LENGTH 5
  #define D_CMND "cmnd"
  #define D_STAT "stat"
  #define D_TELE "tele"
const char kPrefixes[3][PRFX_MAX_STRING_LENGTH] PROGMEM = {
  D_CMND,
  D_STAT,
  D_TELE };

#endif // _SRC_I18N_H_