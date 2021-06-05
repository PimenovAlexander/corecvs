export enum MavCollisionThreatLevel {
	MAV_COLLISION_THREAT_LEVEL_NONE = 0, // Not a threat
	MAV_COLLISION_THREAT_LEVEL_LOW = 1, // Craft is mildly concerned about this threat
	MAV_COLLISION_THREAT_LEVEL_HIGH = 2, // Craft is panicking, and may take actions to avoid threat
	MAV_COLLISION_THREAT_LEVEL_ENUM_END = 3, // 
}