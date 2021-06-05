import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Setpoint in roll, pitch, yaw and thrust from the operator
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// roll Desired roll rate float
// pitch Desired pitch rate float
// yaw Desired yaw rate float
// thrust Collective thrust, normalized to 0 .. 1 float
// mode_switch Flight mode switch position, 0.. 255 uint8_t
// manual_override_switch Override mode switch position, 0.. 255 uint8_t
export class ManualSetpoint extends MAVLinkMessage {
	public time_boot_ms!: number;
	public roll!: number;
	public pitch!: number;
	public yaw!: number;
	public thrust!: number;
	public mode_switch!: number;
	public manual_override_switch!: number;
	public _message_id: number = 81;
	public _message_name: string = 'MANUAL_SETPOINT';
	public _crc_extra: number = 106;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['roll', 'float', false],
		['pitch', 'float', false],
		['yaw', 'float', false],
		['thrust', 'float', false],
		['mode_switch', 'uint8_t', false],
		['manual_override_switch', 'uint8_t', false],
	];
}