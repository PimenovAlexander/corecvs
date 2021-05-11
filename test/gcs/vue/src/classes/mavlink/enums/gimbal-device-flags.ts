export enum GimbalDeviceFlags {
	GIMBAL_DEVICE_FLAGS_RETRACT = 1, // Set to retracted safe position (no stabilization), takes presedence over all other flags.
	GIMBAL_DEVICE_FLAGS_NEUTRAL = 2, // Set to neutral position (horizontal, forward looking, with stabiliziation), takes presedence over all other flags except RETRACT.
	GIMBAL_DEVICE_FLAGS_ROLL_LOCK = 4, // Lock roll angle to absolute angle relative to horizon (not relative to drone). This is generally the default with a stabilizing gimbal.
	GIMBAL_DEVICE_FLAGS_PITCH_LOCK = 8, // Lock pitch angle to absolute angle relative to horizon (not relative to drone). This is generally the default.
	GIMBAL_DEVICE_FLAGS_YAW_LOCK = 16, // Lock yaw angle to absolute angle relative to North (not relative to drone). If this flag is set, the quaternion is in the Earth frame with the x-axis pointing North (yaw absolute). If this flag is not set, the quaternion frame is in the Earth frame rotated so that the x-axis is pointing forward (yaw relative to vehicle).
	GIMBAL_DEVICE_FLAGS_ENUM_END = 17, // 
}