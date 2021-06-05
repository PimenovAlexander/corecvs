export enum MavBatteryMode {
	MAV_BATTERY_MODE_UNKNOWN = 0, // Battery mode not supported/unknown battery mode/normal operation.
	MAV_BATTERY_MODE_AUTO_DISCHARGING = 1, // Battery is auto discharging (towards storage level).
	MAV_BATTERY_MODE_HOT_SWAP = 2, // Battery in hot-swap mode (current limited to prevent spikes that might damage sensitive electrical circuits).
	MAV_BATTERY_MODE_ENUM_END = 3, // 
}