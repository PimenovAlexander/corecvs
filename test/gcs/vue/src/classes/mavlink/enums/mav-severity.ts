export enum MavSeverity {
	MAV_SEVERITY_EMERGENCY = 0, // System is unusable. This is a "panic" condition.
	MAV_SEVERITY_ALERT = 1, // Action should be taken immediately. Indicates error in non-critical systems.
	MAV_SEVERITY_CRITICAL = 2, // Action must be taken immediately. Indicates failure in a primary system.
	MAV_SEVERITY_ERROR = 3, // Indicates an error in secondary/redundant systems.
	MAV_SEVERITY_WARNING = 4, // Indicates about a possible future error if this is not resolved within a given timeframe. Example would be a low battery warning.
	MAV_SEVERITY_NOTICE = 5, // An unusual event has occurred, though not an error condition. This should be investigated for the root cause.
	MAV_SEVERITY_INFO = 6, // Normal operational messages. Useful for logging. No action is required for these messages.
	MAV_SEVERITY_DEBUG = 7, // Useful non-operational messages that can assist in debugging. These should not occur during normal operation.
	MAV_SEVERITY_ENUM_END = 8, // 
}