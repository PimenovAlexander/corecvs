export enum OrbitYawBehaviour {
	ORBIT_YAW_BEHAVIOUR_HOLD_FRONT_TO_CIRCLE_CENTER = 0, // Vehicle front points to the center (default).
	ORBIT_YAW_BEHAVIOUR_HOLD_INITIAL_HEADING = 1, // Vehicle front holds heading when message received.
	ORBIT_YAW_BEHAVIOUR_UNCONTROLLED = 2, // Yaw uncontrolled.
	ORBIT_YAW_BEHAVIOUR_HOLD_FRONT_TANGENT_TO_CIRCLE = 3, // Vehicle front follows flight path (tangential to circle).
	ORBIT_YAW_BEHAVIOUR_RC_CONTROLLED = 4, // Yaw controlled by RC input.
	ORBIT_YAW_BEHAVIOUR_ENUM_END = 5, // 
}