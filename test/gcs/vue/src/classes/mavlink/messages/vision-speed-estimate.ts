import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Speed estimate from a vision source.
*/
// usec Timestamp (UNIX time or time since system boot) uint64_t
// x Global X speed float
// y Global Y speed float
// z Global Z speed float
// covariance Row-major representation of 3x3 linear velocity covariance matrix (states: vx, vy, vz; 1st three entries - 1st row, etc.). If unknown, assign NaN value to first element in the array. float
// reset_counter Estimate reset counter. This should be incremented when the estimate resets in any of the dimensions (position, velocity, attitude, angular speed). This is designed to be used when e.g an external SLAM system detects a loop-closure and the estimate jumps. uint8_t
export class VisionSpeedEstimate extends MAVLinkMessage {
	public usec!: number;
	public x!: number;
	public y!: number;
	public z!: number;
	public covariance!: number;
	public reset_counter!: number;
	public _message_id: number = 103;
	public _message_name: string = 'VISION_SPEED_ESTIMATE';
	public _crc_extra: number = 208;
	public _message_fields: [string, string, boolean][] = [
		['usec', 'uint64_t', false],
		['x', 'float', false],
		['y', 'float', false],
		['z', 'float', false],
		['covariance', 'float', true],
		['reset_counter', 'uint8_t', true],
	];
}