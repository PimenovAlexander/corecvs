import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavFrame} from '../enums/mav-frame';
import {LandingTargetType} from '../enums/landing-target-type';
/*
The location of a landing target. See: https://mavlink.io/en/services/landing_target.html
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// target_num The ID of the target if multiple targets are present uint8_t
// frame Coordinate frame used for following fields. uint8_t
// angle_x X-axis angular offset of the target from the center of the image float
// angle_y Y-axis angular offset of the target from the center of the image float
// distance Distance to the target from the vehicle float
// size_x Size of target along x-axis float
// size_y Size of target along y-axis float
// x X Position of the landing target in MAV_FRAME float
// y Y Position of the landing target in MAV_FRAME float
// z Z Position of the landing target in MAV_FRAME float
// q Quaternion of landing target orientation (w, x, y, z order, zero-rotation is 1, 0, 0, 0) float
// type Type of landing target uint8_t
// position_valid Boolean indicating whether the position fields (x, y, z, q, type) contain valid target position information (valid: 1, invalid: 0). Default is 0 (invalid). uint8_t
export class LandingTarget extends MAVLinkMessage {
	public time_usec!: number;
	public target_num!: number;
	public frame!: MavFrame;
	public angle_x!: number;
	public angle_y!: number;
	public distance!: number;
	public size_x!: number;
	public size_y!: number;
	public x!: number;
	public y!: number;
	public z!: number;
	public q!: number;
	public type!: LandingTargetType;
	public position_valid!: number;
	public _message_id: number = 149;
	public _message_name: string = 'LANDING_TARGET';
	public _crc_extra: number = 200;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['angle_x', 'float', false],
		['angle_y', 'float', false],
		['distance', 'float', false],
		['size_x', 'float', false],
		['size_y', 'float', false],
		['target_num', 'uint8_t', false],
		['frame', 'uint8_t', false],
		['x', 'float', true],
		['y', 'float', true],
		['z', 'float', true],
		['q', 'float', true],
		['type', 'uint8_t', true],
		['position_valid', 'uint8_t', true],
	];
}