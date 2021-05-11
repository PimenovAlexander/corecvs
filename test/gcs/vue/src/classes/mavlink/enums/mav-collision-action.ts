export enum MavCollisionAction {
	MAV_COLLISION_ACTION_NONE = 0, // Ignore any potential collisions
	MAV_COLLISION_ACTION_REPORT = 1, // Report potential collision
	MAV_COLLISION_ACTION_ASCEND_OR_DESCEND = 2, // Ascend or Descend to avoid threat
	MAV_COLLISION_ACTION_MOVE_HORIZONTALLY = 3, // Move horizontally to avoid threat
	MAV_COLLISION_ACTION_MOVE_PERPENDICULAR = 4, // Aircraft to move perpendicular to the collision's velocity vector
	MAV_COLLISION_ACTION_RTL = 5, // Aircraft to fly directly back to its launch point
	MAV_COLLISION_ACTION_HOVER = 6, // Aircraft to stop in place
	MAV_COLLISION_ACTION_ENUM_END = 7, // 
}