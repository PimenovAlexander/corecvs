export enum GimbalDeviceErrorFlags {
	GIMBAL_DEVICE_ERROR_FLAGS_AT_ROLL_LIMIT = 1, // Gimbal device is limited by hardware roll limit.
	GIMBAL_DEVICE_ERROR_FLAGS_AT_PITCH_LIMIT = 2, // Gimbal device is limited by hardware pitch limit.
	GIMBAL_DEVICE_ERROR_FLAGS_AT_YAW_LIMIT = 4, // Gimbal device is limited by hardware yaw limit.
	GIMBAL_DEVICE_ERROR_FLAGS_ENCODER_ERROR = 8, // There is an error with the gimbal encoders.
	GIMBAL_DEVICE_ERROR_FLAGS_POWER_ERROR = 16, // There is an error with the gimbal power source.
	GIMBAL_DEVICE_ERROR_FLAGS_MOTOR_ERROR = 32, // There is an error with the gimbal motor's.
	GIMBAL_DEVICE_ERROR_FLAGS_SOFTWARE_ERROR = 64, // There is an error with the gimbal's software.
	GIMBAL_DEVICE_ERROR_FLAGS_COMMS_ERROR = 128, // There is an error with the gimbal's communication.
	GIMBAL_DEVICE_ERROR_FLAGS_CALIBRATION_RUNNING = 256, // Gimbal is currently calibrating.
	GIMBAL_DEVICE_ERROR_FLAGS_ENUM_END = 257, // 
}