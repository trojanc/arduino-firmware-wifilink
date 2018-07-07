#include "WebServer.h"

/*********************************************************************************************\
 * Web server and WiFi Manager
 *
 * Enables configuration and reconfiguration of WiFi credentials using a Captive Portal
 * Based on source by AlexT (https://github.com/tzapu)
\*********************************************************************************************/

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// TODO move to language file
#define D_UPLOAD_ERR_1 "No file selected"
#define D_UPLOAD_ERR_2 "Not enough space"
#define D_UPLOAD_ERR_3 "Magic byte is not 0xE9"
#define D_UPLOAD_ERR_4 "Program flash size is larger than real flash size"
#define D_UPLOAD_ERR_5 "Upload buffer miscompare"
#define D_UPLOAD_ERR_6 "Upload failed. Enable logging 3"
#define D_UPLOAD_ERR_7 "Upload aborted"
#define D_UPLOAD_ERR_8 "File invalid"
#define D_UPLOAD_ERR_9 "File too large"

const char HTTP_HEAD[] PROGMEM =
  "<!DOCTYPE html><html lang=\"en\" class=\"\">"
  "<head>"
  "<meta charset='utf-8'>"
  "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\"/>"
  "<title>{h} - {v}</title>"

  "<script>"
  "var cn;"
  "cn=180;"
  "function eb(s){"
    "return document.getElementById(s);"  // Save code space
  "}"
  "function u(){"
    "if(cn>=0){"
      "eb('t').innerHTML='Restart in '+cn+' seconds';"
      "cn--;"
      "setTimeout(u,1000);"
    "}"
  "}"
  "function c(l){"
    "eb('s1').value=l.innerText||l.textContent;"
    "eb('p1').focus();"
  "}";

const char HTTP_HEAD_STYLE[] PROGMEM =
  "</script>"
  "<style>"
  "div,fieldset,input,select{padding:5px;font-size:1em;}"
  "input{width:100%;box-sizing:border-box;-webkit-box-sizing:border-box;-moz-box-sizing:border-box;}"
  "select{width:100%;}"
  "textarea{resize:none;width:98%;height:318px;padding:5px;overflow:auto;}"
  "body{text-align:center;font-family:verdana;}"
  "td{padding:0px;}"
  "button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;-webkit-transition-duration:0.4s;transition-duration:0.4s;cursor:pointer;}"
  "button:hover{background-color:#0e70a4;}"
  ".bred{background-color:#d43535;}"
  ".bred:hover{background-color:#931f1f;}"
  ".bgrn{background-color:#47c266;}"
  ".bgrn:hover{background-color:#5aaf6f;}"
  "a{text-decoration:none;}"
  ".p{float:left;text-align:left;}"
  ".q{float:right;text-align:right;}"
  "</style>"

  "</head>"
  "<body>"
  "<div style='text-align:left;display:inline-block;min-width:340px;'>"
  "<div style='text-align:center;'><h3>{ha Arduino Uno Wifi</h3><h2>{h}</h2></div>";

const char HTTP_BTN_MENU1[] PROGMEM =
  "<br/><form action='cn' method='get'><button>Configuration</button></form>"
  "<br/><form action='in' method='get'><button>Information</button></form>"
  "<br/><form action='up' method='get'><button>Firmware Upgrade</button></form>"
  "<br/><form action='cs' method='get'><button>Console</button></form>";

const char HTTP_BTN_RSTRT[] PROGMEM =
  "<br/><form action='rb' method='get' onsubmit='return confirm(\"Are you sure to restart?\");'><button class='button bred'>Restart</button></form>";

const char HTTP_END[] PROGMEM =
  "<br/>"
  "<div style='text-align:right;font-size:11px;'><hr/><a href='#' target='_blank' style='color:#aaa;'>FIRMWARE NAME {mv by TrojanC</a></div>"
  "</div>"
  "</body>"
  "</html>";

const char HTTP_SCRIPT_INFO_BEGIN[] PROGMEM =
  "function i(){"
    "var s,o=\"";
const char HTTP_SCRIPT_INFO_END[] PROGMEM =
    "\";"                   // "}1" and "}2" means do not use "}x" in Information text
    "s=o.replace(/}1/g,\"</td></tr><tr><th>\").replace(/}2/g,\"</th><td>\");"
    "eb('i').innerHTML=s;"
  "}"
  "</script>";

const char HTTP_FORM_UPG[] PROGMEM =
  "<div id='f1' name='f1' style='display:block;'>"
  "<fieldset><legend><b>&nbsp;Upgrade by file upload&nbsp;</b></legend>";
const char HTTP_FORM_RST_UPG[] PROGMEM =
  "<form method='post' action='u2' enctype='multipart/form-data'>"
  "<br/><input type='file' name='u2'><br/>"
  "<br/><button type='submit' onclick='eb(\"f1\").style.display=\"none\";eb(\"f2\").style.display=\"block\";this.form.submit();'>Start Upgrade {r1</button></form>"
  "</fieldset>"
  "</div>"
  "<div id='f2' name='f2' style='display:none;text-align:center;'><b>Upload Started ...</b></div>";



const char HTTP_BTN_MAIN[] PROGMEM =
  "<br/><br/><form action='.' method='get'><button>Main Menu</button></form>";
const char HTTP_BTN_MENU_WIFI[] PROGMEM =
  "<br/><form action='w0' method='get'><button>Configure Wifi</button></form>";
const char HTTP_BTN_MENU_MQTT[] PROGMEM =
  "<br/><form action='mq' method='get'><button>Configure MQTT</button></form>";


const char HTTP_MSG_RSTRT[] PROGMEM =
  "<br/><div style='text-align:center;'>Device will restart</div><br/>";

const char HDR_CTYPE_HTML[] PROGMEM = "text/html";


uint8_t upload_error = 0;
uint8_t upload_file_type;
uint8_t *settings_new = NULL;
uint8_t upload_progress_dot_count;
uint8_t config_block_count = 0;
uint8_t config_xor_on = 0;
uint8_t config_xor_on_set = CONFIG_FILE_XOR;

// Helper function to avoid code duplication (saves 4k Flash)
static void WebGetArg(const char* arg, char* out, size_t max)
{
  String s = server.arg(arg);
  strncpy(out, s.c_str(), max);
  out[max-1] = '\0';  // Ensure terminating NUL
}

void SetHeader()
{
	server.sendHeader(F("Cache-Control"), F("no-cache, no-store, must-revalidate"));
	server.sendHeader(F("Pragma"), F("no-cache"));
	server.sendHeader(F("Expires"), F("-1"));
	server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
}
void ShowPage(String &page) {
  page.replace(F("{ha"), "Arduino Uno Wifi");
  page.replace(F("{h}"), "Arduino Uno Wifi Friendly");
  page += FPSTR(HTTP_END);
  page.replace(F("{mv"), FW_VERSION);
  SetHeader();
  server.send(200, FPSTR(HDR_CTYPE_HTML), page);
}

void HandleRoot(){
	String page = FPSTR(HTTP_HEAD);
	page.replace(F("{v}"), "Main Menu");
	page += FPSTR(HTTP_HEAD_STYLE);
	page.replace(F("<body>"), F("<body onload='la()'>"));
	page += F("<div id='l1' name='l1'></div>");
	page += FPSTR(HTTP_BTN_MENU1);
	page += FPSTR(HTTP_BTN_RSTRT);
	ShowPage(page);
}



void HandleInformation(){
  char stopic[TOPSZ];

  int freeMem = ESP.getFreeHeap();

  String page = FPSTR(HTTP_HEAD);
  page.replace(F("{v}"), "Information");
  page += FPSTR(HTTP_HEAD_STYLE);
  //  page += F("<fieldset><legend><b>&nbsp;Information&nbsp;</b></legend>");

  page += F("<style>td{padding:0px 5px;}</style>");
  page += F("<div id='i' name='i'></div>");

  // Save 1k of code space replacing table html with javascript replace codes
  // }1 = </td></tr><tr><th>
  // }2 = </th><td>
  String func = FPSTR(HTTP_SCRIPT_INFO_BEGIN);
  func += F("<table style='width:100%'><tr><th>");
  func += F("Program Version }2"); func += FW_VERSION;
//  func += F("}1 Build Date & Time }2"); func += GetBuildDateAndTime();
  func += F("}1 Core/SDK Version }2" ARDUINO_ESP8266_RELEASE "/"); func += String(ESP.getSdkVersion());
//  func += F("}1 Uptime }2"); func += GetDateAndTime(DT_UPTIME);
//  snprintf_P(stopic, sizeof(stopic), PSTR(" at %X"), GetSettingsAddress());
//  func += F("}1" D_FLASH_WRITE_COUNT "}2"); func += String(Settings.save_flag); func += stopic;
//  func += F("}1" D_BOOT_COUNT "}2"); func += String(Settings.bootcount);
//  func += F("}1" D_RESTART_REASON "}2"); func += GetResetReason();
//  uint8_t maxfn = (devices_present > MAX_FRIENDLYNAMES) ? MAX_FRIENDLYNAMES : devices_present;
//  for (byte i = 0; i < maxfn; i++) {
//    func += F("}1" D_FRIENDLY_NAME " "); func += i +1; func += F("}2"); func += Settings.friendlyname[i];
//  }
//
//  func += F("}1}2&nbsp;");  // Empty line
//  func += F("}1" D_AP); func += String(Settings.sta_active +1);
//    func += F(" " D_SSID " (" D_RSSI ")}2"); func += Settings.sta_ssid[Settings.sta_active]; func += F(" ("); func += WifiGetRssiAsQuality(WiFi.RSSI()); func += F("%)");
//  func += F("}1" D_HOSTNAME "}2"); func += my_hostname;
  if (static_cast<uint32_t>(WiFi.localIP()) != 0) {
    func += F("}1IP Address}2"); func += WiFi.localIP().toString();
//    func += F("}1Gateway}2"); func += IPAddress(Settings.ip_address[1]).toString();
//    func += F("}1Subnet Mask}2"); func += IPAddress(Settings.ip_address[2]).toString();
//    func += F("}1DNS Server}2"); func += IPAddress(Settings.ip_address[3]).toString();
    func += F("}1MAC Address}2"); func += WiFi.macAddress();
  }
  if (static_cast<uint32_t>(WiFi.softAPIP()) != 0) {
    func += F("}1AP IP Address}2"); func += WiFi.softAPIP().toString();
    func += F("}1AP Gateway}2"); func += WiFi.softAPIP().toString();
    func += F("}1AP MAC Address}2"); func += WiFi.softAPmacAddress();
  }
//
//  func += F("}1}2&nbsp;");  // Empty line
//  if (Settings.flag.mqtt_enabled) {
//    func += F("}1" D_MQTT_HOST "}2"); func += Settings.mqtt_host;
//    func += F("}1" D_MQTT_PORT "}2"); func += String(Settings.mqtt_port);
//    func += F("}1" D_MQTT_CLIENT " &<br/>&nbsp;" D_FALLBACK_TOPIC "}2"); func += mqtt_client;
//    func += F("}1" D_MQTT_USER "}2"); func += Settings.mqtt_user;
//    func += F("}1" D_MQTT_TOPIC "}2"); func += Settings.mqtt_topic;
//    func += F("}1" D_MQTT_GROUP_TOPIC "}2"); func += Settings.mqtt_grptopic;
//    GetTopic_P(stopic, CMND, mqtt_topic, "");
//    func += F("}1" D_MQTT_FULL_TOPIC "}2"); func += stopic;
//
//  } else {
//    func += F("}1" D_MQTT "}2" D_DISABLED);
//  }

  func += F("}1}2&nbsp;");  // Empty line

//  func += F("}1" D_MDNS_DISCOVERY "}2");
//#ifdef USE_DISCOVERY
//  func += F(D_ENABLED);
//  func += F("}1" D_MDNS_ADVERTISE "}2");
//#ifdef WEBSERVER_ADVERTISE
//  func += F(D_WEB_SERVER);
//#else
//  func += F(D_DISABLED);
//#endif // WEBSERVER_ADVERTISE
//#else
//  func += F(D_DISABLED);
//#endif // USE_DISCOVERY

  func += F("}1}2&nbsp;");  // Empty line
  func += F("}1ESP Chip Id}2"); func += String(ESP.getChipId());
  func += F("}1Flash Chip Id}2"); func += String(ESP.getFlashChipId());
  func += F("}1Flash Size}2"); func += String(ESP.getFlashChipRealSize() / 1024); func += F("kB");
  func += F("}1Program Flash Size}2"); func += String(ESP.getFlashChipSize() / 1024); func += F("kB");
  func += F("}1Program Size}2"); func += String(ESP.getSketchSize() / 1024); func += F("kB");
  func += F("}1Free Program Space}2"); func += String(ESP.getFreeSketchSpace() / 1024); func += F("kB");
  func += F("}1Free Program upload}2"); func += String(( (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000) / 1024); func += F("kB");
 ;
  func += F("}1Free Memory}2"); func += String(freeMem / 1024); func += F("kB");
  func += F("</td></tr></table>");
  func += FPSTR(HTTP_SCRIPT_INFO_END);
  page.replace(F("</script>"), func);
  page.replace(F("<body>"), F("<body onload='i()'>"));

  //  page += F("</fieldset>");
  page += FPSTR(HTTP_BTN_MAIN);
  ShowPage(page);
}



void HandleConfiguration()
{
  String page = FPSTR(HTTP_HEAD);
  page.replace(F("{v}"), "Configuration");
  page += FPSTR(HTTP_HEAD_STYLE);
  page += FPSTR(HTTP_BTN_MENU_WIFI);
  page += FPSTR(HTTP_BTN_MENU_MQTT);
  page += FPSTR(HTTP_BTN_MAIN);
  ShowPage(page);
}

void SettingsNewFree()
{
  if (settings_new != NULL) {
    free(settings_new);
    settings_new = NULL;
  }
}

void HandleUpgradeFirmware()
{
//   if (HttpUser()) { return; }
//   AddLog_P(LOG_LEVEL_DEBUG, S_LOG_HTTP, S_FIRMWARE_UPGRADE);

  String page = FPSTR(HTTP_HEAD);
  page.replace(F("{v}"), "Firmware Upgrade");
  page += FPSTR(HTTP_HEAD_STYLE);
  page += FPSTR(HTTP_FORM_UPG);
  page += FPSTR(HTTP_FORM_RST_UPG);
  page.replace(F("{r1"), "Upgrade");
  page += FPSTR(HTTP_BTN_MAIN);
  ShowPage(page);

  upload_error = 0;
  upload_file_type = 0;
}

void HandleUploadLoop()
{
  // Based on ESP8266HTTPUpdateServer.cpp uses ESP8266WebServer Parsing.cpp and Cores Updater.cpp (Update)
//   boolean _serialoutput = (LOG_LEVEL_DEBUG <= seriallog_level);

//   if (HTTP_USER == webserver_state) { return; }
  if (upload_error) {
    if (!upload_file_type) { Update.end(); }
    return;
  }
  HTTPUpload& upload = server.upload();

  if (UPLOAD_FILE_START == upload.status) {
    restart_flag = 60;
    if (0 == upload.filename.c_str()[0]) {
      upload_error = 1;
      return;
    }
    SettingsSave();  // Free flash for upload
    // snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_UPLOAD D_FILE " %s ..."), upload.filename.c_str());
    // AddLog(LOG_LEVEL_INFO);
    if (upload_file_type) {
      SettingsNewFree();
      if (!(settings_new = (uint8_t *)malloc(sizeof(Settings)))) {
        upload_error = 2;
        return;
      }
    } else {
    //   MqttRetryCounter(60);
    //   if (Settings.flag.mqtt_enabled) MqttDisconnect();
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace)) {         //start with max available size
        upload_error = 2;
        return;
      }
    }
    upload_progress_dot_count = 0;
  } else if (!upload_error && (UPLOAD_FILE_WRITE == upload.status)) {
    if (0 == upload.totalSize) {
      if (upload_file_type) {
        if (upload.buf[0] != CONFIG_FILE_SIGN) {
          upload_error = 8;
          return;
        }
        config_xor_on = upload.buf[1];
        config_block_count = 0;
      } else {
        if (upload.buf[0] != 0xE9) {
          upload_error = 3;
          return;
        }
        uint32_t bin_flash_size = ESP.magicFlashChipSize((upload.buf[3] & 0xf0) >> 4);
        if(bin_flash_size > ESP.getFlashChipRealSize()) {
          upload_error = 4;
          return;
        }
        upload.buf[2] = 3;  // Force DOUT - ESP8285
      }
    }
    if (upload_file_type) { // config
      if (!upload_error) {
        if (upload.currentSize > (sizeof(Settings) - (config_block_count * HTTP_UPLOAD_BUFLEN))) {
          upload_error = 9;
          return;
        }
        memcpy(settings_new + (config_block_count * HTTP_UPLOAD_BUFLEN), upload.buf, upload.currentSize);
        config_block_count++;
      }
    } else {  // firmware
      if (!upload_error && (Update.write(upload.buf, upload.currentSize) != upload.currentSize)) {
        upload_error = 5;
        return;
      }
    }
  } else if(!upload_error && (UPLOAD_FILE_END == upload.status)) {
    if (upload_file_type) {
      if (config_xor_on) {
        for (uint16_t i = 2; i < sizeof(Settings); i++) {
          settings_new[i] ^= (config_xor_on_set +i);
        }
      }
      SettingsDefaultSet2();
      memcpy((char*)&Settings +16, settings_new +16, sizeof(Settings) -16);
      memcpy((char*)&Settings +8, settings_new +8, 4);  // Restore version and auto upgrade
      SettingsNewFree();
    } else {
      if (!Update.end(true)) { // true to set the size to the current progress
        upload_error = 6;
        return;
      }
    }
    if (!upload_error) {
    //   snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_UPLOAD D_SUCCESSFUL " %u bytes. " D_RESTARTING), upload.totalSize);
    //   AddLog(LOG_LEVEL_INFO);
    }
  } else if (UPLOAD_FILE_ABORTED == upload.status) {
    restart_flag = 0;
    // MqttRetryCounter(0);
    upload_error = 7;
    if (!upload_file_type) { Update.end(); }
  }
  delay(0);
}


void HandleUpgradeFirmwareStart()
{
//   if (HttpUser()) { return; }
  char svalue[100];

//   AddLog_P(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP D_UPGRADE_STARTED));
//   WifiConfigCounter();

  char tmp[100];
  String page = FPSTR(HTTP_HEAD);
  page.replace(F("{v}"), "Information");
  page += FPSTR(HTTP_HEAD_STYLE);
  page += F("<div style='text-align:center;'><b>Upgrade Started ...</b></div>");
  page += FPSTR(HTTP_MSG_RSTRT);
  page += FPSTR(HTTP_BTN_MAIN);
  ShowPage(page);

//   snprintf_P(svalue, sizeof(svalue), PSTR("Upgrade 1"));
//   ExecuteCommand(svalue);
}

void HandleUploadDone()
{
//   if (HttpUser()) { return; }
//   AddLog_P(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP D_UPLOAD_DONE));

  char error[100];

//   WifiConfigCounter();
  restart_flag = 0;
//   MqttRetryCounter(0);

  String page = FPSTR(HTTP_HEAD);
  page.replace(F("{v}"), "Information");
  page += FPSTR(HTTP_HEAD_STYLE);
  page += F("<div style='text-align:center;'><b>Upload <font color='");
  if (upload_error) {
    page += F("red'>Failed</font></b><br/><br/>");
    switch (upload_error) {
      case 1: strncpy_P(error, PSTR(D_UPLOAD_ERR_1), sizeof(error)); break;
      case 2: strncpy_P(error, PSTR(D_UPLOAD_ERR_2), sizeof(error)); break;
      case 3: strncpy_P(error, PSTR(D_UPLOAD_ERR_3), sizeof(error)); break;
      case 4: strncpy_P(error, PSTR(D_UPLOAD_ERR_4), sizeof(error)); break;
      case 5: strncpy_P(error, PSTR(D_UPLOAD_ERR_5), sizeof(error)); break;
      case 6: strncpy_P(error, PSTR(D_UPLOAD_ERR_6), sizeof(error)); break;
      case 7: strncpy_P(error, PSTR(D_UPLOAD_ERR_7), sizeof(error)); break;
      case 8: strncpy_P(error, PSTR(D_UPLOAD_ERR_8), sizeof(error)); break;
      case 9: strncpy_P(error, PSTR(D_UPLOAD_ERR_9), sizeof(error)); break;
      default:
        snprintf_P(error, sizeof(error), PSTR("Upload error code %d"), upload_error);
    }
    page += error;
    // snprintf_P(log_data, sizeof(log_data), PSTR(D_UPLOAD ": %s"), error);
    // AddLog(LOG_LEVEL_DEBUG);
    // stop_flash_rotate = Settings.flag.stop_flash_rotate;
  } else {
    page += F("green'>Successful</font></b><br/>");
    page += FPSTR(HTTP_MSG_RSTRT);
    restart_flag = 2;
  }
  SettingsNewFree();
  page += F("</div><br/>");
  page += FPSTR(HTTP_BTN_MAIN);
  ShowPage(page);
}

void PollDnsWebserver()
{
  // if (DnsServer) { DnsServer->processNextRequest(); }
  server.handleClient();
}

void initWebServer(){

//	tot = WiFi.scanNetworks();

	server.on("/", HandleRoot);
	server.on("/in", HandleInformation);
	server.on("/cn", HandleConfiguration);
	server.on("/up", HandleUpgradeFirmware);
    server.on("/u1", HandleUpgradeFirmwareStart);  // OTA
    server.on("/u2", HTTP_POST, HandleUploadDone, HandleUploadLoop);
	server.onNotFound([](){
		server.send(404, "text/plain", "FileNotFound");
	});

	server.begin();
	delay(5); // VB: exactly 5, no more or less, no yield()!
}
