#include "WebServer.h"

/*********************************************************************************************\
 * Web server and WiFi Manager
 *
 * Enables configuration and reconfiguration of WiFi credentials using a Captive Portal
 * Based on source by AlexT (https://github.com/tzapu)
\*********************************************************************************************/

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

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
const char HTTP_BTN_MAIN[] PROGMEM =
  "<br/><br/><form action='.' method='get'><button>Main Menu</button></form>";

const char HDR_CTYPE_HTML[] PROGMEM = "text/html";
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
  func += F("}1Free Memory}2"); func += String(freeMem / 1024); func += F("kB");
  func += F("</td></tr></table>");
  func += FPSTR(HTTP_SCRIPT_INFO_END);
  page.replace(F("</script>"), func);
  page.replace(F("<body>"), F("<body onload='i()'>"));

  //  page += F("</fieldset>");
  page += FPSTR(HTTP_BTN_MAIN);
  ShowPage(page);
}

// uint8_t MAC_array[6];	 //mac_address
// char macAddress[12];		//mac_address
String newSSID_param;
String newPASSWORD_param;
const char* newSSID_par = "";
const char* newPASSWORD_par = "";
// IPAddress default_IP(192,168,240,1);	//defaul IP Address
bool SERVER_STOP = false;			 //check stop server

// extern "C" void system_set_os_print(uint8 onoff);		//TODO to test without
// extern "C" void ets_install_putc1(void* routine);
int tot = 0;
bool connect_wifi = false;
//String HOSTNAME = "arduino";


String getContentType(String filename){
	if(server.hasArg("download")) return "application/octet-stream";
	else if(filename.endsWith(".htm")) return "text/html";
	else if(filename.endsWith(".html")) return "text/html";
	else if(filename.endsWith(".css")) return "text/css";
	else if(filename.endsWith(".js")) return "application/javascript";
	else if(filename.endsWith(".png")) return "image/png";
	else if(filename.endsWith(".gif")) return "image/gif";
	else if(filename.endsWith(".jpg")) return "image/jpeg";
	else if(filename.endsWith(".ico")) return "image/x-icon";
	else if(filename.endsWith(".xml")) return "text/xml";
	else if(filename.endsWith(".pdf")) return "application/x-pdf";
	else if(filename.endsWith(".zip")) return "application/x-zip";
	else if(filename.endsWith(".gz")) return "application/x-gzip";
	return "text/plain";
}

bool handleFileRead(String path){
	if(path.endsWith("/")) path += "index.html";
	String contentType = getContentType(path);
	File file = SPIFFS.open(path, "r");
	if (file == NULL)
		return false;
	size_t sent = server.streamFile(file, contentType);
	file.close();
	return true;
}

String toStringIp(IPAddress ip) {
	String res = "";
	for (int i = 0; i < 3; i++) {
		res += String((ip >> (8 * i)) & 0xFF) + ".";
	}
	res += String(((ip >> 8 * 3)) & 0xFF);
	return res;
}

String toStringWifiMode(int mod) {
	String mode;
	switch (mod) {
		case 0:
			mode = "OFF";
			break;
		case 1:
			mode = "STA";
			break;
		case 2:
			mode = "AP";
			break;
		case 3:
			mode = "AP+STA";
			break;
		case 4:
			mode = "----";
			break;
		default:
			break;
	}
	return mode;
}

WiFiMode intToWifiMode(int mod) {
	WiFiMode mode;
	switch (mod) {
		case 0:
			mode = WIFI_OFF;
			break;
		case 1:
			mode = WIFI_STA;
			break;
		case 2:
			mode = WIFI_AP;
			break;
		case 3:
			mode = WIFI_AP_STA;
			break;
		case 4:
			break;
		default:
			break;
	}
	return mode;
}

String toStringWifiStatus(int state) {
	String status;
	switch (state) {
		case 0:
			status = "connecting";
			break;
		case 1:
			status = "unknown status";
			break;
		case 2:
			status = "wifi scan completed";
			break;
		case 3:
			status = "got IP address";
			// statements
			break;
		case 4:
			status = "connection failed";
			break;
		default:
			break;
	}
	return status;
}

String toStringEncryptionType(int thisType) {
	String eType;
	switch (thisType) {
		case ENC_TYPE_WEP:
			eType = "WEP";
			break;
		case ENC_TYPE_TKIP:
			eType = "WPA";
			break;
		case ENC_TYPE_CCMP:
			eType = "WPA2";
			break;
		case ENC_TYPE_NONE:
			eType = "None";
			break;
		case ENC_TYPE_AUTO:
			eType = "Auto";
			break;
	}
	return eType;
}

IPAddress stringToIP(String address) {
	int p1 = address.indexOf('.'), p2 = address.indexOf('.', p1 + 1), p3 = address.indexOf('.', p2 + 1); //, 4p = address.indexOf(3p+1);
	String ip1 = address.substring(0, p1), ip2 = address.substring(p1 + 1, p2), ip3 = address.substring(p2 + 1, p3), ip4 = address.substring(p3 + 1);

	return IPAddress(ip1.toInt(), ip2.toInt(), ip3.toInt(), ip4.toInt());
}

void handleWebServer(){
	if(connect_wifi){
		ETS_SPI_INTR_DISABLE();
		WiFi.begin(newSSID_param.c_str(),newPASSWORD_param.c_str());
		delay(500);	// VB: exactly 500, and here!
		connect_wifi = false;
		ETS_SPI_INTR_ENABLE();
	}
	if(CommunicationLogic.UI_alert){			//stop UI SERVER
		if(!SERVER_STOP){
			server.stop();
			SERVER_STOP = true;
		}
	}
	else{
		server.handleClient();
	}
}

void clearStaticIP() {
	dhcp = "on";
	Config.setParam("staticIP", "0.0.0.0");
	Config.setParam("netMask", "255.255.255.0");
	Config.setParam("gatewayIP", "192.168.1.1");
}

void initWebServer(){

//	tot = WiFi.scanNetworks();

	server.on("/", HandleRoot);
	server.on("/in", HandleInformation);

	//"wifi/info" information
	server.on("/wifi/info", []() {
		String ipadd = (WiFi.getMode() == 1 || WiFi.getMode() == 3) ? toStringIp(WiFi.localIP()) : toStringIp(WiFi.softAPIP());
		String staticadd = dhcp.equals("on") ? "0.0.0.0" : staticIP_param;
		int change = WiFi.getMode() == 1 ? 3 : 1;
		String cur_ssid = (WiFi.getMode() == 2 )? "none" : WiFi.SSID();

		server.send(200,
				"text/plain",
				String("{\"ssid\":\"" + cur_ssid +
				"\",\"hostname\":\"" + WiFi.hostname() +
				"\",\"ip\":\"" + ipadd +
				"\",\"mode\":\"" + toStringWifiMode(WiFi.getMode()) +
				"\",\"chan\":\""+ WiFi.channel() +
				"\",\"status\":\"" + toStringWifiStatus(WiFi.status()) +
				"\",\"gateway\":\"" + toStringIp(WiFi.gatewayIP()) +
				"\",\"netmask\":\"" + toStringIp(WiFi.subnetMask()) +
				"\",\"rssi\":\""+ WiFi.RSSI() +
				"\",\"mac\":\"" + WiFi.macAddress() +
				"\",\"phy\":\"" + WiFi.getPhyMode() +
				"\",\"dhcp\": \"" + dhcp +
				"\",\"staticip\":\"" + staticadd +
				"\",\"warn\": \"" + "<a href='#' class='pure-button button-primary button-larger-margin' onclick='changeWifiMode(" + change + ")'>Switch to " + toStringWifiMode(change) + " mode</a>\""+
				"}" ));
		});

		//"system/info" information
		server.on("/system/info", []() {
			server.send(200,
					"text/plain",
					String("{\"heap\":\""+ String(ESP.getFreeHeap()/1024)+" KB\",\"id\":\"" + String(ESP.getFlashChipId()) + "\",\"size\":\"" + (ESP.getFlashChipSize() / 1024 / 1024) + " MB\",\"baud\":\"9600\"}"));
		});

		server.on("/heap", []() {
			server.send(200,
					"text/plain",
					String(ESP.getFreeHeap()));
		});

		server.on("/system/update", []() {
			String newhostname = server.arg("name");
			WiFi.hostname(newhostname);
			MDNS.begin(newhostname.c_str());
			MDNS.setInstanceName(newhostname);
			server.send(200, "text/plain", newhostname);
			Config.setParam("hostname", newhostname);
		});

//		server.on("/wifi/netNumber", []() {
//			tot = WiFi.scanNetworks();
//			server.send(200, "text/plain", String(tot));
//		});

//		server.on("/wifi/scan", []() {
//			String scanResp = "";
//			if (tot == 0) {
//				server.send(200, "text/plain", "No networks found");
//			}
//			if (tot == -1 ) {
//				server.send(500, "text/plain", "Error during scanning");
//			}
//
//			scanResp += "{\"result\": { \"APs\" : [ ";
//			//ETS_SPI_INTR_DISABLE();
//			for (int netIndex = 0; netIndex < tot; netIndex++) {
//				scanResp += "{\"enc\" : \"";
//				scanResp += toStringEncryptionType (WiFi.encryptionType(netIndex));
//				scanResp += "\",";
//				scanResp += "\"essid\":\"";
//				scanResp += WiFi.SSID(netIndex);
//				scanResp += "\",";
//				scanResp += "\"rssi\" :\"";
//				scanResp += WiFi.RSSI(netIndex);
//				scanResp += "\"}";
//				if (netIndex != tot - 1)
//					scanResp += ",";
//			}
//			scanResp += "]}}";
//			//ETS_SPI_INTR_ENABLE();
//			server.send(200, "text/plain", scanResp);
//		});

		server.on("/connect", []() {
			newSSID_param = server.arg("essid");
			newPASSWORD_param = server.arg("passwd");
			server.send(200, "text/plain", "1");
			clearStaticIP();
			connect_wifi = true;
		});

		server.on("/connstatus", []() {
			String ipadd = (WiFi.getMode() == 1 || WiFi.getMode() == 3) ? toStringIp(WiFi.localIP()) : toStringIp(WiFi.softAPIP());
			server.send(200,
					"text/plain",
					String("{\"url\":\"got IP address\", \"ip\":\""+ipadd+"\", \"modechange\":\"no\", \"ssid\":\""+WiFi.SSID()+"\", \"reason\":\"-\", \"status\":\""+ toStringWifiStatus(WiFi.status()) +"\"}"));
		});


		server.on("/setmode", []() {
			int newMode = server.arg("mode").toInt();
			switch (newMode){
				case 1 :
				case 3 :
					server.send(200, "text/plain", String("Mode changed " + toStringWifiMode(WiFi.getMode())));
					WiFi.mode(intToWifiMode(newMode));
					break;
				case 2 :
					server.send(200, "text/plain", String("Mode changed " + toStringWifiMode(WiFi.getMode())));
					WiFi.mode(WIFI_AP);
					break;
			}

		});

		server.on("/special", []() {
			dhcp = server.arg("dhcp");
			staticIP_param = server.arg("staticip");
			netmask_param = server.arg("netmask");
			gateway_param = server.arg("gateway");

			if (dhcp == "off") {
				server.send(200, "text/plain", String("{\"url\":\"" + staticIP_param + "\"}"));
				delay(500); // to let the response finish
				WiFi.config(stringToIP(staticIP_param), stringToIP(gateway_param), stringToIP(netmask_param));
				Config.setParam("staticIP", staticIP_param);
				Config.setParam("netMask", netmask_param);
				Config.setParam("gatewayIP", gateway_param);
			 }
			 else {
				server.send(200, "text/plain",	"1");
				clearStaticIP();
				ESP.restart();
			 }
		 });

		 server.on("/boardInfo", []() {
				StaticJsonBuffer<200> jsonBuffer;
				JsonObject& boardInfo = jsonBuffer.createObject();
				String output = "";
				boardInfo["name"] = "Uno WiFi";
				boardInfo["icon"] = "/img/logoUnoWiFi.ico";
				boardInfo["logo"] = "/img/logoUnoWiFi.png";
				boardInfo["link"] = "https://github.com/jandrassy/arduino-library-wifilink#wifi-link-library";

				boardInfo["fw_name"] = FW_NAME;
				boardInfo["fw_version"] = FW_VERSION;
				boardInfo["build_date"] = BUILD_DATE;

				boardInfo.printTo(output);
				server.send(200, "text/json", output);
			});

		server.onNotFound([](){
			server.send(404, "text/plain", "FileNotFound");
		});

		server.begin();
		delay(5); // VB: exactly 5, no more or less, no yield()!
}
