export enum CellularNetworkFailedReason {
	CELLULAR_NETWORK_FAILED_REASON_NONE = 0, // No error
	CELLULAR_NETWORK_FAILED_REASON_UNKNOWN = 1, // Error state is unknown
	CELLULAR_NETWORK_FAILED_REASON_SIM_MISSING = 2, // SIM is required for the modem but missing
	CELLULAR_NETWORK_FAILED_REASON_SIM_ERROR = 3, // SIM is available, but not usuable for connection
	CELLULAR_NETWORK_FAILED_REASON_ENUM_END = 4, // 
}