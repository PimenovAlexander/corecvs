export enum AisFlags {
	AIS_FLAGS_POSITION_ACCURACY = 1, // 1 = Position accuracy less than 10m, 0 = position accuracy greater than 10m.
	AIS_FLAGS_VALID_COG = 2, // 
	AIS_FLAGS_VALID_VELOCITY = 4, // 
	AIS_FLAGS_HIGH_VELOCITY = 8, // 1 = Velocity over 52.5765m/s (102.2 knots)
	AIS_FLAGS_VALID_TURN_RATE = 16, // 
	AIS_FLAGS_TURN_RATE_SIGN_ONLY = 32, // Only the sign of the returned turn rate value is valid, either greater than 5deg/30s or less than -5deg/30s
	AIS_FLAGS_VALID_DIMENSIONS = 64, // 
	AIS_FLAGS_LARGE_BOW_DIMENSION = 128, // Distance to bow is larger than 511m
	AIS_FLAGS_LARGE_STERN_DIMENSION = 256, // Distance to stern is larger than 511m
	AIS_FLAGS_LARGE_PORT_DIMENSION = 512, // Distance to port side is larger than 63m
	AIS_FLAGS_LARGE_STARBOARD_DIMENSION = 1024, // Distance to starboard side is larger than 63m
	AIS_FLAGS_VALID_CALLSIGN = 2048, // 
	AIS_FLAGS_VALID_NAME = 4096, // 
	AIS_FLAGS_ENUM_END = 4097, // 
}