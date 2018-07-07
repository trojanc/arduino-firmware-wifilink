#include "ArduinoFirmwareEsp.h"

uint8_t stop_flash_rotate = 0;              // Allow flash configuration rotation
int restart_flag = 0;                       // Sonoff restart flag

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