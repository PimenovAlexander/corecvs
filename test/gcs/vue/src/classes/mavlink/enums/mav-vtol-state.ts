export enum MavVtolState {
	MAV_VTOL_STATE_UNDEFINED = 0, // MAV is not configured as VTOL
	MAV_VTOL_STATE_TRANSITION_TO_FW = 1, // VTOL is in transition from multicopter to fixed-wing
	MAV_VTOL_STATE_TRANSITION_TO_MC = 2, // VTOL is in transition from fixed-wing to multicopter
	MAV_VTOL_STATE_MC = 3, // VTOL is in multicopter state
	MAV_VTOL_STATE_FW = 4, // VTOL is in fixed-wing state
	MAV_VTOL_STATE_ENUM_END = 5, // 
}