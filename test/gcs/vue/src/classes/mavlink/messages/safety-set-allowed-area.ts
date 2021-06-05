import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavFrame} from '../enums/mav-frame';
/*
Set a safety zone (volume), which is defined by two corners of a cube. This message can be used to tell the MAV which setpoints/waypoints to accept and which to reject. Safety areas are often enforced by national or competition regulations.
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
// frame Coordinate frame. Can be either global, GPS, right-handed with Z axis up or local, right handed, Z axis down. uint8_t
// p1x x position 1 / Latitude 1 float
// p1y y position 1 / Longitude 1 float
// p1z z position 1 / Altitude 1 float
// p2x x position 2 / Latitude 2 float
// p2y y position 2 / Longitude 2 float
// p2z z position 2 / Altitude 2 float
export class SafetySetAllowedArea extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public frame!: MavFrame;
	public p1x!: number;
	public p1y!: number;
	public p1z!: number;
	public p2x!: number;
	public p2y!: number;
	public p2z!: number;
	public _message_id: number = 54;
	public _message_name: string = 'SAFETY_SET_ALLOWED_AREA';
	public _crc_extra: number = 15;
	public _message_fields: [string, string, boolean][] = [
		['p1x', 'float', false],
		['p1y', 'float', false],
		['p1z', 'float', false],
		['p2x', 'float', false],
		['p2y', 'float', false],
		['p2z', 'float', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['frame', 'uint8_t', false],
	];
}