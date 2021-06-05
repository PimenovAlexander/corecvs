export enum MavMode {
	MAV_MODE_PREFLIGHT = 0, // System is not ready to fly, booting, calibrating, etc. No flag is set.
	MAV_MODE_MANUAL_DISARMED = 64, // System is allowed to be active, under manual (RC) control, no stabilization
	MAV_MODE_TEST_DISARMED = 66, // UNDEFINED mode. This solely depends on the autopilot - use with caution, intended for developers only.
	MAV_MODE_STABILIZE_DISARMED = 80, // System is allowed to be active, under assisted RC control.
	MAV_MODE_GUIDED_DISARMED = 88, // System is allowed to be active, under autonomous control, manual setpoint
	MAV_MODE_AUTO_DISARMED = 92, // System is allowed to be active, under autonomous control and navigation (the trajectory is decided onboard and not pre-programmed by waypoints)
	MAV_MODE_MANUAL_ARMED = 192, // System is allowed to be active, under manual (RC) control, no stabilization
	MAV_MODE_TEST_ARMED = 194, // UNDEFINED mode. This solely depends on the autopilot - use with caution, intended for developers only.
	MAV_MODE_STABILIZE_ARMED = 208, // System is allowed to be active, under assisted RC control.
	MAV_MODE_GUIDED_ARMED = 216, // System is allowed to be active, under autonomous control, manual setpoint
	MAV_MODE_AUTO_ARMED = 220, // System is allowed to be active, under autonomous control and navigation (the trajectory is decided onboard and not pre-programmed by waypoints)
	MAV_MODE_ENUM_END = 221, // 
}