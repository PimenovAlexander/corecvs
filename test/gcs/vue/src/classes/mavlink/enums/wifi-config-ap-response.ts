export enum WifiConfigApResponse {
	WIFI_CONFIG_AP_RESPONSE_UNDEFINED = 0, // Undefined response. Likely an indicative of a system that doesn't support this request.
	WIFI_CONFIG_AP_RESPONSE_ACCEPTED = 1, // Changes accepted.
	WIFI_CONFIG_AP_RESPONSE_REJECTED = 2, // Changes rejected.
	WIFI_CONFIG_AP_RESPONSE_MODE_ERROR = 3, // Invalid Mode.
	WIFI_CONFIG_AP_RESPONSE_SSID_ERROR = 4, // Invalid SSID.
	WIFI_CONFIG_AP_RESPONSE_PASSWORD_ERROR = 5, // Invalid Password.
	WIFI_CONFIG_AP_RESPONSE_ENUM_END = 6, // 
}