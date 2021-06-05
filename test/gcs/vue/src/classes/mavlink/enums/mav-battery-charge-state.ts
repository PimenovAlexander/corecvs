export enum MavBatteryChargeState {
	MAV_BATTERY_CHARGE_STATE_UNDEFINED = 0, // Low battery state is not provided
	MAV_BATTERY_CHARGE_STATE_OK = 1, // Battery is not in low state. Normal operation.
	MAV_BATTERY_CHARGE_STATE_LOW = 2, // Battery state is low, warn and monitor close.
	MAV_BATTERY_CHARGE_STATE_CRITICAL = 3, // Battery state is critical, return or abort immediately.
	MAV_BATTERY_CHARGE_STATE_EMERGENCY = 4, // Battery state is too low for ordinary abort sequence. Perform fastest possible emergency stop to prevent damage.
	MAV_BATTERY_CHARGE_STATE_FAILED = 5, // Battery failed, damage unavoidable. Possible causes (faults) are listed in MAV_BATTERY_FAULT.
	MAV_BATTERY_CHARGE_STATE_UNHEALTHY = 6, // Battery is diagnosed to be defective or an error occurred, usage is discouraged / prohibited. Possible causes (faults) are listed in MAV_BATTERY_FAULT.
	MAV_BATTERY_CHARGE_STATE_CHARGING = 7, // Battery is charging.
	MAV_BATTERY_CHARGE_STATE_ENUM_END = 8, // 
}