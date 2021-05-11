import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavFrame} from '../enums/mav-frame';
import {PositionTargetTypemask} from '../enums/position-target-typemask';
/*
Reports the current commanded vehicle position, velocity, and acceleration as specified by the autopilot. This should match the commands sent in SET_POSITION_TARGET_GLOBAL_INT if the vehicle is being controlled this way.
*/
// time_boot_ms Timestamp (time since system boot). The rationale for the timestamp in the setpoint is to allow the system to compensate for the transport delay of the setpoint. This allows the system to compensate processing latency. uint32_t
// coordinate_frame Valid options are: MAV_FRAME_GLOBAL_INT = 5, MAV_FRAME_GLOBAL_RELATIVE_ALT_INT = 6, MAV_FRAME_GLOBAL_TERRAIN_ALT_INT = 11 uint8_t
// type_mask Bitmap to indicate which dimensions should be ignored by the vehicle. uint16_t
// lat_int X Position in WGS84 frame int32_t
// lon_int Y Position in WGS84 frame int32_t
// alt Altitude (MSL, AGL or relative to home altitude, depending on frame) float
// vx X velocity in NED frame float
// vy Y velocity in NED frame float
// vz Z velocity in NED frame float
// afx X acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N float
// afy Y acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N float
// afz Z acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N float
// yaw yaw setpoint float
// yaw_rate yaw rate setpoint float
export class PositionTargetGlobalInt extends MAVLinkMessage {
	public time_boot_ms!: number;
	public coordinate_frame!: MavFrame;
	public type_mask!: PositionTargetTypemask;
	public lat_int!: number;
	public lon_int!: number;
	public alt!: number;
	public vx!: number;
	public vy!: number;
	public vz!: number;
	public afx!: number;
	public afy!: number;
	public afz!: number;
	public yaw!: number;
	public yaw_rate!: number;
	public _message_id: number = 87;
	public _message_name: string = 'POSITION_TARGET_GLOBAL_INT';
	public _crc_extra: number = 150;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['lat_int', 'int32_t', false],
		['lon_int', 'int32_t', false],
		['alt', 'float', false],
		['vx', 'float', false],
		['vy', 'float', false],
		['vz', 'float', false],
		['afx', 'float', false],
		['afy', 'float', false],
		['afz', 'float', false],
		['yaw', 'float', false],
		['yaw_rate', 'float', false],
		['type_mask', 'uint16_t', false],
		['coordinate_frame', 'uint8_t', false],
	];
}