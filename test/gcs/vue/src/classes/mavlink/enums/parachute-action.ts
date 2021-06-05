export enum ParachuteAction {
	PARACHUTE_DISABLE = 0, // Disable auto-release of parachute (i.e. release triggered by crash detectors).
	PARACHUTE_ENABLE = 1, // Enable auto-release of parachute.
	PARACHUTE_RELEASE = 2, // Release parachute and kill motors.
	PARACHUTE_ACTION_ENUM_END = 3, // 
}