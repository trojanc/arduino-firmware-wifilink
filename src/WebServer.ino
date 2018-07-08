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

const char HDR_CTYPE_XML[] PROGMEM = "text/xml";

const char HTTP_HEAD[] PROGMEM =
  "<!DOCTYPE html><html lang=\"" D_HTML_LANGUAGE "\" class=\"\">"
  "<head>"
  "<meta charset='utf-8'>"
  "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\"/>"
  "<title>{h} - {v}</title>"

  "<script>"
  "var cn,x,lt;"
  "cn=180;"
  "x=null;"                  // Allow for abortion
  "function eb(s){"
    "return document.getElementById(s);"  // Save code space
  "}"
  "function u(){"
    "if(cn>=0){"
      "eb('t').innerHTML='" D_RESTART_IN " '+cn+' " D_SECONDS "';"
      "cn--;"
      "setTimeout(u,1000);"
    "}"
  "}"
  "function c(l){"
    "eb('s1').value=l.innerText||l.textContent;"
    "eb('p1').focus();"
  "}"
  "function la(p){"
    "var a='';"
    "if(la.arguments.length==1){"
      "a=p;"
      "clearTimeout(lt);"
    "}"
    "if(x!=null){x.abort();}"    // Abort if no response within 2 seconds (happens on restart 1)
    "x=new XMLHttpRequest();"
    "x.onreadystatechange=function(){"
      "if(x.readyState==4&&x.status==200){"
        "var s=x.responseText.replace(/{t}/g,\"<table style='width:100%'>\").replace(/{s}/g,\"<tr><th>\").replace(/{m}/g,\"</th><td>\").replace(/{e}/g,\"</td></tr>\").replace(/{c}/g,\"%'><div style='text-align:center;font-weight:\");"
        "eb('l1').innerHTML=s;"
      "}"
    "};"
    "x.open('GET','ay'+a,true);"
    "x.send();"
    "lt=setTimeout(la,2345);"
  "}"
  "function lb(p){"
    "la('?d='+p);"
  "}"
  "function lc(p){"
    "la('?t='+p);"
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

const char HTTP_FORM_MQTT[] PROGMEM =
  "<fieldset><legend><b>&nbsp;" D_MQTT_PARAMETERS "&nbsp;</b></legend><form method='get' action='sv'>"
  "<input id='w' name='w' value='2,1' hidden>"
  "<br/><b>" D_HOST "</b> (" MQTT_HOST ")<br/><input id='mh' name='mh' placeholder='" MQTT_HOST" ' value='{m1'><br/>"
  "<br/><b>" D_PORT "</b> (" STR(MQTT_PORT) ")<br/><input id='ml' name='ml' placeholder='" STR(MQTT_PORT) "' value='{m2'><br/>"
  "<br/><b>" D_CLIENT "</b> ({m0)<br/><input id='mc' name='mc' placeholder='" MQTT_CLIENT_ID "' value='{m3'><br/>"
  "<br/><b>" D_USER "</b> (" MQTT_USER ")<br/><input id='mu' name='mu' placeholder='" MQTT_USER "' value='{m4'><br/>"
  "<br/><b>" D_PASSWORD "</b><br/><input id='mp' name='mp' type='password' placeholder='" MQTT_PASS "' value='{m5'><br/>"
  "<br/><b>" D_TOPIC "</b> = %topic% (" MQTT_TOPIC ")<br/><input id='mt' name='mt' placeholder='" MQTT_TOPIC" ' value='{m6'><br/>"
  "<br/><b>" D_FULL_TOPIC "</b> (" MQTT_FULLTOPIC ")<br/><input id='mf' name='mf' placeholder='" MQTT_FULLTOPIC" ' value='{m7'><br/>";


const char HTTP_FORM_WIFI[] PROGMEM =
  "<fieldset><legend><b>&nbsp;" D_WIFI_PARAMETERS "&nbsp;</b></legend><form method='get' action='sv'>"
  "<input id='w' name='w' value='1,1' hidden>"
  "<br/><b>" D_AP1_SSID "</b> (" STA_SSID1 ")<br/><input id='s1' name='s1' placeholder='" STA_SSID1 "' value='{s1'><br/>"
  "<br/><b>" D_AP1_PASSWORD "</b><br/><input id='p1' name='p1' type='password' placeholder='" D_AP1_PASSWORD "' value='" D_ASTERIX "'><br/>"
  "<br/><b>" D_AP2_SSID "</b> (" STA_SSID2 ")<br/><input id='s2' name='s2' placeholder='" STA_SSID2 "' value='{s2'><br/>"
  "<br/><b>" D_AP2_PASSWORD "</b><br/><input id='p2' name='p2' type='password' placeholder='" D_AP2_PASSWORD "' value='" D_ASTERIX "'><br/>"
  "<br/><b>" D_HOSTNAME "</b> (" WIFI_HOSTNAME ")<br/><input id='h' name='h' placeholder='" WIFI_HOSTNAME" ' value='{h1'><br/>";


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


const char HTTP_SCRIPT_CONSOL[] PROGMEM =
  "var sn=0;"                    // Scroll position
  "var id=0;"                    // Get most of weblog initially
  "function l(p){"               // Console log and command service
    "var c,o,t;"
    "clearTimeout(lt);"
    "o='';"
    "t=eb('t1');"
    "if(p==1){"
      "c=eb('c1');"
      "o='&c1='+encodeURIComponent(c.value);"
      "c.value='';"
      "t.scrollTop=sn;"
    "}"
    "if(t.scrollTop>=sn){"       // User scrolled back so no updates
      "if(x!=null){x.abort();}"  // Abort if no response within 2 seconds (happens on restart 1)
      "x=new XMLHttpRequest();"
      "x.onreadystatechange=function(){"
        "if(x.readyState==4&&x.status==200){"
          "var z,d;"
          "d=x.responseXML;"
          "id=d.getElementsByTagName('i')[0].childNodes[0].nodeValue;"
          "if(d.getElementsByTagName('j')[0].childNodes[0].nodeValue==0){t.value='';}"
          "z=d.getElementsByTagName('l')[0].childNodes;"
          "if(z.length>0){t.value+=decodeURIComponent(z[0].nodeValue);}"
          "t.scrollTop=99999;"
          "sn=t.scrollTop;"
        "}"
      "};"
      "x.open('GET','ax?c2='+id+o,true);"
      "x.send();"
    "}"
    "lt=setTimeout(l,2345);"
    "return false;"
  "}"
  "</script>";
const char HTTP_FORM_CMND[] PROGMEM =
  "<br/><textarea readonly id='t1' name='t1' cols='340' wrap='off'></textarea><br/><br/>"
  "<form method='get' onsubmit='return l(1);'>"
  "<input id='c1' name='c1' placeholder='" D_ENTER_COMMAND "' autofocus><br/>"
  //  "<br/><button type='submit'>Send command</button>"
  "</form>";
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


/**
 * Buttons
 */

const char HTTP_BTN_MAIN[] PROGMEM =
  "<br/><br/><form action='.' method='get'><button>Main Menu</button></form>";
const char HTTP_BTN_MENU_WIFI[] PROGMEM =
  "<br/><form action='w0' method='get'><button>Configure Wifi</button></form>";
const char HTTP_BTN_MENU_MQTT[] PROGMEM =
  "<br/><form action='mq' method='get'><button>Configure MQTT</button></form>";
const char HTTP_FORM_END[] PROGMEM =
  "<br/><button type='submit' class='button bgrn'>" D_SAVE "</button></form></fieldset>";
const char HTTP_BTN_CONF[] PROGMEM =
  "<br/><br/><form action='cn' method='get'><button>" D_CONFIGURATION "</button></form>";

const char HTTP_MSG_RSTRT[] PROGMEM =
  "<br/><div style='text-align:center;'>Device will restart</div><br/>";

const char HDR_CTYPE_HTML[] PROGMEM = "text/html";

#define DNS_PORT 53
enum HttpOptions {HTTP_OFF, HTTP_USER, HTTP_ADMIN, HTTP_MANAGER};

ESP8266WebServer *WebServer;    //server UI
DNSServer *DnsServer;

uint8_t upload_error = 0;
uint8_t upload_file_type;
uint8_t *settings_new = NULL;
uint8_t upload_progress_dot_count;
uint8_t config_block_count = 0;
uint8_t config_xor_on = 0;
uint8_t config_xor_on_set = CONFIG_FILE_XOR;
uint8_t webserver_state = HTTP_OFF;

// Helper function to avoid code duplication (saves 4k Flash)
static void WebGetArg(const char* arg, char* out, size_t max)
{
  String s = WebServer->arg(arg);
  strncpy(out, s.c_str(), max);
  out[max-1] = '\0';  // Ensure terminating NUL
}

void SetHeader()
{
	WebServer->sendHeader(F("Cache-Control"), F("no-cache, no-store, must-revalidate"));
	WebServer->sendHeader(F("Pragma"), F("no-cache"));
	WebServer->sendHeader(F("Expires"), F("-1"));
	WebServer->sendHeader(F("Access-Control-Allow-Origin"), F("*"));
}
void ShowPage(String &page) {
  page.replace(F("{ha"), "Arduino Uno Wifi");
  page.replace(F("{h}"), "Arduino Uno Wifi Friendly");
  page += FPSTR(HTTP_END);
  page.replace(F("{mv"), FW_VERSION);
  SetHeader();
  WebServer->send(200, FPSTR(HDR_CTYPE_HTML), page);
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
  func += F("}1" D_BOOT_COUNT "}2"); func += String(Settings.bootcount);
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
  AddLog_P(LOG_LEVEL_DEBUG, S_LOG_HTTP, S_FIRMWARE_UPGRADE);

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
  HTTPUpload& upload = WebServer->upload();

  if (UPLOAD_FILE_START == upload.status) {
    restart_flag = 60;
    if (0 == upload.filename.c_str()[0]) {
      upload_error = 1;
      return;
    }
    SettingsSave();  // Free flash for upload
    snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_UPLOAD D_FILE " %s ..."), upload.filename.c_str());
    AddLog(LOG_LEVEL_INFO);
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
      snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_UPLOAD D_SUCCESSFUL " %u bytes. " D_RESTARTING), upload.totalSize);
      AddLog(LOG_LEVEL_INFO);
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

  AddLog_P(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP D_UPGRADE_STARTED));
  WifiConfigCounter();

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
  AddLog_P(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP D_UPLOAD_DONE));

  char error[100];

  WifiConfigCounter();
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
    snprintf_P(log_data, sizeof(log_data), PSTR(D_UPLOAD ": %s"), error);
    AddLog(LOG_LEVEL_DEBUG);
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


void HandleMqttConfiguration()
{
  // if (HttpUser()) { return; }
  AddLog_P(LOG_LEVEL_DEBUG, S_LOG_HTTP, S_CONFIGURE_MQTT);

  String page = FPSTR(HTTP_HEAD);
  page.replace(F("{v}"), "Configure MQTT");
  page += FPSTR(HTTP_HEAD_STYLE);
  page += FPSTR(HTTP_FORM_MQTT);
  char str[sizeof(Settings.mqtt_client)];
  page.replace(F("{m0"), Format(str, MQTT_CLIENT_ID, sizeof(Settings.mqtt_client)));
  page.replace(F("{m1"), Settings.mqtt_host);
  page.replace(F("{m2"), String(Settings.mqtt_port));
  page.replace(F("{m3"), Settings.mqtt_client);
  page.replace(F("{m4"), (Settings.mqtt_user[0] == '\0')?"0":Settings.mqtt_user);
  page.replace(F("{m5"), (Settings.mqtt_pwd[0] == '\0')?"0":Settings.mqtt_pwd);
  page.replace(F("{m6"), Settings.mqtt_topic);
  page.replace(F("{m7"), Settings.mqtt_fulltopic);
  page += FPSTR(HTTP_FORM_END);
  page += FPSTR(HTTP_BTN_CONF);
  ShowPage(page);
}


void HandleSaveSettings()
{
  // if (HttpUser()) { return; }

  char stemp[TOPSZ];
  char stemp2[TOPSZ];
  String result = "";

  AddLog_P(LOG_LEVEL_DEBUG, S_LOG_HTTP, S_SAVE_CONFIGURATION);

  char tmp[100];
  WebGetArg("w", tmp, sizeof(tmp));  // Returns "5,1" where 5 is config type and 1 is restart flag
  char *p = tmp;
  uint8_t what = strtol(p, &p, 10);
  p++;  // Skip comma
  uint8_t restart = strtol(p, &p, 10);
  switch (what) {
  case 1:
//     WebGetArg("h", tmp, sizeof(tmp));
//     strlcpy(Settings.hostname, (!strlen(tmp)) ? WIFI_HOSTNAME : tmp, sizeof(Settings.hostname));
//     if (strstr(Settings.hostname,"%")) {
//       strlcpy(Settings.hostname, WIFI_HOSTNAME, sizeof(Settings.hostname));
//     }
//     WebGetArg("s1", tmp, sizeof(tmp));
//     strlcpy(Settings.sta_ssid[0], (!strlen(tmp)) ? STA_SSID1 : tmp, sizeof(Settings.sta_ssid[0]));
//     WebGetArg("s2", tmp, sizeof(tmp));
//     strlcpy(Settings.sta_ssid[1], (!strlen(tmp)) ? STA_SSID2 : tmp, sizeof(Settings.sta_ssid[1]));
// //    WebGetArg("s1", tmp, sizeof(tmp));
// //    strlcpy(Settings.sta_ssid[0], (!strlen(tmp)) ? "" : tmp, sizeof(Settings.sta_ssid[0]));
// //    WebGetArg("s2", tmp, sizeof(tmp));
// //    strlcpy(Settings.sta_ssid[1], (!strlen(tmp)) ? "" : tmp, sizeof(Settings.sta_ssid[1]));
//     WebGetArg("p1", tmp, sizeof(tmp));
//     strlcpy(Settings.sta_pwd[0], (!strlen(tmp)) ? "" : (strchr(tmp,'*')) ? Settings.sta_pwd[0] : tmp, sizeof(Settings.sta_pwd[0]));
//     WebGetArg("p2", tmp, sizeof(tmp));
//     strlcpy(Settings.sta_pwd[1], (!strlen(tmp)) ? "" : (strchr(tmp,'*')) ? Settings.sta_pwd[1] : tmp, sizeof(Settings.sta_pwd[1]));
//     // snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_WIFI D_CMND_HOSTNAME " %s, " D_CMND_SSID "1 %s, " D_CMND_SSID "2 %s"),
//     //   Settings.hostname, Settings.sta_ssid[0], Settings.sta_ssid[1]);
//     // AddLog(LOG_LEVEL_INFO);
//     result += F("<br/>" D_TRYING_TO_CONNECT "<br/>");
//     break;
  case 2:
    WebGetArg("mt", tmp, sizeof(tmp));
    strlcpy(stemp, (!strlen(tmp)) ? MQTT_TOPIC : tmp, sizeof(stemp));
    MakeValidMqtt(0, stemp);
    WebGetArg("mf", tmp, sizeof(tmp));
    strlcpy(stemp2, (!strlen(tmp)) ? MQTT_FULLTOPIC : tmp, sizeof(stemp2));
    MakeValidMqtt(1,stemp2);
    // if ((strcmp(stemp, Settings.mqtt_topic)) || (strcmp(stemp2, Settings.mqtt_fulltopic))) {
    //   snprintf_P(mqtt_data, sizeof(mqtt_data), (Settings.flag.mqtt_offline) ? S_OFFLINE : "");
    //   MqttPublishPrefixTopic_P(TELE, S_LWT, true);  // Offline or remove previous retained topic
    // }
    strlcpy(Settings.mqtt_topic, stemp, sizeof(Settings.mqtt_topic));
    strlcpy(Settings.mqtt_fulltopic, stemp2, sizeof(Settings.mqtt_fulltopic));
    WebGetArg("mh", tmp, sizeof(tmp));
    strlcpy(Settings.mqtt_host, (!strlen(tmp)) ? MQTT_HOST : (!strcmp(tmp,"0")) ? "" : tmp, sizeof(Settings.mqtt_host));
    WebGetArg("ml", tmp, sizeof(tmp));
    Settings.mqtt_port = (!strlen(tmp)) ? MQTT_PORT : atoi(tmp);
    WebGetArg("mc", tmp, sizeof(tmp));
    strlcpy(Settings.mqtt_client, (!strlen(tmp)) ? MQTT_CLIENT_ID : tmp, sizeof(Settings.mqtt_client));
    WebGetArg("mu", tmp, sizeof(tmp));
    strlcpy(Settings.mqtt_user, (!strlen(tmp)) ? MQTT_USER : (!strcmp(tmp,"0")) ? "" : tmp, sizeof(Settings.mqtt_user));
    WebGetArg("mp", tmp, sizeof(tmp));
    strlcpy(Settings.mqtt_pwd, (!strlen(tmp)) ? MQTT_PASS : (!strcmp(tmp,"0")) ? "" : tmp, sizeof(Settings.mqtt_pwd));
    // snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_MQTT D_CMND_MQTTHOST " %s, " D_CMND_MQTTPORT " %d, " D_CMND_MQTTCLIENT " %s, " D_CMND_MQTTUSER " %s, " D_CMND_MQTTPASSWORD " %s, " D_CMND_TOPIC " %s, " D_CMND_FULLTOPIC " %s"),
    //   Settings.mqtt_host, Settings.mqtt_port, Settings.mqtt_client, Settings.mqtt_user, Settings.mqtt_pwd, Settings.mqtt_topic, Settings.mqtt_fulltopic);
    // AddLog(LOG_LEVEL_INFO);
    break;
  case 3:
    // WebGetArg("ls", tmp, sizeof(tmp));
    // Settings.seriallog_level = (!strlen(tmp)) ? SERIAL_LOG_LEVEL : atoi(tmp);
    // WebGetArg("lw", tmp, sizeof(tmp));
    // Settings.weblog_level = (!strlen(tmp)) ? WEB_LOG_LEVEL : atoi(tmp);
    // WebGetArg("ll", tmp, sizeof(tmp));
    // Settings.syslog_level = (!strlen(tmp)) ? SYS_LOG_LEVEL : atoi(tmp);
    // // syslog_level = Settings.syslog_level;
    // // syslog_timer = 0;
    // WebGetArg("lh", tmp, sizeof(tmp));
    // strlcpy(Settings.syslog_host, (!strlen(tmp)) ? SYS_LOG_HOST : tmp, sizeof(Settings.syslog_host));
    // WebGetArg("lp", tmp, sizeof(tmp));
    // Settings.syslog_port = (!strlen(tmp)) ? SYS_LOG_PORT : atoi(tmp);
    // WebGetArg("lt", tmp, sizeof(tmp));
    // Settings.tele_period = (!strlen(tmp)) ? TELE_PERIOD : atoi(tmp);
    // if ((Settings.tele_period > 0) && (Settings.tele_period < 10)) {
    //   Settings.tele_period = 10;   // Do not allow periods < 10 seconds
    // }
    // snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_LOG D_CMND_SERIALLOG " %d, " D_CMND_WEBLOG " %d, " D_CMND_SYSLOG " %d, " D_CMND_LOGHOST " %s, " D_CMND_LOGPORT " %d, " D_CMND_TELEPERIOD " %d"),
    //   Settings.seriallog_level, Settings.weblog_level, Settings.syslog_level, Settings.syslog_host, Settings.syslog_port, Settings.tele_period);
    // AddLog(LOG_LEVEL_INFO);
    break;
  // case 5:
  //   WebGetArg("p1", tmp, sizeof(tmp));
  //   strlcpy(Settings.web_password, (!strlen(tmp)) ? "" : (strchr(tmp,'*')) ? Settings.web_password : tmp, sizeof(Settings.web_password));
  //   Settings.flag.mqtt_enabled = WebServer->hasArg("b1");
  //   WebGetArg("a1", tmp, sizeof(tmp));
  //   strlcpy(Settings.friendlyname[0], (!strlen(tmp)) ? FRIENDLY_NAME : tmp, sizeof(Settings.friendlyname[0]));
  //   WebGetArg("a2", tmp, sizeof(tmp));
  //   strlcpy(Settings.friendlyname[1], (!strlen(tmp)) ? FRIENDLY_NAME"2" : tmp, sizeof(Settings.friendlyname[1]));
  //   WebGetArg("a3", tmp, sizeof(tmp));
  //   strlcpy(Settings.friendlyname[2], (!strlen(tmp)) ? FRIENDLY_NAME"3" : tmp, sizeof(Settings.friendlyname[2]));
  //   WebGetArg("a4", tmp, sizeof(tmp));
  //   strlcpy(Settings.friendlyname[3], (!strlen(tmp)) ? FRIENDLY_NAME"4" : tmp, sizeof(Settings.friendlyname[3]));
  //   snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_OTHER D_MQTT_ENABLE " %s, " D_CMND_EMULATION " %d, " D_CMND_FRIENDLYNAME " %s, %s, %s, %s"),
  //     GetStateText(Settings.flag.mqtt_enabled), Settings.flag2.emulation, Settings.friendlyname[0], Settings.friendlyname[1], Settings.friendlyname[2], Settings.friendlyname[3]);
  //   AddLog(LOG_LEVEL_INFO);
  //   break;
  case 6:
    // WebGetArg("g99", tmp, sizeof(tmp));
    // byte new_module = (!strlen(tmp)) ? MODULE : atoi(tmp);
    // Settings.last_module = Settings.module;
    // Settings.module = new_module;
    // mytmplt cmodule;
    // memcpy_P(&cmodule, &kModules[Settings.module], sizeof(cmodule));
    // String gpios = "";
    // for (byte i = 0; i < MAX_GPIO_PIN; i++) {
    //   if (Settings.last_module != new_module) {
    //     Settings.my_gp.io[i] = 0;
    //   } else {
    //     if (GPIO_USER == cmodule.gp.io[i]) {
    //       snprintf_P(stemp, sizeof(stemp), PSTR("g%d"), i);
    //       WebGetArg(stemp, tmp, sizeof(tmp));
    //       Settings.my_gp.io[i] = (!strlen(tmp)) ? 0 : atoi(tmp);
    //       gpios += F(", " D_GPIO ); gpios += String(i); gpios += F(" "); gpios += String(Settings.my_gp.io[i]);
    //     }
    //   }
    // }
    // snprintf_P(stemp, sizeof(stemp), kModules[Settings.module].name);
    // snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_MODULE "%s " D_CMND_MODULE "%s"), stemp, gpios.c_str());
    // AddLog(LOG_LEVEL_INFO);
    break;
  }

  if (restart) {
    String page = FPSTR(HTTP_HEAD);
    page.replace(F("{v}"), "Save Configuration");
    page += FPSTR(HTTP_HEAD_STYLE);
    page += F("<div style='text-align:center;'><b>" D_CONFIGURATION_SAVED "</b><br/>");
    page += result;
    page += F("</div>");
    page += FPSTR(HTTP_MSG_RSTRT);
    page += FPSTR(HTTP_BTN_MAIN);
    ShowPage(page);

    restart_flag = 2;
  } else {
    HandleConfiguration();
  }
}

void HandleWifiConfiguration(){
  //   if (HttpUser()) { return; }

  // AddLog_P(LOG_LEVEL_DEBUG, S_LOG_HTTP, S_CONFIGURE_WIFI);

  String page = FPSTR(HTTP_HEAD);
  page.replace(F("{v}"), "Configure Wifi");
  page += FPSTR(HTTP_HEAD_STYLE);
  page += FPSTR(HTTP_FORM_WIFI);
  page.replace(F("{h1"), Settings.hostname);
  page.replace(F("{s1"), Settings.sta_ssid[0]);
  page.replace(F("{s2"), Settings.sta_ssid[1]);
  page += FPSTR(HTTP_FORM_END);
  page += FPSTR(HTTP_BTN_CONF);
  ShowPage(page);
}


void HandleConsole()
{
  // if (HttpUser()) { return; }
  AddLog_P(LOG_LEVEL_DEBUG, S_LOG_HTTP, S_CONSOLE);

  String page = FPSTR(HTTP_HEAD);
  page.replace(F("{v}"), FPSTR(S_CONSOLE));
  page += FPSTR(HTTP_HEAD_STYLE);
  page.replace(F("</script>"), FPSTR(HTTP_SCRIPT_CONSOL));
  page.replace(F("<body>"), F("<body onload='l()'>"));
  page += FPSTR(HTTP_FORM_CMND);
  page += FPSTR(HTTP_BTN_MAIN);
  ShowPage(page);
}


void HandleAjaxConsoleRefresh()
{
  // if (HttpUser()) { return; }
  char svalue[INPUT_BUFFER_SIZE];  // Large to serve Backlog
  byte cflg = 1;
  byte counter = 0;                // Initial start, should never be 0 again

  WebGetArg("c1", svalue, sizeof(svalue));
  if (strlen(svalue)) {
    snprintf_P(log_data, sizeof(log_data), PSTR(D_LOG_COMMAND "%s"), svalue);
    AddLog(LOG_LEVEL_INFO);
    // ExecuteCommand(svalue);
  }

  WebGetArg("c2", svalue, sizeof(svalue));
  if (strlen(svalue)) { counter = atoi(svalue); }

  byte last_reset_web_log_flag = reset_web_log_flag;
  String message = F("}9");  // Cannot load mqtt_data here as <> will be encoded by replacements below
  if (!reset_web_log_flag) {
    counter = 0;
    reset_web_log_flag = 1;
  }
  if (counter != web_log_index) {
    if (!counter) {
      counter = web_log_index;
      cflg = 0;
    }
    do {
      char* tmp;
      size_t len;
      GetLog(counter, &tmp, &len);
      if (len) {
        if (cflg) {
          message += F("\n");
        } else {
          cflg = 1;
        }
        strlcpy(mqtt_data, tmp, len);
        message += mqtt_data;
      }
      counter++;
      if (!counter) { counter++; } // Skip 0 as it is not allowed
    } while (counter != web_log_index);
    // XML encoding to fix blank console log in concert with javascript decodeURIComponent
    message.replace(F("%"), F("%25"));  // Needs to be done first as otherwise the % in %26 will also be converted
    message.replace(F("&"), F("%26"));
    message.replace(F("<"), F("%3C"));
    message.replace(F(">"), F("%3E"));
  }
  snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("<r><i>%d</i><j>%d</j><l>"), web_log_index, last_reset_web_log_flag);
  message.replace(F("}9"), mqtt_data);  // Save to load here
  message += F("</l></r>");
  WebServer->send(200, FPSTR(HDR_CTYPE_XML), message);
}


void PollDnsWebserver()
{
  if (DnsServer) { DnsServer->processNextRequest(); }
  if (WebServer) { WebServer->handleClient(); }
}

void StartWebserver(IPAddress ipweb){

  if (!WebServer) {
    WebServer = new ESP8266WebServer(80);
    WebServer->on("/", HandleRoot);
    WebServer->on("/in", HandleInformation);
    WebServer->on("/cn", HandleConfiguration);
    WebServer->on("/up", HandleUpgradeFirmware);
    WebServer->on("/u1", HandleUpgradeFirmwareStart);  // OTA
    WebServer->on("/u2", HTTP_POST, HandleUploadDone, HandleUploadLoop);
    WebServer->on("/mq", HandleMqttConfiguration);
    WebServer->on("/sv", HandleSaveSettings);
    WebServer->on("/w0", HandleWifiConfiguration);
    WebServer->on("/cs", HandleConsole);
    WebServer->on("/ax", HandleAjaxConsoleRefresh);
    WebServer->onNotFound([](){
      WebServer->send(404, "text/plain", "FileNotFound");
    });

    WebServer->begin();
  }
	delay(5); // VB: exactly 5, no more or less, no yield()!
}

void StopWebserver()
{
  if (webserver_state) {
    WebServer->close();
    webserver_state = HTTP_OFF;
    AddLog_P(LOG_LEVEL_INFO, PSTR(D_LOG_HTTP D_WEBSERVER_STOPPED));
  }
}

void WifiManagerBegin()
{
  // setup AP
  if ((WL_CONNECTED == WiFi.status()) && (static_cast<uint32_t>(WiFi.localIP()) != 0)) {
    WiFi.mode(WIFI_AP_STA);
    AddLog_P(LOG_LEVEL_DEBUG, PSTR(D_LOG_WIFI D_WIFIMANAGER_SET_ACCESSPOINT_AND_STATION));
  } else {
    WiFi.mode(WIFI_AP);
    AddLog_P(LOG_LEVEL_DEBUG, PSTR(D_LOG_WIFI D_WIFIMANAGER_SET_ACCESSPOINT));
  }

  StopWebserver();

  DnsServer = new DNSServer();
  WiFi.softAP(my_hostname);
  delay(500); // Without delay I've seen the IP address blank
  /* Setup the DNS server redirecting all the domains to the apIP */
  DnsServer->setErrorReplyCode(DNSReplyCode::NoError);
  DnsServer->start(DNS_PORT, "*", WiFi.softAPIP());

  StartWebserver(WiFi.softAPIP());
}