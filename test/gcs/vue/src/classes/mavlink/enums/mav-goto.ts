export enum MavGoto {
	MAV_GOTO_DO_HOLD = 0, // Hold at the current position.
	MAV_GOTO_DO_CONTINUE = 1, // Continue with the next item in mission execution.
	MAV_GOTO_HOLD_AT_CURRENT_POSITION = 2, // Hold at the current position of the system
	MAV_GOTO_HOLD_AT_SPECIFIED_POSITION = 3, // Hold at the position specified in the parameters of the DO_HOLD action
	MAV_GOTO_ENUM_END = 4, // 
}