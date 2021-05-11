import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavFrame} from '../enums/mav-frame';
import {PositionTargetTypemask} from '../enums/position-target-typemask';
/*
Sets a desired vehicle position in a local north-east-down coordinate frame. Used by an external controller to command the vehicle (manual controller or other system).
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// target_system System ID uint8_t
// target_component Component ID uint8_t
// coordinate_frame Valid options are: MAV_FRAME_LOCAL_NED = 1, MAV_FRAME_LOCAL_OFFSET_NED = 7, MAV_FRAME_BODY_NED = 8, MAV_FRAME_BODY_OFFSET_NED = 9 uint8_t
// type_mask Bitmap to indicate which dimensions should be ignored by the vehicle. uint16_t
// x X Position in NED frame float
// y Y Position in NED frame float
// z Z Position in NED frame (note, altitude is negative in NED) float
// vx X velocity in NED frame float
// vy Y velocity in NED frame float
// vz Z velocity in NED frame float
// afx X acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N float
// afy Y acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N float
// afz Z acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N float
// yaw yaw setpoint float
// yaw_rate yaw rate setpoint float
export class SetPositionTargetLocalNed extends MAVLinkMessage {
	public time_boot_ms!: number;
	public target_system!: number;
	public target_component!: number;
	public coordinate_frame!: MavFrame;
	public type_mask!: PositionTargetTypemask;
	public x!: number;
	public y!: number;
	public z!: number;
	public vx!: number;
	public vy!: number;
	public vz!: number;
	public afx!: number;
	public afy!: number;
	public afz!: number;
	public yaw!: number;
	public yaw_rate!: number;
	public _message_id: number = 84;
	public _message_name: string = 'SET_POSITION_TARGET_LOCAL_NED';
	public _crc_extra: number = 143;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['x', 'float', false],
		['y', 'float', false],
		['z', 'float', false],
		['vx', 'float', false],
		['vy', 'float', false],
		['vz', 'float', false],
		['afx', 'float', false],
		['afy', 'float', false],
		['afz', 'float', false],
		['yaw', 'float', false],
		['yaw_rate', 'float', false],
		['type_mask', 'uint16_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['coordinate_frame', 'uint8_t', false],
	];
}