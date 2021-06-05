export enum MavCollisionSrc {
	MAV_COLLISION_SRC_ADSB = 0, // ID field references ADSB_VEHICLE packets
	MAV_COLLISION_SRC_MAVLINK_GPS_GLOBAL_INT = 1, // ID field references MAVLink SRC ID
	MAV_COLLISION_SRC_ENUM_END = 2, // 
}