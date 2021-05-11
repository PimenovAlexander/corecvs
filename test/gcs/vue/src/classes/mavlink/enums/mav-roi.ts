export enum MavRoi {
	MAV_ROI_NONE = 0, // No region of interest.
	MAV_ROI_WPNEXT = 1, // Point toward next waypoint, with optional pitch/roll/yaw offset.
	MAV_ROI_WPINDEX = 2, // Point toward given waypoint.
	MAV_ROI_LOCATION = 3, // Point toward fixed location.
	MAV_ROI_TARGET = 4, // Point toward of given id.
	MAV_ROI_ENUM_END = 5, // 
}