export enum MavMissionResult {
	MAV_MISSION_ACCEPTED = 0, // mission accepted OK
	MAV_MISSION_ERROR = 1, // Generic error / not accepting mission commands at all right now.
	MAV_MISSION_UNSUPPORTED_FRAME = 2, // Coordinate frame is not supported.
	MAV_MISSION_UNSUPPORTED = 3, // Command is not supported.
	MAV_MISSION_NO_SPACE = 4, // Mission items exceed storage space.
	MAV_MISSION_INVALID = 5, // One of the parameters has an invalid value.
	MAV_MISSION_INVALID_PARAM1 = 6, // param1 has an invalid value.
	MAV_MISSION_INVALID_PARAM2 = 7, // param2 has an invalid value.
	MAV_MISSION_INVALID_PARAM3 = 8, // param3 has an invalid value.
	MAV_MISSION_INVALID_PARAM4 = 9, // param4 has an invalid value.
	MAV_MISSION_INVALID_PARAM5_X = 10, // x / param5 has an invalid value.
	MAV_MISSION_INVALID_PARAM6_Y = 11, // y / param6 has an invalid value.
	MAV_MISSION_INVALID_PARAM7 = 12, // z / param7 has an invalid value.
	MAV_MISSION_INVALID_SEQUENCE = 13, // Mission item received out of sequence
	MAV_MISSION_DENIED = 14, // Not accepting any mission commands from this communication partner.
	MAV_MISSION_OPERATION_CANCELLED = 15, // Current mission operation cancelled (e.g. mission upload, mission download).
	MAV_MISSION_RESULT_ENUM_END = 16, // 
}