export enum WinchActions {
	WINCH_RELAXED = 0, // Relax winch.
	WINCH_RELATIVE_LENGTH_CONTROL = 1, // Wind or unwind specified length of cable, optionally using specified rate.
	WINCH_RATE_CONTROL = 2, // Wind or unwind cable at specified rate.
	WINCH_ACTIONS_ENUM_END = 3, // 
}