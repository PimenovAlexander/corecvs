export enum MavOdidOperatorLocationType {
	MAV_ODID_OPERATOR_LOCATION_TYPE_TAKEOFF = 0, // The location of the operator is the same as the take-off location.
	MAV_ODID_OPERATOR_LOCATION_TYPE_LIVE_GNSS = 1, // The location of the operator is based on live GNSS data.
	MAV_ODID_OPERATOR_LOCATION_TYPE_FIXED = 2, // The location of the operator is a fixed location.
	MAV_ODID_OPERATOR_LOCATION_TYPE_ENUM_END = 3, // 
}