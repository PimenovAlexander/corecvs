import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
This message provides an API for manually controlling the vehicle using standard joystick axes nomenclature, along with a joystick-like input device. Unused axes can be disabled an buttons are also transmit as boolean values of their
*/
// target The system to be controlled. uint8_t
// x X-axis, normalized to the range [-1000,1000]. A value of INT16_MAX indicates that this axis is invalid. Generally corresponds to forward(1000)-backward(-1000) movement on a joystick and the pitch of a vehicle. int16_t
// y Y-axis, normalized to the range [-1000,1000]. A value of INT16_MAX indicates that this axis is invalid. Generally corresponds to left(-1000)-right(1000) movement on a joystick and the roll of a vehicle. int16_t
// z Z-axis, normalized to the range [-1000,1000]. A value of INT16_MAX indicates that this axis is invalid. Generally corresponds to a separate slider movement with maximum being 1000 and minimum being -1000 on a joystick and the thrust of a vehicle. Positive values are positive thrust, negative values are negative thrust. int16_t
// r R-axis, normalized to the range [-1000,1000]. A value of INT16_MAX indicates that this axis is invalid. Generally corresponds to a twisting of the joystick, with counter-clockwise being 1000 and clockwise being -1000, and the yaw of a vehicle. int16_t
// buttons A bitfield corresponding to the joystick buttons' current state, 1 for pressed, 0 for released. The lowest bit corresponds to Button 1. uint16_t
export class ManualControl extends MAVLinkMessage {
	public target!: number;
	public x!: number;
	public y!: number;
	public z!: number;
	public r!: number;
	public buttons!: number;
	public _message_id: number = 69;
	public _message_name: string = 'MANUAL_CONTROL';
	public _crc_extra: number = 243;
	public _message_fields: [string, string, boolean][] = [
		['x', 'int16_t', false],
		['y', 'int16_t', false],
		['z', 'int16_t', false],
		['r', 'int16_t', false],
		['buttons', 'uint16_t', false],
		['target', 'uint8_t', false],
	];
}