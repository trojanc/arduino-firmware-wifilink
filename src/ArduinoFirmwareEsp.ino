#include "ArduinoFirmwareEsp.h"

#include <Ticker.h>                         // RTC, Energy, OSWatch

#ifdef __cplusplus
extern "C" {
#endif

#include "user_interface.h"

// Function prototypes
void WifiWpsStatusCallback(wps_cb_status status);

#ifdef __cplusplus
}
#endif


uint8_t stop_flash_rotate = 0;              // Allow flash configuration rotation
int restart_flag = 0;                       // Sonoff restart flag
byte web_log_index = 1;                     // Index in Web log buffer (should never be 0)
byte reset_web_log_flag = 0;                // Reset web console log
char log_data[LOGSZ];                       // Logging
char web_log[WEB_LOG_SIZE] = {'\0'};        // Web log buffer
char mqtt_data[MESSZ];                      // MQTT publish buffer and web page ajax buffer
char mqtt_client[33];                       // Composed MQTT Clientname
char mqtt_topic[33];                        // Composed MQTT topic
char my_version[33];                        // Composed version string
char my_hostname[33];                       // Composed Wifi hostname
int tele_period = 0;                        // Tele period timer
int status_update_timer = 0;                // Refresh initial status
uint16_t mqtt_cmnd_publish = 0;             // ignore flag for publish command
unsigned long state_loop_timer = 0;         // State loop timer
int state = 0;                              // State per second flag
int ota_state_flag = 0;                     // OTA state flag
int ota_result = 0;                         // OTA result
boolean mdns_begun = false;
byte seriallog_level;                       // Current copy of Settings.seriallog_level
uint16_t seriallog_timer = 0;               // Timer to disable Seriallog

uint8_t sleep;                              // Current copy of Settings.sleep
int blinks = 201;                           // Number of LED blinks
uint8_t blinkstate = 0;                     // LED state
uint8_t led_inverted = 0;                   // LED inverted flag (1 = (0 = On, 1 = Off))

WiFiClient EspClient;                     // Wifi Client

int ledState = LOW;             // used to set the LED state
long previousMillis = 0;        // will store last time LED was updated
long ap_interval = 50;         //blink interval in ap mode
int wifi_state_flag = WIFI_RESTART;         // Wifi state flag

void setup() {
	Serial.begin(APP_BAUDRATE);
  	delay(10);
 	Serial.println();
	seriallog_level = LOG_LEVEL_INFO;  // Allow specific serial messages until config loaded

	byte idx;
	snprintf_P(my_version, sizeof(my_version), PSTR("%d.%d.%d"), VERSION >> 24 & 0xff, VERSION >> 16 & 0xff, VERSION >> 8 & 0xff);
	if (VERSION & 0x1f) {
		idx = strlen(my_version);
		my_version[idx] = 96 + (VERSION & 0x1f);
		my_version[idx +1] = 0;
	}

	SettingsLoad();

	OsWatchInit();
	seriallog_level = Settings.seriallog_level;
    seriallog_timer = SERIALLOG_TIMER;
	sleep = Settings.sleep;
	Settings.bootcount++;
	snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_APPLICATION D_BOOT_COUNT " %d"), Settings.bootcount);
	AddLog(LOG_LEVEL_DEBUG);

	Format(mqtt_client, Settings.mqtt_client, sizeof(mqtt_client));
	Format(mqtt_topic, Settings.mqtt_topic, sizeof(mqtt_topic));

	if (strstr(Settings.hostname, "%")) {
	strlcpy(Settings.hostname, WIFI_HOSTNAME, sizeof(Settings.hostname));
	snprintf_P(my_hostname, sizeof(my_hostname)-1, Settings.hostname, mqtt_topic, ESP.getChipId() & 0x1FFF);
	} else {
	snprintf_P(my_hostname, sizeof(my_hostname)-1, Settings.hostname);
	}


	// _setup_dfu();
	pinMode(WIFI_LED, OUTPUT);      //initialize wifi LED
	ArduinoOTA.begin();             //OTA ESP
	//   CommunicationLogic.begin();
}

void loop() {

  // XdrvCall(FUNC_LOOP);
  MqttLoop();

  if (millis() >= state_loop_timer) StateLoop();
//  CommunicationLogic.handle();
  PollDnsWebserver();
  // _handle_Mcu_OTA();


  ArduinoOTA.handle();
  //  yield();     // yield == delay(0), delay contains yield, auto yield in loop
  delay(sleep);
}


/*********************************************************************************************\
 * State loop
\*********************************************************************************************/

void StateLoop()
{
  state_loop_timer = millis() + (1000 / STATES);
  state++;

/*-------------------------------------------------------------------------------------------*\
 * Every second
\*-------------------------------------------------------------------------------------------*/

  if (STATES == state) {
    state = 0;
    // PerformEverySecond();
  }

/*-------------------------------------------------------------------------------------------*\
 * Every 0.1 second
\*-------------------------------------------------------------------------------------------*/

  if (!(state % (STATES/10))) {
    if (mqtt_cmnd_publish) mqtt_cmnd_publish--;  // Clean up
  }

/*-------------------------------------------------------------------------------------------*\
 * Every 0.05 second
\*-------------------------------------------------------------------------------------------*/

  // ButtonHandler();
  // SwitchHandler();

  // XdrvCall(FUNC_EVERY_50_MSECOND);
  // XsnsCall(FUNC_EVERY_50_MSECOND);

/*-------------------------------------------------------------------------------------------*\
 * Every 0.2 second
\*-------------------------------------------------------------------------------------------*/

  if (!(state % ((STATES/10)*2))) {
    if (blinks || restart_flag || ota_state_flag) {
      if (restart_flag || ota_state_flag) {
        blinkstate = 1;   // Stay lit
      } else {
        blinkstate ^= 1;  // Blink
      }
      if ((blinks > 200) || (blinkstate)) {
        SetLedPower(blinkstate);
      }
      if (!blinkstate) {
        blinks--;
        if (200 == blinks) blinks = 0;
      }
    }
  }

/*-------------------------------------------------------------------------------------------*\
 * Every second at 0.2 second interval
\*-------------------------------------------------------------------------------------------*/

  switch (state) {
  case (STATES/10)*2:
    // if (ota_state_flag) {
    //   ota_state_flag--;
    //   if (2 == ota_state_flag) {
    //     ota_url = Settings.ota_url;
    //     RtcSettings.ota_loader = 0;  // Try requested image first
    //     ota_retry_counter = OTA_ATTEMPTS;
    //     ESPhttpUpdate.rebootOnUpdate(false);
    //     SettingsSave(1);  // Free flash for OTA update
    //   }
    //   if (ota_state_flag <= 0) {
      //  StopWebserver();
      //   ota_state_flag = 92;
      //   ota_result = 0;
      //   ota_retry_counter--;
      //   if (ota_retry_counter) {
          // strlcpy(mqtt_data, GetOtaUrl(log_data, sizeof(log_data)), sizeof(mqtt_data));
          // if (RtcSettings.ota_loader) {
          //   char *pch = strrchr(mqtt_data, '-');  // Change from filename-DE.bin into filename-minimal.bin
          //   char *ech = strrchr(mqtt_data, '.');  // Change from filename.bin into filename-minimal.bin
          //   if (!pch) pch = ech;
          //   if (pch) {
          //     mqtt_data[pch - mqtt_data] = '\0';
          //     char *ech = strrchr(Settings.ota_url, '.');  // Change from filename.bin into filename-minimal.bin
          //     snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("%s-" D_JSON_MINIMAL "%s"), mqtt_data, ech);  // Minimal filename must be filename-minimal
          //   }
          // }
          // snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_UPLOAD "%s"), mqtt_data);
          // AddLog(LOG_LEVEL_DEBUG);
          // ota_result = (HTTP_UPDATE_FAILED != ESPhttpUpdate.update(mqtt_data));
          // if (!ota_result) {
          //   int ota_error = ESPhttpUpdate.getLastError();
//            snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_UPLOAD "Ota error %d"), ota_error);
//            AddLog(LOG_LEVEL_DEBUG);
      //       ota_state_flag = 2;    // Upgrade failed - retry
      //     }
      //   }
      // }
      // if (90 == ota_state_flag) {  // Allow MQTT to reconnect
      //   ota_state_flag = 0;
      //   if (ota_result) {
      //     SetFlashModeDout();      // Force DOUT for both ESP8266 and ESP8285
      //     snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR(D_JSON_SUCCESSFUL ". " D_JSON_RESTARTING));
      //   } else {
      //     snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR(D_JSON_FAILED " %s"), ESPhttpUpdate.getLastErrorString().c_str());
      //   }
      //   restart_flag = 2;          // Restart anyway to keep memory clean webserver
      //   MqttPublishPrefixTopic_P(STAT, PSTR(D_CMND_UPGRADE));
      // }
    // }
    break;
  case (STATES/10)*4:
    // if (MidnightNow()) CounterSaveState();
    // if (save_data_counter && (backlog_pointer == backlog_index)) {
    //   save_data_counter--;
    //   if (save_data_counter <= 0) {
    //     if (Settings.flag.save_state) {
    //       power_t mask = POWER_MASK;
    //       for (byte i = 0; i < MAX_PULSETIMERS; i++) {
    //         if ((Settings.pulse_timer[i] > 0) && (Settings.pulse_timer[i] < 30)) {  // 3 seconds
    //           mask &= ~(1 << i);
    //         }
    //       }
    //       if (!((Settings.power &mask) == (power &mask))) {
    //         Settings.power = power;
    //       }
    //     } else {
    //       Settings.power = 0;
    //     }
    //     SettingsSave(0);
    //     save_data_counter = Settings.save_data;
    //   }
    // }
    if (restart_flag) {
      if (213 == restart_flag) {
        SettingsSdkErase();  // Erase flash SDK parameters
        restart_flag = 2;
      } else if (212 == restart_flag) {
        SettingsErase(0);    // Erase all flash from program end to end of physical flash
        restart_flag = 211;
      }
      if (211 == restart_flag) {
        SettingsDefault();
        restart_flag = 2;
      }
      SettingsSaveAll();
      restart_flag--;
      if (restart_flag <= 0) {
        AddLog_P(LOG_LEVEL_INFO, PSTR(D_LOG_APPLICATION D_RESTARTING));
        ESP.restart();
      }
    }
    break;
  case (STATES/10)*6:
    WifiCheck(wifi_state_flag);
    wifi_state_flag = WIFI_RESTART;
    break;
  case (STATES/10)*8:
    if (WL_CONNECTED == WiFi.status()) MqttCheck();
    break;
  }
}
void SetLedPower(uint8_t state)
{
  if (state) state = 1;
  digitalWrite(WIFI_LED, (bitRead(led_inverted, 0)) ? !state : state);
}

void MakeValidMqtt(byte option, char* str)
{
// option 0 = replace by underscore
// option 1 = delete character
  uint16_t i = 0;
  while (str[i] > 0) {
//        if ((str[i] == '/') || (str[i] == '+') || (str[i] == '#') || (str[i] == ' ')) {
    if ((str[i] == '+') || (str[i] == '#') || (str[i] == ' ')) {
      if (option) {
        uint16_t j = i;
        while (str[j] > 0) {
          str[j] = str[j +1];
          j++;
        }
        i--;
      } else {
        str[i] = '_';
      }
    }
    i++;
  }
}

char* Format(char* output, const char* input, int size)
{
  char *token;
  uint8_t digits = 0;

  if (strstr(input, "%")) {
    strlcpy(output, input, size);
    token = strtok(output, "%");
    if (strstr(input, "%") == input) {
      output[0] = '\0';
    } else {
      token = strtok(NULL, "");
    }
    if (token != NULL) {
      digits = atoi(token);
      if (digits) {
        if (strchr(token, 'd')) {
          snprintf_P(output, size, PSTR("%s%c0%dd"), output, '%', digits);
          snprintf_P(output, size, output, ESP.getChipId() & 0x1fff);       // %04d - short chip ID in dec, like in hostname
        } else {
          snprintf_P(output, size, PSTR("%s%c0%dX"), output, '%', digits);
          snprintf_P(output, size, output, ESP.getChipId());                // %06X - full chip ID in hex
        }
      } else {
        if (strchr(token, 'd')) {
          snprintf_P(output, size, PSTR("%s%d"), output, ESP.getChipId());  // %d - full chip ID in dec
          digits = 8;
        }
      }
    }
  }
  if (!digits) strlcpy(output, input, size);
  return output;
}


void GetTopic_P(char *stopic, byte prefix, char *topic, const char* subtopic)
{
  /* prefix 0 = Cmnd
     prefix 1 = Stat
     prefix 2 = Tele
  */
  char romram[CMDSZ];
  String fulltopic;

  snprintf_P(romram, sizeof(romram), subtopic);

  fulltopic = Settings.mqtt_fulltopic;
  if ((0 == prefix) && (-1 == fulltopic.indexOf(F(MQTT_TOKEN_PREFIX)))) {
    fulltopic += F("/" MQTT_TOKEN_PREFIX);  // Need prefix for commands to handle mqtt topic loops
  }
  for (byte i = 0; i < 3; i++) {
    if ('\0' == Settings.mqtt_prefix[i][0]) {
      snprintf_P(Settings.mqtt_prefix[i], sizeof(Settings.mqtt_prefix[i]), kPrefixes[i]);
    }
  }
  fulltopic.replace(F(MQTT_TOKEN_PREFIX), Settings.mqtt_prefix[prefix]);
  fulltopic.replace(F(MQTT_TOKEN_TOPIC), topic);
  fulltopic.replace(F("#"), "");
  fulltopic.replace(F("//"), "/");
  if (!fulltopic.endsWith("/")) fulltopic += "/";
  snprintf_P(stopic, TOPSZ, PSTR("%s%s"), fulltopic.c_str(), romram);
}

struct dfu_data *global_dfu;
struct dfu_binary_file *global_binary_file;

static int serial_release(void *dummy)
{
  //stop Serial communication
  Serial.end();
  return 0;
}

static int _setup_dfu(void)
{
global_dfu = dfu_init(&esp8266_serial_arduino_unowifi_interface_ops,
              NULL,
              NULL,
              serial_release,
              NULL,
              &stk500_dfu_target_ops,
              &atmega328p_device_data,
              &esp8266_dfu_host_ops);
  if (!global_dfu) {
    /* FIXME: Is this ok ? */
    return -1;
  }

  global_binary_file = dfu_binary_file_start_rx(&dfu_rx_method_http_arduino, global_dfu, WebServer);
  if (!global_binary_file) {
    return -1;
  }

  if (dfu_binary_file_flush_start(global_binary_file) < 0) {
      return -1;
  }
  return 0;
}

void _finalize_dfu(void)
{
  dfu_binary_file_fini(global_binary_file);
  dfu_fini(global_dfu);
  global_dfu = NULL;
  global_binary_file = NULL;
}

void _handle_Mcu_OTA(void)
{
   if (!global_dfu)
     _setup_dfu();
   if (!global_dfu)
     return;
   switch (dfu_idle(global_dfu)) {
    case DFU_ERROR:
      _finalize_dfu();
      break;
    case DFU_ALL_DONE:
      dfu_target_go(global_dfu);
      _finalize_dfu();
      delay(80);
      //open Serial Communication
      Serial.begin(BAUDRATE_COMMUNICATION);
      break;
    case DFU_CONTINUE:
      break;
    }
}