export enum CellularStatusFlag {
	CELLULAR_STATUS_FLAG_UNKNOWN = 0, // State unknown or not reportable.
	CELLULAR_STATUS_FLAG_FAILED = 1, // Modem is unusable
	CELLULAR_STATUS_FLAG_INITIALIZING = 2, // Modem is being initialized
	CELLULAR_STATUS_FLAG_LOCKED = 3, // Modem is locked
	CELLULAR_STATUS_FLAG_DISABLED = 4, // Modem is not enabled and is powered down
	CELLULAR_STATUS_FLAG_DISABLING = 5, // Modem is currently transitioning to the CELLULAR_STATUS_FLAG_DISABLED state
	CELLULAR_STATUS_FLAG_ENABLING = 6, // Modem is currently transitioning to the CELLULAR_STATUS_FLAG_ENABLED state
	CELLULAR_STATUS_FLAG_ENABLED = 7, // Modem is enabled and powered on but not registered with a network provider and not available for data connections
	CELLULAR_STATUS_FLAG_SEARCHING = 8, // Modem is searching for a network provider to register
	CELLULAR_STATUS_FLAG_REGISTERED = 9, // Modem is registered with a network provider, and data connections and messaging may be available for use
	CELLULAR_STATUS_FLAG_DISCONNECTING = 10, // Modem is disconnecting and deactivating the last active packet data bearer. This state will not be entered if more than one packet data bearer is active and one of the active bearers is deactivated
	CELLULAR_STATUS_FLAG_CONNECTING = 11, // Modem is activating and connecting the first packet data bearer. Subsequent bearer activations when another bearer is already active do not cause this state to be entered
	CELLULAR_STATUS_FLAG_CONNECTED = 12, // One or more packet data bearers is active and connected
	CELLULAR_STATUS_FLAG_ENUM_END = 13, // 
}