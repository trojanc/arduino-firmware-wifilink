/*
  user_config_override.h - user configuration overrides user_config.h for Sonoff-Tasmota

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

#ifndef _USER_CONFIG_OVERRIDE_H_
#define _USER_CONFIG_OVERRIDE_H_

// force the compiler to show a warning to confirm that this file is inlcuded
#warning **** user_config_override.h: Using Settings from this File ****
// -- Master parameter control --------------------
#undef  CFG_HOLDER
#define CFG_HOLDER        0x20161220             // [Reset 1] Change this value to load SECTION1 configuration parameters to flash

// -- Setup your own Wifi settings  ---------------
#undef STA_SSID1
#define STA_SSID1              "TrojanCWIFI"

#undef STA_PASS1
#define STA_PASS1              "TrojanC2102"

#undef STA_SSID2
#define STA_SSID2              ""

#undef STA_PASS2
#define STA_PASS2              ""

#undef TELE_PERIOD
#define TELE_PERIOD 10

#undef WEB_LOG_LEVEL
#define WEB_LOG_LEVEL          LOG_LEVEL_DEBUG_MORE    // [WebLog] (LOG_LEVEL_NONE, LOG_LEVEL_ERROR, LOG_LEVEL_INFO, LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG_MORE)

#undef SERIAL_LOG_LEVEL
#define SERIAL_LOG_LEVEL       LOG_LEVEL_DEBUG_MORE
#endif //_USER_CONFIG_OVERRIDE_H_