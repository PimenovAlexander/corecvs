import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {AttitudeTargetTypemask} from '../enums/attitude-target-typemask';
/*
Reports the current commanded attitude of the vehicle as specified by the autopilot. This should match the commands sent in a SET_ATTITUDE_TARGET message if the vehicle is being controlled this way.
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// type_mask Bitmap to indicate which dimensions should be ignored by the vehicle. uint8_t
// q Attitude quaternion (w, x, y, z order, zero-rotation is 1, 0, 0, 0) float
// body_roll_rate Body roll rate float
// body_pitch_rate Body pitch rate float
// body_yaw_rate Body yaw rate float
// thrust Collective thrust, normalized to 0 .. 1 (-1 .. 1 for vehicles capable of reverse trust) float
export class AttitudeTarget extends MAVLinkMessage {
	public time_boot_ms!: number;
	public type_mask!: AttitudeTargetTypemask;
	public q!: number;
	public body_roll_rate!: number;
	public body_pitch_rate!: number;
	public body_yaw_rate!: number;
	public thrust!: number;
	public _message_id: number = 83;
	public _message_name: string = 'ATTITUDE_TARGET';
	public _crc_extra: number = 22;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['q', 'float', false],
		['body_roll_rate', 'float', false],
		['body_pitch_rate', 'float', false],
		['body_yaw_rate', 'float', false],
		['thrust', 'float', false],
		['type_mask', 'uint8_t', false],
	];
}