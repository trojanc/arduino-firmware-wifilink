
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


void AddLog(byte loglevel)
{
  char mxtime[10];  // "13:45:21 "

  snprintf_P(mxtime, sizeof(mxtime), PSTR("%02d" D_HOUR_MINUTE_SEPARATOR "%02d" D_MINUTE_SECOND_SEPARATOR "%02d "), RtcTime.hour, RtcTime.minute, RtcTime.second);

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