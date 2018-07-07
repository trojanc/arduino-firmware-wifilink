#include "ArduinoFirmwareEsp.h"

uint8_t stop_flash_rotate = 0;              // Allow flash configuration rotation
int restart_flag = 0;                       // Sonoff restart flag
byte web_log_index = 1;                     // Index in Web log buffer (should never be 0)
byte reset_web_log_flag = 0;                // Reset web console log
char log_data[LOGSZ];                       // Logging
char web_log[WEB_LOG_SIZE] = {'\0'};        // Web log buffer
char mqtt_data[MESSZ];                      // MQTT publish buffer and web page ajax buffer

int ledState = LOW;             // used to set the LED state
long previousMillis = 0;        // will store last time LED was updated
long ap_interval = 50;         //blink interval in ap mode

ESP8266WebServer server(80);    //server UI

void setup() {
  _setup_dfu();
  pinMode(WIFI_LED, OUTPUT);      //initialize wifi LED
  ArduinoOTA.begin();             //OTA ESP
  SettingsLoad();
  initMDNS();
  CommunicationLogic.begin();
  SPIFFS.begin();
  setupWifi();
  initWebServer();                 //UI begin
}

void loop() {

  ArduinoOTA.handle();
//  CommunicationLogic.handle();
  PollDnsWebserver();
//  wifiLed();
  // _handle_Mcu_OTA();

  //  yield();     // yield == delay(0), delay contains yield, auto yield in loop
  delay(0);
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


// void MqttPublishPrefixTopic_P(uint8_t prefix, const char* subtopic, boolean retained)
// {
// /* prefix 0 = cmnd using subtopic
//  * prefix 1 = stat using subtopic
//  * prefix 2 = tele using subtopic
//  * prefix 4 = cmnd using subtopic or RESULT
//  * prefix 5 = stat using subtopic or RESULT
//  * prefix 6 = tele using subtopic or RESULT
//  */
//   char romram[33];
//   char stopic[TOPSZ];

//   snprintf_P(romram, sizeof(romram), ((prefix > 3) && !Settings.flag.mqtt_response) ? S_RSLT_RESULT : subtopic);
//   for (byte i = 0; i < strlen(romram); i++) {
//     romram[i] = toupper(romram[i]);
//   }
//   prefix &= 3;
//   GetTopic_P(stopic, prefix, mqtt_topic, romram);
//   MqttPublish(stopic, retained);
// }

// void MqttPublishPrefixTopic_P(uint8_t prefix, const char* subtopic)
// {
//   MqttPublishPrefixTopic_P(prefix, subtopic, false);
// }

void initMDNS(){

  MDNS.begin(Settings.hostname);
  MDNS.setInstanceName(Settings.hostname);
  MDNS.addServiceTxt("arduino", "tcp", "fw_name", FW_NAME);
  MDNS.addServiceTxt("arduino", "tcp", "fw_version", FW_VERSION);
  MDNS.addService("http", "tcp", 80);

}


void setupWifi(){
  WiFi.hostname(Settings.hostname);
	WiFi.begin(Settings.sta_ssid[0], Settings.sta_pwd[0]);
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

  global_binary_file = dfu_binary_file_start_rx(&dfu_rx_method_http_arduino, global_dfu, &server);
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