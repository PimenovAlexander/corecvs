export enum HlFailureFlag {
	HL_FAILURE_FLAG_GPS = 1, // GPS failure.
	HL_FAILURE_FLAG_DIFFERENTIAL_PRESSURE = 2, // Differential pressure sensor failure.
	HL_FAILURE_FLAG_ABSOLUTE_PRESSURE = 4, // Absolute pressure sensor failure.
	HL_FAILURE_FLAG_3D_ACCEL = 8, // Accelerometer sensor failure.
	HL_FAILURE_FLAG_3D_GYRO = 16, // Gyroscope sensor failure.
	HL_FAILURE_FLAG_3D_MAG = 32, // Magnetometer sensor failure.
	HL_FAILURE_FLAG_TERRAIN = 64, // Terrain subsystem failure.
	HL_FAILURE_FLAG_BATTERY = 128, // Battery failure/critical low battery.
	HL_FAILURE_FLAG_RC_RECEIVER = 256, // RC receiver failure/no rc connection.
	HL_FAILURE_FLAG_OFFBOARD_LINK = 512, // Offboard link failure.
	HL_FAILURE_FLAG_ENGINE = 1024, // Engine failure.
	HL_FAILURE_FLAG_GEOFENCE = 2048, // Geofence violation.
	HL_FAILURE_FLAG_ESTIMATOR = 4096, // Estimator failure, for example measurement rejection or large variances.
	HL_FAILURE_FLAG_MISSION = 8192, // Mission failure.
	HL_FAILURE_FLAG_ENUM_END = 8193, // 
}