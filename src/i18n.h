#ifndef _SRC_I18N_H_
#define _SRC_I18N_H_


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
#define D_ADMIN "Admin"
#define D_CONFIGURE_MQTT "Configure MQTT"
#define D_RSLT_RESULT "RESULT"
#define D_LOG_RESULT "RSL: "       // Result
#define D_LOG_APPLICATION "APP: "  // Application
#define D_LOG_UPLOAD "UPL: "       // Upload
#define D_LOG_COMMAND "CMD: "      // Command
#define D_LOG_SERIAL "SER: "       // Serial
#define D_LOG_HTTP "HTP: "         // HTTP webserver
#define D_LOG_MQTT "MQT: "         // MQTT
#define D_RECEIVED "Received"
#define D_CONSOLE "Console"
#define D_LWT "LWT"

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

#define PRFX_MAX_STRING_LENGTH 5
  #define D_CMND "cmnd"
  #define D_STAT "stat"
  #define D_TELE "tele"
const char kPrefixes[3][PRFX_MAX_STRING_LENGTH] PROGMEM = {
  D_CMND,
  D_STAT,
  D_TELE };

#endif // _SRC_I18N_H_