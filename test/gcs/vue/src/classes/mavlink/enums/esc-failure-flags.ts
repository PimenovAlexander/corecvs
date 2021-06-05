export enum EscFailureFlags {
	ESC_FAILURE_NONE = 0, // No ESC failure.
	ESC_FAILURE_OVER_CURRENT = 1, // Over current failure.
	ESC_FAILURE_OVER_VOLTAGE = 2, // Over voltage failure.
	ESC_FAILURE_OVER_TEMPERATURE = 4, // Over temperature failure.
	ESC_FAILURE_OVER_RPM = 8, // Over RPM failure.
	ESC_FAILURE_INCONSISTENT_CMD = 16, // Inconsistent command failure i.e. out of bounds.
	ESC_FAILURE_MOTOR_STUCK = 32, // Motor stuck failure.
	ESC_FAILURE_GENERIC = 64, // Generic ESC failure.
	ESC_FAILURE_FLAGS_ENUM_END = 65, // 
}