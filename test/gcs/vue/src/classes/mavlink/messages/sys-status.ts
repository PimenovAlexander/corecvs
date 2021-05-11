import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavSysStatusSensor} from '../enums/mav-sys-status-sensor';
/*
The general system state. If the system is following the MAVLink standard, the system state is mainly defined by three orthogonal states/modes: The system mode, which is either LOCKED (motors shut down and locked), MANUAL (system under RC control), GUIDED (system with autonomous position control, position setpoint controlled manually) or AUTO (system guided by path/waypoint planner). The NAV_MODE defined the current flight state: LIFTOFF (often an open-loop maneuver), LANDING, WAYPOINTS or VECTOR. This represents the internal navigation state machine. The system status shows whether the system is currently active or not and if an emergency occurred. During the CRITICAL and EMERGENCY states the MAV is still considered to be active, but should start emergency procedures autonomously. After a failure occurred it should first move from active to critical to allow manual intervention and then move to emergency after a certain timeout.
*/
// onboard_control_sensors_present Bitmap showing which onboard controllers and sensors are present. Value of 0: not present. Value of 1: present. uint32_t
// onboard_control_sensors_enabled Bitmap showing which onboard controllers and sensors are enabled:  Value of 0: not enabled. Value of 1: enabled. uint32_t
// onboard_control_sensors_health Bitmap showing which onboard controllers and sensors have an error (or are operational). Value of 0: error. Value of 1: healthy. uint32_t
// load Maximum usage in percent of the mainloop time. Values: [0-1000] - should always be below 1000 uint16_t
// voltage_battery Battery voltage, UINT16_MAX: Voltage not sent by autopilot uint16_t
// current_battery Battery current, -1: Current not sent by autopilot int16_t
// battery_remaining Battery energy remaining, -1: Battery remaining energy not sent by autopilot int8_t
// drop_rate_comm Communication drop rate, (UART, I2C, SPI, CAN), dropped packets on all links (packets that were corrupted on reception on the MAV) uint16_t
// errors_comm Communication errors (UART, I2C, SPI, CAN), dropped packets on all links (packets that were corrupted on reception on the MAV) uint16_t
// errors_count1 Autopilot-specific errors uint16_t
// errors_count2 Autopilot-specific errors uint16_t
// errors_count3 Autopilot-specific errors uint16_t
// errors_count4 Autopilot-specific errors uint16_t
export class SysStatus extends MAVLinkMessage {
	public onboard_control_sensors_present!: MavSysStatusSensor;
	public onboard_control_sensors_enabled!: MavSysStatusSensor;
	public onboard_control_sensors_health!: MavSysStatusSensor;
	public load!: number;
	public voltage_battery!: number;
	public current_battery!: number;
	public battery_remaining!: number;
	public drop_rate_comm!: number;
	public errors_comm!: number;
	public errors_count1!: number;
	public errors_count2!: number;
	public errors_count3!: number;
	public errors_count4!: number;
	public _message_id: number = 1;
	public _message_name: string = 'SYS_STATUS';
	public _crc_extra: number = 124;
	public _message_fields: [string, string, boolean][] = [
		['onboard_control_sensors_present', 'uint32_t', false],
		['onboard_control_sensors_enabled', 'uint32_t', false],
		['onboard_control_sensors_health', 'uint32_t', false],
		['load', 'uint16_t', false],
		['voltage_battery', 'uint16_t', false],
		['current_battery', 'int16_t', false],
		['drop_rate_comm', 'uint16_t', false],
		['errors_comm', 'uint16_t', false],
		['errors_count1', 'uint16_t', false],
		['errors_count2', 'uint16_t', false],
		['errors_count3', 'uint16_t', false],
		['errors_count4', 'uint16_t', false],
		['battery_remaining', 'int8_t', false],
	];
}