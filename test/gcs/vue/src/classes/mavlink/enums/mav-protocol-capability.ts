export enum MavProtocolCapability {
	MAV_PROTOCOL_CAPABILITY_MISSION_FLOAT = 1, // Autopilot supports MISSION float message type.
	MAV_PROTOCOL_CAPABILITY_PARAM_FLOAT = 2, // Autopilot supports the new param float message type.
	MAV_PROTOCOL_CAPABILITY_MISSION_INT = 4, // Autopilot supports MISSION_ITEM_INT scaled integer message type.
	MAV_PROTOCOL_CAPABILITY_COMMAND_INT = 8, // Autopilot supports COMMAND_INT scaled integer message type.
	MAV_PROTOCOL_CAPABILITY_PARAM_UNION = 16, // Autopilot supports the new param union message type.
	MAV_PROTOCOL_CAPABILITY_FTP = 32, // Autopilot supports the new FILE_TRANSFER_PROTOCOL message type.
	MAV_PROTOCOL_CAPABILITY_SET_ATTITUDE_TARGET = 64, // Autopilot supports commanding attitude offboard.
	MAV_PROTOCOL_CAPABILITY_SET_POSITION_TARGET_LOCAL_NED = 128, // Autopilot supports commanding position and velocity targets in local NED frame.
	MAV_PROTOCOL_CAPABILITY_SET_POSITION_TARGET_GLOBAL_INT = 256, // Autopilot supports commanding position and velocity targets in global scaled integers.
	MAV_PROTOCOL_CAPABILITY_TERRAIN = 512, // Autopilot supports terrain protocol / data handling.
	MAV_PROTOCOL_CAPABILITY_SET_ACTUATOR_TARGET = 1024, // Autopilot supports direct actuator control.
	MAV_PROTOCOL_CAPABILITY_FLIGHT_TERMINATION = 2048, // Autopilot supports the flight termination command.
	MAV_PROTOCOL_CAPABILITY_COMPASS_CALIBRATION = 4096, // Autopilot supports onboard compass calibration.
	MAV_PROTOCOL_CAPABILITY_MAVLINK2 = 8192, // Autopilot supports MAVLink version 2.
	MAV_PROTOCOL_CAPABILITY_MISSION_FENCE = 16384, // Autopilot supports mission fence protocol.
	MAV_PROTOCOL_CAPABILITY_MISSION_RALLY = 32768, // Autopilot supports mission rally point protocol.
	MAV_PROTOCOL_CAPABILITY_FLIGHT_INFORMATION = 65536, // Autopilot supports the flight information protocol.
	MAV_PROTOCOL_CAPABILITY_ENUM_END = 65537, // 
}