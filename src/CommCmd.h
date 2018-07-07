enum {
	RESET_ESP_CMD				= 0x01,

	SET_NET_CMD 				= 0x10,
	SET_PASSPHRASE_CMD			= 0x11,
	SET_KEY_CMD					= 0x12,
	TEST_CMD					= 0x13,
	SET_IP_CONFIG_CMD			= 0x14,
	SET_DNS_CONFIG_CMD			= 0x15,

	GET_CONN_STATUS_CMD			= 0x20,
	GET_IPADDR_CMD				= 0x21,
	GET_MACADDR_CMD				= 0x22,
	GET_CURR_SSID_CMD			= 0x23,
	GET_CURR_BSSID_CMD			= 0x24,
	GET_CURR_RSSI_CMD			= 0x25,
	GET_CURR_ENCT_CMD			= 0x26,
	SCAN_NETWORKS				= 0x27,
	START_SERVER_TCP_CMD		= 0x28,
	GET_STATE_TCP_CMD			= 0x29,
	DATA_SENT_TCP_CMD			= 0x2A,
	AVAIL_DATA_TCP_CMD			= 0x2B,
	GET_DATA_TCP_CMD			= 0x2C,
	START_CLIENT_TCP_CMD		= 0x2D,
	STOP_CLIENT_TCP_CMD 		= 0x2E,
	GET_CLIENT_STATE_TCP_CMD	= 0x2F,
	DISCONNECT_CMD				= 0x30,
	GET_IDX_SSID_CMD			= 0x31,
	GET_IDX_RSSI_CMD			= 0x32,
	GET_IDX_ENCT_CMD			= 0x33,
	REQ_HOST_BY_NAME_CMD		= 0x34,
	GET_HOST_BY_NAME_CMD		= 0x35,
	START_SCAN_NETWORKS			= 0x36,
	GET_FW_VERSION_CMD			= 0x37,
	GET_TEST_CMD				= 0x38,
	SEND_DATA_UDP_CMD			= 0x39,
	GET_REMOTE_DATA_CMD 		= 0x3A,

	// All command with DATA_FLAG 0x40 send a 16bit Len

	SEND_DATA_TCP_CMD			= 0x44,
	GET_DATABUF_TCP_CMD			= 0x45,
	INSERT_DATABUF_CMD			= 0x46,
};