import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Current motion information from a designated system
*/
// timestamp Timestamp (time since system boot). uint64_t
// est_capabilities bit positions for tracker reporting capabilities (POS = 0, VEL = 1, ACCEL = 2, ATT + RATES = 3) uint8_t
// lat Latitude (WGS84) int32_t
// lon Longitude (WGS84) int32_t
// alt Altitude (MSL) float
// vel target velocity (0,0,0) for unknown float
// acc linear target acceleration (0,0,0) for unknown float
// attitude_q (1 0 0 0 for unknown) float
// rates (0 0 0 for unknown) float
// position_cov eph epv float
// custom_state button states or switches of a tracker device uint64_t
export class FollowTarget extends MAVLinkMessage {
	public timestamp!: number;
	public est_capabilities!: number;
	public lat!: number;
	public lon!: number;
	public alt!: number;
	public vel!: number;
	public acc!: number;
	public attitude_q!: number;
	public rates!: number;
	public position_cov!: number;
	public custom_state!: number;
	public _message_id: number = 144;
	public _message_name: string = 'FOLLOW_TARGET';
	public _crc_extra: number = 127;
	public _message_fields: [string, string, boolean][] = [
		['timestamp', 'uint64_t', false],
		['custom_state', 'uint64_t', false],
		['lat', 'int32_t', false],
		['lon', 'int32_t', false],
		['alt', 'float', false],
		['vel', 'float', false],
		['acc', 'float', false],
		['attitude_q', 'float', false],
		['rates', 'float', false],
		['position_cov', 'float', false],
		['est_capabilities', 'uint8_t', false],
	];
}