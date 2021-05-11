import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
The attitude in the aeronautical frame (right-handed, Z-down, X-front, Y-right), expressed as quaternion. Quaternion order is w, x, y, z and a zero rotation would be expressed as (1 0 0 0).
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// q Quaternion components, w, x, y, z (1 0 0 0 is the null-rotation) float
// rollspeed Roll angular speed float
// pitchspeed Pitch angular speed float
// yawspeed Yaw angular speed float
// covariance Row-major representation of a 3x3 attitude covariance matrix (states: roll, pitch, yaw; first three entries are the first ROW, next three entries are the second row, etc.). If unknown, assign NaN value to first element in the array. float
export class AttitudeQuaternionCov extends MAVLinkMessage {
	public time_usec!: number;
	public q!: number;
	public rollspeed!: number;
	public pitchspeed!: number;
	public yawspeed!: number;
	public covariance!: number;
	public _message_id: number = 61;
	public _message_name: string = 'ATTITUDE_QUATERNION_COV';
	public _crc_extra: number = 167;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['q', 'float', false],
		['rollspeed', 'float', false],
		['pitchspeed', 'float', false],
		['yawspeed', 'float', false],
		['covariance', 'float', false],
	];
}