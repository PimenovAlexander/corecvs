import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
The state of the fixed wing navigation and position controller.
*/
// nav_roll Current desired roll float
// nav_pitch Current desired pitch float
// nav_bearing Current desired heading int16_t
// target_bearing Bearing to current waypoint/target int16_t
// wp_dist Distance to active waypoint uint16_t
// alt_error Current altitude error float
// aspd_error Current airspeed error float
// xtrack_error Current crosstrack error on x-y plane float
export class NavControllerOutput extends MAVLinkMessage {
	public nav_roll!: number;
	public nav_pitch!: number;
	public nav_bearing!: number;
	public target_bearing!: number;
	public wp_dist!: number;
	public alt_error!: number;
	public aspd_error!: number;
	public xtrack_error!: number;
	public _message_id: number = 62;
	public _message_name: string = 'NAV_CONTROLLER_OUTPUT';
	public _crc_extra: number = 183;
	public _message_fields: [string, string, boolean][] = [
		['nav_roll', 'float', false],
		['nav_pitch', 'float', false],
		['alt_error', 'float', false],
		['aspd_error', 'float', false],
		['xtrack_error', 'float', false],
		['nav_bearing', 'int16_t', false],
		['target_bearing', 'int16_t', false],
		['wp_dist', 'uint16_t', false],
	];
}