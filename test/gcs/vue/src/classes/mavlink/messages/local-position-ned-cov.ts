import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavEstimatorType} from '../enums/mav-estimator-type';
/*
The filtered local position (e.g. fused computer vision and accelerometers). Coordinate frame is right-handed, Z-axis down (aeronautical frame, NED / north-east-down convention)
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// estimator_type Class id of the estimator this estimate originated from. uint8_t
// x X Position float
// y Y Position float
// z Z Position float
// vx X Speed float
// vy Y Speed float
// vz Z Speed float
// ax X Acceleration float
// ay Y Acceleration float
// az Z Acceleration float
// covariance Row-major representation of position, velocity and acceleration 9x9 cross-covariance matrix upper right triangle (states: x, y, z, vx, vy, vz, ax, ay, az; first nine entries are the first ROW, next eight entries are the second row, etc.). If unknown, assign NaN value to first element in the array. float
export class LocalPositionNedCov extends MAVLinkMessage {
	public time_usec!: number;
	public estimator_type!: MavEstimatorType;
	public x!: number;
	public y!: number;
	public z!: number;
	public vx!: number;
	public vy!: number;
	public vz!: number;
	public ax!: number;
	public ay!: number;
	public az!: number;
	public covariance!: number;
	public _message_id: number = 64;
	public _message_name: string = 'LOCAL_POSITION_NED_COV';
	public _crc_extra: number = 191;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['x', 'float', false],
		['y', 'float', false],
		['z', 'float', false],
		['vx', 'float', false],
		['vy', 'float', false],
		['vz', 'float', false],
		['ax', 'float', false],
		['ay', 'float', false],
		['az', 'float', false],
		['covariance', 'float', false],
		['estimator_type', 'uint8_t', false],
	];
}