export enum MavBatteryFault {
	MAV_BATTERY_FAULT_DEEP_DISCHARGE = 1, // Battery has deep discharged.
	MAV_BATTERY_FAULT_SPIKES = 2, // Voltage spikes.
	MAV_BATTERY_FAULT_CELL_FAIL = 4, // One or more cells have failed. Battery should also report MAV_BATTERY_CHARGE_STATE_FAILE (and should not be used).
	MAV_BATTERY_FAULT_OVER_CURRENT = 8, // Over-current fault.
	MAV_BATTERY_FAULT_OVER_TEMPERATURE = 16, // Over-temperature fault.
	MAV_BATTERY_FAULT_UNDER_TEMPERATURE = 32, // Under-temperature fault.
	MAV_BATTERY_FAULT_INCOMPATIBLE_VOLTAGE = 64, // Vehicle voltage is not compatible with this battery (batteries on same power rail should have similar voltage).
	MAV_BATTERY_FAULT_ENUM_END = 65, // 
}