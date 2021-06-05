export enum MavWinchStatusFlag {
	MAV_WINCH_STATUS_HEALTHY = 1, // Winch is healthy
	MAV_WINCH_STATUS_FULLY_RETRACTED = 2, // Winch thread is fully retracted
	MAV_WINCH_STATUS_MOVING = 4, // Winch motor is moving
	MAV_WINCH_STATUS_CLUTCH_ENGAGED = 8, // Winch clutch is engaged allowing motor to move freely
	MAV_WINCH_STATUS_FLAG_ENUM_END = 9, // 
}