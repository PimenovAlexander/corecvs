export enum MavLandedState {
	MAV_LANDED_STATE_UNDEFINED = 0, // MAV landed state is unknown
	MAV_LANDED_STATE_ON_GROUND = 1, // MAV is landed (on ground)
	MAV_LANDED_STATE_IN_AIR = 2, // MAV is in air
	MAV_LANDED_STATE_TAKEOFF = 3, // MAV currently taking off
	MAV_LANDED_STATE_LANDING = 4, // MAV currently landing
	MAV_LANDED_STATE_ENUM_END = 5, // 
}