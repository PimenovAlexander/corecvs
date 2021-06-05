export enum MavBatteryFunction {
	MAV_BATTERY_FUNCTION_UNKNOWN = 0, // Battery function is unknown
	MAV_BATTERY_FUNCTION_ALL = 1, // Battery supports all flight systems
	MAV_BATTERY_FUNCTION_PROPULSION = 2, // Battery for the propulsion system
	MAV_BATTERY_FUNCTION_AVIONICS = 3, // Avionics battery
	MAV_BATTERY_TYPE_PAYLOAD = 4, // Payload battery
	MAV_BATTERY_FUNCTION_ENUM_END = 5, // 
}