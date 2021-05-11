export enum UtmFlightState {
	UTM_FLIGHT_STATE_UNKNOWN = 1, // The flight state can't be determined.
	UTM_FLIGHT_STATE_GROUND = 2, // UAS on ground.
	UTM_FLIGHT_STATE_AIRBORNE = 3, // UAS airborne.
	UTM_FLIGHT_STATE_EMERGENCY = 16, // UAS is in an emergency flight state.
	UTM_FLIGHT_STATE_NOCTRL = 32, // UAS has no active controls.
	UTM_FLIGHT_STATE_ENUM_END = 33, // 
}